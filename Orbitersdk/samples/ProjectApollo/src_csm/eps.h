/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2024

Command Module Electrical Power Subsystem (Header)

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

class Saturn;

/// This class simulates the cryo pressure switches and controls for cryo fans and heaters in the CSM.
class CryoPressureSwitch
{
public:
	CryoPressureSwitch();
	virtual ~CryoPressureSwitch();
	void Init(Saturn *s, h_Tank *tnk1, h_Tank *tnk2, Boiler *htr1, Boiler *htr2, Boiler *fn1, Boiler *fn2,
		ThreePosSwitch *htrsw1, ThreePosSwitch *htrsw2, ThreePosSwitch *fnsw1, ThreePosSwitch *fnsw2, 
		DCBusController* dcacont, DCBusController* dcbcont,
		double lp, double hp);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn, char *name_str);
	void LoadState(char *line, int strlen);

protected:
	Saturn *saturn;
	h_Tank *tank1, *tank2;
	ThreePosSwitch *htrswitch1, *htrswitch2, *fanswitch1, *fanswitch2;
	Boiler *heater1, *heater2, *fan1, *fan2;
	DCBusController *dcacontrol, *dcbcontrol;
	bool PressureSwitch1;
	bool PressureSwitch2;
	double lowpress;
	double highpress;
};

/// This class simulates flood lighting behavior in the CSM
class FloodLights
{
public:
	FloodLights();
	void FloodLights::Init(Saturn *s, e_object *flood_mna, e_object *flood_mnb, e_object *flood_pl,
		ToggleSwitch *pnl8_dim, ThreePosSwitch *pnl8_fixed, ContinuousRotationalSwitch *pnl8_rty,
		ToggleSwitch *pnl5_dim, ToggleSwitch *pnl5_fixed, ContinuousRotationalSwitch *pnl5_rty,
		ToggleSwitch *pnl100_dim, ToggleSwitch *pnl100_fixed, ContinuousRotationalSwitch *pnl100_rty);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);

	double GetLHPrimVoltage();
	double GetLHSecVoltage();
	double GetRHPrimVoltage();
	double GetRHSecVoltage();
	double GetLEBPrimVoltage();
	double GetLEBSecVoltage();
	//bool IsPowered();
	//bool IsHatchOpen();
	//double GetLMPRotaryVoltage();
	//double GetCDRRotaryVoltage();
	//double GetALLPowerDraw();
	//double GetOVHDFWDPowerDraw();
	//double GetPowerDraw();
protected:
	Saturn *saturn;
	e_object *FloodMNAcb;
	e_object *FloodMNBcb;
	e_object *FloodPLcb;
	ToggleSwitch *PNL8_DIMsw;
	ThreePosSwitch *PNL8_FIXEDsw;
	ContinuousRotationalSwitch *PNL8_Rotary;
	ToggleSwitch *PNL5_DIMsw;
	ToggleSwitch *PNL5_FIXEDsw;
	ContinuousRotationalSwitch *PNL5_Rotary;
	ToggleSwitch *PNL100_DIMsw;
	ToggleSwitch *PNL100_FIXEDsw;
	ContinuousRotationalSwitch *PNL100_Rotary;
};
