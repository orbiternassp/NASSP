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
#define ORBITER_MODULE

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "nassputils.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"
#include "sm.h"
#include "sivb.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "papi.h"
#include "mcc.h"
#include "mccvessel.h"
#include "LVDC.h"
#include "iu.h"
#include "Mission.h"

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

using namespace nassp;

//extern FILE *PanelsdkLogFile;

#define CSM_AXIS_INPUT_CNT  53
VesimInputDefinition vesim_csm_inputs[CSM_AXIS_INPUT_CNT] = {
	{ CSM_AXIS_INPUT_RHC_R,       "RHC Roll",                      VESIM_INPUTTYPE_AXIS,     VESIM_DEFAULT_AXIS_VALUE, false },
	{ CSM_AXIS_INPUT_RHC_P,       "RHC Pitch",                     VESIM_INPUTTYPE_AXIS,     VESIM_DEFAULT_AXIS_VALUE, false },
	{ CSM_AXIS_INPUT_RHC_Y,       "RHC Yaw",                       VESIM_INPUTTYPE_AXIS,     VESIM_DEFAULT_AXIS_VALUE, false },
	{ CSM_AXIS_INPUT_THC_X,       "THC X",                         VESIM_INPUTTYPE_AXIS,     VESIM_DEFAULT_AXIS_VALUE, false },
	{ CSM_AXIS_INPUT_THC_Y,       "THC Y",                         VESIM_INPUTTYPE_AXIS,     VESIM_DEFAULT_AXIS_VALUE, false },
	{ CSM_AXIS_INPUT_THC_Z,       "THC Z",                         VESIM_INPUTTYPE_AXIS,     VESIM_DEFAULT_AXIS_VALUE, false },
	{ CSM_BUTTON_INPUT_THC_CCW,   "THC Move CCW",                  VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_INPUT_THC_CW,    "THC Move CW",                   VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_ROT_LIN,         "Rotation/Translation toggle",   VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_PRO,       "Main DSKY PRO",                 VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_KEY_REL,   "Main DSKY KEY REL",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_VERB,      "Main DSKY VERB",                VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NOUN,      "Main DSKY NOUN",                VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_ENTR,      "Main DSKY ENTR",                VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_CLR,       "Main DSKY CLR",                 VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_PLUS,      "Main DSKY +",                   VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_MINUS,     "Main DSKY -",                   VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_RSET,      "Main DSKY RSET",                VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_0,     "Main DSKY Number 0",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_1,     "Main DSKY Number 1",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_2,     "Main DSKY Number 2",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_3,     "Main DSKY Number 3",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_4,     "Main DSKY Number 4",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_5,     "Main DSKY Number 5",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_6,     "Main DSKY Number 6",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_7,     "Main DSKY Number 7",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_8,     "Main DSKY Number 8",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY1_NUM_9,     "Main DSKY Number 9",            VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_PRO,       "LEB DSKY PRO",                  VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_KEY_REL,   "LEB DSKY KEY REL",              VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_VERB,      "LEB DSKY VERB",                 VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NOUN,      "LEB DSKY NOUN",                 VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_ENTR,      "LEB DSKY ENTR",                 VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_CLR,       "LEB DSKY CLR",                  VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_PLUS,      "LEB DSKY +",                    VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_MINUS,     "LEB DSKY -",                    VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_RSET,      "LEB DSKY RSET",                 VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_0,     "LEB DSKY Number 0",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_1,     "LEB DSKY Number 1",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_2,     "LEB DSKY Number 2",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_3,     "LEB DSKY Number 3",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_4,     "LEB DSKY Number 4",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_5,     "LEB DSKY Number 5",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_6,     "LEB DSKY Number 6",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_7,     "LEB DSKY Number 7",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_8,     "LEB DSKY Number 8",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DSKY2_NUM_9,     "LEB DSKY Number 9",             VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DIR_ULL,         "Direct Ullage",                 VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_THR_ON,          "Thrust On",                     VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DVTA_NORM,       "DV Thrust A Norm",              VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DVTA_OFF,        "DV Thrust A Off",               VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DVTB_NORM,       "DV Thrust B Norm",              VESIM_INPUTTYPE_BUTTON,  0, true },
	{ CSM_BUTTON_DVTB_OFF,        "DV Thrust B Off",               VESIM_INPUTTYPE_BUTTON,  0, true }
};

