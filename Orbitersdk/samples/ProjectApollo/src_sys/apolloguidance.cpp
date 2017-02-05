/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Basic Apollo Guidance computer setup

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************************************************************/


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "inttypes.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "yaAGC/agc_engine.h"
#include "ioChannels.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "IMU.h"
#include "powersource.h"
#include "papi.h"

#include "tracer.h"

char TwoSpaceTwoFormat[7] = "XXX XX";
char RegFormat[7] = "XXXXXX";

// Moved DELTAT definition to avoid INTERNAL COMPILER ERROR
#define DELTAT 2.0

ApolloGuidance::ApolloGuidance(SoundLib &s, DSKY &display, IMU &im, PanelSDK &p) : soundlib(s), dsky(display), imu(im), DCPower(0, p)

{
	ProgRunning = VerbRunning = NounRunning = 0;
	ProgState = 0;
	Standby = false;

	MaxThrust = 0;
	VesselISP = 1000000;

	Reset = false;
	CurrentTimestep = 0;
	LastTimestep = 0;
	LastCycled = 0;
	LastEventTime = 0.0;
	InOrbit = false;

	LastVerb16Time = 0;

	BurnTime = 0;
	BurnStartTime = 0;
	BurnEndTime = 0;
	CutOffVel = 0;
	NextEventTime = 0;
	NextControlTime = 0;
	BankSumNum = 1;

	//
	// Default to C++ AGC.
	//

	Yaagc = false;

	//
	// Target attitude.
	//

	TargetPitch = 0;
	TargetRoll = 0;
	TargetYaw = 0;

	DesiredApogee = 0.0;
	DesiredPerigee = 0.0;
	DesiredDeltaV = 0.0;
	DesiredAzimuth = 0.0;
	DesiredInclination = 0.0;
	DesiredDeltaVx = 0.0;
	DesiredDeltaVy = 0.0;
	DesiredDeltaVz = 0.0;
	DesiredPlaneChange = 0.0;
	DesiredLAN = 0.0;

	DeltaPitchRate = 0.0;

	CurrentRoll = 0.0;
	CurrentPitch = 0.0;
	CurrentYaw = 0.0;

	LandingLongitude = 0.0;
	LandingLatitude = 0.0;
	LandingAltitude = 0.0;

	//
	// Expected dV from thrust decay of engine.
	//

	ThrustDecayDV = 0.0;

	//
	// Alarm codes.
	//

	Alarm01 = 0;
	Alarm02 = 0;
	Alarm03 = 0;

	//
	// Flight number.
	//

	ApolloNo = 0;

	Realism = REALISM_DEFAULT;

	OtherVesselName[0] = 0;

	//
	// Default to meters per second for
	// velocity.
	//

	DisplayUnits = UnitMetric;

	//
	// 10ms timer.
	//

	TIME1 = 0;

	//
	// Clear channels.
	//

	int i;

	for (i = 0; i <= MAX_OUTPUT_CHANNELS; i++)
		OutputChannel[i] = 0;
	for (i = 0; i <= MAX_INPUT_CHANNELS; i++)
		InputChannel[i] = 0;

	Chan10Flags = 0;

	//
	// Dsky interface.
	//

	dsky2 = NULL;

	Prog = 0;
	Verb = 0;
	Noun = 0;

	R1 = 0;
	R2 = 0;
	R3 = 0;

	EnteringVerb = false;
	EnteringNoun = false;
	EnteringData = 0;
	EnterPos = 0;
	EnterCount = 0;
	EnteringOctal = false;
	EnterPositive = true;

	R1Decimal = true;
	R2Decimal = true;
	R3Decimal = true;

	ProgBlanked = false;
	VerbBlanked = false;
	NounBlanked = false;
	R1Blanked = false;
	R2Blanked = false;
	R3Blanked = false;
	KbInUse = false;

	SetR1Format(RegFormat);
	SetR2Format(RegFormat);
	SetR3Format(RegFormat);

	strncpy (TwoDigitEntry, "  ", 2);
	strncpy (FiveDigitEntry, "      ", 6);

	LastAlt = 0.0;

	isFirstTimestep = true;
	PadLoaded = false;

	ProgAlarm = false;
	GimbalLockAlarm = false;

	//
	// Virtual AGC.
	//
	memset(&vagc, 0, sizeof(vagc));
	vagc.agc_clientdata = this;
	agc_engine_init(&vagc, NULL, NULL, 0);

#ifdef _DEBUG
	out_file = fopen("ProjectApollo AGC.log", "wt");
	vagc.out_file = out_file;
#endif

	PowerConnected = false;
}

ApolloGuidance::~ApolloGuidance()

{
#ifdef _DEBUG
	fclose(out_file);
#endif
}

void ApolloGuidance::InitVirtualAGC(char *binfile)

{

	(void) agc_load_binfile(&vagc, binfile);

	// Set channels only once, otherwise this code overwrites the channel values in the scenario
	if (!PadLoaded) { 
		ChannelValue val30;
		ChannelValue val31;
		ChannelValue val32;
		ChannelValue val33;

		//
		// Set default state. Note that these are oddities, as zero means
		// true and one means false for the 'Virtual AGC'!
		//

		val30 = 077777;
		// Enable to turn on
		// val30.Bits.IMUOperate = 0;
		val30.reset(TempInLimits);

		//
		// We default to the IMU turned off. If you change this, change the IMU code to
		// match.
		//
		val30.set(IMUOperate);

		vagc.InputChannel[030] = (int16_t)val30.to_ulong();
		InputChannel[030] = (val30.to_ulong() ^ 077777);

		val31 = 077777;
		// Default position of the CMC MODE switch is FREE
		val31[FreeFunction] = 0;

		vagc.InputChannel[031] = (int16_t)val31.to_ulong();
		InputChannel[031] = (val31.to_ulong() ^ 077777);


		val32 = 077777;
		vagc.InputChannel[032] = (int16_t)val32.to_ulong();
		InputChannel[032] = (val32.to_ulong() ^ 077777);

		val33 = 077777;
	//	val33.Bits.RangeUnitDataGood = 0;
	//	val33.Bits.BlockUplinkInput = 0;

		//
		// Setting AGCWarning is needed to avoid the 1107 alarm on startup. It basically forces the AGC to do a
		// hard reset in the RESTART code.
		//
		//	CA	BIT14			# IF AGC WARNING ON (BIT = 0), DO A FRESH
		//	EXTEND				# START ON THE ASSUMPTION THAT WE'RE IN A
		//	RAND	CHAN33		# RESTART LOOP.
		//

		val33[AGCWarning] = 0;
		
		vagc.InputChannel[033] = (int16_t)val33.to_ulong();
		InputChannel[033] = (val33.to_ulong() ^ 077777);
	}
}

//
// Start the computer up from standby.
//

void ApolloGuidance::Startup()

{
	if (!Standby)
		return;

	Standby = false;
	dsky.ClearStby();
	if (dsky2) dsky2->ClearStby();

	//
	// Light NO ATT if the IMU isn't running.
	//

	ChannelValue val30;

	val30 = GetInputChannel(030);
	if (val30[IMUCage] || !val30[IMUOperate]) {
		LightNoAtt();
	}

	//
	// We always start up in program zero.
	//

	RunProgram(0);

	//
	// Indicate to user that they should change
	// programs as appropriate.
	//

	NounRunning = 37;	// Change program.

	if (KBCheck()) {
		BlankData();
		SetVerb(37);
		SetVerbNounFlashing();
	}
}

//
// Force a hardware restart.
//

void ApolloGuidance::ForceRestart()

{
	Reset = false;
	ProgState = 0;
}

//
// Go to standby.
//

void ApolloGuidance::GoStandby()

{
	BlankAll();
	ClearVerbNounFlashing();
	dsky.LightStby();
	if (dsky2) dsky2->LightStby();
	Standby = true;
}

bool ApolloGuidance::OutOfReset()

{
	return (Yaagc || Reset);
}

//
// Convert velocities to and from feet per second as
// appropriate. Internally we always use meters per
// second, but displays can use either.
//

double ApolloGuidance::DisplayVel(double vel)

{
	if (DisplayUnits == UnitMetric)
		return vel;

	return (vel * (1 / 0.3048));
}

double ApolloGuidance::GetVel(double vel)

{
	if (DisplayUnits == UnitMetric)
		return vel;

	return (vel * 0.3048);
}

//
// Convert altitude in km to nautical miles if
// appropriate.
//

double ApolloGuidance::DisplayAlt(double alt)

{
	if (DisplayUnits == UnitMetric)
		return alt;

	return (alt * 0.539956803);
}

bool ApolloGuidance::ValidateCommonProgram(int prog)

{
	switch (prog) {

		//
		// 00: idle
		//

	case 00:
		return true;

	//
	// 16: LOI
	//

	case 16:
		return true;

	//
	// 17: DOI for Per / landing site alignment after n passes
	//

	case 17:
		return true;

	//
	// 18: Plane Change for landing site alignment after n passes
	//

	case 18:
		return true;

	//
	// 19: Orbit adjustment
	//

	case 19:
		return true;

	//
	// 37: orbit altitude adjustment.
	//

	case 37:
//		if (!InOrbit)
//			return false;

		return true;

	}

	return false;
}

bool ApolloGuidance::ValidateCommonVerbNoun(int verb, int noun)

{
	switch (verb) {

	//
	// Data display.
	//

	case 3:
	case 4:
	case 5:
	case 6:
	case 16:
		return true;

	//
	// EMEM display.
	//

	case 1:
	case 11:
	case 21:
		if (noun == 2)
			return true;
		return false;

	//
	// 37: change program.
	//

	case 37:
		if (ValidateCommonProgram(noun))
			return true;
		if (ValidateProgram(noun))
			return true;

		return false;

	//
	// 69: cause restart.
	//

	case 69:
		return true;

	//
	// 91: display memory bank checksum.
	//

	case 91:
		return true;

	}

	return false;
}

void ApolloGuidance::ProcessCommonVerbNoun(int verb, int noun)

{
	switch (verb) {

	case 11:
		DisplayEMEM(CurrentEMEMAddr);
		break;

	//
	// 16: monitor display.
	//

	case 16:
		LastVerb16Time = LastTimestep;

	//
	// 03 - 05: octal display data
	// 06: decimal display data.
	//

	case 3:
	case 4:
	case 5:
	case 6:

		//
		// First blank all for safety.
		//

		BlankData();

		//
		// Then display the approprirate data.
		//

		DisplayNounData(noun);

		//
		// Set octal mode if required.
		//

		switch (verb) {
		case 5:
			DisplayR3Octal();
		case 4:
			DisplayR2Octal();
		case 3:
			DisplayR1Octal();
			break;
		}
		break;

	//
	// 37: run a program.
	//

	case 37:
		RunProgram(noun);
		break;

	//
	// 69: cause restart.
	//

	case 69:
		Reset = false;
		BlankAll();
		dsky.LightRestart();
		if (dsky2) dsky2->LightRestart();
		RunProgram(0);
		break;

	//
	// 91: display bank checksum.
	//

	case 91:
		BankSumNum = 0;
		BlankData();
		DisplayBankSum();
		break;

	}
}

//
// Fake up bank checksums.
//

void ApolloGuidance::DisplayBankSum()

{
	if (BankSumNum > 8)
		BankSumNum = 0;

	if (BankSumNum & 1)
		SetR1Octal(BankSumNum);
	else
		SetR1Octal(077777 -BankSumNum);

	SetR2Octal(BankSumNum);
	SetR3Octal((BankSumNum * 10000 + 4223) & 077777);
}

bool ApolloGuidance::CommonProceedNoData()

{
	switch (VerbRunning) {
	case 91:
		BankSumNum++;
		DisplayBankSum();
		return true;
	}

	return false;
}

//
// Terminate a program.
//

void ApolloGuidance::TerminateCommonProgram()

{
	switch (ProgRunning)
	{

	//
	// 37: if the engine is on, stop it.
	//

	case 37:
		if (BurnFlag)
			BurnMainEngine(0);
		break;
	}
}

void ApolloGuidance::DisplayOrbitCalculations()

{
	ELEMENTS el;
	double mjd_ref, rad;
	OBJHANDLE hPlanet;

	hPlanet = OurVessel->GetGravityRef();
	rad = oapiGetSize(hPlanet);

	OurVessel->GetElements(el, mjd_ref);

	double apogee = (el.a * (1.0 + el.e)) - rad;
	double perigee = (el.a * (1.0 - el.e)) - rad;

	if (apogee < 0)
		apogee = 0;

	SetR1((int)DisplayAlt(apogee) / 1000);
	SetR2((int)DisplayAlt(perigee) / 1000);
}

//
// Display a time in standard format.
//

void ApolloGuidance::DisplayTime(double t)

{
	if (t < 0)
		t = 0;

	int h = ((int)t) / 3600;
	int m = ((int)t - (3600 * h)) / 60;
	int s = ((int)(t * 100)) % 6000;

	SetR1(h);
	SetR2(m);
	SetR3(s);
}

bool ApolloGuidance::DisplayCommonNounData(int noun)

{
	switch (noun) {

	//
	// 09: program alarms.
	//

	case 9:
		SetR1Octal(Alarm01);
		SetR2Octal(Alarm02);
		SetR3Octal(Alarm03);
		return true;

	//
	// 16: desired attitude.
	//

	case 16:
		SetR1((int)(TargetRoll * 100));
		SetR2((int)(TargetPitch * 100));
		SetR3((int)(TargetYaw * 100));
		return true;

	//
	// 17: Current attitude w.r.t equatorial plane
	//

	case 17:
		double bank, pitch, hdg;

		oapiGetFocusHeading(&hdg);
		bank  = OurVessel->GetBank();
		pitch = OurVessel->GetPitch();		
		bank  *= DEG;
		pitch *= DEG;
		hdg   *= DEG;

		SetR1((int)(bank * 100));
		SetR2((int)(pitch * 100));
		SetR3((int)(hdg * 100));
		return true;

	//
	// 19: time to burn, altitude, crossrange
	//

	case 19:
		{
			int min, sec;
			double dt, alt;

			// time from ignition
			dt=CurrentTimestep-BurnStartTime;
			min = (int) (dt / 60.0);
			sec = ((int) dt) - (min * 60);
			if (min > 99 || min < -99) {
				SetR1( (int) dt);
			} else {
				SetR1(min * 1000 + sec);
				SetR1Format("XXX XX");
			}
			alt = OurVessel->GetAltitude();
			SetR2((int) (DisplayAlt( alt )/100.0));
			SetR3((int) (DeltaPitchRate/100.0));
		}
		return true;


	//
	// 23: number of passes, mm ss to burn, offplane distance (fictitious)
	//

	case 23:
		{
			int min, sec;
			double dt;

			SetR1((int) (DeltaPitchRate));

			// time from ignition
			dt=CurrentTimestep-BurnStartTime;
			min = (int) (dt / 60.0);
			sec = ((int) dt) - (min * 60);
			if (min > 99 || min < -99) {
				SetR2( (int) dt);
			} else {
				SetR2(min * 1000 + sec);
				SetR2Format("XXX XX");
			}

			SetR3((int) (DesiredDeltaV/100.0 ));
		}
		return true;

	//
	// 28: Current attitude w.r.t airspeed vector
	//

	case 28:
		double alpha, slip;

		alpha = OurVessel->GetAOA() * DEG;
		slip  = OurVessel->GetSlipAngle() * DEG;
		
		SetR1((int)(alpha * 100));
		SetR2((int)(slip * 100));
		BlankR3();
		return true;

	//
	// 29: launch azimuth.
	//

	case 29:
		SetR1((int)(DesiredAzimuth * 100));
		return true;

	//
	// 30: Apoapsis, periapsis, phase angle
	//

	case 30:
		SetR1((int)(DesiredApogee / 100.0));
		SetR2((int)(DesiredPerigee / 100.0));
		SetR3((int)(DesiredAzimuth * 100.0));
		return true;

	//
	// 32: Time from periapsis
	//

	case 32:
		{
			OBJHANDLE gBody;
			ELEMENTS Elements;
			VECTOR3 Rp;
			VECTOR3 Rv;
			double GMass, GSize, Mu, mjd_ref, Me;
			double E, T, tsp, RDotV, R, p, v, apd;		
			int tsph, tspm, tsps;
			
			gBody = OurVessel->GetApDist(apd);
			GMass = oapiGetMass(gBody);
			GSize = oapiGetSize(gBody) / 1000;
			Mu    = GK * GMass;	
			
			OurVessel->GetElements(Elements, mjd_ref);
			OurVessel->GetRelativePos(gBody, Rp);
			OurVessel->GetRelativeVel(gBody, Rv);	
			R = sqrt(pow(Rp.x,2) + pow(Rp.y,2) + pow(Rp.z,2)) / 1000;		
			p = Elements.a / 1000 * (1 - Elements.e * Elements.e);
			v = acos((1 / Elements.e) * (p / R - 1));
		
			RDotV = dotp(Rv, Rp);
			if (RDotV < 0) 
			{
				v = 2 * PI - v;
			}				
			
			E   = 2 * atan(sqrt((1 - Elements.e)/(1 + Elements.e)) * tan(v / 2));//		Ecc anomaly
			Me  = E - Elements.e * sin(E);										 //		Mean anomaly
			T   = 2 * PI * sqrt((pow(Elements.a,3) / 1e9) / Mu);			     //		Orbit period			
			tsp = Me / (2 * PI) * T;											 //		Time from ped

			tsph = ((int)tsp) / 3600;
			tspm = ((int)tsp - (3600 * tsph)) / 60;
			tsps = ((int)(tsp * 100)) % 6000;

			SetR1(tsph);
			SetR2(tspm);
			SetR3(tsps);
		}
		return true;


	//
	// 33: time to burn.
	//

	case 33:
		{
//			double Met;
			double TimeToBurn = 0.0;
//			LEM *lem = (LEM *) OurVessel;
//			lem->GetMissionTime(Met);

			if (BurnTime > LastTimestep) {
				TimeToBurn = BurnTime - LastTimestep;
			}

			int h = ((int)TimeToBurn) / 3600;
			int m = ((int)TimeToBurn - (3600 * h)) / 60;
			int s = ((int)(TimeToBurn * 100)) % 6000;

			SetR1(h);
			SetR2(m);
			SetR3(s);
		}
		return true;

	//
	// 34: time of next event.
	//

	case 34:
		DisplayTime(NextEventTime);
		return true;

	//
	// 35: time to next event.
	//

	case 35:
		DisplayTime(NextEventTime - LastTimestep);
		return true;

	//
	// 38: time since startup. 65 seems to be basically the same.
	//

	case 38:
	case 65:

		//
		// If we haven't started, then blank it.
		//

		if (!Reset) {
			BlankData();
			return true;
		}

		DisplayTime(LastTimestep - ResetTime);
		return true;

	//
	// 42: Apocenter, Pericenter, Accumulated DeltaV
	//

	case 42:
		{
			OBJHANDLE gBody;
			double apd, ped, dv, GSize;

			gBody = OurVessel->GetApDist(apd);			
			GSize = oapiGetSize(gBody);
			
			OurVessel->GetPeDist(ped);

			apd -= GSize;
			ped -= GSize;

			dv = sqrt(pow(DesiredDeltaVx,2) + pow(DesiredDeltaVy,2) + pow(DesiredDeltaVz,2));

			SetR1((int)( DisplayAlt( apd ) / 10));
			SetR2((int)( DisplayAlt( ped ) / 10));
			SetR3((int)dv);
		}
		return true;


	//
	// 43: latitude, longitude, altitude.
	//

	case 43:
		double latitude, longitude, radius;

		OurVessel->GetEquPos(longitude, latitude, radius);
		CurrentAlt = OurVessel->GetAltitude();

		//
		// Convert position to degrees.
		//

		longitude *= DEG;
		latitude *= DEG;

		//
		// And display it.
		//

		SetR1((int)(latitude * 100.0));
		SetR2((int)(longitude * 100.0));
		SetR3((int)DisplayAlt(CurrentAlt * 0.01));

		return true;

	//
	// 44: Orbit parameters. Only available when the AGC is theoretically
	//	   calculating orbit parameters, otherwise request desired values.
	//

	case 44:
		if (OrbitCalculationsValid())
		{
			DisplayOrbitCalculations();
			SetR3((int)DisplayAlt(OurVessel->GetAltitude()) / 1000);
		}
		else if (ProgRunning == 37) {
			SetR1((int)(DisplayAlt( DesiredApogee ) * 10.0));
			SetR2((int)(DisplayAlt( DesiredPerigee ) * 10.0));
		}
		else {
			SetR1((int)(DisplayAlt( DesiredApogee ) * 100.0));
			SetR2((int)(DisplayAlt( DesiredPerigee ) * 100.0));
			if (ProgRunning == 10) {
				SetR3((int)(DesiredAzimuth * 100.0));
			}
		}
		return true;

	//
	// 46: Target DeltaV, DeltaV so far, Time to burn (used by P37)
	//

	case 46:
		{
			double dv, dt;

			dv = sqrt(pow(DesiredDeltaVx,2) + pow(DesiredDeltaVy,2) + pow(DesiredDeltaVz,2));			

			dt   = BurnStartTime - CurrentTimestep;			
			
			int min = (int) (dt / 60.0);
			int	sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			SetR1((int) DisplayVel( dv ));
			SetR2((int)DisplayVel( DesiredDeltaV ));
			SetR3(min * 1000 + sec);
			SetR3Format("XXX XX");
		}			
		return true;


	case 50:
		{
			int min, sec;
			double dt, dv, bt;

			// time from ignition
			dt=CurrentTimestep-BurnStartTime;
			min = (int) (dt / 60.0);
			sec = ((int) dt) - (min * 60);
			if (min > 99 || min < -99) {
				SetR1( (int) dt);
			} else {
				SetR1(min * 1000 + sec);
				SetR1Format("XXX XX");
			}
			dv=CutOffVel;
            bt=BurnEndTime-BurnStartTime;
			if(dt > 0.0) {
				dv=(CutOffVel*(BurnEndTime-CurrentTimestep))/bt;
				bt=BurnEndTime-CurrentTimestep;
			}
			SetR2((int) (dv*10.0));
			SetR3((int) (bt*10.0));
		}
		return true;


	//
	// 73: altitude, velocity, AoA
	//

	case 73:
		SetR1((int)(DisplayAlt(CurrentAlt) / 1000));
		SetR2((int)DisplayVel(CurrentVel));
		SetR3((int)(OurVessel->GetAOA() * 5729.57795));
		return true;

	//
	// 81: Desired DeltaV components in Local Vertical (LV) coordinates (X left, Y up, Z front)
	//

	case 81:
		SetR1((int)DesiredDeltaVx);
		SetR2((int)DesiredDeltaVy);
		SetR3((int)DesiredDeltaVz);
		return true;

	//
	// 82: Desired DeltaV in horizon coordinates
	//

	case 82:
		SetR1((int)(DesiredDeltaVx));
		SetR2((int)(DesiredDeltaVy));
		SetR3((int)(DesiredDeltaVz));
		return true; 

	//
	// 95: time to ignition, expected deltav and accumulated deltav
	//

	case 95:
		double IgnitionTime = BurnStartTime - LastTimestep;
		double R2 = DesiredDeltaV;
		double R3 = CutOffVel;

		if (BurnStartTime <= CurrentTimestep) {
			IgnitionTime = BurnEndTime - CurrentTimestep;
			R2 = CutOffVel - CurrentRVel;
		}

		int	Min = (int)(IgnitionTime / 60);
		int Sec = ((int)IgnitionTime) % 60;

		if (Min < 0)
			Min = 0;
		if (Sec < 0)
			Sec = 0;

		SetR1(Min * 1000 + Sec);
		SetR1Format(TwoSpaceTwoFormat);
		SetR2((int)DisplayVel(R2));
		SetR3((int)DisplayVel(R3));
		return true;
	}

	return false;
}



