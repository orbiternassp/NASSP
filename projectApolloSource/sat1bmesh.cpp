/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn 1b mesh code

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
  *	Revision 1.3  2005/03/06 03:23:26  chode99
  *	Relocated and redirected the SIVB ullage thrusters to coincide with the mesh.
  *	
  *	Revision 1.2  2005/02/19 19:45:37  chode99
  *	Moved 1 line of code (VECTOR3 mesh_dir=...) to allow compilation in .NET 2003
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"

#include "saturn.h"

#include "saturn1b.h"

static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hastp;
static MESHHANDLE hastp2;

PARTICLESTREAMSPEC srb_contrail = {
	0, 12.0, 1, 50.0, 0.3, 4.0, 4, 3.0, PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};
PARTICLESTREAMSPEC srb_exhaust = {
	0, 8.0, 2, 50.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,//	0, 4.0, 20, 150.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};


void Saturn1b::SetS4B()

{
	char VName2[256];

	strcpy (VName2, GetName());
	strcat (VName2, "-S4BSTG");

	Saturn::hs4bM=oapiGetVesselByName(VName2);
	VESSEL *targetvessel;

	targetvessel=oapiGetVesselInterface(hs4bM);
	targetvessel->ClearMeshes();
	VECTOR3 mesh_dir=_V(0,0,0);
	targetvessel->AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(0,4,-4.2);
	//targetvessel->AddMesh (hapsl, &mesh_dir);
	mesh_dir=_V(0,-4,-4.2);
	//targetvessel->AddMesh (hapsh, &mesh_dir);
}

void Saturn1b::SetFirstStage ()
{
	int i;
	UINT meshidx;
	ClearThrusterDefinitions();
	SetSize (45);
	SetEmptyMass (169000.0);
	SetPMI (_V(140,145,28));
	SetCrossSections (_V(395,380,115));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();
	ShiftCentreOfMass (_V(0,0,8.935));
	double TCP=-54.485-TCPO;//STG0O;
	SetTouchdownPoints (_V(0,-1.0,TCP), _V(-.7,.7,TCP), _V(.7,.7,TCP));
	VECTOR3 mesh_dir=_V(0,0,-14);
	AddMesh (hSat1stg1, &mesh_dir);
	mesh_dir=_V(0,0,2.2);
	AddMesh (hSat1intstg, &mesh_dir);
	mesh_dir=_V(0,0,9.25);
	AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(0,4,4.7);
    //vessel->AddMesh (hapsl, &mesh_dir);
	mesh_dir=_V(0,-4,4.7);
    //vessel->AddMesh (hapsh, &mesh_dir);
	mesh_dir=_V(1.85,1.85,24.5);
    AddMesh (hSat1stg21, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,24.5);
    AddMesh (hSat1stg22, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,24.5);
    AddMesh (hSat1stg23, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,24.5);
    AddMesh (hSat1stg24, &mesh_dir);
	mesh_dir=_V(0,SMVO,31.25);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(0,0,35.4);
	meshidx =AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,35.2);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,35.2);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0,0,40.35);
	meshidx = AddMesh (hsat5tower, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	SetView(35.2);

	Offset1st = -28.5;
	SetCameraOffset (_V(-1,1.0,35.15));

	// ************************* propellant specs **********************************
	if (!ph_1st)
		ph_1st  = CreatePropellantResource(407100); //1st stage Propellant
	SetDefaultPropellantResource (ph_1st); // display 1st stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	Offset1st = -80.1;//+STG0O;
	VECTOR3 m_exhaust_ref = {0,0,-1};
    VECTOR3 m_exhaust_pos5= {1,1,Offset1st+55};
    VECTOR3 m_exhaust_pos6= {-1,-1,Offset1st+55};
	VECTOR3 m_exhaust_pos7= {-1,1,Offset1st+55};
	VECTOR3 m_exhaust_pos8= {1,-1,Offset1st+55};
	VECTOR3 m_exhaust_pos1= {3,0,Offset1st+55};
    VECTOR3 m_exhaust_pos2= {-3,0,Offset1st+55};
	VECTOR3 m_exhaust_pos3= {0,3,Offset1st+55};
	VECTOR3 m_exhaust_pos4= {0,-3,Offset1st+55};
	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), 981000 , ph_1st, 2755);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), 981000 , ph_1st, 2755);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), 981000 , ph_1st, 2755);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), 981000 , ph_1st, 2755);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), 981000 , ph_1st, 2755);
	th_main[5] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), 981000 , ph_1st, 2755);
	th_main[6] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), 981000 , ph_1st, 2755);
	th_main[7] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), 981000 , ph_1st, 2755);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust2");
	thg_main = CreateThrusterGroup (th_main, 8, THGROUP_MAIN);
	for (i = 0; i < 8; i++)
		AddExhaust (th_main[i], 60.0, 0.80, tex);

	srb_exhaust.tex = oapiRegisterParticleTexture ("Contrail2");

	AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,-12), &srb_exhaust);
	AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,-12), &srb_exhaust);
	AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,-12), &srb_exhaust);
	AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,-12), &srb_exhaust);
	AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,-12), &srb_exhaust);
	AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,-12), &srb_exhaust);
	AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,-12), &srb_exhaust);
	AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,-12), &srb_exhaust);


	EnableTransponder (true);

	// **************************** NAV radios *************************************

	InitNavRadios (4);

	RPswitch10 = true;
	RPswitch11 = true;
}

