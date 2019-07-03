/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017-2019

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
	LVEnginesCutoffEnable1 = false;
	LVEnginesCutoffEnable2 = false;
	SIIEngineOutIndicationA = false;
	SIIEngineOutIndicationB = false;
	SIVBEngineOutIndicationA = false;
	SIVBEngineOutIndicationB = false;
	EDSLiftoffEnableA = false;
	EDSLiftoffEnableB = false;
	EDSLiftoffInhibitA = false;
	EDSLiftoffInhibitB = false;
	LiftoffA = false;
	LiftoffB = false;
	LVEnginesCutoffFromSC1 = false;
	LVEnginesCutoffFromSC2 = false;
	LVEnginesCutoffFromSC3 = false;
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
	IUCommandSystemEnable = false;
	LVEnginesCutoffCommand1 = false;
	LVEnginesCutoffCommand2 = false;
	LVEnginesCutoffCommand3 = false;
	GSEEngineThrustIndicationEnableA = false;
	GSEEngineThrustIndicationEnableB = false;
	SIVBEngineThrustMonitorA = false;
	SIVBEngineThrustMonitorB = false;
	PadAbortRequest = false;
	RangeSafetyDestructArmedAFromSIVB = false;
	RangeSafetyDestructArmedBFromSIVB = false;
	ExcessiveRollRateIndication = false;
	ExcessivePitchYawRateIndication = false;
	SIAllEnginesOKA = false;
	SIAllEnginesOKB = false;

	AutoAbortBus = false;
	IUEDSBusPowered = true;

	AbortLightSignal = false;

	PlatformFailure = false;
	PlatformFailureTime = 0.0;
	LiftoffCircuitAFailure = false;
	LiftoffCircuitBFailure = false;

	LVRateAutoSwitch = 0;
	TwoEngineOutAutoSwitch = 0;
	Stage = 0;
	AttRate = _V(0, 0, 0);
	EDSBus1Powered = false;
	EDSBus2Powered = false;
	EDSBus3Powered = false;
	BECOA = false;
	BECOB = false;
}

