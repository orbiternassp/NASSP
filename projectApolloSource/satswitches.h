/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn-specific switches

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
  *	Revision 1.2  2005/08/23 20:13:12  movieman523
  *	Added RCS talkbacks and changed AGC to use octal addresses for EMEM.
  *	
  *	Revision 1.1  2005/08/16 20:55:23  movieman523
  *	Added first saturn-specific switch for Xlunar Inject.
  *	
  **************************************************************************/

class Saturn;

class SaturnToggleSwitch : public ToggleSwitch {
public:
	SaturnToggleSwitch() { sat = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, Saturn *s);

protected:
	Saturn *sat;
};

class SaturnThreePosSwitch : public ThreePosSwitch {
public:
	SaturnThreePosSwitch() { sat = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, Saturn *s);

protected:
	Saturn *sat;
};

class SaturnValveSwitch: public SaturnThreePosSwitch {
public:
	SaturnValveSwitch() { Valve = 0; Indicator = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, Saturn *s, int valve, IndicatorSwitch *ind);
	bool CheckMouseClick(int event, int mx, int my);

protected:
	int Valve;
	IndicatorSwitch *Indicator;
};

class XLunarSwitch : public SaturnToggleSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
};

class SaturnSPSSwitch : public SaturnThreePosSwitch {
public:
	bool CheckMouseClick(int event, int mx, int my);
};
