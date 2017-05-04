/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008

  CSM Reaction Control System

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
#include "Orbitersdk.h"

#include "stdio.h"
#include "math.h"
#include "soundlib.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "ioChannels.h"
#include "saturn.h"
#include "papi.h"

SMRCSHeliumValve::SMRCSHeliumValve() {
	isOpen = false;
	isSec = false;
}

void SMRCSHeliumValve::SetPropellantSource(SMRCSPropellantSource *p, bool sec) {
	propellant = p;
	isSec = sec;
}

void SMRCSHeliumValve::SetState(bool open) {
	isOpen = open;
	if (!isSec)
		propellant->CheckHeliumValves(); 
	else
		propellant->CheckSecHeliumValve(); 		
}

void SMRCSHeliumValve::SwitchToggled(PanelSwitchItem *s) {

	if (s->SRC && (s->SRC->Voltage() > SP_MIN_DCVOLTAGE)) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			SetState(false);
		}
	}
}


RCSPropellantValve::RCSPropellantValve() {
	isOpen = false;
}

void RCSPropellantValve::SetState(bool open) {
	isOpen = open;
}


SMRCSPropellantSource::SMRCSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p) : 
	PropellantSource(ph) {

	primPropellantMass = PRIM_RCS_FUEL_PER_QUAD;
	lastPropellantMass = RCS_FUEL_PER_QUAD;
	lastPrimPropellantMassHeliumValvesClosed = PRIM_RCS_FUEL_PER_QUAD;
	lastSecPropellantMassHeliumValvesClosed = RCS_FUEL_PER_QUAD - PRIM_RCS_FUEL_PER_QUAD;
	
	propellantPressurePSI = 0;
	heliumPressurePSI = 0;
	propellantQuantityToDisplay = 0;

	heliumValve1.SetPropellantSource(this, false); 
	heliumValve2.SetPropellantSource(this, false); 
	secPropellantPressureValve.SetPropellantSource(this, true); 
	propellantBuffer = -1;
}

SMRCSPropellantSource::~SMRCSPropellantSource() {
	// Nothing for now.
}

void SMRCSPropellantSource::Init(THRUSTER_HANDLE *th, h_Radiator *p) {

	thrusters = th;
	package = p;
}

