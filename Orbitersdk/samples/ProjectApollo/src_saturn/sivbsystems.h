/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IVB Systems (Header)

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

#include "DelayTimer.h"

#define PUVALVE_CLOSED 0
#define PUVALVE_NULL 1
#define PUVALVE_OPEN 2

//This has class has everything that needs to be copied from Saturn to S-IVB class
class BaseSIVBSystems
{
public:
	BaseSIVBSystems();
	virtual ~BaseSIVBSystems();

	void CopyData(BaseSIVBSystems *other);

	void SetVehicleNumber(int VehNo) { VehicleNo = VehNo; }
protected:
	//Forward Power Distributor (411A99) relays
	bool PUInverterAndDCPowerOn; //K1

	//Sequencer Assembly (404A3) relays
	bool LOXTankVentValveOpen; //K1
	bool LOXTankVentAndNPVValvesBoostCloseOn; //K2
	bool LH2TankVentValveOpen; //K3
	bool LH2TankVentAndLatchingReliefValveBoostCloseOn; //K4
	bool LH2TankContinuousVentOrificeSOVOpenOn; //K5
	bool LH2TankContinuousVentValveCloseOn; //K6
	bool LOXTankRepressControlValveOpenOn; //K7
	bool SecondBurnRelay; //K8
	bool FirstBurnRelay; //K9
	bool LH2TankRepressurizationControlValveOpenOn; //K10
	bool LOXTankFlightPressureSystemOn; //K11
	bool LH2TankLatchingReliefValveLatchOn; //K12
	bool LOXTankNPVValveOpenOn; //K13
	bool LOXTankNPVValveLatchOpenOn; //K14
	bool LOXHeatExchangerBypassValveControlEnable; //K15
	bool BurnerExcitersOn; //K16
	bool ChargeUllageIgnitionOn; //K21
	bool ChargeUllageJettisonOn; //K22
	bool FireUllageIgnitionOn; //K23
	bool FireUllageJettisonOn; //K24
	//K29 and K30 are non-latching relays involved in LOX tank pressurization
	//K31 is a non-latching relay indicating LH2 repress control switch enabled
	bool LOXTankPressurizationShutoffValvesClose; //K34
	bool RepressSystemModeSelectOn; //K35
	bool BurnerAutoCutoffSystemArm; //K36
	bool StartTankRechargeValveOpen; //K45
	bool PrevalvesCloseOn; //K46 (K198)
	bool EnginePumpPurgeControlValveEnableOn; //K47
	bool ChilldownShutoffValveCloseOn; //K48
	//K52 is a non-latching relay indicating repress system ambient mode
	bool LH2TankLatchingReliefValveOpenOn; //K53
	bool InflightCalibrateModeOn; //K54
	bool FuelInjTempOKBypass; //K55
	bool LH2TankContinuousVentReliefOverrideSOVOpenOn; //K56
	bool LVDCEngineStopRelay; //K61
	bool PointLevelSensorArmed; //K62
	bool EngineStart; //K63 (K57)
	bool TelemetryCalibrateOn; //K66
	//K71 and K73 are non-latching relays involved in LOX tank pressurization
	//K75 is a non-latching relay indicating LOX repress flight switch enabled
	bool BurnerLOXShutdownValveOpenOn; //K79
	bool BurnerLOXShutdownValveCloseOn; //K80
	bool BurnerLH2PropellantValveOpenOn; //K81
	bool BurnerLH2PropellantValveCloseOn; //K82
	//K87 is a non-latching relay indicating repress system ambient mode
	bool PassivationRelay; //K91
	bool EngineIgnitionPhaseControlValveOpenOn; //K92
	bool StartTankVentControlValveOpenOn; //K93
	bool EngineHeliumControlValveOpen; //K94
	bool EngineMainstageControlValveOpen; //K95
	bool AmbientHeliumSupplyShutoffValveClosedOn; //K96
	bool StartTankRechargeValveArm; //K97

