/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant, Rodrigo R. M. B. Maia

  ORBITER vessel module: Saturn LEM computer

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
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "lemcomputer.h"

LEMcomputer::LEMcomputer(SoundLib &s, DSKY &display) : ApolloGuidance(s, display)

{
	//
	// Default ascent parameters.
	//

	DesiredApogee = 82.250;
	DesiredPerigee = 74.360;
	DesiredAzimuth = 270.0;

	InOrbit = 1;
}

LEMcomputer::~LEMcomputer()

{
	//
	// Nothing for now.
	//
}

//
// Validate verb and noun combinations when they are
// entered on the DSKY, for verbs specific to this
// class of computer.
//

bool LEMcomputer::ValidateVerbNoun(int verb, int noun)

{
	switch (verb) {

	case 6:
	case 16:
		return true;

	case 99:
		return true;
	}

	return false;
}


//
// Display data for the appropriate noun..
//

void LEMcomputer::DisplayNounData(int noun)

{
	if (DisplayCommonNounData(noun))
		return;

	switch (noun) {

	//
	// 11: Time to iginition (TIG) of CSI / Time to apoapsis
	//

	case 11:
		{
			OBJHANDLE gBody;
			ELEMENTS Elements;
			VECTOR3 Rp;
			VECTOR3 Rv;
			double GMass, GSize, Mu, mjd_ref, Me, TtPeri;
			double E, T, tsp, RDotV, R, p, v, apd, TtApo;
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

			// Time to next periapsis & apoapsis
			TtPeri = T - tsp;
			if (RDotV < 0) {
				TtPeri = -1 * tsp;
			}

			if (TtPeri > (T / 2)) {
				TtApo = fabs((T/2) - TtPeri);
			} 
			else 
			{
				TtApo = fabs(TtPeri + (T/2));
			}
	
			tsph = ((int)TtApo) / 3600;
			tspm = ((int)TtApo - (3600 * tsph)) / 60;
			tsps = ((int)(TtApo * 100)) % 6000;

			dsky.SetR1(tsph);
			dsky.SetR2(tspm);
			dsky.SetR3(tsps);
		}
		break;

	//
	// 45: Plane change angle, target LAN (Used by P32)
	//

	case 45:		
		dsky.SetR1((int)(DesiredPlaneChange * 100));		
		dsky.SetR2((int)(DesiredLAN));		
		dsky.BlankR3();
		break;

	//
	// 50: apogee, perigee, fuel
	//

	case 50:
		if (OrbitCalculationsValid())
		{
			DisplayOrbitCalculations();
			dsky.SetR3((int)((OurVessel->GetFuelMass() / OurVessel->GetMaxFuelMass()) * 10000.0));
		}
		break;

	//
	// 74: time to launch, yaw angle, heading.
	//

	case 74:
		{
			double dt, hdga, yaw;
			
			oapiGetFocusHeading(&hdga);			
			dt   = BurnStartTime - CurrentTimestep;			
			hdga *= DEG;
			yaw  = DesiredAzimuth - hdga;

			int min = (int) (dt / 60.0);
			int	sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR1(min * 1000 + sec);
			dsky.SetR1Format("XXX XX");
			dsky.SetR2((int)(yaw * 100.0));
			dsky.SetR3((int)(DesiredAzimuth * 100.0));
		}
		break;

	//
	// 75: Time to CSI burn, DesiredDeltaV, DesiredPlaneChange
	//

	case 75:
		{
			double dt = (BurnStartTime - 10) - CurrentTimestep;
			
			int min = (int) (dt / 60.0);
			int	sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR1(min * 1000 + sec);
			dsky.SetR1Format("XXX XX");
			dsky.SetR2((int)DesiredLAN);
			dsky.SetR3((int)(DesiredPlaneChange * 100));
		}		
		break;

	//
	// 76: Desired, downrange and radial velocity and azimuth
	//

	case 76:		
		dsky.SetR1((int)DesiredDeltaVx);			//		Insertion hoiriozontal speed, fixed
		dsky.SetR2((int)DesiredDeltaVy);			//		Insertion vertical velocity, also fixed
		dsky.SetR3((int)DesiredAzimuth * 100);		//		Launch azimuth, can be defined
		break;

	//
	// 89: for now, landing site definition.
	//
	case 89:
		dsky.SetR1((int) (LandingLatitude * 100.0));
		dsky.SetR2((int) (LandingLongitude * 100.0));
		dsky.SetR3((int) DisplayAlt(LandingAltitude));
		break;

	case 94:
		{
			double ap;
			OurVessel->GetApDist(ap);

			OBJHANDLE hPlanet = OurVessel->GetGravityRef();
			double aps = ap - oapiGetSize(hPlanet);

			dsky.SetR1((int)(aps  / 10));		//		Display current apoapsis (will be periapsis)
			dsky.SetR2((int)(OurVessel->GetAltitude()  / 10));		//		Display current altitude
			dsky.SetR3((int)(DesiredApogee * 100.0));		//		Display target apoapsis
		}
		break;
	}
}

void LEMcomputer::ProcessVerbNoun(int verb, int noun)

