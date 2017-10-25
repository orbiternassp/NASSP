/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Emergency Detection System

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

#include "soundlib.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "EDS.h"

EDS::EDS(LVRG &rg) : lvrg(rg)
{
	AutoAbortInitiate = false;
	TwoEngOutAutoAbortDeactivate = false;
	ExcessRatesAutoAbortDeactivatePY = false;
	ExcessRatesAutoAbortDeactivateR = false;
	TwoEngOutAutoAbortInhibit = false;
	ExcessiveRatesAutoAbortInhibit = false;
	LVEnginesCutoffEnable = false;
	RateGyroSCIndicationSwitchA = false;
	RateGyroSCIndicationSwitchB = false;
	SIEngineOutIndicationA = false;
	SIEngineOutIndicationB = false;
	SIIEngineOutIndicationA = false;
	SIIEngineOutIndicationB = false;
	SIVBEngineOutIndicationA = false;
	SIVBEngineOutIndicationB = false;
	SI_Engine_Out = false;
	SII_Engine_Out = false;
	AutoAbortEnableRelayA = false;
	AutoAbortEnableRelayB = false;
	LiftoffA = false;
	LiftoffB = false;

	PlatformFailure = false;
	PlatformFailureTime = 0.0;

	iu = NULL;
}

void EDS::Init(IU *i)
{
	iu = i;
}

void EDS::SetPlatformFailureParameters(bool PlatFail, double PlatFailTime)
{
	PlatformFailure = PlatFail;
	PlatformFailureTime = PlatFailTime;
}

void EDS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOABORTINITIATE", AutoAbortInitiate);
	papiWriteScenario_bool(scn, "AUTOABORTENABLERELAYA", AutoAbortEnableRelayA);
	papiWriteScenario_bool(scn, "AUTOABORTENABLERELAYB", AutoAbortEnableRelayB);
	papiWriteScenario_bool(scn, "LIFTOFFA", LiftoffA);
	papiWriteScenario_bool(scn, "LIFTOFFB", LiftoffB);
	papiWriteScenario_bool(scn, "TWOENGOUTAUTOABORTDEACTIVATE", TwoEngOutAutoAbortDeactivate);
	papiWriteScenario_bool(scn, "EXCESSRATESAUTOABORTDEACTIVATEPY", ExcessRatesAutoAbortDeactivatePY);
	papiWriteScenario_bool(scn, "EXCESSRATESAUTOABORTDEACTIVATER", ExcessRatesAutoAbortDeactivateR);
	papiWriteScenario_bool(scn, "TWOENGOUTAUTOABORTINHIBIT", TwoEngOutAutoAbortInhibit);
	papiWriteScenario_bool(scn, "RATEGYROSCINDICATIONSWITCHA", RateGyroSCIndicationSwitchA);
	papiWriteScenario_bool(scn, "RATEGYROSCINDICATIONSWITCHB", RateGyroSCIndicationSwitchB);
	papiWriteScenario_bool(scn, "SIENGINEOUTINDICATIONA", SIEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIENGINEOUTINDICATIONB", SIEngineOutIndicationB);
	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONB", SIVBEngineOutIndicationB);
	papiWriteScenario_bool(scn, "SIENGINEOUT", SI_Engine_Out);
	papiWriteScenario_bool(scn, "SIIENGINEOUT", SII_Engine_Out);

	oapiWriteLine(scn, end_str);
}

void EDS::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "AUTOABORTINITIATE", AutoAbortInitiate);
		papiReadScenario_bool(line, "AUTOABORTENABLERELAYA", AutoAbortEnableRelayA);
		papiReadScenario_bool(line, "AUTOABORTENABLERELAYB", AutoAbortEnableRelayB);
		papiReadScenario_bool(line, "LIFTOFFA", LiftoffA);
		papiReadScenario_bool(line, "LIFTOFFB", LiftoffB);
		papiReadScenario_bool(line, "TWOENGOUTAUTOABORTDEACTIVATE", TwoEngOutAutoAbortDeactivate);
		papiReadScenario_bool(line, "EXCESSRATESAUTOABORTDEACTIVATEPY", ExcessRatesAutoAbortDeactivatePY);
		papiReadScenario_bool(line, "EXCESSRATESAUTOABORTDEACTIVATER", ExcessRatesAutoAbortDeactivateR);
		papiReadScenario_bool(line, "TWOENGOUTAUTOABORTINHIBIT", TwoEngOutAutoAbortInhibit);
		papiReadScenario_bool(line, "EXCESSIVERATESAUTOABORTINHIBIT", ExcessiveRatesAutoAbortInhibit);
		papiReadScenario_bool(line, "RATEGYROSCINDICATIONSWITCHA", RateGyroSCIndicationSwitchA);
		papiReadScenario_bool(line, "RATEGYROSCINDICATIONSWITCHB", RateGyroSCIndicationSwitchB);
		papiReadScenario_bool(line, "SIENGINEOUTINDICATIONA", SIEngineOutIndicationA);
		papiReadScenario_bool(line, "SIENGINEOUTINDICATIONB", SIEngineOutIndicationB);
		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
		papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
		papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONB", SIEngineOutIndicationB);
		papiReadScenario_bool(line, "SIENGINEOUT", SI_Engine_Out);
		papiReadScenario_bool(line, "SIIENGINEOUT", SII_Engine_Out);

	}
}

