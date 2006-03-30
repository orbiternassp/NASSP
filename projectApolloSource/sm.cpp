/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Service Module class

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
  *	Revision 1.1  2006/03/30 00:21:37  movieman523
  *	Pass empty mass correctly and remember to check in SM files :).
  *	
  *	
  **************************************************************************/

#include "orbiterSDK.h"

#include "nasspdefs.h"
#include "sm.h"

#include <stdio.h>
#include <string.h>

//
// Meshes are globally loaded.
//

MESHHANDLE hSM;
MESHHANDLE hSMRCS;
MESHHANDLE hSMSPS;
MESHHANDLE hSMPanel1;
MESHHANDLE hSMPanel2;
MESHHANDLE hSMPanel3;
MESHHANDLE hSMPanel4;
MESHHANDLE hSMPanel5;
MESHHANDLE hSMPanel6;
MESHHANDLE hSMhga;

#define LOAD_MESH(var, name) var = oapiLoadMeshGlobal(name);

void SMLoadMeshes()

{
	//
	// SM meshes
	//

	LOAD_MESH(hSM, "ProjectApollo/SM-core");
	LOAD_MESH(hSMRCS, "ProjectApollo/SM-RCS");
	LOAD_MESH(hSMSPS, "ProjectApollo/SM-SPS");
	LOAD_MESH(hSMPanel1, "ProjectApollo/SM-Panel1");
	LOAD_MESH(hSMPanel2, "ProjectApollo/SM-Panel2");
	LOAD_MESH(hSMPanel3, "ProjectApollo/SM-Panel3");
	LOAD_MESH(hSMPanel4, "ProjectApollo/SM-Panel4");
	LOAD_MESH(hSMPanel5, "ProjectApollo/SM-Panel5");
	LOAD_MESH(hSMPanel6, "ProjectApollo/SM-Panel6");
	LOAD_MESH(hSMhga, "ProjectApollo/SM_HGA");
}

SM::SM (OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	InitSM();
}

SM::~SM()

{
	// Nothing for now.
}

void SM::InitSM()

{
	State = SM_STATE_SHUTTING_DOWN;

	EmptyMass = 6100.0;
	MainFuel = 5000.0;
	Realism = REALISM_DEFAULT;

	RetrosFired = false;
	LowRes = false;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	ph_rcsa = 0;
	ph_rcsb = 0;
	ph_rcsc = 0;
	ph_rcsd = 0;

	showSPS = true;
	showHGA = true;
	showRCS = true;
	showPanel1 = true;
	showPanel2 = true;
	showPanel3 = true;
	showPanel4 = true;
	showPanel5 = true;
	showPanel6 = true;
}

const double SMVO = -0.14;

void SM::SetSM()

{
	ClearMeshes();

	double mass = EmptyMass + MainFuel;

	ClearThrusterDefinitions();
	
	SetSize (5);
	SetPMI (_V(20.0, 20.0, 2.00));
	SetCOG_elev (19.0);
	SetCrossSections (_V(20.0, 20.0, 10.0));
	SetCW (0.5, 1.1, 2, 2.4);
	SetRotDrag (_V(2,2,2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);

    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 mesh_dir=_V(0, SMVO, 0);

	AddMesh (hSM, &mesh_dir);

	if (showRCS)
		AddMesh (hSMRCS, &mesh_dir);

	if (showPanel1)
		AddMesh (hSMPanel1, &mesh_dir);

	if (showPanel2)
		AddMesh (hSMPanel2, &mesh_dir);

	if (showPanel3)
		AddMesh (hSMPanel3, &mesh_dir);

	if (showPanel4)
		AddMesh (hSMPanel4, &mesh_dir);

	if (showPanel5)
		AddMesh (hSMPanel5, &mesh_dir);

	if (showPanel6)
		AddMesh (hSMPanel6, &mesh_dir);

	if (showSPS) {
		mesh_dir = _V(0, SMVO, -1.5);
		AddMesh(hSMSPS, &mesh_dir);
	}

	if (showHGA) {
		mesh_dir=_V(-2.2,-1.7,-1.1);
		AddMesh (hSMhga, &mesh_dir);
	}

	SetEmptyMass (mass);

	AddEngines ();
}

void SM::clbkPreStep(double simt, double simdt, double mjd)

{
	MissionTime += simdt;

	//
	// Currently this just starts the retros. At some point it should
	// run down the engines if they haven't completely shut down, and
	// then do any other simulation like ejecting camera pods.
	//

	switch (State) {

	case SM_STATE_SHUTTING_DOWN:
		State = SM_STATE_WAITING;
		break;

	case SM_STATE_WAITING:
		//
		// Nothing for now.
		//
		break;

	default:
		break;
	}
}

void SM::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_int (scn, "REALISM", Realism);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
}

