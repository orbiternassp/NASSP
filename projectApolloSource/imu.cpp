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
#include "yaAGC/agc_engine.h"

#include "toggleswitch.h"
#include "saturn.h"

IMU::IMU(ApolloGuidance & comp) : agc(comp)

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
	
	LogInit();
}

void IMU::TurnOn() 

{
	if (!Operate) {
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
	int pulses;
	double delta;
  	IMU_Matrix3 t;
  	IMU_Vector3 newAngles;
	
	if (address != 07 && address != 033 /*&& address != 010*/) {  	  
    	LogState(address, "out", value);
	}

  	if (address == 012) {
    	ChannelValue12 val12;
    	val12.Value = value;

		if (val12.Bits.ISSTurnOnDelayComplete) 
		{
			agc.SetOutputChannelBit(030, 14, false);
			TurnedOn = true;
		}
    
    	if (val12.Bits.ZeroIMUCDUs) {
			ZeroIMUCDUs();

			agc.SetErasable(0, RegCDUX, 0);
			agc.SetErasable(0, RegCDUY, 0);
			agc.SetErasable(0, RegCDUZ, 0);
		}
	}
    	 
	if (!TurnedOn) {
		return;
	}

	if (address == 014) {
    	ChannelValue14 val14;
    	val14.Value = value;
  	  	
  		if (val14.Bits.DriveCDUX) { 
  	  		DriveCDUX();
  		}
  		if (val14.Bits.DriveCDUY) { 
  	  		DriveCDUY();
  		}
  		if (val14.Bits.DriveCDUZ) { 
  	  		DriveCDUZ();
  		}
  	
		if (val14.Bits.GyroActivity) {		  			  
		  	if (val14.Bits.GyroSign) {
				pulses = -agc.GetErasable(0, RegGYROCTR);
		  	} 
			else {
				pulses = agc.GetErasable(0, RegGYROCTR);
		  	}	  		  
		  	delta = gyroPulsesToRad(pulses);
		  	
		  	// gyro torquing is done in stable member coordinates
		  	if (val14.Bits.GyroSelectA && val14.Bits.GyroSelectB) {
		  		t = getRotationMatrixZ(delta);
		  	} 
			else if (val14.Bits.GyroSelectA) {
		  		t = getRotationMatrixY(delta);
		  	} 
			else if (val14.Bits.GyroSelectB) {
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

			// set to zero, otherwise the AGC adds the new pulses to the old ones 
			agc.SetErasable(0, RegGYROCTR, 0);
		}  	
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

	// ***********************************************************
	// Earth orbit "calibration" WITHOUT non-spherical gravity
	// TODO: better solution

/*	double orbitalt = (desired_apogee + desired_perigee) / 2.0;
	double accrad = dotp(pos, Acceleration);
	VECTOR3 accRad = pos * accrad;
	VECTOR3 accTan = Acceleration - accRad;

	int stage = ((Saturn *) OurVessel)->GetStage();
	if (stage < LAUNCH_STAGE_ONE || stage > LAUNCH_STAGE_SIVB) {
		double orbitdist = 6373338.0 + orbitalt;
		accRad = accRad * (1.0 + length(gb) / (0.3986032e15 / (orbitdist * orbitdist)));
	} else {
		
		double a = (-0.3 - -0.3) / (184.55 - 166.96);
		double b = -0.3 - (a * 184.55);
		double y = (a * orbitalt) + b;

		accRad = accRad * (1.0 + length(gb) * (accrad / fabs(accrad)) * y);
	
		double c = (0.85 - 0.52) / (72.06 - 91.5);
		double d = 0.85 - (c * 72.06);
		double z = (c * desired_azimuth) + d;

		accTan = accTan * (1.0 + length(gb) * z);

	}
	Acceleration = accRad + accTan;
*/

	// ***********************************************************
	// Earth orbit calibration WITH nonspherical gravity
	// TODO: better solution

	double accrad = dotp(pos, Acceleration);
	VECTOR3 accRad = pos * accrad;
	VECTOR3 accTan = Acceleration - accRad;

#if MUSTFIX // Commented out for the minute as this will go horribly wrong in the LEM.
	int stage = ((Saturn *) OurVessel)->GetStage();
	if (stage >= LAUNCH_STAGE_ONE && stage <= LAUNCH_STAGE_SIVB) {		
 		accRad = accRad * (1.0 + (accrad / fabs(accrad)) * -0.001);
		accTan = accTan * 1.0028;

		Acceleration = accRad + accTan;
	}
#endif

	// ***********************************************************
	// various debug-prints

/*	VECTOR3 hvel;
	double vvel = 0;
	if (OurVessel->GetHorizonAirspeedVector(hvel)) {
		vvel = hvel.y * 3.2808399;
	}	
	sprintf(oapiDebugString(), "Stage %d.%d Vert. Vel %.1f Alt %.1f Acc %f GAcc %f TAcc %f accR %f",  
		((Saturn *) OurVessel)->GetStage(), ((Saturn *) OurVessel)->GetStageState(), 
		vvel, OurVessel->GetAltitude() * 0.000539957 * 10, 
		length(Acceleration), length(GravityAcceleration), length(Acceleration + GravityAcceleration), 
		length(accR) * dotp(pos, Acceleration) / fabs(dotp(pos, Acceleration)));
*/

/*	sprintf(oapiDebugString(), "A X%f Y%f Z%f (%f) Ar X%f Y%f Z%f (%f) Ap X%f Y%f Z%f (%f)",  
		Acceleration.x, Acceleration.y, Acceleration.z, length(Acceleration), 
		accR.x, accR.y, accR.z, length(accR), 
		accP.x, accP.y, accP.z, length(accP));
*/
		
/*		sprintf(oapiDebugString(), "Uz X %f Y %f Z %f - Pos X %f Y %f Z %f (%f)",  
			uz.x, uz.y, uz.z, pos.x, pos.y, pos.z, distance); 
*/

//	VECTOR3 tacc = Acceleration + GravityAcceleration; 
/*	sprintf(oapiDebugString(), "A X%f Y%f Z%f (%f) GA X%f Y%f Z%f (%f) GAb X%f Y%f Z%f (%f) TA X%f Y%f Z%f (%f)",  
		Acceleration.x, Acceleration.y, Acceleration.z, length(Acceleration), 
		GravityAcceleration.x - gb.x, GravityAcceleration.y - gb.y, GravityAcceleration.z - gb.z, length(GravityAcceleration - gb), 
		gb.x, gb.y, gb.z, length(gb),
		tacc.x, tacc.y, tacc.z, length(tacc));
*/
/*	sprintf(oapiDebugString(), "A X%f Y%f Z%f (%f) GA X%f Y%f Z%f (%f) TA X%f Y%f Z%f (%f)",  
		Acceleration.x, Acceleration.y, Acceleration.z, length(Acceleration), 
		GravityAcceleration.x, GravityAcceleration.y, GravityAcceleration.z, length(GravityAcceleration), 
		tacc.x, tacc.y, tacc.z, length(tacc));
*/

	// TODO Test
/*	VECTOR3 ug = GravityAcceleration / length(GravityAcceleration);
	double accG = dotp(Acceleration, ug);

	VECTOR3 east = crossp(pos, uz);
	east = east / length(east);
	Acceleration += east * (dotp(Acceleration, east) * 0.1);

	Acceleration += ug * (accG * -0.2); 
	
	sprintf(oapiDebugString(), "East X %f Y %f Z %f (%f)", 
		east.x, east.y, east.z, length(east));
*/

/*	fprintf(logFile, "%f Dist %f Acc %f\n", 
		 oapiGetSimTime(), distance, length(Acceleration)); 
	fflush(logFile);
*/


	// ***********************************************************
	// store for next timestep

	Velocity = vel;

	return Acceleration + GravityAcceleration;
}

void IMU::Timestep(double simt) 

{
	double deltaTime, pulses;
	IMU_Matrix3 t;
	IMU_Vector3 newAngles, acc, accI; 
	ChannelValue12 val12;

	if (!TurnedOn) {
		return; 
	}

	VESSELSTATUS vs;

	// fill OrbiterData
	OurVessel->GetStatus(vs);
	double orbiterAttitudeX = vs.arot.x;
	double orbiterAttitudeY = vs.arot.y;
	double orbiterAttitudeZ = vs.arot.z;

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
				
		val12.Value = agc.GetInputChannel(012);
		if (val12.Bits.ZeroIMUCDUs) {
			ZeroIMUCDUs();
		}
		else if(val12.Bits.CoarseAlignEnable) {
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
			acc.x = accel.x;;
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

void IMU::PulsePIPA(int RegPIPA, int pulses) 

{
	int val = (agc.GetErasable(0, RegPIPA) + pulses);
	agc.SetErasable(0, RegPIPA, (val & 077777));
}

void IMU::DriveGimbals(double x, double y, double z) 

{
	DriveGimbal(0, RegCDUX, x - Gimbal.X, 0);
	DriveGimbal(1, RegCDUY, y - Gimbal.Y, 0);
	DriveGimbal(2, RegCDUZ, z - Gimbal.Z, 0);

	SetOrbiterAttitudeReference();
}

void IMU::DriveGimbalX(double angle) 

{
	DriveGimbal(0, RegCDUX, angle, 0);
}

void IMU::DriveGimbalY(double angle) 

{
	DriveGimbal(1, RegCDUY, angle, 0);
}

void IMU::DriveGimbalZ(double angle) 

{
	DriveGimbal(2, RegCDUZ, angle, 0);
}

void IMU::DriveGimbal(int index, int RegCDU, double angle, int changeCDU) 

{
  	int  pulses; //i, delta;
	
	Gimbals[index] += angle;
	if (Gimbals[index] >= TWO_PI) {
		Gimbals[index] -= TWO_PI;
	}
	if (Gimbals[index] < 0) {
		Gimbals[index] += TWO_PI;
	}
	
	// Gyro pulses to CDU pulses
//	if (changeCDU && angle != 0) {
//		imuLogMessage("Drive CDU");
		pulses = (int)(((double)radToGyroPulses(Gimbals[index])) / 64.0);	
/*		if (angle >= 0) {
			delta = pulses - *RegCDU;
			if (delta < 0) {
				delta += 32768;		// TwoPI in CDU pulses
			}
    		for (i = 0; i < delta; i++) {
				int CounterPCDU (int16_t *Counter);    		
      			CounterPCDU(RegCDU);
    		}
		} else {
			delta = *RegCDU - pulses;
			if (delta < 0) {
				delta += 32768;		// TwoPI in CDU pulses
			}
    		for (i = 0; i < delta; i++) {
				int CounterMCDU (int16_t *Counter);    		
      			CounterMCDU(RegCDU);
    		}
		}
*/
		int val = agc.GetErasable(0, RegCDU) + pulses;
		agc.SetErasable(0, RegCDU, (val & 077777));
//	}
}

void IMU::DriveCDUX() 

{
	DriveCDU(0, RegCDUX, RegCDUXCMD);
}

void IMU::DriveCDUY() 

{
	DriveCDU(1, RegCDUY, RegCDUYCMD);
}

void IMU::DriveCDUZ() 

{
	DriveCDU(2, RegCDUZ, RegCDUZCMD);
}

void IMU::DriveCDU(int index, int RegCDU, int RegCDUCMD) 

{
	int pulses;

	int cmd = agc.GetErasable(0, RegCDUCMD);

	if (040000 & cmd) {  // Negative?
		pulses = (cmd - 077777) * 256;	// Coarse align
	} else {
		pulses = cmd * 256;				// Coarse align
	}	
	DriveGimbal(index, RegCDU, gyroPulsesToRad(pulses), 1);
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

typedef union

{
	struct {
		unsigned Operate:1;
		unsigned TurnedOn:1;
		unsigned Initialized:1;
	} u;
	unsigned long word;
} IMUState;

void IMU::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IMU_END_STRING, sizeof(IMU_END_STRING)))
			return;
		if (!strnicmp (line, "RPX", 3)) {
			sscanf(line + 3, "%f", &flt);
			RemainingPIPA.X = flt;
		}
		else if (!strnicmp (line, "RPY", 3)) {
			sscanf(line + 3, "%f", &flt);
			RemainingPIPA.Y = flt;
		}
		else if (!strnicmp (line, "RPZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			RemainingPIPA.Z = flt;
		}
		else if (!strnicmp (line, "GMX", 3)) {
			sscanf(line + 3, "%f", &flt);
			Gimbal.X = flt;
		}
		else if (!strnicmp (line, "GMY", 3)) {
			sscanf(line + 3, "%f", &flt);
			Gimbal.Y = flt;
		}
		else if (!strnicmp (line, "GMZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			Gimbal.Z = flt;
		}
		else if (!strnicmp (line, "OAX", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.Attitude.X = flt;
		}
		else if (!strnicmp (line, "OAY", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.Attitude.Y = flt;
		}
		else if (!strnicmp (line, "OAZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.Attitude.Z = flt;
		}
		else if (!strnicmp (line, "LAX", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.LastAttitude.X = flt;
		}
		else if (!strnicmp (line, "LAY", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.LastAttitude.Y = flt;
		}
		else if (!strnicmp (line, "LAZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.LastAttitude.Z = flt;
		}
		else if (!strnicmp (line, "VLX", 3)) {
			sscanf(line + 3, "%f", &flt);
			Velocity.x = flt;
		}
		else if (!strnicmp (line, "VLY", 3)) {
			sscanf(line + 3, "%f", &flt);
			Velocity.y = flt;
		}
		else if (!strnicmp (line, "VLZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			Velocity.z = flt;
		}
		else if (!strnicmp (line, "M11", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m11 = flt;
		}
		else if (!strnicmp (line, "M12", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m12 = flt;
		}
		else if (!strnicmp (line, "M13", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m13 = flt;
		}
		else if (!strnicmp (line, "M21", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m21 = flt;
		}
		else if (!strnicmp (line, "M22", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m22 = flt;
		}
		else if (!strnicmp (line, "M23", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m23 = flt;
		}
		else if (!strnicmp (line, "M31", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m31 = flt;
		}
		else if (!strnicmp (line, "M32", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m32 = flt;
		}
		else if (!strnicmp (line, "M33", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m33 = flt;
		}
		else if (!strnicmp (line, "LTM", 3)) {
			sscanf(line + 3, "%f", &flt);
			LastTime = flt;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			IMUState state;
			sscanf (line+5, "%d", &state.word);

			Operate = (state.u.Operate != 0);
			Initialized = (state.u.Initialized != 0);
			TurnedOn = (state.u.TurnedOn != 0);
		}
	}
}

void IMU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IMU_START_STRING);

	oapiWriteScenario_float(scn, "RPX", RemainingPIPA.X);
	oapiWriteScenario_float(scn, "RPY", RemainingPIPA.Y);
	oapiWriteScenario_float(scn, "RPZ", RemainingPIPA.Z);
	oapiWriteScenario_float(scn, "GMX", Gimbal.X);
	oapiWriteScenario_float(scn, "GMY", Gimbal.Y);
	oapiWriteScenario_float(scn, "GMZ", Gimbal.Z);
	oapiWriteScenario_float(scn, "OAX", Orbiter.Attitude.X);
	oapiWriteScenario_float(scn, "OAY", Orbiter.Attitude.Y);
	oapiWriteScenario_float(scn, "OAZ", Orbiter.Attitude.Z);
	oapiWriteScenario_float(scn, "LAX", Orbiter.LastAttitude.X);
	oapiWriteScenario_float(scn, "LAY", Orbiter.LastAttitude.Y);
	oapiWriteScenario_float(scn, "LAZ", Orbiter.LastAttitude.Z);
	oapiWriteScenario_float(scn, "VLX", Velocity.x);
	oapiWriteScenario_float(scn, "VLY", Velocity.y);
	oapiWriteScenario_float(scn, "VLZ", Velocity.z);
	oapiWriteScenario_float(scn, "M11", Orbiter.AttitudeReference.m11);
	oapiWriteScenario_float(scn, "M12", Orbiter.AttitudeReference.m12);
	oapiWriteScenario_float(scn, "M13", Orbiter.AttitudeReference.m13);
	oapiWriteScenario_float(scn, "M21", Orbiter.AttitudeReference.m21);
	oapiWriteScenario_float(scn, "M22", Orbiter.AttitudeReference.m22);
	oapiWriteScenario_float(scn, "M23", Orbiter.AttitudeReference.m23);
	oapiWriteScenario_float(scn, "M31", Orbiter.AttitudeReference.m31);
	oapiWriteScenario_float(scn, "M32", Orbiter.AttitudeReference.m32);
	oapiWriteScenario_float(scn, "M33", Orbiter.AttitudeReference.m33);
	oapiWriteScenario_float(scn, "LTM", LastTime);

	//
	// Copy internal state to the structure.
	//

	IMUState state;

	state.word = 0;
	state.u.Operate = Operate;
	state.u.TurnedOn = TurnedOn;
	state.u.Initialized = Initialized;

	oapiWriteScenario_int (scn, "STATE", state.word);

	oapiWriteLine(scn, IMU_END_STRING);
}
