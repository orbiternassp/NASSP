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
#include "vesim.h"
#include "dsky.h"
#include "IMU.h"
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
#include "lm_ttca.h"
#include "lm_scea.h"
#include "lm_rcs.h"
#include "lm_ecs.h"
#include "lemconnector.h"
#include "lm_cwea.h"
#include "lm_eps.h"
#include "LEMcomputer.h"
#include "lm_rr.h"
#include "lm_lr.h"
#include "lm_aeaa.h"
#include "inertial.h"

// Cosmic background temperature in degrees F
#define CMBG_TEMP -459.584392

//
// Lem state settings from scenario file, passed from CSM.
//

#include "lemswitches.h"
#include "missiontimer.h"
#include "MechanicalAccelerometer.h"
#include "connector.h"
#include "checklistController.h"
#include "payload.h"

enum LMRCSThrusters
{
	LMRCS_A1U = 0,
	LMRCS_A1F,
	LMRCS_B1L,
	LMRCS_B1D,
	LMRCS_B2U,
	LMRCS_B2L,
	LMRCS_A2A,
	LMRCS_A2D,
	LMRCS_A3U,
	LMRCS_A3R,
	LMRCS_B3A,
	LMRCS_B3D,
	LMRCS_B4U,
	LMRCS_B4F,
	LMRCS_A4R,
	LMRCS_A4D
};

#define LM_RCS_QUAD_1		0
#define LM_RCS_QUAD_2		1
#define LM_RCS_QUAD_3		2
#define LM_RCS_QUAD_4		3

// LM ECS status

typedef struct {
	int crewNumber;
	int crewStatus;
	int cdrStatus;	//0 = cabin, 1 = suit, 2 = EVA, 3 = PLSS
	int lmpStatus;
} LEMECSStatus;

// Systems things

class LEM_RadarTape : public e_object {
public:
	LEM_RadarTape();
	void Init(LEM *s, e_object * dc_src, e_object *ac_src, SURFHANDLE surf1, SURFHANDLE surf2);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	void setRange(double range) { reqRange = range; };
	void setRate(double rate) { reqRate = rate ; }; 
	void RenderRange(SURFHANDLE surf);
	void RenderRate(SURFHANDLE surf);
	void RenderRangeVC(SURFHANDLE surf, SURFHANDLE surf1a, SURFHANDLE surf1b, SURFHANDLE surf2, int xTexMul = 1);
	void RenderRateVC(SURFHANDLE surf, SURFHANDLE surf1a, SURFHANDLE surf1b, int xTexMul = 1);
	void SetLGCAltitude(int val);
	void SetLGCAltitudeRate(int val);
	void AGSAltitudeAltitudeRate(int Data);

	double GetLGCAltitude() { return lgc_alt; };
	double GetLGCAltitudeRate() { return lgc_altrate; };

	bool PowerSignalMonOn();
	bool PowerFailure();
	bool SignalFailure();
	bool TimingFailure();
	bool IsPowered();
private:
	void TapeDrive(double &Angle, double AngleCmd, double RateLimit, double simdt);
	LEM *lem;					// Pointer at LEM
	e_object *dc_source;
	e_object *ac_source;
	double reqRange;
	double reqRate;
	double dispRange;
	double dispRate;
	double lgc_alt, lgc_altrate;
	double ags_alt, ags_altrate;
	SURFHANDLE tape1, tape2;
	double desRange, desRate;
	double LGCaltUpdateTime, LGCaltRateUpdateTime;
	double AGSaltUpdateTime, AGSaltRateUpdateTime;

	const double ALTSCALEFACTOR = 0.3048 * 2.345 * pow(2.0, -3.0);
	const double ALTRATESCALEFACTOR = 0.3048 * pow(2.0, -4.0);
};

class CrossPointer
{
public:
	CrossPointer();
	virtual ~CrossPointer();
	void Init(LEM *s, e_object *dc_src, ToggleSwitch *scaleSw, ToggleSwitch *rateErrMon);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	void GetVelocities(double &vx, double &vy);

	void DrawSwitchVC(int id, int event, SURFHANDLE surf);
	void SetDirection(const VECTOR3 &xvec, const VECTOR3 &yvec);
	void DefineVCAnimations(UINT vc_idx);
	void DefineMeshGroup(UINT _grpX, UINT _grpY);

	bool IsPowered();
protected:
	LEM *lem;
	e_object *dc_source;
	ToggleSwitch *scaleSwitch;
	ToggleSwitch *rateErrMonSw;

	double vel_x, vel_y;
	double lgc_forward, lgc_lateral;

	UINT anim_xpointerx, anim_xpointery;
	UINT grpX, grpY;

	VECTOR3 xvector, yvector;
	MGROUP_TRANSLATE *xtrans, *ytrans;
};

#define CROSSPOINTER_LEFT_START_STRING "CROSSPOINTER_LEFT_START"
#define CROSSPOINTER_RIGHT_START_STRING "CROSSPOINTER_RIGHT_START"
#define CROSSPOINTER_END_STRING "CROSSPOINTER_END"

namespace mission
{
	class Mission;
};

