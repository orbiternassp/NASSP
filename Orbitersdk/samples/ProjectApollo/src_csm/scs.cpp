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
	targetAttitude = _V(0,0,0);
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
	AttitudeReference::Init(v);
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
	AttitudeReference::Timestep(simdt);

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

/*	if (number == 1) {
		sprintf(oapiDebugString(),"BMAG Att x %.3f y %.3f z %.3f Tar x %.3f y %.3f z %.3f Err x %.3f y %.3f z %.3f",  
			Attitude.x * DEG, Attitude.y * DEG, Attitude.z * DEG,
			targetAttitude.x * DEG, targetAttitude.y * DEG, targetAttitude.z * DEG,
			(targetAttitude.x - Attitude.x) * DEG, (targetAttitude.y - Attitude.y) * DEG, (targetAttitude.z - Attitude.z) * DEG);

		sprintf(oapiDebugString(),"BMAG Err x %.3f y %.3f z %.3f", 
			errorAttitude.x * DEG, errorAttitude.y * DEG, errorAttitude.z * DEG);
	}
*/
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
		targetAttitude.data[axis] = Attitude.data[axis];
		uncaged.data[axis] = 1;

		errorAttitude.data[axis] = 0;
	}
}

VECTOR3 BMAG::GetAttitudeError() {

	if (!SCS_INERTIAL_BMAGS)
		return errorAttitude;

	VECTOR3 err = _V(0, 0, 0);
	if (powered) 
	{
		if (uncaged.x == 1) 
			err.x = targetAttitude.x - Attitude.x ;

		if (uncaged.y == 1) 
			err.y = targetAttitude.y - Attitude.y ;

		if (uncaged.z == 1) 
			err.z = targetAttitude.z - Attitude.z;
	}
	return err; 
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
	papiWriteScenario_double(scn, "TARGETATTITUDEX", targetAttitude.x);
	papiWriteScenario_double(scn, "TARGETATTITUDEY", targetAttitude.y);
	papiWriteScenario_double(scn, "TARGETATTITUDEZ", targetAttitude.z);
	papiWriteScenario_double(scn, "RATESX", rates.x);
	papiWriteScenario_double(scn, "RATESY", rates.y);
	papiWriteScenario_double(scn, "RATESZ", rates.z);
	papiWriteScenario_vec(scn, "ERRORATTITUDE", errorAttitude);
	
	AttitudeReference::SaveState(scn);

	oapiWriteLine(scn, BMAG_END_STRING);
}

void BMAG::LoadState(FILEHANDLE scn){

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, BMAG_END_STRING, sizeof(BMAG_END_STRING))){
			return;
		}

		if (!strnicmp (line, "UNCAGEDX", 8)) {
			sscanf(line + 8, "%lf", &uncaged.x);
		}
		else if (!strnicmp (line, "UNCAGEDY", 8)) {
			sscanf(line + 8, "%lf", &uncaged.y);
		}
		else if (!strnicmp (line, "UNCAGEDZ", 8)) {
			sscanf(line + 8, "%lf", &uncaged.z);
		}
		else if (!strnicmp (line, "TARGETATTITUDEX", 15)) {
			sscanf(line + 15, "%lf", &targetAttitude.x);
		}
		else if (!strnicmp (line, "TARGETATTITUDEY", 15)) {
			sscanf(line + 15, "%lf", &targetAttitude.y);
		}
		else if (!strnicmp (line, "TARGETATTITUDEZ", 15)) {
			sscanf(line + 15, "%lf", &targetAttitude.z);
		}
		else if (!strnicmp (line, "RATESX", 6)) {
			sscanf(line + 6, "%lf", &rates.x);
		}
		else if (!strnicmp (line, "RATESY", 6)) {
			sscanf(line + 6, "%lf", &rates.y);
		}
		else if (!strnicmp (line, "RATESZ", 6)) {
			sscanf(line + 6, "%lf", &rates.z);
		}
		else if (papiReadScenario_vec(line, "ERRORATTITUDE", errorAttitude));
		else {
			AttitudeReference::LoadState(line);
		}
	}
}


// GDC

GDC::GDC()
{
	rates = _V(0,0,0);	
	sat = NULL;
	fdai_err_ena = 0;
	fdai_err_x = 0;
	fdai_err_y = 0;
	fdai_err_z = 0;
	rsiRotationOn = false;
	rsiRotationStart = 0;
	rollstabilityrate = 0;
}

