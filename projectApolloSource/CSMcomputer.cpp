/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Saturn CSM computer

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
  *	Revision 1.32  2006/01/12 00:09:07  movieman523
  *	Few fixes: Program 40 now starts and stops the SPS engine, but doesn't orient the CSM first.
  *	
  *	Revision 1.31  2006/01/11 22:34:20  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.30  2006/01/11 19:57:55  movieman523
  *	Load appropriate AGC binary file based on mission number.
  *	
  *	Revision 1.29  2006/01/09 21:56:44  movieman523
  *	Added support for LEM and CSM AGC PAD loads in scenario file.
  *	
  *	Revision 1.28  2005/11/26 16:30:50  movieman523
  *	Fixed retros and trying to fix TLI audio.
  *	
  *	Revision 1.27  2005/11/25 21:30:47  movieman523
  *	Fine-tuned thrust decay default.
  *	
  *	Revision 1.26  2005/11/25 20:59:48  movieman523
  *	Added thrust decay for SIVb in TLI burn. Still needs tweaking.
  *	
  *	Revision 1.25  2005/11/25 02:03:47  movieman523
  *	Fixed mixture-ratio change code and made it more realistic.
  *	
  *	Revision 1.24  2005/11/17 00:28:36  movieman523
  *	Wired in AGC circuit breakers.
  *	
  *	Revision 1.23  2005/10/12 11:23:06  tschachim
  *	Bugfix OPR ERR during P15.
  *	
  *	Revision 1.22  2005/10/11 16:34:04  tschachim
  *	Only for NASSP AGC mode:
  *	P01 requires the IMU turned on and P11 drives the IMU to prelaunch alignment.
  *	
  *	Revision 1.21  2005/09/24 20:55:18  lazyd
  *	No change
  *	
  *	Revision 1.20  2005/09/20 22:35:20  lazyd
  *	Made changes to save/restore landing latitude in EMEM
  *	
  *	Revision 1.19  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.18  2005/08/22 19:47:33  movieman523
  *	Fixed long timestep on startup, and added new Virtual AGC with EDRUPT fix.
  *	
  *	Revision 1.17  2005/08/19 23:54:13  movieman523
  *	Should have fixed Prog 37, at least for CSM.
  *	
  *	Revision 1.16  2005/08/19 18:38:13  movieman523
  *	Wired up parachute switches properly, and added 'Comp Acty' to CSM AGC.
  *	
  *	Revision 1.15  2005/08/19 13:57:22  tschachim
  *	Added earth orbit insertation monitoring.
  *	Added check for nonspherical gravity sources.
  *	
  *	Revision 1.14  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.13  2005/08/13 20:20:17  movieman523
  *	Created MissionTimer class and wired it into the LEM and CSM.
  *	
  *	Revision 1.12  2005/08/13 01:09:43  movieman523
  *	Display NO ATT light when IMU is caged.
  *	
  *	Revision 1.11  2005/08/12 21:42:14  movieman523
  *	Added support for 'SIVB Takeover' bit on launch.
  *	
  *	Revision 1.10  2005/08/12 18:37:01  movieman523
  *	Made Virtual AGC work all the way to orbit: enabled autopilot appropriately on launch and saved desired apogee/perigee/azimuth in scenario file.
  *	
  *	Revision 1.9  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.8  2005/08/10 22:31:57  movieman523
  *	IMU is now enabled when running Prog 01.
  *	
  *	Revision 1.7  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.6  2005/08/09 02:28:25  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.5  2005/08/08 22:32:49  movieman523
  *	First steps towards reimplementing the DSKY interface to use the same I/O channels as the real AGC/DSKY interface.
  *	
  *	Revision 1.4  2005/08/08 21:10:29  movieman523
  *	Fixed broken TLI program. LastAlt wasn't being set and that screwed up the burn end calculations.
  *	
  *	Revision 1.3  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.2  2005/04/30 23:09:14  movieman523
  *	Revised CSM banksums and apogee/perigee display to match the real AGC.
  *	
  *	Revision 1.1  2005/02/11 12:54:05  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "toggleswitch.h"
#include "saturn.h"

#include "ioChannels.h"

static const double ERADIUS2 = (ERADIUS * ERADIUS * 1000000);

CSMcomputer::CSMcomputer(SoundLib &s, DSKY &display, DSKY &display2, IMU &im) : ApolloGuidance(s, display, im), dsky2(display2)

{
	BurnTime = 0;
	BurnStartTime = 0;
	CutOffVel = 0;
	lastOrbitalElementsTime = 0;

	MainThrusterIsHover = false;

	FlagWord0.word = 0;
	FlagWord1.word = 0;
	FlagWord2.word = 0;
	FlagWord3.word = 0;
	FlagWord4.word = 0;
	FlagWord5.word = 0;

	//
	// Set sensible values for flags.
	//

	FlagWord0.u.P29FLAG = 1;
	FlagWord0.u.IMUSE = 1;
	FlagWord1.u.AVEGFLAG = 1;
	FlagWord1.u.TRACKFLG = 1;
	FlagWord1.u.UPDATFLG = 1;
	FlagWord2.u.CALCMAN2 = 1;
	FlagWord3.u.VINTFLAG = 1;
	FlagWord3.u.V50N18FL = 1;
	FlagWord5.u.DSKYFLAG = 1;

	//
	// Generic thrust decay value. This still needs tweaking.
	//

	ThrustDecayDV = 6.1;

	//
	// Last RCS settings.
	//

	LastOut5 = 0;
	LastOut6 = 0;
	LastOut11 = 0;
}

