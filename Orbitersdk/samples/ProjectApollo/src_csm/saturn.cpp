/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant, Matthias Mueller

  ORBITER vessel module: generic Saturn base class

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
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"
#include "sm.h"
#include "sivb.h"
#include "lemcomputer.h"
#include "LEM.h"
#include "papi.h"

#include "CollisionSDK/CollisionSDK.h"
#include <crtdbg.h>

extern "C" {
#include <lua\lua.h>
#include <lua\lualib.h>
#include <lua\lauxlib.h>
}

//
// Random functions from Yaagc.
//

extern "C" {
	void srandom (unsigned int x);
	long int random ();
}

//extern FILE *PanelsdkLogFile;


// DX8 callback for enumerating joysticks
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pSaturn)
{
	class Saturn * sat = (Saturn*)pSaturn; // Pointer to us
	HRESULT hr;

	if(sat->js_enabled > 1){  // Do we already have enough joysticks?
		return DIENUM_STOP; } // If so, stop enumerating additional devices.

	// Obtain an interface to the enumerated joystick.
    hr = sat->dx8ppv->CreateDevice(pdidInstance->guidInstance, &sat->dx8_joystick[sat->js_enabled], NULL);
	
	if(FAILED(hr)) {              // Did that work?
		return DIENUM_CONTINUE; } // No, keep enumerating (if there's more)

	sat->js_enabled++;      // Otherwise, Next!
	return DIENUM_CONTINUE; // and keep enumerating
}

// DX8 callback for enumerating joystick axes
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pSaturn )
{
	class Saturn * sat = (Saturn*)pSaturn; // Pointer to us

    if (pdidoi->guidType == GUID_ZAxis) {
		if (sat->js_current == sat->rhc_id) {
			sat->rhc_rzx_id = 1;
		} else {
			sat->thc_rzx_id = 1;
		}
	}

    if (pdidoi->guidType == GUID_RzAxis) {
		if (sat->js_current == sat->rhc_id) {
			sat->rhc_rot_id = 2;
		} else {
			sat->thc_rot_id = 2;
		}
	}

    if (pdidoi->guidType == GUID_POV) {
		if (sat->js_current == sat->rhc_id) {
			sat->rhc_pov_id = 0;
		} else {
			sat->thc_pov_id = 0;
		}
	}
    return DIENUM_CONTINUE;
}

//
// CAUTION: This disables the warning, which is triggered by the use of the "this" pointer in the 
// initializations of iuCommandConnector, sivbControlConnector and sivbCommandConnector below. 
// This warning can ignored here, because the "this" pointer is only stored in the constructors, 
// but not used. Except for these 3 classes the "this" pointer still should NOT be used in initializations. 
//

#pragma warning ( push )
#pragma warning ( disable:4355 )

Saturn::Saturn(OBJHANDLE hObj, int fmodel) : ProjectApolloConnectorVessel (hObj, fmodel), 

	agc(soundlib, dsky, dsky2, imu, Panelsdk, iuCommandConnector, sivbControlConnector), 
	dsky(soundlib, agc, 015),
	dsky2(soundlib, agc, 016), 
	imu(agc, Panelsdk),
	cws(SMasterAlarm, Bclick, Panelsdk),
	dockingprobe(0, SDockingCapture, SDockingLatch, SDockingExtend, SUndock, CrashBumpS, Panelsdk),
	NonEssBus1("Non-Essential-Bus1", &NonessBusSwitch),
	NonEssBus2("Non-Essential-Bus2", &NonessBusSwitch),
	ACBus1PhaseA("AC-Bus1-PhaseA", 115, NULL),
	ACBus1PhaseB("AC-Bus1-PhaseB", 115, NULL),
	ACBus1PhaseC("AC-Bus1-PhaseC", 115, NULL),
	ACBus2PhaseA("AC-Bus2-PhaseA", 115, NULL),
	ACBus2PhaseB("AC-Bus2-PhaseB", 115, NULL),
	ACBus2PhaseC("AC-Bus2-PhaseC", 115, NULL),
	ACBus1("ACBus1", Panelsdk),
	ACBus2("ACBus2", Panelsdk),
	SIVBToCSMPowerSource("SIVBToCSMPower", Panelsdk),
	CSMToLEMPowerDrain("CSMToLEMPower", Panelsdk),
	MainBusAController("MainBusAController", Panelsdk),
	MainBusBController("MainBusBController", Panelsdk),
	BatteryBusA("Battery-Bus-A", Panelsdk),
	BatteryBusB("Battery-Bus-B", Panelsdk),
	PyroBusAFeeder("Pyro-Bus-A-Feeder", Panelsdk),
	PyroBusBFeeder("Pyro-Bus-B-Feeder", Panelsdk),
	PyroBusA("Pyro-Bus-A", NULL),
	PyroBusB("Pyro-Bus-B", NULL),
	SECSLogicBusA("SECSLogic-Bus-A", NULL),
	SECSLogicBusB("SECSLogic-Bus-B", NULL),	
	BatteryRelayBus("Battery-Relay-Bus", Panelsdk),
	FlightBus("Flight-Bus",&FlightBusFeeder),
	FlightBusFeeder("Flight-Bus-Feeder",Panelsdk),
	FlightPostLandingBus("FlightPostLanding-Bus",&FlightPostLandingBusFeeder),
	FlightPostLandingBusFeeder("FlightPostLanding-Bus-Feeder",Panelsdk, 5),
	LMUmbilicalFeeder("LM-Umbilical-Feeder", Panelsdk),
	SwitchPower("Switch-Power", Panelsdk),
	GaugePower("Gauge-Power", Panelsdk),
	SMQuadARCS(ph_rcs0, Panelsdk),
	SMQuadBRCS(ph_rcs1, Panelsdk),
	SMQuadCRCS(ph_rcs2, Panelsdk),
	SMQuadDRCS(ph_rcs3, Panelsdk),
	CMRCS1(ph_rcs_cm_1, Panelsdk),
	CMRCS2(ph_rcs_cm_2, Panelsdk),
	SPSPropellant(ph_sps, Panelsdk),
	SPSEngine(th_main[0]),
	CMSMPyros("CM-SM-Pyros", Panelsdk),
	CMSMPyrosFeeder("CM-SM-Pyros-Feeder", Panelsdk),
	CMDockingRingPyros("CM-DockingRing-Pyros", Panelsdk),
	CMDockingRingPyrosFeeder("CM-DockingRing-Pyros-Feeder", Panelsdk),
	CSMLVPyros("CSM-LV-Pyros", Panelsdk),
	CSMLVPyrosFeeder("CSM-LV-Pyros-Feeder", Panelsdk),
	EcsGlycolPumpsSwitch(Panelsdk),
	SuitCompressor1Switch(Panelsdk),
	SuitCompressor2Switch(Panelsdk),
	BatteryCharger("BatteryCharger", Panelsdk),
	timedSounds(soundlib),
	iuCommandConnector(agc, this),
	sivbControlConnector(agc, dockingprobe, this),
	sivbCommandConnector(this),
	checkControl(soundlib),
	MFDToPanelConnector(MainPanel, checkControl),
	ascp(ThumbClick),
	RHCNormalPower("RHCNormalPower", Panelsdk),
	RHCDirect1Power("RHCDirect1Power", Panelsdk),
	RHCDirect2Power("RHCDirect2Power", Panelsdk),
	ems(Panelsdk),
	CabinPressureReliefValve1(PostLandingVentSound),
	CabinPressureReliefValve2(PostLandingVentSound),
	CrewStatus(CrewDeadSound),
	CSMDCVoltMeter(20.0, 45.0),
	CSMACVoltMeter(90.0, 140.0),
	DCAmpMeter(0.0, 100.0),
	SystemTestAttenuator("SystemTestAttenuator", 0.0, 256.0, 0.0, 5.0),
	SystemTestVoltMeter(0.0, 5.0),
	EMSDvSetSwitch(Sclick),
	SideHatch(HatchOpenSound, HatchCloseSound)	// SDockingCapture

#pragma warning ( pop ) // disable:4355

{	
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF|_CRTDBG_CHECK_ALWAYS_DF );
	InitSaturnCalled = false;
	autopilot = false;
	LastTimestep = 0;

	//
	// VESSELSOUND initialisation
	// 
	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	cws.MonitorVessel(this);
	dockingprobe.RegisterVessel(this);

	//Initialize the link to the MFD's debug function.
	debugString = 0;
	debugConnected = false;
	HMODULE hdbg = GetModuleHandle("modules//plugin//ProjectApolloMFD.dll");
	if (hdbg)
	{
		debugString = (char *(__cdecl *)()) GetProcAddress(hdbg,"pacMFDGetDebugString");
		if (debugString != 0)
			debugConnected = true;
	}
	if (!debugConnected)
	{
		debugString = &oapiDebugString;
	}

	//
	// Register visible connectors.
	//
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &MFDToPanelConnector);
	RegisterConnector(0, &CSMToLEMConnector);
	RegisterConnector(0, &CSMToSIVBConnector);
}

Saturn::~Saturn()

{
	TRACESETUP("~Saturn");

	if (LMPad) {
		delete[] LMPad;
		LMPad = 0;
	}

	// Release DirectX joystick stuff
	if(js_enabled > 0){
		// Release joysticks
		while(js_enabled > 0){
			js_enabled--;
			dx8_joystick[js_enabled]->Unacquire();
			dx8_joystick[js_enabled]->Release();
		}
		dx8ppv->Release();
		dx8ppv = NULL;
	}

	//fclose(PanelsdkLogFile);
}

void Saturn::initSaturn()

{
	//
	// Default to the latest model Saturn
	//

	VehicleNo = 600;
	ApolloNo = 0;
	TCPO = 0.0;
	SaturnType = SAT_UNKNOWN;

	StagesString[0] = 0;

	FirstTimestep = true;
	GenericFirstTimestep = true;

	InterstageAttached = true;
	LESAttached = true;
	ApexCoverAttached = true;
	ChutesAttached = true;
	CSMAttached = true;

	NosecapAttached = false;

	TLICapableBooster = false;
	TLISoundsLoaded = false;

	//
	// Do we have the Skylab-type SM and CM?
	//
	SkylabSM = false;
	SkylabCM = false;

	//
	// Do we have an HGA? This is a 'negative' variable for backward
	// compatibility with old scenarios... otherwise it would default
	// to having no HGA when the state was read from those files.
	//
	NoHGA = false;

	//
	// Or the S1b panel with 8 engine lights?
	//
	S1bPanel = false;

	// DS20070204 LVDC++ mode is off by default
	use_lvdc = false;

	bAbort = false;
	ABORT_IND = false;
	LEM_DISPLAY=false;
	ASTPMission = false;

	AutoSlow = false;
	Crewed = true;
	SIVBPayload = PAYLOAD_LEM;
	CSMSepSet = false;
	CSMSepTime = 0.0;

	CMSepSet = false;
	CMSepTime = 0.0;

	CSMAccelSet = false;
	CSMAccelTime = 0.0;
	CSMAccelEnd = 0.0;
	CSMAccelPitch = 0.0;

	PayloadDeploySet = false;
	PayloadDeployTime = 0.0;

	SIVBBurnStart = 0.0;
	SIVBApogee = 0.0;

	CSMBurn = false;
	CSMBurnStart = 0.0;
	CSMApogee = 0.0;
	CSMPerigee = 0.0;

	PayloadDataTransfer = false;
	PostSplashdownPlayed = false;
	SplashdownPlayed = false;

	DeleteLaunchSite = true;

	buildstatus = 6;

	ThrustAdjust = 1.0;
	MixtureRatio = 5.5;
	SIVBCutoffTime = -MINUS_INFINITY;
	J2IsActive = true;

	DockAngle = 0;
	SeparationSpeed = 0;

	AtempP  = 0;
	AtempY  = 0;
	AtempR  = 0;

	StopRot = false;
	IGMEnabled = false;
	HasProbe = false;

	LowRes = false;

	SLARotationLimit = 45;
	SLAWillSeparate = true;

	hStage1Mesh = 0;
	hStage2Mesh = 0;
	hStage3Mesh = 0;
	hInterstageMesh = 0;
	hStageSLA1Mesh = 0;
	hStageSLA2Mesh = 0;
	hStageSLA3Mesh = 0;
	hStageSLA4Mesh = 0;
	SPSidx = 0;

	refSaturn1B = 0;
	refPREV = 0;
	AltitudePREV = 0;

	hstg1 = 0;
	hstg2 = 0;
	hintstg = 0;
	hesc1 = 0;
	hPROBE = 0;
	hs4bM = 0;
	hs4b1 = 0;
	hs4b2 = 0;
	hs4b3 = 0;
	hs4b4 = 0;
	habort = 0;
	hSMJet = 0;
	hLMV = 0;
	hVAB = 0;
	hML = 0;
	hCrawler = 0;
	hMSS = 0;
	hApex = 0;
	hDrogueChute = 0;
	hMainChute = 0;
	hOpticsCover = 0;

	//
	// Apollo 13 flags.
	//

	ApolloExploded = false;
	CryoStir = false;
	KranzPlayed = false;

	//
	// LM PAD data.
	//

	LMPadCount = 0;
	LMPad = 0;
	LMPadLoadCount = 0;
	LMPadValueCount = 0;

	//
	// Default mission time to an hour prior to launch.
	//

	MissionTime = (-3600);
	NextMissionEventTime = 0;
	LastMissionEventTime = 0;

	//
	// No point trying to destroy things if we haven't launched.
	//

	NextDestroyCheckTime = 0;
	NextFailureTime = MINUS_INFINITY;

	//
	// Panel flash.
	//

	NextFlashUpdate = MINUS_INFINITY;
	PanelFlashOn = false;

	//
	// Defaults.
	//

	agc.SetDesiredApogee(220);
	agc.SetDesiredPerigee(215);
	agc.SetDesiredAzimuth(45);

	IGMStartTime = 204.1 ;

	//
	// Typical center engine shutdown times.
	//

	FirstStageCentreShutdownTime = 135.0;
	SecondStageCentreShutdownTime = 460.0;

	//
	// Stage shutdown times greater than stage burn times, to guarantee we burn all fuel.
	//

	FirstStageShutdownTime = 250.0;
	SecondStageShutdownTime = 1000.0;

	//
	// Same for interstage and LES jettison.
	//

	InterstageSepTime = 1000.0;
	LESJettisonTime = 1000.0;

	//
	// PU shift time. Default to 8:15
	//

	SecondStagePUShiftTime = 495.0;

	//
	// Failure modes.
	//

	LandFail.word = 0;
	LaunchFail.word = 0;
	SwitchFail.word = 0;

	//
	// Configure AGC and DSKY.
	//

	agc.ControlVessel(this);
	imu.SetVessel(this, false);
	dsky.Init(&LightingNumIntLMDCCB, &NumericRotarySwitch);
	dsky2.Init(&LightingNumIntLEBCB, &Panel100NumericRotarySwitch);

	//
	// Configure SECS.
	//

	secs.ControlVessel(this);
	els.ControlVessel(this);

	//
	// Wire up timers.
	//

	MissionTimerDisplay.WireTo(&GaugePower);
	EventTimerDisplay.WireTo(&GaugePower);

	//
	// Configure connectors.
	//

	iuCommandConnector.SetSaturn(this);
	sivbCommandConnector.SetSaturn(this);

	CSMToSIVBConnector.SetType(CSM_SIVB_DOCKING);
	SIVBToCSMPowerConnector.SetType(CSM_SIVB_POWER);

	CSMToLEMConnector.SetType(CSM_LEM_DOCKING);
	CSMToLEMPowerConnector.SetType(LEM_CSM_POWER);
	CSMToLEMPowerConnector.SetPowerDrain(&CSMToLEMPowerDrain);

	SIVBToCSMPowerSource.SetConnector(&SIVBToCSMPowerConnector);

	//
	// Propellant sources.
	//

	SMQuadARCS.SetVessel(this);
	SMQuadBRCS.SetVessel(this);
	SMQuadCRCS.SetVessel(this);
	SMQuadDRCS.SetVessel(this);
	CMRCS1.SetVessel(this);
	CMRCS2.SetVessel(this);
	SPSPropellant.SetVessel(this);

	//
	// Default masses.
	//

	CM_EmptyMass = 5430;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
	CM_FuelMass =  CM_RCS_FUEL_PER_TANK * 2.;	// The CM has 2 tanks

	SM_EmptyMass = 4100;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
	SM_FuelMass = SPS_DEFAULT_PROPELLANT;

	S4PL_Mass = 15094;							// LM mass is default (Apollo by the numbers)
												// \todo Apollo 15-17 LMs have about 16440 kg

	Abort_Mass = 4050;

	SI_MassLoaded = false;
	SII_MassLoaded = false;
	S4B_MassLoaded = false;

	S1_ThrustLoaded = false;
	S2_ThrustLoaded = false;
	S3_ThrustLoaded = false;

	//
	// Set defaults so the save code doesn't explode if
	// we don't set them later.
	//

	SI_EngineNum = 0;
	SII_EngineNum = 0;
	SIII_EngineNum = 0;

	THRUST_FIRST_VAC = 0.0;
	THRUST_SECOND_VAC = 0.0;
	THRUST_THIRD_VAC = 0.0;

	ISP_SECOND_VAC = 0.0;
	ISP_SECOND_SL = 0.0;
	ISP_THIRD_VAC = 0.0;

	//
	// LET setup.
	//

	//
	// ISPs are estimates.
	//

	ISP_LET_SL   = 2200.0;
	ISP_LET_VAC  = 2600.0;

	//
	// I'm not sure whether the thrust values quoted are for sea level
	// or vacuum. If they're sea-level then we should multiply them by
	// (ISP_VAC / ISP_SL) to get vacuum thrust.
	//

	THRUST_VAC_LET  = (653888.6 / 4.0);

	//
	// Propellant handles.
	//

	ph_1st = 0;
	ph_2nd = 0;
	ph_3rd = 0;
	ph_rcs0 = 0;
	ph_rcs1 = 0;
	ph_rcs2 = 0;
	ph_rcs3 = 0;
	ph_rcs_cm_1 = 0;
	ph_rcs_cm_2 = 0;
	ph_sps = 0;
	ph_let = 0;
	ph_sep = 0;
	ph_sep2 = 0;
	ph_o2_vent = 0;
	ph_ullage1 = 0;
	ph_ullage2 = 0;
	ph_ullage3 = 0;

	//
	// Thruster groups.
	//

	thg_main = 0;
	thg_let = 0;
	thg_ull = 0;
	thg_ver = 0;
	thg_retro1 = 0;
	thg_retro2 = 0;
	thg_aps = 0;
	th_o2_vent = 0;

	//
	// Particle streams
	//
	int i;

	dyemarker = NULL;
	wastewaterdump = NULL;
	urinedump = NULL;

	for (i = 0; i < 12; i++) {
		cmrcsdump[i] = NULL;
	}
	for (i = 0; i < 3; i++) {
		prelaunchvent[i] = NULL;
	}
	for (i = 0; i < 8; i++) {
		stagingvent[i] = NULL;
	}
	for (i = 0; i < 8; i++) {
		contrail[i] = NULL;
	}
	contrailLevel = 0;

	//
	// Random virtual cockpit motion.
	//

	ViewOffsetx = 0;
	ViewOffsety = 0;
	ViewOffsetz = 0;

	InVC = false;
	InPanel = false;
	CheckPanelIdInTimestep = false;
	RefreshPanelIdInTimestep = false;
	FovFixed = false;
	FovExternal = 0;
	FovSave = 0;
	FovSaveExternal = 0;

	//
	// Save the last view offset set.
	//

	CurrentViewOffset = 0;

	//
	// Zeroing arrays for safety.
	//

	for (i = 1; i < 6; i++){
		ClearEngineIndicator(i);
	}

	ClearLVGuidLight();
	ClearLVRateLight();
	ClearLiftoffLight();

	for (i = 0; i < 8; i++)
	{
		LAUNCHIND[i] = false;
	}

	for (i = 0; i < nsurf; i++)
	{
		srf[i] = 0;
	}

	for (i = 0; i < 5; i++)
	{
		th_main[i] = 0;
	}

	for (i = 0; i < 4; i++)
	{
		th_let[i] = 0;
	}

	for (i = 0; i < 8; i++) {
		th_ull[i] = 0;
		th_sep[i] = 0;
		th_sep2[i] = 0;
		stagingvent[i] = NULL;
	}

	for (i = 0; i < 3; i++) {
		th_ver[i] = 0;
		th_aps[i] = 0;
	}

	//
	// SM RCS thrusters
	//

	for (i = 0; i < 8; i++) {
		th_rcs_a[i] = 0;
		th_rcs_b[i] = 0;
		th_rcs_c[i] = 0;
		th_rcs_d[i] = 0;
	}

	for (i = 0; i < 24; i++) {
		th_att_rot[i] = 0;
		th_att_lin[i] = 0;
	}

	//
	// CM RCS thrusters
	//

	for (i = 0; i < 12; i++) {
		th_att_cm[i] = 0;
		th_att_cm_commanded[i] = false;
	}

	for (i = 0; i < 6; i++) {
		th_att_cm_sys1[i] = 0;
		th_att_cm_sys2[i] = 0;
	}


	for (i = 0; i < 8; i++)
		ENGIND[i] = false;

	PayloadName[0] = 0;
	LEMCheck[0] = 0;
	LEMCheckAuto = 0;
	LMDescentFuelMassKg = 8375.0;
	LMAscentFuelMassKg = 2345.0;

	UseATC = false;

	SIISepState = false;
	bRecovery = false;

	stage = 0;

	KEY1=false;
	KEY2=false;
	KEY3=false;
	KEY4=false;
	KEY5=false;
	KEY6=false;
	KEY7=false;
	KEY8=false;
	KEY9=false;

	actualVEL = 0;
	actualALT = 0;
	actualFUEL = 0;
	aVAcc = 0;
	aVSpeed = 0;
	aHAcc = 0;
	aZAcc = 0;

	for (i = 0; i < LASTVELOCITYCOUNT; i++) {
		LastVelocity[i] = _V(0, 0, 0);
		LastVerticalVelocity[i] = 0;
		LastSimt[i] = 0;
	}
	LastVelocityFilled = -1;

	viewpos = SATVIEW_LEFTSEAT;

	probeidx = -1;
	probeextidx = -1;
	crewidx = -1;
	cmpidx = -1;
	sidehatchidx = -1;
	sidehatchopenidx = -1;
	sidehatchburnedidx = -1;
	sidehatchburnedopenidx = -1;
	opticscoveridx = -1;

	Scorrec = false;

	//
	// VAGC Mode settings
	//

	VAGCChecklistAutoSlow = false;
	VAGCChecklistAutoEnabled = false;

	//
	// LM landing data.
	//

	LMLandingLatitude = 0;
	LMLandingLongitude = 0;
	LMLandingAltitude = 0;
	LMLandingBase[0] = 0;
	LMLandingMJD = 0;

	//
	// Earth landing data.
	//

	SplashdownLatitude = 0;
	SplashdownLongitude = 0;
	EntryInterfaceMJD = 0;

	//
	// Mission TLI and LOI parameters
	//

	TransLunarInjectionMJD = 0;			
	LunarOrbitInsertionMJD = 0;	
	FreeReturnPericynthionMJD = 0;		
	FreeReturnPericynthionAltitude = 0;	
	TransLunarInjectionOffsetLon = 0;	
	TransLunarInjectionOffsetLat = 0;	
	TransLunarInjectionOffsetRad = 0;	


	//
	// Load textures that are used a lot. Should these be global
	// variables?
	//

	SMExhaustTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_atrcs");
	J2Tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_j2");
	SIVBRCSTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");
	CMTex = oapiRegisterReentryTexture("reentry");

	strncpy(AudioLanguage, "English", 64);

	hEVA = 0;

	//
	// Timestep tracking for debugging.
	//

	LongestTimestep = 0;
	LongestTimestepLength = 0.0;
	CurrentTimestep = 0;

	// call only once 
	if (!InitSaturnCalled) {

		// PanelSDK pointers.
		ClearPanelSDKPointers();

		// Initialize the internal systems
		SystemsInit();

		// Initialize the panel
		fdaiDisabled = false;
		fdaiSmooth = false;
		hBmpFDAIRollIndicator = 0;

		PanelId = SATPANEL_MAIN; 		// default panel
		MainPanelSplit = false;
		GNSplit = false;
		InitSwitches();

		// "dummy" SetSwitches to enable the panel event handling
		SetSwitches(PanelId);

		// Switch to compatible dock mode 
		SetDockMode(0);
	}
	InitSaturnCalled = true;
}

