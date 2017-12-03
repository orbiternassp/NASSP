/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Environmental Control System (Header)

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

#pragma once

class LEMOVHDCabinReliefDumpValve
{
public:
	LEMOVHDCabinReliefDumpValve();
	void Init(h_Pipe *cohv, ThreePosSwitch *cohs);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *cabinOVHDHatchValve;
	ThreePosSwitch *cabinOVHDHatchValveSwitch;
};

class LEMFWDCabinReliefDumpValve
{
public:
	LEMFWDCabinReliefDumpValve();
	void Init(h_Pipe *cfv, ThreePosSwitch *cfvs);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *cabinFWDHatchValve;
	ThreePosSwitch *cabinFWDHatchValveSwitch;
};

class LEMSuitCircuitReliefValve
{
public:
	LEMSuitCircuitReliefValve();
	void Init(h_Pipe *scrv, RotationalSwitch *scrvs);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *SuitCircuitReliefValve;
	RotationalSwitch *SuitCircuitReliefValveSwitch;
};

class LEMCabinRepressValve
{
public:
	LEMCabinRepressValve();
	void Init(h_Pipe *crv, CircuitBrakerSwitch *crcb, RotationalSwitch *crvs, RotationalSwitch* pras, RotationalSwitch *prbs);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *cabinRepressValve;
	CircuitBrakerSwitch *cabinRepressCB;
	RotationalSwitch *cabinRepressValveSwitch;
	RotationalSwitch *pressRegulatorASwitch;
	RotationalSwitch *pressRegulatorBSwitch;
};

class LEMSuitCircuitPressureRegulator
{
public:
	LEMSuitCircuitPressureRegulator();
	void Init(h_Pipe *prv, h_Tank *sc, RotationalSwitch *prs);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *pressRegulatorValve;
	h_Tank *suitCircuit;
	RotationalSwitch *pressRegulatorSwitch;
};

class LEM_ECS {
public:
	LEM_ECS(PanelSDK &p);
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	double AscentOxyTank1Pressure();
	double AscentOxyTank2Pressure();
	double DescentOxyTankPressure();
	double AscentOxyTank1PressurePSI();
	double AscentOxyTank2PressurePSI();
	double DescentOxyTankPressurePSI();
	double AscentOxyTank1Quantity();
	double AscentOxyTank2Quantity();
	double DescentOxyTankQuantity();
	double GetCabinPressure();
	double GetCabinPressurePSI();
	double GetCabinTemperature();
	double GetCabinCO2MMHg();
	double GetSuitPressure();
	double GetSuitPressurePSI();
	double GetSuitTemperature();
	double GetSuitCO2MMHg();
	double AscentWaterTank1QuantityLBS();
	double AscentWaterTank2QuantityLBS();
	double DescentWaterTankQuantityLBS();
	double GetPrimaryGlycolPressure();
	double GetSecondaryGlycolPressure();
	double GetPrimaryGlycolTemperature();
	double GetSecondaryGlycolTemperature();

	LEM *lem;													// Pointer at LEM
	double *Cabin_Press, *Cabin_Temp, *Cabin_CO2;					// Cabin Atmosphere
	double *Suit_Press, *Suit_Temp, *Suit_CO2;					// Suit Circuit Atmosphere
	double *Asc_Water1, *Asc_Water2, *Des_Water;					// Water tanks
	double *Asc_Oxygen1, *Asc_Oxygen2, *Des_Oxygen;				// Oxygen tanks
	double *Asc_Oxygen1Press, *Asc_Oxygen2Press, *Des_OxygenPress;  // Oxygen Tank Pressures
	double *Primary_CL_Glycol_Press;							// Pressure before and after pumps
	double *Secondary_CL_Glycol_Press;						// Pressure before and after pumps
	double *Primary_CL_Glycol_Temp;							// Teperature before and after pumps
	double *Secondary_CL_Glycol_Temp;						// Teperature before and after pumps
	double *Primary_Glycol_Accu;								// Glycol Accumulator
	double *Secondary_Glycol_Accu;								// Glycol Accumulator
	double *Primary_Glycol;										// Glycol in system
	double *Secondary_Glycol;									// Glycol in system
	int *Asc_H2O_To_PLSS, *Des_H2O_To_PLSS;						// PLSS Water Fill valves
	int *Water_Tank_Selector;									// WT selection valve
	int *Pri_Evap_Flow_1, *Pri_Evap_Flow_2;						// Primary evaporator flow valves
	int *Sec_Evap_Flow;											// Secondary evaporator flow valve
	int *Water_Sep_Selector;									// WS Select Valve
	int *Asc_O2_To_PLSS, *Des_O2_To_PLSS;						// PLSS Oxygen Fill Valves
	int *Des_O2;												// Descent O2 Valve
	int *Asc_O2Vlv1, *Asc_O2Vlv2;								// Ascent O2 Valves
	int *Cabin_Repress;											// Cabin Repress Valve
	int *CO2_Can_Select;										// CO2 Canister Selection
	int *Suit_Gas_Diverter;										// Suit Gas Diverter Valve
	int *Suit_Circuit_Relief;									// Suit Circuit Relief Valve
	int *Suit_IsolationCDR;										// CDR suit isolation valves
	int *Suit_IsolationLMP;										// LMP suit isolation valves

protected:
	PanelSDK &sdk;
};
