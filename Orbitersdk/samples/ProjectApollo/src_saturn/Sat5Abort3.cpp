/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2018

  ORBITER vessel module: Saturn5Abort3

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
#include "papi.h"
#include "nasspdefs.h"
#include "sat5abort3.h"

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};


static int refcount = 0;
static MESHHANDLE hsat5stg3low;
static MESHHANDLE hsat5stg31low;
static MESHHANDLE hsat5stg32low;
static MESHHANDLE hsat5stg33low;
static MESHHANDLE hsat5stg34low;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;
static MESHHANDLE hsat5tower;
static MESHHANDLE hSM;

const double TCPO=0;
const double SMVO = -0.14;
const double STG0O= 12.4;
const double STG1O= 0;
const double STG2O= 20;

Sat5Abort3::Sat5Abort3 (OBJHANDLE hObj, int fmodel)
    : VESSEL3 (hObj, fmodel)

{
	init();
}

Sat5Abort3::~Sat5Abort3 ()
{
	if (SMJCA)
	{
		delete SMJCA;
		SMJCA = NULL;
	}
	if (SMJCB)
	{
		delete SMJCB;
		SMJCB = NULL;
	}
}

void Sat5Abort3::init()

{
	LowRes = false;
	SMBusAPowered = false;
	SMBusBPowered = false;
	SMJCA = NULL;
	SMJCB = NULL;

	int i;

	ph_rcsa = 0;
	ph_rcsb = 0;
	ph_rcsc = 0;
	ph_rcsd = 0;

	for (i = 0; i < 4; i++)
	{
		th_rcs_a[i] = 0;
		th_rcs_b[i] = 0;
		th_rcs_c[i] = 0;
		th_rcs_d[i] = 0;
	}
}

void Sat5Abort3::Setup()

