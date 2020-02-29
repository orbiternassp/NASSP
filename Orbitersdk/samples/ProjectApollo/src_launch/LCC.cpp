/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  ORBITER vessel module: Launch Control Center

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

#include "RCA110A.h"
#include "LCC.h"
#include "LCC_MFD.h"
#include "ML.h"
#include "LC34.h"
#include "LC37.h"

#define ORBITER_MODULE

LCC::LCC(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel)
{
	pPad = NULL;

	sprintf_s(PadName, 256, "");

	static char *name = "LCC";
	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_T;
	spec.context = NULL;
	spec.msgproc = LCC_MFD::MsgProc;
	mfdID = RegisterMFDMode(spec);
	rca110a = new RCA110AL(this);
}

LCC::~LCC()
{
	UnregisterMFDMode(mfdID);
	delete rca110a;
}

void LCC::clbkPostCreation()
{
	// get pointer to LCCPadInterface
	OBJHANDLE hPad = oapiGetVesselByName(PadName);
	if (hPad != NULL)
	{
		VESSEL* pVessel = oapiGetVesselInterface(hPad);
		if (pVessel != NULL)
		{
			if (!_stricmp(pVessel->GetClassName(), "ProjectApollo\\ML")) pPad = static_cast<ML*>(pVessel);
			else if (!_stricmp(pVessel->GetClassName(), "ProjectApollo/ML")) pPad = static_cast<ML*>(pVessel);
			//else if (!_strnicmp(pVessel->GetClassName(), "LC34", 8)) pPad = static_cast<LC34*>(pVessel);

			if (pPad)
			{
				pPad->ConnectGroundComputer(rca110a);
			}
		}
	}
	if (pPad == NULL) oapiWriteLog("(LCC) ERROR: Failed to get pointer to LCCPadInterface");
	return;
}

void LCC::clbkPreStep(double simt, double simdt, double mjd)
{
	rca110a->Timestep(simt, simdt);
}

void LCC::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	oapiWriteScenario_string(scn, "PAD_NAME", PadName);
}

void LCC::clbkLoadStateEx(FILEHANDLE scn, void *status)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!_strnicmp(line, "PAD_NAME", 8)) {
			sscanf_s(line + 8, "%s", PadName, sizeof(PadName));
		}
		else ParseScenarioLineEx(line, status);
	}
}

void LCC::SetDiscreteInput(size_t n, bool set)
{
	relayrack.set(n, set);
}

void LCC::SLCCCheckDiscreteInput(RCA110A *c)
{
	for (size_t i = 0;i < RCA110A_INPUT_LINES;i++)
	{
		c->SetInput(i, relayrack.test(i));
	}
}

DLLCLBK void InitModule(HINSTANCE hDLL)
{
}

DLLCLBK void ExitModule(HINSTANCE hDLL)
{
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hVessel, int iFlightModel)
{
	return new LCC(hVessel, iFlightModel);
}

DLLCLBK void ovcExit(VESSEL* pVessel)
{
	delete static_cast<LCC*>(pVessel);
}