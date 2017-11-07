/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-II Systems

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

#include "soundlib.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "siisystems.h"

SIISystems::SIISystems(Saturn *v, THRUSTER_HANDLE *j2, PROPELLANT_HANDLE &j2prop, THGROUP_HANDLE &ull, Sound &pushifts, Sound &SepS)
	:j2engines(j2), main_propellant(j2prop), ullage(ull), puShiftSound(pushifts), sepSound(SepS)
{
	int i;

	vessel = v;

	for (i = 0;i < 5;i++)
	{
		ThrustOK[i] = false;
		EarlySIICutoff[i] = false;
		SecondStageFailureTime[i] = 0.0;
	}

	PUValveState = PUVALVE_NULL;

	LVDCStartCommand = false;
	EngineStart = false;
	EnginesRunning = false;
	LVDCEnginesStopRelay = true;
	EnginesReady = false;
	EnginesReadyBypass = false;
	PointLevelSensorArmed = false;
	UllageTrigger = false;
	EDSEnginesStop = false;
	RSSEnginesStop = false;
	EnginesStop = false;
	PropellantDepletionSensors = false;
	OrdnanceArmed = false;
	SIISIVBOrdnanceArmed = false;

	ThrustTimer = 0.0;
	ThrustLevel = 0.0;
	FailureTimer = 0.0;
}

void SIISystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SIISYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "ENGINESTART", EngineStart);
	papiWriteScenario_bool(scn, "LVDCSTARTCOMMAND", LVDCStartCommand);
	papiWriteScenario_bool(scn, "LVDCENGINESSTOPRELAY", LVDCEnginesStopRelay);
	papiWriteScenario_bool(scn, "EDSENGINESSTOP", EDSEnginesStop);
	papiWriteScenario_bool(scn, "RSSENGINESSTOP", RSSEnginesStop);
	papiWriteScenario_bool(scn, "ENGINESSTOP", EnginesStop);
	papiWriteScenario_bool(scn, "ENGINESREADY", EnginesReady);
	papiWriteScenario_bool(scn, "ENGINESREADYBYPASS", EnginesReadyBypass);
	papiWriteScenario_bool(scn, "ENGINESRUNNING", EnginesRunning);
	papiWriteScenario_bool(scn, "ULLAGETRIGGER", UllageTrigger);
	papiWriteScenario_bool(scn, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
	papiWriteScenario_bool(scn, "ORDNANCEARMED", OrdnanceArmed);
	papiWriteScenario_bool(scn, "SIISIVBORDNANCEARMED", SIISIVBOrdnanceArmed);
	oapiWriteScenario_int(scn, "PUVALVESTATE", PUValveState);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	papiWriteScenario_double(scn, "THRUSTLEVEL", ThrustLevel);
	papiWriteScenario_double(scn, "J2DEFAULTTHRUST", J2DefaultThrust);
	papiWriteScenario_boolarr(scn, "EARLYSIICUTOFF", EarlySIICutoff, 5);
	papiWriteScenario_doublearr(scn, "SECONDSTAGEFAILURETIME", SecondStageFailureTime, 5);

	oapiWriteLine(scn, SIISYSTEMS_END_STRING);
}

void SIISystems::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SIISYSTEMS_END_STRING, sizeof(SIISYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "ENGINESTART", EngineStart);
		papiReadScenario_bool(line, "LVDCSTARTCOMMAND", LVDCStartCommand);
		papiReadScenario_bool(line, "LVDCENGINESSTOPRELAY", LVDCEnginesStopRelay);
		papiReadScenario_bool(line, "EDSENGINESSTOP", EDSEnginesStop);
		papiReadScenario_bool(line, "RSSENGINESSTOP", RSSEnginesStop);
		papiReadScenario_bool(line, "ENGINESSTOP", EnginesStop);
		papiReadScenario_bool(line, "ENGINESREADY", EnginesReady);
		papiReadScenario_bool(line, "ENGINESREADYBYPASS", EnginesReadyBypass);
		papiReadScenario_bool(line, "ENGINESRUNNING", EnginesRunning);
		papiReadScenario_bool(line, "ULLAGETRIGGER", UllageTrigger);
		papiReadScenario_bool(line, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
		papiReadScenario_bool(line, "ORDNANCEARMED", OrdnanceArmed);
		papiReadScenario_bool(line, "SIISIVBORDNANCEARMED", SIISIVBOrdnanceArmed);
		papiReadScenario_int(line, "PUVALVESTATE", PUValveState);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
		papiReadScenario_double(line, "THRUSTLEVEL", ThrustLevel);
		papiReadScenario_double(line, "J2DEFAULTTHRUST", J2DefaultThrust);
		papiReadScenario_boolarr(line, "EARLYSIICUTOFF", EarlySIICutoff, 5);
		papiReadScenario_doublearr(line, "SECONDSTAGEFAILURETIME", SecondStageFailureTime, 5);
	}
}

