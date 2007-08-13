/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Main Chute vessel

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
  *	Revision 1.1  2007/07/17 14:33:14  tschachim
  *	Added entry and post landing stuff.
  *	
  **************************************************************************/

#define ORBITER_MODULE

#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "tracer.h"
#include "MainChute.h"
#include "papi.h"

#include "CollisionSDK/CollisionSDK.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo MainChute.log";

// States
#define STATE_CHUTE1			1
#define STATE_CHUTE2			2
#define STATE_CHUTE3			3
#define STATE_DEPLOYED			4
#define STATE_LANDED			5


DLLCLBK void InitModule(HINSTANCE hModule) {

	g_hDLL = hModule;
	InitCollisionSDK();
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new MainChute(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (MainChute*)vessel;
}


MainChute::MainChute(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	state = STATE_CHUTE1;
	anim1 = 0;
	anim2 = 0;
	anim3 = 0;
	animLanding = 0;
	proc1 = 1;
	proc2 = 1;
	proc3 = 1;
	procLanding = 0;
}

MainChute::~MainChute() {
}

void MainChute::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(10);
	SetSize(15);
	SetFuelMass(0);
	ClearAirfoilDefinitions();
	SetPMI(_V(20, 20, 12));
	SetCrossSections(_V(2.8, 2.8, 15.0));
	SetCW(1.0, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 1.2));
	SetSurfaceFrictionCoeff(1, 1);
	SetTouchdownPoints(_V(0, 10, -2.5), _V(10, -10, -2.5), _V(-10, -10, -2.5));
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale(-5e-3);
		SetBankMomentScale(-5e-3);
	}
	SetLiftCoeffFunc(0);
	
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset1 = _V(0, -0.25, 3.9);
    meshindex1 = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_3chuteEX"), &meshoffset1);
	SetMeshVisibilityMode(meshindex1, MESHVIS_NEVER);

	VECTOR3 meshoffset2 = _V(0, -1.38, 23.3);
    meshindex2 = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_3chuteHD"), &meshoffset2);
	SetMeshVisibilityMode(meshindex2, MESHVIS_NEVER);

	VECTOR3 meshoffset3 = _V(0, -1.9, 15.95);
    meshindex3 = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Apollo_3chute"), &meshoffset3);
	SetMeshVisibilityMode(meshindex3, MESHVIS_NEVER);

	static MGROUP_SCALE chute1(meshindex1, NULL, 0, _V(0, 0.25, -4.387),  _V(0.2, 0.2, 0.001)); 
	anim1 = CreateAnimation(0.0);
	AddAnimationComponent(anim1, 0, 1, &chute1);

	static MGROUP_SCALE chute2(meshindex2, NULL, 0, _V(0, 1.38, -23.026),  _V(0.185, 0.185, 0.185)); 
	anim2 = CreateAnimation(0.0);
	AddAnimationComponent(anim2, 0, 1, &chute2);

	static MGROUP_SCALE chute3(meshindex3, NULL, 0, _V(0, 1.908, -15.673),  _V(0.65, 0.65, 1.2)); 
	anim3 = CreateAnimation(0.0);
	AddAnimationComponent(anim3, 0, 1, &chute3);

	static UINT landing_groups1[3] = {4, 5, 6};
	static UINT landing_groups2[3] = {1, 2, 3};
	static UINT landing_groups3[3] = {7, 8, 9};
	static UINT landing_groups4[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	static MGROUP_ROTATE landing1(meshindex3, landing_groups1, 3, _V(0, 1.9, -12.5), _V( 1, 0, 0), (float)(0.38 * PI));
	static MGROUP_ROTATE landing2(meshindex3, landing_groups2, 3, _V(0, 1.9, -12.5), _V( 0.5, 0.8660254, 0), (float)(-0.38 * PI));
	static MGROUP_ROTATE landing3(meshindex3, landing_groups3, 3, _V(0, 1.9, -12.5), _V( -0.5, 0.8660254, 0), (float)(0.38 * PI));
	static MGROUP_SCALE landing4(meshindex3, landing_groups4, 10, _V(0, 1.9, -15.7),  _V(1.0, 1.0, 0.1)); 
	static MGROUP_ROTATE landing5(meshindex3, landing_groups1, 3, _V(0, 1.9, -15.5), _V( 1, 0, 0), (float)(0.03 * PI));
	static MGROUP_ROTATE landing6(meshindex3, landing_groups2, 3, _V(0, 1.9, -15.5), _V( 0.5, 0.8660254, 0), (float)(-0.03 * PI));
	static MGROUP_ROTATE landing7(meshindex3, landing_groups3, 3, _V(0, 1.9, -15.5), _V( -0.5, 0.8660254, 0), (float)(0.03 * PI));
	animLanding = CreateAnimation(0.0);
	AddAnimationComponent(animLanding, 0,    0.45, &landing1);
	AddAnimationComponent(animLanding, 0,    0.45, &landing2);
	AddAnimationComponent(animLanding, 0,    0.45, &landing3);
	AddAnimationComponent(animLanding, 0.45, 0.9,  &landing4);
	AddAnimationComponent(animLanding, 0.9,  1,    &landing5);
	AddAnimationComponent(animLanding, 0.9,  1,    &landing6);
	AddAnimationComponent(animLanding, 0.9,  1,    &landing7);

	VSRegVessel(GetHandle());
	VSDisableCollisions(GetHandle());
}

