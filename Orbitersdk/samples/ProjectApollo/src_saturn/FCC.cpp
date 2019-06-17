/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017-2019

Flight Control Computer

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

#include "LVIMU.h"
#include "IUControlDistributor.h"
#include "papi.h"
#include "iu.h"

#include "FCC.h"

FCC::FCC(IU *iu)
{
	SIBurnMode = false;
	SIVBBurnMode = false;
	SCControlEnableRelay = false;
	PermanentSCControlEnabled = false;

	a_0p = a_0y = a_0r = 0.0;
	a_1p = a_1y = a_1r = 0.0;
	beta_pc = beta_yc = beta_rc = 0.0;
	beta_y1c = beta_y2c = beta_y3c = beta_y4c = 0.0;
	beta_p1c = beta_p2c = beta_p3c = beta_p4c = 0.0;
	eps_p = eps_ymr = eps_ypr = 0.0;

	LVDCAttitudeError = _V(0.0, 0.0, 0.0);

	this->iu = iu;
}

void FCC::Init(IU *i)
{
	iu = i;
}

void FCC::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "SCCONTROLENABLERELAY", SCControlEnableRelay);
	papiWriteScenario_bool(scn, "PERMANENTSCCONTROLENABLED", PermanentSCControlEnabled);
	papiWriteScenario_vec(scn, "LVDCATTITUDEERROR", LVDCAttitudeError);

	oapiWriteLine(scn, end_str);
}

void FCC::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "SCCONTROLENABLERELAY", SCControlEnableRelay);
		papiReadScenario_bool(line, "PERMANENTSCCONTROLENABLED", PermanentSCControlEnabled);
		papiReadScenario_vec(line, "LVDCATTITUDEERROR", LVDCAttitudeError);

	}
}

FCC1B::FCC1B(IU *iu) : FCC(iu)
{

}

