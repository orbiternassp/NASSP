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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.2  2009/09/01 06:18:32  dseagrav
  *	LM Checkpoint Commit. Added switches. Added history to LM SCS files. Added bitmap to LM. Added AIDs.
  *	
  *
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK35.h"
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
	int x=0; while(x < 16){ jet_start[x] = 0; jet_stop[x] = 0; x++; }
}
// GuidContSwitch is the Guidance Control switch

void ATCA::Timestep(double simt){
	// Which mode are we in?
	if(lem == NULL){ return; }
	int GC_Mode = lem->GuidContSwitch.GetState();

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

	switch(GC_Mode){
		case TOGGLESWITCH_UP:    // PGNS
			// If this is vAGC mode
			if(lem->agc.Yaagc){
				lem->agc.SetInputChannelBit(030,10,1);    // LGC HAS CONTROL
				/*
				// CH5
				lem->SetRCSJet(12,(ch5.Bits.B4U != 0));
				lem->SetRCSJet(15,(ch5.Bits.A4D != 0));
				lem->SetRCSJet(8,(ch5.Bits.A3U != 0));
				lem->SetRCSJet(11,(ch5.Bits.B3D != 0));
				lem->SetRCSJet(4,(ch5.Bits.B2U != 0));
				lem->SetRCSJet(7,(ch5.Bits.A2D != 0));
				lem->SetRCSJet(0,(ch5.Bits.A1U != 0));
				lem->SetRCSJet(3,(ch5.Bits.B1D != 0));
				// CH6
				lem->SetRCSJet(10,(ch6.Bits.B3A != 0));
				lem->SetRCSJet(13,(ch6.Bits.B4F != 0));
				lem->SetRCSJet(1,(ch6.Bits.A1F != 0));
				lem->SetRCSJet(6,(ch6.Bits.A2A != 0));
				lem->SetRCSJet(5,(ch6.Bits.B2L != 0));
				lem->SetRCSJet(9,(ch6.Bits.A3R != 0));
				lem->SetRCSJet(14,(ch6.Bits.A4R != 0));
				lem->SetRCSJet(2,(ch6.Bits.B1L != 0));
				*/
			}
			// Otherwise do nothing.
			break;

		case TOGGLESWITCH_DOWN:  // AGS
			lem->agc.SetInputChannelBit(030,10,0);  // LGC Doesn't Have Control
			break;
	}
// LM RCS thrusters take 12.5ms to ramp up to full thrust and 17.5ms to ramp back down. There is a dead time of 10ms before thrust starts.
// The upshot of this is that full thrust always starts 20ms after commanded and stops 8ms after commanded. 
// Orbiter has no hope of providing enough resolution to properly map this. It depends on framerate.
// We can do our best though...
	int x=0;
	while(x < 16){
		double power=0;
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
		lem->SetRCSJetLevel(x,power);
		x++;
	}
}


