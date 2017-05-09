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
	bool IsRunning() { return Running; };
	void SetContact(bool cont) { Contact = cont; };
	bool ContactClosed() { return Contact; };

	void SetDelay(double del) { delay = del; }

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
	bool GetStart() { return Start; };

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
	bool GetCMTransferMotor1() { return CMTransferMotor1; };
	bool GetCMTransferMotor2() { return CMTransferMotor2; };
	bool GetInterconnectAndPropellantBurnRelayA() { return InterconnectAndPropellantBurnA; }
	bool GetInterconnectAndPropellantBurnRelayB() { return InterconnectAndPropellantBurnB; }
	bool GetOxidFuelPurgeRelay() { return FuelAndOxidBypassPurgeA || FuelAndOxidBypassPurgeB; }
	bool GetFuelDumpRelay() { return TD3.ContactClosed() || TD2.ContactClosed(); }
	bool GetOxidizerDumpRelay() { return OxidizerDumpA || OxidizerDumpB; }
	void StartPropellantDumpInhibitTimerA() { TD1.SetStart(true); }
	void StartPropellantDumpInhibitTimerB() { TD8.SetStart(true); }
	bool GetPropellantDumpInhibitA() { return TD1.ContactClosed(); }
	bool GetPropellantDumpInhibitB() { return TD8.ContactClosed(); }

	void SetPropellantDumpInhibitTimers(double del) { TD1.SetDelay(del); TD8.SetDelay(del); }

protected:

	void TimerTimestep(double simdt);

	//Source 6
	bool CMRCSLogicA();
	bool CMRCSLogicB();
	//Source 7
	bool CMPropellantDumpLogicA();
	bool CMPropellantDumpLogicB();
	//Source 8
	bool CMPropellantPurgeLogicA();
	bool CMPropellantPurgeLogicB();
	//Source 9
	bool CMRCSHeDumpLogicA();
	bool CMRCSHeDumpLogicB();

	//Motor Switches

	//S1
	bool CMTransferMotor1;
	//S2
	bool CMTransferMotor2;

	//Relays

	//K1
	bool OxidizerDumpA;
	//K2
	bool OxidizerDumpB;
	//K3
	bool InterconnectAndPropellantBurnA;
	//K4
	bool InterconnectAndPropellantBurnB;
	//K5
	bool FuelAndOxidBypassPurgeA;
	//K6
	bool FuelAndOxidBypassPurgeB;
	//K13
	bool RCSCCMSMTransferA;
	//K14
	bool RCSCCMSMTransferB;

	//Propellant Dump and Purge Disable Timer
	RestartableSECSTimer TD1, TD8;

	//CM RCS Propellant Dump Delay
	RestartableSECSTimer TD3, TD2;

	//Purge Delay
	SECSTimer TD5, TD4;

	//Main DC Bus Delay
	SECSTimer TD7, TD6;

	//Misc
	bool Mode1ASignal;

	Saturn *Sat;
};

//Master Events Sequence Controller
class MESC
{
public:
	MESC();
	void Init(Saturn *v, DCbus *LogicBus, DCbus *PyroBus, CircuitBrakerSwitch *SECSLogic, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch *RCSLogicCB, CircuitBrakerSwitch *ELSBatteryCB, CircuitBrakerSwitch *EDSBreaker, MissionTimer *MT, EventTimer *ET, MESC* OtherMESCSystem);
	void Timestep(double simdt);

	void Liftoff();
	bool GetApexCoverJettisonRelay() { return ApexCoverJettison; };
	bool GetCSMLVSeparateRelay() { return CSMLVSeparateRelay; };
	bool GetCMSMSeparateRelay() { return CMSMSeparateRelay; };
	bool GetCMSMDeadFace() { return CMSMDeadFace && MESCLogicBus(); };
	bool GetCMRCSPressRelay() { return CMRCSPress; };
	bool GetAutoRCSEnableRelay() { return RCSEnableDisableRelay; };
	void SetAutoRCSEnableRelay(bool relay) { RCSEnableDisableRelay = relay; };
	void SetEDSAbortRelay1(bool relay) { EDSAbort1Relay = relay; }
	void SetEDSAbortRelay2(bool relay) { EDSAbort2Relay = relay; }
	void SetEDSAbortRelay3(bool relay) { EDSAbort3Relay = relay; }
	bool FireUllage() { return MESCLogicArm && UllageRelay; };
	bool BECO() { return BoosterCutoffAbortStartRelay; };
	bool ELSActivateLogic();

	//Source 31
	bool EDSMainPower();

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

	//Z1K1
	bool EDSAbort2Relay;
	//Z1K2
	bool EDSAbort1Relay;
	//Z2K2
	bool EDSAbort3Relay;
	//Z2K2
	bool EDSBusChangeover;
	//Z3
	bool LETPhysicalSeparationMonitor;
	//Z4
	bool AutoAbortEnableRelay;
	//Z5
	bool BoosterCutoffAbortStartRelay;
	//Z6
	bool LESAbortRelay;
	//Z7
	bool CMRCSPress;
	//Z8
	bool CMSMDeadFace;
	//Z9
	bool CMSMSeparateRelay;
	//Z10
	bool LESMotorFire;
	//Z11
	bool CanardDeploy;
	//Z12
	bool ELSActivateRelay;
	//Z13
	bool LETJettisonAndFrangibleNutsRelay;
	//Z14
	bool ApexCoverJettison;
	//Z16
	bool UllageRelay;
	//Z17
	bool CSMLVSeparateRelay;
	//Z18
	bool RCSEnableArmRelay;
	//Z19 (latching)
	bool RCSEnableDisableRelay;
	//Z20
	bool MESCLogicArm;
	//Z23
	bool PyroCutout;
	//Z24
	bool PitchControlMotorFire;
	//Z25
	bool ApexCoverDragChuteDeploy;
	bool ELSActivateSolidStateSwitch;

