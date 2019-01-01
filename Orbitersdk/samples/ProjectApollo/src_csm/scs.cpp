/****************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2006 dseagrav

  SCS simulation

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
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "saturn.h"
#include "tracer.h"
#include "papi.h"

#include <time.h>

//#include "afxdlgs.h"  // This header allows file write dialog for Scroll output...  HACKED.


AttitudeReference::AttitudeReference() {

	AttitudeInitialized = false;
	Vessel = NULL;
	Attitude = _V(0,0,0);
	LastAttitude = _V(0,0,0);
	OrbiterAttitude.Attitude = _V(0,0,0);

	int i;
	for (i = 0; i < 9; i++)
		OrbiterAttitude.AttitudeReference.data[i] = 0.0;
}

void AttitudeReference::Init(VESSEL *v) {

	Vessel = v;
}

void AttitudeReference::Timestep(double simdt) {

	LastAttitude = Attitude;	

	if (Vessel == NULL) return;
	// Get vessel status
	VECTOR3 arot;
	Vessel->GetGlobalOrientation(arot);

	// Get eccliptic-plane attitude
	OrbiterAttitude.Attitude.x = arot.x;
	OrbiterAttitude.Attitude.y = arot.y;
	OrbiterAttitude.Attitude.z = arot.z;

	if (!AttitudeInitialized) {
		// Reset		
		SetOrbiterAttitudeReference();
		AttitudeInitialized = true;

	} else { 
		// Gimbals
		MATRIX3 t = OrbiterAttitude.AttitudeReference;
		t = mul(GetRotationMatrixX(OrbiterAttitude.Attitude.x), t);
		t = mul(GetRotationMatrixY(OrbiterAttitude.Attitude.y), t);
		t = mul(GetRotationMatrixZ(OrbiterAttitude.Attitude.z), t);
	  	
	  	t = mul(GetOrbiterLocalToNavigationBaseTransformation(), t);
	  	
		// calculate the new gimbal angles
		VECTOR3 newAngles = GetRotationAnglesXZY(t);

		// Correct new angles
		double OldGimbal = Attitude.x;
		Attitude.x += (-newAngles.x - Attitude.x);
		if (Attitude.x >= TWO_PI) {
			Attitude.x -= TWO_PI;
		}
		if (Attitude.x < 0) {
			Attitude.x += TWO_PI;
		}		
		OldGimbal = Attitude.y;
		Attitude.y += (-newAngles.y - Attitude.y);
		if (Attitude.y >= TWO_PI) {
			Attitude.y -= TWO_PI;
		}
		if (Attitude.y < 0) {
			Attitude.y += TWO_PI;
		}
		OldGimbal = Attitude.z;
		Attitude.z += (-newAngles.z - Attitude.z);
		if (Attitude.z >= TWO_PI) {
			Attitude.z -= TWO_PI;
		}
		if (Attitude.z < 0) {
			Attitude.z += TWO_PI;
		}
	}
}

void AttitudeReference::SetAttitude(VECTOR3 a) {

	Attitude = a;
	SetOrbiterAttitudeReference();
}

void AttitudeReference::SetOrbiterAttitudeReference() {
	
	MATRIX3 t;

	// transformation to navigation base coordinates
	// CAUTION: gimbal angles are left-handed
	t = GetRotationMatrixY(-Attitude.y);
	t = mul(GetRotationMatrixZ(-Attitude.z), t);
	t = mul(GetRotationMatrixX(-Attitude.x), t);
	
	// tranform to orbiter local coordinates
	t = mul(GetNavigationBaseToOrbiterLocalTransformation(), t);
	
	// tranform to orbiter global coordinates
	t = mul(GetRotationMatrixZ(-OrbiterAttitude.Attitude.z), t);
	t = mul(GetRotationMatrixY(-OrbiterAttitude.Attitude.y), t);
	t = mul(GetRotationMatrixX(-OrbiterAttitude.Attitude.x), t);

	OrbiterAttitude.AttitudeReference = t;
}

void AttitudeReference::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int(scn, "ATTITUDEINITIALIZED", AttitudeInitialized);

	papiWriteScenario_double(scn, "ATTITUDEX", Attitude.x);
	papiWriteScenario_double(scn, "ATTITUDEY", Attitude.y);
	papiWriteScenario_double(scn, "ATTITUDEZ", Attitude.z);
	papiWriteScenario_double(scn, "LASTATTITUDEX", LastAttitude.x);
	papiWriteScenario_double(scn, "LASTATTITUDEY", LastAttitude.y);
	papiWriteScenario_double(scn, "LASTATTITUDEZ", LastAttitude.z);
	papiWriteScenario_double(scn, "ORBITERATTITUDEX", OrbiterAttitude.Attitude.x);
	papiWriteScenario_double(scn, "ORBITERATTITUDEY", OrbiterAttitude.Attitude.y);
	papiWriteScenario_double(scn, "ORBITERATTITUDEZ", OrbiterAttitude.Attitude.z);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM11", OrbiterAttitude.AttitudeReference.m11);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM12", OrbiterAttitude.AttitudeReference.m12);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM13", OrbiterAttitude.AttitudeReference.m13);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM21", OrbiterAttitude.AttitudeReference.m21);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM22", OrbiterAttitude.AttitudeReference.m22);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM23", OrbiterAttitude.AttitudeReference.m23);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM31", OrbiterAttitude.AttitudeReference.m31);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM32", OrbiterAttitude.AttitudeReference.m32);
	papiWriteScenario_double(scn, "ORBITERATTITUDEM33", OrbiterAttitude.AttitudeReference.m33);
}

void AttitudeReference::LoadState(char *line) {

	if (!strnicmp (line, "ATTITUDEINITIALIZED", 19)) {
		sscanf(line + 19, "%i", (int *)&AttitudeInitialized);
	}
	else if (!strnicmp (line, "ATTITUDEX", 9)) {
		sscanf(line + 9, "%lf", &Attitude.x);
	}
	else if (!strnicmp (line, "ATTITUDEY", 9)) {
		sscanf(line + 9, "%lf", &Attitude.y);
	}
	else if (!strnicmp (line, "ATTITUDEZ", 9)) {
		sscanf(line + 9, "%lf", &Attitude.z);
	}
	else if (!strnicmp (line, "LASTATTITUDEX", 13)) {
		sscanf(line + 13, "%lf", &LastAttitude.x);
	}
	else if (!strnicmp (line, "LASTATTITUDEY", 13)) {
		sscanf(line + 13, "%lf", &LastAttitude.y);
	}
	else if (!strnicmp (line, "LASTATTITUDEZ", 13)) {
		sscanf(line + 13, "%lf", &LastAttitude.z);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEX", 16)) {
		sscanf(line + 16, "%lf", &OrbiterAttitude.Attitude.x);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEY", 16)) {
		sscanf(line + 16, "%lf", &OrbiterAttitude.Attitude.y);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEZ", 16)) {
		sscanf(line + 16, "%lf", &OrbiterAttitude.Attitude.z);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM11", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m11);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM12", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m12);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM13", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m13);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM21", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m21);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM22", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m22);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM23", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m23);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM31", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m31);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM32", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m32);
	}
	else if (!strnicmp (line, "ORBITERATTITUDEM33", 18)) {
		sscanf(line + 18, "%lf", &OrbiterAttitude.AttitudeReference.m33);
	}
}

MATRIX3 AttitudeReference::GetNavigationBaseToOrbiterLocalTransformation() {

	MATRIX3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	m.m12 = 1.0;	
	m.m23 = -1.0;
	m.m31 = 1.0;

	return m;
} 

MATRIX3 AttitudeReference::GetOrbiterLocalToNavigationBaseTransformation() {

	MATRIX3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	m.m13 = 1.0;
	m.m21 = 1.0;	
	m.m32 = -1.0;

	return m;
}

VECTOR3 AttitudeReference::GetRotationAnglesXZY(MATRIX3 m) {
	
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

MATRIX3 AttitudeReference::GetRotationMatrixX(double angle) {
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

MATRIX3 AttitudeReference::GetRotationMatrixY(double angle) {
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

MATRIX3 AttitudeReference::GetRotationMatrixZ(double angle) {
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


// BMAG

BMAG::BMAG() {

	sat = NULL;
	powered = FALSE;
	dc_source = NULL;
	ac_source = NULL;
	dc_bus = NULL;
	ac_bus = NULL;
	temperature=0;
	rates = _V(0,0,0);
	uncaged = _V(0,0,0);
	errorAttitude = _V(0,0,0);
	number = 0;
}

void BMAG::Init(int n, Saturn *v, e_object *dcbus, e_object *acbus, Boiler *h) {
	// Initialize
	number = n;
	sat = v;
	dc_bus = dcbus;
	ac_bus = acbus;
	heater = h;
	temperature = 349.817;
}

void BMAG::Timestep(double simdt) {
	// For right now, simply get rates if both the heater and the gyro are powered.
	// Later, we should consider temperature and spin-up time and such.
	
	powered = false; // No data
	rates = _V(0,0,0);
	if (sat != NULL) {
		if (dc_source != NULL && dc_source->Voltage() > SP_MIN_DCVOLTAGE) {
			if (ac_source != NULL && ac_source->Voltage() > SP_MIN_ACVOLTAGE) {
				powered = true;
				sat->GetAngularVel(rates); // From those, generate ROTATION RATE data.
			}
		}
	}
	
	// If unpowered cage BMAG so that we loose reference for attitude hold
	if (!powered) {
		uncaged = _V(0, 0, 0);
		errorAttitude = _V(0, 0, 0);
	}

	heater->WireTo(dc_source);	// Take DC power to heat the gyro

	if (uncaged.x == 1) {
		errorAttitude.x -= rates.z * simdt;
		while (errorAttitude.x <= TWO_PI) errorAttitude.x += TWO_PI;
		while (errorAttitude.x >= TWO_PI) errorAttitude.x -= TWO_PI;
	}
	if (uncaged.y == 1) {
		errorAttitude.y -= rates.x * simdt;
		while (errorAttitude.y <= TWO_PI) errorAttitude.y += TWO_PI;
		while (errorAttitude.y >= TWO_PI) errorAttitude.y -= TWO_PI;
	}
	if (uncaged.z == 1) {
		errorAttitude.z += rates.y * simdt;
		while (errorAttitude.z <= TWO_PI) errorAttitude.z += TWO_PI;
		while (errorAttitude.z >= TWO_PI) errorAttitude.z -= TWO_PI;
	}
}

void BMAG::SystemTimestep(double simdt) {

	if (powered && ac_source)
		ac_source->DrawPower(8.7);  // take AC power to spin the gyro
}

void BMAG::Cage(int axis) {

	uncaged.data[axis] = 0;
	errorAttitude.data[axis] = 0;
}

void BMAG::Uncage(int axis) {

	// Uncage only if powered
	if (!powered) return;

	// If caged store current attitude as reference for errors
	if (uncaged.data[axis] == 0) {
		uncaged.data[axis] = 1;

		errorAttitude.data[axis] = 0;
	}
}

VECTOR3 BMAG::GetAttitudeError()
{
	return errorAttitude;
}

void BMAG::SetPower(bool dc, bool ac) {

	if (dc) 
		dc_source = dc_bus;
	else	
		dc_source = NULL;

	if (ac) 
		ac_source = ac_bus;
	else	
		ac_source = NULL;
}

double BMAG::GetTempF() {

	if (!sat) return 0;

	return KelvinToFahrenheit(temperature);
}

void BMAG::SaveState(FILEHANDLE scn) {

	// START_STRING is written in Saturn
	papiWriteScenario_double(scn, "UNCAGEDX", uncaged.x);
	papiWriteScenario_double(scn, "UNCAGEDY", uncaged.y);
	papiWriteScenario_double(scn, "UNCAGEDZ", uncaged.z);
	papiWriteScenario_double(scn, "RATESX", rates.x);
	papiWriteScenario_double(scn, "RATESY", rates.y);
	papiWriteScenario_double(scn, "RATESZ", rates.z);
	papiWriteScenario_vec(scn, "ERRORATTITUDE", errorAttitude);

	oapiWriteLine(scn, BMAG_END_STRING);
}

void BMAG::LoadState(FILEHANDLE scn){

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, BMAG_END_STRING, sizeof(BMAG_END_STRING))){
			return;
		}

		papiReadScenario_double(line, "UNCAGEDX", uncaged.x);
		papiReadScenario_double(line, "UNCAGEDY", uncaged.y);
		papiReadScenario_double(line, "UNCAGEDZ", uncaged.z);
		papiReadScenario_double(line, "RATESX", rates.x);
		papiReadScenario_double(line, "RATESY", rates.y);
		papiReadScenario_double(line, "RATESZ", rates.z);
		papiReadScenario_vec(line, "ERRORATTITUDE", errorAttitude);
	}
}


// GDC

GDC::GDC()
{
	rates = _V(0,0,0);
	Attitude = _V(0, 0, 0);
	sat = NULL;
	fdai_err_ena = 0;
	fdai_err_x = 0;
	fdai_err_y = 0;
	fdai_err_z = 0;

	A2K1 = false;
	A2K2 = false;
	A2K3 = false;
	A2K4 = false;
	A3K1 = false;
	A3K2 = false;
	A3K3 = false;
	A3K4 = false;
	A4K1 = false;
	A4K2 = false;
	A4K3 = false;
	A4K4 = false;
	A6K1 = false;
	A6K2 = false;
	A8K1 = false;
	A8K2 = false;
	A9K1 = false;
	A9K2 = false;
	A9K3 = false;
}

void GDC::Init(Saturn *v)
{
	sat = v;
}

void GDC::SystemTimestep(double simdt) {

	// Do we have power? 
	/// \todo DC power is needed, too
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2) return;  // Switched off

	if (sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->StabContSystemAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) return;

	sat->StabContSystemAc1CircuitBraker.DrawPower(10.4); 
	sat->StabContSystemAc2CircuitBraker.DrawPower(3.4); 	
}
		
void GDC::Timestep(double simdt) {

	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2)
	{
		E0_505PR = false;
		E0_505Y = false;
		return;
	}

	//POWER
	bool power, ac1power, ac2power, E1_504, E1_503, E1_502, E0_503PR, E0_503Y, E2_503, E2_502, EA_501, EB_501;

	ac1power = sat->StabContSystemAc1CircuitBraker.IsPowered();
	ac2power = sat->StabContSystemAc2CircuitBraker.IsPowered();

	//SCS electronics power switch state is assured above
	if (ac1power)
	{
		E1_502 = true;
		E1_503 = true;
		E1_504 = true;
	}
	else
	{
		E1_502 = false;
		E1_503 = false;
		E1_504 = false;
	}

	if (A8K2)
	{
		power = ac2power;
	}
	else
	{
		if (A8K1)
		{
			power = ac1power;
		}
		else
		{
			power = ac2power;
		}
	}

	if (power)
	{
		E0_503Y = true;
		E0_505Y = true;
	}
	else
	{
		E0_503Y = false;
		E0_505Y = false;
	}

	if (A8K1)
		power = ac1power;
	else
		power = ac2power;

	if (power)
	{
		E0_503PR = true;
		E0_505PR = true;
	}
	else
	{
		E0_503PR = false;
		E0_505PR = false;
	}

	if (ac2power)
	{
		E2_502 = true;
		E2_503 = true;
	}
	else
	{
		E2_502 = false;
		E2_503 = false;
	}

	if (sat->SystemMnACircuitBraker.IsPowered())
		EA_501 = true;
	else
		EA_501 = false;

	if (sat->SystemMnBCircuitBraker.IsPowered())
		EB_501 = true;
	else
		EB_501 = false;

	//SWITCH LOGIC

	//CMC ATT - IMU Enable
	bool S2_1;
	//Att Set - GDC
	bool S6_1;
	//BMAG Roll - Rate 1
	bool S20_3;
	//BMAG Pitch - Rate 1
	bool S21_3;
	//BMAG Yaw - Rate 1
	bool S22_3;
	//GDC Align - Depressed
	bool S37_D;
	//EMS Roll - On
	bool S50_2;
	//Entry 0.05G - On
	bool S51_2;

	//2-1
	if (sat->CMCAttSwitch.IsUp() && (sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE || sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE))
		S2_1 = true;
	else
		S2_1 = false;

	//6-1
	if (sat->FDAIAttSetSwitch.IsDown() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE)
		S6_1 = true;
	else
		S6_1 = false;

	//20-3
	if (sat->BMAGRollSwitch.IsDown() && sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE)
		S20_3 = true;
	else
		S20_3 = false;

	//21-3
	if (sat->BMAGPitchSwitch.IsDown() && sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE)
		S21_3 = true;
	else
		S21_3 = false;

	//22-3
	if (sat->BMAGYawSwitch.IsDown() && sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE)
		S22_3 = true;
	else
		S22_3 = false;

	//37-D
	if (sat->GDCAlignButton.GetState() == 1 && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE)
		S37_D = true;
	else
		S37_D = false;

	//50-2
	if (sat->EMSRollSwitch.IsUp() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE)
		S50_2 = true;
	else
		S50_2 = false;

	//51-2
	if (sat->GSwitch.IsUp() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE)
		S51_2 = true;
	else
		S51_2 = false;

	//Relay logic

	if (EA_501 && S2_1 && !(S37_D || S51_2))
	{
		A2K1 = true;
		A3K1 = true;
	}
	else
	{
		A2K1 = false;
		A3K1 = false;
	}

	if (EB_501 && S2_1 && !(S37_D || S51_2))
		A4K1 = true;
	else
		A4K1 = false;

	if (EA_501 && S51_2 && !S37_D)
	{
		A2K2 = true;
		A3K2 = true;
	}
	else
	{
		A2K2 = false;
		A3K2 = false;
	}

	if (EB_501 && S51_2 && !S37_D)
	{
		A4K2 = true;
		A6K1 = true;
	}
	else
	{
		A4K2 = false;
		A6K1 = false;
	}

	if (S37_D)
	{
		A2K3 = true;
		A3K3 = true;
		A4K3 = true;
	}
	else
	{
		A2K3 = false;
		A3K3 = false;
		A4K3 = false;
	}

	if (S21_3)
		A2K4 = true;
	else
		A2K4 = false;

	if (S20_3)
		A3K4 = true;
	else
		A3K4 = false;

	if (S22_3)
		A4K4 = true;
	else
		A4K4 = false;

	if (S50_2)
	{
		A6K2 = true;
		A8K2 = true;
	}
	else
	{
		A6K2 = false;
		A8K2 = false;
	}

	if (sat->eda.GetGDCResolverExcitation())
		A8K1 = true;
	else
		A8K1 = false;

	if (S6_1)
	{
		A9K1 = true;
		A9K2 = true;
	}
	else
	{
		A9K1 = false;
		A9K2 = false;
	}

	if (S2_1)
		A9K3 = true;
	else
		A9K3 = false;

	// Get rates from the appropriate BMAG
	// GDC attitude is based on RATE data, not ATT data.
	BMAG *pitchBmag = NULL;
	BMAG *primRollBmag = NULL;
	BMAG *redunRollBmag = NULL;
	BMAG *yawBmag = NULL;

	if (A2K4)
		pitchBmag = &sat->bmag1;
	else
		pitchBmag = &sat->bmag2;

	if (A3K4)
		redunRollBmag = &sat->bmag1;
	else
		redunRollBmag = &sat->bmag2;

	if (A3K2)
		primRollBmag = &sat->bmag1;
	else
		primRollBmag = redunRollBmag;

	if (A4K4)
		yawBmag = &sat->bmag1;
	else
		yawBmag = &sat->bmag2;

	//Pitch Voltage to Frequency Converter (Module A2)
	double pitchrate;

	if (A2K1)
	{
		//Euler mode
		if (E1_504 && E2_502)
		{
			pitchrate = pitchBmag->GetRates().x*cos(Attitude.x) + yawBmag->GetRates().y*sin(Attitude.x);
			if (A9K3)
			{
				//Assumption: secant amplifier gets saturated at +/-80° yaw
				pitchrate *= min(5.75877, max(-5.75877, 1.0 / cos(Attitude.z)));
			}
		}
		else
			pitchrate = 0.0;
	}
	else
	{
		if (A2K3)
		{
			//GDC align
			//Factor 3.33 because:
			//ASCP output: 19.1V * sin(error)
			//BMAG output: 0.1V per °/s (so 5.729 V per rad/s)
			//ASCP output basically needs to be convered to an attitude rate in rad/s, so the factor should be 19.1 divided by 5.729
			//Source: Apollo SCS Detailed Training Program
			if (E0_503PR)
				pitchrate = 3.33*sat->ascp.GetPitchEulerAttitudeSetInput();
			else
				pitchrate = 0.0;
		}
		else
		{
			if (A2K2)
			{
				//Entry (0.05G)
				pitchrate = 0.0;
			}
			else
			{
				//Non-Euler
				pitchrate = pitchBmag->GetRates().x;
			}
		}
	}

	//Roll Voltage to Frequency Converter (Module A3)
	double rollrate;
	if (A3K1)
	{
		//Euler mode
		rollrate = primRollBmag->GetRates().z - pitchrate * sin(Attitude.z);
	}
	else
	{
		if (A3K3)
		{
			//GDC align
			if (E0_503PR)
				rollrate = 3.33*sat->ascp.GetRollEulerAttitudeSetInput();
			else
				rollrate = 0.0;
		}
		else
		{
			if (A3K2)
			{
				//Entry (0.05G)
				rollrate = primRollBmag->GetRates().z*cos(21.0*RAD) +  sat->bmag1.GetRates().y*sin(21.0*RAD);
			}
			else
			{
				//Non-Euler
				rollrate = primRollBmag->GetRates().z;
			}
		}
	}
	
	//Yaw Voltage to Frequency Converter (Module A4)
	double yawrate;
	if (A4K1)
	{
		if (E1_504 && E2_502)
		{
			//Euler mode
			yawrate = -yawBmag->GetRates().y*cos(Attitude.x) + pitchBmag->GetRates().x*sin(Attitude.x);
		}
		else
			yawrate = 0.0;
	}
	else
	{
		if (A4K3)
		{
			//GDC align
			if (E0_503Y)
				yawrate = 3.33*sat->ascp.GetYawEulerAttitudeSetInput();
			else
				yawrate = 0.0;
		}
		else
		{
			if (A4K2)
			{
				//Entry (0.05G)
				yawrate = redunRollBmag->GetRates().z*cos(21.0*RAD) + yawBmag->GetRates().y*sin(21.0*RAD);
			}
			else
			{
				//Non-Euler
				yawrate = -yawBmag->GetRates().y;
			}
		}
	}
	
	//Yaw and EMS Digital Output (Module A6)
	//Yaw
	if (!A6K1)
	{
		if (E2_502 && E2_503)
		{
			Attitude.z += yawrate * simdt;

			if (Attitude.z >= PI2) {
				Attitude.z -= PI2;
			}
			if (Attitude.z < 0) {
				Attitude.z += PI2;
			}
		}
	}
	//EMS
	if (A6K2 && E2_502)
	{
		sat->ems.SetRSIDeltaRotation(yawrate*simdt);
	}

	//Roll and Pitch Digital Output (Module A7)
	if (E1_502 && E1_503)
	{
		Attitude.x += rollrate * simdt;
		if (Attitude.x >= PI2) {
			Attitude.x -= PI2;
		}
		if (Attitude.x < 0) {
			Attitude.x += PI2;
		}
	}

	if (E2_502 && E2_503)
	{
		Attitude.y += pitchrate * simdt;
		if (Attitude.y >= PI2) {
			Attitude.y -= PI2;
		}
		if (Attitude.y < 0) {
			Attitude.y += PI2;
		}
	}

	//TBD: Remove this part when it doesn't get used anymore by other SCS subsystems
	rates.x = pitchBmag->GetRates().x;
	// Special Logic for Entry .05 Switch
	if (sat->GSwitch.IsUp()) {
		// Entry Stability Roll Transformation
		rates.y = -primRollBmag->GetRates().z * tan(21.0 * RAD) + yawBmag->GetRates().y;
		// sprintf(oapiDebugString(), "entry roll rate? %f", rates.y);
	} else {
		// Normal Operation
		rates.y = yawBmag->GetRates().y;
	}
	rates.z = primRollBmag->GetRates().z;

	//sprintf(oapiDebugString(), "Body: %f %f %f Euler: %f %f %f", sat->bmag1.GetRates().z*DEG, sat->bmag1.GetRates().x*DEG, sat->bmag1.GetRates().y*DEG, rollrate*DEG, pitchrate*DEG, yawrate*DEG);
}

double GDC::GetRollBodyMinusEulerError()
{
	return sat->ascp.GetPitchEulerAttitudeSetInput()*sin(Attitude.z);
}

double GDC::GetPitchBodyError()
{
	return sat->ascp.GetYawEulerAttitudeSetInput()*sin(Attitude.x) + sat->ascp.GetPitchEulerAttitudeSetInput()*cos(Attitude.z)*cos(Attitude.x);
}

double GDC::GetYawBodyError()
{
	return sat->ascp.GetYawEulerAttitudeSetInput()*cos(Attitude.x) - sat->ascp.GetPitchEulerAttitudeSetInput()*cos(Attitude.z)*sin(Attitude.x);
}

double GDC::GetRollEulerResolver()
{
	if (E0_505PR) return Attitude.x;

	return 0.0;
}

double GDC::GetPitchEulerResolver()
{
	if (E0_505PR) return Attitude.y;

	return 0.0;
}

double GDC::GetYawEulerResolver()
{
	if (E0_505Y) return Attitude.z;

	return 0.0;
}

void GDC::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, GDC_START_STRING);
	oapiWriteScenario_int(scn, "FDAIERRENA", fdai_err_ena);
	oapiWriteScenario_int(scn, "FDAIERRX", fdai_err_x);
	oapiWriteScenario_int(scn, "FDAIERRY", fdai_err_y);
	oapiWriteScenario_int(scn, "FDAIERRZ", fdai_err_z);
	papiWriteScenario_double(scn, "ATTITUDEX", Attitude.x);
	papiWriteScenario_double(scn, "ATTITUDEY", Attitude.y);
	papiWriteScenario_double(scn, "ATTITUDEZ", Attitude.z);
	papiWriteScenario_bool(scn, "A8K1", A8K1);
	papiWriteScenario_bool(scn, "A8K2", A8K2);
	papiWriteScenario_bool(scn, "E0_505PR", E0_505PR);
	papiWriteScenario_bool(scn, "E0_505Y", E0_505Y);

	oapiWriteLine(scn, GDC_END_STRING);
}

void GDC::LoadState(FILEHANDLE scn){

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, GDC_END_STRING, sizeof(GDC_END_STRING))){
			return;
		}

		if (!strnicmp (line, "FDAIERRENA", 10)) {
			sscanf(line + 10, "%i", &fdai_err_ena);
		}
		else if (!strnicmp (line, "FDAIERRX", 8)) {
			sscanf(line + 8, "%i", &fdai_err_x);
		}
		else if (!strnicmp (line, "FDAIERRY", 8)) {
			sscanf(line + 8, "%i", &fdai_err_y);
		}
		else if (!strnicmp (line, "FDAIERRZ", 8)) {
			sscanf(line + 8, "%i", &fdai_err_z);
		} 
		else if (!strnicmp(line, "ATTITUDEX", 9)) {
			sscanf(line + 9, "%lf", &Attitude.x);
		}
		else if (!strnicmp(line, "ATTITUDEY", 9)) {
			sscanf(line + 9, "%lf", &Attitude.y);
		}
		else if (!strnicmp(line, "ATTITUDEZ", 9)) {
			sscanf(line + 9, "%lf", &Attitude.z);
		}
		papiReadScenario_bool(line, "A8K1", A8K1);
		papiReadScenario_bool(line, "A8K2", A8K2);
		papiReadScenario_bool(line, "E0_505PR", E0_505PR);
		papiReadScenario_bool(line, "E0_505Y", E0_505Y);
	}
}

//
// ASCP
//

#define ASCP_ROLLUP		1
#define ASCP_ROLLDOWN	2
#define ASCP_PITCHUP	3
#define ASCP_PITCHDOWN	4
#define ASCP_YAWUP		5
#define ASCP_YAWDOWN	6

ASCP::ASCP(Sound &clicksound) : ClickSound(clicksound)

{
	// These are the nominal values for a 72° launch azimuth.
	output.x = 162;
	output.y = 90;
	output.z = 0;
	sat = NULL;

	rolldisplay = 0;
	pitchdisplay = 0;
	yawdisplay = 0;

	mousedowncounter = 0;
	mousedownposition = 0;
	mousedownangle = -3;
}

void ASCP::Init(Saturn *vessel)

{
	sat = vessel;
}

void ASCP::TimeStep(double simdt)

{
	// Mouse hold handling
				
	if (mousedownposition) {
		mousedownangle += simdt * 6.0;
		if (mousedownangle > 0) {
			double change = floor(mousedownangle * 10.0) / 10.0;
			mousedownangle -= change;
			if (change > 0) {
				ClickSound.play();
				if (mousedownposition == ASCP_ROLLUP) {
					output.x += change;
					// Wrap around
					if (output.x > 359.95) { output.x -= 360.0; }
					
					rolldisplay += change * 2.0;
					if (rolldisplay > 4) rolldisplay = 0;
				
				} else if (mousedownposition == ASCP_ROLLDOWN) {
					output.x -= change;
					// Wrap around
					if (output.x < 0) { output.x += 360.0; }
					if (output.x > 359.95) { output.x -= 360.0; }

					rolldisplay -= change * 2.0;
					if (rolldisplay < 0) rolldisplay = 4;
				
				}  else if (mousedownposition == ASCP_PITCHUP) {
					output.y += change;

					// Wrap around
					if (output.y > 359.95) { output.y -= 360.0; }

					pitchdisplay += change * 2.0;
					if (pitchdisplay > 4) pitchdisplay = 0;

				}  else if (mousedownposition == ASCP_PITCHDOWN) {
					output.y -= change;

					// Wrap around
					if (output.y < 0) { output.y += 360; }
					if (output.y > 359.95) { output.y -= 360.0; }

					pitchdisplay -= change * 2.0;
					if (pitchdisplay < 0) pitchdisplay = 4;

				}  else if (mousedownposition == ASCP_YAWUP) {
					output.z += change;

					if(output.z > 90 && output.z < 270){ // Can't get here
						output.z = 90;
					}	
					// Wrap around zero
					if (output.z > 359.95) { output.z -= 360.0; }

					yawdisplay += change * 2.0;
					if (yawdisplay > 4) yawdisplay = 0;

				}  else if (mousedownposition == ASCP_YAWDOWN) {
					output.z -= change;

					if(output.z < 270 && output.z > 90){ // Can't get here
						output.z = 270;
					}	
					// Wrap around zero
					if (output.z < 0) { output.z += 360; }
					if (output.z > 359.95) { output.z -= 360.0; }

					yawdisplay -= change * 2.0;
					if (yawdisplay < 0) yawdisplay = 4;
				}
			}
		}
	}
	
	// Debug messages
	if(msgcounter > 0){
		msgcounter--;
		if(msgcounter == 0) {
#ifdef _DEBUG
			sprintf(oapiDebugString(),""); // Clear message
#endif
		}
	}
}

double ASCP::CalcRollEulerAttitudeSetError()
{
	if (sat->eda.GetIMUtoAttSetRoll())
	{
		return sin(output.x*RAD - sat->imu.GetTotalAttitude().x);
	}
	else
	{
		return sin(output.x*RAD - sat->gdc.GetRollEulerResolver());
	}
}

double ASCP::CalcPitchEulerAttitudeSetError()
{
	if (sat->eda.GetIMUtoAttSetPitch())
	{
		return sin(output.y*RAD - sat->imu.GetTotalAttitude().y);
	}
	else
	{
		return sin(output.y*RAD - sat->gdc.GetPitchEulerResolver());
	}
}

double ASCP::CalcYawEulerAttitudeSetError()
{
	if (sat->eda.GetIMUtoAttSetYaw())
	{
		return sin(output.z*RAD - sat->imu.GetTotalAttitude().z);
	}
	else
	{
		return sin(output.z*RAD - sat->gdc.GetYawEulerResolver());
	}
}

double ASCP::GetRollEulerAttitudeSetError()
{
	return CalcRollEulerAttitudeSetError();
}

double ASCP::GetPitchEulerAttitudeSetError()
{
	if (!sat->gdc.GetPitchRollAttitudeSetInputEnable())
	{
		return CalcPitchEulerAttitudeSetError();
	}

	return 0.0;
}

double ASCP::GetYawEulerAttitudeSetError()
{
	if (!sat->gdc.GetYawAttitudeSetInputEnable())
	{
		return CalcYawEulerAttitudeSetError();
	}

	return 0.0;
}

double ASCP::GetRollEulerAttitudeSetInput()
{
	if (sat->gdc.GetPitchRollAttitudeSetInputEnable())
	{
		return CalcRollEulerAttitudeSetError();
	}

	return 0.0;
}

double ASCP::GetPitchEulerAttitudeSetInput()
{
	if (sat->gdc.GetPitchRollAttitudeSetInputEnable())
	{
		return CalcPitchEulerAttitudeSetError();
	}

	return 0.0;
}

double ASCP::GetYawEulerAttitudeSetInput()
{
	if (sat->gdc.GetYawAttitudeSetInputEnable())
	{
		return CalcYawEulerAttitudeSetError();
	}

	return 0.0;
}

bool ASCP::RollDisplayClicked(){
	msgcounter = 50; // Keep for 50 timesteps
#ifdef _DEBUG
	sprintf(oapiDebugString(),"ASCP: Roll = %05.1f",output.x);
#endif
	return true;
}

bool ASCP::PitchDisplayClicked(){
	msgcounter = 50;
#ifdef _DEBUG
	sprintf(oapiDebugString(),"ASCP: Pitch = %05.1f",output.y);
#endif
	return true;
}

bool ASCP::YawDisplayClicked(){
	msgcounter = 50;
#ifdef _DEBUG
	sprintf(oapiDebugString(),"ASCP: Yaw = %05.1f",output.z);
#endif
	return true;
}

bool ASCP::RollClick(int Event, int mx, int my)

{
	if (my > 18) {
		if (RollDnClick(Event)) {
			rolldisplay--;
			if (rolldisplay < 0) rolldisplay = 4;
			ClickSound.play();
			return true;
		}
	} else {
		if (RollUpClick(Event)) {
			rolldisplay++;
			if (rolldisplay > 4) rolldisplay = 0;
			ClickSound.play();
			return true;
		}
	}
	return false;
}

bool ASCP::RollUpClick(int Event)

{
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedownposition = ASCP_ROLLUP;
			mousedowncounter++;
			if (mousedowncounter == 1) {
				changed = true;
				output.x++;
			} 
			break;

		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if (mousedowncounter == 1){
				output.x += 0.1;
				changed = true;
			} 
			break;

		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0;
			mousedownposition = 0;
			mousedownangle = -3;
			return false; 
			break;
	}
	// Wrap around 
	if (output.x > 359.95) { output.x -= 360.0; }
	RollDisplayClicked();
	return changed;
}

bool ASCP::RollDnClick(int Event)

{
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedownposition = ASCP_ROLLDOWN;
			mousedowncounter++;
			if (mousedowncounter == 1) {
				changed = true;
				output.x--;
			} 
			break;

		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.x -= 0.1;
				changed = true;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0;
			mousedownposition = 0;
			mousedownangle = -3;
			return false; 
			break;
	}
	// Wrap around
	if (output.x < 0) { output.x += 360.0; }
	if (output.x > 359.95) { output.x -= 360.0; }
	RollDisplayClicked();
	return changed;
}

bool ASCP::PitchClick(int Event, int mx, int my)

{
	if (my > 18) {
		if (PitchDnClick(Event)) {
			pitchdisplay--;
			if (pitchdisplay < 0) pitchdisplay = 4;
			ClickSound.play();
			return true;
		}
	} else {
		if (PitchUpClick(Event)) {
			pitchdisplay++;
			if (pitchdisplay > 4) pitchdisplay = 0;
			ClickSound.play();
			return true;
		}
	}
	return false;
}

bool ASCP::PitchUpClick(int Event)

{
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedownposition = ASCP_PITCHUP;
			mousedowncounter++;
			if (mousedowncounter == 1) {
				changed = true;
				output.y++;
			} 
			break;

		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.y += 0.1;
				changed = true;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0;
			mousedownposition = 0;
			mousedownangle = -3;
			return false; 
			break;
	}
	// Wrap around
	if (output.y > 359.95) { output.y -= 360.0; }
	PitchDisplayClicked();
	return changed;
}

bool ASCP::PitchDnClick(int Event)

{
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedownposition = ASCP_PITCHDOWN;
			mousedowncounter++;
			if (mousedowncounter == 1) {
				changed = true;
				output.y--;
			} 
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.y -= 0.1;
				changed = true;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0;
			mousedownposition = 0;
			mousedownangle = -3;
			return false; 
			break;
	}
	// Wrap around
	if (output.y < 0) { output.y += 360; }
	if (output.y > 359.95) { output.y -= 360.0; }
	PitchDisplayClicked();
	return changed;
}

bool ASCP::YawClick(int Event, int mx, int my)

{
	if (my > 18) {
		if (YawDnClick(Event)) {
			yawdisplay--;
			if (yawdisplay < 0) yawdisplay = 4;
			ClickSound.play();
			return true;
		}
	} else {
		if (YawUpClick(Event)) {
			yawdisplay++;
			if (yawdisplay > 4) yawdisplay = 0;
			ClickSound.play();
			return true;
		}
	}
	return false;
}

bool ASCP::YawUpClick(int Event)

{
	// Cannot click beyond 90 degrees.
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedownposition = ASCP_YAWUP;
			mousedowncounter++;
			if (mousedowncounter == 1) {
				changed = true;
				output.z++;
			} 
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.z += 0.1;
				changed = true;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0;
			mousedownposition = 0;
			mousedownangle = -3;
			return false; 
			break;
	}
	if(output.z > 90 && output.z < 270){ // Can't get here
		output.z = 90;
	}	
	// Wrap around zero
	if (output.z > 359.95) { output.z -= 360.0; }
	YawDisplayClicked();
	return changed;
}

bool ASCP::YawDnClick(int Event)

{
	// Cannot click beyond 270 degrees.
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedownposition = ASCP_YAWDOWN;
			mousedowncounter++;
			if (mousedowncounter == 1) {
				changed = true;
				output.z--;
			} 
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.z -= 0.1;
				changed = true;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0;
			mousedownposition = 0;
			mousedownangle = -3;
			return false; 
			break;
	}
	if(output.z < 270 && output.z > 90){ // Can't get here
		output.z = 270;
	}	
	// Wrap around zero
	if (output.z < 0) { output.z += 360; }
	if (output.z > 359.95) { output.z -= 360.0; }
	YawDisplayClicked();
	return changed;
}

void ASCP::PaintRoll(SURFHANDLE surf, SURFHANDLE wheel)

{
	oapiBlt(surf, wheel, 0, 0, (int) rolldisplay * 17, 0, 17, 36, SURF_PREDEF_CK);
}

void ASCP::PaintPitch(SURFHANDLE surf, SURFHANDLE wheel)

{
	oapiBlt(surf, wheel, 0, 0, (int) pitchdisplay * 17, 0, 17, 36, SURF_PREDEF_CK);
}

void ASCP::PaintYaw(SURFHANDLE surf, SURFHANDLE wheel)

{
	oapiBlt(surf, wheel, 0, 0, (int) yawdisplay * 17, 0, 17, 36, SURF_PREDEF_CK);
}

bool ASCP::PaintDisplay(SURFHANDLE surf, SURFHANDLE digits, double value) {
	int srx, sry, beta, digit, digit0, digit1;
	int x=(int)fabs(value*10);
	beta = x%10;
	digit = (x%100)/10;
	digit1 = (x%1000)/100;
	digit0 = x/1000;
	sry = (int)(beta * 1.2);

	srx = 8 + (digit0 * 25);	 
	if (!(beta != 0 && digit == 9 && (digit1 == 9 || (digit1 == 5 && digit0 == 3)))) {
		oapiBlt(surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	} else {
		oapiBlt(surf, digits, 0, sry, srx, 33, 9, 12 - sry, SURF_PREDEF_CK);			
		if (digit0 == 3) digit0 = 0; else digit0++;
		srx = 8 + (digit0 * 25);			
		oapiBlt(surf, digits, 0, 0, srx, 45 - sry, 9, sry, SURF_PREDEF_CK);
	}

	srx = 8 + (digit1 * 25);
	if (digit != 9 || beta == 0) {
		oapiBlt(surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	} else {
		oapiBlt(surf, digits, 10, sry, srx, 33, 9, 12 - sry, SURF_PREDEF_CK);			

		if (digit1 == 9) digit1 = 0; else digit1++;
		if (digit1 == 6 && digit0 == 3) digit1 = 0;
		srx = 8 + (digit1 * 25);			
		oapiBlt(surf, digits, 10, 0, srx, 45 - sry, 9, sry, SURF_PREDEF_CK);
	}

	srx = 8 + (digit * 25);
	if (beta == 0) {		
		oapiBlt(surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	} else {
		oapiBlt(surf, digits, 20, sry, srx, 33, 9, 12 - sry, SURF_PREDEF_CK);			
		if (digit == 9) digit = 0; else digit++;
		srx = 8 + (digit * 25);			
		oapiBlt(surf, digits, 20, 0, srx, 45 - sry, 9, sry, SURF_PREDEF_CK);
	}
	return true;
}

bool ASCP::PaintRollDisplay(SURFHANDLE surf, SURFHANDLE digits) {
	return PaintDisplay(surf, digits, output.x);
}

bool ASCP::PaintPitchDisplay(SURFHANDLE surf, SURFHANDLE digits) {
	return PaintDisplay(surf, digits, output.y);
}

bool ASCP::PaintYawDisplay(SURFHANDLE surf, SURFHANDLE digits){
	return PaintDisplay(surf, digits, output.z);
}

void ASCP::SaveState(FILEHANDLE scn){
	oapiWriteLine(scn, ASCP_START_STRING);
	oapiWriteScenario_float(scn, "OPX", output.x);
	oapiWriteScenario_float(scn, "OPY", output.y);
	oapiWriteScenario_float(scn, "OPZ", output.z);
	oapiWriteLine(scn, ASCP_END_STRING);
}

void ASCP::LoadState(FILEHANDLE scn){
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, ASCP_END_STRING, sizeof(ASCP_END_STRING))){
			return;
		}
		if (!strnicmp (line, "OPX", 3)) {
			sscanf(line + 3, "%f", &flt);
			output.x = flt;
		}
		else if (!strnicmp (line, "OPY", 3)) {
			sscanf(line + 3, "%f", &flt);
			output.y = flt;
		}
		else if (!strnicmp (line, "OPZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			output.z = flt;
		}
	}
}


// EDA
EDA::EDA(){
	sat = NULL; // Initialize
	ac_source1 = NULL;
	ac_source2 = NULL;
	mna_source = NULL;
	mnb_source = NULL;

	FDAI1AttitudeRate = _V(0, 0, 0);
	FDAI2AttitudeRate = _V(0, 0, 0);
	InstrAttitudeRate = _V(0, 0, 0);
	FDAI1Attitude = _V(0, 0, 0);
	FDAI2Attitude = _V(0, 0, 0);
	FDAI1AttitudeError = _V(0, 0, 0);
	FDAI2AttitudeError = _V(0, 0, 0);
	InstrAttitudeError = _V(0, 0, 0);
	GPFPIPitch[0] = GPFPIPitch[1] = 0.0;
	GPFPIYaw[0] = GPFPIYaw[1] = 0.0;

	ResetRelays();
	ResetTransistors();
}

void EDA::Init(Saturn *vessel){
	sat = vessel;
}

void EDA::WireTo(e_object *ac1, e_object *ac2, e_object *dca, e_object *dcb)
{
	ac_source1 = ac1;
	ac_source2 = ac2;
	mna_source = dca;
	mnb_source = dcb;
}

void EDA::ResetRelays()
{
	A4K1 = false;
	A4K2 = false;
	A4K3 = false;
	A4K4 = false;
	A5K1 = false;
	A5K2 = false;
	A5K3 = false;
	A5K4 = false;
	A5K5 = false;
	A8K1 = false;
	A8K3 = false;
	A8K5 = false;
	A8K7 = false;
	A8K9 = false;
	A8K11 = false;
	A8K13 = false;
	A8K15 = false;
	A8K17 = false;
	A9K1 = false;
	A9K2 = false;
	A9K4 = false;
	A9K5 = false;
	A11K1 = false;
	A11K2 = false;
	A11K3 = false;
	A11K4 = false;
	A11K5 = false;
	A11K6 = false;
	GDC118A8K1 = false;
}

void EDA::ResetTransistors()
{
	T1QS53 = false;
	T1QS54 = false;
	T1QS55 = false;
	T1QS56 = false;
	T1QS57 = false;
	T1QS58 = false;
	T1QS59 = false;
	T1QS60 = false;
	T1QS63 = false;
	T1QS64 = false;
	T1QS65 = false;
	T1QS68 = false;
	T1QS71 = false;
	T1QS72 = false;
	T1QS73 = false;
	T1QS75 = false;
	T1QS76 = false;
	T1QS78 = false;
	T2QS53 = false;
	T2QS54 = false;
	T2QS55 = false;
	T2QS56 = false;
	T2QS57 = false;
	T2QS58 = false;
	T2QS59 = false;
	T2QS60 = false;
	T2QS63 = false;
	T2QS64 = false;
	T2QS65 = false;
	T2QS68 = false;
	T2QS71 = false;
	T2QS72 = false;
	T2QS73 = false;
	T2QS76 = false;
	T3QS53 = false;
	T3QS54 = false;
	T3QS55 = false;
	T3QS56 = false;
	T3QS57 = false;
	T3QS58 = false;
	T3QS59 = false;
	T3QS60 = false;
	T3QS64 = false;
	T3QS65 = false;
	T3QS71 = false;
	T3QS72 = false;
	T3QS73 = false;
	T3QS76 = false;
}

void EDA::Timestep(double simdt)
{
	if (!IsPowered())
	{
		GDC118A8K1 = false;
		return;
	}

	//POWER
	bool E1_307, E1_309, E2_307, E2_309, EA_315, EB_315;

	if (ac_source1 && ac_source1->Voltage() > SP_MIN_ACVOLTAGE)
	{
		E1_307 = true;
		E1_309 = true;
	}
	else
	{
		E1_307 = false;
		E1_309 = false;
	}

	if (ac_source2 && ac_source2->Voltage() > SP_MIN_ACVOLTAGE)
	{
		E2_307 = true;
		E2_309 = true;
	}
	else
	{
		E2_307 = false;
		E2_309 = false;
	}

	if (mna_source && mna_source->Voltage() > SP_MIN_DCVOLTAGE)
		EA_315 = true;
	else
		EA_315 = false;

	if (mnb_source && mnb_source->Voltage() > SP_MIN_DCVOLTAGE)
		EB_315 = true;
	else
		EB_315 = false;

	//CMC ATT - IMU Enable
	bool S2_1;
	//FDAI Scale 50/15 Enable
	bool S3_1;
	//FDAI Scale 5/5 Enable
	bool S3_2;
	//FDAI Select 1
	bool S4_1;
	//FDAI Select 2
	bool S4_2;
	//FDAI Select 1/2
	bool S4_3;
	//FDAI Source - CMC
	bool S5_1;
	//FDAI Source - GDC
	bool S5_2;
	//FDAI Source - Att Set
	bool S5_3;
	//Att Set - GDC
	bool S6_1;
	//Att Set - IMU
	bool S6_2;
	//BMAG Roll - Rate 1
	bool S20_3;
	//BMAG Pitch - Rate 1
	bool S21_3;
	//BMAG Yaw - Rate 1
	bool S22_3;
	//Entry 0.05G - Off
	bool S51_1;
	//BMAG uncaged
	bool GP;

	//Inversed logic used in circuits
	if (sat->bmag1.IsUncaged().x != 0.0 || sat->bmag1.IsUncaged().y != 0.0 || sat->bmag1.IsUncaged().z != 0.0)
		GP = false;
	else
		GP = true;

	//SWITCH LOGIC
	//2-1
	if (sat->CMCAttSwitch.IsUp() && (sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE || sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE))
		S2_1 = true;
	else
		S2_1 = false;

	//3-1
	if (sat->FDAIScaleSwitch.IsDown() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE)
		S3_1 = true;
	else
		S3_1 = false;

	//3-2
	if (sat->FDAIScaleSwitch.IsCenter() && sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE)
		S3_2 = true;
	else
		S3_2 = false;

	//4-1
	if (sat->FDAISelectSwitch.IsDown() && sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE)
		S4_1 = true;
	else
		S4_1 = false;

	//4-2
	if (sat->FDAISelectSwitch.IsCenter() && sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE)
		S4_2 = true;
	else
		S4_2 = false;

	//4-3
	if (sat->FDAISelectSwitch.IsUp() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE)
		S4_3 = true;
	else
		S4_3 = false;

	//5-1
	if (sat->FDAISourceSwitch.IsUp() && sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE)
		S5_1 = true;
	else
		S5_1 = false;

	//5-2
	if (sat->FDAISourceSwitch.IsDown() && sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE)
		S5_2 = true;
	else
		S5_2 = false;

	//5-3
	if (sat->FDAISourceSwitch.IsCenter() && sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE)
		S5_3 = true;
	else
		S5_3 = false;

	//6-1
	if (sat->FDAIAttSetSwitch.IsDown() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE)
		S6_1 = true;
	else
		S6_1 = false;

	//6-2
	if (sat->FDAIAttSetSwitch.IsUp() && sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE)
		S6_2 = true;
	else
		S6_2 = false;

	//20-3
	if (sat->BMAGRollSwitch.IsDown() && sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE)
		S20_3 = true;
	else
		S20_3 = false;

	//21-3
	if (sat->BMAGPitchSwitch.IsDown() && sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE)
		S21_3 = true;
	else
		S21_3 = false;

	//22-3
	if (sat->BMAGYawSwitch.IsDown() && sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE)
		S22_3 = true;
	else
		S22_3 = false;

	//51-1
	if (sat->GSwitch.IsDown() && sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE)
		S51_1 = true;
	else
		S51_1 = false;

	//RELAYS AND TRANSISTORS

	if (EB_315 && S4_1 && (S5_2 || (S5_3 && S6_1)))
		A4K1 = true;
	else
		A4K1 = false;

	if (EA_315 && S2_1 && (S4_1 || S4_3))
		A4K2 = true;
	else
		A4K2 = false;

	if (EA_315 && S4_1 && (S5_2 || (S5_3 && S6_1)))
		A4K3 = true;
	else
		A4K3 = false;

	if (EB_315 && S4_1 && (S5_2 || (S5_3 && S6_1)))
		GDC118A8K1 = true;
	else
		GDC118A8K1 = false;

	if (EB_315 && S2_1 && (S4_2 || S4_3))
		A4K4 = true;
	else
		A4K4 = false;

	if (S20_3)
		A5K1 = true;
	else
		A5K1 = false;

	if (S22_3 || (S20_3 && !S51_1))
		A5K2 = true;
	else
		A5K2 = false;

	if (S21_3)
		A5K3 = true;
	else
		A5K3 = false;	
	
	if (S4_2)
	{
		A9K5 = true;
		A5K5 = true;
		A5K4 = true;
	}
	else
	{
		A9K5 = false;
		A5K5 = false;
		A5K4 = false;
	}

	if ((S4_2 && (S5_1 || (S5_3 && S6_2))) || (S4_1 && (S5_2 || (S5_3 && S6_1))))
	{
		A8K1 = true;
		A8K3 = true;
		A8K5 = true;
		A8K7 = true;
		A8K9 = true;
		A8K11 = true;
	}
	else
	{
		A8K1 = false;
		A8K3 = false;
		A8K5 = false;
		A8K7 = false;
		A8K9 = false;
		A8K11 = false;
	}

	if (S6_2)
	{
		A8K13 = true;
		A8K15 = true;
		A8K17 = true;
	}
	else
	{
		A8K13 = false;
		A8K15 = false;
		A8K17 = false;
	}

	if (S4_1 && (S5_2 || (S5_3 && S6_1)))
		A9K1 = true;
	else
		A9K1 = false;

	if (S5_2 || S4_3)
		A9K2 = true;
	else
		A9K2 = false;

	if (S5_3 && S4_2 && S6_1)
		A9K4 = true;
	else
		A9K4 = false;

	if (sat->LVFuelTankPressIndicatorSwitch.IsUp() && sat->EDS1BatACircuitBraker.IsPowered())
		A11K1 = A11K3 = A11K6 = true;
	else
		A11K1 = A11K3 = A11K6 = false;

	if (sat->LVFuelTankPressIndicatorSwitch.IsUp() && sat->EDS3BatBCircuitBraker.IsPowered())
		A11K2 = A11K4 = A11K5 = true;
	else
		A11K2 = A11K4 = A11K5 = false;

	if (!((S4_1 && S5_2) || (S5_3 && S6_1)))
	{
		T1QS53 = true;
		T2QS53 = true;
		T3QS53 = true;
	}
	else
	{
		T1QS53 = false;
		T2QS53 = false;
		T3QS53 = false;
	}

	if (!((S4_2 && S5_2) || (S5_3 && S6_1) || S4_3))
	{
		T1QS54 = true;
		T2QS54 = true;
		T3QS54 = true;
	}
	else
	{
		T1QS54 = false;
		T2QS54 = false;
		T3QS54 = false;
	}

	if (S4_2 || S4_3 || !S2_1 || S5_2 || S5_1)
		T1QS55 = true;
	else
		T1QS55 = false;

	if (S4_1|| S4_3 || !S2_1 || S5_2 || S5_1)
		T1QS56 = true;
	else
		T1QS56 = false;

	if (!(S6_2 && S2_1 && S5_3 && S4_1))
	{
		T2QS55 = true;
		T3QS55 = true;
	}
	else
	{
		T2QS55 = false;
		T3QS55 = false;
	}

	if (!(S5_3 && S6_2 && S4_2 && S2_1))
	{
		T2QS56 = true;
		T3QS56 = true;
	}
	else
	{
		T2QS56 = false;
		T3QS56 = false;
	}

	if (S5_1 || S5_3 || S4_2 || S4_3 || GP)
	{
		T1QS57 = true;
		T2QS57 = true;
		T3QS57 = true;
	}
	else
	{
		T1QS57 = false;
		T2QS57 = false;
		T3QS57 = false;
	}

	if (!((S4_3 && S51_1 && !GP) || (S4_2 && S5_2 && !GP)))
	{
		T1QS58 = true;
		T2QS58 = true;
		T3QS58 = true;
	}
	else
	{
		T1QS58 = false;
		T2QS58 = false;
		T3QS58 = false;
	}

	if (!((S5_1 && S4_1) || S4_3))
	{
		T1QS59 = true;
		T2QS59 = true;
		T3QS59 = true;
	}
	else
	{
		T1QS59 = false;
		T2QS59 = false;
		T3QS59 = false;
	}

	if (S5_2 || S5_3 || S4_1 || S4_3)
	{
		T1QS60 = true;
		T2QS60 = true;
		T3QS60 = true;
	}
	else
	{
		T1QS60 = false;
		T2QS60 = false;
		T3QS60 = false;
	}

	if (S20_3 || S4_1)
		T1QS63 = true;
	else
		T1QS63 = false;

	if (!S20_3 || S4_2)
		T1QS64 = true;
	else
		T1QS64 = false;

	if (!S20_3 || S4_1)
		T1QS65 = true;
	else
		T1QS65 = false;

	if (S20_3 || S4_2)
		T1QS68 = true;
	else
		T1QS68 = false;

	if (!(S3_1 && (S5_1 || !(S4_1 || S4_2))))
		T1QS75 = true;
	else
		T1QS75 = false;

	if (!(S3_1 && S5_1 && S4_2))
		T1QS76 = true;
	else
		T1QS76 = false;

	if (S20_3 || S51_1 || S4_2 || S22_3)
		T2QS63 = true;
	else
		T2QS63 = false;

	if (S22_3 || S4_2 || (S20_3 && !S51_1))
		T2QS64 = true;
	else
		T2QS64 = false;

	if (S22_3 || S4_1 || (S20_3 && !S51_1))
		T2QS65 = true;
	else
		T2QS65 = false;

	if (S20_3 || S22_3 || S51_1 || S4_1)
		T2QS66 = true;
	else
		T2QS66 = false;

	if (!S3_1)
	{
		T1QS67 = true;
		T2QS67 = true;
		T3QS67 = true;
	}
	else
	{
		T1QS67 = false;
		T2QS67 = false;
		T3QS67 = false;
	}

	if (S51_1 || S4_2 || (!S20_3 && !S22_3))
		T2QS68 = true;
	else
		T2QS68 = false;

	if (S51_1 || S4_1 || (!S20_3 && !S22_3))
		T2QS69 = true;
	else
		T2QS69 = false;

	if (S3_1 || S3_2)
	{
		T1QS71 = true;
		T2QS71 = true;
		T3QS71 = true;
	}
	else
	{
		T1QS71 = false;
		T2QS71 = false;
		T3QS71 = false;
	}

	if (S3_1)
	{
		T1QS72 = true;
		T2QS72 = true;
		T3QS72 = true;
	}
	else
	{
		T1QS72 = false;
		T2QS72 = false;
		T3QS72 = false;
	}

	if (S5_1 || S5_2 || S4_3 || S6_2)
		T1QS73 = true;
	else
		T1QS73 = false;

	if (S5_1 || S5_2 || S4_2 || S4_3 || S6_2)
	{
		T2QS73 = true;
		T3QS73 = true;
	}
	else
	{
		T2QS73 = false;
		T3QS73 = false;
	}

	if (S5_1 || S5_2 || S4_1 || S4_3 || S6_2)
	{
		T2QS74 = true;
		T3QS74 = true;
	}
	else
	{
		T2QS74 = false;
		T3QS74 = false;
	}

	if (S4_2 || !S22_3 && (!S20_3 || S51_1))
		T2QS76 = true;
	else
		T2QS76 = false;

	if (S4_1 || !S22_3 && (!S20_3 || S51_1))
		T2QS77 = true;
	else
		T2QS77 = false;

	if (S21_3 || S4_2)
		T3QS64 = true;
	else
		T3QS64 = false;

	if (S21_3 || S4_1)
		T3QS65 = true;
	else
		T3QS65 = false;

	if (!S21_3 || S4_2)
		T3QS76 = true;
	else
		T3QS76 = false;

	if (!S21_3 || S4_1)
		T3QS77 = true;
	else
		T3QS77 = false;

	if (S5_1 || S5_2 || S6_1)
		T1QS78 = true;
	else
		T1QS78 = false;

	//Input signal collection
	VECTOR3 bmag1rates = sat->bmag1.GetRates();
	VECTOR3 bmag2rates = sat->bmag2.GetRates();
	VECTOR3 imuatt = sat->imu.GetTotalAttitude();
	VECTOR3 cmcerr = _V(sat->gdc.fdai_err_x, sat->gdc.fdai_err_y, sat->gdc.fdai_err_z) * 5.0 / 0.3 / 384.0*RAD;	//Converted from -384/384 to radians (-16.66°/16.66°)

	double rate, err;

	//PITCH DISPLAY PROCESSING
	//FDAI 1 attitude
	if (A4K2)
	{
		if (A8K5)
			FDAI1Attitude.y = sat->gdc.GetPitchEulerResolver();
		else
			FDAI1Attitude.y = imuatt.y;

		//ORDEAL should be interfacing between the EDA and the DEA(?), but this is a convenient place
		FDAI1Attitude.y += sat->ordeal.GetFDAI1PitchAngle();
		if (FDAI1Attitude.y >= PI2) FDAI1Attitude.y -= PI2;
	}
	else
		FDAI1Attitude.y = 0.0;

	//FDAI 2 attitude
	if (A4K4)
	{
		if (A8K7)
			FDAI2Attitude.y = imuatt.y;
		else
			FDAI2Attitude.y = sat->gdc.GetPitchEulerResolver();

		//ORDEAL should be interfacing between the EDA and the DEA(?), but this is a convenient place
		FDAI2Attitude.y += sat->ordeal.GetFDAI2PitchAngle();
		if (FDAI2Attitude.y >= PI2) FDAI2Attitude.y -= PI2;
	}
	else
		FDAI2Attitude.y = 0.0;

	//FDAI 1 attitude rate
	if (E1_307)
	{
		//Disable logic
		rate = (T3QS76 ? 0.0 : bmag1rates.x) + (T3QS64 ? 0.0 : bmag2rates.x);
		//Scale factor for 1°/s
		rate /= (1.0*RAD);
		//Scale to 5°/s
		if (T3QS71) rate /= 5.0;
		//Scale to 10°/s
		if (T3QS72) rate /= 2.0;
		FDAI1AttitudeRate.x = rate;
	}
	else
	{
		FDAI1AttitudeRate.x = 0.0;
	}

	//FDAI2 attitude rate
	if (E2_307)
	{
		//Disable logic
		rate = (T3QS77 ? 0.0 : bmag1rates.x) + (T3QS65 ? 0.0 : bmag2rates.x);
		//Scale factor for 1°/s
		rate /= (1.0*RAD);
		//Scale to 5°/s
		if (T3QS71) rate /= 5.0;
		//Scale to 10°/s
		if (T3QS72) rate /= 2.0;
		FDAI2AttitudeRate.x = rate;
	}
	else
	{
		FDAI2AttitudeRate.x = 0.0;
	}

	//Instrumentation
	if (HasSigCondPower())
	{
		if (A5K5)
		{
			InstrAttitudeRate.x = FDAI2AttitudeRate.x;
		}
		else
		{
			InstrAttitudeRate.x = FDAI1AttitudeRate.x;
		}
	}
	else
	{
		InstrAttitudeRate.x = 0.0;
	}

	//FDAI 1 attitude error
	if (E1_307)
	{
		err = (T3QS73 ? 0.0 : -sat->gdc.GetPitchBodyError()) + (T3QS55 ? 0.0 : -sat->ascp.GetPitchEulerAttitudeSetError()) + (T3QS59 ? 0.0 : cmcerr.y) + (T3QS57 ? 0.0 : NormalizeAngle(-sat->bmag1.GetAttitudeError().y));
		//Scale factor for 15°/s
		err /= (15.0*RAD);
		//Scale to 5°/s
		if (T3QS67) err *= 3.0;
		FDAI1AttitudeError.y = 41.0*err;
	}
	else
	{
		FDAI1AttitudeError.y = 0.0;
	}

	//FDAI 2 attitude error
	if (E2_307)
	{
		err = (T3QS74 ? 0.0 : -sat->gdc.GetPitchBodyError()) + (T3QS56 ? 0.0 : -sat->ascp.GetPitchEulerAttitudeSetError()) + (T3QS60 ? 0.0 : cmcerr.y) + (T3QS58 ? 0.0 : NormalizeAngle(-sat->bmag1.GetAttitudeError().y));
		//Scale factor for 15°/s
		err /= (15.0*RAD);
		//Scale to 5°/s
		if (T3QS67) err *= 3.0;
		FDAI2AttitudeError.y = 41.0*err;
	}
	else
	{
		FDAI2AttitudeError.y = 0.0;
	}

	//Instrumentation attitude error
	if (HasSigCondPower())
	{
		if (A9K5)
		{
			InstrAttitudeError.y = FDAI2AttitudeError.y;
		}
		else
		{
			InstrAttitudeError.y = FDAI1AttitudeError.y;
		}
	}
	else
	{
		InstrAttitudeError.y = 0.0;
	}

	//YAW DISPLAY PROCESSING
	//FDAI 1 attitude
	if (A4K2)
	{
		if (A8K9)
			FDAI1Attitude.z = sat->gdc.GetYawEulerResolver();
		else
			FDAI1Attitude.z = imuatt.z;
	}
	else
		FDAI1Attitude.z = 0.0;

	//FDAI 2 attitude
	if (A4K4)
	{
		if (A8K11)
			FDAI2Attitude.z = imuatt.z;
		else
			FDAI2Attitude.z = sat->gdc.GetYawEulerResolver();
	}
	else
		FDAI2Attitude.z = 0.0;

	//FDAI 1 attitude rate
	if (E1_307)
	{
		//Disable logic
		rate = (T2QS76 ? 0.0 : bmag1rates.y) + (T2QS64 ? 0.0 : bmag2rates.y) + (T2QS68 ? 0.0 : -bmag1rates.z*tan(21.0*RAD)) + (T2QS63 ? 0.0 : -bmag2rates.z*tan(21.0*RAD));
		//Scale factor for 1°/s
		rate /= (1.0*RAD);
		//Scale to 5°/s
		if (T2QS71) rate /= 5.0;
		//Scale to 10°/s
		if (T2QS72) rate /= 2.0;
		FDAI1AttitudeRate.y = rate;
	}
	else
	{
		FDAI1AttitudeRate.y = 0.0;
	}

	//FDAI 2 attitude rate
	if (E2_307)
	{
		//Disable logic
		rate = (T2QS77 ? 0.0 : bmag1rates.y) + (T2QS65 ? 0.0 : bmag2rates.y) + (T2QS69 ? 0.0 : -bmag1rates.z*tan(21.0*RAD)) + (T2QS66 ? 0.0 : -bmag2rates.z*tan(21.0*RAD));
		//Scale factor for 1°/s
		rate /= (1.0*RAD);
		//Scale to 5°/s
		if (T2QS71) rate /= 5.0;
		//Scale to 10°/s
		if (T2QS72) rate /= 2.0;
		FDAI2AttitudeRate.y = rate;
	}
	else
	{
		FDAI2AttitudeRate.y = 0.0;
	}

	//Instrumentation
	if (HasSigCondPower())
	{
		if (A5K4)
		{
			InstrAttitudeRate.y = FDAI2AttitudeRate.y;
		}
		else
		{
			InstrAttitudeRate.y = FDAI1AttitudeRate.y;
		}
	}
	else
	{
		InstrAttitudeRate.y = 0.0;
	}

	//FDAI 1 attitude error
	if (E1_307)
	{
		err = (T2QS73 ? 0.0 : sat->gdc.GetYawBodyError()) + (T2QS55 ? 0.0 : sat->ascp.GetYawEulerAttitudeSetError()) + (T2QS59 ? 0.0 : cmcerr.z) + (T2QS57 ? 0.0 : NormalizeAngle(sat->bmag1.GetAttitudeError().z));
		//Scale factor for 15°/s
		err /= (15.0*RAD);
		//Scale to 5°/s
		if (T2QS67) err *= 3.0;
		//FDAI error is -scaled -41 to 41
		FDAI1AttitudeError.z = 41.0*err;
	}
	else
	{
		FDAI1AttitudeError.z = 0.0;
	}

	//FDAI 2 attitude error
	if (E2_307)
	{
		err = (T2QS74 ? 0.0 : sat->gdc.GetYawBodyError()) + (T2QS56 ? 0.0 : sat->ascp.GetYawEulerAttitudeSetError()) + (T2QS60 ? 0.0 : cmcerr.z) + (T2QS58 ? 0.0 : NormalizeAngle(sat->bmag1.GetAttitudeError().z));
		//Scale factor for 15°/s
		err /= (15.0*RAD);
		//Scale to 5°/s
		if (T2QS67) err *= 3.0;
		//FDAI error is -scaled -41 to 41
		FDAI2AttitudeError.z = 41.0*err;
	}
	else
	{
		FDAI2AttitudeError.z = 0.0;
	}

	//Instrumentation attitude error
	if (HasSigCondPower())
	{
		if (A9K5)
		{
			InstrAttitudeError.z = FDAI2AttitudeError.z;
		}
		else
		{
			InstrAttitudeError.z = FDAI1AttitudeError.z;
		}
	}
	else
	{
		InstrAttitudeError.z = 0.0;
	}

	//ROLL DISPLAY PROCESSING
	//FDAI 1 attitude
	if (A4K2)
	{
		if (A8K1)
			FDAI1Attitude.x = sat->gdc.GetRollEulerResolver();
		else
			FDAI1Attitude.x = imuatt.x;
	}
	else
		FDAI1Attitude.x = 0.0;

	//FDAI 2 attitude
	if (A4K4)
	{
		if (A8K3)
			FDAI2Attitude.x = imuatt.x;
		else
			FDAI2Attitude.x = sat->gdc.GetRollEulerResolver();
	}
	else
		FDAI2Attitude.x = 0.0;

	//FDAI 1 attitude rate
	if (E1_307)
	{
		//Disable logic
		rate = (T1QS64 ? 0.0 : bmag1rates.z) + (T1QS68 ? 0.0 : bmag2rates.z);
		//Scale factor for 1°/s
		rate /= (1.0*RAD);
		//Scale to 5°/s
		if (T1QS71) rate /= 5.0;
		//Scale to 50°/s
		if (T1QS72) rate /= 10.0;
		FDAI1AttitudeRate.z = rate;
	}
	else
	{
		FDAI1AttitudeRate.z = 0.0;
	}
	//FDAI 2 attitude rate
	if (E2_307)
	{
		//Disable logic
		rate = (T1QS65 ? 0.0 : bmag1rates.z) + (T1QS63 ? 0.0 : bmag2rates.z);
		//Scale factor for 1°/s
		rate /= (1.0*RAD);
		//Scale to 5°/s
		if (T1QS71) rate /= 5.0;
		//Scale to 50°/s
		if (T1QS72) rate /= 10.0;
		FDAI2AttitudeRate.z = rate;
	}
	else
	{
		FDAI2AttitudeRate.z = 0.0;
	}
	//Instrumentation
	if (HasSigCondPower())
	{
		if (A5K5)
		{
			InstrAttitudeRate.z = FDAI2AttitudeRate.z;
		}
		else
		{
			InstrAttitudeRate.z = FDAI1AttitudeRate.z;
		}
	}
	else
	{
		InstrAttitudeRate.z = 0.0;
	}

	//FDAI 1 attitude error
	if (E1_307)
	{
		err = (T1QS55 ? 0.0 : ((T1QS73 ? 0.0 : sat->gdc.GetRollBodyMinusEulerError()) + sat->ascp.GetRollEulerAttitudeSetError())) + (T1QS59 ? 0.0 : cmcerr.x) + (T1QS57 ? 0.0 : NormalizeAngle(sat->bmag1.GetAttitudeError().x));
		//Scale factor for 50°/s
		err /= (50.0*RAD);
		//Scale to 5.0°/s
		if (T1QS67) err *= 10.0;
		//Scale to 12.5°/s (for CDU)
		if (!T1QS75) err *= 4.0;
		//error is -scaled -41 to 41
		FDAI1AttitudeError.x = 41.0*err;
	}
	else
	{
		FDAI1AttitudeError.x = 0.0;
	}

	//FDAI 2 attitude error
	if (E2_307)
	{
		err = (T1QS56 ? 0.0 : ((T1QS73 ? 0.0 : sat->gdc.GetRollBodyMinusEulerError()) + sat->ascp.GetRollEulerAttitudeSetError())) + (T1QS60 ? 0.0 : cmcerr.x) + (T1QS58 ? 0.0 : NormalizeAngle(sat->bmag1.GetAttitudeError().x));
		//Scale factor for 50°/s
		err /= (50.0*RAD);
		//Scale to 5.0°/s
		if (T1QS67) err *= 10.0;
		//Scale to 12.5°/s (for CDU)
		if (!T1QS76) err *= 4.0;
		//FDAI error is -scaled -41 to 41
		FDAI2AttitudeError.x = 41.0*err;
	}
	else
	{
		FDAI2AttitudeError.x = 0.0;
	}

	//Instrumentation attitude error
	if (HasSigCondPower())
	{
		if (A5K4)
		{
			InstrAttitudeError.x = FDAI2AttitudeError.x;
		}
		else
		{
			InstrAttitudeError.x = FDAI1AttitudeError.x;
		}
	}
	else
	{
		InstrAttitudeError.x = 0.0;
	}

	// GPI/S-IVB Pressure DECA
	if (E1_307)
	{
		if (A11K3)
		{
			LVTankQuantities LVq;
			sat->GetLVTankQuantities(LVq);

			if (sat->GetStage() > LAUNCH_STAGE_TWO_ISTG_JET) {
				GPFPIPitch[0] = 89.0 * LVq.SIVBOxQuantity / LVq.S4BOxMass;
			}
			else {
				GPFPIPitch[0] = 89.0 * LVq.SIIQuantity / LVq.SIIFuelMass;
			}
		}
		else
		{
			GPFPIPitch[0] = (10.0 * sat->tvsa.GetPitchGimbalPosition()*DEG);
		}
		GPFPIPitch[0] += (A11K1 ? 0.0 : 44.0);

		if (A11K6)
		{
			LVTankQuantities LVq;
			sat->GetLVTankQuantities(LVq);

			GPFPIYaw[0] = 89.0 * LVq.SIVBFuelQuantity / LVq.S4BFuelMass;
		}
		else
		{
			GPFPIYaw[0] = (10.0 * sat->tvsa.GetYawGimbalPosition()*DEG);
		}
		GPFPIYaw[0] += (A11K1 ? 0.0 : 44.0);
	}
	else
		GPFPIPitch[0] = GPFPIYaw[0] = 0.0;

	if (E2_307)
	{
		if (A11K4)
		{
			LVTankQuantities LVq;
			sat->GetLVTankQuantities(LVq);

			if (sat->GetStage() > LAUNCH_STAGE_TWO_ISTG_JET) {
				GPFPIPitch[1] = 89.0 * LVq.SIVBOxQuantity / LVq.S4BOxMass;
			}
			else {
				GPFPIPitch[1] = 89.0 * LVq.SIIQuantity / LVq.SIIFuelMass;
			}
		}
		else
		{
			GPFPIPitch[1] = (10.0 * sat->tvsa.GetPitchGimbalPosition()*DEG);
		}
		GPFPIPitch[1] += (A11K2 ? 0.0 : 44.0);

		if (A11K5)
		{
			LVTankQuantities LVq;
			sat->GetLVTankQuantities(LVq);

			GPFPIYaw[1] = 89.0 * LVq.SIVBFuelQuantity / LVq.S4BFuelMass;
		}
		else
		{
			GPFPIYaw[1] = (10.0 * sat->tvsa.GetYawGimbalPosition()*DEG);
		}
		GPFPIYaw[1] += (A11K2 ? 0.0 : 44.0);
	}
	else
		GPFPIPitch[1] = GPFPIYaw[1] = 0.0;

	//sprintf(oapiDebugString(), "FDAI1: %.01f %.01f %.01f FDAI2: %.01f %.01f %.01f", FDAI1AttitudeRate.x, FDAI1AttitudeRate.y, FDAI1AttitudeRate.z,
	//	FDAI2AttitudeRate.x, FDAI2AttitudeRate.y, FDAI2AttitudeRate.z);
	//sprintf(oapiDebugString(), "FDAI1: %.1f %.1f %.1f FDAI2: %.1f %.1f %.1f", FDAI1AttitudeError.x, FDAI1AttitudeError.y, FDAI1AttitudeError.z,
	//	FDAI2AttitudeError.x, FDAI2AttitudeError.y, FDAI2AttitudeError.z);
	//sprintf(oapiDebugString(), "%d %d %d %d %f %f %f", T3QS74, T3QS56, T3QS60, T3QS58,
	//	sat->bmag1.GetAttitudeError().x, sat->bmag1.GetAttitudeError().y, sat->bmag1.GetAttitudeError().z);
	//sprintf(oapiDebugString(), "%d %d %d %d", T1QS73, T1QS56, T1QS60, T1QS58);
	//sprintf(oapiDebugString(), "%d %d %d %d %f %f", T1QS55, T1QS73, T1QS59, T1QS57, sat->gdc.GetRollBodyMinusEulerError()*DEG, sat->ascp.GetRollEulerAttitudeSetError()*DEG);
	//sprintf(oapiDebugString(), "%f %f %f", gdcatt.x*DEG, gdcatt.y*DEG, gdcatt.z*DEG);
	//sprintf(oapiDebugString(), "%f %f %f", bmag1rates.x*DEG, bmag1rates.y*DEG, bmag1rates.z*DEG);
}

bool EDA::IsPowered()
{
	if (ac_source1 && ac_source1->Voltage() > SP_MIN_DCVOLTAGE) return true;
	if (ac_source2 && ac_source2->Voltage() > SP_MIN_DCVOLTAGE) return true;

	return false;
}

bool EDA::HasSigCondPower()
{
	if (sat->SIGCondDriverBiasPower1Switch.IsPowered()) return true;

	return false;
}

double EDA::GetConditionedPitchAttErr()
{
	if (!HasSigCondPower()) { return 0.0; }

	//Attitude error already scaled -41 to 41 pixels
	return scale_data(InstrAttitudeError.y);
}

double EDA::GetConditionedYawAttErr()
{
	if (!HasSigCondPower()) { return 0.0; }

	//Attitude error already scaled -41 to 41 pixels
	return scale_data(InstrAttitudeError.z);
}

double EDA::GetConditionedRollAttErr()
{
	if (!HasSigCondPower()) { return 0.0; }

	//Attitude error already scaled -41 to 41 pixels
	return scale_data(InstrAttitudeError.x);
}

double EDA::GetInstPitchAttRate()
{
	if (!HasSigCondPower()) { return 0.0; }
	
	return inst_scale_rates(InstrAttitudeRate.x);
}

double EDA::GetInstYawAttRate()
{
	if (!HasSigCondPower()) { return 0.0; }

	return inst_scale_rates(InstrAttitudeRate.y);
}

double EDA::GetInstRollAttRate()
{
	if (!HasSigCondPower()) { return 0.0; }

	return inst_scale_rates(InstrAttitudeRate.z);
}

double EDA::GetGPFPIPitch(int sys)
{
	if (sys == 1 || sys == 2) return GPFPIPitch[sys - 1];

	return 0.0;
}

double EDA::GetGPFPIYaw(int sys)
{
	if (sys == 1 || sys == 2) return GPFPIYaw[sys - 1];

	return 0.0;
}

double EDA::scale_data(double data)
{
	// First eliminate cases outside of the scales
	if (data >= 41.0) { return 5.0; }
	if (data <= -41.0) { return 0.0; }

	// then return result
	return (data + 41.0) / 16.4;
}

double EDA::inst_scale_rates(double data)
{
	// First eliminate cases outside of the scales
	if (data >= 1.0) { return 5.0; }
	if (data <= -1.0) { return 0.0; }

	// then return result
	return (data + 1.0) / 0.4;
}

double EDA::NormalizeAngle(double ang)
{
	if (ang > PI)
		return ang - PI2;
	else if (ang < -PI)
		return ang + PI2;
	return ang;
}

void EDA::SaveState(FILEHANDLE scn)
{
	oapiWriteLine(scn, EDA_START_STRING);
	papiWriteScenario_vec(scn, "FDAI1TOTALATTITUDE", FDAI1Attitude);
	papiWriteScenario_vec(scn, "FDAI2TOTALATTITUDE", FDAI2Attitude);
	papiWriteScenario_vec(scn, "FDAI1ATTITUDERATE", FDAI1AttitudeRate);
	papiWriteScenario_vec(scn, "FDAI2ATTITUDERATE", FDAI2AttitudeRate);
	papiWriteScenario_vec(scn, "FDAI1ATTITUDEERROR", FDAI1AttitudeError);
	papiWriteScenario_vec(scn, "FDAI2ATTITUDEERROR", FDAI2AttitudeError);
	papiWriteScenario_doublearr(scn, "GPFPIPITCH", GPFPIPitch, 2);
	papiWriteScenario_doublearr(scn, "GPFPIYAW", GPFPIYaw, 2);
	papiWriteScenario_bool(scn, "GDC118A8K1", GDC118A8K1);
	oapiWriteLine(scn, EDA_END_STRING);
}

void EDA::LoadState(FILEHANDLE scn)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, EDA_END_STRING, sizeof(EDA_END_STRING))) {
			return;
		}
		papiReadScenario_vec(line, "FDAI1TOTALATTITUDE", FDAI1Attitude);
		papiReadScenario_vec(line, "FDAI2TOTALATTITUDE", FDAI2Attitude);
		papiReadScenario_vec(line, "FDAI1ATTITUDERATE", FDAI1AttitudeRate);
		papiReadScenario_vec(line, "FDAI2ATTITUDERATE", FDAI2AttitudeRate);
		papiReadScenario_vec(line, "FDAI1ATTITUDEERROR", FDAI1AttitudeError);
		papiReadScenario_vec(line, "FDAI2ATTITUDEERROR", FDAI2AttitudeError);
		papiReadScenario_doublearr(line, "GPFPIPITCH", GPFPIPitch, 2);
		papiReadScenario_doublearr(line, "GPFPIYAW", GPFPIYaw, 2);
		papiReadScenario_bool(line, "GDC118A8K1", GDC118A8K1);
	}
}


// Reaction Jet / Engine Control
RJEC::RJEC() : 
engineOnDelayA(1.0),
engineOnDelayB(1.0),
engineOffDelay(1.0)
{
	
	sat = NULL;
	DirectPitchActive = false;
	DirectYawActive = false;
	DirectRollActive = false;
	SCSLatchUpA = false;
	SCSLatchUpB = false;
	SPSEnableA = false;
	SPSEnableB = false;
	IGN1 = false;
	IGN2 = false;

	int i = 0;
	while (i < 20) {
		ThrusterDemand[i] = false;
		PoweredSwitch[i] = NULL;
		i++;
	}
}

void RJEC::Init(Saturn *vessel) {
	sat = vessel;
}

void RJEC::SystemTimestep(double simdt) {

	// Draw AC power
	if (sat->SIGCondDriverBiasPower1Switch.IsPowered()) {
		sat->SIGCondDriverBiasPower1Switch.DrawPower(2.3); 
	}
	if (sat->SIGCondDriverBiasPower2Switch.IsPowered()) {
		sat->SIGCondDriverBiasPower2Switch.DrawPower(0.2); 
	}

	// Thruster power
	int i;
	for (i = 0; i < 17; i++) {
		if (PoweredSwitch[i] != NULL) {	
			PoweredSwitch[i]->DrawPower(50); 
		}
	}
}

bool RJEC::IsThrusterPowered(ThreePosSwitch *s) {

	// see AOH Figure 2.5-2
	if (s->IsPowered() && 
		((s->IsUp() && sat->secs.MESCA.GetAutoRCSEnableRelay() && sat->ContrAutoMnACircuitBraker.IsPowered()) ||
		 (s->IsDown() && sat->secs.MESCB.GetAutoRCSEnableRelay() && sat->ContrAutoMnBCircuitBraker.IsPowered()))) {
		return true;
	}
	return false;
}

void RJEC::SetRCSState(int thruster, bool td, bool cm, int smquad, int smthruster, int cmthruster, ThreePosSwitch *s, bool lockout) {

	if (IsThrusterPowered(s) && !lockout) {
		if (td != 0) { 
			PoweredSwitch[thruster] = s;
		}			
		if (!cm || cmthruster < 0) {
			sat->SetRCSState(smquad, smthruster, td); 
			if (cmthruster >= 0) {
				sat->SetCMRCSState(cmthruster, false);
			}
		} else {
			sat->SetCMRCSState(cmthruster, td);
			sat->SetRCSState(smquad, smthruster, false); 
		}
	} else {
		sat->SetRCSState(smquad, smthruster, false); 
		if (cmthruster >= 0) {
			sat->SetCMRCSState(cmthruster, false);
		}
	}
}

void RJEC::TimeStep(double simdt){
	//Delay timers
	engineOnDelayA.Timestep(simdt);
	engineOnDelayB.Timestep(simdt);
	engineOffDelay.Timestep(simdt);

	//Logic
	bool scslogic1, scslogic2, scslogic3, S8_1, S9_1, S10_1, S18_1, S18_2, thc_cw;

	scslogic1 = sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE;
	scslogic2 = sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE;
	scslogic3 = sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE;

	S8_1 = sat->ManualAttPitchSwitch.IsUp() && scslogic1;
	S9_1 = sat->ManualAttYawSwitch.IsUp() && scslogic1;
	S10_1 = sat->ManualAttRollSwitch.IsUp() && scslogic1;
	S18_1 = sat->SCContSwitch.IsDown() && scslogic2;
	S18_2 = sat->SCContSwitch.IsDown() && scslogic3;
	thc_cw = sat->THCRotary.IsClockwise() && scslogic2;

	/* Thruster List:
	CM#		SM#		INDEX#		SWITCH GROUP		ROT AXIS

	1		C3		1			PITCH				+PITCH
	2		A4		2			PITCH				-PITCH
	3		A3		3			PITCH				+PITCH
	4		C4		4			PITCH				-PITCH
	5		D3		5			YAW					+YAW
	6		B4		6			YAW					-YAW
	7		B3		7			YAW					+YAW
	8		D4		8			YAW					-YAW
	9		B1		9			ROLL B/D			+ROLL
	10		D2		10			ROLL B/D			-ROLL
	11		D1		11			ROLL B/D			+ROLL
	12		B2		12			ROLL B/D			-ROLL
	xx		A1		13			ROLL A/C			+ROLL
	xx		A2		14			ROLL A/C			-ROLL
	xx		C1		15			ROLL A/C			+ROLL
	xx		C2		16			ROLL A/C			-ROLL

	*/

	// Reset thruster power demand
	bool td[20];
	int i;
	for (i = 0; i < 17; i++) {
		td[i] = false;
		PoweredSwitch[i] = NULL;
	}

	//
	// ACCEL CMD: ECA auto-control is inhibited. Auto fire commands are generated from the breakout switches.
	//

	// Roll

	if ((S18_1 || S18_2) && S10_1) {
		if (sat->rhc1.GetMinusRollBreakoutSwitch() || sat->rhc2.GetMinusRollBreakoutSwitch()) {  // MINUS
			td[10] = true;
			td[12] = true;
			td[14] = true;
			td[16] = true;
		}
		if (sat->rhc1.GetPlusRollBreakoutSwitch() || sat->rhc2.GetPlusRollBreakoutSwitch()) { // PLUS
			td[9] = true;
			td[11] = true;
			td[13] = true;
			td[15] = true;
		}
	}
	else
	{
		td[9] = ThrusterDemand[9];
		td[10] = ThrusterDemand[10];
		td[11] = ThrusterDemand[11];
		td[12] = ThrusterDemand[12];
		td[13] = ThrusterDemand[13];
		td[14] = ThrusterDemand[14];
		td[15] = ThrusterDemand[15];
		td[16] = ThrusterDemand[16];
	}

	// Pitch

	if ((S18_1 || S18_2) && S8_1) {
		if (sat->rhc1.GetMinusPitchBreakoutSwitch() || sat->rhc2.GetMinusPitchBreakoutSwitch()) {  // MINUS
			td[2] = true;
			td[4] = true;
		}
		if (sat->rhc1.GetPlusPitchBreakoutSwitch() || sat->rhc2.GetPlusPitchBreakoutSwitch()) { // PLUS
			td[1] = true;
			td[3] = true;
		}
	}
	else
	{
		td[1] = ThrusterDemand[1];
		td[2] = ThrusterDemand[2];
		td[3] = ThrusterDemand[3];
		td[4] = ThrusterDemand[4];
	}

	// Yaw
	
	if ((S18_1 || S18_2) && S9_1) {
		if (sat->rhc1.GetMinusYawBreakoutSwitch() || sat->rhc2.GetMinusYawBreakoutSwitch()) {  // MINUS
			td[6] = true;
			td[8] = true;
		}
		if (sat->rhc1.GetPlusYawBreakoutSwitch() || sat->rhc2.GetPlusYawBreakoutSwitch()) { // PLUS
			td[5] = true;
			td[7] = true;
		}
	}
	else
	{
		td[5] = ThrusterDemand[5];
		td[6] = ThrusterDemand[6];
		td[7] = ThrusterDemand[7];
		td[8] = ThrusterDemand[8];
	}

	// Ensure AC logic power, see Systems Handbook 8.2 
	if (!sat->SIGCondDriverBiasPower1Switch.IsPowered()) {
		td[1] = false;
		td[2] = false;
		td[4] = false;
		td[6] = false;
		td[8] = false;
		td[9] = false;
		td[12] = false;
		td[14] = false;
	}
	if (!sat->SIGCondDriverBiasPower2Switch.IsPowered()) {
		td[3] = false;
		td[5] = false;
		td[7] = false;
		td[10] = false;
		td[11] = false;
		td[13] = false;
		td[15] = false;
		td[16] = false;
	}

	//
	// TRANSLATION HANDLING
	//

	// CM/SM transfer handling
	bool sm_sep = false;
	bool CMTransferMotor1 = sat->secs.rcsc.GetCMTransferMotor1();
	bool CMTransferMotor2 = sat->secs.rcsc.GetCMTransferMotor2();
	if (CMTransferMotor1 || CMTransferMotor2) sm_sep = true;

	if ((S18_2 || thc_cw) && !sm_sep) {
		if (sat->eca.thc_x < 16384) { // PLUS X
			td[14] = true;
			td[15] = true;
		}
		if (sat->eca.thc_x > 49152) { // MINUS X
			td[16] = true;
			td[13] = true;
		}
		if (sat->eca.thc_y > 49152) { // MINUS Y (FORWARD)
			td[1] = true;
			td[2] = true;
			td[5] = true;
			td[6] = true;
		}
		if (sat->eca.thc_y < 16384) { // PLUS Y (BACKWARD)
			td[3] = true;
			td[4] = true;
			td[7] = true;
			td[8] = true;
		}
		if (sat->eca.thc_z > 49152) { // MINUS Z (UP)
			td[11] = true;
			td[12] = true;
		}
		if (sat->eca.thc_z < 16384) { // PLUS Z (DOWN)
			td[9] = true;
			td[10] = true;
		}
	}

	int thruster = 1;
	bool thruster_lockout;
	while(thruster < 17) {
		// THRUSTER LOCKOUT CHECKING
		thruster_lockout = 0;
		// If it's a pitch or yaw jet, lockout on SPS thrusting
		if (thruster < 9 && IGN1) {
			thruster_lockout = 1; 
		} 
		// Lockout on direct axes.
		if (thruster < 5 && DirectPitchActive) {
			thruster++; 
			continue; // Skip entirely
		} 
		if (thruster > 4 && thruster < 9 && DirectYawActive) {
			thruster++; 
			continue; // Skip entirely
		} 
		if (thruster > 8 && DirectRollActive) {
			thruster++; 
			continue; // Skip entirely
		} 
		// THRUSTER PROCESSING
		switch(thruster) {
			case 1:	 SetRCSState(thruster, td[thruster], CMTransferMotor1, RCS_SM_QUAD_C, 3,  0, &sat->PitchC3Switch,  thruster_lockout); break;
			case 2:	 SetRCSState(thruster, td[thruster], CMTransferMotor1, RCS_SM_QUAD_A, 4,  2, &sat->PitchA4Switch,  thruster_lockout); break;
			case 3:	 SetRCSState(thruster, td[thruster], CMTransferMotor2, RCS_SM_QUAD_A, 3,  1, &sat->PitchA3Switch,  thruster_lockout); break;
			case 4:	 SetRCSState(thruster, td[thruster], CMTransferMotor2, RCS_SM_QUAD_C, 4,  3, &sat->PitchC4Switch,  thruster_lockout); break;
			case 5:	 SetRCSState(thruster, td[thruster], CMTransferMotor2, RCS_SM_QUAD_D, 3,  5, &sat->YawD3Switch,    thruster_lockout); break;
			case 6:	 SetRCSState(thruster, td[thruster], CMTransferMotor2, RCS_SM_QUAD_B, 4,  6, &sat->YawB4Switch,    thruster_lockout); break;
			case 7:	 SetRCSState(thruster, td[thruster], CMTransferMotor1, RCS_SM_QUAD_B, 3,  4, &sat->YawB3Switch,    thruster_lockout); break;
			case 8:	 SetRCSState(thruster, td[thruster], CMTransferMotor1, RCS_SM_QUAD_D, 4,  7, &sat->YawD4Switch,    thruster_lockout); break;
			case 9:	 SetRCSState(thruster, td[thruster], CMTransferMotor1, RCS_SM_QUAD_B, 1,  8, &sat->BdRollB1Switch, thruster_lockout); break;
			case 10: SetRCSState(thruster, td[thruster], CMTransferMotor2, RCS_SM_QUAD_D, 2, 10, &sat->BdRollD2Switch, thruster_lockout);	break;
			case 11: SetRCSState(thruster, td[thruster], CMTransferMotor2, RCS_SM_QUAD_D, 1,  9, &sat->BdRollD1Switch, thruster_lockout);	break;
			case 12: SetRCSState(thruster, td[thruster], CMTransferMotor1, RCS_SM_QUAD_B, 2, 11, &sat->BdRollB2Switch, thruster_lockout);	break;
			case 13: SetRCSState(thruster, td[thruster], false,            RCS_SM_QUAD_A, 1, -1, &sat->AcRollA1Switch, thruster_lockout);	break;
			case 14: SetRCSState(thruster, td[thruster], false,            RCS_SM_QUAD_A, 2, -1, &sat->AcRollA2Switch, thruster_lockout);	break;
			case 15: SetRCSState(thruster, td[thruster], false,            RCS_SM_QUAD_C, 1, -1, &sat->AcRollC1Switch, thruster_lockout);	break;
			case 16: SetRCSState(thruster, td[thruster], false,            RCS_SM_QUAD_C, 2, -1, &sat->AcRollC2Switch, thruster_lockout);	break;
		}
		thruster++;
	}

	//SPS ENGINE ON/OFF LOGIC
	bool S24, S25, S26, S59, AB_X16, AB_X70, AB_X71, scsmode, scsengineon;
	bool scsengineonA1, scsengineonB1, scsengineonA2, scsengineonB2, DV_EMS_Set, cmcsignal, cmcengineon, logicA, logicB;
	ChannelValue val11;

	//Switches
	S24 = sat->DirectUllageButton.GetState() == 1 && scslogic1;
	S25 = sat->ThrustOnButton.GetState() == 1 && scslogic1;
	S26 = sat->dVThrust1Switch.Voltage() > SP_MIN_DCVOLTAGE;
	S59 = sat->dVThrust2Switch.Voltage() > SP_MIN_DCVOLTAGE;
	AB_X16 = td[1] && td[2] && td[5] && td[6];	//TBD: This can be done better with a THC class
	AB_X70 = sat->secs.MESCA.FireUllage() && sat->RCSLogicMnACircuitBraker.IsPowered();
	AB_X71 = sat->secs.MESCB.FireUllage() && sat->RCSLogicMnBCircuitBraker.IsPowered();
	DV_EMS_Set = sat->ems.IsdVMode() && sat->ems.GetdVRangeCounter() >= 0;
	val11 = sat->agc.GetOutputChannel(011);
	cmcsignal = val11[12];

	//Engine Signals
	//Put this first to not introduce a one timestep delay
	bool SCSLatchUpLogic, engineofflogic1, engineofflogic2;

	scsmode = S18_2 || thc_cw;
	SCSLatchUpA = S26 && sat->SPSEngine.IsThrustOnA();
	SCSLatchUpB = S59 && sat->SPSEngine.IsThrustOnB();

	SCSLatchUpLogic = SCSLatchUpA || SCSLatchUpB;
	engineOnDelayA.SetRunning(SCSLatchUpLogic);
	engineOnDelayB.SetRunning(SCSLatchUpLogic);
	engineofflogic1 = engineOnDelayA.ContactClosed() && scsmode;
	engineOffDelay.SetRunning(engineofflogic1);
	engineofflogic2 = (engineOnDelayB.ContactClosed() || engineOffDelay.ContactClosed()) && scsmode;
	IGN1 = engineofflogic1 || engineofflogic2;
	IGN2 = SCSLatchUpLogic || IGN1;

	//Engine on logic
	scsengineon = S25 && (S24 || AB_X16 || AB_X70 || AB_X71);
	scsengineonA1 = scsengineon || SCSLatchUpA;
	scsengineonB1 = scsengineon || SCSLatchUpB;
	scsengineonA2 = scsmode && scsengineonA1 && DV_EMS_Set;
	scsengineonB2 = scsmode && scsengineonB1 && DV_EMS_Set;
	cmcengineon = !scsmode && cmcsignal;
	logicA = scsengineonA2 || cmcengineon;
	logicB = scsengineonB2 || cmcengineon;
	SPSEnableA = logicA && S26 && sat->SIGCondDriverBiasPower1Switch.IsPowered();
	SPSEnableB = logicB && S59 && sat->SIGCondDriverBiasPower2Switch.IsPowered();
	
	//sprintf(oapiDebugString(), "%d %d %f %d %d", IGN1, IGN2, engineOffDelay.GetTime(), engineOffDelay.IsRunning(), engineOffDelay.ContactClosed());
}

