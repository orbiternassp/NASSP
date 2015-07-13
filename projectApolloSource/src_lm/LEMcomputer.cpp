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
  *	Revision 1.15  2011/07/11 01:42:36  vrouleau
  *	- Removed AGC_SOCKET_ENABLED flag. Rework is needed to make this an optional feature instead of a conditional define. To many untested think exists in the socket version
  *	
  *	- Checkpoint commit on the LEM RR. If the RR as been slew to track the CSM , the auto mode will continue tracking it.
  *	
  *	Revision 1.14  2011/07/07 11:58:45  vrouleau
  *	Checkpoint commit for LEM rendezvous radar:
  *	 - Added range,rate and CSM direction calculation.
  *	 - Slewing of the shaft & trunnion
  *	 - IO from/to AGC.
  *	
  *	Revision 1.13  2010/08/28 16:16:33  dseagrav
  *	Fixed LM DSKY to use dimmer. (Dimmer source may be wrong)
  *	Corrected a typo and did some bracketization in DSKY source.
  *	Wasted a lot of time figuring out our copy of Luminary had been garbaged.
  *	Your Luminary 99 binary should be overwritten by the commit before this one.
  *	If your binary is 700 bytes in size you have the garbaged version.
  *	
  *	Revision 1.12  2010/05/24 03:50:34  dseagrav
  *	Updates to RCS, CWEA, ATCA
  *	
  *	Revision 1.11  2010/05/12 05:01:30  dseagrav
  *	CWEA stuff for LGC and ISS, beginnings of ECS
  *	
  *	Revision 1.10  2010/02/19 00:55:43  dseagrav
  *	Add Apollo 11 LM Pad Load (this was the flown pad load; It will be correct.)
  *	
  *	Revision 1.9  2009/10/19 12:24:49  dseagrav
  *	LM checkpoint commit.
  *	Put back one FDAI for testing purposes (graphic is wrong)
  *	Messed around with mass properties
  *	LGC now runs Luminary 099 instead of 131
  *	Added LGC pad load, values need worked but addresses are checked.
  *	
  *	Revision 1.8  2009/09/13 15:20:14  dseagrav
  *	LM Checkpoint Commit. Adds LM telemetry, fixed missing switch row init, initial LM support for PAMFD.
  *	
  *	Revision 1.7  2009/09/02 18:26:46  vrouleau
  *	MultiThread support for vAGC
  *	
  *	Revision 1.6  2009/09/01 06:18:32  dseagrav
  *	LM Checkpoint Commit. Added switches. Added history to LM SCS files. Added bitmap to LM. Added AIDs.
  *	
  *	Revision 1.5  2009/08/16 03:12:38  dseagrav
  *	More LM EPS work. CSM to LM power transfer implemented. Optics bugs cleared up.
  *	
  *	Revision 1.4  2009/08/02 19:20:24  spacex15
  *	AGC socket version reenabled
  *	
  *	Revision 1.3  2009/08/01 23:06:33  jasonims
  *	LM Optics Code Cleaned Up... Panel Code added for LM Optics... Knobs activated... Counter and Computer Controls still to come.
  *	
  *	Revision 1.2  2009/08/01 19:48:33  jasonims
  *	LM Optics Code Added, along with rudimentary Graphics for AOT.
  *	Reticle uses GDI objects to allow realtime rotation.
  *	
  *	Revision 1.1  2009/02/18 23:21:14  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.39  2008/04/11 11:49:11  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.38  2007/10/18 00:23:16  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.37  2007/10/07 22:29:10  tschachim
  *	Bugfixes
  *	
  *	Revision 1.36  2007/10/05 13:39:03  dseagrav
  *	Cause vLGC restart if unpowered.
  *	
  *	Revision 1.35  2007/06/06 15:02:09  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.34  2007/04/26 00:18:18  movieman523
  *	Fixed some build warnings.
  *	
  *	Revision 1.33  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.32  2006/08/13 16:01:52  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  *	Revision 1.31  2006/08/13 06:30:49  dseagrav
  *	LM checkpoint commit.
  *	
  *	Revision 1.30  2006/07/26 15:42:02  tschachim
  *	Temporary fix of the lm landing autopilot until correct attitude control is ready.
  *	
  *	Revision 1.29  2006/06/18 22:45:30  dseagrav
  *	LM ECA bug fix, LGC,IMU,DSKY and IMU OPR wired to CBs, IMU OPR,LGC,FDAI,and DSKY draw power
  *	
  *	Revision 1.28  2006/04/25 08:11:27  dseagrav
  *	Crash avoidance for DEBUG builds, LM IMU correction, LM still needs more work
  *	
  *	Revision 1.27  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.26  2006/01/14 20:58:15  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.25  2006/01/11 19:57:55  movieman523
  *	Load appropriate AGC binary file based on mission number.
  *	
  *	Revision 1.24  2005/09/24 20:56:08  lazyd
  *	Moved some code
  *	
  *	Revision 1.23  2005/09/20 22:37:48  lazyd
  *	Moved programs from here to apolloguidance
  *	
  *	Revision 1.22  2005/09/05 21:11:18  lazyd
  *	Added noun 54 for rendezvous
  *	
  *	Revision 1.21  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.20  2005/08/27 23:35:54  lazyd
  *	Added P30 but don't use it yet
  *	
  *	Revision 1.19  2005/08/22 19:47:33  movieman523
  *	Fixed long timestep on startup, and added new Virtual AGC with EDRUPT fix.
  *	
  *	Revision 1.18  2005/08/13 20:00:28  lazyd
  *	Fixed noun 77
  *	
  *	Revision 1.17  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.16  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.15  2005/08/09 09:24:01  tschachim
  *	Introduced toggleswitch lib
  *	
  *	Revision 1.14  2005/08/09 02:28:25  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.13  2005/08/08 22:32:49  movieman523
  *	First steps towards reimplementing the DSKY interface to use the same I/O channels as the real AGC/DSKY interface.
  *	
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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "IMU.h"
#include "lvimu.h"
#include "csmcomputer.h"
#include "lemcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "ioChannels.h"
#include "LEM.h"

