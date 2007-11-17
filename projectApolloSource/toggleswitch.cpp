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
  *	Revision 1.79  2007/10/21 21:25:13  movieman523
  *	Added SHIFT-click to hold spring-loaded buttons.
  *	
  *	Revision 1.78  2007/08/13 16:06:24  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.77  2007/07/17 14:33:10  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.76  2007/06/06 15:02:23  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.75  2007/01/22 14:47:38  tschachim
  *	Moved FDAIPowerRotationalSwitch to satswitches, horizontal thumbwheel.
  *	
  *	Revision 1.74  2006/11/24 22:42:44  dseagrav
  *	Enable changing bits in AGC channel 33, enable LEB optics switch, enable tracker switch as optics status debug switch.
  *	
  *	Revision 1.73  2006/11/13 14:47:34  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.72  2006/08/18 05:45:01  dseagrav
  *	LM EDS now exists. Talkbacks wired to a power source will revert to BP when they lose power.
  *	
  *	Revision 1.71  2006/07/24 06:41:30  dseagrav
  *	Many changes - Rearranged / corrected FDAI power usage, added LM AC equipment, many bugfixes
  *	
  *	Revision 1.70  2006/06/17 18:13:13  tschachim
  *	Moved BMAGPowerRotationalSwitch.
  *	
  *	Revision 1.69  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.68  2006/06/07 09:53:20  tschachim
  *	Improved ASCP and GDC align button, added cabin closeout sound, bugfixes.
  *	
  *	Revision 1.67  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.66  2006/05/30 14:40:21  tschachim
  *	Fixed fuel cell - dc bus connectivity, added battery charger
  *	
  *	Revision 1.65  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.64  2006/05/17 18:42:35  movieman523
  *	Partial fix for loading sound volume from scenario.
  *	
  *	Revision 1.63  2006/05/01 08:52:50  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.62  2006/04/25 13:54:07  tschachim
  *	Warning removed.
  *	
  *	Revision 1.61  2006/04/25 08:11:27  dseagrav
  *	Crash avoidance for DEBUG builds, LM IMU correction, LM still needs more work
  *	
  *	Revision 1.60  2006/04/17 18:20:11  movieman523
  *	Removed #if 0 code.
  *	
  *	Revision 1.59  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  *	Revision 1.58  2006/04/17 15:16:16  movieman523
  *	Beginnings of checklist code, added support for flashing borders around control panel switches and updated a portion of the Saturn panel switches appropriately.
  *	
  *	Revision 1.57  2006/04/12 06:27:19  dseagrav
  *	LM checkpoint commit. The LM is not airworthy at this point. Please be patient.
  *	
  *	Revision 1.56  2006/04/05 16:52:17  tschachim
  *	Bugfix MeterSwitch.
  *	
  *	Revision 1.55  2006/03/12 01:13:29  dseagrav
  *	Added lots of SCS items and FDAI stuff.
  *	
  *	Revision 1.54  2006/02/22 18:53:10  tschachim
  *	Bugfixes for Apollo 4-6.
  *	
  *	Revision 1.53  2006/02/21 23:22:06  quetalsi
  *	Bugfix in EVENT TIMER RESET/DOWN switch.
  *	
  *	Revision 1.52  2006/02/02 18:51:50  tschachim
  *	Bugfix.
  *	
  *	Revision 1.51  2006/02/01 18:17:30  tschachim
  *	Added TwoSourceSwitch::SwitchTo function.
  *	
  *	Revision 1.50  2006/01/26 03:07:50  movieman523
  *	Quick hack to support low-res mesh.
  *	
  *	Revision 1.49  2006/01/14 20:03:35  movieman523
  *	Fixed some switch bugs.
  *	
  *	Revision 1.48  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
  *	Revision 1.47  2006/01/11 22:34:21  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.46  2006/01/11 02:59:43  movieman523
  *	Valve talkbacks now check the valve state directlry. This means they barberpole on SM sep and can't then be changed.
  *	
  *	Revision 1.45  2006/01/10 19:34:45  movieman523
  *	Fixed AC bus switches and added ELS Logic/Auto support.
  *	
  *	Revision 1.44  2006/01/08 17:50:38  movieman523
  *	Wired up electrical meter switches other than battery charger.
  *	
  *	Revision 1.43  2006/01/08 04:00:24  movieman523
  *	Added first two engineering cameras.
  *	
  *	Revision 1.42  2006/01/07 19:11:44  tschachim
  *	Checklist actions for FDAIPowerRotarySwitch.
  *	
  *	Revision 1.41  2006/01/07 03:28:28  movieman523
  *	Removed a lot of unused switches and wired up the FDAI power switch.
  *	
  *	Revision 1.40  2006/01/07 00:43:58  movieman523
  *	Added non-essential buses, though there's nothing connected to them at the moment.
  *	
  *	Revision 1.39  2006/01/05 12:07:40  tschachim
  *	Bugfix
  *	
  *	Revision 1.38  2005/12/19 17:22:50  tschachim
  *	Realism mode, SwitchTo functions.
  *	
  *	Revision 1.37  2005/12/02 19:47:19  movieman523
  *	Replaced most PowerSource code with e_object.
  *	
  *	Revision 1.36  2005/12/02 19:29:24  movieman523
  *	Started integrating PowerSource code into PanelSDK.
  *	
  *	Revision 1.35  2005/11/20 21:46:31  movieman523
  *	Added initial volume control support.
  *	
  *	Revision 1.34  2005/11/17 23:32:46  movieman523
  *	Added support for specifying the maximum current for a circuit breaker. Exceed that current and the breaker pops.
  *	
  *	Revision 1.33  2005/11/17 01:34:25  movieman523
  *	Extended circuit breaker init function so it can be wired directly to a power source.
  *	
  *	Revision 1.32  2005/11/17 01:23:11  movieman523
  *	Revised circuit breaker code. Now all switchers are PowerSources, so no need for the seperate PowerBreaker class.
  *	
  *	Revision 1.31  2005/11/16 23:50:31  movieman523
  *	More updates to CWS operation. Still not completely correct, but closer.
  *	
  *	Revision 1.30  2005/11/16 23:14:02  movieman523
  *	Initial support for wiring in the circuit breakers.
  *	
  *	Revision 1.29  2005/11/15 05:44:45  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.28  2005/10/31 10:29:38  tschachim
  *	Added CircuitBrakerSwitch and ThumbwheelSwitch.
  *	
  *	Revision 1.27  2005/10/19 11:45:15  tschachim
  *	Update MeterSwitch.DisplayValue when requested (and not only when displayed).
  *	
  *	Revision 1.26  2005/10/11 16:48:31  tschachim
  *	Enhanced guard handling, SwitchTo functions added, bugfixes.
  *	
  *	Revision 1.25  2005/09/30 11:26:47  tschachim
  *	Added new spring-loaded modes, new event handler, added MeterSwitch.
  *	
  *	Revision 1.24  2005/08/25 18:40:24  movieman523
  *	Fixed talkback initialisation from scenario file.
  *	
  *	Revision 1.23  2005/08/24 23:29:31  movieman523
  *	Fixed event timer reset.
  *	
  *	Revision 1.22  2005/08/23 20:13:12  movieman523
  *	Added RCS talkbacks and changed AGC to use octal addresses for EMEM.
  *	
  *	Revision 1.21  2005/08/20 12:24:02  movieman523
  *	Added a FailedState as well as a Failed flag to each switch. You now must call GetState(), IsUp(), IsDown() etc to get the functional state of the switch. The state variable now only tells you the visual state (e.g. switch up, down, center) and not the functional state.
  *	
  *	Revision 1.20  2005/08/19 21:33:20  movieman523
  *	Added initial random failure support.
  *	
  *	Revision 1.19  2005/08/18 19:12:21  movieman523
  *	Added Event Timer switches and null Event Timer class.
  *	
  *	Revision 1.18  2005/08/17 22:54:26  movieman523
  *	Added ELS and CM RCS switches.
  *	
  *	Revision 1.17  2005/08/16 11:46:58  tschachim
  *	Fixed rotational switch because of new bitmap.
  *	
  *	Revision 1.16  2005/08/13 20:20:17  movieman523
  *	Created MissionTimer class and wired it into the LEM and CSM.
  *	
  *	Revision 1.15  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
  *	Revision 1.14  2005/08/13 14:21:36  movieman523
  *	Added beginnings of caution and warning system.
  *	
  *	Revision 1.13  2005/08/13 11:48:27  movieman523
  *	Added remaining caution and warning switches to CSM (currently not wired up to anything).
  *	
  *	Revision 1.12  2005/08/13 00:09:43  movieman523
  *	Added IMU Cage switch
  *	
  *	Revision 1.11  2005/08/12 23:15:49  movieman523
  *	Added switches to update mission time display.
  *	
  *	Revision 1.10  2005/08/05 13:13:27  tschachim
  *	Indicator change speed faster, bugfix
  *	
  *	Revision 1.9  2005/07/30 16:10:44  tschachim
  *	Added SwitchTo function to simulate switch usage.
  *	
  *	Revision 1.8  2005/07/05 17:59:00  tschachim
  *	Introduced spring-loaded switches
  *	
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
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"

