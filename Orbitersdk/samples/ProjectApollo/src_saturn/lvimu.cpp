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

  **************************************************************************/

// This is an abbreviated ST-124 IMU for the launch vehicle's use.

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "nasspdefs.h"
#include "LVIMU.h"
#include "papi.h"

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
	ZeroIMUCDUFlag = false;
	CoarseAlignEnableFlag = false;
	
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

	LastWeightAcceleration = _V(0, 0, 0);
	LastGlobalVel = _V(0, 0, 0);

	OurVessel = 0;

	CDURegisters[LVRegCDUX]=0;
	CDURegisters[LVRegCDUY]=0;
	CDURegisters[LVRegCDUZ]=0;
	CDURegisters[LVRegPIPAX]=0;
	CDURegisters[LVRegPIPAY]=0;
	CDURegisters[LVRegPIPAZ]=0;

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

bool LVIMU::IsPowered()

{
	return true; // We may simulate the LV power system later
}


void LVIMU::Timestep(double simt) 

{
	double deltaTime, pulses;

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
	
	// fill OrbiterData
	VESSELSTATUS vs;
	OurVessel->GetStatus(vs);

	Orbiter.Attitude.X = vs.arot.x;
	Orbiter.Attitude.Y = vs.arot.y;
	Orbiter.Attitude.Z = vs.arot.z;

	// Vessel to Orbiter global transformation
	MATRIX3	tinv = getRotationMatrixZ(-vs.arot.z);
	tinv = mul(getRotationMatrixY(-vs.arot.y), tinv);
	tinv = mul(getRotationMatrixX(-vs.arot.x), tinv);

	if (!Initialized) {
		SetOrbiterAttitudeReference();

		// Get current weight vector in vessel coordinates
		VECTOR3 w;
		OurVessel->GetWeightVector(w);
		// Transform to Orbiter global and calculate weight acceleration
		w = mul(tinv, w) / OurVessel->GetMass();
		LastWeightAcceleration = w;

		OurVessel->GetGlobalVel(LastGlobalVel);

		LastTime = simt;
		Initialized = true;
	} 
	else {
		deltaTime = (simt - LastTime);


		// Calculate accelerations
		VECTOR3 w, vel;
		OurVessel->GetWeightVector(w);
		// Transform to Orbiter global and calculate accelerations
		w = mul(tinv, w) / OurVessel->GetMass();
		OurVessel->GetGlobalVel(vel);
		VECTOR3 dvel = (vel - LastGlobalVel) / deltaTime;

		// Measurements with the 2006-P1 version showed that the average of the weight 
		// vector of this and the last step match the force vector while in free fall
		// The force vector matches the global velocity change of the last timestep exactly,
		// but isn't used because GetForceVector isn't working while docked
		VECTOR3 dw1 = w - dvel;
		VECTOR3 dw2 = LastWeightAcceleration - dvel;	
		VECTOR3 accel = -(dw1 + dw2) / 2.0;
		LastWeightAcceleration = w;
		LastGlobalVel = vel;

		// orbiter earth rotation
		// imuState->Orbiter.Y = imuState->Orbiter.Y + (deltaTime * TwoPI / 86164.09);		
		// sprintf(oapiDebugString(), "accel x %.10f y %.10f z %.10f DT %f", accel.x, accel.y, accel.z, deltaTime);								

		// Process channel bits				
		if (ZeroIMUCDUFlag) {
			ZeroIMUCDUs();
		}
		else if (CoarseAlignEnableFlag) {
			SetOrbiterAttitudeReference();
		}
		else if (Caged) {
			SetOrbiterAttitudeReference();
		}
		else {
			// Gimbals
			MATRIX3 t = Orbiter.AttitudeReference;
	  		t = mul(getRotationMatrixX(Orbiter.Attitude.X), t);
	  		t = mul(getRotationMatrixY(Orbiter.Attitude.Y), t);
	  		t = mul(getRotationMatrixZ(Orbiter.Attitude.Z), t);
	  		
	  		t = mul(getOrbiterLocalToNavigationBaseTransformation(), t);
	  		
			// calculate the new gimbal angles
			VECTOR3 newAngles = getRotationAnglesXZY(t);

			// drive gimbals to new angles		  		  				  		  	 	 	  		  	
			// CAUTION: gimbal angles are left-handed
			DriveGimbalX(-newAngles.x - Gimbal.X);
		  	DriveGimbalY(-newAngles.y - Gimbal.Y);
		  	DriveGimbalZ(-newAngles.z - Gimbal.Z);

			// PIPAs
			accel = tmul(Orbiter.AttitudeReference, accel);
			// sprintf(oapiDebugString(), "accel x %.10f y %.10f z %.10f DT %f", accel.x, accel.y, accel.z, deltaTime);								

			// pulse PIPAs			
			//pulses = RemainingPIPA.X + (accel.x * deltaTime / 0.0585);
			//PulsePIPA(LVRegPIPAX, (int) pulses);
			//RemainingPIPA.X = pulses - (int) pulses;
			
			//pulses = RemainingPIPA.Y + (accel.y * deltaTime / 0.0585);
			//PulsePIPA(LVRegPIPAY, (int) pulses);
			//RemainingPIPA.Y = pulses - (int) pulses;

			//pulses = RemainingPIPA.Z + (accel.z * deltaTime / 0.0585);
			//PulsePIPA(LVRegPIPAZ, (int) pulses);
			//RemainingPIPA.Z = pulses - (int) pulses;			

			pulses = (accel.x * deltaTime);
			PulsePIPA(LVRegPIPAX, pulses);
						
			pulses = (accel.y * deltaTime);
			PulsePIPA(LVRegPIPAY, pulses);
			
			pulses = (accel.z * deltaTime);
			PulsePIPA(LVRegPIPAZ, pulses);			
		}
		LastTime = simt;
	}	
}

