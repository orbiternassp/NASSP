/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Q-Ball

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

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "qball.h"

QBall::QBall()
{
	IsPowered = true;
}

void QBall::Init(Saturn *vessel) {

	sat = vessel;
}

double QBall::GetAOA()
{
	if (sat->LETAttached() && sat->GetDynPressure() > 100.0 && IsPowered) {
		return sat->GetAOA();
	}
	else {
		return 0;
	}
}

void QBall::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "ISPOWERED", IsPowered);

	oapiWriteLine(scn, end_str);
}

void QBall::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "ISPOWERED", IsPowered);

	}
}