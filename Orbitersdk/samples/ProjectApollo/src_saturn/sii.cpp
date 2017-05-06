/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SII class

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
#include "orbiterSDK.h"

#include "nasspdefs.h"
#include "sii.h"

#include <stdio.h>
#include <string.h>

//
// Meshes are globally loaded.
//

static MESHHANDLE hsat5stg2, hsat5stg2low;

void SIILoadMeshes()

{
	//
	// Saturn V
	//

	hsat5stg2 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg2");
	hsat5stg2low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg2");
}

SII::SII (OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	InitSII();
}

SII::~SII()

{
	// Nothing for now.
}

void SII::InitSII()

{
	int i;

	State = SII_STATE_SETUP;

	ph_retro = 0;
	ph_main = 0;

	thg_retro = 0;
	thg_main = 0;

	EmptyMass = 47500.0;
	MainFuel = 5000.0;

	RetrosFired = false;
	LowRes = false;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	CurrentThrust = 0.0;

	THRUST_SECOND_VAC  = 1023000;
	ISP_SECOND_SL   = 300*G;
	ISP_SECOND_VAC  = 418*G;

	for (i = 0; i < 4; i++)
		th_retro[i] = 0;

	th_main[0] = 0;

	SMMETex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_j2");
	exhaust_tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");
}

void SII::SetSII()

{
	ClearMeshes();
	VECTOR3 mesh_dir=_V(0,0,0);

	double mass = EmptyMass;

	ClearThrusterDefinitions();
	
	SetSize (10);
	SetPMI (_V(374, 374, 97));
	SetCOG_elev (2.0);
	SetCrossSections (_V(524, 524, 97));
	SetCW (0.5, 1.1, 2, 2.4);
	SetRotDrag (_V(2,2,2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	if (LowRes) {
		AddMesh (hsat5stg2low, &mesh_dir);
	}
	else {
		AddMesh (hsat5stg2, &mesh_dir);
	}

	SetEmptyMass (mass);

	AddEngines ();
}

void SII::clbkPreStep(double simt, double simdt, double mjd)

{

	MissionTime += simdt;

	//
	// Currently this just starts the retros and shuts down the main engines.
	// At some point it should then do any other simulation like ejecting 
	// camera pods.
	//

	switch (State) {

	case SII_STATE_SHUTTING_DOWN:
		if (!RetrosFired && thg_retro) {
			SetThrusterGroupLevel(thg_retro, 1.0);
			RetrosFired = true;
		}
		
		if (CurrentThrust > 0.0) {
			CurrentThrust -= simdt;
			for (int i = 0; i < 4; i++) {
				SetThrusterLevel(th_main[i], CurrentThrust);
			}
		}
		else {
			SetThrusterGroupLevel(THGROUP_MAIN, 0.0);
			State = SII_STATE_WAITING;
		}
		break;

	case SII_STATE_WAITING:
		//
		// Nothing for now.
		//
		break;

	default:
		break;
	}
}

void SII::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "T2V", THRUST_SECOND_VAC);
	oapiWriteScenario_float (scn, "I2S", ISP_SECOND_SL);
	oapiWriteScenario_float (scn, "I2V", ISP_SECOND_VAC);
	oapiWriteScenario_float (scn, "CTR", CurrentThrust);
}

typedef union
{
	struct
	{
		unsigned int RetrosFired:1;
		unsigned int LowRes:1;
	} u;
	unsigned long word;
} MainState;

int SII::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.RetrosFired = RetrosFired;
	state.u.LowRes = LowRes;

	return state.word;
}

void SII::AddEngines()

