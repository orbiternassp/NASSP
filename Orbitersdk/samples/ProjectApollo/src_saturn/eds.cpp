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
	AutoAbortEnableRelayA = false;
	AutoAbortEnableRelayB = false;
	LiftoffA = false;
	LiftoffB = false;
	LVEnginesCutoff1 = false;
	LVEnginesCutoff2 = false;
	LVEnginesCutoff3 = false;
	SecondPlaneSeparationMonitorRelay = false;
	SIVBEngineCutoffDisabled = false;
	SIEDSCutoff = false;
	SIIEDSCutoff = false;
	SIVBEDSCutoff = false;

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
	papiWriteScenario_bool(scn, "LVENGINESCUTOFFENABLE", LVEnginesCutoffEnable);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFF1", LVEnginesCutoff1);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFF2", LVEnginesCutoff2);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFF3", LVEnginesCutoff3);
	papiWriteScenario_bool(scn, "SECONDPLANESEPARATIONMONITORRELAY", SecondPlaneSeparationMonitorRelay);
	papiWriteScenario_bool(scn, "SIEDSCUTOFF", SIEDSCutoff);
	papiWriteScenario_bool(scn, "SIIEDSCUTOFF", SIIEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBEDSCUTOFF", SIVBEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);

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
		papiReadScenario_bool(line, "LVENGINESCUTOFFENABLE", LVEnginesCutoffEnable);
		papiReadScenario_bool(line, "LVENGINESCUTOFF1", LVEnginesCutoff1);
		papiReadScenario_bool(line, "LVENGINESCUTOFF2", LVEnginesCutoff2);
		papiReadScenario_bool(line, "LVENGINESCUTOFF3", LVEnginesCutoff3);
		papiReadScenario_bool(line, "SECONDPLANESEPARATIONMONITORRELAY", SecondPlaneSeparationMonitorRelay);
		papiReadScenario_bool(line, "SIEDSCUTOFF", SIEDSCutoff);
		papiReadScenario_bool(line, "SIIEDSCUTOFF", SIIEDSCutoff);
		papiReadScenario_bool(line, "SIVBEDSCUTOFF", SIVBEDSCutoff);
		papiReadScenario_bool(line, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);

	}
}

EDS1B::EDS1B(LVRG &rg) : EDS(rg)
{
	for (int i = 0;i < 8;i++)
	{
		EarlySICutoff[i] = false;
		FirstStageFailureTime[i] = 0.0;
		SIThrustOK[i] = false;
	}
}

bool EDS1B::ThrustCommitEval()
{
	for (int i = 0;i < 8;i++) if (!SIThrustOK[i]) return false;

	return true;
}

