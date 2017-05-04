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


SPSPropellantSource::SPSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p) : 
	PropellantSource(ph), DCPower(0, p) {

	oxidMass = -1;
	primOxidFlowValve = 0.03;
	secOxidFlowValve = 0;
	primTestStatus = 0;
	auxTestStatus = 0;
	primTestTimer = 0;
	heliumValveAOpen = false;
	heliumValveBOpen = false;
	propellantPressurePSI = 0;
	heliumPressurePSI = 0;
	lastPropellantMassHeliumValvesClosed = -1;

	propellantMassToDisplay = SPS_DEFAULT_PROPELLANT;
	oxidMassToDisplay = SPS_DEFAULT_PROPELLANT * SPS_NORM_OXIDIZER_FLOW;
	propellantMaxMassToDisplay = SPS_DEFAULT_PROPELLANT;

	propellantInitialized = false;
	lastPropellantMass = 0;
	propellantBuffer = -1;
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
		pmax = SPS_DEFAULT_PROPELLANT;

		propellantPressurePSI = 175.0;
		heliumPressurePSI = 3600.0;
	
	} else if (!source_prop) {
		p = 0;
		o = 0;
		pmax = 1;

		propellantPressurePSI = 0;
		heliumPressurePSI = 0;
		propellantInitialized = false;

	} else {
		p = our_vessel->GetPropellantMass(source_prop);
		pmax = SPS_DEFAULT_PROPELLANT; 
		
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

		//
		// Helium pressure is calculated by a quadratic approximation
		// Data points are 3600 psi with full tank (AOH), 200 psi with empty tank (guessed, otherwise it
		// would be lower than prop. pressure) and 1500 psi at 39% (Apollo 12 flight journal, GET 081:51:57)
		//

		double pMaxForPressures = our_vessel->GetPropellantMaxMass(source_prop);
		heliumPressurePSI = 100.0 * (p / pMaxForPressures) * (p / pMaxForPressures) + 3300.0 * (p / pMaxForPressures) + 200.0;
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

	if (our_vessel->GetStage() > CSM_LEM_STAGE) return;

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
	papiWriteScenario_double(scn, "OXIDMASS", oxidMass);
	papiWriteScenario_double(scn, "PRIMOXIDFLOWVALVE", primOxidFlowValve);
	papiWriteScenario_double(scn, "SECOXIDFLOWVALVE", secOxidFlowValve);
	papiWriteScenario_double(scn, "PRIMTESTSTATUS", primTestStatus);
	papiWriteScenario_double(scn, "PRIMTESTTIMER", primTestTimer);
	papiWriteScenario_double(scn, "AUXTESTSTATUS", auxTestStatus);
	papiWriteScenario_double(scn, "PROPMASSTODISPLAY", propellantMassToDisplay);
	papiWriteScenario_double(scn, "OXIDMASSTODISPLAY", oxidMassToDisplay);
	papiWriteScenario_double(scn, "PROPELLANTMAXMASSTODISPLAY", propellantMaxMassToDisplay);
	papiWriteScenario_double(scn, "PROPELLANTPRESSUREPSI", propellantPressurePSI);
	papiWriteScenario_double(scn, "HELIUMPRESSUREPSI", heliumPressurePSI);
	papiWriteScenario_double(scn, "LASTPROPELLANTMASSHELIUMVALVESCLOSED", lastPropellantMassHeliumValvesClosed);
	if (source_prop && our_vessel)
		papiWriteScenario_double(scn, "PROPELLANTMASS", our_vessel->GetPropellantMass(source_prop));
			
	oapiWriteScenario_int(scn, "HELIUMVALVEAOPEN", (heliumValveAOpen ? 1 : 0));
	oapiWriteScenario_int(scn, "HELIUMVALVEBOPEN", (heliumValveBOpen ? 1 : 0));
	oapiWriteLine(scn, SPSPROPELLANT_END_STRING);
}

