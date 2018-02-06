/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Environmental Control System

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
#include "LEM.h"
#include "lm_ecs.h"

LEMOverheadHatch::LEMOverheadHatch(Sound &opensound, Sound &closesound) :
	OpenSound(opensound), CloseSound(closesound)
{
	open = false;
	ovhdHatchHandle = NULL;
	lem = NULL;
}

void LEMOverheadHatch::Init(LEM *l, ToggleSwitch *fhh)
{
	lem = l;
	ovhdHatchHandle = fhh;
}

void LEMOverheadHatch::Toggle()
{
	if (open == false)
	{
		if (ovhdHatchHandle->GetState() == 1)
		{
			open = true;
			OpenSound.play();
			lem->PanelRefreshOverheadHatch();
			lem->SetOvhdHatchMesh();
		}
	}
	else
	{
		open = false;
		CloseSound.play();
		lem->PanelRefreshOverheadHatch();
		lem->SetOvhdHatchMesh();
	}
}

void LEMOverheadHatch::LoadState(char *line) {

	int i1;

	sscanf(line + 13, "%d", &i1);
	open = (i1 != 0);
}

void LEMOverheadHatch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i", (open ? 1 : 0));
	oapiWriteScenario_string(scn, "OVERHEADHATCH", buffer);
}

LEMOVHDCabinReliefDumpValve::LEMOVHDCabinReliefDumpValve()
{
	cabinOVHDHatchValve = NULL;
	cabinOVHDHatchValveSwitch = NULL;
	ovhdHatch = NULL;
}

void LEMOVHDCabinReliefDumpValve::Init(h_Pipe *cohv, ThreePosSwitch *cohs, LEMOverheadHatch *oh)
{
	cabinOVHDHatchValve = cohv;
	cabinOVHDHatchValveSwitch = cohs;
	ovhdHatch = oh;
}

void LEMOVHDCabinReliefDumpValve::SystemTimestep(double simdt)
{
	if (!cabinOVHDHatchValve) return;

	// Valve in motion
	if (cabinOVHDHatchValve->in->pz) return;

	if (ovhdHatch->IsOpen())
	{
		cabinOVHDHatchValve->in->Open();
		cabinOVHDHatchValve->in->size = (float) 100.;	// no pressure in a few seconds
		cabinOVHDHatchValve->flowMax = 2000. / LBH;
	}
	else
	{
		cabinOVHDHatchValve->in->size = (float) 10.;
		//DUMP
		if (cabinOVHDHatchValveSwitch->GetState() == 0)
		{
			cabinOVHDHatchValve->flowMax = 660.0 / LBH;
			cabinOVHDHatchValve->in->Open();
		}
		//CLOSE
		else if (cabinOVHDHatchValveSwitch->GetState() == 2)
		{
			cabinOVHDHatchValve->flowMax = 0;
			cabinOVHDHatchValve->in->Close();
		}
		//AUTO
		else if (cabinOVHDHatchValveSwitch->GetState() == 1)
		{
			double cabinpress = cabinOVHDHatchValve->in->parent->space.Press;

			if (cabinpress > 5.4 / PSI && cabinOVHDHatchValve->in->open == 0)
			{
				cabinOVHDHatchValve->in->Open();
			}
			else if (cabinpress < 5.25 / PSI && cabinOVHDHatchValve->in->open == 1)
			{
				cabinOVHDHatchValve->in->Close();
			}

			if (cabinOVHDHatchValve->in->open == 1)
			{
				if (cabinpress > 5.8 / PSI)
				{
					cabinOVHDHatchValve->flowMax = 660.0 / LBH;
				}
				else if (cabinpress < 5.25 / PSI)
				{
					cabinOVHDHatchValve->flowMax = 0;
				}
				else
				{
					//0 flow at 5.25 psi, full flow at 5.8 psi
					cabinOVHDHatchValve->flowMax = (660.0 / LBH) * (1.81818*(cabinpress*PSI) - 9.54545);
				}
			}
			else
			{
				cabinOVHDHatchValve->flowMax = 0;
			}
		}
	}
}


LEMForwardHatch::LEMForwardHatch(Sound &opensound, Sound &closesound) :
	OpenSound(opensound), CloseSound(closesound)
{
	open = false;
	ForwardHatchHandle = NULL;
	lem = NULL;
}

void LEMForwardHatch::Init(LEM *l, ToggleSwitch *fhh)
{
	lem = l;
	ForwardHatchHandle = fhh;
}

void LEMForwardHatch::Toggle()
{
	if (open == false)
	{
		if (ForwardHatchHandle->GetState() == 1)
		{
			open = true;
			OpenSound.play();
			lem->PanelRefreshForwardHatch();
			lem->SetFwdHatchMesh();
		}
	}
	else
	{
		open = false;
		CloseSound.play();
		lem->PanelRefreshForwardHatch();
		lem->SetFwdHatchMesh();
	}
}

void LEMForwardHatch::LoadState(char *line) {

	int i1;

	sscanf(line + 12, "%d", &i1);
	open = (i1 != 0);
}

