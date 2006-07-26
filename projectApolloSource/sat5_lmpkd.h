/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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
  *	Revision 1.49  2006/07/24 06:41:30  dseagrav
  *	Many changes - Rearranged / corrected FDAI power usage, added LM AC equipment, many bugfixes
  *	
  *	Revision 1.48  2006/07/16 16:21:31  flydba
  *	COAS on the overhead rendezvous window now works.
  *	
  *	Revision 1.47  2006/07/16 02:55:55  flydba
  *	New COAS added on the rendezvous window.
  *	
  *	Revision 1.46  2006/06/26 19:05:36  movieman523
  *	More doxygen, made Lunar EVA a VESSEL2, made SM breakup, made LRV use VESSEL2 save/load functions.
  *	
  *	Revision 1.45  2006/06/18 22:45:31  dseagrav
  *	LM ECA bug fix, LGC,IMU,DSKY and IMU OPR wired to CBs, IMU OPR,LGC,FDAI,and DSKY draw power
  *	
  *	Revision 1.44  2006/06/18 16:43:07  dseagrav
  *	LM EPS fixes, LMP/CDR DC busses now powered thru CBs, ECA power-off bug fixed and ECA speed improvement
  *	
  *	Revision 1.43  2006/06/11 09:20:30  dseagrav
  *	LM ECA #2 added, ECA low-voltage tap usage added, CDR & LMP DC busses wired to ECAs
  *	
  *	Revision 1.42  2006/05/01 08:52:50  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.41  2006/04/23 04:15:46  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.40  2006/04/22 03:53:48  jasonims
  *	Began initial support for multiple EVA's (two astronauts), as well as improving upon the LRV controls.  No longer turns while standing still.  Throttle controlled via (NUM+ and NUM-).
  *	
  *	Revision 1.39  2006/04/17 15:16:16  movieman523
  *	Beginnings of checklist code, added support for flashing borders around control panel switches and updated a portion of the Saturn panel switches appropriately.
  *	
  *	Revision 1.38  2006/04/12 06:27:19  dseagrav
  *	LM checkpoint commit. The LM is not airworthy at this point. Please be patient.
  *	
  *	Revision 1.37  2006/03/08 02:24:21  movieman523
  *	Added event timer and fuel display.
  *	
  *	Revision 1.36  2006/03/07 02:20:27  flydba
  *	Circuit breakers added to panel 11.
  *	
  *	Revision 1.35  2006/03/04 13:40:34  flydba
  *	Switches added on LEM panels 3 & 4
  *	
  *	Revision 1.34  2006/01/29 00:48:36  flydba
  *	Switches added on LEM panel 2.
  *	
  *	Revision 1.33  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.32  2006/01/22 16:38:11  flydba
  *	Switches added on LEM panel 1.
  *	
  *	Revision 1.31  2006/01/19 14:55:13  tschachim
  *	Initial Meshland support.
  *	
  *	Revision 1.30  2006/01/18 20:12:54  flydba
  *	Contact lights added.
  *	
  *	Revision 1.29  2006/01/14 21:59:52  movieman523
  *	Added PanelSDK, init, timestep, save and load.
  *	
  *	Revision 1.28  2006/01/14 20:58:16  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.27  2006/01/09 21:56:44  movieman523
  *	Added support for LEM and CSM AGC PAD loads in scenario file.
  *	
  *	Revision 1.26  2005/11/28 01:13:54  movieman523
  *	Added LEM right-hand panel.
  *	
  *	Revision 1.25  2005/11/21 12:42:17  tschachim
  *	New LM landed and descent stage mesh.
  *	
  *	Revision 1.24  2005/11/16 20:21:39  movieman523
  *	CSM/LEM renaming changes.
  *	
  *	Revision 1.23  2005/10/31 10:36:32  tschachim
  *	New toggleswitch callback PanelThumbwheelSwitchChanged.
  *	
  *	Revision 1.22  2005/09/30 11:22:00  tschachim
  *	New panel event handler.
  *	
  *	Revision 1.21  2005/09/22 00:40:20  flydba
  *	AOT - Alignment Optical Telescope view and left instrument panel added to the lem
  *	
  *	Revision 1.20  2005/09/18 23:18:43  flydba
  *	Lem overhead rendezvous window added...
  *	
  *	Revision 1.19  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.18  2005/08/14 16:08:20  tschachim
  *	LM is now a VESSEL2
  *	Changed panel restore mechanism because the CSM mechanism
  *	caused CTDs, reason is still unknown.
  *	
  *	Revision 1.17  2005/08/13 20:20:17  movieman523
  *	Created MissionTimer class and wired it into the LEM and CSM.
  *	
  *	Revision 1.16  2005/08/11 16:29:33  spacex15
  *	Added PNGS and AGS mode control 3 pos switches
  *	
  *	Revision 1.15  2005/08/11 14:00:35  spacex15
  *	Added Descent Engine Command Override switch
  *	
  *	Revision 1.14  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.13  2005/08/10 20:00:55  spacex15
  *	Activated 3 position lem eng arm switch
  *	
  *	Revision 1.12  2005/08/09 09:17:29  tschachim
  *	Introduced toggleswitch lib
  *	
  *	Revision 1.11  2005/08/06 01:25:27  movieman523
  *	Added Realism variable to AGC and fixed a bug with the APOLLONO scenario entry in the saturn class.
  *	
  *	Revision 1.10  2005/08/01 21:51:11  lazyd
  *	Added code for Abort Stage
  *	
  *	Revision 1.9  2005/07/16 20:43:01  lazyd
  *	*** empty log message ***
  *	
  *	Revision 1.8  2005/07/14 10:06:14  spacex15
  *	Added full apollo11 landing sound
  *	initial release
  *	
  *	Revision 1.7  2005/07/06 14:33:56  lazyd
  *	Changed ConsumeDirectKey to get key events
  *	
  *	Revision 1.6  2005/06/15 20:32:32  lazyd
  *	Added two variables: bool InFOV and SaveFOV for save/restore of original view and FOV
  *	
  *	Revision 1.5  2005/06/09 14:17:22  lazyd
  *	Added SetGimbal function declaration
  *	
  *	Revision 1.4  2005/05/21 16:14:36  movieman523
  *	Pass Realism and AudioLanguage correctly from CSM to LEM.
  *	
  *	Revision 1.3  2005/04/11 23:46:17  yogenfrutz
  *	added InPanel and Panel_ID
  *	
  *	Revision 1.2  2005/02/24 00:27:28  movieman523
  *	Revisions to make LEVA sounds work.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

