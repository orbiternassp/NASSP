/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2009

  LM Abort Guidance System

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

#include "yaAGS/aea_engine.h"

class LEM_DEDA;

// AGS Channel 04, Input Discretes
enum AGSChannelValue04_Bits {
	AGSAbortStageDiscrete = 9,
	AGSAbortDiscrete,
	AGSAscentEngineOnDiscrete,
	AGSDescentEngineOnDiscrete,
	AGSAutomaticDiscrete,
	AGSFollowUpDiscrete,
	AGSOutputTelemetryStopDiscrete,
	AGSDownlinkTelemetryStopDiscrete
};

// AGS Channel 05, Input Discretes
enum AGSChannelValue05_Bits {
	DEDAReadoutDiscrete = 10,
	DEDAEnterDiscrete,
	DEDAHoldDiscrete,
	DEDAClearDiscrete,
	GSEDiscrete1,
	GSEDiscrete2,
	GSEDiscrete3
};

// AGS Channel 40, Output Discretes
enum AGSChannelValue40_Bits {
	RippleCarryInhibit = 0,
	AGSAltitude,
	AGSAltitudeRate,
	DEDAShiftIn,
	DEDAShiftOut,
	GSEDiscrete4,
	GSEDiscrete5,
	GSEDiscrete6,
	AGSTestModeFailure,
	AGSEngineOff,
	AGSEngineOn
};

typedef std::bitset<11> AGSChannelValue40;

// ABORT SENSOR ASSEMBLY (ASA)
class LEM_ASA{
public:
	LEM_ASA();							// Cons
	void Init(LEM *l, ThreePosSwitch *s, Boiler *fastht, Boiler *fineht, h_Radiator *hr, h_HeatLoad *asah); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);
	bool IsHeaterPowered();
	bool IsPowered();
	double GetASATempF();
	double GetASA12V();
	double GetASAFreq();
	double GetASA28V();

	void PulseTimestep(int* AttPulses);
	MATRIX3 transpose_matrix(MATRIX3 a);
	VECTOR3 MatrixToEuler(MATRIX3 mat);
	LEM *lem;					// Pointer at LEM
protected:

	void TurnOn();
	void TurnOff();

	VECTOR3 GetGravityVector();

	h_Radiator *hsink;			// Case (Connected to primary coolant loop)
	Boiler *fastheater;				// Fast Warmup Heater
	Boiler *fineheater;				// Fine Control Heater
	h_HeatLoad *asaHeat;
	ThreePosSwitch *PowerSwitch;

	bool PulsesSent;
	bool Initialized;
	bool Operate;

	double LastSimDT;
	MATRIX3 CurrentRotationMatrix;
	VECTOR3 EulerAngles;
	VECTOR3 RemainingDeltaVel;
	VECTOR3 LastWeightAcceleration;
	VECTOR3 LastGlobalVel;

	const double AttPulsesScal = pow(2.0, 16.0);
	const double AccPulsesScal = 1.0 / 0.003125 / 0.3048;
};

// ABORT ELECTRONICS ASSEMBLY (AEA)
class LEM_AEA{
public:
	LEM_AEA(PanelSDK &p, LEM_DEDA &display);							// Cons
	void Init(LEM *s, h_HeatLoad *aeah, h_HeatLoad *secaeah); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void InitVirtualAGS(char *binfile);
	void SetInputPortBit(int port, int bit, bool val);
	void SetInputPort(int port, int val);
	void SetOutputChannel(int Type, int Data);
	unsigned int GetOutputChannel(int channel);
	unsigned int GetInputChannel(int channel);
	void SetMissionInfo(int MissionNo);
	void SetFlightProgram(int FP);
	void PadLoad(unsigned int address, unsigned int value);
	void WriteMemory(unsigned int loc, int val);
	bool ReadMemory(unsigned int loc, int &val);
	void SetAGSAttitude(int Type, int Data);
	void SetAGSAttitudeError(int Type, int Data);
	void SetLateralVelocity(int Data);
	void SetAltitudeAltitudeRate(int Data);
	void SetPGNSIntegratorRegister(int channel, int val);

	double GetLateralVelocity();
	double GetAltitude();
	double GetAltitudeRate();
	VECTOR3 GetTotalAttitude();
	VECTOR3 GetAttitudeError();

	void ResetDEDAShiftIn();
	void ResetDEDAShiftOut();

	void WireToBuses(e_object *a, e_object *b, ThreePosSwitch *s);
	bool IsPowered();
	bool IsACPowered();
	bool GetTestModeFailure();
	LEM *lem;					// Pointer at LEM
	h_HeatLoad *aeaHeat;
	h_HeatLoad *secaeaHeat;

protected:
	ags_t vags;
	PowerMerge DCPower;
	ThreePosSwitch *PowerSwitch;

	LEM_DEDA &deda;

#define MAX_INPUT_PORTS		020
#define MAX_OUTPUT_PORTS	021

	unsigned int OutputPorts[MAX_OUTPUT_PORTS];

	int FlightProgram;
	bool AEAInitialized;
	double LastCycled;
	int ASACycleCounter;

	//AEA attitude display
	double sin_theta;
	double cos_theta;
	double sin_phi;
	double cos_phi;
	double sin_psi;
	double cos_psi;

	//AEA attitude error
	VECTOR3 AGSAttitudeError;

