/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  Toggle switch handling code definitions.

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
  *	Revision 1.10  2012/01/14 22:21:57  tschachim
  *	Bugfixes, flash handling, flashing for handcontroller
  *	
  *	Revision 1.9  2009/12/22 18:14:47  tschachim
  *	More bugfixes related to the prelaunch/launch checklists.
  *	
  *	Revision 1.8  2009/12/17 17:47:18  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.7  2009/09/17 17:48:42  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.6  2009/09/10 02:12:37  dseagrav
  *	Added lm_ags and lm_telecom files, LM checkpoint commit.
  *	
  *	Revision 1.5  2009/08/16 03:12:38  dseagrav
  *	More LM EPS work. CSM to LM power transfer implemented. Optics bugs cleared up.
  *	
  *	Revision 1.4  2009/08/12 23:26:53  tschachim
  *	"Sideways" toggle switch.
  *	
  *	Revision 1.3  2009/08/10 14:38:03  tschachim
  *	ECS enhancements
  *	
  *	Revision 1.2  2009/08/10 02:23:06  dseagrav
  *	LEM EPS (Part 2)
  *	Split ECAs into channels, Made bus cross tie system, Added ascent systems and deadface/staging logic.
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.78  2008/12/07 18:35:21  movieman523
  *	Very basics of DSE telemetry recording: the play/record switch works but nothing else does!
  *	
  *	Also don't try to create a panel if we can't load the bitmap.
  *	
  *	Revision 1.77  2008/05/24 17:27:22  tschachim
  *	Added switch borders.
  *	
  *	Revision 1.76  2008/04/11 12:01:55  tschachim
  *	Cleanup of the checklist events.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.75  2008/03/14 19:19:20  lassombra
  *	Changed setCallback to SetCallback
  *	
  *	Revision 1.74  2008/03/14 19:12:06  lassombra
  *	Fixed functor inheritance.
  *	
  *	Revision 1.73  2008/03/14 05:21:22  lassombra
  *	Implemented basic functor based callback for all panel switch items.  Can be used in place of current panel listener.
  *	
  *	Revision 1.72  2008/01/25 20:06:07  lassombra
  *	Implemented delayable switch functions.
  *	
  *	Now, all register functions on all toggle switches should take, at the end, a boolean
  *	 for whether it is delayable, and an int for how many seconds to delay.
  *	
  *	Actual delay can be anywhere between the int and the int + 1.
  *	
  *	Function is implemented as a timestepped switch which is called intelligently from
  *	 the panel, which now gets a timestep call.
  *	
  *	Revision 1.71  2008/01/25 05:58:53  lassombra
  *	Minor bugfix
  *	
  *	Revision 1.70  2008/01/25 04:39:42  lassombra
  *	All switches now handle change of state through SwitchTo function which is vitual
  *	 and is called by existing mouse and connector handling methods.
  *	
  *	Support for delayed spring switches and other ChecklistController functionality following soon.
  *	
  *	Revision 1.69  2008/01/14 01:17:11  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.68  2008/01/09 01:46:45  movieman523
  *	Added initial support for talking to checklist controller from MFD.
  *	
  *	Revision 1.67  2007/12/21 02:47:08  movieman523
  *	Connector cleanup, and fix my build break!
  *	
  *	Revision 1.66  2007/12/21 02:31:18  movieman523
  *	Added SetState() call and some more documentation.
  *	
  *	Revision 1.65  2007/12/21 01:00:27  movieman523
  *	Really basic Checklist MFD based on Project Apollo MFD, along with the various support functions required to make it work.
  *	
  *	Revision 1.64  2007/11/30 16:40:40  movieman523
  *	Revised LEM to use generic voltmeter and ammeter code. Note that the ED battery select switch needs to be implemented to fully support the voltmeter/ammeter now.
  *	
  *	Revision 1.63  2007/11/29 22:08:27  movieman523
  *	Moved electric meters to generic classes in toggleswitch.cpp rather than Saturn-specific.
  *	
  *	Revision 1.62  2007/11/17 19:37:26  movieman523
  *	Doxygen changes and more use of IsSpringLoaded() function in place of directly testing the value of the variable. Checking this in prior to switching development work from my laptop to desktop system.
  *	
  *	Revision 1.61  2007/10/21 21:25:13  movieman523
  *	Added SHIFT-click to hold spring-loaded buttons.
  *	
  *	Revision 1.60  2007/08/13 16:06:25  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.59  2007/07/17 14:33:11  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.58  2007/01/22 14:47:38  tschachim
  *	Moved FDAIPowerRotationalSwitch to satswitches, horizontal thumbwheel.
  *	
  *	Revision 1.57  2006/11/24 22:42:44  dseagrav
  *	Enable changing bits in AGC channel 33, enable LEB optics switch, enable tracker switch as optics status debug switch.
  *	
  *	Revision 1.56  2006/11/13 14:47:34  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.55  2006/07/24 06:41:30  dseagrav
  *	Many changes - Rearranged / corrected FDAI power usage, added LM AC equipment, many bugfixes
  *	
  *	Revision 1.54  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.53  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.52  2006/06/17 18:13:13  tschachim
  *	Moved BMAGPowerRotationalSwitch.
  *	
  *	Revision 1.51  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.50  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.49  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.48  2006/05/01 08:52:50  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.47  2006/04/17 18:20:11  movieman523
  *	Removed #if 0 code.
  *	
  *	Revision 1.46  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  *	Revision 1.45  2006/04/17 15:16:16  movieman523
  *	Beginnings of checklist code, added support for flashing borders around control panel switches and updated a portion of the Saturn panel switches appropriately.
  *	
  *	Revision 1.44  2006/03/12 01:13:29  dseagrav
  *	Added lots of SCS items and FDAI stuff.
  *	
  *	Revision 1.43  2006/02/22 18:54:24  tschachim
  *	Bugfixes for Apollo 4-6.
  *	
  *	Revision 1.42  2006/02/01 18:16:50  tschachim
  *	Added TwoSourceSwitch::SwitchTo function.
  *	
  *	Revision 1.41  2006/01/26 03:07:52  movieman523
  *	Quick hack to support low-res mesh.
  *	
  *	Revision 1.40  2006/01/14 20:03:35  movieman523
  *	Fixed some switch bugs.
  *	
  *	Revision 1.39  2006/01/11 22:34:21  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.38  2006/01/11 02:59:43  movieman523
  *	Valve talkbacks now check the valve state directlry. This means they barberpole on SM sep and can't then be changed.
  *	
  *	Revision 1.37  2006/01/10 19:34:45  movieman523
  *	Fixed AC bus switches and added ELS Logic/Auto support.
  *	
  *	Revision 1.36  2006/01/08 17:50:39  movieman523
  *	Wired up electrical meter switches other than battery charger.
  *	
  *	Revision 1.35  2006/01/08 04:00:24  movieman523
  *	Added first two engineering cameras.
  *	
  *	Revision 1.34  2006/01/07 19:11:44  tschachim
  *	Checklist actions for FDAIPowerRotarySwitch.
  *	
  *	Revision 1.33  2006/01/07 03:28:28  movieman523
  *	Removed a lot of unused switches and wired up the FDAI power switch.
  *	
  *	Revision 1.32  2006/01/07 00:43:58  movieman523
  *	Added non-essential buses, though there's nothing connected to them at the moment.
  *	
  *	Revision 1.31  2005/12/19 16:47:36  tschachim
  *	Realism mode, SwitchTo functions.
  *	
  *	Revision 1.30  2005/12/02 19:47:19  movieman523
  *	Replaced most PowerSource code with e_object.
  *	
  *	Revision 1.29  2005/11/20 21:46:32  movieman523
  *	Added initial volume control support.
  *	
  *	Revision 1.28  2005/11/17 23:32:46  movieman523
  *	Added support for specifying the maximum current for a circuit breaker. Exceed that current and the breaker pops.
  *	
  *	Revision 1.27  2005/11/17 01:34:25  movieman523
  *	Extended circuit breaker init function so it can be wired directly to a power source.
  *	
  *	Revision 1.26  2005/11/17 01:23:11  movieman523
  *	Revised circuit breaker code. Now all switchers are PowerSources, so no need for the seperate PowerBreaker class.
  *	
  *	Revision 1.25  2005/11/16 23:14:02  movieman523
  *	Initial support for wiring in the circuit breakers.
  *	
  *	Revision 1.24  2005/10/31 10:43:13  tschachim
  *	Added CircuitBrakerSwitch and ThumbwheelSwitch.
  *	
  *	Revision 1.23  2005/10/19 11:48:09  tschachim
  *	Update MeterSwitch.DisplayValue when requested (and not only when displayed).
  *	
  *	Revision 1.22  2005/10/11 16:53:12  tschachim
  *	Enhanced guard handling, SwitchTo functions added, bugfixes.
  *	
  *	Revision 1.21  2005/09/30 11:26:47  tschachim
  *	Added new spring-loaded modes, new event handler, added MeterSwitch.
  *	
  *	Revision 1.20  2005/08/20 12:24:03  movieman523
  *	Added a FailedState as well as a Failed flag to each switch. You now must call GetState(), IsUp(), IsDown() etc to get the functional state of the switch. The state variable now only tells you the visual state (e.g. switch up, down, center) and not the functional state.
  *	
  *	Revision 1.19  2005/08/19 21:33:20  movieman523
  *	Added initial random failure support.
  *	
  *	Revision 1.18  2005/08/18 19:12:21  movieman523
  *	Added Event Timer switches and null Event Timer class.
  *	
  *	Revision 1.17  2005/08/17 22:54:26  movieman523
  *	Added ELS and CM RCS switches.
  *	
  *	Revision 1.16  2005/08/16 11:46:58  tschachim
  *	Fixed rotational switch because of new bitmap.
  *	
  *	Revision 1.15  2005/08/13 20:20:17  movieman523
  *	Created MissionTimer class and wired it into the LEM and CSM.
  *	
  *	Revision 1.14  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
  *	Revision 1.13  2005/08/13 14:21:36  movieman523
  *	Added beginnings of caution and warning system.
  *	
  *	Revision 1.12  2005/08/13 11:48:27  movieman523
  *	Added remaining caution and warning switches to CSM (currently not wired up to anything).
  *	
  *	Revision 1.11  2005/08/13 00:09:43  movieman523
  *	Added IMU Cage switch
  *	
  *	Revision 1.10  2005/08/12 23:15:49  movieman523
  *	Added switches to update mission time display.
  *	
  *	Revision 1.9  2005/07/30 16:08:11  tschachim
  *	Added SwitchTo function to simulate switch usage.
  *	
  *	Revision 1.8  2005/07/05 17:58:03  tschachim
  *	Introduced spring-loaded switches
  *	
  *	Revision 1.7  2005/06/06 12:29:44  tschachim
  *	Introduced PushSwitch, GuardedPushSwitch, PanelScenarioHandler
  *	
  *	Revision 1.6  2005/05/05 21:36:41  tschachim
  *	Introduced PanelSwitchItem and IndicatorSwitch
  *	
  *	Revision 1.5  2005/04/22 14:05:15  tschachim
  *	Init functions changed
  *	SwitchListener introduced
  *	Some defines
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
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *
  **************************************************************************/

