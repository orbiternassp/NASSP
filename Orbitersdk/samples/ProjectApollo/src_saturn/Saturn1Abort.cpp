/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn1Abort

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
#include "nasspdefs.h"
#include "papi.h"

#include "Saturn1Abort.h"

static int refcount = 0;
static MESHHANDLE hSat1stg1low;
static MESHHANDLE hSat1intstglow;
static MESHHANDLE hSat1stg2low;
static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hSat1stg2cross;
static MESHHANDLE hSM;
static MESHHANDLE hLM;
static MESHHANDLE hLM1;
static MESHHANDLE hastp;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hapollo8lta;
static MESHHANDLE hlta_2r;

static PARTICLESTREAMSPEC seperation_junk = {
	0,		// flag
	0.02,	// size
	1000,	// rate
	0.3,    // velocity
	5.0,    // velocity distribution
	100,	// lifetime
	0,	    // growthrate
	0,      // atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

const double PLOFS = 9.25;

Sat1Abort1::Sat1Abort1(OBJHANDLE hObj, int fmodel)
	: VESSEL3(hObj, fmodel)

{
	init();
}

Sat1Abort1::~Sat1Abort1()

{
	// Nothing for now.
}

void Sat1Abort1::init()

{
	SmPresent = false;
	LowRes = false;
	PayloadType = PAYLOAD_EMPTY;

	thg_sep = 0;

	panelMesh1Saturn1b = -1;
	panelMesh2Saturn1b = -1;
	panelMesh3Saturn1b = -1;
	panelMesh4Saturn1b = -1;

	RotationLimit = 0.25;
	panelProc = 0;
	panelTimestepCount = 0;
	panelAnim = 0;
}

void Sat1Abort1::Setup()
{
	SetSize (25);
	SetEmptyMass (185500.0);
	SetMaxFuelMass (407100);
	SetFuelMass(407100);
	//SetISP (ISP_FIRST_VAC);
	SetISP (2795);
	//slThrust = CalcSeaLevelThrust(THRUST_FIRST_VAC,ISP_FIRST_VAC,ISP_FIRST_SL, 407100);
	//vacThrust = THRUST_FIRST_VAC;
	SetMaxThrust (ENGINE_MAIN, 0);//slThrust
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 3e4);
	SetPMI (_V(140,145,28));
	SetCrossSections (_V(395,380,115));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double TCP = -39.5;
	double Mass = 592600;
	double ro = 1;
	TOUCHDOWNVTX td[4];
	double x_target = -0.05;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = -cos(30 * RAD)*ro;
	td[0].pos.y = -sin(30 * RAD)*ro;
	td[0].pos.z = TCP;
	td[1].pos.x = 0;
	td[1].pos.y = 1 * ro;
	td[1].pos.z = TCP;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = -sin(30 * RAD)*ro;
	td[2].pos.z = TCP;
	td[3].pos.x = 0;
	td[3].pos.y = 0;
	td[3].pos.z = TCP + 60;

	SetTouchdownPoints(td, 4);

	VECTOR3 mesh_dir = _V(0, 0, -18.7);
	if (LowRes) {
		AddMesh(hSat1stg1low, &mesh_dir);
		mesh_dir = _V(0, 0, -2.5);
		AddMesh(hSat1intstglow, &mesh_dir);
		mesh_dir = _V(0, 0, 9.25);
		AddMesh(hSat1stg2low, &mesh_dir);
	}
	else
	{
		mesh_dir = _V(0, 0, -18.7);
		AddMesh(hSat1stg1, &mesh_dir);
		mesh_dir = _V(0, 0, -2.5);
		AddMesh(hSat1intstg, &mesh_dir);
		mesh_dir = _V(0, 0, 9.25);
		AddMesh(hSat1stg2, &mesh_dir);
	}

	if (SmPresent)
	{
		mesh_dir = _V(0, -0.14, 26.6);
		AddMesh(hSM, &mesh_dir);
	}
	else
	{
		switch (PayloadType) {
		case PAYLOAD_LEM:
			mesh_dir = _V(0, 0, 9.8 + PLOFS);
			AddMesh(hLM, &mesh_dir);
			break;

		case PAYLOAD_LM1:
			mesh_dir = _V(0, 0, 9.8 + PLOFS);
			AddMesh(hLM1, &mesh_dir);
			break;

		case PAYLOAD_LTA:
		case PAYLOAD_LTA6:
			mesh_dir = _V(0, 0, 9.6 + PLOFS);
			AddMesh(hlta_2r, &mesh_dir);
			break;

		case PAYLOAD_LTA8:
			mesh_dir = _V(0.0, 0, 8.8 + PLOFS);
			AddMesh(hapollo8lta, &mesh_dir);
			break;

		case PAYLOAD_DOCKING_ADAPTER:
			mesh_dir = _V(0, -0.15, 7.8 + PLOFS);
			AddMesh(hastp, &mesh_dir);
			mesh_dir = _V(-1.04, 1.04, 9.1 + PLOFS);
			AddMesh(hCOAStarget, &mesh_dir);
			break;

		case PAYLOAD_EMPTY:
			break;

		case PAYLOAD_TARGET:
			mesh_dir = _V(-1.3, 0, 9.6 + PLOFS);
			AddMesh(hCOAStarget, &mesh_dir);
			mesh_dir = _V(0, 0, 0 + PLOFS);
			AddMesh(hSat1stg2cross, &mesh_dir);
			break;

		case PAYLOAD_ASTP:
			mesh_dir = _V(0, 0, 8.6 + PLOFS);
			AddMesh(hastp, &mesh_dir);
			break;
		}
	}

	// SLA panel meshes
	mesh_dir = _V(2.45, 0, 19.8);
	panelMesh1Saturn1b = AddMesh(hSat1stg21, &mesh_dir);
	mesh_dir = _V(0, 2.45, 19.8);
	panelMesh2Saturn1b = AddMesh(hSat1stg22, &mesh_dir);
	mesh_dir = _V(0, -2.45, 19.8);
	panelMesh3Saturn1b = AddMesh(hSat1stg23, &mesh_dir);
	mesh_dir = _V(-2.45, 0, 19.8);
	panelMesh4Saturn1b = AddMesh(hSat1stg24, &mesh_dir);

	DefineAnimations();

	VECTOR3 m_exhaust_pos1= {2,0,-23.5};
    VECTOR3 m_exhaust_pos2= {-2,0,-23.5};
	VECTOR3 m_exhaust_pos3= {0,2,-23.5};
	VECTOR3 m_exhaust_pos4= {0,-2,-23.5};
	VECTOR3 m_exhaust_ref = {0,0,-1};
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos1, 30.0, 1.25, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos2, 30.0, 1.25, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos3, 30.0, 1.25, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos4, 30.0, 1.25, &m_exhaust_ref);
	SetCameraOffset (_V(-1,1.0,34.15));
	SetEngineLevel(ENGINE_MAIN,0);

	AddSepJunk();
}