// Vesim input IDs
#define LM_AXIS_INPUT_ACAR           1
#define LM_AXIS_INPUT_ACAP           2
#define LM_AXIS_INPUT_ACAY           3
#define LM_AXIS_INPUT_TTCAX          4
#define LM_AXIS_INPUT_TTCAY          5
#define LM_AXIS_INPUT_TTCAZ          6
#define LM_BUTTON_ROT_LIN            7
#define LM_AXIS_INPUT_THROTTLE       8
#define LM_BUTTON_ENG_START          9
#define LM_BUTTON_ENG_STOP          10
#define LM_BUTTON_DES_RATE_PLUS     11
#define LM_BUTTON_DES_RATE_MINUS    12
#define LM_BUTTON_ABORT             13
#define LM_BUTTON_ABORT_STAGE       14
#define LM_BUTTON_ABORT_STAGE_GRD   15
#define LM_BUTTON_DSKY_PRO          16
#define LM_BUTTON_DSKY_KEY_REL      17
#define LM_BUTTON_DSKY_VERB         18
#define LM_BUTTON_DSKY_NOUN         19
#define LM_BUTTON_DSKY_ENTR         20
#define LM_BUTTON_DSKY_CLR          21
#define LM_BUTTON_DSKY_PLUS         22
#define LM_BUTTON_DSKY_MINUS        23
#define LM_BUTTON_DSKY_RSET         24
#define LM_BUTTON_DSKY_NUM_0        25
#define LM_BUTTON_DSKY_NUM_1        26
#define LM_BUTTON_DSKY_NUM_2        27
#define LM_BUTTON_DSKY_NUM_3        28
#define LM_BUTTON_DSKY_NUM_4        29
#define LM_BUTTON_DSKY_NUM_5        30
#define LM_BUTTON_DSKY_NUM_6        31
#define LM_BUTTON_DSKY_NUM_7        32
#define LM_BUTTON_DSKY_NUM_8        33
#define LM_BUTTON_DSKY_NUM_9        34
#define LM_BUTTON_MDCTRL_PGNS       35
#define LM_BUTTON_MDCTRL_PGNS_AUT   36
#define LM_BUTTON_MDCTRL_PGNS_ATH   37
#define LM_BUTTON_MDCTRL_PGNS_OFF   38
#define LM_AXIS_THR_JET_LEVER       39


