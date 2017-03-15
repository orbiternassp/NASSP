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

//
// Flagword definitions. These are really intended for future
// expansion as the AGC simulation gets closer to the real thing.
// For the moment only a few of these flags will actually be
// used.
//

///
/// \ingroup AGCFlags
///
typedef union {
	struct {
		unsigned P29FLAG:1;			///< First pass through Program 29.
		unsigned AMOONFLG:1;		///< State vector in lunar sphere.
		unsigned FREEFLAG:1;		///< Temporary flag for use in any routine.
		unsigned CYC61FLG:1;		///< KALCMANU calculates maneuver time.
		unsigned F2RTE:1;			///< In time-critical mode.
		unsigned R53FLAG:1;			///< V51 initiated.
		unsigned RNDVZFLG:1;		///< P20 Rendevouz option running.
		unsigned IMUSE:1;			///< IMU in use.
		unsigned NEEDLFLG:1;		///< Total attitude error display.
		unsigned ZMEASURE:1;		///< Measurement planet and primary planet different.
		unsigned NORFHOR:1;			///< Far horizon.
		unsigned MOONFLAG:1;		///< Moon is sphere of influence.
		unsigned MIDFLAG:1;			///< Integration with solar perturbations.
		unsigned JSWITCH:1;			///< Integration of W matrix.
		unsigned UNUSED:1;			///< Unused.
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value for EMEM access.
} CSMFlagWord0;

///
/// \ingroup AGCFlags
///
typedef union {
	struct {
		unsigned AVEGFLAG:1;		///< Average G to continue.
		unsigned GUESSW:1;			///< No starting value for iteration exists.
		unsigned SLOPESW:1;			///< Iterate with bias method in iterator.
		unsigned MARKFLG:1;			///< Mark accepted.
		unsigned TRACKFLG:1;		///< Tracking allowed.
		unsigned IDLEFAIL:1;		///< Inhibit R40
		unsigned UPDATFLG:1;		///< Updating by marks allowed.
		unsigned VEHUPFLG:1;		///< CSM state vector being updated.
		unsigned TARG2FLG:1;		///< Sighting landmark.
		unsigned TARG1FLG:1;		///< Sighting LEM.
		unsigned NODOP01:1;			///< Program 01 cannot be run at this time.
		unsigned ERADFLAG:1;		///< Earth: compute ellipsoid radius, Moon: use fixed radius
		unsigned STIKFLAG:1;		///< RHC control.
		unsigned NJETSFLG:1;		///< Two-jet RCS burn.
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value for EMEM access.
} CSMFlagWord1;

///
/// \ingroup AGCFlags
///
typedef union {
	struct {
		unsigned NODOFLAG:1;		///< Verb 37 (run program) not permitted.
		unsigned CALCMAN2:1;		///< Perform maneuver starting procedure.
		unsigned P24MKFLG:1;		///< New mark from SXT mark.
		unsigned PFRATFLG:1;		///< Preferred attitude computed.
		unsigned AVFLAG:1;			///< LM is active vehicle.
		unsigned FINALFLG:1;		///< Last pass through rendevouz program computations.
		unsigned FIRSTFLG:1;		///< First pass through S40.9.
		unsigned ETPIFAG:1;			///< Elevation angle supplied for P34/74.
		unsigned XDELVFLG:1;		///< External Delta V VG computation.
		unsigned IMPULSW:1;			///< Minimum impulse burn (cutoff time specified).
		unsigned SKIPVHF:1;			///< Disregard radar read because of software or hardware RESTART.
		unsigned STEERSW:1;			///< Steering to be done (P40), or calculate time to SIVB ignition (P15).
		unsigned P21FLAG:1;			///< Succeeding pass through P21, use base vector for calculation.
		unsigned F22DPSFLG:1;		///< Display DR, DV
		unsigned R21MARK:1;			///< Rendevouz optics marking active.
		unsigned DRIFTFLG:1;		///< T3RUPT calls gyro compensation.
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value for EMEM access.
} CSMFlagWord2;

///
/// \ingroup AGCFlags
///
typedef union {
	struct {
		unsigned DIM0FLAG:1;		///< W matrix is to be used.
		unsigned D6OR9FLG:1;		///< Dimension of W is 9 for integration.
		unsigned VINTFLAG:1;		///< CSM state vector being integrated.
		unsigned INTYPFLG:1;		///< Conic integration.
		unsigned STATEFLG:1;		///< Permanent state vector being updated.
		unsigned ORBWFLG:1;			///< W matrix valid for orbital navigation.
		unsigned CULTFLAG:1;		///< Star occulted.
		unsigned PRECIFLG:1;		///< CSMPREC, LEMPREC or INTEGRVS called.
		unsigned P00FLAG:1;			///< Inhibit backwards integration.
		unsigned VFLAG:1;			///< Less than two stars in the field of view.
		unsigned P22MKFLG:1;		///< P22/P24 downlinked mark data was just taken.
		unsigned LUNAFLG:1;			///< Lunar latitude/longitude.
		unsigned REFSMFLG:1;		///< REFSMMAT good.
		unsigned GLOKFAIL:1;		///< Gimbal lock has occured.
		unsigned V50N18FL:1;		///< Enable R60 attitude maneuver.
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value for EMEM access.
} CSMFlagWord3;

///
/// \ingroup AGCFlags
///
typedef union {
	struct {
		unsigned XDSPFLAG:1;		///< Mark display not to be interrupted.
		unsigned VNFLAG:1;			///< VN flash display.
		unsigned MKOVFLAG:1;		///< Mark display over normal display.
		unsigned NRUPTFLG:1;		///< Normal display interrupted by priority or mark display.
		unsigned MRUPTFLG:1;		///< Mark display interruted by priority display.
		unsigned PINBRFLG:1;		///< Astronaut has interfered with normal display.
		unsigned PRONVFLG:1;		///< Astronaut using keyboard when priority display initiated.
		unsigned NRMNVFLG:1;		///< Astronaut using keyboard when normal display initiated.
		unsigned MRKNVFLG:1;		///< Astronaut using keyboard when mark display initiated.
		unsigned NWAITFLG:1;		///< Higher priority display operating when normal display iniated.
		unsigned MWAITFLG:1;		///< Higher priority display operating when mark dispaly initiated.
		unsigned PDSPFLAG:1;		///< Cannot interrupt priority display.
		unsigned NRMIDFLG:1;		///< Normal display in ENDIDLE.
		unsigned PRIODFLG:1;		///< Priority display in ENDIDLE.
		unsigned MRKIDFLG:1;		///< Mark display in ENDIDLE.
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value for EMEM access.
} CSMFlagWord4;

///
/// \ingroup AGCFlags
///
typedef union {
	struct {
		unsigned RENDWFLG:1;		///< W matrix valid for rendezvous navigation.
		unsigned UNUSED1:1;			///< Unused on real AGC.
		unsigned SOLNSW:1;			///< Lambert does not converge, or time-radius nearly circular.
		unsigned UNUSED2:1;			///< Unused on real AGC.
		unsigned GRRBKFLG:1;		///< Backup GRR received.
		unsigned F3AXISFLG:1;		///< Maneuver specified by three axes.
		unsigned ENGONBIT:1;		///< Engine turned on.
		unsigned COMPUTER:1;		///< Not used in program.
		unsigned DMENFLG:1;			///< Dimension of W is 9 for incorporation.
		unsigned NEWTFLAG:1;		///< New longitude iteration, skip display.
		unsigned INCORFLG:1;		///< First incorporation.
		unsigned V59FLAG:1;			///< Calibrating for P23.
		unsigned SLOWFLG:1;			///< P37 transearth coast slowdown is desired.
		unsigned RETROFLG:1;		///< P37 premaneuver orbit is desired.
		unsigned DSKYFLAG:1;		///< Displays sent to DSKY.
	} u;
	unsigned int word;				///< Word holds the flags from the bitfield in one 32-bit value for EMEM access.
} CSMFlagWord5;

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
	/// \brief Get the desired launch inclination for the autopilot.
	///
	double GetDesiredInclination() { return DesiredInclination; };

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

	///
	/// \brief Set the desired inclination for the autopilot.
	///
	void SetDesiredInclination(double val) { DesiredInclination = val; };

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
//constants
	const double ACONST = 0.159;
	const double XS = 0.035;
	const double KCONTROL = 0.25;
	const double RMAX = 0.35;

	const double MUE = 3.984e14;
	const double C1 = 1.25;
	const double C16 = 0.032;
	const double C17 = 0.0032;
	const double C18 = 152.4;
	const double C20 = 53.34;
	const double CHOOK = 0.25;
	const double CH1 = 1.0;
	const double GMAX = 78.48;
	const double KA1 = 1.3;
	const double KA2 = 1.829;
	const double KA3 = 27.43;
	const double KA4 = 12.19;
	const double KB1 = 2.5;
	const double KB2 = 0.0025;
	const double KDMIN = 0.1524;
	const double KTETA = 1000.0;
	const double K44 = 6019663.0;
//	const double LATBIAS = 0.76317978;
	const double LATBIAS = 0.0001198;
	const double POINT1 = 0.1;
	const double Q3 = 0.415;
	const double Q5 = 13043.0;
	const double Q6 = 0.0349;
	const double Q7MIN = 12.19;
	const double Q19 = 0.5;
	const double VFINAL1 = 8229.6;
	const double VFINAL = 8107.7;
	const double VLMIN = 5486.4;
	const double VSAT = 7853.54;
	const double VMIN = VSAT/2.0;
	const double VRCONTRL = 213.36;
	const double VCORLIM = 304.8;
	const double VQUIT = 304.8;
	const double C21 = 42.67;
	const double ATK = 6359.83;
	const double GS = 9.81;
	const double HS = 8686.8;
	const double RE = 6371000.0;
	const double LOD = 0.18;
	const double LAD = 0.3;
//	const double Q2 = 1.85 * (-1152 + 500 * LAD);
	const double Q2 = 1.85 * (-1992 + 3500 * LAD);
	const double Q7F = 1.829;
	const double LEWD1 = 0.15;
	const double DLEWDzero = -0.05;
	const double KLAT1 = 1.0/24.0;
	const double KLAT = KLAT1 * LAD;
	const double COS15 = 0.965;
	const double LDCMINR = COS15 * LAD;
	const double RTOGO[13] = {6.845,19.24,43.66,85.655,139.49,184.815,
		316.165,389.055,493.58,636.955,933.88,1189.55,1469.455};
	const double VREF[13] = {302.9712,640.9944,1195.4256,1918.716,2600.2488,3078.7848,
		4271.4672,4861.8648,5595.2136,6348.6792,7037.832,7162.8,10668};
	const double RDOTREF[13] = {-210.58632,-219.1512,-211.5312,-185.6232,-150.2664,
		-126.7968,-107.2896,-126.7968,-172.5168,-238.0488,-282.5496,-249.936,-249.936};
	const double DREFR[13] = {12.54252,18.288,24.8412,28.62072,30.0228,31.18104,36.17976,
		38.16096,36.69792,29.07792,8.56488,1.95072,1.95072};
	const double F2[13] = {0.01521749,0.02174274,0.04272673,0.0877722,0.1504753,0.2058337,
		0.3726373,0.4663581,0.6059074,0.810345,1.320225,1.848922,1.848922};
	const double F4[13] = {0.01521749,0.02174274,0.04272673,0.0877722,0.1504753,0.2058337,
		0.7,2.0,5.0,2.5,1.320225,1.0,1.0};
//	const double F1[13] = {-0.210022,-0.3369457,-0.5483638,-0.85587,-1.200646,-1.439804,
//		-2.006135,-4.553,-6.07,-6.07,-12.26747,-20.35878,-20.35878};
	const double F1[13] = {-0.210022,-0.3369457,-0.5483638,-0.85587,-1.200646,-1.439804,
		-2.006135,-4.553,-12.07,-12.07,-12.26747,-20.35878,-20.35878};
	const double F3[13] = {22.57,40.367,80.068,178.895,346.764,522.07,
		609.39,861.175,1262.995,1813.925,2562.25,2789.8,2789.8};

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
