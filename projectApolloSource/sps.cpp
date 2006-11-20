/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  CSM Service Propulsion System

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
  *	Revision 1.1  2006/11/13 14:47:34  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

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


SPSPropellantSource::SPSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p) : 
	PropellantSource(ph), DCPower(0, p) {

	oxidMass = -1;
	primOxidFlowValve = 0;
	secOxidFlowValve = 0;
	primTestStatus = 0;
	auxTestStatus = 0;
	primTestTimer = 0;
	heliumValveAOpen = false;
	heliumValveBOpen = false;
	propellantPressurePSI = 0;
	heliumPressurePSI = 0;
	nitrogenPressurePSI = 0;
	lastPropellantMassHeliumValvesClosed = -1;

	propellantMassToDisplay = SPS_DEFAULT_PROPELLANT;
	oxidMassToDisplay = SPS_DEFAULT_PROPELLANT * SPS_NORM_OXIDIZER_FLOW;
	propellantMaxMassToDisplay = SPS_DEFAULT_PROPELLANT;

	propellantInitialized = false;
	lastPropellantMass = 0;
}

SPSPropellantSource::~SPSPropellantSource() {
	// Nothing for now.
}

void SPSPropellantSource::Init(e_object *dc1, e_object *dc2, e_object *ac, h_Radiator *propline) {

	DCPower.WireToBuses(dc1, dc2);
	ACPower = ac;
	propellantLine = propline;
}

