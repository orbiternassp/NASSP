/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Digital Command System

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
#include "papi.h"
#include "iu.h"
#include "dcs.h"

DCS::DCS(IU *i)
{
	iu = i;

	CommandSystemEnabled = false;
}

void DCS::LoadState(FILEHANDLE scn)

{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, DCS_END_STRING, sizeof(DCS_END_STRING)))
			return;

		papiReadScenario_bool(line, "COMMANDSYSTEMENABLED", CommandSystemEnabled);
	}
}

void DCS::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, DCS_START_STRING);

	papiWriteScenario_bool(scn, "COMMANDSYSTEMENABLED", CommandSystemEnabled);

	oapiWriteLine(scn, DCS_END_STRING);
}

bool DCS::Uplink(int type, void *upl)
{
	if (IsCommandSystemEnabled())
	{
		if (type == DCSUPLINK_SWITCH_SELECTOR)
		{
			DCSSWITSEL *switsel = static_cast<DCSSWITSEL*>(upl);

			return iu->lvda.GeneralizedSwitchSelector(switsel->stage, switsel->channel);
		}
		else if (type == DCSUPLINK_TIMEBASE_UPDATE)
		{
			DCSTBUPDATE *tbupdate = static_cast<DCSTBUPDATE*>(upl);

			return iu->lvda.TimebaseUpdate(tbupdate->dt);
		}
		else if (type == DCSUPLINK_LM_ABORT)
		{
			return iu->lvda.LMAbort();
		}
		else if (type == DCSUPLINK_RESTART_MANEUVER_ENABLE)
		{
			return iu->lvda.RestartManeuverEnable();
		}
		else if (type == DCSUPLINK_INHIBIT_MANEUVER)
		{
			return iu->lvda.InhibitAttitudeManeuver();
		}
		else if (type == DCSUPLINK_TIMEBASE_8_ENABLE)
		{
			return iu->lvda.Timebase8Enable();
		}
		else if (type == DCSUPLINK_EVASIVE_MANEUVER_ENABLE)
		{
			return iu->lvda.EvasiveManeuverEnable();
		}
		else if (type == DCSUPLINK_EXECUTE_COMM_MANEUVER)
		{
			return iu->lvda.ExecuteCommManeuver();
		}
		else if (type == DCSUPLINK_SIVBIU_LUNAR_IMPACT)
		{
			DCSLUNARIMPACT *lunarimpact = static_cast<DCSLUNARIMPACT*>(upl);

			return iu->lvda.SIVBIULunarImpact(lunarimpact->tig, lunarimpact->dt, lunarimpact->pitch, lunarimpact->yaw);
		}
		else if (type == DCSUPLINK_SATURNIB_LAUNCH_TARGETING)
		{
			DCSLAUNCHTARGET *targeting = static_cast<DCSLAUNCHTARGET*>(upl);

			return iu->lvda.LaunchTargetingUpdate(targeting->V_T, targeting->R_T, targeting->theta_T, targeting->i, targeting->lambda_0, targeting->lambda_dot, targeting->T_GRR0);
		}
	}

	return false;
}

bool DCS::IsCommandSystemEnabled()
{ 
	bool IsEnabled = CommandSystemEnabled || iu->GetCommandConnector()->GetIUUPTLMAccept();

	return IsEnabled;
}