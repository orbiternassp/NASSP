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
#include "sat5abort1.h"

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};


static int refcount = 0;
static MESHHANDLE hsat5stg1;
static MESHHANDLE hsat5intstg;
static MESHHANDLE hsat5stg2;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;
static MESHHANDLE hsat5tower;
static MESHHANDLE hSM;

const double TCPO=0;
const double SMVO = -0.14;
const double STG0O= 0;
const double STG1O= 10.25;
const double STG2O= 20;

Sat5Abort1::Sat5Abort1 (OBJHANDLE hObj, int fmodel)
: VESSEL3 (hObj, fmodel)

{
	init();
}

Sat5Abort1::~Sat5Abort1 ()

{
	// Nothing for now.
}

void Sat5Abort1::init()

{
}

void Sat5Abort1::Setup()

{
	double Offset1st;

	SetSize (55);
	SetEmptyMass (704000.0);
	SetMaxFuelMass (2396000);
	SetFuelMass(2396000);
	//SetISP (ISP_FIRST_VAC);
	SetISP (2303.05);
	//slThrust = CalcSeaLevelThrust(THRUST_FIRST_VAC,ISP_FIRST_VAC,ISP_FIRST_SL, 407100);
	//vacThrust = THRUST_FIRST_VAC;
	SetMaxThrust (ENGINE_MAIN,0 );//slThrust
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 0);
	SetPMI (_V(1147,1147,216.68));
	SetCrossSections (_V(1129,1133,152.4));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double TCP=-107.0+STG0O-TCPO;
	SetTouchdownPoints (_V(0,-1.0,0), _V(-.7,.7,0), _V(.7,.7,0));
	VECTOR3 mesh_dir=_V(0,0,-54+STG0O);
	AddMesh (hsat5stg1, &mesh_dir);
	mesh_dir=_V(0,0,-30.5+STG0O);
	AddMesh (hsat5intstg, &mesh_dir);
	mesh_dir=_V(0,0,-17.2+STG0O);
	AddMesh (hsat5stg2, &mesh_dir);
	mesh_dir=_V(0,0,2.+STG0O);
	AddMesh (hsat5stg3, &mesh_dir);
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
	mesh_dir=_V(0,0,23.5+STG0O);
	Offset1st = -80.1+STG0O;
	VECTOR3 m_exhaust_pos1= {3,3,Offset1st};
    VECTOR3 m_exhaust_pos2= {-3,-3,Offset1st};
	VECTOR3 m_exhaust_pos3= {-3,3,Offset1st};
	VECTOR3 m_exhaust_pos4= {3,-3,Offset1st};
	VECTOR3 m_exhaust_pos5= {0,0,Offset1st};

	VECTOR3 m_exhaust_ref = {0,0,-1};
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos1, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos2, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos3, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos4, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos5, 120.0, 5.80, &m_exhaust_ref);
	SetCameraOffset (_V(-1,1.0,32.4+STG0O));
}



// ==============================================================
// API interface
// ==============================================================

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hsat5stg1 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg1");
		hsat5intstg = oapiLoadMeshGlobal ("ProjectApollo/sat5intstg");
		hsat5stg2 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg2");
		hsat5stg3 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg3");
		hsat5stg31 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg31");
		hsat5stg32 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg32");
		hsat5stg33 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg33");
		hsat5stg34 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg34");
		hSM = oapiLoadMeshGlobal ("ProjectApollo/SM");
	}
	return new Sat5Abort1 (hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat5Abort1*)vessel;
}

void Sat5Abort1::clbkSetClassCaps(FILEHANDLE cfg)
{
	init();
	Setup();
}

