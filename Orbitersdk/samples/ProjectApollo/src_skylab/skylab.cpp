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


Skylab::Skylab(OBJHANDLE hObj, int fmodel): ProjectApolloConnectorVessel(hObj, fmodel)
{
	
}

Skylab::~Skylab() {

}

void Skylab::InitSkylab() {
	SkylabMesh = oapiLoadMeshGlobal("ProjectApollo/Skylab1973/Skylab I");
	UINT meshidx;
	VECTOR3 mesh_dir = _V(0, 0, -7.925); //fix mesh scaling and geometry
	meshidx = AddMesh(SkylabMesh, &mesh_dir);
	SetMeshVisibilityMode(meshidx, MESHVIS_ALWAYS);
}

void Skylab::clbkPostCreation() {
	InitSkylab();
	ShiftCG(_V(0.066,0.6198,-6.1392)); //Initial CoM Relative to Vessel Coordinate System (Y,Z,X) in skylab coordinates
}

void Skylab::clbkPreStep(double simt, double simdt, double mjd)
{
}

void Skylab::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL4::clbkSetClassCaps(cfg);
	ClearExhaustRefs();
	ClearAttExhaustRefs();
	ClearThrusterDefinitions();

	SetSize(15);
	//Mass
	double mass = 88474; //https://ntrs.nasa.gov/api/citations/19730025115/downloads/19730025115.pdf#page=189
	SetEmptyMass(mass);
	//Principal Moment of Inertia
	//https://ntrs.nasa.gov/api/citations/19770014164/downloads/19770014164.pdf
	double PMI_X = 7.93321E5 / mass;
	double PMI_Y = 3.767828E6 / mass;
	double PMI_Z = 3.694680E6 / mass;
	SetPMI(_V(PMI_Y, PMI_Z, PMI_X));
	
	//Rough Drag Properties
	SetCrossSections(_V(79.46, 79.46, 79.46)); //From Skylab Operational Databook
	SetCW(2.401*2,2.590*2,4.384*2,9.330*2); //From Skylab Operational Databook, oversimplification
	SetRotDrag(_V(0.7, 0.7, 1.2)); //complete fabrication...
}

void Skylab::clbkSaveState(FILEHANDLE scn) {
	VESSEL4::clbkSaveState(scn);
}

void Skylab::clbkLoadState(FILEHANDLE scn) {
	VESSEL4::clbkSaveState(scn);
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	VESSEL* skylab;
	skylab = new Skylab(hvessel, flightmodel);

	return skylab;
}


DLLCLBK void ovcExit(VESSEL* vessel)
{
	if (vessel) {
		delete (Skylab*)vessel;
	}
}


