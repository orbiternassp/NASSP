/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Reaction Control System (Header)

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

#include "lm_dps.h"

class LEMRCSValve {

public:
	LEMRCSValve();
	void SetState(bool open);
	bool IsOpen() { return isOpen; };
	void SwitchToggled(PanelSwitchItem *s);

protected:
	bool isOpen;
};

class RCSPropellantSource : public LEMPropellantSource {
public:
	RCSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p);

	void Init(Pyro *rcshsp);
	void Timestep(double simt, double simdt);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void PrimInterconnectToggled(PanelSwitchItem *s);
	void SecInterconnectToggled(PanelSwitchItem *s);

	double GetRCSHeliumPressPSI();
	double GetRCSRegulatorPressPSI();
	double GetRCSFuelManifoldPressPSI();
	double GetRCSOxidManifoldPressPSI();
	double GetRCSPropellantQuantity();

	LEMRCSValve *GetMainShutoffValve() { return &mainShutoffValve; }
	LEMRCSValve *GetQuad1IsolationValve() { return &quad1IsolationValve; }
	LEMRCSValve *GetQuad2IsolationValve() { return &quad2IsolationValve; }
	LEMRCSValve *GetQuad3IsolationValve() { return &quad3IsolationValve; }
	LEMRCSValve *GetQuad4IsolationValve() { return &quad4IsolationValve; }
	LEMRCSValve *GetPrimOxidInterconnectValve() { return &primOxidInterconnectValve; }
	LEMRCSValve *GetSecOxidInterconnectValve() { return &secOxidInterconnectValve; }
	LEMRCSValve *GetPrimFuelInterconnectValve() { return &primFuelInterconnectValve; }
	LEMRCSValve *GetSecFuelInterconnectValve() { return &secFuelInterconnectValve; }
protected:
	void SetThrusters(PROPELLANT_HANDLE ph);

	double heliumPressurePSI;
	double regulatorPressurePSI;
	double oxidTankPressurePSI;
	double fuelTankPressurePSI;
	double oxidManifoldPressurePSI;
	double fuelManifoldPressurePSI;

	LEMRCSValve heliumSupplyValve;
	LEMRCSValve mainShutoffValve;
	LEMRCSValve quad1IsolationValve;
	LEMRCSValve quad2IsolationValve;
	LEMRCSValve quad3IsolationValve;
	LEMRCSValve quad4IsolationValve;
	LEMRCSValve primOxidInterconnectValve;
	LEMRCSValve secOxidInterconnectValve;
	LEMRCSValve primFuelInterconnectValve;
	LEMRCSValve secFuelInterconnectValve;

	Pyro *RCSHeliumSupplyPyros;
};

//Reaction Control System Thrust Control Assembly (RCS TCA)

class TCA_FlipFlop
{
public:
	TCA_FlipFlop();
	bool IsSet() { return State == true; }
	bool IsReset() { return State == false; }
	void Set() { State = true; }
	void Reset() { State = false; }
protected:
	bool State;
};

class RCS_TCA
{
public:
	RCS_TCA(int jdsa, int jdcirc1, int jdcirc2, int tcpsa1, int tcpcirc1, int tcps2, int tcpcirc2);
	void Init(LEM *l, int rsetcirc);
	void Timestep();
	TCA_FlipFlop *GetTCAFailureFlipFlop() { return &TCAFailure; }
	bool GetTCAFailure() { return TCAFailure.IsSet(); }
protected:
	bool voltageDiscreteDetector[2];
	bool thrusterTCP[2];
	int pulseCounter[2];
	bool resetSignal;

	//Flip-Flop
	TCA_FlipFlop TCAFailure;

	int jetDriverSA;
	int jetDriverCircuit[2];
	int thrustChamberPressureSA[2];
	int thrustChamberPressureCircuit[2];
	int resetChannel;

	LEM *lem;
};