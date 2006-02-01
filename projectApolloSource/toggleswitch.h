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

class PanelSwitchItem: public e_object {

public:
	PanelSwitchItem();
	void SetNext(PanelSwitchItem *s) { next = s; };
	PanelSwitchItem *GetNext() { return next; };
	void SetNextForScenario(PanelSwitchItem *s) { nextForScenario = s; };
	PanelSwitchItem *GetNextForScenario() { return nextForScenario; };
	void SetFailed(bool fail, int fail_state = 0) { Failed = fail; FailedState = fail_state; };
	bool IsFailed() { return Failed; };
	char *GetName() { return name; }

	virtual bool CheckMouseClick(int event, int mx, int my) = 0;
	virtual void DrawSwitch(SURFHANDLE DrawSurface) = 0;
	virtual void SaveState(FILEHANDLE scn) = 0;
	virtual void LoadState(char *line) = 0;

protected:
	bool Failed;
	int FailedState;
	char *name;
	PanelSwitchItem *next;
	PanelSwitchItem *nextForScenario;
};


class ToggleSwitch: public PanelSwitchItem {

public:
	ToggleSwitch();
	virtual ~ToggleSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int springloaded = SPRINGLOADEDSWITCH_NONE);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s,
		      int xoffset = 0, int yoffset = 0);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row,
		      int xoffset = 0, int yoffset = 0);
	void SetSize(int w, int h) { width = w; height = h; };
	void SetPosition(int xp, int yp) { x = xp; y = yp; };
	void SetState(bool s) { state = s; };
	void SetOffset(int xo, int yo) {xOffset = xo; yOffset = yo; };
	void SetSpringLoaded(int springloaded) { springLoaded = springloaded; }; 
	virtual int GetState();
	void SetActive(bool s);
	void SetVisible(bool v) {visible = v; };
	bool Toggled() { return SwitchToggled; };
	void ClearToggled() { SwitchToggled = false; };
	
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
	void DoDrawSwitch(SURFHANDLE DrawSurface);
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
	bool visible;

	SURFHANDLE SwitchSurface;
	VESSEL *OurVessel;
	Sound Sclick;

	SwitchRow *switchRow;
};


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

class NavModeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, VESSEL *v, int mode, SoundLib &s);

protected:
	int NAVMode;
};


class HUDToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int mode, SoundLib &s);

protected:
	int	HUDMode;
};


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

class ThreeSourceSwitch : public ThreePosSwitch {
public:
	ThreeSourceSwitch() { source1 = source2 = source3 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3);
	bool CheckMouseClick(int event, int mx, int my);
	void LoadState(char *line);

protected:
	virtual void UpdateSourceState();

	e_object *source1;
	e_object *source2;
	e_object *source3;
};

class TwoSourceSwitch : public ToggleSwitch {
public:
	TwoSourceSwitch() { source1 = source2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, e_object *s1, e_object *s2);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);
	void LoadState(char *line);

protected:
	virtual void UpdateSourceState();

	e_object *source1;
	e_object *source2;
};

class TwoOutputSwitch : public ToggleSwitch {
public:
	TwoOutputSwitch() { output1 = output2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, e_object *o1, e_object *o2);
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
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3, e_object *d1, e_object *d2);

protected:
	void UpdateSourceState();

	e_object *dest1;
	e_object *dest2;
};

//
// Mission Timer switches.
//

class MissionTimer; // Forward reference for files which include this before missiontimer.h

class MissionTimerSwitch: public ThreePosSwitch {

public:
	MissionTimerSwitch() { timer = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, MissionTimer *ptimer);

protected:
	MissionTimer *timer;
};

class TimerControlSwitch: public MissionTimerSwitch {

public:
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	void SetTimer();
};

class TimerUpdateSwitch: public MissionTimerSwitch {

public:
	TimerUpdateSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int adjuster, MissionTimer *ptimer);
	bool CheckMouseClick(int event, int mx, int my);