#include "lm_channels.h"

LEMcomputer::LEMcomputer(SoundLib &s, DSKY &display, IMU &im, PanelSDK &p) : ApolloGuidance(s, display, im, p)

{

	isLGC = true;
	FlagWord0.word = 0;
	FlagWord1.word = 0;
	FlagWord2.word = 0;

	ProgFlag01 = false;
	ProgFlag02 = false;
	ProgFlag03 = false;
	ProgFlag04 = false;
	RetroFlag = false;
	BurnFlag = false;

	int i;
	for (i = 0; i < 16; i++) {
		RCSCommand[i] = 0;
	}
	CommandedAttitudeRotLevel = _V(0, 0, 0);
	CommandedAttitudeLinLevel = _V(0, 0, 0);

	//
	// Default ascent parameters.
	//

	DesiredApogee = 82.250;
	DesiredPerigee = 74.360;
	DesiredAzimuth = 270.0;

	InOrbit = 1;

    mode = -1;
	simcomputert = -1.0;
    timeremaining = 99999.0;
	timeafterpdi = -1.0;

	/* FIXME LOAD FILE SHOULD BE SET IN SCENARIO */
	InitVirtualAGC("Config/ProjectApollo/Luminary099.bin");

	/* FIXME REMOVE THIS LATER, THIS IS TEMPORARY FOR TESTING ONLY AND SHOULD BE IN THE SCENARIO LATER */
	/* LM PAD LOAD FOR LUMINARY 099 AND APOLLO 11  - OFFICIAL VERSION */

	thread.Resume();
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

			SetR1(tsph);
			SetR2(tspm);
			SetR3(tsps);
		}
		break;

	//
	// 18: pitch, roll and yaw angles
	//

	case 18:
		SetR1((int) CurrentPitch);
		SetR2((int) CurrentRoll);
		SetR3((int) CurrentYaw);
		break;

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
			if (min > 99)
				min = 99;
			SetR2(min * 1000 + sec);
			SetR2Format("XXX XX");

			SetR3((int) (DesiredDeltaV/100.0 ));
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

			SetR1(min * 1000 + sec);
			SetR1Format("XXX XX");
			sect=BurnEndTime-BurnStartTime;
			if(dt > 0.0) {
				secs=BurnEndTime-CurrentTimestep;
				SetR2((int) secs);
			} else {
				secs=sect;
				SetR2((int) sect);
			}

			dv.x=DesiredDeltaVx;
			dv.y=DesiredDeltaVy;
			dv.z=DesiredDeltaVz;
			dvr=Mag(dv)*(secs/sect);
			SetR3((int) (dvr*10.0));
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
		LEM *lem = (LEM *) OurVessel;
		Met = lem->GetMissionTime();
