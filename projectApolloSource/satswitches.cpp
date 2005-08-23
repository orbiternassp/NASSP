/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn-specific switches

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
  *	Revision 1.1  2005/08/16 20:55:23  movieman523
  *	Added first saturn-specific switch for Xlunar Inject.
  *	
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
#include "IMU.h"

#include "saturn.h"


void SaturnToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, Saturn *s)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, row);
	sat = s;
}

bool XLunarSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my)) {
		if (sat) {
			if (IsUp()) {
				sat->EnableTLI();
			}
			else if (IsDown()) {
				sat->DisableTLI();
			}
		}
		return true;
	}

	return false;
}

void SaturnThreePosSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, Saturn *s)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, row);
	sat = s;
}

void SaturnValveSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, Saturn *s, int valve, IndicatorSwitch *ind)

{
	SaturnThreePosSwitch::Init(xp, yp, w, h, surf, row, s);

	Valve = valve;
	Indicator = ind;
}

bool SaturnValveSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (SaturnThreePosSwitch::CheckMouseClick(event, mx, my)) {
		if (sat) {
			if (IsUp()) {
				sat->SetValveState(Valve, true);
				if (Indicator)
					*Indicator = true;
			}
			else if (IsDown()) {
				sat->SetValveState(Valve, false);
				if (Indicator)
					*Indicator = false;
			}
		}
		return true;
	}

	return false;
}
