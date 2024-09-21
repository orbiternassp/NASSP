/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IVB Systems

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

#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "sivbsystems.h"

static PARTICLESTREAMSPEC lh2_npv_venting_spec = {
	0,		// flag
	0.003,	// size
	20.0,	// rate
	3.0,	// velocity
	0.001,  // velocity distribution
	0.5,	// lifetime
	3.0,	// growthrate
	0.5,    // atmslowdown
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

static PARTICLESTREAMSPEC lh2_cvs_venting_spec = {
	0,		// flag
	0.005,	// size
	20.0,	// rate
	3.0,	// velocity
	0.01,   // velocity distribution
	0.5,	// lifetime
	1.0,	// growthrate
	0.5,    // atmslowdown
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

static PARTICLESTREAMSPEC lox_npv_venting_spec = {
	0,		// flag
	0.003,	// size
	20.0,	// rate
	3.0,	// velocity
	0.001,  // velocity distribution
	0.5,	// lifetime
	3.0,	// growthrate
	0.5,    // atmslowdown
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

static PARTICLESTREAMSPEC lox_dump_venting_spec = {
	0,		// flag
	0.8,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	20,		// lifetime
	0.15,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.6, 0.6,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

//Constants
const double BTU = 1055.06;										// BTU to Jouls
const double R = 8314.4621;										// (L*Pa) / (mol*K)

//LH2
const double LH2_TANK_VOLUME = 294.27*1000.0;					// Liters
const double M_H2 = 2.01588;									// g/mol
const double LH2_ENTHALPY_VAPORIZATION = 0.44936*1000.0 / M_H2; // J/g
const double GH2_TEMP = 83.3;									// K, value is 150°R
const double CVS_VALVE_SIZE = 0.009;
const double CVS_BYPASS_VALVE_SIZE = 0.002;
const double LH2_NPV_VALVE_SIZE = 0.0226; //TBD: 0.09 works best for post TLI, might be a temperature issue
const double LH2_CVS_ISP = 750.0;
const double LH2_AMBIENT_HELIUM_MASS_FLOW = 0.1; //kg/s, simulating LH2 pressure increase caused by ambient temperature helium. TBD: Value
const double LH2_CRYO_HELIUM_MASS_FLOW = 0.2; //kg/s, simulating LH2 pressure increase caused by helium heater. TBD: Value

//LOX
const double LO2_TANK_VOLUME = 80.137*1000.0;					// Liters
const double M_O2 = 31.9988;									// g/mol
const double GO2_TEMP = 100.0;									// K
const double LOX_NPV_VALVE_SIZE = 0.13;// 0.009;
const double LOX_VENT_VALVE_SIZE = 0.5;
const double LOX_AMBIENT_HELIUM_MASS_FLOW = 0.1; //kg/s, simulating LOX pressure increase caused by ambient temperature helium. TBD: Value
const double LOX_CRYO_HELIUM_MASS_FLOW = 0.2; //kg/s, simulating LOX pressure increase caused by helium heater. TBD: Value
const double LOX_DUMP_ISP = 148.12; // m/s, value from Apollo 8 flight evaluation report
const double GOX_DUMP_ISP = 238.0; // m/s
const double GOX_VENT_ISP = 86.0; // m/s, seems low but gives the right DV. Vent direction might be through the CG and not 100% forwards

BaseSIVBSystems::BaseSIVBSystems() :
	HeliumControlDeenergizedTimer(1.0),
	StartTankDischargeDelayTimer(1.0),
	IgnitionPhaseTimer(0.45),
	SparksDeenergizedTimer(3.3)
{
	VehicleNo = 600;

	PUInverterAndDCPowerOn = false;

	//PROPULSION
	LOXTankVentValveOpen = false;
	LOXTankVentAndNPVValvesBoostCloseOn = false;
	LH2TankVentValveOpen = false;
	LH2TankVentAndLatchingReliefValveBoostCloseOn = false;
	LH2TankContinuousVentOrificeSOVOpenOn = false;
	LH2TankContinuousVentValveCloseOn = false;
	LOXTankRepressControlValveOpenOn = false;
	SecondBurnRelay = false;
	FirstBurnRelay = false;
	LH2TankRepressurizationControlValveOpenOn = false;
	LOXTankFlightPressureSystemOn = false;
	LH2TankLatchingReliefValveLatchOn = false;
	LOXTankNPVValveOpenOn = false;
	LOXTankNPVValveLatchOpenOn = false;
	LOXHeatExchangerBypassValveControlEnable = false;
	BurnerExcitersOn = false;

	ChargeUllageIgnitionOn = false;
	ChargeUllageJettisonOn = false;
	FireUllageIgnitionOn = false;
	FireUllageJettisonOn = false;

	LOXTankPressurizationShutoffValvesClose = false;
	RepressSystemModeSelectOn = false;
	BurnerAutoCutoffSystemArm = false;
	StartTankRechargeValveOpen = false;
	PrevalvesCloseOn = false;
	EnginePumpPurgeControlValveEnableOn = false;
	ChilldownShutoffValveCloseOn = false;
	LH2TankLatchingReliefValveOpenOn = false;
	InflightCalibrateModeOn = false;
	FuelInjTempOKBypass = false;
	LH2TankContinuousVentReliefOverrideSOVOpenOn = false;
	BurnerLOXShutdownValveOpenOn = false;
	BurnerLOXShutdownValveCloseOn = false;
	BurnerLH2PropellantValveOpenOn = false;
	BurnerLH2PropellantValveCloseOn = false;
	PassivationRelay = false;
	EngineIgnitionPhaseControlValveOpenOn = false;
	StartTankVentControlValveOpenOn = false;
	EngineHeliumControlValveOpen = false;
	EngineMainstageControlValveOpen = false;
	AmbientHeliumSupplyShutoffValveClosedOn = false;
	StartTankRechargeValveArm = false;

	LH2CVSOrificeShutoffValve = false;
	LH2CVSReliefOverrideShutoffValve = false;
	LH2LatchingReliefValve = false;
	LH2LatchingReliefValveLatch = false;
	LH2FuelVentAndReliefValve = false;
	HeliumHeaterLH2PropellantValve = false;
	HeliumHeaterLOXShutdownValve = false;
	LOXVentAndReliefValve = false;
	LOXNPVVentAndReliefValve = false;
	LOXNPVValveLatch = false;

	LH2PressureSwitch = false;
	LOXPressureSwitch = false;

	O2H2BurnerOn = false;
	BurnerMalfunctionSignal = false;
	LH2AmbientRepressurizationOn = LH2CryoRepressurizationOn = false;
	LOXAmbientRepressurizationOn = LOXCryoRepressurizationOn = false;

	LOXChilldownPumpOn = false;
	FuelChillDownPumpOn = false;

	//ENGINE
	EngineStart = false;
	LVDCEngineStopRelay = false;
	EDSEngineStop = false;
	EngineReady = false;
	EngineReadyBypass = false;
	EnginePower = false;
	PropellantDepletionSensors = false;
	RSSEngineStop = false;
	ThrustOKRelay = false;
	PointLevelSensorArmed = false;
	LH2ReliefValveOpen = false;
	//Gets switched on at about T-8 minutes
	AuxHydPumpFlightMode = true;
	//Switched on at T-TBD minutes
	EngineControlBusMotorSwitch = true;
	EDSCutoffNo2DisableInhibit = false;

	for (int i = 0; i < 2; i++)
	{
		APSUllageOnRelay[i] = false;
	}

	PUValveState = PUVALVE_NULL;

	J2DefaultThrust = 0.0;
	ThrustTimer = 0.0;
	ThrustLevel = 0.0;
	BoiloffTime = 0.0;
	O2H2BurnerTime = 0.0;
	GH2_Mass = -1;
	GO2_Mass = -1;
	LH2TankUllagePressurePSI = 31.0;
	LOXTankUllagePressurePSI = 41.0;

	CutoffSignalA = CutoffSignalX = false;
	HeliumControlOn = false;
	EMTEMP1 = EngineReady1 = false;
	StartTankDischargeControlOn = false;
	EngineStartLockUp = SparkSystemOn = EngineStart3 = EngineStart4 = false;
	SparksDeenergized = false;
	PBSignal1 = StartTurbines = PBSignal4 = false;
	MainstageSignal = MainstageOn = IgnitionPhaseControlOn = false;
	VCBSignal1 = VCBSignal2 = VCBSignal3 = false;
	IgnitionDetector = CC1Signal1 = IgnitionDetectionLockup = false;
	CC2Signal1 = CC2Signal2 = CC2Signal3 = CutoffLockup = false;
	EngineState = 0;
	EngineFailed = false;
	O2H2BurnerFailed = false;

	LH2_NPV_Stream_Lvl = 0.0;
	LH2_CVS_Stream_Lvl = 0.0;
	LOX_NPV_Stream_Lvl = 0.0;
	LOX_Dump_Stream_Lvl = 0.0;

	propellantInitialized = false;
	lastPropellantMass = 0;
	MixtureRatio = 5.0;
	oxidMass = -1;
	fuelMass = 0.0;

	//DebugTimer = 0.0;
	F_CVS = 0.0;
}

BaseSIVBSystems::~BaseSIVBSystems()
{

}

void BaseSIVBSystems::CopyData(BaseSIVBSystems *other)
{
	*other = *this;
}

SIVBSystems::SIVBSystems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THGROUP_HANDLE &ver, double PropLoadMR) :
	j2engine(j2), vernier(ver), main_propellant(j2prop), apsThrusters(aps), ullage(ull),PropellantLoadMixtureRatio(PropLoadMR)
{

	vessel = v;	

	LH2_NPV_Stream1 = NULL;
	LH2_NPV_Stream2 = NULL;
	LOX_NPV_Stream1 = NULL;
	LOX_NPV_Stream2 = NULL;
	LOX_Dump_Stream = NULL;
	LH2_CVS_Stream1 = NULL;
	LH2_CVS_Stream2 = NULL;
}

SIVBSystems::~SIVBSystems()
{
	if (LH2_NPV_Stream1)
	{
		vessel->DelExhaustStream(LH2_NPV_Stream1);
		LH2_NPV_Stream1 = NULL;
	}
	if (LH2_NPV_Stream2)
	{
		vessel->DelExhaustStream(LH2_NPV_Stream2);
		LH2_NPV_Stream2 = NULL;
	}
	if (LH2_CVS_Stream1)
	{
		vessel->DelExhaustStream(LH2_CVS_Stream1);
		LH2_CVS_Stream1 = NULL;
	}
	if (LH2_CVS_Stream2)
	{
		vessel->DelExhaustStream(LH2_CVS_Stream2);
		LH2_CVS_Stream2 = NULL;
	}
	if (LOX_NPV_Stream1)
	{
		vessel->DelExhaustStream(LOX_NPV_Stream1);
		LOX_NPV_Stream1 = NULL;
	}
	if (LOX_NPV_Stream2)
	{
		vessel->DelExhaustStream(LOX_NPV_Stream2);
		LOX_NPV_Stream2 = NULL;
	}
	if (LOX_Dump_Stream)
	{
		vessel->DelExhaustStream(LOX_Dump_Stream);
		LOX_Dump_Stream = NULL;
	}
}

void SIVBSystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SIVBSYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
	papiWriteScenario_bool(scn, "EDSENGINESTOP", EDSEngineStop);
	papiWriteScenario_bool(scn, "RSSENGINESTOP", RSSEngineStop);
	papiWriteScenario_bool(scn, "HeliumControlOn", HeliumControlOn);
	papiWriteScenario_bool(scn, "StartTankDischargeControlOn", StartTankDischargeControlOn);
	papiWriteScenario_bool(scn, "EngineStartLockUp", EngineStartLockUp);
	papiWriteScenario_bool(scn, "SparkSystemOn", SparkSystemOn);
	papiWriteScenario_bool(scn, "SparksDeenergized", SparksDeenergized);
	papiWriteScenario_bool(scn, "StartTurbines", StartTurbines);
	papiWriteScenario_bool(scn, "MainstageSignal", MainstageSignal);
	papiWriteScenario_bool(scn, "MainstageOn", MainstageOn);
	papiWriteScenario_bool(scn, "IgnitionPhaseControlOn", IgnitionPhaseControlOn);
	papiWriteScenario_bool(scn, "IgnitionDetectionLockup", IgnitionDetectionLockup);
	papiWriteScenario_bool(scn, "CutoffLockup", CutoffLockup);
	papiWriteScenario_boolarr(scn, "APSULLAGEONRELAY", APSUllageOnRelay, 2);
	oapiWriteScenario_int(scn, "PUVALVESTATE", PUValveState);
	oapiWriteScenario_int(scn, "EngineState", EngineState);
	papiWriteScenario_double(scn, "BOILOFFTIME", BoiloffTime);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	papiWriteScenario_double(scn, "THRUSTLEVEL", ThrustLevel);
	papiWriteScenario_double(scn, "J2DEFAULTTHRUST", J2DefaultThrust);
	papiWriteScenario_double(scn, "OXIDMASS", oxidMass);
	papiWriteScenario_double(scn, "GH2_MASS", GH2_Mass);
	papiWriteScenario_double(scn, "GO2_MASS", GO2_Mass);
	papiWriteScenario_bool(scn, "O2H2BURNERON", O2H2BurnerOn);
	papiWriteScenario_double(scn, "O2H2BURNERTIME", O2H2BurnerTime);

	bool arr1[50] = { LOXTankVentValveOpen, LOXTankVentAndNPVValvesBoostCloseOn, LH2TankVentValveOpen, LH2TankVentAndLatchingReliefValveBoostCloseOn,LH2TankContinuousVentOrificeSOVOpenOn,
		LH2TankContinuousVentValveCloseOn, LOXTankRepressControlValveOpenOn, SecondBurnRelay, FirstBurnRelay, LH2TankRepressurizationControlValveOpenOn, LOXTankFlightPressureSystemOn,
		LH2TankLatchingReliefValveLatchOn, LOXTankNPVValveOpenOn, LOXTankNPVValveLatchOpenOn, LOXHeatExchangerBypassValveControlEnable, BurnerExcitersOn, false, false, false, false,
		ChargeUllageIgnitionOn, ChargeUllageJettisonOn, FireUllageIgnitionOn, FireUllageJettisonOn, false, false, false, false, false, false, false, false, false, LOXTankPressurizationShutoffValvesClose,
		RepressSystemModeSelectOn, BurnerAutoCutoffSystemArm, false, false, false, false, false, false, false, false, StartTankRechargeValveOpen, PrevalvesCloseOn, EnginePumpPurgeControlValveEnableOn,
		ChilldownShutoffValveCloseOn, false, false };
	papiWriteScenario_boolarr(scn, "SEQUENCERASSEMBLYRELAYS1", arr1, 50);
	
	bool arr2[47] = { false, false, LH2TankLatchingReliefValveOpenOn, InflightCalibrateModeOn, FuelInjTempOKBypass, LH2TankContinuousVentReliefOverrideSOVOpenOn, false, false, false, false,
		LVDCEngineStopRelay, PointLevelSensorArmed, EngineStart, false, false, TelemetryCalibrateOn, false, false, false, false, false, false, false, false, false, false, false, false, 
		BurnerLOXShutdownValveOpenOn, BurnerLOXShutdownValveCloseOn, BurnerLH2PropellantValveOpenOn, BurnerLH2PropellantValveCloseOn, false, false, false, false, false, false, false, false,
		PassivationRelay, EngineIgnitionPhaseControlValveOpenOn, StartTankVentControlValveOpenOn, EngineHeliumControlValveOpen, EngineMainstageControlValveOpen, AmbientHeliumSupplyShutoffValveClosedOn, 
		StartTankRechargeValveArm };
	papiWriteScenario_boolarr(scn, "SEQUENCERASSEMBLYRELAYS2", arr2, 47);

	bool arr3[10] = { LH2CVSOrificeShutoffValve, LH2CVSReliefOverrideShutoffValve, LH2LatchingReliefValve, LH2LatchingReliefValveLatch, LH2FuelVentAndReliefValve, HeliumHeaterLH2PropellantValve,
		HeliumHeaterLOXShutdownValve, LOXVentAndReliefValve, LOXNPVVentAndReliefValve, LOXNPVValveLatch };
	papiWriteScenario_boolarr(scn, "VALVESTATES", arr3, 10);

	bool arr4[3] = { LH2PressureSwitch, LOXPressureSwitch, EngineControlBusMotorSwitch };
	papiWriteScenario_boolarr(scn, "PRESSURESWITCHES", arr4, 3);

	HeliumControlDeenergizedTimer.SaveState(scn, "HeliumControlDeenergizedTimer_BEGIN", "TD_END");
	StartTankDischargeDelayTimer.SaveState(scn, "StartTankDischargeDelayTimer_BEGIN", "TD_END");
	IgnitionPhaseTimer.SaveState(scn, "IgnitionPhaseTimer_BEGIN", "TD_END");
	SparksDeenergizedTimer.SaveState(scn, "SparksDeenergizedTimer_BEGIN", "TD_END");

	oapiWriteLine(scn, SIVBSYSTEMS_END_STRING);
}

void SIVBSystems::LoadState(FILEHANDLE scn) {
	char *line;
	bool arr[64];

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SIVBSYSTEMS_END_STRING, sizeof(SIVBSYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
		papiReadScenario_bool(line, "EDSENGINESTOP", EDSEngineStop);
		papiReadScenario_bool(line, "RSSENGINESTOP", RSSEngineStop);
		papiReadScenario_bool(line, "HeliumControlOn", HeliumControlOn);
		papiReadScenario_bool(line, "StartTankDischargeControlOn", StartTankDischargeControlOn);
		papiReadScenario_bool(line, "EngineStartLockUp", EngineStartLockUp);
		papiReadScenario_bool(line, "SparkSystemOn", SparkSystemOn);
		papiReadScenario_bool(line, "SparksDeenergized", SparksDeenergized);
		papiReadScenario_bool(line, "StartTurbines", StartTurbines);
		papiReadScenario_bool(line, "MainstageSignal", MainstageSignal);
		papiReadScenario_bool(line, "MainstageOn", MainstageOn);
		papiReadScenario_bool(line, "IgnitionPhaseControlOn", IgnitionPhaseControlOn);
		papiReadScenario_bool(line, "IgnitionDetectionLockup", IgnitionDetectionLockup);
		papiReadScenario_bool(line, "CutoffLockup", CutoffLockup);
		papiReadScenario_boolarr(line, "APSULLAGEONRELAY", APSUllageOnRelay, 2);
		papiReadScenario_int(line, "PUVALVESTATE", PUValveState);
		papiReadScenario_int(line, "EngineState", EngineState);
		papiReadScenario_double(line, "BOILOFFTIME", BoiloffTime);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
		papiReadScenario_double(line, "THRUSTLEVEL", ThrustLevel);
		papiReadScenario_double(line, "J2DEFAULTTHRUST", J2DefaultThrust);
		papiReadScenario_double(line, "OXIDMASS", oxidMass);
		papiReadScenario_double(line, "GH2_MASS", GH2_Mass);
		papiReadScenario_double(line, "GO2_MASS", GO2_Mass);
		papiReadScenario_bool(line, "O2H2BURNERON", O2H2BurnerOn);
		papiReadScenario_double(line, "O2H2BURNERTIME", O2H2BurnerTime);

		if (papiReadScenario_boolarr(line, "SEQUENCERASSEMBLYRELAYS1", arr, 50))
		{
			LOXTankVentValveOpen = arr[0];
			LOXTankVentAndNPVValvesBoostCloseOn = arr[1];
			LH2TankVentValveOpen = arr[2];
			LH2TankVentAndLatchingReliefValveBoostCloseOn = arr[3];
			LH2TankContinuousVentOrificeSOVOpenOn = arr[4];
			LH2TankContinuousVentValveCloseOn = arr[5];
			LOXTankRepressControlValveOpenOn = arr[6];
			SecondBurnRelay = arr[7];
			FirstBurnRelay = arr[8];
			LH2TankRepressurizationControlValveOpenOn = arr[9];
			LOXTankFlightPressureSystemOn = arr[10];
			LH2TankLatchingReliefValveLatchOn = arr[11];
			LOXTankNPVValveOpenOn = arr[12];
			LOXTankNPVValveLatchOpenOn = arr[13];
			LOXHeatExchangerBypassValveControlEnable = arr[14];
			BurnerExcitersOn = arr[15];
			ChargeUllageIgnitionOn = arr[20];
			ChargeUllageJettisonOn = arr[21];
			FireUllageIgnitionOn = arr[22];
			FireUllageJettisonOn = arr[23];
			LOXTankPressurizationShutoffValvesClose = arr[33];
			RepressSystemModeSelectOn = arr[34];
			BurnerAutoCutoffSystemArm = arr[35];
			StartTankRechargeValveOpen = arr[44];
			PrevalvesCloseOn = arr[45];
			EnginePumpPurgeControlValveEnableOn = arr[46];
			ChilldownShutoffValveCloseOn = arr[47];
		}
		if (papiReadScenario_boolarr(line, "SEQUENCERASSEMBLYRELAYS2", arr, 47))
		{
			LH2TankLatchingReliefValveOpenOn = arr[2];
			InflightCalibrateModeOn = arr[3];
			FuelInjTempOKBypass = arr[4];
			LH2TankContinuousVentReliefOverrideSOVOpenOn = arr[5];
			LVDCEngineStopRelay = arr[10];
			PointLevelSensorArmed = arr[11];
			EngineStart = arr[12];
			TelemetryCalibrateOn = arr[15];
			BurnerLOXShutdownValveOpenOn = arr[28];
			BurnerLOXShutdownValveCloseOn = arr[29];
			BurnerLH2PropellantValveOpenOn = arr[30];
			BurnerLH2PropellantValveCloseOn = arr[31];
			PassivationRelay = arr[40];
			EngineIgnitionPhaseControlValveOpenOn = arr[41];
			StartTankVentControlValveOpenOn = arr[42];
			EngineHeliumControlValveOpen = arr[43];
			EngineMainstageControlValveOpen = arr[44];
			AmbientHeliumSupplyShutoffValveClosedOn = arr[45];
			StartTankRechargeValveArm = arr[46];
		}
		else if (papiReadScenario_boolarr(line, "VALVESTATES", arr, 10))
		{
			LH2CVSOrificeShutoffValve = arr[0];
			LH2CVSReliefOverrideShutoffValve = arr[1];
			LH2LatchingReliefValve = arr[2];
			LH2LatchingReliefValveLatch = arr[3];
			LH2FuelVentAndReliefValve = arr[4];
			HeliumHeaterLH2PropellantValve = arr[5];
			HeliumHeaterLOXShutdownValve = arr[6];
			LOXVentAndReliefValve = arr[7];
			LOXNPVVentAndReliefValve = arr[8];
			LOXNPVValveLatch = arr[9];
		}
		else if (papiReadScenario_boolarr(line, "PRESSURESWITCHES", arr, 3))
		{
			LH2PressureSwitch = arr[0];
			LOXPressureSwitch = arr[1];
			EngineControlBusMotorSwitch = arr[2];
		}
		else if (!strnicmp(line, "HeliumControlDeenergizedTimer_BEGIN", sizeof("HeliumControlDeenergizedTimer_BEGIN"))) {
			HeliumControlDeenergizedTimer.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "StartTankDischargeDelayTimer_BEGIN", sizeof("StartTankDischargeDelayTimer_BEGIN"))) {
			StartTankDischargeDelayTimer.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "IgnitionPhaseTimer_BEGIN", sizeof("IgnitionPhaseTimer_BEGIN"))) {
			IgnitionPhaseTimer.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "SparksDeenergizedTimer_BEGIN", sizeof("SparksDeenergizedTimer_BEGIN"))) {
			SparksDeenergizedTimer.LoadState(scn, "TD_END");
		}
	}
}

