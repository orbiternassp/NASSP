/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005
  Copyright 2002-2005 Chris Knestrick


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

  **************************************************************************/

#if !defined(_PA_LEM_H)
#define _PA_LEM_H

#include "FDAI.h"
#include "ORDEAL.h"

// DS20060413 Include DirectInput
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"
#include "dsky.h"
#include "imu.h"
#include "cdu.h"
#include "lmscs.h"
#include "lm_ags.h"
#include "lm_telecom.h"
#include "pyro.h"
#include "lm_eds.h"
#include "lm_aps.h"
#include "lm_dps.h"
#include "lm_programer.h"
#include "lm_aca.h"

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
#include "MechanicalAccelerometer.h"
#include "connector.h"
#include "checklistController.h"
#include "payload.h"

// Systems things
// ELECTRICAL
// LEM to CSM Power Connector
class LEMPowerConnector : public Connector
{
public:
	LEMPowerConnector();
	int csm_power_latch;
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

// XLunar Bus Controller Voltage Source
class LEM_XLBSource : public e_object {
public:
	LEM_XLBSource();							// Cons
	void SetVoltage(double v);
	void DrawPower(double watts);
};


// XLunar Bus Controller
class LEM_XLBControl : public e_object {
public:
	LEM_XLBControl();	// Cons
	void Init(LEM *s);
	void UpdateFlow(double dt);
	void DrawPower(double watts);

	LEM *lem;					// Pointer at LEM
	LEM_XLBSource dc_output;	// DC output
};

// Electrical Control Assembly Subchannel
class LEM_ECAch : public e_object {
public:
	LEM_ECAch();								 // Cons
	void Init(LEM *s,e_object *src, int inp); // Init
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	IndicatorSwitch *dc_source_tb;    // Pointer at TB
	LEM *lem;					// Pointer at LEM
	e_object *dc_source;		// Associated battery
	int input;                  // Channel input selector
};

// Bus feed controller object
class LEM_BusFeed : public e_object {
public:
	LEM_BusFeed();							// Cons
	void Init(LEM *s,e_object *sra,e_object *srb); // Init
	void UpdateFlow(double dt);
	void DrawPower(double watts);

	LEM *lem;					// Pointer at LEM
	e_object *dc_source_a;		// This has two inputs.
	e_object *dc_source_b;
};

// Voltage source item for cross-tie balancer
class LEM_BCTSource : public e_object {
public:
	LEM_BCTSource();							// Cons
	void SetVoltage(double v);
};

// Bus cross-tie balancer object
class LEM_BusCrossTie : public e_object {
public:
	LEM_BusCrossTie();	// Cons
	void LEM_BusCrossTie::Init(LEM *s,DCbus *sra,DCbus *srb,CircuitBrakerSwitch *cb1,CircuitBrakerSwitch *cb2,CircuitBrakerSwitch *cb3,CircuitBrakerSwitch *cb4);
	void UpdateFlow(double dt);
	void DrawPower(double watts);

	LEM *lem;					// Pointer at LEM
	DCbus *dc_bus_cdr;
	DCbus *dc_bus_lmp;
	LEM_BCTSource dc_output_cdr;
	LEM_BCTSource dc_output_lmp;
	CircuitBrakerSwitch *lmp_bus_cb,*lmp_bal_cb;
	CircuitBrakerSwitch *cdr_bus_cb,*cdr_bal_cb;
	double last_cdr_ld;
	double last_lmp_ld;
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

// ENVIRONMENTAL CONTROL SYSTEM
class LEM_ECS{
public:
	LEM_ECS();
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	double AscentOxyTankPressure(int tank);
	double DescentOxyTankPressure(int tank);

	LEM *lem;					// Pointer at LEM
	double Cabin_Press,Cabin_Temp,Cabin_CO2;	// Cabin Atmosphere
	double Suit_Press,Suit_Temp,Suit_CO2;		// Suit Circuit Atmosphere
	double Asc_Water[2],Des_Water[2];			// Water tanks
	double Asc_Oxygen[2],Des_Oxygen[2];			// Oxygen tanks
	double Primary_CL_Glycol_Press[2];			// Pressure before and after pumps
	double Secondary_CL_Glycol_Press[2];		// Pressure before and after pumps
	double Primary_CL_Glycol_Temp[2];			// Teperature before and after pumps
	double Secondary_CL_Glycol_Temp[2];			// Teperature before and after pumps
	double Primary_Glycol_Accu;					// Glycol Accumulator
	double Secondary_Glycol_Accu;				// Glycol Accumulator
	double Primary_Glycol;						// Glycol in system
	double Secondary_Glycol;					// Glycol in system
	int Asc_H2O_To_PLSS,Des_H2O_To_PLSS;		// PLSS Water Fill valves
	int Water_Tank_Selector;					// WT selection valve
	int Pri_Evap_Flow_1,Pri_Evap_Flow_2;		// Primary evaporator flow valves
	int Sec_Evap_Flow;							// Secondary evaporator flow valve
	int Water_Sep_Selector;						// WS Select Valve
	int Asc_O2_To_PLSS,Des_O2_To_PLSS;			// PLSS Oxygen Fill Valves
	int Des_O2;									// Descent O2 Valve
	int Asc_O2[2];								// Ascent O2 Valves
	int Cabin_Repress;							// Cabin Repress Valve
	int CO2_Can_Select;
	int Suit_Gas_Diverter;
	int Suit_Circuit_Relief;
	int Suit_Isolation[2];						// CDR and LMP suit isolation valves
};

// Landing Radar
class LEM_LR : public e_object{
public:
	LEM_LR();
	void Init(LEM *s,e_object *dc_src, h_Radiator *ant, Boiler *anheat);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	double GetAntennaTempF();
	bool IsRangeDataGood() { return rangeGood == 1; };
	bool IsVelocityDataGood() { return velocityGood == 1; };
	double GetAltitude() { return range*0.3048; };
	double GetAltitudeRate() { return rate[0]*0.3048; };
	double lastTemp;

	bool IsPowered(); 

	LEM *lem;					// Pointer at LEM
	h_Radiator *antenna;		// Antenna (loses heat into space)
	Boiler *antheater;			// Antenna Heater (puts heat back into antenna)
    e_object *dc_source;		// Source of DC power
	double range;				// Range in feet
	double rate[3];				// Velocity X/Y/Z in feet/second
	double antennaAngle;		// Antenna angle
	int ruptSent;				// Rupt sent
	int rangeGood;				// RDG flag
	int velocityGood;			// VDG flag
};

// Rendezvous Radar
class LEM_RR : public e_object {
public:
	LEM_RR();
	void Init(LEM *s,e_object *dc_src, e_object *ac_src, h_Radiator *ant, Boiler *anheat);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	void SystemTimeStep(double simdt);
	double GetAntennaTempF();
	double GetRadarTrunnionVel() { return -trunnionVel ; } ;
	double GetRadarShaftVel() { return shaftVel ; } ;
	double GetRadarTrunnionPos() { return -asin(sin(trunnionAngle)); }
	double GetRadarShaftPos() { return -asin(sin(shaftAngle)) ; }
	double GetRadarRange() { return range; } ;
	double GetRadarRate() { return rate ; };
	double GetSignalStrength() { return SignalStrength*4.0; }
	double GetShaftErrorSignal();
	double GetTrunnionErrorSignal();
	
