/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Ascent Propulsion System (Header)

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

class APSValve {

public:
	APSValve();
	void SetState(bool open);
	bool IsOpen() { return isOpen; };
	void SwitchToggled(PanelSwitchItem *s);

protected:
	bool isOpen;
};

class APSPropellantSource : public LEMPropellantSource {
public:
	APSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p);

	void Timestep(double simt, double simdt);

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	double GetAscentHelium1PressPSI();
	double GetAscentHelium2PressPSI();
	double GetFuelTankUllagePressurePSI();
	double GetOxidizerTankUllagePressurePSI();
	double GetHeliumRegulator1OutletPressurePSI();
	double GetHeliumRegulator2OutletPressurePSI();
	double GetFuelTrimOrificeOutletPressurePSI() { return FuelTrimOrificeOutletPressurePSI; }
	double GetOxidTrimOrificeOutletPressurePSI() { return OxidTrimOrificeOutletPressurePSI; }
	bool GetFuelLowLevel() { return fuelLevelLow; }
	bool GetOxidLowLevel() { return oxidLevelLow; }
	double GetFuelTankBulkTempF();
	double GetOxidizerTankBulkTempF();

	APSValve *GetHeliumValve1() { return &PrimaryHeRegulatorShutoffValve; }
	APSValve *GetHeliumValve2() { return &SecondaryHeRegulatorShutoffValve; }
protected:

	double helium1PressurePSI;
	double helium2PressurePSI;
	double heliumRegulator1OutletPressurePSI;
	double heliumRegulator2OutletPressurePSI;
	double heliumRegulatorManifoldPressurePSI;
	double FuelTankUllagePressurePSI;
	double OxidTankUllagePressurePSI;
	double FuelTrimOrificeOutletPressurePSI;
	double OxidTrimOrificeOutletPressurePSI;

	bool fuelLevelLow;
	bool oxidLevelLow;

	APSValve PrimaryHeliumIsolationValve;
	APSValve RedundantHeliumIsolationValve;
	APSValve PrimaryHeRegulatorShutoffValve;
	APSValve SecondaryHeRegulatorShutoffValve;
	APSValve OxidCompatibilityValve;
	APSValve FuelCompatibilityValve;
};

// Ascent Engine
class LEM_APS {
public:
	LEM_APS();
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void Timestep(double simdt);

	double GetThrustChamberPressurePSI();

	LEM *lem;					// Pointer at LEM
	bool thrustOn;
	bool armedOn;
	double ChamberPressure;
};

#define APSPROPELLANT_START_STRING   "APSPROPELLANT_BEGIN"
#define APSPROPELLANT_END_STRING     "APSPROPELLANT_END"