void LEMForwardHatch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i", (open ? 1 : 0));
	oapiWriteScenario_string(scn, "FORWARDHATCH", buffer);
}

LEMFWDCabinReliefDumpValve::LEMFWDCabinReliefDumpValve()
{
	cabinFWDHatchValve = NULL;
	cabinFWDHatchValveSwitch = NULL;
	fwdHatch = NULL;
}

void LEMFWDCabinReliefDumpValve::Init(h_Pipe *cfv, ThreePosSwitch *cfvs, LEMForwardHatch *fh)
{
	cabinFWDHatchValve = cfv;
	cabinFWDHatchValveSwitch = cfvs;
	fwdHatch = fh;
}

void LEMFWDCabinReliefDumpValve::SystemTimestep(double simdt)
{
	if (!cabinFWDHatchValve) return;

	// Valve in motion
	if (cabinFWDHatchValve->in->pz) return;

	if (fwdHatch->IsOpen())
	{
		cabinFWDHatchValve->in->Open();
		cabinFWDHatchValve->in->size = (float) 100.;	// no pressure in a few seconds
		cabinFWDHatchValve->flowMax = 2000. / LBH;
	}
	else
	{
		cabinFWDHatchValve->in->size = (float) 10.;

		//DUMP
		if (cabinFWDHatchValveSwitch->GetState() == 0)
		{
			cabinFWDHatchValve->flowMax = 660.0 / LBH;
			cabinFWDHatchValve->in->Open();
		}
		//CLOSE
		else if (cabinFWDHatchValveSwitch->GetState() == 2)
		{
			cabinFWDHatchValve->flowMax = 0;
			cabinFWDHatchValve->in->Close();
		}
		//AUTO
		else if (cabinFWDHatchValveSwitch->GetState() == 1)
		{
			double cabinpress = cabinFWDHatchValve->in->parent->space.Press;

			if (cabinpress > 5.4 / PSI && cabinFWDHatchValve->in->open == 0)
			{
				cabinFWDHatchValve->in->Open();
			}
			else if (cabinpress < 5.25 / PSI && cabinFWDHatchValve->in->open == 1)
			{
				cabinFWDHatchValve->in->Close();
			}

			if (cabinFWDHatchValve->in->open == 1)
			{
				if (cabinpress > 5.8 / PSI)
				{
					cabinFWDHatchValve->flowMax = 660.0 / LBH;
				}
				else if (cabinpress < 5.25 / PSI)
				{
					cabinFWDHatchValve->flowMax = 0;
				}
				else
				{
					//0 flow at 5.25 psi, full flow at 5.8 psi
					cabinFWDHatchValve->flowMax = (660.0 / LBH) * (1.81818*(cabinpress*PSI) - 9.54545);
				}
			}
			else
			{
				cabinFWDHatchValve->flowMax = 0;
			}
		}
	}
}

LEMSuitCircuitReliefValve::LEMSuitCircuitReliefValve()
{
	SuitCircuitReliefValve = NULL;
	SuitCircuitReliefValveSwitch = NULL;
}

void LEMSuitCircuitReliefValve::Init(h_Pipe *scrv, RotationalSwitch *scrvs)
{
	SuitCircuitReliefValve = scrv;
	SuitCircuitReliefValveSwitch = scrvs;
}

void LEMSuitCircuitReliefValve::SystemTimestep(double simdt)
{
	if (!SuitCircuitReliefValve) return;

	// Valve in motion
	if (SuitCircuitReliefValve->in->pz) return;

	//OPEN
	if (SuitCircuitReliefValveSwitch->GetState() == 0)
	{
		SuitCircuitReliefValve->flowMax = 7.8 / LBH;
		SuitCircuitReliefValve->in->Open();
	}
	//CLOSE
	else if (SuitCircuitReliefValveSwitch->GetState() == 2)
	{
		SuitCircuitReliefValve->flowMax = 0;
		SuitCircuitReliefValve->in->Close();
	}
	//AUTO
	else if (SuitCircuitReliefValveSwitch->GetState() == 1)
	{
		double suitcircuitpress = SuitCircuitReliefValve->in->parent->space.Press;

		if (suitcircuitpress > 4.3 / PSI)
		{
			SuitCircuitReliefValve->in->Open();
		}
		else
		{
			SuitCircuitReliefValve->in->Close();
		}

		if (SuitCircuitReliefValve->in->open == 1)
		{
			if (suitcircuitpress > 4.7 / PSI)
			{
				SuitCircuitReliefValve->flowMax = 7.8 / LBH;
			}
			else if (suitcircuitpress <= 4.3 / PSI)
			{
				SuitCircuitReliefValve->flowMax = 0;
			}
			else
			{
				//0 flow at 4.3 psi, full flow at 4.7 psi
				SuitCircuitReliefValve->flowMax = (7.8 / LBH) * (2.5*(suitcircuitpress*PSI) - 10.75);
			}
		}
		else
		{
			SuitCircuitReliefValve->flowMax = 0;
		}
	}
}