#ifndef __toggleswitch_h
#define __toggleswitch_h

#include "cautionwarning.h"
#include "powersource.h"
#include "nasspdefs.h"

//
// Switch states. Only use positive numbers.
//

#define TOGGLESWITCH_DOWN		0			///< Toggle switch is up.
#define TOGGLESWITCH_UP			1			///< Toggle switch is down.

#define THREEPOSSWITCH_DOWN		0			///< Three-position switch is down.
#define THREEPOSSWITCH_CENTER	1			///< Three-position switch is centered.
#define THREEPOSSWITCH_UP		2			///< Three-position switch is up.

#define FIVEPOSSWITCH_DOWN		0			///< Five-position switch is down.
#define FIVEPOSSWITCH_CENTER	1			///< Five-position switch is centered.
#define FIVEPOSSWITCH_UP		2			///< Five-position switch is up.
#define FIVEPOSSWITCH_LEFT		3			///< Five-position switch is left.
#define FIVEPOSSWITCH_RIGHT		4			///< Five-position switch is right.

#define GUARDEDSWITCH_UNGUARD	101			///<SetState can open a guard from this.
#define GUARDEDSWITCH_GUARD		102			///<SetState can close a guard from this.

#define SPRINGLOADEDSWITCH_NONE					0		///< Switch is not spring-loaded.
#define SPRINGLOADEDSWITCH_DOWN					1		///< Switch is spring-loaded to down.
#define SPRINGLOADEDSWITCH_CENTER				2		///< Switch is spring-loaded to center.
#define SPRINGLOADEDSWITCH_UP					3		///< Switch is spring-loaded to up.
#define SPRINGLOADEDSWITCH_CENTER_SPRINGUP		4
#define SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN	5

