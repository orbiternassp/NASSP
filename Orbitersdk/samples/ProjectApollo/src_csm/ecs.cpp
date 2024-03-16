/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Environmental Control System.

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

  **************************** Revision History ****************************/

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Hsystems.h"

#include "soundlib.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "ioChannels.h"

#include "saturn.h"

#include "CM_VC_Resource.h"


CabinPressureRegulator::CabinPressureRegulator() {

	pipe = NULL;
	cabinRepressValve = NULL;
	emergencyCabinPressureRegulator = NULL;
	cabinRepressValveSwitch = NULL;
	emergencyCabinPressureSwitch = NULL;

	closed = false;
	press = 0;
}

CabinPressureRegulator::~CabinPressureRegulator() {

}

void CabinPressureRegulator::Init(h_Pipe* p, h_Pipe *crv, h_Pipe *ecpr, RotationalSwitch *crvs, RotationalSwitch *ecps, PushSwitch *ecpts) {

	pipe = p;	
	cabinRepressValve = crv;
	emergencyCabinPressureRegulator = ecpr;
	cabinRepressValveSwitch = crvs;
	emergencyCabinPressureSwitch = ecps;
	emergencyCabinPressureTestSwitch = ecpts;
}

void CabinPressureRegulator::SystemTimestep(double simdt) {
	
	if (!pipe) return;
	// Valve in motion
	if (pipe->in->pz) return;

	// Forcibly closed?
	if (closed) {
		pipe->in->Close();
		return;
	} 
	pipe->in->Open();
	// Close, if cabin pressure below 3.5 psi
	double cabinpress = pipe->out->parent->space.Press;
	if (cabinpress < 3.5 / PSI) {
		pipe->P_max = 0;
	} else {
		pipe->P_max = press / PSI;
	}

	// Cabin repress valve
	if (cabinRepressValveSwitch->GetState() == 0) {
		cabinRepressValve->P_max = 0;

	} else {
		cabinRepressValve->P_max = 1000. / PSI; // i.e. disabled
		cabinRepressValve->flowMax = ((double) cabinRepressValveSwitch->GetState()) / LBH; // 6 lb/h max, see AOH
	}

	// Emergency Cabin Pressure Regulator
	if (emergencyCabinPressureSwitch->GetState() == 3 || (cabinpress > 4.6 / PSI && emergencyCabinPressureTestSwitch->GetState() == 0)) {
		emergencyCabinPressureRegulator->P_max = 0;
	} else {
		if (emergencyCabinPressureTestSwitch->GetState() != 0) 
			emergencyCabinPressureRegulator->P_max = 1000. / PSI; // i.e. disabled
		else
			emergencyCabinPressureRegulator->P_max = 4.6 / PSI;	
		emergencyCabinPressureRegulator->flowMax = 40.2 / LBH; // 0.67 lb/min max, see AOH
	}
}

void CabinPressureRegulator::Reset() {

	closed = false;
}

void CabinPressureRegulator::Close() {

	closed = true;
}

void CabinPressureRegulator::SetPressurePSI(double p) {

	press = p;
}

void CabinPressureRegulator::SetMaxFlowLBH(double f) {

	if (!pipe) return;
	pipe->flowMax = f / LBH;
}

void CabinPressureRegulator::ResetMaxFlow() {

	// Real max. flow is 1.4 lb/h, see AOH
	if (!pipe) return;
	pipe->flowMax = 1.4 / LBH;
}

void CabinPressureRegulator::LoadState(char *line) {

	int i;
	double d;

	sscanf(line + 22, "%d %lf", &i, &d);
	closed = (i != 0);
	press = d;
}

void CabinPressureRegulator::SaveState(FILEHANDLE scn) {

	char buffer[256];

	sprintf(buffer, "%i %lf", (closed ? 1 : 0), press);
	oapiWriteScenario_string(scn, "CABINPRESSUREREGULATOR", buffer);
}


O2DemandRegulator::O2DemandRegulator() {

	pipe = NULL;
	suitReliefValve = NULL;
	closed = false;
	suitReliefValveOpen = false; 
}

O2DemandRegulator::~O2DemandRegulator() {

}

void O2DemandRegulator::Init(h_Pipe *p, h_Pipe *s, h_Pipe *t, RotationalSwitch *o2ds, RotationalSwitch *sts) {

	pipe = p;
	suitReliefValve = s;	
	suitTestValve = t;
	o2DemandSwitch = o2ds;
	suitTestSwitch = sts;
}

void O2DemandRegulator::SystemTimestep(double simdt) {

	double cabinPress = suitReliefValve->out->parent->space.Press;
	double scrvPress = suitReliefValve->in->parent->space.Press;

	// O2 demand regulator
	if (!pipe->in->pz && !suitTestValve->out->pz) {		// is it moving?
		// Forcibly closed or suit circuit return valve is open or regulator is turned off 
		if (closed || suitReliefValve->out->parent->OUT_valve.open || o2DemandSwitch->GetState() == 0) {
			pipe->in->Close();
			suitTestValve->out->Close();
		
		// Suit test - press mode
		} else if (suitTestSwitch->GetState() == 2) {

			pipe->in->Open();
			pipe->P_max = 0;

			suitTestValve->out->Open();
			suitTestValve->flowMax = 70. / LBH; // calibrated for 75 sec pressurization time

			double suitPress = suitTestValve->out->parent->space.Press;
			if (suitPress - cabinPress < 4.2 / PSI) {
				suitTestValve->P_max = 1000. / PSI; // i.e. disabled
			} else {
				suitTestValve->P_max = cabinPress + 4.3 / PSI;
			}
		} else {
			// one/both regulators
			double count = 1.;
			if (o2DemandSwitch->GetState() == 2) {
				count = 2.;
			}
			pipe->in->Open();
			pipe->P_max = max(3.75 / PSI, cabinPress) + 2.5 / INH2O;

			double dp = scrvPress - max(3.75 / PSI, cabinPress);
			if (dp > -5.5 / INH2O) {
				pipe->flowMax = count * 0.42 / LBH; // 0.007 lb/min per regulator (Systems Handbook)
			} else if (cabinPress > 3.75 / PSI) {
				pipe->flowMax = count * 9. / LBH; // 0.3 lb/min both (Systems Handbook)
			} else {
				pipe->flowMax = count * 19.8 / LBH; // 0.66 lb/min both (Systems Handbook)
			}
			suitTestValve->out->Close();
		}
	}
				
	// Suit relief valve
	if (!suitReliefValve->in->pz) {		// is it moving?
		// Forcibly open?
		if (suitReliefValveOpen) {
			suitReliefValve->in->Open();
			suitReliefValve->P_max = 1000. / PSI;	// Disable PREG
			suitReliefValve->flowMax = 0;

		} else {
			// Disable if suit circuit return valve is open or suit test - press mode
			if (suitReliefValve->out->parent->OUT_valve.open || (o2DemandSwitch->GetState() != 0 && suitTestSwitch->GetState() == 2)) {
				suitReliefValve->in->Close();
			} else {
				// Open if suit is overpressured (see AOH)			
				if (scrvPress > 3.75 / PSI + 3.0 / INH2O && cabinPress <= 3.75 / PSI) { 
					suitReliefValve->in->Open();
					suitReliefValve->P_max = 1000. / PSI;	// Disable PREG
				} else if (cabinPress > 3.75 / PSI) {								
					suitReliefValve->in->Open();
					suitReliefValve->P_max = scrvPress - 3.0 / INH2O;
				} else {
					suitReliefValve->in->Close();
				}
			}
			if (suitTestSwitch->GetState() == 1) {
				suitReliefValve->flowMax = 70. / LBH; // like suit test valve
			} else {
				suitReliefValve->flowMax = 39.6 / LBH; // 0.66 lb/min (Systems Handbook)
			}
		}
	}
}

