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


// To force Orbitersdk.h to use <fstream> in any compiler version
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
	DimmerOverride = NULL;
	srand((unsigned int) time(NULL));

	ResetFlag = false;
	ResetStatus = false;
	hours = 0;
	minutes = 0;
	seconds = 0;
	extra = 0.0;
}

MissionTimer::~MissionTimer()
{
	// Nothing for now.
}

void MissionTimer::Init(e_object *a, e_object *b, ContinuousRotationalSwitch *dimmer, e_object *c, ToggleSwitch *override)
{
	DCPower.WireToBuses(a, b);
	WireTo(c);
	DimmerRotationalSwitch = dimmer;
	DimmerOverride = override;
}

void MissionTimer::Reset()
{	
	//Only reset, if it isn't already being reset
	if (!ResetStatus)
	{
		hours = 0;
		minutes = 0;
		seconds = 0;
		extra = 0.0;
		ResetStatus = true;
	}

	ResetFlag = true;
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

void MissionTimer::CountingThroughZero(double &t)
{
	//Mission timers can't count down, so we should never even get here
	t = 0.0;
}

bool MissionTimer::IsPowered()
{
	if (DCPower.Voltage() < SP_MIN_DCVOLTAGE)
		return false;

	return true;
}

bool MissionTimer::IsDisplayPowered()
{
	if (Voltage() < SP_MIN_ACVOLTAGE) return false;

	if (DimmerOverride && DimmerOverride->GetState() == 1)
	{
		//Do nothing
	}
	else if (DimmerRotationalSwitch->GetOutput() < 0.00001)
	{
		return false;
	}

	return true;
}

void MissionTimer::SystemTimestep(double simdt)
{
	if (IsPowered())
		DCPower.DrawPower(11.2);

	if (IsDisplayPowered())
		DrawPower(7.0 * 7.0 * 0.022);
}

void EventTimer::SystemTimestep(double simdt)
{
	if (IsPowered()) {
		if (Running)
			DCPower.DrawPower(5.0);
		else
			DCPower.DrawPower(1.0);
	}
}


void LEMEventTimer::SystemTimestep(double simdt)
{
	if (IsPowered())
		DCPower.DrawPower(11.2);

	if (IsDisplayPowered())
		DrawPower(4.0 * 7.0 * 0.022);
}

//
// This isn't really the most efficient way to update the clock, but the original
// design didn't allow counting down. We might want to rewrite this at some point.
//

void MissionTimer::Timestep(double simt, double deltat, bool persistent)
{
	//sprintf(oapiDebugString(), "Timer status. Garbage: %d Powered: %d DC: %f AC: %f", TimerTrash, IsPowered(), DCPower.Voltage(), Voltage());
	if (!IsPowered()) {
		if (!TimerTrash && !persistent)
			Garbage();

		return;
	}

	TimerTrash = false;

	if (Running && (CountUp != TIMER_COUNT_NONE)) {
		double t = GetTime();

		if (CountUp)
			t += deltat;
		else
			t -= deltat;

		SetTime(t);
	}

	//If the reset condition is over (ResetFlag false), then reset the ResetStatus, to allow new resets
	if (!ResetFlag && ResetStatus)
	{
		ResetStatus = false;
	}
	else
	{
		ResetFlag = false;
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
		CountingThroughZero(t);
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

void MissionTimer::Render(SURFHANDLE surf, SURFHANDLE digits, bool csm, int TexMul)
{
	if (!IsPowered() || !IsDisplayPowered())
		return;

	const int DigitWidth = 19*TexMul;
	const int DigitHeight = 21*TexMul;
	int Curdigit, Curdigit2;

	// Hour display on three digit
	Curdigit = hours / 100;
	Curdigit2 = hours / 1000;
	oapiBlt(surf, digits, 0, 0, DigitWidth*(Curdigit- (Curdigit2 * 10)), 0,DigitWidth,DigitHeight);

	Curdigit = hours / 10;
	Curdigit2 = hours / 100;
	oapiBlt(surf, digits, 0+20*TexMul, 0, DigitWidth*(Curdigit- (Curdigit2 * 10)), 0, DigitWidth,DigitHeight);

	Curdigit = hours;
	Curdigit2 = hours / 10;
	oapiBlt(surf, digits, 0+39*TexMul, 0, DigitWidth*(Curdigit- (Curdigit2 * 10)), 0, DigitWidth,DigitHeight);

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0+62*TexMul, 0, DigitWidth*(Curdigit- (Curdigit2 * 10)), 0, DigitWidth,DigitHeight);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 0+81*TexMul, 0, DigitWidth*(Curdigit- (Curdigit2 * 10)), 0, DigitWidth,DigitHeight);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 0+104*TexMul, 0, DigitWidth*(Curdigit- (Curdigit2 * 10)), 0, DigitWidth,DigitHeight);

	Curdigit = seconds;
	Curdigit2 = seconds/10;
	oapiBlt(surf, digits, 0+123*TexMul, 0, DigitWidth*(Curdigit- (Curdigit2 * 10)), 0, DigitWidth,DigitHeight);
}

