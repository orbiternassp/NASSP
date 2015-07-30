/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Floodlight vessel

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

#define MAXEXHAUST	16

class Floodlight: public VESSEL2 {

public:
	Floodlight(OBJHANDLE hObj, int fmodel);
	virtual ~Floodlight();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPostCreation();
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	int clbkConsumeDirectKey(char *kstate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);

protected:
	bool firstTimestepDone;
	double touchdownPointHeight;
	SoundLib soundlib;
	THRUSTER_HANDLE th;
	PROPELLANT_HANDLE ph;
	SURFHANDLE tex;

	int exhausts;
	UINT exhaust[MAXEXHAUST];
	LightEmitter *exhaustLight[MAXEXHAUST];
	VECTOR3 exhaustPos[MAXEXHAUST];
	double exhaustDir[MAXEXHAUST][2];
	double exhaustSize[MAXEXHAUST][2];
	bool exhaustsEnabled;
	int configMode;
	int currentExhaust;

	void DoFirstTimestep();
	void SetTouchdownPointHeight(double height);
	void SetExhaust(int index);
	MATRIX3 GetRotationMatrixY(double angle);
	MATRIX3 GetRotationMatrixZ(double angle);
};
