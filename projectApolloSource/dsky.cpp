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
  *	Revision 1.9  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.8  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.7  2005/08/09 13:05:07  spacex15
  *	fixed some initialization bugs in dsky and apolloguidance
  *	
  *	Revision 1.6  2005/08/09 02:28:25  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.5  2005/08/08 22:32:49  movieman523
  *	First steps towards reimplementing the DSKY interface to use the same I/O channels as the real AGC/DSKY interface.
  *	
  *	Revision 1.4  2005/04/16 00:14:10  tschachim
  *	fixed dsky keyboard and g&n panel lights
  *	
  *	Revision 1.3  2005/04/10 19:25:33  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.2  2005/04/10 16:05:43  flydba
  *	*** empty log message ***
  *	
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
#include "ioChannels.h"

#include "nasspdefs.h"

static char TwoSpace[] = "  ";
static char SixSpace[] = "      ";

DSKY::DSKY(SoundLib &s, ApolloGuidance &computer, int IOChannel) : soundlib(s), agc(computer)

{
	Reset();
	KeyCodeIOChannel = IOChannel;
}

void DSKY::Reset()

{
	CompActy = false;
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
	VelLight = false;
	AltLight = false;

	strcpy (Prog, TwoSpace);
	strcpy (Verb, TwoSpace);
	strcpy (Noun, TwoSpace);
	strcpy (R1, SixSpace);
	strcpy (R2, SixSpace);
	strcpy (R3, SixSpace);

	VerbFlashing = false;
	NounFlashing = false;

	FlashOn = true;

	//
	// LastFlashTime needs to be set a long way back in the past so
	// that it will flash the digits prior to launch.
	//

	LastFlashTime = MINUS_INFINITY;
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

void DSKY::KeyClick()

{
	Sclick.play(NOLOOP, 255);
}

void DSKY::SendKeyCode(int val)

{
	agc.SetInputChannel(KeyCodeIOChannel, val);
}

void DSKY::KeyRel()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();
	SendKeyCode(25);
}

void DSKY::VerbPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	VerbFlashing = false;
	SendKeyCode(17);
}

void DSKY::NounPressed()

{	
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();

	NounFlashing = false;
	SendKeyCode(31);
}

void DSKY::EnterPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();
	SendKeyCode(28);
}

void DSKY::ClearPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();
	SendKeyCode(30);
}

void DSKY::PlusPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();
	SendKeyCode(26);
}

void DSKY::MinusPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();
	SendKeyCode(27);
}

void DSKY::ProgPressed()

{
	if (!agc.OutOfReset())
		return;

	KeyClick();

	agc.SetInputChannelBit(032, 14, true);
}

void DSKY::ProgReleased()

{
	if (!agc.OutOfReset())
		return;

	agc.SetInputChannelBit(032, 14, false);
}

void DSKY::ResetPressed()

{
	if (!agc.OutOfReset() || agc.OnStandby())
		return;

	KeyClick();
	SendKeyCode(18);
}

void DSKY::NumberPressed(int n)

{
	KeyClick();
	if (!n)
		SendKeyCode(16);
	else
		SendKeyCode(n);
}

//
// Convert from AGC output to character codes.
//

char DSKY::ValueChar(unsigned val)

{
	switch (val) {
	case 21:
		return '0';

	case 3:
		return '1';

	case 25:
		return '2';

	case 27:
		return '3';

	case 15:
		return '4';

	case 30:
		return '5';
	
	case 28:
		return '6';

	case 19:
		return '7';

	case 29:
		return '8';

	case 31:
		return '9';
	}
	return ' ';
}

void DSKY::ProcessChannel10(int val)

