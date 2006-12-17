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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.3  2006/11/13 14:47:31  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.2  2006/10/05 16:09:02  tschachim
  *	Fixed SCS attitude hold mode.
  *	
  *	Revision 1.1  2006/06/17 18:18:00  tschachim
  *	Bugfixes SCS automatic modes,
  *	Changed quickstart separation key to J.
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"

inline void WriteScenario_double(FILEHANDLE scn, char *item, double d) {

	char buffer[256];

	sprintf(buffer, "  %s %lf", item, d);
	oapiWriteLine(scn, buffer);
}

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

	// Get vessel status
	VESSELSTATUS vs;
	if (Vessel == NULL) return;
	Vessel->GetStatus(vs);

	// Get eccliptic-plane attitude
	OrbiterAttitude.Attitude.x = vs.arot.x;
	OrbiterAttitude.Attitude.y = vs.arot.y;
	OrbiterAttitude.Attitude.z = vs.arot.z;

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

	WriteScenario_double(scn, "ATTITUDEX", Attitude.x);
	WriteScenario_double(scn, "ATTITUDEY", Attitude.y);
	WriteScenario_double(scn, "ATTITUDEZ", Attitude.z);
	WriteScenario_double(scn, "LASTATTITUDEX", LastAttitude.x);
	WriteScenario_double(scn, "LASTATTITUDEY", LastAttitude.y);
	WriteScenario_double(scn, "LASTATTITUDEZ", LastAttitude.z);
	WriteScenario_double(scn, "ORBITERATTITUDEX", OrbiterAttitude.Attitude.x);
	WriteScenario_double(scn, "ORBITERATTITUDEY", OrbiterAttitude.Attitude.y);
	WriteScenario_double(scn, "ORBITERATTITUDEZ", OrbiterAttitude.Attitude.z);
	WriteScenario_double(scn, "ORBITERATTITUDEM11", OrbiterAttitude.AttitudeReference.m11);
	WriteScenario_double(scn, "ORBITERATTITUDEM12", OrbiterAttitude.AttitudeReference.m12);
	WriteScenario_double(scn, "ORBITERATTITUDEM13", OrbiterAttitude.AttitudeReference.m13);
	WriteScenario_double(scn, "ORBITERATTITUDEM21", OrbiterAttitude.AttitudeReference.m21);
	WriteScenario_double(scn, "ORBITERATTITUDEM22", OrbiterAttitude.AttitudeReference.m22);
	WriteScenario_double(scn, "ORBITERATTITUDEM23", OrbiterAttitude.AttitudeReference.m23);
	WriteScenario_double(scn, "ORBITERATTITUDEM31", OrbiterAttitude.AttitudeReference.m31);
	WriteScenario_double(scn, "ORBITERATTITUDEM32", OrbiterAttitude.AttitudeReference.m32);
	WriteScenario_double(scn, "ORBITERATTITUDEM33", OrbiterAttitude.AttitudeReference.m33);
}

