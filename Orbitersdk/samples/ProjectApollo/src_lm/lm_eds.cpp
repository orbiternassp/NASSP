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
#include "LEM.h"
#include "papi.h"
#include "lm_eds.h"
#include "LM_DescentStageResource.h"

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
	AscentPropCompValvesRelay = false;
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
		AscentPropCompValvesRelay = true;

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
		AscentPropCompValvesRelay = false;
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

bool LEM_EDRelayBox::GetEDRelayMonitor()
{
	return !RCSPropPressRelay && !LandingGearDeployRelay && !DescentEngineOnRelay && !AscentPropPressTank1Relay && !AscentPropPressTank2Relay && 
		!AscentPropCompValvesRelay && !DescentPropVentRelay && !DescentPropPressRelay && !DescentTankIsolValvesRelay;
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
LEM_EDS::LEM_EDS() :
	HeliumPressurizationDelayA(6.0),
	HeliumPressurizationDelayB(6.0)
{
	lem = NULL;
	LG_Deployed = FALSE;
	Deadface = false;

	gear_state.SetOperatingSpeed(1.0);
	anim_Gear = -1;

	for (int i = 0; i < 4; i++) {
		mgt_Leg[i] = NULL;
		mgt_Strut[i] = NULL;
		mgt_Downlock[i] = NULL;
	}

	for (int i = 0; i < 3; i++) {
		mgt_Probes1[i] = NULL;
		mgt_Probes2[i] = NULL;
	}
}

LEM_EDS::~LEM_EDS() {

	for (int i = 0; i < 4; i++) {
		if (mgt_Leg[i]) delete mgt_Leg[i];
		if (mgt_Strut[i]) delete mgt_Strut[i];
		if (mgt_Downlock[i]) delete mgt_Downlock[i];
	}

	for (int i = 0; i < 3; i++) {
		if (mgt_Probes1[i]) delete mgt_Probes1[i];
		if (mgt_Probes2[i]) delete mgt_Probes2[i];
	}
}

void LEM_EDS::Init(LEM *s) {
	lem = s;
	if (lem->stage < 2)
	{
		RelayBoxA.Init(lem, &lem->EDS_CB_LOGIC_A, &lem->ED28VBusA, lem->EDBatteryA);
	}
	RelayBoxB.Init(lem, &lem->EDS_CB_LOGIC_B, &lem->ED28VBusB, lem->EDBatteryB);
}

void LEM_EDS::DefineAnimations(UINT idx) {

	// Landing Gear animations
	ANIMATIONCOMPONENT_HANDLE	ach_GearLeg[4], ach_GearStrut[4], ach_GearLock[4], achGearProbes1[3], achGearProbes2[3];

	const VECTOR3	DES_LEG_AXIS[4] = { { -1, 0, 0 },{ 1, 0, 0 },{ 0, 0,-1 },{ 0, 0, 1 } };
	const VECTOR3	DES_PROBE_AXIS[3] = { { 1, 0, 0 },{ 0, 0,-1 },{ 0, 0, 1 } };
	const VECTOR3	DES_LEG_PIVOT[4] = { { 0.00, 0.55965, 2.95095 },{ 0.00, 0.55965, -2.95095 },{ -2.95095, 0.55965, 0.00 },{ 2.95095, 0.55965, 0.00 } };
	const VECTOR3	DES_STRUT_PIVOT[4] = { { 0.00,-1.27178, 3.83061 },{ 0.00,-1.27178,-3.83061 },{ -3.83061,-1.27178, 0.00 },{ 3.83061,-1.27178, 0.00 } };
	const VECTOR3	DES_LOCK_PIVOT[4] = { { 0.00,-1.02, 2.91 },{ 0.00,-1.02,-2.91 },{ -2.91,-1.02, 0.00 },{ 2.91,-1.02, 0.00 } };
	const VECTOR3	DES_PROBE_PIVOT[3] = { { 0.00, 0.55965, -2.95095 },{ -2.95095, 0.55965, 0.00 },{ 2.95095, 0.55965, 0.00 } };
	const VECTOR3	DES_PROBE2_PIVOT[3] = { { -0.00696, -2.56621, -4.10490 },{ -4.10426, -2.56621, 0.007022 },{ 4.10458,-2.56621, -0.007012 } };

	static UINT meshgroup_Legs[4][3] = {
		{ DS_GRP_Footpad,	DS_GRP_LowerStrut,	DS_GRP_MainStrut },
		{ DS_GRP_FootpadAft,	DS_GRP_LowerStrutAft, DS_GRP_MainStrutAft },
		{ DS_GRP_FootpadLeft,	DS_GRP_LowerStrutLeft,	DS_GRP_MainStrutLeft },
		{ DS_GRP_FootpadRight,	DS_GRP_LowerStrutRight,	DS_GRP_MainStrutRight } };
	static UINT meshgroup_Struts[4] = { DS_GRP_SupportStruts2, DS_GRP_SupportStruts2Aft, DS_GRP_SupportStruts2Left, DS_GRP_SupportStruts2Right };
	static UINT meshgroup_Locks[4] = { DS_GRP_Downlock, DS_GRP_DownlockAft, DS_GRP_DownlockLeft, DS_GRP_DownlockRight };
	static UINT meshgroup_Ladder = DS_GRP_Ladder;
	static UINT meshgroup_Probes1[3] = { DS_GRP_Probes1Aft, DS_GRP_Probes1Left, DS_GRP_Probes1Right };
	static UINT meshgroup_Probes2[3] = { DS_GRP_Probes2Aft, DS_GRP_Probes2Left, DS_GRP_Probes2Right };

	anim_Gear = lem->CreateAnimation(1.0);

	for (int i = 0; i < 4; i++)
	{
		mgt_Leg[i] = new MGROUP_ROTATE(idx, &meshgroup_Legs[i][0], 3, DES_LEG_PIVOT[i], DES_LEG_AXIS[i], (float)(45 * RAD));
		mgt_Strut[i] = new MGROUP_ROTATE(idx, &meshgroup_Struts[i], 1, DES_STRUT_PIVOT[i], DES_LEG_AXIS[i], (float)(-63 * RAD));
		mgt_Downlock[i] = new MGROUP_ROTATE(idx, &meshgroup_Locks[i], 1, DES_LOCK_PIVOT[i], DES_LEG_AXIS[i], (float)(155 * RAD));

		ach_GearLeg[i] = lem->AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Leg[i]);
		ach_GearStrut[i] = lem->AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Strut[i], ach_GearLeg[i]);
		ach_GearLock[i] = lem->AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Downlock[i], ach_GearStrut[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		mgt_Probes1[i] = new MGROUP_ROTATE(idx, &meshgroup_Probes1[i], 1, DES_PROBE_PIVOT[i], DES_PROBE_AXIS[i], (float)(45 * RAD));
		mgt_Probes2[i] = new MGROUP_ROTATE(idx, &meshgroup_Probes2[i], 1, DES_PROBE2_PIVOT[i], DES_PROBE_AXIS[i], (float)(171 * RAD));

		achGearProbes1[i] = lem->AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Probes1[i]);
		achGearProbes2[i] = lem->AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Probes2[i], achGearProbes1[i]);
	}

	static MGROUP_ROTATE mgt_Ladder(idx, &meshgroup_Ladder, 1, DES_LEG_PIVOT[0], DES_LEG_AXIS[0], (float)(45 * RAD));
	lem->AddAnimationComponent(anim_Gear, 0.0, 1, &mgt_Ladder);

	lem->SetAnimation(anim_Gear, gear_state.State());
}

