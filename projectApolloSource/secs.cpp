/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Sequential Events Controller simulation.

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
  *	Revision 1.8  2007/07/17 14:33:10  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.7  2007/06/06 15:02:20  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.6  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.5  2006/08/11 21:16:22  movieman523
  *	Dummy ELS implementation.
  *	
  *	Revision 1.4  2006/08/11 19:34:47  movieman523
  *	Added code to take the docking probe with the LES on a post-abort jettison.
  *	
  *	Revision 1.3  2006/08/11 18:44:56  movieman523
  *	Beginnings of SECS implementation.
  *	
  *	Revision 1.2  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.1  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
  **************************** Revision History ****************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"

#include "ioChannels.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"

#include "connector.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"
#include "FloatBag.h"
#include "secs.h"


SECS::SECS()

{
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	OurVessel = 0;
}

SECS::~SECS()

{
}

void SECS::ControlVessel(Saturn *v)

{
	OurVessel = v;
}

void SECS::Timestep(double simt, double simdt)

{
	//
	// Nothing to do at this moment.
	//

	if (!OurVessel || !IsPowered())
		return;
}

bool SECS::IsPowered()

{
	return Voltage() > SP_MIN_DCVOLTAGE;
}

void SECS::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, SECS_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	oapiWriteScenario_float(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	oapiWriteScenario_float(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);

	oapiWriteLine(scn, SECS_END_STRING);
}

void SECS::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SECS_END_STRING, sizeof(SECS_END_STRING)))
			return;

		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%d", &State);
		}
		else if (!strnicmp (line, "NEXTMISSIONEVENTTIME", 20)) {
			sscanf(line + 20, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp (line, "LASTMISSIONEVENTTIME", 20)) {
			sscanf(line + 20, "%f", &flt);
			LastMissionEventTime = flt;
		}
	}
}


ELS::ELS()

{
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	FloatBag1Size = 0;
	FloatBag2Size = 0;
	FloatBag3Size = 0;

	OurVessel = 0;
	FloatBagVessel = 0;

	DyeMarkerLevel = 0;
	DyeMarkerTime = 3600; // 1 hour
}

ELS::~ELS()

{
}

void ELS::ControlVessel(Saturn *v)

{
	OurVessel = v;
}

void ELS::Timestep(double simt, double simdt)

{
	if (!OurVessel)	return;

	//
	// Float Bags
	//

	if (!OurVessel->ApexCoverAttached) {
		VESSELSTATUS vs;
		OurVessel->GetStatus(vs);

		if (!FloatBagVessel) {
			char VName[256]="";
			OurVessel->GetApolloName(VName);
			strcat(VName, "-FLOATBAG");
			OBJHANDLE hFloatBag = oapiGetVesselByName(VName);

			// Create the float bag vessel
			if (!hFloatBag) {
				OBJHANDLE hFloatBag = oapiCreateVessel(VName, "ProjectApollo/FloatBag", vs);
				FloatBagVessel = (FloatBag *) oapiGetVesselInterface(hFloatBag);
				// Attach it
				ATTACHMENTHANDLE ah = OurVessel->GetAttachmentHandle(false, 0);
				ATTACHMENTHANDLE ahc = FloatBagVessel->GetAttachmentHandle(true, 0);
				OurVessel->AttachChild(hFloatBag, ah, ahc);
			} else {
				FloatBagVessel = (FloatBag *) oapiGetVesselInterface(hFloatBag);
			}
		}

		// Float Bag sizes
		FloatBag1Size = NewFloatBagSize(FloatBag1Size, &OurVessel->FloatBagSwitch1, &OurVessel->FloatBag1BatACircuitBraker, simdt);
		FloatBag2Size = NewFloatBagSize(FloatBag2Size, &OurVessel->FloatBagSwitch2, &OurVessel->FloatBag2BatBCircuitBraker, simdt);
		FloatBag3Size = NewFloatBagSize(FloatBag3Size, &OurVessel->FloatBagSwitch3, &OurVessel->FloatBag3FLTPLCircuitBraker, simdt);
		FloatBagVessel->SetBagSize(1, FloatBag1Size);
		FloatBagVessel->SetBagSize(2, FloatBag2Size);
		FloatBagVessel->SetBagSize(3, FloatBag3Size);

		// Beacon
		if (OurVessel->GetStage() >= CM_ENTRY_STAGE_SIX) {
			// Extend beacon automatically
			FloatBagVessel->ExtendBeacon();
			// Control the light
			if (OurVessel->FloatBag3FLTPLCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || OurVessel->PostLandingBCNLTSwitch.IsCenter()) {
				FloatBagVessel->SetBeaconLight(false, false);
			} else if (OurVessel->PostLandingBCNLTSwitch.IsDown()) {
				FloatBagVessel->SetBeaconLight(true, false);
			} else {
				FloatBagVessel->SetBeaconLight(true, true);
			}
		}

		// Dye marker
		if (OurVessel->GetStage() >= CM_ENTRY_STAGE_SEVEN && DyeMarkerTime > 0) {
			// Turned on?			
			if (OurVessel->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && OurVessel->PostLandingDYEMarkerSwitch.IsUp()) { 
				DyeMarkerLevel = 1;
				DyeMarkerTime -= simdt;
			} else {
				DyeMarkerLevel = 0;
			}
		} else {
			DyeMarkerLevel = 0;
		}
	}
}

