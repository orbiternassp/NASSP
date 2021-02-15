/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Animation utilities (Header)

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

class AnimState2 {
public:
	AnimState2();
	AnimState2(double operating_speed, double initial_state = 0.0);
	void SetOperatingSpeed(double opspeed);
	void SetState(double _state, double _speed);
	void Open();
	void Close();
	void Stop();
	void SetOpened();
	void SetClosed();
	inline bool IsActive() const { return speed != 0.0; }
	inline bool IsOpen() const { return state == 1.0; }
	inline bool IsClosed() const { return state == 0.0; }
	inline bool IsOpening() const { return speed > 0.0; }
	inline bool IsClosing() const { return speed < 0.0; }
	bool Process(double dt);
	inline double State() const { return state; }
	inline double Speed() const { return speed; }
	inline const double *StatePtr() const { return &state; }
	void SaveState(FILEHANDLE scn, const char *label);
	bool ParseScenarioLine(const char *line, const char *label);

private:
	double state;
	double speed;
	double inc_speed;
	double dec_speed;
};