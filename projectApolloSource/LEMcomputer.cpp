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
  *	Revision 1.12  2005/08/06 00:03:48  movieman523
  *	Beginnings of support for AGC I/O channels in LEM.
  *	
  *	Revision 1.11  2005/08/04 19:55:15  lazyd
  *	Added function def for OrbitalParameters
  *	
  *	Revision 1.10  2005/08/03 10:44:33  spacex15
  *	improved audio landing synchro
  *	
  *	Revision 1.9  2005/08/01 21:48:12  lazyd
  *	Added new programs for ascent and rendezvous
  *	
  *	Revision 1.8  2005/07/16 20:38:59  lazyd
  *	Added Nouns 77 and 94 for P70 and P71
  *	
  *	Revision 1.7  2005/07/14 10:06:14  spacex15
  *	Added full apollo11 landing sound
  *	initial release
  *	
  *	Revision 1.6  2005/06/13 18:45:26  lazyd
  *	Added call to P66 and recoded noun 60
  *	
  *	Revision 1.5  2005/06/08 22:42:06  lazyd
  *	Added new EMEM code for lat/lon
  *	
  *	Revision 1.4  2005/06/06 22:25:41  lazyd
  *	Small hack to display my own Noun 33
  *	
  *	Revision 1.3  2005/06/04 20:24:57  lazyd
  *	Added nouns and programs specific to landing
  *	
  *	Revision 1.2  2005/05/19 20:26:52  movieman523
  *	Rmaia's AGC 2.0 changes integrated: can't test properly as the LEM DSKY currently doesn't work!
  *	
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
#include "sat5_lmpkd.h"

LEMcomputer::LEMcomputer(SoundLib &s, DSKY &display) : ApolloGuidance(s, display)

