/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER MODULE: ApolloSaturn
  Class interface for Apollo Saturn vessel class module

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
  *	Revision 1.2  2008/04/11 11:49:04  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.1  2005/02/11 12:17:54  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef __ApolloSaturn_H
#define __ApolloSaturn_H

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"


const double FHATCH_OPERATING_SPEED = 0.1;
// Opening/closing speed of outer airlock (1/sec)
// => cycle = 10 sec

const int nsurf = 4; // number of bitmap handles

// ==========================================================
// Interface for derived vessel class: ApolloSaturn
// ==========================================================

class ApolloSaturn: public VESSEL {
public:
	ApolloSaturn (OBJHANDLE hObj, int fmodel);
	~ApolloSaturn ();
	void LoadState (FILEHANDLE scn, VESSELSTATUS *vs);
	void SaveState (FILEHANDLE scn);
	void SetClassCaps (FILEHANDLE cfg);
	void DefineAnimations ();
	void ReleaseSurfaces();
	void Timestep (double simt);
	enum DoorStatus { DOOR_CLOSED, DOOR_OPEN, DOOR_CLOSING, DOOR_OPENING } nose_status, gear_status, lock_status;
	void ActivateAirlock (DoorStatus action);
	double hatch_proc;     // logical status
	UINT anim_hatch;     // handle for airlock animation

#endif // !__ApolloSaturn_H