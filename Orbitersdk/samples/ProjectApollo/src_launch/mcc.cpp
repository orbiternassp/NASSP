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
#include "papi.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"
#include "mcc.h"
#include "rtcc.h"

// This is a threadenwerfer. It werfs threaden.
static DWORD WINAPI MCC_Trampoline(LPVOID ptr){	
	MCC *mcc = (MCC *)ptr;
	return(mcc->subThread());
}

// SCENARIO FILE MACROLOGY
#define SAVE_BOOL(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_INT(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_DOUBLE(KEY,VALUE) papiWriteScenario_double(scn, KEY, VALUE)
#define SAVE_V3(KEY,VALUE) papiWriteScenario_vec(scn, KEY, VALUE)
#define SAVE_M3(KEY,VALUE) papiWriteScenario_mx(scn, KEY, VALUE)
#define SAVE_STRING(KEY,VALUE) oapiWriteScenario_string(scn, KEY, VALUE)
#define LOAD_BOOL(KEY,VALUE) if(strnicmp(line, KEY, strlen(KEY)) == 0){ sscanf(line + strlen(KEY), "%i", &tmp); if (tmp == 1) { VALUE = true; } else { VALUE = false; } }
#define LOAD_INT(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%i",&VALUE); }
#define LOAD_DOUBLE(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf",&VALUE); }
#define LOAD_V3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf",&VALUE.x,&VALUE.y,&VALUE.z); }
#define LOAD_M3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&VALUE.m11,&VALUE.m12,&VALUE.m13,&VALUE.m21,&VALUE.m22,&VALUE.m23,&VALUE.m31,&VALUE.m32,&VALUE.m33); }
#define LOAD_STRING(KEY,VALUE,LEN) if(strnicmp(line,KEY,strlen(KEY))==0){ strncpy(VALUE, line + (strlen(KEY)+1), LEN); }

// CONS
MCC::MCC(){
	// Reset data
	rtcc = NULL;
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
	padState = -1;
	padNumber = 0;
	padForm = NULL;
	NHmenu = 0;
	NHmessages = 0;
	NHpad = 0;
	StateTime = 0;
	SubStateTime = 0;
	PCOption_Enabled = false;
	PCOption_Text[0] = 0;
	NCOption_Enabled = false;
	NCOption_Text[0] = 0;
}

void MCC::Init(Saturn *vs){
	// Make a new RTCC if we don't have one already
	if (rtcc == NULL) { rtcc = new RTCC; rtcc->Init(this); }
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
	setState(MST_PRELAUNCH);
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
	// Default PC/NC
	PCOption_Enabled = false;
	sprintf(PCOption_Text, "Roger");
	NCOption_Enabled = false;
	sprintf(NCOption_Text, "Negative");
	// Uplink items
	uplink_size = 0;
}

void MCC::setState(int newState){
	MissionState = newState;
	SubState = 0;
	StateTime = 0;
	SubStateTime = 0;
}

