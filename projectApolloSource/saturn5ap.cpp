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
  **************************************************************************/

#include <stdio.h>
#include <math.h>
#include "Orbitersdk.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"

#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"

#define N	16

const double met[N]    = { 0,  58, 70, 80,  90, 110, 130,   160, 170, 205, 450, 480, 490, 500, 535, 700};   // MET in sec
const double cpitch[N] = {90,  75, 60, 50,  45,  40,  35,    30,   30, 30,    25,   20, 10 , 5, -2,0};	// Commanded pitch in °

static double GetCPitch(double t)
{
	int i = 1;

	//
	// Make sure we don't run off the end.
	//

	if (t>met[N-1]) return cpitch[N-1];

	//
	// Find the first MET that's greater than our current time.
	//

	while (met[i]<t) i++;

	//
	// And calculate pitch as appropriate between those two times.
	//

	return cpitch[i-1]+(cpitch[i]-cpitch[i-1])/(met[i]-met[i-1])*(t-met[i-1]);
}

void SaturnV::AttitudeLaunch2()
{
	TRACESETUP("AttitudeLaunch2");

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
	}else{
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
	LimitSetThrusterDir(th_main[4],pitchvectorm+yawvectorm+_V( 0,0,1));//5
	// sprintf (oapiDebugString(), "roll input: %f, roll vel: %f", tempR, ang_vel.z);

}

void SaturnV::AttitudeLaunch4()
{
	TRACESETUP("AttitudeLaunch4");

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
	if(LPswitch6){
		tempP = GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);}
	if(LPswitch7){
		tempY = GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);}
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
	LimitSetThrusterDir(th_main[0],pitchvectorm+yawvectorm+_V( 0,0,1));//4
	// sprintf (oapiDebugString(), "roll input: %f, roll vel: %f", tempR, ang_vel.z);

}

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
	SetThrusterDir(th_main[4],pitchvectorm+yawvectorm+_V( 0,0,1));//5
	// sprintf (oapiDebugString(), "pitch vector: %f, roll vel: %f", tempR, ang_vel.z);
}

