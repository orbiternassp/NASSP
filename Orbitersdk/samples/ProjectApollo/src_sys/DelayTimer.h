/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Delay Timer (Header)

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

#pragma once

#include "Orbitersdk.h"

struct DelayTimerState
{
	double Running;
	double seconds;
};

class DelayTimer {

public:
	DelayTimer(double delay);

	virtual void Timestep(double simdt);
	virtual void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	virtual void LoadState(FILEHANDLE scn, char *end_str);
	virtual void SetTime(double t);
	double GetTime();

	void Reset();
	void SetRunning(bool run) { Running = run; };
	bool IsRunning() { return Running; };
	void SetContact(bool cont) { Contact = cont; };
	bool ContactClosed() { return Contact; };

	void SetDelay(double del) { delay = del; }

	void SetState(const DelayTimerState &state);
	void GetState(DelayTimerState &state);

protected:
	double seconds;
	double delay;

	bool Running;
	bool Contact;
};

class RestartableDelayTimer : public DelayTimer
{
public:
	RestartableDelayTimer(double delay);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetStart(bool st) { Start = st; };
	bool GetStart() { return Start; };

protected:
	bool Start;
};

class DelayOffTimer : public DelayTimer
{
public:
	DelayOffTimer(double delay);
	void Timestep(double simdt);
	void SetTime(double t);
};