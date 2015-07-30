/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn
  Saturn 5 autopilot code

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
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.22  2008/05/24 17:21:57  tschachim
  *	Removed useless "if".
  *	
  *	Revision 1.21  2008/04/11 11:50:00  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.20  2007/06/10 20:25:12  tschachim
  *	Fixed/clarified that center engines aren't gimbaled.
  *	
  *	Revision 1.19  2007/06/06 15:02:17  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.18  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.17  2006/07/27 23:24:11  tschachim
  *	The Saturn 1b now has the Saturn V IGM autopilot.
  *	
  *	Revision 1.16  2006/02/27 00:57:48  dseagrav
  *	Added SPS thrust-vector control. Changes 20060225-20060226.
  *	
  *	Revision 1.15  2006/02/21 11:56:30  tschachim
  *	Fixes to make code build with MS C++ 2005
  *	
  *	Revision 1.14  2006/02/06 20:57:41  lazyd
  *	Improved roll control for SII and SIVB
  *	
  *	Revision 1.13  2006/02/05 20:55:58  lazyd
  *	Added RCS roll control to SIVB
  *	
  *	Revision 1.12  2006/02/04 20:58:57  lazyd
  *	Accounted for Ullage rockets for SIVB shutdown
  *	
  *	Revision 1.11  2006/02/02 21:40:08  lazyd
  *	Changed to a new frame of reference for launch, and fixed the roll program.
  *	
  *	Revision 1.10  2006/01/31 21:31:11  lazyd
  *	Added zero AOA guidance for 45-100 sec MET
  *	
  *	Revision 1.9  2006/01/31 00:52:43  lazyd
  *	Added linear guidance for SII and SIVB
  *	
  *	Revision 1.8  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.7  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.6  2005/07/29 23:05:38  movieman523
  *	Added Inertial Guidance Mode start time to scenario file.
  *	
  *	Revision 1.5  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.4  2005/07/29 20:37:42  movieman523
  *	Fixed the weird pitching after the SII interstage jettison.
  *	
  *	Revision 1.3  2005/03/24 00:14:14  chode99
  *	Fixed a bug in the "heads-down" autopilot.
  *	
  *	Revision 1.2  2005/03/11 18:18:18  chode99
  *	Changed launch autopilot to allow for "heads-down" launch.
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"

void SaturnV::AttitudeLaunch2()
{
//	TRACESETUP("AttitudeLaunch2");

	VECTOR3 ang_vel;
	GetAngularVel(ang_vel);// gets current angular velocity for stabilizer and rate control
// variables to store each component deflection vector
	VECTOR3 rollvectorl={0.0,0.0,0.0};
	VECTOR3 rollvectorr={0.0,0.0,0.0};
	VECTOR3 pitchvector={0.0,0.0,0.0};
	VECTOR3 yawvector={0.0,0.0,0.0};
	VECTOR3 yawvectorm={0.0,0.0,0.0};
	VECTOR3 pitchvectorm={0.0,0.0,0.0};
//************************************************************
// variables to store Manual control levels for each axis
	double tempP = 0.0;
	double tempY = 0.0;
	double tempR = 0.0;
//************************************************************
// Variables to store correction factors for rate control
	double rollcorrect = 0.0;
	double yawcorrect= 0.0;
	double pitchcorrect = 0.0;
//************************************************************
// gets manual control levels in each axis, this code copied directly from Rob Conley's Mercury Atlas
	if(AutopilotActive()){
		tempP =AtempP ;tempR =AtempR ;tempY =AtempY ;
	}
	else{
		tempP = GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempY = GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempR = GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
	}
//*************************************************************
//Creates correction factors for rate control in each axis as a function of input level
// and current angular velocity. Varies from 1 to 0 as angular velocity approaches command level
// multiplied by maximum rate desired
	if(tempR != 0.0)	{
		rollcorrect = (1/(fabs(tempR)*0.35))*((fabs(tempR)*0.35)-fabs(ang_vel.z));
			}
	if(tempP != 0.0)	{
		pitchcorrect = (1/(fabs(tempP)*0.175))*((fabs(tempP)*0.175)-fabs(ang_vel.x));
		if((tempP > 0 && ang_vel.x > 0) || (tempP < 0 && ang_vel.x < 0))	{
						pitchcorrect = 1;
					}
	}
	if(tempY != 0.0)	{
	yawcorrect = (1/(fabs(tempY)*0.175))*((fabs(tempY)*0.175)-fabs(ang_vel.y));
	if((tempY > 0 && ang_vel.y < 0) || (tempY < 0 && ang_vel.y > 0))	{
						yawcorrect = 1;
					}
	}
//*************************************************************
// Create deflection vectors in each axis
	pitchvector = _V(0.0,0.05*tempP*pitchcorrect,0.0);
	pitchvectorm = _V(0.0,0.09*tempP*pitchcorrect,0.0);
	yawvector = _V(0.05*tempY*yawcorrect,0.0,0.0);
	yawvectorm = _V(0.09*tempY*yawcorrect,0.0,0.0);
	rollvectorl = _V(0.0,0.090*tempR*rollcorrect,0.0);
	rollvectorr = _V(0.0,-0.09*tempR*rollcorrect,0.0);

//*************************************************************
// create opposite vectors for "gyro stabilization" if command levels are 0
	if(tempP==0.0) {

		pitchvectorm=_V(0.0,0.95*ang_vel.x*2,0.0);
	}
	if(tempY==0.0) {

		yawvectorm=_V(-0.95*ang_vel.y*2,0.0,0.0);
	}
	if(tempR==0.0) {
		rollvectorl = _V(0.0,0.99*ang_vel.z*2,0.0);

	}
//**************************************************************
// Sets thrust vectors by simply adding up all the axis deflection vectors and the
// "neutral" default vector	
	LimitSetThrusterDir(th_main[0],pitchvectorm+yawvectorm-rollvectorl+_V( 0,0,1));//4
	LimitSetThrusterDir(th_main[1],pitchvectorm+yawvectorm+rollvectorl+_V( 0,0,1));//2
	LimitSetThrusterDir(th_main[2],pitchvectorm+yawvectorm-rollvectorl+_V( 0,0,1));//1
	LimitSetThrusterDir(th_main[3],pitchvectorm+yawvectorm+rollvectorl+_V( 0,0,1));//3
	// Center engine isn't gimbaled
	// LimitSetThrusterDir(th_main[4],pitchvectorm+yawvectorm+_V( 0,0,1));//5	

	// sprintf (oapiDebugString(), "roll input: %f, roll vel: %f", tempR, ang_vel.z);

}

