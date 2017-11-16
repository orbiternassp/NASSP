/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Mission timer code.

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


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "soundlib.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "powersource.h"
#include "missiontimer.h"
#include "papi.h"

MissionTimer::MissionTimer(PanelSDK &p) : DCPower(0, p)

{
	Running = false;
	CountUp = TIMER_COUNT_UP;
	TimerTrash = false;
	DimmerRotationalSwitch = NULL;
	srand((unsigned int)time(NULL)); // time gives 64bit 'time_t', srand wants int. We specify unsigned int because data loss doesn't matter.

	Reset();
}

MissionTimer::~MissionTimer()

{
	// Nothing for now.
}

void MissionTimer::Init(e_object *a, e_object *b, RotationalSwitch *dimmer, e_object *c) {
	DCPower.WireToBuses(a, b);
	WireTo(c);
	DimmerRotationalSwitch = dimmer;
}

void MissionTimer::Reset()

{
	hours = 0;
	minutes = 0;
	seconds = 0;
	extra = 0.0;
}

void MissionTimer::Garbage()

{
	hours = rand() % 1000;
	minutes = rand() % 60;
	seconds = rand() % 60;
	TimerTrash = true;
}

void MissionTimer::UpdateHours(int n)

{
	if (!IsPowered())
		return;

	if (CountUp == TIMER_COUNT_UP) {
		hours += n;
	}
	else if (CountUp == TIMER_COUNT_DOWN) {
		hours -= n;
	}

	while (hours > 999) {
		hours -= 1000;
	}
	while (hours < 0) {
		hours += 1000;
	}
}

void MissionTimer::UpdateMinutes(int n)

{
	if (!IsPowered())
		return;

	if (CountUp == TIMER_COUNT_UP) {
		minutes += n;
	}
	else if (CountUp == TIMER_COUNT_DOWN) {
		minutes -= n;
	}

	while (minutes > 59) {
		minutes -= 60;
	}
	while (minutes < 0) {
		minutes += 60;
	}
}

void MissionTimer::UpdateSeconds(int n)

{
	if (!IsPowered())
		return;

	if (CountUp == TIMER_COUNT_UP) {
		seconds += n;
	}
	else if (CountUp == TIMER_COUNT_DOWN) {
		seconds -= n;
	}

	while (seconds > 59) {
		seconds -= 60;
	}
	while (seconds < 0) {
		seconds += 60;
	}
}

bool MissionTimer::IsPowered()

{
	if (DCPower.Voltage() < SP_MIN_DCVOLTAGE) { return false; } // DC
	return true;
}

bool MissionTimer::IsDisplayPowered()

{
	if (Voltage() < SP_MIN_ACVOLTAGE || DimmerRotationalSwitch->GetState() == 0)
		return false;
	
	return true;
}

void MissionTimer::SystemTimestep(double simdt)

{
	DCPower.DrawPower(11.2);
	DrawPower(7 * 7 * 0.022);
}

void EventTimer::SystemTimestep(double simdt)

{
	if (Running)
		DCPower.DrawPower(5.0);
	else
		DCPower.DrawPower(1.0);
}


void LEMEventTimer::SystemTimestep(double simdt)

{
	DCPower.DrawPower(11.2);
	DrawPower(4 * 7 * 0.022);
}

//
// This isn't really the most efficient way to update the clock, but the original
// design didn't allow counting down. We might want to rewrite this at some point.
//

void MissionTimer::Timestep(double simt, double deltat, bool persistent)

{
	//sprintf(oapiDebugString(), "Timer status. Garbage: %d Powered: %d DC: %f AC: %f", TimerTrash, IsPowered(), DCPower.Voltage() ,Voltage());
	if (!IsPowered()) {
		if (!TimerTrash && !persistent) {
			Garbage();
		}
		return;
	}

	TimerTrash = false;

	if (Running && (CountUp != TIMER_COUNT_NONE)) {
		double t = GetTime();

		if (CountUp) {
			t += deltat;
		}
		else {
			t -= deltat;
		}

		SetTime(t);
	}
}

double MissionTimer::GetTime()

{
	double t;

	t = extra + (double) seconds + (60.0 * (double) minutes) + (3600.0 * (double) hours);
	return t;
}

//
// The real mission timer couldn't handle negative times, so we don't either anymore.
//

void MissionTimer::SetTime(double t)

{
	if (t < 0.0) {
		Reset();
		return;
	}

	int secs = (int) floor(t);
	extra = t - (double) secs;

	hours = (secs / 3600);
	secs -= (hours * 3600);
	while (hours > 999)
		hours -= 1000;
	minutes = (secs / 60);
	seconds = secs - (60 * minutes);
}

void MissionTimer::Render(SURFHANDLE surf, SURFHANDLE digits, bool csm)

{
	if (!IsPowered() || !IsDisplayPowered())
		return;

	int Curdigit, Curdigit2;

	// Hour display on three digit
	Curdigit = hours / 100;
	Curdigit2 = hours / 1000;
	oapiBlt(surf, digits, 0,0, 19*(Curdigit-(Curdigit2*10)),0,19,21);

	Curdigit = hours / 10;
	Curdigit2 = hours / 100;
	oapiBlt(surf, digits, 0+20,0, 19*(Curdigit-(Curdigit2*10)),0,19,21);

	Curdigit = hours;
	Curdigit2 = hours / 10;
	oapiBlt(surf, digits,0+39,0, 19*(Curdigit-(Curdigit2*10)),0,19,21);

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits,0+62,0, 19*(Curdigit-(Curdigit2*10)),0,19,21);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits,0+81,0, 19*(Curdigit-(Curdigit2*10)),0,19,21);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits,0+104,0, 19*(Curdigit-(Curdigit2*10)),0,19,21);

	Curdigit = seconds;
	Curdigit2 = seconds/10;
	oapiBlt(surf, digits,0+123,0, 19*(Curdigit-(Curdigit2*10)),0,19,21);
}