protected:
	void AdjustTime(int val);

	int adjust_type;
};

//
// Switch that starts and stops the event timer.
//

class EventTimerControlSwitch: public MissionTimerSwitch {

public:
	bool CheckMouseClick(int event, int mx, int my);

};

//
// Switch that controls up/down/reset for event timer.
//

class EventTimerResetSwitch: public MissionTimerSwitch {

public:
	bool CheckMouseClick(int event, int mx, int my);

};

class CautionWarningSystem; // Forward reference for files which include this before cautionwarning.h

class CWSThreePosSwitch: public ThreePosSwitch {

public:
	CWSThreePosSwitch() { cws = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, CautionWarningSystem *c);

protected:
	CautionWarningSystem *cws;
};

class CWSLightTestSwitch: public CWSThreePosSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
};


class PushSwitch: public ToggleSwitch {

public:
	bool CheckMouseClick(int event, int mx, int my);

	int operator=(const int b) { state = b; return state; };

protected:
	virtual void InitSound(SoundLib *s);
};


class CircuitBrakerSwitch: public ToggleSwitch {

public:
	CircuitBrakerSwitch() { MaxAmps = 0.0; };

	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, e_object *s = 0, double amps = 30.0);

	double Voltage();
	void DrawPower(double watts);

	int operator=(const int b) { state = b; return state; };

protected:
	virtual void InitSound(SoundLib *s);
	double MaxAmps;
};


//
// Caution and warning system mode switch.
//

class CWSModeSwitch: public CWSThreePosSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);
};

//
// Caution and warning system power switch.
//

class CWSPowerSwitch: public CWSThreePosSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	void SetPowerBus();
};

//
// Caution and warning system source switch.
//

class CWSSourceSwitch: public ToggleSwitch {
public:
	CWSSourceSwitch() { cws = 0; };
	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, CautionWarningSystem *c);

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
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, ApolloGuidance *c);

protected:
	ApolloGuidance *agc;
};

class AGCThreePoswitch: public ThreePosSwitch {

public:
	AGCThreePoswitch() { agc = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, ApolloGuidance *c);

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

class GuardedTwoOutputSwitch : public GuardedToggleSwitch {
public:
	GuardedTwoOutputSwitch() { output1 = output2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, e_object *o1, e_object *o2);
	bool CheckMouseClick(int event, int mx, int my);
	void LoadState(char *line);

protected:
	virtual void UpdateSourceState();

	e_object *output1;
	e_object *output2;
};

class IMU; // Forward reference for files which include this before IMU.h

class IMUCageSwitch: public GuardedToggleSwitch {

public:
	IMUCageSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, IMU *im);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	IMU *imu;

	void SetIMU();
};

class UnguardedIMUCageSwitch: public ToggleSwitch {

public:
	UnguardedIMUCageSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, IMU *im);
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

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState);
	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf,
				   int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetGuardState() { return guardState; };
	void SetGuardState(bool s) { guardState = s; };

	int operator=(const int b) { state = b; return state; };

protected:
	int	guardX;
	int guardY;
	int guardWidth;
	int guardHeight;
	int guardState;
	SURFHANDLE guardSurface;
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
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row);
	void AddPosition(int value, double angle);
	void DrawSwitch(SURFHANDLE drawSurface);
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
	FDAIPowerRotationalSwitch() { FDAI1 = FDAI2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, FDAI *F1, FDAI *F2);

	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newValue);
	void LoadState(char *line);

protected:
	void CheckFDAIPowerState();

	FDAI *FDAI1;
	FDAI *FDAI2;
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
	int operator=(const bool b) { state = b; return state; };
	operator bool() {return state; };

protected:
	bool state;
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
	bool DrawRow(int id, SURFHANDLE DrawSurface);
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
	bool DrawRow(int id, SURFHANDLE DrawSurface);
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