void SIVBSystems::CreateParticleEffects(double TRANZ)
{
	//TRANZ is CG location on z-axis (Orbiter)

	//LH2 NPV: at STA 1601.9 (Saturn IB). Vent 1 is near position I, rotated 25° to position IV
	if (LH2_NPV_Stream1) vessel->DelExhaustStream(LH2_NPV_Stream1);
	LH2_NPV_Stream1 = vessel->AddParticleStream(&lh2_npv_venting_spec, _V(-1.395485500267790, 2.992628312795018, 1601.9*0.0254 - TRANZ), _V(-0.422618261740699, 0.906307787036650, 0.0), &LH2_NPV_Stream_Lvl);
	
	if (LH2_NPV_Stream2) vessel->DelExhaustStream(LH2_NPV_Stream2);
	LH2_NPV_Stream2 = vessel->AddParticleStream(&lh2_npv_venting_spec, _V(1.395485500267790, -2.992628312795018, 1601.9*0.0254 - TRANZ), _V(0.422618261740699, -0.906307787036650, 0.0), &LH2_NPV_Stream_Lvl);

	//LH2 CVS: location TBD
	if (LH2_CVS_Stream1) vessel->DelExhaustStream(LH2_CVS_Stream1);
	LH2_CVS_Stream1 = vessel->AddParticleStream(&lh2_cvs_venting_spec, _V(-2.352827103720118, 2.352827103720118, 1600.0*0.0254 - TRANZ), _V(0.0, 0.0, -1.0), &LH2_CVS_Stream_Lvl);

	if (LH2_CVS_Stream2) vessel->DelExhaustStream(LH2_CVS_Stream2);
	LH2_CVS_Stream2 = vessel->AddParticleStream(&lh2_cvs_venting_spec, _V(2.352827103720118, -2.352827103720118, 1600.0*0.0254 - TRANZ), _V(0.0, 0.0, -1.0), &LH2_CVS_Stream_Lvl);

	//LOX NPV: at STA 1199.304 (Saturn IB). Vent I is near position IV, rotated 14°27' to position III
	if (LOX_NPV_Stream1) vessel->DelExhaustStream(LOX_NPV_Stream1);
	LOX_NPV_Stream1 = vessel->AddParticleStream(&lox_npv_venting_spec, _V(-3.197543770821965, -0.823964704145541, 1199.304*0.0254 - TRANZ), _V(-0.968365769479699, -0.249535040625542, 0.0), &LOX_NPV_Stream_Lvl);

	if (LOX_NPV_Stream2) vessel->DelExhaustStream(LOX_NPV_Stream2);
	LOX_NPV_Stream2 = vessel->AddParticleStream(&lox_npv_venting_spec, _V(3.197543770821965, 0.823964704145541, 1199.304*0.0254 - TRANZ), _V(0.968365769479699, 0.249535040625542, 0.0), &LOX_NPV_Stream_Lvl);

	//LOX dump: approximately at STA 962.304 (Saturn IB)
	if (LOX_Dump_Stream) vessel->DelExhaustStream(LOX_Dump_Stream);
	LOX_Dump_Stream = vessel->AddParticleStream(&lox_dump_venting_spec, _V(0, 0, 962.304*0.0254 - TRANZ), _V(0.0, 0.0, -1.0), &LOX_Dump_Stream_Lvl);
}