void Saturn::clbkPostCreation() {

	//
	// Check propellants
	//

	SMQuadARCS.CheckPropellantMass();
	SMQuadBRCS.CheckPropellantMass();
	SMQuadCRCS.CheckPropellantMass();
	SMQuadDRCS.CheckPropellantMass();
	CMRCS1.CheckPropellantMass();
	CMRCS2.CheckPropellantMass();
	SPSPropellant.CheckPropellantMass();

	//
	// IMFD5 communication support
	//
	
	IMFD_Client.SetVessel(this);
	// Set constant configuration
	IMFDConstantConfiguration cc;
	strcpy(cc.LandingBase, LMLandingBase);
	strcpy(cc.TargetCelbody, "Moon");
	cc.AZIMUTCOR = agc.GetDesiredAzimuth() - 90.0;
	cc.MJDTLI = TransLunarInjectionMJD;
	cc.MJDLOI = LunarOrbitInsertionMJD;
	cc.MJDPEC = FreeReturnPericynthionMJD;
	cc.ALTPEC = FreeReturnPericynthionAltitude;
	cc.TLIOffsetLon = TransLunarInjectionOffsetLon; 
	cc.TLIOffsetLat = TransLunarInjectionOffsetLat; 
	cc.TLIOffsetRad = TransLunarInjectionOffsetRad; 
	cc.MJDLDG = LMLandingMJD;
	cc.MJDSPL = EntryInterfaceMJD;
	cc.SPL_lon = SplashdownLongitude;
	cc.SPL_lat = SplashdownLatitude;
	cc.ApolloP30Mode = true;
	IMFD_Client.SetConstantConfiguration(cc);

	// Connect to IMFD
	IMFD_Client.Connect();

	// Connect to the Checklist controller.
	checkControl.linktoVessel(this);
}

void Saturn::GetPayloadName(char *s)

{
	if (PayloadName[0]) {
		strcpy (s, PayloadName);
		return;
	}

	strcpy (s, GetName()); strcat (s, "-PAYLOAD");
}

void Saturn::GetApolloName(char *s)

{
	sprintf(s, "AS-%d", VehicleNo);
}

void Saturn::UpdateLaunchTime(double t)

{
	if (t < 0)
		return;

	if (MissionTime < 0) {
		MissionTime = (-t);
	}
}

//
// Pitch program.
//

double Saturn::GetCPitch(double t)
{
	int i = 1;

	//
	// Make sure we don't run off the end.
	//

	if (t>met[PITCH_TABLE_SIZE - 1]) return cpitch[PITCH_TABLE_SIZE - 1];

	//
	// Find the first MET that's greater than our current time.
	//

	while (met[i]<t) i++;

	//
	// And calculate pitch as appropriate between those two times.
	//

	return cpitch[i-1]+(cpitch[i]-cpitch[i-1])/(met[i]-met[i-1])*(t-met[i-1]);
}

double Saturn::SetPitchApo()

{
	double cpitch;
	double SatApo1;
	double DiffApo;

	OBJHANDLE hbody = GetGravityRef();
	double bradius = oapiGetSize(hbody);

	GetApDist(SatApo1);
	DiffApo = SatApo1 - (agc.GetDesiredApogee() * 1000. + bradius);
	cpitch = (GetPitch() * DEG) - aVAcc - (aVSpeed / 50.) + ((agc.GetDesiredApogee() * 1000. - GetAltitude()) / 8000.);


	if (cpitch > 35){
		cpitch=35;
	}
	else if(cpitch < -35){
		cpitch = -35;
	}

	//sprintf(oapiDebugString(), "diff %f", cpitch );
	return cpitch;
}

//
// Kill a vessel (typically a stage) based on its distance from us.
//

void Saturn::KillDist(OBJHANDLE &hvessel, double kill_dist)

{
	VECTOR3 posr  = {0,0,0};
	double dist = 0.0;

	oapiGetRelativePos (GetHandle() ,hvessel, &posr);
	dist = (posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);

	if (dist > (kill_dist * kill_dist)){
		oapiDeleteVessel(hvessel, GetHandle());
		hvessel = 0;
	}
}

//
// Kill a vessel (typically a stage) based on its current altitude.
//

void Saturn::KillAlt(OBJHANDLE &hvessel, double altVS)

{
	double CurrentAlt;
	int getit = oapiGetAltitude(hvessel, &CurrentAlt);

	if ((CurrentAlt < altVS) && CurrentAlt >= 0 )
	{
		//
		// If this is the SM, we need to tidy up any seperated meshes before
		// we delete it. We don't want them hanging around forever.
		//
		// We have to delete the parts first and then wait until the next timestep
		// to delete the SM, otherwise Orbiter blows away.
		//
		if (hvessel == hSMJet)
		{
			SM *vsm = static_cast<SM *> (oapiGetVesselInterface(hvessel));
			if (vsm->TidyUpMeshes(GetHandle()))
				return;
		}

		oapiDeleteVessel(hvessel, GetHandle());
		hvessel = 0;
	}
}

void Saturn::LookForSIVb()

{
	if (!hs4bM)
	{
		char VName[256];
		char ApolloName[64];

		GetApolloName(ApolloName);

		strcpy (VName, ApolloName); strcat (VName, "-S4BSTG");
		hs4bM = oapiGetVesselByName(VName);
	}
}

void Saturn::LookForLEM()

{
	if (!hLMV)
	{
		char VName[256];
		GetPayloadName(VName);
		hLMV = oapiGetVesselByName(VName);
	}
}

void Saturn::clbkDockEvent(int dock, OBJHANDLE connected)

{
	//
	// Ensure the docking probe is updated first.
	//
	dockingprobe.DockEvent(dock, connected); 

	if (connected)
	{
		if (dockingprobe.IsHardDocked())
		{
			DockConnectors(dock);
		}
	}
	else
	{
		UndockConnectors(dock);
	}
}

void Saturn::HaveHardDocked(int port)

{
	DockConnectors(port);
}

void Saturn::Undocking(int port)

{
	UndockConnectors(port);
}

void Saturn::clbkPreStep(double simt, double simdt, double mjd)

{
	char buffer[100];
	TRACESETUP("Saturn::clbkPreStep");
	sprintf(buffer, "MissionTime %f, simt %f, simdt %f, time(0) %lld", MissionTime, simt, simdt, time(0)); 
	TRACE(buffer);

	//
	// We die horribly if you set 100x or higher acceleration during launch.
	//

	if (stage >= LAUNCH_STAGE_ONE && stage <= LAUNCH_STAGE_SIVB &&
		oapiGetTimeAcceleration() > 10.0) {
		oapiSetTimeAcceleration(10.0);
	}

	//
	// You'll also die horribly if you set time acceleration at all in the
	// early parts of the launch.
	//

	if (stage == LAUNCH_STAGE_ONE && MissionTime < 50 &&
		oapiGetTimeAcceleration() > 1.0) {
		oapiSetTimeAcceleration(1.0);
	}

	//
	// Change to desired panel if necessary
	//

	if (CheckPanelIdInTimestep) {
		oapiSetPanel(PanelId);
		CheckPanelIdInTimestep = false;
	}

	if (RefreshPanelIdInTimestep && oapiCameraInternal()) {
		oapiSetPanel(PanelId);
		RefreshPanelIdInTimestep = false;
	}

	//
	// Check FOV for external view
	//

	// The current FOV hack doesn't work in external view, 
	// so we need to switch back to internal view for one
	// time step and switch then back outside again.
	if (FovExternal == 3) {
		oapiCameraAttach(oapiCameraTarget(), 1);
		FovExternal = 4;
	}
	if (FovExternal == 2) {
		papiCameraSetAperture(FovSave);
		FovExternal = 3;
	}
	if (FovExternal == 1) {
		oapiCameraAttach(oapiCameraTarget(), 0);
		FovExternal = 2;
	}
	if ((!oapiCameraInternal() || (oapiGetFocusInterface() != this)) && FovFixed && FovExternal == 0) {
		FovSaveExternal = papiCameraAperture();
		FovExternal = 1;
	}

	if ((oapiGetFocusInterface() == this) && oapiCameraInternal() && FovFixed && FovExternal == 4) {
		FovSave = papiCameraAperture();
		papiCameraSetAperture(FovSaveExternal);
		FovExternal = 0;	
	}

	//
	// dV because of staging
	//

	if (SeparationSpeed > 0) {
		// For unknown reasons we need twice the force. 
		// This may be related to the staging event.
		double F = 2. * GetMass() * SeparationSpeed / simdt;
		AddForce(_V(0, 0, F), _V(0,0,0));
		SeparationSpeed = 0;
	}

	//
	// Subclass specific handling
	//

	Timestep(simt, simdt, mjd);

	sprintf(buffer, "End time(0) %lld", time(0)); 
	TRACE(buffer);
}

void Saturn::clbkPostStep (double simt, double simdt, double mjd)

{
	char buffer[100];
	TRACESETUP("Saturn::clbkPostStep");
	sprintf(buffer, "MissionTime %f, simt %f, simdt %f, time(0) %lld", MissionTime, simt, simdt, time(0)); 
	TRACE(buffer);

	if (debugConnected == false)
	{
		sprintf(debugString(), "Please enable the Project Apollo MFD on the modules tab of the launchpad.");
		debugConnected = true;
	}
	if (stage >= PRELAUNCH_STAGE && !GenericFirstTimestep) {

		//
		// The SPS engine must be in post time step 
		// to inhibit Orbiter's thrust control
		//
		
		SPSEngine.Timestep(MissionTime, simdt);

		// Better acceleration measurement stability
		imu.Timestep(simdt);
		ems.TimeStep(MissionTime, simdt);
		CrewStatus.Timestep(simdt);

		iu.PostStep(simt, simdt, mjd);
	}
	// Order is important, otherwise delayed springloaded switches are reset immediately
	MainPanel.timestep(MissionTime);
	checkControl.timestep(MissionTime,eventControl);

	sprintf(buffer, "End time(0) %lld", time(0)); 
	TRACE(buffer);
}

void Saturn::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	int i = 1;
	char str[256];

	oapiWriteScenario_int (scn, "NASSPVER", NASSP_VERSION);
	oapiWriteScenario_int (scn, "STAGE", stage);
	oapiWriteScenario_int(scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "APOLLONO", ApolloNo);
	oapiWriteScenario_int (scn, "SATTYPE", SaturnType);
	oapiWriteScenario_int (scn, "PANEL_ID", PanelId);
	papiWriteScenario_double (scn, "TCP", TCPO);
	papiWriteScenario_double (scn, "MISSNTIME", MissionTime);
	papiWriteScenario_double (scn, "NMISSNTIME", NextMissionEventTime);
	papiWriteScenario_double (scn, "LMISSNTIME", LastMissionEventTime);
	papiWriteScenario_double (scn, "NFAILTIME", NextFailureTime);
	papiWriteScenario_double (scn, "MTD", MissionTimerDisplay.GetTime());
	papiWriteScenario_double (scn, "ETD", EventTimerDisplay.GetTime());
	papiWriteScenario_double (scn, "THRUSTA", ThrustAdjust);
	papiWriteScenario_double (scn, "MR", MixtureRatio);
	papiWriteScenario_double (scn, "SIVBCUTOFFTIME", SIVBCutoffTime);
	papiWriteScenario_bool (scn, "J2ISACTIVE", J2IsActive);

//	oapiWriteScenario_string (scn, "STAGECONFIG", StagesString);

	oapiWriteScenario_int (scn, "DLS", DeleteLaunchSite ? 1 : 0);

	if (buildstatus < 6) {
		oapiWriteScenario_int (scn, "BUILDSTATUS", buildstatus);
	}

	oapiWriteScenario_int(scn, "STAGESTATUS", StageState);

	if (LEM_DISPLAY) {
		oapiWriteScenario_int (scn, "LEM_DISPLAY", int(LEM_DISPLAY));
	}

	oapiWriteScenario_int (scn, "MAINSTATE",  GetMainState());
	oapiWriteScenario_int (scn, "ATTACHSTATE",  GetAttachState());
	oapiWriteScenario_int (scn, "LIGHTSTATE",  GetLightState());

	//
	// Save vessel-specific stats.
	//

	SaveVehicleStats(scn);

	if (stage < LAUNCH_STAGE_ONE)
		oapiWriteScenario_int (scn, "PRELAUNCHATC",  int(UseATC));

	if (stage < LAUNCH_STAGE_TWO) {
		papiWriteScenario_double (scn, "SICSHUT", FirstStageCentreShutdownTime);
		papiWriteScenario_double (scn, "SISHUT", FirstStageShutdownTime);
		papiWriteScenario_double (scn, "T1V", THRUST_FIRST_VAC);
		papiWriteScenario_double (scn, "I1S", ISP_FIRST_SL);
		papiWriteScenario_double (scn, "I1V", ISP_FIRST_VAC);
		oapiWriteScenario_int (scn, "SIENG", SI_EngineNum);
	}

	if (stage < STAGE_ORBIT_SIVB) {
		char fname[64];

		papiWriteScenario_double (scn, "SIICSHUT", SecondStageCentreShutdownTime);
		papiWriteScenario_double (scn, "SIIPUT", SecondStagePUShiftTime);
		papiWriteScenario_double (scn, "SIISHUT", SecondStageShutdownTime);
		papiWriteScenario_double (scn, "T2V", THRUST_SECOND_VAC);
		papiWriteScenario_double (scn, "I2S", ISP_SECOND_SL);
		papiWriteScenario_double (scn, "I2V", ISP_SECOND_VAC);
		papiWriteScenario_double (scn, "T3V", THRUST_THIRD_VAC);
		papiWriteScenario_double (scn, "I3V", ISP_THIRD_VAC);
		papiWriteScenario_double (scn, "ISTGJT", InterstageSepTime);
		papiWriteScenario_double (scn, "LESJT", LESJettisonTime);
		oapiWriteScenario_int (scn, "SIIENG", SII_EngineNum);

		//
		// Save pitch program.
		//

		for (i = 0; i < PITCH_TABLE_SIZE; i++) {
			sprintf(fname, "PMET%03d", i);
			papiWriteScenario_double (scn, fname, met[i]);
		}

		for (i = 0; i < PITCH_TABLE_SIZE; i++) {
			sprintf(fname, "CPITCH%03d", i);
			papiWriteScenario_double (scn, fname, cpitch[i]);
		}

		//
		// IGM start time.
		//

		papiWriteScenario_double (scn, "IGMST", IGMStartTime);
	}

	if (stage < CSM_LEM_STAGE) {
		oapiWriteScenario_int (scn, "SIIIENG", SIII_EngineNum);
		oapiWriteScenario_int (scn, "LAUNCHSTATE", GetLaunchState());
		oapiWriteScenario_int (scn, "SLASTATE", GetSLAState());
	}

	if (stage < CM_STAGE) {
		papiWriteScenario_double (scn, "SMFUELLOAD", SM_FuelMass);
		papiWriteScenario_double (scn, "SMMASS", SM_EmptyMass);
	}

	papiWriteScenario_double (scn, "CMFUELLOAD", CM_FuelMass);
	papiWriteScenario_double (scn, "CMMASS", CM_EmptyMass);

	papiWriteScenario_double (scn, "MOONLAT", LMLandingLatitude);
	papiWriteScenario_double (scn, "MOONLONG", LMLandingLongitude);
	papiWriteScenario_double (scn, "MOONALT", LMLandingAltitude);
	papiWriteScenario_double (scn, "MOONMJD", LMLandingMJD);	
	if (LMLandingBase[0])
		oapiWriteScenario_string (scn, "MOONBASE", LMLandingBase);

	if (!PayloadDataTransfer) {
		if (LMPadCount > 0) {
			oapiWriteScenario_int (scn, "LMPADCNT", LMPadCount);
			for (i = 0; i < LMPadCount; i++) {
				sprintf(str, "%04o %05o", LMPad[i * 2], LMPad[i * 2 + 1]);
				oapiWriteScenario_string (scn, "LMPAD", str);
			}
		}
	}

	papiWriteScenario_double (scn, "SPLASHLAT", SplashdownLatitude);
	papiWriteScenario_double (scn, "SPLASHLONG", SplashdownLongitude);
	papiWriteScenario_double (scn, "EARTHEIMJD", EntryInterfaceMJD);

	papiWriteScenario_double (scn, "TLIMJD", TransLunarInjectionMJD);
	papiWriteScenario_double (scn, "LOIMJD", LunarOrbitInsertionMJD);
	papiWriteScenario_double (scn, "FREERETURNPECMJD", FreeReturnPericynthionMJD);
	papiWriteScenario_double (scn, "FREERETURNPECALT", FreeReturnPericynthionAltitude);
	papiWriteScenario_double (scn, "TLIOFFSETLON", TransLunarInjectionOffsetLon);
	papiWriteScenario_double (scn, "TLIOFFSETLAT", TransLunarInjectionOffsetLat);
	papiWriteScenario_double (scn, "TLIOFFSETRAD", TransLunarInjectionOffsetRad);

	if (!Crewed) {
		oapiWriteScenario_int (scn, "UNMANNED", 1);

		if (CSMSepSet && (CSMSepTime >= MissionTime))
		{
			papiWriteScenario_double (scn, "CSMSEP", CSMSepTime);
		}

		if (CMSepSet && (CMSepTime >= MissionTime))
		{
			papiWriteScenario_double (scn, "CMSEP", CMSepTime);
		}

		if (PayloadDeploySet && (PayloadDeployTime >= MissionTime))
		{
			papiWriteScenario_double (scn, "PLSEP", PayloadDeployTime);
		}

		if (stage <= CSM_LEM_STAGE) {
			papiWriteScenario_double (scn, "S4APO", SIVBApogee);
			papiWriteScenario_double (scn, "S4BURN", SIVBBurnStart);
		}

		if (CSMBurn && (stage <= CM_STAGE)) {
			papiWriteScenario_double (scn, "CSMAPO", CSMApogee);
			papiWriteScenario_double (scn, "CSMPER", CSMPerigee);
			papiWriteScenario_double (scn, "CSMBURN", CSMBurnStart);
		}

		if (CSMAccelSet && (stage <= CM_STAGE))
		{
			papiWriteScenario_double (scn, "CSMACCEL", CSMAccelTime);
			papiWriteScenario_double (scn, "CSMACCEND", CSMAccelEnd);
			papiWriteScenario_double (scn, "CSMACCPITCH", CSMAccelPitch);
		}
	}

	if (AutoSlow) {
		oapiWriteScenario_int (scn, "AUTOSLOW", 1);
	}
	if (LandFail.word) {
		oapiWriteScenario_int (scn, "LANDFAIL", LandFail.word);
	}
	if (LaunchFail.word) {
		oapiWriteScenario_int (scn, "LAUNCHFAIL", LaunchFail.word);
	}
	if (SwitchFail.word) {
		oapiWriteScenario_int (scn, "SWITCHFAIL", SwitchFail.word);
	}
	if (ApolloNo == 13) {
		oapiWriteScenario_int (scn, "A13STATE", GetA13State());
	}
	if (SIVBPayload != PAYLOAD_LEM) {
		oapiWriteScenario_int (scn, "S4PL", SIVBPayload);
	}
	if (use_lvdc){oapiWriteScenario_string(scn,"USE_LVDC","PLEASE");};
	oapiWriteScenario_string (scn, "LANG", AudioLanguage);
	
	if (PayloadName[0])
		oapiWriteScenario_string (scn, "PAYN", PayloadName);

	if (!PayloadDataTransfer) {
		if (LEMCheck[0]) {
			oapiWriteScenario_string(scn, "LEMCHECK", LEMCheck);
			oapiWriteScenario_int(scn, "LEMCHECKAUTO", int(LEMCheckAuto));
		}
		oapiWriteScenario_float (scn, "LMDSCFUEL", LMDescentFuelMassKg);
		oapiWriteScenario_float (scn, "LMASCFUEL", LMAscentFuelMassKg);
	}
	oapiWriteScenario_int (scn, "COASENABLED", coasEnabled);
	oapiWriteScenario_int (scn, "ORDEALENABLED", ordealEnabled);
	oapiWriteScenario_int (scn, "OPTICSDSKYENABLED", opticsDskyEnabled);
	oapiWriteScenario_int (scn, "HATCHPANEL600ENABLED", hatchPanel600EnabledLeft);
	oapiWriteScenario_int (scn, "PANEL382ENABLED", panel382Enabled);
	oapiWriteScenario_int (scn, "FOVFIXED", (FovFixed ? 1 : 0));
	oapiWriteScenario_int (scn, "FOVEXTERNAL", FovExternal);
	papiWriteScenario_double(scn, "FOVSAVE", FovSave);
	papiWriteScenario_double(scn, "FOVSAVEEXTERNAL", FovSaveExternal);

	dsky.SaveState(scn, DSKY_START_STRING, DSKY_END_STRING);
	dsky2.SaveState(scn, DSKY2_START_STRING, DSKY2_END_STRING);
	agc.SaveState(scn);
	imu.SaveState(scn);
	cws.SaveState(scn);
	secs.SaveState(scn);
	els.SaveState(scn);

	//
	// If we've seperated from the SIVb, the IU is history.
	//
	if (stage < CSM_LEM_STAGE)
	{
		iu.SaveState(scn);
	}

	gdc.SaveState(scn);
	rjec.SaveState(scn);
	ascp.SaveState(scn);
	ems.SaveState(scn);
	ordeal.SaveState(scn);
	dockingprobe.SaveState(scn);
	SPSPropellant.SaveState(scn);
	SPSEngine.SaveState(scn);
	optics.SaveState(scn);
	
	oapiWriteLine(scn, SMRCSPROPELLANT_A_START_STRING);
	SMQuadARCS.SaveState(scn);

	oapiWriteLine(scn, SMRCSPROPELLANT_B_START_STRING);
	SMQuadBRCS.SaveState(scn);

	oapiWriteLine(scn, SMRCSPROPELLANT_C_START_STRING);
	SMQuadCRCS.SaveState(scn);

	oapiWriteLine(scn, SMRCSPROPELLANT_D_START_STRING);
	SMQuadDRCS.SaveState(scn);

	oapiWriteLine(scn, CMRCSPROPELLANT_1_START_STRING);
	CMRCS1.SaveState(scn);

	oapiWriteLine(scn, CMRCSPROPELLANT_2_START_STRING);
	CMRCS2.SaveState(scn);

	oapiWriteLine(scn, SPSGIMBALACTUATOR_PITCH_START_STRING);
	SPSEngine.pitchGimbalActuator.SaveState(scn);

	oapiWriteLine(scn, SPSGIMBALACTUATOR_YAW_START_STRING);
	SPSEngine.yawGimbalActuator.SaveState(scn);
	
	fdaiLeft.SaveState(scn, FDAI_START_STRING, FDAI_END_STRING);
	fdaiRight.SaveState(scn, FDAI2_START_STRING, FDAI2_END_STRING);

	oapiWriteLine(scn, BMAG1_START_STRING);
	bmag1.SaveState(scn);

	oapiWriteLine(scn, BMAG2_START_STRING);
	bmag2.SaveState(scn);
	SaveLVDC(scn);
	mcc.SaveState(scn);
	mcc.rtcc->SaveState(scn);

	//
	// This has to be after the AGC otherwise the AGC state will override it.
	// Both should be saving the same information, but this is human-readable
	// and the AGC data isn't.
	//

	if (stage < STAGE_ORBIT_SIVB)
	{
		papiWriteScenario_double (scn, "TOAPO", agc.GetDesiredApogee());
		papiWriteScenario_double (scn, "TOPER", agc.GetDesiredPerigee());
		papiWriteScenario_double (scn, "TOHDG", agc.GetDesiredAzimuth());
		papiWriteScenario_double (scn, "TOINCLINATION", agc.GetDesiredInclination());
	}

	// save the internal systems 
	oapiWriteScenario_int(scn, "SYSTEMSSTATE", systemsState);
	papiWriteScenario_double(scn, "LSYSTEMSMISSNTIME", lastSystemsMissionTime);

	CabinPressureRegulator.SaveState(scn);
	O2DemandRegulator.SaveState(scn);
	CabinPressureReliefValve1.SaveState(1, scn);
	CabinPressureReliefValve2.SaveState(2, scn);
	O2SMSupply.SaveState(scn);
	CrewStatus.SaveState(scn);
	SideHatch.SaveState(scn);
	usb.SaveState(scn);
	hga.SaveState(scn);
	dataRecorder.SaveState(scn);

	Panelsdk.Save(scn);	

	// save the state of the switches
	PSH.SaveState(scn);	

	//
	// This is now controlled by the launch pad configurator
	//

	// oapiWriteScenario_int (scn, "FDAIDISABLED", fdaiDisabled);
	// oapiWriteScenario_int (scn, "FDAISMOOTH", fdaiSmooth);
	// oapiWriteScenario_int (scn, "MAINPANELSPLIT", MainPanelSplit);
	// oapiWriteScenario_int (scn, "LOWRES", LowRes ? 1 : 0);

	checkControl.save(scn);
	eventControl.save(scn);
}

