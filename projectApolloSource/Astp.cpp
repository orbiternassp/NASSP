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
  *	Revision 1.2  2005/07/04 23:56:46  movieman523
  *	New C++ version. Totally untested!
  *	
  *	Revision 1.1  2005/02/11 12:15:50  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "orbitersdk.h"
#include "stdio.h"
#include "astp.h"

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


ASTP::ASTP (OBJHANDLE hObj, int fmodel)
: VESSEL (hObj, fmodel)

{
	init();
}

ASTP::~ASTP ()

{
	// Nothing for now.
}

void ASTP::init()

{
}

void ASTP::Setup()

{
	SetSize (15);
	SetCOG_elev (15.225);
	SetEmptyMass (23500);
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

	//
	// Something very odd is going on here. After docking, the ASTP mesh vanishes!
	//

    VECTOR3 mesh_dir=_V(0,0,0);
    AddMesh (hSat1stg2, &mesh_dir);
	mesh_dir=_V(0,0,13.3);
	AddMesh (hastp, &mesh_dir);

	//
	// DockRef = 0.0 -0.1 13.1
	// DockDir = 0 0 1
	// DockRot = 0 -1 0
	//

	VECTOR3 dockpos = {0.0, -0.1, 15.0};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};

	SetDockParams(dockpos, dockdir, dockrot);
}

// ==============================================================
// API interface
// ==============================================================


DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hSat1stg2 = oapiLoadMeshGlobal ("nsat1stg2");
		hastp = oapiLoadMeshGlobal ("nASTP2");
	}
	return new ASTP (hvessel, flightmodel);
}


DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	ASTP *sv = (ASTP *) vessel;
	sv->init();
	sv->Setup();
}