//		sprintf(oapiDebugString(),"met=%.1f",Met);
		double times=Met+BurnStartTime-CurrentTimestep;
		int hou=(int) (times/3600.);
		times=(int)(times-hou*3600.);
		int min=(int)(times/60.);
		times=(int)(times-min*60.);
		int sec=(int)(times*100.);
		SetR1(hou);
		SetR2(min);
		SetR3(sec);
		}
		break;


	case 37:
		{
		double Met;
		LEM *lem = (LEM *) OurVessel;
		Met = lem->GetMissionTime();
		double times=Met+BurnStartTime-CurrentTimestep;
		int hou=(int) (times/3600.);
		times=(int)(times-hou*3600.);
		int min=(int)(times/60.);
		times=(int)(times-min*60.);
		int sec=(int)(times*100.);
		SetR1(hou);
		SetR2(min);
		SetR3(sec);
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
		SetR1(hou);
		SetR2(min);
		SetR3(sec);
		}
		break;


	//
	// 45: Plane change angle, target LAN (Used by P32)
	//

	case 45:		
		SetR1((int)(DesiredPlaneChange * 100));		
		SetR2((int)(DesiredLAN));		
		BlankR3();
		break;

	//
	// 50: apogee, perigee, fuel
	//

	case 50:
		if (OrbitCalculationsValid())
		{
			DisplayOrbitCalculations();
			SetR3((int)((OurVessel->GetFuelMass() / OurVessel->GetMaxFuelMass()) * 10000.0));
		}
		break;

	//
	// 54: range, range rate, theta
	//

	case 54:
		double range, rate, phase, delta;
		Phase(phase, delta);
		Radar(range, rate);
		SetR1 ((int) (range/10.0));
		SetR2 ((int) (rate*10.0));
		SetR3 ((int) (phase*DEG*100.0));
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
			SetR1((int)(hvel*cos(cbrg-heading)*10.0));
			SetR2((int)(velocity.y*10.0));
			SetR3((int)(CurrentAlt));
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

			SetR1(min * 1000 + sec);
			SetR1Format("XXX XX");

			// time from ignition
			dt=CurrentTimestep-BurnStartTime;
			min = (int) (dt / 60.0);
			sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			SetR2(min * 1000 + sec);
			SetR2Format("XXX XX");

			SetR3((int) (DesiredDeltaV ));
		}
		break;

	//
	// 62: Velocity, time from ignition, Accumulated deltav
	//
	case 62:
		{
			SetR1((int)(CurrentVel*10.0));

			// time from ignition
			double dt=CurrentTimestep-BurnStartTime;
			int min = (int) (dt / 60.0);
			int sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			SetR2(min * 1000 + sec);
			SetR2Format("XXX XX");

			SetR3((int)(CurrentRVel*10.0));
		}
		break;

	//
	// 63: delta altitude, altitude rate, altitude
	//

	case 63:
		{
			SetR1((int)((CurrentAlt-15384.0)));
			SetR2((int)(DesiredApogee));
			SetR3((int)(CurrentAlt));
		}
		break;

	//
	// 64: LPD time, LPD, Altitude rate, Altitude
	//

	case 64:
		{
			SetR1((int)(CutOffVel));
			SetR1Format("XXX XX");
			SetR2((int)(DesiredApogee));
			SetR3((int)(CurrentAlt));
		}
		break;

	//
	// 68: Distance to landing site, time-to-go until P64, velocity
	//

	case 68:
		{
			SetR1((int)(DeltaPitchRate/100.0));

			double dt=BurnEndTime-CurrentTimestep;
			int min = (int) (dt / 60.0);
			int sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			SetR2(min * 1000 + sec);
			SetR2Format("XXX XX");
			SetR3((int)(CurrentVel));
		}
		break;

	//
	// 74: time to launch, yaw angle, heading.
	//

	case 74:
		{
			double dt;
			
//			oapiGetFocusHeading(&hdga);			
			dt   = BurnStartTime - CurrentTimestep;			
//			hdga *= DEG;
//			yaw  = DesiredAzimuth - hdga;

			int min = (int) (dt / 60.0);
			int	sec = ((int) dt) - (min * 60);

			if (min > 99)
				min = 99;

			SetR1(min * 1000 + sec);
			SetR1Format("XXX XX");
			SetR2((int)(DesiredAzimuth * 100.0));
			SetR3((int)(-54.0 * 100.0));
//			SetR2((int)(yaw * 100.0));
//			SetR3((int)(DesiredAzimuth * 100.0));
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

			SetR1(min * 1000 + sec);
			SetR1Format("XXX XX");
			SetR2((int)DesiredLAN);
			SetR3((int)(DesiredPlaneChange * 100));
		}		
		break;

	//
	// 76: Desired, downrange and radial velocity and azimuth
	//

	case 76:		
		SetR1((int)(DesiredDeltaVx*10.0));			//		Insertion hoiriozontal speed, fixed
		SetR2((int)(DesiredDeltaVy*10.0));			//		Insertion vertical velocity, also fixed
