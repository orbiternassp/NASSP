/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Float Bag vessel

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

 *************************************************************************/

#define ORBITER_MODULE

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"

#include "stdio.h"
#include "math.h"
#include "tracer.h"
#include "FloatBag.h"
#include "papi.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo FloatBag.log";


DLLCLBK void InitModule(HINSTANCE hModule) {

	g_hDLL = hModule;
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new FloatBag(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (FloatBag*)vessel;
}


FloatBag::FloatBag(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	meshhandle = NULL;
	extendBeacon = false;
}

FloatBag::~FloatBag() {
}

void FloatBag::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(10);
	SetSize(120);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0, 0, -1.2);
    UINT meshindex = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\CM-Balloons"), &meshoffset);
	SetMeshVisibilityMode(meshindex, MESHVIS_ALWAYS);

	static UINT balloon1_groups[1] = {0};
	static UINT balloon2_groups[1] = {1};
	static UINT balloon3_groups[1] = {2};
	static UINT beacon_groups[2] = {4, 5};
	static MGROUP_SCALE balloon1(meshindex, balloon1_groups, 1, _V(-0.4,   0.1,  0.67), _V(0.1, 0.1, 0.01)); 
	static MGROUP_SCALE balloon2(meshindex, balloon2_groups, 1, _V( 0.37, -0.05, 0.7 ), _V(0.05, 0.05, 0.01)); 
	static MGROUP_SCALE balloon3(meshindex, balloon3_groups, 1, _V( 0.15, -0.4,  0.75), _V(0.01, 0.01, 0.01)); 
	static MGROUP_ROTATE beacon(meshindex, beacon_groups, 2, _V(-0.34, -0.355, 1.255), _V(0.70710678, -0.70710678, 0), (float)(-145. * RAD));
	anim1 = CreateAnimation(0.0);
	anim2 = CreateAnimation(0.0);
	anim3 = CreateAnimation(0.0);
	animBeacon = CreateAnimation(0.0);
	AddAnimationComponent(anim1, 0, 1, &balloon1);
	AddAnimationComponent(anim2, 0, 1, &balloon2);
	AddAnimationComponent(anim3, 0, 1, &balloon3);
	AddAnimationComponent(animBeacon, 0, 1, &beacon);
	proc1 = 1;
	proc2 = 1;
	proc3 = 1;
	procBeacon = 0;

	static VECTOR3 beaconPos = _V(-0.34, -0.355, 0.495);
	static VECTOR3 beaconCol = _V(1, 1, 1);
	beaconLight.shape = BEACONSHAPE_STAR;
	beaconLight.pos = &beaconPos;
	beaconLight.col = &beaconCol;
	beaconLight.size = 0.1;
	beaconLight.falloff = 0.5;
	beaconLight.period = 0.5; 
	beaconLight.duration = 0.1;
	beaconLight.tofs = 0;
	beaconLight.active = false;
	AddBeacon(&beaconLight);
}

void FloatBag::clbkPostCreation() {

	SetAnimation(anim1, proc1);
	SetAnimation(anim2, proc2);
	SetAnimation(anim3, proc3);
	SetAnimation(animBeacon, procBeacon);
}

void FloatBag::clbkVisualCreated (VISHANDLE vis, int refcount) {

	meshhandle = GetMesh(vis, 0);
}

void FloatBag::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	meshhandle = NULL;
}

void FloatBag::clbkPreStep(double simt, double simdt, double mjd) {
}

void FloatBag::clbkPostStep (double simt, double simdt, double mjd) {

	if (extendBeacon) {
		if (procBeacon < 1) {
			procBeacon += simdt / 5;
			procBeacon  = min(procBeacon, 1);
			SetAnimation(animBeacon, procBeacon);
		} 
	}
}

void FloatBag::SetBagSize(int index, double size) {
		
	if (index == 1) proc1 = 1. - size;
	if (index == 2) proc2 = 1. - size;
	if (index == 3) proc3 = 1. - size;

	SetAnimation(anim1, proc1);
	SetAnimation(anim2, proc2);
	SetAnimation(anim3, proc3);

	if (meshhandle) {
		oapiMeshGroup(meshhandle, 0)->UsrFlag = (proc1 == 1 ? 3 : 1);
		oapiMeshGroup(meshhandle, 1)->UsrFlag = (proc2 == 1 ? 3 : 1);
		oapiMeshGroup(meshhandle, 2)->UsrFlag = (proc3 == 1 ? 3 : 1);	
	}
}

void FloatBag::SetBeaconLight(bool on, bool high) {

	if (!on || procBeacon < 1) {
		beaconLight.active = false;
	} else {
		if (high) {
			beaconLight.period = 0.5; 
			beaconLight.duration = 0.1;
		} else {
			beaconLight.period = 4; 
			beaconLight.duration = 0.2;
		}
		beaconLight.active = true;
	}
}

void FloatBag::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	int i;
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "PROC1", 5)) {
			sscanf (line + 5, "%lf", &proc1);

		} else if (!strnicmp (line, "PROC2", 5)) {
			sscanf (line + 5, "%lf", &proc2);

		} else if (!strnicmp (line, "PROC3", 5)) {
			sscanf (line + 5, "%lf", &proc3);

		} else if (!strnicmp (line, "EXTENDBEACON", 12)) {
			sscanf (line + 12, "%d", &i);
			extendBeacon = (i == 1);
		 
		} else if (!strnicmp (line, "PROCBEACON", 10)) {
			sscanf (line + 10, "%lf", &procBeacon);
			
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
}

void FloatBag::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	papiWriteScenario_double(scn, "PROC1", proc1);
	papiWriteScenario_double(scn, "PROC2", proc2);
	papiWriteScenario_double(scn, "PROC3", proc3);
	oapiWriteScenario_int(scn, "EXTENDBEACON", (extendBeacon ? 1 : 0));
	papiWriteScenario_double(scn, "PROCBEACON", procBeacon);
}

