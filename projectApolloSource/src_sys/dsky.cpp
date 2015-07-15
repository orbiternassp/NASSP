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
  *	Revision 1.6  2011/07/11 01:42:36  vrouleau
  *	- Removed AGC_SOCKET_ENABLED flag. Rework is needed to make this an optional feature instead of a conditional define. To many untested think exists in the socket version
  *	
  *	- Checkpoint commit on the LEM RR. If the RR as been slew to track the CSM , the auto mode will continue tracking it.
  *	
  *	Revision 1.5  2010/08/28 16:16:33  dseagrav
  *	Fixed LM DSKY to use dimmer. (Dimmer source may be wrong)
  *	Corrected a typo and did some bracketization in DSKY source.
  *	Wasted a lot of time figuring out our copy of Luminary had been garbaged.
  *	Your Luminary 99 binary should be overwritten by the commit before this one.
  *	If your binary is 700 bytes in size you have the garbaged version.
  *	
  *	Revision 1.4  2010/07/16 17:14:42  tschachim
  *	Changes for Orbiter 2010 and bugfixes
  *	
  *	Revision 1.3  2009/09/17 17:48:42  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.2  2009/08/02 19:21:07  spacex15
  *	agc socket version reenabled
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.26  2008/04/15 20:46:12  lassombra
  *	Panel callback compatible callback methods implemented
  *	
  *	Revision 1.25  2008/04/11 11:49:34  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.24  2007/10/18 00:23:19  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.23  2007/08/13 16:06:09  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.22  2007/06/06 15:02:12  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.21  2006/12/26 06:24:43  dseagrav
  *	vAGC restart if not powered, AGC VOLTAGE ALARM simulated with DSKY RESTART lights, more telemetry stuff, Merry Day-After-Christmas!
  *	
  *	Revision 1.20  2006/06/21 13:11:29  tschachim
  *	Bugfix power drawing.
  *	
  *	Revision 1.19  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.18  2006/06/08 15:25:57  tschachim
  *	Disabkled buggy DrawPower.
  *	
  *	Revision 1.17  2006/05/30 23:15:14  movieman523
  *	Mission timer and DSKY now need power to operate.
  *	
  *	Revision 1.16  2006/05/17 01:50:45  movieman523
  *	Fixed DSKY key-clicks (bug 1375310).
  *	
  *	Revision 1.15  2005/11/15 05:42:54  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.14  2005/08/29 19:16:43  tschachim
  *	Rendering of the DSKY keys.
  *	
  *	Revision 1.13  2005/08/19 13:45:26  tschachim
  *	Added missing DSKY display elements.
  *	Added channel 013 handling.
  *	
  *	Revision 1.12  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.11  2005/08/18 20:07:19  spacex15
  *	fixed click sound missing in lm dsky
  *	
  *	Revision 1.10  2005/08/18 00:22:52  movieman523
  *	Wired in CM Uplink switch, removed some old code, added initial support for second DSKY.
  *	
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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "soundlib.h"
#include "nasspsound.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "ioChannels.h"

#include "nasspdefs.h"

static char TwoSpace[] = "  ";
static char SixSpace[] = "      ";

static int SegmentCount[] = {6, 2, 5, 5, 4, 5, 6, 3, 7, 5 };

DSKY::DSKY(SoundLib &s, ApolloGuidance &computer, int IOChannel) : soundlib(s), agc(computer)