void O2DemandRegulator::Reset() {

	closed = false;
}

void O2DemandRegulator::Close() {

	closed = true;
}

void O2DemandRegulator::OpenSuitReliefValve() {

	suitReliefValveOpen = true;
}

void O2DemandRegulator::ResetSuitReliefValve() {

	suitReliefValveOpen = false;
}

void O2DemandRegulator::LoadState(char *line) {

	int i1, i2;

	sscanf(line + 17, "%d %d", &i1, &i2);
	closed = (i1 != 0);
	suitReliefValveOpen = (i2 != 0);
}

void O2DemandRegulator::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i", (closed ? 1 : 0), (suitReliefValveOpen ? 1 : 0)); 
	oapiWriteScenario_string(scn, "O2DEMANDREGULATOR", buffer);
}


CabinPressureReliefValve::CabinPressureReliefValve(Sound &plventsound) : postLandingVentSound(plventsound) {

	pipe = NULL;
	inlet = NULL;
	reliefPressure = 6.0 / PSI;
	leakSize = 0; 
}

CabinPressureReliefValve::~CabinPressureReliefValve() {

}

void CabinPressureReliefValve::Init(h_Pipe *p, h_Pipe *i, Saturn *v, ThumbwheelSwitch *l, CircuitBrakerSwitch *plvlv, ThreePosSwitch *plv, e_object *plpower, SaturnSideHatch *sh) {

	pipe = p;
	inlet = i;
	saturn = v;
	lever = l;
	postLandingValve = plvlv;
	postLandingVent = plv;
	postLandingPower = plpower;
	sideHatch = sh;
}

void CabinPressureReliefValve::SystemTimestep(double simdt) {

	if (!pipe && !inlet) return;
	// Valve in motion
	if (pipe->in->pz || inlet->in->pz) return;

	// Side hatch vent valve
	bool hatchOpen = false;
	double cabinPress = pipe->in->parent->space.Press;
	if (sideHatch->IsOpen() && saturn->GetSystemsState() >= SATSYSTEMS_READYTOLAUNCH) { // Hatch disabled during GSE support
		if (cabinPress > saturn->GetAtmPressure()) {
			pipe->in->Open();
			pipe->in->size = (float) 100.;	// no pressure in a few seconds
			pipe->flowMax = 2000. / LBH; 
			
			inlet->in->Close();
		} else {
			inlet->in->Open();		
			inlet->in->size = (float) 10.0;  // full pressure in a few seconds
			inlet->P_max = saturn->GetAtmPressure();
			inlet->flowMax = 0; // no max. flow

			pipe->in->Close(); 
		}
		hatchOpen = true;

	} else if (sideHatch->GetVentValveRotary()->GetState() <= 3) {
		double f = sideHatch->GetVentValveRotary()->GetState();
		f = (4. - f) / 4.;
		if (cabinPress > saturn->GetAtmPressure()) {
			pipe->in->Open();
			pipe->in->size = (float) (20. * f);
			pipe->flowMax = 250. / LBH * f;	// about 1 min from 5 psi to 0.1 psi
			
			inlet->in->Close();
		} else {
			inlet->in->Open();		
			inlet->in->size = (float) (1.0 * f);  // guessed in order to have reasonable reaction times in pressure
			inlet->P_max = saturn->GetAtmPressure();
			inlet->flowMax = 0; // no max. flow

			pipe->in->Close(); 
		}
		hatchOpen = true;
	}
	//sprintf(oapiDebugString(), "Cabin Press %f Flow %f", cabinPress * PSI, inlet->flow * LBH); 

	// Post Landing Vent
	if (postLandingValve->IsDown() && !postLandingVent->IsDown() && postLandingPower->Voltage() > SP_MIN_DCVOLTAGE) {
		if (!hatchOpen) {
			if (cabinPress > saturn->GetAtmPressure()) {
				pipe->in->Open();
				if (postLandingVent->IsUp()) {		// hi to lo is 1:1.5, rest is guessed in order to have reasonable reaction times in pressure
					pipe->in->size = (float) 20.;
					pipe->flowMax =  400./ LBH;
				} else {
					pipe->in->size = (float) 12.;
					pipe->flowMax =  260./ LBH;
				}
				inlet->in->Close();
			} else {
				inlet->in->Open();
				if (postLandingVent->IsUp()) {		// hi to lo is 1:1.5, rest is guessed in order to have reasonable reaction times in pressure
					inlet->in->size = (float) 20.0;
				} else {
					inlet->in->size = (float) 12.0;
				}
				inlet->P_max = saturn->GetAtmPressure();
				inlet->flowMax = 0; // no max. flow
				pipe->in->Close();
			}
		}
		if (postLandingVent->IsUp()) {
			postLandingPower->DrawPower(25.5);	// systems handbook
			postLandingVentSound.play(LOOP); 
		} else {
			postLandingPower->DrawPower(22.1);	// systems handbook
			postLandingVentSound.play(LOOP, 170.0 / 255.0); 
		}
		return;
	}
	postLandingVentSound.stop();
	if (hatchOpen) return; 
	
	// Closed
	bool closed = true;
	if (lever->GetState() == 0) {
		inlet->in->Close();

	// Normal / Boost
	} else if (lever->GetState() == 1 || lever->GetState() == 2) {
		if (cabinPress > 14.7 / PSI + 1.0 / INH2O) {
			pipe->in->Open();
			pipe->in->size = (float) 0.01;
			pipe->flowMax = 0; // No max. flow
			inlet->in->Close();
			closed = false;

		} else if (cabinPress - saturn->GetAtmPressure() > reliefPressure) {
			pipe->in->Open();
			pipe->in->size = (float) 6.0;
			// Normal
			if (lever->GetState() == 1) {
				pipe->flowMax = 30./ LBH; // Value is guessed
			// Boost
			} else {
				pipe->flowMax = 70./ LBH; // about 6 min from 3 psi to (almost) zero
			}
			inlet->in->Close();
			closed = false;

		} else if (saturn->GetAtmPressure() - cabinPress > 25. / INH2O) {	// Systems handbook
			inlet->in->Open();
			inlet->in->size = (float) 15.0;
			inlet->P_max = saturn->GetAtmPressure() - 25. / INH2O;
			// Normal
			if (lever->GetState() == 1) {
				inlet->flowMax = 30./ LBH; 
			// Boost
			} else {
				inlet->flowMax = 150./ LBH; 
			}
			
		} else {
			inlet->in->Close();
		}

	// Dump
	} else if (lever->GetState() == 3) {
		if (cabinPress > saturn->GetAtmPressure()) {
			pipe->in->Open();
			pipe->in->size = (float) 6.0;
			pipe->flowMax = 70./ LBH; // about 6 min from 3 psi to (almost) zero
			inlet->in->Close();
			closed = false;
		} else {
			inlet->in->Open();
			inlet->in->size = (float) 15.0;
			inlet->P_max = saturn->GetAtmPressure();
			inlet->flowMax = 150./ LBH; 
		}
	}

	// Leak handling
	if (closed) {
		if (leakSize > 0) {
			pipe->in->Open();
			pipe->in->size = (float) leakSize;
			pipe->flowMax = 0; // No max. flow
	
		} else {
			pipe->in->Close();
		}
	}
}

