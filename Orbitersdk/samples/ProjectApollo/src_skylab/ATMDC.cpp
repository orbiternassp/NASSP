/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2023

  Apollo Telescope Mount Digital Computer

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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/NASSP-LICENSE.txt
  for more details.

**************************************************************************/

#include "ATMDC.h"
#include "papi.h"

MATRIX3 MatrixRH_LH(MATRIX3 A)
{
	return _M(A.m11, A.m13, A.m12, A.m31, A.m33, A.m32, A.m21, A.m23, A.m22);
}

MATRIX3 tmat(MATRIX3 A)
{
	return _M(A.m11, A.m21, A.m31, A.m12, A.m22, A.m32, A.m13, A.m23, A.m33);
}

double atan3(double x, double y)
{
	double a;

	a = atan2(x, y);
	if (a < 0.0)
	{
		a += PI2;
	}
	return a;
}

VECTOR3 CALCGAR(MATRIX3 SMNB)
{
	//Input: Attitude matrix
	//Output: Gimbal Angles
	VECTOR3 X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, A_MG;
	double sinOGA, cosOGA, sinMGA, cosMGA, sinIGA, cosIGA, OGA, IGA, MGA;

	X_SM = _V(1, 0, 0);
	Y_SM = _V(0, 1, 0);
	Z_SM = _V(0, 0, 1);
	X_NB = _V(SMNB.m11, SMNB.m12, SMNB.m13);
	Y_NB = _V(SMNB.m21, SMNB.m22, SMNB.m23);
	Z_NB = _V(SMNB.m31, SMNB.m32, SMNB.m33);

	A_MG = unit(crossp(X_NB, Y_SM));
	cosOGA = dotp(A_MG, Z_NB);
	sinOGA = dotp(A_MG, Y_NB);
	OGA = atan3(sinOGA, cosOGA);
	cosMGA = dotp(Y_SM, crossp(A_MG, X_NB));
	sinMGA = dotp(Y_SM, X_NB);
	MGA = atan3(sinMGA, cosMGA);
	cosIGA = dotp(A_MG, Z_SM);
	sinIGA = dotp(A_MG, X_SM);
	IGA = atan3(sinIGA, cosIGA);

	return _V(OGA, IGA, MGA);
}

ATMDC::ATMDC(VESSEL *v) : vessel(v)
{
	AttitudeControlMode = 0;
	for (int i = 0; i < 6; i++)
	{
		ThrusterDemand[i] = false;
	}
	M_AttHold = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);

	a_0p = a_0y = 1;
	a_0r = 1;
	a_1p = a_1y = 5;
	a_1r = 5;
	MaxErr = 2.5*RAD; //Results in max 0.3 deg/sec

	hEarth = oapiGetObjectByName("Earth");
	hSun = oapiGetObjectByName("Sun");
}