void RJEC::SetThruster(int thruster, bool Active) {
	if (thruster > 0 && thruster < 20) {
		ThrusterDemand[thruster] = Active; // Next timestep does the work
	}
}

bool RJEC::GetThruster(int thruster) {
	if (thruster > 0 && thruster < 20) {
		return ThrusterDemand[thruster];
	}
	return false;
}

void RJEC::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, RJEC_START_STRING);
	
	/// \todo Don't save thruster demand for now, because the controlling devices (AGC, ECA) 
	///       doesn't store their state properly, which leads to stuck thrusters at scenario start
	///		  or at separation events
	/* 
	int i;
	char buffer[100];
	
	for (i = 1; i < 17; i++) {
		sprintf(buffer, "THRUSTERDEMAND%i", i);
		papiWriteScenario_bool(scn, buffer, ThrusterDemand[i]);
	} 
	*/
	papiWriteScenario_bool(scn, "IGN1", IGN1); 
	papiWriteScenario_bool(scn, "IGN2", IGN2);
	papiWriteScenario_bool(scn, "SCSLATCHUPA", SCSLatchUpA);
	papiWriteScenario_bool(scn, "SCSLATCHUPB", SCSLatchUpB);
	papiWriteScenario_bool(scn, "SPSENABLEA", SPSEnableA);
	papiWriteScenario_bool(scn, "SPSENABLEB", SPSEnableB);
	papiWriteScenario_bool(scn, "DIRECTPITCHACTIVE", DirectPitchActive); 
	papiWriteScenario_bool(scn, "DIRECTYAWACTIVE", DirectYawActive); 
	papiWriteScenario_bool(scn, "DIRECTROLLACTIVE", DirectRollActive);
	engineOnDelayA.SaveState(scn, "DELAYONA_BEGIN", "DELAYONA_END");
	engineOnDelayB.SaveState(scn, "DELAYONB_BEGIN", "DELAYONB_END");
	engineOffDelay.SaveState(scn, "DELAYOFF_BEGIN", "DELAYOFF_END");

	oapiWriteLine(scn, RJEC_END_STRING);
}

