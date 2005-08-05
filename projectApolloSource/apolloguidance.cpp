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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.9  2005/08/01 21:45:41  lazyd
  *	Changed Program 33 to Program 37 to avoid conflict with rendezvous
  *	
  *	Revision 1.8  2005/07/16 20:34:52  lazyd
  *	*** empty log message ***
  *	
  *	Revision 1.7  2005/07/09 18:30:17  lazyd
  *	Changed attitude-control code to allow faster rates
  *	P33 changed to use 10% throttle for hover thrusters
  *	
  *	Revision 1.6  2005/07/06 21:57:15  lazyd
  *	Added code to orient the hover engine to prograde or retrograde
  *	
  *	Revision 1.5  2005/05/19 20:26:52  movieman523
  *	Rmaia's AGC 2.0 changes integrated: can't test properly as the LEM DSKY currently doesn't work!
  *	
  *	Revision 1.4  2005/04/30 23:09:15  movieman523
  *	Revised CSM banksums and apogee/perigee display to match the real AGC.
  *	
  *	Revision 1.3  2005/04/20 17:44:48  movieman523
  *	Added call to force restart of the AGC.
  *	
  *	Revision 1.2  2005/03/03 17:36:27  tschachim
  *	initialized BurnEndTime
  *	
  *	Revision 1.1  2005/02/10 21:22:00  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"

char TwoSpaceTwoFormat[7] = "XXX XX";

ApolloGuidance::ApolloGuidance(SoundLib &s, DSKY &display) : soundlib(s), dsky(display)

{
	ProgRunning = VerbRunning = NounRunning = 0;
	ProgState = 0;
	Standby = false;

	MaxThrust = 0;
	VesselISP = 1000000;

	Reset = false;
	LastTimestep = 0;
	LastEventTime = 0.0;
	InOrbit = false;

	LastVerb16Time = 0;

	BurnTime = 0;
	BurnStartTime = 0;
	BurnEndTime = 0;
	CutOffVel = 0;
	NextEventTime = 0;
	BankSumNum = 1;

	//
	// Default to C++ AGC.
	//

	Yaagc = 0;

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

	DesiredDeltaVx = 0.0;
	DesiredDeltaVy = 0.0;
	DesiredDeltaVz = 0.0;

	DesiredPlaneChange = 0.0;
	DesiredLAN = 0.0;

	DeltaPitchRate = 0.0;

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
}

ApolloGuidance::~ApolloGuidance()