void GDC::Init(Saturn *v)
{
	sat = v;
	AttitudeReference::Init(v);
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

	// Get rates from the appropriate BMAG
	// GDC attitude is based on RATE data, not ATT data.
	BMAG *rollBmag = NULL;
	BMAG *pitchBmag = NULL;
	BMAG *yawBmag = NULL;

	switch(sat->BMAGRollSwitch.GetState()){
		case THREEPOSSWITCH_UP:     // RATE2
			rollBmag = &sat->bmag2;
			sat->bmag1.Cage(0);
			break;

		case THREEPOSSWITCH_CENTER: // RATE2/ATT1
			rollBmag = &sat->bmag2;
	
			// Uncage Roll BMAG 1, see AOH SCS figure 2.3-11
			if (((sat->ManualAttRollSwitch.GetState() == THREEPOSSWITCH_CENTER && sat->eca.rhc_x > 28673 && 
				  sat->eca.rhc_x < 36863) || sat->rjec.GetSPSActive()) && sat->GSwitch.IsDown()) {
				sat->bmag1.Uncage(0);
			} else {
				sat->bmag1.Cage(0);
			}
			break;

		case THREEPOSSWITCH_DOWN:   // RATE1
			rollBmag = &sat->bmag1;
			sat->bmag1.Cage(0);
			break;			
	}

	switch(sat->BMAGPitchSwitch.GetState()){
		case THREEPOSSWITCH_UP:     // RATE2
			pitchBmag = &sat->bmag2;
			sat->bmag1.Cage(1);
			break;

		case THREEPOSSWITCH_CENTER: // RATE2/ATT1
			pitchBmag = &sat->bmag2;

			// Uncage Pitch BMAG 1, see AOH SCS figure 2.3-11
			if (((sat->ManualAttPitchSwitch.GetState() == THREEPOSSWITCH_CENTER && sat->eca.rhc_y > 28673 && 
				  sat->eca.rhc_y < 36863) || sat->rjec.GetSPSActive()) && sat->GSwitch.IsDown()) {
				sat->bmag1.Uncage(1);
			} else {
				sat->bmag1.Cage(1);
			}
			break;

		case THREEPOSSWITCH_DOWN:   // RATE1
			pitchBmag = &sat->bmag1;
			sat->bmag1.Cage(1);
			break;			
	}

	switch(sat->BMAGYawSwitch.GetState()){
		case THREEPOSSWITCH_UP:     // RATE2
			yawBmag = &sat->bmag2;
			sat->bmag1.Cage(2);
			break;

		case THREEPOSSWITCH_CENTER: // RATE2/ATT1
			yawBmag = &sat->bmag2;

			// Uncage Yaw BMAG 1, see AOH SCS figure 2.3-11
			if (((sat->ManualAttYawSwitch.GetState() == THREEPOSSWITCH_CENTER && sat->eca.rhc_z > 28673 && 
				  sat->eca.rhc_z < 36863) || sat->rjec.GetSPSActive()) && sat->GSwitch.IsDown()) {
				sat->bmag1.Uncage(2);
			} else {
				sat->bmag1.Cage(2);
			}
			break;

		case THREEPOSSWITCH_DOWN:   // RATE1
			yawBmag = &sat->bmag1;
			sat->bmag1.Cage(2);
			break;
	}		
	
	// Handling for .05G Switch:  According to AOH SCS Section 2.3.3.3.1...
	// "...selecting backup rate (BMAG1) in yaw will automatically select the backup rate gyro (BMAG 1) in roll and vice versa..."
	if (sat->GSwitch.IsUp() && (sat->BMAGPitchSwitch.IsDown() || sat->BMAGRollSwitch.IsDown())) {
		rollBmag = &sat->bmag1;
		yawBmag = &sat->bmag1;
	}

	AttitudeReference::Timestep(simdt);

	rates.x = pitchBmag->GetRates().x;
	// Special Logic for Entry .05 Switch
	if (sat->GSwitch.IsUp()) {
		// Entry Stability Roll Transformation
		rates.y = -rollBmag->GetRates().z * tan(21.0 * RAD) + yawBmag->GetRates().y;
		rollstabilityrate = rollBmag->GetRates().z*cos(21.0*RAD) + yawBmag->GetRates().y*sin(21.0*RAD);
		// sprintf(oapiDebugString(), "entry roll rate? %f", rates.y);
	} else {
		// Normal Operation
		rates.y = yawBmag->GetRates().y;
		rollstabilityrate = rollBmag->GetRates().z;
	}
	rates.z = rollBmag->GetRates().z;

	// If the current BMAG has no power it doesn't provide rates so we don't change 
	// the attitude of the failed axis
	if (!rollBmag->IsPowered())  SetAttitude(_V(LastAttitude.x, Attitude.y, Attitude.z));
	if (!pitchBmag->IsPowered()) SetAttitude(_V(Attitude.x, LastAttitude.y, Attitude.z));
	if (!yawBmag->IsPowered())   SetAttitude(_V(Attitude.x, Attitude.y, LastAttitude.z));

	// Do we have power?
	/// \todo DC power is needed, too
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2 ||
	    sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE || 
		sat->StabContSystemAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) {

		// Reset Attitude
		SetAttitude(_V(0, 0, 0));		
	}

	// GDCAlign button
	if (sat->GDCAlignButton.GetState() == 1) {
		AlignGDC();
	} else {
		rsiRotationOn = false;
	}
}

