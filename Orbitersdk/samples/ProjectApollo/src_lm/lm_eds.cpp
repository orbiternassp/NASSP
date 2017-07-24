/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

LM Explosive Devices Subsystem

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
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "papi.h"
#include "lm_eds.h"

LEM_EDRelayBox::LEM_EDRelayBox():
	StagingBoltsNutsDelay(0.05),
	CableCuttingDelay(0.05),
	DescentEngineOnDelay(1.3)
{
	MasterArmRelay = false;
	StagingRelay = false;
	RCSPropPressRelay = false;
	LandingGearDeployRelay = false;
	DescentEngineOnRelay = false;
	AscentPropPressTank1Relay = false;
	AscentPropPressTank2Relay = false;
	DescentPropVentRelay = false;
	DescentPropPressRelay = false;
	DescentTankIsolValvesRelay = false;
}

void LEM_EDRelayBox::Init(LEM *l, CircuitBrakerSwitch *LogicPower, DCbus *edbus, Battery *edbatt)
{
	lem = l;
	EDLogicPower = LogicPower;
	EDDCBus = edbus;
	EDBattery = edbatt;
}

void LEM_EDRelayBox::Timestep(double simdt)
{
	StagingBoltsNutsDelay.Timestep(simdt);
	CableCuttingDelay.Timestep(simdt);
	DescentEngineOnDelay.Timestep(simdt);

	if (HasDCPower())
	{
		//Master Arm Relay
		if ((lem->EDMasterArm.IsUp() && lem->AbortStageSwitch.IsUp()) || lem->AbortStageSwitch.IsDown())
		{
			MasterArmRelay = true;
			EDDCBus->WireTo(EDBattery);
		}
		else if (lem->EDMasterArm.IsDown() && lem->AbortStageSwitch.IsUp())
		{
			MasterArmRelay = false;
			EDDCBus->Disconnect();
		}

		//Staging Relay
		if (lem->EDStage.IsUp())
		{
			StagingRelay = true;
		}
		else if (lem->EDStageRelay.IsUp())
		{
			StagingRelay = false;
		}
	}

	if (HasDCPower() && StagingRelay)
	{
		StagingBoltsNutsDelay.SetRunning(true);
	}

	if (HasDCPower() && StagingRelay && StagingBoltsNutsDelay.ContactClosed())
	{
		InitiateStagingRelay = true;
	}
	else
	{
		InitiateStagingRelay = false;
	}

	if (HasDCPower() && StagingRelay && InitiateStagingRelay)
	{
		CableCuttingDelay.SetRunning(true);
	}

	if (HasDCPower() && StagingRelay && CableCuttingDelay.ContactClosed())
	{
		CableCuttingRelay = true;
	}
	else
	{
		CableCuttingRelay = false;
	}

	if (HasDCPower() && StagingRelay)
	{
		StagingBoltsNutsRelay = true;
	}
	else if (HasDCPower() && InitiateStagingRelay)
	{
		StagingBoltsNutsRelay = true;
	}
	else
	{
		StagingBoltsNutsRelay = false;
	}

	if (HasDCPower() && StagingRelay)
	{
		DeadFacingRelay = true;
	}
	else
	{
		DeadFacingRelay = false;
	}

	if (HasDCPower() && lem->EDHePressRCS.IsUp())
	{
		RCSPropPressRelay = true;
	}
	else
	{
		RCSPropPressRelay = false;
	}

	if (HasDCPower() && lem->EDLGDeploy.IsUp())
	{
		LandingGearDeployRelay = true;
	}
	else
	{
		LandingGearDeployRelay = false;
	}

	if (lem->deca.GetThrustOn())
	{
		DescentEngineOnDelay.SetRunning(true);
	}

	if (DescentEngineOnDelay.ContactClosed())
	{
		DescentEngineOnRelay = true;
	}
	else
	{
		DescentEngineOnRelay = false;
	}

	if (HasDCPower() && (lem->EDHePressASC.IsUp() || lem->AbortStageSwitch.IsDown()))
	{
		//TBD: K12
		if (lem->EDASCHeSel.IsUp())
		{
			AscentPropPressTank1Relay = true;
			AscentPropPressTank2Relay = false;
		}
		else if (lem->EDASCHeSel.IsCenter())
		{
			AscentPropPressTank1Relay = true;
			AscentPropPressTank2Relay = true;
		}
		else if (lem->EDASCHeSel.IsDown())
		{
			AscentPropPressTank1Relay = false;
			AscentPropPressTank2Relay = true;
		}
	}
	else
	{
		AscentPropPressTank1Relay = false;
		AscentPropPressTank2Relay = false;
	}

	if (HasDCPower() && lem->EDDesVent.IsUp())
	{
		DescentPropVentRelay = true;
	}
	else
	{
		DescentPropVentRelay = false;
	}

	if (HasDCPower() && lem->EDHePressDesStart.IsUp())
	{
		DescentPropPressRelay = true;
	}
	else
	{
		DescentPropPressRelay = false;
	}

	if (HasDCPower() && lem->EDDesPrpIsol.IsUp())
	{
		DescentTankIsolValvesRelay = true;
	}
	else
	{
		DescentTankIsolValvesRelay = false;
	}
}

