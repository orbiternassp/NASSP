/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Drogue Chute vessel

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
  *	Revision 1.1  2007/07/17 14:33:11  tschachim
  *	Added entry and post landing stuff.
  *	
  **************************************************************************/

#define ORBITER_MODULE

#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "tracer.h"
#include "DrogueChute.h"
#include "papi.h"

#include "CollisionSDK/CollisionSDK.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo DrogueChute.log";

DLLCLBK void InitModule(HINSTANCE hModule) {

	g_hDLL = hModule;
	InitCollisionSDK();
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new DrogueChute(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (DrogueChute*)vessel;
}


DrogueChute::DrogueChute(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	anim = 0;
	proc = 1;
}

DrogueChute::~DrogueChute() {
}

void DrogueChute::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(10);
	SetSize(15);
	SetFuelMass(0);
	ClearAirfoilDefinitions();
	SetPMI(_V(20, 20, 12));
	SetCrossSections(_V(2.8, 2.8, 10.0));
	SetCW(1.0, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 1.2));
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale(-5e-3);
		SetBankMomentScale(-5e-3);
	}
	SetLiftCoeffFunc(0);
	
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0, 0, 11.2);
    meshindex = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_2chute"), &meshoffset);
	SetMeshVisibilityMode(meshindex, MESHVIS_ALWAYS);

	static MGROUP_SCALE chute(meshindex, NULL, 0, _V(0, 0, -10.912),  _V(0.001, 0.001, 0.001)); 
	anim = CreateAnimation(0.0);
	AddAnimationComponent(anim, 0, 1, &chute);

	VSRegVessel(GetHandle());
	VSDisableCollisions(GetHandle());
}

void DrogueChute::clbkPostCreation() {

	SetAnimation(anim, proc);
}

void DrogueChute::clbkPreStep(double simt, double simdt, double mjd) {

	if (proc > 0) {
		proc -= simdt / 3.;
		proc  = max(proc, 0);
		SetAnimation(anim, proc);
	}
	// sprintf(oapiDebugString(), "Dist %f", GetDistanceTo(VAB_LON, VAB_LAT));
}

void DrogueChute::clbkPostStep (double simt, double simdt, double mjd) {
}

void DrogueChute::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "PROC", 4)) {
			sscanf (line + 4, "%lf", &proc);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
}

void DrogueChute::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	papiWriteScenario_double(scn, "PROC", proc);
}

