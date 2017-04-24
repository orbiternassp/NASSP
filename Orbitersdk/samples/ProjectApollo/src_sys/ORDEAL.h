/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Orbital Rate Display Earth And Lunar

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

#if !defined(_PA_ORDEAL_H)
#define _PA_ORDEAL_H

// ORDEAL

#define ORDEAL_START_STRING		"ORDEAL_BEGIN"
#define ORDEAL_END_STRING		"ORDEAL_END"

class ORDEAL {

public:
	ORDEAL();
	void Init(ToggleSwitch *EarthSw, CircuitBrakerSwitch *ACCB, CircuitBrakerSwitch *DCCB, OrdealRotationalSwitch *AltSet, ToggleSwitch *ModeSw, ThreePosSwitch *SlewSw, ToggleSwitch *FDAI1Sw, ToggleSwitch *FDAI2Sw);		// Initialization
	void Timestep(double simdt);                                    // Timestep
	void SystemTimestep(double simdt);

	double GetFDAI1PitchAngle();
	double GetFDAI2PitchAngle();

	void SaveState(FILEHANDLE scn);                                // SaveState callback
	void LoadState(FILEHANDLE scn);                                // LoadState callback

private:
	bool IsPowered();

	double pitchOffset;

	ToggleSwitch *EarthSwitch;
	CircuitBrakerSwitch *ACCircuitBraker;
	CircuitBrakerSwitch *DCCircuitBraker;
	OrdealRotationalSwitch *AltSetRotary;
	ToggleSwitch *ModeSwitch;
	ThreePosSwitch *SlewSwitch;
	ToggleSwitch *FDAI1Switch;
	ToggleSwitch *FDAI2Switch;
};

#endif