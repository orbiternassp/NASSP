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

#if !defined(_PA_S1C_S2INTSTAGE_H)
#define _PA_S1C_S2INTSTAGE_H


#include "Orbitersdk.h"
#include "PanelSDK/PanelSDK.h"
#include "nasspdefs.h"
#include "connector.h" //replace with interstage connector header.


class S1C_S2_Interstage : public ProjectApolloConnectorVessel {
public:
	S1C_S2_Interstage(OBJHANDLE hObj, int fmodel);
	virtual ~S1C_S2_Interstage();
	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPostCreation();
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkSaveState(FILEHANDLE scn);
	//void clbkLoadStateEx(FILEHANDLE scn, void* vstatus);
	//void clbkGetRadiationForce(const VECTOR3& mflux, VECTOR3& F, VECTOR3& pos);

protected:

	void SetupTouchdownPoints();

	DOCKHANDLE hSIIDock, hSICDock;
	PROPELLANT_HANDLE ULLAGE_PROP[8];
	THRUSTER_HANDLE ULLAGE_MOTORS[8];
	MESHHANDLE Sat1C_Sat2InterstageMesh;
};

#endif
