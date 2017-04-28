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

#if !defined(_PA_SECS_H)
#define _PA_SECS_H

class Saturn;
class FloatBag;

class SECSTimer{

public:
	SECSTimer(double delay);

	virtual void Timestep(double simdt);
	virtual void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	virtual void LoadState(FILEHANDLE scn, char *end_str);
	void SetTime(double t);
	double GetTime();

	void Reset();
	void SetRunning(bool run) { Running = run; };
	void SetContact(bool cont) { Contact = cont; };
	bool ContactClosed() { return Contact; };

protected:
	double seconds;
	double delay;

	bool Running;
	bool Contact;
};

class RestartableSECSTimer : public SECSTimer
{
public:
	RestartableSECSTimer(double delay);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void SetStart(bool st) { Start = st; };

protected:
	bool Start;
};

//Reaction Control System Controller
class RCSC
{
public:
	RCSC();
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void ControlVessel(Saturn *v);
	bool GetMode1ASignal() { return Mode1ASignal; };

	//Propellant Dump and Purge Disable Timer
	RestartableSECSTimer TD1, TD2;

	//CM RCS Propellant Dump Delay
	RestartableSECSTimer TD3, TD4;

	//Purge Delay
	SECSTimer TD5, TD6;

protected:

	void TimerTimestep(double simdt);

	//Relays
	bool OxidizerDumpA;
	bool OxidizerDumpB;
	bool InterconnectAndPropellantBurnA;
	bool InterconnectAndPropellantBurnB;
	bool FuelAndOxidBypassPurgeA;
	bool FuelAndOxidBypassPurgeB;
	bool RCSCCMSMTransferA;
	bool RCSCCMSMTransferB;

	bool Mode1ASignal;

	Saturn *Sat;
};

class MESC
{
public:
	MESC();
	void Init(Saturn *v, DCbus *LogicBus, DCbus *PyroBus, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch *RCSLogicCB, CircuitBrakerSwitch *ELSBatteryCB, CircuitBrakerSwitch *EDSBreaker, MissionTimer *MT, EventTimer *ET);
	void Timestep(double simdt);

	void Liftoff();
	bool GetCSMLVSeparateRelay() { return CSMLVSeparateRelay; };
	bool GetCMSMSeparateRelay() { return CMSMSeparateRelay; };
	bool GetCMSMDeadFace() { return CMSMDeadFace; };
	bool GetAutoRCSEnableRelay() { return RCSEnableDisableRelay; };
	void SetAutoRCSEnableRelay(bool relay) { RCSEnableDisableRelay = relay; };
	bool FireUllage() { return MESCLogicBus() && RCSLogicCircuitBreaker->IsPowered() && UllageRelay; };

	//Source 31
	bool EDSMainPower();

	bool EDSAbort1Relay;
	bool EDSAbort2Relay;
	bool EDSAbort3Relay;

	void LoadState(FILEHANDLE scn, char *end_str);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
protected:

	void TimerTimestep(double simdt);

	bool EDSVote();

	//Source 11
	bool SequentialLogicBus();
	//Source 12
	bool MESCLogicBus();
	//Source 15
	bool SequentialArmBus();
	//Source 21
	bool EDSLogicPower();
	//Source P4
	bool SequentialPyroBus();

	//MESC Relays
	bool MESCLogicArm;
	bool BoosterCutoffAbortStartRelay;
	bool LETPhysicalSeparationMonitor;
	bool LESAbortRelay;
	bool AutoAbortEnableRelay;
	bool CMSMDeadFace;
	bool CMSMSeparateRelay;
	bool PyroCutout;
	bool CMRCSPress;
	bool CanardDeploy;
	bool UllageRelay;
	bool CSMLVSeparateRelay;
	bool LESMotorFire;
	bool PitchControlMotorFire;
	bool RCSEnableArmRelay;
	bool RCSEnableDisableRelay;
	bool LETJettisonAndFrangibleNutsRelay;
	bool ApexCoverJettison;
	bool ApexCoverDragChuteDeploy;
	bool ELSActivateRelay;
	bool EDSBusChangeover;

	//Miscellaneous
	bool AbortStarted;

	//Abort Start Delay
	SECSTimer TD1;
	//CM/SM Seperate Delay
	SECSTimer TD3;
	//Canard Deploy Delay
	SECSTimer TD5;
	//ELS Activate Delay
	SECSTimer TD7;
	//CSM/LM Separation Delay
	SECSTimer TD11;
	//RCS Enable Arm Delay
	SECSTimer TD13;
	//RCS CMD Enable Delay
	SECSTimer TD15;
	//Apex Cover Jettison Delay
	SECSTimer TD17;

	Saturn *Sat;

	DCbus *SECSLogicBus;
	DCbus *SECSPyroBus;
	CircuitBrakerSwitch *SECSArmBreaker;
	CircuitBrakerSwitch *RCSLogicCircuitBreaker;
	CircuitBrakerSwitch *ELSBatteryBreaker;
	CircuitBrakerSwitch *EDSBatteryBreaker;
	MissionTimer *MissionTimerDisplay;
	EventTimer *EventTimerDisplay;

	CircuitBrakerSwitch *EDSLogicBreaker;
};

///
/// This class simulates the Sequential Events Control System in the CM.
/// \ingroup InternalSystems
/// \brief SECS simulation.
///
class SECS { 

public:
	SECS();
	virtual ~SECS();

	void ControlVessel(Saturn *v);
	void Timestep(double simt, double simdt);

	void LiftoffA();
	void LiftoffB();

	void TD1_GSEReset();
	bool AbortLightPowerA();
	bool AbortLightPowerB();

	void SetEDSAbort1(bool set) { MESCA.EDSAbort1Relay = set; MESCB.EDSAbort1Relay = set; };
	void SetEDSAbort2(bool set) { MESCA.EDSAbort2Relay = set; MESCB.EDSAbort2Relay = set; };
	void SetEDSAbort3(bool set) { MESCA.EDSAbort3Relay = set; MESCB.EDSAbort3Relay = set; };

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	MESC MESCA;
	MESC MESCB;
	RCSC rcsc;

protected:
	bool IsLogicPoweredAndArmedA();
	bool IsLogicPoweredAndArmedB();
	
	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;

	bool PyroBusAMotor;
	bool PyroBusBMotor;

	Saturn *Sat;
};

///
/// This class simulates the Earth Landing System in the CM.
/// \ingroup InternalSystems
/// \brief ELS simulation.
///
class ELS { 

public:
	ELS();
	virtual ~ELS();

	void ControlVessel(Saturn *v);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);
	double *GetDyeMarkerLevelRef() { return &DyeMarkerLevel; }


protected:
	double NewFloatBagSize(double size, ThreePosSwitch *sw, CircuitBrakerSwitch *cb, double simdt);

	int State;
	double NextMissionEventTime;
	double LastMissionEventTime;

	double FloatBag1Size;
	double FloatBag2Size;
	double FloatBag3Size;
	double DyeMarkerLevel;
	double DyeMarkerTime;

	Saturn *Sat;
	FloatBag *FloatBagVessel;

	//Relays
	//K1: 24,000 Feet Lock Up
	bool LockUp24000FT;

	//Timers
	//TD1: 
};

//
// Strings for state saving.
//

#define SECS_START_STRING		"SECS_BEGIN"
#define SECS_END_STRING			"SECS_END"

#define ELS_START_STRING		"ELS_BEGIN"
#define ELS_END_STRING			"ELS_END"

#endif // _PA_SECS_H
