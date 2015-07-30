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

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "soundlib.h"

#include "nasspdefs.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "lemcomputer.h"
#include "IMU.h"

#include "LEM.h"

void LEMcomputer::Prog13Pressed(int R1, int R2, int R3)
{
	switch(ProgState)
	{
	case 2:
		if (R1 < 0 || R2 < 0) 
		{
			LightOprErr();
			return;
		}

		if (R3 < 0 || R3 >= 36000)
		{
			LightOprErr();
			return;
		}
		
		double hdg; 
		DesiredAzimuth = R3 / 100.0;
		oapiGetHeading(OurVessel->GetHandle(), &hdg);		
		hdg *= DEG;

		//
		//	Since we have time to yaw only 26 degrees to either side before pitchover starts,
		//	we need to adjust the desired azimuth to fit within the limits, if it doesn't
		//
				
		if(DesiredAzimuth > (hdg + 26))
			DesiredAzimuth = hdg + 26;

		if(DesiredAzimuth < (hdg - 26))
			DesiredAzimuth = hdg - 26;	
		
		ProgState++;		
		return;

	case 4:
		UpdateBurnTime(R1, R2, R3);
		BurnStartTime = BurnTime;
		ProgState++;
		return;

	case  6:
	case 10:
		//
		// Do nothing, just step on through the program.
		//
		ProgState++;
		return;
	}
	LightOprErr();
}

