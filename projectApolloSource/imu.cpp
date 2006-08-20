/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  IMU code

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
  *	Revision 1.23  2006/06/29 22:38:44  tschachim
  *	Bugfix saving/loading
  *	
  *	Revision 1.22  2006/06/17 18:11:52  tschachim
  *	More precise saving/loading.
  *	
  *	Revision 1.21  2006/05/30 14:40:21  tschachim
  *	Fixed fuel cell - dc bus connectivity, added battery charger
  *	
  *	Revision 1.20  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.19  2006/04/25 13:33:43  tschachim
  *	Comment removed.
  *	
  *	Revision 1.18  2006/04/25 08:11:27  dseagrav
  *	Crash avoidance for DEBUG builds, LM IMU correction, LM still needs more work
  *	
  *	Revision 1.17  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.16  2006/02/13 21:35:02  tschachim
  *	Bugfix turn on process.
  *	
  *	Revision 1.15  2006/02/12 01:07:49  tschachim
  *	Bugfix coarse align.
  *	
  *	Revision 1.14  2006/01/14 20:58:16  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.13  2006/01/09 21:56:44  movieman523
  *	Added support for LEM and CSM AGC PAD loads in scenario file.
  *	
  *	Revision 1.12  2005/11/18 02:40:55  movieman523
  *	Major revamp of PanelSDK electrical code, and various modifications to run off fuel cells.
  *	
  *	Revision 1.11  2005/11/17 21:04:52  movieman523
  *	IMU and AGC now start powered-down. Revised battery code, and wired up all batteries in CSM.
  *	
  *	Revision 1.10  2005/10/19 11:28:18  tschachim
  *	Changed log file name.
  *	
  *	Revision 1.9  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.8  2005/08/19 13:41:03  tschachim
  *	Fixes because of new Virtual AGC version.
  *	
  *	Revision 1.7  2005/08/13 00:09:43  movieman523
  *	Added IMU Cage switch
  *	
  *	Revision 1.6  2005/08/12 17:49:42  movieman523
  *	Fixed stupid cut-and-paste error: IMU is up and running!
  *	
  *	Revision 1.5  2005/08/11 23:20:21  movieman523
  *	Fixed a few more IMU bugs and other odds and ends.
  *	
  *	Revision 1.4  2005/08/11 22:27:00  movieman523
  *	Fixed stupid cut-and-paste error in IMU.
  *	
  *	Revision 1.3  2005/08/11 12:16:23  spacex15
  *	fixed initialization bug
  *	
  *	Revision 1.2  2005/08/10 22:31:57  movieman523
  *	IMU is now enabled when running Prog 01.
  *	
  *	Revision 1.1  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"

#include "ioChannels.h"
#include "IMU.h"
#ifndef AGC_SOCKET_ENABLED
#include "yaAGC/agc_engine.h"
#endif

#include "toggleswitch.h"
#include "saturn.h"

#include "tracer.h"

#ifdef AGC_SOCKET_ENABLED
#define RegPIPAX 037
#define RegPIPAY 040
#define RegPIPAZ 041

#define RegCDUX 032
#define RegCDUY 033
#define RegCDUZ 034
#endif



IMU::IMU(ApolloGuidance & comp, PanelSDK &p) : agc(comp), DCPower(0, p), DCHeaterPower(0, p)

{
	Init();
}

IMU::~IMU()

{
}

void IMU::Init() 

{
	Operate = false;
	TurnedOn = false;
	Initialized = false;
	Caged = false;
	
	RemainingPIPA.X = 0;
	RemainingPIPA.Y = 0;
	RemainingPIPA.Z = 0;

	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;

	Orbiter.Attitude.X = 0;
	Orbiter.Attitude.Y = 0;
	Orbiter.Attitude.Z = 0;

	Orbiter.LastAttitude.X = 0;
	Orbiter.LastAttitude.Y = 0;
	Orbiter.LastAttitude.Z = 0;

	Orbiter.AttitudeReference.m11 = 0;
	Orbiter.AttitudeReference.m12 = 0;
	Orbiter.AttitudeReference.m13 = 0;
	Orbiter.AttitudeReference.m21 = 0;
	Orbiter.AttitudeReference.m22 = 0;
	Orbiter.AttitudeReference.m23 = 0;
	Orbiter.AttitudeReference.m31 = 0;
	Orbiter.AttitudeReference.m32 = 0;
	Orbiter.AttitudeReference.m33 = 0;

	Velocity.x = 0;
	Velocity.y = 0;
	Velocity.z = 0;

	OurVessel = 0;
	IMUHeater = 0;

	ZeroIMUCDUs();
	LastTime = -1;
	
	LogInit();
}

