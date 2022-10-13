/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2022

InertialData class is the common source of inertial acceleration data
for variety of instruments (e.g. IMU, EMS, etc.).

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

#include "inertial.h"
#include "papi.h"

InertialData::InertialData(VESSEL *vessel) {
	this->vessel = vessel;
	accel = _V(0.0, 0.0, 0.0);
	dVInitialized = false;
}

VECTOR3 InertialData::GetGravityVector() {
	OBJHANDLE gravref = vessel->GetGravityRef();
	OBJHANDLE hSun = oapiGetObjectByName("Sun");
	VECTOR3 R, U_R;
	vessel->GetRelativePos(gravref, R);
	U_R = unit(R);
	double r = length(R);
	VECTOR3 R_S, U_R_S;
	vessel->GetRelativePos(hSun, R_S);
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
		vessel->GetRelativePos(hEarth, R_Ea);
		U_R_E = unit(R_Ea);
		double r_E = length(R_Ea);
		double mu_E = GGRAV * oapiGetMass(hEarth);

		a_dP -= U_R_E * mu_E / pow(r_E, 2.0);
	}

	return a_dP;
}


void InertialData::Timestep(double simdt) {
	VECTOR3 w, vel;

	vessel->GetWeightVector(w);
	vessel->GetGlobalVel(vel);
	vessel->GetRotationMatrix(rotmat);

	w = mul(rotmat, w) / vessel->GetMass();

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
		VECTOR3 avg = (dw1 + dw2) / 2.0;
		accel = tmul(rotmat, avg);
	}
}

void InertialData::getAcceleration(VECTOR3 &acc) {
	acc.x = accel.x;
	acc.y = accel.y;
	acc.z = accel.z;
}

void InertialData::SaveState(FILEHANDLE scn){
	oapiWriteLine(scn, INERTIAL_DATA_START_STRING);
	oapiWriteScenario_int(scn, "DVINITIALIZED", (dVInitialized ? 1 : 0));
	papiWriteScenario_vec(scn, "LASTWEIGHT", lastWeight);
	papiWriteScenario_vec(scn, "LASTGLOBALVEL", lastGlobalVel);
	papiWriteScenario_double(scn, "LASTSIMDT", lastSimDT);

	oapiWriteLine(scn, INERTIAL_DATA_END_STRING);
}

void InertialData::LoadState(FILEHANDLE scn) {
	int i;
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, INERTIAL_DATA_END_STRING, sizeof(INERTIAL_DATA_END_STRING))) {
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