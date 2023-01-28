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

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "dsky.h"

#include "ioChannels.h"
#include "IMU.h"
#include "yaAGC/agc_engine.h"

#include "toggleswitch.h"
#include "saturn.h"
#include "tracer.h"
#include "papi.h"




IMU::IMU(ApolloGuidance &comp, PanelSDK &p, InertialData &inertialData) : agc(comp), DCPower(0, p), DCHeaterPower(0, p), inertialData(inertialData)

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
	Powered = false;
	
	RemainingPIPA.X = 0;
	RemainingPIPA.Y = 0;
	RemainingPIPA.Z = 0;

	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;

	Orbiter.Attitude_v2g = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	Orbiter.Attitude_g2v = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	Orbiter.AttitudeReference = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	OurVessel = 0;
	IMUHeater = 0;
	IMUHeat = 0;
	IMUCase = 0;
	PTAHeat = 0;
	PSAHeat = 0;
	CDUHeat = 0;
	PowerSwitch = 0;

	DoZeroIMUGimbals();
	LastSimDT = -1;
	IMUTempF = 0.0;
	
	LogInit();
}

void IMU::SetVessel(VESSEL *v, bool LEMFlag) 
{
	OurVessel = v;
	LEM = LEMFlag;
	if ( LEM ) 
		pipaRate = 0.01;	// LM: 1 pulse = 1 cm/s
	else
		pipaRate = 0.0585; // CSM: 1 pulse = 5.85 cm/s
};

void IMU::SetVesselFlag(bool LEMFlag)
{
	LEM = LEMFlag;
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
		agc.SetInputChannelBit(030, IMUCage, val);

		if (val) {
			DoZeroIMUGimbals();
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
		agc.SetInputChannelBit(030, ISSTurnOnRequest, true);
		// Clear IMUFail
		agc.SetInputChannelBit(030, IMUFail, false);
		Operate = true;
	}
}

void IMU::TurnOff() 

{
	if (Operate) {
		agc.SetInputChannelBit(030, ISSTurnOnRequest, false);

		// The IMU is monitored by a separate "IMU Fail Detect Logic",
		// which sets the IMUFail and IMUCDUFail bits of channel 030
		// under certain conditions, see CSM systems handbook 8.1 H9.
		// For now we just raise an IMUFail in case of an turn off.
		agc.SetInputChannelBit(030, IMUFail, true);

		Operate = false;
		TurnedOn = false;
		Initialized = false;
	}
}

void IMU::ChannelOutput(int address, ChannelValue value) 

