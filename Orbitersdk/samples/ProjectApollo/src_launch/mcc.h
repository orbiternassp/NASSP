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

// Mission Control Center class
class MCC {	
public:
	MCC();													// Cons
	void Init(Saturn *vs);									// Initialization
	void TimeStep(double simdt);					        // Timestep
	void keyDown(DWORD key);								// Notification of keypress	
	void addMessage(char *msg);								// Add message into buffer

	Saturn *cm;												// Pointer to CM
	// GROUND TRACKING NETWORK
	struct GroundStation GroundStations[MAX_GROUND_STATION]; // Ground Station Array
	double LastAOSUpdate;									// Last update to AOS data
	double CM_Position[3];                                  // CM's position and altitude
	double CM_Prev_Position[3];                             // CM's previous position and altitude
	bool   CM_DeepSpace;                                    // CM Deep Space Mode flag (Not in Earth's SOI)
	bool   GT_Enabled;										// Ground tracking enable/disable

	// MISSION STATE
	int MissionType;										// Mission Type
	int MissionState;										// Major state
	int EarthRev;											// Revolutions around Earth
	int MoonRev;											// Revolutions around moon
	// CAPCOM INTERFACE
	NOTEHANDLE NHmenu;										// Menu notification handle
	NOTEHANDLE NHmessages;									// Message notification handle	
	int menuState;											// Menu state
	char messages[MAX_MESSAGES][MAX_MSGSIZE];				// Message buffer
	double msgtime[MAX_MESSAGES];							// Message timeout list
	int currentMessage;										// Index to tail of ring buffer
	char msgOutputBuf[MAX_MSGSIZE*MAX_MESSAGES];			// Final output string

};

#endif // _PA_MCC_H
