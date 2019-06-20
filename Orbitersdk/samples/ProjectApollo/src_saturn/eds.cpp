/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Emergency Detection System 602A5

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
#include "iu.h"

#include "soundlib.h"

#include "saturn.h"
#include "papi.h"

#include "EDS.h"

EDS::EDS(IU *iu)
{
	this->iu = iu;

	TwoEngOutAutoAbortDeactivate = false;
	ExcessRatesAutoAbortDeactivatePY = false;
	ExcessRatesAutoAbortDeactivateR = false;
	LVEnginesCutoffEnable = false;
	SIIEngineOutIndicationA = false;
	SIIEngineOutIndicationB = false;
	SIVBEngineOutIndicationA = false;
	SIVBEngineOutIndicationB = false;
	AutoAbortEnableRelayA = false;
	AutoAbortEnableRelayB = false;
	AutoAbortInhibitRelayA = false;
	AutoAbortInhibitRelayB = false;
	LiftoffA = false;
	LiftoffB = false;
	LVEnginesCutoff1 = false;
	LVEnginesCutoff2 = false;
	LVEnginesCutoff3 = false;
	SecondPlaneSeparationMonitor = false;
	SIVBEngineCutoffDisabled = false;
	SIEDSCutoff = false;
	SIIEDSCutoff = false;
	SIVBEDSCutoff = false;
	EDSAbortSignal1 = false;
	EDSAbortSignal2 = false;
	EDSAbortSignal3 = false;
	SIVBRestartAlert = false;
	IULiftoffRelay = false;
	SCControlEnableRelay = false;
	LVAttRefFail = false;
	AttRefFailMonitor = false;
	IUCommandSystemEnable = false;

	AutoAbortBus = false;
	IUEDSBusPowered = false;

	PlatformFailure = false;
	PlatformFailureTime = 0.0;
}

void EDS::SetPlatformFailureParameters(bool PlatFail, double PlatFailTime)
{
	PlatformFailure = PlatFail;
	PlatformFailureTime = PlatFailTime;
}

bool EDS::GetEDSAbort(int n)
{
	if (n == 1)
	{
		return EDSAbortSignal1;
	}
	else if (n == 2)
	{
		return EDSAbortSignal2;
	}
	else if (n == 3)
	{
		return EDSAbortSignal3;
	}

	return false;
}

bool EDS::GetSCControl()
{
	return (SCControlEnableRelay && iu->GetCommandConnector()->GetCMCSIVBTakeover());
}

void EDS::ResetBus1()
{
	LVEnginesCutoffEnable = false;
	SIIEngineOutIndicationA = false;
	SIVBRestartAlert = false;
	SIVBEngineOutIndicationB = false;
	IUCommandSystemEnable = false;
}
void EDS::ResetBus2()
{
	SIIEngineOutIndicationB = false;
	SIVBEngineOutIndicationA = false;
	SCControlEnableRelay = false;
}

bool EDS::LVEnginesCutoffVote()
{
	if (!LVEnginesCutoff1 && !LVEnginesCutoff2) return true;
	if (!LVEnginesCutoff1 && !LVEnginesCutoff3) return true;
	if (!LVEnginesCutoff2 && !LVEnginesCutoff3) return true;

	return false;
}