void CabinPressureReliefValve::SetLeakSize(double s) {

	leakSize = s;
}

void CabinPressureReliefValve::SetReliefPressurePSI(double p) {

	if (!pipe) return;
	reliefPressure = p / PSI;
}

void CabinPressureReliefValve::LoadState(char *line) {
	
	sscanf(line + 25, "%lf %lf", &leakSize, &reliefPressure);
}

void CabinPressureReliefValve::SaveState(int index, FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "  CABINPRESSURERELIEFVALVE%i %lf %lf", index, leakSize, reliefPressure); 
	oapiWriteLine(scn, buffer);
}


SuitCircuitReturnValve::SuitCircuitReturnValve() {

	pipe = NULL;
	lever = NULL;
}

SuitCircuitReturnValve::~SuitCircuitReturnValve() {

}

void SuitCircuitReturnValve::Init(h_Pipe *p, CircuitBrakerSwitch *l) {

	pipe = p;
	lever = l;
}

void SuitCircuitReturnValve::SystemTimestep(double simdt) {

	if (!pipe || !lever) return;
	// Valve in motion
	if (pipe->in->pz) return;

	if (lever->IsDown()) {
		double cabinPress = pipe->in->parent->space.Press;
		pipe->P_max = cabinPress - 2.0 / INH2O;
		pipe->in->Open();
	} else {
		pipe->in->Close();
	}
}


O2SMSupply::O2SMSupply() {

	o2SMSupply = NULL;
	o2MainRegulator = NULL;
	o2SurgeTank = NULL;
	o2RepressPackage = NULL;
	smSupplyValve = NULL;
	surgeTankValve = NULL;
	repressPackageValve = NULL;
	closed = false;
	o2SMSupplyVoid = false;
	o2MainRegulatorVoid = false;
}

O2SMSupply::~O2SMSupply() {

}

void O2SMSupply::Init(h_Tank *o2sm, h_Tank *o2mr, h_Tank *o2st, h_Tank *o2rp, h_Tank *o2rpo, h_Pipe *o2rpop,
					  RotationalSwitch *smv, RotationalSwitch *stv, RotationalSwitch *rpv,
					  CircuitBrakerSwitch *mra, CircuitBrakerSwitch *mrb, PanelSwitchItem *eo2v,
					  PanelSwitchItem *ro2v, RotationalSwitch *strv) {

	o2SMSupply = o2sm;	
	o2MainRegulator = o2mr;
	o2SurgeTank = o2st;
	o2RepressPackage = o2rp;
	o2RepressPackageOutlet = o2rpo;
	o2RepressPackageOutletPipe = o2rpop;
	smSupplyValve = smv;	
	surgeTankValve = stv;
	repressPackageValve = rpv;
	mainRegulatorASwitch = mra;
	mainRegulatorBSwitch = mrb;
	emergencyO2Valve = eo2v;
	repressO2Valve = ro2v;
	surgeTankReliefValve = strv;
}

void O2SMSupply::SystemTimestep(double simdt) {

	// Is something moving?
	if (o2SMSupply->IN_valve.pz || o2SMSupply->OUT2_valve.pz || o2SMSupply->LEAK_valve.pz || 
		o2SurgeTank->IN_valve.pz || o2RepressPackage->IN_valve.pz || o2MainRegulator->IN_valve.pz ||
		o2SurgeTank->OUT_valve.pz || o2RepressPackageOutlet->OUT_valve.pz) return;

	bool allClosed = true;

	// SM supply, forcibly closed after SM separation 
	if (closed || smSupplyValve->GetState() == 0) {
		o2SMSupply->IN_valve.Close();			
	} else {
		o2SMSupply->IN_valve.Open();
		allClosed = false;
	}

	// Surge tank
	o2SurgeTank->BoilAllAndSetTemp(285);	//Needs to be done later by a heat exchanger

	if (surgeTankValve->GetState() == 0) {
		o2SurgeTank->IN_valve.Close();
	} else {
			o2SurgeTank->IN_valve.Open();
		}

	//Surge tank relief
	if (surgeTankReliefValve->GetState() == 0) {
		o2SurgeTank->OUT_valve.Close();
	}
	else {
		o2SurgeTank->OUT_valve.Open();
	}

	// Repress package
	if (repressPackageValve->GetState() == 1) {
		o2SMSupply->LEAK_valve.Close();	
		o2RepressPackage->IN_valve.Close();
		o2RepressPackage->OUT2_valve.Close();
	} else {
		// SM supply open? 
		if (o2SMSupply->IN_valve.open) {
			o2RepressPackage->IN_valve.Open();
		} else {
			o2RepressPackage->IN_valve.Close();
		}
		o2RepressPackage->OUT2_valve.Open();
		// On or Fill?
		if (repressPackageValve->GetState() == 2) {
			o2SMSupply->LEAK_valve.Open();
			allClosed = false;
		} else {
			o2SMSupply->LEAK_valve.Close();	
		}
	}

	// Purge "pipe tanks" in case of no supply
	bool mainregvoid = false;
	if (allClosed) {
		if (!o2SMSupplyVoid) {
			o2SMSupplyO2 = o2SMSupply->space.composition[SUBSTANCE_O2];
			o2SMSupply->space.Void();
			o2SMSupplyVoid = true;
		}
		mainregvoid = true;

	} else {
		if (o2SMSupplyVoid) {
			*o2SMSupply += o2SMSupplyO2;
			o2SMSupplyVoid = false;
		}
		o2SMSupply->BoilAllAndSetTemp(285);	//Needs to be done later by a heat exchanger
	
	// O2 main regulator
		if (mainRegulatorASwitch->GetState() && mainRegulatorBSwitch->GetState()) {
			o2MainRegulator->IN_valve.Close();
			mainregvoid = true;

		} else {
			o2MainRegulator->IN_valve.Open();
		}
	}
	if (mainregvoid) {
		if (!o2MainRegulatorVoid) {
			o2MainRegulatorO2 = o2MainRegulator->space.composition[SUBSTANCE_O2];
			o2MainRegulator->space.Void();
			o2MainRegulatorVoid = true;
		}
	} else {
		if (o2MainRegulatorVoid) {
			*o2MainRegulator += o2MainRegulatorO2;
			o2MainRegulatorVoid = false;
		}
	}

	// Repress package outlet
	o2RepressPackage->BoilAllAndSetTemp(285);	//Needs to be done later by a heat exchanger
	o2RepressPackageOutlet->BoilAllAndSetTemp(285);	//Needs to be done later by a heat exchanger
	if (repressO2Valve->GetState() == THREEPOSSWITCH_UP) {
		o2RepressPackageOutlet->OUT_valve.Open();
		o2RepressPackageOutletPipe->flowMax = 300. / LBH;	// cabin pressure 0 to 3 psi in about one minute
	
	} else if (emergencyO2Valve->GetState() == TOGGLESWITCH_UP)  {
		o2RepressPackageOutlet->OUT_valve.Open();
		o2RepressPackageOutletPipe->flowMax = 1. / LBH;		// real flow through the masks is unknown
	
	} else {
		o2RepressPackageOutlet->OUT_valve.Close();
	}
}

