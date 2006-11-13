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

#include "cautionwarning.h"

#define TOGGLESWITCH_DOWN		0
#define TOGGLESWITCH_UP			1

#define THREEPOSSWITCH_DOWN		0
#define THREEPOSSWITCH_CENTER	1
#define THREEPOSSWITCH_UP		2

#define SPRINGLOADEDSWITCH_NONE					0
#define SPRINGLOADEDSWITCH_DOWN					1
#define SPRINGLOADEDSWITCH_CENTER				2
#define SPRINGLOADEDSWITCH_UP					3
#define SPRINGLOADEDSWITCH_CENTER_SPRINGUP		4
#define SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN	5

#define PANELSWITCH_START_STRING	"PANELSWITCHES_BEGIN"
#define PANELSWITCH_END_STRING		"PANELSWITCHES_END"

#define TIME_UPDATE_SECONDS	0
#define TIME_UPDATE_MINUTES 1
#define TIME_UPDATE_HOURS	2

class SwitchRow;
class PanelSwitchScenarioHandler;

#include "powersource.h"

///
/// This is the base class for panel items. Items using this class can be looked up by name,
/// and are automatically saved into scenario files and loaded from them without us having to
/// individually track each one.
/// \brief Panel item base class.
/// \ingroup PanelItems
///
class PanelSwitchItem: public e_object {

public:
	PanelSwitchItem();
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
	char *GetName() { return name; }

	virtual bool CheckMouseClick(int event, int mx, int my) = 0;
	virtual void DrawSwitch(SURFHANDLE DrawSurface) = 0;
	virtual void SaveState(FILEHANDLE scn) = 0;
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

	void SetVisible(bool v) {visible = v; };

protected:
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
	virtual void SetState(bool s) { state = s; };
	void SetOffset(int xo, int yo) {xOffset = xo; yOffset = yo; };
	void SetSpringLoaded(int springloaded) { springLoaded = springloaded; }; 
	virtual int GetState();
	void SetActive(bool s);

	bool Toggled() { return SwitchToggled; };
	void ClearToggled() { SwitchToggled = false; };
	
	void DrawFlash(SURFHANDLE DrawSurface);
	void SetBorderSurface(SURFHANDLE border) { BorderSurface = border; };

	virtual bool IsUp() { return (GetState() == 1); };
	virtual bool IsDown() { return (GetState() == 0); };
	virtual bool IsCenter() { return false; };

	virtual bool SwitchTo(int newState);
	virtual void DrawSwitch(SURFHANDLE DrawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
	virtual void SaveState(FILEHANDLE scn);
	virtual void LoadState(char *line);

	//
	// Operator overloads so we don't need to call GetState() and SetState() all
	// the time.
	//

	//
	// Note: now that we have integer states rather than bool states, we should stop
	// using these operators and go back to calling GetState(). At some point I'm
	// going to delete all of these and remove the code which uses them.
	//

    bool operator=(const bool b) { state = (int) b; return b; };
	int operator=(const int b) { state = b; return state; };
	unsigned operator=(const unsigned b) { state = b; return (unsigned)state; };
	bool operator!() { return !GetState(); };
	bool operator&&(const bool b) { return (GetState() && b); };
	bool operator||(const bool b) { return (GetState() || b); };

	operator bool() { return (GetState() != 0); };
	operator int() { return (int) GetState(); };
	operator unsigned() { return (unsigned) GetState(); };

protected:
	virtual void InitSound(SoundLib *s);
	virtual void DoDrawSwitch(SURFHANDLE DrawSurface);
	bool DoCheckMouseClick(int event, int mx, int my);

	int	x;
	int y;
	int width;
	int height;
	int xOffset;
	int yOffset;

	int state;
	int springLoaded;
	bool Active;
	bool SwitchToggled;

	SURFHANDLE SwitchSurface;
	SURFHANDLE BorderSurface;
	VESSEL *OurVessel;
	Sound Sclick;

	SwitchRow *switchRow;
};

///
/// \brief Orbiter attitude-control toggle switch.
/// \ingroup PanelItems
///
class AttitudeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);

	//
	// I don't understand why this isn't inherited properly from ToggleSwitch?
	// Answer from the language reference:
	// All overloaded operators except assignment (operator=) are inherited by derived classes.
	unsigned operator=(const unsigned b) { state = (b != 0); return (unsigned)state; };
};

