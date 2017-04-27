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

	ph_tjm = 0;
	ph_lem = 0;
	ph_pcm = 0;

	thg_lem = 0;
	thg_tjm = 0;

	EmptyMass = 2023.0;
	LaunchEscapeFuel = LaunchEscapeFuelMax = 1425.138;
	JettisonFuel = JettisonFuelMax = 93.318;
	PitchControlFuel = PitchControlFuelMax = 4.07247;

	FireLEM = false;
	FireTJM = false;
	FirePCM = false;
	LowRes = false;
	ProbeAttached = false;

	ISP_TJM_SL = 1745.5837;
	ISP_TJM_VAC = 1765.197;
	THRUST_VAC_TJM = (135745.3 / 2.0)*ISP_TJM_VAC / ISP_TJM_SL;

	ISP_LEM_SL = 1725.9704;
	ISP_LEM_VAC = 1922.1034;
	THRUST_VAC_LEM = (533786.6 / 4.0)*ISP_LEM_VAC / ISP_LEM_SL;

	ISP_PCM_SL = 1931.91005;
	ISP_PCM_VAC = 1971.13665;
	THRUST_VAC_PCM = 6271.4;

	for (i = 0; i < 2; i++)
		th_tjm[i] = 0;

	for (i = 0; i < 4; i++)
		th_lem[0] = 0;
	th_pcm = 0;
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

	thg_lem = CreateThrusterGroup (th_lem, 4, THGROUP_MAIN);
	SetDefaultPropellantResource (ph_lem);
	
	thg_tjm = CreateThrusterGroup (th_tjm, 2, THGROUP_USER);
}

void LES::clbkPreStep(double simt, double simdt, double mjd)

{
	if (thg_lem)
	{
		if (FireLEM)
		{
			if (GetThrusterGroupLevel(thg_lem) < 1.0)
			{
				SetThrusterGroupLevel(thg_lem, 1.0);
			}
		}
		else
		{
			if (GetThrusterGroupLevel(thg_lem) > 0.0)
			{
				SetThrusterGroupLevel(thg_lem, 0.0);
			}
		}
	}

	if (thg_tjm)
	{
		if (FireTJM)
		{
			if (GetThrusterGroupLevel(thg_tjm) < 1.0)
			{
				SetThrusterGroupLevel(thg_tjm, 1.0);
			}
		}
		else
		{
			if (GetThrusterGroupLevel(thg_tjm) > 0.0)
			{
				SetThrusterGroupLevel(thg_tjm, 0.0);
			}
		}
	}

	if (th_pcm)
	{
		if (FirePCM)
		{
			if (GetThrusterLevel(th_pcm) < 1.0)
			{
				SetThrusterLevel(th_pcm, 1.0);
			}
		}
		else
		{
			if (GetThrusterLevel(th_pcm) > 0.0)
			{
				SetThrusterLevel(th_pcm, 0.0);
			}
		}
	}
}

void LES::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "FMASS", LaunchEscapeFuel);
	oapiWriteScenario_float (scn, "JMASS", JettisonFuel);
	oapiWriteScenario_float(scn, "PMASS", PitchControlFuel);
}

typedef union {
	struct {
		unsigned int FireLEM:1;
		unsigned int LowRes:1;
		unsigned int ProbeAttached:1;
		unsigned int FireTJM:1;
		unsigned int FirePCM:1;
	} u;
	unsigned long word;
} MainState;

int LES::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.FireLEM = FireLEM;
	state.u.LowRes = LowRes;
	state.u.ProbeAttached = ProbeAttached;
	state.u.FireTJM = FireTJM;
	state.u.FirePCM = FirePCM;

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

	if (!ph_tjm)
		ph_tjm = CreatePropellantResource(JettisonFuelMax, JettisonFuel);

	if (!ph_lem)
		ph_lem = CreatePropellantResource(LaunchEscapeFuelMax, LaunchEscapeFuel);

	if (!ph_pcm)
		ph_pcm = CreatePropellantResource(PitchControlFuelMax, PitchControlFuel);

	if (!th_tjm[0])
	{
		th_tjm[0] = CreateThruster (m_exhaust_pos1, m_exhaust_ref1, THRUST_VAC_TJM, ph_tjm, ISP_TJM_VAC, ISP_TJM_SL);
		th_tjm[1] = CreateThruster (m_exhaust_pos2, m_exhaust_ref2, THRUST_VAC_TJM, ph_tjm, ISP_TJM_VAC, ISP_TJM_SL);
	}

	//
	// Add exhausts.
	//

	int i;
	for (i = 0; i < 2; i++)
	{
		AddExhaust (th_tjm[i], 5.0, 0.5, exhaust_tex);
		AddExhaustStream (th_tjm[i], &solid_exhaust);
		AddExhaustStream (th_tjm[i], &srb_exhaust);
	}

	m_exhaust_pos1= _V(0.0, -0.5, -2.2);
	m_exhaust_pos2= _V(0.0, 0.5, -2.2);
	VECTOR3 m_exhaust_pos3= _V(-0.5, 0.0, -2.2);
	VECTOR3 m_exhaust_pos4 = _V(0.5, 0.0, -2.2);

	//
	// LEM thrusters.
	//

	th_lem[0] = CreateThruster (m_exhaust_pos1, _V(0.0, 0.4, 0.7), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);
	th_lem[1] = CreateThruster (m_exhaust_pos2, _V(0.0, -0.4, 0.7), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);
	th_lem[2] = CreateThruster (m_exhaust_pos3, _V(0.4, 0.0, 0.7), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);
	th_lem[3] = CreateThruster (m_exhaust_pos4, _V(-0.4, 0.0, 0.7), THRUST_VAC_LEM, ph_lem, ISP_LEM_VAC, ISP_LEM_SL);

	//
	// Add exhausts
	//

	for (i = 0; i < 4; i++)
	{
		AddExhaust(th_lem[i], 8.0, 0.5, exhaust_tex);
		AddExhaustStream(th_lem[i], &solid_exhaust);
	}

	m_exhaust_pos1 = _V(0.0, 0.0, 4.5);

	//Pitch Control Motor
	th_pcm = CreateThruster(m_exhaust_pos1, _V(0.0, 1.0, 0.0), THRUST_VAC_PCM, ph_pcm, ISP_PCM_VAC, ISP_PCM_SL);
	AddExhaust(th_pcm, 8.0, 0.5, exhaust_tex);
	AddExhaustStream(th_pcm, &solid_exhaust);
}

void LES::SetMainState(int s)

{
	MainState state;

	state.word = s;

	FireLEM = (state.u.FireLEM != 0);
	LowRes = (state.u.LowRes != 0);
	ProbeAttached = (state.u.ProbeAttached != 0);
	FireTJM = (state.u.FireTJM != 0);
	FirePCM = (state.u.FirePCM != 0);
}

void LES::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

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
			LaunchEscapeFuel = flt;
		}
		else if (!strnicmp (line, "JMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			JettisonFuel = flt;
		}
		else if (!strnicmp(line, "PMASS", 5))
		{
			sscanf(line + 5, "%g", &flt);
			PitchControlFuel = flt;
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
		LaunchEscapeFuel = state.LaunchEscapeFuelKg;
		JettisonFuel = state.JettisonFuelKg;
		PitchControlFuel = state.PitchControlFuelKg;
	}

	if (state.SettingsType.LES_SETTINGS_ENGINES)
	{
		FireLEM = state.FireLEM;
		FireTJM = state.FireTJM;
		FirePCM = state.FirePCM;
	}

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