{
	TRACESETUP("CHANNEL OUTPUT PROCESS");

	char buffers[80];
	sprintf(buffers,"CHANNEL OUTPUT %o %o", address,value.to_ulong());
	TRACE(buffers);

	int pulses;
	double delta;
  	MATRIX3 t;
  	VECTOR3 newAngles;
    ChannelValue val12;
	
	if (address != 07 && address != 033 /*&& address != 010*/) {  	  
    	LogState(address, "out", value.to_ulong());
	}

  	if (address == 012) {
    	val12 = value;

		if (val12[ISSTurnOnDelayComplete]) 
		{
			if(!TurnedOn)
			{
			    agc.SetInputChannelBit(030, ISSTurnOnRequest, false);
			    TurnedOn = true;
			}
		}
    
    	if (val12[ZeroIMUCDUs]) {
			DoZeroIMUCDUs();
			agc.ProcessIMUCDUReadCount(RegCDUX, 0);
			agc.ProcessIMUCDUReadCount(RegCDUY, 0);
			agc.ProcessIMUCDUReadCount(RegCDUZ, 0);
		}
	}
    	 
	if (!TurnedOn) {
		return;
	}

	// coarse align 
	val12 = agc.GetOutputChannel(012);

	if(val12[CoarseAlignEnable]) {
		if (address == 0174) {
			DriveCDUX(value.to_ulong());
		}
		if (address == 0175) {
			DriveCDUY(value.to_ulong());
		}
		if (address == 0176) {
			DriveCDUZ(value.to_ulong());
		}
	}
	
	// gyro torquing
	if (address == 0177) {
		ChannelValue177 val177;
		val177.Value = value.to_ulong();

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
		t = mul(getRotationMatrixY(-Gimbal.Y), t);
		t = mul(getRotationMatrixZ(-Gimbal.Z), t);
		t = mul(getRotationMatrixX(-Gimbal.X), t);
		
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

bool IMU::IsPowered()

{
	if (DCPower.Voltage() < SP_MIN_DCVOLTAGE) { return false; }
	//TBD: Implement IMU failure logic for very off nominal temperatures
	//if (IMUHeater && !IMUHeater->pumping) { return false; }
	if (PowerSwitch) {
		if (PowerSwitch->IsDown()) { return false; }
	}
	return true;
}

void IMU::WireToBuses(e_object *a, e_object *b, GuardedToggleSwitch *s) 

{ 
	DCPower.WireToBuses(a, b); 
	PowerSwitch = s;
}


void IMU::WireHeaterToBuses(Boiler *heater, e_object *a, e_object *b)

{ 
	IMUHeater = heater;
	DCHeaterPower.WireToBuses(a, b);
	if (IMUHeater)
		IMUHeater->WireTo(&DCHeaterPower);
}

void IMU::InitThermals(h_HeatLoad *imuht, h_Radiator *cas, h_HeatLoad* ptaht, h_HeatLoad* psaht, h_HeatLoad* cduht)
{
	IMUHeat = imuht;
	IMUCase = cas;
	PTAHeat = ptaht;
	PSAHeat = psaht;
	CDUHeat = cduht;
}

void IMU::Timestep(double simdt) 

{
	TRACESETUP("IMU::Timestep");

	double pulses;
	ChannelValue val12;

	//ISS Temperature Alarm Module

	bool tempBit = agc.GetInputChannelBit(030, TempInLimits);

	if (DCHeaterPower.Voltage() < SP_MIN_DCVOLTAGE)
	{
		if (tempBit) agc.SetInputChannelBit(030, TempInLimits, false);
	}
	else
	{
		if (IMUCase)
		{
			IMUTempF = KelvinToFahrenheit(IMUCase->GetTemp());
		}
		else
		{
			IMUTempF = 130.0;
		}

		if (IMUTempF > 126.0 && IMUTempF < 134.0)
		{
			if (!tempBit) agc.SetInputChannelBit(030, TempInLimits, true);
		}
		else
		{
			if (tempBit) agc.SetInputChannelBit(030, TempInLimits, false);
		}
	}

	//IMU On
	Powered = IsPowered();

	if (Powered)
	{
		agc.SetInputChannelBit(030, IMUOperate, true);
	}
	else
	{
		agc.SetInputChannelBit(030, IMUOperate, false);
	}
	
	if (!Operate) {
		if (Powered)
			TurnOn();
		else
			return; 
	}
	else if (!Powered) {
		TurnOff();
		return;
	}

	// fill OrbiterData
	OurVessel->GetRotationMatrix(Orbiter.Attitude_v2g);
	Orbiter.Attitude_g2v.m11 = Orbiter.Attitude_v2g.m11;
	Orbiter.Attitude_g2v.m12 = Orbiter.Attitude_v2g.m21;
	Orbiter.Attitude_g2v.m13 = Orbiter.Attitude_v2g.m31;
	Orbiter.Attitude_g2v.m21 = Orbiter.Attitude_v2g.m12;
	Orbiter.Attitude_g2v.m22 = Orbiter.Attitude_v2g.m22;
	Orbiter.Attitude_g2v.m23 = Orbiter.Attitude_v2g.m32;
	Orbiter.Attitude_g2v.m31 = Orbiter.Attitude_v2g.m13;
	Orbiter.Attitude_g2v.m32 = Orbiter.Attitude_v2g.m23;
	Orbiter.Attitude_g2v.m33 = Orbiter.Attitude_v2g.m33;

	// IMU turn on mode
	if (Operate && !TurnedOn)
		DoZeroIMUGimbals();

	if (!TurnedOn) {
		return;
	}

	// orbiter earth rotation
	//imuState->Orbiter.Y = imuState->Orbiter.Y + (deltaTime * TwoPI / 86164.09);

	// Process channel bits
	val12 = agc.GetOutputChannel(012);
	if (val12[ZeroIMUCDUs]) {
		DoZeroIMUCDUs();
	}
	else if (val12[CoarseAlignEnable]) {
		TRACE("CHANNEL 12 COARSE");
		SetOrbiterAttitudeReference();
	}
	else if (Caged) {
		SetOrbiterAttitudeReference();
	}
	else {

		TRACE("CHANNEL 12 NORMAL");

		// Gimbals
		MATRIX3 t = Orbiter.AttitudeReference;
		t = mul(Orbiter.Attitude_g2v, t);
		t = mul(getOrbiterLocalToNavigationBaseTransformation(), t);
	  		
		// calculate the new gimbal angles
		VECTOR3 newAngles = getRotationAnglesXZY(t);

		// drive gimbals to new angles
		// CAUTION: gimbal angles are left-handed
		DriveGimbalX(-newAngles.x - Gimbal.X);
		DriveGimbalY(-newAngles.y - Gimbal.Y);
		DriveGimbalZ(-newAngles.z - Gimbal.Z);

		// PIPAs
		VECTOR3 accel;
		inertialData.getAcceleration(accel);
		accel = mul(Orbiter.Attitude_v2g, -accel);
		accel = tmul(Orbiter.AttitudeReference, accel);

		// pulse PIPAs
		pulses = RemainingPIPA.X + (accel.x * LastSimDT / pipaRate);
		PulsePIPA(RegPIPAX, (int) pulses);
		RemainingPIPA.X = pulses - (int) pulses;

		pulses = RemainingPIPA.Y + (accel.y * LastSimDT / pipaRate);
		PulsePIPA(RegPIPAY, (int) pulses);
		RemainingPIPA.Y = pulses - (int) pulses;

		pulses = RemainingPIPA.Z + (accel.z * LastSimDT / pipaRate);
		PulsePIPA(RegPIPAZ, (int) pulses);
		RemainingPIPA.Z = pulses - (int) pulses;
	}
	LastSimDT = simdt;
}

void IMU::SystemTimestep(double simdt) 
{
	if (Operate) {
		if (Caged)
		{
			DCPower.DrawPower(61.7); //Need to check these values, no source
			if (IMUHeat)
				IMUHeat->GenerateHeat(8.9); //Need to check these values, no source
			if (PTAHeat)
				PTAHeat->GenerateHeat(8.9); //Need to check these values, no source
			if (PSAHeat)
				PSAHeat->GenerateHeat(8.9); //Need to check these values, no source
			if (CDUHeat)
				CDUHeat->GenerateHeat(8.9); //Need to check these values, no source
		}
		else
		{
			DCPower.DrawPower(200.0); //Power on IMU OPR breaker in LM-8
			//total heat load on breaker 78.6W
			if (IMUHeat)
				IMUHeat->GenerateHeat(10.0); //guess using CSM databook
			if (PTAHeat)
				PTAHeat->GenerateHeat(24.0); //guess using CSM databook
			if (PSAHeat)
				PSAHeat->GenerateHeat(26.0); //guess using CSM databook
			if (CDUHeat)
				CDUHeat->GenerateHeat(28.6); //guess using CSM databook
		}
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
	
	// Gyro pulses to CDU pulses
	pulses = (int)(((double)radToGyroPulses(Gimbals[index])) / 64.0);	
	agc.ProcessIMUCDUReadCount(RegCDU, (pulses & 077777));

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
	// transformation to navigation base coordinates
	// CAUTION: gimbal angles are left-handed
	MATRIX3 t = getRotationMatrixY(-Gimbal.Y);
	t = mul(getRotationMatrixZ(-Gimbal.Z), t);
	t = mul(getRotationMatrixX(-Gimbal.X), t);
	
	// tranform to orbiter local coordinates
	t = mul(getNavigationBaseToOrbiterLocalTransformation(), t);
	
	// tranform to orbiter global coordinates
	t = mul(Orbiter.Attitude_v2g, t);

	// "Orbiter's REFSMMAT"
	Orbiter.AttitudeReference = t;
}

void IMU::DoZeroIMUCDUs() 

{
	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;
}

void IMU::DoZeroIMUGimbals()

{
	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;
	SetOrbiterAttitudeReference();
}

VECTOR3 IMU::GetTotalAttitude() 

{
	VECTOR3 v;
	v.x = Gimbal.X;
	v.y = Gimbal.Y;
	v.z = Gimbal.Z;
	return v;
}

double IMU::GetPIPATempF()
{
	if (IMUCase) return KelvinToFahrenheit(IMUCase->GetTemp());
	
	return 130.0;
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
		else if (!strnicmp (line, "LSDT", 4)) {
			sscanf(line + 4, "%lf", &flt);
			LastSimDT = flt;
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

void IMU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IMU_START_STRING);

	papiWriteScenario_double(scn, "RPX", RemainingPIPA.X);
	papiWriteScenario_double(scn, "RPY", RemainingPIPA.Y);
	papiWriteScenario_double(scn, "RPZ", RemainingPIPA.Z);
	papiWriteScenario_double(scn, "GMX", Gimbal.X);
	papiWriteScenario_double(scn, "GMY", Gimbal.Y);
	papiWriteScenario_double(scn, "GMZ", Gimbal.Z);
	papiWriteScenario_double(scn, "M11", Orbiter.AttitudeReference.m11);
	papiWriteScenario_double(scn, "M12", Orbiter.AttitudeReference.m12);
	papiWriteScenario_double(scn, "M13", Orbiter.AttitudeReference.m13);
	papiWriteScenario_double(scn, "M21", Orbiter.AttitudeReference.m21);
	papiWriteScenario_double(scn, "M22", Orbiter.AttitudeReference.m22);
	papiWriteScenario_double(scn, "M23", Orbiter.AttitudeReference.m23);
	papiWriteScenario_double(scn, "M31", Orbiter.AttitudeReference.m31);
	papiWriteScenario_double(scn, "M32", Orbiter.AttitudeReference.m32);
	papiWriteScenario_double(scn, "M33", Orbiter.AttitudeReference.m33);
	papiWriteScenario_double(scn, "LSDT", LastSimDT);

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
