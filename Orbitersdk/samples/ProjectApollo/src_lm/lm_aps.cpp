/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Ascent Propulsion System

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
#include "LEM.h"
#include "papi.h"
#include "lm_aps.h"

APSValve::APSValve() {
	isOpen = false;
}

void APSValve::SetState(bool open) {
	isOpen = open;
}

void APSValve::SwitchToggled(PanelSwitchItem *s) {

	if (s->SRC && (s->SRC->Voltage() > SP_MIN_DCVOLTAGE)) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			SetState(false);
		}
	}
}

APSPropellantSource::APSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p) :
	LEMPropellantSource(ph)
{
	helium1PressurePSI = 0;
	helium2PressurePSI = 0;
	heliumRegulator1OutletPressurePSI = 0;
	heliumRegulator2OutletPressurePSI = 0;
	heliumRegulatorManifoldPressurePSI = 0;
	FuelTankUllagePressurePSI = 0;
	OxidTankUllagePressurePSI = 0;
	FuelTrimOrificeOutletPressurePSI = 0;
	OxidTrimOrificeOutletPressurePSI = 0;

	fuelLevelLow = false;
	oxidLevelLow = false;

	//Open by default
	PrimaryHeRegulatorShutoffValve.SetState(true);
	SecondaryHeRegulatorShutoffValve.SetState(true);
}

void APSPropellantSource::Timestep(double simt, double simdt)
{
	if (!our_vessel) return;

	double p, pMaxForPressures;

	if (!source_prop)
	{
		p = pMaxForPressures = our_vessel->AscentFuelMassKg;
	}
	else
	{
		p = our_vessel->GetPropellantMass(source_prop);
		pMaxForPressures = our_vessel->GetPropellantMaxMass(source_prop);
	}

	helium1PressurePSI = 3020.0;
	helium2PressurePSI = 3020.0;

	double InletPressure1, InletPressure2;

	//Primary Helium Regulator Inlet
	if (PrimaryHeRegulatorShutoffValve.IsOpen() && PrimaryHeliumIsolationValve.IsOpen())
	{
		InletPressure1 = helium1PressurePSI;
	}
	else
	{
		InletPressure1 = 0.0;
	}

	//Secondary Helium Regulator Inlet
	if (SecondaryHeRegulatorShutoffValve.IsOpen() && RedundantHeliumIsolationValve.IsOpen())
	{
		InletPressure2 = helium2PressurePSI;
	}
	else
	{
		InletPressure2 = 0.0;
	}

	//Helium Regulator 1 Outlet
	if (InletPressure1 > 190.0)
	{
		heliumRegulator1OutletPressurePSI = 190.0;
	}
	else
	{
		heliumRegulator1OutletPressurePSI = InletPressure1;
	}

	//Helium Regulator 2 Outlet
	if (InletPressure2 > 182.0)
	{
		heliumRegulator2OutletPressurePSI = 182.0;
	}
	else
	{
		heliumRegulator2OutletPressurePSI = InletPressure2;
	}

	//Helium Manifold
	heliumRegulatorManifoldPressurePSI = (heliumRegulator1OutletPressurePSI + heliumRegulator2OutletPressurePSI) / 2.0;

	//Fuel Tank
	if (FuelCompatibilityValve.IsOpen() && heliumRegulatorManifoldPressurePSI - 2.0 > 133.5*p / pMaxForPressures)
	{
		FuelTankUllagePressurePSI = max(0.0, heliumRegulatorManifoldPressurePSI - 2.0);
	}
	else
	{
		FuelTankUllagePressurePSI = 133.5*p / pMaxForPressures;
	}

	//Oxidizer Tank
	if (OxidCompatibilityValve.IsOpen() && heliumRegulatorManifoldPressurePSI - 2.0 > 133.5*p / pMaxForPressures)
	{
		OxidTankUllagePressurePSI = max(0.0, heliumRegulatorManifoldPressurePSI - 2.0);
	}
	else
	{
		OxidTankUllagePressurePSI = 133.5*p / pMaxForPressures;
	}

	FuelTrimOrificeOutletPressurePSI = max(0.0, FuelTankUllagePressurePSI - 14.0);
	OxidTrimOrificeOutletPressurePSI = max(0.0, OxidTankUllagePressurePSI - 14.0);

	//Primary Helium Isolation Valve
	if (!PrimaryHeliumIsolationValve.IsOpen() && our_vessel->AscentHeliumIsol1Pyros.Blown())
	{
		PrimaryHeliumIsolationValve.SetState(true);
	}

	//Redundant Helium Isolation Valve
	if (!RedundantHeliumIsolationValve.IsOpen() && our_vessel->AscentHeliumIsol2Pyros.Blown())
	{
		RedundantHeliumIsolationValve.SetState(true);
	}

	//Propellant Compatibility Valves
	if (!OxidCompatibilityValve.IsOpen() && our_vessel->AscentOxidCompValvePyros.Blown())
	{
		OxidCompatibilityValve.SetState(true);
	}

	if (!FuelCompatibilityValve.IsOpen() && our_vessel->AscentFuelCompValvePyros.Blown())
	{
		FuelCompatibilityValve.SetState(true);
	}

	//Propellant Low
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered() && p < 50.5)
	{
		fuelLevelLow = true;
		oxidLevelLow = true;
	}
	else
	{
		fuelLevelLow = false;
		oxidLevelLow = false;
	}
}