{
	DimmerRotationalSwitch = NULL;
	Reset();
	ResetKeyDown();
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
	if(agc.Yaagc && agc.vagc.VoltageAlarm != 0){
		RestartLight = true;
	}else{
		RestartLight = false;
	}
	TrackerLight = false;
	VelLight = false;
	AltLight = false;

	LightsLit = 0;
	SegmentsLit = 0;

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

void DSKY::Init(e_object *powered, RotationalSwitch *dimmer)

{
	WireTo(powered);
	DimmerRotationalSwitch = dimmer;
	Reset();
	FirstTimeStep = true;
}

bool DSKY::IsPowered() {
	if (Voltage() < SP_MIN_ACVOLTAGE){ return false; }

	if (DimmerRotationalSwitch != NULL) {
		if (DimmerRotationalSwitch->GetState() == 0) {
			return false;
		}
	}
	return true;
}

void DSKY::Timestep(double simt)

{
	if(FirstTimeStep)
	{
		FirstTimeStep = false;
	    soundlib.LoadSound(Sclick, BUTTON_SOUND);
	}

	//
	// Flash counter. For simplicity we'll always update
	// this even though nothing may be flashing.
	//

	if (simt > (LastFlashTime + 0.5)) {
		LastFlashTime = simt;
		FlashOn = !FlashOn;
	}
}

void DSKY::SystemTimestep(double simdt)

{
	if (!IsPowered()){ return; }
	//
	// The DSKY power consumption is a little bit hard to figure out. According 
	// to the Systems Handbook the complete interior lightning draws about 30W, so
	// we assume one DSKY draws 10W max, for now. We DO NOT rely on the render code to
	// track the number of lights that are lit, because during pause the still called render 
	// code causes wrong power loads
	//

	//
	// Check the lights.
	//

	LightsLit = 0;
	if (UplinkLit()) LightsLit++;
	if (NoAttLit()) LightsLit++;
	if (StbyLit()) LightsLit++;
	if (KbRelLit() && FlashOn) LightsLit++;
	if (OprErrLit() && FlashOn) LightsLit++;
	if (TempLit()) LightsLit++;
	if (GimbalLockLit()) LightsLit++;
	if (ProgLit()) LightsLit++;
	if (RestartLit()) LightsLit++;
	if (TrackerLit()) LightsLit++;

	//
	// Check the segments
	//

	SegmentsLit = 6;
	if (CompActy) 
		SegmentsLit += 4;

	SegmentsLit += TwoDigitDisplaySegmentsLit(Prog, false);
	SegmentsLit += TwoDigitDisplaySegmentsLit(Verb, VerbFlashing);
	SegmentsLit += TwoDigitDisplaySegmentsLit(Noun, NounFlashing);

	SegmentsLit += SixDigitDisplaySegmentsLit(R1);
	SegmentsLit += SixDigitDisplaySegmentsLit(R2);
	SegmentsLit += SixDigitDisplaySegmentsLit(R3);

	// 10 lights with together max. 6W, 184 segments with together max. 4W  
	DrawPower((LightsLit * 0.6) + (SegmentsLit * 0.022));

	//sprintf(oapiDebugString(), "DSKY %f", (LightsLit * 0.6) + (SegmentsLit * 0.022));
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
	KeyClick();
	SendKeyCode(25);
}

void DSKY::VerbPressed()

{
	KeyClick();

	VerbFlashing = false;
	SendKeyCode(17);
}

void DSKY::NounPressed()

{	
	KeyClick();

	NounFlashing = false;
	SendKeyCode(31);
}

void DSKY::EnterPressed()

{
	KeyClick();
	SendKeyCode(28);
}

void DSKY::ClearPressed()

{
	KeyClick();
	SendKeyCode(30);
}

void DSKY::PlusPressed()

{
	KeyClick();
	SendKeyCode(26);
}

void DSKY::MinusPressed()

{
	KeyClick();
	SendKeyCode(27);
}

void DSKY::ProgPressed()

{
	KeyClick();

	agc.SetInputChannelBit(032, 14, true);
}

void DSKY::ProgReleased()

{
	agc.SetInputChannelBit(032, 14, false);
}

void DSKY::ResetPressed()

{
	KeyClick();
	SendKeyCode(18);

	// DS20061225 If the RESTART light is lit, this resets it externally to the AGC program. CSM 104 SYS HBK pg 399
	if (RestartLit()) {
		ClearRestart(); 
	}

	if(agc.Yaagc && agc.vagc.VoltageAlarm != 0){
		agc.vagc.VoltageAlarm = 0;
	}
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

void DSKY::ProcessChannel13(int val)

{
	ChannelValue13 out_val;

	out_val.Value = val;

	/// \todo Standby light with PRO key?
	if (out_val.Bits.EnableStandby || out_val.Bits.TestAlarms)
	{
		SetStby(true);
	}
	else
	{
		SetStby(false);
	}

	/// \todo Other conditions restart light
	if (out_val.Bits.TestAlarms || (agc.Yaagc && agc.vagc.VoltageAlarm != 0))
	{
		SetRestart(true);
	}
	else
	{
		SetRestart(false);
	}
}

void DSKY::DSKYLightBlt(SURFHANDLE surf, SURFHANDLE lights, int dstx, int dsty, bool lit, int xOffset, int yOffset)

{
	if (lit) {
		oapiBlt(surf, lights, dstx + xOffset, dsty + yOffset, dstx + 101, dsty + 0, 49, 23);
	}
	else {
		oapiBlt(surf, lights, dstx + xOffset, dsty + yOffset, dstx + 0, dsty + 0, 49, 23);
	}
}

void DSKY::RenderLights(SURFHANDLE surf, SURFHANDLE lights, int xOffset, int yOffset, bool hasAltVel)

{
	if (!IsPowered())
		return;

	//
	// Check the lights.
	//

	DSKYLightBlt(surf, lights, 0, 0,  UplinkLit(), xOffset, yOffset);
	DSKYLightBlt(surf, lights, 0, 25, NoAttLit(), xOffset, yOffset);
	DSKYLightBlt(surf, lights, 0, 49, StbyLit(), xOffset, yOffset);
	DSKYLightBlt(surf, lights, 0, 73, KbRelLit() && FlashOn, xOffset, yOffset);
	DSKYLightBlt(surf, lights, 0, 97, OprErrLit() && FlashOn, xOffset, yOffset);

	DSKYLightBlt(surf, lights, 52, 0,  TempLit(), xOffset, yOffset);
	DSKYLightBlt(surf, lights, 52, 25, GimbalLockLit(), xOffset, yOffset);
	DSKYLightBlt(surf, lights, 52, 49, ProgLit(), xOffset, yOffset);
	DSKYLightBlt(surf, lights, 52, 73, RestartLit(), xOffset, yOffset);
	DSKYLightBlt(surf, lights, 52, 97, TrackerLit(), xOffset, yOffset);

	if (hasAltVel) {
		DSKYLightBlt(surf, lights, 52, 121, AltLit(), xOffset, yOffset);
		DSKYLightBlt(surf, lights, 52, 144, VelLit(), xOffset, yOffset);
	}
}


//
// Process a keypress based on the X and Y coords.
//

void DSKY::ProcessKeyPress(int mx, int my)

{
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

}

void DSKY::ProcessKeyRelease(int mx, int my)

{
	if (mx > 2+5*41 && mx < 39+5*41) {
		if (my > 41 && my < 79) {
			ProgReleased();
		}
	}
	else {
		// this seems to cause operation errors
		// SendKeyCode(0);
	}

	ResetKeyDown();
}

void DSKY::ResetKeyDown() 

{
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
}

void DSKY::RenderTwoDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str, bool Flash)

{
	int Curdigit;

	if (Flash && !FlashOn)
		return;

	if (Str[0] >= '0' && Str[0] <= '9') {
		Curdigit = Str[0] - '0';
		oapiBlt(surf,digits,dstx,dsty,16*Curdigit,0,16,19);
	}

	if (Str[1] >= '0' && Str[1] <= '9') {
		Curdigit = Str[1] - '0';
		oapiBlt(surf,digits,dstx+16,dsty,16*Curdigit,0,16,19);
	}
}

int DSKY::TwoDigitDisplaySegmentsLit(char *Str, bool Flash)

{
	int Curdigit, s = 0;

	if (Flash && !FlashOn)
		return s;

	if (Str[0] >= '0' && Str[0] <= '9') {
		Curdigit = Str[0] - '0';
		s += SegmentCount[Curdigit];
	}

	if (Str[1] >= '0' && Str[1] <= '9') {
		Curdigit = Str[1] - '0';
		s += SegmentCount[Curdigit];
	}
	return s;
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
		if (Str[i] >= '0' && Str[i] <= '9') {
			Curdigit = Str[i] - '0';
			oapiBlt(surf, digits, dstx + (16*i), dsty, 16*Curdigit, 0, 16,19);
		}
		else {
//			oapiBlt(surf, digits, dstx + (10*i), dsty, 440, 6, 10, 15);
		}
	}
}

