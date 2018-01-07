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

#include "LEM.h"

#include "iu.h"
#include "s1bsystems.h"
#include "sivbsystems.h"

#include "LEMSaturn.h"

static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1stg1low;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1intstglow;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg2low;
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

LEMSaturn::LEMSaturn(OBJHANDLE hObj, int fmodel) : LEM(hObj, fmodel)
{
	J2Tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_j2");
}

LEMSaturn::~LEMSaturn()
{

}

void LEMSaturn::CalculateStageMass()
{
	SI_Mass = SI_EmptyMass + SI_FuelMass;
	SIVB_Mass = SIVB_EmptyMass + SIVB_FuelMass;
	LM_Mass = LM_EmptyMass + LM_FuelMass;

	Stage2Mass = LM_Mass + SIVB_EmptyMass;
	Stage1Mass = Stage2Mass + SI_EmptyMass + SIVB_FuelMass;
}

void LEMSaturn::ClearThrusters()

{
	ClearThrusterDefinitions();

	//
	// Thruster groups.
	//

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