// Callback for Vesim events
void cbLMVesim(int inputID, int eventType, int newValue, void *pdata);

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
		SRF_INDICATOR = 1,
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
		SRF_BORDER_47x43,
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
		SRF_BORDER_40x40,
		SRF_BORDER_126x131,
		SRF_BORDER_115x115,
		SRF_BORDER_68x68,
		SRF_BORDER_169x168,
		SRF_BORDER_67x64,
		SRF_BORDER_201x205,
		SRF_BORDER_122x265,
		SRF_BORDER_225x224,
		SRF_BORDER_51x54,
		SRF_BORDER_205x205,
		SRF_BORDER_30x144,
		SRF_BORDER_400x400,
		SRF_BORDER_1001x240,
		SRF_BORDER_360x316,
		SRF_BORDER_178x187,
		SRF_BORDER_55x55,
		SRF_BORDER_109x119,
		SRF_BORDER_68x69,
		SRF_BORDER_210x200,
		SRF_BORDER_104x106,
		SRF_BORDER_286x197,
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
		SRF_AOT_FONT,
		SRF_FIVE_POS_SWITCH,
		SRF_DEDA_KEY,
		SRF_DEDA_LIGHTS,
		SRF_LEM_STAGESWITCH,
		SRF_DIGITALDISP2,
		SRF_RR_NOTRACK,
		SRF_RADAR_TAPE,
		SRF_RADAR_TAPE2,
		SRF_ORDEAL_PANEL,
		SRF_ORDEAL_ROTARY,
		SRF_TW_NEEDLE,
		SRF_SEQ_LIGHT,
		SRF_LMENGINE_START_STOP_BUTTONS,
		SRF_LMTRANSLBUTTON,
		SRF_LEMVENT,
		SRF_LEM_ACT_OVRD,
		SRF_LEM_CAN_SEL,
		SRF_LEM_ECS_ROTARY,
		SRF_LEM_H20_SEL,
		SRF_LEM_H20_SEP,
		SRF_LEM_ISOL_ROTARY,
		SRF_LEM_PRIM_C02,
		SRF_LEM_SEC_C02,
		SRF_LEM_SGD_LEVER,
		SRF_LEM_U_HATCH_REL_VLV,
		SRF_LEM_U_HATCH_HNDL,
		SRF_LEM_F_HATCH_HNDL,
		SRF_LEM_F_HATCH_REL_VLV,
		SRF_LEM_INTLK_OVRD,
		SRF_RED_INDICATOR,
		SRF_LEM_MASTERALARM,
		SRF_PWRFAIL_LIGHT,

		//
		// NSURF MUST BE THE LAST ENTRY HERE. PUT ANY NEW SURFACE IDS ABOVE THIS LINE
		//
		nsurf	///< nsurf gives the count of surfaces for the array size calculation.
	};

	enum SurfaceID_VC
	{
		//
		// First value in the enum must be set to one. Entry zero is not
		// used.
		//

		// VC Surfaces
		SRF_VC_DSKYDISP,
		SRF_VC_DSKY_LIGHTS,
		SRF_VC_DIGITALDISP,
		SRF_VC_DIGITALDISP2,
		SRF_VC_RADAR_TAPEA,
		SRF_VC_RADAR_TAPEB,
		SRF_VC_RADAR_TAPE2,
		SFR_VC_CW_LIGHTS,
		SRF_INDICATORVC,
		SRF_INDICATORREDVC,
		SRF_LEM_MASTERALARMVC,
		SRF_DEDA_LIGHTSVC,
		SRF_AOTFONT_VC,
		SRF_ENGSTARTSTOP_VC,

		//
		// NSURF MUST BE THE LAST ENTRY HERE. PUT ANY NEW SURFACE IDS ABOVE THIS LINE
		//
		nsurfvc	///< nsurfvc gives the count of surfaces for the array size calculation.
	};

	LEM(OBJHANDLE hObj, int fmodel);
	virtual ~LEM();

	void Init();
	void SetStateEx(const void *status);
	void SetLmVesselDockStage();
	void SetLmVesselHoverStage();
	void SetLmAscentHoverStage();
	void SetLmDockingPort(double offs);
	void SetLmLandedMesh();
	void SetLMMeshVis();
	void SetLMMeshVisAsc();
	void SetLMMeshVisVC();
	void SetLMMeshVisDsc();
	void SetCrewMesh();
	void DrogueVis();
	void HideProbes();
	void HideDeflectors();
	void ShowXPointerShades();
	void SetTrackLight();
	void SetDockingLights();
	void SetCOAS();
	void SetWindowShades();
	double GetMissionTime() { return MissionTime; }; // This must be here for the MFD can't use it.
	int GetApolloNo() { return ApolloNo; }
	UINT GetStage() { return stage; }
	virtual double GetAscentStageMass();

	virtual void PlayCountSound(bool StartStop) {};
	virtual void PlaySepsSound(bool StartStop) {};
	virtual void LMSLASeparationFire() {};

	bool clbkLoadPanel (int id);
	bool clbkLoadVC(int id);
	bool clbkPanelMouseEvent (int id, int event, int mx, int my);
	bool clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf);

	bool clbkVCMouseEvent(int id, int event, VECTOR3 &p);
	bool clbkVCRedrawEvent(int id, int event, SURFHANDLE surf);

	int clbkConsumeDirectKey(char* keystate);
	int  clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkPreStep (double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	void clbkLoadStateEx (FILEHANDLE scn, void *vs);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkLoadGenericCockpit ();
	void clbkMFDMode (int mfd, int mode);
	void clbkPostCreation();
	void clbkVisualCreated(VISHANDLE vis, int refcount);
	void clbkVisualDestroyed(VISHANDLE vis, int refcount);
	void clbkDockEvent(int dock, OBJHANDLE connected);
	void clbkFocusChanged(bool getfocus, OBJHANDLE hNewVessel, OBJHANDLE hOldVessel);
	void clbkGetRadiationForce(const VECTOR3& mflux, VECTOR3& F, VECTOR3& pos);

	void GetScenarioState(FILEHANDLE scn, void *vs);
	void SetGenericStageState(int stat);
	void PostLoadSetup(bool define_anims = true);
	void DefineAnimations();
	void SetMeshes();
	void RegisterActiveAreas();

	void RCSHeaterSwitchToggled(TwoPositionSwitch *s, int *pump);
	void PanelSwitchToggled(TwoPositionSwitch *s);
	void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s); 
	void PanelRotationalSwitchChanged(RotationalSwitch *s);
	void PanelThumbwheelSwitchChanged(ThumbwheelSwitch *s);

	void PanelRefreshForwardHatch();
	void PanelRefreshOverheadHatch();

	// Panel SDK
	void SetPipeMaxFlow(char *pipe, double flow);
	h_Pipe* GetLMTunnelPipe();
	h_Valve* GetCSMO2HoseOutlet();
	void ConnectTunnelToCabinVent();
	double GetRCSQuadTempF(int index);
	virtual void GetECSStatus(LEMECSStatus &ecs);
	virtual void SetCrewNumber(int number);
	virtual void SetCDRInSuit();
	virtual void SetLMPInSuit();
	virtual void StartEVA();
	void StartSeparationPyros();
	void StopSeparationPyros();

	h_Tank *DesO2Tank;
	h_Tank *AscO2Tank1;
	h_Tank *AscO2Tank2;
	h_Tank *DesO2Manifold;
	h_Tank *O2Manifold;
	h_Tank *PressRegA;
	h_Tank *PressRegB;
	h_Tank *DesH2OTank;
	h_Tank *DesBatCooling;
	h_crew *CrewInCabin;
	h_crew *CDRSuited;
	h_crew *LMPSuited;
	Pump *CabinFan1;
	Pump *SuitFan1;
	Pump *SuitFan2;
	Pump *PrimGlyPump1;
	Pump *PrimGlyPump2;
	Pump *SecGlyPump;
	Pump *LCGPump;

	h_HeatLoad *CabinHeat;
	h_HeatLoad *SuitFan1Heat;
	h_HeatLoad *SuitFan2Heat;
	h_HeatLoad *SecGlyPumpHeat;

	Boiler *RCSHtr1Quad1;
	Boiler *RCSHtr1Quad2;
	Boiler *RCSHtr1Quad3;
	Boiler *RCSHtr1Quad4;
	Boiler *RCSHtr2Quad1;
	Boiler *RCSHtr2Quad2;
	Boiler *RCSHtr2Quad3;
	Boiler *RCSHtr2Quad4;

	h_Radiator *LMQuad1RCS;
	h_Radiator *LMQuad2RCS;
	h_Radiator *LMQuad3RCS;
	h_Radiator *LMQuad4RCS;

	// DS20060416 RCS management
	void SetRCSJet(int jet,bool fire);
	void SetRCSJetLevelPrimary(int jet, double level);
	double GetRCSThrusterLevel(int jet);

	// DS20160916 Physical parameters updation
	double CurrentFuelWeight, LastFuelWeight; // Fuel weights right now and at the last update
	VECTOR3 currentCoG; // 0,0,0 corresponds to CoG at center of mesh

	//
	// These functions must be virtual so they can be called from the Saturn V or the LEVA
	//

	virtual bool SetupPayload(PayloadSettings &ls);
	virtual void PadLoad(unsigned int address, unsigned int value);
	virtual void AEAPadLoad(unsigned int address, unsigned int value);
	virtual void StopEVA(bool isCDR);
	virtual bool IsForwardHatchOpen() { return ForwardHatch.IsOpen(); }

	char *getOtherVesselName() { return agc.OtherVesselName;};
	APSPropellantSource *GetAPSPropellant() { return &APSPropellant; };
	DPSPropellantSource *GetDPSPropellant() { return &DPSPropellant; };

	///
	/// \brief Triggers Virtual AGC core dump
	///
	virtual void VirtualAGCCoreDump() { agc.VirtualAGCCoreDump("ProjectApollo LGC.core"); }

	PROPELLANT_HANDLE ph_RCSA,ph_RCSB;   // RCS Fuel A and B, replaces ph_rcslm0
	PROPELLANT_HANDLE ph_Dsc, ph_Asc; // handles for propellant resources
	THRUSTER_HANDLE th_hover[1];               // handles for orbiter main engines
	// There are 16 RCS. 4 clusters, 4 per cluster.
	THRUSTER_HANDLE th_rcs[16];
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
	Vesim vesim;                          ///< Vessel Specific Input Mngr
	int rhc_id;							  // Joystick # for the RHC
	int rhc_rot_id;						  // ID of ROTATOR axis to use for RHC Z-axis
	int rhc_sld_id;                       // ID of SLIDER axis to use for throttle control from joystick configured as ACA
	int rhc_rzx_id;                       // Flag to use native Z-axis as RHC Z-axis
	int rhc_pov_id;						  ///< ID of the cooliehat a.k.a. POV
	int thc_id;                           // Joystick # for the THC
	int thc_rot_id;						  // ID of ROTATOR axis to use for THC Z-axis
	int thc_rzx_id;                       // Flag to use native Z-axis as THC Z-axis	
	int thc_pov_id;						  ///< ID of the cooliehat a.k.a. POV
	int thc_tjt_id;                       // Flag to use axis as TTCA THROTTLE/JETS select lever
	int rhc_debug;						  // Flags to print debugging messages.
	int thc_debug;
	bool rhc_auto;						  ///< RHC Z-axis auto detection
	bool thc_auto;						  ///< THC Z-axis auto detection
	bool rhc_thctoggle;					  ///< Enable RHC/THC toggle
	int rhc_thctoggle_id;				  ///< RHC button id for RHC/THC toggle
	bool enableVESIM;                     ///< Vessel Specific Input Mgmt enabled
	bool rhc_thctoggle_pressed;			  ///< Button pressed flag
	int ttca_throttle_pos;                // TTCA THROTTLE-mode position
	double ttca_throttle_pos_dig;		  // TTCA THROTTLE-mode position mapped to 0-1
	int ttca_throttle_vel;
	int js_current;

	// Variables for checklists
	char Checklist_Variable[16][32];

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

	void UpdateMassAndCoG();
	void RedrawPanel_XPointer (CrossPointer *cp, SURFHANDLE surf);
	void RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset);
	void MousePanel_MFDButton(int mfd, int event, int mx, int my);
	void ReleaseSurfaces ();
	void ReleaseSurfacesVC();
	void ResetThrusters();
	virtual void SeparateStage (UINT stage);
	void CheckDescentStageSystems();
	void CreateMissionSpecificSystems();
	void InitPanel (int panel);
	void InitPanelVC();
	void SetSwitches(int panel);
	void AddRCS_LMH(double TRANY);
	void ToggleEVA(bool isCDR);
	void SetView();
	void RedrawPanel_Horizon (SURFHANDLE surf);
	void RedrawPanel_AOTReticle (SURFHANDLE surf);
	void SwitchClick();
	void VoxSound();
	void ButtonClick();
	void GuardClick();
	void AbortFire();
	void InitSwitches();
	void InitVCAnimations();
	void DefineVCAnimations();
	void DoFirstTimestep();
	void LoadDefaultSounds();
	void EngineSoundTimestep();
	// void GetDockStatus();
	void JostleViewpoint(double amount);
	void VCFreeCam(VECTOR3 dir, bool slow);
	void AddDust();
	void SetCompLight(int m, bool state);
	void SetContactLight(int m, bool state);
	void SetPowerFailureLight(int m, bool state);
	void SetStageSeqRelayLight(int m, bool state);
	void InitFDAI(UINT mesh);

	// LM touchdown points
	// mass in kg, ro1 (distance from center of the middle points), ro2 (distance from center of footpad points), tdph (height of footpad points),
	// probeh (height of the probes), height (of the top point), x_target (stiffness/damping factor, stable default is -0.5)
	void ConfigTouchdownPoints(double mass, double ro1, double ro2, double tdph, double probeh, double height, double x_target = -0.5);
	void DefineTouchdownPoints(int s);

	void CalculatePMIandCOG(VECTOR3 &PMI, VECTOR3 &COG);
	void CreateAirfoils();

	void SystemsTimestep(double simt, double simdt);
	void SystemsInit();
	void SystemsInternalTimestep(double simdt);
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
	PanelSwitchesVC MainPanelVC;
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

	SwitchRow LeftMasterAlarmSwitchRow;
	LEMMasterAlarmSwitch LeftMasterAlarmSwitch;

	SwitchRow LeftMonitorSwitchRow;
	ToggleSwitch RateErrorMonSwitch;
	ToggleSwitch AttitudeMonSwitch;

	SwitchRow MPSRegControlLeftSwitchRow;
	LEMSCEATalkback ASCHeReg1TB;
	LEMDPSValveTalkback DESHeReg1TB;
	ThreePosSwitch ASCHeReg1Switch;	
	ThreePosSwitch DESHeReg1Switch;
	
	SwitchRow MPSRegControlRightSwitchRow;
	LEMSCEATalkback ASCHeReg2TB;
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
	LMCO2Meter LMCO2Meter;

	SwitchRow ECSLowerIndicatorRow;
	LMGlycolTempMeter LMGlycolTempMeter;
	LMGlycolPressMeter LMGlycolPressMeter;
	LMOxygenQtyMeter LMOxygenQtyMeter;
	LMWaterQtyMeter LMWaterQtyMeter;

	SwitchRow RightMasterAlarmSwitchRow;
	LEMMasterAlarmSwitch RightMasterAlarmSwitch;

	SwitchRow RightMonitorSwitchRow;
	ToggleSwitch RightRateErrorMonSwitch;
	ToggleSwitch RightAttitudeMonSwitch;

	SwitchRow TempPressMonRotaryRow;
	RotationalSwitch TempPressMonRotary;

	SwitchRow RCSAscFeedTBSwitchRow;
	LEMSCEATalkback RCSAscFeed1ATB;
	LEMSCEATalkback RCSAscFeed2ATB;
	LEMSCEATalkback RCSAscFeed1BTB;
	LEMSCEATalkback RCSAscFeed2BTB;

	SwitchRow RCSAscFeedSwitchRow;
	ThreePosSwitch RCSAscFeed1ASwitch;
	ThreePosSwitch RCSAscFeed2ASwitch;
	ThreePosSwitch RCSAscFeed1BSwitch;
	ThreePosSwitch RCSAscFeed2BSwitch;

	SwitchRow RCSQuad14TBSwitchRow;
	LEMRCSQuadTalkback RCSQuad1ACmdEnableTB;
	LEMRCSQuadTalkback RCSQuad4ACmdEnableTB;
	LEMRCSQuadTalkback RCSQuad1BCmdEnableTB;
	LEMRCSQuadTalkback RCSQuad4BCmdEnableTB;

	SwitchRow RCSQuad14SwitchRow;
	LGCThrusterPairSwitch RCSQuad1ACmdEnableSwitch;
	LGCThrusterPairSwitch RCSQuad4ACmdEnableSwitch;
	LGCThrusterPairSwitch RCSQuad1BCmdEnableSwitch;
	LGCThrusterPairSwitch RCSQuad4BCmdEnableSwitch;

	SwitchRow RCSQuad23TBSwitchRow;
	LEMRCSQuadTalkback RCSQuad2ACmdEnableTB;
	LEMRCSQuadTalkback RCSQuad3ACmdEnableTB;
	LEMRCSQuadTalkback RCSQuad2BCmdEnableTB;
	LEMRCSQuadTalkback RCSQuad3BCmdEnableTB;

	SwitchRow RCSQuad23SwitchRow;
	LGCThrusterPairSwitch RCSQuad2ACmdEnableSwitch;
	LGCThrusterPairSwitch RCSQuad3ACmdEnableSwitch;
	LGCThrusterPairSwitch RCSQuad2BCmdEnableSwitch;
	LGCThrusterPairSwitch RCSQuad3BCmdEnableSwitch;

	SwitchRow RCSXfeedTBSwitchRow;
	LEMSCEATalkback RCSXFeedTB;

	SwitchRow RCSXfeedSwitchRow;
	ThreePosSwitch RCSXFeedSwitch;

	SwitchRow RCSMainSOVTBRow;
	LEMSCEATalkback RCSMainSovATB;
	LEMSCEATalkback RCSMainSovBTB;

	SwitchRow RCSMainSOVSwitchRow;
	ThreePosSwitch RCSMainSovASwitch;
	ThreePosSwitch RCSMainSovBSwitch;

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
	ThreePosSwitch ModeControlPGNSSwitch;
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

	///////////////////////
	// DEDAs             //
	///////////////////////

	SwitchRow DedaSwitchRow;
	DEDAPushSwitch DedaSwitchPlus;
	DEDAPushSwitch DedaSwitchMinus;
	DEDAPushSwitch DedaSwitchZero;
	DEDAPushSwitch DedaSwitchOne;
	DEDAPushSwitch DedaSwitchTwo;
	DEDAPushSwitch DedaSwitchThree;
	DEDAPushSwitch DedaSwitchFour;
	DEDAPushSwitch DedaSwitchFive;
	DEDAPushSwitch DedaSwitchSix;
	DEDAPushSwitch DedaSwitchSeven;
	DEDAPushSwitch DedaSwitchEight;
	DEDAPushSwitch DedaSwitchNine;
	DEDAPushSwitch DedaSwitchClear;
	DEDAPushSwitch DedaSwitchHold;
	DEDAPushSwitch DedaSwitchReadOut;
	DEDAPushSwitch DedaSwitchEnter;

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

	bool CMPowerToCDRBusRelayA, CMPowerToCDRBusRelayB; //Relays 3K3 and 3K4

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
	LEMSCEATalkback EDLGTB;
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
	CDRCOASPowerSwitch CDRCOASSwitch;

	bool COASswitch;
	int COASreticlevisible;   // 0 = OFF  1 = FWD  2 = OVHD

	//////////////////
	// LEM panel 14 //
	//////////////////

	SwitchRow EPSP14VoltMeterSwitchRow;
	LEMDCVoltMeter EPSDCVoltMeter;

	VoltageAttenuator ACVoltsAttenuator;

	SwitchRow EPSP14AmMeterSwitchRow;
	LEMDCAmMeter EPSDCAmMeter;
	
	SwitchRow EPSLeftControlArea;
	RotationalSwitch  EPSMonitorSelectRotary;
	LEMInverterSwitch EPSInverterSwitch;
	ThreeSourceSwitch EPSEDVoltSelect;

	SwitchRow DSCHiVoltageSwitchRow;
	ThreePosSwitch DSCSEBat1HVSwitch;
	ThreePosSwitch DSCSEBat2HVSwitch;
	ThreePosSwitch DSCCDRBat3HVSwitch;
	ThreePosSwitch DSCCDRBat4HVSwitch;
	ThreePosSwitch DSCBattFeedSwitch;

	SwitchRow DSCLoVoltageSwitchRow;
	ThreePosSwitch DSCSEBat1LVSwitch;
	ThreePosSwitch DSCSEBat2LVSwitch;
	ThreePosSwitch DSCCDRBat3LVSwitch;
	ThreePosSwitch DSCCDRBat4LVSwitch;

	SwitchRow DSCBatteryTBSwitchRow;
	LEMDoubleSCEATalkback DSCBattery1TB;
	LEMDoubleSCEATalkback DSCBattery2TB;
	LEMDoubleSCEATalkback DSCBattery3TB;
	LEMDoubleSCEATalkback DSCBattery4TB;
	IndicatorSwitch DSCBattFeedTB;

	SwitchRow ASCBatteryTBSwitchRow;
	LEMSCEATalkback ASCBattery5ATB;
	LEMSCEATalkback ASCBattery5BTB;
	LEMSCEATalkback ASCBattery6ATB;
	LEMSCEATalkback ASCBattery6BTB;

	SwitchRow ASCBatterySwitchRow;
	ThreePosSwitch ASCBat5SESwitch;
	ThreePosSwitch ASCBat5CDRSwitch;
	ThreePosSwitch ASCBat6CDRSwitch;
	ThreePosSwitch ASCBat6SESwitch;
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

	PowerMerge DescentECAMainFeeder;
	PowerMerge DescentECAContFeeder;
	PowerMerge AscentECAMainFeeder;
	PowerMerge AscentECAContFeeder;

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

	///////////////
	// ECS Panel //
	///////////////

	SwitchRow ECSSuitGasDiverterSwitchRow;
	CircuitBrakerSwitch SuitGasDiverterSwitch;

	SwitchRow OxygenControlSwitchRow;
	RotationalSwitch CabinRepressValveSwitch;
	RotationalSwitch PLSSFillValve;
	RotationalSwitch PressRegAValve;
	RotationalSwitch PressRegBValve;
	RotationalSwitch DESO2Valve;
	AscentO2RotationalSwitch ASCO2Valve1;
	AscentO2RotationalSwitch ASCO2Valve2;
	PushSwitch IntlkOvrd;

	SwitchRow SuitIsolSwitchRow;
	RotationalSwitch CDRSuitIsolValve;
	ToggleSwitch	   CDRActuatorOvrd;
	RotationalSwitch LMPSuitIsolValve;
	ToggleSwitch     LMPActuatorOvrd;

	SwitchRow WaterControlSwitchRow;
	RotationalSwitch SecEvapFlowValve;
	RotationalSwitch PrimEvap2FlowValve;
	RotationalSwitch DESH2OValve;
	RotationalSwitch PrimEvap1FlowValve;
	RotationalSwitch WaterTankSelectValve;
	LMSuitTempRotationalSwitch SuitTempValve;

	SwitchRow ASCH2OSwitchRow;
	RotationalSwitch ASCH2OValve;

	SwitchRow GarmentCoolingSwitchRow;
	LMLiquidGarmentCoolingRotationalSwitch LiquidGarmentCoolingValve;

	SwitchRow SuitCircuitAssySwitchRow;
	RotationalSwitch SuitCircuitReliefValveSwitch;
	RotationalSwitch CabinGasReturnValveSwitch;
	ToggleSwitch CO2CanisterSelectSwitch;
	RotationalSwitch CO2CanisterPrimValve;
    PushSwitch       CO2CanisterPrimVent;
	RotationalSwitch CO2CanisterSecValve;
    PushSwitch       CO2CanisterSecVent;
	CircuitBrakerSwitch WaterSepSelectSwitch;

	/////////////////////
	// LEM Upper Hatch //
	/////////////////////

	SwitchRow UpperHatchHandleSwitchRow;
	LMOverheadHatchHandle UpperHatchHandle;
	SwitchRow UpperHatchValveSwitchRow;
	ThreePosSwitch UpperHatchReliefValve;
	SwitchRow UilityLightSwitchRow;
	ThreePosSwitch UtilityLightSwitchCDR;
	ThreePosSwitch UtilityLightSwitchLMP;
	
	///////////////////////
	// LEM Forward Hatch //
	///////////////////////

	SwitchRow ForwardHatchHandleSwitchRow;
	LMForwardHatchHandle ForwardHatchHandle;
	SwitchRow ForwardHatchValveSwitchRow;
	ThreePosSwitch ForwardHatchReliefValve;
	
	///////////////////////////
	// LEM Rendezvous Window //
	///////////////////////////

	int LEMCoas1Enabled;
	int LEMCoas2Enabled;

	///////////////////////
	// LEM Window Shades //
	///////////////////////

	int LEMWindowShades;

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
	PowerMerge AOTLampFeeder;
	PowerMerge NumDockCompLTGFeeder;

	int ordealEnabled;

	bool FirstTimestep;

	bool ToggleEva;
	bool EVA_IP[2];

	int CDRinPLSS;
	int LMPinPLSS;

