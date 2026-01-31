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

  **************************************************************************/

// To force Orbitersdk.h to use <fstream> in any compiler version

#define ORBITER_MODULE

#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "connector.h"
#include "csmconnector.h"
#include "lemconnector.h"

#include "nasspsound.h"
#include "nasspdefs.h"
#include "soundlib.h"

#include "toggleswitch.h"
#include "saturn.h"
#include "LEM.h"

#include "eva.h"
#include "tracer.h"

char trace_file[] = "ProjectApollo CMP_EVA.log";

//Vessel Consumables
const double MAIN_ISP = 500;
const double RCS_TH = 4;
const double FUEL_MASS = 30;
const double EMP_MASS = 115;

double stiffness = 1e3;
double damping = 2e2;
static TOUCHDOWNVTX tdvtx_geardown[3] = {
	{_V(0,      -1,  0.5), stiffness, damping, 1.0, 0.2},
	{_V(-0.433, -1, -0.25), stiffness, damping, 1.0, 0.2},
	{_V(0.433, -1, -0.25), stiffness, damping, 1.0, 0.2}
};

EVA::EVA(OBJHANDLE hVessel, int flightmodel)
	: VESSEL3(hVessel, flightmodel)
{
	init();
}

EVA::~EVA()
{
}

void EVA::init()

{
	hCSM = NULL;
	hLEM = NULL;
	GoDockCSM = false;
	GoDockLEM = false;
	FirstTimestep = true;
	StateSetCMP = false;
	StateSetLMP = false;
	ApolloNo = 0;
	Astro = true;
	isCMP = false;
	isLMP = false;
	CSMMotherShip = false;
	LEMMotherShip = false;
	CSMName[0] = 0;
	LEMName[0] = 0;

}

void EVA::SetEVAStatsCMP(EVASettingsCMP& evascmp)

{
	ApolloNo = evascmp.MissionNo;
	isCMP = evascmp.isCMP;
	strcpy(CSMName, evascmp.CSMName);
	StateSetCMP = true;
}

void EVA::SetEVAStatsLMP(EVASettingsLMP& evaslmp)

{
	ApolloNo = evaslmp.MissionNo;
	isLMP = evaslmp.isLMP;
	strcpy(LEMName, evaslmp.LEMName);
	StateSetLMP = true;
}

void EVA::DoFirstTimestep()
{
	if (StateSetCMP || StateSetLMP)
	{

		VECTOR3 mesh_dir = _V(0, 0, 0);

		if (ApolloNo == 9)
		{
			if (isLMP)
			{
				AddMesh("ProjectApollo/LM-LMPEVA-9", &mesh_dir);
			}
			else if (isCMP)
			{
				AddMesh("ProjectApollo/CM-CMPEVA-9", &mesh_dir);
			}
		}
		else
		{
			AddMesh("ProjectApollo/CM-CMPEVA", &mesh_dir);
		}

		FirstTimestep = false;
	}
}

typedef union {
	struct {
		unsigned int StateSetCMP : 1;
		unsigned int StateSetLMP : 1;
		unsigned int isCMP : 1;
		unsigned int isLMP : 1;
	} u;
	unsigned int word;
} MainEVAState;

int EVA::GetMainState()

{
	MainEVAState s;

	s.word = 0;
	s.u.StateSetCMP = StateSetCMP;
	s.u.StateSetLMP = StateSetLMP;
	s.u.isCMP = isCMP;
	s.u.isLMP = isLMP;

	return s.word;
}

void EVA::SetMainState(int n)

{
	MainEVAState s;

	s.word = n;
	StateSetCMP = (s.u.StateSetCMP != 0);
	StateSetLMP = (s.u.StateSetLMP != 0);
	isCMP = (s.u.isCMP != 0);
	isLMP = (s.u.isLMP != 0);
}

void EVA::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetAstroStage();
}