double APSPropellantSource::GetAscentHelium1PressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return helium1PressurePSI;

	return 0.0;
}

double APSPropellantSource::GetAscentHelium2PressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return helium2PressurePSI;

	return 0.0;
}

double APSPropellantSource::GetFuelTankUllagePressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return FuelTankUllagePressurePSI;

	return 0.0;
}

double APSPropellantSource::GetOxidizerTankUllagePressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return OxidTankUllagePressurePSI;

	return 0.0;
}

double APSPropellantSource::GetHeliumRegulator1OutletPressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return heliumRegulator1OutletPressurePSI;

	return 0.0;
}

double APSPropellantSource::GetHeliumRegulator2OutletPressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return heliumRegulator2OutletPressurePSI;

	return 0.0;
}

double APSPropellantSource::GetFuelTankBulkTempF()
{
	return 70.0;
}

double APSPropellantSource::GetOxidizerTankBulkTempF()
{
	return 70.0;
}

void APSPropellantSource::SaveState(FILEHANDLE scn)
{
	oapiWriteLine(scn, APSPROPELLANT_START_STRING);

	papiWriteScenario_bool(scn, "FUELLEVELLOW", fuelLevelLow);
	papiWriteScenario_bool(scn, "OXIDLEVELLOW", oxidLevelLow);
	papiWriteScenario_double(scn, "HELIUM1PRESSUREPSI", helium1PressurePSI);
	papiWriteScenario_double(scn, "HELIUM2PRESSUREPSI", helium2PressurePSI);
	papiWriteScenario_double(scn, "HELIUMREGULATOR1OUTLETPRESSUREPSI", heliumRegulator1OutletPressurePSI);
	papiWriteScenario_double(scn, "HELIUMREGULATOR2OUTLETPRESSUREPSI", heliumRegulator2OutletPressurePSI);
	papiWriteScenario_double(scn, "HELIUMREGULATORMANIFOLDPRESSUREPSI", heliumRegulatorManifoldPressurePSI);
	papiWriteScenario_double(scn, "FUELTANKULLAGEPRESSUREPSI", FuelTankUllagePressurePSI);
	papiWriteScenario_double(scn, "OXIDTANKULLAGEPRESSUREPSI", OxidTankUllagePressurePSI);
	papiWriteScenario_double(scn, "FUELTRIMORIFICEOUTLETPRESSUREPSI", FuelTrimOrificeOutletPressurePSI);
	papiWriteScenario_double(scn, "OXIDTRIMORIFICEOUTLETPRESSUREPSI", OxidTrimOrificeOutletPressurePSI);

	papiWriteScenario_bool(scn, "PRIMREGHELIUMVALVE_ISOPEN", PrimaryHeRegulatorShutoffValve.IsOpen());
	papiWriteScenario_bool(scn, "SECREGHELIUMVALVE_ISOPEN", SecondaryHeRegulatorShutoffValve.IsOpen());
	papiWriteScenario_bool(scn, "PRIMARYHELIUMISOLATIONVALVE_ISOPEN", PrimaryHeliumIsolationValve.IsOpen());
	papiWriteScenario_bool(scn, "REDUNDANTHELIUMISOLATIONVALVE_ISOPEN", RedundantHeliumIsolationValve.IsOpen());
	papiWriteScenario_bool(scn, "FUELCOMPATIBILITYVALVE_ISOPEN", FuelCompatibilityValve.IsOpen());
	papiWriteScenario_bool(scn, "OXIDCOMPATIBILITYVALVE_ISOPEN", OxidCompatibilityValve.IsOpen());

	oapiWriteLine(scn, APSPROPELLANT_END_STRING);
}