///
/// \brief Orbiter attitude-control toggle switch.
/// \ingroup PanelItems
///
class NavModeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, VESSEL *v, int mode, SoundLib &s);

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

protected:
	int	HUDMode;
};

///
/// Generic three-positiont toggle switch
/// \brief Three position toggle switch base class.
/// \ingroup PanelItems
///
class ThreePosSwitch: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

	bool IsDown() { return (GetState() == THREEPOSSWITCH_DOWN); };
	bool IsCenter() { return (GetState() == THREEPOSSWITCH_CENTER); };
	bool IsUp() { return (GetState() == THREEPOSSWITCH_UP); };
	void SetState(int s) { state = s; };

	int operator=(const int b) { state = b; return state; };
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
	bool CheckMouseClick(int event, int mx, int my);
	void LoadState(char *line);
	bool SwitchTo(int newState);

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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);
	void LoadState(char *line);

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
	bool CheckMouseClick(int event, int mx, int my);
	void LoadState(char *line);

protected:
	virtual void UpdateSourceState();

	e_object *output1;
	e_object *output2;
};

class ThreeSourceTwoDestSwitch : public ThreeSourceSwitch {
public:
	ThreeSourceTwoDestSwitch() { dest1 = dest2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3, e_object *d1, e_object *d2);

protected:
	void UpdateSourceState();

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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

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
	bool CheckMouseClick(int event, int mx, int my);

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
	bool CheckMouseClick(int event, int mx, int my);

};

///
/// Switch that controls count up/count down/reset for event timer.
/// \brief Event timer count/reset switch.
/// \ingroup PanelItems
///
class EventTimerResetSwitch: public MissionTimerSwitch {

public:
	bool CheckMouseClick(int event, int mx, int my);

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
	bool CheckMouseClick(int event, int mx, int my);
};

///
/// A two-position switch which is pushed in/out rather than toggled up/down.
/// \brief Two-position push switch.
/// \ingroup PanelItems
///
class PushSwitch: public ToggleSwitch {

public:
	bool CheckMouseClick(int event, int mx, int my);

	int operator=(const int b) { state = b; return state; };

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
	void DrawPower(double watts);

	int operator=(const int b) { state = b; return state; };

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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);
};

///
/// A three-position switch controlling caution and warning system power.
/// \brief CWS power switch.
/// \ingroup PanelItems
///
class CWSPowerSwitch: public CWSThreePosSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

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
	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c);

protected:
	///
	/// \brief The caution and warning system that this switch controls.
	///
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
	bool CheckMouseClick(int event, int mx, int my);
	void SetChannelData(int chan, int bit, bool value) { Channel = chan; Bit = bit; UpValue = value; };

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
	bool CheckMouseClick(int event, int mx, int my);
};

class PGNSSwitch : public AGCThreePoswitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
};

class GuardedToggleSwitch: public ToggleSwitch {

public:
	GuardedToggleSwitch();
	virtual ~GuardedToggleSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, int springloaded = SPRINGLOADEDSWITCH_NONE);
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

	int operator=(const int b) { state = b; return state; };

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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);
	void LoadState(char *line);

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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);
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
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	IMU *imu;

	void SetIMU();
};

