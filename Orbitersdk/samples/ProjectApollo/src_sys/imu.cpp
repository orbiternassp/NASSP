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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"

#include "ioChannels.h"
#include "IMU.h"
#include "yaAGC/agc_engine.h"

#include "toggleswitch.h"
#include "saturn.h"
#include "tracer.h"
#include "papi.h"


PIPA::PIPA(ApolloGuidance &comp) : agc(comp) {

}

void PIPA::SetVessel(bool flgCSM, int Idx)
{
	idx = Idx;

	//These dynamical parameters of PIPA are from Apollo 15 Delco
	double J = 14.0;            // dyne cm/(rad/sec2), PIP float inertia
	double C = 120000.0;        // dyne cm/(rad/sec),  PIP viscous damping
	double P = 0.25;            // dyne cm/(cm/sec2),  Pendulosity
	double t = 1.0 / 3200.0;    // sec, Pulse interval

	double sampshift = 0.639;   // relative samp lag, it is not given in specs,
	                            // simply tuned to make PIPA work in 0g with 
	                            // 3:3 moding
	                            
	double M0;                  // dyne cm, Torque to balance
	if(flgCSM)
		M0 = 4680.0;     
	else
		M0 = 800.0; 

	double tsamp = t*(1.0 - sampshift);
	double tau = J / C;
	double expt=exp(-t/tau), exptsamp = exp(-tsamp / tau);
	
	coeff_phi_pulse = (t - tau*(1.0 - expt))/C;
	coeff_phi_accel = -100.0*P*coeff_phi_pulse;  // We multiply with 100 to get accel i units of m/sec2 instead of cm/sec2
	coeff_phi_pulse = M0*coeff_phi_pulse;
	coeff_phi_prevdot = tau*(1.0 - expt);

	coeff_phi_samp_pulse = (tsamp - tau*(1.0 - exptsamp)) / C;
	coeff_phi_samp_accel = -100.0*P*coeff_phi_samp_pulse;
	coeff_phi_samp_pulse = M0*coeff_phi_samp_pulse;
	coeff_phi_samp_prevdot = tau*(1.0 - exptsamp);

	coeff_phidot_pulse = (1.0 - expt) / C;
	coeff_phidot_accel = -100.0*P*coeff_phidot_pulse;
	coeff_phidot_pulse = M0*coeff_phidot_pulse;
	coeff_phidot_prevdot = expt;

	coeff_phidot_samp_pulse = (1.0 - exptsamp) / C;
	coeff_phidot_samp_accel = -100.0*P*coeff_phidot_samp_pulse;
	coeff_phidot_samp_pulse = M0*coeff_phidot_samp_pulse;
	coeff_phidot_samp_prevdot = exptsamp;

	phi = 0.0;
	phidot = 0.001;
	flipflop = 1;
	precounter = 0;
	setAccel(0.0);

	//char log[1024];
	//sprintf(log, "PIPA%d CSM:%d coeff phi: %.10lf %.10lf %.10lf", idx, flgCSM? 1:0, coeff_phi_pulse, coeff_phi_accel, coeff_phi_prevdot);
	//oapiWriteLog(log);
	//sprintf(log, "PIPA%d CSM:%d coeff phisamp: %.10lf %.10lf %.10lf", idx, flgCSM ? 1 : 0, coeff_phi_samp_pulse, coeff_phi_samp_accel, coeff_phi_samp_prevdot);
	//oapiWriteLog(log);
	//sprintf(log, "PIPA%d CSM:%d coeff phidot: %.10lf %.10lf %.10lf", idx, flgCSM ?1:0, coeff_phidot_pulse, coeff_phidot_accel, coeff_phidot_prevdot);
	//oapiWriteLog(log);
	//sprintf(log, "PIPA%d CSM:%d coeff phidotsamp: %.10lf %.10lf %.10lf", idx, flgCSM ? 1 : 0, coeff_phidot_samp_pulse, coeff_phidot_samp_accel, coeff_phidot_samp_prevdot);
	//oapiWriteLog(log);
}

