/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IB Systems (Header)

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

class H1Engine
{
public:
	H1Engine(VESSEL *v, THRUSTER_HANDLE &h1, bool cangimbal, double pcant, double ycant);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetEngineStart() { EngineStart = true; }
	void SetProgrammedEngineCutoff() { ProgrammedCutoff = true; }
	void SetEDSCutoff() { EDSCutoff = true; }
	void SetThrustNotOKCutoff() { ThrustNotOKCutoff = true; }
	void SetThrusterDir(double beta_y, double beta_p);
	void SetFailed() { EngineFailed = true; }

	bool GetThrustOK() { return ThrustOK; }
	double GetThrustLevel() { return ThrustLevel; }
	bool GetFailed() { return EngineFailed; }
protected:
	THRUSTER_HANDLE &th_h1;
	VESSEL *vessel;

	bool EngineStart;
	bool EngineStop;
	bool ProgrammedCutoff;
	bool EDSCutoff;
	bool GSECutoff;
	bool RSSCutoff;
	bool ThrustNotOKCutoff;

	bool ThrustOK;
	bool EngineRunning;

	bool EngineFailed;

	double ThrustTimer;
	double ThrustLevel;

	bool EngineCanGimbal;
	double pitchCant;
	double yawCant;
	const double GIMBALLIMIT = 8.0*RAD;
};


class SIBSystems
{
public:
	SIBSystems(Saturn *v, THRUSTER_HANDLE *h1, PROPELLANT_HANDLE &h1prop, Sound &LaunchS, Sound &SShutS, double &contraillvl);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	void SwitchSelector(int channel);
	void SetThrusterDir(int n, double beta_y, double beta_p);

	void SetSingleEngineCutoffEnable() { SingleEngineCutoffEnabledLatch = true; }
	void SetMultipleEngineCutoffEnable1() { MultipleEngineCutoffEnabledLatch1 = true; }
	void SetMultipleEngineCutoffEnable2() { MultipleEngineCutoffEnabledLatch2 = true; }
	void SetPropellantLevelSensorsEnable() { PropellantLevelSensorsEnabledLatch = true; }
	void SetInboardEnginesCutoff() { InboardEnginesCutoffLatch = true; }
	void SetOutboardEnginesCutoff() { OutboardEnginesCutoffLatch = true; }
	void SetLOXDepletionCutoffEnable() { LOXDepletionCutoffEnabledLatch = true; }
	void SetFuelDepletionCutoffEnable() { FuelDepletionCutoffEnabled = true; }

	bool LowLevelSensorsDry();
	bool GetInboardEngineOut();
	bool GetOutboardEngineOut();
protected:
	double GetSumThrust();

	Saturn *vessel;
	PROPELLANT_HANDLE &main_propellant;

	Sound &SShutSound;
	Sound &LaunchSound;
	double &contrailLevel;

	H1Engine h1engine1;
	H1Engine h1engine2;
	H1Engine h1engine3;
	H1Engine h1engine4;
	H1Engine h1engine5;
	H1Engine h1engine6;
	H1Engine h1engine7;
	H1Engine h1engine8;
	H1Engine *h1engines[8];

	//MAIN DISTRIBUTOR

	bool MultiEngineCutoffInhibitBusPowered;
	//K4
	bool Liftoff;
	//K6
	bool InboardEnginesCutoffRelay;
	//K7
	bool OutboardEnginesCutoffRelay;
	//K22
	bool PropellantLevelSensorsEnabledRelay;
	//K42
	bool MultipleEngineCutoffEnabledLatch1;
	//K44
	bool MultipleEngineCutoffEnabledLatch2;
	//K47
	bool PropellantLevelSensorsEnabledLatch;
	//K48
	bool InboardEnginesCutoffLatch;
	//K49
	bool LOXDepletionCutoffEnabledLatch;
	//K50
	bool OutboardEnginesCutoffLatch;
	//K53
	bool LOXDepletionCutoffEnabledRelay;
	//K58
	bool FuelDepletionCutoffEnabled;
	//K59
	bool SingleEngineCutoffEnabledLatch;
	//K75
	bool K75;
	//K76
	bool K76;
	//K77
	bool K77;

	//PRPULSION DISTRIBUTOR
	//K10
	bool SingleEngineFailedInhibitRelay;
	//K11-18
	bool EngineCutoffRelay[8];
	//K66
	bool MultiEngineCutoffInhibitRelay;
	//K67
	bool SingleEngineCutoffInhibitRelay;

	//Sensors
	bool FuelLevelSensor;
	bool LOXLevelSensor;
	bool FuelDepletionSensors;

	bool ThrustOK[8];

	bool FailInit;
	bool EarlySICutoff[8];
	double FirstStageFailureTime[8];
};