void SPSPropellantSource::Timestep(double simt, double simdt) {

	if (!our_vessel) return;

	// Propellant consumption
	double p, o, pmax;
	bool thrustOn = false;

	if (our_vessel->GetStage() < CSM_LEM_STAGE) {
		p = our_vessel->SM_FuelMass;
		o = p * SPS_NORM_OXIDIZER_FLOW;
		pmax = p;

		propellantPressurePSI = 175.0;
		heliumPressurePSI = 3600.0;
		nitrogenPressurePSI = 2500.0;
	
	} else if (!source_prop) {
		p = 0;
		o = 0;
		pmax = 1;

		propellantPressurePSI = 0;
		heliumPressurePSI = 0;
		nitrogenPressurePSI = 0;
		propellantInitialized = false;

	} else {
		p = our_vessel->GetPropellantMass(source_prop);
		pmax = our_vessel->GetPropellantMaxMass(source_prop);
		
		if (!propellantInitialized) {
			lastPropellantMass = p;
			if (oxidMass == -1) {
				oxidMass = p * SPS_NORM_OXIDIZER_FLOW;
			}
			if (lastPropellantMassHeliumValvesClosed == -1) {
				lastPropellantMassHeliumValvesClosed = p;
			}
			propellantInitialized = true;
		
		} else {
			// Thrust on?
			if (lastPropellantMass != p) {
				thrustOn = true;

				// Oxidizer consumption
				double of = SPS_NORM_OXIDIZER_FLOW * (1.0 + primOxidFlowValve + secOxidFlowValve);
				oxidMass -= (lastPropellantMass - p) * of;
				oxidMass = max(oxidMass, 0);

				lastPropellantMass = p;
			}
		}
		o = oxidMass;

		// Pressures
		if (p > 0 && o > 0 && p - o > 0) {
			if (heliumValveAOpen || heliumValveBOpen) {
				propellantPressurePSI = 175.0;
			} else {
				propellantPressurePSI = (pmax - lastPropellantMassHeliumValvesClosed + 0.001) /
										(pmax - p + 0.001) * 175.0;
			}
		} else {
			propellantPressurePSI = 0;
		}
		heliumPressurePSI = 3400.0 * (p / pmax) * (p / pmax) + 200.0;
		nitrogenPressurePSI = 700.0 * (p / pmax) + 1800.0;
	}

	// Propellant masses for display
	if (IsGaugingPowered()) {

		propellantMassToDisplay = p;
		oxidMassToDisplay = o;
		propellantMaxMassToDisplay = pmax;

		// Prim test mode
		if (primTestStatus && !our_vessel->SPSPugModeSwitch.IsDown()) {
			 propellantMassToDisplay = ((1.0 + primTestStatus * 0.05) * (propellantMassToDisplay - oxidMassToDisplay)) + ((1.0 + primTestStatus * 0.07) * oxidMassToDisplay);
			 oxidMassToDisplay *= (1.0 + primTestStatus * 0.07);
		}

		// Aux test mode
		if (auxTestStatus && our_vessel->SPSPugModeSwitch.IsDown()) {
			 propellantMassToDisplay *= (1.0 + auxTestStatus * 0.06);
			 oxidMassToDisplay *= (1.0 + auxTestStatus * 0.06);
		}
	}

	// Quantity test mode
	if (our_vessel->SPSTestSwitch.IsCenter() || our_vessel->SPSPugModeSwitch.IsDown() || !IsGaugingPowered()) {
		primTestTimer = 0;

	} else if (primTestTimer) {
		primTestTimer -= simdt;
		if (primTestTimer <= 0) {
			if (our_vessel->SPSTestSwitch.IsUp()) {
				primTestStatus += 1.0;
				primTestStatus = min(primTestStatus, 1.0);

			} else if (our_vessel->SPSTestSwitch.IsDown()) {
				primTestStatus -= 1.0;
				primTestStatus = max(primTestStatus, -1.0);
			}		
			primTestTimer = 0;
		}
	}

	// Helium valve A
	if (our_vessel->HeValveMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && 
	   (our_vessel->SPSHeliumValveASwitch.IsDown() || (thrustOn && our_vessel->SPSHeliumValveASwitch.IsUp()))) {
		heliumValveAOpen = true;

	} else {
		if (heliumValveAOpen && !heliumValveBOpen) {
			lastPropellantMassHeliumValvesClosed = p;
		}
		heliumValveAOpen = false;
	}

	// Helium valve B
	if (our_vessel->HeValveMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && 
	   (our_vessel->SPSHeliumValveBSwitch.IsDown() || (thrustOn && our_vessel->SPSHeliumValveBSwitch.IsUp()))) {
		heliumValveBOpen = true;

	} else {
		if (heliumValveBOpen && !heliumValveAOpen) {
			lastPropellantMassHeliumValvesClosed = p;
		}
		heliumValveBOpen = false;
	}

	// Drive oxidizer flow valves
	if (IsGaugingPowered()) {
		// Only working while thrust on or during test mode
		if (thrustOn || !our_vessel->SPSTestSwitch.IsCenter() || primTestStatus != 0 || auxTestStatus != 0) {
			// primary valve, max 3%, min 3.5%
			if (our_vessel->SPSOxidFlowValveSelectorSwitch.IsUp()) {
				if (our_vessel->SPSOxidFlowValveSwitch.IsCenter()) {
					if (primOxidFlowValve > 0) {
						primOxidFlowValve -= 0.03 * simdt / 3.5;
						primOxidFlowValve = max(primOxidFlowValve, 0);
					} else if (primOxidFlowValve < 0) {
						primOxidFlowValve += 0.035 * simdt / 3.5;
						primOxidFlowValve = min(primOxidFlowValve, 0);
					}
				
				} else if (our_vessel->SPSOxidFlowValveSwitch.IsUp()) {
					if (primOxidFlowValve < 0.03) {
						primOxidFlowValve += 0.03 * simdt / 3.5;
						primOxidFlowValve = min(primOxidFlowValve, 0.03);
					}

				} else if (our_vessel->SPSOxidFlowValveSwitch.IsDown()) {
					if (primOxidFlowValve > -0.035) {
						primOxidFlowValve -= 0.035 * simdt / 3.5;
						primOxidFlowValve = max(primOxidFlowValve, -0.035);
					}
				}
			
			// secondary valve, max 6%, min 7%
			} else {
				if (our_vessel->SPSOxidFlowValveSwitch.IsCenter()) {
					if (secOxidFlowValve > 0) {
						secOxidFlowValve -= 0.06 * simdt / 3.5;
						secOxidFlowValve = max(secOxidFlowValve, 0);
					} else if (secOxidFlowValve < 0) {
						secOxidFlowValve += 0.07 * simdt / 3.5;
						secOxidFlowValve = min(secOxidFlowValve, 0);
					}
				
				} else if (our_vessel->SPSOxidFlowValveSwitch.IsUp()) {
					if (secOxidFlowValve < 0.06) {
						secOxidFlowValve += 0.06 * simdt / 3.5;
						secOxidFlowValve = min(secOxidFlowValve, 0.06);
					}

				} else if (our_vessel->SPSOxidFlowValveSwitch.IsDown()) {
					if (secOxidFlowValve > -0.07) {
						secOxidFlowValve -= 0.07 * simdt / 3.5;
						secOxidFlowValve = max(secOxidFlowValve, -0.07);
					}
				}
			}
		}
	}

/*	sprintf(oapiDebugString(), "Prop %.1f Fuel %.1f Oxid %.1f Ratio %.3f press %.1f primOxidFV %.4f secOxidV %.4f primTest %.1f primTimer %.1f auxTest %.1f", 
		p, p - oxidMass, oxidMass, oxidMass / (p - oxidMass), propellantPressurePSI, primOxidFlowValve, 
		secOxidFlowValve, primTestStatus, primTestTimer, auxTestStatus);
*/
}