CSMcomputer::~CSMcomputer()

{
	//
	// Nothing for now.
	//
}

void CSMcomputer::SetMissionInfo(int MissionNo, int RealismValue, char *OtherVessel)

{
	ApolloGuidance::SetMissionInfo(MissionNo, RealismValue, OtherVessel);

	//
	// Pick the appropriate AGC binary file based on the mission number.
	//

	char *binfile = "Config/ProjectApollo/Artemis072.bin";
	if (MissionNo < 15 || MissionNo == 1301)
		binfile = "Config/ProjectApollo/Colossus249.bin";

	InitVirtualAGC(binfile);
}

//
// Validate verb and noun combinations when they are
// entered on the DSKY, for verbs specific to this
// class of computer.
//

bool CSMcomputer::ValidateVerbNoun(int verb, int noun)

{
	switch (verb) {

	case 21:
		if (ProgRunning == 15 && (noun == 14 || noun == 33))
			return true;
		return false;

	case 75:
	case 82:
		return true;

	case 46:
		if (ProgRunning == 11)
			return true;
		return false;

	}

	return false;
}


//
// CalcTFF calculates the freefall time from our current position to
// entry at 300,000 feet. This assumes that g is constant, so is only
// valid in a fairly circular orbit.. this is a good approximation for
// launch and entry, since g won't change much over the course of the
// time we're interested in.
//

const double tff_alt = (300000 * 0.3048);	// 300,000 feet.

double CSMcomputer::CalcTFF(double vy, double r0, double g)

{
	double c = (vy * vy) + (2 * g * (r0 - tff_alt));

	//
	// If we're never going to reach 300,000 feet then return max time.
	//

	if (c <= 0 || g <= 0)
		return 3600.0;

	//
	// Otherwise calculate the tff.
	//

	double tff =  (vy + sqrt(c)) / g;

	//
	// If the time is negative, we've already dropped below 300,000 feet,
	// so return max time.
	//

	if (tff < 0)
		return 3600.0;

	return tff;
}

//
// Actually process the verb and noun.
//

void CSMcomputer::DisplayNounData(int noun)

{
	//
	// CSM-specific overrides
	//

	switch (noun)
	{
	//
	// 44: Orbit parameters. Only available during program 11.
	//

	case 44:
		if (ProgRunning == 11)
		{
			ELEMENTS el;
			double mjd_ref;

			OurVessel->GetElements(el, mjd_ref);

			double apogee = (el.a * (1.0 + el.e)) - (ERADIUS * 1000);
			double perigee = (el.a * (1.0 - el.e)) - (ERADIUS * 1000);

			if (apogee < 0)
				apogee = 0;

			SetR1((int)DisplayAlt(apogee) / 100);
			SetR2((int)DisplayAlt(perigee) / 100);

			//
			// Adjust gravity to take into account our current velocity parallel to the planet's
			// surface.
			//

			double g = CurrentG() - ((CurrentVelX * CurrentVelX + CurrentVelZ * CurrentVelZ) / (CurrentAlt + (ERADIUS * 1000)));
			double tff = CalcTFF(CurrentVelY, CurrentAlt, g);

			int	min = (int)(tff / 60);
			int sec = (int)tff - (min * 60);

			if (min > 59) {
				min = 59;
				sec = 59;
			}

			SetR3(min * 1000 + sec);
			SetR3Format(TwoSpaceTwoFormat);
			return;
		}
		break;
	}

	if (DisplayCommonNounData(noun))
		return;

	switch (noun) {

	//
	// 14: desired delta V
	//

	case 14:
		SetR1((int)DisplayVel(DesiredDeltaV));
		break;

	//
	// 62: inertial velocity information. Velocity, altitude rate, altitude.
	//

	case 62:
		SetR1((int)DisplayVel(CurrentVel));
		SetR2((int)DisplayVel(CurrentVelY));
		SetR3((int)(DisplayAlt(CurrentAlt) / 1000));
		break;

	//
	// 73: altitude, velocity, AoA
	//

	case 73:
		if (!Reset) {
			BlankData();
			return;
		}

		SetR1((int)(DisplayAlt(CurrentAlt) / 1000));
		SetR2((int)DisplayVel(CurrentVel));
		SetR3((int)(OurVessel->GetAOA() * 5729.57795));
		break;
	}
}

//
// Are the apogee/perigee orbit calculations valid?
//

bool CSMcomputer::OrbitCalculationsValid()

{
	switch (ProgRunning)
	{
	case 11:
	case 15:
		return true;

	case 37:
		if (ProgState >= 2)
			return true;
		break;
	}

	return false;
}

void CSMcomputer::ProcessVerbNoun(int verb, int noun)