LEMCabinRepressValve::LEMCabinRepressValve()
{
	cabinRepressValve = NULL;
	cabinRepressValveSwitch = NULL;
	cabinRepressCB = NULL;
	pressRegulatorASwitch = NULL;
	pressRegulatorBSwitch = NULL;
	EmergencyCabinRepressRelay = false;
}

void LEMCabinRepressValve::Init(h_Pipe *crv, CircuitBrakerSwitch *crcb, RotationalSwitch *crvs, RotationalSwitch* pras, RotationalSwitch *prbs)
{
	cabinRepressValve = crv;
	cabinRepressValveSwitch = crvs;
	cabinRepressCB = crcb;
	pressRegulatorASwitch = pras;
	pressRegulatorBSwitch = prbs;
}

void LEMCabinRepressValve::SystemTimestep(double simdt)
{
	if (!cabinRepressValve) return;

	// Valve in motion
	if (cabinRepressValve->in->pz) return;

	EmergencyCabinRepressRelay = false;

	//MANUAL
	if (cabinRepressValveSwitch->GetState() == 0)
	{
		cabinRepressValve->flowMax = 480.0 / LBH;
		cabinRepressValve->in->Open();
	}
	//CLOSE
	else if (cabinRepressValveSwitch->GetState() == 2)
	{
		cabinRepressValve->flowMax = 0;
		cabinRepressValve->in->Close();
	}
	//AUTO
	else if (cabinRepressValveSwitch->GetState() == 1)
	{
		cabinRepressValve->flowMax = 396.0 / LBH;

		if (cabinRepressCB->IsPowered() && !pressRegulatorASwitch->GetState() == 0 && !pressRegulatorBSwitch->GetState() == 0)
		{
			//Cabin pressure
			double cabinpress = cabinRepressValve->out->parent->space.Press;

			if (cabinpress < 4.07 / PSI && cabinRepressValve->in->open == 0)
			{
				cabinRepressValve->in->Open();
				EmergencyCabinRepressRelay = true;
			}
			else if (cabinpress > 4.7 / PSI && cabinRepressValve->in->open == 1)
			{
				cabinRepressValve->in->Close();
			}
		}
		else
		{
			cabinRepressValve->in->Close();
		}
	}
}

LEMSuitCircuitPressureRegulator::LEMSuitCircuitPressureRegulator()
{
	pressRegulatorSwitch = NULL;
	pressRegulatorValve = NULL;
	suitCircuit = NULL;
}

void LEMSuitCircuitPressureRegulator::Init(h_Pipe *prv, h_Tank *sc, RotationalSwitch *prs)
{
	pressRegulatorSwitch = prs;
	pressRegulatorValve = prv;
	suitCircuit = sc;
}

void LEMSuitCircuitPressureRegulator::SystemTimestep(double simdt)
{
	
	if (!pressRegulatorValve) return;

	// Valve in motion
	if (pressRegulatorValve->in->pz) return;

	//Suit pressure
	double suitpress = suitCircuit->space.Press;

	//DIRECT O2
	if (pressRegulatorSwitch->GetState() == 2)
	{
		pressRegulatorValve->in->Open();
	}
	//CLOSE
	else if (pressRegulatorSwitch->GetState() == 3)
	{
		pressRegulatorValve->in->Close();
	}
	//EGRESS
	else if (pressRegulatorSwitch->GetState() == 0)
	{
		if (suitpress < 3.8 / PSI)
		{
			pressRegulatorValve->in->Open();
		}
		else
		{
			pressRegulatorValve->in->Close();
		}
	}
	//CABIN
	else if (pressRegulatorSwitch->GetState() == 1)
	{
		if (suitpress < 4.8 / PSI)
		{
			pressRegulatorValve->in->Open();
		}
		else
		{
			pressRegulatorValve->in->Close();
		}
	}
}

LEMSuitGasDiverter::LEMSuitGasDiverter()
{
	suitGasDiverterValve = NULL;
	cabin = NULL;
	suitGasDiverterSwitch = NULL;
	DivertVLVCB = NULL;
	pressRegulatorASwitch = NULL;
	pressRegulatorBSwitch = NULL;
}

void LEMSuitGasDiverter::Init(h_Tank *sgdv, h_Tank *cab, CircuitBrakerSwitch *sgds, CircuitBrakerSwitch *dvcb, RotationalSwitch* pras, RotationalSwitch *prbs)
{
	suitGasDiverterValve = sgdv;
	cabin = cab;
	suitGasDiverterSwitch = sgds;
	DivertVLVCB = dvcb;
	pressRegulatorASwitch = pras;
	pressRegulatorBSwitch = prbs;
}