void SPSPropellantSource::SystemTimestep(double simdt) {

	if (heliumValveAOpen && our_vessel->HeValveMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) 
		our_vessel->HeValveMnACircuitBraker.DrawPower(91.2);

	if (heliumValveBOpen && our_vessel->HeValveMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) 
		our_vessel->HeValveMnBCircuitBraker.DrawPower(91.2);
}

void SPSPropellantSource::SPSTestSwitchToggled() {

	if (!IsGaugingPowered()) return;

	// Aux mode
	if (our_vessel->SPSPugModeSwitch.IsDown()) {
		if (our_vessel->SPSTestSwitch.IsUp()) {
			auxTestStatus += 1.0;
			auxTestStatus = min(auxTestStatus, 1.0);

		} else if (our_vessel->SPSTestSwitch.IsDown()) {
			auxTestStatus -= 1.0;
			auxTestStatus = max(auxTestStatus, -1.0);
		}

	// Prim mode
	} else if (!our_vessel->SPSTestSwitch.IsCenter()) {
		primTestTimer = 4.0;
	}
}

bool SPSPropellantSource::IsOxidFlowValveMin() {

	if (!our_vessel) return false;
	if (our_vessel->GetStage() > CSM_LEM_STAGE) return false;

	if (our_vessel->SPSOxidFlowValveSelectorSwitch.IsUp()) return (primOxidFlowValve == -0.035);	
	else return (secOxidFlowValve == -0.07);	
}

bool SPSPropellantSource::IsOxidFlowValveMax() {

	if (!our_vessel) return false;
	if (our_vessel->GetStage() > CSM_LEM_STAGE) return false;

	if (our_vessel->SPSOxidFlowValveSelectorSwitch.IsUp()) return (primOxidFlowValve == 0.03);	
	else return (secOxidFlowValve == 0.06);	
}

double SPSPropellantSource::GetFuelPercent() {

	return (propellantMassToDisplay - oxidMassToDisplay) / (propellantMaxMassToDisplay * (1.0 - SPS_NORM_OXIDIZER_FLOW));
}

double SPSPropellantSource::GetOxidPercent() {

	return oxidMassToDisplay / (propellantMaxMassToDisplay * SPS_NORM_OXIDIZER_FLOW);
}

double SPSPropellantSource::GetOxidUnbalanceLB() {

	return (oxidMassToDisplay - (propellantMassToDisplay * SPS_NORM_OXIDIZER_FLOW)) * 2.205;
}