{
	//
	// Nothing for now.
	//
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

	//
	// We always start up in program zero.
	//

	RunProgram(0);

	//
	// Indicate to user that they should change
	// programs as appropriate.
	//

	NounRunning = 37;	// Change program.

	if (dsky.KBCheck()) {
		dsky.SetProg(ProgRunning);
		dsky.BlankData();
		dsky.SetVerb(37);
		dsky.SetVerbFlashing();
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
	dsky.BlankAll();
	dsky.ClearVerbFlashing();
	dsky.ClearNounFlashing();
	dsky.LightStby();
	Standby = true;
}

bool ApolloGuidance::OutOfReset()

{
	return Reset;
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
	// 33: orbit altitude adjustment.
	//

	case 33:
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

		dsky.BlankData();

		//
		// Then display the approprirate data.
		//

		DisplayNounData(noun);

		//
		// Set octal mode if required.
		//

		switch (verb) {
		case 5:
			dsky.DisplayR3Octal();
		case 4:
			dsky.DisplayR2Octal();
		case 3:
			dsky.DisplayR1Octal();
			break;
		}
		break;

	//
	// 37: run a program.
	//

	case 37:
		RunProgram(noun);
		dsky.SetProg(ProgRunning);
		break;

	//
	// 69: cause restart.
	//

	case 69:
		Reset = false;
		dsky.BlankAll();
		dsky.LightRestart();
		RunProgram(0);
		break;

	//
	// 91: display bank checksum.
	//

	case 91:
		BankSumNum = 0;
		dsky.BlankData();
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
		dsky.SetR1Octal(BankSumNum);
	else
		dsky.SetR1Octal(077777 -BankSumNum);

	dsky.SetR2Octal(BankSumNum);
	dsky.SetR3Octal((BankSumNum * 10000 + 4223) & 077777);
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
	// 33: if the engine is on, stop it.
	//

	case 33:
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

	GetPosVel();
	OurVessel->GetElements(el, mjd_ref);

	double apogee = (el.a * (1.0 + el.e)) - rad;
	double perigee = (el.a * (1.0 - el.e)) - rad;

	if (apogee < 0)
		apogee = 0;

	dsky.SetR1((int)DisplayAlt(apogee) / 1000);
	dsky.SetR2((int)DisplayAlt(perigee) / 1000);
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

	dsky.SetR1(h);
	dsky.SetR2(m);
	dsky.SetR3(s);
}

bool ApolloGuidance::DisplayCommonNounData(int noun)

{
	switch (noun) {

	//
	// 09: program alarms.
	//

	case 9:
		dsky.SetR1Octal(Alarm01);
		dsky.SetR2Octal(Alarm02);
		dsky.SetR3Octal(Alarm03);
		return true;

	//
	// 16: desired attitude.
	//

	case 16:
		dsky.SetR1((int)(TargetRoll * 100));
		dsky.SetR2((int)(TargetPitch * 100));
		dsky.SetR3((int)(TargetYaw * 100));
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

		dsky.SetR1((int)(bank * 100));
		dsky.SetR2((int)(pitch * 100));
		dsky.SetR3((int)(hdg * 100));
		return true;

	//
	// 28: Current attitude w.r.t airspeed vector
	//

	case 28:
		double alpha, slip;

		alpha = OurVessel->GetAOA() * DEG;
		slip  = OurVessel->GetSlipAngle() * DEG;
		
		dsky.SetR1((int)(alpha * 100));
		dsky.SetR2((int)(slip * 100));
		dsky.BlankR3();
		return true;

	//
	// 29: launch azimuth.
	//

	case 29:
		dsky.SetR1((int)(DesiredAzimuth * 100));
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

			dsky.SetR1(tsph);
			dsky.SetR2(tspm);
			dsky.SetR3(tsps);
		}
		return true;


	//
	// 33: time to burn.
	//

	case 33:
		{
//			double Met;
			double TimeToBurn = 0.0;
//			sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel;
//			lem->GetMissionTime(Met);

			if (BurnTime > LastTimestep) {
				TimeToBurn = BurnTime - LastTimestep;
			}

			int h = ((int)TimeToBurn) / 3600;
			int m = ((int)TimeToBurn - (3600 * h)) / 60;
			int s = ((int)(TimeToBurn * 100)) % 6000;

			dsky.SetR1(h);
			dsky.SetR2(m);
			dsky.SetR3(s);
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
			dsky.BlankData();
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

			dsky.SetR1((int)(apd / 10));
			dsky.SetR2((int)(ped / 10));
			dsky.SetR3((int)dv);
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

		dsky.SetR1((int)(latitude * 100.0));
		dsky.SetR2((int)(longitude * 100.0));
		dsky.SetR3((int)DisplayAlt(CurrentAlt * 0.01));

		return true;

	//
	// 44: Orbit parameters. Only available when the AGC is theoretically
	//	   calculating orbit parameters, otherwise request desired values.
	//

	case 44:
		if (OrbitCalculationsValid())
		{
			DisplayOrbitCalculations();
			dsky.SetR3((int)DisplayAlt(OurVessel->GetAltitude()) / 1000);
		}
		else if (ProgRunning == 33) {
			dsky.SetR1((int)(DesiredApogee * 10.0));
			dsky.SetR2((int)(DesiredPerigee * 10.0));
		}
		else {
			dsky.SetR1((int)(DesiredApogee * 100.0));
			dsky.SetR2((int)(DesiredPerigee * 100.0));
			if (ProgRunning == 10) {
				dsky.SetR3((int)(DesiredAzimuth * 100.0));
			}
		}
		return true;

	//
	// 46: Target DeltaV, DeltaV so far, Time to burn (used by P30)
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

			dsky.SetR1((int)dv);
			dsky.SetR2((int)DesiredDeltaV);
			dsky.SetR3(min * 1000 + sec);
			dsky.SetR3Format("XXX XX");
		}			
		return true;


	//
	// 73: altitude, velocity, AoA
	//

	case 73:
		GetPosVel();

		dsky.SetR1((int)(DisplayAlt(CurrentAlt) / 1000));
		dsky.SetR2((int)DisplayVel(CurrentVel));
		dsky.SetR3((int)(OurVessel->GetAOA() * 5729.57795));
		return true;

	//
	// 81: Desired DeltaV components in Local Vertical (LV) coordinates (X left, Y up, Z front)
	//

	case 81:
		dsky.SetR1((int)DesiredDeltaVx);
		dsky.SetR2((int)DesiredDeltaVy);
		dsky.SetR3((int)DesiredDeltaVz);
		return true;

	//
	// 82: Desired DeltaV in horizon coordinates
	//

	case 82:
		dsky.SetR1((int)(DesiredDeltaVx));
		dsky.SetR2((int)(DesiredDeltaVy));
		dsky.SetR3((int)(DesiredDeltaVz));
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

		GetPosVel();

		int	Min = (int)(IgnitionTime / 60);
		int Sec = ((int)IgnitionTime) % 60;

		if (Min < 0)
			Min = 0;
		if (Sec < 0)
			Sec = 0;

		dsky.SetR1(Min * 1000 + Sec);
		dsky.SetR1Format(TwoSpaceTwoFormat);
		dsky.SetR2((int)DisplayVel(R2));
		dsky.SetR3((int)DisplayVel(R3));
		return true;
	}

	return false;
}