void MissionTimer::Render90(SURFHANDLE surf, SURFHANDLE digits, bool csm)

{
	if (!IsPowered() || !IsDisplayPowered())
		return;

	int Curdigit, Curdigit2;

	// Hour display on three digit
	Curdigit = hours / 100;
	Curdigit2 = hours / 1000;
	oapiBlt(surf, digits, 0, 0, 21 * (Curdigit - (Curdigit2 * 10)), 0, 21, 19);

	Curdigit = hours / 10;
	Curdigit2 = hours / 100;
	oapiBlt(surf, digits, 0, 0 + 20, 21 * (Curdigit - (Curdigit2 * 10)), 0, 21, 19);

	Curdigit = hours;
	Curdigit2 = hours / 10;
	oapiBlt(surf, digits, 0, 0 + 39, 21 * (Curdigit - (Curdigit2 * 10)), 0, 21, 19);

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0 + 62, 21 * (Curdigit - (Curdigit2 * 10)), 0, 21, 19);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 0, 0 + 81, 21 * (Curdigit - (Curdigit2 * 10)), 0, 21, 19);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 0, 0 + 104, 21 * (Curdigit - (Curdigit2 * 10)), 0, 21, 19);

	Curdigit = seconds;
	Curdigit2 = seconds / 10;
	oapiBlt(surf, digits, 0, 0 + 123, 21 * (Curdigit - (Curdigit2 * 10)), 0, 21, 19);
}

void MissionTimer::SaveState(FILEHANDLE scn, char *start_str, char *end_str, bool persistent) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "RUNNING", Running);
	oapiWriteScenario_int(scn, "COUNTUP", CountUp);
	if (!persistent) {
		papiWriteScenario_bool(scn, "TIMERTRASH", TimerTrash);
	}
	papiWriteScenario_double(scn, "MTD", GetTime());
	oapiWriteLine(scn, end_str);
}

void MissionTimer::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);
	float ftcp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		if (!strnicmp(line, "MTD", 3)) {
			sscanf(line + 3, "%f", &ftcp);
			SetTime(ftcp);
		}
		papiReadScenario_bool(line, "RUNNING", Running);
		papiReadScenario_int(line, "COUNTUP", CountUp);
		papiReadScenario_bool(line, "TIMERTRASH", TimerTrash);
	}
}

LEMEventTimer::LEMEventTimer(PanelSDK &p) : EventTimer(p)

{
	//
	// Nothing for now
	//
}

LEMEventTimer::~LEMEventTimer()
{
	//
	// Nothing for now
	//
}

void LEMEventTimer::Render(SURFHANDLE surf, SURFHANDLE digits)

{
	// Don't do this if not powered.
	if (!IsPowered() || !IsDisplayPowered())
		return;

	//
	// Digits are 16x19.
	//

	int Curdigit, Curdigit2;

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0, 19 * (Curdigit-(Curdigit2*10)), 0, 19,21);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 20, 0, 19 * (Curdigit-(Curdigit2*10)), 0, 19,21);

	//oapiBlt(surf, digits, 37,0, 192,0,4,19);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 43, 0, 19 * (Curdigit-(Curdigit2*10)), 0, 19,21);

	Curdigit = seconds;
	Curdigit2 = seconds/10;
	oapiBlt(surf, digits, 62, 0, 19 * (Curdigit-(Curdigit2*10)), 0, 19,21);
}

EventTimer::EventTimer(PanelSDK &p) : MissionTimer(p)

{

}

EventTimer::~EventTimer()

{
	//
	// Nothing for now
	//
}

void EventTimer::Render(SURFHANDLE surf, SURFHANDLE digits)

{
	//
	// Digits are 13x18.
	//

	int Curdigit, Curdigit2;

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0, 13 * (Curdigit-(Curdigit2*10)), 0, 13, 18);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 13, 0, 13 * (Curdigit-(Curdigit2*10)), 0, 13, 18);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 45, 0, 13 * (Curdigit-(Curdigit2*10)), 0, 13, 18);

	Curdigit = seconds;
	Curdigit2 = seconds/10;
	oapiBlt(surf, digits, 58, 0, 13 * (Curdigit-(Curdigit2*10)), 0, 13, 18);
}

void EventTimer::Render90(SURFHANDLE surf, SURFHANDLE digits)

{
	//
	// Digits are 13x18.
	//

	int Curdigit, Curdigit2;

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0, 19 * (Curdigit - (Curdigit2 * 10)), 0, 18, 13);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 0, 13, 19 * (Curdigit - (Curdigit2 * 10)), 0, 18, 13);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 0, 45, 19 * (Curdigit - (Curdigit2 * 10)), 0, 18, 13);

	Curdigit = seconds;
	Curdigit2 = seconds / 10;
	oapiBlt(surf, digits, 0, 58, 19 * (Curdigit - (Curdigit2 * 10)), 0, 18, 13);
}
