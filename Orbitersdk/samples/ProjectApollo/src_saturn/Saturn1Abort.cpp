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
#include "Saturn1Abort.h"

static int refcount = 0;
static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hSat1tower;
static MESHHANDLE hSM;

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
}

void Sat1Abort1::Setup()
{
	SetSize (25);
	SetCOG_elev (21.2);
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
	SetTouchdownPoints (_V(0,-1.0,-22.185), _V(-.7,.7,-22.185), _V(.7,.7,-22.185));
	VECTOR3 mesh_dir=_V(0,0,-18.7);
	AddMesh (hSat1stg1, &mesh_dir);
    mesh_dir=_V(0,0,-2.5);
	AddMesh (hSat1intstg, &mesh_dir);
    mesh_dir=_V(0,0,9.25);
	AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(1.85,1.85,19.8);
    AddMesh (hSat1stg21, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,19.8);
    AddMesh (hSat1stg22, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,19.8);
    AddMesh (hSat1stg23, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,19.8);
    AddMesh (hSat1stg24, &mesh_dir);
	mesh_dir=_V(0,-0.14,26.6);
	AddMesh (hSM, &mesh_dir);
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
}



// ==============================================================
// API interface
// ==============================================================


DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hSat1stg1 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg1");
		hSat1intstg = oapiLoadMeshGlobal("ProjectApollo/nsat1intstg");
		hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
		hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
		hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
		hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
		hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
		hSM = oapiLoadMeshGlobal ("ProjectApollo/SM");
	}
	return new Sat1Abort1(hvessel, flightmodel);

}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat1Abort1*)vessel;
}

void Sat1Abort1::clbkSetClassCaps(FILEHANDLE cfg)
{
	Setup();
}

