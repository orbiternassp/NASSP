/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn1Abort2

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

static int refcount = 0;
static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hSat1tower;
static MESHHANDLE hSM;

void SetSecondStage (VESSEL *vessel)
{

	vessel->SetSize (15);
	vessel->SetCOG_elev (15.225);
	vessel->SetEmptyMass (21500);
	vessel->SetMaxFuelMass (106000);
	vessel->SetFuelMass(106000);
	//vessel->SetISP (ISP_SECOND_VAC);
	vessel->SetISP (4160);
	//slThrust = CalcSeaLevelThrust(THRUST_SECOND_VAC,ISP_SECOND_VAC,ISP_SECOND_SL, 106000);
	//vacThrust = THRUST_SECOND_VAC;
	//vessel->SetMaxThrust (ENGINE_MAIN, slThrust);
	vessel->SetMaxThrust (ENGINE_MAIN, 0);
	vessel->SetMaxThrust (ENGINE_RETRO, 0);
	vessel->SetMaxThrust (ENGINE_HOVER, 0);
	vessel->SetMaxThrust (ENGINE_ATTITUDE, 5e3);
	vessel->SetEngineLevel(ENGINE_MAIN, 0.0);
	vessel->SetPMI (_V(94,94,20));
	vessel->SetCrossSections (_V(267,267,97));
	vessel->SetCW (0.1, 0.3, 1.4, 1.4);
	vessel->SetRotDrag (_V(0.7,0.7,1.2));
	vessel->SetPitchMomentScale (0);
	vessel->SetBankMomentScale (0);
	vessel->SetLiftCoeffFunc (0);
    vessel->ClearMeshes();
    vessel->ClearExhaustRefs();
    vessel->ClearAttExhaustRefs();
	VECTOR3 mesh_dir=_V(0,0,9.25-12.25);
    vessel->AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(1.85,1.85,19.8-12.25);
    vessel->AddMesh (hSat1stg21, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,19.8-12.25);
    vessel->AddMesh (hSat1stg22, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,19.8-12.25);
    vessel->AddMesh (hSat1stg23, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,19.8-12.25);
    vessel->AddMesh (hSat1stg24, &mesh_dir);
	mesh_dir=_V(0,-0.14,26.6-12.25);
	vessel->AddMesh (hSM, &mesh_dir);
}



// ==============================================================
// API interface
// ==============================================================


DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hSat1stg1 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg1");
		hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
		hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
		hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
		hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
		hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
		hSM = oapiLoadMeshGlobal ("ProjectApollo/SM");
	}
	return new VESSEL (hvessel, flightmodel);
}

DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	SetSecondStage (vessel);
}