	bool IsPowered(); 
	bool IsDCPowered(); 
	bool IsRadarDataGood() { return radarDataGood;};

private:

	LEM *lem;					// Pointer at LEM
	h_Radiator *antenna;			// Antenna (loses heat into space)
	Boiler *antheater;			// Antenna Heater (puts heat back into antenna)
    e_object *dc_source;
	e_object *ac_source;
	double tstime;
	int	   tstate[2];
	double tsangle[2];
	int    isTracking;
	bool   radarDataGood;
	double trunnionAngle;
	double shaftAngle;
	double trunnionVel;
	double shaftVel;
	double range;
	double rate;
	int ruptSent;				// Rupt sent
	int scratch[2];             // Scratch data
	int mode;					//Mode I = false, Mode II = true
	double hpbw_factor;			//Beamwidth factor
	double SignalStrength;
	double SignalStrengthQuadrant[4];
	VECTOR3 U_RRL[4];
	bool AutoTrackEnabled;
	double ShaftErrorSignal;
	double TrunnionErrorSignal;
	VECTOR3 GyroRates;
};


class LEM_RadarTape : public e_object {
public:
	LEM_RadarTape();
	void Init(LEM *s, e_object * dc_src, e_object *ac_src);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	void SystemTimeStep(double simdt);
	void setRange(double range) { reqRange = range; };
	void setRate(double rate) { reqRate = rate ; }; 
	void RenderRange(SURFHANDLE surf, SURFHANDLE tape);
	void RenderRate(SURFHANDLE surf, SURFHANDLE tape);
	void SetLGCAltitude(int val);
	void SetLGCAltitudeRate(int val);

	bool IsPowered();
private:
	LEM *lem;					// Pointer at LEM
	e_object *dc_source;
	e_object *ac_source;
	double reqRange;
	double reqRate;
	int  dispRange;
	int  dispRate;
	double lgc_alt, lgc_altrate;
};

class CrossPointer
{
public:
	CrossPointer();
	void Init(LEM *s, e_object *dc_src, ToggleSwitch *scaleSw, ToggleSwitch *rateErrMon);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void TimeStep(double simdt);
	void SystemTimeStep(double simdt);
	void GetVelocities(double &vx, double &vy);

	bool IsPowered();
protected:
	LEM *lem;
	e_object *dc_source;
	ToggleSwitch *scaleSwitch;
	ToggleSwitch *rateErrMonSw;

	double vel_x, vel_y;
	double lgc_forward, lgc_lateral;
};

#define CROSSPOINTER_LEFT_START_STRING "CROSSPOINTER_LEFT_START"
#define CROSSPOINTER_RIGHT_START_STRING "CROSSPOINTER_RIGHT_START"
#define CROSSPOINTER_END_STRING "CROSSPOINTER_END"


// Caution and Warning Electronics Assembly
class LEM_CWEA{
public:
	LEM_CWEA();
	void Init(LEM *s);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	void RedrawLeft(SURFHANDLE sf, SURFHANDLE ssf);
	void RedrawRight(SURFHANDLE sf, SURFHANDLE ssf);

	int LightStatus[5][8];		// 1 = lit, 2 = not
	int CabinLowPressLt;		// FF for this
	int WaterWarningDisabled;   // FF for this
	LEM *lem;					// Pointer at LEM
};

///
/// \ingroup LEM
///
class LEM : public Payload, public PanelSwitchListener {

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
		SRF_LIGHTS2,
		SRF_LEMSWITCH1,
		SRF_LEMSWTICH3,
		SRF_SECSWITCH,
		SRF_THRUSTMETER,
		SRF_SEQUENCERSWITCHES,
		SRF_LMTWOPOSLEVER,
		SRF_MASTERALARM_BRIGHT,
		SRF_DSKY,
		SRF_THREEPOSSWITCH,
		SRF_MFDFRAME,
		SRF_MFDPOWER,
		SRF_ROTATIONALSWITCH,
		SRF_CONTACTLIGHT,
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
		SRF_BORDER_38x38,
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
		SRF_AOTRETICLEKNOB,
		SRF_AOTSHAFTKNOB,
		SRF_FIVE_POS_SWITCH,
		SRF_DEDA_KEY,
		SRF_DEDA_LIGHTS,
		SRF_LEM_STAGESWITCH,
		SRF_DIGITALDISP2,
		SRF_RR_NOTRACK,
		SRF_RADAR_TAPE,
		SRF_ORDEAL_PANEL,
		SRF_ORDEAL_ROTARY,
		SRF_TW_NEEDLE,
		SRF_SEQ_LIGHT,
		SRF_LMENGINE_START_STOP_BUTTONS,
		SRF_LMTRANSLBUTTON,

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
	double GetMissionTime() { return MissionTime; }; // This must be here for the MFD can't use it.

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
	void SetRCSJetLevelPrimary(int jet, double level);
	void CheckRCS();

	// DS20160916 Physical parameters updation
	double CurrentFuelWeight, LastFuelWeight; // Fuel weights right now and at the last update

	//
	// These functions must be virtual so they can be called from the Saturn V or the LEVA
	//

	virtual bool SetupPayload(PayloadSettings &ls);
	virtual void PadLoad(unsigned int address, unsigned int value);
	virtual void StopEVA();

	char *getOtherVesselName() { return agc.OtherVesselName;};
	DPSPropellantSource *GetDPSPropellant() { return &DPSPropellant; };

	///
	/// \brief Triggers Virtual AGC core dump
	///
	virtual void VirtualAGCCoreDump() { agc.VirtualAGCCoreDump("ProjectApollo LGC.core"); }

	PROPELLANT_HANDLE ph_RCSA,ph_RCSB;   // RCS Fuel A and B, replaces ph_rcslm0
	PROPELLANT_HANDLE ph_Dsc, ph_Asc; // handles for propellant resources
	THRUSTER_HANDLE th_hover[2];               // handles for orbiter main engines,added 2 for "virtual engine"
	// There are 16 RCS. 4 clusters, 4 per cluster.
	THRUSTER_HANDLE th_rcs[16];
	// These RCSes are for Orbiter's use and should be deleted once the internal guidance is working.
	//THRUSTER_HANDLE th_rcs_orbiter_rot[24];
	//THRUSTER_HANDLE th_rcs_orbiter_lin[16];
	THGROUP_HANDLE thg_hover;		          // handles for thruster groups
	SURFHANDLE exhaustTex;

