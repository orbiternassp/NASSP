/****************************************************************************
  This file is part of Project Apollo - NASSP

  Mission Control Center / Ground Support Network

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
#include <string.h>
#include <math.h>
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
#include "mcc.h"

// This is a threadenwerfer. It werfs threaden.
static DWORD WINAPI MCC_Trampoline(LPVOID ptr){	
	MCC *mcc = (MCC *)ptr;
	return(mcc->subThread());
}

// MCC CLASS

// CONS
MCC::MCC(){
	// Reset data
	cm = NULL;
	Earth = NULL;
	Moon = NULL;
	CM_DeepSpace = false;
	GT_Enabled = false;
	MT_Enabled = false;
	AbortMode = 0;
	LastAOSUpdate=0;
	// Reset ground stations
	int x=0;
	while(x<MAX_GROUND_STATION){
		GroundStations[x].Active = false;
		GroundStations[x].Position[0] = 0;
		GroundStations[x].Position[1] = 0;
		GroundStations[x].DownTlmCaps = 0;
		GroundStations[x].CommCaps = 0;
		GroundStations[x].HasAcqAid = false;
		GroundStations[x].HasRadar = false;
		GroundStations[x].SBandAntenna = 0;
		GroundStations[x].StationType = 0;
		GroundStations[x].TelemetryCaps = 0;
		GroundStations[x].TrackingCaps = 0;
		GroundStations[x].UpTlmCaps = 0;
		GroundStations[x].USBCaps = 0;
		GroundStations[x].StationPurpose = 0;
		GroundStations[x].AOS = 0;
		sprintf(GroundStations[x].Name,"INVALID");
		sprintf(GroundStations[x].Code,"XXX");
		x++;
	}	
	// Reset capcom interface
	menuState = 0;
	padState = 0;
	padNumber = 0;
	NHmenu = 0;
	NHmessages = 0;
	NHpad = 0;
}

void MCC::Init(Saturn *vs){
	// Set CM pointer
	cm = vs;

	// Obtain Earth and Moon pointers
	Earth = oapiGetGbodyByName("Earth");
	Moon = oapiGetGbodyByName("Moon");
	
	// GROUND TRACKING INITIALIZATION
	LastAOSUpdate=0;

	// Load ground station information.
	// Later this can be made dynamic, but this will work for now.

	// This particular ground station list is the stations that were on-net for Apollo 8.
	// The index numbers represent status as of 1968.	
	sprintf(GroundStations[1].Name,"ANTIGUA"); sprintf(GroundStations[1].Code,"ANG");
	// GroundStations[1].Position[0] = 28.40433; GroundStations[1].Position[1] = -80.60192; // This is the Cape?
	GroundStations[1].Position[0] = 17.137222; GroundStations[1].Position[1] = -61.775833;
	GroundStations[1].SBandAntenna = GSSA_9METER;
	GroundStations[1].HasAcqAid = true;
	GroundStations[1].DownTlmCaps = GSDT_USB;
	GroundStations[1].UpTlmCaps = GSCC_USB;
	GroundStations[1].StationPurpose = GSPT_LV_CUTOFF|GSPT_NEAR_SPACE;
	GroundStations[1].CommCaps = 0;
	GroundStations[1].USBCaps = 0;
	GroundStations[1].TrackingCaps = 0;
	GroundStations[1].Active = true;

	sprintf(GroundStations[2].Name,"ASCENSION"); sprintf(GroundStations[2].Code,"ASC");
	GroundStations[2].Position[0] = -7.94354; GroundStations[2].Position[1] = -14.37105;
	GroundStations[2].SBandAntenna = GSSA_9METER;
	GroundStations[2].HasAcqAid = true;
	GroundStations[2].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[2].UpTlmCaps = GSCC_USB;
	GroundStations[2].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[2].CommCaps = 0;
	GroundStations[2].USBCaps = 0;
	GroundStations[2].TrackingCaps = GSTK_CBAND_LOWSPEED;
	GroundStations[2].Active = true;

	sprintf(GroundStations[3].Name,"BERMUDA"); sprintf(GroundStations[3].Code,"BDA");
	GroundStations[3].Position[0] = 32.36864; GroundStations[3].Position[1] = -64.68563;
	GroundStations[3].SBandAntenna = GSSA_9METER;
	GroundStations[3].HasRadar = true;
	GroundStations[3].HasAcqAid = true;
	GroundStations[3].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[3].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[3].StationPurpose = GSPT_LV_CUTOFF|GSPT_NEAR_SPACE;
	GroundStations[3].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[3].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[3].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[3].Active = true;

	sprintf(GroundStations[4].Name,"GRAND CANARY"); sprintf(GroundStations[4].Code,"CYI");
	GroundStations[4].Position[0] = 27.74055; GroundStations[4].Position[1] = -15.60077;
	GroundStations[4].SBandAntenna = GSSA_9METER;
	GroundStations[4].HasRadar = true;
	GroundStations[4].HasAcqAid = true;
	GroundStations[4].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[4].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[4].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[4].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_USB;
	GroundStations[4].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[4].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[4].Active = true;

	sprintf(GroundStations[5].Name,"HONEYSUCKLE"); sprintf(GroundStations[5].Code,"HSK");
	GroundStations[5].Position[0] = -35.40282; GroundStations[5].Position[1] = 148.98144;
	GroundStations[5].SBandAntenna = GSSA_26METER;
	GroundStations[5].DownTlmCaps = GSDT_USB;
	GroundStations[5].UpTlmCaps = GSCC_USB;
	GroundStations[5].StationPurpose = GSPT_LUNAR;
	GroundStations[5].TrackingCaps = GSTK_USB;
	GroundStations[5].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[5].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VIDEO;
	GroundStations[5].Active = true;

	sprintf(GroundStations[6].Name,"CARNARVON"); sprintf(GroundStations[6].Code,"CRO");
	GroundStations[6].Position[0] = -24.90619; GroundStations[6].Position[1] = 113.72595;
	GroundStations[6].SBandAntenna = GSSA_9METER;
	GroundStations[6].HasRadar = true;
	GroundStations[6].HasAcqAid = true;
	GroundStations[6].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[6].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[6].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[6].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[6].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[6].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[6].Active = true;

	sprintf(GroundStations[7].Name,"CORPUS CHRISTI"); sprintf(GroundStations[7].Code,"TEX");
	GroundStations[7].Position[0] = 27.65273; GroundStations[7].Position[1] = -97.37588;
	GroundStations[7].SBandAntenna = GSSA_9METER;
	GroundStations[7].HasAcqAid = true;
	GroundStations[7].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[7].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[7].StationPurpose = GSPT_ORBITAL;
	GroundStations[7].TrackingCaps = GSTK_USB;
	GroundStations[7].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[7].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[7].Active = true;

	sprintf(GroundStations[8].Name,"GOLDSTONE"); sprintf(GroundStations[8].Code,"GDS");
	GroundStations[8].Position[0] = 35.33820; GroundStations[8].Position[1] = -116.87421;
	GroundStations[8].SBandAntenna = GSSA_26METER;
	GroundStations[8].DownTlmCaps = GSDT_USB;
	GroundStations[8].UpTlmCaps = GSCC_USB;
	GroundStations[8].StationPurpose = GSPT_LUNAR;
	GroundStations[8].TrackingCaps = GSTK_USB;
	GroundStations[8].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[8].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VIDEO;
	GroundStations[8].Active = true;

	sprintf(GroundStations[9].Name,"GRAND BAHAMA"); sprintf(GroundStations[9].Code,"GBM");
	GroundStations[9].Position[0] = 26.62022; GroundStations[9].Position[1] = -78.35825;
	GroundStations[9].SBandAntenna = GSSA_9METER;
	GroundStations[9].HasAcqAid = true;
	GroundStations[9].DownTlmCaps = GSDT_USB;
	GroundStations[9].UpTlmCaps = GSCC_USB;
	GroundStations[9].StationPurpose = GSPT_LAUNCH|GSPT_ORBITAL;
	GroundStations[9].TrackingCaps = GSTK_USB;
	GroundStations[9].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[9].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[9].Active = true;

	sprintf(GroundStations[10].Name,"GUAM"); sprintf(GroundStations[10].Code,"GWM");
	GroundStations[10].Position[0] = 13.30929; GroundStations[10].Position[1] = 144.73694;
	GroundStations[10].SBandAntenna = GSSA_9METER;
	GroundStations[10].HasAcqAid = true;
	GroundStations[10].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[10].UpTlmCaps = GSCC_USB;
	GroundStations[10].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[10].TrackingCaps = GSTK_USB;
	GroundStations[10].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[10].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[10].Active = true;

	sprintf(GroundStations[11].Name,"GUAYMAS"); sprintf(GroundStations[11].Code,"GYM");
	GroundStations[11].Position[0] = 27.95029; GroundStations[11].Position[1] = -110.90846;
	GroundStations[11].SBandAntenna = GSSA_9METER;
	GroundStations[11].HasAcqAid = true;
	GroundStations[11].DownTlmCaps = GSDT_USB;
	GroundStations[11].UpTlmCaps = GSCC_USB;
	GroundStations[11].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[11].TrackingCaps = GSTK_USB;
	GroundStations[11].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[11].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[11].Active = true;

	sprintf(GroundStations[12].Name,"HAWAII"); sprintf(GroundStations[12].Code,"HAW");
	GroundStations[12].Position[0] = 21.44719; GroundStations[12].Position[1] = -157.76307;
	GroundStations[12].SBandAntenna = GSSA_9METER;
	GroundStations[12].HasAcqAid = true;
	GroundStations[12].HasRadar = true;
	GroundStations[12].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[12].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[12].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[12].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_USB;
	GroundStations[12].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[12].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[12].Active = true;

	sprintf(GroundStations[13].Name,"MADRID"); sprintf(GroundStations[13].Code,"MAD");
	GroundStations[13].Position[0] = 40.45443; GroundStations[13].Position[1] = -4.16990;
	GroundStations[13].SBandAntenna = GSSA_26METER;
	GroundStations[13].DownTlmCaps = GSDT_USB;
	GroundStations[13].UpTlmCaps = GSCC_USB;
	GroundStations[13].StationPurpose = GSPT_LUNAR;
	GroundStations[13].TrackingCaps = GSTK_USB;
	GroundStations[13].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[13].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VIDEO;
	GroundStations[13].Active = true;

	sprintf(GroundStations[14].Name,"MERRIT"); sprintf(GroundStations[14].Code,"MIL");
	GroundStations[14].Position[0] = 28.40433; GroundStations[14].Position[1] = -80.60192;
	GroundStations[14].SBandAntenna = GSSA_3PT7METER;
	GroundStations[14].HasRadar = true;
	GroundStations[14].HasAcqAid = true;
	GroundStations[14].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[14].StationPurpose = GSPT_PRELAUNCH|GSPT_LAUNCH|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[14].TrackingCaps = GSTK_USB;
	GroundStations[14].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[14].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[14].Active = true;

	sprintf(GroundStations[15].Name,"USNS HUNTSVILLE"); sprintf(GroundStations[15].Code,"HTV");
	GroundStations[15].SBandAntenna = GSSA_3PT7METER;
	GroundStations[15].HasRadar = true;
	GroundStations[15].HasAcqAid = true;
	GroundStations[15].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[15].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_USB;
	GroundStations[15].USBCaps = GSSC_VOICE|GSSC_TELEMETRY;
	GroundStations[15].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE;
	GroundStations[15].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;

	sprintf(GroundStations[16].Name,"USNS MERCURY"); sprintf(GroundStations[16].Code,"MER");
	GroundStations[16].SBandAntenna = GSSA_9METER;
	GroundStations[16].HasRadar = true;
	GroundStations[16].HasAcqAid = true;
	GroundStations[16].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[16].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[16].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[16].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[16].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[16].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	sprintf(GroundStations[17].Name,"USNS REDSTONE"); sprintf(GroundStations[17].Code,"RED");
	GroundStations[17].SBandAntenna = GSSA_9METER;
	GroundStations[17].HasRadar = true;
	GroundStations[17].HasAcqAid = true;
	GroundStations[17].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[17].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[17].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[17].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[17].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[17].CommCaps = GSGC_DATAHISPEED|GSGC_DATAWIDEBAND|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	sprintf(GroundStations[18].Name,"USNS VANGUARD"); sprintf(GroundStations[18].Code,"VAN");
	GroundStations[18].SBandAntenna = GSSA_9METER;
	GroundStations[18].HasRadar = true;
	GroundStations[18].HasAcqAid = true;
	GroundStations[18].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[18].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[18].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[18].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[18].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[18].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	sprintf(GroundStations[19].Name,"USNS WATERTOWN"); sprintf(GroundStations[19].Code,"WTN");
	GroundStations[19].SBandAntenna = GSSA_3PT7METER;
	GroundStations[19].HasRadar = true;
	GroundStations[19].HasAcqAid = true;
	GroundStations[19].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[19].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	// WATERTOWN was not listed in the capabilites table, so I assumed it was outfitted with the usual equipment.
	// This is because it was released to the DoD in 1968, between the Apollo 7 and Apollo 8 flights.
	GroundStations[19].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[19].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[19].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	// 20 = ARIA-1
	// 21 = ARIA-2
	// 22 = ARIA-3
	// 23 = ARIA-4
	// 24 = ARIA-5
	// 25 = ARIA-6
	// 26 = ARIA-7
	// 27 = ARIA-8
	// 28 = MSFNOC (? THE OPERATIONS CENTER ?)

	// 29 = HSK WING
	sprintf(GroundStations[29].Name,"HONEYSUCKLE"); sprintf(GroundStations[29].Code,"HSKX");
	GroundStations[29].Position[0] = -35.40282; GroundStations[29].Position[1] = 148.98144;
	GroundStations[29].SBandAntenna = GSSA_26METER;
	GroundStations[29].StationPurpose = GSPT_LUNAR;
	GroundStations[29].TrackingCaps = GSTK_USB;
	GroundStations[29].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[29].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[29].Active = true;

	// 30 = GDS WING
	sprintf(GroundStations[30].Name,"GOLDSTONE"); sprintf(GroundStations[30].Code,"GDSX");
	GroundStations[30].Position[0] = 35.33820; GroundStations[30].Position[1] = -116.87421;
	GroundStations[30].SBandAntenna = GSSA_26METER;
	GroundStations[30].StationPurpose = GSPT_LUNAR;
	GroundStations[30].TrackingCaps = GSTK_USB;
	GroundStations[30].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[30].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[30].Active = true;

	// 31 = MAD WING
	sprintf(GroundStations[31].Name,"MADRID"); sprintf(GroundStations[31].Code,"MADX");
	GroundStations[31].Position[0] = 40.45443; GroundStations[31].Position[1] = -4.16990;
	GroundStations[31].SBandAntenna = GSSA_26METER;
	GroundStations[31].StationPurpose = GSPT_LUNAR;
	GroundStations[31].TrackingCaps = GSTK_USB;
	GroundStations[31].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[31].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[31].Active = true;

	// 32 = NTTF (PRELAUNCH?)

	sprintf(GroundStations[33].Name,"TANANARIVE"); sprintf(GroundStations[33].Code,"TAN");
	GroundStations[33].Position[0] = -19.00000; GroundStations[33].Position[1] = 47.27556;	
	GroundStations[33].HasRadar = true;
	GroundStations[33].HasAcqAid = true;
	GroundStations[33].StationPurpose = GSPT_ORBITAL;
	GroundStations[33].TrackingCaps = GSTK_CBAND_LOWSPEED;
	GroundStations[33].USBCaps = 0;
	GroundStations[33].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE;
	GroundStations[33].Active = true;

	// ALL SUBSEQUENT ARE DOD STATIONS
	sprintf(GroundStations[34].Name,"ANTIGUA"); sprintf(GroundStations[34].Code,"ANT");
	GroundStations[34].Position[0] = 17.137222; GroundStations[34].Position[1] = -61.775833;
	GroundStations[34].HasRadar = true;
	GroundStations[34].HasAcqAid = true;
	GroundStations[34].UpTlmCaps = GSCC_UHF;
	GroundStations[34].StationPurpose = GSPT_ORBITAL;
	GroundStations[34].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[34].USBCaps = 0;
	GroundStations[34].CommCaps = 0;
	GroundStations[34].Active = true;

	sprintf(GroundStations[35].Name,"ASCENSION"); sprintf(GroundStations[35].Code,"ASC");
	GroundStations[35].Position[0] = -7.969444; GroundStations[35].Position[1] = -14.393889;
	GroundStations[35].HasRadar = true;
	GroundStations[35].HasAcqAid = true;
	GroundStations[35].StationPurpose = GSPT_ORBITAL;
	GroundStations[35].TrackingCaps = GSTK_USB;
	GroundStations[35].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[35].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[35].Active = true;

	// CAPE KENNEDY: These numbers are guesses. Since it's a DOD site, it's probably at the AFB.
	sprintf(GroundStations[36].Name,"CAPE KENNEDY"); sprintf(GroundStations[36].Code,"CNV");
	GroundStations[36].Position[0] = 28.488889; GroundStations[36].Position[1] = -80.577778;
	GroundStations[36].SBandAntenna = GSSA_9METER;
	GroundStations[36].StationPurpose = GSPT_LAUNCH;
	GroundStations[36].TrackingCaps = GSTK_OPTICAL|GSTK_ODOP|GSTK_CBAND_HIGHSPEED;
	GroundStations[36].USBCaps = 0;
	GroundStations[36].CommCaps = GSGC_VHFAG_VOICE;
	GroundStations[36].Active = true;	

	sprintf(GroundStations[37].Name,"GRAND BAHAMA"); sprintf(GroundStations[37].Code,"GBI");
	GroundStations[37].Position[0] = 26.574167; GroundStations[37].Position[1] = -78.664722;
	GroundStations[37].HasRadar = true;
	GroundStations[37].HasAcqAid = true;
	GroundStations[37].DownTlmCaps = GSDT_VHF;
	GroundStations[37].UpTlmCaps = GSCC_UHF;
	GroundStations[37].StationPurpose = GSPT_LAUNCH;
	GroundStations[37].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[37].USBCaps = 0;
	GroundStations[37].CommCaps = 0;
	GroundStations[37].Active = true;

	sprintf(GroundStations[38].Name,"MERRIT ISLAND"); sprintf(GroundStations[38].Code,"MLA");
	GroundStations[38].Position[0] = 28.488889; GroundStations[38].Position[1] = -80.577778; // Generic KSC coordinates.
	GroundStations[38].HasRadar = true;
	GroundStations[38].HasAcqAid = true;
	GroundStations[38].StationPurpose = GSPT_LAUNCH;
	GroundStations[38].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[38].USBCaps = 0;
	GroundStations[38].CommCaps = 0;
	GroundStations[38].Active = true;

	sprintf(GroundStations[39].Name,"PATRICK AFB"); sprintf(GroundStations[39].Code,"PAT");	
	GroundStations[39].Position[0] = 28.235; GroundStations[39].Position[1] = -80.61;
	GroundStations[39].HasRadar = true;
	GroundStations[39].StationPurpose = GSPT_LAUNCH;
	GroundStations[39].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[39].USBCaps = 0;
	GroundStations[39].CommCaps = 0;
	GroundStations[39].Active = true;

	sprintf(GroundStations[40].Name,"PRETORIA"); sprintf(GroundStations[40].Code,"PRE");
	GroundStations[40].Position[0] = -30.78330; GroundStations[40].Position[1] = 28.58330;	
	GroundStations[40].HasRadar = true;
	GroundStations[40].StationPurpose = GSPT_ORBITAL;
	GroundStations[40].TrackingCaps = GSTK_CBAND_LOWSPEED;
	GroundStations[40].USBCaps = 0;
	GroundStations[40].CommCaps = 0;
	GroundStations[40].Active = true;

	sprintf(GroundStations[41].Name,"VANDENBERG"); sprintf(GroundStations[41].Code,"CAL");
	GroundStations[41].Position[0] = 34.74007; GroundStations[41].Position[1] = -120.61909;	
	GroundStations[41].HasRadar = true;
	GroundStations[41].HasAcqAid = true;
	GroundStations[41].StationPurpose = GSPT_ORBITAL;
	GroundStations[41].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[41].USBCaps = 0;
	GroundStations[41].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE;
	GroundStations[41].Active = true;

	sprintf(GroundStations[42].Name,"WHITE SANDS"); sprintf(GroundStations[42].Code,"WHS");
	GroundStations[42].Position[0] = 32.35637; GroundStations[42].Position[1] = -106.37826;
	GroundStations[42].HasRadar = true;
	GroundStations[42].HasAcqAid = true;
	GroundStations[42].StationPurpose = GSPT_ORBITAL;
	GroundStations[42].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[42].USBCaps = 0;
	GroundStations[42].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[42].Active = true;

	// MISSION STATE
	MissionState = MST_PRELAUNCH;
	// Revolutions count up from 1.
	EarthRev = 1;
	MoonRev = 1;

	// CAPCOM INTERFACE INITIALIZATION
	// Get handles to annotations.
	// The menu lives in the top left, the message box to the right of that
	NHmenu = oapiCreateAnnotation(false,0.75,_V(1,1,0));
	oapiAnnotationSetPos(NHmenu,0,0,0.15,0.2);
	NHmessages = oapiCreateAnnotation(false,0.75,_V(1,1,0));
	oapiAnnotationSetPos(NHmessages,0.18,0,0.87,0.2);
	NHpad = oapiCreateAnnotation(false,0.75,_V(1,1,0));
	oapiAnnotationSetPos(NHpad,0,0.2,0.33,1);
	// Clobber message output buffer
	msgOutputBuf[0] = 0;
	// Clobber the message ring buffer, then set the index back to zero
	currentMessage = 0;
	while(currentMessage < MAX_MESSAGES){
		messages[currentMessage][0] = 0;
		msgtime[currentMessage] = -1;
		currentMessage++;
	}
	currentMessage = 0;

}

void MCC::TimeStep(double simdt){
	int x=0,y=0,z=0;				// Scratch
	char buf[MAX_MSGSIZE];			// More Scratch

	/* AOS DETERMINATION */
	
	if(GT_Enabled == true){
		LastAOSUpdate += simdt;
		if(LastAOSUpdate > 1){
			double LateralRange;	
			double SlantRange;
			double LOSRange;
			VECTOR3 CMGlobalPos = _V(0,0,0);

			LastAOSUpdate = 0;
			// Bail out if we failed to find either major body
			if(Earth == NULL){ addMessage("Can't find Earth"); GT_Enabled = false; return; }
			if(Moon == NULL){ addMessage("Can't find Moon"); GT_Enabled = false; return; }
				
			// Update previous position data
			CM_Prev_Position[0] = CM_Position[0];
			CM_Prev_Position[1] = CM_Position[1];
			CM_Prev_Position[2] = CM_Position[2];
			// Obtain CM's global position
			cm->GetGlobalPos(CMGlobalPos);
			// Convert to Earth equatorial
			oapiGlobalToEqu(Earth,CMGlobalPos,&CM_Position[1],&CM_Position[0],&CM_Position[2]);
			// Convert from radians
			CM_Position[0] *= DEG; 
			CM_Position[1] *= DEG; 
			// Convert from radial distance
			CM_Position[2] -= 6373338; // Launch pad radius should be good enough

			// If we just crossed the rev line, count it
			if(CM_Prev_Position[1] < -80 && CM_Position[1] >= -80 && cm->stage >= STAGE_ORBIT_SIVB){
				EarthRev++;
				sprintf(buf,"Rev %d",EarthRev);
				addMessage(buf);
			}

			if(CM_Position[2] < 23546000){
				// Our comm range is lunar even with an omni, so the determining factor for AOS-ness is angle above horizon.
				// But we are lazy, so we'll figure our line-of-sight radio range from our altitude instead.
				LOSRange = 4120*sqrt(CM_Position[2]); // In meters
				if(LOSRange > 20000000){ LOSRange = 20000000; } // Cap it just in case
			}else{
				// We are high enough to cover the entire hemisphere. No point in doing the math.
				LOSRange = 20000000;
			}

			y = 0;
			while(x<MAX_GROUND_STATION){
				if(GroundStations[x].Active == true){
					// Get lateral range
					LateralRange = sqrt(pow(GroundStations[x].Position[0]-CM_Position[0],2)+pow(GroundStations[x].Position[1]-CM_Position[1],2));
					LateralRange *= 111123; // Nice number, isn't it? Meters per degree.
					// Figure slant range
					SlantRange = sqrt((LateralRange*LateralRange)+(CM_Position[2]*CM_Position[2]));
					// Check
					if(SlantRange < LOSRange && GroundStations[x].AOS == 0 && ((GroundStations[x].USBCaps&GSSC_VOICE)||(GroundStations[x].CommCaps&GSGC_VHFAG_VOICE))){
						GroundStations[x].AOS = 1;
						sprintf(buf,"AOS %s",GroundStations[x].Name);
						addMessage(buf);
					}
					if(SlantRange > LOSRange && GroundStations[x].AOS == 1){
						GroundStations[x].AOS = 0;
						sprintf(buf,"LOS %s",GroundStations[x].Name);
						addMessage(buf);
					}			
					if(GroundStations[x].AOS){ y++; }
				}
				x++;
			}
		}
	}

	// MISSION STATE EVALUATOR
	if(MT_Enabled == true){
		// Make sure ground tracking is also on
		if(GT_Enabled == false){ addMessage("Mission tracking requires ground tracking"); MT_Enabled = false; return; }
		// Handle global mission states
		switch(MissionState){			
		case MST_INIT:
			// INITIALIZATION STATE
			AbortMode = 0;
			// Determine mission type.
			switch(cm->ApolloNo){
				case 7:
					MissionType = MTP_C;
					MissionState = MST_1B_PRELAUNCH;
					break;
				case 8:
					MissionType = MTP_C_PRIME;
					MissionState = MST_SV_PRELAUNCH;
					break;
				case 9:
					MissionType = MTP_D;
					break;
				case 10:
					MissionType = MTP_F;
					break;
				case 11:
					MissionType = MTP_G;
					break;
				case 12:
				case 13:
				case 14:
					MissionType = MTP_H;
					break;
				case 15:
				case 16:
				case 17:
					MissionType = MTP_J;
					break;
				default:
					// If the ApolloNo is not on this list, you are expected to provide a mission type in the scenario file, which will override the default.
					if(cm->SaturnType == SAT_SATURNV){
						MissionType = MTP_H;
					}
					if(cm->SaturnType == SAT_SATURN1B){
						MissionType = MTP_C;
					}
					break;

			}
			if(MissionType == 0){
				sprintf(buf,"Unsupported Mission %d",cm->ApolloNo);
				addMessage(buf);
				MT_Enabled = false;
			}
			break;
		case MST_1B_PRELAUNCH:
			// Await launch
			if(cm->stage >= CM_STAGE){
				// Pad Abort
				addMessage("PAD ABORT");
				MissionState = MST_ABORT_PL;					
			}else{
				if(cm->stage == LAUNCH_STAGE_ONE){
					// Normal Liftoff
					addMessage("LIFTOFF!");
					MissionState = MST_1B_LAUNCH;
				}
			}
			break;
		case MST_SV_PRELAUNCH:
			// Await launch
			if(cm->stage >= CM_STAGE){
				// Pad Abort
				addMessage("PAD ABORT");
				MissionState = MST_ABORT_PL;
			}else{
				if(cm->stage == LAUNCH_STAGE_ONE){
					// Normal Liftoff
					addMessage("LIFTOFF!");
					MissionState = MST_SV_LAUNCH;
				}
			}
			break;
		case MST_1B_LAUNCH:
			// Abort?
			if(cm->bAbort){
				// What type?
				if(cm->LETAttached()){
					// ABORT MODE 1
					addMessage("ABORT MODE 1");
					MissionState = MST_LAUNCH_ABORT;
					AbortMode = 1;
				}else{
					// AP7 Abort Summary says:
					// Mode 2: 2:46 - 9:30
					// Mode 2 is selected if we are within the time range and must land immediately/no other options available.
					// Mode 3: 9:30 - Insertion
					// Mode 3 is selected if we are within the time range and must land immediately.
					// Mode 4: 9:21 - Insertion
					// Mode 4 is selected if we are within the time range and can land in the Pacific.
					addMessage("ABORT MODE 2/3/4");
					MissionState = MST_LAUNCH_ABORT;
					AbortMode = 2;
				}
			}else{
				// Await insertion
				if(cm->stage >= STAGE_ORBIT_SIVB){
					switch(MissionType){
					case MTP_C:
						addMessage("INSERTION");
						MissionState = MST_C_INSERTION;
						break;
					}				
				}
			}
			break;
		case MST_SV_LAUNCH:
			// Abort?
			if(cm->bAbort){
				// What type?
				if(cm->LETAttached()){
					// ABORT MODE 1
					addMessage("ABORT MODE 1");
					MissionState = MST_LAUNCH_ABORT;
					AbortMode = 1;
				}else{
					// ABORT MODE 2/3/4
					addMessage("ABORT MODE 2/3/4");
					MissionState = MST_LAUNCH_ABORT;
					AbortMode = 2;
				}
			}else{
				// Await insertion
				if(cm->stage >= STAGE_ORBIT_SIVB){
					switch(MissionType){
					case MTP_C_PRIME:
						addMessage("INSERTION");
						MissionState = MST_CP_INSERTION;
						break;
					}				
				}
			}
			break;
		}
		// Now handle mission-specific states
		switch(MissionType){
		case MTP_C:
			/* *********************
			 * MISSION C: APOLLO 7 *
			 ********************* */
			switch(MissionState){
			case MST_C_INSERTION:
				// Await separation.
				if(cm->stage == CSM_LEM_STAGE){
					MissionState = MST_C_SEPARATION;
				}else{
					break;
				}
				// FALL INTO
			case MST_C_SEPARATION:
				// Ends with 1ST RDZ PHASING BURN
				// The phasing burn was intended to place the spacecraft 76.5 n mi ahead of the S-IVB in 23 hours.
				break;
			case MST_C_COAST1:
				// Ends with 1ST SPS BURN (NCC BURN)
				break;
			}
		}
	}

	// MESSAGE LIST MAINTENANCE should come last that way if any of the above prints anything,
	// it gets printed in this timestep rather than the next.
	x = currentMessage+1;					// Index, Point beyond tail
	y = 0;									// Tail of output buffer
	z = 0;									// Updation flag
	msgOutputBuf[0] = 0;					// Clobber buffer
	if(x == MAX_MESSAGES){ x = 0; }			// Wrap index if necessary
	while(x != currentMessage){				// we'll wait till it comes around on the guitar here, and sing it when it does
		if(msgtime[x] != -1){				// with feeling!
			if(msgtime[x] == 0){ z = 1; }	// If message is brand new, set updation flag			
			msgtime[x] += simdt;			// Increment lifetime
			if(msgtime[x] > MSG_DISPLAY_TIME){
				// messages[x][0] = 0;			// Message too old, kill it
				msgtime[x] = -1;			// Mark not-in-use
				z = 1;						// Set updation flag
			}else{				
				sprintf(msgOutputBuf+y,"%s\n",messages[x]);	// Otherwise push to output buffer
				y += strlen(messages[x])+1; // Add message length plus NL
			}
		}
		x++;								// Next message
		if(x == MAX_MESSAGES){ x = 0; }		// Wrap index if necessary
	}
	if(z == 1){								// If we set the updation flag
		oapiAnnotationSetText(NHmessages,msgOutputBuf);	// update the annotation
	}
}