int DSKY::SixDigitDisplaySegmentsLit(char *Str)

{
	int	Curdigit;
	int i, s = 0;

	if (Str[0] == '-') 
		s += 1;
	else if (Str[0] == '+') 
		s += 2;

	for (i = 1; i < 6; i++) {
		if (Str[i] >= '0' && Str[i] <= '9') {
			Curdigit = Str[i] - '0';
			s += SegmentCount[Curdigit];
		}
	}
	return s;
}

void DSKY::RenderData(SURFHANDLE surf, SURFHANDLE digits, SURFHANDLE disp, int xOffset, int yOffset)

{
	if (!IsPowered())
		return;

	oapiBlt(surf, disp, 66 + xOffset,   3 + yOffset, 35,  0, 35, 10, SURF_PREDEF_CK);
	oapiBlt(surf, disp, 66 + xOffset,  38 + yOffset, 35, 10, 35, 10, SURF_PREDEF_CK);
	oapiBlt(surf, disp,  6 + xOffset,  38 + yOffset, 35, 20, 35, 10, SURF_PREDEF_CK);

	oapiBlt(surf, disp,  8 + xOffset,  73 + yOffset,  0, 32, 89,  4, SURF_PREDEF_CK);
	oapiBlt(surf, disp,  8 + xOffset, 107 + yOffset,  0, 32, 89,  4, SURF_PREDEF_CK);
	oapiBlt(surf, disp,  8 + xOffset, 141 + yOffset,  0, 32, 89,  4, SURF_PREDEF_CK);

	if (CompActy) {
		//
		// Do stuff to update Comp Acty light.
		//

		oapiBlt(surf, disp,  6 + xOffset,   4 + yOffset,  0,  0, 35, 31, SURF_PREDEF_CK);
	}

	RenderTwoDigitDisplay(surf, digits, 67 + xOffset, 16 + yOffset, Prog, false);
	RenderTwoDigitDisplay(surf, digits,  8 + xOffset, 51 + yOffset, Verb, VerbFlashing);
	RenderTwoDigitDisplay(surf, digits, 67 + xOffset, 51 + yOffset, Noun, NounFlashing);

	//
	// Register contents.
	//

	RenderSixDigitDisplay(surf, digits, 3 + xOffset, 83 + yOffset, R1);
	RenderSixDigitDisplay(surf, digits, 3 + xOffset, 117 + yOffset, R2);
	RenderSixDigitDisplay(surf, digits, 3 + xOffset, 151 + yOffset, R3);
}