bool SIVBSystems::BurnerRelaysReset()
{
	return (!BurnerLH2PropellantValveCloseOn && !BurnerLH2PropellantValveOpenOn && !BurnerLOXShutdownValveCloseOn && !BurnerLOXShutdownValveOpenOn && !BurnerExcitersOn);
}

bool SIVBSystems::RepressRelaysReset()
{
	return (!LOXTankRepressControlValveOpenOn && !LH2TankRepressurizationControlValveOpenOn && !RepressSystemModeSelectOn && !BurnerAutoCutoffSystemArm);
}

void SIVBSystems::Timestep(double simdt)
{
	if (j2engine == NULL) return;

	HeliumControlDeenergizedTimer.Timestep(simdt);
	StartTankDischargeDelayTimer.Timestep(simdt);
	IgnitionPhaseTimer.Timestep(simdt);
	SparksDeenergizedTimer.Timestep(simdt);

	//Thrust OK switch
	bool ThrustOK = vessel->GetThrusterLevel(j2engine) > 0.65 && !EngineFailed;

	//Engine Control Power Switch (EDS no. 1, range safety no. 1)
	if (EDSEngineStop || RSSEngineStop)
	{
		AftPowerDisableRelay = true;
	}
	else
	{
		AftPowerDisableRelay = false;
	}

	//Engine Power
	if (!AftPowerDisableRelay)
	{
		EnginePower = true;
	}
	else
	{
		EnginePower = false;
	}

	if (EnginePower && ThrustOK)
	{
		ThrustOKRelay = true;
	}
	else
	{
		ThrustOKRelay = false;
	}

	//Propellant Systems
	if (main_propellant)
	{
		UpdatePropellants();
		LH2PropellantCalculations(simdt);
		LOXPropellantCalculations(simdt);
		O2H2Burner(simdt);
		ChilldownSystem();

		//Set new propellant mass
		lastPropellantMass = oxidMass + fuelMass;
		vessel->SetPropellantMass(main_propellant, lastPropellantMass);
		//Inhibit thruster
		if (oxidMass == 0.0 || fuelMass == 0.0)
		{
			vessel->SetThrusterResource(j2engine, NULL);
		}
	}

	bool B4D11 = true; //Aft Bus No. 1
	bool B4D31 = true;

	bool K22;

	if ((B4D31 && RSSEngineStop) || (EDSEngineStop && !EDSCutoffDisabled))
	{
		K22 = true;
	}
	else
	{
		K22 = false;
	}

	if (EngineControlBusMotorSwitch == false && !K22 && (ESECommandEngineControlBusPowerOn() || (B4D11 && EDSCutoffDisabled)))
	{
		EngineControlBusMotorSwitch = true;
	}
	else if (EngineControlBusMotorSwitch == true && ((B4D11 && K22) || (!K22 && ESECommandEngineControlBusPowerOff())))
	{
		EngineControlBusMotorSwitch = false;
	}

	bool K101 = (B4D11 && EngineControlBusMotorSwitch);
	//Engine ignition bus
	bool K103 = true;

	//Passivation
	EDSCutoffNo2DisableInhibit = K101;

	J2SignalH = (B4D11 && EngineControlBusMotorSwitch && PassivationRelay);
	J2SignalF = (J2SignalH && EngineHeliumControlValveOpen);
	J2SignalD = (J2SignalH && EngineMainstageControlValveOpen);
	J2SignalE = (J2SignalH && EngineIgnitionPhaseControlValveOpenOn);

	//Propellant Depletion
	if (PropellantLowLevel())
	{
		PropellantDepletionSensors = true;
	}
	else
	{
		PropellantDepletionSensors = false;
	}

	//Signals A and X from Saturn Systems Handbook
	//Engine Cutoff Bus (switch selector, range safety system no. 2, EDS no. 2, propellant depletion sensors)
	CutoffSignalX = ((!EDSCutoffDisabled && EDSEngineStop) || RSSEngineStop || PropellantDepletionSensors);
	CutoffSignalA = CutoffSignalX || LVDCEngineStopRelay;
	EngineStop = (CutoffSignalX || CutoffSignalA || CutoffLockup);

	//ENGINE MONITOR BOARD (EM)
	EMTEMP1 = (EngineStartLockUp || HeliumControlOn || IgnitionPhaseControlOn || StartTankDischargeControlOn || MainstageOn || SparkSystemOn || ThrustOKRelay);
	EngineReady1 = (!ThrustOKRelay && !EMTEMP1); //TBD: More
	EngineReady = (EngineReady1 || EngineReadyBypass);

	//sprintf(oapiDebugString(), "ENGINE MONITOR BOARD: EMTEMP1 %d EngineReady1 %d EngineReady %d", EMTEMP1, EngineReady1, EngineReady);

	//EngineReadyBypass would only set be as long as the switch selector powers it
	EngineReadyBypass = false;

	//ENGINE START BOARD
	if (EngineStop)
	{
		HeliumControlDeenergizedTimer.SetRunning(true);
	}
	EngineStart4 = (EngineStop || SparksDeenergized);
	EngineStartLockUp = (EngineStart || SparkSystemOn); //TBD: Require S-II/S-IVB staging
	SparkSystemOn = (EngineStartLockUp && K101 && !EngineStart4);
	EngineStart3 = HeliumControlOn || SparkSystemOn;
	HeliumControlOn = (EngineStart3 && K101 && !HeliumControlDeenergizedTimer.ContactClosed()) || J2SignalF;

	//sprintf(oapiDebugString(), "ENGINE START BOARD: EngineStart %d EngineStartLockUp %d SparkSystemOn %d EngineStart3 %d EngineStart4 %d HeliumControlOn %d Timer %lf", EngineStart, EngineStartLockUp, SparkSystemOn, EngineStart3, EngineStart4, HeliumControlOn, HeliumControlDeenergizedTimer.GetTime());

	//PROGRAMMER BOARD
	PBSignal1 = (FuelInjTempOKBypass || StartTurbines);
	if (HeliumControlOn)
	{
		StartTankDischargeDelayTimer.SetRunning(true);
	}
	StartTurbines = (K101 && PBSignal1 && StartTankDischargeDelayTimer.ContactClosed());
	if (StartTurbines)
	{
		IgnitionPhaseTimer.SetRunning(true);
	}
	PBSignal4 = IgnitionPhaseTimer.ContactClosed();
	if (PBSignal4)
	{
		SparksDeenergizedTimer.SetRunning(true);
	}
	SparksDeenergized = SparksDeenergizedTimer.ContactClosed();

	//sprintf(oapiDebugString(), "PB: Byp %d R1: %d Tim1 %lf R3: %d Tim2 %lf R4: %d Tim3 %lf Spark %d", FuelInjTempOKBypass, PBSignal1, StartTankDischargeDelayTimer.GetTime(), StartTurbines, IgnitionPhaseTimer.GetTime(), PBSignal4, SparksDeenergizedTimer.GetTime(), SparksDeenergized);

	//VALVE CONTROL BOARD
	VCBSignal1 = EngineStop || PBSignal4;
	StartTankDischargeControlOn = (StartTurbines && K101 && !VCBSignal1);
	VCBSignal2 = MainstageSignal || SparkSystemOn;
	VCBSignal3 = VCBSignal2 && K101 && !EngineStop;
	MainstageSignal = K101 && VCBSignal3 && (MainstageSignal || PBSignal4) && !EngineStop;

	IgnitionPhaseControlOn = (VCBSignal3 || J2SignalE);
	MainstageOn = (MainstageSignal || J2SignalD);

	//sprintf(oapiDebugString(), "He %d STDV: %d Ign: %d Main: %d", HeliumControlOn, StartTankDischargeControlOn, IgnitionPhaseControlOn, MainstageOn);

	//CUTOFF CONTROL NO. 1 BOARD
	IgnitionDetector = true;
	CC1Signal1 = HeliumControlOn && IgnitionDetectionLockup;
	IgnitionDetectionLockup = CC1Signal1 || IgnitionDetector;

	//CUTOFF CONTROL NO. 2 BOARD
	CC2Signal1 = PBSignal4 && !IgnitionDetectionLockup;
	CC2Signal2 = !ThrustOKRelay && SparksDeenergized;
	CC2Signal3 = CC2Signal1 || CC2Signal2 || EngineStop || CutoffLockup;
	CutoffLockup = !EngineReady && CC2Signal3 && K101;

	//sprintf(oapiDebugString(), "CCB 1: %d 2: %d 3: %d 4: %d", CC2Signal1, CC2Signal2, CC2Signal3, CutoffLockup);

	switch (EngineState)
	{
	case 0: //Off
		if (EngineOnLogic())
		{
			EngineState = 1;
			ThrustTimer = 0.0;
		}
		else
		{
			break;
		}
	case 1: //Starting
		if (!EngineOnLogic())
		{
			ThrustTimer = 0.0;
			EngineState = 3;
			break;
		}
		ThrustTimer += simdt;

		//Thrust rising linearly from STDV opening for 2.8 seconds to 100%
		if (ThrustTimer < 2.8)
		{
			ThrustLevel = min(ThrustTimer*0.357, 1.0);
		}
		else
		{
			ThrustLevel = 1.0;
			EngineState = 2;
		}
		vessel->SetThrusterLevel(j2engine, ThrustLevel);
		break;
	case 2: //Running
		if (!EngineOnLogic())
		{
			ThrustTimer = 0.0;
			EngineState = 3;
		}
		else
		{
			ThrustLevel = 1.0;
			vessel->SetThrusterLevel(j2engine, ThrustLevel);
			break;
		}
	case 3: //Stopping
		ThrustTimer += simdt;
		if (ThrustTimer < 0.25)
		{
			// 86% of thrust dies in the first .25 second
			ThrustLevel = 1.0 - (ThrustTimer*3.456);
		}
		else if (ThrustTimer < 1.5)
		{
			// The remainder dies over the next 1.25 second
			ThrustLevel = 0.136 - ((ThrustTimer - 0.25)*0.1088);
		}
		else
		{
			// Engine is completely shut down at 1.5 second
			ThrustLevel = 0.0;
			EngineState = 0;
			ThrustTimer = 0.0;
		}
		vessel->SetThrusterLevel(j2engine, ThrustLevel);
		break;
	}

	//sprintf(oapiDebugString(), "State: %d Timer %lf Level: %lf", EngineState, ThrustTimer, ThrustLevel);

	//Venting

	if (vernier)
	{
		if (vessel->GetThrusterGroupLevel(vernier) < 1.0 && FireUllageIgnitionOn)
		{
			vessel->SetThrusterGroupLevel(vernier, 1.0);
		}
	}

	//sprintf(oapiDebugString(), "Ready %d Start %d FuelInjTempOKBypass %d Stop %d Cut Inhibit %d Level %f Timer %f", EngineReady, EngineStart, FuelInjTempOKBypass, EngineStop, ThrustOKCutoffInhibit, ThrustLevel, ThrustTimer);

	/*if (DebugTimer < oapiGetSimTime())
	{
		char Buffer[128];

		sprintf(Buffer, "Time %lf LH2 M %lf LH2 P %lf LOX M %lf LOX P %lf CVS F %lf", BoiloffTime + 540.0, fuelMass, LH2TankUllagePressurePSI, oxidMass, LOXTankUllagePressurePSI, F_CVS);
		oapiWriteLog(Buffer);
		DebugTimer = oapiGetSimTime() + 10.0;
	}*/
}

