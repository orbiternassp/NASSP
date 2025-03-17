/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2025

  ORBITER vessel module: Flag

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
#include "Flag.h"
#include "nasspdefs.h"

static 	int refcount = 0;
static MESHHANDLE hFlag;

Flag::Flag(OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)
{
	hMaster = hObj;
}

Flag::~Flag()
{

}

void Flag::clbkSetClassCaps(FILEHANDLE cfg)
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
	AddMesh(hFlag, &mesh_adjust);
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
	tdvtx[0].pos.y = -0.795;
	tdvtx[0].pos.z = -sin(30 * RAD) * 0.1;
	tdvtx[1].pos.x = 0;
	tdvtx[1].pos.y = -0.795;
	tdvtx[1].pos.z = 1.55;
	tdvtx[2].pos.x = -cos(30 * RAD) * 0.1;
	tdvtx[2].pos.y = -0.795;
	tdvtx[2].pos.z = -sin(30 * RAD) * 0.1;

	SetTouchdownPoints(tdvtx, ntdvtx);

	firstTimestep = true;
	liftedOff = false;
	vectorToLM = _V(0, 0, 0);
	distanceToLM = 0;
	doFlagFall = false;
	flagFallProc = 0;
	animFlagFall = 0;

	DefineFallAnimation();
}

void Flag::DoFirstTimestep()
{
	hLM = FindLM();
	GetVectorToLM();
	if (doFlagFall == true) {
		flagFallProc = 1;
		SetAnimation(animFlagFall, flagFallProc);
	}
	firstTimestep = false;
}

void Flag::clbkPreStep(double SimT, double SimDT, double mjd)
{
	if (firstTimestep) {
		DoFirstTimestep();
		return;
	}

	if (!liftedOff) {
		liftedOff = DetectLiftoff();

		if (liftedOff) {
			RotateFlag();
			doFlagFall = CalcFlagFall();
		}
	}

	if ((doFlagFall == 1) && (flagFallProc < 1)) {
		MoveFlagFallAnimation(SimDT);
	}
}

void Flag::clbkPostCreation()
{

}

void Flag::clbkLoadStateEx(FILEHANDLE scn, void* vs)
{
	char* line;
	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "LM_LIFTED_OFF", 13)) {
			int value = 0;
			sscanf(line + 13, "%d", &value);
			if (value != 0) { liftedOff = true; }
		}
		else if (!strnicmp(line, "FLAG_FELL", 9)) {
			int value = 0;
			sscanf(line + 9, "%d", &value);
			if (value != 0) { doFlagFall = true; }
		}
		else {
			ParseScenarioLineEx(line, vs);
		}
	}
}

void Flag::clbkSaveState(FILEHANDLE scn)
{
	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "LM_LIFTED_OFF", (int)liftedOff);

	oapiWriteScenario_int(scn, "FLAG_FELL", (int)doFlagFall);
}

void Flag::clbkVisualCreated(VISHANDLE vis, int refcount)
{

}

void Flag::clbkVisualDestroyed(VISHANDLE vis, int refcount)
{

}

OBJHANDLE Flag::FindLM()
{
	int vessCount = 0;
	OBJHANDLE lmHandle = NULL;
	double minDistance = INFINITY;

	vessCount = oapiGetVesselCount();
	for (int i = 0; i < vessCount; i++)
	{
		if ((oapiGetMass(oapiGetVesselByIndex(i)) > 1000) && (oapiGetMaxFuelMass(oapiGetVesselByIndex(i)) != 0.2)) { //Avoid detecting the descent stage
			VECTOR3 vecDistance = _V(0, 0, 0);
			double distance = 0;
			GetRelativePos(oapiGetVesselByIndex(i), vecDistance);
			distance = length(vecDistance);
			if ((distance < minDistance) && (distance < 250)) {
				minDistance = distance;
				lmHandle = oapiGetVesselByIndex(i);
			}
		}
	}

	return lmHandle;
}

bool Flag::DetectLiftoff()
{
	double lmThrustLvl = 0;
	if (hLM) {
		int thCount = 0;
		VESSEL2* v = (VESSEL2*)oapiGetVesselInterface(hLM);
		thCount = v->GetThrusterCount();
		for (int i = 0; i < thCount; i++) {
			if (v->GetThrusterMax(v->GetThrusterHandleByIndex(i)) == APS_THRUST) {
				lmThrustLvl = v->GetThrusterLevel(v->GetThrusterHandleByIndex(i));
				break;
			}
		}
	}

	if (lmThrustLvl > 0.5) {
		return true;
	}
	else {
		return false;
	}
}

void Flag::GetVectorToLM()
{
	if (hLM) {
		VECTOR3 globalLMPos;
		oapiGetGlobalPos(hLM, &globalLMPos);
		Global2Local(globalLMPos, vectorToLM);
		vectorToLM.y = 0;
		distanceToLM = length(vectorToLM);
	}
}

void Flag::RotateFlag()
{
	double heading = 0;
	oapiGetHeading(hMaster, &heading);
	if (heading >= PI2) heading -= PI2; //range always 0-2pi
	else if (heading < 0) heading += PI2;
	double lmangle = atan2(vectorToLM.x, vectorToLM.z);
	heading += lmangle;
	heading = fmod(heading, PI2);

	heading += PI05;

	VESSELSTATUS vs1;
	GetStatus(vs1);
	vs1.vdata[0].z = heading;
	DefSetState(&vs1);
}

bool Flag::CalcFlagFall()
{
	if (distanceToLM < fallDistance) {
		return true;
	}
	else if (distanceToLM > safeDistance) {
		return false;
	}
	else {
		double dist = distanceToLM - fallDistance;
		dist /= (safeDistance - fallDistance);
		double randomNumber = oapiRand();
		if (randomNumber >= dist) {
			return true;
		}
		else {
			return false;
		}
	}
}

void Flag::DefineFallAnimation()
{
	static UINT flagGroups1[1] = { 0 };
	static UINT flagGroups2[3] = { 1,2,3 };
	static MGROUP_ROTATE flagFallComp1(0, flagGroups1, 1, _V(0, -0.794, 0), _V(0, 0, -1), (float)(89 * RAD));
	static MGROUP_ROTATE flagFallComp2(0, flagGroups2, 3, _V(0, -0.794, 0), _V(0, 1, 0), (float)(90 * RAD));
	ANIMATIONCOMPONENT_HANDLE parent;
	animFlagFall = CreateAnimation(0);
	parent = AddAnimationComponent(animFlagFall, 0, 1, &flagFallComp1);
	AddAnimationComponent(animFlagFall, 0.675, 1, &flagFallComp2, parent);
}

void Flag::MoveFlagFallAnimation(double simdt)
{
	double da = simdt * 1.25;
	if (flagFallProc < 1) { flagFallProc = min(1, flagFallProc + da); }
	SetAnimation(animFlagFall, flagFallProc);
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hFlag = oapiLoadMeshGlobal("ProjectApollo/Sat5flag");
	}

	return new Flag(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL* vessel)
{
	Flag* sv = (Flag*)vessel;

	if (sv)
		delete sv;
}

DLLCLBK void InitModule(HINSTANCE hModule)
{

}
