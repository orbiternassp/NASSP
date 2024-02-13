/****************************************************************************
This file is part of Project Apollo - NASSP

Lunar Targeting Program (Header)

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

#pragma once

#include "Orbitersdk.h"
#include "RTCCTables.h"
#include "RTCCModule.h"

struct LunarTargetingProgramInput
{
	EphemerisData sv_in;
	double mass;
	double pitch_guess;
	double yaw_guess;
	double bt_guess;
	double tig_guess; //In GET

	double lat_tgt;
	double lng_tgt;
	double TB8; //Time of TB8 since GRR
};

struct LunarTargetingProgramOutput
{
	double tig = 0.0; //Time since TB8
	double pitch = 0.0;
	double yaw = 0.0;
	double bt = 0.0;

	double lat_imp = 0.0;
	double lng_imp = 0.0;
	double get_imp = 0.0;

	int err = 0;
	double FlybyAlt = 0.0; //Nautical miles, only in case of no impact
};

struct LUNTARGeneralizedIteratorArray
{
	//Stored inputs
	double dv, dgamma, dpsi;

	//Outputs
	EphemerisData sv_tig_apo;
	double mass_f;
	double lat;
	double lng;
	double gmt_imp;
	double dv_R;
};

class LunarTargetingProgram : public RTCCModule
{
public:
	LunarTargetingProgram(RTCC *r);
	void Call(const LunarTargetingProgramInput &in, LunarTargetingProgramOutput &out);
	bool TrajectoryComputerSTR(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool TrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);

	LUNTARGeneralizedIteratorArray outarray;
protected:
	bool ConvergeOnImpactSTR(double dv, double dgamma, double dpsi, double lat, double lng);
	bool ConvergeOnImpact(double dv, double dgamma, double dpsi, double lat, double lng);
	EphemerisData SimulateBurn(double pitch, double yaw, double dv, double &mass_f, double &bt);
	VECTOR3 BurnAttitude(double pitch, double yaw);

	void RVIO(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &theta, double &phi, double &gamma, double&psi);
	void BURN(VECTOR3 R, VECTOR3 V, double dv, double dgamma, double dpsi, double &dv_R, double &mfm0, VECTOR3 &RF, VECTOR3 &VF);

	EphemerisData sv_tig;
	VECTOR3 RLS;
	double mass;
	double F, isp;
};