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
  *	Revision 1.17  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.16  2007/01/22 15:48:17  tschachim
  *	SPS Thrust Vector Control, RHC power supply, THC clockwise switch, bugfixes.
  *	
  *	Revision 1.15  2007/01/20 02:09:51  dseagrav
  *	Tweaked RCS positions
  *	
  *	Revision 1.14  2007/01/13 08:20:53  dseagrav
  *	Moved telecommunications code to new file csm_telecom.cpp (Adjust VS6,VS2005 files accordingly please)
  *	
  *	Revision 1.13  2007/01/10 02:57:38  dseagrav
  *	Much improved downtelemetry data-gathering method using measure()
  *	
  *	Revision 1.12  2007/01/06 23:08:32  dseagrav
  *	More telecom stuff. A lot of the S-band signal path exists now, albeit just to consume electricity.
  *	
  *	Revision 1.11  2007/01/06 07:34:36  dseagrav
  *	FLIGHT bus added, uptelemetry now draws power, UPTLM switches on MDC now operate
  *	
  *	Revision 1.10  2007/01/06 04:44:49  dseagrav
  *	Corrected CREW ALARM command behavior, PCM downtelemetry generator now draws power
  *	
  *	Revision 1.9  2007/01/02 01:38:25  dseagrav
  *	Digital uplink and associated stuff.
  *	
  *	Revision 1.8  2006/12/26 06:24:43  dseagrav
  *	vAGC restart if not powered, AGC VOLTAGE ALARM simulated with DSKY RESTART lights, more telemetry stuff, Merry Day-After-Christmas!
  *	
  *	Revision 1.7  2006/12/25 11:16:48  dseagrav
  *	More telemetry bugfixes, corrects stupid mistakes in HBR
  *	
  *	Revision 1.6  2006/12/24 09:14:36  dseagrav
  *	Telemetry enhancements, now allows connection from remote hosts.
  *	
  *	Revision 1.5  2006/12/19 15:56:10  tschachim
  *	ECS test stuff, bugfixes.
  *	
  *	Revision 1.4  2006/12/17 04:35:24  dseagrav
  *	Telecom bugfixes, eliminate false error on client disconnect, vAGC now gets cycles by a different method, eliminated old and unused vAGC P11 debugging code that was eating up FPS on every timestep.
  *	
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
#include "lvimu.h"
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
	// TODO DC power is needed, too
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
				  sat->eca.rhc_x < 36863) || sat->rjec.SPSActive) && sat->GSwitch.IsDown()) {
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
				  sat->eca.rhc_y < 36863) || sat->rjec.SPSActive) && sat->GSwitch.IsDown()) {
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
				  sat->eca.rhc_z < 36863) || sat->rjec.SPSActive) && sat->GSwitch.IsDown()) {
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

	// Do we have power?
	// TODO DC power is needed, too
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2 ||
	    sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE || 
		sat->StabContSystemAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) {

		// Reset Attitude
		SetAttitude(_V(0, 0, 0));
		return;
	}
}

