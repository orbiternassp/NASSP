/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Thrust/Translation Controller Assembly

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
#include "lm_ttca.h"
#include "LEM.h"

LEM_TTCA::LEM_TTCA()
{
	ResetSwitches();
	deflection[0] = 0.0;
	deflection[1] = 0.0;
	deflection[2] = 0.0;
	throttle = 0.0;
}

void LEM_TTCA::Init(LEM * l)
{
	lem = l;
}

void LEM_TTCA::Timestep(int* ttca_pos, int leverposition, bool realistic_throttle, double throttle_pos_dig)
{
	int i;

	ResetSwitches();

	if (leverposition < 32768 && realistic_throttle)
	{
		S7 = true;
	}
	else
	{
		S7 = false;
	}

	//Map to -9.5° to 9.5°
	for (i = 0;i < 3;i++)
	{
		deflection[i] = ((double)ttca_pos[i])*19.0 / 65536.0;
	}

	if (!S7)
	{
		if (deflection[0] > 4.75)
		{
			S2 = true;
		}
		else if (deflection[0] < -4.75)
		{
			S1 = true;
		}
	}

	if (deflection[1] > 4.75)
	{
		S3 = true;
	}
	else if (deflection[1] < -4.75)
	{
		S4 = true;
	}

	if (deflection[2] > 4.75)
	{
		S6 = true;
	}
	else if (deflection[2] < -4.75)
	{
		S5 = true;
	}

	if (lem->SCS_ATCA_CB.IsPowered())
	{
		if (realistic_throttle)
		{
			if (S7)
			{
				//Map from 0 to 1
				throttle = (32768.0 - (double)ttca_pos[0]) / 65536.0;
			}
			else
			{
				throttle = 0.0;
			}
		}
		else
		{
			throttle = throttle_pos_dig;
		}
	}
	else
	{
		throttle = 0.0;
	}

	//sprintf(oapiDebugString(), "%d %d %d %d %d %d %d", S1, S2, S3, S4, S5, S6, S7);
}

void LEM_TTCA::ResetSwitches()
{
	S1 = false;
	S2 = false;
	S3 = false;
	S4 = false;
	S5 = false;
	S6 = false;
	S7 = false;
}