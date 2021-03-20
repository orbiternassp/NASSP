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

#include "animations.h"

  ///
  /// This class simulates the crew status (dead or alive) in the LM.
  /// \ingroup InternalSystems
  /// \brief crew status.
  ///

#define ECS_CREWSTATUS_OK			0
#define ECS_CREWSTATUS_CRITICAL		1
#define ECS_CREWSTATUS_DEAD			2


class LEMCrewStatus {

public:
	LEMCrewStatus(Sound &crewdeadsound);
	virtual ~LEMCrewStatus();
	void Init(LEM *s);
	void Timestep(double simdt);
	int GetStatus() { return status; };
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

protected:
	int status;
	double SuitPressureLowTime;
	double PressureLowTime;
	double SuitPressureHighTime;
	double PressureHighTime;
	double SuitTemperatureTime;
	double TemperatureTime;
	double CO2Time;
	double accelerationTime;
	double lastVerticalVelocity;

	LEM *lem;
	Sound &crewDeadSound;
	bool firstTimestepDone;
};

class LEMOverheadHatch
{
public:
	LEMOverheadHatch(Sound &opensound, Sound &closesound);
	void Init(LEM *l, ToggleSwitch *ohh);
	void DefineAnimations(UINT idx);
	void DefineAnimationsVC(UINT idx);
	void Timestep(double simdt);
	void Toggle();

	bool IsOpen() { return open; };

	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
protected:
	AnimState2 ovhdhatch_state;
	bool open;

	LEM *lem;
	ToggleSwitch *ovhdHatchHandle;

	Sound &OpenSound;
	Sound &CloseSound;

	UINT anim_OvhdHatch;
	UINT anim_OvhdHatchVC;
};

class LEMOVHDCabinReliefDumpValve
{
public:
	LEMOVHDCabinReliefDumpValve();
	void Init(h_Pipe *cohv, ThreePosSwitch *cohs, LEMOverheadHatch *oh);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *cabinOVHDHatchValve;
	ThreePosSwitch *cabinOVHDHatchValveSwitch;
	LEMOverheadHatch *ovhdHatch;
};

class LEMForwardHatch
{
public:
	LEMForwardHatch(Sound &opensound, Sound &closesound);
	void Init(LEM *l, ToggleSwitch *fhh);
	void DefineAnimations(UINT idx);
	void DefineAnimationsVC(UINT idx);
	void Timestep(double simdt);
	void Toggle();

	bool IsOpen() { return open; };

	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
protected:
	AnimState2 hatch_state;
	bool open;

	LEM *lem;
	ToggleSwitch *ForwardHatchHandle;

	Sound &OpenSound;
	Sound &CloseSound;

	UINT anim_Hatch;
	UINT anim_HatchVC;
};

class LEMFWDCabinReliefDumpValve
{
public:
	LEMFWDCabinReliefDumpValve();
	void Init(h_Pipe *cfv, ThreePosSwitch *cfvs, LEMForwardHatch *fh);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *cabinFWDHatchValve;
	ThreePosSwitch *cabinFWDHatchValveSwitch;
	LEMForwardHatch *fwdHatch;
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

class LEMPressureSwitch
{
public:
	LEMPressureSwitch();
	void Init(h_Tank *st, double max, double min);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn, char *name_str);
	void LoadState(char *line, int strlen);

	bool GetPressureSwitch() { return PressureSwitch; }
protected:
	h_Tank *switchtank;
	double maxpress;
	double minpress;

	bool PressureSwitch;
};

class LEMSuitIsolValve
{
public:
	LEMSuitIsolValve();
	void Init(LEM *l, RotationalSwitch *scv, ToggleSwitch *ovrd);
	void SystemTimestep(double simdt);

protected:
	LEM *lem;
	RotationalSwitch *suitisolvlv;
	ToggleSwitch *actuatorovrdswitch;

};

class LEMCabinRepressValve
{
public:
	LEMCabinRepressValve();
	void Init(LEM *l, h_Pipe *crv, CircuitBrakerSwitch *crcb, RotationalSwitch *crvs, RotationalSwitch* pras, RotationalSwitch *prbs);
	void SystemTimestep(double simdt);

	bool GetEmergencyCabinRepressRelay() { return EmergencyCabinRepressRelay; }
protected:
	LEM *lem;
	h_Pipe *cabinRepressValve;
	CircuitBrakerSwitch *cabinRepressCB;
	RotationalSwitch *cabinRepressValveSwitch;
	RotationalSwitch *pressRegulatorASwitch;
	RotationalSwitch *pressRegulatorBSwitch;

