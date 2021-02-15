/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Delay Timer

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
#include "DelayTimer.h"

DelayTimer::DelayTimer(double del)

{
	Running = false;

	delay = del;
	seconds = 0.0;
	Contact = false;
}

double DelayTimer::GetTime()

{
	return seconds;
}

void DelayTimer::SetTime(double t)

{
	if (seconds > delay) {
		SetContact(true);
		return;
	}

	seconds = t;
}

void DelayTimer::Timestep(double simdt)
{

	if (Running) {
		double t = GetTime();

		t += simdt;

		SetTime(t);

		Running = false;
	}
	else
	{
		Reset();
	}
}

void DelayTimer::Reset()

{
	seconds = 0.0;
	Running = false;
	Contact = false;
}

void DelayTimer::SetState(const DelayTimerState &state)
{
	Running = state.Running;
	SetTime(state.seconds);
}

void DelayTimer::GetState(DelayTimerState &state)
{
	state.Running = Running;
	state.seconds = GetTime();
}

void DelayTimer::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "CONTACT", Contact);
	papiWriteScenario_bool(scn, "RUNNING", Running);
	papiWriteScenario_double(scn, "TIME", GetTime());
	oapiWriteLine(scn, end_str);
}

void DelayTimer::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);
	float ftcp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		if (!strnicmp(line, "TIME", 4)) {
			sscanf(line + 4, "%f", &ftcp);
			SetTime(ftcp);
		}
		papiReadScenario_bool(line, "CONTACT", Contact);
		papiReadScenario_bool(line, "RUNNING", Running);
	}
}

RestartableDelayTimer::RestartableDelayTimer(double del) : DelayTimer(del)
{
	Start = false;
}

void RestartableDelayTimer::Timestep(double simdt)

{

	if (Start && Running) {
		double t = GetTime();

		t += simdt;

		SetTime(t);

		Running = false;
	}
}

void RestartableDelayTimer::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "CONTACT", Contact);
	papiWriteScenario_bool(scn, "RUNNING", Running);
	papiWriteScenario_bool(scn, "START", Start);
	papiWriteScenario_double(scn, "TIME", GetTime());
	oapiWriteLine(scn, end_str);
}

void RestartableDelayTimer::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);
	float ftcp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		if (!strnicmp(line, "TIME", 4)) {
			sscanf(line + 4, "%f", &ftcp);
			SetTime(ftcp);
		}
		papiReadScenario_bool(line, "CONTACT", Contact);
		papiReadScenario_bool(line, "RUNNING", Running);
		papiReadScenario_bool(line, "START", Start);
	}
}

DelayOffTimer::DelayOffTimer(double del) : DelayTimer(del)
{

}

void DelayOffTimer::Timestep(double simdt)
{
	if (Running) {
		seconds = delay;
		SetContact(true);
		Running = false;
	}
	else
	{
		double t = GetTime();

		if (t > 0)
		{
			t -= simdt;
			SetTime(t);
		}
		else
		{
			Reset();
		}
	}
}

void DelayOffTimer::SetTime(double t)
{
	seconds = t;

	if (seconds < 0)
	{
		SetContact(false);
	}
}