void LEM_EDS::DeleteAnimations(){

	if (anim_Gear != -1) lem->DelAnimation(anim_Gear);
	anim_Gear = -1;
}

void LEM_EDS::Timestep(double simdt) {

	// Animate Gear
	if (lem->stage < 2) {
		if (gear_state.Process(simdt)) {
			lem->SetAnimation(anim_Gear, gear_state.State());
		}
		if (LG_Deployed) gear_state.Open();
	}
	
	if (lem->stage < 2)
	{
		RelayBoxA.Timestep(simdt);
		HeliumPressurizationDelayA.Timestep(simdt);
	}
	RelayBoxB.Timestep(simdt);
	HeliumPressurizationDelayB.Timestep(simdt);

	if (RelayBoxA.GetDescentEngineOnRelay())
	{
		HeliumPressurizationDelayA.SetRunning(true);
	}
	if (RelayBoxB.GetDescentEngineOnRelay())
	{
		HeliumPressurizationDelayB.SetRunning(true);
	}

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


	// Ascent Helium Tank 1 Isolation Valve
	pyroA = false, pyroB = false;
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetAscentPropPressTank1Relay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetAscentPropPressTank1Relay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}
	lem->AscentHeliumIsol1PyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

	// Ascent Helium Tank 2 Isolation Valve
	pyroA = false, pyroB = false;
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetAscentPropPressTank2Relay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetAscentPropPressTank2Relay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}
	lem->AscentHeliumIsol2PyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

	// Ascent Propellant Compatibility Valves
	pyroA = false, pyroB = false;
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetAscentPropCompValvesRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetAscentPropCompValvesRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}
	lem->AscentFuelCompValvePyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));
	lem->AscentOxidCompValvePyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));


	// RCS Helium Supply Valves
	pyroA = false, pyroB = false;
	if (lem->stage < 2)
	{
		if (RelayBoxA.GetRCSPropPressRelay() && RelayBoxA.GetMasterArmRelay())
		{
			// Blow Pyro A
			pyroA = true;
		}
	}
	if (RelayBoxB.GetRCSPropPressRelay() && RelayBoxB.GetMasterArmRelay())
	{
		// Blow Pyro B
		pyroB = true;
	}
	lem->RCSHeliumSupplyAPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));
	lem->RCSHeliumSupplyBPyrosFeeder.WireToBuses((pyroA ? &lem->ED28VBusA : NULL),
		(pyroB ? &lem->ED28VBusB : NULL));

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
	HeliumPressurizationDelayA.SaveState(scn, "HEPRESSDELAYA_BEGIN", "HEPRESSDELAYA_END");
	HeliumPressurizationDelayB.SaveState(scn, "HEPRESSDELAYB_BEGIN", "HEPRESSDELAYB_END");
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
		else if (!strnicmp(line, "DEADFACE", 8)) {
			sscanf(line + 8, "%d", &dec);
			Deadface = (bool)(dec != 0);
		}
		else if (!strnicmp(line, "HEPRESSDELAYA_BEGIN", sizeof("HEPRESSDELAYA_BEGIN"))) {
			HeliumPressurizationDelayA.LoadState(scn, "HEPRESSDELAYA_END");
		}
		else if (!strnicmp(line, "HEPRESSDELAYB_BEGIN", sizeof("HEPRESSDELAYB_BEGIN"))) {
			HeliumPressurizationDelayB.LoadState(scn, "HEPRESSDELAYB_END");
		}
		if (!strnicmp(line, "LEM_EDS_RELAYBOXA_BEGIN", sizeof("LEM_EDS_RELAYBOXA_BEGIN"))) {
			RelayBoxA.LoadState(scn, "LEM_EDS_RELAYBOX_END");
		}
		else if (!strnicmp(line, "LEM_EDS_RELAYBOXB_BEGIN", sizeof("LEM_EDS_RELAYBOXB_BEGIN"))) {
			RelayBoxB.LoadState(scn, "LEM_EDS_RELAYBOX_END");
		}
	}
}