{
	SetSize (15);
	SetCOG_elev(10);
	SetEmptyMass(21500);
	SetMaxFuelMass(106000);
	SetFuelMass(106000);
	//SetISP (ISP_SECOND_VAC);
	SetISP(4160);
	//slThrust = CalcSeaLevelThrust(THRUST_SECOND_VAC,ISP_SECOND_VAC,ISP_SECOND_SL, 106000);
	//vacThrust = THRUST_SECOND_VAC;
	//vessel->SetMaxThrust (ENGINE_MAIN, slThrust);
	SetMaxThrust(ENGINE_MAIN, 0);
	SetMaxThrust(ENGINE_RETRO, 0);
	SetMaxThrust(ENGINE_HOVER, 0);
	SetMaxThrust(ENGINE_ATTITUDE, 5e3);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI(_V(94, 94, 20));
	SetCrossSections(_V(267, 267, 97));
	SetCW(0.1, 0.3, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//vessel->ShiftCentreOfMass (_V(0,0,12.25));
	VECTOR3 mesh_dir=_V(0,0,2.-STG1O);
	if (LowRes) {
		AddMesh(hsat5stg3low, &mesh_dir);
		mesh_dir = _V(-1.48, -1.48, 14.55 - STG1O);
		AddMesh(hsat5stg31low, &mesh_dir);
		mesh_dir = _V(1.48, -1.48, 14.55 - STG1O);
		AddMesh(hsat5stg32low, &mesh_dir);
		mesh_dir = _V(1.48, 1.48, 14.55 - STG1O);
		AddMesh(hsat5stg33low, &mesh_dir);
		mesh_dir = _V(-1.48, 1.48, 14.55 - STG1O);
		AddMesh(hsat5stg34low, &mesh_dir);
	}
	else
	{
		AddMesh(hsat5stg3, &mesh_dir);
		mesh_dir = _V(-1.48, -1.48, 14.55 - STG1O);
		AddMesh(hsat5stg31, &mesh_dir);
		mesh_dir = _V(1.48, -1.48, 14.55 - STG1O);
		AddMesh(hsat5stg32, &mesh_dir);
		mesh_dir = _V(1.48, 1.48, 14.55 - STG1O);
		AddMesh(hsat5stg33, &mesh_dir);
		mesh_dir = _V(-1.48, 1.48, 14.55 - STG1O);
		AddMesh(hsat5stg34, &mesh_dir);
	}
	mesh_dir = _V(0, SMVO, 19.1 - STG1O);
	AddMesh(hSM, &mesh_dir);

	AddEngines();
}

void Sat5Abort3::clbkPreStep(double simt, double simdt, double mjd)
{
	SMJCA->Timestep(simdt, SMBusAPowered);
	SMJCB->Timestep(simdt, SMBusBPowered);

	if (SMJCA->GetFireMinusXTranslation() || SMJCB->GetFireMinusXTranslation())
	{
		SetThrusterLevel(th_rcs_a[3], 1.0);
		SetThrusterLevel(th_rcs_b[3], 1.0);
		SetThrusterLevel(th_rcs_c[4], 1.0);
		SetThrusterLevel(th_rcs_d[4], 1.0);
	}
	else
	{
		SetThrusterLevel(th_rcs_a[3], 0.0);
		SetThrusterLevel(th_rcs_b[3], 0.0);
		SetThrusterLevel(th_rcs_c[4], 0.0);
		SetThrusterLevel(th_rcs_d[4], 0.0);
	}

	if (SMJCA->GetFirePositiveRoll() || SMJCB->GetFirePositiveRoll())
	{
		SetThrusterLevel(th_rcs_a[1], 1.0);
		SetThrusterLevel(th_rcs_b[1], 1.0);
		SetThrusterLevel(th_rcs_c[1], 1.0);
		SetThrusterLevel(th_rcs_d[1], 1.0);
	}
	else
	{
		SetThrusterLevel(th_rcs_a[1], 0.0);
		SetThrusterLevel(th_rcs_b[1], 0.0);
		SetThrusterLevel(th_rcs_c[1], 0.0);
		SetThrusterLevel(th_rcs_d[1], 0.0);
	}
}

void Sat5Abort3::AddMissionSpecificSystems()
{
	AddSMJC();
}

void Sat5Abort3::AddSMJC()
{
	if (VehicleNo < 507)
	{
		//Old SMJC
		if (SMJCA == NULL)
		{
			SMJCA = new SMJC();
		}
		if (SMJCB == NULL)
		{
			SMJCB = new SMJC();
		}
	}
	else
	{
		//New SMJC
		if (SMJCA == NULL)
		{
			SMJCA = new SMJC_MOD1();
		}
		if (SMJCB == NULL)
		{
			SMJCB = new SMJC_MOD1();
		}
	}
}

void Sat5Abort3::AddEngines()

{
	ClearThrusterDefinitions();

	//
	// Add the RCS. SPS won't fire with SM seperated.
	//

	if (!ph_rcsa)
		ph_rcsa = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsb)
		ph_rcsb = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsc)
		ph_rcsc = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsd)
		ph_rcsd = CreatePropellantResource(RCS_FUEL_PER_QUAD);

	double TRANZ = 1.9 + 19.1 - STG1O;

	int i;
	const double ATTCOOR = 0;
	const double ATTCOOR2 = 2.05;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH = .2;
	const double ATTHEIGHT = .5;
	const double TRANWIDTH = .2;
	const double TRANHEIGHT = 1;
	const double RCSOFFSET = 0.25;
	const double RCSOFFSET2 = -0.25;
	const double RCSOFFSETM = -0.05;
	const double RCSOFFSETM2 = 0.02; // Was 0.05

	//
	// Clear any old thrusters.
	//

	for (i = 0; i < 4; i++)
	{
		th_rcs_a[i] = 0;
		th_rcs_b[i] = 0;
		th_rcs_c[i] = 0;
		th_rcs_d[i] = 0;
	}

	double RCS_ISP = SM_RCS_ISP;
	double RCS_Thrust = SM_RCS_THRUST;

	const double CENTEROFFS = 0.25;

	th_rcs_a[4] = CreateThruster(_V(-CENTEROFFS, ATTCOOR2, TRANZ + RCSOFFSET2), _V(0, -0.1, 1), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_c[3] = CreateThruster(_V(CENTEROFFS, -ATTCOOR2, TRANZ + RCSOFFSET2), _V(0, 0.1, 1), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_d[3] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS, TRANZ + RCSOFFSET2), _V(0.1, 0, 1), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_b[4] = CreateThruster(_V(ATTCOOR2, CENTEROFFS, TRANZ + RCSOFFSET2), _V(-0.1, 0, 1), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_a[3] = CreateThruster(_V(-CENTEROFFS, ATTCOOR2, TRANZ + RCSOFFSET), _V(0, -0.1, -1), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_c[4] = CreateThruster(_V(CENTEROFFS, -ATTCOOR2, TRANZ + RCSOFFSET), _V(0, 0.1, -1), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_d[4] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS, TRANZ + RCSOFFSET), _V(0.1, 0, -1), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_b[3] = CreateThruster(_V(ATTCOOR2, CENTEROFFS, TRANZ + RCSOFFSET), _V(-0.1, 0, -1), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);

	th_rcs_a[1] = CreateThruster(_V(-CENTEROFFS - 0.2, ATTCOOR2, TRANZ + RCSOFFSETM), _V(1, -0.1, 0), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_c[2] = CreateThruster(_V(CENTEROFFS - 0.2, -ATTCOOR2, TRANZ + RCSOFFSETM2), _V(1, 0.1, 0), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);

	th_rcs_a[2] = CreateThruster(_V(-CENTEROFFS + 0.2, ATTCOOR2, TRANZ + RCSOFFSETM2), _V(-1, -0.1, 0), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_c[1] = CreateThruster(_V(CENTEROFFS + 0.2, -ATTCOOR2, TRANZ + RCSOFFSETM), _V(-1, 0.1, 0), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);

	th_rcs_b[2] = CreateThruster(_V(ATTCOOR2, CENTEROFFS - 0.2, TRANZ + RCSOFFSETM2), _V(-0.1, 1, 0), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_d[1] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS - 0.2, TRANZ + RCSOFFSETM), _V(-0.1, 1, 0), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);

	th_rcs_b[1] = CreateThruster(_V(ATTCOOR2, CENTEROFFS + 0.2, TRANZ + RCSOFFSETM), _V(-0.1, -1, 0), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);
	th_rcs_d[2] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS + 0.2, TRANZ + RCSOFFSETM2), _V(0.1, -1, 0), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);

	//
	// We don't create thruster groups here as the user shouldn't be able to control the SM after
	// it seperates.
	//

	SURFHANDLE SMExhaustTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_atrcs");

	for (i = 1; i < 5; i++)
	{
		if (th_rcs_a[i])
			AddExhaust(th_rcs_a[i], 3.0, 0.15, SMExhaustTex);
		if (th_rcs_b[i])
			AddExhaust(th_rcs_b[i], 3.0, 0.15, SMExhaustTex);
		if (th_rcs_c[i])
			AddExhaust(th_rcs_c[i], 3.0, 0.15, SMExhaustTex);
		if (th_rcs_d[i])
			AddExhaust(th_rcs_d[i], 3.0, 0.15, SMExhaustTex);
	}
}

void Sat5Abort3::SetState(bool lowres, int Vehicle, bool SMBusAPower, bool SMBusBPower, SMJCState *sta, SMJCState *stb)
{
	LowRes = lowres;
	VehicleNo = Vehicle;
	SMBusAPowered = SMBusAPower;
	SMBusBPowered = SMBusBPower;

	AddSMJC();
	SMJCA->SetState(*sta);
	SMJCB->SetState(*stb);

	Setup();
}

void Sat5Abort3::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState(scn);

	papiWriteScenario_bool(scn, "LOWRES", LowRes);
	oapiWriteScenario_int(scn, "VECHNO", VehicleNo);
}