#include "IMU.h"
#include "missiontimer.h"
#include "apolloguidance.h"
#include "powersource.h"
#include "fdai.h"
#include "scs.h"

// DS20060304 SCS PANEL OBJECTS

//
// Generic panel switch item.
//

PanelSwitchItem::PanelSwitchItem()

{
	Failed = false;
	FailedState = 0;

	name = 0;
	next = 0;
	nextForScenario = 0;

	DisplayName = 0;
	flashing = false;
	visible = true;
}

char *PanelSwitchItem::GetDisplayName()

{
	if (DisplayName)
		return DisplayName;

	return name;
}

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
	BorderSurface = 0;

	OurVessel = 0;
	switchRow = 0;

	Active = true;
	Held = false;
}

ToggleSwitch::~ToggleSwitch() {
	Sclick.done();
}

void ToggleSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int springloaded, char *dname) {

	name = n;
	state = defaultState;
	springLoaded = springloaded;
	scnh.RegisterSwitch(this);

	DisplayName = dname;
}

void ToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset)

{
	x = xp;
	y = yp;
	width = w;
	height = h;
	xOffset = xoffset;
	yOffset = yoffset;

	SwitchSurface = surf;
	BorderSurface = bsurf;
	
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

bool ToggleSwitch::SwitchTo(int newState) {

	// Switch only with REALISM 0
	if (switchRow) {
		if (switchRow->panelSwitches->Realism) 
			return false;
	}

	if (Active)
	{
		if (state != newState)
		{
			state = newState;
			Sclick.play();
			SwitchToggled = true;
			if (switchRow)
			{
				if (switchRow->panelSwitches->listener) 
					switchRow->panelSwitches->listener->PanelSwitchToggled(this);
			}

			//
			// Reset the switch if it's spring-loaded and not held.
			//
			if (IsSpringLoaded() && !IsHeld())
			{
				if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   state = TOGGLESWITCH_DOWN;
				if (springLoaded == SPRINGLOADEDSWITCH_UP)     state = TOGGLESWITCH_UP;
			}
		}
		return true;
	}
	return false;
}

unsigned int ToggleSwitch::GetFlags()

{
	ToggleSwitchFlags fs;

	fs.Held = Held ? 1 : 0;

	return fs.flags;
}

void ToggleSwitch::SetFlags(unsigned int f)

{
	ToggleSwitchFlags fs;

	fs.flags = f;

	Held = (fs.Held != 0);
}

//
// Get the state. If it's failed, return the failed state.
//
// Note that you should always call this to get the state unless you're actually concerned with
// the appearance of the switch rather than the functionality.
//

int ToggleSwitch::GetState()

{
	if (!Failed)
		return state;

	return FailedState;
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

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//

	if (event == PANEL_MOUSE_LBDOWN) {
		if (my > (y + (height / 2.0))) {
			if (state != TOGGLESWITCH_DOWN) {
				state = TOGGLESWITCH_DOWN;
				Sclick.play();
			}
		}
		else {
			if (state != TOGGLESWITCH_UP) {
				state = TOGGLESWITCH_UP;
				Sclick.play();
			}
		}
	}
	else if (IsSpringLoaded() && (event & PANEL_MOUSE_LBUP) && !IsHeld()) {
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

void ToggleSwitch::DoDrawSwitch(SURFHANDLE DrawSurface)

{
	if (IsUp())
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset, yOffset, width, height, SURF_PREDEF_CK);
	}
	else
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset + width, yOffset, width, height, SURF_PREDEF_CK);
	}
}

void ToggleSwitch::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (visible) 
		DoDrawSwitch(DrawSurface);
}

//
// Generic function to draw a flashing box around the switch. This is only called if the
// flashing is currently active.
//

void ToggleSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (BorderSurface)
		oapiBlt(DrawSurface, BorderSurface, x, y, 0, 0, width, height, SURF_PREDEF_CK);
}