void SMRCSPropellantSource::Timestep(double simt, double simdt) {

	if (!our_vessel) return;

	double p;
	if (our_vessel->GetStage() < CSM_LEM_STAGE) {
		p = RCS_FUEL_PER_QUAD;

		propellantPressurePSI = 200.0;
		heliumPressurePSI = 4150.0;
		propellantQuantityToDisplay = 1.0;
	
	} else if (!source_prop) {
		p = 0;

		propellantPressurePSI = 0;
		heliumPressurePSI = 0;
		propellantQuantityToDisplay = 0.0;

	} else {
		p = our_vessel->GetPropellantMass(source_prop);
		
		// Propellant pressure/consumption
		const double d = 2.; // "pressure decay" parameter
		if (p == 0) {
			propellantPressurePSI = 0.0;
			primPropellantMass = 0;

		} else if (primPropellantMass == 0) {
			if ((heliumValve1.IsOpen() || heliumValve2.IsOpen()) && secPropellantPressureValve.IsOpen()) {
				propellantPressurePSI = 185.0;
			} else {
				propellantPressurePSI = ((RCS_FUEL_PER_QUAD - PRIM_RCS_FUEL_PER_QUAD) - lastSecPropellantMassHeliumValvesClosed + d) /
										((RCS_FUEL_PER_QUAD - PRIM_RCS_FUEL_PER_QUAD) - p + d) * 185.0;
			}

		} else if (lastPropellantMass - primPropellantMass == 0) {
			if (heliumValve1.IsOpen() || heliumValve2.IsOpen()) {
				propellantPressurePSI = 185.0;
			} else {
				propellantPressurePSI = (PRIM_RCS_FUEL_PER_QUAD - lastPrimPropellantMassHeliumValvesClosed + d) /
										(PRIM_RCS_FUEL_PER_QUAD - p + d) * 185.0;
			}
			primPropellantMass -= lastPropellantMass - p;

		} else { // both tanks aren't empty
			if ((heliumValve1.IsOpen() || heliumValve2.IsOpen()) && secPropellantPressureValve.IsOpen()) {
				propellantPressurePSI = 185.0;
				primPropellantMass -= (lastPropellantMass - p) / 2.;

			} else if ((heliumValve1.IsOpen() || heliumValve2.IsOpen()) && !secPropellantPressureValve.IsOpen()) {
				propellantPressurePSI = 185.0;
				primPropellantMass -= lastPropellantMass - p;

			} else { // both tanks aren't pressurized
				// each tank spends a propellant mass so that the propellant pressure is the same
				double primdp = (((lastPropellantMass - p + RCS_FUEL_PER_QUAD - PRIM_RCS_FUEL_PER_QUAD - (lastPropellantMass - primPropellantMass) + d) * (PRIM_RCS_FUEL_PER_QUAD - lastPrimPropellantMassHeliumValvesClosed + d)) - 
					             ((PRIM_RCS_FUEL_PER_QUAD - primPropellantMass + d) * (RCS_FUEL_PER_QUAD - PRIM_RCS_FUEL_PER_QUAD - lastSecPropellantMassHeliumValvesClosed + d))) /
								(RCS_FUEL_PER_QUAD - lastPrimPropellantMassHeliumValvesClosed - lastSecPropellantMassHeliumValvesClosed + 2. * d);
				primPropellantMass -= primdp;
				propellantPressurePSI = (PRIM_RCS_FUEL_PER_QUAD - lastPrimPropellantMassHeliumValvesClosed + d) /
										(PRIM_RCS_FUEL_PER_QUAD - primPropellantMass + d) * 185.0;
			}
		}
		lastPropellantMass = p;
		primPropellantMass = max(0, primPropellantMass);

		// propellant/thruster handling
		//
		// Nitrogen tetroxide (the oxidizer) has a freezing temperature of about 10°F (fuel much below)
		// Min. pressure of 50 psi was chosen arbitrarily just to have reasonable engine behavior
		//		
		if (GetPackageTempF() > 10 && propellantPressurePSI > 50 && (primPropellantValve.IsOpen() || secPropellantValve.IsOpen())) {
			SetThrusters(source_prop);
		} else {
			SetThrusters(NULL);
		}

		// propellant quantity
		propellantQuantityToDisplay = 0.825 * p / RCS_FUEL_PER_QUAD + 0.175 + (1. - p / RCS_FUEL_PER_QUAD) * (GetHeliumTempF() - 65.) * 0.075 / 25.; 

/*		sprintf(oapiDebugString(), "PCK-T %.1f He-T %.1f p %.1f Prp-p %.1f qty %.1f m %.2f pm %.2f sm %.2f lpm %.2f lsm %.2f", GetPackageTempF(), GetHeliumTempF(), 
			GetHeliumPressurePSI(), GetPropellantPressurePSI(), GetPropellantQuantityToDisplay() * 100., p, primPropellantMass, p - primPropellantMass, 
			lastPrimPropellantMassHeliumValvesClosed, lastSecPropellantMassHeliumValvesClosed); 
*/	}
}

void SMRCSPropellantSource::SystemTimestep(double simdt) {

}

void SMRCSPropellantSource::SetThrusters(PROPELLANT_HANDLE ph) {

	for (int i = 0; i < 8; i++) {
		if (thrusters[i])
			our_vessel->SetThrusterResource(thrusters[i], ph);
	}
}