//		SetR3((int)DesiredAzimuth * 100);		//		Launch azimuth, can be defined
		SetR3((int)(DesiredPlaneChange/100.0));		
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

		SetR1(min * 1000 + sec);
		SetR1Format("XXX XX");
		SetR2((int)(LandingAltitude*10.0));
		SetR3((int)(CurrentVel*10.0));
		}
		break;

	//
	// 81: Local Vertical VGX (up), VGY(right), VGZ(forward)
	//

	case 81:
		SetR1((int) (DesiredDeltaVx*10.0));
		SetR2((int) (DesiredDeltaVy*10.0));
		SetR3((int) (DesiredDeltaVz*10.0));
		break;	

	//
	// 85: Body VGX (up), VGY(right), VGZ(forward)
	//

	case 85:
		SetR1((int) (DesiredDeltaVx*10.0));
		SetR2((int) (DesiredDeltaVy*10.0));
		SetR3((int) (DesiredDeltaVz*10.0));
		break;	

	//
	// 89: for now, landing site definition.
	//
	case 89:
		SetR1((int) (LandingLatitude * 1000.0));
		SetR2((int) (LandingLongitude * 500.0));
		SetR3((int) DisplayAlt(LandingAltitude));
		break;

	case 94:
		{
			double altitude;
			VECTOR3 hvel;
			OurVessel->GetHorizonAirspeedVector(hvel);
			altitude=OurVessel->GetAltitude();

			SetR1((int)(CurrentVelX*10.0));
			SetR2((int)(hvel.y*10.0));
			SetR3((int)(altitude));

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

		BlankData();

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
	//	12: Ascent 
	//
	case 12:
		return true;

	//
	// 13: Ascent
	//
	case 13:
		return true;


	//
	// 30: External Delta V
	//

	case 30:
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
	//	40: DPS program
	//

	case 40:
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


void LEMcomputer::agcTimestep(double simt, double simdt)
{
	GenericTimestep(simt, simdt);
}

void LEMcomputer::Run ()
{
	while(true)
	{
		timeStepEvent.Wait();
		{
			Lock lock(agcCycleMutex);
			agcTimestep(thread_simt,thread_simdt);
		}
	}
};


void LEMcomputer::Timestep(double simt, double simdt)

{
	LEM *lem = (LEM *) OurVessel;
	// If the power is out, the computer should restart.
	if (Yaagc ){
		// HARDWARE MUST RESTART
		if( !IsPowered() ) {
			if(vagc.Erasable[0][05] != 04000){		
				// Clear flip-flop based registers
				vagc.Erasable[0][00] = 0;     // A
				vagc.Erasable[0][01] = 0;     // L
				vagc.Erasable[0][02] = 0;     // Q
				vagc.Erasable[0][03] = 0;     // EB
				vagc.Erasable[0][04] = 0;     // FB
				vagc.Erasable[0][05] = 04000; // Z
				vagc.Erasable[0][06] = 0;     // BB
				// Clear ISR flag
				vagc.InIsr = 0;
				// Clear interrupt requests
				vagc.InterruptRequests[0] = 0;
				vagc.InterruptRequests[1] = 0;
				vagc.InterruptRequests[2] = 0;
				vagc.InterruptRequests[3] = 0;
				vagc.InterruptRequests[4] = 0;
				vagc.InterruptRequests[5] = 0;
				vagc.InterruptRequests[6] = 0;
				vagc.InterruptRequests[7] = 0;
				vagc.InterruptRequests[8] = 0;
				vagc.InterruptRequests[9] = 0;
				vagc.InterruptRequests[10] = 0;
				// Reset cycle counter and Extracode flags
				vagc.CycleCounter = 0;
				vagc.ExtraCode = 0;
				vagc.ExtraDelay = 0;
				// No idea about the interrupts/pending/etc so we reset those
				vagc.AllowInterrupt = 0;				  
				vagc.PendFlag = 0;
				vagc.PendDelay = 0;
				// Don't disturb erasable core
				// IO channels are flip-flop based and should reset, but that's difficult, so we'll ignore it.
				// Light OSCILLATOR FAILURE and LGC WARNING bits to signify power transient, and be forceful about it
				// Those two bits are what causes the CWEA to notice.
				InputChannel[033] &= 017777;
				vagc.InputChannel[033] &= 017777;				
				OutputChannel[033] &= 017777;				
				vagc.Ch33Switches &= 017777;
				// Also, simulate the operation of the VOLTAGE ALARM and light the RESTART light on the DSKY.
				// This happens externally to the AGC program. See CSM 104 SYS HBK pg 399
				vagc.VoltageAlarm = 1;
				dsky.LightRestart();
			}
			// and do nothing more.
			return;
		}
		
		//
		// If MultiThread is enabled and the simulation is accellerated, the run vAGC in the AGC Thread,
		// otherwise run in main thread. at x1 acceleration, it is better to run vAGC totally synchronized
		//
		if(lem->isMultiThread && oapiGetTimeAcceleration() > 1.0){
			Lock lock(agcCycleMutex);
			thread_simt = simt;
			thread_simdt = simdt;
			timeStepEvent.Raise();
		}else{
			agcTimestep(simt,simdt);
		}
		return;
	}
	if (GenericTimestep(simt, simdt))
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
	//  29: Plane Change
	//

//	case 29:
//		Prog29(simt);
//		break;


	//
	//  30: External Delta V
	//

	case 30:
		Prog30(simt);
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
	// 40: DPS program
	//

	case 40:
		Prog40(simt);
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

	BlankData();
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
		FlagWord0.word = val;
		ProgFlag01 = FlagWord0.u.FREEFBIT;
		break;

	case 1:
		FlagWord1.word = val;
		ProgFlag02 = FlagWord1.u.NOTASSIGNED;
		ProgFlag03 = FlagWord1.u.NOTASSIGNED2;
		ProgFlag04 = FlagWord1.u.NOTASSIGNED3;
		break;

	case 2:
		FlagWord2.word = val;
		RetroFlag = FlagWord2.u.NOTASSIGNED;
		break;
	}
}

