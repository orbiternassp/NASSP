/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IVB Systems (Header)

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

class SIVBSystems
{
public:
	SIVBSystems(VESSEL *v, THRUSTER_HANDLE &j2, THRUSTER_HANDLE &lox, THGROUP_HANDLE &ver);
	void Timestep(double simdt);

	void LVDCEngineCutoff() { LVDCEngineStopRelay = true; }
	void LVDCEngineCutoffOff() { LVDCEngineStopRelay = false; }
	void EDSEngineCutoff(bool cut) { EDSEngineStop = cut; }
	void EngineReadyBypass() { EngineReady = true; }
	void EngineStartOn() { EngineStart = true; }
	void EngineStartOff() { EngineStart = true; }
	void FirstBurnRelayOn() { FirstBurnRelay = true; }
	void FirstBurnRelayOff() { FirstBurnRelay = false; }
	void SecondBurnRelayOn() { SecondBurnRelay = true; }
	void SecondBurnRelayOff() { SecondBurnRelay = false; }
	void EDSCutoffDisable() { EDSCutoffDisabled = true; }
	void StartLOXVenting() { LOXVentValveOpen = true; }
	void EndLOXVenting() { LOXVentValveOpen = false; }
	void FireUllageIgnitionOn() { FireUllageIgnition = true; }
	void SetThrusterDir(double beta_y, double beta_p);
	
	bool GetThrustOK() { return ThrustOKRelay; }

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
protected:
	bool FirstBurnRelay;
	bool SecondBurnRelay;
	bool EngineReady;

	//K100 and K101
	bool ThrustOKRelay;
	//K105
	bool AftPowerDisableRelay;
	//K112
	bool LVDCEngineStopRelay;

	bool RSSEngineStop;
	bool EDSEngineStop;
	bool PropellantDepletionSensors;

	bool EnginePower;
	bool EngineCutoffBus;
	bool EngineStart;
	bool EngineStop;

	bool EDSCutoffDisabled;

	double ThrustTimer;
	double ThrustLevel;

	bool LOXVentValveOpen;
	bool FireUllageIgnition;

	VESSEL *vessel;
	THRUSTER_HANDLE &j2engine;
	THRUSTER_HANDLE &loxvent;
	THGROUP_HANDLE &vernier;
};

#define SIVBSYSTEMS_START_STRING	"SIVBSYSTEMS_BEGIN"
#define SIVBSYSTEMS_END_STRING		"SIVBSYSTEMS_END"