void SMRCSPropellantSource::PropellantSwitchToggled(PanelSwitchItem *s) {

	if (s->IsPowered()) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			primPropellantValve.SetState(true);
			secPropellantValve.SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			primPropellantValve.SetState(false);
			secPropellantValve.SetState(false);
		}
	}
}

void SMRCSPropellantSource::CheckHeliumValves() {

	if (!heliumValve1.IsOpen() && !heliumValve2.IsOpen()) {
		lastPrimPropellantMassHeliumValvesClosed = primPropellantMass;
		if (secPropellantPressureValve.IsOpen()) {
			lastSecPropellantMassHeliumValvesClosed = lastPropellantMass - primPropellantMass;
		}			
	}
}

void SMRCSPropellantSource::CheckSecHeliumValve() {

	if ((heliumValve1.IsOpen() || heliumValve2.IsOpen()) && !secPropellantPressureValve.IsOpen()) {
		lastSecPropellantMassHeliumValvesClosed = lastPropellantMass - primPropellantMass;					
	}
}

double SMRCSPropellantSource::GetPackageTempF() {

	if (!our_vessel) return 0;
	if (our_vessel->GetStage() > CSM_LEM_STAGE) return 0;
	
	return KelvinToFahrenheit(package->GetTemp());
}

double SMRCSPropellantSource::GetHeliumTempF() {

	if (!our_vessel) return 0;
	if (our_vessel->GetStage() > CSM_LEM_STAGE) return 0;

	return 0.41666667 * GetPackageTempF() + 6.66666667;
}

double SMRCSPropellantSource::GetHeliumPressurePSI() {

	if (!our_vessel) return 0;
	if (our_vessel->GetStage() > CSM_LEM_STAGE) return 0;

	return (propellantQuantityToDisplay + 1.18426) * FahrenheitToKelvin(GetHeliumTempF()) / 0.1549;
}

void SMRCSPropellantSource::SaveState(FILEHANDLE scn) {

	// START_STRING is written in Saturn
	if (source_prop && our_vessel)
		papiWriteScenario_double(scn, "PROPELLANTMASS", our_vessel->GetPropellantMass(source_prop));
	papiWriteScenario_double(scn, "LASTPROPELLANTMASS", lastPropellantMass);
	papiWriteScenario_double(scn, "PRIMPROPELLANTMASS", primPropellantMass);
	papiWriteScenario_double(scn, "LASTPRIMPROPELLANTMASSHELIUMVALVESCLOSED", lastPrimPropellantMassHeliumValvesClosed);
	papiWriteScenario_double(scn, "LASTSECPROPELLANTMASSHELIUMVALVESCLOSED", lastSecPropellantMassHeliumValvesClosed);
	papiWriteScenario_double(scn, "PROPELLANTPRESSUREPSI", propellantPressurePSI);
	papiWriteScenario_double(scn, "HELIUMPRESSUREPSI", heliumPressurePSI);
	papiWriteScenario_double(scn, "PROPELLANTQUANTITYTODISPLAY", propellantQuantityToDisplay);

	papiWriteScenario_bool(scn, "HELIUMVALVE1_ISOPEN", heliumValve1.IsOpen()); 
	papiWriteScenario_bool(scn, "HELIUMVALVE2_ISOPEN", heliumValve2.IsOpen()); 
	papiWriteScenario_bool(scn, "SECPROPELLANTPRESSUREVALVE_ISOPEN", secPropellantPressureValve.IsOpen()); 
	papiWriteScenario_bool(scn, "PRIMPROPELLANTVALVE_ISOPEN", primPropellantValve.IsOpen()); 
	papiWriteScenario_bool(scn, "SECPROPELLANTVALVE_ISOPEN", secPropellantValve.IsOpen()); 

	oapiWriteLine(scn, SMRCSPROPELLANT_END_STRING);
}

