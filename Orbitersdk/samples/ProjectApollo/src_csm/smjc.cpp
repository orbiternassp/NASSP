/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

Service Module Jettison Controller

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
#include "smjc.h"

SMJC::SMJC(double td2, double td3) : TD2(td2), TD3(td3)
{
	Z1 = false;
	Z2 = false;
	Z3 = false;
	FireMinusXTrans = false;
	FirePlusRoll = false;
}

void SMJC::Timestep(double simdt, bool smjettbuspowered)
{
	TD2.Timestep(simdt);
	TD3.Timestep(simdt);

	if (smjettbuspowered && Z1)
	{
		FireMinusXTrans = true;
		TD2.SetRunning(true);
	}
	else
		FireMinusXTrans = false;

	if (TD2.ContactClosed())
		Z2 = true;
	else
		Z2 = false;

	if (smjettbuspowered && Z1 && Z2)
		TD3.SetRunning(true);

	if (smjettbuspowered && Z1 && Z2 && (TD3.ContactClosed() || Z3))
		Z3 = true;
	else
		Z3 = false;

	if (smjettbuspowered && Z1 && Z2 && !Z3)
		FirePlusRoll = true;
	else
		FirePlusRoll = false;
}

void SMJC::SMJettControllerStart()
{
	Z1 = true;
}

void SMJC::GSEReset()
{
	Z1 = false;
}

void SMJC::SaveState(FILEHANDLE scn, char *start_str)
{
	oapiWriteLine(scn, start_str);

	char buffer[256];

	sprintf(buffer, "%i %i %i", (Z1 ? 1 : 0), (Z2 ? 1 : 0), (Z3 ? 1 : 0));
	oapiWriteScenario_string(scn, "RELAYS", buffer);
	TD2.SaveState(scn, "TD2_BEGIN", "TD_END");
	TD3.SaveState(scn, "TD3_BEGIN", "TD_END");

	oapiWriteLine(scn, SMJC_END_STRING);
}

void SMJC::LoadState(FILEHANDLE scn)
{
	char *line;
	int i, j, k;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SMJC_END_STRING, sizeof(SMJC_END_STRING)))
			return;

		if (!strnicmp(line, "RELAYS", 6)) {
			sscanf(line + 6, "%i %i %i", &i, &j, &k);
			Z1 = (i != 0);
			Z2 = (j != 0);
			Z3 = (k != 0);
		}
		else if (!strnicmp(line, "TD2_BEGIN", sizeof("TD2_BEGIN"))) {
			TD2.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD3_BEGIN", sizeof("TD3_BEGIN"))) {
			TD3.LoadState(scn, "TD_END");
		}
	}
}

void SMJC::SetState(const SMJCState &state)
{
	Z1 = state.Z1;
	Z2 = state.Z2;
	Z3 = state.Z3;
	FireMinusXTrans = state.FireMinusXTrans;
	FirePlusRoll = state.FirePlusRoll;
	TD2.SetState(state.TD2State);
	TD3.SetState(state.TD3State);
}

void SMJC::GetState(SMJCState &state)
{
	state.Z1 = Z1;
	state.Z2 = Z2;
	state.Z3 = Z3;
	state.FireMinusXTrans = FireMinusXTrans;
	state.FirePlusRoll = FirePlusRoll;
	TD2.GetState(state.TD2State);
	TD3.GetState(state.TD3State);
}

SMJC_MOD1::SMJC_MOD1() : SMJC(2.0, 2.0) , TD4(25.0)
{
	Z4 = false;
}

void SMJC_MOD1::Timestep(double simdt, bool smjettbuspowered)
{
	TD2.Timestep(simdt);
	TD3.Timestep(simdt);
	TD4.Timestep(simdt);

	if (smjettbuspowered && Z1)
	{
		TD2.SetRunning(true);
		TD4.SetRunning(true);
	}

	if (TD4.ContactClosed())
		Z4 = true;
	else
		Z4 = false;

	if (smjettbuspowered && Z1 && !Z4)
		FireMinusXTrans = true;
	else
		FireMinusXTrans = false;

	if (TD2.ContactClosed())
		Z2 = true;
	else
		Z2 = false;

	if (smjettbuspowered && Z1 && Z2)
		TD3.SetRunning(true);

	if (smjettbuspowered && Z1 && Z2 && (TD3.ContactClosed() || Z3))
		Z3 = true;
	else
		Z3 = false;

	if (smjettbuspowered && Z1 && Z2 && !Z3)
		FirePlusRoll = true;
	else
		FirePlusRoll = false;
}

void SMJC_MOD1::SetState(const SMJCState &state)
{
	SMJC::SetState(state);
	Z4 = state.Z4;
	TD4.SetState(state.TD4State);
}

void SMJC_MOD1::GetState(SMJCState &state)
{
	SMJC::GetState(state);
	state.Z4 = Z4;
	TD4.GetState(state.TD4State);
}