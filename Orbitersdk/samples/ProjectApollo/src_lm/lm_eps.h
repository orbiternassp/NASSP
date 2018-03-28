/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module Electrical Power Subsystem (Header)

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

// XLunar Bus Controller Voltage Source
class LEM_XLBSource : public e_object {
public:
	LEM_XLBSource();							// Cons
	void SetVoltage(double v);
	void DrawPower(double watts);
};


// XLunar Bus Controller
class LEM_XLBControl : public e_object {
public:
	LEM_XLBControl();	// Cons
	void Init(LEM *s);
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	LEM *lem;					// Pointer at LEM
	LEM_XLBSource dc_output;	// DC output
};

// Electrical Control Assembly Subchannel
class LEM_ECAch : public e_object {
public:
	LEM_ECAch();								 // Cons
	void Init(LEM *s, e_object *src, int inp); // Init
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	LEM *lem;					// Pointer at LEM
	e_object *dc_source;		// Associated battery
	int input;                  // Channel input selector
};

// Bus feed controller object
class LEM_BusFeed : public e_object {
public:
	LEM_BusFeed();							// Cons
	void Init(LEM *s, e_object *sra, e_object *srb); // Init
	void UpdateFlow(double dt);
	void DrawPower(double watts);

	LEM *lem;					// Pointer at LEM
	e_object *dc_source_a;		// This has two inputs.
	e_object *dc_source_b;
};

// Voltage source item for cross-tie balancer
class LEM_BCTSource : public e_object {
public:
	LEM_BCTSource();							// Cons
	void SetVoltage(double v);
};

// Bus cross-tie balancer object
class LEM_BusCrossTie : public e_object {
public:
	LEM_BusCrossTie();	// Cons
	void LEM_BusCrossTie::Init(LEM *s, DCbus *sra, DCbus *srb, CircuitBrakerSwitch *cb1, CircuitBrakerSwitch *cb2, CircuitBrakerSwitch *cb3, CircuitBrakerSwitch *cb4);
	void UpdateFlow(double dt);
	void DrawPower(double watts);

	LEM *lem;					// Pointer at LEM
	DCbus *dc_bus_cdr;
	DCbus *dc_bus_lmp;
	LEM_BCTSource dc_output_cdr;
	LEM_BCTSource dc_output_lmp;
	CircuitBrakerSwitch *lmp_bus_cb, *lmp_bal_cb;
	CircuitBrakerSwitch *cdr_bus_cb, *cdr_bal_cb;
	double last_cdr_ld;
	double last_lmp_ld;
};

// Inverter
class LEM_INV : public e_object {
public:
	LEM_INV();							// Cons
	void Init(LEM *s, h_HeatLoad *invh, h_HeatLoad *secinvh);
	void DrawPower(double watts);
	void UpdateFlow(double dt);
	void SystemTimestep(double simdt);
	LEM *lem;					// Pointer at LM
	e_object *dc_input;
protected:
	double BASE_HLPW[19]; // Base heat loss per watt factors, calculated from LM-1 Systems Handbook
	double heatloss;
private:
	double get_hlpw(double base_hlpw_factor);
	double calc_hlpw_util(double maxw, int index);
	h_HeatLoad *InvHeat;
	h_HeatLoad *SecInvHeat;
};

class LEM_TLE
{
public:
	LEM_TLE();
	void Init(LEM *l, e_object *trk_cb, ThreePosSwitch *tracksw, h_HeatLoad *tleh, h_HeatLoad *sectleh);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);

	bool IsPowered();
protected:
	LEM * lem;
	e_object *TrackCB;
	ThreePosSwitch *TrackSwitch;
	h_HeatLoad *TLEHeat;
	h_HeatLoad *SecTLEHeat;

};

class LEM_DockLights
{
public:
	LEM_DockLights();
	void Init(LEM *l, e_object *cdr_cb, e_object *lmp_cb, ThreePosSwitch *docksw);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);

	bool IsPowered();
protected:
	LEM * lem;
	e_object *CDRDockCB;
	e_object *LMPDockCB;
	ThreePosSwitch *DockSwitch;

};