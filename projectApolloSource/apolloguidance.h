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
  *	Revision 1.15  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.14  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.13  2005/08/09 02:28:25  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.12  2005/08/08 22:32:49  movieman523
  *	First steps towards reimplementing the DSKY interface to use the same I/O channels as the real AGC/DSKY interface.
  *	
  *	Revision 1.11  2005/08/08 21:10:30  movieman523
  *	Fixed broken TLI program. LastAlt wasn't being set and that screwed up the burn end calculations.
  *	
  *	Revision 1.10  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.9  2005/08/05 23:37:21  movieman523
  *	Added AGC I/O channel simulation to make integrating Virtual AGC easier.
  *	
  *	Revision 1.8  2005/08/01 21:46:18  lazyd
  *	Changed Program 33 to Program 37
  *	
  *	Revision 1.7  2005/07/09 18:30:57  lazyd
  *	Added argument to ComAttitude function
  *	
  *	Revision 1.6  2005/07/06 21:58:09  lazyd
  *	Added function defs for GetHoverAttitiude and ComAttitude
  *	
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
class IMU;

#include "control.h"
#include "yaAGC/agc_engine.h"

//
// Velocity in feet per second or meters per second?
//

typedef enum MeasurementUnits { UnitImperial, UnitMetric };

class ApolloGuidance

{
public:
	ApolloGuidance(SoundLib &s, DSKY &display, IMU &im, char *binfile);
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
	bool OnStandby() { return (!Yaagc && Standby); };

	bool OutOfReset();

	void Checklist(int num);
	void BlankAll();

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
	// External event handlers.
	//

	void SetFuel(double fuel) { CurrentFuel = fuel; };
	void SetRVel(double vel) { CurrentRVel = vel; };
	void LaunchShutdown();

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	//
	// I/O channels.
	//

	virtual bool GetOutputChannelBit(int channel, int bit);
	virtual unsigned int GetOutputChannel(int channel);
	virtual void SetInputChannel(int channel, unsigned int val);
	virtual void SetInputChannelBit(int channel, int bit, bool val);
	virtual void SetOutputChannelBit(int channel, int bit, bool val);
	virtual void SetOutputChannel(int channel, unsigned int val);

	bool GetInputChannelBit(int channel, int bit);
	unsigned int GetInputChannel(int channel);

	//
	// Virtual AGC memory access.
	//

	int GetErasable(int bank, int address);
	void SetErasable(int bank, int address, int value);

	//
	// Generally useful setup.
	//

	void SetMissionInfo(int MissonNo, int RealismValue);

protected:

	//
	// Various programs we can run.
	//

	void ResetProg(double simt);
	void Prog37(double simt);
	void Prog37Pressed(int R1, int R2, int R3);

	//
	// DSKY interface.
	//

	char ValueChar(unsigned val);
	unsigned CharValue(char val);

	void SetChannel10Lights(int bit, bool val);

	void ProcessInputChannel15(int val);
	void ProcessInputChannel32(int bit, bool val);

	virtual void ProcessChannel10(int val);
	virtual void ProcessChannel11Bit(int bit, bool val);
	virtual void ProcessChannel11(int val);

	void LightUplink();
	void ClearUplink();
	void LightCompActy();
	void ClearCompActy();
	void LightTemp();
	void ClearTemp();
	void LightKbRel();
	void ClearKbRel();
	void LightOprErr();
	void ClearOprErr();
	void LightNoAtt();
	void ClearNoAtt();
	void LightGimbalLock();
	void ClearGimbalLock();
	void LightTracker();
	void ClearTracker();
	void LightProg();
	void ClearProg();
	void LightVel();
	void ClearVel();
	void LightAlt();
	void ClearAlt();

	void SetVerbNounFlashing();
	void ClearVerbNounFlashing();

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
	void ComAttitude(VECTOR3 &actatt, VECTOR3 &tgtatt, bool fast);
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
	// DSKY interface.
	//

	int Chan10Flags;

