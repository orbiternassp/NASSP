/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5
  This file holds the mesh-related functions.

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
  *	Revision 1.2  2005/02/18 00:40:17  movieman523
  *	Play appropriate seperation sound at SM sep.
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include <stdio.h>
#include <math.h>
#include "Orbitersdk.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "lemcomputer.h"

#include "landervessel.h"
#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"
#include "sat5_lmpkd.h"

PARTICLESTREAMSPEC srb_contrail = {
	0, 12.0, 5, 150.0, 0.3, 4.0, 4, 3.0, PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};
PARTICLESTREAMSPEC srb_exhaust = {
	0, 8.0, 20, 150.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,//	0, 4.0, 20, 150.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};

static MESHHANDLE hCRAWL;
static MESHHANDLE hsat5stg1;
static MESHHANDLE hsat5intstg;
static MESHHANDLE hsat5stg2;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;

static MESHHANDLE hCONE;

static MESHHANDLE hApollocsm;

static MESHHANDLE hLMPKD;

#define LOAD_MESH(var, name) var = oapiLoadMeshGlobal(name);

void LoadSat5Meshes()

{
	LOAD_MESH(hsat5stg1, "sat5stg1");
	LOAD_MESH(hsat5intstg, "sat5intstg");
	LOAD_MESH(hsat5stg2, "sat5stg2");
	LOAD_MESH(hsat5stg3, "sat5stg3");
	LOAD_MESH(hsat5stg31, "sat5stg31");
	LOAD_MESH(hsat5stg32, "sat5stg32");
	LOAD_MESH(hsat5stg33, "sat5stg33");
	LOAD_MESH(hsat5stg34, "sat5stg34");
	LOAD_MESH(hLMPKD, "LM_Parked");
	LOAD_MESH(hCRAWL, "CRAWLER");
}

void SaturnV::BuildFirstStage (int bstate)

{
	TRACESETUP("BuildFirstStage");

	SetSize (59.5);
	SetEmptyMass (Stage1Mass);
	SetMaxFuelMass (SI_FuelMass);
	SetFuelMass(0);
	SetPMI (_V(1147,1147,216.68));
	SetCrossSections (_V(1129,1133,52.4));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	SetSurfaceFrictionCoeff(10e80,10e80);
	ClearMeshes();
	VECTOR3 m_exhaust_pos1= {3,3,Offset1st};
	VECTOR3 mesh_dir=_V(0,0,-80.0+STG0O);
	AddMesh (hCRAWL, &mesh_dir);
	mesh_dir=_V(0,0,-54.0+STG0O);
	AddMesh (hsat5stg1, &mesh_dir);
	if (bstate >=1){
		mesh_dir=_V(0,0,-30.5+STG0O);
		AddMesh (hsat5intstg, &mesh_dir);
	}
	if (bstate >=1){
		mesh_dir=_V(0,0,-17.2+STG0O);
		AddMesh (hsat5stg2, &mesh_dir);
	}
	if (bstate >=2){
		mesh_dir=_V(0,0,2.+STG0O);
		AddMesh (hsat5stg3, &mesh_dir);
	}
	if (bstate >=3){
		mesh_dir=_V(0,0,12+STG0O);
		AddMesh (hLMPKD, &mesh_dir);
	}
	if (bstate >=4){
		mesh_dir=_V(-1.48,-1.48,14.55+STG0O);
		AddMesh (hsat5stg31, &mesh_dir);
	}
	if (bstate >=4){
		mesh_dir=_V(1.48,-1.48,14.55+STG0O);
		AddMesh (hsat5stg32, &mesh_dir);
	}
	if (bstate >=4){
		mesh_dir=_V(1.48,1.48,14.55+STG0O);
		AddMesh (hsat5stg33, &mesh_dir);
	}
	if (bstate >=4){
		mesh_dir=_V(-1.48,1.48,14.55+STG0O);
		AddMesh (hsat5stg34, &mesh_dir);
	}
	if (bstate >=5){
		mesh_dir=_V(0,SMVO,19.1+STG0O);
		AddMesh (hSM, &mesh_dir);
	}
	if (bstate >=6){
		mesh_dir=_V(0,0,23.25+STG0O);
		AddMesh (hCM, &mesh_dir);
	}
	if (bstate >=7){
		mesh_dir=_V(0,0,28.2+STG0O);
		AddMesh (hsat5tower, &mesh_dir);
	}
	Offset1st = -60.1+STG0O;
	SetCameraOffset (_V(-1,1.0,23.1+STG0O));
}

void SaturnV::SetFirstStage ()
{
	int i;
	TRACESETUP("SetFirstStage");

	// *********************** physical parameters *********************************
	DelThrusterGroup(THGROUP_MAIN,true);
    SetSize (59.5);
	SetEmptyMass (Stage1Mass);
	SetPMI (_V(1147,1147,116.60));
	SetCrossSections (_V(1129,1133,52.4));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	SetSurfaceFrictionCoeff(10e90,10e90);
	// ************************* propellant specs **********************************
	if (!ph_1st)
		ph_1st  = CreatePropellantResource(SI_FuelMass); //1st stage Propellant
	SetDefaultPropellantResource (ph_1st); // display 1st stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	ClearThrusterDefinitions();
	Offset1st = -75.1+STG0O;
	VECTOR3 m_exhaust_ref = {0,0,-1};
    VECTOR3 MAIN4a_Vector= {3,3,Offset1st+0.5};
	VECTOR3 MAIN2a_Vector={-3,-3,Offset1st+0.5};
	VECTOR3 MAIN1a_Vector= {-3,3,Offset1st+0.5};
	VECTOR3 MAIN3a_Vector={3,-3,Offset1st+0.5};
	VECTOR3 MAIN5a_Vector={0,0,Offset1st+0.5};

	PROPELLANT_HANDLE	pph;

	if (MissionTime < (-20.0))
		pph = 0;
	else
		pph = ph_1st;

	// orbiter main thrusters

	th_main[0] = CreateThruster (MAIN4a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , pph, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[1] = CreateThruster (MAIN2a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , pph, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[2] = CreateThruster (MAIN1a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , pph, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[3] = CreateThruster (MAIN3a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , pph, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[4] = CreateThruster (MAIN5a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , pph, ISP_FIRST_VAC, ISP_FIRST_SL);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust2");//"Exhaust2"
	thg_main = CreateThrusterGroup (th_main, 5, THGROUP_MAIN);
	for (i = 0; i < 5; i++) AddExhaust (th_main[i], 120.0, 3.5, tex);

	srb_exhaust.tex = oapiRegisterParticleTexture ("Contrail2");

	AddExhaustStream (th_main[0], MAIN4a_Vector+_V(0,0,-30), &srb_contrail);
	AddExhaustStream (th_main[0], MAIN4a_Vector+_V(0,0,-25), &srb_exhaust);
	AddExhaustStream (th_main[1], MAIN2a_Vector+_V(0,0,-30), &srb_contrail);
	AddExhaustStream (th_main[1], MAIN2a_Vector+_V(0,0,-25), &srb_exhaust);
	AddExhaustStream (th_main[2], MAIN1a_Vector+_V(0,0,-30), &srb_contrail);
	AddExhaustStream (th_main[2], MAIN1a_Vector+_V(0,0,-25), &srb_exhaust);
	AddExhaustStream (th_main[3], MAIN3a_Vector+_V(0,0,-30), &srb_contrail);
	AddExhaustStream (th_main[3], MAIN3a_Vector+_V(0,0,-25), &srb_exhaust);
	AddExhaustStream (th_main[4], MAIN5a_Vector+_V(0,0,-30), &srb_contrail);
	AddExhaustStream (th_main[4], MAIN5a_Vector+_V(0,0,-25), &srb_exhaust);

	SetISP(ISP_FIRST_VAC);

	// attitude - this is temporary
	// attitude adjustment during launch phase should really be done via ENGINE gimbaling
	//CreateAttControls_Launch();
	//SetMaxThrust (ENGINE_ATTITUDE, 8e5);
	// ************************ visual parameters **********************************


	ClearMeshes();
	UINT meshidx;
	double TCP=-101.5+STG0O-TCPO;
	SetTouchdownPoints (_V(0,-100.0,TCP), _V(-7,7,TCP), _V(7,7,TCP));
	VECTOR3 mesh_dir=_V(0,0,-54.0+STG0O);
	AddMesh (hsat5stg1, &mesh_dir);
	mesh_dir=_V(0,0,-30.5+STG0O);
	AddMesh (hsat5intstg, &mesh_dir);
	mesh_dir=_V(0,0,-17.2+STG0O);
	AddMesh (hsat5stg2, &mesh_dir);
	mesh_dir=_V(0,0,2.+STG0O);
	AddMesh (hsat5stg3, &mesh_dir);
	if (LEM_DISPLAY && (SIVBPayload == PAYLOAD_LEM)){
		mesh_dir=_V(0,0,12+STG0O);
		AddMesh (hLMPKD, &mesh_dir);
	}
	mesh_dir=_V(-1.48,-1.48,14.55+STG0O);
	AddMesh (hsat5stg31, &mesh_dir);
	mesh_dir=_V(1.48,-1.48,14.55+STG0O);
	AddMesh (hsat5stg32, &mesh_dir);
	mesh_dir=_V(1.48,1.48,14.55+STG0O);
    AddMesh (hsat5stg33, &mesh_dir);
	mesh_dir=_V(-1.48,1.48,14.55+STG0O);
    AddMesh (hsat5stg34, &mesh_dir);
	mesh_dir=_V(0,SMVO,19.1+STG0O);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(0,0,23.25+STG0O);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir=_V(0,0.15,23.1+STG0O);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,23.1+STG0O);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0.02,1.35,23.39+STG0O);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,28.2+STG0O);
	meshidx = AddMesh (hsat5tower, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	SetView(23.1+STG0O);

	buildstatus=8;

	EnableTransponder (true);

	// **************************** NAV radios *************************************

	InitNavRadios (4);
	CMCswitch= true;
	SCswitch= true;
		bAbtlocked =false;

	ThrustAdjust = 1.0;
}

void SaturnV::SetSecondStage ()
{
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	TRACESETUP("SetSecondStage");

	UINT meshidx;
	ClearMeshes();
	DelThrusterGroup(THGROUP_MAIN,true);
    ClearThrusterDefinitions();
	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}
	SetSize (35.5);
	SetCOG_elev (15.225);
	SetEmptyMass (Stage2Mass);
	SetPMI (_V(374,374,60));
	SetCrossSections (_V(524,524,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ShiftCentreOfMass (_V(0,0,STG1O));
	VECTOR3 mesh_dir=_V(0,0,-30.5-STG1O);
	AddMesh (hsat5intstg, &mesh_dir);
	mesh_dir=_V(0,0,-17.2-STG1O);
	AddMesh (hsat5stg2, &mesh_dir);
	mesh_dir=_V(0,0,2.-STG1O);
	AddMesh (hsat5stg3, &mesh_dir);
	if (LEM_DISPLAY && (SIVBPayload == PAYLOAD_LEM)){
		mesh_dir=_V(0,0,12-STG1O);
		AddMesh (hLMPKD, &mesh_dir);
	}
	mesh_dir=_V(-1.48,-1.48,14.55-STG1O);
	AddMesh (hsat5stg31, &mesh_dir);
	mesh_dir=_V(1.48,-1.48,14.55-STG1O);
	AddMesh (hsat5stg32, &mesh_dir);
	mesh_dir=_V(1.48,1.48,14.55-STG1O);
    AddMesh (hsat5stg33, &mesh_dir);
	mesh_dir=_V(-1.48,1.48,14.55-STG1O);
    AddMesh (hsat5stg34, &mesh_dir);
	mesh_dir=_V(0,SMVO,19.1-STG1O);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(0,0,23.25-STG1O);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir=_V(0,0.15,23.1-STG1O);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,23.1-STG1O);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0.02,1.35,23.39-STG1O);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,28.2-STG1O);
	meshidx = AddMesh (hsat5tower, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	SetView(23.1-STG1O);

	// ************************* propellant specs **********************************
	if (!ph_2nd)
		ph_2nd  = CreatePropellantResource(SII_FuelMass); //2nd stage Propellant
	SetDefaultPropellantResource (ph_2nd); // display 2nd stage propellant level in generic HUD
	// attitude - this is temporary
	// attitude adjustment during launch phase should really be done via ENGINE gimbaling
	//SetMaxThrust (ENGINE_ATTITUDE, 2e5);

	// *********************** thruster definitions ********************************
	int i;

	VECTOR3 m_exhaust_pos1= {-1.8,-1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos2= {1.8,1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos3= {-1.8,1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos4 = {1.8,-1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos5 = {0,0,-33.5-STG1O};

	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2,_V( 0,0,1),  THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	thg_main = CreateThrusterGroup (th_main, 5, THGROUP_MAIN);

	for (i = 0; i < 5; i++)
		AddExhaust (th_main[i], 25.0, 1.5, SMMETex);
	SetThrusterGroupLevel(thg_main, 0.0);

	SetISP(ISP_SECOND_VAC);

	if (SII_UllageNum) {
		VECTOR3	m_exhaust_pos6= _V(0,5.07,-33.15-STG1O);
		VECTOR3 m_exhaust_pos7= _V(0,-5.07,-33.15-STG1O);
		VECTOR3	m_exhaust_pos8= _V(5.07,0,-33.15-STG1O);
		VECTOR3 m_exhaust_pos9= _V(-5.07,0,-33.15-STG1O);
		VECTOR3	m_exhaust_pos10= _V(3.55,3.7,-33.15-STG1O);
		VECTOR3 m_exhaust_pos11= _V(3.55,-3.7,-33.15-STG1O);
		VECTOR3	m_exhaust_pos12= _V(-3.55,3.7,-33.15-STG1O);
		VECTOR3 m_exhaust_pos13= _V(-3.55,-3.7,-33.15-STG1O);

		th_ull[0] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), 100000 , ph_2nd, 3000);
		th_ull[1] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), 100000 , ph_2nd, 3000);
		th_ull[2] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), 100000 , ph_2nd, 3000);
		th_ull[3] = CreateThruster (m_exhaust_pos9, _V( 0,0,1), 100000 , ph_2nd, 3000);
		th_ull[4] = CreateThruster (m_exhaust_pos10, _V( 0,0,1),100000 , ph_2nd, 3000);
		th_ull[5] = CreateThruster (m_exhaust_pos11, _V( 0,0,1),100000, ph_2nd, 3000);
		th_ull[6] = CreateThruster (m_exhaust_pos12, _V( 0,0,1),100000, ph_2nd, 3000);
		th_ull[7] = CreateThruster (m_exhaust_pos13, _V( 0,0,1),100000, ph_2nd, 3000);

		for (i = 0; i < SII_UllageNum; i ++)
			AddExhaust (th_ull[i], 5.0, 0.15);

		thg_ull = CreateThrusterGroup (th_ull, SII_UllageNum, THGROUP_USER);
	}

	LAUNCHIND[6]=true;

	ThrustAdjust = 1.0;
	bAbtlocked =false;
}

