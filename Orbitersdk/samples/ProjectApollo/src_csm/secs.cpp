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
  *	Revision 1.2  2009/12/17 17:47:18  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.1  2009/02/18 23:20:56  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.11  2008/07/13 17:47:13  tschachim
  *	Rearranged realism levels, merged Standard and Quickstart Mode.
  *	
  *	Revision 1.10  2008/04/11 12:19:20  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.9  2007/10/18 00:23:24  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
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
#include "papi.h"


SECS::SECS()

{
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	PyroBusAMotor = false;
	PyroBusBMotor = false;

	Sat = 0;
}

SECS::~SECS()

{
}

void SECS::ControlVessel(Saturn *v)

{
	Sat = v;
}

void SECS::Timestep(double simt, double simdt)

{
	if (!Sat) return;

	// See AOH Figure 2.9-20 and Systems Handbook

	//
	// SECS Logic Buses
	//

	bool switchOn = (Sat->SECSLogic1Switch.IsUp() || Sat->SECSLogic2Switch.IsUp());
	if (switchOn && Sat->SECSArmBatACircuitBraker.IsPowered()) {
		Sat->SECSLogicBusA.WireTo(&Sat->SECSLogicBatACircuitBraker);
	} else {
		Sat->SECSLogicBusA.Disconnect();
	}
	if (switchOn && Sat->SECSArmBatBCircuitBraker.IsPowered()) {
		Sat->SECSLogicBusB.WireTo(&Sat->SECSLogicBatBCircuitBraker);
	} else {
		Sat->SECSLogicBusB.Disconnect();
	}

	//
	// Pyro Bus Motors
	//

	if (Sat->SECSArmBatACircuitBraker.IsPowered()) {
		if (Sat->PyroArmASwitch.IsUp()) {
			PyroBusAMotor = true;
			Sat->PyroBusA.WireTo(&Sat->PyroBusAFeeder);
		} else {
			PyroBusAMotor = false;
			Sat->PyroBusA.Disconnect();
		}
	}

	if (Sat->SECSArmBatBCircuitBraker.IsPowered()) {
		if (Sat->PyroArmBSwitch.IsUp()) {
			PyroBusBMotor = true;
			Sat->PyroBusB.WireTo(&Sat->PyroBusBFeeder);
		} else {
			PyroBusBMotor = false;
			Sat->PyroBusB.Disconnect();
		}
	}

	//
	// CSM LV separation relays
	//

	bool pyroA = false, pyroB = false;

	if (Sat->CsmLvSepSwitch.IsUp()) {
		if (IsLogicPoweredAndArmedA()) {
			// Blow Pyro A
			pyroA = true;

			// Activate Auto RCS Enable Relay A
			Sat->rjec.SetAutoRCSEnableRelayA(true);
		}
		if (IsLogicPoweredAndArmedB()) {
			// Blow Pyro B
			pyroB = true;

			// Activate Auto RCS Enable Relay B
			Sat->rjec.SetAutoRCSEnableRelayB(true);
		}
	}
	Sat->CSMLVPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
									  (pyroB ? &Sat->PyroBusB : NULL));

	//
	// S-IVB/LM separation relays
	//

	pyroA = false, pyroB = false;

	if (Sat->SIVBPayloadSepSwitch.IsUp()) {
		if (Sat->SECSArmBatACircuitBraker.IsPowered() && Sat->SIVBLMSepPyroACircuitBraker.IsPowered()) {
			// Blow Pyro A
			pyroA = true;
		}
		if (Sat->SECSArmBatBCircuitBraker.IsPowered() && Sat->SIVBLMSepPyroBCircuitBraker.IsPowered()) {
			// Blow Pyro B
			pyroB = true;
		}
	}
	/// \todo This assumes instantaneous separation of the LM, but it avoids connector calls each time step
	if (pyroA || pyroB) {
		Sat->sivbControlConnector.StartSeparationPyros();
	}

	//
	// Docking ring separation relays
	//

	pyroA = false, pyroB = false;

	if (Sat->CsmLmFinalSep1Switch.IsUp() || Sat->CsmLmFinalSep2Switch.IsUp()) {
		if (IsLogicPoweredAndArmedA()) {
			// Blow Pyro A
			pyroA = true;
		}
		if (IsLogicPoweredAndArmedB()) {
			// Blow Pyro B
			pyroB = true;
		}
	}
	Sat->CMDockingRingPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
											  (pyroB ? &Sat->PyroBusB : NULL));

	//
	// CM/SM separation relays
	//

	pyroA = false, pyroB = false;

	if (Sat->CmSmSep1Switch.IsUp() || Sat->CmSmSep2Switch.IsUp()) {
		if (IsLogicPoweredAndArmedA()) {
			if (!Sat->rjec.GetCMTransferMotor1()) {
				// Blow Pyro A
				pyroA = true;
			}

			// Pressurize CM RCS
			if (Sat->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) {
				Sat->CMRCS1.OpenHeliumValves();
				Sat->CMRCS2.OpenHeliumValves();

				// Auto Tie Main Buses
				Sat->MainBusAController.SetTieAuto(true);
				Sat->MainBusBController.SetTieAuto(true);
			}

			// Transfer RCS to CM
			if (Sat->CMSMPyros.Blown() && Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnACircuitBraker.IsPowered()) {
				Sat->rjec.ActivateCMTransferMotor1();
			}
		}
		if (IsLogicPoweredAndArmedB()) {
			if (!Sat->rjec.GetCMTransferMotor2()) {
				// Blow Pyro B
				pyroB = true;
			}

			// Pressurize CM RCS
			if (Sat->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE) {
				Sat->CMRCS1.OpenHeliumValves();
				Sat->CMRCS2.OpenHeliumValves();

				// Auto Tie Main Buses
				Sat->MainBusAController.SetTieAuto(true);
				Sat->MainBusBController.SetTieAuto(true);
			}

			// Transfer RCS to CM
			if (Sat->CMSMPyros.Blown() && Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnBCircuitBraker.IsPowered()) {
				Sat->rjec.ActivateCMTransferMotor2();
			}
		}
	}
	Sat->CMSMPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
									 (pyroB ? &Sat->PyroBusB : NULL));

	//
	// Pyros
	//
	
	if (Sat->CMDockingRingPyros.Blown() && Sat->HasProbe && Sat->dockingprobe.IsEnabled())
	{
		if (!Sat->dockingprobe.IsDocked()) {
			Sat->JettisonDockingProbe();

		} else if (Sat->GetDockHandle(0)) {
			// Undock, the docking probe remains in the drogue of the other vessel
			Sat->Undock(0);
		}

		//Time to hear the Stage separation
		Sat->SMJetS.play(NOLOOP);
		// Disable docking probe because it's jettisoned 
		Sat->dockingprobe.SetEnabled(false);
		Sat->HasProbe = false;
		Sat->SetDockingProbeMesh();
	}
}