void LEMSuitGasDiverter::SystemTimestep(double simdt)
{
	if (!suitGasDiverterValve) return;

	// Valve in motion
	if (suitGasDiverterValve->OUT_valve.pz) return;
	if (suitGasDiverterValve->OUT2_valve.pz) return;

	//Solenoid
	if (suitGasDiverterSwitch->GetState() == 1 && DivertVLVCB->IsPowered())
	{
		//Cabin pressure
		double cabinpress = cabin->space.Press;

		if (cabinpress < 4.0 / PSI || pressRegulatorASwitch->GetState() == 0 || pressRegulatorBSwitch->GetState() == 0)
		{
			suitGasDiverterSwitch->SwitchTo(0);
		}
	}

	//EGRESS
	if (suitGasDiverterSwitch->GetState() == 0)
	{
		suitGasDiverterValve->OUT_valve.Close();
		suitGasDiverterValve->OUT2_valve.Open();
	}
	//CABIN
	else
	{
		suitGasDiverterValve->OUT_valve.Open();
		suitGasDiverterValve->OUT2_valve.Close();
	}
}

LEMCO2CanisterSelect::LEMCO2CanisterSelect()
{
	PrimCO2Canister = NULL;
	SecCO2Canister = NULL;
	CO2CanisterSelectSwitch = NULL;
}

void LEMCO2CanisterSelect::Init(h_Tank *pco2, h_Tank *sco2, ToggleSwitch* co2s)
{
	PrimCO2Canister = pco2;
	SecCO2Canister = sco2;
	CO2CanisterSelectSwitch = co2s;
}

void LEMCO2CanisterSelect::SystemTimestep(double simdt)
{

	if (!PrimCO2Canister) return;

	if (!SecCO2Canister) return;

	// Valve in motion

	if (PrimCO2Canister->IN_valve.pz) return;

	//PRIM
	if (CO2CanisterSelectSwitch->GetState() == 1)
	{
		PrimCO2Canister->IN_valve.Open();
		PrimCO2Canister->OUT_valve.Open();
		SecCO2Canister->IN_valve.Close();
		SecCO2Canister->OUT_valve.Close();
	}
	//SEC
	else
	{
		PrimCO2Canister->IN_valve.Close();
		PrimCO2Canister->OUT_valve.Close();
		SecCO2Canister->IN_valve.Open();
		SecCO2Canister->OUT_valve.Open();
	}
}

LEMCO2CanisterVent::LEMCO2CanisterVent()
{
	CO2Canister = NULL;
	CO2CanisterVentSwitch = NULL;
}

void LEMCO2CanisterVent::Init(h_Tank *co2c, PushSwitch *co2vs)
{
	CO2Canister = co2c;
	CO2CanisterVentSwitch = co2vs;
}

void LEMCO2CanisterVent::SystemTimestep(double simdt)
{

	if (!CO2Canister) return;

	if (!CO2CanisterVentSwitch) return;

	// Valve in motion
	if (CO2Canister->OUT2_valve.pz) return;

	if (CO2CanisterVentSwitch->GetState() == 1)
	{
		CO2Canister->OUT2_valve.Open();
	}
	else
	{
		CO2Canister->OUT2_valve.Close();
	}
}

LEMCabinGasReturnValve::LEMCabinGasReturnValve()
{
	cabinGasReturnValve = NULL;
	cabinGasReturnValveSwitch = NULL;
}

void LEMCabinGasReturnValve::Init(h_Pipe * cgrv, RotationalSwitch *cgrvs)
{
	cabinGasReturnValve = cgrv;
	cabinGasReturnValveSwitch = cgrvs;
}

void LEMCabinGasReturnValve::SystemTimestep(double simdt)
{
	if (!cabinGasReturnValve) return;

	// Valve in motion
	if (cabinGasReturnValve->in->pz) return;

	//OPEN
	if (cabinGasReturnValveSwitch->GetState() == 0)
	{
		cabinGasReturnValve->in->Open();
	}
	//EGRESS
	else if (cabinGasReturnValveSwitch->GetState() == 2)
	{
		cabinGasReturnValve->in->Close();
	}
	//AUTO
	else if (cabinGasReturnValveSwitch->GetState() == 1)
	{
		if (cabinGasReturnValve->out->parent->space.Press > cabinGasReturnValve->in->parent->space.Press)
		{
			cabinGasReturnValve->in->Open();
		}
		else
		{
			cabinGasReturnValve->in->Close();
		}
	}
}

LEMWaterSeparationSelector::LEMWaterSeparationSelector()
{
	WaterSeparationSelectorValve = NULL;
	WaterSeparationSelectorSwitch = NULL;
}

void LEMWaterSeparationSelector::Init(h_Tank *wssv, CircuitBrakerSwitch* wsss)
{
	WaterSeparationSelectorValve = wssv;
	WaterSeparationSelectorSwitch = wsss;
}

void LEMWaterSeparationSelector::SystemTimestep(double simdt)
{
	if (!WaterSeparationSelectorValve) return;

	// Valve in motion
	if (WaterSeparationSelectorValve->OUT_valve.pz) return;
	if (WaterSeparationSelectorValve->OUT2_valve.pz) return;

	//SEP1
	if (WaterSeparationSelectorSwitch->GetState())
	{
		WaterSeparationSelectorValve->OUT_valve.Open();
		WaterSeparationSelectorValve->OUT2_valve.Close();
	}
	//SEP2
	else
	{
		WaterSeparationSelectorValve->OUT_valve.Close();
		WaterSeparationSelectorValve->OUT2_valve.Open();
	}
}