{
	ChannelValue10 out_val;
	char	C1, C2;

	out_val.Value = val;

	C1 = ValueChar(out_val.Bits.c);
	C2 = ValueChar(out_val.Bits.d);

	switch (out_val.Bits.a) {

	case 11:
		Prog[0] = C1;
		Prog[1] = C2;
		break;

	case 10:
		Verb[0] = C1;
		Verb[1] = C2;
		break;

	case 9:
		Noun[0] = C1;
		Noun[1] = C2;
		break;
	
	case 8:
		R1[1] = C2;
		break;

	case 7:
		R1[2] = C1;
		R1[3] = C2;
		if (out_val.Bits.b) {
			R1[0] = '+';
		}
		else if (R1[0] == '+') {
			R1[0] = ' ';
		}
		break;

	case 6:
		R1[4] = C1;
		R1[5] = C2;
		if (out_val.Bits.b) {
			R1[0] = '-';
		}
		else if (R1[0] == '-') {
			R1[0] = ' ';
		}
		break;

	case 5:
		R2[1] = C1;
		R2[2] = C2;
		if (out_val.Bits.b) {
			R2[0] = '+';
		}
		else if (R2[0] == '+') {
			R2[0] = ' ';
		}
		break;

	case 4:
		R2[3] = C1;
		R2[4] = C2;
		if (out_val.Bits.b) {
			R2[0] = '-';
		}
		else if (R2[0] == '-') {
			R2[0] = ' ';
		}
		break;

	case 3:
		R2[5] = C1;
		R3[1] = C2;
		break;

	case 2:
		R3[2] = C1;
		R3[3] = C2;
		if (out_val.Bits.b) {
			R3[0] = '+';
		}
		else if (R3[0] == '+') {
			R3[0] = ' ';
		}
		break;

	case 1:
		R3[4] = C1;
		R3[5] = C2;
		if (out_val.Bits.b) {
			R3[0] = '-';
		}
		else if (R3[0] == '-') {
			R3[0] = ' ';
		}
		break;

	// 12 - set light states.
	case 12:
		SetNoAtt((out_val.Value & (1 << 3)) != 0);
		SetGimbalLock((out_val.Value & (1 << 5)) != 0);
		SetTracker((out_val.Value & (1 << 7)) != 0);
		SetProg((out_val.Value & (1 << 8)) != 0);
		break;
	}
}

void DSKY::DSKYLightBlt(SURFHANDLE surf, SURFHANDLE lights, int dstx, int dsty, bool lit)

{
	if (lit) {
		oapiBlt(surf, lights, dstx, dsty, dstx + 101, dsty + 0, 49, 23);
	}
	else {
		oapiBlt(surf, lights, dstx, dsty, dstx + 0, dsty + 0, 49, 23);
	}
}

void DSKY::RenderLights(SURFHANDLE surf, SURFHANDLE lights)

{
	//
	// Check the lights.
	//

	DSKYLightBlt(surf, lights, 0, 0,  UplinkLit());
	DSKYLightBlt(surf, lights, 0, 25, NoAttLit());
	DSKYLightBlt(surf, lights, 0, 49, StbyLit());
	DSKYLightBlt(surf, lights, 0, 73, KbRelLit());
	DSKYLightBlt(surf, lights, 0, 97, OprErrLit());

	DSKYLightBlt(surf, lights, 52, 0,  TempLit());
	DSKYLightBlt(surf, lights, 52, 25, GimbalLockLit());
	DSKYLightBlt(surf, lights, 52, 49, ProgLit());
	DSKYLightBlt(surf, lights, 52, 73, RestartLit());
	DSKYLightBlt(surf, lights, 52, 97, TrackerLit());

	//
	// Need to add the Alt and Vel lights here for the LEM.
	//
}


//
// Process a keypress based on the X and Y coords.
//

void DSKY::ProcessKeyPress(int mx, int my)

{
	bool KeyDown_Verb;
	bool KeyDown_Noun;
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
	bool KeyDown_Prog;
	bool KeyDown_KeyRel;
	bool KeyDown_Enter;
	bool KeyDown_Reset;

	if (mx > 2 && mx < 39) {
		if (my > 21 && my < 59) {
			KeyDown_Verb = true;
			VerbPressed();
		}

		if (my > 61 && my < 99) {
			KeyDown_Noun = true;
			NounPressed();
		}
	}

	if (mx > 2+1*41 && mx < 39+1*41) {
		if (my > 1 && my < 39) {
			KeyDown_Plus = true;
			PlusPressed();
		}
		if (my > 41 && my < 79) {
			KeyDown_Minus = true;
			MinusPressed();
		}
		if (my > 81 && my < 119) {
			KeyDown_0 = true;
			NumberPressed(0);
		}
	}

	if (mx > 2+2*41 && mx < 39+2*41) {
		if (my > 1 && my < 39) {
			KeyDown_7 = true;
			NumberPressed(7);
		}
		if (my > 41 && my < 79) {
			KeyDown_4 = true;
			NumberPressed(4);
		}
		if (my > 81 && my < 119) {
			KeyDown_1 = true;
			NumberPressed(1);
		}
	}

	if (mx > 2+3*41 && mx < 39+3*41) {
		if (my > 1 && my < 39) {
			KeyDown_8 = true;
			NumberPressed(8);
		}
		if (my > 41 && my < 79) {
			KeyDown_5 = true;
			NumberPressed(5);
		}
		if (my > 81 && my < 119) {
			KeyDown_2 = true;
			NumberPressed(2);
		}
	}

	if (mx > 2+4*41 && mx < 39+4*41) {
		if (my > 1 && my < 39) {
			KeyDown_9 = true;
			NumberPressed(9);
		}
		if (my > 41 && my < 79) {
			KeyDown_6 = true;
			NumberPressed(6);
		}
		if (my > 81 && my < 119) {
			KeyDown_3 = true;
			NumberPressed(3);
		}
	}

	if (mx > 2+5*41 && mx < 39+5*41) {
		if (my > 1 && my < 39) {
			KeyDown_Clear = true;
			ClearPressed();
		}
		if (my > 41 && my < 79) {
			KeyDown_Prog = true;
			ProgPressed();
		}
		if (my > 81 && my < 119) {
			KeyDown_KeyRel = true;
			KeyRel();
		}
	}

	if (mx > 2+6*41 && mx < 39+6*41) {
		if (my > 21 && my < 59) {
			KeyDown_Enter= true;
			EnterPressed();
		}

		if (my > 61 && my < 99) {
			KeyDown_Reset = true;
			ResetPressed();
		}
	}

/*	if (mx > 0 && mx < 26){
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
	}*/
}

