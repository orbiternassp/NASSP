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


#if !defined(_PA_MISSIONTIMER_H)
#define _PA_MISSIONTIMER_H

#define MISSIONTIMER_2_START_STRING "MISSIONTIMER2_START"
#define MISSIONTIMER_306_START_STRING "MISSIONTIMER306_START"
#define MISSIONTIMER_END_STRING "MISSIONTIMER_END"
#define EVENTTIMER_2_START_STRING "EVENTTIMER2_START"
#define EVENTTIMER_306_START_STRING "EVENTTIMER306_START"
#define EVENTTIMER_END_STRING "EVENTTIMER_END"

class Saturn;

class MissionTimer : public e_object {

public:
	MissionTimer(PanelSDK &p);
	virtual ~MissionTimer();

	void Init(e_object *mna, e_object *mnb, RotationalSwitch *dimmer);
	void Timestep(double simt, double deltat, bool eventimer);
	virtual void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str, bool eventimer);
	void LoadState(FILEHANDLE scn, char *end_str);
	void WireTo(e_object *a, e_object *b) { DCPower.WireToBuses(a, b); };

	Saturn *sat;

	void SetTime(double t);
	double GetTime();

	void UpdateMinutes(int n);
	void UpdateHours(int n);
	void UpdateSeconds(int n);
	void Reset();
	void Garbage();
	bool IsPowered();
	bool IsDisplayPowered();
	void SetRunning(bool run) { Running = run; };
	bool IsRunning() { return Running; };
	void SetEnabled(bool run) { Enabled = run; };
	bool IsEnabled() { return Enabled; };
	void SetCountUp(int val) { CountUp = val; };
	int GetCountUp() { return CountUp; };
	int GetHours(){ return hours; }
	int GetMinutes(){ return minutes; }
	int GetSeconds(){ return seconds; }

	virtual void Render(SURFHANDLE surf, SURFHANDLE digits, bool csm = false);
	virtual void Render90(SURFHANDLE surf, SURFHANDLE digits, bool csm = false);

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
	bool TimerTrash;
	int CountUp;

	//
	// Don't need to be saved.
	//

	RotationalSwitch *DimmerRotationalSwitch;
	PowerMerge DCPower;
};

//
// Event Timer needs a different render function.
//

class EventTimer: public MissionTimer {
public:
	EventTimer(PanelSDK &p);
	virtual ~EventTimer();
	void Render(SURFHANDLE surf, SURFHANDLE digits);
	void Render90(SURFHANDLE surf, SURFHANDLE digits);
	void SystemTimestep(double simdt);

protected:
};

//
// And LEM Event Timer needs yet another render function!
//

class LEMEventTimer: public EventTimer {
public:
	LEMEventTimer(PanelSDK &p);
	virtual ~LEMEventTimer();
	void Render(SURFHANDLE surf, SURFHANDLE digits);
	void SystemTimestep(double simdt);

protected:
};


#define TIMER_COUNT_DOWN	0
#define TIMER_COUNT_NONE	1
#define TIMER_COUNT_UP		2

#endif