#if !defined(_PA_SAT5_LMPKD_H)
#define _PA_SAT5_LMPKD_H

#include "FDAI.h"

// DS20060413 Include DirectInput
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

//
// Valves.
//
#define N_LEM_VALVES	32

#define LEM_RCS_MAIN_SOV_A				1
#define LEM_RCS_MAIN_SOV_B				2

//
// Lem state settings from scenario file, passed from CSM.
//

#include "lemswitches.h"
#include "missiontimer.h"

typedef struct {

	double LandingLatitude;
	double LandingLongitude;
	double LandingAltitude;
	int MissionNo;
	double MissionTime;
	char language[64];
	char CSMName[64];
	bool Crewed;
	bool AutoSlow;
	int Realism;
	bool Yaagc;

} LemSettings;

// Systems things

// ELECTRICAL
// Electrical Control Assembly
class LEM_ECA : public e_object {
public:
	LEM_ECA();							// Cons
	void Init(sat5_lmpkd *s,e_object *hi_a,e_object *hi_b,e_object *lo_a,e_object *lo_b); // Init
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	IndicatorSwitch *dc_source_a_tb;    // Pointer at TB #1
	IndicatorSwitch *dc_source_b_tb;    // Pointer at TB #2
	IndicatorSwitch *dc_source_c_tb;    // Pointer at TB #3 (LUNAR STAY battery for later model LM)
	sat5_lmpkd *lem;					// Pointer at LEM
	e_object *dc_source_hi_a;			// An ECA has four inputs - Two HV inputs, and two LV inputs.
	e_object *dc_source_hi_b;
	e_object *dc_source_lo_a;
	e_object *dc_source_lo_b;
	int input_a;                        // A-side input selector
	int input_b;                        // B-side input selector
};

// Inverter
class LEM_INV : public e_object {
public:
	LEM_INV();							// Cons
	void Init(sat5_lmpkd *s);
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	int active;
	sat5_lmpkd *lem;					// Pointer at LM
	e_object *dc_input;
};


