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
	Contact = false;
}

double SECSTimer::GetTime()

{
	return seconds;
}

void SECSTimer::SetTime(double t)

{
	if (seconds > delay) {
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
	RCSCCMSMTransferA = false;
	RCSCCMSMTransferB = false;

	Mode1ASignal = false;
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

	Mode1ASignal = false;
	
	if (Sat->secs.MESCA.GetCMSMDeadFace())
	{
		RCSCCMSMTransferA = true;
		if (Sat->PropDumpAutoSwitch.IsUp() && !TD1.ContactClosed())
		{
			Mode1ASignal = true;
			
			OxidizerDumpA = true;
			TD3.SetStart(true);
		}
	}
	if (Sat->secs.MESCB.GetCMSMDeadFace())
	{
		RCSCCMSMTransferB = true;
		if (Sat->PropDumpAutoSwitch.IsUp() && !TD2.ContactClosed())
		{
			Mode1ASignal = true;
			
			OxidizerDumpB = true;
			TD4.SetStart(true);
		}
	}
	
	if (Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnACircuitBraker.IsPowered()) {
		//Activate Transfer Motor
		if (Sat->CMSMPyros.Blown()) {
			Sat->rjec.ActivateCMTransferMotor1();
		}
		if (!RCSCCMSMTransferA)
		{
			TD1.SetRunning(true);
		}
		TD3.SetRunning(true);

		if (TD3.ContactClosed())
		{
			TD5.SetRunning(true);
		}

		if (OxidizerDumpA)
		{
			InterconnectAndPropellantBurnA = true;
		}
	}

	if (Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnBCircuitBraker.IsPowered()) {
		//Activate Transfer Motor
		if (Sat->CMSMPyros.Blown()) {
			Sat->rjec.ActivateCMTransferMotor2();
		}
		if (!RCSCCMSMTransferB)
		{
			TD2.SetRunning(true);
		}
		TD4.SetRunning(true);

		if (TD4.ContactClosed())
		{
			TD6.SetRunning(true);
		}

		if (OxidizerDumpB)
		{
			InterconnectAndPropellantBurnB = true;
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
		//Fuel dump, TBD: No separate Fuel and Oxidizer yet
		//Sat->CMRCS1.SetAutoFuelDump();
		//Sat->CMRCS2.SetAutoFuelDump();
	}
	if (OxidizerDumpA || OxidizerDumpB)
	{
		//Oxidizer Dump
		Sat->CMRCS1.SetAutoFuelDump();
		Sat->CMRCS2.SetAutoFuelDump();
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

void RCSC::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "OXIDIZERDUMPA", OxidizerDumpA);
	papiWriteScenario_bool(scn, "OXIDIZERDUMPB", OxidizerDumpB);
	papiWriteScenario_bool(scn, "INTERCONNECTANDPROPELLANTBURNA", InterconnectAndPropellantBurnA);
	papiWriteScenario_bool(scn, "INTERCONNECTANDPROPELLANTBURNB", InterconnectAndPropellantBurnB);
	papiWriteScenario_bool(scn, "FUELANDOXIDBYPASSPURGEA", FuelAndOxidBypassPurgeA);
	papiWriteScenario_bool(scn, "FUELANDOXIDBYPASSPURGEB", FuelAndOxidBypassPurgeB);
	papiWriteScenario_bool(scn, "RCSCCMSMTRANSFERA", RCSCCMSMTransferA);
	papiWriteScenario_bool(scn, "RCSCCMSMTRANSFERB", RCSCCMSMTransferB);
	TD1.SaveState(scn, "TD1_BEGIN", "TD1_END");
	TD2.SaveState(scn, "TD2_BEGIN", "TD_END");
	TD3.SaveState(scn, "TD3_BEGIN", "TD_END");
	TD4.SaveState(scn, "TD4_BEGIN", "TD_END");
	TD5.SaveState(scn, "TD5_BEGIN", "TD_END");
	TD6.SaveState(scn, "TD6_BEGIN", "TD_END");

	oapiWriteLine(scn, end_str);
}

void RCSC::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "OXIDIZERDUMPA", OxidizerDumpA);
		papiReadScenario_bool(line, "OXIDIZERDUMPB", OxidizerDumpB);
		papiReadScenario_bool(line, "INTERCONNECTANDPROPELLANTBURNA", InterconnectAndPropellantBurnA);
		papiReadScenario_bool(line, "INTERCONNECTANDPROPELLANTBURNB", InterconnectAndPropellantBurnB);
		papiReadScenario_bool(line, "FUELANDOXIDBYPASSPURGEA", FuelAndOxidBypassPurgeA);
		papiReadScenario_bool(line, "FUELANDOXIDBYPASSPURGEB", FuelAndOxidBypassPurgeB);
		papiReadScenario_bool(line, "RCSCCMSMTRANSFERA", RCSCCMSMTransferA);
		papiReadScenario_bool(line, "RCSCCMSMTRANSFERB", RCSCCMSMTransferB);

		if (!strnicmp(line, "TD1_BEGIN", sizeof("TD1_BEGIN"))) {
			TD1.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD2_BEGIN", sizeof("TD2_BEGIN"))) {
			TD2.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD3_BEGIN", sizeof("TD3_BEGIN"))) {
			TD3.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD4_BEGIN", sizeof("TD4_BEGIN"))) {
			TD4.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD5_BEGIN", sizeof("TD5_BEGIN"))) {
			TD5.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD6_BEGIN", sizeof("TD6_BEGIN"))) {
			TD6.LoadState(scn, "TD_END");
		}
	}
}

