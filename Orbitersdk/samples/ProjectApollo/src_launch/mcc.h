/****************************************************************************
  This file is part of Project Apollo - NASSP

  Mission Control Center / Ground Support Network

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

#if !defined(_PA_MCC_H)
#define _PA_MCC_H
// Save file strings
#define MCC_START_STRING	"MCC_BEGIN"
#define MCC_END_STRING	    "MCC_END"

// Ground Station Tracking Capabilities bits
#define GSTK_CBAND_HIGHSPEED 0x01
#define GSTK_CBAND_LOWSPEED  0x02
#define GSTK_ODOP            0x04
#define GSTK_OPTICAL		 0x08
#define GSTK_USB			 0x10

// Ground Station Unified S-Band Capabilities bits
#define GSSC_VOICE     0x01
#define GSSC_COMMAND   0x02
#define GSSC_TELEMETRY 0x04

// Ground Station Telemetry Handling Capabilities bits
#define GSTM_VHFLINKS     0x01
#define GSTM_FMREMOTE     0x02
#define GSTM_MAGTAPE      0x04
#define GSTM_DECOMMUTATOR 0x08
#define GSTM_DISPLAYS     0x10

// Ground Station Communications Capabilities Bits
#define GSGC_DATAHISPEED  0x01
#define GSGC_DATAWIDEBAND 0x02
#define GSGC_TELETYPE     0x04
#define GSGC_SCAMA_VOICE  0x08
#define GSGC_VHFAG_VOICE  0x10
#define GSGC_VIDEO        0x20

// Ground Station USB Antenna Types
#define GSSA_3PT7METER	0x01
#define GSSA_9METER     0x02
#define GSSA_26METER    0x04

// Ground Station Downtelemetry Capabilities
#define GSDT_VHF	0x01
#define GSDT_USB	0x02

// Ground Station Command Capabilities
#define GSCC_UHF	0x01
#define GSCC_USB	0x02

// Ground Station Types (Not flags)
#define GSTP_MSFN_GROUND 1
#define GSTP_DSN_GROUND  2
#define GSTP_DOD_GROUND  3
#define GSTP_ARIA		 4
#define GSTP_SHIP        5

// Ground Station Purpose Types
#define GSPT_PRELAUNCH	0x0001
#define GSPT_LV_CUTOFF	0x0002
#define GSPT_NEAR_SPACE	0x0004
#define GSPT_LUNAR		0x0008
#define GSPT_ORBITAL	0x0010
#define GSPT_LAUNCH		0x0020
#define GSPT_TLI		0x0040
#define GSPT_ENTRY		0x0080

// Max number of ground stations
#define MAX_GROUND_STATION	43

// Message buffer limits
#define MAX_MESSAGES		10
#define MAX_MSGSIZE			128
#define MSG_DISPLAY_TIME	10

// Mission major phase numbers
#define MMST_PRELAUNCH		0
#define MMST_BOOST			1
#define MMST_EARTH_ORBIT	2
#define MMST_TL_COAST		3
#define MMST_LUNAR_ORBIT	4
#define MMST_TE_COAST		5
#define MMST_ENTRY			6

// Mission Types
// Unmanned and unflown missions are included for completeness. I don't intend to support them, or at least it's not a priority.

// MISSION A is an unmanned CSM launched with a Saturn V. Apollo 4 and Apollo 6.
#define MTP_A				0
// MISSION B is an unmanned LM launched with a Saturn 1B. Apollo 5.
#define MTP_B				1
// MISSION C is a manned CSM launched with a Saturn 1B to earth orbit for evaluation. Apollo 7.
#define MTP_C				2
// MISSION C PRIME is a manned CSM launched into lunar orbit for evaluation. Apollo 8.
#define MTP_C_PRIME			3
// MISSION D is a manned CSM and LM launched with a Saturn V to earth orbit for evaluation. This was supposed to be Apollo 8, but became Apollo 9.
#define MTP_D				4
// MISSION E was to be a manned CSM and LM, launched with a Saturn V into a 3500nm orbit to simulate lunar operations. It was never flown.
#define MTP_E				5
// MISSION F is a manned CSM and LM launched to lunar orbit for evaluation. Apollo 10.
#define MTP_F				6
// MISSION G is a manned CSM and LM launched to lunar orbit for landing. Apollo 11.
#define MTP_G				7
// MISSION H is a manned CSM and LM launched to lunar orbit for landing and up to 2-day stay. Apollo 12-14.
#define MTP_H				8
// MISSION J is a manned CSM and modified LM launched to lunar orbit for landing, LRV operations, and extended stay. Apollo 15-17.
#define MTP_J				9
// SKYLAB is a manned CSM launched with a Saturn 1B to earth orbit to dock with the SKYLAB space station.
#define MTP_SKYLAB			10
// ASTP is a manned CSM launched with a Saturn 1B to earth orbit to dock with Soyuz 19.
#define MTP_ASTP			11

//UPDATE TYPES
#define UTP_PADONLY				0
#define UTP_PADWITHCMCUPLINK	1
#define UTP_CMCUPLINKONLY		2
#define UTP_LGCUPLINKDIRECT		3
#define UTP_LGCUPLINKONLY		4
#define UTP_PADWITHLGCUPLINK	5
#define UTP_NONE				99

// PAD Types
#define PT_AP7BLK			1
#define PT_P27PAD			2
#define PT_AP7NAV			3
#define PT_AP7MNV			4
#define PT_AP7TPI			5
#define PT_AP7ENT			6
#define PT_P37PAD			7
#define PT_AP11MNV			8
#define PT_AP11ENT			9
#define PT_TLIPAD			10
#define PT_STARCHKPAD		11
#define PT_AP10MAPUPDATE	12
#define PT_AP11LMARKTRKPAD	13
#define PT_AP10DAPDATA		14
#define PT_AP11LMMNV		15
#define PT_AP10CSI			16
#define PT_AP9AOTSTARPAD	17
#define PT_TORQANG			18
#define PT_AP9LMTPI			19
#define PT_AP9LMCDH			20
#define PT_S065UPDATE		21
#define PT_AP11AGSACT		22
#define PT_AP11PDIPAD		23
#define PT_PDIABORTPAD		24
#define PT_AP11T2ABORTPAD	25
#define PT_AP11T3ABORTPAD	26
#define PT_AP11P76PAD		27
#define PT_AP11LMASCPAD		28
#define PT_LIFTOFFTIMES		29
#define PT_LMACTDATA		30
#define PT_NONE				99
#define PT_GENERIC			100

// MISSION STATES: GLOBAL
// The init state is supposed to set up the MCC state engine for the mission and dispatch accordingly. Not called when reloading.
#define MST_INIT			0
// Prelaunch is the default state. There are separate states for 1B and V just in case one needs something the other doesn't.
#define MST_1B_PRELAUNCH	1
#define MST_SV_PRELAUNCH	2
// Ends at launch time, goes to
#define MST_1B_LAUNCH		3
#define MST_SV_LAUNCH		4
// Ends at S4B cutoff, goes to the relevant mission INSERTION state.

// This is entry from Earth orbit, ala Saturn 1B missions, or an abort from EPO.
#define MST_ORBIT_ENTRY		5
// This is entry at translunar speed, in case that's different.
#define MST_ENTRY			6
// Landing phase
#define MST_LANDING			7

// MISSION STATES: ABORTS
// The abort state is entered when the astronauts or the MCC initiates a mission abort.
// Mission-specific aborts may have their own state.
// Prelaunch abort
#define MST_ABORT_PL		900
// Launch abort, return to Earth
#define MST_LAUNCH_ABORT	901
// Abort from orbit is handled by mission-specific abort state and goes to entry.

// Ground Station Information Structure
struct GroundStation {
	char Name[64];		 // Station name
	char Code[8];		 // Station ID code
	double Position[2];  // Latitude, Longitude
	bool Active;         // This entry is valid and active
	char TrackingCaps;	 // Tracking capabilities
	char USBCaps;        // Unified S-Band Capabilities
	char SBandAntenna;   // S-Band Antenna Type
	char TelemetryCaps;  // Telemetry Handling Capabilities
	char CommCaps;		 // Radio/Ground Communications Capabilities
	bool HasRadar;       // Has radar capability
	bool HasAcqAid;      // Has target acquisition aid
	char DownTlmCaps;    // Downtelemetry Capabilities
	char UpTlmCaps;      // Command Capabilities
	char StationType;    // Station Type
	int  StationPurpose; // Station Purpose
	int	 AOS;            // AOS flag
};

// PAD FORMS
// APOLLO 7 - BLOCK DATA
struct AP7BLK{
	char Area[8][10];	// XXX.YY where XXX is rev and YY is recovery area/supt caps
	double Lat[8];		// Target point
	double Lng[8];		// Target point
	double GETI[8];		// TIG
	double dVC[8];		// dV for EMS
	char Wx[8][10];		// Weather cndx ("GOOD-FAIR-POOR")
};

// APOLLO 7 - P27 CMC UPDATE
// IDENTICAL TO APOLLO 11

// APOLLO 7 - NAV CHECK
struct AP7NAV{
	double NavChk[5];	// Time for nav check
	double lat[5];		// Latitude for N43
	double lng[5];		// Longitude for N43
	double alt[5];		// Altitude for N43
};

// APOLLO 7 - MANEUVER
struct AP7MNV{
	char purpose[64];	// PURPOSE
	double GETI;		// TIG
	VECTOR3 dV;			// P30 dV
	double HA,HP;		// Predicted apogee/perigee after maneuver
	double Vc;			// EMS dV
	double Weight;		// Vehicle weight
	double pTrim,yTrim; // SPS pitch/yaw trim
	double burntime;	// Burn time
	int Star;			// Nav star for orientation check
	double Shaft,Trun;  // Shaft and trunnion values for orientation check
	VECTOR3 Att;		// Attitude at TIG
	double NavChk;		// Time for nav check
	double lat;			// Latitude for N43
	double lng;			// Longitude for N43
	double alt;			// Altitude for N43
	char remarks[128];	// remarks
};

// APOLLO 7 - TERMINAL PHASE INITIATE
// (The form doesn't match the field list?)
struct AP7TPI{
	// ON THE FORM:
	double GETI;		// TIG
	VECTOR3 Vg;			// P40 velocity to be gained
	VECTOR3 Backup_dV;	// Backup "line-of-sight to Target" dV (fore/aft, left/right, up/down)
	double dH_TPI;		// Altitude difference at TIG
	double R;			// Range from chaser to target
	double Rdot;		// Range rate
	double EL;			// Target LoS elevation relative to chaser
	double AZ;			// Target LoS azimuth relative to chaser
	// NOT ON THE FORM?
	double E;			// Elevation of target relative to S/C at TIG
	double dTT;			// Time of transfer (time from TIG to intercept)
	VECTOR3 Backup_bT;  // Burn time to get dV in backup axes
	double dH_Max,dH_Min; // Max/Min altitude difference prior to TPI
	double GET;			// Time that range,range-rate,azimuth, and elevation parameters are valid
};

// APOLLO 7 - ENTRY UPDATE
struct AP7ENT{
	// Pre-burn
	char Area[2][10];	// XXX.YY where XXX is rev and YY is recovery area/supt caps
	double dVTO[2];		// Tailoff dV from EMS
	VECTOR3 Att400K[2];	// R/P/Y gimbal angle to ensure capture
	double RTGO[2];		// Range to go from .05G
	double VIO[2];		// Inertial velocity at .05G
	double Ret05[2];	// Time from retro fire to .05G
	double Lat[2];		// Target point lat
	double Lng[2];		// Target point lng
	double Ret2[2];		// Time from retro fire to .2G
	double DRE[2];		// Downrange error at .2G
	double BankAN[2];	// Backup bank angle SCS type entry (sign = roll left/right)
	double RetRB[2];	// Ret to reverse backup bank angle
	double RetBBO[2];	// Ret to begin blackout
	double RetEBO[2];	// Ret to end blackout
	double RetDrog[2];	// Ret to drogue deploy
	// Post-burn
	double PB_R400K[2];	// Roll entry gimbal angle to ensure capture
	double PB_RTGO[2];	// Range to go from .05G
	double PB_VIO[2];	// Inertial velocity at .05G
	double PB_Ret05[2];	// Time from retro fire to .05G
	double PB_Ret2[2];	// Time from retro fire to .2G
	double PB_DRE[2];	// Downrange error at .2G
	double PB_BankAN[2];// Backup bank angle SCS type entry (sign = roll left/right)
	double PB_RetRB[2];	// Ret to reverse backup bank angle
	double PB_RetBBO[2];// Ret to begin blackout
	double PB_RetEBO[2];// Ret to end blackout
	double PB_RetDrog[2];// Ret to drogue deploy
};

// APOLLO 9 - TERMINAL PHASE INITIATE
struct AP9LMTPI {
	double GETI;		// TIG
	VECTOR3 Vg;			// P40 velocity to be gained
	double dVR;			// Total dV
	VECTOR3 Att;		// Attitude at TIG (only Roll and Pitch)
	double R;			// Range from chaser to target
	double Rdot;		// Range rate
	VECTOR3 Backup_dV;	// Backup "line-of-sight to Target" dV (fore/aft, left/right, up/down)
};

// APOLLO 9 - CONSTANT DELTA HEIGHT
struct AP9LMCDH {
	double GETI;		// TIG
	VECTOR3 Vg;			// Velocity to be gained
	double Pitch;		// FDAI pitch angle
	VECTOR3 Vg_AGS;		// AGS velocity to be gained
};

// CSM STAR CHECK UPDATE

struct STARCHKPAD
{
	double GET[2];		// Time of sunrise at start of daylight star check
	VECTOR3 Att[2];		// Gimbal angles required to place SC at proper initial attitude for daylight star check
	double TAlign[2];	// Align Time for nominal IMU orientation prior to daylight star check - if required
};


// APOLLO 11 - LUNAR ENTRY

struct AP11ENT
{
	char Area[2][10];			// Splashdown area defined by target line
	VECTOR3 Att05[2];			// Spacecraft IMU gimbal angles required for aerodynamic trim at 0.05G
	double GETHorCheck[2];		// Time of Entry attitude horizon check at EI - 17 min
	double PitchHorCheck[2];	// Pitch attitude for horizon check at EI - 17 min
	double Lat[2];				// Latitude of target point
	double Lng[2];				// Longitude of target point
	double MaxG[2];				// Predicted maximum reentry acceleration
	double V400K[2];			// Intertial velocity at entry interface
	double Gamma400K[2];		// Inertial flight path angle at entry interface
	double RTGO[2];				// Range to go from .05G to target for EMS initialization
	double VIO[2];				// Inertial velocity at .05G for EMS initialization
	double RRT[2];				// Reentry reference time based on GET of predicted 400K (GET start)
	double RET05[2];			// Time of .05G from 400K (RRT)
	double DLMax[2];			// Maximum acceptable value of predicted drag level (from CMC)
	double DLMin[2];			// Minimum acceptable value of predicted drag level (from CMC)
	double VLMax[2];			// Maximum acceptable value of exit velocity (from CMC)
	double VLMin[2];			// Minimum acceptable value of exit velocity (from CMC)
	double DO[2];				// Planned drag level during Constant G
	double RETVCirc[2];			// Time from EI that S/C velocity becomes circular
	double RETBBO[2];			// Time from EI to the beginning of blackout
	double RETEBO[2];			// Time from EI to the end of blackout
	double RETDRO[2];			// Time from EI to drogue deploy
	int SXTS[2];				// Sextant star for entry attitude check
	double SFT[2];				// Sextant shaft setting for entry attitude check
	double TRN[2];				// Sextant trunnion setting for entry attitude check
	int BSS[2];					// Boresight star for entry attitude check using the COAS
	double SPA[2];				// BSS pitch angle on COAS for entry attitude check
	double SXP[2];				// BSS X position on COAS for entry attitude check
	char LiftVector[2][4];		// Lift vector desired at .05G's based on entry corridor
	char remarks[2][128];		// Remarks
};

// APOLLO 11 - TRANSLUNAR INJECTION
struct TLIPAD{
	double TB6P;		// Predicted start of TB6	
	VECTOR3	IgnATT;		// SC attitude at ignition
	double BurnTime;	// Burn duration
	double dVC;			// dV for EMS
	double VI;			// Inertial velocity at cutoff
	VECTOR3 SepATT;		// SC attitude after S4B sep att maneuver
	VECTOR3 ExtATT;		// SC attitude at LM extraction
	char remarks[128];	// Remarks
};

// APOLLO 11 - P37 RETURN-TO-EARTH
struct P37PAD{
	// This PAD has 7 blocks on it
	double GETI[7];		// TIG
	double dVT[7];		// Total dV
	double lng[7];		// Longitude of landing point
	double GET400K[7];	// Time of entry interface
};

// APOLLO 11 - P27 CMC/LGC UPDATE
struct P27PAD{
	// This PAD has 3 blocks on the top part
	char Purpose[3][64]; // Purpose of update
	int	Verb[3];		// Verb to use for update
	double GET[3];		// Time data recorded
	int Index[3];		// 304 01 Index #
	int Data[3][19];	// Data
	// From here out there is only one block
	double NavChk;		// Time for nav check
	double lat;			// Latitude for N43
	double lng;			// Longitude for N43
	double alt;			// Altitude for N43
};

// APOLLO 11 - MANEUVER
struct AP11MNV {
	char purpose[64];	// PURPOSE
	char PropGuid[64];	// Propulsion & Guidance System
	double Weight;		// Vehicle weight
	double pTrim, yTrim;// SPS pitch/yaw trim
	double GETI;		// TIG
	VECTOR3 dV;			// P30 dV
	VECTOR3 Att;		// Attitude at TIG
	double HA, HP;		// Predicted apogee/perigee after maneuver
	double Vt;			//Total dV
	double burntime;	// Burn time
	double Vc;			// EMS dV
	int Star;			// Nav star for orientation check
	double Shaft, Trun; // Shaft and trunnion values for orientation check
	int BSSStar;		// Boresight star
	double SPA, SXP;	// Boresight star angles
	double lat;			// Latitude for N61
	double lng;			// Longitude for N61
	double RTGO;		// EMS Range
	double VI0;			// Inertial velocity for EMS
	double GET05G;		// GET at 0.05G
	char SetStars[32]; // Stars for Backup GDC Alignment
	VECTOR3 GDCangles;	// Backup GDC Align
	char remarks[128];	// remarks

	double LMWeight;	// LM weight
};

// APOLLO 11 LM - MANEUVER
struct AP11LMMNV {
	AP11LMMNV() : type(0) {}

	char purpose[64];	// PURPOSE
	double GETI;		// TIG
	VECTOR3 dV;			// P30 dV
	double HA, HP;		// Predicted apogee/perigee after maneuver
	double dVR;			// Total dV
	double burntime;	// Burn time
	VECTOR3 Att;		// Attitude at TIG (only Roll and Pitch)
	VECTOR3 dV_AGS;		// AGS DV
	int BSSStar;		// Boresight star
	double SPA, SXP;	// Boresight star angles

	char remarks[128];	// remarks
	double LMWeight;	// Vehicle weight
	double CSMWeight;	// CSM weight

	//Not part of the PAD
	VECTOR3 IMUAtt;		// Inertial Attitude at TIG

	//Optional
	double t_CSI;
	double t_TPI;

	int type; //0 = PAD with BSS, 1 = PAD without BSS, but with CSI and TPI
};

// APOLLO 11 PDI PAD
struct AP11PDIPAD {
	double GETI;		// TIG PDI
	double t_go;		// Time-to-go in P63
	double CR;			// Crossrange
	VECTOR3 Att;		// Attitude at TIG
	double DEDA231;		// Landing site radius in 100 feet
};

// APOLLO 11 AGS STATE VECTOR UPDATE
struct AP11AGSSVPAD
{
	double DEDA240; //LM Position X
	double DEDA241; //LM Position Y
	double DEDA242; //LM Position Z
	double DEDA260; //LM Velocity X
	double DEDA261; //LM Velocity Y
	double DEDA262; //LM Velocity Z
	double DEDA254; //LM Epoch Time
	double DEDA244; //CSM Position X
	double DEDA245; //CSM Position Y
	double DEDA246; //CSM Position Z
	double DEDA264; //CSM Velocity X
	double DEDA265; //CSM Velocity Y
	double DEDA266; //CSM Velocity Z
	double DEDA272; //CSM Epoch Time
};

// APOLLO 10 MAP UPDATE

struct AP10MAPUPDATE
{
	AP10MAPUPDATE() :Rev(0), LOSGET(0.0), AOSGET(0.0), PMGET(0.0), SSGET(0.0), SRGET(0.0), type(0) {}

	int Rev;		//Revolution of update
	double LOSGET;	//Time of LOS
	double AOSGET;	//Time of AOS
	double PMGET;	//Time of meridian crossing (150° or 180° W), alternative: AOS without burn
	double SSGET;	//Time of sunset
	double SRGET;	//Time of sunrise
	int type;		//0 = Only LOS/AOS and PM, 1 = Display all parameters, 2 = LOS, AOS with and AOS without LOI
};

// APOLLO 11 LANDMARK TRACKING PAD

struct AP11LMARKTRKPAD
{
	int entries;
	char LmkID[4][128];		// Landmark ID
	double T1[4];			// T1 time (landmark over horizon)
	double T2[4];			// T2 time (spacecraft at 35° elevation from landmark)
	double CRDist[4];		// landmark distance to ground track
	double Lat[4];			// landmark latitude
	double Lng05[4];		// landmark longitude divided by 2
	double Alt[4];			// landmark altitude
};

// APOLLO 10 DAP DATA PAD

struct AP10DAPDATA
{
	double ThisVehicleWeight;	// CSM weight (or LM for LM DAP PAD)
	double OtherVehicleWeight;	// LM weight (or CSM for LM DAP PAD)
	double PitchTrim;			// Pitch gimbal trim
	double YawTrim;				// Yaw gimbal trim (or roll for LM DAP PAD)
};

//APOLLO 10 CSI PAD

struct AP10CSI
{
	AP10CSI() : type(0) {}
	double t_CSI;
	double t_TPI;
	VECTOR3 dV_LVLH;
	double PLM_FDAI;
	VECTOR3 dV_AGS;
	double DEDA373;
	double DEDA275;
	int type;		//0 = Apollo 10, 1 = Apollo 11 CSI Data Card
};

//LIFTOFF TIME LIST
struct LIFTOFFTIMES
{
	LIFTOFFTIMES() : entries(0) {}
	double TIG[10];
	int entries;
	int startdigit;
};

//APOLLO 9 LM AOT STAR OBSERVATION PAD

struct AP9AOTSTARPAD
{
	double GET;		//Ground elapsed time of star observation
	int Detent;		//AOT detent position to be used
	int Star;		//Nav star to be used for test
	VECTOR3 CSMAtt;	//Inertial CSM attitude
};

//APOLLO 9 CSM S065 PHOTOGRAPHY UPDATE

struct S065UPDATE
{
	char Area[4][16];			//Site number or name of area to be photographed
	VECTOR3 FDAIAngles[4];		//FDAI Angles required to place the S/C at proper attitude for the pass
	double GETStart[4];			//Time to maneuver S/C to proper attitude to begin pass (5 minutes before the first exposure is made)
	double TAlign[4];			//Align time for nominal IMU orientation prior to the experiment (if required)
	double ExposureInterval[4];	//Time interval between exposures
	int ExposureNum[4];			//Number of exposures to be made over the site or area
	bool OrbRate[4];			//Whether the pass is to be made in orbit rate (for long passes over large areas) 
								//or in inertial attitude hold (for short passes over individual sites)
};

//GYRO TORQUING ANGLES

struct TORQANG
{
	VECTOR3 V42Angles;
};

//LM ACTIVATION DATA CARD

struct LMACTDATA
{
	double LMWeight;	// LM weight
	double CSMWeight;	// CSM weight
	double PitchTrim;	// Pitch gimbal trim
	double RollTrim;	// Roll gimbal trim
	VECTOR3 V42Angles;	// Torquing angles
};

//APOLLO 11 AGS ACTIVATION PAD

struct AP11AGSACT
{
	double KFactor;
	int DEDA224;	//Term in O.I. Semimajor Axis
	int DEDA225;	//Predicted O.I. LM Semi-Major Axis Lower Limit
	int DEDA226;	//Predicted O.I. LM Semi-Major Axis Upper Limit
	int DEDA227;	//Factor in O.I. LM Desired Semi-major Axis
};

//APOLLO 11 PDI ABORT PAD

struct PDIABORTPAD
{
	PDIABORTPAD() : type(0) {}
	double T_TPI_Pre10Min;	//GET of TPI maneuver for abort prior to PDI+10 minutes
	double T_Phasing;		//GET of Phasing maneuver for abort subsequent to PDI+10 minutes
	double T_TPI_Post10Min;	//GET of TPI maneuver for abort subsequent to PDI+10 minutes
	int type;				//0 = PDI Abort PAD for LM, 1 = CSM Rescue PAD
};

//APOLLO 11 T2 ABORT PAD

struct AP11T2ABORTPAD
{
	double TIG;
	double t_Phasing;
	double t_CSI1;
	double t_TPI;
};

//APOLLO 11 T3 ABORT PAD

struct AP11T3ABORTPAD
{
	double TIG;
	double t_Period;
	double t_PPlusDT;
	double t_CSI;
	double t_TPI;
};

//APOLLO 11 P76 UPDATE PAD

struct AP11P76PAD
{
	int entries = 0;
	char purpose[2][16];
	double TIG[2];
	VECTOR3 DV[2];
};

//APOLLO 11 LM ASCENT PAD

struct AP11LMASCPAD
{
	double TIG;			//Time of APS ignition for LM ascent
	double V_hor;		//horizontal velocity at orbit insertion
	double V_vert;		//Vertical velocity at orbit insertion
	double CR;			//Crossrange distance at orbital insertion
	int DEDA047;		//Sine of landing azimuth angle
	int DEDA053;		//Cosine of landing azimuth angle
	double DEDA225_226;	//Lower/Upper limit of semi-major axis at orbit insertion
	double DEDA231;		//Radial distance of launch site from center of Moon
	char remarks[128];
};

//GENERIC STRING

struct GENERICPAD
{
	char paddata[512];	// generic PAD string
};

class LEM;
class Saturn;
class SIVB;

// Mission Control Center class
class MCC : public VESSEL4 {
public:
	MCC(OBJHANDLE hVessel, int flightmodel);				// Cons
	
	char CSMName[64];
	char LEMName[64];
	char LVName[64];
	
	void Init();											// Initialization
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostCreation();
	void TimeStep(double simdt);					        // Timestep
	virtual void keyDown(DWORD key);						// Notification of keypress	
	void addMessage(char *msg);								// Add message into buffer
	void redisplayMessages();								// Cause messages in ring buffer to be redisplayed
	void pushCMCUplinkString(const char *str);              // Send sequence to CMC
	void pushAGCUplinkKey(char key, bool cm);               // Send key to AGC
	void pushUplinkData(unsigned char data);				// Add uplink data word to queue
	int CM_uplink(const unsigned char *data,int len);		// Uplink string to CM
	int CM_uplink_buffer();									// Send uplink buffer to CM
	void pushLGCUplinkString(const char *str);              // Send sequence to LM
	int LM_uplink(const unsigned char *data, int len);		// Uplink string to LM
	int LM_uplink_buffer();									// Send uplink buffer to LM
	void setState(int newState);							// Set mission state
	void setSubState(int newState);							// Set mission substate
	void drawPad();											// Draw PAD display
	void allocPad(int Number);								// Allocate memory for PAD form
	void freePad();											// Free memory occupied by PAD form
	void UpdateMacro(int type, int padtype, bool condition, int updatenumber, int nextupdate, bool altcriterium = false, bool altcondition = false, int altnextupdate = 0);
	int  subThread();										// Subthread entry point
	int startSubthread(int fcn, int type);					// Subthread start request
	void subThreadMacro(int type, int updatenumber);
	void enableMissionTracking(){ MT_Enabled = true; GT_Enabled = true; }
	void initiateAbort();
	void SlowIfDesired();
	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state
	void clbkSaveState(FILEHANDLE scn);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);

	// MISSION SPECIFIC FUNCTIONS
	void MissionSequence_B();
	void MissionSequence_C();
	void MissionSequence_C_Prime();
	void MissionSequence_D();
	void MissionSequence_F();
	void MissionSequence_G();

	class RTCC *rtcc;										// Pointer to RTCC
	Saturn *cm;												// Pointer to CM
	LEM *lm;												// Pointer to LM
	SIVB *sivb;												// Pointer to SIVB
	OBJHANDLE Earth;										// Handle for Earth
	OBJHANDLE Moon;											// Handle for the moon

	// SUBTHREAD MANAGEMENT
	int subThreadMode;										// What should the subthread do?
	int subThreadType;										// What type of subthread?
	int subThreadStatus;									// 0 = done/not busy, 1 = busy, negative = done with error

	// GROUND TRACKING NETWORK
	struct GroundStation GroundStations[MAX_GROUND_STATION]; // Ground Station Array
	double LastAOSUpdate;									// Last update to AOS data
	double CM_Position[3];                                  // CM's position and altitude
	double CM_Prev_Position[3];                             // CM's previous position and altitude
	double CM_MoonPosition[3];                              // CM's position and altitude relative to the Moon
	double CM_Prev_MoonPosition[3];                         // CM's previous position and altitude relative to the Moon
	bool   CM_DeepSpace;                                    // CM Deep Space Mode flag (Not in Earth's SOI)
	bool   GT_Enabled;										// Ground tracking enable/disable
	bool   MT_Enabled;										// Mission status tracking enable/disable

	// MISSION STATE
	int MissionType;										// Mission Type
	int MissionState;										// Major state
	int MissionPhase;										// Major mission phase
	int SubState;											// Substate number
	int EarthRev;											// Revolutions around Earth
	int MoonRev;											// Revolutions around moon
	int AbortMode;											// Abort mode
	double StateTime;										// Time in this state
	double SubStateTime;									// Time in this substate
	double MoonRevTime;										// Time in this lunar revolution

	// PAD FORMS
	int padState;											// PAD display state
	bool padAutoShow;										// PAD Auto Show flag
	int padNumber;											// PAD display number
	LPVOID padForm;											// Pointer to pad form buffer
	char upString[1024 * 3];								// Uplink string buffer
	char upDescr[1024];										// Uplink Description
	char upMessage[1024];									// Update message
	bool scrubbed;											// Maneuver scrubbed
	int upType;												// Uplink type (1 = CSM, 2 = LM)

	// UPLINK DATA
	int uplink_size;										// Size of uplink buffer
	unsigned char uplink_data[2048];						// Uplink buffer

	// CAPCOM INTERFACE
	NOTEHANDLE NHmenu;										// Menu notification handle
	NOTEHANDLE NHmessages;									// Message notification handle
	NOTEHANDLE NHpad;										// PAD display handle
	int menuState;											// Menu state
	bool PCOption_Enabled;									// Positive Completion Option Enabled
	char PCOption_Text[32];									// Positive Completion Option Text
	bool NCOption_Enabled;									// Negative Completion Option Enabled	
	char NCOption_Text[32];									// Positive Completion Option Text
	char messages[MAX_MESSAGES][MAX_MSGSIZE];				// Message buffer
	double msgtime[MAX_MESSAGES];							// Message timeout list
	int currentMessage;										// Index to tail of ring buffer
	char msgOutputBuf[MAX_MSGSIZE*MAX_MESSAGES];			// Final output string

	// FRIEND CLASSES
	friend class RTCC;										// RTCC can handle our data
};

#endif // _PA_MCC_H