void EDS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOABORTBUS", AutoAbortBus);
	papiWriteScenario_bool(scn, "AUTOABORTENABLERELAYA", AutoAbortEnableRelayA);
	papiWriteScenario_bool(scn, "AUTOABORTENABLERELAYB", AutoAbortEnableRelayB);
	papiWriteScenario_bool(scn, "AUTOABORTINHIBITRELAYA", AutoAbortInhibitRelayA);
	papiWriteScenario_bool(scn, "AUTOABORTINHIBITRELAYB", AutoAbortInhibitRelayB);
	papiWriteScenario_bool(scn, "LIFTOFFA", LiftoffA);
	papiWriteScenario_bool(scn, "LIFTOFFB", LiftoffB);
	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONB", SIVBEngineOutIndicationB);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFFENABLE", LVEnginesCutoffEnable);
	papiWriteScenario_bool(scn, "SECONDPLANESEPARATIONMONITOR", SecondPlaneSeparationMonitor);
	papiWriteScenario_bool(scn, "SIEDSCUTOFF", SIEDSCutoff);
	papiWriteScenario_bool(scn, "SIIEDSCUTOFF", SIIEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBEDSCUTOFF", SIVBEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL1", EDSAbortSignal1);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL2", EDSAbortSignal2);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL3", EDSAbortSignal3);
	papiWriteScenario_bool(scn, "SIVBRESTARTALERT", SIVBRestartAlert);
	papiWriteScenario_bool(scn, "IULIFTOFFRELAY", IULiftoffRelay);
	papiWriteScenario_bool(scn, "SCCONTROLENABLERELAY", SCControlEnableRelay);
	papiWriteScenario_bool(scn, "ATTREFFAILMONITOR", AttRefFailMonitor);
	papiWriteScenario_bool(scn, "IUEDSBUSPOWERED", IUEDSBusPowered);
	papiWriteScenario_bool(scn, "IUCOMMANDSYSTEMENABLE", IUCommandSystemEnable);

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
		papiReadScenario_bool(line, "AUTOABORTBUS", AutoAbortBus);
		papiReadScenario_bool(line, "AUTOABORTENABLERELAYA", AutoAbortEnableRelayA);
		papiReadScenario_bool(line, "AUTOABORTENABLERELAYB", AutoAbortEnableRelayB);
		papiReadScenario_bool(line, "LIFTOFFA", LiftoffA);
		papiReadScenario_bool(line, "LIFTOFFB", LiftoffB);
		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
		papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
		papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONB", SIVBEngineOutIndicationB);
		papiReadScenario_bool(line, "LVENGINESCUTOFFENABLE", LVEnginesCutoffEnable);
		papiReadScenario_bool(line, "SECONDPLANESEPARATIONMONITOR", SecondPlaneSeparationMonitor);
		papiReadScenario_bool(line, "SIEDSCUTOFF", SIEDSCutoff);
		papiReadScenario_bool(line, "SIIEDSCUTOFF", SIIEDSCutoff);
		papiReadScenario_bool(line, "SIVBEDSCUTOFF", SIVBEDSCutoff);
		papiReadScenario_bool(line, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);
		papiReadScenario_bool(line, "EDSABORTSIGNAL1", EDSAbortSignal1);
		papiReadScenario_bool(line, "EDSABORTSIGNAL2", EDSAbortSignal2);
		papiReadScenario_bool(line, "EDSABORTSIGNAL3", EDSAbortSignal3);
		papiReadScenario_bool(line, "SIVBRESTARTALERT", SIVBRestartAlert);
		papiReadScenario_bool(line, "IULIFTOFFRELAY", IULiftoffRelay);
		papiReadScenario_bool(line, "SCCONTROLENABLERELAY", SCControlEnableRelay);
		papiReadScenario_bool(line, "ATTREFFAILMONITOR", AttRefFailMonitor);
		papiReadScenario_bool(line, "IUEDSBUSPOWERED", IUEDSBusPowered);
		papiReadScenario_bool(line, "IUCOMMANDSYSTEMENABLE", IUCommandSystemEnable);
	}
}

EDS1B::EDS1B(IU *iu) : EDS(iu)
{
	for (int i = 0;i < 8;i++)
	{
		SIThrustNotOK[i] = false;
		ThrustOKSignal[i] = false;
	}
}

