/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2024

  ORBITER vessel module: EASEP LR3

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

static const DWORD ntdvtx = 3;
static TOUCHDOWNVTX tdvtx[ntdvtx];

class EasepLR3 : public VESSEL2 {
public:
	EasepLR3(OBJHANDLE hObj, int fmodel);
	virtual ~EasepLR3();
	void clbkPreStep(double SimT, double SimDT, double mjd);
	void clbkPostCreation();
	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkVisualCreated(VISHANDLE vis, int refcount);
	void clbkVisualDestroyed(VISHANDLE vis, int refcount);
	void clbkLoadStateEx(FILEHANDLE scn, void* vs);

private:
	void DoFirstTimestep();
	bool spawned;

protected:
	OBJHANDLE hMaster;

};
