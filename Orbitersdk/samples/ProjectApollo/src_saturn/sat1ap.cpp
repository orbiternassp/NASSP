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

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
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

#include "tracer.h"


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
	TRACESETUP("Saturn1b::AutoPilot");

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

	AltitudePREV = altitude = GetAltitude();
	VESSELSTATUS vsp;
	GetStatus(vsp);
	double totalRot=0;
	totalRot=vsp.vrot.x+vsp.vrot.y+vsp.vrot.z;
	if (fabs(totalRot) >= 0.0025){
		StopRot = true;
	}

	// This vector rotation will be used to tell if heads up (rhoriz.z<0) or heads down.
	HorizonRot(_V(1,0,0), rhoriz);

	//
	// Shut down the engines when we reach the desired
	// orbit.
	//

	double apogee, perigee;
	OBJHANDLE ref = GetGravityRef();
	GetApDist(apogee);
	GetPeDist(perigee);
	apogee = (apogee - oapiGetSize(ref)) / 1000.;
	perigee = (perigee - oapiGetSize(ref)) / 1000.;

	// We're aiming for periapsis and shutdown when apoapsis is reached at the opposite side of the orbit
	if (apogee >= agc.GetDesiredApogee() && perigee >= agc.GetDesiredPerigee() - 0.1) {
		// See Saturn::CheckForLaunchShutdown()
		if (GetThrusterLevel(th_main[0]) > 0){
			SetThrusterLevel(th_main[0], 0);			
			if (oapiGetTimeAcceleration() > 1.0)
				oapiSetTimeAcceleration(1.0);

			// Reset autopilot commands
			AtempP  = 0;
			AtempY  = 0;
			AtempR  = 0;			
		}
		return;
	}

	// navigation
	pitch = GetPitch();
	pitch = pitch*180./PI;
	//sprintf(oapiDebugString(), "Autopilot %f", altitude);
	// guidance
	pitch_c = GetCPitch(autoT);
	// control
	if (altitude > 4500) {
		// Damp roll motion
		bank = GetBank();
		bank = bank *180. / PI;
		if (bank > 90) bank = bank - 180.;
		else if (bank < -90) bank = bank + 180.;
		AtempR = -bank / 20.0;
		if (fabs(bank) < 0.3) AtempR = 0;

		// navigation
		pitch = GetPitch();
		pitch = pitch * 180. / PI;

		if (IGMEnabled) {
			VECTOR3 target;
			double pit, yaw;
			double bradius = oapiGetSize(ref);
			double bmass = oapiGetMass(ref);
			double mu = GRAVITY * bmass;
			// Aim for periapsis
			double altco = agc.GetDesiredPerigee() * 1000.;
			double velo = sqrt(mu / (bradius + altco));
			target.x = velo;
			target.y = 0.0;
			target.z = altco;
			LinearGuidance(target, pit, yaw);
			AtempP=(pit * DEG - pitch) / 30.0;
			if (AtempP < -0.15) AtempP = -0.15;
			if (AtempP >  0.15) AtempP =  0.15;
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
			// sprintf(oapiDebugString(), "autoT %f AtempP %f AtempR %f AtempY %f altitude %f pitch %f pitch_c %f", 
			//  					       autoT, AtempP, AtempR, AtempY, altitude, pitch, pitch_c);
		}
	}
	// sprintf(oapiDebugString(), "Alt %f Pitch %f Roll %f Yaw %f autoT %f", altitude, AtempP, AtempR, AtempY, autoT);

	double slip;
	VECTOR3 az;
	VECTOR3 up, north, east, ygl, zgl, zerogl;
	OBJHANDLE hbody=GetGravityRef();
	double bradius=oapiGetSize(hbody);

	// set up our reference frame
	Local2Global(_V(0.0, 0.0, 0.0), zerogl);
	Local2Global(_V(0.0, 1.0, 0.0), ygl);
	Local2Global(_V(0.0, 0.0, 1.0), zgl);
	ygl=ygl-zerogl;
	zgl=zgl-zerogl;

	oapiGetHeading(GetHandle(),&heading);
	heading = heading*180./PI;

	// Inclination control
	static int incinit = 0; 
	static ELEMENTS elemlast; 
	static double incratelast;

	ELEMENTS elem;
	GetElements(ref, elem, 0, 0, FRAME_EQU);
	double incrate = (elem.i - elemlast.i) / oapiGetSimStep();
	double incraterate = (incrate - incratelast) / oapiGetSimStep();	
	double target = (agc.GetDesiredInclination() - elem.i * DEG) / (FirstStageShutdownTime - MissionTime);

	if (agc.GetDesiredInclination() != 0 && autoT > 45) {	
		if (incinit < 2) {
			incinit++;
			AtempY = 0;
		} else {
			if (autoT < FirstStageShutdownTime - 10) {	
				AtempY = (incrate * DEG - target) / 0.7 + incraterate * DEG / 2.;
				if (AtempY < -0.1) AtempY = -0.1;
				if (AtempY >  0.1) AtempY =  0.1;
			} else if (autoT < FirstStageShutdownTime + 10) {	
				AtempY = 0;
			} else {
				AtempY = (elem.i * DEG - agc.GetDesiredInclination()) / 7. + (incrate * DEG ) / 1.;
				if (AtempY < -0.01) AtempY = -0.01;
				if (AtempY >  0.01) AtempY =  0.01;
			}
		}
	}
	
	elemlast = elem;
	incratelast = incrate;

	// stage handling
	switch (stage){
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
				if (AtempR < -0.25) AtempR = -0.25;
				if (AtempR >  0.25) AtempR =  0.25;
			}

			if(autoT > 30.0 && autoT < 45.0) {
				//pitch and adjust for relative wind
				AtempR=asin(ygl*normal)*DEG/20.0;
				//AtempY=(slip+asin(zgl*normal)*DEG)/20.0;
				AtempY=(TO_HDG-(heading+slip))/20.0;
				if (AtempR < -0.25) AtempR = -0.25;
				if (AtempR >  0.25) AtempR =  0.25;
			}
			pitch = GetPitch();
			pitch=pitch*180./PI;
			pitch_c=GetCPitch(autoT);
			AtempP = (pitch_c - pitch);

			// Fix for LC 39
			if (autoT < 10 && heading > 180)
				AtempP = -(180. - pitch_c - pitch);

			if (AtempP > 1.0) AtempP = 1.0;
			if (AtempP < -1.0) AtempP = -1.0;

			// zero angle-of-attack...
			if(autoT > 45.0 && autoT < 115.0) {

				/// \todo Disabled for now, the Saturn 1B doesn't seem to do that...
				//double aoa=GetAOA()*DEG;
				//pitch_c=pitch+aoa-0.3;

				AtempP=(pitch_c - pitch) / 5.0;
				if(AtempP < -0.2) AtempP = -0.2;
				if(AtempP >  0.2) AtempP = 0.2;
				// sprintf(oapiDebugString(), " pitch=%.3f pc=%.3f ap=%.3f", pitch, pitch_c, AtempP);
			}
			if (autoT > 115.0) {
				if (autoT < 120.0) {
					if (AtempP < -0.1) AtempP = -0.1;
					if (AtempP >  0.1) AtempP =  0.1;
				} else {
					if (AtempP < -0.2) AtempP = -0.2;
					if (AtempP >  0.2) AtempP =  0.2;
				}
				normal=Normalize(CrossProduct(Normalize(vsp.rpos), Normalize(vsp.rvel)));
			}
			// sprintf(oapiDebugString(), "roll=%.3f yaw=%.3f slip=%.3f sum=%.3f hdg+slip=%.3f hdg=%.3f ay=%.3f", 
			//     asin(ygl*normal)*DEG, asin(zgl*normal)*DEG, slip, slip+asin(zgl*normal)*DEG, heading+slip, heading, AtempY);
			// sprintf(oapiDebugString(), "autoT %f AtempP %f AtempR %f AtempY %f altitude %f pitch %f pitch_c %f rhoriz.z %f", 
			//     autoT, AtempP, AtempR, AtempY, altitude, pitch, pitch_c, rhoriz.z);
			/*
			char buffer[80];
			sprintf(buffer,"AtempP %f AtempR %f AtempY %f", AtempP, AtempR, AtempY);	
			TRACE(buffer);
			*/

			AttitudeLaunch1();
			break;

		case LAUNCH_STAGE_SIVB:
			AttitudeLaunchSIVB();
			break;
	}

	// sprintf(oapiDebugString(), "AP - inc %f rate %f target %f raterate %f AtempP %f AtempR %f AtempY %f", elem.i * DEG, incrate * DEG, target, incraterate * DEG, AtempP, AtempR, AtempY);
	// sprintf(oapiDebugString(), "AP - pitch %f pitch_c %f heading %f AtempP %f AtempR %f AtempY %f", pitch, pitch_c, heading, AtempP, AtempR, AtempY);
}
