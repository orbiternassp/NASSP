/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2023 Matthew Hume


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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/NASSP-LICENSE.txt
  for more details.

  **************************************************************************/
#define ORBITER_MODULE
#include "skylab.h"


Skylab::Skylab(OBJHANDLE hObj, int fmodel): VESSEL4(hObj, fmodel)
{
	
}

Skylab::~Skylab() {

}

void Skylab::InitSkylab() {
	SkylabMesh = oapiLoadMeshGlobal("ProjectApollo/sat5skylab");
	UINT meshidx;
	VECTOR3 mesh_dir = _V(0, 0, 0);
	meshidx = AddMesh(SkylabMesh, &mesh_dir);
	SetMeshVisibilityMode(meshidx, MESHVIS_ALWAYS);
}

void Skylab::clbkPostCreation() {
	Skylab::InitSkylab();
}

void Skylab::clbkPreStep(double simt, double simdt, double mjd)
{
}

void Skylab::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL4::clbkSetClassCaps(cfg);
}

void Skylab::clbkSaveState(FILEHANDLE scn) {
	return;
}

void Skylab::clbkLoadState(FILEHANDLE scn) {

}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	Skylab* skylab;
	skylab = new Skylab(hvessel, flightmodel);

	return static_cast<VESSEL*> (skylab);
}


DLLCLBK void ovcExit(VESSEL* vessel)
{
	if (vessel) {
		delete (Skylab*)vessel;
	}
}


