/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2025

  ORBITER vessel module: Saturn V Interstage

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

#include "S1C_S2_IntStage.h"

#define ORBITER_MODULE

S1C_S2_Interstage::S1C_S2_Interstage(OBJHANDLE hObj, int fmodel): ProjectApolloConnectorVessel(hObj, fmodel)
{
	SIIDock = nullptr;
	SICDock = nullptr;
}

S1C_S2_Interstage::~S1C_S2_Interstage() {

}

void S1C_S2_Interstage::clbkSetClassCaps(FILEHANDLE cfg)
{
}

void S1C_S2_Interstage::clbkPostCreation()
{
}

void S1C_S2_Interstage::clbkPreStep(double simt, double simdt, double mjd)
{
}

void S1C_S2_Interstage::clbkSaveState(FILEHANDLE scn)
{
}

void S1C_S2_Interstage::clbkLoadStateEx(FILEHANDLE scn, void* vstatus)
{
}

void S1C_S2_Interstage::clbkGetRadiationForce(const VECTOR3& mflux, VECTOR3& F, VECTOR3& pos)
{
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	return new S1C_S2_Interstage(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL* vessel)
{
	delete (S1C_S2_Interstage*)vessel;
}