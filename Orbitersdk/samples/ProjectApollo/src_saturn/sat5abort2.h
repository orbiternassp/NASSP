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

  **************************************************************************/

class Sat5Abort2: public VESSEL3 {

public:

	Sat5Abort2 (OBJHANDLE hObj, int fmodel);
	virtual ~Sat5Abort2();
	void init();
	void Setup();
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	virtual void SetState(bool sm, bool lr, int pl);
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void DefineAnimations();

	void clbkSetClassCaps(FILEHANDLE cfg);

protected:

	bool PanelsOpened;
	double RotationLimit;			///< Panel rotation limit from 0.0 to 1.0 (1.0 = 180 degrees).

	bool smpresent;                 ///< SM flag
	bool LowRes;                    ///< Mesh resolution flag
	int PayloadType;				///< Payload type.

	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;

	THGROUP_HANDLE thg_sep, thg_sepPanel;
	UINT panelAnim;
	double panelProc;
	int panelTimestepCount;
	int panelMesh1SaturnV, panelMesh2SaturnV, panelMesh3SaturnV, panelMesh4SaturnV;
	int panelMesh1SaturnVLow, panelMesh2SaturnVLow, panelMesh3SaturnVLow, panelMesh4SaturnVLow;

	void AddSepJunk();
	void HidePanelMeshes();

	//
	// We don't actually seem to use any of these variables, even though they're
	// set. Maybe this isn't neccesary at all?
	//

	int stage;
	int status;
	double Offset1st;

};
