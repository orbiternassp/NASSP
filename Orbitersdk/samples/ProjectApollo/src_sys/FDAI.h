/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Rob Conley, Markus Joachim

  This class is used to display a FDAI on the panel.

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

/// \bug Avoids bug in VC++
#pragma once

#include < GL\gl.h >                                
#include < GL\glu.h >

class FDAI {

public:
	FDAI();
	virtual ~FDAI();
	void RegisterMe(int index, int x, int y);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void PaintMe(VECTOR3 attitude, int no_att, VECTOR3 rates, VECTOR3 errors, int ratescale, SURFHANDLE surf, SURFHANDLE hFDAI, 
		         SURFHANDLE hFDAIRoll, SURFHANDLE hFDAIOff, SURFHANDLE hFDAINeedles, HBITMAP hBmpRoll, int smooth);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	bool IsPowered();
	void SetLMmode();
	void WireTo(e_object *dc, e_object *ac) { DCSource = dc; ACSource = ac; };
	bool LM_FDAI;
	void WireTo(e_object *dc) { DCSource = dc; noAC = true; };

protected:
	int ScrX;
	int ScrY;			//coords on screen
	int idx;			//index on the panel list 
	int init;
	int list_name; //we store the rendering into a display list
	VECTOR3 now, target, lastRates, lastErrors;
	double lastPaintTime;
	bool newRegistered;

	//some stuff for OpenGL
	HDC hDC2;
	HGLRC hRC;
	HBITMAP hBMP;
	HBITMAP hBMP_old;
	GLUquadricObj *quadObj;

	e_object *DCSource, *ACSource;
	bool noAC;

	void InitGL();
	void MoveBall();
	void SetAttitude(VECTOR3 attitude);
	int LoadOGLBitmap(char *filename);
};

//
// Strings for state saving.
//

#define FDAI_START_STRING	"FDAI_BEGIN"
#define FDAI_END_STRING		"FDAI_END"

#define FDAI2_START_STRING	"FDAI2_BEGIN"
#define FDAI2_END_STRING	"FDAI2_END"