//
// Scenario state functions.
//

int Saturn::GetMainState()

{
	MainState state;

	state.MissionTimerRunning = MissionTimerDisplay.IsRunning();
	state.MissionTimerEnabled = MissionTimerDisplay.IsEnabled();
	state.EventTimerRunning = EventTimerDisplay.IsRunning();
	state.EventTimerEnabled = EventTimerDisplay.IsEnabled();
	state.EventTimerCountUp = EventTimerDisplay.GetCountUp();
	state.SIISepState = SIISepState;
	state.Scorrec = Scorrec;
	state.Burned = Burned;
	state.ABORT_IND = ABORT_IND;
	state.viewpos = viewpos;
	state.PayloadDataTransfer = PayloadDataTransfer;
	state.SplashdownPlayed = SplashdownPlayed;
	state.PostSplashdownPlayed = PostSplashdownPlayed;
	state.IGMEnabled = IGMEnabled;
	state.SkylabSM = SkylabSM;
	state.SkylabCM = SkylabCM;
	state.S1bPanel = S1bPanel;
	state.NoHGA = NoHGA;
	state.TLISoundsLoaded = TLISoundsLoaded;

	return state.word;
}

void Saturn::SetMainState(int s)

{
	MainState state;

	state.word = s;
	SIISepState = state.SIISepState;
	Scorrec = state.Scorrec;
	Burned = state.Burned;
	ABORT_IND = state.ABORT_IND;
	viewpos = state.viewpos;
	PayloadDataTransfer = (state.PayloadDataTransfer != 0);
	SplashdownPlayed = (state.SplashdownPlayed != 0);
	PostSplashdownPlayed = (state.PostSplashdownPlayed != 0);
	IGMEnabled = (state.IGMEnabled != 0);
	MissionTimerDisplay.SetRunning(state.MissionTimerRunning != 0);
	MissionTimerDisplay.SetEnabled(state.MissionTimerEnabled != 0);
	EventTimerDisplay.SetRunning(state.EventTimerRunning != 0);
	EventTimerDisplay.SetEnabled(state.EventTimerEnabled != 0);
	EventTimerDisplay.SetCountUp(state.EventTimerCountUp);
	SkylabSM = (state.SkylabSM != 0);
	SkylabCM = (state.SkylabCM != 0);
	S1bPanel = (state.S1bPanel != 0);
	NoHGA = (state.NoHGA != 0);
	TLISoundsLoaded = (state.TLISoundsLoaded != 0);
}

int Saturn::GetSLAState()

{
	SLAState state;

	state.SLARotationLimit = SLARotationLimit;
	state.SLAWillSeparate = SLAWillSeparate;

	return state.word;
}

void Saturn::SetSLAState(int s)

{
	SLAState state;

	state.word = s;
	SLARotationLimit = state.SLARotationLimit;
	SLAWillSeparate = state.SLAWillSeparate;
}

int Saturn::GetAttachState()

{
	AttachState state;

	state.CSMAttached = CSMAttached;
	state.NosecapAttached = NosecapAttached;
	state.InterstageAttached = InterstageAttached;
	state.LESAttached = LESAttached;
	state.HasProbe = HasProbe;
	state.ApexCoverAttached = ApexCoverAttached;
	state.ChutesAttached = ChutesAttached;

	return state.word;
}

void Saturn::SetAttachState(int s)

{
	AttachState state;

	state.word = s;

	CSMAttached = (state.CSMAttached != 0);
	NosecapAttached = (state.NosecapAttached != 0);
	LESAttached = (state.LESAttached != 0);
	InterstageAttached = (state.InterstageAttached != 0);
	HasProbe = (state.HasProbe != 0);
	ApexCoverAttached = (state.ApexCoverAttached != 0);
	ChutesAttached = (state.ChutesAttached != 0);
}

int Saturn::GetA13State()

{
	A13State state;

	state.ApolloExploded = ApolloExploded;
	state.CryoStir = CryoStir;
	state.KranzPlayed = KranzPlayed;

	return state.word;
}

void Saturn::SetA13State(int s)

{
	A13State state;

	state.word = s;
	ApolloExploded = (state.ApolloExploded != 0);
	CryoStir = (state.CryoStir != 0);
	KranzPlayed = (state.KranzPlayed != 0);
}

int Saturn::GetLaunchState()

{
	LaunchState state;

	state.autopilot = autopilot;

	return state.word;
}

void Saturn::SetLaunchState(int s)

{
	LaunchState state;

	state.word = s;
	autopilot = (state.autopilot != 0);
}

int Saturn::GetLightState()

{
	LightState state;

	state.Engind0 = ENGIND[0];
	state.Engind1 = ENGIND[1];
	state.Engind2 = ENGIND[2];
	state.Engind3 = ENGIND[3];
	state.Engind4 = ENGIND[4];
	state.Engind5 = ENGIND[5];
	state.Engind6 = ENGIND[6];
	state.Engind7 = ENGIND[7];
	state.Engind8 = ENGIND[8];
	state.LVGuidLight = LVGuidLight;
	state.Launchind0 = LAUNCHIND[0];
	state.Launchind1 = LAUNCHIND[1];
	state.Launchind2 = LAUNCHIND[2];
	state.Launchind3 = LAUNCHIND[3];
	state.Launchind4 = LAUNCHIND[4];
	state.Launchind5 = LAUNCHIND[5];
	state.Launchind6 = LAUNCHIND[6];
	state.Launchind7 = LAUNCHIND[7];
	state.LVRateLight = LVRateLight;

	return state.word;
}

void Saturn::SetLightState(int s)

{
	LightState state;

	state.word = s;
	ENGIND[0] = (state.Engind0 != 0);
	ENGIND[1] = (state.Engind1 != 0);
	ENGIND[2] = (state.Engind2 != 0);
	ENGIND[3] = (state.Engind3 != 0);
	ENGIND[4] = (state.Engind4 != 0);
	ENGIND[5] = (state.Engind5 != 0);
	ENGIND[6] = (state.Engind6 != 0);
	ENGIND[7] = (state.Engind7 != 0);
	ENGIND[8] = (state.Engind8 != 0);
	LVGuidLight = (state.LVGuidLight != 0);
	LAUNCHIND[0] = (state.Launchind0 != 0);
	LAUNCHIND[1] = (state.Launchind1 != 0);
	LAUNCHIND[2] = (state.Launchind2 != 0);
	LAUNCHIND[3] = (state.Launchind3 != 0);
	LAUNCHIND[4] = (state.Launchind4 != 0);
	LAUNCHIND[5] = (state.Launchind5 != 0);
	LAUNCHIND[6] = (state.Launchind6 != 0);
	LAUNCHIND[7] = (state.Launchind7 != 0);
	LVRateLight = (state.LVRateLight != 0);
}

bool Saturn::ProcessConfigFileLine(FILEHANDLE scn, char *line)