void SMRCSPropellantSource::LoadState(FILEHANDLE scn) {

	char *line;
	bool isOpen;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SMRCSPROPELLANT_END_STRING, sizeof(SMRCSPROPELLANT_END_STRING))) {
			return;
		}
		papiReadScenario_double(line, "PROPELLANTMASS", propellantBuffer);
		papiReadScenario_double(line, "LASTPROPELLANTMASS", lastPropellantMass);
		papiReadScenario_double(line, "PRIMPROPELLANTMASS", primPropellantMass);
		papiReadScenario_double(line, "LASTPRIMPROPELLANTMASSHELIUMVALVESCLOSED", lastPrimPropellantMassHeliumValvesClosed);
		papiReadScenario_double(line, "LASTSECPROPELLANTMASSHELIUMVALVESCLOSED", lastSecPropellantMassHeliumValvesClosed);
		papiReadScenario_double(line, "PROPELLANTPRESSUREPSI", propellantPressurePSI);
		papiReadScenario_double(line, "HELIUMPRESSUREPSI", heliumPressurePSI);
		papiReadScenario_double(line, "PROPELLANTQUANTITYTODISPLAY", propellantQuantityToDisplay);

		if (papiReadScenario_bool(line, "HELIUMVALVE1_ISOPEN", isOpen))					heliumValve1.SetState(isOpen); 
		if (papiReadScenario_bool(line, "HELIUMVALVE2_ISOPEN", isOpen))					heliumValve2.SetState(isOpen); 
		if (papiReadScenario_bool(line, "SECPROPELLANTPRESSUREVALVE_ISOPEN", isOpen))	secPropellantPressureValve.SetState(isOpen); 
		if (papiReadScenario_bool(line, "PRIMPROPELLANTVALVE_ISOPEN", isOpen))			primPropellantValve.SetState(isOpen); 
		if (papiReadScenario_bool(line, "SECPROPELLANTVALVE_ISOPEN", isOpen))			secPropellantValve.SetState(isOpen); 
	}
}

void SMRCSPropellantSource::CheckPropellantMass() {

	if (propellantBuffer != -1 && source_prop && our_vessel)
		our_vessel->SetPropellantMass(source_prop, propellantBuffer);
}


CMRCSPropellantSource::CMRCSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p) : 
	PropellantSource(ph) {

	int i;

	heliumQuantity = 1;
	lastPropellantMass = CM_RCS_FUEL_PER_TANK;
	propellantBuffer = -1;
	heliumValvesOpen = false;
	fuelInterconnectValvesOpen = false;
	oxidizerInterconnectValvesOpen = false;
	purgeValvesOpen = false;
	oxidizerDumpValvesOpen = false;
	fuelDumpValvesOpen = false;
	for (i = 0; i < 6; i++) {
		purgeLevel[i] = 0;
	}
}

CMRCSPropellantSource::~CMRCSPropellantSource() {
	// Nothing for now.
}

void CMRCSPropellantSource::Init(THRUSTER_HANDLE *th, h_Radiator *t, CMRCSPropellantSource *ic, e_object *pp, e_object *ppp, e_object *isol) {

	thrusters = th;
	heliumTank = t;
	interconnectedSystem = ic;
	purgePower = pp;
	purgePyroPower = ppp;
	isolPower = isol;
}