void MainChute::clbkPostCreation() {

	SetAnimation(anim1, proc1);
	SetAnimation(anim2, proc2);
	SetAnimation(anim3, proc3);
	SetAnimation(animLanding, procLanding);
}

void MainChute::clbkPreStep(double simt, double simdt, double mjd) {

	switch (state) {

	case STATE_CHUTE1:
		if (proc1 > 0) {
			proc1 -= simdt / 1;
			proc1  = max(proc1, 0);
			SetAnimation(anim1, proc1);
		} else {
			state = STATE_CHUTE2;
		}
		SetMeshVisibilityMode(meshindex1, MESHVIS_ALWAYS);
		SetMeshVisibilityMode(meshindex2, MESHVIS_NEVER);
		SetMeshVisibilityMode(meshindex3, MESHVIS_NEVER);
		break;

	case STATE_CHUTE2:
		if (proc2 > 0) {
			proc2 -= simdt / 6;
			proc2  = max(proc2, 0);
			SetAnimation(anim2, proc2);
		} else {
			state = STATE_CHUTE3;
		}
		SetMeshVisibilityMode(meshindex1, MESHVIS_NEVER);
		SetMeshVisibilityMode(meshindex2, MESHVIS_ALWAYS);
		SetMeshVisibilityMode(meshindex3, MESHVIS_NEVER);
		break;

	case STATE_CHUTE3:
		if (proc3 > 0) {
			proc3 -= simdt / 5;
			proc3  = max(proc3, 0);
			SetAnimation(anim3, proc3);
		} else {
			state = STATE_DEPLOYED;
		}
		SetMeshVisibilityMode(meshindex1, MESHVIS_NEVER);
		SetMeshVisibilityMode(meshindex2, MESHVIS_NEVER);
		SetMeshVisibilityMode(meshindex3, MESHVIS_ALWAYS);
		break;

	case STATE_DEPLOYED:
		if (GetAltitude() < 2.3) {
			if (procLanding < 1) {
				procLanding += simdt / 20;
				procLanding  = min(procLanding, 1);
				SetAnimation(animLanding, procLanding);
			} else {
				state = STATE_LANDED;
			}
		}
		SetMeshVisibilityMode(meshindex1, MESHVIS_NEVER);
		SetMeshVisibilityMode(meshindex2, MESHVIS_NEVER);
		SetMeshVisibilityMode(meshindex3, MESHVIS_ALWAYS);
		//sprintf(oapiDebugString(), "### Altitude %.1f", GetAltitude());
		break;

	}
}

void MainChute::clbkPostStep (double simt, double simdt, double mjd) {
}

void MainChute::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%d", &state);
		
		} else if (!strnicmp (line, "PROC1", 5)) {
			sscanf (line + 5, "%lf", &proc1);

		} else if (!strnicmp (line, "PROC2", 5)) {
			sscanf (line + 5, "%lf", &proc2);

		} else if (!strnicmp (line, "PROC3", 5)) {
			sscanf (line + 5, "%lf", &proc3);

		} else if (!strnicmp (line, "PROCLANDING", 11)) {
			sscanf (line + 11, "%lf", &procLanding);
			
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
}

void MainChute::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "STATE", state);
	papiWriteScenario_double(scn, "PROC1", proc1);
	papiWriteScenario_double(scn, "PROC2", proc2);
	papiWriteScenario_double(scn, "PROC3", proc3);
	papiWriteScenario_double(scn, "PROCLANDING", procLanding);
}