#define PANELSWITCH_START_STRING	"PANELSWITCHES_BEGIN"	///< Beginning of saved switch states in scenario file.
#define PANELSWITCH_END_STRING		"PANELSWITCHES_END"		///< End of saved switch states in scenario file.

#define TIME_UPDATE_SECONDS	0
#define TIME_UPDATE_MINUTES 1
#define TIME_UPDATE_HOURS	2

class SwitchRow;
class PanelSwitchScenarioHandler;
class PanelSwitchCallbackInterface;

class PanelSwitchCallbackInterface;
///
/// This is the base class for panel items. Items using this class can be looked up by name,
/// and are automatically saved into scenario files and loaded from them without us having to
/// individually track each one.
/// \brief Panel item base class.
/// \ingroup PanelItems
///
class PanelSwitchItem: public e_object {
friend class SwitchRow;
public:
	PanelSwitchItem();
	virtual ~PanelSwitchItem();

	void SetNext(PanelSwitchItem *s) { next = s; };
	PanelSwitchItem *GetNext() { return next; };
	void SetNextForScenario(PanelSwitchItem *s) { nextForScenario = s; };
	PanelSwitchItem *GetNextForScenario() { return nextForScenario; };

	///
	/// Force an object to fail, or return it to correct operation, and set the
	/// state it was in when it failed.
	/// \brief Force an object to fail and set state.
	/// \param fail True to make the object fail, false to return it to operation.
	/// \param fail_state The state the object will fail in (e.g. switch up or switch down).
	///
	void SetFailed(bool fail, int fail_state = 0) { Failed = fail; FailedState = fail_state; };

	///
	/// \brief Has the object failed?
	/// \return True if the object failed, false if it's operating normally.
	///
	bool IsFailed() { return Failed; };

	///
	/// \brief Get the name of this item.
	/// \return Pointer to a string containing the item name.
	///
	char *GetName() { return name; }

	///
	/// \brief Process a mouse click which may be for this item.
	/// \param event Mouse event type.
	/// \param mx Mouse x position.
	/// \param my Mouse y position.
	/// \return True if we processed it, false if it wasn't for us.
	///
	virtual bool CheckMouseClick(int event, int mx, int my) = 0;

	///
	/// \brief Draw the switch with its current state and position.
	/// \param DrawSurface Surface to draw the switch into.
	///
	virtual void DrawSwitch(SURFHANDLE DrawSurface) = 0;

	///
	/// \brief Save the switch state.
	/// \param scn Scenario file to save state into.
	///
	virtual void SaveState(FILEHANDLE scn) = 0;

	///
	/// \brief Load state from scenario.
	/// \param line A line from the scenario file, which may or may not be for us.
	///
	virtual void LoadState(char *line) = 0;
	virtual void DrawFlash(SURFHANDLE DrawSurface) {};

	///
	/// Each object has a human-readable displayable name. Normally this will be a
	/// pre-initialised string rather than a dynamic name, so we just copy the pointer
	/// passed into us and don't take a copy. If you call this with a temporary string
	/// on the stack our pointer will end up pointing to garbage after you return from
	/// the routine which set up the string.
	/// \brief Set the displayable name.
	///
	void SetDisplayName(char *s) { DisplayName = s; };

	///
	/// \brief Get the displayable name.
	/// \return Pointer to the displayable name, or NULL.
	///
	char *GetDisplayName();

	///
	/// \brief Start flashing the object.
	/// \param flash True to turn flashing on, false to turn it off.
	///
	void SetFlashing(bool flash) { flashing = flash; };

	///
	/// \brief Check whether flashing is enabled.
	/// \return True if the object is flashing.
	///
	bool IsFlashing() { return flashing; };

	///
	/// \brief Set the visibitility state of the item.
	/// \param v True if it's visible, false if it's not.
	///
	void SetVisible(bool v) {visible = v; };

	///
	/// \brief Get current state.
	/// \return Current state value.
	///
	virtual int GetState();

	///
	/// \brief Set current state.
	/// \param value State to set.
	///
	virtual void SetState(int value);

	///
	/// \brief Check the power state.
	/// \return True if the switch is powered.
	///
	virtual bool IsPowered() { return Voltage() > SP_MIN_DCVOLTAGE; };

	///
	/// \brief Timestep function, only called if doTimeStep is true
	/// \param time - Mission time
	///
	virtual void timestep(double missionTime){return;}

	///
	/// \brief Set a callback.  Note, make absolute sure that the old callback is properly disposed of.
	/// \param call - New callback to use.
	///
	virtual void SetCallback(PanelSwitchCallbackInterface* call);

	///
	/// \brief Unguard
	///
	virtual void Unguard() {};