bool LEM_EDRelayBox::HasDCPower()
{
	if (EDLogicPower)
		return EDLogicPower->Voltage() > SP_MIN_DCVOLTAGE;

	return false;
}

bool LEM_EDRelayBox::GetStageRelayMonitor()
{
	if (!HasDCPower()) return false;

	if (!MasterArmRelay && !StagingRelay && !InitiateStagingRelay && !CableCuttingRelay && !StagingBoltsNutsRelay && !DeadFacingRelay)
	{
		return false;
	}
	else if (MasterArmRelay && !StagingRelay && !InitiateStagingRelay && !CableCuttingRelay && !StagingBoltsNutsRelay && !DeadFacingRelay)
	{
		return true;
	}
	else if (!MasterArmRelay && (StagingRelay || InitiateStagingRelay || CableCuttingRelay || StagingBoltsNutsRelay || DeadFacingRelay))
	{
		return true;
	}

	return false;
}

void LEM_EDRelayBox::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "MASTERARMRELAY", MasterArmRelay);
	papiWriteScenario_bool(scn, "STAGINGRELAY", StagingRelay);
	papiWriteScenario_bool(scn, "INITIATESTAGINGRELAY", InitiateStagingRelay);
	papiWriteScenario_bool(scn, "CABLECUTTINGRELAY", CableCuttingRelay);
	papiWriteScenario_bool(scn, "STAGINGBOLTSNUTSRELAY", StagingBoltsNutsRelay);
	papiWriteScenario_bool(scn, "DEADFACINGRELAY", DeadFacingRelay);
	papiWriteScenario_bool(scn, "RCSPROPPRESSRELAY", RCSPropPressRelay);
	papiWriteScenario_bool(scn, "LANDINGGEARDEPLOYRELAY", LandingGearDeployRelay);
	papiWriteScenario_bool(scn, "DESCENTENGINEONRELAY", DescentEngineOnRelay);
	papiWriteScenario_bool(scn, "ASCENTPROPPRESSTANK1RELAY", AscentPropPressTank1Relay);
	papiWriteScenario_bool(scn, "ASCENTPROPPRESSTANK2RELAY", AscentPropPressTank2Relay);
	papiWriteScenario_bool(scn, "ASCENTPROPCOMPVALVESRELAY", AscentPropCompValvesRelay);
	papiWriteScenario_bool(scn, "DESCENTPROPVENTRELAY", DescentPropVentRelay);
	papiWriteScenario_bool(scn, "DESCENTPROPPRESSRELAY", DescentPropPressRelay);
	papiWriteScenario_bool(scn, "DESCENTTANKISOLVALVESRELAY", DescentTankIsolValvesRelay);

	StagingBoltsNutsDelay.SaveState(scn, "STAGINGDELAY_BEGIN", "STAGINGDELAY_END");
	CableCuttingDelay.SaveState(scn, "CABLECUTTINGDELAY_BEGIN", "CABLECUTTINGDELAY_END");
	DescentEngineOnDelay.SaveState(scn, "ENGINEONDELAY_BEGIN", "ENGINEONDELAY_END");

	oapiWriteLine(scn, end_str);
}

