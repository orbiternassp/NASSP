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

LEMOVHDCabinReliefDumpValve::LEMOVHDCabinReliefDumpValve()
{
	cabinOVHDHatchValve = NULL;
	cabinOVHDHatchValveSwitch = NULL;
}

void LEMOVHDCabinReliefDumpValve::Init(h_Pipe *cohv, ThreePosSwitch *cohs)
{
	cabinOVHDHatchValve = cohv;
	cabinOVHDHatchValveSwitch = cohs;
}

void LEMOVHDCabinReliefDumpValve::SystemTimestep(double simdt)
{
	if (!cabinOVHDHatchValve) return;

	// Valve in motion
	if (cabinOVHDHatchValve->in->pz) return;

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
		cabinOVHDHatchValve->flowMax = 660.0 / LBH;

			double cabinpress = cabinOVHDHatchValve->out->parent->space.Press;
			if (cabinpress < 5.4 / PSI && cabinOVHDHatchValve->in->open == 0)
			{
				cabinOVHDHatchValve->in->Open();
			}
			else if (cabinpress >= 5.4 / PSI && cabinOVHDHatchValve->in->open == 1)
			{
				cabinOVHDHatchValve->in->Close();
			}
		else
		{
			cabinOVHDHatchValve->in->Close();
		}
	}
}


LEMFWDCabinReliefDumpValve::LEMFWDCabinReliefDumpValve()
{
	cabinFWDHatchValve = NULL;
	cabinFWDHatchValveSwitch = NULL;
}

void LEMFWDCabinReliefDumpValve::Init(h_Pipe *cfv, ThreePosSwitch *cfvs)
{
	cabinFWDHatchValve = cfv;
	cabinFWDHatchValveSwitch = cfvs;
}

void LEMFWDCabinReliefDumpValve::SystemTimestep(double simdt)
{
	if (!cabinFWDHatchValve) return;

	// Valve in motion
	if (cabinFWDHatchValve->in->pz) return;

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
		cabinFWDHatchValve->flowMax = 660.0 / LBH;

		double cabinpress = cabinFWDHatchValve->out->parent->space.Press;
		if (cabinpress < 5.4 / PSI && cabinFWDHatchValve->in->open == 0)
		{
			cabinFWDHatchValve->in->Open();
		}
		else if (cabinpress >= 5.4 / PSI && cabinFWDHatchValve->in->open == 1)
		{
			cabinFWDHatchValve->in->Close();
		}
		else
		{
			cabinFWDHatchValve->in->Close();
		}
	}
}


LEMCabinPressureRegulator::LEMCabinPressureRegulator()
{
	cabinRepressValve = NULL;
	cabinRepressValveSwitch = NULL;
	cabinRepressCB = NULL;
	pressRegulatorASwitch = NULL;
	pressRegulatorBSwitch = NULL;
	pressRegulatorAValve = NULL;
	pressRegulatorBValve = NULL;
	suitCircuit = NULL;
}

void LEMCabinPressureRegulator::Init(h_Pipe *crv, h_Pipe *prav, h_Pipe *prbv, h_Tank *sc, CircuitBrakerSwitch *crcb, RotationalSwitch *crvs, RotationalSwitch* pras, RotationalSwitch *prbs)
{
	cabinRepressValve = crv;
	cabinRepressValveSwitch = crvs;
	cabinRepressCB = crcb;
	pressRegulatorASwitch = pras;
	pressRegulatorBSwitch = prbs;
	pressRegulatorAValve = prav;
	pressRegulatorBValve = prbv;
	suitCircuit = sc;
}