bool SPSPropellantSource::IsGaugingPowered() {

	if (DCPower.Voltage() < SP_MIN_DCVOLTAGE) return false;

	if (!ACPower) return false;
	if (ACPower->Voltage() < SP_MIN_ACVOLTAGE) return false;

	return true;
}

double SPSPropellantSource::GetPropellantLineTempF() {

	if (!our_vessel) return 0;
	if (our_vessel->GetStage() > CSM_LEM_STAGE) return 0;

	
	return KelvinToFahrenheit(propellantLine->GetTemp());
}

void SPSPropellantSource::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, SPSPROPELLANT_START_STRING);
	oapiWriteScenario_float(scn, "OXIDMASS", oxidMass);
	oapiWriteScenario_float(scn, "PRIMOXIDFLOWVALVE", primOxidFlowValve);
	oapiWriteScenario_float(scn, "SECOXIDFLOWVALVE", secOxidFlowValve);
	oapiWriteScenario_float(scn, "PRIMTESTSTATUS", primTestStatus);
	oapiWriteScenario_float(scn, "PRIMTESTTIMER", primTestTimer);
	oapiWriteScenario_float(scn, "AUXTESTSTATUS", auxTestStatus);
	oapiWriteScenario_float(scn, "PROPELLANTMASSTODISPLAY", propellantMassToDisplay);
	oapiWriteScenario_float(scn, "OXIDMASSTODISPLAY", oxidMassToDisplay);
	oapiWriteScenario_float(scn, "PROPELLANTMAXMASSTODISPLAY", propellantMaxMassToDisplay);
	oapiWriteScenario_float(scn, "PROPELLANTPRESSUREPSI", propellantPressurePSI);
	oapiWriteScenario_float(scn, "HELIUMPRESSUREPSI", heliumPressurePSI);
	oapiWriteScenario_float(scn, "NITROGENPRESSUREPSI", nitrogenPressurePSI);
	oapiWriteScenario_float(scn, "LASTPROPELLANTMASSHELIUMVALVESCLOSED", lastPropellantMassHeliumValvesClosed);
	oapiWriteScenario_int(scn, "HELIUMVALVEAOPEN", (heliumValveAOpen ? 1 : 0));
	oapiWriteScenario_int(scn, "HELIUMVALVEBOPEN", (heliumValveBOpen ? 1 : 0));
	oapiWriteLine(scn, SPSPROPELLANT_END_STRING);
}

void SPSPropellantSource::LoadState(FILEHANDLE scn) {

	char *line;
	int i;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SPSPROPELLANT_END_STRING, sizeof(SPSPROPELLANT_END_STRING)))
			return;
		else if (!strnicmp (line, "LASTPROPELLANTMASSHELIUMVALVESCLOSED", 36)) {
			sscanf (line+36, "%lf", &lastPropellantMassHeliumValvesClosed);
		}
		else if (!strnicmp (line, "PROPELLANTMAXMASSTODISPLAY", 26)) {
			sscanf (line+26, "%lf", &propellantMaxMassToDisplay);
		}
		else if (!strnicmp (line, "PROPELLANTMASSTODISPLAY", 23)) {
			sscanf (line+23, "%lf", &propellantMassToDisplay);
		}
		else if (!strnicmp (line, "PROPELLANTPRESSUREPSI", 21)) {
			sscanf (line+21, "%lf", &propellantPressurePSI);
		}
		else if (!strnicmp (line, "NITROGENPRESSUREPSI", 19)) {
			sscanf (line+19, "%lf", &nitrogenPressurePSI);
		}
		else if (!strnicmp (line, "HELIUMPRESSUREPSI", 17)) {
			sscanf (line+17, "%lf", &heliumPressurePSI);
		}
		else if (!strnicmp (line, "OXIDMASSTODISPLAY", 17)) {
			sscanf (line+17, "%lf", &oxidMassToDisplay);
		}
		else if (!strnicmp (line, "PRIMOXIDFLOWVALVE", 17)) {
			sscanf (line+17, "%lf", &primOxidFlowValve);
		}
		else if (!strnicmp (line, "SECOXIDFLOWVALVE", 16)) {
			sscanf (line+16, "%lf", &secOxidFlowValve);
		}
		else if (!strnicmp (line, "HELIUMVALVEAOPEN", 16)) {
			sscanf (line+16, "%d", &i);
			heliumValveAOpen = (i != 0);
		}
		else if (!strnicmp (line, "HELIUMVALVEBOPEN", 16)) {
			sscanf (line+16, "%d", &i);
			heliumValveBOpen = (i != 0);
		}
		else if (!strnicmp (line, "PRIMTESTSTATUS", 14)) {
			sscanf (line+14, "%lf", &primTestStatus);
		}
		else if (!strnicmp (line, "PRIMTESTTIMER", 13)) {
			sscanf (line+13, "%lf", &primTestTimer);
		}
		else if (!strnicmp (line, "AUXTESTSTATUS", 13)) {
			sscanf (line+13, "%lf", &auxTestStatus);
		}
		else if (!strnicmp (line, "OXIDMASS", 8)) {
			sscanf (line+8, "%lf", &oxidMass);
		}
	}
}


