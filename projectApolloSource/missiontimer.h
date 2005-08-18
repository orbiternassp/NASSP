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
  *	Revision 1.2  2005/08/14 15:25:43  movieman523
  *	Based on advice from ProjectApollo list, mission timer now starts running from zero at liftoff, and doesn't run on the pad.
  *	
  *	Revision 1.1  2005/08/13 22:05:17  movieman523
  *	Mission timer class.
  *	
  **************************************************************************/


#if !defined(_PA_MISSIONTIMER_H)
#define _PA_MISSIONTIMER_H

class MissionTimer {

public:
	MissionTimer();
	virtual ~MissionTimer();

	void Timestep(double simt, double deltat);

	void SetTime(double t);
	double GetTime();

	void UpdateMinutes(int n);
	void UpdateHours(int n);
	void UpdateSeconds(int n);
	void Reset();
	void SetRunning(bool run) { Running = run; };
	bool IsRunning() { return Running; };
	void SetEnabled(bool run) { Enabled = run; };
	bool IsEnabled() { return Enabled; };
	void SetCountUp(bool val) { CountUp = val; };
	bool GetCountUp() { return CountUp; };

	virtual void Render(SURFHANDLE surf, SURFHANDLE digits);

protected:

	//
	// These are expected to be saved by the owning class.
	//

	int hours;
	int minutes;
	int seconds;
	double extra;

	bool Running;
	bool Enabled;
	bool CountUp;

	//
	// Don't need to be saved.
	//
};

//
// Event Timer needs a different render function.
//

class EventTimer: public MissionTimer {
public:
	void Render(SURFHANDLE surf, SURFHANDLE digits);

protected:
};

#endif
