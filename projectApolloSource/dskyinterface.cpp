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
  *	Revision 1.1  2005/08/08 21:46:34  movieman523
  *	Initial version: this file holds the AGC->DSKY interface code.
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"

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

/*
		dsky.SetProg((out_val.Value & (1 << 8)) != 0);
*/

//
// Structure defining AGC channel 010.
//

typedef union {
	struct {
		unsigned d:5;
		unsigned c:5;
		unsigned b:1;
		unsigned a:4;
	} Bits;
	unsigned int Value;
} ChannelValue10;

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

void ApolloGuidance::LightNoAtt()

{
	SetChannel10Lights(3, true);
}

void ApolloGuidance::ClearNoAtt()

{
	SetChannel10Lights(3, false);
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

//
// Convert from AGC output to character codes.
//

char ApolloGuidance::CharValue(unsigned val)

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

void ApolloGuidance::ProcessChannel10()

{
	ChannelValue10 out_val;
	char	C1, C2;

	out_val.Value = OutputChannel[010];

	C1 = CharValue(out_val.Bits.c);
	C2 = CharValue(out_val.Bits.d);

	switch (out_val.Bits.a) {
#if 0
	case 11:
		Prog[0] = C1;
		Prog[1] = C2;
		dsky->SetProg(Prog);
		break;

	case 10:
		Verb[0] = C1;
		Verb[1] = C2;
		dsky->SetVerb(Verb);
		break;

	case 9:
		Noun[0] = C1;
		Noun[1] = C2;
		dsky->SetNoun(Noun);
		break;
	
	case 8:
		R1[1] = C2;
		dsky->SetR1(R1);
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
		dsky->SetR1(R1);
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
		dsky->SetR1(R1);
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
		dsky->SetR2(R2);
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
		dsky->SetR2(R2);
		break;

	case 3:
		R2[5] = C1;
		R3[1] = C2;
		dsky->SetR2(R2);
		dsky->SetR3(R3);
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
		dsky->SetR3(R3);
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
		dsky->SetR3(R3);
		break;
#endif
	// 12 - set light states.
	case 12:
		dsky.SetNoAtt((out_val.Value & (1 << 3)) != 0);
		dsky.SetGimbalLock((out_val.Value & (1 << 5)) != 0);
		dsky.SetTracker((out_val.Value & (1 << 7)) != 0);
		dsky.SetProg((out_val.Value & (1 << 8)) != 0);
		break;
	}
}

void ApolloGuidance::ProcessChannel11(int bit, bool val)

{
	//
	// Channel 011 has bits to control the lights on the DSKY.
	//

	switch (bit) {

	// 2 - Comp Acty
	case 2:
		dsky.SetCompActy(val);
		break;

	// 3 - Uplink
	case 3:
		dsky.SetUplink(val);
		break;

	// 4 - Temp caution
	case 4:
		dsky.SetTemp(val);
		break;

	// 5 - Kbd Rel
	case 5:
		dsky.SetKbRel(val);
		break;

	// 6 - flash verb and noun
	case 6:
		if (val) {
			dsky.SetVerbDisplayFlashing();
			dsky.SetNounDisplayFlashing();
		}
		else {
			dsky.ClearVerbDisplayFlashing();
			dsky.ClearNounDisplayFlashing();
		}
		break;

	// 7 - Opr Err
	case 7:
		dsky.SetOprErr(val);
		break;
	}
}

