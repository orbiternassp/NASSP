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
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"

#include "saturn.h"

#include "saturn1b.h"

#define N_entries	16	// flight plan table size
const double met[N_entries]    = { 0,  58, 75, 80,  90, 110, 130,   160, 170, 205, 450, 480, 490, 500, 535, 700};   // MET in sec
const double cpitch[N_entries] = {90,  82, 65, 55,  48,  45,  35,    35,   35, 35,    25,   25, 25 , 25, 25,25};	// Commanded pitch in °

//#define N	13	// flight plan table size
//const double met[N]    = { 0, 25, 60, 95, 130, 145, 152, 165, 200, 235, 270, 305, 330};   // MET in sec
//const double cpitch[N] = {90, 90, 80, 70,  60,  55,  55,  50,  40,  30,  20,  10,   0};	// Commanded pitch in °

double Saturn1b::GetCPitch(double t)
{
	int i;
	if (t>met[N_entries-1]) return cpitch[N_entries-1];
	i=1;
	while (met[i]<t) i++;
	return cpitch[i-1]+(cpitch[i]-cpitch[i-1])/(met[i]-met[i-1])*(t-met[i-1]);
}

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
	if(CMCswitch && autopilot){
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
	SetThrusterDir(th_main[0],pitchvectorm+yawvectorm+rollvectorl+_V( 0,0,1));//4
	SetThrusterDir(th_main[1],pitchvectorm+yawvectorm-rollvectorl+_V( 0,0,1));//2
	SetThrusterDir(th_main[2],pitchvectorm+yawvectorm+_V( 0,0,1));//1
	SetThrusterDir(th_main[3],pitchvectorm+yawvectorm+_V( 0,0,1));//3
	SetThrusterDir(th_main[4],pitchvectorm+yawvectorm+_V( 0,0,1));//5
	SetThrusterDir(th_main[5],pitchvectorm+yawvectorm+_V( 0,0,1));//5
	SetThrusterDir(th_main[6],pitchvectorm+yawvectorm+_V( 0,0,1));//5
	SetThrusterDir(th_main[7],pitchvectorm+yawvectorm+_V( 0,0,1));//5

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
	SetThrusterDir(th_main[0],pitchvectorm+yawvectorm+_V( 0,0,1));//4
	// sprintf (oapiDebugString(), "roll input: %f, roll vel: %f", tempR, ang_vel.z);

}