void ATMDC::Timestep()
{
	//ATTITUDE CONTROL
	if (AttitudeControlMode == 0)
	{
		for (int i = 0; i < 6; i++)
		{
			ThrusterDemand[i] = false;
		}
		return;
	}

	MATRIX3 M_G_S; //Current attitude matrix (global to body)
	VECTOR3 CurrentAtt, AttitudeError, AttRate;
	double eps_p, eps_ymr, eps_ypr; //Thruster demand values

	//Current attitude
	M_G_S = GetAttitudeMatrix();
	CurrentAtt = CALCGAR(M_G_S);

	//Calculate desired attitude
	AttitudeError = _V(0, 0, 0);

	if (AttitudeControlMode <= 4)
	{
		MATRIX3 M_G_DS; //Global to desired Skylab (body)
		VECTOR3 x_axis, y_axis, z_axis, DesiredAtt, DeltaAtt;
		double theta_xa, theta_za, A1, A2, A3, A4, A5;

		if (AttitudeControlMode == 1)
		{
			//Attitude hold

			M_G_DS = M_AttHold;
		}
		else if (AttitudeControlMode == 2)
		{
			//Solar inertial

			VECTOR3 pos_ows, vel_ows, pos_s, h_ows;

			vessel->GetRelativePos(hEarth, pos_ows);
			vessel->GetRelativeVel(hEarth, vel_ows);
			vessel->GetRelativePos(hSun, pos_s);

			pos_ows = _V(pos_ows.x, pos_ows.z, pos_ows.y);
			vel_ows = _V(vel_ows.x, vel_ows.z, vel_ows.y);
			pos_s = _V(pos_s.x, pos_s.z, pos_s.y);

			h_ows = unit(crossp(pos_ows, vel_ows));

			z_axis = unit(-pos_s);
			x_axis = -unit(crossp(z_axis, h_ows));
			y_axis = crossp(z_axis, x_axis);

			M_G_DS = _M(x_axis.x, x_axis.y, x_axis.z, y_axis.x, y_axis.y, y_axis.z, z_axis.x, z_axis.y, z_axis.z); //Ecliptic (right-handed) to Skylab desired
		}
		else
		{
			//ZLV

			VECTOR3 pos_ows, vel_ows, h_ows;

			vessel->GetRelativePos(hEarth, pos_ows);
			vessel->GetRelativeVel(hEarth, vel_ows);

			pos_ows = _V(pos_ows.x, pos_ows.z, pos_ows.y);
			vel_ows = _V(vel_ows.x, vel_ows.z, vel_ows.y);

			h_ows = unit(crossp(pos_ows, vel_ows));

			z_axis = unit(pos_ows);
			if (AttitudeControlMode == 3)
			{
				x_axis = unit(crossp(z_axis, -h_ows));
			}
			else
			{
				x_axis = unit(crossp(z_axis, h_ows));
			}
			y_axis = crossp(z_axis, x_axis);

			M_G_DS = _M(x_axis.x, x_axis.y, x_axis.z, y_axis.x, y_axis.y, y_axis.z, z_axis.x, z_axis.y, z_axis.z); //Ecliptic (right-handed) to Skylab desired
		}

		DesiredAtt = CALCGAR(M_G_DS);

		//sprintf(oapiDebugString(), "Cur %lf %lf %lf Des %lf %lf %lf", CurrentAtt.x, CurrentAtt.y, CurrentAtt.z, DesiredAtt.x, DesiredAtt.y, DesiredAtt.z);

		//Euler correction
		theta_xa = CurrentAtt.x;
		theta_za = CurrentAtt.z;

		A1 = cos(theta_xa) * cos(theta_za);
		A2 = sin(theta_xa);
		A3 = sin(theta_za);
		A4 = sin(theta_xa) * cos(theta_za);
		A5 = cos(theta_xa);

		DeltaAtt = CurrentAtt - DesiredAtt;

		for (int i = 0; i < 3; i++)
		{
			if (DeltaAtt.data[i] > PI)
			{
				DeltaAtt.data[i] -= PI2;
			}
			else if (DeltaAtt.data[i] < -PI)
			{
				DeltaAtt.data[i] += PI2;
			}
		}

		// ROLL ERROR
		AttitudeError.x = (DeltaAtt.x + A3 * DeltaAtt.y);
		// PITCH ERROR
		AttitudeError.y = (A1 * DeltaAtt.y + A2 * DeltaAtt.z);
		// YAW ERROR
		AttitudeError.z = (-A4 * DeltaAtt.y + A5 * DeltaAtt.z);

		for (int i = 0; i < 3; i++)
		{
			if (abs(AttitudeError.data[i]) > MaxErr)
			{
				if (AttitudeError.data[i] > 0.0)
				{
					AttitudeError.data[i] = MaxErr;
				}
				else
				{
					AttitudeError.data[i] = -MaxErr;
				}
			}
		}
	}
	else
	{
		//Manual

		if (vessel->GetManualControlLevel(THGROUP_ATT_BANKLEFT) > 0) {
			AttitudeError.x = vessel->GetManualControlLevel(THGROUP_ATT_BANKLEFT);
		}
		else if (vessel->GetManualControlLevel(THGROUP_ATT_BANKRIGHT) > 0) {
			AttitudeError.x = -vessel->GetManualControlLevel(THGROUP_ATT_BANKRIGHT);
		}
		// Pitch
		if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN) > 0) {
			AttitudeError.y = -vessel->GetManualControlLevel(THGROUP_ATT_PITCHDOWN);
		}
		else if (vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP) > 0) {
			AttitudeError.y = vessel->GetManualControlLevel(THGROUP_ATT_PITCHUP);
		}
		// Yaw
		if (vessel->GetManualControlLevel(THGROUP_ATT_YAWLEFT) > 0) {
			AttitudeError.z = -vessel->GetManualControlLevel(THGROUP_ATT_YAWLEFT);
		}
		else if (vessel->GetManualControlLevel(THGROUP_ATT_YAWRIGHT) > 0) {
			AttitudeError.z = vessel->GetManualControlLevel(THGROUP_ATT_YAWRIGHT);
		}
		AttitudeError *= 2.5*RAD; //Full deflection = maximum atitude rate (0.3 deg/sec)
	}

	//Attitude error limit for rate limiting

	//Rate gyros
	vessel->GetAngularVel(AttRate);
	AttRate = _V(AttRate.z, -AttRate.x, AttRate.y); //Orbiter to Skylab coordinate system

	//sprintf(oapiDebugString(), "Err %lf %lf %lf Rate %lf %lf %lf", AttitudeError.x*DEG, AttitudeError.y*DEG, AttitudeError.z*DEG, AttRate.x*DEG, AttRate.y*DEG, AttRate.z*DEG);

	//Thruster selection
	eps_p = (a_0p * AttitudeError.y * DEG) + (a_1p * AttRate.y * DEG); //pitch thruster demand
	eps_ymr = (a_0y * AttitudeError.z * DEG) - (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) - (a_1r * AttRate.x * DEG); //yaw minus roll
	eps_ypr = (a_0y * AttitudeError.z * DEG) + (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) + (a_1r * AttRate.x * DEG); //yaw plus roll

	if (eps_p > 1.0) ThrusterDemand[0] = true;
	else ThrusterDemand[0] = false;

	if (eps_p < -1.0) ThrusterDemand[3] = true;
	else ThrusterDemand[3] = false;

	if (eps_ymr > 1.0) ThrusterDemand[4] = true;
	else ThrusterDemand[4] = false;

	if (eps_ymr < -1.0) ThrusterDemand[5] = true;
	else ThrusterDemand[5] = false;

	if (eps_ypr > 1.0) ThrusterDemand[1] = true;
	else ThrusterDemand[1] = false;

	if (eps_ypr < -1.0) ThrusterDemand[2] = true;
	else ThrusterDemand[2] = false;
}

