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
  **************************************************************************/

class DSKY

{
public:

	DSKY(SoundLib &s, ApolloGuidance &computer);
	virtual ~DSKY();

	void Init();
	void Reset();

	//
	// Light status.
	//

	bool UplinkLit()	{ return UplinkLight; };
	bool NoAttLit()		{ return NoAttLight; };
	bool StbyLit()		{ return StbyLight; };
	bool KbRelLit()		{ return KbRelLight; };
	bool OprErrLit()	{ return OprErrLight; };
	bool TempLit()		{ return TempLight; };
	bool GimbalLockLit() { return GimbalLockLight; };
	bool ProgLit()		{ return ProgLight; };
	bool RestartLit()	{ return RestartLight; };
	bool TrackerLit()	{ return TrackerLight; };

	//
	// Set light status.
	//

	void LightUplink()		{ UplinkLight = true; };
	void LightNoAtt()		{ NoAttLight = true; };
	void LightStby()		{ StbyLight = true; };
	void LightKbRel()		{ KbRelLight = true; };
	void LightOprErr()		{ OprErrLight = true; };
	void LightTemp()		{ TempLight = true; };
	void LightGimbalLock()	{ GimbalLockLight = true; };
	void LightProg()		{ ProgLight = true; };
	void LightRestart()		{ RestartLight = true; };
	void LightTracker()		{ TrackerLight = true; };

	void ClearUplink()		{ UplinkLight = false; };
	void ClearNoAtt()		{ NoAttLight = false; };
	void ClearStby()		{ StbyLight = false; };
	void ClearKbRel()		{ KbRelLight = false; };
	void ClearOprErr()		{ OprErrLight = false; };
	void ClearTemp()		{ TempLight = false; };
	void ClearGimbalLock()	{ GimbalLockLight = false; };
	void ClearProg()		{ ProgLight = false; };
	void ClearRestart()		{ RestartLight = false; };
	void ClearTracker()		{ TrackerLight = false; };

	//
	// Flashing status.
	//

	void SetVerbFlashing()	{ VerbFlashing = true; };
	void SetNounFlashing()	{ NounFlashing = true; };

	void ClearVerbFlashing() { VerbFlashing = false; };
	void ClearNounFlashing() { NounFlashing = false; };

	//
	// Timestep to run programs.
	//

	void Timestep(double simt);

	//
	// Display status.
	//

	void ProgDisplay(char *ProgStr);
	void VerbDisplay(char *VerbStr);
	void NounDisplay(char *NounStr);
	void R1Display(char *RegStr);
	void R2Display(char *RegStr);
	void R3Display(char *RegStr);

	//
	// Keypad interface.
	//

	void KeyRel();
	void VerbPressed();
	void NounPressed();
	void EnterPressed();
	void ClearPressed();
	void ResetPressed();
	void ProgPressed();
	void PlusPressed();
	void MinusPressed();
	void NumberPressed(int n);

	void ProcessKeypress(int mx, int my);
	void RenderLights(SURFHANDLE surf, SURFHANDLE lights);
	void RenderData(SURFHANDLE surf, SURFHANDLE digits);

	//
	// External event inputs.
	//

	void Liftoff();

	//
	// Helper functions.
	//

	bool KBCheck();
	void SetR1(int val);
	void SetR2(int val);
	void SetR3(int val);
	void SetR1Octal(int val);
	void SetR2Octal(int val);
	void SetR3Octal(int val);
	void DisplayR1Octal() { R1Decimal = false; };
	void DisplayR2Octal() { R2Decimal = false; };
	void DisplayR3Octal() { R3Decimal = false; };
	void SetR1Format(char *fmt) { strncpy(R1Format, fmt, 7); };
	void SetR2Format(char *fmt) { strncpy(R2Format, fmt, 7); };
	void SetR3Format(char *fmt) { strncpy(R3Format, fmt, 7); };
	void SetProg(int val);
	void SetVerb(int val);
	void SetNoun(int val);
	void BlankAll();
	void BlankData();
	void UnBlankAll();
	void LightsOff();

	void BlankR1();
	void BlankR2();
	void BlankR3();

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:

	void DataEntryR3();
	void DataEntryR2();

	//
	// Lights.
	//

	bool UplinkLight;
	bool NoAttLight;
	bool StbyLight;
	bool KbRelLight;
	bool OprErrLight;
	bool TempLight;
	bool GimbalLockLight;
	bool ProgLight;
	bool RestartLight;
	bool TrackerLight;

	//
	// Keyboard state.
	//

	bool KbInUse;

	//
	// Current program state.
	//

	int CurrentVerb;
	int CurrentNoun;

	//
	// Internal variables.
	//

	int Prog;
	bool ProgBlanked;
	bool ProgFlashing;
	int Verb;
	bool VerbBlanked;
	bool VerbFlashing;
	int Noun;
	bool NounBlanked;
	bool NounFlashing;
	int R1;
	bool R1Blanked;
	bool R1Flashing;
	bool R1Decimal;
	char R1Format[7];
	int R2;
	bool R2Blanked;
	bool R2Flashing;
	bool R2Decimal;
	char R2Format[7];
	int R3;
	bool R3Blanked;
	bool R3Flashing;
	bool R3Decimal;
	char R3Format[7];

	//
	// Data entry status.
	//

	bool EnteringVerb;
	bool EnteringNoun;
	bool EnteringOctal;

	int	EnteringData;
	int EnterCount;

	bool EnterPositive;

	//
	// FlashOn tracks whether flashing displays should currently be shown or
	// blanked out. LastFlashTime tracks when the state was updated.
	//

	bool FlashOn;
	double LastFlashTime;

	void TwoDigitDisplay(char *Str, int val, bool Blanked, bool Flashing);
	void FiveDigitDisplay(char *Str, int val, bool Blanked, bool Flashing, bool Decimal, char *Format);

	char TwoDigitEntry[2];
	char FiveDigitEntry[6];

	int	EnterPos;
	int EnterVal;

	//
	// AGC we're connected to.
	//

	ApolloGuidance &agc;

	//
	// Sound library.
	//

	SoundLib &soundlib;
	Sound Sclick;

	//
	// Local helper functions.
	//

	void StartTwoDigitEntry();
	void StartFiveDigitEntry(bool octal);
	void UpdateTwoDigitEntry(int n);
	void UpdateFiveDigitEntry(int n);
	void ReleaseKeyboard();
	void KeyClick();


	void DSKYLightBlt(SURFHANDLE surf, SURFHANDLE lights, int dstx, int dsty, bool lit);
	void RenderTwoDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str);
	void RenderSixDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str);
};

//
// Strings for state saving.
//

#define DSKY_START_STRING	"DSKY_BEGIN"
#define DSKY_END_STRING		"DSKY_END"