void Sat5Abort3::clbkLoadStateEx(FILEHANDLE scn, void *vstatus)

{
	char *line;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!_strnicmp(line, "LOWRES", 6))
		{
			int i;
			sscanf(line + 6, "%d", &i);
			LowRes = (i != 0);
		}
		else if (!strnicmp(line, "VECHNO", 6))
		{
			sscanf(line + 6, "%d", &VehicleNo);
			AddMissionSpecificSystems();
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

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hsat5stg3low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg3");
		hsat5stg31low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg31");
		hsat5stg32low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg32");
		hsat5stg33low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg33");
		hsat5stg34low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg34");
		hsat5stg3 = oapiLoadMeshGlobal("ProjectApollo/sat5stg3");
		hsat5stg31 = oapiLoadMeshGlobal("ProjectApollo/sat5stg31");
		hsat5stg32 = oapiLoadMeshGlobal("ProjectApollo/sat5stg32");
		hsat5stg33 = oapiLoadMeshGlobal("ProjectApollo/sat5stg33");
		hsat5stg34 = oapiLoadMeshGlobal("ProjectApollo/sat5stg34");
		hSM = oapiLoadMeshGlobal("ProjectApollo/SM_uext");

	}
	return new Sat5Abort3 (hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat5Abort3*)vessel;
}

void Sat5Abort3::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL2::clbkSetClassCaps(cfg);
	init();
}

