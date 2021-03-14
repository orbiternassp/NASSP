/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2021

Lunar Module Ascent Engine Arming Assembly

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

#include "lm_aeaa.h"

LEM_AEAA::LEM_AEAA()
{
	K1 = false;
	K2 = false;
	K3 = false;
	K4 = false;
	K5 = false;
	K6 = false;
	K7 = false;
	K8 = false;
}

void LEM_AEAA::SaveState(FILEHANDLE scn)
{
	char buffer[1000];

	sprintf(buffer, "%d %d %d %d %d %d %d %d", K1, K2, K3, K4, K5, K6, K7, K8);
	oapiWriteScenario_string(scn, "AscEngArmAssy", buffer);
}

void LEM_AEAA::LoadState(char *line)
{
	int rel[8];
	sscanf(line + 14, "%d %d %d %d %d %d %d %d", &rel[0], &rel[1], &rel[2], &rel[3], &rel[4], &rel[5], &rel[6], &rel[7]);
	K1 = (rel[0] != 0);
	K2 = (rel[1] != 0);
	K3 = (rel[2] != 0);
	K4 = (rel[3] != 0);
	K5 = (rel[4] != 0);
	K6 = (rel[5] != 0);
	K7 = (rel[6] != 0);
	K8 = (rel[7] != 0);
}

bool LEM_AEAA::GetArmingSignal()
{
	if ((K1 || K2) && (K3 || K4))
	{
		return true;
	}
	return false;
}

bool LEM_AEAA::GetGuidanceSignal()
{
	if ((K5 || K6) && (K7 || K8))
	{
		return true;
	}
	return false;
}

void LEM_AEAA::SetRelay(int set, int select, int doreset)
{
	if (set == 1)
	{
		if (select == 1)
		{
			if (doreset)
			{
				K1 = false;
			}
			else
			{
				K1 = true;
			}
		}
		else if (select == 2)
		{
			if (doreset)
			{
				K3 = false;
			}
			else
			{
				K3 = true;
			}
		}
		else if (select == 3)
		{
			if (doreset)
			{
				K5 = false;
			}
			else
			{
				K5 = true;
			}
		}
		else
		{
			if (doreset)
			{
				K7 = false;
			}
			else
			{
				K7 = true;
			}
		}
	}
	else
	{
		if (select == 1)
		{
			if (doreset)
			{
				K4 = false;
			}
			else
			{
				K4 = true;
			}
		}
		else if (select == 2)
		{
			if (doreset)
			{
				K2 = false;
			}
			else
			{
				K2 = true;
			}
		}
		else if (select == 3)
		{
			if (doreset)
			{
				K8 = false;
			}
			else
			{
				K8 = true;
			}
		}
		else
		{
			if (doreset)
			{
				K6 = false;
			}
			else
			{
				K6 = true;
			}
		}
	}
}