void FCC1B::Timestep(double simdt)
{
	if (iu == NULL) return;

	VECTOR3 AttRate, AttitudeError;

	//Input signals
	AttRate = iu->GetLVRG()->GetRates();
	SIBurnMode = iu->GetControlDistributor()->GetSIBurnMode();
	SIVBBurnMode = iu->GetControlDistributor()->GetSIVBBurnMode();
	SCControlEnableRelay = iu->GetEDS()->GetSCControl();

	// S/C takeover function
	if (SCControlEnableRelay) {
		//scaling factor seems to be 31.6; didn't find any source for it, but at least it leads to the right rates
		//note that any 'threshold solution' is pointless: ARTEMIS supports EMEM-selectable saturn rate output
		AttitudeError.x = iu->GetCommandConnector()->GetAGCAttitudeError(0) * RAD / 31.6;
		AttitudeError.y = iu->GetCommandConnector()->GetAGCAttitudeError(1) * RAD / 31.6;
		AttitudeError.z = iu->GetCommandConnector()->GetAGCAttitudeError(2) * RAD / -31.6;
	}
	else
	{
		AttitudeError = LVDCAttitudeError;
	}

	/* **** FLIGHT CONTROL COMPUTER OPERATIONS **** */
	if (SIBurnMode)
	{
		if (iu->GetControlDistributor()->GetFCCSwitchPoint3())
		{
			a_0p = a_0y = 0.7;
			a_0r = 0.2;
			a_1p = a_1y = 0.75;
			a_1r = 0.15;
			//sprintf(oapiDebugString(), "S-IB Mode, Gain 3");
		}
		else if (iu->GetControlDistributor()->GetFCCSwitchPoint2())
		{
			a_0p = a_0y = 2;
			a_0r = 0.3;
			a_1p = a_1y = 1.6;
			a_1r = 0.25;
			//sprintf(oapiDebugString(), "S-IB Mode, Gain 2");
		}
		else if (iu->GetControlDistributor()->GetFCCSwitchPoint1())
		{
			a_0p = a_0y = 3;
			a_0r = 0.3;
			a_1p = a_1y = 1.6;
			a_1r = 0.25;
			//sprintf(oapiDebugString(), "S-IB Mode, Gain 1");
		}
		else
		{
			a_0p = a_0y = 2;
			a_0r = 0.3;
			a_1p = a_1y = 1.6;
			a_1r = 0.25;
			//sprintf(oapiDebugString(), "S-IB Mode, None");
		}
	}
	else if (SIVBBurnMode)
	{
		a_0p = a_0y = 0.6;
		a_0r = 1;
		a_1p = a_1y = 0.5;
		a_1r = 5;
		//sprintf(oapiDebugString(), "S-IVB Burn Mode, None");
	}
	else
	{
		a_0p = a_0y = 1;
		a_0r = 1;
		a_1p = a_1y = 5;
		a_1r = 5;
		//sprintf(oapiDebugString(), "S-IVB Coast Mode, None");
	}

	beta_pc = a_0p * AttitudeError.y + a_1p * AttRate.y;
	beta_yc = a_0y * AttitudeError.z + a_1y * AttRate.z;
	beta_rc = a_0r * AttitudeError.x + a_1r * AttRate.x;

	if (SIBurnMode) {
		beta_y1c = beta_yc + beta_rc / pow(2, 0.5); //orbiter's engines are gimballed differently then the real one
		beta_p1c = beta_pc - beta_rc / pow(2, 0.5);
		beta_y2c = beta_yc - beta_rc / pow(2, 0.5);
		beta_p2c = beta_pc - beta_rc / pow(2, 0.5);
		beta_y3c = beta_yc - beta_rc / pow(2, 0.5);
		beta_p3c = beta_pc + beta_rc / pow(2, 0.5);
		beta_y4c = beta_yc + beta_rc / pow(2, 0.5);
		beta_p4c = beta_pc + beta_rc / pow(2, 0.5);

		iu->GetLVCommandConnector()->SetSIThrusterDir(0, beta_y1c, beta_p1c);
		iu->GetLVCommandConnector()->SetSIThrusterDir(1, beta_y2c, beta_p2c);
		iu->GetLVCommandConnector()->SetSIThrusterDir(2, beta_y3c, beta_p3c);
		iu->GetLVCommandConnector()->SetSIThrusterDir(3, beta_y4c, beta_p4c);
	}
	else if (SIVBBurnMode == true) {
		//SIVB powered flight
		beta_p1c = beta_pc; //gimbal angles
		beta_y1c = beta_yc;
		iu->GetLVCommandConnector()->SetSIVBThrusterDir(beta_y1c, beta_p1c);
		eps_p = 0; //we want neither the APS pitch thrusters to fire
		eps_ymr = -(a_0r * AttitudeError.x * DEG) - (a_1r * AttRate.x * DEG); //nor the yaw thrusters
		eps_ypr = (a_0r * AttitudeError.x * DEG) + (a_1r * AttRate.x * DEG);
	}
	else {
		//SIVB coast flight; full APS control
		eps_p = (a_0p * AttitudeError.y * DEG) + (a_1p * AttRate.y * DEG); //pitch thruster demand
		eps_ymr = (a_0y * AttitudeError.z * DEG) - (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) - (a_1r * AttRate.x * DEG); //yaw minus roll
		eps_ypr = (a_0y * AttitudeError.z * DEG) + (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) + (a_1r * AttRate.x * DEG); //yaw plus roll
	}

	if (SIBurnMode == false) {
		//APS thruster on/off control
		if (eps_p > 1) {
			//fire+pitch
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, true);
			//if (eps_p >= 1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, 1); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, (eps_p - 1) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, false);
		}
		if (eps_p < -1) {
			//fire-pitch
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, true);
			//if (eps_p <= -1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, 1); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, (-eps_p - 1) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, false);
		}
		if (eps_ymr > 1) {
			//fire+yaw-roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, true);
			//if (eps_ymr >= 1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, 1); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, (eps_ymr - 1) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, false);
		}
		if (eps_ymr < -1) {
			//fire-yaw+roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, true);
			//if (eps_ymr <= -1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, 1); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, (-eps_ymr - 1) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, false);
		}
		if (eps_ypr > 1) {
			//fire+yaw+roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, true);
			//if (eps_ypr >= 1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, 1); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, (eps_ypr - 1) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, false);
		}
		if (eps_ypr < -1) {
			//fire-yaw-roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, true);
			//if (eps_ypr <= -1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, 1); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, (-eps_ypr - 1) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, false);
		}
	}
}

FCCSV::FCCSV(IU *iu) : FCC(iu)
{
	SIIBurnMode = false;
	SICOrSIIBurnMode = false;
}