void EDS::Timestep(double simdt)
{
	if (iu->GetAuxPowrDistr()->IsIUEDSBusPowered())
		IUEDSBusPowered = true;
	else
		IUEDSBusPowered = false;

	if (iu->ESEGetCommandVehicleLiftoffIndicationInhibit())
		IULiftoffRelay = true;
	else
		IULiftoffRelay = false;

	if (iu->ESEPadAbortRequest())
		PadAbortRequest = true;
	else
		PadAbortRequest = false;

	if (iu->ESEGetEngineThrustIndicationEnableInhibitA())
		GSEEngineThrustIndicationEnableA = true;
	else
		GSEEngineThrustIndicationEnableA = false;

	if (iu->ESEGetEngineThrustIndicationEnableInhibitB())
		GSEEngineThrustIndicationEnableB = true;
	else
		GSEEngineThrustIndicationEnableB = false;

	if (iu->ESEEDSLiftoffInhibitA())
		EDSLiftoffInhibitA = true;
	else
		EDSLiftoffInhibitA = false;

	if (iu->ESEEDSLiftoffInhibitB())
		EDSLiftoffInhibitB = true;
	else
		EDSLiftoffInhibitB = false;

	//S-IVB Thrust Monitor
	if (IUEDSBusPowered && SIVBEngineOutIndicationA && iu->GetLVCommandConnector()->GetSIVBThrustOK() == false)
		SIVBEngineThrustMonitorA = true;
	else
		SIVBEngineThrustMonitorA = false;

	if (IUEDSBusPowered && SIVBEngineOutIndicationB && iu->GetLVCommandConnector()->GetSIVBThrustOK() == false)
		SIVBEngineThrustMonitorB = true;
	else
		SIVBEngineThrustMonitorB = false;

	//Input signals
	LVRateAutoSwitch = iu->GetCommandConnector()->LVRateAutoSwitchState();
	TwoEngineOutAutoSwitch = iu->GetCommandConnector()->TwoEngineOutAutoSwitchState();
	AttRate = iu->GetLVRG()->GetRates();
	Stage = iu->GetLVCommandConnector()->GetStage();
	EDSBus1Powered = iu->GetCommandConnector()->IsEDSBusPowered(1);
	EDSBus2Powered = iu->GetCommandConnector()->IsEDSBusPowered(2);
	EDSBus3Powered = iu->GetCommandConnector()->IsEDSBusPowered(3);
	BECOA = iu->GetCommandConnector()->GetBECOCommand(true);
	BECOB = iu->GetCommandConnector()->GetBECOCommand(false);

	//Auto Abort Relays
	if (iu->ESEGetAutoAbortInhibit() || iu->GetControlDistributor()->GetTwoEnginesOutAutoAbortInhibit() || (IUEDSBusPowered && TwoEngineOutAutoSwitch == TOGGLESWITCH_DOWN))
		TwoEngOutAutoAbortDeactivate = true;
	else
		TwoEngOutAutoAbortDeactivate = false;

	if (iu->ESEGetAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitch == TOGGLESWITCH_DOWN))
		ExcessRatesAutoAbortDeactivatePY = true;
	else
		ExcessRatesAutoAbortDeactivatePY = false;

	if (iu->ESEGetAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitch == TOGGLESWITCH_DOWN))
		ExcessRatesAutoAbortDeactivateR = true;
	else
		ExcessRatesAutoAbortDeactivateR = false;

	//Backup LV Engines Cutoff Enable
	if (LVEnginesCutoffCommand2 == false && iu->GetEngineCutoffEnableTimer()->ContactClosed())
	{
		LVEnginesCutoffEnable2 = true;
	}

	//LV Engines EDS Cutoff Inhibit
	if (IUEDSBusPowered && !BECOA)
		LVEnginesCutoffFromSC1 = true;
	else
		LVEnginesCutoffFromSC1 = false;

	if (IUEDSBusPowered && (!BECOA || !BECOB))
		LVEnginesCutoffFromSC2 = true;
	else
		LVEnginesCutoffFromSC2 = false;

	if (IUEDSBusPowered && !BECOB)
		LVEnginesCutoffFromSC3 = true;
	else
		LVEnginesCutoffFromSC3 = false;

	//LV Engines Cutoff Command
	if (IUEDSBusPowered && LVEnginesCutoffEnable1 && LVEnginesCutoffVote())
		LVEnginesCutoffCommand1 = true;
	else
		LVEnginesCutoffCommand1 = false;

	if (IUEDSBusPowered && LVEnginesCutoffVote())
		LVEnginesCutoffCommand2 = true;
	else
		LVEnginesCutoffCommand2 = false;

	if (IUEDSBusPowered && LVEnginesCutoffEnable2 && LVEnginesCutoffVote())
		LVEnginesCutoffCommand3 = true;
	else
		LVEnginesCutoffCommand3 = false;

	//Auto Abort Logic
	AutoAbortBus = false;

	//Simulated Auto Abort
	if (iu->ESEAutoAbortSimulate()) AutoAbortBus = true;

	//Overrate Auto Abort
	double PYLimit;

	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit())
	{
		PYLimit = 9.2*RAD;
	}
	else
	{
		PYLimit = 4.0*RAD;
	}

	if (iu->ESEGetGSEOverrateSimulate() || (IUEDSBusPowered && (abs(AttRate.y) > PYLimit || abs(AttRate.z) > PYLimit)))
		ExcessivePitchYawRateIndication = true;
	else
		ExcessivePitchYawRateIndication = false;

	if (iu->ESEGetGSEOverrateSimulate() || (IUEDSBusPowered && abs(AttRate.x) > 20.0*RAD))
		ExcessiveRollRateIndication = true;
	else
		ExcessiveRollRateIndication = false;

	if (ExcessRatesAutoAbortDeactivatePY == false && ExcessivePitchYawRateIndication) AutoAbortBus = true;
	if (ExcessRatesAutoAbortDeactivateR == false && ExcessiveRollRateIndication) AutoAbortBus = true;

	//LV Guidance Light
	if (iu->GetLVDA()->GetGuidanceReferenceFailure())
		LVAttRefFail = true;
	else
		LVAttRefFail = false;

	if (LVAttRefFail && iu->GetSCControlPoweredFlight())
	{
		SCControlEnableRelay = true;
	}

	if (LVAttRefFail && (EDSBus1Powered || EDSBus3Powered))
		iu->GetCommandConnector()->SetLVGuidLight();
	else
		iu->GetCommandConnector()->ClearLVGuidLight();

	//LV Rates Light
	bool logic = ((!ExcessRatesAutoAbortDeactivatePY && LVAttRefFail) || ExcessivePitchYawRateIndication || ExcessiveRollRateIndication) && (EDSBus1Powered || EDSBus3Powered);

	if (logic)
		iu->GetCommandConnector()->SetLVRateLight();
	else
		iu->GetCommandConnector()->ClearLVRateLight();

	//Abort Light
	if ((EDSBus1Powered || EDSBus3Powered) && (PadAbortRequest || RangeSafetyDestructArmedAFromSIVB || RangeSafetyDestructArmedBFromSIVB))
		AbortLightSignal = true;
	else
		AbortLightSignal = false;

	//Liftoff Circuits
	if (EDSBus1Powered && !EDSLiftoffInhibitA && EDSLiftoffEnableA)
	{
		LiftoffA = true;
	}
	else
	{
		LiftoffA = false;
	}

	if (EDSBus3Powered && !EDSLiftoffInhibitB && EDSLiftoffEnableB)
	{
		LiftoffB = true;
	}
	else
	{
		LiftoffB = false;
	}

	//Failure code
	if (PlatformFailure && iu->GetLVCommandConnector()->GetMissionTime() > PlatformFailureTime && !iu->GetLVIMU()->IsFailed())
	{
		iu->GetLVIMU()->SetFailed();
	}
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

