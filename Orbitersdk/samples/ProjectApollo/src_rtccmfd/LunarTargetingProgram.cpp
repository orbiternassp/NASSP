/****************************************************************************
This file is part of Project Apollo - NASSP

Lunar Targeting Program

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

#include "LunarTargetingProgram.h"
#include "OrbMech.h"
#include "GeneralizedIterator.h"
#include "rtcc.h"

LunarTargetingProgram::LunarTargetingProgram(RTCC *r) : RTCCModule(r)
{
	F = 2.0*310.0;
	isp = (290.0 * G);
}

void LunarTargetingProgram::Call(const LunarTargetingProgramInput &in, LunarTargetingProgramOutput &out)
{
	mass = in.mass;
	out.err = 0;

	//Calculate impact site coordinates
	RLS = OrbMech::r_from_latlong(in.lat_tgt, in.lng_tgt, pRTCC->SystemParameters.MCSMLR);

	EphemerisData sv_tig_apo, sv_imp;
	int ITS;

	//Propagate to TIG, if applicable
	if (in.tig_guess != 0.0)
	{
		double mass_f;

		pRTCC->PMMCEN(in.sv_in, 0.0, 0.0, 1, pRTCC->GMTfromGET(in.tig_guess) - in.sv_in.GMT, 1.0, sv_tig, ITS);

		//Simulate maneuver, if applicable
		if (in.bt_guess != 0.0)
		{
			double dv_guess, bt;

			dv_guess = F / mass * in.bt_guess;
			sv_tig_apo = SimulateBurn(in.pitch_guess, in.yaw_guess, dv_guess, mass_f, bt);
		}
		else
		{
			sv_tig_apo = sv_tig;
		}
	}
	else
	{
		sv_tig_apo = in.sv_in;
	}
	//Propagate to impact
	pRTCC->PMMCEN(sv_tig_apo, 0.0, 5.0*24.0*3600.0, 3, pRTCC->SystemParameters.MCSMLR, 1.0, sv_imp, ITS);
	
	if (ITS != 3 || sv_imp.RBI != BODY_MOON)
	{
		//Initial guess didn't impact
		out.err = 1;
		//Find closest approach
		pRTCC->PMMCEN(sv_tig_apo, 0.0, 5.0*24.0*3600.0, 2, 0.0, 1.0, sv_imp, ITS);
		out.FlybyAlt = (length(sv_imp.R) - pRTCC->SystemParameters.MCSMLR) / 1852.0;
		return;
	}

	if (in.tig_guess == 0.0)
	{
		//Impact evaluation
		double alt;
		pRTCC->GLSSAT(sv_imp.R, sv_imp.GMT, sv_imp.RBI, out.lat_imp, out.lng_imp, alt);
		out.get_imp = pRTCC->GETfromGMT(sv_imp.GMT);
		out.tig = 0.0;
		out.pitch = 0.0;
		out.yaw = 0.0;
		out.bt = 0.0;
		return;
	}

	//Get initial guess for spherical coordinates
	double r, v1, v2, gamma1, gamma2, psi1, psi2, theta, phi, dv_mcc, dgamma_mcc, dpsi_mcc;

	RVIO(true, sv_tig.R, sv_tig.V, r, v1, theta, phi, gamma1, psi1);
	RVIO(true, sv_tig_apo.R, sv_tig_apo.V, r, v2, theta, phi, gamma2, psi2);

	dv_mcc = v2 - v1;
	dgamma_mcc = gamma2 - gamma1;
	dpsi_mcc = psi2 - psi1;

	//Converge on impact in STR coordinates
	bool err = ConvergeOnImpactSTR(dv_mcc, dgamma_mcc, dpsi_mcc, in.lat_tgt, in.lng_tgt);
	if (err)
	{
		out.err = 2;
		return;
	}

	//Converge and optimize lat/lng
	err = ConvergeOnImpact(outarray.dv, outarray.dgamma, outarray.dpsi, in.lat_tgt, in.lng_tgt);
	if (err)
	{
		out.err = 2;
		return;
	}

	//Calculate output values
	MATRIX3 Mat;
	VECTOR3 DV, BurnVec, BurnVec_LVLH;
	DV = outarray.sv_tig_apo.V - sv_tig.V;
	BurnVec = unit(DV);
	Mat = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);
	BurnVec_LVLH = mul(Mat, BurnVec);

	out.pitch = atan2(-BurnVec_LVLH.z, BurnVec_LVLH.x);
	out.yaw = asin(BurnVec_LVLH.y);
	out.bt = length(DV) / (F / mass);

	//Just for debugging
	//EphemerisData sv_tig_apo2 = SimulateBurn(out.pitch, out.yaw, length(DV), mass_f, out.bt);

	double temp = sv_tig.GMT - pRTCC->GetIUClockZero();
	out.tig = temp - in.TB8;
	out.lat_imp = outarray.lat;
	out.lng_imp = outarray.lng;
	out.get_imp = pRTCC->GETfromGMT(outarray.gmt_imp);
}

bool LunarTargetingProgram::ConvergeOnImpactSTR(double dv, double dgamma, double dpsi, double lat, double lng)
{
	void *constPtr;

	constPtr = &outarray;

	bool LunarTargetingProgramTrajectoryComputerSTRPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &LunarTargetingProgramTrajectoryComputerSTRPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv;
	block.IndVarGuess[1] = dgamma;
	block.IndVarGuess[2] = dpsi;
	block.IndVarStep[0] = 0.001;
	block.IndVarStep[1] = pow(2, -19);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[1] = true;
	block.DepVarSwitch[2] = true;
	block.DepVarLowerLimit[0] = -10000.0;
	block.DepVarLowerLimit[1] = -10000.0;
	block.DepVarLowerLimit[2] = -10000.0;
	block.DepVarUpperLimit[0] = 10000.0;
	block.DepVarUpperLimit[1] = 10000.0;
	block.DepVarUpperLimit[2] = 10000.0;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 1;
	block.DepVarClass[2] = 1;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

bool LunarTargetingProgram::ConvergeOnImpact(double dv, double dgamma, double dpsi, double lat, double lng)
{
	void *constPtr;

	constPtr = &outarray;

	bool LunarTargetingProgramTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &LunarTargetingProgramTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = dv;
	block.IndVarGuess[1] = dgamma;
	block.IndVarGuess[2] = dpsi;
	block.IndVarStep[0] = 0.001;
	block.IndVarStep[1] = pow(2, -19);
	block.IndVarStep[2] = pow(2, -19);
	block.IndVarWeight[0] = 512.0;
	block.IndVarWeight[1] = 512.0;
	block.IndVarWeight[2] = 512.0;
	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[1] = true;
	block.DepVarSwitch[2] = true;
	block.DepVarLowerLimit[0] = lat - 0.1*RAD;
	block.DepVarLowerLimit[1] = lng - 0.1*RAD;
	block.DepVarLowerLimit[2] = mass + 100.0;
	block.DepVarUpperLimit[0] = lat + 0.1*RAD;
	block.DepVarUpperLimit[1] = lng + 0.1*RAD;
	block.DepVarUpperLimit[2] = mass + 100.0;
	block.DepVarWeight[2] = 1.0;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 1;
	block.DepVarClass[2] = 3;

	std::vector<double> result;
	std::vector<double> y_vals;
	return GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
}

EphemerisData LunarTargetingProgram::SimulateBurn(double pitch, double yaw, double dv, double &mass_f, double &bt)
{
	EphemerisData sv_out = sv_tig;
	VECTOR3 Att = BurnAttitude(pitch, yaw);
	sv_out.V = sv_tig.V + Att*dv;
	bt = dv / (F / mass);
	mass_f = mass - bt * F / isp;
	return sv_out;
}

VECTOR3 LunarTargetingProgram::BurnAttitude(double pitch, double yaw)
{
	MATRIX3 Mat;
	double SPITCH, CPITCH, SYAW, CYAW;

	SPITCH = sin(pitch);
	CPITCH = cos(pitch);
	SYAW = sin(yaw);
	CYAW = cos(yaw);

	Mat = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	return tmul(Mat, _V(CPITCH*CYAW, SYAW, -SPITCH * CYAW));
}

void LunarTargetingProgram::BURN(VECTOR3 R, VECTOR3 V, double dv, double dgamma, double dpsi, double &dv_R, double &mfm0, VECTOR3 &RF, VECTOR3 &VF)
{
	double r, v;
	RF = R;

	r = length(R);
	v = length(V);

	VECTOR3 Rdot1, Rdot2;
	double d, h;

	d = dotp(R, V);
	h = length(crossp(R, V));
	Rdot1 = V * cos(dgamma) + (R*v*v - V * d) / h * sin(dgamma);
	Rdot2 = R * 2.0*dotp(R, Rdot1) / r / r * pow(sin(dpsi / 2.0), 2) + Rdot1 * cos(dpsi) - crossp(R, Rdot1) / r * sin(dpsi);
	VF = Rdot2 * (1.0 + dv / v);
	dv_R = sqrt(dv*dv + 4.0*v*(v + dv)*(pow(sin(dgamma / 2.0), 2) + (h*h*cos(dgamma) - h * d*sin(dgamma)) / (r*r*v*v)*pow(sin(dpsi / 2.0), 2)));
	mfm0 = exp(-dv_R / isp);
}

void LunarTargetingProgram::RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double&psi)
{
	if (vecinp)
	{
		r = length(R);
		v = length(V);
		phi = asin(R.z / r);
		theta = atan2(R.y, R.x);
		if (theta < 0)
		{
			theta += PI2;
		}
		gamma = asin(dotp(R, V) / r / v);
		psi = atan2(R.x*V.y - R.y*V.x, V.z*r - R.z*dotp(R, V) / r);
		//if (psi < 0)
		//{
		//	psi += PI2;
		//}
	}
	else
	{
		R = _V(cos(phi)*cos(theta), cos(phi)*sin(theta), sin(phi))*r;
		V = mul(_M(cos(phi)*cos(theta), -sin(theta), -sin(phi)*cos(theta), cos(phi)*sin(theta), cos(theta), -sin(phi)*sin(theta), sin(phi), 0, cos(phi)), _V(sin(gamma), cos(gamma)*sin(psi), cos(gamma)*cos(psi))*v);
	}
}

bool LunarTargetingProgramTrajectoryComputerSTRPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((LunarTargetingProgram*)data)->TrajectoryComputerSTR(var, varPtr, arr, mode);
}

bool LunarTargetingProgram::TrajectoryComputerSTR(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	//Independent variables:
	//0: MCC delta velocity in m/s
	//1: MCC delta flight-path angle in rad
	//2: MCC delta azimuth in rad
	//Dependent variables:
	//0: X-axis error in impact coordinates, meters
	//1: Y-axis error in impact coordinates, meters
	//2: Z-axis error in impact coordinates, meters

	LUNTARGeneralizedIteratorArray *vars;
	vars = static_cast<LUNTARGeneralizedIteratorArray*>(varPtr);

	EphemerisData sv_imp;
	double mfm0;
	int ITS;

	vars->dv = var[0];
	vars->dgamma = var[1];
	vars->dpsi = var[2];

	//Simulate burn
	BURN(sv_tig.R, sv_tig.V, vars->dv, vars->dgamma, vars->dpsi, vars->dv_R, mfm0, vars->sv_tig_apo.R, vars->sv_tig_apo.V);
	vars->sv_tig_apo.RBI = sv_tig.RBI;
	vars->sv_tig_apo.GMT = sv_tig.GMT;
	vars->mass_f = mass * mfm0;

	//Propagate to impact
	pRTCC->PMMCEN(vars->sv_tig_apo, 0.0, 5.0*24.0*3600.0, 3, pRTCC->SystemParameters.MCSMLR, 1.0, sv_imp, ITS);

	if (ITS != 3 || sv_imp.RBI != BODY_MOON)
	{
		//Didn't impact
		return true;
	}

	MATRIX3 M_MCI_MCT, M_STR;
	VECTOR3 R_EM, V_EM,  N, E, S, T, R, R_STR_imp, R_STR_des, DR_STR;
	double r, v, e, beta;

	pRTCC->PLEFEM(3, sv_imp.GMT / 3600.0, 0, &R_EM, &V_EM, NULL, &M_MCI_MCT);
	N = unit(crossp(R_EM, V_EM));

	r = length(sv_imp.R);
	v = length(sv_imp.V);
	E = (sv_imp.R*(v*v - OrbMech::mu_Moon / r) - sv_imp.V*dotp(sv_imp.R, sv_imp.V)) / OrbMech::mu_Moon;
	e = length(E);
	beta = acos(1.0 / e);
	S = unit(E)*cos(beta) + unit(crossp(N, E))*sin(beta);
	T = unit(crossp(S, N));
	R = crossp(S, T);

	M_STR = _M(R.x, R.y, R.z, S.x, S.y, S.z, T.x, T.y, T.z);

	R_STR_imp = mul(M_STR, sv_imp.R);
	R_STR_des = mul(M_STR, tmul(M_MCI_MCT, RLS));

	DR_STR = R_STR_des - R_STR_imp;

	arr[0] = DR_STR.x;
	arr[1] = DR_STR.y;
	arr[2] = DR_STR.z;
	return false;
}

bool LunarTargetingProgramTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((LunarTargetingProgram*)data)->TrajectoryComputer(var, varPtr, arr, mode);
}

bool LunarTargetingProgram::TrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	//Independent variables:
	//0: MCC delta velocity in m/s
	//1: MCC delta flight-path angle in rad
	//2: MCC delta azimuth in rad
	//Dependent variables:
	//0: Impact latitude in rad
	//1: Impact longitude in rad
	//2: Burnout mass in kg

	LUNTARGeneralizedIteratorArray *vars;
	vars = static_cast<LUNTARGeneralizedIteratorArray*>(varPtr);

	EphemerisData sv_imp;
	double mfm0, alt;
	int ITS;

	vars->dv = var[0];
	vars->dgamma = var[1];
	vars->dpsi = var[2];

	//Simulate burn
	BURN(sv_tig.R, sv_tig.V, vars->dv, vars->dgamma, vars->dpsi, vars->dv_R, mfm0, vars->sv_tig_apo.R, vars->sv_tig_apo.V);
	vars->mass_f = mass * mfm0;

	//Propagate to impact
	pRTCC->PMMCEN(vars->sv_tig_apo, 0.0, 5.0*24.0*3600.0, 3, pRTCC->SystemParameters.MCSMLR, 1.0, sv_imp, ITS);

	if (ITS != 3 || sv_imp.RBI != BODY_MOON)
	{
		//Didn't impact
		return true;
	}
	vars->gmt_imp = sv_imp.GMT;
	pRTCC->GLSSAT(sv_imp.R, sv_imp.GMT, sv_imp.RBI, vars->lat, vars->lng, alt);

	arr[0] = vars->lat;
	arr[1] = vars->lng;
	arr[2] = vars->mass_f;
	return false;
}