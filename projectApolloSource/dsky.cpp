/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Saturn DSKY

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
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

#include "soundlib.h"
#include "nasspsound.h"

#include "apolloguidance.h"
#include "dsky.h"

static char RegFormat[7] = "XXXXXX";

DSKY::DSKY(SoundLib &s, ApolloGuidance &computer) : soundlib(s), agc(computer)

{
	Reset();
}

void DSKY::Reset()

{
	LightsOff();

	KbInUse = false;

	Prog = 0;
	Verb = 0;
	Noun = 0;
	R1 = 0;
	R2 = 0;
	R3 = 0;

	UnBlankAll();

	ProgFlashing = false;
	VerbFlashing = false;
	NounFlashing = false;
	R1Flashing = false;
	R2Flashing = false;
	R3Flashing = false;

	EnteringVerb = false;
	EnteringNoun = false;
	EnteringData = 0;
	EnterPos = 0;
	EnteringOctal = false;

	R1Decimal = true;
	R2Decimal = true;
	R3Decimal = true;

	SetR1Format(RegFormat);
	SetR2Format(RegFormat);
	SetR3Format(RegFormat);

	FlashOn = true;

	//
	// LastFlashTime needs to be set a long way back in the past so
	// that it will flash the digits prior to launch.
	//

	LastFlashTime = (-100000);

	strncpy (TwoDigitEntry, "  ", 2);
	strncpy (FiveDigitEntry, "      ", 6);
}

DSKY::~DSKY()

{
	//
	// Nothing for now.
	//
}

void DSKY::Init()

{
	Reset();
	soundlib.LoadSound(Sclick, CLICK_SOUND);
}

void DSKY::Timestep(double simt)

{
	//
	// Flash counter. For simplicity we'll always update
	// this even though nothing may be flashing.
	//

	if (simt > (LastFlashTime + 0.5)) {
		LastFlashTime = simt;
		FlashOn = !FlashOn;
	}
}

void DSKY::TwoDigitDisplay(char *Str, int val, bool Blanked, bool Flashing)

{
	if (val > 99)
		val = 99;

	if (Blanked || (Flashing && !FlashOn)) {
		strcpy(Str, "  ");
	}
	else {
		Str[0] = '0' + (val / 10);
		Str[1] = '0' + (val % 10);
	}
}

void DSKY::FiveDigitDisplay(char *Str, int val, bool Blanked, bool Flashing, bool Decimal, char *Format)

{
	int	i;
	int divisor, dividestep, rval;

	if (Blanked || (Flashing && !FlashOn)) {
		strcpy(Str, "      ");
		return;
	}

	if (Decimal) {
		if (val >= 0)
			Str[0] = '+';
		else {
			Str[0] = '-';
			val = (-val);
		}
	}
	else
		Str[0] = ' ';

	//
	// Set up decimal and octal divisors.
	//

	if (Decimal) {
		divisor = 10000;
		dividestep = 10;
	}
	else {
		divisor = 010000;
		dividestep = 8;
	}

	//
	// Limit maximum value.
	//

	if (val >= (divisor * dividestep)) {
		val = (divisor * dividestep) - 1;
	}

	for (i = 1; i < 6; i++) {
		rval = (val / divisor);
		Str[i] = '0' + rval;
		val -= divisor * rval;
		divisor /= dividestep;
	}

	//
	// Update string for formatting, so we can blank out spaces as
	// appropriate.
	//

	for (i = 0; i < 6; i++) {
		if (Format[i] == ' ')
			Str[i] = ' ';
	}
}

void DSKY::StartTwoDigitEntry()

{
	TwoDigitEntry[0] = TwoDigitEntry[1] = ' ';
	EnterPos = 0;
	EnterVal = 0;
}

void DSKY::StartFiveDigitEntry(bool octal)

