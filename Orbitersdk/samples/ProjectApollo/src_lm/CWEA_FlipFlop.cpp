/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module CWEA Flip-Flop

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
#include "CWEA_FlipFlop.h"

CWEA_FlipFlop::CWEA_FlipFlop()
{
	state = false;
	S = false;
	R = false;
}

void CWEA_FlipFlop::Set(bool IsSet)
{
	if (IsSet)
	{
		if (!S) state = true;
		S = true;
	}
	else
	{
		S = false;
	}
}

void CWEA_FlipFlop::Reset(bool IsReset)
{
	if (IsReset)
	{
		if (!R) state = false;
		R = true;
	}
	else
	{
		R = false;
	}
}

void CWEA_FlipFlop::HardReset()
{
	state = false;
	S = false;
	R = false;
}

void CWEA_FlipFlop::SaveState(FILEHANDLE scn, char *str)
{
	char buffer[100];

	sprintf(buffer, "%d %d %d", state, S, R);
	oapiWriteScenario_string(scn, str, buffer);
}

void CWEA_FlipFlop::LoadState(char *line, int len)
{
	int i, j, k;

	sscanf(line + len, "%i %i %i", &i, &j, &k);

	state = (i != 0);
	S = (j != 0);
	R = (k != 0);
}