	///
	/// \brief Guard
	///
	virtual void Guard() {};

	virtual	void SetHeld(bool s) {};
	
protected:
	///
	/// This can be interpreted in any desired manner by derived classes. Only positive values should be used.
	/// \brief Current state.
	///
	int state;

	///
	/// boolean determining whether to call timestep.
	///
	bool doTimeStep;

	///
	/// If a switch fails, you can no longer change the state even if you can still move
	/// the toggle. If a gauge fails it will no longer read the correct value.
	/// \brief Has the switch failed?
	///
	bool Failed;

	///
	/// \brief State that the switch failed in.
	///
	int FailedState;

	///
	/// \brief Name of the switch.
	///
	char *name;

	///
	/// We can display a flashing rectangle around a switch or gauge to point the user to
	/// it when running a checklist. For example, if they need to click a switch to continue
	/// we can indicate the switch to click until it's in the correct state, or if they should
	/// check a gauge reading we can flash that briefly.
	/// \brief Is the object flashing?
	///
	bool flashing;

	///
	/// A human-readable object name which we can display to them when we want them to use the
	/// object. For example 'The CSM/LV seperation switch', or 'The liquid oxygen quantity gauge.'
	/// \brief Displayable name.
	///
	char *DisplayName;

	///
	/// \brief Is the object visible?
	///
	bool visible;

	PanelSwitchItem *next;
	PanelSwitchItem *nextForScenario;
	PanelSwitchCallbackInterface *callback;
};

///
/// Generic two-position toggle switch.
/// \brief Toggle switch base class.
/// \ingroup PanelItems
///
class ToggleSwitch: public PanelSwitchItem {

public:
	ToggleSwitch();
	virtual ~ToggleSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int springloaded = SPRINGLOADEDSWITCH_NONE, char *dname = 0);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s,
		      int xoffset = 0, int yoffset = 0);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,
		      int xoffset = 0, int yoffset = 0);
	void SetSize(int w, int h) { width = w; height = h; };
	void SetPosition(int xp, int yp) { x = xp; y = yp; };
	void SetOffset(int xo, int yo) {xOffset = xo; yOffset = yo; };
	void SetSpringLoaded(int springloaded) { springLoaded = springloaded; };
	bool IsSpringLoaded() { return (springLoaded != SPRINGLOADEDSWITCH_NONE); };
	void SetHeld(bool s) { Held = s; };
	bool IsHeld() { return Held; };
	void SetActive(bool s);
	void SetSideways(bool s) { Sideways = s; };
	void SetDelayTime(double t) { delayTime = t; };

	bool Toggled() { return SwitchToggled; };
	void ClearToggled() { SwitchToggled = false; };
	
	void DrawFlash(SURFHANDLE DrawSurface);
	void SetBorderSurface(SURFHANDLE border) { BorderSurface = border; };

	virtual bool IsUp() { return (GetState() == TOGGLESWITCH_UP); };
	virtual bool IsDown() { return (GetState() == TOGGLESWITCH_DOWN); };
	virtual bool IsCenter() { return false; };

	virtual bool SwitchTo(int newState, bool dontspring = false);
	virtual void DrawSwitch(SURFHANDLE DrawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
	virtual void SaveState(FILEHANDLE scn);
	virtual void LoadState(char *line);
	virtual void SetState(int value); //Needed to properly process set states from toggle switches.
	virtual void timestep(double missionTime);

protected:
	virtual void InitSound(SoundLib *s);
	virtual void DoDrawSwitch(SURFHANDLE DrawSurface);
	bool DoCheckMouseClick(int event, int mx, int my);

	virtual unsigned int GetFlags();
	virtual void SetFlags(unsigned int f);

	int	x;
	int y;
	int width;
	int height;
	int xOffset;
	int yOffset;

	int springLoaded;
	bool Active;
	bool SwitchToggled;
	bool Held;
	bool Sideways;

	double delayTime;
	double resetTime;

	SURFHANDLE SwitchSurface;
	SURFHANDLE BorderSurface;
	VESSEL *OurVessel;
	Sound Sclick;

	SwitchRow *switchRow;

	///
	/// Flags structure for saving state to scenario file.
	///
	struct ToggleSwitchFlags {
		union {
			struct {
				unsigned int Held:1;	///< Is the switch held?
			};
			unsigned int flags;			///< Packed structure value to save.
		};

		///
		/// Constructor. Set flags to zero.
		///
		ToggleSwitchFlags() { flags = 0; };
	};
};

///
/// \brief Orbiter attitude-control toggle switch.
/// \ingroup PanelItems
///
class AttitudeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	virtual bool SwitchTo(int newState, bool dontspring = false);

};

///
/// \brief Orbiter attitude-control toggle switch.
/// \ingroup PanelItems
///
class NavModeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, VESSEL *v, int mode, SoundLib &s);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	int NAVMode;
};

///
/// \brief Orbiter HUD-mode toggle switch.
/// \ingroup PanelItems
///
class HUDToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int mode, SoundLib &s);
	virtual bool SwitchTo(int newState,bool dontspring = false);

protected:
	int	HUDMode;
};

///
/// Generic three-position toggle switch
/// \brief Three position toggle switch base class.
/// \ingroup PanelItems
///
class ThreePosSwitch: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState, bool dontspring = false);

	bool IsDown() { return (GetState() == THREEPOSSWITCH_DOWN); };
	bool IsCenter() { return (GetState() == THREEPOSSWITCH_CENTER); };
	bool IsUp() { return (GetState() == THREEPOSSWITCH_UP); };
};

///
/// Generic five-position toggle switch
/// \brief Five position toggle switch base class.
/// \ingroup PanelItems
///

