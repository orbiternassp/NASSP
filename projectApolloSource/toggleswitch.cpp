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
  *	Revision 1.7  2005/06/06 12:30:27  tschachim
  *	Introduced PushSwitch, GuardedPushSwitch, PanelScenarioHandler
  *	
  *	Revision 1.6  2005/05/05 21:38:04  tschachim
  *	Introduced PanelSwitchItem and IndicatorSwitch
  *	Renamed some interfaces and functions
  *	
  *	Revision 1.5  2005/04/22 14:12:39  tschachim
  *	ToggleSwitch.Init changed
  *	SwitchListener introduced
  *	
  *	Revision 1.4  2005/04/01 15:38:34  tschachim
  *	Introduced RotationalSwitch
  *	
  *	Revision 1.3  2005/03/11 17:54:00  tschachim
  *	Introduced GuardedToggleSwitch and GuardedThreePosSwitch
  *	
  *	Revision 1.2  2005/03/09 21:59:30  tschachim
  *	Introduced visible flag
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
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

ToggleSwitch::ToggleSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	state = 0;
	springLoaded = SPRINGLOADEDSWITCH_NONE;

	next = 0;

	SwitchSurface = 0;
	OurVessel = 0;
	Active = true;
	visible = true;
}

ToggleSwitch::~ToggleSwitch() {
	Sclick.done();
}

void ToggleSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int springloaded) {

	name = n;
	state = defaultState;
	springLoaded = springloaded;
	scnh.RegisterSwitch(this);
}

void ToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int xoffset, int yoffset) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	xOffset = xoffset;
	yOffset = yoffset;
	SwitchSurface = surf;
	SwitchToggled = false;
	
	row.AddSwitch(this);
	switchRow = &row;
	OurVessel = switchRow->panelSwitches->vessel;

	InitSound(switchRow->panelSwitches->soundlib);
}

void ToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s, int xoffset, int yoffset) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	xOffset = xoffset;
	yOffset = yoffset;
	SwitchSurface = surf;

	InitSound(&s);
}

void ToggleSwitch::InitSound(SoundLib *s) {

	if (!Sclick.isValid())
		s->LoadSound(Sclick, CLICK_SOUND);
}


bool ToggleSwitch::DoCheckMouseClick(int event, int mx, int my) {

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

	if (event == PANEL_MOUSE_LBDOWN) {
		if (my > (y + (height / 2.0))) {
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
	} else if (springLoaded != SPRINGLOADEDSWITCH_NONE && event == PANEL_MOUSE_LBUP) {
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   state = TOGGLESWITCH_DOWN;
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     state = TOGGLESWITCH_UP;
	}


	if (Active && (state != OldState)) {
		SwitchToggled = true;
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelSwitchToggled(this);
		}
	}
	return true;
}

bool ToggleSwitch::CheckMouseClick(int event, int mx, int my) {

	if (visible)
		return DoCheckMouseClick(event, mx, my);
	else
		return false;
}

void ToggleSwitch::DoDrawSwitch(SURFHANDLE DrawSurface) {

	if (state) {
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset, yOffset, width, height, SURF_PREDEF_CK);
	} else {
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset + width, yOffset, width, height, SURF_PREDEF_CK);
	}
}

void ToggleSwitch::DrawSwitch(SURFHANDLE DrawSurface) {
	if (visible) 
		DoDrawSwitch(DrawSurface);
}

void ToggleSwitch::SetActive(bool s) {
	Active = s;
}

void ToggleSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state);
}

void ToggleSwitch::LoadState(char *line) {

	char buffer[100];
	int st;

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
	}
}


//
// Three pos switch.
//

