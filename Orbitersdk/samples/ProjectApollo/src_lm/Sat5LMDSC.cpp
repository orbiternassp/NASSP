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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "stdio.h"
#include "Sat5LMDSC.h"

static int refcount = 0;
static MESHHANDLE LM_Descent;
static MESHHANDLE LM_DescentNoProbes;
static MESHHANDLE LM_DescentGearRet;
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
	init();
}

Sat5LMDSC::~Sat5LMDSC()

{
	// Nothing for now.
}

void Sat5LMDSC::init()

{
	state = 0;
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
	ClearMeshes();
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
	
	if (state == 0) {
		
		double tdph = -2.7;
		double Mass = 4570.0;
		double ro = 1;
		double ro1 = 3;
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
		td[0].pos.z = 1 * ro;
		td[1].pos.x = -cos(30 * RAD)*ro;
		td[1].pos.y = tdph;
		td[1].pos.z = -sin(30 * RAD)*ro;
		td[2].pos.x = cos(30 * RAD)*ro;
		td[2].pos.y = tdph;
		td[2].pos.z = -sin(30 * RAD)*ro;
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

		AddMesh(LM_DescentGearRet);
	}
	
	if (state == 1 || state == 11) {
		
		double tdph = -2.7;
		double Mass = 4570.0;
		double ro = 4;
		double ro1 = 3;
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
		td[0].pos.z = 1 * ro;
		td[1].pos.x = -cos(30 * RAD)*ro;
		td[1].pos.y = tdph;
		td[1].pos.z = -sin(30 * RAD)*ro;
		td[2].pos.x = cos(30 * RAD)*ro;
		td[2].pos.y = tdph;
		td[2].pos.z = -sin(30 * RAD)*ro;
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
		
		if (state == 11) {
			AddMesh(LM_Descent);
		}
		else
		{
			AddMesh(LM_DescentNoProbes);
		}
	}

	if (state == 10) {
		AddMesh(LM_DescentNoLeg);
	}
}

void Sat5LMDSC::SetState(int stage)
{
	state = stage;
	Setup();
}

void Sat5LMDSC::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "STATE", state);
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
		LM_DescentNoProbes = oapiLoadMeshGlobal("ProjectApollo/LM_DescentStageNoProbes");
		LM_DescentGearRet = oapiLoadMeshGlobal("ProjectApollo/LM_DescentStageGearRet");
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
}