{
	for (int i = 0; i < 6; i++)
		FiveDigitEntry[i] = ' ';

	EnterPos = 1;
	EnterVal = 0;

	EnteringOctal = octal;

	//
	// Octal entry is always a positive number.
	//

	if (octal)
		EnterPositive = true;
}

void DSKY::UpdateFiveDigitEntry(int n)

{
	if (!EnterPos) {
		//
		// No octal support yet.
		//
		StartFiveDigitEntry(true);
	}

	FiveDigitEntry[EnterPos] = '0' + n;

	if (EnteringOctal) {
		if (n > 7) {
			LightOprErr();
			return;
		}
		EnterVal = EnterVal * 8 + n;
	}
	else {
		EnterVal = (EnterVal * 10) + n;
	}

	EnterPos++;

	if (EnterPos > 5) {

		if (!EnterPositive)
			EnterVal = (-EnterVal);

		switch(EnteringData) {

		case 1:
			R1 = EnterVal;
			R1Blanked = false;
			break;

		case 2:
			R2 = EnterVal;
			R2Blanked = false;
			break;

		case 3:
			R3 = EnterVal;
			R3Blanked = false;
			break;

		}

		EnteringData++;
		ReleaseKeyboard();

		EnterPos = 0;
		strncpy (FiveDigitEntry, "      ", 6);

		if (EnteringData > EnterCount) {
			EnteringData = 0;
		}
	}
}

void DSKY::UpdateTwoDigitEntry(int n)

{
	if (EnterPos > 1) {
		LightOprErr();
		return;
	}

	TwoDigitEntry[EnterPos] = '0' + n;

	if (!EnterPos) {
		EnterVal = n * 10;
	}
	else {
		EnterVal += n;

		if (EnteringVerb) {
			Verb = EnterVal;
			EnteringVerb = false;
		}

		if (EnteringNoun) {
			Noun = EnterVal;
			EnteringNoun = false;
		}

		ReleaseKeyboard();
	}

	EnterPos++;
}


//
// Keyboard interface.
//

void DSKY::ReleaseKeyboard()

{
	KbInUse = false;
	KbRelLight = false;
}

void DSKY::KeyClick()

{
	Sclick.play(NOLOOP, 255);
}

void DSKY::KeyRel()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	ReleaseKeyboard();
}

void DSKY::VerbPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	if (EnteringNoun) {
		LightOprErr();
		return;
	}

	KeyClick();

	VerbFlashing = false;
	VerbBlanked = false;

	StartTwoDigitEntry();

	KbInUse = true;
	EnteringVerb = true;
}

void DSKY::NounPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	if (EnteringVerb) {
		LightOprErr();
		return;
	}

	KeyClick();

	NounFlashing = false;
	NounBlanked = false;

	StartTwoDigitEntry();

	KbInUse = true;
	EnteringNoun = true;

}

void DSKY::DataEntryR2()

{
	EnteringData = 2;
	EnterCount = 2;
	EnterPos = 0;
	R2Blanked = true;
}

void DSKY::DataEntryR3()

{
	EnteringData = 3;
	EnterCount = 3;
	EnterPos = 0;
	R3Blanked = true;
}

void DSKY::EnterPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	//
	// Must complete entering the data before pressing
	// ENTER.
	//

	if (EnteringVerb || EnteringNoun) {
		LightOprErr();
		return;
	}

	ReleaseKeyboard();

	VerbFlashing = false;
	NounFlashing = false;

	switch (Verb) {

	case 21:
		EnteringData = 1;
		EnterCount = 1;
		EnterPos = 0;
		R1Blanked = true;
		break;

	case 1:
	case 11:
		if (Noun != 2) {
			LightOprErr();
			return;
		}

		VerbFlashing = true;
		NounFlashing = true;

		R2Decimal = false;
		R1Decimal = false;
		R1Blanked = false;

		//
		// And fall through to get R2.
		//

	case 22:
		DataEntryR2();
		break;

	case 23:
		DataEntryR3();
		break;

	case 24:
		EnteringData = 1;
		EnterCount = 2;
		EnterPos = 0;
		R1Blanked = true;
		R2Blanked = true;
		break;

	case 25:
		EnteringData = 1;
		EnterCount = 3;
		EnterPos = 0;
		R1Blanked = true;
		R2Blanked = true;
		R3Blanked = true;
		break;

	//
	// 33: Proceed without data.
	//

	case 33:
		agc.ProceedNoData();
		break;

	//
	// 34: terminate.
	//

	case 34:
		agc.TerminateProgram();
		break;

	default:
		agc.VerbNounEntered(Verb, Noun);
		break;
	}
}

