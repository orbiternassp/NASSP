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

#define TOGGLESWITCH_DOWN		0
#define TOGGLESWITCH_UP			1

#define THREEPOSSWITCH_DOWN		0
#define THREEPOSSWITCH_CENTER	1
#define THREEPOSSWITCH_UP		2


class SwitchRow;

class PanelSwitchItem {

public:
	void SetNext(PanelSwitchItem *s) { next = s; };
	PanelSwitchItem *GetNext() { return next; };

	virtual bool CheckMouseClick(int event, int mx, int my) = 0;
	virtual void DrawSwitch(SURFHANDLE DrawSurface) = 0;

protected:
	PanelSwitchItem *next;

};

class ToggleSwitch: public PanelSwitchItem {

public:
	ToggleSwitch();
	virtual ~ToggleSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row);
	void SetSize(int w, int h) { width = w; height = h; };
	void SetPosition(int xp, int yp) { x = xp; y = yp; };
	void SetState(bool s) { state = s; };
	int GetState() { return state; };
	void SetActive(bool s);
	void SetVisible(bool v) {visible = v; };
	bool Toggled() { return SwitchToggled; };
	void ClearToggled() { SwitchToggled = false; };
	virtual bool IsUp() { return (state == 1); };
	virtual bool IsDown() { return (state == 0); };
	virtual bool IsCenter() { return false; };

	virtual void DrawSwitch(SURFHANDLE DrawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);

	//
	// Operator overloads so we don't need to call GetState() and SetState() all
	// the time.
	//

    bool operator=(const bool b) { state = (int) b; return b; };
	int operator=(const int b) { state = b; return state; };
	unsigned operator=(const unsigned b) { state = b; return (unsigned)state; };
	bool operator!() { return !state; };
	bool operator&&(const bool b) { return (state && b); };
	bool operator||(const bool b) { return (state || b); };

	operator bool() { return (state != 0); };
	operator int() { return (int) state; };
	operator unsigned() { return (unsigned) state; };

protected:

	void DoDrawSwitch(SURFHANDLE DrawSurface);
	bool DoCheckMouseClick(int mx, int my);

	int	x;
	int y;
	int width;
	int height;

	int state;
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
	bool IsCenter() { return (state == 1); };
	bool IsUp() { return (state == 2); };

	int operator=(const int b) { state = b; return state; };
};

class GuardedToggleSwitch: public ToggleSwitch {

public:
	GuardedToggleSwitch();
	virtual ~GuardedToggleSwitch();

	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf,
				   SoundLib &s, int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
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

	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf,
				   SoundLib &s, int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
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


typedef struct {
	double angle;
	int xOffset;
	int yOffset;
} RotationalSwitchBitmap;

const int RotationalSwitchBitmapCount = 20;


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

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row);
	void AddPosition(int value, double angle);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
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

	double AngleDiff(double a1, double a2);
	void DeletePositions();
};

class IndicatorSwitch: public PanelSwitchItem {

public:
	IndicatorSwitch();
	virtual ~IndicatorSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
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

class PanelSwitches;

class SwitchRow {

public:
	SwitchRow();
	virtual ~SwitchRow();

	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface);
	void AddSwitch(PanelSwitchItem *s) { s->SetNext(SwitchList); SwitchList = s; };
	void Init(int area, PanelSwitches &panel);
	SwitchRow *GetNext() { return RowList; };
	void SetNext(SwitchRow *s) { RowList = s; };

protected:
	PanelSwitchItem *SwitchList;
	SwitchRow *RowList;
	int PanelArea;
	PanelSwitches *panelSwitches;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
};

class PanelSwitchListener {

public:
	virtual void PanelSwitchToggled(ToggleSwitch *s) = 0;
	virtual void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) = 0;
};

class PanelSwitches {

public:
	PanelSwitches() { PanelID = 0; RowList = 0; };
	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface);
	void AddRow(SwitchRow *s) { s->SetNext(RowList); RowList = s; };
	void Init(int id, VESSEL *v, SoundLib *s, PanelSwitchListener *l) { PanelID = id; RowList = 0; vessel = v; soundlib = s; listener = l; };

protected:
	VESSEL *vessel;
	SoundLib *soundlib;
	PanelSwitchListener *listener;
	int	PanelID;
	SwitchRow *RowList;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
};
