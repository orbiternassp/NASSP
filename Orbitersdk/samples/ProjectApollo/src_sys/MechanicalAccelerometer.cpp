/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Mechanical Accelerometer Simulation

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

#include "Orbitersdk.h"
#include "papi.h"
#include "MechanicalAccelerometer.h"

MechanicalAccelerometer::MechanicalAccelerometer()
{
	vessel = NULL;
	dVInitialized = false;
	lastWeight = _V(0, 0, 0);
	lastGlobalVel = _V(0, 0, 0);
	lastSimDT = 0.0;
	xacc = 0.0;
	yacc = 0.0;
}

void MechanicalAccelerometer::Init(VESSEL* v)
{
	vessel = v;
}

void MechanicalAccelerometer::TimeStep(double simdt) {

	VESSELSTATUS vs;
	VECTOR3 w, vel;

	vessel->GetStatus(vs);
	vessel->GetWeightVector(w);
	vessel->GetGlobalVel(vel);

	MATRIX3	tinv = GetRotationMatrixZ(-vs.arot.z);
	tinv = mul(GetRotationMatrixY(-vs.arot.y), tinv);
	tinv = mul(GetRotationMatrixX(-vs.arot.x), tinv);
	w = mul(tinv, w) / vessel->GetMass();

	if (!dVInitialized) {
		lastWeight = w;
		lastGlobalVel = vel;
		lastSimDT = simdt;
		dVInitialized = true;
	}
	else {
		// Acceleration calculation, see IMU
		VECTOR3 dvel = (vel - lastGlobalVel) / lastSimDT;
		VECTOR3 dw1 = w - dvel;
		VECTOR3 dw2 = lastWeight - dvel;
		lastWeight = w;
		lastGlobalVel = vel;
		lastSimDT = simdt;

		// Transform to vessel coordinates
		MATRIX3	t = GetRotationMatrixX(vs.arot.x);
		t = mul(GetRotationMatrixY(vs.arot.y), t);
		t = mul(GetRotationMatrixZ(vs.arot.z), t);
		VECTOR3 avg = (dw1 + dw2) / 2.0;
		avg = mul(t, avg);
		xacc = -avg.z;
		yacc = -avg.y;
	}

}

double MechanicalAccelerometer::GetXAccel()
{
	return xacc;
}

double MechanicalAccelerometer::GetYAccel()
{
	return yacc;
}

MATRIX3 MechanicalAccelerometer::GetRotationMatrixX(double angle) {
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

MATRIX3 MechanicalAccelerometer::GetRotationMatrixY(double angle) {
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

MATRIX3 MechanicalAccelerometer::GetRotationMatrixZ(double angle) {
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

void MechanicalAccelerometer::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, MECHACCEL_START_STRING);
	oapiWriteScenario_int(scn, "DVINITIALIZED", (dVInitialized ? 1 : 0));
	papiWriteScenario_vec(scn, "LASTWEIGHT", lastWeight);
	papiWriteScenario_vec(scn, "LASTGLOBALVEL", lastGlobalVel);
	papiWriteScenario_double(scn, "LASTSIMDT", lastSimDT);
	oapiWriteLine(scn, MECHACCEL_END_STRING);
}

void MechanicalAccelerometer::LoadState(FILEHANDLE scn) {

	int i;
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, MECHACCEL_END_STRING, sizeof(MECHACCEL_END_STRING))) {
			return;
		}

		if (!strnicmp(line, "DVINITIALIZED", 13)) {
			sscanf(line + 13, "%i", &i);
			dVInitialized = (i == 1);
		}
		else if (!strnicmp(line, "LASTWEIGHT", 10)) {
			sscanf(line + 10, "%lf %lf %lf", &lastWeight.x, &lastWeight.y, &lastWeight.z);
		}
		else if (!strnicmp(line, "LASTGLOBALVEL", 13)) {
			sscanf(line + 13, "%lf %lf %lf", &lastGlobalVel.x, &lastGlobalVel.y, &lastGlobalVel.z);
		}
		else if (!strnicmp(line, "LASTSIMDT", 9)) {
			sscanf(line + 9, "%lf", &lastSimDT);
		}
	}
}