void DSKY::ClearPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	if (EnteringData) {
		EnterPos = 0;
		strncpy (FiveDigitEntry, "      ", 6);
	}
	else {
		LightOprErr();
	}
}

void DSKY::PlusPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	if (EnteringData && !EnterPos) {
		EnterPositive = true;
		StartFiveDigitEntry(false);
		FiveDigitEntry[0] = '+';
	}
	else {
		LightOprErr();
	}
}

void DSKY::MinusPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	if (EnteringData && !EnterPos) {
		EnterPositive = false;
		StartFiveDigitEntry(false);
		FiveDigitEntry[0] = '-';
	}
	else {
		LightOprErr();
	}
}

void DSKY::ProgPressed()

{
	if (!agc.OutOfReset())
		return;

	KeyClick();

	//
	// If AGC is in standby mode, then start it up.
	//

	if (agc.OnStandby()) {
		agc.Startup();
		return;
	}

	//
	// Else if program 6 is running, shut it down to
	// standby mode.
	//

	if (Prog == 06) {
		agc.GoStandby();
		return;
	}

	//
	// Now we have a value in R3, we can tell the agc that
	// verb 1 or 11 is running.
	//

	if (Verb == 1 || Verb == 11) {
		agc.VerbNounEntered(Verb, Noun);
	}

	//
	// Otherwise tell the AGC that it's been pressed.
	//

	agc.ProgPressed(R1, R2, R3);

	//
	// For verb 1, let us enter another address.
	//

	switch (Verb) {
	case 1:
		DataEntryR2();
		break;

	case 11:
		VerbFlashing = false;
		NounFlashing = false;
		break;
	}
}

void DSKY::ResetPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	agc.RSetPressed();

	KeyClick();

	LightsOff();
}

void DSKY::NumberPressed(int n)

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	if (EnteringOctal && n > 7) {
		LightOprErr();
		return;
	}

	if (EnteringVerb || EnteringNoun) {
		UpdateTwoDigitEntry(n);
		return;
	}

	if (EnteringData) {
		UpdateFiveDigitEntry(n);
		return;
	}

	LightOprErr();
}

void DSKY::DSKYLightBlt(SURFHANDLE surf, SURFHANDLE lights, int dstx, int dsty, bool lit)

{
	if (lit) {
		oapiBlt(surf, lights, dstx, dsty, dstx + 81, dsty + 0, 35, 16);
	}
	else {
		oapiBlt(surf, lights, dstx, dsty, dstx + 0, dsty + 0, 35, 16);
	}
}

void DSKY::RenderLights(SURFHANDLE surf, SURFHANDLE lights)

{
	//
	// Check the lights.
	//

	DSKYLightBlt(surf, lights, 0, 0, UplinkLit());
	DSKYLightBlt(surf, lights, 0, 18, NoAttLit());
	DSKYLightBlt(surf, lights, 0, 37, StbyLit());
	DSKYLightBlt(surf, lights, 0, 55, KbRelLit());
	DSKYLightBlt(surf, lights, 0, 72, OprErrLit());

	DSKYLightBlt(surf, lights, 39, 0, TempLit());
	DSKYLightBlt(surf, lights, 39, 18, GimbalLockLit());
	DSKYLightBlt(surf, lights, 39, 37, ProgLit());
	DSKYLightBlt(surf, lights, 39, 55, RestartLit());
	DSKYLightBlt(surf, lights, 39, 72, TrackerLit());
}