void DSKY::ProcessKeyRelease(int mx, int my)

{
	if (mx > 2+5*41 && mx < 39+5*41) {
		if (my > 41 && my < 79) {
			ProgReleased();
		}
	}
	else {
		SendKeyCode(0);
	}

#if 0
	// Reset KeyDown-flags
	KeyDown_Verb = false;
	KeyDown_Noun = false;
	KeyDown_Plus = false;
	KeyDown_Minus = false;
	KeyDown_0 = false;
	KeyDown_1 = false;
	KeyDown_2 = false;
	KeyDown_3 = false;
	KeyDown_4 = false;
	KeyDown_5 = false;
	KeyDown_6 = false;
	KeyDown_7 = false;
	KeyDown_8 = false;
	KeyDown_9 = false;
	KeyDown_Clear = false;
	KeyDown_Prog = false;
	KeyDown_KeyRel = false;
	KeyDown_Enter = false;
	KeyDown_Reset = false;
#endif
}

void DSKY::RenderTwoDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str, bool Flash)

{
	int Curdigit;

	if (Flash && !FlashOn)
		return;

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
	if (CompActy) {
		//
		// Do stuff to update Comp Acty light.
		//
	}

	RenderTwoDigitDisplay(surf, digits, 67, 18, Prog, false);
	RenderTwoDigitDisplay(surf, digits, 8, 52, Verb, VerbFlashing);
	RenderTwoDigitDisplay(surf, digits, 67, 52, Noun, NounFlashing);

	//
	// Register contents.
	//

	RenderSixDigitDisplay(surf, digits, 3, 85, R1);
	RenderSixDigitDisplay(surf, digits, 3, 119, R2);
	RenderSixDigitDisplay(surf, digits, 3, 153, R3);
}

typedef union

{
	struct {
		unsigned R1Blanked:1;
		unsigned VelLight:1;
		unsigned R1Decimal:1;
		unsigned R2Blanked:1;
		unsigned AltLight:1;
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
		unsigned CompActy:1;
	} u;
	unsigned long word;
} DSKYState;

void DSKY::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, DSKY_START_STRING);
	oapiWriteScenario_string (scn, "PROG", Prog);
	oapiWriteScenario_string (scn, "VERB", Verb);
	oapiWriteScenario_string (scn, "NOUN", Noun);
	oapiWriteScenario_string (scn, "R1", R1);
	oapiWriteScenario_string (scn, "R2", R2);
	oapiWriteScenario_string (scn, "R3", R3);

	//
	// Copy internal state to the structure.
	//

	DSKYState state;

	state.word = 0;
	state.u.VerbFlashing = VerbFlashing;
	state.u.NounFlashing = NounFlashing;

	state.u.CompActy = CompActy;
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
	state.u.AltLight = AltLight;
	state.u.VelLight = VelLight;

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
			strncpy (Prog, line+5, 2);
		}
		else if (!strnicmp (line, "VERB", 4)) {
			strncpy (Verb, line+5, 2);
		}
		else if (!strnicmp (line, "NOUN", 4)) {
			strncpy (Noun, line+5, 2);
		}
		else if (!strnicmp (line, "R1", 2)) {
			strncpy (R1, line+3, 6);
		}
		else if (!strnicmp (line, "R2", 2)) {
			strncpy (R2, line+3, 6);
		}
		else if (!strnicmp (line, "R3", 2)) {
			strncpy (R3, line+3, 6);
		}
		else if (!strnicmp (line, "STATE", 5)) {
			DSKYState state;
			sscanf (line+5, "%d", &state.word);

			VerbFlashing = state.u.VerbFlashing;
			NounFlashing = state.u.NounFlashing;

			CompActy = (state.u.CompActy != 0);
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
			AltLight = (state.u.AltLight != 0);
			VelLight = (state.u.VelLight != 0);
		}
	}
}
