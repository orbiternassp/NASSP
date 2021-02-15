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

#include "MCCPADForms.h"

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
#define PT_AP11LUNSURFPAD	25
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

class LEM;
class Saturn;
class SIVB;
class RTCC;

// Mission Control Center class
class MCC {
public:
	MCC(RTCC *rtc);											// Cons

	char CSMName[64];
	char LEMName[64];
	char LVName[64];
	
	void Init();											// Initialization
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
	void SetCSM(char *csmname);
	void SetLM(char *lemname);
	void SetLV(char *lvname);
	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state

	// MISSION SPECIFIC FUNCTIONS
	void MissionSequence_B();
	void MissionSequence_C();
	void MissionSequence_C_Prime();
	void MissionSequence_D();
	void MissionSequence_F();
	void MissionSequence_G();

	RTCC *rtcc;												// Pointer to RTCC
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
