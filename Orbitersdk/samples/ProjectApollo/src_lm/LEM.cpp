/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn V Module Parked/Docked mode

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
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"
#include "tracer.h"
#include "papi.h"
#include "CollisionSDK/CollisionSDK.h"

#include "connector.h"

char trace_file[] = "ProjectApollo LM.log";


// ==============================================================
// Global parameters
// ==============================================================

static int refcount;

const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};

const int TO_EVA=1;


// Modif x15 to manage landing sound
#ifdef DIRECTSOUNDENABLED
static SoundEvent sevent        ;
static double NextEventTime = 0.0;
#endif

static GDIParams g_Param;

// ==============================================================
// API interface
// ==============================================================

BOOL WINAPI DllMain (HINSTANCE hModule,
					 DWORD ul_reason_for_call,
					 LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		InitGParam(hModule);
		g_Param.hDLL = hModule; // DS20060413 Save for later
		InitCollisionSDK();
		break;

	case DLL_PROCESS_DETACH:
		FreeGParam();
		break;
	}
	return TRUE;
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	LEM *lem;

	if (!refcount++) {
		LEMLoadMeshes();
	}
	
	// VESSELSOUND 

	lem = new LEM(hvessel, flightmodel);
	return static_cast<VESSEL *> (lem);
}

DLLCLBK void ovcExit (VESSEL *vessel)

{
	TRACESETUP("ovcExit LMPARKED");

	--refcount;

	if (!refcount) {
		TRACE("refcount == 0");

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) delete static_cast<LEM *> (vessel);
}

// DS20060302 DX8 callback for enumerating joysticks
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pLEM)
{
	class LEM * lem = (LEM*)pLEM; // Pointer to us
	HRESULT hr;

	if(lem->js_enabled > 1){  // Do we already have enough joysticks?
		return DIENUM_STOP; } // If so, stop enumerating additional devices.

	// Obtain an interface to the enumerated joystick.
    hr = lem->dx8ppv->CreateDevice(pdidInstance->guidInstance, &lem->dx8_joystick[lem->js_enabled], NULL);
	
	if(FAILED(hr)) {              // Did that work?
		return DIENUM_CONTINUE; } // No, keep enumerating (if there's more)

	lem->js_enabled++;      // Otherwise, Next!
	return DIENUM_CONTINUE; // and keep enumerating
}

// DX8 callback for enumerating joystick axes
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pLEM )
{
	class LEM * lem = (LEM*)pLEM; // Pointer to us

    if (pdidoi->guidType == GUID_ZAxis) {
		if (lem->js_current == lem->rhc_id) {
			lem->rhc_rzx_id = 1;
		} else {
			lem->thc_rzx_id = 1;
		}
	}

    if (pdidoi->guidType == GUID_RzAxis) {
		if (lem->js_current == lem->rhc_id) {
			lem->rhc_rot_id = 2;
		} else {
			lem->thc_rot_id = 2;
		}
	}

    if (pdidoi->guidType == GUID_POV) {
		if (lem->js_current == lem->rhc_id) {
			lem->rhc_pov_id = 0;
		} else {
			lem->thc_pov_id = 0;
		}
	}
    return DIENUM_CONTINUE;
}

// Constructor
LEM::LEM(OBJHANDLE hObj, int fmodel) : Payload (hObj, fmodel), 
	
	CDRs28VBus("CDR-28V-Bus",NULL),
	LMPs28VBus("LMP-28V-Bus",NULL),
	DES_CDRs28VBusA("DES-CDR-28V-BusA",NULL),
	DES_CDRs28VBusB("DES-CDR-28V-BusB",NULL),
	DES_LMPs28VBusA("DES-LMP-28V-BusA",NULL),
	DES_LMPs28VBusB("DES-LMP-28V-BusB",NULL),
	ED28VBusA("ED-28V-BusA", NULL),
	ED28VBusB("ED-28V-BusB", NULL),
	ACBusA("AC-Bus-A",NULL),
	ACBusB("AC-Bus-B",NULL),
	dsky(soundlib, agc, 015),
	LandingGearPyros("Landing-Gear-Pyros", Panelsdk),
	LandingGearPyrosFeeder("Landing-Gear-Pyros-Feeder", Panelsdk),
	StagingBoltsPyros("Staging-Bolts-Pyros", Panelsdk),
	StagingNutsPyros("Staging-Nuts-Pyros", Panelsdk),
	CableCuttingPyros("Cable-Cutting-Pyros", Panelsdk),
	CableCuttingPyrosFeeder("Cable-Cutting-Pyros-Feeder", Panelsdk),
	agc(soundlib, dsky, imu, Panelsdk),
	CSMToLEMPowerSource("CSMToLEMPower", Panelsdk),
	ACVoltsAttenuator("AC-Volts-Attenuator", 62.5, 125.0, 20.0, 40.0),
	EPSDCAmMeter(0, 120.0, 220.0, -50.0),
	EPSDCVoltMeter(20.0, 40.0, 215.0, -35.0),
	ComPitchMeter(0.0, 5.0, 220.0, -50.0),
	ComYawMeter(0.0, 5.0, 220.0, -50.0),
	Panel12SignalStrengthMeter(0.0, 5.0, 220.0, -50.0),
	RadarSignalStrengthMeter(0.0, 5.0, 220.0, -50.0),
	checkControl(soundlib),
	MFDToPanelConnector(MainPanel, checkControl),
	//imucase("LM-IMU-Case",_vector3(0.013, 3.0, 0.03),0.03,0.04),
	//imuheater("LM-IMU-Heater",1,NULL,150,53,0,326,328,&imucase),
	imu(agc, Panelsdk),
	deda(this,soundlib, aea, 015),
	DPS(th_hover),
	MissionTimerDisplay(Panelsdk),
	EventTimerDisplay(Panelsdk)
{
	dllhandle = g_Param.hDLL; // DS20060413 Save for later
	InitLEMCalled = false;
	SystemsInitialized = 0;

	// VESSELSOUND initialisation
	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	// Force to NULL to avoid stupid VC++ optimization failure
	int x;
	for (x = 0; x < N_LEM_VALVES; x++){
		pLEMValves[x] = NULL;
		ValveState[x] = FALSE;
	}

	// Init further down
	Init();
}

LEM::~LEM()