void SaturnV::SetSecondStage1 ()
{
	UINT meshidx;
	TRACESETUP("SetSecondStage1");

	ClearMeshes();
	DelThrusterGroup(THGROUP_MAIN,true);
	ClearThrusterDefinitions();
	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}
	SetSize (35.5);
	SetCOG_elev (15.225);
	SetEmptyMass (Stage2Mass - Interstage_Mass);
	SetPMI (_V(374,374,60));
	SetCrossSections (_V(524,524,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    VECTOR3 mesh_dir=_V(0,0,-17.2-STG1O);
	AddMesh (hsat5stg2, &mesh_dir);
	mesh_dir=_V(0,0,2.-STG1O);
	AddMesh (hsat5stg3, &mesh_dir);
	if (LEM_DISPLAY && (SIVBPayload == PAYLOAD_LEM)){
		mesh_dir=_V(0,0,12-STG1O);
		AddMesh (hLMPKD, &mesh_dir);
	}
	mesh_dir=_V(-1.48,-1.48,14.55-STG1O);
	AddMesh (hsat5stg31, &mesh_dir);
	mesh_dir=_V(1.48,-1.48,14.55-STG1O);
	AddMesh (hsat5stg32, &mesh_dir);
	mesh_dir=_V(1.48,1.48,14.55-STG1O);
    AddMesh (hsat5stg33, &mesh_dir);
	mesh_dir=_V(-1.48,1.48,14.55-STG1O);
    AddMesh (hsat5stg34, &mesh_dir);
	mesh_dir=_V(0,SMVO,19.1-STG1O);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(0,0,23.25-STG1O);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir=_V(0,0.15,23.1-STG1O);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,23.1-STG1O);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0.02,1.35,23.39-STG1O);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,28.2-STG1O);
	meshidx = AddMesh (hsat5tower, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	SetView(23.1-STG1O);

	// ************************* propellant specs **********************************
	if (!ph_2nd)
		ph_2nd  = CreatePropellantResource(SII_FuelMass); //2nd stage Propellant
	SetDefaultPropellantResource (ph_2nd); // display 2nd stage propellant level in generic HUD
	// attitude - this is temporary
	// attitude adjustment during launch phase should really be done via ENGINE gimbaling
//	SetMaxThrust (ENGINE_ATTITUDE, 2e5);

	// *********************** thruster definitions ********************************
	int i;

	VECTOR3 m_exhaust_pos1= {-1.8,-1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos2= {1.8,1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos3= {-1.8,1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos4 = {1.8,-1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos5 = {0,0,-33.5-STG1O};
	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	thg_main = CreateThrusterGroup (th_main, 5, THGROUP_MAIN);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust_atsme");//"Exhaust2"
	for (i = 0; i < 5; i++) AddExhaust (th_main[i], 25.0, 1.5,tex);

	SetThrusterGroupLevel(thg_main, 1.0);

	SetISP(ISP_SECOND_VAC);

	ThrustAdjust = 1.0;
	bAbtlocked =false;
}

void SaturnV::SetSecondStage2 ()

{
	TRACESETUP("SetSecondStage2");

	ClearMeshes();
	DelThrusterGroup(THGROUP_MAIN,true);
    ClearThrusterDefinitions();
	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}
	SetSize (35.5);
	SetCOG_elev (15.225);
	SetEmptyMass (Stage2Mass - Interstage_Mass - Abort_Mass);
	SetPMI (_V(374,374,60));
	SetCrossSections (_V(524,524,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    VECTOR3 mesh_dir=_V(0,0,-17.2-STG1O);
	AddMesh (hsat5stg2, &mesh_dir);
	mesh_dir=_V(0,0,2.-STG1O);
	AddMesh (hsat5stg3, &mesh_dir);
	if (LEM_DISPLAY && (SIVBPayload == PAYLOAD_LEM)){
		mesh_dir=_V(0,0,12-STG1O);
		AddMesh (hLMPKD, &mesh_dir);
	}
	mesh_dir=_V(-1.48,-1.48,14.55-STG1O);
	AddMesh (hsat5stg31, &mesh_dir);
	mesh_dir=_V(1.48,-1.48,14.55-STG1O);
	AddMesh (hsat5stg32, &mesh_dir);
	mesh_dir=_V(1.48,1.48,14.55-STG1O);
    AddMesh (hsat5stg33, &mesh_dir);
	mesh_dir=_V(-1.48,1.48,14.55-STG1O);
    AddMesh (hsat5stg34, &mesh_dir);
	mesh_dir=_V(0,SMVO,19.1-STG1O);
	AddMesh (hSM, &mesh_dir);

	UINT meshidx;

	mesh_dir=_V(0,0,23.25-STG1O);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir=_V(0,0.15,23.1-STG1O);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,23.1-STG1O);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0.02,1.35,23.39-STG1O);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	SetView(23.1-STG1O);

	mesh_dir=_V(0,0,24.8-STG1O);
	probeidx=AddMesh (hprobe, &mesh_dir);
	// ************************* propellant specs **********************************
	if (!ph_2nd)
		ph_2nd  = CreatePropellantResource(SII_FuelMass); //2nd stage Propellant
	SetDefaultPropellantResource (ph_2nd); // display 2nd stage propellant level in generic HUD

	// *********************** thruster definitions ********************************
	int i;

	VECTOR3 m_exhaust_pos1= {-1.8,-1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos2= {1.8,1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos3= {-1.8,1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos4 = {1.8,-1.8,-33.5-STG1O};
	VECTOR3 m_exhaust_pos5 = {0,0,-33.5-STG1O};

	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	thg_main = CreateThrusterGroup (th_main, 5, THGROUP_MAIN);

	for (i = 0; i < 5; i++)
		AddExhaust (th_main[i], 25.0, 1.5,SMMETex);
	SetThrusterGroupLevel(thg_main, 1.0);

	SetISP(ISP_SECOND_VAC);

	ThrustAdjust = 1.0;
	bAbtlocked =false;
}

void SaturnV::SetThirdStage ()
{
	TRACESETUP("SetThirdStage");

	ClearMeshes();
	DelThrusterGroup(THGROUP_MAIN, true);
    ClearThrusterDefinitions();
	if(ph_2nd)
		DelPropellantResource(ph_2nd);
	SetSize (15.5);
	SetCOG_elev (15.225);
	SetEmptyMass (Stage3Mass);
	SetPMI (_V(53.5,53.5,5));
	SetCrossSections (_V(167,167,47));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ShiftCentreOfMass (_V(0,0,STG2O));
	VECTOR3 mesh_dir=_V(0,0,2.-STG2O);

	AddMesh (hsat5stg3, &mesh_dir);
	if (LEM_DISPLAY && (SIVBPayload == PAYLOAD_LEM)){
		mesh_dir=_V(0,0,12-STG2O);
		AddMesh (hLMPKD, &mesh_dir);
	}
	mesh_dir=_V(-1.48,-1.48,14.55-STG2O);
	AddMesh (hsat5stg31, &mesh_dir);
	mesh_dir=_V(1.48,-1.48,14.55-STG2O);
	AddMesh (hsat5stg32, &mesh_dir);
	mesh_dir=_V(1.48,1.48,14.55-STG2O);
    AddMesh (hsat5stg33, &mesh_dir);
	mesh_dir=_V(-1.48,1.48,14.55-STG2O);
    AddMesh (hsat5stg34, &mesh_dir);
	mesh_dir=_V(0,SMVO,19.1-STG2O);
	AddMesh (hSM, &mesh_dir);
	mesh_dir=_V(0,0,23.25-STG2O);

	UINT meshidx;
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir=_V(0,0.15,23.1-STG2O);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,23.1-STG2O);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0.02,1.35,23.39-STG2O);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,24.8-STG2O);
	probeidx=AddMesh (hprobe, &mesh_dir);

	SetView(23.1-STG2O);

	// ************************* propellant specs **********************************
	if (!ph_3rd)
		ph_3rd  = CreatePropellantResource(S4B_FuelMass); //3rd stage Propellant
	SetDefaultPropellantResource (ph_3rd); // display 3rd stage propellant level in generic HUD

	// *********************** thruster definitions ********************************


	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG2O};
	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_THIRD_VAC, ph_3rd, ISP_THIRD_VAC);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	SetISP(ISP_THIRD_VAC);

	AddExhaust (th_main[0], 25.0, 1.5,SMMETex);

	// attitude - this is temporary
	// attitude adjustment during launch phase should really be done via ENGINE gimbaling

	SetEngineLevel(ENGINE_MAIN, 0.0);

	SetCameraOffset (_V(-1,1.0,32.4-STG2O));

#if 0
	VECTOR3	m_exhaust_pos6= _V(0,3.3,-4-STG2O);
	VECTOR3 m_exhaust_pos7= _V(0,-3.3,-4-STG2O);
#endif

	VECTOR3	m_exhaust_pos6= _V(3.6, -0.425, -3.6-STG2O);
	VECTOR3 m_exhaust_pos7= _V(-3.6, 0.925, -3.6-STG2O);

	th_ver[0] = CreateThruster (m_exhaust_pos6, _V( -0.4,0.0,1), 311 , ph_3rd, 45790.85);
	th_ver[1] = CreateThruster (m_exhaust_pos7, _V( 0.4,0.0,1), 311 , ph_3rd, 45790.85);

	for (int i = 0; i < 2; i++)
		AddExhaust (th_ver[i], 5.0, 0.25);

	thg_ver = CreateThrusterGroup (th_ver, 2, THGROUP_USER);

	SetThrusterGroupLevel(thg_ver,1.0);

	RPswitch3=true;
	RPswitch4=true;
	RPswitch5=true;

	ThrustAdjust = 1.0;
	bAbtlocked = false;
}

void SaturnV::SetPayloadMesh(VESSEL *s4b)

{
	VECTOR3 mesh_dir;

	switch (SIVBPayload) {
	case PAYLOAD_LEM:
		mesh_dir=_V(-0.0,0,-4.7);
		s4b->AddMesh (hLMPKD, &mesh_dir);

		VECTOR3 dockpos = {0,0,-2.2};
		VECTOR3 dockdir = {0,0,1};
		VECTOR3 dockrot = {-0.705,-0.705,0};
		s4b->SetDockParams(dockpos, dockdir, dockrot);

		break;
	}
}

void SaturnV::setupS4B(OBJHANDLE hvessel)

{
	VESSEL *stgSMvessel = oapiGetVesselInterface(hvessel);

	stgSMvessel->SetSize (15.5);
	stgSMvessel->SetEmptyMass (65600);
	stgSMvessel->SetMaxFuelMass (114000);
	stgSMvessel->SetFuelMass(100);
	stgSMvessel->SetISP (5159.5);
	stgSMvessel->SetEnableFocus(false);
	stgSMvessel->SetMaxThrust (ENGINE_MAIN, 1201725);
	stgSMvessel->SetMaxThrust (ENGINE_ATTITUDE, 4700);
	stgSMvessel->SetEngineLevel(ENGINE_MAIN, 0.0);
	stgSMvessel->ClearMeshes();
	VECTOR3 mesh_dir=_V(0,0,-14.5);
	stgSMvessel->AddMesh (hsat5stg3, &mesh_dir);

	if( dockstate <= 2) {
		SetPayloadMesh(stgSMvessel);
	}
	//stgSMvessel->CreateDock(_V(0.0,0.0,-2.2),_V(0.0,0.0,1.0),_V(-0.705, -0.705, 0));
}

void SaturnV::SeparateStage (int stage)

{
	//ResetThrusters(vessel);
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;
	VESSELSTATUS vs3;
	VESSELSTATUS vs4;
	VESSELSTATUS vs5;
	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);
	VECTOR3 ofs3 = _V(0,0,0);
	VECTOR3 ofs4 = _V(0,0,0);
	VECTOR3 ofs5 = _V(0,0,0);
	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);
	VECTOR3 vel3 = _V(0,0,0);
	VECTOR3 vel4 = _V(0,0,0);
	VECTOR3 vel5 = _V(0,0,0);

	GetStatus (vs1);
	GetStatus (vs2);
	GetStatus (vs3);
	GetStatus (vs4);
	GetStatus (vs5);
	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;
	vs3.eng_main = vs3.eng_hovr = 0.0;
	vs4.eng_main = vs4.eng_hovr = 0.0;
	vs5.eng_main = vs5.eng_hovr = 0.0;

	if (stage == LAUNCH_STAGE_ONE && !bAbort)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0,0,-4.0);
	}
	if (stage == LAUNCH_STAGE_ONE && bAbort)
	{
		ofs1= OFS_ABORT;
		vel1 = _V(0,0,-4.0);
	}
	if (stage == LAUNCH_STAGE_TWO && !bAbort)
	{
		ofs1 = OFS_STAGE12;
		vel1 = _V(0,0,-4.0);
	}
	if (stage == LAUNCH_STAGE_TWO_ISTG_JET && !bAbort|| stage == CSM_ABORT_STAGE && !bAbort)
	{
		ofs1 = OFS_TOWER;
		vel1 = _V(15.0,15.0,106.0);
	}

	if (stage == LAUNCH_STAGE_TWO && bAbort || stage == LAUNCH_STAGE_TWO_ISTG_JET && bAbort)
	{

		ofs1= OFS_ABORT2;
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_TWO_TWR_JET)
	{
	 	ofs1 = OFS_STAGE2;
		vel1 = _V(0,0,-6.0);

	}

	if (stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)
	{
	 	ofs1 = OFS_STAGE3;
		vel1 = _V(0,0,-0.135);
		ofs2 = OFS_STAGE31;
		vel2 = _V(-0.5,-0.5,-0.55);
		ofs3 = OFS_STAGE32;
		vel3 = _V(0.5,-0.5,-0.55);
		ofs4 = OFS_STAGE33;
		vel4 = _V(0.5,0.5,-0.55);
		ofs5 = OFS_STAGE34;
		vel5 = _V(-0.5,0.5,-0.55);
	}

	if (stage == CSM_LEM_STAGE)
	{
	 	ofs1 = OFS_SM;
		vel1 = _V(0,0,-0.5);
		ofs2 = OFS_DOCKING;
		vel2 = _V(0.0,0.0,0.6);

	}
	if (stage == CM_STAGE)
	{
		ofs1 = OFS_CM_CONE;
		vel1 = _V(1.0,1.0,1.0);
	}
	if (stage == CSM_ABORT_STAGE)
	{
		ofs1 = OFS_ABORT_TOWER;
		vel1 = _V(15.0,15.0,50.0);
	}

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};
	VECTOR3 rofs3, rvel3 = {vs3.rvel.x, vs3.rvel.y, vs3.rvel.z};
	VECTOR3 rofs4, rvel4 = {vs4.rvel.x, vs4.rvel.y, vs4.rvel.z};
	VECTOR3 rofs5, rvel5 = {vs5.rvel.x, vs5.rvel.y, vs5.rvel.z};
	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);
	Local2Rel (ofs3, vs3.rpos);
	Local2Rel (ofs4, vs4.rpos);
	Local2Rel (ofs5, vs5.rpos);
	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);
	GlobalRot (vel3, rofs3);
	GlobalRot (vel4, rofs4);
	GlobalRot (vel5, rofs5);
	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;
	vs3.rvel.x = rvel3.x+rofs3.x;
	vs3.rvel.y = rvel3.y+rofs3.y;
	vs3.rvel.z = rvel3.z+rofs3.z;
	vs4.rvel.x = rvel4.x+rofs4.x;
	vs4.rvel.y = rvel4.y+rofs4.y;
	vs4.rvel.z = rvel4.z+rofs4.z;
	vs5.rvel.x = rvel5.x+rofs5.x;
	vs5.rvel.y = rvel5.y+rofs5.y;
	vs5.rvel.z = rvel5.z+rofs5.z;