void EDS1B::SetEngineFailureParameters(bool *SICut, double *SICutTimes)
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

	bool BECOA = iu->GetCommandConnector()->GetBECOCommand(true);
	bool BECOB = iu->GetCommandConnector()->GetBECOCommand(false);

	if (Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(SIThrustOK);
	}

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

	if (LVEnginesCutoffEnable)
	{
		if (BECOA)
		{
			LVEnginesCutoff1 = true;
		}
		else
		{
			LVEnginesCutoff1 = false;
		}

		if (BECOA && BECOB)
		{
			LVEnginesCutoff2 = true;
		}
		else
		{
			LVEnginesCutoff2 = false;
		}

		if (BECOB)
		{
			LVEnginesCutoff3 = true;
		}
		else
		{
			LVEnginesCutoff3 = false;
		}
	}
	else
	{
		LVEnginesCutoff1 = false;
		LVEnginesCutoff2 = false;
		LVEnginesCutoff3 = false;
	}

	//EDS Engine Cutoff
	if ((EDSBus1Powered && LVEnginesCutoff1) || (EDSBus2Powered && LVEnginesCutoff2) || (EDSBus3Powered && LVEnginesCutoff3))
	{
		if (Stage == LAUNCH_STAGE_ONE)
		{
			if (!SIEDSCutoff)
			{
				SIEDSCutoff = true;
				iu->GetLVCommandConnector()->SIEDSCutoff(true);
			}
		}
		else if (Stage == LAUNCH_STAGE_SIVB || Stage == STAGE_ORBIT_SIVB)
		{
			if (!SIVBEngineCutoffDisabled && !SIVBEDSCutoff)
			{
				SIVBEDSCutoff = true;
				iu->GetLVCommandConnector()->SIVBEDSCutoff(true);
			}
		}
	}
	else
	{
		if (SIEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIEDSCutoff(false);
			SIEDSCutoff = false;
		}
		if (SIVBEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIVBEDSCutoff(false);
			SIVBEDSCutoff = false;
		}
	}

	bool S1_TwoEngines_Out, RollRateExceeded, PYRateExceeded;

	if (Stage == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		for (int i = 0;i < 8;i++)
		{
			if (!SIThrustOK[i]) enginesout++;
		}

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

	if ((abs(AttRate.y) > 4.5*RAD || abs(AttRate.z) > 10.0*RAD) && !ExcessRatesAutoAbortDeactivatePY)
	{
		PYRateExceeded = true;
	}
	else
	{
		PYRateExceeded = false;
	}

	if (abs(AttRate.x) > 20.5*RAD && !ExcessRatesAutoAbortDeactivateR)
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
				if (SIThrustOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(i + 1); }
				if (!SIThrustOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == false) { iu->GetCommandConnector()->SetEngineIndicator(i + 1); }
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
			if (iu->GetLVCommandConnector()->GetSIVBThrustOK() && iu->GetCommandConnector()->GetEngineIndicator(1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(1); } // UNLIGHT
			if (!iu->GetLVCommandConnector()->GetSIVBThrustOK() && iu->GetCommandConnector()->GetEngineIndicator(1) == false) { iu->GetCommandConnector()->SetEngineIndicator(1); }   // LIGHT
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

	//Failure code

	if (PlatformFailure && iu->GetLVCommandConnector()->GetMissionTime() > PlatformFailureTime && !iu->lvimu.IsFailed())
	{
		iu->lvimu.SetFailed();
	}
}

EDSSV::EDSSV(LVRG &rg) : EDS(rg)
{
	for (int i = 0;i < 5;i++)
	{
		SIThrustOK[i] = false;
		SIIThrustOK[i] = false;
	}
}

bool EDSSV::ThrustCommitEval()
{
	for (int i = 0;i < 5;i++) if (!SIThrustOK[i]) return false;

	return true;
}

void EDSSV::SetEngineFailureParameters(bool *SICut, double *SICutTimes)
{

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
	bool EDSPowered = EDSBus1Powered || EDSBus2Powered || EDSBus3Powered;

	bool BECOA = iu->GetCommandConnector()->GetBECOCommand(true);
	bool BECOB = iu->GetCommandConnector()->GetBECOCommand(false);

	if (Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(SIThrustOK);
	}
	else if (Stage == LAUNCH_STAGE_TWO || Stage == LAUNCH_STAGE_TWO_ISTG_JET)
	{
		iu->GetLVCommandConnector()->GetSIIThrustOK(SIIThrustOK);
	}

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

	if (LVEnginesCutoffEnable)
	{
		if (BECOA)
		{
			LVEnginesCutoff1 = true;
		}
		else
		{
			LVEnginesCutoff1 = false;
		}

		if (BECOA && BECOB)
		{
			LVEnginesCutoff2 = true;
		}
		else
		{
			LVEnginesCutoff2 = false;
		}

		if (BECOB)
		{
			LVEnginesCutoff3 = true;
		}
		else
		{
			LVEnginesCutoff3 = false;
		}
	}
	else
	{
		LVEnginesCutoff1 = false;
		LVEnginesCutoff2 = false;
		LVEnginesCutoff3 = false;
	}

	//EDS Engine Cutoff
	if ((EDSBus1Powered && LVEnginesCutoff1) || (EDSBus2Powered && LVEnginesCutoff2) || (EDSBus3Powered && LVEnginesCutoff3))
	{
		if (Stage == LAUNCH_STAGE_ONE)
		{
			if (!SIEDSCutoff)
			{
				SIEDSCutoff = true;
				iu->GetLVCommandConnector()->SIEDSCutoff(true);
			}
		}
		else if (Stage == LAUNCH_STAGE_TWO || Stage == LAUNCH_STAGE_TWO_ISTG_JET)
		{
			if (!SIIEDSCutoff)
			{
				SIIEDSCutoff = true;
				iu->GetLVCommandConnector()->SIIEDSCutoff(true);
			}
		}
		else if (Stage == LAUNCH_STAGE_SIVB || Stage == STAGE_ORBIT_SIVB)
		{
			if (!SIVBEngineCutoffDisabled && !SIVBEDSCutoff)
			{
				SIVBEDSCutoff = true;
				iu->GetLVCommandConnector()->SIVBEDSCutoff(true);
			}
		}
	}
	else
	{
		if (SIEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIEDSCutoff(false);
			SIEDSCutoff = false;
		}
		if (SIIEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIIEDSCutoff(false);
			SIIEDSCutoff = false;
		}
		if (SIVBEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIVBEDSCutoff(false);
			SIVBEDSCutoff = false;
		}
	}

	bool S1_TwoEngines_Out, RollRateExceeded, PYRateExceeded;

	if (Stage == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		for (int i = 0;i < 5;i++)
		{
			if (!SIThrustOK[i]) enginesout++;
		}

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

	if ((abs(AttRate.y) > 4.5*RAD || abs(AttRate.z) > 10.0*RAD) && !ExcessRatesAutoAbortDeactivatePY)
	{
		PYRateExceeded = true;
	}
	else
	{
		PYRateExceeded = false;
	}

	if (abs(AttRate.x) > 20.5*RAD && !ExcessRatesAutoAbortDeactivateR)
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
				if (SIThrustOK[i]  && iu->GetCommandConnector()->GetEngineIndicator(i+1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(i+1); }
				if (!SIThrustOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i+1) == false) { iu->GetCommandConnector()->SetEngineIndicator(i+1); }
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
				if (SIIThrustOK[i]  && iu->GetCommandConnector()->GetEngineIndicator(SIIEngInd[i]) == true) { iu->GetCommandConnector()->ClearEngineIndicator(SIIEngInd[i]); }
				if (!SIIThrustOK[i] && iu->GetCommandConnector()->GetEngineIndicator(SIIEngInd[i]) == false) { iu->GetCommandConnector()->SetEngineIndicator(SIIEngInd[i]); }
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
			if (iu->GetLVCommandConnector()->GetSIVBThrustOK() && iu->GetCommandConnector()->GetEngineIndicator(1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(1); } // UNLIGHT
			if (!iu->GetLVCommandConnector()->GetSIVBThrustOK() && iu->GetCommandConnector()->GetEngineIndicator(1) == false) { iu->GetCommandConnector()->SetEngineIndicator(1); }   // LIGHT
		}
		else
		{
			LVIndicatorsOff();
		}
		break;
	default:
		break;
	}

	//Second Plane Separation Monitor
	if (Stage == LAUNCH_STAGE_TWO && !SecondPlaneSeparationMonitorRelay && EDSPowered)
	{
		SecondPlaneSeparationMonitorRelay = true;
		iu->GetCommandConnector()->SetSIISep();
	}
	else if ((Stage != LAUNCH_STAGE_TWO || !EDSPowered) && SecondPlaneSeparationMonitorRelay)
	{
		SecondPlaneSeparationMonitorRelay = false;
		iu->GetCommandConnector()->ClearSIISep();
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

	//Failure code

	if (PlatformFailure && iu->GetLVCommandConnector()->GetMissionTime() > PlatformFailureTime && !iu->lvimu.IsFailed())
	{
		iu->lvimu.SetFailed();
	}

	//sprintf(oapiDebugString(), "%f", PlatformFailureTime);
}