void MissionTimer::Render90(SURFHANDLE surf, SURFHANDLE digits, bool csm, int TexMul)
{
	if (!IsPowered() || !IsDisplayPowered())
		return;

	const int DigitWidth = 21*TexMul;
	const int DigitHeight = 19*TexMul;
	int Curdigit, Curdigit2;

	// Hour display on three digit
	Curdigit = hours / 100;
	Curdigit2 = hours / 1000;
	oapiBlt(surf, digits, 0, 0, DigitWidth * (Curdigit - (Curdigit2 * 10)), 0, DigitWidth, DigitHeight);

	Curdigit = hours / 10;
	Curdigit2 = hours / 100;
	oapiBlt(surf, digits, 0, 0 + 20*TexMul, DigitWidth * (Curdigit - (Curdigit2 * 10)), 0, DigitWidth, DigitHeight);

	Curdigit = hours;
	Curdigit2 = hours / 10;
	oapiBlt(surf, digits, 0, 0 + 39*TexMul, DigitWidth * (Curdigit - (Curdigit2 * 10)), 0, DigitWidth, DigitHeight);

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0 + 62*TexMul, DigitWidth * (Curdigit - (Curdigit2 * 10)), 0, DigitWidth, DigitHeight);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 0, 0 + 81*TexMul, DigitWidth * (Curdigit - (Curdigit2 * 10)), 0, DigitWidth, DigitHeight);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 0, 0 + 104*TexMul, DigitWidth * (Curdigit - (Curdigit2 * 10)), 0, DigitWidth, DigitHeight);

	Curdigit = seconds;
	Curdigit2 = seconds / 10;
	oapiBlt(surf, digits, 0, 0 + 123*TexMul, DigitWidth * (Curdigit - (Curdigit2 * 10)), 0, DigitWidth, DigitHeight);
}

void MissionTimer::SaveState(FILEHANDLE scn, char *start_str, char *end_str, bool persistent)
{
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "RUNNING", Running);
	oapiWriteScenario_int(scn, "COUNTUP", CountUp);
	papiWriteScenario_bool(scn, "RESETFLAG", ResetFlag);
	papiWriteScenario_bool(scn, "RESETSTATUS", ResetStatus);
	if (!persistent)
		papiWriteScenario_bool(scn, "TIMERTRASH", TimerTrash);
	papiWriteScenario_double(scn, "MTD", GetTime());
	oapiWriteLine(scn, end_str);
}

void MissionTimer::LoadState(FILEHANDLE scn, char *end_str)
{
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);
	float ftcp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			break;

		if (!strnicmp(line, "MTD", 3)) {
			sscanf(line + 3, "%f", &ftcp);
			SetTime(ftcp);
		}

		papiReadScenario_bool(line, "RUNNING", Running);
		papiReadScenario_int(line, "COUNTUP", CountUp);
		papiReadScenario_bool(line, "TIMERTRASH", TimerTrash);
		papiReadScenario_bool(line, "RESETFLAG", ResetFlag);
		papiReadScenario_bool(line, "RESETSTATUS", ResetStatus);
	}
}