void Saturn1b::AutoPilot(double autoT)
{
	static int first_time=1;
	static int t = 0;
	static int out_level=0;
	double level=0.;
	double altitude;
	double pitch;
	double pitch_c;
	double heading;
	double bank;

	double TO_HDG = agc.GetDesiredAzimuth();
	char current_key=0;

	AltitudePREV = altitude = GetAltitude();
	VESSELSTATUS vsp;
	GetStatus(vsp);
	double totalRot=0;
	totalRot=vsp.vrot.x+vsp.vrot.y+vsp.vrot.z;

	if (fabs(totalRot) >= 0.0025){
		StopRot=true;
	}

	//
	// Shut down the engines when we reach the desired
	// orbit.
	//

	if (altitude >= 120000 && CheckForLaunchShutdown())
		return;

 // navigation
	oapiGetFocusPitch(&pitch);
	pitch = pitch*180./PI;
	oapiGetFocusAltitude(&altitude);
	//sprintf(oapiDebugString(), "Autopilot %f", altitude);
 // guidance
	pitch_c = GetCPitch(altitude);
 // control

	if (GetAltitude() < 250  && GetAltitude() > 150 ){
		//sprintf(oapiDebugString(), "Autopilot initial Pitch DEG %d", (int)fabs(pitch));
		if (pitch >88){
			AtempP = 0.8;
			AtempR = 0.0;
			AtempY = 0.0;
		}else{
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
	}else if (GetAltitude() < 1200  && GetAltitude() > 250)	{
		oapiGetFocusBank(&bank);
		bank = bank*180./PI;
		if (fabs(bank+(90-TO_HDG)) <15  && StopRot){
				AtempP = 0.0;
				AtempR = 0.0;
				AtempY = 0.0;
		}
		if (fabs(bank+(90-TO_HDG))<0.5){
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}

		//sprintf(oapiDebugString(), "Autopilot Bank Mode TotalROT %f", totalRot);

		if (bank > -(90-TO_HDG)+0.5){
			AtempR = -fabs((90-TO_HDG)-bank);
			if (AtempR < -1)AtempR = -1;
			AtempP = 0.0;
			AtempY = 0.0;
		}else if (bank < -(90-TO_HDG)-0.5){
			AtempR = fabs((90-TO_HDG)-bank);
			if (AtempR > 1)AtempR = 1;
			AtempP = 0.0;
			AtempY = 0.0;
		}else {
			AtempR = 0.0;
		}
	}
	else if (GetAltitude() < 2800  && GetAltitude() > 1200){

		oapiGetHeading(GetHandle(),&heading);
		heading = heading*180./PI;
		//sprintf(oapiDebugString(), "Autopilot Heading Mode DEG %f", heading);
		oapiGetFocusBank(&bank);
		bank = bank*180./PI;

		if (fabs(heading-TO_HDG) <10 && StopRot) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		if (fabs(heading-TO_HDG) <0.5) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
		else if (heading > (TO_HDG +0.5) && TO_HDG <90) {
			AtempP = heading - TO_HDG ;
			if (AtempP > 1.0) AtempP = 1.0;
			AtempR = -1.0;
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, -1);
		}
		else if (heading < (TO_HDG -0.5) && TO_HDG <90) {
			AtempP = heading  - TO_HDG;
			if (AtempP<-1.0) AtempP = -1.0;
			AtempR = 0.2;
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, 1);
		}
		else if (heading > (TO_HDG +0.5) && TO_HDG >90) {
			AtempP = -(heading - TO_HDG) ;
			if (AtempP<-1.0) AtempP = -1.0;
			AtempR = -0.2;
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, -1);
		}
		else if (heading < (TO_HDG -0.5) && TO_HDG >90) {
			AtempP = -(heading  - TO_HDG);
			if (AtempP > 1.0) AtempP = 1.0;
			AtempR = 1.0;
			AtempY = 0.0;
			//vessel->SetAttitudeRotLevel(0, 1);
		}
		else {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;
		}
	}
	else if (GetAltitude() < 4500  && GetAltitude() > 2800) {
		oapiGetFocusBank(&bank);
		bank = bank*180./PI;
		oapiGetHeading(GetHandle(),&heading);
		heading = heading*180./PI;

		if (fabs(bank) < 0.5) {
			AtempP = 0.0;
			AtempR = 0.0;
			AtempY = 0.0;

		}else if (bank < 0 && fabs(vsp.vrot.z) < 0.11) {
			AtempR = 1.0;
			//vessel->SetAttitudeRotLevel(2,-1 );//bank/30
		}else if (bank > 0 && fabs(vsp.vrot.z) < 0.11) {
			AtempR = -1.0;
			//vessel->SetAttitudeRotLevel(2, 1);
		}else {
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
	}
	else if (GetAltitude() > 4500)	{

		// navigation
		oapiGetFocusPitch(&pitch);
		pitch = pitch*180./PI;
		oapiGetFocusAltitude(&altitude);

	 // guidance
		pitch_c = GetCPitch(autoT);

	 // control
		double SatApo;
		GetApDist(SatApo);

		if (SatApo >= ((agc.GetDesiredApogee() *.90) + ERADIUS)*1000){
			double TimeW=0;
			TimeW = oapiGetTimeAcceleration ();
			if (TimeW>1){
			//TimeW = 1;
			//oapiSetTimeAcceleration (1);
			}
			pitch_c = SetPitchApo();
			level = pitch_c - pitch;
		}
		else if (MissionTime < 150){
			level = pitch_c - pitch;
		}else{
			if (stage >= LAUNCH_STAGE_SIVB){
				pitch_c = 35;
			}
			else{
				pitch_c = GetCPitch(autoT);
			}
		level = pitch_c - pitch;
	}
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
		AtempP = -(fabs(level)/10);
		if (AtempP < -1.0)
			AtempP = -1.0;
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
			AttitudeLaunch1();
			break;

			case LAUNCH_STAGE_TWO:
			case LAUNCH_STAGE_SIVB:
			AttitudeLaunchSIVB();
			break;
		}
	}
}
