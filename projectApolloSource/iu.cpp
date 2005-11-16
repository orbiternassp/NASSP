/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Saturn Instrument Unit header

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

  **************************** Revision History ****************************
  *	
  **************************************************************************/

#include <stdio.h>
#include "orbitersdk.h"

#include "iu.h"

IU::IU()

{
	TLIBurnTime = 0.0;
	TLIBurnDeltaV = 0.0;
	isTLICapable = false;
}

IU::~IU()

{
	// Nothing for now.
}

void IU::Timestep(double simt, double simdt)

{
}

typedef union

{
	struct {
		unsigned isTLICapable:1;
	} u;
	unsigned long word;
} IUState;

void IU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IU_START_STRING);

	oapiWriteScenario_float(scn, "TLITIME", TLIBurnTime);
	oapiWriteScenario_float(scn, "TLIDV", TLIBurnDeltaV);

	//
	// Copy internal state to the structure.
	//

	IUState state;

	state.word = 0;
	state.u.isTLICapable = isTLICapable;

	oapiWriteScenario_int (scn, "STATE", state.word);

	oapiWriteLine(scn, IU_END_STRING);
}


void IU::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IU_END_STRING, sizeof(IU_END_STRING)))
			return;
		if (!strnicmp (line, "TLITIME", 7)) {
			sscanf(line + 7, "%f", &flt);
			TLIBurnTime = flt;
		}
		else if (!strnicmp (line, "TLIDV", 5)) {
			sscanf(line + 5, "%f", &flt);
			TLIBurnDeltaV = flt;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			IUState state;
			sscanf (line+5, "%d", &state.word);

			isTLICapable = (state.u.isTLICapable != 0);
		}
	}
}
