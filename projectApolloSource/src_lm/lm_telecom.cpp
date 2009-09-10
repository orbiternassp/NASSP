/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Telecommunications Implementation

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

  **************************** Revision History ****************************
  *	$Log$
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"
#include "tracer.h"
#include "papi.h"
#include "CollisionSDK/CollisionSDK.h"

#include "connector.h"

// VHF System (and shared stuff)
LM_VHF::LM_VHF(){
	lem = NULL;
}

void LM_VHF::Init(LEM *vessel){
	lem = vessel;
}

void LM_VHF::SystemTimestep(double simdt) {
	if(lem == NULL){ return; } // Do nothing if not initialized
	// VHF XMTR A
	// Draws 30 watts of DC when transmitting and 3.5 watts when not.
	if(lem->COMM_VHF_XMTR_A_CB.Voltage() > 24){
		// For now, we'll just draw idle.
		if(lem->VHFAVoiceSwitch.GetState() != THREEPOSSWITCH_CENTER){
			lem->COMM_VHF_XMTR_A_CB.DrawPower(3.5);
		}
	}
	// VHF RCVR A
	// Draws 1.2 watts of DC when on
	if(lem->COMM_VHF_RCVR_A_CB.Voltage() > 24 && lem->VHFARcvrSwtich.GetState() == TOGGLESWITCH_UP){
		lem->COMM_VHF_RCVR_A_CB.DrawPower(1.2);	
	}
	// VHF XMTR B
	// Draws 28.9 watts of DC when transmitting voice and 31.7 watts when transmitting data.
	// Draws 3.5 watts when not transmitting.
	if(lem->COMM_VHF_XMTR_B_CB.Voltage() > 24){
		// For now, we'll just draw idle or data.
		if(lem->VHFBVoiceSwitch.GetState() == THREEPOSSWITCH_UP){
			lem->COMM_VHF_XMTR_B_CB.DrawPower(3.5); // Voice Mode
		}
		if(lem->VHFBVoiceSwitch.GetState() == THREEPOSSWITCH_DOWN){
			lem->COMM_VHF_XMTR_B_CB.DrawPower(31.7); // Data Mode
		}
	}
	// VHF RCVR B
	// Draws 1.2 watts of DC when on
	if(lem->COMM_VHF_RCVR_B_CB.Voltage() > 24 && lem->VHFBRcvrSwtich.GetState() == TOGGLESWITCH_UP){
		lem->COMM_VHF_RCVR_B_CB.DrawPower(1.2);	
	}
	// CDR and LMP Audio Centers
	if(lem->COMM_CDR_AUDIO_CB.Voltage() > 0){ lem->COMM_CDR_AUDIO_CB.DrawPower(4.8); }
	if(lem->COMM_SE_AUDIO_CB.Voltage() > 0){ lem->COMM_SE_AUDIO_CB.DrawPower(4.8); }
	// PMP
	if(lem->COMM_PMP_CB.Voltage() > 0){ lem->COMM_PMP_CB.DrawPower(4.3); }
	// ERAs
	if(lem->INST_SIG_CONDR_1_CB.Voltage() > 0){ lem->INST_SIG_CONDR_1_CB.DrawPower(16.04); }
	if(lem->INST_SIG_CONDR_2_CB.Voltage() > 0){ lem->INST_SIG_CONDR_2_CB.DrawPower(14.23); }
	// FIXME: Need current drain amount for INST_SIG_SENSOR_CB
	// PCMTEA
	if(lem->INST_PCMTEA_CB.Voltage() > 0){
		lem->INST_PCMTEA_CB.DrawPower(11);
	}
}

void LM_VHF::TimeStep(double simt){
}

// S-Band System
LM_SBAND::LM_SBAND(){
	lem = NULL;
	pa_mode_1 = 0; pa_timer_1 = 0;
	pa_mode_2 = 0; pa_timer_2 = 0;
	tc_mode_1 = 0; tc_timer_1 = 0;
	tc_mode_2 = 0; tc_timer_2 = 0;

}

void LM_SBAND::Init(LEM *vessel){
	lem = vessel;
}