	//7K6
	bool EmergencyCabinRepressRelay;
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

class LEMSuitGasDiverter
{
public:
	LEMSuitGasDiverter();
	void Init(h_Tank *sgdv, h_Tank *cab, CircuitBrakerSwitch *sgds, CircuitBrakerSwitch *dvcb, RotationalSwitch* pras, RotationalSwitch *prbs);
	void SystemTimestep(double simdt);
protected:
	h_Tank *suitGasDiverterValve;
	h_Tank *cabin;
	CircuitBrakerSwitch *suitGasDiverterSwitch;
	CircuitBrakerSwitch *DivertVLVCB;
	RotationalSwitch *pressRegulatorASwitch;
	RotationalSwitch *pressRegulatorBSwitch;
};

class LEMCO2CanisterSelect
{
public:
	LEMCO2CanisterSelect();
	void Init(h_Tank *pco2, h_Tank *sco2, ToggleSwitch* co2s);
	void SystemTimestep(double simdt);
protected:
	h_Tank *PrimCO2Canister;
	h_Tank *SecCO2Canister;
	ToggleSwitch *CO2CanisterSelectSwitch;
};

class LEMCO2CanisterVent
{
public:
	LEMCO2CanisterVent();
	void Init(h_Tank *co2c, PushSwitch *co2vs);
	void SystemTimestep(double simdt);
protected:
	h_Tank *CO2Canister;
	PushSwitch *CO2CanisterVentSwitch;
};

class LEMCabinGasReturnValve
{
public:
	LEMCabinGasReturnValve();
	void Init(h_Pipe * cgrv, RotationalSwitch *cgrvs);
	void SystemTimestep(double simdt);
protected:
	h_Pipe *cabinGasReturnValve;
	RotationalSwitch *cabinGasReturnValveSwitch;
};

class LEMWaterSeparationSelector
{
public:
	LEMWaterSeparationSelector();
	void Init(h_Tank *wssv, CircuitBrakerSwitch* wsss);
	void SystemTimestep(double simdt);
protected:
	h_Tank *WaterSeparationSelectorValve;
	CircuitBrakerSwitch *WaterSeparationSelectorSwitch;
};

class LEMCabinFan
{
public:
	LEMCabinFan(FadeInOutSound &cabinfanS);
	void Init(CircuitBrakerSwitch *cf1cb, CircuitBrakerSwitch *cfccb, RotationalSwitch *pras, RotationalSwitch *prbs, Pump *cf, h_HeatLoad *cfh);
	void SystemTimestep(double simdt);
protected:

	void CabinFanSound();
	void StopCabinFanSound();

	CircuitBrakerSwitch *cabinFan1CB;
	CircuitBrakerSwitch *cabinFanContCB;
	RotationalSwitch *pressRegulatorASwitch;
	RotationalSwitch *pressRegulatorBSwitch;
	Pump *cabinFan;
	FadeInOutSound &cabinfansound;
	h_HeatLoad *cabinFanHeat;
};

class LEMWaterTankSelect
{
public:
	LEMWaterTankSelect();
	void Init(h_Tank *wts, h_Tank *st, RotationalSwitch *wtss);
	void SystemTimestep(double simdt);
protected:
	h_Tank *WaterTankSelect;
	h_Tank *SurgeTank;
	RotationalSwitch *WaterTankSelectSwitch;
};

class LEMPrimGlycolPumpController
{
public:
	LEMPrimGlycolPumpController();
	void Init(h_Tank *pgat, h_Tank *pgpmt, Pump *gp1, Pump *gp2, RotationalSwitch *gr, CircuitBrakerSwitch *gp1cb, CircuitBrakerSwitch *gp2cb, CircuitBrakerSwitch *gpatcb, h_HeatLoad *gp1h, h_HeatLoad *gp2h);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);

	bool GetPressureSwitch() { return PressureSwitch; }
	bool GetGlycolPumpFailRelay() { return GlycolPumpFailRelay; }
	bool GetGlycolPumpState(int i);
protected:
	h_Tank *primGlycolAccumulatorTank;
	h_Tank *primGlycolPumpManifoldTank;
	Pump *glycolPump1;
	Pump *glycolPump2;
	RotationalSwitch *glycolRotary;
	CircuitBrakerSwitch *glycolPump1CB;
	CircuitBrakerSwitch *glycolPump2CB;
	CircuitBrakerSwitch *glycolPumpAutoTransferCB;
	h_HeatLoad *glycolPump1Heat;
	h_HeatLoad *glycolPump2Heat;

	//7K8 (Latching)
	bool GlycolAutoTransferRelay;
	//7K9
	bool GlycolPumpFailRelay;

	bool PressureSwitch;
	int AutoTransferCounter;
};

class LEMSuitFanDPSensor
{
public:
	LEMSuitFanDPSensor();
	void Init(h_Tank *sfmt, h_Tank *schect, CircuitBrakerSwitch *sfdpcb);
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);

	bool GetSuitFanFail() { return SuitFanFailRelay; }
protected:
	h_Tank *suitFanManifoldTank;
	h_Tank *suitCircuitHeatExchangerCoolingTank;
	CircuitBrakerSwitch *suitFanDPCB;

	//K12
	bool SuitFanFailRelay;

	bool PressureSwitch;
};