bool IMU::IsCaged()

{
	return Caged;
}

//
// Cage the IMU. I presume this also turns it off? -> NO!
//

void IMU::SetCaged(bool val)

{
	if (Caged != val) {
		Caged = val;
		agc.SetInputChannelBit(030, 11, val);

		if (val) {
			ZeroIMUCDUs();
		}
	}
}

//
// Turn on the IMU. For now we also uncage it.
//

void IMU::TurnOn() 

{
	if (!Operate) {
		SetCaged(false);
		agc.SetInputChannelBit(030, 9, true);
		agc.SetInputChannelBit(030, 14, true);
		Operate = true;
	}
}

void IMU::TurnOff() 

{
	if (Operate) {
		agc.SetInputChannelBit(030, 9, false);
		agc.SetInputChannelBit(030, 14, false);

		Operate = false;
		TurnedOn = false;
		Initialized = false;
	}
}

void IMU::ChannelOutput(int address, int value) 

{
	TRACESETUP("CHANNEL OUTPUT PROCESS");

	char buffers[80];
	sprintf(buffers,"CHANNEL OUTPUT %o %o", address,value);
	TRACE(buffers);

	int pulses;
	double delta;
  	IMU_Matrix3 t;
  	IMU_Vector3 newAngles;
    ChannelValue12 val12;
	
	if (address != 07 && address != 033 /*&& address != 010*/) {  	  
    	LogState(address, "out", value);
	}

  	if (address == 012) {
    	val12.Value = value;

		if (val12.Bits.ISSTurnOnDelayComplete) 
		{
			if(!TurnedOn)
			{
			    agc.SetInputChannelBit(030, 14, false);
			    TurnedOn = true;
			}
		}
    
    	if (val12.Bits.ZeroIMUCDUs) {
			ZeroIMUCDUs();
#ifndef AGC_SOCKET_ENABLED
			agc.SetErasable(0, RegCDUX, 0);
			agc.SetErasable(0, RegCDUY, 0);
			agc.SetErasable(0, RegCDUZ, 0);
#endif
		}
	}
    	 
	if (!TurnedOn) {
		return;
	}

	// coarse align 
	val12.Value = agc.GetOutputChannel(012);

	if(val12.Bits.CoarseAlignEnable) {
		if (address == 0174) {
			DriveCDUX(value);
		}
		if (address == 0175) {
			DriveCDUY(value);
		}
		if (address == 0176) {
			DriveCDUZ(value);
		}
	}
	
	// gyro torquing
	if (address == 0177) {
		ChannelValue177 val177;
		val177.Value = value;

		if (val177.Bits.GyroSign) {
			pulses = -1 * val177.Bits.GyroPulses; 
		} 
		else {
			pulses = val177.Bits.GyroPulses;
		}	  		  
		delta = gyroPulsesToRad(pulses);
		
		// gyro torquing is done in stable member coordinates
		if (val177.Bits.GyroSelectA && val177.Bits.GyroSelectB) {
		  	t = getRotationMatrixZ(delta);
		} 
		else if (val177.Bits.GyroSelectA) {
		  	t = getRotationMatrixY(delta);
		} 
		else if (val177.Bits.GyroSelectB) {
		  	t = getRotationMatrixX(delta);
		}
		
		// transformation to navigation base coordinates
		// CAUTION: gimbal angles are left-handed
		t = multiplyMatrix(getRotationMatrixY(-Gimbal.Y), t);
		t = multiplyMatrix(getRotationMatrixZ(-Gimbal.Z), t);
		t = multiplyMatrix(getRotationMatrixX(-Gimbal.X), t);
		
		// calculate the new gimbal angles
		newAngles = getRotationAnglesXZY(t);

		// drive gimabals to new angles
		// CAUTION: gimbal angles are left-handed			
		DriveGimbalX(-newAngles.x - Gimbal.X);
		DriveGimbalY(-newAngles.y - Gimbal.Y);
		DriveGimbalZ(-newAngles.z - Gimbal.Z);
		SetOrbiterAttitudeReference();
	}

}

