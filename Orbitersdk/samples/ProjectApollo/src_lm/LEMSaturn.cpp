/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

LEM Saturn

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

#include "Orbitersdk.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"

#include "iu.h"
#include "sivbsystems.h"

#include "LEM.h"
#include "s1b.h"
#include "Saturn1Abort.h"
#include "sivb.h"

#include "s1bsystems.h"

#include "LEMSaturn.h"

static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hNosecap;
static MESHHANDLE hastp;
static MESHHANDLE hastp2;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hlm_1;

static SURFHANDLE exhaust_tex;

PARTICLESTREAMSPEC s1b_exhaust = {
	0,		// flag
	3.2,	// size
	7000,	// rate
	180.0,	// velocity
	0.15,	// velocity distribution
	0.33,	// lifetime
	4.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	2.75,	// size
	2000,	// rate
	60.0,	// velocity
	0.1,	// velocity distribution
	0.4,	// lifetime
	2.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0,
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

// "staging vent" particle streams
static PARTICLESTREAMSPEC stagingvent_spec = {
	0,		// flag
	2.5,	// size
	100,	// rate
	10,	    // velocity
	2,		// velocity distribution
	2,		// lifetime
	2.0,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};

// "fuel venting" particle streams
static PARTICLESTREAMSPEC fuel_venting_spec = {
	0,		// flag
	0.8,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	20,		// lifetime
	0.15,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.6, 0.6,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

// "prelaunch tank venting" particle streams
static PARTICLESTREAMSPEC prelaunchvent_spec = {
	0,		// flag
	0.4,	// size
	200,	// rate
	2,	    // velocity
	0.6,    // velocity distribution
	0.5,	// lifetime
	0.2,	// growthrate
	0.9,    // atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};

static int refcount;

DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		LEMLoadMeshes();
	}

	VESSEL *vessel = new VESSEL(hvessel, flightmodel);
	char *className = new char[strlen(vessel->GetClassNameA()) + 1];
	strcpy(className, vessel->GetClassNameA());
	delete vessel;
	_strlwr(className);
	if (strcmp(className, "projectapollo/lem") == 0) vessel = new LEM(hvessel, flightmodel);
	else if (strcmp(className, "projectapollo/lemsaturn") == 0) vessel = new LEMSaturn(hvessel, flightmodel);
	delete[] className;

	return vessel;
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	//TRACESETUP("ovcExit LMPARKED");

	--refcount;

	if (!refcount) {
		//TRACE("refcount == 0");

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel)
	{
		char *className = new char[strlen(vessel->GetClassName()) + 1];
		strcpy(className, vessel->GetClassName());
		_strlwr(className);
		if (strcmp(className, "projectapollo/lem") == 0) delete (LEM*)vessel;
		else if (strcmp(className, "projectapollo/lemsaturn") == 0) delete (LEMSaturn*)vessel;
		delete[] className;
	}
}

LEMSaturn::LEMSaturn(OBJHANDLE hObj, int fmodel) : LEM(hObj, fmodel),
	SIBSIVBSepPyros("SIB-SIVB-Separation-Pyros", Panelsdk)
{
	refcount = 0;

	int i;

	for (i = 0;i < 8;i++)
	{
		th_1st[i] = 0;
	}

	th_3rd[0] = 0;
	th_3rd_lox = 0;

	for (i = 0;i < 3;i++)
	{
		th_ver[i] = 0;
	}

	for (i = 0;i < 6;i++)
	{
		th_aps_rot[i] = 0;
	}

	thg_1st = 0;
	thg_3rd = 0;
	thg_ver = 0;

	ph_1st = 0;
	ph_3rd = 0;
	ph_ullage3 = 0;
	ph_aps1 = 0;
	ph_aps2 = 0;

	hstg1 = 0;
	habort = 0;
	hs4bM = 0;

	initSaturn1b();
}

LEMSaturn::~LEMSaturn()
{

}

void LEMSaturn::SetStage(int s)
{
	lemsat_stage = s;
}

void LEMSaturn::SeparateStage(UINT new_stage)
{
	if (lemsat_stage == CSM_LEM_STAGE)
	{
		LEM::SeparateStage(new_stage);
		return;
	}

	lemsat_stage = new_stage;

	VESSELSTATUS vs1;
	VESSELSTATUS vs2;

	VECTOR3 ofs1 = _V(0, 0, 0);
	VECTOR3 ofs2 = _V(0, 0, 0);

	VECTOR3 vel1 = _V(0, 0, 0);
	VECTOR3 vel2 = _V(0, 0, 0);

	GetStatus(vs1);
	GetStatus(vs2);

	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;

	if (lemsat_stage == LAUNCH_STAGE_ONE && new_stage == LAUNCH_STAGE_SIVB)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0, 0, -4.0);
	}

	if ((lemsat_stage == PRELAUNCH_STAGE || lemsat_stage == LAUNCH_STAGE_ONE) && new_stage > LAUNCH_STAGE_SIVB)
	{
		ofs1 = _V(0, 0, 4.7);
		vel1 = _V(0, 0, -4.0);
	}

	if ((lemsat_stage == LAUNCH_STAGE_SIVB && new_stage != CM_STAGE) || lemsat_stage == STAGE_ORBIT_SIVB)
	{
		ofs1 = _V(0, 0, 1.7);
		vel1 = _V(0, 0, 0);
	}

	VECTOR3 rofs1, rvel1 = { vs1.rvel.x, vs1.rvel.y, vs1.rvel.z };
	VECTOR3 rofs2, rvel2 = { vs2.rvel.x, vs2.rvel.y, vs2.rvel.z };

	Local2Rel(ofs1, vs1.rpos);
	Local2Rel(ofs2, vs2.rpos);

	GlobalRot(vel1, rofs1);
	GlobalRot(vel2, rofs2);

	vs1.rvel.x = rvel1.x + rofs1.x;
	vs1.rvel.y = rvel1.y + rofs1.y;
	vs1.rvel.z = rvel1.z + rofs1.z;
	vs2.rvel.x = rvel2.x + rofs2.x;
	vs2.rvel.y = rvel2.y + rofs2.y;
	vs2.rvel.z = rvel2.z + rofs2.z;

	if (lemsat_stage == LAUNCH_STAGE_ONE && new_stage == LAUNCH_STAGE_SIVB)
	{
		vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;
		StageS.play();

		//
		// Create S1b stage and set it up.
		//

		if (hstg1) {
			S1B *stage1 = (S1B *)oapiGetVesselInterface(hstg1);
			stage1->DefSetState(&vs1);

			S1BSettings S1Config;

			S1Config.SettingsType.S1B_SETTINGS_ENGINES = 1;
			S1Config.SettingsType.S1B_SETTINGS_FUEL = 1;
			S1Config.SettingsType.S1B_SETTINGS_GENERAL = 1;
			S1Config.SettingsType.S1B_SETTINGS_MASS = 1;

			S1Config.EngineNum = 8;
			S1Config.RetroNum = 4;
			S1Config.EmptyMass = SI_EmptyMass;
			S1Config.MainFuelKg = GetPropellantMass(ph_1st);
			S1Config.MissionTime = MissionTime;
			S1Config.VehicleNo = 204;
			S1Config.ISP_FIRST_SL = ISP_FIRST_SL;
			S1Config.ISP_FIRST_VAC = ISP_FIRST_VAC;
			S1Config.THRUST_FIRST_VAC = THRUST_FIRST_VAC;
			S1Config.CurrentThrust = GetThrusterLevel(th_1st[0]);
			S1Config.LowRes = false;

			stage1->SetState(S1Config);
		}

		SetSecondStage();
		SetSecondStageEngines();
		ShiftCentreOfMass(_V(0, 0, 12.25));
	}

	if ((lemsat_stage == LAUNCH_STAGE_SIVB && new_stage == CSM_LEM_STAGE) || lemsat_stage == STAGE_ORBIT_SIVB)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		CreateSIVBStage("ProjectApollo/nsat1stg2", vs1);

		//SeparationS.play();
		SetLmVesselDockStage();

		ShiftCentreOfMass(_V(0, 0, 20.8));
	}

	if ((lemsat_stage == PRELAUNCH_STAGE || lemsat_stage == LAUNCH_STAGE_ONE) && new_stage >= CSM_LEM_STAGE)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel("Saturn_Abort", "ProjectApollo/Saturn1bAbort1", vs1);

		Sat1Abort1 *stage1 = static_cast<Sat1Abort1 *> (oapiGetVesselInterface(habort));
		stage1->SetState(new_stage == CM_STAGE);

		SetLmVesselDockStage();

		ShiftCentreOfMass(_V(0, 0, 35.15));
	}
}