bool ATMDC::GetThrusterDemand(int i)
{
	if (i < 0 || i > 5) return false;

	return ThrusterDemand[i];
}

MATRIX3 ATMDC::GetAttitudeMatrix()
{
	MATRIX3 M_V_G; //vehicle to global
	MATRIX3 M_S_G; //Skylab to global

	vessel->GetRotationMatrix(M_V_G); //Local to global (left-handed)
	M_V_G = MatrixRH_LH(M_V_G);//Local to global (right-handed)

	static const MATRIX3 M_S_V = _M(0, -1, 0, 1, 0, 0, 0, 0, 1); //Skylab (right-handed) to local (right-handed)

	M_S_G = mul(M_V_G, M_S_V); //Skylab to Global (right handed)
	return tmat(M_S_G);
}

void ATMDC::SetAttitudeControlMode(int mode)
{
	if (mode < 0 || mode > 5) return;

	AttitudeControlMode = mode;

	if (mode == 1)
	{
		//Save current attitude as reference
		M_AttHold = GetAttitudeMatrix();
	}
}

void ATMDC::CommandSystem(int Function, int Command)
{
	switch (Function)
	{
	case 052016: //SEL APCS MODE
		switch (Command)
		{
		case 050000: //STANDBY
			SetAttitudeControlMode(0);
			break;
		case 050002: //SOLAR INERTIAL
			SetAttitudeControlMode(2);
			break;
		case 050003: //ZLV
			SetAttitudeControlMode(3);
			break;
		case 050005: //ATT HOLD TACS
			SetAttitudeControlMode(1);
			break;
		}
		break;
	}
}

void ATMDC::SaveState(FILEHANDLE scn)
{
	oapiWriteLine(scn, ATMDC_START_STRING);

	oapiWriteScenario_int(scn, "ATTITUDECONTROLMODE", AttitudeControlMode);
	papiWriteScenario_mx(scn, "M_ATTHOLD", M_AttHold);

	oapiWriteLine(scn, ATMDC_END_STRING);
}

void ATMDC::LoadState(FILEHANDLE scn)
{
	char *line;
	double flt = 0;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, ATMDC_END_STRING, sizeof(ATMDC_END_STRING)))
			return;
		papiReadScenario_int(line, "ATTITUDECONTROLMODE", AttitudeControlMode);
		papiReadScenario_mat(line, "M_ATTHOLD", M_AttHold);
	}
}