void O2SMSupply::Close() {

	closed = true;
}

void O2SMSupply::LoadState(char *line) {

	int i, j, k;

	sscanf(line + 10, "%i %i %i %i %lf %lf %lf %i %lf %lf %lf", &i, &j, &k,
		&o2SMSupplyO2.subst_type, &o2SMSupplyO2.mass , &o2SMSupplyO2.vapor_mass, &o2SMSupplyO2.Q,
		&o2MainRegulatorO2.subst_type, &o2MainRegulatorO2.mass , &o2MainRegulatorO2.vapor_mass, &o2MainRegulatorO2.Q);
	closed = (i != 0);
	o2SMSupplyVoid = (j != 0);
	o2MainRegulatorVoid = (k != 0);
}

void O2SMSupply::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i %i %i %8.4f %8.4f %8.4f %i %8.4f %8.4f %8.4f", (closed ? 1 : 0), (o2SMSupplyVoid ? 1 : 0), (o2MainRegulatorVoid ? 1 : 0),
		o2SMSupplyO2.subst_type, o2SMSupplyO2.mass , o2SMSupplyO2.vapor_mass, o2SMSupplyO2.Q,
		o2MainRegulatorO2.subst_type, o2MainRegulatorO2.mass , o2MainRegulatorO2.vapor_mass, o2MainRegulatorO2.Q); 
	oapiWriteScenario_string(scn, "O2SMSUPPLY", buffer);
}


CrewStatus::CrewStatus(Sound &crewdeadsound) : crewDeadSound(crewdeadsound) {

	status = ECS_CREWSTATUS_OK;
	suitPressureLowTime = 600;
	suitPressureHighTime = 3600;
	suitTemperatureTime = 12 * 3600;
	suitCO2Time = 1800;
	accelerationTime = 10;
	lastVerticalVelocity = 0;

	saturn = NULL;
	firstTimestepDone = false;
}

CrewStatus::~CrewStatus() {
}

void CrewStatus::Init(Saturn *s) {

	saturn = s;
}

void CrewStatus::Timestep(double simdt) {

	if (!firstTimestepDone) {
		// Dead at startup?
		if (status == ECS_CREWSTATUS_DEAD) {
			crewDeadSound.play(); 
		}
		firstTimestepDone = true;
	}

	if (!saturn) return;

	// Already dead?
	if (status == ECS_CREWSTATUS_DEAD) return;

	// No crew?
	if (!saturn->Crewed || saturn->Crew->number == 0) return;

	status = ECS_CREWSTATUS_OK;

	// Acceleration exceeds 12 g for 10 seconds
	VECTOR3 f, w;
	saturn->GetForceVector(f);
	saturn->GetWeightVector(w);
	if (length(f - w) / saturn->GetMass() > 12. * G) {
		if (accelerationTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;

		}
		else {
			status = ECS_CREWSTATUS_CRITICAL;
			accelerationTime -= simdt;
		}
	}
	else {
		accelerationTime = 10;
	}

	// Touchdown speed exceeds 15 m/s (= about 50 ft/s)
	if (saturn->GroundContact()) {
		if (fabs(lastVerticalVelocity) > 15) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play();
			return;
		}
	}
	else {
		VECTOR3 v;
		saturn->GetAirspeedVector(FRAME_HORIZON, v);
		lastVerticalVelocity = v.y;
	}

	// In atmosphere with hatch open?
	if ((saturn->GetAtmPressure() > 572 * 100) && (saturn->SideHatch.IsOpen() || saturn->ForwardHatch.IsOpen())) return;

	AtmosStatus atm;
	saturn->GetAtmosStatus(atm);

	// Pressure in suit lower than 2.8 psi for 10 minutes
	if (atm.SuitPressurePSI < 2.8) {
		if (suitPressureLowTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 
			return;

		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			suitPressureLowTime -= simdt;
		}
	} else {
		suitPressureLowTime = 600;
	}

	// Pressure in suit higher than 22 psi for 1 hour
	if (atm.SuitPressurePSI > 22) {
		if (suitPressureHighTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 
			return;

		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			suitPressureHighTime -= simdt;
		}
	} else {
		suitPressureHighTime = 3600;
	}

	// Suit temperature above about 45�C or below about 0�C for 12 hours
	if (atm.SuitTempK > 320 || atm.SuitTempK < 270) {
		if (suitTemperatureTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 
			return;

		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			suitTemperatureTime -= simdt;
		}
	} else {
		suitTemperatureTime = 12 * 3600;
	}

	// Suit CO2 above 10 mmHg for 30 minutes
	if (atm.SuitCO2MMHG > 10) {
		if (suitCO2Time <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 
			return;

		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			suitCO2Time -= simdt;
		}
	} else {
		suitCO2Time = 1800;
	}
}

void CrewStatus::LoadState(char *line) {

	sscanf(line + 10, "%d %lf %lf %lf %lf %lf %lf", &status, &suitPressureLowTime, &suitPressureHighTime, &suitTemperatureTime,
		                                            &suitCO2Time, &accelerationTime, &lastVerticalVelocity); 
}

void CrewStatus::SaveState(FILEHANDLE scn) {

	char buffer[1000];

	sprintf(buffer, "%d %lf %lf %lf %lf %lf %lf", status, suitPressureLowTime, suitPressureHighTime, suitTemperatureTime,
		                                          suitCO2Time, accelerationTime, lastVerticalVelocity); 
	oapiWriteScenario_string(scn, "CREWSTATUS", buffer);
}


SaturnSideHatch::SaturnSideHatch(Sound &opensound, Sound &closesound) :
	OpenSound(opensound), CloseSound(closesound) {
	open = true;	// Hatch open at prelaunch
	toggle = 0;

	sidehatch_state.SetState(1.0, 0.0);
	sidehatch_state.SetOperatingSpeed(0.2);
	anim_SideHatchVC = -1;
	anim_gearboxsel = -1;
	anim_actuatorsel = -1;
	anim_ventvalve = -1;
}

SaturnSideHatch::~SaturnSideHatch() {
}

void SaturnSideHatch::Init(Saturn *s, RotationalSwitch *gbs, RotationalSwitch *ahs, RotationalSwitch *ahso, RotationalSwitch *vvr) {
	saturn = s;
	gearBoxSelector = gbs;
	actuatorHandleSelector = ahs;
	actuatorHandleSelectorOpen = ahso;
	ventValveRotary = vvr;
}

void SaturnSideHatch::Toggle(bool enforce) {

	if (toggle == 0) {
		if (!open) {
			if (enforce || (actuatorHandleSelector->GetState() == 0 && gearBoxSelector->GetState() == 0)) {
				open = true;
				toggle = 2;	// Orbiter 2006P1 is crashing sometimes if this is set to a smaller value
				saturn->SetSideHatchMesh();
				sidehatch_state.Open();
				// Set switches on the open panel
				actuatorHandleSelectorOpen->SetState(0);
				OpenSound.play();
			}
		} else {
			if (enforce || actuatorHandleSelectorOpen->GetState() == 2) {
				open = false;
				toggle = 2;	// Orbiter 2006P1 is crashing sometimes if this is set to a smaller value
				saturn->SetSideHatchMesh();
				sidehatch_state.Close();
				// Set switches on the open panel
				actuatorHandleSelector->SetState(2);
				gearBoxSelector->SetState(2);
				CloseSound.play();
			}
		}
	}
}

