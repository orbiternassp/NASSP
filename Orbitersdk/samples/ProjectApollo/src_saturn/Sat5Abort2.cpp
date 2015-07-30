/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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
  *	Revision 1.4  2008/04/11 11:49:29  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.3  2006/01/04 23:06:03  movieman523
  *	Moved meshes into ProjectApollo directory and renamed a few.
  *	
  *	Revision 1.2  2005/11/21 23:08:15  movieman523
  *	Moved more mesh files into the ProjectApollo directory.
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "stdio.h"

#include "sat5abort2.h"

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};


static int refcount = 0;
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
const double STG0O= 12.4;
const double STG1O= 0;
const double STG2O= 20;

Sat5Abort2::Sat5Abort2 (OBJHANDLE hObj, int fmodel)
: VESSEL (hObj, fmodel)

{
	init();
}

Sat5Abort2::~Sat5Abort2 ()

{
	// Nothing for now.
}

void Sat5Abort2::init()

{
	SetSize (15);
	SetCOG_elev (15.225);
	SetEmptyMass (233000);
	SetMaxFuelMass (471000);
	SetFuelMass(444760);
	//SetISP (ISP_SECOND_VAC);
	SetISP (4780);
	//slThrust = CalcSeaLevelThrust(THRUST_SECOND_VAC,ISP_SECOND_VAC,ISP_SECOND_SL, 106000);
	//vacThrust = THRUST_SECOND_VAC;
	//vessel->SetMaxThrust (ENGINE_MAIN, slThrust);
	SetMaxThrust (ENGINE_MAIN, 6008625);
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 5e4);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(374,374,80));
	SetCrossSections (_V(524,524,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//vessel->ShiftCentreOfMass (_V(0,0,12.25));
	VECTOR3 mesh_dir=_V(0,0,-17.2-STG1O);
	AddMesh (hsat5stg2, &mesh_dir);
	mesh_dir=_V(0,0,2.-STG1O);
	AddMesh (hsat5stg3, &mesh_dir);
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
	VECTOR3 m_exhaust_ref = {0,0,-1};
	VECTOR3 m_exhaust_pos1= {0,0,-28.25-STG1O};
	VECTOR3 m_exhaust_pos2= {2,2,-28.25-STG1O};
	VECTOR3 m_exhaust_pos3= {-2,2,-28.25-STG1O};
	VECTOR3 m_exhaust_pos4 = {2,-2.0,-28.25-STG1O};
	VECTOR3 m_exhaust_pos5 = {-2,-2,-28.25-STG1O};
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos1, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos2, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos3, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos4, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos5, 25.0, 1.5, &m_exhaust_ref);
	status = 3;
	SetCameraOffset (_V(-1,1.0,32.4-STG1O));
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK void ovcSaveState (VESSEL *vessel, FILEHANDLE scn)

{
	Sat5Abort2 *sv = (Sat5Abort2 *) vessel;
	sv->SaveDefaultState (scn);
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hsat5stg2 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg2");
		hsat5stg3 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg3");
		hsat5stg31 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg31");
		hsat5stg32 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg32");
		hsat5stg33 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg33");
		hsat5stg34 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg34");
		hSM = oapiLoadMeshGlobal ("ProjectApollo/SM");

	}
	return new Sat5Abort2 (hvessel, flightmodel);
}

DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	Sat5Abort2 *sv = (Sat5Abort2 *) vessel;
	sv->init();
}

