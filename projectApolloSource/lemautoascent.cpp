/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Rodrigo R. M. B. Maia, Mark Grant

  ORBITER vessel module: Saturn LEM computer
  Ascent autopilot

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
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "lemcomputer.h"

#include "sat5_lmpkd.h"

void LEMcomputer::Prog10Pressed(int R1, int R2, int R3)
{
	switch(ProgState)
	{
	case 1:
		if (R1 < 1800 || R2 < 1800) {
			dsky.LightOprErr();
			return;
		}

		if (R2 >= R1) {
			dsky.LightOprErr();
			return;
		}

		if (R3 < 0 || R3 >= 36000) {
			dsky.LightOprErr();
			return;
		}

		DesiredApogee = ((double) R1) / 100.0;			//		Set final apoapsis distance
		DesiredPerigee = ((double) R2) / 100.0;			//		Set final periapsis distance
		DesiredAzimuth = ((double) R3) / 100.0;				//		Set final heading
		ProgState++;
		return;

	case 3:
		UpdateBurnTime(R1, R2, R3);
		BurnStartTime = BurnTime;
		ProgState++;
		return;

	case 5:
	case 9:
		//
		// Do nothing, just step on through the program.
		//
		ProgState++;
		return;
	}
	dsky.LightOprErr();
}

void LEMcomputer::Prog11Pressed(int R1, int R2, int R3)
{
	switch(ProgState)
	{
	case 4:
		//
		// Do nothing, just step on through the program.
		//
		ProgState++;
		return;
	}
	dsky.LightOprErr();
}

void LEMcomputer::Prog10(double simt)
{
	//
	//	In this mode, we insert the final orbit data for the auto-pilot,
	//

	switch (ProgState) {
	case 0:
		SetVerbNounAndFlash(6, 44);
		ProgState++;
		break;

	case 2:
		SetVerbNounAndFlash(6, 33);
		ProgState++;
		break;

	case 4:
		Checklist(0203);
		ProgState++;
		break;

	case 6:
		SetVerbNoun(6, 74);
		NextEventTime = BurnStartTime - 35.0;
		ProgState++;
		break;

	case 7:
		if (simt >= NextEventTime) {
			dsky.BlankAll();

			VerbRunning = 0;
			NounRunning = 0;

			NextEventTime = BurnStartTime - 30.0;
			ProgState++;
		}
		break;

	case 8:
		if (simt >= NextEventTime) {
			dsky.UnBlankAll();
			SetVerbNoun(6, 74);
			SetVerbNounAndFlash(99, 74);
			soundlib.LoadMissionSound(LunarAscent, LUNARASCENT_SOUND, LUNARASCENT_SOUND);
			ProgState++;
		}
		break;

	case 10:
		if (simt >= BurnStartTime) {
			AwaitProgram();
			ProgState = 100;
			break;
		}

		NextEventTime = BurnStartTime - 10.0;
		SetVerbNoun(16, 74);
		ProgState++;
		break;

	case 11:
		if (simt >= NextEventTime) {
			if (simt < (BurnStartTime - 9.0))
				LunarAscent.play();
			NextEventTime = BurnStartTime;
			ProgState++;
		}
		break;

	case 12:
		if (simt >= NextEventTime) {
			sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel;

			lem->StartAscent();

			LunarAscent.done();
			NextEventTime = simt + 0.1;
			LastEventTime = simt;
			RunProgram(11);
		}
		break;
	}

}

