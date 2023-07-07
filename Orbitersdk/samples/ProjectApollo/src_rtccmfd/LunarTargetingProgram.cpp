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

	//Propagate to TIG
	EphemerisData sv_imp, sv_tig_apo;
	double mass_f;
	int ITS;

	pRTCC->PMMCEN(in.sv_in, 0.0, 0.0, 1, in.tig_guess - in.sv_in.GMT, 1.0, sv_tig, ITS);

	//Simulate maneuver
	double dv_guess, bt;

	dv_guess = F / mass * in.bt_guess;
	sv_tig_apo = SimulateBurn(in.pitch_guess, in.yaw_guess, dv_guess, mass_f, bt);

	//Propagate to impact
	pRTCC->PMMCEN(sv_tig_apo, 0.0, 10.0*24.0*3600.0, 3, pRTCC->SystemParameters.MCSMLR, 1.0, sv_imp, ITS);
	
	if (ITS != 3)
	{
		//Initial guess didn't impact
		out.err = 1;
		return;
	}

	bool err = ConvergeOnImpact(in.pitch_guess, in.yaw_guess, dv_guess, in.lat_tgt, in.lng_tgt);
	if (err)
	{
		out.err = 2;
		return;
	}

	double temp = sv_tig.GMT - pRTCC->GetIUClockZero();
	out.tig = temp - in.TB8;

	out.pitch = outarray.pitch;
	out.yaw = outarray.yaw;
	out.bt = outarray.bt;
	out.lat_imp = outarray.lat;
	out.lng_imp = outarray.lng;
	out.get_imp = pRTCC->GETfromGMT(outarray.gmt_imp);
}

bool LunarTargetingProgram::ConvergeOnImpact(double pitch, double yaw, double dv, double lat, double lng)
{
	void *constPtr;

	constPtr = &outarray;

	bool LunarTargetingProgramTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &LunarTargetingProgramTrajectoryComputerPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarGuess[0] = pitch;
	block.IndVarGuess[1] = yaw;
	block.IndVarGuess[2] = dv;
	block.IndVarStep[0] = pow(2, -19);
	block.IndVarStep[1] = pow(2, -19);
	block.IndVarStep[2] = 0.001;
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

bool LunarTargetingProgramTrajectoryComputerPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((LunarTargetingProgram*)data)->TrajectoryComputer(var, varPtr, arr, mode);
}

bool LunarTargetingProgram::TrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	LUNTARGeneralizedIteratorArray *vars;
	vars = static_cast<LUNTARGeneralizedIteratorArray*>(varPtr);

	EphemerisData sv_imp;
	double alt;
	int ITS;

	vars->pitch = var[0];
	vars->yaw = var[1];
	vars->dv = var[2];

	if (vars->dv < 0)
	{
		//Not allowed
		return true;
	}

	vars->sv_tig_apo = SimulateBurn(vars->pitch, vars->yaw, vars->dv, vars->mass_f, vars->bt);

	//Propagate to impact
	pRTCC->PMMCEN(vars->sv_tig_apo, 0.0, 10.0*24.0*3600.0, 3, pRTCC->SystemParameters.MCSMLR, 1.0, sv_imp, ITS);

	if (ITS != 3)
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