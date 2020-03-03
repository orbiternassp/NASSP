/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IC Systems (Header)

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

class F1Engine
{
public:
	F1Engine(VESSEL *v, THRUSTER_HANDLE &f1);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetEngineStart() { EngineStart = true; }
	void SetProgrammedEngineCutoff() { ProgrammedCutoff = true; }
	void SetEDSCutoff() { EDSCutoff = true; }
	void SetGSECutoff() { GSECutoff = true; }
	void SetThrusterDir(double beta_y, double beta_p);
	void SetFailed() { EngineFailed = true; }

	bool GetThrustOK() { return ThrustOK; }
	double GetThrustLevel() { return ThrustLevel; }
	bool GetFailed() { return EngineFailed; }
	bool GetEngineStop() { return EngineStop; }
protected:

	void ServoDrive(double &Angle, double AngleCmd, double RateLimit, double simdt);

	THRUSTER_HANDLE &th_f1;
	VESSEL *vessel;

	bool EngineStart;
	bool EngineStop;
	bool ProgrammedCutoff;
	bool EDSCutoff;
	bool GSECutoff;
	bool RSSCutoff;

	bool ThrustOK;
	bool EngineRunning;

	bool EngineFailed;

	double ThrustTimer;
	double ThrustLevel;

	double pitchCmd;
	double pitchPos;
	double yawCmd;
	double yawPos;
};

class TSMUmbilical;
class Pyro;
class Sound;

class SICSystems
{
public:
	SICSystems(VESSEL *v, THRUSTER_HANDLE *f1, PROPELLANT_HANDLE &f1prop, Pyro &SIC_SII_Sep, Sound &LaunchS, Sound &SShutS, double &contraillvl);
	~SICSystems();
	void Timestep(double misst, double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	virtual void SetEngineStart(int n);
	void InboardEngineCutoff();
	void OutboardEnginesCutoff();
	void OutboardEnginesCutoffEnable() { PointLevelSensorArmed = true; }
	void OutboardEnginesCutoffDisable() { PointLevelSensorArmed = false; }
	void TwoAdjacentOutboardEnginesOutCutoffEnable() { TwoAdjacentOutboardEnginesOutCutoff = true; }
	void MultipleEngineCutoffEnable() { MultipleEngineCutoffEnabled = true; }
	void EDSEnginesCutoff(bool cut);
	virtual void GSEEnginesCutoff(bool cut);
	void SetThrusterDir(int n, double beta_y, double beta_p);
	void SwitchSelector(int channel);

	void SetEngineFailureParameters(bool *SICut, double *SICutTimes);
	void SetEngineFailureParameters(int n, double SICutTimes, bool fail);
	void GetEngineFailureParameters(int n, bool &fail, double &failtime);

	bool PropellantLowLevel();
	void GetThrustOK(bool *ok);
	bool GetPropellantDepletionEngineCutoff();
	bool GetInboardEngineOut();
	bool GetOutboardEngineOut();

	virtual bool GetEngineStop();

	virtual void ConnectUmbilical(TSMUmbilical *umb);
	virtual void DisconnectUmbilical();
	bool IsUmbilicalConnected();
protected:

	double GetSumThrust();
	bool ESEGetSICThrustOKSimulate(int eng, int n);

	VESSEL *vessel;
	PROPELLANT_HANDLE &main_propellant;

	Pyro &SIC_SII_Separation_Pyros;

	Sound &SShutSound;
	Sound &LaunchSound;
	double &contrailLevel;

	F1Engine f1engine1;
	F1Engine f1engine2;
	F1Engine f1engine3;
	F1Engine f1engine4;
	F1Engine f1engine5;
	F1Engine *f1engines[5];

	bool PropellantDepletionSensors;
	//K1
	bool TwoAdjacentOutboardEnginesOutCutoff;
	//K2
	bool MultipleEngineCutoffEnabled;
	//K3
	bool PointLevelSensorArmed;

	bool ThrustOK[15];

	bool EarlySICutoff[5];
	double FirstStageFailureTime[5];

	TSMUmbilical *TSMUmb;
};