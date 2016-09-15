/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"
#include "tracer.h"
#include "papi.h"
#include "CollisionSDK/CollisionSDK.h"

#include "connector.h"

static char ThreeSpace[] = "   ";
static char SixSpace[] = "      ";
static int SegmentCount[] = {6, 2, 5, 5, 4, 5, 6, 3, 7, 5 };

// Abort Sensor Assembly
LEM_ASA::LEM_ASA()// : hsink("LEM-ASA-HSink",_vector3(0.013, 3.0, 0.03),0.03,0.04),
	//heater("LEM-ASA-Heater",1,NULL,15,20,0,272,274,&hsink)
{
	lem = NULL;	
}

void LEM_ASA::Init(LEM *s, Boiler *hb, h_Radiator *hr) {
	lem = s;
	heater = hb;
	hsink = hr;
	// Therm setup
	hsink->isolation = 1.0;
	hsink->Area = 975.0425;
	//hsink.mass = 9389.36206;
	//hsink.SetTemp(270);
	if (lem != NULL) {
		heater->WireTo(&lem->SCS_ASA_CB);
		//lem->Panelsdk.AddHydraulic(&hsink);
		//lem->Panelsdk.AddElectrical(&heater, false);
		heater->Enable();
		heater->SetPumpAuto();
	}
}

void LEM_ASA::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// AGS OFF  = ASA heaters active (OFF mode)
	// AGS STBY = ASA fully active   (WARMUP mode, becomes OPERATE mode when temp allows)
	// ASA OPR  = ASA fully active   (ditto)

	// ASA is 11.5x8x5.125 inches and weighs 20.7 pounds
	// ASA draws 74 watts operating? Need more info

	// ASA wants to stay at 120F.
	// Fast Warmup can get the ASA from 30F to 116F in 40 minutes.
	// Fast Warmup is active below 116F.
	// At 116F the Fine Warmup circuit takes over and gets to 120F and maintains it to within 0.2 degree F

	// There is no information on what the "OFF" mode does other than run the ASA heaters.
	// My guess is that some small heater keeps the ASA at 30F until standby happens.
	// sprintf(oapiDebugString(),"ASA Temp: %f AH %f",hsink.Temp,heater.pumping);

	// FIXME: ASA goes here	
}

void LEM_ASA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_ASA::LoadState(FILEHANDLE scn,char *end_str){

}

// Abort Electronics Assembly
LEM_AEA::LEM_AEA(){
	lem = NULL;	
}

void LEM_AEA::Init(LEM *s){
	lem = s;
}

void LEM_AEA::TimeStep(double simdt){
	if(lem == NULL){ return; }
}

void LEM_AEA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_AEA::LoadState(FILEHANDLE scn,char *end_str){

}

// Data Entry and Display Assembly

LEM_DEDA::LEM_DEDA(LEM *lm, SoundLib &s,LEM_AEA &computer, int IOChannel) :  lem(lm), soundlib(s), ags(computer)

{
	Reset();
	ResetKeyDown();
	KeyCodeIOChannel = IOChannel;
}

LEM_DEDA::~LEM_DEDA()
{
	//
	// Nothing for now.
	//
}

void LEM_DEDA::Init(e_object *powered)

{
	WireTo(powered);
	Reset();
	FirstTimeStep = true;
}

void LEM_DEDA::TimeStep(double simdt){
	if(lem == NULL){ return; }

	if(FirstTimeStep)
	{
		FirstTimeStep = false;
	    soundlib.LoadSound(Sclick, BUTTON_SOUND);
	}
}

void LEM_DEDA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_DEDA::LoadState(FILEHANDLE scn,char *end_str){

}

void LEM_DEDA::KeyClick()

{
	Sclick.play(NOLOOP, 255);
}

void LEM_DEDA::Reset()

{
	OprErrLight = false;
	LightsLit = 0;
	SegmentsLit = 0;
	State = 0;
	Held = false;

	strcpy (Adr, ThreeSpace);
	strcpy (Data, SixSpace);
}

void LEM_DEDA::ResetKeyDown() 

{
	// Reset KeyDown-flags
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
	KeyDown_ReadOut = false;
	KeyDown_Enter = false;
	KeyDown_Hold = false;
}

void LEM_DEDA::SystemTimestep(double simdt)

{
	if (!IsPowered())
		return;
	
	// We will use a similar scan as the DSKY power consumption

	// The DSYK power consumption is a little bit hard to figure out. According 
	// to the Systems Handbook the complete interior lightning draws about 30W, so
	// we assume one DSKY draws 10W max, for now. We DO NOT rely on the render code to
	// track the number of lights that are lit, because during pause the still called render 
	// code causes wrong power loads
	//

	//
	// Check the lights.
	//

	LightsLit = 0;
	if (OprErrLit()) LightsLit++;
	//
	// Check the segments
	//

	SegmentsLit += ThreeDigitDisplaySegmentsLit(Adr);
	SegmentsLit += SixDigitDisplaySegmentsLit(Data);

	// 10 lights with together max. 6W, 184 segments with together max. 4W  
	DrawPower((LightsLit * 0.6) + (SegmentsLit * 0.022));

	//sprintf(oapiDebugString(), "DSKY %f", (LightsLit * 0.6) + (SegmentsLit * 0.022));
}