//
// Process a keypress based on the X and Y coords.
//

void DSKY::ProcessKeypress(int mx, int my)

{
	if (mx > 0 && mx < 26){
		if (my > 13 && my < 39){
			VerbPressed();
		}
		if (my > 45 && my < 69){
			NounPressed();
		}
	}else if (mx > 172 && mx < 200){
		if (my > 13 && my < 39){
			EnterPressed();
		}
		if (my > 45 && my < 69){
			ResetPressed();
		}
	}else if (mx > 27 && mx < 54){
		if (my > 2 && my < 25){
			PlusPressed();
		}
		if (my > 30 && my < 55){
			MinusPressed();
		}
		if (my > 60 && my < 85){
			NumberPressed(0);
		}
	}else if (mx > 57 && mx < 83){
		if (my > 2 && my < 25){
			NumberPressed(7);
		}
		if (my > 30 && my < 55){
			NumberPressed(4);
		}
		if (my > 60 && my < 85){
			NumberPressed(1);
		}
	}else if (mx > 87 && mx < 112){
		if (my > 2 && my < 25){
			NumberPressed(8);
		}
		if (my > 30 && my < 55){
			NumberPressed(5);
		}
		if (my > 60 && my < 85){
			NumberPressed(2);
		}
	}else if (mx > 117 && mx < 141){
		if (my > 2 && my < 25){
			NumberPressed(9);
		}
		if (my > 30 && my < 55){
			NumberPressed(6);
		}
		if (my > 60 && my < 85){
			NumberPressed(3);
		}
	}else if (mx > 146 && mx < 169){
		if (my > 2 && my < 25){
			ClearPressed();
		}
		if (my > 30 && my < 55){
			ProgPressed();
		}
		if (my > 60 && my < 85){
			KeyRel();
		}
	}
}

void DSKY::ProgDisplay(char *ProgStr)

{
	TwoDigitDisplay(ProgStr, Prog, ProgBlanked, ProgFlashing);
}


void DSKY::VerbDisplay(char *VerbStr)

{
	if (EnteringVerb) {
		strncpy(VerbStr, TwoDigitEntry, 2);
		return;
	}

	TwoDigitDisplay(VerbStr, Verb, VerbBlanked, VerbFlashing);
}

void DSKY::NounDisplay(char *NounStr)

{
	if (EnteringNoun) {
		strncpy(NounStr, TwoDigitEntry, 2);
		return;
	}

	TwoDigitDisplay(NounStr, Noun, NounBlanked, NounFlashing);
}

void DSKY::R1Display(char *RegStr)

{
	if (EnteringData == 1) {
		strncpy(RegStr, FiveDigitEntry, 6);
		return;
	}

	FiveDigitDisplay(RegStr, R1, R1Blanked, R1Flashing, R1Decimal, R1Format);
}

void DSKY::R2Display(char *RegStr)

{
	if (EnteringData == 2) {
		strncpy(RegStr, FiveDigitEntry, 6);
		return;
	}

	FiveDigitDisplay(RegStr, R2, R2Blanked, R2Flashing, R2Decimal, R2Format);
}

void DSKY::R3Display(char *RegStr)

{
	if (EnteringData == 3) {
		strncpy(RegStr, FiveDigitEntry, 6);
		return;
	}

	FiveDigitDisplay(RegStr, R3, R3Blanked, R3Flashing, R3Decimal, R3Format);
}

//
// Attempt to set a value into the register, and light the KBD REL
// light if the keyboard is in use.
//

bool DSKY::KBCheck()

{
	if (KbInUse) {
		KbRelLight = true;
		return false;
	}

	return true;
}

void DSKY::SetR1(int val)

{
	if (KBCheck()) {
		R1 = val;
		R1Blanked = false;
		SetR1Format(RegFormat);
		R1Decimal = true;
	}
}