	double DebugLineClearTimer;			// Timer for clearing debug line
		
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
	int rhc_pov_id;						  ///< ID of the cooliehat a.k.a. POV
	int thc_id;                           // Joystick # for the THC
	int thc_rot_id;						  // ID of ROTATOR axis to use for THC Z-axis
	int thc_sld_id;                       // ID of SLIDER axis to use for THC Z-axis
	int thc_rzx_id;                       // Flag to use native Z-axis as THC Z-axis	
	int thc_pov_id;						  ///< ID of the cooliehat a.k.a. POV
	int thc_tjt_id;                       // Flag to use axis as TTCA THROTTLE/JETS select lever
	int rhc_debug;						  // Flags to print debugging messages.
	int thc_debug;
	bool rhc_auto;						  ///< RHC Z-axis auto detection
	bool thc_auto;						  ///< THC Z-axis auto detection
	bool rhc_thctoggle;					  ///< Enable RHC/THC toggle
	int rhc_thctoggle_id;				  ///< RHC button id for RHC/THC toggle
	bool rhc_thctoggle_pressed;			  ///< Button pressed flag
	int ttca_mode;                        // TTCA Throttle/Jets Mode
#define TTCA_MODE_THROTTLE 0
#define TTCA_MODE_JETS 1
	int ttca_throttle_pos;                // TTCA THROTTLE-mode position
	double ttca_throttle_pos_dig;		  // TTCA THROTTLE-mode position mapped to 0-1
	int ttca_throttle_vel;
	double ttca_thrustcmd;
	int js_current;


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
	void RedrawPanel_XPointer (CrossPointer *cp, SURFHANDLE surf);
	void RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset);
	void MousePanel_MFDButton(int mfd, int event, int mx, int my);
	void ReleaseSurfaces ();
	void ResetThrusters();
	void SeparateStage (UINT stage);
	void InitPanel (int panel);
	void SetSwitches(int panel);
	void AddRCS_LMH(double TRANZ);
	void AddRCS_LMH2(double TRANZ);
	void ToggleEVA();
	void SetupEVA();
	void SetView();
	void RedrawPanel_Horizon (SURFHANDLE surf);
	void RedrawPanel_AOTReticle (SURFHANDLE surf);
	void SwitchClick();
	void CabinFanSound();
	void VoxSound();
	void ButtonClick();
	void GuardClick();
	void AbortFire();
	void InitSwitches();
	void DoFirstTimestep();
	void LoadDefaultSounds();
	void RCSSoundTimestep();
	// void GetDockStatus();

	bool CabinFansActive();

	void SystemsTimestep(double simt, double simdt);
	void SystemsInit();
	void JoystickTimestep(double simdt);
	bool ProcessConfigFileLine (FILEHANDLE scn, char *line);
	//
	// Save/Load support functions.
	//

	int GetCSwitchState();
	void SetCSwitchState(int s);

	SURFHANDLE srf[nsurf];  // handles for panel bitmaps

	double MissionTime;

	// Panel components
	PanelSwitches MainPanel;
	PanelSwitchScenarioHandler PSH;

	SwitchRow AbortSwitchesRow;

	LMAbortButton AbortSwitch;
	LMAbortStageButton AbortStageSwitch;

	
	SwitchRow RRGyroSelSwitchRow;
	ThreePosSwitch RRGyroSelSwitch;
	
	/////////////////
	// LEM panel 1 //
	/////////////////

	FDAI fdaiLeft;
	int fdaiDisabled;
	int fdaiSmooth;

	CrossPointer crossPointerLeft;

	HBITMAP hBmpFDAIRollIndicator;

	SwitchRow LeftXPointerSwitchRow;
	ToggleSwitch LeftXPointerSwitch;

	SwitchRow MainPropOxidPercentRow;
	LEMDPSOxidPercentMeter DPSOxidPercentMeter;

	SwitchRow MainPropFuelPercentRow;
	LEMDPSFuelPercentMeter DPSFuelPercentMeter;

	SwitchRow MainPropHeliumPressRow;
	LEMDigitalHeliumPressureMeter MainHeliumPressureMeter;

	SwitchRow MainPropAndEngineIndRow;
	EngineThrustInd EngineThrustInd;
	CommandedThrustInd CommandedThrustInd;
	MainFuelTempInd MainFuelTempInd;
	MainFuelPressInd MainFuelPressInd;
	MainOxidizerTempInd MainOxidizerTempInd;
	MainOxidizerPressInd MainOxidizerPressInd;

	SwitchRow ThrustWeightIndRow;
	ThrustWeightInd ThrustWeightInd;

	SwitchRow GuidContSwitchRow;
	ToggleSwitch GuidContSwitch;
	ModeSelectSwitch ModeSelSwitch;
	ToggleSwitch AltRngMonSwitch;

	SwitchRow LeftMonitorSwitchRow;
	ToggleSwitch RateErrorMonSwitch;
	ToggleSwitch AttitudeMonSwitch;

	SwitchRow MPSRegControlLeftSwitchRow;
	IndicatorSwitch ASCHeReg1TB;
	LEMDPSValveTalkback DESHeReg1TB;
	ThreePosSwitch ASCHeReg1Switch;	
	ThreePosSwitch DESHeReg1Switch;
	
	SwitchRow MPSRegControlRightSwitchRow;
	IndicatorSwitch ASCHeReg2TB;
	LEMDPSValveTalkback DESHeReg2TB;
	ThreePosSwitch ASCHeReg2Switch;
	ThreePosSwitch DESHeReg2Switch;

	SwitchRow FDAILowerSwitchRow;
	ToggleSwitch ShiftTruSwitch;
	ToggleSwitch RateScaleSwitch;
	ToggleSwitch ACAPropSwitch;
	
	SwitchRow EngineThrustContSwitchRow;
	AGCIOSwitch THRContSwitch;
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

	FDAI fdaiRight;

	CrossPointer crossPointerRight;

	SwitchRow RCSIndicatorRow;
	LMRCSATempInd LMRCSATempInd;
	LMRCSBTempInd LMRCSBTempInd;
	LMRCSAPressInd LMRCSAPressInd;
	LMRCSBPressInd LMRCSBPressInd;
	LMRCSAQtyInd LMRCSAQtyInd;
	LMRCSBQtyInd LMRCSBQtyInd;

	SwitchRow ECSUpperIndicatorRow;
	LMSuitTempMeter LMSuitTempMeter;
	LMCabinTempMeter LMCabinTempMeter;
	LMSuitPressMeter LMSuitPressMeter;
	LMCabinPressMeter LMCabinPressMeter;
	LMCabinCO2Meter LMCabinCO2Meter;

	SwitchRow ECSLowerIndicatorRow;
	LMGlycolTempMeter LMGlycolTempMeter;
	LMGlycolPressMeter LMGlycolPressMeter;
	LMOxygenQtyMeter LMOxygenQtyMeter;
	LMWaterQtyMeter LMWaterQtyMeter;