bool ThreePosSwitch::CheckMouseClick(int event, int mx, int my) {

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

	if (event == PANEL_MOUSE_LBDOWN) {
		if (my > (y + (height / 2.0))) {
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
	} else if (springLoaded != SPRINGLOADEDSWITCH_NONE && event == PANEL_MOUSE_LBUP) {
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   state = THREEPOSSWITCH_DOWN;
		if (springLoaded == SPRINGLOADEDSWITCH_CENTER) state = THREEPOSSWITCH_CENTER;
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     state = THREEPOSSWITCH_UP;
	}

	if (Active && (state != OldState)) {
		SwitchToggled = true;
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelSwitchToggled(this);
		}
	}
	return true;
}

void ThreePosSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	oapiBlt(DrawSurface, SwitchSurface, x, y, (state * width), 0, width, height, SURF_PREDEF_CK);
}


//
// Push button like switch.
//

bool PushSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	if (!visible) return false;
	if (mx < x || my < y) return false;
	if (mx > (x + width) || my > (y + height)) return false;

	if (event == PANEL_MOUSE_LBDOWN) {
		state = 1;
		Sclick.play();
	} else if (event == PANEL_MOUSE_LBUP) {
		state = 0;
	}

	if (Active && (state != OldState)) {
		SwitchToggled = true;
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelSwitchToggled(this);
		}
	}
	return true;
}

void PushSwitch::InitSound(SoundLib *s) {

	if (!Sclick.isValid())
		s->LoadSound(Sclick, BUTTON_SOUND);
}

//
// Attitude mode toggle switch.
//

bool AttitudeToggle::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (!visible)
		return false;

	if (!DoCheckMouseClick(event, mx, my))
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
	if (!visible)
		return;
	
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

	if (!DoCheckMouseClick(event, mx, my))
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
	ToggleSwitch::Init(xp, yp, w, h, surf, row);
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

	if (!DoCheckMouseClick(event, mx, my))
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

SwitchRow::SwitchRow() {

	SwitchList = 0;
	RowList = 0;
	PanelArea = (-1);
}

SwitchRow::~SwitchRow() {
}

bool SwitchRow::CheckMouseClick(int id, int event, int mx, int my) {

	if (id != PanelArea)
		return false;

	PanelSwitchItem *s = SwitchList;
	while (s) {
		if (s->CheckMouseClick(event, mx, my))
			return true;
		s = s->GetNext();
	}
	return false;
}

void SwitchRow::Init(int area, PanelSwitches &panel) {

	SwitchList = 0;
	RowList = 0;
	PanelArea = area;
	panelSwitches = &panel;
	panel.AddRow(this);
}

bool SwitchRow::DrawRow(int id, SURFHANDLE DrawSurface) {

	if (id != PanelArea)
		return false;

	PanelSwitchItem *s = SwitchList;
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

bool PanelSwitches::CheckMouseClick(int id, int event, int mx, int my) {

	SwitchRow *row = RowList;

	while (row) {
		if (row->CheckMouseClick(id, event, mx, my))
			return true;
		row = row->GetNext();
	}

	return false;
}

bool PanelSwitches::DrawRow(int id, SURFHANDLE DrawSurface) {

	SwitchRow *row = RowList;

	while (row) {
		if (row->DrawRow(id, DrawSurface))
			return true;
		row = row->GetNext();
	}

	return false;
}

//
// Guarded toggle switch.
//

GuardedToggleSwitch::GuardedToggleSwitch() {
	
	guardX = 0;
	guardY = 0;
	guardWidth = 0;
	guardHeight = 0;
	guardSurface = 0;
	guardState = 0;
}

GuardedToggleSwitch::~GuardedToggleSwitch() {
	guardClick.done();
}

void GuardedToggleSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState) {

	ToggleSwitch::Register(scnh, n, defaultState);
	guardState = defaultGuardState;
}

void GuardedToggleSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, 
									int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;
	guardSurface = surf;
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		switchRow->panelSwitches->soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedToggleSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if(guardState) {
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		DoDrawSwitch(DrawSurface);
	} else {
		DoDrawSwitch(DrawSurface);
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
	}
}

bool GuardedToggleSwitch::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				guardState = 0;
				if (Active && state) SwitchToggled = true;
				state = 0;
			} else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	} else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return ToggleSwitch::CheckMouseClick(event, mx, my);
		}
	}
	return false;
}

void GuardedToggleSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i", state, guardState); 
	oapiWriteScenario_string(scn, name, buffer);
}

void GuardedToggleSwitch::LoadState(char *line) {
	
	char buffer[100];
	int st, gst;

	sscanf(line, "%s %i %i", buffer, &st, &gst); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
	}
}


//
// Guarded push switch.
//

GuardedPushSwitch::GuardedPushSwitch() {
	
	guardX = 0;
	guardY = 0;
	guardWidth = 0;
	guardHeight = 0;
	guardSurface = 0;
	guardState = 0;
}

GuardedPushSwitch::~GuardedPushSwitch() {
	guardClick.done();
}

void GuardedPushSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState) {

	PushSwitch::Register(scnh, n, defaultState);
	guardState = defaultGuardState;
}

void GuardedPushSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, 
									int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;
	guardSurface = surf;
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		switchRow->panelSwitches->soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedPushSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if(guardState) {
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		DoDrawSwitch(DrawSurface);
	} else {
		DoDrawSwitch(DrawSurface);
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
	}
}

bool GuardedPushSwitch::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				guardState = 0;
				if (Active && state) SwitchToggled = true;
				state = 0;
			} else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	} else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return PushSwitch::CheckMouseClick(event, mx, my);
		}
	}
	return false;
}

void GuardedPushSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i", state, guardState); 
	oapiWriteScenario_string(scn, name, buffer);
}

void GuardedPushSwitch::LoadState(char *line) {
	
	char buffer[100];
	int st, gst;

	sscanf(line, "%s %i %i", buffer, &st, &gst); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
	}
}


//
// Guarded three pos switch.
//

GuardedThreePosSwitch::GuardedThreePosSwitch() {
	
	guardX = 0;
	guardY = 0;
	guardWidth = 0;
	guardHeight = 0;
	guardSurface = 0;
	guardState = 0;
}

GuardedThreePosSwitch::~GuardedThreePosSwitch() {
	guardClick.done();
}

void GuardedThreePosSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState) {

	ThreePosSwitch::Register(scnh, n, defaultState);
	guardState = defaultGuardState;
}

void GuardedThreePosSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, 
									SoundLib &s, int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;
	guardSurface = surf;
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		s.LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedThreePosSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if(guardState) {
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight);
		ThreePosSwitch::DrawSwitch(DrawSurface);
	} else {
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight);
	}
}

bool GuardedThreePosSwitch::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				guardState = 0;
				if (Active && state) SwitchToggled = true;
				state = 0;
			} else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	} else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return ThreePosSwitch::CheckMouseClick(event, mx, my);
		}
	}
	return false;
}

void GuardedThreePosSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i", state, guardState); 
	oapiWriteScenario_string(scn, name, buffer);
}

void GuardedThreePosSwitch::LoadState(char *line) {
	
	char buffer[100];
	int st, gst;

	sscanf(line, "%s %i %i", buffer, &st, &gst); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
	}
}


//
// Rotational Switch
//

