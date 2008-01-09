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
  *	Revision 1.11  2007/12/21 18:10:27  movieman523
  *	Revised docking connector code; checking in a working version prior to a rewrite to automate the docking process.
  *	
  *	Revision 1.10  2007/12/21 01:00:09  movieman523
  *	Really basic Checklist MFD based on Project Apollo MFD, along with the various support functions required to make it work.
  *	
  *	Revision 1.9  2007/11/30 17:46:32  movieman523
  *	Implemented remaining meters as 0-5V voltmeters for now.
  *	
  *	Revision 1.8  2007/11/30 16:40:40  movieman523
  *	Revised LEM to use generic voltmeter and ammeter code. Note that the ED battery select switch needs to be implemented to fully support the voltmeter/ammeter now.
  *	
  *	Revision 1.7  2007/11/25 06:55:42  movieman523
  *	Tidied up surface ID code, moving the enum from a shared include file to specific versions for the Saturn and LEM classes.
  *	
  *	Revision 1.6  2007/06/06 15:02:09  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.5  2006/08/21 03:04:38  dseagrav
  *	This patch adds DC volt/amp meters and associated switches, which was an unholy pain in the
  *	
  *	Revision 1.4  2006/08/20 08:28:06  dseagrav
  *	LM Stage Switch actually causes staging (VERY INCOMPLETE), Incorrect "Ascent RCS" removed, ECA outputs forced to 24V during initialization to prevent IMU/LGC failure on scenario load, Valves closed by default, EDS saves RCS valve states, would you like fries with that?
  *	
  *	Revision 1.3  2006/08/18 05:45:01  dseagrav
  *	LM EDS now exists. Talkbacks wired to a power source will revert to BP when they lose power.
  *	
  *	Revision 1.2  2006/08/13 23:12:41  dseagrav
  *	Joystick improvements
  *	
  *	Revision 1.1  2006/08/13 16:01:53  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  **************************************************************************/

#if !defined(_PA_LEM_H)
#define _PA_LEM_H

#include "FDAI.h"

// DS20060413 Include DirectInput
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
// DS20060730 Include LM SCS
#include "lmscs.h"

// Cosmic background temperature in degrees F
#define CMBG_TEMP -459.584392

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

#include "connector.h"
#include "checklistController.h"

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
	void Init(LEM *s,e_object *hi_a,e_object *hi_b,e_object *lo_a,e_object *lo_b); // Init
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	IndicatorSwitch *dc_source_a_tb;    // Pointer at TB #1
	IndicatorSwitch *dc_source_b_tb;    // Pointer at TB #2
	IndicatorSwitch *dc_source_c_tb;    // Pointer at TB #3 (LUNAR STAY battery for later model LM)
	LEM *lem;					// Pointer at LEM
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
	void Init(LEM *s);
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	int active;
	LEM *lem;					// Pointer at LM
	e_object *dc_input;
};

// EXPLOSIVE DEVICES SYSTEM
class LEM_EDS{
public:
	LEM_EDS();							// Cons
	void Init(LEM *s); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep();
	LEM *lem;					// Pointer at LEM
	bool LG_Deployed;           // Landing Gear Deployed Flag	
};


///
/// \ingroup LEM
///
class LEM : public ProjectApolloConnectorVessel, public PanelSwitchListener {

public:

	///
	/// This enum gives IDs for the surface bitmaps. We don't use #defines because we want
	/// to automatically calculate the maximum number of bitmaps.
	///
	/// Note that this is copied from the Saturn code, so many of these values aren't actually
	/// needed for the LEM. Tidy it up later as appropriate.
	///
	/// \ingroup LEM
	///
	enum SurfaceID
	{
		//
		// First value in the enum must be set to one. Entry zero is not
		// used.
		//
		SRF_INDICATOR							=	 1,
		SRF_NEEDLE,
		SRF_DIGITAL,
		SRF_SWITCHUP,
		SRF_SWITCHLEVER,
		SRF_SWITCHGUARDS,
		SRF_ABORT,
		SRF_ALTIMETER,
		SRF_THRUSTMETER,
		SRF_SEQUENCERSWITCHES,
		SRF_LMTWOPOSLEVER,
		SRF_MASTERALARM_BRIGHT,
		SRF_DSKY,
		SRF_THREEPOSSWITCH,
		SRF_MFDFRAME,
		SRF_MFDPOWER,
		SRF_ROTATIONALSWITCH,
		SRF_SUITCABINDELTAPMETER,
		SRF_THREEPOSSWITCH305,
		SRF_LMABORTBUTTON,
		SRF_LMMFDFRAME,
		SRF_LMTHREEPOSLEVER,
		SRF_LMTHREEPOSSWITCH,
		SRF_DSKYDISP,
		SRF_FDAI,
		SRF_FDAIROLL,
		SRF_CWSLIGHTS,
		SRF_EVENT_TIMER_DIGITS,
		SRF_DSKYKEY,
		SRF_ECSINDICATOR,
		SRF_SWITCHUPSMALL,
		SRF_CMMFDFRAME,
		SRF_COAS,
		SRF_THUMBWHEEL_SMALLFONTS,
		SRF_CIRCUITBRAKER,
		SRF_THREEPOSSWITCH20,
		SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL,
		SRF_THREEPOSSWITCH30,
		SRF_SWITCH20,
		SRF_SWITCH30,
		SRF_SWITCH20LEFT,
		SRF_THREEPOSSWITCH20LEFT,
		SRF_GUARDEDSWITCH20,
		SRF_SWITCHGUARDPANEL15,
		SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT48,
		SRF_THREEPOSSWITCH30LEFT,
		SRF_SWITCH30LEFT,
		SRF_THREEPOSSWITCH305LEFT,
		SRF_SWITCH305LEFT,
		SRF_FDAIPOWERROTARY,
		SRF_DIRECTO2ROTARY,
		SRF_ECSGLYCOLPUMPROTARY,
		SRF_GTACOVER,
		SRF_DCVOLTS,
		SRF_ACVOLTS,
		SRF_DCAMPS,
		SRF_LMYAWDEGS,
		SRF_LMPITCHDEGS,
		SRF_LMSIGNALSTRENGTH,
		SRF_POSTLDGVENTVLVLEVER,
		SRF_SPSMAXINDICATOR,
		SRF_ECSROTARY,
		SRF_GLYCOLLEVER,
		SRF_LEMROTARY,
		SRF_FDAIOFFFLAG,
		SRF_FDAINEEDLES,
		SRF_THUMBWHEEL_LARGEFONTS,
		SRF_SPS_FONT_WHITE,
		SRF_SPS_FONT_BLACK,
		SRF_BORDER_34x29,
		SRF_BORDER_34x61,
		SRF_BORDER_55x111,
		SRF_BORDER_46x75,
		SRF_BORDER_39x38,
		SRF_BORDER_92x40,
		SRF_BORDER_34x33,
		SRF_BORDER_29x29,
		SRF_BORDER_34x31,
		SRF_BORDER_50x158,
		SRF_BORDER_38x52,
		SRF_BORDER_34x34,
		SRF_BORDER_90x90,
		SRF_BORDER_84x84,
		SRF_BORDER_70x70,
		SRF_BORDER_23x20,
		SRF_BORDER_78x78,
		SRF_BORDER_32x160,
		SRF_BORDER_72x72,
		SRF_BORDER_75x64,
		SRF_BORDER_34x39,
		SRF_THUMBWHEEL_SMALL,
		SRF_THUMBWHEEL_LARGEFONTSINV,
		SRF_SWLEVERTHREEPOS,
		SRF_LEM_COAS1,
		SRF_LEM_COAS2,
		SRF_THC,
		SRF_SUITRETURN_LEVER,
		SRF_OPTICS_HANDCONTROLLER,
		SRF_MARK_BUTTONS,
		SRF_THREEPOSSWITCHSMALL,

		//
		// NSURF MUST BE THE LAST ENTRY HERE. PUT ANY NEW SURFACE IDS ABOVE THIS LINE
		//
		nsurf	///< nsurf gives the count of surfaces for the array size calculation.
	};

	LEM(OBJHANDLE hObj, int fmodel);
	virtual ~LEM();

	void Init();
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

	PROPELLANT_HANDLE ph_RCSA,ph_RCSB;   // RCS Fuel A and B, replaces ph_rcslm0
	PROPELLANT_HANDLE ph_Dsc, ph_Asc; // handles for propellant resources
	THRUSTER_HANDLE th_hover[2];               // handles for orbiter main engines,added 2 for "virtual engine"
	// There are 16 RCS. 4 clusters, 4 per cluster.
	THRUSTER_HANDLE th_rcs[16];
	// These RCSes are for Orbiter's use and should be deleted once the internal guidance is working.
	THRUSTER_HANDLE th_rcs_orbiter_rot[24];
	THRUSTER_HANDLE th_rcs_orbiter_lin[16];
	THGROUP_HANDLE thg_hover;		          // handles for thruster groups
	SURFHANDLE exhaustTex;

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
	int thc_tjt_id;                       // Flag to use axis as TTCA THROTTLE/JETS select lever
	int rhc_debug;						  // Flags to print debugging messages.
	int thc_debug;
	int rhc_pos[3];                       // RHC x/y/z positions
	int ttca_mode;                        // TTCA Throttle/Jets Mode
#define TTCA_MODE_THROTTLE 0
#define TTCA_MODE_JETS 1
	int ttca_throttle_pos;                // TTCA THROTTLE-mode position

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
	void AttitudeLaunch1();
	void SeparateStage (UINT stage);
	void InitPanel (int panel);
	void SetSwitches(int panel);
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
	ThreePosSwitch ModeSelSwitch;
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

