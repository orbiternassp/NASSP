/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn
  Saturn common autopilot code

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
  *	Revision 1.2  2010/08/25 17:48:42  tschachim
  *	Bugfixes Saturn autopilot.
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.9  2008/04/11 11:50:00  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.8  2007/06/06 15:02:17  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.7  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.6  2006/08/27 21:51:12  tschachim
  *	Improved SIVB roll damping.
  *	
  *	Revision 1.5  2006/07/27 23:24:11  tschachim
  *	The Saturn 1b now has the Saturn V IGM autopilot.
  *	
  *	Revision 1.4  2006/02/05 20:55:09  lazyd
  *	Added roll control (RCS) to SIVB
  *	
  *	Revision 1.3  2005/08/24 00:30:00  movieman523
  *	Revised CM RCS code, and removed a load of switches that aren't used anymore.
  *	
  *	Revision 1.2  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
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

void Saturn::AttitudeLaunchSIVB()

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
	if(AutopilotActive()){
		tempP = AtempP;
		tempR = AtempR;
		tempY = AtempY;
	}else{
		tempP = GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempY = GetManualControlLevel(THGROUP_ATT_YAWLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_YAWRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
		tempR = GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
	}

//*************************************************************
//Creates correction factors for rate control in each axis as a function of input level
// and current angular velocity. Varies from 1 to 0 as angular velocity approaches command level
// multiplied by maximum rate desired
	if (tempR != 0.0) {
		rollcorrect = (1./(fabs(tempR)*0.35))*((fabs(tempR)*0.35)-fabs(ang_vel.z));
	}
	if (tempP != 0.0) {
		pitchcorrect = (1./(fabs(tempP)*0.175))*((fabs(tempP)*0.175)-fabs(ang_vel.x));
		if ((tempP > 0 && ang_vel.x > 0) || (tempP < 0 && ang_vel.x < 0)) {
			pitchcorrect = 1;
		}
	}
	if (tempY != 0.0) {
		yawcorrect = (1./(fabs(tempY)*0.175))*((fabs(tempY)*0.175)-fabs(ang_vel.y));
		if ((tempY > 0 && ang_vel.y < 0) || (tempY < 0 && ang_vel.y > 0)) {
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
		pitchvectorm=_V(0.0, 0.95 * ang_vel.x * 2., 0.0);
	}
	if(tempY==0.0) {
		yawvectorm=_V(-0.95 * ang_vel.y * 2., 0.0, 0.0);
	}
	if(tempR==0.0) {
		rollvectorl = _V(0.0, 0.99 * ang_vel.z * 10., 0.0);
	}

//**************************************************************
// Roll control
	SetSaturnAttitudeRotLevel(_V(0, 0, -rollvectorl.y));

//**************************************************************
// Sets thrust vectors by simply adding up all the axis deflection vectors and the
// "neutral" default vector
	LimitSetThrusterDir(th_main[0],pitchvectorm+yawvectorm+_V( 0,0,1));//4
	// sprintf (oapiDebugString(), "roll input: %f, roll vel: %f", tempR, ang_vel.z);
}

void Saturn::LinearGuidance(VECTOR3 &target, double &pitch, double &yaw)
// target.x = total velocity
// target.y = vertical velocity
// target.z = cutoff altitude
{
	const double GRAVITY=6.67259e-11;
	VECTOR3 pos, vel, hvel, acc, h, ve;
	double altitude, crossrange, crossvel, Mass, vthrust, velexh, fuelflow, totvel, veltbg, tau, tgo, 
		velo, velv, altco, L, D12, D21, E, B, D, A, C, centrip, grav, acctot, simt, mu, p, e, a, apo, 
		thmax, thlvl;
	VESSELSTATUS2 status;
	status.version=2;
	status.flag=0;
	GetStatusEx(&status);
	simt=oapiGetSimTime();
	OBJHANDLE hbody=GetGravityRef();
	double bradius=oapiGetSize(hbody);
	double bmass=oapiGetMass(hbody);
	velo=target.x;
	velv=target.y;
	altco=target.z;
	mu=GRAVITY*bmass;
	pos=status.rpos;
	vel=status.rvel;

	h=CrossProduct(pos, vel);
	ve=(pos*(Mag(vel)*Mag(vel)-(mu/Mag(pos)))-vel*(pos*vel))/mu;
	p=(h*h)/mu;
	e=Mag(ve);
	a=p/(1.0-e*e);
	apo=a*(1.0+e)-bradius;

	GetHorizonAirspeedVector(hvel);
	altitude=GetAltitude();
	crossrange=pos*normal;
	crossvel=vel*normal;
	Mass=GetMass();
//	sprintf(oapiDebugString(), "mass=%.3f", Mass);
	thmax=0.0;

	int i;
	double tmax;
	THGROUP_HANDLE thm=GetThrusterGroupHandle(THGROUP_MAIN);
	int num=GetGroupThrusterCount(THGROUP_MAIN);
	for(i=0; i<num;i++){
		THRUSTER_HANDLE th=GetGroupThruster(THGROUP_MAIN, i);
		tmax=GetThrusterMax(th);
		thmax=thmax+tmax;
		velexh=GetThrusterIsp(th);
	}
	thlvl=GetThrusterGroupLevel(THGROUP_MAIN);

	vthrust=thmax*thlvl;
	if(thlvl < 0.7) {
		pitch = GetPitch();	
//		fprintf(outstr, "lvl=%.1f pitch=%.3f\n", thlvl, pitch*DEG);
		return;
	}
	fuelflow=vthrust/velexh;
	totvel=Mag(vel);
	if(totvel > velo) {
//		SetThrusterGroupLevel(THGROUP_MAIN, 0.0);
		pitch = GetPitch();	
		return;
	}
	veltbg=velo-totvel;
	tau=Mass/fuelflow;
	//estimate time-to-go
	tgo=tau*(1.0-exp(-(veltbg/velexh)));
//	sprintf(oapiDebugString(), "IGM tgo=%.1f tgt=%.1f %.1f %.1f", tgo, target);
//	fprintf(outstr, "th=%.1f lvl=%.3f max=%.1f isp=%.1f fl=%.3f vel=%.1f tgo=%.1f acc=%.3f\n", 
//		vthrust, thlvl, thmax, velexh, fuelflow, totvel, tgo, vthrust/Mass);
//	BurnEndTime=simt+tgo;
	//now compute the linear guidance coefficients
	L=log(1.0-tgo/tau);
	D12=tau+tgo/L;
	D21=tgo-D12;
	E=tgo/2.0-D21;
	B=(D21*(velv-hvel.y)-(altco-altitude-hvel.y*tgo))/(tgo*E);
	D=(D21*(-crossvel)-(-crossrange-crossvel*tgo))/(tgo*E);
	A=-D12*B-(velv-hvel.y)/L;
//	fprintf(outstr, "L=%.3f altco=%.1f alt=%.1f velv=%.1f vy=%.1f D12=%.3f velo=%.1f vel=%.1f\n",
//		L, altco, altitude, velv, hvel.y, D12, velo, totvel);
	C=-D12*D-(-crossvel)/L;
	centrip=(totvel*totvel-hvel.y*hvel.y)/(bradius+altitude);
	grav=(GRAVITY*bmass)/((bradius+altitude)*(bradius+altitude));
//	fprintf(outstr, "tau=%.3f L=%.3f D12=%.3f D21=%.3f E=%.3f B=%.3f D=%.3f A=%.3f C=%.3f\n",
//		tau, L, D12, D21, E, B, D, A, C);
	acctot=vthrust/Mass;
	acc.y=(1.0/tau)*(A+B)-centrip+grav;
	acc.z=(1.0/tau)*(C+D);
	
	double zmax=acctot*0.2;
	if(fabs(acc.z) > zmax) {
		acc.z=zmax*(acc.z/fabs(acc.z));
	}
	double xacc2=acctot*acctot-acc.y*acc.y-acc.z*acc.z;
	if(xacc2 > 0.0) {
		acc.x=sqrt(xacc2);
		pitch=atan(acc.y/acc.x);
	} else {
		acc.x=0.0;
		acc.y=acctot;
		acc.z=0.0;
		pitch=PI*0.5;
	}

	
	if(tgo < 5.0) pitch = GetPitch();
	yaw=asin(acc.z/acctot);
//	fprintf(outstr, "tau=%.3f a=%.5f b=%.5f c=%.3f g=%.3f g-c=%.3f\n", tau, A, B, centrip, grav, grav-centrip);
//	fprintf(outstr, "tgo=%.1f acc=%.3f %.3f %.3f cpitch=%.3f pitch=%.3f velo=%.1f vel=%.1f\n", 
//		tgo, acc, pitch*DEG, GetPitch()*DEG, velo, totvel);
//	sprintf(oapiDebugString(), "tgo=%.1f acc=%.3f %.3f %.3f cpitch=%.3f pitch=%.3f velo=%.1f vel=%.1f", 
//		tgo, acc, pitch*DEG, GetPitch()*DEG, velo, totvel);
//	sprintf(oapiDebugString(), "acc=%.3f %.3f %.3f atot=%.3f cvel=%.3f cr=%.1f yaw=%.3f",
//		acc, acctot, crossvel, crossrange, yaw*DEG);
}