void SaturnSideHatch::Timestep(double simdt) {

	if (toggle > 0) {
		toggle--;
		if (toggle == 0) {
			saturn->PanelRefreshSideHatch();
		}
	}

	if (sidehatch_state.Process(simdt)) {
		saturn->SetAnimation(anim_SideHatchVC, sidehatch_state.State());
	}

	int act_state = 0;
	double vent_state = (double)ventValveRotary->GetState();

	if (!open) {
		act_state = actuatorHandleSelector->GetState();
	} else {
		act_state = actuatorHandleSelectorOpen->GetState();
	}

	if (gearBoxSelector->GetState() == 2) {
		saturn->SetAnimation(anim_gearboxsel, 1.0);

	} else if (gearBoxSelector->GetState() == 1) {
		saturn->SetAnimation(anim_gearboxsel, 0.5);

	} else {
		saturn->SetAnimation(anim_gearboxsel, 0.0);
	}

	if (act_state == 2) {
		saturn->SetAnimation(anim_actuatorsel, 1.0);

	} else if (act_state == 1) {
		saturn->SetAnimation(anim_actuatorsel, 0.5);

	} else {
		saturn->SetAnimation(anim_actuatorsel, 0.0);
	}

	saturn->SetAnimation(anim_ventvalve, vent_state / 7);
}

void SaturnSideHatch::SwitchToggled(PanelSwitchItem *s) {
	Toggle();
}

void SaturnSideHatch::LoadState(char *line) {

	int i1;
	double a, b;

	sscanf(line + 9, "%d %d %lf %lf", &i1, &toggle, &a, &b);
	open = (i1 != 0);
	sidehatch_state.SetState(a, b);
}

void SaturnSideHatch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i %lf %lf", (open ? 1 : 0), toggle, sidehatch_state.State(), sidehatch_state.Speed());
	oapiWriteScenario_string(scn, "SIDEHATCH", buffer);
}
void SaturnSideHatch::DefineAnimationsVC(UINT idx)
{
	// Side Hatch Animations
	ANIMATIONCOMPONENT_HANDLE ach_SideHatchVC, ach_gearboxsel, ach_actuatorsel, ach_ventvalve;

	const VECTOR3 SideHatch_HandleRot1Location = { 0.3076, 1.3543, -0.1137 };
	const VECTOR3 SideHatch_HandleRot2Location = { 0.2348, 1.2453, 0.0608 };
	const VECTOR3 SideHatch_VentValveLocation = { -0.2637, 1.1932, 0.1462 };
	const VECTOR3 sidehatch_gearbox_axis = { -0.160457905417272, -0.797020760042779, -0.582246656194721 };
	const VECTOR3 sidehatch_ventvalve_axis = { 0.080192220002342, -0.837079540734271, -0.541171923084705 };

	static UINT	meshgroup_SideHatchVC[7] = { VC_GRP_SideHatch_1, VC_GRP_SideHatch_2, VC_GRP_SideHatch_3, VC_GRP_SideHatch_4, VC_GRP_SideHatch_5, VC_GRP_SideHatch_6,
		VC_GRP_SideHatch_7};
	static UINT	meshgroup_gearboxsel = { VC_GRP_SideHatch_HandleRot1 };
	static UINT	meshgroup_actuatorsel = { VC_GRP_SideHatch_HandleRot2 };
	static UINT	meshgroup_ventvalve = { VC_GRP_SideHatch_VentValve };

	static MGROUP_ROTATE mgt_SideHatchVC(idx, meshgroup_SideHatchVC, 7, _V(-0.427397, 1.06886, 0.440263), _V(-0.187232813439751, 0.501366307175263, -0.844734099939665), (float)(-90.0*RAD));
	static MGROUP_ROTATE mgt_gearboxsel(idx, &meshgroup_gearboxsel, 1, SideHatch_HandleRot1Location, sidehatch_gearbox_axis, (float)(RAD * 60));
	static MGROUP_ROTATE mgt_actuatorsel(idx, &meshgroup_actuatorsel, 1, SideHatch_HandleRot2Location, sidehatch_gearbox_axis, (float)(RAD * 60));
	static MGROUP_ROTATE mgt_ventvalve(idx, &meshgroup_ventvalve, 1, SideHatch_VentValveLocation, sidehatch_ventvalve_axis, (float)(RAD * 180));

	anim_SideHatchVC = saturn->CreateAnimation(0.0);
	anim_gearboxsel = saturn->CreateAnimation(0.5);
	anim_actuatorsel = saturn->CreateAnimation(0.5);
	anim_ventvalve = saturn->CreateAnimation(1.0);

	ach_SideHatchVC = saturn->AddAnimationComponent(anim_SideHatchVC, 0.0f, 1.0f, &mgt_SideHatchVC);
	ach_gearboxsel = saturn->AddAnimationComponent(anim_gearboxsel, 0.0f, 1.0f, &mgt_gearboxsel, ach_SideHatchVC);
	ach_actuatorsel = saturn->AddAnimationComponent(anim_actuatorsel, 0.0f, 1.0f, &mgt_actuatorsel, ach_SideHatchVC);
	ach_ventvalve = saturn->AddAnimationComponent(anim_ventvalve, 0.0f, 1.0f, &mgt_ventvalve, ach_SideHatchVC);

	saturn->SetAnimation(anim_SideHatchVC, sidehatch_state.State());
	saturn->SetAnimation(anim_gearboxsel, 0.5);
	saturn->SetAnimation(anim_actuatorsel, 0.5);
	saturn->SetAnimation(anim_ventvalve, 0.5);
}

SaturnWaterController::SaturnWaterController() {
}

SaturnWaterController::~SaturnWaterController() {
}

void SaturnWaterController::Init(Saturn *s, h_Tank *pt, h_Tank *wt, h_Tank *pit, h_Tank *wit, 
								 h_Pipe *wvp, h_Pipe *wivp) {
	wasteWaterDumpLevel = 0;
	urineDumpLevel = 0;

	saturn = s;
	potableTank = pt;
	wasteTank = wt;
	potableInletTank = pit;
	wasteInletTank = wit;
	wasteVentPipe = wvp;
	wasteInletVentPipe = wivp;
}

