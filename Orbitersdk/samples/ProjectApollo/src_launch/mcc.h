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

// MISSION STATES: MISSION C
#define MST_C_INSERTION		10
// Ends at specified time, goes to
#define MST_C_SEPARATION	11
// Ends at 1ST RDZ PHASING BURN, goes to
#define MST_C_COAST1		12
// Ends at 1ST SPS BURN (NCC BURN), goes to
#define MST_C_COAST2		13
// Ends at 2ND SPS BURN (NSR BURN), goes to
#define MST_C_COAST3		14
// Ends at TERMINAL PHASE INITIATION BURN, goes to
#define MST_C_COAST4		15
// Ends at MCC, goes to
#define MST_C_COAST5		16
// Ends at TERMINAL PHASE FINALIZE BURN, goes to
#define MST_C_COAST6		17
// Ends at SEPARATION MANEUVER, goes to
#define MST_C_COAST7		18
// Ends at 3RD SPS BURN, goes to
#define MST_C_COAST8		19
// Ends at 4TH SPS BURN (MIN IMP TEST), goes to
#define MST_C_COAST9		20
// Ends at 5TH SPS BURN, goes to
#define MST_C_COAST10		21
// Ends at 6TH SPS BURN (MIN IMP TEST 2), goes to
#define MST_C_COAST11		22
// Ends at 7TH SPS BURN (TIME ANOMALY ADJ FOR DEORBIT), goes to
#define MST_C_COAST12		23
// Ends at DEORBIT BURN, goes to
#define MST_C_COAST13		24
// Ends at entry interface, goes to entry.
#define MST_C_COAST14		25
#define MST_C_COAST15		26
#define MST_C_COAST16		27
#define MST_C_COAST17		28
#define MST_C_COAST18		29
#define MST_C_COAST19		30
#define MST_C_COAST20		31
#define MST_C_COAST21		32
#define MST_C_COAST22		33
#define MST_C_COAST23		34
#define MST_C_COAST24		35
#define MST_C_COAST25		36
#define MST_C_COAST26		37
#define MST_C_COAST27		38
#define MST_C_COAST28		39
#define MST_C_COAST29		40
#define MST_C_COAST30		41
#define MST_C_COAST31		42
#define MST_C_COAST32		43
#define MST_C_COAST33		44
#define MST_C_COAST34		45
#define MST_C_COAST35		46
#define MST_C_COAST36		47
#define MST_C_COAST37		48
#define MST_C_COAST38		49
#define MST_C_COAST39		50
#define MST_C_COAST40		51
#define MST_C_COAST41		52
#define MST_C_COAST42		53
#define MST_C_COAST43		54
#define MST_C_COAST44		55
#define MST_C_COAST45		56
#define MST_C_COAST46		57
#define MST_C_COAST47		58
#define MST_C_COAST48		59
#define MST_C_COAST49		60
#define MST_C_COAST50		61
#define MST_C_COAST51		62
#define MST_C_COAST52		63
#define MST_C_COAST53		64
#define MST_C_COAST54		65
#define MST_C_COAST55		66
#define MST_C_COAST56		67
#define MST_C_COAST57		68
#define MST_C_COAST58		69
#define MST_C_COAST59		70
#define MST_C_COAST60		71
#define MST_C_ABORT			75
// Entered on abort from orbit, works like COAST13, goes to MST_ORBIT_ENTRY

//UPDATE TYPES
#define UTP_BLOCKDATA		0
#define UTP_P47MANEUVER		1
#define UTP_P30MANEUVER		2
#define UTP_TPI				3
#define UTP_ENTRY			4
#define UTP_UPLINKONLY		5
#define UTP_SVNAVCHECK		6
#define UTP_P27PAD			7
#define UTP_TLIPAD			8
#define UTP_LUNARENTRY		9
#define UTP_FINALLUNARENTRY 10
#define UTP_STARCHKPAD		11
#define UTP_NONE			99

// MISSION STATES: MISSION C PRIME
#define MST_CP_INSERTION	10
// Ends at TLI Update
#define MST_CP_EPO1			11
// Ends at TLI BURN
#define MST_CP_TRANSLUNAR1	21
// Ends at separation
#define MST_CP_TRANSLUNAR2	22
//Ends at Block Data 1 Update
#define MST_CP_TRANSLUNAR3	23
//Ends at MCC1 Update
#define MST_CP_TRANSLUNAR4	24
//Ends at Block Data 2 Update
#define MST_CP_TRANSLUNAR5	25
//Ends at Block Data 3 Update
#define MST_CP_TRANSLUNAR6	26
//Ends at MCC2 Update
#define MST_CP_TRANSLUNAR7	27
//Ends at Block Data 4 Update
#define MST_CP_TRANSLUNAR8	28
//Ends at Block Data 5 Update
#define MST_CP_TRANSLUNAR9	29
//Ends at MCC3 Update
#define MST_CP_TRANSLUNAR10	30
//Ends at Block Data 6 Update
#define MST_CP_TRANSLUNAR11	31
//Ends at MCC4 Update
#define MST_CP_TRANSLUNAR12	32
//Ends at Preliminary LOI-1 Update
#define MST_CP_TRANSLUNAR13	33
//Ends at Block Data 7 Update
#define MST_CP_TRANSLUNAR14	34
//Ends at Map Update
#define MST_CP_TRANSLUNAR15	35
//Ends at LOI-1 Update
#define MST_CP_TRANSLUNAR16	36
//Ends at LOI-2 Update