void AttitudeReference::LoadState(char *line) {

	if (!strnicmp (line, "ATTITUDEINITIALIZED", 19)) {
		sscanf(line + 19, "%i", &AttitudeInitialized);
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
}

void BMAG::Init(Saturn *v, e_object *dcbus, e_object *acbus, Boiler *h) {
	// Initialize
	sat = v;
	dc_bus = dcbus;
	ac_bus = acbus;
	heater = h;
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
	if (!powered) 
		uncaged = _V(0, 0, 0);

	heater->WireTo(dc_source);	// Take DC power to heat the gyro
	AttitudeReference::Timestep(simdt);
}

void BMAG::SystemTimestep(double simdt) {

	if (powered && ac_source)
		ac_source->DrawPower(8.7);  // take AC power to spin the gyro
}

void BMAG::Cage(int axis) {

	uncaged.data[axis] = 0;
}

void BMAG::Uncage(int axis) {

	// Uncage only if powered
	if (!powered) return;

	// If caged store current attitude as reference for errors
	if (uncaged.data[axis] == 0) {
		targetAttitude.data[axis] = Attitude.data[axis];
		uncaged.data[axis] = 1;
	}
}

VECTOR3 BMAG::GetAttitudeError() {

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

void BMAG::SaveState(FILEHANDLE scn) {

	// START_STRING is written in Saturn
	WriteScenario_double(scn, "UNCAGEDX", uncaged.x);
	WriteScenario_double(scn, "UNCAGEDY", uncaged.y);
	WriteScenario_double(scn, "UNCAGEDZ", uncaged.z);
	WriteScenario_double(scn, "TARGETATTITUDEX", targetAttitude.x);
	WriteScenario_double(scn, "TARGETATTITUDEY", targetAttitude.y);
	WriteScenario_double(scn, "TARGETATTITUDEZ", targetAttitude.z);
	WriteScenario_double(scn, "RATESX", rates.x);
	WriteScenario_double(scn, "RATESY", rates.y);
	WriteScenario_double(scn, "RATESZ", rates.z);
	
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
}

void GDC::Init(Saturn *v)
{
	sat = v;
	AttitudeReference::Init(v);
}

void GDC::SystemTimestep(double simdt) {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2) return;  // Switched off

	if (sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->StabContSystemAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) return;

	sat->StabContSystemAc1CircuitBraker.DrawPower(10.4); 
	sat->StabContSystemAc2CircuitBraker.DrawPower(3.4); 	
}
		
void GDC::Timestep(double simdt) {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2) return;  // Switched off

	if (sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->StabContSystemAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) return;

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
	
			// Uncage Roll BMAG 1, if in RATE CMD and breakout switches open
			if (sat->ManualAttRollSwitch.GetState() == THREEPOSSWITCH_CENTER &&
				sat->eca.rhc_x > 28673 && sat->eca.rhc_x < 36863) {
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

			// Uncage Pitch BMAG 1, if in RATE CMD and breakout switches open
			if (sat->ManualAttPitchSwitch.GetState() == THREEPOSSWITCH_CENTER &&
				sat->eca.rhc_y > 28673 && sat->eca.rhc_y < 36863) {
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

			// Uncage Yaw BMAG 1, if in RATE CMD and breakout switches open
			if (sat->ManualAttYawSwitch.GetState() == THREEPOSSWITCH_CENTER &&
				sat->eca.rhc_z > 28673 && sat->eca.rhc_z < 36863) {
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

	AttitudeReference::Timestep(simdt);

	rates.x = pitchBmag->GetRates().x;
	rates.y = yawBmag->GetRates().y;
	rates.z = rollBmag->GetRates().z;

	// If the current BMAG has no power it doesn't provide rates so we don't change 
	// the attitude of the failed axis
	if (!rollBmag->IsPowered())  SetAttitude(_V(LastAttitude.x, Attitude.y, Attitude.z));
	if (!pitchBmag->IsPowered()) SetAttitude(_V(Attitude.x, LastAttitude.y, Attitude.z));
	if (!yawBmag->IsPowered())   SetAttitude(_V(Attitude.x, Attitude.y, LastAttitude.z));
}

bool GDC::AlignGDC(){
	// User pushed the Align GDC button.
	// Set the GDC attitude to match what's on the ASCP.
	SetAttitude(_V(sat->ascp.output.x * 0.017453,
				   sat->ascp.output.y * 0.017453,
	               sat->ascp.output.z * 0.017453)); // Degrees to radians
	return true;
}

void GDC::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, GDC_START_STRING);
	oapiWriteScenario_int(scn, "FDAIERRENA", fdai_err_ena);
	oapiWriteScenario_int(scn, "FDAIERRX", fdai_err_x);
	oapiWriteScenario_int(scn, "FDAIERRY", fdai_err_y);
	oapiWriteScenario_int(scn, "FDAIERRZ", fdai_err_z);
	
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
	output.x = 0;
	output.y = 0;
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

bool ASCP::PaintRollDisplay(SURFHANDLE surf, SURFHANDLE digits){
	char cheat[10];                       // Have plenty of room for this
	int srx,sry,beta,digit;
	sprintf(cheat,"%+06.1f",output.x);    // Arithmetic is for suckers!	
	srx = 8+((cheat[1]-0x30)*25);	      // Hint: 0x30 = ASCII "0"
	oapiBlt (surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8+((cheat[2]-0x30)*25);
	oapiBlt (surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	digit = cheat[3]-0x30; srx = 8+(digit*25); beta = cheat[5]-0x30; sry = (int)(beta*1.2);
	if(beta == 0){		
		oapiBlt (surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	}else{
		oapiBlt (surf, digits, 20, sry, srx, 33, 9, 12-sry, SURF_PREDEF_CK);			
		if(digit == 9){digit=0;}else{digit++;}
		srx = 8+(digit*25);			
		oapiBlt (surf, digits, 20, 0, srx, 45-sry, 9, sry, SURF_PREDEF_CK);
	}
	return true;
}

bool ASCP::PaintPitchDisplay(SURFHANDLE surf, SURFHANDLE digits){
	char cheat[10];                       // Have plenty of room for this
	int srx,sry,beta,digit;
	sprintf(cheat,"%+06.1f",output.y);    // Arithmetic is for suckers!	
	srx = 8+((cheat[1]-0x30)*25);	      // Hint: 0x30 = ASCII "0"
	oapiBlt (surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8+((cheat[2]-0x30)*25);
	oapiBlt (surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	digit = cheat[3]-0x30; srx = 8+(digit*25); beta = cheat[5]-0x30; sry = (int)(beta*1.2);
	if(beta == 0){		
		oapiBlt (surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	}else{
		oapiBlt (surf, digits, 20, sry, srx, 33, 9, 12-sry, SURF_PREDEF_CK);			
		if(digit == 9){digit=0;}else{digit++;}
		srx = 8+(digit*25);			
		oapiBlt (surf, digits, 20, 0, srx, 45-sry, 9, sry, SURF_PREDEF_CK);
	}
	return true;
}

bool ASCP::PaintYawDisplay(SURFHANDLE surf, SURFHANDLE digits){
	char cheat[10];                       // Have plenty of room for this
	int srx,sry,beta,digit;
	sprintf(cheat,"%+06.1f",output.z);    // Arithmetic is for suckers!	
	srx = 8+((cheat[1]-0x30)*25);	      // Hint: 0x30 = ASCII "0"
	oapiBlt (surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8+((cheat[2]-0x30)*25);
	oapiBlt (surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	digit = cheat[3]-0x30; srx = 8+(digit*25); beta = cheat[5]-0x30; sry = (int)(beta*1.2);
	if(beta == 0){		
		oapiBlt (surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	}else{
		oapiBlt (surf, digits, 20, sry, srx, 33, 9, 12-sry, SURF_PREDEF_CK);			
		if(digit == 9){digit=0;}else{digit++;}
		srx = 8+(digit*25);			
		oapiBlt (surf, digits, 20, 0, srx, 45-sry, 9, sry, SURF_PREDEF_CK);
	}
	return true;
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
	errors.x = sat->gdc.fdai_err_x * 0.106770; // CMC error value, CMC-scaled
	errors.y = sat->gdc.fdai_err_y * 0.106770; // CMC error value, CMC-scaled
	errors.z = sat->gdc.fdai_err_z * 0.106770; // CMC error value, CMC-scaled
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
	AGCActiveTimer = 0;

	int x = 0;
	while (x < 20) {
		ThrusterDemand[x] = 0;
		x++;
	}
}

void RJEC::Init(Saturn *vessel){
	sat = vessel;
}

void RJEC::SystemTimestep(double simdt) {

	// Ensure AC power
	if (sat->SIGCondDriverBiasPower1Switch.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->SIGCondDriverBiasPower2Switch.Voltage() < SP_MIN_ACVOLTAGE) return;

	sat->SIGCondDriverBiasPower1Switch.DrawPower(2.3); 
	sat->SIGCondDriverBiasPower2Switch.DrawPower(0.2); 	
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

	// TODO Dirty Hack for the AGC++ attitude control
	// see CSMcomputer::SetAttitudeRotLevel(VECTOR3 level)
	if (AGCActiveTimer > 0) {
		AGCActiveTimer = max(0, AGCActiveTimer - simdt);
		return;
	}

	// Ensure AC power
	if (sat->SIGCondDriverBiasPower1Switch.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->SIGCondDriverBiasPower2Switch.Voltage() < SP_MIN_ACVOLTAGE) return;
	
	ChannelValue30 val30;
	int sm_sep=0;
	val30.Value = sat->agc.GetInputChannel(030); 
	sm_sep = val30.Bits.CMSMSeperate; // There should probably be a way for the SCS to do this if VAGC is not running
	int thruster = 1;
	int thruster_lockout;

	while(thruster < 17){
		// THRUSTER LOCKOUT CHECKING
		thruster_lockout = 0;
		// If it's a pitch or yaw jet, lockout on SPS thrusting
		if(thruster < 9 && SPSActive != 0){ thruster_lockout = 1; } 
		// Lockout on direct axes.
		if(thruster < 5 && DirectPitchActive != 0){ thruster++; continue; } // Skip entirely
		if(thruster > 4 && thruster < 9 && DirectYawActive != 0){ thruster++; continue; } 
		if(thruster > 8 && DirectRollActive != 0){ thruster++; continue; } 
		// THRUSTER PROCESSING
		switch(thruster){
			case 1:
				if(sat->PitchC3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchC3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(0,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 3, 0); 
					}else{
						sat->SetCMRCSState(0,0);
					}
				}
				break;

			case 2:
				if(sat->PitchA4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchA4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(2,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 4, 0); 
					}else{
						sat->SetCMRCSState(2,0);
					}
				}
				break;

			case 3:
				if(sat->PitchA3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchA3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(1,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 3, 0); 
					}else{
						sat->SetCMRCSState(1,0);
					}
				}
				break;

			case 4:
				if(sat->PitchC4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchC4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(3,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 4, 0); 
					}else{
						sat->SetCMRCSState(3,0);
					}
				}
				break;

			case 5:
				if(sat->YawD3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawD3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(4,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 3, 0); 
					}else{
						sat->SetCMRCSState(4,0);
					}
				}
				break;

			case 6:
				if(sat->YawB4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawB4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(6,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 4, 0); 
					}else{
						sat->SetCMRCSState(6,0);
					}
				}
				break;

			case 7:
				if(sat->YawB3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawB3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(5,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 3, 0); 
					}else{
						sat->SetCMRCSState(5,0);
					}
				}
				break;

			case 8:
				if(sat->YawD4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawD4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(7,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 4, 0); 
					}else{
						sat->SetCMRCSState(7,0);
					}
				}
				break;

			case 9:
				if(sat->BdRollB1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollB1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 1, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(8,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 1, 0); 
					}else{
						sat->SetCMRCSState(8,0);
					}
				}
				break;

			case 10:
				if(sat->BdRollD2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollD2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 2, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(10,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 2, 0); 
					}else{
						sat->SetCMRCSState(10,0);
					}
				}
				break;

			case 11:
				if(sat->BdRollD1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollD1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 1, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(9,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 1, 0); 
					}else{
						sat->SetCMRCSState(9,0);
					}
				}
				break;

			case 12:
				if(sat->BdRollB2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollB2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 2, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(11,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 2, 0); 
					}else{
						sat->SetCMRCSState(11,0);
					}
				}
				break;

			case 13:
				if(sat->AcRollA1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollA1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 1, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 1, 0); 
					}
				}
				break;

			case 14:
				if(sat->AcRollA2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollA2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 2, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 2, 0); 
					}
				}
				break;

			case 15:
				if(sat->AcRollC1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollC1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 1, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 1, 0); 
					}
				}
				break;

			case 16:
				if(sat->AcRollC2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollC2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 2, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 2, 0); 
					}
				}
				break;
		}
		thruster++;
	}		
}