bool SECS::IsLogicPoweredAndArmedA() {

	if (Sat->SECSArmBatACircuitBraker.IsPowered() && Sat->SECSLogicBusA.Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

bool SECS::IsLogicPoweredAndArmedB() {

	if (Sat->SECSArmBatBCircuitBraker.IsPowered() && Sat->SECSLogicBusB.Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

void SECS::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, SECS_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	papiWriteScenario_double(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	papiWriteScenario_double(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);
	papiWriteScenario_bool(scn, "PYROBUSAMOTOR", PyroBusAMotor);
	papiWriteScenario_bool(scn, "PYROBUSBMOTOR", PyroBusBMotor);
	
	oapiWriteLine(scn, SECS_END_STRING);
}

void SECS::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SECS_END_STRING, sizeof(SECS_END_STRING)))
			break;

		papiReadScenario_int(line, "STATE", State);
		papiReadScenario_double(line, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
		papiReadScenario_double(line, "LASTMISSIONEVENTTIME", LastMissionEventTime);
		papiReadScenario_bool(line, "PYROBUSAMOTOR", PyroBusAMotor);
		papiReadScenario_bool(line, "PYROBUSBMOTOR", PyroBusBMotor);
	}

	// connect pyro buses
	if (PyroBusAMotor)
		Sat->PyroBusA.WireTo(&Sat->PyroBusAFeeder);
	else
		Sat->PyroBusA.Disconnect();

	if (PyroBusBMotor)
		Sat->PyroBusB.WireTo(&Sat->PyroBusAFeeder);
	else
		Sat->PyroBusB.Disconnect();
}


ELS::ELS()

{
	State = 0;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	FloatBag1Size = 0;
	FloatBag2Size = 0;
	FloatBag3Size = 0;

	Sat = 0;
	FloatBagVessel = 0;

	DyeMarkerLevel = 0;
	DyeMarkerTime = 3600; // 1 hour
}

ELS::~ELS()

{
}

void ELS::ControlVessel(Saturn *v)

{
	Sat = v;
}

void ELS::Timestep(double simt, double simdt)

{
	if (!Sat)	return;

	//
	// Float Bags
	//

	if (!Sat->ApexCoverAttached) {
		VESSELSTATUS vs;
		Sat->GetStatus(vs);

		if (!FloatBagVessel) {
			char VName[256]="";
			Sat->GetApolloName(VName);
			strcat(VName, "-FLOATBAG");
			OBJHANDLE hFloatBag = oapiGetVesselByName(VName);

			// Create the float bag vessel
			if (!hFloatBag) {
				OBJHANDLE hFloatBag = oapiCreateVessel(VName, "ProjectApollo/FloatBag", vs);
				FloatBagVessel = (FloatBag *) oapiGetVesselInterface(hFloatBag);
				// Attach it
				ATTACHMENTHANDLE ah = Sat->GetAttachmentHandle(false, 0);
				ATTACHMENTHANDLE ahc = FloatBagVessel->GetAttachmentHandle(true, 0);
				Sat->AttachChild(hFloatBag, ah, ahc);
			} else {
				FloatBagVessel = (FloatBag *) oapiGetVesselInterface(hFloatBag);
			}
		}

		// Float Bag sizes
		FloatBag1Size = NewFloatBagSize(FloatBag1Size, &Sat->FloatBagSwitch1, &Sat->FloatBag1BatACircuitBraker, simdt);
		FloatBag2Size = NewFloatBagSize(FloatBag2Size, &Sat->FloatBagSwitch2, &Sat->FloatBag2BatBCircuitBraker, simdt);
		FloatBag3Size = NewFloatBagSize(FloatBag3Size, &Sat->FloatBagSwitch3, &Sat->FloatBag3FLTPLCircuitBraker, simdt);
		FloatBagVessel->SetBagSize(1, FloatBag1Size);
		FloatBagVessel->SetBagSize(2, FloatBag2Size);
		FloatBagVessel->SetBagSize(3, FloatBag3Size);

		// Beacon
		if (Sat->GetStage() >= CM_ENTRY_STAGE_SIX) {
			// Extend beacon automatically
			FloatBagVessel->ExtendBeacon();
			// Control the light
			if (Sat->FloatBag3FLTPLCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || Sat->PostLandingBCNLTSwitch.IsCenter()) {
				FloatBagVessel->SetBeaconLight(false, false);
			} else if (Sat->PostLandingBCNLTSwitch.IsDown()) {
				FloatBagVessel->SetBeaconLight(true, false);
			} else {
				FloatBagVessel->SetBeaconLight(true, true);
			}
		}

		// Dye marker
		if (Sat->GetStage() >= CM_ENTRY_STAGE_SEVEN && DyeMarkerTime > 0) {
			// Turned on?			
			if (Sat->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->PostLandingDYEMarkerSwitch.IsUp()) { 
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
			size -= simdt / (7. * 60.);	// same as fill? 
			size = max(0, size);
		} else if (sw->IsUp() && (Sat->UprightingSystemCompressor1CircuitBraker.IsPowered() || Sat->UprightingSystemCompressor1CircuitBraker.IsPowered())) {
			size += simdt / (7. * 60.);	// Apollo 15 entry checklist
			size = min(1, size);
		}
	}
	return size;
}

void ELS::SystemTimestep(double simdt) 

{
	// Float bag compressor 1
	bool comp = false;
	if (Sat->FloatBag1BatACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->FloatBagSwitch1.IsUp()) {
		comp = true;
	}

	// Float bag compressor 2 
	if ((Sat->FloatBag2BatBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->FloatBagSwitch2.IsUp()) ||
		(Sat->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->FloatBagSwitch3.IsUp())) {
		comp = true;
	}

	if (comp) {
		if (Sat->UprightingSystemCompressor1CircuitBraker.IsPowered()) {
			Sat->UprightingSystemCompressor1CircuitBraker.DrawPower(424); // Systems handbook
		}
		if (Sat->UprightingSystemCompressor2CircuitBraker.IsPowered()) {
			Sat->UprightingSystemCompressor2CircuitBraker.DrawPower(424); // Systems handbook
		}
	}

	// Beacon light
	if (Sat->GetStage() >= CM_ENTRY_STAGE_SIX && Sat->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) {
		// LO/HI
		if (Sat->PostLandingBCNLTSwitch.IsDown()) {
			Sat->FloatBag3FLTPLCircuitBraker.DrawPower(10);	// guessed
		} else if (Sat->PostLandingBCNLTSwitch.IsUp()) {
			Sat->FloatBag3FLTPLCircuitBraker.DrawPower(40);	// guessed
		}
	}

	// Dye marker
	if (DyeMarkerLevel == 1) {
		Sat->FloatBag3FLTPLCircuitBraker.DrawPower(10);	// guessed
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