// DS20060226 AttitudeLaunch4 deleted.

void SaturnV::AttitudeLaunch1()
{
	//Original code function by Richard Craig From MErcury Sample by Rob CONLEY
	// Modification for NASSP specific needs by JL Rocca-Serra
	VECTOR3 ang_vel;
	GetAngularVel(ang_vel);// gets current angular velocity for stabilizer and rate control
// variables to store each component deflection vector
	VECTOR3 rollvectorl={0.0,0.0,0.0};
	VECTOR3 rollvectorr={0.0,0.0,0.0};
	VECTOR3 pitchvector={0.0,0.0,0.0};
	VECTOR3 yawvector={0.0,0.0,0.0};
	VECTOR3 yawvectorm={0.0,0.0,0.0};
	VECTOR3 pitchvectorm={0.0,0.0,0.0};
//************************************************************
// variables to store Manual control levels for each axis
	double tempP = 0.0;
	double tempY = 0.0;
	double tempR = 0.0;
//************************************************************
// Variables to store correction factors for rate control
	double rollcorrect = 0.0;
	double yawcorrect= 0.0;
	double pitchcorrect = 0.0;
//************************************************************
// gets manual control levels in each axis, this code copied directly from Rob Conley's Mercury Atlas
	if(AutopilotActive()){
		tempR = AtempR ;
		tempP =AtempP ;
		tempY =AtempY ;
	}else{
		tempP = GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempY = GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempR = GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
	}
	//sprintf (oapiDebugString(), "roll input: %f, roll vel: %f,pitch input: %f, pitch vel: %f", tempR, ang_vel.z,tempP, ang_vel.x);

//*************************************************************
//Creates correction factors for rate control in each axis as a function of input level
// and current angular velocity. Varies from 1 to 0 as angular velocity approaches command level
// multiplied by maximum rate desired
	if(tempR != 0.0)	{
		rollcorrect = (1/(fabs(tempR)*0.175))*((fabs(tempR)*0.175)-fabs(ang_vel.z));
		if((tempR > 0 && ang_vel.z > 0) || (tempR < 0 && ang_vel.z < 0))	{
			rollcorrect = 1;
		}
	}
	if(tempP != 0.0)	{
		pitchcorrect = (1/(fabs(tempP)*0.175))*((fabs(tempP)*0.175)-fabs(ang_vel.x));
		if((tempP > 0 && ang_vel.x > 0) || (tempP < 0 && ang_vel.x < 0))	{
			pitchcorrect = 1;
		}
	}
	if(tempY != 0.0)	{
		yawcorrect = (1/(fabs(tempY)*0.175))*((fabs(tempY)*0.175)-fabs(ang_vel.y));
		if((tempY > 0 && ang_vel.y < 0) || (tempY < 0 && ang_vel.y > 0))	{
			yawcorrect = 1;
		}
	}
//*************************************************************
// Create deflection vectors in each axis
	pitchvector = _V(0.0,0.05*tempP*pitchcorrect,0.0);
	pitchvectorm = _V(0.0,0.09*tempP*pitchcorrect,0.0);
	yawvector = _V(0.05*tempY*yawcorrect,0.0,0.0);
	yawvectorm = _V(0.09*tempY*yawcorrect,0.0,0.0);
	rollvectorl = _V(0.0,0.60*tempR*rollcorrect,0.0);
	rollvectorr = _V(0.0,-0.60*tempR*rollcorrect,0.0);

//*************************************************************
// create opposite vectors for "gyro stabilization" if command levels are 0
	if(tempP==0.0) {
		pitchvectorm=_V(0.0,0.8*ang_vel.x*3,0.0);
	}
	if(tempY==0.0) {
		yawvectorm=_V(-0.8*ang_vel.y*3,0.0,0.0);
	}
	if(tempR==0.0) {
		rollvectorl = _V(0.0,0.95*ang_vel.z*3,0.0);
	}
//**************************************************************
// Sets thrust vectors by simply adding up all the axis deflection vectors and the
// "neutral" default vector
	SetThrusterDir(th_main[0],pitchvectorm+yawvectorm+rollvectorl+_V( 0,0,1));//4
	SetThrusterDir(th_main[1],pitchvectorm+yawvectorm-rollvectorl+_V( 0,0,1));//2
	SetThrusterDir(th_main[2],pitchvectorm+yawvectorm-rollvectorl+_V( 0,0,1));//1
	SetThrusterDir(th_main[3],pitchvectorm+yawvectorm+rollvectorl+_V( 0,0,1));//3
	// Center engine isn't gimbaled
	// SetThrusterDir(th_main[4],pitchvectorm+yawvectorm+_V( 0,0,1));//5

	// sprintf (oapiDebugString(), "pitch vector: %f, roll vel: %f", tempR, ang_vel.z);
}