void MCC::setSubState(int newState){
	SubState = newState;
	SubStateTime = 0;
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
		// Clock timers
		StateTime += simdt;
		SubStateTime += simdt;
		// Handle global mission states
		switch(MissionState){			
		case MST_INIT:
			// INITIALIZATION STATE
			AbortMode = 0;
			// Determine mission type.
			switch(cm->ApolloNo){
				case 7:
					MissionType = MTP_C;
					setState(MST_1B_PRELAUNCH);
					break;
				case 8:
					MissionType = MTP_C_PRIME;
					setState(MST_SV_PRELAUNCH);
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
				setState(MST_ABORT_PL);					
			}else{
				if(cm->stage == LAUNCH_STAGE_ONE){
					// Normal Liftoff
					addMessage("LIFTOFF!");
					setState(MST_1B_LAUNCH);
				}
			}
			break;
		case MST_SV_PRELAUNCH:
			// Await launch
			if(cm->stage >= CM_STAGE){
				// Pad Abort
				addMessage("PAD ABORT");
				setState(MST_ABORT_PL);
			}else{
				if(cm->stage == LAUNCH_STAGE_ONE){
					// Normal Liftoff
					addMessage("LIFTOFF!");
					setState(MST_SV_LAUNCH);
				}
			}
			break;
		case MST_1B_LAUNCH:
			// CALLOUTS TO MAKE:
			// TOWER CLEAR
			if(SubState == 0 && CM_Position[2] > 100){
				addMessage("CLEAR OF THE TOWER");
				setSubState(1);
			}
			// ABORT MODES (1B, 1C)
			// These callouts can safely be made based purely on time; They are early enough in the mission that
			// large deviations from the mission plan will likely cause an abort anyway. Later abort callouts
			// must be made on criteria.
			if(SubState == 1 && StateTime > 61){
				addMessage("MODE 1B");
				setSubState(2);
			}
			if(SubState == 2 && StateTime > 110){
				addMessage("MODE 1C");
				setSubState(3);
			}
			// GO FOR STAGING
			// (What criteria?)

			// TRAJECTORY/GUIDANCE GO
			// (What criteria?)

			// ABORT MODE 4
			// (What criteria?)

			// Abort?
			if(cm->bAbort){
				// What type?
				if(cm->LETAttached()){
					// ABORT MODE 1
					addMessage("ABORT MODE 1");
					setState(MST_LAUNCH_ABORT);
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
					setState(MST_LAUNCH_ABORT);
					AbortMode = 2;
				}
			}else{
				// Await insertion
				if(cm->stage >= STAGE_ORBIT_SIVB){
					switch(MissionType){
					case MTP_C:
						addMessage("INSERTION");
						setState(MST_C_INSERTION);
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
					setState(MST_LAUNCH_ABORT);
					AbortMode = 1;
				}else{
					// ABORT MODE 2/3/4
					addMessage("ABORT MODE 2/3/4");
					setState(MST_LAUNCH_ABORT);
					AbortMode = 2;
				}
			}else{
				// Await insertion
				if(cm->stage >= STAGE_ORBIT_SIVB){
					switch(MissionType){
					case MTP_C_PRIME:
						addMessage("INSERTION");
						setState(MST_CP_INSERTION);
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
				// CALLOUTS TO MAKE:
				// Acknowledge CDR's SECO call
				// Acknowledge CDR's DSKY reports
				// GO FOR ORBIT
				// S4B SAFED
				// ORBIT APOGEE/PERIGEE IN (ROUND) NAUTICAL MILES
				// LOX dump time and expected dV
				// NAV uplink of some kind (state vector?) and GO for CDR to leave seat and/or unsuiting (?)
				// S4B Passivation complete (OK to terminate P47)
				// S4B takeover experiment
				// GO for pyro arm

				// Await separation.
				if(cm->stage == CSM_LEM_STAGE){
					addMessage("SEPARATION");
					setState(MST_C_SEPARATION);
				}else{
					break;
				}
				// FALL INTO
			case MST_C_SEPARATION:
				// Ends with 1ST RDZ PHASING BURN
				// The phasing burn was intended to place the spacecraft 76.5 n mi ahead of the S-IVB in 23 hours.
				switch(SubState){
				case 0: // Must plot phasing burn
					allocPad(4); // Allocate AP7 Maneuver Pad
					if(padForm != NULL){
						// If success
						startSubthread(1); // Start subthread to fill PAD
					}else{
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if(SubStateTime > 10 && padState > -1){
						addMessage("You can has PAD");
						if(padAutoShow == true && padState == 0){ drawPad(); }
						setSubState(2);
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 4 * 60 * 60 + 45 * 60)
					{
						setState(MST_C_COAST1);
						setSubState(0);
					}
					break;
				}
				break;
			case MST_C_COAST1:
				// 6-4 Deorbit Maneuver update to Block Data 2
				switch (SubState) {
				case 0:
					allocPad(4); // Allocate AP7 Maneuver Pad					
					if (padForm != NULL) {
						// If success
						startSubthread(2); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 1 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						// Completed. We really should test for P00 and proceed since that would be visible to the ground.
						addMessage("Ready for uplink?");
						sprintf(PCOption_Text, "Ready for uplink");
						PCOption_Enabled = true;
						setSubState(2);
					}
					break;
				case 2: // Awaiting user response
				case 3: // Negative response / not ready for uplink
					break;				
				case 4: // Ready for uplink
					if (SubStateTime > 1 && padState > -1) {
						// The uplink should also be ready, so flush the uplink buffer to the CMC
						this->CM_uplink_buffer();
						// uplink_size = 0; // Reset
						PCOption_Enabled = false; // No longer needed
						setSubState(5);
					}
					break;
				case 5: // Await uplink completion
					if (cm->pcm.mcc_size == 0) {
						addMessage("Uplink completed!");
						setSubState(6);
					}
					break;
				case 6: // Await burn
					if (cm->GetMissionTime() > 10 * 60 * 60 + 30 * 60)
					{
						setState(MST_C_COAST2);
						setSubState(0);
					}
					break;
				}
				break;
			case MST_C_COAST2: //Block Data 2 to 2nd Phasing Maneuver Update
				switch (SubState) {
				case 0:
					allocPad(1);// Allocate AP7 Block Data Pad
					if (padForm != NULL) {
						// If success
						startSubthread(3); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 14 * 60 * 60 + 16 * 60)
					{
						setState(MST_C_COAST3);
						setSubState(0);
					}
					break;
				}
				break;
			case MST_C_COAST3: // 2nd Phasing Maneuver Update to Block Data 3
				switch (SubState) {
				case 0:
					allocPad(4);// Allocate AP7 Maneuver Pad
					if (padForm != NULL) {
						// If success
						startSubthread(4); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 21 * 60 * 60 + 50 * 60)
					{
						setState(MST_C_COAST4);
					}
					break;
				}
				break;
			case MST_C_COAST4: // Block Data 3 to Preliminary NCC1 Update
				switch (SubState) {
				case 0:
					allocPad(1);// Allocate AP7 Block Data Pad
					if (padForm != NULL) {
						// If success
						startSubthread(5); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 22 * 60 * 60 + 25 * 60)
					{
						setState(MST_C_COAST5);
					}
					break;
				}
				break;
			case MST_C_COAST5: // Preliminary NCC1 Update to Final NCC1 Update
				switch (SubState) {
				case 0:
					allocPad(4);// Allocate AP7 Maneuver Pad
					addMessage("P00 and accept");
					if (padForm != NULL) {
						// If success
						startSubthread(6); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
						// The uplink should also be ready, so flush the uplink buffer to the CMC
						this->CM_uplink_buffer();
						// uplink_size = 0; // Reset
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 25 * 60 * 60 + 30 * 60)
					{
						setState(MST_C_COAST6);
					}
					break;
				}
				break;
			case MST_C_COAST6: // Preliminary NCC1 Update to Final NCC1 Update
				switch (SubState) {
				case 0:
					allocPad(4);// Allocate AP7 Maneuver Pad
					addMessage("P00 and accept");
					if (padForm != NULL) {
						// If success
						startSubthread(7); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
						// The uplink should also be ready, so flush the uplink buffer to the CMC
						this->CM_uplink_buffer();
						// uplink_size = 0; // Reset
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 27 * 60 * 60)
					{
						setState(MST_C_COAST7);
					}
					break;
				}
				break;
			case MST_C_COAST7: // Final NCC1 Update to NCC2 Update
				switch (SubState) {
				case 0:
					allocPad(4);// Allocate AP7 Maneuver Pad
					addMessage("P00 and accept");
					if (padForm != NULL) {
						// If success
						startSubthread(8); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
						// The uplink should also be ready, so flush the uplink buffer to the CMC
						this->CM_uplink_buffer();
						// uplink_size = 0; // Reset
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 27 * 60 * 60 + 32 * 60)
					{
						setState(MST_C_COAST8);
					}
					break;
				}
				break;
			case MST_C_COAST8: // NCC2 Update to NSR Update
				switch (SubState) {
				case 0:
					allocPad(4);// Allocate AP7 Maneuver Pad
					if (padForm != NULL) {
						// If success
						startSubthread(9); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
						// The uplink should also be ready, so flush the uplink buffer to the CMC
						this->CM_uplink_buffer();
						// uplink_size = 0; // Reset
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 28 * 60 * 60 + 50 * 60)
					{
						setState(MST_C_COAST9);
					}
					break;
				}
				break;
			case MST_C_COAST9: // NSR Update to TPI Update
				switch (SubState) {
				case 0:
					allocPad(5);// Allocate AP7 TPI Pad
					if (padForm != NULL) {
						// If success
						startSubthread(10); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 30 * 60 * 60 + 11 * 60)
					{
						setState(MST_C_COAST10);
					}
					break;
				}
				break;
			case MST_C_COAST10: // TPI Update to Final Separation Maneuver update
				switch (SubState) {
				case 0:
					allocPad(4);// Allocate AP7 Maneuver Pad
					if (padForm != NULL) {
						// If success
						startSubthread(11); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
					}
					break;
				case 2: // Await burn
					if (cm->GetMissionTime() > 30 * 60 * 60 + 54 * 60)
					{
						setState(MST_C_COAST11);
					}
					break;
				}
				break;
			case MST_C_COAST11:
				switch (SubState) {
				case 0:
					allocPad(6);
					if (padForm != NULL) {
						// If success
						startSubthread(12); // Start subthread to fill PAD
					}
					else {
						// ERROR STATE
					}
					setSubState(1);
					// FALL INTO
				case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
					if (SubStateTime > 10 && padState > -1) {
						addMessage("You can has PAD");
						if (padAutoShow == true && padState == 0) { drawPad(); }
						setSubState(2);
					}
				case 2: // Await burn
					break;
				}
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

// Push CM uplink sequence
void MCC::pushCMCUplinkString(const char *str) {
	if (str == NULL) { return; } // Bail
	int len = strlen(str);
	int x = 0;
	while (x < len) {
		if (str[x] == '\n' || str[x] == '\r') { x++; continue; } // Ignore CR and LF if they are present (they should not be)
		this->pushCMCUplinkKey(str[x]);
		x++;
	}
}

// Push CM uplink keystroke
void MCC::pushCMCUplinkKey(char key) {
	this->pushUplinkData(043); // VA, SA for CMC
	switch (key) {
	case 'V': // VERB
		// 11-000-101 11-010-001
		this->pushUplinkData(0305);
		this->pushUplinkData(0321);
		break;
	case 'N': // NOUN
		// 11-111-100 00-011-111
		this->pushUplinkData(0374);
		this->pushUplinkData(0037);
		break;
	case 'E': // ENTER
		// 11-110-000 01-111-100
		this->pushUplinkData(0360);
		this->pushUplinkData(0174);
		break;
	case 'R': // RESET
		// 11-001-001 10-110-010
		this->pushUplinkData(0311);
		this->pushUplinkData(0262);
		break;
	case 'C': // CLEAR
		// 11-111-000 00-111-110
		this->pushUplinkData(0370);
		this->pushUplinkData(0076);
		break;
	case 'K': // KEY RELEASE
		// 11-100-100 11-011-001
		this->pushUplinkData(0344);
		this->pushUplinkData(0331);
		break;
	case '+': 
		// 11-101-000 10-111-010
		this->pushUplinkData(0350);
		this->pushUplinkData(0372);
		break;
	case '-': 
		// 11-101-100 10-011-011
		this->pushUplinkData(0354);
		this->pushUplinkData(0233);
		break;
	case '1': 
		// 10-000-111 11-000-001
		this->pushUplinkData(0207);
		this->pushUplinkData(0301);
		break;
	case '2': 
		// 10-001-011 10-100-010
		this->pushUplinkData(0213);
		this->pushUplinkData(0242);
		break;
	case '3': 
		// 10-001-111 10-000-011
		this->pushUplinkData(0217);
		this->pushUplinkData(0203);
		break;
	case '4': 
		// 10-010-011 01-100-100
		this->pushUplinkData(0223);
		this->pushUplinkData(0144);
		break;
	case '5': 
		// 10-010-111 01-000-101
		this->pushUplinkData(0227);
		this->pushUplinkData(0105);
		break;
	case '6': 
		// 10-011-011 00-100-110
		this->pushUplinkData(0233);
		this->pushUplinkData(0046);
		break;
	case '7': 
		// 10-011-111 00-000-111
		this->pushUplinkData(0237);
		this->pushUplinkData(0007);
		break;
	case '8': 
		// 10-100-010 11-101-000
		this->pushUplinkData(0242);
		this->pushUplinkData(0350);
		break;
	case '9': 
		// 10-100-110 11-001-001
		this->pushUplinkData(0246);
		this->pushUplinkData(0311);
		break;
	case '0': 
		// 11-000-001 11-110-000
		this->pushUplinkData(0301);
		this->pushUplinkData(0360);
		break;
	default:
		// Unknown keystroke!
		return;
	}
}

// Push uplink data word on stack
void MCC::pushUplinkData(unsigned char data) {
	uplink_data[uplink_size] = data;
	uplink_size++;
}

// Uplink string to CM
int MCC::CM_uplink(const unsigned char *data, int len) {
	int remsize = 1024;
	remsize -= cm->pcm.mcc_size;
	// if (cm->pcm.mcc_size > 0) { return -1; } // If busy, bail
	if (len > remsize) { return -2; } // Too long!
	memcpy((cm->pcm.mcc_data+cm->pcm.mcc_size), data, len);
	cm->pcm.mcc_size += len;
	return len;
}

// Send uplink buffer to CMC
int MCC::CM_uplink_buffer() {
	int rv = this->CM_uplink(uplink_data, uplink_size);
	if (rv > 0) {
		uplink_size = 0; // Reset
	}
	return(rv);
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
	case 1: // FILL PAD AP7MNV FOR AP7 PHASING BURN
		{
			AP7MNV * form = (AP7MNV *)padForm;
			sprintf(form->purpose,"PHASING BURN");
			// Ask RTCC for numbers
			rtcc->calcParams.src = cm;
			rtcc->calcParams.tgt = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Should be user-programmable later
			rtcc->Calculation(1,padForm);
			// Done filling form, OK to show
			padState = 0;
			// Pretend we did the math
			Result = 0; // Done
		}
		break;
	case 2: //FILL PAD AP7MNV FOR AP7 6-4 CONTINGENCY DEORBIT MANEUVER
		{
		AP7MNV * form = (AP7MNV *)padForm;
		sprintf(form->purpose, "6-4 DEORBIT");
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		// Clobber string
		upString[0] = 0;
		// Do math
		rtcc->Calculation(2, padForm, upString);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
		}
		// Done filling form, OK to show
		padState = 0;
		// Pretend we did the math
		Result = 0; // Done
		}
		break;
	case 3: // FILL PAD AP7BLK FOR AP7 BLOCK DATA
		{
			AP7BLK * form = (AP7BLK *)padForm;
			// Ask RTCC for numbers
			rtcc->calcParams.src = cm;
			rtcc->Calculation(3, padForm);
			// Done filling form, OK to show
			padState = 0;
			// Pretend we did the math
			Result = 0; // Done
		}
		break;
	case 4: // FILL PAD AP7MNV FOR 2ND PHASING MANEUVER
	{
		AP7MNV * form = (AP7MNV *)padForm;
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		rtcc->calcParams.tgt = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Should be user-programmable later
		rtcc->Calculation(4, padForm);
		// Done filling form, OK to show
		padState = 0;
		// Pretend we did the math
		Result = 0; // Done
	}
	break;
	case 5: // FILL PAD AP7BLK FOR AP7 BLOCK DATA
	{
		AP7BLK * form = (AP7BLK *)padForm;
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		rtcc->Calculation(5, padForm);
		// Done filling form, OK to show
		padState = 0;
		// Pretend we did the math
		Result = 0; // Done
	}
	break;
	case 6: // FILL PRELIMINARY PAD AP7MNV FOR NCC1 MANEUVER
	{
		AP7MNV * form = (AP7MNV *)padForm;
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		rtcc->calcParams.tgt = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Should be user-programmable later
		// Clobber string
		upString[0] = 0;
		//Do math
		rtcc->Calculation(6, padForm, upString);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
		}
		// Done filling form, OK to show
		padState = 0;
		// Pretend we did the math
		Result = 0; // Done
	}
	break;
	case 7: // FILL FINAL PAD AP7MNV FOR NCC1 MANEUVER
	{
		AP7MNV * form = (AP7MNV *)padForm;
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		rtcc->calcParams.tgt = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Should be user-programmable later
																							 // Clobber string
		upString[0] = 0;
		//Do math
		rtcc->Calculation(7, padForm, upString);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
		}
		// Done filling form, OK to show
		padState = 0;
		// Pretend we did the math
		Result = 0; // Done
	}
	break;
	case 8: // FILL PAD AP7MNV FOR NCC2 MANEUVER
	{
		AP7MNV * form = (AP7MNV *)padForm;
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		rtcc->calcParams.tgt = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Should be user-programmable later
		// Clobber string
		upString[0] = 0;
		//Do math
		rtcc->Calculation(8, padForm, upString);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
		}
		// Done filling form, OK to show
		padState = 0;
		// Pretend we did the math
		Result = 0; // Done
	}
	break;
	case 9: // FILL PAD AP7MNV FOR NSR MANEUVER
	{
		AP7MNV * form = (AP7MNV *)padForm;
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		rtcc->calcParams.tgt = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Should be user-programmable later
		 // Clobber string
		upString[0] = 0;
		//Do math
		rtcc->Calculation(9, padForm, upString);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
		}
		// Done filling form, OK to show
		padState = 0;
		// Pretend we did the math
		Result = 0; // Done
	}
	break;
	case 10: // FILL PAD AP7TPI FOR AP7 TPI PAD
		{
			AP7TPI * form = (AP7TPI *)padForm;
			// Ask RTCC for numbers
			rtcc->calcParams.src = cm;
			rtcc->calcParams.tgt = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Should be user-programmable later
			rtcc->Calculation(10, padForm);
			// Done filling form, OK to show
			padState = 0;
			// Pretend we did the math
			Result = 0; // Done
		}
		break;
	case 11: //FILL PAD AP7MNV FOR AP7 FINAL SEPARATION PAD
		{
			AP7MNV * form = (AP7MNV *)padForm;
			// Ask RTCC for numbers
			rtcc->calcParams.src = cm;
			rtcc->Calculation(11, padForm);
			// Done filling form, OK to show
			padState = 0;
			// Pretend we did the math
			Result = 0; // Done
		}
		break;
	case 29: //FILL PAD AP7ENT FOR AP7 NOMINAL DEORBIT
		{
			AP7ENT * form = (AP7ENT *)padForm;
			// Ask RTCC for numbers
			rtcc->calcParams.src = cm;
			rtcc->Calculation(29, padForm);
			// Done filling form, OK to show
			padState = 0;
			// Pretend we did the math
			Result = 0; // Done
		}
		break;
	case 30: //GENERIC CSM STATE VECTOR UPDATE
	{
		//padForm = NULL;
		// Ask RTCC for numbers
		rtcc->calcParams.src = cm;
		// Clobber string
		upString[0] = 0;
		//Do math
		rtcc->Calculation(30, padForm, upString);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
		}
		//padState = 0;
		Result = 0; // Done
	}
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

// Save State
void MCC::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, MCC_START_STRING);
	// Booleans
	SAVE_BOOL("MCC_GT_Enabled", GT_Enabled);	
	SAVE_BOOL("MCC_MT_Enabled", MT_Enabled);
	SAVE_BOOL("MCC_padAutoShow", padAutoShow);
	SAVE_BOOL("MCC_PCOption_Enabled", PCOption_Enabled);
	SAVE_BOOL("MCC_NCOption_Enabled", NCOption_Enabled);
	// Integers
	SAVE_INT("MCC_MissionType", MissionType);
	SAVE_INT("MCC_MissionState", MissionState);
	SAVE_INT("MCC_SubState", SubState);
	SAVE_INT("MCC_EarthRev", EarthRev);
	SAVE_INT("MCC_MoonRev", MoonRev);
	SAVE_INT("MCC_AbortMode", AbortMode);
	// Floats
	SAVE_DOUBLE("MCC_StateTime", StateTime);
	SAVE_DOUBLE("MCC_SubStateTime", SubStateTime);
	// Strings
	if (PCOption_Enabled == true) { SAVE_STRING("MCC_PCOption_Text", PCOption_Text); }
	if (NCOption_Enabled == true) { SAVE_STRING("MCC_NCOption_Text", NCOption_Text); }
	// Write PAD here!
	// Write uplink buffer here!
	// Done
	oapiWriteLine(scn, MCC_END_STRING);
}

