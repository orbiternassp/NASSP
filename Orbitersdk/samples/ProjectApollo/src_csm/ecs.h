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

#if !defined(_PA_ECS_H)
#define _PA_ECS_H

class Saturn;
class SaturnSideHatch;

///
/// This class simulates the cabin pressure regulator in the CSM.
/// \ingroup InternalSystems
/// \brief cabin pressure regulator.
///
class CabinPressureRegulator {

public:
	CabinPressureRegulator();
	virtual ~CabinPressureRegulator();

	void Init(h_Pipe *p, h_Pipe *crv, h_Pipe *ecpr, RotationalSwitch *crvs, RotationalSwitch *ecps, PushSwitch *ecpts);
	void SystemTimestep(double simdt);
	void Close();
	void Reset();
	void SetPressurePSI(double p);
	void SetMaxFlowLBH(double f);
	void ResetMaxFlow();
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

protected:
	h_Pipe *pipe;
	h_Pipe *cabinRepressValve;
	h_Pipe *emergencyCabinPressureRegulator;

	RotationalSwitch *cabinRepressValveSwitch;
	RotationalSwitch *emergencyCabinPressureSwitch;
	PushSwitch *emergencyCabinPressureTestSwitch;

	bool closed;
	double press;
};


///
/// This class simulates the oxygen demand regulator in the CSM.
/// \ingroup InternalSystems
/// \brief O2 demand regulator.
///
class O2DemandRegulator {

public:
	O2DemandRegulator();
	virtual ~O2DemandRegulator();

	void Init(h_Pipe *p, h_Pipe *s, h_Pipe *t, RotationalSwitch *o2ds, RotationalSwitch *sts);
	void SystemTimestep(double simdt);
	void Close();
	void Reset();
	void OpenSuitReliefValve();
	void ResetSuitReliefValve();
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

protected:
	h_Pipe *pipe;
	h_Pipe *suitReliefValve;
	h_Pipe *suitTestValve;
	RotationalSwitch *o2DemandSwitch;
	RotationalSwitch *suitTestSwitch;

	bool closed;
	bool suitReliefValveOpen;
};


///
/// This class simulates the cabin pressure relief valve and the post landing vent in the CSM.
/// \ingroup InternalSystems
/// \brief Cabin pressure relief valve.
///
class CabinPressureReliefValve {

public:
	CabinPressureReliefValve(Sound &plventsound);
	virtual ~CabinPressureReliefValve();

	void Init(h_Pipe *p, h_Pipe *i, Saturn *v, ThumbwheelSwitch *l, CircuitBrakerSwitch *plvlv, ThreePosSwitch *plv, e_object *plpower, SaturnSideHatch *sh);
	void SystemTimestep(double simdt);
	void SetLeakSize(double s);
	void SetReliefPressurePSI(double p);
	void LoadState(char *line);
	void SaveState(int index, FILEHANDLE scn);

protected:
	h_Pipe *pipe;
	h_Pipe *inlet;
	Saturn *saturn;
	ThumbwheelSwitch *lever;
	CircuitBrakerSwitch *postLandingValve;
	ThreePosSwitch *postLandingVent;
	e_object *postLandingPower;
	SaturnSideHatch *sideHatch;
	Sound &postLandingVentSound;

	double leakSize;
	double reliefPressure;
};

///
/// This class simulates the suit circuit return valve in the CSM.
/// \ingroup InternalSystems
/// \brief Suit circuit return valve.
///
class SuitCircuitReturnValve {

public:
	SuitCircuitReturnValve();
	virtual ~SuitCircuitReturnValve();

	void Init(h_Pipe *p, CircuitBrakerSwitch *l);
	void SystemTimestep(double simdt);
	bool IsOpen() { return (pipe->in->open != 0); };

protected:
	h_Pipe *pipe;
	CircuitBrakerSwitch *lever;
};

///
/// This class simulates the O2 SM supply valve, the surge tank and the repress package in the CSM.
/// \ingroup InternalSystems
/// \brief O2 SM supply.
///
class O2SMSupply {

public:
	O2SMSupply();
	virtual ~O2SMSupply();

	void Init(h_Tank *o2sm, h_Tank *o2mr, h_Tank *o2st, h_Tank *o2rp, h_Tank *o2rpo, h_Pipe *o2rpop,
		      RotationalSwitch *smv, RotationalSwitch *stv, RotationalSwitch *rpv,
			  CircuitBrakerSwitch *mra, CircuitBrakerSwitch *mrb, PanelSwitchItem *eo2v,
			  PanelSwitchItem *ro2v);
	void SystemTimestep(double simdt);
	void Close();
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);
	
protected:
	h_Tank *o2SMSupply;
	h_Tank *o2MainRegulator;
	h_Tank *o2SurgeTank;
	h_Tank *o2RepressPackage;
	h_Tank *o2RepressPackageOutlet;
	h_Pipe *o2RepressPackageOutletPipe;
	RotationalSwitch *smSupplyValve;
	RotationalSwitch *surgeTankValve;
	RotationalSwitch *repressPackageValve;
	CircuitBrakerSwitch *mainRegulatorASwitch;
	CircuitBrakerSwitch *mainRegulatorBSwitch;
	PanelSwitchItem *emergencyO2Valve;
	PanelSwitchItem *repressO2Valve;

	bool closed;
	bool o2SMSupplyVoid;
	bool o2MainRegulatorVoid;
	h_substance o2SMSupplyO2;
	h_substance o2MainRegulatorO2;
};

