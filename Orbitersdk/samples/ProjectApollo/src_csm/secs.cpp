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

SECSTimer::SECSTimer(double del)

{
	Running = false;

	delay = del;
	seconds = 0.0;
}

double SECSTimer::GetTime()

{
	return seconds;
}

void SECSTimer::SetTime(double t)

{
	if (t > delay) {
		SetContact(true);
		return;
	}

	seconds = t;
}

void SECSTimer::Timestep(double simdt)

{

	if (Running) {
		double t = GetTime();

		t += simdt;

		SetTime(t);

		Running = false;
	}
	else
	{
		Reset();
	}
}

void SECSTimer::Reset()

{
	seconds = 0.0;
	Running = false;
	Contact = false;
}

void SECSTimer::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "CONTACT", Contact);
	papiWriteScenario_bool(scn, "RUNNING", Running);
	papiWriteScenario_double(scn, "TIME", GetTime());
	oapiWriteLine(scn, end_str);
}

void SECSTimer::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);
	float ftcp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		if (!strnicmp(line, "TIME", 4)) {
			sscanf(line + 4, "%f", &ftcp);
			SetTime(ftcp);
		}
		papiReadScenario_bool(line, "CONTACT", Contact);
		papiReadScenario_bool(line, "RUNNING", Running);
	}
}

RestartableSECSTimer::RestartableSECSTimer(double del) :SECSTimer(del)
{
	Start = false;
}

void RestartableSECSTimer::Timestep(double simdt)

{

	if (Start && Running) {
		double t = GetTime();

		t += simdt;

		SetTime(t);

		Running = false;
	}
}

void RestartableSECSTimer::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "CONTACT", Contact);
	papiWriteScenario_bool(scn, "RUNNING", Running);
	papiWriteScenario_bool(scn, "START", Start);
	papiWriteScenario_double(scn, "TIME", GetTime());
	oapiWriteLine(scn, end_str);
}

void RestartableSECSTimer::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);
	float ftcp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		if (!strnicmp(line, "TIME", 4)) {
			sscanf(line + 4, "%f", &ftcp);
			SetTime(ftcp);
		}
		papiReadScenario_bool(line, "CONTACT", Contact);
		papiReadScenario_bool(line, "RUNNING", Running);
		papiReadScenario_bool(line, "START", Start);
	}
}

RCSC::RCSC():
TD1(42.0),
TD2(42.0),
TD3(5.0),
TD4(5.0),
TD5(13.0),
TD6(13.0)
{
	OxidizerDumpA = false;
	OxidizerDumpB = false;
	InterconnectAndPropellantBurnA = false;
	InterconnectAndPropellantBurnB = false;
	FuelAndOxidBypassPurgeA = false;
	FuelAndOxidBypassPurgeB = false;
	RSCSCMSMTransferA = false;
	RSCSCMSMTransferB = false;

	MESCDeadfaceA = false;
	MESCDeadfaceB = false;
}

void RCSC::TimerTimestep(double simdt)
{
	TD1.Timestep(simdt);
	TD2.Timestep(simdt);
	TD3.Timestep(simdt);
	TD4.Timestep(simdt);
	TD5.Timestep(simdt);
	TD6.Timestep(simdt);
}