void SPSPropellantSource::LoadState(FILEHANDLE scn) {

	int i;
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SPSPROPELLANT_END_STRING, sizeof(SPSPROPELLANT_END_STRING)))
			return;
		else if (!strnicmp (line, "LASTPROPELLANTMASSHELIUMVALVESCLOSED", 36)) {
			sscanf (line+36, "%lf", &lastPropellantMassHeliumValvesClosed);
		}
		else if (!strnicmp (line, "PROPELLANTMAXMASSTODISPLAY", 26)) {
			sscanf (line+26, "%lf", &propellantMaxMassToDisplay);
		}
		else if (!strnicmp (line, "PROPELLANTPRESSUREPSI", 21)) {
			sscanf (line+21, "%lf", &propellantPressurePSI);
		}
		else if (!strnicmp (line, "PROPMASSTODISPLAY", 17)) {
			sscanf (line+17, "%lf", &propellantMassToDisplay);
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
		else if (!strnicmp (line, "PROPELLANTMASS", 14)) {
			sscanf (line+14, "%lf", &propellantBuffer);
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

void SPSPropellantSource::CheckPropellantMass() {

	if (propellantBuffer != -1 && source_prop && our_vessel)
		our_vessel->SetPropellantMass(source_prop, propellantBuffer);
}


SPSEngine::SPSEngine(THRUSTER_HANDLE &sps) :
	spsThruster(sps) {

	thrustOn = false;
	injectorValves12Open = false;
	injectorValves34Open = false;
	saturn = 0;
	enforceBurn = false;
	engineOnCommanded = false;
	nitrogenPressureAPSI = 2500.0;
	nitrogenPressureBPSI = 2500.0;
	cmcErrorCountersEnabled = false;
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
		if (!saturn->agc.GetInputChannelBit(030, SPSReady))
			saturn->agc.SetInputChannelBit(030, SPSReady, true);
	} else {
		if (saturn->agc.GetInputChannelBit(030, SPSReady))
			saturn->agc.SetInputChannelBit(030, SPSReady, false);
	}

	//
	// Thrust on/off logic
	//

	if (saturn->dVThrust1Switch.Voltage() > SP_MIN_DCVOLTAGE || saturn->dVThrust2Switch.Voltage() > SP_MIN_DCVOLTAGE) {
		// SCS dV mode
		/// \todo SC CONT switch is supplied by SCS LOGIC BUS 3
		/// \todo TVC CW is supplied by SCS LOGIC BUS 2
		if (saturn->SCContSwitch.IsDown() || saturn->THCRotary.IsClockwise()) {
			if (!saturn->ems.IsdVMode()) {
				thrustOn = false;
			
			} else if (saturn->ems.GetdVRangeCounter() < 0) {
				thrustOn = false;

			} else if (saturn->ThrustOnButton.GetState() == 1) {
				thrustOn = true;
			}
		}

		// CMC mode
		/// \todo SC CONT switch is supplied by G/N IMU PWR
		if (saturn->SCContSwitch.IsUp() && !saturn->THCRotary.IsClockwise()) {
			// Check i/o channel
			ChannelValue val11;
			val11 = saturn->agc.GetOutputChannel(011);
			if (val11[EngineOn]) {
				thrustOn = true;
			} else {
				thrustOn = false;
			}
		}

		// SPS thrust direct on mode
		if (saturn->SPSswitch.IsUp()) {
			thrustOn = true;
		}
	} else {
		thrustOn = false;
	}

	//
	// Injector valves
	//

	if (saturn->GetStage() <= CSM_LEM_STAGE) {
		if (thrustOn && saturn->dVThrust1Switch.Voltage() > SP_MIN_DCVOLTAGE) {
			if (injectorPreValveAOpen && !injectorValves12Open && nitrogenPressureAPSI > 400.0) {	// N2 pressure condition see http://www.history.nasa.gov/alsj/a11/a11transcript_pao.htm
				injectorValves12Open = true;
				nitrogenPressureAPSI -= 50.0;	// Average pressure decay, see Apollo 11 Mission report, 16.1.1
			}
		} else {
			injectorValves12Open = false;
		}

		if (thrustOn && saturn->dVThrust2Switch.Voltage() > SP_MIN_DCVOLTAGE) {
			if (injectorPreValveBOpen && !injectorValves34Open && nitrogenPressureBPSI > 400.0) {
				injectorValves34Open = true;
				nitrogenPressureBPSI -= 50.0;
			}
		} else {
			injectorValves34Open = false;
		}
	} else {
		injectorValves12Open = false;
		injectorValves34Open = false;
		nitrogenPressureAPSI = 0;
		nitrogenPressureBPSI = 0;
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
			saturn->rjec.SetSPSActive(true);
			engineOnCommanded = true;

		} else {
			// Stop engine
			saturn->SetThrusterResource(spsThruster, NULL);
			saturn->SetThrusterLevel(spsThruster, 0);
			saturn->rjec.SetSPSActive(false);
			engineOnCommanded = false;
		}
	} else {
		saturn->rjec.SetSPSActive(false);
	}

	//
	// TVC gimbal actuators
	//

	// Get BMAG1 attitude errors
	VECTOR3 error = saturn->bmag1.GetAttitudeError();

	if (error.x > 0) { // Positive Error
		if (error.x > PI) { 
			error.x = -(TWO_PI - error.x); 
		}
	} else {		  // Negative Error
		if (error.x < -PI) {
			error.x = TWO_PI + error.x;
		}
	}
	if (error.y > 0) { 
		if (error.y > PI) { 
			error.y = TWO_PI - error.y; 
		} else {
			error.y = -error.y; 
		}
	} else {
		if (error.y < -PI) {
			error.y = -(TWO_PI + error.y); 
		} else { 
			error.y = -error.y;
		}
	}
	if (error.z > 0) { 
		if (error.z > PI){ 
			error.z = -(TWO_PI - error.z); 
		}
	} else {
		if (error.z < -PI) {
			error.z = TWO_PI + error.z; 
		}
	}
	// Now adjust for rotation
	if (SCS_INERTIAL_BMAGS)
		error = saturn->eda.AdjustErrorsForRoll(saturn->bmag1.GetAttitude(), error);

	// TVC SCS automatic mode only when BMAG 1 uncaged and powered
	if (!saturn->bmag1.IsPowered() || saturn->bmag1.IsUncaged().x == 0) error.x = 0;
	if (!saturn->bmag1.IsPowered() || saturn->bmag1.IsUncaged().y == 0) error.y = 0;
	if (!saturn->bmag1.IsPowered() || saturn->bmag1.IsUncaged().z == 0) error.z = 0;

	// Do time step
	pitchGimbalActuator.Timestep(simt, simdt, error.y, saturn->gdc.rates.x, saturn->eca.rhc_ac_y);
	yawGimbalActuator.Timestep(simt, simdt, -error.z, -saturn->gdc.rates.y, saturn->eca.rhc_ac_z);

	if (saturn->GetStage() == CSM_LEM_STAGE && spsThruster) {
		// Directions X,Y,Z = YAW (+ = left),PITCH (+ = DOWN),FORE/AFT
		VECTOR3 spsvector;
		spsvector.x = (yawGimbalActuator.GetPosition() + SPS_YAW_OFFSET) * RAD; // Convert deg to rad
		spsvector.y = (pitchGimbalActuator.GetPosition() + SPS_PITCH_OFFSET) * RAD;
		spsvector.z = 1;
		saturn->SetThrusterDir(spsThruster, spsvector);
	}
}

