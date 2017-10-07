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
	lvCommandConnector = NULL;

	AutoAbortInitiate = false;
	TwoEngOutAutoAbortDeactivate = false;
	ExcessRatesAutoAbortDeactivatePY = false;
	ExcessRatesAutoAbortDeactivateR = false;
	TwoEngOutAutoAbortInhibit = false;
	ExcessiveRatesAutoAbortInhibit = false;
	LVEnginesCutoffEnable = false;
	RateGyroSCIndicationSwitchA = false;
	RateGyroSCIndicationSwitchB = false;
	EngineOutIndicationA = false;
	EngineOutIndicationB = false;
	SI_Engine_Out = false;
	SII_Engine_Out = false;
}

void EDS::Configure(IUToLVCommandConnector *lvCommandConn, IUToCSMCommandConnector *commandConn)
{
	lvCommandConnector = lvCommandConn;
	commandConnector = commandConn;
}

void EDS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOABORTINITIATE", AutoAbortInitiate);
	papiWriteScenario_bool(scn, "TWOENGOUTAUTOABORTDEACTIVATE", TwoEngOutAutoAbortDeactivate);
	papiWriteScenario_bool(scn, "EXCESSRATESAUTOABORTDEACTIVATEPY", ExcessRatesAutoAbortDeactivatePY);
	papiWriteScenario_bool(scn, "EXCESSRATESAUTOABORTDEACTIVATER", ExcessRatesAutoAbortDeactivateR);
	papiWriteScenario_bool(scn, "TWOENGOUTAUTOABORTINHIBIT", TwoEngOutAutoAbortInhibit);
	papiWriteScenario_bool(scn, "RATEGYROSCINDICATIONSWITCHA", RateGyroSCIndicationSwitchA);
	papiWriteScenario_bool(scn, "RATEGYROSCINDICATIONSWITCHB", RateGyroSCIndicationSwitchB);
	papiWriteScenario_bool(scn, "ENGINEOUTINDICATIONA", EngineOutIndicationA);
	papiWriteScenario_bool(scn, "ENGINEOUTINDICATIONB", EngineOutIndicationB);
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
		papiReadScenario_bool(line, "TWOENGOUTAUTOABORTDEACTIVATE", TwoEngOutAutoAbortDeactivate);
		papiReadScenario_bool(line, "EXCESSRATESAUTOABORTDEACTIVATEPY", ExcessRatesAutoAbortDeactivatePY);
		papiReadScenario_bool(line, "EXCESSRATESAUTOABORTDEACTIVATER", ExcessRatesAutoAbortDeactivateR);
		papiReadScenario_bool(line, "TWOENGOUTAUTOABORTINHIBIT", TwoEngOutAutoAbortInhibit);
		papiReadScenario_bool(line, "EXCESSIVERATESAUTOABORTINHIBIT", ExcessiveRatesAutoAbortInhibit);
		papiReadScenario_bool(line, "RATEGYROSCINDICATIONSWITCHA", RateGyroSCIndicationSwitchA);
		papiReadScenario_bool(line, "RATEGYROSCINDICATIONSWITCHB", RateGyroSCIndicationSwitchB);
		papiReadScenario_bool(line, "ENGINEOUTINDICATIONA", EngineOutIndicationA);
		papiReadScenario_bool(line, "ENGINEOUTINDICATIONB", EngineOutIndicationB);
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
	}
}

void EDS1B::SetEngineFailureParameters(bool *SICut, double *SICutTimes, bool *SIICut, double *SIICutTimes)
{
	for (int i = 0;i < 8;i++)
	{
		EarlySICutoff[i] = SICut[i];
		FirstStageFailureTime[i] = SICutTimes[i];
	}
}