//
// Access simulated erasable memory.
//

bool LEMcomputer::ReadMemory(unsigned int loc, int &val)

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
			val = (int)(LandingLatitude * 100000000.0 - ((int)(LandingLatitude * 1000.0)) * 100000.0);
			return true;

		case 0114:
			val = (int)(LandingLongitude * 100000000.0 - ((int)(LandingLongitude * 1000.0)) * 100000.0);
			return true;
		}
	}

	return GenericReadMemory(loc, val);
}

void LEMcomputer::WriteMemory(unsigned int loc, int val)

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
	else {
		GenericWriteMemory(loc, val);
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

// DS20060413

void LEMcomputer::ProcessChannel13(int val){
	LEM *lem = (LEM *) OurVessel;	
	LMChannelValue13 ch13;
	ch13.Value = val;
	if(ch13.Bits.EnableRHCCounter && ch13.Bits.RHCRead){
		int rhc_count[3];
		rhc_count[0] = lem->rhc_pos[0]/550;
		rhc_count[1] = lem->rhc_pos[1]/550;
		rhc_count[2] = lem->rhc_pos[2]/550;
		
		WriteMemory(042,rhc_count[1]); // PITCH 
		WriteMemory(043,rhc_count[2]); // YAW   
		WriteMemory(044,rhc_count[0]); // ROLL  		
		/*
		sprintf(oapiDebugString(),"LM CH13: %o RHC: SENT %d %d %d",val,
			rhc_count[0],rhc_count[1],rhc_count[2]);
		return;
		*/
	}
//	sprintf(oapiDebugString(),"LM CH13: %o",val);	

}

void LEMcomputer::ProcessChannel5(int val){
	// This is now handled inside the ATCA
	LEM *lem = (LEM *) OurVessel;	
	lem->atca.ProcessLGC(5,val);
}

void LEMcomputer::ProcessChannel6(int val){	
	// This is now handled inside the ATCA
	LEM *lem = (LEM *) OurVessel;	
	lem->atca.ProcessLGC(6,val);
}


void LEMcomputer::ProcessChannel160(int val) {
	
	ChannelValue12 val12;
	val12.Value = GetOutputChannel(012);
	LEM *lem = (LEM *) OurVessel;
	
	lem->RR.RRShaftDrive(val,val12.Value);
	
}

void LEMcomputer::ProcessChannel161(int val) {

	ChannelValue12 val12;
	val12.Value = GetOutputChannel(012);
	LEM *lem = (LEM *) OurVessel;
	lem->RR.RRTrunionDrive(val,val12.Value);
}


// Process IMU CDU error counters.
void LEMcomputer::ProcessIMUCDUErrorCount(int channel, unsigned int val){
	// FULL NEEDLE DEFLECTION is 16.88 DEGREES
	// 030 PULSES = MAX IN ONE RELAY EVENT
	// 22 PULSES IS ONE DEGREE, 384 PULSES = FULL SCALE
	// 0.10677083 PIXELS PER PULSE
	LEM *lem = (LEM *) OurVessel;	
	LMChannelValue12 val12;
	if(channel != 012){ val12.Value = GetOutputChannel(012); }else{ val12.Value = val; }
	// 174 = X, 175 = Y, 176 = Z
	if(val12.Bits.CoarseAlignEnable){ return; } // Does not apply to us here.
	switch(channel){
	case 012:
		if(val12.Bits.EnableIMUCDUErrorCounters){
			if(lem->atca.lgc_err_ena == 0){
				// sprintf(oapiDebugString(),"LEM: LGC-ERR: RESET");
				lem->atca.lgc_err_x = 0;
				lem->atca.lgc_err_y = 0;
				lem->atca.lgc_err_z = 0;
				lem->atca.lgc_err_ena = 1;
			}
		}else{
			lem->atca.lgc_err_ena = 0;
		}
		break;

	case 0174: // YAW ERROR
		if(val12.Bits.EnableIMUCDUErrorCounters){
			int delta = val&0777;
			// NEGATIVE = RIGHT
			if(val&040000){
				lem->atca.lgc_err_z -= delta;
			}else{
				lem->atca.lgc_err_z += delta;
			}
		}
		// sprintf(oapiDebugString(),"LEM: LGC-ERR: %d %d %d",lem->atca.lgc_err_x,lem->atca.lgc_err_y,lem->atca.lgc_err_z);
		break;
	
	case 0175: // PITCH ERROR
		if(val12.Bits.EnableIMUCDUErrorCounters){
			int delta = val&0777;
			// NEGATIVE = DOWN
			if(val&040000){
				lem->atca.lgc_err_y -= delta;
			}else{
				lem->atca.lgc_err_y += delta;
			}
		}
		// sprintf(oapiDebugString(),"LEM: LGC-ERR: %d %d %d",lem->atca.lgc_err_x,lem->atca.lgc_err_y,lem->atca.lgc_err_z);
		break;

	case 0176: // ROLL ERROR
		if(val12.Bits.EnableIMUCDUErrorCounters){
			int delta = val&0777;
			// NEGATIVE = RIGHT
			if(val&040000){
				lem->atca.lgc_err_x += delta;
			}else{
				lem->atca.lgc_err_x -= delta;
			}
		}
		// sprintf(oapiDebugString(),"LEM: LGC-ERR: %d %d %d",lem->atca.lgc_err_x,lem->atca.lgc_err_y,lem->atca.lgc_err_z);
		break;
	}

}

void LEMcomputer::BurnMainEngine(double thrust)

{
	/// \todo This function is used by the AGC++ burn programs
	/// and should simulate the VAGC behaviour, i.e. I/O channels etc.
	/// to control the main engine

	OurVessel->SetEngineLevel(ENGINE_HOVER, thrust);

	ApolloGuidance::BurnMainEngine(thrust);
}

/// \todo Dirty Hack for the AGC++ RCS burn control, 
/// remove this and use I/O channels and pulsed thrusters 
/// identical to the VAGC instead
///

void LEMcomputer::ResetAttitudeLevel() {

	int i;
	for (i = 0; i < 16; i++) {
		RCSCommand[i] = 0;
	}
}

void LEMcomputer::AddAttitudeRotLevel(VECTOR3 level) {

	int i;

	// Pitch
	if (level.x < 0) {
		RCSCommand[0] -= level.x;
		RCSCommand[7] -= level.x;
		RCSCommand[11] -= level.x;
		RCSCommand[12] -= level.x;
	}
	else {
		RCSCommand[3] += level.x;
		RCSCommand[4] += level.x;
		RCSCommand[8] += level.x;
		RCSCommand[15] += level.x;
	}

	// Roll
	if (level.z < 0) {
		RCSCommand[0] -= level.z;
		RCSCommand[4] -= level.z;
		RCSCommand[11] -= level.z;
		RCSCommand[15] -= level.z;
	}
	else {
		RCSCommand[3] += level.z;
		RCSCommand[7] += level.z;
		RCSCommand[8] += level.z;
		RCSCommand[12] += level.z;
	}

	// Yaw
	if (level.y > 0) {
		RCSCommand[1] += level.y;
		RCSCommand[5] += level.y;
		RCSCommand[10] += level.y;
		RCSCommand[14] += level.y;
	}
	else {
		RCSCommand[2] -= level.y;
		RCSCommand[6] -= level.y;
		RCSCommand[9] -= level.y;
		RCSCommand[13] -= level.y;
	}

	// Renormalize
	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] > 1) {
			RCSCommand[i] = 1;
		}
	}

	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] < -1) {
			RCSCommand[i] = -1;
		}
	}

	// Set thrust
	LEM *lem = (LEM *) OurVessel;
	for (i = 0; i < 16; i++) {
		lem->SetRCSJetLevelPrimary(i, RCSCommand[i]);
	}
}