void DSKY::RenderKeys(SURFHANDLE surf, SURFHANDLE keys, int xOffset, int yOffset)

{
	DSKYKeyBlt(surf, keys, 2, 21, 0, 20, KeyDown_Verb, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2, 61, 0, 60, KeyDown_Noun, xOffset, yOffset);

	DSKYKeyBlt(surf, keys, 2 + 41 * 1, 1,  41 * 1, 0,  KeyDown_Plus, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 1, 41, 41 * 1, 40, KeyDown_Minus, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 1, 81, 41 * 1, 80, KeyDown_0, xOffset, yOffset);

	DSKYKeyBlt(surf, keys, 2 + 41 * 2, 1,  41 * 2, 0,  KeyDown_7, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 2, 41, 41 * 2, 40, KeyDown_4, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 2, 81, 41 * 2, 80, KeyDown_1, xOffset, yOffset);

	DSKYKeyBlt(surf, keys, 2 + 41 * 3, 1,  41 * 3, 0,  KeyDown_8, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 3, 41, 41 * 3, 40, KeyDown_5, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 3, 81, 41 * 3, 80, KeyDown_2, xOffset, yOffset);

	DSKYKeyBlt(surf, keys, 2 + 41 * 4, 1,  41 * 4, 0,  KeyDown_9, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 4, 41, 41 * 4, 40, KeyDown_6, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 4, 81, 41 * 4, 80, KeyDown_3, xOffset, yOffset);

	DSKYKeyBlt(surf, keys, 2 + 41 * 5, 1,  41 * 5, 0,  KeyDown_Clear, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 5, 41, 41 * 5, 40, KeyDown_Prog, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 5, 81, 41 * 5, 80, KeyDown_KeyRel, xOffset, yOffset);

	DSKYKeyBlt(surf, keys, 2 + 41 * 6, 21, 41 * 6, 20, KeyDown_Enter, xOffset, yOffset);
	DSKYKeyBlt(surf, keys, 2 + 41 * 6, 61, 41 * 6, 60, KeyDown_Reset, xOffset, yOffset);
}