void SaturnV::AutoPilot(double autoT)
{
	const double GRAVITY=6.67259e-11;
	static int t = 0;
	double level=0.;
	double altitude;
	double pitch;
	double pitch_c;
	double heading;
	double bank;
	VECTOR3 rhoriz, up, north, east, ygl, zgl, zerogl;

	double TO_HDG = agc.GetDesiredAzimuth();

	// set up our reference frame
	Local2Global(_V(0.0, 0.0, 0.0), zerogl);
	Local2Global(_V(0.0, 1.0, 0.0), ygl);
	Local2Global(_V(0.0, 0.0, 1.0), zgl);
	ygl=ygl-zerogl;
	zgl=zgl-zerogl;

	AltitudePREV = altitude = GetAltitude();
	VESSELSTATUS vsp;
	GetStatus(vsp);

	double totalRot=0;
	totalRot=vsp.vrot.x+vsp.vrot.y+vsp.vrot.z;

	if (fabs(totalRot) >= 0.025){
//		StopRot=true;
	}

	//
	// Shut down the engines when we reach the desired
	// orbit.
	//

	if (altitude >= 120000 && CheckForLaunchShutdown())
		return;

 // navigation
	pitch = GetPitch();
	pitch = pitch*180./PI;
	altitude = GetAltitude();
	oapiGetHeading(GetHandle(),&heading);
	heading = heading*180./PI;

	// This vector rotation will be used to tell if heads up (rhoriz.z<0) or heads down.

	HorizonRot(_V(1,0,0),rhoriz);


 // guidance
	pitch_c = GetCPitch(altitude);
 // control
	if (altitude >= 4500)	{
		bank = GetBank();
		bank = bank*180./PI;
		if(bank > 90) bank = bank - 180;
		else if(bank < -90) bank = bank + 180;

		AtempR=-bank/20.0;
		if(fabs(bank) < 0.3) AtempR=0.0;



	// navigation
		pitch = GetPitch();
		pitch=pitch*180./PI;
		altitude = GetAltitude();

	 // control
		if (IGMEnabled)
		{
			pitch_c = SetPitchApo();
		}
		else
		{
			double SatApo;
			GetApDist(SatApo);

			pitch_c = GetCPitch(autoT);
			if (MissionTime >= IGMStartTime) {
				IGMEnabled = true;
//				char fname[8];
//				sprintf(fname,"asclog.txt");
//				if(pit > 22.0*DEG) pit=22.0*DEG;
//				outstr=fopen(fname,"w");
			}
		}
//		sprintf(oapiDebugString(), "t=%.1f IGM=%d pitch=%.3f cpitch=%.3f tpitch=%.3f", 
//			autoT, IGMEnabled, pitch, pitch_c, GetCPitch(autoT));

		level = pitch_c - pitch;
		AtempP=level/30.0;
		AtempY=(-asin(zgl*normal)*DEG)/50.0;

		if(IGMEnabled) {
			VECTOR3 target;
			double pit, yaw;
			OBJHANDLE hbody=GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			double mu=GRAVITY*bmass;
			double altco=agc.GetDesiredApogee()*1000.0;
			double velo=sqrt(mu/(bradius+altco));
			if(stage == LAUNCH_STAGE_SIVB) {
				target.x=velo-0.234;
				target.y=0.0;
				target.z=altco;
				LinearGuidance(target, pit, yaw);
				AtempP=(pit*DEG-pitch)/30.0;
//					AtempR=-bank/20.0;
			} else {
				target.x=velo-500.0;
//				target.y=73.0;
				target.y=123.0;
				target.z=altco;
				LinearGuidance(target, pit, yaw);
				if(pit > 26.0*RAD) pit=26.0*RAD;
				AtempP=(pit*DEG-pitch)/30.0;
			}
		}
	}

	double slip, aoa;
	VECTOR3 az;
	OBJHANDLE hbody=GetGravityRef();
	double bradius=oapiGetSize(hbody);

	switch (stage) {
	case LAUNCH_STAGE_ONE:
		GetRelativePos(hbody, up);
		up=Normalize(up);
		agc.EquToRel(PI/2.0, 0.0, bradius, north);
		north=Normalize(north);
		east=Normalize(CrossProduct(north, up));
		north=Normalize(CrossProduct(up, east));
		az=east*sin(TO_HDG*RAD)-north*cos(TO_HDG*RAD);
		if(autoT < 60.0) normal=Normalize(CrossProduct(up, az));

		slip=GetSlipAngle()*DEG;

		if(autoT < 10.) {
			AtempR=0.0;
			AtempY=0.0;
			// cancel out the yaw maneuver...
			AtempY=(-0.4+asin(zgl*normal)*DEG)/20.0;
		}

		if(autoT > 10.0 && autoT < 30.0) {
			// roll program
			AtempR=asin(ygl*normal)*DEG/20.0;
			AtempY=asin(zgl*normal)*DEG/20.0;
		}

		if(autoT > 30.0 && autoT < 45.0) {
			//pitch and adjust for relative wind
			AtempR=asin(ygl*normal)*DEG/20.0;
//				AtempY=(slip+asin(zgl*normal)*DEG)/20.0;
			AtempY=(TO_HDG-(heading+slip))/20.0;
		}
		pitch = GetPitch();
		pitch=pitch*180./PI;

		pitch_c=GetCPitch(autoT);

		AtempP = (pitch - pitch_c);
		if(rhoriz.z>0)AtempP= -AtempP;
		if (AtempP > 1.0)
			AtempP = 1.0;
		if (AtempP < (-1.0))
			AtempP = -1.0;

		// zero angle-of-attack...
		if(autoT > 45.0 && autoT < 115.0) {
			aoa=GetAOA()*DEG;
			pitch_c=pitch+aoa-0.3;
			AtempP=(pitch_c-pitch)/5.0;
			if(AtempP < -0.2) AtempP=-0.2;
			if(AtempP >  0.2) AtempP= 0.2;
//				sprintf(oapiDebugString(), " pitch=%.3f pc=%.3f ap=%.3f", pitch, pitch_c, AtempP);
			if(autoT < 50.0) {
//					AtempY=(slip+asin(zgl*normal)*DEG)/20.0;
				AtempY=(TO_HDG-(heading+slip))/20.0;
				// try this...
				AtempR=asin(ygl*normal)*DEG/20.0;
			} else {
//					AtempY=slip/10.0;
				AtempY=(TO_HDG-(heading+slip))/20.0;
			}
		}
		if (autoT > 115.0) {
			AtempY=0.0;
			if (autoT < 120.0) {
				if (AtempP < -0.1) AtempP = -0.1;
				if (AtempP >  0.1) AtempP =  0.1;
			} else {
				if (AtempP < -0.2) AtempP = -0.2;
				if (AtempP >  0.2) AtempP =  0.2;
			}
			normal=Normalize(CrossProduct(Normalize(vsp.rpos), Normalize(vsp.rvel)));
		}
//			sprintf(oapiDebugString(), "roll=%.3f yaw=%.3f slip=%.3f sum=%.3f hdg+slip=%.3f hdg=%.3f ay=%.3f", 
//			asin(ygl*normal)*DEG, asin(zgl*normal)*DEG, slip, slip+asin(zgl*normal)*DEG, heading+slip, heading, AtempY);
//			sprintf(oapiDebugString(), "autoT %f AtempP %f", autoT, AtempP);

		AttitudeLaunch1();
		break;
	case LAUNCH_STAGE_TWO:
		AttitudeLaunch2();
		break;
	case LAUNCH_STAGE_TWO_ISTG_JET:
		AttitudeLaunch2();
		break;
	case LAUNCH_STAGE_SIVB:
		AttitudeLaunchSIVB();
		break;
	}
}