void LEMCabinPressureRegulator::SystemTimestep(double simdt)
{
	if (!cabinRepressValve) return;
	if (!pressRegulatorAValve) return;
	if (!pressRegulatorBValve) return;

	// Valve in motion
	if (cabinRepressValve->in->pz) return;
	if (pressRegulatorAValve->in->pz) return;
	if (pressRegulatorBValve->in->pz) return;

	//CABIN REPRESS VALVE

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
			}
			else if(cabinpress > 4.7 / PSI && cabinRepressValve->in->open == 1)
			{
				cabinRepressValve->in->Close();
			}
		}
		else
		{
			cabinRepressValve->in->Close();
		}
	}

	//Suit pressure
	double suitpress = suitCircuit->space.Press;

	//PRESSURE REGULATOR A

	//DIRECT O2
	if (pressRegulatorASwitch->GetState() == 2)
	{
		pressRegulatorAValve->in->Open();
	}
	//CLOSE
	else if (pressRegulatorASwitch->GetState() == 3)
	{
		pressRegulatorAValve->in->Close();
	}
	//EGRESS
	else if (pressRegulatorASwitch->GetState() == 0)
	{
		if (suitpress < 3.8 / PSI)
		{
			pressRegulatorAValve->in->Open();
		}
		else
		{
			pressRegulatorAValve->in->Close();
		}
	}
	//CABIN
	else if (pressRegulatorASwitch->GetState() == 1)
	{
		if (suitpress < 4.8 / PSI)
		{
			pressRegulatorAValve->in->Open();
		}
		else
		{
			pressRegulatorAValve->in->Close();
		}
	}


	//PRESSURE REGULATOR B

	//DIRECT O2
	if (pressRegulatorBSwitch->GetState() == 2)
	{
		pressRegulatorBValve->in->Open();
	}
	//CLOSE
	else if (pressRegulatorBSwitch->GetState() == 3)
	{
		pressRegulatorBValve->in->Close();
	}
	//EGRESS
	else if (pressRegulatorBSwitch->GetState() == 0)
	{
		if (suitpress < 3.8 / PSI)
		{
			pressRegulatorBValve->in->Open();
		}
		else
		{
			pressRegulatorBValve->in->Close();
		}
	}
	//CABIN
	else if (pressRegulatorBSwitch->GetState() == 1)
	{
		if (suitpress < 4.8 / PSI)
		{
			pressRegulatorBValve->in->Open();
		}
		else
		{
			pressRegulatorBValve->in->Close();
		}
	}
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
	Primary_Glycol = 0;
	Secondary_Glycol = 0;
	// Open valves as would be for IVT
	Des_O2 = 0;
	Des_H2O_To_PLSS = 0;
	Cabin_Repress = 0; // Auto
					   // For simplicity's sake, we'll use a docked LM as it would be at IVT, at first docking the LM is empty!
	Cabin_Press = 0; Cabin_Temp = 0; Cabin_CO2 = 0;
	Suit_Press = 0; Suit_Temp = 0; Suit_CO2 = 0;

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

double LEM_ECS::DescentOxyTankPressure() {
	if (!Des_OxygenPress) {
		Des_OxygenPress = (double*)sdk.GetPointerByString("HYDRAULIC:DESO2TANK:PRESS");
	}
	return *Des_OxygenPress;
}

double LEM_ECS::DescentOxyTankPressurePSI() {
	return DescentOxyTankPressure() * PSI;
}

double LEM_ECS::AscentOxyTank1Pressure() {
	if (!Asc_Oxygen1Press) {
		Asc_Oxygen1Press = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:PRESS");
	}
	return *Asc_Oxygen1Press;
}

double LEM_ECS::AscentOxyTank1PressurePSI() {
	return AscentOxyTank1Pressure() * PSI;
}

double LEM_ECS::AscentOxyTank2Pressure() {
	if (!Asc_Oxygen2Press) {
		Asc_Oxygen2Press = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:PRESS");
	}
	return *Asc_Oxygen2Press;
}

double LEM_ECS::AscentOxyTank2PressurePSI() {
	return AscentOxyTank2Pressure() * PSI;
}