{
#ifdef DIRECTSOUNDENABLED
    sevent.Stop();
	sevent.Done();
#endif

	// DS20060413 release DirectX stuff
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

}

void LEM::Init()

{
	toggleRCS =false;

	DebugLineClearTimer = 0;

	ABORT_IND=false;

	bToggleHatch=false;
	bModeDocked=false;
	bModeHover=false;
	HatchOpen=false;
	ToggleEva=false;
	CDREVA_IP=false;
	refcount = 0;
	viewpos = LMVIEW_LMP;
	ContactOK = false;
	stage = 0;
	status = 0;

	actualFUEL = 0.0;

	InVC = false;
	InPanel = false;
	CheckPanelIdInTimestep = false;
	InFOV = true;
	SaveFOV = 0;

	Crewed = true;
	AutoSlow = false;

	MissionTime = 0;
	FirstTimestep = true;

	SwitchFocusToLeva = 0;

	agc.ControlVessel(this);
	imu.SetVessel(this, TRUE);
	
	ph_Dsc = 0;
	ph_Asc = 0;
	ph_RCSA = 0;
	ph_RCSB = 0;

	DescentFuelMassKg = 8375.0;
	AscentFuelMassKg = 2345.0;

	ApolloNo = 0;
	Landed = false;

	//
	// VAGC Mode settings
	//

	VAGCChecklistAutoSlow = false;
	VAGCChecklistAutoEnabled = false;

	// DS20160916 Physical parameters updation
	CurrentFuelWeight = 0;
	LastFuelWeight = 999999; // Ensure update at first opportunity

	LEMToCSMConnector.SetType(CSM_LEM_DOCKING);
	CSMToLEMPowerConnector.SetType(LEM_CSM_POWER);

	LEMToCSMConnector.AddTo(&CSMToLEMPowerConnector);
	CSMToLEMPowerSource.SetConnector(&CSMToLEMPowerConnector);

	//
	// Panel flash.
	//

	NextFlashUpdate = MINUS_INFINITY;
	PanelFlashOn = false;

	//
	// For now we'll turn on the mission timer. We don't yet have a switch to control
	// it.
	//
	// Switch added -- DS20100501
	//

	// MissionTimerDisplay.SetRunning(true);
	// MissionTimerDisplay.SetEnabled(true);

	//
	// And Event Timer.
	//

	EventTimerDisplay.SetRunning(true);
	EventTimerDisplay.SetEnabled(true);

	//
	// Initial sound setup
	//

	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);
	/// \todo Disabled for now because of the LEVA and the descent stage vessel
	///		  Enable before CSM docking
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);

	strncpy(AudioLanguage, "English", 64);
	soundlib.SetLanguage(AudioLanguage);
	SoundsLoaded = false;

	exhaustTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_atrcs");

	//
	// Register visible connectors.
	//
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &MFDToPanelConnector);
	RegisterConnector(0, &LEMToCSMConnector);

	// Do this stuff only once
	if(!InitLEMCalled){
		SystemsInit();

		// Panel items
		fdaiDisabled = false;
		fdaiSmooth = false;

		PanelId = LMPANEL_MAIN;	// default panel
		InitSwitches();
		// "dummy" SetSwitches to enable the panel event handling
		SetSwitches(PanelId);

		//
		// Default channel setup.
		//
		agc.SetInputChannelBit(030, DescendStageAttached, true);	// Descent stage attached.
		agc.SetInputChannelBit(030, TempInLimits, true);	// Temperature in limits.


		InitLEMCalled = true;
	}
}

void LEM::DoFirstTimestep()

{
	checkControl.linktoVessel(this);
	// Load sounds in case of dynamic creation, otherwise during clbkLoadStageEx
	if (!SoundsLoaded) {
		LoadDefaultSounds();
	}

#ifdef DIRECTSOUNDENABLED
	NextEventTime = 0.0;
#endif

	if (CabinFansActive()) {
		CabinFans.play(LOOP,255);
	}

	char VName10[256]="";

	strcpy (VName10, GetName()); strcat (VName10, "-LEVA");
	hLEVA=oapiGetVesselByName(VName10);
}

void LEM::LoadDefaultSounds()