void RJEC::SetThruster(int thruster,bool Active){
	if(thruster > 0 && thruster < 20){
		ThrusterDemand[thruster] = Active; // Next timestep does the work
	}
}

// Electronic Control Assembly
ECA::ECA(){

	rhc_x = 32768;
	rhc_y = 32768;
	rhc_z = 32768;
	thc_x = 32768;
	thc_y = 32768;
	thc_z = 32768;

	accel_roll_trigger = 0;
	mnimp_roll_trigger = 0;
	accel_pitch_trigger = 0;
	mnimp_pitch_trigger = 0;
	accel_yaw_trigger = 0;
	mnimp_yaw_trigger = 0;
	trans_x_trigger = 0;
	trans_y_trigger = 0;
	trans_z_trigger = 0;
	pseudorate = _V(0,0,0);

	sat = NULL;
}

void ECA::Init(Saturn *vessel){
	sat = vessel;
}

void ECA::SystemTimestep(double simdt) {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return;  // Switched off

	// Ensure DC power
	if (sat->SystemMnACircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || 
	    sat->SystemMnBCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE) return;

	sat->SystemMnACircuitBraker.DrawPower(10);	// TODO real power is unknown
	sat->SystemMnBCircuitBraker.DrawPower(10);	// TODO real power is unknown
}