VECTOR3 IMU::CalculateAccelerations(double deltaT) 

{
	VESSELSTATUS vs;
	VECTOR3 vel, pos, Acceleration, GravityAcceleration;

	// ***********************************************************
	// calculate acceleration caused by velocity change 
	// according to an inertial system
	
	OBJHANDLE earth = oapiGetGbodyByName("Earth");
	OurVessel->GetRelativeVel(earth, vel);
	//OurVessel->GetGlobalVel(vel);

	Acceleration = (vel - Velocity) * (1.0 / deltaT);

	// Transform to Orbiter local
	OurVessel->GetStatus(vs);
	IMU_Matrix3	t = getRotationMatrixX(vs.arot.x);
	t = multiplyMatrix(getRotationMatrixY(vs.arot.y), t);
	t = multiplyMatrix(getRotationMatrixZ(vs.arot.z), t);

	IMU_Vector3 acc = VECTOR3ToIMU_Vector3(Acceleration); 
	acc = multiplyMatrixByVector(t, acc);
	Acceleration = IMU_Vector3ToVECTOR3(acc); 


	// ***********************************************************
	// calculate acceleration caused by gravity
	// Reference: page 5 of the "Guidance System Operations Plan" 
	// www.ibiblio.org/apollo/NARA-SW/R-577-sec5-rev4-5.3-5.5.pdf

	OurVessel->GetRelativePos(earth, pos);

	IMU_Vector3 p = VECTOR3ToIMU_Vector3(pos); 
	p = multiplyMatrixByVector(t, p);	
	pos = IMU_Vector3ToVECTOR3(p); 

	double distance = length(pos);
	pos = pos * (1.0 / distance);

	// "normal" force term
	GravityAcceleration =  pos * (0.3986032e15 / (distance * distance)); 

	//  non-spherical term
	IMU_Vector3 iuz = VECTOR3ToIMU_Vector3(_V(0.0, 1.0, 0.0));
	MATRIX3 m;
	oapiGetPlanetObliquityMatrix(earth, &m);
	IMU_Matrix3 im = MATRIX3ToIMU_Matrix3(m); 

	iuz = multiplyMatrixByVector(im, iuz);
	iuz = multiplyMatrixByVector(t, iuz);	
	VECTOR3 uz = IMU_Vector3ToVECTOR3(iuz);
	
	double cosphi = dotp(pos, uz);
	double k = (0.3986032e15 / (distance * distance)) * (3.0 / 2.0) * 0.10823e-2 * (6378165.0 / distance) * (6378165.0 / distance);
	VECTOR3 gb = pos * (k * (1 - 5 * cosphi * cosphi));
	gb = gb + uz * (k * 2 * cosphi);

	// total gravity
	GravityAcceleration = GravityAcceleration + gb;

	// sprintf(oapiDebugString(), "Total Gravity %f", Acceleration + GravityAcceleration);

	// ***********************************************************
	// store for next timestep

	Velocity = vel;

	return Acceleration + GravityAcceleration;
}

bool IMU::IsPowered()

{
	if (DCPower.Voltage() < SP_MIN_DCVOLTAGE){ return false; }
	if (IMUHeater && !IMUHeater->pumping){ return false; }
	return true;
}

void IMU::WireHeaterToBuses(Boiler *heater, e_object *a, e_object *b)

