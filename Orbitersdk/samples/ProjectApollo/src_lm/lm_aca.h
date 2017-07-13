/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Attitude Control Assembly (Header)

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
class ToggleSwitch;

class LEM_ACA
{
public:
	LEM_ACA();
	void Init(LEM *l, ToggleSwitch *ACAPropSw);
	void Timestep(int* aca_pos);
	bool GetOutOfDetent() { return OutOfDetent; }
	bool GetMinusYawBreakout() { return S4; }
	bool GetPlusYawBreakout() { return S5; }
	bool GetMinusPitchBreakout() { return S6; }
	bool GetPlusPitchBreakout() { return S7; }
	bool GetMinusRollBreakout() { return S8; }
	bool GetPlusRollBreakout() { return S9; }
	bool GetMinusYawHardover() { return S10; }
	bool GetPlusYawHardover() { return S11; }
	bool GetMinusPitchHardover() { return S12; }
	bool GetPlusPitchHardover() { return S13; }
	bool GetMinusRollHardover() { return S14; }
	bool GetPlusRollHardover() { return S15; }
	double GetACAProp(int i) { return PropVoltage[i]; }
private:
	void ResetSwitches();

	double deflection[3];
	bool OutOfDetent;
	double PropVoltage[3];

	//Yaw Out-of-Detent
	bool S1;
	//Pitch Out-of-Detent
	bool S2;
	//Roll Out-of-Detent
	bool S3;
	//-Y Breakout
	bool S4;
	//+Y Breakout
	bool S5;
	//-P Breakout
	bool S6;
	//+P Breakout
	bool S7;
	//-R Breakout
	bool S8;
	//+R Breakout
	bool S9;
	//-Y Hardover
	bool S10;
	//+Y Hardover
	bool S11;
	//-P Hardover
	bool S12;
	//+P Hardover
	bool S13;
	//-R Hardover
	bool S14;
	//+R Hardover
	bool S15;

	LEM *lem;
	ToggleSwitch *ACAPropSwitch;
};