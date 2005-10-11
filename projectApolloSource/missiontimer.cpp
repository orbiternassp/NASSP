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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.5  2005/08/24 23:29:31  movieman523
  *	Fixed event timer reset.
  *	
  *	Revision 1.4  2005/08/21 17:21:10  movieman523
  *	Added event timer display.
  *	
  *	Revision 1.3  2005/08/18 19:12:21  movieman523
  *	Added Event Timer switches and null Event Timer class.
  *	
  *	Revision 1.2  2005/08/14 15:25:43  movieman523
  *	Based on advice from ProjectApollo list, mission timer now starts running from zero at liftoff, and doesn't run on the pad.
  *	
  *	Revision 1.1  2005/08/13 22:05:17  movieman523
  *	Mission timer class.
  *	
  **************************************************************************/


#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

#include "soundlib.h"
#include "nasspsound.h"

#include "missiontimer.h"

MissionTimer::MissionTimer()

{
	Reset();

	Running = false;
	Enabled = false;
	CountUp = TIMER_COUNT_UP;
}

MissionTimer::~MissionTimer()

{
	// Nothing for now.
}

void MissionTimer::Reset()

{
	hours = 0;
	minutes = 0;
	seconds = 0;
	extra = 0.0;
}

void MissionTimer::UpdateHours(int n)

{
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

//
// This isn't really the most efficient way to update the clock, but the original
// design didn't allow counting down. We might want to rewrite this at some point.
//

void MissionTimer::Timestep(double simt, double deltat)

{
	if (Running && Enabled && (CountUp != TIMER_COUNT_NONE)) {
		double t = GetTime();

		if (CountUp) {
			t += deltat;
		}
		else {
			t -= deltat;
		}

		if (t < 0.0)
			t = 0.0;

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

void MissionTimer::Render(SURFHANDLE surf, SURFHANDLE digits)

{
	int Curdigit, Curdigit2;

	// Hour display on three digit
	Curdigit = hours / 100;
	Curdigit2 = hours / 1000;
	oapiBlt(surf, digits, 0,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);

	Curdigit = hours / 10;
	Curdigit2 = hours / 100;
	oapiBlt(surf, digits, 0+17,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);

	Curdigit = hours;
	Curdigit2 = hours / 10;
	oapiBlt(surf, digits,0+34,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
	oapiBlt(surf, digits,0+54,0, 192,0,4,19);

	// Minute display on two digit
	Curdigit = minutes / 10;
	Curdigit2 = minutes / 100;
	oapiBlt(surf, digits,0+61,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);

	Curdigit = minutes;
	Curdigit2 = minutes / 10;
	oapiBlt(surf, digits,0+78,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
	oapiBlt(surf, digits,0+98,0, 192,0,4,19);

	// second display on two digit
	Curdigit = seconds / 10;
	Curdigit2 = seconds / 100;
	oapiBlt(surf, digits,0+105,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);

	Curdigit = seconds;
	Curdigit2 = seconds/10;
	oapiBlt(surf, digits,0+122,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);

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
