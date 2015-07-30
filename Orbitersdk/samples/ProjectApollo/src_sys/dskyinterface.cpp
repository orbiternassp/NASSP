/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: DSKY interface code for AGC

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
  *	Revision 1.2  2009/09/17 17:48:42  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.13  2008/04/11 11:49:35  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.12  2007/06/06 15:02:12  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.11  2007/01/11 01:21:34  chode99
  *	Added ability for correct entry of "extended" verbs like. (verb - ENTER - noun - ENTER)
  *	
  *	Revision 1.10  2006/05/17 01:50:45  movieman523
  *	Fixed DSKY key-clicks (bug 1375310).
  *	
  *	Revision 1.9  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.8  2005/09/22 22:27:40  movieman523
  *	Updated erasable memory display/edit functions in AGC.
  *	
  *	Revision 1.7  2005/08/19 13:58:29  tschachim
  *	Added Channel 13 handling
  *	
  *	Revision 1.6  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.5  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.4  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.3  2005/08/09 02:28:26  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.2  2005/08/08 22:32:49  movieman523
  *	First steps towards reimplementing the DSKY interface to use the same I/O channels as the real AGC/DSKY interface.
  *	
  *	Revision 1.1  2005/08/08 21:46:34  movieman523
  *	Initial version: this file holds the AGC->DSKY interface code.
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "ioChannels.h"

extern char RegFormat[];

void ApolloGuidance::LightUplink()

{
	SetOutputChannelBit(011, 3, true);
}

void ApolloGuidance::ClearUplink()

{
	SetOutputChannelBit(011, 3, false);
}

void ApolloGuidance::LightCompActy()

{
	SetOutputChannelBit(011, 2, true);
}

void ApolloGuidance::ClearCompActy()

{
	SetOutputChannelBit(011, 2, false);
}

void ApolloGuidance::LightTemp()

{
	SetOutputChannelBit(011, 4, true);
}

void ApolloGuidance::ClearTemp()

{
	SetOutputChannelBit(011, 4, false);
}

void ApolloGuidance::LightKbRel()

{
	SetOutputChannelBit(011, 5, true);
}

void ApolloGuidance::ClearKbRel()

{
	SetOutputChannelBit(011, 5, false);
}

void ApolloGuidance::SetVerbNounFlashing()

{
	SetOutputChannelBit(011, 6, true);
}

void ApolloGuidance::ClearVerbNounFlashing()

{
	SetOutputChannelBit(011, 6, false);
}

void ApolloGuidance::LightOprErr()

{
	SetOutputChannelBit(011, 7, true);
}

void ApolloGuidance::ClearOprErr()

{
	SetOutputChannelBit(011, 7, false);
}

//
// Send special commands to set the Channel 10 bits indicating that a light state has
// changed.
//

void ApolloGuidance::SetChannel10Lights(int bit, bool val)

{
	ChannelValue10 v;
	unsigned int mask = (1 << bit);

	//
	// We have to track the state of the light flags, otherwise when we set one light,
	// we'll clear all the others!
	//

	if (val)
		Chan10Flags |= mask;
	else
		Chan10Flags &= ~mask;
	
	v.Value = Chan10Flags;
	v.Bits.a = 12;

	SetOutputChannel(010, v.Value);
}

void ApolloGuidance::LightVel()

{
	SetChannel10Lights(2, true);
}

void ApolloGuidance::ClearVel()

{
	SetChannel10Lights(2, false);
}

void ApolloGuidance::LightNoAtt()

{
	SetChannel10Lights(3, true);
}

void ApolloGuidance::ClearNoAtt()

{
	SetChannel10Lights(3, false);
}

void ApolloGuidance::LightAlt()

{
	SetChannel10Lights(4, true);
}

void ApolloGuidance::ClearAlt()

{
	SetChannel10Lights(4, false);
}

void ApolloGuidance::LightGimbalLock()

{
	SetChannel10Lights(5, true);
}

void ApolloGuidance::ClearGimbalLock()

{
	SetChannel10Lights(5, false);
}

void ApolloGuidance::LightTracker()

