/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  Toggle switch handling code.
  This file defines a number of useful toggle switches which can
  be set up to avoid huge swathes of code checking for mouse events,
  checking and processing switch state, and displaying switch state.

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
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspsound.h"

#include "toggleswitch.h"

//
// Generic toggle switch.
//

ToggleSwitch::ToggleSwitch()

{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	state = 0;

	next = 0;

	SwitchSurface = 0;
	OurVessel = 0;
	Active = true;
}

ToggleSwitch::~ToggleSwitch()

{
	Sclick.done();
}

bool ToggleSwitch::DoCheckMouseClick(int mx, int my)

{
	int OldState = state;

	//
	// Check whether it's actually in our switch region.
	//

	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//

	if (my > (y + (height / 2))) {
		if (state) {
			state = 0;
			Sclick.play();
		}
	}
	else {
		if (!state) {
			state = 1;
			Sclick.play();
		}
	}

	if (Active && (state != OldState))
		SwitchToggled = true;

	return true;
}

bool ToggleSwitch::CheckMouseClick(int event, int mx, int my)

{
	return DoCheckMouseClick(mx, my);
}

bool ThreePosSwitch::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	//
	// Check whether it's actually in our switch region.
	//

	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//

	if (my > (y + (height / 2))) {
		if (state > 0) {
			state--;
			Sclick.play();
		}
	}
	else {
		if (state < 2) {
			state++;
			Sclick.play();
		}
	}

	if (Active && (state != OldState))
		SwitchToggled = true;

	return true;
}

void ToggleSwitch::DoDrawSwitch(SURFHANDLE DrawSurface)

{
	if (state) {
		oapiBlt(DrawSurface, SwitchSurface, x, y, 0, 0, width, height);
	}
	else {
		oapiBlt(DrawSurface, SwitchSurface, x, y, width, 0, width, height);
	}
}


void ThreePosSwitch::DrawSwitch(SURFHANDLE DrawSurface)

{
	oapiBlt(DrawSurface, SwitchSurface, x, y, (state * width), 0, width, height);
}


void ToggleSwitch::DrawSwitch(SURFHANDLE DrawSurface)

{
	DoDrawSwitch(DrawSurface);
}

void ToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, VESSEL *v, SoundLib &s)

{
	x = xp;
	y = yp;
	width = w;
	height = h;
	SwitchSurface = surf;
	row.AddSwitch(this);
	OurVessel = v;
	SwitchToggled = false;

	if (!Sclick.isValid())
		s.LoadSound(Sclick, CLICK_SOUND);
}

void ToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s)

{
	x = xp;
	y = yp;
	width = w;
	height = h;
	SwitchSurface = surf;

	if (!Sclick.isValid())
		s.LoadSound(Sclick, CLICK_SOUND);
};


void ToggleSwitch::SetActive(bool s)

{
	Active = s;
}

//
// Attitude mode toggle switch.
//

bool AttitudeToggle::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (!DoCheckMouseClick(mx, my))
		return false;

	if (!Active || state == OldState)
		return true;

	if (state) {
		OurVessel->SetAttitudeMode(ATTMODE_ROT);
	}
	else {
		OurVessel->SetAttitudeMode(ATTMODE_LIN);
	}

	return true;
}

void AttitudeToggle::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (Active) {
		if (OurVessel->GetAttitudeMode() == ATTMODE_ROT) {
			state = true;
		}
		else {
			state = false;
		}
	}

	DoDrawSwitch(DrawSurface);
}

//
// HUD mode toggle switches.
//

void HUDToggle::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int mode, SoundLib &s)

{
	HUDMode = mode;
	ToggleSwitch::Init(xp, yp, w, h, surf, s);
	row.AddSwitch(this);
}

void HUDToggle::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (Active) {
		if (oapiGetHUDMode() == HUDMode) {
			state = true;
		}
		else {
			state = false;
		}
	}

	DoDrawSwitch(DrawSurface);
}

bool HUDToggle::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (!DoCheckMouseClick(mx, my))
		return false;

	if (!Active || state == OldState)
		return true;

	if (state) {
		oapiSetHUDMode(HUDMode);
	}
	else {
		oapiSetHUDMode(HUD_NONE) ;
	}

	return true;
}

//
// NAV mode toggle switches.
//

void NavModeToggle::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, VESSEL *v, int mode, SoundLib &s)

{
	NAVMode = mode;
	ToggleSwitch::Init(xp, yp, w, h, surf, row, v, s);
}

void NavModeToggle::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (Active) {
		if (OurVessel->GetNavmodeState(NAVMode)) {
			state = true;
		}
		else {
			state = false;
		}
	}

	DoDrawSwitch(DrawSurface);
}

bool NavModeToggle::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (!DoCheckMouseClick(mx, my))
		return false;

	if (!Active || state == OldState)
		return true;

	if (state) {
		OurVessel->ActivateNavmode(NAVMode);
	}
	else {
		OurVessel->DeactivateNavmode(NAVMode) ;
	}

	return true;
}

//
// Switch row support.
//

SwitchRow::SwitchRow()

{
	SwitchList = 0;
	RowList = 0;
	PanelArea = (-1);
}

SwitchRow::~SwitchRow()

{
}

bool SwitchRow::CheckMouseClick(int id, int event, int mx, int my)

{
	if (id != PanelArea)
		return false;

	ToggleSwitch *s = SwitchList;

	while (s) {
		if (s->CheckMouseClick(event, mx, my))
			return true;
		s = s->GetNext();
	}

	return false;
}

void SwitchRow::Init(int area, PanelSwitches &panel)

{
	SwitchList = 0;
	RowList = 0;
	PanelArea = area;
	panel.AddRow(this);
}

bool SwitchRow::DrawRow(int id, SURFHANDLE DrawSurface)

{
	if (id != PanelArea)
		return false;

	ToggleSwitch *s = SwitchList;

	while (s) {
		s->DrawSwitch(DrawSurface);
		s = s->GetNext();
	}

	return true;
}

//
// Panel of switches. This code wraps up a whole panel with multiple
// rows of switches, and passes redraw and mouse events to the appropriate
// switches.
//

bool PanelSwitches::CheckMouseClick(int id, int event, int mx, int my)

{
	SwitchRow *row = RowList;

	while (row) {
		if (row->CheckMouseClick(id, event, mx, my))
			return true;
		row = row->GetNext();
	}

	return false;
}

bool PanelSwitches::DrawRow(int id, SURFHANDLE DrawSurface)

{
	SwitchRow *row = RowList;

	while (row) {
		if (row->DrawRow(id, DrawSurface))
			return true;
		row = row->GetNext();
	}

	return false;
}