// Load State
void MCC::LoadState(FILEHANDLE scn) {
	char *line;
	int tmp = 0; // Used in boolean type loader

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, MCC_END_STRING, sizeof(MCC_END_STRING))) {
			break;
		}
		LOAD_BOOL("MCC_GT_Enabled", GT_Enabled);
		LOAD_BOOL("MCC_MT_Enabled", MT_Enabled);
		LOAD_BOOL("MCC_padAutoShow", padAutoShow);
		LOAD_BOOL("MCC_PCOption_Enabled", PCOption_Enabled);		
		LOAD_BOOL("MCC_NCOption_Enabled", NCOption_Enabled);
		LOAD_INT("MCC_MissionType", MissionType);
		LOAD_INT("MCC_MissionState", MissionState);
		LOAD_INT("MCC_SubState", SubState);
		LOAD_INT("MCC_EarthRev", EarthRev);
		LOAD_INT("MCC_MoonRev", MoonRev);
		LOAD_INT("MCC_AbortMode", AbortMode);
		LOAD_DOUBLE("MCC_StateTime", StateTime);
		LOAD_DOUBLE("MCC_SubStateTime", SubStateTime);
		LOAD_STRING("MCC_PCOption_Text", PCOption_Text, 32);
		LOAD_STRING("MCC_NCOption_Text", NCOption_Text, 32);
	}
	return;
}

