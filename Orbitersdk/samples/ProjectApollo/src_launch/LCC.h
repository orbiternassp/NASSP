/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  ORBITER vessel module: Launch Control Center (Header)

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

#include <bitset>
#include "Orbitersdk.h"
#include "PadLCCInterface.h"
#include "LCCPadInterface.h"

class RCA110AL;

class LCC : public VESSEL4, public PadLCCInterface
{
public:
	LCC(OBJHANDLE hVessel, int flightmodel);
	~LCC();

	void clbkPostCreation();
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);

	void SetDiscreteInput(size_t n, bool set);
	void SLCCLoadProgram(const char *str);
private:
	char PadName[256];
	int mfdID;

	LCCPadInterface* pPad;

	// Pad/LCC Interface
	void SLCCCheckDiscreteInput(RCA110A *c);

	RCA110AL *rca110a;

	std::bitset<3024> relayrack;
};