{ 
	IMUHeater = heater;
	DCHeaterPower.WireToBuses(a, b);
	IMUHeater->WireTo(&DCHeaterPower);
}

void IMU::Timestep(double simt) 

{
	TRACESETUP("IMU::Timestep");

	double deltaTime, pulses;
	IMU_Matrix3 t;
	IMU_Vector3 newAngles, acc, accI; 
	ChannelValue12 val12;

	if (!Operate) {
		if (IsPowered())
			TurnOn();
		else
			return; 
	}
	else if (!IsPowered()) {
		TurnOff();
		return;
	}

	//
	// If we get here, we're powered up.
	//

	if (!TurnedOn) {
		return;
	}
	
	VESSELSTATUS vs;

	// fill OrbiterData
	OurVessel->GetStatus(vs);
	double orbiterAttitudeX;
	double orbiterAttitudeY;
	double orbiterAttitudeZ;

	// LM needs yaw and roll swapped from CM orientation
	orbiterAttitudeX = vs.arot.x;
	orbiterAttitudeY = vs.arot.y;
	orbiterAttitudeZ = vs.arot.z;

	if (!Initialized) {
		Orbiter.Attitude.X = orbiterAttitudeX;
		Orbiter.Attitude.Y = orbiterAttitudeY;
		Orbiter.Attitude.Z = orbiterAttitudeZ;
		SetOrbiterAttitudeReference();
		
		Orbiter.LastAttitude.X = orbiterAttitudeX;
		Orbiter.LastAttitude.Y = orbiterAttitudeY;
		Orbiter.LastAttitude.Z = orbiterAttitudeZ;	
		
		LastTime = simt;
		Initialized = true;
	} 
	else {
		deltaTime = (simt - LastTime);

		VECTOR3 accel = CalculateAccelerations(deltaTime);

		// orbiter earth rotation
		//imuState->Orbiter.Y = imuState->Orbiter.Y + (deltaTime * TwoPI / 86164.09);

		Orbiter.Attitude.X = orbiterAttitudeX;
		Orbiter.Attitude.Y = orbiterAttitudeY;
		Orbiter.Attitude.Z = orbiterAttitudeZ;
				
		val12.Value = agc.GetOutputChannel(012);
		if (val12.Bits.ZeroIMUCDUs) {
			ZeroIMUCDUs();
		}
		else if(val12.Bits.CoarseAlignEnable) {
			TRACE("CHANNEL 12 COARSE");
			SetOrbiterAttitudeReference();
		}
		else if(Caged) {
			SetOrbiterAttitudeReference();
		}
		else {

			TRACE("CHANNEL 12 NORMAL");
			// Gimbals
			t = Orbiter.AttitudeReference;
	  		t = multiplyMatrix(getRotationMatrixX(Orbiter.Attitude.X), t);
	  		t = multiplyMatrix(getRotationMatrixY(Orbiter.Attitude.Y), t);
	  		t = multiplyMatrix(getRotationMatrixZ(Orbiter.Attitude.Z), t);
	  		
	  		t = multiplyMatrix(getOrbiterLocalToNavigationBaseTransformation(), t);
	  		
			// calculate the new gimbal angles
			newAngles = getRotationAnglesXZY(t);

			// drive gimbals to new angles		  		  				  		  	 	 	  		  	
			// CAUTION: gimbal angles are left-handed
			DriveGimbalX(-newAngles.x - Gimbal.X);
		  	DriveGimbalY(-newAngles.y - Gimbal.Y);
		  	DriveGimbalZ(-newAngles.z - Gimbal.Z);

			// PIPAs
			acc.x = accel.x;
			acc.y = accel.y;
			acc.z = accel.z;

			// transformation to stable member coordinates
			t = getOrbiterLocalToNavigationBaseTransformation();
			// CAUTION: gimbal angles are left-handed
			t = multiplyMatrix(getRotationMatrixX(Gimbal.X), t);
	  		t = multiplyMatrix(getRotationMatrixZ(Gimbal.Z), t);
	  		t = multiplyMatrix(getRotationMatrixY(Gimbal.Y), t);
			accI = multiplyMatrixByVector(t, acc);

			// pulse PIPAs
			pulses = RemainingPIPA.X + (accI.x * deltaTime / 0.0585);
			PulsePIPA(RegPIPAX, (int) pulses);
			RemainingPIPA.X = pulses - (int) pulses;

			pulses = RemainingPIPA.Y + (accI.y * deltaTime / 0.0585);
			PulsePIPA(RegPIPAY, (int) pulses);
			RemainingPIPA.Y = pulses - (int) pulses;

			pulses = RemainingPIPA.Z + (accI.z * deltaTime / 0.0585);
			PulsePIPA(RegPIPAZ, (int) pulses);
			RemainingPIPA.Z = pulses - (int) pulses;			
		}

		Orbiter.LastAttitude.X = Orbiter.Attitude.X;
		Orbiter.LastAttitude.Y = Orbiter.Attitude.Y;
		Orbiter.LastAttitude.Z = Orbiter.Attitude.Z;
		LastTime = simt;
	}	
}