void ToggleSwitch::SetActive(bool s) {
	Active = s;
}

void ToggleSwitch::SaveState(FILEHANDLE scn)

{
	char buffer[1000];

	sprintf(buffer, "%i %u", state, GetFlags()); 
	oapiWriteScenario_string(scn, name, buffer);
}

void ToggleSwitch::LoadState(char *line)
{
	// Load state
	char buffer[100];
	int st = 0;
	unsigned int f = 0;

	sscanf(line, "%s %i %u", buffer, &st, &f);
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		SetFlags(f);
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

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

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

	}
	else if (IsSpringLoaded() && event == PANEL_MOUSE_LBUP && !IsHeld()) {		
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   state = THREEPOSSWITCH_DOWN;
		if (springLoaded == SPRINGLOADEDSWITCH_CENTER) state = THREEPOSSWITCH_CENTER;
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     state = THREEPOSSWITCH_UP;

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)     
			state = THREEPOSSWITCH_CENTER;

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)     
			state = THREEPOSSWITCH_CENTER;
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

void ThreePosSwitch::DrawSwitch(SURFHANDLE DrawSurface)

{
	oapiBlt(DrawSurface, SwitchSurface, x, y, (state * width), 0, width, height, SURF_PREDEF_CK);
}

bool ThreePosSwitch::SwitchTo(int newState)

{
	// Switch only with REALISM 0
	if (switchRow) {
		if (switchRow->panelSwitches->Realism) 
			return false;
	}

	if (Active && (state != newState)) {
		state = newState;
		Sclick.play();
		SwitchToggled = true;
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelSwitchToggled(this);
		}

		if (IsSpringLoaded()) {
			if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   state = THREEPOSSWITCH_DOWN;
			if (springLoaded == SPRINGLOADEDSWITCH_CENTER) state = THREEPOSSWITCH_CENTER;
			if (springLoaded == SPRINGLOADEDSWITCH_UP)     state = THREEPOSSWITCH_UP;

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)     
				state = THREEPOSSWITCH_CENTER;

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)     
				state = THREEPOSSWITCH_CENTER;
		}
		return true;
	}
	return false;
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
// Circuit braker switch.
//

bool CircuitBrakerSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	if (!visible) return false;
	if (mx < x || my < y) return false;
	if (mx > (x + width) || my > (y + height)) return false;

	if (event == PANEL_MOUSE_LBDOWN) {
		if (state) {
			state = 0;
			Sclick.play();
		} else {
			state = 1;
			Sclick.play();
		}
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

double CircuitBrakerSwitch::Voltage()

{
	if ((state != 0) && SRC)
		return SRC->Voltage();

	return 0.0;
}

void CircuitBrakerSwitch::InitSound(SoundLib *s) {

	if (!Sclick.isValid())
		s->LoadSound(Sclick, CIRCUITBREAKER_SOUND);
}

void CircuitBrakerSwitch::DrawPower(double watts)

{
	//
	// Do nothing if the breaker is open.
	//

	if (state == 0){
		return;
	}

	//
	// Check the current isn't over the max rating, and if it is then
	// pop the breaker.
	//

	if (SRC) {
		double volts = SRC->Voltage();
		if (volts > 0.0) {
			double amps = watts / volts;
			if (amps > MaxAmps) {
				// sprintf(oapiDebugString(),"CB: Break! Amps = %f MaxAmps = %f",amps,MaxAmps);
				state = 0;
				SwitchToggled = true;

				if (switchRow) {
					if (switchRow->panelSwitches->listener) 
						switchRow->panelSwitches->listener->PanelSwitchToggled(this);
				}
				return;
			}
		}
		e_object::DrawPower(watts);
	}
}

void CircuitBrakerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s, double amps)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	if (s) {
		WireTo(s);
	}

	MaxAmps = amps;
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

	if (GetState()) {
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

void HUDToggle::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int mode, SoundLib &s)

{
	HUDMode = mode;
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
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

	if (GetState()) {
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

void NavModeToggle::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, VESSEL *v, int mode, SoundLib &s)

{
	NAVMode = mode;
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
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

	if (GetState()) {
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

	RowPower = 0;
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

void SwitchRow::AddSwitch(PanelSwitchItem *s)

{
	s->SetNext(SwitchList); 
	SwitchList = s;

	//
	// If we have power, wire it to the switch. Unless someone's already connected it
	// to another power source.
	//
	if (RowPower && !(s->IsWired()))
		s->WireTo(RowPower);
}

void SwitchRow::Init(int area, PanelSwitches &panel, e_object *p) {

	SwitchList = 0;
	RowList = 0;
	PanelArea = area;
	panelSwitches = &panel;
	RowPower = p;

	panel.AddRow(this);
}

bool SwitchRow::DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn) {

	if (id != PanelArea)
		return false;

	PanelSwitchItem *s = SwitchList;
	while (s) {
		s->DrawSwitch(DrawSurface);
		if (FlashOn && s->IsFlashing())
			s->DrawFlash(DrawSurface);
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

bool PanelSwitches::DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn) {

	SwitchRow *row = RowList;

	while (row) {
		if (row->DrawRow(id, DrawSurface, FlashOn))
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
	guardResetsState = true;
}

GuardedToggleSwitch::~GuardedToggleSwitch() {
	guardClick.done();
}

void GuardedToggleSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, int springloaded) {

	ToggleSwitch::Register(scnh, n, defaultState, springloaded);
	guardState = defaultGuardState;
}

void GuardedToggleSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf,
									int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;

	guardSurface = surf;
	guardBorder = bsurf;
	
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		switchRow->panelSwitches->soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedToggleSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if (guardResetsState) { 
		if (guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			DoDrawSwitch(DrawSurface);
		} else {
			DoDrawSwitch(DrawSurface);
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		}
	} else {
		if (guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			DoDrawSwitch(DrawSurface);
		} else {
			if (state) {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + 3 * guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			} else {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			}
		}
	}
}


void GuardedToggleSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (!guardState && guardBorder)
		oapiBlt(DrawSurface, guardBorder, guardX, guardY, 0, 0, guardWidth, guardHeight, SURF_PREDEF_CK);
	else
		ToggleSwitch::DrawFlash(DrawSurface);
}

bool GuardedToggleSwitch::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				guardState = 0;

				int OldState = state;
				// reset by guard
				if (guardResetsState) { 
					if (Active && state) {
						state = TOGGLESWITCH_DOWN;
					}
				}
				// reset by spring
				if (IsSpringLoaded()) {
					if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   state = TOGGLESWITCH_DOWN;
					if (springLoaded == SPRINGLOADEDSWITCH_UP)     state = TOGGLESWITCH_UP;
				}

				if (state != OldState) {
					SwitchToggled = true;
					if (switchRow) {
						if (switchRow->panelSwitches->listener) 
							switchRow->panelSwitches->listener->PanelSwitchToggled(this);
					}
				}
			} 
			else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	}
	else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
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
	guardBorder = 0;
	guardState = 0;

	lit = false;
}

void GuardedPushSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, int lxoffset, int lyoffset)

{
	litOffsetX = lxoffset;
	litOffsetY = lyoffset;

	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset);
}

GuardedPushSwitch::~GuardedPushSwitch() {
	guardClick.done();
}

void GuardedPushSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState) {

	PushSwitch::Register(scnh, n, defaultState);
	guardState = defaultGuardState;
}

void GuardedPushSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf,
									int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;

	guardSurface = surf;
	guardBorder = bsurf;
	
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		switchRow->panelSwitches->soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedPushSwitch::DoDrawSwitch(SURFHANDLE DrawSurface)

{
	if (lit)
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, litOffsetX, litOffsetY, width, height, SURF_PREDEF_CK);
	}
	else
		ToggleSwitch::DoDrawSwitch(DrawSurface);
}

void GuardedPushSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if(guardState)
	{
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		DoDrawSwitch(DrawSurface);
	}
	else
	{
		DoDrawSwitch(DrawSurface);
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
	}
}

void GuardedPushSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (!guardState && guardBorder)
		oapiBlt(DrawSurface, guardBorder, guardX, guardY, 0, 0, guardWidth, guardHeight, SURF_PREDEF_CK);
	else
		ToggleSwitch::DrawFlash(DrawSurface);
}

bool GuardedPushSwitch::CheckMouseClick(int event, int mx, int my)

{

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				guardState = 0;
				if (Active && state) SwitchToggled = true;
				state = 0;
			}
			else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	}
	else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return PushSwitch::CheckMouseClick(event, mx, my);
		}
	}
	return false;
}

void GuardedPushSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i %d", state, guardState, lit ? 1 : 0); 
	oapiWriteScenario_string(scn, name, buffer);
}

void GuardedPushSwitch::LoadState(char *line) {
	
	char buffer[100];
	int st, gst, l = 0;

	//
	// Note we set l to zero before the sscanf call to allow backward
	// compatibility with old scenarios: default to unlit if the lit
	// state isn't saved in the file.
	//
	sscanf(line, "%s %i %i %d", buffer, &st, &gst, &l); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
		lit = (l != 0);
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

void GuardedThreePosSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, 
									 int springloaded) {

	ThreePosSwitch::Register(scnh, n, defaultState, springloaded);
	guardState = defaultGuardState;
}

void GuardedThreePosSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, 
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

void GuardedThreePosSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if (guardResetsState) { 
		if(guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			ThreePosSwitch::DrawSwitch(DrawSurface);
		} else {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		}
	} else {
		if (guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			ThreePosSwitch::DrawSwitch(DrawSurface);
		} else {
			if (state == THREEPOSSWITCH_DOWN) {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			} else if (state == THREEPOSSWITCH_CENTER) {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + 2 * guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			} else {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + 3 * guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			}
		}
	}
}

bool GuardedThreePosSwitch::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				guardState = 0;

				int OldState = state;
				// reset by guard
				if (guardResetsState) { 
					if (Active && state) {
						state = THREEPOSSWITCH_DOWN;
					}
				}
				// reset by spring
				if (IsSpringLoaded()) {
					if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   state = THREEPOSSWITCH_DOWN;
					if (springLoaded == SPRINGLOADEDSWITCH_CENTER) state = THREEPOSSWITCH_CENTER;
					if (springLoaded == SPRINGLOADEDSWITCH_UP)     state = THREEPOSSWITCH_UP;

					if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)     
						state = THREEPOSSWITCH_CENTER;

					if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)     
						state = THREEPOSSWITCH_CENTER;
				}

				if (state != OldState) {
					SwitchToggled = true;
					if (switchRow) {
						if (switchRow->panelSwitches->listener) 
							switchRow->panelSwitches->listener->PanelSwitchToggled(this);
					}
				}
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
	switchBorder = 0;
	
	switchRow = 0;

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

	bitmaps[14].angle = 70;
	bitmaps[14].xOffset = 2;
	bitmaps[14].yOffset = 1;

	bitmaps[15].angle = 110;
	bitmaps[15].xOffset = 3;
	bitmaps[15].yOffset = 1;

	bitmaps[16].angle = 135;
	bitmaps[16].xOffset = 4;
	bitmaps[16].yOffset = 1;

	bitmaps[17].angle = 160;
	bitmaps[17].xOffset = 5;
	bitmaps[17].yOffset = 1;

	bitmaps[18].angle = 200;
	bitmaps[18].xOffset = 6;
	bitmaps[18].yOffset = 1;

	bitmaps[19].angle = 225;
	bitmaps[19].xOffset = 7;
	bitmaps[19].yOffset = 1;

	bitmaps[20].angle = 250;
	bitmaps[20].xOffset = 8;
	bitmaps[20].yOffset = 1;

	bitmaps[21].angle = 290;
	bitmaps[21].xOffset = 9;
	bitmaps[21].yOffset = 1;

	bitmaps[22].angle = 315;
	bitmaps[22].xOffset = 10;
	bitmaps[22].yOffset = 1;

	bitmaps[23].angle = 340;
	bitmaps[23].xOffset = 11;
	bitmaps[23].yOffset = 1;
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

void RotationalSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;

	switchSurface = surf;
	switchBorder = bsurf;
	
	row.AddSwitch(this);
	switchRow = &row;

	if (!sclick.isValid()) {
		row.panelSwitches->soundlib->LoadSound(sclick, ROTARY_SOUND);
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

void RotationalSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (switchBorder)
		oapiBlt(DrawSurface, switchBorder, x, y, 0, 0, width, height, SURF_PREDEF_CK);
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
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelRotationalSwitchChanged(this);
		}
	}
	return true;
}

bool RotationalSwitch::SwitchTo(int newValue) {

	// Switch only with REALISM 0
	if (switchRow) {
		if (switchRow->panelSwitches->Realism) 
			return false;
	}

	if (!position || (position->GetValue() != newValue)) {
		SetValue(newValue);
		sclick.play();
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelRotationalSwitchChanged(this);
		}
		return true;
	}
	return false;
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