{
	switch (verb) {

	//
	// 46: disable autopilot (actually toggle here).
	//

	case 46:
		if (ProgRunning == 11) {
			ChannelValue12 val12;
			val12.Value = GetOutputChannel(012);

			val12.Bits.EnableSIVBTakeover = !val12.Bits.EnableSIVBTakeover;
			SetOutputChannel(012, val12.Value);
		}
		break;

	//
	// 75: launch.
	//

	case 75:
		Liftoff(LastTimestep);
		break;

	//
	// 82: display orbit parameter settings.
	//

	case 82:
		if (ProgRunning == 11) {
			SetVerbNounAndFlash(16, 44);
		}
	}
}

//
// Validate program number for programs specific to this
// class of computer.
//

bool CSMcomputer::ValidateProgram(int prog)

{
	if (FlagWord2.u.NODOFLAG)
		return false;

	switch (prog) {

	case 1:
	case 2:
	case 6:
	case 11:
		return true;

	case 15:
		Saturn *sat = (Saturn *) OurVessel;
		return sat->isTLICapable();

	}

	return false;
}

//
// Program 01: pre-launch IMU align.
//

void CSMcomputer::Prog01(double simt)

{
	switch (ProgState) {

	case 0:

		if (FlagWord1.u.NODOP01) {
			AbortWithError(021521);
			break;
		}

		LightCompActy();
		LightNoAtt();

		// Check if the IMU is running and uncaged.
		ChannelValue30 val30;
		val30.Value = GetInputChannel(030);
		if (val30.Bits.IMUCage || !val30.Bits.IMUOperate) {
			AbortWithError(0210);
			break;
		}
		//imu.TurnOn();

		NextProgTime = simt + 2.0;
		ProgState++;
		break;

	case 1:
		if (simt >= NextProgTime) {
			LightCompActy();
			SetOutputChannelBit(012, 5, true);
			SetOutputChannelBit(012, 5, false);
			SetOutputChannelBit(012, 15, true);
			NextProgTime = simt + 1.0;
			ProgState++;
		}
		break;

	case 2:
		if (simt >= NextProgTime) {
			ChannelValue30 val30;

			LightCompActy();
			val30.Value = GetInputChannel(030);
			if (val30.Bits.IMUOperate && !val30.Bits.IMUCage) {
				ClearNoAtt();
				RunProgram(2);
			}
			else 
				NextProgTime = simt + 1.0;
		}
		break;
	}
}

//
// Program 02: launch setup.
//

void CSMcomputer::Prog02(double simt)

{
	switch (ProgState) {

	case 0:
		LightCompActy();
		SetVerbNounAndFlash(6, 29);
		ProgState++;
		break;

	case 2:
		LightCompActy();
		SetVerbNounAndFlash(6, 44);
		ProgState++;
		break;

	case 4:
		LightCompActy();
		SetVerbNounAndFlash(6, 33);
		ProgState++;
		break;
	}
}


//
// Data entry for Prog 02.
//

void CSMcomputer::Prog02Pressed(int R1, int R2, int R3)

{
	switch (ProgState) {

	case 1:
		//
		// Waiting for launch azimuth.
		//

		if (R1 < 0 || R1 > 35999) {
			LightOprErr();
			return;
		}

		DesiredAzimuth = ((double) R1) / 100.0;
		ProgState++;
		break;

	case 3:
		//
		// Waiting for apogee and perigee.
		//

		DesiredApogee = ((double) R1) / 100.0;
		DesiredPerigee = ((double) R2) / 100.0;
		ProgState++;
		break;

	case 5:
		//
		// Waiting for burn time.
		//

		{
			Saturn *sat = (Saturn *) OurVessel;
			UpdateBurnTime(R1, R2, R3);
			sat->UpdateLaunchTime(BurnTime - LastTimestep);
			ProgState = 4;
		}
		break;

	default:
		//
		// Anything else is an error
		//

		LightOprErr();
		break;
	}
}

//
// Program 06: standby.
//


void CSMcomputer::Prog06(double simt)

{
	switch (ProgState) {

	case 0:
		Checklist(062);
		ProgState++;
		break;
	}
}

//
// Program 11: main launch program.
//

void CSMcomputer::Prog11(double simt)

{
	Saturn *Sat = (Saturn *)OurVessel;

	switch (ProgState) {

	case 0:
		SetVerbNoun(16, 62);

		LightCompActy();

		//
		// Clear flash in case this program was auto-started by
		// the launch.
		//

		ClearVerbNounFlashing();

		ProgState++;
		break;

	case 1:

		//
		// If we've reached orbit, turn off the autopilot and indicate to
		// the user that they should select a new program.
		//

		if (InOrbit) {

			VerbRunning = 0;
			NounRunning = 0;

			InOrbit = 1;

			Sat->SetAutopilot(false);

			AwaitProgram();

			ProgState++;
		}
		break;

	};
}

//
// Prog 15: TLI burn.
//

void CSMcomputer::DoTLICalcs(double simt)

