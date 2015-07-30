/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: EVA

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
  *	Revision 1.1  2009/02/18 23:20:56  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.2  2008/04/11 11:49:35  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.1  2006/08/12 15:05:35  movieman523
  *	Renamed EVA and LEVA files.
  *	
  **************************************************************************/

#pragma warning(disable : 4996 ) 

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"

#include "stdio.h"
#include "eva.h"
#include "tracer.h"
#include "math.h"

//
// Set the file name for the tracer code.
//

char trace_file[] = "ProjectApollo EVA.log";

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};

static OBJHANDLE hMaster;
static int refcount = 0;
static MESHHANDLE hCMPEVA;

EVA::EVA(OBJHANDLE hObj, int fmodel)
: VESSEL2 (hObj, fmodel)
{
	init();
}

void EVA::init ()
{
	GoDock1 = false;
	SetSize (3.5);
	SetEmptyMass (115);
	SetMaxFuelMass (10);
	SetFuelMass (10);
	SetISP(10000);
	SetMaxThrust (ENGINE_ATTITUDE, 5);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(5,5,5));
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
    VECTOR3 mesh_dir=_V(0,0,0);
    AddMesh (hCMPEVA, &mesh_dir);
}

EVA::~EVA()

{
	// Nothing for now.
}


// ==============================================================
// API interface
// ==============================================================
void EVA::clbkSaveState (FILEHANDLE scn)

{
	SaveDefaultState (scn);
	oapiWriteScenario_int (scn, "GODOCK", GoDock1 ? 1 : 0);
}

void EVA::clbkLoadStateEx(FILEHANDLE scn, void *status)

{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) 
	{
		if (!strnicmp (line, "GODOCK", 6)) 
		{
			int i;
			sscanf(line + 6, "%d", &i);
			GoDock1 = (i != 0);
		}
		else 
		{
            ParseScenarioLineEx (line, status);
        }
    }
}

int EVA::clbkConsumeDirectKey(char *keystate)

{
	if (KEYMOD_SHIFT (keystate))
	{
		return 0;
	}
	else if (KEYMOD_CONTROL (keystate))
	{

	}
	else
	{
		if (KEYDOWN (keystate, OAPI_KEY_E)) 
		{
			if (oapiAcceptDelayedKey (OAPI_KEY_E, 1.0))
			{
				GoDock1 = true;
				return 1;
			}
		}

	}
	return 0;
}

void EVA::clbkPreStep (double simt, double SimDT, double mjd)

{
	char EVAName[256]="";
	char CSMName[256]="";
	char MSName[256]="";

	strcpy(EVAName,GetName());
	double VessCount;
	int i=0;
	VessCount=oapiGetVesselCount();
	hMaster=oapiGetVesselByIndex(i);
	while (i<VessCount)i++;{
	oapiGetObjectName(hMaster,MSName,256);
	strcpy(CSMName,MSName);strcat(CSMName,"-EVA");
		if (strcmp(CSMName,EVAName)==0)
		{
			i=int(VessCount);
		}
	}
	sprintf(oapiDebugString(), "EVA Cable Attached to %s", MSName);
	VESSELSTATUS csmV;
	VESSELSTATUS evaV;
	VESSEL *csmvessel;
	VECTOR3 rdist = {0,0,0};
	VECTOR3 posr  = {0,0,0};
	VECTOR3 rvel  = {0,0,0};
	VECTOR3 RelRot  = {0,0,0};
	double dist = 0.0;
	double Vel = 0.0;

	if (hMaster)
	{
		csmvessel = oapiGetVesselInterface(hMaster);
		oapiGetRelativePos (GetHandle() ,hMaster, &posr);
		oapiGetRelativeVel (GetHandle() ,hMaster , &rvel);
		GetStatus(evaV);
		csmvessel->GetStatus(csmV);
		GlobalRot (posr, RelRot);
		dist = sqrt(posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);
		Vel = sqrt(rvel.x * rvel.x + rvel.y * rvel.y + rvel.z * rvel.z);
		if (dist >= 25)
		{
			rvel  = evaV.rvel-csmV.rvel;
			rvel.x = -rvel.x;
			rvel.y = -rvel.y;
			rvel.z = -rvel.z;
			GetStatus(evaV);
			csmvessel->GetStatus(csmV);
			evaV.rvel = csmV.rvel + rvel;
			DefSetState(&evaV);
		}
		if (GoDock1){
			sprintf(oapiDebugString(), "EVA Back CSM Mode Relative Distance M/s %f", dist);
			if (dist <= 0.55 && dist>=0.50 ){
				GoDock1 =false;
				oapiSetFocusObject(hMaster);
				oapiDeleteVessel(GetHandle());
			}
		}

	}
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++)
	{
		hCMPEVA = oapiLoadMeshGlobal ("ProjectApollo/saturn1_CMP_EVA");
	}
	return new EVA (hvessel, flightmodel);
}

void EVA::clbkSetClassCaps (FILEHANDLE cfg)
{
	init();
}

DLLCLBK void ovcExit (VESSEL *vessel)
{

}