void LEMSaturn::clbkLoadStateEx(FILEHANDLE scn, void *vs)
{
	GetScenarioState(scn, vs);

	switch (lemsat_stage) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStage();
		SetFirstStageEngines();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStage();
		SetSecondStageEngines();
		AddRCS_S4B();
		break;
	default:
		SetGenericStageState(status);
		break;
	}

	PostLoadSetup();
}

void LEMSaturn::initSaturn1b()
{
	J2Tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_j2");
	SIVBRCSTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");

	lemsat_stage = 0;
	NosecapAttached = true;

	ISP_FIRST_SL = 262 * G;
	ISP_FIRST_VAC = 294 * G;
	THRUST_FIRST_VAC = 1008000;

	ISP_SECOND_SL = 424 * G;
	ISP_SECOND_VAC = 424 * G;

	THRUST_SECOND_VAC = 1009902;

	SIVB_EmptyMass = 12495;
	SIVB_FuelMass = 105795;

	SI_EmptyMass = 41874;
	SI_FuelMass = 411953;

	LM_Mass = 14360;

	TCPO = 0.0;

	CalculateStageMass();
}

void LEMSaturn::CalculateStageMass()
{
	SI_Mass = SI_EmptyMass + SI_FuelMass;
	SIVB_Mass = SIVB_EmptyMass + SIVB_FuelMass;

	Stage2Mass = LM_Mass + SIVB_EmptyMass;
	Stage1Mass = Stage2Mass + SI_EmptyMass + SIVB_FuelMass;
}

