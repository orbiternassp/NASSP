/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2026


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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/NASSP-LICENSE.txt
  for more details.

 **************************************************************************/
#define ORBITER_MODULE
#include "vhfdummy.h"


VHFDummy::VHFDummy(OBJHANDLE hObj, int fmodel) : ProjectApolloConnectorVessel(hObj, fmodel)
{
	csm = NULL;
}

VHFDummy::~VHFDummy() {

}

void VHFDummy::InitVHFDummy() {
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &vhfdummy_vhf2csm_vhf_connector);
}

void VHFDummy::clbkPostCreation() {
	InitVHFDummy();
}

void VHFDummy::clbkPreStep(double simt, double simdt, double mjd)
{
	//Communications

	if (vhfdummy_vhf2csm_vhf_connector.connectedTo)
	{
		vhfdummy_vhf2csm_vhf_connector.SendRF(296.8E6, 5, 10, 0, true);
	}
	else
	{
		vhfdummy_vhf2csm_vhf_connector.ConnectTo(GetVesselConnector(csm, VIRTUAL_CONNECTOR_PORT, VHF_RNG));
	}
}

void VHFDummy::clbkSetClassCaps(FILEHANDLE cfg)
{
}

void VHFDummy::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	if (csm) oapiWriteScenario_string(scn, "ONAME", csm->GetName());
}

void VHFDummy::clbkLoadStateEx(FILEHANDLE scn, void* vstatus)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!strnicmp(line, "ONAME", 5))
		{
			char temp[64];
			strncpy(temp, line + 6, 64);

			OBJHANDLE hVessel = oapiGetVesselByName(temp);
			if (hVessel != NULL) csm = oapiGetVesselInterface(hVessel);
		}
		else
		{
			ParseScenarioLineEx(line, vstatus);
		}
	}
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	VESSEL* vhfdummy;
	vhfdummy = new VHFDummy(hvessel, flightmodel);

	return vhfdummy;
}


DLLCLBK void ovcExit(VESSEL* vessel)
{
	if (vessel) {
		delete (VHFDummy*)vessel;
	}
}
