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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.14  2006/03/12 01:13:28  dseagrav
  *	Added lots of SCS items and FDAI stuff.
  *	
  *	Revision 1.13  2006/02/27 00:57:48  dseagrav
  *	Added SPS thrust-vector control. Changes 20060225-20060226.
  *	
  *	Revision 1.12  2006/02/21 11:54:40  tschachim
  *	Moved TLI sequence to the IU.
  *	
  *	Revision 1.11  2006/01/14 20:58:15  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.10  2006/01/11 22:34:20  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.9  2006/01/11 19:57:55  movieman523
  *	Load appropriate AGC binary file based on mission number.
  *	
  *	Revision 1.8  2005/08/22 19:47:33  movieman523
  *	Fixed long timestep on startup, and added new Virtual AGC with EDRUPT fix.
  *	
  *	Revision 1.7  2005/08/19 13:51:39  tschachim
  *	Added lastOrbitalElementsTime for earth orbit insertation monitoring.
  *	
  *	Revision 1.6  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.5  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.4  2005/08/08 21:10:30  movieman523
  *	Fixed broken TLI program. LastAlt wasn't being set and that screwed up the burn end calculations.
  *	
  *	Revision 1.3  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.2  2005/04/30 23:09:15  movieman523
  *	Revised CSM banksums and apogee/perigee display to match the real AGC.
  *	
  *	Revision 1.1  2005/02/11 12:17:54  tschachim
  *	Initial version
  *	
  **************************************************************************/

//
// Flagword definitions. These are really intended for future
// expansion as the AGC simulation gets closer to the real thing.
// For the moment only a few of these flags will actually be
// used.
//

typedef union {
	struct {
		unsigned P29FLAG:1;
		unsigned AMOONFLG:1;
		unsigned FREEFLAG:1;
		unsigned CYC61FLG:1;
		unsigned F2RTE:1;
		unsigned R53FLAG:1;
		unsigned RNDVZFLG:1;
		unsigned IMUSE:1;
		unsigned NEEDLFLG:1;
		unsigned ZMEASURE:1;
		unsigned NORFHOR:1;
		unsigned MOONFLAG:1;
		unsigned MIDFLAG:1;
		unsigned JSWITCH:1;
		unsigned UNUSED:1;
	} u;
	unsigned int word;
} CSMFlagWord0;


typedef union {
	struct {
		unsigned AVEGFLAG:1;
		unsigned GUESSW:1;
		unsigned SLOPESW:1;
		unsigned MARKFLG:1;
		unsigned TRACKFLG:1;
		unsigned IDLEFAIL:1;
		unsigned UPDATFLG:1;
		unsigned VEHUPFLG:1;
		unsigned TARG2FLG:1;
		unsigned TARG1FLG:1;
		unsigned NODOP01:1;
		unsigned ERADFLAG:1;
		unsigned STIKFLAG:1;
		unsigned NJETSFLG:1;
	} u;
	unsigned int word;
} CSMFlagWord1;


typedef union {
	struct {
		unsigned NODOFLAG:1;
		unsigned CALCMAN2:1;
		unsigned P24MKFLG:1;
		unsigned PFRATFLG:1;
		unsigned AVFLAG:1;
		unsigned FINALFLG:1;
		unsigned FIRSTFLG:1;
		unsigned XDELVFLG:1;
		unsigned IMPULSW:1;
		unsigned SKIPVHF:1;
		unsigned STEERSW:1;
		unsigned P21FLAG:1;
		unsigned F22DPSFLG:1;
		unsigned R21MARK:1;
		unsigned DRIFTFLG:1;
	} u;
	unsigned int word;
} CSMFlagWord2;

typedef union {
	struct {
		unsigned DIM0FLAG:1;
		unsigned D6OR9FLG:1;
		unsigned VINTFLAG:1;
		unsigned INTYPFLG:1;
		unsigned STATEFLG:1;
		unsigned ORBWFLG:1;
		unsigned CULTFLAG:1;
		unsigned PRECIFLG:1;
		unsigned P00FLAG:1;
		unsigned VFLAG:1;
		unsigned P22MKFLG:1;
		unsigned LUNAFLG:1;
		unsigned REFSMFLG:1;
		unsigned GLOKFAIL:1;
		unsigned V50N18FL:1;
	} u;
	unsigned int word;
} CSMFlagWord3;


typedef union {
	struct {
		unsigned XDSPFLAG:1;
		unsigned VNFLAG:1;
		unsigned MKOVFLAG:1;
		unsigned NRUPTFLG:1;
		unsigned MRUPTFLG:1;
		unsigned PINBRFLG:1;
		unsigned PRONVFLG:1;
		unsigned NRMNVFLG:1;
		unsigned MRKNVFLG:1;
		unsigned NWAITFLG:1;
		unsigned MWAITFLG:1;
		unsigned PDSPFLAG:1;
		unsigned NRMIDFLG:1;
		unsigned PRIODFLG:1;
		unsigned MRKIDFLG:1;
	} u;
	unsigned int word;
} CSMFlagWord4;

typedef union {
	struct {
		unsigned RENDWFLG:1;
		unsigned UNUSED1:1;
		unsigned SOLNSW:1;
		unsigned UNUSED2:1;
		unsigned GRRBKFLG:1;
		unsigned F3AXISFLG:1;
		unsigned ENGONBIT:1;
		unsigned COMPUTER:1;
		unsigned DMENFLG:1;
		unsigned NEWTFLAG:1;
		unsigned INCORFLG:1;
		unsigned V59FLAG:1;
		unsigned SLOWFLG:1;
		unsigned RETROFLG:1;
		unsigned DSKYFLAG:1;
	} u;
	unsigned int word;
} CSMFlagWord5;

