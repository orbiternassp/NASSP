/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant, Matthias M?Eler

  ORBITER vessel module: LEM panel code

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
#include "IMU.h"
#include "dsky.h"

#include "LEM.h"
 
#define VIEWANGLE 34

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

static GDIParams g_Param;

void InitGParam(HINSTANCE hModule)

{
	g_Param.hDLL = hModule;

	// allocate GDI resources
	g_Param.font[0]  = CreateFont (-13, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");

	g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
	g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
	g_Param.brush[2] = CreateSolidBrush (RGB(154,154,154));  // Grey
	g_Param.brush[3] = CreateSolidBrush (RGB(3,3,3));  // Black

	g_Param.pen[0] = CreatePen (PS_SOLID, 1, RGB(224,224,224));
	g_Param.pen[1] = CreatePen (PS_SOLID, 3, RGB(164,164,164));
	g_Param.pen[2] = CreatePen (PS_SOLID, 1, RGB(255,0,0));
	g_Param.pen[3] = CreatePen (PS_SOLID, 3, RGB(255,0,0));
	g_Param.pen[4] = CreatePen (PS_SOLID, 3, RGB(0,0,0));

	g_Param.col[2] = oapiGetColour(154,154,154);
	g_Param.col[3] = oapiGetColour(3,3,3);
	g_Param.col[4] = oapiGetColour(255,0,255);
}

void FreeGParam()

{
	int i;
	// deallocate GDI resources
	for (i = 0; i < 2; i++) DeleteObject (g_Param.font[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.brush[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.pen[i]);
}

#define RETICLE_X_CENTER 268
#define RETICLE_Y_CENTER 268
#define RETICLE_RADIUS   268
#define RETICLE_SPLIT_ANGLE 0.05 // about 2.25 degrees
#define RETICLE_SCREW_NPTS 360

void DrawReticle (HDC hDC, double angle, int dimmer)
{
	HGDIOBJ oldObj;
	int xend,yend;
	// Set up Dimmer Pen
	HPEN pen = CreatePen(PS_SOLID,2,RGB(dimmer,64,64));
	oldObj = SelectObject (hDC, pen);
	// Draw crosshair vertical member
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle+PI));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle+PI));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	// Draw crosshair horizontal member
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * cos(angle));
	yend = RETICLE_Y_CENTER + (int)(RETICLE_RADIUS * sin(angle));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * cos(angle+PI));
	yend = RETICLE_Y_CENTER + (int)(RETICLE_RADIUS * sin(angle+PI));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	// Draw radial line #1
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle + RETICLE_SPLIT_ANGLE));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle + RETICLE_SPLIT_ANGLE));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	// Draw radial line #2
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle - RETICLE_SPLIT_ANGLE));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle - RETICLE_SPLIT_ANGLE));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	int i;
	double theta,b, r;
	b = - RETICLE_RADIUS / (2*PI);
	POINT ScrewPt[RETICLE_SCREW_NPTS];
	// Draw Archemedes screw #1
	for (i = 0; i < RETICLE_SCREW_NPTS; i++){
		theta = 2*PI / RETICLE_SCREW_NPTS * i;
		r = b*theta;
		ScrewPt[i].x = RETICLE_X_CENTER - (int)r*sin(theta+angle+RETICLE_SPLIT_ANGLE+PI);
		ScrewPt[i].y = RETICLE_Y_CENTER - (int)r*cos(theta+angle+RETICLE_SPLIT_ANGLE+PI);
	}
	Polyline (hDC, ScrewPt, RETICLE_SCREW_NPTS);
	// Draw Archemedes screw #2
	for (i = 0; i < RETICLE_SCREW_NPTS; i++){
		theta = 2*PI / RETICLE_SCREW_NPTS * i;
		r = b*theta;
		ScrewPt[i].x = RETICLE_X_CENTER - (int)r*sin(theta+angle-RETICLE_SPLIT_ANGLE+PI);
		ScrewPt[i].y = RETICLE_Y_CENTER - (int)r*cos(theta+angle-RETICLE_SPLIT_ANGLE+PI);
	}
	Polyline (hDC, ScrewPt, RETICLE_SCREW_NPTS);

	SelectObject(hDC, oldObj);
	DeleteObject(pen);
}

void LEM::RedrawPanel_AOTReticle(SURFHANDLE surf)
{
	HDC hDC = oapiGetDC (surf);
	DrawReticle (hDC, optics.OpticsReticle, optics.RetDimmer);
	oapiReleaseDC (surf,hDC);

}


//
// Initialise panel to default state.
//

