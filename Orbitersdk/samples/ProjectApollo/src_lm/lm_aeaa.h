/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2021

Lunar Module Ascent Engine Arming Assembly (Header)

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

#include "Orbitersdk.h"

class LEM_AEAA
{
public:
	LEM_AEAA();
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);

	void SetRelay(int set, int select, int doreset);

	bool GetArmingSignal();
	bool GetGuidanceSignal();
protected:
	bool K1;
	bool K2;
	bool K3;
	bool K4;
	bool K5;
	bool K6;
	bool K7;
	bool K8;
};