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
  *	Revision 1.3  2005/08/13 23:48:57  movieman523
  *	Added some documentation on caution and warning checks that I found on the web.
  *	
  *	Revision 1.2  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
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

		//
		// LOX/LH2: "The caution and warning system will activate on alarm when oxygen pressure 
		// in either tank exceeds 950 psia or falls below 800 psia or when the hydrogen system 
		// pressure exceeds 270 psia or drops below 220 psia."
		//

		//
		// Inverter: "A temperature sensor with a range of 32 degrees to 248 degrees F is installed 
		// in each inverter and will illuminate a light in the caution and warning system at an 
		// inverter overtemperature of 190 degrees F"
		//

		//
		// Power Bus: "If voltage drops below 26.25 volts dc, the applicable dc undervoltage light 
		// on the caution and warning panel will illuminate."
		//

		//
		// Oxygen flow: "Flow rates of 1 pound per hour or more with a duration in excess of 16.5 
		// seconds will illuminate a light on the caution and warning panel to alert the crew to 
		// the fact that the oxygen flow rate is greater than is normally required."
		//

		//
		// CO2: "A carbon dioxide sensor is connected between the suit inlet and return manifold. It 
		// is connected to an indicator on the main display console, to telemetry, and to the caution 
		// and warning system and will activate a warning if the carbon dioxide partial pressure 
		// reaches 7.6 millimeters of mercury."
		//

		NextUpdateTime = simt + (0.2 * oapiGetTimeAcceleration());
	}
}

void CSMCautionWarningSystem::RenderLights(SURFHANDLE surf, SURFHANDLE lightsurf, bool leftpanel)

{
	if (leftpanel) {
		RenderLightPanel(surf, lightsurf, LeftLights, TestState == CWS_TEST_LIGHTS_LEFT, 6, 122);
	}
	else {
		RenderLightPanel(surf, lightsurf, RightLights, TestState == CWS_TEST_LIGHTS_RIGHT, 261, 122);
	}
}

void CSMCautionWarningSystem::RenderLightPanel(SURFHANDLE surf, SURFHANDLE lightsurf, bool *LightState, bool LightTest, int sdx, int sdy)

{
	int i = 0;
	int row, column;

	if (!IsPowered())
		return;

	for (row = 0; row < 6; row++) {
		for (column = 0; column < 4; column++) {
			if (LightTest || LightState[i]) {
				oapiBlt(surf, lightsurf, column * 53, row * 18, column * 53 + sdx, row * 18 + sdy, 50, 16);
			}
			i++;
		}
	}
}