///
/// \ingroup LEM
///
class sat5_lmpkd : public VESSEL2, public PanelSwitchListener {

public:
	sat5_lmpkd(OBJHANDLE hObj, int fmodel);
	virtual ~sat5_lmpkd();

	void Init();
	void PostCreation();
	void SetStateEx(const void *status);
	void SetLmVesselDockStage();
	void SetLmVesselHoverStage();
	void SetLmAscentHoverStage();
	void SetLmLandedMesh();
	void SetGimbal(bool setting);
	void GetMissionTime(double &Met);
	void AbortStage();
	void StartAscent();
	void CheckRCS();

	bool clbkLoadPanel (int id);
	bool clbkLoadVC(int id);
	bool clbkPanelMouseEvent (int id, int event, int mx, int my);
	bool clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf);
	int  clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkPreStep (double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	void clbkLoadStateEx (FILEHANDLE scn, void *vs);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkLoadGenericCockpit ();
	void clbkMFDMode (int mfd, int mode);

	void PanelSwitchToggled(ToggleSwitch *s);
	void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s); 
	void PanelRotationalSwitchChanged(RotationalSwitch *s);
	void PanelThumbwheelSwitchChanged(ThumbwheelSwitch *s);

	// Panel SDK
	bool GetValveState(int valve);
	void SetValveState(int valve, bool open);

	// DS20060416 RCS management
	void SetRCSJet(int jet,bool fire);
	void SetRCSJetLevel(int jet, double level);
	//
	// These functions must be virtual so they can be called from the Saturn V or the LEVA
	//

	virtual void SetLanderData(LemSettings &ls);
	virtual void PadLoad(unsigned int address, unsigned int value);
	virtual void StopEVA();
	
	PROPELLANT_HANDLE ph_DscRCSA,ph_DscRCSB;   // Descent RCS A and B, replaces ph_rcslm0
	PROPELLANT_HANDLE ph_Dsc, ph_Asc ,ph_rcslm1; // handles for propellant resources
	THRUSTER_HANDLE th_hover[2];               // handles for orbiter main engines,added 2 for "virtual engine"
	// There should be only 16 RCS. 4 clusters, 4 per cluster.
	THRUSTER_HANDLE th_rcs[16];
	// These RCSes are for Orbiter's use and should be deleted once the internal guidance is working.
	THRUSTER_HANDLE th_rcs_orbiter_rot[24];
	// THRUSTER_HANDLE th_att_rot[24], th_att_lin[24];                 // handles for SPS engines
	THGROUP_HANDLE thg_hover;		          // handles for thruster groups

	// DS20060413 DirectInput stuff
	// Handle to DLL instance
	HINSTANCE dllhandle;
	// pointer to DirectInput class itself
	LPDIRECTINPUT8 dx8ppv;
	// Joysticks-Enabled flag / counter - Zero if we aren't using DirectInput, nonzero is the number of joysticks we have.
	int js_enabled;
	// Pointers to DirectInput joystick devices
	LPDIRECTINPUTDEVICE8 dx8_joystick[2]; // One for THC, one for RHC, ignore extras
	DIDEVCAPS			 dx8_jscaps[2];   // Joystick capabilities
	DIJOYSTATE2			 dx8_jstate[2];   // Joystick state
	HRESULT				 dx8_failure;     // DX failure reason
	int rhc_id;							  // Joystick # for the RHC
	int rhc_rot_id;						  // ID of ROTATOR axis to use for RHC Z-axis
	int rhc_sld_id;                       // ID of SLIDER axis to use for RHC Z-axis
	int rhc_rzx_id;                       // Flag to use native Z-axis as RHC Z-axis
	int thc_id;                           // Joystick # for the THC
	int thc_rot_id;						  // ID of ROTATOR axis to use for THC Z-axis
	int thc_sld_id;                       // ID of SLIDER axis to use for THC Z-axis
	int thc_rzx_id;                       // Flag to use native Z-axis as THC Z-axis	
	int rhc_debug;						  // Flags to print debugging messages.
	int thc_debug;
	int rhc_pos[3];                       // RHC x/y/z positions