{
	extern void PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel, double a, double Mu,
						  double Time, VECTOR3 &NewPos, VECTOR3 &NewVel, double &NewVelMag);
	//
	// We know the expected deltaV, so we need to calculate the time for the burn,
	// and the expected end velocity.
	//

	double mass = OurVessel->GetMass();
	double isp = VesselISP;
	double fuelmass = OurVessel->GetFuelMass();
	double thrust = MaxThrust;

	double massrequired = mass * (1 - exp(-((DesiredDeltaV - ThrustDecayDV) / isp)));

	LightCompActy();

	if (massrequired > fuelmass) {
		ProgState = 2;
		return;
	}

	double deltaT = massrequired / (thrust / isp);

	//
	// Start the burn early, subtracting half the burn length, and 25
	// seconds for SIVB startup.
	//

	BurnStartTime = (BurnTime - (deltaT / 2) - 25.0);
	BurnEndTime = BurnTime + (deltaT / 2);

	//
	// Now calculate the cutoff velocity.
	//

	VECTOR3 Pos, Vel;
	OurVessel->GetRelativePos(OurVessel->GetGravityRef(), Pos);
	OurVessel->GetRelativeVel(OurVessel->GetGravityRef(), Vel);

	ELEMENTS el;
	double mjd_ref;
	OurVessel->GetElements(el, mjd_ref);

	VECTOR3 NewPos, NewVel;
	double NewVelMag;

	PredictPosVelVectors(Pos, Vel, el.a, 3.986e14,
						  BurnTime - LastTimestep, NewPos, NewVel, NewVelMag);

	CutOffVel = NewVelMag + DesiredDeltaV - ThrustDecayDV;
}

double CSMcomputer::CurrentG()

{
	double CurrentDist = (ERADIUS * 1000) + CurrentAlt;
	return (G * ERADIUS2) / (CurrentDist * CurrentDist);
}

void CSMcomputer::UpdateTLICalcs(double simt)

{
	double MaxE = CutOffVel * CutOffVel;
	double deltaE = 2.0 * (CurrentG() * (CurrentAlt - LastAlt));

	LightCompActy();
	LastAlt = CurrentAlt;
	CutOffVel = sqrt(MaxE - deltaE);
}

void CSMcomputer::Prog15(double simt)

{
	switch (ProgState) {

	//
	// First display time to burn.
	//

	case 0:
		LightCompActy();
		FlagWord2.u.STEERSW = 0;
		SetOutputChannelBit(012, 9, true);
		BurnTime = 0;
		SetVerbNounAndFlash(6, 33);
		ProgState++;
		break;

	case 2:
		LightCompActy();
		SetVerbNounAndFlash(6, 14);
		ProgState++;
		break;

	case 4:
		DoTLICalcs(simt);

		SetVerbNounAndFlash(6, 95);
		ProgState++;

		NextEventTime = (BurnStartTime - (9 * 60 + 38));

		if (NextEventTime < simt)
			NextEventTime = simt;

		break;

	case 5:
		if (simt > NextEventTime) {
			LightCompActy();
			Saturn *sat = (Saturn *)OurVessel;
			LightUplink();
			sat->SetSIISep();
			ProgState++;
			NextEventTime += 10;
		}
		break;

	case 6:
		if (simt > NextEventTime) {
			LightCompActy();
			Saturn *sat = (Saturn *)OurVessel;
			DoTLICalcs(simt);
			ClearUplink();
			sat->ClearSIISep();
			ProgState++;
			NextEventTime = BurnStartTime - 105;
		}
		break;

	case 7:
		if (simt > NextEventTime) {
			BlankAll();

			LightCompActy();
			soundlib.LoadMissionSound(STLI, GO_FOR_TLI_SOUND, NULL);

			VerbRunning = 0;
			NounRunning = 0;

			ProgState++;
			NextEventTime += 5;
		}
		break;

	case 8:
		if (simt > NextEventTime) {
			Saturn *sat = (Saturn *) OurVessel;

			UnBlankAll();
			SetVerbNounAndFlash(16, 95);

			DoTLICalcs(simt);

			//
			// Reset time acceleration to normal.
			//

			oapiSetTimeAcceleration (1);


			//
			// Only play this the first time.
			//

			if (STLI.isValid() && !(sat->TLIDone())) {
				STLI.play();
				STLI.done();
			}

			ProgState++;
			NextEventTime += 10;
		}
		break;

	case 9:
		if (simt >= NextEventTime) {
			OurVessel->ActivateNavmode(NAVMODE_PROGRADE);
			DoTLICalcs(simt);

			if (simt > (BurnStartTime - 20)) {
				NextEventTime = simt + 2.0;
			}
			else {
				NextEventTime = BurnStartTime - 84.0;
				ProgState++;
			}
		}
		break;

	case 10:
		if (simt >= NextEventTime) {
			LightCompActy();
			Saturn *sat = (Saturn *)OurVessel;
			sat->SetSIISep();
			NextEventTime = BurnStartTime - 18;
			ProgState++;
		}
		break;


	case 11:
		if (simt >= NextEventTime) {
			LightCompActy();
			Saturn *sat = (Saturn *)OurVessel;
			sat->ClearSIISep();
			NextEventTime = BurnStartTime - 10.9;
			soundlib.LoadSound(Scount, COUNT10_SOUND);
			ProgState++;
		}
		break;

	case 12:
		if (simt >= NextEventTime) {

			//
			// Reset time acceleration to normal.
			//

			oapiSetTimeAcceleration (1);

			//
			// And play the countdown.
			//


			DoTLICalcs(simt);

			Scount.play(NOLOOP,245);
			Scount.done();

			soundlib.LoadMissionSound(STLIStart, TLI_START_SOUND, NULL);

			ProgState++;
			NextEventTime = BurnStartTime - 1;
		}
		break;

	//
	// One second before the burn the engine light comes
	// on to warn the crew.
	//

	case 13:
		if (simt >= NextEventTime) {
			LightCompActy();
			Saturn *sat = (Saturn *)OurVessel;
			NextEventTime = BurnStartTime;
			sat->SetEngineIndicator(1);
			ProgState++;
		}
		break;

	//
	// Signal the SIVB to start.
	//

	case 14:
		if (simt > BurnStartTime) {
			LightCompActy();
			OurVessel->ActivateNavmode(NAVMODE_PROGRADE);
			SetOutputChannelBit(012, 13, true);
			ProgState++;
			LastAlt = CurrentAlt;
			NextEventTime = simt + 1;
		}
		break;

	//
	// We should really wait for the output channel bit to be cleared here.
	//

	//
	// Ignition!
	//
	// Oddly, the engine indicator is turned _off_ here.
	//

	case 15:
		if (OurVessel->GetEngineLevel(ENGINE_MAIN) >= 1.0) {
			if (STLIStart.isValid()) {
				if (oapiGetTimeAcceleration () < 10)
					STLIStart.play();
				STLIStart.done();
			}

			LightCompActy();
			Saturn *sat = (Saturn *) OurVessel;
			sat->ClearEngineIndicator(1);
			ProgState++;
		}

		//
		// And fall through to normal processing.
		//

	case 16:
		{
			OurVessel->ActivateNavmode(NAVMODE_PROGRADE);

			if (simt > NextEventTime) {
				UpdateTLICalcs(simt);
				NextEventTime = simt + 0.5;
			}

			//
			// Cut off the engine at the appropriate velocity.
			//
			// This is indicated to the crew by the engine light
			// coming on.
			//

			if (CurrentRVel >= CutOffVel) {
				Saturn *sat = (Saturn *)OurVessel;
				SetOutputChannelBit(012, 13, false);
				SetOutputChannelBit(012, 14, true);
				SetVerbNoun(6, 95);
				sat->SetEngineIndicator(1);
				OurVessel->DeactivateNavmode(NAVMODE_PROGRADE);
				NextEventTime = simt + 2.0;
				ProgState = 17;
			}
		}
		break;

		//
		// Turn engine indicator off.
		//

	case 17:
		if (simt >= NextEventTime) {
			UpdateTLICalcs(simt);
			LightCompActy();
			Saturn *sat = (Saturn *)OurVessel;
			sat->ClearEngineIndicator(1);
			FlagWord5.u.ENGONBIT = 0;
			FlagWord2.u.STEERSW = 0;
			ProgState++;
		}
		break;
	}
}