double EDS::GetLVTankPressure(int n)
{
	//S-IVB LOX No. 1 Tank Pressure
	if (n == 1)
	{
		return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
	}
	//S-IVB LOX No. 2 Tank Pressure
	else if (n == 2)
	{
		return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
	}
	//S-IVB Fuel No. 1 Tank Pressure
	else if (n == 3)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}
	//S-IVB Fuel No. 2 Tank Pressure
	else if (n == 4)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}

	return 0.0;
}

bool EDS::GetSCControl()
{
	return (SCControlEnableRelay && iu->GetCommandConnector()->GetCMCSIVBTakeover());
}

bool EDS::GetAllSIEnginesRunning()
{
	return (!SIAllEnginesOKA && !SIAllEnginesOKB);
}

bool EDS::IsEDSUnsafe()
{
	return iu->GetCommandConnector()->IsEDSUnsafe();
}

void EDS::ResetBus1()
{
	LVEnginesCutoffEnable1 = false;
	SIIEngineOutIndicationA = false;
	SIVBRestartAlert = false;
	SIVBEngineOutIndicationB = false;
	IUCommandSystemEnable = false;
}
void EDS::ResetBus2()
{
	LVEnginesCutoffEnable2 = false;
	SIIEngineOutIndicationB = false;
	SIVBEngineOutIndicationA = false;
	SCControlEnableRelay = false;
}

bool EDS::LVEnginesCutoffVote()
{
	if (!LVEnginesCutoffFromSC1 && !LVEnginesCutoffFromSC2) return true;
	if (!LVEnginesCutoffFromSC1 && !LVEnginesCutoffFromSC3) return true;
	if (!LVEnginesCutoffFromSC2 && !LVEnginesCutoffFromSC3) return true;

	return false;
}