SPSEngine::SPSEngine(THRUSTER_HANDLE &sps) :
	spsThruster(sps) {

	injectorValves12Open = false;
	injectorValves34Open = false;
	saturn = 0;
	enforceBurn = false;
	engineOnCommanded = false;
}

SPSEngine::~SPSEngine() {
	// Nothing for now.
}

void SPSEngine::Init(Saturn *s) {

	saturn = s;
}

void SPSEngine::Timestep(double simt, double simdt) {

	if (!saturn) return;

	// Prevalves
	bool injectorPreValveAOpen = false;
	bool injectorPreValveBOpen = false;

	if (saturn->HeValveMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && saturn->dVThrust1Switch.IsUp()) {
		injectorPreValveAOpen = true;
	}

	if (saturn->HeValveMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && saturn->dVThrust2Switch.IsUp()) {
		injectorPreValveBOpen = true;
	}

	// SPS ready signal to CMC
	if (saturn->GetStage() <= CSM_LEM_STAGE  && (saturn->dVThrust1Switch.Voltage() > SP_MIN_DCVOLTAGE || saturn->dVThrust2Switch.Voltage() > SP_MIN_DCVOLTAGE)) {
		if (!saturn->agc.GetInputChannelBit(030, 3))
			saturn->agc.SetInputChannelBit(030, 3, true);
	} else {
		if (saturn->agc.GetInputChannelBit(030, 3))
			saturn->agc.SetInputChannelBit(030, 3, false);
	}

	//
	// Thrust on/off logic
	//

	bool thrustOn = false;

	// SPS thrust direct on mode
	if (saturn->SPSswitch.IsUp()) {
		thrustOn = true;
	}

	// CMC mode
	if (saturn->SCContSwitch.IsUp()) {
		// Check i/o channel
		ChannelValue11 val11;
		val11.Value = saturn->agc.GetOutputChannel(011);
		if (val11.Bits.EngineOnOff) {
			thrustOn = true;
		}
	}

	// TODO SCS dV mode

	//
	// Injector valves
	//

	if (thrustOn && saturn->dVThrust1Switch.Voltage() > SP_MIN_DCVOLTAGE) {
		if (injectorPreValveAOpen && !injectorValves12Open) {
			injectorValves12Open = true;
		}
	} else {
		injectorValves12Open = false;
	}

	if (thrustOn && saturn->dVThrust2Switch.Voltage() > SP_MIN_DCVOLTAGE) {
		if (injectorPreValveBOpen && !injectorValves34Open) {
			injectorValves34Open = true;
		}
	} else {
		injectorValves34Open = false;
	}

	//
	// Engine control
	//
	
	if (saturn->GetStage() == CSM_LEM_STAGE && saturn->GetSPSPropellant()->Handle() && spsThruster) {
		if (injectorValves12Open || injectorValves34Open || enforceBurn) {
			// Burn engine
			saturn->SetThrusterResource(spsThruster, saturn->GetSPSPropellant()->Handle());
			// Thrust decay if propellant pressure below 170 psi 
			double thrust = min(1, saturn->GetSPSPropellant()->GetPropellantPressurePSI() / 170.0);
			saturn->SetThrusterLevel(spsThruster, thrust);
			saturn->rjec.SPSActive = true;
			engineOnCommanded = true;

		} else {
			if (saturn->Realism) {
				// Stop engine
				saturn->SetThrusterResource(spsThruster, NULL);			
				saturn->SetThrusterLevel(spsThruster, 0);
				saturn->rjec.SPSActive = false;
				engineOnCommanded = false;
			} else {
				// Manual engine if REALISM 0
				saturn->SetThrusterResource(spsThruster, saturn->GetSPSPropellant()->Handle());

				// Reset automatically commanded thrust
				if (engineOnCommanded) {
					saturn->SetThrusterLevel(spsThruster, 0);
					engineOnCommanded = false;
				}
				if (saturn->GetThrusterLevel(spsThruster) > 0) {
					saturn->rjec.SPSActive = true;
					// Show all injector valves open
					injectorValves12Open = true;
					injectorValves34Open = true;
				} else {
					saturn->rjec.SPSActive = false;
				}
			}
		}
	} else {
		saturn->rjec.SPSActive = false;
	}
}