void Sat1Abort1::clbkPreStep(double simt, double simdt, double mjd)

{
	//
	// Open the SLA panels.
	//

	if (panelTimestepCount < 2) {
		panelTimestepCount++;
	}
	else
	{
		if (panelProc < RotationLimit) {
			// Activate separation junk
			if (thg_sep)
				SetThrusterGroupLevel(thg_sep, 1);

			panelProc = min(RotationLimit, panelProc + simdt / 5.0);
			SetAnimation(panelAnim, panelProc);
		}
	}
}

void Sat1Abort1::clbkPostStep(double simt, double simdt, double mjd) {

	// Nothing for now
}

void Sat1Abort1::SetState(bool sm, bool lowres, int payload)
{
	SmPresent = sm;
	LowRes = lowres;
	PayloadType = payload;
	Setup();
}

void Sat1Abort1::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState(scn);

	papiWriteScenario_bool(scn, "SM", SmPresent);
	papiWriteScenario_bool(scn, "LOWRES", LowRes);
	oapiWriteScenario_int(scn, "PAYLOAD", PayloadType);
	oapiWriteScenario_float(scn, "PANELPROC", panelProc);
}

void Sat1Abort1::clbkLoadStateEx(FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!_strnicmp(line, "SM", 2))
		{
			int i;
			sscanf(line + 2, "%d", &i);
			SmPresent = (i != 0);
		}
		else if (!_strnicmp(line, "LOWRES", 6))
		{
			int i;
			sscanf(line + 6, "%d", &i);
			LowRes = (i != 0);
		}
		else if (!_strnicmp(line, "PAYLOAD", 7))
		{
			sscanf(line + 7, "%d", &PayloadType);
		}
		else if (!strnicmp(line, "PANELPROC", 9))
		{
			sscanf(line + 9, "%f", &flt);
			panelProc = flt;
		}
		else
		{
			ParseScenarioLineEx(line, vstatus);
		}
	}

	Setup();
}

