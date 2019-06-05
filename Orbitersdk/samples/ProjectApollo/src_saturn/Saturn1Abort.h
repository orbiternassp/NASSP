/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2004-2005

Saturn IB Abort Stage 2

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

#pragma once

class Sat1Abort1 : public VESSEL3 {

public:

	Sat1Abort1(OBJHANDLE hObj, int fmodel);
	virtual ~Sat1Abort1();
	void init();
	void Setup();
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	virtual void SetState(bool sm, bool lowres, int payload);
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void DefineAnimations();

	void clbkSetClassCaps(FILEHANDLE cfg);

protected:

	double RotationLimit;			///< Panel rotation limit from 0.0 to 1.0 (1.0 = 180 degrees).

	bool SmPresent;                 ///< SM flag
	bool LowRes;                    ///< Mesh resolution flag
	int PayloadType;				///< Payload type.

	THGROUP_HANDLE thg_sep;
	UINT panelAnim;
	int panelMesh1Saturn1b, panelMesh2Saturn1b, panelMesh3Saturn1b, panelMesh4Saturn1b;
	double panelProc;
	int panelTimestepCount;
	void AddSepJunk();
};