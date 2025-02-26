/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2024

  ORBITER vessel module: EASEP PSEP

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

#define ORBITER_MODULE

  // To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "EasepPSEP.h"

static 	int refcount = 0;
static MESHHANDLE hEasepPSEP;

EasepPSEP::EasepPSEP(OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)
{
	hMaster = hObj;
}

EasepPSEP::~EasepPSEP()
{

}

void EasepPSEP::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetEmptyMass(2);
	SetSize(1);
	SetPMI(_V(15, 15, 15));

	SetSurfaceFrictionCoeff(0.5, 0.5);
	SetRotDrag(_V(0, 0, 0));
	SetCW(0, 0, 0, 0);
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0);

	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();
	VECTOR3 mesh_adjust = _V(0.0, 0.0, 0.0);
	AddMesh(hEasepPSEP, &mesh_adjust);
	SetMeshVisibilityMode(0, MESHVIS_ALWAYS);

	double x_target = -0.001;
	double stiffness = (-1) * (2 * G) / (3 * x_target);
	double damping = 0.9 * (2 * sqrt(2 * stiffness));
	for (int i = 0; i < ntdvtx; i++)
	{
		tdvtx[i].damping = damping;
		tdvtx[i].mu = 3;
		tdvtx[i].mu_lng = 3;
		tdvtx[i].stiffness = stiffness;
	}
	tdvtx[0].pos.x = cos(30 * RAD) * 0.1;
	tdvtx[0].pos.y = -0.774;
	tdvtx[0].pos.z = -sin(30 * RAD) * 0.1;
	tdvtx[1].pos.x = 0;
	tdvtx[1].pos.y = -0.774;
	tdvtx[1].pos.z = 1.55;
	tdvtx[2].pos.x = -cos(30 * RAD) * 0.1;
	tdvtx[2].pos.y = -0.774;
	tdvtx[2].pos.z = -sin(30 * RAD) * 0.1;

	SetTouchdownPoints(tdvtx, ntdvtx);

	spawned = true;
}

void EasepPSEP::DoFirstTimestep()
{
	if (spawned) {
		RotateToFaceSun(GetSunPos());
		spawned = false;
	}
}

VECTOR3 EasepPSEP::GetSunPos()
{
	VECTOR3 rsun;
	VECTOR3 rsuninv;
	VECTOR3 rsunloc;
	GetGlobalPos(rsun);
	rsuninv = _V(-rsun.x, -rsun.y, -rsun.z);
	Global2Local(rsuninv, rsunloc);

	return rsunloc;
}

void EasepPSEP::RotateToFaceSun(VECTOR3 rsunloc)
{
	double heading = 0;
	oapiGetHeading(hMaster, &heading);
	if (heading >= PI2) heading -= PI2; //range always 0-2pi
	else if (heading < 0) heading += PI2;

	double sunangle = atan2(rsunloc.x, rsunloc.z); //current angle of sun on XY plane relative to z axis

	double heading1 = heading;
	double heading2 = heading;
	heading1 += sunangle;
	heading2 += (sunangle - PI);

	if ((heading >= PI05) && (heading <= (PI05 * 3))) {
		heading = heading2;
	}
	else {
		heading = heading1;
	}

	VESSELSTATUS vs1;
	GetStatus(vs1);
	vs1.vdata[0].z = heading;
	DefSetState(&vs1);
}

void EasepPSEP::clbkPreStep(double SimT, double SimDT, double mjd)
{
	if (spawned) {
		DoFirstTimestep();
		return;
	}
}

void EasepPSEP::clbkPostCreation()
{

}

void EasepPSEP::clbkLoadStateEx(FILEHANDLE scn, void* vs)
{
	char* line;
	while (oapiReadScenario_nextline(scn, line))
	{
		ParseScenarioLineEx(line, vs);
	}

	spawned = false;
}

void EasepPSEP::clbkVisualCreated(VISHANDLE vis, int refcount)
{

}

void EasepPSEP::clbkVisualDestroyed(VISHANDLE vis, int refcount)
{

}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hEasepPSEP = oapiLoadMeshGlobal("ProjectApollo/EASEP_PSEP");
	}

	return new EasepPSEP(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL* vessel)
{
	EasepPSEP* sv = (EasepPSEP*)vessel;

	if (sv)
		delete sv;
}

DLLCLBK void InitModule(HINSTANCE hModule)
{

}
