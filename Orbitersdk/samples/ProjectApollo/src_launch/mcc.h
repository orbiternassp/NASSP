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

// Ground Station Ground Communications Capabilities Bits
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
#define GSCC_VHF	0x01
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
#define MAX_GROUND_STATION 42

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
	char GroundCommCaps; // Ground Communications Capabilities
	bool HasRadar;       // Has radar capability
	bool HasAcqAid;      // Has target acquisition aid
	char DownTlmCaps;    // Downtelemetry Capabilities
	char UpTlmCaps;      // Command Capabilities
	char StationType;    // Station Type
	int  StationPurpose; // Station Purpose
};

class MCC {
	// Mission Control Center main class
public:
	MCC();															// Cons
	virtual void Init();									// Initialization
	virtual void TimeStep(double simdt);                                    // Timestep

	Saturn *cm;														// Pointer to CM
};

class MC_GroundTrack : public MCC {
	// Derived GroundTrack class from Mission Control Center main class
public:

	MC_GroundTrack();												// Cons
	void InitGroundStations();
	virtual void Init(Saturn *vessel);								// Initialization
	virtual void TimeStep(double simdt);							// Timestep

	struct GroundStation GroundStations[MAX_GROUND_STATION];        // Ground Station Array
	double LastAOSUpdate;											// Last update to AOS data
	double CM_Position[3];                                          // CM's position and altitude
	bool   CM_DeepSpace;                                            // CM Deep Space Mode flag	
	double ClosestRange;											// CM range to closest ground station
	int    ClosestStation;											// CM closest station number

};
class MC_CapCom : public MCC {
	// Derived CapCom class from Mission Control Center main class
public:

	MC_CapCom();
	~MC_CapCom();
	virtual void Init();
	virtual void TimeStep(double simdt);
	
    int Talk(char *ID, ...);


	FILE *trnscrpt_h; // Transcript file pointer
	FILE *capcomdb_h; // CapCom Phrase Database file pointer
	char language[100];
	char trnscrpt_fname[100];
	char capcomdb_fname[100];
};
#endif // _PA_MCC_H
