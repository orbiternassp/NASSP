/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn
  Saturn 1b autopilot code

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
  *	Revision 1.11  2007/06/10 20:25:12  tschachim
  *	Fixed/clarified that center engines aren't gimbaled.
  *	
  *	Revision 1.10  2007/06/06 15:02:16  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.9  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.8  2006/08/01 18:21:51  tschachim
  *	Some code cleanup.
  *	
  *	Revision 1.7  2006/07/27 23:24:11  tschachim
  *	The Saturn 1b now has the Saturn V IGM autopilot.
  *	
  *	Revision 1.6  2006/04/17 19:12:27  movieman523
  *	Removed some unused switches.
  *	
  *	Revision 1.5  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.4  2005/07/30 02:05:47  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.3  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.2  2005/03/24 01:44:05  chode99
  *	Made changes required by correcting thruster positions. Allow for "heads-down" launch.
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"

#include "saturn1b.h"

void Saturn1b::AttitudeLaunch1()
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
	if (autopilot) {
		tempR = AtempR ;
		tempP = AtempP ;
		tempY = AtempY ;
	} else {
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
		rollcorrect = (1/(fabs(tempR)*0.275))*((fabs(tempR)*0.275)-fabs(ang_vel.z));
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

	// Only the four outer engines are gimbaled
	SetThrusterDir(th_main[0],pitchvectorm+yawvectorm+rollvectorl+_V( 0,0,1));
	SetThrusterDir(th_main[1],pitchvectorm+yawvectorm+_V( 0,0,1));
	SetThrusterDir(th_main[2],pitchvectorm+yawvectorm-rollvectorl+_V( 0,0,1));
	SetThrusterDir(th_main[3],pitchvectorm+yawvectorm+_V( 0,0,1));

	// sprintf (oapiDebugString(), "pitch vector: %f, roll vel: %f", tempR, ang_vel.z);
}

void Saturn1b::AttitudeLaunch4()
{
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
	tempP = GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
	tempY = GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
	tempR = GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
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
	SetThrusterDir(th_main[0],pitchvectorm+yawvectorm+_V( 0,0,1));//4
	// sprintf (oapiDebugString(), "roll input: %f, roll vel: %f", tempR, ang_vel.z);

}