bool GDC::AlignGDC() {
	// User pushed the Align GDC button.
	// Set the GDC attitude to match what's on the ASCP.
	if (sat->FDAIAttSetSwitch.IsDown()) {
		SetAttitude(_V(sat->ascp.output.x * 0.017453,
					   sat->ascp.output.y * 0.017453,
					   sat->ascp.output.z * 0.017453)); // Degrees to radians
		return true;
	} 
	return false;	
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

	// CM or SM thrusters
	int sm_sep = 0;
	if (sat->GetStage() > CSM_LEM_STAGE) sm_sep = 1;

	// Direct ullage thrust
	bool directUllageActive = false;
	if (sat->DirectUllageButton.GetState() == 1 && !sm_sep) {
		sat->SetRCSState(RCS_SM_QUAD_A, 3, 0);
		sat->SetRCSState(RCS_SM_QUAD_A, 4, 0);
		sat->SetRCSState(RCS_SM_QUAD_B, 3, 0);
		sat->SetRCSState(RCS_SM_QUAD_B, 4, 0);
		sat->SetRCSState(RCS_SM_QUAD_C, 3, 0);
		sat->SetRCSState(RCS_SM_QUAD_C, 4, 0);
		sat->SetRCSState(RCS_SM_QUAD_D, 3, 0);
		sat->SetRCSState(RCS_SM_QUAD_D, 4, 0);

		if (sat->DirectUllMnACircuitBraker.Voltage() >= SP_MIN_DCVOLTAGE) {   
			sat->SetRCSState(RCS_SM_QUAD_B, 4, 1);
			sat->SetRCSState(RCS_SM_QUAD_D, 3, 1);
		}
		if (sat->DirectUllMnBCircuitBraker.Voltage() >= SP_MIN_DCVOLTAGE) {   
			sat->SetRCSState(RCS_SM_QUAD_A, 4, 1); 
			sat->SetRCSState(RCS_SM_QUAD_C, 3, 1);
		}
		directUllageActive = true;
	}

	// Ensure AC power
	if (sat->SIGCondDriverBiasPower1Switch.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->SIGCondDriverBiasPower2Switch.Voltage() < SP_MIN_ACVOLTAGE) return;
	
	int thruster = 1;
	int thruster_lockout;
	while(thruster < 17) {
		// THRUSTER LOCKOUT CHECKING
		thruster_lockout = 0;
		// If it's a pitch or yaw jet, lockout on SPS thrusting
		if (thruster < 9 && SPSActive != 0) { thruster_lockout = 1; } 
		// Lockout on direct axes.
		if (thruster < 5 && (DirectPitchActive != 0 || directUllageActive)) { thruster++; continue; } // Skip entirely
		if (thruster > 4 && thruster < 9 && (DirectYawActive != 0 || directUllageActive)) { thruster++; continue; } 
		if (thruster > 8 && DirectRollActive != 0) { thruster++; continue; } 
		// THRUSTER PROCESSING
		switch(thruster) {
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
	trans_x_trigger = 0;
	trans_y_trigger = 0;
	trans_z_trigger = 0;
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

	sat->SystemMnACircuitBraker.DrawPower(10);	// TODO real power is unknown
	sat->SystemMnBCircuitBraker.DrawPower(10);	// TODO real power is unknown
}

void ECA::TimeStep(double simdt) {

	// Do we have power?
	if (!IsPowered()) return;

	// SCS is in control if the THC is CLOCKWISE 
	// or if the SC CONT switch is set to SCS.

	// TODO: TVC CW is supplied by SCS LOGIC BUS 2
	// TODO: SC CONT switch is supplied by ???
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
	if (sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN || sat->THCRotary.IsClockwise()) {
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
		int x_def = 0, y_def = 0, z_def = 0;

		if(rhc_ac_x < 28673){ // MINUS 
			x_def = 28673-rhc_ac_x; 
		}
		if(rhc_ac_x > 36863){ // PLUS 
			x_def = (36863-rhc_ac_x);
		}
		if(rhc_ac_y < 28673){ // MINUS 
			y_def = 28673-rhc_ac_y; 
		}
		if(rhc_ac_y > 36863){ // PLUS 
			y_def = (36863-rhc_ac_y);
		}
		if(rhc_ac_z < 28673){ // MINUS 
			z_def = 28673-rhc_ac_z; 
		}
		if(rhc_ac_z > 36863){ // PLUS 
			z_def = (36863-rhc_ac_z);
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
		// sprintf(oapiDebugString(),"SCS ROLL rate %.3f cmd %.3f pseudo %.3f error %.3f", sat->gdc.rates.z * DEG, cmd_rate.x * DEG, pseudorate.x * DEG, rate_err.x * DEG);
	}
	// ROTATION
	if (sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN  || sat->THCRotary.IsClockwise()) {
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
	if (sat->GetStage() > CSM_LEM_STAGE) sm_sep = 1;

	if ((sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN || sat->THCRotary.IsClockwise()) && !sm_sep){
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
		if(thc_y > 49152){ // MINUS Y (FORWARD)
			if(accel_pitch_flag > -1){ sat->rjec.SetThruster(1,1); }else{ sat->rjec.SetThruster(1,0); }
			if(accel_pitch_flag < 1 ){ sat->rjec.SetThruster(2,1); }else{ sat->rjec.SetThruster(2,0); }
			if(accel_yaw_flag   > -1){ sat->rjec.SetThruster(5,1); }else{ sat->rjec.SetThruster(5,0); }
			if(accel_yaw_flag   < 1 ){ sat->rjec.SetThruster(6,1); }else{ sat->rjec.SetThruster(6,0); }
			trans_y_trigger=1; trans_y_flag=1;
		}
		if(thc_y < 16384){ // PLUS Y (BACKWARD)
			if(accel_pitch_flag > -1){ sat->rjec.SetThruster(3,1); }else{ sat->rjec.SetThruster(3,0); }
			if(accel_pitch_flag < 1 ){ sat->rjec.SetThruster(4,1); }else{ sat->rjec.SetThruster(4,0); }
			if(accel_yaw_flag   > -1){ sat->rjec.SetThruster(7,1); }else{ sat->rjec.SetThruster(7,0); }
			if(accel_yaw_flag   < 1 ){ sat->rjec.SetThruster(8,1); }else{ sat->rjec.SetThruster(8,0); }
			trans_y_trigger=1; trans_y_flag=1;
		}
		if(thc_z > 49152){ // MINUS Z (UP)
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(11,1); }else{ sat->rjec.SetThruster(11,0); }
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(12,1); }else{ sat->rjec.SetThruster(12,0); }
			trans_z_trigger=1; trans_z_flag=1;
		}
		if(thc_z < 16384){ // PLUS Z (DOWN)
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


// Entry Monitor Systen

#define EMS_STATUS_OFF			0
#define EMS_STATUS_STANDBY		1
#define EMS_STATUS_DVSET		2
#define EMS_STATUS_VHFRNG		3
#define EMS_STATUS_DV			4
#define EMS_STATUS_DV_BACKUP	5
#define EMS_STATUS_DVTEST		6
#define EMS_STATUS_DVTEST_DONE	7


EMS::EMS(PanelSDK &p) : DCPower(0, p) {

	status = EMS_STATUS_OFF;
	dVInitialized = false;
	lastWeight = _V(0, 0, 0);
	dVRangeCounter = 0;
	dVTestTime = 0;
	sat = NULL;
}

void EMS::Init(Saturn *vessel) {
	sat = vessel;
	DCPower.WireToBuses(&sat->EMSMnACircuitBraker, &sat->EMSMnBCircuitBraker);
}

void EMS::TimeStep(double simdt) {

	VESSELSTATUS vs;
	VECTOR3 w;
	double position;

	if (!IsPowered()) {
		status = EMS_STATUS_OFF; 
		return;
	}

	// Turn on reset
	if (status == EMS_STATUS_OFF && sat->EMSFunctionSwitch.GetState() != 0) {
		SwitchChanged();
	}

	switch (status) {
		case EMS_STATUS_DV:
		case EMS_STATUS_DV_BACKUP:
			sat->GetStatus(vs);
			sat->GetWeightVector(w);

			MATRIX3	tinv = AttitudeReference::GetRotationMatrixZ(-vs.arot.z);
			tinv = mul(AttitudeReference::GetRotationMatrixY(-vs.arot.y), tinv);
			tinv = mul(AttitudeReference::GetRotationMatrixX(-vs.arot.x), tinv);
			w = mul(tinv, w) / sat->GetMass();

			if (!dVInitialized) {
				lastWeight = w;
				dVInitialized = true;

			} else {
				// Acceleration calculation, see IMU
				VECTOR3 f;
				sat->GetForceVector(f);
				f = mul(tinv, f) / sat->GetMass();

				VECTOR3 dw1 = w - f;
				VECTOR3 dw2 = lastWeight - f;
				lastWeight = w;

				// Transform to vessel coordinates
				MATRIX3	t = AttitudeReference::GetRotationMatrixX(vs.arot.x);
				t = mul(AttitudeReference::GetRotationMatrixY(vs.arot.y), t);
				t = mul(AttitudeReference::GetRotationMatrixZ(vs.arot.z), t);

				VECTOR3 avg = (dw1 + dw2) / 2.0;
				avg = mul(t, avg);	
				double xacc = -avg.z;

				// Ground test switch
				if (sat->GTASwitch.IsUp()) {
					// Handle different gravity and size of the Earth
					if (sat->IsVirtualAGC()) {
						xacc -= 9.7916;		// the Virtual AGC needs nonspherical gravity anyway
					} else {
						if (sat->NonsphericalGravityEnabled()) {
							xacc -= 9.7988;
						} else {
							xacc -= 9.7939;
						}
					}
				}

				// dV/Range display
				if (xacc > 0 || status == EMS_STATUS_DV) {
					dVRangeCounter -= xacc * simdt * FPS;
					dVRangeCounter = max(-1000.0, min(14000.0, dVRangeCounter));
				}				
				//sprintf(oapiDebugString(), "xacc %.10f", xacc);
				//sprintf(oapiDebugString(), "Avg x %.10f y %.10f z %.10f l%.10f", avg.x, avg.y, avg.z, length(avg));								
			}
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

			dVRangeCounter = max(-1000.0, min(14000.0, dVRangeCounter));
			break;

		case EMS_STATUS_DVTEST:
			dVTestTime -= simdt;
			dVRangeCounter -= 160.0 * simdt;	// AOH SCS fig. 2.3-13

			if (dVTestTime < 0 || dVRangeCounter < -1000.0) {
				if (dVTestTime < -0.176875) {	// Minimum -41.5 when starting with 1586.8
					dVRangeCounter -= (dVTestTime + 0.176875) * 160.0;
				}
				dVTestTime = 0;
				dVRangeCounter = max(-1000.0, min(14000.0, dVRangeCounter));
				status = EMS_STATUS_DVTEST_DONE;
			}
			break;
	}
}

void EMS::SystemTimestep(double simdt) {

	if (IsPowered() && !IsOff()) {
		DCPower.DrawPower(93.28);	// see CSM Systems Handbook
	}
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

		default:
			status = EMS_STATUS_STANDBY;
			break;
	}
	// sprintf(oapiDebugString(),"EMSFunctionSwitch %d", sat->EMSFunctionSwitch.GetState());
}

bool EMS::SPSThrustLight() {

	if (!IsPowered()) return false;	
	
	if (status == EMS_STATUS_DVTEST) return true;
	if (sat->SPSEngine.IsThrustOn()) return true;
	return false;
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

bool EMS::IsPowered() {

	return DCPower.Voltage() > SP_MIN_DCVOLTAGE; 
}

void EMS::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, EMS_START_STRING);
	oapiWriteScenario_int(scn, "STATUS", status);
	oapiWriteScenario_int(scn, "DVINITIALIZED", (dVInitialized ? 1 : 0));
	oapiWriteScenario_vec(scn, "LASTWEIGHT", lastWeight);
	WriteScenario_double(scn, "DVRANGECOUNTER", dVRangeCounter);
	WriteScenario_double(scn, "DVTESTTIME", dVTestTime);

	oapiWriteLine(scn, EMS_END_STRING);
}

void EMS::LoadState(FILEHANDLE scn){

	int i;
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
			sscanf(line + 12, "%lf %lf %lf", &lastWeight.x, &lastWeight.y, &lastWeight.z);
		} else if (!strnicmp (line, "DVRANGECOUNTER", 14)) {
			sscanf(line + 14, "%lf", &dVRangeCounter);
		} else if (!strnicmp (line, "DVTESTTIME", 10)) {
			sscanf(line + 10, "%lf", &dVTestTime);
		}	
	}
}