void DSKY::SetR2(int val)

{
	if (KBCheck()) {
		R2 = val;
		R2Blanked = false;
		SetR2Format(RegFormat);
		R2Decimal = true;
	}
}

void DSKY::SetR3(int val)

{
	if (KBCheck()) {
		R3 = val;
		R3Blanked = false;
		SetR3Format(RegFormat);
		R3Decimal = true;
	}
}

void DSKY::SetR1Octal(int val)

{
	SetR1(val);
	R1Decimal = false;
}

void DSKY::SetR2Octal(int val)

{
	SetR2(val);
	R2Decimal = false;
}

void DSKY::SetR3Octal(int val)

{
	SetR3(val);
	R3Decimal = false;
}

void DSKY::SetProg(int val)

{
	if (KBCheck()) {
		Prog = val;
		ProgBlanked = false;
	}
}

void DSKY::SetVerb(int val)

{
	if (KBCheck()) {
		Verb = val;
		VerbBlanked = false;
	}
}

void DSKY::SetNoun(int val)

{
	if (KBCheck()) {
		Noun = val;
		NounBlanked = false;
	}
}

void DSKY::UnBlankAll()

{
	ProgBlanked = false;
	VerbBlanked = false;
	NounBlanked = false;
	R1Blanked = false;
	R2Blanked = false;
	R3Blanked = false;
}

void DSKY::BlankAll()

{
	ProgBlanked = true;
	VerbBlanked = true;
	NounBlanked = true;
	R1Blanked = true;
	R2Blanked = true;
	R3Blanked = true;
}

void DSKY::BlankData()

{
	R1Blanked = true;
	R2Blanked = true;
	R3Blanked = true;
}

void DSKY::BlankR1()

{
	if (KBCheck()) {
		R1Blanked = true;
	}
}

void DSKY::BlankR2()

{
	if (KBCheck()) {
		R2Blanked = true;
	}
}

void DSKY::BlankR3()

{
	if (KBCheck()) {
		R3Blanked = true;
	}
}

void DSKY::LightsOff()

{
	UplinkLight = false;
	NoAttLight = false;
	StbyLight = false;
	KbRelLight = false;
	OprErrLight = false;
	TempLight = false;
	GimbalLockLight = false;
	ProgLight = false;
	RestartLight = false;
	TrackerLight = false;
}

void DSKY::RenderTwoDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str)

{
	int Curdigit;

	if (Str[0] != ' ') {
		Curdigit = Str[0] - '0';
		oapiBlt(surf,digits,dstx,dsty,16*Curdigit,0,16,19);
	}

	if (Str[1] != ' ') {
		Curdigit = Str[1] - '0';
		oapiBlt(surf,digits,dstx+16,dsty,16*Curdigit,0,16,19);
	}
}

void DSKY::RenderSixDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str)

{
	int	Curdigit;
	int i;

	if (Str[0] == '-') {
		oapiBlt(surf,digits,dstx,dsty,161,0,10,19);
	}
	else if (Str[0] == '+') {
		oapiBlt(surf,digits,dstx,dsty,174,0,12,19);
	}

	for (i = 1; i < 6; i++) {
		if (Str[i] != ' ') {
			Curdigit = Str[i] - '0';
			oapiBlt(surf, digits, dstx + (16*i), dsty, 16*Curdigit, 0, 16,19);
		}
		else {
//			oapiBlt(surf, digits, dstx + (10*i), dsty, 440, 6, 10, 15);
		}
	}
}

void DSKY::RenderData(SURFHANDLE surf, SURFHANDLE digits)