void PIPA::setAccel(double Accel)
{
	accel = Accel;
	phi_accel = accel*coeff_phi_accel;
	phidot_accel = accel*coeff_phidot_accel;
	phi_samp_accel = accel*coeff_phi_samp_accel;
	phidot_samp_accel= accel*coeff_phidot_samp_accel;
}

void PIPA::step3200pps()
{
	double oldphi = phi, oldphidot = phidot;
	double phi_samp, phidot_samp;
	if (flipflop==1) {
		phi = oldphi+ coeff_phi_prevdot*oldphidot+coeff_phi_pulse + phi_accel;
		phidot = coeff_phidot_prevdot*oldphidot + coeff_phidot_pulse + phidot_accel;
		phi_samp = oldphi + coeff_phi_samp_prevdot*oldphidot + coeff_phi_samp_pulse + phi_samp_accel;
		phidot_samp = coeff_phidot_samp_prevdot*oldphidot + coeff_phidot_samp_pulse + phidot_samp_accel;
	}
	else {
		phi = oldphi + coeff_phi_prevdot*oldphidot - coeff_phi_pulse + phi_accel;
		phidot = coeff_phidot_prevdot*oldphidot - coeff_phidot_pulse + phidot_accel;
		phi_samp = oldphi + coeff_phi_samp_prevdot*oldphidot - coeff_phi_samp_pulse + phi_samp_accel;
		phidot_samp = coeff_phidot_samp_prevdot*oldphidot - coeff_phidot_samp_pulse + phidot_samp_accel;
	}
	if (phi_samp >= 0.0) {
		flipflop = -1;
		if (precounter > 0)
			precounter--;
		else
			setCntrReqM(&agc.vagc, idx + 013);
	}
	else {
		flipflop = 1;
		if (precounter < 3)
			precounter++;
		else
			setCntrReqP(&agc.vagc, idx + 013);
	}
}
void PIPA::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_double(scn, "  ACCEL", accel);
	papiWriteScenario_double(scn, "  PENDULANGLE", phi);
	papiWriteScenario_double(scn, "  PENDULANGLEDOT", phidot);
	oapiWriteScenario_int(scn, "  FLIPFLOP", flipflop);
	oapiWriteScenario_int(scn, "  PRECOUNTER", precounter);

	oapiWriteLine(scn, end_str);
}

void PIPA::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		double a;
		papiReadScenario_double(line, "ACCEL", a);
		setAccel(a);
		papiReadScenario_double(line, "PENDULANGLE", phi);
		papiReadScenario_double(line, "PENDULANGLEDOT", phidot);
		papiReadScenario_int(line, "FLIPFLOP", flipflop);
		papiReadScenario_int(line, "PRECOUNTER", precounter);
	}
}

IMU::IMU(ApolloGuidance & comp, PanelSDK &p) :
	agc(comp),
	DCPower(0, p),
	DCHeaterPower(0, p),
	pipax(comp), 
	pipay(comp),
	pipaz(comp)

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
	
	/*RemainingPIPA.X = 0;
	RemainingPIPA.Y = 0;
	RemainingPIPA.Z = 0;*/

	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;

	Orbiter.Attitude.X = 0;
	Orbiter.Attitude.Y = 0;
	Orbiter.Attitude.Z = 0;

	Orbiter.AttitudeReference.m11 = 0;
	Orbiter.AttitudeReference.m12 = 0;
	Orbiter.AttitudeReference.m13 = 0;
	Orbiter.AttitudeReference.m21 = 0;
	Orbiter.AttitudeReference.m22 = 0;
	Orbiter.AttitudeReference.m23 = 0;
	Orbiter.AttitudeReference.m31 = 0;
	Orbiter.AttitudeReference.m32 = 0;
	Orbiter.AttitudeReference.m33 = 0;

	LastWeightAcceleration = _V(0, 0, 0);
	LastGlobalVel = _V(0, 0, 0);

	OurVessel = 0;
	IMUHeater = 0;
	IMUHeat = 0;
	IMUCase = 0;
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
	/*if ( LEM ) 
		pipaRate = 0.01;	// LM: 1 pulse = 1 cm/s
	else
		pipaRate = 0.0585; // CSM: 1 pulse = 5.85 cm/s*/
	pipax.SetVessel(!LEMFlag, 0);
	pipay.SetVessel(!LEMFlag, 1);
	pipaz.SetVessel(!LEMFlag, 2);
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
		agc.SetInputChannelBit(030, IMUOperate, true);
		agc.SetInputChannelBit(030, ISSTurnOnRequest, true);
		// Clear IMUFail
		agc.SetInputChannelBit(030, IMUFail, false);

		agc.ogcdu.UpdateAngle();
		agc.mgcdu.UpdateAngle();
		agc.igcdu.UpdateAngle();
		agc.ogcdu.flg_enable_drive = false;
		agc.mgcdu.flg_enable_drive = false;
		agc.igcdu.flg_enable_drive = false;

		Operate = true;
	}
}