	//Aft Power Distributor Assembly (404A45)
	bool LOXChilldownPumpOn; //K5
	bool FuelChillDownPumpOn; //K6

	//J-2 electronics
	bool EngineReady;
	//K11
	bool EngineReadyBypass;

	//K100 and K101
	bool ThrustOKRelay;
	//K105
	bool AftPowerDisableRelay;

	bool RSSEngineStop;
	bool EDSEngineStop;
	bool PropellantDepletionSensors;

	bool EnginePower;
	bool EngineStop;

	bool EDSCutoffDisabled; //K10 and K11
	bool EDSCutoffNo2DisableInhibit; //K23

	double ThrustTimer;
	double ThrustLevel;
	double BoiloffTime;
	double O2H2BurnerTime;
	double GH2_Mass;
	double GO2_Mass;

	//Valve states
	bool LH2CVSOrificeShutoffValve;
	bool LH2CVSReliefOverrideShutoffValve;
	bool LH2LatchingReliefValve;
	bool LH2LatchingReliefValveLatch;
	bool LH2FuelVentAndReliefValve;
	bool HeliumHeaterLH2PropellantValve;
	bool HeliumHeaterLOXShutdownValve;
	bool LOXVentAndReliefValve;
	bool LOXNPVVentAndReliefValve;
	bool LOXNPVValveLatch;

	//Pressure switches
	bool LH2PressureSwitch;
	bool LOXPressureSwitch;

	//Motor switches
	bool EngineControlBusMotorSwitch;

	//Signals
	bool O2H2BurnerOn;
	bool BurnerMalfunctionSignal;
	bool LH2AmbientRepressurizationOn, LH2CryoRepressurizationOn;
	bool LOXAmbientRepressurizationOn, LOXCryoRepressurizationOn;

	bool LH2ReliefValveOpen;
	bool APSUllageOnRelay[2];
	bool AuxHydPumpFlightMode;

	double J2DefaultThrust;
	int PUValveState;
	double MixtureRatio;
	bool propellantInitialized;
	double lastPropellantMass;
	double oxidMass;
	double fuelMass;

	double LH2TankUllagePressurePSI;
	double LOXTankUllagePressurePSI;

	int VehicleNo;

	//Particle stream levels
	double LH2_NPV_Stream_Lvl;
	double LH2_CVS_Stream_Lvl;
	double LOX_NPV_Stream_Lvl;
	double LOX_Dump_Stream_Lvl;

	//Logic
	bool CutoffSignalA, CutoffSignalX;
	bool J2SignalD, J2SignalE, J2SignalF, J2SignalH;
	bool HeliumControlOn;
	bool EMTEMP1, EngineReady1;
	bool StartTankDischargeControlOn;
	bool EngineStartLockUp, SparkSystemOn, EngineStart3, EngineStart4;
	bool SparksDeenergized;
	bool PBSignal1, StartTurbines, PBSignal4;
	bool MainstageSignal, MainstageOn, IgnitionPhaseControlOn;
	bool VCBSignal1, VCBSignal2, VCBSignal3;
	bool IgnitionDetector, CC1Signal1, IgnitionDetectionLockup;
	bool CC2Signal1, CC2Signal2, CC2Signal3, CutoffLockup;
	int EngineState; //0 = Off, 1 = Starting, 2 = Running, 3 = Stopping

	DelayTimer HeliumControlDeenergizedTimer;
	DelayTimer StartTankDischargeDelayTimer;
	DelayTimer IgnitionPhaseTimer;
	DelayTimer SparksDeenergizedTimer;

	bool EngineFailed;
	bool O2H2BurnerFailed;
	//double DebugTimer;
	double F_CVS;
};

class SIVBSystems : public BaseSIVBSystems
{
protected:
	SIVBSystems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THGROUP_HANDLE &ver, double PropLoadMR);