///
/// This class simulates the crew status (dead or alive) in the CSM.
/// \ingroup InternalSystems
/// \brief crew status.
///

#define ECS_CREWSTATUS_OK			0
#define ECS_CREWSTATUS_CRITICAL		1
#define ECS_CREWSTATUS_DEAD			2


class CrewStatus {

public:
	CrewStatus(Sound &crewdeadsound);
	virtual ~CrewStatus();
	void Init(Saturn *s);
	void Timestep(double simdt);
	int GetStatus() { return status; };
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

protected:
	int status;
	double suitPressureLowTime;
	double suitPressureHighTime;
	double suitTemperatureTime;
	double suitCO2Time;
	double accelerationTime;
	double lastVerticalVelocity;

	Saturn *saturn;
	Sound &crewDeadSound;
	bool firstTimestepDone;
};

///
/// This class simulates the side hatch in the CSM.
/// \ingroup InternalSystems
/// \brief Side hatch.
///
class SaturnSideHatch {

public:
	SaturnSideHatch(Sound &opensound, Sound &closesound);
	virtual ~SaturnSideHatch();

	void Init(Saturn *s, RotationalSwitch *gbs, RotationalSwitch *ahs, RotationalSwitch *ahso, RotationalSwitch *vvr);
	void Toggle(bool enforce = false);
	void Timestep(double simdt);
	bool IsOpen() { return open; };
	RotationalSwitch *GetVentValveRotary() { return ventValveRotary; };
	void SwitchToggled(PanelSwitchItem *s);
	void LoadState(char *line);
	void SaveState(FILEHANDLE scn);

protected:
	bool open;
	int toggle;

	Saturn *saturn;
	RotationalSwitch *gearBoxSelector;
	RotationalSwitch *actuatorHandleSelector;
	RotationalSwitch *actuatorHandleSelectorOpen;
	RotationalSwitch *ventValveRotary;

	Sound &OpenSound;
	Sound &CloseSound;
};

///
/// This class simulates the water plumbing in the CSM.
/// \ingroup InternalSystems
/// \brief Water Controller.
///
class SaturnWaterController {

public:
	SaturnWaterController();
	virtual ~SaturnWaterController();

	void Init(Saturn *s, h_Tank *pt, h_Tank *wt, h_Tank *pit, h_Tank *wit, 
		      h_Pipe *wvp, h_Pipe *wivp);
	void SystemTimestep(double simdt);
	double *GetWasteWaterDumpLevelRef() { return &wasteWaterDumpLevel; }
	double *GetUrineDumpLevelRef() { return &urineDumpLevel; }
	void FoodPreparationWaterSwitchToggled(PanelSwitchItem *s);

protected:
	double wasteWaterDumpLevel;
	double urineDumpLevel;

	Saturn *saturn;
	h_Tank *potableTank;
	h_Tank *wasteTank;
	h_Tank *potableInletTank;
	h_Tank *wasteInletTank;
	h_Pipe *wasteVentPipe;
	h_Pipe *wasteInletVentPipe;
};

///
/// This class simulates the gycol cooling loops in the CSM.
/// \ingroup InternalSystems
/// \brief Glycol Cooling Controller.
///
class SaturnGlycolCoolingController {

public:
	SaturnGlycolCoolingController();
	virtual ~SaturnGlycolCoolingController();

	void Init(Saturn *s);
	void SystemTimestep(double simdt);
	void GlycolEvapTempInSwitchToggled(PanelSwitchItem *s);
	void PrimaryGlycolEvapInletTempRotaryToggled(PanelSwitchItem *s);
	void PrimEvapSwitchesToggled(PanelSwitchItem *s);
	void SecEvapSwitchesToggled(PanelSwitchItem *s);
	void H2oAccumSwitchesToggled(PanelSwitchItem *s);
	void CabinTempSwitchToggled(PanelSwitchItem *s);

protected:
	Saturn *saturn;

	Boiler *suitHeater;
	Boiler *suitCircuitHeater;
	h_MixingPipe *evapInletMixer;
	h_Evaporator *primEvap;
	h_Evaporator *secEvap;
};

class SaturnLMTunnelVent
{
public:
	SaturnLMTunnelVent();
	void Init(h_Valve *tvv, h_Valve *tpv, RotationalSwitch *lmtvs);
	void SystemTimestep(double simdt);
protected:
	RotationalSwitch *LMTunnelVentSwitch;
	h_Valve *TunnelVentValve;
	h_Valve *TunnelPressValve;
};

class SaturnForwardHatch
{
public:
	SaturnForwardHatch();
	void Init(h_Valve *pev, RotationalSwitch *pes);
	void SystemTimestep(double simdt);
protected:
	RotationalSwitch *PressureEqualizationSwitch;
	h_Valve *PressureEqualizationValve;
};

#endif // _PA_ECS_H