	int Prog;
	bool ProgBlanked;
	int Verb;
	bool VerbBlanked;
	int Noun;
	bool NounBlanked;
	int R1;
	bool R1Blanked;
	bool R1Decimal;
	char R1Format[7];
	int R2;
	bool R2Blanked;
	bool R2Decimal;
	char R2Format[7];
	int R3;
	bool R3Blanked;
	bool R3Decimal;
	char R3Format[7];

	char TwoDigitEntry[2];
	char FiveDigitEntry[6];

	//
	// Temporary for transferring values to DSKY.
	//

	char RegStr[8];

	//
	// Data entry status.
	//

	bool EnteringVerb;
	bool EnteringNoun;
	bool EnteringOctal;

	int	EnterPos;
	int EnterVal;
	int	EnteringData;
	int EnterCount;

	bool EnterPositive;

	bool KbInUse;

	//
	// DSKY interface functions.
	//

	void StartTwoDigitEntry();
	void StartFiveDigitEntry(bool octal);
	void ClearFiveDigitEntry();
	void UpdateTwoDigitEntry(int n);
	void UpdateFiveDigitEntry(int n);

	void TwoDigitDisplay(char *Str, int val, bool Blanked);
	void FiveDigitDisplay(char *Str, int val, bool Blanked, bool Decimal, char *Format);

	bool KBCheck();
	void SetR1(int val);
	void SetR2(int val);
	void SetR3(int val);
	void SetR1Octal(int val);
	void SetR2Octal(int val);
	void SetR3Octal(int val);
	void DisplayR1Octal() { R1Decimal = false; UpdateR1(); };
	void DisplayR2Octal() { R2Decimal = false; UpdateR2(); };
	void DisplayR3Octal() { R3Decimal = false; UpdateR3(); };
	void SetR1Format(char *fmt) { strncpy(R1Format, fmt, 7); UpdateR1(); };
	void SetR2Format(char *fmt) { strncpy(R2Format, fmt, 7); UpdateR2(); };
	void SetR3Format(char *fmt) { strncpy(R3Format, fmt, 7); UpdateR3(); };
	void SetProg(int val);
	void SetVerb(int val);
	void SetNoun(int val);

	void BlankData();
	void BlankR1();
	void BlankR2();
	void BlankR3();
	void UnBlankAll();
		
	void ReleaseKeyboard();
	void UpdateProg();
	void UpdateNoun();
	void UpdateVerb();
	void UpdateR1();
	void UpdateR2();
	void UpdateR3();
	void UpdateAll();
	void UpdateEntry();
	void KeyRel();
	void VerbPressed();
	void NounPressed();
	void ProgKeyPressed();
	void ResetPressed();
	void ClearPressed();
	void DataEntryR1();
	void DataEntryR2();
	void DataEntryR3();

	void DoSetR1(int val, bool decimal);
	void DoSetR2(int val, bool decimal);
	void DoSetR3(int val, bool decimal);

	void PlusPressed();
	void MinusPressed();
	void EnterPressed();
	void NumberPressed(int val);

	void LightsOff();

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
	bool isFirstTimestep;

	DSKY &dsky;
	IMU &imu;

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
	// Program data.
	//

	double LastAlt;

	//
	// Various program internal state variables.
	//

	int ResetCount;
	int ApolloNo;
	int Realism;
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

#define MAX_INPUT_CHANNELS	0200
#define MAX_OUTPUT_CHANNELS	0200

	unsigned int InputChannel[MAX_INPUT_CHANNELS + 1];
	unsigned int OutputChannel[MAX_OUTPUT_CHANNELS + 1];

	//
	// The Vessel we're controlling.
	//

	VESSEL	*OurVessel;
	SoundLib &soundlib;

	//
	// Virtual AGC.
	//

	agc_t vagc;
};

extern char TwoSpaceTwoFormat[];

//
// Strings for state saving.
//

#define AGC_START_STRING	"AGC_BEGIN"
#define AGC_END_STRING		"AGC_END"

#define EMEM_ENTRIES	(8 * 0400)