bool SIVBSystems::EngineOnLogic()
{
	return HeliumControlOn && IgnitionPhaseControlOn && (StartTankDischargeControlOn || MainstageOn);
}

void SIVBSystems::UpdatePropellants()
{
	double p;

	p = vessel->GetPropellantMass(main_propellant);

	if (!propellantInitialized)
	{
		if (oxidMass == -1) {
			oxidMass = p * PropellantLoadMixtureRatio / (1.0 + PropellantLoadMixtureRatio);
		}
		propellantInitialized = true;
	}
	else
	{
		if (lastPropellantMass != p)
		{
			double of = (MixtureRatio / (1.0 + MixtureRatio));
			oxidMass -= (lastPropellantMass - p) * of;
			oxidMass = max(oxidMass, 0);

		}
	}
	fuelMass = p - oxidMass;
	//lastPropellantMass gets updated after LH2 and LOX calculations

	//sprintf(oapiDebugString(), "Prop %lf MR %lf Prop ratio %lf Ox %lf Fuel %lf", p, MixtureRatio, oxidMass / fuelMass, oxidMass, fuelMass);
}

void SIVBSystems::UpdateLH2ValveStates()
{
	//TBD: ESE for nearly everything

	//CVS Orifice Shutoff Valve
	if (LH2TankContinuousVentOrificeSOVOpenOn && !LH2TankContinuousVentValveCloseOn)
	{
		LH2CVSOrificeShutoffValve = true;
	}
	else if (!LH2TankContinuousVentOrificeSOVOpenOn && LH2TankContinuousVentValveCloseOn)
	{
		LH2CVSOrificeShutoffValve = false;
	}

	//CVS Relief Override Shutoff Valve
	if (LH2TankContinuousVentReliefOverrideSOVOpenOn && !LH2TankContinuousVentValveCloseOn)
	{
		LH2CVSReliefOverrideShutoffValve = true;
	}
	else if (!LH2TankContinuousVentReliefOverrideSOVOpenOn && LH2TankContinuousVentValveCloseOn)
	{
		LH2CVSReliefOverrideShutoffValve = false;
	}

	//Latching Relief Valve Latch
	if (LH2TankLatchingReliefValveLatchOn && LH2LatchingReliefValve)
	{
		LH2LatchingReliefValveLatch = true;
	}

	//Latching Relief Valve
	if (LH2TankLatchingReliefValveOpenOn && !LH2TankVentAndLatchingReliefValveBoostCloseOn)
	{
		LH2LatchingReliefValve = true;
	}
	else if (!LH2TankLatchingReliefValveOpenOn && LH2TankVentAndLatchingReliefValveBoostCloseOn)
	{
		LH2LatchingReliefValve = false;
		LH2LatchingReliefValveLatch = false;
	}
	else
	{
		//Based on pressure
		if (!LH2LatchingReliefValve && LH2TankUllagePressurePSI > 34.0)
		{
			LH2LatchingReliefValve = true;
		}
		else if (!LH2LatchingReliefValveLatch && LH2LatchingReliefValve && LH2TankUllagePressurePSI < 31.0)
		{
			LH2LatchingReliefValve = false;
		}
	}

	//Fuel Vent and Relief Valve
	if (LH2TankVentValveOpen && !LH2TankVentAndLatchingReliefValveBoostCloseOn)
	{
		LH2FuelVentAndReliefValve = true;
	}
	else if (!LH2TankVentValveOpen && LH2TankVentAndLatchingReliefValveBoostCloseOn)
	{
		LH2FuelVentAndReliefValve = false;
	}
	else
	{
		//Based on pressure
		if (!LH2FuelVentAndReliefValve && LH2TankUllagePressurePSI > 34.0)
		{
			LH2FuelVentAndReliefValve = true;
		}
		else if (LH2FuelVentAndReliefValve && LH2TankUllagePressurePSI < 31.0)
		{
			LH2FuelVentAndReliefValve = false;
		}
	}
}

void SIVBSystems::UpdateO2H2BurnerValveStates()
{
	if (BurnerLH2PropellantValveOpenOn && !BurnerLH2PropellantValveCloseOn)
	{
		HeliumHeaterLH2PropellantValve = true;
	}
	else if (!BurnerLH2PropellantValveOpenOn && BurnerLH2PropellantValveCloseOn)
	{
		HeliumHeaterLH2PropellantValve = false;
	}

	if (BurnerLOXShutdownValveOpenOn && !BurnerLOXShutdownValveCloseOn)
	{
		HeliumHeaterLOXShutdownValve = true;
	}
	else if (!BurnerLOXShutdownValveOpenOn && BurnerLOXShutdownValveCloseOn)
	{
		HeliumHeaterLOXShutdownValve = false;
	}
}

void SIVBSystems::LH2PropellantCalculations(double simdt)
{
	//Only run this from S-II/S-IVB staging on!

	//Timer for heat flow
	BoiloffTime += simdt;

	//To make sure the valve states are right
	UpdateLH2ValveStates();

	//LH2TankUllagePressurePSI = LOXTankUllagePressurePSI * 0.9362 + 3.19;

	double FuelPercentage, UllageVolume, heatflow, mdot_boil, m_boil, P, CVSRegulator, CVSBypass, mdot_gas_CVS, mdot_gas_NPV, mdot_gas;

	FuelPercentage = fuelMass / (vessel->GetPropellantMaxMass(main_propellant) * 1.0 / (PropellantLoadMixtureRatio + 1.0));
	UllageVolume = LH2_TANK_VOLUME * (1.0 - FuelPercentage);

	if (UllageVolume < 1000.0)
	{
		//Tank full, bypass
		LH2TankUllagePressurePSI = 31.0;
		//Reset particle stream levels
		LH2_CVS_Stream_Lvl = 0.0;
		LH2_NPV_Stream_Lvl = 0.0;
		//sprintf(oapiDebugString(), "Tank Full! UllageVolume %lf", UllageVolume);
		return;
	}
	if (vessel->GetThrusterLevel(j2engine) > 0.01)
	{
		//Cheaty mass during thrust
		LH2TankUllagePressurePSI = 31.0;
		double InitPress = LH2TankUllagePressurePSI / PSI;
		GH2_Mass = InitPress * UllageVolume / R / GH2_TEMP * M_H2 / 1000.0;
		//Reset particle stream levels
		LH2_CVS_Stream_Lvl = 0.0;
		LH2_NPV_Stream_Lvl = 0.0;
		//sprintf(oapiDebugString(), "Burn active! UllageVolume %lf GH2_Mass %lf", UllageVolume, GH2_Mass);
		return;
	}

	if (GH2_Mass == -1)
	{
		LH2TankUllagePressurePSI = 31.0;
		double InitPress = LH2TankUllagePressurePSI / PSI;
		GH2_Mass = InitPress * UllageVolume / R / GH2_TEMP * M_H2 / 1000.0;
	}

	//Calculate heat flow, J/g
	if (BoiloffTime < 10.0)
	{
		heatflow = 0.0;
	}
	else
	{
		heatflow = 100000.0 / 1.5 * BTU / 3600.0 * (3.0 * exp(-BoiloffTime / (2363.0 + 500.0)) + 0.9)*(FuelPercentage); //Empirical
	}
	//Calculate mass rate that gets vaporized
	mdot_boil = heatflow / LH2_ENTHALPY_VAPORIZATION / 1000.0; //kg
	//Calculate mass that goes from LH2 to GH2
	m_boil = mdot_boil * simdt;
	//Calculate new fuel mass
	if (fuelMass - m_boil < 0.0)
	{
		m_boil = fuelMass;
		fuelMass = 0.0;
	}
	else
	{
		fuelMass = fuelMass - m_boil;
	}
	
	//Add mass to total GH2 mass
	GH2_Mass = GH2_Mass + m_boil;

	//Repressurization
	if (LH2AmbientRepressurizationOn)
	{
		GH2_Mass += LH2_AMBIENT_HELIUM_MASS_FLOW * simdt;
	}
	if (LH2CryoRepressurizationOn)
	{
		GH2_Mass += LH2_CRYO_HELIUM_MASS_FLOW * simdt;
	}

	//Calculate pressure
	P = R * GH2_TEMP*GH2_Mass*1000.0 / M_H2 / UllageVolume;
	//Calculate pressure in PSI
	LH2TankUllagePressurePSI = P * PSI;

	//Pressure switch
	if (LH2PressureSwitch == false && LH2TankUllagePressurePSI > 31.0)
	{
		LH2PressureSwitch = true;
	}
	else if (LH2PressureSwitch == true && LH2TankUllagePressurePSI < 28.0)
	{
		LH2PressureSwitch = false;
	}

	//Continuous Vent System
	if (LH2CVSReliefOverrideShutoffValve)
	{
		if (LH2TankUllagePressurePSI > 21.0)
		{
			CVSRegulator = 1.0;
		}
		else if (LH2TankUllagePressurePSI < 19.5)
		{
			CVSRegulator = 0.0;
		}
		else
		{
			CVSRegulator = (LH2TankUllagePressurePSI - 19.5) / 1.5;
		}
	}
	else
	{
		CVSRegulator = 0.0;
	}

	if (LH2CVSOrificeShutoffValve)
	{
		CVSBypass = 1.0;
	}
	else
	{
		CVSBypass = 0.0;
	}
	//Gas flow rate of the CVS
	mdot_gas_CVS = LH2TankUllagePressurePSI * (CVSRegulator*CVS_VALVE_SIZE + CVSBypass * CVS_BYPASS_VALVE_SIZE);
	//Thrust
	F_CVS = mdot_gas_CVS * LH2_CVS_ISP;

	//Non-propulsive vent
	mdot_gas_NPV = 0.0;
	if (LH2LatchingReliefValve) mdot_gas_NPV += LH2_NPV_VALVE_SIZE * LH2TankUllagePressurePSI;
	if (LH2FuelVentAndReliefValve) mdot_gas_NPV += LH2_NPV_VALVE_SIZE * LH2TankUllagePressurePSI;

	//Calculate total venting flowrate
	mdot_gas = mdot_gas_CVS + mdot_gas_NPV;
	//Calculate updated GH2 mass
	GH2_Mass = max(0.0, GH2_Mass - mdot_gas * simdt);

	//sprintf(oapiDebugString(), "Fuel %lf Ullage %lf BoiloffTime %lf heatflow %lf, mdot_boil %lf m_boil %lf GH2_Mass %lf Press %lf", FuelPercentage, UllageVolume, BoiloffTime, heatflow, mdot_boil, m_boil, GH2_Mass, LH2TankUllagePressurePSI);
	//sprintf(oapiDebugString(), "CVS %d Press %lf, Regulator %lf Bypass %lf mdot %lf F %lf NPV %d mdot_gas_NPV %lf", LH2CVSReliefOverrideShutoffValve, LH2TankUllagePressurePSI, CVSRegulator, CVSBypass, mdot_gas_CVS, F_CVS, LH2LatchingReliefValve || LH2FuelVentAndReliefValve, mdot_gas_NPV);

	//Orbiter stuff below here
	//Apply thrust
	if (F_CVS > 0.0)
	{
		vessel->AddForce(_V(0.0, 0.0, F_CVS), _V(0, 0, 0));
	}

	//Particle effect for CVS
	LH2_CVS_Stream_Lvl = min(1.0, mdot_gas_CVS);
	//Particle effect for NPV
	LH2_NPV_Stream_Lvl = min(1.0, mdot_gas_NPV);
}