protected:

	//
	// PanelSDK functions as a interface between the
	// actual System & Panel SDK and VESSEL class
	//
	// Note that this must be defined early in the file, so it will be initialised
	// before any other classes which rely on it at creation. Don't move it further
	// down without good reason, or you're likely to crash!
	//

    PanelSDK Panelsdk;

	void RedrawPanel_Thrust (SURFHANDLE surf);
	void RedrawPanel_XPointer (SURFHANDLE surf);
	void RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset);
	void MousePanel_MFDButton(int mfd, int event, int mx, int my);
	void ReleaseSurfaces ();
	void ResetThrusters();
	// void SetRCS(PROPELLANT_HANDLE ph_prop); OBSOLETED DS20060410	
	void AttitudeLaunch1();
	void SeparateStage (UINT stage);
	void InitPanel (int panel);
	void SetSwitches(int panel);
	// These two not used! DS20060413
	/* void AddRCS_LM(double TRANZ);
	void AddRCS_LM2(double TRANZ); */
	void AddRCS_LMH(double TRANZ);
	void AddRCS_LMH2(double TRANZ);
	void ToggleEVA();
	void SetupEVA();
	void SetView();
	void RedrawPanel_Horizon (SURFHANDLE surf);
	void SwitchClick();
	void CabinFanSound();
	void VoxSound();
	void ButtonClick();
	void GuardClick();
	void AbortFire();
	void InitPanel();
	void DoFirstTimestep();
	void LoadDefaultSounds();
	void GetDockStatus();

	bool CabinFansActive();
	bool AscentEngineArmed();
	bool AscentRCSArmed();

	void SystemsTimestep(double simt, double simdt);
	void SystemsInit();

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

	SURFHANDLE srf[nsurf];  // handles for panel bitmaps

	double actualVEL;
	double actualALT;
	double actualFUEL;
	double aVAcc;
	double aVSpeed;
	double aHAcc;
	double ALTN1;
	double SPEEDN1;
	double VSPEEDN1;
	double aTime;
	double AtempP ;
	double AtempY ;
	double AtempR ;
	double MissionTime;

	// Panel components
	PanelSwitches MainPanel;
	PanelSwitchScenarioHandler PSH;

	SwitchRow AbortSwitchesRow;

	PushSwitch AbortSwitch;
	PushSwitch AbortStageSwitch;
	bool AbortStageSwitchLight;

	/////////////////
	// LEM panel 1 //
	/////////////////

	FDAI fdaiLeft;
	int fdaiDisabled;
	int fdaiSmooth;

	HBITMAP hBmpFDAIRollIndicator;

	SwitchRow LeftXPointerSwitchRow;
	ToggleSwitch LeftXPointerSwitch;

	SwitchRow GuidContSwitchRow;
	ToggleSwitch GuidContSwitch;
	ToggleSwitch ModeSelSwitch;
	ToggleSwitch AltRngMonSwitch;

	SwitchRow LeftMonitorSwitchRow;
	ToggleSwitch RateErrorMonSwitch;
	ToggleSwitch AttitudeMonSwitch;

	SwitchRow FDAILowerSwitchRow;
	ToggleSwitch ShiftTruSwitch;
	ToggleSwitch RateScaleSwitch;
	ToggleSwitch ACAPropSwitch;
	
	SwitchRow EngineThrustContSwitchRow;
	ToggleSwitch THRContSwitch;
	ToggleSwitch MANThrotSwitch;
	ToggleSwitch ATTTranslSwitch;
	ToggleSwitch BALCPLSwitch;

	SwitchRow PropellantSwitchRow;
	ThreePosSwitch QTYMonSwitch;
	ThreePosSwitch TempPressMonSwitch;

	SwitchRow HeliumMonRotaryRow;
	RotationalSwitch HeliumMonRotary;

	/////////////////
	// LEM panel 2 //
	/////////////////

	SwitchRow RightMonitorSwitchRow;
	ToggleSwitch RightRateErrorMonSwitch;
	ToggleSwitch RightAttitudeMonSwitch;

	SwitchRow TempPressMonRotaryRow;
	RotationalSwitch TempPressMonRotary;

	// DS20060406 RCS MAIN SHUTOFF VALVES
	SwitchRow RCSMainSOVTBRow;
	LEMValveTalkback RCSMainSovATB;
	LEMValveTalkback RCSMainSovBTB;

	SwitchRow RCSMainSOVSwitchRow;
	LEMValveSwitch RCSMainSovASwitch;
	LEMValveSwitch RCSMainSovBSwitch;
	LEMValveTalkback RCSMainSovATalkback;
	LEMValveTalkback RCSMainSovBTalkback;

	SwitchRow RightACAPropSwitchRow;
	ToggleSwitch RightACAPropSwitch;

	SwitchRow ClycolSuitFanRotaryRow;
	RotationalSwitch ClycolRotary;
	RotationalSwitch SuitFanRotary;

	SwitchRow QtyMonRotaryRow;
	RotationalSwitch QtyMonRotary;

	/////////////////
	// LEM panel 3 //
	/////////////////
	
	SwitchRow EngineArmSwitchesRow;
	ThreePosSwitch EngineArmSwitch;

	SwitchRow EngineDescentCommandOverrideSwitchesRow;
	ToggleSwitch EngineDescentCommandOverrideSwitch;

	SwitchRow ModeControlSwitchesRow;
	PGNSSwitch ModeControlPNGSSwitch;
	ThreePosSwitch ModeControlAGSSwitch;
    UnguardedIMUCageSwitch IMUCageSwitch;

	SwitchRow RadarAntTestSwitchesRow;
	ThreePosSwitch LandingAntSwitch;
	ThreePosSwitch RadarTestSwitch;

	SwitchRow TestMonitorRotaryRow;
	RotationalSwitch TestMonitorRotary;

	SwitchRow SlewRateSwitchRow;
	ToggleSwitch SlewRateSwitch;

	SwitchRow RendezvousRadarRotaryRow;
	RotationalSwitch RendezvousRadarRotary;

	SwitchRow StabContSwitchesRow;
	ToggleSwitch DeadBandSwitch;
	ThreePosSwitch GyroTestLeftSwitch;
	ThreePosSwitch GyroTestRightSwitch;

	SwitchRow AttitudeControlSwitchesRow;
	ThreePosSwitch RollSwitch;
	ThreePosSwitch PitchSwitch;
	ThreePosSwitch YawSwitch;

	SwitchRow TempMonitorRotaryRow;
	RotationalSwitch TempMonitorRotary;

	SwitchRow RCSSysQuadSwitchesRow;
	ThreePosSwitch RCSSysQuad1Switch;
	ThreePosSwitch RCSSysQuad2Switch;
	ThreePosSwitch RCSSysQuad3Switch;
	ThreePosSwitch RCSSysQuad4Switch;

	SwitchRow LightingSwitchesRow;
	ToggleSwitch SidePanelsSwitch;
	ThreePosSwitch FloodSwitch;

	SwitchRow FloodRotaryRow;
	RotationalSwitch FloodRotary;

	SwitchRow LampToneTestRotaryRow;
	RotationalSwitch LampToneTestRotary;

	SwitchRow RightXPointerSwitchRow;
	ToggleSwitch RightXPointerSwitch;

	SwitchRow ExteriorLTGSwitchRow;
	ThreePosSwitch ExteriorLTGSwitch;

	/////////////////
	// LEM panel 4 //
	/////////////////

	SwitchRow Panel4LeftSwitchRow;
	ToggleSwitch LeftACA4JetSwitch;
	ToggleSwitch LeftTTCATranslSwitch;

	SwitchRow Panel4RightSwitchRow;
	ToggleSwitch RightACA4JetSwitch;
	ToggleSwitch RightTTCATranslSwitch;

	//////////////////
	// LEM panel 11 //
	//////////////////

	SwitchRow Panel11CB1SwitchRow;
	// I have to get to these from the inverter select switch class
	public:
	CircuitBrakerSwitch AC_A_INV_1_FEED_CB;
	CircuitBrakerSwitch AC_A_INV_2_FEED_CB;
	CircuitBrakerSwitch AC_B_INV_1_FEED_CB;
	CircuitBrakerSwitch AC_B_INV_2_FEED_CB;
	protected:

	SwitchRow Panel11CB2SwitchRow;
	CircuitBrakerSwitch CDR_FDAI_DC_CB;
	CircuitBrakerSwitch CDR_FDAI_AC_CB;

	SwitchRow Panel11CB4SwitchRow;
	CircuitBrakerSwitch IMU_OPR_CB;
	CircuitBrakerSwitch IMU_SBY_CB;
	CircuitBrakerSwitch LGC_DSKY_CB;

	SwitchRow Panel11CB5SwitchRow;
	// Battery feed tie breakers (ECA output breakers)
	CircuitBrakerSwitch CDRBatteryFeedTieCB1;
	CircuitBrakerSwitch CDRBatteryFeedTieCB2;
	// AC Inverter 1 feed
	CircuitBrakerSwitch CDRInverter1CB;

	bool RCS_Full;
	bool Eds;

	bool toggleRCS;

	bool Cswitch1;
	bool Cswitch2;
	bool Cswitch3;
	bool Cswitch4;
	bool Cswitch5;
	bool Cswitch6;
	bool Cswitch7;
	bool Cswitch8;
	bool Cswitch9;

	bool Sswitch1;
	bool Sswitch2;
	bool Sswitch3;
	bool Sswitch4;
	bool Sswitch5;
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
	bool RPswitch10;
	bool RPswitch11;
	bool RPswitch12;
	bool RPswitch13;
	bool RPswitch14;
	bool RPswitch15;
	bool RPswitch16;
	bool RPswitch17;

	bool LPswitch1;
	bool LPswitch2;
	bool LPswitch3;
	bool LPswitch4;
	bool LPswitch5;
	bool LPswitch6;
	bool LPswitch7;
	bool SPSswitch;
	bool EDSswitch;

	bool DESHE1switch;
	bool DESHE2switch;

