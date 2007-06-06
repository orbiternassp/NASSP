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
  *	Revision 1.1  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	
  **************************************************************************/

// This is an abbreviated ST-124 IMU for the launch vehicle's use.

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "LVIMU.h"

LVIMU::LVIMU()

{
	Init();
}

LVIMU::~LVIMU()

{
}

void LVIMU::Init() 

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

	ZeroIMUCDUs();
	LastTime = -1;
}

bool LVIMU::IsCaged()

{
	return Caged;
}

void LVIMU::SetCaged(bool val)

{
	if (Caged != val) {
		Caged = val;

		if (val) {
			ZeroIMUCDUs();
		}
	}
}

//
// Turn on the IMU. The LV IMU starts caged.
//

void LVIMU::TurnOn() 

{
	if (!Operate) {
		SetCaged(true);
		Operate = true;
		TurnedOn = true; // For LV, turn on here.
	}
}

void LVIMU::TurnOff() 

{
	if (Operate) {
		Operate = false;
		TurnedOn = false;
		Initialized = false;
	}
}

VECTOR3 LVIMU::CalculateAccelerations(double deltaT) 

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
	LVIMU_Matrix3	t = getRotationMatrixX(vs.arot.x);
	t = multiplyMatrix(getRotationMatrixY(vs.arot.y), t);
	t = multiplyMatrix(getRotationMatrixZ(vs.arot.z), t);

	LVIMU_Vector3 acc = VECTOR3ToLVIMU_Vector3(Acceleration); 
	acc = multiplyMatrixByVector(t, acc);
	Acceleration = LVIMU_Vector3ToVECTOR3(acc); 


	// ***********************************************************
	// calculate acceleration caused by gravity
	// Reference: page 5 of the "Guidance System Operations Plan" 
	// www.ibiblio.org/apollo/NARA-SW/R-577-sec5-rev4-5.3-5.5.pdf

	OurVessel->GetRelativePos(earth, pos);

	LVIMU_Vector3 p = VECTOR3ToLVIMU_Vector3(pos); 
	p = multiplyMatrixByVector(t, p);	
	pos = LVIMU_Vector3ToVECTOR3(p); 

	double distance = length(pos);
	pos = pos * (1.0 / distance);

	// "normal" force term
	GravityAcceleration =  pos * (0.3986032e15 / (distance * distance)); 

	//  non-spherical term
	LVIMU_Vector3 iuz = VECTOR3ToLVIMU_Vector3(_V(0.0, 1.0, 0.0));
	MATRIX3 m;
	oapiGetPlanetObliquityMatrix(earth, &m);
	LVIMU_Matrix3 im = MATRIX3ToLVIMU_Matrix3(m); 

	iuz = multiplyMatrixByVector(im, iuz);
	iuz = multiplyMatrixByVector(t, iuz);	
	VECTOR3 uz = LVIMU_Vector3ToVECTOR3(iuz);
	
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

bool LVIMU::IsPowered()

{
	return true; // We may simulate the LV power system later
}


void LVIMU::Timestep(double simt) 

{
	double deltaTime, pulses;
	LVIMU_Matrix3 t;
	LVIMU_Vector3 newAngles, acc, accI; 

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
				
		if (ZeroIMUCDUFlag) {
			ZeroIMUCDUs();
		}
		else if(CoarseAlignEnableFlag) {
			SetOrbiterAttitudeReference();
		}
		else if(Caged) {
			SetOrbiterAttitudeReference();
		}
		else {
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
			PulsePIPA(LVRegPIPAX, (int) pulses);
			RemainingPIPA.X = pulses - (int) pulses;

			pulses = RemainingPIPA.Y + (accI.y * deltaTime / 0.0585);
			PulsePIPA(LVRegPIPAY, (int) pulses);
			RemainingPIPA.Y = pulses - (int) pulses;

			pulses = RemainingPIPA.Z + (accI.z * deltaTime / 0.0585);
			PulsePIPA(LVRegPIPAZ, (int) pulses);
			RemainingPIPA.Z = pulses - (int) pulses;			
		}

		Orbiter.LastAttitude.X = Orbiter.Attitude.X;
		Orbiter.LastAttitude.Y = Orbiter.Attitude.Y;
		Orbiter.LastAttitude.Z = Orbiter.Attitude.Z;
		LastTime = simt;
	}	
}

void LVIMU::PulsePIPA(int RegPIPA, int pulses) 

{
	CDURegisters[RegPIPA] = pulses;
}

void LVIMU::DriveGimbals(double x, double y, double z) 

{
	DriveGimbal(0, LVRegCDUX, x - Gimbal.X);
	DriveGimbal(1, LVRegCDUY, y - Gimbal.Y);
	DriveGimbal(2, LVRegCDUZ, z - Gimbal.Z);
	SetOrbiterAttitudeReference();
}

