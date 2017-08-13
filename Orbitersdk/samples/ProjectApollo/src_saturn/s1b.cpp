/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: S1B class

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
#include "s1b.h"

#include <stdio.h>
#include <string.h>


PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0, 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};


S1B::S1B (OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	InitS1b();
}

S1B::~S1B()

{
	// Nothing for now.
}

void S1B::InitS1b()

{
	int i;

	State = SIB_STATE_HIDDEN;

	hsat1stg1 = 0;
	hSat1intstg = 0;

	ph_retro = 0;
	ph_main = 0;

	thg_retro = 0;
	thg_main = 0;

	EmptyMass = 37500.0;
	MainFuel = 5000.0;

	CurrentThrust = 0.0;

	RetrosFired = false;
	LowRes = false;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	THRUST_FIRST_VAC = 8062309;
	ISP_FIRST_SL    = 265*G;
	ISP_FIRST_VAC   = 304*G;

	for (i = 0; i < 4; i++)
		th_retro[i] = 0;

	for (i = 0; i < 8; i++)
		th_main[i] = 0;
}

void S1B::SetS1b()

{
	ClearMeshes();
	ClearThrusterDefinitions();
	
	SetSize (20);
	SetPMI (_V(78.45, 78.45, 12.00));
	SetCOG_elev (19.0);
	SetCrossSections (_V(230.35, 229.75, 58.85));
	SetCW (0.5, 1.1, 2, 2.4);
	SetRotDrag (_V(2,2,2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetEmptyMass (EmptyMass);
}

void S1B::clbkPreStep(double simt, double simdt, double mjd)

{
	if (State == SIB_STATE_HIDDEN)
	{
		return;
	}

	MissionTime += simdt;

	//
	// Currently this just starts the retros. At some point it should
	// run down the engines if they haven't completely shut down, and
	// then do any other simulation like ejecting camera pods.
	//

	switch (State) {

	case S1B_STATE_SHUTTING_DOWN:
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
			State = S1B_STATE_WAITING;
		}
		break;

	case S1B_STATE_WAITING:
		//
		// Nothing for now.
		//
		break;

	default:
		break;
	}
}

void S1B::clbkSaveState (FILEHANDLE scn)

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
	oapiWriteScenario_float (scn, "T1V", THRUST_FIRST_VAC);
	oapiWriteScenario_float (scn, "I1S", ISP_FIRST_SL);
	oapiWriteScenario_float (scn, "I1V", ISP_FIRST_VAC);
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

int S1B::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.RetrosFired = RetrosFired;
	state.u.LowRes = LowRes;

	return state.word;
}

void S1B::AddEngines()

{
	//
	// Just in case someone removes all the retros, do nothing.
	//

	if (!RetroNum)
		return;

	VECTOR3 m_exhaust_pos2= {-2.5,-2.5, 20.2};
	VECTOR3 m_exhaust_pos3= {-2.5,2.5, 20.2};
	VECTOR3 m_exhaust_pos4= {2.5,-2.5, 20.2};
	VECTOR3 m_exhaust_pos5= {2.5,2.5, 20.2};

	if (!ph_retro)
		ph_retro = CreatePropellantResource(200);

	if (!ph_main && MainFuel > 0.0)
		ph_main = CreatePropellantResource(MainFuel);

	double thrust = 100000;

	if (!th_retro[0]) {
		th_retro[0] = CreateThruster (m_exhaust_pos2, _V(0.1, 0.1, -0.9), thrust, ph_retro, 4000);
		th_retro[1] = CreateThruster (m_exhaust_pos3, _V(0.1, -0.1, -0.9), thrust, ph_retro, 4000);
		th_retro[2] = CreateThruster (m_exhaust_pos4, _V(-0.1, 0.1, -0.9), thrust, ph_retro, 4000);
		th_retro[3] = CreateThruster (m_exhaust_pos5, _V(-0.1, -0.1, -0.9), thrust, ph_retro, 4000);
	}

	thg_retro = CreateThrusterGroup(th_retro, 4, THGROUP_RETRO);

	int i;
	SURFHANDLE tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");
	for (i = 0; i < 4; i++) {
		AddExhaust(th_retro[i], 8.0, 0.2, tex);
		AddExhaustStream(th_retro[i], &solid_exhaust);
	}

	double Offset1st = -65.5;

	m_exhaust_pos5= _V(0,1.414,Offset1st+55);
    VECTOR3 m_exhaust_pos6= {1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos7= {0,-1.414,Offset1st+55};
	VECTOR3 m_exhaust_pos8= {-1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos1= _V(2.12,2.12,Offset1st+55);
    m_exhaust_pos2= _V(2.12,-2.12,Offset1st+55);
	m_exhaust_pos3= _V(-2.12,-2.12,Offset1st+55);
	m_exhaust_pos4= _V(-2.12,2.12,Offset1st+55);

	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_FIRST_VAC , 0, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[5] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), THRUST_FIRST_VAC , 0, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[6] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), THRUST_FIRST_VAC , 0, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[7] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), THRUST_FIRST_VAC , 0, ISP_FIRST_VAC, ISP_FIRST_SL);

	thg_main = CreateThrusterGroup (th_main, 8, THGROUP_MAIN);
	for (i = 0; i < 8; i++)
		AddExhaust (th_main[i], 60.0, 0.80, tex);
}

