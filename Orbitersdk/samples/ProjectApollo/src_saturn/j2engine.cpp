/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

J-2 Engine

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
#include "j2engine.h"

J2Engine::J2Engine(VESSEL *v, THRUSTER_HANDLE &j2)
	:vessel(v), th_j2(j2)
{
	EngineReady = false;
	EngineReadyBypass = false;
	EngineStart = false;
	EngineStop = false;
	LVDCEngineStopRelay = false;
	EDSCutoff = false;
	RSSCutoff = false;
	EngineRunning = false;
	ThrustOK = false;
	EngineFailed = false;

	ThrustTimer = 0.0;
	ThrustLevel = 0.0;
}

void J2Engine::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "ENGINEREADY", EngineReady);
	papiWriteScenario_bool(scn, "ENGINEREADYBYPASS", EngineReadyBypass);
	papiWriteScenario_bool(scn, "ENGINESTART", EngineStart);
	papiWriteScenario_bool(scn, "ENGINESTOP", EngineStop);
	papiWriteScenario_bool(scn, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
	papiWriteScenario_bool(scn, "EDSCUTOFF", EDSCutoff);
	papiWriteScenario_bool(scn, "RSSCUTOFF", RSSCutoff);
	papiWriteScenario_bool(scn, "ENGINERUNNING", EngineRunning);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	oapiWriteLine(scn, end_str);
}

void J2Engine::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "ENGINEREADY", EngineReady);
		papiReadScenario_bool(line, "ENGINEREADYBYPASS", EngineReadyBypass);
		papiReadScenario_bool(line, "ENGINESTART", EngineStart);
		papiReadScenario_bool(line, "ENGINESTOP", EngineStop);
		papiReadScenario_bool(line, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
		papiReadScenario_bool(line, "EDSCUTOFF", EDSCutoff);
		papiReadScenario_bool(line, "RSSCUTOFF", RSSCutoff);
		papiReadScenario_bool(line, "ENGINERUNNING", EngineRunning);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
	}
}

void J2Engine::Timestep(double simdt)
{
	if (th_j2 == NULL) return;

	ThrustOK = vessel->GetThrusterLevel(th_j2) > 0.65 && !EngineFailed;

	if (!EngineRunning)
	{
		EngineReady = true;
	}
	else
	{
		EngineReady = false;
	}

	if ((EngineReady || EngineReadyBypass) && EngineStart)
	{
		EngineStart = true;
	}
	else
	{
		EngineStart = false;
	}

	if (LVDCEngineStopRelay || EDSCutoff || RSSCutoff || (!ThrustOK && EngineRunning))
	{
		EngineStop = true;
	}
	else
	{
		EngineStop = false;
	}

	if (EngineStop)
	{
		EngineRunning = false;

		if (ThrustLevel > 0.0)
		{
			ThrustTimer += simdt;

			// Cutoff transient thrust
			if (ThrustTimer < 2.0) {
				if (ThrustTimer < 0.25) {
					// 95% of thrust dies in the first .25 second
					ThrustLevel = 1.0 - (ThrustTimer*3.3048);
					vessel->SetThrusterLevel(th_j2, ThrustLevel);
				}
				else {
					if (ThrustTimer < 1.5) {
						// The remainder dies over the next 1.25 second
						ThrustLevel = 0.1738 - ((ThrustTimer - 0.25)*0.1390);
						vessel->SetThrusterLevel(th_j2, ThrustLevel);
					}
					else {
						// Engine is completely shut down at 1.5 second
						ThrustLevel = 0.0;
						vessel->SetThrusterLevel(th_j2, ThrustLevel);
					}
				}
			}
		}
	}
	else if (EngineStart && !EngineRunning)
	{
		ThrustTimer += simdt;

		//First Burn
		if (ThrustTimer >= 1.0 && ThrustTimer < 3.2) {
			ThrustLevel = (ThrustTimer - 1.0)*0.45;
			vessel->SetThrusterLevel(th_j2, ThrustLevel);
		}
		else if (ThrustTimer > 3.2 && ThrustLevel < 1.0)
		{
			ThrustLevel = 1.0;
			vessel->SetThrusterLevel(th_j2, ThrustLevel);

			EngineRunning = true;
		}
	}
	else if (ThrustTimer > 0.0)
	{
		ThrustTimer = 0.0;
	}
}

void J2Engine::SetThrusterDir(double beta_y, double beta_p)
{
	if (th_j2 == NULL) return;

	VECTOR3 j2vector;

	if (beta_y > 7.0*RAD)
	{
		j2vector.x = 7.0*RAD;
	}
	else if (beta_y < -7.0*RAD)
	{
		j2vector.x = -7.0*RAD;
	}
	else
	{
		j2vector.x = beta_y;
	}

	if (beta_p > 7.0*RAD)
	{
		j2vector.y = 7.0*RAD;
	}
	else if (beta_p < -7.0*RAD)
	{
		j2vector.y = -7.0*RAD;
	}
	else
	{
		j2vector.y = beta_p;
	}

	j2vector.z = 1.0;

	vessel->SetThrusterDir(th_j2, j2vector);
}