void cbCSMVesim(int inputID, int eventType, int newValue, void *pdata) {
	Saturn *pSaturn = (Saturn *)pdata;
	if (eventType == VESIM_EVTTYPE_BUTTON_ON) {
		switch (inputID) {
		case CSM_BUTTON_ROT_LIN:			
			if (pSaturn->GetAttitudeMode() == RCS_ROT)
				pSaturn->SetAttitudeMode(RCS_LIN);
			else
				pSaturn->SetAttitudeMode(RCS_ROT);
			break;
		case CSM_BUTTON_INPUT_THC_CCW:
			pSaturn->MoveTHC(1);
			break;
		case CSM_BUTTON_INPUT_THC_CW:
			pSaturn->MoveTHC(0);
			break;
		case CSM_BUTTON_DSKY1_PRO:
			pSaturn->dsky.ProceedPressed();
			break;
		case CSM_BUTTON_DSKY1_KEY_REL:
			pSaturn->dsky.KeyRel();
			break;
		case CSM_BUTTON_DSKY1_VERB:
			pSaturn->dsky.VerbPressed();
			break;
		case CSM_BUTTON_DSKY1_NOUN:
			pSaturn->dsky.NounPressed();
			break;
		case CSM_BUTTON_DSKY1_ENTR:
			pSaturn->dsky.EnterPressed();
			break;
		case CSM_BUTTON_DSKY1_CLR:
			pSaturn->dsky.ClearPressed();
			break;
		case CSM_BUTTON_DSKY1_PLUS:
			pSaturn->dsky.PlusPressed();
			break;
		case CSM_BUTTON_DSKY1_MINUS:
			pSaturn->dsky.MinusPressed();
			break;
		case CSM_BUTTON_DSKY1_RSET:
			pSaturn->dsky.ResetPressed();
			break;
		case CSM_BUTTON_DSKY1_NUM_0:
			pSaturn->dsky.NumberPressed(0);
			break;
		case CSM_BUTTON_DSKY1_NUM_1:
			pSaturn->dsky.NumberPressed(1);
			break;
		case CSM_BUTTON_DSKY1_NUM_2:
			pSaturn->dsky.NumberPressed(2);
			break;
		case CSM_BUTTON_DSKY1_NUM_3:
			pSaturn->dsky.NumberPressed(3);
			break;
		case CSM_BUTTON_DSKY1_NUM_4:
			pSaturn->dsky.NumberPressed(4);
			break;
		case CSM_BUTTON_DSKY1_NUM_5:
			pSaturn->dsky.NumberPressed(5);
			break;
		case CSM_BUTTON_DSKY1_NUM_6:
			pSaturn->dsky.NumberPressed(6);
			break;
		case CSM_BUTTON_DSKY1_NUM_7:
			pSaturn->dsky.NumberPressed(7);
			break;
		case CSM_BUTTON_DSKY1_NUM_8:
			pSaturn->dsky.NumberPressed(8);
			break;
		case CSM_BUTTON_DSKY1_NUM_9:
			pSaturn->dsky.NumberPressed(9);
			break;
		case CSM_BUTTON_DSKY2_PRO:
			pSaturn->dsky2.ProceedPressed();
			break;
		case CSM_BUTTON_DSKY2_KEY_REL:
			pSaturn->dsky2.KeyRel();
			break;
		case CSM_BUTTON_DSKY2_VERB:
			pSaturn->dsky2.VerbPressed();
			break;
		case CSM_BUTTON_DSKY2_NOUN:
			pSaturn->dsky2.NounPressed();
			break;
		case CSM_BUTTON_DSKY2_ENTR:
			pSaturn->dsky2.EnterPressed();
			break;
		case CSM_BUTTON_DSKY2_CLR:
			pSaturn->dsky2.ClearPressed();
			break;
		case CSM_BUTTON_DSKY2_PLUS:
			pSaturn->dsky2.PlusPressed();
			break;
		case CSM_BUTTON_DSKY2_MINUS:
			pSaturn->dsky2.MinusPressed();
			break;
		case CSM_BUTTON_DSKY2_RSET:
			pSaturn->dsky2.ResetPressed();
			break;
		case CSM_BUTTON_DSKY2_NUM_0:
			pSaturn->dsky2.NumberPressed(0);
			break;
		case CSM_BUTTON_DSKY2_NUM_1:
			pSaturn->dsky2.NumberPressed(1);
			break;
		case CSM_BUTTON_DSKY2_NUM_2:
			pSaturn->dsky2.NumberPressed(2);
			break;
		case CSM_BUTTON_DSKY2_NUM_3:
			pSaturn->dsky2.NumberPressed(3);
			break;
		case CSM_BUTTON_DSKY2_NUM_4:
			pSaturn->dsky2.NumberPressed(4);
			break;
		case CSM_BUTTON_DSKY2_NUM_5:
			pSaturn->dsky2.NumberPressed(5);
			break;
		case CSM_BUTTON_DSKY2_NUM_6:
			pSaturn->dsky2.NumberPressed(6);
			break;
		case CSM_BUTTON_DSKY2_NUM_7:
			pSaturn->dsky2.NumberPressed(7);
			break;
		case CSM_BUTTON_DSKY2_NUM_8:
			pSaturn->dsky2.NumberPressed(8);
			break;
		case CSM_BUTTON_DSKY2_NUM_9:
			pSaturn->dsky2.NumberPressed(9);
			break;
		case CSM_BUTTON_DIR_ULL:
			pSaturn->DirectUllageButton.VesimSwitchTo(1);
			break;
		case CSM_BUTTON_THR_ON:
			pSaturn->ThrustOnButton.VesimSwitchTo(1);
			break;
		case CSM_BUTTON_DVTA_NORM:
			pSaturn->dVThrust1Switch.VesimSwitchTo(TOGGLESWITCH_UP);
			break;
		case CSM_BUTTON_DVTA_OFF:
			pSaturn->dVThrust1Switch.VesimSwitchTo(TOGGLESWITCH_DOWN);
			break;
		case CSM_BUTTON_DVTB_NORM:
			pSaturn->dVThrust2Switch.VesimSwitchTo(TOGGLESWITCH_UP);
			break;
		case CSM_BUTTON_DVTB_OFF:
			pSaturn->dVThrust2Switch.VesimSwitchTo(TOGGLESWITCH_DOWN);
			break;
		}
	}
	else if (eventType == VESIM_EVTTYPE_BUTTON_OFF) {
		switch (inputID) {		
		case CSM_BUTTON_DSKY1_PRO:
			pSaturn->dsky.ProceedReleased();
			break;
		case CSM_BUTTON_DSKY2_PRO:
			pSaturn->dsky2.ProceedReleased();
			break;
		case CSM_BUTTON_DIR_ULL:
			pSaturn->DirectUllageButton.VesimSwitchTo(0);
			break;
		case CSM_BUTTON_THR_ON:
			pSaturn->ThrustOnButton.VesimSwitchTo(0);
			break;
		}
	}
}

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
	inertialData(this),
	agc(soundlib, dsky, dsky2, imu, scdu, tcdu, Panelsdk),
	dsky(soundlib, agc, 015),
	dsky2(soundlib, agc, 016),
	CMCDCBusFeeder("CMC-DCBus-Feeder", Panelsdk),
	imu(agc, Panelsdk, inertialData),
	scdu(agc, RegOPTX, 0140, 2),
	tcdu(agc, RegOPTY, 0141, 2),
	mechanicalAccelerometer(inertialData),
	cws(SMasterAlarm, Bclick, Panelsdk),
	dockingprobe(0, SDockingCapture, SDockingLatch, SDockingExtend, SUndock, CrashBumpS, Panelsdk),
	MissionTimerDisplay(Panelsdk),
	MissionTimer306Display(Panelsdk),
	EventTimerDisplay(Panelsdk),
	EventTimer306Display(Panelsdk),
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
	SCSLogicBus1("SCS-Logic-Bus-1", &SCSLogicBus1Feeder),
	SCSLogicBus1Feeder("SCS-Logic-Bus-1-Feeder", Panelsdk),
	SCSLogicBus2("SCS-Logic-Bus-2", NULL),
	SCSLogicBus2Feeder("SCS-Logic-Bus-2-Feeder", Panelsdk),
	SCSLogicBus3("SCS-Logic-Bus-3", NULL),
	SCSLogicBus3Feeder("SCS-Logic-Bus-3-Feeder", Panelsdk),
	SCSLogicBus4("SCS-Logic-Bus-4", &SCSLogicBus4Feeder),
	SCSLogicBus4Feeder("SCS-Logic-Bus-4-Feeder", Panelsdk),
	SwitchPower("Switch-Power", Panelsdk),
	GaugePower("Gauge-Power", Panelsdk),
	CryoFanMotorsTank1Feeder("Cryo-Fan-Motors-Tank-1-Feeder", Panelsdk),
	CryoFanMotorsTank2Feeder("Cryo-Fan-Motors-Tank-2-Feeder", Panelsdk),
	SMQuadARCS(ph_rcs0, Panelsdk),
	SMQuadBRCS(ph_rcs1, Panelsdk),
	SMQuadCRCS(ph_rcs2, Panelsdk),
	SMQuadDRCS(ph_rcs3, Panelsdk),
	CMRCS1(ph_rcs_cm_1, Panelsdk),
	CMRCS2(ph_rcs_cm_2, Panelsdk),
	SPSPropellant(ph_sps, Panelsdk),
	SPSEngine(th_sps[0]),
	CMSMPyros("CM-SM-Pyros", Panelsdk),
	CMSMPyrosFeeder("CM-SM-Pyros-Feeder", Panelsdk),
	CMDockingRingPyros("CM-DockingRing-Pyros", Panelsdk),
	CMDockingRingPyrosFeeder("CM-DockingRing-Pyros-Feeder", Panelsdk),
	CSMLVPyros("CSM-LV-Pyros", Panelsdk),
	CSMLVPyrosFeeder("CSM-LV-Pyros-Feeder", Panelsdk),
	ApexCoverPyros("Apex-Cover-Pyros", Panelsdk),
	ApexCoverPyrosFeeder("Apex-Cover-Pyros-Feeder", Panelsdk),
	DrogueChutesDeployPyros("Drogue-Chutes-Deploy-Pyros", Panelsdk),
	DrogueChutesDeployPyrosFeeder("Drogue-Chutes-Deploy-Pyros-Feeder", Panelsdk),
	MainChutesDeployPyros("Main-Chutes-Deploy-Pyros", Panelsdk),
	MainChutesDeployPyrosFeeder("Main-Chutes-Deploy-Pyros-Feeder", Panelsdk),
	MainChutesReleasePyros("Main-Chutes-Release-Pyros", Panelsdk),
	MainChutesReleasePyrosFeeder("Main-Chutes-Release-Pyros-Feeder", Panelsdk),
	CabinFan1Feeder("Cabin-Fan-1-Feeder", Panelsdk),
	CabinFan2Feeder("Cabin-Fan-2-Feeder", Panelsdk),
	EcsGlycolPumpsSwitch(Panelsdk),
	GlycolPump1Feeder("Glycol-Pump-1-Feeder", Panelsdk),
	GlycolPump2Feeder("Glycol-Pump-2-Feeder", Panelsdk),
	SuitCompressor1Switch(Panelsdk),
	SuitCompressor2Switch(Panelsdk),
	SuitCompressor1Feeder("Suit-Compressor-1-Feeder", Panelsdk),
	SuitCompressor2Feeder("Suit-Compressor-2-Feeder", Panelsdk),
	BatteryCharger("BatteryCharger", Panelsdk),
	timedSounds(soundlib),
	iuCommandConnector(agc, this),
	sivbCommandConnector(this),
	lemECSConnector(this),
	payloadCommandConnector(this),
	CSM_RRTto_LM_RRConnector(this, &RRTsystem),
	csm_vhfto_lm_vhfconnector(this, &vhftransceiver, &vhfranging),
	CSMToLEMPowerConnector(this),
	cdi(this),
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
	CSMDCVoltMeter(19.0, 46.0),
	CSMACVoltMeter(89.0, 131.0),
	DCAmpMeter(0.0, 100.0),
	SystemTestVoltMeter(0.0, 5.0),
	EMSDvSetSwitch(Sclick),
	SideHatch(HatchOpenSound, HatchCloseSound),	// SDockingCapture
	ForwardHatch(HatchOpenSound, HatchCloseSound),
	omnia(_V(0.707108, -0.707108, 0.0)), //Orbiter Coordinate System
	omnib(_V(-0.707108, -0.707108, 0.0)), //Orbiter Coordinate System
	omnic(_V(-0.707108, 0.707108, 0.0)), //Orbiter Coordinate System
	omnid(_V(0.707108, 0.707108, 0.0)), //Orbiter Coordinate System
	vhfAntRight(_V(0.7716246, -0.63607822, 0.0)), //Orbiter Coordinate System
	vhfAntLeft(_V(-0.7716246, 0.63607822, 0.0)), //Orbiter Coordinate System
	LogicPowerSwitch(2),
	H2Tank1TempSensor("H2Tank1-Temp-Sensor", -425.0, -200.0),
	H2Tank2TempSensor("H2Tank2-Temp-Sensor", -425.0, -200.0),
	O2Tank1TempSensor("O2Tank1-Temp-Sensor", -325.0, 80.0),
	O2Tank2TempSensor("O2Tank2-Temp-Sensor", -325.0, 80.0),
	H2Tank1PressSensor("H2Tank1-Press-Sensor", 0.0, 350.0),
	H2Tank2PressSensor("H2Tank2-Press-Sensor", 0.0, 350.0),
	O2Tank1PressSensor("O2Tank1-Press-Sensor", 50.0, 1050.0),
	O2Tank2PressSensor("O2Tank2-Press-Sensor", 50.0, 1050.0),
	H2Tank1QuantitySensor("H2Tank1-Quantity-Sensor", 0.0, 1.0, CSM_H2TANK_CAPACITY),
	H2Tank2QuantitySensor("H2Tank2-Quantity-Sensor", 0.0, 1.0, CSM_H2TANK_CAPACITY),
	O2Tank1QuantitySensor("O2Tank1-Quantity-Sensor", 0.0, 1.0, CSM_O2TANK_CAPACITY),
	O2Tank2QuantitySensor("O2Tank2-Quantity-Sensor", 0.0, 1.0, CSM_O2TANK_CAPACITY),
	FCO2PressureSensor1("FuelCell1-O2-Press-Sensor", 0.0, 75.0),
	FCO2PressureSensor2("FuelCell2-O2-Press-Sensor", 0.0, 75.0),
	FCO2PressureSensor3("FuelCell3-O2-Press-Sensor", 0.0, 75.0),
	FCH2PressureSensor1("FuelCell1-H2-Press-Sensor", 0.0, 75.0),
	FCH2PressureSensor2("FuelCell2-H2-Press-Sensor", 0.0, 75.0),
	FCH2PressureSensor3("FuelCell3-H2-Press-Sensor", 0.0, 75.0),
	FCN2PressureSensor1("FuelCell1-N2-Press-Sensor", 0.0, 75.0),
	FCN2PressureSensor2("FuelCell2-N2-Press-Sensor", 0.0, 75.0),
	FCN2PressureSensor3("FuelCell3-N2-Press-Sensor", 0.0, 75.0),
	FCO2FlowSensor1("FuelCell1-O2-Flow-Sensor", 0.0, 1.6),
	FCO2FlowSensor2("FuelCell2-O2-Flow-Sensor", 0.0, 1.6),
	FCO2FlowSensor3("FuelCell3-O2-Flow-Sensor", 0.0, 1.6),
	FCH2FlowSensor1("FuelCell1-H2-Flow-Sensor", 0.0, 0.2),
	FCH2FlowSensor2("FuelCell2-H2-Flow-Sensor", 0.0, 0.2),
	FCH2FlowSensor3("FuelCell3-H2-Flow-Sensor", 0.0, 0.2),
	CabinPressSensor("Cabin-Press-Sensor", 0.0, 17.0),
	ECSPressGroups1Feeder("ECS-Press-Groups1-Feeder", Panelsdk),
	ECSPressGroups2Feeder("ECS-Press-Groups2-Feeder", Panelsdk),
	CabinTempSensor("Cabin-Temp-Sensor", 40.0, 125.0),
	ECSTempTransducerFeeder("ECS-Temp-Transducer-Feeder", Panelsdk),
	InstrumentationPowerFeeder("Instrumentation-Power-Feeder", Panelsdk),
	SuitCabinDeltaPressSensor("Suit-Cabin-Delta-Press-Sensor", -5.0, 5.0),
	CO2PartPressSensor("CO2-Part-Press-Sensor", 0.0, 30.0),
	O2SurgeTankPressSensor("O2-Surge-Tank-Press-Sensor", 50.0, 1050.0),
	SuitTempSensor("Suit-Temp-Sensor", 20.0, 95.0),
	ECSWastePotTransducerFeeder("ECS-Waste-Pot-Transducer-Feeder", Panelsdk),
	WasteH2OQtySensor("Waste-H2O-Qty-Sensor", 0.0, 1.0, 25400.0),
	PotH2OQtySensor("Pot-H2O-Qty-Sensor", 0.0, 1.0, 16330.0),
	SuitPressSensor("SuitPressSensor", 0.0, 17.0),
	SuitCompressorDeltaPSensor("Suit-Compressor-DeltaP-Sensor", 0.0, 1.0),
	GlycolPumpOutPressSensor("Glycol-Pump-Out-Press-Sensor", 0.0, 60.0),
	GlyEvapOutSteamTempSensor("Gly-Evap-Out-Steam-Temp-Sensor", 20.0, 95.0),
	GlyEvapOutTempSensor("Gly-Evap-Out-Temp-Sensor", 25.0, 75.0),
	GlycolAccumQtySensor("Glycol-Accum-Qty-Sensor", 0.0, 1.0, 10000.0),
	ECSRadOutTempSensor("ECSRadOutTempSensor", -50.0, 100.0),
	GlyEvapBackPressSensor("Gly-Evap-Back-Press-Sensor", 0.05, 0.25),
	ECSO2FlowO2SupplyManifoldSensor("ECS-O2-Flow-O2-Supply-Manifold-Sensor", 0.2, 1.0),
	O2SupplyManifPressSensor("O2-Supply-Manif-Press-Sensor", 0.0, 150.0),
	ECSSecTransducersFeeder("ECS-Sec-Transducers-Feeder", Panelsdk),
	SecGlyPumpOutPressSensor("Sec-Gly-Pump-Out-Press-Sensor", 0.0, 60.0),
	SecEvapOutLiqTempSensor("Sec-Eva-pOut-Liq-Temp-Sensor", 25.0, 75.0),
	SecGlycolAccumQtySensor("Sec-Glycol-Accum-Qty-Sensor", 0.0, 1.0, 10000.0),
	SecEvapOutSteamPressSensor("Sec-Evap-Out-Steam-Press-Sensor", 0.05, 0.25),
	//PriGlycolFlowRateSensor("Pri-Glycol-Flow-Rate-Sensor", 150.0, 300.0)
	PriEvapInletTempSensor("Pri-Evap-Inlet-Temp-Sensor", 35.0, 100.0),
	PriRadInTempSensor("Pri-Rad-In-Temp-Sensor", 55.0, 120.0),
	SecRadInTempSensor("Sec-Rad-In-Temp-Sensor", 55.0, 120.0),
	SecRadOutTempSensor("Sec-Rad-Out-Temp-Sensor", 30.0, 70.0),
	CMRCSEngine12TempSensor("CM-RCS-Engine-12-Sensor", -50.0, 50.0),
	CMRCSEngine14TempSensor("CM-RCS-Engine-14-Sensor", -50.0, 50.0),
	CMRCSEngine16TempSensor("CM-RCS-Engine-16-Sensor", -50.0, 50.0),
	CMRCSEngine21TempSensor("CM-RCS-Engine-21-Sensor", -50.0, 50.0),
	CMRCSEngine24TempSensor("CM-RCS-Engine-24-Sensor", -50.0, 50.0),
	CMRCSEngine25TempSensor("CM-RCS-Engine-25-Sensor", -50.0, 50.0),
	BatteryManifoldPressureSensor("Battery-Manifold-Pressure-Sensor", 0.0, 20.0),
	WasteH2ODumpTempSensor("Waste-H2O-Dump-Temp-Sensor", 0.0, 100.0),
	UrineDumpTempSensor("Urine-Dump-Temp-Sensor", 0.0, 100.0),
	SPSFuelLineTempSensor("SPS-Fuel-Line-Temp-Sensor", 0.0, 200.0),
	SPSOxidizerLineTempSensor("SPS-Oxidizer-Line-Temp-Sensor", 0.0, 200.0),
	SPSFuelFeedTempSensor("SPS-Fuel-Feed-Temp-Sensor", 0.0, 200.0),
	SPSOxidizerFeedTempSensor("SPS-Oxidizer-Feed-Temp-Sensor", 0.0, 200.0),
	SPSEngVlvTempSensor("SPS-Engine-Valve-Temp-Sensor", 0.0, 200.0),
	vesim(&cbCSMVesim, this),
	CueCards(vcidx, this, 11),
	Failures(this)
#pragma warning ( pop ) // disable:4355

{	
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF|_CRTDBG_CHECK_ALWAYS_DF );
	InitSaturnCalled = false;

	//Mission File
	InitMissionManagementMemory();
	pMission = paGetDefaultMission();

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

	// Clobber checklist variables
	for (int i = 0; i < 16; i++) {
		Checklist_Variable[i][0] = 0;
	}

	//
	// Register visible connectors.
	//
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &MFDToPanelConnector);
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &cdi);
	RegisterConnector(0, &CSMToLEMConnector);
	RegisterConnector(0, &lemECSConnector);
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &CSM_RRTto_LM_RRConnector);
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &csm_vhfto_lm_vhfconnector);
}

Saturn::~Saturn()

