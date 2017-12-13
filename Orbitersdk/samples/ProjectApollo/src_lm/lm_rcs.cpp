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
	oxidTankPressurePSI = 0.0;
	fuelTankPressurePSI = 0.0;

	RCSHeliumSupplyPyros = NULL;

	//Open at launch
	primOxidInterconnectValve.SetState(true);
	primFuelInterconnectValve.SetState(true);
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
		oxidTankPressurePSI = 0.0;
		fuelTankPressurePSI = 0.0;
	}
	else
	{
		double p = our_vessel->GetPropellantMass(source_prop);

		heliumPressurePSI = 3050.0;

		//Regulator
		if (heliumSupplyValve.IsOpen())
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
		if (mainShutoffValve.IsOpen())
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
		if (!heliumSupplyValve.IsOpen() && RCSHeliumSupplyPyros->Blown())
		{
			heliumSupplyValve.SetState(true);
		}
	}
}

void RCSPropellantSource::PrimInterconnectToggled(PanelSwitchItem *s) {

	if (s->SRC && (s->SRC->Voltage() > SP_MIN_DCVOLTAGE)) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			primOxidInterconnectValve.SetState(true);
			primFuelInterconnectValve.SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			primOxidInterconnectValve.SetState(false);
			primFuelInterconnectValve.SetState(false);
		}
	}
}

void RCSPropellantSource::SecInterconnectToggled(PanelSwitchItem *s) {

	if (s->SRC && (s->SRC->Voltage() > SP_MIN_DCVOLTAGE)) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			secOxidInterconnectValve.SetState(true);
			secFuelInterconnectValve.SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			secOxidInterconnectValve.SetState(false);
			secFuelInterconnectValve.SetState(false);
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

double RCSPropellantSource::GetRCSPropellantQuantity()
{
	if (our_vessel->RCS_B_PQGS_DISP_CB.IsPowered())
		return our_vessel->GetPropellantMass(source_prop)/ our_vessel->GetPropellantMaxMass(source_prop);

	return 0.0;
}

void RCSPropellantSource::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_double(scn, "HELIUMPRESSUREPSI", heliumPressurePSI);
	papiWriteScenario_double(scn, "REGULATORPRESSUREPSI", regulatorPressurePSI);
	papiWriteScenario_double(scn, "OXIDTANKPRESSUREPSI", oxidTankPressurePSI);
	papiWriteScenario_double(scn, "FUELTANKPRESSUREPSI", fuelTankPressurePSI);
	papiWriteScenario_double(scn, "OXIDMANIFOLDPRESSUREPSI", oxidManifoldPressurePSI);
	papiWriteScenario_double(scn, "FUELMANIFOLDPRESSUREPSI", fuelManifoldPressurePSI);

	papiWriteScenario_bool(scn, "HELIUMSUPPLYVALVE_ISOPEN", heliumSupplyValve.IsOpen());
	papiWriteScenario_bool(scn, "MAINSHUTOFFVALVE_ISOPEN", mainShutoffValve.IsOpen());
	papiWriteScenario_bool(scn, "QUAD1ISOLATIONVALVE_ISOPEN", quad1IsolationValve.IsOpen());
	papiWriteScenario_bool(scn, "QUAD2ISOLATIONVALVE_ISOPEN", quad2IsolationValve.IsOpen());
	papiWriteScenario_bool(scn, "QUAD3ISOLATIONVALVE_ISOPEN", quad3IsolationValve.IsOpen());
	papiWriteScenario_bool(scn, "QUAD4ISOLATIONVALVE_ISOPEN", quad4IsolationValve.IsOpen());
	papiWriteScenario_bool(scn, "PRIMOXIDINTERCONNECTVALVE_ISOPEN", primOxidInterconnectValve.IsOpen());
	papiWriteScenario_bool(scn, "PRIMFUELINTERCONNECTVALVE_ISOPEN", primFuelInterconnectValve.IsOpen());
	papiWriteScenario_bool(scn, "SECOXIDINTERCONNECTVALVE_ISOPEN", secOxidInterconnectValve.IsOpen());
	papiWriteScenario_bool(scn, "SECFUELINTERCONNECTVALVE_ISOPEN", secFuelInterconnectValve.IsOpen());

	oapiWriteLine(scn, end_str);
}

