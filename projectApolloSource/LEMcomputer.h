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
  *	Revision 1.18  2005/09/11 15:16:59  lazyd
  *	Added definition for GetIMUOrientation
  *	
  *	Revision 1.17  2005/09/05 21:12:02  lazyd
  *	Added Radar function
  *	
  *	Revision 1.16  2005/08/27 23:36:52  lazyd
  *	P30 added
  *	
  *	Revision 1.15  2005/08/22 19:47:33  movieman523
  *	Fixed long timestep on startup, and added new Virtual AGC with EDRUPT fix.
  *	
  *	Revision 1.14  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.13  2005/08/10 21:19:44  lazyd
  *	Changed arguments for OrientAxis
  *	
  *	Revision 1.12  2005/08/06 00:03:48  movieman523
  *	Beginnings of support for AGC I/O channels in LEM.
  *	
  *	Revision 1.11  2005/08/03 10:44:33  spacex15
  *	improved audio landing synchro
  *	
  *	Revision 1.10  2005/08/01 21:48:52  lazyd
  *	Added new programs for ascent and rendezvous
  *	
  *	Revision 1.9  2005/07/16 20:40:27  lazyd
  *	Added function defs for P70 and P71
  *	
  *	Revision 1.8  2005/07/14 10:06:14  spacex15
  *	Added full apollo11 landing sound
  *	initial release
  *	
  *	Revision 1.7  2005/07/12 12:21:52  lazyd
  *	*** empty log message ***
  *	
  *	Revision 1.6  2005/06/17 18:27:02  lazyd
  *	Added declaration for GetHorizVelocity
  *	
  *	Revision 1.5  2005/06/13 18:47:03  lazyd
  *	Added declarations for ChangeDescentRate and RedesignateTarget for autoland
  *	
  *	Revision 1.4  2005/06/09 12:07:46  lazyd
  *	Added a File variable for making a log file for debugging
  *	
  *	Revision 1.3  2005/06/04 20:25:42  lazyd
  *	Added routines for landing
  *	
  *	Revision 1.2  2005/05/19 20:26:52  movieman523
  *	Rmaia's AGC 2.0 changes integrated: can't test properly as the LEM DSKY currently doesn't work!
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

	LEMcomputer(SoundLib &s, DSKY &display, IMU &im);
	virtual ~LEMcomputer();

	bool ValidateVerbNoun(int verb, int noun);
	void ProcessVerbNoun(int verb, int noun);
	bool ValidateProgram(int prog);
	unsigned int GetFlagWord(int num);
	void SetFlagWord(int num, unsigned int val);
	bool ReadMemory(unsigned int loc, int &val);
	void WriteMemory(unsigned int loc, int val);
	void ChangeDescentRate(double delta);
	void RedesignateTarget(int axis, double direction);
	void GetHorizVelocity(double &forward, double &lateral);
	int GetProgRunning();

	void Timestep(double simt, double simdt);

    int  GetStatus(double *simtime,
		                 int    *mode,
						 double *timeremaining,
						 double *timeafterpdi,
						 double *timetoapproach);
	int  SetStatus(double simtime,
                         int    mode,
				         double timeremaining,
					     double timeafterpdi,
						 double timetoapproach);

	//
	// I/O channels.
	//

	void SetInputChannelBit(int channel, int bit, bool val);

protected:

	void DisplayNounData(int noun);
	void ProgPressed(int R1, int R2, int R3);
	void ProceedNoData();
	void TerminateProgram();
	// Descent, Abort, Ascent and Rendezvous routines
	void Prog12(double simt);
//	void Prog29(double simt);
	void Prog30(double simt);
	void Prog32(double simt);
	void Prog33(double simt);
	void Prog34(double simt);
	void Prog35(double simt);
	void Prog36(double simt);
	void Prog40(double simt);
	void Prog41(double simt);
	void Prog42(double simt);
	void Prog63(double simt);
	void Prog64(double simt);
	void Prog65(double simt);
	void Prog66(double simt);
	void Prog68(double simt);
	void Prog70(double simt);
	void Prog71(double simt);
	void AbortAscent(double simt);
//	void PredictPosVelVectors(const VECTOR3 &Pos, const VECTOR3 &Vel,
//		double Mu, double Time, VECTOR3 &NewPos, VECTOR3 &NewVel, double &NewVelMag);
//	void Lambert(VECTOR3 &stpos, VECTOR3 &renpos, double dt, double mu, 
//						  VECTOR3 &v1, VECTOR3 &v2);
//	void OrientAxis(VECTOR3 &vec, int axis, int ref);
//	void OrbitParams(VECTOR3 &rpos, VECTOR3 &rvel, double &period, double &apo, double &tta, 
//				 double &per, double &ttp);
//	void EquToRel(double vlat, double vlon, double vrad, VECTOR3 &pos);
	void Phase(double &phase, double &delta);
	void Radar(double &range, double &rate);
	void GetIMUOrientation(int type, double arg, VECTOR3 &x, VECTOR3 &y, VECTOR3 &z);
	void Prog12Pressed(int R1, int R2, int R3);
//	void Prog29Pressed(int R1, int R2, int R3);
	void Prog34Pressed(int R1, int R2, int R3);
	void Prog41Pressed(int R1, int R2, int R3);
	void Prog63Pressed(int R1, int R2, int R3);
	void Prog68Pressed(int R1, int R2, int R3) { ProgState++; };
	bool OrbitCalculationsValid();
	bool DescentPhase();
	bool AscentPhase();

	LEMFlagWord0 FlagWord0;
	LEMFlagWord1 FlagWord1;
	LEMFlagWord2 FlagWord2;

	//
	// log file for autoland debugging
	//
	FILE *outstr;

	//
	//	Ascent Auto-Pilot Data
	//

	void Prog13(double simt);
	void Prog13Pressed(int R1, int R2, int R3);
	double iba;							//		Misc stuff, disregard

	Sound LunarAscent;

// Modif x15 status variable used for landing sound management	
	double simcomputert;
    int    mode;
	double timeremaining;
	double timeafterpdi;
	double timetoapproach;
	int    flags;


};