bool EDS1B::ThrustCommitEval()
{
	for (int i = 0;i < 8;i++) if (SIThrustNotOK[i]) return false;

	return true;
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
	bool logic;

	AttRate = iu->GetLVRG()->GetRates();

	int LVRateAutoSwitch = iu->GetCommandConnector()->LVRateAutoSwitchState();
	int TwoEngineOutAutoSwitch = iu->GetCommandConnector()->TwoEngineOutAutoSwitchState();
	int Stage = iu->GetLVCommandConnector()->GetStage();

	bool EDSBus1Powered = iu->GetCommandConnector()->IsEDSBusPowered(1);
	bool EDSBus2Powered = iu->GetCommandConnector()->IsEDSBusPowered(2);
	bool EDSBus3Powered = iu->GetCommandConnector()->IsEDSBusPowered(3);

	bool BECOA = iu->GetCommandConnector()->GetBECOCommand(true);
	bool BECOB = iu->GetCommandConnector()->GetBECOCommand(false);

	if (iu->IsUmbilicalConnected())
	{
		IULiftoffRelay = true;
		AutoAbortInhibitRelayA = true;
		AutoAbortInhibitRelayB = true;
	}
	else
	{
		IULiftoffRelay = false;
		IUEDSBusPowered = true;
		AutoAbortInhibitRelayA = false;
		AutoAbortInhibitRelayB = false;
	}

	AutoAbortBus = false;

	if (IUEDSBusPowered && Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(ThrustOKSignal);
		for (int i = 0;i < 8;i++)
		{
			SIThrustNotOK[i] = !ThrustOKSignal[i];
		}
	}
	else
	{
		for (int i = 0;i < 8;i++)
		{
			SIThrustNotOK[i] = false;
		}
	}

	if (iu->GetControlDistributor()->GetTwoEnginesOutAutoAbortInhibit() || (IUEDSBusPowered && TwoEngineOutAutoSwitch == TOGGLESWITCH_DOWN))
		TwoEngOutAutoAbortDeactivate = true;
	else
		TwoEngOutAutoAbortDeactivate = false;

	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitch == TOGGLESWITCH_DOWN))
		ExcessRatesAutoAbortDeactivatePY = true;
	else
		ExcessRatesAutoAbortDeactivatePY = false;

	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitch == TOGGLESWITCH_DOWN))
		ExcessRatesAutoAbortDeactivateR = true;
	else
		ExcessRatesAutoAbortDeactivateR = false;

	if (IUEDSBusPowered && !BECOA)
		LVEnginesCutoff1 = true;
	else
		LVEnginesCutoff1 = false;

	if (IUEDSBusPowered && !BECOA && !BECOB)
		LVEnginesCutoff2 = true;
	else
		LVEnginesCutoff2 = false;

	if (IUEDSBusPowered && !BECOB)
		LVEnginesCutoff3 = true;
	else
		LVEnginesCutoff3 = false;

	//EDS Engine Cutoff
	if (IUEDSBusPowered && LVEnginesCutoffEnable && LVEnginesCutoffVote())
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

	if (Stage == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		for (int i = 0;i < 8;i++)
		{
			if (SIThrustNotOK[i]) enginesout++;
		}

		if (enginesout >= 2 && !TwoEngOutAutoAbortDeactivate) AutoAbortBus = true;
	}

	if ((abs(AttRate.y) > 4.5*RAD || abs(AttRate.z) > 10.0*RAD) && !ExcessRatesAutoAbortDeactivatePY) AutoAbortBus = true;
	if (abs(AttRate.x) > 20.5*RAD && !ExcessRatesAutoAbortDeactivateR) AutoAbortBus = true;

	if (EDSBus1Powered && !AutoAbortBus)
	{
		EDSAbortSignal1 = true;
	}
	else
	{
		EDSAbortSignal1 = false;
	}
	if (EDSBus2Powered && !AutoAbortBus)
	{
		EDSAbortSignal2 = true;
	}
	else
	{
		EDSAbortSignal2 = false;
	}
	if (EDSBus3Powered && !AutoAbortBus)
	{
		EDSAbortSignal3 = true;
	}
	else
	{
		EDSAbortSignal3 = false;
	}

	double PYLimit;

	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit())
	{
		PYLimit = 9.2*RAD;
	}
	else
	{
		PYLimit = 4.0*RAD;
	}

	//LV Guidance Light
	if (iu->lvda.GetGuidanceReferenceFailure())
		LVAttRefFail = true;
	else
		LVAttRefFail = false;

	if (LVAttRefFail && iu->GetSCControlPoweredFlight())
	{
		SCControlEnableRelay = true;
	}

	logic = LVAttRefFail && (EDSBus1Powered || EDSBus3Powered);

	if (logic && !AttRefFailMonitor)
	{
		iu->GetCommandConnector()->SetLVGuidLight();
		AttRefFailMonitor = true;
	}
	else if (!logic && AttRefFailMonitor)
	{
		iu->GetCommandConnector()->ClearLVGuidLight();
		AttRefFailMonitor = false;
	}

	//LV Rates Light
	logic = ((!ExcessRatesAutoAbortDeactivatePY && LVAttRefFail) || abs(AttRate.y) > PYLimit || abs(AttRate.z) > PYLimit || abs(AttRate.x) > 20.0*RAD) && (EDSBus1Powered || EDSBus3Powered);

	if (logic)
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
		if (EDSBus1Powered || EDSBus3Powered) {
			int i = 0;
			while (i < 8) {
				if (!SIThrustNotOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(i + 1); }
				if (SIThrustNotOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i + 1) == false) { iu->GetCommandConnector()->SetEngineIndicator(i + 1); }
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

	if (EDSBus1Powered && !AutoAbortInhibitRelayA && AutoAbortEnableRelayA)
	{
		LiftoffA = true;
	}
	else
	{
		LiftoffA = false;
	}

	if (EDSBus3Powered && !AutoAbortInhibitRelayB && AutoAbortEnableRelayB)
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

EDSSV::EDSSV(IU *iu) : EDS(iu)
{
	for (int i = 0;i < 5;i++)
	{
		SIThrustNotOK[i] = false;
		SIIThrustNotOK[i] = false;
		ThrustOKSignal[i] = false;
	}
	SIISIVBNotSeparated = false;
}

bool EDSSV::ThrustCommitEval()
{
	for (int i = 0;i < 5;i++) if (SIThrustNotOK[i]) return false;

	return true;
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
	bool logic;

	AttRate = iu->GetLVRG()->GetRates();

	int LVRateAutoSwitch = iu->GetCommandConnector()->LVRateAutoSwitchState();
	int TwoEngineOutAutoSwitch = iu->GetCommandConnector()->TwoEngineOutAutoSwitchState();
	int Stage = iu->GetLVCommandConnector()->GetStage();

	bool EDSBus1Powered = iu->GetCommandConnector()->IsEDSBusPowered(1);
	bool EDSBus2Powered = iu->GetCommandConnector()->IsEDSBusPowered(2);
	bool EDSBus3Powered = iu->GetCommandConnector()->IsEDSBusPowered(3);
	bool EDSPowered = EDSBus1Powered || EDSBus2Powered || EDSBus3Powered;

	bool BECOA = iu->GetCommandConnector()->GetBECOCommand(true);
	bool BECOB = iu->GetCommandConnector()->GetBECOCommand(false);

	if (iu->IsUmbilicalConnected())
	{
		AutoAbortInhibitRelayA = true;
		AutoAbortInhibitRelayB = true;
	}
	else
	{
		IUEDSBusPowered = true;
		AutoAbortInhibitRelayA = false;
		AutoAbortInhibitRelayB = false;
	}

	if (Stage < LAUNCH_STAGE_SIVB)
		SIISIVBNotSeparated = true;
	else
		SIISIVBNotSeparated = false;

	AutoAbortBus = false;

	if (IUEDSBusPowered && Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(ThrustOKSignal);
		for (int i = 0;i < 5;i++)
		{
			SIThrustNotOK[i] = !ThrustOKSignal[i];
		}
	}
	else
	{
		for (int i = 0;i < 5;i++)
		{
			SIThrustNotOK[i] = false;
		}
	}

	if (IUEDSBusPowered && (Stage == LAUNCH_STAGE_TWO || Stage == LAUNCH_STAGE_TWO_ISTG_JET))
	{
		iu->GetLVCommandConnector()->GetSIIThrustOK(ThrustOKSignal);
		for (int i = 0;i < 5;i++)
		{
			SIIThrustNotOK[i] = !ThrustOKSignal[i];
		}
	}
	else
	{
		for (int i = 0;i < 5;i++)
		{
			SIIThrustNotOK[i] = false;
		}
	}

	if (iu->GetControlDistributor()->GetTwoEnginesOutAutoAbortInhibit() || (IUEDSBusPowered && TwoEngineOutAutoSwitch == TOGGLESWITCH_DOWN))
		TwoEngOutAutoAbortDeactivate = true;
	else
		TwoEngOutAutoAbortDeactivate = false;

	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitch == TOGGLESWITCH_DOWN))
		ExcessRatesAutoAbortDeactivatePY = true;
	else
		ExcessRatesAutoAbortDeactivatePY = false;

	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitch == TOGGLESWITCH_DOWN))
		ExcessRatesAutoAbortDeactivateR = true;
	else
		ExcessRatesAutoAbortDeactivateR = false;

	if (IUEDSBusPowered && !BECOA)
		LVEnginesCutoff1 = true;
	else
		LVEnginesCutoff1 = false;

	if (IUEDSBusPowered && !BECOA && !BECOB)
		LVEnginesCutoff2 = true;
	else
		LVEnginesCutoff2 = false;

	if (IUEDSBusPowered && !BECOB)
		LVEnginesCutoff3 = true;
	else
		LVEnginesCutoff3 = false;

	//EDS Engine Cutoff
	if (IUEDSBusPowered && LVEnginesCutoffEnable && LVEnginesCutoffVote())
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

	if (IUEDSBusPowered && Stage == LAUNCH_STAGE_ONE)
	{
		int enginesout = 0;

		for (int i = 0;i < 5;i++)
		{
			if (SIThrustNotOK[i]) enginesout++;
		}

		if (enginesout >= 2 && !TwoEngOutAutoAbortDeactivate) AutoAbortBus = true;
	}

	if ((abs(AttRate.y) > 4.5*RAD || abs(AttRate.z) > 10.0*RAD) && !ExcessRatesAutoAbortDeactivatePY) AutoAbortBus = true;

	if (abs(AttRate.x) > 20.5*RAD && !ExcessRatesAutoAbortDeactivateR) AutoAbortBus = true;

	if (EDSBus1Powered && !AutoAbortBus)
	{
		EDSAbortSignal1 = true;
	}
	else
	{
		EDSAbortSignal1 = false;
	}
	if (EDSBus2Powered && !AutoAbortBus)
	{
		EDSAbortSignal2 = true;
	}
	else
	{
		EDSAbortSignal2 = false;
	}
	if (EDSBus3Powered && !AutoAbortBus)
	{
		EDSAbortSignal3 = true;
	}
	else
	{
		EDSAbortSignal3 = false;
	}

	double PYLimit;

	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit())
	{
		PYLimit = 9.2*RAD;
	}
	else
	{
		PYLimit = 4.0*RAD;
	}

	//LV Guidance Light
	if (iu->lvda.GetGuidanceReferenceFailure())
		LVAttRefFail = true;
	else
		LVAttRefFail = false;

	if (LVAttRefFail && iu->GetSCControlPoweredFlight())
	{
		SCControlEnableRelay = true;
	}

	logic = LVAttRefFail && (EDSBus1Powered || EDSBus3Powered);

	if (logic && !AttRefFailMonitor)
	{
		iu->GetCommandConnector()->SetLVGuidLight();
		AttRefFailMonitor = true;
	}
	else if (!logic && AttRefFailMonitor)
	{
		iu->GetCommandConnector()->ClearLVGuidLight();
		AttRefFailMonitor = false;
	}

	//LV Rates Light
	logic = ((!ExcessRatesAutoAbortDeactivatePY && LVAttRefFail) || abs(AttRate.y) > PYLimit || abs(AttRate.z) > PYLimit || abs(AttRate.x) > 20.0*RAD) && (EDSBus1Powered || EDSBus3Powered);

	if (logic)
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
		if (EDSBus1Powered && EDSBus3Powered) {
			int i = 0;
			while (i < 5) {
				if (!SIThrustNotOK[i]  && iu->GetCommandConnector()->GetEngineIndicator(i+1) == true) { iu->GetCommandConnector()->ClearEngineIndicator(i+1); }
				if (SIThrustNotOK[i] && iu->GetCommandConnector()->GetEngineIndicator(i+1) == false) { iu->GetCommandConnector()->SetEngineIndicator(i+1); }
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
				if (!SIIThrustNotOK[i]  && iu->GetCommandConnector()->GetEngineIndicator(SIIEngInd[i]) == true) { iu->GetCommandConnector()->ClearEngineIndicator(SIIEngInd[i]); }
				if (SIIThrustNotOK[i] && iu->GetCommandConnector()->GetEngineIndicator(SIIEngInd[i]) == false) { iu->GetCommandConnector()->SetEngineIndicator(SIIEngInd[i]); }
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
	bool SIISecPlaneSepA = EDSBus1Powered && SIIEngineOutIndicationA && (SIVBRestartAlert || Stage < LAUNCH_STAGE_TWO_ISTG_JET);
	bool SIISecPlaneSepB = EDSBus3Powered && SIIEngineOutIndicationB && (SIVBRestartAlert || Stage < LAUNCH_STAGE_TWO_ISTG_JET);

	if (!SecondPlaneSeparationMonitor && (SIISecPlaneSepA || SIISecPlaneSepB))
	{
		SecondPlaneSeparationMonitor = true;
		iu->GetCommandConnector()->SetSIISep();
	}
	else if (SecondPlaneSeparationMonitor && !(SIISecPlaneSepA || SIISecPlaneSepB))
	{
		SecondPlaneSeparationMonitor = false;
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

	if (EDSBus1Powered && !AutoAbortInhibitRelayA && AutoAbortEnableRelayA)
	{
		LiftoffA = true;
	}
	else
	{
		LiftoffA = false;
	}

	if (EDSBus3Powered && !AutoAbortInhibitRelayB && AutoAbortEnableRelayB)
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