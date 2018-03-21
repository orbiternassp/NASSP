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

RCSPropellantSource::RCSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p, bool hasXFeedValve) : LEMPropellantSource(ph)
{
	hasCrossFeedValve = hasXFeedValve;

	heliumPressurePSI = 3050.0;
	regulatorPressurePSI = 30.0;
	oxidManifoldPressurePSI = 21.0;
	fuelManifoldPressurePSI = 21.0;

	lastPropellantMass = 0.0;

	otherSystem = NULL;
	RCSHeliumSupplyPyros = NULL;
	prop = ph;

	for (int i = 0;i < 4;i++)
	{
		quadThruster1ID[i] = 0;
	}

	//Open at launch
	primOxidInterconnectValve.SetState(true);
	primFuelInterconnectValve.SetState(true);
	mainShutoffValve.SetState(true);
}

RCSPropellantSource::~RCSPropellantSource()
{

}

void RCSPropellantSource::Init(THRUSTER_HANDLE *th, Pyro *rcshsp, RCSPropellantSource *otherSys, int q1th1, int q2th1, int q3th1, int q4th1)
{
	thrusters = th;
	RCSHeliumSupplyPyros = rcshsp;
	otherSystem = otherSys;
	quadThruster1ID[0] = q1th1;
	quadThruster1ID[1] = q2th1;
	quadThruster1ID[2] = q3th1;
	quadThruster1ID[3] = q4th1;
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
		double pMaxForPressures = our_vessel->GetPropellantMaxMass(source_prop);

		//Quadratic fit: 3050 PSI at 100%, 2500 PSI at 74%, 2200 PSI at 55%
		heliumPressurePSI = 1192.08*(p / pMaxForPressures)*(p / pMaxForPressures) + 41.1606*(p / pMaxForPressures) + 1816.76;

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
			regulatorPressurePSI = 30.0 * (p / pMaxForPressures);
		}

		bool ascFeed = primOxidInterconnectValve.IsOpen() && secOxidInterconnectValve.IsOpen() &&
			primFuelInterconnectValve.IsOpen() && secFuelInterconnectValve.IsOpen();

		double APSOxidPressure, APSFuelPressure, FuelPressure, OxidPressure, XFeedOxidPressure, XFeedFuelPressure;

		if (mainShutoffValve.IsOpen() && p > 0)
		{
			OxidPressure = FuelPressure = regulatorPressurePSI - 9.0;
		}
		else
		{
			OxidPressure = FuelPressure = 0.0;
		}

		if (ascFeed)
		{
			APSFuelPressure = our_vessel->APSPropellant.GetFuelTrimOrificeOutletPressurePSI();
			APSOxidPressure = our_vessel->APSPropellant.GetOxidTrimOrificeOutletPressurePSI();
		}
		else
		{
			APSFuelPressure = 0;
			APSOxidPressure = 0;
		}

		bool crossFeed;
		
		if (hasCrossFeedValve)
		{
			crossFeed = fuelCrossfeedValve.IsOpen() && oxidCrossfeedValve.IsOpen();
		}
		else
		{
			crossFeed = otherSystem->fuelCrossfeedValve.IsOpen() && otherSystem->oxidCrossfeedValve.IsOpen();
		}

		if (crossFeed)
		{
			XFeedOxidPressure = otherSystem->oxidManifoldPressurePSI;
			XFeedFuelPressure = otherSystem->fuelManifoldPressurePSI;
		}
		else
		{
			XFeedOxidPressure = 0;
			XFeedFuelPressure = 0;
		}

		//Manifold

		//Propellant from other source
		if ((XFeedFuelPressure > FuelPressure) && (XFeedFuelPressure > APSFuelPressure))
		{
			fuelManifoldPressurePSI = max((fuelManifoldPressurePSI + otherSystem->fuelManifoldPressurePSI) / 2.0 - 9.0, 21.0);

			if (fuelManifoldPressurePSI > otherSystem->fuelManifoldPressurePSI)
			{
				prop = source_prop;
			}
			else
			{
				prop = otherSystem->prop;
			}
		}
		//propellant from APS
		else if ((APSFuelPressure > FuelPressure) && (APSFuelPressure > XFeedFuelPressure))
		{
			fuelManifoldPressurePSI = APSFuelPressure - 9.0;
			prop = our_vessel->ph_Asc;
		}
		//propellant from own source
		else
		{
			fuelManifoldPressurePSI = max(FuelPressure - 9.0, 21.0);
			prop = source_prop;
		}

		if ((XFeedOxidPressure > OxidPressure) && (XFeedOxidPressure > APSOxidPressure))
		{
			oxidManifoldPressurePSI = max((oxidManifoldPressurePSI + otherSystem->oxidManifoldPressurePSI) / 2.0 - 9.0, 21.0);
		}

		else if ((APSOxidPressure > OxidPressure) && (APSOxidPressure > XFeedOxidPressure))
		{
			oxidManifoldPressurePSI = APSOxidPressure - 9.0;
		}
		else
		{
			oxidManifoldPressurePSI = max(OxidPressure - 9.0, 21.0);
		}

		if (oxidManifoldPressurePSI > 100.0 && fuelManifoldPressurePSI > 100.0)
		{
			if (quad1IsolationValve.IsOpen())
			{
				SetThrusters(1, prop);
			}
			else
			{
				SetThrusters(1, NULL);
			}

			if (quad2IsolationValve.IsOpen())
			{
				SetThrusters(2, prop);
			}
			else
			{
				SetThrusters(2, NULL);
			}

			if (quad3IsolationValve.IsOpen())
			{
				SetThrusters(3, prop);
			}
			else
			{
				SetThrusters(3, NULL);
			}

			if (quad4IsolationValve.IsOpen())
			{
				SetThrusters(4, prop);
			}
			else
			{
				SetThrusters(4, NULL);
			}
		}
		else
		{
			SetThrusters(NULL);
		}

		//Explosive valves
		if (!heliumSupplyValve.IsOpen() && RCSHeliumSupplyPyros->Blown())
		{
			heliumSupplyValve.SetState(true);
		}


		lastPropellantMass = p;
	}
}