LEMCabinFan::LEMCabinFan(Sound &cabinfanS) : cabinfansound(cabinfanS)
{
	cabinFan1CB = NULL;
	cabinFanContCB = NULL;
	pressRegulatorASwitch = NULL;
	pressRegulatorBSwitch = NULL;
	cabinFan = NULL;
}

void LEMCabinFan::Init(CircuitBrakerSwitch *cf1cb, CircuitBrakerSwitch *cfccb, RotationalSwitch *pras, RotationalSwitch *prbs, Pump *cf)
{
	cabinFan1CB = cf1cb;
	cabinFanContCB = cfccb;
	pressRegulatorASwitch = pras;
	pressRegulatorBSwitch = prbs;
	cabinFan = cf;
}

void LEMCabinFan::SystemTimestep(double simdt)
{
	bool cabinFanSwitch;

	if (cabinFanContCB->IsPowered() && (pressRegulatorASwitch->GetState() == 0 || pressRegulatorBSwitch->GetState() == 0))
	{
		cabinFanContCB->DrawPower(1.1);
		cabinFanSwitch = true;
	}
	else
	{
		cabinFanSwitch = false;
	}

	if (cabinFan1CB->IsPowered() && !cabinFanSwitch)
	{
		cabinFan->SetPumpOn();
		CabinFanSound();

		//TBD: Switching heat exchanger on I guess?
	}
	else
	{
		cabinFan->SetPumpOff();
		StopCabinFanSound();
		//TBD: Switching heat exchanger off I guess?
	}
}

void LEMCabinFan::CabinFanSound()
{
	cabinfansound.play(LOOP, 200);
}

void LEMCabinFan::StopCabinFanSound()
{
	cabinfansound.stop();
}

LEMWaterTankSelect::LEMWaterTankSelect()
{
	WaterTankSelect = NULL;
	WaterTankSelectSwitch = NULL;
	SurgeTank = NULL;
}

void LEMWaterTankSelect::Init(h_Tank* wts, h_Tank *st, RotationalSwitch *wtss)
{
	WaterTankSelect = wts;
	WaterTankSelectSwitch = wtss;
	SurgeTank = st;
}

void LEMWaterTankSelect::SystemTimestep(double simdt)
{
	if (!WaterTankSelect) return;
	if (!SurgeTank) return;

	// Valve in motion
	if (WaterTankSelect->IN_valve.pz) return;
	if (SurgeTank->OUT2_valve.pz) return;

	//DES
	if (WaterTankSelectSwitch->GetState() == 0)
	{
		WaterTankSelect->IN_valve.Open();
		WaterTankSelect->OUT_valve.Open();
		WaterTankSelect->OUT2_valve.Close();
		SurgeTank->OUT_valve.Open();
		SurgeTank->OUT2_valve.Close();
	}
	//ASC
	else if (WaterTankSelectSwitch->GetState() == 1)
	{
		WaterTankSelect->IN_valve.Close();
		WaterTankSelect->OUT_valve.Open();
		WaterTankSelect->OUT2_valve.Open();
		SurgeTank->OUT_valve.Open();
		SurgeTank->OUT2_valve.Close();
	}
	//SEC
	else if (WaterTankSelectSwitch->GetState() == 2)
	{
		WaterTankSelect->IN_valve.Close();
		WaterTankSelect->OUT_valve.Close();
		WaterTankSelect->OUT2_valve.Open();
		SurgeTank->OUT_valve.Close();
		SurgeTank->OUT2_valve.Open();
	}
}

LEMPrimGlycolPumpController::LEMPrimGlycolPumpController()
{
	primGlycolAccumulatorTank = NULL;
	primGlycolPumpManifoldTank = NULL;
	glycolPump1CB = NULL;
	glycolPump2CB = NULL;
	glycolPumpAutoTransferCB = NULL;
	glycolRotary = NULL;
	glycolPump1 = NULL;
	glycolPump2 = NULL;

	GlycolAutoTransferRelay = false;
	GlycolPumpFailRelay = false;
	PressureSwitch = true;
}

void LEMPrimGlycolPumpController::Init(h_Tank *pgat, h_Tank *pgpmt, Pump *gp1, Pump *gp2, RotationalSwitch *gr, CircuitBrakerSwitch *gp1cb, CircuitBrakerSwitch *gp2cb, CircuitBrakerSwitch *gpatcb)
{
	primGlycolAccumulatorTank = pgat;
	primGlycolPumpManifoldTank = pgpmt;
	glycolPump1 = gp1;
	glycolPump2 = gp2;
	glycolRotary = gr;
	glycolPump1CB = gp1cb;
	glycolPump2CB = gp2cb;
	glycolPumpAutoTransferCB = gpatcb;
}

