/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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

class Saturn5_LEVA: public VESSEL {

public:
	Saturn5_LEVA(OBJHANDLE hObj, int fmodel);
	virtual ~Saturn5_LEVA();
	int ConsumeKey(const char *keystate);
	void SetAstroStage ();
	void SetRoverStage ();
	void init();
	void Timestep(double simt);
	void LoadState(FILEHANDLE scn, VESSELSTATUS *vs);
	void SaveState(FILEHANDLE scn);

private:

	void ScanMotherShip();
	void MoveEVA();
	void SetFlag();

protected:

	OBJHANDLE hMaster;
	bool GoDock1;
	bool GoHover;
	bool starthover;
	bool GoRover;
	bool Astro;
	bool MotherShip;
	char EVAName[256];
	char CSMName[256];
	char MSName[256];
	bool KEY1;
	bool KEY2;
	bool KEY3;
	bool KEY4;
	bool KEY5;
	bool KEY6;
	bool KEY7;
	bool KEY8;
	bool KEY9;
	bool GoFlag;
	bool Flagtrue;
};