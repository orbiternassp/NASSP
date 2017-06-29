/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Mission Programer

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
#include "LEM.h"
#include "lm_programer.h"
#include "papi.h"

LEM_Programer::LEM_Programer()
{

}

void LEM_Programer::Init(LEM *l)
{
	lem = l;
}

void LEM_Programer::ProcessChannel10(ChannelValue val)
{
	if (!lem->HasProgramer) return;

	ChannelValue10LMP out_val;

	out_val.Value = val.to_ulong();

	if (out_val.Bits.a == 15)
	{
		if (out_val.Bits.b == 22)	//22: Abort Stage Fire
		{
			lem->AbortStageSwitch.SetState(0);
		}
		else if (out_val.Bits.b == 23)	//134: Abort Stage Fire Reset
		{
			lem->AbortStageSwitch.SetState(1);
		}
		else if (out_val.Bits.b == 134)	//134: APS Arm
		{
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_UP);
		}
		else if (out_val.Bits.b == 135)	//135: APS Arm Reset
		{
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_CENTER);
		}
		else if (out_val.Bits.b == 150)	//150: DPS Arm
		{
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_DOWN);
		}
		else if (out_val.Bits.b == 151)	//151: DPS Reset
		{
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_CENTER);
		}
	}
}