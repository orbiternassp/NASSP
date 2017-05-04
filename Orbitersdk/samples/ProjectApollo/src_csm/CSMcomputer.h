/***************************************************************************
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

  **************************************************************************/

#if !defined(_PA_CSMCOMPUTER_H)
#define _PA_CSMCOMPUTER_H

#include "thread.h"

// OPTICS CONFIGURATION DEFINES
// Step values in radians.
#define OCDU_SHAFT_STEP 0.000191747598876953125 
#define OCDU_TRUNNION_STEP 0.00004793689959716796875


///
/// AGC output channel five, used to control CM and SM RCS thrusters.
///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 5.
///
typedef union {
	struct {
		unsigned SMC3:1;			///< Fire SM RCS C-3
		unsigned SMC4:1;			///< Fire SM RCS C-4
		unsigned SMA3:1;			///< Fire SM RCS A-3
		unsigned SMA4:1;			///< Fire SM RCS A-4
		unsigned SMD3:1;			///< Fire SM RCS D-3
		unsigned SMD4:1;			///< Fire SM RCS D-4
		unsigned SMB3:1;			///< Fire SM RCS B-3
		unsigned SMB4:1;			///< Fire SM RCS B-4
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value.
} CSMOut5;

///
///
/// AGC output channel five, used to control CM and SM RCS thrusters.
///
/// \ingroup AGCIO
/// \brief CSM AGC output channel 6.
///
typedef union {
	struct {
		unsigned SMB1:1;			///< Fire SM RCS B-1
		unsigned SMB2:1;			///< Fire SM RCS B-2
		unsigned SMD1:1;			///< Fire SM RCS D-1
		unsigned SMD2:1;			///< Fire SM RCS D-2
		unsigned SMA1:1;			///< Fire SM RCS A-1
		unsigned SMA2:1;			///< Fire SM RCS A-2
		unsigned SMC1:1;			///< Fire SM RCS C-1
		unsigned SMC2:1;			///< Fire SM RCS C-2
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value.
} CSMOut6;

class Saturn;
class IU;
class CSMToIUConnector;
class CSMToSIVBControlConnector;

//
// Class definition.
//

///
/// CSM guidance computer.
///
/// \ingroup AGC
/// \brief Derived class for the CSM AGC with extra CSM-specific functionality.
///
class CSMcomputer: public ApolloGuidance

{
public:

	///
	/// The AGC needs to talk to various other objects in the CSM. These are passed to the
	/// constructor so we can set references to them.
	///
	/// \brief CSM AGC constructor.
	/// \param s Sound library to use for playing sound.
	/// \param display Main control panel DSKY interface.
	/// \param display2 Lower equipment bay DSKY interface.
	/// \param im The CSM Inertial Measurement Unit.
	/// \param p The Panel SDK library.
	/// \param i The launch vehicle Instrument Unit connector for the launch vehicle autopilot.
	/// \param sivb The CSM to SIVb command connector (e.g. for fuel venting).
	///
	CSMcomputer(SoundLib &s, DSKY &display, DSKY &display2, IMU &im, PanelSDK &p, CSMToIUConnector &i, CSMToSIVBControlConnector &sivb);
	virtual ~CSMcomputer();

	bool ReadMemory(unsigned int loc, int &val);
	void WriteMemory(unsigned int loc, int val);

	void Timestep(double simt, double simdt);
	void Run() ;
	void agcTimestep(double simt, double simdt);

	//
	// External event handling.
	//

	///
	/// Called to tell the AGC that liftoff has occured, and it should switch into the liftoff
	/// program.
	///
	/// \brief Liftoff trigger.
	///
	void Liftoff(double simt);

	//
	// Data access.
	//

	///
	/// \brief Get the desired apogee for the autopilot.
	///
	double GetDesiredApogee() { return DesiredApogee; };

	///
	/// \brief Get the desired perigee for the autopilot.
	///
	double GetDesiredPerigee() { return DesiredPerigee; };

	///
	/// \brief Get the desired launch azimuth for the autopilot.
	///
	double GetDesiredAzimuth() { return DesiredAzimuth; };

	///
	/// \brief Set the desired apogee for the autopilot.
	///
	void SetDesiredApogee(double val) { DesiredApogee = val; };

	///
	/// \brief Set the desired perigee for the autopilot.
	///
	void SetDesiredPerigee(double val) { DesiredPerigee = val; };

	///
	/// \brief Set the desired launch azimuth for the autopilot.
	///
	void SetDesiredAzimuth(double val) { DesiredAzimuth = val; };

	void SetInputChannelBit(int channel, int bit, bool val);
	void SetOutputChannelBit(int channel, int bit, bool val);
	void SetOutputChannel(int channel, ChannelValue val);

	void SetMissionInfo(int MissionNo, char *OtherVessel = 0);

	void LVGuidanceSwitchToggled(PanelSwitchItem *s);

protected:

	void ProcessChannel5(ChannelValue val);
	void ProcessChannel6(ChannelValue val);
	void ProcessChannel10(ChannelValue val);
	void ProcessChannel11Bit(int bit, bool val);
	void ProcessChannel11(ChannelValue val);
	// DS20060226 TVC / Optics
	void ProcessChannel14(ChannelValue val);
	void ProcessChannel140(ChannelValue val);
	void ProcessChannel141(ChannelValue val);
	// DS20060308 FDAI NEEDLES
	void ProcessIMUCDUErrorCount(int channel, ChannelValue val);

	FILE *Dfile;
	int count;

	unsigned int LastOut5;
	unsigned int LastOut6;
	unsigned int LastOut11;

	unsigned int VesselStatusDisplay;

	///
	/// \brief Second DSKY in the lower equipment bay.
	///
	DSKY &dsky2;
	
	///
	/// \brief Connection to Saturn Instrument Unit.
	///
	CSMToIUConnector &iu;

	///
	/// \brief Connection to Saturn launch vehicle.
	///
	CSMToSIVBControlConnector &lv;

	Saturn *sat;
};

class Saturn;

// *** CM OPTICS ***
// I guess this can go here; it doesn't really warrant its own file, and it's part of GNC, so...

///
/// \ingroup AGC
/// \brief CM Optics.
///
class CMOptics {	
public: 
	CMOptics();														// Cons
	void Init(Saturn *vessel);										// Initialization
	void TimeStep(double simdt);                                    // Timestep
	void SystemTimestep(double simdt);
	void CMCShaftDrive(int val, int ch12);                          // CMC pulses
	void CMCTrunionDrive(int val, int ch12); 
	bool PaintShaftDisplay(SURFHANDLE surf, SURFHANDLE digits);		// Update panel image
	bool PaintTrunnionDisplay(SURFHANDLE surf, SURFHANDLE digits);	// Update panel image

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	//
	// These should really be protected variables.
	//

	Saturn *sat;													// Our Ship

	int Powered;                                                    // 0 = NO, 1 = MNA, 2 = MNB, 3 = Both
	int OpticsManualMovement;										// Manual Movement Demand Flags
	double OpticsShaft;												// Shaft Position
	double SextTrunion;												// SXT Trunion
	double TeleTrunion;												// SCT Trunion
	double TargetShaft;												// Reserved
	double TargetTrunion;											// Reserved
	double ShaftMoved;												// Movement counters for manual mode
	double TrunionMoved;
	double dShaft;
	double dTrunion;
	bool SextDualView;												// Toggle logical for sextant dual-view
	bool SextDVLOSTog;												// Alternating flag that controls LineOfSight cycling in Dual-View mode
	double SextDVTimer;												// Governing timer to prevent view switching at greater than 15 frames per sim second
	bool OpticsCovered;												// Are optics covers in place?
protected:
	bool PaintDisplay(SURFHANDLE surf, SURFHANDLE digits, int value);
};


//
// Strings for state saving.
//

#define CMOPTICS_START_STRING "CMOPTICS_BEGIN"
#define CMOPTICS_END_STRING   "CMOPTICS_END"


#endif // _PA_CSMCOMPUTER_H