//
// Data entry for Prog 15.
//

void CSMcomputer::Prog15Pressed(int R1, int R2, int R3)

{
	switch (ProgState) {

	case 1:
		//
		// Waiting for burn start time.
		//

		UpdateBurnTime(R1, R2, R3);
		ProgState++;
		break;

	case 3:
		//
		// Waiting for delta-V.
		//

		if (R1 < 0) {
			LightOprErr();
			return;
		}

		DesiredDeltaV = GetVel((double)R1);

		DoTLICalcs(LastTimestep);

		ProgState++;
		break;

	case 18:
		//
		// Finished.
		//

		NounRunning = 0;
		VerbRunning = 0;

		AwaitProgram();
		BlankData();
		ProgState++;
		break;

	default:
		//
		// Anything else is an error
		//

		LightOprErr();
		break;
	}
}

void CSMcomputer::Timestep(double simt, double simdt)

{
	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	//
	// Initial startup hack for Yaagc.
	//
	if (Yaagc && !PadLoaded) {

#ifndef AGC_SOCKET_ENABLED		
		double latitude, longitude, radius, heading;

		// init pad load
		OurVessel->GetEquPos(longitude, latitude, radius);
		oapiGetHeading(OurVessel->GetHandle(), &heading);

		// set launch pad latitude
		vagc.Erasable[5][2] = (int16_t)((16384.0 * latitude) / TWO_PI);

		// set launch pad azimuth
		vagc.Erasable[5][0] = (int16_t)((16384.0 * heading) / TWO_PI);

		// z-component of the normalized earth's rotational vector in basic reference coord.
		// x and y are 0313 and 0315 and are zero
		vagc.Erasable[3][0317] = 037777;	

		// set launch pad altitude
		vagc.Erasable[2][0273] = (int16_t) (0.5 * OurVessel->GetAltitude());
		//State->Erasable[2][0272] = 01;	// 17.7 nmi

		//
		// Enable DAP. These should work, but no guarantees.
		//

		vagc.Erasable[AGC_BANK(AGC_DAPDTR1)][AGC_ADDR(AGC_DAPDTR1)] = 010002;
		vagc.Erasable[AGC_BANK(AGC_DAPDTR2)][AGC_ADDR(AGC_DAPDTR2)] = 001111;
#endif

		PadLoaded = true;
	}

	if (!GenericTimestep(simt, simdt)) {

		switch (ProgRunning) {

		//
		// 00: idle program.
		//

		case 0:
			break;

		//
		// 01: pre-launch IMU align.
		//

		case 1:
			Prog01(simt);
			break;

		//
		// 02: pre-launch setup.
		//

		case 2:
			Prog02(simt);
			break;

		//
		// 06: power-down.
		//

		case 6:
			Prog06(simt);
			break;

		//
		// 11: initial launch program.
		//

		case 11:
			Prog11(simt);
			break;

		//
		// 15: TLI program.
		//

		case 15:
			Prog15(simt);
			break;


		}
	}

	if (Yaagc) {

		//
		// Debug output to check P11
		//
		VECTOR3 vel, hvel;
		double vvel = 0, apDist, peDist;
		OBJHANDLE earth = oapiGetGbodyByName("Earth");
		OurVessel->GetRelativeVel(earth, vel); 
		if (OurVessel->GetHorizonAirspeedVector(hvel)) {
			vvel = hvel.y * 3.2808399;
		}

		if (lastOrbitalElementsTime == 0) {
			apDist = 0;
			peDist = 0;
			lastOrbitalElementsTime = simt;

		} else if (simt - lastOrbitalElementsTime >= 2.0) {
			OurVessel->GetApDist(apDist);
			OurVessel->GetPeDist(peDist);

			apDist -= 6.373338e6;
			peDist -= 6.373338e6;

#ifdef _DEBUG
			sprintf(oapiDebugString(), "P11 - Vel %.0f Vert. Vel %.0f Alt %.0f ApD %.0f PeD %.0f",  
				length(vel) * 3.2808399, vvel, OurVessel->GetAltitude() * 0.000539957 * 10, 
				apDist * 0.000539957 * 10, peDist * 0.000539957 * 10);
#endif

			lastOrbitalElementsTime = simt;
		}

		//
		// Check nonspherical gravity sources
		//
		if (!OurVessel->NonsphericalGravityEnabled()) {
			sprintf(oapiDebugString(), "*** PLEASE ENABLE NONSPHERICAL GRAVITY SOURCES ***");
		}
	}
}