double ELS::NewFloatBagSize(double size, ThreePosSwitch *sw, CircuitBrakerSwitch *cb, double simdt) 

{
	if (cb->Voltage() > SP_MIN_DCVOLTAGE) {
		if (sw->IsDown()) {
			size -= simdt / (7. * 60.) * (OurVessel->Realism ? 1. : 20.);	// same as fill? Quickstart mode is faster
			size = max(0, size);
		} else if (sw->IsUp()) {		/// \todo Compressor power, panel 298
			size += simdt / (7. * 60.) * (OurVessel->Realism ? 1. : 20.);	// Apollo 15 entry checklist
			size = min(1, size);
		}
	}
	return size;
}

void ELS::SystemTimestep(double simdt) 

{
	// Float bag compressor 1
	if (OurVessel->FloatBag1BatACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && OurVessel->FloatBagSwitch1.IsUp()) {
		OurVessel->BatteryBusA.DrawPower(424); // Systems handbook
	}

	// Float bag compressor 2 
	if ((OurVessel->FloatBag2BatBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && OurVessel->FloatBagSwitch2.IsUp()) ||
		(OurVessel->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && OurVessel->FloatBagSwitch3.IsUp())) {
		OurVessel->BatteryBusB.DrawPower(424); // Systems handbook
	}

	// Beacon light
	if (OurVessel->GetStage() >= CM_ENTRY_STAGE_SIX && OurVessel->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) {
		// LO/HI
		if (OurVessel->PostLandingBCNLTSwitch.IsDown()) {
			OurVessel->FloatBag3FLTPLCircuitBraker.DrawPower(10);	// guessed
		} else if (OurVessel->PostLandingBCNLTSwitch.IsUp()) {
			OurVessel->FloatBag3FLTPLCircuitBraker.DrawPower(40);	// guessed
		}
	}

	// Dye marker
	if (DyeMarkerLevel == 1) {
		OurVessel->FloatBag3FLTPLCircuitBraker.DrawPower(10);	// guessed
	}
}

void ELS::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, ELS_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	oapiWriteScenario_float(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	oapiWriteScenario_float(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);
	oapiWriteScenario_float(scn, "FLOATBAG1SIZE", FloatBag1Size);
	oapiWriteScenario_float(scn, "FLOATBAG2SIZE", FloatBag2Size);
	oapiWriteScenario_float(scn, "FLOATBAG3SIZE", FloatBag3Size);
	oapiWriteScenario_float(scn, "DYEMARKERLEVEL", DyeMarkerLevel);
	oapiWriteScenario_float(scn, "DYEMARKERTIME", DyeMarkerTime);

	oapiWriteLine(scn, ELS_END_STRING);
}

void ELS::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, ELS_END_STRING, sizeof(ELS_END_STRING)))
			return;

		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%d", &State);
		}
		else if (!strnicmp (line, "NEXTMISSIONEVENTTIME", 20)) {
			sscanf(line + 20, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp (line, "LASTMISSIONEVENTTIME", 20)) {
			sscanf(line + 20, "%f", &flt);
			LastMissionEventTime = flt;
		}
		else if (!strnicmp (line, "FLOATBAG1SIZE", 13)) {
			sscanf (line+13, "%lf", &FloatBag1Size);
		}
		else if (!strnicmp (line, "FLOATBAG2SIZE", 13)) {
			sscanf (line+13, "%lf", &FloatBag2Size);
		}
		else if (!strnicmp (line, "FLOATBAG3SIZE", 13)) {
			sscanf (line+13, "%lf", &FloatBag3Size);
		}
		else if (!strnicmp (line, "DYEMARKERLEVEL", 14)) {
			sscanf (line+14, "%lf", &DyeMarkerLevel);
		}
		else if (!strnicmp (line, "DYEMARKERTIME", 13)) {
			sscanf (line+13, "%lf", &DyeMarkerTime);
		}
	}
}