void IMU::SystemTimestep(double simdt) 

{
	if (Operate) {
		if (Caged)
			DCPower.DrawPower(61.7);
		else
			DCPower.DrawPower(325.0);
	}
}


void IMU::PulsePIPA(int RegPIPA, int pulses) 

{
	agc.PulsePIPA(RegPIPA, pulses);
}

void IMU::DriveGimbals(double x, double y, double z) 

{
	DriveGimbal(0, RegCDUX, x - Gimbal.X);
	DriveGimbal(1, RegCDUY, y - Gimbal.Y);
	DriveGimbal(2, RegCDUZ, z - Gimbal.Z);
	SetOrbiterAttitudeReference();
}

void IMU::DriveGimbalX(double angle) 

{
	DriveGimbal(0, RegCDUX, angle);
}

void IMU::DriveGimbalY(double angle) 

{
	DriveGimbal(1, RegCDUY, angle);
}

void IMU::DriveGimbalZ(double angle) 

{
	DriveGimbal(2, RegCDUZ, angle);
}

void IMU::DriveGimbal(int index, int RegCDU, double angle) 

{
	TRACESETUP("DRIVE GIMBAL");
  	int  pulses; //i, delta;
	double OldGimbal;
	double delta;
	
	OldGimbal = Gimbals[index];
	Gimbals[index] += angle;
	if (Gimbals[index] >= TWO_PI) {
		Gimbals[index] -= TWO_PI;
	}
	if (Gimbals[index] < 0) {
		Gimbals[index] += TWO_PI;
	}
	delta = Gimbals[index] - OldGimbal;
	if(delta > PI)
		delta -= TWO_PI;
	if(delta < - PI)
		delta += TWO_PI;
	
#ifdef AGC_SOCKET_ENABLED

    int channel,i;

    channel = RegCDU | 0x80;
	pulses = (int)(((double)radToGyroPulses(fabs(delta))) / 64.0);
	pulses = pulses & 077777;
	   
	LogState( channel, "inG", pulses);
	// sprintf(oapiDebugString(),"PCDU %d CHANNEL %o ", pulses,channel);

	if (delta >= 0) {
 		for (i = 0; i < pulses; i++) {
			agc.SetInputChannel(channel,1); // PCDU 
		}
	} else {
 		for (i = 0; i < pulses; i++) {
			agc.SetInputChannel(channel,3); // MCDU 
		}
	}
#else
	// Gyro pulses to CDU pulses
	pulses = (int)(((double)radToGyroPulses(Gimbals[index])) / 64.0);	
	agc.SetErasable(0, RegCDU, (pulses & 077777));

#endif
	char buffers[80];
	sprintf(buffers,"DRIVE GIMBAL index %o REGCDU %o angle %f pulses %o",index,RegCDU,angle,pulses);
	if (pulses)
		TRACE(buffers);
}

void IMU::DriveCDUX(int cducmd) 

{
    DriveCDU(0, RegCDUX, cducmd);
}

void IMU::DriveCDUY(int cducmd) 