void Saturn1b::SetSecondStage ()
{
	ClearThrusterDefinitions();
	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}
	UINT meshidx;
	SetSize (22);
	SetCOG_elev (15.225);
	SetEmptyMass (40000);
	SetPMI (_V(94,94,20));
	SetCrossSections (_V(267,267,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	ShiftCentreOfMass (_V(0,0,12.25));
	VECTOR3 mesh_dir=_V(0,0,9.25-12.25);
    AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(0,0,2.2-12.25);
	AddMesh (hSat1intstg, &mesh_dir);
	mesh_dir=_V(0,4,5.2-12.25);
    //vessel->AddMesh (hapsl, &mesh_dir);
	mesh_dir=_V(0,-4,5.2-12.25);
    //vessel->AddMesh (hapsh, &mesh_dir);
	mesh_dir=_V(1.85,1.85,24.5-12.25);
    AddMesh (hSat1stg21, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,24.5-12.25);
    AddMesh (hSat1stg22, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,24.5-12.25);
    AddMesh (hSat1stg23, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,24.5-12.25);
    AddMesh (hSat1stg24, &mesh_dir);
	mesh_dir=_V(0,SMVO,31.25-12.25);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(0,0,35.4-12.25);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	
	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,22.95);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,22.95);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	
    SetView(22.95);


	//Don't Forget the Hatch
	mesh_dir=_V(0.02,1.35,35.415-12.25);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,40.35-12.25);
	meshidx = AddMesh (hsat5tower, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);


		// ************************* propellant specs **********************************
	if (!ph_3rd)
		ph_3rd  = CreatePropellantResource(114000); //2nd stage Propellant
	SetDefaultPropellantResource (ph_3rd); // display 3rd stage propellant level in generic HUD


	// *********************** thruster definitions ********************************


	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG1O+9};
	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), 1251725, ph_3rd, 5259.5);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);
	AddExhaust (th_main[0], 25.0, 1.5,SMMETex);