	SwitchRow RightMonitorSwitchRow;
	ToggleSwitch RightRateErrorMonSwitch;
	ToggleSwitch RightAttitudeMonSwitch;

	SwitchRow TempPressMonRotaryRow;
	RotationalSwitch TempPressMonRotary;

	SwitchRow RCSAscFeedTBSwitchRow;
	IndicatorSwitch RCSAscFeed1ATB;
	IndicatorSwitch RCSAscFeed2ATB;
	IndicatorSwitch RCSAscFeed1BTB;
	IndicatorSwitch RCSAscFeed2BTB;

	SwitchRow RCSAscFeedSwitchRow;
	ThreePosSwitch RCSAscFeed1ASwitch;
	ThreePosSwitch RCSAscFeed2ASwitch;
	ThreePosSwitch RCSAscFeed1BSwitch;
	ThreePosSwitch RCSAscFeed2BSwitch;

	SwitchRow RCSQuad14TBSwitchRow;
	IndicatorSwitch RCSQuad1ACmdEnableTB;
	IndicatorSwitch RCSQuad4ACmdEnableTB;
	IndicatorSwitch RCSQuad1BCmdEnableTB;
	IndicatorSwitch RCSQuad4BCmdEnableTB;

	SwitchRow RCSQuad14SwitchRow;
	ThreePosSwitch RCSQuad1ACmdEnableSwitch;
	ThreePosSwitch RCSQuad4ACmdEnableSwitch;
	ThreePosSwitch RCSQuad1BCmdEnableSwitch;
	ThreePosSwitch RCSQuad4BCmdEnableSwitch;

	SwitchRow RCSQuad23TBSwitchRow;
	IndicatorSwitch RCSQuad2ACmdEnableTB;
	IndicatorSwitch RCSQuad3ACmdEnableTB;
	IndicatorSwitch RCSQuad2BCmdEnableTB;
	IndicatorSwitch RCSQuad3BCmdEnableTB;

	SwitchRow RCSQuad23SwitchRow;
	ThreePosSwitch RCSQuad2ACmdEnableSwitch;
	ThreePosSwitch RCSQuad3ACmdEnableSwitch;
	ThreePosSwitch RCSQuad2BCmdEnableSwitch;
	ThreePosSwitch RCSQuad3BCmdEnableSwitch;

	SwitchRow RCSXfeedTBSwitchRow;
	IndicatorSwitch RCSXFeedTB;

	SwitchRow RCSXfeedSwitchRow;
	ThreePosSwitch RCSXFeedSwitch;

	// DS20060406 RCS MAIN SHUTOFF VALVES
	SwitchRow RCSMainSOVTBRow;
	LEMValveTalkback RCSMainSovATB;
	LEMValveTalkback RCSMainSovBTB;

	SwitchRow RCSMainSOVSwitchRow;
	LEMValveSwitch RCSMainSovASwitch;
	LEMValveSwitch RCSMainSovBSwitch;

	SwitchRow RightACAPropSwitchRow;
	ToggleSwitch RightACAPropSwitch;

	SwitchRow GlycolSuitFanRotaryRow;
	RotationalSwitch GlycolRotary;
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
	PGNSSwitch ModeControlPGNSSwitch;
	ThreePosSwitch ModeControlAGSSwitch;
    UnguardedIMUCageSwitch IMUCageSwitch;

	SwitchRow EngGimbalEnableSwitchRow;
	AGCIOSwitch EngGimbalEnableSwitch;

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

	SwitchRow TempMonitorIndRow;
	TempMonitorInd TempMonitorInd;

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

	SwitchRow RadarSlewSwitchRow;
	FivePosSwitch RadarSlewSwitch;

	SwitchRow EventTimerSwitchRow;
	EventTimerResetSwitch EventTimerCtlSwitch;
	EventTimerControlSwitch EventTimerStartSwitch;
	TimerUpdateSwitch EventTimerMinuteSwitch;
	TimerUpdateSwitch EventTimerSecondSwitch;

	//
	// Currently these are just 0-5V meters; at some point we may want
	// to change them to a different class.
	//

	SwitchRow RaderSignalStrengthMeterRow;
	DCVoltMeter RadarSignalStrengthMeter;
	RadarSignalStrengthAttenuator RadarSignalStrengthAttenuator;


	/////////////////
	// LEM panel 4 //
	/////////////////

	SwitchRow Panel4LeftSwitchRow;
	ToggleSwitch LeftACA4JetSwitch;
	ToggleSwitch LeftTTCATranslSwitch;

	SwitchRow Panel4RightSwitchRow;
	ToggleSwitch RightACA4JetSwitch;
	ToggleSwitch RightTTCATranslSwitch;

	///////////////////////
	// DSKYs             //
	///////////////////////

	SwitchRow DskySwitchRow;
	DSKYPushSwitch DskySwitchVerb;
	DSKYPushSwitch DskySwitchNoun;
	DSKYPushSwitch DskySwitchPlus;
	DSKYPushSwitch DskySwitchMinus;
	DSKYPushSwitch DskySwitchZero;
	DSKYPushSwitch DskySwitchOne;
	DSKYPushSwitch DskySwitchTwo;
	DSKYPushSwitch DskySwitchThree;
	DSKYPushSwitch DskySwitchFour;
	DSKYPushSwitch DskySwitchFive;
	DSKYPushSwitch DskySwitchSix;
	DSKYPushSwitch DskySwitchSeven;
	DSKYPushSwitch DskySwitchEight;
	DSKYPushSwitch DskySwitchNine;
	DSKYPushSwitch DskySwitchClear;
	DSKYPushSwitch DskySwitchProg;
	DSKYPushSwitch DskySwitchKeyRel;
	DSKYPushSwitch DskySwitchEnter;
	DSKYPushSwitch DskySwitchReset;

	//////////////////
	// LEM panel 11 //
	//////////////////

	SwitchRow Panel11CB1SwitchRow;
	CircuitBrakerSwitch SE_WND_HTR_AC_CB;
	CircuitBrakerSwitch HE_PQGS_PROP_DISP_AC_CB;
	CircuitBrakerSwitch SBD_ANT_AC_CB;
	CircuitBrakerSwitch ORDEAL_AC_CB;
	CircuitBrakerSwitch AGS_AC_CB;
	CircuitBrakerSwitch AOT_LAMP_ACB_CB;
	CircuitBrakerSwitch LMP_FDAI_AC_CB;
	CircuitBrakerSwitch NUM_LTG_AC_CB;
	CircuitBrakerSwitch AC_B_INV_1_FEED_CB;
	CircuitBrakerSwitch AC_B_INV_2_FEED_CB;
	CircuitBrakerSwitch AC_A_INV_1_FEED_CB;
	CircuitBrakerSwitch AC_A_INV_2_FEED_CB;
	LEMVoltCB           AC_A_BUS_VOLT_CB;
	CircuitBrakerSwitch CDR_WND_HTR_AC_CB;
	CircuitBrakerSwitch TAPE_RCDR_AC_CB;
	CircuitBrakerSwitch AOT_LAMP_ACA_CB;
	CircuitBrakerSwitch RDZ_RDR_AC_CB;
	CircuitBrakerSwitch DECA_GMBL_AC_CB;
	CircuitBrakerSwitch INTGL_LTG_AC_CB;

