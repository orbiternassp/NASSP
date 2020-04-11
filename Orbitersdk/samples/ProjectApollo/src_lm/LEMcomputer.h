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
#if !defined(_PA_LMCOMPUTER_H)
#define _PA_LMCOMPUTER_H

#include "thread.h"

class LEM;

///
/// \ingroup AGC
/// LEM AGC
/// \brief Derived class for the LEM AGC with extra LEM-specific functionality.
///
class LEMcomputer: public ApolloGuidance

{
public:

	LEMcomputer(SoundLib &s, DSKY &display, IMU &im, CDU &sc, CDU &tc, PanelSDK &p);
	virtual ~LEMcomputer();

	bool ReadMemory(unsigned int loc, int &val);
	void WriteMemory(unsigned int loc, int val);

	void Timestep(double simt, double simdt);
	void Run() ;
	void agcTimestep(double simt, double simdt);

	//
	// I/O channels.
	//

	void SetInputChannelBit(int channel, int bit, bool val);

	void SetMissionInfo(std::string AGCVersion, char *OtherVessel = 0);

	VESSEL *GetCSM();
protected:
	// DS20060413
	void ProcessChannel13(ChannelValue val);
	void ProcessChannel5(ChannelValue val);
	void ProcessChannel6(ChannelValue val);
	void ProcessChannel10(ChannelValue val);
	// DS20090919
	void ProcessIMUCDUErrorCount(int channel, ChannelValue val);
	void ProcessIMUCDUReadCount(int channel, int val);
	void ProcessChannel142(ChannelValue val);
	void ProcessChannel143(ChannelValue val);
	void ProcessChannel34(ChannelValue val);

	//
	// log file for autoland debugging
	//
	FILE *outstr;

	Sound LunarAscent;

	LEM *lem;
};

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
	void Timestep(double simdt);                                // Timestep
	void SystemTimestep(double simdt);
	bool PaintReticleAngle(SURFHANDLE surf, SURFHANDLE digits);	// Update panel image
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