void DSKY::DSKYKeyBlt(SURFHANDLE surf, SURFHANDLE keys, int dstx, int dsty, int srcx, int srcy, bool lit, int xOffset, int yOffset) 

{
	if (lit) {
		oapiBlt(surf, keys, dstx + xOffset, dsty + yOffset, srcx, srcy, 38, 37);
	}
	else {
		oapiBlt(surf, keys, dstx + xOffset, dsty + yOffset, srcx, srcy + 120, 38, 37);
	}
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

void DSKY::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
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

	oapiWriteLine(scn, end_str);
}


void DSKY::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int end_len = strlen (end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
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

//
// I/O channel processing.
//


void DSKY::ProcessChannel11(int val)

{
	ChannelValue11 val11;

	val11.Value = val;
	SetCompActy(val11.Bits.LightComputerActivity);
	SetUplink(val11.Bits.LightUplink);
	SetTemp(val11.Bits.LightTempCaution);
	SetKbRel(val11.Bits.LightKbRel);
	SetOprErr(val11.Bits.LightOprErr);

	if (val11.Bits.FlashVerbNoun) {
		SetVerbDisplayFlashing();
		SetNounDisplayFlashing();
	}
	else {
		ClearVerbDisplayFlashing();
		ClearNounDisplayFlashing();
	}
}

void DSKY::ProcessChannel11Bit(int bit, bool val)

{
	//
	// Channel 011 has bits to control the lights on the DSKY.
	//

	switch (bit) {

	// 2 - Comp Acty
	case 2:
		SetCompActy(val);
		break;

	// 3 - Uplink
	case 3:
		SetUplink(val);
		break;

	// 4 - Temp caution
	case 4:
		SetTemp(val);
		break;

	// 5 - Kbd Rel
	case 5:
		SetKbRel(val);
		break;

	// 6 - flash verb and noun
	case 6:
		if (val) {
			SetVerbDisplayFlashing();
			SetNounDisplayFlashing();
		}
		else {
			ClearVerbDisplayFlashing();
			ClearNounDisplayFlashing();
		}
		break;

	// 7 - Opr Err
	case 7:
		SetOprErr(val);
		break;
	}
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
		SetVel((out_val.Value & (1 << 2)) != 0);
		SetNoAtt((out_val.Value & (1 << 3)) != 0);
		SetAlt((out_val.Value & (1 << 4)) != 0);
		SetGimbalLock((out_val.Value & (1 << 5)) != 0);
		SetTracker((out_val.Value & (1 << 7)) != 0);
		SetProg((out_val.Value & (1 << 8)) != 0);
		break;
	}
}
// Callbacks to handle button presses from the panel.
// These allow us to set up callbacks on regular panel
// Switches instead of using special case mouse handlers.
// Also allows for automatic checklists to perform certain duties
// Such as V37E00E on insertion or post burn.

void DSKY::VerbCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Verb = true;
		VerbPressed();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::NounCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Noun = true;
		NounPressed();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::EnterCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Enter = true;
		EnterPressed();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::ClearCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Clear = true;
		ClearPressed();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::ResetCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Reset = true;
		ResetPressed();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::ProgCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Prog = true;
		ProgPressed();
	}
	else
	{
		ProgReleased();
		ResetKeyDown();
	}
}
void DSKY::KeyRelCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_KeyRel = true;
		KeyRel();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::PlusCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Plus = true;
		PlusPressed();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::MinusCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_Minus = true;
		MinusPressed();
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::zeroCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_0 = true;
		NumberPressed(0);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::oneCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_1 = true;
		NumberPressed(1);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::twoCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_2 = true;
		NumberPressed(2);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::threeCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_3 = true;
		NumberPressed(3);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::fourCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_4 = true;
		NumberPressed(4);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::fiveCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_5 = true;
		NumberPressed(5);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::sixCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_6 = true;
		NumberPressed(6);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::sevenCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_7 = true;
		NumberPressed(7);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::eightCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_8 = true;
		NumberPressed(8);
	}
	else
	{
		ResetKeyDown();
	}
}
void DSKY::nineCallback(PanelSwitchItem* s)
{
	if (s->GetState() == 1)
	{
		KeyDown_9 = true;
		NumberPressed(9);
	}
	else
	{
		ResetKeyDown();
	}
}
