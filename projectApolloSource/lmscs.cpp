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
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"
#include "resource.h"
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
}
// GuidContSwitch is the Guidance Control switch

void ATCA::Timestep(double simdt){

	// Which mode are we in?
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
				// Do thruster updation
				LMChannelValue5 ch5;
				LMChannelValue6 ch6;
				ch5.Value = lem->agc.GetOutputChannel(5); // Initialize CH5 data
				ch6.Value = lem->agc.GetOutputChannel(6); // Initialize CH6 data
				lem->agc.SetInputChannelBit(030,10,1);    // LGC HAS CONTROL

				// Set thrusters
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
			}
			// Otherwise do nothing.
			break;

		case TOGGLESWITCH_DOWN:  // AGS
			lem->agc.SetInputChannelBit(030,10,0);  // LGC Doesn't Have Control
			break;

	}
}