void Saturn1b::AutoPilot(double autoT)
{
	const double GRAVITY=6.67259e-11;
	static int first_time=1;
	static int t = 0;
	static int out_level=0;
	double level=0.;
	double altitude;
	double pitch;
	double pitch_c;
	double heading;
	double bank;
	VECTOR3 rhoriz;


	double TO_HDG = agc.GetDesiredAzimuth();
	char current_key=0;

	AltitudePREV = altitude = GetAltitude();
	VESSELSTATUS vsp;
	GetStatus(vsp);
	double totalRot=0;
	totalRot=vsp.vrot.x+vsp.vrot.y+vsp.vrot.z;

	if (fabs(totalRot) >= 0.0025){
		StopRot = true;
	}

	//
	// Shut down the engines when we reach the desired
	// orbit.
	//
	// This vector rotation will be used to tell if heads up (rhoriz.z<0) or heads down.

	HorizonRot(_V(1,0,0),rhoriz);

	if (altitude >= 120000 && CheckForLaunchShutdown())
		return;

 // navigation
	pitch = GetPitch();
	pitch = pitch*180./PI;
	//sprintf(oapiDebugString(), "Autopilot %f", altitude);
 // guidance
	pitch_c = GetCPitch(autoT);
 // control

	if ((altitude < 250)  && (altitude > 150)) {
		//sprintf(oapiDebugString(), "Autopilot initial Pitch DEG %d", (int)fabs(pitch));
		if (pitch >86) {
			AtempP = 0.4;
			if (rhoriz.z>0) AtempP = -0.4;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		else {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
	}
	else if (altitude < 1200  && altitude > 250)	{
		bank = GetBank();
		bank = bank*180. / PI;
		if(bank > 90) bank = bank - 180.;
		else if(bank < -90) bank = bank + 180.;

//		sprintf(oapiDebugString(), "bank =  %d", (int)bank);

		if (fabs(bank + (90. - TO_HDG)) < 15  && StopRot){
				AtempP = 0.0;
				AtempR = 0.0;
				AtempY = 0.0;
		}
		if (fabs(bank + (90. - TO_HDG)) < 0.5){
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}

		//sprintf(oapiDebugString(), "Autopilot Bank Mode TotalROT %f", totalRot);

		if (bank > -(90. - TO_HDG) + 0.5){
			AtempR = -fabs((90. - TO_HDG) - bank);
			//if (AtempR < -1) AtempR = -1;
			if (AtempR < -0.5) AtempR = -0.5;
			AtempP = 0.0;
			AtempY = 0.0;
		}
		else if (bank < -(90. - TO_HDG) - 0.5){
			AtempR = fabs((90. - TO_HDG) - bank);
			//if (AtempR > 1) AtempR = 1;
			if (AtempR > 0.5) AtempR = 0.5;
			AtempP = 0.0;
			AtempY = 0.0;
		}
		else {
			AtempR = 0.0;
		}
	}
	else if (altitude < 2800  && altitude > 1200){

		oapiGetHeading(GetHandle(),&heading);
		heading = heading*180./PI;
		//sprintf(oapiDebugString(), "Autopilot Heading Mode DEG %f", heading);
		bank = GetBank();
		bank = bank*180./PI;
		if(bank > 90) bank = bank - 180.;
		else if(bank < -90) bank = bank + 180.;

		if (fabs(heading - TO_HDG) < 10 && StopRot) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		if (fabs(heading-TO_HDG) < 0.5) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		else if (heading > (TO_HDG + 0.5) && TO_HDG < 90) {
			AtempP = heading - TO_HDG ;
//			if (AtempP > 1.0) AtempP = 1.0;
			if (AtempP > 0.15) AtempP = 0.15;
			AtempR = -0.2;
			if(rhoriz.z > 0) 
			{
//				AtempR = -AtempR;
				AtempP = -AtempP;
			}
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, -1);
		}
		else if (heading < (TO_HDG - 0.5) && TO_HDG < 90) {
			AtempP = heading  - TO_HDG;
//			if (AtempP < -1.0) AtempP = -1.0;
			if (AtempP < -0.15) AtempP = -0.15;
			AtempR = 0.2;
			if(rhoriz.z > 0) 
			{
//				AtempR = -AtempR;
				AtempP = -AtempP;
			}
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, 1);
		}
		else if (heading > (TO_HDG + 0.5) && TO_HDG > 90) {
			AtempP = -(heading - TO_HDG) ;
//			if (AtempP < -1.0) AtempP = -1.0;
			if (AtempP < -0.15) AtempP = -0.15;
			AtempR = -0.2;
			if(rhoriz.z > 0) 
			{
//				AtempR = -AtempR;
				AtempP = -AtempP;
			}
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, -1);
		}
		else if (heading < (TO_HDG - 0.5) && TO_HDG > 90) {
			AtempP = -(heading  - TO_HDG);
//			if (AtempP > 1.0) AtempP = 1.0;
			if (AtempP > 0.15) AtempP = 0.15;
			AtempR = 0.2;
			if(rhoriz.z > 0) 
			{
//				AtempR = -AtempR;
				AtempP = -AtempP;
			}
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, 1);
		}
		else {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
	}
	else if (altitude < 4500  && altitude > 2800) {
		bank = GetBank();
		bank = bank * 180. / PI;
		if(bank > 90) 
			bank = bank - 180.;
		else if(bank < -90) 
			bank = bank + 180.;
		oapiGetHeading(GetHandle(),&heading);
		heading = heading*180./PI;

		if (fabs(bank) < 0.5) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;

		}
		else if (bank < 0 && fabs(vsp.vrot.z) < 0.11) {
			AtempR = 0.2; // 1.0;
		}
		else if (bank > 0 && fabs(vsp.vrot.z) < 0.11) {
			AtempR = -0.2; // -1.0;
		}
		else {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		if (fabs(bank) < 10 && fabs(vsp.vrot.z) > 0.1) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
			StopRot=false;
		}
		// zero angle-of-attack...
		// AtempP = (GetAOA() * DEG - 0.3) / 5.0;
		// if(AtempP < -0.2) AtempP = -0.2;
		// if(AtempP >  0.2) AtempP = 0.2;

		pitch_c = GetCPitch(autoT);
		level = pitch_c - GetPitch() * DEG;
		if (level > 0 && fabs(vsp.vrot.z) < 0.09){
			AtempP = -(fabs(level) / 10.);
			if (AtempP < -0.1) AtempP = -0.1;
			if (rhoriz.z>0) AtempP = -AtempP;
		}
		else if (level < 0 && fabs(vsp.vrot.z) < 0.09) {
			AtempP = (fabs(level) / 10.);
			if (AtempP > 0.1) AtempP = 0.1;
			if (rhoriz.z > 0) AtempP = -AtempP;
		}
		else {
			AtempP = 0.0;						
		}
	}
	else if (altitude > 4500) {

		// Damp roll motion
		bank = GetBank();
		bank = bank*180./PI;
		if(bank > 90) bank = bank - 180;
		else if(bank < -90) bank = bank + 180;

		AtempR=-bank/20.0;
		if(fabs(bank) < 0.3) AtempR=0.0;

		// navigation
		pitch = GetPitch();
		pitch = pitch*180./PI;

		if (IGMEnabled) {
			VECTOR3 target;
			double pit, yaw;
			OBJHANDLE hbody=GetGravityRef();
			double bradius=oapiGetSize(hbody);
			double bmass=oapiGetMass(hbody);
			double mu=GRAVITY*bmass;
			double altco=agc.GetDesiredApogee()*1000.0;
			double velo=sqrt(mu/(bradius+altco));
			target.x=velo-0.234;
			target.y=0.0;
			target.z=altco;
			LinearGuidance(target, pit, yaw);
			AtempP=(pit*DEG-pitch)/30.0;
		}
		else {
			 // guidance
			pitch_c = GetCPitch(autoT);

			 // control
			double SatApo;
			GetApDist(SatApo);

			if ((SatApo >= ((agc.GetDesiredApogee() *.90) + ERADIUS)*1000) || MissionTime >= IGMStartTime)
				IGMEnabled = true;
		
			level = pitch_c - pitch;

			//sprintf(oapiDebugString(), "Autopilot Pitch Mode%f", elemSaturn1B.a );

			if (fabs(level)<10 && StopRot){	// above atmosphere, soft correction
				AtempP = 0.0;
				AtempR = 0.0;
				AtempY = 0.0;
				StopRot = false;
			}
			if (fabs(level)<0.05){	// above atmosphere, soft correction
				AtempP = 0.0;
				AtempR = 0.0;
				AtempY = 0.0;
			}
			else if (level>0 && fabs(vsp.vrot.z) < 0.09){
				AtempP = -(fabs(level) / 10.);
				if (AtempP < -1.0)AtempP = -1.0;
				if (rhoriz.z>0) AtempP = -AtempP;
			}
			else if (level<0 && fabs(vsp.vrot.z) < 0.09) {
				AtempP = (fabs(level) / 10.);
				if (AtempP > 1.0) AtempP = 1.0;
				if (rhoriz.z>0) AtempP = -AtempP;
			}
			else {
				AtempP = 0.0;
				AtempR = 0.0;
				AtempY = 0.0;
			}
		}
	}
	// sprintf(oapiDebugString(), "Alt %f Pitch %f Roll %f Yaw %f autoT %f", altitude, AtempP, AtempR, AtempY, autoT);

	switch (stage){
		case LAUNCH_STAGE_ONE:
			AttitudeLaunch1();
			break;

		case LAUNCH_STAGE_SIVB:
			AttitudeLaunchSIVB();
			break;
	}
}