//

	if (stage == CM_ENTRY_STAGE_TWO)
	{
		if (GetAtmPressure()>350000){

		}
		SetChuteStage1 ();
	}

    if (stage == LAUNCH_STAGE_ONE && !bAbort )
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;

		StageS.play(NOLOOP, 255);

		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-STG1");
		hstg1 = oapiCreateVessel(VName,"sat5stg1",vs1);

		SetSecondStage ();
	}

	if (stage == LAUNCH_STAGE_TWO && !bAbort )
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;

		CrashBumpS.play(NOLOOP, 150);

		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-INTSTG");
		hintstg = oapiCreateVessel(VName,"sat5intstg",vs1);
		SetSecondStage1 ();
	}

	if (stage == LAUNCH_STAGE_TWO_ISTG_JET && !bAbort )
	{

		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		TowerJS.play();
		TowerJS.done();

		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-TWR");

		hesc1 = oapiCreateVessel(VName,"sat5btower",vs1);
		SetSecondStage2 ();
	}

	if (stage == LAUNCH_STAGE_TWO_TWR_JET)
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;

		StageS.play();

		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-STG2");
		hstg2 = oapiCreateVessel(VName,"sat5stg2",vs1);
		Ullage2(hstg2,5);
		SetThirdStage ();
	}

	if (stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)
	{
		char VName[256]="";
		char *s4bconfig;

		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = -0.1;
		vs2.vrot.y = 0.1;
		vs2.vrot.z = 0.0;
		vs3.vrot.x = -0.1;
		vs3.vrot.y = -0.1;
		vs3.vrot.z = 0.0;
		vs4.vrot.x = 0.1;
		vs4.vrot.y = -0.1;
		vs4.vrot.z = 0.0;
		vs5.vrot.x = 0.1;
		vs5.vrot.y = 0.1;
		vs5.vrot.z = 0.0;

		//
		// Pick appropriate config file for payload. We need different config
		// files so we can specify the docking ports there: otherwise Orbiter
		// blows up when loading a saved scenario.
		//

		switch (SIVBPayload) {
		case PAYLOAD_LEM:
			s4bconfig = "sat5stg3lem";
			break;

		case PAYLOAD_LTA:
			s4bconfig = "sat5stg3lta";
			break;

		//
		// Shouldn't have ASTP on Saturn V, but what the heck?
		//

		case PAYLOAD_ASTP:
			s4bconfig = "sat5stg3astp";
			break;

		case PAYLOAD_LM1:
			s4bconfig = "sat5stg3lm1";
			break;

		default:
			s4bconfig = "sat5stg3";
			break;
		}

		strcpy (VName, GetName()); strcat (VName, "-S4BSTG");
		hs4bM = oapiCreateVessel(VName, s4bconfig, vs1);
		setupS4B(hs4bM);
		strcpy (VName, GetName()); strcat (VName, "-S4B1");
		hs4b1 = oapiCreateVessel(VName, "sat5stg31", vs2);
		strcpy (VName, GetName()); strcat (VName, "-S4B2");
		hs4b2 = oapiCreateVessel(VName, "sat5stg32", vs3);
		strcpy (VName, GetName()); strcat (VName, "-S4B3");
		hs4b3 = oapiCreateVessel(VName, "sat5stg33", vs4);
		strcpy (VName, GetName()); strcat (VName, "-S4B4");
		hs4b4 = oapiCreateVessel(VName, "sat5stg34", vs5);

		SeparationS.play(NOLOOP,255);

		dockstate = 1;
		FIRSTCSM = true;
		SetCSMStage ();
	}

	if (stage == CSM_LEM_STAGE)
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		//
		// Play appropriate sound for SM seperation.
		//

		if (ApolloExploded && SSMSepExploded.isValid()) {
			SSMSepExploded.play(NOLOOP, 200);
		}
		else {
			SMJetS.play();
		}

		SMJetS.done();
		SSMSepExploded.done();

		if(dockstate != 5){
			VECTOR3 ofs = OFS_DOCKING2;
			VECTOR3 vel = {0.0,0.0,2.5};
			VESSELSTATUS vs4b;
			GetStatus (vs4b);
			StageTransform(this, &vs4b,ofs,vel);
			vs4b.vrot.x = 0.0;
			vs4b.vrot.y = 0.0;
			vs4b.vrot.z = 0.0;
			strcpy (VName, GetName()); strcat (VName, "-DCKPRB");
			hPROBE = oapiCreateVessel(VName, "sat1probe", vs4b);
		}

		strcpy (VName, GetName()); strcat (VName, "-SM");
		hSMJet = oapiCreateVessel(VName, "sat1_SM", vs1);
		stgSM = true;
		SetReentryStage ();
	}

	if (stage == CM_STAGE)
	{
		SetChuteStage2 ();
	}
	if (stage == CM_ENTRY_STAGE_TWO)
	{
		SetChuteStage3 ();
	}
	if (stage == CM_ENTRY_STAGE_FOUR)
	{
		SetChuteStage4 ();
	}
	if (stage == CM_ENTRY_STAGE_FIVE)
	{
		SetSplashStage ();
	}

	if (stage == LAUNCH_STAGE_ONE && bAbort )
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		StageS.play();

		habort = oapiCreateVessel ("Saturn_Abort", "Saturn5Abort1", vs1);
		SetAbortStage ();
	}
	if (stage == LAUNCH_STAGE_TWO && bAbort || stage == LAUNCH_STAGE_TWO_ISTG_JET && bAbort )
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		StageS.play();

		habort = oapiCreateVessel ("Saturn_Abort", "Saturn5Abort2", vs1);
		SetAbortStage ();
	}

	if (stage == CSM_ABORT_STAGE )
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		TowerJS.play(NOLOOP, 160);
		TowerJS.done();

		strcpy (VName, GetName()); strcat (VName, "-TWR");
		hesc1 = oapiCreateVessel (VName, "sat5btower", vs1);
		SetReentryStage ();
		ActivateNavmode(NAVMODE_KILLROT);
	}
}

