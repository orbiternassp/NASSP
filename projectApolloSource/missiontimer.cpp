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
	hours += n;
	while (hours > 999)
		hours -= 1000;
	if (hours < 0)
		hours = 0;
}

void MissionTimer::UpdateMinutes(int n)

{
	minutes += n;
	while (minutes > 59) {
		minutes -= 60;
	}
	if (minutes < 0)
		minutes = 0;
}

void MissionTimer::UpdateSeconds(int n)

{
	seconds += n;
	while (seconds > 59) {
		seconds -= 60;
	}
	if (seconds < 0)
		seconds = 0;
}

void MissionTimer::Timestep(double simt, double deltat)

{
	if (Running && Enabled) {
		extra += deltat;

		if (extra >= 1.0) {
			int n = (int) extra;
			seconds += n;
			if (seconds > 59) {
				int m = (seconds / 60);
				minutes += m;
				seconds -= (m * 60);
				if (minutes > 59) {
					m = (minutes / 60);
					hours += m;
					minutes -= (m * 60);
					if (hours > 999) {
						m = (hours / 1000);
						hours -= (m * 1000);
					}
				}
			}
			extra -= (double) n;
		}
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