int LEM_DEDA::ThreeDigitDisplaySegmentsLit(char *Str)

{
	int Curdigit, s = 0;

	if (Str[0] >= '0' && Str[0] <= '9') {
		Curdigit = Str[0] - '0';
		s += SegmentCount[Curdigit];
	}

	if (Str[1] >= '0' && Str[1] <= '9') {
		Curdigit = Str[1] - '0';
		s += SegmentCount[Curdigit];
	}

	if (Str[2] >= '0' && Str[2] <= '9') {
		Curdigit = Str[1] - '0';
		s += SegmentCount[Curdigit];
	}
	return s;
}

void LEM_DEDA::RenderThreeDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str)

{
	int Curdigit;

	if (Str[0] >= '0' && Str[0] <= '9') {
		Curdigit = Str[0] - '0';
		oapiBlt(surf,digits,dstx+0,dsty,19*Curdigit,0,19,21);
	}

	if (Str[1] >= '0' && Str[1] <= '9') {
		Curdigit = Str[1] - '0';
		oapiBlt(surf,digits,dstx+20,dsty,19*Curdigit,0,19,21);
	}

	if (Str[2] >= '0' && Str[2] <= '9') {
		Curdigit = Str[2] - '0';
		oapiBlt(surf,digits,dstx+39,dsty,19*Curdigit,0,19,21);
	}
}

int LEM_DEDA::SixDigitDisplaySegmentsLit(char *Str)
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

void LEM_DEDA::RenderSixDigitDisplay(SURFHANDLE surf, SURFHANDLE digits, int dstx, int dsty, char *Str)

{
	int	Curdigit;
	int i;

	if (Str[0] == '-') {
		oapiBlt(surf,digits,dstx+4,dsty,191,0,19,21);
	}
	else if (Str[0] == '+') {
		oapiBlt(surf,digits,dstx+4,dsty,210,0,19,21);
	}

	for (i = 1; i < 6; i++) {
		if (Str[i] >= '0' && Str[i] <= '9') {
			Curdigit = Str[i] - '0';
			oapiBlt(surf, digits, dstx + (20*i)+ 4, dsty, 19*Curdigit, 0, 19,21);
		}
		else {
//			oapiBlt(surf, digits, dstx + (10*i), dsty, 440, 6, 10, 15);
		}
	}
}


void LEM_DEDA::RenderAdr(SURFHANDLE surf, SURFHANDLE digits, int xOffset, int yOffset)

{
	if (!IsPowered())
		return;

	RenderThreeDigitDisplay(surf, digits, xOffset, yOffset, Adr);
}

void LEM_DEDA::RenderData(SURFHANDLE surf, SURFHANDLE digits, int xOffset, int yOffset)

{
	if (!IsPowered())
		return;

	//
	// Register contents.
	//

	RenderSixDigitDisplay(surf, digits, xOffset, yOffset, Data);
}

void LEM_DEDA::RenderKeys(SURFHANDLE surf, SURFHANDLE keys, int xOffset, int yOffset)

{
	DEDAKeyBlt(surf, keys, 1 + 44 * 0, 1,  44 * 0, 0,  KeyDown_Plus, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 0, 45, 44 * 0, 44, KeyDown_Minus, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 0, 90, 44 * 0, 88, KeyDown_0, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 1, 1,  44 * 1, 0,  KeyDown_7, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 1, 45, 44 * 1, 44, KeyDown_4, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 1, 90, 44 * 1, 88, KeyDown_1, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 1,  44 * 2, 0,  KeyDown_8, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 45, 44 * 2, 44, KeyDown_5, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 90, 44 * 2, 88, KeyDown_2, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 3, 1,  44 * 3, 0,  KeyDown_9, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 3, 45, 44 * 3, 44, KeyDown_6, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 3, 90, 44 * 3, 88, KeyDown_3, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 4, 1,  44 * 4, 0,  KeyDown_Clear, xOffset, yOffset);
	DEDAKeyBlt(surf, keys, 1 + 44 * 4, 45, 44 * 4, 44, KeyDown_ReadOut, xOffset, yOffset);
    DEDAKeyBlt(surf, keys, 1 + 44 * 4, 90, 44 * 4, 88, KeyDown_Enter, xOffset, yOffset);

	DEDAKeyBlt(surf, keys, 1 + 44 * 2, 134, 44 * 2, 132, KeyDown_Hold, xOffset, yOffset);
}

void LEM_DEDA::DEDAKeyBlt(SURFHANDLE surf, SURFHANDLE keys, int dstx, int dsty, int srcx, int srcy, bool lit, int xOffset, int yOffset) 