	//
	// Currently these are just 0-5V meters; at some point we may want
	// to change them to a different class.
	//

	SwitchRow RaderSignalStrengthMeterRow;
	DCVoltMeter RadarSignalStrengthMeter;

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

	SwitchRow Panel11CB3SwitchRow;
	CircuitBrakerSwitch EDS_CB_LOGIC_A;
	CircuitBrakerSwitch EDS_CB_LG_FLAG;

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

	/////////////////
	// LEM Panel 8 //
	/////////////////

	SwitchRow Panel8SwitchRow;
	ToggleSwitch EDMasterArm;
	ToggleSwitch EDDesVent;
	ThreePosSwitch EDASCHeSel;
	ToggleSwitch EDDesPrpIsol;
	ToggleSwitch EDLGDeploy;
	ToggleSwitch EDHePressRCS;
	ToggleSwitch EDHePressDesStart;
	ToggleSwitch EDHePressASC;
	ToggleSwitch EDStage;
	ToggleSwitch EDStageRelay;
	ThreePosSwitch EDDesFuelVent;
	ThreePosSwitch EDDesOxidVent;
	IndicatorSwitch EDLGTB;
	IndicatorSwitch EDDesFuelVentTB;
	IndicatorSwitch EDDesOxidVentTB;

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

	SwitchRow EPSP14VoltMeterSwitchRow;
	DCVoltMeter EPSDCVoltMeter;

	VoltageAttenuator ACVoltsAttenuator;

	SwitchRow EPSP14AmMeterSwitchRow;
	DCAmpMeter EPSDCAmMeter;
	
	SwitchRow EPSLeftControlArea;
	PowerStateRotationalSwitch EPSMonitorSelectRotary;
	LEMInverterSwitch EPSInverterSwitch;
	ThreePosSwitch EPSEDVoltSelect;

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

	//
	// Currently these are just 0-5V meters; at some point we may want
	// to change them to a different class.
	//
	SwitchRow ComPitchMeterRow;
	DCVoltMeter ComPitchMeter;

	SwitchRow ComYawMeterRow;
	DCVoltMeter ComYawMeter;

	SwitchRow Panel14SignalStrengthMeterRow;
	DCVoltMeter Panel14SignalStrengthMeter;

	//////////////////
	// LEM panel 16 //
	//////////////////

	SwitchRow Panel16CB2SwitchRow;
	CircuitBrakerSwitch EDS_CB_LOGIC_B;

	SwitchRow Panel16CB4SwitchRow;
	CircuitBrakerSwitch LMPInverter2CB;
	// Battery feed tie breakers (ECA output breakers)
	CircuitBrakerSwitch LMPBatteryFeedTieCB1;
	CircuitBrakerSwitch LMPBatteryFeedTieCB2;

	///////////////////////////
	// LEM Rendezvous Window //
	///////////////////////////

	int LEMCoas1Enabled;
	int LEMCoas2Enabled;

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

	// ChecklistController
	ChecklistController checkControl;

	SoundLib soundlib;

	Sound Sclick;
	Sound Bclick;
	Sound Gclick;
	Sound Rclick;
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

	//
	// Connectors.
	//

	///
	/// \brief MFD to panel connector.
	///
	PanelConnector MFDToPanelConnector;

	///
	/// \brief Connector from LEM to CSM when docked.
	///
	MultiConnector LEMToCSMConnector;

	PowerSourceConnectorObject CSMToLEMPowerSource;
	Connector CSMToLEMPowerConnector;

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

	// GNC
	ATCA atca;
	
	// EDS
	LEM_EDS eds;

	// Friend classes
	friend class ATCA;
	friend class LEM_EDS;
	friend class LEMDCVoltMeter;
	friend class LEMDCAmMeter;
};

extern void LEMLoadMeshes();
extern void InitGParam(HINSTANCE hModule);
extern void FreeGParam();

#endif