void SaturnV::AutoPilot(double autoT)
{
	static int t = 0;
	double level=0.;
	double altitude;
	double pitch;
	double pitch_c;
	double heading;
	double bank;

	double TO_HDG = agc.GetDesiredAzimuth();

	AltitudePREV = altitude = GetAltitude();
	VESSELSTATUS vsp;
	GetStatus(vsp);
	double totalRot=0;
	totalRot=vsp.vrot.x+vsp.vrot.y+vsp.vrot.z;

	if (fabs(totalRot) >= 0.025){
		StopRot=true;
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

	//sprintf(oapiDebugString(), "Autopilot %f", altitude);
 // guidance
	pitch_c = GetCPitch(altitude);
 // control

	if ((altitude < 250)  && (altitude > 150)) {
	//	sprintf(oapiDebugString(), "Autopilot initial Pitch DEG %d", (int)fabs(pitch));
		if (pitch >84){
			AtempP = 2.8;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		else{
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
	}
	else if (altitude < 750  && altitude > 250)	{
		bank = GetBank();
//		oapiGetFocusBank(&bank);
		bank = bank*180./PI;
		if (fabs(bank+(90-TO_HDG)) <15  && StopRot){
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		if (fabs(bank+(90-TO_HDG)) <0.5){
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}

		//sprintf(oapiDebugString(), "Autopilot Bank Mode TotalROT %f", totalRot);

		if (bank > -(90-TO_HDG)+0.5){
			AtempR = -fabs((90-TO_HDG)-bank);
			if (AtempR < -1)
				AtempR = -1;
			AtempP = 0.0;
			AtempY = 0.0;
		}
		else if (bank < -(90-TO_HDG)-0.5){
			AtempR = fabs((90-TO_HDG)-bank);
			if (AtempR > 1)
				AtempR = 1;
			AtempP = 0.0;
			AtempY = 0.0;
		}
		else {
			AtempR = 0.0;
		}

	}
	else if (altitude < 2800  && altitude > 750){
		oapiGetHeading(GetHandle(),&heading);
		heading = heading*180./PI;
		bank = GetBank();
//		oapiGetFocusBank(&bank);
		bank = bank*180./PI;
		int etat;
		if (fabs(heading-TO_HDG) <10 && StopRot) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
			etat=1;
		}
		if (fabs(heading-TO_HDG) <0.1) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
			etat=2;

		}
		else if (heading > (TO_HDG +0.15) && TO_HDG <90) {
			AtempP = heading - TO_HDG ;
			if (AtempP > 4.0) AtempP = 4.0;
			AtempR = 0.0;
			AtempY = AtempP/35;
			//SetAttitudeRotLevel(0, -1);
			etat=3;

		}
		else if (heading < (TO_HDG -0.15) && TO_HDG <90) {
			AtempP = (heading  - TO_HDG);
			if (AtempP<-4.0) AtempP = -4.0;
			AtempR = 0.0;
			AtempY = AtempP/35;
			//SetAttitudeRotLevel(0, 1);
			etat=4;
		}
		else if (heading > (TO_HDG +0.15) && TO_HDG >90) {
			AtempP = -(heading - TO_HDG) ;
			if (AtempP<-2.0) AtempP = -2.0;
			AtempR = 0.0;
			AtempY = -AtempP/35;
			//SetAttitudeRotLevel(0, -1);
			etat=5;

		}
		else if (heading < (TO_HDG -0.15) && TO_HDG >90) {
			AtempP = -(heading  - TO_HDG);
			if (AtempP > 2.0) AtempP = 2.0;
			AtempR = 0.0;
			AtempY = -AtempP/35;
			//SetAttitudeRotLevel(0, 1);
		etat=6;
		}
		else {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
			etat=7;
		}
		//sprintf(oapiDebugString(), "Autopilot Heading Mode DEG %f %f %d", heading, AtempP,etat);
	}
	else if (altitude < 4500  && altitude > 2800){
//		oapiGetFocusBank(&bank);
		bank = GetBank();
		bank = bank*180./PI;
		oapiGetHeading(GetHandle(),&heading);
		heading = heading*180./PI;

		if (fabs(bank) < 0.5) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;

		}
		else if (bank < 0 && fabs(vsp.vrot.z) < 0.11) {
			AtempR = 1.0;
			//SetAttitudeRotLevel(2,-1 );//bank/30
		}
		else if (bank > 0 && fabs(vsp.vrot.z) < 0.11) {
			AtempR = -1.0;
			//SetAttitudeRotLevel(2, 1);
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

	}else if (altitude >= 4500)	{

	// navigation
//	oapiGetFocusPitch(&pitch);
	pitch = GetPitch();
	pitch=pitch*180./PI;
	altitude = GetAltitude();
//	oapiGetFocusAltitude(&altitude);

 // guidance
	pitch_c=GetCPitch(autoT);

 // control
	double SatApo;
	GetApDist(SatApo);

	if (SatApo >= ((agc.GetDesiredApogee() *.80) + ERADIUS)*1000) {
		double TimeW=0;
		TimeW = oapiGetTimeAcceleration ();
		if (TimeW>1){
		//TimeW = 1;
		//oapiSetTimeAcceleration (1);
		}
		pitch_c = SetPitchApo();
		level = pitch_c - pitch;
	}
	else if(MissionTime < 150) {
		level = pitch_c - pitch;
	}
	else{
		if (stage >= LAUNCH_STAGE_TWO_ISTG_JET) {
			pitch_c = 35;
		}
		else{
			pitch_c = GetCPitch(autoT);
		}
		level = pitch_c - pitch;
	}
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
	}else if (level>0 && fabs(vsp.vrot.z) < 0.09) {
		AtempP = -(fabs(level)/10);
		if (AtempP < -1.0) AtempP = -1.0;
	}
	else if (level<0 && fabs(vsp.vrot.z) < 0.09) {
		AtempP = (fabs(level)/10);
		if (AtempP > 1.0) AtempP = 1.0;
	}
	else{
		AtempP = 0.0;
		AtempR = 0.0;
		AtempY = 0.0;
	}
	}

	if (CMCswitch){
		switch (stage){
		case LAUNCH_STAGE_ONE:
			pitch = GetPitch();
//			oapiGetFocusPitch(&pitch);
			pitch=pitch*180./PI;

			pitch_c=GetCPitch(autoT);

			AtempP = (pitch - pitch_c);
			if (AtempP > 1.0)
				AtempP = 1.0;
			if (AtempP < (-1.0))
				AtempP = (-1.0);

			AttitudeLaunch1();
		break;
			case LAUNCH_STAGE_TWO:
			AttitudeLaunch2();
		break;
			case LAUNCH_STAGE_TWO_ISTG_JET:
			AttitudeLaunch2();
		break;
			case LAUNCH_STAGE_TWO_TWR_JET:
			AttitudeLaunch2();
		break;
			case LAUNCH_STAGE_SIVB:
			AttitudeLaunchSIVB();
		break;
		}
	}

}