void EVA::SetAstroStage()
{
	// physical specs
	SetCW(1, 1, 1, 1);
	SetRotDrag(_V(1, 1, 1));
	SetPMI(_V(0.2, 0.2, 0.2));
	SetCrossSections(_V(0.5, 0.3, 0.7));
	SetSize(1);
	SetEmptyMass(EMP_MASS);
	ClearMeshes();
	SetCameraOffset(_V(0, 1, 0));
	SetTouchdownPoints(tdvtx_geardown, 3);

	hProp = CreatePropellantResource(FUEL_MASS, FUEL_MASS);

	// ***************** thruster definitions *******************

	th_rcs[0] = CreateThruster(_V(0, 0.15, 0), _V(0, 0, 1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[1] = CreateThruster(_V(0, 0.15, 0), _V(0, 0, -1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[2] = CreateThruster(_V(0, 0.15, 0), _V(1, 0, 0), RCS_TH, hProp, MAIN_ISP);
	th_rcs[3] = CreateThruster(_V(0, 0.15, 0), _V(-1, 0, 0), RCS_TH, hProp, MAIN_ISP);

	th_rcs[4] = CreateThruster(_V(0, -0.15, 0), _V(0, 0, 1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[5] = CreateThruster(_V(0, -0.15, 0), _V(0, 0, -1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[6] = CreateThruster(_V(0, -0.15, 0), _V(1, 0, 0), RCS_TH, hProp, MAIN_ISP);
	th_rcs[7] = CreateThruster(_V(0, -0.15, 0), _V(-1, 0, 0), RCS_TH, hProp, MAIN_ISP);

	th_rcs[8] = CreateThruster(_V(0.15, 0, 0), _V(0, 0, 1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[9] = CreateThruster(_V(0.15, 0, 0), _V(0, 0, -1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[10] = CreateThruster(_V(0.15, 0, 0), _V(0, 1, 0), RCS_TH, hProp, MAIN_ISP);
	th_rcs[11] = CreateThruster(_V(0.15, 0, 0), _V(0, -1, 0), RCS_TH, hProp, MAIN_ISP);

	th_rcs[12] = CreateThruster(_V(-0.15, 0, 0), _V(0, 0, 1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[13] = CreateThruster(_V(-0.15, 0, 0), _V(0, 0, -1), RCS_TH, hProp, MAIN_ISP);
	th_rcs[14] = CreateThruster(_V(-0.15, 0, 0), _V(0, 1, 0), RCS_TH, hProp, MAIN_ISP);
	th_rcs[15] = CreateThruster(_V(-0.15, 0, 0), _V(0, -1, 0), RCS_TH, hProp, MAIN_ISP);


	th_group[0] = th_rcs[1];
	th_group[1] = th_rcs[4];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_PITCHUP);

	th_group[0] = th_rcs[0];
	th_group[1] = th_rcs[5];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_PITCHDOWN);

	th_group[0] = th_rcs[3];
	th_group[1] = th_rcs[6];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_BANKLEFT);

	th_group[0] = th_rcs[2];
	th_group[1] = th_rcs[7];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_BANKRIGHT);

	th_group[0] = th_rcs[8];
	th_group[1] = th_rcs[13];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_YAWLEFT);

	th_group[0] = th_rcs[9];
	th_group[1] = th_rcs[12];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_YAWRIGHT);

	th_group[0] = th_rcs[0];
	th_group[1] = th_rcs[4];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_FORWARD);

	th_group[0] = th_rcs[9];
	th_group[1] = th_rcs[13];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_BACK);

	th_group[0] = th_rcs[10];
	th_group[1] = th_rcs[14];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_UP);

	th_group[0] = th_rcs[11];
	th_group[1] = th_rcs[15];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_DOWN);

	th_group[0] = th_rcs[3];
	th_group[1] = th_rcs[7];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_LEFT);

	th_group[0] = th_rcs[2];
	th_group[1] = th_rcs[6];
	CreateThrusterGroup(th_group, 2, THGROUP_ATT_RIGHT);

	Astro = true;
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	return new EVA(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL* vessel)
{
	if (vessel) delete (EVA*)vessel;
}

void EVA::clbkPostCreation()
{
}

void EVA::GetCSM()
{
	double VessCount;
	int i = 0;

	VessCount = oapiGetVesselCount();
	for (i = 0; i < VessCount; i++)
	{
		char vesselName[256] = "";
		hCSM = oapiGetVesselByIndex(i);
		oapiGetObjectName(hCSM, vesselName, 256);

		if (strcmp(CSMName, vesselName) == 0) {
			CSMMotherShip = true;
			i = int(VessCount);
		}
	}
}

void EVA::GetLEM()
{
	double VessCount;
	int i = 0;

	VessCount = oapiGetVesselCount();
	for (i = 0; i < VessCount; i++)
	{
		char vesselName[256] = "";
		hLEM = oapiGetVesselByIndex(i);
		oapiGetObjectName(hLEM, vesselName, 256);

		if (stricmp(LEMName, vesselName) == 0) {
			LEMMotherShip = true;
			i = int(VessCount);
		}
	}
}

int EVA::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
{
	if (!down)
		return 0;

	if (KEYMOD_SHIFT(kstate))
	{
		return 0;
	}

	else if (KEYMOD_CONTROL(kstate))
	{
		return 0;
	}

	if (KEYMOD_ALT(kstate))
	{
		return 0;
	}

	if (key == OAPI_KEY_E && down == true) {
		if (Astro) 
		{
			if (isCMP) {
				GoDockCSM = true;
			}
			if (isLMP) {
				GoDockLEM = true;
			}
		}
		return 1;
	}

	return 0;
}

void EVA::clbkPreStep(double SimT, double SimDT, double MJD)
{
	if (FirstTimestep)
	{
		DoFirstTimestep();
		return;
	}

	if (!CSMMotherShip && isCMP) GetCSM();
	if (!LEMMotherShip && isLMP) GetLEM();

	// Finish EVA CSM
	if (CSMMotherShip && hCSM && GoDockCSM)
	{
		Saturn* csmvessel = (Saturn*)oapiGetVesselInterface(hCSM);
		if (csmvessel)
		csmvessel->StopEVA();
		GoDockCSM = false;
	}

	// Finish EVA LM
	if (LEMMotherShip && hLEM && GoDockLEM)
	{
		LEM* lemvessel = (LEM*)oapiGetVesselInterface(hLEM);
		if (lemvessel)
		lemvessel->StopSpaceEVA();
		GoDockLEM = false;
	}
}

void EVA::clbkLoadStateEx(FILEHANDLE scn, void* vs)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line)) 
	{
		if (!strnicmp(line, "CSMNAME", 7)) {
			sscanf(line + 7, "%s", &CSMName);
		}
		else if (!strnicmp(line, "LEMNAME", 7)) {
			sscanf(line + 7, "%s", &LEMName);
		}
		else if (!strnicmp(line, "MISSIONNO", 9)) {
			sscanf(line + 9, "%d", &ApolloNo);
	    }
		else if (!strnicmp(line, "STATE", 5)) {
			int	s;
			sscanf(line + 5, "%d", &s);
			SetMainState(s);
		}
		else {
			ParseScenarioLineEx(line, vs);
		}
	}
}

void EVA::clbkSaveState(FILEHANDLE scn)
{
	VESSEL3::clbkSaveState(scn);

	int s = GetMainState();
	if (s) {
		oapiWriteScenario_int(scn, "STATE", s);
	}

	oapiWriteScenario_string(scn, "CSMNAME", CSMName);

	oapiWriteScenario_string(scn, "LEMNAME", LEMName);

	if (ApolloNo != 0) {
		oapiWriteScenario_int(scn, "MISSIONNO", ApolloNo);
	}
}