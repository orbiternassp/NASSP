/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module Caution and Warning Electronics Assembly (Header)

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

#include "CWEA_FlipFlop.h"

class LEM;

class LEM_CWEA : public e_object {
public:
	LEM_CWEA(SoundLib &s, Sound &buttonsound);
	void Init(LEM *l, e_object *cwea, e_object *ma, h_HeatLoad *cweah, h_HeatLoad *seccweah);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	bool IsCWEAPowered();
	bool IsMAPowered();
	bool IsLTGPowered();
	bool IsCWPWRLTGPowered();
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	void SetMasterAlarm(bool alarm);
	void PushMasterAlarm();
	double GetCWBank1Lights();
	double GetCWBank2Lights();
	double GetCWBank3Lights();
	double GetCWBank4Lights();
	double GetNumberLightsOn();
	double GetNonDimmableLoad();
	double GetDimmableLoad();

	bool CheckMasterAlarmMouseClick(int event);
	void RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit, SURFHANDLE border);
	void RedrawLeft(SURFHANDLE sf, SURFHANDLE ssf);
	void RedrawRight(SURFHANDLE sf, SURFHANDLE ssf);

protected:
	void SetLight(int row, int column, int state, bool TriggerMA = true);
	void SetLightStates(int state);
	void TurnOn();
	void TurnOff();

	int LightStatus[5][8];		// 0 = not lit, 1 = lit, 2 = light doesn't exist
	bool Operate;
	CWEA_FlipFlop DesRegWarnFF;
	CWEA_FlipFlop AGSWarnFF;
	CWEA_FlipFlop CESDCWarnFF;
	CWEA_FlipFlop CESACWarnFF;
	CWEA_FlipFlop RCSCautFF1;
	CWEA_FlipFlop RCSCautFF2;
	CWEA_FlipFlop RRHeaterCautFF;
	CWEA_FlipFlop SBDHeaterCautFF;
	CWEA_FlipFlop QD1HeaterCautFF;
	CWEA_FlipFlop QD2HeaterCautFF;
	CWEA_FlipFlop QD3HeaterCautFF;
	CWEA_FlipFlop QD4HeaterCautFF;
	CWEA_FlipFlop OxygenCautFF1;
	CWEA_FlipFlop OxygenCautFF2;
	CWEA_FlipFlop OxygenCautFF3;
	CWEA_FlipFlop WaterCautFF1;
	CWEA_FlipFlop WaterCautFF2;
	CWEA_FlipFlop WaterCautFF3;
	CWEA_FlipFlop RRCautFF;
	CWEA_FlipFlop SBDCautFF;

	bool MasterAlarm;

	e_object *cwea_pwr;
	e_object *ma_pwr;
	h_HeatLoad *CWEAHeat;
	h_HeatLoad *SecCWEAHeat;

	SoundLib &soundlib;
	Sound &ButtonSound;
	Sound MasterAlarmSound;

	LEM *lem;					// Pointer at LEM
};

#define CWEA_START_STRING   "CWEA_BEGIN"
#define CWEA_END_STRING     "CWEA_END"