typedef union {
	struct {
		unsigned SMC3:1;
		unsigned SMC4:1;
		unsigned SMA3:1;
		unsigned SMA4:1;
		unsigned SMD3:1;
		unsigned SMD4:1;
		unsigned SMB3:1;
		unsigned SMB4:1;
	} u;
	unsigned int word;
} CSMOut5;

typedef union {
	struct {
		unsigned SMB1:1;
		unsigned SMB2:1;
		unsigned SMD1:1;
		unsigned SMD2:1;
		unsigned SMA1:1;
		unsigned SMA2:1;
		unsigned SMC1:1;
		unsigned SMC2:1;
	} u;
	unsigned int word;
} CSMOut6;

class IU;

//
// Class definition.
//

///
/// \ingroup AGC
/// CSM AGC
/// \brief Derived class for the CSM AGC with extra CSM-specific functionality.
///
class CSMcomputer: public ApolloGuidance

{
public:

	///
	/// The AGC needs to talk to various other objects in the CSM. These are passed to the
	/// constructor so we can set references to them.
	/// \brief CSM AGC constructor.
	/// \param s Sound library to use for playing sound.
	/// \param display Main control panel DSKY interface.
	/// \param display2 Lower equipment bay DSKY interface.
	/// \param im The CSM Inertial Measurement Unit.
	/// \param p The Panel SDK library.
	/// \param i The launch vehicle Instrument Unit for the launch vehicle autopilot.
	///
	CSMcomputer(SoundLib &s, DSKY &display, DSKY &display2, IMU &im, PanelSDK &p, IU &i);
	virtual ~CSMcomputer();

	bool ValidateVerbNoun(int verb, int noun);
	void ProcessVerbNoun(int verb, int noun);
	bool ValidateProgram(int prog);
	void ProceedNoData();
	void TerminateProgram();
	unsigned int GetFlagWord(int num);
	void SetFlagWord(int num, unsigned int val);
	bool ReadMemory(unsigned int loc, int &val);
	void WriteMemory(unsigned int loc, int val);

	void Timestep(double simt, double simdt);

	//
	// External event handling.
	//

	///
	/// Called to tell the AGC that liftoff has occured, and it should switch into the liftoff
	/// program.
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
	void SetBurnTime(double val) { BurnTime = val; };

	void SetInputChannelBit(int channel, int bit, bool val);
	void SetOutputChannelBit(int channel, int bit, bool val);
	void SetOutputChannel(int channel, unsigned int val);

	///
	/// Pass information about the mission to the AGC, which needs to know which vessels
	/// it's working with, the mission it's flying, and the realism level.
	///
	/// Amongst other things, we need to know the mission number so that we know which version
	/// of the Colossus software to load into the Virtual AGC.
	///
	/// \brief Set mission info in AGC.
	/// \param MissionNo Apollo mission number.
	/// \param RealismValue Current realism level.
	/// \param OtherVessel Pointer to the LEM so that the CSM can track it for rendevouz.
	///
	void SetMissionInfo(int MissionNo, int RealismValue, char *OtherVessel = 0);

protected:

	void DisplayNounData(int noun);
	void ProgPressed(int R1, int R2, int R3);

	///
	/// Approximate method to calculate 'Time of Free Fall': the time in seconds that the CM would
	/// take to free fall to 300,000 feet if the engines were to fail at this time.
	/// \brief Calculate TFF for DSKY display.
	/// \param vy Vertical velocity relative to Earth in m/s.
	/// \param r0 Current altitude in meters.
	/// \param g Current gravitational acceleration in m/s^2
	///
	double CalcTFF(double vy, double r0, double g);

	///
	/// Calculate the current gravitational acceleration.
	/// \brief Get current g.
	/// \return Current gravitational accelerations in m/s^2.
	///
	double CurrentG();

	///
	/// The real CSM could only perform orbit calculations while running certain specific programs. We
	/// simulate this through this call: the calculations will only be performed if it returns true.
	/// \brief Can the AGC perform orbit calculations at this time?
	/// \return True if the current program supports orbit calculation.
	///
	bool OrbitCalculationsValid();
	void DisplayBankSum();

	void ProcessChannel5(int val);
	void ProcessChannel6(int val);
	void ProcessChannel10(int val);
	void ProcessChannel11Bit(int bit, bool val);
	void ProcessChannel11(int val);
	// DS20060226 TVC / Optics
	void ProcessChannel160(int val);
	void ProcessChannel161(int val);
	void CheckEngineOnOff(int val);
	// DS20060308 FDAI NEEDLES
	void ProcessIMUCDUErrorCount(int channel, unsigned int val);

	//
	// Programs we can run.
	//

	void Prog01(double simt);
	void Prog02(double simt);
	void Prog06(double simt);
	void Prog11(double simt);

	///
	/// Program 15 is the TLI (Trans Lunar Injection) program used to send the SIVb and CSM to the moon.
	/// \brief Run program 15.
	/// \param simt Current Mission Time.
	///
	void Prog15(double simt);

	//
	// Program support
	//

	void DoTLICalcs(double simt);
	void UpdateTLICalcs(double simt);

	//
	// Program input processing.
	//

	void Prog02Pressed(int R1, int R2, int R3);
	void Prog15Pressed(int R1, int R2, int R3);

	//
	// Program data.
	//
	double lastOrbitalElementsTime;

	CSMFlagWord0 FlagWord0;
	CSMFlagWord1 FlagWord1;
	CSMFlagWord2 FlagWord2;
	CSMFlagWord3 FlagWord3;
	CSMFlagWord4 FlagWord4;
	CSMFlagWord5 FlagWord5;

	unsigned int LastOut5;
	unsigned int LastOut6;
	unsigned int LastOut11;

	DSKY &dsky2;
	IU &iu;
};
