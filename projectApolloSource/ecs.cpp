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

  **************************** Revision History ****************************
  *	$Log$
  **************************** Revision History ****************************/

#include <stdio.h>
#include "orbitersdk.h"

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Hsystems.h"

#include "OrbiterSoundSDK35.h"
#include "soundlib.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "ioChannels.h"

#include "saturn.h"


CabinPressureRegulator::CabinPressureRegulator() {

	pipe = NULL;
	closed = false;
}

CabinPressureRegulator::~CabinPressureRegulator() {

}

void CabinPressureRegulator::Init(h_Pipe* p) {

	pipe = p;	
}

void CabinPressureRegulator::SystemTimestep(double simdt) {
	
	if (!pipe) return;
	// Valve in motion
	if (pipe->in->pz) return;

	// Forcibly closed?
	if (closed) {
		pipe->in->Close();
	} else {
		// Close, if cabin pressure below 3.5 psi
		if (pipe->out->parent->space.Press < 3.5 / PSI) {
			pipe->in->Close();
		} else {
			pipe->in->Open();
		}
	}
}

void CabinPressureRegulator::Reset() {

	closed = false;
}

void CabinPressureRegulator::Close() {

	closed = true;
}

void CabinPressureRegulator::SetPressurePSI(double p) {

	if (!pipe) return;
	pipe->P_max = p / PSI;
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

	sscanf(line + 22, "%d", &i);
	closed = (i != 0);
}

void CabinPressureRegulator::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int(scn, "CABINPRESSUREREGULATOR", (closed ? 1 : 0));
}


O2DemandRegulator::O2DemandRegulator() {

	pipe = NULL;
	suitReliefValve = NULL;
	closed = false;
	suitReliefValveOpen = false; 
}

O2DemandRegulator::~O2DemandRegulator() {

}

void O2DemandRegulator::Init(h_Pipe *p, h_Pipe *s) {

	pipe = p;
	suitReliefValve = s;	
}