{
    DriveCDU(1, RegCDUY, cducmd);
}

void IMU::DriveCDUZ(int cducmd) 

{
	DriveCDU(2, RegCDUZ, cducmd);
}

void IMU::DriveCDU(int index, int RegCDU, int cducmd) 

{
	TRACESETUP("DRIVECDU");

	int pulses;
	if (040000 & cducmd) {  // Negative?
		pulses = (040000 - cducmd) * 256;	// Coarse align
	} else {
		pulses = cducmd * 256;				// Coarse align
	}	

	char buffers[80];
	sprintf(buffers,"DRIVECDU index %o RegCDU %o cducmd %o pulses %o", index,RegCDU,cducmd,pulses);
	TRACE(buffers);
	
	DriveGimbal(index, RegCDU, gyroPulsesToRad(pulses));
	SetOrbiterAttitudeReference();
}

void IMU::SetOrbiterAttitudeReference() 
{
	IMU_Matrix3 t;

	// transformation to navigation base coordinates
	// CAUTION: gimbal angles are left-handed
	t = getRotationMatrixY(-Gimbal.Y);
	t = multiplyMatrix(getRotationMatrixZ(-Gimbal.Z), t);
	t = multiplyMatrix(getRotationMatrixX(-Gimbal.X), t);
	
	// tranform to orbiter local coordinates
	t = multiplyMatrix(getNavigationBaseToOrbiterLocalTransformation(), t);
	
	// tranform to orbiter global coordinates
	t = multiplyMatrix(getRotationMatrixZ(-Orbiter.Attitude.Z), t);
	t = multiplyMatrix(getRotationMatrixY(-Orbiter.Attitude.Y), t);
	t = multiplyMatrix(getRotationMatrixX(-Orbiter.Attitude.X), t);

	Orbiter.AttitudeReference = t;
}

void IMU::ZeroIMUCDUs() 

{
	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;
	SetOrbiterAttitudeReference();
}

VECTOR3 IMU::GetTotalAttitude() 

{
	VECTOR3 v;
	if(LEM){
		v.x = Gimbal.Z;
		v.y = Gimbal.Y;
		v.z = -Gimbal.X;
	}else{
		v.x = Gimbal.X;
		v.y = Gimbal.Y;
		v.z = Gimbal.Z;
	}
	return v;
}

typedef union

{
	struct {
		unsigned Operate:1;
		unsigned TurnedOn:1;
		unsigned Initialized:1;
		unsigned Caged:1;
	} u;
	unsigned long word;
} IMUState;

void IMU::LoadState(FILEHANDLE scn)