void SIVBSystems::UpdateLOXValveStates()
{
	//NPV Vent and Relief Valve Latch
	if (LOXTankNPVValveLatchOpenOn && LOXNPVVentAndReliefValve)
	{
		LOXNPVValveLatch = true;
	}

	//LOX NPV Vent and Relief Valve
	if (LOXTankNPVValveOpenOn && !LOXTankVentAndNPVValvesBoostCloseOn)
	{
		LOXNPVVentAndReliefValve = true;
	}
	else if (!LOXTankNPVValveOpenOn && LOXTankVentAndNPVValvesBoostCloseOn)
	{
		LOXNPVVentAndReliefValve = false;
		LOXNPVValveLatch = false;
	}
	else
	{
		if (LOXNPVVentAndReliefValve == false && LOXTankUllagePressurePSI > 45.5)
		{
			LOXNPVVentAndReliefValve = true;
		}
		else if (!LOXNPVValveLatch && LOXNPVVentAndReliefValve == true && LOXTankUllagePressurePSI < 42.5)
		{
			LOXNPVVentAndReliefValve = false;
		}
	}

	//LOX Vent and Relief Valve
	if (LOXTankVentValveOpen && !LOXTankVentAndNPVValvesBoostCloseOn)
	{
		LOXVentAndReliefValve = true;
	}
	else if (!LOXTankVentValveOpen && LOXTankVentAndNPVValvesBoostCloseOn)
	{
		LOXVentAndReliefValve = false;
	}
	else
	{
		if (LOXVentAndReliefValve == false && LOXTankUllagePressurePSI > 45.5)
		{
			LOXVentAndReliefValve = true;
		}
		else if (LOXVentAndReliefValve == true && LOXTankUllagePressurePSI < 42.5)
		{
			LOXVentAndReliefValve = false;
		}
	}
}

void SIVBSystems::LOXPropellantCalculations(double simdt)
{
	double OxidPercentage, UllageVolume, P, mdot_gas_NPV, mdot_gas_vent, mdot_gas_leak;
	bool LOXSignalA, LOXSignalD, K74;

	UpdateLOXValveStates();

	OxidPercentage = oxidMass / (vessel->GetPropellantMaxMass(main_propellant) * PropellantLoadMixtureRatio*(1.0 + PropellantLoadMixtureRatio));
	UllageVolume = LO2_TANK_VOLUME * (1.0 - OxidPercentage);

	if (UllageVolume < 1000.0)
	{
		//Tank full, bypass
		LOXTankUllagePressurePSI = 41.0;
		//Reset particle stream levels
		LOX_NPV_Stream_Lvl = 0.0;
		LOX_Dump_Stream_Lvl = 0.0;
		//sprintf(oapiDebugString(), "Tank Full! UllageVolume %lf OxidPercentage %lf", UllageVolume, OxidPercentage);
		return;
	}
	if (vessel->GetThrusterLevel(j2engine) > 0.01)
	{
		//Cheaty mass during thrust
		LOXTankUllagePressurePSI = 41.0;
		double InitPress = LOXTankUllagePressurePSI / PSI;
		GO2_Mass = InitPress * UllageVolume / R / GO2_TEMP * M_O2 / 1000.0;
		//Reset particle stream levels
		LOX_NPV_Stream_Lvl = 0.0;
		LOX_Dump_Stream_Lvl = 0.0;
		//sprintf(oapiDebugString(), "Burn active! UllageVolume %lf GH2_Mass %lf", UllageVolume, GH2_Mass);
		return;
	}

	//Pressure switch
	if (LOXPressureSwitch == false && LOXTankUllagePressurePSI > 41.0)
	{
		LOXPressureSwitch = true;
	}
	else if (LOXPressureSwitch == true && LOXTankUllagePressurePSI < 38.0)
	{
		LOXPressureSwitch = false;
	}

	//Flight pressurization
	LOXSignalA = LOXPressureSwitch;
	LOXSignalD = !LOXPressureSwitch;
	K74 = LOXSignalA;

	bool LOXFlightPressurizationOff;
	if (LOXTankPressurizationShutoffValvesClose || (K74 && !LOXTankFlightPressureSystemOn))
	{
		LOXFlightPressurizationOff = true;
	}
	else
	{
		LOXFlightPressurizationOff = false;
	}

	if (GO2_Mass == -1)
	{
		LOXTankUllagePressurePSI = 41.0;
		double InitPress = LOXTankUllagePressurePSI / PSI;
		GO2_Mass = InitPress * UllageVolume / R / GO2_TEMP * M_O2 / 1000.0;
	}

	//Repressurization
	if (LOXAmbientRepressurizationOn)
	{
		GO2_Mass += LOX_AMBIENT_HELIUM_MASS_FLOW * simdt;
	}
	if (LOXCryoRepressurizationOn)
	{
		GO2_Mass += LOX_CRYO_HELIUM_MASS_FLOW * simdt;
	}

	//Calculate pressure
	P = R * GO2_TEMP*GO2_Mass*1000.0 / M_O2 / UllageVolume;
	//Calculate pressure in PSI
	LOXTankUllagePressurePSI = P * PSI;

	//Non-propulsive vent
	mdot_gas_NPV = 0.0;
	if (LOXNPVVentAndReliefValve)
	{
		mdot_gas_NPV = LOX_NPV_VALVE_SIZE * LOXTankUllagePressurePSI;

		if (GO2_Mass < mdot_gas_NPV*simdt)
		{
			GO2_Mass = mdot_gas_NPV = 0.0;
		}
		else
		{
			GO2_Mass = GO2_Mass - mdot_gas_NPV * simdt;
		}
	}
	LOX_NPV_Stream_Lvl = min(1.0, mdot_gas_NPV / 4.0);

	//Propulsive vent
	mdot_gas_vent = 0.0;
	if (LOXVentAndReliefValve)
	{
		mdot_gas_vent = LOX_VENT_VALVE_SIZE * LOXTankUllagePressurePSI;

		if (GO2_Mass < mdot_gas_vent*simdt)
		{
			GO2_Mass = mdot_gas_vent = 0.0;
		}
		else
		{
			GO2_Mass = GO2_Mass - mdot_gas_vent * simdt;

			//Vent thrust
			vessel->AddForce(_V(0, 0, mdot_gas_vent*GOX_VENT_ISP), _V(0, 0, 0));
			//sprintf(oapiDebugString(), "%lf", mdot_gas_vent*GOX_VENT_ISP);
		}
	}

	//Pressure loss in EPO, simulated as a simple leak, but likely a temperature decrease and other factors
	mdot_gas_leak = 2.039614202645544e-04*LOXTankUllagePressurePSI;

	//Calculate updated GOX mass
	GO2_Mass = max(0.0, GO2_Mass - mdot_gas_leak * simdt);

	//LOX dump
	double F_LOX_Dump = 0.0;
	double mdot_GOX_dump = 0.0;

	if (oxidMass > 0.0 && MainstageOn && HeliumControlOn && !EngineOnLogic())
	{
		//LOX dump mass flow
		double mdot_lox_dump = min(20.0, 0.06*oxidMass);
		if (oxidMass < mdot_lox_dump*simdt)
		{
			mdot_lox_dump = 0.0;
			oxidMass = 0.0;
		}
		else
		{
			oxidMass -= mdot_lox_dump * simdt;
		}

		//Gas ingestion
		if (oxidMass < 248.569) //Value from AS-205 S-IVB Flight Evaluation Report
		{
			mdot_GOX_dump = (1.0 - oxidMass / 248.569) / 40.0*LOXTankUllagePressurePSI;

			if (GO2_Mass < mdot_GOX_dump*simdt)
			{
				mdot_GOX_dump = 0.0;
				GO2_Mass = 0.0;
			}
			else
			{
				GO2_Mass = GO2_Mass - mdot_GOX_dump * simdt;
			}
		}

		F_LOX_Dump = mdot_lox_dump * LOX_DUMP_ISP + mdot_GOX_dump * GOX_DUMP_ISP;
		vessel->AddForce(_V(0, 0, F_LOX_Dump), _V(0, 0, 0));

		LOX_Dump_Stream_Lvl = min(1.0, F_LOX_Dump / 2500.0);
	}
	else
	{
		LOX_Dump_Stream_Lvl = 0.0;
	}

	//sprintf(oapiDebugString(), "LOX %lf GOX %lf mdot_gas_NPV %lf mdot_gas_leak %lf F_LOX_Dump %lf", oxidMass, GO2_Mass, mdot_gas_NPV, mdot_gas_leak, F_LOX_Dump);
	//sprintf(oapiDebugString(), "Passivation %d EngineControlBus %d HeliumControl %d Mainstage %d HeliumControlOn %d MainstageOn %d oxidMass %lf F_LOX_Dump %lf GO2_Mass %lf mdot_GOX_dump %lf", PassivationRelay, EngineControlBusMotorSwitch, EngineHeliumControlValveOpen, EngineMainstageControlValveOpen, HeliumControlOn, MainstageOn, oxidMass, F_LOX_Dump, GO2_Mass, mdot_GOX_dump);
}