void RJEC::LoadState(FILEHANDLE scn){

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, RJEC_END_STRING, sizeof(RJEC_END_STRING))) {
			return;

		/// \todo Don't load thruster demand for now, because the controlling devices (AGC, ECA) 
		///       doesn't store their state properly, which leads to stuck thrusters at scenario start
		///		  or at separation events
		/*
		} else if (!strnicmp (line, "THRUSTERDEMAND", 14)) {			
			int i, val;
			sscanf(line+14, "%i %i", &i, &val);
			ThrusterDemand[i] = (val != 0 ? true : false);
		*/
		}
		papiReadScenario_bool(line, "IGN1", IGN1);
		papiReadScenario_bool(line, "IGN2", IGN2);
		papiReadScenario_bool(line, "SCSLATCHUPA", SCSLatchUpA);
		papiReadScenario_bool(line, "SCSLATCHUPB", SCSLatchUpB);
		papiReadScenario_bool(line, "SPSENABLEA", SPSEnableA);
		papiReadScenario_bool(line, "SPSENABLEB", SPSEnableB);
		papiReadScenario_bool(line, "DIRECTPITCHACTIVE", DirectPitchActive); 
		papiReadScenario_bool(line, "DIRECTYAWACTIVE", DirectYawActive); 
		papiReadScenario_bool(line, "DIRECTROLLACTIVE", DirectRollActive);

		if (!strnicmp(line, "DELAYONA_BEGIN", sizeof("DELAYONA_BEGIN"))) {
			engineOnDelayA.LoadState(scn, "DELAYONA_END");
		}
		else if (!strnicmp(line, "DELAYONB_BEGIN", sizeof("DELAYONB_BEGIN"))) {
			engineOnDelayB.LoadState(scn, "DELAYONB_END");
		}
		else if (!strnicmp(line, "DELAYOFF_BEGIN", sizeof("DELAYOFF_BEGIN"))) {
			engineOffDelay.LoadState(scn, "DELAYOFF_END");
		}
	}
}