// Do a single timestep - Used by CM to maintain sync between telemetry and vAGC.
bool ApolloGuidance::SingleTimestepPrep(double simt, double simdt){
	LastTimestep = CurrentTimestep;
	CurrentTimestep = simt;
	TIME1 += (int)((simt - LastTimestep) * 1600.0);

	// Get position and velocity data since it's generally useful.
	GetPosVel();
	return TRUE;
}

bool ApolloGuidance::SingleTimestep() {

	agc_engine(&vagc);
	return TRUE;
}

void ApolloGuidance::VirtualAGCCoreDump(char *fileName) {

	MakeCoreDump(&vagc, fileName); 
}

bool ApolloGuidance::GenericTimestep(double simt, double simdt)
{
//	TRACESETUP("COMPUTER TIMESTEP");
	int i;

	LastTimestep = CurrentTimestep;
	CurrentTimestep = simt;

	TIME1 += (int)((simt - LastTimestep) * 1600.0);

	//
	// Get position and velocity data since it's generally useful.
	//

	GetPosVel();

	if (Yaagc) {
		// Physical AGC timing was generated from a master 1024 KHz clock, divided by 12.
		// This resulted in a machine cycle of just over 11.7 microseconds.
		int cycles = (long) ((simdt) * 1024000 / 12);

		for (i = 0; i < cycles; i++) {
			agc_engine(&vagc);
		}

		return true;
	}

	//
	// Now we know this isn't a Virtual AGC, run the C++ AGC code.
	//

	//
	// Clear the COMP ACTY light. That way any code can set it while processing, and
	// it will automatically be reset on the next timestep.
	//

	ClearCompActy();

	if (Standby)
		return true;

	if (!Reset) {
		ResetProg(simt);
		return true;
	}

	switch (VerbRunning) {

	//
	// Verb 11 monitors an EMEM location.
	//

	case 11:
		DisplayEMEM(CurrentEMEMAddr);
		break;

	//
	// Verb 16 updates data roughly twice a second.
	//

	case 16:
		if (simt > (LastVerb16Time + 0.5)) {
			LightCompActy();
			DisplayNounData(NounRunning);
			LastVerb16Time = simt;
		}
		break;
	}

	switch (ProgRunning)
	{

	//
	// 16: LOI
	//

	case 16:
		Prog16(simt);
		break;

	//
	// 17: DOI
	//

	case 17:
		Prog17(simt);
		break;

	//
	// 18: Plane Change
	//

	case 18:
		Prog18(simt);
		break;
	//
	// 19: Orbit altitude and phase adjust
	//

	case 19:
		Prog19(simt);
		break;

	//
	// 37: orbit altitude adjustment.
	//

	case 37:
		Prog37(simt);
		break;
	}

	return false;
}

void ApolloGuidance::SystemTimestep(double simdt) 

{
	if (!IsPowered()) return;

	if (OnStandby()) {
		DCPower.DrawPower(22.9);
	}
	else {
		DCPower.DrawPower(106.0);
	}
}

//
// Start the specified program running.
//

void ApolloGuidance::RunProgram(int prog)

{
	ProgRunning = prog;
	ProgState = 0;
	LastProgTime = LastTimestep;
	SetProg(prog);

	if (prog == 0) {
		BlankData();
	}
}

void ApolloGuidance::VerbNounEntered(int verb, int noun)

{
	//
	// Validate this verb and noun combination.
	//

	if (ValidateCommonVerbNoun(verb, noun)) {
		VerbRunning = verb;
		NounRunning = noun;
		ProcessCommonVerbNoun(verb, noun);
		return;
	}

	if (ValidateVerbNoun(verb, noun)) {
		VerbRunning = verb;
		NounRunning = noun;
		ProcessVerbNoun(verb, noun);
		return;
	}

	//
	// If it didn't validate, then light the Opr Error
	// light.
	//

	if (verb != 21)
		LightOprErr();
}

void ApolloGuidance::SetMissionInfo(int MissionNo, int RealismValue, char *OtherName) 

{
	Realism = RealismValue;

	//
	// Older scenarios saved the mission number in the AGC. For backwards
	// compatibility we'll only let the new number overwrite the saved value
	// if it's zero.
	//

	if (!ApolloNo)
		ApolloNo = MissionNo; 

	if (OtherName != 0)
		strncpy(OtherVesselName, OtherName, 64);
}


//
// Run the reset program on startup.
//

void ApolloGuidance::ResetCountdown()

{
	int Val2 = 11 * ResetCount;
	int	Val5 = 11111 * ResetCount;

	SetProg(Val2);
	SetNoun(Val2);
	SetVerb(Val2);

	SetR1(Val5);
	SetR2(Val5);
	SetR3(Val5);

	if (ResetCount > 0) {
		ResetCount--;
	}
}

void ApolloGuidance::SetVerbNounAndFlash(int Verb, int Noun)

{
	VerbRunning = Verb;
	NounRunning = Noun;

	BlankData();
	SetVerb(VerbRunning);
	SetNoun(NounRunning);
	SetVerbNounFlashing();

	DisplayNounData(NounRunning);
}


void ApolloGuidance::SetVerbNoun(int Verb, int Noun)

{
	VerbRunning = Verb;
	NounRunning = Noun;

	BlankData();
	SetVerb(VerbRunning);
	SetNoun(NounRunning);
	ClearVerbNounFlashing();

	DisplayNounData(NounRunning);
}

//
// Most of this burn calculation code is lifted from the Soyuz guidance MFD.
//

// Returns the absolute value of a vector
double AbsOfVector(const VECTOR3 &Vec)
{
	double Result;
	Result = sqrt(Vec.x*Vec.x + Vec.y*Vec.y + Vec.z*Vec.z);
	return Result;
}

// Returns a Velocity at Periapsis or Apoapsis for a projected orbit

inline double ApolloGuidance::NewVelocity_AorP(double Mu_Planet, double &Rapo, double &Rperi, double &Rnew)
{
	double a;
	double e;
	double h;
	double Vnew;

	a = (Rapo + Rperi)/2;
	e = (Rapo / a) - 1;
	h = sqrt(Mu_Planet * a * (1 - e*e));
	Vnew = h / Rnew;
	return Vnew;
}

//
// Calculate the orbit correction burns.
//
// Flag01 == raising apogee (0) or perigee (1)
// Flag02 == backup of Flag01 from phase one burn
//

void ApolloGuidance::DoOrbitBurnCalcs(double simt)

{
	OBJHANDLE hSetGbody;
	double GbodyMass, GbodySize, pcthrust;
	double p, v, R, RDotV, Mu_Planet, J2000, E, Me, T, tsp;
	double TtPeri, TtApo;
	double OrbitApo, OrbitPeri, AnsOne, AnsTwo, Orbit1StaticR;
	double VnewBurn1, VesselMass;
	VECTOR3 RelPosition, RelVelocity;
	ELEMENTS Elements;

	LightCompActy();

	// Planet parameters
	hSetGbody = OurVessel->GetApDist(OrbitApo);
	GbodyMass = oapiGetMass(hSetGbody);
	GbodySize = oapiGetSize(hSetGbody) / 1000;
	Mu_Planet = GK * GbodyMass;

	// Get eccentricity and orbital radius
	OurVessel->GetElements(Elements, J2000);
	OurVessel->GetRelativePos(hSetGbody, RelPosition);
	OurVessel->GetRelativeVel(hSetGbody, RelVelocity);

	R = AbsOfVector(RelPosition) / 1000;

	// Calculate semi-latus rectum and true anomaly
	p = Elements.a/1000 *(1 - Elements.e*Elements.e);
	v = acos((1/Elements.e)*(p/R - 1));

	RDotV = dotp(RelVelocity, RelPosition);
	if (RDotV < 0)
	{
		v = 2*PI - v;
	}

	// Determine the time since periapsis
	//   - Eccentric anomaly
	E = 2 * atan(sqrt((1-Elements.e)/(1+Elements.e))*tan(v/2));
	//   - Mean anomaly
	Me = E - Elements.e*sin(E);
	//   - Period of orbit
	T = 2*PI*sqrt((Elements.a*Elements.a*Elements.a/1e9)/Mu_Planet);

	// Time since periapsis is
	tsp = Me/(2*PI)*T;

	// Time to next periapsis & apoapsis
	TtPeri = T - tsp;
	if (RDotV < 0) {
		TtPeri = -1 * tsp;
	}

	if (TtPeri > (T / 2)) {
		TtApo = fabs((T/2) - TtPeri);
	}
	else {
		TtApo = fabs(TtPeri + (T/2));
	}

	//
	// On the second burn we must ensure we change the opposite side of the
	// orbit to the first. This check gets around some weird special cases
	// where the first burn lowers the apogee to be less than the old perigee,
	// so the code would otherwise end up seeing the old perigee as the apogee
	// and putting us into a circular orbit.
	//

	if (ProgState > 50) {
		ProgFlag01 = !ProgFlag02;
	}
	else {
		if (TtApo < TtPeri) {
			ProgFlag01 = 1;
		}
		else {
			ProgFlag01 = 0;
		}
		ProgFlag02 = ProgFlag01;
	}

	if (ProgFlag01) {
		OrbitPeri = DesiredPerigee + GbodySize;
		OrbitApo = Elements.a*(1+Elements.e) / 1000;
		Orbit1StaticR = OrbitApo;
	}
	else {
		OrbitApo = DesiredApogee + GbodySize;
		OrbitPeri = Elements.a*(1-Elements.e) / 1000;
		Orbit1StaticR = OrbitPeri;
	}

	// Calculate new velocities at Apo and Peri.
	VnewBurn1 = NewVelocity_AorP(Mu_Planet, OrbitPeri, OrbitApo, Orbit1StaticR);
	AnsOne = sqrt(Mu_Planet * (Elements.a/1000) * (1 - Elements.e*Elements.e)); // h
	AnsTwo = AnsOne / Orbit1StaticR; // h / (R static)
	DesiredDeltaV = (VnewBurn1 - AnsTwo) * 1000;
	CutOffVel = VnewBurn1 * 1000;

	// Burn times
	VesselMass = OurVessel->GetMass();
	pcthrust=1.0;
	if(MainThrusterIsHover) pcthrust=0.1;

	double massrequired = VesselMass * (1 - exp(-(fabs(DesiredDeltaV) / VesselISP)));
	double deltaT = massrequired / (MaxThrust*pcthrust / VesselISP);

	if (TtApo < TtPeri) {
		BurnTime = simt + TtApo;
	}
	else {
		BurnTime = simt + TtPeri;
	}

	BurnStartTime = BurnTime - (deltaT / 2);
	BurnEndTime = BurnStartTime + deltaT;

	if (DesiredDeltaV < 0)
	{
		// Retrograde
		RetroFlag = 1;
	}
	else
	{
		// Prograde
		RetroFlag = 0;
	}
}

void ApolloGuidance::OrientForOrbitBurn(double simt)

{
	if (RetroFlag) {
		if (MainThrusterIsHover) {
			if(simt > NextEventTime) {
				NextEventTime=simt+1.0;
				VECTOR3 actatt;
				GetHoverAttitude(actatt);
				VECTOR3 tgtatt=_V(-PI/2.0, 0.0, 0.0);
				ComAttitude(actatt, tgtatt, false);
			}
		}
		else {
			OurVessel->ActivateNavmode(NAVMODE_RETROGRADE);
		}
	}
	else {
		if (MainThrusterIsHover) {
			if(simt > NextEventTime) {
				NextEventTime=simt+1.0;
				VECTOR3 actatt;
				GetHoverAttitude(actatt);
				VECTOR3 tgtatt=_V(PI/2.0, 0.0, 0.0);
				ComAttitude(actatt, tgtatt, false);
			}
		}
		else {
			OurVessel->ActivateNavmode(NAVMODE_PROGRADE);
		}
	}
}

void ApolloGuidance::GetHoverAttitude( VECTOR3 &actatt)

{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	static VECTOR3 xloc={1.0, 0.0, 0.0};
	static VECTOR3 yloc={0.0, 1.0, 0.0};
	static VECTOR3 zloc={0.0, 0.0, 1.0};
	double dir;
	VECTOR3 relpos, relvel, plvec, vperp, zerogl, xgl, ygl, zgl;
	OBJHANDLE hbody=OurVessel->GetGravityRef();
	OurVessel->GetRelativePos(hbody, relpos);
	OurVessel->GetRelativeVel(hbody, relvel);
	plvec=CrossProduct(relvel, relpos);
	//plvec is normal to orbital plane...
	plvec=Normalize(plvec);
	relvel=Normalize(relvel);
	vperp=CrossProduct(relvel, plvec);
	vperp=Normalize(vperp);
	OurVessel->Local2Global(zero, zerogl);
	OurVessel->Local2Global(xloc, xgl);
	OurVessel->Local2Global(yloc, ygl);
	OurVessel->Local2Global(zloc, zgl);
	xgl=xgl-zerogl;
	ygl=ygl-zerogl;
	zgl=zgl-zerogl;
	dir=relvel*ygl;
	if(dir > 0.0) {
		dir=1.0;
	} else {
		dir=-1.0;
	}
	actatt.x=dir*(PI/2.0-asin(vperp*ygl));
	actatt.y=asin(plvec*zgl);
	actatt.z=-asin(plvec*ygl);
}

