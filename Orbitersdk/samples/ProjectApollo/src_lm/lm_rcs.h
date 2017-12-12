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

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	double GetRCSHeliumPressPSI();
	double GetRCSRegulatorPressPSI();
	double GetRCSFuelManifoldPressPSI();
	double GetRCSOxidManifoldPressPSI();

	LEMRCSValve *GetMainShutoffValve() { return &MainShutoffValve; }
	LEMRCSValve *GetQuad1IsolationValve() { return &quad1IsolationValve; }
	LEMRCSValve *GetQuad2IsolationValve() { return &quad2IsolationValve; }
	LEMRCSValve *GetQuad3IsolationValve() { return &quad3IsolationValve; }
	LEMRCSValve *GetQuad4IsolationValve() { return &quad4IsolationValve; }
protected:
	void SetThrusters(PROPELLANT_HANDLE ph);

	double heliumPressurePSI;
	double regulatorPressurePSI;
	double oxidTankPressurePSI;
	double fuelTankPressurePSI;
	double oxidManifoldPressurePSI;
	double fuelManifoldPressurePSI;

	LEMRCSValve HeliumSupplyValve;
	LEMRCSValve MainShutoffValve;
	LEMRCSValve quad1IsolationValve;
	LEMRCSValve quad2IsolationValve;
	LEMRCSValve quad3IsolationValve;
	LEMRCSValve quad4IsolationValve;

	Pyro *RCSHeliumSupplyPyros;
};