void EDS1B::Timestep(double simdt)
{

	if (lvCommandConnector == NULL) return;
	if (commandConnector == NULL) return;

	VECTOR3 AttRate;

	AttRate = lvrg.GetRates();

	int EDSSwitch = commandConnector->EDSSwitchState();
	int LVRateAutoSwitch = commandConnector->LVRateAutoSwitchState();
	int TwoEngineOutAutoSwitch = commandConnector->TwoEngineOutAutoSwitchState();

	bool EDSBus1Powered = commandConnector->IsEDSBusPowered(1);
	bool EDSBus2Powered = commandConnector->IsEDSBusPowered(2);
	bool EDSBus3Powered = commandConnector->IsEDSBusPowered(3);


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

	if (lvCommandConnector->GetStage() == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(1)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(2)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(3)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(4)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(5)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(6)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(7)) < 0.65) enginesout++;

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
		commandConnector->SetEDSAbort(1);
	}
	if (EDSBus2Powered && AutoAbortInitiate)
	{
		commandConnector->SetEDSAbort(2);
	}
	if (EDSBus3Powered && AutoAbortInitiate)
	{
		commandConnector->SetEDSAbort(3);
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
		commandConnector->SetLVRateLight();
	}
	else
	{
		commandConnector->ClearLVRateLight();
	}

	// Update engine indicators and failure flags
	if ((EngineOutIndicationA && EDSBus1Powered) || (EngineOutIndicationB && EDSBus3Powered)) {
		int i = 0;
		switch (lvCommandConnector->GetStage()) {
			// 5-engine stages
		case PRELAUNCH_STAGE:
		case LAUNCH_STAGE_ONE:
			while (i<8) {
				if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(i)) >= 0.65  && commandConnector->GetEngineIndicator(i + 1) == true) { commandConnector->ClearEngineIndicator(i + 1); }
				if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(i)) < 0.65 && commandConnector->GetEngineIndicator(i + 1) == false) { commandConnector->SetEngineIndicator(i + 1); }
				i++;
			}
			break;
			// S4B only
		case LAUNCH_STAGE_SIVB:
		case STAGE_ORBIT_SIVB:
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) >= 0.65  && commandConnector->GetEngineIndicator(1) == true) { commandConnector->ClearEngineIndicator(1); } // UNLIGHT
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) < 0.65 && commandConnector->GetEngineIndicator(1) == false) { commandConnector->SetEngineIndicator(1); }   // LIGHT
			break;
			// Error
		default:
			EngineOutIndicationA = false;
			EngineOutIndicationB = false;
			break;
		}
	}
	else {
		commandConnector->SetEngineIndicator(1);
		commandConnector->SetEngineIndicator(2);
		commandConnector->SetEngineIndicator(3);
		commandConnector->SetEngineIndicator(4);
		commandConnector->SetEngineIndicator(5);
		commandConnector->SetEngineIndicator(6);
		commandConnector->SetEngineIndicator(7);
		commandConnector->SetEngineIndicator(8);
	}

	//Engine failure code
	switch (lvCommandConnector->GetStage())
	{
	case LAUNCH_STAGE_ONE:
		for (int i = 0;i < 8;i++)
		{
			if (EarlySICutoff[i] && (lvCommandConnector->GetMissionTime() > FirstStageFailureTime[i]) && (lvCommandConnector->GetThrusterResource(lvCommandConnector->GetMainThruster(i)) != NULL))
			{
				lvCommandConnector->SetThrusterResource(lvCommandConnector->GetMainThruster(i), NULL); // Should stop the engine
				SI_Engine_Out = true;
			}
		}
		break;
	default:
		SI_Engine_Out = false;
		break;
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
	}
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