// This is adapted from Chris Knestrick's Control.cpp, which wouldn't work right here
// The main differences are rates are a linear function of delta angle, rather than a 
// step function, and we do all three axes at once
void ApolloGuidance :: ComAttitude(VECTOR3 &actatt, VECTOR3 &tgtatt, bool fast)
{
	const double RATE_MAX = RAD*(15.0);
	const double DEADBAND_LOW = RAD*(0.01);
	const double RATE_FINE = RAD*(0.005);
	const double RATE_NULL = RAD*(0.0001);

	VECTOR3 PMI, Level, Drate, delatt, Rate;
	double Mass, Size, MaxThrust, Thrust, Rdead, factor, tacc;

	VESSELSTATUS status2;
	OurVessel->GetStatus(status2);
	OurVessel->GetPMI(PMI);
	Mass=OurVessel->GetMass();
	Size=OurVessel->GetSize();
	MaxThrust=OurVessel->GetMaxThrust(ENGINE_ATTITUDE);
//	THGROUP_HANDLE GetThrusterGroupHandle(THGROUP_TYPE THGROUP_ATT_PITCHUP);
//	MaxThrust=GetThrusterMax0(?);

	if(fast) {
		factor=PI;
	} else {
		factor=1.0;
	}
	tacc=oapiGetTimeAcceleration();
	if(tacc > 1) factor=1;

	delatt=tgtatt - actatt;
//X axis
	if (fabs(delatt.x) < DEADBAND_LOW) {
		if(fabs(status2.vrot.x) < RATE_NULL) {
		// set level to zero
			Level.x=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.x*status2.vrot.x)/Size;
			Level.x = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.x=fabs(delatt.x)/3.0;
		if(Rate.x < RATE_FINE) Rate.x=RATE_FINE;
		if (Rate.x > RATE_MAX ) Rate.x=RATE_MAX;
		Rdead=min(Rate.x/2,RATE_FINE);
		if(delatt.x < 0) {
			Rate.x=-Rate.x;
			Rdead=-Rdead;
		}
		Drate.x=Rate.x-status2.vrot.x;
		Thrust=factor*(Mass*PMI.x*Drate.x)/Size;
		if(delatt.x > 0) {
			if(Drate.x > Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else if (Drate.x < -Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else {
				Level.x=0;
			}
		} else {
			if(Drate.x < Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else if (Drate.x > -Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else {
				Level.x=0;
			}
		}
	}
//	sprintf(oapiDebugString(),"datt=%.3f rate=%.3f lvl=%.3f drat=%.3f dead=%.3f thr=%.3f act=%.3f",
//		delatt.x*DEG, Rate.x*DEG, Level.x, Drate.x*DEG, Rdead*DEG, Thrust, status2.vrot.x*DEG);

//Y-axis
	if (fabs(delatt.y) < DEADBAND_LOW) {
		if(fabs(status2.vrot.y) < RATE_NULL) {
		// set level to zero
			Level.y=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.y*status2.vrot.y)/Size;
			Level.y = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.y=fabs(delatt.y)/3.0;
		if(Rate.y < RATE_FINE) Rate.y=RATE_FINE;
		if (Rate.y > RATE_MAX ) Rate.y=RATE_MAX;
		Rdead=min(Rate.y/2,RATE_FINE);
		if(delatt.y < 0) {
			Rate.y=-Rate.y;
			Rdead=-Rdead;
		}
		Drate.y=Rate.y-status2.vrot.y;
		Thrust=factor*(Mass*PMI.y*Drate.y)/Size;
		if(delatt.y > 0) {
			if(Drate.y > Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else if (Drate.y < -Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else {
				Level.y=0;
			}
		} else {
			if(Drate.y < Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else if (Drate.y > -Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else {
				Level.y=0;
			}
		}
	}
//Z axis
	if (fabs(delatt.z) < DEADBAND_LOW) {
		if(fabs(status2.vrot.z) < RATE_NULL) {
		// set level to zero
			Level.z=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.z*status2.vrot.z)/Size;
			Level.z = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.z=fabs(delatt.z)/3.0;
		if(Rate.z < RATE_FINE) Rate.z=RATE_FINE;
		if (Rate.z > RATE_MAX ) Rate.z=RATE_MAX;
		Rdead=min(Rate.z/2,RATE_FINE);
		if(delatt.z< 0) {
			Rate.z=-Rate.z;
			Rdead=-Rdead;
		}
		Drate.z=Rate.z-status2.vrot.z;
		Thrust=factor*(Mass*PMI.z*Drate.z)/Size;
		if(delatt.z > 0) {
			if(Drate.z > Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else if (Drate.z < -Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else {
				Level.z=0;
			}
		} else {
			if(Drate.z < Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else if (Drate.z > -Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else {
				Level.z=0;
			}
		}
	}
	SetAttitudeRotLevel(Level);
}


void ApolloGuidance::BurnMainEngine(double thrust)

{
	if (thrust > 0.0)
		BurnFlag = true;
	else
		BurnFlag = false;
}

//
// LOI Program
//

void ApolloGuidance::Prog16(double simt)
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	VECTOR3 pos, vel, spos, svel, align, b, norm, veln, hvel, up;
	double blat, blon, period, apo, tta, per, ttp, vc, vthrust, mu, velm, fmass, 
		burn, dv, vmass, v, theta, offplane, dvn, vt;  
	const double GRAVITY=6.67259e-11;
	int i, n, k;

	if(ProgState > 1) return;
	LightCompActy();
	if(ProgState == 0) {
		ProgState++;
		ProgFlag01=false;
		DesiredDeltaVx = 0;
		DesiredDeltaVy = 0;
		BurnEndTime=simt;
		NextEventTime=simt;
//			char fname[8];
//			sprintf(fname,"attlog.txt");
//			outstr=fopen(fname,"w");
//			fprintf(outstr, "Open file \n");
		SetVerbNoun(16, 19);
	}
	if(ProgFlag01) {
		if(simt > BurnEndTime) {
			BurnMainEngine(0.0);
			OurVessel->SetAttitudeRotLevel(zero);
			oapiSetTimeAcceleration(DesiredDeltaV);
//			fclose(outstr);
			AwaitProgram();
			ProgState++;
			return;
		}
	}
	if(ProgState == 1) {
		if(simt > NextEventTime) {
			NextEventTime=simt + 0.1; //+DELTAT;
			blat=LandingLatitude*RAD;
			blon=LandingLongitude*RAD;
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			mu=GRAVITY*bmass;
			OurVessel->GetRelativePos(hbody, pos);
			OurVessel->GetRelativeVel(hbody, vel);
			norm=Normalize(CrossProduct(vel, pos));
			up=Normalize(pos);
			EquToRel(blat, blon, bradius, b);
			offplane=b*norm;
			DeltaPitchRate=offplane;
			theta=asin(offplane/bradius);
			dvn=(Mag(vel)*offplane)/bradius;
			veln=-Normalize(vel);
			OrbitParams(pos, vel, period, apo, tta, per, ttp);
			vc=sqrt(mu/(per+bradius));
			if(period <= 0.0) {
				// hyperbolic
				PredictPosVelVectors(pos, vel, mu, ttp, spos, svel, velm);
				dv=Mag(svel)-vc;
			} else {
				// elliptical orbit
				OurVessel->GetHorizonAirspeedVector(hvel);
				vt=sqrt(hvel.x*hvel.x+hvel.z*hvel.z);
				vc=sqrt(mu/(Mag(pos)));
				dv=vt-vc;
				// Burn stop condition
				if (apo < DesiredDeltaVy || per < 0.9 * DesiredDeltaVx) {
					dv=0.0;
					dvn=0.0;
				}
				//sprintf(oapiDebugString(), "apo %f per %f", apo, per);

			}
			dv=sqrt(dv*dv+dvn*dvn);

			if (MainThrusterIsHover) {
				vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
			} else {
				vthrust=OurVessel->GetMaxThrust(ENGINE_MAIN);
			}
			vmass=OurVessel->GetMass();
			n=OurVessel->DockCount();
			for (k=0; k<n; k++) {
				DOCKHANDLE Dock=OurVessel->GetDockHandle(k);
				i=OurVessel->DockingStatus(k);
				if(i == 0) break;
				OBJHANDLE hOther=OurVessel->GetDockStatus(Dock);
				VESSEL *Other=oapiGetVesselInterface(hOther);
				v=Other->GetMass();
				vmass=vmass+v;
			}
			fmass=vmass*(1.0-exp(-dv/VesselISP));
			burn=fmass/(vthrust/VesselISP);
			align=Normalize((veln*dv)+(norm*dvn));
			if (MainThrusterIsHover) {
				OrientAxis(align, 1, 0);
			} else {
				OrientAxis(align, 2, 0);
			}
			if(ProgFlag01) {
				if(burn > 30.0) {
					BurnEndTime=simt+burn;
				} else {
					if(simt+burn <= BurnEndTime) BurnEndTime=simt+burn;
				}
				BurnStartTime=BurnEndTime;
				return;
			} else {
				BurnStartTime=simt+(ttp-burn/2.0);
				BurnEndTime=simt+burn;
			}

			if(burn/2.0 > ttp) {
				DesiredDeltaV = oapiGetTimeAcceleration();
				DesiredDeltaVx = per;
				if(period <= 0.0) {
					// hyperbolic
					DesiredDeltaVy = per * 2.8;	// LOI1
				} else {
					// elliptic
					DesiredDeltaVy = per * 1.1;	// LOI2
				}
				oapiSetTimeAcceleration(1.0);
				BurnMainEngine(1.0);
				BurnStartTime = simt + burn;
				ProgFlag01 = true;
			}
			//sprintf(oapiDebugString(), "apo %f per %f ttp %f", apo, per, ttp);
		}
	}
}

//
// DOI Program
//

void ApolloGuidance::Prog17(double simt)
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	VECTOR3 pos, vel, spos, svel, norm, fwd, b, align;
	double mu, period, tburn, an, vc, altb, dt, blat, blon, apo, tta, per, ttp,
		vlon, vlat, vrad, dlat, dlon, aa, cc, sign, pday, velm, tpdi,
		passes, a, e, h, v, dv, vthrust, vmass, burn, psave, fmass;
	const double GRAVITY=6.67259e-11;
	int k, n, i;
	if(ProgState > 1) return;
	LightCompActy();
	if(ProgState == 0) {
//			char fname[8];
//			sprintf(fname,"P28log.txt");
//			outstr=fopen(fname,"w");
//			fprintf(outstr, "P28: ProgState=%d\n", ProgState);
		ProgState++;
		NextEventTime=simt;
		BurnStartTime=0.0;
		DesiredPlaneChange=0.0;
		DeltaPitchRate=0.0;
		DesiredLAN=9900.0;
		ProgFlag01=false;
		ProgFlag02=false;
		ProgFlag03=false;
	}

	if(ProgFlag01) {
//		sprintf(oapiDebugString(), "Time=%.1f dV=%.3f pass=%.1f", 
//			BurnStartTime-simt, DesiredDeltaV, DeltaPitchRate);
		if(simt >= BurnEndTime) {
			BurnMainEngine(0.0);
			OurVessel->SetAttitudeRotLevel(zero);
			oapiSetTimeAcceleration(DesiredDeltaV);
//			fclose(outstr);
			AwaitProgram();
			ProgState++;
			return;
		}
		if(simt > NextEventTime) {
			NextEventTime=simt+DELTAT;
			align.x=DesiredDeltaVx;
			align.y=DesiredDeltaVy;
			align.z=DesiredDeltaVz;
			if (MainThrusterIsHover) {
				OrientAxis(align, 1, 0);
			} else {
				OrientAxis(align, 2, 0);
			}
		}
		if(simt >= BurnStartTime) {
			if(ProgFlag03) return;
			DesiredDeltaV=oapiGetTimeAcceleration();
			oapiSetTimeAcceleration(1.0);
			ProgFlag03=true;
			dt=BurnEndTime-BurnStartTime;
			BurnEndTime=simt+dt;
			if (MainThrusterIsHover) {
				BurnMainEngine(0.1);
			} else {
				BurnMainEngine(1.0);
			}
		}
		return;
	}

	if(ProgState == 1) {
		if(simt > NextEventTime) {
			NextEventTime=simt+DELTAT;
			blat=LandingLatitude*RAD;
			blon=LandingLongitude*RAD;
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			pday=oapiGetPlanetPeriod(hbody);
			EquToRel(blat, blon, bradius, b);
			OurVessel->GetEquPos(vlon, vlat, vrad);
			dlat=vlat-blat;
			dlon=vlon-blon;
			aa = pow(sin(dlat/2), 2) + cos(blat) * cos (vlat) * pow(sin(dlon/2), 2);
			cc = 2.0 * atan2(sqrt(aa), sqrt(1 - aa));
			if(DesiredPlaneChange == 0.0) {
				DesiredPlaneChange=cc;
				NextEventTime=simt+0.2;
				return;
			} else {
				if (cc > DesiredPlaneChange) {
					sign=-1.0;
				} else {
					sign=1.0;
				}
				DesiredPlaneChange=cc;
			}
			if(sign < 0.0) {
				cc=2.0*PI-cc;
			}
			OurVessel->GetRelativePos(hbody, pos);
			OurVessel->GetRelativeVel(hbody, vel);
			OrbitParams(pos, vel, period, apo, tta, per, ttp);
			passes=DeltaPitchRate;
//			fprintf(outstr, "passes in=%.1f \n", passes);
			// cc is central angle TO site...
			if(cc > PI*1.1) {
				// burn is before next pass
				tburn=period*((cc-PI*1.1)/(2.0*PI));
			} else {
				// burn is after next pass
				tburn=period+period*((cc-PI*1.1)/(2.0*PI));
				if(passes > 0.0) {
					passes=passes-1.0;
				} else {
					passes=1.0;
				}
			}
			DeltaPitchRate=passes;
//			fprintf(outstr, "passes=%.1f \n", passes);
			psave=period;
//			fprintf(outstr, "tburn=%.1f angle=%.3f\n", tburn, cc*DEG);
//			sprintf(oapiDebugString(), "tburn=%.1f angle=%.3f", tburn, cc*DEG);
			// do the burn before next pass...
			mu=GRAVITY*bmass;
			//iterate to get the timing just right...
			for (k=0; k<20; k++) {
				PredictPosVelVectors(pos, vel, mu, tburn, spos, svel, velm);
				altb=Mag(spos);
				a=(altb+bradius+15000.0)/2.0;
				period=2.0*PI*sqrt((a*a*a)/mu);
				tpdi=tburn+period*(passes+0.5);
//				sprintf(oapiDebugString(), "k=%d apo=%.1f per=%.1f time=%.1f tburn=%.1f", 
//					k, altb, period, tpdi, tburn);
				dlon=(tpdi/pday)*2.0*PI;
				blon=LandingLongitude*RAD+dlon;
				if(blon < -PI) blon=blon+2.0*PI;
				if(blon > PI) blon=blon-2.0*PI;
				EquToRel(blat, blon, bradius+15000.0, b);
//				fprintf(outstr, "tpdi=%.1f blon=%.3f dlon=%.3f \n",
//					tpdi, blon*DEG, dlon*DEG);
				an=acos(Normalize(spos)*Normalize(b));
				vc=svel*b;
				if(vc < 0.0) an=2.0*PI-an;
				dt=period*((an-195.0*RAD)/(2.0*PI));
//				fprintf(outstr, "tburn=%.1f dt=%.3f an=%.3f k=%d\n", 
//					tburn, dt, an*DEG, k);
				tburn=tburn+dt;
//			sprintf(oapiDebugString(), "dt=%.3f tburn=%.1f an=%.3f k=%d", dt, tburn, an*DEG, k);
				if(fabs(dt) < 1.0) {
					// we have a solution...
					ProgFlag01=true;
					break;
				}
			}
//			sprintf(oapiDebugString(), "dt=%.3f tburn=%.1f an=%.3f k=%d", dt, tburn, an*DEG, k);
			if(tburn > psave) DeltaPitchRate=1.0;
			e=altb/a-1.0;
			h=sqrt(mu*a*(1.0-e*e));
			v=h/altb;
			norm=Normalize(CrossProduct(svel, spos));
			fwd=Normalize(CrossProduct(spos, norm))*v;
			norm=fwd-svel;
			dv=Mag(norm);
//			sprintf(oapiDebugString(), "%d t=%.1f s=%.1f %.1f %.1f v=%.1f %.1f %.1f dv=%.1f",
//				k, tburn, svel, fwd, dv);
			norm=Normalize(norm);
			DesiredDeltaVx=norm.x;
			DesiredDeltaVy=norm.y;
			DesiredDeltaVz=norm.z;
			if (MainThrusterIsHover) {
				OrientAxis(align, 1, 0);
				vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
			} else {
				OrientAxis(align, 2, 0);
				vthrust=OurVessel->GetMaxThrust(ENGINE_MAIN);
			}
			v=0.0;
			vmass=OurVessel->GetMass();
			n=OurVessel->DockCount();
			for (k=0; k<n; k++) {
				DOCKHANDLE Dock=OurVessel->GetDockHandle(k);
				i=OurVessel->DockingStatus(k);
				if(i == 0) break;
				OBJHANDLE hOther=OurVessel->GetDockStatus(Dock);
				VESSEL *Other=oapiGetVesselInterface(hOther);
				v=Other->GetMass();
				vmass=vmass+v;
//				sprintf(oapiDebugString(), "mass=%.3f v=%.1f count=%d %d %d", vmass, v, n, k, i);
			}
			fmass=vmass*(1.0-exp(-dv/VesselISP));
			burn=fmass/(vthrust/VesselISP);
			if(MainThrusterIsHover) burn=burn*10.0;
//			sprintf(oapiDebugString(), "dv=%.3f mass=%.1f fmass=%.3f burn=%.1f time=%.1f",
//				dv, vmass, fmass, burn, tburn);
			DesiredDeltaV=dv*1000.0;
			BurnStartTime=simt+tburn-burn/2.0;
			BurnEndTime=BurnStartTime+burn;
			SetVerbNounAndFlash(6, 23);
			
		}
	}

}


void ApolloGuidance::Prog17Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	case 1:
		DeltaPitchRate=R1;
		ProgFlag01=false;
		break;

	}
}

//
// Plane Change program
//

void ApolloGuidance::Prog18(double simt)
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	VECTOR3 pos, vel, norm, b, pn, nodes, npn, align;
	double mu, vmass, vlat, vlon, vrad, blat, blon, dlat, dlon, aa, cc,  
		tpass, period, pday, offplane, apo, per, tta, ttp, sign, an, ann, 
		dv, vthrust, burn, anext, ttn, v, fmass, dt;
	const double GRAVITY=6.67259e-11;
	int i, n, k;
	if(ProgState > 3) return;
	LightCompActy();
	if(ProgState == 0) {
		NextEventTime=simt;
		ProgState++;
		ProgFlag01=false;
		DeltaPitchRate=0.0;
		BurnStartTime=0.0;
		DesiredDeltaV=0.0;
		DesiredLAN=9900.0;
		DesiredPlaneChange=0.0;
	}
	if(ProgState < 3) {
		if(simt >= NextEventTime) {
			NextEventTime=simt+DELTAT;
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			mu=GRAVITY*bmass;
			OurVessel->GetRelativePos(hbody, pos);
			OurVessel->GetRelativeVel(hbody, vel);
			norm=Normalize(CrossProduct(vel, pos));
			blat=LandingLatitude*RAD;
			blon=LandingLongitude*RAD;
//			sprintf(oapiDebugString(), "lat=%.3f lon=%.3f ", LandingLatitude, LandingLongitude);
			EquToRel(blat, blon, bradius, b);
			OurVessel->GetEquPos(vlon, vlat, vrad);
			dlat=vlat-blat;
			dlon=vlon-blon;
			aa = pow(sin(dlat/2), 2) + cos(blat) * cos (vlat) * pow(sin(dlon/2), 2);
			cc = 2.0 * atan2(sqrt(aa), sqrt(1 - aa));
			if(DesiredPlaneChange == 0.0) {
				DesiredPlaneChange=cc;
				NextEventTime=simt+0.2;
				return;
			} else {
				if (cc > DesiredPlaneChange) {
					sign=-1.0;
				} else {
					sign=1.0;
				}
				DesiredPlaneChange=cc;
			}
			if(sign < 0.0) {
				cc=2.0*PI-cc;
			}
			OrbitParams(pos, vel, period, apo, tta, per, ttp);
			pday=oapiGetPlanetPeriod(hbody);
			tpass=(cc/(2.0*PI))*period-period/24.0;
			if(tpass < 0.0) tpass=period+tpass;
//			sprintf(oapiDebugString(), "tpass=%.1f oldt=%.1f npass=%.1f cc=%.3f", tpass,
//				DesiredLAN, DeltaPitchRate, cc*DEG);
			if(tpass > DesiredLAN) {
				DeltaPitchRate=DeltaPitchRate-1.0;
			}
			DesiredLAN=tpass;
			dlon=((period*DeltaPitchRate+tpass)/pday)*2.0*PI;
			blon=LandingLongitude*RAD+dlon;
			if(blon < -PI) dlon=dlon+2.0*PI;
			if(blon > PI) dlon=dlon-2.0*PI;
			EquToRel(blat, blon, bradius+15000.0, b);
			offplane=b*norm;
			DesiredDeltaV=offplane;
			b=Normalize(b);
			nodes=Normalize(CrossProduct(norm, b));
			npn=CrossProduct(b, nodes);
			pn=Normalize(pos);
			an=acos(norm*npn);
			ann=acos(pn*nodes);
			if(vel*nodes > 0.0) {
				anext=0.0;
//				align=Normalize(norm*offplane);
				align=Normalize((norm+npn)*0.5*offplane); // better for huge changes
			} else {
				anext=PI;
//				align=-Normalize(norm*offplane);
				align=-Normalize((norm+npn)*0.5*offplane);
			}
			DesiredDeltaVx=align.x;
			DesiredDeltaVy=align.y;
			DesiredDeltaVz=align.z;
			if (MainThrusterIsHover) {
				OrientAxis(align, 1, 0);
				vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
			} else {
				OrientAxis(align, 2, 0);
				vthrust=OurVessel->GetMaxThrust(ENGINE_MAIN);
			}
			dv=2.0*Mag(vel)*sin(an/2.0);
			vmass=OurVessel->GetMass();
			n=OurVessel->DockCount();
			for (k=0; k<n; k++) {
				DOCKHANDLE Dock=OurVessel->GetDockHandle(k);
				i=OurVessel->DockingStatus(k);
				if(i == 0) break;
				OBJHANDLE hOther=OurVessel->GetDockStatus(Dock);
				VESSEL *Other=oapiGetVesselInterface(hOther);
				v=Other->GetMass();
				vmass=vmass+v;
//				sprintf(oapiDebugString(), "mass=%.3f v=%.1f count=%d %d %d", vmass, v, n, k, i);
			}
			fmass=vmass*(1.0-exp(-dv/VesselISP));
			burn=fmass/(vthrust/VesselISP);
//			sprintf(oapiDebugString(), "dv=%.3f mass=%.1f fmass=%.3f t=%.1f isp=%.1f",
//				dv, vmass, fmass, burn, VesselISP);
//			burn=(dv*vmass)/vthrust;
//			sprintf(oapiDebugString(), "an=%.5f node=%.5f off=%.1f burn=%.3f nx=%.5f", 
//			an*DEG, ann*DEG, offplane, burn, anext*DEG);
			if(anext > 0.0) {
				ttn=period*((anext-ann)/(2.0*PI));
			} else {
				ttn=period*(ann/(2.0*PI));
			}
			if(DeltaPitchRate == 0.0) {
				if(tpass < ttn) DeltaPitchRate=1.0;
			}
			BurnStartTime=simt+ttn-(burn/2.0);
			if(ProgState == 1) {
				ProgState++;
				SetVerbNounAndFlash(6, 23);
			}
			if(ttn < 120.0) {
				ProgState=3;
				BurnEndTime=BurnStartTime+burn;
				return;
			}
		}
		return;
	}
	if(ProgState == 3) {
//		sprintf(oapiDebugString(), "Time-to-Burn=%.1f", BurnStartTime-simt);
		if(simt >= BurnEndTime) {
			BurnMainEngine(0.0);
			OurVessel->SetAttitudeRotLevel(zero);
			oapiSetTimeAcceleration(DesiredDeltaV);
			AwaitProgram();
			ProgState++;
			return;
		}
		if(simt > NextEventTime) {
			NextEventTime=simt+DELTAT;
			align.x=DesiredDeltaVx;
			align.y=DesiredDeltaVy;
			align.z=DesiredDeltaVz;
			if (MainThrusterIsHover) {
				OrientAxis(align, 1, 0);
			} else {
				OrientAxis(align, 2, 0);
			}
		}
		if(simt >= BurnStartTime) {
			if(ProgFlag01) return;
			DesiredDeltaV=oapiGetTimeAcceleration();
			oapiSetTimeAcceleration(1.0);
			ProgFlag01=true;
			dt=BurnEndTime-BurnStartTime;
			BurnEndTime=simt+dt;
			BurnMainEngine(1.0);
		}
	}
}

void ApolloGuidance::Prog18Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	case 1:
		DeltaPitchRate=R1;
		break;

	case 2:
		DeltaPitchRate=R1;
		ProgState=1;
	}
}

//
// Orbit change program
//

void ApolloGuidance::Prog19(double simt)
{
	static VECTOR3 zero={0.0, 0.0, 0.0};
	VECTOR3 pos, vel, spos, svel, up, no, fwd, delta, veln, 
		vh, vf, vup, epos, v1, v2;
	double mu, e, a, p, v, ea, ma, vr, tsp, tr, period, apo, per, tta, ttp,
		velm, vdot, sign, dv, vc, vthrust, vmass, fmass, burn, bmass, tb,
		bradius, theta, ap, pe, en, an, pn, r, h, vn, rc, rn, dph,
		vch, vcl, vnh, vnl, vp, vv, vhv, cth, sth, vfwd, can, dt;
	const double GRAVITY=6.67259e-11;
	int i, n, k;
	bool high, intersect;
    if(ProgState > 4) return;
	LightCompActy();
	if(simt > NextEventTime) {
		NextEventTime=simt+DELTAT;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		switch(ProgState)
		{
		case 0:
			ProgState++;
			ProgFlag01=false;
			ProgFlag02=false;
			DesiredAzimuth=0.0;
			NextEventTime=simt;
			break;

		case 1:
			ProgState++;
			OurVessel->GetRelativePos(hbody, pos);
			OurVessel->GetRelativeVel(hbody, vel);
			OrbitParams(pos, vel, period, apo, tta, per, ttp);
//			char fname[8];
//			sprintf(fname,"P19log.txt");
//			outstr=fopen(fname,"w");
			if(period > 0.0) {
				DesiredApogee=apo;
				DesiredPerigee=per;
				SetVerbNounAndFlash(6,30);
			} else {
				//we're not in orbit
				AbortWithError(606);
				return;
			}
			break;

		case 3:
			OurVessel->GetRelativePos(hbody, pos);
			OurVessel->GetRelativeVel(hbody, vel);
			rc=Mag(pos);
			OrbitParams(pos, vel, period, apo, tta, per, ttp);
			bradius=oapiGetSize(hbody);
			bmass=oapiGetMass(hbody);
			mu=GRAVITY*bmass;
			if (MainThrusterIsHover) {
				vthrust=OurVessel->GetMaxThrust(ENGINE_HOVER);
			} else {
				vthrust=OurVessel->GetMaxThrust(ENGINE_MAIN);
			}
			vmass=OurVessel->GetMass();
			n=OurVessel->DockCount();
			for (k=0; k<n; k++) {
				DOCKHANDLE Dock=OurVessel->GetDockHandle(k);
				i=OurVessel->DockingStatus(k);
				if(i == 0) break;
				OBJHANDLE hOther=OurVessel->GetDockStatus(Dock);
				VESSEL *Other=oapiGetVesselInterface(hOther);
				v=Other->GetMass();
				vmass=vmass+v;
			}
			// the orbit we're in
//			fprintf(outstr, "apo=%.1f tta=%.1f per=%.1f ttp=%.1f \n", apo, tta, per, ttp);
			e=(2.0*(apo+bradius))/(apo+per+2.0*bradius)-1.0;
			a=bradius+((apo+per)/2.0);
			p=a*(1.0-e*e);
			vdot=vel*pos;
			sign=vdot/fabs(vdot);
			v=sign*acos(((p/Mag(pos))-1.0)/e); //current angle from periapsis
			ea=acos((cos(v)+e)/(1.0+e*cos(v)));
			ma=ea-e*sin(ea);
			tsp=ma*sqrt(fabs(a*a*a)/mu);

			if(DesiredPerigee > apo) {
				tb=300.0;
				PredictPosVelVectors(pos, vel, mu, tb, spos, svel, velm);
				vh=Normalize(CrossProduct(spos, svel));
				vf=Normalize(CrossProduct(vh, spos));
				vup=Normalize(spos);
				theta=DesiredAzimuth*RAD;
				ap=DesiredApogee+bradius;
				pe=DesiredPerigee+bradius;
				// now calculate elements for the desired orbit
				en=(2.0*ap)/(ap+pe)-1.0;
				an=(ap+pe)/2.0;
				pn=an*(1.0-en*en);
				// calc new orbit altitude at intercept
				dph=acos(Normalize(pos)*Normalize(spos));
				can=130.0*RAD;
				vn=v+theta-dph-can;
				rn=pn/(1.0+en*cos(vn));
//				fprintf(outstr, "vn=%.3f dph=%.3f alt=%.1f\n", vn*DEG,dph*DEG, rn-bradius);
				epos=vup*(rn*cos(can))+vf*(rn*sin(can));
//				dt=45.0*60.0;
				dt=period*(can/(2.0*PI));
				Lambert(spos, epos, dt, mu, v1, v2);
//				fprintf(outstr, "Lambert spos=%.1f %.1f %.1f epos=%.1f %.1f %.1f \n",
//					spos, epos);
//				fprintf(outstr, "Lambert v1=%.1f %.1f %.1f v2=%.1f %.1f %.1f \n",
//					v1, v2);

				delta=v1-svel;
				dv=Mag(delta);
				CutOffVel=dv;
//				fprintf(outstr, "delta=%.3f %.3f %.3f dv=%.3f \n", delta, dv);
				fmass=vmass*(1.0-exp(-dv/VesselISP));
				burn=fmass/(vthrust/VesselISP);
				BurnStartTime=simt+tb-burn/2.0;
				BurnEndTime=BurnStartTime+burn;	
				delta=Normalize(delta);
				DesiredDeltaVx=delta.x;
				DesiredDeltaVy=delta.y;
				DesiredDeltaVz=delta.z;
				BurnTime=BurnStartTime;
				SetVerbNoun(16, 50);
				ProgFlag01=true;
				ProgState++;
				return;
			}
			if(DesiredApogee < per) {
				tb=300.0;
				PredictPosVelVectors(pos, vel, mu, tb, spos, svel, velm);
				vh=Normalize(CrossProduct(spos, svel));
				vf=Normalize(CrossProduct(vh, spos));
				vup=Normalize(spos);

				theta=DesiredAzimuth*RAD;
				ap=DesiredApogee+bradius;
				pe=DesiredPerigee+bradius;
				// now calculate elements for the desired orbit
				en=(2.0*ap)/(ap+pe)-1.0;
				an=(ap+pe)/2.0;
				pn=an*(1.0-en*en);
				// calc new orbit altitude at intercept
				dph=acos(Normalize(pos)*Normalize(spos));
				can=130.0*RAD;
				vn=v+theta-dph-can;
				rn=pn/(1.0+en*cos(vn));
//				fprintf(outstr, "vn=%.3f dph=%.3f alt=%.1f\n", vn*DEG,dph*DEG, rn-bradius);
				epos=vup*(rn*cos(can))+vf*(rn*sin(can));

//				dt=45.0*60.0;
				dt=period*(can/(2.0*PI));
				Lambert(spos, epos, dt, mu, v1, v2);
//				fprintf(outstr, "Lambert spos=%.1f %.1f %.1f epos=%.1f %.1f %.1f \n",
//					spos, epos);
//				fprintf(outstr, "Lambert v1=%.1f %.1f %.1f v2=%.1f %.1f %.1f \n",
//					v1, v2);

				delta=v1-svel;
				dv=Mag(delta);
				CutOffVel=dv;
//				fprintf(outstr, "delta=%.3f %.3f %.3f dv=%.3f \n", delta, dv);
				fmass=vmass*(1.0-exp(-dv/VesselISP));
				burn=fmass/(vthrust/VesselISP);
				BurnStartTime=simt+tb-burn/2.0;
				BurnEndTime=BurnStartTime+burn;	
				delta=Normalize(delta);
				DesiredDeltaVx=delta.x;
				DesiredDeltaVy=delta.y;
				DesiredDeltaVz=delta.z;
				BurnTime=BurnStartTime;
				SetVerbNoun(16, 50);
				ProgFlag01=true;
				ProgState++;
				return;
			}
			if(fabs(DesiredApogee-DesiredPerigee) < 1000.0) {
			// circularize at a specific altitude...
				vr=acos(((p/(DesiredApogee+bradius))-1.0)/e); //angle from periapsis where r=a
				ea=acos((cos(vr)+e)/(1.0+e*cos(vr)));
				ma=ea-e*sin(ea);
				tr=ma*sqrt(fabs(a*a*a)/mu);
				if(ttp < tr) {
					tb=ttp+tr;
				} else {
					tb=ttp-tr;
					// added 01/16/05
					if(tsp < tr) {
						tb=tr-tsp;
						if(tb < 300.0) tb=ttp-tr;
					}
				}
				if(tb < 300.0) tb=ttp+tr;
				PredictPosVelVectors(pos, vel, mu, tb, spos, svel, velm);
				up=Normalize(spos);
				no=Normalize(CrossProduct(spos, svel));
				fwd=Normalize(CrossProduct(no, up));
				vc=sqrt(mu/Mag(spos));  // vel for circular orbit at spos radius
				delta=fwd*vc-svel;
				dv=Mag(delta);
				CutOffVel=dv;
				fmass=vmass*(1.0-exp(-dv/VesselISP));
				burn=fmass/(vthrust/VesselISP);
				BurnStartTime=simt+tb-burn/2.0;
				BurnEndTime=BurnStartTime+burn;	
				delta=Normalize(delta);
				DesiredDeltaVx=delta.x;
				DesiredDeltaVy=delta.y;
				DesiredDeltaVz=delta.z;
				BurnTime=BurnStartTime;
				SetVerbNoun(16, 50);
				ProgFlag01=false;
				ProgState++;
				return;
			}
			// if we made it to here our orbit goes between DesiredPerigee and DesiredApogee
			// let's see if they intersect...
			theta=DesiredAzimuth*RAD;
//			fprintf(outstr, "Theta=%.3f\n", theta);
//			fprintf(outstr, "old ap=%.1f ta=%.1f pe=%.1f tp=%.1f\n", apo, tta, per, ttp);
			// real anomoly from our desired periapsis
			vn=v+theta;
			ap=DesiredApogee+bradius;
			pe=DesiredPerigee+bradius;
			// now calculate elements for the desired orbit
			en=(2.0*ap)/(ap+pe)-1.0;
			an=(ap+pe)/2.0;
			pn=an*(1.0-en*en);
			rn=pn/(1.0+en*cos(vn));
			// rc and rn the radii at this v of the current and desired orbits
//			sprintf(oapiDebugString(), "rc=%.1f rn=%.1f v=%.5f vn=%.5f",
//				rc, rn, v*DEG, vn*DEG);
			high=false;
			if(rc > rn) high=true;


			// look for intersection starting here...
			intersect=false;
			vc=v;
			for (k=0; k<36; k++) {
				vc=vc+10.0*RAD;
				vn=vn+10.0*RAD;
				rc=p/(1.0+e*cos(vc));
				rn=pn/(1.0+en*cos(vn));
//				fprintf(outstr, "k=%d vc=%.3f vn=%.3f rc=%.1f rn=%.1f\n",
//					k, vc*DEG, vn*DEG, rc-bradius, rn-bradius);
				if(high) {
					if(rc < rn) {
						high=false;
						intersect=true;
						break;
					}
				} else {
					if(rc > rn) {
						high=true;
						intersect=true;
						break;
					}
				}
			}
//			fprintf(outstr, "intersect=%d\n", intersect);
//			fprintf(outstr, "k=%d vc=%.3f vn=%.3f rc=%.1f rn=%.1f\n",
//					k, vc*DEG, vn*DEG, rc-bradius, rn-bradius);
			if(intersect) {
// intersection is between vc and vc-10 degrees 
				vcl=vc-10.0*RAD;
				vch=vc;
				vnl=vn-10.0*RAD;
				vnh=vn;
				for (k=0; k<32;k++) {
//					fprintf(outstr, "k=%d vcl=%.3f vch=%.3f vnl=%.3f vnh=%.3f \n",
//						k, vcl*DEG, vch*DEG, vnl*DEG, vnh*DEG);
					vc=(vcl+vch)/2.0;
					vn=(vnl+vnh)/2.0;
					rc=p/(1.0+e*cos(vc));
					rn=pn/(1.0+en*cos(vn));
					if(fabs(rc-rn) < 0.001) break;
					high=false;
//					fprintf(outstr, "vc=%.3f vn=%.3f rc=%.1f rn=%.1f high=%d\n",
//						vc*DEG, vn*DEG, rc-bradius, rn-bradius, high);
					if(high) {
						if(rc>rn) {
							vch=vc;
							vnh=vn;
						} else {
							vcl=vc;
							vnl=vn;
						}
					} else {
						if(rc>rn) {
							vcl=vc;
							vnl=vn;
						} else {
							vch=vc;
							vnh=vn;
						}
					}
				}

				ea=acos((cos(vc)+e)/(1.0+e*cos(vc)));
				ma=ea-e*sin(ea);
				tsp=ma*sqrt(fabs(a*a*a)/mu);
				// choose the next intersection >= 300 seconds from now
				tb=ttp-tsp;

				//added 01/15/05
				if(period-ttp < tsp) {
					tb=tsp-(period-ttp);
					if(tb < 300.0) tb=ttp-tsp;
				}
				if(tb < 300.0) tb=ttp+tsp;
//				fprintf(outstr, "tsp=%.1f ttp=%.1f\n", tsp, ttp);
				PredictPosVelVectors(pos, vel, mu, tb, spos, svel, velm);
//				fprintf(outstr, "pos=%.1f %.1f %.1f vel=%.1f %.1f %.1f tb=%.1f\n", pos, vel, tb);
//				fprintf(outstr, "spos=%.1f %.1f %.1f svel=%.1f %.1f %.1f\n", spos, svel);
				r=Mag(spos);
				// now we know the position and time of the intersection, and starting v
				// let's calculate the v for new orbit...
				h=sqrt(mu*an*(1-en*en));
				vp=h/pe;  // velocity at periapsis for new orbit...
				vhv=h/r;
				sth=(en*sin(vn))/sqrt(1.0+2.0*en*cos(vn)+en*en);
				cth=sqrt(1.0-sth*sth);
				vv=sth*vhv;
				vfwd=cth*vhv;
//				fprintf(outstr, "h/r=%.1f fwd=%.1f vert=%.1f\n", vhv, vfwd, vv);
				vh=Normalize(CrossProduct(spos, svel));
				vf=Normalize(CrossProduct(vh, spos));
				vup=Normalize(spos);
				veln=vf*vfwd+vup*vv;
				OrbitParams(spos, veln, period, apo, tta, per, ttp);
//				fprintf(outstr, "new ap=%.1f ta=%.1f pe=%.1f tp=%.1f\n", apo, tta, per, ttp);
				delta=veln-svel;
//				fprintf(outstr, "veln=%.3f %.3f %.3f delta=%.3f %.3f %.3f\n", veln, delta);
				dv=Mag(delta);
				CutOffVel=dv;
				fmass=vmass*(1.0-exp(-dv/VesselISP));
				burn=fmass/(vthrust/VesselISP);
				BurnStartTime=simt+tb-burn/2.0;
				BurnEndTime=BurnStartTime+burn;	
				delta=Normalize(delta);
				DesiredDeltaVx=delta.x;
				DesiredDeltaVy=delta.y;
				DesiredDeltaVz=delta.z;
				BurnTime=BurnStartTime;
//				fprintf(outstr, "burn=%.1f \n", burn);
				SetVerbNoun(16, 50);
				ProgFlag01=false;
				ProgState++;
				return;
			} else { 
// no intersection
				tb=300.0;
				PredictPosVelVectors(pos, vel, mu, tb, spos, svel, velm);
				vh=Normalize(CrossProduct(spos, svel));
				vf=Normalize(CrossProduct(vh, spos));
				vup=Normalize(spos);
				// calc new orbit altitude at intercept
				dph=acos(Normalize(pos)*Normalize(spos));
				can=130.0*RAD;
				vn=v+theta-dph-can;
				rn=pn/(1.0+en*cos(vn));
//				fprintf(outstr, "vn=%.3f dph=%.3f rn=%.1f\n", vn*DEG,dph*DEG, rn);
				epos=vup*(rn*cos(can))+vf*(rn*sin(can));
				dt=period*(can/(2.0*PI));
//				dt=45.0*60.0;
				Lambert(spos, epos, dt, mu, v1, v2);
//				fprintf(outstr, "Lambert spos=%.1f %.1f %.1f epos=%.1f %.1f %.1f ealt=%.1f\n",
//					spos, epos, Mag(epos)-bradius);
//				fprintf(outstr, "Lambert v1=%.1f %.1f %.1f v2=%.1f %.1f %.1f \n",
//					v1, v2);

				delta=v1-svel;
				dv=Mag(delta);
				CutOffVel=dv;
//				fprintf(outstr, "delta=%.3f %.3f %.3f dv=%.3f \n", delta, dv);
				fmass=vmass*(1.0-exp(-dv/VesselISP));
				burn=fmass/(vthrust/VesselISP);
				BurnStartTime=simt+tb-burn/2.0;
				BurnEndTime=BurnStartTime+burn;	
				delta=Normalize(delta);
				DesiredDeltaVx=delta.x;
				DesiredDeltaVy=delta.y;
				DesiredDeltaVz=delta.z;
				BurnTime=BurnStartTime;
				SetVerbNoun(16, 50);
				ProgFlag01=true;
				ProgState++;
				return;
			}


		case 4:
			delta.x=DesiredDeltaVx;
			delta.y=DesiredDeltaVy;
			delta.z=DesiredDeltaVz;
//			sprintf(oapiDebugString(), "Orienting...");
			if (MainThrusterIsHover) {
				OrientAxis(delta, 1, 0);
			} else {
				OrientAxis(delta, 2, 0);
			}
			break;
		}	
	}
	if(ProgState == 4) {
//		sprintf(oapiDebugString(), "burn in %.1f secs f2=%d", simt-BurnStartTime, ProgFlag02);
		if ((simt+0.05) > BurnEndTime) {
			BurnMainEngine(0.0);
			ProgFlag02=false;
			OurVessel->SetAttitudeRotLevel(zero);
			oapiSetTimeAcceleration(DesiredDeltaV);
			if(ProgFlag01 == false) {
//				fclose(outstr);
				AwaitProgram();
				ProgState++;
				return;
			}
			ProgState=3;
			return;
		}
		if(ProgFlag02) return;  //engine is already on...
		if (simt > BurnStartTime) {
			DesiredDeltaV=oapiGetTimeAcceleration();
			oapiSetTimeAcceleration(1.0);
			ProgFlag02=true;
			dt=BurnEndTime-BurnStartTime;
			BurnEndTime=simt+dt;
//			sprintf(oapiDebugString(), "dt=%.1f Engine on", dt);
			BurnMainEngine(1.0);
		}
	}
}

void ApolloGuidance::Prog19Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	case 2:
		DesiredApogee=R1*100.0;
		DesiredPerigee=R2*100.0;
		DesiredAzimuth=R3/100.0;
		ProgState++;
		break;
	}
}

void ApolloGuidance::EquToRel(double vlat, double vlon, double vrad, VECTOR3 &pos)
{
		VECTOR3 a;
		double obliq, theta, rot;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		a.x=cos(vlat)*cos(vlon)*vrad;
		a.z=cos(vlat)*sin(vlon)*vrad;
		a.y=sin(vlat)*vrad;
		obliq=oapiGetPlanetObliquity(hbody);
		theta=oapiGetPlanetTheta(hbody);
		rot=oapiGetPlanetCurrentRotation(hbody);
		pos.x=a.x*(cos(theta)*cos(rot)-sin(theta)*cos(obliq)*sin(rot))-
			a.y*sin(theta)*sin(obliq)-
			a.z*(cos(theta)*sin(rot)+sin(theta)*cos(obliq)*cos(rot));
		pos.y=a.x*(-sin(obliq)*sin(rot))+
			a.y*cos(obliq)-
			a.z*sin(obliq)*cos(rot);
		pos.z=a.x*(sin(theta)*cos(rot)+cos(theta)*cos(obliq)*sin(rot))+
			a.y*cos(theta)*sin(obliq)+
			a.z*(-sin(theta)*sin(rot)+cos(theta)*cos(obliq)*cos(rot));

}
//
// This will calculate orbit params for elliptical or hyperbolic trajectories
// if period=0.0, the orbit is hyperbolic and there is no apoapsis or time-to-apoapsis
// These variables are set to zero.  Periapsis will be valid, ttp is positive before
// periapsis and negative after periapsis.  In the elliptical case, tta and ttp are
// always positive and correspond to the time till next apo or per.
//
void ApolloGuidance::OrbitParams(VECTOR3 &rpos, VECTOR3 &rvel, double &period, 
				double &apo, double &tta, double &per, double &ttp)
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 h, n, ve;
	double rdotv, p, e, i, om, w, v, u, l, a, eanom, manom, tsp, mu, y;
	OBJHANDLE hbody=OurVessel->GetGravityRef();
	double bradius=oapiGetSize(hbody);
	double bmass=oapiGetMass(hbody);
	mu=GRAVITY*bmass;
	rdotv=rvel*rpos;
	h=CrossProduct(rpos, rvel);
	n=CrossProduct(_V(0.0, 0.0, 1.0), h);
	ve=(rpos*(Mag(rvel)*Mag(rvel)-(mu/Mag(rpos)))-rvel*(rpos*rvel))/mu;
	//calculate orbit elements...
	p=(h*h)/mu;
	e=Mag(ve);
	i=acos(h.z/Mag(h));
	om=acos(n.x/Mag(n));
	if(n.y > 0.0) om=-om;
	w=acos((n*ve)/(Mag(n)*Mag(ve)));
	if(ve.z < 0.0) w=2.0*PI-w;
	v=acos((ve*rpos)/(Mag(ve)*Mag(rpos)));
	if((rdotv) < 0.0) v=2.0*PI-v;
	u=acos((n*rpos)/(Mag(n)*Mag(rpos)));
	if(rpos.z < 0) u=2.0*PI-u;
	l=om+u;
	a=p/(1.0-e*e);
	if(a > 0.0) {
		// circular or elliptical orbit...
		period=2.0*PI*sqrt((a*a*a)/mu);
		apo=a*(1.0+e)-bradius;
		eanom=2.0*atan(sqrt((1.0-e)/(1.0+e))*tan(v/2.0));
		manom=eanom-e*sin(eanom);
		tsp=period*(manom/(2*PI));
		ttp=period-tsp;
		if(rdotv < 0.0) ttp=-tsp;
		if(ttp > (period/2.0)) {
			tta=fabs((period/2.0)-ttp);
		} else {
			tta=fabs(ttp + period/2.0);
		}
	} else {
		// hyperbolic orbit no period, apo or tta...
		period=0.0;
		apo=0.0;
		tta=0.0;
		y=(e+cos(v))/(1.0+e*cos(v));
		eanom=log(y+sqrt(y*y-1.0));
//		eanom=acosh((e+cos(v))/(1.0+e*cos(v)));
		if(v < PI) {
			eanom=fabs(eanom);
		} else {
			eanom=-fabs(eanom);
		}
		manom=e*sinh(eanom)-eanom;
		ttp=-sqrt(pow(-a, 3)/mu)*(e*sinh(eanom)-eanom);
	}
//	sprintf(oapiDebugString(), "p=%.1f e=%.3f v=%.3f a=%.1f ttp=%.1f ",
//		p, e, v*DEG, a, ttp);
	per=a*(1.0-e)-bradius;
}
//	This is a simple Lambert solver based on BMW Chapter 5 - LazyD
//
//  It takes as input two position vectors and the time (seconds) to move from 
//  the first to the second, and mu for the gravitational reference.  The output 
//  is the velocities required at position 1 and the velocity at position 2.  
//
//	For Apollo rendezvous our TPI maneuvers are all less than 1/2 orbit, so we
//	only need to consider the "short way" solution.
//

void ApolloGuidance::Lambert(VECTOR3 &stpos, VECTOR3 &enpos, double dt, double mu, 
						  VECTOR3 &v1, VECTOR3 &v2)
{
	int k;
	double r1, r2, sa, angle, ca, zmin, zmax, z, c, s, y, x, t, sdz, cdz, tdz, f, g,
			gdot, zold;
		r1=Mag(stpos);
		r2=Mag(enpos);
		sa=(stpos/r1)*(enpos/r2);
		angle=PI/2.0-asin(sa);
		// we know this is a "short way" problem so this works
		// this would need to be changed for central angle of 180 or greater...
		ca=sqrt(r1*r2*(1.0+cos(angle)));
		zmin=-40.0;
		zmax=(PI*2.0)*(PI*2.0);
		z=(zmin+zmax)/2.0;
		zold=z;
		for (k=0; k<30; k++) {
			if (z > 0) {   //BMW 4.4.10 and .11
				c=((1 - cos(sqrt(z))) / z);
				s=((sqrt(z) - sin(sqrt(z))) / sqrt(pow(z, 3)));
			} else if (z < 0) {
				c=((1 - cosh(sqrt(-z))) / z);
				s=((sinh(sqrt(-z)) - sqrt(-z)) / sqrt(pow(-z, 3)));
			} else {
				c=0.5;
				s=1.0/6.0;
			}

			y=r1+r2-ca*((1.0-z*s)/sqrt(c)); //BMW 5.3.9
			x=sqrt(y/c);  //BMW 5.3.10
			t=((x*x*x)*s+ca*sqrt(y))/sqrt(mu);  //BMW 5.3.12
			sdz=(1.0/(2.0*z))*(c-3.0*s); //BMW 5.3.20
			cdz=(1.0/(2.0*z))*(1.0-z*s-2.0*c);  //BMW 5.3.21
			tdz=((x*x*x)*(sdz-(3.0*s*cdz)/(2.0*c))+(ca/8.0)*((3.0*s*sqrt(y))/c+ca/x))/sqrt(mu);
			zold=z;
			// choose a new z using Newton-Raphson
			z=z-(t-dt)/tdz;
			if(fabs(zold-z) < 0.000000001) break;
		}
		// we converged, so see what velocities z corresponds to
		f=1.0-y/r1; //BMW 5.3.13
		g=ca*sqrt(y/mu); //BMW 5.3.14
		gdot=1.0-y/r2;  //BMW 5.3.15
		v1=(enpos - stpos*f)/g;  //BMW 5.3.16
		v2=(enpos*gdot-stpos)/g; //BMW 5.3.17
}
/*
  Copyright 2003-2005 Chris Knestrick

  Code to solve the prediction problem - given the position and velocity
  vectors at some initial time, what will the position and velocity vectors
  be at specified time in the future. The method implemented here is
  described in Chapter 4 of "Fundementals of Astrodynamics" by Bate, Mueller,
  and White.
*/
// LazyD combined two functions into one
static inline void CalcCAndS(double &Z, double &C, double &S)
{
	if (Z > 0) {
		C = ((1 - cos(sqrt(Z))) / Z);
		S = ((sqrt(Z) - sin(sqrt(Z))) / sqrt(pow(Z, 3)));
	} else if (Z < 0) {
		C = ((1 - cosh(sqrt(-Z))) / Z);
		S = ((sinh(sqrt(-Z)) - sqrt(-Z)) / sqrt(pow(-Z, 3)));
	} else {
		C = 0.5; // probably doesn't matter, but this is correct - LazyD
		S = 1.0/6.0; // probably doesn't matter, but this is correct - LazyD
	}
}

// Required accuracy for the iterative computations
const double EPSILON = 0.000000001;

// Iterative method to calculate new X and Z values for the specified time of flight
static inline void CalcXandZ(double &X, double &Z, const VECTOR3 Pos, const VECTOR3 Vel,
							 double a, const double Time, const double SqrtMu)
{
	const double MAX_ITERS = 16;
	double C, S, T, dTdX, DeltaTime, r = Mag(Pos), IterNum = 0;

	// These don't change over the iterations
	double RVMu = (Pos * Vel) / SqrtMu;		// Dot product of position and velocity divided
											// by the squareroot of Mu
	double OneRA = (1 - (r / a));			// One minus Pos over the semi-major axis

	CalcCAndS(Z, C, S);
	T = ((RVMu * pow(X, 2) * C) +  (OneRA * pow(X, 3) * S) + (r * X)) / SqrtMu;

	DeltaTime = Time - T;

	// Iterate while the result isn't within tolerances
	while (fabs(DeltaTime) > EPSILON && IterNum++ < MAX_ITERS) {
		dTdX = ((pow(X, 2) * C) + (RVMu * X * (1 - Z * S)) + (r * (1 - Z * C))) / SqrtMu;

		X = X + (DeltaTime / dTdX);
		Z=(X*X)/a;

		CalcCAndS(Z, C, S);
		T = ((RVMu * pow(X, 2) * C) +  (OneRA * pow(X, 3) * S) + (r * X)) / SqrtMu;

		DeltaTime = Time - T;

	}


}

// Given the specified position and velocity vectors for a given orbit, retuns the position
// and velocity vectors after a specified time
void ApolloGuidance::PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel, double Mu,
						  double Time, VECTOR3 &NewPos, VECTOR3 &NewVel, double &NewVelMag)
{
	double SqrtMu = sqrt(Mu);
	double v=sqrt(Vel.x*Vel.x+Vel.y*Vel.y+Vel.z*Vel.z);
	double r=sqrt(Pos.x*Pos.x+Pos.y*Pos.y+Pos.z*Pos.z);
	double e=(v*v)/2.0-Mu/r;
	double a=-Mu/(2.0*e);

	// Variables for computation
	double X = (SqrtMu * Time) / a;					// Initial guesses for X
	double Z = (X*X)/a;							    // and Z
	double C, S;									// C(Z) and S(Z)
	double F, FDot, G, GDot;

	// Calculate the X and Z for the specified time of flight
	CalcXandZ(X, Z, Pos, Vel, a, Time, SqrtMu);

	// Calculate C(Z) and S(Z)
	CalcCAndS(Z, C, S);

	// Calculate the new position and velocity vectors
	F = 1.0 - (((X*X )/ r) * C);
	G = Time - (((X*X*X) / SqrtMu) * S);
	NewPos = (Pos * F) + (Vel * G);

	FDot = (SqrtMu / (r * Mag(NewPos))) * X * (Z * S - 1.0);
	GDot = 1.0 - (((X*X) / Mag(NewPos)) * C);
	NewVel = (Pos * FDot) + (Vel * GDot);
	NewVelMag = Mag(NewVel);
}
VECTOR3 ApolloGuidance::OrientAxis(VECTOR3 &vec, int axis, int ref, double gainFactor)
{
	//axis=0=x, 1=y, 2=z
	//ref =0 body coordinates 1=local vertical
	//orients a vessel axis with a body-relative normalized vector
	//allows rotation about the axis being oriented
	const double RATE_MAX = RAD*(5.0);
	const double DEADBAND_LOW = RAD*(0.01);
	const double RATE_FINE = RAD*(0.005);
	const double RATE_NULL = RAD*(0.0001);

	VECTOR3 PMI, Level, Drate, delatt, Rate, zerogl, xgl, ygl, zgl, norm, pos, vel, up,
		left, forward;
	double Mass, Size, MaxThrust, Thrust, Rdead, factor, xa, ya, za, v, rmax, denom;
	int i, n, k;

	VESSELSTATUS status2;
	OurVessel->GetStatus(status2);
	OurVessel->GetPMI(PMI);
	Mass=OurVessel->GetMass();
	n=OurVessel->DockCount();
	Size=OurVessel->GetSize();
	rmax=RATE_MAX;
	denom=3.0;
	for (k=0; k<n; k++) {
		DOCKHANDLE Dock=OurVessel->GetDockHandle(k);
		i=OurVessel->DockingStatus(k);
		if(i == 0) break;
		Size=Size*2.0;
		OBJHANDLE hOther=OurVessel->GetDockStatus(Dock);
		VESSEL *Other=oapiGetVesselInterface(hOther);
		v=Other->GetMass();
		Mass=Mass+v;
		if (MainThrusterIsHover) {
		// for the LM docked to the CSM
			rmax=RATE_MAX/3.0;
			PMI.x=165.0;
			PMI.y=PMI.y+7.0;
			PMI.z=165.0;
		} else {
		// for the CSM docked to the LM
//			PMI.x=PMI.x*15.0;
//			PMI.y=PMI.y*15.0;
			PMI.x=180.0;
			PMI.y=180.0;
			PMI.z=PMI.z+2.37;
		}
		denom=10.0;
	}
	MaxThrust=OurVessel->GetMaxThrust(ENGINE_ATTITUDE);
	factor = gainFactor;
	
	OurVessel->Local2Global(_V(0.0, 0.0, 0.0), zerogl);
	OurVessel->Local2Global(_V(1.0, 0.0, 0.0), xgl);
	OurVessel->Local2Global(_V(0.0, 1.0, 0.0), ygl);
	OurVessel->Local2Global(_V(0.0, 0.0, 1.0), zgl);
	xgl=xgl-zerogl;
	ygl=ygl-zerogl;
	zgl=zgl-zerogl;
	norm=Normalize(vec);
	if(ref == 1) {
		// vec is in local vertical reference, change to body rel coords
		// vec.x=forward component
		// vec.y=up component
		// vec.z=left component
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		OurVessel->GetRelativePos(hbody, pos);
		OurVessel->GetRelativeVel(hbody, vel);
		up=Normalize(pos);
		left=Normalize(CrossProduct(pos, vel));
		forward=Normalize(CrossProduct(left, up));
		norm=forward*vec.x+up*vec.y+left*vec.z;	
	}
	if(axis == 0) {
		xa=norm*xgl;
		xa=xa/fabs(xa);
		ya=asin(norm*ygl);
		za=asin(norm*zgl);
		if(xa < 0.0) {
			ya=(ya/fabs(ya))*PI-ya;
			za=(za/fabs(za))*PI-za;
		}
		delatt.x=0.0;
		delatt.y=za;
		delatt.z=-ya;
	}
	if(axis == 1) {
		xa=asin(norm*xgl);
		ya=norm*ygl;
		ya=ya/fabs(ya);
		za=asin(norm*zgl);
		if(ya < 0.0) {
			xa=(xa/fabs(xa))*PI-xa;
			za=(za/fabs(za))*PI-za;
		}
		delatt.x=-za;
		delatt.y=0.0;
		delatt.z=xa;
//		fprintf(outstr, "delatt=%.3f %.3f %.3f xyz=%.3f %.3f %.3f\n", delatt*DEG, 
//			xa*DEG, ya*DEG, za*DEG);
	}
	if(axis == 2) {
		xa=asin(norm*xgl);
		ya=asin(norm*ygl);
		za=norm*zgl;
		za=za/fabs(za);
		if(za < 0.0) {
			xa=(xa/fabs(xa))*PI-xa;
			ya=(ya/fabs(ya))*PI-ya;
		}
		delatt.x=ya;
		delatt.y=-xa;
		delatt.z=0.0;
	}
//	sprintf(oapiDebugString(), "norm=%.3f %.3f %.3f x=%.3f y=%.3f z=%.3f ax=%d", 
//	norm, xa*DEG, ya*DEG, za*DEG, axis);


//X axis
	if (fabs(delatt.x) < DEADBAND_LOW) {
		if(fabs(status2.vrot.x) < RATE_NULL) {
		// set level to zero
			Level.x=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.x*status2.vrot.x)/Size;
			Level.x = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.x=fabs(delatt.x)/denom;
		if(Rate.x < RATE_FINE) Rate.x=RATE_FINE;
		if (Rate.x > rmax ) Rate.x=rmax;
		Rdead=min(Rate.x/2,RATE_FINE);
		if(delatt.x < 0) {
			Rate.x=-Rate.x;
			Rdead=-Rdead;
		}
		Drate.x=Rate.x-status2.vrot.x;
		Thrust=factor*(Mass*PMI.x*Drate.x)/Size;
		if(delatt.x > 0) {
			if(Drate.x > Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else if (Drate.x < -Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else {
				Level.x=0;
			}
		} else {
			if(Drate.x < Rdead) {
				Level.x=max((Thrust/MaxThrust),-1);
			} else if (Drate.x > -Rdead) {
				Level.x=min((Thrust/MaxThrust),1);
			} else {
				Level.x=0;
			}
		}
	}
//	fprintf(outstr, "del=%.5f rate=%.5f drate=%.5f lvl=%.5f vrot=%.5f\n",
//		delatt.x*DEG, Rate.x*DEG, Drate.x*DEG, Level.x, status2.vrot.x*DEG);

//	sprintf(oapiDebugString(), "del=%.5f rate=%.5f drate=%.5f lvl=%.5f pmi=%.5f th=%.3f vr=%.3f",
//		delatt.x*DEG, Rate.x*DEG, Drate.x*DEG, Level.x, PMI.x, Thrust, status2.vrot.x*DEG);

//Y-axis
	if (fabs(delatt.y) < DEADBAND_LOW) {
		if(fabs(status2.vrot.y) < RATE_NULL) {
		// set level to zero
			Level.y=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.y*status2.vrot.y)/Size;
			Level.y = min((Thrust/MaxThrust), 1);
		}
//		sprintf(oapiDebugString(),"yrate=%.5f level%.5f", status2.vrot.y, Level.y);
	} else {
		Rate.y=fabs(delatt.y)/denom;
		if(Rate.y < RATE_FINE) Rate.y=RATE_FINE;
		if (Rate.y > rmax ) Rate.y=rmax;
		Rdead=min(Rate.y/2,RATE_FINE);
		if(delatt.y < 0) {
			Rate.y=-Rate.y;
			Rdead=-Rdead;
		}
		Drate.y=Rate.y-status2.vrot.y;
		Thrust=factor*(Mass*PMI.y*Drate.y)/Size;
		if(delatt.y > 0) {
			if(Drate.y > Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else if (Drate.y < -Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else {
				Level.y=0;
			}
		} else {
			if(Drate.y < Rdead) {
				Level.y=max((Thrust/MaxThrust),-1);
			} else if (Drate.y > -Rdead) {
				Level.y=min((Thrust/MaxThrust),1);
			} else {
				Level.y=0;
			}
		}
	}

//Z axis
	if (fabs(delatt.z) < DEADBAND_LOW) {
		if(fabs(status2.vrot.z) < RATE_NULL) {
		// set level to zero
			Level.z=0;
		} else {
		// null the rate
			Thrust=-(Mass*PMI.z*status2.vrot.z)/Size;
			Level.z = min((Thrust/MaxThrust), 1);
		}
	} else {
		Rate.z=fabs(delatt.z)/denom;
		if(Rate.z < RATE_FINE) Rate.z=RATE_FINE;
		if (Rate.z > rmax ) Rate.z=rmax;
		Rdead=min(Rate.z/2,RATE_FINE);
		if(delatt.z< 0) {
			Rate.z=-Rate.z;
			Rdead=-Rdead;
		}
		Drate.z=Rate.z-status2.vrot.z;
		Thrust=factor*(Mass*PMI.z*Drate.z)/Size;
		if(delatt.z > 0) {
			if(Drate.z > Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else if (Drate.z < -Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else {
				Level.z=0;
			}
		} else {
			if(Drate.z < Rdead) {
				Level.z=max((Thrust/MaxThrust),-1);
			} else if (Drate.z > -Rdead) {
				Level.z=min((Thrust/MaxThrust),1);
			} else {
				Level.z=0;
			}
		}
	}
	SetAttitudeRotLevel(Level);
	return Level;
}

void ApolloGuidance::Prog37(double simt)

{
	double pcthrust;

	switch (ProgState)
	{
	case 0:
		BlankData();
		SetVerbNounAndFlash(6, 44);
		ProgState++;
		break;

	case 2:
	case 52:
		NextEventTime = simt + 1.0;
		DoOrbitBurnCalcs(simt);
		SetVerbNounAndFlash(6, 95);
		ProgState++;
		break;

	//
	// Perform the burn calculations here.
	//

	case 3:
	case 53:
		if (simt >= NextEventTime) {
			DoOrbitBurnCalcs(simt);
			if (simt >= BurnStartTime - 300.0) {
				if (NextEventTime >= BurnStartTime - 180.0) {
					RaiseAlarm(0603);
				}
				VerbRunning = 0;
				BlankAll();
				NextEventTime = simt + 5.0;
				ProgState++;
			}
			else
				NextEventTime = simt + 1.0;
		}
		break;

	//
	// Unblank display.
	//

	case 4:
	case 54:
		if (simt >= NextEventTime) {
			UnBlankAll();
			SetVerbNounAndFlash(16, 95);
			ProgState++;
			NextEventTime = simt + 1.0;
		}
		break;


	//
	// Orient for the burn.
	//

	case 5:
	case 55:
		NextEventTime=0.0;
		OrientForOrbitBurn(simt);
		if (simt >= BurnStartTime)
			ProgState++;
		break;

	//
	// Ignition.
	//

	case 6:
	case 56:
		if (simt >= BurnStartTime) {
			OrientForOrbitBurn(simt);
			pcthrust=1.0;
			if(MainThrusterIsHover) pcthrust=0.1;
			BurnMainEngine(pcthrust);
			NextEventTime = simt + 0.5;
			ProgState++;
		}
		break;

	//
	// Wait for shutdown.
	//

	case 7:
	case 57:
		double CurrentAlt, TargetAlt;
		double CurrentAp, CurrentPer;
		OBJHANDLE hPlanet;

		OrientForOrbitBurn(simt);
		pcthrust=1.0;
		if(MainThrusterIsHover) pcthrust=0.1;
		BurnMainEngine(pcthrust);

		//
		// Get the current and target values.
		//
		// This is actually somewhat complicated. The apogee and perigee can swap
		// part-way through the burn, if the final desired apogee is lower than the
		// current perigee, or the final perigee is higher than the current apogee.
		//

		hPlanet = OurVessel->GetPeDist(CurrentPer);
		OurVessel->GetApDist(CurrentAp);
		if (ProgFlag01) {
			TargetAlt = oapiGetSize(hPlanet) + (DesiredPerigee * 1000.0);
			//
			// We're aiming for the appropriate perigee. If the desired perigee
			// is actually greater than the current apogee we need to compare it
			// to that, not to the current perigee.
			//
			if (TargetAlt > CurrentAp) {
				CurrentAlt = CurrentAp;
			} else {
				CurrentAlt = CurrentPer;
			}
		}
		else {
			TargetAlt = oapiGetSize(hPlanet) + (DesiredApogee * 1000.0);
			//
			// We're aiming for the appropriate apogee. If the desired apogee
			// is lower than the current apogee, then we need to compare it
			// to the current perigee, not the current apogee.
			//
			if (TargetAlt < CurrentAp) {
				CurrentAlt = CurrentPer;
			} else {
				CurrentAlt = CurrentAp;
			}
		}

		//
		// If retrograde, we need to check when we're less than or equal
		// to the correct value, otherwise greater than or equal to.

		bool DoShutdown;

		DoShutdown = false;
		if (RetroFlag) {
			if (CurrentAlt <= (TargetAlt + 10))
				DoShutdown = true;
		} else {
			if (CurrentAlt >= (TargetAlt - 10))
				DoShutdown = true;
		}

		//
		// Sanity check. This shouldn't be required, but there are still some
		// bugs in the handling of special cases here.
		//

#if 1
		if (simt >= BurnEndTime + 0.5)
			DoShutdown = true;
#endif

		if (DoShutdown) {
			BurnMainEngine(0.0);
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			ProgState++;
		}

		NextEventTime = simt + 0.1;
		break;

	//
	// Now move on to sort out the other side of the orbit.
	//

	case 8:
		ProgState = 52;
		break;

	//
	// Done.
	//

	case 58:
		AwaitProgram();
		ProgState++;
		break;
	}
}

void ApolloGuidance::ResetProg(double simt)

{
	switch (ProgState) {

	case 0:
		BlankAll();
		NextEventTime = simt;
		ProgState++;
		break;

	case 2:
		NextEventTime = simt;

		LightUplink();
		LightNoAtt();
		dsky.LightStby();
		if (dsky2) dsky2->LightStby();
		LightKbRel();
		LightOprErr();
		LightTemp();
		LightGimbalLock();
		dsky.LightRestart();
		if (dsky2) dsky2->LightRestart();
		LightTracker();
		LightProg();

		UnBlankAll();

		ClearVerbNounFlashing();

		ResetCount = 9;
		ResetCountdown();

		ProgState++;
		break;

	case 4:
		NextEventTime = simt;
		ClearUplink();
		ResetCountdown();
		ProgState++;
		break;

	case 6:
		NextEventTime = simt;
		ClearNoAtt();
		ResetCountdown();
		ProgState++;
		break;

	case 8:
		NextEventTime = simt;
		dsky.ClearStby();
		if (dsky2) dsky2->ClearStby();
		ResetCountdown();
		ProgState++;
		break;

	case 10:
		NextEventTime = simt;
		ClearKbRel();
		ResetCountdown();
		ProgState++;
		break;

	case 12:
		NextEventTime = simt;
		ClearOprErr();
		ResetCountdown();
		ProgState++;
		break;

	case 14:
		NextEventTime = simt;
		ClearTemp();
		ResetCountdown();
		ProgState++;
		break;

	case 16:
		NextEventTime = simt;
		ClearGimbalLock();
		ResetCountdown();
		ProgState++;
		break;

	case 18:
		NextEventTime = simt;
		ClearProg();
		ResetCountdown();
		ProgState++;
		break;

	case 20:
		NextEventTime = simt;
		dsky.ClearRestart();
		if (dsky2) dsky2->ClearRestart();
		ResetCountdown();

		SetVerbNounFlashing();

		ProgState++;
		break;

	case 22:
		NextEventTime = simt;
		ResetTime = simt;

		ClearTracker();
		ProgState++;

		//
		// Ok, now we're done. Set Reset state and blank the displays.
		//

		Reset = true;

		BlankAll();
		ClearVerbNounFlashing();

		//
		// And drop to standby.
		//

		GoStandby();

		break;

	default:

		//
		// In a real Apollo the program would wait 5.12 seconds between
		// steps, but here we'll wait half that to save boring people :).
		//

		if (simt > (NextEventTime + 2.56))
			ProgState++;
		break;
	}

}

void ApolloGuidance::Prog37Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	case 1:
		if (R1 > R2) {
			DesiredApogee = R1 * 0.1;
			DesiredPerigee = R2 * 0.1;
			ProgState++;
		}
		else {
			LightOprErr();
		}
		break;
	}
}

void ApolloGuidance::DisplayEMEM(unsigned int addr)

{
	int val;

	if (ReadMemory(addr, val))
		SetR2Octal(val);
	else
		SetR2Octal(077777);
}

//
// Virtual AGC Erasable memory functions.
//
// Currenty do nothing.
//


int ApolloGuidance::GetErasable(int bank, int address)

{
	if (bank < 0 || bank > 8)
		return 0;
	if (address < 0 || address > 0400)
		return 0;

	return vagc.Erasable[bank][address];
}

void ApolloGuidance::SetErasable(int bank, int address, int value)

{
	if (bank < 0 || bank > 8)
		return;
	if (address < 0 || address > 0400)
		return;

	vagc.Erasable[bank][address] = value;
}

void ApolloGuidance::PulsePIPA(int RegPIPA, int pulses) 

{
	int i;
	//
	// No pulsing, Don't lock the thread mutex. Locking the mutex here slows time acceleration, like single thread.
	// There should not be any pulsing during time acceleration anyways. i.e. no thruster firing. Still locking
	// the mutex in case the agc does small maneuvering during time acceleration.
	//
	if (pulses == 0 ) 
		return;

	Lock lock(agcCycleMutex);


	if (pulses >= 0) {
    	for (i = 0; i < pulses; i++) {
			UnprogrammedIncrement(&vagc, RegPIPA, 0);	// PINC

    	}
	} else {
    	for (i = 0; i < -pulses; i++) {
			UnprogrammedIncrement(&vagc, RegPIPA, 2);	// MINC
    	}
	}

}

//
// PROG pressed.
//

bool ApolloGuidance::GenericProgPressed(int R1, int R2, int R3)

{
	switch (VerbRunning) {
	case 1:
		DisplayEMEM(R1);
	case 11:
		CurrentEMEMAddr = R1;
		return true;

	//
	// 21: update erasable memory.
	//

	case 21:
		if (NounRunning == 2) {
			WriteMemory(R1, R2);
			return true;
		}
		return false;

	case 91:
		BankSumNum++;
		DisplayBankSum();
		return true;
	}

	switch(ProgRunning) {

	case 17:
		Prog17Pressed(R1, R2, R3);
		return true;

	case 18:
		Prog18Pressed(R1, R2, R3);
		return true;

	case 19:
		Prog19Pressed(R1, R2, R3);
		return true;

	case 37:
		Prog37Pressed(R1, R2, R3);
		return true;
	}

	return false;
}

//
// A flashing verb 50 noun 25 is used to indicate a checklist option to
// the user... checklist number is in R1.
//

void ApolloGuidance::Checklist(int num)

{
	SetVerb(50);
	SetNoun(25);
	BlankData();
	SetR1Octal(num);
	SetVerbNounFlashing();
}

//
// Get position and velocity relative to local horizon.
//

void ApolloGuidance::GetPosVel()

{
	CurrentAlt = OurVessel->GetAltitude();

	VECTOR3	vHVel;
	if (OurVessel->GetHorizonAirspeedVector(vHVel)) {

		//
		// Horizon-relative velocity.
		//

		CurrentVel = sqrt(vHVel.x *vHVel.x + vHVel.y * vHVel.y + vHVel.z * vHVel.z);
		CurrentVelX = vHVel.x;
		CurrentVelY = vHVel.y;
		CurrentVelZ = vHVel.z;
	}
}

void ApolloGuidance::LaunchShutdown()

{
	InOrbit = true;
}

//
// State save/load routines.
//

//
// Note that this state structure is now full! If you want to add a bit you'll have to add it to
// a new structure.
//

typedef union

{
	struct {
		unsigned Reset:1;
		unsigned InOrbit:1;
		unsigned Standby:1;
		unsigned Units:1;
		unsigned R1Decimal:1;
		unsigned R2Decimal:1;
		unsigned R3Decimal:1;
		unsigned EnteringVerb:1;
		unsigned EnteringNoun:1;
		unsigned EnteringOctal:1;
		unsigned EnterPositive:1;
		unsigned ProgBlanked:1;
		unsigned VerbBlanked:1;
		unsigned NounBlanked:1;
		unsigned R1Blanked:1;
		unsigned R2Blanked:1;
		unsigned R3Blanked:1;
		unsigned KbInUse:1;
		unsigned isFirstTimestep:1;
		unsigned ExtraCode:1;
		unsigned AllowInterrupt:1;
		unsigned InIsr:1;
		unsigned SubstituteInstruction:1;
		unsigned PendFlag:1;
		unsigned PendDelay:3;
		unsigned ExtraDelay:3;
		unsigned DownruptTimeValid:1;
		unsigned PadLoaded:1;
	} u;
	unsigned long word;
} AGCState;

typedef union
{
	struct {
		unsigned NightWatchman : 1;
		unsigned RuptLock : 1;
		unsigned NoRupt : 1;
		unsigned TCTrap : 1;
		unsigned NoTC : 1;
		unsigned VAGCStandby : 1;
		unsigned SbyPressed : 1;
	} u;
	unsigned long word;
} AGCState2;


//
// Global variables in agc_engine.c which probably have to be saved, too
//

extern "C" {
	extern int NextZ;
	extern int ScalerCounter;
	extern int ChannelRoutineCount;
}

void ApolloGuidance::SaveState(FILEHANDLE scn)

{
	char fname[32], str[32], buffer[256];
	int i;
	int val;

	oapiWriteLine(scn, AGC_START_STRING);

	oapiWriteScenario_int (scn, "YAAGC", Yaagc ? 1 : 0);

	if (!Yaagc) {
		oapiWriteScenario_float (scn, "BRNTIME", BurnTime);
		oapiWriteScenario_float (scn, "BRNSTIME", BurnStartTime);
		oapiWriteScenario_float (scn, "BRNETIME", BurnEndTime);
		oapiWriteScenario_float (scn, "EVTTIME", NextEventTime);
		oapiWriteScenario_float (scn, "CUTFVEL", CutOffVel);

		if (InOrbit) {
			oapiWriteScenario_float (scn, "LALT", LastAlt);
			oapiWriteScenario_float (scn, "TGTDV", DesiredDeltaV);
		}

		oapiWriteScenario_int (scn, "PROG", Prog);
		oapiWriteScenario_int (scn, "VERB", Verb);
		oapiWriteScenario_int (scn, "NOUN", Noun);
		oapiWriteScenario_int (scn, "R1", R1);
		oapiWriteScenario_int (scn, "R2", R2);
		oapiWriteScenario_int (scn, "R3", R3);
		oapiWriteScenario_int (scn, "EPOS", EnterPos);
		oapiWriteScenario_int (scn, "EVAL", EnterVal);
		oapiWriteScenario_int (scn, "EDAT", EnteringData);
		oapiWriteScenario_int (scn, "ECNT", EnterCount);

		memset(str, 0, 32);

		strncpy(str, TwoDigitEntry, 2);
		oapiWriteScenario_string (scn, "E2", str);
		strncpy(str, FiveDigitEntry, 6);
		oapiWriteScenario_string (scn, "E5", str);

		oapiWriteScenario_string(scn, "R1FMT", R1Format);
		oapiWriteScenario_string(scn, "R2FMT", R2Format);
		oapiWriteScenario_string(scn, "R3FMT", R3Format);
	}

	oapiWriteScenario_float(scn, "TGTA", DesiredApogee);
	oapiWriteScenario_float(scn, "TGTP", DesiredPerigee);
	oapiWriteScenario_float(scn, "TGTZ", DesiredAzimuth);

	if (OtherVesselName[0])
		oapiWriteScenario_string(scn, "ONAME", OtherVesselName);

	//
	// Copy internal state to the structure.
	//

	AGCState state;

	state.word = 0;
	state.u.Reset = Reset;
	state.u.InOrbit = InOrbit;
	state.u.Standby = Standby;
	state.u.Units = (DisplayUnits == UnitImperial);
	state.u.R1Decimal = R1Decimal;
	state.u.R2Decimal = R2Decimal;
	state.u.R3Decimal = R3Decimal;
	state.u.EnteringVerb = EnteringVerb;
	state.u.EnteringNoun = EnteringNoun;
	state.u.EnteringOctal = EnteringOctal;
	state.u.EnterPositive = EnterPositive;
	state.u.ProgBlanked = ProgBlanked;
	state.u.VerbBlanked = VerbBlanked;
	state.u.NounBlanked = NounBlanked;
	state.u.R1Blanked = R1Blanked;
	state.u.R2Blanked = R2Blanked;
	state.u.R3Blanked = R3Blanked;
	state.u.KbInUse = KbInUse;
	state.u.isFirstTimestep = isFirstTimestep;
	state.u.ExtraCode = vagc.ExtraCode;
	state.u.AllowInterrupt = vagc.AllowInterrupt;
	state.u.InIsr = vagc.InIsr;
	state.u.SubstituteInstruction = vagc.SubstituteInstruction;
	state.u.PendFlag = vagc.PendFlag;
	state.u.PendDelay = vagc.PendDelay;
	state.u.ExtraDelay = vagc.ExtraDelay;
	state.u.DownruptTimeValid = vagc.DownruptTimeValid;
	state.u.PadLoaded = PadLoaded;

	oapiWriteScenario_int(scn, "STATE", state.word);

	AGCState2 state2;

	state2.u.NightWatchman = vagc.NightWatchman;
	state2.u.RuptLock = vagc.RuptLock;
	state2.u.NoRupt = vagc.NoRupt;
	state2.u.TCTrap = vagc.TCTrap;
	state2.u.NoTC = vagc.NoTC;
	state2.u.VAGCStandby = vagc.Standby;
	state2.u.SbyPressed = vagc.SbyPressed;

	oapiWriteScenario_int (scn, "VAGCSTATE2", state2.word);

	//
	// Write out any non-zero EMEM state.
	//

	for (i = 0; i < EMEM_ENTRIES; i++) {
		// Always save RegZ because it's set in agc_engine_init, so we have to store 0, too
		if (ReadMemory(i, val) && (val != 0 || i == RegZ)) {
			sprintf(fname, "EMEM%04o", i);
			sprintf(str, "%o", val);
			oapiWriteScenario_string (scn, fname, str);
		}
	}

	//
	// And non-zero I/O state.
	//

	for (i = 0; i < MAX_INPUT_CHANNELS; i++) {
		val = GetInputChannel(i);
		if (val != 0) {
			sprintf(fname, "ICHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	for (i = 0; i < MAX_OUTPUT_CHANNELS; i++) {
		val = GetOutputChannel(i);
		if (val != 0) {
			sprintf(fname, "OCHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}


	if (Yaagc) {
		for (i = 0; i < NUM_CHANNELS; i++) {
			val = vagc.InputChannel[i];
			// Always save channel 030 - 033 because they're set in agc_engine_init, so we have to store 0, too
			if (val != 0 || (i >= 030 && i <= 033)) {
				sprintf(fname, "VICHAN%03d", i);
				oapiWriteScenario_int (scn, fname, val);
			}
		}

		oapiWriteScenario_int (scn, "VOC7", vagc.OutputChannel7);
		oapiWriteScenario_int (scn, "IDXV", vagc.IndexValue);
		oapiWriteScenario_int (scn, "NEXTZ", NextZ);
		oapiWriteScenario_int (scn, "SCALERCOUNTER", ScalerCounter);
		oapiWriteScenario_int (scn, "CRCOUNT", ChannelRoutineCount);
		oapiWriteScenario_int (scn, "CH33SWITCHES", vagc.Ch33Switches);

		sprintf(buffer, "  CYCLECOUNTER %I64d", vagc.CycleCounter);
		oapiWriteLine(scn, buffer);
		
		for (i = 0; i < 16; i++) {
			val = vagc.OutputChannel10[i];
			sprintf(fname, "V10CHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}

		for (i = 0; i < (1 + NUM_INTERRUPT_TYPES); i++) {
			val = vagc.InterruptRequests[i];
			sprintf(fname, "VINT%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}
	papiWriteScenario_bool(scn, "PROGALARM", ProgAlarm);
	papiWriteScenario_bool(scn, "GIMBALLOCKALARM", GimbalLockAlarm);

	oapiWriteLine(scn, AGC_END_STRING);
}

void ApolloGuidance::LoadState(FILEHANDLE scn)

{
	char	*line;
	float	flt;

	//
	// Now load the data.
	//

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, AGC_END_STRING, sizeof(AGC_END_STRING)))
			break;
			
		if (!strnicmp (line, "PROGSTATE", 9)) {
			sscanf (line+9, "%d", &ProgState);
		}
		else if (!strnicmp (line, "RCOUNT", 6)) {
			sscanf (line+6, "%d", &ResetCount);
		}
		else if (!strnicmp (line, "ALRM01", 6)) {
			sscanf (line+6, "%d", &Alarm01);
		}
		else if (!strnicmp (line, "ALRM02", 6)) {
			sscanf (line+6, "%d", &Alarm02);
		}
		else if (!strnicmp (line, "ALRM03", 6)) {
			sscanf (line+6, "%d", &Alarm03);
		}
		else if (!strnicmp (line, "BRNTIME", 7)) {
			sscanf (line+7, "%f", &flt);
			BurnTime = flt;
		}
		else if (!strnicmp (line, "BRNSTIME", 8)) {
			sscanf (line+8, "%f", &flt);
			BurnStartTime = flt;
		}
		else if (!strnicmp (line, "BRNETIME", 8)) {
			sscanf (line+8, "%f", &flt);
			BurnEndTime = flt;
		}
		else if (!strnicmp (line, "EVTTIME", 7)) {
			sscanf (line+7, "%g", &flt);
			NextEventTime = flt;
		}
		else if (!strnicmp (line, "CUTFVEL", 7)) {
			sscanf (line+7, "%g", &flt);
			CutOffVel = flt;
		}
		else if (!strnicmp (line, "LALT", 4)) {
			sscanf (line+4, "%f", &flt);
			LastAlt = flt;
		}
		else if (!strnicmp (line, "TGTA", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredApogee = flt;
		}
		else if (!strnicmp (line, "TGTP", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredPerigee = flt;
		}
		else if (!strnicmp (line, "TGTZ", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredAzimuth = flt;
		}
		else if (!strnicmp (line, "TGTDV", 5)) {
			sscanf (line+5, "%f", &flt);
			DesiredDeltaV = flt;
		}
		else if (!strnicmp (line, "BKSUM", 5)) {
			sscanf (line+5, "%d", &BankSumNum);
		}
		else if (!strnicmp (line, "EMEM", 4)) {
			int num, val;
			sscanf(line+4, "%o", &num);
			sscanf(line+9, "%o", &val);
			WriteMemory(num, val);
		}
		else if (!strnicmp (line, "ICHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line+5, "%d", &num);
			sscanf(line+9, "%d", &val);
			InputChannel[num] = val;
		}
		else if (!strnicmp (line, "VICHAN", 6)) {
			int num;
			unsigned int val;
			sscanf(line+6, "%d", &num);
			sscanf(line+10, "%d", &val);
			vagc.InputChannel[num] = val;
		}
		else if (!strnicmp (line, "V10CHAN", 7)) {
			int num;
			unsigned int val;
			sscanf(line+7, "%d", &num);
			sscanf(line+11, "%d", &val);
			vagc.OutputChannel10[num] = val;
		}
		else if (!strnicmp (line, "OCHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line+5, "%d", &num);
			sscanf(line+9, "%d", &val);
			OutputChannel[num] = val;
		}
		else if (!strnicmp (line, "VOC7", 4)) {
			sscanf (line+4, "%" SCNd16, &vagc.OutputChannel7);
		}
		else if (!strnicmp (line, "IDXV", 4)) {
			sscanf (line+4, "%" SCNd16, &vagc.IndexValue);
		}
		else if (!strnicmp (line, "NEXTZ", 5)) {
			sscanf (line+5, "%d", &NextZ);
		}
		else if (!strnicmp (line, "SCALERCOUNTER", 13)) {
			sscanf (line+13, "%d", &ScalerCounter);
		}
		else if (!strnicmp (line, "CRCOUNT", 7)) {
			sscanf (line+7, "%d", &ChannelRoutineCount);
		}
		else if (!strnicmp (line, "CH33SWITCHES", 12)) {
			sscanf (line+12, "%" SCNd16, &vagc.Ch33Switches);
		}
		/*
		TODO Do NOT load CycleCounter until CduFifos are saved/loaded, too
		else if (!strnicmp (line, "CYCLECOUNTER", 12)) {
			sscanf (line+12, "%I64d", &vagc.CycleCounter);
		}
		*/
		else if (!strnicmp (line, "VINT", 4)) {
			int num;
			unsigned int val;
			sscanf(line+4, "%d", &num);
			sscanf(line+8, "%d", &val);
			vagc.InterruptRequests[num] = val;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			AGCState state;
			sscanf (line+5, "%d", &state.word);

			Reset = state.u.Reset;
			InOrbit = state.u.InOrbit;
			Standby = state.u.Standby;
			DisplayUnits = state.u.Units ? UnitImperial: UnitMetric;
			R1Decimal = (state.u.R1Decimal != 0);
			R2Decimal = (state.u.R2Decimal != 0);
			R3Decimal = (state.u.R3Decimal != 0);
			EnteringVerb = (state.u.EnteringVerb != 0);
			EnteringNoun = (state.u.EnteringNoun != 0);
			EnteringOctal = (state.u.EnteringOctal != 0);
			EnterPositive = (state.u.EnterPositive != 0);
			ProgBlanked = (state.u.ProgBlanked != 0);
			VerbBlanked = (state.u.VerbBlanked != 0);
			NounBlanked = (state.u.NounBlanked != 0);
			R1Blanked = (state.u.R1Blanked != 0);
			R2Blanked = (state.u.R2Blanked != 0);
			R3Blanked = (state.u.R3Blanked != 0);
			KbInUse = (state.u.KbInUse != 0);
			isFirstTimestep = (state.u.isFirstTimestep != 0);
			vagc.ExtraCode = state.u.ExtraCode;
			vagc.AllowInterrupt = state.u.AllowInterrupt;
			vagc.InIsr = state.u.InIsr;
			vagc.SubstituteInstruction = state.u.SubstituteInstruction;
			vagc.PendFlag = state.u.PendFlag;
			vagc.PendDelay = state.u.PendDelay;
			vagc.ExtraDelay = state.u.ExtraDelay;
			vagc.DownruptTimeValid = state.u.DownruptTimeValid;
			PadLoaded = state.u.PadLoaded;
		}
		else if (!strnicmp(line, "VAGCSTATE2", 10)) {
			AGCState2 state2;
			sscanf(line + 10, "%d", &state2.word);

			vagc.NightWatchman = state2.u.NightWatchman;
			vagc.RuptLock = state2.u.RuptLock;
			vagc.NoRupt = state2.u.NoRupt;
			vagc.TCTrap = state2.u.TCTrap;
			vagc.NoTC = state2.u.NoTC;
			vagc.Standby = state2.u.VAGCStandby;
			vagc.SbyPressed = state2.u.SbyPressed;
		}
		else if (!strnicmp (line, "ONAME", 5)) {
			strncpy (OtherVesselName, line + 6, 64);
		}
		else if (!strnicmp (line, "YAAGC", 5)) {
			int is_virtual = 0;
			sscanf (line+5, "%d", &is_virtual);
			Yaagc = (is_virtual != 0);
		}
		else if (!strnicmp (line, "PROG", 4)) {
			sscanf (line+4, "%d", &Prog);
		}
		else if (!strnicmp (line, "VERB", 4)) {
			sscanf (line+4, "%d", &Verb);
		}
		else if (!strnicmp (line, "NOUN", 4)) {
			sscanf (line+4, "%d", &Noun);
		}
		else if (!strnicmp (line, "R1FMT", 5)) {
			strncpy (R1Format, line + 6, 6);
		}
		else if (!strnicmp (line, "R2FMT", 5)) {
			strncpy (R2Format, line + 6, 6);
		}
		else if (!strnicmp (line, "R3FMT", 5)) {
			strncpy (R3Format, line + 6, 6);
		}
		else if (!strnicmp (line, "R1", 2)) {
			sscanf (line+2, "%d", &R1);
		}
		else if (!strnicmp (line, "R2", 2)) {
			sscanf (line+2, "%d", &R2);
		}
		else if (!strnicmp (line, "R3", 2)) {
			sscanf (line+2, "%d", &R3);
		}
		else if (!strnicmp (line, "EPOS", 4)) {
			sscanf (line+4, "%d", &EnterPos);
		}
		else if (!strnicmp (line, "EVAL", 4)) {
			sscanf (line+4, "%d", &EnterVal);
		}
		else if (!strnicmp (line, "EDAT", 4)) {
			sscanf (line+4, "%d", &EnteringData);
		}
		else if (!strnicmp (line, "ECNT", 4)) {
			sscanf (line+4, "%d", &EnterCount);
		}
		else if (!strnicmp (line, "E2", 2)) {
			strncpy (TwoDigitEntry, line + 3, 2);
		}
		else if (!strnicmp (line, "E5", 2)) {
			strncpy (FiveDigitEntry, line + 3, 6);
		}
		papiReadScenario_bool(line, "PROGALARM", ProgAlarm);
		papiReadScenario_bool(line, "GIMBALLOCKALARM", GimbalLockAlarm);
	}

	//
	// Quick hack to make the code work with old scenario files. Can be removed after NASSP 7
	// release.
	//

	if (!OtherVesselName[0] && OurVessel) {
		strncpy (OtherVesselName, OurVessel->GetName(), 63);
		OtherVesselName[6] = 0;
	}
}

//
// Power.
//

bool ApolloGuidance::IsPowered()

{
	if (DCPower.Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	//
	// Quick hack for now: if no power connected, pretend we
	// have power.
	//

	if (!PowerConnected)
		return true;

	return false;
}

//
// I/O channel support code.
//
// Note that the AGC 'bit 1' is actually 'bit 0' in today's terminology, so we have
// to adjust the bit number here to match the real AGC.
//

bool ApolloGuidance::GetOutputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return false;

	int val = OutputChannel[channel];
	return (OutputChannel[channel] & (1 << (bit))) != 0;
}

unsigned int ApolloGuidance::GetOutputChannel(int channel)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return 0;

	return OutputChannel[channel];
}

void ApolloGuidance::SetInputChannel(int channel, std::bitset<16> val) 
{
	if (channel >= 0 && channel <= MAX_INPUT_CHANNELS)
		InputChannel[channel] = val.to_ulong();

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	if (Yaagc) {

#ifdef _DEBUG
	//
	// Don't print debug for IMU channels or we get a multi-gigabyte log file!
	//
	if (!(channel & 0x80))
		fprintf(out_file, "Wrote %05o to input channel %04o\n", channel, val);
#endif

		if (channel & 0x80) {
			// In this case we're dealing with a counter increment.
			// So increment the counter.
			UnprogrammedIncrement (&vagc, channel, val.to_ulong());
		}
		else {
			// If this is a keystroke from the DSKY, generate an interrupt req.
			if (channel == 015){
				vagc.InterruptRequests[5] = 1;
			}else{ if (channel == 016){ // Secondary DSKY
				vagc.InterruptRequests[6] = 1;
			}}

			//
			// Channels 030-034 are inverted!
			//

			if (channel >= 030 && channel <= 034){
				val ^= 077777;
			}

			// Standby stuff
			if (channel == 032 && 0 != (val.to_ulong() & 020000))
			{
				vagc.SbyPressed = 0;
			}

			WriteIO(&vagc, channel, val.to_ulong());
		}
	}
	else {
		switch (channel) {
		case 015:
			ProcessInputChannel15(val);
			break;
		//
		// Channel 016 is slightly different to 015, but for now we'll
		// just call the same function.
		//
		case 016:
			ProcessInputChannel15(val);
			break;
		}
	}
}

void ApolloGuidance::SetInputChannelBit(int channel, int bit, bool val)

{
	unsigned int mask = (1 << (bit));
	int	data = InputChannel[channel];

	if (Yaagc) {

		data = vagc.InputChannel[channel];
		//
		// Channels 030-034 are inverted!
		//

		if ((channel >= 030) && (channel <= 034))
			data ^= 077777;

#ifdef _DEBUG
		fprintf(out_file, "Set bit %d of input channel %04o to %d\n", bit, channel, val ? 1 : 0); 
#endif
	}

	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return;

	if (val) {
		data |= mask;
	}
	else {
		data &= ~mask;
	}

	InputChannel[channel] = data;

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	if (Yaagc) {
		//
		// Channels 030-034 are inverted!
		//

		if ((channel >= 030) && (channel <= 034))
			data ^= 077777;

		// Channel 33 special hack
		if(channel == 033){
			if(bit == 10){
				// Update channel 33 switch bits
				int ch33bits = GetCh33Switches();
				if(val != 0){ ch33bits |= 001000; }else{ ch33bits &= 076777; }
					SetCh33Switches(ch33bits);
				// We're done here. SetCh33Switches rewrites the IO channel.
				return;
			}
		}

		// If this is a keystroke from the DSKY (Or MARK/MARKREJ), generate an interrupt req.
		if (channel == 015 && val != 0){
			vagc.InterruptRequests[5] = 1;
		}else{ if (channel == 016 && val != 0){ // Secondary DSKY
			vagc.InterruptRequests[6] = 1;
		}}

		WriteIO(&vagc, channel, data);

	}
	else {
		switch (channel) {
		case 030:
			ProcessInputChannel30(bit, val);
			break;

		case 032:
			ProcessInputChannel32(bit, val);
			break;
		}
	}
}

void ApolloGuidance::ProcessInputChannel30(int bit, bool val)

{	
	if (bit == 14) {	// Answer to ISSTurnOnRequest
		if (val) {
			ChannelValue val12;
	    	val12 = 0;
			val12[ISSTurnOnDelayComplete] = 1;
			imu.ChannelOutput(012, val12);
		}
	}
}

void ApolloGuidance::SetOutputChannel(int channel, ChannelValue val)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return;

	OutputChannel[channel] = val.to_ulong();

#ifdef _DEBUG
	if (Yaagc) {
		switch (channel) {
		case 010:
		case 034:
		case 035:
		case 01:
		case 02:
			break;

		default:
			fprintf(out_file, "AGC write %05o to %04o\n", val, channel);
			break;
		}
	}
#endif

	//
	// Special-case processing.
	//

	switch (channel)
	{
	case 05:
		ProcessChannel5(val);
		break;

	case 06:
		ProcessChannel6(val);
		break;

	case 010:
		ProcessChannel10(val);
		break;

	case 011:
		ProcessChannel11(val);
		break;

	case 013:
		ProcessChannel13(val);
		break;

	case 014:
		{
			// DS20060225 Enable SPS gimbal control
			// TVC Enable does not disconnect the IMU from this channel			
			// (Even though it probably doesn't matter)
			imu.ChannelOutput(channel, val);
			// DS20060829 Allow other stuff too
			ProcessChannel14(val);
		}
		break;

	// Various control bits
	case 012:		
	// 174-177 are ficticious channels with the IMU CDU angles.
	case 0174:  // FDAI ROLL CHANNEL
	case 0175:  // FDAI PITCH CHANNEL
	case 0176:  // FDAI YAW CHANNEL
	case 0177:		
		ProcessIMUCDUErrorCount(channel, val);
		imu.ChannelOutput(channel, val);
		break;

	// DS20060225 Enable SPS gimbal control
	// Ficticious channels 160 & 161 have the optics shaft & trunion angles.
	case 0160:
		ProcessChannel160(val);
		break;

	case 0161:		
		ProcessChannel161(val);
		break;
	case 0162:
		ProcessChannel162(val);
		break;
	case 0163:
		ProcessChannel163(val);
		break;

	case 033: 
		/* No longer needed DS20061226
		{			
			ChannelValue33 val33;
			val33.Value = val;
		} */
		break;
	}
}

//
// By default, do nothing for the RCS channels.
//

void ApolloGuidance::ProcessChannel5(ChannelValue val){
}

void ApolloGuidance::ProcessChannel6(ChannelValue val){
}

// DS20060226 Stubs for optics controls and TVC
void ApolloGuidance::ProcessChannel14(ChannelValue val){
}

void ApolloGuidance::ProcessChannel160(ChannelValue val){
}

void ApolloGuidance::ProcessChannel161(ChannelValue val){
}

// Stub for LGC thrust drive
void ApolloGuidance::ProcessChannel162(ChannelValue val) {
}

// Stub for LGC altitude meter drive
void ApolloGuidance::ProcessChannel163(ChannelValue val) {
}

// DS20060308 Stub for FDAI
void ApolloGuidance::ProcessIMUCDUErrorCount(int channel, ChannelValue val){
}

void ApolloGuidance::GenerateHandrupt() {
	GenerateHANDRUPT(&vagc);
}

// DS20060402 DOWNRUPT
void ApolloGuidance::GenerateDownrupt(){
	GenerateDOWNRUPT(&vagc);
}

void ApolloGuidance::GenerateUprupt(){
	GenerateUPRUPT(&vagc);
}

void ApolloGuidance::GenerateRadarupt(){
	GenerateRADARUPT(&vagc);
}

bool ApolloGuidance::IsUpruptActive() {
	if (!Yaagc) return false;
	return (IsUPRUPTActive(&vagc) == 1);
}

// DS200608xx CH33 SWITCHES
void ApolloGuidance::SetCh33Switches(unsigned int val){
	if( isLGC)
		SetLMCh33Bits(&vagc,val);
	else 
		SetCh33Bits(&vagc,val);
}

unsigned int ApolloGuidance::GetCh33Switches(){
	return vagc.Ch33Switches; 
}


// DS20060903 PINC, DINC, ETC
int ApolloGuidance::DoPINC(int16_t *Counter){
	return(CounterPINC(Counter));
}

int ApolloGuidance::DoPCDU(int16_t *Counter){
	return(CounterPCDU(Counter));
}

int ApolloGuidance::DoMCDU(int16_t *Counter){
	return(CounterMCDU(Counter));
}

int ApolloGuidance::DoDINC(int CounterNum, int16_t *Counter){
	return(CounterDINC(&vagc,CounterNum,Counter));
}


void ApolloGuidance::SetOutputChannelBit(int channel, int bit, bool val)

{
	unsigned int mask = (1 << (bit));

	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return;

	if (val) {
		OutputChannel[channel] |= mask;
	}
	else {
		OutputChannel[channel] &= ~mask;
	}

	//
	// Special-case processing.
	//

	switch (channel)
	{
	case 05:
		ProcessChannel5(OutputChannel[05]);
		break;

	case 06:
		ProcessChannel6(OutputChannel[06]);
		break;

	case 010:
		ProcessChannel10(OutputChannel[010]);
		break;

	case 011:
		ProcessChannel11Bit(bit, val);
		break;

	case 012:
	case 014:
		imu.ChannelOutput(channel, OutputChannel[channel]);
		break;
	}
}

bool ApolloGuidance::GetInputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return false;

	return (GetInputChannel(channel) & (1 << (bit))) != 0;
}

unsigned int ApolloGuidance::GetInputChannel(int channel)

{
	if (Yaagc) {

		if (channel < 0 || channel >= NUM_CHANNELS)
			return 0;

		//
		// Virtual AGC code stores values in native form. C++ AGC expects to read them out in
		// 0 = false, 1 = true form.
		//

		unsigned int val = vagc.InputChannel[channel];

		if ((channel >= 030) && (channel <= 034))
			val ^= 077777;

		return val;
	}
	else {
		if (channel < 0 || channel > MAX_INPUT_CHANNELS){
			return 0;
		}
		if(channel == 033){ return 0; } // Causes AGC++ to not throw CMC WARNING bit
		return InputChannel[channel];
	}
}

//
// Flag that we're waiting for a new program.
//

void ApolloGuidance::AwaitProgram()

{
	if (KBCheck()) {
		SetVerbNounAndFlash(37, 0);
	}
}

//
// RSet will clear the two-level alarm code stack, but not the 'most
// recent' alarm.
//

void ApolloGuidance::RSetPressed()

{
	Alarm01 = Alarm02 = 0;
}

//
// Raise an Alarm.
//

void ApolloGuidance::RaiseAlarm(int AlarmNo)

{
	//
	// Alarms are stored as a two-level stack and the third
	// level always stores the last alarm raised.
	//

	if (!Alarm01)
		Alarm01 = AlarmNo;
	else if (!Alarm02)
		Alarm02 = AlarmNo;

	Alarm03 = AlarmNo;

	LightProg();
}

//
// Indicate an error and abort the current program.
//

void ApolloGuidance::AbortWithError(int ErrNo)

{
	BlankData();
	RunProgram(0);
	RaiseAlarm(ErrNo);
	SetVerbNounAndFlash(5, 9);
}

void ApolloGuidance::SetVesselStats(double ISP, double Thrust, bool MainIsHover)

{
	VesselISP = ISP;
	MaxThrust = Thrust;
	MainThrusterIsHover = MainIsHover;
}


void ApolloGuidance::KillAllThrusters()
{
	OurVessel->SetAttitudeLinLevel(0, 0);
	OurVessel->SetAttitudeLinLevel(1, 0);
	OurVessel->SetAttitudeLinLevel(2, 0);
	OurVessel->SetAttitudeRotLevel(0, 0);
	OurVessel->SetAttitudeRotLevel(1, 0);
	OurVessel->SetAttitudeRotLevel(2, 0);
}

void ApolloGuidance::SetDesiredLanding(double latitude, double longitude, double altitude)

{
	LandingAltitude = altitude;
	LandingLongitude = longitude;
	LandingLatitude = latitude;
}

bool ApolloGuidance::GenericReadMemory(unsigned int loc, int &val)

{
	if (Yaagc) {
		int bank, addr;

		bank = (loc / 0400);
		addr = loc - (bank * 0400);

		if (bank >= 0 && bank < 8) {
			val = vagc.Erasable[bank][addr];
			return true;
		}

		val = 0;
		return true;

	}

	//
	// C++ EMEM locations.
	//

	switch (loc)
	{
	case 00:
		val = ProgState;
		return true;

	case 01:
		val = Alarm01;
		return true;

	case 02:
		val = Alarm02;
		return true;

	case 03:
		val = Alarm03;
		return true;

	case 04:
		val = ResetCount;
		return true;

	case 05:
		val = ProgRunning;
		return true;

	case 06:
		val = VerbRunning;
		return true;

	case 07:
		val = NounRunning;
		return true;

	case 010:
		val = BankSumNum;
		return true;

	case 013:
		val = CurrentEMEMAddr;
		return true;

	case 014:
		val = ApolloNo;
		return true;

	case 016:
		val = (int) (DeltaPitchRate * 100.0);
		return true;

	case 017:
		val = (int) (LastEventTime * 100.0);
		return true;

	case 020:
		val = (int) (DesiredDeltaVx * 100.0);
		return true;

	case 021:
		val = (int) (DesiredDeltaVy * 100.0);
		return true;

	case 022:
		val = (int) (DesiredDeltaVz * 100.0);
		return true;

	case 023:
		val = (int) (DesiredPlaneChange * 100.0);
		return true;

	case 024:
		val = (int) (DesiredLAN * 100.0);
		return true;

	case 025:
		val = ((TIME1 / 16) & 077777);
		return true;

	case 026:
		val = Realism;
		return true;

	case 027:
		val = Chan10Flags;
		return true;

	case 030:
		val = (int) (VesselISP * 100.0);
		return true;

	case 031:
		val = (int) MaxThrust;
		return true;

	case 032:
		val = (int) (ThrustDecayDV * 100.0);
		return true;
	}

	val = 0;
	return false;
}

//
// Load a PAD value into the AGC. Used for initialising the LEM when created.
//

void ApolloGuidance::PadLoad(unsigned int address, unsigned int value)

{
	if (Yaagc) {
		WriteMemory(address, value);
	}
}

void ApolloGuidance::GenericWriteMemory(unsigned int loc, int val)

{
	if (Yaagc) {
		int bank, addr;

		bank = (loc / 0400);
		addr = loc - (bank * 0400);

		if (bank >= 0 && bank < 8)
			vagc.Erasable[bank][addr] = val;
		return;
	}

	switch (loc)
	{
	case 00:
		ProgState = val;
		break;

	case 01:
		Alarm01 = val;
		break;

	case 02:
		Alarm02 = val;
		break;

	case 03:
		Alarm03 = val;
		break;

	case 04:
		ResetCount = val;
		break;

	case 05:
		ProgRunning = val;
		break;

	case 06:
		VerbRunning = val;
		break;

	case 07:
		NounRunning = val;
		break;

	case 010:
		BankSumNum = val;
		break;

	case 013:
		CurrentEMEMAddr = val;
		break;

	case 014:
		ApolloNo = val;
		break;

	case 016:
		DeltaPitchRate = ((double) val) / 100.0;
		break;

	case 017:
		LastEventTime = ((double) val) / 100.0;
		break;

	case 020:
		DesiredDeltaVx = ((double) val) / 100.0;
		break;

	case 021:
		DesiredDeltaVy = ((double) val) / 100.0;
		break;

	case 022:
		DesiredDeltaVz = ((double) val) / 100.0;
		break;

	case 023:
		DesiredPlaneChange = ((double) val) / 100.0;
		break;

	case 024:
		DesiredLAN = ((double) val) / 100.0;
		break;

	case 025:
		TIME1 = val * 16;
		break;

	case 026:
		Realism = val;
		break;

	case 027:
		Chan10Flags = val;
		break;

	case 030:
		VesselISP = ((double) val) / 100.0;
		break;

	case 031:
		MaxThrust = (double) val;
		break;

	case 032:
		ThrustDecayDV = ((double) val) / 100.0;
		break;
	}
}

void ApolloGuidance::UpdateBurnTime(int R1, int R2, int R3)

{
	if (R2 > 59 || R3 > 5999) {
		LightOprErr();
		return;
	}

	if (R1 < 0 || R2 < 0 || R3 < 0) {
		LightOprErr();
		return;
	}

	BurnTime = LastTimestep + (double) (R1 * 3600 + R2 * 60) + ((double)R3) / 100;
}


int16_t ApolloGuidance::ConvertDecimalToAGCOctal(double x, bool highByte) 

{
	int sign = 0, value = 0, i = 0;

	if (x < 0) {
		sign = 1;
		x = -x;
    }
	
	if (x >= 1.0) {
		// Illegal value: Must be <1.0 according to AGC rules
		return -1;
    }

	for (value = 0, i = 0; i < 28; i++) {
		value = value << 1;
		if (x >= 0.5) {
			value++;
			x -= 0.5;
		}
		x *= 2;
    }
  
	if (x >= 0.5)
		value++;

	i = value & 0x00003fff;
	value = (value >> 14) & 0x00003fff;
	if (sign) {
		value = ~value;
		i = ~i;
		i &= 0x00007fff;
		value &= 0x00007fff;
	}

	if (highByte)
		return value;
	else
		return i;
}


//
// Virtual AGC functions.
//


//-----------------------------------------------------------------------------
// Function for broadcasting "output channel" data to all connected clients.

void ChannelOutput (agc_t * State, int Channel, int Value) 

{

  // Some output channels have purposes within the CPU, so we have to
  // account for those separately.
  if (Channel == 7)
    {
      State->InputChannel[7] = State->OutputChannel7 = (Value & 0160);
      return;
    }
  // Most output channels are simply transmitted to clients representing
  // hardware simulations.

  ApolloGuidance *agc;

  agc = (ApolloGuidance *) State->agc_clientdata;
  agc->SetOutputChannel(Channel, Value);
}

void ShiftToDeda (agc_t *State, int Data)

{
	// Nothing for now.
}

//
// Do nothing here. We'll process input seperately.
//

int ChannelInput (agc_t *State)

{
	return 0;
}

void ChannelRoutine (agc_t *State)

{
}

