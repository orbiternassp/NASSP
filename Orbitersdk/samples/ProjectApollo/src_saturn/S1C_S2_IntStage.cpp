/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2025

  ORBITER vessel module: Saturn V Interstage

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

#include "S1C_S2_IntStage.h"

#define ORBITER_MODULE

S1C_S2_Interstage::S1C_S2_Interstage(OBJHANDLE hObj, int fmodel): ProjectApolloConnectorVessel(hObj, fmodel)
{
	hSIIDock = nullptr;
	hSICDock = nullptr;
	Sat1C_Sat2InterstageMesh = nullptr;
}

S1C_S2_Interstage::~S1C_S2_Interstage() {

}

void S1C_S2_Interstage::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetEmptyMass(1000.0);
	SetSize(10.0);
	//Sat1C_Sat2InterstageMesh = oapiLoadMeshGlobal("ProjectApollo/sat5intstg");
	Sat1C_Sat2InterstageMesh = oapiLoadMeshGlobal("ProjectApollo/sat5intstg4");
	//Sat1C_Sat2InterstageMesh = oapiLoadMeshGlobal("ProjectApollo/sat5intstg8");

	VECTOR3 mesh_dir = _V(0, 0, 0);
	unsigned int meshidx = AddMesh(Sat1C_Sat2InterstageMesh, &mesh_dir);
	SetMeshVisibilityMode(meshidx, MESHVIS_ALWAYS);

	hSIIDock = CreateDock(_V(0.0, 0.0, 2.1675), _V(0, 0, 1), _V(0, 1, 0));
	hSICDock = CreateDock(_V(0.0, 0.0, -3.3807), _V(0, 0, -1), _V(0, 1, 0));
}

void S1C_S2_Interstage::clbkPostCreation()
{
	
}

void S1C_S2_Interstage::clbkPreStep(double simt, double simdt, double mjd)
{
}

void S1C_S2_Interstage::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);
}

void S1C_S2_Interstage::SetupTouchdownPoints()
{
	double td_mass = 2214000.0;
	double td_width = 10.0;
	double td_tdph = -49.0;
	double td_height = 40.0;

	static DWORD ntdp = 4;
	static TOUCHDOWNVTX td[8];
	double stiffness = (-1) * (td_mass * 9.80655) / (3 * -0.05);
	double damping = 0.9 * (2 * sqrt(td_mass * stiffness));
	for (int i = 0; i < 4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = -cos(30 * RAD) * td_width;
	td[0].pos.y = -sin(30 * RAD) * td_width;
	td[0].pos.z = td_tdph;
	td[1].pos.x = 0;
	td[1].pos.y = 1 * td_width;
	td[1].pos.z = td_tdph;
	td[2].pos.x = cos(30 * RAD) * td_width;
	td[2].pos.y = -sin(30 * RAD) * td_width;
	td[2].pos.z = td_tdph;
	td[3].pos.x = 0;
	td[3].pos.y = 0;
	td[3].pos.z = td_tdph + td_height;

	SetTouchdownPoints(td, ntdp);
}

//void S1C_S2_Interstage::clbkLoadStateEx(FILEHANDLE scn, void* vstatus)
//{
//}
//
//void S1C_S2_Interstage::clbkGetRadiationForce(const VECTOR3& mflux, VECTOR3& F, VECTOR3& pos)
//{
//}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	return new S1C_S2_Interstage(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL* vessel)
{
	delete (S1C_S2_Interstage*)vessel;
}