// ECA Integrator
ECAIntegrator::ECAIntegrator(double g, double ulim, double llim)
{
	gain = g;
	upperLimit = ulim;
	lowerLimit = llim;
}

void ECAIntegrator::Timestep(double input, double simdt)
{
	state += gain * input * simdt;
	if (state > upperLimit)
	{
		state = upperLimit;
	}
	else if (state < lowerLimit)
	{
		state = lowerLimit;
	}
}


//
// Electronic Control Assembly
//

ECA::ECA() :
	PitchMTVCIntegrator(0.125/0.4, 14.0*RAD, -10.0*RAD),
	YawMTVCIntegrator(0.125/0.4, 14.0*RAD, -10.0*RAD),
	PitchAutoTVCIntegrator(0.0565, 5.6*RAD, -4.7*RAD),
	YawAutoTVCIntegrator(0.0565, 5.6*RAD, -4.7*RAD)
{

	thc_x = 32768;
	thc_y = 32768;
	thc_z = 32768;

	accel_roll_trigger = 0;
	mnimp_roll_trigger = 0;
	accel_pitch_trigger = 0;
	mnimp_pitch_trigger = 0;
	accel_yaw_trigger = 0;
	mnimp_yaw_trigger = 0;
	pseudorate = _V(0,0,0);
	yawMTVCPosition = 0.0;
	pitchMTVCPosition = 0.0;

	sat = NULL;

	ResetRelays();
	ResetTransistors();
}