{
	TRACESETUP("~Saturn");

	ReleaseSurfaces();
	ReleaseSurfacesVC();

	if (sivb)
	{
		delete sivb;
		sivb = 0;
	}

	if (LMPad) {
		delete[] LMPad;
		LMPad = 0;
	}

	if (AEAPad) {
		delete[] AEAPad;
		AEAPad = 0;
	}

	ClearMissionManagementMemory();

	// Release DirectX joystick stuff
	if(enableVESIM || js_enabled > 0){
		// Release joysticks
		while(js_enabled > 0){
			js_enabled--;
			dx8_joystick[js_enabled]->Unacquire();
			dx8_joystick[js_enabled]->Release();
		}
		dx8ppv->Release();
		dx8ppv = NULL;
	}

	for (int i = 0; i < 2; i++) {
		delete[] ReticleLineLen[i];
		for (int k = 0; k < 2; k++)
			delete[] ReticleLine[i][k];
	}
	delete[] ReticlePoint;

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
	LESLegsCut = false;
	ApexCoverAttached = true;
	ChutesAttached = true;
	CSMAttached = true;
	SIMBayPanelJett = false;

	TLISoundsLoaded = false;
	IUSCContPermanentEnabled = true;

	//
	// Do we have the Skylab-type SM and CM?
	//
	SkylabSM = false;
	SkylabCM = false;

	CMdocktgt = false;

	//
	// Or the S1b panel with 8 engine lights?
	//
	S1bPanel = false;

	LEM_DISPLAY=false;

	AutoSlow = false;
	Crewed = true;
	SIVBPayload = PAYLOAD_LEM;

	PayloadDataTransfer = false;
	PostSplashdownPlayed = false;
	SplashdownPlayed = false;

	FireLEM = false;
	FireTJM = false;
	FirePCM = false;

	DeleteLaunchSite = true;

	buildstatus = 6;

	DockAngle = 0;

	StopRot = false;
	HasProbe = false;

	LowRes = false;

	SLARotationLimit = 45;
	SLAWillSeparate = true;

	UseWideSLA = false;

	hStage1Mesh = 0;
	hStage2Mesh = 0;
	hStage3Mesh = 0;
	hInterstageMesh = 0;
	hStageSLA1Mesh = 0;
	hStageSLA2Mesh = 0;
	hStageSLA3Mesh = 0;
	hStageSLA4Mesh = 0;
	SPSidx = 0;

	hstg1 = 0;
	hstg2 = 0;
	hintstg = 0;
	hesc1 = 0;
	hPROBE = 0;
	hs4b1 = 0;
	hs4b2 = 0;
	hs4b3 = 0;
	hs4b4 = 0;
	habort = 0;
	hSMJet = 0;
	hVAB = 0;
	hML = 0;
	hCrawler = 0;
	hMSS = 0;
	hApex = 0;
	hDrogueChute = 0;
	hMainChute = 0;
	hOpticsCover = 0;
	hLC34 = 0;
	hLC37 = 0;
	hLCC = 0;

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
	AEAPadCount = 0;
	AEAPad = 0;
	AEAPadLoadCount = 0;
	AEAPadValueCount = 0;

	//
	// Default mission time to an hour prior to launch.
	//

	MissionTime = (-3600);
	NextMissionEventTime = 0;

	//
	// No point trying to destroy things if we haven't launched.
	//

	NextDestroyCheckTime = 0;

	//
	// Panel flash.
	//

	NextFlashUpdate = MINUS_INFINITY;
	PanelFlashOn = false;

	//
	// Configure AGC and DSKY.
	//

	agc.ControlVessel(this);
	imu.SetVessel(this, false);
	dsky.Init(&LightingNumIntLMDCCB, &CMCDCBusFeeder, &NumericRotarySwitch, &IntegralRotarySwitch, NULL, NULL);
	dsky2.Init(&LightingNumIntLEBCB, &CMCDCBusFeeder, &Panel100NumericRotarySwitch, &Panel100IntegralRotarySwitch, NULL, NULL);

	//
	// Configure SECS.
	//

	secs.ControlVessel(this);
	els.ControlVessel(this);

	//
	// Wire up timers.
	//
	MissionTimerDisplay.Init(&TimersMnACircuitBraker, &TimersMnBCircuitBraker, &NumericRotarySwitch, &LightingNumIntLMDCCB, NULL);
	MissionTimer306Display.Init(&TimersMnACircuitBraker, &TimersMnBCircuitBraker, &Panel100NumericRotarySwitch, &LightingNumIntLEBCB, NULL);
	EventTimerDisplay.Init(&TimersMnACircuitBraker, &TimersMnBCircuitBraker, &NumericRotarySwitch, &LightingNumIntLEBCB, NULL);
	EventTimer306Display.Init(&TimersMnACircuitBraker, &TimersMnBCircuitBraker, &Panel100NumericRotarySwitch, &LightingNumIntLEBCB, NULL);

	//
	// Configure connectors.
	//

	iuCommandConnector.SetSaturn(this);
	sivbCommandConnector.SetSaturn(this);
	CSM_RRTto_LM_RRConnector.SetSaturn(this);
	csm_vhfto_lm_vhfconnector.SetSaturn(this);


	CSMToLEMConnector.SetType(CSM_LEM_DOCKING);
	CSMToLEMPowerConnector.SetType(LEM_CSM_POWER);
	lemECSConnector.SetType(LEM_CSM_ECS);
	payloadCommandConnector.SetType(CSM_PAYLOAD_COMMAND);
	CSMToLEMPowerConnector.SetPowerDrain(&CSMToLEMPowerDrain);
	CSM_RRTto_LM_RRConnector.SetType(RADAR_RF_SIGNAL);
	csm_vhfto_lm_vhfconnector.SetType(VHF_RNG);


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
	// TJM, LEM and PCM setup.
	//

	ISP_TJM_SL = 1745.5837;
	ISP_TJM_VAC = 1765.197;
	THRUST_VAC_TJM = (135745.3 / 2.0)*ISP_TJM_VAC / ISP_TJM_SL / cos(30.0*RAD);

	ISP_LEM_SL = 1725.9704;
	ISP_LEM_VAC = 1922.1034;
	THRUST_VAC_LEM = (533786.6 / 4.0)*ISP_LEM_VAC / ISP_LEM_SL / cos(35.0*RAD);

	ISP_PCM_SL = 1931.91005;
	ISP_PCM_VAC = 1971.13665;
	THRUST_VAC_PCM = 12542.8;

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
	ph_lem = 0;
	//ph_tjm = 0;
	ph_pcm = 0;
	ph_sep = 0;
	ph_sep2 = 0;
	ph_o2_vent = 0;
	ph_ullage1 = 0;
	ph_ullage2 = 0;
	ph_ullage3 = 0;
	ph_aps1 = 0;
	ph_aps2 = 0;

	//
	// Thruster groups.
	//

	thg_1st = 0;
	thg_2nd = 0;
	thg_3rd = 0;
	thg_sps = 0;
	thg_lem = 0;
	//thg_tjm = 0;
	thg_ull = 0;
	thg_ver = 0;
	thg_retro1 = 0;
	thg_retro2 = 0;
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

	NoiseOffsetx = 0;
	NoiseOffsety = 0;
	NoiseOffsetz = 0;

	//
	// VC Free Cam
	//

	vcFreeCamx = 0;
	vcFreeCamy = 0;
	vcFreeCamz = 0;
	vcFreeCamSpeed = 0.2;
	vcFreeCamMaxOffset = 0.5;


	InVC = false;
	InPanel = false;
	CheckPanelIdInTimestep = false;
	RefreshPanelIdInTimestep = false;
	FovFixed = false;
	FovExternal = false;
	FovSave = 0;

	//
	// Flashlight
	//
	flashlight = 0;
	flashlightColor = { 1,1,1,0 };
	flashlightColor2 = { 0,0,0,0 };
	flashlightPos = { 0,0,0 };
	vesselPosGlobal = { 0,0,0 };
	flashlightDirGlobal = { 0,0,1 };
	flashlightDirLocal = { 0,0,1 };
	flashlightOn = 0;

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

	for (i = 0; i < nsurf; i++)
	{
		srf[i] = 0;
	}

	for (i = 0; i < 8; i++)
	{
		th_1st[i] = 0;
	}

	for (i = 0; i < 5; i++)
	{
		th_2nd[i] = 0;
	}

	th_3rd[0] = 0;
	th_sps[0] = 0;

	/*for (i = 0; i < 2; i++)
	{
		th_tjm[i] = 0;
	}*/

	for (i = 0; i < 4; i++)
	{
		th_lem[i] = 0;
	}

	th_pcm = 0;

	for (i = 0; i < 8; i++) {
		th_ull[i] = 0;
		th_sep[i] = 0;
		th_sep2[i] = 0;
	}

	for (i = 0; i < 3; i++) {
		th_ver[i] = 0;
	}

	for (i = 0; i < 6; i++) {
		th_aps_rot[i] = 0;
	}

	for (i = 0; i < 2; i++) {
		th_aps_ull[i] = 0;
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
	LMDescentFuelMassKg = 8375.0;
	LMAscentFuelMassKg = 2345.0;
	LMAscentEmptyMassKg = 2150.0;
	LMDescentEmptyMassKg = 2224.0;

	customPayloadMass = 0;
	customPayloadClass[0] = 0;

	UseATC = false;

	SIISepState = false;
	bRecovery = false;
	DontDeleteIU = false;

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

	viewpos = SATVIEW_LEFTSEAT;

	dockringidx = -1;
	probeidx = -1;
	probeextidx = -1;
	crewidx = -1;
	cmpidx = -1;
	sidehatchidx = -1;
	sidehatchopenidx = -1;
	sidehatchburnedidx = -1;
	sidehatchburnedopenidx = -1;
	fwdhatchidx = -1;
	opticscoveridx = -1;
	cmdocktgtidx = -1;
	simbaypanelidx = -1;
	vcidx = -1;
	seatsfoldedidx = -1;
	seatsunfoldedidx = -1;
	coascdridx = -1;
	coascdrreticleidx = -1;

	vcmesh = NULL;
	vis = NULL;

	Scorrec = false;

	//
	// VAGC Mode settings
	//

	VAGCChecklistAutoSlow = false;
	VAGCChecklistAutoEnabled = false;

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

	pMCC = NULL;

	iu = NULL;
	sivb = NULL;

	Panel181 = NULL;
    Panel277 = NULL;
	Panel278J = NULL;

	//
	// Timestep tracking for debugging.
	//

	LongestTimestep = 0;
	LongestTimestepLength = 0.0;
	CurrentTimestep = 0;

	// VC Seats status
	VCSeatsfolded = false;

	COASreticlevisible = false;

	CurrentFuelWeight = 0;
	LastFuelWeight = numeric_limits<double>::infinity(); // Ensure update at first opportunity
	currentCoG = _V(0, 0, 0);

	// call only once 
	if (!InitSaturnCalled) {

		// PanelSDK pointers.
		ClearPanelSDKPointers();

		// Initialize the internal systems
		SystemsInit();

		InitVCAnimations();
		InitReticle();

		// Initialize the panel
		fdaiDisabled = false;
		fdaiSmooth = false;

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

void Saturn::clbkPostCreation()
{
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

	//Set Animation States
	hga.clbkPostCreation();
	SPSEngine.clbkPostCreation();

	// Connect to the Checklist controller.
	checkControl.linktoVessel(this);

	//Find MCC, if it exists
	pMCC = NULL;
	hMCC = oapiGetVesselByName("MCC");
	if (hMCC != NULL) {
		VESSEL* pVessel = oapiGetVesselInterface(hMCC);
		if (pVessel) {
			if (utils::IsVessel(pVessel, utils::MCC))
			{
				MCCVessel *pMCCVessel = static_cast<MCCVessel*>(pVessel);
				if (pMCCVessel->mcc)
				{
					pMCC = pMCCVessel->mcc;
				}
			}
		}
	}


	//
	// VESSELSOUND initialisation
	// 
	soundlib.InitSoundLib(this, SOUND_DIRECTORY);
	soundlib.SetLanguage(AudioLanguage);
	LoadDefaultSounds();

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
			LaunchS.setFlags(SOUNDFLAG_1XORLESS | SOUNDFLAG_COMMS);
		}
	}

	//
	// Load the window sound if the launch escape tower is attached.
	//

	if (LESAttached)
		soundlib.LoadMissionSound(SwindowS, WINDOW_SOUND, POST_TOWER_JET_SOUND);

	if (stage < LAUNCH_STAGE_TWO) {
		soundlib.LoadMissionSound(SShutS, SI_CUTOFF_SOUND, SISHUTDOWN_SOUND);
	}

	if (stage < STAGE_ORBIT_SIVB) {

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
		Scount.setFlags(SOUNDFLAG_1XORLESS | SOUNDFLAG_COMMS);
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

	if (pMission->DoApollo13Failures()) {
		if (!KranzPlayed)
			soundlib.LoadMissionSound(SKranz, A13_KRANZ, NULL, INTERNAL_ONLY);
		if (!CryoStir)
			soundlib.LoadMissionSound(SApollo13, A13_CRYO_STIR, NULL);
		if (!ApolloExploded)
			soundlib.LoadMissionSound(SExploded, A13_PROBLEM, NULL);

		if (stage <= CSM_LEM_STAGE) {
			soundlib.LoadMissionSound(SSMSepExploded, A13_SM_SEP_SOUND, NULL);
		}

		SKranz.setFlags(SOUNDFLAG_1XORLESS | SOUNDFLAG_COMMS);
		SApollo13.setFlags(SOUNDFLAG_1XORLESS | SOUNDFLAG_COMMS);
		SExploded.setFlags(SOUNDFLAG_1XORLESS | SOUNDFLAG_COMMS);
	}

	//
	// Check Saturn devices.
	//
	CheckSaturnSystemsState();

	//
	// Initialize the IU
	//

	if (stage < CSM_LEM_STAGE)
	{
		iu->SetMissionInfo(Crewed, IUSCContPermanentEnabled);
	}

	//
	// Disable master alarm sound on unmanned flights.
	//

	cws.SetPlaySounds(Crewed);
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
	// Internal/External view check
	//

	if (!FovExternal && !oapiCameraInternal()) {
		FovExternal = true;
		if (FovFixed) oapiCameraSetAperture(FovSave);
	}

	if (FovExternal && oapiCameraInternal()) {
		FovExternal = false;
		SetView();
	}

	//
	// Subclass specific handling
	//

	Timestep(simt, simdt, mjd);

	if (oapiGetFocusObject() == GetHandle()) {
		dsky.SendNetworkPacketDSKY();
	}

	if ((oapiGetFocusObject() == GetHandle()) && (oapiCockpitMode() == COCKPIT_VIRTUAL) && (oapiCameraMode() == CAM_COCKPIT)) {
		//We have focus on this vessel, and are in the VC
		MoveFlashlight();
	}

	sprintf(buffer, "End time(0) %lld", time(0)); 
	TRACE(buffer);
}

void Saturn::clbkPostStep(double simt, double simdt, double mjd)

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

	inertialData.Timestep(simdt);

	if (stage >= PRELAUNCH_STAGE && !GenericFirstTimestep) {

		//
		// The SPS engine must be in post time step 
		// to inhibit Orbiter's thrust control
		//

		SPSEngine.Timestep(MissionTime, simdt);

		// Better acceleration measurement stability
		imu.Timestep(simdt);
		tcdu.Timestep(simdt);
		scdu.Timestep(simdt);
		ems.TimeStep(simdt);
		CrewStatus.Timestep(simdt);

		if (stage < CSM_LEM_STAGE)
		{
			iu->PostStep(simt, simdt, mjd);
		}
	}
	// Order is important, otherwise delayed springloaded switches are reset immediately
	MainPanel.timestep(MissionTime);
	checkControl.timestep(MissionTime, eventControl);

	// Update VC animations
	if (oapiCameraInternal() && oapiCockpitMode() == COCKPIT_VIRTUAL)
	{
		MainPanelVC.OnPostStep(simt, simdt, mjd);
	}

	// Do this last to override previous debug strings
	if (nasspver != NASSP_VERSION) {
		sprintf(oapiDebugString(), "The scenario you are using is too old (Scenario: %d, NASSP: %d). Please go here for more info: https://nassp.space/index.php/Scenario_File_Updates", nasspver, NASSP_VERSION);
	}

	sprintf(buffer, "End time(0) %lld", time(0)); 
	TRACE(buffer);
}

void Saturn::clbkSaveState(FILEHANDLE scn)

{
	// set CoG to center of mesh before saving scenario; otherwise, CSM position will change slightly when saved scenario is loaded
	if (stage == CSM_LEM_STAGE)
	{
		ShiftCG(-currentCoG);
	}
	// save default vessel parameters
	VESSEL4::clbkSaveState(scn);
	// reset CoG to correct position
	if (stage == CSM_LEM_STAGE)
	{
		ShiftCG(currentCoG);
	}

	int i = 1;
	char str[256];

	oapiWriteScenario_int (scn, "NASSPVER", nasspver);
	oapiWriteScenario_int (scn, "STAGE", stage);
	oapiWriteScenario_int(scn, "VECHNO", VehicleNo);

	if (ApolloNo == 0)
	{
		//New system, save mission name
		strcpy(str, pMission->GetMissionName().c_str());
		oapiWriteScenario_string(scn, "MISSION", str);
	}
	else
	{
		//Old system, remove at some point
		oapiWriteScenario_int(scn, "APOLLONO", ApolloNo);
	}

	oapiWriteScenario_int (scn, "SATTYPE", SaturnType);
	oapiWriteScenario_int (scn, "PANEL_ID", PanelId);
	oapiWriteScenario_int(scn, "VIEWPOS", viewpos);
	papiWriteScenario_double (scn, "TCP", TCPO);
	papiWriteScenario_double (scn, "MISSNTIME", MissionTime);
	papiWriteScenario_double (scn, "NMISSNTIME", NextMissionEventTime);

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
		papiWriteScenario_double (scn, "T1V", THRUST_FIRST_VAC);
		papiWriteScenario_double (scn, "I1S", ISP_FIRST_SL);
		papiWriteScenario_double (scn, "I1V", ISP_FIRST_VAC);
		oapiWriteScenario_int (scn, "SIENG", SI_EngineNum);
	}

	if (stage < STAGE_ORBIT_SIVB) {

		papiWriteScenario_double (scn, "T2V", THRUST_SECOND_VAC);
		papiWriteScenario_double (scn, "I2S", ISP_SECOND_SL);
		papiWriteScenario_double (scn, "I2V", ISP_SECOND_VAC);
		papiWriteScenario_double (scn, "T3V", THRUST_THIRD_VAC);
		papiWriteScenario_double (scn, "I3V", ISP_THIRD_VAC);
		oapiWriteScenario_int (scn, "SIIENG", SII_EngineNum);

	}

	if (stage < CSM_LEM_STAGE) {
		oapiWriteScenario_int (scn, "SIIIENG", SIII_EngineNum);
		oapiWriteScenario_int (scn, "SLASTATE", GetSLAState());
	}

	if (stage < CM_STAGE) {
		papiWriteScenario_double (scn, "SMFUELLOAD", SM_FuelMass);
		papiWriteScenario_double (scn, "SMMASS", SM_EmptyMass);
	}

	papiWriteScenario_double (scn, "CMFUELLOAD", CM_FuelMass);
	papiWriteScenario_double (scn, "CMMASS", CM_EmptyMass);

	if (!PayloadDataTransfer) {
		if (LMPadCount > 0) {
			oapiWriteScenario_int (scn, "LMPADCNT", LMPadCount);
			for (i = 0; i < LMPadCount; i++) {
				sprintf(str, "%04o %05o", LMPad[i * 2], LMPad[i * 2 + 1]);
				oapiWriteScenario_string (scn, "LMPAD", str);
			}
		}
	}

	if (!PayloadDataTransfer) {
		if (AEAPadCount > 0) {
			oapiWriteScenario_int(scn, "AEAPADCNT", AEAPadCount);
			for (i = 0; i < AEAPadCount; i++) {
				sprintf(str, "%04o %06o", AEAPad[i * 2], AEAPad[i * 2 + 1]);
				oapiWriteScenario_string(scn, "AEAPAD", str);
			}
		}
	}

	if (!Crewed) {
		oapiWriteScenario_int (scn, "UNMANNED", 1);
	}

	if (AutoSlow) {
		oapiWriteScenario_int (scn, "AUTOSLOW", 1);
	}
	if (pMission->DoApollo13Failures()) {
		oapiWriteScenario_int (scn, "A13STATE", GetA13State());
	}
	if (SIVBPayload != PAYLOAD_LEM) {
		oapiWriteScenario_int (scn, "S4PL", SIVBPayload);
	}
	oapiWriteScenario_int(scn, "WIDESLA", UseWideSLA);
	oapiWriteScenario_float(scn, "CUSTOMPAYLOADMASS", customPayloadMass);
	if (customPayloadClass[0])
		oapiWriteScenario_string(scn, "CUSTOMPAYLOADCLASS", customPayloadClass);

	oapiWriteScenario_string (scn, "LANG", AudioLanguage);
	
	if (PayloadName[0])
		oapiWriteScenario_string (scn, "PAYN", PayloadName);

	if (!PayloadDataTransfer) {
		if (LEMCheck[0]) {
			oapiWriteScenario_string(scn, "LEMCHECK", LEMCheck);
		}
		oapiWriteScenario_float (scn, "LMDSCFUEL", LMDescentFuelMassKg);
		oapiWriteScenario_float (scn, "LMASCFUEL", LMAscentFuelMassKg);
		oapiWriteScenario_float(scn, "LMDSCEMPTY", LMDescentEmptyMassKg);
		oapiWriteScenario_float(scn, "LMASCEMPTY", LMAscentEmptyMassKg);
	}
	oapiWriteScenario_int (scn, "COASENABLED", coasEnabled);
	oapiWriteScenario_int (scn, "ORDEALENABLED", ordealEnabled);
	oapiWriteScenario_int (scn, "OPTICSDSKYENABLED", opticsDskyEnabled);
	oapiWriteScenario_int (scn, "HATCHPANEL600ENABLED", hatchPanel600EnabledLeft);
	oapiWriteScenario_int (scn, "PANEL382ENABLED", panel382Enabled);
	papiWriteScenario_bool(scn, "FOVFIXED", FovFixed);
	papiWriteScenario_double(scn, "FOVSAVE", FovSave);

	for (int i = 0; i < 16; i++) {
		if (Checklist_Variable[i][0] != 0) {
			char name[16];
			sprintf(name, "CHKVAR_%d", i);
			oapiWriteScenario_string(scn, name, Checklist_Variable[i]);
		}
	}

	Failures.SaveState(scn);
	inertialData.SaveState(scn);
	dsky.SaveState(scn, DSKY_START_STRING, DSKY_END_STRING);
	dsky2.SaveState(scn, DSKY2_START_STRING, DSKY2_END_STRING);
	agc.SaveState(scn);
	imu.SaveState(scn);
	scdu.SaveState(scn, "SCDU_START", "CDU_END");
	tcdu.SaveState(scn, "TCDU_START", "CDU_END");
	cws.SaveState(scn);
	secs.SaveState(scn);
	els.SaveState(scn);

	if (LESAttached)
	{
		qball.SaveState(scn, QBALL_START_STRING, QBALL_END_STRING);
		canard.SaveState(scn, CANARD_START_STRING, CANARD_END_STRING);
	}

	if (stage < LAUNCH_STAGE_TWO)
	{
		SaveSI(scn);
	}
	if (stage < LAUNCH_STAGE_SIVB && SaturnType == SAT_SATURNV)
	{
		SaveSII(scn);
	}
	//
	// If we've seperated from the SIVb, the IU is history.
	//
	if (stage < CSM_LEM_STAGE)
	{
		sivb->SaveState(scn);
		SaveIU(scn);
		SaveLVDC(scn);
	}

	gdc.SaveState(scn);
	rjec.SaveState(scn);
	tvsa.SaveState(scn);
	ascp.SaveState(scn);
	eda.SaveState(scn);
	ems.SaveState(scn);
	ordeal.SaveState(scn);

	MissionTimerDisplay.SaveState(scn, MISSIONTIMER_2_START_STRING, MISSIONTIMER_END_STRING, false);
	MissionTimer306Display.SaveState(scn, MISSIONTIMER_306_START_STRING, MISSIONTIMER_END_STRING, false);
	EventTimerDisplay.SaveState(scn, EVENTTIMER_2_START_STRING, EVENTTIMER_END_STRING, true);
	EventTimer306Display.SaveState(scn, EVENTTIMER_306_START_STRING, EVENTTIMER_END_STRING, true);

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

	sce.SaveState(scn);

	// save the internal systems 
	oapiWriteScenario_int(scn, "SYSTEMSSTATE", systemsState);
	papiWriteScenario_double(scn, "LSYSTEMSMISSNTIME", lastSystemsMissionTime);

	CabinPressureRegulator.SaveState(scn);
	O2DemandRegulator.SaveState(scn);
	CabinPressureReliefValve1.SaveState(1, scn);
	CabinPressureReliefValve2.SaveState(2, scn);
	O2SMSupply.SaveState(scn);
	CrewStatus.SaveState(scn);
	ForwardHatch.SaveState(scn);
	SideHatch.SaveState(scn);
	H2CryoPressureSwitch.SaveState(scn, "H2PRESSSWITCHES");
	O2CryoPressureSwitch.SaveState(scn, "O2PRESSSWITCHES");
	usb.SaveState(scn);
	if (pMission->CSMHasHGA()) hga.SaveState(scn);
	vhftransceiver.SaveState(scn);
	if (pMission->CSMHasVHFRanging()) vhfranging.SaveState(scn);
	dataRecorder.SaveState(scn);
	RRTsystem.SaveState(scn);
	udl.SaveState(scn);

	CueCards.SaveState(scn);

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

	state.IUSCContPermanentEnabled = IUSCContPermanentEnabled;
	state.SIISepState = SIISepState;
	state.Scorrec = Scorrec;
	state.Burned = Burned;
	state.FireLEM = FireLEM;
	state.FireTJM = FireTJM;
	state.viewpos = viewpos;
	state.PayloadDataTransfer = PayloadDataTransfer;
	state.SplashdownPlayed = SplashdownPlayed;
	state.FirePCM = FirePCM;
	state.PostSplashdownPlayed = PostSplashdownPlayed;
	state.SkylabSM = SkylabSM;
	state.SkylabCM = SkylabCM;
	state.S1bPanel = S1bPanel;
	state.TLISoundsLoaded = TLISoundsLoaded;
	state.CMdocktgt = CMdocktgt;
	state.VCSeatsfolded = VCSeatsfolded;
	state.COASreticlevisible = COASreticlevisible;

	return state.word;
}