{
	if (lit) {
		oapiBlt(surf, keys, dstx + xOffset, dsty + yOffset, srcx, srcy, 40, 40);
	}
	else {
		oapiBlt(surf, keys, dstx + xOffset, dsty + yOffset, srcx, srcy + 173, 40, 40);
	}
}


void LEM_DEDA::RenderOprErr(SURFHANDLE surf, SURFHANDLE lights)

{
	if (!IsPowered())
		return;

	//
	// Check the lights.
	//

	if (OprErrLit()) {
		oapiBlt(surf, lights, 0, 0, 46, 0, 45, 25);
	}
	else {
		oapiBlt(surf, lights, 0, 0, 0, 0, 45, 25);
	}

}


//
// Process a keypress based on the X and Y coords.
//

void LEM_DEDA::ProcessKeyPress(int mx, int my)

{
	KeyClick();

	if (mx > 2+0*44 && mx < 43+0*44) {

		if (my > 1 && my < 43) {
			KeyDown_Plus = true;
			if (OprErrLit() || !IsPowered())
					return;
			PlusPressed();
		}
		if (my > 44 && my < 88) {
			KeyDown_Minus = true;
			if (OprErrLit() || !IsPowered())
					return;
			MinusPressed();
		}
		if (my > 88 && my < 132) {
			KeyDown_0 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(0);
		}
	}

	if (mx > 2+1*43 && mx < 43+1*44) {
		if (my > 1 && my < 43) {
			KeyDown_7 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(7);
		}
		if (my > 44 && my < 88) {
			KeyDown_4 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(4);
		}
		if (my > 88 && my < 132) {
			KeyDown_1 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(1);
		}
	}

	if (mx > 2+2*44 && mx < 43+2*44) {
		if (my > 1 && my < 43) {
			KeyDown_8 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(8);
		}
		if (my > 44 && my < 88) {
			KeyDown_5 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(5);
		}
		if (my > 88 && my < 132) {
			KeyDown_2 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(2);
		}

		if (my > 132 && my < 176) {
			KeyDown_Hold = true;
			if (OprErrLit() || !IsPowered())
					return;
			HoldPressed();
		}
	}

	if (mx > 2+3*44 && mx < 43+3*44) {
		if (my > 1 && my < 43) {
			KeyDown_9 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(9);
		}
		if (my > 44 && my < 88) {
			KeyDown_6 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(6);
		}
		if (my > 88 && my < 132) {
			KeyDown_3 = true;
			if (OprErrLit() || !IsPowered())
					return;
			NumberPressed(3);
		}
	}

	if (mx > 2+4*44 && mx < 43+4*44) {
		if (my > 1 && my < 43) {
			KeyDown_Clear = true;
//			ClearPressed();
		}
		if (my > 44 && my < 88) {
			KeyDown_ReadOut = true;
			if (OprErrLit() || !IsPowered())
					return;
			ReadOutPressed();
		}

		if (my > 88 && my < 132) {
			KeyDown_Enter = true;
			if (OprErrLit() || !IsPowered())
					return;
			EnterPressed();
		}
	}
}

void LEM_DEDA::ProcessKeyRelease(int mx, int my)

{
	if (mx > 2+4*44 && mx < 43+4*44) {
		if (my > 1 && my < 43) {
			ClearPressed();
		}
	}
	ResetKeyDown();
}

void LEM_DEDA::SendKeyCode(int val)

{
	//agc.SetInputChannel(KeyCodeIOChannel, val);
}

void LEM_DEDA::KeyRel()

{
	SendKeyCode(25);
}

void LEM_DEDA::EnterPressed()

{
	if (State == 9)
		SendKeyCode(28);
	else
		SetOprErr(true);

	Held = false;
}

void LEM_DEDA::ClearPressed()

{
	Reset();
	ResetKeyDown();
}

void LEM_DEDA::PlusPressed()

{
	if (State == 3){
			State++;
			Data[0] = '+';
	} else 
		SetOprErr(true);
}

void LEM_DEDA::MinusPressed()

{
	if (State == 3){
			State++;
			Data[0] = '-';
	} else 
		SetOprErr(true);
}

void LEM_DEDA::ReadOutPressed()

{
	if (State == 3){
		SendKeyCode(18);
	} else 
		SetOprErr(true);

	Held = false;
}

void LEM_DEDA::HoldPressed()

{
	if (State == 3 || State == 9){
		SendKeyCode(18);
	} else 
		SetOprErr(true);

	Held = true;
}

void LEM_DEDA::NumberPressed(int n)

{

	switch(State){
		case 0:
		case 1:
		case 2:
			if (n > 7){
				SetOprErr(true);
				return;
			}
			Adr[State] = '0' + n;
			State++;
			return;
		case 3:
			SetOprErr(true);
			return;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			Data[State-3] = '0' + n;
			State++;
			return;
		case 9:
			SetOprErr(true);
			return;
	}
}