void LVIMU::DriveGimbalX(double angle) 

{
	DriveGimbal(0, LVRegCDUX, angle);
}

void LVIMU::DriveGimbalY(double angle) 

{
	DriveGimbal(1, LVRegCDUY, angle);
}

void LVIMU::DriveGimbalZ(double angle) 

{
	DriveGimbal(2, LVRegCDUZ, angle);
}

void LVIMU::DriveGimbal(int index, int RegCDU, double angle) 

{
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
	
	// Gyro pulses to CDU pulses
	pulses = (int)(((double)radToGyroPulses(Gimbals[index])) / 64.0);
	CDURegisters[RegCDU] = pulses;
}

void LVIMU::DriveCDUX(int cducmd) 

{
    DriveCDU(0, LVRegCDUX, cducmd);
}

void LVIMU::DriveCDUY(int cducmd) 

{
    DriveCDU(1, LVRegCDUY, cducmd);
}

void LVIMU::DriveCDUZ(int cducmd) 

{
	DriveCDU(2, LVRegCDUZ, cducmd);
}

void LVIMU::DriveCDU(int index, int RegCDU, int cducmd) 

{
	int pulses;
	if (040000 & cducmd) {  // Negative?
		pulses = (040000 - cducmd) * 256;	// Coarse align
	} else {
		pulses = cducmd * 256;				// Coarse align
	}	
	
	DriveGimbal(index, RegCDU, gyroPulsesToRad(pulses));
	SetOrbiterAttitudeReference();
}

void LVIMU::SetOrbiterAttitudeReference() 
{
	LVIMU_Matrix3 t;

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

void LVIMU::ZeroIMUCDUs() 

{
	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;
	SetOrbiterAttitudeReference();
}

VECTOR3 LVIMU::GetTotalAttitude() 

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

void LVIMU::LoadState(FILEHANDLE scn)

{
	char *line;
	double flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, LVIMU_END_STRING, sizeof(LVIMU_END_STRING)))
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

void LVIMU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, LVIMU_START_STRING);

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

	oapiWriteLine(scn, LVIMU_END_STRING);
}

//
// These probably don't need to be part of the LV IMU class, but I've put them there
// for now to avoid touching the normal IMU in case I screw it up.
//

double LVIMU::degToRad(double angle) {
	return angle * PI / 180.0;	
}

double LVIMU::radToDeg(double angle) {
	return angle * 180.0 / PI;	
}

double LVIMU::gyroPulsesToRad(int pulses) {
	return (((double)pulses) * TWO_PI) / 2097152.0;
}

int LVIMU::radToGyroPulses(double angle) {
	return (int)((angle * 2097152.0) / TWO_PI);
}

LVIMU_Matrix3 LVIMU::getRotationMatrixX(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the X axis (Pitch)
	
	LVIMU_Matrix3 RotMatrixX;
	
	RotMatrixX.m11 = 1;
	RotMatrixX.m12 = 0;
	RotMatrixX.m13 = 0;
	RotMatrixX.m21 = 0;
	RotMatrixX.m22 = cos(angle);
	RotMatrixX.m23 = -sin(angle);
	RotMatrixX.m31 = 0;
	RotMatrixX.m32 = sin(angle);
	RotMatrixX.m33 = cos(angle);
	
	return RotMatrixX;
}

LVIMU_Matrix3 LVIMU::getRotationMatrixY(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Y axis (Yaw)

	LVIMU_Matrix3 RotMatrixY;
	
	RotMatrixY.m11 = cos(angle);
	RotMatrixY.m12 = 0;
	RotMatrixY.m13 = sin(angle);
	RotMatrixY.m21 = 0;
	RotMatrixY.m22 = 1;
	RotMatrixY.m23 = 0;
	RotMatrixY.m31 = -sin(angle);
	RotMatrixY.m32 = 0;
	RotMatrixY.m33 = cos(angle);
	
	return RotMatrixY;
}

LVIMU_Matrix3 LVIMU::getRotationMatrixZ(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Z axis (Roll)

	LVIMU_Matrix3 RotMatrixZ;
	
	RotMatrixZ.m11 = cos(angle);
	RotMatrixZ.m12 = -sin(angle);
	RotMatrixZ.m13 = 0;
	RotMatrixZ.m21 = sin(angle);
	RotMatrixZ.m22 = cos(angle);
	RotMatrixZ.m23 = 0;
	RotMatrixZ.m31 = 0;
	RotMatrixZ.m32 = 0;
	RotMatrixZ.m33 = 1;
	
	return RotMatrixZ;	
}

