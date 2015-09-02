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

// Mission major state numbers
#define MST_PRELAUNCH		0
#define MST_BOOST			1
#define MST_EARTH_ORBIT		2
#define MST_TL_COAST		3
#define MST_LUNAR_ORBIT		4
#define MST_TE_COAST		5
#define MST_ENTRY			6

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
#define MST_ABORT_PL		90
// Launch abort, return to Earth
#define MST_LAUNCH_ABORT	91
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
#define MST_C_ABORT			25
// Entered on abort from orbit, works like COAST13, goes to MST_ORBIT_ENTRY

// MISSION STATES: MISSION C PRIME
#define MST_CP_INSERTION	10
// Ends at TLI BURN
#define MST_CP_TRANSLUNAR1	11
// Ends at separation
#define MST_CP_TRANSLUNAR2	12
// Includes MCC burns as necessary.
// Ends at LOI BURN
#define MST_CP_LUNAR_ORBIT	13
// Ends at TEI BURN
#define MST_CP_TRANSEARTH	14
// Ends at entry interface, goes to entry.
#define MST_CP_ABORT_ORBIT	20
// Abort from earth orbit, goes to MST_ORBIT_ENTRY
#define MST_CP_ABORT		21
// post-TLI abort, ends at abort burn (if any)
// goes to MST_CP_TRANSEARTH with an abort flag.

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
	char Area[4][10];	// XXX.YY where XXX is rev and YY is recovery area/supt caps
	double Lat[4];		// Target point
	double Lng[4];		// Target point
	double GETI[4];		// TIG
	double dVC[4];		// dV for EMS
	char Wx[4][10];		// Weather cndx ("GOOD-FAIR-POOR")
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

// Mission Control Center class
class MCC {	
public:
	MCC();													// Cons
	void Init(Saturn *vs);									// Initialization
	void TimeStep(double simdt);					        // Timestep
	void keyDown(DWORD key);								// Notification of keypress	
	void addMessage(char *msg);								// Add message into buffer
	void redisplayMessages();								// Cause messages in ring buffer to be redisplayed
	void setState(int newState);							// Set mission state
	void setSubState(int newState);							// Set mission substate
	void drawPad();											// Draw PAD display
	void allocPad(int Number);								// Allocate memory for PAD form
	void freePad();											// Free memory occupied by PAD form
	int  subThread();										// Subthread entry point
	int startSubthread(int fcn);							// Subthread start request
	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state

	class RTCC *rtcc;										// Pointer to RTCC
	Saturn *cm;												// Pointer to CM
	Saturn *lm;												// Pointer to LM
	OBJHANDLE Earth;										// Handle for Earth
	OBJHANDLE Moon;											// Handle for the moon

	// SUBTHREAD MANAGEMENT
	int subThreadMode;										// What should the subthread do?
	int subThreadStatus;									// 0 = done/not busy, 1 = busy, negative = done with error

	// GROUND TRACKING NETWORK
	struct GroundStation GroundStations[MAX_GROUND_STATION]; // Ground Station Array
	double LastAOSUpdate;									// Last update to AOS data
	double CM_Position[3];                                  // CM's position and altitude
	double CM_Prev_Position[3];                             // CM's previous position and altitude
	bool   CM_DeepSpace;                                    // CM Deep Space Mode flag (Not in Earth's SOI)
	bool   GT_Enabled;										// Ground tracking enable/disable
	bool   MT_Enabled;										// Mission status tracking enable/disable

	// MISSION STATE
	int MissionType;										// Mission Type
	int MissionState;										// Major state
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

	// CAPCOM INTERFACE
	NOTEHANDLE NHmenu;										// Menu notification handle
	NOTEHANDLE NHmessages;									// Message notification handle
	NOTEHANDLE NHpad;										// PAD display handle
	int menuState;											// Menu state
	char messages[MAX_MESSAGES][MAX_MSGSIZE];				// Message buffer
	double msgtime[MAX_MESSAGES];							// Message timeout list
	int currentMessage;										// Index to tail of ring buffer
	char msgOutputBuf[MAX_MSGSIZE*MAX_MESSAGES];			// Final output string

	// FRIEND CLASSES
	friend class RTCC;										// RTCC can handle our data
};

#endif // _PA_MCC_H