{
    char buffers[80];

	soundlib.SetLanguage(AudioLanguage);
	sprintf(buffers, "Apollo%d", agc.GetApolloNo());
    soundlib.SetSoundLibMissionPath(buffers);

	//
	// load sounds now that the audio language has been set up.
	//

	soundlib.LoadMissionSound(LunarAscent, LUNARASCENT_SOUND, LUNARASCENT_SOUND);
	soundlib.LoadSound(StageS, "Stagesep.wav");
	soundlib.LoadMissionSound(Scontact, LUNARCONTACT_SOUND, LUNARCONTACT_SOUND);
	soundlib.LoadSound(Sclick, CLICK_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Rclick, ROTARY_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Bclick, "button.wav", INTERNAL_ONLY);
	soundlib.LoadSound(Gclick, "guard.wav", INTERNAL_ONLY);
	soundlib.LoadSound(CabinFans, "cabin.wav", INTERNAL_ONLY);
	soundlib.LoadSound(Vox, "vox.wav");
	soundlib.LoadSound(Afire, "des_abort.wav");
	soundlib.LoadSound(RCSFireSound, RCSFIRE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(RCSSustainSound, RCSSUSTAIN_SOUND, INTERNAL_ONLY);

// MODIF X15 manage landing sound
#ifdef DIRECTSOUNDENABLED
    sevent.LoadMissionLandingSoundArray(soundlib,"sound.csv");
    sevent.InitDirectSound(soundlib);
#endif
	SoundsLoaded = true;
}

int LEM::clbkConsumeBufferedKey(DWORD key, bool down, char *keystate) {

	// rewrote to get key events rather than monitor key state - LazyD

	// DS20060404 Allow keys to control DSKY like in the CM
	if (KEYMOD_SHIFT(keystate)){
		// Do DSKY stuff
		if(down){
			switch(key){
				case OAPI_KEY_PRIOR:
					dsky.ResetPressed();
					break;
				case OAPI_KEY_NEXT:
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
				case OAPI_KEY_DECIMAL:
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
				
			}
		}else{
			// KEY UP
			switch(key){
				case OAPI_KEY_DECIMAL:
					dsky.ProgReleased();
					break;

			}
		}
		return 0;
	}

	if (down){
		switch(key){
			// Valid shaft positions should be:
			// 000.00  00 (F) 
			// 057.30  60 (R)
			// 114.59 120 (Rr)
			// 171.89 180 (CL)
			// 229.18 240 (Lr)
			// 286.48 300 (L)

			case OAPI_KEY_A:
				optics.OpticsShaft--;
				if (optics.OpticsShaft < 0) {
					optics.OpticsShaft = 5; // Clobber
				}
				//Load panel to trigger change of the default camera direction
				if (PanelId == LMPANEL_AOTVIEW)
				{
					oapiSetPanel(LMPANEL_AOTVIEW);
				}
				break;

			case OAPI_KEY_D:
				optics.OpticsShaft++;
				if (optics.OpticsShaft > 5) {
					optics.OpticsShaft = 0; // Clobber
				}
				//Load panel to trigger change of the default camera direction
				if (PanelId == LMPANEL_AOTVIEW)
				{
					oapiSetPanel(LMPANEL_AOTVIEW);
				}
				break;

			case OAPI_KEY_W:
				optics.ReticleMoved = 0.52;  //Fast Rate (about 30 deg/sec)

				if (KEYMOD_ALT(keystate)) {
					optics.ReticleMoved = 0.01;  //Slow Rate (about 0.5 deg/sec)
				}
				break;

			case OAPI_KEY_S:
				optics.ReticleMoved = -0.52;  //Fast Rate (about 30 deg/sec)

				if (KEYMOD_ALT(keystate)) {
					optics.ReticleMoved = -0.01;  //Slow Rate (about 0.5 deg/sec)
				}
				break;
			case OAPI_KEY_Q:
				agc.SetInputChannelBit(016, MarkX, 1);  // Mark X
				break;
			case OAPI_KEY_Y:
				agc.SetInputChannelBit(016, MarkY, 1);  // Mark Y
				break;
			case OAPI_KEY_E:
				agc.SetInputChannelBit(016, MarkReject_LM, 1);  // Mark Reject
				break;
			case OAPI_KEY_MINUS:
				//increase descent rate
				agc.SetInputChannelBit(016, DescendMinus, 1);
				break;
			case OAPI_KEY_EQUALS:
				//decrease descent rate
				agc.SetInputChannelBit(016, DescendPlus, 1);
				break;

			case OAPI_KEY_NUMPAD0:
				//TTCA Throttle up
				ttca_throttle_vel = 1;
				break;
			case OAPI_KEY_DECIMAL:
				//TTCA Throttle down
				ttca_throttle_vel = -1;
				break;

		}
	}else{
		switch(key){
			case OAPI_KEY_W:
			case OAPI_KEY_S:
				optics.ReticleMoved = 0;
				break;
			case OAPI_KEY_Q:
				agc.SetInputChannelBit(016, MarkX, 0);  // Mark X
				break;
			case OAPI_KEY_Y:
				agc.SetInputChannelBit(016, MarkY, 0);  // Mark Y
				break;
			case OAPI_KEY_E:
				agc.SetInputChannelBit(016, MarkReject_LM, 0);  // Mark Reject
				break;
			case OAPI_KEY_MINUS:
				//increase descent rate
				agc.SetInputChannelBit(016, DescendMinus, 0);
				break;
			case OAPI_KEY_EQUALS:
				//decrease descent rate
				agc.SetInputChannelBit(016, DescendPlus, 0);
				break;

			case OAPI_KEY_NUMPAD0:
				ttca_throttle_vel = 0;
				break;
			case OAPI_KEY_DECIMAL:
				ttca_throttle_vel = 0;
				break;
		}

	}

	if (KEYMOD_SHIFT(keystate) || KEYMOD_CONTROL(keystate) || !down) {
		return 0; 
	}

	switch (key) {

	case OAPI_KEY_K:
		bToggleHatch = true;
		return 1;

	case OAPI_KEY_E:
		return 0;

	case OAPI_KEY_6:
		viewpos = LMVIEW_CDR;
		SetView();
		return 1;

	case OAPI_KEY_7:
		viewpos = LMVIEW_LMP;
		SetView();
		return 1;

	//
	// Used by P64
	//


	case OAPI_KEY_COMMA:
		// move landing site left
		//agc.RedesignateTarget(1,1.0);
		ButtonClick();
		return 1;

	case OAPI_KEY_PERIOD:
		// move landing site right
		//agc.RedesignateTarget(1,-1.0);
		ButtonClick();
		return 1;

	case OAPI_KEY_HOME:
		//move the landing site downrange
		//agc.RedesignateTarget(0,-1.0);
		ButtonClick();
		return 1;

	case OAPI_KEY_END:
		//move the landing site closer
		//agc.RedesignateTarget(0,1.0);
		ButtonClick();
		return 1;

	//
	// Used by P66
	//

	case OAPI_KEY_MINUS:
		//increase descent rate
		//agc.ChangeDescentRate(-0.3077);
		return 1;

	case OAPI_KEY_EQUALS:
		//decrease descent rate
		//agc.ChangeDescentRate(0.3077);
		return 1;	

	//
	// Engine start and stop
	//

	case OAPI_KEY_ADD:
		//Engine Start Button
		ManualEngineStart.Push();
		ButtonClick();
		return 1;
	case OAPI_KEY_SUBTRACT:
		//Engine Stop Button
		ManualEngineStop.Push();
		ButtonClick();
		return 1;
	}

	return 0;
}

//
// Timestep code.
//

void LEM::clbkPreStep (double simt, double simdt, double mjd) {

	if (CheckPanelIdInTimestep) {
		oapiSetPanel(PanelId);
		CheckPanelIdInTimestep = false;
	}

	// RCS propellant pressurization
	// Descent Propellant Tank Prepressurization (ambient helium)
	// Descent Propellant Tank Prepressurization (supercritical helium)
	// Descent Propellant Tank Venting
	// Ascent Propellant Tank Pressurization
	if (status < 2) {
		if (StagingBoltsPyros.Blown() && StagingNutsPyros.Blown() && CableCuttingPyros.Blown()) {
			AbortFire();
			// Stage
			SeparateStage(stage);
		}
	}
}


void LEM::clbkPostStep(double simt, double simdt, double mjd)

{
	if (FirstTimestep)
	{
		DoFirstTimestep();
		FirstTimestep = false;
		return;
	}

	//
	// Panel flash counter.
	//

	if (MissionTime >= NextFlashUpdate) {
		PanelFlashOn = !PanelFlashOn;
		NextFlashUpdate = MissionTime + 0.25;
	}

	// Orbiter 2016 fix
	// Force GetWeightVector() to the correct value
	VESSELSTATUS vs;
	GetStatus(vs);
	if (vs.status == 1) {
		if (simt > 3 && simt < 4) {
			AddForce(_V(0, 0, -0.1), _V(0, 0, 0));
		}
	}
	
	//
	// If we switch focus to the astronaut immediately after creation, Orbitersound doesn't
	// play any sounds, or plays LEM sounds rather than astronauts sounds. We need to delay
	// the focus switch a few timesteps to allow it to initialise properly in the background.
	//

	if (SwitchFocusToLeva > 0 && hLEVA) {
		SwitchFocusToLeva--;
		if (!SwitchFocusToLeva) {
			oapiSetFocusObject(hLEVA);
		}
	}
	
	VECTOR3 RVEL = _V(0.0,0.0,0.0);
	GetRelativeVel(GetGravityRef(),RVEL);

	double deltat = oapiGetSimStep();

	MissionTime = MissionTime + deltat;
	SystemsTimestep(MissionTime, deltat);

	// DS20160916 Physical parameters updation
	CurrentFuelWeight = 0;
	if (ph_Asc != NULL) { CurrentFuelWeight += GetPropellantMass(ph_Asc); }
	if (ph_Dsc != NULL) { CurrentFuelWeight += GetPropellantMass(ph_Dsc); }
	if (ph_RCSA != NULL) { CurrentFuelWeight += GetPropellantMass(ph_RCSA); }
	if (ph_RCSB != NULL) { CurrentFuelWeight += GetPropellantMass(ph_RCSB); }
	// If the weight has changed by more than this value, update things.
	// The value is to be adjusted such that the updates are not too frequent (impacting framerate)
	//   but are sufficiently fine to keep the LGC happy.
	if ((LastFuelWeight - CurrentFuelWeight) > 5) {
		// Update physical parameters
		// Use SetPMI, ShiftCentreOfMass, etc.
		// All done!
		LastFuelWeight = CurrentFuelWeight;
	}

	actualFUEL = GetFuelMass()/GetMaxFuelMass()*100;

	if( toggleRCS){
			if(P44switch){
			SetAttitudeMode(2);
			toggleRCS =false;
			}
			else if (!P44switch){
			SetAttitudeMode(1);
			toggleRCS =false;
			}
		}
		if (GetAttitudeMode()==1){
		P44switch=false;
		}
		else if (GetAttitudeMode()==2 ){
		P44switch=true;
		}

	//
	// Play RCS sound in case of Orbiter's attitude control is disabled
	//

	RCSSoundTimestep();

	if (stage == 0)	{
		

	}else if (stage == 1 || stage == 5)	{

		if (CDREVA_IP) {
			if(!hLEVA) {
				ToggleEVA();
			}
		}

		if (ToggleEva && GroundContact()){
			ToggleEVA();
		}
		
		if (bToggleHatch){
			VESSELSTATUS vs;
			GetStatus(vs);
			if (vs.status == 1){
				//PlayVesselWave(Scontact,NOLOOP,255);
				//SetLmVesselHoverStage2(vessel);
			}
			bToggleHatch=false;
		}

		double vsAlt = GetAltitude(ALTMODE_GROUND);
		if (!ContactOK && (GroundContact() || (vsAlt < 1.0))) {

#ifdef DIRECTSOUNDENABLED
			if (!sevent.isValid())
#endif
				Scontact.play();

			SetEngineLevel(ENGINE_HOVER,0);
			ContactOK = true;

			SetLmLandedMesh();
		}

		if (CPswitch && HATCHswitch && EVAswitch && GroundContact()){
			ToggleEva = true;
			EVAswitch = false;
		}

		//sprintf (oapiDebugString(),"FUEL %d",GetPropellantMass(ph_Dsc));
	}

	MainPanel.timestep(MissionTime);
	checkControl.timestep(MissionTime, DummyEvents);

    // x15 landing sound management
#ifdef DIRECTSOUNDENABLED

    double     simtime       ;
	int        mode          ;
	double     timeremaining ;
	double     timeafterpdi  ;
	double     timetoapproach;
	char names [255]         ;
	int        todo          ;
	double     offset        ;
	int        newbuffer     ;

	
	if(simt >NextEventTime)
	{
        NextEventTime=simt+0.1;
	    agc.GetStatus(&simtime,&mode,&timeremaining,&timeafterpdi,&timetoapproach);
    	todo = sevent.play(soundlib,
			    this,
				names,
				&offset,
				&newbuffer,
		        simtime,
				MissionTime,
				mode,
				timeremaining,
				timeafterpdi,
				timetoapproach,
				NOLOOP,
				255);
        if (todo)
		{
           if(offset > 0.)
                sevent.PlaySound( names,newbuffer,offset);
		   else sevent.PlaySound( names,true,0);
		}
	} 
#endif
}

//
// Set GMBLswitch
//

void LEM::SetGimbal(bool setting)
{
	agc.SetInputChannelBit(032, DescentEngineGimbalsDisabled, setting);
	GMBLswitch = setting;
}

typedef union {
	struct {
		unsigned MissionTimerRunning:1;
		unsigned MissionTimerEnabled:1;
		unsigned EventTimerRunning:1;
		unsigned EventTimerEnabled:1;
	} u;
	unsigned long word;
} LEMMainState;

//
// Scenario state functions.
//

void LEM::clbkLoadStateEx (FILEHANDLE scn, void *vs)

{
	char *line;
	int	SwitchState;
	float ftcp;
	
	while (oapiReadScenario_nextline (scn, line)) {
        if (!strnicmp (line, "CONFIGURATION", 13)) {
            sscanf (line+13, "%d", &status);
		}
		else if (!strnicmp (line, "EVA", 3)) {
			CDREVA_IP = true;
		} 
		else if (!strnicmp (line, "CSWITCH", 7)) {
            SwitchState = 0;
			sscanf (line+7, "%d", &SwitchState);
			SetCSwitchState(SwitchState);
		} 
		else if (!strnicmp (line, "SSWITCH", 7)) {
            SwitchState = 0;
			sscanf (line+7, "%d", &SwitchState);
			SetSSwitchState(SwitchState);
		} 
		else if (!strnicmp (line, "LPSWITCH", 8)) {
            SwitchState = 0;
			sscanf (line+8, "%d", &SwitchState);
			SetLPSwitchState(SwitchState);
		} 
		else if (!strnicmp(line, "MISSNTIME", 9)) {
            sscanf (line+9, "%f", &ftcp);
			MissionTime = ftcp;
		} 
		else if (!strnicmp(line, "MTD", 3)) {
            sscanf (line+3, "%f", &ftcp);
			MissionTimerDisplay.SetTime(ftcp);
		} 
		else if (!strnicmp(line, "ETD", 3)) {
            sscanf (line+3, "%f", &ftcp);
			EventTimerDisplay.SetTime(ftcp);
		}
		else if (!strnicmp(line, "UNMANNED", 8)) {
			int i;
			sscanf(line + 8, "%d", &i);
			Crewed = (i == 0);
		}
		else if (!strnicmp (line, "LANG", 4)) {
			strncpy (AudioLanguage, line + 5, 64);
		}
		else if (!strnicmp (line, "APOLLONO", 8)) {
			sscanf (line+8, "%d", &ApolloNo);
		}
		else if (!strnicmp (line, "LANDED", 6)) {
			sscanf (line+6, "%d", &Landed);
		}
		else if (!strnicmp(line, "DSCFUEL", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			DescentFuelMassKg = ftcp;
		}
		else if (!strnicmp(line, "ASCFUEL", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			AscentFuelMassKg = ftcp;
		}
		else if (!strnicmp (line, "FDAIDISABLED", 12)) {
			sscanf (line + 12, "%i", &fdaiDisabled);
		}
		else if (!strnicmp(line, "ORDEALENABLED", 13)) {
			sscanf(line + 13, "%i", &ordealEnabled);
		}
		else if (!strnicmp(line, "VAGCCHECKLISTAUTOENABLED", 24)) {
			int i;
			sscanf(line + 24, "%d", &i);
			VAGCChecklistAutoEnabled = (i != 0);
		}
		else if (!strnicmp(line, DSKY_START_STRING, sizeof(DSKY_START_STRING))) {
			dsky.LoadState(scn, DSKY_END_STRING);
		}
		else if (!strnicmp(line, AGC_START_STRING, sizeof(AGC_START_STRING))) {
			agc.LoadState(scn);
		}
		else if (!strnicmp(line, IMU_START_STRING, sizeof(IMU_START_STRING))) {
			imu.LoadState(scn);
		}
		else if (!strnicmp (line, "ECA_1A_START",sizeof("ECA_1A_START"))) {
			ECA_1a.LoadState(scn,"ECA_1A_END");
		}
		else if (!strnicmp (line, "ECA_2A_START",sizeof("ECA_2A_START"))) {
			ECA_2a.LoadState(scn,"ECA_2A_END");
		}
		else if (!strnicmp (line, "ECA_1B_START",sizeof("ECA_1B_START"))) {
			ECA_1b.LoadState(scn,"ECA_1B_END");
		}
		else if (!strnicmp (line, "ECA_2B_START",sizeof("ECA_2B_START"))) {
			ECA_2b.LoadState(scn,"ECA_2B_END");
		}
		else if (!strnicmp (line, "ECA_3A_START",sizeof("ECA_3A_START"))) {
			ECA_3a.LoadState(scn,"ECA_3A_END");
		}
		else if (!strnicmp (line, "ECA_4A_START",sizeof("ECA_4A_START"))) {
			ECA_4a.LoadState(scn,"ECA_4A_END");
		}
		else if (!strnicmp (line, "ECA_3B_START",sizeof("ECA_3B_START"))) {
			ECA_3b.LoadState(scn,"ECA_3B_END");
		}
		else if (!strnicmp (line, "ECA_4B_START",sizeof("ECA_4B_START"))) {
			ECA_4b.LoadState(scn,"ECA_4B_END");
		}
		else if (!strnicmp (line, "PANEL_ID", 8)) { 
			sscanf (line+8, "%d", &PanelId);
		}
		else if (!strnicmp (line, "STATE", 5)) {
			LEMMainState state;
			sscanf (line+5, "%d", &state.word);

			MissionTimerDisplay.SetRunning(state.u.MissionTimerRunning != 0);
			MissionTimerDisplay.SetEnabled(state.u.MissionTimerEnabled != 0);
			EventTimerDisplay.SetRunning(state.u.EventTimerRunning != 0);
			EventTimerDisplay.SetEnabled(state.u.EventTimerEnabled != 0);
		} 
        else if (!strnicmp (line, PANELSWITCH_START_STRING, strlen(PANELSWITCH_START_STRING))) { 
			PSH.LoadState(scn);	
		}
		else if (!strnicmp (line, "LEM_EDS_START",sizeof("LEM_EDS_START"))) {
			eds.LoadState(scn,"LEM_EDS_END");
		}
		else if (!strnicmp (line, "LEM_RR_START",sizeof("LEM_RR_START"))) {
			RR.LoadState(scn,"LEM_RR_END");
		}
		else if (!strnicmp(line, "LEM_LR_START", sizeof("LEM_LR_START"))) {
			LR.LoadState(scn, "LEM_LR_END");
		}
		else if (!strnicmp(line, FDAI_START_STRING, sizeof(FDAI_START_STRING))) {
			fdaiLeft.LoadState(scn, FDAI_END_STRING);
		}
		else if (!strnicmp(line, FDAI2_START_STRING, sizeof(FDAI2_START_STRING))) {
			fdaiRight.LoadState(scn, FDAI2_END_STRING);
		}
		else if (!strnicmp(line, "DPS_BEGIN", sizeof("DPS_BEGIN"))) {
			DPS.LoadState(scn, "DPS_END");
		}
		else if (!strnicmp(line, "DPSGIMBALACTUATOR_PITCH_BEGIN", sizeof("DPSGIMBALACTUATOR_PITCH_BEGIN"))) {
			DPS.pitchGimbalActuator.LoadState(scn);
		}
		else if (!strnicmp(line, "DPSGIMBALACTUATOR_ROLL_BEGIN", sizeof("DPSGIMBALACTUATOR_ROLL_BEGIN"))) {
			DPS.rollGimbalActuator.LoadState(scn);
		}
		else if (!strnicmp(line, "DECA_BEGIN", sizeof("DECA_BEGIN"))) {
			deca.LoadState(scn);
		}
		else if (!strnicmp(line, "SCCA1_BEGIN", sizeof("SCCA_BEGIN"))) {
			scca1.LoadState(scn, "SCCA_END");
		}
		else if (!strnicmp(line, "SCCA2_BEGIN", sizeof("SCCA_BEGIN"))) {
			scca2.LoadState(scn, "SCCA_END");
		}
		else if (!strnicmp(line, "SCCA3_BEGIN", sizeof("SCCA_BEGIN"))) {
			scca3.LoadState(scn, "SCCA_END");
		}
		else if (!strnicmp(line, "APS_BEGIN", sizeof("APS_BEGIN"))) {
			APS.LoadState(scn, "APS_END");
		}
		else if (!strnicmp(line, CROSSPOINTER_LEFT_START_STRING, sizeof(CROSSPOINTER_LEFT_START_STRING))) {
			crossPointerLeft.LoadState(scn);
		}
		else if (!strnicmp(line, CROSSPOINTER_RIGHT_START_STRING, sizeof(CROSSPOINTER_RIGHT_START_STRING))) {
			crossPointerRight.LoadState(scn);
		}
		else if (!strnicmp(line, ORDEAL_START_STRING, sizeof(ORDEAL_START_STRING))) {
			ordeal.LoadState(scn);
		}
		else if (!strnicmp(line, MECHACCEL_START_STRING, sizeof(MECHACCEL_START_STRING))) {
			mechanicalAccelerometer.LoadState(scn);
		}
		else if (!strnicmp(line, ATCA_START_STRING, sizeof(ATCA_START_STRING))) {
			atca.LoadState(scn);
		}
        else if (!strnicmp (line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
			Panelsdk.Load(scn);			//send the loading to the Panelsdk
		}
		else if (!strnicmp (line, ChecklistControllerStartString, strlen(ChecklistControllerStartString)))
		{
			checkControl.load(scn);
		}
		else 
		{
            ParseScenarioLineEx (line, vs);
        }
    }

	switch (status) {
	case 0:
		stage=0;
		SetLmVesselDockStage();
		break;

	case 1:
		stage=1;
		SetLmVesselHoverStage();

		if (CDREVA_IP){
			SetupEVA();
		}
		break;

	case 2:
		stage=2;
		SetLmAscentHoverStage();
		break;
	}

	// Descent Stage Deadface Bus Stubs wire to the ECAs
	// I was doing this in SystemsInit which is wrong.
	if(stage < 2){
		DES_LMPs28VBusA.WireTo(&ECA_1a);
		DES_LMPs28VBusB.WireTo(&ECA_1b);
		DES_CDRs28VBusA.WireTo(&ECA_2a); 
		DES_CDRs28VBusB.WireTo(&ECA_2b); 
		DSCBattFeedTB.SetState(1);
	}else{
		DES_LMPs28VBusA.Disconnect();
		DES_LMPs28VBusB.Disconnect();
		DES_CDRs28VBusA.Disconnect();
		DES_CDRs28VBusB.Disconnect();
		DSCBattFeedTB.SetState(0);
	}
	// SOVs open by default
	SetValveState(LEM_RCS_MAIN_SOV_A,true);
	SetValveState(LEM_RCS_MAIN_SOV_B,true);

	//
	// Pass on the mission number and realism setting to the AGC.
	//

	agc.SetMissionInfo(ApolloNo);

	///
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

	//
	// Load sounds, this is mandatory if loading in cockpit view, 
	// because OrbiterSound crashes when loading sounds during clbkLoadPanel
	//
	LoadDefaultSounds();

	// Also cause the AC busses to wire up
	switch(EPSInverterSwitch.GetState()){
		case THREEPOSSWITCH_UP:      // INV 2
			INV_1.active = 0;
			INV_2.active = 1; 
			ACBusA.WireTo(&AC_A_INV_2_FEED_CB);
			ACBusB.WireTo(&AC_B_INV_2_FEED_CB);
			break;
		case THREEPOSSWITCH_CENTER:  // INV 1
			INV_1.active = 1;
			INV_2.active = 0; 
			ACBusA.WireTo(&AC_A_INV_1_FEED_CB);
			ACBusB.WireTo(&AC_B_INV_1_FEED_CB);
			break;
		case THREEPOSSWITCH_DOWN:    // OFF	
			break;                   // Handled later
	}
	HRESULT         hr;

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
	}else{
		// We can't print an error message this early in initialization, so save this reason for later investigation.
		dx8_failure = hr;
	}

}

void LEM::clbkSetClassCaps (FILEHANDLE cfg) {

	VSEnableCollisions(GetHandle(),"ProjectApollo");
	// Switch to compatible dock mode 
	SetDockMode(0);

	SetLmVesselDockStage();

	//
	// Scan the launchpad config file.
	//
	char *line, buffer[1000];

	sprintf(buffer, "%s.launchpad.cfg", GetClassName());
	FILEHANDLE hFile = oapiOpenFile(buffer, FILE_IN, CONFIG);

	while (oapiReadScenario_nextline(hFile, line)) {
		ProcessConfigFileLine(hFile, line);
	}
	oapiCloseFile(hFile, FILE_IN);
}


bool LEM::ProcessConfigFileLine(FILEHANDLE scn, char *line)

{
	int i;

	if (!strnicmp(line, "FDAIDISABLED", 12)) {
		sscanf(line + 12, "%i", &fdaiDisabled);
	}
	else if (!strnicmp(line, "FDAISMOOTH", 10)) {
			sscanf(line + 10, "%i", &fdaiSmooth);
	}
	else if (!strnicmp (line, "MULTITHREAD", 11)) {
		int value;
		sscanf (line+11, "%d", &value);
		isMultiThread=(value>0)?true:false;
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
	else if (!strnicmp(line, "VAGCCHECKLISTAUTOSLOW", 21)) {
		sscanf(line + 21, "%i", &i);
		VAGCChecklistAutoSlow = (i != 0);
	}
	return true;
}

void LEM::SetStateEx(const void *status)

{
	const VESSELSTATUS2 *vslm = static_cast<const VESSELSTATUS2 *> (status);

	DefSetStateEx(status);
}

void LEM::clbkSaveState (FILEHANDLE scn)

{
	SaveDefaultState (scn);	
	oapiWriteScenario_int (scn, "CONFIGURATION", status);
	if (CDREVA_IP){
		oapiWriteScenario_int (scn, "EVA", int(TO_EVA));
	}

	oapiWriteScenario_int (scn, "CSWITCH",  GetCSwitchState());
	oapiWriteScenario_int (scn, "SSWITCH",  GetSSwitchState());
	oapiWriteScenario_int (scn, "LPSWITCH",  GetLPSwitchState());
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "MTD", MissionTimerDisplay.GetTime());
	oapiWriteScenario_float (scn, "ETD", EventTimerDisplay.GetTime());
	oapiWriteScenario_string (scn, "LANG", AudioLanguage);
	oapiWriteScenario_int (scn, "PANEL_ID", PanelId);	

	oapiWriteScenario_int (scn, "APOLLONO", ApolloNo);
	oapiWriteScenario_int (scn, "LANDED", Landed);
	oapiWriteScenario_int (scn, "FDAIDISABLED", fdaiDisabled);
	oapiWriteScenario_int(scn, "ORDEALENABLED", ordealEnabled);

	oapiWriteScenario_int(scn, "VAGCCHECKLISTAUTOENABLED", VAGCChecklistAutoEnabled);

	oapiWriteScenario_float (scn, "DSCFUEL", DescentFuelMassKg);
	oapiWriteScenario_float (scn, "ASCFUEL", AscentFuelMassKg);

	if (!Crewed) {
		oapiWriteScenario_int (scn, "UNMANNED", 1);
	}

	//
	// Main state flags are packed into a 32-bit value.
	//

	LEMMainState state;
	state.word = 0;

	state.u.MissionTimerRunning = MissionTimerDisplay.IsRunning();
	state.u.MissionTimerEnabled = MissionTimerDisplay.IsEnabled();
	state.u.EventTimerEnabled = EventTimerDisplay.IsEnabled();
	state.u.EventTimerRunning = EventTimerDisplay.IsRunning();

	oapiWriteScenario_int (scn, "STATE", state.word);

	dsky.SaveState(scn, DSKY_START_STRING, DSKY_END_STRING);
	agc.SaveState(scn);
	imu.SaveState(scn);

	//
	// Save the Panel SDK state.
	//

	Panelsdk.Save(scn);
	
	//
	// Save the state of the switches
	//

	PSH.SaveState(scn);	

	// Save ECAs
	ECA_1a.SaveState(scn,"ECA_1A_START","ECA_1A_END");
	ECA_1b.SaveState(scn,"ECA_1B_START","ECA_1B_END");
	ECA_2a.SaveState(scn,"ECA_2A_START","ECA_2A_END");
	ECA_2b.SaveState(scn,"ECA_2B_START","ECA_2B_END");
	ECA_3a.SaveState(scn,"ECA_3A_START","ECA_3A_END");
	ECA_3b.SaveState(scn,"ECA_3B_START","ECA_3B_END");
	ECA_4a.SaveState(scn,"ECA_4A_START","ECA_4A_END");
	ECA_4b.SaveState(scn,"ECA_4B_START","ECA_4B_END");

	// Save EDS
	eds.SaveState(scn,"LEM_EDS_START","LEM_EDS_END");
	RR.SaveState(scn,"LEM_RR_START","LEM_RR_END");
	LR.SaveState(scn, "LEM_LR_START", "LEM_LR_END");

	// Save FDAIs
	fdaiLeft.SaveState(scn, FDAI_START_STRING, FDAI_END_STRING);
	fdaiRight.SaveState(scn, FDAI2_START_STRING, FDAI2_END_STRING);

	//Save DPS
	DPS.SaveState(scn, "DPS_BEGIN", "DPS_END");
	//Save pitch and roll gimbal actuators
	oapiWriteLine(scn, "DPSGIMBALACTUATOR_PITCH_BEGIN");
	DPS.pitchGimbalActuator.SaveState(scn);
	oapiWriteLine(scn, "DPSGIMBALACTUATOR_ROLL_BEGIN");
	DPS.rollGimbalActuator.SaveState(scn);
	oapiWriteLine(scn, "DECA_BEGIN");
	deca.SaveState(scn);
	scca1.SaveState(scn, "SCCA1_BEGIN", "SCCA_END");
	scca2.SaveState(scn, "SCCA2_BEGIN", "SCCA_END");
	scca3.SaveState(scn, "SCCA3_BEGIN", "SCCA_END");
	APS.SaveState(scn, "APS_BEGIN", "APS_END");
	oapiWriteLine(scn, CROSSPOINTER_LEFT_START_STRING);
	crossPointerLeft.SaveState(scn);
	oapiWriteLine(scn, CROSSPOINTER_RIGHT_START_STRING);
	crossPointerRight.SaveState(scn);
	ordeal.SaveState(scn);
	mechanicalAccelerometer.SaveState(scn);
	atca.SaveState(scn);
	checkControl.save(scn);
}

bool LEM::clbkLoadGenericCockpit ()

{
	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
	oapiCameraSetCockpitDir(0,0);

	InVC = false;
	InPanel = false;
	SetView();

	return true;
}

//
// Transfer important data from the CSM to the LEM when the LEM is first
// created.
//

bool LEM::SetupPayload(PayloadSettings &ls)

{
	char CSMName[64];

	MissionTime = ls.MissionTime;
	MissionTimerDisplay.SetTime(MissionTime);

	agc.SetDesiredLanding(ls.LandingLatitude, ls.LandingLongitude, ls.LandingAltitude);
	strncpy (AudioLanguage, ls.language, 64);
	strncpy (CSMName, ls.CSMName, 64);

	Crewed = ls.Crewed;
	AutoSlow = ls.AutoSlow;
	ApolloNo = ls.MissionNo;

	DescentFuelMassKg = ls.DescentFuelKg;
	AscentFuelMassKg = ls.AscentFuelKg;

	agc.SetMissionInfo(ApolloNo, CSMName);

	// Initialize the checklist Controller in accordance with scenario settings.
	checkControl.init(ls.checklistFile, true);
	checkControl.autoExecute(ls.checkAutoExecute);

	//Set the transfered payload setting for the checklist controller
	VAGCChecklistAutoEnabled = ls.checkAutoExecute;

	// Sounds are initialized during the first timestep

	return true;
}

void LEM::PadLoad(unsigned int address, unsigned int value)
{ 
	agc.PadLoad(address, value);
}

// *** REACTION CONTROL SYSTEM ***
void LEM::CheckRCS(){
	/* sprintf(oapiDebugString(),"CheckRCS: %d %d %f %f",GetValveState(LEM_RCS_MAIN_SOV_A),GetValveState(LEM_RCS_MAIN_SOV_B),
		GetPropellantMass(ph_DscRCSA),GetPropellantMass(ph_DscRCSB)); */	
	if(GetValveState(LEM_RCS_MAIN_SOV_A)){
		SetThrusterResource(th_rcs[0],ph_RCSA);
		SetThrusterResource(th_rcs[1],ph_RCSA);
		SetThrusterResource(th_rcs[6],ph_RCSA);
		SetThrusterResource(th_rcs[7],ph_RCSA);
		SetThrusterResource(th_rcs[8],ph_RCSA);
		SetThrusterResource(th_rcs[9],ph_RCSA);
		SetThrusterResource(th_rcs[14],ph_RCSA);
		SetThrusterResource(th_rcs[15],ph_RCSA);
	}else{
		SetThrusterResource(th_rcs[0],NULL);
		SetThrusterResource(th_rcs[1],NULL);
		SetThrusterResource(th_rcs[6],NULL);
		SetThrusterResource(th_rcs[7],NULL);
		SetThrusterResource(th_rcs[8],NULL);
		SetThrusterResource(th_rcs[9],NULL);
		SetThrusterResource(th_rcs[14],NULL);
		SetThrusterResource(th_rcs[15],NULL);
	}
	if(GetValveState(LEM_RCS_MAIN_SOV_B)){
		SetThrusterResource(th_rcs[2],ph_RCSB);
		SetThrusterResource(th_rcs[3],ph_RCSB);
		SetThrusterResource(th_rcs[4],ph_RCSB);
		SetThrusterResource(th_rcs[5],ph_RCSB);
		SetThrusterResource(th_rcs[10],ph_RCSB);
		SetThrusterResource(th_rcs[11],ph_RCSB);
		SetThrusterResource(th_rcs[12],ph_RCSB);
		SetThrusterResource(th_rcs[13],ph_RCSB);
	}else{
		SetThrusterResource(th_rcs[2],NULL);
		SetThrusterResource(th_rcs[3],NULL);
		SetThrusterResource(th_rcs[4],NULL);
		SetThrusterResource(th_rcs[5],NULL);
		SetThrusterResource(th_rcs[10],NULL);
		SetThrusterResource(th_rcs[11],NULL);
		SetThrusterResource(th_rcs[12],NULL);
		SetThrusterResource(th_rcs[13],NULL);
	}
	return;
}

void LEM::SetRCSJet(int jet, bool fire) {
	SetThrusterLevel(th_rcs[jet], fire);
}


// Set level of RCS thruster, using primary coils
void LEM::SetRCSJetLevelPrimary(int jet, double level) {
	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/
	// The thruster is a Marquardt R-4D, which uses 46 watts @ 28 volts to fire.
	// This applies to the SM as well, someone should probably tell them about this.
	// RCS pressurized?

	//Direct override
	if (atca.GetDirectRollActive())
	{
		if (jet == 0 || jet == 3 || jet == 4 || jet == 7 || jet == 8 || jet == 11 || jet == 12 || jet == 15)
		{
			return;
		}
	}
	if (atca.GetDirectPitchActive())
	{
		if (jet == 0 || jet == 3 || jet == 4 || jet == 7 || jet == 8 || jet == 11 || jet == 12 || jet == 15)
		{
			return;
		}
	}
	if (atca.GetDirectYawActive())
	{
		if (jet == 1 || jet == 2 || jet == 5 || jet == 6 || jet == 9 || jet == 10 || jet == 13 || jet == 14)
		{
			return;
		}
	}

	// Is this thruster on?	
	switch(jet){
		// SYS A
		case 0: // QUAD 1
		case 1:
			if(RCS_A_QUAD1_TCA_CB.Voltage() > 24){ RCS_A_QUAD1_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 6: // QUAD 2
		case 7:
			if(RCS_A_QUAD2_TCA_CB.Voltage() > 24){ RCS_A_QUAD2_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 8: // QUAD 3
		case 9:
			if(RCS_A_QUAD3_TCA_CB.Voltage() > 24){ RCS_A_QUAD3_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 14: // QUAD 4
		case 15:
			if(RCS_A_QUAD4_TCA_CB.Voltage() > 24){ RCS_A_QUAD4_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;

		// SYS B
		case 2: // QUAD 1
		case 3:
			if(RCS_B_QUAD1_TCA_CB.Voltage() > 24){ RCS_B_QUAD1_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 4: // QUAD 2
		case 5:
			if(RCS_B_QUAD2_TCA_CB.Voltage() > 24){ RCS_B_QUAD2_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 10: // QUAD 3
		case 11:
			if(RCS_B_QUAD3_TCA_CB.Voltage() > 24){ RCS_B_QUAD3_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 12: // QUAD 4
		case 13:
			if(RCS_B_QUAD4_TCA_CB.Voltage() > 24){ RCS_B_QUAD4_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
	}

	SetThrusterLevel(th_rcs[jet], level);
}

void LEM::RCSSoundTimestep() {

	// In case of disabled Orbiter attitude thruster groups OrbiterSound plays no
	// engine sound, so this needs to be done manually

	int i;
	bool on = false;
	// LM RCS
	for (i = 0; i < 16; i++) {
		if (th_rcs[i]) {
			if (GetThrusterLevel(th_rcs[i])) on = true;
		}
	}
	// Play/stop sounds
	if (on) {
		if (RCSFireSound.isPlaying()) {
			RCSSustainSound.play(LOOP);
		}
		else if (!RCSSustainSound.isPlaying()) {
			RCSFireSound.play();
		}
	}
	else {
		RCSSustainSound.stop();
	}
}