class FivePosSwitch: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState, bool dontspring = false);

	bool IsDown() { return (GetState() == FIVEPOSSWITCH_DOWN); };
	bool IsCenter() { return (GetState() == FIVEPOSSWITCH_CENTER); };
	bool IsUp() { return (GetState() == FIVEPOSSWITCH_UP); };
	bool IsLeft() { return (GetState() == FIVEPOSSWITCH_LEFT); };
	bool IsRight() { return (GetState() == FIVEPOSSWITCH_RIGHT); };

};

///
/// A three-position switch which can switch between three different electrical sources.
/// \brief Three power source switch.
/// \ingroup PanelItems
///
class ThreeSourceSwitch : public ThreePosSwitch {
public:
	ThreeSourceSwitch() { source1 = source2 = source3 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	virtual void UpdateSourceState();

	e_object *source1;
	e_object *source2;
	e_object *source3;
};

///
/// A two-position switch which can switch between two different electrical sources.
/// \brief Two power source switch.
/// \ingroup PanelItems
///
class TwoSourceSwitch : public ToggleSwitch {
public:
	TwoSourceSwitch() { source1 = source2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void LoadState(char *line);
	//virtual void SetState(int value);

protected:
	virtual void UpdateSourceState();

	e_object *source1;
	e_object *source2;
};

///
/// A two-position switch which can switch its power between two different electrical outputs.
/// \brief Two power output switch.
/// \ingroup PanelItems
///
class TwoOutputSwitch : public ToggleSwitch {
public:
	TwoOutputSwitch() { output1 = output2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	virtual void UpdateSourceState();

	e_object *output1;
	e_object *output2;
};

///
/// A three-position switch which can switch its power between three different electrical outputs.
/// \brief Three power output switch.
/// \ingroup PanelItems
///
class ThreeOutputSwitch : public ThreePosSwitch {
public:
	ThreeOutputSwitch() { output1 = output2 = output3 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2, e_object *o3);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	virtual void UpdateSourceState();

	e_object *output1;
	e_object *output2;
	e_object *output3;
};

class ThreeSourceTwoDestSwitch : public ThreeSourceSwitch {
public:
	ThreeSourceTwoDestSwitch() { dest1 = dest2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3, e_object *d1, e_object *d2);

protected:
	virtual void UpdateSourceState();

	e_object *dest1;
	e_object *dest2;
};

//
// Mission Timer switches.
//

class MissionTimer; // Forward reference for files which include this before missiontimer.h

///
/// A generic three-position switch which can control a mission timer.
/// \brief Mission timer switch base class.
/// \ingroup PanelItems
///
class MissionTimerSwitch: public ThreePosSwitch {

public:
	MissionTimerSwitch() { timer = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, MissionTimer *ptimer);

protected:
	MissionTimer *timer;
};

///
/// A three-position switch which controls a mission timer.
/// \brief Mission timer control switch.
/// \ingroup PanelItems
///
class TimerControlSwitch: public MissionTimerSwitch {

public:
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	void SetTimer();
};

///
/// A three-position switch which updates the time on a mission timer.
/// \brief Mission timer time update switch.
/// \ingroup PanelItems
///
class TimerUpdateSwitch: public MissionTimerSwitch {

public:
	TimerUpdateSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int adjuster, MissionTimer *ptimer);
	virtual bool SwitchTo(int newstate, bool dontspring = false);

protected:
	void AdjustTime(int val);

	int adjust_type;
};


///
/// Switch that starts and stops the event timer.
/// \brief Event timer control switch.
/// \ingroup PanelItems
///
class EventTimerControlSwitch: public MissionTimerSwitch {

public:
	virtual bool SwitchTo(int newState, bool dontspring = false);

};

///
/// Switch that controls count up/count down/reset for event timer.
/// \brief Event timer count/reset switch.
/// \ingroup PanelItems
///
class EventTimerResetSwitch: public MissionTimerSwitch {

public:
	//bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState, bool dontspring = false);

};

class CautionWarningSystem; // Forward reference for files which include this before cautionwarning.h

///
/// A generic three-position switch which can control a caution and warning system.
/// \brief CWS control switch base class.
/// \ingroup PanelItems
///
class CWSThreePosSwitch: public ThreePosSwitch {

public:
	CWSThreePosSwitch() { cws = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c);

protected:
	///
	/// \brief The caution and warning system that this switch controls.
	///
	CautionWarningSystem *cws;
};

///
/// A three-position switch controlling a caution and warning system light test.
/// \brief CWS light test switch.
/// \ingroup PanelItems
///
class CWSLightTestSwitch: public CWSThreePosSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

///
/// A two-position switch which is pushed in/out rather than toggled up/down.
/// \brief Two-position push switch.
/// \ingroup PanelItems
///
class PushSwitch: public ToggleSwitch {

public:
	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, char *dname = 0);
	bool CheckMouseClick(int event, int mx, int my);

protected:
	virtual void InitSound(SoundLib *s);
};

///
/// A two-position electrical circuit breaker switch which is pushed in and pulled out rather 
/// than toggled up/down, and turns its electrical supply on and off as it does so.
/// \brief Two-position circuit breaker switch.
/// \ingroup PanelItems
///
class CircuitBrakerSwitch: public ToggleSwitch {

public:
	CircuitBrakerSwitch() { MaxAmps = 0.0; };

	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s = 0, double amps = 30.0);

	double Voltage();
	double Current();
	void DrawPower(double watts);

	///
	/// Maximum current which can be pulled through the circuit breaker before it automatically
	/// pops out.
	/// \brief Maximum safe current.
	///
	double MaxAmps;
	/// This has to be set early in initialization, otherwise MaxLoad is referred to before assignment
	/// when loading a scenario. See the MaxAmps setting happening in the LM systems init as the systems are wired.

protected:
	virtual void InitSound(SoundLib *s);

};

///
/// A three-position switch controlling a caution and warning system operating mode.
/// \brief CWS mode switch.
/// \ingroup PanelItems
///
class CWSModeSwitch: public CWSThreePosSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