bool ApolloGuidance::GenericTimestep(double simt)

{
	LastTimestep = CurrentTimestep;
	CurrentTimestep = simt;

	TIME1 += (int)((simt - LastTimestep) * 1600.0);

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
			DisplayNounData(NounRunning);
			LastVerb16Time = simt;
		}
		break;
	}

	switch (ProgRunning)
	{

	//
	// 37: orbit altitude adjustment.
	//

	case 37:
		Prog37(simt);
		break;
	}

	return false;
}

//
// Start the specified program running.
//

void ApolloGuidance::RunProgram(int prog)

{
	ProgRunning = prog;
	ProgState = 0;
	LastProgTime = LastTimestep;
	dsky.SetProg(prog);

	if (prog == 0) {
		dsky.BlankData();
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

	dsky.LightOprErr();
}

//
// Run the reset program on startup.
//

void ApolloGuidance::ResetCountdown()

{
	int Val2 = 11 * ResetCount;
	int	Val5 = 11111 * ResetCount;

	dsky.SetProg(Val2);
	dsky.SetNoun(Val2);
	dsky.SetVerb(Val2);

	dsky.SetR1(Val5);
	dsky.SetR2(Val5);
	dsky.SetR3(Val5);

	if (ResetCount > 0) {
		ResetCount--;
	}
}

void ApolloGuidance::SetVerbNounAndFlash(int Verb, int Noun)

{
	VerbRunning = Verb;
	NounRunning = Noun;

	dsky.BlankData();
	dsky.SetVerb(VerbRunning);
	dsky.SetNoun(NounRunning);
	dsky.SetVerbFlashing();
	dsky.SetNounFlashing();

	DisplayNounData(NounRunning);
}


void ApolloGuidance::SetVerbNoun(int Verb, int Noun)

{
	VerbRunning = Verb;
	NounRunning = Noun;

	dsky.BlankData();
	dsky.SetVerb(VerbRunning);
	dsky.SetNoun(NounRunning);
	dsky.ClearVerbFlashing();
	dsky.ClearNounFlashing();

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
	OurVessel->SetAttitudeRotLevel(Level);
}


void ApolloGuidance::BurnMainEngine(double thrust)

{
	if (MainThrusterIsHover) {
		OurVessel->SetEngineLevel(ENGINE_HOVER, thrust);
	}
	else {
		OurVessel->SetEngineLevel(ENGINE_MAIN, thrust);
	}

	if (thrust > 0.0)
		BurnFlag = true;
	else
		BurnFlag = false;
}

void ApolloGuidance::Prog37(double simt)

{
	double pcthrust;

	switch (ProgState)
	{
	case 0:
		dsky.BlankData();
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
				dsky.BlankAll();
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
			dsky.UnBlankAll();
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
		dsky.BlankAll();
		NextEventTime = simt;
		ProgState++;
		break;

	case 2:
		NextEventTime = simt;

		dsky.LightUplink();
		dsky.LightNoAtt();
		dsky.LightStby();
		dsky.LightKbRel();
		dsky.LightOprErr();
		dsky.LightTemp();
		dsky.LightGimbalLock();
		dsky.LightRestart();
		dsky.LightTracker();
		dsky.LightProg();

		dsky.UnBlankAll();

		dsky.ClearVerbFlashing();
		dsky.ClearNounFlashing();

		ResetCount = 9;
		ResetCountdown();

		ProgState++;
		break;

	case 4:
		NextEventTime = simt;
		dsky.ClearUplink();
		ResetCountdown();
		ProgState++;
		break;

	case 6:
		NextEventTime = simt;
		dsky.ClearNoAtt();
		ResetCountdown();
		ProgState++;
		break;

	case 8:
		NextEventTime = simt;
		dsky.ClearStby();
		ResetCountdown();
		ProgState++;
		break;

	case 10:
		NextEventTime = simt;
		dsky.ClearKbRel();
		ResetCountdown();
		ProgState++;
		break;

	case 12:
		NextEventTime = simt;
		dsky.ClearOprErr();
		ResetCountdown();
		ProgState++;
		break;

	case 14:
		NextEventTime = simt;
		dsky.ClearTemp();
		ResetCountdown();
		ProgState++;
		break;

	case 16:
		NextEventTime = simt;
		dsky.ClearGimbalLock();
		ResetCountdown();
		ProgState++;
		break;

	case 18:
		NextEventTime = simt;
		dsky.ClearProg();
		ResetCountdown();
		ProgState++;
		break;

	case 20:
		NextEventTime = simt;
		dsky.ClearRestart();
		ResetCountdown();

		dsky.SetVerbFlashing();
		dsky.SetNounFlashing();

		ProgState++;
		break;

	case 22:
		NextEventTime = simt;
		ResetTime = simt;

		dsky.ClearTracker();
		ProgState++;

		//
		// Ok, now we're done. Set Reset state and blank the displays.
		//

		Reset = true;

		dsky.BlankAll();
		dsky.ClearVerbFlashing();
		dsky.ClearNounFlashing();

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
			dsky.LightOprErr();
		}
		break;
	}
}