int RotationalSwitch::GetState() {

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


PowerStateRotationalSwitch::PowerStateRotationalSwitch()

{
	int i;

	for (i = 0; i < 16; i++)
		sources[i] = 0;
}

void PowerStateRotationalSwitch::CheckPowerState()

{
	WireTo(sources[GetState()]);
}

bool PowerStateRotationalSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (RotationalSwitch::CheckMouseClick(event, mx, my)) {
		CheckPowerState();
		return true;
	}

	return false;
}

void PowerStateRotationalSwitch::SetSource(int num, e_object *s)

{
	if (num >= 0 && num < 16)
		sources[num] = s; 

	CheckPowerState();
}

bool PowerStateRotationalSwitch::SwitchTo(int newValue)

{
	if (RotationalSwitch::SwitchTo(newValue)) {
		CheckPowerState();
		return true;
	}

	return false;
}

void PowerStateRotationalSwitch::LoadState(char *line)

{
	RotationalSwitch::LoadState(line);
	CheckPowerState();
}

//
// Thumbwheel Switch
//

ThumbwheelSwitch::ThumbwheelSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	state = 0;
	maxState = 0;
	switchSurface = 0;
	switchRow = 0;
}

ThumbwheelSwitch::~ThumbwheelSwitch() {
	sclick.done();
}

void ThumbwheelSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState) {

	Register(scnh, n, defaultState, maximumState, false);
}

void ThumbwheelSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState, bool horizontal) {

	name = n;
	state = defaultState;
	maxState = maximumState;
	isHorizontal = horizontal;
	scnh.RegisterSwitch(this);
}

void ThumbwheelSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	switchSurface = surf;
	
	row.AddSwitch(this);
	switchRow = &row;

	if (!sclick.isValid()) {
		row.panelSwitches->soundlib->LoadSound(sclick, THUMBWHEEL_SOUND);
	}
}

int ThumbwheelSwitch::GetState() {

	return state;
}

bool ThumbwheelSwitch::CheckMouseClick(int event, int mx, int my) {

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
		if (isHorizontal) {
			if (mx < (x + (width / 2.0))) {
				if (state < maxState) {
					state++;
					sclick.play();
				}
			}
			else {
				if (state > 0) {
					state--;
					sclick.play();
				}
			}
		} else {
			if (my < (y + (height / 2.0))) {
				if (state < maxState) {
					state++;
					sclick.play();
				}
			}
			else {
				if (state > 0) {
					state--;
					sclick.play();
				}
			}
		}
	}

	if (state != OldState) {
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelThumbwheelSwitchChanged(this);
		}
	}
	return true;
}

bool ThumbwheelSwitch::SwitchTo(int newState) {

	// Switch only with REALISM 0
	if (switchRow) {
		if (switchRow->panelSwitches->Realism) 
			return false;
	}

	if (newState >= 0 && newState <= maxState && state != newState) {
		state = newState;
		sclick.play();
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelThumbwheelSwitchChanged(this);
		}
		return true;
	}
	return false;
}

void ThumbwheelSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	oapiBlt(DrawSurface, switchSurface, x, y, state * width, 0, width, height, SURF_PREDEF_CK);
}

void ThumbwheelSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state);
}

void ThumbwheelSwitch::LoadState(char *line) {

	char buffer[100];
	int st;

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
	}
}

//
// Thumbwheel which adjusts volume levels.
//

VolumeThumbwheelSwitch::VolumeThumbwheelSwitch()

{
	sl = 0;
	volume_class = -1;
}

void VolumeThumbwheelSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vclass, SoundLib *s)

{
	ThumbwheelSwitch::Init(xp, yp, w, h, surf, row);
	sl = s;
	volume_class = vclass;

	if (sl) {
		sl->SetVolume(volume_class, (int) (state * (100.0 / 9.0)));
	}
}

//
// We override CheckMouseClick so we can update the volume on change.
//

bool VolumeThumbwheelSwitch::CheckMouseClick(int event, int mx, int my)

{
	bool ret = ThumbwheelSwitch::CheckMouseClick(event, mx, my);

	if (ret && sl) {
		sl->SetVolume(volume_class, (int) (state * (100.0 / 9.0)));
	}

	return ret;
}

//
// We override LoadState() so we can update the volume on loading.
//

void VolumeThumbwheelSwitch::LoadState(char *line)

{
	ThumbwheelSwitch::LoadState(line);

	if (sl) {
		sl->SetVolume(volume_class, (int) (state * (100.0 / 9.0)));
	}
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
	if (state) 
		displayState = 3.0;
	else
		displayState = 0.0;

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

	int drawState=0;
	if (switchRow) {
		if (switchRow->panelSwitches->listener) 
			switchRow->panelSwitches->listener->PanelIndicatorSwitchStateRequested(this);
	}

	// Require power if wired
	if(SRC != NULL){
		if(SRC->Voltage() > 0){
			drawState = GetState();
		}else{
			drawState = 0;
		}
	}else{
		drawState = GetState();
	}

	if (drawState && displayState < 3.0)
		displayState += oapiGetSimStep() * 4.0;

	if (!drawState && displayState > 0.0) 
		displayState -= oapiGetSimStep() * 4.0;

	if (displayState > 3.0) displayState = 3.0;
	if (displayState < 0.0) displayState = 0.0;

	// Cheating beyond normal saves switch subclasses and associated etcetera
	if (displayState == 3.0 && drawState > 1){
		displayState += (drawState - 1);
	}

	oapiBlt(drawSurface, switchSurface, x, y, width * (int)displayState, 0, width, height);
}

void IndicatorSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state); 
}

void IndicatorSwitch::LoadState(char *line) {

	char buffer[100];
	int st = FALSE; // Avoids crash bug

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		if (state != 0){
			displayState = 3.0;
		}else{
			displayState = 0.0;
		}
		// Cheating beyond normal saves switch subclasses and associated etcetera
		if (displayState == 3.0 && state > 1){
			displayState += (state - 1);
		}
	}
}


//
// Meter Switch
//

MeterSwitch::MeterSwitch() {

	value = 0;
	displayValue = 0;
	minValue = 0;
	maxValue = 0;
	switchRow = 0;
	lastDrawTime = -1;
}

MeterSwitch::~MeterSwitch() {
}

void MeterSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time) {

	Register(scnh, n, min, max, time, min);
}

void MeterSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time, double defaultValue) {

	name = n;
	value = defaultValue;
	displayValue = defaultValue;
	minValue = min;
	maxValue = max;
	minMaxTime = time;
	scnh.RegisterSwitch(this);
}

void MeterSwitch::Init(SwitchRow &row) {

	row.AddSwitch(this);
	switchRow = &row;
}

bool MeterSwitch::CheckMouseClick(int event, int mx, int my) {

	return false;
}

void MeterSwitch::DrawSwitch(SURFHANDLE drawSurface) {

	DoDrawSwitch(GetDisplayValue(), drawSurface);
}

double MeterSwitch::GetDisplayValue() {

	if (lastDrawTime == -1) {
		lastDrawTime = oapiGetSysTime(); // oapiGetSimTime();
		return displayValue;
	}

	value = AdjustForPower(QueryValue());
	if (value > maxValue) value = maxValue;
	if (value < minValue) value = minValue;

	if (minMaxTime == 0) {
		displayValue = value;
	} else {
		double dt = oapiGetSysTime() - lastDrawTime; // oapiGetSimTime() - lastDrawTime;
		if (dt > 0) {
			if (fabs(value - displayValue) / dt > (maxValue - minValue) / minMaxTime) {
				displayValue += ((value - displayValue) / fabs(value - displayValue)) * (maxValue - minValue) / minMaxTime * dt;
			} else {
				displayValue = value;
			}
		}
	}
	lastDrawTime = oapiGetSysTime(); // oapiGetSimTime();
	return displayValue;
}

//
// Adjust the input value based on the voltage fed to the meter.
// CWS limit for main bus undervolt is 26.25 V, so we begin at 26 V 
// with the adjustment
//

double MeterSwitch::AdjustForPower(double val)

{
	double volts = Voltage();

	if (volts >= 26.0)
		return val;

	// sprintf(oapiDebugString(), "MeterSwitch::AdjustForPower %s undervolt", name);
	return ((val * (volts - 1.0)) / 25.0);
}

void MeterSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%lf %lf", value, displayValue); 
	oapiWriteScenario_string(scn, name, buffer);
}

void MeterSwitch::LoadState(char *line) {

	char buffer[100];
	double v, dv;

	sscanf(line, "%s %lf %lf", buffer, &v, &dv); 
	if (!strnicmp(buffer, name, strlen(name))) {
		value = v;
		displayValue = dv;
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

PanelSwitchItem* PanelSwitchScenarioHandler::GetSwitch(char *name) {

	PanelSwitchItem *s = switchList;
	while (s) {
		if (!stricmp(s->GetName(), name)) 
			return s;
		s = s->GetNextForScenario();
	}
	return 0;
}


void MissionTimerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, MissionTimer *ptimer)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	timer = ptimer;
}

//
// ThreeSourceSwitch allows you to connect the output to one of three inputs based on the position
// of the switch.
//

void ThreeSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source1 = s1;
	source2 = s2;
	source3 = s3;

	UpdateSourceState();
}

bool ThreeSourceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ThreePosSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}

bool ThreeSourceSwitch::SwitchTo(int newState)

{
	if (ToggleSwitch::SwitchTo(newState)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

void ThreeSourceSwitch::UpdateSourceState()

{
	if (IsUp()) {
		WireTo(source1);
	}
	else if (IsCenter()) {
		WireTo(source2);
	}
	else if (IsDown()) {
		WireTo(source3);
	}
}

void ThreeSourceSwitch::LoadState(char *line)

{
	ThreePosSwitch::LoadState(line);
	UpdateSourceState();
}

//
// TwoSourceSwitch allows you to connect the output to one of two inputs based on the position
// of the switch.
//

void TwoSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source1 = s1;
	source2 = s2;

	UpdateSourceState();
}

bool TwoSourceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}

bool TwoSourceSwitch::SwitchTo(int newState)

{
	if (ToggleSwitch::SwitchTo(newState)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

void TwoSourceSwitch::UpdateSourceState()

{
	if (IsUp()) {
		WireTo(source1);
	}
	else if (IsDown()) {
		WireTo(source2);
	}
}

void TwoSourceSwitch::LoadState(char *line)

{
	ToggleSwitch::LoadState(line);
	UpdateSourceState();
}

//
// GuardedTwoSourceSwitch allows you to connect the output to one of two inputs based on the position
// of the switch.
//

void GuardedTwoSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2)

{
	GuardedToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source1 = s1;
	source2 = s2;

	UpdateSourceState();
}

bool GuardedTwoSourceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (GuardedToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}

bool GuardedTwoSourceSwitch::SwitchTo(int newState)

{
	if (GuardedToggleSwitch::SwitchTo(newState)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

void GuardedTwoSourceSwitch::UpdateSourceState()

{
	if (IsUp()) {
		WireTo(source1);
	}
	else if (IsDown()) {
		WireTo(source2);
	}
}

void GuardedTwoSourceSwitch::LoadState(char *line)

{
	GuardedToggleSwitch::LoadState(line);
	UpdateSourceState();
}

//
// TwoOutputSwitch allows you to connect one of the two outputs to the input based on the position
// of the switch.
//

void TwoOutputSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	output1 = o1;
	output2 = o2;

	UpdateSourceState();
}

bool TwoOutputSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}

void TwoOutputSwitch::UpdateSourceState()

{
	if (IsUp()) {
		if (output1)
			output1->WireTo(this);
		if (output2)
			output2->WireTo(0);
	}
	else if (IsDown()) {
		if (output1)
			output1->WireTo(0);
		if (output2)
			output2->WireTo(this);
	}
}

void TwoOutputSwitch::LoadState(char *line)

{
	ToggleSwitch::LoadState(line);
	UpdateSourceState();
}

//
// GuardedTwoOutputSwitch allows you to connect one of the two outputs to the input based on the position
// of the switch.
//

void GuardedTwoOutputSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2)

{
	GuardedToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	output1 = o1;
	output2 = o2;

	UpdateSourceState(GetState());
}

bool GuardedTwoOutputSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (GuardedToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState(GetState());
		return true;
	}

	return false;
}

bool GuardedTwoOutputSwitch::SwitchTo(int newState)

{
	if (GuardedToggleSwitch::SwitchTo(newState))
	{
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		UpdateSourceState(newState);
		return true;
	}

	return false;
}

void GuardedTwoOutputSwitch::UpdateSourceState(int newState)