void LEMPrimGlycolPumpController::SystemTimestep(double simdt)
{
	if (!primGlycolPumpManifoldTank || !primGlycolAccumulatorTank) return;

	double DPSensor = primGlycolPumpManifoldTank->space.Press - primGlycolAccumulatorTank->space.Press;

	if (PressureSwitch == false && DPSensor < 3.0 / PSI)
	{
		PressureSwitch = true;
	}
	else if (PressureSwitch == true && DPSensor > 7.0 / PSI)
	{
		PressureSwitch = false;
	}

	if (PressureSwitch && glycolRotary->GetState() == 1 && glycolPumpAutoTransferCB->IsPowered())
	{
		GlycolAutoTransferRelay = true;
	}
	else if (glycolRotary->GetState() == 2 && glycolPumpAutoTransferCB->IsPowered())
	{
		GlycolAutoTransferRelay = false;
	}

	if (GlycolAutoTransferRelay && glycolRotary->GetState() == 1 && glycolPump1CB->IsPowered())
	{
		GlycolPumpFailRelay = true;
	}
	else
	{
		GlycolPumpFailRelay = false;
	}

	//PUMP 1
	if (glycolRotary->GetState() == 1 && !GlycolAutoTransferRelay && glycolPump1CB->IsPowered())
	{
		glycolPump1->SetPumpOn();
	}
	else
	{
		glycolPump1->SetPumpOff();
	}

	//PUMP 2
	if ((glycolRotary->GetState() == 2 || GlycolAutoTransferRelay) && glycolPump2CB->IsPowered())
	{
		glycolPump2->SetPumpOn();
	}
	else
	{
		glycolPump2->SetPumpOff();
	}

	//sprintf(oapiDebugString(), "DP %f DPSwitch %d ATRelay %d Pump1 %d Pump2 %d", DPSensor*PSI, PressureSwitch, GlycolAutoTransferRelay, glycolPump1->h_pump, glycolPump2->h_pump);
}

void LEMPrimGlycolPumpController::LoadState(char *line)
{
	int i, j, k;

	sscanf(line + 21, "%i %i %i", &i, &j, &k);

	PressureSwitch = (i != 0);
	GlycolAutoTransferRelay = (j != 0);
	GlycolPumpFailRelay = (j != 0);
}

void LEMPrimGlycolPumpController::SaveState(FILEHANDLE scn)
{
	char buffer[100];

	sprintf(buffer, "%d %d %d", PressureSwitch, GlycolAutoTransferRelay, GlycolPumpFailRelay);
	oapiWriteScenario_string(scn, "PRIMGLYPUMPCONTROLLER", buffer);
}

LEMSuitFanDPSensor::LEMSuitFanDPSensor()
{
	suitFanManifoldTank = NULL;
	suitCircuitHeatExchangerCoolingTank = NULL;
	suitFanDPCB = NULL;
	SuitFanFailRelay = false;
	PressureSwitch = false;
}

void LEMSuitFanDPSensor::Init(h_Tank *sfmt, h_Tank *schect, CircuitBrakerSwitch *sfdpcb)
{
	suitFanManifoldTank = sfmt;
	suitCircuitHeatExchangerCoolingTank = schect;
	suitFanDPCB = sfdpcb;
}

void LEMSuitFanDPSensor::SystemTimestep(double simdt)
{
	if (!suitFanManifoldTank || !suitCircuitHeatExchangerCoolingTank) return;

	double DPSensor = suitCircuitHeatExchangerCoolingTank->space.Press - suitFanManifoldTank->space.Press;

	if (PressureSwitch == false && DPSensor < 1.0 / PSI)
	{
		PressureSwitch = true;
	}
	else if (PressureSwitch == true && DPSensor > 1.33 / PSI)
	{
		PressureSwitch = false;
	}

	if (PressureSwitch && suitFanDPCB->IsPowered())
	{
		SuitFanFailRelay = true;
	}
	else
	{
		SuitFanFailRelay = false;
	}
}

void LEMSuitFanDPSensor::LoadState(char *line)
{
	int i, j;

	sscanf(line + 15, "%i %i", &i, &j);

	PressureSwitch = (i != 0);
	SuitFanFailRelay = (j != 0);
}

void LEMSuitFanDPSensor::SaveState(FILEHANDLE scn)
{
	char buffer[100];

	sprintf(buffer, "%d %d", PressureSwitch, SuitFanFailRelay);
	oapiWriteScenario_string(scn, "SUITFANDPSENSOR", buffer);
}

