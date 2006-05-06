/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  Docking probe

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
  *	Revision 1.4  2006/02/01 18:26:04  tschachim
  *	Pyros and secs logic necessary for retraction.
  *	Automatic retraction if REALISM 0.
  *	
  *	Revision 1.3  2006/01/14 20:58:16  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.2  2006/01/09 17:55:26  tschachim
  *	Connected the dockingprobe to the EPS.
  *	
  *	Revision 1.1  2006/01/05 11:24:56  tschachim
  *	Initial version
  *	
  **************************************************************************/


#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "powersource.h"
#include "dockingprobe.h"
#include "nasspdefs.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"


DockingProbe::DockingProbe(Sound &capturesound, Sound &latchsound, Sound &extendsound, 
						   Sound &undocksound, Sound &dockfailedsound, PanelSDK &p) : 
	                       CaptureSound(capturesound), LatchSound(latchsound), ExtendSound(extendsound), 
						   UndockSound(undocksound), DockFailedSound(dockfailedsound), DCPower(0, p)
{
	Enabled = false;
	Status = DOCKINGPROBE_STATUS_RETRACTED;
	ExtendingRetracting = 0;
	Docked = false;
	FirstTimeStepDone = false;
	UndockNextTimestep = false;
	IgnoreNextDockEvent = false;
	Realism = REALISM_DEFAULT;
}

DockingProbe::~DockingProbe()

{
	// Nothing for now.
}

void DockingProbe::Extend()

{
	if (!Enabled) return;
	if (!IsPowered()) return;

	ExtendingRetracting = 1;
	if (Status != DOCKINGPROBE_STATUS_EXTENDED) {
		ProbeExtended = true;
		if (Docked) {
			OurVessel->Undock(0);
			UndockSound.play();
		
		} else {
			ExtendSound.play(); 
		}
	}
}

void DockingProbe::Retract()

{
	if (!Enabled) return;
	if (!IsPowered()) return;

	// sequencial logic and pyros have to powered for retraction
	if (!OurVessel) return;
	if (!OurVessel->SECSLogicActive()) return;
	if (!OurVessel->PyrosArmed()) return;

	ExtendingRetracting = -1;
	if (Status != DOCKINGPROBE_STATUS_RETRACTED) {
		ProbeExtended = false;
		if (Docked) {
			LatchSound.play();
		} else {
			// Use the same sound for retracting until we have proper sounds 
			ExtendSound.play(); 
		}
	}	
}

void DockingProbe::DockEvent(int dock, OBJHANDLE connected) 

{
	if (!FirstTimeStepDone) return;
	
	if (IgnoreNextDockEvent) {
		Docked = (connected != NULL);
		IgnoreNextDockEvent = false;
		return;
	}

	if (connected == NULL) {
		Docked = false;
	} else {
		Docked = true;
		if (!Enabled || !IsPowered() || Status != DOCKINGPROBE_STATUS_EXTENDED) {
			DockFailedSound .play(NOLOOP, 200);
			UndockNextTimestep = true;

		} else {
			Status = 0.8;
			CaptureSound.play();

			// Retract automatically if REALISM 0
			if (!Realism) {
				Retract();
			}
		}
	}
}

void DockingProbe::TimeStep(double simt, double simdt)

{
	if (!FirstTimeStepDone) {
		DoFirstTimeStep();
		FirstTimeStepDone = true;
		return;
	}

	if (UndockNextTimestep) {
		OurVessel->Undock(0);
		UndockNextTimestep = false;
	}

	if (ExtendingRetracting > 0) {
		if (Status >= DOCKINGPROBE_STATUS_EXTENDED) {
			Status = DOCKINGPROBE_STATUS_EXTENDED;
			ExtendingRetracting = 0;
		
		} else {
			Status += 0.33 * simdt;
		}
		DCPower.DrawPower(100.0);	// The real power consumption is unknown yet

	} else if (ExtendingRetracting < 0) {
		if (Status <= DOCKINGPROBE_STATUS_RETRACTED) {
			Status = DOCKINGPROBE_STATUS_RETRACTED;
			ExtendingRetracting = 0;
		
		} else {
			Status -= 0.33 * simdt;
		}	
		DCPower.DrawPower(100.0);	// The real power consumption is unknown yet
	}
}

void DockingProbe::DoFirstTimeStep() 

{
	Docked = false;

	DOCKHANDLE d = OurVessel->GetDockHandle(0);
	if (d) {
		if (OurVessel->GetDockStatus(d) != NULL) {
			Docked = true;
		}
	}
}

void DockingProbe::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, DOCKINGPROBE_START_STRING);
	oapiWriteScenario_int (scn, "ENABLED", Enabled);
	oapiWriteScenario_float (scn, "STATUS", Status);
	oapiWriteScenario_int (scn, "EXTENDINGRETRACTING", ExtendingRetracting);
	oapiWriteLine(scn, DOCKINGPROBE_END_STRING);
}


void DockingProbe::LoadState(FILEHANDLE scn)

{
	char *line;
	int i;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, DOCKINGPROBE_END_STRING, sizeof(DOCKINGPROBE_END_STRING)))
			return;
		else if (!strnicmp (line, "ENABLED", 7)) {
			sscanf (line+7, "%d", &i);
			Enabled = (i != 0);
		}
		else if (!strnicmp (line, "STATUS", 6)) {
			sscanf (line+6, "%lf", &Status);
		}
		else if (!strnicmp (line, "EXTENDINGRETRACTING", 19)) {
			sscanf (line+19, "%d", &ExtendingRetracting);
		}
	}
}
