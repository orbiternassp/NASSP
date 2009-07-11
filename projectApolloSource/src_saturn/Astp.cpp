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
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.7  2008/04/11 11:49:05  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.6  2008/01/18 05:57:23  movieman523
  *	Moved SIVB creation code into generic Saturn function, and made ASTP sort of start to work.
  *	
  *	Revision 1.5  2008/01/14 01:17:01  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.4  2005/11/21 23:08:15  movieman523
  *	Moved more mesh files into the ProjectApollo directory.
  *	
  *	Revision 1.3  2005/08/15 23:42:50  movieman523
  *	Improved ASTP a bit. Still buggy, but vaguely working.
  *	
  *	Revision 1.2  2005/07/04 23:56:46  movieman523
  *	New C++ version. Totally untested!
  *	
  *	Revision 1.1  2005/02/11 12:15:50  tschachim
  *	Initial version
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "stdio.h"
#include "astp.h"

static int refcount = 0;
static MESHHANDLE hastp;

ASTP::ASTP (OBJHANDLE hObj, int fmodel)
: Payload (hObj, fmodel)

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
	//
	// These numbers are all wrong since they're for the SIVB + ASTP combo. We need
	// to find the correct numbers to use.
	//
	// Currently it also only has one docking port.
	//
	SetSize (15);
	SetCOG_elev (15.225);
	SetEmptyMass (23500);
	SetPMI (_V(94,94,20));
	SetCrossSections (_V(5,5,5));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

    VECTOR3 mesh_dir=_V(0,0,0);
	AddMesh (hastp, &mesh_dir);

	//
	// DockRef = 0.0 -0.1 13.1
	// DockDir = 0 0 1
	// DockRot = 0 -1 0
	//

	VECTOR3 dockpos = _V(0.0, 0.15, 1.2);
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = _V(-1.0, 0.0, 0);

	SetDockParams(dockpos, dockdir, dockrot);
	hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
}

// ==============================================================
// API interface
// ==============================================================


DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++)
	{
		hastp = oapiLoadMeshGlobal ("ProjectApollo/nASTP2");
	}
	return new ASTP (hvessel, flightmodel);
}


DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	ASTP *sv = (ASTP *) vessel;
	sv->init();
	sv->Setup();
}

