/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra

  ORBITER vessel module: ASTP

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
  **************************************************************************/

#include "orbitersdk.h"
#include "stdio.h"

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};


static int refcount = 0;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hastp;
static MESHHANDLE hapsh;
static MESHHANDLE hapsl;


void SetSecondStage (VESSEL *vessel)
{
	vessel->SetSize (15);
	vessel->SetCOG_elev (15.225);
	vessel->SetEmptyMass (23500);
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
    VECTOR3 mesh_dir=_V(0,0,0);
    vessel->AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(0,4,-4.2);
    //vessel->AddMesh (hapsl, &mesh_dir);
	mesh_dir=_V(0,-4,-4.2);
    //vessel->AddMesh (hapsh, &mesh_dir);
	mesh_dir=_V(0,0,13.3);
	vessel->AddMesh (hastp, &mesh_dir);

	VECTOR3 dockpos = {0.0, 0.0, -2.2};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};

	vessel->SetDockParams(dockpos, dockdir, dockrot);
}





// ==============================================================
// API interface
// ==============================================================


DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hSat1stg2 = oapiLoadMeshGlobal ("nsat1stg2");
		hastp = oapiLoadMeshGlobal ("nASTP2");
		hapsh = oapiLoadMeshGlobal ("napshigh");
		hapsl = oapiLoadMeshGlobal ("napslow");
	}
	return new VESSEL (hvessel, flightmodel);
}


DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	SetSecondStage(vessel);
}

