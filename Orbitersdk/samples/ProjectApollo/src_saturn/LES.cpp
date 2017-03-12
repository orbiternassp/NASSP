/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LES class

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
#include "LES.h"

#include <stdio.h>
#include <string.h>

//
// Meshes are globally loaded.
//

static MESHHANDLE hLES;
static MESHHANDLE hPROBE;

//
// Solid rocket exhausts.
//

static PARTICLESTREAMSPEC solid_exhaust2 = {
	0, 1.0, 25, 100.0, 0.1, 0.5, 1.0, 2.5, 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0.1, 1.0,
	PARTICLESTREAMSPEC::ATM_PLIN, 0.3, 1.0
};

static PARTICLESTREAMSPEC solid_exhaust = {
	0, 
	1.0,	// size
	10,		// rate
	50.0,	// velocity
	0.1,	// velocity distribution
	1.0,	// lifetime
	1.0,		// growthrate
	3.5,	// atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0.1, 1.0,
	PARTICLESTREAMSPEC::ATM_PLIN, -0.5, 1.0
};

static PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	0.5,	// size
	25.0,	// rate
	30.0,	// velocity
	0.1,	// velocity distribution
	0.25,	// lifetime
	0.1,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0.5, 1.0,
	PARTICLESTREAMSPEC::ATM_PLIN, -0.3, 1.0
};

static SURFHANDLE exhaust_tex;

void LESLoadMeshes()

{
	//
	// Saturn V
	//

	hLES = oapiLoadMeshGlobal("ProjectApollo/BoostCover");
	hPROBE = oapiLoadMeshGlobal("ProjectApollo/CM-Probe");

	exhaust_tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");
	SURFHANDLE contrail_tex = oapiRegisterParticleTexture("Contrail2");
	solid_exhaust.tex = contrail_tex;
	srb_exhaust.tex = contrail_tex;
}

LES::LES (OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	InitLES();
}

LES::~LES()

{
	// Nothing for now.
}

void LES::InitLES()

{
	int i;

	State = LES_STATE_SETUP;

	ph_jettison = 0;
	ph_main = 0;

	thg_main = 0;

	EmptyMass = 2023.0;
	MainFuel = 1405.0;
	JettisonFuel = 159.0;

	FireMain = false;
	LowRes = false;
	ProbeAttached = false;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	//
	// ISPs are estimates.
	//

	ISP_SL   = 2200.0;
	ISP_VAC  = 2600.0;

	//
	// I'm not sure whether the thrust values quoted are for sea level
	// or vacuum. If they're sea-level then we should multiply them by
	// (ISP_VAC / ISP_SL) to get vacuum thrust.
	//

	THRUST_VAC_MAIN  = (653888.6 / 4.0);
	THRUST_VAC_JETTISON = (140119.0 / 2.0);

	for (i = 0; i < 2; i++)
		th_jettison[i] = 0;

	for (i = 0; i < 4; i++)
		th_main[0] = 0;
}

void LES::SetLES()

{
	ClearMeshes();
	VECTOR3 mesh_dir=_V(0,0,0);
	
	SetSize (4);
	SetPMI (_V(20, 20, 10));
	SetCrossSections (_V(8.5, 8.5, 12.5));

	SetCOG_elev (2.0);
	SetCW (5.5, 0.1, 3.4, 3.4);
	SetRotDrag (_V(0.07,0.07,0.003));

	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	AddMesh (hLES, &mesh_dir);

	if (ProbeAttached)
	{
		mesh_dir = _V(0, 0, -4.95);
		AddMesh(hPROBE, &mesh_dir);
	}

	SetEmptyMass (EmptyMass);

	AddEngines();

	//
	// Set up correctly for engine burn and 'cockpit' displays.
	//

	if (FireMain)
	{
		thg_main = CreateThrusterGroup (th_main, 4, THGROUP_MAIN);
		SetDefaultPropellantResource (ph_main);
	}
	else
	{
		thg_main = CreateThrusterGroup (th_jettison, 2, THGROUP_MAIN);
		SetDefaultPropellantResource (ph_jettison);
	}
}

void LES::clbkPreStep(double simt, double simdt, double mjd)

{
	MissionTime += simdt;

	//
	// Currently this just fires the main engines.
	//

	switch (State) {

	case LES_STATE_JETTISON:
		//
		// Fire thrusters, then wait.
		//
		SetThrusterGroupLevel(thg_main, 1.0);
		State = LES_STATE_WAITING;
		break;

	case LES_STATE_WAITING:
		//
		// Nothing for now.
		//
		break;

	default:
		break;
	}
}

void LES::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "JMASS", JettisonFuel);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "T2V", THRUST_VAC_MAIN);
	oapiWriteScenario_float (scn, "T2J", THRUST_VAC_JETTISON);
	oapiWriteScenario_float (scn, "I2S", ISP_SL);
	oapiWriteScenario_float (scn, "I2V", ISP_VAC);
}

typedef union {
	struct {
		unsigned int FireMain:1;
		unsigned int LowRes:1;
		unsigned int ProbeAttached:1;
	} u;
	unsigned long word;
} MainState;