void CMRCSPropellantSource::Timestep(double simt, double simdt) {

	if (!our_vessel) return;

	// Helium squib valves
	if (!heliumValvesOpen) {
		if (our_vessel->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) {
			// Manual or automatic pressurization
			if (our_vessel->secs.MESCA.GetCMRCSPressRelay()) {
				OpenHeliumValves();
			}			
		}
		if (our_vessel->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE) {
			// Manual or automatic pressurization
			if (our_vessel->secs.MESCB.GetCMRCSPressRelay()) {
				OpenHeliumValves();
			}
		}
	}

	// Fuel/oxidizer interconnect valves
	if (!fuelInterconnectValvesOpen && our_vessel->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE) {
		// Manual control
		if (our_vessel->secs.rcsc.GetInterconnectAndPropellantBurnRelayB()) {
			fuelInterconnectValvesOpen = true;
		}
	}
	if (!oxidizerInterconnectValvesOpen && our_vessel->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) {
		// Manual control
		if (our_vessel->secs.rcsc.GetInterconnectAndPropellantBurnRelayA()) {
			oxidizerInterconnectValvesOpen = true;
		}
	}

	// Purge valves
	if (!purgeValvesOpen && purgePyroPower->Voltage() > SP_MIN_DCVOLTAGE && purgePower->Voltage() > SP_MIN_DCVOLTAGE) {
		if (our_vessel->secs.rcsc.GetOxidFuelPurgeRelay()) {
			OpenPurgeValves();
		}
	}

	// Interconnect propellant exchange
	// As a simplification, propellant and helium are only transferred when both interconnect valves are open
	double myMass, otherMass, newMass;
	if (interconnectedSystem != NULL && fuelInterconnectValvesOpen && oxidizerInterconnectValvesOpen) {
		// Equalize propellant mass
		if (source_prop != NULL && interconnectedSystem->Handle() != NULL) {
			myMass = our_vessel->GetPropellantMass(source_prop);
			otherMass = our_vessel->GetPropellantMass(interconnectedSystem->Handle());
			if (myMass != otherMass) {
				newMass = (myMass + otherMass) / 2.0;
				if (newMass < 1e-6) newMass = 0;
				our_vessel->SetPropellantMass(source_prop, newMass);
				our_vessel->SetPropellantMass(interconnectedSystem->Handle(), newMass);
			}
		}
		// Equalize helium quantity
		myMass = heliumQuantity;
		otherMass = interconnectedSystem->heliumQuantity;
		if (myMass != otherMass) {
			newMass = (myMass + otherMass) / 2.0;
			if (newMass < 1e-6) newMass = 0;
			heliumQuantity = newMass;
			interconnectedSystem->heliumQuantity = newMass;
		}
	}

	//Automatic closing of propellant valves
	if (propellantValve.IsOpen() && isolPower->Voltage() > SP_MIN_DCVOLTAGE && our_vessel->secs.rcsc.GetOxidizerDumpRelay())
	{
		propellantValve.SetState(false);
	}

	//Propellant dump valves
	if (!fuelDumpValvesOpen) {
		if (our_vessel->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) {
			// Open fuel dump valves
			if (our_vessel->secs.rcsc.GetFuelDumpRelay()) {
				fuelDumpValvesOpen = true;
			}
		}
		if (our_vessel->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE) {
			// Open fuel dump valves
			if (our_vessel->secs.rcsc.GetFuelDumpRelay()) {
				fuelDumpValvesOpen = true;
			}
		}
	}

	if (!oxidizerDumpValvesOpen) {
		if (our_vessel->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) {
			// Open oxidizer dump valves
			if (our_vessel->secs.rcsc.GetOxidizerDumpRelay()) {
				oxidizerDumpValvesOpen = true;
			}
		}
		if (our_vessel->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE) {
			// Open oxidizer dump valves
			if (our_vessel->secs.rcsc.GetOxidizerDumpRelay()) {
				oxidizerDumpValvesOpen = true;
			}
		}
	}

	if (source_prop) {

		// Adiabatic helium cooling because of expansion, see Apollo 9 Mission Report Supplement - CSM RCS Performance [NTRS 19740079928]
		double p = our_vessel->GetPropellantMass(source_prop);
		if (p != lastPropellantMass) {
			double t1 = KelvinToFahrenheit(heliumTank->GetTemp());
			double t2 = (lastPropellantMass - p) / CM_RCS_FUEL_PER_TANK * 60.;
			double t3 = t1 - t2;
			double t4 = FahrenheitToKelvin(t3);
			heliumTank->SetTemp(FahrenheitToKelvin(KelvinToFahrenheit(heliumTank->GetTemp()) - (lastPropellantMass - p) / CM_RCS_FUEL_PER_TANK * 60.));
			lastPropellantMass = p;
		}

		// Propellant valve
		if (heliumValvesOpen && propellantValve.IsOpen() && heliumQuantity > 0) {
			if (purgeValvesOpen) {
				SetThrusters(NULL);
				SetPurgeLevel(true, simdt);
			} else {
				SetThrusters(source_prop);
				SetPurgeLevel(false, simdt);
			}
		} else {
			SetThrusters(NULL);
			SetPurgeLevel(false, simdt);
		}

		//Dump propellant. We don't differentiate between oxidizer and fuel yet.
		if (oxidizerDumpValvesOpen || fuelDumpValvesOpen)
		{
			double PropMass = our_vessel->GetPropellantMass(source_prop);

			//2.2 kg/s, just an estimate
			if (oxidizerDumpValvesOpen)
			{
				PropMass -= 2.2*simdt;
			}
			if (fuelDumpValvesOpen)
			{
				PropMass -= 2.2*simdt;
			}

			PropMass = max(0.0, PropMass);

			our_vessel->SetPropellantMass(source_prop, PropMass);

			if (heliumValvesOpen && purgeValvesOpen && heliumQuantity > 0) {
				heliumQuantity = max(0, heliumQuantity - (simdt / 15.));
			}
		}
	}
	// sprintf(oapiDebugString(), "heliumQuantity %f heliumTemp %f", heliumQuantity, heliumTank->GetTemp());
}

