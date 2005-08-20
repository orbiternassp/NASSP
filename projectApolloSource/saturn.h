/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Generic Saturn class for AGC interface. As much code as possible will be shared
  here between the SaturnV and Saturn1B.

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
  *	Revision 1.54  2005/08/20 11:14:52  movieman523
  *	Added Rot Contr Pwr switches and removed a number of old switches which aren't used anymore.
  *	
  *	Revision 1.53  2005/08/19 21:33:20  movieman523
  *	Added initial random failure support.
  *	
  *	Revision 1.52  2005/08/19 20:05:45  movieman523
  *	Added abort switches. Wired in Tower Jett switches and SIVB Sep switch.
  *	
  *	Revision 1.51  2005/08/19 18:38:13  movieman523
  *	Wired up parachute switches properly, and added 'Comp Acty' to CSM AGC.
  *	
  *	Revision 1.50  2005/08/19 13:41:47  tschachim
  *	Added FDAI.
  *	
  *	Revision 1.49  2005/08/18 20:54:16  movieman523
  *	Added Main Release switch and wired it up to the parachutes.
  *	
  *	Revision 1.48  2005/08/18 19:12:21  movieman523
  *	Added Event Timer switches and null Event Timer class.
  *	
  *	Revision 1.47  2005/08/18 00:22:53  movieman523
  *	Wired in CM Uplink switch, removed some old code, added initial support for second DSKY.
  *	
  *	Revision 1.46  2005/08/17 22:54:26  movieman523
  *	Added ELS and CM RCS switches.
  *	
  *	Revision 1.45  2005/08/17 00:01:59  movieman523
  *	Added ECS indicator switch, revised state saving, revised Timestep code to pass in the delta-time so we don't need to keep calculating it.
  *	
  *	Revision 1.44  2005/08/16 20:55:23  movieman523
  *	Added first saturn-specific switch for Xlunar Inject.
  *	
  *	Revision 1.43  2005/08/16 18:54:30  movieman523
  *	Added Altimeter and launch vehicle switches.
  *	
  *	Revision 1.42  2005/08/15 21:37:02  movieman523
  *	Added FDAI switches.
  *	
  *	Revision 1.41  2005/08/15 19:47:08  movieman523
  *	Added BMAG switches.
  *	
  *	Revision 1.40  2005/08/15 19:25:03  movieman523
  *	Added CSM attitude control switches and removed old ones.
  *	
  *	Revision 1.39  2005/08/15 18:48:50  movieman523
  *	Moved the stage destroy code into a generic function for Saturn V and 1b.
  *	
  *	Revision 1.38  2005/08/15 02:37:57  movieman523
  *	SM RCS is now wired up.
  *	
  *	Revision 1.37  2005/08/13 20:20:17  movieman523
  *	Created MissionTimer class and wired it into the LEM and CSM.
  *	
  *	Revision 1.36  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
  *	Revision 1.35  2005/08/13 14:59:24  movieman523
  *	Added initial null implementation of CSM caution and warning system, and removed 'master alarm' flag from Saturn class.
  *	
  *	Revision 1.34  2005/08/13 14:21:36  movieman523
  *	Added beginnings of caution and warning system.
  *	
  *	Revision 1.33  2005/08/13 11:48:26  movieman523
  *	Added remaining caution and warning switches to CSM (currently not wired up to anything).
  *	
  *	Revision 1.32  2005/08/13 00:43:50  movieman523
  *	Added more caution and warning switches.
  *	
  *	Revision 1.31  2005/08/13 00:09:43  movieman523
  *	Added IMU Cage switch
  *	
  *	Revision 1.30  2005/08/12 23:15:49  movieman523
  *	Added switches to update mission time display.
  *	
  *	Revision 1.29  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.28  2005/08/08 20:33:00  movieman523
  *	Added initial support for offsetting the mission timer and event timer from MissionTime: the real timers could be adjusted using the switches on the control panel (which aren't wired up yet), and the event timer would reset to zero on an abort.
  *	
  *	Revision 1.27  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.26  2005/08/05 13:02:54  tschachim
  *	Added crawler callback function LaunchVesselRolloutEnd
  *	
  *	Revision 1.25  2005/08/04 01:06:03  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.24  2005/08/01 19:07:47  movieman523
  *	Genericised code to deal with SM destruction on re-entry, and did some tidying up of Saturn 1b code.
  *	
  *	Revision 1.23  2005/07/31 01:43:13  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.22  2005/07/30 16:04:55  tschachim
  *	Added systemsState for the internal systems
  *	
  *	Revision 1.21  2005/07/30 02:05:55  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.20  2005/07/29 23:05:38  movieman523
  *	Added Inertial Guidance Mode start time to scenario file.
  *	
  *	Revision 1.19  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.18  2005/07/19 15:58:57  tschachim
  *	new switches
  *	
  *	Revision 1.17  2005/07/06 22:11:31  tschachim
  *	Finished SequencerSwitchesRow, no functionality yet
  *	
  *	Revision 1.16  2005/06/06 11:58:52  tschachim
  *	New switches, PanelSwitchScenarioHandler
  *	
  *	Revision 1.15  2005/05/31 02:12:08  movieman523
  *	Updated pre-entry burn variables and wrote most of the code to handle them.
  *	
  *	Revision 1.14  2005/05/31 00:17:33  movieman523
  *	Added CSMACCEL variables for unmanned flights which made burns just before re-entry to raise velocity to levels similar to a return from the moon.
  *	
  *	Revision 1.13  2005/05/26 15:51:11  tschachim
  *	New fuel cell displays and controls
  *	
  *	Revision 1.12  2005/05/18 23:34:23  movieman523
  *	Added roughly correct masses for the various Saturn payloads.
  *	
  *	Revision 1.11  2005/05/05 21:33:46  tschachim
  *	Introduced cryo fans and fuel cell indicators
  *	Renamed some toggleswitch interfaces and functions
  *	
  *	Revision 1.10  2005/04/22 13:59:46  tschachim
  *	Introduced PanelSDK
  *	Panelid defines
  *	New switches
  *	
  *	Revision 1.9  2005/04/14 23:12:44  movieman523
  *	Added post-splashdown audio support. Unfortunately I can't test this at the moment as the control panel switches for getting out of the CM after splashdown aren't working :).
  *	
  *	However, it's pretty simple code, so 90+% likely to work.
  *	
  *	Revision 1.8  2005/04/01 14:22:00  tschachim
  *	Added RCSIndicatorsSwitch
  *	
  *	Revision 1.7  2005/03/13 21:20:12  chode99
  *	Added support for accurate g-gauge calculations.
  *	
  *	Revision 1.6  2005/03/11 17:54:00  tschachim
  *	Introduced GuardedToggleSwitch and GuardedThreePosSwitch
  *	
  *	Revision 1.5  2005/03/09 22:09:02  tschachim
  *	Docking panel camera without changing direction from Yogenfrutz
  *	Switchable docking panel MFD
  *	
  *	Revision 1.4  2005/03/09 00:24:15  chode99
  *	Added header stuff for SII retrorockets.
  *	
  *	Revision 1.3  2005/03/03 17:54:13  tschachim
  *	new panel and MFD variables
  *	
  *	Revision 1.2  2005/02/18 00:44:06  movieman523
  *	Added new Apollo 13 sounds, removed unused MESHHANDLEs.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