// PAD Utility: Format time.
void format_time(char *buf, double time) {
	buf[0] = 0; // Clobber
	int hours, minutes, seconds;
	if (time < 0) { return;  } // don't do that
	hours = (int)(time / 3600);
	minutes = (int)((time / 60) - (hours * 60));
	seconds = (int)((time - (hours * 3600)) - (minutes * 60));
	sprintf(buf, "%03d:%02d:%02d", hours, minutes, seconds);
}

// Draw PAD display
void MCC::drawPad(){
	char buffer[512];
	char tmpbuf[32];
	char tmpbuf2[32];
	if(padNumber > 0 && padForm == NULL){
		oapiAnnotationSetText(NHpad,"PAD data lost");
		return;
	}
	switch(padNumber){
	case 0:
		// NO PAD (*knifed*)
		oapiAnnotationSetText(NHpad, "No PAD");
		break;
	case 1: //AP7BLK
		{
			AP7BLK * form = (AP7BLK *)padForm;
			sprintf(buffer, "BLOCK DATA\n");

			for (int i = 0;i < 4;i++)
			{
				format_time(tmpbuf, form->GETI[i]);
				format_time(tmpbuf2, form->GETI[i + 4]);
				sprintf(buffer, "%sXX%s XX%s AREA\nXXX%+05.1f  XXX%+05.1f LAT\nXX%+06.1f  XX%+06.1f LONG\n%s  %s GETI\nXXX%4.1f  XXX%4.1f DVC\n%s       %s WX\n\n", buffer, form->Area[i], form->Area[i + 4], form->Lat[i], form->Lat[i + 4], form->Lng[i], form->Lng[i + 4], tmpbuf, tmpbuf2, form->dVC[i], form->dVC[i + 4], form->Wx[i], form->Wx[i + 4]);
			}
			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case 4: // AP7MNV
		{
			AP7MNV * form = (AP7MNV *)padForm;
			format_time(tmpbuf, form->GETI);
			format_time(tmpbuf2, form->NavChk);
			sprintf(buffer,"MANEUVER PAD\nPURPOSE: %s\nGETI (N33): %s\ndV X: %+07.1f\ndV Y: %+07.1f\ndV Z: %+07.1f\nHA: %+07.1f\nHP: %+07.1f\nVC: %+07.1f\nWGT: %+06.0f\nPTRM: %+07.2f\n YTRM: %+07.2f\nBT: %02.0f\nSXTS: %02d\n SFT: %+07.2f\nTRN: %+07.3f\n%s TLAT, LONG\n%+07.2f LAT\n%+07.2f LONG\n%+07.1f ALT\nXXX%03.0f R\nXXX%03.0f P\nXXX%03.0f Y",form->purpose,tmpbuf,form->dV.x,form->dV.y,form->dV.z, form->HA, form->HP, form->Vc, form->Weight, form->pTrim, form->yTrim, form->burntime, form->Star, form->Shaft, form->Trun, tmpbuf2, form->lat, form->lng, form->alt, form->Att.x, form->Att.y, form->Att.z);			
			oapiAnnotationSetText(NHpad,buffer);
		}
		break;
	case 5: //AP7TPI
		{
			AP7TPI * form = (AP7TPI *)padForm;
			format_time(tmpbuf, form->GETI);
			sprintf(buffer, "TERMINAL PHASE INITIATE\n%s GETI\n%+07.1f Vgx\n%+07.1f Vgy\n%+07.1f Vgz\n", tmpbuf, form->Vg.x, form->Vg.y, form->Vg.z);
			if (form->Backup_dV.x > 0)
			{
				sprintf(buffer, "%sF%04.1f/%02.0f DVX LOS/BT\n", buffer, abs(form->Backup_dV.x), form->Backup_bT.x);
			}
			else
			{
				sprintf(buffer, "%sA%04.1f/%02.0f DVX LOS/BT\n", buffer, abs(form->Backup_dV.x), form->Backup_bT.x);
			}
			if (form->Backup_dV.y > 0)
			{
				sprintf(buffer, "%sR%04.1f/%02.0f DVY LOS/BT\n", buffer, abs(form->Backup_dV.y), form->Backup_bT.y);
			}
			else
			{
				sprintf(buffer, "%sL%04.1f/%02.0f DVY LOS/BT\n", buffer, abs(form->Backup_dV.y), form->Backup_bT.y);
			}
			if (form->Backup_dV.z > 0)
			{
				sprintf(buffer, "%sD%04.1f/%02.0f DVZ LOS/BT\n", buffer, abs(form->Backup_dV.z), form->Backup_bT.z);
			}
			else
			{
				sprintf(buffer, "%sU%04.1f/%02.0f DVZ LOS/BT\n", buffer, abs(form->Backup_dV.z), form->Backup_bT.z);
			}
			sprintf(buffer, "%sX%04.1f/%3.1f\nX%06.2f R\n%+06.1f RDOT AT TPI\n%06.2f EL MINUS 5 MIN\n%06.2f AZ", buffer, form->dH_TPI, form->dH_Max, form->R, form->Rdot, form->EL, form->AZ);

			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case 6: //APTENT
		{
			AP7ENT * form = (AP7ENT *)padForm;
			sprintf(buffer, "ENTRY UPDATE\nPREBURN\nX%s AREA\nXX%+5.1f DV TO\nXXX%3.0f R400K\nXXX%3.0f P400K\nXXX%3.0f Y400K\n%+7.1f RTGO .05G\n%+7.0f VIO .05G\nXX%4.0f RET .05G\n%+07.2f LAT\n%+07.2f LONG", form->Area[0], form->dVTO[0], form->Att400K[0].x, form->Att400K[0].y, form->Att400K[0].z, form->RTGO[0], form->VIO[0], form->Ret05[0], form->Lat[0], form->Lng[0]);
			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	default:
		sprintf(buffer,"Unknown padNumber %d",padNumber);
		oapiAnnotationSetText(NHpad,buffer);
		break;
	}
	padState = 1;
}

// Allocate PAD
void MCC::allocPad(int Number){
	char msg[256];
	// Ensure PAD display off and disabled
	if(padState == 1){ 
		oapiAnnotationSetText(NHpad,""); // Clear PAD		
	}
	padState = -1;
	padNumber = Number;
	// Ensure pointer freed
	if(padForm != NULL){
		free(padForm);
	}
	switch(padNumber){
	case 1: // AP7BLK
		padForm = calloc(1,sizeof(AP7BLK));
		break;
	case 2: // P27PAD
		padForm = calloc(1,sizeof(P27PAD));
		break;
	case 3: // AP7NAV
		padForm = calloc(1,sizeof(AP7NAV));
		break;
	case 4: // AP7MNV
		padForm = calloc(1,sizeof(AP7MNV));
		break;
	case 5: // AP7TPI
		padForm = calloc(1,sizeof(AP7TPI));
		break;
	case 6: // AP7ENT
		padForm = calloc(1,sizeof(AP7ENT));
		break;

	default:
		padForm = NULL;
		return;
	}
	if(padForm == NULL){
		sprintf(msg,"Could not calloc() PAD: %s",strerror(errno));
		addMessage(msg);
	}
	return;
}

// Free PAD
void MCC::freePad(){
	if(padState == 1){ 
		oapiAnnotationSetText(NHpad,""); // Clear PAD		
	}
	padState = -1;
	if(padForm == NULL){ return; }
	free(padForm);
	padForm = NULL; // ensure clobber
}

// Keypress handler
void MCC::keyDown(DWORD key){
	char buf[MAX_MSGSIZE];
	char menubuf[300];
	switch(key){
		case OAPI_KEY_TAB:
			if(menuState == 0){
				// Build buf
				buf[0] = 0;
				if (PCOption_Enabled == true) { sprintf(buf, "2: %s\n", PCOption_Text); }
				if (NCOption_Enabled == true) { sprintf(buf, "%s3: %s\n", buf, NCOption_Text); }
				sprintf(menubuf, "CAPCOM MENU\n1: Voice Check\n%s4: Toggle Auto PAD\n5: Hide/Show PAD\n6: Redisplay Messages\n9: Debug Options", buf);
				oapiAnnotationSetText(NHmenu,menubuf); // Present menu
				// 2: Toggle Ground Trk\n3: Toggle Mission Trk\n
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
			if (menuState == 2) {
				if (GT_Enabled == false) {
					GT_Enabled = true;
					sprintf(buf, "Ground Tracking Enabled");
				}
				else {
					GT_Enabled = false;
					sprintf(buf, "Ground Tracking Disabled");
				}
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_2:
			if (menuState == 1 && PCOption_Enabled == true) {
				// Positive Completion
				SubState += 2;
				SubStateTime = 0;
				addMessage(PCOption_Text);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				if (MT_Enabled == false) {
					MT_Enabled = true; GT_Enabled = true;
					sprintf(buf, "Mission and Ground Tracking Enabled");
				}
				else {
					MT_Enabled = false;
					sprintf(buf, "Mission Tracking Disabled");
				}
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_3:
			if (menuState == 1 && NCOption_Enabled == true) {
				// Negative Completion
				SubState++;
				SubStateTime = 0;
				addMessage(NCOption_Text);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Report State
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_4:
			if(menuState == 1){
				if(padAutoShow == false){
					padAutoShow = true;
					sprintf(buf,"PAD Auto Show Enabled");
				}else{
					padAutoShow = false;
					sprintf(buf,"PAD Auto Show Disabled");
				}
				addMessage(buf);			
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Increment State
				MissionState++;
				SubState = 0;
				StateTime = SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_5:
			if(menuState == 1){
				if(padState == -1 || padNumber == 0){
					addMessage("No PAD available");
				}else{
					if(padState == 1){
						oapiAnnotationSetText(NHpad,""); // Clear PAD
						padState = 0;
					}else{
						drawPad();						
					}
				}
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Decrement State
				MissionState--;
				SubState = 0;
				StateTime = SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_6:
			if(menuState == 1){
				redisplayMessages();
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Increment SubState				
				SubState++; 
				SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_7:
			/*
			if (menuState == 1) {
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			*/
			if (menuState == 2) {
				// Decrement SubState				
				SubState--;
				SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_8:
			/*
			if (menuState == 1) {
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			*/
			if (menuState == 2) {
				// Reset State				
				SubState = 0;
				StateTime = SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_9:
			if (menuState == 2) {
				// Reset SubState				
				SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 1) {
				oapiAnnotationSetText(NHmenu, "DEBUG MENU\n1: Toggle GT\n2: Toggle MT\n3: Report State\n4: Inc State\n5: Dec State\n6: Inc SubState\n7: Dec SubState\n8: Reset State\n9: Reset SubState"); // Debug menu
				menuState = 2;
			}
			break;

	}
}
