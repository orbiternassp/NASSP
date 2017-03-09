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

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "ioChannels.h"

extern char RegFormat[];

//
// Send special commands to set the Channel 10 bits indicating that a light state has
// changed.
//

void ApolloGuidance::ProcessChannel13(ChannelValue val)

{
	OutputChannel[013] = val.to_ulong();
	dsky.ProcessChannel13(val);
	if (dsky2) dsky2->ProcessChannel13(val);
}

//
// By default, just pass these calls through to the dsky.
//

void ApolloGuidance::ProcessChannel10(ChannelValue val)

{
	dsky.ProcessChannel10(val.to_ulong());
	if (dsky2) dsky2->ProcessChannel10(val.to_ulong());
}

void ApolloGuidance::ProcessChannel11Bit(int bit, bool val)

{
	dsky.ProcessChannel11Bit(bit, val);
	if (dsky2) dsky2->ProcessChannel11Bit(bit, val);
}

void ApolloGuidance::ProcessChannel11(ChannelValue val)

{
	dsky.ProcessChannel11(val);
	if (dsky2) dsky2->ProcessChannel11(val);
}