typedef union {
	struct {
		unsigned int showSPS:1;
		unsigned int showHGA:1;
		unsigned int showRCS:1;
		unsigned int showPanel1:1;
		unsigned int showPanel2:1;
		unsigned int showPanel3:1;
		unsigned int showPanel4:1;
		unsigned int showPanel5:1;
		unsigned int showPanel6:1;
		unsigned int LowRes:1;
		unsigned int A13Exploded:1;
	} u;
	unsigned long word;
} MainState;

int SM::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.showSPS = showSPS;
	state.u.showHGA = showHGA;
	state.u.showRCS = showRCS;
	state.u.showPanel1 = showPanel1;
	state.u.showPanel2 = showPanel2;
	state.u.showPanel3 = showPanel3;
	state.u.showPanel4 = showPanel4;
	state.u.showPanel5 = showPanel5;
	state.u.showPanel6 = showPanel6;
	state.u.LowRes = LowRes;
	state.u.A13Exploded = A13Exploded;

	return state.word;
}

void SM::AddEngines()

{
	//
	// Add the RCS. SPS won't fire with SM seperated.
	//

	if (!ph_rcsa)
		ph_rcsa = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsb)
		ph_rcsb = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsc)
		ph_rcsc = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsd)
		ph_rcsd = CreatePropellantResource(RCS_FUEL_PER_QUAD);
}

void SM::SetMainState(int s)

{
	MainState state;

	state.word = s;

	showSPS = (state.u.showSPS != 0);
	showHGA = (state.u.showHGA != 0);
	showRCS = (state.u.showRCS != 0);
	showPanel1 = (state.u.showPanel1 != 0);
	showPanel2 = (state.u.showPanel2 != 0);
	showPanel3 = (state.u.showPanel3 != 0);
	showPanel4 = (state.u.showPanel4 != 0);
	showPanel5 = (state.u.showPanel5 != 0);
	showPanel6 = (state.u.showPanel6 != 0);
	LowRes = (state.u.LowRes != 0);
	A13Exploded = (state.u.A13Exploded != 0);
}

void SM::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "MAINSTATE", 9)) {
            int MainState = 0;;
			sscanf (line+9, "%d", &MainState);
			SetMainState(MainState);
		}
		else if (!strnicmp (line, "VECHNO", 6)) {
			sscanf (line+6, "%d", &VehicleNo);
		}
		else if (!strnicmp (line, "EMASS", 5)) {
			sscanf (line+5, "%g", &flt);
			EmptyMass = flt;
		}
		else if (!strnicmp (line, "FMASS", 5)) {
			sscanf (line+5, "%g", &flt);
			MainFuel = flt;
		}
		else if (!strnicmp(line, "MISSNTIME", 9)) {
            sscanf (line+9, "%f", &flt);
			MissionTime = flt;
		}
		else if (!strnicmp(line, "NMISSNTIME", 10)) {
            sscanf (line + 10, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp(line, "LMISSNTIME", 10)) {
            sscanf (line + 10, "%f", &flt);
			LastMissionEventTime = flt;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			sscanf (line+5, "%d", &State);
		}
		else if (!strnicmp (line, "REALISM", 7)) {
			sscanf (line+7, "%d", &Realism);
		}
		else {
			ParseScenarioLineEx (line, vstatus);
        }
	}

	SetSM();
}

void SM::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	InitSM();
}

void SM::clbkDockEvent(int dock, OBJHANDLE connected)

{
}

void SM::SetState(SMSettings &state)

{
	if (state.SettingsType & SM_SETTINGS_GENERAL) {
		MissionTime = state.MissionTime;
		VehicleNo = state.VehicleNo;
		Realism = state.Realism;
		showHGA = state.showHGA;
		A13Exploded = state.A13Exploded;
	}

	if (state.SettingsType & SM_SETTINGS_MASS) {
		EmptyMass = state.EmptyMass;
	}

	if (state.SettingsType & SM_SETTINGS_FUEL) {
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType & SM_SETTINGS_ENGINES) {
		//
		// Nothing for now. Later we can enable or disable RCS as
		// appropriate based on seperation state.
		//
	}

	SetSM();

	//
	// Now the RCS propellant resource has been created, set the
	// fuel levels.
	//

	if (state.SettingsType & SM_SETTINGS_FUEL) {
		SetPropellantMass(ph_rcsa, state.RCSAFuelKg);
		SetPropellantMass(ph_rcsb, state.RCSBFuelKg);
		SetPropellantMass(ph_rcsc, state.RCSCFuelKg);
		SetPropellantMass(ph_rcsd, state.RCSDFuelKg);
	}
}

static int refcount = 0;

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	VESSEL *v;

	if (!refcount++) {
		SMLoadMeshes();
	}

	v = new SM (hvessel, flightmodel);
	return v;
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	--refcount;

	if (!refcount) {

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) 
		delete (SM *)vessel;
}