void SaturnWaterController::SystemTimestep(double simdt) {

	// Is something moving?
	if (potableInletTank->OUT_valve.pz || potableInletTank->OUT2_valve.pz || 
		wasteInletTank->OUT_valve.pz || potableTank->OUT_valve.pz ||
		wasteTank->OUT_valve.pz || wasteInletTank->OUT2_valve.pz) return;

	// Potable tank inlet
	if (saturn->PotableTankInletRotary.GetState() == 0) {	// open
		potableInletTank->OUT_valve.Open();
		potableInletTank->OUT2_valve.Close();
	} else {										// close
		potableInletTank->OUT_valve.Close();
		potableInletTank->OUT2_valve.Open();
	}

	// Waste tank inlet
	if (saturn->WasteTankInletRotary.GetState() == 0) {	// auto
		wasteInletTank->OUT_valve.Open();
		if (potableTank->space.Press - wasteTank->space.Press < 4.5 / PSI) {
			potableTank->OUT_valve.Close();
		} else if (potableTank->space.Press - wasteTank->space.Press > 6.5 / PSI) {
			potableTank->OUT_valve.Open();
		}
	} else {										// closed
		wasteInletTank->OUT_valve.Close();			
		potableTank->OUT_valve.Open();
	}

	// Pressure relief
	if ((saturn->PressureReliefRotary.GetState() == 0 || saturn->PressureReliefRotary.GetState() == 3) && saturn->WasteH2ODumpHeater.IsFrozen() == false) {	// dump a/b
		wasteTank->OUT_valve.Open();
		if (wasteInletTank->OUT_valve.open) {
			wasteInletTank->OUT2_valve.Close();
		} else {
			wasteInletTank->OUT2_valve.Open();
		}
	} else if (saturn->PressureReliefRotary.GetState() == 1 && saturn->WasteH2ODumpHeater.IsFrozen() == false) {	// "2"
		if (wasteTank->space.Press < 40.0 / PSI) {
			wasteTank->OUT_valve.Close();
		} else if (wasteTank->space.Press > 48.0 / PSI) {
			wasteTank->OUT_valve.Open();
		}
		if (wasteInletTank->OUT_valve.open) {
			wasteInletTank->OUT2_valve.Close();
		} else {
			if (wasteInletTank->space.Press < 40.0 / PSI) {
				wasteInletTank->OUT2_valve.Close();
			} else if (wasteInletTank->space.Press > 48.0 / PSI) {
				wasteInletTank->OUT2_valve.Open();
			}
		}		
	} else {	// off
		wasteTank->OUT_valve.Close();
		wasteInletTank->OUT2_valve.Close();
	}

	// water dump
	wasteWaterDumpLevel = wasteVentPipe->flow / wasteVentPipe->flowMax;
	if (wasteInletTank->OUT2_valve.open) {
		wasteWaterDumpLevel = 0.5 * (wasteWaterDumpLevel + (wasteInletVentPipe->flow / wasteInletVentPipe->flowMax));
	}

	// Urine dump
	urineDumpLevel = 0;

	if (saturn->WasteMGMTOvbdDrainDumpRotary.GetState() == 3 && saturn->UrineDumpHeater.IsFrozen() == false) {
			urineDumpLevel = 1;
		}

	// potable h2o heaters
	if (saturn->PotH2oHtrSwitch.IsUp() && saturn->ECSPOTH2OHTRMnACircuitBraker.IsPowered()) {
		saturn->ECSPOTH2OHTRMnACircuitBraker.DrawPower(45.0);
	}
	if (saturn->PotH2oHtrSwitch.IsDown() && saturn->ECSPOTH2OHTRMnBCircuitBraker.IsPowered()) {
		saturn->ECSPOTH2OHTRMnBCircuitBraker.DrawPower(45.0);
	}
	
	//sprintf(oapiDebugString(), "wasteWaterDumpLevel %f", wasteWaterDumpLevel);
}

void SaturnWaterController::FoodPreparationWaterSwitchToggled(PanelSwitchItem *s) {

	if (s->GetState() == TOGGLESWITCH_UP) {
		potableTank->GetFlow(0.03);	// 1 ounce per cycle (systems handbook)
	}
}


SaturnGlycolCoolingController::SaturnGlycolCoolingController() {
}

SaturnGlycolCoolingController::~SaturnGlycolCoolingController() {
}

void SaturnGlycolCoolingController::Init(Saturn *s) {

	saturn = s;

    suitHeater = (Boiler *) saturn->Panelsdk.GetPointerByString("ELECTRIC:SUITHEATER");
    suitCircuitHeater = (Boiler *) saturn->Panelsdk.GetPointerByString("ELECTRIC:SUITCIRCUITHEATER");
	evapInletMixer = (h_MixingPipe *) saturn->Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLEVAPINLETTEMPVALVE");
	primEvap = (h_Evaporator *) saturn->Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR");
	secEvap = (h_Evaporator *) saturn->Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR");
}

void SaturnGlycolCoolingController::SystemTimestep(double simdt) {

	// Prim/sec suit heat exchanger
	if (saturn->SuitCircuitHeatExchSwitch.IsDown()) {
		saturn->SuitHeatExchangerPrimaryGlycolRotary.SetState(1);
	
	} else if (saturn->SuitCircuitHeatExchSwitch.IsUp()) {
		saturn->SuitHeatExchangerPrimaryGlycolRotary.SetState(0);
	}

	if (saturn->SuitHeatExchangerPrimaryGlycolRotary.GetState() == 0) {
		saturn->PrimSuitHeatExchanger->SetPumpAuto();
		saturn->PrimSuitCircuitHeatExchanger->SetPumpAuto();
	} else {
		saturn->PrimSuitHeatExchanger->SetPumpOff();
		saturn->PrimSuitCircuitHeatExchanger->SetPumpOff();
	}

	if (saturn->SuitHeatExchangerSecondaryGlycolRotary.GetState() == 0) {
		saturn->SecSuitHeatExchanger->SetPumpAuto();
		saturn->SecSuitCircuitHeatExchanger->SetPumpAuto();
	} else {
		saturn->SecSuitHeatExchanger->SetPumpOff();
		saturn->SecSuitCircuitHeatExchanger->SetPumpOff();
	}

	// "Dummy" heaters are active, when either loop is active
	if (saturn->SuitHeatExchangerPrimaryGlycolRotary.GetState() == 0 ||
		saturn->SuitHeatExchangerSecondaryGlycolRotary.GetState() == 0) {
		
		suitHeater->SetPumpAuto();
		suitCircuitHeater->SetPumpAuto();
	} else {
		suitHeater->SetPumpOff();
		suitCircuitHeater->SetPumpOff();
	}

	// Prim. evaporator inlet temp
	saturn->PrimaryGlycolEvapInletTempRotary.SoundEnabled(false);
	if (evapInletMixer->ratio <= 0.16) {
		saturn->PrimaryGlycolEvapInletTempRotary.SetState(0);
	
	} else if (evapInletMixer->ratio <= 0.5) {
		saturn->PrimaryGlycolEvapInletTempRotary.SetState(1);
	
	} else if (evapInletMixer->ratio <= 0.83) {
		saturn->PrimaryGlycolEvapInletTempRotary.SetState(2);
	
	} else {
		saturn->PrimaryGlycolEvapInletTempRotary.SetState(3);	
	} 
	saturn->PrimaryGlycolEvapInletTempRotary.SoundEnabled(true);
}

void SaturnGlycolCoolingController::GlycolEvapTempInSwitchToggled(PanelSwitchItem *s) {

	if (s->GetState() == TOGGLESWITCH_UP) {
		evapInletMixer->SetPumpAuto();
	} else {
		evapInletMixer->SetPumpOn();
	}
}

void SaturnGlycolCoolingController::PrimaryGlycolEvapInletTempRotaryToggled(PanelSwitchItem *s) {

	switch (s->GetState()) {
	case 0:
		evapInletMixer->ratio = 0;
		break;

	case 1:
		evapInletMixer->ratio = 0.33;
		break;

	case 2:
		evapInletMixer->ratio = 0.66;
		break;

	case 3:
		evapInletMixer->ratio = 1;
		break;
	}
}