void FCCSV::Timestep(double simdt)
{
	if (iu == NULL) return;

	VECTOR3 AttRate, AttitudeError;

	//Input Signals
	AttRate = iu->GetLVRG()->GetRates();
	SIBurnMode = iu->GetControlDistributor()->GetSIBurnMode();
	SIIBurnMode = iu->GetControlDistributor()->GetSIIBurnMode();
	SICOrSIIBurnMode = SIBurnMode || SIIBurnMode;
	SIVBBurnMode = iu->GetControlDistributor()->GetSIVBBurnMode();
	SCControlEnableRelay = iu->GetEDS()->GetSCControl();

	if (SCControlEnableRelay) {
		//scaling factor seems to be 31.6; didn't find any source for it, but at least it leads to the right rates
		//note that any 'threshold solution' is pointless: ARTEMIS supports EMEM-selectable saturn rate output
		AttitudeError.x = iu->GetCommandConnector()->GetAGCAttitudeError(0) * RAD / 31.6;
		AttitudeError.y = iu->GetCommandConnector()->GetAGCAttitudeError(1) * RAD / 31.6;
		AttitudeError.z = iu->GetCommandConnector()->GetAGCAttitudeError(2) * RAD / -31.6;
	}
	else
	{
		AttitudeError = LVDCAttitudeError;
	}

	/* **** FLIGHT CONTROL COMPUTER OPERATIONS **** */
	if (SIBurnMode)
	{
		if (iu->GetControlDistributor()->GetFCCSwitchPoint2())
		{
			a_0p = a_0y = 0.32;
			a_0r = 0.32;
			a_1p = a_1y = 0.30;
			a_1r = 0.30;
			//sprintf(oapiDebugString(), "S-IC Mode, Gain 2");
		}
		else if (iu->GetControlDistributor()->GetFCCSwitchPoint1())
		{
			a_0p = a_0y = 0.45;
			a_0r = 0.45;
			a_1p = a_1y = 0.44;
			a_1r = 0.44;
			//sprintf(oapiDebugString(), "S-IC Mode, Gain 1");
		}
		else
		{
			a_0p = a_0y = 0.9;
			a_0r = 0.9;
			a_1p = a_1y = 0.69;
			a_1r = 0.69;
			//sprintf(oapiDebugString(), "S-IC Mode, None");
		}
	}
	else if (SIIBurnMode)
	{
		if (iu->GetControlDistributor()->GetFCCSwitchPoint4())
		{
			a_0p = a_0y = 0.44;
			a_0r = 0.44;
			a_1p = a_1y = 0.74;
			a_1r = 0.74;
			//sprintf(oapiDebugString(), "S-II Mode, Gain 4");
		}
		else if (iu->GetControlDistributor()->GetFCCSwitchPoint3())
		{
			a_0p = a_0y = 0.65;
			a_0r = 0.65;
			a_1p = a_1y = 1.1;
			a_1r = 1.1;
			//sprintf(oapiDebugString(), "S-II Mode, Gain 3");
		}
		else
		{
			a_0p = a_0y = 1.12;
			a_0r = 1.12;
			a_1p = a_1y = 1.9;
			a_1r = 1.9;
			//sprintf(oapiDebugString(), "S-II Mode, None");
		}
	}
	else if (SIVBBurnMode)
	{
		if (iu->GetControlDistributor()->GetFCCSwitchPoint6())
		{
			a_0p = a_0y = 0.81;
			a_0r = 1;
			a_1p = a_1y = 0.7;
			a_1r = 5;
			//sprintf(oapiDebugString(), "S-IVB Burn Mode, Gain 6");
		}
		else
		{
			a_0p = a_0y = 0.81;
			a_0r = 1;
			a_1p = a_1y = 0.97;
			a_1r = 5;
			//sprintf(oapiDebugString(), "S-IVB Burn Mode, None");
		}
	}
	else
	{
		if (iu->GetControlDistributor()->GetFCCSwitchPoint5())
		{
			a_0p = a_0y = 1;
			a_0r = 1;
			a_1p = a_1y = 5;
			a_1r = 5;
			//sprintf(oapiDebugString(), "S-IVB Coast Mode, Gain 5");
		}
		else
		{
			a_0p = a_0y = 1;
			a_0r = 1;
			a_1p = a_1y = 5;
			a_1r = 5;
			//sprintf(oapiDebugString(), "S-IVB Coast Mode, None");
		}
	}

	beta_pc = a_0p * AttitudeError.y + a_1p * AttRate.y;
	beta_yc = a_0y * AttitudeError.z + a_1y * AttRate.z;
	beta_rc = a_0r * AttitudeError.x + a_1r * AttRate.x;
	if (SICOrSIIBurnMode) {
		//orbiter's engines are gimballed differently then the real one
		beta_p1c = beta_pc + beta_rc / pow(2, 0.5);
		beta_p2c = beta_pc + beta_rc / pow(2, 0.5);
		beta_p3c = beta_pc - beta_rc / pow(2, 0.5);
		beta_p4c = beta_pc - beta_rc / pow(2, 0.5);
		beta_y1c = beta_yc + beta_rc / pow(2, 0.5);
		beta_y2c = beta_yc - beta_rc / pow(2, 0.5);
		beta_y3c = beta_yc - beta_rc / pow(2, 0.5);
		beta_y4c = beta_yc + beta_rc / pow(2, 0.5);
		if (SIBurnMode) {
			//SIC
			iu->GetLVCommandConnector()->SetSIThrusterDir(0, beta_y1c, beta_p1c);
			iu->GetLVCommandConnector()->SetSIThrusterDir(1, beta_y2c, beta_p2c);
			iu->GetLVCommandConnector()->SetSIThrusterDir(2, beta_y3c, beta_p3c);
			iu->GetLVCommandConnector()->SetSIThrusterDir(3, beta_y4c, beta_p4c);
		}
		else {
			//SII: engines 2 & 4 are flipped!
			iu->GetLVCommandConnector()->SetSIIThrusterDir(0, beta_y2c, beta_p2c);
			iu->GetLVCommandConnector()->SetSIIThrusterDir(1, beta_y4c, beta_p4c);
			//1 & 3 are the same on both stages
			iu->GetLVCommandConnector()->SetSIIThrusterDir(2, beta_y1c, beta_p1c);
			iu->GetLVCommandConnector()->SetSIIThrusterDir(3, beta_y3c, beta_p3c);
		}
	}
	else if (SIVBBurnMode) {
		//SIVB powered flight
		beta_p1c = beta_pc; //gimbal angles
		beta_y1c = beta_yc;
		iu->GetLVCommandConnector()->SetSIVBThrusterDir(beta_y1c, beta_p1c);
		eps_p = 0; //we want neither the APS pitch thrusters to fire
		eps_ymr = -(a_0r * AttitudeError.x * DEG) - (a_1r * AttRate.x * DEG); //nor the yaw thrusters
		eps_ypr = (a_0r * AttitudeError.x * DEG) + (a_1r * AttRate.x * DEG);
	}
	else {
		//SIVB coast flight; full APS control
		eps_p = (a_0p * AttitudeError.y * DEG) + (a_1p * AttRate.y * DEG); //pitch thruster demand
		eps_ymr = (a_0y * AttitudeError.z * DEG) - (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) - (a_1r * AttRate.x * DEG); //yaw minus roll
		eps_ypr = (a_0y * AttitudeError.z * DEG) + (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) + (a_1r * AttRate.x * DEG); //yaw plus roll
	}

	if (SICOrSIIBurnMode == false) {
		//APS thruster on/off control
		if (eps_p > 1) {
			//fire+pitch
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, true);
			//if (eps_p >= 1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, 1.0); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, (eps_p - 1.0) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(1, false);
		}
		if (eps_p < -1) {
			//fire-pitch
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, true);
			//if (eps_p <= -1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, 1.0); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, (-eps_p - 1.0) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(0, false);
		}
		if (eps_ymr > 1) {
			//fire+yaw-roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, true);
			//if (eps_ymr >= 1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, 1.0); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, (eps_ymr - 1.0) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(3, false);
		}
		if (eps_ymr < -1) {
			//fire-yaw+roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, true);
			//if (eps_ymr <= -1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, 1.0); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, (-eps_ymr - 1.0) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(5, false);
		}
		if (eps_ypr > 1) {
			//fire+yaw+roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, true);
			//if (eps_ypr >= 1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, 1.0); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, (eps_ypr - 1.0) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(4, false);
		}
		if (eps_ypr < -1) {
			//fire-yaw-roll;
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, true);
			//if (eps_ypr <= -1.6) { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, 1.0); }
			//else { iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, (-eps_ypr - 1.0) / 0.6); }
		}
		else {
			iu->GetLVCommandConnector()->SetAPSAttitudeEngine(2, false);
		}
	}

	//sprintf(oapiDebugString(), "%d %d %d %f %f %f", GainSwitch, StageSwitch, SIVBBurnMode, AttitudeError.x*DEG, AttitudeError.y*DEG, AttitudeError.z*DEG);
	//sprintf(oapiDebugString(), "%f %f %f", eps_p, eps_ymr, eps_ypr);
}