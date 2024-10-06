/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2004-2017

ORBITER vessel module: Lunar Module Descent Stage

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

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "Sat5LMDSC.h"
#include "LM_DescentStageResource.h"
#include "Mission.h"

static int refcount = 0;
static MESHHANDLE LM_Descent;
static MESHHANDLE LM_DescentNoLeg;

//
// Spew out particles to simulate the junk thrown out by stage
// seperation explosives.
//

static PARTICLESTREAMSPEC seperation_junk = {
	0,		// flag
	0.05,	// size
	1000,	// rate
	10,    // velocity
	5.0,    // velocity distribution
	100,	// lifetime
	0,	    // growthrate
	0,      // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

Sat5LMDSC::Sat5LMDSC(OBJHANDLE hObj, int fmodel)
	: VESSEL3(hObj, fmodel)

{
	//Mission File
	InitMissionManagementMemory();
	pMission = paGetDefaultMission();

	init();
}

Sat5LMDSC::~Sat5LMDSC()

{
	for (int i = 0; i < 4; i++) {
		if (mgt_Leg[i]) delete mgt_Leg[i];
		if (mgt_Strut[i]) delete mgt_Strut[i];
		if (mgt_Downlock[i]) delete mgt_Downlock[i];
	}

	for (int i = 0; i < 3; i++) {
		if (mgt_Probes1[i]) delete mgt_Probes1[i];
		if (mgt_Probes2[i]) delete mgt_Probes2[i];
	}
}

void Sat5LMDSC::init()

{
	state = 0;
	ro1 = 3;
	ro2 = 1;

	ApolloNo = 0;

	for (int i = 0; i < 4; i++) {
		mgt_Leg[i] = NULL;
		mgt_Strut[i] = NULL;
		mgt_Downlock[i] = NULL;
	}

	for (int i = 0; i < 3; i++) {
		mgt_Probes1[i] = NULL;
		mgt_Probes2[i] = NULL;
	}

	anim_Gear = -1;
	desstg_devmesh = NULL;
	dscidx = -1;
}

void Sat5LMDSC::Setup()

{
	SetSize(5);
	SetEmptyMass(4570.0);
	SetPMI(_V(2.8, 2.29, 2.37));
	SetCrossSections(_V(21, 23, 17));
	SetCW(0.1, 0.3, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 0.7));
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0);
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	//
	// Seperation junk 'thrusters'.
	//

	int i;

	VECTOR3	s_exhaust_pos1 = _V(-0.58, 0.81, 0.58);
	VECTOR3 s_exhaust_pos2 = _V(0.58, 0.81, 0.58);
	VECTOR3	s_exhaust_pos3 = _V(0.58, 0.81, -0.58);
	VECTOR3 s_exhaust_pos4 = _V(-0.58, 0.81, -0.58);

	PROPELLANT_HANDLE ph_sep = CreatePropellantResource(0.2);

	THRUSTER_HANDLE th_sep[4];
	th_sep[0] = CreateThruster(s_exhaust_pos1, _V(1, 0, -1), 1.0, ph_sep, 10.0, 10.0);
	th_sep[1] = CreateThruster(s_exhaust_pos2, _V(-1, 0, -1), 1.0, ph_sep, 10.0, 10.0);
	th_sep[2] = CreateThruster(s_exhaust_pos3, _V(-1, 0, 1), 1.0, ph_sep, 10.0, 10.0);
	th_sep[3] = CreateThruster(s_exhaust_pos4, _V(1, 0, 1), 1.0, ph_sep, 10.0, 10.0);

	for (i = 0; i < 4; i++) {
		AddExhaustStream(th_sep[i], &seperation_junk);
	}
	thg_sep = CreateThrusterGroup(th_sep, 4, THGROUP_USER);

	SetThrusterGroupLevel(thg_sep, 1);

	if (!pMission->LMHasLegs()) {
		InsertMesh(LM_DescentNoLeg, dscidx);
	}

	if (state > 0)
	{
		if (state < 2)
		{
			DefineAnimations(dscidx);
		}
		else
		{
			ro1 = 3;
			ro2 = 4;
			if (state > 2) HideProbes();
		}
		HideDeflectors();
	}

	double tdph = -2.38;
	double Mass = 4570.0;
	TOUCHDOWNVTX td[7];
	double x_target = -0.25;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i < 7; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = 0;
	td[0].pos.y = tdph;
	td[0].pos.z = 1 * ro2;
	td[1].pos.x = -cos(30 * RAD)*ro2;
	td[1].pos.y = tdph;
	td[1].pos.z = -sin(30 * RAD)*ro2;
	td[2].pos.x = cos(30 * RAD)*ro2;
	td[2].pos.y = tdph;
	td[2].pos.z = -sin(30 * RAD)*ro2;
	td[3].pos.x = cos(30 * RAD)*ro1;
	td[3].pos.y = 0;
	td[3].pos.z = sin(30 * RAD)*ro1;
	td[4].pos.x = -cos(30 * RAD)*ro1;
	td[4].pos.y = 0;
	td[4].pos.z = sin(30 * RAD)*ro1;
	td[5].pos.x = 0;
	td[5].pos.y = 0;
	td[5].pos.z = -1 * ro1;
	td[6].pos.x = 0;
	td[6].pos.y = tdph + 3.5;
	td[6].pos.z = 0;

	SetTouchdownPoints(td, 7);
}