void RCSPropellantSource::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	bool isOpen;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_double(line, "HELIUMPRESSUREPSI", heliumPressurePSI);
		papiReadScenario_double(line, "REGULATORPRESSUREPSI", regulatorPressurePSI);
		papiReadScenario_double(line, "OXIDTANKPRESSUREPSI", oxidTankPressurePSI);
		papiReadScenario_double(line, "FUELTANKPRESSUREPSI", fuelTankPressurePSI);
		papiReadScenario_double(line, "OXIDMANIFOLDPRESSUREPSI", oxidManifoldPressurePSI);
		papiReadScenario_double(line, "FUELMANIFOLDPRESSUREPSI", fuelManifoldPressurePSI);

		if (papiReadScenario_bool(line, "HELIUMSUPPLYVALVE_ISOPEN", isOpen))			heliumSupplyValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "MAINSHUTOFFVALVE_ISOPEN", isOpen))				mainShutoffValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "QUAD1ISOLATIONVALVE_ISOPEN", isOpen))			quad1IsolationValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "QUAD2ISOLATIONVALVE_ISOPEN", isOpen))			quad2IsolationValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "QUAD3ISOLATIONVALVE_ISOPEN", isOpen))			quad3IsolationValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "QUAD4ISOLATIONVALVE_ISOPEN", isOpen))			quad4IsolationValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "PRIMOXIDINTERCONNECTVALVE_ISOPEN", isOpen))	primOxidInterconnectValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "PRIMFUELINTERCONNECTVALVE_ISOPEN", isOpen))	primFuelInterconnectValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "SECOXIDINTERCONNECTVALVE_ISOPEN", isOpen))		secOxidInterconnectValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "SECFUELINTERCONNECTVALVE_ISOPEN", isOpen))		secFuelInterconnectValve.SetState(isOpen);
	}
}

TCA_FlipFlop::TCA_FlipFlop()
{
	State = false;
}

RCS_TCA::RCS_TCA(int jdsa, int jdcirc1, int jdcirc2, int tcpsa1, int tcpcirc1, int tcpsa2, int tcpcirc2)
{
	for (int i = 0;i < 2;i++)
	{
		voltageDiscreteDetector[i] = false;
		thrusterTCP[i] = false;
		pulseCounter[i] = 0;
	}

	jetDriverSA = jdsa;
	jetDriverCircuit[0] = jdcirc1;
	jetDriverCircuit[1] = jdcirc2;
	thrustChamberPressureSA[0] = tcpsa1;
	thrustChamberPressureSA[1] = tcpsa2;
	thrustChamberPressureCircuit[0] = tcpcirc1;
	thrustChamberPressureCircuit[1] = tcpcirc2;

	resetSignal = false;
	resetChannel = 0;
	lem = NULL;
}

void RCS_TCA::Init(LEM *l, int rsetcirc)
{
	lem = l;
	resetChannel = rsetcirc;
}

void RCS_TCA::Timestep()
{
	if (lem == NULL) return;

	resetSignal = lem->scera1.GetVoltage(14, resetChannel) > 2.5;

	for (int i = 0;i < 2;i++)
	{
		voltageDiscreteDetector[i] = lem->scera1.GetVoltage(jetDriverSA, jetDriverCircuit[i]) > 2.5;
		thrusterTCP[i] = lem->scera1.GetVoltage(thrustChamberPressureSA[i], thrustChamberPressureCircuit[i]) > 2.5;

		if (resetSignal || thrusterTCP[i])
		{
			pulseCounter[i] = 0;
		}
		else if (voltageDiscreteDetector[i])
		{
			pulseCounter[i]++;
		}
	}


	//Flip-Flop
	if (resetSignal)
	{
		TCAFailure.Reset();
	}
	else if (pulseCounter[0] >= 6 || pulseCounter[1] >= 6)
	{
		TCAFailure.Set();
	}
}