void LM_SBAND::SystemTimestep(double simdt) {
	if(lem == NULL){ return; } // Do nothing if not initialized
	// SBand Transcievers take 30 seconds to warm up when turned on, or 5 seconds to change modulation modes.
	// SBand Power Amplifiers take 60 seconds to warm up when turned on, and 20 seconds to recycle if RF is interrupted.
	// SBand Primary Transciever
	// Pulls 36 watts operating.
	if(lem->SBandXCvrSelSwitch.GetState() == THREEPOSSWITCH_UP){
		if(tc_mode_1 == 0){ tc_mode_1 = 2; } // Start warming up
	}else{
		if(tc_mode_1 > 1){ tc_mode_1 = 1; } // Start cooling off
	}
	// SBand Secondary Transciever
	// Pulls 36 watts operating.
	if(lem->SBandXCvrSelSwitch.GetState() == THREEPOSSWITCH_DOWN){
		if(tc_mode_2 == 0){ tc_mode_2 = 2; } // Start warming up
	}else{
		if(tc_mode_2 > 1){ tc_mode_2 = 1; } // Start cooling off
	}	
	// SBand Primary PA
	// Pulls 72 watts operating
	if(lem->SBandPASelSwitch.GetState() == THREEPOSSWITCH_UP){
		if(pa_mode_1 == 0){ pa_mode_1 = 2; } // Start warming up
	}else{
		if(pa_mode_1 > 1){ pa_mode_1 = 1; } // Start cooling off
	}
	// SBand Secondary PA
	// Pulls 72 watts operating
	if(lem->SBandPASelSwitch.GetState() == THREEPOSSWITCH_DOWN){
		if(pa_mode_2 == 0){ pa_mode_2 = 2; } // Start warming up
	}else{
		if(pa_mode_2 > 1){ pa_mode_2 = 1; } // Start cooling off
	}
}