	SwitchRow Panel11CB2SwitchRow;
	CircuitBrakerSwitch RCS_A_MAIN_SOV_CB;
	CircuitBrakerSwitch RCS_A_QUAD4_TCA_CB;
	CircuitBrakerSwitch RCS_A_QUAD3_TCA_CB;
	CircuitBrakerSwitch RCS_A_QUAD2_TCA_CB;
	CircuitBrakerSwitch RCS_A_QUAD1_TCA_CB;
	CircuitBrakerSwitch RCS_A_ISOL_VLV_CB;
	CircuitBrakerSwitch RCS_A_ASC_FEED_2_CB;
	CircuitBrakerSwitch RCS_A_ASC_FEED_1_CB;
	CircuitBrakerSwitch THRUST_DISP_CB;
	CircuitBrakerSwitch MISSION_TIMER_CB;
	CircuitBrakerSwitch CDR_XPTR_CB;
	CircuitBrakerSwitch RNG_RT_ALT_RT_DC_CB;
	CircuitBrakerSwitch GASTA_DC_CB;
	CircuitBrakerSwitch CDR_FDAI_DC_CB;
	CircuitBrakerSwitch COAS_DC_CB;
	CircuitBrakerSwitch ORDEAL_DC_CB;
	CircuitBrakerSwitch RNG_RT_ALT_RT_AC_CB;
	CircuitBrakerSwitch GASTA_AC_CB;
	CircuitBrakerSwitch CDR_FDAI_AC_CB;

	SwitchRow Panel11CB3SwitchRow;
	CircuitBrakerSwitch PROP_DES_HE_REG_VENT_CB;
	CircuitBrakerSwitch HTR_RR_STBY_CB;
	CircuitBrakerSwitch HTR_RR_OPR_CB;
	CircuitBrakerSwitch HTR_LR_CB;
	CircuitBrakerSwitch HTR_DOCK_WINDOW_CB;
	CircuitBrakerSwitch HTR_AOT_CB;
	CircuitBrakerSwitch INST_SIG_CONDR_1_CB;
	CircuitBrakerSwitch CDR_SCS_AEA_CB;
	CircuitBrakerSwitch CDR_SCS_ABORT_STAGE_CB;
	CircuitBrakerSwitch CDR_SCS_ATCA_CB;
	CircuitBrakerSwitch CDR_SCS_AELD_CB;
	CircuitBrakerSwitch SCS_ENG_CONT_CB;
	CircuitBrakerSwitch SCS_ATT_DIR_CONT_CB;
	CircuitBrakerSwitch SCS_ENG_START_OVRD_CB;
	CircuitBrakerSwitch SCS_DECA_PWR_CB;
	CircuitBrakerSwitch EDS_CB_LG_FLAG;
	CircuitBrakerSwitch EDS_CB_LOGIC_A;
	CircuitBrakerSwitch CDR_LTG_UTIL_CB;
	CircuitBrakerSwitch CDR_LTG_ANUN_DOCK_COMPNT_CB;

	SwitchRow Panel11CB4SwitchRow;
	CircuitBrakerSwitch RCS_QUAD_4_CDR_HTR_CB;
	CircuitBrakerSwitch RCS_QUAD_3_CDR_HTR_CB;
	CircuitBrakerSwitch RCS_QUAD_2_CDR_HTR_CB;
	CircuitBrakerSwitch RCS_QUAD_1_CDR_HTR_CB;
	CircuitBrakerSwitch ECS_SUIT_FAN_1_CB;
	CircuitBrakerSwitch ECS_CABIN_FAN_1_CB;
	CircuitBrakerSwitch ECS_GLYCOL_PUMP_2_CB;
	CircuitBrakerSwitch ECS_GLYCOL_PUMP_1_CB;
	CircuitBrakerSwitch ECS_GLYCOL_PUMP_AUTO_XFER_CB;
	CircuitBrakerSwitch COMM_UP_DATA_LINK_CB;
	CircuitBrakerSwitch COMM_SEC_SBAND_XCVR_CB;
	CircuitBrakerSwitch COMM_SEC_SBAND_PA_CB;
	CircuitBrakerSwitch COMM_VHF_XMTR_B_CB;
	CircuitBrakerSwitch COMM_VHF_RCVR_A_CB;
	CircuitBrakerSwitch COMM_CDR_AUDIO_CB;
	CircuitBrakerSwitch PGNS_SIG_STR_DISP_CB;
	CircuitBrakerSwitch PGNS_LDG_RDR_CB;
	CircuitBrakerSwitch PGNS_RNDZ_RDR_CB;
	CircuitBrakerSwitch IMU_OPR_CB;
	CircuitBrakerSwitch IMU_SBY_CB;
	CircuitBrakerSwitch LGC_DSKY_CB;

	SwitchRow Panel11CB5SwitchRow;
	// Bus feed tie breakers
	CircuitBrakerSwitch CDRBatteryFeedTieCB1;
	CircuitBrakerSwitch CDRBatteryFeedTieCB2;
	CircuitBrakerSwitch CDRCrossTieBusCB;
	CircuitBrakerSwitch CDRCrossTieBalCB;
	CircuitBrakerSwitch CDRXLunarBusTieCB;
	// ECA control & Voltmeter
	CircuitBrakerSwitch CDRDesECAContCB;
	CircuitBrakerSwitch CDRDesECAMainCB;
	CircuitBrakerSwitch CDRAscECAContCB;
	CircuitBrakerSwitch CDRAscECAMainCB;
	LEMVoltCB CDRDCBusVoltCB;
	// AC Inverter 1 feed
	CircuitBrakerSwitch CDRInverter1CB;

	/////////////////
	// LEM Panel 5 //
	/////////////////