#if !defined(_PA_SATURN_H)
#define _PA_SATURN_H

#include "PanelSDK/PanelSDK.h"

#include "cautionwarning.h"
#include "csmcautionwarning.h"
#include "missiontimer.h"
#include "FDAI.h"
#include "satswitches.h"

//
// Random failure flags.
//

typedef union {
	struct {
		unsigned Init:1;
		unsigned CoverFail:1;
		unsigned DrogueFail:1;
		unsigned MainFail:1;
	} u;
	int word;
} LandingFailures;

typedef union {
	struct {
		unsigned Init:1;
		unsigned EarlySICenterCutoff:1;
		unsigned EarlySIICenterCutoff:1;
		unsigned LETAutoJetFail:1;
		unsigned SIIAutoSepFail:1;
	} u;
	int word;
} LaunchFailures;

typedef union {
	struct {
		unsigned Init:1;
		unsigned TowerJett1Fail:1;
		unsigned TowerJett2Fail:1;
		unsigned SMJett1Fail:1;
		unsigned SMJett2Fail:1;
	} u;
	int word;
} SwitchFailures;

class Saturn: public VESSEL2, public PanelSwitchListener {

public:
	Saturn(OBJHANDLE hObj, int fmodel);
	virtual ~Saturn();

	void SetAutopilot(bool ap) { autopilot = ap; };
	bool GetAutopilot() { return autopilot; };

	void SetSIISep() { SIISepState = true; };
	void ClearSIISep() { SIISepState = false; };
	void SetEngineIndicator(int i);
	void ClearEngineIndicator(int i);

	bool TLIDone() { return TLIBurnDone; };
	bool isTLICapable() { return TLICapableBooster; };

	void UpdateLaunchTime(double t);

	//
	// Set up the default mesh for the virtual cockpit.
	//

	void DefaultCMMeshSetup();

	virtual bool SIVBStart();
	virtual void SIVBStop();

	void clbkSaveState (FILEHANDLE scn);
	void clbkPostStep (double simt, double simdt, double mjd);
	void clbkPreStep(double simt, double simdt, double mjd);
	bool clbkLoadPanel (int id);

	//
	// General functions that handle calls from Orbiter.
	//
	void clbkDockEvent(int dock, OBJHANDLE connected);
	bool clbkLoadGenericCockpit ();
	bool clbkPanelMouseEvent(int id, int event, int mx, int my);
	bool clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf);
	void clbkMFDMode (int mfd, int mode);

	virtual void Timestep(double simt, double simdt) = 0;
	int clbkConsumeDirectKey(char *keystate);

	bool clbkLoadVC (int id);

	void PanelSwitchToggled(ToggleSwitch *s);
	void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s); 

	// called by crawler after arrival on launch pad
	virtual void LaunchVesselRolloutEnd() {};

	int GetStage() { return stage; };

	void EnableTLI();
	void DisableTLI() { TLIEnabled = false; };

protected:

	void InitSwitches();
	void SetEngineIndicators();
	void ClearEngineIndicators();
	void SetLiftoffLight();
	void ClearLiftoffLight();
	void SetAutopilotLight();
	void ClearAutopilotLight();

	//
	// State that needs to be saved.
	//

	bool autopilot;
	bool SIISepState;
	bool TLIBurnDone;
	bool ABORT_IND;

	int buildstatus;

	//
	// Current mission time and mission times for stage events.
	//

	double MissionTime;
	double NextMissionEventTime;
	double LastMissionEventTime;
	double NextDestroyCheckTime;
	double NextFailureTime;

	//
	// Offset from mission time for clock display on control
	// panel, and offset of event timer from mission time.
	//

	MissionTimer MissionTimerDisplay;
	EventTimer EventTimerDisplay;

	//
	// Center engine shutdown times for first and
	// second stage.
	//

	double FirstStageCentreShutdownTime;
	double SecondStageCentreShutdownTime;

	//
	// Mixture-ratio shift time for second stage.
	//

	double SecondStagePUShiftTime;

	//
	// Iterative Guidance Mode start time, when we stop following the pitch program and start aiming for
	// the correct orbit.
	//

	double IGMStartTime;
	bool IGMEnabled;

	bool LEM_DISPLAY;

	int dockstate;

	int VehicleNo;
	int ApolloNo;

	bool UseATC;
	int Realism;

	bool Scorrec;
	bool ApolloExploded;
	bool Burned;
	bool EVA_IP;
	bool HatchOpen;
	bool CryoStir;

	double TCPO;

	//
	// Failures.
	//

	LandingFailures LandFail;
	LaunchFailures LaunchFail;
	SwitchFailures SwitchFail;

	//
	// Pitch table.
	//

