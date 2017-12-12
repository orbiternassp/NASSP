/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Reaction Control System

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
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "LEM.h"
#include "papi.h"
#include "lm_rcs.h"

LEMRCSValve::LEMRCSValve() {
	isOpen = false;
}

void LEMRCSValve::SetState(bool open) {
	isOpen = open;
}

void LEMRCSValve::SwitchToggled(PanelSwitchItem *s) {

	if (s->SRC && (s->SRC->Voltage() > SP_MIN_DCVOLTAGE)) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			SetState(false);
		}
	}
}

RCSPropellantSource::RCSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p) : LEMPropellantSource(ph)
{
	heliumPressurePSI = 0.0;
	regulatorPressurePSI = 0.0;
	oxidManifoldPressurePSI = 0.0;
	fuelManifoldPressurePSI = 0.0;

	RCSHeliumSupplyPyros = NULL;
}

void RCSPropellantSource::Init(Pyro *rcshsp)
{
	RCSHeliumSupplyPyros = rcshsp;
}

void RCSPropellantSource::Timestep(double simt, double simdt)
{
	if (!our_vessel) return;

	if (!source_prop)
	{
		heliumPressurePSI = 0.0;
		regulatorPressurePSI = 0.0;
		oxidManifoldPressurePSI = 0.0;
		fuelManifoldPressurePSI = 0.0;
	}
	else
	{
		double p = our_vessel->GetPropellantMass(source_prop);

		heliumPressurePSI = 3050.0;

		//Regulator
		if (HeliumSupplyValve.IsOpen())
		{
			if (heliumPressurePSI > 179.0)
			{
				regulatorPressurePSI = 179.0;
			}
			else
			{
				regulatorPressurePSI = heliumPressurePSI;
			}
		}
		else
		{
			regulatorPressurePSI = 0.0;
		}

		//Manifold
		if (MainShutoffValve.IsOpen())
		{
			oxidManifoldPressurePSI = regulatorPressurePSI - 9.0;
			fuelManifoldPressurePSI = regulatorPressurePSI - 9.0;
		}
		else
		{
			oxidManifoldPressurePSI = 0.0;
			fuelManifoldPressurePSI = 0.0;
		}

		//Explosive valves
		if (!HeliumSupplyValve.IsOpen() && RCSHeliumSupplyPyros->Blown())
		{
			HeliumSupplyValve.SetState(true);
		}
	}
}

double RCSPropellantSource::GetRCSHeliumPressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return heliumPressurePSI;

	return 0.0;
}

double RCSPropellantSource::GetRCSRegulatorPressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return regulatorPressurePSI;

	return 0.0;
}

double RCSPropellantSource::GetRCSFuelManifoldPressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return fuelManifoldPressurePSI;

	return 0.0;
}

double RCSPropellantSource::GetRCSOxidManifoldPressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return oxidManifoldPressurePSI;

	return 0.0;
}