void ApolloGuidance::DisplayEMEM(unsigned int addr)

{
	int val;

	if (ReadMemory(addr, val))
		dsky.SetR1Octal(val);
	else
		dsky.SetR1Octal(077777);
}

//
// PROG pressed.
//

bool ApolloGuidance::GenericProgPressed(int R1, int R2, int R3)

{
	switch (VerbRunning) {
	case 1:
		DisplayEMEM(R2);
	case 11:
		CurrentEMEMAddr = R2;
		return true;

	case 91:
		BankSumNum++;
		DisplayBankSum();
		return true;
	}

	switch(ProgRunning) {

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
	dsky.SetVerb(50);
	dsky.SetNoun(25);
	dsky.BlankData();
	dsky.SetR1Octal(num);
	dsky.SetVerbFlashing();
	dsky.SetNounFlashing();
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

typedef union

{
	struct {
		unsigned Reset:1;
		unsigned InOrbit:1;
		unsigned Standby:1;
		unsigned Units:1;
	} u;
	unsigned long word;
} AGCState;

void ApolloGuidance::SaveState(FILEHANDLE scn)

{
	char fname[32];
	int i;
	int val;

	oapiWriteLine(scn, AGC_START_STRING);

//
// Now obsolete values.
//

	oapiWriteScenario_int (scn, "YAAGC", Yaagc);
	oapiWriteScenario_float (scn, "BRNTIME", BurnTime);
	oapiWriteScenario_float (scn, "BRNSTIME", BurnStartTime);
	oapiWriteScenario_float (scn, "BRNETIME", BurnEndTime);
	oapiWriteScenario_float (scn, "EVTTIME", NextEventTime);
	oapiWriteScenario_float (scn, "CUTFVEL", CutOffVel);
//	oapiWriteScenario_float (scn, "TGTP", TargetPitch);
//	oapiWriteScenario_float (scn, "TGTR", TargetRoll);
//	oapiWriteScenario_float (scn, "TGTY", TargetYaw);

	if (InOrbit) {
		oapiWriteScenario_float (scn, "TGTDV", DesiredDeltaV);
	}

	//
	// Copy internal state to the structure.
	//

	AGCState state;

	state.word = 0;
	state.u.Reset = Reset;
	state.u.InOrbit = InOrbit;
	state.u.Standby = Standby;
	state.u.Units = (DisplayUnits == UnitImperial);

	oapiWriteScenario_int (scn, "STATE", state.word);

	//
	// Write out any non-zero EMEM state.
	//

	for (i = 0; i < EMEM_ENTRIES; i++) {
		if (ReadMemory(i, val) && val != 0) {
			sprintf(fname, "EMEM%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	//
	// And non-zero I/O state.
	//

	for (i = 0; i <= MAX_INPUT_CHANNELS; i++) {
		val = GetInputChannel(i);
		if (val != 0) {
			sprintf(fname, "ICHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	for (i = 0; i <= MAX_OUTPUT_CHANNELS; i++) {
		val = GetOutputChannel(i);
		if (val != 0) {
			sprintf(fname, "OCHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	oapiWriteLine(scn, AGC_END_STRING);
}

void ApolloGuidance::LoadState(FILEHANDLE scn)

{
	char	*line;
	float	flt;

	//
	// Set EMEM to defaults.
	//


	for (unsigned int i = 0; i < EMEM_ENTRIES; i++) {
		WriteMemory(i, 0);
	}

	//
	// Now load the data.
	//

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, AGC_END_STRING, sizeof(AGC_END_STRING)))
			return;
		if (!strnicmp (line, "PROGSTATE", 9)) {
			sscanf (line+9, "%d", &ProgState);
		}
		else if (!strnicmp (line, "VERB", 4)) {
			sscanf (line+4, "%d", &VerbRunning);
		}
		else if (!strnicmp (line, "NOUN", 4)) {
			sscanf (line+4, "%d", &NounRunning);
		}
		else if (!strnicmp (line, "PROG", 4)) {
			sscanf (line+4, "%d", &ProgRunning);
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
		else if (!strnicmp (line, "TGTA", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredApogee = flt;
		}
		else if (!strnicmp (line, "TGTP", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredPerigee = flt;
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
			sscanf(line+4, "%d", &num);
			sscanf(line+8, "%d", &val);
			WriteMemory(num, val);
		}
		else if (!strnicmp (line, "ICHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line+5, "%d", &num);
			sscanf(line+9, "%d", &val);
			SetInputChannel(num, val);
		}
		else if (!strnicmp (line, "OCHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line+5, "%d", &num);
			sscanf(line+9, "%d", &val);
			SetOutputChannel(num, val);
		}
		else if (!strnicmp (line, "STATE", 5)) {
			AGCState state;
			sscanf (line+5, "%d", &state.word);

			Reset = state.u.Reset;
			InOrbit = state.u.InOrbit;
			Standby = state.u.Standby;
			DisplayUnits = state.u.Units ? UnitImperial: UnitMetric;
		}
		else if (!strnicmp (line, "YAAGC", 5)) {
			sscanf (line+5, "%d", &Yaagc);
		}
	}
}

//
// I/O channel support code.
//

bool ApolloGuidance::GetOutputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return false;

	return (OutputChannel[channel] & (1 << bit)) != 0;
}

unsigned int ApolloGuidance::GetOutputChannel(int channel)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return 0;

	return OutputChannel[channel];
}

void ApolloGuidance::SetInputChannel(int channel, unsigned int val)

{
	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return;

	InputChannel[channel] = val;
}

void ApolloGuidance::SetInputChannelBit(int channel, int bit, bool val)

{
	unsigned int mask = (1 << bit);

	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return;

	if (val) {
		InputChannel[channel] |= mask;
	}
	else {
		InputChannel[channel] &= ~mask;
	}
}

void ApolloGuidance::SetOutputChannel(int channel, unsigned int val)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return;

	OutputChannel[channel] = val;
}

void ApolloGuidance::SetOutputChannelBit(int channel, int bit, bool val)

{
	unsigned int mask = (1 << bit);

	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return;

	if (val) {
		OutputChannel[channel] |= mask;
	}
	else {
		OutputChannel[channel] &= ~mask;
	}
}

bool ApolloGuidance::GetInputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return false;

	return (InputChannel[channel] & (1 << bit)) != 0;
}

unsigned int ApolloGuidance::GetInputChannel(int channel)

{
	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return 0;

	return InputChannel[channel];
}

//
// Flag that we're waiting for a new program.
//

void ApolloGuidance::AwaitProgram()

{
	if (dsky.KBCheck()) {
		dsky.SetVerb(37);
		dsky.SetNoun(0);
		dsky.SetVerbFlashing();
		dsky.SetNounFlashing();
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
}

//
// Indicate an error and abort the current program.
//

void ApolloGuidance::AbortWithError(int ErrNo)

{
	dsky.BlankData();
	dsky.LightProg();
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

	case 011:
		val = (unsigned int) (DesiredApogee * 100.0);
		return true;

	case 012:
		val = (unsigned int) (DesiredPerigee * 100.0);
		return true;

	case 013:
		val = CurrentEMEMAddr;
		return true;

	case 014:
		val = ApolloNo;
		return true;

	case 015:
		val = (int) (DesiredAzimuth * 100.0);
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
	}

	val = 0;
	return false;
}

void ApolloGuidance::GenericWriteMemory(unsigned int loc, int val)

{
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

	case 011:
		DesiredApogee = ((double) val) / 100.0;
		break;

	case 012:
		DesiredPerigee = ((double) val) / 100.0;
		break;

	case 013:
		CurrentEMEMAddr = val;
		break;

	case 014:
		ApolloNo = val;
		break;

	case 015:
		DesiredAzimuth = ((double) val) / 100.0;
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

	}
}

void ApolloGuidance::UpdateBurnTime(int R1, int R2, int R3)

{
	if (R2 > 59 || R3 > 5999) {
		dsky.LightOprErr();
		return;
	}

	if (R1 < 0 || R2 < 0 || R3 < 0) {
		dsky.LightOprErr();
		return;
	}

	BurnTime = LastTimestep + (double) (R1 * 3600 + R2 * 60) + ((double)R3) / 100;
}