void SaturnGlycolCoolingController::PrimEvapSwitchesToggled(PanelSwitchItem *s) {

	if (saturn->GlycolEvapH2oFlowSwitch.IsCenter() || saturn->EvapWaterControlPrimaryRotary.GetState() == 0) {
		primEvap->SetPumpOff(); 

	} else if (saturn->GlycolEvapH2oFlowSwitch.IsDown()) {
		primEvap->SetPumpOn();
		primEvap->throttle = 1;

	} else {
		if (saturn->GlycolEvapSteamPressAutoManSwitch.IsUp())
			primEvap->SetPumpAuto();
		else
			primEvap->SetPumpOn();
	}
}

void SaturnGlycolCoolingController::SecEvapSwitchesToggled(PanelSwitchItem *s) {

	if (saturn->SecCoolantLoopEvapSwitch.IsCenter() || saturn->EvapWaterControlSecondaryRotary.GetState() == 0) {
		secEvap->SetPumpOff(); 
	
	} else if (saturn->SecCoolantLoopEvapSwitch.IsUp()) {
		secEvap->SetPumpAuto(); 

	} else {
		secEvap->SetPumpOn(); 
		secEvap->throttle = 0;
	}
}

void SaturnGlycolCoolingController::H2oAccumSwitchesToggled(PanelSwitchItem *s) {

	if (saturn->SuitCircuitH2oAccumAutoSwitch.IsCenter() && saturn->SuitCircuitH2oAccumOnSwitch.IsCenter()) {
		saturn->SuitCompressor1->h_pumpH2o = SP_PUMP_OFF;
		saturn->SuitCompressor2->h_pumpH2o = SP_PUMP_OFF;
	} else if (((saturn->SuitCircuitH2oAccumAutoSwitch.IsUp() || saturn->SuitCircuitH2oAccumOnSwitch.IsUp()) && saturn->WaterAccumulator1Rotary.GetState() != 1) ||
			   ((saturn->SuitCircuitH2oAccumAutoSwitch.IsDown() || saturn->SuitCircuitH2oAccumOnSwitch.IsDown()) && saturn->WaterAccumulator2Rotary.GetState() != 1)) {
		saturn->SuitCompressor1->h_pumpH2o = SP_PUMP_ON;
		saturn->SuitCompressor2->h_pumpH2o = SP_PUMP_ON;
	} else {
		saturn->SuitCompressor1->h_pumpH2o = SP_PUMP_OFF;
		saturn->SuitCompressor2->h_pumpH2o = SP_PUMP_OFF;
	}
}

void SaturnGlycolCoolingController::CabinTempSwitchToggled(PanelSwitchItem *s) {

	double targetTemp = 294;
	if (saturn->CabinTempAutoManSwitch.IsUp()) {
		targetTemp = 294.0 + saturn->CabinTempAutoControlSwitch.GetState() * 6.0 / 9.0;
	} else {
		// Use the SecondaryCabinTempValve for now until there's a primary
		targetTemp = 294.0 + (4.0 - saturn->SecondaryCabinTempValve.GetState()) * 6.0 / 4.0;
	}
	saturn->PrimCabinHeatExchanger->tempMin = targetTemp;
	saturn->PrimCabinHeatExchanger->tempMax = targetTemp + 0.5;

	saturn->SecCabinHeatExchanger->tempMin = targetTemp;
	saturn->SecCabinHeatExchanger->tempMax = targetTemp + 0.5;

	saturn->CabinHeater->valueMin = targetTemp - 1.0;
	saturn->CabinHeater->valueMax = targetTemp;		
}

SaturnLMTunnelVent::SaturnLMTunnelVent()
{
	LMTunnelVentSwitch = NULL;
	TunnelVentValve = NULL;
	TunnelPressValve = NULL;
}

void SaturnLMTunnelVent::Init(h_Valve *tvv, h_Valve *tpv, RotationalSwitch *lmtvs)
{
	TunnelVentValve = tvv;
	TunnelPressValve = tpv;
	LMTunnelVentSwitch = lmtvs;
}

void SaturnLMTunnelVent::SystemTimestep(double simdt)
{
	if (!TunnelVentValve || !TunnelPressValve) return;

	// Valve in motion
	if (TunnelVentValve->pz) return;
	if (TunnelPressValve->pz) return;

	//OFF
	if (LMTunnelVentSwitch->GetState() == 0)
	{
		TunnelVentValve->Close();
		TunnelPressValve->Close();
	}
	//LM PRESS
	else if (LMTunnelVentSwitch->GetState() == 1)
	{
		TunnelVentValve->Close();
		TunnelPressValve->Open();
	}
	//LM/CM DELTA P
	else if (LMTunnelVentSwitch->GetState() == 2)
	{
		TunnelVentValve->Close();
		TunnelPressValve->Close();
	}
	//LM TUNNEL VENT
	else if (LMTunnelVentSwitch->GetState() == 3)
	{
		TunnelVentValve->Open();
		TunnelPressValve->Close();
	}
}

SaturnForwardHatch::SaturnForwardHatch(Sound &opensound, Sound &closesound) :
	OpenSound(opensound), CloseSound(closesound)
{
	open = false;
	toggle = 0;
	pipe = NULL;
	saturn = NULL;

	anim_FwdHatchVC = -1;
	anim_pressequalvlv = -1;
}

SaturnForwardHatch::~SaturnForwardHatch()
{

}

void SaturnForwardHatch::Init(Saturn *s, h_Pipe *p, RotationalSwitch *pev)
{
	saturn = s;
	pipe = p;
	pressureequalvalve = pev;
}

void SaturnForwardHatch::Toggle()
{
	if (!pipe) return;

	if (toggle == 0)
	{
		if (open == false)
		{
			if (pipe->in->parent->space.Press - pipe->out->parent->space.Press < 0.08 / PSI)
			{
				open = true;
				toggle = 2;
				OpenSound.play();
				saturn->SetAnimation(anim_FwdHatchVC, 1.0);
				saturn->SetFwdHatchMesh();
				saturn->SetDockingProbeMesh(); // Hide docking probe mesh when CM forward hatch is open
			}
		}
		else
		{
			//Is hose connected? If yes prevent hatch from being closed
			if (saturn->GetCSMO2Hose()->out != NULL) return;

			open = false;
			toggle = 2;
			CloseSound.play();
			saturn->SetAnimation(anim_FwdHatchVC, 0.0);
			saturn->SetFwdHatchMesh();
			saturn->SetDockingProbeMesh(); // Show docking probe mesh when CM forward hatch is closed
		}
	}
}

void SaturnForwardHatch::Timestep(double simdt) {

	if (toggle > 0) {
		toggle--;
		if (toggle == 0) {
			saturn->PanelRefreshForwardHatch();
		}
	}

	double equalvalve_state = (double)pressureequalvalve->GetState();
	saturn->SetAnimation(anim_pressequalvlv, equalvalve_state / 3);
}

void SaturnForwardHatch::LoadState(char *line) {

	int i1;

	sscanf(line + 12, "%d %d", &i1, &toggle);
	open = (i1 != 0);
}