void Sat5LMDSC::DefineAnimations(UINT idx) {

	// Landing Gear animations
	ANIMATIONCOMPONENT_HANDLE	ach_GearLeg[4], ach_GearStrut[4], ach_GearLock[4], achGearProbes1[3], achGearProbes2[3];

	const VECTOR3	DES_LEG_AXIS[4] = { { -1, 0, 0 },{ 1, 0, 0 },{ 0, 0,-1 },{ 0, 0, 1 } };
	const VECTOR3	DES_PROBE_AXIS[3] = { { 1, 0, 0 },{ 0, 0,-1 },{ 0, 0, 1 } };
	const VECTOR3	DES_LEG_PIVOT[4] = { { 0.00, 0.55965, 2.95095 },{ 0.00, 0.55965, -2.95095 },{ -2.95095, 0.55965, 0.00 },{ 2.95095, 0.55965, 0.00 } };
	const VECTOR3	DES_STRUT_PIVOT[4] = { { 0.00,-1.27178, 3.83061 },{ 0.00,-1.27178,-3.83061 },{ -3.83061,-1.27178, 0.00 },{ 3.83061,-1.27178, 0.00 } };
	const VECTOR3	DES_LOCK_PIVOT[4] = { { 0.00,-1.02, 2.91 },{ 0.00,-1.02,-2.91 },{ -2.91,-1.02, 0.00 },{ 2.91,-1.02, 0.00 } };
	const VECTOR3	DES_PROBE_PIVOT[3] = { { 0.00, 0.55965, -2.95095 },{ -2.95095, 0.55965, 0.00 },{ 2.95095, 0.55965, 0.00 } };
	const VECTOR3	DES_PROBE2_PIVOT[3] = { { -0.00696, -2.56621, -4.10490 },{ -4.10426, -2.56621, 0.007022 },{ 4.10458,-2.56621, -0.007012 } };

	static UINT meshgroup_Legs[4][3] = {
		{ DS_GRP_Footpad,	DS_GRP_LowerStrut,	DS_GRP_MainStrut },
		{ DS_GRP_FootpadAft,	DS_GRP_LowerStrutAft, DS_GRP_MainStrutAft },
		{ DS_GRP_FootpadLeft,	DS_GRP_LowerStrutLeft,	DS_GRP_MainStrutLeft },
		{ DS_GRP_FootpadRight,	DS_GRP_LowerStrutRight,	DS_GRP_MainStrutRight } };
	static UINT meshgroup_Struts[4] = { DS_GRP_SupportStruts2, DS_GRP_SupportStruts2Aft, DS_GRP_SupportStruts2Left, DS_GRP_SupportStruts2Right };
	static UINT meshgroup_Locks[4] = { DS_GRP_Downlock, DS_GRP_DownlockAft, DS_GRP_DownlockLeft, DS_GRP_DownlockRight };
	static UINT meshgroup_Ladder = DS_GRP_Ladder;
	static UINT meshgroup_Plaque = DS_GRP_Plaque;
	static UINT meshgroup_Probes1[3] = { DS_GRP_Probes1Aft, DS_GRP_Probes1Left, DS_GRP_Probes1Right };
	static UINT meshgroup_Probes2[3] = { DS_GRP_Probes2Aft, DS_GRP_Probes2Left, DS_GRP_Probes2Right };

	anim_Gear = CreateAnimation(1.0);

	for (int i = 0; i < 4; i++)
	{
		mgt_Leg[i] = new MGROUP_ROTATE(idx, &meshgroup_Legs[i][0], 3, DES_LEG_PIVOT[i], DES_LEG_AXIS[i], (float)(45 * RAD));
		mgt_Strut[i] = new MGROUP_ROTATE(idx, &meshgroup_Struts[i], 1, DES_STRUT_PIVOT[i], DES_LEG_AXIS[i], (float)(-63 * RAD));
		mgt_Downlock[i] = new MGROUP_ROTATE(idx, &meshgroup_Locks[i], 1, DES_LOCK_PIVOT[i], DES_LEG_AXIS[i], (float)(155 * RAD));

		ach_GearLeg[i] = AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Leg[i]);
		ach_GearStrut[i] = AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Strut[i], ach_GearLeg[i]);
		ach_GearLock[i] = AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Downlock[i], ach_GearStrut[i]);
	}

	for (int i = 0; i < 3; i++)
	{
		mgt_Probes1[i] = new MGROUP_ROTATE(idx, &meshgroup_Probes1[i], 1, DES_PROBE_PIVOT[i], DES_PROBE_AXIS[i], (float)(45 * RAD));
		mgt_Probes2[i] = new MGROUP_ROTATE(idx, &meshgroup_Probes2[i], 1, DES_PROBE2_PIVOT[i], DES_PROBE_AXIS[i], (float)(171 * RAD));

		achGearProbes1[i] = AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Probes1[i]);
		achGearProbes2[i] = AddAnimationComponent(anim_Gear, 0.0, 1.0, mgt_Probes2[i], achGearProbes1[i]);
	}

	static MGROUP_ROTATE mgt_Ladder(idx, &meshgroup_Ladder, 1, DES_LEG_PIVOT[0], DES_LEG_AXIS[0], (float)(45 * RAD));
	AddAnimationComponent(anim_Gear, 0.0, 1, &mgt_Ladder);

	static MGROUP_ROTATE mgt_Plaque(idx, &meshgroup_Plaque, 1, DES_LEG_PIVOT[0], DES_LEG_AXIS[0], (float)(45 * RAD));
	AddAnimationComponent(anim_Gear, 0.0, 1, &mgt_Plaque);

	SetAnimation(anim_Gear, 0.0);
}