{
	switch (verb) {

	//
	// 16: monitor display.
	//

	case 16:
		LastVerb16Time = LastTimestep;

	//
	// 6: decimal display data.
	//

	case 6:

		//
		// First blank all for safety.
		//

		dsky.BlankData();

		//
		// Then display the approprirate data.
		//

		DisplayNounData(noun);
		break;
	}
}

bool LEMcomputer::ValidateProgram(int prog)

{
	switch (prog)
	{

	//
	//	10: Auto Pilot Data Entry
	//
	case 10:
		return true;

	//
	//	11: Ascent first phase
	//
	case 11:
		return true;

	//
	//	12: Ascent second phase
	//
	case 12:
		return true;

	//
	// 63: braking burn.
	//
	case 63:
		return true;

	//
	// 68: landing confirmation.
	//
	case 68:
		return true;
	}
	return false;
}

void LEMcomputer::Timestep(double simt)

{
	if (GenericTimestep(simt))
		return;

	switch (ProgRunning) {

	//
	// 00: idle program.
	//

	case 0:
		break;

	//
	//	12: Ascent program
	//

	case 12:
		Prog12(simt);
		break;

	//
	// 63: braking.
	//

	case 63:
		Prog63(simt);
		break;

	//
	// 68: landing confirmation.
	//

	case 68:
		Prog68(simt);
		break;
	}

	switch (VerbRunning) {

	default:
		break;
	}
}

void LEMcomputer::ProgPressed(int R1, int R2, int R3)

{
	if (GenericProgPressed(R1, R2, R3))
		return;

	switch (ProgRunning) {

	case 12:
		Prog12Pressed(R1, R2, R3);
		return;	

	case 63:
		Prog63Pressed(R1, R2, R3);
		return;

	case 68:
		Prog68Pressed(R1, R2, R3);
		return;
	}

	dsky.LightOprErr();
}

//
// We've been told to terminate the program.
//

void LEMcomputer::TerminateProgram()

{
	switch (ProgRunning) {
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

	dsky.BlankData();
	RunProgram(0);
	AwaitProgram();
}

//
// We've been told to proceed with no data. Check where we are
// in the program and proceed apropriately.
//

void LEMcomputer::ProceedNoData()

{
	if (CommonProceedNoData())
		return;

	dsky.LightOprErr();
}


//
// Program flags.
//

unsigned int LEMcomputer::GetFlagWord(int num)

{
	switch (num) {
	case 0:
		FlagWord0.u.FREEFBIT = ProgFlag01;
		return FlagWord0.word;

	case 1:
		FlagWord1.u.NOTASSIGNED = ProgFlag02;
		FlagWord1.u.NOTASSIGNED2 = ProgFlag03;
		FlagWord1.u.NOTASSIGNED3 = ProgFlag04;
		return FlagWord1.word;

	case 2:
		FlagWord2.u.NOTASSIGNED = RetroFlag;
		return FlagWord2.word;
	}

	return 0;
}

void LEMcomputer::SetFlagWord(int num, unsigned int val)

{
	switch(num) {
	case 0:
		FlagWord0.word = num;
		ProgFlag01 = FlagWord0.u.FREEFBIT;
		break;

	case 1:
		FlagWord1.word = num;
		ProgFlag02 = FlagWord1.u.NOTASSIGNED;
		ProgFlag03 = FlagWord1.u.NOTASSIGNED2;
		ProgFlag04 = FlagWord1.u.NOTASSIGNED3;
		break;

	case 2:
		FlagWord2.word = num;
		RetroFlag = FlagWord2.u.NOTASSIGNED;
		break;
	}
}

//
// Access simulated erasable memory.
//

bool LEMcomputer::ReadMemory(unsigned int loc, int &val)

{
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
		val = (int) (LandingLatitude * 100.0);
		return true;

	case 0111:
		val = (int) (LandingLongitude * 100.0);
		return true;

	case 0112:
		val = (int) LandingAltitude;
		return true;
	}

	return GenericReadMemory(loc, val);
}

void LEMcomputer::WriteMemory(unsigned int loc, int val)

{
	//
	// Note that these values are in OCTAL, so need the
	// zero prefix.
	//

	if (loc >= 074 && loc <= 0107) {
		SetFlagWord(loc - 074, (unsigned int) val);
		return;
	}

	switch (loc) {
	case 0110:
		LandingLatitude = ((double) val) / 100.0;
		break;

	case 0111:
		LandingLongitude = ((double) val) / 100.0;
		break;

	case 0112:
		LandingAltitude = (double) val;
		break;

	default:
		GenericWriteMemory(loc, val);
		break;
	}
}

bool LEMcomputer::DescentPhase()

{
	if (ProgRunning >= 63 && ProgRunning <= 68)
		return true;

	return false;
}

bool LEMcomputer::AscentPhase()

{
	if (ProgRunning > 10 && ProgRunning < 13)
		return true;

	return false;
}

void LEMcomputer::Prog68(double simt)

{
	//
	// Landing confirmation. Shut down engines and display lat/long/altitude information.
	//
	switch (ProgState) {
	case 0:
		OurVessel->SetEngineLevel(ENGINE_HOVER, 0.0);
		SetVerbNounAndFlash(6, 43);
		ProgState++;
		break;

	case 2:
		AwaitProgram();
		ProgState++;
		break;
	}
}

bool LEMcomputer::OrbitCalculationsValid()

{
	return DescentPhase() || AscentPhase();
}