// Process thruster commands from LGC
void ATCA::ProcessLGC(int ch, int val){	
	double now = oapiGetSimTime();
	if(lem->GuidContSwitch.GetState() != TOGGLESWITCH_UP){ val = 0; } // If not in primary mode, force jets off (so jets will switch off at programmed times)
	// When in primary, thruster commands are passed from LGC to jets.
	switch(ch){
		case 05:
			LMChannelValue5 ch5;
			ch5.Value = val;			
			if(ch5.Bits.B4U != 0 && jet_start[12] == 0){ jet_start[12] = now; }else if(ch5.Bits.B4U == 0 && (jet_start[12] > 0 && jet_stop[12] == 0)){ jet_stop[12] = now; }
			if(ch5.Bits.A4D != 0 && jet_start[15] == 0){ jet_start[15] = now; }else if(ch5.Bits.A4D == 0 && (jet_start[15] > 0 && jet_stop[15] == 0)){ jet_stop[15] = now; }
			if(ch5.Bits.A3U != 0 && jet_start[8]  == 0){ jet_start[8]  = now; }else if(ch5.Bits.A3U == 0 && (jet_start[8]  > 0 && jet_stop[8]  == 0)){ jet_stop[8]  = now; }
			if(ch5.Bits.B3D != 0 && jet_start[11] == 0){ jet_start[11] = now; }else if(ch5.Bits.B3D == 0 && (jet_start[11] > 0 && jet_stop[11] == 0)){ jet_stop[11] = now; }
			if(ch5.Bits.B2U != 0 && jet_start[4]  == 0){ jet_start[4]  = now; }else if(ch5.Bits.B2U == 0 && (jet_start[4]  > 0 && jet_stop[4]  == 0)){ jet_stop[4]  = now; }
			if(ch5.Bits.A2D != 0 && jet_start[7]  == 0){ jet_start[7]  = now; }else if(ch5.Bits.A2D == 0 && (jet_start[7]  > 0 && jet_stop[7]  == 0)){ jet_stop[7]  = now; }
			if(ch5.Bits.A1U != 0 && jet_start[0]  == 0){ jet_start[0]  = now; }else if(ch5.Bits.A1U == 0 && (jet_start[0]  > 0 && jet_stop[0]  == 0)){ jet_stop[0]  = now; }
			if(ch5.Bits.B1D != 0 && jet_start[3]  == 0){ jet_start[3]  = now; }else if(ch5.Bits.B1D == 0 && (jet_start[3]  > 0 && jet_stop[3]  == 0)){ jet_stop[3]  = now; }			
			break;
		case 06:
			LMChannelValue6 ch6;
			ch6.Value = val;
			if(ch6.Bits.B3A != 0 && jet_start[10] == 0){ jet_start[10] = now; }else if(ch6.Bits.B3A == 0 && (jet_start[10] > 0 && jet_stop[10] == 0)){ jet_stop[10] = now; }
			if(ch6.Bits.B4F != 0 && jet_start[13] == 0){ jet_start[13] = now; }else if(ch6.Bits.B4F == 0 && (jet_start[13] > 0 && jet_stop[13] == 0)){ jet_stop[13] = now; }
			if(ch6.Bits.A1F != 0 && jet_start[1]  == 0){ jet_start[1]  = now; }else if(ch6.Bits.A1F == 0 && (jet_start[1]  > 0 && jet_stop[1]  == 0)){ jet_stop[1]  = now; }
			if(ch6.Bits.A2A != 0 && jet_start[6]  == 0){ jet_start[6]  = now; }else if(ch6.Bits.A2A == 0 && (jet_start[6]  > 0 && jet_stop[6]  == 0)){ jet_stop[6]  = now; }
			if(ch6.Bits.B2L != 0 && jet_start[5]  == 0){ jet_start[5]  = now; }else if(ch6.Bits.B2L == 0 && (jet_start[5]  > 0 && jet_stop[5]  == 0)){ jet_stop[5]  = now; }
			if(ch6.Bits.A3R != 0 && jet_start[9]  == 0){ jet_start[9]  = now; }else if(ch6.Bits.A3R == 0 && (jet_start[9]  > 0 && jet_stop[9]  == 0)){ jet_stop[9]  = now; }
			if(ch6.Bits.A4R != 0 && jet_start[14] == 0){ jet_start[14] = now; }else if(ch6.Bits.A4R == 0 && (jet_start[14] > 0 && jet_stop[14] == 0)){ jet_stop[14] = now; }
			if(ch6.Bits.B1L != 0 && jet_start[2]  == 0){ jet_start[2]  = now; }else if(ch6.Bits.B1L == 0 && (jet_start[2]  > 0 && jet_stop[2]  == 0)){ jet_stop[2]  = now; }			
			break;
		default:
			sprintf(oapiDebugString(),"ATCA::ProcessLGC: Bad channel %o",ch);
	}
}