void LVIMU::PulsePIPA(int RegPIPA, double pulses) 

{
	CDURegisters[RegPIPA] += pulses;
}

void LVIMU::ZeroPIPACounters()

{
	CDURegisters[LVRegPIPAX]=0;
	CDURegisters[LVRegPIPAY]=0;
	CDURegisters[LVRegPIPAZ]=0;
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
	// transformation to navigation base coordinates
	// CAUTION: gimbal angles are left-handed
	MATRIX3 t = getRotationMatrixY(-Gimbal.Y);
	t = mul(getRotationMatrixZ(-Gimbal.Z), t);
	t = mul(getRotationMatrixX(-Gimbal.X), t);
	
	// tranform to orbiter local coordinates
	t = mul(getNavigationBaseToOrbiterLocalTransformation(), t);
	
	// tranform to orbiter global coordinates
	t = mul(getRotationMatrixZ(-Orbiter.Attitude.Z), t);
	t = mul(getRotationMatrixY(-Orbiter.Attitude.Y), t);
	t = mul(getRotationMatrixX(-Orbiter.Attitude.X), t);

	// "Orbiter's REFSMMAT"
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
	// LEM FLAG HERE
	v.x = Gimbal.X;
	v.y = Gimbal.Y;
	v.z = Gimbal.Z;
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

void LVIMU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, LVIMU_START_STRING);

	papiWriteScenario_double(scn, "RPX", RemainingPIPA.X);
	papiWriteScenario_double(scn, "RPY", RemainingPIPA.Y);
	papiWriteScenario_double(scn, "RPZ", RemainingPIPA.Z);
	papiWriteScenario_double(scn, "GMX", Gimbal.X);
	papiWriteScenario_double(scn, "GMY", Gimbal.Y);
	papiWriteScenario_double(scn, "GMZ", Gimbal.Z);
	papiWriteScenario_double(scn, "OAX", Orbiter.Attitude.X);
	papiWriteScenario_double(scn, "OAY", Orbiter.Attitude.Y);
	papiWriteScenario_double(scn, "OAZ", Orbiter.Attitude.Z);
	papiWriteScenario_double(scn, "LAX", Orbiter.LastAttitude.X);
	papiWriteScenario_double(scn, "LAY", Orbiter.LastAttitude.Y);
	papiWriteScenario_double(scn, "LAZ", Orbiter.LastAttitude.Z);
	papiWriteScenario_double(scn, "VLX", Velocity.x);
	papiWriteScenario_double(scn, "VLY", Velocity.y);
	papiWriteScenario_double(scn, "VLZ", Velocity.z);
	papiWriteScenario_double(scn, "M11", Orbiter.AttitudeReference.m11);
	papiWriteScenario_double(scn, "M12", Orbiter.AttitudeReference.m12);
	papiWriteScenario_double(scn, "M13", Orbiter.AttitudeReference.m13);
	papiWriteScenario_double(scn, "M21", Orbiter.AttitudeReference.m21);
	papiWriteScenario_double(scn, "M22", Orbiter.AttitudeReference.m22);
	papiWriteScenario_double(scn, "M23", Orbiter.AttitudeReference.m23);
	papiWriteScenario_double(scn, "M31", Orbiter.AttitudeReference.m31);
	papiWriteScenario_double(scn, "M32", Orbiter.AttitudeReference.m32);
	papiWriteScenario_double(scn, "M33", Orbiter.AttitudeReference.m33);
	papiWriteScenario_double(scn, "LTM", LastTime);

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

MATRIX3 LVIMU::getRotationMatrixX(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the X axis (Pitch)
	
	MATRIX3 RotMatrixX;
	
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

MATRIX3 LVIMU::getRotationMatrixY(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Y axis (Yaw)

	MATRIX3 RotMatrixY;
	
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

MATRIX3 LVIMU::getRotationMatrixZ(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Z axis (Roll)

	MATRIX3 RotMatrixZ;
	
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

VECTOR3 LVIMU::getRotationAnglesXZY(MATRIX3 m) {
	
	VECTOR3 v;
	
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

VECTOR3 LVIMU::getRotationAnglesZYX(MATRIX3 m) {
	
	VECTOR3 v;
	
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

MATRIX3 LVIMU::getNavigationBaseToOrbiterLocalTransformation() {
	
	MATRIX3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	// LEM FLAG HERE
	m.m12 = -1.0;	
	m.m23 = 1.0;
	m.m31 = 1.0;
	return m;
} 

MATRIX3 LVIMU::getOrbiterLocalToNavigationBaseTransformation() {
	
	MATRIX3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	// LEM FLAG HERE
	m.m13 = 1.0;
	m.m21 = -1.0;	
	m.m32 = 1.0;
	return m;
}

// LV Rate Gyro

LVRG::LVRG() {
	sat = NULL;
	rates = _V(0,0,0);
}

void LVRG::Init(VESSEL *v) {
	// Initialize
	sat = v;
}

void LVRG::Timestep(double simdt) {
	rates = _V(0,0,0);
	if (sat != NULL) {
		VECTOR3 orbiter_rates = _V(0,0,0);
		sat->GetAngularVel(orbiter_rates); // From those, generate ROTATION RATE data.
		rates.x = -orbiter_rates.z;
		rates.y = orbiter_rates.x;
		rates.z = -orbiter_rates.y;
	}
}