void SIISystems::Timestep(double simdt)
{
	if (j2engines[0] == NULL) return;

	//Thrust OK switch
	for (int i = 0;i < 5;i++)
	{
		ThrustOK[i] = vessel->GetThrusterLevel(j2engines[i]) > 0.65;
	}

	//Propellant Depletion
	if (PropellantLowLevel())
	{
		PropellantDepletionSensors = true;
	}
	else
	{
		PropellantDepletionSensors = false;
	}

	if (!EnginesRunning)
	{
		EnginesReady = true;
	}
	else
	{
		EnginesReady = false;
	}

	if ((EnginesReady || EnginesReadyBypass) && LVDCStartCommand)
	{
		EngineStart = true;
	}
	else
	{
		EngineStart = false;
	}

	if (LVDCEnginesStopRelay || EDSEnginesStop || RSSEnginesStop || PropellantDepletionSensors)
	{
		EnginesStop = true;
	}
	else
	{
		EnginesStop = false;
	}

	if (EnginesStop)
	{
		EnginesRunning = false;

		if (ThrustLevel > 0.0)
		{
			ThrustTimer += simdt;

			// Cutoff transient thrust
			if (ThrustTimer < 2.0) {
				if (ThrustTimer < 0.25) {
					// 95% of thrust dies in the first .25 second
					ThrustLevel = 1.0 - (ThrustTimer*3.3048);
					SetThrusterGroupLevel(ThrustLevel);
				}
				else {
					if (ThrustTimer < 1.5) {
						// The remainder dies over the next 1.25 second
						ThrustLevel = 0.1738 - ((ThrustTimer - 0.25)*0.1390);
						SetThrusterGroupLevel(ThrustLevel);
					}
					else {
						// Engine is completely shut down at 1.5 second
						ThrustLevel = 0.0;
						SetThrusterGroupLevel(ThrustLevel);
					}
				}
			}
		}
	}
	else if (EngineStart && !EnginesRunning)
	{
		ThrustTimer += simdt;

		//First Burn
		if (ThrustTimer >= 1.0 && ThrustTimer < 3.2) {
			ThrustLevel = (ThrustTimer - 1.0)*0.45;
			SetThrusterGroupLevel(ThrustLevel);
		}
		else if (ThrustTimer > 3.2 && ThrustLevel < 1.0)
		{
			ThrustLevel = 1.0;
			SetThrusterGroupLevel(ThrustLevel);

			EnginesRunning = true;
		}
	}
	else if (ThrustTimer > 0.0)
	{
		ThrustTimer = 0.0;
	}

	//Ullage engines
	if (OrdnanceArmed && ullage)
	{
		if (vessel->GetThrusterGroupLevel(ullage) < 1.0 && UllageTrigger)
		{
			vessel->SetThrusterGroupLevel(ullage, 1.0);
			sepSound.play(LOOP, 130);
			UllageTrigger = false;
		}
	}

	//Failure code
	FailureTimer += simdt;

	for (int i = 0;i < 5;i++)
	{
		if (EarlySIICutoff[i] && (FailureTimer > SecondStageFailureTime[i]) && vessel->GetThrusterResource(j2engines[i]) != NULL)
		{
			vessel->SetThrusterResource(j2engines[i], NULL);
		}
	}
}

void SIISystems::SetEngineFailureParameters(bool *SIICut, double *SIICutTimes)
{
	for (int i = 0;i < 5;i++)
	{
		EarlySIICutoff[i] = SIICut[i];
		SecondStageFailureTime[i] = SIICutTimes[i];
	}
}

void SIISystems::SetThrusterGroupLevel(double level)
{
	for (int i = 0;i < 5;i++)
	{
		vessel->SetThrusterLevel(j2engines[i], level);
	}
}

#define MR_STATS 5

static double MixtureRatios[MR_STATS] = { 6.0, 5.5, 5.0, 4.3, 4.0 };
//static double MRISP[MR_STATS] = { 416*G, 418*G, 421*G, 427*G, 432*G };
static double MRISP[MR_STATS] = { 421.4 * G, 423.4 * G, 426.5 * G, 432.6 * G, 437.6 * G };
static double MRThrust[MR_STATS] = { 1.1, 1.0, .898, .7391, .7 };

void SIISystems::GetJ2ISP(double ratio, double &isp, double &ThrustAdjust)

{
	isp = 421 * G;

	// From Usenet:
	// It had roughly three stops. 178,000 lbs at 425s Isp and an O/F of 4.5,
	// 207,000 lbs at 421s Isp and an O/F of 5.0, and 230,500 lbs at 418s Isp
	// and an O/F of 5.5.

	for (int i = 0; i < MR_STATS; i++) {
		if (ratio >= MixtureRatios[i]) {
			double delta = (ratio - MixtureRatios[i]) / (MixtureRatios[i - 1] - MixtureRatios[i]);

			isp = MRISP[i] + ((MRISP[i - 1] - MRISP[i]) * delta);
			ThrustAdjust = MRThrust[i] + ((MRThrust[i - 1] - MRThrust[i]) * delta);

			return;
		}
	}
}

