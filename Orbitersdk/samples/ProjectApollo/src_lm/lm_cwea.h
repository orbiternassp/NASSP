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

class LEM;

class LEM_CWEA : public e_object {
public:
	LEM_CWEA(SoundLib &s, Sound &buttonsound);
	void Init(LEM *l, e_object *cwea, e_object *ma, e_object *ltg);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	bool IsCWEAPowered();
	bool IsMAPowered();
	bool IsLTGPowered();
	void TimeStep(double simdt);
	void SystemTimestep(double simdt);
	void SetMasterAlarm(bool alarm);
	void PushMasterAlarm();

	bool CheckMasterAlarmMouseClick(int event);
	void RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit, SURFHANDLE border);
	void RedrawLeft(SURFHANDLE sf, SURFHANDLE ssf);
	void RedrawRight(SURFHANDLE sf, SURFHANDLE ssf);

protected:
	void SetLight(int row, int column, int state, bool TriggerMA = true);
	void SetLightStates(int state);
	void SetColumnLightStates(int col, int state);
	void TurnOn();
	void TurnOff();

	int LightStatus[5][8];		// 0 = not lit, 1 = lit, 2 = light doesn't exist
	bool Operate;
	bool DesRegWarnFF;   // Flip Flop
	bool AGSWarnFF;   // Flip Flop
	bool CESDCWarnFF;   // Flip Flop
	bool CESACWarnFF;   // Flip Flop
	bool RCSCautFF1;   // Flip Flop
	bool RCSCautFF2;   // Flip Flop
	bool RRHeaterCautFF;   // Flip Flop
	bool SBDHeaterCautFF;   // Flip Flop
	bool OxygenCautFF1;   // Flip Flop
	bool OxygenCautFF2;   // Flip Flop
	bool OxygenCautFF3;   // Flip Flop
	bool WaterCautFF1;   // Flip Flop
	bool WaterCautFF2;   // Flip Flop
	bool WaterCautFF3;   // Flip Flop
	bool RRCautFF;   // Flip Flop
	bool SBDCautFF;   // Flip Flop
	bool MasterAlarm;
	bool AutoTrackChanged;

	e_object *cwea_pwr;
	e_object *ma_pwr;
	e_object *ltg_pwr;

	SoundLib &soundlib;
	Sound &ButtonSound;
	Sound MasterAlarmSound;

	LEM *lem;					// Pointer at LEM
};

#define CWEA_START_STRING   "CWEA_BEGIN"
#define CWEA_END_STRING     "CWEA_END"