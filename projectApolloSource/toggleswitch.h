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
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

class SwitchRow;

class ToggleSwitch {

public:
	ToggleSwitch();
	virtual ~ToggleSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, VESSEL *v, SoundLib &s);
	void SetSize(int w, int h) { width = w; height = h; };
	void SetPosition(int xp, int yp) { x = xp; y = yp; };
	void SetState(bool s) { state = s; };
	void SetNext(ToggleSwitch *s) { next = s; };
	ToggleSwitch *GetNext() { return next; };
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

	ToggleSwitch *next;
};

class AttitudeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);

	//
	// I don't understand why this isn't inherited properly from ToggleSwitch?
	//

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

class PanelSwitches;

class SwitchRow {

public:
	SwitchRow();
	virtual ~SwitchRow();

	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface);
	void AddSwitch(ToggleSwitch *s) { s->SetNext(SwitchList); SwitchList = s; };
	void Init(int area, PanelSwitches &panel);
	SwitchRow *GetNext() { return RowList; };
	void SetNext(SwitchRow *s) { RowList = s; };

protected:
	ToggleSwitch *SwitchList;
	SwitchRow *RowList;
	int PanelArea;

};

class PanelSwitches {

public:
	PanelSwitches() { PanelID = 0; RowList = 0; };
	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface);
	void AddRow(SwitchRow *s) { s->SetNext(RowList); RowList = s; };
	void Init(int id) { PanelID = id; RowList = 0; };

protected:
	int	PanelID;
	SwitchRow *RowList;

};