void ECA::Init(Saturn *vessel) {
	sat = vessel;
}

void ECA::ResetRelays()
{
	R1K22 = false;
	R1K25 = false;
	R2K11 = false;
	R2K22 = false;
	R2K25 = false;
	R2K30 = false;
	R2K31 = false;
	R3K11 = false;
	R3K22 = false;
	R3K25 = false;
	R3K30 = false;
	R3K31 = false;
}

void ECA::ResetTransistors()
{
	T1QS21 = false;
	T1QS25 = false;
	T1QS26 = false;
	T1QS28 = false;
	T1QS29 = false;
	T1QS43 = false;
	T1QS44 = false;
	T2QS1 = false;
	T2QS11 = false;
	T2QS12 = false;
	T2QS21 = false;
	T2QS25 = false;
	T2QS28 = false;
	T2QS29 = false;
	T2QS30 = false;
	T2QS31 = false;
	T2QS43 = false;
	T2QS44 = false;
	T3QS1 = false;
	T3QS11 = false;
	T3QS12 = false;
	T3QS21 = false;
	T3QS25 = false;
	T3QS28 = false;
	T3QS29 = false;
	T3QS30 = false;
	T3QS31 = false;
	T3QS43 = false;
	T3QS44 = false;
}

bool ECA::IsDCPowered() {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return false;  // Switched off

	// Ensure DC power
	if (sat->SystemMnBCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE) return false;

	return true;
}

bool ECA::IsAC1Powered()
{
	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return false;  // Switched off

	// Ensure AC power
	if (sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) return false;

	return true;
}

bool ECA::IsAC2Powered()
{
	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return false;  // Switched off

	// Ensure AC power
	if (sat->ECATVCAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) return false;

	return true;
}

void ECA::SystemTimestep(double simdt) {

	// Do we have power?
	if (IsDCPowered()) sat->SystemMnBCircuitBraker.DrawPower(14.4);
	//SCS training document just says 13.7W AC power
	if (IsAC1Powered()) sat->StabContSystemAc1CircuitBraker.DrawPower(6.85);
	if (IsAC2Powered()) sat->ECATVCAc2CircuitBraker.DrawPower(6.85);
}