double LEM_ECS::DescentOxyTankQuantity() {
	if (!Des_Oxygen) {
		Des_Oxygen = (double*)sdk.GetPointerByString("HYDRAULIC:DESO2TANK:MASS");
	}
	return *Des_Oxygen;
}

double LEM_ECS::AscentOxyTank1Quantity() {
	if (!Asc_Oxygen1) {
		Asc_Oxygen1 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:MASS");
	}
	return *Asc_Oxygen1;
}

double LEM_ECS::AscentOxyTank2Quantity() {
	if (!Asc_Oxygen2) {
		Asc_Oxygen2 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:MASS");
	}
	return *Asc_Oxygen2;
}

double LEM_ECS::GetCabinPressure() {
	if (!Cabin_Press) {
		Cabin_Press = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	}
	return *Cabin_Press;
}

double LEM_ECS::GetCabinPressurePSI() {
	return GetCabinPressure() * PSI;
}

double LEM_ECS::GetCabinCO2MMHg() {
	if (!Cabin_CO2) {
		Cabin_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	}
	return *Cabin_CO2 * MMHG;
}

double LEM_ECS::GetSuitPressure() {
	if (!Suit_Press) {
		Suit_Press = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:PRESS");
	}
	return *Suit_Press;
}

double LEM_ECS::GetSuitPressurePSI() {
	return GetSuitPressure() * PSI;
}

double LEM_ECS::GetSuitCO2MMHg() {
	if (!Suit_CO2) {
		Suit_CO2 = (double*)sdk.GetPointerByString("HYDRAULIC:SUIT:CO2_PPRESS");
	}
	return *Suit_CO2 * MMHG;
}

double LEM_ECS::DescentWaterTankQuantityLBS() {
	if (!Des_Water) {
		Des_Water = (double*)sdk.GetPointerByString("HYDRAULIC:DESH2OTANK:MASS");
	}
	return *Des_Water * 0.0022046226218;  //grams to pounds
}

double LEM_ECS::AscentWaterTank1QuantityLBS() {
	if (!Asc_Water1) {
		Asc_Water1 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK1:MASS");
	}
	return *Asc_Water1 * 0.0022046226218;  //grams to pounds
}

double LEM_ECS::AscentWaterTank2QuantityLBS() {
	if (!Asc_Water2) {
		Asc_Water2 = (double*)sdk.GetPointerByString("HYDRAULIC:ASCH2OTANK2:MASS");
	}
	return *Asc_Water2 * 0.0022046226218;  //grams to pounds
}

double LEM_ECS::GetCabinTemperature() {
	if (!Cabin_Temp) {
		Cabin_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	}
	return *Cabin_Temp * 1.8 - 459.67;   //K to F
}

double LEM_ECS::GetSuitTemperature() {
	if (!Suit_Temp) {
		Suit_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:TEMP");
	}
	return *Suit_Temp * 1.8 - 459.67;   //K to F
}

double LEM_ECS::GetPrimaryGlycolPressure() {
	if (!Primary_CL_Glycol_Press) {
		Primary_CL_Glycol_Press = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP:PRESS");
	}
	return *Primary_CL_Glycol_Press;
}

double LEM_ECS::GetPrimaryGlycolTemperature() {
	if (!Primary_CL_Glycol_Temp) {
		Primary_CL_Glycol_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP:TEMP");
	}
	return *Primary_CL_Glycol_Temp;
}

double LEM_ECS::GetSecondaryGlycolPressure() {
	if (!Secondary_CL_Glycol_Press) {
		Secondary_CL_Glycol_Press = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP:PRESS");
	}
	return *Secondary_CL_Glycol_Press;
}

double LEM_ECS::GetSecondaryGlycolTemperature() {
	if (!Secondary_CL_Glycol_Temp) {
		Secondary_CL_Glycol_Temp = (double*)sdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP:TEMP");
	}
	return *Secondary_CL_Glycol_Temp;
}