EDS1B::EDS1B(LVRG &rg) : EDS(rg)
{
	for (int i = 0;i < 8;i++)
	{
		EarlySICutoff[i] = false;
		FirstStageFailureTime[i] = 0.0;
		ThrustOK[i] = false;
	}
}

bool EDS1B::ThrustCommitEval()
{
	for (int i = 0;i < 8;i++) if (!ThrustOK[i]) return false;

	return true;
}

void EDS1B::SetEngineFailureParameters(bool *SICut, double *SICutTimes, bool *SIICut, double *SIICutTimes)
{
	for (int i = 0;i < 8;i++)
	{
		EarlySICutoff[i] = SICut[i];
		FirstStageFailureTime[i] = SICutTimes[i];
	}
}

void EDS1B::LVIndicatorsOff()
{
	iu->GetCommandConnector()->ClearEngineIndicator(1);
	iu->GetCommandConnector()->ClearEngineIndicator(2);
	iu->GetCommandConnector()->ClearEngineIndicator(3);
	iu->GetCommandConnector()->ClearEngineIndicator(4);
	iu->GetCommandConnector()->ClearEngineIndicator(5);
	iu->GetCommandConnector()->ClearEngineIndicator(6);
	iu->GetCommandConnector()->ClearEngineIndicator(7);
	iu->GetCommandConnector()->ClearEngineIndicator(8);
}