	//AEA lateral velocity in feet
	double AGSLateralVelocity;

	double Altitude;
	double AltitudeRate;

	const double ATTITUDESCALEFACTOR = pow(2.0, -17.0);
	const double ATTITUDEERRORSCALEFACTOR = 0.5113269e-3*pow(2.0, -8.0);
	const double LATVELSCALEFACTOR = 100.0*pow(2.0, -16.0);
	const double ALTSCALEFACTOR = 0.3048*2.345*pow(2.0, -3.0);
	const double ALTRATESCALEFACTOR = 0.3048*pow(2.0, -4.0);
};

// DATA ENTRY and DISPLAY ASSEMBLY (DEDA)

class LEM_DEDA : public e_object
{
public:
	LEM_DEDA(LEM *lem, SoundLib &s, LEM_AEA &computer);
	virtual ~LEM_DEDA();

	void Init(e_object *powered);
	void Reset();

	//
	// Light status.
	//

	bool OprErrLit()	{ return OprErrLight; };
	//
	// Set light status.
	//

	void SetOprErr(bool val)		{ OprErrLight = val; };
	void ClearOprErr()		{ OprErrLight = false; };
	//
	// Timestep to run programs.
	//

	void Timestep(double simt);
	void SystemTimestep(double simdt);

	void ProcessChannel27(int val);
	void ProcessChannel40(AGSChannelValue40 val);

	//
	// Keypad interface.
	//

	void EnterPressed();
	void ClearPressed();
	void HoldPressed();
	void ReadOutPressed();
	void PlusPressed();
	void MinusPressed();
	void NumberPressed(int n);

	void HoldCallback(PanelSwitchItem* s);
	void EnterCallback(PanelSwitchItem* s);
	void ClearCallback(PanelSwitchItem* s);
	void ReadOutCallback(PanelSwitchItem* s);
	void PlusCallback(PanelSwitchItem* s);
	void MinusCallback(PanelSwitchItem* s);
	void zeroCallback(PanelSwitchItem* s);
	void oneCallback(PanelSwitchItem* s);
	void twoCallback(PanelSwitchItem* s);
	void threeCallback(PanelSwitchItem* s);
	void fourCallback(PanelSwitchItem* s);
	void fiveCallback(PanelSwitchItem* s);
	void sixCallback(PanelSwitchItem* s);
	void sevenCallback(PanelSwitchItem* s);
	void eightCallback(PanelSwitchItem* s);
	void nineCallback(PanelSwitchItem* s);

	void ProcessKeyPress(int mx, int my);
	void ProcessKeyRelease(int mx, int my);
	void RenderOprErr(SURFHANDLE surf, SURFHANDLE lights);
	void RenderAdr(SURFHANDLE surf, SURFHANDLE digits, int xoffset = 0, int yoffset = 0);
	void RenderData(SURFHANDLE surf, SURFHANDLE digits, int xoffset = 0, int yoffset = 0);
	void RenderKeys(SURFHANDLE surf, SURFHANDLE keys, int xoffset = 0, int yoffset = 0);

	void KeyClick();
	bool IsPowered() { return Voltage() > 25.0; };

	//
	// Helper functions.
	//

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

protected:
	//
	// Lights.
	//

	void LightOprErrLight()		{ OprErrLight = true; };
	void ClearOprErrLight()		{ OprErrLight = false; };

	//
	// Light power consumption.
	//

	int LightsLit;
	int SegmentsLit;

	//
	// Lights state.
	//

	bool OprErrLight;

	//
	// Keyboard state.
	//

	bool KeyDown_Plus;
	bool KeyDown_Minus;
	bool KeyDown_0;
	bool KeyDown_1;
	bool KeyDown_2;
	bool KeyDown_3;
	bool KeyDown_4;
	bool KeyDown_5;
	bool KeyDown_6;
	bool KeyDown_7;
	bool KeyDown_8;
	bool KeyDown_9;
	bool KeyDown_Clear;
	bool KeyDown_ReadOut;
	bool KeyDown_Enter;
	bool KeyDown_Hold;

	//
	// Current program state.
	//

	int State;
	//
	// Internal variables.
	//

	int ShiftRegister[9];
	char Adr[4];
	char Data[7];

	//
	// AGS we're connected to.
	//

	LEM_AEA &ags;

	//
	// Sound library.
	//

	SoundLib &soundlib;
	Sound Sclick;

	bool FirstTimeStep;

	//
	// Local helper functions.
	//

	void SetAddress();
	void SetData();
	char ValueChar(unsigned val);
	char ValueCharSign(unsigned val);
	void ResetKeyDown();
	void SendKeyCode(int val);

	void DEDAKeyBlt(SURFHANDLE surf, SURFHANDLE keys, int dstx, int dsty, int srcx, int srcy, bool lit, int xOffset, int yOffset); 
	void RenderThreeDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str);
	void RenderSixDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str);
	int ThreeDigitDisplaySegmentsLit(char *Str);
	int SixDigitDisplaySegmentsLit(char *Str);
	LEM *lem;					// Pointer at LEM
};

//
// Strings for state saving.
//

#define DEDA_START_STRING	"DEDA_BEGIN"
#define DEDA_END_STRING		"DEDA_END"

#define AEA_MEM_ENTRIES	(04000)			///< Number of memory values to simulate