{
	SetChannel10Lights(7, true);
}

void ApolloGuidance::ClearTracker()

{
	SetChannel10Lights(7, false);
}

void ApolloGuidance::LightProg()

{
	SetChannel10Lights(8, true);
}

void ApolloGuidance::ClearProg()

{
	SetChannel10Lights(8, false);
}

void ApolloGuidance::ProcessChannel13(int val)

{
	dsky.ProcessChannel13(OutputChannel[013]);
	if (dsky2) dsky2->ProcessChannel13(OutputChannel[013]);
}

void ApolloGuidance::LightsOff()

{
	ClearCompActy();
	ClearUplink();
	ClearNoAtt();
	dsky.SetStby(false);
	if (dsky2) dsky2->SetStby(false);
	ClearKbRel();
	ClearOprErr();
	ClearTemp();
	ClearGimbalLock();
	ClearProg();
	dsky.SetRestart(false);
	if (dsky2) dsky2->SetRestart(false);
	ClearTracker();
	ClearAlt();
	ClearVel();
}

//
// Convert from character codes to AGC output.
//

unsigned ApolloGuidance::CharValue(char val)

{
	switch (val) {
	case '0':
		return 21;

	case '1':
		return 3;

	case '2':
		return 25;

	case '3':
		return 27;

	case '4':
		return 15;

	case '5':
		return 30;

	case '6':
		return 28;

	case '7':
		return 19;

	case '8':
		return 29;

	case '9':
		return 31;
	}

	return 0;
}

void ApolloGuidance::TwoDigitDisplay(char *Str, int val, bool Blanked)

{
	if (val > 99)
		val = 99;

	if (Blanked) {
		strcpy(Str, "  ");
	}
	else {
		Str[0] = '0' + (val / 10);
		Str[1] = '0' + (val % 10);
	}
}

void ApolloGuidance::FiveDigitDisplay(char *Str, int val, bool Blanked, bool Decimal, char *Format)