public:
	virtual ~SIVBSystems();

	void RecalculateEngineParameters(double BaseThrust);
	virtual void RecalculateEngineParameters() = 0;
	virtual void SetSIVBMixtureRatio(double ratio) = 0;
	virtual void SwitchSelector(int channel) = 0;
	void Timestep(double simdt);
	bool PropellantLowLevel();
	void SetPUValve(int state);

	void EDSEngineCutoff(bool cut) { EDSEngineStop = cut; }
	void SetEngineReadyBypass() { EngineReadyBypass = true; }
	void SetThrusterDir(double beta_y, double beta_p);
	void PointLevelSensorArming() { PointLevelSensorArmed = true; }
	void PointLevelSensorDisarming() { PointLevelSensorArmed = false; }
	void AuxHydPumpFlightModeOn() { AuxHydPumpFlightMode = true; }
	void AuxHydPumpFlightModeOff() { AuxHydPumpFlightMode = false; }
	void APSUllageEngineOn(int n);
	void APSUllageEngineOff(int n);
	void SetAPSAttitudeEngine(int n, bool on);
	void PrevalvesCloseOnReset() { PrevalvesCloseOn = false; }
	
	bool GetThrustOK() { return ThrustOKRelay; }

	void GetJ2ISP(double ratio, double &isp, double &ThrustAdjust);

	//From ESE
	bool ESECommandEngineControlBusPowerOn() { return false; } //TBD
	bool ESECommandEngineControlBusPowerOff() { return false; } //TBD
	//To ESE
	bool BurnerRelaysReset();
	bool RepressRelaysReset();

	//To IU
	double GetLH2TankUllagePressurePSI() { return LH2TankUllagePressurePSI; }
	double GetLOXTankUllagePressurePSI() { return LOXTankUllagePressurePSI; }
	bool GetBurnerMalfunction() { return BurnerMalfunctionSignal; }

	void SetEngineFailed();
	void SetO2H2BurnerFailed(bool fail);

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	void CreateParticleEffects(double TRANZ);
protected:

	void UpdatePropellants();
	bool EngineOnLogic();
	void UpdateLH2ValveStates();
	void UpdateLOXValveStates();
	void UpdateO2H2BurnerValveStates();
	void LH2PropellantCalculations(double simdt);
	void LOXPropellantCalculations(double simdt);
	void O2H2Burner(double simdt);
	void ChilldownSystem();
	bool BurnerLogic() const;
	void BurnerShutdown();

	VESSEL *vessel;
	THRUSTER_HANDLE &j2engine;
	THRUSTER_HANDLE *apsThrusters;
	THRUSTER_HANDLE *ullage;
	THGROUP_HANDLE &vernier;

	PROPELLANT_HANDLE &main_propellant;

	PSTREAM_HANDLE LH2_NPV_Stream1, LH2_NPV_Stream2;
	PSTREAM_HANDLE LH2_CVS_Stream1, LH2_CVS_Stream2;
	PSTREAM_HANDLE LOX_NPV_Stream1, LOX_NPV_Stream2;
	PSTREAM_HANDLE LOX_Dump_Stream;

	//Mixture ratio of propellant load
	const double PropellantLoadMixtureRatio;
};

class SIVB200Systems : public SIVBSystems
{
public:
	SIVB200Systems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THGROUP_HANDLE &ver);
	~SIVB200Systems() {};

	void RecalculateEngineParameters();
	void SwitchSelector(int channel);
	void SetSIVBMixtureRatio(double ratio);
};

class SIVB500Systems : public SIVBSystems
{
public:
	SIVB500Systems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THGROUP_HANDLE &ver);
	~SIVB500Systems() {};

	void RecalculateEngineParameters();
	void SwitchSelector(int channel);
	void SetSIVBMixtureRatio(double ratio);
};

#define SIVBSYSTEMS_START_STRING	"SIVBSYSTEMS_BEGIN"
#define SIVBSYSTEMS_END_STRING		"SIVBSYSTEMS_END"