void Sat1Abort1::DefineAnimations() {

	panelAnim = CreateAnimation(0.0);

	static MGROUP_ROTATE panel1Saturn1b(panelMesh1Saturn1b, NULL, 0, _V(0.37, 0, -1.2), _V(0, 1, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2Saturn1b(panelMesh2Saturn1b, NULL, 0, _V(0, 0.37, -1.2), _V(-1, 0, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3Saturn1b(panelMesh3Saturn1b, NULL, 0, _V(0, -0.37, -1.2), _V(1, 0, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4Saturn1b(panelMesh4Saturn1b, NULL, 0, _V(-0.37, 0, -1.2), _V(0, -1, 0), (float)(1.0 * PI));

	AddAnimationComponent(panelAnim, 0, 1, &panel1Saturn1b);
	AddAnimationComponent(panelAnim, 0, 1, &panel2Saturn1b);
	AddAnimationComponent(panelAnim, 0, 1, &panel3Saturn1b);
	AddAnimationComponent(panelAnim, 0, 1, &panel4Saturn1b);

	SetAnimation(panelAnim, panelProc);
}

void Sat1Abort1::AddSepJunk() {

	//
	// Seperation junk 'thrusters'.
	//

	int i;
	double junkOffset;
	junkOffset = 16  + PLOFS;

	VECTOR3	s_exhaust_pos1 = _V(1.41, 1.41, junkOffset);
	VECTOR3 s_exhaust_pos2 = _V(1.41, -1.41, junkOffset);
	VECTOR3	s_exhaust_pos3 = _V(-1.41, 1.41, junkOffset);
	VECTOR3 s_exhaust_pos4 = _V(-1.41, -1.41, junkOffset);
	VECTOR3	s_exhaust_pos6 = _V(0, 2.0, junkOffset);
	VECTOR3 s_exhaust_pos7 = _V(0, -2.0, junkOffset);
	VECTOR3	s_exhaust_pos8 = _V(2.0, 0, junkOffset);
	VECTOR3 s_exhaust_pos9 = _V(-2.0, 0, junkOffset);

	PROPELLANT_HANDLE ph_sep = CreatePropellantResource(0.2);

	THRUSTER_HANDLE th_sep[8];
	th_sep[0] = CreateThruster(s_exhaust_pos1, _V(-1, -1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[1] = CreateThruster(s_exhaust_pos2, _V(-1, 1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[2] = CreateThruster(s_exhaust_pos3, _V(1, -1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[3] = CreateThruster(s_exhaust_pos4, _V(1, 1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[4] = CreateThruster(s_exhaust_pos6, _V(0, -1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[5] = CreateThruster(s_exhaust_pos7, _V(0, 1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[6] = CreateThruster(s_exhaust_pos8, _V(-1, 0, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[7] = CreateThruster(s_exhaust_pos9, _V(1, 0, 0), 1.0, ph_sep, 10.0, 10.0);

	for (i = 0; i < 8; i++) {
		AddExhaustStream(th_sep[i], &seperation_junk);
	}
	thg_sep = CreateThrusterGroup(th_sep, 8, THGROUP_USER);
}

// ==============================================================
// API interface
// ==============================================================


DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hSat1stg1low = oapiLoadMeshGlobal("ProjectApollo/LowRes/nsat1stg1");
		hSat1intstglow = oapiLoadMeshGlobal("ProjectApollo/LowRes/nsat1intstg");
		hSat1stg2low = oapiLoadMeshGlobal("ProjectApollo/LowRes/nsat1stg2");
		hSat1stg1 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg1");
		hSat1intstg = oapiLoadMeshGlobal("ProjectApollo/nsat1intstg");
		hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
		hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
		hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
		hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
		hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
		hSat1stg2cross = oapiLoadMeshGlobal("ProjectApollo/nsat1stg2cross");
		hSM = oapiLoadMeshGlobal("ProjectApollo/SM_uext");
		hLM = oapiLoadMeshGlobal("ProjectApollo/LM_SLA");
		hLM1 = oapiLoadMeshGlobal("ProjectApollo/LM_1");
		hapollo8lta = oapiLoadMeshGlobal("ProjectApollo/apollo8_lta");
		hlta_2r = oapiLoadMeshGlobal("ProjectApollo/LTA_2R");
		hastp = oapiLoadMeshGlobal("ProjectApollo/nASTP3");
		hCOAStarget = oapiLoadMeshGlobal("ProjectApollo/sat_target");

		seperation_junk.tex = oapiRegisterParticleTexture("ProjectApollo/junk");
	}
	return new Sat1Abort1(hvessel, flightmodel);

}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat1Abort1*)vessel;
}

void Sat1Abort1::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL2::clbkSetClassCaps(cfg);
	init();
}