#define PITCH_TABLE_SIZE	16

	double met[PITCH_TABLE_SIZE];
	double cpitch[PITCH_TABLE_SIZE];

	//
	// Switches
	//

	FDAI fdaiRight;
	HBITMAP hBmpFDAIRollIndicator;

	GuardedPushSwitch LiftoffNoAutoAbortSwitch;
	GuardedPushSwitch LesMotorFireSwitch;
	GuardedPushSwitch CanardDeploySwitch;
	GuardedPushSwitch CsmLvSepSwitch;
	GuardedPushSwitch ApexCoverJettSwitch;
	GuardedPushSwitch DrogueDeploySwitch;
	GuardedPushSwitch MainDeploySwitch;
	GuardedPushSwitch CmRcsHeDumpSwitch;

	ToggleSwitch	    EDSSwitch;				
	GuardedToggleSwitch CsmLmFinalSep1Switch;
	GuardedToggleSwitch CsmLmFinalSep2Switch;
	GuardedToggleSwitch CmSmSep1Switch;
	GuardedToggleSwitch CmSmSep2Switch;
	GuardedToggleSwitch SivbLmSepSwitch;
	
	ToggleSwitch   CabinFan1Switch;
	ToggleSwitch   CabinFan2Switch;
	ThreePosSwitch H2Heater1Switch;
	ThreePosSwitch H2Heater2Switch;
	ThreePosSwitch O2Heater1Switch;
	ThreePosSwitch O2Heater2Switch;	
	ToggleSwitch   O2PressIndSwitch;	
	ThreePosSwitch H2Fan1Switch; 
	ThreePosSwitch H2Fan2Switch; 
	ThreePosSwitch O2Fan1Switch; 
	ThreePosSwitch O2Fan2Switch; 

	IndicatorSwitch FuelCellPhIndicator;
	IndicatorSwitch FuelCellRadTempIndicator;

	IndicatorSwitch FuelCellRadiators1Indicator;
	IndicatorSwitch FuelCellRadiators2Indicator;
	IndicatorSwitch FuelCellRadiators3Indicator;
	ThreePosSwitch FuelCellRadiators1Switch;
	ThreePosSwitch FuelCellRadiators2Switch;
	ThreePosSwitch FuelCellRadiators3Switch;

	RotationalSwitch FuelCellIndicatorsSwitch;

	ToggleSwitch FuelCellHeater1Switch;
	ToggleSwitch FuelCellHeater2Switch;
	ToggleSwitch FuelCellHeater3Switch;

	ThreePosSwitch FuelCellPurge1Switch;
	ThreePosSwitch FuelCellPurge2Switch;
	ThreePosSwitch FuelCellPurge3Switch;

	IndicatorSwitch FuelCellReactants1Indicator;
	IndicatorSwitch FuelCellReactants2Indicator;
	IndicatorSwitch FuelCellReactants3Indicator;
	ThreePosSwitch FuelCellReactants1Switch;
	ThreePosSwitch FuelCellReactants2Switch;
	ThreePosSwitch FuelCellReactants3Switch;

	ToggleSwitch FCReacsValvesSwitch;
	ToggleSwitch H2PurgeLineSwitch;

	ThreePosSwitch FuelCellPumps1Switch;
	ThreePosSwitch FuelCellPumps2Switch;
	ThreePosSwitch FuelCellPumps3Switch;

	ThreePosSwitch SuitCompressor1Switch;
	ThreePosSwitch SuitCompressor2Switch;

	ThreePosSwitch SBandNormalXPDRSwitch;
	ThreePosSwitch SBandNormalPwrAmpl1Switch;
	ThreePosSwitch SBandNormalPwrAmpl2Switch;
	ThreePosSwitch SBandNormalMode1Switch;
	ThreePosSwitch SBandNormalMode2Switch;
	ToggleSwitch SBandNormalMode3Switch;

	TimerControlSwitch MissionTimerSwitch;
	CWSModeSwitch CautionWarningModeSwitch;
	CWSSourceSwitch CautionWarningCMCSMSwitch;
	CWSPowerSwitch CautionWarningPowerSwitch;
	CWSLightTestSwitch CautionWarningLightTestSwitch;

	TimerUpdateSwitch MissionTimerHoursSwitch;
	TimerUpdateSwitch MissionTimerMinutesSwitch;
	TimerUpdateSwitch MissionTimerSecondsSwitch;

	IMUCageSwitch IMUGuardedCageSwitch;

	ThreePosSwitch RPswitch15;

	SwitchRow SMRCSHelium1Row;

	ToggleSwitch SMRCSHelium1ASwitch;
	ToggleSwitch SMRCSHelium1BSwitch;
	ToggleSwitch SMRCSHelium1CSwitch;
	ToggleSwitch SMRCSHelium1DSwitch;

	SwitchRow SMRCSHelium2Row;

	ToggleSwitch SMRCSHelium2ASwitch;
	ToggleSwitch SMRCSHelium2BSwitch;
	ToggleSwitch SMRCSHelium2CSwitch;
	ToggleSwitch SMRCSHelium2DSwitch;

	AGCIOSwitch CMUplinkSwitch;
	ToggleSwitch IUUplinkSwitch;

	GuardedToggleSwitch CMRCSPressSwitch;
	ToggleSwitch SMRCSIndSwitch;

	SwitchRow SMRCSProp1Row;

	ToggleSwitch SMRCSProp1ASwitch;
	ToggleSwitch SMRCSProp1BSwitch;
	ToggleSwitch SMRCSProp1CSwitch;
	ToggleSwitch SMRCSProp1DSwitch;

	ThreePosSwitch SMRCSHeaterASwitch;
	ThreePosSwitch SMRCSHeaterBSwitch;
	ThreePosSwitch SMRCSHeaterCSwitch;
	ThreePosSwitch SMRCSHeaterDSwitch;

	SwitchRow SMRCSProp2Row;

	ToggleSwitch SMRCSProp2ASwitch;
	ToggleSwitch SMRCSProp2BSwitch;
	ToggleSwitch SMRCSProp2CSwitch;
	ToggleSwitch SMRCSProp2DSwitch;

	ThreePosSwitch RCSCMDSwitch;
	ThreePosSwitch RCSTrnfrSwitch;
	ThreePosSwitch CMRCSIsolate1;
	ThreePosSwitch CMRCSIsolate2;

	SwitchRow RCSIndicatorsSwitchRow;
	RotationalSwitch RCSIndicatorsSwitch;

	SwitchRow ECSIndicatorsSwitchRow;
	RotationalSwitch ECSIndicatorsSwitch;

	SwitchRow AttitudeControlRow;
	ThreePosSwitch ManualAttRollSwitch;
	ThreePosSwitch ManualAttPitchSwitch;
	ThreePosSwitch ManualAttYawSwitch;
	ToggleSwitch LimitCycleSwitch;
	ToggleSwitch AttDeadbandSwitch;
	ToggleSwitch AttRateSwitch;
	ToggleSwitch TransContrSwitch;

	SwitchRow BMAGRow;
	ThreePosSwitch BMAGRollSwitch;
	ThreePosSwitch BMAGPitchSwitch;
	ThreePosSwitch BMAGYawSwitch;

	SwitchRow EntryModeRow;
	ThreePosSwitch EntryModeSwitch;

	//
	// FDAI control switches.
	//

	SwitchRow FDAISwitchesRow;
	ThreePosSwitch FDAIScaleSwitch;
	ThreePosSwitch FDAISourceSwitch;
	ThreePosSwitch FDAISelectSwitch;
	ToggleSwitch FDAIAttSetSwitch;

	//
	// CMC Att: IMU is normal state, GDC does nothing.
	//

	SwitchRow CMCAttRow;
	ToggleSwitch CMCAttSwitch;

	//
	// Launch vehicle switches.
	//

	SwitchRow LVRow;
	ToggleSwitch LVGuidanceSwitch;
	GuardedToggleSwitch SIISIVBSepSwitch;
	XLunarSwitch TLIEnableSwitch;

	//
	// ELS and CSM propellant switches.
	//

	SwitchRow ELSRow;
	AGCIOSwitch CGSwitch;
	GuardedToggleSwitch ELSLogicSwitch;
	ToggleSwitch ELSAutoSwitch;
	ToggleSwitch CMRCSLogicSwitch;
	GuardedToggleSwitch CMPropDumpSwitch;
	GuardedToggleSwitch CPPropPurgeSwitch;

	//
	// Event Timer switches.
	//

	SwitchRow EventTimerRow;
	ToggleSwitch FCSMSPSASwitch;
	ToggleSwitch FCSMSPSBSwitch;
	EventTimerResetSwitch EventTimerUpDownSwitch;
	EventTimerControlSwitch EventTimerControlSwitch;
	TimerUpdateSwitch EventTimerMinutesSwitch;
	TimerUpdateSwitch EventTimerSecondsSwitch;

	//
	// Main chute release switch.
	//
	// Currently this does nothing, as the parachutes automatically release from the CM
	// after landing.
	//

	SwitchRow MainReleaseRow;
	GuardedToggleSwitch MainReleaseSwitch;

	//
	// Abort switches.
	//

	SwitchRow AbortRow;
	ToggleSwitch PropDumpAutoSwitch;
	ToggleSwitch TwoEngineOutAutoSwitch;
	ToggleSwitch LVRateAutoSwitch;
	GuardedToggleSwitch TowerJett1Switch;
	GuardedToggleSwitch TowerJett2Switch;

	//
	// Rotational Controller power switches.
	//

	SwitchRow RotContrPowerRow;
	ThreePosSwitch RotPowerNormal1Switch;
	ThreePosSwitch RotPowerNormal2Switch;
	ThreePosSwitch RotPowerDirect1Switch;
	ThreePosSwitch RotPowerDirect2Switch;

	//
	// dV Thrust switches.
	//

	SwitchRow dvThrustRow;
	GuardedToggleSwitch dVThrust1Switch;
	GuardedToggleSwitch dVThrust2Switch;

	//
	// OLD Switches: delete these as and when we can do so.
	//
	// old stuff begin
	//

	bool Sswitch6;
	bool Sswitch7;
	bool Sswitch8;
	bool Sswitch9;

	bool RPswitch1;
	bool RPswitch2;
	bool RPswitch3;
	bool RPswitch4;
	bool RPswitch5;
	bool RPswitch6;
	bool RPswitch7;
	bool RPswitch8;
	bool RPswitch9;
	bool CMRswitch;
	bool CMRCswitch;
	bool RPswitch10;
	bool RPswitch11;
	bool RPswitch12;
	bool RPswitch13;
	bool RPswitch14;

	bool RPswitch17;
	bool DPSwitch1;
	bool DPSwitch2;
	bool DPSwitch3;
	bool DPSwitch4;
	bool DPSwitch5;
	bool DPSwitch6;
	bool DPSwitch7;
	bool DPSwitch8;
	bool DPSwitch9;
	bool DPSwitch10;

	bool CMCswitch;
	bool SCswitch;

	bool CMDswitch;
	bool CMDCswitch;
	bool CMPswitch;
	bool CMPCswitch;

	bool DPswitch;
	bool DPCswitch;

	bool FCswitch1;
	bool FCswitch2;
	bool FCswitch3;
	bool FCswitch4;
	bool FCswitch5;
	bool FCswitch6;

	bool P113switch;

	bool EMSKswitch;

	AttitudeToggle LPswitch5;
	ToggleSwitch LPswitch6;
	ToggleSwitch LPswitch7;

	ToggleSwitch SPSswitch;

	ToggleSwitch P111switch;
	ToggleSwitch P112switch;

	ToggleSwitch P114switch;
	ToggleSwitch P115switch;
	ToggleSwitch P116switch;
	ToggleSwitch P117switch;
	ThreePosSwitch P118switch;
	ThreePosSwitch P119switch;

	ThreePosSwitch P24switch;
	ThreePosSwitch P25switch;
	ThreePosSwitch P26switch;
	ThreePosSwitch P27switch;
	ThreePosSwitch P28switch;

	ThreePosSwitch P212switch;
	ThreePosSwitch P213switch;
	ThreePosSwitch P214switch;
	ThreePosSwitch P215switch;

	ThreePosSwitch P216switch;
	ThreePosSwitch P217switch;

	ThreePosSwitch P222switch;
	ThreePosSwitch P223switch;
	ThreePosSwitch P224switch;
	ThreePosSwitch P225switch;
	ThreePosSwitch P226switch;
	ThreePosSwitch P227switch;
	ThreePosSwitch P228switch;
	ThreePosSwitch P229switch;
	ThreePosSwitch P230switch;

	ThreePosSwitch P231switch;
	ThreePosSwitch P232switch;
	ThreePosSwitch P233switch;
	ThreePosSwitch P234switch;
	ThreePosSwitch P235switch;
	ThreePosSwitch P236switch;
	ThreePosSwitch P237switch;
	ThreePosSwitch P238switch;
	ThreePosSwitch P239switch;
	ThreePosSwitch P240switch;
	ThreePosSwitch P241switch;
	ThreePosSwitch P242switch;
	ThreePosSwitch P243switch;
	ThreePosSwitch P244switch;

	ThreePosSwitch P246switch;
	ThreePosSwitch P247switch;

	ThreePosSwitch P248switch;
	ThreePosSwitch P249switch;

	ToggleSwitch P31switch;
	ToggleSwitch P32switch;
	ToggleSwitch P33switch;
	ThreePosSwitch P34switch;
	ThreePosSwitch P35switch;
	ThreePosSwitch P36switch;

	ThreePosSwitch P37switch;

	ThreePosSwitch P38switch;
	ThreePosSwitch P39switch;

	ThreePosSwitch P310switch;

	ThreePosSwitch P311switch;
	ThreePosSwitch P312switch;
	ThreePosSwitch P313switch;
	ThreePosSwitch P314switch;
	ThreePosSwitch P315switch;
	ThreePosSwitch P316switch;
	ThreePosSwitch P317switch;
	ThreePosSwitch P318switch;
	ThreePosSwitch P319switch;
	ThreePosSwitch P320switch;
	ThreePosSwitch P321switch;
	ThreePosSwitch P322switch;
	ThreePosSwitch P323switch;
	ThreePosSwitch P324switch;
	ThreePosSwitch P325switch;
	ThreePosSwitch P326switch;
	ThreePosSwitch P327switch;
	ToggleSwitch P328switch;
	ToggleSwitch P329switch;
	ThreePosSwitch P330switch;
	ThreePosSwitch P331switch;
	ThreePosSwitch P332switch;
	ThreePosSwitch P333switch;
	ThreePosSwitch P334switch;
	ThreePosSwitch P335switch;
	ThreePosSwitch P336switch;

	ThreePosSwitch EMSswitch;

	ThreePosSwitch P337switch;
	ThreePosSwitch P338switch;
	ThreePosSwitch P339switch;
	ThreePosSwitch P340switch;
	ThreePosSwitch P341switch;
	ThreePosSwitch P342switch;
	ThreePosSwitch P343switch;
	ThreePosSwitch P344switch;
	ThreePosSwitch P345switch;
	ThreePosSwitch P346switch;
	ThreePosSwitch P347switch;

	// old stuff end

	//
	// stage is the main stage of the flight. StageState is the current state within
	// that stage.
	//

	int stage;
	int StageState;

