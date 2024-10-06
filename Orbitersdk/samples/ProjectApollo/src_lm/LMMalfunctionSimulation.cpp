/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2024

LM Malfunction Simulation

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

#include "LMMalfunctionSimulation.h"
#include "Malfunction.h"
#include "Orbitersdk.h"
#include "soundlib.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "LEM.h"

LMMalfunctionSimulation::LMMalfunctionSimulation(LEM *l) : lem(l)
{

}

double LMMalfunctionSimulation::GetTimeReference(int i)
{
	if (i == 1) //1 = simulation time
	{
		return oapiGetSimTime();
	}
	//0 = mission time
	return lem->GetMissionTime();
}

void LMMalfunctionSimulation::SetFailure(unsigned i)
{
	//TBD
}

void LMMalfunctionSimulation::ResetFailure(unsigned i)
{
	//TBD
}

void LMMalfunctionSimulation::SetSwitchFailure(unsigned i, bool set)
{
	if (i >= switchmalfunctions.size()) return;

	lem->MainPanel.SetFailedState(switchmalfunctions[i]->GetSwitchName().c_str(), set, switchmalfunctions[i]->GetFailState());
}

int LMMalfunctionSimulation::GetDamageModel()
{
	return lem->GetDamageModel();
}