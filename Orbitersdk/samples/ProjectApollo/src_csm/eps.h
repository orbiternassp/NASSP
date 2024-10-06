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