void LEMcomputer::Prog11(double simt)
{
	//
	//	This is our ascent program. This is the first stage of the ascent. Here we will be inserted
	//  into an orbit that has a pre-fixed periapsis and an apoapsis equal to the periapsis of the final orbit.
	//  Later, in Prog12, we will raise the periapsis of this first stage orbit to the apoapsis of the final
	//	orbit.
	//

	VECTOR3 vh;				//		Velocity vector relative to the horizon
	VECTOR3 PMI;			//		Store the moments of inertia
	VECTOR3 avel;			//		Angular velocity

	double alt, pitch, cpd, cap, dap, papo, tap, sb;
	double tinsalt, adjpr, Thrust, thl, npitch;
	double MaxThrust, Mass, Size, Level, ts, hdg;
	int thdir;

	//
	// Run the cycle ten times a second.
	//

#if 0
	if (simt < NextEventTime)
		return;

	NextEventTime = simt + 0.1;
#endif

	ts = simt - LastEventTime;	//		Get Time Step

	//
	// Target apoapsis,  (meters).
	//
	// For the first stage of the launch this is actually the periapsis, we then
	// rase the apoapsis later.
	//

	tap = DesiredPerigee * 1000.0;

	OBJHANDLE hPlanet = OurVessel->GetGravityRef();
	double moonrad = oapiGetSize(hPlanet);

	//
	//	Let's gather all the data we need from Orbiter itself
	//

	oapiGetHeading(OurVessel->GetHandle(),&hdg);
	OurVessel->GetAngularVel(avel);
	OurVessel->GetHorizonAirspeedVector(vh);
	OurVessel->GetApDist(cap);
	OurVessel->GetPeDist(cpd);
	OurVessel->GetPMI(PMI);

	MaxThrust = OurVessel->GetMaxThrust(ENGINE_ATTITUDE);
	Mass	  = OurVessel->GetMass();
	Size	  = OurVessel->GetSize();
	alt		  = OurVessel->GetAltitude();
	pitch	  = OurVessel->GetPitch();
	pitch	  *= DEG;
	hdg		  *= DEG;

	//
	//	Here's the ascent guidance algorithm. It's so simple!!
	//

	tinsalt = 18350 / vh.y;					//		Time to insertion altitude
	adjpr	= (90 / (tinsalt * 0.58)) * ts;	//		Adjusted pitch rate
	DeltaPitchRate += adjpr;					//		Delta pitch rate
	npitch	= 0 - DeltaPitchRate;						//		New pitch

	papo = tap + moonrad;

	switch (ProgState) {

	case 0:

		//
		// Set variables to defaults.
		//

		DeltaPitchRate	= 0;		//		Delta pitch

		//
		// Initiate launch display.
		//

		SetVerbNoun(16, 50);

		ProgState++;
		break;

	//
	//	Before we start our pitching program, let's yaw to the correct heading
	//

	case 1:
		sb     = (DesiredAzimuth - hdg) / fabs(DesiredAzimuth - hdg);
		Thrust = (Mass * PMI.data[1] * (DesiredAzimuth - hdg)) / (Size);
		Level = min((Thrust/MaxThrust), 1);
		Level = max(Level, -1);
		if(fabs(avel.y) > 0.06)
			Level = 0;
		if(fabs(DesiredAzimuth - hdg) < 1.8)
		{
			Level = 0;
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			ProgState++;
		}
		OurVessel->SetAttitudeRotLevel(1, -(Level * sb));
		break;

	//
	//	Now that we are in the correct heading, let's start pitching
	//

	case 2:
		if(pitch < -1 && pitch > -2)
			OurVessel->DeactivateNavmode(NAVMODE_KILLROT);

		if(npitch <= -82)		//		Don't pitch lower than 82 degs
		{
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			npitch = -82;
		}

		if (npitch != pitch)
		{
			Thrust = (Mass * PMI.data[0] * (npitch - pitch)) / (Size);
			Level = min((Thrust/MaxThrust), 1.0);
			Level = max(Level, -1.0);
			OurVessel->SetAttitudeRotLevel(0, Level);
		}

		if(cap > (papo - 500))							//		Cutoff main engines
		{
			OurVessel->SetThrusterGroupLevel(THGROUP_HOVER, 0);
			ProgState++;
		}
		break;

	//
	//	Now, let's fine tune our apoapsis with linear thrusters
	//

	case 3:
		if((cap - papo) > 0)
			thdir = -1;				//		If we've passed, let's go back a bit
		if((cap - papo) < 0)
			thdir = 1;				//		If we're short, let's push forward a little

		dap = (cap - papo) * thdir;
		thl = 0.5 * thdir;			//		Thruster level
		OurVessel->SetAttitudeLinLevel(1, thl);

		if (fabs(dap) < 5)			//		Allow a 5 meter tolerance
		{
			OurVessel->SetAttitudeRotLevel(0, 0);
			OurVessel->SetAttitudeLinLevel(1, 0);
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);

			DeltaPitchRate		  = 0;

			SetVerbNounAndFlash(6, 44);
			ProgState++;
		}
		break;

	case 5:
		RunProgram(12);			//		Run the the second stage ascent program
		break;
	}

	LastEventTime = simt;
}