void SIVBSystems::O2H2Burner(double simdt)
{
	double TempBurnerChamberDomeF;
	bool BurnerMalfunction;
	bool LOXSignalA, LOXSignalB, LOXSignalC, LOXSignalD, LOXSignalV;
	bool K52; //LH2 tank repress backup pressure switch disable
	bool K75; //LOX repressurization disabled if energized
	bool K76; //LH2 repressurization disabled if energized
	bool K87; //Ambient repress mode if energized, otherwise cryo

	TempBurnerChamberDomeF = O2H2BurnerFailed ? -407.72 : -408.72; //TBD: Simulate

	//Malfunction
	if (TempBurnerChamberDomeF > -408.57 || TempBurnerChamberDomeF  < -408.87)
	{
		BurnerMalfunction = true;
	}
	else
	{
		BurnerMalfunction = false;
	}

	if (BurnerAutoCutoffSystemArm && BurnerMalfunction)
	{
		BurnerMalfunctionSignal = true;
	}
	else
	{
		BurnerMalfunctionSignal = false;
	}

	if (BurnerMalfunctionSignal)
	{
		BurnerShutdown();
	}

	//Is burner operating?
	if (O2H2BurnerOn == false && BurnerExcitersOn && BurnerLogic())
	{
		O2H2BurnerOn = true;
	}
	else if (O2H2BurnerOn && !BurnerLogic())
	{
		O2H2BurnerOn = false;
	}

	//LH2
	if (RepressSystemModeSelectOn)
	{
		K52 = K87 = true;
	}
	else
	{
		K52 = K87 = false;
	}

	if (LH2PressureSwitch) //TBD: Backup
	{
		K76 = true;
	}
	else
	{
		K76 = false;
	}

	LH2AmbientRepressurizationOn = (LH2TankRepressurizationControlValveOpenOn && !K76 && K87);
	LH2CryoRepressurizationOn = (O2H2BurnerOn && LH2TankRepressurizationControlValveOpenOn && !K76 && !K87);
	
	//LOX
	bool LOXColdHeliumPressureSwitch = false; //TBD

	LOXSignalA = LOXPressureSwitch;
	LOXSignalD = !LOXPressureSwitch;
	LOXSignalC = LOXSignalD && !LOXColdHeliumPressureSwitch;

	K75 = (LOXSignalC && LOXTankRepressControlValveOpenOn); //TBD: K83
	LOXSignalB = (K87 && K75); //Ambient mode
	LOXSignalV = (!K87 && K75); //Cryo mode

	LOXAmbientRepressurizationOn = LOXSignalB;
	LOXCryoRepressurizationOn = (O2H2BurnerOn && LOXSignalV);

	//Thrust and effects
	double Force = 0.0;
	if (O2H2BurnerOn)
	{
		O2H2BurnerTime += simdt;

		//TBD: Take fuel and oxidizer mass

		if (O2H2BurnerTime < 200.0)
		{
			Force = 50.0 + O2H2BurnerTime / 200.0*70.0;
		}
		else
		{
			Force = 120.0;
		}
		vessel->AddForce(_V(0.0, 0.0, Force), _V(0.0, 0.0, 0.0));
	}
	else
	{
		O2H2BurnerTime = 0.0;
	}

	//sprintf(oapiDebugString(), "BURNER: LH2 In %d LOX In %d Exciter %d Burner On %d Force %lf LH2 Repress Amb %d Cryo %d LOX Repress Amb %d Cryo %d", HeliumHeaterLH2PropellantValve,
	//	HeliumHeaterLOXShutdownValve, BurnerExcitersOn, O2H2BurnerOn, Force, LH2AmbientRepressurizationOn, LH2CryoRepressurizationOn, LOXAmbientRepressurizationOn, LOXCryoRepressurizationOn);
}

bool SIVBSystems::BurnerLogic() const
{
	//For continuous operation
	return (HeliumHeaterLH2PropellantValve && HeliumHeaterLOXShutdownValve && fuelMass > 0.0 && oxidMass > 0.0);
}

void SIVBSystems::BurnerShutdown()
{
	//Signal J in Saturn Systems Handbook
	LOXTankRepressControlValveOpenOn = false;
	LH2TankRepressurizationControlValveOpenOn = false;
	BurnerExcitersOn = false;
	RepressSystemModeSelectOn = true;
	BurnerLOXShutdownValveOpenOn = false;
	BurnerLOXShutdownValveCloseOn = true;
	BurnerLH2PropellantValveOpenOn = false;
	BurnerLH2PropellantValveCloseOn = true;
}

void SIVBSystems::ChilldownSystem()
{
	bool SignalA, SignalB, K3;

	K3 = ThrustOKRelay;

	if (PrevalvesCloseOn && (!K3 || LVDCEngineStopRelay))
	{
		SignalA = true;
	}
	else
	{
		SignalA = false;
	}

	SignalB = ChilldownShutoffValveCloseOn;

	//TBD: Prevalves and chilldown valves
}

void SIVBSystems::SetThrusterDir(double beta_y, double beta_p)
{
	if (j2engine == NULL) return;
	//Check if aux or main hydraulic pumps are on
	if (!AuxHydPumpFlightMode && vessel->GetThrusterLevel(j2engine) < 0.1) return;

	VECTOR3 j2vector;

	if (beta_y > 7.0*RAD)
	{
		j2vector.x = 7.0*RAD;
	}
	else if (beta_y < -7.0*RAD)
	{
		j2vector.x = -7.0*RAD;
	}
	else
	{
		j2vector.x = beta_y;
	}

	if (beta_p > 7.0*RAD)
	{
		j2vector.y = 7.0*RAD;
	}
	else if (beta_p < -7.0*RAD)
	{
		j2vector.y = -7.0*RAD;
	}
	else
	{
		j2vector.y = beta_p;
	}

	j2vector.z = 1.0;

	vessel->SetThrusterDir(j2engine, j2vector);
}

bool SIVBSystems::PropellantLowLevel()
{
	if (PointLevelSensorArmed)
	{
		if (main_propellant)
		{
			if (vessel->GetPropellantMass(main_propellant) < 50.0)
			{
				return true;
			}
		}
	}

	return false;
}

void SIVBSystems::APSUllageEngineOn(int n)
{
	if (n < 1 || n > 2) return;
	if (ullage[n - 1])
		vessel->SetThrusterLevel(ullage[n - 1], 1);
}

void SIVBSystems::APSUllageEngineOff(int n)
{
	if (n < 1 || n > 2) return;
	if (ullage[n - 1])
		vessel->SetThrusterLevel(ullage[n - 1], 0);
}

void SIVBSystems::SetAPSAttitudeEngine(int n, bool on)
{
	if (apsThrusters[0])
	{
		if (on)
		{
			vessel->SetThrusterLevel(apsThrusters[n], 1.0);
		}
		else
		{
			vessel->SetThrusterLevel(apsThrusters[n], 0.0);
		}
	}
}

void SIVBSystems::SetPUValve(int state)
{
	if (state == PUVALVE_CLOSED)
	{
		PUValveState = PUVALVE_CLOSED;
	}
	else if (state == PUVALVE_NULL)
	{
		PUValveState = PUVALVE_NULL;
	}
	else if (state == PUVALVE_OPEN)
	{
		PUValveState = PUVALVE_OPEN;
	}

	RecalculateEngineParameters();
}

void SIVBSystems::RecalculateEngineParameters(double BaseThrust)
{
	J2DefaultThrust = BaseThrust;

	RecalculateEngineParameters();
}

//
// Get the J2 ISP from the mixture ratio and calculate the thrust adjustment.
//

#define MR_STATS 5

static double MixtureRatios[MR_STATS] = { 6.0, 5.5, 5.0, 4.3, 4.0 };
static double MRISP[MR_STATS] = { 421.4 * G, 423.4 * G, 426.5 * G, 432.6 * G, 437.6 * G };
static double MRThrust[MR_STATS] = { 1.1, 1.0, .898, .7391, .7 };

void SIVBSystems::GetJ2ISP(double ratio, double &isp, double &ThrustAdjust)

{
	isp = 421 * G;

	// From Usenet:
	// It had roughly three stops. 178,000 lbs at 425s Isp and an O/F of 4.5,
	// 207,000 lbs at 421s Isp and an O/F of 5.0, and 230,500 lbs at 418s Isp
	// and an O/F of 5.5.

	for (int i = 0; i < MR_STATS; i++) {
		if (ratio >= MixtureRatios[i]) {
			double delta = (ratio - MixtureRatios[i]) / (MixtureRatios[i - 1] - MixtureRatios[i]);

			isp = MRISP[i] + ((MRISP[i - 1] - MRISP[i]) * delta);
			ThrustAdjust = MRThrust[i] + ((MRThrust[i - 1] - MRThrust[i]) * delta);

			return;
		}
	}
}

void SIVBSystems::SetEngineFailed()
{
	EngineFailed = true;
}

void SIVBSystems::SetO2H2BurnerFailed(bool fail)
{
	O2H2BurnerFailed = fail;
}

SIVB200Systems::SIVB200Systems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THGROUP_HANDLE &ver)
	: SIVBSystems(v, j2, j2prop, aps, ull, ver, 4.9)
{
}

void SIVB200Systems::RecalculateEngineParameters()
{
	if (PUValveState == PUVALVE_CLOSED)
	{
		MixtureRatio = 5.5;
	}
	else if (PUValveState == PUVALVE_OPEN)
	{
		MixtureRatio = 4.5;
	}
	else
	{
		MixtureRatio = 5.0;
	}

	SetSIVBMixtureRatio(MixtureRatio);
}

void SIVB200Systems::SetSIVBMixtureRatio(double ratio)

{
	double isp, thrust;

	// Hardcoded ISP and thrust according to the the Apollo 7 Saturn IB Report, NTRS ID 19900067467
	if (ratio >= 5.25) {
		thrust = 1009902;
		isp = 424 * G;

	}
	else if (ratio >= 4.75){
		thrust = 889951.0;
		isp = 426 * G;
	}
	else {
		thrust = 770000.;
		isp = 428 * G;
	}

	//
	// For simplicity assume no ISP change at sea-level: SIVb stage should always
	// be in near-vacuum anyway.
	//

	vessel->SetThrusterIsp(j2engine, isp, isp);
	vessel->SetThrusterMax0(j2engine, thrust);
}