void LEM_EDRelayBox::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "MASTERARMRELAY", MasterArmRelay);
		papiReadScenario_bool(line, "STAGINGRELAY", StagingRelay);
		papiReadScenario_bool(line, "INITIATESTAGINGRELAY", InitiateStagingRelay);
		papiReadScenario_bool(line, "CABLECUTTINGRELAY", CableCuttingRelay);
		papiReadScenario_bool(line, "STAGINGBOLTSNUTSRELAY", StagingBoltsNutsRelay);
		papiReadScenario_bool(line, "DEADFACINGRELAY", DeadFacingRelay);
		papiReadScenario_bool(line, "RCSPROPPRESSRELAY", RCSPropPressRelay);
		papiReadScenario_bool(line, "LANDINGGEARDEPLOYRELAY", LandingGearDeployRelay);
		papiReadScenario_bool(line, "DESCENTENGINEONRELAY", DescentEngineOnRelay);
		papiReadScenario_bool(line, "ASCENTPROPPRESSTANK1RELAY", AscentPropPressTank1Relay);
		papiReadScenario_bool(line, "ASCENTPROPPRESSTANK2RELAY", AscentPropPressTank2Relay);
		papiReadScenario_bool(line, "ASCENTPROPCOMPVALVESRELAY", AscentPropCompValvesRelay);
		papiReadScenario_bool(line, "DESCENTPROPVENTRELAY", DescentPropVentRelay);
		papiReadScenario_bool(line, "DESCENTPROPPRESSRELAY", DescentPropPressRelay);
		papiReadScenario_bool(line, "DESCENTTANKISOLVALVESRELAY", DescentTankIsolValvesRelay);

		if (!strnicmp(line, "STAGINGDELAY_BEGIN", sizeof("STAGINGDELAY_BEGIN"))) {
			StagingBoltsNutsDelay.LoadState(scn, "STAGINGDELAY_END");
		}
		else if (!strnicmp(line, "CABLECUTTINGDELAY_BEGIN", sizeof("CABLECUTTINGDELAY_BEGIN"))) {
			CableCuttingDelay.LoadState(scn, "CABLECUTTINGDELAY_END");
		}
		else if (!strnicmp(line, "ENGINEONDELAY_BEGIN", sizeof("ENGINEONDELAY_BEGIN"))) {
			DescentEngineOnDelay.LoadState(scn, "ENGINEONDELAY_END");
		}
	}
}

// EXPLOSIVE DEVICES SYSTEM
LEM_EDS::LEM_EDS() {
	lem = NULL;
	LG_Deployed = FALSE;
	Deadface = false;
}

void LEM_EDS::Init(LEM *s) {
	lem = s;
	if (lem->stage < 2)
	{
		RelayBoxA.Init(lem, &lem->EDS_CB_LOGIC_A, &lem->ED28VBusA, lem->EDBatteryA);
	}
	RelayBoxB.Init(lem, &lem->EDS_CB_LOGIC_B, &lem->ED28VBusB, lem->EDBatteryB);
}