{
	//
	// Default ascent parameters.
	//

	DesiredApogee = 82.250;
	DesiredPerigee = 74.360;
	DesiredAzimuth = 270.0;

	InOrbit = 1;

    mode = -1;
	simcomputert = -1.;
    timeremaining = 99999.;
	timeafterpdi = -1.;

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
	//LazyD added this to bypass noun 33 in common nouns
	if(noun != 33 && noun != 81) {
	if (DisplayCommonNounData(noun))
		return;
	}
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
	// 18: pitch, roll and yaw angles
	//

	case 18:
		{
		double pitch, roll, yaw;
		pitch=CurrentVelX;
		roll=CurrentVelY;
		yaw=CurrentVelZ;
		dsky.SetR1((int) pitch);
		dsky.SetR2((int) roll);
		dsky.SetR3((int) yaw);
		}
		break;

	//
	// 31: time from ignition mm ss, secs to end of burn, dv to go (fictitious)
	//

	case 31:
		{
			int min, sec;
			double secs, sect, dvr, dt;
			VECTOR3 dv;

			// time from ignition
			dt=CurrentTimestep-BurnStartTime;
			min = (int) (dt / 60.0);
			sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR1(min * 1000 + sec);
			dsky.SetR1Format("XXX XX");
			sect=BurnEndTime-BurnStartTime;
			if(dt > 0.0) {
				secs=BurnEndTime-CurrentTimestep;
				dsky.SetR2((int) secs);
			} else {
				secs=sect;
				dsky.SetR2((int) sect);
			}

			dv.x=DesiredDeltaVx;
			dv.y=DesiredDeltaVy;
			dv.z=DesiredDeltaVz;
			dvr=Mag(dv)*(secs/sect);
			dsky.SetR3((int) (dvr*10.0));
		}
		break;



// this is in common nouns in apolloguidance.cpp, but I think this is correct...

	//
	// 33: Time of ignition  hh-mm-ss Mission Elapsed Time
	//

	case 33:
		{
//		double dmjd=oapiGetSimMJD();
//		int mjd=(int)dmjd;
//		double times=BurnTime-CurrentTimestep+(dmjd-mjd)*86400;
		double Met;
		sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel;
		lem->GetMissionTime(Met);
//		sprintf(oapiDebugString(),"met=%.1f",Met);
		double times=Met+BurnStartTime-CurrentTimestep;
		int hou=(int) (times/3600.);
		times=(int)(times-hou*3600.);
		int min=(int)(times/60.);
		times=(int)(times-min*60.);
		int sec=(int)(times*100.);
		dsky.SetR1(hou);
		dsky.SetR2(min);
		dsky.SetR3(sec);
		}
		break;


	case 37:
		{
		double Met;
		sat5_lmpkd *lem = (sat5_lmpkd *) OurVessel;
		lem->GetMissionTime(Met);
		double times=Met+BurnStartTime-CurrentTimestep;
		int hou=(int) (times/3600.);
		times=(int)(times-hou*3600.);
		int min=(int)(times/60.);
		times=(int)(times-min*60.);
		int sec=(int)(times*100.);
		dsky.SetR1(hou);
		dsky.SetR2(min);
		dsky.SetR3(sec);
		}
		break;

	case 39:
		{
		double times=BurnStartTime-CurrentTimestep;
		int hou=(int) (times/3600.);
		times=(int)(times-hou*3600.);
		int min=(int)(times/60.);
		times=(int)(times-min*60.);
		int sec=(int)(times*100.);
		dsky.SetR1(hou);
		dsky.SetR2(min);
		dsky.SetR3(sec);
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
	// 60: fwd velocity, Altitude rate, Altitude
	//

	case 60:
		{
			VECTOR3 velocity;
			double cgelev, vlat, vlon, vrad, heading, hvel, cbrg;
			OBJHANDLE hbody=OurVessel->GetGravityRef();
			double bradius=oapiGetSize(hbody);
			OurVessel->GetEquPos(vlon, vlat, vrad);
			OurVessel->GetHorizonAirspeedVector(velocity);
			oapiGetHeading(OurVessel->GetHandle(), &heading);
			hvel=sqrt(velocity.x*velocity.x+velocity.z*velocity.z);
			cbrg=atan2(velocity.x,velocity.z);
			if(velocity.x < 0.0) {
			//retrograde
				cbrg+=2*PI;
			} 
			cgelev=OurVessel->GetCOG_elev();
			CurrentAlt=vrad-bradius-cgelev;
			dsky.SetR1((int)(hvel*cos(cbrg-heading)*10.0));
			dsky.SetR2((int)(velocity.y*10.0));
			dsky.SetR3((int)(CurrentAlt));
		}
		break;

	//
	// 61: time to go, time from ignition, crossrange distance
	//

	case 61:
		{
			// time to go for braking phase...
			double dt = BurnTime;
			double brake=CurrentTimestep-(BurnStartTime+26);
			if(brake > 0) dt=BurnEndTime-CurrentTimestep;
//			if(BurnStartTime < 0.0) dt=BurnTime+BurnStartTime;
			int min = (int) (dt / 60.0);
			int	sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR1(min * 1000 + sec);
			dsky.SetR1Format("XXX XX");

			// time from ignition
			dt=CurrentTimestep-BurnStartTime;
			min = (int) (dt / 60.0);
			sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR2(min * 1000 + sec);
			dsky.SetR2Format("XXX XX");

			dsky.SetR3((int) (DesiredDeltaV ));
		}
		break;

	//
	// 62: Velocity, time from ignition, Accumulated deltav
	//
	case 62:
		{
			dsky.SetR1((int)(CurrentVel*10.0));

			// time from ignition
			double dt=CurrentTimestep-BurnStartTime;
			int min = (int) (dt / 60.0);
			int sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR2(min * 1000 + sec);
			dsky.SetR2Format("XXX XX");

			dsky.SetR3((int)(CurrentRVel*10.0));
		}
		break;

	//
	// 63: delta altitude, altitude rate, altitude
	//

	case 63:
		{
			dsky.SetR1((int)((CurrentAlt-15384.0)));
			dsky.SetR2((int)(DesiredApogee));
			dsky.SetR3((int)(CurrentAlt));
		}
		break;

	//
	// 64: LPD time, LPD, Altitude rate, Altitude
	//

	case 64:
		{
			dsky.SetR1((int)(CutOffVel));
			dsky.SetR1Format("XXX XX");
			dsky.SetR2((int)(DesiredApogee));
			dsky.SetR3((int)(CurrentAlt));
		}
		break;

	//
	// 68: Distance to landing site, time-to-go until P64, velocity
	//

	case 68:
		{
			dsky.SetR1((int)(DeltaPitchRate/100.0));

			double dt=BurnEndTime-CurrentTimestep;
			int min = (int) (dt / 60.0);
			int sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR2(min * 1000 + sec);
			dsky.SetR2Format("XXX XX");
			dsky.SetR3((int)(CurrentVel));
		}
		break;

	//
	// 74: time to launch, yaw angle, heading.
	//

	case 74:
		{
			double dt, hdga, yaw;
			
//			oapiGetFocusHeading(&hdga);			
			dt   = BurnStartTime - CurrentTimestep;			
//			hdga *= DEG;
//			yaw  = DesiredAzimuth - hdga;

			int min = (int) (dt / 60.0);
			int	sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			dsky.SetR1(min * 1000 + sec);
			dsky.SetR1Format("XXX XX");
			dsky.SetR2((int)(DesiredAzimuth * 100.0));
			dsky.SetR3((int)(-54.0 * 100.0));
//			dsky.SetR2((int)(yaw * 100.0));
//			dsky.SetR3((int)(DesiredAzimuth * 100.0));
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
		dsky.SetR1((int)(DesiredDeltaVx*10.0));			//		Insertion hoiriozontal speed, fixed
		dsky.SetR2((int)(DesiredDeltaVy*10.0));			//		Insertion vertical velocity, also fixed
//		dsky.SetR3((int)DesiredAzimuth * 100);		//		Launch azimuth, can be defined
		dsky.SetR3((int)(DesiredPlaneChange/100.0));		
		break;

	//
	// 77: Time to Cutoff, Velocity normal to CSM plane, total velocity
	//

	case 77:
		{
		// time from Cutoff
		double dt=BurnEndTime-CurrentTimestep;
		int min = (int) (dt / 60.0);
		int sec = ((int) dt) - (min * 60);

		if (min > 99) min = 99;

		dsky.SetR1(min * 1000 + sec);
		dsky.SetR1Format("XXX XX");
		dsky.SetR2((int)CurrentVelZ);
		dsky.SetR3((int)CurrentVel);
		}
		break;

	//
	// 81: Local Vertical VGX (up), VGY(right), VGZ(forward)
	//

	case 81:
		dsky.SetR1((int) (DesiredDeltaVx*10.0));
		dsky.SetR2((int) (DesiredDeltaVy*10.0));
		dsky.SetR3((int) (DesiredDeltaVz*10.0));
		break;	

	//
	// 85: Body VGX (up), VGY(right), VGZ(forward)
	//

	case 85:
		dsky.SetR1((int) (DesiredDeltaVx*10.0));
		dsky.SetR2((int) (DesiredDeltaVy*10.0));
		dsky.SetR3((int) (DesiredDeltaVz*10.0));
		break;	

	//
	// 89: for now, landing site definition.
	//
	case 89:
		dsky.SetR1((int) (LandingLatitude * 1000.0));
		dsky.SetR2((int) (LandingLongitude * 500.0));
		dsky.SetR3((int) DisplayAlt(LandingAltitude));
		break;

	case 94:
		{
			double altitude;
			VECTOR3 hvel;
			OurVessel->GetHorizonAirspeedVector(hvel);
			altitude=OurVessel->GetAltitude();

			dsky.SetR1((int)(CurrentVelX*10.0));
			dsky.SetR2((int)(hvel.y*10.0));
			dsky.SetR3((int)(altitude));

/*			double ap;
			OurVessel->GetApDist(ap);

			OBJHANDLE hPlanet = OurVessel->GetGravityRef();
			double aps = ap - oapiGetSize(hPlanet);

			dsky.SetR1((int)(aps  / 10));		//		Display current apoapsis (will be periapsis)
			dsky.SetR2((int)(OurVessel->GetAltitude()  / 10));		//		Display current altitude
			dsky.SetR3((int)(DesiredApogee * 100.0));		//		Display target apoapsis
*/
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
	// 13: Ascent
	//
	case 13:
		return true;

	//
	// 32: CSI
	//

	case 32:
		return true;

	//
	// 33: CDH
	//

	case 33:
		return true;

	//
	// 34: TPI
	//

	case 34:
		return true;

	//
	// 35: TPM
	//

	case 35:
		return true;

	//
	// 36: TPF
	//

	case 36:
		return true;

	//
	//	41: RCS program
	//

	case 41:
		return true;
	//
	//	42: APS program
	//

	case 42:
		return true;

	//
	// 63: braking burn.
	//
	case 63:
		return true;

	//
	// 64: Approach phase
	//
	case 64:
		return true;

	//
	// 65: Landing Phase - Automatic
	//
	case 65:
		return true;

	//
	// 66: Attitude hold, ROD mode
	//
	case 66:
		return true;

	//
	// 68: landing confirmation.
	//
	case 68:
		return true;

	//
	// 70: DPS Abort
	//
	case 70:
		return true;

	//
	// 71: APS Abort
	//
	case 71:
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
	//	13: Ascent program
	//

	case 13:
		Prog13(simt);
		break;

	//
	//  32: Coelliptic Sequence Initiation
	//

	case 32:
		Prog32(simt);
		break;

	//
	//  33: Constant Delta Height
	//

	case 33:
		Prog33(simt);
		break;

	//
	//  34: Transfer Phase Initiation
	//

	case 34:
		Prog34(simt);
		break;

	//
	//  35: Transfer Phase Midcourse
	//

	case 35:
		Prog35(simt);
		break;

	//
	//  36: Transfer Phase Final (fictitious)
	//

	case 36:
		Prog36(simt);
		break;

	//
	// 41: RCS program
	//

	case 41:
		Prog41(simt);
		break;

	//
	// 42: APS program
	//

	case 42:
		Prog42(simt);
		break;

	//
	// 63: braking.
	//

	case 63:
		Prog63(simt);
		break;

	//
	// 64: Approach
	//

	case 64:
		Prog64(simt);
		break;

	//
	// 65: Landing Phase
	//

	case 65:
		Prog65(simt);
		break;

	//
	// 66: Attitude hold, ROD mode
	//

	case 66:
		Prog66(simt);
		break;

	//
	// 68: landing confirmation.
	//

	case 68:
		Prog68(simt);
		break;

	//
	// 70: DPS Abort
	//

	case 70:
		Prog70(simt);
		break;

	//
	// 71: APS Abort
	//

	case 71:
		Prog71(simt);
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

	case 13:
		Prog13Pressed(R1, R2, R3);
		return;	

	case 34:
		Prog34Pressed(R1, R2, R3);
		return;	

	case 41:
		Prog41Pressed(R1, R2, R3);
		return;	

	case 63:
		Prog63Pressed(R1, R2, R3);
		return;

	case 68:
		Prog68Pressed(R1, R2, R3);
		return;
	}

	LightOprErr();
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

	LightOprErr();
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

//
// Special case handling of I/O channel changes.
//

void LEMcomputer::SetInputChannelBit(int channel, int bit, bool val)

{
	ApolloGuidance::SetInputChannelBit(channel, bit, val);

	switch (channel)
	{
	case 030:
		if ((bit == 1) && val) {
			RunProgram(70);			// Abort with descent stage
		}
		else if ((bit == 4) && val) {
			RunProgram(71);			// Abort with ascent stage.
		}
		break;
	}
}

bool LEMcomputer::OrbitCalculationsValid()

{
	return DescentPhase() || AscentPhase();
}


int LEMcomputer::GetStatus(double *simcomputert,
                    int    *mode,
				    double *timeremaining,
					double *timeafterpdi,
					double *timetoapproach)
				
{
	*simcomputert = this->simcomputert;
	*mode = this->mode;
	*timeremaining = this->timeremaining;
	*timeafterpdi = this->timeafterpdi;
	*timetoapproach = this->timetoapproach;
	return true;
}

int LEMcomputer::SetStatus(double simcomputert,
                       int    mode,
				       double timeremaining,
					   double timeafterpdi,
					   double timetoapproach)
					   
{
	this->simcomputert = simcomputert;
	this->mode = mode;
	this->timeremaining = timeremaining;
	this->timeafterpdi = timeafterpdi;
	this->timetoapproach = timetoapproach;
	return true;
}