bool GDC::AlignGDC() {
	// User pushed the Align GDC button.
	// Set the GDC attitude to match what's on the ASCP.
	if (sat->FDAIAttSetSwitch.IsDown()) {
		SetAttitude(_V(sat->ascp.output.x * RAD,
					   sat->ascp.output.y * RAD,
					   sat->ascp.output.z * RAD)); // Degrees to radians


		// Align both BMAGs to the GDC as the GDC gets its attitude data from them
		sat->bmag1.SetAttitude(_V(sat->ascp.output.x * RAD,
								  sat->ascp.output.y * RAD,
								  sat->ascp.output.z * RAD));

		sat->bmag2.SetAttitude(_V(sat->ascp.output.x * RAD,
								  sat->ascp.output.y * RAD,
								  sat->ascp.output.z * RAD));

		// The RSI isn't set to the yaw ASCP setting, but changing when the ASCP yaw setting changes (and the Align GDC button is held down etc.)
		if (sat->EMSRollSwitch.IsUp() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE) {
			if (rsiRotationOn) {
				sat->ems.SetRSIRotation((sat->ascp.output.z * RAD) - rsiRotationStart);
			} else {
				rsiRotationOn = true;
				rsiRotationStart = (sat->ascp.output.z * RAD) - sat->ems.GetRSIRotation(); 
			}
		} else {
			rsiRotationOn = false;
		}
		return true;
	} else {
		rsiRotationOn = false;
	}
	return false;	
}

void GDC::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, GDC_START_STRING);
	oapiWriteScenario_int(scn, "FDAIERRENA", fdai_err_ena);
	oapiWriteScenario_int(scn, "FDAIERRX", fdai_err_x);
	oapiWriteScenario_int(scn, "FDAIERRY", fdai_err_y);
	oapiWriteScenario_int(scn, "FDAIERRZ", fdai_err_z);
	papiWriteScenario_bool(scn, "RSIROTATIONON", rsiRotationOn);	
	papiWriteScenario_double(scn, "RSIROTATIONSTART", rsiRotationStart);

	AttitudeReference::SaveState(scn);

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
		else if (papiReadScenario_bool(line, "RSIROTATIONON", rsiRotationOn));
		else if (papiReadScenario_double(line, "RSIROTATIONSTART", rsiRotationStart));
		else {
			AttitudeReference::LoadState(line);
		}
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
}

void EDA::Init(Saturn *vessel){
	sat = vessel;
}

VECTOR3 EDA::ReturnCMCErrorNeedles(){
	VECTOR3 errors;

	if (sat->FDAIScaleSwitch.IsDown()) {
		// 15 degree max
		///\todo should be 50/15/15 degree max, i.e. fdai_err_x * 0.032031, but for unknown reasons the AGC stops at 15 deg (Colossus version dependent?)
		errors.x = sat->gdc.fdai_err_x * 0.106770; // CMC error value, CMC-scaled
		errors.y = sat->gdc.fdai_err_y * 0.106770; // CMC error value, CMC-scaled
		errors.z = sat->gdc.fdai_err_z * 0.106770; // CMC error value, CMC-scaled
	} else {
		// 5 degree max
		errors.x = sat->gdc.fdai_err_x * 0.32031; // CMC error value, CMC-scaled
		errors.y = sat->gdc.fdai_err_y * 0.32031; // CMC error value, CMC-scaled
		errors.z = sat->gdc.fdai_err_z * 0.32031; // CMC error value, CMC-scaled
	}	
	return(errors);
}

VECTOR3 EDA::ReturnASCPError(VECTOR3 attitude) {

	VECTOR3 setting,target,errors;
	// Get ASCP setting in radians
	setting.x = sat->ascp.output.x * 0.017453;
	setting.y = sat->ascp.output.y * 0.017453;
	setting.z = sat->ascp.output.z * 0.017453;
	// And difference
	target.x = setting.x - attitude.x;
	target.y = setting.y - attitude.y;
	target.z = setting.z - attitude.z;							
	// Now process
	switch(sat->FDAIScaleSwitch.GetState()){
		case THREEPOSSWITCH_UP:
		case THREEPOSSWITCH_CENTER:
			// 5 degree rate
			if(target.x > 0){ // Positive Error
				if(target.x > PI){ 
					errors.x = -((TWO_PI-target.x) * 469.827882); }else{
						errors.x = (target.x * 469.827882);	}
			}else{
				if(target.x < -PI){
					errors.x = ((TWO_PI+target.x) * 469.827882); }else{
						errors.x = (target.x * 469.827882);	}
			}
			if(target.y > 0){ 
				if(target.y > PI){ 
					errors.y = ((TWO_PI-target.y) * 469.827882); }else{
						errors.y = -(target.y * 469.827882);	}
			}else{
				if(target.y < -PI){
					errors.y = -((TWO_PI+target.y) * 469.827882); }else{
						errors.y = -(target.y * 469.827882);	}
			}
			if(target.z > 0){ 
				if(target.z > PI){ 
					errors.z = -((TWO_PI-target.z) * 469.827882); }else{
						errors.z = (target.z * 469.827882);	}
			}else{
				if(target.z < -PI){
					errors.z = ((TWO_PI+target.z) * 469.827882); }else{
						errors.z = (target.z * 469.827882);	}
			}											
			break;
		case THREEPOSSWITCH_DOWN:
			// 50/15/15 degree rate
			if(target.x > 0){ // Positive Error
				if(target.x > PI){ 
					errors.x = -((TWO_PI-target.x) * 46.982572); }else{
						errors.x = (target.x * 46.982572);	}
			}else{
				if(target.x < -PI){
					errors.x = ((TWO_PI+target.x) * 46.982572); }else{
						errors.x = (target.x * 46.982572);	}
			}
			if(target.y > 0){ 
				if(target.y > PI){ 
					errors.y = ((TWO_PI-target.y) * 156.608695); }else{
						errors.y = -(target.y * 156.608695);	}
			}else{
				if(target.y < -PI){
					errors.y = -((TWO_PI+target.y) * 156.608695); }else{
						errors.y = -(target.y * 156.608695);	}
			}
			if(target.z > 0){ 
				if(target.z > PI){ 
					errors.z = -((TWO_PI-target.z) * 156.608695); }else{
						errors.z = (target.z * 156.608695);	}
			}else{
				if(target.z < -PI){
					errors.z = ((TWO_PI+target.z) * 156.608695); }else{
						errors.z = (target.z * 156.608695);	}
			}											
			break;
	}
	return(errors);
}