void EDS1B::Timestep(double simdt)
{
	if (iu->GetLVCommandConnector() == NULL) return;
	if (iu->GetCommandConnector() == NULL) return;

	VECTOR3 AttRate;

	AttRate = lvrg.GetRates();

	int EDSSwitch = iu->GetCommandConnector()->EDSSwitchState();
	int LVRateAutoSwitch = iu->GetCommandConnector()->LVRateAutoSwitchState();
	int TwoEngineOutAutoSwitch = iu->GetCommandConnector()->TwoEngineOutAutoSwitchState();
	int Stage = iu->GetLVCommandConnector()->GetStage();

	bool EDSBus1Powered = iu->GetCommandConnector()->IsEDSBusPowered(1);
	bool EDSBus2Powered = iu->GetCommandConnector()->IsEDSBusPowered(2);
	bool EDSBus3Powered = iu->GetCommandConnector()->IsEDSBusPowered(3);

	if (TwoEngOutAutoAbortInhibit || TwoEngineOutAutoSwitch == TOGGLESWITCH_DOWN)
	{
		TwoEngOutAutoAbortDeactivate = true;
	}
	else
	{
		TwoEngOutAutoAbortDeactivate = false;
	}

	if (ExcessiveRatesAutoAbortInhibit || LVRateAutoSwitch == TOGGLESWITCH_DOWN)
	{
		ExcessRatesAutoAbortDeactivatePY = true;
		ExcessRatesAutoAbortDeactivateR = true;
	}
	else
	{
		ExcessRatesAutoAbortDeactivatePY = false;
		ExcessRatesAutoAbortDeactivateR = false;
	}

	bool S1_TwoEngines_Out, RollRateExceeded, PYRateExceeded;

	if (Stage == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(0)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(1)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(2)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(3)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(4)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(5)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(6)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(7)) < 0.65) enginesout++;

		if (enginesout >= 2 && !TwoEngOutAutoAbortDeactivate)
		{
			S1_TwoEngines_Out = true;
		}
		else
		{
			S1_TwoEngines_Out = false;
		}
	}
	else
	{
		S1_TwoEngines_Out = false;
	}

	if ((abs(AttRate.y) > 4.5*RAD || abs(AttRate.z) > 10.0*RAD) && ExcessRatesAutoAbortDeactivatePY)
	{
		PYRateExceeded = true;
	}
	else
	{
		PYRateExceeded = false;
	}

	if (abs(AttRate.x) > 20.5*RAD && ExcessRatesAutoAbortDeactivateR)
	{
		RollRateExceeded = true;
	}
	else
	{
		RollRateExceeded = false;
	}

	if (RollRateExceeded || PYRateExceeded || S1_TwoEngines_Out)
	{
		AutoAbortInitiate = true;
	}
	else
	{
		AutoAbortInitiate = false;
	}

	if (EDSBus1Powered && AutoAbortInitiate)
	{
		iu->GetCommandConnector()->SetEDSAbort(1);
	}
	if (EDSBus2Powered && AutoAbortInitiate)
	{
		iu->GetCommandConnector()->SetEDSAbort(2);
	}
	if (EDSBus3Powered && AutoAbortInitiate)
	{
		iu->GetCommandConnector()->SetEDSAbort(3);
	}

	double PYLimit;

	if (RateGyroSCIndicationSwitchA && RateGyroSCIndicationSwitchB)
	{
		PYLimit = 9.2*RAD;
	}
	else
	{
		PYLimit = 4.0*RAD;
	}

	//LV Rates light
	if (abs(AttRate.y) > PYLimit || abs(AttRate.z) > PYLimit || abs(AttRate.x) > 20.0*RAD)
	{
		iu->GetCommandConnector()->SetLVRateLight();
	}
	else
	{
		iu->GetCommandConnector()->ClearLVRateLight();
	}

	// Update engine indicators and failure flags
	switch (Stage) {
	case PRELAUNCH_STAGE:
	case LAUNCH_STAGE_ONE:
		if ((SIEngineOutIndicationA && EDSBus1Powered) || (SIEngineOutIndicationB && EDSBus3Powered)) {
			int i = 0;
			while (i < 8) {
				ThrustOK[i] = iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(i)) >= 0.65;
				if (ThrustOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(i + 1); }
				if (!ThrustOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == false) { iu->GetCommandConnector()->SetEngineIndicator(i + 1); }
				i++;
			}
		}
		else
		{
			LVIndicatorsOff();
		}
		break;
	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		if ((SIVBEngineOutIndicationA && EDSBus1Powered) || (SIVBEngineOutIndicationB && EDSBus3Powered)) {
			if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(0)) >= 0.65  && iu->GetCommandConnector()->GetEngineIndicator(1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(1); } // UNLIGHT
			if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(0)) < 0.65 && iu->GetCommandConnector()->GetEngineIndicator(1) == false) { iu->GetCommandConnector()->SetEngineIndicator(1); }   // LIGHT
		}
		else
		{
			LVIndicatorsOff();
		}
		break;
	default:
		break;
	}

	if (Stage == PRELAUNCH_STAGE)
	{
		if (!AutoAbortEnableRelayA && !AutoAbortEnableRelayB)
		{
			if (ThrustCommitEval())
			{
				AutoAbortEnableRelayA = true;
				AutoAbortEnableRelayB = true;
			}
		}
	}

	if (EDSBus1Powered && Stage == LAUNCH_STAGE_ONE && AutoAbortEnableRelayA)
	{
		LiftoffA = true;
	}
	else
	{
		LiftoffA = false;
	}

	if (EDSBus3Powered && Stage == LAUNCH_STAGE_ONE && AutoAbortEnableRelayB)
	{
		LiftoffB = true;
	}
	else
	{
		LiftoffB = false;
	}

	//Engine failure code
	switch (Stage)
	{
	case LAUNCH_STAGE_ONE:
		for (int i = 0;i < 8;i++)
		{
			if (EarlySICutoff[i] && (iu->GetLVCommandConnector()->GetMissionTime() > FirstStageFailureTime[i]) && (iu->GetLVCommandConnector()->GetThrusterResource(iu->GetLVCommandConnector()->GetMainThruster(i)) != NULL))
			{
				iu->GetLVCommandConnector()->SetThrusterResource(iu->GetLVCommandConnector()->GetMainThruster(i), NULL); // Should stop the engine
				SI_Engine_Out = true;
			}
		}
		break;
	default:
		SI_Engine_Out = false;
		break;
	}

	if (PlatformFailure && iu->GetLVCommandConnector()->GetMissionTime() > PlatformFailureTime && !iu->lvimu.IsFailed())
	{
		iu->lvimu.SetFailed();
	}
}

EDSSV::EDSSV(LVRG &rg) : EDS(rg)
{
	for (int i = 0;i < 5;i++)
	{
		EarlySICutoff[i] = false;
		FirstStageFailureTime[i] = 0.0;
		EarlySIICutoff[i] = false;
		SecondStageFailureTime[i] = 0.0;
		ThrustOK[i] = false;
	}
}

