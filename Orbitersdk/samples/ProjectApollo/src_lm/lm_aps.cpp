/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Ascent Propulsion System

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
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "dsky.h"
#include "IMU.h"
#include "LEM.h"
#include "papi.h"
#include "lm_aps.h"

// Ascent Propulsion System
LEM_APS::LEM_APS()
{
	lem = NULL;
	thrustOn = false;
	armedOn = false;
	HePress[0] = 0; HePress[1] = 0;
}

void LEM_APS::Init(LEM *s) {
	lem = s;
}

void LEM_APS::TimeStep(double simdt) {
	if (lem == NULL) { return; }

	if (lem->stage > 1)
	{
		if (armedOn && lem->GetThrusterResource(lem->th_hover[0]) == NULL)
		{
			lem->SetThrusterResource(lem->th_hover[0], lem->ph_Asc);
			lem->SetThrusterResource(lem->th_hover[1], lem->ph_Asc);
		}
		else if (lem->GetThrusterResource(lem->th_hover[0]) != NULL)
		{
			lem->SetThrusterResource(lem->th_hover[0], NULL);
			lem->SetThrusterResource(lem->th_hover[1], NULL);
		}


		if (thrustOn && armedOn)
		{
			lem->SetThrusterLevel(lem->th_hover[0], 1.0);
			lem->SetThrusterLevel(lem->th_hover[1], 1.0);
		}
		else
		{
			lem->SetThrusterLevel(lem->th_hover[0], 0.0);
			lem->SetThrusterLevel(lem->th_hover[1], 0.0);
		}
	}
}

void LEM_APS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "ARMEDON", armedOn);
	papiWriteScenario_bool(scn, "THRUSTON", thrustOn);

	oapiWriteLine(scn, end_str);
}

void LEM_APS::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "ARMEDON", armedOn);
		papiReadScenario_bool(line, "THRUSTON", thrustOn);

	}
}