// Add message to ring buffer
void MCC::addMessage(char *msg){
	strncpy(messages[currentMessage],msg,MAX_MSGSIZE);			// Copy string
	msgtime[currentMessage] = 0;								// Mark new
	currentMessage++;											// Advance tail index
	if(currentMessage == MAX_MESSAGES){ currentMessage = 0; }	// Wrap index if necessary
}

// Redisplay ring buffer
void MCC::redisplayMessages(){
	int x=0;
	while(x < MAX_MESSAGES){
		if(messages[x][0] != 0){
			msgtime[x] = 0;
		}
		x++;
	}
	// The ring buffer pointer will walk from the current message forward, so anything that was displayed
	// should be redisplayed in the right order.
}

// Subthread Entry Point
int MCC::subThread(){
	int Result = 0;
	subThreadStatus = 2; // Running
	switch(subThreadMode){
	case 0: // Test
		Sleep(5000); // Waste 5 seconds
		Result = 0;  // Success (negative = error)
		break;
	}
	subThreadStatus = Result;
	// Printing messages from the subthread is not safe, but this is just for testing.
	addMessage("Thread Completed");
	return(0);
}

// Subthread initiation
int MCC::startSubthread(int fcn){
	if(subThreadStatus < 1){
		// Punt thread
		subThreadMode = fcn;
		subThreadStatus = 1; // Busy
		DWORD id = 0;
		HANDLE h = CreateThread(NULL, 0, MCC_Trampoline, this, 0, &id);
		if(h != NULL){ CloseHandle(h); }
		addMessage("Thread Started");
	}else{
		addMessage("Thread Busy");
		return(-1);
	}
	return(0);
}

