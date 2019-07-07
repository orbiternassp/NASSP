/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Animation utilities

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
#include "animations.h"

AnimState2::AnimState2()
{
	state = 0.0;
	speed = 0.0;
	inc_speed = 1.0;
	dec_speed = -1.0;
}

// --------------------------------------------------------------

AnimState2::AnimState2(double operating_speed, double initial_state)
{
	state = initial_state;
	speed = 0.0;
	inc_speed = operating_speed;
	dec_speed = -operating_speed;
}

// --------------------------------------------------------------

void AnimState2::SetOperatingSpeed(double opspeed)
{
	inc_speed = opspeed;
	dec_speed = -opspeed;
}

// --------------------------------------------------------------

void AnimState2::SetState(double _state, double _speed)
{
	state = _state;
	speed = _speed;
	if (state >= 1.0) {
		state = 1.0;
		if (speed > 0.0)
			speed = 0.0;
	}
	else if (state <= 0.0) {
		state = 0.0;
		if (speed < 0.0)
			speed = 0.0;
	}
}

// --------------------------------------------------------------

void AnimState2::Open()
{
	if (state < 1.0)
		speed = inc_speed;
}

// --------------------------------------------------------------

void AnimState2::Close()
{
	if (state > 0.0)
		speed = dec_speed;
}

// --------------------------------------------------------------

void AnimState2::Stop()
{
	speed = 0.0;
}

// --------------------------------------------------------------

void AnimState2::SetOpened()
{
	state = 1.0;
	speed = 0.0;
}

// --------------------------------------------------------------

void AnimState2::SetClosed()
{
	state = 0.0;
	speed = 0.0;
}

// --------------------------------------------------------------

bool AnimState2::Process(double dt)
{
	if (speed) {
		state += speed * dt;
		if (state <= 0.0) {
			state = 0.0;
			speed = 0.0;
		}
		else if (state >= 1.0) {
			state = 1.0;
			speed = 0.0;
		}
		return true;
	}
	else {
		return false;
	}
}

// --------------------------------------------------------------

void AnimState2::SaveState(FILEHANDLE scn, const char *label)
{
	if (state) {
		char cbuf[256];
		sprintf(cbuf, "%0.4lf %0.4lf", state, speed);
		oapiWriteScenario_string(scn, (char*)label, cbuf);
	}
}

// --------------------------------------------------------------

bool AnimState2::ParseScenarioLine(const char *line, const char *label)
{
	if (!_strnicmp(line, label, strlen(label))) {
		sscanf(line + strlen(label), "%lf%lf", &state, &speed);
		return true;
	}
	return false;
}