void APSPropellantSource::LoadState(FILEHANDLE scn)
{
	char *line;
	bool isOpen;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, APSPROPELLANT_END_STRING, sizeof(APSPROPELLANT_END_STRING))) {
			return;
		}

		papiReadScenario_bool(line, "FUELLEVELLOW", fuelLevelLow);
		papiReadScenario_bool(line, "OXIDLEVELLOW", oxidLevelLow);
		papiReadScenario_double(line, "HELIUM1PRESSUREPSI", helium1PressurePSI);
		papiReadScenario_double(line, "HELIUM2PRESSUREPSI", helium2PressurePSI);
		papiReadScenario_double(line, "HELIUMREGULATOR1OUTLETPRESSUREPSI", heliumRegulator1OutletPressurePSI);
		papiReadScenario_double(line, "HELIUMREGULATOR2OUTLETPRESSUREPSI", heliumRegulator2OutletPressurePSI);
		papiReadScenario_double(line, "HELIUMREGULATORMANIFOLDPRESSUREPSI", heliumRegulatorManifoldPressurePSI);
		papiReadScenario_double(line, "FUELTANKULLAGEPRESSUREPSI", FuelTankUllagePressurePSI);
		papiReadScenario_double(line, "OXIDTANKULLAGEPRESSUREPSI", OxidTankUllagePressurePSI);
		papiReadScenario_double(line, "FUELTRIMORIFICEOUTLETPRESSUREPSI", FuelTrimOrificeOutletPressurePSI);
		papiReadScenario_double(line, "OXIDTRIMORIFICEOUTLETPRESSUREPSI", OxidTrimOrificeOutletPressurePSI);

		if (papiReadScenario_bool(line, "PRIMREGHELIUMVALVE_ISOPEN", isOpen))			PrimaryHeRegulatorShutoffValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "SECREGHELIUMVALVE_ISOPEN", isOpen))			SecondaryHeRegulatorShutoffValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "PRIMARYHELIUMISOLATIONVALVE_ISOPEN", isOpen))		PrimaryHeliumIsolationValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "REDUNDANTHELIUMISOLATIONVALVE_ISOPEN", isOpen))		RedundantHeliumIsolationValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "FUELCOMPATIBILITYVALVE_ISOPEN", isOpen))		FuelCompatibilityValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "OXIDCOMPATIBILITYVALVE_ISOPEN", isOpen))		OxidCompatibilityValve.SetState(isOpen);
	}
}

// Ascent Propulsion System
LEM_APS::LEM_APS()
{
	lem = NULL;
	thrustOn = false;
	armedOn = false;
	ChamberPressure = 0;
}

void LEM_APS::Init(LEM *s) {
	lem = s;
}

void LEM_APS::Timestep(double simdt) {
	if (lem == NULL) { return; }

	ChamberPressure = 0.0;

	if (lem->stage > 1)
	{
		if (armedOn)
		{
			lem->SetThrusterResource(lem->th_hover[0], lem->ph_Asc);
			lem->SetThrusterResource(lem->th_hover[1], lem->ph_Asc);
		}
		else
		{
			lem->SetThrusterResource(lem->th_hover[0], NULL);
			lem->SetThrusterResource(lem->th_hover[1], NULL);
		}


		if (thrustOn && armedOn)
		{
			//Thrust decay with low pressure
			double FuelInletPressure = lem->GetAPSPropellant()->GetFuelTrimOrificeOutletPressurePSI();
			double OxidInletPressure = lem->GetAPSPropellant()->GetOxidTrimOrificeOutletPressurePSI();
			if (FuelInletPressure > OxidInletPressure)
			{
				ChamberPressure = OxidInletPressure*120.0 / 170.0;
			}
			else
			{
				ChamberPressure = FuelInletPressure*120.0 / 170.0;
			}

			double ThrustDecay = min(1.0, ChamberPressure / 120.0);

			lem->SetThrusterLevel(lem->th_hover[0], ThrustDecay);
			lem->SetThrusterLevel(lem->th_hover[1], ThrustDecay);
		}
		else
		{
			lem->SetThrusterLevel(lem->th_hover[0], 0.0);
			lem->SetThrusterLevel(lem->th_hover[1], 0.0);
		}
	}
}

double LEM_APS::GetThrustChamberPressurePSI()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	return ChamberPressure;
}

void LEM_APS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "ARMEDON", armedOn);
	papiWriteScenario_bool(scn, "THRUSTON", thrustOn);

	oapiWriteLine(scn, end_str);
}

void LEM_APS::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "ARMEDON", armedOn);
		papiReadScenario_bool(line, "THRUSTON", thrustOn);

	}
}