bool EDS::GetIUCommandSystemEnable()
{
	return IUEDSBusPowered && (IUCommandSystemEnable || iu->GetCommandConnector()->GetIUUPTLMAccept() || LVEnginesCutoffCommand2);
}

void EDS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOABORTBUS", AutoAbortBus);
	papiWriteScenario_bool(scn, "EDSLIFTOFFENABLEA", EDSLiftoffEnableA);
	papiWriteScenario_bool(scn, "EDSLIFTOFFENABLEB", EDSLiftoffEnableB);
	papiWriteScenario_bool(scn, "LIFTOFFA", LiftoffA);
	papiWriteScenario_bool(scn, "LIFTOFFB", LiftoffB);
	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONB", SIVBEngineOutIndicationB);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFFENABLE1", LVEnginesCutoffEnable1);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFFENABLE2", LVEnginesCutoffEnable2);
	papiWriteScenario_bool(scn, "SIEDSCUTOFF", SIEDSCutoff);
	papiWriteScenario_bool(scn, "SIIEDSCUTOFF", SIIEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBEDSCUTOFF", SIVBEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL1", EDSAbortSignal1);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL2", EDSAbortSignal2);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL3", EDSAbortSignal3);
	papiWriteScenario_bool(scn, "SIVBRESTARTALERT", SIVBRestartAlert);
	papiWriteScenario_bool(scn, "SCCONTROLENABLERELAY", SCControlEnableRelay);
	papiWriteScenario_bool(scn, "IUEDSBUSPOWERED", IUEDSBusPowered);
	papiWriteScenario_bool(scn, "IUCOMMANDSYSTEMENABLE", IUCommandSystemEnable);
	papiWriteScenario_bool(scn, "ABORTLIGHTSIGNAL", AbortLightSignal);

	//Only save GSE relays before liftoff
	if (iu->IsUmbilicalConnected())
	{
		papiWriteScenario_bool(scn, "IULIFTOFFRELAY", IULiftoffRelay);
	}

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
		papiReadScenario_bool(line, "EDSLIFTOFFENABLEA", EDSLiftoffEnableA);
		papiReadScenario_bool(line, "EDSLIFTOFFENABLEB", EDSLiftoffEnableB);
		papiReadScenario_bool(line, "LIFTOFFA", LiftoffA);
		papiReadScenario_bool(line, "LIFTOFFB", LiftoffB);
		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
		papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
		papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONB", SIVBEngineOutIndicationB);
		papiReadScenario_bool(line, "LVENGINESCUTOFFENABLE1", LVEnginesCutoffEnable1);
		papiReadScenario_bool(line, "LVENGINESCUTOFFENABLE2", LVEnginesCutoffEnable2);
		papiReadScenario_bool(line, "SIEDSCUTOFF", SIEDSCutoff);
		papiReadScenario_bool(line, "SIIEDSCUTOFF", SIIEDSCutoff);
		papiReadScenario_bool(line, "SIVBEDSCUTOFF", SIVBEDSCutoff);
		papiReadScenario_bool(line, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);
		papiReadScenario_bool(line, "EDSABORTSIGNAL1", EDSAbortSignal1);
		papiReadScenario_bool(line, "EDSABORTSIGNAL2", EDSAbortSignal2);
		papiReadScenario_bool(line, "EDSABORTSIGNAL3", EDSAbortSignal3);
		papiReadScenario_bool(line, "SIVBRESTARTALERT", SIVBRestartAlert);
		papiReadScenario_bool(line, "SCCONTROLENABLERELAY", SCControlEnableRelay);
		papiReadScenario_bool(line, "IUEDSBUSPOWERED", IUEDSBusPowered);
		papiReadScenario_bool(line, "IUCOMMANDSYSTEMENABLE", IUCommandSystemEnable);
		papiReadScenario_bool(line, "ABORTLIGHTSIGNAL", AbortLightSignal);

		//GSE
		papiReadScenario_bool(line, "IULIFTOFFRELAY", IULiftoffRelay);
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

