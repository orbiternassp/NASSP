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

#include "Orbitersdk.h"
#include "soundlib.h"
#include "toggleswitch.h"
#include "ORDEAL.h"
#include "papi.h"

//
// ORDEAL
//

ORDEAL::ORDEAL() {

	pitchOffset = 0;
}

void ORDEAL::Init(ToggleSwitch *EarthSw, CircuitBrakerSwitch *ACCB, CircuitBrakerSwitch *DCCB, OrdealRotationalSwitch *AltSet, ToggleSwitch *ModeSw, ThreePosSwitch *SlewSw, ToggleSwitch *FDAI1Sw, ToggleSwitch *FDAI2Sw) {
	EarthSwitch = EarthSw;
	ACCircuitBraker = ACCB;
	DCCircuitBraker = DCCB;
	AltSetRotary = AltSet;
	ModeSwitch = ModeSw;
	SlewSwitch = SlewSw;
	FDAI1Switch = FDAI1Sw;
	FDAI2Switch = FDAI2Sw;
}

bool ORDEAL::IsPowered() {

	// Do we have power?
	if (EarthSwitch->IsCenter()) return false;  // Switched off

														  // Ensure AC/DC power
	if (!ACCircuitBraker->IsPowered() ||
		!DCCircuitBraker->IsPowered()) return false;

	return true;
}

void ORDEAL::SystemTimestep(double simdt) {

	// Do we have power?
	if (!IsPowered()) return;

	ACCircuitBraker->DrawPower(4);	// see CSM Systems Handbook
	DCCircuitBraker->DrawPower(3);
}

void ORDEAL::Timestep(double simdt) {

	// Do we have power?
	if (!IsPowered()) return;

	// Calculate rate, see "Guidance and control systems - Orbital rate drive electronics for the Apollo command module and lunar module", NTRS ID 19740026211
	double rate = 0;
	if (EarthSwitch->IsUp()) {
		rate = 0.2 * RAD / (2.8182 + 0.001265 * AltSetRotary->GetValue());
	}
	else {
		rate = 0.2 * RAD / (3.5847 + 0.006342 * AltSetRotary->GetValue());
	}
	// sprintf(oapiDebugString(), "rate %f T %f", rate * DEG, 360. / (rate * DEG));

	if (ModeSwitch->IsDown()) {
		// Hold/Fast
		if (SlewSwitch->IsUp()) {
			pitchOffset += 256. * rate * simdt;
			while (pitchOffset >= TWO_PI) pitchOffset -= TWO_PI;

		}
		else if (SlewSwitch->IsDown()) {
			pitchOffset -= 256. * rate * simdt;
			while (pitchOffset < 0) pitchOffset += TWO_PI;
		}
	}
	else {
		// Apply rate
		pitchOffset += rate * simdt;
		while (pitchOffset >= TWO_PI) pitchOffset -= TWO_PI;

		// Slow slew
		if (SlewSwitch->IsUp()) {
			pitchOffset += 16. * rate * simdt;
			while (pitchOffset >= TWO_PI) pitchOffset -= TWO_PI;

		}
		else if (SlewSwitch->IsDown()) {
			pitchOffset -= 16. * rate * simdt;
			while (pitchOffset < 0) pitchOffset += TWO_PI;
		}
	}
}

double ORDEAL::GetFDAI1PitchAngle() {

	if (IsPowered() && FDAI1Switch->IsUp()) {
		return pitchOffset;
	}
	return 0;
}

double ORDEAL::GetFDAI2PitchAngle() {

	if (IsPowered() && FDAI2Switch->IsUp()) {
		return pitchOffset;
	}
	return 0;
}

void ORDEAL::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, ORDEAL_START_STRING);

	papiWriteScenario_double(scn, "PITCHOFFSET", pitchOffset);

	oapiWriteLine(scn, ORDEAL_END_STRING);
}

void ORDEAL::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, ORDEAL_END_STRING, sizeof(ORDEAL_END_STRING))) {
			return;
		}
		papiReadScenario_double(line, "PITCHOFFSET", pitchOffset);
	}
}
