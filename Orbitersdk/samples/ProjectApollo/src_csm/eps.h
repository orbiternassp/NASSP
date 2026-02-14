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
	virtual ~FloodLights();
	void FloodLights::Init(Saturn *s, e_object *flood_rty_src, e_object *fixed, ToggleSwitch *dim, ContinuousRotationalSwitch *rty);
	double GetPrimVoltage();
	double GetSecVoltage();
	double GetPrimOutput();
	double GetSecOutput();
	double GetCombinedOutput();
	void SystemTimestep(double simdt);

protected:
	Saturn *saturn;
	e_object *FloodRtycb;
	e_object *FIXEDsw;
	ToggleSwitch *DIMsw;
	ContinuousRotationalSwitch *Rotary;
};

/// This class simulates tunnel lighting behavior in the CSM
class TunnelLights
{
public:
	TunnelLights();
	virtual ~TunnelLights();
	void TunnelLights::Init(Saturn *s, e_object *cb, ToggleSwitch *lt_sw);
	double GetOutput();
	void SystemTimestep(double simdt);

protected:
	Saturn *saturn;
	e_object *MNcb;
	ToggleSwitch *TunnelLtsw;
};

/// This class simulates integral lighting behavior in the CSM
class IntegralLights
{
public:
	IntegralLights(double watts);
	virtual ~IntegralLights();
	void IntegralLights::Init(Saturn *s, e_object *cb, ContinuousRotationalSwitch *rty);
	double GetOutput();
	void SystemTimestep(double simdt);

protected:
	Saturn *saturn;
	e_object *Integralcb;
	ContinuousRotationalSwitch *Rotary;
	double powerdraw;
};

/// This class simulates numeric lighting behavior in the CSM
class NumericLights
{
public:
	NumericLights();
	virtual ~NumericLights();
	void NumericLights::Init(Saturn *s, e_object *cb, ContinuousRotationalSwitch *rty);
	double GetOutput();
	void SystemTimestep(double simdt);

protected:
	Saturn *saturn;
	e_object *Numericscb;
	ContinuousRotationalSwitch *Rotary;
};

/// This class simulates exterior lighting behavior in the CSM
class ExteriorLighting
{
public:
	ExteriorLighting();
	virtual ~ExteriorLighting();
	void Init(Saturn *s, CircuitBrakerSwitch *RDVMNB, ThreeSourceTwoDestSwitch *RDZSPOT, PowerMerge *AC, ToggleSwitch *RUNEVA, ElectricLight *EVALT);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn, char *name_str);
	void LoadState(char *line, int strlen);
	bool IsRunEVAOn();
	void DefineAnimations(UINT idx);

protected:
	Saturn *saturn;
	CircuitBrakerSwitch *RNDZSPOTMNBcb;
	ThreeSourceTwoDestSwitch *RDZSPOTsw;
	PowerMerge *ACPower;
	ToggleSwitch *RUNEVAsw;
	ElectricLight *EVALight;
	bool SpotDeployed;
	bool EVALtDeployed;
	UINT anim_EVALt;
};