#define LMVIEW_CDR		 0
#define LMVIEW_LMP		 1
#define LMVIEW_LPD		 2
#define LMVIEW_DSKY		 3
#define LMVIEW_CBLEFT    4
#define LMVIEW_CBRIGHT   5
#define LMVIEW_AOT		 6
#define LMVIEW_FWDHATCH	 7
#define LMVIEW_OVHDHATCH 8
#define LMVIEW_ECS		 9
#define LMVIEW_ECS2		 10
#define LMVIEW_RDVZWIN   11

#define VIEWANGLE 30

	int	viewpos;

	bool Crewed;
	bool AutoSlow;

	OBJHANDLE hLEVA[2];
	OBJHANDLE hdsc;

	ATTACHMENTHANDLE hattDROGUE;

	DOCKHANDLE docksla;

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
	h_HeatExchanger *imublower; // IMU Blower
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
	Pyro AscentHeliumIsol1Pyros;
	Pyro AscentHeliumIsol2Pyros;
	Pyro AscentOxidCompValvePyros;
	Pyro AscentFuelCompValvePyros;
	Pyro RCSHeliumSupplyAPyros;
	Pyro RCSHeliumSupplyBPyros;
	PowerMerge LandingGearPyrosFeeder;
	PowerMerge CableCuttingPyrosFeeder;
	PowerMerge DescentPropVentPyrosFeeder;
	PowerMerge DescentEngineStartPyrosFeeder;
	PowerMerge DescentEngineOnPyrosFeeder;
	PowerMerge DescentPropIsolPyrosFeeder;
	PowerMerge AscentHeliumIsol1PyrosFeeder;
	PowerMerge AscentHeliumIsol2PyrosFeeder;
	PowerMerge AscentOxidCompValvePyrosFeeder;
	PowerMerge AscentFuelCompValvePyrosFeeder;
	PowerMerge RCSHeliumSupplyAPyrosFeeder;
	PowerMerge RCSHeliumSupplyBPyrosFeeder;

	// Some stuff on init should be done only once
	bool InitLEMCalled;

	MissionTimer MissionTimerDisplay;
	LEMEventTimer EventTimerDisplay;

	double DescentFuelMassKg;	///< Mass of fuel in descent stage of LEM.
	double AscentFuelMassKg;	///< Mass of fuel in ascent stage of LEM.
	double DescentEmptyMassKg;
	double AscentEmptyMassKg;

	// Mesh indexes
	UINT ascidx;
	UINT dscidx;
	UINT vcidx;
	UINT windowshadesidx;
	UINT xpointershadesidx;

	DEVMESHHANDLE probes;
	DEVMESHHANDLE deflectors;
	DEVMESHHANDLE drogue;
	DEVMESHHANDLE cdrmesh;
	DEVMESHHANDLE lmpmesh;
	DEVMESHHANDLE vcmesh;

	// VC animations
	UINT anim_fdaiR_cdr, anim_fdaiR_lmp;
	UINT anim_fdaiP_cdr, anim_fdaiP_lmp;
	UINT anim_fdaiY_cdr, anim_fdaiY_lmp;
	UINT anim_fdaiRerror_cdr, anim_fdaiRerror_lmp;
	UINT anim_fdaiPerror_cdr, anim_fdaiPerror_lmp;
	UINT anim_fdaiYerror_cdr, anim_fdaiYerror_lmp;
	UINT anim_fdaiRrate_cdr, anim_fdaiRrate_lmp;
	UINT anim_fdaiPrate_cdr, anim_fdaiPrate_lmp;
	UINT anim_fdaiYrate_cdr, anim_fdaiYrate_lmp;
	UINT anim_attflag_cdr, anim_attflag_lmp;

	// Dust particles
	THRUSTER_HANDLE th_dust[4];
	THGROUP_HANDLE thg_dust;

	// Exterior light definitions
	BEACONLIGHTSPEC trackLight;                   // tracking light
	BEACONLIGHTSPEC dockingLights[5];             // docking lights

	VECTOR3 trackLightPos;
	VECTOR3 dockingLightsPos[5];

