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
#include "lm_aps.h"

// Ascent Propulsion System
LEM_APS::LEM_APS() :
	AbortStageDelay(0.4)
{
	lem = NULL;
	thrustOn = false;
	armedOn = false;
	HePress[0] = 0; HePress[1] = 0;

	K11 = false;
	K12 = false;
	K13 = false;
	K14 = false;
	K19 = false;
	K21 = false;
	K22 = false;
	K23 = false;
	K24 = false;
	K25 = false;
	K206 = false;
	K207 = false;
	AutoOn = false;
}

void LEM_APS::Init(LEM *s) {
	lem = s;
}

void LEM_APS::TimeStep(double simdt) {
	if (lem == NULL) { return; }

	AbortStageDelay.Timestep(simdt);

	//Abort Stage Handling

	if ((lem->CDR_SCS_ABORT_STAGE_CB.IsPowered() || lem->SCS_ABORT_STAGE_CB.IsPowered()) && lem->AbortStageSwitch.GetState() == 0)
	{
		K21 = true;
		AbortStageDelay.SetRunning(true);
		if (AbortStageDelay.ContactClosed())
		{
			K23 = true;
		}
		else
		{
			K23 = false;
		}
	}
	else
	{
		K21 = false;
		K23 = false;
	}

	//Automatic

	if (lem->GuidContSwitch.IsUp()) {
		//PGNS
		// Check i/o channel
		ChannelValue val11;
		val11 = lem->agc.GetOutputChannel(011);

		if (val11[EngineOn] && lem->SCS_ENG_ARM_CB.IsPowered())
		{
			K24 = true;
		}
		else
		{
			K24 = false;
		}

		if (val11[EngineOff] && lem->SCS_ENG_ARM_CB.IsPowered())
		{
			K25 = true;
		}
		else
		{
			K25 = false;
		}
	}
	else
	{
		K24 = false;
		K25 = false;
	}

	//Ascent Engine Logic Circuit
	if ((K24 && !K25) || ((K22 || K23) && (K24 && K25 || !K24 && !K25)))
	{
		AutoOn = true;
	}
	else
	{
		AutoOn = false;
	}

	//Manual

	if ((lem->SCS_ENG_START_OVRD_CB.IsPowered() || lem->PROP_DISP_ENG_OVRD_LOGIC_CB.IsPowered()) && lem->ManualEngineStart.GetState() == 1)
	{
		K19 = true;
		K22 = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->ManualEngineStop.GetState() == 1)
	{
		K19 = false;
		K22 = false;
	}

	if (lem->EngineArmSwitch.IsUp() && (lem->SCS_ENG_START_OVRD_CB.IsPowered() || lem->PROP_DISP_ENG_OVRD_LOGIC_CB.IsPowered()))
	{
		K22 = true;
	}

	//Manual Ascent Engine Arm
	if (K19 && lem->EngineArmSwitch.IsUp() && (lem->SCS_ENG_START_OVRD_CB.IsPowered() || lem->PROP_DISP_ENG_OVRD_LOGIC_CB.IsPowered()))
	{
		K11 = true;
		K12 = true;
	}
	else
	{
		K11 = false;
		K12 = false;
	}

	if (lem->EngineArmSwitch.IsUp() && (lem->SCS_ENG_ARM_CB.IsPowered() || (lem->AbortSwitch.GetState() == 1 && lem->SCS_ENG_CONT_CB.IsPowered())))
	{
		K206 = true;
	}
	else
	{
		K206 = false;
	}

	if (K206 && K22 && lem->EngineArmSwitch.IsUp() && (lem->SCS_ENG_ARM_CB.IsPowered() || (lem->AbortSwitch.GetState() == 1 && lem->SCS_ENG_CONT_CB.IsPowered())))
	{
		K13 = true;
	}
	else if (!K206 && K22 && K23 && lem->AbortStageSwitch.GetState() == 0 && (lem->CDR_SCS_ABORT_STAGE_CB.IsPowered() || lem->SCS_ABORT_STAGE_CB.IsPowered()))
	{
		K13 = true;
	}
	else
	{
		K13 = false;
	}

	if (AutoOn && lem->ManualEngineStop.GetState() == 0 && lem->EngineArmSwitch.IsUp())
	{
		K14 = true;
	}
	else if (AutoOn && lem->ManualEngineStop.GetState() == 0 && K21)
	{
		K14 = true;
	}
	else
	{
		K14 = false;
	}

	if (K14 && lem->CDR_SCS_AELD_CB.IsPowered())
	{
		K207 = true;
	}
	else if (K14 && (lem->SCS_ENG_ARM_CB.IsPowered() || (lem->AbortSwitch.GetState() == 1 && lem->SCS_ENG_CONT_CB.IsPowered())))
	{
		K207 = true;
	}
	else
	{
		K207 = false;
	}

	if (lem->SCS_AELD_CB.IsPowered() && ((K11 && K12) || (K13 && K207)))
	{
		lem->eds.RelayBoxB.SetStagingRelay();
		thrustOn = true;
	}
	else if (lem->CDR_SCS_AELD_CB.IsPowered() && ((K11 && K207) || (K13 && K12)))
	{
		lem->eds.RelayBoxB.SetStagingRelay();
		thrustOn = true;
	}
	else
	{
		thrustOn = false;
	}

	if (lem->SCS_AELD_CB.IsPowered() && ((K11 && K207) || (K13 && K12)))
	{
		lem->eds.RelayBoxA.SetStagingRelay();
		armedOn = true;
	}
	else if (lem->CDR_SCS_AELD_CB.IsPowered() && ((K11 && K12) || (K13 && K207)))
	{
		lem->eds.RelayBoxA.SetStagingRelay();
		armedOn = true;
	}
	else
	{
		armedOn = false;
	}

	if (lem->stage > 1)
	{
		if (armedOn)
		{
			lem->SetThrusterResource(lem->th_hover[0], lem->ph_Asc);
			lem->SetThrusterResource(lem->th_hover[1], lem->ph_Asc);
		}
		else
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
	//sprintf(oapiDebugString(), "Manual: K19 %d K22 %d K11 %d K12 %d K13 %d K14 %d", K19, K22, K11, K12, K13, K14);
	//sprintf(oapiDebugString(), "Auto: K13 %d K207 %d K206 %d K14 %d K24 %d K25 %d AutoOn %d", K13, K207, K206, K14, K24, K25, AutoOn);
	//sprintf(oapiDebugString(), "Abort: K21 %d K23 %d K13 %d K14 %d AutoOn %d armedOn %d thrustOn %d", K21, K23, K13, K14, AutoOn, armedOn, thrustOn);
}

void LEM_APS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

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


	}
}