void EDSSV::Timestep(double simdt)
{

	if (lvCommandConnector == NULL) return;
	if (lvCommandConnector->connectedTo == NULL) return;

	VECTOR3 AttRate;

	AttRate = lvrg.GetRates();

	int EDSSwitch = commandConnector->EDSSwitchState();
	int LVRateAutoSwitch = commandConnector->LVRateAutoSwitchState();
	int TwoEngineOutAutoSwitch = commandConnector->TwoEngineOutAutoSwitchState();

	bool EDSBus1Powered = commandConnector->IsEDSBusPowered(1);
	bool EDSBus2Powered = commandConnector->IsEDSBusPowered(2);
	bool EDSBus3Powered = commandConnector->IsEDSBusPowered(3);

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

	if (lvCommandConnector->GetStage() == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(1)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(2)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(3)) < 0.65) enginesout++;
		if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(4)) < 0.65) enginesout++;

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
		commandConnector->SetEDSAbort(1);
	}
	if (EDSBus2Powered && AutoAbortInitiate)
	{
		commandConnector->SetEDSAbort(2);
	}
	if (EDSBus3Powered && AutoAbortInitiate)
	{
		commandConnector->SetEDSAbort(3);
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
		commandConnector->SetLVRateLight();
	}
	else
	{
		commandConnector->ClearLVRateLight();
	}

	// Update engine indicators and failure flags
	if ((EngineOutIndicationA && EDSBus1Powered) || (EngineOutIndicationB && EDSBus3Powered)) {
		double level;
		switch (lvCommandConnector->GetStage()) {

			// 5-engine stages
		case PRELAUNCH_STAGE:
		case LAUNCH_STAGE_ONE:
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) >= 0.90  && commandConnector->GetEngineIndicator(4) == true) { commandConnector->ClearEngineIndicator(4); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) < 0.90 && commandConnector->GetEngineIndicator(4) == false) { commandConnector->SetEngineIndicator(4); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(1)) >= 0.90  && commandConnector->GetEngineIndicator(2) == true) { commandConnector->ClearEngineIndicator(2); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(1)) < 0.90 && commandConnector->GetEngineIndicator(2) == false) { commandConnector->SetEngineIndicator(2); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(2)) >= 0.90  && commandConnector->GetEngineIndicator(1) == true) { commandConnector->ClearEngineIndicator(1); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(2)) < 0.90 && commandConnector->GetEngineIndicator(1) == false) { commandConnector->SetEngineIndicator(1); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(3)) >= 0.90  && commandConnector->GetEngineIndicator(3) == true) { commandConnector->ClearEngineIndicator(3); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(3)) < 0.90 && commandConnector->GetEngineIndicator(3) == false) { commandConnector->SetEngineIndicator(3); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(4)) >= 0.90  && commandConnector->GetEngineIndicator(5) == true) { commandConnector->ClearEngineIndicator(5); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(4)) < 0.90 && commandConnector->GetEngineIndicator(5) == false) { commandConnector->SetEngineIndicator(5); }
			break;
		case LAUNCH_STAGE_TWO:
		case LAUNCH_STAGE_TWO_ISTG_JET:
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) >= 0.65  && commandConnector->GetEngineIndicator(2) == true) { commandConnector->ClearEngineIndicator(2); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0)) < 0.65 && commandConnector->GetEngineIndicator(2) == false) { commandConnector->SetEngineIndicator(2); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(1)) >= 0.65  && commandConnector->GetEngineIndicator(4) == true) { commandConnector->ClearEngineIndicator(4); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(1)) < 0.65 && commandConnector->GetEngineIndicator(4) == false) { commandConnector->SetEngineIndicator(4); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(2)) >= 0.65  && commandConnector->GetEngineIndicator(1) == true) { commandConnector->ClearEngineIndicator(1); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(2)) < 0.65 && commandConnector->GetEngineIndicator(1) == false) { commandConnector->SetEngineIndicator(1); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(3)) >= 0.65  && commandConnector->GetEngineIndicator(3) == true) { commandConnector->ClearEngineIndicator(3); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(3)) < 0.65 && commandConnector->GetEngineIndicator(3) == false) { commandConnector->SetEngineIndicator(3); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(4)) >= 0.65  && commandConnector->GetEngineIndicator(5) == true) { commandConnector->ClearEngineIndicator(5); }
			if (lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(4)) < 0.65 && commandConnector->GetEngineIndicator(5) == false) { commandConnector->SetEngineIndicator(5); }
			break;
			// S4B only
		case LAUNCH_STAGE_SIVB:
		case STAGE_ORBIT_SIVB:
			level = lvCommandConnector->GetThrusterLevel(lvCommandConnector->GetMainThruster(0));
			if (level >= 0.65  && commandConnector->GetEngineIndicator(1) == true) { commandConnector->ClearEngineIndicator(1); } // UNLIGHT
			if (level < 0.65 && commandConnector->GetEngineIndicator(1) == false) { commandConnector->SetEngineIndicator(1); }  // LIGHT
			break;
			// Error
		default:
			EngineOutIndicationA = false;
			EngineOutIndicationB = false;
			break;
		}
	}
	else {
		commandConnector->ClearEngineIndicator(1);
		commandConnector->ClearEngineIndicator(2);
		commandConnector->ClearEngineIndicator(3);
		commandConnector->ClearEngineIndicator(4);
		commandConnector->ClearEngineIndicator(5);
	}

	//Engine failure code

	switch (lvCommandConnector->GetStage()) {
	case LAUNCH_STAGE_ONE:
		SII_Engine_Out = false;
		for (int i = 0;i < 5;i++)
		{
			if (EarlySICutoff[i] && (lvCommandConnector->GetMissionTime() > FirstStageFailureTime[i]) && (lvCommandConnector->GetThrusterResource(lvCommandConnector->GetMainThruster(i)) != NULL))
			{
				lvCommandConnector->SetThrusterResource(lvCommandConnector->GetMainThruster(i), NULL); // Should stop the engine
				commandConnector->ClearLiftoffLight();
				SI_Engine_Out = true;
			}
		}
		break;
	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SI_Engine_Out = false;
		for (int i = 0;i < 5;i++)
		{
			if (EarlySIICutoff[i] && (lvCommandConnector->GetMissionTime() > SecondStageFailureTime[i]) && (lvCommandConnector->GetThrusterResource(lvCommandConnector->GetMainThruster(i)) != NULL))
			{
				lvCommandConnector->SetThrusterResource(lvCommandConnector->GetMainThruster(i), NULL); // Should stop the engine
				SII_Engine_Out = true;
			}
		}
		break;
	default:
		SI_Engine_Out = false;
		SII_Engine_Out = false;
		break;
	}
}