void RCSC::Timestep(double simdt)
{
	TimerTimestep(simdt);

	if (MESCDeadfaceA)
	{
		RSCSCMSMTransferA = true;
		if (Sat->PropDumpAutoSwitch.GetState() && TD1.ContactClosed())
		{
			OxidizerDumpA = true;
			TD3.SetStart(true);
		}
	}
	if (MESCDeadfaceB)
	{
		RSCSCMSMTransferB = true;
		if (Sat->PropDumpAutoSwitch.GetState() && TD2.ContactClosed())
		{
			OxidizerDumpB = true;
			TD4.SetStart(true);
		}
	}
	
	if (Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnACircuitBraker.IsPowered()) {
		//Activate Transfer Motor
		if (Sat->CMSMPyros.Blown()) {
			Sat->rjec.ActivateCMTransferMotor1();
		}
		if (!RSCSCMSMTransferA)
		{
			TD1.SetRunning(true);
		}
		TD3.SetRunning(true);

		if (TD3.ContactClosed())
		{
			TD5.SetRunning(true);
		}
	}

	if (Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnBCircuitBraker.IsPowered()) {
		//Activate Transfer Motor
		if (Sat->CMSMPyros.Blown()) {
			Sat->rjec.ActivateCMTransferMotor2();
			RSCSCMSMTransferB = true;
		}
		if (!RSCSCMSMTransferB)
		{
			TD2.SetRunning(true);
		}
		TD4.SetRunning(true);

		if (TD4.ContactClosed())
		{
			TD6.SetRunning(true);
		}
	}

	if (TD5.ContactClosed())
	{
		FuelAndOxidBypassPurgeA = true;
	}
	if (TD6.ContactClosed())
	{
		FuelAndOxidBypassPurgeB = true;
	}

	//Auto dumps and purges
	if (TD3.ContactClosed() || TD4.ContactClosed())
	{
		//Fuel dump
		Sat->CMRCS1.SetAutoFuelDump();
		Sat->CMRCS2.SetAutoFuelDump();
	}
	if (OxidizerDumpA || OxidizerDumpB)
	{
		//Oxidizer Dump
	}
	if (InterconnectAndPropellantBurnA)
	{
		//Helium Interconnect
		//Oxid Interconnect
		Sat->CMRCS1.SetAutoOxidizerInterconnect();
		Sat->CMRCS2.SetAutoOxidizerInterconnect();
	}
	if (InterconnectAndPropellantBurnB)
	{
		//Fuel Interconnect
		Sat->CMRCS1.SetAutoFuelInterconnect();
		Sat->CMRCS2.SetAutoFuelInterconnect();
	}
	if (FuelAndOxidBypassPurgeA || FuelAndOxidBypassPurgeB)
	{
		//Purge
		Sat->CMRCS1.SetAutoPurge();
		Sat->CMRCS2.SetAutoPurge();
	}
}

void RCSC::ControlVessel(Saturn *v)

{
	Sat = v;
}

MESC::MESC():
	TD1(0.03),
	TD3(0.1),
	TD5(11.0),
	TD11(3.0)
{
	MESCLogicArm = false;
	BoosterCutoffAbortStartRelay = false;
	LETPhysicalSeperationMonitor = false;
	LESAbortRelay = false;
	AutoAbortEnableRelay = false;
	CMSMDeadFace = false;
	CMSMSeparateRelay = false;
	PyroCutout = false;
	CMRCSPress = false;
	CanardDeploy = false;
	UllageRelay = false;
	CSMLVSeparateRelay = false;
}

void MESC::Init(Saturn *v, DCbus *LogicBus, DCbus *PyroBus, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch *RCSLogicCB, MissionTimer *MT, EventTimer *ET)
{
	SECSLogicBus = LogicBus;
	SECSPyroBus = PyroBus;
	SECSArmBreaker = SECSArm;
	RCSLogicCircuitBraker = RCSLogicCB;
	MissionTimerDisplay = MT;
	EventTimerDisplay = ET;
	Sat = v;
}

void MESC::TimerTimestep(double simdt)
{
	TD1.Timestep(simdt);
	TD1.Timestep(simdt);
	TD5.Timestep(simdt);
	TD11.Timestep(simdt);
}