// Draw PAD display
void MCC::drawPad(){
	char buffer[512];
	switch(padNumber){
	case 0:
		// NO PAD (*knifed*)
		oapiAnnotationSetText(NHpad,"No PAD");
		break;
	default:
		sprintf(buffer,"Unknown padNumber %d",padNumber);
		oapiAnnotationSetText(NHpad,buffer);
		break;
	}
}

// Keypress handler
void MCC::keyDown(DWORD key){
	char buf[MAX_MSGSIZE];
	switch(key){
		case OAPI_KEY_TAB:
			if(menuState == 0){
				oapiAnnotationSetText(NHmenu,"CAPCOM MENU\n1: Voice Check\n2: Toggle Ground Trk\n3: Toggle Mission Trk\n4: Thread Test\n5: Hide/Show PAD\n6: Redisplay Messages"); // Present menu
				menuState = 1;
			}else{
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_1:
			if(menuState == 1){
				sprintf(buf,"Voice Check");
				addMessage(buf);
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_2:
			if(menuState == 1){
				if(GT_Enabled == false){
					GT_Enabled = true;
					sprintf(buf,"Ground Tracking Enabled");
				}else{
					GT_Enabled = false;
					sprintf(buf,"Ground Tracking Disabled");
				}
				addMessage(buf);
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_3:
			if(menuState == 1){
				if(MT_Enabled == false){
					MT_Enabled = true; GT_Enabled = true;
					sprintf(buf,"Mission and Ground Tracking Enabled");
				}else{
					MT_Enabled = false;
					sprintf(buf,"Mission Tracking Disabled");
				}
				addMessage(buf);
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_4:
			if(menuState == 1){
				startSubthread(0);
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_5:
			padNumber = 5; // Cheat
			if(menuState == 1){
				if(padState == 1){
					oapiAnnotationSetText(NHpad,""); // Clear PAD
					padState = 0;
				}else{
					if(padNumber == 0){
						addMessage("No PAD available");
					}else{
						drawPad();
						padState = 1;
					}
				}
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_6:
			if(menuState == 1){
				redisplayMessages();
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
	}
}