VECTOR3 EDA::ReturnBMAG1Error() {

	VECTOR3 target = sat->bmag1.GetAttitudeError();
	return CalcErrors(target); 
}

VECTOR3 EDA::CalcErrors(VECTOR3 target)

{
	VECTOR3 errors = _V(0, 0, 0);

	switch(sat->FDAIScaleSwitch.GetState()){
		case THREEPOSSWITCH_UP:
		case THREEPOSSWITCH_CENTER:
			// 5 degree rate
			if (target.x > 0) { // Positive Error
				if (target.x > PI) { 
					errors.x = -((TWO_PI-target.x) * 469.827882); 
				} else {
					errors.x = (target.x * 469.827882);	
				}
			} else {
				if (target.x < -PI) {
					errors.x = ((TWO_PI+target.x) * 469.827882); 
				} else {
					errors.x = (target.x * 469.827882);	
				}
			}
			if (target.y > 0) { 
				if (target.y > PI) { 
					errors.y = ((TWO_PI-target.y) * 469.827882); 
				} else {
					errors.y = -(target.y * 469.827882);
				}
			} else {
				if (target.y < -PI) {
					errors.y = -((TWO_PI+target.y) * 469.827882); 
				} else {
					errors.y = -(target.y * 469.827882);	
				}
			}
			if (target.z > 0) { 
				if (target.z > PI) { 
					errors.z = -((TWO_PI-target.z) * 469.827882); 
				} else {
					errors.z = (target.z * 469.827882);	
				}
			} else {
				if (target.z < -PI) {
					errors.z = ((TWO_PI+target.z) * 469.827882); 
				} else {
					errors.z = (target.z * 469.827882);	
				}
			}											
			break;
		case THREEPOSSWITCH_DOWN:
			// 50/15/15 degree rate
			if (target.x > 0){ // Positive Error
				if (target.x > PI) { 
					errors.x = -((TWO_PI-target.x) * 46.982572); 
				} else {
					errors.x = (target.x * 46.982572);	
				}
			} else {
				if (target.x < -PI){
					errors.x = ((TWO_PI+target.x) * 46.982572); 
				} else {
					errors.x = (target.x * 46.982572);	
				}
			}
			if (target.y > 0) { 
				if (target.y > PI) { 
					errors.y = ((TWO_PI-target.y) * 156.608695); 
				} else {
					errors.y = -(target.y * 156.608695);	
				}
			} else {
				if (target.y < -PI) {
					errors.y = -((TWO_PI+target.y) * 156.608695); 
				} else {
					errors.y = -(target.y * 156.608695);	
				}
			}
			if (target.z > 0) { 
				if (target.z > PI) { 
					errors.z = -((TWO_PI-target.z) * 156.608695); 
				} else {
					errors.z = (target.z * 156.608695);	
				}
			} else {
				if (target.z < -PI) {
					errors.z = ((TWO_PI+target.z) * 156.608695); 
				} else {
					errors.z = (target.z * 156.608695);	
				}
			}											
			break;
	}
	return(errors);
}

VECTOR3 EDA::AdjustErrorsForRoll(VECTOR3 attitude, VECTOR3 errors)

{
	VECTOR3 output_errors;
	double input_pitch = errors.y;
	double input_yaw = errors.z;
	double roll_percent,output_pitch,output_yaw,pitch_factor = 1;
	// In reality, PITCH and YAW are swapped around as needed to make the error needles  FLY-TO.
	// This does that.
	// ROLL IS LEFT-HANDED
	if(attitude.x == 0){ // If zero or inop, return unmodified to avoid SPECIAL CASE
		return(errors);
	}
	if(attitude.x > 4.712388){                    // 0 thru 90 degrees
		roll_percent = fabs((attitude.x-TWO_PI) / 1.570796);				
		output_pitch = input_pitch * (1-roll_percent); 
		output_pitch += input_yaw * roll_percent;
		output_yaw = input_yaw * (1-roll_percent);
		output_yaw -=input_pitch * roll_percent;       
	}
	if(attitude.x > PI && attitude.x < 4.712388){ // 90 thru 180 degrees
		roll_percent = (attitude.x-PI) / 1.570796;					
		output_pitch = -(input_pitch * (1-roll_percent)); 
		output_pitch += input_yaw * roll_percent;
		output_yaw = -input_yaw * (1-roll_percent);
		output_yaw -=input_pitch * roll_percent;       
	}
	if(attitude.x > 1.570796 && attitude.x < PI){ // 180 thru 270 degrees
		roll_percent = fabs((attitude.x-PI) / 1.570796);
		output_pitch = -(input_pitch * (1-roll_percent)); 
		output_pitch -= input_yaw * roll_percent;
		output_yaw = -input_yaw * (1-roll_percent);
		output_yaw +=input_pitch * roll_percent;       
	}
	if(attitude.x > 0 && attitude.x < 1.570796){ // 270 thru 360 degrees
		roll_percent = attitude.x / 1.570796;					
		output_pitch = input_pitch * (1-roll_percent); 
		output_pitch -= input_yaw * roll_percent;
		output_yaw = input_yaw * (1-roll_percent);
		output_yaw +=input_pitch * roll_percent;       
	}

	//sprintf(oapiDebugString(),"Roll Att %f Percent = %f | P-I %f P-O %f | Y-I %f Y-O %f",
	//	attitude.x,roll_percent,input_pitch,output_pitch,input_yaw,output_yaw);

	output_errors.x = errors.x;
	output_errors.y = output_pitch;
	output_errors.z = output_yaw;
	return(output_errors);
}

