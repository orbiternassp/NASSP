/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

LM Explosive Devices Subsystem (Header)

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

#include "DelayTimer.h"
#include "animations.h"

class LEM_EDRelayBox
{
public:
	LEM_EDRelayBox();
	void Timestep(double simdt);
	void Init(LEM *l, CircuitBrakerSwitch *LogicPower, DCbus *edbus, Battery *edbatt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	bool GetLandingGearDeployRelay() { return LandingGearDeployRelay; }
	bool GetStagingRelay() { return StagingBoltsNutsRelay; }
	bool GetMasterArmRelay() { return MasterArmRelay; }
	bool GetDeadFacingRelay() { return DeadFacingRelay; }
	bool GetCableCuttingRelay() { return CableCuttingRelay; }
	bool GetDescentPropVentRelay() { return DescentPropVentRelay; }
	bool GetDescentEngineOnRelay() { return DescentEngineOnRelay; }
	bool GetDescentPropPressRelay() { return DescentPropPressRelay; }
	bool GetDescentTankIsolValvesRelay() { return DescentTankIsolValvesRelay; }
	bool GetAscentPropPressTank1Relay() { return AscentPropPressTank1Relay; }
	bool GetAscentPropPressTank2Relay() { return AscentPropPressTank2Relay; }
	bool GetAscentPropCompValvesRelay() { return AscentPropCompValvesRelay; }
	bool GetRCSPropPressRelay() { return RCSPropPressRelay; }
	void SetStagingRelay() { StagingRelay = true; }

	bool GetStageRelayMonitor();
	bool GetEDRelayMonitor();
protected:

	bool HasDCPower();

	//Relays:

	//Latching Relays

	//K1
	bool MasterArmRelay;
	//K2
	bool StagingRelay;

	//Non-latching Relays:

	//K3
	bool InitiateStagingRelay;
	//K4
	bool CableCuttingRelay;
	//K5
	bool StagingBoltsNutsRelay;
	//K6
	bool DeadFacingRelay;
	//K7
	bool RCSPropPressRelay;
	//K8
	bool LandingGearDeployRelay;
	//K9
	bool DescentEngineOnRelay;
	//K10
	bool AscentPropPressTank1Relay;
	//K11
	bool AscentPropPressTank2Relay;
	//K12
	bool AscentPropCompValvesRelay;
	//K13
	bool DescentPropVentRelay;
	//K14
	bool DescentPropPressRelay;
	//K15
	bool DescentTankIsolValvesRelay;

	//Delay Timers

	DelayTimer StagingBoltsNutsDelay;
	DelayTimer CableCuttingDelay;
	DelayTimer DescentEngineOnDelay;

	LEM *lem;
	CircuitBrakerSwitch *EDLogicPower;
	DCbus *EDDCBus;
	Battery *EDBattery;
};

// EXPLOSIVE DEVICES SYSTEM
class LEM_EDS {
public:
	LEM_EDS();							// Cons
	virtual ~LEM_EDS();                         // Des
	void Init(LEM *s); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void DefineAnimations(UINT idx);
	void DeleteAnimations();
	void Timestep(double simdt);
	bool GetHeliumPressDelayContactClosed() { return HeliumPressurizationDelayA.ContactClosed() || HeliumPressurizationDelayB.ContactClosed(); }
	bool GetLGDeployed() { return LG_Deployed; }

	LEM *lem;					// Pointer at LEM
	bool LG_Deployed;           // Landing Gear Deployed Flag
	bool Deadface;				// Deadface Flag

	LEM_EDRelayBox RelayBoxA;
	LEM_EDRelayBox RelayBoxB;

protected:
	DelayTimer HeliumPressurizationDelayA;
	DelayTimer HeliumPressurizationDelayB;

	AnimState2 gear_state;
	UINT anim_Gear;
	MGROUP_TRANSFORM	*mgt_Leg[4], *mgt_Strut[4], *mgt_Downlock[4], *mgt_Probes1[3], *mgt_Probes2[3];
};