RotationalSwitch::RotationalSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	position = 0;
	positionList = 0;
	next = 0;
	switchSurface = 0;

	bitmaps[0].angle = 0;
	bitmaps[0].xOffset = 0;
	bitmaps[0].yOffset = 0;

	bitmaps[1].angle = 30;
	bitmaps[1].xOffset = 1;
	bitmaps[1].yOffset = 0;

	bitmaps[2].angle = 60;
	bitmaps[2].xOffset = 2;
	bitmaps[2].yOffset = 0;

	bitmaps[3].angle = 90;
	bitmaps[3].xOffset = 3;
	bitmaps[3].yOffset = 0;

	bitmaps[4].angle = 120;
	bitmaps[4].xOffset = 4;
	bitmaps[4].yOffset = 0;

	bitmaps[5].angle = 150;
	bitmaps[5].xOffset = 5;
	bitmaps[5].yOffset = 0;

	bitmaps[6].angle = 180;
	bitmaps[6].xOffset = 6;
	bitmaps[6].yOffset = 0;

	bitmaps[7].angle = 210;
	bitmaps[7].xOffset = 7;
	bitmaps[7].yOffset = 0;

	bitmaps[8].angle = 240;
	bitmaps[8].xOffset = 8;
	bitmaps[8].yOffset = 0;

	bitmaps[9].angle = 270;
	bitmaps[9].xOffset = 9;
	bitmaps[9].yOffset = 0;

	bitmaps[10].angle = 300;
	bitmaps[10].xOffset = 10;
	bitmaps[10].yOffset = 0;

	bitmaps[11].angle = 330;
	bitmaps[11].xOffset = 11;
	bitmaps[11].yOffset = 0;

	bitmaps[12].angle = 20;
	bitmaps[12].xOffset = 0;
	bitmaps[12].yOffset = 1;

	bitmaps[13].angle = 45;
	bitmaps[13].xOffset = 1;
	bitmaps[13].yOffset = 1;

	bitmaps[14].angle = 135;
	bitmaps[14].xOffset = 3;
	bitmaps[14].yOffset = 1;

	bitmaps[15].angle = 160;
	bitmaps[15].xOffset = 4;
	bitmaps[15].yOffset = 1;

	bitmaps[16].angle = 200;
	bitmaps[16].xOffset = 5;
	bitmaps[16].yOffset = 1;

	bitmaps[17].angle = 225;
	bitmaps[17].xOffset = 6;
	bitmaps[17].yOffset = 1;

	bitmaps[18].angle = 315;
	bitmaps[18].xOffset = 8;
	bitmaps[18].yOffset = 1;

	bitmaps[19].angle = 340;
	bitmaps[19].xOffset = 9;
	bitmaps[19].yOffset = 1;
}

RotationalSwitch::~RotationalSwitch() {

	DeletePositions();
	sclick.done();
}

void RotationalSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultValue) {
	
	name = n;
	SetValue(defaultValue);
	scnh.RegisterSwitch(this);
}

void RotationalSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	switchSurface = surf;
	
	row.AddSwitch(this);

	if (!sclick.isValid()) {
		row.panelSwitches->soundlib->LoadSound(sclick, CLICK_SOUND);
	}
}

void RotationalSwitch::AddPosition(int value, double angle) {

	RotationalSwitchPosition *p = new RotationalSwitchPosition(value, angle);
	p->SetNext(positionList);
	positionList = p;

	if (position == 0) {
		position = p;
	}
}

void RotationalSwitch::DeletePositions() {

	RotationalSwitchPosition *p1, *p = positionList; 
	while (p) {
		p1 = p->GetNext();
		delete p;
		p = p1;
	}

	position = 0;
	positionList = 0;
}

void RotationalSwitch::DrawSwitch(SURFHANDLE drawSurface) {

	if (position) {
		// Find best bitmap for position angle
		int index = -1;
		for (int i = 0; i < RotationalSwitchBitmapCount; i++) {
			if (index == -1) {
				index = i;
			} else if (AngleDiff(bitmaps[i].angle, position->GetAngle()) < AngleDiff(bitmaps[index].angle, position->GetAngle())) {
				index = i;
			}
		}
		oapiBlt(drawSurface, switchSurface, x, y, bitmaps[index].xOffset * width, bitmaps[index].yOffset * height, width, height, SURF_PREDEF_CK);
	}
}

bool RotationalSwitch::CheckMouseClick(int event, int mx, int my) {

	// Check whether it's actually in our switch region.
	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	// Calculate angle of click
	double angle = atan((mx - x - width / 2.0) / (height / 2.0 - my + y)) * DEG;
	if ((height / 2.0 - my + y) < 0.0) angle += 180.0;
	if (angle < 0.0) angle += 360.0;

	// Find closest position
	RotationalSwitchPosition *bestPosition = 0, *p = positionList; 
	while (p) {
		if (bestPosition == 0) {
			bestPosition = p;
		} else if (AngleDiff(p->GetAngle(), angle) < AngleDiff(bestPosition->GetAngle(), angle)) {
			bestPosition = p;
		}
		p = p->GetNext();
	}

	if (position != bestPosition) {
		position = bestPosition;
		sclick.play();
	}
	return true;
}