//
// We've been told to terminate the program.
//

void CSMcomputer::TerminateProgram()

{
	//
	// Special-case handling of individual program events.
	//

	switch (ProgRunning) {

	//
	// 11: disable autopilot.
	//

	case 11:
		{
			Saturn *sat = (Saturn *) OurVessel;
			sat->SetAutopilot(false);
		}
		break;

	//
	// 15: if we started the SIVB, then stop it.
	//

	case 15:
		if (ProgState > 14) {
			Saturn *sat = (Saturn *)OurVessel;
			sat->SIVBStop();
			sat->ClearEngineIndicator(1);
		}
		break;

	default:
		TerminateCommonProgram();
		break;

	}

	//
	// In general, just switch to idle and wait for a
	// new program.
	//

	VerbRunning = 0;
	NounRunning = 0;

	BlankData();
	RunProgram(0);
	AwaitProgram();
}

//
// We've been told to proceed with no data. Check where we are
// in the program and proceed apropriately.
//

void CSMcomputer::ProceedNoData()

{
	if (CommonProceedNoData())
		return;

	switch (ProgRunning) {

	case 2:
		if (ProgState == 1 || ProgState == 3) {
			ProgState++;
			return;
		}
		break;

	case 15:
		if (ProgState == 1 || ProgState == 3) {
			ProgState++;
			return;
		}
		break;
	}

	LightOprErr();
}

//
// The prog button has been pressed, so process it.
//

void CSMcomputer::ProgPressed(int R1, int R2, int R3)

{
	if (GenericProgPressed(R1, R2, R3))
		return;

	switch(ProgRunning) {

	case 02:
		Prog02Pressed(R1, R2, R3);
		return;

	case 15:
		Prog15Pressed(R1, R2, R3);
		return;
	}

	LightOprErr();
}

//
// Program flag routines.
//

unsigned int CSMcomputer::GetFlagWord(int num)

{
	switch (num) {
	case 0:
		FlagWord0.u.FREEFLAG = ProgFlag01;
		FlagWord0.u.UNUSED = ProgFlag02;
		return FlagWord0.word;

	case 1:
		return FlagWord1.word;

	case 2:
		return FlagWord2.word;

	case 3:
		return FlagWord3.word;

	case 4:
		return FlagWord4.word;

	case 5:
		FlagWord5.u.RETROFLG = RetroFlag;
		FlagWord5.u.UNUSED1 = ProgFlag03;
		FlagWord5.u.UNUSED2 = ProgFlag04;
		FlagWord5.u.ENGONBIT = BurnFlag;
		return FlagWord5.word;
	}

	//
	// Invalid flag word number.
	//

	return 0;
}

void CSMcomputer::SetFlagWord(int num, unsigned int val)

