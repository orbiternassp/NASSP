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

  // AGS Channel 11, Output Discretes
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

// ABORT SENSOR ASSEMBLY (ASA)
class LEM_ASA{
public:
	LEM_ASA();							// Cons
	void Init(LEM *s, Boiler *hb, h_Radiator *hr); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simdt);
	LEM *lem;					// Pointer at LEM
protected:
	h_Radiator *hsink;			// Case (Connected to primary coolant loop)
	Boiler *heater;				// Heater
};

// ABORT ELECTRONICS ASSEMBLY (AEA)
class LEM_AEA{
public:
	LEM_AEA(PanelSDK &p, LEM_DEDA &display);							// Cons
	void Init(LEM *s); // Init
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
	void TimeStep(double simt, double simdt);
	void InitVirtualAGS(char *binfile);
	void SetInputPortBit(int port, int bit, bool val);
	void SetInputPort(int port, int val);
	void SetOutputChannel(int Type, int Data);
	unsigned int GetOutputChannel(int channel);
	void SetMissionInfo(int MissionNo);
	void WireToBuses(e_object *a, e_object *b, ThreePosSwitch *s);
	bool IsPowered();
	LEM *lem;					// Pointer at LEM

protected:
	ags_t vags;
	PowerMerge DCPower;
	ThreePosSwitch *PowerSwitch;

	LEM_DEDA &deda;

#define MAX_OUTPUT_PORTS	040

	unsigned int OutputPorts[MAX_OUTPUT_PORTS];

	double LastCycled;
};

// DATA ENTRY and DISPLAY ASSEMBLY (DEDA)

class LEM_DEDA : public e_object
{
public:
	LEM_DEDA(LEM *lem, SoundLib &s, LEM_AEA &computer, int IOChannel = 015);
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

	void TimeStep(double simt);
	void SystemTimestep(double simdt);

	void ProcessChannel27(int val);
	void ProcessChannel40(int val);

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

	void LightsOff();

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

	bool KbInUse;
	bool Held;

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

	char Adr[3];
	char Data[6];

	int	EnterPos;
	int EnterVal;

	//
	// AGS we're connected to.
	//

	LEM_AEA &ags;

	//
	// I/O channel to use for key-codes.
	//

	int KeyCodeIOChannel;

	//
	// Sound library.
	//

	SoundLib &soundlib;
	Sound Sclick;

	bool FirstTimeStep;

	//
	// Local helper functions.
	//

	char ValueChar(unsigned val);
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