{
	int	i;
	int divisor, dividestep, rval;

	if (Blanked) {
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

void ApolloGuidance::ClearFiveDigitEntry()

{
	for (int i = 0; i < 6; i++)
		FiveDigitEntry[i] = ' ';
}

void ApolloGuidance::StartFiveDigitEntry(bool octal)

{
	ClearFiveDigitEntry();

	EnterPos = 1;
	EnterVal = 0;

	EnteringOctal = octal;

	//
	// Octal entry is always a positive number.
	//

	if (octal)
		EnterPositive = true;
}

void ApolloGuidance::UpdateFiveDigitEntry(int n)

{
	if (!EnterPos) {
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

	//
	// Update DSKY display.
	//

	UpdateEntry();

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

void ApolloGuidance::StartTwoDigitEntry()

{
	TwoDigitEntry[0] = TwoDigitEntry[1] = ' ';
	EnterPos = 0;
	EnterVal = 0;
}

void ApolloGuidance::UpdateTwoDigitEntry(int n)

{
	if (EnterPos > 2) {
		LightOprErr();
		return;
	}

	TwoDigitEntry[EnterPos] = '0' + n;

	if (EnteringVerb)
		UpdateVerb();
	if (EnteringNoun)
		UpdateNoun();

	if (!EnterPos) {
		EnterVal = n * 10;
	}
	else {
		EnterVal += n;

//		if (EnteringVerb) {
//			Verb = EnterVal;
//			EnteringVerb = false;
//		}

//		if (EnteringNoun) {
//			Noun = EnterVal;
//			EnteringNoun = false;
//		}

//		ReleaseKeyboard();
	}

	EnterPos++;
}

//
// Keyboard interface.
//

void ApolloGuidance::ProcessInputChannel15(int val)

{
	//
	// Do nothing until the AGC has reset.
	//
	if (!OutOfReset())
		return;

	//
	// The DSKY keys (other than Prog) come through on this
	// channel.
	//
	switch (val) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		NumberPressed(val);
		break;

	case 16:
		NumberPressed(0);
		break;

	case 17:
		VerbPressed();
		break;

	case 18:
		ResetPressed();
		break;

	case 25:
		KeyRel();
		break;

	case 26:
		PlusPressed();
		break;

	case 27:
		MinusPressed();
		break;

	case 28:
		EnterPressed();
		break;

	case 30:
		ClearPressed();
		break;

	case 31:
		NounPressed();
		break;
	}
}

void ApolloGuidance::ProcessInputChannel32(int bit, bool val)

{
	if (bit == 14) {
		if (val)
			ProgKeyPressed();
	}
}

void ApolloGuidance::ProgKeyPressed()

{
	//
	// Do nothing until the AGC has reset.
	//
	if (!OutOfReset())
		return;

	//
	// If AGC is in standby mode, then start it up.
	//

	if (OnStandby()) {
		Startup();
		return;
	}

	//
	// Else if program 6 is running, shut it down to
	// standby mode.
	//

	if (Prog == 06) {
		GoStandby();
		return;
	}

	//
	// Now we have a value in R2, we can process verb 1 or 11.
	//

	if (Verb == 1 || Verb == 11 || Verb == 21) {
		VerbNounEntered(Verb, Noun);
	}

	//
	// Otherwise tell the AGC that it's been pressed.
	//

	ProgPressed(R1, R2, R3);

	//
	// For EMEM access, let us enter another address.
	//

	if (Noun == 2) {
	switch (Verb) {
		case 1:
			DataEntryR1();
			break;

		case 11:
			ClearVerbNounFlashing();
			break;

		case 21:
			DataEntryR1R2();
			break;
		}
	}
}

void ApolloGuidance::ResetPressed()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	RSetPressed();

	UpdateAll();
	LightsOff();
}

void ApolloGuidance::ReleaseKeyboard()

{
	KbInUse = false;
	ClearKbRel();
}

void ApolloGuidance::KeyRel()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	ReleaseKeyboard();
}

void ApolloGuidance::VerbPressed()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	if (EnteringNoun) {
		LightOprErr();
		return;
	}

	ClearVerbNounFlashing();
	VerbBlanked = false;

	StartTwoDigitEntry();

	KbInUse = true;
	EnteringVerb = true;

	UpdateVerb();
}

void ApolloGuidance::NounPressed()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	if (EnteringVerb) {
		if(EnterPos!=2){
			LightOprErr();
			return;
		}
		else
		{
			Verb = EnterVal;
			EnteringVerb = false;
			ReleaseKeyboard();
		}
	}

	NounBlanked = false;

	StartTwoDigitEntry();

	KbInUse = true;
	EnteringNoun = true;

	UpdateNoun();
}

void ApolloGuidance::EnterPressed()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	//
	// Must complete entering the data before pressing
	// ENTER.
	//

//	if (EnteringVerb || EnteringNoun) {
//		LightOprErr();
//		return;
//	}
	if (EnteringVerb){
		if(EnterPos==2){
			Verb = EnterVal;
			EnteringVerb = false;
			ReleaseKeyboard();
		}
		else{
			LightOprErr();
			return;
		}

		ClearVerbNounFlashing();

		switch (Verb) {

		case 1:
		case 11:
			if (Noun != 2) {
				LightOprErr();
				return;
			}

			SetVerbNounFlashing();

			R2Decimal = false;
			R1Decimal = false;
			DataEntryR1();
			break;

		case 21:
			if (Noun == 2) {
				SetVerbNounFlashing();

				R2Decimal = false;
				R1Decimal = false;
				DataEntryR1R2();
			}
			else {
				DataEntryR1();
			}
			break;

		case 22:
			DataEntryR2();
			break;

		case 23:
			DataEntryR3();
			break;

		case 24:
			DataEntryR1R2();
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
			ProceedNoData();
			break;

	//
	// 34: terminate.
	//

		case 34:
			TerminateProgram();
			break;

		case 37:
			NounBlanked = false;

			StartTwoDigitEntry();

			KbInUse = true;
			EnteringNoun = true;

			UpdateNoun();
			break;

		default:
			VerbNounEntered(Verb, Noun);
			break;
		}
		return;
	}
	if(EnteringNoun) {
		if(EnterPos==2){
			Noun = EnterVal;
			EnteringNoun = false;
			ReleaseKeyboard();
			VerbNounEntered(Verb, Noun);
			return;
		}
		else{
			LightOprErr();
			return;
		}
	}
}