LEMEventTimer::LEMEventTimer(PanelSDK &p) : EventTimer(p)
{
	ReverseDirection = false;
}

LEMEventTimer::~LEMEventTimer()
{
	//
	// Nothing for now
	//
}

void LEMEventTimer::Render(SURFHANDLE surf, SURFHANDLE digits, int TexMul)
{
	// Don't do this if not powered.
	if (!IsPowered() || !IsDisplayPowered())
		return;

	//
	// Digits are 16x19.
	//

	const int DigitWidth = 19*TexMul;
	const int DigitHeight = 21*TexMul;
	int Curdigit, Curdigit2;

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0, DigitWidth * (Curdigit-(Curdigit2*10)), 0, DigitWidth,DigitHeight);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 20*TexMul, 0, DigitWidth * (Curdigit-(Curdigit2*10)), 0, DigitWidth,DigitHeight);

	//oapiBlt(surf, digits, 37, 0, DigitWidth2, 0, 4, DigitWidth);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 43*TexMul, 0, DigitWidth * (Curdigit-(Curdigit2*10)), 0, DigitWidth,DigitHeight);

	Curdigit = seconds;
	Curdigit2 = seconds/10;
	oapiBlt(surf, digits, 62*TexMul, 0, DigitWidth * (Curdigit-(Curdigit2*10)), 0, DigitWidth,DigitHeight);
}

void LEMEventTimer::CountingThroughZero(double &t)
{
	if (ReverseDirection)
	{
		//t will be slightly below zero, inverse the sign to countinue counting up
		t = -t;
		//Change direction
		CountUp = TIMER_COUNT_UP;
	}
	else
	{
		//Normal event timer behavior
		EventTimer::CountingThroughZero(t);
	}
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

void EventTimer::Render(SURFHANDLE surf, SURFHANDLE digits, int TexMul)
{
	//
	// Digits are 13x18.
	//

	int Curdigit, Curdigit2;

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0, 13*TexMul * (Curdigit-(Curdigit2*10)), 0, 13*TexMul, 18*TexMul);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 13*TexMul, 0, 13*TexMul * (Curdigit-(Curdigit2*10)), 0, 13*TexMul, 18*TexMul);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 45*TexMul, 0, 13*TexMul * (Curdigit-(Curdigit2*10)), 0, 13*TexMul, 18*TexMul);

	Curdigit = seconds;
	Curdigit2 = seconds/10;
	oapiBlt(surf, digits, 58*TexMul, 0, 13*TexMul * (Curdigit-(Curdigit2*10)), 0, 13*TexMul, 18*TexMul);
}

void EventTimer::Render90(SURFHANDLE surf, SURFHANDLE digits, int TexMul)
{
	//
	// Digits are 13x18.
	//

	int Curdigit, Curdigit2;

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits, 0, 0, 19*TexMul * (Curdigit - (Curdigit2 * 10)), 0, 18*TexMul, 13*TexMul);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits, 0, 13*TexMul, 19*TexMul * (Curdigit - (Curdigit2 * 10)), 0, 18*TexMul, 13*TexMul);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits, 0, 45*TexMul, 19*TexMul * (Curdigit - (Curdigit2 * 10)), 0, 18*TexMul, 13*TexMul);

	Curdigit = seconds;
	Curdigit2 = seconds / 10;
	oapiBlt(surf, digits, 0, 58*TexMul, 19*TexMul * (Curdigit - (Curdigit2 * 10)), 0, 18*TexMul, 13*TexMul);
}

void EventTimer::CountingThroughZero(double &t)
{
	//Only event timers can count down, add 1 hour to make it count through zero
	t += 3600.0;
}