//  Ullage rockets (3)

	SetEngineLevel(ENGINE_MAIN, 0.0);

	SetCameraOffset (_V(-1,1.0,31.15-STG1O));
	VECTOR3	m_exhaust_pos6= _V(3.27,0.46,-2-STG1O+9);
	VECTOR3 m_exhaust_pos7= _V(-1.65,2.86,-2-STG1O+9);
	VECTOR3	m_exhaust_pos8= _V(-1.65,-2.86,-2-STG1O+9);
	int i;
	th_ver[0] = CreateThruster (m_exhaust_pos6, _V( -0.45,0.0,1),725 , ph_3rd, 45790.85);
	th_ver[1] = CreateThruster (m_exhaust_pos7, _V( 0.23,-0.39,1),725 , ph_3rd, 45790.85);
	th_ver[2] = CreateThruster (m_exhaust_pos8, _V( 0.23,0.39,1),725 , ph_3rd, 45790.85);
	for (i = 0; i < 3; i++)
		AddExhaust (th_ver[i], 11.0, 0.25);
	thg_ver = CreateThrusterGroup (th_ver, 3,THGROUP_USER);
	SetThrusterGroupLevel(thg_ver,1.0);

	for (i=0;i<5;i++){
		ENGIND[i]=false;
	}
}

void Saturn1b::SetSecondStage1 ()
{
	ClearThrusterDefinitions();
	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}
	UINT meshidx;
	SetSize (25);
	SetCOG_elev (15.225);
	SetEmptyMass (40000);
	SetPMI (_V(94,94,20));
	SetCrossSections (_V(267,267,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	VECTOR3 mesh_dir=_V(0,0,9.25-12.25);
    AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(0,4,5.2-12.25);
    //vessel->AddMesh (hapsl, &mesh_dir);
	mesh_dir=_V(0,-4,5.2-12.25);
    //vessel->AddMesh (hapsh, &mesh_dir);
	mesh_dir=_V(1.85,1.85,24.5-12.25);
    AddMesh (hSat1stg21, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,24.5-12.25);
    AddMesh (hSat1stg22, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,24.5-12.25);
    AddMesh (hSat1stg23, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,24.5-12.25);
    AddMesh (hSat1stg24, &mesh_dir);
	mesh_dir=_V(0,SMVO,31.25-12.25);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(0,0,35.4-12.25);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,22.95);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,22.95);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	
    SetView(22.95);



	//Don't Forget the Hatch
	mesh_dir=_V(0.02,1.35,35.415-12.25);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,40.30-12.25);
	meshidx = AddMesh (hsat5tower, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	SetCameraOffset (_V(-1,1.0,33.15-STG1O));
		// ************************* propellant specs **********************************
	if (!ph_3rd)
		ph_3rd  = CreatePropellantResource(114000); //2nd stage Propellant
	SetDefaultPropellantResource (ph_3rd); // display 3rd stage propellant level in generic HUD


	// *********************** thruster definitions ********************************


	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG1O+9};
	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), 1251725, ph_3rd, 5259.5);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	AddExhaust (th_main[0], 25.0, 1.5,SMMETex);

	SetEngineLevel(ENGINE_MAIN, 0.0);;

//  Ullage rockets (3)

	VECTOR3	m_exhaust_pos6= _V(3.27,0.46,-2-STG1O+9);
	VECTOR3 m_exhaust_pos7= _V(-1.65,2.86,-2-STG1O+9);
	VECTOR3	m_exhaust_pos8= _V(-1.65,-2.86,-2-STG1O+9);
	int i;
	th_ver[0] = CreateThruster (m_exhaust_pos6, _V( -0.45,0.0,1),725 , ph_3rd, 45790.85);
	th_ver[1] = CreateThruster (m_exhaust_pos7, _V( 0.23,-0.39,1),725 , ph_3rd, 45790.85);
	th_ver[2] = CreateThruster (m_exhaust_pos8, _V( 0.23,0.39,1),725 , ph_3rd, 45790.85);
	for (i = 0; i < 3; i++)
		AddExhaust (th_ver[i], 11.0, 0.25);
	thg_ver = CreateThrusterGroup (th_ver, 3,THGROUP_USER);
	SetThrusterGroupLevel(thg_ver,1.0);
}