void ECA::TimeStep(double simdt) {

	// SCS is in control if the THC is CLOCKWISE 
	// or if the SC CONT switch is set to SCS.

	//Logic needed for unpowered SCS state
	bool scslogic2, scslogic3, S18_2, thc_cw;

	scslogic2 = sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE;
	scslogic3 = sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE;

	S18_2 = sat->SCContSwitch.IsDown() && scslogic3;
	thc_cw = sat->THCRotary.IsClockwise() && scslogic2;

	// Do we have power?
	if (!IsDCPowered() && !IsAC1Powered() && !IsAC2Powered()) {
		// Turn off thrusters when in SCS control and unpowered
		if (S18_2 || thc_cw) {
			for (int i = 0; i < 17; i++) {
				sat->rjec.SetThruster(i, false);
			}
		}
		PitchMTVCIntegrator.Reset();
		YawMTVCIntegrator.Reset();
		PitchAutoTVCIntegrator.Reset();
		YawAutoTVCIntegrator.Reset();
		return;
	}

	//POWER

	bool E1_506, E1_509, E2_507, E2_509, EB_515;

	EB_515 = IsDCPowered();
	E1_506 = E1_509 = IsAC1Powered();
	E2_507 = E2_509 = IsAC2Powered();

	//LOGIC

	bool logic1, logic2, logic3;
	bool scslogic1, scslogic4;
	bool S7_1, S7_3, S8_1, S8_3, S9_1, S9_3, S10_2, S11_2, S12_1, S18_1, S20_1, S20_2, S20_3, S21_1, S21_2, S21_3, S22_1, S22_2, S22_3;
	bool S38_1, S38_2, S38_3, S39_1, S39_2, S39_3, S51_1, S54_1;
	bool IGN2, RHCRollBO, RHCPitchBO, RHCYawBO;

	scslogic1 = sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE;
	scslogic4 = sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE;

	S7_1 = sat->ManualAttRollSwitch.IsUp() && scslogic1;
	S7_3 = sat->ManualAttRollSwitch.IsDown() && scslogic1;
	S8_1 = sat->ManualAttPitchSwitch.IsUp() && scslogic1;
	S8_3 = sat->ManualAttPitchSwitch.IsDown() && scslogic1;
	S9_1 = sat->ManualAttYawSwitch.IsUp() && scslogic1;
	S9_3 = sat->ManualAttYawSwitch.IsDown() && scslogic1;
	S10_2 = sat->LimitCycleSwitch.IsDown() && scslogic1;
	S11_2 = sat->AttDeadbandSwitch.IsDown() && scslogic1;
	S12_1 = sat->AttRateSwitch.IsUp() && scslogic1;
	S18_1 = sat->SCContSwitch.IsUp() && scslogic2;
	
	S20_1 = sat->BMAGRollSwitch.IsUp() && scslogic2;
	S20_2 = sat->BMAGRollSwitch.IsCenter() && scslogic2;
	S20_3 = sat->BMAGRollSwitch.IsDown() && scslogic1;
	S21_1 = sat->BMAGPitchSwitch.IsUp() && scslogic3;
	S21_2 = sat->BMAGPitchSwitch.IsCenter() && scslogic3;
	S21_3 = sat->BMAGPitchSwitch.IsDown() && scslogic1;
	S22_1 = sat->BMAGYawSwitch.IsUp() && scslogic3;
	S22_2 = sat->BMAGYawSwitch.IsCenter() && scslogic3;
	S22_3 = sat->BMAGYawSwitch.IsDown() && scslogic1;
	S51_1 = sat->GSwitch.IsUp() && scslogic4;
	S38_1 = sat->SCSTvcPitchSwitch.IsUp() && scslogic3;
	S38_2 = sat->SCSTvcPitchSwitch.IsCenter() && scslogic3;
	S38_3 = sat->SCSTvcPitchSwitch.IsDown() && scslogic3;
	S39_1 = sat->SCSTvcYawSwitch.IsUp() && scslogic3;
	S39_2 = sat->SCSTvcYawSwitch.IsCenter() && scslogic3;
	S39_3 = sat->SCSTvcYawSwitch.IsDown() && scslogic3;
	S54_1 = sat->CGSwitch.IsUp() && scslogic3;
	
	IGN2 = sat->rjec.GetIGN2();
	RHCRollBO = sat->rhc1.GetPlusRollBreakoutSwitch() || sat->rhc1.GetMinusRollBreakoutSwitch() || sat->rhc2.GetPlusRollBreakoutSwitch() || sat->rhc2.GetMinusRollBreakoutSwitch();
	RHCPitchBO = sat->rhc1.GetPlusPitchBreakoutSwitch() || sat->rhc1.GetMinusPitchBreakoutSwitch() || sat->rhc2.GetPlusPitchBreakoutSwitch() || sat->rhc2.GetMinusPitchBreakoutSwitch();
	RHCYawBO = sat->rhc1.GetPlusYawBreakoutSwitch() || sat->rhc1.GetMinusYawBreakoutSwitch() || sat->rhc2.GetPlusYawBreakoutSwitch() || sat->rhc2.GetMinusYawBreakoutSwitch();

	//GYRO UNCAGE LOGIC (Module A8)
	logic1 = S20_1 || S20_3 || S51_1 || !IGN2;
	logic2 = S20_1 || S20_3 || S51_1 || RHCRollBO || (S7_1 || S7_3);

	if (!logic1 || !logic2)
	{
		T1QS21 = true;
		sat->bmag1.Uncage(0);
	}
	else
	{
		T1QS21 = false;
		sat->bmag1.Cage(0);
	}

	logic1 = S22_1 || S22_3 || S51_1 || !IGN2;
	logic2 = S22_1 || S22_3 || S51_1 || RHCYawBO || (S9_1 || S9_3);

	if (!logic1 || !logic2)
	{
		T2QS21 = true;
		sat->bmag1.Uncage(2);
	}
	else
	{
		T2QS21 = false;
		sat->bmag1.Cage(2);
	}

	logic1 = S21_1 || S21_3 || S51_1 || !IGN2;
	logic2 = S21_1 || S21_3 || S51_1 || RHCPitchBO || (S8_1 || S8_3);

	if (!logic1 || !logic2)
	{
		T3QS21 = true;
		sat->bmag1.Uncage(1);
	}
	else
	{
		T3QS21 = false;
		sat->bmag1.Cage(1);
	}

	//ROTATION CONTROL (Module A3)
	T1QS28 = S20_3;
	T1QS29 = S20_1 || S20_2;
	T2QS28 = S22_3;
	T2QS29 = S22_1 || S22_2;
	T3QS28 = S21_3;
	T3QS29 = S21_1 || S21_2;

	logic1 = S38_2 || S38_3 || !thc_cw || S18_1 || !IGN2;
	logic2 = S38_3 || S38_1 || !IGN2 || !thc_cw || S18_2;
	logic3 = S38_3 || !IGN2 || S38_1 || S18_1;

	R3K31 = (!logic1 || !logic2 || !logic3) && (S21_3);
	R3K30 = (!logic1 || !logic2 || !logic3) && (S21_1 || S21_2);

	logic1 = S39_2 || S39_3 || !thc_cw || S18_1 || !IGN2;
	logic2 = S39_3 || S39_1 || !IGN2 || !thc_cw || S18_2;
	logic3 = S39_3 || !IGN2 || S39_1 || S18_1;

	R2K31 = (!logic1 || !logic2 || !logic3) && (S22_3);
	R2K30 = (!logic1 || !logic2 || !logic3) && (S22_1 || S22_2);

	//REACTION JET CONTROL (Modules A2, A4 and A6)

	if (RHCRollBO)
		T1QS32 = true;
	else
		T1QS32 = false;

	if (RHCPitchBO)
		T3QS32 = true;
	else
		T3QS32 = false;

	if (RHCYawBO)
		T2QS32 = true;
	else
		T2QS32 = false;

	if (S10_2)
	{
		T1QS44 = true;
		T2QS44 = true;
		T3QS44 = true;
	}
	else
	{
		T1QS44 = false;
		T2QS44 = false;
		T3QS44 = false;
	}

	if (S11_2)
	{
		R1K22 = true;
		R2K22 = true;
		R3K22 = true;
	}
	else
	{
		R1K22 = false;
		R2K22 = false;
		R3K22 = false;
	}

	if (S12_1)
	{
		R1K25 = true;
		R2K25 = true;
		R3K25 = true;
	}
	else
	{
		R1K25 = false;
		R2K25 = false;
		R3K25 = false;
	}

	T1QS25 = T2QS25 = T3QS25 = S12_1 || IGN2;

	T1QS43 = S7_3 || S7_1;
	T2QS43 = S9_3 || S9_1;
	T3QS43 = S8_3 || S8_1;

	T1QS26 = S51_1;

	//THRUST VECTOR CONTROL (Modules A1 and A5)
	T2QS1 = !((S18_1 || !(S39_1 && thc_cw)) && (S39_1 || (S18_1 && !thc_cw)));

	logic1 = S18_1 || S39_1 || !IGN2;
	logic2 = !thc_cw || S39_1 || !IGN2;
	logic3 = S39_2 || S39_3 || !thc_cw || S18_1 || !IGN2;

	if (!logic1 || !logic2 || !logic3)
	{
		T2QS30 = true;
		T2QS31 = false;
	}
	else
	{
		T2QS30 = false;
		T2QS31 = true;
	}

	T3QS1 = !((S18_1 || !(S38_1 && thc_cw)) && (S38_1 || (S18_1 && !thc_cw)));

	logic1 = S18_1 || S38_1 || !IGN2;
	logic2 = !thc_cw || S38_1 || !IGN2;
	logic3 = S38_2 || S38_3 || !thc_cw || S18_1 || !IGN2;

	if (!logic1 || !logic2 || !logic3)
	{
		T3QS30 = true;
		T3QS31 = false;
	}
	else
	{
		T3QS30 = false;
		T3QS31 = true;
	}

	R2K11 = R3K11 = IGN2 && (S18_2 || thc_cw);

	if (S54_1)
	{
		T2QS11 = false;
		T3QS11 = false;
		T2QS12 = true;
		T3QS12 = true;
	}
	else
	{
		T2QS11 = true;
		T3QS11 = true;
		T2QS12 = false;
		T3QS12 = false;
	}


	int accel_roll_flag = 0;
	int mnimp_roll_flag = 0;
	int accel_pitch_flag = 0;
	int mnimp_pitch_flag = 0;
	int accel_yaw_flag = 0;
	int mnimp_yaw_flag = 0;
	VECTOR3 cmd_rate = _V(0,0,0);
	VECTOR3 rate_err = _V(0,0,0);
	VECTOR3 rhc_rate = _V(0, 0, 0);
	yawMTVCPosition = 0.0;
	pitchMTVCPosition = 0.0;
	yawAutoTVCPosition = 0.0;
	pitchAutoTVCPosition = 0.0;
	if (mnimp_roll_trigger) {
		sat->rjec.SetThruster(9,0);
		sat->rjec.SetThruster(10,0);
		sat->rjec.SetThruster(11,0);
		sat->rjec.SetThruster(12,0);
		sat->rjec.SetThruster(13,0);
		sat->rjec.SetThruster(14,0);
		sat->rjec.SetThruster(15,0);
		sat->rjec.SetThruster(16,0); 
	}
	if (mnimp_pitch_trigger) {
		sat->rjec.SetThruster(1,0);
		sat->rjec.SetThruster(2,0);
		sat->rjec.SetThruster(3,0);
		sat->rjec.SetThruster(4,0);
	}
	if (mnimp_yaw_trigger) {
		sat->rjec.SetThruster(5,0);
		sat->rjec.SetThruster(6,0);
		sat->rjec.SetThruster(7,0);
		sat->rjec.SetThruster(8,0);
	}
	// ERROR DETERMINATION
	VECTOR3 target, errors;
	if (S18_2 || thc_cw) {
		// Get BMAG1 attitude errors
		// Attitude hold automatic mode only when BMAG 1 uncaged and powered
		target.x = (E1_506 && T1QS21) ? sat->bmag1.GetAttitudeError().x : 0.0;
		target.y = (E1_506 && T3QS21) ? sat->bmag1.GetAttitudeError().y : 0.0;
		target.z = (E1_506 && T2QS21) ? sat->bmag1.GetAttitudeError().z : 0.0;
				
		// Now process
		if(target.x > 0){ // Positive Error
			if(target.x > PI){ 
				errors.x = -(TWO_PI-target.x); }else{ errors.x = target.x;	}
		}else{
			if(target.x < -PI){
				errors.x = TWO_PI+target.x; }else{ errors.x = target.x;	}
		}
		if(target.y > 0){ 
			if(target.y > PI){ 
				errors.y = TWO_PI-target.y; }else{ errors.y = -target.y;	}
		}else{
			if(target.y < -PI){
				errors.y = -(TWO_PI+target.y); }else{ errors.y = -target.y;	}
		}
		if(target.z > 0){ 
			if(target.z > PI){ 
				errors.z = -(TWO_PI-target.z); }else{ errors.z = target.z;	}
		}else{
			if(target.z < -PI){
				errors.z = TWO_PI+target.z; }else{ errors.z = target.z;	}
		}
		//Limit
		if (errors.x > 15.0*RAD) errors.x = 15.0*RAD;
		if (errors.x < -14.0*RAD) errors.x = -14.0*RAD;
		if (errors.y > 15.0*RAD) errors.y = 15.0*RAD;
		if (errors.y < -14.0*RAD) errors.y = -14.0*RAD;
		if (errors.z > 15.0*RAD) errors.z = 15.0*RAD;
		if (errors.z < -14.0*RAD) errors.z = -14.0*RAD;

		//Attitude error deadband
		if (!R1K22)
		{
			if (errors.x < -4.0 * RAD)
				cmd_rate.x = (errors.x + 4.0 * RAD);
			if (errors.x > 4.0 * RAD)
				cmd_rate.x = (errors.x - 4.0 * RAD);
		}
		else
			cmd_rate.x = errors.x;

		if (!R3K22)
		{
			if (errors.y < -4.0 * RAD)
				cmd_rate.y = (-errors.y - 4.0 * RAD);
			if (errors.y > 4.0 * RAD)
				cmd_rate.y = (-errors.y + 4.0 * RAD);
		}
		else
			cmd_rate.y = -errors.y;

		if (!R2K22)
		{
			if (errors.z < -4.0 * RAD)
				cmd_rate.z = (-errors.z - 4.0 * RAD);
			if (errors.z > 4.0 * RAD)
				cmd_rate.z = (-errors.z + 4.0 * RAD);
		}
		else
			cmd_rate.z = -errors.z;
		
		//Attitude error gain
		if (R1K25)
			cmd_rate.x *= 0.5;
		else
			cmd_rate.x *= 10.0;
		if (R3K25)
			cmd_rate.y *= 0.5;
		else
			cmd_rate.y *= 10.0;
		if (R2K25)
			cmd_rate.z *= 0.5;
		else
			cmd_rate.z *= 10.0;

		// Proportional Rate Demand 
		// The proportional rate commands are powered by AC, the signals are routed through the breakout switches, 
		// so the breakout switches must be set, too (see AOH fig. 2.3-19)

		// HIGH RATE: MAX RATE 7 dps pitch/yaw, 20 dps roll
		// LOW RATE: MAX RATE .7 dps roll/pitch/yaw 

		if (E2_507)
		{
			if (E1_509) rhc_rate.x += sat->rhc1.GetRollPropRate();
			if (E2_509) rhc_rate.x += sat->rhc2.GetRollPropRate();
			rhc_rate.x *= 9.0*RAD;
			//High Roll Rate
			if (R1K25) rhc_rate.x *= 22.0 / 9.0;

			if (E1_509) rhc_rate.y += sat->rhc1.GetPitchPropRate();
			if (E2_509) rhc_rate.y += sat->rhc2.GetPitchPropRate();
			rhc_rate.y *= 9.0*RAD;

			if (E1_509) rhc_rate.z += sat->rhc1.GetYawPropRate();
			if (E2_509) rhc_rate.z += sat->rhc2.GetYawPropRate();
			rhc_rate.z *= -9.0*RAD;
		}

		//MTVC Rate
		if (E1_509 && R3K31) rhc_rate.y -= 0.495 / 0.4*sat->bmag1.GetRates().x;
		if (E2_509 && R3K30) rhc_rate.y -= 0.495 / 0.4*sat->bmag2.GetRates().x;
		if (E1_509 && R2K31) rhc_rate.z -= 0.495 / 0.4*sat->bmag1.GetRates().y;
		if (E2_509 && R2K30) rhc_rate.z -= 0.495 / 0.4*sat->bmag2.GetRates().y;

		//MTVC Integrator
		if (E2_507 && T3QS30)
			PitchMTVCIntegrator.Timestep(rhc_rate.y, simdt);
		else
			PitchMTVCIntegrator.Reset();

		if (E2_507 && T2QS30)
			YawMTVCIntegrator.Timestep(rhc_rate.z, simdt);
		else
			YawMTVCIntegrator.Reset();

		rhc_rate.y += PitchMTVCIntegrator.GetState();
		rhc_rate.z += YawMTVCIntegrator.GetState();

		if (T1QS32)
			cmd_rate.x += rhc_rate.x;
		if (T3QS32)
			cmd_rate.y += rhc_rate.y;
		if (T2QS32)
			cmd_rate.z += rhc_rate.z;

		// RATE DAMPING
		VECTOR3 rate_damp;
		double rollrate, pitchrate, yawrate;

		// BMAG RATES are Z = ROLL, X = PITCH, Y = YAW
		rollrate = (T1QS28 ? sat->bmag1.GetRates().z : 0.0) + (T1QS29 ? sat->bmag2.GetRates().z : 0.0);
		yawrate = (T2QS28 ? sat->bmag1.GetRates().y : 0.0) + (T2QS29 ? sat->bmag2.GetRates().y : 0.0);
		pitchrate = (T3QS28 ? sat->bmag1.GetRates().x : 0.0) + (T3QS29 ? sat->bmag2.GetRates().x : 0.0);

		//Amplifier
		if (!E1_506)
		{
			rollrate = yawrate = pitchrate = 0.0;
		}

		//Low Rate
		if (!T1QS25)
			rate_damp.x = rollrate * 10.0;
		else
			rate_damp.x = rollrate;
		if (!T3QS25)
			rate_damp.y = pitchrate * 10.0;
		else
			rate_damp.y = pitchrate;
		if (!T2QS25)
			rate_damp.z = yawrate * 10.0;
		else
			rate_damp.z = yawrate;

		if (rate_damp.x > 39.0*RAD) rate_damp.x = 39.0*RAD;
		if (rate_damp.x < -33.0*RAD) rate_damp.x = -33.0*RAD;
		if (rate_damp.y > 29.0*RAD) rate_damp.y = 29.0*RAD;
		if (rate_damp.y < -25.0*RAD) rate_damp.y = -25.0*RAD;
		if (rate_damp.z > 29.0*RAD) rate_damp.z = 29.0*RAD;
		if (rate_damp.z < -25.0*RAD) rate_damp.z = -25.0*RAD;

		//Roll to yaw cross coupling
		if (T1QS26)
			rate_damp.z += -rate_damp.x * 0.38386; //tan(21.0°)

		// PSEUDORATE FEEDBACK
		if (E1_506 && !T1QS44 && sat->ManualAttRollSwitch.GetState() == THREEPOSSWITCH_CENTER){
			if (sat->rjec.GetThruster(9) || sat->rjec.GetThruster(11) ||
			    sat->rjec.GetThruster(13) || sat->rjec.GetThruster(15)) {
				pseudorate.x += 0.1 * simdt; 
			} else if (sat->rjec.GetThruster(10) || sat->rjec.GetThruster(12) ||
			    sat->rjec.GetThruster(14) || sat->rjec.GetThruster(16)) {
				pseudorate.x -= 0.1 * simdt;
			} else {
				if (pseudorate.x > 0) {
					pseudorate.x -= 0.02 * simdt; 
					pseudorate.x = max(0, pseudorate.x);
				} else {
					pseudorate.x += 0.02 * simdt; 
					pseudorate.x = min(0, pseudorate.x);
				}
			}
		} else {
			pseudorate.x = 0;
		}
		if (E1_506 && !T3QS44 && sat->ManualAttPitchSwitch.GetState() == THREEPOSSWITCH_CENTER){
			if (sat->rjec.GetThruster(1) || sat->rjec.GetThruster(3)) {
				pseudorate.y += 0.1 * simdt; 
			} else if (sat->rjec.GetThruster(2) || sat->rjec.GetThruster(4)) {
				pseudorate.y -= 0.1 * simdt;
			} else {
				if (pseudorate.y > 0) {
					pseudorate.y -= 0.02 * simdt; 
					pseudorate.y = max(0, pseudorate.y);
				} else {
					pseudorate.y += 0.02 * simdt; 
					pseudorate.y = min(0, pseudorate.y);
				}
			}
		} else {
			pseudorate.y = 0;
		}
		if (E1_506 && !T2QS44 && sat->ManualAttYawSwitch.GetState() == THREEPOSSWITCH_CENTER){
			if (sat->rjec.GetThruster(6) || sat->rjec.GetThruster(8)) {
				pseudorate.z += 0.1 * simdt; 
			} else if (sat->rjec.GetThruster(5) || sat->rjec.GetThruster(7)) {
				pseudorate.z -= 0.1 * simdt;
			} else {
				if (pseudorate.z > 0) {
					pseudorate.z -= 0.02 * simdt; 
					pseudorate.z = max(0, pseudorate.z);
				} else {
					pseudorate.z += 0.02 * simdt; 
					pseudorate.z = min(0, pseudorate.z);
				}
			}
		} else {
			pseudorate.z = 0;
		}

		// Command rates done, generate rate error values
		rate_err.x = cmd_rate.x - (rate_damp.x + pseudorate.x);
		rate_err.y = cmd_rate.y - (rate_damp.y + pseudorate.y);
		rate_err.z = cmd_rate.z - (rate_damp.z + pseudorate.z);
		
		// sprintf(oapiDebugString(),"SCS: RATE CMD r%.3f p%.3f y%.3f ERR r%.3f p%.3f y%.3f",
		//	cmd_rate.x * DEG, cmd_rate.y * DEG, cmd_rate.z * DEG, 
		//	rate_err.x * DEG, rate_err.y * DEG, rate_err.z * DEG);	
		// sprintf(oapiDebugString(),"SCS PITCH err %.4f rate %.3f cmd %.3f pseudo %.3f rate error %.3f", errors.y*DEG, pitchrate * DEG, cmd_rate.y * DEG, pseudorate.y * DEG, rate_err.y * DEG);
		// sprintf(oapiDebugString(),"SCS ROLL rate %.3f cmd %.3f pseudo %.3f rate_err %.3f errors %.3f", rollrate * DEG, cmd_rate.x * DEG, pseudorate.x * DEG, rate_err.x * DEG, errors.x * DEG);
		// sprintf(oapiDebugString(),"SCS YAW rate %.3f cmd %.3f pseudo %.3f rate_err %.3f errors %.3f", yawrate * DEG, cmd_rate.z * DEG, pseudorate.z * DEG, rate_err.z * DEG, errors.z * DEG);

		//
		// ROTATION
		//

		// Roll
		switch(sat->ManualAttRollSwitch.GetState()){
			case THREEPOSSWITCH_UP:
				// ACCEL CMD is handled in RJEC
				break;
			case THREEPOSSWITCH_CENTER:  // RATE CMD
				// Automatic mode and proportional-rate mode
				if (rate_err.x > 0.034906585) {
					// ACCEL PLUS
					sat->rjec.SetThruster(9,1);
					sat->rjec.SetThruster(11,1);
					sat->rjec.SetThruster(13,1);
					sat->rjec.SetThruster(15,1);
					sat->rjec.SetThruster(10,0);
					sat->rjec.SetThruster(12,0);
					sat->rjec.SetThruster(14,0);
					sat->rjec.SetThruster(16,0);
					accel_roll_trigger=1; accel_roll_flag=1;
				}
				if (rate_err.x < -0.034906585) {
					// ACCEL MINUS
					sat->rjec.SetThruster(10,1);
					sat->rjec.SetThruster(12,1);
					sat->rjec.SetThruster(14,1);
					sat->rjec.SetThruster(16,1);
					sat->rjec.SetThruster(9,0);
					sat->rjec.SetThruster(11,0);
					sat->rjec.SetThruster(13,0);
					sat->rjec.SetThruster(15,0);
					accel_roll_trigger=1; accel_roll_flag=-1;						
				}							
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				if (sat->rhc1.GetMinusRollBreakoutSwitch() || sat->rhc2.GetMinusRollBreakoutSwitch()) {  // MINUS
					if(!mnimp_roll_trigger){
						sat->rjec.SetThruster(10,1);
						sat->rjec.SetThruster(12,1);
						sat->rjec.SetThruster(14,1);
						sat->rjec.SetThruster(16,1);
					}
					mnimp_roll_trigger=1; mnimp_roll_flag=1;
				}
				if (sat->rhc1.GetPlusRollBreakoutSwitch() || sat->rhc2.GetPlusRollBreakoutSwitch()) { // PLUS
					if(!mnimp_roll_trigger){
						sat->rjec.SetThruster(9,1);
						sat->rjec.SetThruster(11,1);
						sat->rjec.SetThruster(13,1);
						sat->rjec.SetThruster(15,1);
					}
					mnimp_roll_trigger=1; mnimp_roll_flag=1;
				}
				break;
		}

		// Pitch
		switch(sat->ManualAttPitchSwitch.GetState()){
			case THREEPOSSWITCH_UP:
				// ACCEL CMD is handled in RJEC
				break;
			case THREEPOSSWITCH_CENTER:  // RATE CMD
				// Automatic mode and proportional-rate mode
				if(rate_err.y > 0.034906585){
					// ACCEL PLUS
					sat->rjec.SetThruster(1,1);
					sat->rjec.SetThruster(3,1);
					sat->rjec.SetThruster(2,0);
					sat->rjec.SetThruster(4,0);
					accel_pitch_trigger=1; accel_pitch_flag=1;
				}
				if(rate_err.y < -0.034906585){
					// ACCEL MINUS
					sat->rjec.SetThruster(2,1);
					sat->rjec.SetThruster(4,1);
					sat->rjec.SetThruster(1,0);
					sat->rjec.SetThruster(3,0);
					accel_pitch_trigger=1; accel_pitch_flag=-1;
				}							
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				if (sat->rhc1.GetMinusPitchBreakoutSwitch() || sat->rhc2.GetMinusPitchBreakoutSwitch()) {  // MINUS
					if(!mnimp_pitch_trigger){
						sat->rjec.SetThruster(2,1);
						sat->rjec.SetThruster(4,1);
					}
					mnimp_pitch_trigger=1; mnimp_pitch_flag=1;
				}
				if (sat->rhc1.GetPlusPitchBreakoutSwitch() || sat->rhc2.GetPlusPitchBreakoutSwitch()) { // PLUS
					if(!mnimp_pitch_trigger){
						sat->rjec.SetThruster(1,1);
						sat->rjec.SetThruster(3,1);
					}
					mnimp_pitch_trigger=1; mnimp_pitch_flag=1;
				}
				break;
		}

		// Yaw
		switch(sat->ManualAttYawSwitch.GetState()){
			case THREEPOSSWITCH_UP:
				// ACCEL CMD is handled in RJEC
				break;
			case THREEPOSSWITCH_CENTER:  // RATE CMD
				// Automatic mode and proportional-rate mode
				if(rate_err.z > 0.034906585){
					// ACCEL PLUS
					sat->rjec.SetThruster(6,1);
					sat->rjec.SetThruster(8,1);
					sat->rjec.SetThruster(5,0);
					sat->rjec.SetThruster(7,0);
					accel_yaw_trigger=1; accel_yaw_flag=-1;
				}
				if(rate_err.z < -0.034906585){
					// ACCEL MINUS
					sat->rjec.SetThruster(5,1);
					sat->rjec.SetThruster(7,1);
					sat->rjec.SetThruster(6,0);
					sat->rjec.SetThruster(8,0);
					accel_yaw_trigger=1; accel_yaw_flag=1;
				}							
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				if (sat->rhc1.GetMinusYawBreakoutSwitch() || sat->rhc2.GetMinusYawBreakoutSwitch()) {  // MINUS
					if(!mnimp_yaw_trigger){
						sat->rjec.SetThruster(6,1);
						sat->rjec.SetThruster(8,1);
					}
					mnimp_yaw_trigger=1; mnimp_yaw_flag=1;
				}
				if (sat->rhc1.GetPlusYawBreakoutSwitch() || sat->rhc2.GetPlusYawBreakoutSwitch()) { // PLUS
					if(!mnimp_yaw_trigger){
						sat->rjec.SetThruster(5,1);
						sat->rjec.SetThruster(7,1);
					}
					mnimp_yaw_trigger=1; mnimp_yaw_flag=1;
				}
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				break;
		}

		//THRUST VECTOR CONTROL

		//MTVC
		if (T2QS1)
			yawMTVCPosition = 0.4*rhc_rate.z;
		else
			yawMTVCPosition = 0.0;

		if (T3QS1)
			pitchMTVCPosition = -0.4*rhc_rate.y;
		else
			pitchMTVCPosition = 0.0;

		//Auto TVC
		double p = sat->tvsa.GetPitchGimbalPosition() - sat->tvsa.GetPitchGimbalTrim();
		double y = sat->tvsa.GetYawGimbalPosition() - sat->tvsa.GetYawGimbalTrim();

		if (E1_506 && R3K11)
			PitchAutoTVCIntegrator.Timestep(p + errors.y, simdt);
		else
			PitchAutoTVCIntegrator.Reset();

		if (E1_506 && R2K11)
			YawAutoTVCIntegrator.Timestep(y + errors.z, simdt);
		else
			YawAutoTVCIntegrator.Reset();

		if (E1_506 && E1_509)
		{
			//TBD: LM-on/off filters

			pitchAutoTVCPosition = PitchAutoTVCIntegrator.GetState() + errors.y + rate_damp.y;
			if (T3QS11)
				pitchAutoTVCPosition *= 1.0;
			else if (T3QS12)
				pitchAutoTVCPosition *= 1.0;
			else
				pitchAutoTVCPosition = 0.0;

			yawAutoTVCPosition = -(YawAutoTVCIntegrator.GetState() + errors.z + rate_damp.z);
			if (T2QS11)
				yawAutoTVCPosition *= 1.0;
			else if (T2QS12)
				yawAutoTVCPosition *= 1.0;
			else
				yawAutoTVCPosition = 0.0;
		}
		else
			pitchAutoTVCPosition = yawAutoTVCPosition = 0.0;

		//Limit
		if (pitchAutoTVCPosition > 14.0*RAD) pitchAutoTVCPosition = 14.0*RAD;
		else if (pitchAutoTVCPosition < -10.0*RAD) pitchAutoTVCPosition = -10.0*RAD;
		if (yawAutoTVCPosition > 14.0*RAD) yawAutoTVCPosition = 14.0*RAD;
		else if (yawAutoTVCPosition < -10.0*RAD) yawAutoTVCPosition = -10.0*RAD;

		//sprintf(oapiDebugString(), "Pos %.2f Cmd %.2f Trim %.1f Int %.2f Err %.2f Rate %.2f", sat->tvsa.GetPitchGimbalPosition()*DEG, pitchAutoTVCPosition*DEG, sat->tvsa.GetPitchGimbalTrim()*DEG, PitchAutoTVCIntegrator.GetState()*DEG, errors.y*DEG, rate_damp.y*DEG);
	}
	else
	{
		PitchMTVCIntegrator.Reset();
		YawMTVCIntegrator.Reset();
		PitchAutoTVCIntegrator.Reset();
		YawAutoTVCIntegrator.Reset();
	}

	// If accel thrust fired and is no longer needed, kill it.
	if(accel_roll_flag == 0 && accel_roll_trigger){
		sat->rjec.SetThruster(9,0);
		sat->rjec.SetThruster(10,0);
		sat->rjec.SetThruster(11,0);
		sat->rjec.SetThruster(12,0);
		sat->rjec.SetThruster(13,0);
		sat->rjec.SetThruster(14,0);
		sat->rjec.SetThruster(15,0);
		sat->rjec.SetThruster(16,0);
		accel_roll_trigger=0;
	}
	if(accel_pitch_flag == 0 && accel_pitch_trigger){
		sat->rjec.SetThruster(1,0);
		sat->rjec.SetThruster(2,0);
		sat->rjec.SetThruster(3,0);
		sat->rjec.SetThruster(4,0);
		accel_pitch_trigger=0;
	}
	if(accel_yaw_flag == 0 && accel_yaw_trigger){
		sat->rjec.SetThruster(5,0);
		sat->rjec.SetThruster(6,0);
		sat->rjec.SetThruster(7,0);
		sat->rjec.SetThruster(8,0);
		accel_yaw_trigger=0;
	}

	// If the joystick has gone back to center after sending our min pulse, reset the one-shot
	if(mnimp_roll_trigger && !mnimp_roll_flag){
		mnimp_roll_trigger=0;
	}
	if(mnimp_pitch_trigger && !mnimp_pitch_flag){
		mnimp_pitch_trigger=0;
	}
	if(mnimp_yaw_trigger && !mnimp_yaw_flag){
		mnimp_yaw_trigger=0;
	}
	// sprintf(oapiDebugString(),"SCS: mnimp_roll_trigger %d mnimp_roll_flag %d", mnimp_roll_trigger, mnimp_roll_flag);
}