void SPSEngine::SystemTimestep(double simdt) {

	if (injectorValves12Open && saturn->dVThrust1Switch.Voltage() > SP_MIN_DCVOLTAGE) 
		saturn->dVThrust1Switch.DrawPower(76.3);

	if (injectorValves34Open && saturn->dVThrust2Switch.Voltage() > SP_MIN_DCVOLTAGE) 
		saturn->dVThrust2Switch.DrawPower(76.6);
}

double SPSEngine::GetChamberPressurePSI() {

	if (!saturn) return 0;
	if (saturn->GetStage() != CSM_LEM_STAGE) return 0;
	if (!spsThruster) return 0;

	//
	// Nominal chamber pressure at full thrust is 100 psi, 
	// so we use the thrust level percentage for now.
	//

	return saturn->GetThrusterLevel(spsThruster) * 100.0;
}

void SPSEngine::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, SPSENGINE_START_STRING);
	oapiWriteScenario_int(scn, "INJECTORVALVES12OPEN", (injectorValves12Open ? 1 : 0));
	oapiWriteScenario_int(scn, "INJECTORVALVES34OPEN", (injectorValves34Open ? 1 : 0));
	oapiWriteScenario_int(scn, "ENFORCEBURN", (enforceBurn ? 1 : 0));
	oapiWriteScenario_int(scn, "ENGINEONCOMMANDED", (engineOnCommanded ? 1 : 0));
	oapiWriteLine(scn, SPSENGINE_END_STRING);
}

void SPSEngine::LoadState(FILEHANDLE scn) {

	char *line;
	int i;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SPSENGINE_END_STRING, sizeof(SPSENGINE_END_STRING))) {
			return;
		}
		else if (!strnicmp (line, "INJECTORVALVES12OPEN", 20)) {
			sscanf (line+20, "%d", &i);
			injectorValves12Open = (i != 0);
		}
		else if (!strnicmp (line, "INJECTORVALVES34OPEN", 20)) {
			sscanf (line+20, "%d", &i);
			injectorValves34Open = (i != 0);
		}
		else if (!strnicmp (line, "ENGINEONCOMMANDED", 17)) {
			sscanf (line+17, "%d", &i);
			engineOnCommanded = (i != 0);
		}
		else if (!strnicmp (line, "ENFORCEBURN", 11)) {
			sscanf (line+11, "%d", &i);
			enforceBurn = (i != 0);
		}
	}
}