void Sat5LMDSC::clbkVisualCreated(VISHANDLE vis, int refcount)
{
	if (dscidx != -1) {
		desstg_devmesh = GetDevMesh(vis, dscidx);
		HideProbes();
		HideDeflectors();
	}
}

void Sat5LMDSC::clbkVisualDestroyed(VISHANDLE vis, int refcount)
{
	desstg_devmesh = NULL;
}

void Sat5LMDSC::HideProbes() {

	if (!desstg_devmesh)
		return;

	if (state > 2) {
		static UINT meshgroup_Probes[6] = { DS_GRP_Probes1Aft, DS_GRP_Probes1Left, DS_GRP_Probes1Right, DS_GRP_Probes2Aft, DS_GRP_Probes2Left, DS_GRP_Probes2Right };

		GROUPEDITSPEC ges;
		ges.flags = (GRPEDIT_ADDUSERFLAG);
		ges.UsrFlag = 3;

		for (int i = 0; i < 6; i++) {
			oapiEditMeshGroup(desstg_devmesh, meshgroup_Probes[i], &ges);
		}
	}
}

void Sat5LMDSC::HideDeflectors()
{
	if (!desstg_devmesh)
		return;

	if (!pMission->LMHasDeflectors()) {
		static UINT meshgroup_deflectors[2] = { DS_GRP_DeflectorStrut, DS_GRP_RCSdeflector };

		GROUPEDITSPEC ges;
		ges.flags = (GRPEDIT_ADDUSERFLAG);
		ges.UsrFlag = 3;

		for (int i = 0; i < 2; i++) {
			oapiEditMeshGroup(desstg_devmesh, meshgroup_deflectors[i], &ges);
		}
	}
}

void Sat5LMDSC::SetState(int stage, int mission)
{
	state = stage;
	ApolloNo = mission;
	pMission->LoadMission(ApolloNo);
	Setup();
}

void Sat5LMDSC::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "STATE", state);
	oapiWriteScenario_int(scn, "APOLLONO", ApolloNo);
}

void Sat5LMDSC::clbkLoadStateEx(FILEHANDLE scn, void *vstatus)

{
	char *line;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!_strnicmp(line, "STATE", 5))
		{
			sscanf(line + 5, "%d", &state);
		}
		else if (!_strnicmp(line, "APOLLONO", 8)) {
			sscanf(line + 8, "%d", &ApolloNo);

			if (sscanf(line + 8, "%d", &ApolloNo) == 1)
			{
				pMission->LoadMission(ApolloNo);
			}
			else
			{
				char tempBuffer[64];
				strncpy(tempBuffer, line + 9, 63);
				pMission->LoadMission(tempBuffer);
			}
		}
		else
		{
			ParseScenarioLineEx(line, vstatus);
		}
	}

	Setup();
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		LM_Descent = oapiLoadMeshGlobal("ProjectApollo/LM_DescentStage");
		LM_DescentNoLeg = oapiLoadMeshGlobal("ProjectApollo/LM_DescentStageNoLeg");
		seperation_junk.tex = oapiRegisterParticleTexture("ProjectApollo/junk");
	}
	return new Sat5LMDSC(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat5LMDSC*)vessel;
}

void Sat5LMDSC::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL2::clbkSetClassCaps(cfg);
	init();
	dscidx = AddMesh(LM_Descent);
}