#define SATSYSTEMS_NONE				 0
#define SATSYSTEMS_PRELAUNCH		10
#define SATSYSTEMS_CABINCLOSEOUT	20
#define SATSYSTEMS_CABINVENTING		30
#define SATSYSTEMS_FLIGHT			40

	int systemsState;

	//
	// End saved state.
	//

	//
	// Stage masses: should really be saved, but probably aren't at the
	// moment.
	//

	double CM_Mass;
	double CM_EmptyMass;
	double CM_FuelMass;
	double SM_Mass;
	double SM_EmptyMass;
	double SM_FuelMass;
	double S4PL_Mass;
	double Abort_Mass;
	double Interstage_Mass;
	double S4B_Mass;
	double S4B_EmptyMass;
	double S4B_FuelMass;
	double SII_Mass;
	double SII_EmptyMass;
	double SII_FuelMass;
	double SI_Mass;
	double SI_EmptyMass;
	double SI_FuelMass;

	//
	// Calculated total stage mass.
	//

	double Stage1Mass;
	double Stage2Mass;
	double Stage3Mass;

	bool ENGIND[6];
	bool LAUNCHIND[8];
	bool AutopilotLight;

	//
	// And state that doesn't need to be saved.
	//

	double aVAcc;
	double aVSpeed;
	double aHAcc;
	double aZAcc;
	double ALTN1;
	double SPEEDN1;
	double VSPEEDN1;
	double XSPEEDN1;
	double YSPEEDN1;
	double ZSPEEDN1;
	double aTime;

	double actualVEL;
	double actualALT;
	double actualFUEL;

	double ThrustAdjust;

	bool KEY1;
	bool KEY2;
	bool KEY3;
	bool KEY4;
	bool KEY5;
	bool KEY6;
	bool KEY7;
	bool KEY8;
	bool KEY9;

	//
	// State variables that must be initialised but not saved.
	//

	bool FirstTimestep;
	bool GenericFirstTimestep;

	int CurrentTimestep;
	int LongestTimestep;
	double LongestTimestepLength;

	PanelSwitches MainPanel;
	PanelSwitchScenarioHandler PSH;

	SwitchRow SequencerSwitchesRow;
	SwitchRow SeparationSwitchesRow;
	SwitchRow CryoTankSwitchesRow;
	SwitchRow FuelCellPhRadTempIndicatorsRow;
	SwitchRow FuelCellRadiatorsIndicatorsRow;
	SwitchRow FuelCellRadiatorsSwitchesRow;
	SwitchRow FuelCellIndicatorsSwitchRow;
	SwitchRow FuelCellHeatersSwitchesRow;
	SwitchRow FuelCellPurgeSwitchesRow;
	SwitchRow FuelCellReactantsIndicatorsRow;
	SwitchRow FuelCellReactantsSwitchesRow;
	SwitchRow FuelCellLatchSwitchesRow;
	SwitchRow FuelCellPumpsSwitchesRow;
	SwitchRow SuitCompressorSwitchesRow;

	SwitchRow SBandNormalSwitchesRow;

	// old stuff begin
	HUDToggle HUDswitch1;
	HUDToggle HUDswitch2;
	HUDToggle HUDswitch3;

	NavModeToggle NavToggleKillrot;
	NavModeToggle NavTogglePrograde;
	NavModeToggle NavToggleRetrograde;
	NavModeToggle NavToggleNormal;
	NavModeToggle NavToggleAntiNormal;

	NavModeToggle NavToggleHAlt;
	NavModeToggle NavToggleHLevel;

	SwitchRow LPRow;
	SwitchRow HUDRow;
	SwitchRow SPSRow;
	//SwitchRow EDSRow;
	SwitchRow NAVRow1;
	SwitchRow NAVRow2;
	SwitchRow CautionWarningRow;
	SwitchRow MissionTimerSwitchesRow;
	SwitchRow IMUCageSwitchRow;

	SwitchRow SRP1Row;
	SwitchRow P14Row;
	SwitchRow P15Row;

	SwitchRow P17Row;
	SwitchRow P18Row;
	SwitchRow P19Row;

	SwitchRow MRRow;

	SwitchRow P21Row;
	SwitchRow P24Row;
	SwitchRow P25Row;

	SwitchRow P27Row;
	SwitchRow P28Row;
	SwitchRow P29Row;
	SwitchRow P30Row;
	SwitchRow P31Row;
	SwitchRow P32Row;
	SwitchRow P33Row;
	SwitchRow P34Row;
	SwitchRow P35Row;
	SwitchRow P36Row;

	SwitchRow P37Row;

	SwitchRow XRow;
	SwitchRow RHRRow;

	SwitchRow EMS1Row;

	SwitchRow LPSRow;
	// old stuff end

	//
	// Do we have a crew, or is this an unmanned flight?
	//

	bool Crewed;

	//
	// Unmanned flight parameters.
	//

	bool AutoSlow;

	int SIVBPayload;

	//
	// CSM seperation info for unmanned flights.
	//

	bool CSMSepSet;
	double CSMSepTime;
	bool CMSepSet;
	double CMSepTime;

	//
	// SIVB burn info for unmanned flights.
	//

	bool SIVBBurn;
	double SIVBBurnStart;
	double SIVBApogee;

	//
	// CSM burn information in unmanned flights.
	//

	bool CSMBurn;
	double CSMBurnStart;
	double CSMApogee;
	double CSMPerigee;

	//
	// Pre-entry CSM acceleration and end time for unmanned flights.
	//

	bool CSMAccelSet;
	double CSMAccelTime;
	double CSMAccelEnd;
	double CSMAccelPitch;

	bool TLICapableBooster;
	bool TLIEnabled;
	bool bAbort;
	bool bAbtlocked;
	bool bRecovery;
	bool autoDISP;

	bool ActivateLEM;
	bool ActivateS4B;
	bool ToggleEva;