void SPSEngine::SystemTimestep(double simdt) {

	if (saturn->GetStage() > CSM_LEM_STAGE) return;

	if (injectorValves12Open && saturn->dVThrust1Switch.Voltage() > SP_MIN_DCVOLTAGE) 
		saturn->dVThrust1Switch.DrawPower(76.3);

	if (injectorValves34Open && saturn->dVThrust2Switch.Voltage() > SP_MIN_DCVOLTAGE) 
		saturn->dVThrust2Switch.DrawPower(76.6);

	pitchGimbalActuator.SystemTimestep(simdt);
	yawGimbalActuator.SystemTimestep(simdt);
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
	oapiWriteScenario_int(scn, "THRUSTON", (thrustOn ? 1 : 0));
	oapiWriteScenario_int(scn, "INJECTORVALVES12OPEN", (injectorValves12Open ? 1 : 0));
	oapiWriteScenario_int(scn, "INJECTORVALVES34OPEN", (injectorValves34Open ? 1 : 0));
	oapiWriteScenario_int(scn, "ENFORCEBURN", (enforceBurn ? 1 : 0));
	oapiWriteScenario_int(scn, "ENGINEONCOMMANDED", (engineOnCommanded ? 1 : 0));
	papiWriteScenario_double(scn, "NITROGENPRESSUREAPSI", nitrogenPressureAPSI);
	papiWriteScenario_double(scn, "NITROGENPRESSUREBPSI", nitrogenPressureBPSI);
	papiWriteScenario_bool(scn, "CMCERRORCOUNTERSENABLED", cmcErrorCountersEnabled);
	oapiWriteLine(scn, SPSENGINE_END_STRING);
}