MESC::MESC():
	TD1(0.03),
	TD3(0.1),
	TD5(11.0),
	TD7(3.0),
	TD11(3.0),
	TD13(0.8),
	TD15(1.0),
	TD17(0.4)
{
	MESCLogicArm = false;
	BoosterCutoffAbortStartRelay = false;
	LETPhysicalSeparationMonitor = false;
	LESAbortRelay = false;
	AutoAbortEnableRelay = false;
	CMSMDeadFace = false;
	CMSMSeparateRelay = false;
	PyroCutout = false;
	CMRCSPress = false;
	CanardDeploy = false;
	UllageRelay = false;
	CSMLVSeparateRelay = false;
	LESMotorFire = false;
	PitchControlMotorFire = false;
	RCSEnableArmRelay = false;
	RCSEnableDisableRelay = false;
	LETJettisonAndFrangibleNutsRelay = false;
	ApexCoverJettison = false;
	ApexCoverDragChuteDeploy = false;
	ELSActivateRelay = false;
	EDSBusChangeover = false;
	EDSAbort1Relay = false;
	EDSAbort2Relay = false;
	EDSAbort3Relay = false;

	AbortStarted = false;

	EDSLogicBreaker = NULL;
}

void MESC::Init(Saturn *v, DCbus *LogicBus, DCbus *PyroBus, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch *RCSLogicCB, CircuitBrakerSwitch *ELSBatteryCB, CircuitBrakerSwitch *EDSBreaker, MissionTimer *MT, EventTimer *ET)
{
	SECSLogicBus = LogicBus;
	SECSPyroBus = PyroBus;
	SECSArmBreaker = SECSArm;
	RCSLogicCircuitBreaker = RCSLogicCB;
	ELSBatteryBreaker = ELSBatteryCB;
	EDSBatteryBreaker = EDSBreaker;
	MissionTimerDisplay = MT;
	EventTimerDisplay = ET;
	Sat = v;
}

void MESC::TimerTimestep(double simdt)
{
	TD1.Timestep(simdt);
	TD3.Timestep(simdt);
	TD5.Timestep(simdt);
	TD11.Timestep(simdt);
	TD13.Timestep(simdt);
	TD15.Timestep(simdt);
	TD17.Timestep(simdt);
}