void CMRCSPropellantSource::SystemTimestep(double simdt) {

}

void CMRCSPropellantSource::OpenHeliumValves() {

	if (heliumValvesOpen) return;

	// Cool down helium tank by 10°F
	heliumTank->SetTemp(FahrenheitToKelvin(KelvinToFahrenheit(heliumTank->GetTemp()) - 10.));
	heliumValvesOpen = true;
}

void CMRCSPropellantSource::OpenPurgeValves() {

	purgeValvesOpen = true;
}

void CMRCSPropellantSource::SetThrusters(PROPELLANT_HANDLE ph) {

	for (int i = 0; i < 6; i++) {
		if (thrusters[i])
			our_vessel->SetThrusterResource(thrusters[i], ph);
	}
}

void CMRCSPropellantSource::SetPurgeLevel(bool on, double simdt) {

	for (int i = 0; i < 6; i++) {
		if (on && thrusters[i]) {
			if (our_vessel->GetCMRCSStateCommanded(thrusters[i])) {
				purgeLevel[i] = 1;
				// Purging takes 15s with 5 thrusters active per system
				heliumQuantity = max(0, heliumQuantity - (simdt / 75.));
			} else {
				purgeLevel[i] = 0;
			}
		} else {
			purgeLevel[i] = 0;
		}
	}
}

double CMRCSPropellantSource::GetHeliumTempF() {

	if (!our_vessel) return 0;	
	return KelvinToFahrenheit(heliumTank->GetTemp());
}

double CMRCSPropellantSource::GetHeliumPressurePSI() {

	if (!our_vessel) return 0;
	
	if (!heliumValvesOpen) {
		// 4150 psi at 75°F
		return heliumTank->GetTemp() * 13.973;
	} else {
		// 3400 at 65°F to 800 psi at 0°F, linear depending on propellant quantity
		double q = 1;
		if (source_prop) 
			q = our_vessel->GetPropellantMass(source_prop) / CM_RCS_FUEL_PER_TANK;
		q = (913.2 + q * 2486.8) / 291.5 * heliumTank->GetTemp();
		// Pressure decay during helium purge
		q = q * heliumQuantity;
		return q;
	}
}