{
	switch (num) {
	case 0:
		FlagWord0.word = val;
		ProgFlag01 = FlagWord0.u.FREEFLAG;
		ProgFlag02 = FlagWord0.u.UNUSED;
		break;

	case 1:
		FlagWord1.word = val;
		break;

	case 2:
		FlagWord2.word = val;
		break;

	case 3:
		FlagWord3.word = val;
		break;

	case 4:
		FlagWord4.word = val;
		break;

	case 5:
		FlagWord5.word = val;
		RetroFlag = FlagWord5.u.RETROFLG;
		BurnFlag = FlagWord5.u.ENGONBIT;
		ProgFlag03 = FlagWord5.u.UNUSED1;
		ProgFlag04 = FlagWord5.u.UNUSED2;
		break;
	}

	//
	// Invalid flag word number.
	//
}

//
// Access simulated erasable memory.
//

bool CSMcomputer::ReadMemory(unsigned int loc, int &val)

{
	if (!Yaagc) {
		//
		// Note that these values are in OCTAL, so need the
		// zero prefix.
		//

		if (loc >= 074 && loc <= 0107) {
			val = (int) GetFlagWord(loc - 074);
			return true;
		}

		switch (loc)
		{
		case 0110:
			val = (int) (LandingLatitude * 1000.0);
			return true;

		case 0111:
			val = (int) (LandingLongitude * 1000.0);
			return true;

		case 0112:
			val = (int) LandingAltitude;
			return true;

		case 0113:
			val = (int)(LandingLatitude*100000000-((int) (LandingLatitude * 1000.0))*100000);
			return true;

		case 0114:
			val = (int)(LandingLongitude*100000000-((int) (LandingLongitude * 1000.0))*100000);
			return true;
		default:
			break;
		}
	}

	return GenericReadMemory(loc, val);
}

void CSMcomputer::WriteMemory(unsigned int loc, int val)

{
	if (!Yaagc) {
		//
		// Note that these values are in OCTAL, so need the
		// zero prefix.
		//

		if (loc >= 074 && loc <= 0107) {
			SetFlagWord(loc - 074, (unsigned int) val);
			return;
		}

		switch (loc)
		{
		case 0110:
			LandingLatitude = ((double) val) / 1000.0;
			break;

		case 0111:
			LandingLongitude = ((double) val) / 1000.0;
			break;

		case 0112:
			LandingAltitude = (double) val;
			break;

		case 0113:
			LandingLatitude = LandingLatitude+((double) val) / 100000000.0;
			break;

		case 0114:
			LandingLongitude = LandingLongitude+((double) val) / 100000000.0;
			break;

		default:
			GenericWriteMemory(loc, val);
			break;
		}
	}
	else {
		GenericWriteMemory(loc, val);
	}
}

//
// Bank checksums.
//

static unsigned int BankSums[] =
{
	077777, 063743,
	000001, 074057,
	000002, 050032,
	000003, 060757,
	000004, 077375,
	000005, 043705,
	077771, 005143,
	000007, 063337,
	000010, 050763,
	000011, 045337,
	000012, 063341,
	000013, 074774,
	000014, 056404,
	000015, 052523,
	000016, 042460,
	000017, 042700,
	000020, 077316,
	000021, 061425,
	000022, 075664,
	077754, 016525,
	000024, 072000,
	000025, 060227,
	000026, 053053,
	000027, 066445,
	000030, 072005,
	000031, 040224,
	000032, 060374,
	000033, 071456,
	000034, 077620,
	000035, 054100,
	000036, 040555,
	077740, 011751,
	000040, 057252,
	000041, 064667,
	000042, 042661,
	077734, 000152,
};

void CSMcomputer::DisplayBankSum()

{
	int	R1, R3;

	if (BankSumNum > 043)
		BankSumNum = 0;

	R1 = BankSums[BankSumNum * 2];
	R3 = BankSums[(BankSumNum * 2) + 1];

	SetR1Octal(R1);
	SetR2Octal(BankSumNum);
	SetR3Octal(R3);
}

//
// Exernal event handling.
//

void CSMcomputer::Liftoff(double simt)

{

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	//
	// Ensure autopilot is enabled.
	//

	SetOutputChannelBit(012, 9, false);
	Saturn *Sat = (Saturn *)OurVessel;
	Sat->SetAutopilot(true);

	//
	// If we're not running the Virtual AGC, then switch to program 11.
	//

	if (!Yaagc) {

		//
		// As a quick hack we drive the IMU to prelaunch orientation 
		// until the NASSP AGC has a working P01
		//

		imu.DriveGimbals((90.0 + DesiredAzimuth) / DEG, 90.0 / DEG, 0.0);

		//
		// Also, we need to reset the AGC time to zero at this point, as the real AGC did.
		//

		TIME1 = 0;

		if (OnStandby())
			Startup();

		FlagWord1.u.NODOP01 = 1;

		RunProgram(11);

		if (!KBCheck()) {
			UnBlankAll();
		}
	} 
}

void CSMcomputer::SetInputChannelBit(int channel, int bit, bool val)

{
	ApolloGuidance::SetInputChannelBit(channel, bit, val);

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	switch (channel)
	{
	case 030:
		if (bit == 5 && val) {
			Liftoff(CurrentTimestep);	// Liftoff signal
			break;
		}
		else if (!Yaagc && (bit == 11 || bit == 9)) {
			ChannelValue30 val30;

			val30.Value = GetInputChannel(030);
			dsky.SetNoAtt(!val30.Bits.IMUOperate || val30.Bits.IMUCage);
		}
		break;
	}
}

