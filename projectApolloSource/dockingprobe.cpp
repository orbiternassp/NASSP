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
  *	Revision 1.15  2008/04/11 11:49:33  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.14  2008/01/22 05:22:27  movieman523
  *	Added port number to docking probe.
  *	
  *	Revision 1.13  2008/01/14 01:17:05  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.12  2007/08/25 00:27:01  jasonims
  *	*** empty log message ***
  *	
  *	Revision 1.11  2007/08/16 07:33:29  jasonims
  *	Created a header file named orbvmath, within it are some basic vector math functions using Orbiter's VECTOR3 and MATRIX3 types.  Has not been entirely debugged, so refrain form additional usage until I get it all checked out.
  *	
  *	Revision 1.10  2007/06/06 15:02:11  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.9  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.8  2007/02/06 18:30:17  tschachim
  *	Bugfixes docking probe, CSM/LM separation. The ASTP stuff still needs fixing though.
  *	
  *	Revision 1.7  2006/07/28 02:06:57  movieman523
  *	Now need to hard dock to get the connectors connected.
  *	
  *	Revision 1.6  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.5  2006/05/06 06:00:35  jasonims
  *	No more venting our Astronauts into space...and no more LRV popping out of an Astronauts pocket....well sorta.
  *	
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


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"

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
#include "lvimu.h"

#include "saturn.h"
#include "papi.h"


DockingProbe::DockingProbe(int port, Sound &capturesound, Sound &latchsound, Sound &extendsound, 
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
	IgnoreNextDockEvent = 0;
	Realism = REALISM_DEFAULT;
	ourPort = port;
	Dockproc = 0;
	Dockparam[0] = Dockparam[1] = Dockparam[2] = _V(0, 0, 0);
}

DockingProbe::~DockingProbe()

{
	// Nothing for now.
}

void DockingProbe::Extend()

{
	if (!Enabled) return;

	if (Status != DOCKINGPROBE_STATUS_EXTENDED) {
		ExtendingRetracting = 1;
		if (Docked) {
			Dockproc = 0;
			OurVessel->Undock(ourPort);
			UndockSound.play();
		
		} else {
			ExtendSound.play(); 
		}
	}
}

bool DockingProbe::IsHardDocked()

{
	return (Docked && (Status == DOCKINGPROBE_STATUS_RETRACTED));
}

void DockingProbe::Retract()