void SPSEngine::LoadState(FILEHANDLE scn) {

	char *line;
	int i;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SPSENGINE_END_STRING, sizeof(SPSENGINE_END_STRING))) {
			return;
		}
		else if (!strnicmp (line, "THRUSTON", 8)) {
			sscanf (line+8, "%d", &i);
			thrustOn = (i != 0);
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
		else if (!strnicmp (line, "NITROGENPRESSUREAPSI", 20)) {
			sscanf (line+20, "%lf", &nitrogenPressureAPSI);
		}
		else if (!strnicmp (line, "NITROGENPRESSUREBPSI", 20)) {
			sscanf (line+20, "%lf", &nitrogenPressureBPSI);
		}
		papiReadScenario_bool(line, "CMCERRORCOUNTERSENABLED", cmcErrorCountersEnabled);
	}
}


SPSGimbalActuator::SPSGimbalActuator() {

	position = 0;
	commandedPosition = 0;
	cmcPosition = 0;
	scsPosition = 0;
	lastAttitudeError = 0;
	activeSystem = 1;
	motor1Running = false;
	motor2Running = false;

	saturn = 0;
	tvcGimbalDriveSwitch = 0;
	gimbalMotor1Switch = 0;
	gimbalMotor2Switch = 0;
	motor1Source = 0;
	motor1StartSource = 0;
	motor2Source = 0;
	motor2StartSource = 0;
	trimThumbwheel = 0;
	scsTvcModeSwitch = 0;
	CGSwitch = 0;
}

SPSGimbalActuator::~SPSGimbalActuator() {
	// Nothing for now.
}

void SPSGimbalActuator::Init(Saturn *s, ThreePosSwitch *driveSwitch, ThreePosSwitch *m1Switch, ThreePosSwitch *m2Switch,
	                         e_object *m1Source, e_object *m1StartSource, e_object *m2Source, e_object *m2StartSource,
							 ThumbwheelSwitch *tThumbwheel, ThreePosSwitch* modeSwitch, AGCIOSwitch* csmlmcogSwitch) {

	saturn = s;
	tvcGimbalDriveSwitch = driveSwitch;
	gimbalMotor1Switch = m1Switch;
	gimbalMotor2Switch = m2Switch;
	motor1Source = m1Source;
	motor1StartSource = m1StartSource;
	motor2Source = m2Source;
	motor2StartSource = m2StartSource;
	trimThumbwheel = tThumbwheel;
	scsTvcModeSwitch = modeSwitch;
	CGSwitch = csmlmcogSwitch;
}