void Saturn1b::SetSecondStage2 ()
{
	ClearThrusterDefinitions();
	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}
	SetSize (25);
	SetCOG_elev (15.225);
	SetEmptyMass (40000);
	SetPMI (_V(94,94,20));
	SetCrossSections (_V(267,267,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//vessel->ShiftCentreOfMass (_V(0,0,12.25));
	VECTOR3 mesh_dir=_V(0,0,9.25-12.25);
    AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(0,4,5.2-12.25);
    //vessel->AddMesh (hapsl, &mesh_dir);
	mesh_dir=_V(0,-4,5.2-12.25);
    //vessel->AddMesh (hapsh, &mesh_dir);
	mesh_dir=_V(1.85,1.85,24.5-12.25);
    AddMesh (hSat1stg21, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,24.5-12.25);
    AddMesh (hSat1stg22, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,24.5-12.25);
    AddMesh (hSat1stg23, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,24.5-12.25);
    AddMesh (hSat1stg24, &mesh_dir);
	mesh_dir=_V(0,SMVO,31.25-12.25);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(0,0,35.4-12.25);
	UINT meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,22.95);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,22.95);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	
    SetView(22.95);



	//Don't Forget the Hatch
	mesh_dir=_V(0.02,1.35,35.530-12.25);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,36.95-12.25);
	AddMesh (hprobe, &mesh_dir);
		// ************************* propellant specs **********************************
	if (!ph_3rd)
		ph_3rd  = CreatePropellantResource(114000); //2nd stage Propellant
	SetDefaultPropellantResource (ph_3rd); // display 3rd stage propellant level in generic HUD


	// *********************** thruster definitions ********************************


	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG1O+9};
	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1),1251725 , ph_3rd, 5259.5);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	thg_ver = 0;

	AddExhaust (th_main[0], 25.0, 1.5, SMMETex);

	SetThrusterLevel(th_main[0], 1.0);
	SetCameraOffset (_V(-1,1.0,35.4-STG1O));

	LAUNCHIND[2]=true;
}

void Saturn1b::SetASTPStage ()
{
	ClearThrusterDefinitions();
	UINT meshidx;
	probeidx=0;
	SetSize (4.0);
	SetCOG_elev (3.5);
	SetEmptyMass (19318);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(15,15,9.2));
	SetCrossSections (_V(40,40,14));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.3));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
	ClearExhaustRefs();
    ClearAttExhaustRefs();
	//vessel->ShiftCentreOfMass (_V(0,0,21.5));
	VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
	AddMesh (hSM, &mesh_dir);

	mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
	AddMesh (hSMhga, &mesh_dir);
	mesh_dir=_V(0,0,34.4-12.25-21.5);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	//Don't Forget the Hatch
	mesh_dir=_V(0.02,1.35,34.54-12.25-21.5);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0.0,-0.2,37.40-12.25-21.5);
	meshidx = AddMesh (hastp, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	VECTOR3 dockpos = {0,0,35.90-12.25-21.5+2.8};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams (dockpos, dockdir, dockrot);

	dockstate=3;

	SetView();
	AddRCSJets(-2.30,995);

    // ************************* propellant specs **********************************
	if (!ph_sps)  ph_sps  = CreatePropellantResource(20500,10500); //SPS stage Propellant
	SetDefaultPropellantResource (ph_sps); // display SPS stage propellant level in generic HUD
	if (!ph_rcs0)  ph_rcs0  = CreatePropellantResource(500); //RCS stage Propellant

	// *********************** thruster definitions ********************************

	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG1O};
	// orbiter main thrusters
	th_main[0] = CreateThruster (_V( 0,0,-6.5), _V( 0,0,1),100552.5 , ph_sps, 3778.5);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	AddExhaust (th_main[0], 20.0, 2.25, SMMETex);

	ActivateASTP = true;
}

void Saturn1b::AddStageOneInterstage()

{
	if (hstg1) {
		VESSEL *stg1vessel = oapiGetVesselInterface(hstg1);
		VECTOR3 mesh_dir = _V(0, 0, 16.2);
		stg1vessel->AddMesh (hSat1intstg, &mesh_dir);
	}
}