double CMRCSPropellantSource::GetPropellantPressurePSI() {

	if (!our_vessel) return 0;
	
	if (!heliumValvesOpen) {
		// between 80 and 105 psi
		return 92.5;
	} else {
		// Pressure decay during helium purge
		return 294.5 * heliumQuantity;
	}
}

void CMRCSPropellantSource::PropellantSwitchToggled(PanelSwitchItem *s) {
	
	if (s->IsPowered()) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			propellantValve.SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			propellantValve.SetState(false);
		}
	}
}

void CMRCSPropellantSource::SaveState(FILEHANDLE scn) {

	int i;
	char buffer[100];

	// START_STRING is written in Saturn
	for (i = 0; i < 6; i++) {
		sprintf(buffer, "PURGELEVEL%i", i);
		papiWriteScenario_double(scn, buffer, purgeLevel[i]);
	}
	if (source_prop && our_vessel)
		papiWriteScenario_double(scn, "PROPELLANTMASS", our_vessel->GetPropellantMass(source_prop));
	papiWriteScenario_double(scn, "LASTPROPELLANTMASS", lastPropellantMass);
	papiWriteScenario_double(scn, "HELIUMQUANTITY", heliumQuantity);

	papiWriteScenario_bool(scn, "PROPELLANTVALVE_ISOPEN", propellantValve.IsOpen()); 
	papiWriteScenario_bool(scn, "HELIUMVALVESOPEN", heliumValvesOpen); 
	papiWriteScenario_bool(scn, "FUELINTERCONNECTVALVESOPEN", fuelInterconnectValvesOpen); 
	papiWriteScenario_bool(scn, "OXIDIZERINTERCONNECTVALVESOPEN", oxidizerInterconnectValvesOpen); 
	papiWriteScenario_bool(scn, "PURGEVALVESOPEN", purgeValvesOpen);
	papiWriteScenario_bool(scn, "FUELDUMPVALVESOPEN", fuelDumpValvesOpen);
	papiWriteScenario_bool(scn, "OXIDIZERDUMPVALVESOPEN", oxidizerDumpValvesOpen);

	oapiWriteLine(scn, CMRCSPROPELLANT_END_STRING);
}

void CMRCSPropellantSource::LoadState(FILEHANDLE scn) {

	char *line;
	bool isOpen;
	int i;
	double val;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, CMRCSPROPELLANT_END_STRING, sizeof(CMRCSPROPELLANT_END_STRING))) {
			return;
		
		} else if (!strnicmp (line, "PURGELEVEL", 10)) {			
			sscanf(line+10, "%i %lf", &i, &val);
			purgeLevel[i] = val;
		}
		papiReadScenario_double(line, "PROPELLANTMASS", propellantBuffer);
		papiReadScenario_double(line, "LASTPROPELLANTMASS", lastPropellantMass);
		papiReadScenario_double(line, "HELIUMQUANTITY", heliumQuantity);

		if (papiReadScenario_bool(line, "PROPELLANTVALVE_ISOPEN", isOpen)) propellantValve.SetState(isOpen); 
		papiReadScenario_bool(line, "HELIUMVALVESOPEN", heliumValvesOpen); 
		papiReadScenario_bool(line, "FUELINTERCONNECTVALVESOPEN", fuelInterconnectValvesOpen); 
		papiReadScenario_bool(line, "OXIDIZERINTERCONNECTVALVESOPEN", oxidizerInterconnectValvesOpen); 
		papiReadScenario_bool(line, "PURGEVALVESOPEN", purgeValvesOpen); 
		papiReadScenario_bool(line, "FUELDUMPVALVESOPEN", fuelDumpValvesOpen);
		papiReadScenario_bool(line, "OXIDIZERDUMPVALVESOPEN", oxidizerDumpValvesOpen);
	}
}

void CMRCSPropellantSource::CheckPropellantMass() {

	if (propellantBuffer != -1 && source_prop && our_vessel)
		our_vessel->SetPropellantMass(source_prop, propellantBuffer);
}