///
/// A three-position switch controlling caution and warning system power.
/// \brief CWS power switch.
/// \ingroup PanelItems
///
class CWSPowerSwitch: public CWSThreePosSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
	//virtual void SetState(int value);

protected:
	void SetPowerBus();
};

///
/// A two-position switch controlling caution and warning system source, typically switching
/// between monitoring CSM systems and only monitoring CM systems..
/// \brief CWS source switch.
/// \ingroup PanelItems
///
class CWSSourceSwitch: public ToggleSwitch {
public:
	CWSSourceSwitch() { cws = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	///
	/// \brief The caution and warning system that this switch controls.
	///
	CautionWarningSystem *cws;
};

///
/// Dummy switch for checklist control
///
class MasterAlarmSwitch: public PushSwitch {

public:
	void Init(CautionWarningSystem *c) { cws = c; };
	int GetState();
	void SetState(int value);

protected:
	CautionWarningSystem *cws;
};

//
// Switches that talk directly to the AGC.
//

class ApolloGuidance; // Forward reference for files which include this before apolloguidance.h

class AGCSwitch: public ToggleSwitch {

public:
	AGCSwitch() { agc = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c);

protected:
	ApolloGuidance *agc;
};

class AGCThreePoswitch: public ThreePosSwitch {

public:
	AGCThreePoswitch() { agc = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c);

protected:
	ApolloGuidance *agc;
};

//
// This class directly toggles AGC input channel states.
//

class AGCIOSwitch: public AGCSwitch {
public:
	AGCIOSwitch() { Channel = 0; Bit = 0; UpValue = false; };
	void SetChannelData(int chan, int bit, bool value) { Channel = chan; Bit = bit; UpValue = value; };
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	bool UpValue;
	int Channel;
	int Bit;
};

//
// CMC mode switch.
//

class CMCModeHoldFreeSwitch : public AGCThreePoswitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class CMCOpticsModeSwitch : public AGCThreePoswitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class PGNSSwitch : public AGCThreePoswitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class GuardedToggleSwitch: public ToggleSwitch {

public:
	GuardedToggleSwitch();
	virtual ~GuardedToggleSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, int springloaded = SPRINGLOADEDSWITCH_NONE);
	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf,
				   int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	void DrawFlash(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetGuardState() { return guardState; };
	void SetGuardState(bool s) { guardState = s; };
	void SetGuardResetsState(bool s) { guardResetsState = s; };
	void Unguard() { guardState = 1; };
	void Guard();

protected:
	int	guardX;
	int guardY;
	int guardWidth;
	int guardHeight;
	int guardState;
	bool guardResetsState;

	SURFHANDLE guardSurface;
	SURFHANDLE guardBorder;

	int guardXOffset;
	int guardYOffset;
	Sound guardClick;
};

class GuardedTwoOutputSwitch : public GuardedToggleSwitch {
public:
	GuardedTwoOutputSwitch() { output1 = output2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void LoadState(char *line);
	virtual void SetState(int value);

protected:
	virtual void UpdateSourceState(int newState);

	e_object *output1;
	e_object *output2;
};

///
/// A guarded two-position switch which can switch between two different electrical sources.
/// \brief Guarded two power source switch.
/// \ingroup PanelItems
///
class GuardedTwoSourceSwitch : public GuardedToggleSwitch {
public:
	GuardedTwoSourceSwitch() { source1 = source2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void LoadState(char *line);

protected:
	virtual void UpdateSourceState();

	e_object *source1;
	e_object *source2;
};


class IMU; // Forward reference for files which include this before IMU.h

class IMUCageSwitch: public GuardedToggleSwitch {

public:
	IMUCageSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	IMU *imu;

	void SetIMU();
};

class UnguardedIMUCageSwitch: public ToggleSwitch {

public:
	UnguardedIMUCageSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	IMU *imu;

	void SetIMU();
};

class GuardedPushSwitch: public PushSwitch {

public:
	GuardedPushSwitch();
	virtual ~GuardedPushSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,
		int xoffset = 0, int yoffset = 0, int lxoffset = 0, int lyoffset = 0);

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState);
	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE dsurf,
				   int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	void DrawFlash(SURFHANDLE DrawSurface);
	void DoDrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetGuardState() { return guardState; };
	void SetGuardState(bool s) { guardState = s; };
	void Unguard() { guardState = 1; };
	void Guard();

	void SetLit(bool l) { lit = l; };
	bool IsLit() { return lit; };

protected:
	int	guardX;
	int guardY;
	int guardWidth;
	int guardHeight;
	int guardState;

	int litOffsetX;
	int litOffsetY;

	bool lit;

	SURFHANDLE guardSurface;
	SURFHANDLE guardBorder;

	int guardXOffset;
	int guardYOffset;
	Sound guardClick;
};


class GuardedThreePosSwitch: public ThreePosSwitch {

public:
	GuardedThreePosSwitch();
	virtual ~GuardedThreePosSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, 
				  int springloaded = SPRINGLOADEDSWITCH_NONE);
	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf,
				   int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetGuardState() { return guardState; };
	void SetGuardState(bool s) { guardState = s; };
	void SetGuardResetsState(bool s) { guardResetsState = s; };
	void Unguard() { guardState = 1; };
	void Guard();

protected:
	int	guardX;
	int guardY;
	int guardWidth;
	int guardHeight;
	int guardState;
	bool guardResetsState;
	SURFHANDLE guardSurface;
	int guardXOffset;
	int guardYOffset;
	Sound guardClick;
};


typedef struct {
	double angle;
	int xOffset;
	int yOffset;
} RotationalSwitchBitmap;

const int RotationalSwitchBitmapCount = 24;


class RotationalSwitchPosition {

public:
	RotationalSwitchPosition(int v, double a);
	virtual ~RotationalSwitchPosition();
	void SetNext(RotationalSwitchPosition *p) { next = p; };
	RotationalSwitchPosition *GetNext() { return next; };
	double GetAngle() { return angle; };
	int GetValue() { return value; };

protected:
	int value;
	double angle;
	RotationalSwitchPosition* next;
};