void LEM::InitSwitches() {

	AbortSwitch.Register     (PSH, "AbortSwitch", false);
	AbortStageSwitch.Register(PSH, "AbortStageSwitch", false);
	AbortStageSwitchLight = false;

	EngineArmSwitch.Register(PSH, "EngineArmSwitch", THREEPOSSWITCH_CENTER);
	EngineDescentCommandOverrideSwitch.Register(PSH, "EngineDescentCommandOverrideSwitch", TOGGLESWITCH_DOWN);
	ModeControlPNGSSwitch.Register(PSH,"ModeControlPNGSSwitch", THREEPOSSWITCH_DOWN);
	ModeControlAGSSwitch.Register(PSH,"ModeControlAGSSwitch", THREEPOSSWITCH_DOWN);
	IMUCageSwitch.Register(PSH,"IMUCageSwitch", TOGGLESWITCH_DOWN);
	LeftXPointerSwitch.Register(PSH, "LeftXPointerSwitch", true);
	GuidContSwitch.Register(PSH, "GuidContSwitch", true);
	ModeSelSwitch.Register(PSH, "ModeSelSwitch", THREEPOSSWITCH_UP);
	AltRngMonSwitch.Register(PSH, "AltRngMonSwitch", TOGGLESWITCH_DOWN);
	RateErrorMonSwitch.Register(PSH, "RateErrorMonSwitch", TOGGLESWITCH_DOWN);
	AttitudeMonSwitch.Register(PSH, "AttitudeMonSwitch", true);
	ASCHeReg1TB.Register(PSH,"ASCHeReg1TB", true);
	ASCHeReg2TB.Register(PSH,"ASCHeReg2TB", true);
	DESHeReg1TB.Register(PSH,"DESHeReg1TB", true);
	DESHeReg2TB.Register(PSH,"DESHeReg2TB", false);
	ASCHeReg1Switch.Register(PSH,"ASCHeReg1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCHeReg2Switch.Register(PSH,"ASCHeReg2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DESHeReg1Switch.Register(PSH,"DESHeReg1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DESHeReg2Switch.Register(PSH,"DESHeReg2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ShiftTruSwitch.Register(PSH, "ShiftTruSwitch", true);
	RateScaleSwitch.Register(PSH, "RateScaleSwitch", TOGGLESWITCH_UP);
	ACAPropSwitch.Register(PSH, "ACAPropSwitch", true);
	RCSAscFeed1ATB.Register(PSH, "RCSAscFeed1ATB", true);
	RCSAscFeed2ATB.Register(PSH, "RCSAscFeed2ATB", false);
	RCSAscFeed1BTB.Register(PSH, "RCSAscFeed1BTB", true);
	RCSAscFeed2BTB.Register(PSH, "RCSAscFeed2BTB", false);
	RCSAscFeed1ASwitch.Register(PSH, "RCSAscFeed1ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSAscFeed2ASwitch.Register(PSH, "RCSAscFeed2ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSAscFeed1BSwitch.Register(PSH, "RCSAscFeed1BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSAscFeed2BSwitch.Register(PSH, "RCSAscFeed2BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad1ACmdEnableTB.Register(PSH, "RCSQuad1ACmdEnableTB", true);
	RCSQuad4ACmdEnableTB.Register(PSH, "RCSQuad4ACmdEnableTB", true);
	RCSQuad1BCmdEnableTB.Register(PSH, "RCSQuad1BCmdEnableTB", true);
	RCSQuad4BCmdEnableTB.Register(PSH, "RCSQuad4BCmdEnableTB", true);
	RCSQuad1ACmdEnableSwitch.Register(PSH, "RCSQuad1ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad4ACmdEnableSwitch.Register(PSH, "RCSQuad4ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad1BCmdEnableSwitch.Register(PSH, "RCSQuad1BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad4BCmdEnableSwitch.Register(PSH, "RCSQuad4BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad2ACmdEnableTB.Register(PSH, "RCSQuad2ACmdEnableTB", true);
	RCSQuad3ACmdEnableTB.Register(PSH, "RCSQuad3ACmdEnableTB", true);
	RCSQuad2BCmdEnableTB.Register(PSH, "RCSQuad2BCmdEnableTB", true);
	RCSQuad3BCmdEnableTB.Register(PSH, "RCSQuad3BCmdEnableTB", true);
	RCSQuad2ACmdEnableSwitch.Register(PSH, "RCSQuad2ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad3ACmdEnableSwitch.Register(PSH, "RCSQuad3ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad2BCmdEnableSwitch.Register(PSH, "RCSQuad2BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad3BCmdEnableSwitch.Register(PSH, "RCSQuad3BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSXFeedTB.Register(PSH, "RCSXFeedTB", false);
	RCSXFeedSwitch.Register(PSH, "RCSXFeedSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSMainSovATB.Register(PSH, "RCSMainSovATB", true);
	RCSMainSovBTB.Register(PSH, "RCSMainSovBTB", true);
	RCSMainSovASwitch.Register(PSH, "RCSMainSOVASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSMainSovBSwitch.Register(PSH, "RCSMainSOVBSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	THRContSwitch.Register(PSH, "THRContSwitch", true);
	MANThrotSwitch.Register(PSH, "MANThrotSwitch", true);
	ATTTranslSwitch.Register(PSH, "ATTTranslSwitch", TOGGLESWITCH_DOWN);
	BALCPLSwitch.Register(PSH, "BALCPLSwitch", true);
	QTYMonSwitch.Register(PSH, "QTYMonSwitch", THREEPOSSWITCH_DOWN);
	TempPressMonSwitch.Register(PSH, "TempPressMonSwitch", THREEPOSSWITCH_UP);
	RightRateErrorMonSwitch.Register(PSH, "RightRateErrorMonSwitch", TOGGLESWITCH_DOWN);
	RightAttitudeMonSwitch.Register(PSH, "RightAttitudeMonSwitch", TOGGLESWITCH_DOWN);
	RightACAPropSwitch.Register(PSH, "RightACAPropSwitch", TOGGLESWITCH_UP);
	LandingAntSwitch.Register(PSH, "LandingAntSwitch", THREEPOSSWITCH_UP);
	EngGimbalEnableSwitch.Register(PSH, "EngGimbalEnableSwitch", TOGGLESWITCH_UP);
	RadarTestSwitch.Register(PSH, "RadarTestSwitch",  THREEPOSSWITCH_CENTER);
	SlewRateSwitch.Register(PSH, "SlewRateSwitch", true);
	DeadBandSwitch.Register(PSH, "DeadBandSwitch", false);
	LMSuitTempMeter.Register(PSH,"LMSuitTempMeter",40,100,2);
	LMCabinTempMeter.Register(PSH,"LMCabinTempMeter",40,100,2);
	LMSuitPressMeter.Register(PSH,"LMSuitPressMeter",0,10,2);
	LMCabinPressMeter.Register(PSH,"LMCabinPressMeter",0,10,2);
	LMCabinCO2Meter.Register(PSH,"LMCabinCO2Meter",0,30,2);
	LMGlycolTempMeter.Register(PSH,"LMGlycolTempMeter",0,80,2);
	LMGlycolPressMeter.Register(PSH,"LMGlycolPressMeter",0,80,2);
	LMOxygenQtyMeter.Register(PSH,"LMOxygenQtyMeter",0,100,2);
	LMWaterQtyMeter.Register(PSH,"LMWaterQtyMeter",0,100,2);
	LMRCSATempInd.Register(PSH,"LMRCSATempInd",20,120,2);
	LMRCSBTempInd.Register(PSH,"LMRCSBTempInd",20,120,2);
	LMRCSAPressInd.Register(PSH,"LMRCSAPressInd",0,400,2);
	LMRCSBPressInd.Register(PSH,"LMRCSBPressInd",0,400,2);
	LMRCSAQtyInd.Register(PSH,"LMRCSAQtyInd",0,100,2);
	LMRCSBQtyInd.Register(PSH,"LMRCSBQtyInd",0,100,2);
	TempMonitorInd.Register(PSH,"TempMonitorInd",-100.0,200,2);
	EngineThrustInd.Register(PSH,"EngineThrustInd",0,100,2);
	CommandedThrustInd.Register(PSH,"CommandedThrustInd",0,100,2);
	MainFuelTempInd.Register(PSH,"MainFuelTempInd",40,200,2);
	MainFuelPressInd.Register(PSH,"MainFuelPressInd",0,300,2);
	MainOxidizerTempInd.Register(PSH,"MainOxidizerTempInd",40,200,2);
	MainOxidizerPressInd.Register(PSH,"MainOxidizerPressInd",0,300,2);	

	GyroTestLeftSwitch.Register(PSH, "GyroTestLeftSwitch",  THREEPOSSWITCH_UP);
	GyroTestRightSwitch.Register(PSH, "GyroTestRightSwitch",  THREEPOSSWITCH_CENTER);
	RollSwitch.Register(PSH, "RollSwitch",  THREEPOSSWITCH_UP);
	PitchSwitch.Register(PSH, "PitchSwitch",  THREEPOSSWITCH_UP);
	YawSwitch.Register(PSH, "YawSwitch",  THREEPOSSWITCH_UP);
	RCSSysQuad1Switch.Register(PSH, "RCSSysQuad1Switch",  THREEPOSSWITCH_CENTER);
	RCSSysQuad2Switch.Register(PSH, "RCSSysQuad2Switch",  THREEPOSSWITCH_CENTER);
	RCSSysQuad3Switch.Register(PSH, "RCSSysQuad3Switch",  THREEPOSSWITCH_CENTER);
	RCSSysQuad4Switch.Register(PSH, "RCSSysQuad4Switch",  THREEPOSSWITCH_CENTER);
	SidePanelsSwitch.Register(PSH, "SidePanelsSwitch", false);
	FloodSwitch.Register(PSH, "FloodSwitch",  THREEPOSSWITCH_DOWN);
	RightXPointerSwitch.Register(PSH, "RightXPointerSwitch", true);
	ExteriorLTGSwitch.Register(PSH, "ExteriorLTGSwitch", THREEPOSSWITCH_CENTER);
	LeftACA4JetSwitch.Register(PSH, "LeftACA4JetSwitch", TOGGLESWITCH_UP);
	LeftTTCATranslSwitch.Register(PSH, "LeftTTCATranslSwitch", TOGGLESWITCH_UP);
	RightACA4JetSwitch.Register(PSH, "RightACA4JetSwitch", TOGGLESWITCH_UP);
	RightTTCATranslSwitch.Register(PSH, "RightTTCATranslSwitch", TOGGLESWITCH_UP);
	RadarSlewSwitch.Register(PSH,"RadarSlewSwitch",FIVEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EventTimerCtlSwitch.Register(PSH,"EventTimerCtlSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerStartSwitch.Register(PSH,"EventTimerStartSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EventTimerMinuteSwitch.Register(PSH,"EventTimerMinuteSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EventTimerSecondSwitch.Register(PSH,"EventTimerSecondSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);

	HeliumMonRotary.AddPosition(0, 290);
	HeliumMonRotary.AddPosition(1, 315);
	HeliumMonRotary.AddPosition(2, 340);
	HeliumMonRotary.AddPosition(3,  20);
	HeliumMonRotary.AddPosition(4,  45);
	HeliumMonRotary.AddPosition(5,  70);
	HeliumMonRotary.AddPosition(6, 110);
	HeliumMonRotary.Register(PSH, "HeliumMonRotary", 0);

	TempPressMonRotary.AddPosition(0, 340);
	TempPressMonRotary.AddPosition(1,  20);
	TempPressMonRotary.AddPosition(2,  45);
	TempPressMonRotary.AddPosition(3,  70);
	TempPressMonRotary.Register(PSH, "TempPressMonRotary", 0);

	ClycolRotary.AddPosition(0,  45);
	ClycolRotary.AddPosition(1,  70);
	ClycolRotary.AddPosition(2, 110);
	ClycolRotary.Register(PSH, "ClycolRotary", 2);
	
	SuitFanRotary.AddPosition(0,  45);
	SuitFanRotary.AddPosition(1,  70);
	SuitFanRotary.AddPosition(2, 110);
	SuitFanRotary.Register(PSH, "SuitFanRotary", 1);

	QtyMonRotary.AddPosition(0, 340);
	QtyMonRotary.AddPosition(1,  20);
	QtyMonRotary.AddPosition(2,  45);
	QtyMonRotary.AddPosition(3,  70);
	QtyMonRotary.Register(PSH, "QtyMonRotary", 3);

	TestMonitorRotary.AddPosition(0, 315);
	TestMonitorRotary.AddPosition(1, 340);
	TestMonitorRotary.AddPosition(2,  20);
	TestMonitorRotary.AddPosition(3,  45);
	TestMonitorRotary.AddPosition(4,  70);
	TestMonitorRotary.AddPosition(5, 110);
	TestMonitorRotary.Register(PSH, "TestMonitorRotary", 0);

	RendezvousRadarRotary.AddPosition(0, 330);
	RendezvousRadarRotary.AddPosition(1,   0);
	RendezvousRadarRotary.AddPosition(2,  30);
	RendezvousRadarRotary.Register(PSH, "RendezvousRadarRotary", 1);

	TempMonitorRotary.AddPosition(0, 315);
	TempMonitorRotary.AddPosition(1, 340);
	TempMonitorRotary.AddPosition(2,  20);
	TempMonitorRotary.AddPosition(3,  45);
	TempMonitorRotary.AddPosition(4,  70);
	TempMonitorRotary.AddPosition(5, 110);
	TempMonitorRotary.AddPosition(6, 135);
	TempMonitorRotary.Register(PSH, "TempMonitorRotary", 1);

	FloodRotary.AddPosition(0, 240);
	FloodRotary.AddPosition(1, 270);
	FloodRotary.AddPosition(2, 300);
	FloodRotary.AddPosition(3, 330);
	FloodRotary.AddPosition(4, 360);
	FloodRotary.AddPosition(5,  30);
	FloodRotary.AddPosition(6,  60);
	FloodRotary.AddPosition(7,  90);
	FloodRotary.AddPosition(8, 120);
	FloodRotary.Register(PSH, "FloodRotary", 8);

	LampToneTestRotary.AddPosition(0, 250);
	LampToneTestRotary.AddPosition(1, 290);
	LampToneTestRotary.AddPosition(2, 315);
	LampToneTestRotary.AddPosition(3, 340);
	LampToneTestRotary.AddPosition(4,  20);
	LampToneTestRotary.AddPosition(5,  45);
	LampToneTestRotary.AddPosition(6,  70);
	LampToneTestRotary.AddPosition(7, 110);
	LampToneTestRotary.Register(PSH, "LampToneTestRotary", 0);

	EPSMonitorSelectRotary.AddPosition(0,210);
	EPSMonitorSelectRotary.AddPosition(1,240);
	EPSMonitorSelectRotary.AddPosition(2,270);
	EPSMonitorSelectRotary.AddPosition(3,300);
	EPSMonitorSelectRotary.AddPosition(4,330);
	EPSMonitorSelectRotary.AddPosition(5,0);
	EPSMonitorSelectRotary.AddPosition(6,30);
	EPSMonitorSelectRotary.AddPosition(7,60);
	EPSMonitorSelectRotary.AddPosition(8,90);
	EPSMonitorSelectRotary.AddPosition(9,120);

	EPSMonitorSelectRotary.SetSource(0, &EPSEDVoltSelect);
	if(stage < 2){
		EPSMonitorSelectRotary.SetSource(1, Battery1);
		EPSMonitorSelectRotary.SetSource(2, Battery2);
		EPSMonitorSelectRotary.SetSource(3, Battery3);
		EPSMonitorSelectRotary.SetSource(4, Battery4);
	}else{
		EPSMonitorSelectRotary.SetSource(1, NULL);
		EPSMonitorSelectRotary.SetSource(2, NULL);
		EPSMonitorSelectRotary.SetSource(3, NULL);
		EPSMonitorSelectRotary.SetSource(4, NULL);
	}
	EPSMonitorSelectRotary.SetSource(5, Battery5);
	EPSMonitorSelectRotary.SetSource(6, Battery6);
	EPSMonitorSelectRotary.SetSource(7, &CDRDCBusVoltCB);
	EPSMonitorSelectRotary.SetSource(8, &LMPDCBusVoltCB);
	EPSMonitorSelectRotary.SetSource(9, &ACVoltsAttenuator);
	
	EPSMonitorSelectRotary.Register(PSH,"EPSMonitorSelectRotary",0);
	
	EPSDCVoltMeter.Register(PSH,"EPSDCVoltMeter", 19, 42, 3);
	EPSDCAmMeter.Register(PSH,"EPSDCAmMeter", 0, 120, 3);
	DSCBattery1TB.Register(PSH, "DSCBattery1TB", 2);
	DSCBattery2TB.Register(PSH, "DSCBattery2TB", 0);
	DSCBattery3TB.Register(PSH, "DSCBattery3TB", 0);
	DSCBattery4TB.Register(PSH, "DSCBattery4TB", 2);
	ASCBattery5ATB.Register(PSH, "ASCBattery5ATB", 0);
	ASCBattery5BTB.Register(PSH, "ASCBattery5BTB", 0);
	ASCBattery6ATB.Register(PSH, "ASCBattery6ATB", 0);
	ASCBattery6BTB.Register(PSH, "ASCBattery6BTB", 0);
	DSCBattFeedTB.Register(PSH, "DSCBattFeedTB", 1);
	EPSInverterSwitch.Register(PSH,"EPSInverterSwitch",THREEPOSSWITCH_DOWN);
	EPSEDVoltSelect.Register(PSH,"EPSEDVoltSelect",THREEPOSSWITCH_CENTER);
	DSCSEBat1HVSwitch.Register(PSH, "DSCSEBat1HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCSEBat2HVSwitch.Register(PSH, "DSCSEBat2HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat3HVSwitch.Register(PSH, "DSCCDRBat3HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat4HVSwitch.Register(PSH, "DSCCDRBat4HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCSEBat1LVSwitch.Register(PSH, "DSCSEBat1LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCSEBat2LVSwitch.Register(PSH, "DSCSEBat2LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat3LVSwitch.Register(PSH, "DSCCDRBat3LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat4LVSwitch.Register(PSH, "DSCCDRBat4LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCBattFeedSwitch.Register(PSH, "DSCBattFeedSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat5SESwitch.Register(PSH, "ASCBat5SESwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat5CDRSwitch.Register(PSH, "ASCBat5CDRSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat6CDRSwitch.Register(PSH, "ASCBat6CDRSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat6SESwitch.Register(PSH, "ASCBat6SESwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	UpDataSquelchSwitch.Register(PSH,"UpDataSquelchSwitch",TOGGLESWITCH_UP);
	Panel12UpdataLinkSwitch.Register(PSH,"Panel12UpdataLinkSwitch",THREEPOSSWITCH_CENTER);
	Panel12AudioCtlSwitch.Register(PSH,"Panel12AudioCtlSwitch",TOGGLESWITCH_UP);
	Panel12AntTrackModeSwitch.Register(PSH,"Panel12AntTrackModeSwitch",THREEPOSSWITCH_CENTER);

	TimerContSwitch.Register(PSH,"TimerContSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);
	TimerSlewHours.Register(PSH,"TimerSlewHours",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	TimerSlewMinutes.Register(PSH,"TimerSlewMinutes",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	TimerSlewSeconds.Register(PSH,"TimerSlewSeconds",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	LtgORideAnunSwitch.Register(PSH,"LtgORideAnunSwitch",TOGGLESWITCH_DOWN);
	LtgORideNumSwitch.Register(PSH,"LtgORideNumSwitch",TOGGLESWITCH_DOWN);
	LtgORideIntegralSwitch.Register(PSH,"LtgORideIntegralSwitch",TOGGLESWITCH_DOWN);
	LtgSidePanelsSwitch.Register(PSH,"LtgSidePanelsSwitch",TOGGLESWITCH_DOWN);
	LtgFloodOhdFwdKnob.AddPosition(0, 240);
	LtgFloodOhdFwdKnob.AddPosition(1, 270);
	LtgFloodOhdFwdKnob.AddPosition(2, 300);
	LtgFloodOhdFwdKnob.AddPosition(3, 330);
	LtgFloodOhdFwdKnob.AddPosition(4, 360);
	LtgFloodOhdFwdKnob.AddPosition(5,  30);
	LtgFloodOhdFwdKnob.AddPosition(6,  60);
	LtgFloodOhdFwdKnob.AddPosition(7,  90);
	LtgFloodOhdFwdKnob.AddPosition(8, 120);
	LtgFloodOhdFwdKnob.Register(PSH, "LtgFloodOhdFwdKnob", 8);
	LtgAnunNumKnob.AddPosition(0, 240);
	LtgAnunNumKnob.AddPosition(1, 270);
	LtgAnunNumKnob.AddPosition(2, 300);
	LtgAnunNumKnob.AddPosition(3, 330);
	LtgAnunNumKnob.AddPosition(4, 360);
	LtgAnunNumKnob.AddPosition(5,  30);
	LtgAnunNumKnob.AddPosition(6,  60);
	LtgAnunNumKnob.AddPosition(7,  90);
	LtgAnunNumKnob.AddPosition(8, 120);
	LtgAnunNumKnob.Register(PSH, "LtgAnunNumKnob", 0);
	LtgIntegralKnob.AddPosition(0, 240);
	LtgIntegralKnob.AddPosition(1, 270);
	LtgIntegralKnob.AddPosition(2, 300);
	LtgIntegralKnob.AddPosition(3, 330);
	LtgIntegralKnob.AddPosition(4, 360);
	LtgIntegralKnob.AddPosition(5,  30);
	LtgIntegralKnob.AddPosition(6,  60);
	LtgIntegralKnob.AddPosition(7,  90);
	LtgIntegralKnob.AddPosition(8, 120);
	LtgIntegralKnob.Register(PSH, "LtgIntegralKnob", 0);

	EDMasterArm.Register(PSH,"EDMasterArm",TOGGLESWITCH_DOWN);
	EDDesVent.Register(PSH,"EDDesVent",TOGGLESWITCH_DOWN);
	EDASCHeSel.Register(PSH,"EDASCHeSel",THREEPOSSWITCH_CENTER);
	EDDesPrpIsol.Register(PSH,"EDDesPrpIsol",TOGGLESWITCH_DOWN);
	EDLGDeploy.Register(PSH,"EDLGDeploy",TOGGLESWITCH_DOWN);
	EDHePressRCS.Register(PSH,"EDHePressRCS",TOGGLESWITCH_DOWN);
	EDHePressDesStart.Register(PSH,"EDHePressDesStart",TOGGLESWITCH_DOWN);
	EDHePressASC.Register(PSH,"EDHePressASC",TOGGLESWITCH_DOWN);
	EDStage.Register(PSH,"EDStage", TOGGLESWITCH_DOWN);
	//EDStage.SetGuardResetsState(false);
	EDStageRelay.Register(PSH,"EDStageRelay",TOGGLESWITCH_DOWN);
	EDDesFuelVent.Register(PSH,"EDDesFuelVent",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EDDesOxidVent.Register(PSH,"EDDesOxidVent",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EDLGTB.Register(PSH, "EDLGTB", true);
	EDDesFuelVentTB.Register(PSH, "EDDesFuelVentTB", true);
	EDDesOxidVentTB.Register(PSH, "EDDesOxidVentTB", true);

	CDRAudSBandSwitch.Register(PSH,"CDRAudSBandSwitch",THREEPOSSWITCH_CENTER);
	CDRAudICSSwitch.Register(PSH,"CDRAudICSSwitch",THREEPOSSWITCH_CENTER);
	CDRAudRelaySwitch.Register(PSH,"CDRAudRelaySwitch",TOGGLESWITCH_DOWN);
	CDRAudVOXSwitch.Register(PSH,"CDRAudICSSwitch",THREEPOSSWITCH_CENTER);
	CDRAudioControlSwitch.Register(PSH,"CDRAudioControlSwitch",TOGGLESWITCH_UP);
	CDRAudVHFASwitch.Register(PSH,"CDRAudVHFASwitch",THREEPOSSWITCH_CENTER);
	CDRAudVHFBSwitch.Register(PSH,"CDRAudVHFBSwitch",THREEPOSSWITCH_CENTER);
	CDRAudSBandVol.Register(PSH,"CDRAudSBandVol",6,9);
	CDRAudVHFAVol.Register(PSH,"CDRAudVHFAVol",6,9);
	CDRAudVHFBVol.Register(PSH,"CDRAudVHFBVol",6,9);
	CDRAudICSVol.Register(PSH,"CDRAudICSVol",6,9);
	CDRAudMasterVol.Register(PSH,"CDRAudMasterVol",6,9);
	CDRAudVOXSens.Register(PSH,"CDRAudVOXSens",9,9);
	CDRCOASSwitch.Register(PSH,"CDRCOASSwitch",THREEPOSSWITCH_CENTER);

	LMPAudSBandSwitch.Register(PSH,"LMPAudSBandSwitch",THREEPOSSWITCH_CENTER);
	LMPAudICSSwitch.Register(PSH,"LMPAudICSSwitch",THREEPOSSWITCH_CENTER);
	LMPAudRelaySwitch.Register(PSH,"LMPAudRelaySwitch",TOGGLESWITCH_DOWN);
	LMPAudVOXSwitch.Register(PSH,"LMPAudICSSwitch",THREEPOSSWITCH_CENTER);
	LMPAudVHFASwitch.Register(PSH,"LMPAudVHFASwitch",THREEPOSSWITCH_CENTER);
	LMPAudVHFBSwitch.Register(PSH,"LMPAudVHFBSwitch",THREEPOSSWITCH_CENTER);
	LMPAudSBandVol.Register(PSH,"LMPAudSBandVol",6,9);
	LMPAudVHFAVol.Register(PSH,"LMPAudVHFAVol",6,9);
	LMPAudVHFBVol.Register(PSH,"LMPAudVHFBVol",6,9);
	LMPAudICSVol.Register(PSH,"LMPAudICSVol",6,9);
	LMPAudMasterVol.Register(PSH,"LMPAudMasterVol",6,9);
	LMPAudVOXSens.Register(PSH,"LMPAudVOXSens",9,9);
	SBandModulateSwitch.Register(PSH,"SBandModulateSwitch",TOGGLESWITCH_UP);
	SBandXCvrSelSwitch.Register(PSH,"SBandXCvrSelSwitch",THREEPOSSWITCH_CENTER);
	SBandPASelSwitch.Register(PSH,"SBandPASelSwitch",THREEPOSSWITCH_CENTER);
	SBandVoiceSwitch.Register(PSH,"SBandVoiceSwitch",THREEPOSSWITCH_CENTER);
	SBandPCMSwitch.Register(PSH,"SBandPCMSwitch",THREEPOSSWITCH_CENTER);
	SBandRangeSwitch.Register(PSH,"SBandRangeSwitch",THREEPOSSWITCH_CENTER);
	VHFAVoiceSwitch.Register(PSH,"VHFAVoiceSwitch",THREEPOSSWITCH_CENTER);
	VHFARcvrSwtich.Register(PSH,"VHFARcvrSwtich",TOGGLESWITCH_DOWN);
	VHFBVoiceSwitch.Register(PSH,"VHFBVoiceSwitch",THREEPOSSWITCH_CENTER);
	VHFBRcvrSwtich.Register(PSH,"VHFBRcvrSwtich",TOGGLESWITCH_DOWN);
	TLMBiomedSwtich.Register(PSH,"TLMBiomedSwtich",THREEPOSSWITCH_CENTER);
	TLMBitrateSwitch.Register(PSH,"TLMBitrateSwitch",TOGGLESWITCH_UP);
	VHFASquelch.Register(PSH,"VHFASquelch",3,9);
	VHFBSquelch.Register(PSH,"VHFBSquelch",3,9);
	TapeRecorderTB.Register(PSH, "TapeRecorderTB", false);
	TapeRecorderSwitch.Register(PSH,"TapeRecorderSwitch",TOGGLESWITCH_DOWN);
	AGSOperateSwitch.Register(PSH,"AGSOperateSwitch",THREEPOSSWITCH_DOWN);

	Panel12VHFAntSelKnob.AddPosition(0, 330);
	Panel12VHFAntSelKnob.AddPosition(1, 0);
	Panel12VHFAntSelKnob.AddPosition(2, 30);
	Panel12VHFAntSelKnob.Register(PSH, "Panel12VHFAntSelKnob", 0);
	Panel12SBandAntSelKnob.AddPosition(0, 300);
	Panel12SBandAntSelKnob.AddPosition(1, 330);
	Panel12SBandAntSelKnob.AddPosition(2, 0);
	Panel12SBandAntSelKnob.AddPosition(3, 30);
	Panel12SBandAntSelKnob.Register(PSH, "Panel12SBandAntSelKnob", 1);
	Panel12AntPitchKnob.AddPosition(0, 195);
	Panel12AntPitchKnob.AddPosition(1, 210);
	Panel12AntPitchKnob.AddPosition(2, 225);
	Panel12AntPitchKnob.AddPosition(3, 240);
	Panel12AntPitchKnob.AddPosition(4, 255);
	Panel12AntPitchKnob.AddPosition(5, 270);
	Panel12AntPitchKnob.AddPosition(6, 285);
	Panel12AntPitchKnob.AddPosition(7, 300);
	Panel12AntPitchKnob.AddPosition(8, 315);
	Panel12AntPitchKnob.AddPosition(9, 330);
	Panel12AntPitchKnob.AddPosition(10,345);
	Panel12AntPitchKnob.AddPosition(11,  0);
	Panel12AntPitchKnob.AddPosition(12, 15);
	Panel12AntPitchKnob.AddPosition(13, 30);
	Panel12AntPitchKnob.AddPosition(14, 45);
	Panel12AntPitchKnob.AddPosition(15, 60);
	Panel12AntPitchKnob.AddPosition(16, 75);
	Panel12AntPitchKnob.AddPosition(17, 90);
	Panel12AntPitchKnob.AddPosition(18,105);
	Panel12AntPitchKnob.AddPosition(19,120);
	Panel12AntPitchKnob.AddPosition(20,135);
	Panel12AntPitchKnob.AddPosition(21,150);
	Panel12AntPitchKnob.AddPosition(22,165);
	Panel12AntPitchKnob.Register(PSH, "Panel12AntPitchKnob", 0);
	Panel12AntYawKnob.AddPosition(0,270);
	Panel12AntYawKnob.AddPosition(1,285);
	Panel12AntYawKnob.AddPosition(2,300);
	Panel12AntYawKnob.AddPosition(3,315);
	Panel12AntYawKnob.AddPosition(4,330);
	Panel12AntYawKnob.AddPosition(5,345);
	Panel12AntYawKnob.AddPosition(6,  0);
	Panel12AntYawKnob.AddPosition(7, 15);
	Panel12AntYawKnob.AddPosition(8, 30);
	Panel12AntYawKnob.AddPosition(9, 45);
	Panel12AntYawKnob.AddPosition(10,60);
	Panel12AntYawKnob.AddPosition(11,75);
	Panel12AntYawKnob.AddPosition(12,90);
	Panel12AntYawKnob.Register(PSH, "Panel12AntYawKnob", 5);

	// CIRCUIT BREAKERS
	// Panel 11, Row 1
	SE_WND_HTR_AC_CB.Register(PSH,"SE_WND_HTR_AC_CB",0);
	HE_PQGS_PROP_DISP_AC_CB.Register(PSH,"HE_PQGS_PROP_DISP_AC_CB",0);
	SBD_ANT_AC_CB.Register(PSH,"SBD_ANT_AC_CB",0);
	ORDEAL_AC_CB.Register(PSH,"ORDEAL_AC_CB",0);
	AQS_AC_CB.Register(PSH,"AQS_AC_CB",0);
	AOT_LAMP_ACB_CB.Register(PSH,"AOT_LAMP_ACB_CB",0);
	LMP_FDAI_AC_CB.Register(PSH,"LMP_FDAI_AC_CB",0);
	NUM_LTG_AC_CB.Register(PSH,"NUM_LTG_AC_CB",0);
	AC_B_INV_2_FEED_CB.Register(PSH,"AC_B_INV_2_FEED_CB",0);
	AC_B_INV_1_FEED_CB.Register(PSH,"AC_B_INV_1_FEED_CB",0);
	AC_A_INV_2_FEED_CB.Register(PSH,"AC_A_INV_2_FEED_CB",0);
	AC_A_INV_1_FEED_CB.Register(PSH,"AC_A_INV_1_FEED_CB",0);
	AC_A_BUS_VOLT_CB.Register(PSH,"AC_A_BUS_VOLT_CB",0);
	CDR_WND_HTR_AC_CB.Register(PSH,"CDR_WND_HTR_AC_CB",0);
	TAPE_RCDR_AC_CB.Register(PSH,"TAPE_RCDR_AC_CB",0);
	AOT_LAMP_ACA_CB.Register(PSH,"AOT_LAMP_ACA_CB",0);
	RDZ_RDR_AC_CB.Register(PSH,"RDZ_RDR_AC_CB",0);
	DECA_GMBL_AC_CB.Register(PSH,"DECA_GMBL_AC_CB",0);
	INTGL_LTG_AC_CB.Register(PSH,"INTGL_LTG_AC_CB",0);

	// Panel 11, Row 2
	RCS_A_MAIN_SOV_CB.Register(PSH,"RCS_A_MAIN_SOV_CB",0);
	RCS_A_QUAD4_TCA_CB.Register(PSH,"RCS_A_QUAD4_TCA_CB",0);
	RCS_A_QUAD3_TCA_CB.Register(PSH,"RCS_A_QUAD3_TCA_CB",0);
	RCS_A_QUAD2_TCA_CB.Register(PSH,"RCS_A_QUAD2_TCA_CB",0);
	RCS_A_QUAD1_TCA_CB.Register(PSH,"RCS_A_QUAD1_TCA_CB",0);
	RCS_A_ISOL_VLV_CB.Register(PSH,"RCS_A_ISOL_VLV_CB",0);
	RCS_A_ASC_FEED_2_CB.Register(PSH,"RCS_A_ASC_FEED_2_CB",0);
	RCS_A_ASC_FEED_1_CB.Register(PSH,"RCS_A_ASC_FEED_1_CB",0);
	THRUST_DISP_CB.Register(PSH,"THRUST_DISP_CB",0);
	MISSION_TIMER_CB.Register(PSH,"MISSION_TIMER_CB",0);
	CDR_XPTR_CB.Register(PSH,"CDR_XPTR_CB",0);
	RNG_RT_ALT_RT_DC_CB.Register(PSH,"RNG_RT_ALT_RT_DC_CB",0);
	GASTA_DC_CB.Register(PSH,"GASTA_DC_CB",0);	
	CDR_FDAI_DC_CB.Register(PSH,"CDR_FDAI_DC_CB",0);
	COAS_DC_CB.Register(PSH,"COAS_DC_CB",0);
	ORDEAL_DC_CB.Register(PSH,"ORDEAL_DC_CB",0);
	RNG_RT_ALT_RT_AC_CB.Register(PSH,"RNG_RT_ALT_RT_AC_CB",0);
	GASTA_AC_CB.Register(PSH,"GASTA_AC_CB",0); 
	CDR_FDAI_AC_CB.Register(PSH,"CDR_FDAI_AC_CB",0);

	// Panel 11, Row 3
	PROP_DES_HE_REG_VENT_CB.Register(PSH,"PROP_DES_HE_REG_VENT_CB",0);
	HTR_RR_STBY_CB.Register(PSH,"HTR_RR_STBY_CB",1);
	HTR_RR_OPR_CB.Register(PSH,"HTR_RR_OPR_CB",0);
	HTR_LR_CB.Register(PSH,"HTR_LR_CB",1);
	HTR_DOCK_WINDOW_CB.Register(PSH,"HTR_DOCK_WINDOW_CB",0);
	HTR_AOT_CB.Register(PSH,"HTR_AOT_CB",0);
	INST_SIG_CONDR_1_CB.Register(PSH,"INST_SIG_CONDR_1_CB",0);
	CDR_SCS_AEA_CB.Register(PSH,"CDR_SCS_AEA_CB",0);
	CDR_SCS_ABORT_STAGE_CB.Register(PSH,"CDR_SCS_ABORT_STAGE_CB",0);
	CDR_SCS_ATCA_CB.Register(PSH,"CDR_SCS_ATCA_CB",0);
	CDR_SCS_AELD_CB.Register(PSH,"CDR_SCS_AELD_CB",0);
	SCS_ENG_CONT_CB.Register(PSH,"SCS_ENG_CONT_CB",0);
	SCS_ATT_DIR_CONT_CB.Register(PSH,"SCS_ATT_DIR_CONT_CB",0);
	SCS_ENG_START_OVRD_CB.Register(PSH,"SCS_ENG_START_OVRD_CB",0);
	SCS_DECA_PWR_CB.Register(PSH,"SCS_DECA_PWR_CB",0);
	EDS_CB_LG_FLAG.Register(PSH,"EDS_CB_LG_FLAG",0);
	EDS_CB_LOGIC_A.Register(PSH,"EDS_CB_LOGIC_A",0);
	CDR_LTG_UTIL_CB.Register(PSH,"CDR_LTG_UTIL_CB",1);
	CDR_LTG_ANUN_DOCK_COMPNT_CB.Register(PSH,"CDR_LTG_ANUN_DOCK_COMPNT_CB",1);

	// Panel 11, Row 4
	RCS_QUAD_4_CDR_HTR_CB.Register(PSH,"RCS_QUAD_4_CDR_HTR_CB",0);
	RCS_QUAD_3_CDR_HTR_CB.Register(PSH,"RCS_QUAD_3_CDR_HTR_CB",0);
	RCS_QUAD_2_CDR_HTR_CB.Register(PSH,"RCS_QUAD_2_CDR_HTR_CB",0);
	RCS_QUAD_1_CDR_HTR_CB.Register(PSH,"RCS_QUAD_1_CDR_HTR_CB",0);
	ECS_SUIT_FAN_1_CB.Register(PSH,"ECS_SUIT_FAN_1_CB",0);
	ECS_CABIN_FAN_1_CB.Register(PSH,"ECS_CABIN_FAN_1_CB",0);
	ECS_GLYCOL_PUMP_2_CB.Register(PSH,"ECS_GLYCOL_PUMP_2_CB",0);
	ECS_GLYCOL_PUMP_1_CB.Register(PSH,"ECS_GLYCOL_PUMP_1_CB",0);
	ECS_GLYCOL_PUMP_AUTO_XFER_CB.Register(PSH,"ECS_GLYCOL_PUMP_AUTO_XFER_CB",0);
	COMM_UP_DATA_LINK_CB.Register(PSH,"COMM_UP_DATA_LINK_CB",0);
	COMM_SEC_SBAND_XCVR_CB.Register(PSH,"COMM_SEC_SBAND_XCVR_CB",0);
	COMM_SEC_SBAND_PA_CB.Register(PSH,"COMM_SEC_SBAND_PA_CB",0);
	COMM_VHF_XMTR_B_CB.Register(PSH,"COMM_VHF_XMTR_B_CB",0);
	COMM_VHF_RCVR_A_CB.Register(PSH,"COMM_VHF_RCVR_A_CB",0);
	COMM_CDR_AUDIO_CB.Register(PSH,"COMM_CDR_AUDIO_CB",0);
	PGNS_SIG_STR_DISP_CB.Register(PSH,"PGNS_SIG_STR_DISP_CB",0);
	PGNS_LDG_RDR_CB.Register(PSH,"PGNS_LDG_RDR_CB",0);
	PGNS_RNDZ_RDR_CB.Register(PSH,"PGNS_RNDZ_RDR_CB",0);
	LGC_DSKY_CB.Register(PSH, "LGC_DSKY_CB", 0);
	IMU_SBY_CB.Register(PSH, "IMU_SBY_CB", 1);
	IMU_OPR_CB.Register(PSH, "IMU_OPR_CB", 0);

	// Panel 11, Row 5
	CDRBatteryFeedTieCB1.Register(PSH, "CDRBatteryFeedTieCB1", 1);
	CDRBatteryFeedTieCB2.Register(PSH, "CDRBatteryFeedTieCB2", 1);
	CDRCrossTieBalCB.Register(PSH, "CDRCrossTieBalCB", 1);
	CDRCrossTieBusCB.Register(PSH, "CDRCrossTieBusCB", 0);
	CDRXLunarBusTieCB.Register(PSH, "CDRXLunarBusTieCB", 0);
	CDRDesECAContCB.Register(PSH, "CDRDesECAContCB", 0);
	CDRDesECAMainCB.Register(PSH, "CDRDesECAMainCB", 1);
	CDRAscECAContCB.Register(PSH, "CDRAscECAContCB", 0);
	CDRAscECAMainCB.Register(PSH, "CDRAscECAMainCB", 0);
	CDRInverter1CB.Register(PSH,"CDRInverter1CB",0);
	CDRDCBusVoltCB.Register(PSH, "CDRDCBusVoltCB", 1);

	// Panel 16, Row 1
	LMP_EVT_TMR_FDAI_DC_CB.Register(PSH,"LMP_EVT_TMR_FDAI_DC_CB",0);
	SE_XPTR_DC_CB.Register(PSH,"SE_XPTR_DC_CB",0);
	RCS_B_ASC_FEED_1_CB.Register(PSH,"RCS_B_ASC_FEED_1_CB",0);
	RCS_B_ASC_FEED_2_CB.Register(PSH,"RCS_B_ASC_FEED_2_CB",0);
	RCS_B_ISOL_VLV_CB.Register(PSH,"RCS_B_ISOL_VLV_CB",0);
	RCS_B_QUAD1_TCA_CB.Register(PSH,"RCS_B_QUAD1_TCA_CB",0);
	RCS_B_QUAD2_TCA_CB.Register(PSH,"RCS_B_QUAD2_TCA_CB",0);
	RCS_B_QUAD3_TCA_CB.Register(PSH,"RCS_B_QUAD3_TCA_CB",0);
	RCS_B_QUAD4_TCA_CB.Register(PSH,"RCS_B_QUAD4_TCA_CB",0);
	RCS_B_CRSFD_CB.Register(PSH,"RCS_B_CRSFD_CB",0);
	RCS_B_TEMP_PRESS_DISP_FLAGS_CB.Register(PSH,"RCS_B_TEMP_PRESS_DISP_FLAGS_CB",0);
	RCS_B_PQGS_DISP_CB.Register(PSH, "RCS_B_PQGS_DISP_CB", 0);
	RCS_B_MAIN_SOV_CB.Register(PSH,"RCS_B_MAIN_SOV_CB",0);
	PROP_DISP_ENG_OVRD_LOGIC_CB.Register(PSH,"PROP_DISP_ENG_OVRD_LOGIC_CB",0);
	PROP_PQGS_CB.Register(PSH,"PROP_PQGS_CB",0);
	PROP_ASC_HE_REG_CB.Register(PSH,"PROP_ASC_HE_REG_CB",0);

	// Panel 16, Row 2
	LTG_FLOOD_CB.Register(PSH,"LTG_FLOOD_CB",1);
	LTG_TRACK_CB.Register(PSH,"LTG_TRACK_CB",0);
	LTG_ANUN_DOCK_COMPNT_CB.Register(PSH,"LTG_ANUN_DOCK_COMPNT_CB",0);
	LTG_MASTER_ALARM_CB.Register(PSH,"LTG_MASTER_ALARM_CB",0);
	EDS_CB_LOGIC_B.Register(PSH,"EDS_CB_LOGIC_B",1);
	SCS_AEA_CB.Register(PSH,"SCS_AEA_CB",0);
	SCS_ENG_ARM_CB.Register(PSH,"SCS_ENG_ARM_CB",0);
	SCS_ASA_CB.Register(PSH,"SCS_ASA_CB",1);
	SCS_AELD_CB.Register(PSH,"SCS_AELD_CB",0);
	SCS_ATCA_CB.Register(PSH,"SCS_ATCA_CB",0);
	SCS_ABORT_STAGE_CB.Register(PSH,"SCS_ABORT_STAGE_CB",0);
	SCS_ATCA_AGS_CB.Register(PSH,"SCS_ATCA_AGS_CB",0);
	SCS_DES_ENG_OVRD_CB.Register(PSH,"SCS_DES_ENG_OVRD_CB",0);
	INST_CWEA_CB.Register(PSH,"INST_CWEA_CB",0);
	INST_SIG_SENSOR_CB.Register(PSH,"INST_SIG_SENSOR_CB",0);
	INST_PCMTEA_CB.Register(PSH,"INST_PCMTEA_CB",0);
	INST_SIG_CONDR_2_CB.Register(PSH,"INST_SIG_CONDR_2_CB",0);
	ECS_SUIT_FLOW_CONT_CB.Register(PSH,"ECS_SUIT_FLOW_CONT_CB",0);

	// Panel 16, Row 3
	COMM_DISP_CB.Register(PSH,"COMM_DISP_CB",0);
	COMM_SE_AUDIO_CB.Register(PSH,"COMM_SE_AUDIO_CB",0);
	COMM_VHF_XMTR_A_CB.Register(PSH,"COMM_VHF_XMTR_A_CB",0);
	COMM_VHF_RCVR_B_CB.Register(PSH,"COMM_VHF_RCVR_B_CB",0);
	COMM_PRIM_SBAND_PA_CB.Register(PSH,"COMM_PRIM_SBAND_PA_CB",0);
	COMM_PRIM_SBAND_XCVR_CB.Register(PSH,"COMM_PRIM_SBAND_XCVR_CB",0);
	COMM_SBAND_ANT_CB.Register(PSH,"COMM_SBAND_ANT_CB",0);
	COMM_PMP_CB.Register(PSH,"COMM_PMP_CB",0);
	COMM_TV_CB.Register(PSH,"COMM_TV_CB",0);
	ECS_DISP_CB.Register(PSH,"ECS_DISP_CB",0);
	ECS_GLYCOL_PUMP_SEC_CB.Register(PSH,"ECS_GLYCOL_PUMP_SEC_CB",0);
	ECS_LGC_PUMP_CB.Register(PSH,"ECS_LGC_PUMP_CB",0);
	ECS_CABIN_FAN_CONT_CB.Register(PSH,"ECS_CABIN_FAN_CONT_CB",0);
	ECS_CABIN_REPRESS_CB.Register(PSH,"ECS_CABIN_REPRESS_CB",1);
	ECS_SUIT_FAN_2_CB.Register(PSH,"ECS_SUIT_FAN_2_CB",0);
	ECS_SUIT_FAN_DP_CB.Register(PSH,"ECS_SUIT_FAN_DP_CB",0);
	ECS_DIVERT_VLV_CB.Register(PSH,"ECS_DIVERT_VLV_CB",0);
	ECS_CO2_SENSOR_CB.Register(PSH,"ECS_CO2_SENSOR_CB",0);

	// Panel 16, Row 4
	RCS_QUAD_1_LMP_HTR_CB.Register(PSH,"RCS_QUAD_1_LMP_HTR_CB",0);
	RCS_QUAD_2_LMP_HTR_CB.Register(PSH,"RCS_QUAD_2_LMP_HTR_CB",0);
	RCS_QUAD_3_LMP_HTR_CB.Register(PSH,"RCS_QUAD_3_LMP_HTR_CB",0);
	RCS_QUAD_4_LMP_HTR_CB.Register(PSH,"RCS_QUAD_4_LMP_HTR_CB",0);
	HTR_DISP_CB.Register(PSH,"HTR_DISP_CB",0);
	HTR_SBD_ANT_CB.Register(PSH, "HTR_SBD_ANT_CB", 1);
	CAMR_SEQ_CB.Register(PSH,"CAMR_SEQ_CB",0);
	EPS_DISP_CB.Register(PSH,"EPS_DISP_CB",0);
	LMPDCBusVoltCB.Register(PSH, "LMPDCBusVoltCB", 1);
	LMPInverter2CB.Register(PSH,"LMPInverter2CB",0);
	LMPAscECAContCB.Register(PSH, "LMPAscECAContCB", 0);
	LMPAscECAMainCB.Register(PSH, "LMPAscECAMainCB", 0);
	LMPDesECAContCB.Register(PSH, "LMPDesECAContCB", 0);
	LMPDesECAMainCB.Register(PSH, "LMPDesECAMainCB", 1);
	LMPXLunarBusTieCB.Register(PSH, "LMPXLunarBusTieCB", 0);
	LMPCrossTieBusCB.Register(PSH, "LMPCrossTieBusCB", 0);
	LMPCrossTieBalCB.Register(PSH, "LMPCrossTieBalCB", 1);
	LMPBatteryFeedTieCB1.Register(PSH, "LMPBatteryFeedTieCB1", 1);
	LMPBatteryFeedTieCB2.Register(PSH, "LMPBatteryFeedTieCB2", 1);

	LEMCoas1Enabled = false;
	LEMCoas2Enabled = true;

	RRGyroSelSwitch.Register(PSH,"RRGyroSelSwitch",THREEPOSSWITCH_UP);

	//
	// Old stuff.
	//

	Cswitch1=false;
	Cswitch2=false;
	Cswitch4=false;
	Cswitch5=false;
	Cswitch6=false;
	Cswitch7=false;
	Cswitch8=false;
	Cswitch9=false;
	Sswitch1=false;
	Sswitch2=false;
	Sswitch4=false;
	Sswitch5=false;
	Sswitch6=false;
	Sswitch7=false;
	Sswitch8=false;
	Sswitch9=false;
	RPswitch1=false;
	RPswitch2=false;
	RPswitch3=false;
	RPswitch4=false;
	RPswitch5=false;
	RPswitch6=false;
	RPswitch7=false;
	RPswitch8=false;
	RPswitch9=false;
	RPswitch10=false;
	RPswitch11=false;
	RPswitch12=false;
	RPswitch13=false;
	RPswitch14=false;
	RPswitch15=false;
	RPswitch16=false;

	ATT2switch=false;
	ATT3switch=false;
	ATT1switch=false;
	LPswitch4=false;
	LPswitch5=false;
//	ENGARMswitch=ENGINE_ARMED_NONE;
	LDGswitch=true;

	DESHE1switch=true;
	DESHE2switch=true;

	ASCHE1switch=true;
	ASCHE1switch=true;

	QUAD1switch=false;
	QUAD2switch=false;
	QUAD3switch=false;
	QUAD4switch=false;
	QUAD5switch=false;
	QUAD6switch=false;
	QUAD7switch=false;
	QUAD8switch=false;

	AFEED1switch=true;
	AFEED2switch=true;
	AFEED3switch=true;
	AFEED4switch=true;

	ED1switch=false;
	ED2switch=false;
	ED4switch=false;
	ED5switch=false;
	ED6switch=false;

	ED7switch=false;
	ED8switch=false;
	ED9switch=false;

	GMBLswitch=false;

	ASCHE1switch=true;
	ASCHE2switch=true;

	CRSFDswitch=false;

	CABFswitch=false;

	PTTswitch=false;

	RCSS1switch=false;
	RCSS2switch=false;
	RCSS3switch=false;
	RCSS4switch=false;

	X1switch=true;

	RATE1switch=true;
	AT1switch=true;

	GUIDswitch=true;

	ALTswitch=true;

	SHFTswitch=true;

	ETC1switch=true;
	ETC2switch=true;
	ETC3switch=true;
	ETC4switch=true;

	PMON1switch=true;
	PMON2switch=true;

	ACAPswitch=false;

	RATE2switch=true;
	AT2switch=true;

//	DESEswitch=false;

	SLWRswitch=true;

	DBswitch=true;

	IMUCswitch=false;

	SPLswitch=false;

	X2switch=true;

	P41switch=false;
	P42switch=false;
	P43switch=false;
	P44switch=false;

	AUDswitch=true;
	RELswitch=false;

	CPswitch=false;

	HATCHswitch=false;

	EVAswitch=false;

	COASswitch=true;

	for (int i = 0; i < nsurf; i++)
		srf[i] = 0;
}

void LEM::RedrawPanel_Horizon (SURFHANDLE surf)
{
POINT pt[4];
	static double prange = RAD*30.0;
	static int size = 48, size2 = size*2;
	static int extent = (int)(size*prange);
	double bank = GetBank();
	double pitch = GetPitch();
	double pfrac = pitch/prange;
	double sinb = sin(bank), cosb = cos(bank);
	double a = tan(bank);
	double yl, yr, xb, xt, xlr, xll, ylr, yll;
	int i, iphi, n = 0;
	bool bl, br, bb, bt, bblue;

	if (cosb) { // horizon not vertical
		double b = pfrac/cosb;
		bl = (fabs(yl = -a+b) < 1.0); // left edge
		br = (fabs(yr =  a+b) < 1.0);  // right edge
		if (a) { // horizon not horizontal
			bb = (fabs(xb = ( 1.0-b)/a) < 1.0); // bottom edge
			bt = (fabs(xt = (-1.0-b)/a) < 1.0); // top edge
		} else { // horizon horizontal
			bb = bt = false;
		}
	} else { // horizon vertical
		bl = br = false;
		bb = bt = (fabs(xb = xt = pfrac) < 1.0);
	}
	if (bl) {
		pt[0].x = 0;
		pt[0].y = (int)(yl*size)+size;
		if (bt) {
			pt[1].x = (int)(xt*size)+size;
			pt[1].y = 0;
			pt[2].x = 0;
			pt[2].y = 0;
			n = 3;
			bblue = (cosb > 0.0);
		} else if (br) {
			pt[1].x = size2;
			pt[1].y = (int)(yr*size)+size;
			pt[2].x = size2;
			pt[2].y = 0;
			pt[3].x = 0;
			pt[3].y = 0;
			n = 4;
			bblue = (cosb > 0.0);
		} else if (bb) {
			pt[1].x = (int)(xb*size)+size;
			pt[1].y = size2;
			pt[2].x = 0;
			pt[2].y = size2;
			n = 3;
			bblue = (cosb < 0.0);
		}
	} else if (br) {
		pt[0].x = size2;
		pt[0].y = (int)(yr*size)+size;
		if (bt) {
			pt[1].x = (int)(xt*size)+size;
			pt[1].y = 0;
			pt[2].x = size2;
			pt[2].y = 0;
			n = 3;
			bblue = (cosb > 0.0);
		} else if (bb) {
			pt[1].x = (int)(xb*size)+size;
			pt[1].y = size2;
			pt[2].x = size2;
			pt[2].y = size2;
			n = 3;
			bblue = (cosb < 0.0);
		}
	} else if (bt && bb) {
		pt[0].x = (int)(xt*size)+size;
		pt[0].y = 0;
		pt[1].x = (int)(xb*size)+size;
		pt[1].y = size2;
		pt[2].x = 0;
		pt[2].y = size2;
		pt[3].x = 0;
		pt[3].y = 0;
		n = 4;
		bblue = ((xt-xb)*cosb > 0.0);
	}
	if (!n) bblue = (pitch < 0.0);
	oapiClearSurface (surf, bblue ? g_Param.col[3]:g_Param.col[2]);
	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, GetStockObject (BLACK_PEN));
	if (n >= 3) {
		SelectObject (hDC, g_Param.brush[bblue ? 2:3]);
		Polygon (hDC, pt, n);
		SelectObject (hDC, g_Param.pen[0]);
		MoveToEx (hDC, pt[0].x, pt[0].y, NULL); LineTo (hDC, pt[1].x, pt[1].y);
	}
	// bank indicator
	SelectObject (hDC, g_Param.pen[0]);
	SelectObject (hDC, GetStockObject (NULL_BRUSH));
	static double r1 = 40, r2 = 35;
	double sinb1 = sin(bank-0.1), cosb1 = cos(bank-0.1);
	double sinb2 = sin(bank+0.1), cosb2 = cos(bank+0.1);
	pt[0].x = (int)(r2*sinb1+0.5)+size; pt[0].y = -(int)(r2*cosb1+0.5)+size;
	pt[1].x = (int)(r1*sinb+0.5)+size;  pt[1].y = -(int)(r1*cosb+0.5)+size;
	pt[2].x = (int)(r2*sinb2+0.5)+size; pt[2].y = -(int)(r2*cosb2+0.5)+size;
	Polygon (hDC, pt, 3);

	// pitch ladder
	static double d = size*(10.0*RAD)/prange;
	static double ladderw = 14.0;
	double lwcosa = ladderw*cosb, lwsina = ladderw*sinb;
	double dsinb = d*sinb, dcosb = d*cosb;
	double phi0 = floor(pitch*DEG*0.1);
	double d0 = (pitch*DEG*0.1-phi0) * d, d1 = d0-4*d;
	// ladder
	xlr = lwcosa-d1*sinb, xll = -lwcosa-d1*sinb;
	ylr = lwsina+d1*cosb, yll = -lwsina+d1*cosb;
	for (iphi = (int)phi0+4, i = 0; i < 8; i++, iphi--) {
		if (iphi) {
			MoveToEx (hDC, size+(int)xll, size+(int)yll, NULL);
			LineTo   (hDC, size+(int)xlr, size+(int)ylr);
		}
		xlr -= dsinb, ylr += dcosb;
		xll -= dsinb, yll += dcosb;
	}
	oapiReleaseDC (surf, hDC);
	// labels
	lwcosa *= 1.6, lwsina *= 1.6;
	xlr = lwcosa-d1*sinb, xll = -lwcosa-d1*sinb;
	ylr = lwsina+d1*cosb, yll = -lwsina+d1*cosb;
	for (iphi = (int)phi0+4, i = 0; i < 8; i++, iphi--) {
		if (iphi) {
			int lb = abs(iphi)-1; if (lb >= 9) lb = 16-lb;
			oapiBlt (surf, srf[SRF_DIGITAL], size-5+(int)xlr, size-3+(int)ylr, 9*lb, 96, 9, 7, SURF_PREDEF_CK);
			oapiBlt (surf, srf[SRF_DIGITAL], size-5+(int)xll, size-3+(int)yll, 9*lb, 96, 9, 7, SURF_PREDEF_CK);
		}
		xlr -= dsinb, ylr += dcosb;
		xll -= dsinb, yll += dcosb;
	}

	// now overlay markings with transparent blt
	oapiBlt (surf, srf[SRF_DIGITAL], 0, 0, 0, 0, 96, 96, SURF_PREDEF_CK);
	// oapiBlt (surf, srf[5], 0, 0, 0, 0, 96, 96, SURF_PREDEF_CK);
}

void DrawNeedle (HDC hDC, int x, int y, double rad, double angle, HPEN pen0, HPEN pen1)

{
	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;
	oldObj = SelectObject (hDC, pen1);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, oldObj);
	oldObj = SelectObject (hDC, pen0);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
	SelectObject (hDC, oldObj);
}

void LEM::RedrawPanel_Thrust (SURFHANDLE surf)

{
	double DispValue;

	DispValue = GetEngineLevel(ENGINE_HOVER);

	oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
	oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
}

void LEM::RedrawPanel_XPointer (SURFHANDLE surf) {

	int ix, iy;
	double vx, vy;
	HDC hDC;

	//draw the crosspointers
	if((RateErrorMonSwitch.GetState() == 1) && (RR.IsPowered()) )
	{
		vx = RR.GetRadarTrunnionVel();
		vy = RR.GetRadarShaftVel();
	} else
		agc.GetHorizVelocity(vx, vy);
	ix = (int)(-3.0 * vx);
	if(ix < -60) ix = -60;
	if(ix > 60) ix = 60;
	iy = (int)(3.0 * vy);
	if(iy < -60) iy = -60;
	if(iy > 60 ) iy = 60;
	hDC = oapiGetDC(surf);
	SelectObject(hDC, GetStockObject(BLACK_PEN));
	MoveToEx(hDC, 0, 65 + ix, NULL);
	LineTo(hDC, 135, 65 + ix);
	MoveToEx(hDC, 67 + iy, 0, NULL);
	LineTo(hDC, 67 + iy, 131);
	oapiReleaseDC(surf, hDC);

}

void LEM::RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset) {

	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, g_Param.font[1]);
	SetTextColor (hDC, RGB(255, 255, 255));
	SetTextAlign (hDC, TA_CENTER);
	SetBkMode (hDC, TRANSPARENT);
	const char *label;
	for (int bt = 0; bt < 6; bt++) {
		if (label = oapiMFDButtonLabel (mfd, bt+side*6))
			TextOut (hDC, xoffset, 44 * bt + yoffset, label, strlen(label));
		else break;
	}
	oapiReleaseDC (surf, hDC);
}

void LEM::clbkMFDMode (int mfd, int mode) {

	switch (mfd) {
	case MFD_LEFT:		
		oapiTriggerPanelRedrawArea (LMPANEL_MAIN, AID_MFDLEFT);
		break;
	case MFD_RIGHT:		
		oapiTriggerPanelRedrawArea (LMPANEL_MAIN, AID_MFDRIGHT);
		break;
	}
}

void LEM::ReleaseSurfaces ()

{
	for (int i = 0; i < nsurf; i++)
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
}

void LEM::InitPanel (int panel)

{
//	switch (panel) {
//	case LMPANEL_MAIN: // LEM Main Panel
		srf[0]						= oapiCreateSurface (LOADBMP (IDB_ECSG));
		srf[SRF_INDICATOR]			= oapiCreateSurface (LOADBMP (IDB_INDICATOR));
		srf[SRF_NEEDLE]				= oapiCreateSurface (LOADBMP (IDB_NEEDLE1));
		srf[SRF_DIGITAL]			= oapiCreateSurface (LOADBMP (IDB_DIGITAL));
		srf[SRF_SWITCHUP]			= oapiCreateSurface (LOADBMP (IDB_SWITCHUP));
		// Unused surface 5 was
		// srf[5]						= oapiCreateSurface (LOADBMP (IDB_FDAI));
		srf[SRF_LIGHTS2]			= oapiCreateSurface (LOADBMP (IDB_LIGHTS2));
		srf[SRF_LEMSWITCH1]			= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH1));
		srf[SRF_LEMSWTICH3]			= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH3));
		// Unused surface 7 was
		// srf[7]						= oapiCreateSurface (LOADBMP (IDB_SWLEVER));
		srf[SRF_SECSWITCH]			= oapiCreateSurface (LOADBMP (IDB_SECSWITCH));
		// srf[9]						= oapiCreateSurface (LOADBMP (IDB_ABORT));
		// srf[10]						= oapiCreateSurface (LOADBMP (IDB_ANNUN));
		// srf[11]						= oapiCreateSurface (LOADBMP (IDB_LAUNCH));		
		srf[SRF_LMTWOPOSLEVER]		= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH2));
		// srf[12]						= oapiCreateSurface (LOADBMP (IDB_LV_ENG));
		// There was a conflict here between hardcoded index 13 and SRF_DSKY
		// Hardcoded index 13 was moved to SRF_LIGHTS2 (index 5)
		srf[SRF_DSKY]				= oapiCreateSurface (LOADBMP (IDB_DSKY_LIGHTS));
		// srf[14]						= oapiCreateSurface (LOADBMP (IDB_ALTIMETER));
		// srf[15]						= oapiCreateSurface (LOADBMP (IDB_ANLG_GMETER));
		// srf[16]						= oapiCreateSurface (LOADBMP (IDB_THRUST));
		// srf[17]					= oapiCreateSurface (LOADBMP (IDB_HEADING));
		srf[SRF_CONTACTLIGHT]		= oapiCreateSurface (LOADBMP (IDB_CONTACT));
		// srf[19] (SRF_THREEPOSSWITCH305) was hardcoded in several places but never actually loaded?
		// srf[SRF_THREEPOSSWITCH305]	= oapiCreateSurface (LOADBMP (IDB_CONTACT));
		// There was a conflict here between hardcoded index 20 and SRF_LMABORTBUTTON
		// Hardcoded index 20 was moved to SRF_LEMSWITCH3 (index 7)		
		srf[SRF_LMABORTBUTTON]		= oapiCreateSurface (LOADBMP (IDB_LMABORTBUTTON));
		srf[SRF_LMMFDFRAME]			= oapiCreateSurface (LOADBMP (IDB_LMMFDFRAME));
		srf[SRF_LMTHREEPOSLEVER]	= oapiCreateSurface (LOADBMP (IDB_LMTHREEPOSLEVER));
		srf[SRF_LMTHREEPOSSWITCH]	= oapiCreateSurface (LOADBMP (IDB_LMTHREEPOSSWITCH));
		srf[SRF_DSKYDISP]			= oapiCreateSurface (LOADBMP (IDB_DSKY_DISP));		
		srf[SRF_FDAI]	        	= oapiCreateSurface (LOADBMP (IDB_FDAI));
		srf[SRF_FDAIROLL]       	= oapiCreateSurface (LOADBMP (IDB_FDAI_ROLL));
		srf[SRF_CWSLIGHTS]			= oapiCreateSurface (LOADBMP (IDB_CWS_LIGHTS));
		srf[SRF_DSKYKEY]			= oapiCreateSurface (LOADBMP (IDB_DSKY_KEY));
		srf[SRF_LEMROTARY]			= oapiCreateSurface (LOADBMP (IDB_LEMROTARY));
		srf[SRF_FDAIOFFFLAG]       	= oapiCreateSurface (LOADBMP (IDB_FDAIOFFFLAG));
		srf[SRF_FDAINEEDLES]		= oapiCreateSurface (LOADBMP (IDB_FDAINEEDLES));
		srf[SRF_CIRCUITBRAKER]		= oapiCreateSurface (LOADBMP (IDB_CIRCUITBRAKER));
		srf[SRF_BORDER_34x29]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x29));
		srf[SRF_BORDER_34x61]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x61));
		srf[SRF_LEM_COAS1]			= oapiCreateSurface (LOADBMP (IDB_LEM_COAS1));
		srf[SRF_LEM_COAS2]			= oapiCreateSurface (LOADBMP (IDB_LEM_COAS2));
		srf[SRF_DCVOLTS]			= oapiCreateSurface (LOADBMP (IDB_LMDCVOLTS));
		srf[SRF_DCAMPS]				= oapiCreateSurface (LOADBMP (IDB_LMDCAMPS));
		srf[SRF_LMYAWDEGS]			= oapiCreateSurface (LOADBMP (IDB_LMYAWDEGS));
		srf[SRF_LMPITCHDEGS]		= oapiCreateSurface (LOADBMP (IDB_LMPITCHDEGS));
		srf[SRF_LMSIGNALSTRENGTH]	= oapiCreateSurface (LOADBMP (IDB_LMSIGNALSTRENGTH));
		srf[SRF_AOTRETICLEKNOB]     = oapiCreateSurface (LOADBMP (IDB_AOT_RETICLE_KNOB));
		srf[SRF_AOTSHAFTKNOB]       = oapiCreateSurface (LOADBMP (IDB_AOT_SHAFT_KNOB));
		srf[SRF_THUMBWHEEL_LARGEFONTS] = oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_LARGEFONTS));
		srf[SRF_FIVE_POS_SWITCH]	= oapiCreateSurface (LOADBMP (IDB_FIVE_POS_SWITCH));
		srf[SRF_RR_NOTRACK]         = oapiCreateSurface (LOADBMP (IDB_RR_NOTRACK));
		//srf[SRF_LEM_STAGESWITCH]	= oapiCreateSurface (LOADBMP (IDB_LEM_STAGESWITCH));
		srf[SRF_DIGITALDISP2]		= oapiCreateSurface (LOADBMP (IDB_DIGITALDISP2));
		srf[SRF_RADAR_TAPE]        = oapiCreateSurface (LOADBMP (IDB_RADAR_TAPE));

		//
		// Flashing borders.
		//

		srf[SRF_BORDER_34x29]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x29));
		srf[SRF_BORDER_34x61]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x61));
		srf[SRF_BORDER_55x111]		= oapiCreateSurface (LOADBMP (IDB_BORDER_55x111));
		srf[SRF_BORDER_46x75]		= oapiCreateSurface (LOADBMP (IDB_BORDER_46x75));
		srf[SRF_BORDER_39x38]		= oapiCreateSurface (LOADBMP (IDB_BORDER_39x38));
		srf[SRF_BORDER_92x40]		= oapiCreateSurface (LOADBMP (IDB_BORDER_92x40));
		srf[SRF_BORDER_34x33]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x33));
		srf[SRF_BORDER_29x29]		= oapiCreateSurface (LOADBMP (IDB_BORDER_29x29));
		srf[SRF_BORDER_34x31]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x31));
		srf[SRF_BORDER_50x158]		= oapiCreateSurface (LOADBMP (IDB_BORDER_50x158));
		srf[SRF_BORDER_38x52]		= oapiCreateSurface (LOADBMP (IDB_BORDER_38x52));
		srf[SRF_BORDER_34x34]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x34));
		srf[SRF_BORDER_90x90]		= oapiCreateSurface (LOADBMP (IDB_BORDER_90x90));
		srf[SRF_BORDER_84x84]		= oapiCreateSurface (LOADBMP (IDB_BORDER_84x84));
		srf[SRF_BORDER_70x70]		= oapiCreateSurface (LOADBMP (IDB_BORDER_70x70));
		srf[SRF_BORDER_23x20]		= oapiCreateSurface (LOADBMP (IDB_BORDER_23x20));
		srf[SRF_BORDER_78x78]		= oapiCreateSurface (LOADBMP (IDB_BORDER_78x78));
		srf[SRF_BORDER_32x160]		= oapiCreateSurface (LOADBMP (IDB_BORDER_32x160));
		srf[SRF_BORDER_72x72]		= oapiCreateSurface (LOADBMP (IDB_BORDER_72x72));
		srf[SRF_BORDER_75x64]		= oapiCreateSurface (LOADBMP (IDB_BORDER_75x64));
		srf[SRF_LEM_COAS1]			= oapiCreateSurface (LOADBMP (IDB_LEM_COAS1));
		srf[SRF_LEM_COAS2]			= oapiCreateSurface (LOADBMP (IDB_LEM_COAS2));
		srf[SRF_DEDA_KEY]			= oapiCreateSurface (LOADBMP (IDB_DEDA_KEY));
		srf[SRF_DEDA_LIGHTS]		= oapiCreateSurface (LOADBMP (IDB_DEDA_LIGHTS));



		//
		// Set color keys where appropriate.
		//

		oapiSetSurfaceColourKey (srf[0], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_NEEDLE], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_DIGITAL], 0);
		// oapiSetSurfaceColourKey (srf[5], g_Param.col[4]);
		// oapiSetSurfaceColourKey (srf[14], g_Param.col[4]);
		// oapiSetSurfaceColourKey (srf[15], g_Param.col[4]);
		// oapiSetSurfaceColourKey (srf[16], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_CONTACTLIGHT], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMABORTBUTTON],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMTHREEPOSLEVER],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMTWOPOSLEVER],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMTHREEPOSSWITCH],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_DSKYDISP],				g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_SWITCHUP],				g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LEMROTARY],			g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_CIRCUITBRAKER],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_FDAI],					g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_FDAIROLL],				g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_FDAIOFFFLAG],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_FDAINEEDLES],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LEM_COAS1],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LEM_COAS2],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LMYAWDEGS],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LMPITCHDEGS],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LMSIGNALSTRENGTH],		g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_LARGEFONTS],g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_FIVE_POS_SWITCH],		g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_RR_NOTRACK],	     	g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_RADAR_TAPE],	     	g_Param.col[4]);
		//oapiSetSurfaceColourKey	(srf[SRF_LEM_STAGESWITCH],		g_Param.col[4]);

		//		break;
		//
		// Borders need to set the center color to transparent so only the outline
		// is visible.
		//

		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x29], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x61], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_55x111], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_46x75], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_39x38], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_92x40], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x33], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_29x29], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x31], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_50x158], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_38x52], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x34], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_90x90], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_84x84], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_70x70], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_23x20], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_78x78], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_32x160], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_72x72], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_75x64], g_Param.col[4]);



