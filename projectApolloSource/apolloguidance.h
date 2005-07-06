/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Generic Apollo Guidance computer class which is hooked
  up to the DSKY to allow easy support for the CSM and LEM
  computers. This defines the interfaces that the DSKY will
  use to either computer, so only parts specific to the CSM
  or LEM will need to be written specially.

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
  *	Revision 1.5  2005/05/19 20:26:52  movieman523
  *	Rmaia's AGC 2.0 changes integrated: can't test properly as the LEM DSKY currently doesn't work!
  *	
  *	Revision 1.4  2005/04/30 23:09:15  movieman523
  *	Revised CSM banksums and apogee/perigee display to match the real AGC.
  *	
  *	Revision 1.3  2005/04/20 17:44:48  movieman523
  *	Added call to force restart of the AGC.
  *	
  *	Revision 1.2  2005/02/24 00:24:10  movieman523
  *	Added function to get Apollo mission number from AGC.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

class DSKY;

#include "control.h"

//
// Velocity in feet per second or meters per second?
//

typedef enum MeasurementUnits { UnitImperial, UnitMetric };

class ApolloGuidance

{
public:
	ApolloGuidance(SoundLib &s, DSKY &display);
	virtual ~ApolloGuidance();

	void RunProgram(int prog);

	virtual bool ValidateVerbNoun(int verb, int noun) = 0;
	virtual void ProcessVerbNoun(int verb, int noun) = 0;
	virtual bool ValidateProgram(int prog) = 0;
	virtual void DisplayNounData(int noun) = 0;
	virtual void ProgPressed(int R1, int R2, int R3) = 0;
	virtual void ProceedNoData() = 0;
	virtual void TerminateProgram() = 0;
	virtual unsigned int GetFlagWord(int num) = 0;
	virtual void SetFlagWord(int num, unsigned int val) = 0;
	virtual bool ReadMemory(unsigned int loc, int &val) = 0;
	virtual void WriteMemory(unsigned int loc, int val) = 0;


	void RSetPressed();

	void VerbNounEntered(int verb, int noun);
	bool ValidateCommonVerbNoun(int verb, int noun);
	bool ValidateCommonProgram(int prog);
	bool DisplayCommonNounData(int noun);
	bool GenericProgPressed(int R1, int R2, int R3);
	void ProcessCommonVerbNoun(int verb, int noun);
	bool CommonProceedNoData();

	virtual void Timestep(double simt) = 0;

	void SetVesselStats(double ISP, double Thrust, bool MainIsHover);
	void ControlVessel(VESSEL *v) { OurVessel = v; };
	void SetApolloNo(int flight) { ApolloNo = flight; };
	int GetApolloNo() { return ApolloNo; };
	void SetDesiredLanding(double latitude, double longitude, double altitude);

	void ForceRestart();
	void Startup();
	void GoStandby();
	bool OnStandby() { return Standby; };

	bool OutOfReset();

	void Checklist(int num);

	//
	// Program helper functions.
	//

	void ResetCountdown();
	void TerminateCommonProgram();
	void AbortWithError(int ErrNo);
	double DisplayVel(double vel);
	double DisplayAlt(double alt);
	double GetVel(double vel);

	//
	// Externl event handlers.
	//

	void SetFuel(double fuel) { CurrentFuel = fuel; };
	void SetRVel(double vel) { CurrentRVel = vel; };
	void LaunchShutdown();

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:

	//
	// Various programs we can run.
	//

	void ResetProg(double simt);
	void Prog33(double simt);
	void Prog33Pressed(int R1, int R2, int R3);

	//
	// Odds and ends.
	//

	void RaiseAlarm(int AlarmNo);
	void DoOrbitBurnCalcs(double simt);
	bool GenericTimestep(double simt);
	bool GenericReadMemory(unsigned int loc, int &val);
	void GenericWriteMemory(unsigned int loc, int val);
	void DisplayTime(double t);
	void AwaitProgram();
	void SetVerbNounAndFlash(int Verb, int Noun);
	void SetVerbNoun(int Verb, int Noun);
	double NewVelocity_AorP(double Mu_Planet, double &Rapo, double &Rperi, double &Rnew);
	void KillAllThrusters();
	void OrientForOrbitBurn(double simt);
	void GetHoverAttitude( VECTOR3 &actatt);
	void ComAttitude(VECTOR3 &actatt, VECTOR3 &tgtatt);
	void BurnMainEngine(double thrust);
	virtual void DisplayBankSum();
	void DisplayEMEM(unsigned int addr);
	virtual bool OrbitCalculationsValid() = 0;
	void DisplayOrbitCalculations();
	void UpdateBurnTime(int R1, int R2, int R3);

	bool Standby;
	bool Reset;
	bool InOrbit;
	bool MainThrusterIsHover;

	//
	// Generic program flags. Higher
	// level code is responsible for
	// saving these in the Flag Words.
	//

	bool ProgFlag01;
	bool ProgFlag02;
	bool ProgFlag03;
	bool ProgFlag04;
	bool RetroFlag;
	bool BurnFlag;

	DSKY &dsky;

	int ProgRunning;
	int VerbRunning;
	int NounRunning;
	int ProgState;

	int Alarm01;
	int Alarm02;
	int Alarm03;

	unsigned int TIME1;

	MeasurementUnits DisplayUnits;

	//
	// Various program internal state variables.
	//

	int ResetCount;
	int ApolloNo;
	int Yaagc;

	double ResetTime;
	double LastProgTime;
	double NextProgTime;
	double LastTimestep;
	double CurrentTimestep;

	void GetPosVel();

	double CurrentVel;
	double CurrentRVel;
	double CurrentVelX;
	double CurrentVelY;
	double CurrentVelZ;
	double CurrentFuel;
	double CurrentAlt;

	double TargetRoll;
	double TargetPitch;
	double TargetYaw;

	double DesiredApogee;
	double DesiredPerigee;
	double DesiredAzimuth;

	double LandingLatitude;
	double LandingLongitude;
	double LandingAltitude;
	double DeltaPitchRate;

	double LastVerb16Time;
	double NextEventTime;
	double LastEventTime;

	double BurnTime;
	double CutOffVel;
	double BurnStartTime;
	double BurnEndTime;
	double DesiredDeltaV;

	double MaxThrust;
	double VesselISP;

	double DesiredDeltaVx;
	double DesiredDeltaVy;
	double DesiredDeltaVz;

	double DesiredPlaneChange;
	double DesiredLAN;

	unsigned int BankSumNum;
	unsigned int CurrentEMEMAddr;

	//
	// The Vessel we're controlling.
	//

	VESSEL	*OurVessel;
	SoundLib &soundlib;
};

extern char TwoSpaceTwoFormat[];

//
// Strings for state saving.
//

#define AGC_START_STRING	"AGC_BEGIN"
#define AGC_END_STRING		"AGC_END"

#define EMEM_ENTRIES	256