	SwitchRow Panel5SwitchRow;
	LEMMissionTimerSwitch TimerContSwitch;
	LEMMissionTimerSwitch TimerSlewHours;
	LEMMissionTimerSwitch TimerSlewMinutes;
	LEMMissionTimerSwitch TimerSlewSeconds;
	ToggleSwitch LtgORideAnunSwitch;
	ToggleSwitch LtgORideNumSwitch;
	ToggleSwitch LtgORideIntegralSwitch;
	ToggleSwitch LtgSidePanelsSwitch;
	RotationalSwitch LtgFloodOhdFwdKnob;
	RotationalSwitch LtgAnunNumKnob;
	RotationalSwitch LtgIntegralKnob;
	PushSwitch PlusXTranslationButton;
	EngineStartButton ManualEngineStart;
	EngineStopButton CDRManualEngineStop;

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
	GuardedToggleSwitch EDStage;
	ToggleSwitch EDStageRelay;
	ThreePosSwitch EDDesFuelVent;
	ThreePosSwitch EDDesOxidVent;
	IndicatorSwitch EDLGTB;
	LEMDPSValveTalkback EDDesFuelVentTB;
	LEMDPSValveTalkback EDDesOxidVentTB;
	// Audio section
	ThreePosSwitch CDRAudSBandSwitch;
	ThreePosSwitch CDRAudICSSwitch;
	ToggleSwitch CDRAudRelaySwitch;
	ThreePosSwitch CDRAudVOXSwitch;
	ToggleSwitch CDRAudioControlSwitch;
	ThreePosSwitch CDRAudVHFASwitch;
	ThreePosSwitch CDRAudVHFBSwitch;
	ThumbwheelSwitch CDRAudSBandVol;
	ThumbwheelSwitch CDRAudVHFAVol;
	ThumbwheelSwitch CDRAudVHFBVol;
	ThumbwheelSwitch CDRAudICSVol;
	ThumbwheelSwitch CDRAudMasterVol;
	ThumbwheelSwitch CDRAudVOXSens;
	ThreePosSwitch CDRCOASSwitch;

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
	ThreeSourceSwitch EPSEDVoltSelect;

	SwitchRow DSCHiVoltageSwitchRow;
	LEMBatterySwitch DSCSEBat1HVSwitch;
	LEMBatterySwitch DSCSEBat2HVSwitch;
	LEMBatterySwitch DSCCDRBat3HVSwitch;
	LEMBatterySwitch DSCCDRBat4HVSwitch;	
	LEMDeadFaceSwitch DSCBattFeedSwitch;

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
	IndicatorSwitch DSCBattFeedTB;

	SwitchRow ASCBatteryTBSwitchRow;
	IndicatorSwitch ASCBattery5ATB;
	IndicatorSwitch ASCBattery5BTB;
	IndicatorSwitch ASCBattery6ATB;
	IndicatorSwitch ASCBattery6BTB;

	SwitchRow ASCBatterySwitchRow;
	LEMBatterySwitch ASCBat5SESwitch;
	LEMBatterySwitch ASCBat5CDRSwitch;
	LEMBatterySwitch ASCBat6CDRSwitch;
	LEMBatterySwitch ASCBat6SESwitch;
	ToggleSwitch UpDataSquelchSwitch;

	SwitchRow Panel12AudioCtlSwitchRow;
	ToggleSwitch Panel12AudioCtlSwitch;

	SwitchRow Panel12UpdataLinkSwitchRow;
	ThreePosSwitch Panel12UpdataLinkSwitch;

	SwitchRow Panel12AudioControlSwitchRow;
	ThreePosSwitch LMPAudSBandSwitch;
	ThreePosSwitch LMPAudICSSwitch;
	ToggleSwitch LMPAudRelaySwitch;
	ThreePosSwitch LMPAudVOXSwitch;	
	ThreePosSwitch LMPAudVHFASwitch;
	ThreePosSwitch LMPAudVHFBSwitch;
	ThumbwheelSwitch LMPAudSBandVol;
	ThumbwheelSwitch LMPAudVHFAVol;
	ThumbwheelSwitch LMPAudVHFBVol;
	ThumbwheelSwitch LMPAudICSVol;
	ThumbwheelSwitch LMPAudMasterVol;
	ThumbwheelSwitch LMPAudVOXSens;

	SwitchRow Panel12CommSwitchRow1;
	ToggleSwitch SBandModulateSwitch;
	ThreePosSwitch SBandXCvrSelSwitch;
	ThreePosSwitch SBandPASelSwitch;
	ThreePosSwitch SBandVoiceSwitch;
	ThreePosSwitch SBandPCMSwitch;
	ThreePosSwitch SBandRangeSwitch;

	SwitchRow Panel12CommSwitchRow2;
	ThreePosSwitch VHFAVoiceSwitch;
	ToggleSwitch VHFARcvrSwtich;
	ThreePosSwitch VHFBVoiceSwitch;
	ToggleSwitch VHFBRcvrSwtich;
	ThreePosSwitch TLMBiomedSwtich;
	ToggleSwitch TLMBitrateSwitch;

	SwitchRow Panel12CommSwitchRow3;
	ThumbwheelSwitch VHFASquelch;
	ThumbwheelSwitch VHFBSquelch;
	IndicatorSwitch TapeRecorderTB;
	ToggleSwitch TapeRecorderSwitch;

	SwitchRow Panel12AntTrackModeSwitchRow;
	ThreePosSwitch Panel12AntTrackModeSwitch;

	SwitchRow Panel12SignalStrengthMeterRow;
	LEMSBandAntennaStrengthMeter Panel12SignalStrengthMeter;

	SwitchRow Panel12VHFAntSelSwitchRow;
	RotationalSwitch Panel12VHFAntSelKnob;

	SwitchRow Panel12SBandAntSelSwitchRow;
	RotationalSwitch Panel12SBandAntSelKnob;
	
	SwitchRow Panel12AntPitchSwitchRow;
	RotationalSwitch Panel12AntPitchKnob;

	SwitchRow Panel12AntYawSwitchRow;
	RotationalSwitch Panel12AntYawKnob;

	SwitchRow LMPManualEngineStopSwitchRow;
	EngineStopButton LMPManualEngineStop;

	SwitchRow AGSOperateSwitchRow;
	ThreePosSwitch AGSOperateSwitch;

	SwitchRow ComPitchMeterRow;
	LEMSteerableAntennaPitchMeter ComPitchMeter;

	SwitchRow ComYawMeterRow;
	LEMSteerableAntennaYawMeter ComYawMeter;

	//////////////////
	// LEM panel 16 //
	//////////////////

	SwitchRow Panel16CB1SwitchRow;
	CircuitBrakerSwitch LMP_EVT_TMR_FDAI_DC_CB;
	CircuitBrakerSwitch SE_XPTR_DC_CB;
	CircuitBrakerSwitch RCS_B_ASC_FEED_1_CB;
	CircuitBrakerSwitch RCS_B_ASC_FEED_2_CB;
	CircuitBrakerSwitch RCS_B_ISOL_VLV_CB;
	CircuitBrakerSwitch RCS_B_QUAD1_TCA_CB;
	CircuitBrakerSwitch RCS_B_QUAD2_TCA_CB;
	CircuitBrakerSwitch RCS_B_QUAD3_TCA_CB;
	CircuitBrakerSwitch RCS_B_QUAD4_TCA_CB;
	CircuitBrakerSwitch RCS_B_CRSFD_CB;
	CircuitBrakerSwitch RCS_B_TEMP_PRESS_DISP_FLAGS_CB;
	CircuitBrakerSwitch RCS_B_PQGS_DISP_CB;
	CircuitBrakerSwitch RCS_B_MAIN_SOV_CB;
	CircuitBrakerSwitch PROP_DISP_ENG_OVRD_LOGIC_CB;
	CircuitBrakerSwitch PROP_PQGS_CB;
	CircuitBrakerSwitch PROP_ASC_HE_REG_CB;