LEM_ECS::LEM_ECS(PanelSDK &p) : sdk(p)
{
	lem = NULL;
	// Initialize
	Asc_Oxygen1 = 0;
	Asc_Oxygen2 = 0;
	Des_Oxygen = 0;
	Des_OxygenPress = 0;
	Asc_Oxygen1Press = 0;
	Asc_Oxygen2Press = 0;
	//Des_Oxygen2 = 0; Using LM-8 Systems Handbook, only 1 DES O2 tank
	Asc_Water1 = 0;
	Asc_Water2 = 0;
	Des_Water = 0;
	//Des_Water2 = 0; Using LM-8 Systems Handbook, only 1 DES H2O tank
	Primary_CL_Glycol_Press = 0; // Zero this, system will fill from accu
	Secondary_CL_Glycol_Press = 0;  // Zero this, system will fill from accu
	Primary_CL_Glycol_Temp = 0;  // 40 in the accu, 0 other side of the pump
	Secondary_CL_Glycol_Temp = 0; // 40 in the accu, 0 other side of the pump
	Primary_Glycol_Accu = 0; // Cubic inches of coolant
	Secondary_Glycol_Accu = 0; // Cubic inches of coolant
	// Open valves as would be for IVT
	Des_O2 = 0;
	Des_H2O_To_PLSS = 0;
	Cabin_Repress = 0; // Auto
	// For simplicity's sake, we'll use a docked LM as it would be at IVT, at first docking the LM is empty!
	Cabin_Press = 0; Cabin_Temp = 0;
	Suit_Press = 0; Suit_Temp = 0;
	SuitCircuit_CO2 = 0; HX_CO2 = 0;
	Water_Sep1_Flow = 0; Water_Sep2_Flow = 0;
	Suit_Circuit_Relief = 0;
	Cabin_Gas_Return = 0;
	Asc_Water1Temp = 0; Asc_Water2Temp = 0;
}

void LEM_ECS::Init(LEM *s) {
	lem = s;
}
void LEM_ECS::TimeStep(double simdt) {
	if (lem == NULL) { return; }
	// **** Atmosphere Revitalization Section ****
	// First, get air from the suits and/or the cabin into the system.
	// Second, remove oxygen for and add CO2 from the crew.
	// Third, remove CO2 from the air and simulate the reaction in the LiOH can
	// Fourth, use the fans to move the resulting air through the suits and/or the cabin.
	// Fifth, use the heat exchanger to move heat from the air to the HTS if enabled (emergency ops)
	// Sixth, use the water separators to remove water from the air and add it to the WMS and surge tank.
	// Seventh, use the OSCPCS to add pressure if required
	// Eighth, use the regenerative heat exchanger to add heat to the air if required
	// Ninth and optionally, simulate the system behavior if a PGA failure is detected.
	// Tenth, simulate the LCG water movement operation.

	// **** Oxygen Supply and Cabin Pressure Control Section ****
	// Simple, move air from tanks to the cabin as required, and move air from the cabin to space as required.

	// **** Water Management Section ****
	// Also relatively simple, move water from tanks to the HTS / crew / etc as required.

	// **** Heat Transport Section ****
	// First, operate pumps to move glycol/water through the loops.
	// Second, move heat from the equipment to the glycol.
	// Third, move heat from the glycol to the sublimators.
	// Fourth, vent steam from the sublimators overboard.
}

void LEM_ECS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {

}

void LEM_ECS::LoadState(FILEHANDLE scn, char *end_str) {

}

double LEM_ECS::DescentOxyTankPressurePSI() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!Des_OxygenPress) {
		Des_OxygenPress = (double*)sdk.GetPointerByString("HYDRAULIC:DESO2TANK:PRESS");
	}
	return *Des_OxygenPress * PSI;
}

double LEM_ECS::AscentOxyTank1PressurePSI() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!Asc_Oxygen1Press) {
		Asc_Oxygen1Press = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:PRESS");
	}
	return *Asc_Oxygen1Press * PSI;
}

double LEM_ECS::AscentOxyTank2PressurePSI() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Asc_Oxygen2Press) {
		Asc_Oxygen2Press = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:PRESS");
	}
	return *Asc_Oxygen2Press * PSI;
}

double LEM_ECS::DescentOxyTankQuantityLBS() {
	if (!Des_Oxygen) {
		Des_Oxygen = (double*)sdk.GetPointerByString("HYDRAULIC:DESO2TANK:MASS");
	}
	return *Des_Oxygen * LBS;	
}

double LEM_ECS::AscentOxyTank1QuantityLBS() {
	if (!Asc_Oxygen1) {
		Asc_Oxygen1 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:MASS");
	}
	return *Asc_Oxygen1 * LBS;	
}

double LEM_ECS::AscentOxyTank2QuantityLBS() {
	if (!Asc_Oxygen2) {
		Asc_Oxygen2 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:MASS");
	}
	return *Asc_Oxygen2 * LBS;	
}

double LEM_ECS::GetCabinPressurePSI() {
	if (!Cabin_Press) {
		Cabin_Press = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	}
	return *Cabin_Press * PSI;
}

double LEM_ECS::GetSuitPressurePSI()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!Suit_Press) {
		Suit_Press = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:PRESS");
	}
	return *Suit_Press * PSI;
}

double LEM_ECS::GetSensorCO2MMHg() {
	
	if (!lem->ECS_CO2_SENSOR_CB.IsPowered()) return 0.0;
	
	if (!SuitCircuit_CO2) {
		SuitCircuit_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:CO2_PPRESS");
	}
	if (!HX_CO2) {
		HX_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:CO2_PPRESS");
	}
	return ((*SuitCircuit_CO2 + *HX_CO2) / 2.0) * MMHG;
}