void Saturn1b::DockStage (UINT dockstatus)
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

	if (dockstatus == 0)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0,0,-4.0);
		ofs2 = OFS_TOWER;
		vel2 = _V(2.0,2.0,12.0);
	}
	else if (dockstatus == 1)
	{
	 	ofs1 = OFS_STAGE2;
		vel1 = _V(0,0,-0.235);
		ofs2 = OFS_STAGE21;
		vel2 = _V(0.5,0.5,-0.55);
		ofs3 = OFS_STAGE22;
		vel3 = _V(-0.5,0.5,-0.55);
		ofs4 = OFS_STAGE23;
		vel4 = _V(0.5,-0.5,-0.55);
		ofs5 = OFS_STAGE24;
		vel5 = _V(-0.5,-0.5,-0.55);
	}

	else if (dockstatus == 2||dockstatus == 6)
	{
	 	ofs1 = RelPos;
		vel1 = _V(0,0,-0.5);
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
	VESSELSTATUS vs4b;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,0.6);

	VECTOR3 mesh_dir=_V(0,0,0);

   switch (dockstatus)	{

   case 2:
	   	//Interface initialization for mesh modification to SIVB
		VESSEL *targetvessel;
		Undock(0);
		targetvessel=oapiGetVesselInterface(hs4bM);
		targetvessel->ClearMeshes();
		targetvessel->AddMesh (hSat1stg2, &mesh_dir);
		mesh_dir=_V(0,4,-4.2);
		//targetvessel->AddMesh (hapsl, &mesh_dir);
		mesh_dir=_V(0,-4,-4.2);
		//targetvessel->AddMesh (hapsh, &mesh_dir);
		//Release of Slaved Stage
		targetvessel->SetDockParams(_V(0.0,0.0,0.0),_V(0.0,0.0,0.0),_V(0.0,0.0,18.0));
		//Time to hear the Stage separation
		SMJetS.play();
		//Now Lets reconfigure Apollo for the DM.
		SetASTPStage ();
		dockstate=3;
		bManualUnDock= false;
		SetAttitudeLinLevel(2,-1);
	   	break;

   case 3:
	if(bManualUnDock) {

	//DM Jetison preparation
	char VName2[256];
	ofs = OFS_DOCKING2;
	GetStatus (vs4b);
	vs4b.eng_main = vs4b.eng_hovr = 0.0;
	SetEngineLevel(ENGINE_MAIN, 0);
	StageTransform(this,&vs4b,ofs,vel);
	vs4b.status=0;
	vs4b.vrot.x = 0.0;
	vs4b.vrot.y = 0.0;
	vs4b.vrot.z = 0.0;
	strcpy (VName2, GetName()); strcat (VName2, "-ASTPDM");
	VESSEL::Create (VName2, "nSat1astp2", vs4b);
	hAstpDM=oapiGetVesselByName(VName2);
		if (ProbeJetison){
			SetCSM2Stage ();
			StageS.play();
			bManualUnDock= false;
			dockstate=4;
			ProbeJetison=false;
			break;
		}
		else{
			FIRSTCSM = false;
			ShiftCentreOfMass (_V(0,0,-21.5));
			SetCSMStage ();
			SMJetS.play();
			dockstate=4;
			bManualUnDock= false;
			break;
		}
	}
	break;
   case 4:
	   //vessel->Undock(0);
	   if (hAstpDM){
		//
		   	SetASTPStage ();
			DestroyAstp=true;

	   }
	   break;
	case 5:
	   break;
	}
}

void Saturn1bLoadMeshes()

{
	hSat1stg1 = oapiLoadMeshGlobal ("nsat1stg1");
	hSat1intstg = oapiLoadMeshGlobal ("nsat1intstg");
	hSat1stg2 = oapiLoadMeshGlobal ("nsat1stg2");
	hSat1stg21 = oapiLoadMeshGlobal ("nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("nsat1stg24");
	hastp = oapiLoadMeshGlobal ("nASTP3");
	hastp2 = oapiLoadMeshGlobal ("nASTP2");
}