void SaturnV::DockStage (UINT dockstatus)
{
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;
	VESSELSTATUS vs3;
	VESSELSTATUS vs4;
	VESSELSTATUS vs5;
	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);
	VECTOR3 ofs3 = _V(0,0,0);
	VECTOR3 ofs4 = _V(0,0,0);
	VECTOR3 ofs5 = _V(0,0,0);
	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);
	VECTOR3 vel3 = _V(0,0,0);
	VECTOR3 vel4 = _V(0,0,0);
	VECTOR3 vel5 = _V(0,0,0);
	VECTOR3 RelPos = _V(0.0,0.0,0.0);
	SetEngineLevel(ENGINE_MAIN, 0);
	GetStatus (vs1);
	GetStatus (vs2);
	GetStatus (vs3);
	GetStatus (vs4);
	GetStatus (vs5);
	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;
	vs3.eng_main = vs3.eng_hovr = 0.0;
	vs4.eng_main = vs4.eng_hovr = 0.0;
	vs5.eng_main = vs5.eng_hovr = 0.0;

	if (dockstatus == 2 || dockstatus == 6)
	{
	 	ofs1 = RelPos;
		vel1 = _V(0,0,0.5);
	}

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};
	VECTOR3 rofs3, rvel3 = {vs3.rvel.x, vs3.rvel.y, vs3.rvel.z};
	VECTOR3 rofs4, rvel4 = {vs4.rvel.x, vs4.rvel.y, vs4.rvel.z};
	VECTOR3 rofs5, rvel5 = {vs5.rvel.x, vs5.rvel.y, vs5.rvel.z};
	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);
	Local2Rel (ofs3, vs3.rpos);
	Local2Rel (ofs4, vs4.rpos);
	Local2Rel (ofs5, vs5.rpos);
	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);
	GlobalRot (vel3, rofs3);
	GlobalRot (vel4, rofs4);
	GlobalRot (vel5, rofs5);
	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;
	vs3.rvel.x = rvel3.x+rofs3.x;
	vs3.rvel.y = rvel3.y+rofs3.y;
	vs3.rvel.z = rvel3.z+rofs3.z;
	vs4.rvel.x = rvel4.x+rofs4.x;
	vs4.rvel.y = rvel4.y+rofs4.y;
	vs4.rvel.z = rvel4.z+rofs4.z;
	vs5.rvel.x = rvel5.x+rofs5.x;
	vs5.rvel.y = rvel5.y+rofs5.y;
	vs5.rvel.z = rvel5.z+rofs5.z;

    VECTOR3 mesh_dir = _V(0,0,-14.1);

   switch (dockstatus)	{

   case 1:
		break;

   case 2:
		//Interface initialization for mesh modification to SIVB
		VESSEL *targetvessel;
		sat5_lmpkd *lmvessel;
		VESSELSTATUS2 vslm2, *pv;
		VESSELSTATUS2::DOCKINFOSPEC dckinfo;
		char VNameLM[256];

		Undock(0);
		targetvessel = oapiGetVesselInterface(hs4bM);
		//time to free LM from SIVB
		targetvessel->ClearMeshes();

		targetvessel->AddMesh (hsat5stg3, &mesh_dir);
//		targetvessel->ShiftCentreOfMass(_V(0,0,-14.1));

		//Release of Slaved Stage
		//Time to hear the Stage separation
		SMJetS.play(NOLOOP,230);

		//Now Lets create a real LEM and dock it
		//oapiDeleteVessel(hs4bM,GetHandle());

		strcpy (VNameLM, GetName());
		strcat (VNameLM, "-LM");

		vslm2.version = 2;
		vslm2.flag = 0;
		vslm2.fuel = 0;
		vslm2.thruster = 0;
		vslm2.ndockinfo = 1;
		vslm2.dockinfo = &dckinfo;

		GetStatusEx(&vslm2);

		vslm2.dockinfo[0].idx = 0;
		vslm2.dockinfo[0].ridx = 0;
		vslm2.dockinfo[0].rvessel = GetHandle();
  		vslm2.ndockinfo = 1;
		vslm2.flag = VS_DOCKINFOLIST;
		vslm2.version = 2;
		pv = &vslm2;
		hLMV = oapiCreateVesselEx(VNameLM, "sat5_LMPKD", pv);

		//
		// Initialise the state of the LEM AGC information.
		//

		LemSettings ls;

		ls.AutoSlow = AutoSlow;
		ls.Crewed = Crewed;
		ls.LandingAltitude = LMLandingAltitude;
		ls.LandingLatitude = LMLandingLatitude;
		ls.LandingLongitude = LMLandingLongitude;
		strncpy (ls.language, AudioLanguage, 63);
		ls.MissionNo = ApolloNo;
		ls.MissionTime = MissionTime;

		lmvessel = (sat5_lmpkd *) oapiGetVesselInterface(hLMV);
		lmvessel->SetLanderData(ls);
		LEMdatatransfer = true;

		GetStatusEx(&vslm2);

		vslm2.dockinfo = &dckinfo;
		vslm2.dockinfo[0].idx = 0;
		vslm2.dockinfo[0].ridx = 0;
		vslm2.dockinfo[0].rvessel = hLMV;
  		vslm2.ndockinfo = 1;
		vslm2.flag = VS_DOCKINFOLIST;
		vslm2.version = 2;

		DefSetStateEx(&vslm2);

		bManualUnDock = false;
		dockstate = 3;
		SetAttitudeLinLevel(2,-1);
		break;

   case 3:
	if(bManualUnDock) {
		//DM Jetison preparation
		SetAttitudeLinLevel(2,-1);

		//Time to hear the Stage separation
		SMJetS.play(NOLOOP);
		bManualUnDock= false;
		if (ProbeJetison){
			if (stage == CSM_LEM_STAGE){
				SetCSM2Stage ();
			}
			StageS.play(NOLOOP);
			bManualUnDock= false;
			dockstate=5;
		}
		else{
			if (stage == CSM_LEM_STAGE){
				SetCSMStage ();
			}
			SMJetS.play(NOLOOP);
			bManualUnDock= false;
			ProbeJetison=true;
			dockstate=4;
		}
	}
	break;
   case 4:

	   break;
   case 5:

	   break;

	}

}