void SIVB200Systems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 5: //PU Activate On
		break;
	case 6: //PU Activate Off
		break;
	case 7: //P.U. Inverter and DC Power On
		PUInverterAndDCPowerOn = true;
		break;
	case 8: //P.U. Inverter and DC Power Off
		PUInverterAndDCPowerOn = false;
		break;
	case 9: //Engine Ignition Sequence Start
		EngineStart = true;
		break;
	case 10: //Engine Ready Bypass On
		SetEngineReadyBypass();
		break;
	case 11: //Fuel Injection Temperature Ok Bypass
		FuelInjTempOKBypass = true;
		break;
	case 12: //S-IVB Engine Cutoff No. 1 On
		LVDCEngineStopRelay = true;
		break;
	case 13: //S-IVB Engine Cutoff No. 1 Off
		LVDCEngineStopRelay = false;
		break;
	case 16: //Fuel Injection Temperature Ok Bypass Reset
		FuelInjTempOKBypass = false;
		break;
	case 19: //Engine Ready Bypass On (AS-205) or LH2 Tank Latching Relief Valve Latch Off (AS-206)
		if (VehicleNo < 206)
		{
			SetEngineReadyBypass();
		}
		else
		{
			LH2TankLatchingReliefValveLatchOn = false;
			UpdateLH2ValveStates();
		}
		break;
	case 22: //LOX Chilldown Pump On
		LOXChilldownPumpOn = true;
		break;
	case 23: //LOX Chilldown Pump Off
		LOXChilldownPumpOn = false;
		break;
	case 24: //Engine Pump Purge Control Valve Enable On
		EnginePumpPurgeControlValveEnableOn = true;
		break;
	case 25: //Engine Pump Purge Control Valve Enable Off
		EnginePumpPurgeControlValveEnableOn = false;
		break;
	case 27: //Engine Ignition Sequence Start Relay Reset
		EngineStart = false;
		break;
	case 28: //Aux Hydraulic Pump Flight Mode On
		AuxHydPumpFlightModeOn();
		break;
	case 29: //Aux Hydraulic Pump Flight Mode Off
		AuxHydPumpFlightModeOff();
		break;
	case 30: //Mainstage Control Valve Open On
		EngineMainstageControlValveOpen = true;
		break;
	case 32: //P.U. Mixture Ratio 4.5 On
		SetPUValve(PUVALVE_OPEN);
		//SPUShiftS.play(NOLOOP, 255);
		//SPUShiftS.done();
		break;
	case 33: //P.U. Mixture Ratio 4.5 Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 34: //P.U. Mixture Ratio 5.5 On
		SetPUValve(PUVALVE_CLOSED);
		break;
	case 35: //P.U. Mixture Ratio 5.5 Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 37: //LOX Tank NPV Valve Open Off
		LOXTankNPVValveOpenOn = false;
		UpdateLOXValveStates();
		break;
	case 38: //LH2 Tank Vent Valve Open
		LH2TankVentValveOpen = true;
		UpdateLH2ValveStates();
		break;
	case 42: //LOX Tank NPV Valve Open On
		LOXTankNPVValveOpenOn = true;
		UpdateLOXValveStates();
		break;
	case 43: //S-IVB Engine Cutoff No. 2 On
		LVDCEngineStopRelay = true;
		break;
	case 49: //S-IVB Engine Cutoff No. 2 Off
		LVDCEngineStopRelay = false;
		break;
	case 52: //LH2 Tank Latching Relief Valve Latch On
		LH2TankLatchingReliefValveLatchOn = true;
		UpdateLH2ValveStates();
		break;
	case 54: //Charge Ullage Ignition On
		ChargeUllageIgnitionOn = true;
		break;
	case 55: //Charge Ullage Jettison On
		ChargeUllageJettisonOn = true;
		break;
	case 56: //Fire Ullage Ignition On
		FireUllageIgnitionOn = true;
		break;
	case 57: //Fire Ullage Jettison On
		FireUllageJettisonOn = true;
		break;
	case 58: //Fuel Chilldown Pump On
		FuelChillDownPumpOn = true;
		break;
	case 59: //Fuel Chilldown Pump Off
		FuelChillDownPumpOn = false;
		break;
	case 62: //TM Calibrate On
		TelemetryCalibrateOn = true;
		break;
	case 63: //TM Calibrate Off
		TelemetryCalibrateOn = false;
		break;
	case 64: //Engine Ignition Phase Control Valve Open On
		EngineIgnitionPhaseControlValveOpenOn = true;
		break;
	case 65: //Engine Mainstage and Ignition Phase Control Valves Open Off
		EngineMainstageControlValveOpen = false;
		EngineIgnitionPhaseControlValveOpenOn = false;
		break;
	case 73: //Ullage Firing Reset
		FireUllageIgnitionOn = false;
		FireUllageJettisonOn = false;
		break;
	case 76: //LH2 Tank Vent Valve Close
		LH2TankVentValveOpen = false;
		UpdateLH2ValveStates();
		break;
	case 77: //LH2 Tank Vent and Latching Relief Valves Boost Close On
		LH2TankVentAndLatchingReliefValveBoostCloseOn = true;
		UpdateLH2ValveStates();
		break;
	case 78: //LH2 Tank Vent and Latching Relief Valves Boost Close Off
		LH2TankVentAndLatchingReliefValveBoostCloseOn = false;
		UpdateLH2ValveStates();
		break;
	case 79: //LOX Tank Flight Pressurization Shutoff Valves Close On
		LOXTankPressurizationShutoffValvesClose = true;
		break;
	case 80: //LOX Tank Flight Pressurization Shutoff Valves Close Off
		LOXTankPressurizationShutoffValvesClose = false;
		break;
	case 82: //Prevalves Close On
		PrevalvesCloseOn = true;
		break;
	case 83: //Prevalves Close Off
		PrevalvesCloseOn = false;
		break;
	case 85: //Passivation Enable
		PassivationRelay = true; //TBD: require K57 (S-IB/S-IVB separation interlock)
		break;
	case 86: //Passivation Disable
		PassivationRelay = false;
		break;
	case 88: //Ullage Charging Reset
		ChargeUllageIgnitionOn = false;
		ChargeUllageJettisonOn = false;
		break;
	case 91: //Chilldown Shutoff Valves Close On
		ChilldownShutoffValveCloseOn = true;
		break;
	case 92: //Chilldown Shutoff Valves Close Off
		ChilldownShutoffValveCloseOn = false;
		break;
	case 93: //LOX Vent Open Command On
		LOXTankVentValveOpen = true;
		UpdateLOXValveStates();
		break;
	case 94: //LOX Vent Open Command Off
		LOXTankVentValveOpen = false;
		UpdateLOXValveStates();
		break;
	case 95: //LOX Tank Vent and NPV Valves Boost Close On
		LOXTankVentAndNPVValvesBoostCloseOn = true;
		UpdateLOXValveStates();
		break;
	case 96: //LOX Tank Vent and NPV Valves Boost Close Off
		LOXTankVentAndNPVValvesBoostCloseOn = false;
		UpdateLOXValveStates();
		break;
	case 97: //Point Level Sensor Arming
		PointLevelSensorArming();
		break;
	case 98: //Point Level Sensor Disarming
		PointLevelSensorDisarming();
		break;
	case 99: //LH2 Tank Latching Relief Valve Open On
		LH2TankLatchingReliefValveOpenOn = true;
		UpdateLH2ValveStates();
		break;
	case 100: //LH2 Tank Latching Relief Valve Open Off
		LH2TankLatchingReliefValveOpenOn = false;
		UpdateLH2ValveStates();
		break;
	case 101: //LOX Tank NPV Valve Latch Open On
		LOXTankNPVValveLatchOpenOn = true;
		break;
	case 102: //LOX Tank NPV Valve Latch Open Off
		LOXTankNPVValveLatchOpenOn = false;
		break;
	case 103: //LOX Tank Flight Pressurization System On
		LOXTankFlightPressureSystemOn = true;
		break;
	case 104: //LOX Tank Flight Pressurization System Off
		LOXTankFlightPressureSystemOn = false;
		break;
	case 109: //Engine Helium Control Valve Open On
		EngineHeliumControlValveOpen = true;
		break;
	case 110: //Engine Helium Control Valve Open Off
		EngineHeliumControlValveOpen = false;
		break;
	default:
		break;
	}
}

SIVB500Systems::SIVB500Systems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THGROUP_HANDLE &ver)
	: SIVBSystems(v, j2, j2prop, aps, ull, ver, 4.45)
{
	
}

void SIVB500Systems::RecalculateEngineParameters()
{
	if (PUValveState == PUVALVE_CLOSED)
	{
		MixtureRatio = 5.5;
	}
	else if (PUValveState == PUVALVE_OPEN)
	{
		MixtureRatio = 4.5;
	}
	else
	{
		MixtureRatio = 4.946;
	}

	SetSIVBMixtureRatio(MixtureRatio);
}

void SIVB500Systems::SetSIVBMixtureRatio(double ratio)
{
	double isp, thrust, ThrustAdjust;

	GetJ2ISP(ratio, isp, ThrustAdjust);
	thrust = J2DefaultThrust * ThrustAdjust;

	//
	// For simplicity assume no ISP change at sea-level: SIVb stage should always
	// be in near-vacuum anyway.
	//

	vessel->SetThrusterIsp(j2engine, isp, isp);
	vessel->SetThrusterMax0(j2engine, thrust);
}