void SPSGimbalActuator::Timestep(double simt, double simdt, double attitudeError, double attitudeRate, int rhcAxis) {

	if (!saturn) return;

	// After CM/SM separation
	if (saturn->GetStage() > CSM_LEM_STAGE) {
		position = 0;
		return;
	}

	//
	// Motors
	//

	if (motor1Running) {
		if (gimbalMotor1Switch->IsDown() || motor1Source->Voltage() < SP_MIN_DCVOLTAGE) {
			motor1Running = false;
		}
	} else {
		if (gimbalMotor1Switch->IsUp() && motor1Source->Voltage() > SP_MIN_DCVOLTAGE && 
			motor1StartSource->Voltage() > SP_MIN_DCVOLTAGE ) {
			motor1Running = true;
		}
	}

	if (motor2Running) {
		if (gimbalMotor2Switch->IsDown() || motor2Source->Voltage() < SP_MIN_DCVOLTAGE) {
			motor2Running = false;
		}
	} else {
		if (gimbalMotor2Switch->IsUp() && motor2Source->Voltage() > SP_MIN_DCVOLTAGE && 
			motor2StartSource->Voltage() > SP_MIN_DCVOLTAGE ) {
			motor2Running = true;
		}
	}

	// sprintf(oapiDebugString(), "Motor1 %d Motor2 %d", motor1Running, motor2Running);

	//
	// Process commanded position
	//

	if (saturn->SCContSwitch.IsUp() && !saturn->THCRotary.IsClockwise()) {
		// CMC mode
		commandedPosition = cmcPosition;

	} else {		
		// SCS modes
		double rhcPercent = 0;
		if (rhcAxis < 28673) { 
			rhcPercent = (28673. - (double)rhcAxis) / 28673.; 
		}
		if (rhcAxis > 36863) {  
			rhcPercent = (36863. - (double)rhcAxis) / 36863.;
		}

		// AUTO
		if (scsTvcModeSwitch->IsUp() && !(saturn->SCContSwitch.IsDown() && saturn->THCRotary.IsClockwise())) {
			scsPosition = attitudeError * DEG + (attitudeError - lastAttitudeError) / simdt * DEG;
			lastAttitudeError = attitudeError;

		// ACCEL CMD
		} else if (scsTvcModeSwitch->IsDown()) {
			scsPosition += rhcPercent * simdt;
			
		// RATE CMD
		} else {
			scsPosition = attitudeRate * DEG + rhcPercent * 5.0;	// +/- 5° per second maximum for now
		}
		
		// Allow max. 4° for now
		commandedPosition = max(min(scsPosition, 4.0), -4.0) + ((trimThumbwheel->GetState() - 8.0) / 2.0); 
	}

	//
	// Which system is active?
	//

	// "Default" system is 1
	activeSystem = 1;

	// Switched to AUTO and THC CLOCKWISE
	if (tvcGimbalDriveSwitch->IsCenter() && saturn->THCRotary.IsClockwise()) {
		activeSystem = 2;
	}

	/// \todo Auto switch-over because of overcurrent 

	// Switched to system 2
	if (tvcGimbalDriveSwitch->IsDown()) {
		activeSystem = 2;
	}

	//
	// Drive gimbals when powered
	//

	if (activeSystem == 1) {
		if (IsSystem1Powered() && motor1Running) {
			//position = commandedPosition; // Instant positioning
			GimbalTimestep(simdt);
		}
	} else {
		if (IsSystem2Powered() && motor2Running) {
			//position = commandedPosition; // Instant positioning
			GimbalTimestep(simdt);
		}
	}

	// Only 5.5 degrees of travel allowed.
	if (position > 5.5) { position = 5.5; }
	if (position < -5.5) { position = -5.5; }

	// sprintf(oapiDebugString(), "position %.3f commandedPosition %.3f cmcPosition %.3f", position, commandedPosition, cmcPosition);
}

void SPSGimbalActuator::GimbalTimestep(double simdt)
{
	double LMR, dposcmd, poscmdsign, dpos;

	LMR = 0.15*DEG;

	dposcmd = commandedPosition - position;
	poscmdsign = abs(commandedPosition - position) / (commandedPosition - position);
	if (abs(dposcmd)>LMR*simdt)
	{
		dpos = poscmdsign*LMR*simdt;
	}
	else
	{
		dpos = dposcmd;
	}
	position += dpos;
}

void SPSGimbalActuator::SystemTimestep(double simdt) {

	if (saturn->GetStage() > CSM_LEM_STAGE) return;

	if (activeSystem == 1 && IsSystem1Powered()) {
		DrawSystem1Power();
	
	} else if (IsSystem2Powered()) {
		DrawSystem2Power();
	}

	if (motor1Running) {
		motor1Source->DrawPower(100);	/// \todo real power consumption is unknown 
	}
	if (motor2Running) {
		motor2Source->DrawPower(100);	/// \todo real power consumption is unknown 
	}
}

bool SPSGimbalActuator::IsSystem1Powered() {

	if (saturn->TVCServoPower1Switch.IsUp()) {
		if (saturn->StabContSystemTVCAc1CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE  && 
			saturn->SystemMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) {
			return true;
		}
	} else if (saturn->TVCServoPower1Switch.IsDown()) {
		if (saturn->StabContSystemAc2CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE  && 
			saturn->SystemMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) {
			return true;
		}
	}
	return false;
}