void SaturnForwardHatch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i", (open ? 1 : 0), toggle);
	oapiWriteScenario_string(scn, "FORWARDHATCH", buffer);
}

void SaturnForwardHatch::DefineAnimationsVC(UINT idx)
{
	// Forward Hatch Animations
	ANIMATIONCOMPONENT_HANDLE ach_FwdHatchVC, ach_pressequalvlv;
	const VECTOR3 FwdHatch_Equal_ValveLocation = { 0.0011, -0.0000, 1.0773 };

	static UINT	meshgroup_FwdHatchVC[2] = { VC_GRP_FwdHatch_01, VC_GRP_FwdHatch_02 };
	static UINT	meshgroup_pressequalvalve = { VC_GRP_FwdHatch_Equal_Valve };

	static MGROUP_TRANSLATE mgt_fwdhatch(idx, meshgroup_FwdHatchVC, 2, _V(1, 0, 0));
	static MGROUP_ROTATE mgt_pressequalvalve(idx, &meshgroup_pressequalvalve, 1, FwdHatch_Equal_ValveLocation, _V(0, 0, -1), (float)(RAD * 90));

	anim_FwdHatchVC = saturn->CreateAnimation(0.0);
	anim_pressequalvlv = saturn->CreateAnimation(0.5);

	ach_FwdHatchVC = saturn->AddAnimationComponent(anim_FwdHatchVC, 0.0f, 1.0f, &mgt_fwdhatch);
	ach_pressequalvlv = saturn->AddAnimationComponent(anim_pressequalvlv, 0.0f, 1.0f, &mgt_pressequalvalve, ach_FwdHatchVC);

	saturn->SetAnimation(anim_FwdHatchVC, open);
	saturn->SetAnimation(anim_pressequalvlv, 0.5);
}

SaturnPressureEqualizationValve::SaturnPressureEqualizationValve()
{
	PressureEqualizationSwitch = NULL;
	PressureEqualizationValve = NULL;
	ForwardHatch = NULL;
}

void SaturnPressureEqualizationValve::Init(h_Pipe *pev, RotationalSwitch *pes, SaturnForwardHatch *h)
{
	PressureEqualizationSwitch = pes;
	PressureEqualizationValve = pev;
	ForwardHatch = h;
}

void SaturnPressureEqualizationValve::SystemTimestep(double simdt)
{
	if (!PressureEqualizationValve) return;

	// Valve in motion
	if (PressureEqualizationValve->in->pz) return;

	if (ForwardHatch->IsOpen())
	{
		//FORWARD HATCH

		PressureEqualizationValve->in->Open();
		PressureEqualizationValve->in->size = (float) 1000.0;
		PressureEqualizationValve->flowMax = 2000.0 / LBH;
	}
	else
	{
		//PRESSURE EQUALIZATION VALVE

		//CLOSED
		if (PressureEqualizationSwitch->GetState() == 3)
		{
			PressureEqualizationValve->in->Close();
		}
		else
		{
			double f = (double)(3 - PressureEqualizationSwitch->GetState());

			PressureEqualizationValve->in->Open();
			//PressureEqualizationValve->in->size = (float)(0.15*f); //position * 0.15 need to increase for faster LM Press
			PressureEqualizationValve->in->size = (float)(0.20 * f);
			PressureEqualizationValve->flowMax = 220.0 / LBH * f;
		}
	}
}

SaturnWasteStowageVentValve::SaturnWasteStowageVentValve()
{
	WasteStowageValve = NULL;
	WasteStowageSwitch = NULL;
}

void SaturnWasteStowageVentValve::Init(h_Valve* wsv, RotationalSwitch* wss)
{
	WasteStowageValve = wsv;
	WasteStowageSwitch = wss;
}

void SaturnWasteStowageVentValve::SystemTimestep(double simdt)
{
	if (!WasteStowageValve) return;

	// Valve in motion
	if (WasteStowageValve->pz) return;

	if (WasteStowageSwitch->GetState() == 1)
	{
		WasteStowageValve->Close();
	}

	else
		WasteStowageValve->Open();

}

SaturnBatteryVent::SaturnBatteryVent()
{
	BatteryVentSwitch = NULL;
	BatteryManifold = NULL;
}

void SaturnBatteryVent::Init(Saturn* s, RotationalSwitch* bvs, h_Tank* bmt)
{
	saturn = s;
	BatteryVentSwitch = bvs;
	BatteryManifold = bmt;
}

void SaturnBatteryVent::SystemTimestep(double simdt)
{
	if (!BatteryManifold) return;

	// Valve in motion
	if (BatteryManifold->OUT_valve.pz) return;

	if (BatteryVentSwitch->GetState() == 1 && saturn->WasteH2ODumpHeater.IsFrozen() == false)
	{
		BatteryManifold->OUT_valve.Open();
	}

	else
		BatteryManifold->OUT_valve.Close();
}

SaturnSuitFlowValves::SaturnSuitFlowValves()
{
	SuitFlowValve = NULL;
	SuitFlowSwitch = NULL;
}

void SaturnSuitFlowValves::Init(h_Valve* flowvlv, ThreePosSwitch* flowsw)
{
	SuitFlowValve = flowvlv;
	SuitFlowSwitch = flowsw; //0=Full Flow 1=Cabin Flow 2=Close
}

void SaturnSuitFlowValves::SystemTimestep(double simdt)
{
	if (!SuitFlowValve) return;

	// Valve in motion
	if (SuitFlowValve->pz) return;

	if (SuitFlowSwitch->GetState() == 2)
	{
		SuitFlowValve->Close();
	}

	else
		SuitFlowValve->Open();

}

SaturnDumpHeater::SaturnDumpHeater()
{

}

void SaturnDumpHeater::Init(Saturn* s, h_Radiator* noz, Boiler* ha, Boiler* sha, Boiler* hb, Boiler* shb, CircuitBrakerSwitch* cba, CircuitBrakerSwitch* cbb, ThreePosSwitch* sw)
{
	saturn = s;
	nozzle = noz;
	heaterA = ha;
	stripheaterA = sha;
	heaterB = hb;
	stripheaterB = shb;
	circuitbreakerA = cba;
	circuitbreakerB = cbb;
	powerswitch = sw;

	temp = 0.0;
}

double SaturnDumpHeater::GetTemperatureF()
{
	return KelvinToFahrenheit(nozzle->GetTemp());
}

bool SaturnDumpHeater::IsFrozen()
{
	if (GetTemperatureF() < 32.0)
	{
		return true;
	}

	return false;
}

void SaturnDumpHeater::SystemTimestep(double simdt)
{
	//System A
	if (circuitbreakerA->IsPowered())
	{
		stripheaterA->SetPumpOn();

		if (powerswitch->IsUp())
		{
			heaterA->SetPumpOn();
		}

		else
		{
			heaterA->SetPumpOff();
		}
	}

	else
	{
		stripheaterA->SetPumpOff();
		heaterA->SetPumpOff();
	}

	//System B
	if (circuitbreakerB->IsPowered())
	{
		stripheaterB->SetPumpOn();

		if (powerswitch->IsDown())
		{
			heaterB->SetPumpOn();
		}

		else
		{
			heaterB->SetPumpOff();
		}
	}

	else
	{
		stripheaterB->SetPumpOff();
		heaterB->SetPumpOff();
	}
}