void SIVB500Systems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 1: //Passivation Enable (AS-503) or Start Tank Vent Control Valve Open On (later)
		if (VehicleNo <= 504) PassivationRelay = true; //TBD: require K57 (S-IC/S-II separation interlock)
		else StartTankVentControlValveOpenOn = true;
		break;
	case 2: //Passivation Disable (AS-503) or Start Tank Vent Control Valve Open Off (later)
		if (VehicleNo <= 504) PassivationRelay = false;
		else StartTankVentControlValveOpenOn = false;
		break;
	case 3: //LOX Tank Repressurization Control Valve Open On
		LOXTankRepressControlValveOpenOn = true;
		break;
	case 4: //LOX Tank Repressurization Control Valve Open Off
		LOXTankRepressControlValveOpenOn = false;
		break;
	case 5: //PU Activate On
		break;
	case 6: //PU Activate Off
		break;
	case 7: //PU Inverter and DC Power On
		PUInverterAndDCPowerOn = true;
		break;
	case 8: //PU Inverter and DC Power Off
		PUInverterAndDCPowerOn = false;
		break;
	case 9: //S-IVB Engine Start On
		EngineStart = true;
		break;
	case 10: //Engine Ready Bypass
		SetEngineReadyBypass();
		break;
	case 11: //Fuel Injection Temperature OK Bypass
		FuelInjTempOKBypass = true;
		break;
	case 12: //S-IVB Engine Cutoff
		LVDCEngineStopRelay = true;
		break;
	case 13: //S-IVB Engine Cutoff Off
		LVDCEngineStopRelay = false;
		break;
	case 14: //Engine Mainstage Control Valve Open On (AS-506 and earlier) or Start Tank Recharge Valve Open (AS-508 and later)
		if (VehicleNo <= 506)
		{
			EngineMainstageControlValveOpen = true;
		}
		else
		{
			StartTankRechargeValveOpen = true;
		}
		break;
	case 15: //Engine Mainstage Control Valve Open Off (AS-506 and earlier) or Single Sideband System Disable (later)
		if (VehicleNo <= 506)
		{
			EngineMainstageControlValveOpen = false;
			EngineIgnitionPhaseControlValveOpenOn = false;
		}
		else
		{

		}
		break;
	case 16: //Fuel Injector Temperature OK Bypass Reset
		FuelInjTempOKBypass = false;
		break;
	case 17: //PU Valve Hardover Position On
		SetPUValve(PUVALVE_OPEN);
		break;
	case 18: //PU Valve Hardover Position Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 19: //S-IVB Engine EDS Cutoff No. 2 Disable (AS-503) or LH2 Tank Latching Relief Valve Latch Off (AS-507+ at least)
		if (VehicleNo <= 506)
		{
			if (!EDSCutoffNo2DisableInhibit) EDSCutoffDisabled = true;
		}
		else
		{
			LH2TankLatchingReliefValveLatchOn = false;
			UpdateLH2ValveStates();
		}
		break;
	case 20: //LOX Chilldown Pump Purge Control Valve Enable On (AS-503) or Ambient Helium Supply Shutoff Valve Closed On (later)
		if (VehicleNo <= 504)
		{

		}
		else
		{
			AmbientHeliumSupplyShutoffValveClosedOn = true;
		}
		break;
	case 21: //LOX Chilldown Pump Purge Control Valve Enable Off (AS-503) or Ambient Helium Supply Shutoff Valve Closed Off (later)
		if (VehicleNo <= 504)
		{

		}
		else
		{
			AmbientHeliumSupplyShutoffValveClosedOn = false;
		}
		break;
	case 22: //LOX Chilldown Pump On
		LOXChilldownPumpOn = true;
		break;
	case 23: //LOX Chilldown Pump Off
		LOXChilldownPumpOn = false;
		break;
	case 24: //Engine Pump Purge Control Valve Enable On
		EnginePumpPurgeControlValveEnableOn = true;
		break;
	case 25: //Engine Pump Purge Control Valve Enable Off
		EnginePumpPurgeControlValveEnableOn = false;
		break;
	case 26: //Burner LH2 Propellant Valve Open On
		BurnerLH2PropellantValveOpenOn = true;
		UpdateO2H2BurnerValveStates();
		break;
	case 27: //S-IVB Engine Start Off
		EngineStart = false;
		break;
	case 28: //Aux Hydraulic Pump Flight Mode On
		AuxHydPumpFlightModeOn();
		break;
	case 29: //Aux Hydraulic Pump Flight Mode Off
		AuxHydPumpFlightModeOff();
		break;
	case 30: //Start Bottle Vent Control Valve Open On (AS-506 and earlier), Mainstage Control Valve Open On (later)
		if (VehicleNo <= 506)
		{

		}
		else
		{
			EngineMainstageControlValveOpen = true;
		}
		break;
	case 31: //Engine Mainstage Control Valve Open Off (AS-507 and earlier) or Start Tank Recharge Valve Close & Disarm (later)
		if (VehicleNo <= 507)
		{
			EngineMainstageControlValveOpen = false;
		}
		else
		{
			StartTankRechargeValveOpen = false;
			StartTankRechargeValveArm = false;
		}
		break;
	case 32: //Second Burn Relay On
		SecondBurnRelay = true;
		break;
	case 33: //Second Burn Relay Off
		SecondBurnRelay = false;
		break;
	case 34: //PU Fuel Boil Off Bias Command On
		break;
	case 35: //PU Fuel Boil Off Bias Command Off
		break;
	case 36: //Repressurization System Mode Select On (Amb)
		RepressSystemModeSelectOn = true;
		break;
	case 37: //Repressurization System Mode Select Off (Cryo)
		RepressSystemModeSelectOn = false;
		break;
	case 38: //LH2 Tank Vent Open Command On
		LH2TankVentValveOpen = true;
		UpdateLH2ValveStates();
		break;
	case 39: //LH2 Tank Repressurization Control Valve Open On
		LH2TankRepressurizationControlValveOpenOn = true;
		UpdateLH2ValveStates();
		break;
	case 40: //PCM Group System On (AS-503) or Passivation Enable (later)
		if (VehicleNo <= 504)
		{

		}
		else
		{
			PassivationRelay = true;
		}
		break;
	case 41: //PCM Group System Off (AS-503) or Passivation Disable (later)
		if (VehicleNo <= 504)
		{

		}
		else
		{
			PassivationRelay = false;
		}
		break;
	case 42: //S-IVB Ullage Engine No. 1 On
		APSUllageEngineOn(1);
		break;
	case 43: //S-IVB Ullage Engine No. 1 Off
		APSUllageEngineOff(1);
		break;
	case 44: //LOX Tank NPV Valve Latch Open On
		LOXTankNPVValveLatchOpenOn = true;
		break;
	case 45: //LOX Tank NPV Valve Latch Open Off
		LOXTankNPVValveLatchOpenOn = false;
		break;
	case 46: //Single Sideband FM Transmitter On (AS-506 and earlier) or Regular Calibrate Relays On (later)
		break;
	case 47: //Single Sideband FM Transmitter Off (AS-506 and earlier)
		break;
	case 48: //Inflight Calibration Mode On
		InflightCalibrateModeOn = true;
		break;
	case 49: //Inflight Calibration Mode Off
		InflightCalibrateModeOn = false;
		break;
	case 50: //Heat-Exchanger Bypass Valve Control Enable
		LOXHeatExchangerBypassValveControlEnable = true;
		break;
	case 51: //Heat-Exchanger Bypass Valve Control Disable
		LOXHeatExchangerBypassValveControlEnable = false;
		break;
	case 52: //Measurement Transfer Mode Position "B" (AS-503) or LH2 Tank Latching Relief Valve Latch On (later)
		if (VehicleNo <= 506)
		{

		}
		else
		{
			LH2TankLatchingReliefValveLatchOn = true;
		}
		break;
	case 53: //Spare (AS-506 and earlier) or S-IVB Engine EDS Cutoff No. 2 Disable (later)
		if (VehicleNo <= 506)
		{

		}
		else
		{
			if (!EDSCutoffNo2DisableInhibit) EDSCutoffDisabled = true;
		}
		break;
	case 54: //Charge Ullage Ignition On
		ChargeUllageIgnitionOn = true;
		break;
	case 55: //Charge Ullage Jettison On
		ChargeUllageJettisonOn = true;
		break;
	case 56: //Fire Ullage Ignition On
		FireUllageIgnitionOn = true;
		break;
	case 57: //Fire Ullage Jettison On
		FireUllageJettisonOn = true;
		break;
	case 58: //Fuel Chilldown Pump On
		FuelChillDownPumpOn = true;
		break;
	case 59: //Fuel Chilldown Pump Off
		FuelChillDownPumpOn = false;
		break;
	case 60: //Burner LH2 Propellant Valve Close On
		BurnerLH2PropellantValveCloseOn = true;
		UpdateO2H2BurnerValveStates();
		break;
	case 61: //Burner LH2 Propellant Valve Close Off
		BurnerLH2PropellantValveCloseOn = false;
		UpdateO2H2BurnerValveStates();
		break;
	case 62: //TM Calibrate On
		TelemetryCalibrateOn = true;
		break;
	case 63: //TM Calibrate Off
		TelemetryCalibrateOn = false;
		break;
	case 64: //LH2 Tank Latching Relief Valve Latch On (AS-506 and earlier) or Engine Ignition Phase Control Valve Open On (later)
		if (VehicleNo <= 506)
		{
			LH2TankLatchingReliefValveLatchOn = true;
			UpdateLH2ValveStates();
		}
		else
		{
			EngineIgnitionPhaseControlValveOpenOn = true;
		}
		break;
	case 65: //LH2 Tank Latching Relief Valve Latch Off (AS-506 and earlier) or Spare? (AS-507) Engine Mainstage and Ignition Phase Control Valves Open Off (later)
		if (VehicleNo <= 506)
		{
			LH2TankLatchingReliefValveLatchOn = false;
			UpdateLH2ValveStates();
		}
		else if (VehicleNo >= 508)
		{

			EngineMainstageControlValveOpen = false;
			EngineIgnitionPhaseControlValveOpenOn = false;
		}
		break;
	case 66: //RF Assembly Power Command On
		break;
	case 67: //RF Assembly Power Command Off (AS-503) or EDS Cutoff No. 2 Enable (later)
		if (VehicleNo <= 504)
		{

		}
		else
		{
			EDSCutoffDisabled = false;
		}
		break;
	case 68: //First Burn Relay On
		FirstBurnRelay = true;
		break;
	case 69: //First Burn Relay Off
		FirstBurnRelay = false;
		break;
	case 70: //Burner Exciters On
		BurnerExcitersOn = true;
		break;
	case 71: //Burner Exciters Off
		BurnerExcitersOn = false;
		break;
	case 72: //Burner LH2 Propellant Valve Open Off
		BurnerLH2PropellantValveOpenOn = false;
		UpdateO2H2BurnerValveStates();
		break;
	case 73: //Ullage Firing Reset
		FireUllageIgnitionOn = false;
		FireUllageJettisonOn = false;
		break;
	case 74: //Burner LOX Shutdown Valve Close On
		BurnerLOXShutdownValveCloseOn = true;
		UpdateO2H2BurnerValveStates();
		break;
	case 75: //Burner LOX Shutdown Valve Close Off
		BurnerLOXShutdownValveCloseOn = false;
		UpdateO2H2BurnerValveStates();
		break;
	case 76: //LH2 Tank Vent Open Command Off
		LH2TankVentValveOpen = false;
		UpdateLH2ValveStates();
		break;
	case 77: //LH2 Tank Vent and Latching Relief Valve Boost Close On
		LH2TankVentAndLatchingReliefValveBoostCloseOn = true;
		UpdateLH2ValveStates();
		break;
	case 78: //LH2 Tank Vent and Latching Relief Valve Boost Close Off
		LH2TankVentAndLatchingReliefValveBoostCloseOn = false;
		UpdateLH2ValveStates();
		break;
	case 79: //LOX Tank Pressurization Shutoff Valves Close
		LOXTankPressurizationShutoffValvesClose = true;
		break;
	case 80: //LOX Tank Pressurization Shutoff Valves Open
		LOXTankPressurizationShutoffValvesClose = false;
		break;
	case 81: //LH2 Tank Repressurization Control Valve Open Off
		LH2TankRepressurizationControlValveOpenOn = false;
		UpdateLH2ValveStates();
		break;
	case 82: //Prevalves Close On
		PrevalvesCloseOn = true;
		break;
	case 83: //Prevalves Close Off
		PrevalvesCloseOn = false;
		break;
	case 84: //LH2 Tank Continuous Vent Valve Close On
		LH2TankContinuousVentValveCloseOn = true;
		UpdateLH2ValveStates();
		break;
	case 85: //Burner Automatic Cutoff System Arm
		BurnerAutoCutoffSystemArm = true;
		break;
	case 86: //Burner Automatic Cutoff System Disarm
		BurnerAutoCutoffSystemArm = false;
		break;
	case 87: //LH2 Tank Continuous Vent Valve Close Off
		LH2TankContinuousVentValveCloseOn = false;
		UpdateLH2ValveStates();
		break;
	case 88: //Ullage Charging Reset
		ChargeUllageIgnitionOn = false;
		ChargeUllageJettisonOn = false;
		break;
	case 89: //Burner LOX Shutdown Valve Open On
		BurnerLOXShutdownValveOpenOn = true;
		UpdateO2H2BurnerValveStates();
		break;
	case 90: //Burner LOX Shutdown Valve Open Off
		BurnerLOXShutdownValveOpenOn = false;
		UpdateO2H2BurnerValveStates();
		break;
	case 91: //Chilldown Shut-Off Pilot On
		ChilldownShutoffValveCloseOn = true;
		break;
	case 92: //Chilldown Shut-Off Pilot Off
		ChilldownShutoffValveCloseOn = false;
		break;
	case 93: //LOX Vent Open Command On
		LOXTankVentValveOpen = true;
		UpdateLOXValveStates();
		break;
	case 94: //LOX Vent Open Command Off
		LOXTankVentValveOpen = false;
		UpdateLOXValveStates();
		break;
	case 95: //LOX Tank Vent and NPV Valves Boost Close On
		LOXTankVentAndNPVValvesBoostCloseOn = true;
		UpdateLOXValveStates();
		break;
	case 96: //LOX Tank Vent and NPV Valves Boost Close Off
		LOXTankVentAndNPVValvesBoostCloseOn = false;
		UpdateLOXValveStates();
		break;
	case 97: //Point Level Sensor Arming
		PointLevelSensorArming();
		break;
	case 98: //Point Level Sensor Disarming (AS-503) or Start Tank Recharge Valve Arm (later)
		if (VehicleNo <= 507) PointLevelSensorDisarming();
		else StartTankRechargeValveArm = true;
		break;
	case 99: //LH2 Tank Latching Relief Valve Open On
		LH2TankLatchingReliefValveOpenOn = true;
		UpdateLH2ValveStates();
		break;
	case 100: //LH2 Tank Latching Relief Valve Open Off
		LH2TankLatchingReliefValveOpenOn = false;
		UpdateLH2ValveStates();
		break;
	case 101: //S-IVB Ullage Engine No. 2 On
		APSUllageEngineOn(2);
		break;
	case 102: //S-IVB Ullage Engine No. 2 Off
		APSUllageEngineOff(2);
		break;
	case 103: //LOX Tank Flight Pressure System On
		LOXTankFlightPressureSystemOn = true;
		break;
	case 104: //LOX Tank Flight Pressure System Off
		LOXTankFlightPressureSystemOn = false;
		break;
	case 105: //LOX Tank NPV Valve Open On
		LOXTankNPVValveOpenOn = true;
		UpdateLOXValveStates();
		break;
	case 106: //LOX Tank NPV Valve Open Off
		LOXTankNPVValveOpenOn = false;
		UpdateLOXValveStates();
		break;
	case 107: //LH2 Tank Continous Vent Relief Override Shutoff Valve Open On
		LH2TankContinuousVentReliefOverrideSOVOpenOn = true;
		UpdateLH2ValveStates();
		break;
	case 108: //LH2 Tank Continous Vent Relief Override Shutoff Valve Open Off
		LH2TankContinuousVentReliefOverrideSOVOpenOn = false;
		UpdateLH2ValveStates();
		break;
	case 109: //Engine He Control Valve Open On
		EngineHeliumControlValveOpen = true;
		break;
	case 110: //Engine He Control Valve Open Off
		EngineHeliumControlValveOpen = false;
		break;
	case 111: //LH2 Tank Continuous Vent Orifice Shutoff Valve Open On
		LH2TankContinuousVentOrificeSOVOpenOn = true;
		UpdateLH2ValveStates();
		break;
	case 112: //LH2 Tank Continuous Vent Orifice Shutoff Valve Open Off
		LH2TankContinuousVentOrificeSOVOpenOn = false;
		UpdateLH2ValveStates();
		break;
	default:
		break;
	}
}