int LES::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.FireMain = FireMain;
	state.u.LowRes = LowRes;
	state.u.ProbeAttached = ProbeAttached;

	return state.word;
}

void LES::AddEngines()

{
	ClearThrusterDefinitions();

	VECTOR3 m_exhaust_pos1= {-0.3,0.0, 2.0};
	VECTOR3 m_exhaust_pos2= {0.3,0.0, 2.0};

	//
	// Yes, the jettison engines _are_ supposed to point in different
	// directions! It's intended to rotate the LES after jettison to
	// move it away from the Saturn.
	//

	VECTOR3 m_exhaust_ref1 = {0.3, 0.0, 1.0};
	VECTOR3 m_exhaust_ref2 = {-0.25, 0.0, 1.0};

	if (!ph_jettison)
		ph_jettison = CreatePropellantResource(JettisonFuel);

	if (!ph_main && (MainFuel > 0.0))
		ph_main = CreatePropellantResource(MainFuel);

	if (!th_jettison[0])
	{
		th_jettison[0] = CreateThruster (m_exhaust_pos1, m_exhaust_ref1, THRUST_VAC_JETTISON, ph_jettison, ISP_VAC, ISP_SL);
		th_jettison[1] = CreateThruster (m_exhaust_pos2, m_exhaust_ref2, THRUST_VAC_JETTISON, ph_jettison, ISP_VAC, ISP_SL);
	}

	//
	// Add exhausts.
	//

	int i;
	for (i = 0; i < 2; i++)
	{
		AddExhaust (th_jettison[i], 5.0, 0.5, exhaust_tex);
		AddExhaustStream (th_jettison[i], &solid_exhaust);
		AddExhaustStream (th_jettison[i], &srb_exhaust);
	}

	m_exhaust_pos1= _V(0.0, -0.5, -2.2);
	m_exhaust_pos2= _V(0.0, 0.5, -2.2);
	VECTOR3 m_exhaust_pos3= _V(-0.5, 0.0, -2.2);
	VECTOR3 m_exhaust_pos4 = _V(0.5, 0.0, -2.2);

	//
	// Main thrusters. These are only used if the jettison engines
	// don't work.
	//

	th_main[0] = CreateThruster (m_exhaust_pos1, _V(0.0, 0.4, 0.7), THRUST_VAC_MAIN, ph_main, ISP_VAC, ISP_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V(0.0, -0.4, 0.7),  THRUST_VAC_MAIN, ph_main, ISP_VAC, ISP_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V(0.4, 0.0, 0.7), THRUST_VAC_MAIN, ph_main, ISP_VAC, ISP_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V(-0.4, 0.0, 0.7), THRUST_VAC_MAIN, ph_main, ISP_VAC, ISP_SL);

	//
	// Add exhausts
	//

	for (i = 0; i < 4; i++)
	{
		AddExhaust(th_main[i], 8.0, 0.5, exhaust_tex);
		AddExhaustStream(th_main[i], &solid_exhaust);
	}
}

void LES::SetMainState(int s)

{
	MainState state;

	state.word = s;

	FireMain = (state.u.FireMain != 0);
	LowRes = (state.u.LowRes != 0);
	ProbeAttached = (state.u.ProbeAttached != 0);
}

void LES::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;
	int i;

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
		else if (!strnicmp (line, "JMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			JettisonFuel = flt;
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
			THRUST_VAC_MAIN = flt;
		}
		else if (!strnicmp(line, "T2J", 3))
		{
            sscanf (line + 3, "%f", &flt);
			THRUST_VAC_JETTISON = flt;
		}
		else if (!strnicmp(line, "I2S", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_SL = flt;
		}
		else if (!strnicmp(line, "I2V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_VAC = flt;
		}
		else if (!strnicmp (line, "STATE", 5))
		{
			sscanf (line+5, "%d", &i);
			State = (LESState) i;
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}

	SetLES();
}

void LES::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	InitLES();
}

void LES::clbkDockEvent(int dock, OBJHANDLE connected)

{
}

void LES::SetState(LESSettings &state)

{
	if (state.SettingsType.LES_SETTINGS_GENERAL)
	{
		MissionTime = state.MissionTime;
		VehicleNo = state.VehicleNo;
		LowRes = state.LowRes;
		ProbeAttached = state.ProbeAttached;
	}

	if (state.SettingsType.LES_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
	}

	if (state.SettingsType.LES_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
		JettisonFuel = state.JettisonFuelKg;
	}

	if (state.SettingsType.LES_SETTINGS_MAIN_FUEL)
	{
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType.LES_SETTINGS_ENGINES)
	{
		FireMain = state.FireMain;
	}

	if (state.SettingsType.LES_SETTINGS_THRUST)
	{
		ISP_VAC = state.ISP_LET_VAC;
		ISP_SL = state.ISP_LET_SL;
		THRUST_VAC_MAIN = state.THRUST_VAC_LET;
	}

	State = LES_STATE_JETTISON;

	SetLES();
}

static int refcount = 0;

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	VESSEL *v;

	if (!refcount++) {
		LESLoadMeshes();
	}

	v = new LES (hvessel, flightmodel);
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
		delete (LES *)vessel;
}