void Saturn::SetMainState(int s)

{
	MainState state;

	state.word = s;
	IUSCContPermanentEnabled = state.IUSCContPermanentEnabled;
	SIISepState = state.SIISepState;
	Scorrec = state.Scorrec;
	Burned = state.Burned;
	FireLEM = state.FireLEM;
	FireTJM = state.FireTJM;
	viewpos = state.viewpos;
	PayloadDataTransfer = (state.PayloadDataTransfer != 0);
	SplashdownPlayed = (state.SplashdownPlayed != 0);
	FirePCM = state.FirePCM;
	PostSplashdownPlayed = (state.PostSplashdownPlayed != 0);
	SkylabSM = (state.SkylabSM != 0);
	SkylabCM = (state.SkylabCM != 0);
	S1bPanel = (state.S1bPanel != 0);
	TLISoundsLoaded = (state.TLISoundsLoaded != 0);
	CMdocktgt = (state.CMdocktgt != 0);
	VCSeatsfolded = (state.VCSeatsfolded != 0);
	COASreticlevisible = (state.COASreticlevisible != 0);
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
	state.InterstageAttached = InterstageAttached;
	state.LESAttached = LESAttached;
	state.HasProbe = HasProbe;
	state.ApexCoverAttached = ApexCoverAttached;
	state.ChutesAttached = ChutesAttached;
	state.LESLegsCut = LESLegsCut;
	state.SIMBayPanelJett = SIMBayPanelJett;

	return state.word;
}

void Saturn::SetAttachState(int s)

{
	AttachState state;

	state.word = s;

	CSMAttached = (state.CSMAttached != 0);
	LESAttached = (state.LESAttached != 0);
	InterstageAttached = (state.InterstageAttached != 0);
	HasProbe = (state.HasProbe != 0);
	ApexCoverAttached = (state.ApexCoverAttached != 0);
	ChutesAttached = (state.ChutesAttached != 0);
	LESLegsCut = (state.LESLegsCut != 0);
	SIMBayPanelJett = (state.SIMBayPanelJett != 0);
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
	LVRateLight = (state.LVRateLight != 0);
}

bool Saturn::ProcessConfigFileLine(FILEHANDLE scn, char *line)