void LEM_EDS::TimeStep(double simdt) {
	
	if (lem->stage < 2)
	{
		RelayBoxA.Timestep(simdt);
	}
	RelayBoxB.Timestep(simdt);

	bool pyroA = false, pyroB = false;

	//Landing Gear Deployment
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetLandingGearDeployRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetLandingGearDeployRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}

	lem->LandingGearPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

	pyroA = false, pyroB = false;

	//Descent Propellant Isolation Valves
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetDescentTankIsolValvesRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetDescentTankIsolValvesRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}

	lem->DescentPropIsolPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

	pyroA = false, pyroB = false;

	//Descent Engine Start (Ambient Helium)
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetDescentPropPressRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetDescentPropPressRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}

	lem->DescentEngineStartPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

	pyroA = false, pyroB = false;

	//Descent Engine On (Supercritical Helium)
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetDescentEngineOnRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetDescentEngineOnRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}

	lem->DescentEngineOnPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

	pyroA = false, pyroB = false;

	//Descent Propellant Venting
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetDescentPropVentRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetDescentPropVentRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}

	lem->DescentPropVentPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

	//Ascent stage deadfacing

	pyroA = false, pyroB = false;

	if (lem->stage < 2)
	{
		if (RelayBoxA.GetDeadFacingRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetDeadFacingRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}

	if ((pyroA || pyroB) && !Deadface)
	{
		// Disconnect EPS stuff
		lem->DES_LMPs28VBusA.Disconnect();
		lem->DES_LMPs28VBusB.Disconnect();
		lem->DES_CDRs28VBusA.Disconnect();
		lem->DES_CDRs28VBusB.Disconnect();
		// Disconnect monitor select rotaries
		lem->EPSMonitorSelectRotary.SetSource(1, NULL);
		lem->EPSMonitorSelectRotary.SetSource(2, NULL);
		lem->EPSMonitorSelectRotary.SetSource(3, NULL);
		lem->EPSMonitorSelectRotary.SetSource(4, NULL);
		lem->EPSEDVoltSelect.SetSource(0, NULL);
		// Change descent TB
		lem->DSCBattFeedTB.SetState(0);
		Deadface = true;
	}

	//Staging Bolts
	pyroA = false;
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetStagingRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	lem->StagingBoltsPyros.WireTo((pyroA ? &lem->ED28VBusA : NULL));

	//Staging Nuts
	pyroB = false;
	if (RelayBoxB.GetStagingRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}
	lem->StagingNutsPyros.WireTo(pyroB ? &lem->ED28VBusB : NULL);

	// Interstage umbilical severance
	pyroA = false, pyroB = false;
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetCableCuttingRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetCableCuttingRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}
	lem->CableCuttingPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));
	
	// Set TBs
	// BP when descent stage detached
	if (LG_Deployed == TRUE && lem->status < 2) { lem->EDLGTB.SetState(1); }
	else { lem->EDLGTB.SetState(0); }

	// PROCESS THESE IN THIS ORDER:
	// Landing Gear Deployment
	if (LG_Deployed == FALSE && lem->status == 0) {
		// Check?
		if (lem->LandingGearPyros.Blown()) {
			// Deploy landing gear
			lem->SetLmVesselHoverStage();
			LG_Deployed = TRUE;
		}
	}

	//sprintf(oapiDebugString(), "Bolts %d Nuts %d Cables %d", lem->StagingBoltsPyros.Blown(), lem->StagingNutsPyros.Blown(), lem->CableCuttingPyros.Blown());
}

void LEM_EDS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "LG_DEP", LG_Deployed);
	oapiWriteScenario_int(scn, "DEADFACE", Deadface);
	oapiWriteScenario_int(scn, "SOV_A", lem->GetValveState(LEM_RCS_MAIN_SOV_A));
	oapiWriteScenario_int(scn, "SOV_B", lem->GetValveState(LEM_RCS_MAIN_SOV_B));
	if (lem->stage < 2)
		RelayBoxA.SaveState(scn, "LEM_EDS_RELAYBOXA_BEGIN", "LEM_EDS_RELAYBOX_END");
	RelayBoxB.SaveState(scn, "LEM_EDS_RELAYBOXB_BEGIN", "LEM_EDS_RELAYBOX_END");
	oapiWriteLine(scn, end_str);
}

void LEM_EDS::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "LG_DEP", 6)) {
			sscanf(line + 6, "%d", &dec);
			LG_Deployed = (bool)(dec != 0);
		}
		if (!strnicmp(line, "DEADFACE", 8)) {
			sscanf(line + 8, "%d", &dec);
			Deadface = (bool)(dec != 0);
		}
		if (!strnicmp(line, "SOV_A", 5)) {
			sscanf(line + 6, "%d", &dec);
			lem->SetValveState(LEM_RCS_MAIN_SOV_A, (bool)(dec != 0));
		}
		if (!strnicmp(line, "SOV_B", 5)) {
			sscanf(line + 6, "%d", &dec);
			lem->SetValveState(LEM_RCS_MAIN_SOV_B, (bool)(dec != 0));
		}
		if (!strnicmp(line, "LEM_EDS_RELAYBOXA_BEGIN", sizeof("LEM_EDS_RELAYBOXA_BEGIN"))) {
			RelayBoxA.LoadState(scn, "LEM_EDS_RELAYBOX_END");
		}
		else if (!strnicmp(line, "LEM_EDS_RELAYBOXB_BEGIN", sizeof("LEM_EDS_RELAYBOXB_BEGIN"))) {
			RelayBoxB.LoadState(scn, "LEM_EDS_RELAYBOX_END");
		}
	}
}