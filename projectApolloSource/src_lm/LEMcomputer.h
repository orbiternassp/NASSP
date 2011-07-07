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
  *	Revision 1.6  2009/10/19 12:24:49  dseagrav
  *	LM checkpoint commit.
  *	Put back one FDAI for testing purposes (graphic is wrong)
  *	Messed around with mass properties
  *	LGC now runs Luminary 099 instead of 131
  *	Added LGC pad load, values need worked but addresses are checked.
  *	
  *	Revision 1.5  2009/09/02 18:26:46  vrouleau
  *	MultiThread support for vAGC
  *	
  *	Revision 1.4  2009/08/16 03:12:38  dseagrav
  *	More LM EPS work. CSM to LM power transfer implemented. Optics bugs cleared up.
  *	
  *	Revision 1.3  2009/08/01 23:06:33  jasonims
  *	LM Optics Code Cleaned Up... Panel Code added for LM Optics... Knobs activated... Counter and Computer Controls still to come.
  *	
  *	Revision 1.2  2009/08/01 19:48:33  jasonims
  *	LM Optics Code Added, along with rudimentary Graphics for AOT.
  *	Reticle uses GDI objects to allow realtime rotation.
  *	
  *	Revision 1.1  2009/02/18 23:21:14  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.26  2007/06/06 15:02:09  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.25  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.24  2006/07/26 15:42:02  tschachim
  *	Temporary fix of the lm landing autopilot until correct attitude control is ready.
  *	
  *	Revision 1.23  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.22  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.21  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.20  2006/01/14 20:58:15  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.19  2005/09/20 22:38:27  lazyd
  *	Moved programs from here to apolloguidance
  *	
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
#if !defined(_PA_LMCOMPUTER_H)
#define _PA_LMCOMPUTER_H

#include "thread.h"

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

///
/// \ingroup AGCFlags
///
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

///
/// \ingroup AGCFlags
///
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


class Saturn;
///
/// \ingroup AGC
/// LEM AGC
/// \brief Derived class for the LEM AGC with extra LEM-specific functionality.
///
class LEMcomputer: public ApolloGuidance

{
public:

	LEMcomputer(SoundLib &s, DSKY &display, IMU &im, PanelSDK &p);
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
	void Run() ;
	void agcTimestep(double simt, double simdt);


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

	VESSEL *GetCSM();
protected:
	// DS20060413
	void ProcessChannel13(int val);
	void ProcessChannel5(int val);
	void ProcessChannel6(int val);
	// DS20090919
	void ProcessIMUCDUErrorCount(int channel, unsigned int val);
	void ProcessChannel160(int val);
	void ProcessChannel161(int val);

	void DisplayNounData(int noun);
	void ProgPressed(int R1, int R2, int R3);
	void ProceedNoData();
	void TerminateProgram();
	// Descent, Abort, Ascent and Rendezvous routines
	void Prog12(double simt);
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
	void Phase(double &phase, double &delta);
	void Radar(double &range, double &rate);
	void GetIMUOrientation(int type, double arg, VECTOR3 &x, VECTOR3 &y, VECTOR3 &z);
	void Prog12Pressed(int R1, int R2, int R3);
	void Prog34Pressed(int R1, int R2, int R3);
	void Prog41Pressed(int R1, int R2, int R3);
	void Prog63Pressed(int R1, int R2, int R3);
	void Prog68Pressed(int R1, int R2, int R3) { ProgState++; };
	bool OrbitCalculationsValid();
	bool DescentPhase();
	bool AscentPhase();
	void ResetAttitudeLevel();
	void AddAttitudeRotLevel(VECTOR3 level);
	void AddAttitudeLinLevel(VECTOR3 level);
	void AddAttitudeLinLevel(int axis, double level);
	void SetAttitudeRotLevel(VECTOR3 level);
	
	///
	/// \brief Set the thrust level of the main engine.
	/// \param thrust Thrust level (0.0 to 1.0).
	///
	virtual void BurnMainEngine(double thrust);


	LEMFlagWord0 FlagWord0;
	LEMFlagWord1 FlagWord1;
	LEMFlagWord2 FlagWord2;

	double RCSCommand[16];
	VECTOR3 CommandedAttitudeRotLevel;	// store current thrust levels between the guidance loop steps
	VECTOR3 CommandedAttitudeLinLevel;	

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

	Saturn *sat;
};
class LEM;

// *** LM OPTICS ***
// I guess this can go here; it doesn't really warrant its own file, and it's part of GNC, so...

///
/// \ingroup AGC
/// \brief LM Optics.
///
class LMOptics {	
public: 
	LMOptics();													// Cons
	void Init(LEM *vessel);										// Initialization
	void TimeStep(double simdt);                                // Timestep
	void SystemTimestep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	//
	// These should really be protected variables.
	//

	LEM *lem; 										 			// Our Ship

	int OpticsShaft;											// Shaft Position (can be 0=0, 1=60, 2=120, 3=180, 4=-120, 5=-60 degrees)
	double OpticsReticle;										// AOT Reticle rotation
	double ReticleMoved;										// 0 is no movement detected, - is clockwise, + is counterclockwise
	int RetDimmer;
	int KnobTurning;
};

//
// Strings for state saving.
//

#define LMOPTICS_START_STRING "LMOPTICS_BEGIN"
#define LMOPTICS_END_STRING   "LMOPTICS_END"


#endif // _PA_LMCOMPUTER_H