//
// We need to pass these I/O channels to both DSKYs.
//

void CSMcomputer::ProcessChannel10(int val)

{
	dsky.ProcessChannel10(val);
	dsky2.ProcessChannel10(val);
}

void CSMcomputer::ProcessChannel11Bit(int bit, bool val)

{
	dsky.ProcessChannel11Bit(bit, val);
	dsky2.ProcessChannel11Bit(bit, val);

	//
	// Channel 11 also controls the SPS engine.
	//

	if (bit == 13) {
		CheckEngineOnOff(GetOutputChannel(011));
	}

	LastOut11 = GetOutputChannel(011);
}

void CSMcomputer::ProcessChannel11(int val)

{
	dsky.ProcessChannel11(val);
	dsky2.ProcessChannel11(val);

	CheckEngineOnOff(val);

	LastOut11 = val;
}

void CSMcomputer::CheckEngineOnOff(int val)

{
	ChannelValue30 val30;
	val30.Value = GetInputChannel(030);

	Saturn *sat = (Saturn *) OurVessel;

	if (!val30.Bits.CMSMSeperate) {
		ChannelValue11 Current;
		ChannelValue11 Changed;

		Current.Value = val;
		Changed.Value = (val ^ LastOut11);

		if (Changed.Bits.EngineOnOff) {
#ifdef _DEBUG
	fprintf(out_file, "Setting SPS state = %d\n", Current.Bits.EngineOnOff);
#endif
			sat->SetSPSState(Current.Bits.EngineOnOff != 0);
		}
	}
}

//
// Process RCS channels
//

void CSMcomputer::ProcessChannel5(int val)

{
	ChannelValue30 val30;
	val30.Value = GetInputChannel(030);

	Saturn *sat = (Saturn *) OurVessel;

	//
	// SM channels.
	//

	if (!val30.Bits.CMSMSeperate) {
		CSMOut5 Current;
		CSMOut5 Changed;

		//
		// Get the current state and a mask of any changed state.
		//

		Current.word = val;
		Changed.word = (val ^ LastOut5);

		//
		// Update any thrusters that have changed.
		//

		if (Changed.u.SMA3) {
			sat->SetRCSState(RCS_SM_QUAD_A, 3, Current.u.SMA3 != 0);
		}
		if (Changed.u.SMA4) {
			sat->SetRCSState(RCS_SM_QUAD_A, 4, Current.u.SMA4 != 0);
		}

		if (Changed.u.SMB3) {
			sat->SetRCSState(RCS_SM_QUAD_B, 3, Current.u.SMB3 != 0);
		}
		if (Changed.u.SMB4) {
			sat->SetRCSState(RCS_SM_QUAD_B, 4, Current.u.SMB4 != 0);
		}

		if (Changed.u.SMC3) {
			sat->SetRCSState(RCS_SM_QUAD_C, 3, Current.u.SMC3 != 0);
		}
		if (Changed.u.SMC4) {
			sat->SetRCSState(RCS_SM_QUAD_C, 4, Current.u.SMC4 != 0);
		}

		if (Changed.u.SMD3) {
			sat->SetRCSState(RCS_SM_QUAD_D, 3, Current.u.SMD3 != 0);
		}
		if (Changed.u.SMD4) {
			sat->SetRCSState(RCS_SM_QUAD_D, 4, Current.u.SMD4 != 0);
		}
	}

	LastOut5 = val;
}

void CSMcomputer::ProcessChannel6(int val)

{
	ChannelValue30 val30;
	val30.Value = GetInputChannel(030);

	Saturn *sat = (Saturn *) OurVessel;

	//
	// SM channels.
	//

	if (!val30.Bits.CMSMSeperate) {
		CSMOut6 Current;
		CSMOut6 Changed;

		//
		// Get the current state and a mask of any changed state.
		//

		Current.word = val;
		Changed.word = (val ^ LastOut6);

		//
		// Update any thrusters that have changed.
		//

		if (Changed.u.SMA1) {
			sat->SetRCSState(RCS_SM_QUAD_A, 1, Current.u.SMA1 != 0);
		}
		if (Changed.u.SMA2) {
			sat->SetRCSState(RCS_SM_QUAD_A, 2, Current.u.SMA2 != 0);
		}

		if (Changed.u.SMB1) {
			sat->SetRCSState(RCS_SM_QUAD_B, 1, Current.u.SMB1 != 0);
		}
		if (Changed.u.SMB2) {
			sat->SetRCSState(RCS_SM_QUAD_B, 2, Current.u.SMB2 != 0);
		}

		if (Changed.u.SMC1) {
			sat->SetRCSState(RCS_SM_QUAD_C, 1, Current.u.SMC1 != 0);
		}
		if (Changed.u.SMC2) {
			sat->SetRCSState(RCS_SM_QUAD_C, 2, Current.u.SMC2 != 0);
		}

		if (Changed.u.SMD1) {
			sat->SetRCSState(RCS_SM_QUAD_D, 1, Current.u.SMD1 != 0);
		}
		if (Changed.u.SMD2) {
			sat->SetRCSState(RCS_SM_QUAD_D, 2, Current.u.SMD2 != 0);
		}
	}

	LastOut6 = val;
}

