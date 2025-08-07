/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2025

  ORBITER vessel module: Flag

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

  **************************************************************************/

//These are my best guess based on this https://www.nasa.gov/history/alsj/ApolloFlags-Condition.html and LROC quickmap
//a11	8.2m	fell
//a12	17.3m	standing
//a14	9.5m?	probably fell?
//a15	13.6m	standing
//a16	16.5m	standing
//a17	19.7m	standing
static const double fallDistance = 8.25;
static const double safeDistance = 13.5;

static const DWORD ntdvtx = 3;
static TOUCHDOWNVTX tdvtx[ntdvtx];

class Flag : public VESSEL2 {
public:
	Flag(OBJHANDLE hObj, int fmodel);
	virtual ~Flag();
	void clbkPreStep(double SimT, double SimDT, double mjd);
	void clbkPostCreation();
	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkVisualCreated(VISHANDLE vis, int refcount);
	void clbkVisualDestroyed(VISHANDLE vis, int refcount);
	void clbkLoadStateEx(FILEHANDLE scn, void* vs);
	void clbkSaveState(FILEHANDLE scn);

private:
	void DoFirstTimestep();
	OBJHANDLE FindLM();
	bool DetectLiftoff();
	void GetVectorToLM();
	void RotateFlag();
	bool CalcFlagFall();
	void DefineFallAnimation();
	void MoveFlagFallAnimation(double simdt);
	bool firstTimestep;
	OBJHANDLE hLM;
	bool liftedOff;
	VECTOR3 vectorToLM;
	double distanceToLM;
	bool doFlagFall;
	double flagFallProc;
	UINT animFlagFall;

protected:
	OBJHANDLE hMaster;

};