{
		char DSKYString[10];

		ProgDisplay(DSKYString);
		RenderTwoDigitDisplay(surf, digits, 67, 18, DSKYString);
		VerbDisplay(DSKYString);
		RenderTwoDigitDisplay(surf, digits, 8, 52, DSKYString);
		NounDisplay(DSKYString);
		RenderTwoDigitDisplay(surf, digits, 67, 52, DSKYString);

		//
		// Register contents.
		//

		R1Display(DSKYString);
		RenderSixDigitDisplay(surf, digits, 3, 85, DSKYString);
		R2Display(DSKYString);
		RenderSixDigitDisplay(surf, digits, 3, 119, DSKYString);
		R3Display(DSKYString);
		RenderSixDigitDisplay(surf, digits, 3, 153, DSKYString);
}

typedef union

{
	struct {
		unsigned R1Blanked:1;
		unsigned R1Flashing:1;
		unsigned R1Decimal:1;
		unsigned R2Blanked:1;
		unsigned R2Flashing:1;
		unsigned R2Decimal:1;
		unsigned R3Blanked:1;
		unsigned R3Flashing:1;
		unsigned R3Decimal:1;
		unsigned VerbBlanked:1;
		unsigned VerbFlashing:1;
		unsigned NounBlanked:1;
		unsigned NounFlashing:1;
		unsigned ProgBlanked:1;
		unsigned ProgFlashing:1;
		unsigned UplinkLight:1;
		unsigned NoAttLight:1;
		unsigned StbyLight:1;
		unsigned KbRelLight:1;
		unsigned OprErrLight:1;
		unsigned TempLight:1;
		unsigned GimbalLockLight:1;
		unsigned ProgLight:1;
		unsigned RestartLight:1;
		unsigned TrackerLight:1;
		unsigned KbInUse:1;
		unsigned EnteringVerb:1;
		unsigned EnteringNoun:1;
		unsigned EnteringOctal:1;
	} u;
	unsigned long word;
} DSKYState;

void DSKY::SaveState(FILEHANDLE scn)

{
	char	str[10];

	oapiWriteLine(scn, DSKY_START_STRING);
	oapiWriteScenario_int (scn, "PROG", Prog);
	oapiWriteScenario_int (scn, "VERB", Verb);
	oapiWriteScenario_int (scn, "NOUN", Noun);
	oapiWriteScenario_int (scn, "R1", R1);
	oapiWriteScenario_int (scn, "R2", R2);
	oapiWriteScenario_int (scn, "R3", R3);
	oapiWriteScenario_int (scn, "EPOS", EnterPos);
	oapiWriteScenario_int (scn, "EVAL", EnterVal);

	memset(str, 0, 10);

	strncpy(str, TwoDigitEntry, 2);
	oapiWriteScenario_string (scn, "E2", str);
	strncpy(str, FiveDigitEntry, 6);
	oapiWriteScenario_string (scn, "E5", str);

	oapiWriteScenario_string(scn, "R1FMT", R1Format);
	oapiWriteScenario_string(scn, "R2FMT", R2Format);
	oapiWriteScenario_string(scn, "R3FMT", R3Format);

	//
	// Copy internal state to the structure.
	//

	DSKYState state;

	state.word = 0;
	state.u.R1Blanked = R1Blanked;
	state.u.R1Flashing = R1Flashing;
	state.u.R1Decimal = R1Decimal;
	state.u.R2Blanked = R2Blanked;
	state.u.R2Flashing = R2Flashing;
	state.u.R2Decimal = R2Decimal;
	state.u.R3Blanked = R3Blanked;
	state.u.R3Flashing = R3Flashing;
	state.u.R3Decimal = R3Decimal;
	state.u.VerbBlanked = VerbBlanked;
	state.u.VerbFlashing = VerbFlashing;
	state.u.NounBlanked = NounBlanked;
	state.u.NounFlashing = NounFlashing;
	state.u.ProgBlanked = ProgBlanked;
	state.u.ProgFlashing = ProgFlashing;

	state.u.UplinkLight = UplinkLight;
	state.u.NoAttLight = NoAttLight;
	state.u.StbyLight = StbyLight;
	state.u.KbRelLight = KbRelLight;
	state.u.OprErrLight = OprErrLight;
	state.u.TempLight = TempLight;
	state.u.GimbalLockLight = GimbalLockLight;
	state.u.ProgLight = ProgLight;
	state.u.RestartLight = RestartLight;
	state.u.TrackerLight = TrackerLight;

	state.u.KbInUse = KbInUse;

	state.u.EnteringNoun = EnteringNoun;
	state.u.EnteringVerb = EnteringVerb;
	state.u.EnteringOctal = EnteringOctal;

	oapiWriteScenario_int (scn, "STATE", state.word);

	oapiWriteLine(scn, DSKY_END_STRING);
}