void EDS1B::Timestep(double simdt)
{
	if (iu->GetLVCommandConnector() == NULL) return;
	if (iu->GetCommandConnector() == NULL) return;

	EDS::Timestep(simdt);

	SIAllEnginesOKA = false;
	SIAllEnginesOKB = false;

	//S-IB Thrust Monitor
	if (IUEDSBusPowered && Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(ThrustOKSignal);
		for (int i = 0;i < 8;i++)
		{
			SIThrustNotOK[i] = !ThrustOKSignal[i];

			if (SIThrustNotOK[i])
			{
				SIAllEnginesOKA = true;
				SIAllEnginesOKB = true;
			}
		}
	}
	else
	{
		for (int i = 0;i < 8;i++)
		{
			SIThrustNotOK[i] = false;
		}
	}

	//EDS Engine Cutoff
	if (LVEnginesCutoffCommand1 || LVEnginesCutoffCommand3)
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

	//Two Engines Out Auto Abort
	if (IUEDSBusPowered && !TwoEngOutAutoAbortDeactivate)
	{
		int enginesout = 0;

		for (int i = 0;i < 8;i++)
		{
			if (SIThrustNotOK[i]) enginesout++;
		}

		if (enginesout >= 2) AutoAbortBus = true;
	}

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

	// Update engine indicators and failure flags
	bool LVIndicatorsPower = ((EDSBus1Powered || EDSBus3Powered) && (!GSEEngineThrustIndicationEnableA || !GSEEngineThrustIndicationEnableB));

	//Engine Light 1
	if (LVIndicatorsPower && (SIThrustNotOK[0] || SIVBEngineThrustMonitorA || SIVBEngineThrustMonitorB))
		iu->GetCommandConnector()->SetEngineIndicator(1);
	else
		iu->GetCommandConnector()->ClearEngineIndicator(1);
	//Engine Lights 2-8
	for (int i = 1;i < 8;i++)
	{
		if (LVIndicatorsPower && SIThrustNotOK[i])
			iu->GetCommandConnector()->SetEngineIndicator(i + 1);
		else
			iu->GetCommandConnector()->ClearEngineIndicator(i + 1);
	}
}

EDSSV::EDSSV(IU *iu) : EDS(iu)
{
	for (int i = 0;i < 5;i++)
	{
		SIThrustNotOK[i] = false;
		SIIEngineThrustMonitorA[i] = false;
		SIIEngineThrustMonitorB[i] = false;
		ThrustOKSignal[i] = false;
	}
	SIISIVBNotSeparated = false;
	SIISIVBSepSeqStart = false;
}

double EDSSV::GetLVTankPressure(int n)
{
	if (n == 1)
	{
		if (SIISIVBNotSeparated)
		{
			//S-II LH2 Tank Ullage Pressure No. 1
			return iu->GetLVCommandConnector()->GetSIIFuelTankPressurePSI();
		}
		else
		{
			//S-IVB LOX No. 1 Tank Pressure
			return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
		}
	}
	else if (n == 2)
	{
		if (SIISIVBNotSeparated)
		{
			//S-II LH2 Tank Ullage Pressure No. 2
			return iu->GetLVCommandConnector()->GetSIIFuelTankPressurePSI();
		}
		else
		{
			//S-IVB LOX No. 2 Tank Pressure
			return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
		}
	}
	//S-IVB Fuel No. 1 Tank Pressure
	else if (n == 3)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}
	//S-IVB Fuel No. 2 Tank Pressure
	else if (n == 4)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}

	return 0.0;
}

