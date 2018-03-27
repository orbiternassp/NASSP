/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Attitude Control Assembly

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
#include "lm_aca.h"
#include "LEM.h"

LEM_ACA::LEM_ACA()
{
	ResetSwitches();
	deflection[0] = 0.0;
	deflection[1] = 0.0;
	deflection[2] = 0.0;
	PropVoltage[0] = 0.0;
	PropVoltage[1] = 0.0;
	PropVoltage[2] = 0.0;
	OutOfDetent = false;
}

void LEM_ACA::Init(LEM *l, ToggleSwitch *ACAPropSw) {
	lem = l;
	ACAPropSwitch = ACAPropSw;
}

void LEM_ACA::Timestep(int* aca_pos)
{
	int i;

	ResetSwitches();

	//Map to -13° to 13°
	for (i = 0;i < 3;i++)
	{
		deflection[i] = ((double)(aca_pos[i] - 32768))*26.0 / 65536.0;
	}

	//Yaw
	if (deflection[2] > 0.5)
	{
		S1 = true;
		if (deflection[2] > 2.5)
		{
			S5 = true;
			if (deflection[2] > 12.0)
			{
				S11 = true;
			}
		}
	}
	else if (deflection[2] < -0.5)
	{
		S1 = true;
		if (deflection[2] < -2.5)
		{
			S4 = true;
			if (deflection[2] < -12.0)
			{
				S10 = true;
			}
		}
	}

	//Pitch
	if (deflection[1] > 0.5)
	{
		S2 = true;
		if (deflection[1] > 2.5)
		{
			S7 = true;
			if (deflection[1] > 12.0)
			{
				S13 = true;
			}
		}
	}
	else if (deflection[1] < -0.5)
	{
		S2 = true;
		if (deflection[1] < -2.5)
		{
			S6 = true;
			if (deflection[1] < -12.0)
			{
				S12 = true;
			}
		}
	}

	//Roll
	if (deflection[0] > 0.5)
	{
		S3 = true;
		if (deflection[0] > 2.5)
		{
			S9 = true;
			if (deflection[0] > 12.0)
			{
				S15 = true;
			}
		}
	}
	else if (deflection[0] < -0.5)
	{
		S3 = true;
		if (deflection[0] < -2.5)
		{
			S8 = true;
			if (deflection[0] < -12.0)
			{
				S14 = true;
			}
		}
	}

	if (S1 || S2 || S3)
	{
		OutOfDetent = true;
	}
	else
	{
		OutOfDetent = false;
	}

	//1° to 11° proportional voltage

	double InputVoltage;

	if (ACAPropSwitch->IsUp())
	{
		if (lem->IMU_OPR_CB.IsPowered() && !lem->scca2.GetK5())	//PGNS Control
		{
			InputVoltage = 1.0;
		}
		else if (lem->SCS_ATCA_CB.IsPowered() && lem->scca2.GetK5())	//AGS Control
		{
			InputVoltage = 1.0;
		}
		else
		{
			InputVoltage = 0.0;
		}
	}
	else
	{
		InputVoltage = 0.0;
	}

	for (i = 0;i < 3;i++)
	{
		if (deflection[i] >= 0.0)
		{
			PropVoltage[i] = min(1.0, max(0.0, (deflection[i] - 1.0) / 10.0))*InputVoltage;
		}
		else
		{
			PropVoltage[i] = min(0.0, max(-1.0, (deflection[i] + 1.0) / 10.0))*InputVoltage;
		}
	}

	//sprintf(oapiDebugString(), "YAW: Input: %d, Deflect: %f, OOD: %d, Switches: %d %d %d %d", aca_pos[2], deflection[2], S1, S4, S5, S10, S11);
	//sprintf(oapiDebugString(), "PITCH: Input: %d, Deflect: %f, OOD: %d, Switches: %d %d %d %d", aca_pos[1], deflection[1], S2, S6, S7, S12, S13);
	//sprintf(oapiDebugString(), "ROLL: Input: %d, Deflect: %f, OOD: %d, Switches: %d %d %d %d", aca_pos[0], deflection[0], S3, S8, S9, S14, S15);
	//sprintf(oapiDebugString(), "PROP: %f %f %f", PropVoltage[2], PropVoltage[1], PropVoltage[0]);
}

void LEM_ACA::ResetSwitches()
{
	S1 = false;
	S2 = false;
	S3 = false;
	S4 = false;
	S5 = false;
	S6 = false;
	S7 = false;
	S8 = false;
	S9 = false;
	S10 = false;
	S11 = false;
	S12 = false;
	S13 = false;
	S14 = false;
	S15 = false;
}