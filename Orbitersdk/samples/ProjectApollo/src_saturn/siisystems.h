/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-II Systems (Header)

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

#include "j2engine.h"

#define PUVALVE_CLOSED 0
#define PUVALVE_NULL 1
#define PUVALVE_OPEN 2

class SIISystems
{
public:
	SIISystems(VESSEL *v, THRUSTER_HANDLE *j2, PROPELLANT_HANDLE &j2prop, THGROUP_HANDLE &ull, Pyro &SII_Inter, Pyro &SII_SIVB_Sep, Sound &pushifts, Sound &SepS);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	void LVDCEnginesCutoff();
	void LVDCCenterEngineCutoff();
	void LVDCEnginesCutoffReset();
	void SetEnginesReadyBypass();
	void ResetEnginesReadyBypass();
	void EngineStartOn();
	void EngineStartOff();
	void EDSEnginesCutoff(bool cut);
	void SetDepletionSensorsCutoffArm() { PointLevelSensorArmed = true; }
	void SetDepletionSensorsCutoffDisarm() { PointLevelSensorArmed = false; }
	void FireUllageTrigger() { UllageTrigger = true; }
	void SetOrdnanceArm() { OrdnanceArmed = true; }
	void SetSIISIVBOrdnanceArm() { SIISIVBOrdnanceArmed = true; }
	void SetThrusterDir(int n, double beta_y, double beta_p);
	void SetPUValve(int state);
	void StartPhaseLimiterCutoffArmReset() { StartPhaseLimiterCutoffArm = false; }
	void LH2StepPressurizationReset() { LH2StepPressurization = false; }

	void RecalculateEngineParameters();
	void RecalculateEngineParameters(double BaseThrust);
	void SetMixtureRatio(double ratio);
	void SwitchSelector(int channel);

	void SetEngineFailed(int n);

	void GetThrustOK(bool *ok);
	bool GetPropellantDepletionEngineCutoff();

	//To IU
	double GetLH2TankUllagePressurePSI() { return LH2TankUllagePressurePSI; }

protected:
	void GetJ2ISP(double ratio, double &isp, double &ThrustAdjust);
	bool PropellantLowLevel();

	bool ThrustOK[5];

	int PUValveState;

	//K20
	bool StartPhaseLimiterCutoffArm;
	//K18 (LOX), K23 (LH2)
	bool PointLevelSensorArmed;
	//K16 (K37)
	bool SICSIIEngineStartLockout;
	//K17 (K43)
	bool CommandEngineStart;
	//K50
	bool LH2StepPressurization;
	bool PropellantDepletionSensors;
	bool UllageTrigger;
	bool OrdnanceArmed;
	bool SIISIVBOrdnanceArmed;

	double J2DefaultThrust;

	double LH2TankUllagePressurePSI;

	VESSEL *vessel;
	J2Engine j2engine1;
	J2Engine j2engine2;
	J2Engine j2engine3;
	J2Engine j2engine4;
	J2Engine j2engine5;
	J2Engine *j2engines[5];
	THGROUP_HANDLE &ullage;
	PROPELLANT_HANDLE &main_propellant;

	Pyro &SII_Interstage_Pyros;
	Pyro &SII_SIVB_Separation_Pyros;

	Sound &puShiftSound;
	Sound &sepSound;
};

#define SIISYSTEMS_START_STRING		"SIISYSTEMS_BEGIN"
#define SIISYSTEMS_END_STRING		"SIISYSTEMS_END"