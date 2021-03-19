/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  Mission Control Center Vessel Virtual cockpit code

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

  // To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "mccvessel.h"

MESHHANDLE hMCCVC;

#define MCCVIEW_SEAT1		0
#define MCCVIEW_SEAT2		1

void MCCVessel::LoadVC() 
{
	//hMCCVC = oapiLoadMeshGlobal("ProjectApollo/LM_VC");
	//int vcidx = AddMesh(hMCCVC, &_V(0, 0, 0));
	//SetMeshVisibilityMode(vcidx, MESHVIS_VC);
}

bool MCCVessel::clbkLoadVC(int id) 
{

	switch (id) {

	case MCCVIEW_SEAT1:
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		oapiVCSetNeighbours(-1, MCCVIEW_SEAT2, -1, -1);
		SetCameraOffset(_V(-1,0,0));
		return true;

	case MCCVIEW_SEAT2:
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		oapiVCSetNeighbours(MCCVIEW_SEAT1, -1, -1, -1);
		SetCameraOffset(_V(1, 0, 0));
		return true;

	default:
		return false;
	}
}