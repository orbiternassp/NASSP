/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: NASSP pyro simulation.

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
#include "orbitersdk.h"
#include <stdio.h>

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "pyro.h"

Pyro::Pyro(char *i_name, PanelSDK &p) : sdk(p)

{
	energy = 0.0;
	blown = false;

	strcpy(name, i_name);

	//
	// Register with the Panel SDK so it will call our timestep
	// function.
	//

	sdk.AddElectrical(this, false);
}

void Pyro::SetBlown(bool b)

{
	blown = b;
	if (!b)
	{
		energy = 0.0;
	}
}

void Pyro::UpdateFlow(double dt)

{
	if (!blown) {
		energy += ((power_load - 100.0) * dt);
		power_load = 0.0;

		//
		// For now we assume that 100 joules is enough to blow the pyro, and the
		// firing wire will cool down fast if it's deactivated.
		//

		if (energy > 100.0) {
			blown = true;
		}
		else if (energy < 0.0) {
			energy = 0.0;
		}
	}
}

void Pyro::refresh(double dt)

{
	if (!blown) {

		//
		// If it's not blown and we are powered, then suck some power out until
		// the pyro blows.
		//

		if (Voltage() > 10.0) {
			DrawPower(500.0);
		}
	}
}

void Pyro::DrawPower(double watts)

{
	power_load = watts;

	if (SRC)
		SRC->DrawPower(watts);
}

void Pyro::Load(char *line)

{
	int s_blown;
	
	sscanf (line,"    <PYRO> %s %i %lf", name, &s_blown, &energy);
	blown = (s_blown != 0);
}

void Pyro::Save(FILEHANDLE scn)

{
	char cbuf[1000];
	sprintf (cbuf, "%s %i %5.4f",name, blown ? 1 : 0, energy);
	oapiWriteScenario_string (scn, "    <PYRO> ", cbuf);
}