{
	float ftcp;
	double autopTime, d;
	int SwitchState = 0;
	int nasspver = 0, status = 0;
	int n, DummyLoad, i;
	bool found;

	found = true;

    if (!strnicmp (line, "CONFIGURATION", 13)) {
        sscanf (line+13, "%d", &status);
	}
	else if (!strnicmp (line, "NASSPVER", 8)) {
		sscanf (line + 8, "%d", &nasspver);
	}
	else if (!strnicmp (line, "TOALT", 5)) {
        int toalt;
		sscanf (line+5, "%d", &toalt);
		agc.SetDesiredApogee(toalt);
		agc.SetDesiredPerigee(toalt * 0.98);
	}
	else if (papiReadScenario_double(line, "TOAPO", d)) { 
		agc.SetDesiredApogee(d);
	}
	else if (papiReadScenario_double(line, "TOPER", d)) { 
		agc.SetDesiredPerigee(d);
	}
	else if (!strnicmp (line, "BUILDSTATUS", 11)) {
		sscanf (line+11, "%d", &buildstatus);
	}
	else if (!strnicmp (line, "PANEL_ID", 8)) {
		sscanf (line+8, "%d", &PanelId);
	}
	else if (!strnicmp (line, "STAGESTATUS", 11)) {
		sscanf (line+11, "%d", &StageState);
	}
	else if (papiReadScenario_double(line, "DOCKANGLE", DockAngle));
	else if (!strnicmp(line, "STAGECONFIG", 11)) {
		strncpy (StagesString, line + 12, 256);
	}
	else if (papiReadScenario_double(line, "TOHDG", d)) { 
		agc.SetDesiredAzimuth(d);
	}
	else if (!strnicmp (line, "VECHNO", 6)) {
        int numb;
		sscanf (line+6, "%d", &numb);
		VehicleNo = numb;
		SetVehicleStats();
	}
	else if (papiReadScenario_double(line, "TCP", TCPO));
	else if (!strnicmp (line, "DLS", 3)) {
        sscanf (line+3, "%d", &DummyLoad);
		DeleteLaunchSite = (DummyLoad != 0);
	}
	else if (!strnicmp (line, "LOWRES", 6)) {
        sscanf (line+6, "%d", &DummyLoad);
		LowRes = (DummyLoad != 0);
	}
	else if (papiReadScenario_double(line, "SICSHUT", FirstStageCentreShutdownTime)); 
	else if (papiReadScenario_double(line, "SIICSHUT", SecondStageCentreShutdownTime)); 
	else if (papiReadScenario_double(line, "SISHUT", FirstStageShutdownTime)); 
	else if (papiReadScenario_double(line, "SIISHUT", SecondStageShutdownTime)); 
	else if (!strnicmp (line, "SIENG", 5)) {
		sscanf (line + 5, "%d", &SI_EngineNum);
	}
	else if (!strnicmp (line, "SIIENG", 6)) {
		sscanf (line + 6, "%d", &SII_EngineNum);
	}
	else if (!strnicmp (line, "SIIIENG", 7)) {
		sscanf (line + 7, "%d", &SIII_EngineNum);
	}
	else if (!strnicmp (line, "SIIENG", 6)) {
		sscanf (line + 6, "%d", &SII_EngineNum);
	}
	else if (!strnicmp (line, "ISTGJT", 6)) {
		sscanf (line + 6, "%f", &ftcp);
		InterstageSepTime = ftcp;
	}
	else if (!strnicmp (line, "LESJT", 5)) {
		sscanf (line + 5, "%f", &ftcp);
		LESJettisonTime = ftcp;
	}
	else if (!strnicmp (line, "SIIPUT", 6)) {
		sscanf (line + 6, "%f", &ftcp);
		SecondStagePUShiftTime = ftcp;
	}
	else if (!strnicmp (line, "IGMST", 5)) {
		sscanf (line + 5, "%f", &ftcp);
		IGMStartTime = ftcp;
	}
	else if (!strnicmp (line, "THRUSTA", 7)) {
		sscanf (line + 7, "%f", &ftcp);
		ThrustAdjust = ftcp;
	}
	else if (!strnicmp (line, "LEM_DISPLAY", 11)) {
		LEM_DISPLAY = true;
	}
	else if (!strnicmp (line, "MAINSTATE", 9)) {
        SwitchState = 0;
		sscanf (line+9, "%d", &SwitchState);
		SetMainState(SwitchState);
	}
	else if (!strnicmp (line, "ATTACHSTATE", 11)) {
        SwitchState = 0;
		sscanf (line+11, "%d", &SwitchState);
		SetAttachState(SwitchState);
	}
	else if (!strnicmp (line, "A13STATE", 8)) {
        SwitchState = 0;
		sscanf (line+8, "%d", &SwitchState);
		SetA13State(SwitchState);
	}
	else if (!strnicmp (line, "SLASTATE", 8)) {
        SwitchState = 0;
		sscanf (line+8, "%d", &SwitchState);
		SetSLAState(SwitchState);
	}
	else if (!strnicmp (line, "LAUNCHSTATE", 11)) {
        SwitchState = 0;
		sscanf (line+11, "%d", &SwitchState);
		SetLaunchState(SwitchState);
	}
	else if (!strnicmp (line, "LIGHTSTATE", 10)) {
        SwitchState = 0;
		sscanf (line+10, "%d", &SwitchState);
		SetLightState(SwitchState);
	}
	else if (!strnicmp (line, "LMPADCNT", 8)) {
		if (!LMPad) {
			sscanf (line+8, "%d", &LMPadCount);
			if (LMPadCount > 0) {
				LMPad = new unsigned int[LMPadCount * 2];
			}
		}
	}
	else if (!strnicmp (line, "LMPAD", 5)) {
		unsigned int addr, val;
		sscanf (line+5, "%o %o", &addr, &val);
		LMPadValueCount++;
		if (LMPad && LMPadLoadCount < (LMPadCount * 2)) {
			LMPad[LMPadLoadCount++] = addr;
			LMPad[LMPadLoadCount++] = val;
		}
	}
	else if (!strnicmp (line, "CMPAD", 5)) {
		unsigned int addr, val;

		//
		// CM PAD value aren't saved, as the AGC will save them itself. They're only used to load values in
		// a starting scenario.
		//
		// Be sure that you put CMPAD lines _after_ the AGC entries, so that the AGC will know whether it's
		// running Virtual AGC or not. PAD loads are ignored if it's not a Virtual AGC.
		//

		sscanf (line+5, "%o %o", &addr, &val);
		agc.PadLoad(addr, val);
	}
	else if (!strnicmp (line, "APOLLONO", 8)) {
		sscanf (line+8, "%d", &ApolloNo);
	}
	else if (!strnicmp (line, "SATTYPE", 7)) {
		sscanf (line+7, "%d", &SaturnType);
	}
	else if (!strnicmp (line, "AUTOTIMER", 9)) {
        sscanf (line+9, "%f", &ftcp);
		autopTime = ftcp;
	}
	else if (!strnicmp(line, "MISSNTIME", 9)) {
        sscanf (line+9, "%f", &ftcp);
		MissionTime = ftcp;
	}
	else if (!strnicmp(line, "MTD", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		MissionTimerDisplay.SetTime(ftcp);
	}
	else if (!strnicmp(line, "ETD", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		EventTimerDisplay.SetTime(ftcp);
	}
	else if (!strnicmp(line, "NMISSNTIME", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		NextMissionEventTime = ftcp;
	}
	else if (!strnicmp(line, "LMISSNTIME", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		LastMissionEventTime = ftcp;
	}
	else if (!strnicmp(line, "NFAILTIME", 9)) {
        sscanf (line + 9, "%f", &ftcp);
		NextFailureTime = ftcp;
	}
	else if (!strnicmp(line, "SIFUELMASS", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		SI_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "SIIFUELMASS", 11)) {
        sscanf (line + 11, "%f", &ftcp);
		SII_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "S4FUELMASS", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		S4B_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "S4EMPTYMASS", 11)) {
        sscanf (line + 11, "%f", &ftcp);
		S4B_EmptyMass = ftcp;
		S4B_MassLoaded = true;
	}
	else if (!strnicmp(line, "SIEMPTYMASS", 11)) {
        sscanf (line + 11, "%f", &ftcp);
		SI_EmptyMass = ftcp;
		SI_MassLoaded = true;
	}
	else if (!strnicmp(line, "SIIEMPTYMASS", 12)) {
        sscanf (line + 12, "%f", &ftcp);
		SII_EmptyMass = ftcp;
		SII_MassLoaded = true;
	}
	else if (!strnicmp(line, "T1V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		THRUST_FIRST_VAC = ftcp;
		S1_ThrustLoaded = true;
	}
	else if (!strnicmp(line, "I1S", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_FIRST_SL = ftcp;
	}
	else if (!strnicmp(line, "I1V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_FIRST_VAC = ftcp;
	}
	else if (!strnicmp(line, "T2V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		THRUST_SECOND_VAC = ftcp;
		S2_ThrustLoaded = true;
	}
	else if (!strnicmp(line, "I2S", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_SECOND_SL = ftcp;
	}
	else if (!strnicmp(line, "I2V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_SECOND_VAC = ftcp;
	}
	else if (!strnicmp(line, "T3V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		THRUST_THIRD_VAC = ftcp;
		S3_ThrustLoaded = true;
	}
	else if (!strnicmp(line, "I3V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_THIRD_VAC = ftcp;
	}
	else if (!strnicmp(line, "PRELAUNCHATC", 12)) {
		int i;
		sscanf (line + 12, "%d", &i);
		UseATC = (i != 0);
	}
	else if (!strnicmp (line, "PMET", 4)) {
		sscanf(line+4, "%d", &n);
		sscanf(line+8, "%f", &ftcp);
		if (n >= 0 && n < PITCH_TABLE_SIZE) {
			met[n] = ftcp;
		}
	}
	else if (!strnicmp (line, "CPITCH", 6)) {
		sscanf(line+6, "%d", &n);
		sscanf(line+10, "%f", &ftcp);
		if (n >= 0 && n < PITCH_TABLE_SIZE) {
			cpitch[n] = ftcp;
		}
	}
	else if (!strnicmp(line, "MOONLAT", 7)) {
		sscanf(line + 7, "%f", &ftcp);
		LMLandingLatitude = ftcp;
	}
	else if (!strnicmp(line, "MOONLONG", 8)) {
		sscanf(line + 8, "%f", &ftcp);
		LMLandingLongitude = ftcp;
	}
	else if (!strnicmp(line, "MOONALT", 7)) {
		sscanf(line + 7, "%f", &ftcp);
		LMLandingAltitude = ftcp;
	}
	else if (!strnicmp(line, "UNMANNED", 8)) {
		int i;
		sscanf(line + 8, "%d", &i);
		Crewed = (i == 0);
	}
	else if (!strnicmp(line, "AUTOSLOW", 8)) {
		int i;
		sscanf(line + 8, "%d", &i);
		AutoSlow = (i != 0);
	}
	else if (!strnicmp(line, "HASPROBE", 8)) {
		//
		// HASPROBE isn't saved in the scenario, this is solely to allow you
		// to override the default probe state in startup scenarios.
		//
		int i;
		sscanf(line + 8, "%d", &i);
		HasProbe = (i != 0);
	}
	else if (!strnicmp(line, "S4PL", 4)) {
		sscanf(line + 4, "%d", &SIVBPayload);
	}
	else if (!strnicmp(line, "CSMSEP", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CSMSepTime = ftcp;
		CSMSepSet = true;
	}
	else if (!strnicmp(line, "CMSEP", 5)) {
		sscanf(line + 5, "%f", &ftcp);
		CMSepTime = ftcp;
		CMSepSet = true;
	}
	else if (!strnicmp(line, "PLSEP", 5)) {
		sscanf(line + 5, "%f", &ftcp);
		PayloadDeployTime = ftcp;
		PayloadDeploySet = true;
	}
	else if (!strnicmp(line, "S4APO", 5)) {
		sscanf(line + 5, "%f", &ftcp);
		SIVBApogee = ftcp;
	}
	else if (!strnicmp(line, "S4BURN", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		SIVBBurnStart = ftcp;
	}
	else if (!strnicmp(line, "CSMAPO", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CSMApogee = ftcp;
	}
	else if (!strnicmp(line, "CSMPER", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CSMPerigee = ftcp;
	}
	else if (!strnicmp(line, "CSMBURN", 7)) {
		sscanf(line + 7, "%f", &ftcp);
		CSMBurnStart = ftcp;
	}
	else if (!strnicmp(line, "CSMACCEL", 8)) {
		sscanf(line + 8, "%f", &ftcp);
		CSMAccelSet = true;
		CSMAccelTime = ftcp;
	}
	else if (!strnicmp(line, "CSMACCEND", 9)) {
		sscanf(line + 9, "%f", &ftcp);
		CSMAccelEnd = ftcp;
	}
	else if (!strnicmp(line, "CSMACCPITCH", 11)) {
		sscanf(line + 11, "%f", &ftcp);
		CSMAccelPitch = ftcp;
	}
	else if (!strnicmp(line, "SMFUELLOAD", 10)) {
		sscanf(line + 10, "%f", &ftcp);
		SM_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "CMFUELLOAD", 10)) {
		sscanf(line + 10, "%f", &ftcp);
		CM_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "SMMASS", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		SM_EmptyMass = ftcp;
	}
	else if (!strnicmp(line, "CMMASS", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CM_EmptyMass = ftcp;
	}
	else if (!strnicmp(line, "LANDFAIL", 8)) {
		sscanf(line + 8, "%d", &LandFail.word);
	}
	else if (!strnicmp(line, "LAUNCHFAIL", 10)) {
		sscanf(line + 10, "%d", &LaunchFail.word);
	}
	else if (!strnicmp(line, "SWITCHCHFAIL", 10)) {
		sscanf(line + 10, "%d", &SwitchFail.word);
	}
	else if (!strnicmp(line, "LANG", 4)) {
		strncpy (AudioLanguage, line + 5, 64);
	}
	else if (!strnicmp(line, "LEMN", 4)) {
		//
		// LEMN (LEM name) is a synonym for PAYN (Payload name) for old scenarios.
		//
		strncpy (PayloadName, line + 5, 64);
	}
	else if (!strnicmp(line, "PAYN", 4)) {
		strncpy (PayloadName, line + 5, 64);
	}
	else if (!strnicmp(line, "MISSIONTRACKING", 15)) {
		int i;
		sscanf(line + 15, "%d", &i);
		if (i)
			mcc.enableMissionTracking();
	}
	else if (!strnicmp(line, DSKY_START_STRING, sizeof(DSKY_START_STRING))) {
		dsky.LoadState(scn, DSKY_END_STRING);
	}
	else if (!strnicmp(line, DSKY2_START_STRING, sizeof(DSKY2_START_STRING))) {
		dsky2.LoadState(scn, DSKY2_END_STRING);
	}
	else if (!strnicmp(line, FDAI_START_STRING, sizeof(FDAI_START_STRING))) {
		fdaiLeft.LoadState(scn, FDAI_END_STRING);
	}
	else if (!strnicmp(line, FDAI2_START_STRING, sizeof(FDAI2_START_STRING))) {
		fdaiRight.LoadState(scn, FDAI2_END_STRING);
	}
	else if (!strnicmp(line, AGC_START_STRING, sizeof(AGC_START_STRING))) {
		agc.LoadState(scn);
	}
	else if (!strnicmp(line, IMU_START_STRING, sizeof(IMU_START_STRING))) {
		imu.LoadState(scn);
	}
	else if (!strnicmp(line, GDC_START_STRING, sizeof(GDC_START_STRING))) {
		gdc.LoadState(scn);
	}
	else if (!strnicmp(line, BMAG1_START_STRING, sizeof(BMAG1_START_STRING))) {
		bmag1.LoadState(scn);
	}
	else if (!strnicmp(line, BMAG2_START_STRING, sizeof(BMAG2_START_STRING))) {
		bmag2.LoadState(scn);
	}
	else if (!strnicmp(line, MCC_START_STRING, sizeof(MCC_START_STRING))) {
		mcc.LoadState(scn);
	}
	else if (!strnicmp(line, RTCC_START_STRING, sizeof(RTCC_START_STRING))) {
		mcc.rtcc->LoadState(scn);
	}
	else if (!strnicmp(line, LVDC_START_STRING, sizeof(LVDC_START_STRING))) {
		LoadLVDC(scn);
	}
	else if (!strnicmp(line, ASCP_START_STRING, sizeof(ASCP_START_STRING))) {
		ascp.LoadState(scn);
	}
	else if (!strnicmp(line, IU_START_STRING, sizeof(IU_START_STRING))) {
		iu.LoadState(scn);
	}
	else if (!strnicmp(line, CWS_START_STRING, sizeof(CWS_START_STRING))) {
		cws.LoadState(scn);
	}
	else if (!strnicmp(line, SECS_START_STRING, sizeof(SECS_START_STRING))) {
		secs.LoadState(scn);
	}
	else if (!strnicmp(line, ELS_START_STRING, sizeof(ELS_START_STRING))) {
		els.LoadState(scn);
	}
	else if (!strnicmp(line, DOCKINGPROBE_START_STRING, sizeof(DOCKINGPROBE_START_STRING))) {
		dockingprobe.LoadState(scn);
	}
	else if (!strnicmp (line, "SYSTEMSSTATE", 12)) {
		sscanf (line + 12, "%d", &systemsState);
	}
	else if (!strnicmp (line, "LSYSTEMSMISSNTIME", 17)) {
		sscanf (line + 17, "%f", &ftcp);
		lastSystemsMissionTime = ftcp;
	}
    else if (!strnicmp (line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
		Panelsdk.Load(scn);			//send the loading to the Panelsdk
	}
    else if (!strnicmp (line, PANELSWITCH_START_STRING, strlen(PANELSWITCH_START_STRING))) { 
		PSH.LoadState(scn);	
	}
	else if (!strnicmp (line, "COASENABLED", 11)) {
		sscanf (line + 11, "%i", &coasEnabled);
	}
	else if (!strnicmp (line, "MR", 2)) {
		sscanf (line + 2, "%f", &ftcp);
		MixtureRatio = ftcp;
	}
	else
		found = false;

	// Next block because of Visual C++ compiler restriction, only 128 "else if's" allowed
	if (!found) {
		found = true;

		if (!strnicmp(line, SPSPROPELLANT_START_STRING, sizeof(SPSPROPELLANT_START_STRING))) {
			SPSPropellant.LoadState(scn);
		}
		else if (!strnicmp(line, SPSENGINE_START_STRING, sizeof(SPSENGINE_START_STRING))) {
			SPSEngine.LoadState(scn);
		}
	    else if (!strnicmp (line, "FDAIDISABLED", 12)) {
		    sscanf (line + 12, "%i", &fdaiDisabled);
	    }
	    else if (!strnicmp (line, "FDAISMOOTH", 10)) {
		    sscanf (line + 10, "%i", &fdaiSmooth);
	    }
	    else if (!strnicmp (line, "MAINPANELSPLIT", 14)) {
		    sscanf (line + 14, "%i", &MainPanelSplit);
	    }
		else if (!strnicmp (line, "GNSPLIT", 7)) {
			sscanf (line + 7, "%i", &GNSplit);
		}
		else if (!strnicmp (line, "STAGE", 5)) {
			sscanf (line+5, "%d", &stage);
		}
		else if (!strnicmp (line, "MAXTIMEACC", 10)) {
			sscanf (line+10, "%d", &maxTimeAcceleration);
		}
		else if (!strnicmp (line, "MULTITHREAD", 11)) {
			int value;
			sscanf (line+11, "%d", &value);
			IsMultiThread=(value>0)?true:false;
		}

		else if (!strnicmp(line, "NOHGA", 5)) {
			//
			// NOHGA isn't saved in the scenario, this is solely to allow you
			// to override the default NOHGA state in startup scenarios.
			//
			sscanf(line + 5, "%d", &i);
			NoHGA = (i != 0);
		}
		else if (!strnicmp(line, SPSGIMBALACTUATOR_PITCH_START_STRING, sizeof(SPSGIMBALACTUATOR_PITCH_START_STRING))) {
			SPSEngine.pitchGimbalActuator.LoadState(scn);
		}
		else if (!strnicmp(line, SPSGIMBALACTUATOR_YAW_START_STRING, sizeof(SPSGIMBALACTUATOR_YAW_START_STRING))) {
			SPSEngine.yawGimbalActuator.LoadState(scn);
		}
		else if (!strnicmp(line, EMS_START_STRING, sizeof(EMS_START_STRING))) {
			ems.LoadState(scn);
		}
		else if (!strnicmp(line, SMRCSPROPELLANT_A_START_STRING, sizeof(SMRCSPROPELLANT_A_START_STRING))) {
			SMQuadARCS.LoadState(scn);
		}
		else if (!strnicmp(line, SMRCSPROPELLANT_B_START_STRING, sizeof(SMRCSPROPELLANT_B_START_STRING))) {
			SMQuadBRCS.LoadState(scn);
		}
		else if (!strnicmp(line, SMRCSPROPELLANT_C_START_STRING, sizeof(SMRCSPROPELLANT_C_START_STRING))) {
			SMQuadCRCS.LoadState(scn);
		}
		else if (!strnicmp(line, SMRCSPROPELLANT_D_START_STRING, sizeof(SMRCSPROPELLANT_D_START_STRING))) {
			SMQuadDRCS.LoadState(scn);
		}
		else if (!strnicmp(line, CMRCSPROPELLANT_1_START_STRING, sizeof(CMRCSPROPELLANT_1_START_STRING))) {
			CMRCS1.LoadState(scn);
		}
		else if (!strnicmp(line, CMRCSPROPELLANT_2_START_STRING, sizeof(CMRCSPROPELLANT_2_START_STRING))) {
			CMRCS2.LoadState(scn);
		}
		// DS20070204 LVDC++ MODE
	    else if (!strnicmp (line, "USE_LVDC", 8)) {
		    use_lvdc = true;
	    }
	    else if (!strnicmp (line, "CABINPRESSUREREGULATOR", 22)) {
		    CabinPressureRegulator.LoadState(line);
	    }
	    else if (!strnicmp (line, "O2DEMANDREGULATOR", 17)) {
		    O2DemandRegulator.LoadState(line);
	    }
	    else if (!strnicmp (line, "CABINPRESSURERELIEFVALVE1", 25)) {
		    CabinPressureReliefValve1.LoadState(line);
	    }
	    else if (!strnicmp (line, "CABINPRESSURERELIEFVALVE2", 25)) {
		    CabinPressureReliefValve2.LoadState(line);
	    }
	    else if (!strnicmp (line, "O2SMSUPPLY", 10)) {
		    O2SMSupply.LoadState(line);
		}
	    else if (!strnicmp (line, "CREWSTATUS", 10)) {
		    CrewStatus.LoadState(line);
	    }
	    else if (!strnicmp (line, "SIDEHATCH", 9)) {
		    SideHatch.LoadState(line);
	    }
	    else if (!strnicmp (line, "UNIFIEDSBAND", 12)) {
		    usb.LoadState(line);
	    }
	    else if (!strnicmp (line, "HIGHGAINANTENNA", 12)) {
		    hga.LoadState(line);
	    }
	    else if (!strnicmp (line, "DATARECORDER", 12)) {
		    dataRecorder.LoadState(line);
	    }
		else if (!strnicmp(line, CMOPTICS_START_STRING, sizeof(CMOPTICS_START_STRING))) {
			optics.LoadState(scn);
		} 
		else if (!strnicmp (line, "VAGCCHECKLISTAUTOSLOW", 21)) {
			sscanf (line + 21, "%i", &i);
			VAGCChecklistAutoSlow = (i != 0);
		} 
		else if (!strnicmp (line, "VAGCCHECKLISTAUTOENABLED", 24)) {
			sscanf (line + 24, "%i", &i);
			VAGCChecklistAutoEnabled = (i != 0);
		} 
		else if (!strnicmp (line, "OPTICSDSKYENABLED", 17)) {
			sscanf (line + 17, "%i", &opticsDskyEnabled);
		}
		else if (!strnicmp (line, "ORDEALENABLED", 13)) {
			sscanf (line + 13, "%i", &ordealEnabled);
		}
		else if (!strnicmp (line, "HATCHPANEL600ENABLED", 20)) {
			sscanf (line + 20, "%i", &hatchPanel600EnabledLeft);
			hatchPanel600EnabledRight = hatchPanel600EnabledLeft;
		}
		else if (!strnicmp (line, "PANEL382ENABLED", 15)) {
			sscanf (line + 15, "%i", &panel382Enabled);
		}
		else if (!strnicmp (line, "FOVFIXED", 8)) {
			sscanf (line + 8, "%i", &i);
			FovFixed = (i != 0);
		}
		else if (!strnicmp (line, "FOVEXTERNAL", 11)) {
			sscanf (line + 11, "%i", &FovExternal);
		}
		else if (!strnicmp (line, "FOVSAVEEXTERNAL", 15)) {
			sscanf (line + 15, "%lf", &FovSaveExternal);
		}
		else if (!strnicmp (line, "FOVSAVE", 7)) {
			sscanf (line + 7, "%lf", &FovSave);
		}
		else if (papiReadScenario_double(line, "TOINCLINATION", d)) { 
			agc.SetDesiredInclination(d);
		}
		else if (!strnicmp (line, "JOYSTICK_RHC", 12)) {
			sscanf (line + 12, "%i", &rhc_id);
			if(rhc_id > 1){ rhc_id = 1; } // Be paranoid
		}
		else if (!strnicmp (line, "JOYSTICK_RTTID", 14)) {
			sscanf (line + 14, "%i", &rhc_thctoggle_id);
			if (rhc_thctoggle_id > 128){ rhc_thctoggle_id = 128; } // Be paranoid
		}
		else if (!strnicmp (line, "JOYSTICK_RRT", 12)) {
			sscanf (line + 12, "%i", &rhc_rot_id);
			if(rhc_rot_id > 2){ rhc_rot_id = 2; } // Be paranoid
		}
		else if (!strnicmp (line, "JOYSTICK_RSL", 12)) {
			sscanf (line + 12, "%i", &rhc_sld_id);
			if(rhc_sld_id > 2){ rhc_sld_id = 2; } // Be paranoid
		}
		else if (!strnicmp (line, "JOYSTICK_RZX", 12)) {
			sscanf (line + 12, "%i", &rhc_rzx_id);
		}
		else if (!strnicmp (line, "JOYSTICK_THC", 12)) {
			sscanf (line + 12, "%i", &thc_id);
			if(thc_id > 1){ thc_id = 1; } // Be paranoid
		}
		else if (!strnicmp (line, "JOYSTICK_TRT", 12)) {
			sscanf (line + 12, "%i", &thc_rot_id);
			if(thc_rot_id > 2){ thc_rot_id = 2; } // Be paranoid
		}
		else if (!strnicmp (line, "JOYSTICK_TSL", 12)) {
			sscanf (line + 12, "%i", &thc_sld_id);
			if(thc_sld_id > 2){ thc_sld_id = 2; } // Be paranoid
		}
		else if (!strnicmp (line, "JOYSTICK_TZX", 12)) {
			thc_rzx_id = 1;
		}
		else if (!strnicmp (line, "JOYSTICK_RDB", 12)) {
			rhc_debug = 1;
		}
		else if (!strnicmp (line, "JOYSTICK_TDB", 12)) {
			thc_debug = 1;
		}
		else if (!strnicmp (line, "JOYSTICK_RAUTO", 14)) {
			rhc_auto = 1;
		}
		else if (!strnicmp (line, "JOYSTICK_TAUTO", 14)) {
			thc_auto = 1;
		}
		else if (!strnicmp (line, "JOYSTICK_RTT", 12)) {
			rhc_thctoggle = true;
		}
		else if (papiReadScenario_double(line, "MOONMJD", LMLandingMJD)); 
		else if (papiReadScenario_double(line, "LMDSCFUEL", LMDescentFuelMassKg)); 
		else if (papiReadScenario_double(line, "LMASCFUEL", LMAscentFuelMassKg)); 
		else if (!strnicmp(line, "MOONBASE", 8)) {
			strncpy (LMLandingBase, line + 9, 256);
		}
		else if (papiReadScenario_double(line, "SPLASHLAT", SplashdownLatitude)); 
		else if (papiReadScenario_double(line, "SPLASHLONG", SplashdownLongitude)); 
		else if (papiReadScenario_double(line, "EARTHEIMJD", EntryInterfaceMJD)); 
		else if (papiReadScenario_double(line, "TLIMJD", TransLunarInjectionMJD)); 
		else if (papiReadScenario_double(line, "LOIMJD", LunarOrbitInsertionMJD)); 
		else if (papiReadScenario_double(line, "FREERETURNPECMJD", FreeReturnPericynthionMJD)); 
		else if (papiReadScenario_double(line, "FREERETURNPECALT", FreeReturnPericynthionAltitude)); 
		else if (papiReadScenario_double(line, "TLIOFFSETLON", TransLunarInjectionOffsetLon)); 
		else if (papiReadScenario_double(line, "TLIOFFSETLAT", TransLunarInjectionOffsetLat)); 
		else if (papiReadScenario_double(line, "TLIOFFSETRAD", TransLunarInjectionOffsetRad)); 
		else if (papiReadScenario_double(line, "SIVBCUTOFFTIME", SIVBCutoffTime)); 
		else if (papiReadScenario_bool(line, "J2ISACTIVE", J2IsActive)); 
		else if (!strnicmp(line, ChecklistControllerStartString, strlen(ChecklistControllerStartString))) {
			checkControl.load(scn);
		}
		else if (!strnicmp(line, "LEMCHECKAUTO", 12)) {
			int temp = 0;
			sscanf(line+12, "%i", &temp);
			if (temp != 0)
				LEMCheckAuto = true;
			else
				LEMCheckAuto = false;
		} else if (!strnicmp(line, "LEMCHECK", 8)) {
			strcpy(LEMCheck, line + 9);
		} else if (!strnicmp(line, SaturnEventStartString, strlen(SaturnEventStartString))) {
			eventControl.load(scn);
		} else if (!strnicmp(line, RJEC_START_STRING, sizeof(RJEC_START_STRING))) {
			rjec.LoadState(scn);
		} else if (!strnicmp(line, ORDEAL_START_STRING, sizeof(ORDEAL_START_STRING))) {
			ordeal.LoadState(scn);
		} else {
			found = false;
		}
	}
	return found;
}

void Saturn::GetPayloadSettings(PayloadSettings &ls)

{
	ls.AutoSlow = AutoSlow;
	ls.Crewed = Crewed;
	ls.LandingAltitude = LMLandingAltitude;
	ls.LandingLatitude = LMLandingLatitude;
	ls.LandingLongitude = LMLandingLongitude;
	ls.AscentFuelKg = LMAscentFuelMassKg;
	ls.DescentFuelKg = LMDescentFuelMassKg;
	strncpy (ls.language, AudioLanguage, 63);
	strncpy (ls.CSMName, GetName(), 63);
	ls.MissionNo = ApolloNo;
	ls.MissionTime = MissionTime;
	strncpy (ls.checklistFile, LEMCheck, 100);
	ls.checkAutoExecute = LEMCheckAuto;
}

void Saturn::GetScenarioState (FILEHANDLE scn, void *vstatus)

{
    char VNameApollo[256];
	char *line;
	int n;

	//
	// If the name of the spacecraft is "AS-xxx" then get the vehicle
	// number from that.
	//

	strncpy (VNameApollo, GetName(), 255);
	if (!strnicmp (VNameApollo, "AS-", 3)) {
		sscanf(VNameApollo+3, "%d", &n);
		if (n > 200) {
			VehicleNo = n;
			SetVehicleStats();
		}
	}

	//
	// Seed the random number generator with whatever junk we can
	// find.
	//

	srandom(VehicleNo + (int) vstatus + (int) time(0));

	//
	// At some point we should reorder these checks by length, to minimise the chances
	// of accidentally matching a longer string.
	//

	while (oapiReadScenario_nextline (scn, line)) {
		if (!ProcessConfigFileLine(scn, line)) {
			ParseScenarioLineEx (line, vstatus);
        }
    }

	if (!Crewed && (CSMApogee > 0.0) && (CSMBurnStart > 0) && (stage < CM_STAGE)) {
		CSMBurn = true;
	}

	soundlib.SetLanguage(AudioLanguage);
	LoadDefaultSounds();

	//
	// Recalculate stage masses.
	//

	UpdatePayloadMass();
	CalculateStageMass ();

	//
	// And pass it the mission number and realism settings.
	//

	agc.SetMissionInfo(ApolloNo, PayloadName);

	//
	// Set random failures if appropriate.
	//

	if (!ApolloNo) {
		SetRandomFailures();
	}

	//
	// Realism Mode Settings
	//

	// Enable Orbiter's attitude control for unmanned missions
	// as long as they rely on Orbiter's navmodes (killrot etc.)

	if (!Crewed) {
		checkControl.autoExecute(true);
		checkControl.autoExecuteSlow(false);
		checkControl.autoExecuteAllItemsAutomatic(true);
	}
	
	// Disable it and do some other settings when not in 
	// Quickstart mode

	else {
		checkControl.autoExecute(VAGCChecklistAutoEnabled);
		checkControl.autoExecuteSlow(VAGCChecklistAutoSlow);
		checkControl.autoExecuteAllItemsAutomatic(false);
	
	}
}

//
// Set the appropriate mass based on the SIVB payload. I believe these are roughly the
// correct numbers.
//

void Saturn::UpdatePayloadMass()

{
	switch (SIVBPayload) {
	case PAYLOAD_LEM:
		S4PL_Mass = 4374.0 + LMAscentFuelMassKg + LMDescentFuelMassKg;
		break;

	case PAYLOAD_ASTP:
		S4PL_Mass = 2012;
		break;

	case PAYLOAD_LTA:
		S4PL_Mass = 13381;
		break;

	case PAYLOAD_LTA6:
		S4PL_Mass = 11794;
		break;

	case PAYLOAD_LM1:
		S4PL_Mass = 14360;
		break;

	case PAYLOAD_LTA8:
		S4PL_Mass = 9026;
		break;

	case PAYLOAD_TARGET:
		S4PL_Mass = 1789;
		break;

	case PAYLOAD_DOCKING_ADAPTER:
		S4PL_Mass = 4700.0; // see http://www.ibiblio.org/mscorbit/mscforum/index.php?topic=2064.0
		break;

	default:
		S4PL_Mass = 0;
		break;
	}
}

//
// Destroy any old stages that we don't want to keep around.
//

void Saturn::DestroyStages(double simt)

{
	if (hstg1 && GetStage() > LAUNCH_STAGE_ONE) {
		KillAlt(hstg1, 60);
	}

	if (hstg2) {
		KillAlt(hstg2, 1000);
	}

	if (hintstg) {
		KillAlt(hintstg, 1000);
	}

	if (hesc1) {
		KillAlt(hesc1,90);
	}

	if (hPROBE) {
		KillDist(hPROBE);
	}

	if (hOpticsCover) {
		KillDist(hOpticsCover);
	}

	if (hApex) {
		KillAlt(hApex, 5);
	}

	if (hDrogueChute) {
		VESSEL *drogueChute = oapiGetVesselInterface(hDrogueChute);
		ATTACHMENTHANDLE ah = drogueChute->GetAttachmentHandle(true, 0);
		if (drogueChute->GetAttachmentStatus(ah) == NULL) {
			KillAlt(hDrogueChute, 100);
		}
	}

	if (hMainChute) {
		VESSEL *mainChute = oapiGetVesselInterface(hMainChute);
		ATTACHMENTHANDLE ah = mainChute->GetAttachmentHandle(true, 0);
		if (mainChute->GetAttachmentStatus(ah) == NULL) {
			KillAlt(hMainChute, 100);
		}
	}

	if (hs4b1) {
		KillDist(hs4b1);
	}
	if (hs4b2) {
		KillDist(hs4b2);
	}
	if (hs4b3) {
		KillDist(hs4b3);
	}
	if (hs4b4) {
		KillDist(hs4b4);
	}

	//
	// In most missions we can delete the VAB, Crawler and mobile launcher when we can no longer
	// see them.
	//

	if (DeleteLaunchSite && stage >= STAGE_ORBIT_SIVB) {
		if (hVAB) {
			KillDist(hVAB, 100000.0);
		}

		if (hCrawler) {
			KillDist(hCrawler, 10000.0);
		}

		if (hML) {
			KillDist(hML, 50000.0);
		}

		if (hMSS) {
			KillDist(hMSS, 50000.0);
		}
	}

	//
	// Destroy seperated SM when it drops too low in the atmosphere.
	//

	if (hSMJet) {
		KillAlt(hSMJet, 35000);
	}
}

void Saturn::SetStage(int s)

{
	stage = s;
	StageState = 0;

	CheckSMSystemsState();
	
	//
	// Event management
	//

	switch (stage) 
	{
	case LAUNCH_STAGE_TWO:
		eventControl.SECOND_STAGE_STAGING = MissionTime;
		break;
	case LAUNCH_STAGE_SIVB:
		eventControl.SIVB_STAGE_STAGING = MissionTime;
		// In case of the Saturn 1B, the S-IVB stage and the second stage is the same  
		if (eventControl.SECOND_STAGE_STAGING == MINUS_INFINITY)
			eventControl.SECOND_STAGE_STAGING = MissionTime;
		break;
	case STAGE_ORBIT_SIVB:
		eventControl.EARTH_ORBIT_INSERTION = MissionTime;
		break;
	case CSM_LEM_STAGE:
		eventControl.CSM_LV_SEPARATION_DONE = MissionTime;
		break;
	case CM_STAGE:
		eventControl.CM_SM_SEPARATION_DONE = MissionTime;
		break;
	}

	//
	// CSM/LV separation
	//

	if (stage == CSM_LEM_STAGE) {

		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
		ClearTLISounds();

		iuCommandConnector.Disconnect();
		sivbCommandConnector.Disconnect();
		sivbControlConnector.Disconnect();

		CSMToSIVBConnector.AddTo(&iuCommandConnector);
		CSMToSIVBConnector.AddTo(&sivbControlConnector);
	}
}

void Saturn::DoLaunch(double simt)

{
	//
	// Uncage IMU
	//
	IMUGuardedCageSwitch.SwitchTo(TOGGLESWITCH_DOWN); 

	//
	// Light the liftoff indicator for the crew.
	//

	SetLiftoffLight();

	//
	// Switch to the first launch stage.
	//

	SetStage(LAUNCH_STAGE_ONE);

	//
	// For now, we'll reset the mission timer to zero and enable it.
	//
	// The people on the ProjectApollo mailing list believe that this is the correct
	// behaviour for the Mission Timer, and it shouldn't run at all until liftoff.
	//
	// However, others believe it should free run. We haven't found a definitive
	// answer yet.
	//
	// Meanwhile we have a definite answer, the prelaunch procedures in the AOH clearly 
	// state that it runs free. Additionally do NOT call MissionTimerDisplay.SetRunning(true) here,
	// either it's already running (which it should) or it's not, but when you let it run here, you have
	// a running mission timer and the mission timer start switch is at stop or reset, which is not 
	// possible electrically.
	//

	MissionTimerDisplay.Reset();
	MissionTimerDisplay.SetEnabled(true);
	EventTimerDisplay.Reset();
	EventTimerDisplay.SetEnabled(true);
	EventTimerDisplay.SetRunning(true);

	//
	// Tell the AGC that we've lifted off.
	//

	agc.SetInputChannelBit(030, LiftOff, true);

	//
	// Set full thrust, just in case.
	//

	SetThrusterGroupLevel(thg_main, 1.0);
	contrailLevel = 1.0;

	//
	// And play the launch sound.
	//

	if (LaunchS.isValid() && !LaunchS.isPlaying())
	{
		LaunchS.play(NOLOOP,255);
		LaunchS.done();
	}

	//
	// Ensure autopilot is on at launch.
	//

	autopilot = true;
}

void Saturn::GenericTimestep(double simt, double simdt, double mjd)

{
	int i;

	if (GenericFirstTimestep) {
		//
		// Do any generic setup.
		//

		//
		// Get the handles for any odds and ends that are out there.
		//

		hML = oapiGetVesselByName("ML");
		hMSS = oapiGetVesselByName("MSS");
		hCrawler = oapiGetVesselByName("Crawler-Transporter");
		hVAB = oapiGetVesselByName("VAB");

		GenericFirstTimestep = false;
		SetView();
	}

	//
	// Update mission time and mission timer.
	//

	MissionTime += simdt;
	MissionTimerDisplay.Timestep(simt, simdt);
	EventTimerDisplay.Timestep(simt, simdt);

	//
	// Panel flash counter.
	//

	if (MissionTime >= NextFlashUpdate) {
		PanelFlashOn = !PanelFlashOn;
		NextFlashUpdate = MissionTime + 0.25;
	}

	//
	// Timestep tracking.
	//

#ifdef TRACK_TIMESTEPS
	CurrentTimestep++;
	if (oapiGetSimStep() > LongestTimestep) {
		LongestTimestep = CurrentTimestep;
		LongestTimestepLength = oapiGetSimStep();
	}
#endif // TRACK_TIMESTEPS

	//
	// Reduce jostle.
	//

	ViewOffsetx *= 0.95;
	ViewOffsety *= 0.95;
	ViewOffsetz *= 0.95;

	//
	// And update for acceleration.
	//

	double amt = fabs(aHAcc / 25.0) - 0.1;
	if (amt > 0.25)
		amt = 0.25;

	if (amt > 0)
		JostleViewpoint(amt);

	//
	// Velocity calculations
	//

	VESSELSTATUS status;
	GetStatus(status);
	
	double aSpeed = length(status.rvel);
	actualVEL = (aSpeed / 1000.0 * 3600.0);
	actualALT = GetAltitude();
	actualFUEL = ((GetFuelMass() * 100.0) / GetMaxFuelMass());
		
	VECTOR3 hvel;
	GetHorizonAirspeedVector(hvel);
	aVSpeed = hvel.y;

	// Manage velocity cache
	for (i = LASTVELOCITYCOUNT - 1; i > 0; i--) {
		LastVelocity[i] = LastVelocity[i - 1];
		LastVerticalVelocity[i] = LastVerticalVelocity[i -1];
		LastSimt[i] = LastSimt[i - 1];
	}
	if (LastVelocityFilled < LASTVELOCITYCOUNT - 1)	LastVelocityFilled++;

	// Store current velocities
	LastVelocity[0] = status.rvel;
	LastVerticalVelocity[0] = aVSpeed;
	LastSimt[0] = simt;

	// Calculate accelerations
	if (LastVelocityFilled > 0) {
		aHAcc = (aSpeed - length(LastVelocity[LastVelocityFilled])) / (simt - LastSimt[LastVelocityFilled]);
		aVAcc = (aVSpeed - LastVerticalVelocity[LastVelocityFilled]) / (simt - LastSimt[LastVelocityFilled]);

		//  This stuff is to compute the component of the total acceleration
		//	along the z axis. This supports the "ACCEL G meter" on the panel.
		double agrav, radius, mass, calpha, salpha, cbeta, sbeta, radius2, DVX, DVY, DVZ;
		OBJHANDLE hPlanet;

		calpha = cos(status.arot.x);
		cbeta = cos(status.arot.y);
		salpha = sin(status.arot.x);
		sbeta = sin(status.arot.y);

		DVX = status.rvel.x - LastVelocity[LastVelocityFilled].x;
		DVY = status.rvel.y - LastVelocity[LastVelocityFilled].y;
		DVZ = status.rvel.z - LastVelocity[LastVelocityFilled].z;
		DVZ = cbeta * (DVY * salpha + DVZ * calpha) - DVX * sbeta;
		aZAcc = DVZ / (simt - LastSimt[LastVelocityFilled]);

		hPlanet = GetSurfaceRef();
		mass = oapiGetMass(hPlanet);
		radius2 = status.rpos.x * status.rpos.x + status.rpos.y * status.rpos.y + status.rpos.z * status.rpos.z;
		radius = sqrt(radius2);
		agrav = cbeta * (status.rpos.y * salpha + status.rpos.z * calpha) - status.rpos.x * sbeta;
		agrav *= GKSI * mass / (radius * radius2);
		aZAcc += agrav;
	}

	SystemsTimestep(simt, simdt, mjd);

	//
	// Check for LES jettison.
	//

	if (LESAttached)
	{
		if (TowerJett1Switch.GetState() == THREEPOSSWITCH_UP || TowerJett2Switch.GetState() == THREEPOSSWITCH_UP)
		{
			JettisonLET();
		}
	}

	if(stage < LAUNCH_STAGE_SIVB) {
		if (GetNavmodeState(NAVMODE_KILLROT)) {
			DeactivateNavmode(NAVMODE_KILLROT);
		}
	}

	//
	// Only the SM has linear thrusters.
	//

	if (stage != CSM_LEM_STAGE) {
		if (GetAttitudeMode() == ATTMODE_LIN){
			SetAttitudeMode(ATTMODE_ROT);
		}
	}

	if (GetAltitude() < 1470) {
		actualVEL = actualVEL-1470+GetAltitude();
	}

	if (GroundContact()) {
		actualVEL = 0;
	}

	if (habort) {
		double altabort;
		int getIT;

		getIT = oapiGetAltitude(habort,&altabort);

		if (altabort < 100 && getIT > 0) {
			oapiDeleteVessel(habort,GetHandle());
			habort = NULL;
		}
	}

	//
	// Docking radar sound only for CSM_LEM_STAGE when nothing docked
	//

	if (stage == CSM_LEM_STAGE) {
		if (!dockingprobe.IsDocked())
			soundlib.SoundOptionOnOff(PLAYRADARBIP, TRUE);
		else
			soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	}
	else {
		soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	}

	//
	// Destroy obsolete stages
	//

	if (MissionTime >= NextDestroyCheckTime) {
		DestroyStages(simt);
		NextDestroyCheckTime = MissionTime + 1.0;
	}

	//
	// Actualize timed sounds
	//

	if (stage >= ONPAD_STAGE) {
		timedSounds.Timestep(MissionTime, simdt, AutoSlow);
	}

	//
	// Play RCS sound in case of Orbiter's attitude control is disabled
	//

	RCSSoundTimestep();

	//
	// IMFD5 communication support
	//

	IMFDVariableConfiguration vc;
	// Set GET
	if (MissionTime >= 0) {
		vc.GET = MissionTime;
	} else {
		vc.GET = -1;
	}
	vc.DataTimeStamp = simt;
	IMFD_Client.SetVariableConfiguration(vc);
	IMFD_Client.TimeStep(); 
}

void StageTransform(VESSEL *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel)
{
	VECTOR3 rofs, rvel = {vs->rvel.x, vs->rvel.y, vs->rvel.z};

	rofs.x = 0;
	rofs.y = 0;
	rofs.z = 0;

	// Staging Velocity represents
	// Need to do some transforms to get the correct vector to eject the stage

	vessel->Local2Rel (ofs, vs->rpos);
	vessel->GlobalRot (vel, rofs);
	vs->rvel.x = rvel.x+rofs.x;
	vs->rvel.y = rvel.y+rofs.y;
	vs->rvel.z = rvel.z+rofs.z;
}

int Saturn::clbkConsumeDirectKey(char *kstate)

{
	if (KEYMOD_SHIFT(kstate) || KEYMOD_ALT(kstate)) {
		return 0; 
	}

	// position test
	/*
	VESSELSTATUS vs;
	GetStatus(vs);
	double moveStep = 1.0e-7;

	if (KEYMOD_CONTROL(kstate))
		moveStep = 1.0e-9;

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		vs.vdata[0].x += moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {			
		vs.vdata[0].y -= moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);			
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {			
		vs.vdata[0].y += moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		vs.vdata[0].x -= moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD1)) {
		vs.vdata[0].z -= 1.0e-4;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		vs.vdata[0].z += 1.0e-4;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		SetTouchdownPoints (_V(0, -1, -GetCOG_elev() + 0.01), _V(-1, 1, -GetCOG_elev() + 0.01), _V(1, 1, -GetCOG_elev() + 0.01));
		RESETKEY(kstate, OAPI_KEY_A);
	}
	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		SetTouchdownPoints (_V(0, -1, -GetCOG_elev() - 0.01), _V(-1, 1, -GetCOG_elev() - 0.01), _V(1, 1, -GetCOG_elev() - 0.01));
		RESETKEY(kstate, OAPI_KEY_S);
	}
	sprintf(oapiDebugString(), "GetCOG_elev %f", GetCOG_elev());
	*/
	
	return 0;
}

int Saturn::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	if (KEYMOD_SHIFT(kstate)){
		// Do DSKY stuff
		if(down){
			switch(key){
				case OAPI_KEY_DECIMAL:
					dsky.ClearPressed();
					break;
				case OAPI_KEY_PRIOR:
					dsky.ResetPressed();
					break;
				case OAPI_KEY_HOME:
					dsky.KeyRel();
					break;
				case OAPI_KEY_NUMPADENTER:
					dsky.EnterPressed();
					break;
				case OAPI_KEY_DIVIDE:
					dsky.VerbPressed();
					break;
				case OAPI_KEY_MULTIPLY:
					dsky.NounPressed();
					break;
				case OAPI_KEY_ADD:
					dsky.PlusPressed();
					break;
				case OAPI_KEY_SUBTRACT:
					dsky.MinusPressed();
					break;
				case OAPI_KEY_END:
					dsky.ProgPressed();
					break;
				case OAPI_KEY_NUMPAD1:
					dsky.NumberPressed(1);
					break;
				case OAPI_KEY_NUMPAD2:
					dsky.NumberPressed(2);
					break;
				case OAPI_KEY_NUMPAD3:
					dsky.NumberPressed(3);
					break;
				case OAPI_KEY_NUMPAD4:
					dsky.NumberPressed(4);
					break;
				case OAPI_KEY_NUMPAD5:
					dsky.NumberPressed(5);
					break;
				case OAPI_KEY_NUMPAD6:
					dsky.NumberPressed(6);
					break;
				case OAPI_KEY_NUMPAD7:
					dsky.NumberPressed(7);
					break;
				case OAPI_KEY_NUMPAD8:
					dsky.NumberPressed(8);
					break;
				case OAPI_KEY_NUMPAD9:
					dsky.NumberPressed(9);
					break;
				case OAPI_KEY_NUMPAD0:
					dsky.NumberPressed(0);
					break;
				case OAPI_KEY_W: // Minimum impulse controller, pitch down
					agc.SetInputChannelBit(032, MinusPitchMinImpulse,1);
					break;
				case OAPI_KEY_S: // Minimum impulse controller, pitch up
					agc.SetInputChannelBit(032, PlusPitchMinImpulse,1);
					break;
				case OAPI_KEY_A: // Minimum impulse controller, yaw left
					agc.SetInputChannelBit(032, MinusYawMinimumImpulse,1);
					break;
				case OAPI_KEY_D: // Minimum impulse controller, yaw right
					agc.SetInputChannelBit(032, PlusYawMinimumImpulse,1);
					break;
				case OAPI_KEY_Q: // Minimum impulse controller, roll left
					agc.SetInputChannelBit(032, MinusRollMinimumImpulse,1);
					break;
				case OAPI_KEY_E: // Minimum impulse controller, roll right
					agc.SetInputChannelBit(032, PlusRollMinimumImpulse,1);
					break;
			}
		}else{
			// KEY UP
			switch(key){
				case OAPI_KEY_END:
					dsky.ProgReleased();
					break;
				case OAPI_KEY_W: // Minimum impulse controller, pitch down
					agc.SetInputChannelBit(032, MinusPitchMinImpulse, 0);
					break;
				case OAPI_KEY_S: // Minimum impulse controller, pitch up
					agc.SetInputChannelBit(032, PlusPitchMinImpulse, 0);
					break;
				case OAPI_KEY_A: // Minimum impulse controller, yaw left
					agc.SetInputChannelBit(032, MinusYawMinimumImpulse, 0);
					break;
				case OAPI_KEY_D: // Minimum impulse controller, yaw right
					agc.SetInputChannelBit(032, PlusYawMinimumImpulse, 0);
					break;
				case OAPI_KEY_Q: // Minimum impulse controller, roll left
					agc.SetInputChannelBit(032, MinusRollMinimumImpulse, 0);
					break;
				case OAPI_KEY_E: // Minimum impulse controller, roll right
					agc.SetInputChannelBit(032, PlusRollMinimumImpulse, 0);
					break;
			}
		}
		return 0;
	}
	if (KEYMOD_CONTROL(kstate) || KEYMOD_ALT(kstate)) {
		return 0; 
	}

	// OPTICS CONTROL
	if(down){
		switch(key){
			case OAPI_KEY_W: // Optics Up
				optics.OpticsManualMovement |= 0x01; 
				return 1;
			case OAPI_KEY_S: // Optics Down
				optics.OpticsManualMovement |= 0x02; 
				return 1;
			case OAPI_KEY_A: // Optics Left
				optics.OpticsManualMovement |= 0x04; 
				return 1;
			case OAPI_KEY_D: // Optics Right
				optics.OpticsManualMovement |= 0x08; 
				return 1;
			case OAPI_KEY_Q: // Optics Mark
				agc.SetInputChannelBit(016, Mark,1);
				return 1;
			case OAPI_KEY_E: // Optics Mark Reject
				agc.SetInputChannelBit(016, MarkReject,1);
				return 1;
			case OAPI_KEY_V: // Change Sextant View Mode to DualView
				optics.SextDualView = !optics.SextDualView;
				return 1;
		}
	}else{
		switch(key){
			case OAPI_KEY_W: 
				optics.OpticsManualMovement &= 0xFE; 
				return 1;
			case OAPI_KEY_S: 
				optics.OpticsManualMovement &= 0xFD; 
				return 1;
			case OAPI_KEY_A: 
				optics.OpticsManualMovement &= 0xFB; 
				return 1;
			case OAPI_KEY_D: 
				optics.OpticsManualMovement &= 0xF7; 
				return 1;
			case OAPI_KEY_Q: 
				agc.SetInputChannelBit(016,Mark,0);
				return 1;
			case OAPI_KEY_E: 
				agc.SetInputChannelBit(016,MarkReject,0);
				return 1;
		}
	}

	// MCC CAPCOM interface key handling                                                                                                
	if(down && !InVC && !KEYMOD_SHIFT(kstate)){
		switch(key){
		case OAPI_KEY_TAB:
		case OAPI_KEY_1:
		case OAPI_KEY_2:
		case OAPI_KEY_3:
		case OAPI_KEY_4:
		case OAPI_KEY_5:
		case OAPI_KEY_6:
		case OAPI_KEY_7:
		case OAPI_KEY_8:
		case OAPI_KEY_9:
		case OAPI_KEY_0:
			mcc.keyDown(key);
			break;
		}
	}

	if (key == OAPI_KEY_9 && down == true && InVC && (stage == CSM_LEM_STAGE || stage == CM_RECOVERY_STAGE)) {
		if (viewpos == SATVIEW_LEFTDOCK){
			viewpos = SATVIEW_RIGHTDOCK;
		}else{
			viewpos = SATVIEW_LEFTDOCK;
		}
		SetView(true);
		return 1;
	}

	if (key == OAPI_KEY_8 && down == true && InVC ) {
		viewpos = SATVIEW_RIGHTSEAT;
		SetView(true);
		return 1;
	}

	if (key == OAPI_KEY_7 && down == true && InVC ) {
		viewpos = SATVIEW_CENTERSEAT;
		SetView(true);
		return 1;
	}

	if (key == OAPI_KEY_6 && down == true && InVC ) {
		viewpos = SATVIEW_LEFTSEAT;
		SetView(true);
		return 1;
	}

	if (key == OAPI_KEY_5 && down == true && InVC ) {
		viewpos = SATVIEW_GNPANEL;
		SetView(true);
		return 1;
	}


	//
	// We only allow this switch in VC mode, as we need to disable the panel when selecting these
	// cameras.
	//
	// For now this is limited to the Saturn V.
	//

	if (key == OAPI_KEY_1 && down == true && InVC && iu.IsTLICapable() && stage < LAUNCH_STAGE_TWO && stage >= LAUNCH_STAGE_ONE) {
		viewpos = SATVIEW_ENG1;
		SetView();
		oapiCameraAttach(GetHandle(), CAM_COCKPIT);
		return 1;
	}

	if (key == OAPI_KEY_2 && down == true && InVC && iu.IsTLICapable() && stage < LAUNCH_STAGE_SIVB && stage >= LAUNCH_STAGE_ONE) {
		viewpos = SATVIEW_ENG2;
		oapiCameraAttach(GetHandle(), CAM_COCKPIT);
		SetView();
		return 1;
	}

	if (key == OAPI_KEY_3 && down == true && InVC && iu.IsTLICapable() && stage < LAUNCH_STAGE_SIVB && stage >= PRELAUNCH_STAGE)
	{
		//
		// Key 3 switches to position 3 by default, then cycles around them.
		//
		switch (viewpos)
		{
		case SATVIEW_ENG3:
			viewpos = SATVIEW_ENG4;
			break;

		case SATVIEW_ENG4:
			viewpos = SATVIEW_ENG5;
			break;

		case SATVIEW_ENG5:
			viewpos = SATVIEW_ENG6;
			break;

		case SATVIEW_ENG6:
			viewpos = SATVIEW_ENG3;
			break;

		default:
			viewpos = SATVIEW_ENG3;
			break;
		}
		oapiCameraAttach(GetHandle(), CAM_COCKPIT);
		SetView();
		return 1;
	}
	return 0;
}

void Saturn::AddRCSJets(double TRANZ, double MaxThrust)

{
	int i;
	const double ATTCOOR = 0;
	const double ATTCOOR2 = 2.05; 
	const double ATTCOOR3 = 2.10; // Was 2.05
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.25;
	const double RCSOFFSET2=-0.25;
	const double RCSOFFSETM=-0.05;
	const double RCSOFFSETM2=0.02; // Was 0.05

	//
	// Clear any old thrusters.
	//

	for (i = 0; i < 24; i++) {
		th_att_lin[i] = 0;
		th_att_rot[i] = 0;
	}

	double RCS_ISP = SM_RCS_ISP;
	double RCS_Thrust = MaxThrust;

	//
	// SM RCS Propellant tanks
	//

	//
	// RCS0 = quad A
	// RCS1 = quad B
	// RCS2 = quad C
	// RCS3 = quad D
	//

	if (!ph_rcs0)
		ph_rcs0  = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcs1)
		ph_rcs1  = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcs2)
		ph_rcs2  = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcs3)
		ph_rcs3  = CreatePropellantResource(RCS_FUEL_PER_QUAD);

	const double CENTEROFFS  = 0.25; 

	th_att_lin[0]=th_att_rot[0]=CreateThruster (_V(-CENTEROFFS,ATTCOOR2,TRANZ+RCSOFFSET2), _V(0,-0.1,1), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[1]=th_att_rot[3]=CreateThruster (_V(CENTEROFFS,-ATTCOOR2,TRANZ+RCSOFFSET2), _V(0,0.1,1), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[2]=th_att_rot[4]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS,TRANZ+RCSOFFSET2), _V(0.1,0,1), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[3]=th_att_rot[7]=CreateThruster (_V(ATTCOOR2,CENTEROFFS,TRANZ+RCSOFFSET2), _V(-0.1,0,1), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[4]=th_att_rot[2]=CreateThruster (_V(-CENTEROFFS,ATTCOOR2,TRANZ+RCSOFFSET), _V(0,-0.1,-1), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[5]=th_att_rot[1]=CreateThruster (_V(CENTEROFFS,-ATTCOOR2,TRANZ+RCSOFFSET), _V(0,0.1,-1), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[6]=th_att_rot[6]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS,TRANZ+RCSOFFSET), _V(0.1,0,-1), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[7]=th_att_rot[5]=CreateThruster (_V(ATTCOOR2,CENTEROFFS,TRANZ+RCSOFFSET), _V(-0.1,0,-1), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[8]=th_att_rot[16]=th_att_rot[17]=CreateThruster (_V(-CENTEROFFS - 0.2,ATTCOOR2,TRANZ+RCSOFFSETM), _V(1,-0.1,0), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[9]=th_att_rot[8]=th_att_rot[9]=CreateThruster (_V(CENTEROFFS -0.2,-ATTCOOR3,TRANZ+RCSOFFSETM2), _V(1,0.1,0), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[12]=th_att_rot[10]=th_att_rot[11]=CreateThruster (_V(-CENTEROFFS + 0.2,ATTCOOR3,TRANZ+RCSOFFSETM2), _V(-1,-0.1,0), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[13]=th_att_rot[18]=th_att_rot[19]=CreateThruster (_V(CENTEROFFS + 0.2,-ATTCOOR2,TRANZ+RCSOFFSETM), _V(-1,0.1,0), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[16]=th_att_rot[14]=th_att_rot[15]=CreateThruster (_V(ATTCOOR3,CENTEROFFS -0.2,TRANZ+RCSOFFSETM2), _V(-0.1,1,0), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[17]=th_att_rot[22]=th_att_rot[23]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS -0.2,TRANZ+RCSOFFSETM), _V(-0.1,1,0), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[20]=th_att_rot[20]=th_att_rot[21]=CreateThruster (_V(ATTCOOR2,CENTEROFFS + 0.2,TRANZ+RCSOFFSETM), _V(-0.1,-1,0), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[21]=th_att_rot[12]=th_att_rot[13]=CreateThruster (_V(-ATTCOOR3,-CENTEROFFS + 0.2,TRANZ+RCSOFFSETM2), _V(0.1,-1,0), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);

	for (i = 0; i < 24; i++) {
		if (th_att_lin[i])
			AddExhaust (th_att_lin[i], 1.2, 0.1, SMExhaustTex); 
	}

	//
	// Map thrusters to RCS quads. Note that we don't use entry zero, we're matching the array to
	// Apollo numbering for simplicity... we also have to include the fake thrusters here so we
	// can enable and disable them.
	//

	th_rcs_a[1] = th_att_rot[16];
	th_rcs_a[2] = th_att_rot[10];
	th_rcs_a[3] = th_att_rot[2];
	th_rcs_a[4] = th_att_rot[0];

	th_rcs_b[1] = th_att_rot[20];
	th_rcs_b[2] = th_att_rot[14];
	th_rcs_b[3] = th_att_rot[5];
	th_rcs_b[4] = th_att_rot[7];

	th_rcs_c[1] = th_att_rot[18];
	th_rcs_c[2] = th_att_rot[8];
	th_rcs_c[3] = th_att_rot[3];
	th_rcs_c[4] = th_att_rot[1];

	th_rcs_d[1] = th_att_rot[22];
	th_rcs_d[2] = th_att_rot[12];
	th_rcs_d[3] = th_att_rot[4];
	th_rcs_d[4] = th_att_rot[6];
}

void Saturn::AddRCS_CM(double MaxThrust, double offset, bool createThrusterGroups)

{	
	const double ATTCOOR = 0.95;
	const double ATTCOOR2 = 1.92;
	const double TRANZ = -0.65 + offset;

	static PARTICLESTREAMSPEC cmrcsdump_spec = {
		0,		// flag
		0.01,	// size
		30,     // rate
		1,	    // velocity
		1,      // velocity distribution
		0.5,    // lifetime
		0.5, 	// growthrate
		1.25,   // atmslowdown 
		PARTICLESTREAMSPEC::DIFFUSE,
		PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
		PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
	};
	SURFHANDLE tex = oapiRegisterExhaustTexture("ProjectApollo/CMRCSDump");
	cmrcsdump_spec.tex = tex;

	if (!ph_rcs_cm_1)
		ph_rcs_cm_1 = CreatePropellantResource(CM_RCS_FUEL_PER_TANK); 
	if (!ph_rcs_cm_2)
		ph_rcs_cm_2 = CreatePropellantResource(CM_RCS_FUEL_PER_TANK);

	//
	// display rcs stage propellant level in generic HUD
	//

	if (stage > CSM_LEM_STAGE)
		SetDefaultPropellantResource (ph_rcs_cm_1);

	double RCS_ISP = CM_RCS_ISP;
	double RCS_Thrust = MaxThrust;

	for (int i = 0; i < 12; i++) {
		if (cmrcsdump[i]) 
			DelExhaustStream(cmrcsdump[i]);
	}

	// For thrusters - X means LEFT/RIGHT, Y means IN/OUT, Z means UP/DOWN)

	// Jet #1 
	th_att_cm[0] = CreateThruster (_V(0.09, ATTCOOR2 - 0.07, TRANZ), _V(0, -0.81, -0.59), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);	
	cmrcsdump[0] = AddParticleStream(&cmrcsdump_spec, _V(0.09, ATTCOOR2 - 0.07, TRANZ), _V(0, 0.81, 0.59), CMRCS1.GetPurgeLevelRef(0));	
	AddExhaust(th_att_cm[0],1.0,0.1, SMExhaustTex);

	// Jet #3 
	th_att_cm[1] = CreateThruster (_V(-0.09, ATTCOOR2 - 0.07, TRANZ), _V(0, -0.81, -0.59), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[0] = AddParticleStream(&cmrcsdump_spec, _V(-0.09, ATTCOOR2 - 0.07, TRANZ), _V(0, 0.81, 0.59), CMRCS2.GetPurgeLevelRef(0));	
	AddExhaust(th_att_cm[1],1.0,0.1, SMExhaustTex);

	// Jet #2
	th_att_cm[2] = CreateThruster (_V(0.09, ATTCOOR + 0.05, TRANZ + 1.45), _V(0, -1, 0), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[2] = AddParticleStream(&cmrcsdump_spec, _V(0.09, ATTCOOR + 0.05, TRANZ + 1.45), _V(0, 1, 0), CMRCS1.GetPurgeLevelRef(1));	
	AddExhaust(th_att_cm[2], 1.0, 0.1, SMExhaustTex);
	// Jet #4
	th_att_cm[3] = CreateThruster (_V(-0.09, ATTCOOR + 0.05, TRANZ + 1.45), _V(0, -1, 0), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[3] = AddParticleStream(&cmrcsdump_spec, _V(-0.09, ATTCOOR + 0.05, TRANZ + 1.45), _V(0, 1, 0), CMRCS2.GetPurgeLevelRef(1));	
	AddExhaust(th_att_cm[3], 1.0, 0.1, SMExhaustTex); 

	// Jet #5 
	th_att_cm[4] = CreateThruster(_V(ATTCOOR2 - 0.06,  0.05, TRANZ), _V(-0.81, 0, -0.59), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[4] = AddParticleStream(&cmrcsdump_spec, _V(ATTCOOR2 - 0.06,  0.05, TRANZ), _V(0.81, 0, 0.59), CMRCS1.GetPurgeLevelRef(2));	
	AddExhaust(th_att_cm[4], 1.0, 0.1, SMExhaustTex); 
	// Jet #7
	th_att_cm[5] = CreateThruster(_V(ATTCOOR2 - 0.06, -0.12, TRANZ), _V(-0.81, 0, -0.59), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[5] = AddParticleStream(&cmrcsdump_spec, _V(ATTCOOR2 - 0.06, -0.12, TRANZ), _V(0.81, 0, 0.59), CMRCS2.GetPurgeLevelRef(2));	
	AddExhaust(th_att_cm[5], 1.0, 0.1, SMExhaustTex); 

	// Jet #6 
	th_att_cm[6] = CreateThruster(_V(-ATTCOOR2 + 0.07, 0.05, TRANZ), _V(0.81, 0, -0.59), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[6] = AddParticleStream(&cmrcsdump_spec, _V(-ATTCOOR2 + 0.07, 0.05, TRANZ), _V(-0.81, 0, 0.59), CMRCS2.GetPurgeLevelRef(3));	
	AddExhaust(th_att_cm[6], 1.0, 0.1, SMExhaustTex);
	// Jet #8
	th_att_cm[7] = CreateThruster(_V(-ATTCOOR2 + 0.07, -0.13, TRANZ), _V(0.81, 0, -0.59), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[7] = AddParticleStream(&cmrcsdump_spec, _V(-ATTCOOR2 + 0.07, -0.13, TRANZ), _V(-0.81, 0, 0.59), CMRCS1.GetPurgeLevelRef(3));	
	AddExhaust(th_att_cm[7], 1.0, 0.1, SMExhaustTex); 

	// The roll jets introduce a slight upward pitch if not corrected for.
	// Apparently the AGC expects this.

	// Jet #9
	th_att_cm[8] = CreateThruster(_V(ATTCOOR2/1.42, ATTCOOR2/1.42 - 0.2, TRANZ + 0.11), _V(0.17, -0.98, 0.21), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[8] = AddParticleStream(&cmrcsdump_spec, _V(ATTCOOR2/1.42, ATTCOOR2/1.42 - 0.2, TRANZ + 0.11), _V(-0.17, 0.98, -0.21), CMRCS1.GetPurgeLevelRef(4));	
	AddExhaust(th_att_cm[8], 1.0, 0.1, SMExhaustTex); 
	// Jet #11
	th_att_cm[9] = CreateThruster(_V(-ATTCOOR2/1.42 + 0.12, (ATTCOOR2/1.42) - 0.07, TRANZ + 0.11), _V(0.98, 0.17, 0.21), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[9] = AddParticleStream(&cmrcsdump_spec, _V(-ATTCOOR2/1.42 + 0.12, (ATTCOOR2/1.42) - 0.07, TRANZ + 0.11), _V(-0.98, -0.17, -0.21), CMRCS2.GetPurgeLevelRef(4));	
	AddExhaust(th_att_cm[9], 1.0, 0.1, SMExhaustTex); 
		
	// Jet #10 
	th_att_cm[10] = CreateThruster(_V(-ATTCOOR2/1.42 + 0.04, ATTCOOR2/1.42 - 0.16, TRANZ + 0.11), _V(-0.17, -0.98, 0.21), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[10] = AddParticleStream(&cmrcsdump_spec, _V(-ATTCOOR2/1.42 + 0.04, ATTCOOR2/1.42 - 0.16, TRANZ + 0.11), _V(0.17, 0.98, -0.21), CMRCS2.GetPurgeLevelRef(5));	
	AddExhaust(th_att_cm[10], 1.0, 0.1, SMExhaustTex);
	// Jet #12
	th_att_cm[11] = CreateThruster(_V(ATTCOOR2/1.42 - 0.08, (ATTCOOR2/1.42) - 0.11, TRANZ + 0.11), _V(-0.98, 0.17, 0.21), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	cmrcsdump[11] = AddParticleStream(&cmrcsdump_spec, _V(ATTCOOR2/1.42 - 0.08, (ATTCOOR2/1.42) - 0.11, TRANZ + 0.11), _V(0.98, -0.17, -0.21), CMRCS1.GetPurgeLevelRef(5));	
	AddExhaust(th_att_cm[11], 1.0, 0.1, SMExhaustTex); 

	//
	// Thruster helper arrays per system
	//

	th_att_cm_sys1[0] = th_att_cm[0];
	th_att_cm_sys1[1] = th_att_cm[2];
	th_att_cm_sys1[2] = th_att_cm[4];
	th_att_cm_sys1[3] = th_att_cm[7];
	th_att_cm_sys1[4] = th_att_cm[8];
	th_att_cm_sys1[5] = th_att_cm[11];

	th_att_cm_sys2[0] = th_att_cm[1];
	th_att_cm_sys2[1] = th_att_cm[3];
	th_att_cm_sys2[2] = th_att_cm[5];
	th_att_cm_sys2[3] = th_att_cm[6];
	th_att_cm_sys2[4] = th_att_cm[9];
	th_att_cm_sys2[5] = th_att_cm[10];
}

void Saturn::AddRCS_S4B()

{
	const double ATTCOOR = -10;
	const double ATTCOOR2 = 3.61;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double TRANZ=-3.2-STG2O;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;
	const double RCSX=0.35;
	VECTOR3 m_exhaust_pos2= {0,ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos3= {0,-ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos4= {-ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_pos5= {ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_ref2 = {0,0.1,-1};
	VECTOR3 m_exhaust_ref3 = {0,-0.1,-1};
	VECTOR3 m_exhaust_ref4 = {-0.1,0,-1};
	VECTOR3 m_exhaust_ref5 = {0.1,0,-1};

	double offset = 0.0;
	if (SaturnType == SAT_SATURN1B)
		offset=7.7;

	th_att_rot[0] = CreateThruster(_V(0, ATTCOOR2 + 0.15, TRANZ - 0.25 + offset), _V(0, -1, 0), SIVB_RCS_PITCH_THRUST, ph_3rd,5000000, 4000000);
	th_att_rot[1] = CreateThruster(_V(0, -ATTCOOR2 - 0.15, TRANZ - 0.25 + offset), _V(0, 1, 0), SIVB_RCS_PITCH_THRUST, ph_3rd,5000000, 4000000);
	
	AddExhaust (th_att_rot[0], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[1], 0.6, 0.078, SIVBRCSTex);

	th_att_rot[2] = CreateThruster (_V(RCSX,ATTCOOR2-0.2,TRANZ-0.25+offset), _V(-1,0,0),17400.0, ph_3rd,250000, 240000);
	th_att_rot[3] = CreateThruster (_V(-RCSX,-ATTCOOR2+0.2,TRANZ-0.25+offset), _V( 1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[4] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V( 1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[5] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0),17400.0, ph_3rd,250000, 240000);

	AddExhaust (th_att_rot[2], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[3], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[4], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[5], 0.6, 0.078, SIVBRCSTex);

	th_att_rot[6] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[7] = CreateThruster (_V(-RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[8] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[9] = CreateThruster (_V(RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(-1,0,0), 17400.0, ph_3rd,250000, 240000);
		
	AddExhaust (th_att_rot[6], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[7], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[8], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[9], 0.6, 0.078, SIVBRCSTex);

	//
	// APS thrusters are only 320N (72 pounds) thrust
	//

	th_att_lin[0] = CreateThruster (_V(0,ATTCOOR2-0.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_3rd,250000, 240000);
	th_att_lin[1] = CreateThruster (_V(0,-ATTCOOR2+.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_3rd,250000, 240000);
	AddExhaust (th_att_lin[0], 7, 0.15, SIVBRCSTex);
	AddExhaust (th_att_lin[1], 7, 0.15, SIVBRCSTex);

	thg_aps = CreateThrusterGroup (th_att_lin, 2, THGROUP_USER);
}

void Saturn::SetSaturnAttitudeRotLevel(VECTOR3 th) {

	if ((stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)) {
		if (th_att_rot[0] != 0) {
			if (th.x >= 0) {
				SetThrusterLevel(th_att_rot[0], th.x);
				SetThrusterLevel(th_att_rot[1], 0);
			} else {
				SetThrusterLevel(th_att_rot[0], 0);
				SetThrusterLevel(th_att_rot[1], -th.x);
			}
			if (th.y >= 0) {
				SetThrusterLevel(th_att_rot[6], th.y);
				SetThrusterLevel(th_att_rot[7], th.y);
				SetThrusterLevel(th_att_rot[8], 0);
				SetThrusterLevel(th_att_rot[9], 0);
			} else {
				SetThrusterLevel(th_att_rot[6], 0);
				SetThrusterLevel(th_att_rot[7], 0);
				SetThrusterLevel(th_att_rot[8], -th.y);
				SetThrusterLevel(th_att_rot[9], -th.y);
			}
			if (th.z >= 0) {
				SetThrusterLevel(th_att_rot[5], th.z);
				SetThrusterLevel(th_att_rot[4], th.z);
				SetThrusterLevel(th_att_rot[3], 0);
				SetThrusterLevel(th_att_rot[2], 0);
			} else {
				SetThrusterLevel(th_att_rot[5], 0);
				SetThrusterLevel(th_att_rot[4], 0);
				SetThrusterLevel(th_att_rot[3], -th.z);
				SetThrusterLevel(th_att_rot[2], -th.z);
			}
		}
	} else {
		SetAttitudeRotLevel(th);
	}
}

double Saturn::GetSaturnMaxThrust(ENGINETYPE eng) {

	if (stage == STAGE_ORBIT_SIVB && eng == ENGINE_ATTITUDE) { 
		// thrust of the THGROUP_ATT_PITCHUP thruster (Orbiter API manual)
		return SIVB_RCS_PITCH_THRUST;
	} else {
		return GetMaxThrust(eng);
	}
}

void Saturn::FireSeperationThrusters(THRUSTER_HANDLE *pth)

{
	int i;
	for (i = 0; i < 8; i++)
	{
		if (pth[i])
			SetThrusterLevel(pth[i], 1.0);
	}
}

// ==============================================================
// DLL entry point
// ==============================================================

BOOL WINAPI DllMain (HINSTANCE hModule,
					 DWORD ul_reason_for_call,
					 LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		SetupgParam(hModule);
		InitCollisionSDK();
		break;

	case DLL_PROCESS_DETACH:
		DeletegParam();
		break;
	}
	return TRUE;
}

void Saturn::LaunchCountdown(double simt)
{
	if (GetEngineLevel(ENGINE_MAIN) > 0 && MissionTime <= (-8.9)) {
		SetThrusterGroupLevel(thg_main, 0);
		contrailLevel = 0;
	}

	if (oapiGetTimeAcceleration() > 100)
		oapiSetTimeAcceleration(100);

	if (MissionTime >= 0) {
		DoLaunch(simt);
		return;
	}

	// Prelaunch tank venting between -3:00h and engine ignition
	// No clue if the venting start time is correct
	if (MissionTime < -10800 || MissionTime > -9) {
		DeactivatePrelaunchVenting();
	}
	else {
		ActivatePrelaunchVenting();
	}

	switch (StageState) {

	case 0:
		if (MissionTime >= -((4 * 60) + 10)) {
			//
			// Engine lights on.
			//

			SetEngineIndicators();
			StageState++;
		}
		break;

	case 1:

		//
		// Reset time acceleration to normal at
		// 20 seconds, and reconnect the fuel to
		// the main engines.
		//

		if (MissionTime >= -20.0) {
			oapiSetTimeAcceleration (1);
			for (int i = 0; i < 5; i++) {
				SetThrusterResource(th_main[i], ph_1st);
			}
			CreateStageOne();
			StageState++;
		}
		break;

	case 2:

		//
		// Play the countdown sound at 10 seconds,
		//

		if (MissionTime >= -10.9) {
			if (!UseATC && Scount.isValid()) {
				Scount.play();
				Scount.done();
			}
			StageState++;
		}
		break;

	case 3:
		if (MissionTime >= -4.9) {
			StageState++;
		}
		break;

	case 4:
		//
		// Build up engine thrust. Slower at first so we don't
		// leave the ground before we should.
		//

		double thrst;

		if (MissionTime > (-2.0)) {
			thrst = 0.9 + (0.05 * (MissionTime + 2.0));

			//
			// Engine lights off. This should really be done per-engine,
			// based on thrust level.
			//
			for (int i = 1; i <= SI_EngineNum; i++)
			{
				ClearEngineIndicator(i);
			}
		}
		else
		{
			thrst = (0.9 / 2.9) * (MissionTime + 4.9);
		}
		SetThrusterGroupLevel(thg_main, thrst);
		contrailLevel = thrst;

		double amt = (thrst) * 0.1;
		JostleViewpoint(amt);

		// AddForce(_V(0, 0, -10. * THRUST_FIRST_VAC), _V(0, 0, 0));
		AddForce(_V(0, 0, -(THRUST_FIRST_VAC*(thrst + .01))), _V(0, 0, 0));
		break;
	}
}

void Saturn::GenericTimestepStage(double simt, double simdt)

{
	//
	// Do stage-specific processing.
	//

	bool deploy = false;

	switch (stage) {
	case PRELAUNCH_STAGE:
		LaunchCountdown(simt);
		break;

	case CSM_LEM_STAGE:
		StageSix(simt);
		break;

	case CM_STAGE:
		if (ELSAuto() && GetAtmPressure() > 37680 && !LandFail.CoverFail) {
			// Deactivate Auto RCS Enable Relays
			rjec.SetAutoRCSEnableRelayA(false);
			rjec.SetAutoRCSEnableRelayB(false);
			
			// Deploy apex cover
			deploy = true;
		}

		if (ELSActive() && ApexCoverJettSwitch.GetState())
			deploy = true;

		if (deploy && PyrosArmed()) {
			StageEight(simt);
			ShiftCentreOfMass(_V(0, 0, 1.2));

		} else {
			// DS20070622 Do not run stage seven if we still have the LET.
			if (!LESAttached) { 
				StageSeven(simt); 
			}
		}
		break;

	case CM_ENTRY_STAGE:
		if (ELSAuto() && GetAtmPressure() > 37680 && !LandFail.DrogueFail) {
			// Deactivate Auto RCS Enable Relays
			rjec.SetAutoRCSEnableRelayA(false);
			rjec.SetAutoRCSEnableRelayB(false);
			
			// Deploy apex cover
			deploy = true;
		}

		if (ELSActive() && ApexCoverJettSwitch.GetState())
			deploy = true;

		if (deploy && PyrosArmed())	{
			StageEight(simt);
			ShiftCentreOfMass(_V(0, 0, 1.2));
		}
		// sprintf(oapiDebugString(), "AtmPressure %f (37680)", GetAtmPressure());
		break;

	case CM_ENTRY_STAGE_TWO:
		if (ELSAuto() && GetAtmPressure() > 39000 && !LandFail.DrogueFail && ChutesAttached)
			deploy = true;

		if (ELSActive() && DrogueDeploySwitch.GetState()) 
			deploy = true;

		if (deploy && PyrosArmed()) {
			DrogueS.play();
			DrogueS.done();

			SetChuteStage1();
			SetStage(CM_ENTRY_STAGE_THREE);

			// Create the drogue chute vessel
			VESSELSTATUS vs;
			GetStatus(vs);
			char VName[256]="";
			GetApolloName(VName);
			strcat(VName, "-DROGUECHUTE");
			hDrogueChute = oapiCreateVessel(VName, "ProjectApollo/DrogueChute", vs);
			VESSEL *drogueChute = oapiGetVesselInterface(hDrogueChute);
			ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 1);
			ATTACHMENTHANDLE ahc = drogueChute->GetAttachmentHandle(true, 0);
			AttachChild(hDrogueChute, ah, ahc);
		}
		break;

	//
	// We should really make the time taken to open the main chutes based on
	// the dynamic air pressure.
	//

	case CM_ENTRY_STAGE_THREE:	// Drogue chute is attached
		if (PyrosArmed() && ChutesAttached && ((ELSAuto() && GetAtmPressure() > 66000 && !LandFail.MainFail) || (ELSActive() && MainDeploySwitch.GetState()))) {
			// Detach drogue
			ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 1);
			DetachChild(ah, 1);

			SetChuteStage2();			  
			SetStage(CM_ENTRY_STAGE_FOUR); 

			// Create and attach the main chute vessel
			VESSELSTATUS vs;
			GetStatus(vs);
			char VName[256]="";
			GetApolloName(VName);
			strcat(VName, "-MAINCHUTE");
			hMainChute = oapiCreateVessel(VName, "ProjectApollo/MainChute", vs);
			VESSEL *mainChute = oapiGetVesselInterface(hMainChute);
			ATTACHMENTHANDLE ahc = mainChute->GetAttachmentHandle(true, 0);
			AttachChild(hMainChute, ah, ahc);

			NextMissionEventTime = MissionTime + 1.;
		}
		// Landing
		if (GetAltitude() < 2.5) {
			// Detach drogue
			ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 1);
			DetachChild(ah);

			// Move to splash stage
			SetSplashStage();
			SetStage(CM_ENTRY_STAGE_SEVEN);
			soundlib.LoadSound(Swater, WATERLOOP_SOUND);
		}
		// sprintf(oapiDebugString(), "Altitude %.1f", GetAltitude());
		break;

	case CM_ENTRY_STAGE_FOUR:
		if (ChutesAttached && (MissionTime >= NextMissionEventTime)) {
			SetChuteStage3();
			SetStage(CM_ENTRY_STAGE_FIVE);
			NextMissionEventTime = MissionTime + 6.;
		}
		break;

	case CM_ENTRY_STAGE_FIVE:
		if (ChutesAttached && (MissionTime >= NextMissionEventTime)) {
			SetChuteStage4();
			SetStage(CM_ENTRY_STAGE_SIX);
		}
		break;

	case CM_ENTRY_STAGE_SIX:	// Main chute is attached		
		if (!SplashdownPlayed && GetAltitude() < 2.5) {
			SplashS.play(NOLOOP, 180);
			SplashS.done();

			SplashdownPlayed = true;
			NextMissionEventTime = MissionTime + 25.;

			//
			// Event handler
			//
			eventControl.SPLASHDOWN = MissionTime;
		}
		if ((MainReleasePyroACircuitBraker.IsPowered() || MainReleasePyroBCircuitBraker.IsPowered()) && ChutesAttached && ELSActive() && MainReleaseSwitch.IsUp()) {
			// Detach Main 
			ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 1);
			if (GetAttachmentStatus(ah) != NULL) {
				DetachChild(ah);
				if (hMainChute) {
					VESSEL *mainChute = oapiGetVesselInterface(hMainChute);
					double F = mainChute->GetMass() * 10. / simdt;
					mainChute->AddForce(_V(0, 0, F), _V(0,0,0));
				}
			}		
			SetSplashStage();
			SetStage(CM_ENTRY_STAGE_SEVEN);
			soundlib.LoadSound(Swater, WATERLOOP_SOUND);

		}
		//sprintf(oapiDebugString(), "Altitude %.1f", GetAltitude());
		break;

	case CM_ENTRY_STAGE_SEVEN:	// Main chute is detached	
		if (SideHatch.IsOpen()) {
			if (!Swater.isPlaying())
				Swater.play(LOOP);
		} else {
			Swater.stop();
		}

		if (bRecovery){
			SetRecovery();
			bRecovery = false;
			soundlib.LoadMissionSound(PostSplashdownS, POSTSPLASHDOWN_SOUND, POSTSPLASHDOWN_SOUND);
			NextMissionEventTime = MissionTime + 10.0;
			SetStage(CM_RECOVERY_STAGE);
		}
		break;

	case CM_RECOVERY_STAGE:
		if (!Swater.isPlaying())
			Swater.play(LOOP);

		if (!PostSplashdownPlayed && MissionTime >= NextMissionEventTime) {
			PostSplashdownS.play();
			PostSplashdownS.done();
			PostSplashdownPlayed = true;
		}
		break;
	}
}

void Saturn::LoadTLISounds()

{
	soundlib.LoadMissionSound(STLI, GO_FOR_TLI_SOUND, NULL);
	soundlib.LoadMissionSound(STLIStart, TLI_START_SOUND, NULL);
	soundlib.LoadMissionSound(SecoSound, SECO_SOUND, SECO_SOUND);

	TLISoundsLoaded = true;
}

void Saturn::ClearTLISounds()

{
	STLI.done();
	STLIStart.done();
	SecoSound.done();

	TLISoundsLoaded = false;
}

void Saturn::GenericLoadStateSetup()

{
	//
	// Some switches only work in the CSM/LEM stage. Disable them
	// otherwise.
	//

	if (stage != CSM_LEM_STAGE)
	{
		OrbiterAttitudeToggle.SetActive(false);
	}
	else
	{
		OrbiterAttitudeToggle.SetActive(true);
	}

	//
	// Set up connectors.
	//

	if (stage >= CSM_LEM_STAGE)
	{
		CSMToSIVBConnector.AddTo(&iuCommandConnector);
		CSMToSIVBConnector.AddTo(&sivbControlConnector);
	}
	else
	{
		iu.ConnectToCSM(&iuCommandConnector);
		iu.ConnectToLV(&sivbCommandConnector);
	}

	CSMToSIVBConnector.AddTo(&SIVBToCSMPowerConnector);
	CSMToLEMConnector.AddTo(&CSMToLEMPowerConnector);

	//
	// Disable cabin fans.
	//

	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);

	//
	// We do our own countdown, so ignore the standard one.
	//

	if (!UseATC)
		soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);

	//
	// Load mission-based sound files. Some of these are just being
	// preloaded here for the CSM computer.
	//

	char MissionName[24];

	_snprintf(MissionName, 23, "Apollo%d", ApolloNo);
	soundlib.SetSoundLibMissionPath(MissionName);

    timedSounds.LoadFromFile("csmsound.csv", MissionTime);

	//
	// Set up options for prelaunch stage.
	//

	if (MissionTime < 0) {

		//
		// Open the countdown sound file.
		//

		if (!UseATC) {
			soundlib.LoadMissionSound(LaunchS, LAUNCH_SOUND, LAUNCH_SOUND);
			if (SaturnType == SAT_SATURNV)
				soundlib.LoadMissionSound(CabincloseoutS, CABINCLOSEOUT_SOUND, CABINCLOSEOUT_SOUND);
			LaunchS.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
		}
	}

	//
	// Load the window sound if the launch escape tower is attached.
	//

	if (LESAttached)
		soundlib.LoadMissionSound(SwindowS, WINDOW_SOUND, POST_TOWER_JET_SOUND);

	//
	// Only the CSM and LEM have translational thrusters, so disable the message
	// telling us that they're being switched in other stages.
	//

	if (stage != CSM_LEM_STAGE)
	{
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, FALSE);
	}
	else
	{
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
	}

	if (stage < LAUNCH_STAGE_TWO) {
		soundlib.LoadMissionSound(SShutS, SI_CUTOFF_SOUND, SISHUTDOWN_SOUND);
	}

	if (stage < STAGE_ORBIT_SIVB) {

		//
		// We'll do our own radio playback during launch.
		//

		if (!UseATC)
			soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);

		soundlib.LoadMissionSound(S2ShutS, SII_CUTOFF_SOUND, SIISHUTDOWN_SOUND);
		soundlib.LoadMissionSound(S4CutS, GO_FOR_ORBIT_SOUND, SIVBSHUTDOWN_SOUND);

		SwindowS.setFlags(SOUNDFLAG_COMMS);
		S4CutS.setFlags(SOUNDFLAG_COMMS);
		S2ShutS.setFlags(SOUNDFLAG_COMMS);
	}

	if (stage < CSM_LEM_STAGE) {
		soundlib.LoadSound(TowerJS, TOWERJET_SOUND);
		soundlib.LoadSound(SepS, SEPMOTOR_SOUND, INTERNAL_ONLY);
		soundlib.LoadMissionSound(Scount, LAUNCH_COUNT_10_SOUND, DEFAULT_LAUNCH_COUNT_SOUND);
		Scount.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
	}

	if (!TLISoundsLoaded)
	{
		LoadTLISounds();
	}

	if (stage <= CSM_LEM_STAGE) {
		soundlib.LoadMissionSound(SMJetS, SM_SEP_SOUND, DEFAULT_SM_SEP_SOUND);
	}

	if (stage >= CM_ENTRY_STAGE_SEVEN)
	{
		soundlib.LoadSound(Swater, WATERLOOP_SOUND);
		soundlib.LoadMissionSound(PostSplashdownS, POSTSPLASHDOWN_SOUND, POSTSPLASHDOWN_SOUND);
	}

	//
	// Load Apollo-13 specific sounds.
	//

	if (ApolloNo == 13) {
		if (!KranzPlayed)
			soundlib.LoadMissionSound(SKranz, A13_KRANZ, NULL, INTERNAL_ONLY);
		if (!CryoStir)
			soundlib.LoadMissionSound(SApollo13, A13_CRYO_STIR, NULL);
		if (!ApolloExploded)
			soundlib.LoadMissionSound(SExploded, A13_PROBLEM, NULL);

		if (stage <= CSM_LEM_STAGE) {
			soundlib.LoadMissionSound(SSMSepExploded, A13_SM_SEP_SOUND, NULL);
		}

		SKranz.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
		SApollo13.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
		SExploded.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
	}

	//
	// Turn off the timer display on launch.
	//
	
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	//
	// Turn off docking sound
	//
	
	soundlib.SoundOptionOnOff(PLAYDOCKINGSOUND, FALSE);

	//
	// Initialize the IU
	//

	iu.SetMissionInfo(TLICapableBooster, Crewed, SIVBBurnStart, SIVBApogee); 

	//
	// Disable master alarm sound on unmanned flights.
	//

	cws.SetPlaySounds(Crewed);

	//
	// Fake up a timestep to get Orbitersound started.
	//

	timedSounds.Timestep(MissionTime, 0.0, AutoSlow);

	//
	// Check SM devices.
	//

	CheckSMSystemsState();

	//
	// Set up joysticks.
	//

	HRESULT hr;
	// Having read the configuration file, set up DirectX...	
	hr = DirectInput8Create(dllhandle,DIRECTINPUT_VERSION,IID_IDirectInput8,(void **)&dx8ppv,NULL); // Give us a DirectInput context
	if(!FAILED(hr)){
		int x=0;
		// Enumerate attached joysticks until we find 2 or run out.
		dx8ppv->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
		if(js_enabled == 0){   // Did we get anything?			
			dx8ppv->Release(); // No. Close down DirectInput
			dx8ppv = NULL;     // otherwise it won't get closed later
			sprintf(oapiDebugString(),"DX8JS: No joysticks found");
		}else{
			while(x < js_enabled){                                // For each joystick
				dx8_joystick[x]->SetDataFormat(&c_dfDIJoystick2); // Use DIJOYSTATE2 structure to report data
				// Can't do this because we don't own a window.
				// dx8_joystick[x]->SetCooperativeLevel(dllhandle,   // We want data all the time,
				// 	 DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);		 // and we don't need exclusive joystick access.

				dx8_jscaps[x].dwSize = sizeof(dx8_jscaps[x]);     // Initialize size of capabilities data structure
				dx8_joystick[x]->GetCapabilities(&dx8_jscaps[x]); // Get capabilities
				// Z-axis detection
				if ((rhc_id == x && rhc_auto) || (thc_id == x && thc_auto)) {
					js_current = x;
					dx8_joystick[x]->EnumObjects(EnumAxesCallback, this, DIDFT_AXIS | DIDFT_POV);
				}
				x++;                                              // Next!
			}
		}
	} else {
		// We can't print an error message this early in initialization, so save this reason for later investigation.
		dx8_failure = hr;
	}
}

bool Saturn::CheckForLaunchShutdown()

{
	//
	// Shut down the engine when we're close to the desired apogee and perigee.
	//

	double apogee, perigee;

	OBJHANDLE ref = GetGravityRef();
	GetElements(elemSaturn1B, refSaturn1B);
	GetApDist(apogee);
	GetPeDist(perigee);
	apogee = (apogee - oapiGetSize(ref)) / 1000.;
	perigee = (perigee - oapiGetSize(ref)) / 1000.;

	if ((refSaturn1B - refPREV) >= 0 &&
		(stage == LAUNCH_STAGE_SIVB) &&
		((elemSaturn1B.e > elemPREV.e && elemSaturn1B.e < 0.03) ||
		(apogee >= agc.GetDesiredApogee() && perigee >= agc.GetDesiredPerigee() - 0.8)))
	{
		if (GetEngineLevel(ENGINE_MAIN) > 0){
			SetEngineLevel(ENGINE_MAIN, 0);
			if (oapiGetTimeAcceleration() > 1.0)
				oapiSetTimeAcceleration(1.0);

			S4CutS.play(NOLOOP, 255);
			S4CutS.done();

			ActivateNavmode(NAVMODE_KILLROT);

			// Reset autopilot commands
			AtempP  = 0;
			AtempY  = 0;
			AtempR  = 0;			
		}
		return true;
	}
	elemPREV = elemSaturn1B;
	refPREV = refSaturn1B;

	return false;
}

void Saturn::SetGenericStageState()

{
	switch(stage) {
	case CSM_LEM_STAGE:
		SetCSMStage();
		break;

	case CM_STAGE:
	case CM_ENTRY_STAGE_TWO:
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_THREE:
		SetChuteStage1();
		break;

	case CM_ENTRY_STAGE_FOUR:
		SetChuteStage2();
		break;

	case CM_ENTRY_STAGE_FIVE:
		SetChuteStage3();
		break;

	case CM_ENTRY_STAGE_SIX:
		SetChuteStage4();
		break;

	case CM_ENTRY_STAGE_SEVEN:
		SetSplashStage();
		break;

	case CM_RECOVERY_STAGE:
		SetRecovery();
		break;

	case CM_ENTRY_STAGE:
		SetReentryStage();
		break;
	}
}

//
// Clear all thrusters and handles.
//

void Saturn::ClearThrusters()

{
	ClearThrusterDefinitions();

	//
	// Thruster groups.
	//

	thg_main = 0;
	thg_let = 0;
	thg_ull = 0;
	thg_ver = 0;
	thg_retro1 = 0;
	thg_retro2 = 0;
	thg_aps = 0;
	th_o2_vent = 0;

}

//
// Clear all propellants and handles.
//

void Saturn::ClearPropellants()

{
	ClearPropellantResources();

	//
	// Zero everything.
	//

	ph_1st = 0;
	ph_2nd = 0;
	ph_3rd = 0;
	ph_sps = 0;
	ph_let = 0;

	ph_rcs0 = 0;
	ph_rcs1 = 0;
	ph_rcs2 = 0;
	ph_rcs3 = 0;

	ph_rcs_cm_1 = 0;
	ph_rcs_cm_2 = 0;

	ph_sep = 0;
	ph_sep2 = 0;

	ph_ullage1 = 0;
	ph_ullage2 = 0;
	ph_ullage3 = 0;

	ph_o2_vent = 0;
}

//
// Do we have a CSM on this launcher?
//

bool Saturn::SaturnHasCSM()

{
	return CSMAttached;
}

//
// Set thruster state on or off. This should really turn off all but roll
// thrusters until we're in orbit.
//
// Also, they should have their own fuel tank rather than use the main SIVB
// fuel!
//

void Saturn::SetSIVBThrusters(bool active)

{
	if (active)
	{
		SetThrusterResource(th_att_rot[0], ph_3rd);
		SetThrusterResource(th_att_rot[1], ph_3rd);
		SetThrusterResource(th_att_rot[2], ph_3rd);
		SetThrusterResource(th_att_rot[3], ph_3rd);
		SetThrusterResource(th_att_rot[4], ph_3rd);
		SetThrusterResource(th_att_rot[5], ph_3rd);
		SetThrusterResource(th_att_rot[7], ph_3rd);
		SetThrusterResource(th_att_rot[6], ph_3rd);
		SetThrusterResource(th_att_rot[8], ph_3rd);
		SetThrusterResource(th_att_rot[9], ph_3rd);
	}
	else
	{
		SetThrusterResource(th_att_rot[0], NULL);
		SetThrusterResource(th_att_rot[1], NULL);
		SetThrusterResource(th_att_rot[2], NULL);
		SetThrusterResource(th_att_rot[3], NULL);
		SetThrusterResource(th_att_rot[4], NULL);
		SetThrusterResource(th_att_rot[5], NULL);
		SetThrusterResource(th_att_rot[6], NULL);
		SetThrusterResource(th_att_rot[7], NULL);
		SetThrusterResource(th_att_rot[8], NULL);
		SetThrusterResource(th_att_rot[9], NULL);
	}
}

//
// Thrusters can't turn more than five degrees from straight ahead. Check for that,
// and alarm if they try to go beyond it.
//

void Saturn::LimitSetThrusterDir (THRUSTER_HANDLE th, const VECTOR3 &dir)

{
	VECTOR3 realdir = dir;
	bool alarm = false;

	if (realdir.x > 0.1) {
		realdir.x = 0.1;
		alarm = true;
	}

	if (realdir.x < (-0.1)) {
		realdir.x = -0.1;
		alarm = true;
	}

	if (realdir.y > 0.1) {
		realdir.y = 0.1;
		alarm = true;
	}

	if (realdir.y < (-0.1)) {
		realdir.y = (-0.1);
		alarm = true;
	}

//
//	I don't think this is historically correct, so I disabled it
//    
/*	if (alarm && autopilot) {
		MasterAlarm();
	}
*/
	SetThrusterDir(th, realdir);
}

void Saturn::LoadDefaultSounds()

{
	//
	// Remember that you can't load mission sounds at this point as the
	// mission path hasn't been set up!
	//

	soundlib.LoadSound(SeparationS, SEPARATION_SOUND);
	soundlib.LoadSound(Sctdw, COUNT10_SOUND);
	soundlib.LoadSound(Sclick, CLICK_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Bclick, BUTTON_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Gclick, GUARD_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(ThumbClick, THUMBWHEEL_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Psound, PROBE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(CabinFans, CMCABIN_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SuitCompressorSound, SUITCOMPRESSOR_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SMasterAlarm, MASTERALARM_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SplashS, SPLASH_SOUND);
	soundlib.LoadSound(StageS, "Stagesep.wav");
	soundlib.LoadSound(CrashBumpS, "Crash.wav");
	soundlib.LoadSound(SDockingCapture, DOCKINGCAPTURE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SDockingLatch, DOCKINGLATCH_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SDockingExtend, DOCKINGEXTEND_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SUndock, UNDOCK_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(PostLandingVentSound, POSTLANDINGVENT_SOUND, BOTHVIEW_FADED_CLOSE);
	soundlib.LoadSound(CrewDeadSound, CREWDEAD_SOUND);
	soundlib.LoadSound(RCSFireSound, RCSFIRE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(RCSSustainSound, RCSSUSTAIN_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(HatchOpenSound, HATCHOPEN_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(HatchCloseSound, HATCHCLOSE_SOUND, INTERNAL_ONLY);

	Sctdw.setFlags(SOUNDFLAG_1XONLY|SOUNDFLAG_COMMS);
}

void Saturn::SIVBBoiloff()

{
	//
	// The SIVB stage boils off a small amount of fuel while in orbit.
	//
	// For the time being we'll ignore any thrust created by the venting
	// of this fuel.
	//

	double FuelMass = GetPropellantMass(ph_3rd) * 0.99998193;
	SetPropellantMass(ph_3rd, FuelMass);
}


void Saturn::StageOrbitSIVB(double simt, double simdt)

{
	// We get here after orbit insertion util CSM/LV separation. The engine and ullage have both shut down.

	//
	// Attitude control
	//

	if (LVGuidanceSwitch.IsUp() && use_lvdc == false) {
		if (ApolloNo == 7) {
			if (MissionTime >= 10275) {
				iu.HoldAttitude();
				
			} else if (MissionTime >= 9780) {
				iu.SetLVLHAttitude(_V(cos(20. * RAD), -sin(20. * RAD), 0));

			} else if (MissionTime >= SIVBCutoffTime + 20)	{
				iu.SetLVLHAttitude(_V(1, 0, 0));
			}
		} else {
			// In all other missions maintain LVLH attitude for now
			// \todo Correct behaviour of the S-IVB 
			
		}
	} else {
		// Manual S-IVB control via CMC
		SaturnTakeoverMode();
	}

	//
	// Venting, see Apollo 7 Saturn IB Report, NTRS ID 19900067467
	// \todo other missions?
	//

	if (ApolloNo == 7) {
		if (MissionTime >= SIVBCutoffTime + 5773) {
			if (GetThrusterLevel(th_main[0]) > 0) {
				SetJ2ThrustLevel(0);
				EnableDisableJ2(false);
			}
		} else if (MissionTime >= SIVBCutoffTime + 5052) {
			if (GetThrusterLevel(th_main[0]) == 0) {
				EnableDisableJ2(true);
				SetJ2ThrustLevel(1);
			}
		}
	}

	//
	// Enable random ATC chatter.
	//

	if (!UseATC)
		soundlib.SoundOptionOnOff(PLAYRADIOATC, TRUE);

	//
	// Fuel boiloff every ten seconds.
	//

	if (MissionTime >= NextMissionEventTime) 
	{
		if (GetThrusterLevel(th_main[0]) < 0.5)
			SIVBBoiloff();
		NextMissionEventTime = MissionTime + 10.0;
	}

	//
	// For unmanned launches, seperate the CSM on timer.
	//

	if (!Crewed && CSMSepSet && (MissionTime >= CSMSepTime - 20.))
	{
		SlowIfDesired();
	}

	if (!Crewed && CSMSepSet && (MissionTime >= CSMSepTime))
	{
		SlowIfDesired();
		// Raise checklist event
		eventControl.CSM_LV_SEPARATION = MissionTime;
		CSMSepSet = false;
	}

	//
	// For unmanned launches, seperate the payload on timer.
	//

	bool PayloadDeployed = false;

	if (!Crewed && PayloadDeploySet && (MissionTime >= PayloadDeployTime - 20.))
	{
		SlowIfDesired();
	}

	if (!Crewed && PayloadDeploySet && (MissionTime >= PayloadDeployTime))
	{
		SlowIfDesired();
		PayloadDeployed = true;
		// Payload deploy
		SeparateStage(CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
	}

	//
	// CSM/LV separation
	//

	if (CSMLVPyros.Blown() || bAbort)
	{
		SeparateStage(CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);

		if (bAbort)
		{
			/// \todo SPS abort handling correct? Check also Saturn V & 1B SPS aborts
			StartAbort();
			bAbort = false;
			autopilot = false;
		}
		else if (ApolloNo == 11)
		{
			//
			// Apollo 11 seperation knocked out propellant valves for RCS Quad B.
			//

			SMQuadBRCS.GetPrimPropellantValve()->SetState(false);  
			SMQuadBRCS.GetSecPropellantValve()->SetState(false);  
		}
	}

	//
	// If the payload was deployed, delete us. Note that this just means that the SLA panels have
	// been blown off of the SIVB; the SIVB will have to do the actual payload deployment.
	//
	if (PayloadDeployed && hs4bM)
	{
		PayloadDeploySet = false;
		oapiSetFocusObject(hs4bM);
		oapiDeleteVessel(GetHandle(), hs4bM);
	}

	/* sprintf(oapiDebugString(), "StageOrbitSIVB SIVB thrust %.1f isp %.2f propellant %.1f", 
		GetThrusterLevel(th_main[0]) * GetThrusterMax(th_main[0]), GetThrusterIsp(th_main[0]), GetPropellantMass(ph_3rd));
	*/
}

void Saturn::SaturnTakeoverMode() {

	// see GSOP 3.5

	const double RATE_FINE = RAD*(0.001);
	const double GAIN_FACTOR = 10.;

	VECTOR3 PMI;
	double Mass, Size, MaxThrust, Thrust;

	VESSELSTATUS status;
	GetStatus(status);
	GetPMI(PMI); 
	Mass = GetMass();
	Size = GetSize();
	MaxThrust = GetSaturnMaxThrust(ENGINE_ATTITUDE);

	VECTOR3 lvl = _V(0, 0, 0);
	VECTOR3 target = _V(0, 0, 0);

	// Is S-IVB Takeover enabled (DAP setting)?
	ChannelValue val12;
	val12 = agc.GetInputChannel(012); 
	if (val12[EnableSIVBTakeover]) { 
		// roll
		if (gdc.fdai_err_x > 40)
			target.z = 0.5 * RAD;
		if (gdc.fdai_err_x < -40)
			target.z = -0.5 * RAD;
		// pitch
		if (gdc.fdai_err_y > 40)
			target.x = -0.3 * RAD;
		if (gdc.fdai_err_y < -40)
			target.x = 0.3 * RAD;
		// yaw
		if (gdc.fdai_err_z > 40)
			target.y = -0.3 * RAD;
		if (gdc.fdai_err_z < -40)
			target.y = 0.3 * RAD;
	}
	target = target - status.vrot;

	// x axis
	Thrust = GAIN_FACTOR * (Mass * PMI.x * target.x) / Size;
	if (target.x > RATE_FINE) {
		lvl.x = min((Thrust / MaxThrust), 1);
	} else if (target.x < -RATE_FINE) {
		lvl.x = max((Thrust / MaxThrust), -1);
	} else {
		lvl.x = 0;
	}
	// y axis
	Thrust = GAIN_FACTOR * (Mass * PMI.y * target.y) / Size;
	if (target.y > RATE_FINE) {
		lvl.y = min((Thrust / MaxThrust), 1);
	} else if (target.y < -RATE_FINE) {
		lvl.y = max((Thrust / MaxThrust), -1);
	} else {
		lvl.y = 0;
	}
	// z axis
	Thrust = GAIN_FACTOR * (Mass * PMI.z * target.z) / Size;
	if (target.z > RATE_FINE) {
		lvl.z = min((Thrust/MaxThrust), 1);
	} else if (target.z < -RATE_FINE) {
		lvl.z = max((Thrust/MaxThrust), -1);
	} else {
		lvl.z = 0;
	}

	SetSaturnAttitudeRotLevel(lvl);

	// sprintf(oapiDebugString(), "Z rate %f target %f level %f", status.vrot.z * DEG, target.z * DEG, lvl.z);
}

void Saturn::StageSix(double simt)

{
	//
	// Should we be turning off these lights here?
	//

	for (int i=0 ;i<6; i++)
	{
		LAUNCHIND[i]=false;
	}

	if (ApolloNo == 13) {

		//
		// Play cryo-stir audio.
		//

		if (!CryoStir && MissionTime >= (APOLLO_13_EXPLOSION_TIME - 30))
		{
			double TimeW = oapiGetTimeAcceleration ();
			if (TimeW > 1){
				oapiSetTimeAcceleration (1);
			}

			SApollo13.play(NOLOOP, 255);
			CryoStir = true;
		}

		//
		// Play explosion audio.
		//

		if (CryoStir && !ApolloExploded && MissionTime >= APOLLO_13_EXPLOSION_TIME) {
			double TimeW = oapiGetTimeAcceleration ();
			if (TimeW > 1){
				oapiSetTimeAcceleration (1);
			}

			if (SApollo13.isValid()) {
				SApollo13.done();
			}

			SExploded.play(NOLOOP,255);
			SExploded.done();

			MasterAlarm();

			//
			// AGC restarted as the explosion occured.
			//

			agc.ForceRestart();

			ApolloExploded = true;

			//
			// Update the mesh.
			//

			SetCSMStage();

			//
			// Blow off Panel 4.
			//

			VESSELSTATUS vs1;

			const double CGOffset = 12.25+21.5-1.8+0.35;

			VECTOR3 vel1 = { 0.0, -0.25, 0.15 };
			VECTOR3 ofs1 = { 0, 0, 30.25 - CGOffset};

			GetStatus (vs1);

			VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};

			Local2Rel (ofs1, vs1.rpos);
			GlobalRot (vel1, rofs1);

			vs1.rvel.x = rvel1.x+rofs1.x;
			vs1.rvel.y = rvel1.y+rofs1.y;
			vs1.rvel.z = rvel1.z+rofs1.z;
			vs1.vrot.x = 0.005;
			vs1.vrot.y = 0.05;
			vs1.vrot.z = 0.01;

			char VName[256];

			GetApolloName(VName);
			strcat (VName, "-PANEL4");

			oapiCreateVessel(VName,"ProjectApollo/SM-Panel4",vs1);

			//
			// This is actually wrong because it will give us an
			// artificially low mass for the CSM. We should just disable
			// the engines.
			//

			SetPropellantMass(ph_sps,0);
		}

		//
		// Play Kranz comments in the background.
		//

		if (!KranzPlayed && (MissionTime >= APOLLO_13_EXPLOSION_TIME + 30)) {

			if (SExploded.isValid()) {
				SExploded.stop();
				SExploded.done();
			}

			SKranz.play(NOLOOP, 150);
			SKranz.done();

			KranzPlayed = true;
		}

		if (ApolloExploded && ph_o2_vent) {
			TankQuantities t;
			GetTankQuantities(t);

			SetThrusterLevel(th_o2_vent, t.O2Tank1Quantity + 0.1);
			SetO2TankQuantities(GetPropellantMass(ph_o2_vent) / 2.0);
		}
	}

	//
	// Handle automation of unmanned launches.
	//

	if (!Crewed) {
		switch (StageState)
		{
		case 0:
			if (CSMBurn) {
				NextMissionEventTime = CSMBurnStart - 300.0;
				StageState++;
			} else {
				StageState = 10;
			}
			break;

		case 1:
			if (MissionTime >= NextMissionEventTime) {
				ActivateCSMRCS();
				StageState++;
			}
			break;

		case 2:
			SlowIfDesired();
			ActivateNavmode(NAVMODE_PROGRADE);
			NextMissionEventTime = CSMBurnStart;
			StageState++;
			break;

		case 3:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				SPSEngine.EnforceBurn(true);
				NextMissionEventTime = MissionTime + 0.25;
				StageState++;
			}
			break;

		case 4:
			if (MissionTime >= NextMissionEventTime) {
				OBJHANDLE hPlanet = GetGravityRef();
				double prad = oapiGetSize(hPlanet);
				double ap;
				GetApDist(ap);

				ActivateNavmode(NAVMODE_PROGRADE);
				SPSEngine.EnforceBurn(true);

				NextMissionEventTime = MissionTime + 0.25;

				//
				// Burn until the orbit is about right or we're out of fuel.
				//

				if ((ap >= (prad + (CSMApogee * 1000.0))) || (actualFUEL <= 0.1)) {
					StageState++;
					DeactivateNavmode(NAVMODE_PROGRADE);
					DeactivateCSMRCS();
					SPSEngine.EnforceBurn(false);
					NextMissionEventTime = MissionTime + CalculateApogeeTime() - 800;
					StageState++;
				}
			}
			break;

		//
		// Get a more accurate apogee time.
		//

		case 5:
			if (MissionTime >= NextMissionEventTime) {
				NextMissionEventTime = MissionTime + CalculateApogeeTime() - 100;
				StageState++;
			}
			break;

		case 6:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				ActivateCSMRCS();
				ActivateNavmode(NAVMODE_RETROGRADE);
				NextMissionEventTime = MissionTime + CalculateApogeeTime() - 15;
				StageState++;
			}
			break;

		case 7:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();

				OBJHANDLE hPlanet = GetGravityRef();
				double prad = oapiGetSize(hPlanet);
				double pe;
				GetPeDist(pe);

				SPSEngine.EnforceBurn(true);
				ActivateNavmode(NAVMODE_RETROGRADE);

				NextMissionEventTime = MissionTime + 0.25;

				if ((pe <= (prad + (CSMPerigee * 1000.0))) || (actualFUEL <= 0.1)) {
					StageState++;
					ActivateNavmode(NAVMODE_PROGRADE);
					SPSEngine.EnforceBurn(false);
					CSMBurn = false;
					NextMissionEventTime = MissionTime + 500.0;
					StageState++;
				}
			}
			break;

		case 8:
			if (MissionTime >= NextMissionEventTime) {
				DeactivateNavmode(NAVMODE_PROGRADE);
				ActivateNavmode(NAVMODE_KILLROT);
				NextMissionEventTime = MissionTime + 100.0;
				StageState++;
			}
			break;

		case 9:
			if (MissionTime >= NextMissionEventTime) {
				DeactivateNavmode(NAVMODE_PROGRADE);
				DeactivateNavmode(NAVMODE_KILLROT);
				DeactivateCSMRCS();
				StageState++;
			}
			break;

		//
		// Final acceleration burn prior to entry.
		//

		case 10:
			if (CSMAccelSet) {
				NextMissionEventTime = CSMAccelTime - 180;
				StageState++;
			} else {
				StageState = 16;
			}
			break;

		case 11:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				ActivateCSMRCS();
				ActivateNavmode(NAVMODE_PROGRADE);
				NextMissionEventTime = CSMAccelTime;
				StageState++;
			}
			break;

		/// \todo What we really need to do here is ensure that we stay pitched down at the appropriate
		/// level relative to the local horizon. Currently this code will leave us with a perigee
		/// that's way too high, so disable it for now.

		case 12:
			if (MissionTime >= NextMissionEventTime) {
				// SlowIfDesired();
				// SPSEngine.EnforceBurn(true);
				ActivateNavmode(NAVMODE_PROGRADE);
				NextMissionEventTime = CSMAccelEnd;
				StageState++;
			}
			break;

		case 13:
			if (MissionTime >= NextMissionEventTime) {
				ActivateNavmode(NAVMODE_PROGRADE);
				SPSEngine.EnforceBurn(false);
				CSMAccelSet = false;
				NextMissionEventTime = MissionTime + 10.0;
				StageState++;
			}
			break;

		case 14:
			if (MissionTime >= NextMissionEventTime) {
				ActivateNavmode(NAVMODE_KILLROT);
				NextMissionEventTime = MissionTime + 10.0;
				StageState++;
			}
			break;

		case 15:
			if (MissionTime >= NextMissionEventTime) {
				DeactivateNavmode(NAVMODE_PROGRADE);
				DeactivateNavmode(NAVMODE_KILLROT);
				DeactivateCSMRCS();
				StageState++;
			}
			break;

		//
		// CM/SM separation
		//

		case 16:
			if (CMSepSet) {
				NextMissionEventTime = CMSepTime - 10;
				StageState++;
			}
			break;

		case 17:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				NextMissionEventTime = CMSepTime;
				StageState++;
			}
			break;

		case 18:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				// Raise checklist event
				eventControl.CM_SM_SEPARATION = MissionTime;
				StageState++;
			}
			break;
		}
	}

	//
	// CM/SM separation pyros
	//

	if (CMSMPyros.Blown())
	{
		SeparateStage(CM_STAGE);
		SetStage(CM_STAGE);
	}
}

void Saturn::StartAbort()

{
	//
	// Event timer resets to zero on abort.
	//

	EventTimerDisplay.Reset();
	EventTimerDisplay.SetRunning(true);
	EventTimerDisplay.SetEnabled(true);

	//
	// Fire the LET.
	//

	if (thg_let)
		SetThrusterGroupLevel (thg_let, 1.0);

	ABORT_IND = true;

	ClearEngineIndicators();
}

void Saturn::SlowIfDesired()

{
	if (AutoSlow && (oapiGetTimeAcceleration() > 1.0)) {
		oapiSetTimeAcceleration(1.0);
	}
}

//
// Allows Launch Vehicle Quantities to be accessed outside of class
//
void Saturn::GetLVTankQuantities(LVTankQuantities &LVq)
{
	if (SaturnType == SAT_SATURN1B) {

		//
		// Clear to defaults, for the Saturn 1B SIC and SII quantities are the same,
		// SIVB fuel mass is SII_FuelMass!
		//

		LVq.SICQuantity = 0.0;  //current quantities
		LVq.SIIQuantity = 0.0;
		LVq.SIVBOxQuantity = 0.0;
		LVq.SIVBFuelQuantity = 0.0;
		LVq.SICFuelMass = SI_FuelMass;  //Initial amounts
		LVq.SIIFuelMass = SI_FuelMass;
		LVq.S4BOxMass = SII_FuelMass;
		LVq.S4BFuelMass = SII_FuelMass;
		
		//
		// No tanks if we've seperated from the different stages of LV
		//

		if (stage >= CSM_LEM_STAGE) {
			return;
		}
		else if (stage >= LAUNCH_STAGE_TWO) {
			if (!ph_3rd) {
				return;
			}
			else if (ph_3rd) {
				//Someday we'll need to simulate SIVB Ox and Fuel Tanks seperately for true Guage support for now it's just done with an correction value that roughly equates to 94% fuel burned for 100% ox burned
				LVq.SIVBOxQuantity = GetPropellantMass(ph_3rd);  
				LVq.SIVBFuelQuantity = (GetPropellantMass(ph_3rd) + ((.0638 * SII_FuelMass) * (1 - (GetPropellantMass(ph_3rd) / SII_FuelMass))));
				return;
			}
		}
		else if (stage >= LAUNCH_STAGE_ONE) {
			LVq.SIVBOxQuantity = SII_FuelMass;
			LVq.SIVBFuelQuantity = SII_FuelMass;
			if (!ph_1st) {
				return;
			}
			else if (ph_1st) {
				LVq.SICQuantity = GetPropellantMass(ph_1st);
				LVq.SIIQuantity = GetPropellantMass(ph_1st);
				return;
			}
		}
		else {
			LVq.SICQuantity = SI_FuelMass;
			LVq.SIIQuantity = SI_FuelMass;
			LVq.SIVBOxQuantity = SII_FuelMass;
			LVq.SIVBFuelQuantity = SII_FuelMass;
			return;
		}

	} else {

		//
		// Clear to defaults.
		//
		LVq.SICQuantity = 0.0;  //current quantities
		LVq.SIIQuantity = 0.0;
		LVq.SIVBOxQuantity = 0.0;
		LVq.SIVBFuelQuantity = 0.0;
		LVq.SICFuelMass = SI_FuelMass;  //Initial amounts
		LVq.SIIFuelMass = SII_FuelMass;
		LVq.S4BOxMass = S4B_FuelMass;
		LVq.S4BFuelMass = S4B_FuelMass;

		
		//
		// No tanks if we've seperated from the different stages of LV
		//

		if (stage >= CSM_LEM_STAGE) {
			return;
		}
		else if (stage >= LAUNCH_STAGE_SIVB) {
			if (!ph_3rd){
				return;
			}
			else if (ph_3rd) {
				//Someday we'll need to simulate SIVB Ox and Fuel Tanks seperately for true Guage support for now it's just done with an correction value that roughly equates to 94% fuel burned for 100% ox burned
				LVq.SIVBOxQuantity = GetPropellantMass(ph_3rd);  
				LVq.SIVBFuelQuantity = (GetPropellantMass(ph_3rd) + ((.0638 * S4B_FuelMass) * (1 - (GetPropellantMass(ph_3rd) / S4B_FuelMass))));
				return;
			}
		}
		else if (stage >= LAUNCH_STAGE_TWO) {
			LVq.SIVBOxQuantity = S4B_FuelMass;
			LVq.SIVBFuelQuantity = S4B_FuelMass;
			if (!ph_2nd) {
				return;
			}
			else if (ph_2nd) {
				LVq.SIIQuantity = GetPropellantMass(ph_2nd);
				return;
			}
		}
		else if (stage >= LAUNCH_STAGE_ONE) {
			LVq.SIVBOxQuantity = S4B_FuelMass;
			LVq.SIVBFuelQuantity = S4B_FuelMass;
			LVq.SIIQuantity = SII_FuelMass;
			if (!ph_1st) {
				return;
			}
			else if (ph_1st) {
				LVq.SICQuantity = GetPropellantMass(ph_1st);
				return;
			}
		}
		else {
			LVq.SICQuantity = SI_FuelMass;
			LVq.SIIQuantity = SII_FuelMass;
			LVq.SIVBOxQuantity = S4B_FuelMass;
			LVq.SIVBFuelQuantity = S4B_FuelMass;
			return;
		}
	}
}

//
// Get the J2 ISP from the mixture ratio and calculate the thrust adjustment.
//

#define MR_STATS 5

static double MixtureRatios[MR_STATS] = {6.0, 5.5, 5.0, 4.3, 4.0 };
//static double MRISP[MR_STATS] = { 416*G, 418*G, 421*G, 427*G, 432*G };
static double MRISP[MR_STATS] = { 421.4 * G, 423.4 * G, 426.5 * G, 432.6 * G, 437.6 * G };
static double MRThrust[MR_STATS] = { 1.1, 1.0, .898, .7391, .7 };

double Saturn::GetJ2ISP(double ratio)

{
	double isp = 421*G;

	// From Usenet:
	// It had roughly three stops. 178,000 lbs at 425s Isp and an O/F of 4.5,
	// 207,000 lbs at 421s Isp and an O/F of 5.0, and 230,500 lbs at 418s Isp
	// and an O/F of 5.5.

	for (int i = 0; i < MR_STATS; i++) {
		if (ratio >= MixtureRatios[i]) {
			double delta = (ratio - MixtureRatios[i]) / (MixtureRatios[i - 1] - MixtureRatios[i]);

			isp = MRISP[i] + ((MRISP[i - 1] - MRISP[i]) * delta);
			ThrustAdjust = MRThrust[i] + ((MRThrust[i - 1] - MRThrust[i]) * delta);

			return isp;
		}
	}

	return isp;
}

//
// Set up random failures if required.
//

void Saturn::SetRandomFailures()

{
	//
	// I'm not sure how reliable the random number generator is. We may want to get a
	// number from 0-1000 and then see if that's less than some threshold, rather than
	// checking for the bottom bits being zero.
	//

	//
	// Set up launch failures.
	//

	if (!LaunchFail.Init)
	{
		LaunchFail.Init = 1;
		if (!(random() & 15))
		{
			LaunchFail.EarlySICenterCutoff = 1;
			FirstStageCentreShutdownTime = 20.0 + ((double) (random() & 1023) / 10.0);
		}
		if (!(random() & 15))
		{
			LaunchFail.EarlySIICenterCutoff = 1;
			SecondStageCentreShutdownTime = 200.0 + ((double) (random() & 2047) / 10.0);
		}
		if (!(random() & 127))
		{
			LaunchFail.LETAutoJetFail = 1;
		}
		if (!(random() & 63))
		{
			LaunchFail.SIIAutoSepFail = 1;
		}
		if (!(random() & 255))
		{
			LaunchFail.LESJetMotorFail = 1;
		}
	}

	//
	// Set up landing failures.
	//

	if (!LandFail.Init)
	{
		LandFail.Init = 1;
		if (!(random() & 127))
		{
			LandFail.CoverFail = 1;
		}
		if (!(random() & 127))
		{
			LandFail.DrogueFail = 1;
		}
		if (!(random() & 127)) 
		{
			LandFail.MainFail = 1;
		}
	}

	//
	// Set up switch failures.
	//

	if (!SwitchFail.Init)
	{
		SwitchFail.Init = 1;
		if (!(random() & 127))
		{
			SwitchFail.TowerJett1Fail = 1;
		}
		else if (!(random() & 127))
		{
			SwitchFail.TowerJett2Fail = 1;
		}
		if (!(random() & 127))
		{
			SwitchFail.SMJett1Fail = 1;
		}
		else if (!(random() & 127))
		{
			SwitchFail.SMJett2Fail = 1;
		}

		//
		// Random CWS light failures.
		//
		if (!(random() & 15)) 
		{
			int i, n = (random() & 7) + 1;

			for (i = 0; i < n; i++)
			{
				cws.FailLight(random() & 63, true);
			}
		}
	}

}

void Saturn::SetJ2ThrustLevel(double thrust)

{
	if (stage != STAGE_ORBIT_SIVB || !th_main[0])
		return;

	SetThrusterLevel(th_main[0], thrust);
}

void Saturn::EnableDisableJ2(bool Enable)

{
	if (stage != STAGE_ORBIT_SIVB || !th_main[0] || !ph_3rd)
		return;

	if (Enable)
	{
		SetThrusterResource(th_main[0], ph_3rd);
	}
	else
	{
		SetThrusterResource(th_main[0], NULL);
	}
}

double Saturn::GetJ2ThrustLevel()

{
	if (stage != STAGE_ORBIT_SIVB || !th_main[0])
		return 0.0;

	return GetThrusterLevel(th_main[0]);
}

void Saturn::SetAPSThrustLevel(double thrust)

{
	if (stage != STAGE_ORBIT_SIVB || !thg_aps)
		return;

	SetThrusterGroupLevel(thg_aps, thrust);
}

double Saturn::GetSIVbPropellantMass()

{
	if (stage > STAGE_ORBIT_SIVB)
		return 0.0;

	if (stage < LAUNCH_STAGE_SIVB)
		return S4B_FuelMass;

	return GetPropellantMass(ph_3rd);
}

void Saturn::SetSIVbPropellantMass(double mass)

{
	if (stage > STAGE_ORBIT_SIVB)
		return;

	if (stage < LAUNCH_STAGE_SIVB)
	{
		S4B_FuelMass = mass;
		return;
	}

	SetPropellantMass(ph_3rd, mass);
}

int Saturn::GetTLIEnableSwitchState()

{
	return TLIEnableSwitch.GetState();
}

int Saturn::GetSIISIVbSepSwitchState()

{
	return SIISIVBSepSwitch.GetState();
}

void Saturn::PlayCountSound(bool StartStop)

{
	if (StartStop)
	{
		Scount.play(NOLOOP,245);
	}
	else
	{
		Scount.stop();
	}
}

void Saturn::PlaySecoSound(bool StartStop)

{
}

void Saturn::PlaySepsSound(bool StartStop)

{
	if (StartStop)
	{
		SepS.play(LOOP, 130);
	}
	else
	{
		SepS.stop();
	}
}

void Saturn::PlayTLISound(bool StartStop)

{
}

void Saturn::PlayTLIStartSound(bool StartStop)

{
}

//
// Most of this calculation code is lifted from the Soyuz guidance MFD.
//

extern double AbsOfVector(const VECTOR3 &Vec);

double Saturn::CalculateApogeeTime()

{
	OBJHANDLE hSetGbody;
	double GbodyMass, GbodySize;
	double p, v, R, RDotV, Mu_Planet, J2000, E, Me, T, tsp;
	double TtPeri, TtApo;
	double OrbitApo;
	VECTOR3 RelPosition, RelVelocity;
	ELEMENTS Elements;

	// Planet parameters
	hSetGbody = GetApDist(OrbitApo);
	GbodyMass = oapiGetMass(hSetGbody);
	GbodySize = oapiGetSize(hSetGbody) / 1000.;
	Mu_Planet = GK * GbodyMass;

	// Get eccentricity and orbital radius
	GetElements(Elements, J2000);
	GetRelativePos(hSetGbody, RelPosition);
	GetRelativeVel(hSetGbody, RelVelocity);

	R = AbsOfVector(RelPosition) / 1000.;

	// Calculate semi-latus rectum and true anomaly
	p = Elements.a / 1000. * (1. - Elements.e * Elements.e);
	v = acos((1. / Elements.e) * (p / R - 1.));

	RDotV = dotp(RelVelocity, RelPosition);
	if (RDotV < 0)
	{
		v = 2. * PI - v;
	}

	// Determine the time since periapsis
	//   - Eccentric anomaly
	E = 2. * atan(sqrt((1. - Elements.e) / (1. + Elements.e)) * tan(v / 2.));
	//   - Mean anomaly
	Me = E - Elements.e * sin(E);
	//   - Period of orbit
	T = 2. * PI * sqrt((Elements.a * Elements.a * Elements.a / 1e9) / Mu_Planet);

	// Time since periapsis is
	tsp = Me / (2.* PI) * T;

	// Time to next periapsis & apoapsis
	TtPeri = T - tsp;
	if (RDotV < 0) {
		TtPeri = -1. * tsp;
	}

	if (TtPeri > (T / 2.)) {
		TtApo = fabs((T / 2.) - TtPeri);
	}
	else {
		TtApo = fabs(TtPeri + (T / 2.));
	}

	return TtApo;
}

// Get checklist controller pointer
ChecklistController *Saturn::GetChecklistControl()
{
	return &checkControl;
}
void Saturn::TLI_Begun()
{
	eventControl.TLI = MissionTime;
}

void Saturn::TLI_Ended()
{
	eventControl.TLI_DONE = MissionTime;
}


//
// LUA Interface
//

int Saturn::clbkGeneric (int msgid, int prm, void *context)
{
	switch (msgid) {
	case VMSG_LUAINTERPRETER:
		return Saturn::Lua_InitInterpreter (context);
	case VMSG_LUAINSTANCE:
		return Saturn::Lua_InitInstance (context);
	}
	return 0;
}

int Saturn::Lua_InitInterpreter(void *context)
{
	lua_State *L = (lua_State*)context;

	// add interpreter initialisation script here
	// luaL_dofile (L, "..\\A7\\luatest\\init.lua");

	return 0;
}


int LuaCall_GetPanelSwitch(lua_State *L);
int LuaCall_SetPanelSwitch(lua_State *L);
int LuaCall_GetPanelSwitchFlashing(lua_State *L);
int LuaCall_SetPanelSwitchFlashing(lua_State *L);
int LuaCall_GetAGCErasable(lua_State *L);
int LuaCall_GetAGCChannel(lua_State *L);
int LuaCall_SetAGCErasable(lua_State *L);
int LuaCall_GetAGCUplinkStatus(lua_State *L);


int Saturn::Lua_InitInstance (void *context)
{
  lua_State *L = (lua_State*)context;
  luaL_getmetatable (L, "VESSEL.Saturn");
  if (lua_isnil(L,-1)) { // class not yet registered
    lua_pop(L,1);
    static const struct luaL_reg SaturnLib[] = {
		{"get_panelswitch", LuaCall_GetPanelSwitch},	
		{"set_panelswitch", LuaCall_SetPanelSwitch},	
		{"get_panelswitch_flashing", LuaCall_GetPanelSwitchFlashing},	
		{"set_panelswitch_flashing", LuaCall_SetPanelSwitchFlashing},	
		{"get_agcerasable", LuaCall_GetAGCErasable},	
		{"get_agcchannel", LuaCall_GetAGCChannel},	
		{"set_agcerasable", LuaCall_SetAGCErasable},	
		{"get_agcuplinkstatus", LuaCall_GetAGCUplinkStatus},	
		{NULL, NULL}
    };
	luaL_newmetatable (L, "Saturn.vtable");
    luaL_openlib (L, "Saturn.method", SaturnLib, 0);
    luaL_newmetatable (L, "Saturn.base");
    lua_pushstring (L, "__index");
    luaL_getmetatable (L, "VESSEL.vtable");
    lua_settable (L, -3);
    lua_setmetatable (L, -2);
    lua_pushstring (L, "__index");
    lua_pushvalue (L, -2);
    lua_settable (L, -4);
    lua_pop (L, 1);
  }
  lua_setmetatable (L, -2);

  return 0;
}

int Saturn::Lua_GetPanelSwitch(const char *name) {
	return MainPanel.GetState(name);
}

void Saturn::Lua_SetPanelSwitch(const char *name, int state) {
	MainPanel.SetState(name, state);
}

bool Saturn::Lua_GetPanelSwitchFlashing(const char *name) {
	return MainPanel.GetFlashing(name);
}

void Saturn::Lua_SetPanelSwitchFlashing(const char *name, bool flash) {
	MainPanel.SetFlashing(name, flash);
}

int Saturn::Lua_GetAGCErasable(int page, int addr) {
	return agc.GetErasable(page,addr);
}

int Saturn::Lua_GetAGCChannel(int ch) {
	return agc.GetOutputChannel(ch);
}

void Saturn::Lua_SetAGCErasable(int page, int addr, int value) {
	agc.SetErasable(page, addr, value);
	agc.GenerateUprupt();	
}

int Saturn::Lua_GetAGCUplinkStatus() {
	int st = 0;
	if (agc.IsUpruptActive()) {
		st = 1;
	}
	return st;
}


static int LuaCall_GetPanelSwitch(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	const char *name = lua_tostring(L,2);

	int outparam = v->Lua_GetPanelSwitch(name);
	lua_pushnumber(L, outparam);
	return 1;
}

static int LuaCall_SetPanelSwitch(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	const char *name = lua_tostring(L,2);
	int state = lua_tointeger(L,3);

	v->Lua_SetPanelSwitch(name, state);
	return 0;
}

static int LuaCall_GetPanelSwitchFlashing(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	const char *name = lua_tostring(L,2);

	int outparam = v->Lua_GetPanelSwitchFlashing(name);
	lua_pushnumber(L, outparam);
	return 1;
}

static int LuaCall_SetPanelSwitchFlashing(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	const char *name = lua_tostring(L,2);
	bool flash = (lua_toboolean(L,3) != 0);

	v->Lua_SetPanelSwitchFlashing(name, flash);
	return 0;
}

static int LuaCall_GetAGCErasable(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	int page = lua_tointeger(L,2);
	int addr = lua_tointeger(L,3);

	int outparam = v->Lua_GetAGCErasable(page, addr);
	lua_pushnumber(L, outparam);
	return 1;
}

static int LuaCall_GetAGCChannel(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	int chnum = lua_tointeger(L,2);

	int outword = v->Lua_GetAGCChannel(chnum);
	lua_pushnumber(L, outword);
	return 1;
}

static int LuaCall_SetAGCErasable(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	int page = lua_tointeger(L,2);
	int addr = lua_tointeger(L,3);
	int value = lua_tointeger(L,3);

	v->Lua_SetAGCErasable(page, addr, value);
	return 0;
}

static int LuaCall_GetAGCUplinkStatus(lua_State *L) {
	VESSEL **pv = (VESSEL**)lua_touserdata(L,1);
	Saturn *v = (Saturn*) *pv;

	int uplinkstatus = v->Lua_GetAGCUplinkStatus();
	lua_pushnumber(L, uplinkstatus);
	return 1;
}
