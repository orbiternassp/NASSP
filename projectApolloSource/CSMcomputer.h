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

//
// Class definition.
//

class CSMcomputer: public ApolloGuidance

{
public:

	CSMcomputer(SoundLib &s, DSKY &display, DSKY &display2, IMU &im, PanelSDK &p);
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

	void Liftoff(double simt);

	//
	// Data access.
	//

	double GetDesiredApogee() { return DesiredApogee; };
	double GetDesiredPerigee() { return DesiredPerigee; };
	double GetDesiredAzimuth() { return DesiredAzimuth; };

	void SetDesiredApogee(double val) { DesiredApogee = val; };
	void SetDesiredPerigee(double val) { DesiredPerigee = val; };
	void SetDesiredAzimuth(double val) { DesiredAzimuth = val; };
	void SetBurnTime(double val) { BurnTime = val; };

	void SetInputChannelBit(int channel, int bit, bool val);

	void SetMissionInfo(int MissionNo, int RealismValue, char *OtherVessel = 0);

protected:

	void DisplayNounData(int noun);
	void ProgPressed(int R1, int R2, int R3);
	double CalcTFF(double vy, double r0, double g);
	double CurrentG();
	bool OrbitCalculationsValid();
	void DisplayBankSum();

	void ProcessChannel5(int val);
	void ProcessChannel6(int val);
	void ProcessChannel10(int val);
	void ProcessChannel11Bit(int bit, bool val);
	void ProcessChannel11(int val);
	void CheckEngineOnOff(int val);

	//
	// Programs we can run.
	//

	void Prog01(double simt);
	void Prog02(double simt);
	void Prog06(double simt);
	void Prog11(double simt);
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

	//
	// Sounds.
	//

	Sound STLI;
	Sound STLIStart;
	Sound Scount;

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
};