{
	if (newState == TOGGLESWITCH_UP) {
		if (output1)
			output1->WireTo(this);
		if (output2)
			output2->WireTo(0);
	}
	else if (newState == TOGGLESWITCH_DOWN) {
		if (output1)
			output1->WireTo(0);
		if (output2)
			output2->WireTo(this);
	}
}

void GuardedTwoOutputSwitch::LoadState(char *line)

{
	GuardedToggleSwitch::LoadState(line);
	UpdateSourceState(GetState());
}

TimerUpdateSwitch::TimerUpdateSwitch()

{
	adjust_type = TIME_UPDATE_SECONDS;
	springLoaded = SPRINGLOADEDSWITCH_CENTER;
}

void ThreeSourceTwoDestSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3, e_object *d1, e_object *d2)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source1 = s1;
	source2 = s2;
	source3 = s3;
	dest1 = d1;
	dest2 = d2;

	UpdateSourceState();
}

void ThreeSourceTwoDestSwitch::UpdateSourceState()

{
	if (IsUp()) {
		//
		// Source 1 to dest 1, source 3 to dest 2
		//
	}
	else if (IsCenter()) {
		//
		// Disconnect.
		//
	}
	else if (IsDown()) {
		//
		// Source 2 to dest 2, source 3 to dest 1.
		//
	}
}

void TimerUpdateSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int adjuster, MissionTimer *ptimer)

{
	MissionTimerSwitch::Init(xp, yp, w, h, surf, bsurf, row, ptimer);
	adjust_type = adjuster;
}

void TimerUpdateSwitch::AdjustTime(int val)

{
	if (timer) {
		switch (adjust_type) {
		case TIME_UPDATE_HOURS:
			timer->UpdateHours(val);
			break;

		case TIME_UPDATE_MINUTES:
			timer->UpdateMinutes(val);
			break;

		case TIME_UPDATE_SECONDS:
			timer->UpdateSeconds(val);
			break;
		}
	}
}

bool TimerControlSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (MissionTimerSwitch::CheckMouseClick(event, mx, my))
	{
		SetTimer();
		return true;
	}

	return false;
}

bool TimerControlSwitch::SwitchTo(int newState) 

{
	if (MissionTimerSwitch::SwitchTo(newState)) {
		SetTimer();
		return true;
	}
	return false;
}

void TimerControlSwitch::SetTimer() 

{
	if (timer) {
		if (IsUp()) {
			timer->SetRunning(true);
		}
		else if (IsCenter()) {
			timer->SetRunning(false);
		}
		else if (IsDown()) {
			timer->SetRunning(false);
			timer->Reset();
		}
	}
}


//
// Event timer start/stop switch. Although it's a three-position switch, the center position does
// nothing.
//

bool EventTimerControlSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (MissionTimerSwitch::CheckMouseClick(event, mx, my))
	{
		if (timer) {
			if (IsUp()) {
				timer->SetRunning(true);
			}
			else if (IsDown()) {
				timer->SetRunning(false);
			}
		}
		return true;
	}

	return false;
}

//
// Event timer up/down/reset switch.
//

bool EventTimerResetSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (MissionTimerSwitch::CheckMouseClick(event, mx, my))
	{
		if (timer) {
			if (IsUp()) {
				timer->SetCountUp(TIMER_COUNT_NONE);
				timer->Reset();
				timer->SetRunning(false);
			}
			else if (IsCenter()) {
				timer->SetCountUp(TIMER_COUNT_UP);
			}
			else if (IsDown()) {
				timer->SetCountUp(TIMER_COUNT_DOWN);
			}
		}
		return true;
	}

	return false;
}

//
// Generic switch to update the timer time. AdjustTime() knows whether to update the hours, minutes or seconds.
//

bool TimerUpdateSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (MissionTimerSwitch::CheckMouseClick(event, mx, my))
	{
		//
		// We need to increase by one if the switch is up, and ten if it's down.
		//
		if (IsUp()) {
			AdjustTime(10);
		}
		else if (IsDown()) {
			AdjustTime(1);
		}
		return true;
	}
	else return false;
}

IMUCageSwitch::IMUCageSwitch()

{
	imu = 0;
}

void IMUCageSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im)

{
	GuardedToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	imu = im;
}

bool IMUCageSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (GuardedToggleSwitch::CheckMouseClick(event, mx, my))
	{
		SetIMU();
		return true;
	}
	else return false;
}

bool IMUCageSwitch::SwitchTo(int newState)

{
	if (GuardedToggleSwitch::SwitchTo(newState))
	{
		SetIMU();
		return true;
	}
	else return false;
}

void IMUCageSwitch::SetIMU() 

{
	//
	// Cage the IMU if it's up, release if it's down. For now we'll also turn it
	// on if the switch is down.
	//
	if (imu) {
		if (IsUp()) {
			imu->SetCaged(true);
		}
		else if (IsDown()) {
			imu->SetCaged(false);
			imu->TurnOn();
		}
	}
}

//
// Unfortunately we need to duplicate this code for the LEM switch which doesn't have a guard.
//

UnguardedIMUCageSwitch::UnguardedIMUCageSwitch()

{
	imu = 0;
}

void UnguardedIMUCageSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	imu = im;
}

bool UnguardedIMUCageSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my))
	{
		SetIMU();
		return true;
	}
	else return false;
}

bool UnguardedIMUCageSwitch::SwitchTo(int newState)

{
	if (ToggleSwitch::SwitchTo(newState))
	{
		SetIMU();
		return true;
	}
	else return false;
}

void UnguardedIMUCageSwitch::SetIMU() 

{
	//
	// Cage the IMU if it's up, release if it's down. For now we'll also turn it
	// on if the switch is down.
	//
	if (imu) {
		if (IsUp()) {
			imu->SetCaged(true);
		}
		else if (IsDown()) {
			imu->SetCaged(false);
			imu->TurnOn();
		}
	}
}

//
// Enable light test on caution and warning system.
//

bool CWSLightTestSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (CWSThreePosSwitch::CheckMouseClick(event, mx,my)) {
		//
		// Up lights left-hand lights, down lights right-hand lights.
		//

		if (cws) {
			if (IsUp()) {
				cws->LightTest(CWS_TEST_LIGHTS_LEFT);
			}
			else if (IsDown()) {
				cws->LightTest(CWS_TEST_LIGHTS_RIGHT);
			}
			else if (IsCenter()) {
				cws->LightTest(CWS_TEST_LIGHTS_NONE);
			}
		}
		return true;
	}

	return false;
}

//
// Set caution and warning mode state.
//

