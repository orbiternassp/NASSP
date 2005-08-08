/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: DSKY interface code for AGC

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
  *	$Log$
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"

void ApolloGuidance::LightUplink()

{
	SetOutputChannelBit(011, 3, true);
}

void ApolloGuidance::ClearUplink()

{
	SetOutputChannelBit(011, 3, false);
}

void ApolloGuidance::LightCompActy()

{
	SetOutputChannelBit(011, 2, true);
}

void ApolloGuidance::ClearCompActy()

{
	SetOutputChannelBit(011, 2, false);
}

void ApolloGuidance::LightTemp()

{
	SetOutputChannelBit(011, 4, true);
}

void ApolloGuidance::ClearTemp()

{
	SetOutputChannelBit(011, 4, false);
}

void ApolloGuidance::LightKbRel()

{
	SetOutputChannelBit(011, 5, true);
}

void ApolloGuidance::ClearKbRel()

{
	SetOutputChannelBit(011, 5, false);
}

void ApolloGuidance::SetVerbNounFlashing()

{
	SetOutputChannelBit(011, 6, true);
}

void ApolloGuidance::ClearVerbNounFlashing()

{
	SetOutputChannelBit(011, 6, false);
}

void ApolloGuidance::LightOprErr()

{
	SetOutputChannelBit(011, 7, true);
}

void ApolloGuidance::ClearOprErr()

{
	SetOutputChannelBit(011, 7, false);
}

void ApolloGuidance::ProcessChannel11(int bit, bool val)

{
	//
	// Channel 011 has bits to control the lights on the DSKY.
	//

	switch (bit) {

	// 2 - Comp Acty
	case 2:
		if (val)
			dsky.LightCompActyLight();
		else
			dsky.ClearCompActyLight();
		break;

	// 3 - Uplink
	case 3:
		if (val)
			dsky.LightUplinkLight();
		else
			dsky.ClearUplinkLight();
		break;

	// 4 - Temp caution
	case 4:
		if (val)
			dsky.LightTempLight();
		else
			dsky.ClearTempLight();
		break;

	// 5 - Kbd Rel
	case 5:
		if (val)
			dsky.LightKbRelLight();
		else
			dsky.ClearKbRelLight();
		break;

	// 6 - flash verb and noun
	case 6:
		if (val) {
			dsky.SetVerbDisplayFlashing();
			dsky.SetNounDisplayFlashing();
		}
		else {
			dsky.ClearVerbDisplayFlashing();
			dsky.ClearNounDisplayFlashing();
		}
		break;

	// 7 - Opr Err
	case 7:
		if (val)
			dsky.LightOprErrLight();
		else
			dsky.ClearOprErrLight();
		break;
	}
}