void MESC::Timestep(double simdt)
{
	if (SECSLogicBus->Voltage() > SP_MIN_DCVOLTAGE)
	{
		MESCLogicArm = true;
	}
	else
	{
		MESCLogicArm = false;
	}

	if (Sat->CmSmSep1Switch.IsUp() || Sat->CmSmSep2Switch.IsUp()) {
		if (SequentialArmBus()){

			if (!PyroCutout)
				PyroCutout = true;

			if (!CMSMDeadFace)
				TD3.SetRunning(true);

			if (TD3.ContactClosed() && !CMSMSeperate)
				CMSMSeparateRelay = true;

			if (PyroCutout)
				CMSMDeadFace = true;

			if (CMSMDeadFace)
				CMRCSPress = true;
		}
	}

	// Pressurize CM RCS
	if ((CMRCSPress && SECSPyroBus->Voltage() > SP_MIN_DCVOLTAGE)) {
		Sat->CMRCS1.OpenHeliumValves();
		Sat->CMRCS2.OpenHeliumValves();
	}

	//TBD: Put timers here
	TimerTimestep(simdt);

	// Monitor LET Status
	if (MESCLogicBus())
	{
		if (Sat->LESAttached)
		{
			LETPhysicalSeperationMonitor = true;
		}
		else
		{
			LETPhysicalSeperationMonitor = false;
		}
	}

	// Abort Handling
	if (Sat->THCRotary.IsCounterClockwise())
	{
		if (MESCLogicBus())
		{
			BoosterCutoffAbortStartRelay = true;
		}
	}

	//Abort Start

	if (BoosterCutoffAbortStartRelay)
	{
		Sat->bAbort = true;

		//
		// Event timer resets to zero on abort.
		//

		Sat->EventTimerDisplay.Reset();
		Sat->EventTimerDisplay.SetRunning(true);
		Sat->EventTimerDisplay.SetEnabled(true);

		if (MESCLogicBus())
		{
			if (LETPhysicalSeperationMonitor)
			{
				LESAbortRelay = true;
			}
		}
	}

	//LET Abort Start
	if (MESCLogicBus())
	{
		if (LESAbortRelay)
		{
			TD5.SetRunning(true);
		}
	}

	//Canard Deploy
	if (MESCLogicBus())
	{
		if (TD5.ContactClosed() || Sat->CanardDeploySwitch.GetState())
		{
			CanardDeploy = true;
		}
	}

	if (SECSPyroBus->Voltage() > SP_MIN_DCVOLTAGE && CanardDeploy)
	{
		//TBD: Actually deploy canard
	}

	//SPS Abort
	if (SECSLogicBus->Voltage() > SP_MIN_DCVOLTAGE)
	{
		if (BoosterCutoffAbortStartRelay && !LETPhysicalSeperationMonitor && !LESAbortRelay)
		{
			UllageRelay = true;
			TD11.SetRunning(true);
		}
	}

	if (TD11.ContactClosed() || Sat->CsmLvSepSwitch.GetState())
	{
		CSMLVSeparateRelay = true;
	}
}

bool MESC::SequentialLogicBus()
{
	return SECSLogicBus->Voltage() > SP_MIN_DCVOLTAGE;
}

bool MESC::SequentialArmBus()
{
	return SECSArmBreaker->IsPowered();
}

bool MESC::MESCLogicBus() {

	if (SECSArmBreaker->IsPowered() && SECSLogicBus->Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

void MESC::Liftoff()
{
	if (MESCLogicBus())
	{
		MissionTimerDisplay->Reset();
		MissionTimerDisplay->SetEnabled(true);
		EventTimerDisplay->Reset();
		EventTimerDisplay->SetEnabled(true);
		EventTimerDisplay->SetRunning(true);

		if (Sat->EDSSwitch.GetState())
			AutoAbortEnableRelay = true;
	}
}

void MESC::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "BOOSTERCUTOFFABORTSTARTRELAY", BoosterCutoffAbortStartRelay);
	papiWriteScenario_bool(scn, "AUTOABORTENABLERELAY", AutoAbortEnableRelay);

	oapiWriteLine(scn, end_str);
}