class UnguardedIMUCageSwitch: public ToggleSwitch {

public:
	UnguardedIMUCageSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

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

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState);
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

	void SetLit(bool l) { lit = l; };
	bool IsLit() { return lit; };

	int operator=(const int b) { state = b; return state; };

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

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, 
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

	int operator=(const int b) { state = b; return state; };

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

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultValue);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void AddPosition(int value, double angle);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawFlash(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newValue);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetState();
	int operator=(const int b);
	operator int();

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
	RotationalSwitchBitmap bitmaps[RotationalSwitchBitmapCount];
	SwitchRow *switchRow;

	void SetValue(int newValue);
	double AngleDiff(double a1, double a2);
	void DeletePositions();
};


class FDAI;

class FDAIPowerRotationalSwitch: public RotationalSwitch {
public:
	FDAIPowerRotationalSwitch() { FDAI1 = FDAI2 = NULL; ACSource1 = ACSource2 = DCSource1 = DCSource2 = NULL; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, FDAI *F1, FDAI *F2, 
		      e_object *dc1, e_object *dc2, e_object *ac1, e_object *ac2);

	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newValue);
	void LoadState(char *line);

protected:
	void CheckFDAIPowerState();

	FDAI *FDAI1, *FDAI2;
	e_object *DCSource1, *DCSource2, *ACSource1, *ACSource2;
};

class PowerStateRotationalSwitch: public RotationalSwitch {
public:
	PowerStateRotationalSwitch();

	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newValue);
	void LoadState(char *line);
	void SetSource(int num, e_object *s);

protected:
	void CheckPowerState();

	e_object *sources[16];
};

class IndicatorSwitch: public PanelSwitchItem {

public:
	IndicatorSwitch();
	virtual ~IndicatorSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, bool defaultState);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	virtual int GetState() { return state; };
	int operator=(const int b) { state = b; return state; };
	operator int() {return state; };

protected:
	int state; // Changed to INT for extended capabilities hackery
	double displayState;	//0: false, 1: moving, 2: moving, 3: true
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
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetState();
	int operator=(const int b);
	operator int();

protected:
	int	x;
	int y;
	int width;
	int height;
	int state;
	int maxState;
	SURFHANDLE switchSurface;
	Sound sclick;
	SwitchRow *switchRow;
};

class SoundLib;

class VolumeThumbwheelSwitch: public ThumbwheelSwitch {

public:
	VolumeThumbwheelSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vclass, SoundLib *s);
	bool CheckMouseClick(int event, int mx, int my);
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

protected:
	PanelSwitchItem *SwitchList;
	SwitchRow *RowList;
	int PanelArea;
	PanelSwitches *panelSwitches;

	e_object *RowPower;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class PushSwitch;
	friend class GuardedToggleSwitch;
	friend class GuardedThreePosSwitch;
	friend class GuardedPushSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
	friend class ThumbwheelSwitch;
	friend class CircuitBrakerSwitch;
};

class PanelSwitchListener {

public:
	virtual void PanelSwitchToggled(ToggleSwitch *s) = 0;
	virtual void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) = 0;
	virtual void PanelRotationalSwitchChanged(RotationalSwitch *s) = 0;
	virtual void PanelThumbwheelSwitchChanged(ThumbwheelSwitch *s) = 0;
};

class PanelSwitches {

public:
	PanelSwitches() { PanelID = 0; RowList = 0; Realism = 0; };
	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn);
	void AddRow(SwitchRow *s) { s->SetNext(RowList); RowList = s; };
	void Init(int id, VESSEL *v, SoundLib *s, PanelSwitchListener *l) { PanelID = id; RowList = 0; vessel = v; soundlib = s; listener = l; };
	void SetRealism(int r) { Realism = r; };

protected:
	VESSEL *vessel;
	SoundLib *soundlib;
	PanelSwitchListener *listener;
	int	PanelID;
	SwitchRow *RowList;
	int Realism;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class PushSwitch;
	friend class GuardedToggleSwitch;
	friend class GuardedThreePosSwitch;
	friend class GuardedPushSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
	friend class ThumbwheelSwitch;
	friend class CircuitBrakerSwitch;
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