class RotationalSwitch: public PanelSwitchItem {

public:
	RotationalSwitch();
	virtual ~RotationalSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultValue);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void AddPosition(int value, double angle);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawFlash(SURFHANDLE drawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newValue);
	virtual void SaveState(FILEHANDLE scn);
	virtual void LoadState(char *line);
	int GetState();
	operator int();
	virtual void SetState(int value);
	void SoundEnabled(bool on) { soundEnabled = on; };

protected:
	int	x;
	int y;
	int width;
	int height;
	RotationalSwitchPosition *position;
	RotationalSwitchPosition *positionList;

	SURFHANDLE switchSurface;
	SURFHANDLE switchBorder;

	Sound sclick;
	bool soundEnabled;
	RotationalSwitchBitmap bitmaps[RotationalSwitchBitmapCount];
	SwitchRow *switchRow;

	void SetValue(int newValue);
	double AngleDiff(double a1, double a2);
	void DeletePositions();
};

class PowerStateRotationalSwitch: public RotationalSwitch {
public:
	PowerStateRotationalSwitch();

	virtual bool SwitchTo(int newValue);
	void LoadState(char *line);
	void SetSource(int num, e_object *s);
	double Current();
	double Voltage();

protected:
	void CheckPowerState();

	e_object *sources[16];
};

class IndicatorSwitch: public PanelSwitchItem {

public:
	IndicatorSwitch();
	virtual ~IndicatorSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, bool failopen = false);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	virtual int GetState() { return state; };
	virtual void SetState(int s) { state = s; };

//	int operator=(const int b) { state = b; return state; };
//	operator int() {return state; };

protected:
	int state; // Changed to INT for extended capabilities hackery
	double displayState;	//0: false, 1: moving, 2: moving, 3: true
	bool failOpen;
	int	x;
	int y;
	int width;
	int height;
	SURFHANDLE switchSurface;
	SwitchRow *switchRow;
};


class MeterSwitch: public PanelSwitchItem {

public:
	MeterSwitch();
	virtual ~MeterSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time, double defaultValue);
	void Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time);
	void Init(SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	double GetDisplayValue();

	virtual double QueryValue() = 0;
	virtual void DoDrawSwitch(double v, SURFHANDLE drawSurface) = 0;

protected:

	virtual double AdjustForPower(double val);

	double value;
	double displayValue;
	double minValue;
	double maxValue;
	double minMaxTime;
	SwitchRow *switchRow;
	double lastDrawTime;
};


class ThumbwheelSwitch: public PanelSwitchItem {

public:
	ThumbwheelSwitch();
	virtual ~ThumbwheelSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState);
	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState, bool horizontal);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawFlash(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetState();
//	int operator=(const int b);
//	operator int();
	virtual void SetState(int value);

protected:
	int	x;
	int y;
	int width;
	int height;
	int state;
	int maxState;
	bool isHorizontal;
	SURFHANDLE switchSurface;
	SURFHANDLE switchBorder;
	Sound sclick;
	SwitchRow *switchRow;
};


class HandcontrollerSwitch: public PanelSwitchItem {

public:
	HandcontrollerSwitch();
	virtual ~HandcontrollerSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, bool hasyawaxis = false);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawFlash(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetState();

protected:
	bool hasYawAxis;
	int	x;
	int y;
	int width;
	int height;
	int state;
	SURFHANDLE switchSurface;
	SURFHANDLE borderSurface;
	Sound sclick;
	SwitchRow *switchRow;
};

class SoundLib;

class VolumeThumbwheelSwitch: public ThumbwheelSwitch {

public:
	VolumeThumbwheelSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int vclass, SoundLib *s);
	virtual bool SwitchTo(int newState);
	void LoadState(char *line);

protected:
	SoundLib *sl;
	int volume_class;
};

class PanelSwitches;

class SwitchRow {

public:
	SwitchRow();
	virtual ~SwitchRow();

	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn);
	void AddSwitch(PanelSwitchItem *s);
	void Init(int area, PanelSwitches &panel, e_object *p = 0);
	SwitchRow *GetNext() { return RowList; };
	void SetNext(SwitchRow *s) { RowList = s; };
	void timestep(double missionTime);

	///
	/// Look up a panel switch item by its name.
	///
	/// \param n String for panel item name.
	/// \return Item if found, NULL if not.
	///
	PanelSwitchItem *GetItemByName(const char *n);

protected:
	PanelSwitchItem *SwitchList;
	SwitchRow *RowList;
	int PanelArea;
	PanelSwitches *panelSwitches;

	e_object *RowPower;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class FivePosSwitch;
	friend class PushSwitch;
	friend class GuardedToggleSwitch;
	friend class GuardedThreePosSwitch;
	friend class GuardedPushSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
	friend class ThumbwheelSwitch;
	friend class CircuitBrakerSwitch;
	friend class HandcontrollerSwitch;
};

class PanelSwitchListener {

public:
	virtual void PanelSwitchToggled(ToggleSwitch *s) = 0;
	virtual void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) = 0;
	virtual void PanelRotationalSwitchChanged(RotationalSwitch *s) = 0;
};

class PanelSwitches {

public:
	PanelSwitches() { PanelID = 0; RowList = 0; Realism = 0; lastexecutedtime=MINUS_INFINITY;};
	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn);
	void AddRow(SwitchRow *s) { s->SetNext(RowList); RowList = s; };
	void Init(int id, VESSEL *v, SoundLib *s, PanelSwitchListener *l) { PanelID = id; RowList = 0; vessel = v; soundlib = s; listener = l; };
	void SetRealism(int r) { Realism = r; };
	void timestep(double missionTime);

	///
	/// Set an item's flashing state.
	///
	/// \param n Item name.
	/// \param flash True for flashing, false for not.
	/// \return True if we found the item, false if not.
	///
	bool SetFlashing(const char *n, bool flash);

	int GetState(const char *n);
	bool SetState(const char *n, int value, bool guard = false, bool hold = false);
	bool GetFailedState(const char *n);
	bool GetFlashing(const char *n);