bool EDSSV::ThrustCommitEval()
{
	for (int i = 0;i < 5;i++) if (!ThrustOK[i]) return false;

	return true;
}

void EDSSV::SetEngineFailureParameters(bool *SICut, double *SICutTimes, bool *SIICut, double *SIICutTimes)
{
	for (int i = 0;i < 5;i++)
	{
		EarlySICutoff[i] = SICut[i];
		FirstStageFailureTime[i] = SICutTimes[i];
		EarlySIICutoff[i] = SIICut[i];
		SecondStageFailureTime[i] = SIICutTimes[i];
	}
}

void EDSSV::LVIndicatorsOff()
{
	iu->GetCommandConnector()->ClearEngineIndicator(1);
	iu->GetCommandConnector()->ClearEngineIndicator(2);
	iu->GetCommandConnector()->ClearEngineIndicator(3);
	iu->GetCommandConnector()->ClearEngineIndicator(4);
	iu->GetCommandConnector()->ClearEngineIndicator(5);
}

void EDSSV::Timestep(double simdt)
{

	if (iu->GetLVCommandConnector() == NULL) return;
	if (iu->GetCommandConnector() == NULL) return;

	VECTOR3 AttRate;

	AttRate = lvrg.GetRates();

	int EDSSwitch = iu->GetCommandConnector()->EDSSwitchState();
	int LVRateAutoSwitch = iu->GetCommandConnector()->LVRateAutoSwitchState();
	int TwoEngineOutAutoSwitch = iu->GetCommandConnector()->TwoEngineOutAutoSwitchState();
	int Stage = iu->GetLVCommandConnector()->GetStage();

	bool EDSBus1Powered = iu->GetCommandConnector()->IsEDSBusPowered(1);
	bool EDSBus2Powered = iu->GetCommandConnector()->IsEDSBusPowered(2);
	bool EDSBus3Powered = iu->GetCommandConnector()->IsEDSBusPowered(3);

	if (TwoEngOutAutoAbortInhibit || TwoEngineOutAutoSwitch == TOGGLESWITCH_DOWN)
	{
		TwoEngOutAutoAbortDeactivate = true;
	}
	else
	{
		TwoEngOutAutoAbortDeactivate = false;
	}

	if (ExcessiveRatesAutoAbortInhibit || LVRateAutoSwitch == TOGGLESWITCH_DOWN)
	{
		ExcessRatesAutoAbortDeactivatePY = true;
		ExcessRatesAutoAbortDeactivateR = true;
	}
	else
	{
		ExcessRatesAutoAbortDeactivatePY = false;
		ExcessRatesAutoAbortDeactivateR = false;
	}

	bool S1_TwoEngines_Out, RollRateExceeded, PYRateExceeded;

	if (Stage == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(0)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(1)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(2)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(3)) < 0.65) enginesout++;
		if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(4)) < 0.65) enginesout++;

		if (enginesout >= 2 && !TwoEngOutAutoAbortDeactivate)
		{
			S1_TwoEngines_Out = true;
		}
		else
		{
			S1_TwoEngines_Out = false;
		}
	}
	else
	{
		S1_TwoEngines_Out = false;
	}

	if ((abs(AttRate.y) > 4.5*RAD || abs(AttRate.z) > 10.0*RAD) && ExcessRatesAutoAbortDeactivatePY)
	{
		PYRateExceeded = true;
	}
	else
	{
		PYRateExceeded = false;
	}

	if (abs(AttRate.x) > 20.5*RAD && ExcessRatesAutoAbortDeactivateR)
	{
		RollRateExceeded = true;
	}
	else
	{
		RollRateExceeded = false;
	}

	if (RollRateExceeded || PYRateExceeded || S1_TwoEngines_Out)
	{
		AutoAbortInitiate = true;
	}
	else
	{
		AutoAbortInitiate = false;
	}

	if (EDSBus1Powered && AutoAbortInitiate)
	{
		iu->GetCommandConnector()->SetEDSAbort(1);
	}
	if (EDSBus2Powered && AutoAbortInitiate)
	{
		iu->GetCommandConnector()->SetEDSAbort(2);
	}
	if (EDSBus3Powered && AutoAbortInitiate)
	{
		iu->GetCommandConnector()->SetEDSAbort(3);
	}

	double PYLimit;

	if (RateGyroSCIndicationSwitchA && RateGyroSCIndicationSwitchB)
	{
		PYLimit = 9.2*RAD;
	}
	else
	{
		PYLimit = 4.0*RAD;
	}

	//LV Rates light
	if (abs(AttRate.y) > PYLimit || abs(AttRate.z) > PYLimit || abs(AttRate.x) > 20.0*RAD)
	{
		iu->GetCommandConnector()->SetLVRateLight();
	}
	else
	{
		iu->GetCommandConnector()->ClearLVRateLight();
	}

	// Update engine indicators and failure flags
	switch (Stage) {
	case PRELAUNCH_STAGE:
	case LAUNCH_STAGE_ONE:
		if ((SIEngineOutIndicationA && EDSBus1Powered) || (SIEngineOutIndicationB && EDSBus3Powered)) {
			int i = 0;
			while (i < 5) {
				ThrustOK[i] = iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(i)) >= 0.90;
				if (ThrustOK[i]  && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(i + 1); }
				if (!ThrustOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == false) { iu->GetCommandConnector()->SetEngineIndicator(i + 1); }
				i++;
			}
		}
		else
		{
			LVIndicatorsOff();
		}
		break;
	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		if ((SIIEngineOutIndicationA && EDSBus1Powered) || (SIIEngineOutIndicationB && EDSBus3Powered)) {
			int i = 0;
			while (i < 5) {
				if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(i)) >= 0.65  && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(i + 1); }
				if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(i)) < 0.65 && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == false) { iu->GetCommandConnector()->SetEngineIndicator(i + 1); }
				i++;
			}
		}
		else
		{
			LVIndicatorsOff();
		}
		break;
		break;
	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		if ((SIVBEngineOutIndicationA && EDSBus1Powered) || (SIVBEngineOutIndicationB && EDSBus3Powered)) {
			if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(0)) >= 0.65  && iu->GetCommandConnector()->GetEngineIndicator(1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(1); } // UNLIGHT
			if (iu->GetLVCommandConnector()->GetThrusterLevel(iu->GetLVCommandConnector()->GetMainThruster(0)) < 0.65 && iu->GetCommandConnector()->GetEngineIndicator(1) == false) { iu->GetCommandConnector()->SetEngineIndicator(1); }   // LIGHT
		}
		else
		{
			LVIndicatorsOff();
		}
		break;
	default:
		break;
	}

	if (Stage == PRELAUNCH_STAGE)
	{
		if (!AutoAbortEnableRelayA && !AutoAbortEnableRelayB)
		{
			if (ThrustCommitEval())
			{
				AutoAbortEnableRelayA = true;
				AutoAbortEnableRelayB = true;
			}
		}
	}

	if (EDSBus1Powered && Stage == LAUNCH_STAGE_ONE && AutoAbortEnableRelayA)
	{
		LiftoffA = true;
	}
	else
	{
		LiftoffA = false;
	}

	if (EDSBus3Powered && Stage == LAUNCH_STAGE_ONE && AutoAbortEnableRelayB)
	{
		LiftoffB = true;
	}
	else
	{
		LiftoffB = false;
	}

	//Engine failure code

	switch (Stage) {
	case LAUNCH_STAGE_ONE:
		SII_Engine_Out = false;
		for (int i = 0;i < 5;i++)
		{
			if (EarlySICutoff[i] && (iu->GetLVCommandConnector()->GetMissionTime() > FirstStageFailureTime[i]) && (iu->GetLVCommandConnector()->GetThrusterResource(iu->GetLVCommandConnector()->GetMainThruster(i)) != NULL))
			{
				iu->GetLVCommandConnector()->SetThrusterResource(iu->GetLVCommandConnector()->GetMainThruster(i), NULL); // Should stop the engine
				SI_Engine_Out = true;
			}
		}
		break;
	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SI_Engine_Out = false;
		for (int i = 0;i < 5;i++)
		{
			if (EarlySIICutoff[i] && (iu->GetLVCommandConnector()->GetMissionTime() > SecondStageFailureTime[i]) && (iu->GetLVCommandConnector()->GetThrusterResource(iu->GetLVCommandConnector()->GetMainThruster(i)) != NULL))
			{
				iu->GetLVCommandConnector()->SetThrusterResource(iu->GetLVCommandConnector()->GetMainThruster(i), NULL); // Should stop the engine
				SII_Engine_Out = true;
			}
		}
		break;
	default:
		SI_Engine_Out = false;
		SII_Engine_Out = false;
		break;
	}

	if (PlatformFailure && iu->GetLVCommandConnector()->GetMissionTime() > PlatformFailureTime && !iu->lvimu.IsFailed())
	{
		iu->lvimu.SetFailed();
	}

	//sprintf(oapiDebugString(), "%f", PlatformFailureTime);
}