	SwitchRow Panel16CB2SwitchRow;
	CircuitBrakerSwitch LTG_FLOOD_CB;
	CircuitBrakerSwitch LTG_TRACK_CB;
	CircuitBrakerSwitch LTG_ANUN_DOCK_COMPNT_CB;
	CircuitBrakerSwitch LTG_MASTER_ALARM_CB;
	CircuitBrakerSwitch EDS_CB_LOGIC_B;
	CircuitBrakerSwitch SCS_AEA_CB;
	CircuitBrakerSwitch SCS_ENG_ARM_CB;
	CircuitBrakerSwitch SCS_ASA_CB;
	CircuitBrakerSwitch SCS_AELD_CB;
	CircuitBrakerSwitch SCS_ATCA_CB;
	CircuitBrakerSwitch SCS_ABORT_STAGE_CB;
	CircuitBrakerSwitch SCS_ATCA_AGS_CB;
	CircuitBrakerSwitch SCS_DES_ENG_OVRD_CB;
	CircuitBrakerSwitch INST_CWEA_CB;
	CircuitBrakerSwitch INST_SIG_SENSOR_CB;
	CircuitBrakerSwitch INST_PCMTEA_CB;
	CircuitBrakerSwitch INST_SIG_CONDR_2_CB;
	CircuitBrakerSwitch ECS_SUIT_FLOW_CONT_CB;

	SwitchRow Panel16CB3SwitchRow;
	CircuitBrakerSwitch COMM_DISP_CB;
	CircuitBrakerSwitch COMM_SE_AUDIO_CB;
	CircuitBrakerSwitch COMM_VHF_XMTR_A_CB;
	CircuitBrakerSwitch COMM_VHF_RCVR_B_CB;
	CircuitBrakerSwitch COMM_PRIM_SBAND_PA_CB;
	CircuitBrakerSwitch COMM_PRIM_SBAND_XCVR_CB;
	CircuitBrakerSwitch COMM_SBAND_ANT_CB;
	CircuitBrakerSwitch COMM_PMP_CB;
	CircuitBrakerSwitch COMM_TV_CB;
	CircuitBrakerSwitch ECS_DISP_CB;
	CircuitBrakerSwitch ECS_GLYCOL_PUMP_SEC_CB;
	CircuitBrakerSwitch ECS_LGC_PUMP_CB;
	CircuitBrakerSwitch ECS_CABIN_FAN_CONT_CB;
	CircuitBrakerSwitch ECS_CABIN_REPRESS_CB;
	CircuitBrakerSwitch ECS_SUIT_FAN_2_CB;
	CircuitBrakerSwitch ECS_SUIT_FAN_DP_CB;
	CircuitBrakerSwitch ECS_DIVERT_VLV_CB;
	CircuitBrakerSwitch ECS_CO2_SENSOR_CB;

	SwitchRow Panel16CB4SwitchRow;
	CircuitBrakerSwitch RCS_QUAD_1_LMP_HTR_CB;
	CircuitBrakerSwitch RCS_QUAD_2_LMP_HTR_CB;
	CircuitBrakerSwitch RCS_QUAD_3_LMP_HTR_CB;
	CircuitBrakerSwitch RCS_QUAD_4_LMP_HTR_CB;
	CircuitBrakerSwitch HTR_DISP_CB;
	CircuitBrakerSwitch HTR_SBD_ANT_CB;
	CircuitBrakerSwitch CAMR_SEQ_CB;
	CircuitBrakerSwitch EPS_DISP_CB;
	// ECA control, AC Inverter & DC Voltmeter
	LEMVoltCB LMPDCBusVoltCB;
	CircuitBrakerSwitch LMPInverter2CB;
	CircuitBrakerSwitch LMPAscECAMainCB;
	CircuitBrakerSwitch LMPAscECAContCB;
	CircuitBrakerSwitch LMPDesECAMainCB;
	CircuitBrakerSwitch LMPDesECAContCB;
	// Battery feed tie breakers
	CircuitBrakerSwitch LMPXLunarBusTieCB;
	CircuitBrakerSwitch LMPCrossTieBusCB;
	CircuitBrakerSwitch LMPCrossTieBalCB;
	CircuitBrakerSwitch LMPBatteryFeedTieCB1;
	CircuitBrakerSwitch LMPBatteryFeedTieCB2;

	///////////////////////////
	// LEM Rendezvous Window //
	///////////////////////////

	int LEMCoas1Enabled;
	int LEMCoas2Enabled;

	///////////////////////////
	// ORDEAL Panel switches //
	///////////////////////////

	SwitchRow ORDEALSwitchesRow;
	ToggleSwitch ORDEALFDAI1Switch;
	ToggleSwitch ORDEALFDAI2Switch;
	ThreePosSwitch ORDEALEarthSwitch;
	ThreePosSwitch ORDEALLightingSwitch;
	ToggleSwitch ORDEALModeSwitch;
	ThreePosSwitch ORDEALSlewSwitch;
	OrdealRotationalSwitch ORDEALAltSetRotary;

	LEMPanelOrdeal PanelOrdeal;		// Dummy switch/display for checklist controller

	int ordealEnabled;

	bool FirstTimestep;

	bool LAUNCHIND[8];
	bool ABORT_IND;
	bool ENGIND[7];

	bool bToggleHatch;
	bool bModeDocked;
	bool bModeHover;
	bool HatchOpen;
	bool ToggleEva;
	bool CDREVA_IP;
	bool HasProgramer;
	bool InvertStageBit;

#define LMVIEW_CDR		0
#define LMVIEW_LMP		1

	int	viewpos;
	
	bool ContactOK;
	bool SoundsLoaded;

	bool Crewed;
	bool AutoSlow;

	OBJHANDLE hLEVA;
	OBJHANDLE hdsc;

	ATTACHMENTHANDLE hattDROGUE;

	UINT stage;
	int status;

	//
	// Panel flash.
	//

	double NextFlashUpdate;
	bool PanelFlashOn;

	int ApolloNo;
	int Landed;

	//
	// VAGC Mode settings
	//

	bool VAGCChecklistAutoSlow;
	bool VAGCChecklistAutoEnabled;

	int SwitchFocusToLeva;

	DSKY dsky;
	LEMcomputer agc;
	Boiler *imuheater; // IMU Standby Heater
	h_Radiator *imucase; // IMU Case
	IMU imu;
	CDU tcdu;
	CDU scdu;
	LMOptics optics;

	//Pyros

