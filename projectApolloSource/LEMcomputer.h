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
		unsigned P66PROFL:1;
		unsigned R10FLAG:1;
		unsigned FREEFBIT:1;
		unsigned NEEDLBIT:1;
		unsigned LOKONBIT:1;
		unsigned RRNBSW:1;
		unsigned RNDVZBIT:1;
		unsigned IMUSE:1;
		unsigned P25FLAG:1;
		unsigned FSPASFLG:1;
		unsigned P21FLAG:1;
		unsigned MOONFLAG:1;
		unsigned MIDFLAG:1;
		unsigned JSWITCH:1;
		unsigned NEED2BIT:1;
	} u;
	unsigned int word;
} LEMFlagWord0;


typedef union {
	struct {
		unsigned NOTASSIGNED:1;
		unsigned GUESSW:1;
		unsigned SLOPESW:1;
		unsigned NOTASSIGNED2:1;
		unsigned TRACKBIT:1;
		unsigned NOUPFBIT:1;
		unsigned UPDATBIT:1;
		unsigned VEHUPFLG:1;
		unsigned NOTASSIGNED3:1;
		unsigned R61FLAG:1;
		unsigned NOTERBT:1;
		unsigned RODFLAG:1;
		unsigned ERADFLAG:1;
		unsigned DIDFLBIT:1;
		unsigned NJETSFLG:1;
	} u;
	unsigned int word;
} LEMFlagWord1;

typedef union {
	struct {
		unsigned NODOBIT:1;
		unsigned CALCMAN2:1;
		unsigned CALCMAN3:1;
		unsigned PFRATBIT:1;
		unsigned AVFLAG:1;
		unsigned FINALFLG:1;
		unsigned ETPIFLAG:1;
		unsigned XDELVFLG:1;
		unsigned IMPULBIT:1;
		unsigned NOTASSIGNED:1;
		unsigned STEERBIT:1;
		unsigned LOSCMBIT:1;
		unsigned ACMODFLG:1;
		unsigned SRCHOBIT:1;
		unsigned DRFTBIT:1;
	} u;
	unsigned int word;
} LEMFlagWord2;

class LEMcomputer: public ApolloGuidance

{
public:

	LEMcomputer(SoundLib &s, DSKY &display);
	virtual ~LEMcomputer();

	bool ValidateVerbNoun(int verb, int noun);
	void ProcessVerbNoun(int verb, int noun);
	bool ValidateProgram(int prog);
	unsigned int GetFlagWord(int num);
	void SetFlagWord(int num, unsigned int val);
	bool ReadMemory(unsigned int loc, int &val);
	void WriteMemory(unsigned int loc, int val);

	void Timestep(double simt);

protected:

	void DisplayNounData(int noun);
	void ProgPressed(int R1, int R2, int R3);
	void ProceedNoData();
	void TerminateProgram();
	void Prog63(double simt);
	void Prog68(double simt);
	void Prog63Pressed(int R1, int R2, int R3);
	void Prog68Pressed(int R1, int R2, int R3) { ProgState++; };
	bool OrbitCalculationsValid();
	bool DescentPhase();
	bool AscentPhase();

	LEMFlagWord0 FlagWord0;
	LEMFlagWord1 FlagWord1;
	LEMFlagWord2 FlagWord2;

	//
	//	Ascent Auto-Pilot Data
	//

	void Prog12(double simt);
	void Prog12Pressed(int R1, int R2, int R3);
	double iba;							//		Misc stuff, disregard

	Sound LunarAscent;

};