//	int ENGARMswitch;
	

	bool QUAD1switch;
	bool QUAD2switch;
	bool QUAD3switch;
	bool QUAD4switch;
	bool QUAD5switch;
	bool QUAD6switch;
	bool QUAD7switch;
	bool QUAD8switch;

	bool AFEED1switch;
	bool AFEED2switch;
	bool AFEED3switch;
	bool AFEED4switch;

	bool LDGswitch;

	bool ED1switch;
	bool ED2switch;
	bool ED4switch;
	bool ED5switch;
	bool ED6switch;

	bool ED7switch;
	bool ED8switch;
	bool ED9switch;

	bool GMBLswitch;

	bool ASCHE1switch;
	bool ASCHE2switch;

	bool RCSQ1switch;
	bool RCSQ2switch;
	bool RCSQ3switch;
	bool RCSQ4switch;

	bool ATT1switch;
	bool ATT2switch;
	bool ATT3switch;

	bool CRSFDswitch;

	bool CABFswitch;

	bool PTTswitch;

	bool RCSS1switch;
	bool RCSS2switch;
	bool RCSS3switch;
	bool RCSS4switch;

	bool X1switch;

	bool GUIDswitch;

	bool ALTswitch;

	bool RATE1switch;
	bool AT1switch;

	bool SHFTswitch;

	bool ETC1switch;
	bool ETC2switch;
	bool ETC3switch;
	bool ETC4switch;

	bool PMON1switch;
	bool PMON2switch;

	bool ACAPswitch;

	bool RATE2switch;
	bool AT2switch;