#define SATVIEW_CDR		0
#define SATVIEW_DMP		1
#define SATVIEW_CMP		2
#define SATVIEW_DOCK	3

	unsigned int	viewpos;

	UINT probeidx;
	bool probeOn;
	bool ActivateASTP;
	bool FIRSTCSM;
	bool bManualSeparate;
	bool bManualUnDock;
	bool ASTPMission;
	bool ReadyAstp;
	bool bToggleHatch;
	bool SMSep;
	bool bStartS4B;
	bool Abort_Locked;
	bool stgSM;

	double DockAngle;

	double AtempP;
	double AtempY;
	double AtempR;

	double ignition_SMtime;
	double release_time;
	double abortTimer;

	ELEMENTS elemSaturn1B;
	double refSaturn1B;
	ELEMENTS elemPREV;
	double refPREV;
	double AltitudePREV;

	double 	Offset1st;

	bool StopRot;
	bool ProbeJetison;
	bool RCS_Full;
	bool LEMdatatransfer;
	bool InVC;
	bool InPanel;

#define SATPANEL_LOWER				0
#define SATPANEL_MAIN				1
#define SATPANEL_LEFT				2
#define SATPANEL_RIGHT				3
#define SATPANEL_LEFT_RNDZ_WINDOW	4
#define SATPANEL_RIGHT_RNDZ_WINDOW	5
#define SATPANEL_HATCH_WINDOW		6

	int  PanelId;
	bool KranzPlayed;
	bool PostSplashdownPlayed;

	OBJHANDLE hEVA;

	SoundLib soundlib;

	//
	// Surfaces.
	//

	SURFHANDLE srf[nsurf];  // handles for panel bitmaps
	SURFHANDLE SMExhaustTex;
	SURFHANDLE SMMETex;
	SURFHANDLE CMTex;

	//
	// Hardware support.
	//

	//
	// CSM has two DSKYs: one is in the main panel, the other is below. For true realism we should support
	// both.
	//

	DSKY dsky;
	DSKY dsky2;
	CSMcomputer agc;
	IMU imu;
	CSMCautionWarningSystem cws;

	//
	// Vessel handles.
	//

	OBJHANDLE hLMV;
	OBJHANDLE hstg1;
	OBJHANDLE hstg2;
	OBJHANDLE hintstg;
	OBJHANDLE hesc1;
	OBJHANDLE hPROBE;
	OBJHANDLE hs4bM;
	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;
	OBJHANDLE habort;
	OBJHANDLE hMaster;
	OBJHANDLE hSMJet;

	//
	// ISP and thrust values, which vary depending on vehicle number.
	//

	double ISP_FIRST_SL;
	double ISP_FIRST_VAC;
	double ISP_SECOND_SL;//300*G;
	double ISP_SECOND_VAC;//421*G;
	double ISP_THIRD_VAC;//421*G;

	double THRUST_FIRST_VAC;
	double THRUST_SECOND_VAC;//115200*G;
	double THRUST_THIRD_VAC;

	//
	// Generic functions shared between SaturnV and Saturn1B
	//

	void ToggelHatch();
	void ToggelHatch2();
	void AddRCSJets(double TRANZ,double MaxThrust);
	void AddRCS_S4B();
	void ToggleEVA();
	void SetupEVA();
	void SetRecovery();
	void UllageSM(OBJHANDLE hvessel,double gaz1, double time);
	void InitPanel(int panel);
	void SetSwitches(int panel);
	void ReleaseSurfaces();
	void KillDist(OBJHANDLE &hvessel);
	void KillAlt(OBJHANDLE &hvessel,double altVS);
	void RedrawPanel_G (SURFHANDLE surf);
	void RedrawPanel_Thrust (SURFHANDLE surf);
	void RedrawPanel_Alt (SURFHANDLE surf);
	void RedrawPanel_Horizon (SURFHANDLE surf);
	void RedrawPanel_MFDButton (SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset);
	void RedrawPanel_SuitCabinDeltaPMeter (SURFHANDLE surf);
	void RedrawPanel_CryoTankIndicators (SURFHANDLE surf);
	void RedrawPanel_SuitComprDeltaPMeter (SURFHANDLE surf);
	void RedrawPanel_CabinIndicators (SURFHANDLE surf);
	void RedrawPanel_O2CryoTankPressureIndicator(SURFHANDLE surf, SURFHANDLE needle, double value, int xOffset, int xNeedle);
	void RedrawPanel_FuelCellIndicators (SURFHANDLE surf);
	void CryoTankHeaterSwitchToggled(ToggleSwitch *s, int *pump);
	void FuelCellHeaterSwitchToggled(ToggleSwitch *s, int *pump);
	void FuelCellPurgeSwitchToggled(ToggleSwitch *s, int *start);
	void FuelCellReactantsSwitchToggled(ToggleSwitch *s, int *start);
	void FuelCellPumpsSwitchToggled(ToggleSwitch *s, int *pump);
	double SetPitchApo();
	void SetStage(int s);
	void setupSM(OBJHANDLE hvessel);
	void initSaturn();
	void SwitchClick();
	void ProbeSound();
	void CabinFanSound();
	void StopCabinFanSound();
	void ButtonClick();
	void GuardClick();
	void FuelCell();
	void SetView();
	void SetView(double offset);
	void MasterAlarm();
	void StopMasterAlarm();
	void GenericTimestep(double simt, double simdt);
	void SystemsInit();
	void SystemsTimestep(double simt);
	void SIVBBoiloff();
	void SetSIVBThrusters();
	void LimitSetThrusterDir (THRUSTER_HANDLE th, const VECTOR3 &dir);
	void AttitudeLaunchSIVB();
	virtual void AutoPilot(double autoT) = 0;
	virtual void SeparateStage (int stage) = 0;
	void StageOrbitSIVB(double simt);
	void JostleViewpoint(double amount);
	double CalculateApogeeTime();
	void SlowIfDesired();
	void UpdatePayloadMass();
	double GetCPitch(double t);
	double GetJ2ISP(double ratio);
	void StartAbort();

	//
	// Systems functions.
	//

	bool AutopilotActive();
	bool CabinFansActive();
	void ActivateS4RCS();
	void DeactivateS4RCS();
	void ActivateCSMRCS();
	void DeactivateCSMRCS();
	bool SMRCSActive();
	void ActivateSPS();
	void DeactivateSPS();
	void CheckSPSState();
	void ActivateCMRCS();
	void FuelCellCoolingBypass(int fuelcell, bool bypassed);
	bool FuelCellCoolingBypassed(int fuelcell);
	void SetRandomFailures();

	//
	// Save/Load support functions.
	//

	int GetCSwitchState();
	void SetCSwitchState(int s);
	int GetSSwitchState();
	void SetSSwitchState(int s);
	int GetLPSwitchState();
	void SetLPSwitchState(int s);
	int GetRPSwitchState();
	void SetRPSwitchState(int s);
	int GetCP2SwitchState();
	void SetCP2SwitchState(int s);
	int GetCP3SwitchState();
	void SetCP3SwitchState(int s);
	int GetCP4SwitchState();
	void SetCP4SwitchState(int s);
	int GetCP5SwitchState();
	void SetCP5SwitchState(int s);
	int GetCP6SwitchState();
	void SetCP6SwitchState(int s);
	int GetMainState();
	void SetMainState(int s);
	int GetLaunchState();
	void SetLaunchState(int s);
	int GetA13State();
	void SetA13State(int s);
	int GetLightState();
	void SetLightState(int s);
	void GenericLoadStateSetup();

	virtual void SetVehicleStats() = 0;
	virtual void CalculateStageMass () = 0;
	virtual void SaveVehicleStats(FILEHANDLE scn) = 0;

	void GetScenarioState (FILEHANDLE scn, void *status);

	//
	// Mission stage functions.
	//

	void DoLaunch(double simt);

	void LaunchCountdown(double simt);
	void StageSeven(double simt);
	void StageEight(double simt);
	void SetChuteStage1();
	void SetChuteStage2();
	void SetChuteStage3();
	void SetChuteStage4();
	void SetSplashStage();
	void SetAbortStage ();
	void SetCSMStage ();
	void SetCSM2Stage ();
	void SetReentryStage ();
	void AddRCS_CM(double MaxThrust);
	void SetRCS_CM();
	void GenericTimestepStage(double simt, double simdt);
	bool CheckForLaunchShutdown();
	void SetGenericStageState();
	void DestroyStages(double simt);

	void LoadDefaultSounds();

	//
	// Sounds
	//

	Sound Sclick;
	Sound Bclick;
	Sound Gclick;
	Sound SMasterAlarm;
	Sound LaunchS;
	Sound Scount;
	Sound SplashS;
	Sound Swater;
	Sound PostSplashdownS;
	Sound TowerJS;
	Sound StageS;
	Sound Sctdw;
	Sound SeparationS;
	Sound SMJetS;
	Sound S4CutS;
	Sound SShutS;
	Sound S2ShutS;
	Sound SepS;
	Sound CrashBumpS;
	Sound Psound;
	Sound CabinFans;
	Sound SwindowS;
	Sound SKranz;
	Sound SExploded;
	Sound SApollo13;
	Sound SSMSepExploded;
	Sound SPUShiftS;

	//
	// General engine resources.
	//

	PROPELLANT_HANDLE ph_1st, ph_2nd, ph_3rd,ph_rcs0,ph_rcs1,ph_sps; // handles for propellant resources
	PROPELLANT_HANDLE ph_retro1,ph_retro2;

	THGROUP_HANDLE thg_main,thg_ull,thg_ver;		          // handles for thruster groups
	THGROUP_HANDLE thg_retro1, thg_retro2, thg_aps;

	THRUSTER_HANDLE th_main[5],th_ull[8],th_ver[3] ,th_att_cm[12];               // handles for orbiter main engines
	THRUSTER_HANDLE th_sps[1],th_att_rot[24], th_att_lin[24];                 // handles for SPS engines
	THRUSTER_HANDLE	th_retro1[4],th_retro2[4], th_aps[3];

	//
	// LEM data.
	//

	double LMLandingLatitude;
	double LMLandingLongitude;
	double LMLandingAltitude;

	//
	// Earth landing data.
	//

	double EarthLandingLatitude;
	double EarthLandingLongitude;

	//
	// Random motion.
	//

	double ViewOffsetx;
	double ViewOffsety;
	double ViewOffsetz;

	//
	// Save the last view offset.
	//

	double CurrentViewOffset;

	//
	// Time of last timestep call.
	//

	double LastTimestep;

	//
	// Audio language.
	//

	char AudioLanguage[64];

	//
	// PanelSDK functions as a interface between the
	// actual System & Panel SDK and VESSEL class
	//
    PanelSDK Panelsdk;
	// FILE *PanelsdkLogFile;

	// InitSaturn is called twice, but some things must run only once
	bool InitSaturnCalled;
};

extern void BaseInit();
extern void SaturnInitMeshes();
extern void StageTransform(VESSEL *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel);

const double STG2O = 8;
const double SMVO = -0.14;
const VECTOR3 OFS_MAINCHUTE =  { 0, -2, 9};

extern MESHHANDLE hSM;
extern MESHHANDLE hCM;
extern MESHHANDLE hFHC;
extern MESHHANDLE hCMP;
extern MESHHANDLE hCREW;
extern MESHHANDLE hSMhga;
extern MESHHANDLE hprobe;
extern MESHHANDLE hsat5tower;
extern MESHHANDLE hFHO2;

extern void SetupgParam(HINSTANCE hModule);
extern void DeletegParam();

#endif