void LEMcomputer::AddAttitudeLinLevel(int axis, double level) {

	VECTOR3 l = _V(0, 0, 0);
	if (axis == 0) l.x = level;
	if (axis == 1) l.y = level;
	if (axis == 2) l.z = level;
	AddAttitudeLinLevel(l);
}

void LEMcomputer::AddAttitudeLinLevel(VECTOR3 level) {

	int i;

	// Left/right
	if (level.x < 0) {
		RCSCommand[9] -= level.x;
		RCSCommand[14] -= level.x;
	}
	else {
		RCSCommand[2] += level.x;
		RCSCommand[5] += level.x;
	}

	// Down/up
	if (level.y < 0) {
		RCSCommand[0] -= level.y;
		RCSCommand[4] -= level.y;
		RCSCommand[8] -= level.y;
		RCSCommand[12] -= level.y;
	}
	else {
		RCSCommand[3] += level.y;
		RCSCommand[7] += level.y;
		RCSCommand[11] += level.y;
		RCSCommand[15] += level.y;
	}

	// Back/forward
	if (level.z < 0) {
		RCSCommand[1] -= level.z;
		RCSCommand[13] -= level.z;
	}
	else {
		RCSCommand[6] += level.z;
		RCSCommand[10] += level.z;
	}

	// Renormalize
	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] > 1) {
			RCSCommand[i] = 1;
		}
	}

	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] < -1) {
			RCSCommand[i] = -1;
		}
	}

	// Set thrust
	LEM *lem = (LEM *) OurVessel;
	for (i = 0; i < 16; i++) {
		lem->SetRCSJetLevelPrimary(i, RCSCommand[i]);
	}
}