//	bool DESEswitch;
#define DESEswitch EngineDescentCommandOverrideSwitch.IsUp()

#define ModeControlPNGSAuto ModeControlPNGSSwitch.IsUp()
#define ModeControlPNGSAttHold ModeControlPNGSSwitch.IsCenter()
#define ModeControlPNGSOff ModeControlPNGSSwitch.IsDown()

#define ModeControlAGSAuto ModeControlAGSSwitch.IsUp()
#define ModeControlAGSAttHold ModeControlAGSSwitch.IsCenter()
#define ModeControlAGSOff ModeControlAGSSwitch.IsDown()

	bool SLWRswitch;

	bool DBswitch;

	bool IMUCswitch;

	bool SPLswitch;

	bool X2switch;

	bool P41switch;
	bool P42switch;
	bool P43switch;
	bool P44switch;

	bool AUDswitch;
	bool RELswitch;

	bool CPswitch;

	bool HATCHswitch;

	bool EVAswitch;

	bool COASswitch;

	//////////////////
	// LEM panel 14 //
	//////////////////

	SwitchRow EPSLeftControlArea;
	LEMInverterSwitch EPSInverterSwitch;

	SwitchRow DSCHiVoltageSwitchRow;
	LEMBatterySwitch DSCSEBat1HVSwitch;
	LEMBatterySwitch DSCSEBat2HVSwitch;
	LEMBatterySwitch DSCCDRBat3HVSwitch;
	LEMBatterySwitch DSCCDRBat4HVSwitch;	

	SwitchRow DSCLoVoltageSwitchRow;
	LEMBatterySwitch DSCSEBat1LVSwitch;
	LEMBatterySwitch DSCSEBat2LVSwitch;
	LEMBatterySwitch DSCCDRBat3LVSwitch;
	LEMBatterySwitch DSCCDRBat4LVSwitch;	

	SwitchRow DSCBatteryTBSwitchRow;
	IndicatorSwitch DSCBattery1TB;
	IndicatorSwitch DSCBattery2TB;
	IndicatorSwitch DSCBattery3TB;
	IndicatorSwitch DSCBattery4TB;

	//////////////////
	// LEM panel 16 //
	//////////////////

	SwitchRow Panel16CB4SwitchRow;
	CircuitBrakerSwitch LMPInverter2CB;
	// Battery feed tie breakers (ECA output breakers)
	CircuitBrakerSwitch LMPBatteryFeedTieCB1;
	CircuitBrakerSwitch LMPBatteryFeedTieCB2;

	///////////////////////////
	// LEM Rendezvous Window //
	///////////////////////////

	int LEMCoas1Enabled;

	bool FirstTimestep;

	bool LAUNCHIND[8];
	bool ABORT_IND;
	bool ENGIND[7];

	double countdown;

	bool bToggleHatch;
	bool bModeDocked;
	bool bModeHover;
	bool HatchOpen;
	bool bManualSeparate;
	bool ToggleEva;
	bool CDREVA_IP;

