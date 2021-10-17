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
	double tig_guess;

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
};

struct LUNTARGeneralizedIteratorArray
{
	double pitch;
	double yaw;
	double dv;
	EphemerisData sv_tig_apo;
	double bt;
	double mass_f;
	double lat;
	double lng;
	double gmt_imp;
};

class LunarTargetingProgram : public RTCCModule
{
public:
	LunarTargetingProgram(RTCC *r);
	void Call(const LunarTargetingProgramInput &in, LunarTargetingProgramOutput &out);
	bool TrajectoryComputer(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);

	LUNTARGeneralizedIteratorArray outarray;
protected:
	bool ConvergeOnImpact(double pitch, double yaw, double dv, double lat, double lng);
	EphemerisData SimulateBurn(double pitch, double yaw, double dv, double &mass_f, double &bt);
	VECTOR3 BurnAttitude(double pitch, double yaw);

	EphemerisData sv_tig;
	double mass;
	double F, isp;
};