void LEMSaturn::ClearThrusters()

{
	ClearThrusterDefinitions();

	//
	// Thruster groups.
	//

	thg_1st = 0;
	thg_3rd = 0;
	thg_ver = 0;
}

void LEMSaturn::SetFirstStage()
{
	SetSize(45);
	SetEmptyMass(Stage1Mass);
	SetPMI(_V(140, 145, 28));
	SetCrossSections(_V(395, 380, 40));
	SetCW(0.1, 0.3, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 1.2));
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0);
	ClearMeshes();

	SetFirstStageMeshes(-14.0);
	SetSecondStageMeshes(13.95);

	//SetView(34.95, false);

	Offset1st = -28.5;
	SetCameraOffset(_V(-1, 1.0, 35.15));

	EnableTransponder(true);

	// **************************** NAV radios *************************************

	InitNavRadios(4);
}

void LEMSaturn::SetFirstStageMeshes(double offset)

{
	double TCP = -54.485 - TCPO;//STG0O;

	double Mass = Stage1Mass + SI_FuelMass;;
	double ro = 30;
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

	//SetTouchdownPoints (_V(0,-1.0,TCP), _V(-.5,.5,TCP), _V(.5,.5,TCP));

	VECTOR3 mesh_dir = _V(0, 0, offset);

	AddMesh(hStage1Mesh, &mesh_dir);
	mesh_dir = _V(0, 0, 16.2 + offset);
	AddMesh(hInterstageMesh, &mesh_dir);
}

void LEMSaturn::SetFirstStageEngines()