void ApolloGuidance::UpdateEntry()

{
	switch (EnteringData) {
	case 1:
		UpdateR1();
		break;

	case 2:
		UpdateR2();
		break;

	case 3:
		UpdateR3();
		break;
	}
}

void ApolloGuidance::ClearPressed()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	if (EnteringData) {
		EnterPos = 0;
		strncpy (FiveDigitEntry, "      ", 6);
		UpdateEntry();
	}
	else {
		LightOprErr();
	}
}

void ApolloGuidance::PlusPressed()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	if (EnteringData && !EnterPos) {
		EnterPositive = true;
		StartFiveDigitEntry(false);
		FiveDigitEntry[0] = '+';
		UpdateEntry();
	}
	else {
		LightOprErr();
	}
}

void ApolloGuidance::MinusPressed()

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

	if (EnteringData && !EnterPos) {
		EnterPositive = false;
		StartFiveDigitEntry(false);
		FiveDigitEntry[0] = '-';
		UpdateEntry();
	}
	else {
		LightOprErr();
	}
}


void ApolloGuidance::NumberPressed(int n)

{
	//
	// If AGC is in standby mode, do nothing.
	//
	if (OnStandby()) {
		return;
	}

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

void ApolloGuidance::DataEntryR1()

{
	ClearFiveDigitEntry();
	EnteringData = 1;
	EnterCount = 1;
	EnterPos = 0;
	R1Blanked = true;
	UpdateR1();
}

void ApolloGuidance::DataEntryR1R2()

{
	ClearFiveDigitEntry();
	EnteringData = 1;
	EnterCount = 2;
	EnterPos = 0;
	R1Blanked = true;
	R2Blanked = true;
}

void ApolloGuidance::DataEntryR2()

{
	ClearFiveDigitEntry();
	EnteringData = 2;
	EnterCount = 2;
	EnterPos = 0;
	R2Blanked = true;
	UpdateR2();
}

void ApolloGuidance::DataEntryR3()

{
	ClearFiveDigitEntry();
	EnteringData = 3;
	EnterCount = 3;
	EnterPos = 0;
	R3Blanked = true;
	UpdateR3();
}

void ApolloGuidance::UpdateR1()

{
	if (EnteringData == 1)
		strncpy(RegStr, FiveDigitEntry, 6);
	else
		FiveDigitDisplay(RegStr, R1, R1Blanked, R1Decimal, R1Format);

	ChannelValue10	v;

	v.Value = 0;
	v.Bits.a = 8;
	v.Bits.d = CharValue(RegStr[1]);
	SetOutputChannel(010, v.Value);

	v.Bits.a = 7;
	if (RegStr[0] == '+')
		v.Bits.b = 1;
	v.Bits.c = CharValue(RegStr[2]);
	v.Bits.d = CharValue(RegStr[3]);
	SetOutputChannel(010, v.Value);

	v.Value = 0;
	v.Bits.a = 6;
	if (RegStr[0] == '-')
		v.Bits.b = 1;
	v.Bits.c = CharValue(RegStr[4]);
	v.Bits.d = CharValue(RegStr[5]);
	SetOutputChannel(010, v.Value);

	//
	// Reset to zero at the end for safety.
	//
	SetOutputChannel(010, 0);
}

void ApolloGuidance::UpdateR2()

{
	char R3Str[7];

	if (EnteringData == 2)
		strncpy(RegStr, FiveDigitEntry, 6);
	else
		FiveDigitDisplay(RegStr, R2, R2Blanked, R2Decimal, R2Format);

	//
	// Unfortunately we have to send part of R3 in the same words as R2!
	//

	if (EnteringData == 3)
		strncpy(R3Str, FiveDigitEntry, 6);
	else
		FiveDigitDisplay(R3Str, R3, R3Blanked, R3Decimal, R3Format);

	ChannelValue10	v;

	v.Value = 0;
	v.Bits.a = 5;
	if (RegStr[0] == '+')
		v.Bits.b = 1;
	v.Bits.c = CharValue(RegStr[1]);
	v.Bits.d = CharValue(RegStr[2]);
	SetOutputChannel(010, v.Value);

	v.Value = 0;
	v.Bits.a = 4;
	if (RegStr[0] == '-')
		v.Bits.b = 1;
	v.Bits.c = CharValue(RegStr[3]);
	v.Bits.d = CharValue(RegStr[4]);
	SetOutputChannel(010, v.Value);

	v.Value = 0;
	v.Bits.a = 3;
	v.Bits.c = CharValue(RegStr[5]);
	v.Bits.d = CharValue(R3Str[1]);
	SetOutputChannel(010, v.Value);

	//
	// Reset to zero at the end for safety.
	//
	SetOutputChannel(010, 0);
}

void ApolloGuidance::UpdateR3()

{
	char	R2Str[7];

	if (EnteringData == 3)
		strncpy(RegStr, FiveDigitEntry, 6);
	else
		FiveDigitDisplay(RegStr, R3, R3Blanked, R3Decimal, R3Format);

	//
	// Unfortunately we have to send part of R2 in the same words as R3!
	//

	if (EnteringData == 2)
		strncpy(R2Str, FiveDigitEntry, 6);
	else
		FiveDigitDisplay(R2Str, R2, R2Blanked, R2Decimal, R2Format);

	ChannelValue10	v;

	v.Value = 0;
	v.Bits.a = 3;
	v.Bits.c = CharValue(R2Str[5]);
	v.Bits.d = CharValue(RegStr[1]);
	SetOutputChannel(010, v.Value);

	v.Value = 0;
	v.Bits.a = 2;
	if (RegStr[0] == '+')
		v.Bits.b = 1;
	v.Bits.c = CharValue(RegStr[2]);
	v.Bits.d = CharValue(RegStr[3]);
	SetOutputChannel(010, v.Value);

	v.Value = 0;
	v.Bits.a = 1;
	if (RegStr[0] == '-')
		v.Bits.b = 1;
	v.Bits.c = CharValue(RegStr[4]);
	v.Bits.d = CharValue(RegStr[5]);
	SetOutputChannel(010, v.Value);

	//
	// Reset to zero at the end for safety.
	//
	SetOutputChannel(010, 0);
}

void ApolloGuidance::UpdateProg()

{
	TwoDigitDisplay(RegStr, Prog, ProgBlanked);

	ChannelValue10	v;

	v.Value = 0;
	v.Bits.a = 11;
	v.Bits.c = CharValue(RegStr[0]);
	v.Bits.d = CharValue(RegStr[1]);

	SetOutputChannel(010, v.Value);

	//
	// Reset to zero at the end for safety.
	//
	SetOutputChannel(010, 0);
}

void ApolloGuidance::UpdateVerb()

{
	if (EnteringVerb)
		strncpy(RegStr, TwoDigitEntry, 2);
	else
		TwoDigitDisplay(RegStr, Verb, VerbBlanked);

	ChannelValue10	v;

	v.Value = 0;
	v.Bits.a = 10;
	v.Bits.c = CharValue(RegStr[0]);
	v.Bits.d = CharValue(RegStr[1]);

	SetOutputChannel(010, v.Value);

	//
	// Reset to zero at the end for safety.
	//
	SetOutputChannel(010, 0);
}

void ApolloGuidance::UpdateNoun()

{
	if (EnteringNoun)
		strncpy(RegStr, TwoDigitEntry, 2);
	else
		TwoDigitDisplay(RegStr, Noun, NounBlanked);

	ChannelValue10	v;

	v.Value = 0;
	v.Bits.a = 9;
	v.Bits.c = CharValue(RegStr[0]);
	v.Bits.d = CharValue(RegStr[1]);

	SetOutputChannel(010, v.Value);

	//
	// Reset to zero at the end for safety.
	//
	SetOutputChannel(010, 0);
}

void ApolloGuidance::DoSetR1(int val, bool decimal)

{
	if (KBCheck()) {
		R1 = val;
		R1Blanked = false;
		SetR1Format(RegFormat);
		R1Decimal = decimal;
		UpdateR1();
	}
}

void ApolloGuidance::DoSetR2(int val, bool decimal)

{
	if (KBCheck()) {
		R2 = val;
		R2Blanked = false;
		SetR2Format(RegFormat);
		R2Decimal = decimal;
		UpdateR2();
	}
}

void ApolloGuidance::DoSetR3(int val, bool decimal)

{
	if (KBCheck()) {
		R3 = val;
		R3Blanked = false;
		SetR3Format(RegFormat);
		R3Decimal = decimal;
		UpdateR3();
	}
}

void ApolloGuidance::SetR1(int val)

{
	DoSetR1(val, true);
}

void ApolloGuidance::SetR2(int val)

{
	DoSetR2(val, true);
}

void ApolloGuidance::SetR3(int val)

{
	DoSetR3(val, true);
}

void ApolloGuidance::SetR1Octal(int val)

{
	DoSetR1(val, false);
}

void ApolloGuidance::SetR2Octal(int val)

{
	DoSetR2(val, false);
}

void ApolloGuidance::SetR3Octal(int val)

{
	DoSetR3(val, false);
}

void ApolloGuidance::SetProg(int val)

{
	Prog = val;
	ProgBlanked = false;
	UpdateProg();
}

void ApolloGuidance::SetVerb(int val)

{
	if (KBCheck()) {
		Verb = val;
		VerbBlanked = false;
		UpdateVerb();
	}
}

void ApolloGuidance::SetNoun(int val)

{
	if (KBCheck()) {
		Noun = val;
		NounBlanked = false;
		UpdateNoun();
	}
}

void ApolloGuidance::UpdateAll()

{
	UpdateProg();
	UpdateVerb();
	UpdateNoun();
	UpdateR1();
	UpdateR2();
	UpdateR3();
}

void ApolloGuidance::UnBlankAll()

{
	ProgBlanked = false;
	VerbBlanked = false;
	NounBlanked = false;
	R1Blanked = false;
	R2Blanked = false;
	R3Blanked = false;

	UpdateAll();
}

void ApolloGuidance::BlankAll()

{
	ProgBlanked = true;
	VerbBlanked = true;
	NounBlanked = true;
	R1Blanked = true;
	R2Blanked = true;
	R3Blanked = true;

	UpdateAll();
}

void ApolloGuidance::BlankData()

{
	R1Blanked = true;
	R2Blanked = true;
	R3Blanked = true;

	UpdateAll();
}

void ApolloGuidance::BlankR1()

{
	if (KBCheck()) {
		R1Blanked = true;
		UpdateR1();
	}
}

void ApolloGuidance::BlankR2()

{
	if (KBCheck()) {
		R2Blanked = true;
		UpdateR2();
	}
}

void ApolloGuidance::BlankR3()

{
	if (KBCheck()) {
		R3Blanked = true;
		UpdateR3();
	}
}

//
// Attempt to set a value into the register, and light the KBD REL
// light if the keyboard is in use.
//

bool ApolloGuidance::KBCheck()

{
	if (KbInUse) {
		LightKbRel();
		return false;
	}

	return true;
}

//
// By default, just pass these calls through to the dsky.
//

void ApolloGuidance::ProcessChannel10(int val)

{
	dsky.ProcessChannel10(val);
	if (dsky2) dsky2->ProcessChannel10(val);
}

void ApolloGuidance::ProcessChannel11Bit(int bit, bool val)

{
	dsky.ProcessChannel11Bit(bit, val);
	if (dsky2) dsky2->ProcessChannel11Bit(bit, val);
}

void ApolloGuidance::ProcessChannel11(int val)

{
	dsky.ProcessChannel11(val);
	if (dsky2) dsky2->ProcessChannel11(val);
}