protected:
	VESSEL *vessel;
	SoundLib *soundlib;
	PanelSwitchListener *listener;
	int	PanelID;
	SwitchRow *RowList;
	int Realism;
	double lastexecutedtime;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class FivePosSwitch;
	friend class PushSwitch;
	friend class GuardedToggleSwitch;
	friend class GuardedThreePosSwitch;
	friend class GuardedPushSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
	friend class ThumbwheelSwitch;
	friend class CircuitBrakerSwitch;
	friend class HandcontrollerSwitch;
};


class PanelSwitchScenarioHandler {

public:
	PanelSwitchScenarioHandler() { switchList = 0; };
	void RegisterSwitch(PanelSwitchItem *s);
	PanelSwitchItem* GetSwitch(char *name);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	PanelSwitchItem *switchList;
};

///
/// \brief Round meter for control panel.
///
/// This meter displays a value with a rotating dial.
///
/// \ingroup PanelItems
///
class RoundMeter : public MeterSwitch {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row);

protected:
	HPEN Pen0;
	HPEN Pen1;

	void DrawNeedle (SURFHANDLE surf, int x, int y, double rad, double angle);
};

///
/// \brief Electric meter for control panel.
///
/// This meter displays the electrical readings of one of the numerous systems in the CSM (e.g. the main buses
/// or batteries).
///
/// \ingroup PanelItems
///
class ElectricMeter: public RoundMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum value to display (meter may show beyond it).
	/// \param maxVal Maximum value to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum value.
	/// \param vMax Angle of meter at maximum value.
	///
	ElectricMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Initialise the meter.
	///
	void Init(HPEN p0, HPEN p1, SwitchRow &row, e_object *dcindicatorswitch);

	///
	/// \brief Actually draw the switch.
	/// \param volts Current voltage.
	/// \param drawSurface The surface to draw to.
	///
	void DoDrawSwitch(double volts, SURFHANDLE drawSurface);

	///
	/// \brief Set the switch bitmap.
	/// \param srf Frame bitmap surface.
	/// \param x Width in pixels.
	/// \param y Height in pixels.
	///
	void SetSurface(SURFHANDLE srf, int x, int y);

protected:
	double minValue;		///< The minimum value to display.
	double maxValue;		///< The maximum value to  display.
	double minAngle;		///< Angle at minimum voltage.
	double maxAngle;		///< Angle at maximum voltage.
	double ScaleFactor;		///< The internal volts to angle scale factor.

	int xSize;				///< X-size of bitmap in pixels.
	int ySize;				///< Y-size of bitmap in pixels.

	///
	/// \brief The surface to use for the meter frame.
	///
	SURFHANDLE FrameSurface;

	double AdjustForPower(double val) { return val; } ///< These are always powered by definition.
};

///
/// \brief DC voltage meter for control panel.
///
/// \image html DCVolts.bmp "DC voltage meter"
///
/// This meter displays the DC voltage of one of the numerous DC systems in the CSM or LEM (e.g. the main buses
/// or batteries).
///
/// \ingroup PanelItems
///
class DCVoltMeter: public ElectricMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum voltage to display (meter may show beyond it).
	/// \param maxVal Maximum voltage to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum voltage.
	/// \param vMax Angle of meter at maximum voltage.
	///
	DCVoltMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Query the voltage.
	/// \return Current voltage.
	///
	double QueryValue();
};

///
/// \brief DC current meter for control panel.
///
/// \image html DCAmps.bmp "DC current meter"
///
/// This meter displays the DC current of one of the numerous DC systems in the CSM or LEM (e.g. the main buses
/// or batteries).
///
/// \ingroup PanelItems
///
class DCAmpMeter: public ElectricMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum current to display (meter may show beyond it).
	/// \param maxVal Maximum current to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum current.
	/// \param vMax Angle of meter at maximum current.
	///
	DCAmpMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Query the voltage.
	/// \return Current voltage.
	///
	double QueryValue();
};

///
/// \brief AC voltage meter for control panel.
///
/// \image html ACVolts.bmp "ACVolts meter"
///
/// This meter displays the AC voltage on one phase of one of the CSM AC buses.
///
/// \ingroup PanelItems
///
class ACVoltMeter: public ElectricMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum voltage to display (meter may show beyond it).
	/// \param maxVal Maximum voltage to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum voltage.
	/// \param vMax Angle of meter at maximum voltage.
	///
	ACVoltMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Query the voltage.
	/// \return Current voltage.
	///
	double QueryValue();
};

class DSKYPushSwitch: public PushSwitch {
protected:
	virtual void DoDrawSwitch(SURFHANDLE DrawSurface);
};

///
/// This is going to become the core of the panel callback system.  It follows a standard
/// functor model.  This allows each switch to call into the appropriate system rather than
/// the old style switch/case system that was used on a listener to update systems.
/// \brief Panel Item callback system.
/// \ingroup PanelItems
///
class PanelSwitchCallbackInterface
{
public:
	virtual void call(PanelSwitchItem* s) = 0;
};

///
/// Actual Functor for the Panel Switch Callback system.
/// \brief Panel Item callback system.
/// \ingroup PanelItems
///
template <class T> class PanelSwitchCallback : public PanelSwitchCallbackInterface
{
private:
	T* obj_ptr;
	void (T::*func_ptr)(PanelSwitchItem* s);
public:
	PanelSwitchCallback(T* ptr_to_obj, void (T::*ptr_to_func)(PanelSwitchItem* s))
	{
		obj_ptr = ptr_to_obj;
		func_ptr = ptr_to_func;
	}
	virtual void call(PanelSwitchItem* s)
	{
		(*obj_ptr.*func_ptr)(s);
	}
};
#endif