double LEM_ECS::DescentWaterTankQuantity() {
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Des_Water) {
		Des_Water = (double*)sdk.GetPointerByString("HYDRAULIC:DESH2OTANK:MASS");
	}
	return (*Des_Water) / LM_DES_H2O_CAPACITY;
}

double LEM_ECS::AscentWaterTank1Quantity()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Asc_Water1) {
		Asc_Water1 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK1:MASS");
	}
	return (*Asc_Water1)/LM_ASC_H2O_CAPACITY;  
}

double LEM_ECS::AscentWaterTank2Quantity()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Asc_Water2) {
		Asc_Water2 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK2:MASS");
	}
	return (*Asc_Water2) / LM_ASC_H2O_CAPACITY;
}

double LEM_ECS::GetCabinTempF() {
	if (!Cabin_Temp) {
		Cabin_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	}
	return KelvinToFahrenheit(*Cabin_Temp);
}

double LEM_ECS::GetSuitTempF() {
	if (!Suit_Temp) {
		Suit_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:TEMP");
	}
	return KelvinToFahrenheit(*Suit_Temp);
}

double LEM_ECS::GetPrimaryGlycolPressure() {
	if (!Primary_CL_Glycol_Press) {
		Primary_CL_Glycol_Press = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:PRESS");
	}
	return *Primary_CL_Glycol_Press * PSI;
}

double LEM_ECS::GetPrimaryGlycolTempF() {
	if (!Primary_CL_Glycol_Temp) {
		Primary_CL_Glycol_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:TEMP");
	}
	return KelvinToFahrenheit(*Primary_CL_Glycol_Temp);
}

double LEM_ECS::GetSecondaryGlycolPressure() {
	if (!Secondary_CL_Glycol_Press) {
		Secondary_CL_Glycol_Press = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:PRESS");
	}
	return *Secondary_CL_Glycol_Press * PSI;
}

double LEM_ECS::GetSecondaryGlycolTempF() {
	if (!Secondary_CL_Glycol_Temp) {
		Secondary_CL_Glycol_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:TEMP");
	}
	return KelvinToFahrenheit(*Secondary_CL_Glycol_Temp);
}

double LEM_ECS::GetSelectedGlycolTempF()
{
	if (lem->GlycolRotary.GetState() == 0)
	{
		return GetSecondaryGlycolTempF();
	}

	return GetPrimaryGlycolTempF();
}

double LEM_ECS::GetWaterSeparatorRPM()
{
	if (!lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	if (!Water_Sep1_Flow) {
		Water_Sep1_Flow = (double*)sdk.GetPointerByString("HYDRAULIC:WATERSEP1:FLOW");
	}
	if (!Water_Sep2_Flow) {
		Water_Sep2_Flow = (double*)sdk.GetPointerByString("HYDRAULIC:WATERSEP2:FLOW");
	}

	if (*Water_Sep1_Flow > *Water_Sep2_Flow)
	{
		return (*Water_Sep1_Flow)*100.0;
	}
	
	return (*Water_Sep2_Flow)*100.0;
}

double LEM_ECS::GetAscWaterTank1TempF()
{
	if (!Asc_Water1Temp) {
		Asc_Water1Temp = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK1:TEMP");
	}
	return KelvinToFahrenheit(*Asc_Water1Temp);
}

double LEM_ECS::GetAscWaterTank2TempF()
{
	if (!Asc_Water2Temp) {
		Asc_Water2Temp = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK2:TEMP");
	}
	return KelvinToFahrenheit(*Asc_Water2Temp);
}

bool LEM_ECS::GetSuitFan1Failure()
{
	if (lem->SuitFanRotary.GetState() == 1)
	{
		if (lem->SuitFanDPSensor.GetSuitFanFail())
		{
			return true;
		}
	}

	return false;
}

bool LEM_ECS::GetSuitFan2Failure()
{
	if (lem->SuitFanRotary.GetState() == 2)
	{
		if (lem->SuitFanDPSensor.GetSuitFanFail())
		{
			return true;
		}
	}

	return false;
}

bool LEM_ECS::IsSuitCircuitReliefValveOpen()
{
	if (!Suit_Circuit_Relief) {
		Suit_Circuit_Relief = (int*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:OUT2:ISOPEN");
	}
	if (Suit_Circuit_Relief)
	{
		return (*Suit_Circuit_Relief);
	}

	return false;
}

bool LEM_ECS::IsCabinGasReturnValveOpen()
{
	if (!Cabin_Gas_Return) {
		Cabin_Gas_Return = (int*)sdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:LEAK:ISOPEN");
	}
	if (Cabin_Gas_Return)
	{
		return (*Cabin_Gas_Return);
	}

	return false;
}

bool LEM_ECS::GetGlycolPump2Failure()
{
	if (lem->GlycolRotary.GetState() != 0 && lem->PrimGlycolPumpController.GetPressureSwitch())
	{
		return true;
	}

	return false;
}