bool CWSModeSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (CWSThreePosSwitch::CheckMouseClick(event, mx,my)) {
		if (cws) {
			if (IsUp()) {
				cws->SetMode(CWS_MODE_NORMAL);
			}
			else if (IsCenter()) {
				cws->SetMode(CWS_MODE_BOOST);
			}
			else if (IsDown()) {
				cws->SetMode(CWS_MODE_ACK);
			}
		}
		return true;
	}

	return false;
}

bool CWSModeSwitch::SwitchTo(int newState) 

{
	if (CWSThreePosSwitch::SwitchTo(newState)) {
		if (cws) {
			if (IsUp()) {
				cws->SetMode(CWS_MODE_NORMAL);
			}
			else if (IsCenter()) {
				cws->SetMode(CWS_MODE_BOOST);
			}
			else if (IsDown()) {
				cws->SetMode(CWS_MODE_ACK);
			}
		}
		return true;
	}
	return false;
}


//
// Set caution and warning power source.
//

bool CWSPowerSwitch::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (CWSThreePosSwitch::CheckMouseClick(event, mx, my)) {
		if (Active && (state != OldState)) {
			SetPowerBus();
		}
		return true;
	}
	return false;
}

bool CWSPowerSwitch::SwitchTo(int newState) 

{
	if (CWSThreePosSwitch::SwitchTo(newState)) {
		SetPowerBus();
		return true;
	}
	return false;
}

void CWSPowerSwitch::SetPowerBus() 

{
	if (cws) {
		if (IsUp()) {
			cws->SetPowerBus(CWS_POWER_SUPPLY_1);
		}
		else if (IsCenter()) {
			cws->SetPowerBus(CWS_POWER_NONE);
		}
		else if (IsDown()) {
			cws->SetPowerBus(CWS_POWER_SUPPLY_2);
		}
	}
}

void CWSThreePosSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	cws = c;
}

//
// Set caution and warning source (e.g. CSM/LEM or CM)
//

bool CWSSourceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my)) {
		if (cws) {
			if (IsUp()) {
				cws->SetSource(CWS_SOURCE_CSM);
			}
			else if (IsDown()) {
				cws->SetSource(CWS_SOURCE_CM);
			}
		}
		return true;
	}

	return false;
}

//
// Switch that controls AGC input channels.
//

bool AGCIOSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my)) {
		if (agc) {
			if (IsUp()) {
				agc->SetInputChannelBit(Channel, Bit, UpValue);
			}
			else if (IsDown()) {
				agc->SetInputChannelBit(Channel, Bit, !UpValue);
			}
		}
		return true;
	}
	return false;
}

//
// CMC Hold/Free/Auto mode.
//

bool CMCModeHoldFreeSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (AGCThreePoswitch::CheckMouseClick(event, mx, my)) {
		if (agc) {
			bool Hold = false;
			bool Free = false;

			if (IsCenter()) {
				Hold = true;
			}
			else if (IsDown()) {
				Free = true;
			}

			agc->SetInputChannelBit(031, 13, Hold);
			agc->SetInputChannelBit(031, 14, Free);
		}
		return true;
	}

	return false;
}

//
// CMC Optics Mode Switch
bool CMCOpticsModeSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (AGCThreePoswitch::CheckMouseClick(event, mx, my)) {
		if (agc) {
			unsigned int SwitchBits;
			SwitchBits = agc->GetCh33Switches();
			SwitchBits &= 077707;  // Clear bits
			if (IsUp()) {
				SwitchBits |= 010; // CMC MODE, ZERO OFF				
				agc->SetCh33Switches(SwitchBits);
				return true;
			}
			if (IsCenter()) {
				SwitchBits |= 030; // MANUAL MODE, ZERO OFF
				
				agc->SetCh33Switches(SwitchBits);
				return true;
			}
			if (IsDown()) {
				SwitchBits |= 020; // MANUAL MODE, ZERO ON
				agc->SetCh33Switches(SwitchBits);
				return true;
			}
		}
		return true;
	}

	return false;
}

//
// LEM PGNS switch.
//

bool PGNSSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (AGCThreePoswitch::CheckMouseClick(event, mx, my)) {
		if (agc) {
			bool Hold = false;
			bool Free = false;

			if (IsCenter()) {
				Hold = true;
			}
			else if (IsUp()) {
				Free = true;
			}

			agc->SetInputChannelBit(031, 13, Hold);
			agc->SetInputChannelBit(031, 14, Free);
		}
		return true;
	}

	return false;
}

//
// If we add more caution and warning system switches which use toggle-switch, they could be derived from a new
// class which has the generic init function to set the cws.
//

void CWSSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	cws = c;
}


void AGCSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	agc = c;
}

void AGCThreePoswitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	agc = c;
}


//
// Handcontroller Switch
//

HandcontrollerSwitch::HandcontrollerSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	state = 0;
	switchSurface = 0;
	switchRow = 0;
}

HandcontrollerSwitch::~HandcontrollerSwitch() {
	sclick.done();
}

void HandcontrollerSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, bool hasyawaxis) {

	name = n;
	hasYawAxis =hasyawaxis;
	scnh.RegisterSwitch(this);
}

void HandcontrollerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	switchSurface = surf;
	
	row.AddSwitch(this);
	switchRow = &row;

	if (!sclick.isValid()) {
		row.panelSwitches->soundlib->LoadSound(sclick, THUMBWHEEL_SOUND);
	}
}

int HandcontrollerSwitch::GetState() {

	return state;
}

bool HandcontrollerSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	if (event == PANEL_MOUSE_LBDOWN) {
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

		if (mx < (x + (width / 3.))) {
			if (my < (y + (height * 2. / 3.))) {
				state = 2;
				sclick.play();
			} else if (hasYawAxis) {
				state = 5;
				sclick.play();
			}
		} else if (mx > (x + (width * 2. / 3.))) {
			if (my < (y + (height * 2. / 3.))) {
				state = 1;
				sclick.play();
			} else if (hasYawAxis) {
				state = 6;
				sclick.play();
			}
		} else {
			if (my < (y + (height / 2.))) {
				state = 4;
				sclick.play();
			} else {
				state = 3;
				sclick.play();
			}
		}
	} else if (event == PANEL_MOUSE_LBUP) {
		state = 0;
		sclick.play();
	}
	return true;
}

void HandcontrollerSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	oapiBlt(DrawSurface, switchSurface, x, y, state * width, 0, width, height, SURF_PREDEF_CK);
}

void HandcontrollerSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state);
}

void HandcontrollerSwitch::LoadState(char *line) {

	char buffer[100];
	int st;

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
	}
}