void MESC::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	float flt = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "BOOSTERCUTOFFABORTSTARTRELAY", BoosterCutoffAbortStartRelay);
		papiReadScenario_bool(line, "AUTOABORTENABLERELAY", AutoAbortEnableRelay);
	}
}

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
	rcsc.ControlVessel(v);
	MESCA.Init(v, &Sat->SECSLogicBusA, &Sat->PyroBusA, &Sat->SECSArmBatACircuitBraker, &Sat->RCSLogicMnACircuitBraker, &Sat->MissionTimer306Display, &Sat->EventTimer306Display);
	MESCB.Init(v, &Sat->SECSLogicBusB, &Sat->PyroBusB, &Sat->SECSArmBatBCircuitBraker, &Sat->RCSLogicMnBCircuitBraker, &Sat->MissionTimerDisplay, &Sat->EventTimerDisplay);
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

	MESCA.Timestep(simdt);
	MESCB.Timestep(simdt);
	rcsc.Timestep(simdt);

	//
	// CSM LV separation relays
	//

	bool pyroA = false, pyroB = false;

	if (MESCA.GetCSMLVSeparateRelay() || MESCB.GetCSMLVSeparateRelay()) {
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

	if (MESCA.GetCMSMSeparateRelay())
	{
		if (!Sat->rjec.GetCMTransferMotor1()) {
			// Blow Pyro A
			pyroA = true;
		}

		// Auto Tie Main Buses
		if (Sat->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) {

			Sat->MainBusAController.SetTieAuto(true);
			Sat->MainBusBController.SetTieAuto(true);
		}

		// Transfer RCS to CM
		if (Sat->CMSMPyros.Blown() && Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnACircuitBraker.IsPowered()) {
			Sat->rjec.ActivateCMTransferMotor1();
		}
	}

	if (MESCB.GetCMSMSeparateRelay())
	{
		if (!Sat->rjec.GetCMTransferMotor2()) {
			// Blow Pyro B
			pyroB = true;
		}

		// Auto Tie Main Buses
		if (Sat->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE) {

			Sat->MainBusAController.SetTieAuto(true);
			Sat->MainBusBController.SetTieAuto(true);
		}

		// Transfer RCS to CM
		if (Sat->CMSMPyros.Blown() && Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnBCircuitBraker.IsPowered()) {
			Sat->rjec.ActivateCMTransferMotor2();
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

	//RSCS deadface
	if (IsLogicPoweredAndArmedA())
	{
		rcsc.SetDeadFaceA(MESCA.GetCMSMDeadFace());
	}
	else
	{
		rcsc.SetDeadFaceA(false);
	}
	if (IsLogicPoweredAndArmedB())
	{
		rcsc.SetDeadFaceB(MESCA.GetCMSMDeadFace());
	}
	else
	{
		rcsc.SetDeadFaceB(false);
	}

	if (MESCA.FireUllage() && Sat->THCRotary.IsCounterClockwise())
	{
		Sat->SetRCSState(RCS_SM_QUAD_B, 4, true);
		Sat->SetRCSState(RCS_SM_QUAD_D, 3, true);
		//TBD: Inhibit Pitch and Yaw
	}
	if (MESCB.FireUllage() && Sat->THCRotary.IsCounterClockwise())
	{
		Sat->SetRCSState(RCS_SM_QUAD_A, 4, true);
		Sat->SetRCSState(RCS_SM_QUAD_C, 3, true);
		//TBD: Inhibit Pitch and Yaw
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

void SECS::LiftoffA()
{
	if (IsLogicPoweredAndArmedA())
	{
		MESCA.Liftoff();
		rcsc.TD1.SetStart(true);
	}
}

void SECS::LiftoffB()
{
	if (IsLogicPoweredAndArmedB())
	{
		MESCB.Liftoff();
		rcsc.TD2.SetStart(true);
	}
}

void SECS::TD1_GSEReset()
{
	rcsc.TD1.Reset();
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