{
	if (!Enabled) return;

	if (Status != DOCKINGPROBE_STATUS_RETRACTED) {
		ExtendingRetracting = -1;
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
	if (Dockproc != 0) return;

	if (!FirstTimeStepDone) return;
	
	if (IgnoreNextDockEvent > 0) {
		Docked = (connected != NULL);
		IgnoreNextDockEvent--;
		return;
	}

	if (connected == NULL) {
		Docked = false;
 		DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
		OurVessel->SetDockParams(dock, Dockparam[0], Dockparam[1], Dockparam[2]);
	} else {
		Docked = true;
		if (!Enabled || Status != DOCKINGPROBE_STATUS_EXTENDED) {
			DockFailedSound.play(NOLOOP, 200);
			UndockNextTimestep = true;
		} else {
			Status = 0.9;
			CaptureSound.play();
			
			Dockproc = 1;
 			DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
			OurVessel->GetDockParams(dock, Dockparam[0], Dockparam[1], Dockparam[2]);

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
		OurVessel->Undock(ourPort);
		UndockNextTimestep = false;
	}

	if (ExtendingRetracting > 0) {
		if (Status >= DOCKINGPROBE_STATUS_EXTENDED) {
			Status = DOCKINGPROBE_STATUS_EXTENDED;
			ExtendingRetracting = 0;
			Dockproc = 0;
			OurVessel->Undocking(ourPort);
			OurVessel->SetDockingProbeMesh();
		} else {
			Status += 0.33 * simdt;
		}
	} else if (ExtendingRetracting < 0) {
		if (Status <= DOCKINGPROBE_STATUS_RETRACTED) {
			Status = DOCKINGPROBE_STATUS_RETRACTED;
			ExtendingRetracting = 0;
			OurVessel->HaveHardDocked(ourPort);		
			OurVessel->SetDockingProbeMesh();
		} else {
			Status -= 0.33 * simdt;
		}	
	}

	if (Dockproc == 1) {
		UpdatePort(Dockparam[1] * 0.5, simdt);
		Dockproc = 2;
	} else if (Dockproc == 2) {
		if (Status > DOCKINGPROBE_STATUS_RETRACTED) {
			UpdatePort(Dockparam[1] * 0.5 * Status / 0.9, simdt);
		} else {
			UpdatePort(_V(0,0,0), simdt);
			Dockproc = 0;
		}
	}
	//sprintf(oapiDebugString(), "Docked %d Status %.3f Dockproc %d  ExtendingRetracting %d", (Docked ? 1 : 0), Status, Dockproc, ExtendingRetracting); 

	// Switching logic
	if (OurVessel->DockingProbeExtdRelSwitch.IsUp() && IsPowered()) {
		Extend();

	} else if (OurVessel->DockingProbeExtdRelSwitch.IsDown()) {
		if ((!OurVessel->DockingProbeRetractPrimSwitch.IsCenter() && OurVessel->DockProbeMnACircuitBraker.IsPowered() && OurVessel->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) ||
			(!OurVessel->DockingProbeRetractSecSwitch.IsCenter()  && OurVessel->DockProbeMnBCircuitBraker.IsPowered() && OurVessel->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE)) {
			/// \todo Each retraction system (Prim1, Prim2, Sec1, Sec2) can only be used once 
			Retract();
		}
	}
}

void DockingProbe::UpdatePort(VECTOR3 off,double simdt)
{
 	DOCKHANDLE dock = OurVessel->GetDockHandle(ourPort);
	OBJHANDLE v = OurVessel->GetDockStatus(dock);
	bool b = OurVessel->Undock(ourPort); 
	OurVessel->SetDockParams(dock, Dockparam[0] + off, Dockparam[1], Dockparam[2]);
	if (v != NULL) OurVessel->Dock(v, ourPort, 0, 1);	///\todo Port of the docked vessel is assumed 0
}  


void DockingProbe::SystemTimestep(double simdt) 

{
	if (ExtendingRetracting) {
		DCPower.DrawPower(100.0);	// The real power consumption is unknown yet
	}
}

void DockingProbe::DoFirstTimeStep() 

{
	Docked = false;

	DOCKHANDLE d = OurVessel->GetDockHandle(ourPort);
	if (d) {
		if (OurVessel->GetDockStatus(d) != NULL) {
			Docked = true;
		}
	}

	if (IsHardDocked())
	{
		OurVessel->HaveHardDocked(ourPort);
	}
	else
	{
		OurVessel->Undocking(ourPort);
	}
}

void DockingProbe::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, DOCKINGPROBE_START_STRING);
	oapiWriteScenario_int(scn, "ENABLED", Enabled);
	oapiWriteScenario_float(scn, "STATUS", Status);
	oapiWriteScenario_int(scn, "EXTENDINGRETRACTING", ExtendingRetracting);
	oapiWriteScenario_int(scn, "DOCKPROC", Dockproc);
	papiWriteScenario_vec(scn, "DOCKPARAM0", Dockparam[0]);
	papiWriteScenario_vec(scn, "DOCKPARAM1", Dockparam[1]);
	papiWriteScenario_vec(scn, "DOCKPARAM2", Dockparam[2]);
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
		else if (papiReadScenario_int(line, "DOCKPROC", Dockproc));
		else if (papiReadScenario_vec(line, "DOCKPARAM0", Dockparam[0]));
		else if (papiReadScenario_vec(line, "DOCKPARAM1", Dockparam[1]));
		else papiReadScenario_vec(line, "DOCKPARAM2", Dockparam[2]);
	}
}