#define LMPANEL_MAIN			0
#define LMPANEL_RIGHTWINDOW		1
#define LMPANEL_LEFTWINDOW		2
#define LMPANEL_LPDWINDOW		3
#define LMPANEL_RNDZWINDOW		4
#define LMPANEL_LEFTPANEL		5
#define LMPANEL_AOTVIEW			6
#define LMPANEL_RIGHTPANEL		7
#define LMPANEL_ECSPANEL		8
#define LMPANEL_DOCKVIEW		9
#define LMPANEL_AOTZOOM		    10
#define LMPANEL_LEFTZOOM        11   
#define LMPANEL_UPPERHATCH      12  
#define LMPANEL_FWDHATCH        13 

	bool InVC;
	bool InPanel;
	bool ExtView;
	bool InFOV;  
	int  PanelId; 
	double SaveFOV;
	bool CheckPanelIdInTimestep;
	bool RefreshPanelIdInTimestep;
	bool VcInfoActive;
	bool VcInfoEnabled;

	double visibilitySize;

	//
	// Random motion.
	//

	double ViewOffsetx;
	double ViewOffsety;
	double ViewOffsetz;

	//
	// VC Free Cam
	//

	double vcFreeCamx;
	double vcFreeCamy;
	double vcFreeCamz;
	double vcFreeCamSpeed;
	double vcFreeCamMaxOffset;

	//
	// Ground Systems
	//
	MCC *pMCC;
	OBJHANDLE hMCC;

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
	FadeInOutSound CabinFans;
	Sound Vox;
	Sound Afire;
	Sound Slanding;
	Sound RCSFireSound;
	Sound RCSSustainSound;
	Sound HatchOpenSound;
	Sound HatchCloseSound;
	FadeInOutSound GlycolPumpSound;
	FadeInOutSound SuitFanSound;
	Sound CrewDeadSound;
	Sound EngineS;

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
	LEMECSConnector CSMToLEMECSConnector;
	LMToSIVBConnector LEMToSLAConnector;
	LEMCommandConnector CSMToLEMCommandConnector;

	LM_RRtoCSM_RRT_Connector lm_rr_to_csm_connector;
	LM_VHFtoCSM_VHF_Connector lm_vhf_to_csm_csm_connector;

	// Checklist Controller to LEM connector
	ChecklistDataInterface cdi;

	char AudioLanguage[64];

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

	//Relay Junction Box
	LEM_RelayJunctionBox rjb;

	//Deadface Relay Box
	LEM_DeadfaceRelayBox drb;

	// XLUNAR Bus Controller
	LEM_XLBControl BTC_XLunar;

	// ECA
	LEM_DescentECA ECA_1;
	LEM_DescentECA ECA_2;
	LEM_AscentECA ECA_3;
	LEM_AscentECA ECA_4;

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
	InertialData inertialData;
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
	LEM_TTCA CDR_TTCA;
	LEM_AEAA *aeaa;

	LEM_RadarTape RadarTape;
	LEM_CWEA CWEA;

	// COMM
	LEM_SteerableAnt SBandSteerable;
	LM_OMNI omni_fwd;
	LM_OMNI omni_aft;
	LM_VHF VHF;
	LM_SBAND SBand;
	LM_DSEA DSEA;
	LM_PCM PCM;

	//Lighting
	LEM_TLE tle;
	LEM_DockLights DockLights;
	LEM_LCA lca;
	LEM_UtilLights UtilLights;
	LEM_COASLights COASLights;
	LEM_FloodLights FloodLights;
	LEM_PFIRA pfira;

	// ECS
	LEM_ECS ecs;
	LEMPressureSwitch CabinPressureSwitch;
	LEMPressureSwitch SuitPressureSwitch;
	LEMSuitIsolValve CDRIsolValve;
	LEMSuitIsolValve LMPIsolValve;
	LEMSuitCircuitPressureRegulator SuitCircuitPressureRegulatorA;
	LEMSuitCircuitPressureRegulator SuitCircuitPressureRegulatorB;
	LEMCabinRepressValve CabinRepressValve;
	LEMOverheadHatch OverheadHatch;
	LEMOVHDCabinReliefDumpValve OVHDCabinReliefDumpValve;
	LEMForwardHatch ForwardHatch;
	LEMFWDCabinReliefDumpValve FWDCabinReliefDumpValve;
	LEMSuitCircuitReliefValve SuitCircuitReliefValve;
	LEMCabinGasReturnValve CabinGasReturnValve;
	LEMSuitGasDiverter SuitGasDiverter;
	LEMCO2CanisterSelect CO2CanisterSelect;
	LEMCO2CanisterVent PrimCO2CanisterVent;
	LEMCO2CanisterVent SecCO2CanisterVent;
	LEMWaterSeparationSelector WaterSeparationSelector;
	LEMCabinFan CabinFan;
	LEMWaterTankSelect WaterTankSelect;
	LEMPrimGlycolPumpController PrimGlycolPumpController;
	LEMSuitFanDPSensor SuitFanDPSensor;
	LEMCrewStatus CrewStatus;

	// EDS
	LEM_EDS eds;

	// DPS and APS
	DPSPropellantSource DPSPropellant;
	LEM_DPS DPS;
	APSPropellantSource APSPropellant;
	LEM_APS APS;

	// RCS
	RCSPropellantSource RCSA;
	RCSPropellantSource RCSB;
	RCS_TCA tca1A;
	RCS_TCA tca2A;
	RCS_TCA tca3A;
	RCS_TCA tca4A;
	RCS_TCA tca1B;
	RCS_TCA tca2B;
	RCS_TCA tca3B;
	RCS_TCA tca4B;

	// Abort Guidance System stuff
	LEM_ASA asa;
	LEM_AEA aea;
	LEM_DEDA deda;

	// Instrumentation
	SCERA1 scera1;
	SCERA2 scera2;

	bool isMultiThread;

	mission::Mission* pMission;

	// Friend classes
	friend class ATCA;
	friend class LEM_EDS;
	friend class LEM_EDRelayBox;
	friend class LEMcomputer;
	friend class LEMDCVoltMeter;
	friend class LEMDCAmMeter;
	friend class LMOptics;
	friend class LEMInverterSwitch;
	friend class LEM_BusCrossTie;
	friend class LEM_XLBControl;
	friend class LEM_DescentECASector;
	friend class LEM_AscentECASector;
	friend class LEM_RelayJunctionBox;
	friend class LEM_DeadfaceRelayBox;
	friend class LEM_LR;
	friend class LEM_RR;
	friend class LEM_RadarTape;
	friend class LEM_TLE;
	friend class LEM_DockLights;
	friend class LEM_FloodLights;

	friend class LEM_ASA;
	friend class LEM_AEA;
	friend class LEM_DEDA;
	friend class LEM_SteerableAnt;
	friend class LM_VHF;
	friend class LM_PCM;
	friend class LM_SBAND;
	friend class LM_DSEA;
	friend class LEMMissionTimerSwitch;
	friend class LEM_CWEA;
	friend class LMWaterQtyMeter;
	friend class LMOxygenQtyMeter;
	friend class LMGlycolPressMeter;
	friend class LMGlycolTempMeter;
	friend class LMCO2Meter;
	friend class LMCabinPressMeter;
	friend class LMSuitPressMeter;
	friend class LMCabinTempMeter;
	friend class LMSuitTempMeter;
	friend class DPSGimbalActuator;
	friend class DPSPropellantSource;
	friend class LEM_DPS;
	friend class APSPropellantSource;
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
	friend class LEM_TTCA;
	friend class SCERA1;
	friend class SCERA2;
	friend class RCSPropellantSource;
	friend class LGCThrusterPairSwitch;
	friend class LMRCSAPressInd;
	friend class LMRCSBPressInd;
	friend class LMRCSAQtyInd;
	friend class LMRCSBQtyInd;
	friend class LMRCSATempInd;
	friend class LMRCSBTempInd;
	friend class TempMonitorInd;
	friend class MainFuelTempInd;
	friend class MainOxidizerTempInd;
	friend class RCS_TCA;
	friend class LEM_ECS;
	friend class LEMCabinRepressValve;
	friend class LEMSuitIsolValve;
	friend class LEMDigitalHeliumPressureMeter;
	friend class EngineStopButton;
	friend class EngineStartButton;
	friend class LEM_LCA;
	friend class LEM_PFIRA;
	friend class LEMCrewStatus;
	friend class CDRCOASPowerSwitch;

	friend class ApolloRTCCMFD;
	friend class ARCore;
	friend class ProjectApolloMFD;
	friend class MCC;
	friend class RTCC;

	friend void cbLMVesim(int inputID, int eventType, int newValue, void *pdata);
};

extern MESHHANDLE hLMDescent;
extern MESHHANDLE hLMDescentNoLeg;
extern MESHHANDLE hLMAscent;
extern MESHHANDLE hLMVC;

extern void LEMLoadMeshes();

//Offset from center of LM mesh (full LM) to center of descent stage mesh (for staging)
const VECTOR3 OFS_LM_DSC = { 0, -1.25, 0.0 };
//Ofset from center of LM mesh (full LM) to center of ascent stage mesh (for staging)
const VECTOR3 OFS_ASC_DSC = { 0, -1.75, 0 };

#endif