{
	//
	// Just in case someone removes all the retros, do nothing.
	//

	if (!RetroNum)
		return;

	VECTOR3 m_exhaust_pos1;
	VECTOR3 m_exhaust_pos2= {-2.83,-2.83,11.2};
	VECTOR3 m_exhaust_pos3= {-2.83,2.83,11.2};
	VECTOR3 m_exhaust_ref2 = {0.1,0.1,-1};
	VECTOR3 m_exhaust_ref3 = {0.1,-0.1,-1};
	VECTOR3 m_exhaust_pos4= {2.83,-2.83,11.2};
	VECTOR3 m_exhaust_pos5= {2.83,2.83,11.2};
	VECTOR3 m_exhaust_ref4 = {-0.1,0.1,-1};
	VECTOR3 m_exhaust_ref5 = {-0.1,-0.1,-1};

	if (!ph_retro)
		ph_retro = CreatePropellantResource(264);;

	if (!ph_main && (MainFuel > 0.0))
		ph_main = CreatePropellantResource(MainFuel);

	double thrust = 175500;

	if (!th_retro[0])
	{
		th_retro[0] = CreateThruster(m_exhaust_pos2, m_exhaust_ref2, thrust, ph_retro, 4000);
		th_retro[1] = CreateThruster(m_exhaust_pos3, m_exhaust_ref3, thrust, ph_retro, 4000);
		th_retro[2] = CreateThruster(m_exhaust_pos4, m_exhaust_ref4, thrust, ph_retro, 4000);
		th_retro[3] = CreateThruster(m_exhaust_pos5, m_exhaust_ref5, thrust, ph_retro, 4000);
	}

	thg_retro = CreateThrusterGroup(th_retro, 4, THGROUP_RETRO);

	int i;
	for (i = 0; i < 4; i++)
		AddExhaust(th_retro[i], 8.0, 0.2, exhaust_tex);

	m_exhaust_pos1= _V(-1.8,-1.8,-15);
	m_exhaust_pos2= _V(1.8,1.8,-15);
	m_exhaust_pos3= _V(-1.8,1.8,-15);
	m_exhaust_pos4 = _V(1.8,-1.8,-15);
	m_exhaust_pos5 = _V(0,0,-15);

	// orbiter main thrusters
	th_main[0] = CreateThruster(m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC , ph_main, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[1] = CreateThruster(m_exhaust_pos2,_V( 0,0,1),  THRUST_SECOND_VAC , ph_main, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[2] = CreateThruster(m_exhaust_pos3, _V( 0,0,1), THRUST_SECOND_VAC , ph_main, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[3] = CreateThruster(m_exhaust_pos4, _V( 0,0,1), THRUST_SECOND_VAC , ph_main, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_main[4] = CreateThruster(m_exhaust_pos5, _V( 0,0,1), THRUST_SECOND_VAC , 0, ISP_SECOND_VAC, ISP_SECOND_SL);
	thg_main = CreateThrusterGroup(th_main, 5, THGROUP_MAIN);

	for (i = 0; i < 5; i++)
		AddExhaust(th_main[i], 25.0, 1.5, SMMETex);
}

void SII::SetMainState(int s)

{
	MainState state;

	state.word = s;

	RetrosFired = (state.u.RetrosFired != 0);
	LowRes = (state.u.LowRes != 0);
}

void SII::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline (scn, line))
	{
		if (!strnicmp (line, "MAINSTATE", 9))
		{
            int MainState = 0;;
			sscanf (line+9, "%d", &MainState);
			SetMainState(MainState);
		}
		else if (!strnicmp (line, "VECHNO", 6))
		{
			sscanf (line+6, "%d", &VehicleNo);
		}
		else if (!strnicmp (line, "EMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			EmptyMass = flt;
		}
		else if (!strnicmp (line, "FMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			MainFuel = flt;
		}
		else if (!strnicmp(line, "MISSNTIME", 9))
		{
            sscanf (line+9, "%f", &flt);
			MissionTime = flt;
		}
		else if (!strnicmp(line, "NMISSNTIME", 10))
		{
            sscanf (line + 10, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp(line, "LMISSNTIME", 10))
		{
            sscanf (line + 10, "%f", &flt);
			LastMissionEventTime = flt;
		}
		else if (!strnicmp(line, "T2V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			THRUST_SECOND_VAC = flt;
		}
		else if (!strnicmp(line, "I2S", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_SECOND_SL = flt;
		}
		else if (!strnicmp(line, "I2V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_SECOND_VAC = flt;
		}
		else if (!strnicmp(line, "CTR", 3))
		{
            sscanf (line + 3, "%f", &flt);
			CurrentThrust = flt;
		}
		else if (!strnicmp (line, "STATE", 5))
		{
			int i;
			sscanf (line+5, "%d", &i);
			State = (SIIState) i;
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}

	SetSII();
}

void SII::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	InitSII();
}

void SII::clbkDockEvent(int dock, OBJHANDLE connected)

{
}

void SII::SetState(SIISettings &state)

{
	if (state.SettingsType.SII_SETTINGS_GENERAL)
	{
		MissionTime = state.MissionTime;
		VehicleNo = state.VehicleNo;
		RetroNum = state.RetroNum;
		LowRes = state.LowRes;
	}

	if (state.SettingsType.SII_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
	}

	if (state.SettingsType.SII_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType.SII_SETTINGS_ENGINES)
	{
		THRUST_SECOND_VAC = state.THRUST_SECOND_VAC;
		ISP_SECOND_SL = state.ISP_SECOND_SL;
		ISP_SECOND_VAC = state.ISP_SECOND_VAC;
		CurrentThrust = state.CurrentThrust;
	}

	State = SII_STATE_SHUTTING_DOWN;

	SetSII();
}

static int refcount = 0;

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	VESSEL *v;

	if (!refcount++)
	{
		SIILoadMeshes();
	}

	v = new SII (hvessel, flightmodel);
	return v;
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	--refcount;

	if (!refcount)
	{

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) 
		delete (SII *)vessel;
}