ServoAmplifierModule::ServoAmplifierModule(bool *r1, bool *r2, bool *r3, bool *r4)
{
	relays[0] = r1;
	relays[1] = r2;
	relays[2] = r3;
	relays[3] = r4;
	sat = NULL;
}

void ServoAmplifierModule::Init(Saturn *vessel)
{
	sat = vessel;
}

bool ServoAmplifierModule::IsClutch1Powered()
{
	if (relays[0] || relays[3]) return false;
	if (sat->TVCServoPower1Switch.IsUp())
	{
		if (sat->SystemMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) return true;
	}
	else
	{
		if (sat->SystemMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) return true;
	}

	return false;
}

bool ServoAmplifierModule::IsClutch2Powered()
{
	if (!relays[1] && !relays[3]) return false;
	if (sat->TVCServoPower2Switch.IsUp())
	{
		if (sat->SystemMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) return true;
	}
	else
	{
		if (sat->SystemMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) return true;
	}

	return false;
}

void ServoAmplifierModule::DrawSystem1Power() {

	if (sat->TVCServoPower1Switch.IsUp()) {
		sat->SystemMnACircuitBraker.DrawPower(6.7);				/// CSM Data Book

	}
	else if (sat->TVCServoPower1Switch.IsDown()) {
		sat->SystemMnBCircuitBraker.DrawPower(6.7);				/// CSM Data Book
	}
}

void ServoAmplifierModule::DrawSystem2Power() {

	if (sat->TVCServoPower2Switch.IsUp()) {
		sat->SystemMnACircuitBraker.DrawPower(6.7);				/// CSM Data Book 

	}
	else if (sat->TVCServoPower2Switch.IsDown()) {
		sat->SystemMnBCircuitBraker.DrawPower(6.7);				/// CSM Data Book
	}
}

void ServoAmplifierModule::SystemTimestep(double simdt)
{
	if (IsClutch1Powered()) DrawSystem1Power();
	if (IsClutch2Powered()) DrawSystem2Power();
}

//Thrust Vector Servo Amplifier Assembly
TVSA::TVSA() :
	pitchServoAmp(&A4K1, &A4K2, &A4K3, &A4K7),
	yawServoAmp(&A4K4, &A4K5, &A4K6, &A4K8)
{
	cmcErrorCountersEnabled = false;
	cmcPitchPosition = 0.0;
	cmcYawPosition = 0.0;
	pitchGimbalTrim1 = 0.0;
	pitchGimbalTrim2 = 0.0;
	yawGimbalTrim1 = 0.0;
	yawGimbalTrim2 = 0.0;
	pitchGimbalPosition1 = 0.0;
	pitchGimbalPosition2 = 0.0;
	yawGimbalPosition1 = 0.0;
	yawGimbalPosition2 = 0.0;
	A4K1 = false;
	A4K2 = false;
	A4K3 = false;
	A4K4 = false;
	A4K5 = false;
	A4K6 = false;
	A4K7 = false;
	A4K8 = false;
	T2QS2 = false;
	T2QS3 = false;
	T3QS2 = false;
	T3QS3 = false;
}

void TVSA::Init(Saturn *vessel) {
	pitchServoAmp.Init(vessel);
	yawServoAmp.Init(vessel);
	sat = vessel;
}

void TVSA::ChangeCMCPitchPosition(double delta) {

	cmcPitchPosition += delta;
}

void TVSA::ChangeCMCYawPosition(double delta) {

	cmcYawPosition += delta;
}

void TVSA::TimeStep(double simdt)
{
	if (sat->TVCServoPower1Switch.IsCenter() && sat->TVCServoPower2Switch.IsCenter())
	{
		return;
	}
	//POWER

	bool acpower1, acpower2;

	acpower1 = IsSystem1ACPowered();
	acpower2 = IsSystem2ACPowered();

	//LOGIC

	bool logic1, logic2, scslogic1, scslogic2, scslogic3, S18_1, S27_2, S27_3, S28_2, S28_3, S38_1, S39_1, thc_cw, IGN2, yawovercur, pitchovercur;

	scslogic1 = sat->SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE;
	scslogic2 = sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE;
	scslogic3 = sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE;

	S18_1 = sat->SCContSwitch.IsUp() && scslogic2;
	S27_2 = sat->TVCGimbalDrivePitchSwitch.IsCenter() && scslogic1;
	S27_3 = sat->TVCGimbalDrivePitchSwitch.IsDown() && scslogic3;
	S28_2 = sat->TVCGimbalDriveYawSwitch.IsCenter() && scslogic1;
	S28_3 = sat->TVCGimbalDriveYawSwitch.IsDown() && scslogic3;
	S38_1 = sat->SCSTvcPitchSwitch.IsUp() && scslogic3;
	S39_1 = sat->SCSTvcYawSwitch.IsUp() && scslogic3;
	thc_cw = sat->THCRotary.IsClockwise() && scslogic2;
	IGN2 = sat->rjec.GetIGN2();
	pitchovercur = sat->SPSEngine.pitchGimbalActuator.HasGimbalMotorOverCurrent();
	yawovercur = sat->SPSEngine.yawGimbalActuator.HasGimbalMotorOverCurrent();

	logic1 = !(S18_1 && !thc_cw);
	T2QS2 = logic1 && (!((thc_cw && !S18_1) || !(S39_1 && IGN2)));
	T2QS3 = logic1;

	logic1 = !(S18_1 && !thc_cw);
	T3QS2 = logic1 && (!((thc_cw && !S18_1) || !(S38_1 && IGN2)));
	T3QS3 = logic1;

	logic1 = S27_2 || S27_3;
	logic2 = S27_3 || pitchovercur || thc_cw;

	if (logic1 && logic2)
	{
		A4K1 = true;
		A4K2 = true;
		A4K3 = true;
		A4K7 = true;
	}
	else
	{
		A4K1 = false;
		A4K2 = false;
		A4K3 = false;
		A4K7 = false;
	}

	logic1 = S28_2 || S28_3;
	logic2 = S28_3 || yawovercur || thc_cw;

	if (logic1 && logic2)
	{
		A4K4 = true;
		A4K5 = true;
		A4K6 = true;
		A4K8 = true;
	}
	else
	{
		A4K4 = false;
		A4K5 = false;
		A4K6 = false;
		A4K8 = false;
	}

	//Trim
	if (acpower1)
	{
		pitchGimbalTrim1 = (sat->SPSGimbalPitchThumbwheel.GetState() - 8.0) / 2.0*RAD;
		yawGimbalTrim1 = (sat->SPSGimbalYawThumbwheel.GetState() - 8.0) / 2.0*RAD;
	}
	else
		pitchGimbalTrim1 = yawGimbalTrim1 = 0.0;

	if (acpower2)
	{
		pitchGimbalTrim2 = (sat->SPSGimbalPitchThumbwheel.GetState() - 8.0) / 2.0*RAD;
		yawGimbalTrim2 = (sat->SPSGimbalYawThumbwheel.GetState() - 8.0) / 2.0*RAD;
	}
	else
		pitchGimbalTrim2 = yawGimbalTrim2 = 0.0;

	if (acpower1)
	{
		double curPitchPosition1, curYawPosition1, PitchServo1Position, YawServo1Position;

		//Position transducers
		curPitchPosition1 = sat->SPSEngine.pitchGimbalActuator.GetCommandedPosition();
		curYawPosition1 = sat->SPSEngine.yawGimbalActuator.GetCommandedPosition();

		//Pitch Servo No. 1
		if (T3QS3)
		{
			PitchServo1Position = sat->eca.GetPitchMTVCPosition() + (T3QS2 ? sat->eca.GetPitchAutoTVCPosition() : 0.0) + (T3QS3 ? pitchGimbalTrim1 : 0.0) + cmcPitchPosition;
		}

		//Yaw Servo No. 1
		if (T2QS3)
		{
			YawServo1Position = sat->eca.GetYawMTVCPosition() + (T2QS2 ? sat->eca.GetYawAutoTVCPosition() : 0.0) + (T2QS3 ? yawGimbalTrim1 : 0.0) + cmcYawPosition;
		}

		if (pitchServoAmp.IsClutch1Powered())
		{
			sat->SPSEngine.pitchGimbalActuator.CommandedPositionInc(PitchServo1Position - curPitchPosition1);
		}
		if (yawServoAmp.IsClutch1Powered())
		{
			sat->SPSEngine.yawGimbalActuator.CommandedPositionInc(YawServo1Position - curYawPosition1);
		}
	}

	if (acpower2)
	{
		double curPitchPosition2, curYawPosition2, PitchServo2Position, YawServo2Position;

		curPitchPosition2 = sat->SPSEngine.pitchGimbalActuator.GetCommandedPosition();
		curYawPosition2 = sat->SPSEngine.yawGimbalActuator.GetCommandedPosition();

		//Pitch Servo No. 2
		if (acpower2 && T3QS3)
		{
			PitchServo2Position = sat->eca.GetPitchMTVCPosition() + (T3QS2 ? sat->eca.GetPitchAutoTVCPosition() : 0.0) + (T3QS3 ? pitchGimbalTrim2 : 0.0) + cmcPitchPosition;
		}

		//Yaw Servo No. 2
		if (acpower2 && T2QS3)
		{
			YawServo2Position = sat->eca.GetYawMTVCPosition() + (T2QS2 ? sat->eca.GetYawAutoTVCPosition() : 0.0) + (T2QS3 ? yawGimbalTrim2 : 0.0) + cmcYawPosition;
		}

		if (pitchServoAmp.IsClutch2Powered())
		{
			sat->SPSEngine.pitchGimbalActuator.CommandedPositionInc(PitchServo2Position - curPitchPosition2);
		}

		if (yawServoAmp.IsClutch2Powered())
		{
			sat->SPSEngine.yawGimbalActuator.CommandedPositionInc(YawServo2Position - curYawPosition2);
		}
	}

	//Transducers
	if (acpower1)
	{
		pitchGimbalPosition1 = sat->SPSEngine.pitchGimbalActuator.GetPosition()*RAD;
		yawGimbalPosition1 = sat->SPSEngine.yawGimbalActuator.GetPosition()*RAD;
	}
	else
		pitchGimbalPosition1 = yawGimbalPosition1 = 0.0;

	if (acpower2)
	{
		pitchGimbalPosition2 = sat->SPSEngine.pitchGimbalActuator.GetPosition()*RAD;
		yawGimbalPosition2 = sat->SPSEngine.yawGimbalActuator.GetPosition()*RAD;
	}
	else
		pitchGimbalPosition2 = yawGimbalPosition2 = 0.0;
}

void TVSA::SystemTimestep(double simdt)
{
	//DC
	pitchServoAmp.SystemTimestep(simdt);
	yawServoAmp.SystemTimestep(simdt);
	//AC
	if (IsSystem1ACPowered()) DrawSystem1ACPower();
	if (IsSystem2ACPowered()) DrawSystem2ACPower();
}

double TVSA::GetPitchGimbalTrim()
{
	if (A4K1)
		return pitchGimbalTrim2;

	return pitchGimbalTrim1;
}

double TVSA::GetYawGimbalTrim()
{
	if (A4K4)
		return yawGimbalTrim2;

	return yawGimbalTrim1;
}

double TVSA::GetPitchGimbalPosition()
{
	if (A4K3)
		return pitchGimbalPosition2;

	return pitchGimbalPosition1;
}

double TVSA::GetYawGimbalPosition()
{
	if (A4K6)
		return yawGimbalPosition2;

	return yawGimbalPosition1;
}

bool TVSA::IsSystem1ACPowered()
{
	if (sat->TVCServoPower1Switch.IsUp())
	{
		if (sat->StabContSystemTVCAc1CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) return true;
	}
	else if (sat->TVCServoPower1Switch.IsDown())
	{
		if (sat->ECATVCAc2CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) return true;
	}

	return false;
}

bool TVSA::IsSystem2ACPowered()
{
	if (sat->TVCServoPower2Switch.IsUp())
	{
		if (sat->StabContSystemTVCAc1CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) return true;
	}
	else if (sat->TVCServoPower2Switch.IsDown())
	{
		if (sat->ECATVCAc2CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) return true;
	}

	return false;
}

void TVSA::DrawSystem1ACPower() {

	if (sat->TVCServoPower1Switch.IsUp()) {
		sat->StabContSystemTVCAc1CircuitBraker.DrawPower(1.36);	// Systems handbook
	}
	else if (sat->TVCServoPower1Switch.IsDown()) {
		sat->StabContSystemAc2CircuitBraker.DrawPower(1.36);	// Systems handbook
	}
}

void TVSA::DrawSystem2ACPower() {

	if (sat->TVCServoPower2Switch.IsUp()) {
		sat->StabContSystemAc1CircuitBraker.DrawPower(1.36);	// Systems handbook
	}
	else if (sat->TVCServoPower2Switch.IsDown()) {
		sat->ECATVCAc2CircuitBraker.DrawPower(1.36);			// Systems handbook
	}
}

void TVSA::SaveState(FILEHANDLE scn) {

	bool arr[8] = { A4K1, A4K2, A4K3, A4K4, A4K5, A4K6, A4K7, A4K8 };

	oapiWriteLine(scn, TVSA_START_STRING);

	papiWriteScenario_boolarr(scn, "RELAYS", arr, 8);

	oapiWriteLine(scn, TVSA_END_STRING);
}

void TVSA::LoadState(FILEHANDLE scn) {

	char *line;
	bool r[8];

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, TVSA_END_STRING, sizeof(TVSA_END_STRING))) {
			break;
		}

		papiReadScenario_boolarr(line, "RELAYS", r, 8);
	}

	A4K1 = r[0];
	A4K2 = r[1];
	A4K3 = r[2];
	A4K4 = r[3];
	A4K5 = r[4];
	A4K6 = r[5];
	A4K7 = r[6];
	A4K8 = r[7];
}


// Entry Monitor System

#define EMS_STATUS_OFF			0
#define EMS_STATUS_STANDBY		1
#define EMS_STATUS_DVSET		2
#define EMS_STATUS_VHFRNG		3
#define EMS_STATUS_DV			4
#define EMS_STATUS_DV_BACKUP	5
#define EMS_STATUS_DVTEST		6
#define EMS_STATUS_DVTEST_DONE	7
#define EMS_STATUS_RNG_SET		8
#define EMS_STATUS_Vo_SET		9
#define EMS_STATUS_ENTRY		10
#define EMS_STATUS_EMS_TEST1	11
#define EMS_STATUS_EMS_TEST2	12
#define EMS_STATUS_EMS_TEST3	13
#define EMS_STATUS_EMS_TEST4	14
#define EMS_STATUS_EMS_TEST5	15

extern GDIParams g_Param;

//#define IDS_WINDOWS_BITMAP_FILES			_T("Windows Bitmap Files (*.BMP; *.DIB)|*.BMP; *.DIB||" )
//#define IDS_FILE_CREATE_ERROR_MESSAGE		_T("Cannot create the requested file. Please ensure that the path name is valid and that you have write access to the destination drive and try again." )
//#define DEFAULT_BITMAP_FILE_EXTENSION		_T("bmp")
//#define DEFAULT_BITMAP_FILE_NAME			_T("EMS_Scroll")



EMS::EMS(PanelSDK &p) : DCPower(0, p) {

	status = EMS_STATUS_OFF;
	dVInitialized = false;
	lastWeight = _V(0, 0, 0);
	lastGlobalVel = _V(0, 0, 0);
	dVRangeCounter = 0;
	dVTestTime = 0;
	sat = NULL;
	DimmerRotationalSwitch = NULL;
	SlewScribe = 0;
	ScrollPosition = 0;
	GScribe = 1;
	xacc = 9.81;
	xaccG = 1.0;
	constG = 9.7939; //Set initial value
	RSIRotation = PI/2;
	RSITarget = 0;
	vinert = 37000.0;

	switchchangereset = false;

	pt05GLightOn = false;
	pt05GFailed = false;
	LiftVectLightOn = 0;

	ThresholdBreeched = false;
	ThresholdBreechTime = 0.0;
	CorridorEvaluated = false;
	OneSecTimer = 1.0;
	TenSecTimer = 10.0;
	InitialTrip = false;

	ScribePntCnt = 1;
	ScribePntArray[0].x = 40;
	ScribePntArray[0].y = 1;

	//Initial position of RSI Triangle
	RSITriangle[0].x = EMS_RSI_CENTER_X + (int)(cos(RSIRotation)*28);
	RSITriangle[0].y = EMS_RSI_CENTER_Y - (int)(sin(RSIRotation)*28);
	RSITriangle[1].x = EMS_RSI_CENTER_X + (int)(cos(RSIRotation)*8) - (int)(sin(RSIRotation)*8);
	RSITriangle[1].y = EMS_RSI_CENTER_Y - (int)(cos(RSIRotation)*16) + (int)(sin(RSIRotation)*16);
	RSITriangle[2].x = EMS_RSI_CENTER_X - (int)(cos(RSIRotation)*8) + (int)(sin(RSIRotation)*8);
	RSITriangle[2].y = EMS_RSI_CENTER_Y - (int)(cos(RSIRotation)*16) + (int)(sin(RSIRotation)*16);
	//Center of Rotation is 42,42

	//
	//  For proper scaling....
		/*	The scroll bitmap is 2500 pixels in length, and 100 feet per sec on the dV scale is equililent to 3 pixels on the bitmap.
			On the real scroll, 480 feet per sec is roughly equal to .263 inches of scroll.  (AOH-v1-Spacecraft Control System, pg 2.3-58)
			Therefore, the scaling factor between pixels and inches of scroll length is 0.018263888888888888888888888888889
			Or...      the scaling factor between pixels and ft/sec is 0.03*/
	ScrollBitmapLength = 2500; //Pixels
	ScrollBitmapHeight = 145; //Pixels
	ScrollScaling = 0.03; // pixels per ft/sec
}

void EMS::Init(Saturn *vessel, e_object *a, e_object *b, RotationalSwitch *dimmer, e_object *c) {
	sat = vessel;
	DCPower.WireToBuses(a, b);
	WireTo(c);
	DimmerRotationalSwitch = dimmer;
}

void EMS::TimeStep(double MissionTime, double simdt) {

	double position;
	double dV;

	//Accelerometer Timestep
	AccelerometerTimeStep(simdt);

	xaccG = xacc/constG;
	dV = xacc * simdt * FPS;

	if (!IsPowered()) {
		status = EMS_STATUS_OFF; 
		return;
	}
	
	if (switchchangereset) {
		switchchangereset = false;
		pt05GLightOn = false;
		LiftVectLightOn = 0;
	}

	// Turn on reset
	if (status == EMS_STATUS_OFF && sat->EMSFunctionSwitch.GetState() != 0) {
		SwitchChanged();
	}

	switch (status) {
		case EMS_STATUS_DV:
		case EMS_STATUS_DV_BACKUP:
			// dV/Range display
			if (xacc > 0 || status == EMS_STATUS_DV) {
				dVRangeCounter -= dV;
				dVRangeCounter = max(-1000.0, min(14000.0, dVRangeCounter));
			}				
			//sprintf(oapiDebugString(), "xacc %.10f", xacc);
			break;

		case EMS_STATUS_DVSET:
			position = sat->EMSDvSetSwitch.GetPosition();
			if (position == 1)
				dVRangeCounter += 127.5 * simdt;
			else if (position == 2)
				dVRangeCounter += 0.25 * simdt;
			else if (position == 3)
				dVRangeCounter -= 127.5 * simdt;
			else if (position == 4)
				dVRangeCounter -= 0.25 * simdt;
			break;

		case EMS_STATUS_DVTEST:
			dVTestTime -= simdt;
			dVRangeCounter -= 5.*constG * simdt * FPS;	// AOH SCS fig. 2.3-13

			if (dVTestTime < 0 || dVRangeCounter < -1000.0) {
				if (dVTestTime < -0.176875) {	// Minimum -41.5 when starting with 1586.8
					dVRangeCounter -= (dVTestTime + 0.176875) * 160.0;
				}
				dVTestTime = 0;
				status = EMS_STATUS_DVTEST_DONE;
			}
			break;
		
		case EMS_STATUS_ENTRY:

			if (ThresholdBreeched) { // if .05G comparator has been tripped

				if (pt02GComparator(simdt) && !Manual05GInit()) ThresholdBreeched = false;

				double dV_res = 0.948*dV; //Resolution factor from RTCC requirements for reentry phase: https://archive.org/download/nasa_techdoc_19740074547/19740074547.pdf
				ScrollPosition = ScrollPosition + (dV_res * ScrollScaling); //Rough conversion of ft/sec to pixels of scroll
				vinert -= dV_res;

				if (vinert < 0.0) //Did the EMS integrator have the ability to count below 0? If yes, range goes up again.
				{
					vinert = 0.0;
				}
				dVRangeCounter -= 0.000162*(vinert + 0.5*dV_res) * simdt; //Also from the RTCC document

				pt05GLightOn = true;
				if (!Manual05GInit())
				{
					if (!CorridorEvaluated) {
						TenSecTimer -= simdt;
						if (TenSecTimer < 0.0) {
							LiftVectLightOn = VerifyCorridor();
							CorridorEvaluated = true;
						}
					}
					else {
						if (xaccG > 2) LiftVectLightOn = 0;
					}
				}
			}
			else
			{
				if (pt05GComparator(simdt) || Manual05GInit()) ThresholdBreeched = true;
				TenSecTimer = 10.0;
			}

			break;
		
		case EMS_STATUS_Vo_SET:
			position = sat->EMSDvSetSwitch.GetPosition();
			if (position == 1)
			{
				ScrollPosition += 480 * ScrollScaling * simdt; 
				vinert -= 480 * simdt;
			}
			else if (position == 2)
			{
				ScrollPosition += 30 * ScrollScaling * simdt;
				vinert -= 30 * simdt;
			}
			else if (position == 3 && (MaxScrollPosition - ScrollPosition) <= 40.0)
			{
				ScrollPosition -= 480 * ScrollScaling * simdt;
				vinert += 480 * simdt;
			}
			else if (position == 4 && (MaxScrollPosition - ScrollPosition) <= 40.0)
			{
				ScrollPosition -= 30 * ScrollScaling * simdt;
				vinert += 30 * simdt;
			}
			break;

		case EMS_STATUS_RNG_SET:
			position = sat->EMSDvSetSwitch.GetPosition();
			if (position == 1)
				dVRangeCounter += 127.5 * simdt;
			else if (position == 2)
				dVRangeCounter += 0.25 * simdt;
			else if (position == 3)
				dVRangeCounter -= 127.5 * simdt;
			else if (position == 4)
				dVRangeCounter -= 0.25 * simdt;
			break;

		case EMS_STATUS_EMS_TEST1:
			position = sat->EMSDvSetSwitch.GetPosition();
			if (position == 1)
				ScrollPosition += 480*ScrollScaling * simdt;
			else if (position == 2)
				ScrollPosition += 30*ScrollScaling * simdt;
			else if (position == 3 && (MaxScrollPosition-ScrollPosition)<=40.0)
				ScrollPosition -= 480*ScrollScaling * simdt;
			else if (position == 4 && (MaxScrollPosition-ScrollPosition)<=40.0)
				ScrollPosition -= 30*ScrollScaling * simdt;
			pt05GLightOn = false;
			dVRangeCounter = 0;
			break;

		case EMS_STATUS_EMS_TEST2:
			if ((TenSecTimer -= simdt) < 0.0) pt05GLightOn = true;;
			break;

		case EMS_STATUS_EMS_TEST3:
			vinert = 37000.0;
			position = sat->EMSDvSetSwitch.GetPosition();
			if (position == 1)
				dVRangeCounter += 127.5 * simdt;
			else if (position == 2)
				dVRangeCounter += 0.25 * simdt;
			else if (position == 3)
				dVRangeCounter -= 127.5 * simdt;
			else if (position == 4)
				dVRangeCounter -= 0.25 * simdt;
			pt05GLightOn = true;
			if ((TenSecTimer -= simdt) < 0.0) LiftVectLightOn = -1;
			break;

		case EMS_STATUS_EMS_TEST4:
			if (TenSecTimer > 0.0) {
				xaccG = min(25. * (1. - (TenSecTimer / 10.)), 9.);
				TenSecTimer -= simdt;
				dV = 7. * constG * simdt * FPS;
				ScrollPosition=ScrollPosition+(dV*ScrollScaling); 
				dVRangeCounter -= 5.8 * simdt;
			} else {
				xaccG = 9;
			}
			pt05GLightOn = true;
			break;

		case EMS_STATUS_EMS_TEST5:
			vinert = 37000.0;
			position = sat->EMSDvSetSwitch.GetPosition();
			if (position == 1)
				ScrollPosition += 480*ScrollScaling * simdt;
			else if (position == 2)
				ScrollPosition += 30*ScrollScaling * simdt;
			else if (position == 3 && (MaxScrollPosition-ScrollPosition)<=40.0)
				ScrollPosition -= 480*ScrollScaling * simdt;
			else if (position == 4 && (MaxScrollPosition-ScrollPosition)<=40.0)
				ScrollPosition -= 30*ScrollScaling * simdt;
			pt05GLightOn = true;
			xaccG = 0.28;
			dVRangeCounter = 0;
			if ((TenSecTimer -= simdt) < 0.0) LiftVectLightOn = 1;
			break;
		case EMS_STATUS_VHFRNG:
			dVRangeCounter = sat->vhfranging.GetRange();
			break;
	}

	// If powered, drive Glevel
	if (status == EMS_STATUS_ENTRY || 
		status == EMS_STATUS_EMS_TEST1 || status == EMS_STATUS_EMS_TEST4 || status == EMS_STATUS_EMS_TEST5 || 
		status == EMS_STATUS_RNG_SET || status == EMS_STATUS_Vo_SET) {

		// AOH SCS fig. 2.3-34
		if (status != EMS_STATUS_Vo_SET && status != EMS_STATUS_EMS_TEST1) {
			GScribe = (int)(xaccG * 14.) + 1; // 13.6 vertical pixels per G
		}

		// Limit readouts
		dVRangeCounter = max(-1000.0, min(14000.0, dVRangeCounter));
		ScrollPosition = max(0.0, min(ScrollBitmapLength, ScrollPosition));

		// Limit reversing of scroll when in nominal operation by tracking Max position
		if (sat->GTASwitch.IsUp() && sat->GetStage() >= CM_STAGE) {
			MaxScrollPosition = ScrollPosition;
		}else{
			if (ScrollPosition > MaxScrollPosition) {MaxScrollPosition = ScrollPosition;};
		}

		SlewScribe = (int)(ScrollPosition) + 40; //Offset of 40 to shift the drawing correctly

		if (SlewScribe != ScribePntArray[ScribePntCnt-1].x || GScribe != ScribePntArray[ScribePntCnt-1].y) { //If either x or y has changed, add new point to trace
			if (ScribePntCnt < EMS_SCROLL_LENGTH_PX*3) ScribePntCnt++;
		}
		ScribePntArray[ScribePntCnt-1].y = GScribe;
		ScribePntArray[ScribePntCnt-1].x = SlewScribe;

		//sprintf(oapiDebugString(), "ScribePt %d %d %d", ScribePntCnt, ScribePntArray[ScribePntCnt-1].x, ScribePntArray[ScribePntCnt-1].y);
		//sprintf(oapiDebugString(), "ScrollPosition %f", ScrollPosition);
	}

	RotateRSI(simdt);
}