void ECA::TimeStep(double simdt) {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return;  // Switched off

	if (sat->SystemMnACircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || 
	    sat->SystemMnBCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE) return;

	// SCS is in control if the THC CLOCKWISE line is high (not implemented)
	// or if the SC CONT switch is set to SCS.
	int accel_roll_flag = 0;
	int mnimp_roll_flag = 0;
	int accel_pitch_flag = 0;
	int mnimp_pitch_flag = 0;
	int accel_yaw_flag = 0;
	int mnimp_yaw_flag = 0;
	VECTOR3 cmd_rate = _V(0,0,0);
	VECTOR3 rate_err = _V(0,0,0);
	if(mnimp_roll_trigger){
		sat->rjec.SetThruster(9,0);
		sat->rjec.SetThruster(10,0);
		sat->rjec.SetThruster(11,0);
		sat->rjec.SetThruster(12,0);
		sat->rjec.SetThruster(13,0);
		sat->rjec.SetThruster(14,0);
		sat->rjec.SetThruster(15,0);
		sat->rjec.SetThruster(16,0); 
	}
	if(mnimp_pitch_trigger){
		sat->rjec.SetThruster(1,0);
		sat->rjec.SetThruster(2,0);
		sat->rjec.SetThruster(3,0);
		sat->rjec.SetThruster(4,0);
	}
	if(mnimp_yaw_trigger){
		sat->rjec.SetThruster(5,0);
		sat->rjec.SetThruster(6,0);
		sat->rjec.SetThruster(7,0);
		sat->rjec.SetThruster(8,0);
	}
	// ERROR DETERMINATION
	VECTOR3 setting,target,errors;
	if(sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN){
		// Get BMAG1 attitude errors
		setting = sat->bmag1.GetAttitudeError();
		// And difference from GDC attitude (plus rate)
		target.x = setting.x + sat->gdc.rates.z;
		target.y = setting.y + sat->gdc.rates.x;
		target.z = setting.z - sat->gdc.rates.y; // Yaw rate points the wrong way.
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
							cmd_rate.x = errors.x + 4.0 * RAD;
						if (errors.x > 4.0 * RAD)
							cmd_rate.x = errors.x - 4.0 * RAD;
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
						cmd_rate.x = errors.x;
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
		int x_def = 0, y_def = 0, z_def = 0;

		if(rhc_x < 28673){ // MINUS 
			x_def = 28673-rhc_x; 
		}
		if(rhc_x > 36863){ // PLUS 
			x_def = (36863-rhc_x);
		}
		if(rhc_y < 28673){ // MINUS 
			y_def = 28673-rhc_y; 
		}
		if(rhc_y > 36863){ // PLUS 
			y_def = (36863-rhc_y);
		}
		if(rhc_z < 28673){ // MINUS 
			z_def = 28673-rhc_z; 
		}
		if(rhc_z > 36863){ // PLUS 
			z_def = (36863-rhc_z);
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
			if (sat->rjec.ThrusterDemand[9] != 0 || sat->rjec.ThrusterDemand[11] != 0 ||
			    sat->rjec.ThrusterDemand[13] != 0 || sat->rjec.ThrusterDemand[15] != 0 ) {
				pseudorate.x += 0.1 * simdt; 
			} else if (sat->rjec.ThrusterDemand[10] != 0 || sat->rjec.ThrusterDemand[12] != 0 ||
			    sat->rjec.ThrusterDemand[14] != 0 || sat->rjec.ThrusterDemand[16] != 0 ) {
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
			if (sat->rjec.ThrusterDemand[1] != 0 || sat->rjec.ThrusterDemand[3] != 0) {
				pseudorate.y += 0.1 * simdt; 
			} else if (sat->rjec.ThrusterDemand[2] != 0 || sat->rjec.ThrusterDemand[4] != 0) {
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
			if (sat->rjec.ThrusterDemand[6] != 0 || sat->rjec.ThrusterDemand[8] != 0) {
				pseudorate.z += 0.1 * simdt; 
			} else if (sat->rjec.ThrusterDemand[5] != 0 || sat->rjec.ThrusterDemand[7] != 0) {
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
	}
	// ROTATION
	if(sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN){
		switch(sat->ManualAttRollSwitch.GetState()){
			case THREEPOSSWITCH_UP:      // ACCEL CMD
				// ECA auto-control is inhibited. Auto fire commands are generated from the breakout switches.
				if(rhc_x < 28673 && rhc_x > 2738){  // MINUS
					sat->rjec.SetThruster(10,1);
					sat->rjec.SetThruster(12,1);
					sat->rjec.SetThruster(14,1);
					sat->rjec.SetThruster(16,1);
					accel_roll_trigger=1; accel_roll_flag=-1;
				}
				if(rhc_x > 36863 && rhc_x < 62798){ // PLUS
					sat->rjec.SetThruster(9,1);
					sat->rjec.SetThruster(11,1);
					sat->rjec.SetThruster(13,1);
					sat->rjec.SetThruster(15,1);
					accel_roll_trigger=1; accel_roll_flag=1;
				}
				break;
			case THREEPOSSWITCH_CENTER:  // RATE CMD
				// Automatic mode and proportional-rate mode
				switch(sat->AttRateSwitch.GetState()){
					case TOGGLESWITCH_UP:    // HIGH RATE
						if(rate_err.x > 0.034906585){
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
						if(rate_err.x < -0.034906585){
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
				if(rhc_x < 28673 && rhc_x > 2738){  // MINUS
					if(!mnimp_roll_trigger){
						sat->rjec.SetThruster(10,1);
						sat->rjec.SetThruster(12,1);
						sat->rjec.SetThruster(14,1);
						sat->rjec.SetThruster(16,1);
					}
					mnimp_roll_trigger=1; mnimp_roll_flag=1;
				}
				if(rhc_x > 36863 && rhc_x < 62798){ // PLUS
					if(!mnimp_roll_trigger){
						sat->rjec.SetThruster(9,1);
						sat->rjec.SetThruster(11,1);
						sat->rjec.SetThruster(13,1);
						sat->rjec.SetThruster(15,1);
					}
					mnimp_roll_trigger=1; mnimp_roll_flag=1;
				}
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				break;
		}
		switch(sat->ManualAttPitchSwitch.GetState()){
			case THREEPOSSWITCH_UP:      // ACCEL CMD
				// ECA auto-control is inhibited. Auto fire commands are generated from the breakout switches.
				if(rhc_y < 28673 && rhc_y > 2738){  // MINUS
					sat->rjec.SetThruster(2,1);
					sat->rjec.SetThruster(4,1);
					accel_pitch_trigger=1; accel_pitch_flag=-1;
				}
				if(rhc_y > 36863 && rhc_y < 62798){ // PLUS
					sat->rjec.SetThruster(1,1);
					sat->rjec.SetThruster(3,1);
					accel_pitch_trigger=1; accel_pitch_flag=1;
				}
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
				if(rhc_y < 28673 && rhc_y > 2738){  // MINUS
					if(!mnimp_pitch_trigger){
						sat->rjec.SetThruster(2,1);
						sat->rjec.SetThruster(4,1);
					}
					mnimp_pitch_trigger=1; mnimp_pitch_flag=1;
				}
				if(rhc_y > 36863 && rhc_y < 62798){ // PLUS
					if(!mnimp_pitch_trigger){
						sat->rjec.SetThruster(1,1);
						sat->rjec.SetThruster(3,1);
					}
					mnimp_pitch_trigger=1; mnimp_pitch_flag=1;
				}
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				break;
		}
		switch(sat->ManualAttYawSwitch.GetState()){
			case THREEPOSSWITCH_UP:      // ACCEL CMD
				// ECA auto-control is inhibited. Auto fire commands are generated from the breakout switches.
				if(rhc_z < 28673 && rhc_z > 2738){  // MINUS
					sat->rjec.SetThruster(6,1);
					sat->rjec.SetThruster(8,1);
					accel_yaw_trigger=1; accel_yaw_flag=-1;
				}
				if(rhc_z > 36863 && rhc_z < 62798){ // PLUS
					sat->rjec.SetThruster(5,1);
					sat->rjec.SetThruster(7,1);
					accel_yaw_trigger=1; accel_yaw_flag=1;
				}
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
				if(rhc_z < 28673 && rhc_z > 2738){  // MINUS
					if(!mnimp_yaw_trigger){
						sat->rjec.SetThruster(6,1);
						sat->rjec.SetThruster(8,1);
					}
					mnimp_yaw_trigger=1; mnimp_yaw_flag=1;
				}
				if(rhc_z > 36863 && rhc_z < 62798){ // PLUS
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
	// TRANSLATION HANDLING
	int trans_x_flag=0,trans_y_flag=0,trans_z_flag=0;
	int sm_sep=0;
	ChannelValue30 val30;
	val30.Value = sat->agc.GetInputChannel(030); 
	if(sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN && !sm_sep){
		if(thc_x < 16384){ // PLUS X
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(14,1); }else{ sat->rjec.SetThruster(14,0); }
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(15,1); }else{ sat->rjec.SetThruster(15,0); }
			trans_x_trigger=1; trans_x_flag=1;
		}
		if(thc_x > 49152){ // MINUS X
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(16,1); }else{ sat->rjec.SetThruster(16,0); }
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(13,1); }else{ sat->rjec.SetThruster(13,0); }
			trans_x_trigger=1; trans_x_flag=1;
		}
		if(thc_y < 16384){ // MINUS Y (FORWARD)
			if(accel_pitch_flag > -1){ sat->rjec.SetThruster(1,1); }else{ sat->rjec.SetThruster(1,0); }
			if(accel_pitch_flag < 1 ){ sat->rjec.SetThruster(2,1); }else{ sat->rjec.SetThruster(2,0); }
			if(accel_yaw_flag   > -1){ sat->rjec.SetThruster(5,1); }else{ sat->rjec.SetThruster(5,0); }
			if(accel_yaw_flag   < 1 ){ sat->rjec.SetThruster(6,1); }else{ sat->rjec.SetThruster(6,0); }
			trans_y_trigger=1; trans_y_flag=1;
		}
		if(thc_y > 49152){ // PLUS Y (BACKWARD)
			if(accel_pitch_flag > -1){ sat->rjec.SetThruster(3,1); }else{ sat->rjec.SetThruster(3,0); }
			if(accel_pitch_flag < 1 ){ sat->rjec.SetThruster(4,1); }else{ sat->rjec.SetThruster(4,0); }
			if(accel_yaw_flag   > -1){ sat->rjec.SetThruster(7,1); }else{ sat->rjec.SetThruster(7,0); }
			if(accel_yaw_flag   < 1 ){ sat->rjec.SetThruster(8,1); }else{ sat->rjec.SetThruster(8,0); }
			trans_y_trigger=1; trans_y_flag=1;
		}
		if(thc_z < 16384){ // MINUS Z (UP)
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(11,1); }else{ sat->rjec.SetThruster(11,0); }
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(12,1); }else{ sat->rjec.SetThruster(12,0); }
			trans_z_trigger=1; trans_z_flag=1;
		}
		if(thc_z > 49152){ // PLUS Z (DOWN)
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(9,1);  }else{ sat->rjec.SetThruster(9,0);  }
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(10,1); }else{ sat->rjec.SetThruster(10,0); }
			trans_z_trigger=1; trans_z_flag=1;
		}
	}
	if(!trans_x_flag && trans_x_trigger){
		sat->rjec.SetThruster(13,0);
		sat->rjec.SetThruster(14,0);
		sat->rjec.SetThruster(15,0);
		sat->rjec.SetThruster(16,0);
		trans_x_trigger=0;
	}
	if(!trans_y_flag && trans_y_trigger){
		sat->rjec.SetThruster(3,0); 
		sat->rjec.SetThruster(7,0); 
		sat->rjec.SetThruster(4,0); 
		sat->rjec.SetThruster(8,0); 
		sat->rjec.SetThruster(2,0);
		sat->rjec.SetThruster(6,0);
		sat->rjec.SetThruster(1,0); 
		sat->rjec.SetThruster(5,0); 
		trans_y_trigger=0;
	}
	if(!trans_z_flag && trans_z_trigger){
		sat->rjec.SetThruster(9,0);
		sat->rjec.SetThruster(10,0);
		sat->rjec.SetThruster(11,0);
		sat->rjec.SetThruster(12,0);
		trans_z_trigger=0;
	}
}

// DS20060326 TELECOM OBJECTS
// PCM SYSTEM
PCM::PCM(){
	sat = NULL;
	conn_state = 0;
	wsk_error = 0;
	last_update = 0;
}

void PCM::Init(Saturn *vessel){
	sat = vessel;
	conn_state = 0;
	wsk_error = 0;
	last_update = 0;
	word_addr = 0;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR ){
		sprintf(wsk_emsg,"TELECOM: Error at WSAStartup()");
		wsk_error = 1;
		return;
	}
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET ) {
		sprintf(wsk_emsg,"TELECOM: Error at socket(): %ld", WSAGetLastError());
		WSACleanup();
		wsk_error = 1;
		return;
	}
	// Be nonblocking
	int iMode = 1; // 0 = BLOCKING, 1 = NONBLOCKING
	if(ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode) != 0){
		sprintf(wsk_emsg,"TELECOM: ioctlsocket() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Set up incoming options
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	service.sin_port = htons( 14242 );

	if ( bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {
		sprintf(wsk_emsg,"TELECOM: bind() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	if ( listen( m_socket, 1 ) == SOCKET_ERROR ){
		wsk_error = 1;
		sprintf(wsk_emsg,"TELECOM: listen() failed: %ld", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	conn_state = 1; // INITIALIZED, LISTENING
}

void PCM::TimeStep(double simt){
	// This stuff has to happen every timestep, regardless of system status.
	if(wsk_error != 0){
		sprintf(oapiDebugString(),"%s",wsk_emsg);
		// return;
	}
	// For now, don't care about voltages and such.

	// Allow IO to check for connections, etc
	if(conn_state != 2){
		last_update = simt; // Don't care about rate
		perform_io();
		return;
	}

	// Generate PCM datastream
	switch(sat->PCMBitRateSwitch.GetState()){
		case TOGGLESWITCH_DOWN: // LOW			
			tx_size = (int)((simt - last_update) / 0.005);
			// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
			if(tx_size > 0){
				last_update = simt;
				if(tx_size < 1024){
					tx_offset = 0;
					while(tx_offset < tx_size){
						generate_stream_lbr();
						tx_offset++;
					}
					perform_io();
				}
			}	
			break;

		case TOGGLESWITCH_UP:   // HIGH
			tx_size = (int)((simt - last_update) / 0.00015625);
			// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
			if(tx_size > 0){
				last_update = simt;
				if(tx_size < 1024){
					tx_offset = 0;
					while(tx_offset < tx_size){
						generate_stream_hbr();
						tx_offset++;
					}			
					perform_io();
				}
			}	
			break;
	}
}

void PCM::generate_stream_lbr(){
	unsigned char data=0;
	// 40 words per frame, 5 frames, 1 frame per second
	switch(word_addr){
		case 0: // SYNC 1
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();
			// And continue
			tx_data[tx_offset] = 05;
			break;
		case 1: // SYNC 2
			tx_data[tx_offset] = 0171;
			break;
		case 2: // SYNC 3
			tx_data[tx_offset] = 0267;
			break;
		case 3: // SYNC 4 & FRAME COUNT
			tx_data[tx_offset] = (0300|frame_count);
			break;
		case 4: 
			switch(frame_count){
				case 0: // 11A1 ECS: SUIT MANF ABS PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A109 EPS: BAT B CURR
					tx_data[tx_offset] = (unsigned char)(sat->EntryBatteryB->Current() / 0.390625); 
					break;
				case 2: // 11A46 RCS: SM HE MANF C PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A154 CMI: SCE NEG SUPPLY VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A91 EPS: BAT BUS A VOLTS					
					tx_data[tx_offset] = (unsigned char)(sat->BatteryBusA.Voltage() / 0.17578125);
					break;
			}
			break;
		case 5:
			switch(frame_count){
				case 0: // 11A2 ECS: SUIT COMP DELTA P
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A110 EPS: BAT C CURR
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A47 EPS: LM HEATER CURRENT
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A155 RCS: CM HE TK A TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A92 RCS: SM FU MANF A PRESS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 6:
			switch(frame_count){
				case 0: // 11A3 ECS: GLY PUMP OUT PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A111 ECS: SM FU MANF C PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A48 PCM HI LEVEL 85 PCT REF
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A156 CM HE TK B TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A93 BAT BUS B VOLTS
					tx_data[tx_offset] = (unsigned char)(sat->BatteryBusB.Voltage() / 0.17578125);
					break;
			}
			break;
		case 7:
			switch(frame_count){
				case 0: // 11A4 ECS SURGE TANK PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A112 SM FU MANF D PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A49 PC HI LEVEL 15 PCT REF
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A157 SEC GLY PUMP OUT PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A94 SM FU MANF B PRESS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 8: // 51DS1A COMPUTER DIGITAL DATA (40 BITS) 
		case 28:
			unsigned char data;
			ChannelValue13 ch13;
			ch13.Value = sat->agc.GetOutputChannel(013);			
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			if(ch13.Bits.DownlinkWordOrderCodeBit){ data |= 0200; } // WORD ORDER BIT
			/*
			sprintf(oapiDebugString(),"CMC DATA: %o (%lo %lo)",data,sat->agc.GetOutputChannel(034),
				sat->agc.GetOutputChannel(035));		
				*/
			tx_data[tx_offset] = data; 
			break;
		case 9: // 51DS1B COMPUTER DIGITAL DATA (40 BITS)
		case 29:
			data = (sat->agc.GetOutputChannel(034)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 10: // 51DS1C COMPUTER DIGITAL DATA (40 BITS) 
		case 30:
			// PARITY OF CH 34 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(035)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 11: // 51DS1D COMPUTER DIGITAL DATA (40 BITS) 
		case 31:
			data = (sat->agc.GetOutputChannel(035)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 12: // 51DS1E COMPUTER DIGITAL DATA (40 BITS) 
		case 32:
			// PARITY OF CH 35 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 13: // 51DP2 UP-DATA-LINK VALIDITY BITS (4 BITS)
		case 33:
			tx_data[tx_offset] = 0; 
			break;
		case 14:
			switch(frame_count){
				case 0: // 10A123
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A126
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A129
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A132
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A135
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 15:
			switch(frame_count){
				case 0: // 10A138
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A141 H2 TK 1 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A144 H2 TK 2 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A147 O2 TK 1 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A150 O2 TK 1 PRESS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 16:
			switch(frame_count){
				case 0: // 10A3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A6
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A9
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A12
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A15
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 17:
			switch(frame_count){
				case 0: // 10A18
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A21
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A24
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A27
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A30
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 18:
			switch(frame_count){
				case 0: // 10A33 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A36 H2 TK 1 PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A39 H2 TK 2 PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A42 O2 TK 2 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A45
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 19:
			switch(frame_count){
				case 0: // 10A48 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A51
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A54 O2 TK 1 TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A57 O2 TK 2 TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A60 H2 TK 1 TEMP
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 20:
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();
			// and continue
			switch(frame_count){
				case 0: // 10DP1 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP6
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP27
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP15
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP20
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 21:
			switch(frame_count){
				case 0: // SRC 0
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP7
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP28
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP16
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP21
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 22:
			switch(frame_count){
				case 0: // 11A39
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A147 AC BUS 1 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A84
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A21
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A129
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 23:
			switch(frame_count){
				case 0: // 11A40
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A48
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A85
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A22
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A130
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 24:
			switch(frame_count){
				case 0: // 11A73
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A10
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A118
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A55
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A163
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 25:
			switch(frame_count){
				case 0: // 11A74
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A11
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A119
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A56 AC BUS 2 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A164
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 26:
			switch(frame_count){
				case 0: // 11A75
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A12
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A120
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A57
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A165
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 27:
			switch(frame_count){
				case 0: // 11A76
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A13
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A121
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A58
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A166
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 34:
			switch(frame_count){
				case 0: // 11DP3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP8
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP13
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP29
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP22
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 35:
			switch(frame_count){
				case 0: // SRC 1
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP9
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP14
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP17
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP23
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 36:
			switch(frame_count){
				case 0: // 10A63 H2 TK 2 TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A66 O2 TK 2 PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A69
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A72
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A75
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 37:
			switch(frame_count){
				case 0: // 10A78
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A81
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A84
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A87
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A90
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 38:
			switch(frame_count){
				case 0: // 10A93
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A96
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A99
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A102
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A105
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 39:
			switch(frame_count){
				case 0: // 10A108
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A111
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A114
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A117
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A120
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 39){
		word_addr = 0;
		frame_addr++;
		if(frame_addr > 4){
			frame_addr = 0;
		}
		frame_count++;
		if(frame_count > 5){
			frame_count = 0;
		}
	}
}

void PCM::generate_stream_hbr(){
	unsigned char data=0;
	// 128 words per frame, 50 frames pre second
	switch(word_addr){
		case 0: // SYNC 1
			tx_data[tx_offset] = 05;
			break;
		case 1: // SYNC 2
			tx_data[tx_offset] = 0171;
			break;
		case 2: // SYNC 3
			tx_data[tx_offset] = 0267;
			break;
		case 3: // SYNC 4 & FRAME COUNT
			tx_data[tx_offset] = (0300|frame_count);
			break;
		case 4: // 22A1
		case 36:
		case 68:
		case 100:
			tx_data[tx_offset] = 0;
			break;
		case 5: // 22A2
		case 37:
		case 69:
		case 101:
			tx_data[tx_offset] = 0;
			break;
		case 6: // 22A3
		case 38:
		case 70:
		case 102:
			tx_data[tx_offset] = 0;
			break;
		case 7: // 22A4
		case 39:
		case 71:
		case 103:
			tx_data[tx_offset] = 0;
			break;
		case 8:
			switch(frame_count){
				case 0: // 11A1
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A37
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A73
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A109
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A145
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 9: 
			switch(frame_count){
				case 0: // 11A2
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A38
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A74
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A110
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A146
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 10:
			switch(frame_count){
				case 0: // 11A3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A39
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A75
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A111
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A147 AC BUS 1 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 11:
			switch(frame_count){
				case 0: // 11A4
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A40
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A76
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A112
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A148
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 12: // 12A1
		case 76:
			tx_data[tx_offset] = 0;
			break;
		case 13: // 12A2
		case 77:
			tx_data[tx_offset] = 0;
			break;
		case 14: // 12A3
		case 78:
			tx_data[tx_offset] = 0;
			break;
		case 15: // 12A4
		case 79:
			tx_data[tx_offset] = 0;
			break;
		case 16:
			switch(frame_count){
				case 0: // 11A5
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A41
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A77
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A113
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A149
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 17: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 18: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 19: // MAGICAL WORD SRC-0
			tx_data[tx_offset] = 0;
			break;
		case 20: // 12A5
		case 84:
			tx_data[tx_offset] = 0;
			break;
		case 21: // 12A6
		case 85:
			tx_data[tx_offset] = 0;
			break;
		case 22: // 12A7
		case 86:
			tx_data[tx_offset] = 0;
			break;
		case 23: // 12A8
		case 87:
			tx_data[tx_offset] = 0;
			break;
		case 24:
			switch(frame_count){
				case 0: // 11A6
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A42
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A78
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A114
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A150
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 25:
			switch(frame_count){
				case 0: // 11A7
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A43
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A79
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A115
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A151
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 26:
			switch(frame_count){
				case 0: // 11A8
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A44
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A80
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A116
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A152
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 27:
			switch(frame_count){
				case 0: // 11A9
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A45
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A81
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A117
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A153
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 28: // 51A1
			tx_data[tx_offset] = 0;
			break;
		case 29: // 51A2
			tx_data[tx_offset] = 0;
			break;
		case 30: // 51A3
			tx_data[tx_offset] = 0;
			break;
		case 31: // 51DS1A COMPUTER DIGITAL DATA (40 BITS)
			// The very first pass through this loop will get garbage data because there was no downrupt.
			// Generating a downrupt at 0 doesn't give the CMC enough time to get data on the busses.
			ChannelValue13 ch13;
			ch13.Value = sat->agc.GetOutputChannel(013);			
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			if(ch13.Bits.DownlinkWordOrderCodeBit){ data |= 0200; } // WORD ORDER BIT
			/*
			sprintf(oapiDebugString(),"CMC DATA: %o (%lo %lo)",data,sat->agc.GetOutputChannel(034),
				sat->agc.GetOutputChannel(035));
			*/			
			tx_data[tx_offset] = data; 
			break;
		case 32: // 51DS1B COMPUTER DIGITAL DATA (40 BITS)
			data = (sat->agc.GetOutputChannel(034)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 33: // 51DS1C COMPUTER DIGITAL DATA (40 BITS)
			// PARITY OF CH 34 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(035)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 34: // 51DS1C COMPUTER DIGITAL DATA (40 BITS)
			data = (sat->agc.GetOutputChannel(035)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 35: // 51DS1E COMPUTER DIGITAL DATA (40 BITS)
			// PARITY OF CH 35 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			tx_data[tx_offset] = data; 
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();			
			break;
		case 40:
			switch(frame_count){
				case 0: // 11A10
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A46
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A82
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A118
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A154
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 41:
			switch(frame_count){
				case 0: // 11A11
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A47
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A83
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A119
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A155
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 42:
			switch(frame_count){
				case 0: // 11A12
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A48
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A84
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A120
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A156
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 43:
			switch(frame_count){
				case 0: // 11A13
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A49
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A85
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A121
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A157
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 44: // 12A9
		case 108:
			tx_data[tx_offset] = 0;
			break;
		case 45: // 12A10
		case 109:
			tx_data[tx_offset] = 0;
			break;
		case 46: // 12A11
		case 110:
			tx_data[tx_offset] = 0;
			break;
		case 47: // 12A12
		case 111:
			tx_data[tx_offset] = 0;
			break;
		case 48:
			switch(frame_count){
				case 0: // 11A14
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A50
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A86
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A122
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A158
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 49: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 50: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 51: // MAGICAL WORD 2
			tx_data[tx_offset] = 0;
			break;
		case 52: // 12A13
		case 116:
			tx_data[tx_offset] = 0;
			break;
		case 53: // 12A14
		case 117:
			tx_data[tx_offset] = 0;
			break;
		case 54: // 12A15
		case 118:
			tx_data[tx_offset] = 0;
			break;
		case 55: // 12A16
		case 119:
			tx_data[tx_offset] = 0;
			break;
		case 56:
			switch(frame_count){
				case 0: // 11A15
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A51
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A87
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A123
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A159
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 57:
			switch(frame_count){
				case 0: // 11A16
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A52
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A88
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A124
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A160
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 58:
			switch(frame_count){
				case 0: // 11A17
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A53
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A89
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A125
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A161
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 59:
			switch(frame_count){
				case 0: // 11A18
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A54
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A90
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A126
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A162
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 60: // 51A4
			tx_data[tx_offset] = 0;
			break;
		case 61: // 51A5
			tx_data[tx_offset] = 0;
			break;
		case 62: // 51A6
			tx_data[tx_offset] = 0;
			break;
		case 63: // 51A7
			tx_data[tx_offset] = 0;
			break;
		case 64:
			switch(frame_count){
				case 0: // 11DP2A
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP6
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP13
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP20
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP27
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 65:
			switch(frame_count){
				case 0: // 11DP2B
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP7
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP14
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP21
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP28
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 66:
			switch(frame_count){
				case 0: // 11DP2C
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP8
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP15
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP22
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP29
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 67:
			switch(frame_count){
				case 0: // 11DP2D
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP9
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP16
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP23
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP30
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 72: 
			switch(frame_count){
				case 0: // 11A19
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A55
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A91
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A127
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A163
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 73:
			switch(frame_count){
				case 0: // 11A20
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A56 AC BUS 2 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A92
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A128
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A164
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 74:
			switch(frame_count){
				case 0: // 11A21
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A57
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A93
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A129
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A165
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 75:
			switch(frame_count){
				case 0: // 11A22
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A58
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A94
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A130
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A166
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 80:
			switch(frame_count){
				case 0: // 11A23
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A59
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A95
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A131
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A167
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 81: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 82: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 83: // MAGICAL WORD 3
			tx_data[tx_offset] = 0;
			break;
		case 88:
			switch(frame_count){
				case 0: // 11A24
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A60
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A96
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A132
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A168
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 89:
			switch(frame_count){
				case 0: // 11A25
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A61
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A97
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A133
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A169
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 90:
			switch(frame_count){
				case 0: // 11A26
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A62
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A98
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A134
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A170
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 91:
			switch(frame_count){
				case 0: // 11A27
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A63
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A99
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A135
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A171
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 92: // 51A8
			tx_data[tx_offset] = 0;
			break;
		case 93: // 51A9
			tx_data[tx_offset] = 0;
			break;
		case 94: // 51A10
			tx_data[tx_offset] = 0;
			break;
		case 95: // 51A11
			tx_data[tx_offset] = 0;
			break;
		case 96:
			switch(frame_count){
				case 0: // 11DP3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP10
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP17
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP24
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP31
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 97:
			switch(frame_count){
				case 0: // 11DP4
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP11
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP18
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP25
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP32
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 98:
			switch(frame_count){
				case 0: // 11DP5
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP12
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP19
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP26
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP33
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 99: // 51DP2
			tx_data[tx_offset] = 0;
			break;
		case 104:
			switch(frame_count){
				case 0: // 11A28
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A64
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A100
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A136
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A172
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 105:
			switch(frame_count){
				case 0: // 11A29
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A65
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A101
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A137
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A173
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 106:
			switch(frame_count){
				case 0: // 11A30
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A66
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A102
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A138
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A174
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 107:
			switch(frame_count){
				case 0: // 11A31
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A67
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A103
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A139
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A175
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 112:
			switch(frame_count){
				case 0: // 11A32 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A68
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A104
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A140
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A176
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 113: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 114: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 115: // MAGICAL WORD 4
			tx_data[tx_offset] = 0;
			break;
		case 120:
			switch(frame_count){
				case 0: // 11A33 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A69
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A105
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A141
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A177
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 121:
			switch(frame_count){
				case 0: // 11A34
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A70
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A106
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A142
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A178
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 122:
			switch(frame_count){
				case 0: // 11A35
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A71
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A107
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A143
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A179
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 123:
			switch(frame_count){
				case 0: // 11A36
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A72
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A108
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A143
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A180
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 124: // 51A12
			tx_data[tx_offset] = 0;
			break;
		case 125: // 51A13
			tx_data[tx_offset] = 0;
			break;
		case 126: // 51A14
			tx_data[tx_offset] = 0;
			break;
		case 127: // 51A15
			tx_data[tx_offset] = 0;
			break;

		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 127){
		word_addr = 0;
		frame_addr++;
		if(frame_addr > 4){
			frame_addr = 0;
		}
		frame_count++;
		if(frame_count > 50){
			frame_count = 0;
		}
	}
}

void PCM::perform_io(){
	// Do TCP IO
	switch(conn_state){
		case 0: // UNINITIALIZED
			break;
		case 1: // INITALIZED, LISTENING
			// Try to accept
			AcceptSocket = accept( m_socket, NULL, NULL );
			if(AcceptSocket != INVALID_SOCKET){
				conn_state = 2; // Accept this!
				wsk_error = 0; // For now
			}
			// Otherwise loop and try again.
			break;
		case 2: // CONNECTED			
			int bytesSent;

			bytesSent = send(AcceptSocket, (char *)tx_data, tx_size, 0 );
			if(bytesSent == SOCKET_ERROR){
				long errnumber = WSAGetLastError();
				switch(errnumber){
					// KNOWN CODES that we can ignore
					case 10053: // Software caused connection abort
					case 10054: // Connection reset by peer
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						break;

					default:           // If unknown
						wsk_error = 1; // do this
						sprintf(wsk_emsg,"TELECOM: send() failed: %ld",errnumber);
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						break;					
				}
			}
			break;			
	}
}