void RotationalSwitch::SetValue(int newValue) { 
	
	RotationalSwitchPosition *p = positionList; 
	while (p) {
		if (p->GetValue() == newValue) {
			position = p;
		}
		p = p->GetNext();
	}
}

double RotationalSwitch::AngleDiff(double a1, double a2) {

	double diff = fabs(a1 - a2);
	if (diff > 180.0) diff = 360.0 - diff;
	return diff;
}

int RotationalSwitch::operator=(const int b) { 
	
	SetValue(b);
	if (position)
		return position->GetValue(); 
	else
		return 0;
}

RotationalSwitch::operator int() {

	if (position) {
		return position->GetValue();
	} else {	
		return 0;
	}
}

void RotationalSwitch::SaveState(FILEHANDLE scn) {

	if (position) {
		oapiWriteScenario_int (scn, name, position->GetValue()); 
	}		
}

void RotationalSwitch::LoadState(char *line) {

	char buffer[100];
	int val;

	sscanf(line, "%s %i", buffer, &val); 
	if (!strnicmp(buffer, name, strlen(name))) {
		SetValue(val);
	}
}


RotationalSwitchPosition::RotationalSwitchPosition(int v, double a) {

	value = v;
	angle = a;
}

RotationalSwitchPosition::~RotationalSwitchPosition() {
}


//
// Indicator Switch
//

IndicatorSwitch::IndicatorSwitch() {

	state = false;
	displayState = 0.0;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	switchSurface = 0;
	switchRow = 0;
}

IndicatorSwitch::~IndicatorSwitch() {
}

void IndicatorSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, bool defaultState) {

	name = n;
	state = defaultState;
	scnh.RegisterSwitch(this);
}

void IndicatorSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	switchSurface = surf;
	
	row.AddSwitch(this);
	switchRow = &row;
}

bool IndicatorSwitch::CheckMouseClick(int event, int mx, int my) {

	return false;
}

void IndicatorSwitch::DrawSwitch(SURFHANDLE drawSurface) {

	if (switchRow) {
		if (switchRow->panelSwitches->listener) 
			switchRow->panelSwitches->listener->PanelIndicatorSwitchStateRequested(this);
	}

	if (state && displayState < 3.0)
		displayState += oapiGetSimStep() * 5.0;

	if (!state && displayState > 0.0) 
		displayState -= oapiGetSimStep() * 5.0;

	if (displayState > 3.0) displayState = 3.0;
	if (displayState < 0.0) displayState = 0.0;
	oapiBlt(drawSurface, switchSurface, x, y, width * (int)displayState, 0, width, height);
}

void IndicatorSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state); 
}

void IndicatorSwitch::LoadState(char *line) {

	char buffer[100];
	bool st;

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
	}
}


//
// Panel Switch Scenario Handler
//

void PanelSwitchScenarioHandler::RegisterSwitch(PanelSwitchItem *s) {

	s->SetNextForScenario(switchList); 
	switchList = s; 
}

void PanelSwitchScenarioHandler::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, PANELSWITCH_START_STRING);

 	PanelSwitchItem *s = switchList;
	while (s) {
		s->SaveState(scn);
		s = s->GetNextForScenario();
	}

	oapiWriteLine(scn, PANELSWITCH_END_STRING);
}

void PanelSwitchScenarioHandler::LoadState(FILEHANDLE scn) {

	char * line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, PANELSWITCH_END_STRING, strlen(PANELSWITCH_END_STRING)))
			return;

		PanelSwitchItem *s = switchList;
		while (s) {
			s->LoadState(line);
			s = s->GetNextForScenario();
		}
	}
}