void DSKY::LoadState(FILEHANDLE scn)

{
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, DSKY_END_STRING, sizeof(DSKY_END_STRING)))
			return;
		if (!strnicmp (line, "PROG", 4)) {
			sscanf (line+4, "%d", &Prog);
		}
		else if (!strnicmp (line, "VERB", 4)) {
			sscanf (line+4, "%d", &Verb);
		}
		else if (!strnicmp (line, "NOUN", 4)) {
			sscanf (line+4, "%d", &Noun);
		}
		else if (!strnicmp (line, "R1FMT", 5)) {
			strncpy (R1Format, line + 6, 6);
		}
		else if (!strnicmp (line, "R2FMT", 5)) {
			strncpy (R2Format, line + 6, 6);
		}
		else if (!strnicmp (line, "R3FMT", 5)) {
			strncpy (R3Format, line + 6, 6);
		}
		else if (!strnicmp (line, "R1", 2)) {
			sscanf (line+2, "%d", &R1);
		}
		else if (!strnicmp (line, "R2", 2)) {
			sscanf (line+2, "%d", &R2);
		}
		else if (!strnicmp (line, "R3", 2)) {
			sscanf (line+2, "%d", &R3);
		}
		else if (!strnicmp (line, "EPOS", 4)) {
			sscanf (line+4, "%d", &EnterPos);
		}
		else if (!strnicmp (line, "EVAL", 4)) {
			sscanf (line+4, "%d", &EnterVal);
		}
		else if (!strnicmp (line, "E2", 2)) {
			strncpy (TwoDigitEntry, line + 3, 2);
		}
		else if (!strnicmp (line, "E5", 2)) {
			strncpy (FiveDigitEntry, line + 3, 6);
		}
		else if (!strnicmp (line, "STATE", 5)) {
			DSKYState state;
			sscanf (line+5, "%d", &state.word);

			R1Blanked = state.u.R1Blanked;
			R1Flashing = state.u.R1Flashing;
			R1Decimal = state.u.R1Decimal;
			R2Blanked = state.u.R2Blanked;
			R2Flashing = state.u.R2Flashing;
			R2Decimal = state.u.R2Decimal;
			R3Blanked = state.u.R3Blanked;
			R3Flashing = state.u.R3Flashing;
			R3Decimal = state.u.R3Decimal;
			VerbBlanked = state.u.VerbBlanked;
			VerbFlashing = state.u.VerbFlashing;
			NounBlanked = state.u.NounBlanked;
			NounFlashing = state.u.NounFlashing;
			ProgBlanked = state.u.ProgBlanked;
			ProgFlashing = state.u.ProgFlashing;

			UplinkLight = state.u.UplinkLight;
			NoAttLight = state.u.NoAttLight;
			StbyLight = state.u.StbyLight;
			KbRelLight = state.u.KbRelLight;
			OprErrLight = state.u.OprErrLight;
			TempLight = state.u.TempLight;
			GimbalLockLight = state.u.GimbalLockLight;
			ProgLight = state.u.ProgLight;
			RestartLight = state.u.RestartLight;
			TrackerLight = state.u.TrackerLight;

			KbInUse = state.u.KbInUse;

			EnteringVerb = state.u.EnteringVerb;
			EnteringNoun = state.u.EnteringNoun;
			EnteringOctal = state.u.EnteringOctal;

		}
	}
}