#define LMVIEW_CDR		0
#define LMVIEW_LMP		1

	int	viewpos;
	
	bool startimer;
	bool ContactOK;
	bool SoundsLoaded;

	bool Crewed;
	bool AutoSlow;

	OBJHANDLE hLEVA;
	OBJHANDLE hdsc;
	UINT stage;
	int status;

	//
	// Panel flash.
	//

	double NextFlashUpdate;
	bool PanelFlashOn;

	int Realism;
	int ApolloNo;
	int Landed;

	int SwitchFocusToLeva;

	DSKY dsky;
	LEMcomputer agc;
	IMU imu;
	MissionTimer MissionTimerDisplay;
	LEMEventTimer EventTimerDisplay;

#define LMPANEL_MAIN			0
#define LMPANEL_RIGHTWINDOW		1
#define LMPANEL_LEFTWINDOW		2
#define LMPANEL_LPDWINDOW		3
#define LMPANEL_RNDZWINDOW		4
#define LMPANEL_LEFTPANEL		5
#define LMPANEL_AOTVIEW			6
#define LMPANEL_RIGHTPANEL		7

	bool InVC;
	bool InPanel;
	bool InFOV;  
	int  PanelId; 
	double SaveFOV;
	bool CheckPanelIdInTimestep;

	SoundLib soundlib;

	Sound Sclick;
	Sound Bclick;
	Sound Gclick;
	Sound LunarAscent;
	Sound StageS;
	Sound S5P100;
	Sound Scontact;
	Sound SDMode;
	Sound SHMode;
	Sound SLEVA;
	Sound SAbort;
	Sound CabinFans;
	Sound Vox;
	Sound Afire;
	Sound Slanding;

	char AudioLanguage[64];

	// New Panel SDK stuff
	int *pLEMValves[N_LEM_VALVES];
	bool ValveState[N_LEM_VALVES];

	// POWER AND SUCH

	// Descent batteries
	Battery *Battery1;
	Battery *Battery2;
	Battery *Battery3;
	Battery *Battery4;

	// Ascent batteries
	Battery *Battery5;
	Battery *Battery6;

	// Lunar Stay Battery
	Battery *LunarBattery;

	// ECA
	LEM_ECA ECA_1;
	LEM_ECA ECA_2;

	// CDR and LMP 28V DC busses
	DCbus CDRs28VBus;
	DCbus LMPs28VBus;

	// AC Bus A and B
	// This is a cheat. the ACbus class actually simulates an inverter, which is bad for the LM.
	// So we fake it out with a DC bus instead.
	// Also, I have to get to these from the inverter select switch class
	public:
	DCbus ACBusA;
	DCbus ACBusB;
	protected:

	// AC inverters
	LEM_INV INV_1;
	LEM_INV INV_2;
};

extern void LEMLoadMeshes();
extern void InitGParam(HINSTANCE hModule);
extern void FreeGParam();

#endif