void O2DemandRegulator::SystemTimestep(double simdt) {

	double cabinPress = suitReliefValve->out->parent->space.Press;
	double suitPress = suitReliefValve->in->parent->space.Press;

	// O2 demand regulator
	if (!pipe->in->pz) {		// is it moving?
		// Forcibly closed or suit circuit return valve is open 
		if (closed || suitReliefValve->out->parent->OUT_valve.open) {
			pipe->in->Close();
		} else {
			double dp = suitPress - max(3.75 / PSI, cabinPress);
			// TODO one/both regulators
			pipe->in->Open();
			pipe->P_max = max(3.75 / PSI, cabinPress) + 2.5 / INH2O;

			if (dp > -5.5 / INH2O) {
				pipe->flowMax = 2. * 0.42 / LBH; // 0.007 lb/min per regulator (Systems Handbook)
			} else if (cabinPress > 3.75 / PSI) {
				pipe->flowMax = 2. * 9. / LBH; // 0.3 lb/min both (Systems Handbook)
			} else {
				pipe->flowMax = 2. * 19.8 / LBH; // 0.66 lb/min both (Systems Handbook)
			}
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
			// Disable if suit circuit return valve is open 
			if (suitReliefValve->out->parent->OUT_valve.open) {
				suitReliefValve->in->Close();
			} else {
				// Open if suit is overpressured (see AOH)			
				if (suitPress > 3.75 / PSI + 3.0 / INH2O && cabinPress <= 3.75 / PSI) { 
					suitReliefValve->in->Open();
					suitReliefValve->P_max = 1000. / PSI;	// Disable PREG
				} else if (cabinPress > 3.75 / PSI) {								
					suitReliefValve->in->Open();
					suitReliefValve->P_max = suitPress - 3.0 / INH2O;
				} else {
					suitReliefValve->in->Close();
				}
			}
			suitReliefValve->flowMax = 39.6 / LBH; // 0.66 lb/min (Systems Handbook)
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

void CabinPressureReliefValve::Init(h_Pipe *p, h_Pipe *i, VESSEL *v, ThumbwheelSwitch *l, CircuitBrakerSwitch *plvlv, ThreePosSwitch *plv, e_object *plpower) {

	pipe = p;
	inlet = i;
	saturn = v;
	lever = l;
	postLandingValve = plvlv;
	postLandingVent = plv;
	postLandingPower = plpower;
}

void CabinPressureReliefValve::SystemTimestep(double simdt) {

	if (!pipe && !inlet) return;
	// Valve in motion
	if (pipe->in->pz || inlet->in->pz) return;

	// Post Landing Vent
	double cabinPress = pipe->in->parent->space.Press;
	if (postLandingValve->IsDown() && !postLandingVent->IsDown() && postLandingPower->Voltage() > SP_MIN_DCVOLTAGE) {
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
		if (postLandingVent->IsUp()) {
			postLandingPower->DrawPower(25.5);	// systems handbook
			postLandingVentSound.play(LOOP, 255); 
		} else {
			postLandingPower->DrawPower(22.1);	// systems handbook
			postLandingVentSound.play(LOOP, 170); 
		}
		return;
	}
	postLandingVentSound.stop();

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
}

O2SMSupply::~O2SMSupply() {

}

void O2SMSupply::Init(h_Tank *o2sm, h_Tank *o2mr, h_Tank *o2st, h_Tank *o2rp, 
					  RotationalSwitch *smv, RotationalSwitch *stv, RotationalSwitch *rpv) {

	o2SMSupply = o2sm;	
	o2MainRegulator = o2mr;
	o2SurgeTank = o2st;
	o2RepressPackage = o2rp;
	smSupplyValve = smv;	
	surgeTankValve = stv;
	repressPackageValve = rpv;
}

void O2SMSupply::SystemTimestep(double simdt) {

	// Is something moving?
	if (o2SMSupply->IN_valve.pz || o2SMSupply->OUT2_valve.pz || o2SMSupply->LEAK_valve.pz || 
		o2SurgeTank->IN_valve.pz || o2RepressPackage->IN_valve.pz) return;

	bool allClosed = true;

	// SM supply, forcibly closed after SM separation 
	if (closed || smSupplyValve->GetState() == 0) {
		o2SMSupply->IN_valve.Close();			
	} else {
		o2SMSupply->IN_valve.Open();
		allClosed = false;
	}

	// Surge tank
	if (surgeTankValve->GetState() == 0) {
		o2SMSupply->OUT2_valve.Close();	
		o2SurgeTank->IN_valve.Close();
	} else {
		// SM supply open? 
		if (o2SMSupply->IN_valve.open) {
			o2SurgeTank->IN_valve.Open();
		} else {
			o2SurgeTank->IN_valve.Close();
		}
		o2SMSupply->OUT2_valve.Open();
		allClosed = false;
	}

	// Repress package
	if (repressPackageValve->GetState() == 1) {
		o2SMSupply->LEAK_valve.Close();	
		o2RepressPackage->IN_valve.Close();
	} else {
		// SM supply open? 
		if (o2SMSupply->IN_valve.open) {
			o2RepressPackage->IN_valve.Open();
		} else {
			o2RepressPackage->IN_valve.Close();
		}
		// On or Fill?
		if (repressPackageValve->GetState() == 2) {
			o2SMSupply->LEAK_valve.Open();
			allClosed = false;
		} else {
			o2SMSupply->LEAK_valve.Close();	
		}
	}

	// Purge "pipe tanks" in case of no supply
	if (allClosed) {
		o2SMSupply->space.Void();
		o2MainRegulator->space.Void();
	}
}

void O2SMSupply::Close() {

	closed = true;
}

void O2SMSupply::LoadState(char *line) {

	int i;

	sscanf(line + 10, "%d", &i);
	closed = (i != 0);
}

void O2SMSupply::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i", (closed ? 1 : 0)); 
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

	AtmosStatus atm;
	saturn->GetAtmosStatus(atm);
	status = ECS_CREWSTATUS_OK;

	// Pressure in suit lower than 2.8 psi for 10 minutes
	if (atm.SuitPressurePSI < 2.8) {
		if (suitPressureLowTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 

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

		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			suitPressureHighTime -= simdt;
		}
	} else {
		suitPressureHighTime = 3600;
	}

	// Suit temperature above about 45°C or below about 0°C for 12 hours
	if (atm.SuitTempK > 320 || atm.SuitTempK < 270) {
		if (suitTemperatureTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 

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

		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			suitCO2Time -= simdt;
		}
	} else {
		suitPressureHighTime = 1800;
	}

	// Acceleration exceeds 12 g for 10 seconds
	VECTOR3 f, w;
	saturn->GetForceVector(f);
	saturn->GetWeightVector(w);
	if (length(f - w) / saturn->GetMass() > 12. * G) {
		if (accelerationTime <= 0) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 

		} else {
			status = ECS_CREWSTATUS_CRITICAL;
			accelerationTime -= simdt;
		}
	} else {
		accelerationTime = 10;
	}

	// Touchdown speed exceeds 15 m/s (= about 50 ft/s)
	if (saturn->GroundContact()) {
		if (fabs(lastVerticalVelocity) > 15) {
			status = ECS_CREWSTATUS_DEAD;
			crewDeadSound.play(); 
		}
	} else {
		VECTOR3 v;
		saturn->GetHorizonAirspeedVector(v);
		lastVerticalVelocity = v.y;
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