void SIISystems::SetPUValve(int state)
{
	if (state == PUVALVE_CLOSED)
	{
		PUValveState = PUVALVE_CLOSED;
	}
	else if (state == PUVALVE_NULL)
	{
		PUValveState = PUVALVE_NULL;
	}
	else if (state == PUVALVE_OPEN)
	{
		PUValveState = PUVALVE_OPEN;
	}

	RecalculateEngineParameters();
}

void SIISystems::RecalculateEngineParameters(double BaseThrust)
{
	J2DefaultThrust = BaseThrust;

	RecalculateEngineParameters();
}

void SIISystems::RecalculateEngineParameters()
{
	double MixtureRatio;

	if (PUValveState == PUVALVE_CLOSED)
	{
		MixtureRatio = 5.5;
	}
	else if (PUValveState == PUVALVE_OPEN)
	{
		MixtureRatio = 4.5;
	}
	else
	{
		MixtureRatio = 5.0;
	}

	SetMixtureRatio(MixtureRatio);
}

void SIISystems::SetMixtureRatio(double ratio)
{
	double isp, thrust, ThrustAdjust;

	GetJ2ISP(ratio, isp, ThrustAdjust);
	thrust = J2DefaultThrust * ThrustAdjust;

	for (int i = 0; i < 5; i++) {
		vessel->SetThrusterIsp(j2engines[i], isp, isp);
		vessel->SetThrusterMax0(j2engines[i], thrust);
	}
}

void SIISystems::SetThrusterDir(int n, double beta_y, double beta_p)
{
	if (j2engines[0] == NULL) return;
	if (n < 0 || n>3) return;

	VECTOR3 j2vector;

	if (beta_y > 7.0*RAD)
	{
		j2vector.x = 7.0*RAD;
	}
	else if (beta_y < -7.0*RAD)
	{
		j2vector.x = -7.0*RAD;
	}
	else
	{
		j2vector.x = beta_y;
	}

	if (beta_p > 7.0*RAD)
	{
		j2vector.y = 7.0*RAD;
	}
	else if (beta_p < -7.0*RAD)
	{
		j2vector.y = -7.0*RAD;
	}
	else
	{
		j2vector.y = beta_p;
	}

	j2vector.z = 1.0;

	vessel->SetThrusterDir(j2engines[n], j2vector);
}

bool SIISystems::PropellantLowLevel()
{
	if (PointLevelSensorArmed)
	{
		if (main_propellant)
		{
			if (vessel->GetPropellantMass(main_propellant) < 250.0)
			{
				return true;
			}
		}
	}

	return false;
}

void SIISystems::GetThrustOK(bool *ok)
{
	for (int i = 0;i < 5;i++)
	{
		ok[i] = ThrustOK[i];
	}
}

void SIISystems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 3: //S-II LOX Depletion Sensors Cutoff Arm
		SetDepletionSensorsCutoffArm();
		break;
	case 5: //S-II/S-IVB Separation
		if (SIISIVBOrdnanceArmed && vessel->GetStage() < LAUNCH_STAGE_SIVB)
		{
			puShiftSound.done(); // Make sure it's done
			vessel->SeparateStage(LAUNCH_STAGE_SIVB);
			vessel->SetStage(LAUNCH_STAGE_SIVB);
			vessel->AddRCS_S4B();
		}
		break;
	case 8: //S-II/S-IVB Ordnance Arm
		SetSIISIVBOrdnanceArm();
		break;
	case 9: //Stop First PAM - FM/FM Calibration
		break;
	case 11: //S-II Ordnance Arm
		SetOrdnanceArm();
		break;
	case 18: //S-II Engines Cutoff
		LVDCEnginesCutoff();
		break;
	case 20: //Engines Ready Bypass
		SetEnginesReadyBypass();
		break;
	case 23: //S-II Aft Interstage Separation
		if (OrdnanceArmed && vessel->GetStage() == LAUNCH_STAGE_TWO)
		{
			vessel->SeparateStage(LAUNCH_STAGE_TWO_ISTG_JET);
			vessel->SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
		}
		break;
	case 24: //S-II Ullage Trigger
		FireUllageTrigger();
		break;
	case 30: //Start First PAM - FM/FM Relays Reset
		break;
	case 31: //S-II Engines Cutoff Reset
		LVDCEnginesCutoffReset();
		break;
	case 33: //S-II Engine Start
		EngineStartOn();
		break;
	case 38: //LH2 Tank High Pressure Vent Mode
		break;
	case 42: //S-II LH2 Depletion Sensors Cutoff Arm
		SetDepletionSensorsCutoffArm();
		break;
	case 49: //Engines Ready Bypass Reset
		ResetEnginesReadyBypass();
		break;
	case 56: //Low (4.5) Engine MR Ratio On
		SetPUValve(PUVALVE_OPEN);
		puShiftSound.play(NOLOOP, 255);
		puShiftSound.done();
		break;
	case 58: //High (5.5) Engine MR Ratio Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 59: //High (5.5 Engine Mixture Ratio On
		SetPUValve(PUVALVE_CLOSED);
		break;
	case 71: //Start Data Recorders
		break;
	default:
		break;
	}
}