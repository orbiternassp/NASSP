/****************************************************************************
  This file is part of Project Apollo - NASSP

  Mission Control Center Vessel (Header)

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

#pragma once

#include "Orbitersdk.h"

class MCC;
class RTCC;

class MCCVessel : public VESSEL4 {
public:
	MCCVessel(OBJHANDLE hVessel, int flightmodel);
	~MCCVessel();

	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostCreation();
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);

	MCC *mcc;
	RTCC *rtcc;
protected:
	void CreateMCC();

	char CSMName[64];
	char LEMName[64];
	char LVName[64];
};