void LM_SBAND::TimeStep(double simt){
	if(lem == NULL){ return; } // Do nothing if not initialized
	switch(tc_mode_1){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(tc_timer_1 == 0){
				tc_timer_1 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (tc_timer_1+1)){
				// Tubes are cooled, we can stop the timer.
				tc_timer_1 = 0; tc_mode_1 = 0;
			}
			break;
		case 2: // STARTING UP
			if(tc_timer_1 == 0){
				tc_timer_1 = simt; // Initialize
			}
			// Taking 30 seconds, warm up the tubes
			if(lem->COMM_PRIM_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_XCVR_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				tc_timer_1 = simt; break;
			}
			if(simt > (tc_timer_1+30)){
				// Tubes are warm and we're ready to operate.
				tc_mode_1 = 3; tc_timer_1 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_PRIM_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_XCVR_CB.DrawPower(36);
			}else{
				// Power failed - Start cooling
				if(tc_timer_1 == 0){
					tc_timer_1 = simt;
				}
				if(simt > (tc_timer_1+1)){ // After one second, shut down
					tc_mode_1 = 1; break;
				}
			}
			break;
	}
	switch(tc_mode_2){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(tc_timer_2 == 0){
				tc_timer_2 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (tc_timer_2+1)){
				// Tubes are cooled, we can stop the timer.
				tc_timer_2 = 0; tc_mode_2 = 0;
			}
			break;
		case 2: // STARTING UP
			if(tc_timer_2 == 0){
				tc_timer_2 = simt; // Initialize
			}
			// Taking 30 seconds, warm up the tubes
			if(lem->COMM_SEC_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_XCVR_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				tc_timer_2 = simt; break;
			}
			if(simt > (tc_timer_2+30)){
				// Tubes are warm and we're ready to operate.
				tc_mode_2 = 3; tc_timer_2 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_SEC_SBAND_XCVR_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_XCVR_CB.DrawPower(36);
			}else{
				// Power failed - Start cooling
				if(tc_timer_2 == 0){
					tc_timer_2 = simt;
				}
				if(simt > (tc_timer_2+1)){ // After one second, shut down
					tc_mode_2 = 1; break;
				}
			}
			break;
	}
	// PAs
	switch(pa_mode_1){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(pa_timer_1 == 0){
				pa_timer_1 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (pa_timer_1+1)){
				// Tubes are cooled, we can stop the timer.
				pa_timer_1 = 0; pa_mode_1 = 0;
			}
			break;
		case 2: // STARTING UP
			if(pa_timer_1 == 0){
				pa_timer_1 = simt; // Initialize
			}
			// Taking 60 seconds, warm up the tubes
			if(lem->COMM_PRIM_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_PA_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				pa_timer_1 = simt; break;
			}
			if(simt > (pa_timer_1+60)){
				// Tubes are warm and we're ready to operate.
				pa_mode_1 = 3; pa_timer_1 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_PRIM_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_PRIM_SBAND_PA_CB.DrawPower(72);
			}else{
				// Power failed - Start cooling
				if(pa_timer_1 == 0){
					pa_timer_1 = simt;
				}
				if(simt > (pa_timer_1+1)){ // After one second, shut down
					pa_mode_1 = 1; break;
				}
			}
			break;
	}
	switch(pa_mode_2){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(pa_timer_2 == 0){
				pa_timer_2 = simt; // Initialize
			}
			// Wait one second for the tubes to cool
			if(simt > (pa_timer_2+1)){
				// Tubes are cooled, we can stop the timer.
				pa_timer_2 = 0; pa_mode_2 = 0;
			}
			break;
		case 2: // STARTING UP
			if(pa_timer_2 == 0){
				pa_timer_2 = simt; // Initialize
			}
			// Taking 60 seconds, warm up the tubes
			if(lem->COMM_SEC_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_PA_CB.DrawPower(12); // FIXME: I guessed at this number
			}else{
				// Power failed - Start over
				pa_timer_2 = simt; break;
			}
			if(simt > (pa_timer_2+60)){
				// Tubes are warm and we're ready to operate.
				pa_mode_2 = 3; pa_timer_2 = 0;
			}
			break;
		case 3: // OPERATING
			if(lem->COMM_SEC_SBAND_PA_CB.Voltage() > 24){
				lem->COMM_SEC_SBAND_PA_CB.DrawPower(72);
			}else{
				// Power failed - Start cooling
				if(pa_timer_2 == 0){
					pa_timer_2 = simt;
				}
				if(simt > (pa_timer_2+1)){ // After one second, shut down
					pa_mode_2 = 1; break;
				}
			}
			break;
	}
}

// S-Band Steerable Antenna
LEM_SteerableAnt::LEM_SteerableAnt() : antenna("LEM-SBand-Steerable-Antenna",_vector3(0.013, 3.0, 0.03),0.03,0.04),
	antheater("LEM-SBand-Steerable-Antenna-Heater",1,NULL,40,51.7,0,233.15,255,&antenna)
{
	lem = NULL;	
}

void LEM_SteerableAnt::Init(LEM *s){
	lem = s;
	// Set up antenna.
	// SBand antenna 51.7 watts to stay between -40F and 0F
	antenna.isolation = 1.0; 
	antenna.Area = 10783.0112; // Surface area of reflecting dish, probably good enough
	antenna.mass = 10000;      // Probably the same as the RR antenna
	antenna.SetTemp(233); 
	if(lem != NULL){
		antheater.WireTo(&lem->HTR_SBD_ANT_CB);
		lem->Panelsdk.AddHydraulic(&antenna);
		lem->Panelsdk.AddElectrical(&antheater,false);
		antheater.Enable();
		antheater.SetPumpAuto();
	}
}

void LEM_SteerableAnt::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// Draw DC power from COMM_SBAND_ANT_CB to position.
	// Use 7.6 watts to move the antenna and 0.7 watts to maintain auto track.
	// Draw AC power from ???
	// Use 27.9 watts to move the antenna and 4.0 watts to maintain auto track.

	// sprintf(oapiDebugString(),"SBand Antenna Temp: %f AH %f",antenna.Temp,antheater.pumping);
}

void LEM_SteerableAnt::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_SteerableAnt::LoadState(FILEHANDLE scn,char *end_str){

}

double LEM_SteerableAnt::GetAntennaTempF(){

	return(0);
}