{
	char *line;
	double flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IMU_END_STRING, sizeof(IMU_END_STRING)))
			return;
		if (!strnicmp (line, "RPX", 3)) {
			sscanf(line + 3, "%lf", &flt);
			RemainingPIPA.X = flt;
		}
		else if (!strnicmp (line, "RPY", 3)) {
			sscanf(line + 3, "%lf", &flt);
			RemainingPIPA.Y = flt;
		}
		else if (!strnicmp (line, "RPZ", 3)) {
			sscanf(line + 3, "%lf", &flt);
			RemainingPIPA.Z = flt;
		}
		else if (!strnicmp (line, "GMX", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Gimbal.X = flt;
		}
		else if (!strnicmp (line, "GMY", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Gimbal.Y = flt;
		}
		else if (!strnicmp (line, "GMZ", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Gimbal.Z = flt;
		}
		else if (!strnicmp (line, "OAX", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.Attitude.X = flt;
		}
		else if (!strnicmp (line, "OAY", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.Attitude.Y = flt;
		}
		else if (!strnicmp (line, "OAZ", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.Attitude.Z = flt;
		}
		else if (!strnicmp (line, "LAX", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.LastAttitude.X = flt;
		}
		else if (!strnicmp (line, "LAY", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.LastAttitude.Y = flt;
		}
		else if (!strnicmp (line, "LAZ", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.LastAttitude.Z = flt;
		}
		else if (!strnicmp (line, "VLX", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Velocity.x = flt;
		}
		else if (!strnicmp (line, "VLY", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Velocity.y = flt;
		}
		else if (!strnicmp (line, "VLZ", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Velocity.z = flt;
		}
		else if (!strnicmp (line, "M11", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m11 = flt;
		}
		else if (!strnicmp (line, "M12", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m12 = flt;
		}
		else if (!strnicmp (line, "M13", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m13 = flt;
		}
		else if (!strnicmp (line, "M21", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m21 = flt;
		}
		else if (!strnicmp (line, "M22", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m22 = flt;
		}
		else if (!strnicmp (line, "M23", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m23 = flt;
		}
		else if (!strnicmp (line, "M31", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m31 = flt;
		}
		else if (!strnicmp (line, "M32", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m32 = flt;
		}
		else if (!strnicmp (line, "M33", 3)) {
			sscanf(line + 3, "%lf", &flt);
			Orbiter.AttitudeReference.m33 = flt;
		}
		else if (!strnicmp (line, "LTM", 3)) {
			sscanf(line + 3, "%lf", &flt);
			LastTime = flt;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			IMUState state;
			sscanf (line+5, "%d", &state.word);

			Operate = (state.u.Operate != 0);
			Initialized = (state.u.Initialized != 0);
			TurnedOn = (state.u.TurnedOn != 0);
			Caged = (state.u.Caged != 0);
		}
	}
}

inline void WriteScenario_double(FILEHANDLE scn, char *item, double d) {

	char buffer[256];

	sprintf(buffer, "  %s %lf", item, d);
	oapiWriteLine(scn, buffer);
}

void IMU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IMU_START_STRING);

	WriteScenario_double(scn, "RPX", RemainingPIPA.X);
	WriteScenario_double(scn, "RPY", RemainingPIPA.Y);
	WriteScenario_double(scn, "RPZ", RemainingPIPA.Z);
	WriteScenario_double(scn, "GMX", Gimbal.X);
	WriteScenario_double(scn, "GMY", Gimbal.Y);
	WriteScenario_double(scn, "GMZ", Gimbal.Z);
	WriteScenario_double(scn, "OAX", Orbiter.Attitude.X);
	WriteScenario_double(scn, "OAY", Orbiter.Attitude.Y);
	WriteScenario_double(scn, "OAZ", Orbiter.Attitude.Z);
	WriteScenario_double(scn, "LAX", Orbiter.LastAttitude.X);
	WriteScenario_double(scn, "LAY", Orbiter.LastAttitude.Y);
	WriteScenario_double(scn, "LAZ", Orbiter.LastAttitude.Z);
	WriteScenario_double(scn, "VLX", Velocity.x);
	WriteScenario_double(scn, "VLY", Velocity.y);
	WriteScenario_double(scn, "VLZ", Velocity.z);
	WriteScenario_double(scn, "M11", Orbiter.AttitudeReference.m11);
	WriteScenario_double(scn, "M12", Orbiter.AttitudeReference.m12);
	WriteScenario_double(scn, "M13", Orbiter.AttitudeReference.m13);
	WriteScenario_double(scn, "M21", Orbiter.AttitudeReference.m21);
	WriteScenario_double(scn, "M22", Orbiter.AttitudeReference.m22);
	WriteScenario_double(scn, "M23", Orbiter.AttitudeReference.m23);
	WriteScenario_double(scn, "M31", Orbiter.AttitudeReference.m31);
	WriteScenario_double(scn, "M32", Orbiter.AttitudeReference.m32);
	WriteScenario_double(scn, "M33", Orbiter.AttitudeReference.m33);
	WriteScenario_double(scn, "LTM", LastTime);

	//
	// Copy internal state to the structure.
	//

	IMUState state;

	state.word = 0;
	state.u.Operate = Operate;
	state.u.TurnedOn = TurnedOn;
	state.u.Initialized = Initialized;
	state.u.Caged = Caged;

	oapiWriteScenario_int (scn, "STATE", state.word);

	oapiWriteLine(scn, IMU_END_STRING);
}