{
	float ftcp;
	int SwitchState = 0;
	int status = 0;
	int DummyLoad, i;
	bool found;
	char tempBuffer[256];

	found = true;

    if (!strnicmp (line, "CONFIGURATION", 13)) {
        sscanf (line+13, "%d", &status);
	}
	else if (!strnicmp (line, "NASSPVER", 8)) {
		sscanf (line + 8, "%d", &nasspver);
	}
	else if (!strnicmp (line, "BUILDSTATUS", 11)) {
		sscanf (line+11, "%d", &buildstatus);
	}
	else if (!strnicmp (line, "PANEL_ID", 8)) {
		sscanf (line+8, "%d", &PanelId);
	}
	else if (!strnicmp(line, "VIEWPOS", 7)) {
		sscanf(line + 7, "%d", &viewpos);
	}
	else if (!strnicmp (line, "STAGESTATUS", 11)) {
		sscanf (line+11, "%d", &StageState);
	}
	else if (papiReadScenario_double(line, "DOCKANGLE", DockAngle));
	else if (!strnicmp(line, "STAGECONFIG", 11)) {
		strncpy (StagesString, line + 12, 256);
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
	else if (!strnicmp(line, "AEAPADCNT", 9)) {
		if (!AEAPad) {
			sscanf(line + 9, "%d", &AEAPadCount);
			if (AEAPadCount > 0) {
				AEAPad = new unsigned int[AEAPadCount * 2];
			}
		}
	}
	else if (!strnicmp(line, "AEAPAD", 6)) {
		unsigned int addr, val;
		sscanf(line + 6, "%o %o", &addr, &val);
		AEAPadValueCount++;
		if (AEAPad && AEAPadLoadCount < (AEAPadCount * 2)) {
			AEAPad[AEAPadLoadCount++] = addr;
			AEAPad[AEAPadLoadCount++] = val;
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
		
		//APOLLONO is going to be removed!

		sscanf(line + 8, "%d", &ApolloNo);

		pMission->LoadMission(ApolloNo);
		
		//Create mission specific systems
		CreateMissionSpecificSystems();
	}
	else if (papiReadScenario_string(line,"MISSION", tempBuffer)) {

		//New missions should load a mission name!
		pMission->LoadMission(tempBuffer);

		//Create mission specific systems
		CreateMissionSpecificSystems();
		
	}
	else if (!strnicmp (line, "SATTYPE", 7)) {
		sscanf (line+7, "%d", &SaturnType);
	}
	else if (!strnicmp(line, "MISSNTIME", 9)) {
        sscanf (line+9, "%f", &ftcp);
		MissionTime = ftcp;
	}
	else if (!strnicmp(line, "NMISSNTIME", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		NextMissionEventTime = ftcp;
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
	else if (!strnicmp(line, "INTERSTAGE", 10)) {
		sscanf(line + 10, "%f", &ftcp);
		Interstage_Mass = ftcp;
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
	else if (!strnicmp(line, "WIDESLA", 7)) {
		int i;
		sscanf(line + 7, "%d", &i);
		UseWideSLA = (i != 0);
	}
	else if (!strnicmp(line, "CUSTOMPAYLOADMASS", 17)) {
		sscanf(line + 17, "%f", &ftcp);
		customPayloadMass = ftcp;
	}
	else if (!strnicmp(line, "CUSTOMPAYLOADCLASS", 18)) {
		strncpy(customPayloadClass, line + 19, 256);
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
	else if (!strnicmp(line, FAILURES_START_STRING, sizeof(FAILURES_START_STRING))) {
		Failures.LoadState(scn);
	}
	else if (!strnicmp(line, INERTIAL_DATA_START_STRING, sizeof(INERTIAL_DATA_START_STRING))) {
		inertialData.LoadState(scn);
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
	else if (!strnicmp(line, "SCDU_START", sizeof("SCDU_START"))) {
		scdu.LoadState(scn, "CDU_END");
	}
	else if (!strnicmp(line, "TCDU_START", sizeof("TCDU_START"))) {
		tcdu.LoadState(scn, "CDU_END");
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
	else if (!strnicmp(line, ASCP_START_STRING, sizeof(ASCP_START_STRING))) {
		ascp.LoadState(scn);
	}
	else if (!strnicmp(line, EDA_START_STRING, sizeof(EDA_START_STRING))) {
		eda.LoadState(scn);
	}
	else if (!strnicmp(line, QBALL_START_STRING, sizeof(QBALL_START_STRING))) {
		qball.LoadState(scn, QBALL_END_STRING);
	}
	else if (!strnicmp(line, CANARD_START_STRING, sizeof(CANARD_START_STRING))) {
		canard.LoadState(scn, CANARD_END_STRING);
	}
	else if (!strnicmp(line, SISYSTEMS_START_STRING, sizeof(SISYSTEMS_START_STRING))) {
		LoadSI(scn);
	}
	else if (!strnicmp(line, SIISYSTEMS_START_STRING, sizeof(SIISYSTEMS_START_STRING))) {
		LoadSII(scn);
	}
	else if (!strnicmp(line, SIVBSYSTEMS_START_STRING, sizeof(SIVBSYSTEMS_START_STRING))) {
		LoadSIVB(scn);
	}
	else if (!strnicmp(line, IU_START_STRING, sizeof(IU_START_STRING))) {
		LoadIU(scn);
	}
	else if (!strnicmp(line, LVDC_START_STRING, sizeof(LVDC_START_STRING))) {
		LoadLVDC(scn);
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
	else if (!strnicmp(line, "CHKVAR_", 7)) {
		for (int i = 0; i < 16; i++) {
			char name[16];
			sprintf(name, "CHKVAR_%d", i);
			if(!strnicmp(line,name,strlen(name))){
				strncpy(Checklist_Variable[i], line + (strlen(name) + 1), 32);
				break;
			}
		}
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

			CreateStageSpecificSystems();
		}
		else if (!strnicmp (line, "MAXTIMEACC", 10)) {
			sscanf (line+10, "%d", &maxTimeAcceleration);
		}
		else if (!strnicmp (line, "MULTITHREAD", 11)) {
			int value;
			sscanf (line+11, "%d", &value);
			IsMultiThread=(value>0)?true:false;
		}
		else if (!strnicmp(line, "NOMANUALTLI", 11)) {
			//
			// NOMANUALTLI isn't saved in the scenario, this is solely to allow you
			// to override the default NOMANUALTLI state in startup scenarios.
			//
			sscanf(line + 11, "%d", &i);
			IUSCContPermanentEnabled = (i != 1);
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
		else if (!strnicmp(line, SCE_START_STRING, sizeof(SCE_START_STRING))) {
			sce.LoadState(scn);
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
		else if (!strnicmp(line, "FORWARDHATCH", 12)) {
			ForwardHatch.LoadState(line);
		}
	    else if (!strnicmp (line, "SIDEHATCH", 9)) {
		    SideHatch.LoadState(line);
	    }
		else if (!strnicmp(line, "H2PRESSSWITCHES", 15)) {
			H2CryoPressureSwitch.LoadState(line, 15);
		}
		else if (!strnicmp(line, "O2PRESSSWITCHES", 15)) {
			O2CryoPressureSwitch.LoadState(line, 15);
		}
	    else if (!strnicmp (line, "UNIFIEDSBAND", 12)) {
		    usb.LoadState(line);
	    }
	    else if (!strnicmp (line, "HIGHGAINANTENNA", 15)) {
		    hga.LoadState(line);
	    }
		else if (!strnicmp(line, "VHFTRANSCEIVER", 14)) {
			vhftransceiver.LoadState(line);
		}
		else if (!strnicmp(line, "VHFRANGING", 10)) {
			vhfranging.LoadState(line);
		}
	    else if (!strnicmp (line, "DATARECORDER", 12)) {
		    dataRecorder.LoadState(line);
	    }
		else if (!strnicmp(line, "RNDZXPDRSystem", 14)) {
			RRTsystem.LoadState(line);
		}
		else if (!strnicmp(line, UDL_START_STRING, sizeof(UDL_START_STRING))) {
			udl.LoadState(scn);
		}
		else if (!strnicmp(line, CUECARDS_START_STRING, sizeof(CUECARDS_START_STRING))) {
			CueCards.LoadState(scn);
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
		else if (!strnicmp(line, "FOVFIXED", 8)) {
		int i;
		sscanf(line + 8, "%d", &i);
		FovFixed = (i == 1);
		}
		else if (!strnicmp (line, "FOVSAVE", 7)) {
			sscanf (line + 7, "%lf", &FovSave);
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
			sscanf(line + 12, "%i", &i);
			rhc_thctoggle = (i != 0);
		}
		else if (!strnicmp(line, "JOYSTICK_VESIM", 14)) {
			int tmp;
			sscanf(line + 14, "%i", &tmp);
			enableVESIM = (tmp!= 0);
		}
		else if (!strnicmp(line, "VIBRATIONVISUALIZED", 19)) {
			sscanf(line + 19, "%i", &i);
			VibrationVisualizationMultiplier = 0.01*(double)i;
		}
		else if (papiReadScenario_double(line, "LMDSCFUEL", LMDescentFuelMassKg)); 
		else if (papiReadScenario_double(line, "LMASCFUEL", LMAscentFuelMassKg));
		else if (papiReadScenario_double(line, "LMDSCEMPTY", LMDescentEmptyMassKg));
		else if (papiReadScenario_double(line, "LMASCEMPTY", LMAscentEmptyMassKg));
		else if (!strnicmp(line, ChecklistControllerStartString, strlen(ChecklistControllerStartString))) {
			checkControl.load(scn);
		} else if (!strnicmp(line, "LEMCHECK", 8)) {
			strcpy(LEMCheck, line + 9);
		} else if (!strnicmp(line, SaturnEventStartString, strlen(SaturnEventStartString))) {
			eventControl.load(scn);
		} else if (!strnicmp(line, RJEC_START_STRING, sizeof(RJEC_START_STRING))) {
			rjec.LoadState(scn);
		} else if (!strnicmp(line, TVSA_START_STRING, sizeof(TVSA_START_STRING))) {
			tvsa.LoadState(scn);
		} else if (!strnicmp(line, ORDEAL_START_STRING, sizeof(ORDEAL_START_STRING))) {
			ordeal.LoadState(scn);
		} else if (!strnicmp(line, MISSIONTIMER_2_START_STRING, sizeof(MISSIONTIMER_2_START_STRING))) {
			MissionTimerDisplay.LoadState(scn, MISSIONTIMER_END_STRING);
		} else if (!strnicmp(line, MISSIONTIMER_306_START_STRING, sizeof(MISSIONTIMER_306_START_STRING))) {
			MissionTimer306Display.LoadState(scn, MISSIONTIMER_END_STRING);
		} else if (!strnicmp(line, EVENTTIMER_2_START_STRING, sizeof(EVENTTIMER_2_START_STRING))) {
			EventTimerDisplay.LoadState(scn, EVENTTIMER_END_STRING);
		} else if (!strnicmp(line, EVENTTIMER_306_START_STRING, sizeof(EVENTTIMER_306_START_STRING))) {
			EventTimer306Display.LoadState(scn, EVENTTIMER_END_STRING);
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
	ls.AscentFuelKg = LMAscentFuelMassKg;
	ls.DescentFuelKg = LMDescentFuelMassKg;
	ls.AscentEmptyKg = LMAscentEmptyMassKg;
	ls.DescentEmptyKg = LMDescentEmptyMassKg;
	strncpy (ls.CSMName, GetName(), 63);
	ls.MissionNo = ApolloNo;
	ls.MissionTime = MissionTime;
	strncpy (ls.checklistFile, LEMCheck, 100);
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

	//
	// Recalculate stage masses.
	//

	UpdatePayloadMass();
	CalculateStageMass ();

	//
	// And pass it the mission number and realism settings.
	//

	agc.SetMissionInfo(pMission->GetCMCVersion(), PayloadName);
	imu.SetDriftRates(pMission->GetCM_IMU_Drift());
	imu.SetPIPABias(pMission->GetCM_PIPA_Bias());
	imu.SetPIPAScale(pMission->GetCM_PIPA_Scale());
	secs.SetSaturnType(SaturnType);

	//
	// Give the vehicle number to S-IVB systems
	//
	if (sivb) sivb->SetVehicleNumber(VehicleNo);

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

void Saturn::SaveLVDC(FILEHANDLE scn) {
	if (iu != NULL) { iu->SaveLVDC(scn); }
}

void Saturn::SaveIU(FILEHANDLE scn)
{
	if (iu != NULL) { iu->SaveState(scn); }
}

//
// Set the appropriate mass based on the SIVB payload. I believe these are roughly the
// correct numbers.
//

void Saturn::UpdatePayloadMass()

{
	switch (SIVBPayload) {
	case PAYLOAD_LEM:
		S4PL_Mass = LMAscentEmptyMassKg + LMDescentEmptyMassKg + LMAscentFuelMassKg + LMDescentFuelMassKg + 2.0*LM_RCS_FUEL_PER_TANK;
		break;

	case PAYLOAD_ASTP:
		S4PL_Mass = 2012;
		break;

	case PAYLOAD_LTA10R:
		S4PL_Mass = 13381;
		break;

	case PAYLOAD_LTA2R:
		S4PL_Mass = 11794;
		break;

	case PAYLOAD_LM1:
		S4PL_Mass = 14360;
		break;

	case PAYLOAD_LTAB:
		S4PL_Mass = 9026;
		break;

	case PAYLOAD_TARGET:
		S4PL_Mass = 1789;
		break;

	case PAYLOAD_DOCKING_ADAPTER:
		S4PL_Mass = 4700.0; // see http://www.ibiblio.org/mscorbit/mscforum/index.php?topic=2064.0
		break;

	case PAYLOAD_CUSTOM:
		S4PL_Mass = customPayloadMass;
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

		if (hLC34) {
			KillDist(hLC34, 50000.0);
		}

		if (hLC37) {
			KillDist(hLC37, 50000.0);
		}

		if (hLCC) {
			KillDist(hLCC, 50000.0);
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
	CheckSaturnSystemsState();

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
	// CSM/LV separation or Mode I abort
	//

	if (stage >= CSM_LEM_STAGE) {

		ClearTLISounds();

		iuCommandConnector.Disconnect();
		sivbCommandConnector.Disconnect();
	}
}

void Saturn::GenericTimestep(double simt, double simdt, double mjd)

{
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
		hLC34 = oapiGetVesselByName("LC34");
		hLC37 = oapiGetVesselByName("LC37");
		hLCC = oapiGetVesselByName("LCC");

		GenericFirstTimestep = false;
		SetView();
	}

	//
	// Update mission time.
	//

	MissionTime += simdt;

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

	// Visualizing vibration
	// The visualized vibration is a superposition of three factors:
	//  - G-load based offset of the viewpoint(practically head movement due to
	//    the G-load caused by thrust and aerodynamic forces)
	//  - Noise-like viewpoint jostling of viewpoint depending on altitude
	//    (reflected shockwaves); on dynamic pressure(aerodynamic vibration)
	//    and on thruster activity(engine vibration carried to the cockpit by
	//    the structure)
	//  - longitudinal oscillation(POGO effect) depending on the phase of the
	//    launch.
	//
    //  The simulation of noise and longitudinal oscillation is based on Mission
	//  Reports, Launch Vehicle Flight Evaluation Reports and other sources (the
	//  most useful ones were "Saturn V Launch Vehicle Flight Evaluation Report - 
	//  AS-506, Apollo 11 Mission" and the formal and informal crew reports found
	//  in Apollo Flight Journal).
	//
	//  This model of vibration is visual only and has no effect on other parts of the simulation.
	double dynpress = GetDynPressure();
	VECTOR3 vAccel;

	inertialData.getAcceleration(vAccel);
	vAccel = -vAccel;
	THRUSTER_HANDLE *tharr;
	VECTOR3 seatacc = vAccel;
	double thsum = 0.0;
	int nth=0;
	double noiseth = 0.0, noisedp = 0.0, noisefreq = 0.0, latlonratio = 1.0;
	double pogoamp = 0.0, pogofreq = 0.0;
	double proplev = GetTotalPropellantMass(), propratio = 0.0;
	if (stage < PRELAUNCH_STAGE)
	{
		//Nothing
	}
	else if (stage <= LAUNCH_STAGE_ONE) {
		double groundcoeff = max((300.0 - GetAltitude(ALTMODE_GROUND)) / 300.0, 0.0);
		if (SaturnType == SAT_SATURNV) {
			nth = 5;
			tharr = th_1st;
			propratio = proplev / 2147000.0;
			noiseth = 0.006*groundcoeff +0.003;
			noisedp = 5.0e-7;
			noisefreq = 6.0;
			latlonratio = 0.2;

			pogofreq = 5.6 - 1.4*propratio;
			if (propratio > 0.9)
				pogoamp = 2.0 - 18.0*(1.0 - propratio);
			else if (propratio > 0.5)
				pogoamp = 0.5;
			else if (propratio > 0.4)
				pogoamp = 0.5 + 10.0*(0.5 - propratio);
			else if (propratio > 0.2)
				pogoamp = 1.5 - 5.0*(0.4 - propratio);
			else if (propratio > 0.15)
				pogoamp = 0.5 + 30.0*(0.2 - propratio);
			else 
				pogoamp = 2.0;
		}
		else if (SaturnType == SAT_SATURN1B) {
			nth = 8;
			tharr = th_1st;
			propratio = proplev / 413000.0;
			noiseth = 0.002*groundcoeff + 0.001;
			noisedp = 5.0e-7;
			noisefreq = 6.0;
		}
	}
	else if (stage <= LAUNCH_STAGE_TWO_ISTG_JET) {
		nth = 5;
		tharr = th_2nd;
		propratio = proplev / 444000.0;
		noiseth = 0.0015;
		noisefreq = 30.0;
		latlonratio = 0.2;

		if (StageUnloadState > 0 && StageUnloadState <= 3)
			pogofreq = 4.2;
		else {
			pogofreq = 15.0;
			if (propratio > 0.98)
				pogoamp = 2.5*(1.0 - propratio);
			else if (propratio > 0.9)
				pogoamp = 0.05;
			else if (propratio > 0.8)
				pogoamp = 0.05+9.5*(0.9- propratio);
			else if (propratio > 0.75)
				pogoamp = 1.0 - 18.0*(0.8 - propratio);
		}
	}
	else if (stage <= STAGE_ORBIT_SIVB) {
		nth = 1;
		tharr = th_3rd;
		propratio = proplev / 108000.0;
		noiseth = 0.0075;
		noisefreq = 30.0;
	}
	else if (stage <= CSM_LEM_STAGE) {
		nth = 1;
		tharr = th_sps;
		noisedp = 12.0e-7;
		noiseth = 0.0075;
		noisefreq = 15.0;
	}
	else if (stage >= CM_STAGE) {
		noisedp = 4.0e-7;
		noisefreq = 15.0;
	}

	for (int i = 0; i < nth; i++)
		thsum += GetThrusterLevel(tharr[i]);
	
	if (stage >= LAUNCH_STAGE_ONE &&  stage <= STAGE_ORBIT_SIVB) {
		if (SaturnType == SAT_SATURNV) {
			switch (StageUnloadState) {
			case 0:
				if (stage== LAUNCH_STAGE_ONE &&  thsum > 4.9)
					StageUnloadState = 1;
				break;
			case 1:
				if (thsum < 4.8) {
					StageUnloadTime = MissionTime;
					StageUnloadState = 2;
				}
				break;
			case 2:
				pogoamp = 4.0*exp((StageUnloadTime - MissionTime)*0.2);
				if (thsum < 3.8) {
					StageUnloadTime = MissionTime;
					StageUnloadState = 3; //Get ready for a little jolt, fellas!				
				}
				break;
			case 3:
				pogoamp = 10.0*exp((StageUnloadTime - MissionTime)*1.0);
				if ((MissionTime- StageUnloadTime) > 5.0) {
					StageUnloadState = 0;
					StageUnloadTime = -1;
				}
				break;
			default:
				break;
			}
		}
		seatacc.z = vAccel.z - pogoamp * sin(6.28*pogofreq*MissionTime);
	}
	else if (stage < LAUNCH_STAGE_ONE) {
		seatacc.x = 0.0;
		seatacc.y = 0.0;
		seatacc.z = 9.81;
	}

	double noiselat = thsum*noiseth + dynpress*noisedp;
	double noiselong = noiselat*latlonratio;
	//sprintf(oapiDebugString(), "stage=%d AX=%8.4lf AY=%8.4lf AZ=%8.4lf thsum=%5.2lf dynpress=%9.1lf proplev=%9.0lf propratio=%6.2lf pogofreq=%6.1lf pogoamp=%6.2lf voffs=(%5.2lf, %5.2lf, %5.2lf)", stage, vAccel.x, vAccel.y, vAccel.z, thsum, dynpress, proplev, propratio, pogofreq, pogoamp, ViewOffsetx, ViewOffsety, ViewOffsetz);

	if (noiselat > 0.0 || (vAccel.x*vAccel.x + vAccel.y*vAccel.y + vAccel.z*vAccel.z) > 0.01) {
		JostleViewpoint(noiselat, noiselong, noisefreq, simdt, -seatacc.x / 200.0, -seatacc.y / 200.0, -seatacc.z / 300.0);
		LastVPAccelTime = MissionTime;
	}
	else if (MissionTime<LastVPAccelTime + 5.0){	
		ViewOffsetx *= 0.95;
		ViewOffsety *= 0.95;
		ViewOffsetz *= 0.95;
		SetView();
	}

	//
	// Velocity calculations
	//

	VESSELSTATUS status;
	GetStatus(status);

	SystemsTimestep(simt, simdt, mjd);

	if(stage < LAUNCH_STAGE_SIVB) {
		if (GetNavmodeState(NAVMODE_KILLROT)) {
			DeactivateNavmode(NAVMODE_KILLROT);
		}
	}

	//
	// Only the SM has linear thrusters.
	//

	if (stage > CSM_LEM_STAGE || stage < PRELAUNCH_STAGE) {
		if (GetAttitudeMode() == ATTMODE_LIN){
			SetAttitudeMode(ATTMODE_ROT);
		}
	}

	if (habort) {
		double altabort;
		int getIT;

		getIT = oapiGetAltitude(habort,&altabort);

		if (altabort < 30 && getIT > 0) {
			oapiDeleteVessel(habort,GetHandle());
			habort = NULL;
		}
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

	//Countdown
	if (stage == PRELAUNCH_STAGE && MissionTime > -10.9)
	{
		if (!UseATC && Scount.isValid()) {
			Scount.play();
			Scount.done();
		}
	}

	if (stage >= ONPAD_STAGE) {
		timedSounds.Timestep(MissionTime, simdt, AutoSlow);
	}

	//
	// Play engines sound in case of Orbiter's attitude control is disabled
	//

	EnginesSoundTimestep();
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
	//if (KEYMOD_SHIFT(kstate) || KEYMOD_ALT(kstate)) {
	//	return 0; 
	//}

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
	
	bool camSlow = false;
	VECTOR3 camDir = _V(0, 0, 0);
	bool setFreeCam = false;

	if (KEYMOD_SHIFT(kstate)) {
		camSlow = true;
	}

	if (!KEYDOWN(kstate, OAPI_KEY_GRAVE)) {
		if (KEYDOWN(kstate, OAPI_KEY_LEFT)) {
			camDir.x = -1;
			setFreeCam = true;
		}
		if (KEYDOWN(kstate, OAPI_KEY_RIGHT)) {
			camDir.x = 1;
			setFreeCam = true;
		}
		if (KEYDOWN(kstate, OAPI_KEY_UP)) {
			camDir.y = 1;
			setFreeCam = true;
		}
		if (KEYDOWN(kstate, OAPI_KEY_DOWN)) {
			camDir.y = -1;
			setFreeCam = true;
		}
		if (KEYDOWN(kstate, OAPI_KEY_INSERT)) {
			camDir.z = 1;
			setFreeCam = true;
		}
		if (KEYDOWN(kstate, OAPI_KEY_DELETE)) {
			camDir.z = -1;
			setFreeCam = true;
		}
	}
	else {
		if (KEYDOWN(kstate, OAPI_KEY_UP)) {
			camDir.z = 1;
			setFreeCam = true;
		}
		if (KEYDOWN(kstate, OAPI_KEY_DOWN)) {
			camDir.z = -1;
			setFreeCam = true;
		}
	}

	if ((!KEYMOD_CONTROL(kstate)) && (!KEYMOD_ALT(kstate))) {
		if ((oapiCockpitMode() == COCKPIT_VIRTUAL) && (oapiCameraMode() == CAM_COCKPIT)) {
			if (setFreeCam == true) {
				VCFreeCam(camDir, camSlow);
			}
			//return 1;
		}
	}

	return 0;
}

int Saturn::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	if (enableVESIM) vesim.clbkConsumeBufferedKey(key, down, kstate);

	if (KEYMOD_SHIFT(kstate)){
		// Do DSKY stuff
		DSKYPushSwitch* dskyKeyChanged = nullptr;
		switch (key) {
			case OAPI_KEY_DECIMAL:
				dskyKeyChanged = &DskySwitchClear;
				break;
			case OAPI_KEY_PRIOR:
				dskyKeyChanged = &DskySwitchReset;
				break;
			case OAPI_KEY_HOME:
				dskyKeyChanged = &DskySwitchKeyRel;
				break;
			case OAPI_KEY_NUMPADENTER:
				dskyKeyChanged = &DskySwitchEnter;
				break;
			case OAPI_KEY_DIVIDE:
				dskyKeyChanged = &DskySwitchVerb;
				break;
			case OAPI_KEY_MULTIPLY:
				dskyKeyChanged = &DskySwitchNoun;
				break;
			case OAPI_KEY_ADD:
				dskyKeyChanged = &DskySwitchPlus;
				break;
			case OAPI_KEY_SUBTRACT:
				dskyKeyChanged = &DskySwitchMinus;
				break;
			case OAPI_KEY_END:
				dskyKeyChanged = &DskySwitchProceed;
				break;
			case OAPI_KEY_NUMPAD1:
				dskyKeyChanged = &DskySwitchOne;
				break;
			case OAPI_KEY_NUMPAD2:
				dskyKeyChanged = &DskySwitchTwo;
				break;
			case OAPI_KEY_NUMPAD3:
				dskyKeyChanged = &DskySwitchThree;
				break;
			case OAPI_KEY_NUMPAD4:
				dskyKeyChanged = &DskySwitchFour;
				break;
			case OAPI_KEY_NUMPAD5:
				dskyKeyChanged = &DskySwitchFive;
				break;
			case OAPI_KEY_NUMPAD6:
				dskyKeyChanged = &DskySwitchSix;
				break;
			case OAPI_KEY_NUMPAD7:
				dskyKeyChanged = &DskySwitchSeven;
				break;
			case OAPI_KEY_NUMPAD8:
				dskyKeyChanged = &DskySwitchEight;
				break;
			case OAPI_KEY_NUMPAD9:
				dskyKeyChanged = &DskySwitchNine;
				break;
			case OAPI_KEY_NUMPAD0:
				dskyKeyChanged = &DskySwitchZero;
				break;
			case OAPI_KEY_W: // Minimum impulse controller, pitch down
				agc.SetInputChannelBit(032, MinusPitchMinImpulse, down ? 1 : 0);
				break;
			case OAPI_KEY_S: // Minimum impulse controller, pitch up
				agc.SetInputChannelBit(032, PlusPitchMinImpulse, down ? 1 : 0);
				break;
			case OAPI_KEY_A: // Minimum impulse controller, yaw left
				agc.SetInputChannelBit(032, MinusYawMinimumImpulse, down ? 1 : 0);
				break;
			case OAPI_KEY_D: // Minimum impulse controller, yaw right
				agc.SetInputChannelBit(032, PlusYawMinimumImpulse, down ? 1 : 0);
				break;
			case OAPI_KEY_Q: // Minimum impulse controller, roll left
				agc.SetInputChannelBit(032, MinusRollMinimumImpulse, down ? 1 : 0);
				break;
			case OAPI_KEY_E: // Minimum impulse controller, roll right
				agc.SetInputChannelBit(032, PlusRollMinimumImpulse, down ? 1 : 0);
				break;
		}

		// Direction-specific code, handle DSKY key presses if any.
		if (down) {
			// KEY DOWN
			if (dskyKeyChanged != nullptr) {
				dskyKeyChanged->SetHeld(true);
				dskyKeyChanged->SetState(PUSHBUTTON_PUSHED);
			}

			switch (key) {
			case OAPI_KEY_K:
				//kill rotation
				SetAngularVel(_V(0, 0, 0));
				break;
			}
		} else {
			// KEY UP
			if (dskyKeyChanged != nullptr) {
				// Doing SwitchTo instead of SetState prevents a second click on key up.
				dskyKeyChanged->SetHeld(false);
				dskyKeyChanged->SwitchTo(PUSHBUTTON_UNPUSHED);
			}
		}
		return 0;
	}
	if (KEYMOD_CONTROL(kstate)) {
		switch (key) {
			case OAPI_KEY_D:
				// Orbiter undocking messes with our undocking system. We consume the keybind here to block it.
				// This won't work if the user has changed this keybind. Unfortunately Orbiter does not export the keymap through the API (yet). :(
				return 1;
		}
		return 0;
	}
	if (KEYMOD_ALT(kstate))
	{
		if (down) {
			switch (key) {
			case OAPI_KEY_R:
				if (stage == CM_ENTRY_STAGE_SEVEN && SideHatch.IsOpen()) {
					bRecovery = true;
				}
				return 1;
			}
		}
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
			case OAPI_KEY_MINUS:
				MoveTHC(true);
				return 1;
			case OAPI_KEY_EQUALS:
				MoveTHC(false);
				return 1;
			case OAPI_KEY_MULTIPLY:
				dVThrust1Switch.SetState(0);
				dVThrust1Switch.Guard();
				dVThrust2Switch.SetState(0);
				dVThrust2Switch.Guard();
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

	if ((down) && (key == OAPI_KEY_F)) {
		ToggleFlashlight();
	}

	// MCC CAPCOM interface key handling                                                                                                
	if (down && !KEYMOD_SHIFT(kstate)) {
		switch (key) {
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
			if (pMCC != NULL)
				pMCC->keyDown(key);
			break;
		}
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

	th_att_lin[0]=th_att_rot[0]=CreateThruster (_V(-CENTEROFFS,ATTCOOR2,TRANZ+RCSOFFSET2), _V(0.021914, -0.172260, 0.984808), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL); //A4
	th_att_lin[1]=th_att_rot[3]=CreateThruster (_V(CENTEROFFS,-ATTCOOR2,TRANZ+RCSOFFSET2), _V(-0.021914, 0.172260, 0.984808), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL); //C3
	th_att_lin[2]=th_att_rot[4]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS,TRANZ+RCSOFFSET2), _V(0.172260, 0.021914, 0.984808), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL); //D3
	th_att_lin[3]=th_att_rot[7]=CreateThruster (_V(ATTCOOR2,CENTEROFFS,TRANZ+RCSOFFSET2), _V(-0.172260, -0.021914, 0.984808), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL); //B4
	th_att_lin[4]=th_att_rot[2]=CreateThruster (_V(-CENTEROFFS,ATTCOOR2,TRANZ+RCSOFFSET), _V(0.021914, -0.172260, -0.984808), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL); //A3
	th_att_lin[5]=th_att_rot[1]=CreateThruster (_V(CENTEROFFS,-ATTCOOR2,TRANZ+RCSOFFSET), _V(-0.021914, 0.172260, -0.984808), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL); //C4
	th_att_lin[6]=th_att_rot[6]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS,TRANZ+RCSOFFSET), _V(0.172260, 0.021914, -0.984808), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL); //D4
	th_att_lin[7]=th_att_rot[5]=CreateThruster (_V(ATTCOOR2,CENTEROFFS,TRANZ+RCSOFFSET), _V(-0.172260, -0.021914, -0.984808), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL); //B3

	th_att_lin[8]=th_att_rot[16]=th_att_rot[17]=CreateThruster (_V(-CENTEROFFS - 0.2,ATTCOOR2,TRANZ+RCSOFFSETM), _V(0.998848, -0.047978, 0.0), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL); //A1
	th_att_lin[9]=th_att_rot[8]=th_att_rot[9]=CreateThruster (_V(CENTEROFFS -0.2,-ATTCOOR3,TRANZ+RCSOFFSETM2), _V(0.955020, 0.296542, 0.0), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL); //C2

	th_att_lin[12]=th_att_rot[10]=th_att_rot[11]=CreateThruster (_V(-CENTEROFFS + 0.2,ATTCOOR3,TRANZ+RCSOFFSETM2), _V(-0.955020, -0.296542, 0.0), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL); //A2
	th_att_lin[13]=th_att_rot[18]=th_att_rot[19]=CreateThruster (_V(CENTEROFFS + 0.2,-ATTCOOR2,TRANZ+RCSOFFSETM), _V(-0.998848, 0.047978, 0.0), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL); //C1

	th_att_lin[16]=th_att_rot[14]=th_att_rot[15]=CreateThruster (_V(ATTCOOR3,CENTEROFFS -0.2,TRANZ+RCSOFFSETM2), _V(-0.296542, 0.955020, 0.0), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL); //B2
	th_att_lin[17]=th_att_rot[22]=th_att_rot[23]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS -0.2,TRANZ+RCSOFFSETM), _V(0.047978, 0.998848, 0.0), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL); //D1

	th_att_lin[20]=th_att_rot[20]=th_att_rot[21]=CreateThruster (_V(ATTCOOR2,CENTEROFFS + 0.2,TRANZ+RCSOFFSETM), _V(-0.047978, -0.998848, 0.0), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL); //B1
	th_att_lin[21]=th_att_rot[12]=th_att_rot[13]=CreateThruster (_V(-ATTCOOR3,-CENTEROFFS + 0.2,TRANZ+RCSOFFSETM2), _V(0.296542, -0.955020, 0.0), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL); //D2

	for (i = 0; i < 24; i++) {
		if (th_att_lin[i])
			AddExhaust (th_att_lin[i], 3.0, 0.15, SMExhaustTex);
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

	double APSMass;
	if (SaturnType == SAT_SATURN1B)
	{
		APSMass = S4B_APS_FUEL_PER_TANK_SIB;
	}
	else
	{
		APSMass = S4B_APS_FUEL_PER_TANK_SV;
	}

	if (!ph_aps1)
	{
		ph_aps1 = CreatePropellantResource(APSMass);
	}

	if (!ph_aps2)
	{
		ph_aps2 = CreatePropellantResource(APSMass);
	}

	th_aps_rot[0] = CreateThruster(_V(0, ATTCOOR2 + 0.15, TRANZ - 0.25), _V(0, -1, 0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[1] = CreateThruster(_V(0, -ATTCOOR2 - 0.15, TRANZ - 0.25), _V(0, 1, 0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);
	
	AddExhaust (th_aps_rot[0], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[1], 0.6, 0.078, SIVBRCSTex);

	th_aps_rot[2] = CreateThruster (_V(RCSX,ATTCOOR2-0.2,TRANZ-0.2), _V(-1,0,0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[3] = CreateThruster (_V(-RCSX,-ATTCOOR2+0.2,TRANZ-0.25), _V( 1,0,0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[4] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25), _V( 1,0,0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[5] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25), _V(-1,0,0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);

	AddExhaust (th_aps_rot[2], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[3], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[4], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[5], 0.6, 0.078, SIVBRCSTex);

	//
	// APS thrusters are only 310N (72 pounds) thrust
	//

	if (SaturnType == SAT_SATURNV)
	{
		th_aps_ull[0] = CreateThruster(_V(0, ATTCOOR2 - 0.15, TRANZ - .25), _V(0, 0, 1), S4B_APS_ULL_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
		th_aps_ull[1] = CreateThruster(_V(0, -ATTCOOR2 + .15, TRANZ - .25), _V(0, 0, 1), S4B_APS_ULL_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);
		AddExhaust(th_aps_ull[0], 7, 0.15, SIVBRCSTex);
		AddExhaust(th_aps_ull[1], 7, 0.15, SIVBRCSTex);
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

void Saturn::GenericTimestepStage(double simt, double simdt)

{
	//
	// Do stage-specific processing.
	//

	switch (stage) {
	case CSM_LEM_STAGE:
		StageSix(simt);
		break;

	case CM_STAGE:
		if (ApexCoverPyros.Blown() && !HasProbe) {
			ShiftCG(_V(0, 0, 1.2));
			StageEight(simt);

		} else {
			// DS20070622 Do not run stage seven if we still have the LET.
			if (!LESAttached) { 
				StageSeven(simt); 
			}
		}
		break;

	case CM_ENTRY_STAGE:
		if (ApexCoverPyros.Blown() && !HasProbe) {
			ShiftCG(_V(0, 0, 1.2));
			StageEight(simt);
		}
		// sprintf(oapiDebugString(), "AtmPressure %f (37680)", GetAtmPressure());
		break;

	case CM_ENTRY_STAGE_TWO:
		if (DrogueChutesDeployPyros.Blown()) {
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
		if (MainChutesDeployPyros.Blown() && ChutesAttached) {
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
		if (GetAltitude(ALTMODE_GROUND) < 2.5) {
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
		if (!SplashdownPlayed && GetAltitude(ALTMODE_GROUND) < 2.5) {
			SplashS.play(NOLOOP, 180.0 / 255.0);
			SplashS.done();

			SplashdownPlayed = true;
			NextMissionEventTime = MissionTime + 25.;

			//
			// Event handler
			//
			eventControl.SPLASHDOWN = MissionTime;
		}
		if (MainChutesReleasePyros.Blown() && ChutesAttached) {
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

	if (stage < CSM_LEM_STAGE)
	{
		if (CSMAttached)
		{
			iu->ConnectToCSM(&iuCommandConnector);
		}
		iu->ConnectToLV(&sivbCommandConnector);
	}

	CSMToLEMConnector.AddTo(&CSMToLEMPowerConnector);
	CSMToLEMConnector.AddTo(&payloadCommandConnector);

	//
	// Check SM devices.
	//

	CheckSMSystemsState();

	//
	// Set up joysticks.
	//

	HRESULT hr;
	// Having read the configuration file, set up DirectX...	
	hr = DirectInput8Create(dllhandle, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dx8ppv, NULL); // Give us a DirectInput context
	if (!FAILED(hr)) {
		if (enableVESIM) {
			for (int i=0; i<CSM_AXIS_INPUT_CNT; i++)
				vesim.addInput(&vesim_csm_inputs[i]);
			vesim.setupDevices("CSM", dx8ppv);		
		}
		else {
			int x = 0;
			// Enumerate attached joysticks until we find 2 or run out.
			dx8ppv->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
			if (js_enabled == 0) {   // Did we get anything?			
				dx8ppv->Release(); // No. Close down DirectInput
				dx8ppv = NULL;     // otherwise it won't get closed later
				//sprintf(oapiDebugString(), "DX8JS: No joysticks found");
			}
			else {
				while (x < js_enabled) {                                // For each joystick
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
		}
	}
	else {
		// We can't print an error message this early in initialization, so save this reason for later investigation.
		dx8_failure = hr;
	}
}

void Saturn::SetGenericStageState()

{
	switch(stage) {
	case CSM_LEM_STAGE:
		SetCSMStage(_V(0, 0, 0));
		break;

	case CM_STAGE:
	case CM_ENTRY_STAGE_TWO:
		SetReentryStage(_V(0,0,0));
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
		SetReentryStage(_V(0, 0, 0));
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

	thg_1st = 0;
	thg_2nd = 0;
	thg_3rd = 0;
	thg_sps = 0;
	thg_lem = 0;
	//thg_tjm = 0;
	thg_ull = 0;
	thg_ver = 0;
	thg_retro1 = 0;
	thg_retro2 = 0;
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
	ph_lem = 0;
	//ph_tjm = 0;
	ph_pcm = 0;

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

	ph_aps1 = 0;
	ph_aps2 = 0;
}

//
// Do we have a CSM on this launcher?
//

bool Saturn::SaturnHasCSM()

{
	return CSMAttached;
}

void Saturn::FireLaunchEscapeMotor()
{
	if (thg_lem)
	{
		if (GetThrusterGroupLevel(thg_lem) < 1.0)
		{
			SetThrusterGroupLevel(thg_lem, 1.0);
		}
	}

	FireLEM = true;
}

void Saturn::FireTowerJettisonMotor()
{
	/*if (thg_tjm)
	{
		if (GetThrusterGroupLevel(thg_tjm) < 1.0)
		{
			SetThrusterGroupLevel(thg_tjm, 1.0);
		}
	}*/

	FireTJM = true;
}

void Saturn::FirePitchControlMotor()
{
	if (th_pcm)
	{
		if (GetThrusterLevel(th_pcm) < 1.0)
		{
			SetThrusterLevel(th_pcm, 1.0);
		}
	}

	FirePCM = true;
}

void Saturn::MoveTHC(bool dir)
{
	if (dir)
	{
		if (THCRotary.IsCounterClockwise())
		{
			//Do Nothing
		}
		else if (THCRotary.IsClockwise())
		{
			THCRotary.SwitchTo(1);
		}
		else
		{
			THCRotary.SwitchTo(0);
		}
	}
	else
	{
		if (THCRotary.IsCounterClockwise())
		{
			THCRotary.SwitchTo(1);
		}
		else if (THCRotary.IsClockwise())
		{
			//Do Nothing
		}
		else
		{
			THCRotary.SwitchTo(2);
		}
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
	soundlib.LoadSound(EngineS, MAIN_ENGINES_SOUND, INTERNAL_ONLY);

	Sctdw.setFlags(SOUNDFLAG_1XONLY|SOUNDFLAG_COMMS);
}

void Saturn::StageSix(double simt){
	UpdateMassAndCoG();

	if (pMission->DoApollo13Failures()) {

		//
		// Play cryo-stir audio.
		//

		if (!CryoStir && MissionTime >= (APOLLO_13_EXPLOSION_TIME - 120))
		{
			double TimeW = oapiGetTimeAcceleration ();
			if (TimeW > 1){
				oapiSetTimeAcceleration (1);
			}

			SApollo13.play(NOLOOP);
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

			SExploded.play(NOLOOP);
			SExploded.done();

			//MasterAlarm();  Main B Undervolt due to power transient at the explosion should trigger this alarm

			//
			// AGC restarted & lit PGNS light as the explosion occured.  Should be triggered by power transient and not agc.ForceRestart
			//

			//agc.ForceRestart();

			ApolloExploded = true;

			h_Pipe *o2Rupture1 = (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1RUPTURE");
			o2Rupture1->in->size = (float) (50.0 / LBH);	// Set O2 tank 1 leak size
			o2Rupture1->flowMax = 100.0 / LBH;  //Set O2 tank 1 leak rate

			h_Pipe *o2Rupture3 = (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2RUPTURE");
			o2Rupture3->in->size = (float) (900.0 / LBH);	// Set O2 tank 2 leak size
			o2Rupture3->flowMax = 54000.0 / LBH;  //Set O2 tank 2 leak rate

			h_Valve *leakValve1 = (h_Valve *)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:LEAK");
			leakValve1->Open();  //Start O2 tank 1 leak

			h_Valve *leakValve2 = (h_Valve *)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:LEAK");
			leakValve2->Open();  //Start O2 tank 2 leak

			h_Valve *o2react1 = (h_Valve *)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD:IN");
			o2react1->Close();  //Close FC1 O2 reactant valve

			h_Valve *o2react3 = (h_Valve *)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD:IN");
			o2react3->Close();  //Close FC3 O2 reactant valve

			SMQuadBRCS.GetHeliumValve1()->SetState(false);
			SMQuadDRCS.GetHeliumValve1()->SetState(false);

			SMQuadBRCS.GetHeliumValve2()->SetState(false);

			SMQuadARCS.GetSecPropellantValve()->SetState(false);
			SMQuadCRCS.GetSecPropellantValve()->SetState(false);
				

			//
			// Update the mesh.
			//

			SetCSMStage(_V(0, 0, 0));

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


		}

		//
		// Play Kranz comments in the background.
		//

		if (!KranzPlayed && (MissionTime >= APOLLO_13_EXPLOSION_TIME + 30)) {

			if (SExploded.isValid()) {
				SExploded.stop();
				SExploded.done();
			}

			SKranz.play(NOLOOP, 150.0 / 255.0);
			SKranz.done();

			KranzPlayed = true;
		}

		if (ApolloExploded && ph_o2_vent) {
			
			double O2Tank1Mass = O2Tanks[0]->mass/1E3;

			SetThrusterLevel(th_o2_vent, O2Tank1Mass/145.1495584);

			SetPropellantMass(ph_o2_vent, O2Tank1Mass);
		}
	}
}

void Saturn::SlowIfDesired()

{
	if (AutoSlow && (oapiGetTimeAcceleration() > 1.0)) {
		oapiSetTimeAcceleration(1.0);
	}
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

bool Saturn::GetTLIInhibitSignal()

{
	return TLIEnableSwitch.GetState() == TOGGLESWITCH_DOWN;
}

bool Saturn::GetIUUPTLMAccept()

{
	return IUUplinkSwitch.GetState() == TOGGLESWITCH_UP;
}

bool Saturn::GetSIISIVbDirectStagingSignal()

{
	return SIISIVBSepSwitch.GetState() == TOGGLESWITCH_UP;
}

int Saturn::GetLVRateAutoSwitchState()

{
	return LVRateAutoSwitch.GetState();
}

int Saturn::GetTwoEngineOutAutoSwitchState()

{
	return TwoEngineOutAutoSwitch.GetState();
}

bool Saturn::GetBECOSignal(bool IsSysA)
{
	if (IsSysA) return secs.MESCA.BECO();

	return secs.MESCB.BECO();
}

bool Saturn::IsEDSBusPowered(int eds)
{
	if (EDSPowerSwitch.IsUp())
	{
		if (eds == 1)
		{
			return EDS1BatACircuitBraker.IsPowered();
		}
		else if (eds == 2)
		{
			return EDS2BatCCircuitBraker.IsPowered();
		}
		else if (eds == 3)
		{
			return EDS3BatBCircuitBraker.IsPowered();
		}
	}

	return false;
}

int Saturn::GetAGCAttitudeError(int axis)
{
	if (axis == 0)
	{
		return gdc.fdai_err_x;
	}
	else if (axis == 1)
	{
		return gdc.fdai_err_y;
	}
	else if (axis == 2)
	{
		return gdc.fdai_err_z;
	}

	return 0;
}

void Saturn::GetSIIThrustOK(bool *ok)
{
	for (int i = 0;i < 5;i++)
	{
		ok[i] = false;
	}
}

bool Saturn::GetSIVBThrustOK()
{
	if (stage != LAUNCH_STAGE_SIVB && stage != STAGE_ORBIT_SIVB) return false;

	return sivb->GetThrustOK();
}

double Saturn::GetFirstStageThrust()
{
	if (stage > PRELAUNCH_STAGE) return 0.0;

	return THRUST_FIRST_VAC;
}

double Saturn::GetSIVBFuelTankPressurePSI()
{
	if (sivb && stage < CSM_LEM_STAGE)
		return sivb->GetLH2TankUllagePressurePSI();

	return 0.0;
}

double Saturn::GetSIVBLOXTankPressurePSI()
{
	if (sivb && stage < CSM_LEM_STAGE)
		return sivb->GetLOXTankUllagePressurePSI();

	return 0.0;
}

bool Saturn::GetSIIPropellantDepletionEngineCutoff()
{
	return false;
}

bool Saturn::GetSIBLowLevelSensorsDry()
{
	return false;
}

void Saturn::SetSIVBThrusterDir(double yaw, double pitch)
{
	if (stage != LAUNCH_STAGE_SIVB && stage != STAGE_ORBIT_SIVB) return;

	sivb->SetThrusterDir(yaw, pitch);
}

void Saturn::SIVBEDSCutoff(bool cut)
{
	if (stage != LAUNCH_STAGE_SIVB && stage != STAGE_ORBIT_SIVB) return;

	sivb->EDSEngineCutoff(cut);
}

bool Saturn::GetQBallPower()
{
	return iuCommandConnector.GetQBallPower();
}

bool Saturn::GetQBallSimulateCmd()
{
	return iuCommandConnector.GetQBallSimulateCmd();
}

void Saturn::SetAPSAttitudeEngine(int n, bool on)
{
	if (n < 0 || n > 5) return;
	if (stage != LAUNCH_STAGE_SIVB && stage != STAGE_ORBIT_SIVB) return;

	sivb->SetAPSAttitudeEngine(n, on);
}

bool Saturn::GetCMCSIVBTakeover()
{
	if (LVGuidanceSwitch.GetState() == THREEPOSSWITCH_DOWN && agc.GetInputChannelBit(012, EnableSIVBTakeover))
		return true;

	return false;
}

bool Saturn::GetCMCSIVBIgnitionSequenceStart()
{
	if (LVGuidanceSwitch.GetState() == THREEPOSSWITCH_DOWN && agc.GetInputChannelBit(012, SIVBIgnitionSequenceStart))
		return true;

	return false;
}

bool Saturn::GetCMCSIVBCutoff()
{
	if (LVGuidanceSwitch.GetState() == THREEPOSSWITCH_DOWN && agc.GetInputChannelBit(012, SIVBCutoff))
		return true;

	return false;
}

void Saturn::ConnectTunnelToCabinVent()
{
	h_Pipe *pipe = (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CSMTUNNELUNDOCKED");
	h_Vent *vent = (h_Vent *)Panelsdk.GetPointerByString("HYDRAULIC:CABINVENT");

	pipe->out = &vent->IN_valve;
}

h_Pipe* Saturn::GetCSMO2Hose()
{
	return (h_Pipe*)Panelsdk.GetPointerByString("HYDRAULIC:CSMTOLMO2HOSE");
}

void Saturn::ConnectCSMO2Hose()
{
	if (ForwardHatch.IsOpen())
	{
		lemECSConnector.ConnectCSMO2Hose();
	}
}

bool Saturn::GetLMDesBatLVOn()
{
	return LMPowerSwitch.IsDown() && MnbLMPWR2CircuitBraker.IsPowered();
}

bool Saturn::GetLMDesBatLVHVOffA()
{
	return LMPowerSwitch.IsUp() && MnbLMPWR1CircuitBraker.IsPowered();
}

bool Saturn::GetLMDesBatLVHVOffB()
{
	return LMPowerSwitch.IsUp() && MnbLMPWR2CircuitBraker.IsPowered();
}

void Saturn::SetContrailLevel(double level)
{
	contrailLevel = level;
}

void Saturn::SIVBSwitchSelector(int channel)
{
	sivb->SwitchSelector(channel);
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

void Saturn::VHFRangingReturnSignal() //DELETE ME WHEN YOU ADD THE CONNECTOR
{
	if (pMission->CSMHasVHFRanging()) vhfranging.RangingReturnSignal();
}

void Saturn::StartSeparationPyros()
{
	payloadCommandConnector.StartSeparationPyros();
}

void Saturn::UpdateMassAndCoG()
{
	CurrentFuelWeight = 0;
	if (ph_sps != NULL) { CurrentFuelWeight += GetPropellantMass(ph_sps); }
	if ((LastFuelWeight - CurrentFuelWeight) > 100.0)
	{
		// Update physical parameters
		VECTOR3 pmi, CoG;
		CalculatePMIandCOG(pmi, CoG);
		// Use SetPMI, ShiftCG, etc.
		VECTOR3 CoGShift = CoG - currentCoG;
		ShiftCG(CoGShift);
		SetPMI(pmi);
		currentCoG = CoG;

		//Touchdown Points
		ConfigTouchdownPoints();

		//Particle streams
		SetWaterDumpParticleStreams(currentCoG + _V(0, 0, 32.3));
		
		//lights
		SpotLight->UpdatePosition(CoGShift);
		RndzLight->UpdatePosition(CoGShift);

		// All done!
		LastFuelWeight = CurrentFuelWeight;

		//char Buffer[128];
		//sprintf(Buffer, "New CG: %lf %lf %lf", currentCoG.x, currentCoG.y, currentCoG.z);
		//oapiWriteLog(Buffer);
		//sprintf(Buffer, "New PMI: %lf %lf %lf", pmi.x, pmi.y, pmi.z);
		//oapiWriteLog(Buffer);
	}
}

void Saturn::CalculatePMIandCOG(VECTOR3 &PMI, VECTOR3 &COG)
{
	//Empty SM including SM RCS prop and SPS residuals (and SLA ring?)
	static const double MSM = SM_EmptyMass / 0.453597;// 10675.0;
	const VECTOR3 CGSM = pMission->GetCGOfEmptySM();
	//CM including CM RCS
	const double MCM = (CM_EmptyMass + 2.0*55.5) / 0.453597;
	static const VECTOR3 CGCM = _V(1041.7, -0.4, 5.6); // Apollo 9 - Contingency Deorbit Abort Plan, End of Mission
	//Sump tank capacity
	static const double MSUMP = 23068.1; //22300.0;
	//Full SM RCS mass
	static const double MRCINIT = 1344.82;
	//CG of SM RCS
	static const VECTOR3 CGRCS = _V(941.8, 0, 0);

	double propmass = CurrentFuelWeight / 0.453597;

	double mass, sumpm, storem, oxstorem, fuelstorem, oxsumpm, fuelsumpm;
	
	mass = propmass + MRCINIT + MCM + MSM;

	if (propmass > MSUMP)
	{
		sumpm = MSUMP;
		storem = propmass - MSUMP;
	}
	else
	{
		sumpm = propmass;
		storem = 0;
	}

	oxstorem = storem * 1.6 / (1.0 + 1.6);
	fuelstorem = storem * 1.0 / (1.0 + 1.6);
	oxsumpm = sumpm * 1.6 / (1.0 + 1.6);
	fuelsumpm = sumpm * 1.0 / (1.0 + 1.6);

	static const double oxid_store_tank_param[3] = { -8.385141e-9, 0.0061750118, 838.7809363 };
	static const double fuel_store_tank_param[3] = { -2.144163e-8, 0.0098738581, 838.7809363 };
	static const double oxid_sump_tank_param[3] = { -2.599892e-9, 0.0047770151, 839.7803146 };
	static const double fuel_sump_tank_param[3] = { -6.63916e-9, 0.0076383695, 839.7803146 };

	double cgx;

	cgx = oxid_store_tank_param[0] * pow(oxstorem, 2) + oxid_store_tank_param[1] * oxstorem + oxid_store_tank_param[2];
	VECTOR3 ox_store_CG = _V(cgx, 14.8, 47.8);

	cgx = fuel_store_tank_param[0] * pow(oxstorem, 2) + fuel_store_tank_param[1] * fuelstorem + fuel_store_tank_param[2];
	VECTOR3 fuel_store_CG = _V(cgx, -14.8, -47.8);

	cgx = oxid_sump_tank_param[0] * pow(oxsumpm, 2) + oxid_sump_tank_param[1] * oxsumpm + oxid_sump_tank_param[2];
	VECTOR3 ox_sump_CG = _V(cgx, 48.3, 6.6);

	cgx = fuel_sump_tank_param[0] * pow(fuelsumpm, 2) + fuel_sump_tank_param[1] * fuelsumpm + fuel_sump_tank_param[2];
	VECTOR3 fuel_sump_CG = _V(cgx, -48.3, -6.6);

	COG = (CGCM*MCM + CGSM * MSM + ox_store_CG * oxstorem + ox_sump_CG * oxsumpm + fuel_store_CG * fuelstorem + fuel_sump_CG * fuelsumpm + CGRCS*MRCINIT) / mass;
	//sprintf(oapiDebugString(), "%lf %lf %lf %lf", propmass, COG.x, COG.y, COG.z);
	COG = COG * 0.0254;

	//Convert to Orbiter
	COG = _V(COG.y, -COG.z, COG.x - 24.538);

	//PMI
	//Empirical data from CSM-109, Spacecraft Operational Data Book Volume III
	PMI.x = -4.372105e-5*propmass + 5.318744779;
	PMI.y = -4.190526e-5*propmass + 5.371149095;
	PMI.z = -1.626316e-6*propmass + 1.661697874;
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
	agc.RaiseInterrupt(ApolloGuidance::Interrupt::UPRUPT);
}

int Saturn::Lua_GetAGCUplinkStatus() {
	int st = 0;
	if (agc.InterruptPending(ApolloGuidance::Interrupt::UPRUPT)) {
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