void MESC::Timestep(double simdt)
{
	TimerTimestep(simdt);

	if (SequentialLogicBus())
	{
		MESCLogicArm = true;
	}
	else
	{
		MESCLogicArm = false;
	}

	//EDS Logic
	if (Sat->EDSPowerSwitch.IsUp())
	{
		//EDS Changeover
		if (EDSBatteryBreaker->IsPowered())
		{
			EDSBusChangeover = false;
		}
		else
		{
			EDSBusChangeover = true;
		}

		//Switch EDS Power
		if (!EDSBusChangeover)
		{
			EDSLogicBreaker = EDSBatteryBreaker;
		}
		else
		{
			EDSLogicBreaker = &Sat->EDS2BatCCircuitBraker;
		}

		//EDS Auto Abort
		if (EDSLogicPower())
		{
			if (EDSVote() && AutoAbortEnableRelay)
			{
				TD1.SetRunning(true);
			}
		}
	}

	//CM/SM Separation Logic
	if (LESAbortRelay || Sat->CmSmSep1Switch.IsUp() || Sat->CmSmSep2Switch.IsUp()) {
		if (SequentialArmBus()) {

			if (!PyroCutout)
				PyroCutout = true;

			if (PyroCutout)
				CMSMDeadFace = true;
		}
	}

	//Resets? Maybe?
	CMSMSeparateRelay = false;

	if (MESCLogicBus() && CMSMDeadFace)
	{
		CMRCSPress = true;

		TD3.SetRunning(true);

		if (TD3.ContactClosed())
			CMSMSeparateRelay = true;

		if (Sat->PropDumpAutoSwitch.IsDown())
		{
			TD15.SetRunning(true);
		}
	}

	// Pressurize CM RCS
	if ((CMRCSPress && SequentialPyroBus())) {
		Sat->CMRCS1.OpenHeliumValves();
		Sat->CMRCS2.OpenHeliumValves();
	}

	// Monitor LET Status
	if (MESCLogicBus())
	{
		if (Sat->LESAttached)
		{
			LETPhysicalSeparationMonitor = true;
		}
		else
		{
			LETPhysicalSeparationMonitor = false;
		}
	}

	//Auto Abort Logic
	if ((AutoAbortEnableRelay || Sat->LiftoffNoAutoAbortSwitch.GetState()) && LETPhysicalSeparationMonitor && SequentialLogicBus() && Sat->EDSSwitch.GetState())
	{
		//TBD: keep liftoff discrete
	}
	else
	{
		AutoAbortEnableRelay = false;
	}

	// Abort Handling
	if ((Sat->THCRotary.IsCounterClockwise() && SequentialLogicBus()) || TD1.ContactClosed())
	{
		BoosterCutoffAbortStartRelay = true;
	}

	if (MESCLogicBus())
	{
		//Abort Start

		if (BoosterCutoffAbortStartRelay)
		{
			if (!AbortStarted)
			{
				if (LETPhysicalSeparationMonitor)
				{
					LESAbortRelay = true;

					Sat->bAbort = true;
				}

				Sat->EventTimerDisplay.Reset();
				Sat->EventTimerDisplay.SetRunning(true);
				Sat->EventTimerDisplay.SetEnabled(true);

				AbortStarted = true;
			}
		}

		//LET Abort Start
		if (LESAbortRelay)
		{
			TD5.SetRunning(true);
		}
	}

	if ((Sat->LesMotorFireSwitch.GetState() && SequentialArmBus()) || (CMSMSeparateRelay && MESCLogicBus()))
	{
		LESMotorFire = true;
	}

	//
	// Fire the LEM.
	//
	if (LESMotorFire && SequentialPyroBus())
	{
		Sat->FireLaunchEscapeMotor();
		
		Sat->JettisonLET();
	}

	//Tower Jettison Relay
	if (MESCLogicBus() && (Sat->TowerJett1Switch.IsUp() || Sat->TowerJett2Switch.IsUp()))
	{
		LETJettisonAndFrangibleNutsRelay = true;
	}

	//Jettison Tower
	if (SequentialPyroBus() && LETJettisonAndFrangibleNutsRelay)
	{
		Sat->CutLESLegs();

		if (!Sat->LaunchFail.LESJetMotorFail)
		{
			Sat->FireTowerJettisonMotor();
			Sat->JettisonLET();
		}
	}

	//Canard Deploy
	if ((TD5.ContactClosed() && MESCLogicBus()) || (Sat->CanardDeploySwitch.GetState() && SequentialArmBus()))
	{
		CanardDeploy = true;
	}

	if (SequentialPyroBus() && CanardDeploy)
	{
		//TBD: Actually deploy canard
	}

	//Pitch Control Motor
	if (LESMotorFire && Sat->secs.rcsc.GetMode1ASignal())
	{
		PitchControlMotorFire = true;
	}

	//Fire Pitch Control Motor
	if (PitchControlMotorFire && SequentialPyroBus())
	{
		Sat->FirePitchControlMotor();
	}

	//SPS Abort
	if (SequentialLogicBus())
	{
		if (BoosterCutoffAbortStartRelay && !LETPhysicalSeparationMonitor && !LESAbortRelay)
		{
			UllageRelay = true;
			TD11.SetRunning(true);
		}
	}

	//CSM LV Separate Relay
	if (TD11.ContactClosed() || (Sat->CsmLvSepSwitch.GetState() && SequentialArmBus()))
	{
		CSMLVSeparateRelay = true;
	}
	else
	{
		CSMLVSeparateRelay = false;
	}

	//Separate Launch Vehicle
	if (SequentialPyroBus() && CSMLVSeparateRelay)
	{
		//Right now this is done in the SECS class
	}

	if (MESCLogicBus() && CSMLVSeparateRelay)
	{
		TD13.SetRunning(true);
	}

	if (MESCLogicBus() && TD13.ContactClosed())
	{
		RCSEnableArmRelay = true;
	}

	//RCS Enable/Disable Logic
	if ((MESCLogicBus() && RCSEnableArmRelay) || (SequentialArmBus() && Sat->RCSCMDSwitch.IsUp()) || TD15.ContactClosed())
	{
		RCSEnableDisableRelay = true;
	}
	else if (SequentialArmBus() && Sat->RCSCMDSwitch.IsDown())
	{
		RCSEnableDisableRelay = false;
	}

	//Apex Cover Relay
	if ((ELSBatteryBreaker->Voltage() > SP_MIN_DCVOLTAGE && Sat->ApexCoverJettSwitch.GetState()) || (MESCLogicBus() && ApexCoverJettison))
	{
		ApexCoverJettison = true;
	}
	else
	{
		ApexCoverJettison = false;
	}

	//Separate Apex Cover
	if (MESCLogicBus() && ApexCoverJettison && !Sat->ApexCoverAttached)
	{
		ApexCoverDragChuteDeploy = true;
	}

	//ELS Activate
	if (MESCLogicBus())
	{
		if (CanardDeploy)
		{
			TD7.SetRunning(true);
		}
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

bool MESC::SequentialPyroBus()
{
	return SECSPyroBus->Voltage() > SP_MIN_DCVOLTAGE;
}

bool MESC::MESCLogicBus() {

	if (SECSArmBreaker->IsPowered() && SECSLogicBus->Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

bool MESC::EDSLogicPower()
{
	return EDSLogicBreaker->Voltage() > SP_MIN_DCVOLTAGE && Sat->EDSPowerSwitch.IsUp();
}

bool MESC::EDSMainPower()
{
	return EDSBatteryBreaker->IsPowered() && Sat->EDSPowerSwitch.IsUp();
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

bool MESC::EDSVote()
{
	return (EDSAbort1Relay && EDSAbort2Relay) || (EDSAbort1Relay && EDSAbort3Relay) || (EDSAbort2Relay && EDSAbort3Relay);
}

void MESC::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "MESCLOGICARM", MESCLogicArm);
	papiWriteScenario_bool(scn, "BOOSTERCUTOFFABORTSTARTRELAY", BoosterCutoffAbortStartRelay);
	papiWriteScenario_bool(scn, "LETPHYSICALSEPARATIONMONITOR", LETPhysicalSeparationMonitor);
	papiWriteScenario_bool(scn, "LESABORTRELAY", LESAbortRelay);
	papiWriteScenario_bool(scn, "AUTOABORTENABLERELAY", AutoAbortEnableRelay);
	papiWriteScenario_bool(scn, "CMSMDEADFACE", CMSMDeadFace);
	papiWriteScenario_bool(scn, "CMSMSEPARATERELAY", CMSMSeparateRelay);
	papiWriteScenario_bool(scn, "PYROCUTOUT", PyroCutout);
	papiWriteScenario_bool(scn, "CMRCSPRESS", CMRCSPress);
	papiWriteScenario_bool(scn, "CANARDDEPLOY", CanardDeploy);
	papiWriteScenario_bool(scn, "ULLAGERELAY", UllageRelay);
	papiWriteScenario_bool(scn, "CSMLVSEPARATERELAY", CSMLVSeparateRelay);
	papiWriteScenario_bool(scn, "LESMOTORFIRE", LESMotorFire);
	papiWriteScenario_bool(scn, "PITCHCONTROLMOTORFIRE", PitchControlMotorFire);
	papiWriteScenario_bool(scn, "ABORTSTARTED", AbortStarted);
	papiWriteScenario_bool(scn, "RCSENABLEARMRELAY", RCSEnableArmRelay);
	papiWriteScenario_bool(scn, "RCSENABLEDISABLERELAY", RCSEnableDisableRelay);
	papiWriteScenario_bool(scn, "LETJETTISONANDFRANGIBLENUTSRELAY", LETJettisonAndFrangibleNutsRelay);
	papiWriteScenario_bool(scn, "APEXCOVERJETTISON", ApexCoverJettison);
	papiWriteScenario_bool(scn, "APEXCOVERDRAGCHUTEDEPLOY", ApexCoverDragChuteDeploy);
	papiWriteScenario_bool(scn, "ELSACTIVATERELAY", ELSActivateRelay);
	papiWriteScenario_bool(scn, "EDSBUSCHANGEOVER", EDSBusChangeover);
	papiWriteScenario_bool(scn, "EDSABORT1RELAY", EDSAbort1Relay);
	papiWriteScenario_bool(scn, "EDSABORT2RELAY", EDSAbort2Relay);
	papiWriteScenario_bool(scn, "EDSABORT3RELAY", EDSAbort3Relay);

	TD1.SaveState(scn, "TD1_BEGIN", "TD1_END");
	TD3.SaveState(scn, "TD3_BEGIN", "TD_END");
	TD5.SaveState(scn, "TD5_BEGIN", "TD_END");
	TD7.SaveState(scn, "TD7_BEGIN", "TD_END");
	TD11.SaveState(scn, "TD11_BEGIN", "TD_END");
	TD13.SaveState(scn, "TD13_BEGIN", "TD_END");
	TD15.SaveState(scn, "TD15_BEGIN", "TD_END");
	TD17.SaveState(scn, "TD17_BEGIN", "TD_END");

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
		papiReadScenario_bool(line, "MESCLOGICARM", MESCLogicArm);
		papiReadScenario_bool(line, "BOOSTERCUTOFFABORTSTARTRELAY", BoosterCutoffAbortStartRelay);
		papiReadScenario_bool(line, "LETPHYSICALSEPARATIONMONITOR", LETPhysicalSeparationMonitor);
		papiReadScenario_bool(line, "LESABORTRELAY", LESAbortRelay);
		papiReadScenario_bool(line, "AUTOABORTENABLERELAY", AutoAbortEnableRelay);
		papiReadScenario_bool(line, "CMSMDEADFACE", CMSMDeadFace);
		papiReadScenario_bool(line, "CMSMSEPARATERELAY", CMSMSeparateRelay);
		papiReadScenario_bool(line, "PYROCUTOUT", PyroCutout);
		papiReadScenario_bool(line, "CMRCSPRESS", CMRCSPress);
		papiReadScenario_bool(line, "CANARDDEPLOY", CanardDeploy);
		papiReadScenario_bool(line, "ULLAGERELAY", UllageRelay);
		papiReadScenario_bool(line, "CSMLVSEPARATERELAY", CSMLVSeparateRelay);
		papiReadScenario_bool(line, "LESMOTORFIRE", LESMotorFire);
		papiReadScenario_bool(line, "PITCHCONTROLMOTORFIRE", PitchControlMotorFire);
		papiReadScenario_bool(line, "ABORTSTARTED", AbortStarted);
		papiReadScenario_bool(line, "RCSENABLEARMRELAY", RCSEnableArmRelay);
		papiReadScenario_bool(line, "RCSENABLEDISABLERELAY", RCSEnableDisableRelay);
		papiReadScenario_bool(line, "LETJETTISONANDFRANGIBLENUTSRELAY", LETJettisonAndFrangibleNutsRelay);
		papiReadScenario_bool(line, "APEXCOVERJETTISON", ApexCoverJettison);
		papiReadScenario_bool(line, "APEXCOVERDRAGCHUTEDEPLOY", ApexCoverDragChuteDeploy);
		papiReadScenario_bool(line, "ELSACTIVATERELAY", ELSActivateRelay);
		papiReadScenario_bool(line, "EDSBUSCHANGEOVER", EDSBusChangeover);
		papiReadScenario_bool(line, "EDSABORT1RELAY", EDSAbort1Relay);
		papiReadScenario_bool(line, "EDSABORT2RELAY", EDSAbort2Relay);
		papiReadScenario_bool(line, "EDSABORT3RELAY", EDSAbort3Relay);

		if (!strnicmp(line, "TD1_BEGIN", sizeof("TD1_BEGIN"))) {
			TD1.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD3_BEGIN", sizeof("TD3_BEGIN"))) {
			TD3.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD5_BEGIN", sizeof("TD5_BEGIN"))) {
			TD5.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD7_BEGIN", sizeof("TD7_BEGIN"))) {
			TD7.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD11_BEGIN", sizeof("TD11_BEGIN"))) {
			TD11.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD13_BEGIN", sizeof("TD13_BEGIN"))) {
			TD13.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD15_BEGIN", sizeof("TD15_BEGIN"))) {
			TD15.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD17_BEGIN", sizeof("TD17_BEGIN"))) {
			TD17.LoadState(scn, "TD_END");
		}
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
	MESCA.Init(v, &Sat->SECSLogicBusA, &Sat->PyroBusA, &Sat->SECSArmBatACircuitBraker, &Sat->RCSLogicMnACircuitBraker, &Sat->ELSBatACircuitBraker, &Sat->EDS1BatACircuitBraker, &Sat->MissionTimer306Display, &Sat->EventTimer306Display);
	MESCB.Init(v, &Sat->SECSLogicBusB, &Sat->PyroBusB, &Sat->SECSArmBatBCircuitBraker, &Sat->RCSLogicMnBCircuitBraker, &Sat->ELSBatBCircuitBraker, &Sat->EDS3BatBCircuitBraker, &Sat->MissionTimerDisplay, &Sat->EventTimerDisplay);
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
		}
		if (IsLogicPoweredAndArmedB()) {
			// Blow Pyro B
			pyroB = true;
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

bool SECS::AbortLightPowerA()
{
	return MESCA.EDSMainPower();
}

bool SECS::AbortLightPowerB()
{
	return MESCB.EDSMainPower();
}

void SECS::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, SECS_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	papiWriteScenario_double(scn, "NEXTMISSIONEVENTTIME", NextMissionEventTime);
	papiWriteScenario_double(scn, "LASTMISSIONEVENTTIME", LastMissionEventTime);
	papiWriteScenario_bool(scn, "PYROBUSAMOTOR", PyroBusAMotor);
	papiWriteScenario_bool(scn, "PYROBUSBMOTOR", PyroBusBMotor);
	rcsc.SaveState(scn, "RCSC_BEGIN", "RCSC_END");
	MESCA.SaveState(scn, "MESCA_BEGIN", "MESC_END");
	MESCB.SaveState(scn, "MESCB_BEGIN", "MESC_END");
	
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
		
		if (!strnicmp(line, "RCSC_BEGIN", sizeof("RCSC_BEGIN"))) {
			rcsc.LoadState(scn, "RCSC_END");
		}
		else if (!strnicmp(line, "MESCA_BEGIN", sizeof("MESCA_BEGIN"))) {
			MESCA.LoadState(scn, "MESC_END");
		}
		else if (!strnicmp(line, "MESCB_BEGIN", sizeof("MESCB_BEGIN"))) {
			MESCB.LoadState(scn, "MESC_END");
		}
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