	//Miscellaneous
	bool AbortStarted;
	bool CMSMSeparateLogic;
	bool AutoTowerJettison;
	bool SSSInput1;
	bool SSSInput2;
	MESC* OtherMESC;

	//Abort Start Delay
	SECSTimer TD1;
	//CM/SM Seperate Delay
	SECSTimer TD3;
	//Canard Deploy Delay
	SECSTimer TD5;
	//ELS Activate Delay
	SECSTimer TD7;
	//CSM/LV Separation Delay
	SECSTimer TD11;
	//RCS Enable Arm Delay
	SECSTimer TD13;
	//RCS CMD Enable Delay
	SECSTimer TD15;
	//Apex Cover Jettison Delay
	SECSTimer TD17;
	//Pyro Cutout Delay
	SECSTimer TD23;

	Saturn *Sat;

	DCbus *SECSLogicBus;
	DCbus *SECSPyroBus;
	CircuitBrakerSwitch *SECSLogicBreaker;
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

	bool AbortLightPowerA();
	bool AbortLightPowerB();
	bool BECO();

	void SetEDSAbort1(bool set) { MESCA.SetEDSAbortRelay1(set); MESCB.SetEDSAbortRelay1(set); };
	void SetEDSAbort2(bool set) { MESCA.SetEDSAbortRelay2(set); MESCB.SetEDSAbortRelay2(set); };
	void SetEDSAbort3(bool set) { MESCA.SetEDSAbortRelay3(set); MESCB.SetEDSAbortRelay3(set); };

	void SetSaturnType(int sattype);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	//Master Events Sequence Controller A
	MESC MESCA;
	//Master Events Sequence Controller B
	MESC MESCB;
	//Reaction Control System Controller
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

class BaroSwitch
{
public:
	BaroSwitch(double open, double close);

	void ControlVessel(Saturn *v);
	void Timestep();
	bool GetStatus() { return status; }
	void SetStatus(bool stat) { status = stat; }
	bool IsClosed() { return status == 1; }
	bool IsOpen() { return status == 0; }
protected:
	bool status;
	double OpenPa, ClosePa;

	Saturn *Sat;
};


//Pyro Continuity Verification Box
class PCVB
{
public:
	PCVB();
	void Init(Saturn *v);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	bool GetMainChuteReleaseA() { return MainChuteReleaseA; }
	bool GetMainChuteReleaseB() { return MainChuteReleaseB; }
	bool GetDrogueChuteDeployA() { return DrogueChuteDeployA; }
	bool GetDrogueChuteDeployB() { return DrogueChuteDeployB; }
	bool GetMainChuteDeployA() { return DrogueChuteReleasePilotChuteDeployA; }
	bool GetMainChuteDeployB() { return DrogueChuteReleasePilotChuteDeployB; }
protected:
	//Relays

	//Z2K1
	bool DrogueChuteDeployA;
	//Z2K4
	bool DrogueChuteReleasePilotChuteDeployA;
	//Z2K5
	bool MainChuteReleaseA;

	//Z1K1
	bool DrogueChuteDeployB;
	//Z1K4
	bool DrogueChuteReleasePilotChuteDeployB;
	//Z1K5
	bool MainChuteReleaseB;

	Saturn *Sat;
};

//Earth Landing Sequence Controller
class ELSC
{
public:
	ELSC();
	void Init(Saturn *v, CircuitBrakerSwitch *ELSBatteryCB, MESC* ConnectedMESC, ELSC *OtherELSCSystem);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	bool GetDrogueParachuteDeployRelay() { return DrogueParachuteDeploy; }
	bool GetMainParachuteDeployRelay() { return PilotParachuteDeploy; }
protected:

	void TimerTimestep(double simdt);
	bool ELSBatteryPower();

	//Relays

	//K1
	bool BaroswitchLockIn;
	//K2
	bool DrogueParachuteDeploy;
	//K3
	bool PilotParachuteDeploy;

	//Delay Timers:
	
	//Drogue Parachute Deploy
	SECSTimer TD1;
	//Pilot parachute Deploy
	SECSTimer TD3;

	CircuitBrakerSwitch *ELSBatteryBreaker;

	ELSC* OtherELSC;
	MESC* mesc;

	Saturn *Sat;

	friend class PCVB;
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

	//Baroswitches
	BaroSwitch BaroSwitch24k;
	BaroSwitch BaroSwitch10k;

	//Earth Landing Sequence Controller A
	ELSC ELSCA;
	//Earth Landing Sequence Controller B
	ELSC ELSCB;
	//Pyro Continuity Verification Box
	PCVB pcvb;

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
};

//
// Strings for state saving.
//

#define SECS_START_STRING		"SECS_BEGIN"
#define SECS_END_STRING			"SECS_END"

#define ELS_START_STRING		"ELS_BEGIN"
#define ELS_END_STRING			"ELS_END"

#endif // _PA_SECS_H
