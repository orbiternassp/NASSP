/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: CSM caution and warning system code.

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
  *	Revision 1.1  2005/08/13 14:59:24  movieman523
  *	Added initial null implementation of CSM caution and warning system, and removed 'master alarm' flag from Saturn class.
  *	
  **************************************************************************/


#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

#include "soundlib.h"
#include "nasspsound.h"

#include "ioChannels.h"

#include "cautionwarning.h"
#include "csmcautionwarning.h"
#include "nasspdefs.h"

CSMCautionWarningSystem::CSMCautionWarningSystem(Sound &s) : CautionWarningSystem(s)

{
	NextUpdateTime = MINUS_INFINITY;
}

//
// We'll only check the internal systems five times a second (x time acceleration). That should cut the overhead to
// a minimum.
//

void CSMCautionWarningSystem::TimeStep(double simt)

{
	CautionWarningSystem::TimeStep(simt);

	if (simt > NextUpdateTime) {

		//
		// Check systems.
		//

		NextUpdateTime = simt + (0.2 * oapiGetTimeAcceleration());
	}
}