// Reaction Jet / Engine Control
RJEC::RJEC() {
	
	sat = NULL;
	SPSActive = false;
	DirectPitchActive = false;
	DirectYawActive = false;
	DirectRollActive = false;

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

	if (sat->ManualAttRollSwitch.GetState() == THREEPOSSWITCH_UP) {		
		if (sat->eca.rhc_x < 28673) {  // MINUS
			td[10] = true;
			td[12] = true;
			td[14] = true;
			td[16] = true;
		}
		if (sat->eca.rhc_x > 36863) { // PLUS
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

	if (sat->ManualAttPitchSwitch.GetState() == THREEPOSSWITCH_UP) {		
		if (sat->eca.rhc_y < 28673) {  // MINUS
			td[2] = true;
			td[4] = true;
		}
		if (sat->eca.rhc_y > 36863) { // PLUS
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
	
	if (sat->ManualAttYawSwitch.GetState() == THREEPOSSWITCH_UP) {		
		if (sat->eca.rhc_z < 28673) {  // MINUS
			td[6] = true;
			td[8] = true;
		}
		if (sat->eca.rhc_z > 36863) { // PLUS
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

	if ((sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN || sat->THCRotary.IsClockwise()) && !sm_sep) {
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
		if (thruster < 9 && SPSActive) {
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
	papiWriteScenario_bool(scn, "SPSACTIVE", SPSActive); 
	papiWriteScenario_bool(scn, "DIRECTPITCHACTIVE", DirectPitchActive); 
	papiWriteScenario_bool(scn, "DIRECTYAWACTIVE", DirectYawActive); 
	papiWriteScenario_bool(scn, "DIRECTROLLACTIVE", DirectRollActive); 

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
		papiReadScenario_bool(line, "SPSACTIVE", SPSActive); 
		papiReadScenario_bool(line, "DIRECTPITCHACTIVE", DirectPitchActive); 
		papiReadScenario_bool(line, "DIRECTYAWACTIVE", DirectYawActive); 
		papiReadScenario_bool(line, "DIRECTROLLACTIVE", DirectRollActive); 
	}
}


//
// Electronic Control Assembly
//

ECA::ECA() {

	rhc_x = 32768;
	rhc_y = 32768;
	rhc_z = 32768;
	rhc_ac_x = 32768;
	rhc_ac_y = 32768;
	rhc_ac_z = 32768;
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

	sat = NULL;
}

void ECA::Init(Saturn *vessel) {
	sat = vessel;
}

bool ECA::IsPowered() {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return false;  // Switched off

	// Ensure DC power
	if (sat->SystemMnACircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || 
	    sat->SystemMnBCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE) return false;

	return true;
}

void ECA::SystemTimestep(double simdt) {

	// Do we have power?
	if (!IsPowered()) return;

	sat->SystemMnACircuitBraker.DrawPower(10);	/// \todo Real power is unknown
	sat->SystemMnBCircuitBraker.DrawPower(10);	/// \todo Real power is unknown
}

void ECA::TimeStep(double simdt) {

	// SCS is in control if the THC is CLOCKWISE 
	// or if the SC CONT switch is set to SCS.

	/// \todo TVC CW is supplied by SCS LOGIC BUS 2
	/// \todo SC CONT switch is supplied by ???

	// Do we have power?
	if (!IsPowered()) {
		// Turn off thrusters when in SCS control and unpowered
		if (sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN || sat->THCRotary.IsClockwise()) {
			for (int i = 0; i < 17; i++) {
				sat->rjec.SetThruster(i, false);
			}
		}
		return;
	}

	int accel_roll_flag = 0;
	int mnimp_roll_flag = 0;
	int accel_pitch_flag = 0;
	int mnimp_pitch_flag = 0;
	int accel_yaw_flag = 0;
	int mnimp_yaw_flag = 0;
	VECTOR3 cmd_rate = _V(0,0,0);
	VECTOR3 rate_err = _V(0,0,0);
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
	if (sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN || sat->THCRotary.IsClockwise()) {
		// Get BMAG1 attitude errors
		target = sat->bmag1.GetAttitudeError();
				
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
		// Now adjust for rotation
		if (SCS_INERTIAL_BMAGS)
			errors = sat->eda.AdjustErrorsForRoll(sat->bmag1.GetAttitude(), errors);

		// Create demand for rate
		switch(sat->AttRateSwitch.GetState()){
			case TOGGLESWITCH_UP:   // HIGH RATE
				// Are we in or out of deadband?
				switch(sat->AttDeadbandSwitch.GetState()){
					case TOGGLESWITCH_UP:   // MAX
						// 8 degrees attitude deadband
						// 4 degree "real" deadband, 2 degree is accomplished by the rate deadband, so we need factor 2 here					
						if (errors.x < -4.0 * RAD)
							cmd_rate.x = (errors.x + 4.0 * RAD) / 2.0;
						if (errors.x > 4.0 * RAD)
							cmd_rate.x = (errors.x - 4.0 * RAD) / 2.0;
						if (errors.y < -4.0 * RAD)
							cmd_rate.y = (-errors.y - 4.0 * RAD) / 2.0;
						if (errors.y > 4.0 * RAD)
							cmd_rate.y = (-errors.y + 4.0 * RAD) / 2.0;
						if (errors.z < -4.0 * RAD)
							cmd_rate.z = (-errors.z - 4.0 * RAD) / 2.0;
						if (errors.z > 4.0 * RAD)
							cmd_rate.z = (-errors.z + 4.0 * RAD) / 2.0;
						break;
					case TOGGLESWITCH_DOWN: // MIN
						// 4 degrees attitude deadband
						// 2 degree is accomplished by the rate deadband, so we need factor 2 here
						cmd_rate.x = errors.x / 2.0;
						cmd_rate.y = -errors.y / 2.0;
						cmd_rate.z = -errors.z / 2.0;
						break;
				}
				break;
			case TOGGLESWITCH_DOWN: // LOW RATE
				// Are we in or out of deadband?
				switch(sat->AttDeadbandSwitch.GetState()){
					case TOGGLESWITCH_UP:   // MAX
						// 4.2 degrees attitude deadband
						// 4 degree "real" deadband, 0.2 degree is accomplished by the rate deadband					
						if (errors.x < -4.0 * RAD)
							cmd_rate.x = (errors.x + 4.0 * RAD) * 0.5; // Otherwise roll control is not stable
						if (errors.x > 4.0 * RAD)
							cmd_rate.x = (errors.x - 4.0 * RAD) * 0.5;
						if (errors.y < -4.0 * RAD)
							cmd_rate.y = -errors.y - 4.0 * RAD;
						if (errors.y > 4.0 * RAD)
							cmd_rate.y = -errors.y + 4.0 * RAD;
						if (errors.z < -4.0 * RAD)
							cmd_rate.z = -errors.z - 4.0 * RAD;
						if (errors.z > 4.0 * RAD)
							cmd_rate.z = -errors.z + 4.0 * RAD;
						break;
					case TOGGLESWITCH_DOWN: // MIN
						// 0.2 degrees attitude deadband
						// This is accomplished by the rate deadband
						cmd_rate.x = errors.x * 0.5;	// Otherwise roll control is not stable
						cmd_rate.y = -errors.y;
						cmd_rate.z = -errors.z;
						break;
				}
				break;
		}
		
		// Attitude hold automatic mode only when BMAG 1 uncaged and powered
		if (!sat->bmag1.IsPowered() || sat->bmag1.IsUncaged().x == 0) cmd_rate.x = 0;
		if (!sat->bmag1.IsPowered() || sat->bmag1.IsUncaged().y == 0) cmd_rate.y = 0;
		if (!sat->bmag1.IsPowered() || sat->bmag1.IsUncaged().z == 0) cmd_rate.z = 0;

		// Proportional Rate Demand 
		// The proportional rate commands are powered by AC, the signals are routed through the breakout switches, 
		// so the breakout switches must be set, too (see AOH fig. 2.3-19)
		int x_def = 0, y_def = 0, z_def = 0;

		if (rhc_ac_x < 28673 && rhc_x < 28673) { // MINUS 
			x_def = 28673 - rhc_ac_x; 
		}
		if (rhc_ac_x > 36863 && rhc_x > 36863) { // PLUS 
			x_def = (36863 - rhc_ac_x);
		}
		if (rhc_ac_y < 28673 && rhc_y < 28673) { // MINUS 
			y_def = 28673 - rhc_ac_y; 
		}
		if (rhc_ac_y > 36863 && rhc_y > 36863) { // PLUS 
			y_def = (36863 - rhc_ac_y);
		}
		if (rhc_ac_z < 28673 && rhc_z < 28673) { // MINUS 
			z_def = 28673 - rhc_ac_z; 
		}
		if (rhc_ac_z > 36863 && rhc_z > 36863) { // PLUS 
			z_def = (36863 - rhc_ac_z);
		}

		double axis_percent=0;
		switch(sat->AttRateSwitch.GetState()){
			case TOGGLESWITCH_UP:    // HIGH RATE
				// MAX RATE 7 dps pitch/yaw, 20 dps roll
				if(x_def != 0){ 
					axis_percent = (double)x_def / (double)28673;
					cmd_rate.x = -(0.34906585 * axis_percent);	// OVERRIDE
				}
				if(y_def != 0){ 
					axis_percent = (double)y_def / (double)28673;
					cmd_rate.y = -(0.122173048 * axis_percent);	// OVERRIDE
				}
				if(z_def != 0){ 
					axis_percent = (double)z_def / (double)28673;
					cmd_rate.z = (0.122173048 * axis_percent);	// OVERRIDE
				}
				break;
			case TOGGLESWITCH_DOWN:  // LOW RATE
				// MAX RATE .7 dps roll/pitch/yaw 
				if(x_def != 0){ 
					axis_percent = (double)x_def / (double)28673;
					cmd_rate.x = -(0.0122173048 * axis_percent);	// OVERRIDE
				}
				if(y_def != 0){ 
					axis_percent = (double)y_def / (double)28673;
					cmd_rate.y = -(0.0122173048 * axis_percent);	// OVERRIDE
				}
				if(z_def != 0){ 
					axis_percent = (double)z_def / (double)28673;
					cmd_rate.z = (0.0122173048 * axis_percent);	// OVERRIDE
				}
				break;
		}

		// RATE DAMPING
		// Rate damping automatic mode only when no cmd and manual attitude is RATE CMD
		if(cmd_rate.x == 0 && sat->ManualAttRollSwitch.GetState() == THREEPOSSWITCH_CENTER) { 
			switch(sat->AttRateSwitch.GetState()){
				case TOGGLESWITCH_UP:    // HIGH RATE
					// MAX RATE 2 dps roll
					if(sat->gdc.rates.z >  0.034906585){ cmd_rate.x = 0.034906585 - sat->gdc.rates.z; break; }
					if(sat->gdc.rates.z < -0.034906585){ cmd_rate.x = -0.034906585- sat->gdc.rates.z; break; }
					cmd_rate.x = sat->gdc.rates.z; 
					break;
				case TOGGLESWITCH_DOWN:  // LOW RATE
					// MAX RATE .2 dps roll
					if(sat->gdc.rates.z >  0.0034906585){ cmd_rate.x = 0.0034906585 - sat->gdc.rates.z; break; }
					if(sat->gdc.rates.z < -0.0034906585){ cmd_rate.x = -0.0034906585- sat->gdc.rates.z; break; }
					cmd_rate.x = sat->gdc.rates.z; 
					break;
			}
		}

		if(cmd_rate.y == 0 && sat->ManualAttPitchSwitch.GetState() == THREEPOSSWITCH_CENTER) { 
			switch(sat->AttRateSwitch.GetState()){
				case TOGGLESWITCH_UP:    // HIGH RATE
					// MAX RATE 2 dps
					if(sat->gdc.rates.x >  0.034906585){ cmd_rate.y = 0.034906585 - sat->gdc.rates.x; break; }
					if(sat->gdc.rates.x < -0.034906585){ cmd_rate.y = -0.034906585- sat->gdc.rates.x; break; }
					cmd_rate.y = sat->gdc.rates.x; 
					break;
				case TOGGLESWITCH_DOWN:  // LOW RATE
					// MAX RATE .2 dps
					if(sat->gdc.rates.x >  0.0034906585){ cmd_rate.y = 0.0034906585 - sat->gdc.rates.x; break; }
					if(sat->gdc.rates.x < -0.0034906585){ cmd_rate.y = -0.0034906585- sat->gdc.rates.x; break; }
					cmd_rate.y = sat->gdc.rates.x; 
					break;
			}
		}

		if(cmd_rate.z == 0 && sat->ManualAttYawSwitch.GetState() == THREEPOSSWITCH_CENTER) { 
			switch(sat->AttRateSwitch.GetState()){
				case TOGGLESWITCH_UP:    // HIGH RATE
					// MAX RATE 2 dps
					if(sat->gdc.rates.y >  0.034906585){ cmd_rate.z = 0.034906585 - sat->gdc.rates.y; break; }
					if(sat->gdc.rates.y < -0.034906585){ cmd_rate.z = -0.034906585- sat->gdc.rates.y; break; }
					cmd_rate.z = sat->gdc.rates.y; 
					break;
				case TOGGLESWITCH_DOWN:  // LOW RATE
					// MAX RATE .2 dps
					if(sat->gdc.rates.y >  0.0034906585){ cmd_rate.z = 0.0034906585 - sat->gdc.rates.y; break; }
					if(sat->gdc.rates.y < -0.0034906585){ cmd_rate.z = -0.0034906585- sat->gdc.rates.y; break; }
					cmd_rate.z = sat->gdc.rates.y; 
					break;
			}
		}
		// PSEUDORATE FEEDBACK
		if (sat->LimitCycleSwitch.GetState() == TOGGLESWITCH_UP && sat->ManualAttRollSwitch.GetState() == THREEPOSSWITCH_CENTER){
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
		if (sat->LimitCycleSwitch.GetState() == TOGGLESWITCH_UP && sat->ManualAttPitchSwitch.GetState() == THREEPOSSWITCH_CENTER){
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
		if (sat->LimitCycleSwitch.GetState() == TOGGLESWITCH_UP && sat->ManualAttYawSwitch.GetState() == THREEPOSSWITCH_CENTER){
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
		// GDC RATES are Z = ROLL, X = PITCH, Y = YAW
		rate_err.x = cmd_rate.x - (sat->gdc.rates.z + pseudorate.x);
		rate_err.y = cmd_rate.y - (sat->gdc.rates.x + pseudorate.y);
		rate_err.z = cmd_rate.z - (sat->gdc.rates.y + pseudorate.z);
		
		// sprintf(oapiDebugString(),"SCS: RATE CMD r%.3f p%.3f y%.3f ERR r%.3f p%.3f y%.3f",
		//	cmd_rate.x * DEG, cmd_rate.y * DEG, cmd_rate.z * DEG, 
		//	rate_err.x * DEG, rate_err.y * DEG, rate_err.z * DEG);	
		// sprintf(oapiDebugString(),"SCS PITCH rate %.3f cmd %.3f pseudo %.3f error %.3f", sat->gdc.rates.x * DEG, cmd_rate.y * DEG, pseudorate.y * DEG, rate_err.y * DEG);
		// sprintf(oapiDebugString(),"SCS ROLL rate %.3f cmd %.3f pseudo %.3f rate_err %.3f errors %.3f setting %.3f", sat->gdc.rates.z * DEG, cmd_rate.x * DEG, pseudorate.x * DEG, rate_err.x * DEG, errors.x * DEG, setting.x * DEG);

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
				switch(sat->AttRateSwitch.GetState()){
					case TOGGLESWITCH_UP:    // HIGH RATE
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
					case TOGGLESWITCH_DOWN:  // LOW RATE
						if(rate_err.x > 0.0034906585){
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
						if(rate_err.x < -0.0034906585){
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
				}
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				if (rhc_x < 28673) {  // MINUS
					if(!mnimp_roll_trigger){
						sat->rjec.SetThruster(10,1);
						sat->rjec.SetThruster(12,1);
						sat->rjec.SetThruster(14,1);
						sat->rjec.SetThruster(16,1);
					}
					mnimp_roll_trigger=1; mnimp_roll_flag=1;
				}
				if (rhc_x > 36863) { // PLUS
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
				switch(sat->AttRateSwitch.GetState()){
					case TOGGLESWITCH_UP:    // HIGH RATE
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
					case TOGGLESWITCH_DOWN:  // LOW RATE
						if(rate_err.y > 0.0034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(1,1);
							sat->rjec.SetThruster(3,1);
							sat->rjec.SetThruster(2,0);
							sat->rjec.SetThruster(4,0);
							accel_pitch_trigger=1; accel_pitch_flag=1;
						}
						if(rate_err.y < -0.0034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(2,1);
							sat->rjec.SetThruster(4,1);
							sat->rjec.SetThruster(1,0);
							sat->rjec.SetThruster(3,0);
							accel_pitch_trigger=1; accel_pitch_flag=-1;
						}							
						break;
				}
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				if (rhc_y < 28673) {  // MINUS
					if(!mnimp_pitch_trigger){
						sat->rjec.SetThruster(2,1);
						sat->rjec.SetThruster(4,1);
					}
					mnimp_pitch_trigger=1; mnimp_pitch_flag=1;
				}
				if (rhc_y > 36863) { // PLUS
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
				switch(sat->AttRateSwitch.GetState()){
					case TOGGLESWITCH_UP:    // HIGH RATE
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
					case TOGGLESWITCH_DOWN:  // LOW RATE
						if(rate_err.z > 0.0034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(6,1);
							sat->rjec.SetThruster(8,1);
							sat->rjec.SetThruster(5,0);
							sat->rjec.SetThruster(7,0);
							accel_yaw_trigger=1; accel_yaw_flag=-1;
						}
						if(rate_err.z < -0.0034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(5,1);
							sat->rjec.SetThruster(7,1);
							sat->rjec.SetThruster(6,0);
							sat->rjec.SetThruster(8,0);
							accel_yaw_trigger=1; accel_yaw_flag=1;
						}							
						break;
				}
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				if (rhc_z < 28673) {  // MINUS
					if(!mnimp_yaw_trigger){
						sat->rjec.SetThruster(6,1);
						sat->rjec.SetThruster(8,1);
					}
					mnimp_yaw_trigger=1; mnimp_yaw_flag=1;
				}
				if (rhc_z > 36863) { // PLUS
					if(!mnimp_yaw_trigger){
						sat->rjec.SetThruster(5,1);
						sat->rjec.SetThruster(7,1);
					}
					mnimp_yaw_trigger=1; mnimp_yaw_flag=1;
				}
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				break;
		}
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

	//RSI Timestep
	if (sat->EMSRollSwitch.IsUp() && sat->SCSLogicBus4.Voltage() > SP_MIN_DCVOLTAGE) {
		SetRSIRotation(RSITarget + sat->gdc.rollstabilityrate * simdt);
		//sprintf(oapiDebugString(), "entry lift angle? %f", RSITarget);
	}

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
void EMS::SetRSIRotation(double angle) { 

	angle = fmod(angle + TWO_PI, TWO_PI); //remove unwanted multiples of 2PI and avoid negative numbers

	//sprintf(oapiDebugString(),"RSITarget:%f  RSIRotation:%f ", angle,RSIRotation);

	RSITarget = angle;
}

double EMS::GetRSIRotation() {
	return RSITarget;
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
	if (sat->SPSEngine.IsThrustOn()) return true;
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
		} else if (!strnicmp (line, "DVTESTTIME", 10)) {
			sscanf(line + 10, "%lf", &dVTestTime);
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