void EMS::SystemTimestep(double simdt) {

	if (IsPowered() && !IsOff()) {
		DCPower.DrawPower(93.28);	// see CSM Systems Handbook
	}

	if (IsDisplayPowered() && !IsOff()) {
		DrawPower(0.022);
	}
}

void EMS::AccelerometerTimeStep(double simdt) {

	VECTOR3 arot, w, vel;

	sat->GetGlobalOrientation(arot);
	sat->GetWeightVector(w);
	sat->GetGlobalVel(vel);

	MATRIX3	tinv = AttitudeReference::GetRotationMatrixZ(-arot.z);
	tinv = mul(AttitudeReference::GetRotationMatrixY(-arot.y), tinv);
	tinv = mul(AttitudeReference::GetRotationMatrixX(-arot.x), tinv);
	w = mul(tinv, w) / sat->GetMass();

	//Orbiter 2016 hack
	if (length(w) == 0.0)
	{
		w = GetGravityVector();
	}

	if (!dVInitialized) {
		lastWeight = w;
		lastGlobalVel = vel;
		lastSimDT = simdt;
		dVInitialized = true;

	} else {
		// Acceleration calculation, see IMU
		VECTOR3 dvel = (vel - lastGlobalVel) / lastSimDT;
		VECTOR3 dw1 = w - dvel;
		VECTOR3 dw2 = lastWeight - dvel;
		lastWeight = w;
		lastGlobalVel = vel;
		lastSimDT = simdt;

		// Transform to vessel coordinates
		MATRIX3	t = AttitudeReference::GetRotationMatrixX(arot.x);
		t = mul(AttitudeReference::GetRotationMatrixY(arot.y), t);
		t = mul(AttitudeReference::GetRotationMatrixZ(arot.z), t);
		VECTOR3 avg = (dw1 + dw2) / 2.0;
		avg = mul(t, avg);	
		xacc = -avg.z;
		// Ground test switch
	
		constG = 9.7916;		// the Virtual AGC needs nonspherical gravity anyway

		if (sat->GTASwitch.IsUp()) {
			xacc -= constG;
		}
	}

}

VECTOR3 EMS::GetGravityVector()
{
	OBJHANDLE gravref = sat->GetGravityRef();
	OBJHANDLE hSun = oapiGetObjectByName("Sun");
	VECTOR3 R, U_R;
	sat->GetRelativePos(gravref, R);
	U_R = unit(R);
	double r = length(R);
	VECTOR3 R_S, U_R_S;
	sat->GetRelativePos(hSun, R_S);
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
		sat->GetRelativePos(hEarth, R_Ea);
		U_R_E = unit(R_Ea);
		double r_E = length(R_Ea);
		double mu_E = GGRAV * oapiGetMass(hEarth);

		a_dP -= U_R_E * mu_E / pow(r_E, 2.0);
	}

	return a_dP;
}

void EMS::SwitchChanged() {

	if (!IsPowered()) return;

	// Turn on reset
	if (status == EMS_STATUS_OFF && sat->EMSFunctionSwitch.GetState() != 0) {
		dVRangeCounter = 0;
	}

	switch (sat->EMSFunctionSwitch.GetState()) {
		case 0: // OFF
			status = EMS_STATUS_OFF;
			break;

		case 1: // dV
			if (sat->EMSModeSwitch.IsUp()) {
				status = EMS_STATUS_DV;
				dVInitialized = false;
			} else if (sat->EMSModeSwitch.IsCenter()) {
				status = EMS_STATUS_STANDBY;
			} else {
				status = EMS_STATUS_DV_BACKUP;
				dVInitialized = false;
			}
			break;

		case 2: // dV SET/VHF RNG
			if (sat->EMSModeSwitch.IsDown()) {
				status = EMS_STATUS_VHFRNG;
			} else {
				status = EMS_STATUS_DVSET;
			}
			break;

		case 3: // dV TEST
			if (sat->EMSModeSwitch.IsUp()) {
				status = EMS_STATUS_DVTEST;
				dVTestTime = 10.0;
			} else {
				status = EMS_STATUS_STANDBY;
			}
			break;

		case 4: // ENTRY
			if (sat->EMSModeSwitch.IsCenter()) {
				status = EMS_STATUS_STANDBY;
			} else {
				status = EMS_STATUS_ENTRY;
			}
			break;
		case 5: // Vo SET
			status = EMS_STATUS_Vo_SET;
			break;
		case 6: // RNG SET
			status = EMS_STATUS_RNG_SET;
			break;
		case 7: // TEST 5
			TenSecTimer = 10.0;
			status = EMS_STATUS_EMS_TEST5;
			break;
		case 8: // TEST 4
			TenSecTimer = 10.0;
			status = EMS_STATUS_EMS_TEST4;
			break;
		case 9: // TEST 3
			TenSecTimer = 10.0;
			status = EMS_STATUS_EMS_TEST3;
			break;
		case 10: // TEST 2
			TenSecTimer = 10.0;
			status = EMS_STATUS_EMS_TEST2;
			break;
		case 11: // TEST 1
			status = EMS_STATUS_EMS_TEST1;
			break;

		default:
			status = EMS_STATUS_STANDBY;
			break;
	}

	switchchangereset=true;
	// sprintf(oapiDebugString(),"EMSFunctionSwitch %d", sat->EMSFunctionSwitch.GetState());
}

void EMS::SetRSIDeltaRotation(double dangle)
{
	RSITarget += dangle;

	RSITarget = fmod(RSITarget + TWO_PI, TWO_PI); //remove unwanted multiples of 2PI and avoid negative numbers
}

void EMS::RotateRSI(double simdt) {

	double cRSIrot,sRSIrot;
	double error,dtheta;
	int dir; // + : counterclockwise  - : clockwise

	if (RSITarget > RSIRotation) {
		dir = 1;
	}else{
		dir = -1;
	}
	if (fabs(RSITarget - RSIRotation) > PI) dir = -dir;  // Deal with sign crossing of 0/360 value

	dtheta = (50.*(PI/180.)*simdt); //Hardcoded rotation rate of 50 degrees per second...unknown actual value.
	error = fabs(RSITarget - RSIRotation); //Difference between desired and current value
	if (error < dtheta) dtheta = error; //use smaller of two

	RSIRotation = RSIRotation + dir*dtheta;

	// Check Limits and keep within 0-2PI bounds
	if (RSIRotation > (2*PI)) RSIRotation = RSIRotation - 2*PI;
	if (RSIRotation < 0) RSIRotation = RSIRotation + 2*PI;

	//sprintf(oapiDebugString(),"RSITarget:%f  RSIRotation:%f dir:%d", RSITarget,RSIRotation,dir);

	cRSIrot=cos(RSIRotation);
	sRSIrot=sin(RSIRotation);

	RSITriangle[0].x = EMS_RSI_CENTER_X + (int)(sRSIrot*28);
	RSITriangle[0].y = EMS_RSI_CENTER_Y + (int)(-cRSIrot*28);
	RSITriangle[1].x = EMS_RSI_CENTER_X + (int)(-sRSIrot*16 + cRSIrot*8);
	RSITriangle[1].y = EMS_RSI_CENTER_Y + (int)(sRSIrot*8 + cRSIrot*16);
	RSITriangle[2].x = EMS_RSI_CENTER_X + (int)(-sRSIrot*16 - cRSIrot*8);
	RSITriangle[2].y = EMS_RSI_CENTER_Y + (int)(-sRSIrot*8 + cRSIrot*16);

}

bool EMS::pt05GComparator(double simdt) {

	if (xaccG >= 0.05 && !InitialTrip){
		InitialTrip = true;
		OneSecTimer = 1.0;
		return false;
	} else if (xaccG >= 0.05 && InitialTrip){
		OneSecTimer -= simdt;
	} else {
		InitialTrip = false;
		OneSecTimer = 0.0;
	}
	if (OneSecTimer < 0.0) {
		return true;
	}

	return false;
}
bool EMS::pt02GComparator(double simdt) {

	if (xaccG < 0.02 && ThresholdBreeched) {
		return true;
	}
	return false;
}

bool EMS::Manual05GInit() {
	if (sat->EMSModeSwitch.IsDown() && status == EMS_STATUS_ENTRY)
	{
		return true;
	}
	return false;
}

short int EMS::VerifyCorridor() {
	if (xaccG > .262) return 1;
	if (xaccG < .262) return -1;
	return 0;
}

bool EMS::SPSThrustLight() {

	if (!IsPowered()) return false;	
	
	if (status == EMS_STATUS_DVTEST) return true;
	if (sat->SPSEngine.IsThrustOnA() || sat->SPSEngine.IsThrustOnB()) return true;
	return false;
}

bool EMS::pt05GLight() {

	if (!IsPowered()) return false;
	if (pt05GFailed) return false;
	if (pt05GLightOn) return true;
	return false;
}

int EMS::LiftVectLight() {

	if (!IsPowered()) return 0;
	return LiftVectLightOn;
}

bool EMS::IsOff() {

	if (status == EMS_STATUS_OFF) return true;
	return false;
}

bool EMS::IsdVMode() {

	if (status == EMS_STATUS_DV) return true;
	if (status == EMS_STATUS_DV_BACKUP) return true;
	return false;
}

bool EMS::IsDecimalPointBlanked()
{
	if (status == EMS_STATUS_VHFRNG) return true;

	return false;
}

bool EMS::IsPowered() {

	return DCPower.Voltage() > SP_MIN_DCVOLTAGE; 
}

bool EMS::IsDisplayPowered() {

	if (Voltage() < SP_MIN_ACVOLTAGE || DimmerRotationalSwitch->GetState() == 0)
		return false;

	return true;
}

void EMS::SaveState(FILEHANDLE scn) {
	char buffer[100];
	
	oapiWriteLine(scn, EMS_START_STRING);
	oapiWriteScenario_int(scn, "STATUS", status);
	oapiWriteScenario_int(scn, "DVINITIALIZED", (dVInitialized ? 1 : 0));
	papiWriteScenario_vec(scn, "LASTWEIGHT", lastWeight);
	papiWriteScenario_vec(scn, "LASTGLOBALVEL", lastGlobalVel);
	papiWriteScenario_double(scn, "LASTSIMDT", lastSimDT);
	papiWriteScenario_double(scn, "DVRANGECOUNTER", dVRangeCounter);
	papiWriteScenario_double(scn, "VINERTIAL", vinert);
	papiWriteScenario_double(scn, "DVTESTTIME", dVTestTime);
	papiWriteScenario_double(scn, "RSIROTATION", RSIRotation);
	papiWriteScenario_double(scn, "RSITARGET", RSITarget);
	papiWriteScenario_double(scn, "SCROLLPOSITION", ScrollPosition);
	oapiWriteScenario_int(scn, "THRESHOLDBREECHED", (ThresholdBreeched ? 1 : 0));
	oapiWriteScenario_int(scn, "CORRIDOREVALUATED", (CorridorEvaluated ? 1 : 0));
	papiWriteScenario_double(scn, "TENSECTIMER", TenSecTimer);
	oapiWriteScenario_int(scn, "LIFTVECTLTON", LiftVectLightOn);
	oapiWriteScenario_int(scn, "SCRIBEPNTCNT", ScribePntCnt);
	for (int i = 0; i < ScribePntCnt; i++) {
		sprintf(buffer, "SCRIBEPNTARRAY%i %i %i", i, ScribePntArray[i].x, ScribePntArray[i].y);
		oapiWriteScenario_string(scn, buffer, "");
	}	
	oapiWriteLine(scn, EMS_END_STRING);
}

void EMS::LoadState(FILEHANDLE scn) {

	int i;
	long j, k;
	char *line;	

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, EMS_END_STRING, sizeof(EMS_END_STRING))){
			return;
		}

		if (!strnicmp (line, "STATUS", 6)) {
			sscanf(line + 6, "%i", &status);
		} else if (!strnicmp (line, "DVINITIALIZED", 13)) {
			sscanf(line + 13, "%i", &i);
			dVInitialized = (i == 1);
		} else if (!strnicmp (line, "LASTWEIGHT", 10)) {
			sscanf(line + 10, "%lf %lf %lf", &lastWeight.x, &lastWeight.y, &lastWeight.z);
		} else if (!strnicmp (line, "LASTGLOBALVEL", 13)) {
			sscanf(line + 13, "%lf %lf %lf", &lastGlobalVel.x, &lastGlobalVel.y, &lastGlobalVel.z);
		} else if (!strnicmp(line, "LASTSIMDT", 9)) {
			sscanf(line + 9, "%lf", &lastSimDT);
		} else if (!strnicmp (line, "DVRANGECOUNTER", 14)) {
			sscanf(line + 14, "%lf", &dVRangeCounter);
		} else if (!strnicmp (line, "VINERTIAL", 9)) {
			sscanf(line + 9, "%lf", &vinert);
		} else if (!strnicmp(line, "DVTESTTIME", 10)) {
			sscanf(line + 10, "%lf", &dVTestTime);
		} else if (!strnicmp(line, "RSIROTATION", 11)) {
			sscanf(line + 11, "%lf", &RSIRotation);
		} else if (!strnicmp (line, "RSITARGET", 9)) {
			sscanf(line + 9, "%lf", &RSITarget);
		} else if (!strnicmp (line, "SCROLLPOSITION", 14)) {
			sscanf(line + 14, "%lf", &ScrollPosition);
		} else if (!strnicmp (line, "THRESHOLDBREECHED", 17)) {
			sscanf(line + 17, "%i", &i);
			ThresholdBreeched = (i == 1);
		} else if (!strnicmp (line, "CORRIDOREVALUATED", 17)) {
			sscanf(line + 17, "%i", &i);
			CorridorEvaluated = (i == 1);
		} else if (!strnicmp (line, "TENSECTIMER", 11)) {
			sscanf(line + 11, "%lf", &TenSecTimer);
		} else if (!strnicmp (line, "LIFTVECTLTON", 12)) {
			sscanf(line + 12, "%hd", &LiftVectLightOn);
		} 
		else if (papiReadScenario_int(line, "SCRIBEPNTCNT", ScribePntCnt));
		else if (!strnicmp (line, "SCRIBEPNTARRAY", 14)) {
			sscanf(line + 14, "%i %li %li", &i, &j, &k);
			ScribePntArray[i].x = j;
			ScribePntArray[i].y = k;
		}
	}
}

bool EMS::WriteScrollToFile() {
	char sdate[9];
	char stime[9];
	char buffer[100];

	//Special Thanks to computerex at orbiter-forum for assisting in this implementation
	int width = EMS_SCROLL_LENGTH_PX;
	int height = EMS_SCROLL_LENGTH_PY;

	/////////////////////////////////////////////////////////
    // Get the drawing surface, apply the scribe line and create a corresponding 
    // bitmap with the same dimensions

	HDC hMemDC = CreateCompatibleDC(0);
	HBITMAP hBitmap = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE (IDB_EMS_SCROLL_LEO));
	HGDIOBJ hOld = SelectObject(hMemDC, hBitmap);

	// Draw Commands
	SetBkMode(hMemDC, TRANSPARENT);
	HGDIOBJ oldObj = SelectObject(hMemDC, g_Param.pen[5]);
	Polyline(hMemDC, ScribePntArray, ScribePntCnt);

	SelectObject(hMemDC, oldObj);
	SelectObject(hMemDC, hOld);

	PBITMAPINFO bitmapInfo = CreateBitmapInfoStruct(hBitmap);

	_strdate(sdate);
	_strtime(stime);
	sprintf(buffer, "ProjectApollo EMSScroll %s_%s.bmp", sdate, stime);
	for (unsigned int i = 0; i < strlen(buffer); i++) {
		if (buffer[i] == '/' || buffer[i] == ':') {
			buffer[i] = '-';
		}
	}

	bool ret = true;
	ret = CreateBMPFile(buffer, bitmapInfo, hBitmap, hMemDC);

	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	return ret;
}

// The following code was found and supplied by computerex at orbiter-forum.  The code is from the MSDN sample code library.
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
	BITMAP      bmp; 
	PBITMAPINFO pbmi; 
	WORD        cClrBits; 

	// Retrieve the bitmap color format, width, and height. 
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) {
		return NULL;
	}

	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits  = 1; 
	else if (cClrBits <= 4) 
		cClrBits  = 4; 
	else if (cClrBits <= 8) 
		cClrBits  = 8; 
	else if (cClrBits <= 16) 
		cClrBits  = 16; 
	else if (cClrBits <= 24) 
		cClrBits  = 24; 
	else cClrBits = 32; 

	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * (1<< cClrBits)); 
	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER)); 

	pbmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth    = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight   = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes   = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
	pbmi->bmiHeader.biCompression = BI_RGB; 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbmi->bmiHeader.biHeight; 
	pbmi->bmiHeader.biClrImportant = 0; 
	return pbmi; 
}

bool CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC) { 

	HANDLE hf;                 // file handle 
	BITMAPFILEHEADER hdr;       // bitmap file-header 
	PBITMAPINFOHEADER pbih;     // bitmap info-header 
	LPBYTE lpBits;              // memory pointer 
	DWORD dwTotal;              // total count of bytes 
	DWORD cb;                   // incremental count of bytes 
	BYTE *hp;                   // byte pointer 
	DWORD dwTmp; 

	pbih = (PBITMAPINFOHEADER) pbi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) {
	   return false;
	}

	// Retrieve the color table (RGBQUAD array) and the bits 
	// (array of palette indices) from the DIB. 
	if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
	   DIB_RGB_COLORS)) {
		   return false;
	}

	// Create the .BMP file. 
	hf = CreateFile(pszFile, 
	   GENERIC_READ | GENERIC_WRITE, 
	   (DWORD) 0, 
	   NULL, 
	   CREATE_ALWAYS, 
	   FILE_ATTRIBUTE_NORMAL, 
	   (HANDLE) NULL); 
	if (hf == INVALID_HANDLE_VALUE) {
	   return false;
	}
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
	   pbih->biSize + pbih->biClrUsed 
	   * sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 

	// Compute the offset to the array of color indices. 
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
	   pbih->biSize + pbih->biClrUsed 
	   * sizeof (RGBQUAD); 

	// Copy the BITMAPFILEHEADER into the .BMP file. 
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
	   (LPDWORD) &dwTmp,  NULL)) 
	{
	   return false;
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
	   + pbih->biClrUsed * sizeof (RGBQUAD), 
	   (LPDWORD) &dwTmp, NULL)) {
		   return false;
	}

	// Copy the array of color indices into the .BMP file. 
	dwTotal = cb = pbih->biSizeImage; 
	hp = lpBits; 
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) {
	   return false;
	}

	// Close the .BMP file. 
	if (!CloseHandle(hf)) {
	   return false;
	}

	// Free memory. 
	GlobalFree((HGLOBAL)lpBits);
	return true;
}