{
	ClearThrusters();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	// ************************* propellant specs **********************************
	if (!ph_1st)
		ph_1st = CreatePropellantResource(SI_FuelMass); //1st stage Propellant
	SetDefaultPropellantResource(ph_1st); // display 1st stage propellant level in generic HUD

										  // *********************** thruster definitions ********************************

	int i;

	Offset1st = -80.1;//+STG0O;
	VECTOR3 m_exhaust_ref = { 0,0,-1 };

	VECTOR3 m_exhaust_pos5 = { 0,1.414,Offset1st + 55 };
	VECTOR3 m_exhaust_pos6 = { 1.414,0,Offset1st + 55 };
	VECTOR3 m_exhaust_pos7 = { 0,-1.414,Offset1st + 55 };
	VECTOR3 m_exhaust_pos8 = { -1.414,0,Offset1st + 55 };
	VECTOR3 m_exhaust_pos1 = { 2.12,2.12,Offset1st + 55 };
	VECTOR3 m_exhaust_pos2 = { 2.12,-2.12,Offset1st + 55 };
	VECTOR3 m_exhaust_pos3 = { -2.12,-2.12,Offset1st + 55 };
	VECTOR3 m_exhaust_pos4 = { -2.12,2.12,Offset1st + 55 };

	// orbiter main thrusters
	th_1st[0] = CreateThruster(m_exhaust_pos1, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[1] = CreateThruster(m_exhaust_pos2, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[2] = CreateThruster(m_exhaust_pos3, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[3] = CreateThruster(m_exhaust_pos4, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[4] = CreateThruster(m_exhaust_pos5, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[5] = CreateThruster(m_exhaust_pos6, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[6] = CreateThruster(m_exhaust_pos7, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[7] = CreateThruster(m_exhaust_pos8, _V(0, 0, 1), THRUST_FIRST_VAC, ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);

	SURFHANDLE tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");
	thg_1st = CreateThrusterGroup(th_1st, 8, THGROUP_MAIN);

	EXHAUSTSPEC es_1st[8] = {
		{ th_1st[0], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
	{ th_1st[1], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
	{ th_1st[2], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
	{ th_1st[3], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
	{ th_1st[4], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
	{ th_1st[5], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
	{ th_1st[6], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
	{ th_1st[7], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex }
	};

	for (i = 0; i < 8; i++)
		AddExhaust(es_1st + i);

	srb_exhaust.tex = oapiRegisterParticleTexture("ProjectApollo/Contrail_Saturn2");
	s1b_exhaust.tex = oapiRegisterParticleTexture("ProjectApollo/Contrail_Saturn");

	double exhpos = -4;
	double exhpos2 = -5;
	//AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,-15), &srb_contrail);

	AddExhaustStream(th_1st[0], m_exhaust_pos1 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_1st[1], m_exhaust_pos2 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_1st[2], m_exhaust_pos3 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_1st[3], m_exhaust_pos4 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_1st[4], m_exhaust_pos5 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_1st[5], m_exhaust_pos6 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_1st[6], m_exhaust_pos7 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_1st[7], m_exhaust_pos8 + _V(0, 0, exhpos2), &s1b_exhaust);

	AddExhaustStream(th_1st[0], m_exhaust_pos1 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_1st[1], m_exhaust_pos2 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_1st[2], m_exhaust_pos3 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_1st[3], m_exhaust_pos4 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_1st[4], m_exhaust_pos5 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_1st[5], m_exhaust_pos6 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_1st[6], m_exhaust_pos7 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_1st[7], m_exhaust_pos8 + _V(0, 0, exhpos), &srb_exhaust);
	/**/

	// Contrail
	/*for (i = 0; i < 8; i++) {
	if (contrail[i]) {
	DelExhaustStream(contrail[i]);
	contrail[i] = NULL;
	}
	}

	double conpos = -10;
	contrail[0] = AddParticleStream(&srb_contrail, m_exhaust_pos1+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[1] = AddParticleStream(&srb_contrail, m_exhaust_pos2+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[2] = AddParticleStream(&srb_contrail, m_exhaust_pos3+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[3] = AddParticleStream(&srb_contrail, m_exhaust_pos4+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[4] = AddParticleStream(&srb_contrail, m_exhaust_pos5+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[5] = AddParticleStream(&srb_contrail, m_exhaust_pos6+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[6] = AddParticleStream(&srb_contrail, m_exhaust_pos7+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[7] = AddParticleStream(&srb_contrail, m_exhaust_pos8+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	*/
}

void LEMSaturn::SetSecondStage()
{
	SetSize(22);
	SetCOG_elev(15.225);

	double EmptyMass = Stage2Mass;

	SetEmptyMass(EmptyMass);
	SetPMI(_V(94, 94, 20));
	SetCrossSections(_V(267, 267, 97));
	SetCW(0.1, 0.3, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 1.2));
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0);

	ClearMeshes();
	SetSecondStageMeshes(13.95 - 12.25);
}

void LEMSaturn::SetSecondStageMeshes(double offset)
{
	VECTOR3 mesh_dir = _V(0, 0, offset);
	AddMesh(hStage2Mesh, &mesh_dir);

	mesh_dir = _V(2.45, 0, 10.55 + offset);
	AddMesh(hStageSLA1Mesh, &mesh_dir);
	mesh_dir = _V(0, 2.45, 10.55 + offset);
	AddMesh(hStageSLA2Mesh, &mesh_dir);
	mesh_dir = _V(0, -2.45, 10.55 + offset);
	AddMesh(hStageSLA3Mesh, &mesh_dir);
	mesh_dir = _V(-2.45, 0, 10.55 + offset);
	AddMesh(hStageSLA4Mesh, &mesh_dir);

	nosecapidx = -1;
	meshLM_1 = -1;

	
	//
	// Add nosecap.
	//
	mesh_dir = _V(0, 0, 15.8 + offset);
	nosecapidx = AddMesh(hNosecap, &mesh_dir);
	SetNosecapMesh();

	mesh_dir = _V(0, 0, 9.8 + offset);
	meshLM_1 = AddMesh(hlm_1, &mesh_dir);

	// Dummy docking port so the auto burn feature of IMFD 4.2 is working
	// Remove it when a newer release of IMFD don't need that anymore
	VECTOR3 dockpos = { 0,0,24.8 + offset };
	VECTOR3 dockdir = { 0,0,1 };
	VECTOR3 dockrot = { 0,1,0 };
	SetDockParams(dockpos, dockdir, dockrot);

	SetCameraOffset(_V(-1, 1.0, 31.15 - STG1O));
	//SetView(22.7, false);
}

void LEMSaturn::SetSecondStageEngines()

{
	ClearThrusters();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	//
	// ************************* propellant specs **********************************
	//

	if (ph_1st)
	{
		//
		// Delete remaining S1B stage propellant.
		//

		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}

	if (!ph_3rd)
	{
		//
		// Create SIVB stage Propellant
		//

		ph_3rd = CreatePropellantResource(SIVB_FuelMass);
	}

	if (!ph_ullage3)
	{
		//
		// Create SIVB stage ullage rocket propellant
		//

		ph_ullage3 = CreatePropellantResource(3 * 26.67);
	}

	//
	// display SIVB stage propellant level in generic HUD
	//

	SetDefaultPropellantResource(ph_3rd);

	//
	// *********************** thruster definitions ********************************
	//

	VECTOR3 m_exhaust_pos1 = { 0,0,-9. - STG1O + 10 };

	//
	// orbiter main thrusters
	//

	th_3rd[0] = CreateThruster(m_exhaust_pos1, _V(0, 0, 1), THRUST_SECOND_VAC, ph_3rd, ISP_SECOND_VAC, ISP_SECOND_SL);
	thg_3rd = CreateThrusterGroup(th_3rd, 1, THGROUP_MAIN);

	EXHAUSTSPEC es_3rd[1] = {
		{ th_3rd[0], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, J2Tex }
	};

	AddExhaust(es_3rd);

	//
	// Set the actual stats.
	//

	sivb->RecalculateEngineParameters(THRUST_SECOND_VAC);

	// Thrust "calibrated" for apoapsis after venting is about 167.5 nmi
	// To match the predicted dV of about 25 ft/s (21.7 ft/s actual / 25.6 predicted), use about 320 N thrust, but apoapsis is too high then (> 170 nmi)
	th_3rd_lox = CreateThruster(m_exhaust_pos1, _V(0, 0, 1), 220., ph_3rd, 300., 300.);

	fuel_venting_spec.tex = oapiRegisterParticleTexture("ProjectApollo/Contrail_SaturnVenting");
	AddExhaustStream(th_3rd_lox, &fuel_venting_spec);

	//
	//  Ullage rockets (3)
	//

	VECTOR3	m_exhaust_pos6 = _V(-3.27, -0.46, -2 - STG1O + 9);
	VECTOR3 m_exhaust_pos7 = _V(1.65, 2.86, -2 - STG1O + 9);
	VECTOR3	m_exhaust_pos8 = _V(1.65, -2.86, -2 - STG1O + 9);

	int i;

	//
	// Ullage rocket thrust and ISP is a guess for now.
	//

	th_ver[0] = CreateThruster(m_exhaust_pos6, _V(0.45, 0.0, 1), 15079.47, ph_ullage3, 2188.1);
	th_ver[1] = CreateThruster(m_exhaust_pos7, _V(-0.23, -0.39, 1), 15079.47, ph_ullage3, 2188.1);
	th_ver[2] = CreateThruster(m_exhaust_pos8, _V(-0.23, 0.39, 1), 15079.47, ph_ullage3, 2188.1);

	for (i = 0; i < 3; i++) {
		AddExhaust(th_ver[i], 7.0, 0.2, exhaust_tex);
		AddExhaustStream(th_ver[i], &solid_exhaust);
	}
	thg_ver = CreateThrusterGroup(th_ver, 3, THGROUP_USER);
}

void LEMSaturn::SetNosecapMesh() {

	if (nosecapidx == -1)
		return;

	if (NosecapAttached) {
		SetMeshVisibilityMode(nosecapidx, MESHVIS_EXTERNAL);
	}
	else {
		SetMeshVisibilityMode(nosecapidx, MESHVIS_NEVER);
	}
}

void Saturn1bLoadMeshes()

{
	hSat1stg1 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg1");
	hSat1intstg = oapiLoadMeshGlobal("ProjectApollo/nsat1intstg");
	hSat1stg2 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg2");

	hSat1stg21 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg24");
	hastp = oapiLoadMeshGlobal("ProjectApollo/nASTP3");
	hastp2 = oapiLoadMeshGlobal("ProjectApollo/nASTP2");
	hCOAStarget = oapiLoadMeshGlobal("ProjectApollo/sat_target");
	hNosecap = oapiLoadMeshGlobal("ProjectApollo/nsat1aerocap");
	hlm_1 = oapiLoadMeshGlobal("ProjectApollo/LM_1");

	exhaust_tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");
	solid_exhaust.tex = oapiRegisterParticleTexture("Contrail3");
}

//
// Update per-vessel handles to the appropriate low-res or high-res meshes.
//

void LEMSaturn::SetupMeshes()
{
	hStage1Mesh = hSat1stg1;
	hStage2Mesh = hSat1stg2;
	hInterstageMesh = hSat1intstg;
	hStageSLA1Mesh = hSat1stg21;
	hStageSLA2Mesh = hSat1stg22;
	hStageSLA3Mesh = hSat1stg23;
	hStageSLA4Mesh = hSat1stg24;
}

void LEMSaturn::AddRCS_S4B()

{
	const double ATTCOOR = -10;
	const double ATTCOOR2 = 3.61;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double TRANZ = -3.2 - STG2O;
	const double ATTWIDTH = .2;
	const double ATTHEIGHT = .5;
	const double TRANWIDTH = .2;
	const double TRANHEIGHT = 1;
	const double RCSOFFSET = 0.75;
	const double RCSOFFSETM = 0.30;
	const double RCSOFFSETM2 = 0.47;
	const double RCSX = 0.35;
	VECTOR3 m_exhaust_pos2 = { 0,ATTCOOR2,TRANZ };
	VECTOR3 m_exhaust_pos3 = { 0,-ATTCOOR2,TRANZ };
	VECTOR3 m_exhaust_pos4 = { -ATTCOOR2,0,TRANZ };
	VECTOR3 m_exhaust_pos5 = { ATTCOOR2,0,TRANZ };
	VECTOR3 m_exhaust_ref2 = { 0,0.1,-1 };
	VECTOR3 m_exhaust_ref3 = { 0,-0.1,-1 };
	VECTOR3 m_exhaust_ref4 = { -0.1,0,-1 };
	VECTOR3 m_exhaust_ref5 = { 0.1,0,-1 };

	double offset = 7.7;

	if (!ph_aps1)
	{
		ph_aps1 = CreatePropellantResource(S4B_APS_FUEL_PER_TANK);
	}

	if (!ph_aps2)
	{
		ph_aps2 = CreatePropellantResource(S4B_APS_FUEL_PER_TANK);
	}

	th_aps_rot[0] = CreateThruster(_V(0, ATTCOOR2 + 0.15, TRANZ - 0.25 + offset), _V(0, -1, 0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[1] = CreateThruster(_V(0, -ATTCOOR2 - 0.15, TRANZ - 0.25 + offset), _V(0, 1, 0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);

	AddExhaust(th_aps_rot[0], 0.6, 0.078, SIVBRCSTex);
	AddExhaust(th_aps_rot[1], 0.6, 0.078, SIVBRCSTex);

	th_aps_rot[2] = CreateThruster(_V(RCSX, ATTCOOR2 - 0.2, TRANZ - 0.25 + offset), _V(-1, 0, 0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[3] = CreateThruster(_V(-RCSX, -ATTCOOR2 + 0.2, TRANZ - 0.25 + offset), _V(1, 0, 0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[4] = CreateThruster(_V(-RCSX, ATTCOOR2 - .2, TRANZ - 0.25 + offset), _V(1, 0, 0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[5] = CreateThruster(_V(RCSX, -ATTCOOR2 + .2, TRANZ - 0.25 + offset), _V(-1, 0, 0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);

	AddExhaust(th_aps_rot[2], 0.6, 0.078, SIVBRCSTex);
	AddExhaust(th_aps_rot[3], 0.6, 0.078, SIVBRCSTex);
	AddExhaust(th_aps_rot[4], 0.6, 0.078, SIVBRCSTex);
	AddExhaust(th_aps_rot[5], 0.6, 0.078, SIVBRCSTex);
}

void LEMSaturn::CreateStageOne() {

	// Create hidden SIB vessel
	char VName[256];
	VESSELSTATUS vs;

	GetStatus(vs);
	strcpy(VName, GetName());
	strcat(VName, "-STG1");
	hstg1 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg1", vs);

	// Load only the necessary meshes
	S1B *stage1 = (S1B *)oapiGetVesselInterface(hstg1);
	stage1->LoadMeshes(false);
}

void LEMSaturn::CreateSIVBStage(char *config, VESSELSTATUS &vs1)
{
	char VName[256] = "AS-204";

	strcat(VName, "-S4BSTG");
	hs4bM = oapiCreateVessel(VName, config, vs1);

	SIVBSettings S4Config;

	//
	// For now we'll only seperate the panels on ASTP.
	//

	S4Config.SettingsType.word = 0;
	S4Config.SettingsType.SIVB_SETTINGS_FUEL = 1;
	S4Config.SettingsType.SIVB_SETTINGS_GENERAL = 1;
	S4Config.SettingsType.SIVB_SETTINGS_MASS = 1;
	S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD = 1;
	S4Config.SettingsType.SIVB_SETTINGS_ENGINES = 1;
	S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD_INFO = 1;
	S4Config.VehicleNo = 204;
	S4Config.EmptyMass = SIVB_EmptyMass;
	S4Config.MainFuelKg = GetPropellantMass(ph_3rd);
	S4Config.ApsFuel1Kg = GetPropellantMass(ph_aps1);
	S4Config.ApsFuel2Kg = GetPropellantMass(ph_aps2);
	S4Config.SaturnVStage = false;
	S4Config.MissionTime = MissionTime;
	S4Config.LowRes = false;
	S4Config.ISP_VAC = ISP_SECOND_VAC;
	S4Config.THRUST_VAC = THRUST_SECOND_VAC;
	S4Config.PanelsHinged = false;
	//S4Config.SLARotationLimit = (double)SLARotationLimit;

	//GetPayloadName(S4Config.PayloadName);

	/*S4Config.LMAscentFuelMassKg = LMAscentFuelMassKg;
	S4Config.LMDescentFuelMassKg = LMDescentFuelMassKg;
	S4Config.LMAscentEmptyMassKg = LMAscentEmptyMassKg;
	S4Config.LMDescentEmptyMassKg = LMDescentEmptyMassKg;
	S4Config.LMPad = LMPad;
	S4Config.LMPadCount = LMPadCount;
	S4Config.AEAPad = AEAPad;
	S4Config.AEAPadCount = AEAPadCount;
	sprintf(S4Config.LEMCheck, LEMCheck);*/

	S4Config.iu_pointer = iu;

	SIVB *SIVBVessel = static_cast<SIVB *> (oapiGetVesselInterface(hs4bM));
	SIVBVessel->SetState(S4Config);
}