void S1B::SetMainState(int s)

{
	MainState state;

	state.word = s;

	RetrosFired = (state.u.RetrosFired != 0);
	LowRes = (state.u.LowRes != 0);
}

void S1B::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

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
		else if (!strnicmp(line, "T1V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			THRUST_FIRST_VAC = flt;
		}
		else if (!strnicmp(line, "I1S", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_FIRST_SL = flt;
		}
		else if (!strnicmp(line, "I1V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_FIRST_VAC = flt;
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
			State = (S1bState) i;
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}

	LoadMeshes(LowRes); 
	if (State > SIB_STATE_HIDDEN)
		ShowS1b();	

}

void S1B::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	SetS1b();
}

void S1B::LoadMeshes(bool lowres)

{
	LowRes = lowres;
	if (LowRes)
	{
		hsat1stg1 = oapiLoadMeshGlobal("ProjectApollo/LowRes/nsat1stg1");
		hSat1intstg = oapiLoadMeshGlobal("ProjectApollo/LowRes/nsat1intstg");
	}
	else
	{
		hsat1stg1 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg1");
		hSat1intstg = oapiLoadMeshGlobal("ProjectApollo/nsat1intstg");
	}	
	solid_exhaust.tex = oapiRegisterParticleTexture ("Contrail2");
}

void S1B::clbkDockEvent(int dock, OBJHANDLE connected)

{
}


void S1B::SetState(S1BSettings &state)

{
	if (state.SettingsType.S1B_SETTINGS_GENERAL)
	{
		MissionTime = state.MissionTime;
		VehicleNo = state.VehicleNo;
		RetroNum = state.RetroNum;
	}

	if (state.SettingsType.S1B_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
	}

	if (state.SettingsType.S1B_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType.S1B_SETTINGS_ENGINES)
	{
		THRUST_FIRST_VAC = state.THRUST_FIRST_VAC;
		ISP_FIRST_SL = state.ISP_FIRST_SL;
		ISP_FIRST_VAC = state.ISP_FIRST_VAC;
		CurrentThrust = state.CurrentThrust;
		EngineNum = state.EngineNum;
	}

	ShowS1b();
	State = S1B_STATE_SHUTTING_DOWN;
}

void S1B::ShowS1b()

{
	SetEmptyMass(EmptyMass);

	VECTOR3 mesh_dir=_V(0,0,0);
	AddMesh(hsat1stg1, &mesh_dir);

	mesh_dir = _V(0, 0, 16.2);
	AddMesh(hSat1intstg, &mesh_dir);

	AddEngines();
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	return new S1B(hvessel, flightmodel);
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	if (vessel) 
		delete (S1B *)vessel;
}