//		break;	
//	case LMPANEL_RIGHTWINDOW: // LEM Right Window 
//	case LMPANEL_LEFTWINDOW: // LEM Left Window 
		
//		break;

//	}

	SetSwitches(panel);
}

bool LEM::clbkLoadPanel (int id) {

	//
	// Release all surfaces
	//
 	ReleaseSurfaces();

	//
	// Load panel background image
	//
	HBITMAP hBmp;

	switch(id) {
    case LMPANEL_MAIN:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_MAIN_PANEL));
		oapiSetPanelNeighbours(LMPANEL_LEFTWINDOW, LMPANEL_RIGHTWINDOW, LMPANEL_RNDZWINDOW, -1);
		break;

	case LMPANEL_RIGHTWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_RIGHT_WINDOW));
		oapiSetPanelNeighbours(LMPANEL_MAIN, LMPANEL_RIGHTPANEL, -1, -1);
		break;

    case LMPANEL_LEFTWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_LEFT_WINDOW));
		oapiSetPanelNeighbours(LMPANEL_LEFTPANEL, LMPANEL_MAIN, -1, LMPANEL_LPDWINDOW);
		break;

    case LMPANEL_LPDWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_LPD_WINDOW));
		oapiSetPanelNeighbours(-1, LMPANEL_MAIN, LMPANEL_LEFTWINDOW, -1);
		break;

	case LMPANEL_RNDZWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_RENDEZVOUS_WINDOW));
		oapiSetPanelNeighbours(-1, LMPANEL_AOTVIEW, -1, LMPANEL_MAIN);
		break;

	case LMPANEL_LEFTPANEL:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_LEFT_PANEL));
		oapiSetPanelNeighbours(-1, LMPANEL_LEFTWINDOW, -1, -1);
		break;

	case LMPANEL_AOTVIEW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_AOT_VIEW));
		oapiSetPanelNeighbours(LMPANEL_RNDZWINDOW, -1, -1, LMPANEL_MAIN);
		break;

	case LMPANEL_RIGHTPANEL:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_RIGHT_PANEL));
		oapiSetPanelNeighbours(LMPANEL_RIGHTWINDOW, -1, -1, -1);
		break;
	}

	MFDSPEC mfds_left  = {{ 183, 1577,  493, 1886}, 6, 6, 55, 44};
	MFDSPEC mfds_right = {{1188, 1577, 1498, 1886}, 6, 6, 55, 44};

	switch (id) {
	case LMPANEL_MAIN: // LEM Main panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterMFD (MFD_LEFT,  mfds_left);
		oapiRegisterMFD (MFD_RIGHT, mfds_right);
		
		fdaiLeft.RegisterMe(AID_FDAI_LEFT, 233, 625); // Was 135,625
		//fdaiRight.RegisterMe(AID_FDAI_RIGHT, 1103, 625);
		hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE (IDB_FDAI_ROLLINDICATOR));

		oapiRegisterPanelArea (AID_MFDLEFT,						    _R( 125, 1564,  550, 1918), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MFDRIGHT,					    _R(1130, 1564, 1555, 1918), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,              PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_LM_RCSIND,					    _R( 898,  245, 1147,  370), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_ECSIND_UPPER,				    _R(1202,  245, 1478,  370), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_ECSIND_LOWER,				    _R(1199,  439, 1357,  564), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_PROP_AND_ENGINE_IND,	    _R( 535,  428,  784,  553), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_CWS_LEFT,					    _R( 349,   54,  670,  180), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_CWS_RIGHT,				    _R(1184,   54, 1484,  180), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);

		//oapiRegisterPanelArea (AID_ABORT,							_R( 652,  855,  820,  972), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
        // 3 pos Engine Arm Lever
	    oapiRegisterPanelArea (AID_ENG_ARM,							_R( 263,  1078, 297, 1117), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		// 2 pos Descent Engine Command Override Lever
		oapiRegisterPanelArea (AID_DESCENT_ENGINE_SWITCH,			_R( 188,  1321, 230, 1361), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
        // 3 pos Mode control switches
	    oapiRegisterPanelArea (AID_MODECONTROL,						_R( 627,  1425, 875, 1470), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		// DSKY		
		oapiRegisterPanelArea (AID_DSKY_DISPLAY,					_R( 860, 1560,  965, 1736), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_LIGHTS,						_R( 716, 1565,  818, 1734), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_KEY,						_R( 696, 1755,  986, 1878), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_MISSION_CLOCK,					_R( 171,  286,  313,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EVENT_TIMER,						_R( 387,  286,  468,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUEL_DIGIT,						_R( 555,  245,  594,  319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_CONTACTLIGHT1,					_R( 420,  426,  468,  474), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CONTACTLIGHT2,					_R(1450, 1221, 1498, 1261), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTXPOINTERSWITCH,				_R( 428,  515,  462,  544), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GUIDCONTSWITCHROW,				_R( 759,  627,  794,  823), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMONITORSWITCHES,				_R( 149,  712,  183,  824), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FDAILOWERSWITCHROW,				_R( 236,  920,  412,  959), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_REG_CONTROLS_LEFT,			_R( 452,  918,  488, 1125), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_REG_CONTROLS_RIGHT,			_R( 526,  918,  562, 1125), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ENGINETHRUSTCONTSWITCHES,		_R( 309, 1006,  418, 1117), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PROPELLANTSWITCHES,				_R( 582,  991,  644, 1112), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_HELIUMMONROTARY,					_R( 682,  992,  766, 1076), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTMONITORSWITCHES,			_R(1498,  712, 1532,  824), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPPRESSMONROTARY,				_R( 886, 1002,  970, 1086), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ACAPROPSWITCH,					_R(1110, 1012, 1144, 1051), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_ASC_FEED_TALKBACKS,			_R( 905,  440, 1142,  463), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_ASC_FEED_SWITCHES,		    _R( 900,  493, 1147,  532), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_14_TALKBACKS,	_R( 905,  589, 1142,  612), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_14_SWITCHES,	_R( 900,  642, 1148,  682), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_23_TALKBACKS,	_R( 905,  715, 1142,  738), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_23_SWITCHES,	_R( 900,  768, 1147,  807), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_XFEED_TALKBACK,				_R( 906,  871,  929,  894), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_XFEED_SWITCH,				_R( 900,  921,  934,  960), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_SOV_TALKBACKS,				_R(1045,  871, 1138,  894), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_SOV_SWITCHES,			    _R(1039,  922, 1144,  961), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CLYCOLSUITFANROTARIES,			_R(1196,  926, 1280, 1130), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_QTYMONROTARY,					_R(1382,  986, 1466, 1070), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ENGGIMBALENABLESWITCH,			_R( 199, 1227,  233, 1268), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RADARANTTESTSWITCHES,			_R( 269, 1227,  304, 1361), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TESTMONITORROTARY,				_R( 214, 1410,  298, 1494), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SLEWRATESWITCH,				    _R( 370, 1364,  404, 1393), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RENDEZVOUSRADARROTARY,		    _R( 466, 1387,  550, 1471), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SATBCONTSWITCHES,				_R( 627, 1221,  852, 1260), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ATTITUDECONTROLSWITCHES,			_R( 627, 1322,  852, 1351), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPMONITORROTARY,				_R( 906, 1387,  990, 1471), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPMONITORIND,					_R( 928, 1219,  976, 1344), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCSSYSQUADSWITCHES,				_R(1069, 1331, 1161, 1455), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LIGHTINGSWITCHES,				_R(1248, 1222, 1282, 1319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOODROTARY,						_R(1338, 1263, 1422, 1347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LAMPTONETESTROTARY,				_R(1270, 1405, 1354, 1489), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTXPOINTERSWITCH,				_R(1472, 1318, 1506, 1347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EXTERIORLTGSWITCH,				_R(1451, 1432, 1485, 1461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL4LEFTSWITCHROW,				_R( 584, 1614,  618, 1794), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL4RIGHTSWITCHROW,			_R(1062, 1614, 1096, 1794), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_XPOINTERCDR,						_R( 163,  426,  300,  559), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	          PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_XPOINTERLMP,						_R(1400,  426, 1537,  559), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	          PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMRADARSIGNALSTRENGTH,			_R( 342, 1229,  433, 1319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	          PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMRADARSLEWSWITCH,			    _R( 367, 1433,  408, 1472), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EVENT_TIMER_SWITCHES,			_R(1013, 1233, 1214, 1264), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RR_NOTRACK,    					_R( 490,  1300, 524, 1334), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RANGE_TAPE,    					_R( 542,  660,  586,  823), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RATE_TAPE,    					_R( 593,  660,  628,  823), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;	
		
	case LMPANEL_RIGHTWINDOW: // LEM Right Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_XPOINTERLMP,					_R( 235, 246, 372,  379), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	        PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTMONITORSWITCHES,		_R( 333, 532, 368,  645), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CLYCOLSUITFANROTARIES,		_R(  31, 746, 116,  951), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_QTYMONROTARY,				_R( 215, 806, 300,  891), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_LEFTWINDOW: // LEM Left Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		oapiRegisterPanelArea (AID_LEM_COAS2,					_R( 555,    0, 1095,  540), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MISSION_CLOCK,				_R(1335,  106, 1477,  130), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EVENT_TIMER,					_R(1551,  106, 1632,  128), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CONTACTLIGHT1,				_R(1584,  246, 1633,  295), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTXPOINTERSWITCH,			_R(1592,  335, 1627,  365), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMONITORSWITCHES,			_R(1313,  532, 1347,  645), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_XPOINTERCDR,					_R(1327,  246, 1464,  379), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FDAILOWERSWITCHROW,			_R(1400,  740, 1576,  780), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ENGINETHRUSTCONTSWITCHES,	_R(1472,  826, 1582,  938), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_REG_CONTROLS_LEFT,		_R(1616,  738, 1652,  945), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		// 3 pos Engine Arm Lever
	    oapiRegisterPanelArea (AID_ENG_ARM,						_R(1427,  898, 1461,  937), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_LPDWINDOW: // LDP Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		oapiRegisterPanelArea (AID_XPOINTER,		_R(822,  35,  959, 168), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CONTACTLIGHT1,	_R(955, 713, 1004, 762), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE * RAD), cos(VIEWANGLE * RAD)));			
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_RNDZWINDOW: // LEM Rendezvous Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LEM_COAS1,				_R( 739, 0, 1570, 831), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,					   PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 1.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_LEFTPANEL: // LEM Left Panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW1,					_R( 264,  85,  1513,  115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW2,					_R( 264,  258, 1513,  288), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW3,					_R( 264,  431, 1513,  461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW4,					_R( 264,  604, 1637,  634), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW5,					_R( 264,  777,  996,  807), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_LEM_PANEL_8,					    _R( 511,  916, 1654, 1258), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_LEM_PANEL_5,					    _R( 1080,1300, 1640, 1620), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);		

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_RIGHTPANEL: // LEM Right Panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LM_EPS_DC_VOLTMETER,             _R( 110,  706, 209,  804), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EPS_DC_AMMETER,               _R( 110,  818, 209,  916), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EPS_LEFT_CONTROLS,            _R( 314,  728, 542,  913), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW1,					_R( 174,  85,  1223, 115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW2,					_R( 174,  258, 1351, 288), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW3,					_R( 174,  431, 1415, 461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW4,					_R( 174,  604, 1415, 634), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_BATTERY_TALKBACKS,	        _R( 573,  742, 888,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_HIGH_VOLTAGE_SWITCHES,	    _R( 568,  791, 895,  834), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_LOW_VOLTAGE_SWITCHES,	    _R( 568,  867, 824,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ASC_BATTERY_TALKBACKS,		    _R( 955,  767, 1199,  790), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ASC_BATTERY_SWITCHES,		    _R( 949,  825, 1280,  858), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_AUDIO_CONT_SWITCH,		_R(  99, 1054,  133, 1083), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_UPDATA_LINK_SWITCH,		_R(  71, 1199,  105, 1229), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_AUDIO_CONTROLS,			_R( 175,  968,  438, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_SWITCHES_ROW1,		_R( 452,  994,  923, 1024), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_SWITCHES_ROW2,		_R( 568, 1097,  928, 1128), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_SWITCHES_ROW3,		_R( 636, 1189,  926, 1271), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_AGS_OPERATE_SWITCH,			_R(  22, 1571,   59, 1611), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMYAWDEGS,						_R( 1104, 962, 1196, 1053), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMPITCHDEGS,						_R( 984,  962, 1076, 1053), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMSIGNALSTRENGTH,				_R( 1053, 1079, 1145, 1170), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_PITCH_KNOB,		_R( 974,  1171, 1058, 1255), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_YAW_KNOB,		_R( 1128, 1180, 1213, 1265), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_TRACK_MODE_SWITCH,_R( 976, 1092, 1010, 1123), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_VHF_SEL_KNOB,	_R( 1252,  994, 1337, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_SBD_SEL_KNOB,	_R( 1252, 1158, 1337, 1243), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_DISP,					_R(  207, 1408,  341, 1430), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_ADR,						_R(  231, 1363,  289, 1385), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_KEYS,					_R(  136, 1446,  360, 1623), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_LIGHTS,					_R(  144, 1406,  191, 1432), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				 PANEL_MAP_BACKGROUND);
	
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_AOTVIEW: // LEM Alignment Optical Telescope View
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_AOT_RETICLE,                      _R( 572, 257, 1107,  792),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,						PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_AOT_RETICLE_KNOB,                 _R(1334, 694, 1409, 1021),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,      PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_AOT_SHAFT_KNOB,                   _R(1340,   0, 1403,  156),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,						PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RR_GYRO_SEL_SWITCH,               _R( 207,  66,  242,   96),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,						PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(1.0, 0.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		break;
	}

	//
	// Initialize surfaces and switches
	//
	InitPanel(id);

	//
	// Change to desired panel next timestep.
	//
    if (!InPanel && id != PanelId) {	 
		CheckPanelIdInTimestep = true;
	} else {
	    PanelId = id;  
	}
	InPanel = true; 
	InVC = false;

	//
	// Set view parameter
	//
	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetView();

	return hBmp != NULL;
}

void LEM::SetSwitches(int panel) {

	MainPanel.Init(0, this, &soundlib, this);

	switch(panel){
		case LMPANEL_MAIN:
		case LMPANEL_LEFTWINDOW:
		case LMPANEL_RIGHTWINDOW:

			MainPropAndEngineIndRow.Init(AID_MAIN_PROP_AND_ENGINE_IND, MainPanel);
			EngineThrustInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);
			CommandedThrustInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);
			MainFuelTempInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);
			MainFuelPressInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);
			MainOxidizerTempInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);
			MainOxidizerPressInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);

			RCSIndicatorRow.Init(AID_LM_RCSIND, MainPanel);
			LMRCSATempInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
			LMRCSBTempInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
			LMRCSAPressInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
			LMRCSBPressInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
			LMRCSAQtyInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
			LMRCSBQtyInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);

			ECSUpperIndicatorRow.Init(AID_LM_ECSIND_UPPER, MainPanel);
			LMSuitTempMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
			LMCabinTempMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
			LMSuitPressMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
			LMCabinPressMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
			LMCabinCO2Meter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);

			ECSLowerIndicatorRow.Init(AID_LM_ECSIND_LOWER, MainPanel);
			LMGlycolTempMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);
			LMGlycolPressMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);
			LMOxygenQtyMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);
			LMWaterQtyMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);

			AbortSwitchesRow.Init(AID_ABORT, MainPanel);
			AbortSwitch.Init     ( 0, 0, 72, 72, srf[SRF_LMABORTBUTTON], srf[SRF_BORDER_72x72], AbortSwitchesRow, 0, 64);
			AbortStageSwitch.Init(78, 4, 75, 64, srf[SRF_LMABORTBUTTON], srf[SRF_BORDER_75x64], AbortSwitchesRow);

			EngineArmSwitchesRow.Init(AID_ENG_ARM, MainPanel);
			EngineArmSwitch.Init (0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], EngineArmSwitchesRow);

			LeftXPointerSwitchRow.Init(AID_LEFTXPOINTERSWITCH, MainPanel);
			LeftXPointerSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LeftXPointerSwitchRow);

			GuidContSwitchRow.Init(AID_GUIDCONTSWITCHROW, MainPanel);
			GuidContSwitch.Init (0,   0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], GuidContSwitchRow);
			ModeSelSwitch.Init  (0,  83, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], GuidContSwitchRow);
			AltRngMonSwitch.Init(0, 167, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], GuidContSwitchRow);

			LeftMonitorSwitchRow.Init(AID_LEFTMONITORSWITCHES, MainPanel);
			RateErrorMonSwitch.Init(0,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LeftMonitorSwitchRow);
			AttitudeMonSwitch.Init (0, 83, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LeftMonitorSwitchRow);

			// 354,  918,  463, 1124
			MPSRegControlLeftSwitchRow.Init(AID_MPS_REG_CONTROLS_LEFT, MainPanel);
			ASCHeReg1TB.Init(6,   0, 23, 23, srf[SRF_INDICATOR], MPSRegControlLeftSwitchRow);
			DESHeReg1TB.Init(7, 134, 23, 23, srf[SRF_INDICATOR], MPSRegControlLeftSwitchRow);
			ASCHeReg1Switch.Init(0,  43, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlLeftSwitchRow);
			DESHeReg1Switch.Init(0, 177, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlLeftSwitchRow);

			MPSRegControlRightSwitchRow.Init(AID_MPS_REG_CONTROLS_RIGHT, MainPanel);
			ASCHeReg2TB.Init(6,   0, 23, 23, srf[SRF_INDICATOR], MPSRegControlRightSwitchRow);
			DESHeReg2TB.Init(7, 134, 23, 23, srf[SRF_INDICATOR], MPSRegControlRightSwitchRow);
			ASCHeReg2Switch.Init(0,  43, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlRightSwitchRow);
			DESHeReg2Switch.Init(0, 177, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlRightSwitchRow);			

			FDAILowerSwitchRow.Init(AID_FDAILOWERSWITCHROW, MainPanel);
			ShiftTruSwitch.Init (  0,  5, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FDAILowerSwitchRow);
			RateScaleSwitch.Init( 73,  5, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FDAILowerSwitchRow);
			ACAPropSwitch.Init  (142,  0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], FDAILowerSwitchRow);

			EngineThrustContSwitchRow.Init(AID_ENGINETHRUSTCONTSWITCHES, MainPanel);
			THRContSwitch.Init  (  0,  0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineThrustContSwitchRow);
			MANThrotSwitch.Init ( 69,  0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineThrustContSwitchRow);
			ATTTranslSwitch.Init( 20, 77, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EngineThrustContSwitchRow);
			BALCPLSwitch.Init   ( 75, 72, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineThrustContSwitchRow);

			PropellantSwitchRow.Init(AID_PROPELLANTSWITCHES, MainPanel);
			QTYMonSwitch.Init      ( 0,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], PropellantSwitchRow);
			TempPressMonSwitch.Init(28, 92, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], PropellantSwitchRow);

			HeliumMonRotaryRow.Init(AID_HELIUMMONROTARY, MainPanel);
			HeliumMonRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], HeliumMonRotaryRow);

			RightMonitorSwitchRow.Init(AID_RIGHTMONITORSWITCHES, MainPanel);
			RightRateErrorMonSwitch.Init(0,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RightMonitorSwitchRow);
			RightAttitudeMonSwitch.Init (0, 83, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RightMonitorSwitchRow);

			TempPressMonRotaryRow.Init(AID_TEMPPRESSMONROTARY, MainPanel);
			TempPressMonRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], TempPressMonRotaryRow);

			RightACAPropSwitchRow.Init(AID_ACAPROPSWITCH, MainPanel);
			RightACAPropSwitch.Init(  0,  0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], RightACAPropSwitchRow);

			// DS20090901 STILL AT IT...
			RCSAscFeedTBSwitchRow.Init(AID_RCS_ASC_FEED_TALKBACKS, MainPanel);
			RCSAscFeed1ATB.Init(  0, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow);
			RCSAscFeed2ATB.Init( 70, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow);
			RCSAscFeed1BTB.Init(144, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow);
			RCSAscFeed2BTB.Init(214, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow);
			RCSAscFeedSwitchRow.Init(AID_RCS_ASC_FEED_SWITCHES, MainPanel);
			RCSAscFeed1ASwitch.Init(  0,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
			RCSAscFeed2ASwitch.Init( 69,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
			RCSAscFeed1BSwitch.Init(144,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
			RCSAscFeed2BSwitch.Init(213,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
			RCSQuad14TBSwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_14_TALKBACKS, MainPanel);
			RCSQuad1ACmdEnableTB.Init(  0, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad14TBSwitchRow);
			RCSQuad4ACmdEnableTB.Init( 70, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad14TBSwitchRow);
			RCSQuad1BCmdEnableTB.Init(144, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad14TBSwitchRow);
			RCSQuad4BCmdEnableTB.Init(214, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad14TBSwitchRow);
			RCSQuad14SwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_14_SWITCHES, MainPanel);
			RCSQuad1ACmdEnableSwitch.Init(  0,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow);
			RCSQuad4ACmdEnableSwitch.Init( 69,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow);
			RCSQuad1BCmdEnableSwitch.Init(144,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow);
			RCSQuad4BCmdEnableSwitch.Init(213,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow);
			RCSQuad23TBSwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_23_TALKBACKS, MainPanel);
			RCSQuad2ACmdEnableTB.Init(  0, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad23TBSwitchRow);
			RCSQuad3ACmdEnableTB.Init( 70, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad23TBSwitchRow);
			RCSQuad2BCmdEnableTB.Init(144, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad23TBSwitchRow);
			RCSQuad3BCmdEnableTB.Init(214, 0, 23, 23, srf[SRF_INDICATOR], RCSQuad23TBSwitchRow);
			RCSQuad23SwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_23_SWITCHES, MainPanel);
			RCSQuad2ACmdEnableSwitch.Init(  0,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow);
			RCSQuad3ACmdEnableSwitch.Init( 69,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow);
			RCSQuad2BCmdEnableSwitch.Init(144,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow);
			RCSQuad3BCmdEnableSwitch.Init(213,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow);
			RCSXfeedTBSwitchRow.Init(AID_RCS_XFEED_TALKBACK, MainPanel);
			RCSXFeedTB.Init(  0, 0, 23, 23, srf[SRF_INDICATOR], RCSXfeedTBSwitchRow);
			RCSXfeedSwitchRow.Init(AID_RCS_XFEED_SWITCH, MainPanel);
			RCSXFeedSwitch.Init(  0,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSXfeedSwitchRow);

			// DS20060406 HERE WE GO!
			RCSMainSOVTBRow.Init(AID_MAIN_SOV_TALKBACKS, MainPanel);
			RCSMainSovATB.Init(0, 0, 23, 23, srf[SRF_INDICATOR], RCSMainSOVTBRow, LEM_RCS_MAIN_SOV_A, this);
			RCSMainSovBTB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], RCSMainSOVTBRow, LEM_RCS_MAIN_SOV_B, this);

			RCSMainSOVSwitchRow.Init(AID_MAIN_SOV_SWITCHES, MainPanel);
			RCSMainSovASwitch.Init  ( 0,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSMainSOVSwitchRow, this, LEM_RCS_MAIN_SOV_A, &RCSMainSovATB);
			RCSMainSovBSwitch.Init  (71,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSMainSOVSwitchRow, this, LEM_RCS_MAIN_SOV_B, &RCSMainSovBTB);

			ClycolSuitFanRotaryRow.Init(AID_CLYCOLSUITFANROTARIES, MainPanel);
			ClycolRotary.Init (0,   0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], ClycolSuitFanRotaryRow);
			SuitFanRotary.Init(0, 120, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], ClycolSuitFanRotaryRow);

			QtyMonRotaryRow.Init(AID_QTYMONROTARY, MainPanel);
			QtyMonRotary.Init(0,   0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], QtyMonRotaryRow);

			EngineDescentCommandOverrideSwitchesRow.Init(AID_DESCENT_ENGINE_SWITCH,MainPanel);
			EngineDescentCommandOverrideSwitch.Init (0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineDescentCommandOverrideSwitchesRow);

			ModeControlSwitchesRow.Init(AID_MODECONTROL,MainPanel);

			ModeControlPNGSSwitch.Init (0, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ModeControlSwitchesRow, &agc);
			ModeControlAGSSwitch.Init (93, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ModeControlSwitchesRow);

			IMUCageSwitch.Init(191, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], ModeControlSwitchesRow, &imu);

			EngGimbalEnableSwitchRow.Init(AID_ENGGIMBALENABLESWITCH,MainPanel);
			EngGimbalEnableSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngGimbalEnableSwitchRow);

			RadarAntTestSwitchesRow.Init(AID_RADARANTTESTSWITCHES,MainPanel);
			LandingAntSwitch.Init(0,  0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RadarAntTestSwitchesRow);
			RadarTestSwitch.Init (0, 95, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RadarAntTestSwitchesRow);

			TestMonitorRotaryRow.Init(AID_TESTMONITORROTARY, MainPanel);
			TestMonitorRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], TestMonitorRotaryRow);

			SlewRateSwitchRow.Init(AID_SLEWRATESWITCH, MainPanel);
			SlewRateSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SlewRateSwitchRow);

			RendezvousRadarRotaryRow.Init(AID_RENDEZVOUSRADARROTARY, MainPanel);
			RendezvousRadarRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], RendezvousRadarRotaryRow);

			StabContSwitchesRow.Init(AID_SATBCONTSWITCHES, MainPanel);
			DeadBandSwitch.Init     (  0, 5, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], StabContSwitchesRow);
			GyroTestLeftSwitch.Init ( 93, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], StabContSwitchesRow);
			GyroTestRightSwitch.Init(191, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], StabContSwitchesRow);
			AttitudeControlSwitchesRow.Init(AID_ATTITUDECONTROLSWITCHES, MainPanel);
			RollSwitch.Init (  0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlSwitchesRow);
			PitchSwitch.Init( 93, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlSwitchesRow);
			YawSwitch.Init  (191, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlSwitchesRow);

			TempMonitorIndRow.Init(AID_TEMPMONITORIND, MainPanel);
			TempMonitorInd.Init(srf[SRF_NEEDLE], TempMonitorIndRow, this);

			TempMonitorRotaryRow.Init(AID_TEMPMONITORROTARY, MainPanel);
			TempMonitorRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], TempMonitorRotaryRow);

			RCSSysQuadSwitchesRow.Init(AID_RCSSYSQUADSWITCHES, MainPanel);
			RCSSysQuad1Switch.Init( 0,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);
			RCSSysQuad2Switch.Init( 0, 95, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);
			RCSSysQuad3Switch.Init(58, 95, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);
			RCSSysQuad4Switch.Init(58,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);

			LightingSwitchesRow.Init(AID_LIGHTINGSWITCHES, MainPanel);
			SidePanelsSwitch.Init( 0,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LightingSwitchesRow);
			FloodSwitch.Init     ( 0, 68, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], LightingSwitchesRow);

			FloodRotaryRow.Init(AID_FLOODROTARY, MainPanel);
			FloodRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], FloodRotaryRow);

			LampToneTestRotaryRow.Init(AID_LAMPTONETESTROTARY, MainPanel);
			LampToneTestRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], LampToneTestRotaryRow);

			RightXPointerSwitchRow.Init(AID_RIGHTXPOINTERSWITCH, MainPanel);
			RightXPointerSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RightXPointerSwitchRow);

			ExteriorLTGSwitchRow.Init(AID_EXTERIORLTGSWITCH, MainPanel);
			ExteriorLTGSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ExteriorLTGSwitchRow);

			Panel4LeftSwitchRow.Init(AID_PANEL4LEFTSWITCHROW, MainPanel);
			LeftACA4JetSwitch.Init   (0,   0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4LeftSwitchRow);
			LeftTTCATranslSwitch.Init(0, 141, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4LeftSwitchRow);

			Panel4RightSwitchRow.Init(AID_PANEL4RIGHTSWITCHROW, MainPanel);
			RightACA4JetSwitch.Init   (0,   0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4RightSwitchRow);
			RightTTCATranslSwitch.Init(0, 141, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4RightSwitchRow);

			RaderSignalStrengthMeterRow.Init(AID_LMRADARSIGNALSTRENGTH, MainPanel);
			RadarSignalStrengthMeter.Init(g_Param.pen[4], g_Param.pen[4], RaderSignalStrengthMeterRow, 0);
			RadarSignalStrengthMeter.SetSurface(srf[SRF_LMSIGNALSTRENGTH], 91, 90);

			RadarSlewSwitchRow.Init(AID_LMRADARSLEWSWITCH, MainPanel);
			RadarSlewSwitch.Init(0, 0, 39, 39, srf[SRF_FIVE_POS_SWITCH], NULL, RadarSlewSwitchRow);

			EventTimerSwitchRow.Init(AID_LM_EVENT_TIMER_SWITCHES, MainPanel);
			EventTimerCtlSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow);
			EventTimerStartSwitch.Init(55, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow);
			EventTimerMinuteSwitch.Init(111, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow);
			EventTimerSecondSwitch.Init(167, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow);

			break;

		case LMPANEL_RIGHTPANEL: // LEM Right Panel
			Panel16CB1SwitchRow.Init(AID_LEM_P16_CB_ROW1, MainPanel);
			LMP_EVT_TMR_FDAI_DC_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
			SE_XPTR_DC_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
			RCS_B_ASC_FEED_1_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
			RCS_B_ASC_FEED_2_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
			RCS_B_ISOL_VLV_CB.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
			RCS_B_QUAD1_TCA_CB.Init(320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
			RCS_B_QUAD2_TCA_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
			RCS_B_QUAD3_TCA_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
			RCS_B_QUAD4_TCA_CB.Init(512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
			RCS_B_CRSFD_CB.Init(576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
			RCS_B_TEMP_PRESS_DISP_FLAGS_CB.Init(640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
			RCS_B_PQGS_DISP_CB.Init( 704, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
			RCS_B_MAIN_SOV_CB.Init(768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
			PROP_DISP_ENG_OVRD_LOGIC_CB.Init( 832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
			PROP_PQGS_CB.Init(896, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
			PROP_ASC_HE_REG_CB.Init(1019, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);

			Panel16CB2SwitchRow.Init(AID_LEM_P16_CB_ROW2, MainPanel);
			LTG_FLOOD_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 5.0);
			LTG_TRACK_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 20.0);
			LTG_ANUN_DOCK_COMPNT_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			LTG_MASTER_ALARM_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			EDS_CB_LOGIC_B.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			SCS_AEA_CB.Init(320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 10.0);
			SCS_ENG_ARM_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			SCS_ASA_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 20.0);
			SCS_AELD_CB.Init(512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 7.5);
			SCS_ATCA_CB.Init(576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 7.5);
			SCS_ABORT_STAGE_CB.Init(640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			SCS_ATCA_AGS_CB.Init(704, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 3.0);
			SCS_DES_ENG_OVRD_CB.Init(768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 10.0);
			INST_CWEA_CB.Init(832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			INST_SIG_SENSOR_CB.Init( 896,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			INST_PCMTEA_CB.Init( 1019,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			INST_SIG_CONDR_2_CB.Init( 1083,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			ECS_SUIT_FLOW_CONT_CB.Init(1147, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 5.0);

			Panel16CB3SwitchRow.Init(AID_LEM_P16_CB_ROW3, MainPanel);
			COMM_DISP_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			COMM_SE_AUDIO_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			COMM_VHF_XMTR_A_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
			COMM_VHF_RCVR_B_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			COMM_PRIM_SBAND_PA_CB.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 7.5);
			COMM_PRIM_SBAND_XCVR_CB.Init(320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
			COMM_SBAND_ANT_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			COMM_PMP_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			COMM_TV_CB.Init(512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			ECS_DISP_CB.Init(576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			ECS_GLYCOL_PUMP_SEC_CB.Init(640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
			ECS_LGC_PUMP_CB.Init(768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
			ECS_CABIN_FAN_CONT_CB.Init(832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			ECS_CABIN_REPRESS_CB.Init( 896,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			ECS_SUIT_FAN_2_CB.Init(1019, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 20.0);
			ECS_SUIT_FAN_DP_CB.Init(1083, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			ECS_DIVERT_VLV_CB.Init(1147, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
			ECS_CO2_SENSOR_CB.Init(1211, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);

			Panel16CB4SwitchRow.Init(AID_LEM_P16_CB_ROW4, MainPanel);
			RCS_QUAD_1_LMP_HTR_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
			RCS_QUAD_2_LMP_HTR_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
			RCS_QUAD_3_LMP_HTR_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
			RCS_QUAD_4_LMP_HTR_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
			HTR_DISP_CB.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
			HTR_SBD_ANT_CB.Init( 320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 5.0);
			CAMR_SEQ_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
			EPS_DISP_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
			LMPDCBusVoltCB.Init( 512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
			LMPInverter2CB.Init( 576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 30.0);
			LMPAscECAMainCB.Init( 640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
			LMPAscECAContCB.Init( 704, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 3.0);
			LMPDesECAMainCB.Init( 768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 3.0);
			LMPDesECAContCB.Init( 832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 5.0);
			LMPXLunarBusTieCB.Init( 896,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 50.0);
			LMPCrossTieBusCB.Init( 1019,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 100.0);
			LMPCrossTieBalCB.Init( 1083,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 30.0);
			LMPBatteryFeedTieCB1.Init( 1147,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &BTB_LMP_B, 100.0);
			LMPBatteryFeedTieCB2.Init( 1211,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &BTB_LMP_C, 100.0);

			EPSP14VoltMeterSwitchRow.Init(AID_LM_EPS_DC_VOLTMETER,MainPanel);			
			EPSDCVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], EPSP14VoltMeterSwitchRow, &EPSMonitorSelectRotary);
			EPSDCVoltMeter.SetSurface(srf[SRF_LMSIGNALSTRENGTH], 99, 98); // Should be SRF_DCVOLTS but that is invalid for some reason?

			EPSP14AmMeterSwitchRow.Init(AID_LM_EPS_DC_AMMETER,MainPanel);
			EPSDCAmMeter.Init(g_Param.pen[4], g_Param.pen[4], EPSP14AmMeterSwitchRow, &EPSMonitorSelectRotary);
			EPSDCAmMeter.SetSurface(srf[SRF_LMSIGNALSTRENGTH], 99, 98); // Should be SRF_DCAMPS but that is invalid for some reason?

			EPSLeftControlArea.Init(AID_LM_EPS_LEFT_CONTROLS,MainPanel);
			EPSInverterSwitch.Init( 142, 135, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39],EPSLeftControlArea, this, &INV_1, &INV_2);
			EPSEDVoltSelect.Init(319-314, 868-728, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EPSLeftControlArea);
			EPSMonitorSelectRotary.Init(117, 17, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], EPSLeftControlArea);

			DSCHiVoltageSwitchRow.Init(AID_DSC_HIGH_VOLTAGE_SWITCHES, MainPanel);
			DSCSEBat1HVSwitch.Init( 0,  5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_1a,1,0);
			DSCSEBat2HVSwitch.Init( 69,  5, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_1b,1,0);
			DSCCDRBat3HVSwitch.Init(151,  5, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_2a,1,0);
			DSCCDRBat4HVSwitch.Init(220,  5, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_2b,1,0);
			DSCBattFeedSwitch.Init(291,  0, 34, 39,srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], DSCHiVoltageSwitchRow, this);

			DSCLoVoltageSwitchRow.Init(AID_DSC_LOW_VOLTAGE_SWITCHES, MainPanel);
			DSCSEBat1LVSwitch.Init( 0,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCLoVoltageSwitchRow, this,&ECA_1a,2,0);
			DSCSEBat2LVSwitch.Init( 69,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29],DSCLoVoltageSwitchRow, this,&ECA_1b,2,0);
			DSCCDRBat3LVSwitch.Init( 151,  0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29],DSCLoVoltageSwitchRow, this,&ECA_2a,2,0);
			DSCCDRBat4LVSwitch.Init( 220,  0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29],DSCLoVoltageSwitchRow, this,&ECA_2b,2,0);

			DSCBatteryTBSwitchRow.Init(AID_DSC_BATTERY_TALKBACKS, MainPanel);
			DSCBattery1TB.Init(0,  0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);
			DSCBattery2TB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);
			DSCBattery3TB.Init(152,0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);
			DSCBattery4TB.Init(222,0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);
			DSCBattFeedTB.Init(292,0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);

			ASCBatteryTBSwitchRow.Init(AID_ASC_BATTERY_TALKBACKS, MainPanel);
			ASCBattery5ATB.Init(0,  0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow);
			ASCBattery5BTB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow);
			ASCBattery6ATB.Init(138,0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow);
			ASCBattery6BTB.Init(221,0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow);

			ASCBatterySwitchRow.Init(AID_ASC_BATTERY_SWITCHES, MainPanel);
			ASCBat5SESwitch.Init ( 0,    0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this,&ECA_3a,1,1);
			ASCBat5CDRSwitch.Init( 69,   0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this,&ECA_3b,1,1);
			ASCBat6CDRSwitch.Init( 138,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this,&ECA_4a,1,1);
			ASCBat6SESwitch.Init ( 221,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this,&ECA_4b,1,1);
			UpDataSquelchSwitch.Init(295, 0, 34, 29,srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ASCBatterySwitchRow);

			Panel12AudioCtlSwitchRow.Init(AID_LM_P12_AUDIO_CONT_SWITCH, MainPanel);
			Panel12AudioCtlSwitch.Init(0, 0, 34, 29,srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12AudioCtlSwitchRow);
			Panel12UpdataLinkSwitchRow.Init(AID_LM_P12_UPDATA_LINK_SWITCH, MainPanel);
			Panel12UpdataLinkSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12UpdataLinkSwitchRow);

			Panel12AudioControlSwitchRow.Init(AID_LM_P12_AUDIO_CONTROLS, MainPanel);
			LMPAudSBandSwitch.Init(2, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
			LMPAudICSSwitch.Init(106, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
			LMPAudRelaySwitch.Init(206, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel12AudioControlSwitchRow);
			LMPAudSBandVol.Init(4, 61, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow); // FIXME: Need flashing border for this
			LMPAudICSVol.Init(108, 61, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
			LMPAudVOXSwitch.Init(206, 70, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
			LMPAudVHFASwitch.Init(0, 165, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
			LMPAudVHFBSwitch.Init(104, 165, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
			LMPAudVOXSens.Init(210, 125, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
			LMPAudVHFAVol.Init(5, 225, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
			LMPAudVHFBVol.Init(108, 225, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
			LMPAudMasterVol.Init(210, 225, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);

			Panel12CommSwitchRow1.Init(AID_LM_P12_COMM_SWITCHES_ROW1, MainPanel);
			SBandModulateSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
			SBandXCvrSelSwitch.Init(546-452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
			SBandPASelSwitch.Init(631-452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
			SBandVoiceSwitch.Init(721-452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
			SBandPCMSwitch.Init(813-452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
			SBandRangeSwitch.Init(889-452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);

			Panel12CommSwitchRow2.Init(AID_LM_P12_COMM_SWITCHES_ROW2, MainPanel);
			VHFAVoiceSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
			VHFARcvrSwtich.Init(634-568, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
			VHFBVoiceSwitch.Init(698-568, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
			VHFBRcvrSwtich.Init(764-568, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
			TLMBiomedSwtich.Init(828-568, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
			TLMBitrateSwitch.Init(894-568, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);

			Panel12CommSwitchRow3.Init(AID_LM_P12_COMM_SWITCHES_ROW3, MainPanel);
			VHFASquelch.Init(0, 4, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12CommSwitchRow3);
			VHFBSquelch.Init(136, 4, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12CommSwitchRow3);
			TapeRecorderTB.Init(  841-636, 1214-1189, 23, 23, srf[SRF_INDICATOR], Panel12CommSwitchRow3);
			TapeRecorderSwitch.Init(892-636, 1212-1189, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow3);

			ComPitchMeterRow.Init(AID_LMPITCHDEGS, MainPanel);
			ComPitchMeter.Init(g_Param.pen[4], g_Param.pen[4], ComPitchMeterRow, 0);
			ComPitchMeter.SetSurface(srf[SRF_LMPITCHDEGS], 91, 90);
			ComYawMeterRow.Init(AID_LMYAWDEGS, MainPanel);
			ComYawMeter.Init(g_Param.pen[4], g_Param.pen[4], ComYawMeterRow, 0);
			ComYawMeter.SetSurface(srf[SRF_LMYAWDEGS], 91, 90);
			Panel12SignalStrengthMeterRow.Init(AID_LMSIGNALSTRENGTH, MainPanel);
			Panel12SignalStrengthMeter.Init(g_Param.pen[4], g_Param.pen[4], Panel12SignalStrengthMeterRow, 0);
			Panel12SignalStrengthMeter.SetSurface(srf[SRF_LMSIGNALSTRENGTH], 91, 90);

			Panel12AntTrackModeSwitchRow.Init(AID_LM_P12_COMM_ANT_TRACK_MODE_SWITCH,MainPanel);
			Panel12AntTrackModeSwitch.Init(0, 0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AntTrackModeSwitchRow);
			
			Panel12VHFAntSelSwitchRow.Init(AID_LM_P12_COMM_ANT_VHF_SEL_KNOB,MainPanel);
			Panel12VHFAntSelKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12VHFAntSelSwitchRow);
			Panel12SBandAntSelSwitchRow.Init(AID_LM_P12_COMM_ANT_SBD_SEL_KNOB,MainPanel);
			Panel12SBandAntSelKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12SBandAntSelSwitchRow);
			Panel12AntPitchSwitchRow.Init(AID_LM_P12_COMM_ANT_PITCH_KNOB, MainPanel);
			Panel12AntPitchKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12AntPitchSwitchRow);
			Panel12AntYawSwitchRow.Init(AID_LM_P12_COMM_ANT_YAW_KNOB, MainPanel);
			Panel12AntYawKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12AntYawSwitchRow);

			AGSOperateSwitchRow.Init(AID_LM_AGS_OPERATE_SWITCH,MainPanel);
			AGSOperateSwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], AGSOperateSwitchRow);

			break;

		case LMPANEL_LEFTPANEL:
			Panel11CB1SwitchRow.Init(AID_LEM_P11_CB_ROW1, MainPanel);
			SE_WND_HTR_AC_CB.Init(3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			HE_PQGS_PROP_DISP_AC_CB.Init(67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			SBD_ANT_AC_CB.Init(131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			ORDEAL_AC_CB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			AQS_AC_CB.Init(318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			AOT_LAMP_ACB_CB.Init(382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			LMP_FDAI_AC_CB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			NUM_LTG_AC_CB.Init( 510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
			AC_B_INV_2_FEED_CB.Init( 574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_2, 5.0);
			AC_B_INV_1_FEED_CB.Init( 638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_1, 5.0);
			AC_A_INV_2_FEED_CB.Init( 702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_2, 5.0);
			AC_A_INV_1_FEED_CB.Init( 766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_1, 5.0);
			AC_A_BUS_VOLT_CB.Init(830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);			
			CDR_WND_HTR_AC_CB.Init(894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
			TAPE_RCDR_AC_CB.Init(958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
			AOT_LAMP_ACA_CB.Init(1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
			RDZ_RDR_AC_CB.Init(1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
			DECA_GMBL_AC_CB.Init(1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
			INTGL_LTG_AC_CB.Init(1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);

			Panel11CB2SwitchRow.Init(AID_LEM_P11_CB_ROW2, MainPanel);
			RCS_A_MAIN_SOV_CB.Init( 3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0); 
			RCS_A_QUAD4_TCA_CB.Init( 67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0); 
			RCS_A_QUAD3_TCA_CB.Init( 131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0); 
			RCS_A_QUAD2_TCA_CB.Init( 195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0); 
			RCS_A_QUAD1_TCA_CB.Init( 318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0); 
			RCS_A_ISOL_VLV_CB.Init( 382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0); 
			RCS_A_ASC_FEED_2_CB.Init( 446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0); 
			RCS_A_ASC_FEED_1_CB.Init( 510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0); 
			THRUST_DISP_CB.Init( 574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0); 
			MISSION_TIMER_CB.Init( 638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0); 
			CDR_XPTR_CB.Init( 702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0); 
			RNG_RT_ALT_RT_DC_CB.Init( 766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0); 
			GASTA_DC_CB.Init( 830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0); 
			CDR_FDAI_DC_CB.Init( 894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
			COAS_DC_CB.Init( 958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0); 
			ORDEAL_DC_CB.Init( 1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0); 
			RNG_RT_ALT_RT_AC_CB.Init( 1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &ACBusA, 2.0);
			GASTA_AC_CB.Init( 1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &ACBusA, 2.0);
			CDR_FDAI_AC_CB.Init( 1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &ACBusA, 2.0);

			Panel11CB3SwitchRow.Init(AID_LEM_P11_CB_ROW3, MainPanel); // 184,431 to 1433,459
			PROP_DES_HE_REG_VENT_CB.Init( 3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 5.0);
			HTR_RR_STBY_CB.Init( 67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 5.0);
			HTR_RR_OPR_CB.Init( 131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 7.5);
			HTR_LR_CB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 5.0);
			HTR_DOCK_WINDOW_CB.Init( 318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			HTR_AOT_CB.Init( 382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			INST_SIG_CONDR_1_CB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			CDR_SCS_AEA_CB.Init(510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 10.0);
			CDR_SCS_ABORT_STAGE_CB.Init(574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			CDR_SCS_ATCA_CB.Init(638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			CDR_SCS_AELD_CB.Init(702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 7.5);
			SCS_ENG_CONT_CB.Init(766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			SCS_ATT_DIR_CONT_CB.Init(830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 10.0);
			SCS_ENG_START_OVRD_CB.Init(894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			SCS_DECA_PWR_CB.Init(958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 20.0);
			EDS_CB_LG_FLAG.Init(1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			EDS_CB_LOGIC_A.Init(1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			CDR_LTG_UTIL_CB.Init(1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			CDR_LTG_ANUN_DOCK_COMPNT_CB.Init(1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);

			Panel11CB4SwitchRow.Init(AID_LEM_P11_CB_ROW4, MainPanel);
			RCS_QUAD_4_CDR_HTR_CB.Init(3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
			RCS_QUAD_3_CDR_HTR_CB.Init(67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
			RCS_QUAD_2_CDR_HTR_CB.Init(131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
			RCS_QUAD_1_CDR_HTR_CB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
			ECS_SUIT_FAN_1_CB.Init(318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 20.0);
			ECS_CABIN_FAN_1_CB.Init(382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
			ECS_GLYCOL_PUMP_2_CB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
			ECS_GLYCOL_PUMP_1_CB.Init(510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
			ECS_GLYCOL_PUMP_AUTO_XFER_CB.Init(574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
			COMM_UP_DATA_LINK_CB.Init(638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
			COMM_SEC_SBAND_XCVR_CB.Init( 702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
			COMM_SEC_SBAND_PA_CB.Init( 766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
			COMM_VHF_XMTR_B_CB.Init(830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
			COMM_VHF_RCVR_A_CB.Init( 894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
			COMM_CDR_AUDIO_CB.Init( 958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
			PGNS_SIG_STR_DISP_CB.Init( 1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
			PGNS_LDG_RDR_CB.Init( 1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 10.0);
			PGNS_RNDZ_RDR_CB.Init( 1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 15.0);
			LGC_DSKY_CB.Init( 1214,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
			IMU_SBY_CB.Init( 1278,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
			IMU_OPR_CB.Init ( 1342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 20.0);

			Panel11CB5SwitchRow.Init(AID_LEM_P11_CB_ROW5, MainPanel);
			CDRBatteryFeedTieCB1.Init(  3,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &BTB_CDR_B, 100.0);
			CDRBatteryFeedTieCB2.Init( 67,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &BTB_CDR_C, 100.0);
			CDRCrossTieBalCB.Init( 131,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 30.0);
			CDRCrossTieBusCB.Init( 195,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 100.0);
			CDRXLunarBusTieCB.Init( 318,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 50.0);
			CDRDesECAContCB.Init( 382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 5.0);
			CDRDesECAMainCB.Init( 446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 3.0);
			CDRAscECAContCB.Init( 510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 3.0);
			CDRAscECAMainCB.Init( 574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 2.0);
			CDRInverter1CB.Init( 638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 30.0); 
			CDRDCBusVoltCB.Init( 702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 2.0);
			
			// Panel 5 is 1000,1300 to 1560, 1620
			Panel5SwitchRow.Init(AID_LEM_PANEL_5, MainPanel);
			TimerContSwitch.Init(233, 43, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 0);
			TimerSlewHours.Init(333, 43, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 1);
			TimerSlewMinutes.Init(405, 43, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 2);
			TimerSlewSeconds.Init(477, 43, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 3);
			LtgORideAnunSwitch.Init(323, 147, 34, 29,srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
			LtgORideNumSwitch.Init(380, 147, 34, 29,srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
			LtgORideIntegralSwitch.Init(437, 147, 34, 29,srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
			LtgSidePanelsSwitch.Init(494, 147, 34, 29,srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
			LtgFloodOhdFwdKnob.Init(173, 222, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel5SwitchRow);
			LtgAnunNumKnob.Init(333, 222, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel5SwitchRow);
			LtgIntegralKnob.Init(457, 222, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel5SwitchRow);

			// Panel 8 is  431,916 to 1574,1258
			Panel8SwitchRow.Init(AID_LEM_PANEL_8, MainPanel);
			EDMasterArm.Init(861-431, 978-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDDesVent.Init(935-431, 978-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDASCHeSel.Init(571, 62, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDDesPrpIsol.Init(643-431, 978-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDLGDeploy.Init(784-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDHePressRCS.Init(861-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDHePressDesStart.Init(935-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDHePressASC.Init(1002-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDStage.Init(783-431, 1175-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			//EDStage.InitGuard(304, 252, 102, 59, srf[SRF_LEM_STAGESWITCH], Panel8SwitchRow);
			EDStageRelay.Init(1002-431, 1182-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDDesFuelVent.Init(36, 100, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			EDDesOxidVent.Init(109, 100, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			EDLGTB.Init(790-431, 1033-916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow);
			EDDesFuelVentTB.Init(472-431, 960-916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow);
			EDDesOxidVentTB.Init(545-431, 960-916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow);
			// Audio stuff
			CDRAudSBandSwitch.Init(1189-431, 949-916, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			CDRAudICSSwitch.Init(1288-431, 949-916, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			CDRAudRelaySwitch.Init(1394-431, 938-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			CDRAudVOXSwitch.Init(1393-431, 1017-916, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			CDRAudioControlSwitch.Init(1046, 101, 34, 29,srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			CDRAudVHFASwitch.Init(1189-431, 1116-916, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			CDRAudVHFBSwitch.Init(1288-431, 1116-916, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			CDRAudSBandVol.Init(759, 91, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow); // FIXME: Need flashing border for this
			CDRAudVHFAVol.Init(759, 258, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
			CDRAudVHFBVol.Init(858, 258, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
			CDRAudICSVol.Init(858, 91, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
			CDRAudMasterVol.Init(963, 258, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
			CDRAudVOXSens.Init(963, 158, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
			CDRCOASSwitch.Init(1063, 266, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			break;

		case LMPANEL_AOTVIEW:			
			RRGyroSelSwitchRow.Init(AID_RR_GYRO_SEL_SWITCH, MainPanel);
			RRGyroSelSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RRGyroSelSwitchRow);
			break;

		case LMPANEL_LPDWINDOW:
			break;

	}
}

void LEM::PanelSwitchToggled(ToggleSwitch *s) {


	if (s == &AbortSwitch) {
		if (s->IsDown()) {
			// This is the "ABORT" button
			AbortFire();
			SetEngineLevel(ENGINE_HOVER, 1);
			//SetThrusterResource(th_hover[0], ph_Asc);
			//SetThrusterResource(th_hover[1], ph_Asc);
			//stage = 2;
			startimer = false;
			agc.SetInputChannelBit(030, 1, true);
		}
	
	} else if (s == &AbortStageSwitch) {
		// This is the "ABORT STAGE" button
		if (s->IsDown()) {
			AbortFire();
			AbortStageSwitchLight = true;
			SeparateStage(stage);
			SetThrusterResource(th_hover[0], ph_Asc);
			SetThrusterResource(th_hover[1], ph_Asc);
			stage = 2;
			startimer = false;
			AbortStageSwitchLight = true;
			if(agc.GetProgRunning() > 14 ) {
				SetEngineLevel(ENGINE_HOVER, 1);
				agc.SetInputChannelBit(030, 4, true);
			}
		}

	} 
	else if (s == &EngineArmSwitch) {
		if (!s->IsCenter())
 		    agc.SetInputChannelBit(030, 3, true);
    }
}

void LEM::PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) {

}

void LEM::PanelRotationalSwitchChanged(RotationalSwitch *s) {

}

void LEM::PanelThumbwheelSwitchChanged(ThumbwheelSwitch *s) {

}

void LEM::MousePanel_MFDButton(int mfd, int event, int mx, int my) {

	if (oapiGetMFDMode(mfd) != MFD_NONE) {
		if (my > 330 && my < 352) {
			if (mx > 67 && mx < 96) {
				ButtonClick();
				oapiToggleMFD_on(mfd);
			} else if (mx > 295 && mx < 324) {
				ButtonClick();
				oapiSendMFDKey(mfd, OAPI_KEY_F1);	
			} else if (mx > 329 && mx < 356) {
				ButtonClick();
				oapiSendMFDKey(mfd, OAPI_KEY_GRAVE);	
			}
		} else if (mx > 10 && mx < 38 && my > 53 && my < 294) {
			if ((my - 53) % 44 < 21) {
				int bt = (my - 53) / 44 + 0;
				ButtonClick();
				oapiProcessMFDButton (mfd, bt, event);
			}
		} else if (mx > 386 && mx < 416 && my > 53 && my < 294) {
			if ((my - 53) % 44 < 21) {
				int bt = (my - 53) / 44 + 6;
				ButtonClick();
				oapiProcessMFDButton (mfd, bt, event);
			}
		}
	} else {
		ButtonClick();
		oapiToggleMFD_on(mfd);
	}

}

void LEM::SwitchClick()

{
	Sclick.play();
}

void LEM::ButtonClick()

{
	Bclick.play(NOLOOP,255);
}

void LEM::GuardClick()

{
	Gclick.play(NOLOOP,255);
}

void LEM::CabinFanSound()

{
	CabinFans.play(LOOP,255);
}

void LEM::VoxSound()

{
	Vox.play(LOOP,255);
}

void LEM::AbortFire()

{
	Afire.play(NOLOOP,255);
}

bool LEM::clbkPanelMouseEvent (int id, int event, int mx, int my)

{
	static int ctrl = 0;


	if (MainPanel.CheckMouseClick(id, event, mx, my))
		return true;


	switch (id) {
	// panel 0 events:
	case AID_DSKY_KEY:
		if (event & PANEL_MOUSE_LBDOWN) {
			dsky.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky.ProcessKeyRelease(mx, my);
		}
		return true;

	case AID_LM_DEDA_KEYS:
		if (event & PANEL_MOUSE_LBDOWN) {
			deda.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			deda.ProcessKeyRelease(mx, my);
		}
		return true;

	case AID_MFDLEFT:
		MousePanel_MFDButton(MFD_LEFT, event, mx, my);
		return true;

	case AID_MFDRIGHT:
		MousePanel_MFDButton(MFD_RIGHT, event, mx, my);
		return true;

	case AID_LEM_COAS1:
		if (LEMCoas1Enabled)
			LEMCoas1Enabled = false;
		else
			LEMCoas1Enabled = true;
		SwitchClick();
		return true;

	case AID_LEM_COAS2:
		if (LEMCoas2Enabled)
			LEMCoas2Enabled = false;
		else
			LEMCoas2Enabled = true;
		SwitchClick();
		return true;

	case AID_DESCENT_HE:
		if (my >=30 && my <=42 ){
			if (mx > 1 && mx < 26 && !DESHE1switch){
				SwitchClick();
				DESHE1switch=true;
			}else if (mx > 43 && mx < 69 && !DESHE2switch){
				SwitchClick();
				DESHE2switch=true;
			}
		}else if (my >=30 && my <=53 ){
			if (mx > 1 && mx < 26 && DESHE1switch){
				SwitchClick();
				DESHE1switch=false;
			}else if (mx > 43 && mx < 69 && DESHE2switch){
				SwitchClick();
				DESHE2switch=false;
			}
		}
		return true;
/*
	case AID_ENG_ARM:
		if (my >=0 && my <=16 ){
			if (mx > 0 && mx < 23 && !ENGARMswitch){
				SwitchClick();
				ENGARMswitch=true;
			}
		}else if (my >=16 && my <=30 ){
			if (mx > 0 && mx < 23 && ENGARMswitch){
				SwitchClick();
				ENGARMswitch=false;
			}
		}
		return true;
*/
	case AID_LGC_THRUSTER_QUADS:
		if (my >=35 && my <=47 ){
			if (mx > 1 && mx < 25 && !QUAD1switch){
				SwitchClick();
				QUAD1switch=true;
			}else if (mx > 44 && mx < 68 && !QUAD2switch){
				SwitchClick();
				QUAD2switch=true;
			}else if (mx > 92 && mx < 116 && !QUAD3switch){
				SwitchClick();
				QUAD3switch=true;
			}else if (mx > 135 && mx < 159 && !QUAD4switch){
				SwitchClick();
				QUAD4switch=true;
			}
		}else if (my >=115 && my <=126 ){
			if (mx > 1 && mx < 25 && !QUAD5switch){
				SwitchClick();
				QUAD5switch=true;
			}else if (mx > 44 && mx < 68 && !QUAD6switch){
				SwitchClick();
				QUAD6switch=true;
			}else if (mx > 92 && mx < 116 && !QUAD7switch){
				SwitchClick();
				QUAD7switch=true;
			}else if (mx > 135 && mx < 159 && !QUAD8switch){
				SwitchClick();
				QUAD8switch=true;
			}
		}else if (my >=48 && my <=58 ){
			if (mx > 1 && mx < 25 && QUAD1switch){
				SwitchClick();
				QUAD1switch=false;
			}else if (mx > 44 && mx < 68 && QUAD2switch){
				SwitchClick();
				QUAD2switch=false;
			}else if (mx > 92 && mx < 116 && QUAD3switch){
				SwitchClick();
				QUAD3switch=false;
			}else if (mx > 135 && mx < 159 && QUAD4switch){
				SwitchClick();
				QUAD4switch=false;
			}
		}else if (my >=127 && my <=138 ){
			if (mx > 1 && mx < 25 && QUAD5switch){
				SwitchClick();
				QUAD5switch=false;
			}else if (mx > 44 && mx < 68 && QUAD6switch){
				SwitchClick();
				QUAD6switch=false;
			}else if (mx > 92 && mx < 116 && QUAD7switch){
				SwitchClick();
				QUAD7switch=false;
			}else if (mx > 135 && mx < 159 && QUAD8switch){
				SwitchClick();
				QUAD8switch=false;
			}
		}

		return true;

	case AID_RCS_SYSTEMA_SYSTEMB:
		if (my >=38 && my <=49 ){
			if (mx > 0 && mx < 24 && !AFEED1switch){
				SwitchClick();
				AFEED1switch=true;
			}else if (mx > 43 && mx < 67 && !AFEED2switch){
				SwitchClick();
				AFEED2switch=true;
			}else if (mx > 91 && mx < 115 && !AFEED3switch){
				SwitchClick();
				AFEED3switch=true;
			}else if (mx > 134 && mx < 158 && !AFEED4switch){
				SwitchClick();
				AFEED4switch=true;
			}
		}else if (my >=49 && my <=60 ){
		if (mx > 0 && mx < 24 && AFEED1switch){
				SwitchClick();
				AFEED1switch=false;
			}else if (mx > 43 && mx < 67 && AFEED2switch){
				SwitchClick();
				AFEED2switch=false;
			}else if (mx > 91 && mx < 115 && AFEED3switch){
				SwitchClick();
				AFEED3switch=false;
			}else if (mx > 134 && mx < 158 && AFEED4switch){
				SwitchClick();
				AFEED4switch=false;
			}
		}

		return true;

	/*
	case AID_MAIN_SOV_SWITCHES:
		if (my >=35 && my <=46 ){
			if (mx > 0 && mx < 24 && !MSOV1switch){
				SwitchClick();
				MSOV1switch=true;
			}else if (mx > 55 && mx < 79 && !MSOV2switch){
				SwitchClick();
				MSOV2switch=true;
			}
		}else if (my >=45 && my <=57 ){
		if (mx > 0 && mx < 24 && MSOV1switch){
				SwitchClick();
				MSOV1switch=false;
			}else if (mx > 55 && mx < 79 && MSOV2switch){
				SwitchClick();
				MSOV2switch=false;
			}
		}

		return true;
	*/

	case AID_EXPLOSIVE_DEVICES1:
		if (my >=0 && my <=16 ){
			if (mx > 0 && mx < 24 && !ED1switch){
				SwitchClick();
				ED1switch=true;
			}else if (mx > 47 && mx < 71 && !ED2switch){
				SwitchClick();
				ED2switch=true;
				}
		}else if (my >=68 && my <=84 ){
			if (mx > 0 && mx < 24 && !ED4switch){
				SwitchClick();
				ED4switch=true;
			}else if (mx > 47 && mx < 71 && !ED5switch){
				SwitchClick();
				ED5switch=true;
			}else if (mx > 94 && mx < 118 && !ED6switch){
				SwitchClick();
				ED6switch=true;
			}
		}else if (my >=15 && my <=31 ){
		if (mx > 0 && mx < 24 && ED1switch){
				SwitchClick();
				ED1switch=false;
			}else if (mx > 47 && mx < 71 && ED2switch){
				SwitchClick();
				ED2switch=false;
				}
		}else if (my >=83 && my <=98 ){
			if (mx > 0 && mx < 24 && ED4switch){
				SwitchClick();
				ED4switch=false;
			}else if (mx > 47 && mx < 71 && ED5switch){
				SwitchClick();
				ED5switch=false;
			}else if (mx > 94 && mx < 118 && ED6switch){
				SwitchClick();
				ED6switch=false;
			}
		}

		return true;

	case AID_ATTITUDE_CONTROL_SWITCHES:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 24 && !ATT1switch){
				SwitchClick();
				ATT1switch=true;
			}else if (mx > 57 && mx < 81 && !ATT2switch){
				SwitchClick();
				ATT2switch=true;
			}else if (mx > 114 && mx < 138 && !ATT3switch){
				SwitchClick();
				ATT3switch=true;
			}
		}else if (my >=10 && my <=20 ){
		if (mx > 0 && mx < 24 && ATT1switch){
				SwitchClick();
				ATT1switch=false;
			}else if (mx > 57 && mx < 81 && ATT2switch){
				SwitchClick();
				ATT2switch=false;
			}else if (mx > 114 && mx < 138 && ATT3switch){
				SwitchClick();
				ATT3switch=false;
			}
		}

		return true;

	case AID_CRSFD_SWITCH:
		if (my >=29 && my <=45 ){
			if (mx > 0 && mx < 24 && !CRSFDswitch){
				SwitchClick();
				CRSFDswitch=true;
			}
		}else if (my >=44 && my <=59 ){
		if (mx > 0 && mx < 24 && CRSFDswitch){
				SwitchClick();
				CRSFDswitch=false;
			}
		}

		return true;

	case AID_CABIN_FAN_SWITCH:
		if (my >=0 && my <=15 ){
			if (mx > 0 && mx < 24 && !CABFswitch){
				SwitchClick();
				CabinFans.play(LOOP,255);
				CABFswitch=true;
			}
		}else if (my >=14 && my <=29 ){
		if (mx > 0 && mx < 24 && CABFswitch){
				SwitchClick();
				CabinFans.stop();
				CABFswitch=false;
			}
		}

		return true;

	case AID_PTT_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !PTTswitch){
				SwitchClick();
				Vox.play(LOOP,255);
				PTTswitch=true;
			}
		}
		else if (my >=10 && my <=20 ){
			if (mx > 0 && mx < 23 && PTTswitch){
				SwitchClick();
				Vox.stop();
				PTTswitch=false;
			}
		}

		return true;

	case AID_RCS_SYS_AB:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 24 && !RCSS1switch){
				SwitchClick();
				RCSS1switch=true;
			}else if (mx > 46 && mx < 69 && !RCSS2switch){
				SwitchClick();
				RCSS2switch=true;
			}
		}else if (my >=60 && my <=70 ){
			if (mx > 0 && mx < 24 && !RCSS3switch){
				SwitchClick();
				RCSS3switch=true;
			}else if (mx > 46 && mx < 69 && !RCSS4switch){
				SwitchClick();
				RCSS4switch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 24 && RCSS1switch){
				SwitchClick();
				RCSS1switch=false;
			}else if (mx > 46 && mx < 69 && RCSS2switch){
				SwitchClick();
				RCSS2switch=false;
			}
		}else if (my >=69 && my <=79 ){
			if (mx > 0 && mx < 24 && RCSS3switch){
				SwitchClick();
				RCSS3switch=false;
			}else if (mx > 46 && mx < 69 && RCSS4switch){
				SwitchClick();
				RCSS4switch=false;
			}

		}
		return true;

	case AID_XPOINTER_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !X1switch){
				SwitchClick();
				X1switch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && X1switch){
				SwitchClick();
				X1switch=false;
			}

		}
		return true;

	case AID_PANEL1_1:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 20 && !RATE1switch){
				SwitchClick();
				RATE1switch=true;
			}
		}else if (my >=58 && my <=69 ){
			if (mx > 0 && mx < 20 && !AT1switch){
				SwitchClick();
				AT1switch=true;
			}
		}else if (my >=0 && my <=21 ){
			if (mx > 0 && mx < 20 && RATE1switch){
				SwitchClick();
				RATE1switch=false;
			}
		}else if (my >=69 && my <=79 ){
			if (mx > 0 && mx < 20 && AT1switch){
				SwitchClick();
				AT1switch=false;
			}

		}
		return true;

	case AID_GUID_CONT_SWITCH:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !GUIDswitch){
				SwitchClick();
				GUIDswitch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && GUIDswitch){
				SwitchClick();
				GUIDswitch=false;
			}

		}
		return true;

	case AID_ALT_RNG_MON:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !ALTswitch){
				SwitchClick();
				ALTswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && ALTswitch){
				SwitchClick();
				ALTswitch=false;
			}

		}
		return true;

	case AID_SHIFT_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !SHFTswitch){
				SwitchClick();
				SHFTswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && SHFTswitch){
				SwitchClick();
				SHFTswitch=false;
			}

		}
		return true;

	case AID_ENGINE_THRUST_CONT:
		if (my >=0 && my <=16 ){
			if (mx > 0 && mx < 23 && !ETC1switch){
				SwitchClick();
				ETC1switch=true;
		}else if (mx > 47 && mx < 71 && !ETC2switch){
				SwitchClick();
				ETC2switch=true;
			}
		}else if (my >=53 && my <=69 ){
			if (mx > 14 && mx < 38 && !ETC3switch){
				SwitchClick();
				ETC3switch=true;
		}else if (mx > 52 && mx < 76 && !ETC4switch){
				SwitchClick();
				ETC4switch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && ETC1switch){
				SwitchClick();
				ETC1switch=false;
		}else if (mx > 47 && mx < 71 && ETC2switch){
				SwitchClick();
				ETC2switch=false;
			}
		}else if (my >=69 && my <=83 ){
			if (mx > 14 && mx < 38 && ETC3switch){
				SwitchClick();
				ETC3switch=false;
		}else if (mx > 52 && mx < 76 && ETC4switch){
				SwitchClick();
				ETC4switch=false;
			}

		}
		return true;

	case AID_PROP_MON_SWITCHES:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 20 && !PMON1switch){
				SwitchClick();
				PMON1switch=true;
			}
		}else if (my >=55 && my <=66 ){
			if (mx > 14 && mx < 38 && !PMON2switch){
				SwitchClick();
				PMON2switch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 20 && PMON1switch){
				SwitchClick();
				PMON1switch=false;
			}
		}else if (my >=66 && my <=76 ){
			if (mx > 14 && mx < 38 && PMON2switch){
				SwitchClick();
				PMON2switch=false;
			}

		}
		return true;

	case AID_ACA_PROP_SWITCH:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !ACAPswitch){
				SwitchClick();
				ACAPswitch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && ACAPswitch){
				SwitchClick();
				ACAPswitch=false;
			}

		}
		return true;

	case AID_PANEL2_1:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 20 && !RATE2switch){
				SwitchClick();
				RATE2switch=true;
			}
		}else if (my >=58 && my <=69 ){
			if (mx > 0 && mx < 20 && !AT2switch){
				SwitchClick();
				AT2switch=true;
			}
		}else if (my >=0 && my <=21 ){
			if (mx > 0 && mx < 20 && RATE2switch){
				SwitchClick();
				RATE2switch=false;
			}
		}else if (my >=69 && my <=79 ){
			if (mx > 0 && mx < 20 && AT2switch){
				SwitchClick();
				AT2switch=false;
			}

		}
		return true;
/*
	case AID_DESCENT_ENGINE_SWITCH:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !DESEswitch){
				SwitchClick();
				DESEswitch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && DESEswitch){
				SwitchClick();
				DESEswitch=false;
			}

		}
		return true;
*/
	case AID_SLEW_RATE_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !SLWRswitch){
				SwitchClick();
				SLWRswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && SLWRswitch){
				SwitchClick();
				SLWRswitch=false;
			}

		}
		return true;

	case AID_DEAD_BAND_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !DBswitch){
				SwitchClick();
				DBswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && DBswitch){
				SwitchClick();
				DBswitch=false;
			}

		}
		return true;

	case AID_IMU_CAGE_SWITCH:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !IMUCswitch){
				SwitchClick();
				IMUCswitch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && IMUCswitch){
				SwitchClick();
				IMUCswitch=false;
			}

		}
		return true;

	case AID_SIDE_PANEL_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !SPLswitch){
				SwitchClick();
				SPLswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && SPLswitch){
				SwitchClick();
				SPLswitch=false;
			}

		}
		return true;

	case AID_XPOINTER2_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !X2switch){
				SwitchClick();
				X2switch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && X2switch){
				SwitchClick();
				X2switch=false;
			}

		}
		return true;

	case AID_PANEL4_SWITCH1:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !P41switch){
				SwitchClick();
				P41switch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && P41switch){
				SwitchClick();
				P41switch=false;
			}

		}
		return true;

	case AID_PANEL4_SWITCH2:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !P42switch){
				SwitchClick();
				P42switch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && P42switch){
				SwitchClick();
				P42switch=false;
			}

		}
		return true;

	case AID_PANEL4_SWITCH3:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !P43switch){
				SwitchClick();
				P43switch=true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && P43switch){
				SwitchClick();
				P43switch=false;
			}

		}
		return true;

	case AID_PANEL4_SWITCH4:
		if (my >=0 && my <=17 ){
			if (mx > 0 && mx < 23 && !P44switch){
				SwitchClick();
				P44switch=true;
				toggleRCS =true;
			}
		}else if (my >=15 && my <=30 ){
			if (mx > 0 && mx < 23 && P44switch){
				SwitchClick();
				P44switch=false;
				toggleRCS =true;
			}

		}
		return true;

	case AID_AUDIO_CONT_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !AUDswitch){
				SwitchClick();
				AUDswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && AUDswitch){
				SwitchClick();
				AUDswitch=false;
			}

		}
		return true;

	case AID_RELAY_AUDIO_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !RELswitch){
				SwitchClick();
				RELswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && RELswitch){
				SwitchClick();
				RELswitch=false;
			}

		}
		return true;

	case AID_CABIN_PRESS_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !CPswitch){
				SwitchClick();
				CPswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && CPswitch){
				SwitchClick();
				CPswitch=false;
			}

		}
		return true;

	case AID_HATCH_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !HATCHswitch){
				SwitchClick();
				HATCHswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && HATCHswitch){
				SwitchClick();
				HATCHswitch=false;
			}

		}
		return true;

	case AID_EVA_SWITCH:
		if (my >=0 && my <=11 ){
			if (mx > 0 && mx < 23 && !EVAswitch){
				SwitchClick();
				EVAswitch=true;
			}
		}else if (my >=10 && my <=21 ){
			if (mx > 0 && mx < 23 && EVAswitch){
				SwitchClick();
				EVAswitch=false;
			}

		}
		return true;

	case AID_COAS:
		if (my >=0 && my <=149 ){
			if (mx > 0 && mx < 302 && !COASswitch){
				GuardClick();
				COASswitch=true;
			}
		}else if (my >=150 && my <=298 ){
			if (mx > 0 && mx < 302 && COASswitch){
				GuardClick();
				COASswitch=false;
			}

		}
		return true;

	case AID_AOT_RETICLE_KNOB:
		optics.ReticleMoved = 0;
		if (my >=0 && my <= 163 ){
			optics.ReticleMoved = 0.001 * pow(163 - my,1.25);
			optics.KnobTurning++;
		} else if (my >= 164 && my <= 326){
			optics.ReticleMoved = -0.001 * pow(my-164,1.25);
			optics.KnobTurning++;
		}
		if (optics.KnobTurning == 2) optics.KnobTurning = 0;

		if (event & PANEL_MOUSE_UP) optics.ReticleMoved = 0;

		return true;

	case AID_AOT_SHAFT_KNOB:
		if (my >=90 && my <= 155 ){
			optics.OpticsShaft++; 
		} else if (my >= 0 && my <= 90){
			optics.OpticsShaft--;
		}
		optics.OpticsShaft = (optics.OpticsShaft+6) % 6;
		ButtonClick();
		return true;


	case AID_SWITCH_JET:
		if(event & PANEL_MOUSE_RBDOWN){
			Cswitch2 = !Cswitch2;
			GuardClick();
		}
		else if(event & PANEL_MOUSE_LBDOWN){
			if(my >15 && my <26 && !Sswitch2){
				Sswitch2 = true;
				SwitchClick();
			}
			else if(my >26 && my <38 && Sswitch2 && Cswitch2){
				Sswitch2 = false;
				SwitchClick();
			}
		}
		return true;

	case AID_ENGINE_GIMBAL_SWITCH:
		if (my >=0 && my <=16 ){
			if (mx > 0 && mx < 24 && !GMBLswitch){
				SwitchClick();
				SetGimbal(true);
			}
		}else if (my >=15 && my <=31 ){
		if (mx > 0 && mx < 24 && GMBLswitch){
				SwitchClick();
				SetGimbal(false);
			}
		}
		return true;

	case AID_ASCENT_HE:
		if (my >=31 && my <=43 ){
			if (mx > 1 && mx < 27 && !ASCHE1switch){
				SwitchClick();
				ASCHE1switch=true;
			}else if (mx > 44 && mx < 70 && !ASCHE2switch){
				SwitchClick();
				ASCHE2switch=true;
			}
		}else if (my >=42 && my <=54 ){
			if (mx > 1 && mx < 27 && ASCHE1switch){
				SwitchClick();
				ASCHE1switch=false;
			}else if (mx > 44 && mx < 70 && ASCHE2switch){
				SwitchClick();
				ASCHE2switch=false;
			}
		}
		return true;

	case AID_SWITCH_SEP:
		if(event & PANEL_MOUSE_RBDOWN){
			Cswitch1 = !Cswitch1;
			GuardClick();
		}
		else if(event & PANEL_MOUSE_LBDOWN){
			if(my >15 && my <26 && !Sswitch1){
				Sswitch1 = true;
				SwitchClick();
			}
			else if(my >26 && my <38 && Sswitch1 && Cswitch1){
				Sswitch1 = false;
				SwitchClick();
			}
		}
		return true;

		case AID_EXPLOSIVE_DEVICES2:
		if (my >=0 && my <=15 ){
			if (mx > 0 && mx < 24 && !ED7switch){
				SwitchClick();
				ED7switch=true;
			}else if (mx > 48 && mx < 72 && !ED8switch){
				SwitchClick();
				ED8switch=true;
			}else if (mx > 94 && mx < 118 && !ED9switch){
				SwitchClick();
				ED9switch=true;
			}
		}else if (my >=16 && my <=30 ){
		if (mx > 0 && mx < 24 && ED7switch){
				SwitchClick();
				ED7switch=false;
			}else if (mx > 48 && mx < 72 && ED8switch){
				SwitchClick();
				ED8switch=false;
			}else if (mx > 94 && mx < 118 && ED9switch){
				SwitchClick();
				ED9switch=false;
			}
		}

		return true;

		case AID_LANDING_GEAR_SWITCH:
			if (my > 51 && LDGswitch){
				LDGswitch =false;
				SwitchClick();

			}else if(!LDGswitch && my < 51){
				LDGswitch =true;
				SwitchClick();

			}
			VESSELSTATUS vs;
			GetStatus(vs);

				if (GetEngineLevel(ENGINE_MAIN)==0 &&GetEngineLevel(ENGINE_HOVER)==0&& vs.status ==0){
					if (!LDGswitch){
						if (status==1){
						SetLmVesselDockStage();
						bModeDocked = true;
						bModeHover=false;
						//PlayVesselWave(SDMode,NOLOOP,255);
						}
					}else {
						bModeDocked = false;
						bModeHover=true;
						//PlayVesselWave(SHMode,NOLOOP,255);
						if (status==0){
						SetLmVesselHoverStage();
						}
					}
				}

		return true;

	// panel 1 events:
	}
	return false;
}

bool LEM::clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf) 

{
	int Curdigit;
	int Curdigit2;

	//
	// Special handling illuminated abort stage switch
	//

	if (AbortStageSwitchLight) {
		AbortStageSwitch.SetOffset(150, 0);
	} else {
		AbortStageSwitch.SetOffset(0, 0);
	}

	//
	// Process all the generic switches.
	//

	if (MainPanel.DrawRow(id, surf, PanelFlashOn))
		return true;

	//
	// Now special case the rest.
	//
	switch (id) {
	// panel 0 events:
	case AID_LM_CWS_LEFT:
		CWEA.RedrawLeft(surf,srf[SRF_CWSLIGHTS]);
		return true;

	case AID_LM_CWS_RIGHT:
		CWEA.RedrawRight(surf,srf[SRF_CWSLIGHTS]);
		return true;
		
	case AID_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_DSKY]);
		return true;

	case AID_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP]);
		return true;

	case AID_DSKY_KEY:
		dsky.RenderKeys(surf, srf[SRF_DSKYKEY]);
		return true;

	case AID_LM_DEDA_LIGHTS:
		deda.RenderOprErr(surf, srf[SRF_DEDA_LIGHTS]);
		return true;

	case AID_LM_DEDA_DISP:
		deda.RenderData(surf, srf[SRF_DIGITALDISP2]);
		return true;

	case AID_LM_DEDA_ADR:
		deda.RenderAdr(surf, srf[SRF_DIGITALDISP2]);
		return true;

	case AID_LM_DEDA_KEYS:
		deda.RenderKeys(surf, srf[SRF_DEDA_KEY]);
		return true;

	case AID_MFDLEFT:
		if (oapiGetMFDMode(MFD_LEFT) != MFD_NONE) {	
			oapiBlt(surf, srf[SRF_LMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton (surf, MFD_LEFT, 0, 24, 58);	
			RedrawPanel_MFDButton (surf, MFD_LEFT, 1, 402, 58);	
		}
		return true;

	case AID_MFDRIGHT:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	
			oapiBlt(surf, srf[SRF_LMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton (surf, MFD_RIGHT, 0, 24, 58);	
			RedrawPanel_MFDButton (surf, MFD_RIGHT, 1, 402, 58);	
		}
		return true;

	case AID_LEM_COAS1:
		if (LEMCoas1Enabled) {
			oapiBlt(surf, srf[SRF_LEM_COAS1], 0, 0, 0, 0, 830, 830, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_LEM_COAS1], 0, 0, 0, 830, 830, 830, SURF_PREDEF_CK);
		}
		return true;

	case AID_LEM_COAS2:
		if (LEMCoas2Enabled) {
			oapiBlt(surf, srf[SRF_LEM_COAS2], 0, 0, 0, 0, 540, 540, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_LEM_COAS2], 0, 0, 0, 540, 540, 540, SURF_PREDEF_CK);
		}
		return true;

	case AID_XPOINTER:
		RedrawPanel_XPointer(surf);
		return true;

	case AID_XPOINTERCDR:
		RedrawPanel_XPointer(surf);
		return true;

	case AID_XPOINTERLMP:
		RedrawPanel_XPointer(surf);
		return true;

	case AID_CONTACTLIGHT1:
		if (GroundContact()&& stage ==1){
			oapiBlt(surf,srf[SRF_CONTACTLIGHT],0,0,0,0,48,48, SURF_PREDEF_CK);//
		}return true;

	case AID_RR_NOTRACK:
		if(RR.IsDCPowered() && !RR.IsRadarDataGood()){ // The AC side is only needed for the transmitter
			oapiBlt(surf,srf[SRF_RR_NOTRACK],0,0,0,34,34,34, SURF_PREDEF_CK); // Light On
		}else{
			oapiBlt(surf,srf[SRF_RR_NOTRACK],0,0,0,0,34,34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_CONTACTLIGHT2:
		if (GroundContact()&& stage ==1){
			oapiBlt(surf,srf[SRF_CONTACTLIGHT],0,48,0,0,48,48, SURF_PREDEF_CK);//
		}return true;

	case AID_SWITCH_SEP:
		if(Cswitch1){
			oapiBlt(surf,srf[SRF_SECSWITCH],0,0,25,0,25,45);
			if(Sswitch1){
				oapiBlt(surf,srf[SRF_LEMSWTICH3],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[SRF_LEMSWTICH3],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[SRF_SECSWITCH],0,0,0,0,25,45);
			Sswitch1=false;
		}
		return true;

	case AID_FDAI:
		RedrawPanel_Horizon (surf);
		return true;

	case AID_FDAI_LEFT:
		if (!fdaiDisabled){			
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;
			int no_att = 0;
			/*
			// *** DANGER WILL ROBINSON: FDAISourceSwitch and FDAISelectSwitch ARE REVERSED! ***
			switch(FDAISourceSwitch.GetState()){
				case THREEPOSSWITCH_UP:     // 1+2 - FDAI1 shows IMU ATT / CMC ERR
					euler_rates = gdc.rates; */
					euler_rates = _V(0,0,0);
					attitude = imu.GetTotalAttitude();
					errors = _V(0,0,0);
					/* errors = eda.ReturnCMCErrorNeedles();
					break;
				case THREEPOSSWITCH_DOWN:   // 1 -- ALTERNATE DIRECT MODE
					euler_rates = gdc.rates;					
					switch(FDAISelectSwitch.GetState()){
						case THREEPOSSWITCH_UP:   // IMU
							attitude = imu.GetTotalAttitude();
							errors = eda.ReturnCMCErrorNeedles();
							break;
						case THREEPOSSWITCH_CENTER: // ATT SET (ALTERNATE ATT-SET MODE)
							// Get attutude
							if(FDAIAttSetSwitch.GetState() == TOGGLESWITCH_UP){
								attitude = imu.GetTotalAttitude();
							}else{
								attitude = gdc.attitude;
							}
							errors = eda.AdjustErrorsForRoll(attitude,eda.ReturnASCPError(attitude));
							break;
						case THREEPOSSWITCH_DOWN: // GDC
							attitude = gdc.attitude;
							errors = eda.ReturnBMAG1Error();
							break;
					}
					break;				
				case THREEPOSSWITCH_CENTER: // 2
					attitude = _V(0,0,0);   // No
					errors = _V(0,0,0);
					euler_rates = gdc.rates;
					// euler_rates = _V(0,0,0); // Does not disconnect rate inputs?
					no_att = 1;
					break;
			}
			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			if(errors.x > 41){ errors.x = 41; }else{ if(errors.x < -41){ errors.x = -41; }}
			if(errors.y > 41){ errors.y = 41; }else{ if(errors.y < -41){ errors.y = -41; }}
			if(errors.z > 41){ errors.z = 41; }else{ if(errors.z < -41){ errors.z = -41; }}
			fdaiLeft.PaintMe(attitude, no_att, euler_rates, errors, FDAIScaleSwitch.GetState(), surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);			
			*/
			if((RateErrorMonSwitch.GetState() == 1 ) && (RR.IsPowered()) ) {
				errors.z = RR.GetRadarTrunnionPos() * 41 / (180 * RAD) ;
				errors.y = RR.GetRadarShaftPos() * 41 / (180 * RAD) ;
			}
			fdaiLeft.PaintMe(attitude, no_att, euler_rates, errors, 0, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);			
		}
		return true;

	case AID_FDAI_RIGHT:
		if (!fdaiDisabled){			
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;
			int no_att = 0;
			euler_rates = _V(0,0,0);
			attitude = imu.GetTotalAttitude();
			errors = _V(0,0,0);
			fdaiRight.PaintMe(attitude, no_att, euler_rates, errors, 0, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);			
		}
		return true;

	case AID_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_DIGITALDISP2]);
		return true;

	case AID_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_DIGITALDISP2]);
		return true;

	case AID_FUEL_DIGIT:
		double fuel;

		// Don't display with no power
		if(RCS_B_PQGS_DISP_CB.Voltage() < 24){ return true; }

		//
		// What should this display with > 99% fuel?
		//

		fuel = actualFUEL;
		if (fuel > 99.0)
			fuel = 99.0;

		//
		// For now we display the same value for fuel and oxidiser.
		//

		Curdigit= (int) fuel/10;
		Curdigit2= (int) fuel/100;

		oapiBlt(surf, srf[SRF_DIGITALDISP2], 0, 0, 19 * Curdigit, 0, 19, 21);
		oapiBlt(surf, srf[SRF_DIGITALDISP2], 0, 52, 19 * Curdigit, 0, 19, 21);

		Curdigit2= (int) fuel;
		oapiBlt(surf, srf[SRF_DIGITALDISP2], 20, 0, 19 * (Curdigit2 - (Curdigit*10)), 0, 19, 21);
		oapiBlt(surf, srf[SRF_DIGITALDISP2], 20, 52, 19 * (Curdigit2 - (Curdigit*10)), 0, 19, 21);

		return true;

	case AID_DESCENT_HE:
		if(DESHE1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],1,30,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,0,0,0,19,20);
		}
		else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],1,30,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,0,38,0,19,20);
		}
		if(DESHE2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],44,30,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],46,0,0,0,19,20);
		}
		else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],44,30,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],46,0,38,0,19,20);
		}
		return true;
/*
	case AID_ENG_ARM:
		if(ENGARMswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		if(ENGARMswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;
*/
	case AID_LGC_THRUSTER_QUADS:
		if(QUAD1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],1,36,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],1,36,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,0,38,0,19,20);
		}
		if(QUAD2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],44,36,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],46,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],44,36,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],46,0,38,0,19,20);
		}
		if(QUAD3switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],92,36,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],94,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],92,36,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],94,0,38,0,19,20);
		}
		if(QUAD4switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],135,36,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],137,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],135,36,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],137,0,38,0,19,20);
		}
		if(QUAD5switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],1,116,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],1,116,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,80,38,0,19,20);
		}
		if(QUAD6switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],44,116,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],46,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],44,116,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],46,80,38,0,19,20);
		}
		if(QUAD7switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],92,116,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],94,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],92,116,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],94,80,38,0,19,20);
		}
		if(QUAD8switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],135,116,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],137,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],135,116,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],137,80,38,0,19,20);
		}
		return true;

	case AID_RCS_SYSTEMA_SYSTEMB:
		if(AFEED1switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,33,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],2,0,0,0,19,20);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,33,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],2,0,38,0,19,20);
		}
		if(AFEED2switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],43,33,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],45,0,0,0,19,20);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],43,33,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],45,0,38,0,19,20);
		}
		if(AFEED3switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],91,33,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],93,0,0,0,19,20);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],91,33,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],93,0,38,0,19,20);
		}
		if(AFEED4switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],134,33,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],136,0,0,0,19,20);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],134,33,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],136,0,38,0,19,20);
		}
		return true;

	/*
	case AID_MAIN_SOV_SWITCHES:
		if(MSOV1switch){
			oapiBlt(surf,srf[19],0,30,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],2,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],0,30,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],2,0,38,0,19,20);
		}
		if(MSOV2switch){
			oapiBlt(surf,srf[19],55,30,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],57,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],55,30,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],57,0,38,0,19,20);
		}
		return true;
	*/

	case AID_EXPLOSIVE_DEVICES1:
		if(ED1switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		if(ED2switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],47,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],47,0,23,0,23,30);
		}
		if(ED4switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],00,68,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],00,68,23,0,23,30);
		}
		if(ED5switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],47,68,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],47,68,23,0,23,30);
		}
		if(ED6switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],94,68,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],94,68,23,0,23,30);
		}
		return true;

	case AID_ATTITUDE_CONTROL_SWITCHES:
		if(ATT1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		if(ATT2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],57,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],57,0,23,0,23,20);
			}
		if(ATT3switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],114,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],114,0,23,0,23,20);
			}
		return true;

	case AID_ENGINE_GIMBAL_SWITCH:
		if(GMBLswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_ASCENT_HE:
		if(ASCHE1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],2,32,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],4,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],2,32,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],4,1,38,0,19,20);
		}
		if(ASCHE2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],45,32,0,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],47,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],45,32,23,0,23,20);
			oapiBlt(surf,srf[SRF_LIGHTS2],47,1,38,0,19,20);
		}
		return true;

	case AID_EXPLOSIVE_DEVICES2:
		if(ED7switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		if(ED8switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],48,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],48,0,23,0,23,30);
		}
		if(ED9switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],94,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],94,0,23,0,23,30);
		}
		return true;

	case AID_ENG_THRUST:
		RedrawPanel_Thrust(surf);
		return true;

	case AID_ENG_FUEL_TEMP:
		double DispValue;

		if(ph_Dsc){
			DispValue = GetPropellantMass(ph_Dsc)/8165;
		}else{
				DispValue =0;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(47-(DispValue)*47),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_Asc){
		DispValue = GetPropellantMass(ph_Asc)/GetPropellantMaxMass(ph_Asc);
		}else{
			DispValue =1;
		}

		oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(47-(DispValue)*47),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_ENG_FUEL_PRESS:
		if(ph_Dsc){
			DispValue = GetPropellantMass(ph_Dsc)/8165;
		}else{
				DispValue =0;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(62-(DispValue)*62),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_Asc){
		DispValue = GetPropellantMass(ph_Asc)/GetPropellantMaxMass(ph_Asc);
		}else{
			DispValue =1;
		}

		oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(62-(DispValue)*62),8,0,7,7, SURF_PREDEF_CK);//
		return true;

		/* DEPRECIATED 
	case AID_RCS_TEMP:
		if(ph_rcslm0){
		DispValue = GetPropellantMass(ph_rcslm0)/100;
		//sprintf(oapiDebugString(),"rcs0lm = true");
		}else{
			DispValue =0;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(49-(DispValue)*49),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_rcslm1){
		DispValue = GetPropellantMass(ph_rcslm1)/100;
		}else{
			DispValue =1;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(49-(DispValue)*49),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_RCS_PRESS:
		if(ph_rcslm0){
		DispValue = GetPropellantMass(ph_rcslm0)/100;
		//sprintf(oapiDebugString(),"rcs0lm = true");
		}else{
			DispValue =0;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_rcslm1){
		DispValue = GetPropellantMass(ph_rcslm1)/100;
		}else{
			DispValue =1;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_RCS_QTY:
		if(ph_rcslm0){
		DispValue = GetPropellantMass(ph_rcslm0)/100;
		//sprintf(oapiDebugString(),"rcs0lm = true");
		}else{
			DispValue =0;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_rcslm1){
		DispValue = GetPropellantMass(ph_rcslm1)/100;
		}else{
			DispValue =1;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
		return true;
	*/

	case AID_LANDING_GEAR_SWITCH:
		if(LDGswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],1,37,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,0,0,0,19,20);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],1,37,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],3,0,38,0,19,20);
		}
		return true;

	case AID_CRSFD_SWITCH:
		if(CRSFDswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,29,0,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],2,0,0,0,19,20);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,29,23,0,23,30);
			oapiBlt(surf,srf[SRF_LIGHTS2],2,0,38,0,19,20);
		}
		return true;

	case AID_CABIN_FAN_SWITCH:
		if(CABFswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PTT_SWITCH:
		if(PTTswitch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_RCS_SYS_AB:
		if(RCSS1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		if(RCSS2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],46,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],46,0,23,0,23,20);
		}
		if(RCSS3switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,59,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,59,23,0,23,20);
		}
		if(RCSS4switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],46,59,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],46,59,23,0,23,20);
		}
		return true;

	case AID_XPOINTER_SWITCH:
		if(X1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_PANEL1_1:
		if(RATE1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		if(AT1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,58,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,58,23,0,23,20);
		}
		return true;

	case AID_GUID_CONT_SWITCH:
		if(GUIDswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_ALT_RNG_MON:
		if(ALTswitch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_SHIFT_SWITCH:
		if(SHFTswitch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_ENGINE_THRUST_CONT:
		if(ETC1switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		if(ETC2switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],47,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],47,0,23,0,23,30);
		}
		if(ETC3switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],14,58,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],14,58,23,0,23,20);
		}
		if(ETC4switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],52,53,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],52,53,23,0,23,30);
		}
		return true;

	case AID_PROP_MON_SWITCHES:
		if(PMON1switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		if(PMON2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],14,55,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],14,55,23,0,23,20);
		}
		return true;

	case AID_ACA_PROP_SWITCH:
		if(ACAPswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL2_1:
		if(RATE2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		if(AT2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,58,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,58,23,0,23,20);
		}
		return true;
/*
	case AID_DESCENT_ENGINE_SWITCH:
		if(DESEswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;
*/

	case AID_SLEW_RATE_SWITCH:
		if(SLWRswitch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_DEAD_BAND_SWITCH:
		if(DBswitch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_IMU_CAGE_SWITCH:
		if(IMUCswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_SIDE_PANEL_SWITCH:
		if(SPLswitch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_XPOINTER2_SWITCH:
		if(X2switch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_PANEL4_SWITCH1:
		if(P41switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL4_SWITCH2:
		if(P42switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL4_SWITCH3:
		if(P43switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL4_SWITCH4:
		if(P44switch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_AUDIO_CONT_SWITCH:
		if(AUDswitch){
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,0,0,23,20);
		}
		else{
			oapiBlt(surf,srf[SRF_LEMSWITCH1],0,0,23,0,23,20);
		}
		return true;

	case AID_RELAY_AUDIO_SWITCH:
		if(RELswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_CABIN_PRESS_SWITCH:
		if(CPswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_HATCH_SWITCH:
		if(HATCHswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_EVA_SWITCH:
		if(EVAswitch){
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			// UNINITIALIZED USE OF srf[19]
			// oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_ECS_GAUGES:
		if(CPswitch){
			oapiBlt(surf,srf[0],0,0,0,0,145,72);
		}
		return true;

	case AID_AOT_RETICLE:
		RedrawPanel_AOTReticle(surf);
		return true;

	case AID_AOT_RETICLE_KNOB:
		oapiBlt(surf,srf[SRF_AOTRETICLEKNOB],0,0,optics.KnobTurning*74,0,74,326);
		return true;
	
	case AID_AOT_SHAFT_KNOB:
		oapiBlt(surf,srf[SRF_AOTSHAFTKNOB],0,0,optics.OpticsShaft*62,0,62,155);
		
		oapiCameraSetCockpitDir (optics.OpticsShaft*PI/3+PI/2,PI/4,true);
		return true;

	case AID_COAS:
		if(COASswitch){
			oapiBlt(surf,srf[0],0,0,146,0,301,298);
		}
		else{
			oapiBlt(surf,srf[0],0,0,448,0,301,298);
		}
		return true;

	case AID_SWITCH_JET:
		if(Cswitch2){
			oapiBlt(surf,srf[SRF_SECSWITCH],0,0,75,0,25,45);
			if(Sswitch2){
				oapiBlt(surf,srf[SRF_LEMSWTICH3],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[SRF_LEMSWTICH3],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[SRF_SECSWITCH],0,0,50,0,25,45);
			Sswitch2=false;
		}
		return true;
	case AID_RANGE_TAPE:
		RadarTape.RenderRange(surf, srf[SRF_RADAR_TAPE]);
		return true;
	case AID_RATE_TAPE:
		RadarTape.RenderRate(surf, srf[SRF_RADAR_TAPE]);
		return true;

	}
	return false;
}
//
// The switch functions just pack the different switch areas from the control panel
// into 32-bit integers and unpack them from those integers. This provides a much more
// compact means of storing the switch states in the scenario file than saving them as
// individual values for each switch.
//

typedef union {
	struct {
		unsigned Cswitch1:1;
		unsigned Cswitch2:1;
		unsigned Cswitch4:1;
		unsigned Cswitch5:1;
		unsigned Cswitch6:1;
		unsigned Cswitch7:1;
		unsigned Cswitch8:1;
		unsigned Cswitch9:1;
		unsigned DESHE1switch:1;
		unsigned DESHE2switch:1;
		unsigned ASCHE1switch:1;
		unsigned ASCHE2switch:1;
		unsigned CRSFDswitch:1;
		unsigned CABFswitch:1;
		unsigned PTTswitch:1;
		unsigned RCSS1switch:1;
		unsigned RCSS2switch:1;
		unsigned RCSS3switch:1;
		unsigned RCSS4switch:1;
		unsigned P41switch:1;
		unsigned P42switch:1;
		unsigned P43switch:1;
		unsigned P44switch:1;
		unsigned AUDswitch:1;
		unsigned RELswitch:1;
		unsigned CPswitch:1;
		unsigned HATCHswitch:1;
		unsigned EVAswitch:1;
		unsigned COASswitch:1;
		unsigned Abortswitch:1;

	} u;
	unsigned long word;
} CSwitchState;

int LEM::GetCSwitchState()

{
	CSwitchState state;

	state.word = 0;
	state.u.Cswitch1 = Cswitch1;
	state.u.Cswitch2 = Cswitch2;
	state.u.Cswitch4 = Cswitch4;
	state.u.Cswitch5 = Cswitch5;
	state.u.Cswitch6 = Cswitch6;
	state.u.Cswitch7 = Cswitch7;
	state.u.Cswitch8 = Cswitch8;
	state.u.Cswitch9 = Cswitch9;
	state.u.DESHE1switch = DESHE1switch;
	state.u.DESHE2switch = DESHE2switch;
	state.u.ASCHE1switch = ASCHE1switch;
	state.u.ASCHE2switch = ASCHE2switch;
	state.u.CRSFDswitch = CRSFDswitch;
	state.u.CABFswitch = CABFswitch;
	state.u.PTTswitch = PTTswitch;
	state.u.RCSS1switch = RCSS1switch;
	state.u.RCSS2switch = RCSS2switch;
	state.u.RCSS3switch = RCSS3switch;
	state.u.RCSS4switch = RCSS4switch;
	state.u.P41switch = P41switch;
	state.u.P42switch = P42switch;
	state.u.P43switch = P43switch;
	state.u.AUDswitch = AUDswitch;
	state.u.RELswitch = RELswitch;
	state.u.CPswitch = CPswitch;
	state.u.HATCHswitch = HATCHswitch;
	state.u.EVAswitch = EVAswitch;
	state.u.COASswitch = COASswitch;
	state.u.Abortswitch = AbortStageSwitchLight;

	return state.word;
}

void LEM::SetCSwitchState(int s)

{
	CSwitchState state;

	state.word = s;
	Cswitch1 = state.u.Cswitch1;
	Cswitch2 = state.u.Cswitch2;
	Cswitch4 = state.u.Cswitch4;
	Cswitch5 = state.u.Cswitch5;
	Cswitch6 = state.u.Cswitch6;
	Cswitch7 = state.u.Cswitch7;
	Cswitch8 = state.u.Cswitch8;
	Cswitch9 = state.u.Cswitch9;
	DESHE1switch = state.u.DESHE1switch;
	DESHE2switch = state.u.DESHE2switch;
	ASCHE1switch = state.u.ASCHE1switch;
	ASCHE2switch = state.u.ASCHE2switch;
	CRSFDswitch = state.u.CRSFDswitch;
	CABFswitch = state.u.CABFswitch;
	PTTswitch = state.u.PTTswitch;
	RCSS1switch = state.u.RCSS1switch;
	RCSS2switch = state.u.RCSS2switch;
	RCSS3switch = state.u.RCSS3switch;
	RCSS4switch = state.u.RCSS4switch;
	P41switch = state.u.P41switch;
	P42switch = state.u.P42switch;
	P43switch = state.u.P43switch;
	P44switch = state.u.P44switch;
	AUDswitch = state.u.AUDswitch;
	RELswitch = state.u.RELswitch;
	CPswitch = state.u.CPswitch;
	HATCHswitch = state.u.HATCHswitch;
	EVAswitch = state.u.EVAswitch;
	COASswitch = state.u.COASswitch;
	AbortStageSwitchLight = state.u.Abortswitch;
}

typedef union {
	struct {
		unsigned Sswitch1:1;
		unsigned Sswitch2:1;
		unsigned Sswitch4:1;
		unsigned Sswitch5:1;
		unsigned Sswitch6:1;
		unsigned Sswitch7:1;
		unsigned Sswitch8:1;
		unsigned Sswitch9:1;
		unsigned X1switch:1;
		unsigned RATE1switch:1;
		unsigned AT1switch:1;
		unsigned GUIDswitch:1;
		unsigned ALTswitch:1;
		unsigned SHFTswitch:1;
		unsigned ETC1switch:1;
		unsigned ETC2switch:1;
		unsigned ETC3switch:1;
		unsigned ETC4switch:1;
		unsigned PMON1switch:1;
		unsigned PMON2switch:1;
		unsigned ACAPswitch:1;
		unsigned RATE2switch:1;
		unsigned AT2switch:1;
		unsigned dummy:1 ;  // formerly unsigned DESEswitch:1;
		unsigned SLWRswitch:1;
		unsigned DBswitch:1;
		unsigned IMUCswitch:1;
		unsigned SPLswitch:1;
		unsigned X2switch:1;
	} u;
	unsigned long word;
} SSwitchState;

int LEM::GetSSwitchState()

{
	SSwitchState state;

	state.word = 0;
	state.u.Sswitch1 = Sswitch1;
	state.u.Sswitch2 = Sswitch2;
	state.u.Sswitch4 = Sswitch4;
	state.u.Sswitch5 = Sswitch5;
	state.u.Sswitch6 = Sswitch6;
	state.u.Sswitch7 = Sswitch7;
	state.u.Sswitch8 = Sswitch8;
	state.u.Sswitch9 = Sswitch9;
	state.u.X1switch = X1switch;
	state.u.RATE1switch = RATE1switch;
	state.u.AT1switch = AT1switch;
	state.u.GUIDswitch = GUIDswitch;
	state.u.ALTswitch = ALTswitch;
	state.u.SHFTswitch = SHFTswitch;
	state.u.ETC1switch = ETC1switch;
	state.u.ETC2switch = ETC2switch;
	state.u.ETC3switch = ETC3switch;
	state.u.ETC4switch = ETC4switch;
	state.u.PMON1switch = PMON1switch;
	state.u.PMON2switch = PMON2switch;
	state.u.ACAPswitch = ACAPswitch;
	state.u.RATE2switch = RATE2switch;
	state.u.AT2switch = AT2switch;
//	state.u.DESEswitch = DESEswitch;
	state.u.SLWRswitch = SLWRswitch;
	state.u.DBswitch = DBswitch;
	state.u.IMUCswitch = IMUCswitch;
	state.u.SPLswitch = SPLswitch;
	state.u.X2switch = X2switch;

	return state.word;
}

void LEM::SetSSwitchState(int s)

{
	SSwitchState state;

	state.word = s;
	Sswitch1 = state.u.Sswitch1;
	Sswitch2 = state.u.Sswitch2;
	Sswitch4 = state.u.Sswitch4;
	Sswitch5 = state.u.Sswitch5;
	Sswitch6 = state.u.Sswitch6;
	Sswitch7 = state.u.Sswitch7;
	Sswitch8 = state.u.Sswitch8;
	Sswitch9 = state.u.Sswitch9;
	X1switch = state.u.X1switch;
	RATE1switch = state.u.RATE1switch;
	AT1switch = state.u.AT1switch;
	GUIDswitch = state.u.GUIDswitch;
	ALTswitch = state.u.ALTswitch;
	SHFTswitch = state.u.SHFTswitch;
	ETC1switch = state.u.ETC1switch;
	ETC2switch = state.u.ETC2switch;
	ETC3switch = state.u.ETC3switch;
	ETC4switch = state.u.ETC4switch;
	PMON1switch = state.u.PMON1switch;
	PMON2switch = state.u.PMON2switch;
	ACAPswitch = state.u.ACAPswitch;
	RATE2switch = state.u.RATE2switch;
	AT2switch = state.u.AT2switch;
//	DESEswitch = state.u.DESEswitch;
	SLWRswitch = state.u.SLWRswitch;
	DBswitch = state.u.DBswitch;
	IMUCswitch = state.u.IMUCswitch;
	SPLswitch = state.u.SPLswitch;
	X2switch = state.u.X2switch;
}

typedef union {
	struct {
		unsigned ATT2switch:1;
		unsigned ATT3switch:1;
		unsigned ATT1switch:1;
		unsigned LPswitch4:1;
		unsigned LPswitch5:1;
		unsigned GMBLswitch:1;
		unsigned dummy:1;
		unsigned LDGswitch:1;
		unsigned QUAD1switch:1;
		unsigned QUAD2switch:1;
		unsigned QUAD3switch:1;
		unsigned QUAD4switch:1;
		unsigned QUAD5switch:1;
		unsigned QUAD6switch:1;
		unsigned QUAD7switch:1;
		unsigned QUAD8switch:1;
		unsigned AFEED1switch:1;
		unsigned AFEED2switch:1;
		unsigned AFEED3switch:1;
		unsigned AFEED4switch:1;
		unsigned MSOV1switch:1;
		unsigned MSOV2switch:1;
		unsigned ED1switch:1;
		unsigned ED2switch:1;
		unsigned ED4switch:1;
		unsigned ED5switch:1;
		unsigned ED6switch:1;
		unsigned ED7switch:1;
		unsigned ED8switch:1;
		unsigned ED9switch:1;
	} u;
	unsigned long word;
} LPSwitchState;

int LEM::GetLPSwitchState()

{
	LPSwitchState state;

	state.word = 0;
	state.u.ATT2switch = ATT2switch;
	state.u.ATT3switch = ATT3switch;
	state.u.ATT1switch = ATT1switch;
	state.u.LPswitch4 = LPswitch4;
	state.u.LPswitch5 = LPswitch5;
	state.u.GMBLswitch = GMBLswitch;
//	state.u.ENGARMswitch = ENGARMswitch;
	state.u.LDGswitch = LDGswitch;
	state.u.QUAD1switch = QUAD1switch;
	state.u.QUAD2switch = QUAD2switch;
	state.u.QUAD3switch = QUAD3switch;
	state.u.QUAD4switch = QUAD4switch;
	state.u.QUAD5switch = QUAD5switch;
	state.u.QUAD6switch = QUAD6switch;
	state.u.QUAD7switch = QUAD7switch;
	state.u.QUAD8switch = QUAD8switch;
	state.u.AFEED1switch = AFEED1switch;
	state.u.AFEED2switch = AFEED2switch;
	state.u.AFEED3switch = AFEED3switch;
	state.u.AFEED4switch = AFEED4switch;
	state.u.ED1switch = ED1switch;
	state.u.ED2switch = ED2switch;
	state.u.ED4switch = ED4switch;
	state.u.ED5switch = ED5switch;
	state.u.ED6switch = ED6switch;
	state.u.ED7switch = ED7switch;
	state.u.ED8switch = ED8switch;
	state.u.ED9switch = ED9switch;

	return state.word;
}

void LEM::SetLPSwitchState(int s)

{
	LPSwitchState state;

	state.word = s;
	ATT2switch = state.u.ATT2switch;
	ATT3switch = state.u.ATT3switch;
	ATT1switch = state.u.ATT1switch;
	LPswitch4 = state.u.LPswitch4;
	LPswitch5 = state.u.LPswitch5;
	GMBLswitch = state.u.GMBLswitch;
//	ENGARMswitch = state.u.ENGARMswitch;
	LDGswitch = state.u.LDGswitch;
	QUAD1switch = state.u.QUAD1switch;
	QUAD2switch = state.u.QUAD2switch;
	QUAD3switch = state.u.QUAD3switch;
	QUAD4switch = state.u.QUAD4switch;
	QUAD5switch = state.u.QUAD5switch;
	QUAD6switch = state.u.QUAD6switch;
	QUAD7switch = state.u.QUAD7switch;
	QUAD8switch = state.u.QUAD8switch;
	AFEED1switch = state.u.AFEED1switch;
	AFEED2switch = state.u.AFEED2switch;
	AFEED3switch = state.u.AFEED3switch;
	AFEED4switch = state.u.AFEED4switch;
	ED1switch = state.u.ED1switch;
	ED2switch = state.u.ED2switch;
	ED4switch = state.u.ED4switch;
	ED5switch = state.u.ED5switch;
	ED6switch = state.u.ED6switch;
	ED7switch = state.u.ED7switch;
	ED8switch = state.u.ED8switch;
	ED9switch = state.u.ED9switch;

}

typedef union {
	struct {
		unsigned RPswitch1:1;
		unsigned RPswitch2:1;
		unsigned RPswitch3:1;
		unsigned RPswitch4:1;
		unsigned RPswitch5:1;
		unsigned RPswitch6:1;
		unsigned RPswitch7:1;
		unsigned RPswitch8:1;
		unsigned RPswitch9:1;
		unsigned RPswitch10:1;
		unsigned RPswitch11:1;
		unsigned RPswitch12:1;
		unsigned RPswitch13:1;
		unsigned RPswitch14:1;
		unsigned RPswitch15:1;
		unsigned RPswitch16:1;
		unsigned RPswitch17:1;
	} u;
	unsigned long word;
} RPSwitchState;

int LEM::GetRPSwitchState()

{
	RPSwitchState state;

	state.word = 0;
	state.u.RPswitch1 = RPswitch1;
	state.u.RPswitch2 = RPswitch2;
	state.u.RPswitch3 = RPswitch3;
	state.u.RPswitch4 = RPswitch4;
	state.u.RPswitch5 = RPswitch5;
	state.u.RPswitch6 = RPswitch6;
	state.u.RPswitch7 = RPswitch7;
	state.u.RPswitch8 = RPswitch8;
	state.u.RPswitch9 = RPswitch9;
	state.u.RPswitch10 = RPswitch10;
	state.u.RPswitch11 = RPswitch11;
	state.u.RPswitch12 = RPswitch12;
	state.u.RPswitch13 = RPswitch13;
	state.u.RPswitch14 = RPswitch14;
	state.u.RPswitch15 = RPswitch15;
	state.u.RPswitch16 = RPswitch16;

	return state.word;
}

void LEM::SetRPSwitchState(int s)

{
	RPSwitchState state;

	state.word = s;
	RPswitch1 = state.u.RPswitch1;
	RPswitch2 = state.u.RPswitch2;
	RPswitch3 = state.u.RPswitch3;
	RPswitch4 = state.u.RPswitch4;
	RPswitch5 = state.u.RPswitch5;
	RPswitch6 = state.u.RPswitch6;
	RPswitch7 = state.u.RPswitch7;
	RPswitch8 = state.u.RPswitch8;
	RPswitch9 = state.u.RPswitch9;
	RPswitch10 = state.u.RPswitch10;
	RPswitch11 = state.u.RPswitch11;
	RPswitch12 = state.u.RPswitch12;
	RPswitch13 = state.u.RPswitch13;
	RPswitch14 = state.u.RPswitch14;
	RPswitch15 = state.u.RPswitch15;
	RPswitch16 = state.u.RPswitch16;
}
