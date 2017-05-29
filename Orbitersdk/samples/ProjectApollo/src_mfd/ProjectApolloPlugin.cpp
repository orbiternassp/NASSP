/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008

  Project Apollo MFD

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

#define STRICT
#define ORBITER_MODULE

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"

#include "math.h"
#include "windows.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "lemcomputer.h"
#include "saturn.h"
#include "LEM.h"
#include "Crawler.h"
#include "papi.h"
#include <stdio.h>

#include "MFDResource.h"
#include "ProjectApolloMFD.h"
#include "ProjectApolloChecklistMFD.h"
#include "ProjectApolloPlugin.h"

DLLCLBK void opcDLLInit(HINSTANCE hDLL)
{
	ProjectApolloMFDopcDLLInit (hDLL);
	ProjectApolloChecklistMFDopcDLLInit (hDLL);
}

DLLCLBK void opcDLLExit(HINSTANCE hDLL)
{
	ProjectApolloMFDopcDLLExit (hDLL);
	ProjectApolloChecklistMFDopcDLLExit (hDLL);
}

DLLCLBK void opcPreStep(double simt, double simdt, double mjd)
{
	ProjectApolloMFDopcTimestep (simt,simdt,mjd);
	ProjectApolloChecklistMFDopcTimestep (simt,simdt,mjd);
}
