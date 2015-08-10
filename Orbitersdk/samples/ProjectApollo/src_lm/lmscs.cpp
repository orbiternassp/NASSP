/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Stabilization & Control System (and associated parts)

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
#include "soundlib.h"
#include "lmresource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "LEMcomputer.h"
#include "IMU.h"
// #include "saturn.h"
#include "lm_channels.h"
#include "tracer.h"
#include "LEM.h"

// ATTITUDE & TRANSLATION CONTROL ASSEMBLY
ATCA::ATCA(){
	lem = NULL;
}

void ATCA::Init(LEM *vessel){
	lem = vessel;
	int x=0; while(x < 16){ jet_request[x] = 0; jet_last_request[x] = 0; jet_start[x] = 0; jet_stop[x] = 0; x++; }
}
// GuidContSwitch is the Guidance Control switch

void ATCA::Timestep(double simt){
	double now = oapiGetSimTime(); // Get time
	int haspower = 0,hasdriver = 0,balcpl = 0;
	if(lem == NULL){ return; }
	// Fetch mode switch setting.
	int GC_Mode = lem->GuidContSwitch.GetState();
	// Determine ATCA power situation.
	if(lem->SCS_ATCA_CB.Voltage() > 24){
		// ATCA primary power is on.
		haspower = 1;
	}

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

	// *** Determine jet request source path ***
	switch(GC_Mode){
		case TOGGLESWITCH_UP:    // PGNS MODE
			// In this case, thruster demand is direct from the LGC. We have nothing to do.
			if(lem->agc.Yaagc){	lem->agc.SetInputChannelBit(030,10,1); } // Tell the LGC it has control.
			if(haspower == 1 && lem->CDR_SCS_ATCA_CB.Voltage() < 24){ haspower = 0; } // PNGS path requires this.
			if(lem->ModeControlPNGSSwitch.GetState() != THREEPOSSWITCH_DOWN){ hasdriver = 1; } // Drivers disabled when mode control off
			break;

		case TOGGLESWITCH_DOWN:  // ABORT MODE
			// In this case, we have to generate thruster demand ourselves, taking "suggestions" from the AGS.
			// FIXME: Implement this.
			if(lem->agc.Yaagc){	lem->agc.SetInputChannelBit(030,10,0); } // Tell the LGC it doesn't have control
			if(haspower == 1 && lem->SCS_ATCA_AGS_CB.Voltage() < 24){ haspower = 0; } // AGS path requires this.
			if(lem->ModeControlAGSSwitch.GetState() != THREEPOSSWITCH_DOWN){ hasdriver = 1; } // Drivers disabled when mode control off
			break;
	}
	// *** Test "Balanced Couples" switch ***
	if(lem->BALCPLSwitch.GetState() == TOGGLESWITCH_UP){ balcpl = 1; }

	// *** THRUSTER MAINTENANCE ***
	// LM RCS thrusters take 12.5ms to ramp up to full thrust and 17.5ms to ramp back down. There is a dead time of 10ms before thrust starts.
	// The upshot of this is that full thrust always starts 20ms after commanded and stops 8ms after commanded. 
	// Orbiter has no hope of providing enough resolution to properly map this. It depends on framerate.
	// We can do our best though...
	int x=0;
	while(x < 16){
		double power=0;
		// If the ATCA is not powered or driver voltage is absent, it won't work.
		if(haspower != 1 || hasdriver != 1){ jet_request[x] = 0; }
		// If the "Balanced Couples" switch is off, the abort preamps for the four upward-firing thrusters are disabled.
		if(GC_Mode == TOGGLESWITCH_DOWN && balcpl != 1 && (x == 0 || x == 4 || x == 8 || x == 12)){ jet_request[x] = 0;	}
		// Process jet request list to generate start and stop times.
		if(jet_request[x] == 1 && jet_last_request[x] == 0){
			// New fire request
			jet_start[x] = now;
		}else if(jet_request[x] == 0 && jet_last_request[x] == 1){
			// New stop request
			jet_stop[x] = now;
		}
		jet_last_request[x] = jet_request[x]; // Keep track of changes

		if(jet_start[x] == 0 && jet_stop[x] == 0){ x++; continue; } // Done
		// sprintf(oapiDebugString(),"Jet %d fire %f stop %f",x,jet_start[x],jet_stop[x]); 
		if(simt > jet_start[x]+0.01 && simt < jet_start[x]+0.0125){
			// Ramp up
			power = ((simt-jet_start[x])/0.0125);			
		}
		if(jet_stop[x] > 0 && (simt > jet_stop[x]+0.0075 && simt < jet_stop[x]+0.0175)){
		    // Ramp down
			power = 1-((simt-jet_stop[x])/0.0175);
		}
		if(jet_stop[x] > 0 && simt > jet_stop[x]+0.0175){
			// Thruster off
			power=0; jet_start[x] = 0; jet_stop[x] = 0;
		}else{
			if(simt > jet_start[x]+0.0125){
				// Full Power
				power=1;
			}
		}
		// FIXME: This is just for testing.
		// if(power > 0.25){ power = 0.25; }
		if (power >0) 
			sprintf(oapiDebugString(), "FIRE JET");
		lem->SetRCSJetLevelPrimary(x,power);
		x++;
	}
}


