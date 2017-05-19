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

// Ascent Engine
class LEM_APS {
public:
	LEM_APS();
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);

	LEM *lem;					// Pointer at LEM
	bool thrustOn;
	bool armedOn;
	double HePress[2];			// Helium pressure above and below the regulator

protected:

	DelayTimer AbortStageDelay;

	//Relays

	//Latching Relays

	//Manual Ascent Engine Control Latch (K19-1)
	bool K19;
	//Manual Ascent Engine Arm Latch (K22-1)
	bool K22;

	//Non-latching Relays

	//Manual Ascent Engine Arm (K11-1)
	bool K11;
	//Manual Ascent Engine On (K12-1)
	bool K12;
	//Ascent Engine Arm (K13-1)
	bool K13;
	//Ascent Engine On (K14-1)
	bool K14;
	//Abort Stage Relay (K21-1)
	bool K21;
	//Abort Stage Relay (K23-1)
	bool K23;
	//Auto Engine On Command (K24)
	bool K24;
	//Auto Engine Off Command (K25)
	bool K25;
	//Ascent Engine Arm Relay (K206-1)
	bool K206;
	//Auto Ascent Engine On Relay (K207-1)
	bool K207;

	bool AutoOn;
};