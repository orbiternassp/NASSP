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

#if !defined(_PA_VHFDUMMY_H)
#define _PA_VHFDUMMY_H

#include "Orbitersdk.h"
#include "PanelSDK/PanelSDK.h"
#include "VHFDummyConnector.h"
#include "nasspdefs.h"


class VHFDummy : public ProjectApolloConnectorVessel {
public:
	VHFDummy(OBJHANDLE hObj, int fmodel);
	virtual ~VHFDummy();
	void InitVHFDummy();
	void clbkPostCreation();
	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void* vstatus);

private:
	VHFDummy_VHFtoCSM_VHF_Connector vhfdummy_vhf2csm_vhf_connector;
	VESSEL* csm; //Pointer to CSM for various purposes
};


#endif