// Process thruster commands from LGC
void ATCA::ProcessLGC(int ch, int val){		
	if(lem->GuidContSwitch.GetState() != TOGGLESWITCH_UP){ val = 0; } // If not in primary mode, force jets off (so jets will switch off at programmed times)
	// When in primary, thruster commands are passed from LGC to jets.
	switch(ch){
		case 05:
			LMChannelValue5 ch5;
			ch5.Value = val;	
			if(val>0)
			     sprintf(oapiDebugString(),"CHAN 5 Jet"); 
			if(ch5.Bits.B4U != 0){ jet_request[12] = 1; }else{ jet_request[12] = 0; }
			if(ch5.Bits.A4D != 0){ jet_request[15] = 1; }else{ jet_request[15] = 0; }
			if(ch5.Bits.A3U != 0){ jet_request[8]  = 1; }else{ jet_request[8]  = 0; }
			if(ch5.Bits.B3D != 0){ jet_request[11] = 1; }else{ jet_request[11] = 0; }
			if(ch5.Bits.B2U != 0){ jet_request[4]  = 1; }else{ jet_request[4]  = 0; }
			if(ch5.Bits.A2D != 0){ jet_request[7]  = 1; }else{ jet_request[7]  = 0; }
			if(ch5.Bits.A1U != 0){ jet_request[0]  = 1; }else{ jet_request[0]  = 0; }
			if(ch5.Bits.B1D != 0){ jet_request[3]  = 1; }else{ jet_request[3]  = 0; }			
			break;
		case 06:
			LMChannelValue6 ch6;
			ch6.Value = val;
			if(val>0)
			    sprintf(oapiDebugString(), "CHAN 6 Jet");
			if(ch6.Bits.B3A != 0){ jet_request[10] = 1; }else{ jet_request[10] = 0; }
			if(ch6.Bits.B4F != 0){ jet_request[13] = 1; }else{ jet_request[13] = 0; }
			if(ch6.Bits.A1F != 0){ jet_request[1]  = 1; }else{ jet_request[1]  = 0; }
			if(ch6.Bits.A2A != 0){ jet_request[6]  = 1; }else{ jet_request[6]  = 0; }
			if(ch6.Bits.B2L != 0){ jet_request[5]  = 1; }else{ jet_request[5]  = 0; }
			if(ch6.Bits.A3R != 0){ jet_request[9]  = 1; }else{ jet_request[9]  = 0; }
			if(ch6.Bits.A4R != 0){ jet_request[14] = 1; }else{ jet_request[14] = 0; }
			if(ch6.Bits.B1L != 0){ jet_request[2]  = 1; }else{ jet_request[2]  = 0; }			
			break;
		default:
			sprintf(oapiDebugString(),"ATCA::ProcessLGC: Bad channel %o",ch);
	}
}