void SaturnV::Ullage(OBJHANDLE hvessel, double gaz)

{
	TRACESETUP("Ullage");

	//
	// Just in case someone removes all the retros, do nothing.
	//

	if (!SI_RetroNum)
		return;

	VESSEL *stg1vessel = oapiGetVesselInterface(hvessel);

	VECTOR3 m_exhaust_pos2= {-3.9,-3.9, -16};
	VECTOR3 m_exhaust_pos3= {3.9, 3.9, -16};
	VECTOR3 m_exhaust_pos4= {-3.9, 3.9, -16};
	VECTOR3 m_exhaust_pos5= {3.9, -3.9, -16};

	ph_retro1 = stg1vessel->CreatePropellantResource((650 * SI_RetroNum) / 8.0);

	double thrust = (788000 * SI_RetroNum) / 8.0;

	if (!th_retro1[0]) {
		th_retro1[0] = stg1vessel->CreateThruster (m_exhaust_pos2, _V(.1,.1, -0.9), thrust, ph_retro1, 4000);
		th_retro1[1] = stg1vessel->CreateThruster (m_exhaust_pos3, _V(-.1,-.1, -0.9), thrust, ph_retro1, 4000);
		th_retro1[2] = stg1vessel->CreateThruster (m_exhaust_pos4, _V(.1,-.1, -0.9), thrust, ph_retro1, 4000);
		th_retro1[3] = stg1vessel->CreateThruster (m_exhaust_pos5, _V(-.1,.1, -0.9), thrust, ph_retro1, 4000);
	}

	thg_retro1 = stg1vessel->CreateThrusterGroup(th_retro1, 4, THGROUP_RETRO);

	for (int i = 0; i < 4; i++)
		stg1vessel->AddExhaust (th_retro1[i], 8.0, 0.2);

	stg1vessel->SetThrusterGroupLevel(thg_retro1, 1.0);
}