#define MST_CP_LUNAR_ORBIT1	101
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT2	102
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT3	103
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT4	104
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT5	105
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT6	106
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT7	107
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT8	108
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT9	109
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT10 110
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT11 111
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT12 112
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT13 113
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT14 114
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT15 115
#define MST_CP_LUNAR_ORBIT16 116
#define MST_CP_LUNAR_ORBIT17 117
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT18 118
// Ends at TEI Calculation
#define MST_CP_LUNAR_ORBIT19 119
// Ends at TEI BURN

#define MST_CP_TRANSEARTH1	201
//Ends at Entry REFSMMAT update
#define MST_CP_TRANSEARTH2	202
//Ends at MCC5 Update
#define MST_CP_TRANSEARTH3	203
//Ends at MCC6 Update
#define MST_CP_TRANSEARTH4	204
//Ends at Preliminary MCC7 Update
#define MST_CP_TRANSEARTH5	205
//Ends at Preliminary Entry Update
#define MST_CP_TRANSEARTH6	206
//Ends at MCC7 Update
#define MST_CP_TRANSEARTH7	207
//Ends at Entry Update
#define MST_CP_TRANSEARTH8	208
//Ends at Final Entry Update

// Ends at entry interface, goes to entry.
#define MST_CP_ABORT_ORBIT	300
// Abort from earth orbit, goes to MST_ORBIT_ENTRY
#define MST_CP_ABORT		301
// post-TLI abort, ends at abort burn (if any)
// goes to MST_CP_TRANSEARTH with an abort flag.

// MISSION STATES: MISSION D

#define MST_D_INSERTION		10
#define MST_D_DAY1STATE1	11
#define MST_D_DAY1STATE2	12
#define MST_D_DAY1STATE3	13
#define MST_D_DAY1STATE4	14
#define MST_D_DAY2STATE1	20
#define MST_D_DAY2STATE2	21
#define MST_D_DAY2STATE3	22
#define MST_D_DAY2STATE4	23
#define MST_D_DAY2STATE5	24
#define MST_D_DAY2STATE6	25
#define MST_D_DAY3STATE1	30
#define MST_D_DAY3STATE2	31

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

class LEM;


// Mission Control Center class
class MCC : public VESSEL4 {
public:
	MCC(OBJHANDLE hVessel, int flightmodel);				// Cons
	
	char CSMName[64];
	char LEMName[64];
	char LVName[64];
	
	void Init();											// Initialization
	void clbkPreStep(double simt, double simdt, double mjd);
	void TimeStep(double simdt);					        // Timestep
	virtual void keyDown(DWORD key);						// Notification of keypress	
	void addMessage(char *msg);								// Add message into buffer
	void redisplayMessages();								// Cause messages in ring buffer to be redisplayed
	void pushCMCUplinkString(const char *str);              // Send sequence to CMC
	void pushCMCUplinkKey(char key);                        // Send key to CMC
	void pushUplinkData(unsigned char data);				// Add uplink data word to queue
	int CM_uplink(const unsigned char *data,int len);		// Uplink string to CM
	int CM_uplink_buffer();									// Send uplink buffer to CM
	void setState(int newState);							// Set mission state
	void setSubState(int newState);							// Set mission substate
	void drawPad();											// Draw PAD display
	void allocPad(int Number);								// Allocate memory for PAD form
	void freePad();											// Free memory occupied by PAD form
	void UpdateMacro(int type, bool condition, int updatenumber, int nextupdate, bool altcriterium = false, bool altcondition = false, int altnextupdate = 0);
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
	class RTCC *rtcc;										// Pointer to RTCC
	Saturn *cm;												// Pointer to CM
	LEM *lm;												// Pointer to LM
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

	// PAD FORMS
	int padState;											// PAD display state
	bool padAutoShow;										// PAD Auto Show flag
	int padNumber;											// PAD display number
	LPVOID padForm;											// Pointer to pad form buffer
	char upString[1024 * 3];								// Uplink string buffer
	char upDescr[1024];										// Uplink Description
	bool scrubbed;											// Maneuver scrubbed

	// UPLINK DATA
	int uplink_size;										// Size of uplink buffer
	unsigned char uplink_data[1024];						// Uplink buffer

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