void LEMcomputer::Prog12(double simt)
{
	//
	//	This is the second stage of the ascent program. It corrects the first stage orbit by
	//	raising it's periapsis to the final orbit apoapsis.
	//

	VECTOR3 velh;
	VECTOR3 PMI;
	VECTOR3 avel;

	double pd, ap, bank, Thrust;
	double MaxThrust, Mass, Size, Level;
	double pitch, dap, thl, npitch, ts;
	double alt, fmr, aps, pds, sb;
	int thdir;

	//
	// Run the cycle ten times a second.
	//
#if 0
	if (simt < NextEventTime)
		return;

	NextEventTime = simt + 0.1;
#endif

	ts = simt - LastEventTime;	//		Get Time Step

	OurVessel->GetPeDist(pd);
	OurVessel->GetApDist(ap);
	OurVessel->GetPMI(PMI);
	OurVessel->GetHorizonAirspeedVector(velh);
	bank	  = OurVessel->GetBank();
	pitch	  = OurVessel->GetPitch();
	MaxThrust = OurVessel->GetMaxThrust(ENGINE_ATTITUDE);
	Mass	  = OurVessel->GetMass();
	Size	  = OurVessel->GetSize();
	pitch	  *= DEG;
	bank	  *= DEG;

	OBJHANDLE hPlanet = OurVessel->GetGravityRef();
	double moonrad = oapiGetSize(hPlanet);

	fmr = OurVessel->GetFuelMass() / OurVessel->GetMaxFuelMass();
	alt = OurVessel->GetAltitude();
	aps = ap - moonrad;
	pds = pd - moonrad;

	switch (ProgState)
	{
	case 0:
		SetVerbNoun(16, 94);
		ProgState++;
		break;

	//
	//	Here we begin setting apropriate attitude for the burn. Since the LM uses hover engines
	//	it's a bit more complicated than just turning prograde
	//

	case 1:
		if (alt > (aps - 700))		//		Turn Prograde 700m before apoapsis
		{
			OurVessel->ActivateNavmode(NAVMODE_PROGRADE);
			ProgState++;
		}
		break;

	case 2:
		if (alt > (aps - 300))		//		Deactivate Prograde 400m before apoapsis
		{
			OurVessel->DeactivateNavmode(NAVMODE_PROGRADE);
			iba = bank;
			ProgState++;
		}
		break;

	case 3:
		OurVessel->GetAngularVel(avel);

		if (bank != 0)
		{
			sb = (fabs(bank) / bank) * (fabs(0 - bank) / (0 - bank));
			Thrust = (Mass * PMI.data[2] * (0 - bank)) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			if(fabs(bank) < (iba / 2))
				sb *= -1;
			OurVessel->SetAttitudeRotLevel(2, (Level * sb));
			if(fabs(bank) < 1)
				OurVessel->ActivateNavmode(NAVMODE_KILLROT);
		}
		if (fabs(bank) < 1 && fabs(avel.z) < 0.1)
		{
			OurVessel->SetAttitudeRotLevel(2,0);
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			ProgState++;
		}
		break;

	case 4:

		//
		//		Start our pitching program
		//

		DeltaPitchRate  = DeltaPitchRate + (5 * ts);
		npitch = 0 - DeltaPitchRate;

		if (npitch <= -86)				//		Don't pitch lower than 86 degrees
		{
			OurVessel->SetAttitudeRotLevel(0, 0);
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			npitch = -86;
		}

		if (pitch != npitch)
		{
			Thrust = (Mass * PMI.data[0] * (npitch - pitch)) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			OurVessel->SetAttitudeRotLevel(0, Level);
		}

		if (bank != 0.1)
		{
			sb = (fabs(bank) / bank) * (fabs(0 - bank) / (0 - bank));
			Thrust = (Mass * PMI.data[2] * (0.1 - bank)) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			OurVessel->SetAttitudeRotLevel(2, (Level * sb));
		}

		if (alt > (aps - 5))				//		Turn the engine on 5 meters before apoapsis
		{
			OurVessel->SetThrusterGroupLevel(THGROUP_HOVER, 1);
			ProgState++;
		}
		break;

	case 5:
		if (aps > ((DesiredApogee * 1000.0) - 100.0))			//		Cut off the engine
		{
			OurVessel->SetThrusterGroupLevel(THGROUP_HOVER, 0);
			ProgState++;
		}
		break;

	case 6:

		//
		//	Make final correction with linear thrusters
		//

		if((aps - (DesiredApogee * 1000.0)) > 0)
			thdir = -1;				//		If we've passed, let's go back a bit
		if((aps - (DesiredApogee * 1000.0)) < 0)
			thdir = 1;				//		If we're short, let's push forward a little

		dap = (aps - (DesiredApogee * 1000.0)) * thdir;
		thl = 0.7 * thdir;			//		Thruster level
		OurVessel->SetAttitudeLinLevel(1, thl);

		if(fabs(dap) < 5)			//		Allow a 5 meter tolerance
		{
			OurVessel->SetAttitudeRotLevel(0, 0);
			OurVessel->SetAttitudeRotLevel(2, 0);
			OurVessel->SetAttitudeLinLevel(1, 0);
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);

			VerbRunning = 0;
			NounRunning = 0;

			SetVerbNoun(6, 50);
			AwaitProgram();
			ProgState++;
		}
		break;
	}

	LastEventTime = simt;
}