void EDSSV::Timestep(double simdt)
{
	if (iu->GetLVCommandConnector() == NULL) return;
	if (iu->GetCommandConnector() == NULL) return;

	EDS::Timestep(simdt);

	if (Stage < LAUNCH_STAGE_SIVB)
		SIISIVBNotSeparated = true;
	else
		SIISIVBNotSeparated = false;

	SIAllEnginesOKA = false;
	SIAllEnginesOKB = false;

	//S-IC Thrust Monitor
	if (IUEDSBusPowered && Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(ThrustOKSignal);
		for (int i = 0;i < 5;i++)
		{
			SIThrustNotOK[i] = !ThrustOKSignal[i];

			if (SIThrustNotOK[i])
			{
				SIAllEnginesOKA = true;
				SIAllEnginesOKB = true;
			}
		}
	}
	else
	{
		for (int i = 0;i < 5;i++)
		{
			SIThrustNotOK[i] = false;
		}
	}

	//S-II Thrust Monitor
	if (IUEDSBusPowered && (Stage == LAUNCH_STAGE_TWO || Stage == LAUNCH_STAGE_TWO_ISTG_JET))
	{
		iu->GetLVCommandConnector()->GetSIIThrustOK(ThrustOKSignal);
		for (int i = 0;i < 5;i++)
		{
			SIIEngineThrustMonitorA[i] = SIIEngineOutIndicationA && !ThrustOKSignal[SIIEngInd[i]];
			SIIEngineThrustMonitorB[i] = SIIEngineOutIndicationB && !ThrustOKSignal[SIIEngInd[i]];
		}
	}
	else
	{
		for (int i = 0;i < 5;i++)
		{
			SIIEngineThrustMonitorA[i] = false;
			SIIEngineThrustMonitorB[i] = false;
		}
	}

	//EDS Engine Cutoff
	if (LVEnginesCutoffCommand1 || LVEnginesCutoffCommand3)
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

	if (IUEDSBusPowered && !TwoEngOutAutoAbortDeactivate)
	{
		int enginesout = 0;

		for (int i = 0;i < 5;i++)
		{
			if (SIThrustNotOK[i]) enginesout++;
		}

		if (enginesout >= 2) AutoAbortBus = true;
	}

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

	//Update engine indicators and failure flags
	bool LVIndicatorsPower = ((EDSBus1Powered || EDSBus3Powered) && (!GSEEngineThrustIndicationEnableA || !GSEEngineThrustIndicationEnableB));

	//Engine Light 1
	if (LVIndicatorsPower && (SIThrustNotOK[0] || SIIEngineThrustMonitorA[0] || SIIEngineThrustMonitorB[0] || SIVBEngineThrustMonitorA || SIVBEngineThrustMonitorB))
		iu->GetCommandConnector()->SetEngineIndicator(1);
	else
		iu->GetCommandConnector()->ClearEngineIndicator(1);

	//Engine Light 2-5
	for (int i = 1;i < 5;i++)
	{
		if (LVIndicatorsPower && (SIThrustNotOK[i] || SIIEngineThrustMonitorA[i] || SIIEngineThrustMonitorB[i]))
			iu->GetCommandConnector()->SetEngineIndicator(i + 1);
		else
			iu->GetCommandConnector()->ClearEngineIndicator(i + 1);
	}

	//Second Plane Separation Monitor
	bool SIISecPlaneSepA = EDSBus1Powered && SIIEngineOutIndicationA && (SIVBRestartAlert || Stage < LAUNCH_STAGE_TWO_ISTG_JET);
	bool SIISecPlaneSepB = EDSBus3Powered && SIIEngineOutIndicationB && (SIVBRestartAlert || Stage < LAUNCH_STAGE_TWO_ISTG_JET);

	if (SIISecPlaneSepA || SIISecPlaneSepB)
		iu->GetCommandConnector()->SetSIISep();
	else
		iu->GetCommandConnector()->ClearSIISep();

	//S-II/S-IVB Separation Sequence Start
	if (IUEDSBusPowered && iu->GetCommandConnector()->GetSIISIVbDirectStagingSignal())
		SIISIVBSepSeqStart = true;
	else
		SIISIVBSepSeqStart = false;

	//sprintf(oapiDebugString(), "%f", PlatformFailureTime);
}