void LEMcomputer::SetAttitudeRotLevel(VECTOR3 level) {

	ResetAttitudeLevel();
	AddAttitudeRotLevel(level);

}

//
// LM Optics class code
//

LMOptics::LMOptics() {

	lem = NULL;
	OpticsShaft = 3;
	OpticsReticle = 0.0;
	ReticleMoved = 0;
	RetDimmer = 255;
	KnobTurning = 0;
	
}

void LMOptics::Init(LEM *vessel) {

	lem = vessel;
}

void LMOptics::SystemTimestep(double simdt) {

	// LEM Optics is a manual system... no power required.
	// There were however heaters that would keep the optics from freezing or fogging.
	// Might want to implment those...


}

void LMOptics::TimeStep(double simdt) {
	OpticsReticle = OpticsReticle + simdt * ReticleMoved;
	// sprintf(oapiDebugString(), "Optics Shaft %.2f, Optics Reticle %.2f, Moved? %.4f, KnobTurning %d", OpticsShaft/RAD, OpticsReticle/RAD, ReticleMoved, KnobTurning);
	if (OpticsReticle > 2*PI) OpticsReticle -= 2*PI;
	if (OpticsReticle < 0) OpticsReticle += 2*PI;
}

void LMOptics::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, LMOPTICS_START_STRING);
	papiWriteScenario_double(scn, "OPTICSSHAFT", OpticsShaft);
	papiWriteScenario_double(scn, "OPTICSRETICLE", OpticsReticle);
	papiWriteScenario_double(scn, "RETDIMMER", RetDimmer);
	oapiWriteLine(scn, LMOPTICS_END_STRING);
}

void LMOptics::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, LMOPTICS_END_STRING, sizeof(LMOPTICS_END_STRING)))
			return;
		else if (!strnicmp (line, "OPTICSSHAFT", 11)) {
			sscanf (line+11, "%lf", &OpticsShaft);
		}
		else if (!strnicmp (line, "OPTICSRETICLE", 11)) {
			sscanf (line+11, "%lf", &OpticsReticle);
		}
		else if (!strnicmp (line, "RETDIMMER", 11)) {
			sscanf (line+11, "%lf", &RetDimmer);
		}
	}
}
