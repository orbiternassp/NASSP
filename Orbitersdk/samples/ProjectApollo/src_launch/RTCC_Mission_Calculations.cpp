/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

RTCC Mission Calculations

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

#include "Orbitersdk.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "mcc.h"
#include "rtcc.h"

bool RTCC::Calculation(int mission, int fcn, LPVOID &pad, char * upString, char * upDesc, char * upMessage)
{
	bool scrubbed = false;

	switch (mission)
	{
	case MTP_B:
		scrubbed = CalculationMTP_B(fcn, pad, upString, upDesc, upMessage);
		break;
	case MTP_C:
		scrubbed = CalculationMTP_C(fcn, pad, upString, upDesc, upMessage);
		break;
	case MTP_C_PRIME:
		scrubbed = CalculationMTP_C_PRIME(fcn, pad, upString, upDesc, upMessage);
		break;
	case MTP_D:
		scrubbed = CalculationMTP_D(fcn, pad, upString, upDesc, upMessage);
		break;
	case MTP_F:
		scrubbed = CalculationMTP_F(fcn, pad, upString, upDesc, upMessage);
		break;
	}

	return scrubbed;
}