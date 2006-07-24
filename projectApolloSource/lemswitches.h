/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: LEM-specific switches

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
  *	Revision 1.2  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  **************************************************************************/

class sat5_lmpkd;
class LEM_ECA;
class LEM_INV;

class LEMThreePosSwitch : public ThreePosSwitch {
public:
	LEMThreePosSwitch() { lem = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, sat5_lmpkd *s);

protected:
	sat5_lmpkd *lem;
};

class LEMValveSwitch: public LEMThreePosSwitch {
public:
	LEMValveSwitch() { Valve = 0; Indicator = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, sat5_lmpkd *s, int valve, IndicatorSwitch *ind);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	void CheckValve(int s);

	int Valve;
	IndicatorSwitch *Indicator;
};

class LEMBatterySwitch: public LEMThreePosSwitch {
public:
	LEMBatterySwitch() { eca = NULL; srcno=0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, sat5_lmpkd *s,
		LEM_ECA *lem_eca, int src_no);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);

protected:
	void CheckValve(int s);

	int srcno;
	LEM_ECA *eca;
//	IndicatorSwitch *Indicator;
};

class LEMInverterSwitch: public LEMThreePosSwitch {
public:
	LEMInverterSwitch() { inv1 = NULL; inv2 = NULL; };
class LEM_ECA;	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, sat5_lmpkd *s, LEM_INV *lem_inv_1, LEM_INV *lem_inv_2);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newState);
	bool ChangeState(int newState);

protected:
	void CheckValve(int s);
	LEM_INV *inv1;
	LEM_INV *inv2;
};


class LEMValveTalkback : public IndicatorSwitch {
public:
	LEMValveTalkback();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vlv, sat5_lmpkd *v);
	int GetState();

protected:
	int Valve;
	sat5_lmpkd *our_vessel;
};