bool SPSGimbalActuator::IsSystem2Powered() {

	if (saturn->TVCServoPower2Switch.IsUp()) {
		if (saturn->StabContSystemAc1CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE  && 
			saturn->SystemMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) {
			return true;
		}
	} else if (saturn->TVCServoPower2Switch.IsDown()) {
		if (saturn->ECATVCAc2CircuitBraker.Voltage() > SP_MIN_ACVOLTAGE  && 
			saturn->SystemMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) {
			return true;
		}
	}
	return false;
}

void SPSGimbalActuator::DrawSystem1Power() {

	if (saturn->TVCServoPower1Switch.IsUp()) {
		saturn->StabContSystemTVCAc1CircuitBraker.DrawPower(1.36);	// Systems handbook
		saturn->SystemMnACircuitBraker.DrawPower(10);				/// \todo Real power consumption is unknown 

	} else if (saturn->TVCServoPower1Switch.IsDown()) {
		saturn->StabContSystemAc2CircuitBraker.DrawPower(1.36);		// Systems handbook
		saturn->SystemMnBCircuitBraker.DrawPower(10);				/// \todo Real power consumption is unknown 
	}
}

void SPSGimbalActuator::DrawSystem2Power() {

	if (saturn->TVCServoPower2Switch.IsUp()) {
		saturn->StabContSystemAc1CircuitBraker.DrawPower(1.36);		// Systems handbook
		saturn->SystemMnACircuitBraker.DrawPower(10);				/// \todo Real power consumption is unknown 

	} else if (saturn->TVCServoPower2Switch.IsDown()) {
		saturn->ECATVCAc2CircuitBraker.DrawPower(1.36);				// Systems handbook
		saturn->SystemMnBCircuitBraker.DrawPower(10);				/// \todo Real power consumption is unknown 
	}
}

void SPSGimbalActuator::ChangeCMCPosition(double delta) {

	cmcPosition += delta;
}

void SPSGimbalActuator::SaveState(FILEHANDLE scn) {

	// START_STRING is written in Saturn
	papiWriteScenario_double(scn, "POSITION", position);
	papiWriteScenario_double(scn, "COMMANDEDPOSITION", commandedPosition);
	papiWriteScenario_double(scn, "CMCPOSITION", cmcPosition);
	papiWriteScenario_double(scn, "SCSPOSITION", scsPosition);
	papiWriteScenario_double(scn, "LASTATTITUDEERROR", lastAttitudeError);
	oapiWriteScenario_int(scn, "ACTIVESYSTEM", activeSystem);
	oapiWriteScenario_int(scn, "MOTOR1RUNNING", (motor1Running ? 1 : 0));
	oapiWriteScenario_int(scn, "MOTOR2RUNNING", (motor2Running ? 1 : 0));

	oapiWriteLine(scn, SPSGIMBALACTUATOR_END_STRING);
}

void SPSGimbalActuator::LoadState(FILEHANDLE scn) {

	char *line;
	int i;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SPSGIMBALACTUATOR_END_STRING, sizeof(SPSGIMBALACTUATOR_END_STRING))) {
			return;
		}

		if (!strnicmp (line, "POSITION", 8)) {
			sscanf(line + 8, "%lf", &position);
		}
		else if (!strnicmp (line, "COMMANDEDPOSITION", 17)) {
			sscanf(line + 17, "%lf", &commandedPosition);
		}
		else if (!strnicmp (line, "CMCPOSITION", 11)) {
			sscanf(line + 11, "%lf", &cmcPosition);
		}
		else if (!strnicmp (line, "SCSPOSITION", 11)) {
			sscanf(line + 11, "%lf", &scsPosition);
		}
		else if (!strnicmp (line, "LASTATTITUDEERROR", 17)) {
			sscanf(line + 17, "%lf", &lastAttitudeError);
		}
		else if (!strnicmp (line, "ACTIVESYSTEM", 12)) {
			sscanf(line + 12, "%d", &activeSystem);
		}
		else if (!strnicmp (line, "MOTOR1RUNNING", 13)) {
			sscanf(line + 13, "%d", &i);
			motor1Running = (i != 0);
		}
		else if (!strnicmp (line, "MOTOR2RUNNING", 13)) {
			sscanf(line + 13, "%d", &i);
			motor2Running = (i != 0);
		}
	}
}