void RCSPropellantSource::SetThrusters(PROPELLANT_HANDLE ph) {

	for (int i = 1;i <= 4;i++)
	{
		SetThrusters(i, ph);
	}
}

void RCSPropellantSource::SetThrusters(int quad, PROPELLANT_HANDLE ph) {

	for (int i = 0; i < 2; i++) {
		if (thrusters[quadThruster1ID[quad - 1] + i])
			our_vessel->SetThrusterResource(thrusters[quadThruster1ID[quad - 1] + i], ph);
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

void RCSPropellantSource::CrossfeedToggled(PanelSwitchItem *s) {

	if (hasCrossFeedValve)
	{
		if (s->SRC && (s->SRC->Voltage() > SP_MIN_DCVOLTAGE)) {
			if (((ThreePosSwitch *)s)->IsUp()) {
				fuelCrossfeedValve.SetState(true);
				oxidCrossfeedValve.SetState(true);
			}
			else if (((ThreePosSwitch *)s)->IsDown()) {
				fuelCrossfeedValve.SetState(false);
				oxidCrossfeedValve.SetState(false);
			}
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
	if (our_vessel->RCS_B_PQGS_DISP_CB.IsPowered() && source_prop)
		return our_vessel->GetPropellantMass(source_prop)/ our_vessel->GetPropellantMaxMass(source_prop);

	return 0.0;
}

double RCSPropellantSource::GetFuelTankTempF()
{
	return 70.0;
}

void RCSPropellantSource::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_double(scn, "HELIUMPRESSUREPSI", heliumPressurePSI);
	papiWriteScenario_double(scn, "REGULATORPRESSUREPSI", regulatorPressurePSI);
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

	if (hasCrossFeedValve)
	{
		papiWriteScenario_bool(scn, "FUELCROSSFEEDVALVE_ISOPEN", fuelCrossfeedValve.IsOpen());
		papiWriteScenario_bool(scn, "OXIDCROSSFEEDVALVE_ISOPEN", oxidCrossfeedValve.IsOpen());
	}

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
		if (papiReadScenario_bool(line, "FUELCROSSFEEDVALVE_ISOPEN", isOpen))		fuelCrossfeedValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "OXIDCROSSFEEDVALVE_ISOPEN", isOpen))		oxidCrossfeedValve.SetState(isOpen);
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
		jetDriverSignal[i] = false;
		thrusterTCP[i] = false;
		pulseCounter[i] = 0;
		pulseFlag[i] = false;
		failTimer[i] = 0.0;
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

void RCS_TCA::Timestep(double simdt)
{
	if (lem == NULL) return;

	resetSignal = lem->scera1.GetVoltage(14, resetChannel) > 2.5;

	for (int i = 0;i < 2;i++)
	{
		//Is there a thruster demand?
		jetDriverSignal[i] = lem->scera1.GetVoltage(jetDriverSA, jetDriverCircuit[i]) > 2.5;
		//Is the thruster on?
		thrusterTCP[i] = lem->scera1.GetVoltage(thrustChamberPressureSA[i], thrustChamberPressureCircuit[i]) > 2.5;

		//Only allow individual thruster commands to count as a pulse
		if (pulseFlag[i] && !jetDriverSignal[i])
		{
			pulseFlag[i] = false;
		}

		if (resetSignal || thrusterTCP[i])
		{
			pulseCounter[i] = 0;
			pulseFlag[i] = false;
		}
		else if (jetDriverSignal[i] && !pulseFlag[i])
		{
			pulseCounter[i]++;
			pulseFlag[i] = true;
		}

		if ((jetDriverSignal[i] && !resetSignal) && !thrusterTCP[i])
		{
			failTimer[i] += simdt;
		}
		else
		{
			failTimer[i] = 0.0;
		}
	}


	//Flip-Flop
	if (resetSignal)
	{
		TCAFailure.Reset();
	}
	else if (pulseCounter[0] >= 6 || pulseCounter[1] >= 6 || failTimer[0] > 0.08 || failTimer[1] > 0.08)
	{
		TCAFailure.Set();
	}
}

void RCS_TCA::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "TCAFAILURE", TCAFailure.IsSet());

	oapiWriteLine(scn, end_str);
}

void RCS_TCA::LoadState(FILEHANDLE scn, char *end_str)
{
	char *line;
	bool isSet;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		if (papiReadScenario_bool(line, "TCAFAILURE", isSet))			if (isSet) TCAFailure.Set();
	}
}