class LEM_ECS {
public:
	LEM_ECS(PanelSDK &p);
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void Timestep(double simdt);
	double AscentOxyTank1PressurePSI();
	double AscentOxyTank2PressurePSI();
	double DescentOxyTankPressurePSI();
	double AscentOxyTank1QuantityLBS();
	double AscentOxyTank2QuantityLBS();
	double DescentOxyTankQuantityLBS();
	double GetCabinPressurePSI();
	double GetCabinTempF();
	double GetSuitPressurePSI();
	double GetSuitTempF();
	double GetSensorCO2MMHg();
	double AscentWaterTank1Quantity();
	double AscentWaterTank2Quantity();
	double DescentWaterTankQuantity();
	double GetPrimaryGlycolQuantity();
	double GetSecondaryGlycolQuantity();
	double GetPrimaryGlycolPressure();
	double GetSecondaryGlycolPressure();
	double GetSelectedGlycolPressure();
	double DescentWaterTankPressure();
	double GetPrimaryGlycolTempF();
	double GetSecondaryGlycolTempF();
	double GetSelectedGlycolTempF();
	double GetWaterSeparatorRPM();
	double GetAscWaterTank1TempF();
	double GetAscWaterTank2TempF();
	double GetPrimWBWaterInletTempF();
	double GetPrimWBGlycolInletTempF();
	double GetPrimWBGlycolOutletTempF();
	double GetPrimaryGlycolPumpDP();
	double GetPLSSFillPressurePSI();
	double GetECSSuitPSI();
	double GetECSCabinPSI();
	double GetECSSensorCO2MMHg();
	bool GetSuitFan1Failure();
	bool GetSuitFan2Failure();
	bool GetPrimGlycolLowLevel();
	bool GetSecGlycolLowLevel();
	bool IsSuitCircuitReliefValveOpen();
	bool IsCabinGasReturnValveOpen();
	bool GetGlycolPump2Failure();

	LEM *lem;													// Pointer at LEM
	double *Cabin_Press, *Cabin_Temp;					// Cabin Atmosphere
	double *Suit_Press, *Suit_Temp, *SuitCircuit_CO2, *HX_CO2;					// Suit Circuit Atmosphere
	double *Asc_Water1, *Asc_Water2, *Des_Water, *Des_Water_Press;	// Water tanks
	double *Asc_Water1Temp, *Asc_Water2Temp, *WB_Prim_Water_Temp;	// Water tank temperatures
	double *Asc_Oxygen1, *Asc_Oxygen2, *Des_Oxygen;				// Oxygen tanks
	double *Asc_Oxygen1Press, *Asc_Oxygen2Press, *Des_OxygenPress;  // Oxygen Tank Pressures
	double *Primary_CL_Glycol_Press;							// Pressure before and after pumps
	double *Secondary_CL_Glycol_Press;						// Pressure before and after pumps
	double *Primary_CL_Glycol_Temp;							// Temperature before and after pumps
	double *Secondary_CL_Glycol_Temp;						// Temperature before and after pumps
	double *Primary_Glycol_Accu;								// Glycol Accumulator mass
	double *Primary_Glycol_Pump_Manifold;						// Pump manifold mass
	double *Primary_Glycol_HXCooling;						// HXCooling mass
	double *Primary_Glycol_Loop1;							// Loop 1 mass
	double *Primary_Glycol_WaterHX;							// Water glycol HX mass
	double *Primary_Glycol_Loop2;							// Loop 2 mass
	double *Primary_Glycol_HXHeating;						// HXHeating mass
	double *Primary_Glycol_EvapIn;							// Evap inlet mass
	double *Primary_Glycol_EvapOut;							// Evap outlet mass
	double *Primary_Glycol_AscCooling;						// Ascent battery cooling mass
	double *Primary_Glycol_DesCooling;						// Descent battery cooling mass
	double *Secondary_Glycol_Accu;								// Glycol Accumulator mass
	double *Secondary_Glycol_Pump_Manifold;					// Pump manifold mass
	double *Secondary_Glycol_Loop1;							// Loop 1 mass
	double *Secondary_Glycol_AscCooling;					// Ascent battery cooling mass
	double *Secondary_Glycol_Loop2;								// Loop 2 mass
	double *Secondary_Glycol_EvapIn;							// Evap inlet mass
	double *Secondary_Glycol_EvapOut;							// Evap outlet mass
	double *Water_Sep1_RPM, *Water_Sep2_RPM;					// Water separators RPM
	double *WB_Prim_Gly_In_Temp, *WB_Prim_Gly_Out_Temp;			// Primary WB glycol temperatures
	double *Primary_Glycol_Accu_Press;							// Primary glycol accumulator pressure
	double *PLSS_O2_Fill_Press;									// PLSS O2 fill pressure
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
	int *Cabin_Gas_Return;										// Cabin gas return valve

protected:
	PanelSDK &sdk;
};