void LEMcomputer::Prog13 (double simt)
{
	//
	//	This program inserts us into a preliminary orbit, called the insertion orbit. Periapsis
	//	fixed at 17.5 km and apoapsis close to 87 km. Actual results may vary by a small amount
	//	due to the weak moon's gravity, which make small velocity changes have a considerable 
	//	effect on the orbit geometry.
	//

	OBJHANDLE gBody;			//		Handle for the gravity body (in our case, the moon)	
	VECTOR3 horizonvel;			//		Velocity vector relative to the horizon plane
	VECTOR3 PMI;				//		Vector to store the moments if inertia
	VECTOR3 angvel;				//		Angular velocity

	double ts, sb, currentapo, currentper, burntime, remainbt, residualV;
	double hdg, alt, pitch, Mass, Size, Thrust, MaxThrust, hveltogain;
	double moonrad, Level, npitch, alpha, insertionalt, newaccangle;
	double remalt, vvelcalc, vveldiff, vcalcacc, hcalcacc, absvel;
	double moong, cmoong;
	int thdir;

	moong = 1.62;				//		Moon gravity acceleration (m/sec/sec)

	ts = simt - LastTimestep;	//		Get Time Step	
	burntime = 410;				//		Engine burntime, secs (total is 431)
	insertionalt = 18000;		//		Insertion altitude (meters) - Lower to allow for error

	//
	//	These are our insertion velocities. They are just like the real ones, fixed.
	//	The values below are the ones from Apollo 11.
	//	
		
	//
	//	Let's gather all the data we need from Orbiter itself
	//
			
	oapiGetHeading(OurVessel->GetHandle(),&hdg);
	OurVessel->GetAngularVel(angvel);
	OurVessel->GetHorizonAirspeedVector(horizonvel);
	OurVessel->GetApDist(currentapo);
	OurVessel->GetPeDist(currentper);
	OurVessel->GetPMI(PMI);
	
	gBody	   = OurVessel->GetGravityRef();
	moonrad	   = oapiGetSize(gBody);
	currentapo -= moonrad;
	currentper -= moonrad;
	
	MaxThrust = OurVessel->GetMaxThrust(ENGINE_ATTITUDE);
	Mass	  = OurVessel->GetMass();
	Size	  = OurVessel->GetSize();
	alt		  = OurVessel->GetAltitude();
	pitch	  = OurVessel->GetPitch();
	alpha	  = OurVessel->GetAOA();
	alpha     *= DEG;
	pitch	  *= DEG;
	hdg		  *= DEG;
	
	absvel	   = sqrt(pow(horizonvel.x,2) + pow(horizonvel.y,2) + pow(horizonvel.z,2));

	switch (ProgState)
	{
	case 0:
		//
		//	Set program defaults
		//

		DesiredDeltaVx = 1687;		//		Downrange velocity (horizontal)		
		DesiredDeltaVy = 9;			//		Radial velocity (vertical)
		DesiredAzimuth = 270;		//		Default launch azimuth

		ProgState++;
		break;

	case 1:
		SetVerbNounAndFlash(6, 76);
		ProgState++;
		break;

	case 3:
		SetVerbNounAndFlash(6, 33);
		ProgState++;
		break;

	case 5:
		Checklist(0203);
		ProgState++;
		break;

	case 7:
		SetVerbNoun(6, 74);
		NextEventTime = BurnStartTime - 35.0;
		ProgState++;
		break;

	case 8:
		if (simt >= NextEventTime) 
		{
			BlankAll();

			VerbRunning = 0;
			NounRunning = 0;

			NextEventTime = BurnStartTime - 30.0;
			ProgState++;
		}
		break;

	case 9:
		if (simt >= NextEventTime) 
		{
			UnBlankAll();
			SetVerbNoun(6, 74);
			SetVerbNounAndFlash(99, 74);
			ProgState++;
		}
		break;

	case 11:
		if (simt >= BurnStartTime) 
		{
			AwaitProgram();
			ProgState = 100;
			break;
		}
		SetVerbNoun(16, 74);
		ProgState++;
		break;

	case 12:
		if (simt >= BurnStartTime) 
		{
			LEM *lem = (LEM *) OurVessel;

			lem->StartAscent();				
			LastEventTime = simt;
			NextEventTime = simt + 0.1;
			ProgState++;
		}
		break;

	//
	//	Up to here, all we did was process the input data an wait for the lift off time (TIG).
	//	Now, TIG has occured and we are to control the LM through it's ascent.
	//

	case 13:
		//
		//	Wait 2 seconds before any maneuvering, to clear the Descent Stage
		//

		if(simt >= (LastEventTime + 2))
		{
			LastEventTime = simt;
			ProgState++;
		}
		break;

	case 14:
		//
		//	Yaw to desired azimuth
		//

		if((DesiredAzimuth - hdg) > 0)
			Thrust = (Mass * PMI.data[1] * (DesiredAzimuth - hdg)) / (Size);
		if((DesiredAzimuth - hdg) < 0)
			Thrust = (Mass * PMI.data[1] * -(DesiredAzimuth - hdg)) / (Size);
		
		sb     = (DesiredAzimuth - hdg) / fabs(DesiredAzimuth - hdg);		
		Level = min((Thrust/MaxThrust), 1);
		Level = max(Level, -1);
		if(fabs(angvel.y) > 0.05)
			Level = 0;
		if(fabs(DesiredAzimuth - hdg) < 0.8)
		{
			Level = 0;
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			LastEventTime = simt;
			ProgState++;
		}
		OurVessel->SetAttitudeRotLevel(1, -(Level * sb));
		break;

	case 15:
		//
		//	Wait 2 seconds for yaw rate to null out
		//

		if(simt >= (LastEventTime + 2))
		{
			LastEventTime = simt;
			OurVessel->DeactivateNavmode(NAVMODE_KILLROT);
			ProgState++;
		}
		break;

	case 16:
		//
		//	Check if our vertical velocity is at the pitchover level
		//
		
		if(horizonvel.y > 15.5)
		{
			ProgState++;
		}
		break;

	case 17:
		//
		//	Pitchover. Here we aim for a pitch of -52 degrees 
		//

		npitch = -52;
		
		Thrust = (Mass * PMI.data[0] * (npitch - pitch)) / (Size);
		Level = min((Thrust/MaxThrust), 1);
		Level = max(Level, -1);
		if(fabs(angvel.x) > 0.15)
			Level = 0;
		if(fabs(pitch) > 52)
		{
			Level = 0;			
			LastEventTime = simt;
			ProgState++;
		}
		OurVessel->SetAttitudeRotLevel(0, Level);		
		break;

	case 18:
		//
		//	Proceed onto pitch hold
		//

		ProgState++;

		break;

	case 19:
		//
		//	Let's hold -52 pitch until our vertical velocity reaches 80 m/s
		//

		npitch = -52;

		if (npitch != pitch)
		{
			Thrust = (Mass * PMI.data[0] * (npitch - pitch)) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			Level = max(Level, -1);
			OurVessel->SetAttitudeRotLevel(0, Level);	
		}

		if(horizonvel.y > 80)
			ProgState++;
		break;

	case 20:
		//
		//	Here we adjust our pitch to maintain the correct acceleration. Both downrange and
		//	radial.
		//

		remainbt   = burntime - (simt - BurnStartTime);				//		Remaining burntime
		hveltogain = DesiredDeltaVx - fabs(horizonvel.x);			//		Horizontal vel to be gained
		hcalcacc   = hveltogain / remainbt;							//		Calculated horizontal acceleration

		remalt   = insertionalt - alt;								//		Remaining altitude
		vvelcalc = (remalt / (remainbt - 55)) + DesiredDeltaVy;		//		Calculated vertical velocity
		vveldiff = vvelcalc - horizonvel.y;							//		Vertical velocity difference
		vcalcacc = vveldiff / remainbt;								//		Calculated vertical acceleration
		
		cmoong	 = (moong * ts) * ((fabs(vcalcacc) / vcalcacc) * -1);

		newaccangle = atan((vcalcacc + cmoong) / hcalcacc) * DEG;
		npitch      = pitch + newaccangle;		
		
		if(pitch != npitch)
		{
			Thrust = (Mass * PMI.data[0] * (npitch - pitch)) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			Level = max(Level, -1);
			OurVessel->SetAttitudeRotLevel(0, Level);		
		}

		if(npitch < - 89.5)
			ProgState++;

		break;

	case 21:
		//
		//	Here we maintain a horizontal attitude until the end of the burn. 
		//

		remainbt = burntime - (simt - BurnStartTime);				//		Remaining burntime
		npitch = -89.5;												//		Not -90 tp avoid errors


		if(pitch != npitch)
		{
			Thrust = (Mass * PMI.data[0] * (npitch - pitch)) / (Size);
			Level = min((Thrust/MaxThrust), 1);
			Level = max(Level, -1);
			OurVessel->SetAttitudeRotLevel(0, Level);		
		}
		
		//
		//	If we have reached the insertion speed, cutoff the engine
		//

		if(fabs(horizonvel.x) >= (DesiredDeltaVx - 0.5))
		{
			OurVessel->SetThrusterGroupLevel(THGROUP_HOVER, 0);
			OurVessel->SetAttitudeRotLevel(0, 0);
			OurVessel->ActivateNavmode(NAVMODE_KILLROT);
			LastEventTime = simt;
			
			ProgState++;
		}
		break;
		
	case 22:
		//
		//	Wait 2 seconds for any maneuver rate to null out
		//

		if(simt > (LastEventTime + 2))
			ProgState++;

		break;			

	case 23:
		//
		//	First trim any residual vertical velocity, using linear RCS thrusters
		//

		if(fabs(horizonvel.y) > DesiredDeltaVy)
			thdir = 1;							//		If we've passed, let's slow down a bit

		if(fabs(horizonvel.y) < DesiredDeltaVy)		
			thdir = -1;							//		If we're short, let's accelerate a bit

		OurVessel->SetAttitudeLinLevel(2, (0.9 * thdir));

		residualV = fabs(horizonvel.y) - DesiredDeltaVy;

		if(fabs(residualV) < 0.1)
		{
			OurVessel->SetAttitudeLinLevel(2,0);			
			ProgState++;
		}	
		break;

	case 24:
		//
		//	Then trim any residual horizontal speed using linear RCS thrusters
		//

		if(fabs(horizonvel.x) > DesiredDeltaVx)
			thdir = -1;							//		If we've passed, let's brake a bit

		if(fabs(horizonvel.x) < DesiredDeltaVx)		
			thdir = 1;							//		If we're short, let's accelerate a bit

		OurVessel->SetAttitudeLinLevel(1, (0.5 * thdir));

		residualV = fabs(horizonvel.x) - DesiredDeltaVx;
	
		if(fabs(residualV) < 0.1)
		{
			OurVessel->SetAttitudeLinLevel(1,0);
			OurVessel->DeactivateNavmode(NAVMODE_KILLROT);
			
			SetVerbNounAndFlash(37,0);
			AwaitProgram();
			ProgState++;
		}		
		break;
	}	
	//sprintf(oapiDebugString(), "newwaccangle %f npitch %f vvelcalc %f hveltogain %f vcalcacc %f hcalcacc %f remainbt %f", newaccangle, npitch, vvelcalc, hveltogain, (vcalcacc + cmoong), hcalcacc, remainbt);		
}