//
// This now seems to be used for the SIVB ullage thrusters.
//

void SaturnV::Ullage2(OBJHANDLE hvessel,double gaz)

{
	TRACESETUP("Ullage2");
	VESSEL *stg2vessel = oapiGetVesselInterface(hvessel);

	VECTOR3 m_exhaust_pos2= {0,-4,12.2};
	VECTOR3 m_exhaust_pos3= {0,4,12.2};
	VECTOR3 m_exhaust_ref2 = {0,-0.35,1};
	VECTOR3 m_exhaust_ref3 = {0,0.35,1};
	VECTOR3 m_exhaust_pos4= {-4,0,12.2};
	VECTOR3 m_exhaust_pos5= {4,0,12.2};
	VECTOR3 m_exhaust_ref4 = {-0.35,0,1};
	VECTOR3 m_exhaust_ref5 = {0.35,0,1};

	if (!UllageID7){
		UllageID6=stg2vessel ->AddExhaustRef (EXHAUST_CUSTOM, m_exhaust_pos2, 5.0, 0.15, &m_exhaust_ref2);
		UllageID7=stg2vessel ->AddExhaustRef (EXHAUST_CUSTOM, m_exhaust_pos3, 5.0, 0.15, &m_exhaust_ref3);
		UllageID8=stg2vessel ->AddExhaustRef (EXHAUST_CUSTOM, m_exhaust_pos4, 5.0, 0.15, &m_exhaust_ref4);
		UllageID9=stg2vessel ->AddExhaustRef (EXHAUST_CUSTOM, m_exhaust_pos5, 5.0, 0.15, &m_exhaust_ref5);
	}
	stg2vessel ->SetExhaustScales (EXHAUST_CUSTOM, UllageID6, gaz, 0.15);
	stg2vessel ->SetExhaustScales (EXHAUST_CUSTOM, UllageID7, gaz, 0.15);
	stg2vessel ->SetExhaustScales (EXHAUST_CUSTOM, UllageID8, gaz, 0.15);
	stg2vessel ->SetExhaustScales (EXHAUST_CUSTOM, UllageID9, gaz, 0.15);

	//sprintf(oapiDebugString(), "Reentry %f", gaz);
	if (gaz  > 0){
		stg2vessel->SetAttitudeLinLevel(2,-1);
	}else{
		stg2vessel->SetAttitudeLinLevel(2,0);
	}
}