	Pyro LandingGearPyros;
	Pyro StagingBoltsPyros;
	Pyro StagingNutsPyros;
	Pyro CableCuttingPyros;
	Pyro DescentPropVentPyros;
	Pyro DescentEngineStartPyros;
	Pyro DescentEngineOnPyros;
	Pyro DescentPropIsolPyros;
	PowerMerge LandingGearPyrosFeeder;
	PowerMerge CableCuttingPyrosFeeder;
	PowerMerge DescentPropVentPyrosFeeder;
	PowerMerge DescentEngineStartPyrosFeeder;
	PowerMerge DescentEngineOnPyrosFeeder;
	PowerMerge DescentPropIsolPyrosFeeder;

	// Some stuff on init should be done only once
	bool InitLEMCalled;
	int SystemsInitialized;

	MissionTimer MissionTimerDisplay;
	LEMEventTimer EventTimerDisplay;

	double DescentFuelMassKg;	///< Mass of fuel in descent stage of LEM.
	double AscentFuelMassKg;	///< Mass of fuel in ascent stage of LEM.
	double DescentEmptyMassKg;
	double AscentEmptyMassKg;

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

	//Dummy events, not functional
	SaturnEvents DummyEvents;

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
	Sound RCSFireSound;
	Sound RCSSustainSound;

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
	MultiConnector LEMToCSMConnector;				// This carries data *FROM* CSMToLEMPowerConnector
	LEMPowerConnector CSMToLEMPowerConnector;		// This sends data *FROM* CSMToLEMPowerSource *TO* LEMToCSMConnector
	PowerSourceConnectorObject CSMToLEMPowerSource; // This looks like an e-object

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

	// ED Batteries
	Battery *EDBatteryA;
	Battery *EDBatteryB;

	// Bus Tie Blocks (Not real objects)
	LEM_BusFeed BTB_CDR_A;
	LEM_BusFeed BTB_CDR_B;
	LEM_BusFeed BTB_CDR_C;
	LEM_BusFeed BTB_CDR_D;
	LEM_BusFeed BTB_CDR_E;
	LEM_BusFeed BTB_LMP_A;
	LEM_BusFeed BTB_LMP_B;
	LEM_BusFeed BTB_LMP_C;
	LEM_BusFeed BTB_LMP_D;
	LEM_BusFeed BTB_LMP_E;
	
	// Bus Cross Tie Multiplex (Not real object)
	LEM_BusCrossTie BTC_MPX;

	// XLUNAR Bus Controller
	LEM_XLBControl BTC_XLunar;

	// ECA
	LEM_ECAch ECA_1a; // (DESCENT stage, LMP DC bus)
	LEM_ECAch ECA_1b; // (DESCENT stage, LMP DC bus)
	LEM_ECAch ECA_2a; // (DESCENT stage, CDR DC bus)
	LEM_ECAch ECA_2b; // (DESCENT stage, CDR DC bus)
	LEM_ECAch ECA_3a; // (ASCENT  stage, LMP DC bus)
	LEM_ECAch ECA_3b; // (ASCENT  stage, CDR DC bus)
	LEM_ECAch ECA_4a; // (ASCENT  stage, CDR DC bus)
	LEM_ECAch ECA_4b; // (ASCENT  stage, LMP DC bus)

	// Descent stage deadface bus stubs
	DCbus DES_CDRs28VBusA;
	DCbus DES_CDRs28VBusB;
	DCbus DES_LMPs28VBusA;
	DCbus DES_LMPs28VBusB;

	// CDR and LMP 28V DC busses
	DCbus CDRs28VBus;
	DCbus LMPs28VBus;

	// ED 28V DC busses
	DCbus ED28VBusA;
	DCbus ED28VBusB;

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
	DECA deca;
	LEM_LR LR;
	LEM_RR RR;
	GASTA gasta;
	ORDEAL ordeal;
	MechanicalAccelerometer mechanicalAccelerometer;
	SCCA1 scca1;
	SCCA2 scca2;
	SCCA3 scca3;
	LEM_Programer lmp;
	LEM_ACA CDR_ACA;
	LEM_RGA rga;

	LEM_RadarTape RadarTape;
	LEM_CWEA CWEA;

	// COMM
	LEM_SteerableAnt SBandSteerable;
	LM_OMNI omni_fwd;
	LM_OMNI omni_aft;
	LM_VHF VHF;
	LM_SBAND SBand;

	// ECS
	LEM_ECS ecs;

	// EDS
	LEM_EDS eds;

	// DPS and APS
	DPSPropellantSource DPSPropellant;
	LEM_DPS DPS;
	LEM_APS APS;

	// Abort Guidance System stuff
	LEM_ASA asa;
	LEM_AEA aea;
	LEM_DEDA deda;

	bool isMultiThread;

	// Friend classes
	friend class ATCA;
	friend class LEM_EDS;
	friend class LEM_EDRelayBox;
	friend class LEMcomputer;
	friend class LEMDCVoltMeter;
	friend class LEMDCAmMeter;
	friend class LMOptics;
	friend class LEMBatterySwitch;
	friend class LEMDeadFaceSwitch;
	friend class LEMInverterSwitch;
	friend class LEM_BusCrossTie;
	friend class LEM_XLBControl;
	friend class LEM_LR;
	friend class LEM_RR;
	friend class LEM_RadarTape;

	friend class LEM_ASA;
	friend class LEM_AEA;
	friend class LEM_DEDA;
	friend class LEM_SteerableAnt;
	friend class LM_VHF;
	friend class LM_SBAND;
	friend class LEMMissionTimerSwitch;
	friend class LEM_CWEA;
	friend class LMWaterQtyMeter;
	friend class LMOxygenQtyMeter;
	friend class LMGlycolPressMeter;
	friend class LMGlycolTempMeter;
	friend class LMCabinCO2Meter;
	friend class LMCabinPressMeter;
	friend class LMSuitPressMeter;
	friend class LMCabinTempMeter;
	friend class LMSuitTempMeter;
	friend class DPSGimbalActuator;
	friend class DPSPropellantSource;
	friend class LEM_DPS;
	friend class LEM_APS;
	friend class DECA;
	friend class SCCA1;
	friend class SCCA2;
	friend class SCCA3;
	friend class CommandedThrustInd;
	friend class EngineThrustInd;
	friend class ThrustWeightInd;
	friend class CrossPointer;
	friend class LEMPanelOrdeal;
	friend class LMAbortButton;
	friend class LMAbortStageButton;
	friend class RadarSignalStrengthAttenuator;
	friend class LEMSteerableAntennaPitchMeter;
	friend class LEMSteerableAntennaYawMeter;
	friend class LEMSBandAntennaStrengthMeter;
	friend class LEM_Programer;
	friend class LEMDPSDigitalMeter;
	friend class LEM_ACA;
	friend class LEM_RGA;

	friend class ApolloRTCCMFD;
	friend class ProjectApolloMFD;
};

extern void LEMLoadMeshes();
extern void InitGParam(HINSTANCE hModule);
extern void FreeGParam();

#endif
