/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Code for simulation of Saturn hardware systems.

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

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"

#include "saturn.h"

void Saturn::SystemsTimestep(double simt)

{
	//
	// Don't clock the computer unless we're actually at the pad.
	//

	if (stage >= PRELAUNCH_STAGE) {
		dsky.Timestep(MissionTime);
		agc.Timestep(MissionTime);
	}

	//
	// MasterAlarm
	//

	if (masterAlarm && (simt > masterAlarmCycleTime)) {
		masterAlarmLit = !masterAlarmLit;
		masterAlarmCycleTime = simt + 0.25;
		if (masterAlarmLit) {
			SMasterAlarm.play(NOLOOP,255);
		}
	}

}

bool Saturn::AutopilotActive()

{
	return autopilot && CMCswitch;
}

bool Saturn::CabinFansActive()

{
	return CFswitch1 || CFswitch2;
}

//
// Forcibly activate the SIVB RCS for unmanned control.
//

void Saturn::ActivateS4RCS()

{
	RPswitch1 = true;
	RPswitch2 = true;
	RPswitch3 = true;
}

void Saturn::DeactivateS4RCS()

{
	RPswitch1 = false;
	RPswitch2 = false;
	RPswitch3 = false;
}

//
// And CSM
//

void Saturn::ActivateCSMRCS()

{
	LPswitch1 = true;
	LPswitch2 = true;
	LPswitch3 = true;

	RH11switch = false;
	RH12switch = false;
	RH13switch = false;
	RH14switch = false;
	RH21switch = false;
	RH22switch = false;
	RH23switch = false;
	RH24switch = false;

	PP1switch = true;
	PP2switch = true;
	PP3switch = true;
	PP4switch = true;
	SP1switch = true;
	SP2switch = true;
	SP3switch = true;
	SP4switch = true;
}

void Saturn::DeactivateCSMRCS()

{
	LPswitch1 = false;
	LPswitch2 = false;
	LPswitch3 = false;

	RH11switch = true;
	RH12switch = true;
	RH13switch = true;
	RH14switch = true;
	RH21switch = true;
	RH22switch = true;
	RH23switch = true;
	RH24switch = true;

	PP1switch = false;
	PP2switch = false;
	PP3switch = false;
	PP4switch = false;
	SP1switch = false;
	SP2switch = false;
	SP3switch = false;
	SP4switch = false;
}

void Saturn::ActivateSPS()

{
	SPSswitch = true;
}

void Saturn::DeactivateSPS()

{
	SPSswitch = false;
}

void Saturn::SetEngineIndicators()

{
	for (int i = 1; i <= 5; i++) {
		SetEngineIndicator(i);
	}
}

void Saturn::ClearEngineIndicators()

{
	for (int i = 1; i <= 5; i++) {
		ClearEngineIndicator(i);
	}
}

void Saturn::SetLiftoffLight()

{
	LAUNCHIND[0] = true;
}

void Saturn::ClearLiftoffLight()

{
	LAUNCHIND[0] = false;
}

void Saturn::SetAutopilotLight()

{
	AutopilotLight = true;
}

void Saturn::ClearAutopilotLight()

{
	AutopilotLight = false;
}

void Saturn::SetEngineIndicator(int i)

{
	if (i < 1 || i > 5)
		return;

	ENGIND[i - 1] = true;
}

void Saturn::ClearEngineIndicator(int i)

{
	if (i < 1 || i > 5)
		return;

	ENGIND[i - 1] = false;
}