LVIMU_Matrix3 LVIMU::multiplyMatrix(LVIMU_Matrix3 a, LVIMU_Matrix3 b) {

	LVIMU_Matrix3 r;
	
	r.m11 = (a.m11 * b.m11) + (a.m12 * b.m21) + (a.m13 * b.m31);
	r.m12 = (a.m11 * b.m12) + (a.m12 * b.m22) + (a.m13 * b.m32);
	r.m13 = (a.m11 * b.m13) + (a.m12 * b.m23) + (a.m13 * b.m33);
	r.m21 = (a.m21 * b.m11) + (a.m22 * b.m21) + (a.m23 * b.m31);
	r.m22 = (a.m21 * b.m12) + (a.m22 * b.m22) + (a.m23 * b.m32);
	r.m23 = (a.m21 * b.m13) + (a.m22 * b.m23) + (a.m23 * b.m33);
	r.m31 = (a.m31 * b.m11) + (a.m32 * b.m21) + (a.m33 * b.m31);
	r.m32 = (a.m31 * b.m12) + (a.m32 * b.m22) + (a.m33 * b.m32);
	r.m33 = (a.m31 * b.m13) + (a.m32 * b.m23) + (a.m33 * b.m33);	
	return r;
}

LVIMU_Vector3 LVIMU::multiplyMatrixByVector(LVIMU_Matrix3 m, LVIMU_Vector3 v) {

	LVIMU_Vector3 r;

	r.x = (v.x * m.m11) + (v.y * m.m12) + (v.z * m.m13);
	r.y = (v.x * m.m21) + (v.y * m.m22) + (v.z * m.m23);
	r.z = (v.x * m.m31) + (v.y * m.m32) + (v.z * m.m33);

	return r;
}

LVIMU_Vector3 LVIMU::getRotationAnglesXZY(LVIMU_Matrix3 m) {
	
	LVIMU_Vector3 v;
	
	v.z = asin(-m.m12);
	
	if (m.m11 * cos(v.z) > 0) {		  	
		v.y = atan(m.m13 / m.m11);
	} else {
		v.y = atan(m.m13 / m.m11) + PI;
	}
	
	if (m.m22 * cos(v.z) > 0) {
		v.x = atan(m.m32 / m.m22);
	} else {
		v.x = atan(m.m32 / m.m22) + PI;
	}
	return v;
}

LVIMU_Vector3 LVIMU::getRotationAnglesZYX(LVIMU_Matrix3 m) {
	
	LVIMU_Vector3 v;
	
	v.y = asin(-m.m31);
	
	if (m.m33 * cos(v.y) > 0) {		  	
		v.x = atan(-m.m32 / m.m33);
	} else {
		v.x = atan(-m.m32 / m.m33) + PI;
	}
	
	if (m.m11 * cos(v.y) > 0) {
		v.z = atan(-m.m21 / m.m11);
	} else {
		v.z = atan(-m.m21 / m.m11) + PI;
	}
	return v;
}

LVIMU_Matrix3 LVIMU::getNavigationBaseToOrbiterLocalTransformation() {
	
	LVIMU_Matrix3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	if(LEM){
		m.m12 = 1.0;	
		m.m21 = 1.0;
		m.m33 = 1.0;
	}else{
		m.m12 = 1.0;	
		m.m23 = -1.0;
		m.m31 = 1.0;
	}
	return m;
} 

LVIMU_Matrix3 LVIMU::getOrbiterLocalToNavigationBaseTransformation() {
	
	LVIMU_Matrix3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	if(LEM){
		m.m12 = 1.0;
		m.m21 = 1.0;	
		m.m33 = 1.0;
	}else{
		m.m13 = 1.0;
		m.m21 = 1.0;	
		m.m32 = -1.0;
	}
	return m;
}


LVIMU_Vector3 LVIMU::VECTOR3ToLVIMU_Vector3(VECTOR3 v) {

	LVIMU_Vector3 iv;

	iv.x = v.x;
	iv.y = v.y;
	iv.z = v.z;
	return iv;
}

VECTOR3 LVIMU::LVIMU_Vector3ToVECTOR3(LVIMU_Vector3 iv) {

	VECTOR3 v;

	v.x = iv.x;
	v.y = iv.y;
	v.z = iv.z;
	return v;
}

LVIMU_Matrix3 LVIMU::MATRIX3ToLVIMU_Matrix3(MATRIX3 m) {

	LVIMU_Matrix3 im;

	im.m11 = m.m11;
	im.m12 = m.m12;
	im.m13 = m.m13;
	im.m21 = m.m21;
	im.m22 = m.m22;
	im.m23 = m.m23;
	im.m31 = m.m31;
	im.m32 = m.m32;
	im.m33 = m.m33;
	return im;
}