void IMU::TurnOff() 

{
	if (Operate) {
		agc.SetInputChannelBit(030, IMUOperate, false);
		agc.SetInputChannelBit(030, ISSTurnOnRequest, false);

		// The IMU is monitored by a separate "IMU Fail Detect Logic",
		// which sets the IMUFail and IMUCDUFail bits of channel 030
		// under certain conditions, see CSM systems handbook 8.1 H9.
		// For now we just raise an IMUFail in case of an turn off.
		agc.SetInputChannelBit(030, IMUFail, true);

		agc.ogcdu.flg_enable_drive = false;
		agc.mgcdu.flg_enable_drive = false;
		agc.igcdu.flg_enable_drive = false;

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
	
#ifdef _DEBUG
	if (address != 07 && address != 033 /*&& address != 010*/) {  	  
    	LogState(address, "out", value.to_ulong());
	}
#endif

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
    
    	/*if (val12[ZeroIMUCDUs]) {
			DoZeroIMUCDUs();
			agc.ProcessIMUCDUReadCount(RegCDUX, 0);
			agc.ProcessIMUCDUReadCount(RegCDUY, 0);
			agc.ProcessIMUCDUReadCount(RegCDUZ, 0);
		}*/

		// coarse align 
		if (val12[CoarseAlignEnable]) {
			// 	if (address == 0174) {
			// 		DriveCDUX(value.to_ulong());
			//	}
			// 	if (address == 0175) {
			// 		DriveCDUY(value.to_ulong());
			// 	}
			// 	if (address == 0176) {
			// 		DriveCDUZ(value.to_ulong());
			// 	}
			agc.ogcdu.flg_enable_drive = true;
			agc.mgcdu.flg_enable_drive = true;
			agc.igcdu.flg_enable_drive = true;
		}
		else {
			agc.ogcdu.flg_enable_drive = false;
			agc.mgcdu.flg_enable_drive = false;
			agc.igcdu.flg_enable_drive = false;
		}
	}
    	 
	if (!TurnedOn) {
		return;
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

void IMU::InitThermals(h_HeatLoad *heat, h_Radiator *cas)
{
	IMUHeat = heat;
	IMUCase = cas;
}

void IMU::Timestep(double simdt) 

{
	TRACESETUP("IMU::Timestep");

	// double pulses;
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

	//sprintf(oapiDebugString(), "Simdt:%lf Operate:%s Powered:%s TurnedOn:%s", simdt, Operate?"T":"F", IsPowered() ? "T" : "F", TurnedOn ? "T" : "F");
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
	VECTOR3 arot;
	OurVessel->GetGlobalOrientation(arot);

	Orbiter.Attitude.X = arot.x;
	Orbiter.Attitude.Y = arot.y;
	Orbiter.Attitude.Z = arot.z;

	// Vessel to Orbiter global transformation
	MATRIX3	tinv = getRotationMatrixZ(-arot.z);
	tinv = mul(getRotationMatrixY(-arot.y), tinv);
	tinv = mul(getRotationMatrixX(-arot.x), tinv);


	if (!Initialized) {
		SetOrbiterAttitudeReference();

		// Get current weight vector in vessel coordinates
		VECTOR3 w;
		OurVessel->GetWeightVector(w);
		// Transform to Orbiter global and calculate weight acceleration
		w = mul(tinv, w) / OurVessel->GetMass();

		//Orbiter 2016 hack
		if (length(w) == 0.0)
		{
			w = GetGravityVector();
		}

		LastWeightAcceleration = w;

		OurVessel->GetGlobalVel(LastGlobalVel);

		LastSimDT = simdt;
		Initialized = true;
	} 
	else {
		// Calculate accelerations
		VECTOR3 w, vel;
		OurVessel->GetWeightVector(w);
		// Transform to Orbiter global and calculate accelerations
		w = mul(tinv, w) / OurVessel->GetMass();

		//Orbiter 2016 hack
		if (length(w) == 0.0)
		{
			w = GetGravityVector();
		}
		OurVessel->GetGlobalVel(vel);
		VECTOR3 dvel = (vel - LastGlobalVel) / LastSimDT;

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
		//imuState->Orbiter.Y = imuState->Orbiter.Y + (deltaTime * TwoPI / 86164.09);

		// Process channel bits				
		val12 = agc.GetOutputChannel(012);
		// if (val12[ZeroIMUCDUs]) {
		// 	DoZeroIMUCDUs();
		// }
		// else 
		if (val12[CoarseAlignEnable]) {
			TRACE("CHANNEL 12 COARSE");

			// Force gimbals into [0, 2Pi[ interval as cdu.cpp
			// doesn't handles this during coarse alignment
			for (int index = 0; index < 3; index++){ 
				if (Gimbals[index] >= TWO_PI) {
					Gimbals[index] -= TWO_PI;
				}
				if (Gimbals[index] < 0) {
					Gimbals[index] += TWO_PI;
				}
			}
			SetOrbiterAttitudeReference();
		}
		else if (Caged) {
			SetOrbiterAttitudeReference();
		}
		else {

			TRACE("CHANNEL 12 NORMAL");
			// sprintf(oapiDebugString(), "GimbX:%6.2lf OCDUrc:%5d RegCDUX:%5d c12:%05o",
			// 	Gimbal.X,
			// 	agc.ogcdu.GetReadCounter(),
			// 	(int)agc.vagc.Erasable[0][RegCDUX],
			// 	agc.GetOutputChannel(012));

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
			//sprintf(oapiDebugString(), "accel x %.10f y %.10f z %.10f l %.10f", accel.x, accel.y, accel.z, length(accel));								
			pipax.setAccel(accel.x);
			pipay.setAccel(accel.y);
			pipaz.setAccel(accel.z);
			// pulse PIPAs
			/*pulses = RemainingPIPA.X + (accel.x * LastSimDT / pipaRate);
			PulsePIPA(RegPIPAX, (int) pulses);
			RemainingPIPA.X = pulses - (int) pulses;

			pulses = RemainingPIPA.Y + (accel.y * LastSimDT / pipaRate);
			PulsePIPA(RegPIPAY, (int) pulses);
			RemainingPIPA.Y = pulses - (int) pulses;

			pulses = RemainingPIPA.Z + (accel.z * LastSimDT / pipaRate);
			PulsePIPA(RegPIPAZ, (int) pulses);
			RemainingPIPA.Z = pulses - (int) pulses;			
			*/

			//sprintf(oapiDebugString(), "RegPIPAX: %hu RegPIPAY: %hu RegPIPAZ: %hu accel.x=%lf", agc.vagc.Erasable[0][037], agc.vagc.Erasable[0][040], agc.vagc.Erasable[0][041], accel.x);
		}
		LastSimDT = simdt;
	}
}

void IMU::SystemTimestep(double simdt) 

{
	if (Operate) {
		if (Caged)
		{
			DCPower.DrawPower(61.7);
			if (IMUHeat)
				IMUHeat->GenerateHeat(14.9);
		}
		else
		{
			DCPower.DrawPower(325.0);
			if (IMUHeat)
				IMUHeat->GenerateHeat(78.6);
		}
	}
}

void IMU::step3200pps()
{
	pipax.step3200pps();
	pipay.step3200pps();
	pipaz.step3200pps();
}

void IMU::PulsePIPA(int RegPIPA, int pulses) 

{
	agc.PulsePIPA(RegPIPA, pulses);
}

#if 0
void IMU::DriveGimbals(double x, double y, double z) 

{
	DriveGimbal(0, RegCDUX, x - Gimbal.X);
	DriveGimbal(1, RegCDUY, y - Gimbal.Y);
	DriveGimbal(2, RegCDUZ, z - Gimbal.Z);
	
	SetOrbiterAttitudeReference();
}
#endif

void IMU::DriveGimbalX(double angle) 

{
	DriveGimbal(0, RegCDUX, angle);
	agc.ogcdu.UpdateAngle();
}

void IMU::DriveGimbalY(double angle) 

{
	DriveGimbal(1, RegCDUY, angle);
	agc.mgcdu.UpdateAngle();
}

void IMU::DriveGimbalZ(double angle) 

{
	DriveGimbal(2, RegCDUZ, angle);
	agc.igcdu.UpdateAngle();
}

void IMU::DriveGimbal(int index, int RegCDU, double angle) 

{
	TRACESETUP("DRIVE GIMBAL");
  	// int  pulses; //i, delta;
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
	// pulses = (int)(((double)radToGyroPulses(Gimbals[index])) / 64.0);	
	// agc.ProcessIMUCDUReadCount(RegCDU, (pulses & 077777));

	// char buffers[80];
	// sprintf(buffers,"DRIVE GIMBAL index %o REGCDU %o angle %f pulses %o",index,RegCDU,angle,pulses);
	// if (pulses)
	// 	TRACE(buffers);
}

#if 0
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
#endif


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
	t = mul(getRotationMatrixZ(-Orbiter.Attitude.Z), t);
	t = mul(getRotationMatrixY(-Orbiter.Attitude.Y), t);
	t = mul(getRotationMatrixX(-Orbiter.Attitude.X), t);

	// "Orbiter's REFSMMAT"
	Orbiter.AttitudeReference = t;
}

// void IMU::DoZeroIMUCDUs() 
// 
// {
// 	Gimbal.X = 0;
// 	Gimbal.Y = 0;
// 	Gimbal.Z = 0;
// }

void IMU::DoZeroIMUGimbals()

{
	Gimbal.X = 0;
	Gimbal.Y = 0;
	Gimbal.Z = 0;
	// agc.ogcdu.UpdateAngle();
	// agc.mgcdu.UpdateAngle();
	// agc.igcdu.UpdateAngle();

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

VECTOR3 IMU::GetGravityVector()
{
	OBJHANDLE gravref = OurVessel->GetGravityRef();
	OBJHANDLE hSun = oapiGetObjectByName("Sun");
	VECTOR3 R, U_R;
	OurVessel->GetRelativePos(gravref, R);
	U_R = unit(R);
	double r = length(R);
	VECTOR3 R_S, U_R_S;
	OurVessel->GetRelativePos(hSun, R_S);
	U_R_S = unit(R_S);
	double r_S = length(R_S);
	double mu = GGRAV * oapiGetMass(gravref);
	double mu_S = GGRAV * oapiGetMass(hSun);
	int jcount = oapiGetPlanetJCoeffCount(gravref);
	double JCoeff[5];
	for (int i = 0; i < jcount; i++)
	{
		JCoeff[i] = oapiGetPlanetJCoeff(gravref, i);
	}
	double R_E = oapiGetSize(gravref);

	VECTOR3 a_dP;

	a_dP = -U_R;

	if (jcount > 0)
	{
		MATRIX3 mat;
		VECTOR3 U_Z;
		double costheta, P2, P3;

		oapiGetPlanetObliquityMatrix(gravref, &mat);
		U_Z = mul(mat, _V(0, 1, 0));

		costheta = dotp(U_R, U_Z);

		P2 = 3.0 * costheta;
		P3 = 0.5*(15.0*costheta*costheta - 3.0);
		a_dP += (U_R*P3 - U_Z * P2)*JCoeff[0] * pow(R_E / r, 2.0);
		if (jcount > 1)
		{
			double P4;
			P4 = 1.0 / 3.0*(7.0*costheta*P3 - 4.0*P2);
			a_dP += (U_R*P4 - U_Z * P3)*JCoeff[1] * pow(R_E / r, 3.0);
			if (jcount > 2)
			{
				double P5;
				P5 = 0.25*(9.0*costheta*P4 - 5.0 * P3);
				a_dP += (U_R*P5 - U_Z * P4)*JCoeff[2] * pow(R_E / r, 4.0);
			}
		}
	}
	a_dP *= mu / pow(r, 2.0);
	a_dP -= U_R_S * mu_S / pow(r_S, 2.0);

	if (gravref == oapiGetObjectByName("Moon"))
	{
		OBJHANDLE hEarth = oapiGetObjectByName("Earth");

		VECTOR3 R_Ea, U_R_E;
		OurVessel->GetRelativePos(hEarth, R_Ea);
		U_R_E = unit(R_Ea);
		double r_E = length(R_Ea);
		double mu_E = GGRAV * oapiGetMass(hEarth);

		a_dP -= U_R_E * mu_E / pow(r_E, 2.0);
	}

	return a_dP;
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
		/*if (!strnicmp (line, "RPX", 3)) {
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
		else*/
		if (!strnicmp (line, "GMX", 3)) {
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
		else if (!strnicmp (line, "WLX", 3)) {
			sscanf(line + 3, "%lf", &flt);
			LastWeightAcceleration.x = flt;
		}
		else if (!strnicmp (line, "WLY", 3)) {
			sscanf(line + 3, "%lf", &flt);
			LastWeightAcceleration.y = flt;
		}
		else if (!strnicmp (line, "WLZ", 3)) {
			sscanf(line + 3, "%lf", &flt);
			LastWeightAcceleration.z = flt;
		}
		else if (!strnicmp (line, "VLX", 3)) {
			sscanf(line + 3, "%lf", &flt);
			LastGlobalVel.x = flt;
		}
		else if (!strnicmp (line, "VLY", 3)) {
			sscanf(line + 3, "%lf", &flt);
			LastGlobalVel.y = flt;
		}
		else if (!strnicmp (line, "VLZ", 3)) {
			sscanf(line + 3, "%lf", &flt);
			LastGlobalVel.z = flt;
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
		else if (!strnicmp(line, "PIPAX_START", sizeof("PIPAX_START"))) {
			pipax.LoadState(scn, "PIPA_END");
		}
		else if (!strnicmp(line, "PIPAY_START", sizeof("PIPAY_START"))) {
			pipay.LoadState(scn, "PIPA_END");
		}
		else if (!strnicmp(line, "PIPAZ_START", sizeof("PIPAZ_START"))) {
			pipaz.LoadState(scn, "PIPA_END");
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

	/*papiWriteScenario_double(scn, "RPX", RemainingPIPA.X);
	papiWriteScenario_double(scn, "RPY", RemainingPIPA.Y);
	papiWriteScenario_double(scn, "RPZ", RemainingPIPA.Z);*/
	papiWriteScenario_double(scn, "GMX", Gimbal.X);
	papiWriteScenario_double(scn, "GMY", Gimbal.Y);
	papiWriteScenario_double(scn, "GMZ", Gimbal.Z);
	papiWriteScenario_double(scn, "OAX", Orbiter.Attitude.X);
	papiWriteScenario_double(scn, "OAY", Orbiter.Attitude.Y);
	papiWriteScenario_double(scn, "OAZ", Orbiter.Attitude.Z);
	papiWriteScenario_double(scn, "WLX", LastWeightAcceleration.x);
	papiWriteScenario_double(scn, "WLY", LastWeightAcceleration.y);
	papiWriteScenario_double(scn, "WLZ", LastWeightAcceleration.z);
	papiWriteScenario_double(scn, "VLX", LastGlobalVel.x);
	papiWriteScenario_double(scn, "VLY", LastGlobalVel.y);
	papiWriteScenario_double(scn, "VLZ", LastGlobalVel.z);
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
	
	pipax.SaveState(scn, "  PIPAX_START", "  PIPA_END");
	pipay.SaveState(scn, "  PIPAY_START", "  PIPA_END");
	pipaz.SaveState(scn, "  PIPAZ_START", "  PIPA_END");

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
