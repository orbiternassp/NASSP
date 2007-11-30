/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant, Matthias MÅEler

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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.69  2007/06/06 15:02:14  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.68  2006/09/23 01:55:15  flydba
  *	Final graphics update completed.
  *	
  *	Revision 1.67  2006/08/21 03:04:38  dseagrav
  *	This patch adds DC volt/amp meters and associated switches, which was an unholy pain in the
  *	
  *	Revision 1.66  2006/08/18 05:45:01  dseagrav
  *	LM EDS now exists. Talkbacks wired to a power source will revert to BP when they lose power.
  *	
  *	Revision 1.65  2006/08/13 16:55:35  movieman523
  *	Removed a bunch of unused files.
  *	
  *	Revision 1.64  2006/08/13 16:01:52  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  *	Revision 1.63  2006/08/09 00:47:03  flydba
  *	Last missing COAS added on the left hand side window.
  *	
  *	Revision 1.62  2006/07/24 06:41:29  dseagrav
  *	Many changes - Rearranged / corrected FDAI power usage, added LM AC equipment, many bugfixes
  *	
  *	Revision 1.61  2006/07/16 16:20:52  flydba
  *	COAS on the overhead rendezvous window now works.
  *	
  *	Revision 1.60  2006/07/16 02:53:13  flydba
  *	New COAS added for the rendezvous window.
  *	
  *	Revision 1.59  2006/06/18 22:45:31  dseagrav
  *	LM ECA bug fix, LGC,IMU,DSKY and IMU OPR wired to CBs, IMU OPR,LGC,FDAI,and DSKY draw power
  *	
  *	Revision 1.58  2006/06/18 16:43:07  dseagrav
  *	LM EPS fixes, LMP/CDR DC busses now powered thru CBs, ECA power-off bug fixed and ECA speed improvement
  *	
  *	Revision 1.57  2006/06/11 09:20:30  dseagrav
  *	LM ECA #2 added, ECA low-voltage tap usage added, CDR & LMP DC busses wired to ECAs
  *	
  *	Revision 1.56  2006/05/01 08:52:49  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.55  2006/04/25 08:11:27  dseagrav
  *	Crash avoidance for DEBUG builds, LM IMU correction, LM still needs more work
  *	
  *	Revision 1.54  2006/04/24 20:58:18  jasonims
  *	Got the X-Pointers working on the main-panel.
  *	
  *	Revision 1.53  2006/04/23 04:15:45  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.52  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  *	Revision 1.51  2006/04/17 15:16:16  movieman523
  *	Beginnings of checklist code, added support for flashing borders around control panel switches and updated a portion of the Saturn panel switches appropriately.
  *	
  *	Revision 1.50  2006/04/12 06:27:19  dseagrav
  *	LM checkpoint commit. The LM is not airworthy at this point. Please be patient.
  *	
  *	Revision 1.49  2006/03/23 11:59:48  tschachim
  *	Bugfix switch surfaces.
  *	
  *	Revision 1.48  2006/03/08 02:24:21  movieman523
  *	Added event timer and fuel display.
  *	
  *	Revision 1.47  2006/03/07 02:18:17  flydba
  *	Circuit breakers added to panel 11.
  *	
  *	Revision 1.46  2006/03/04 20:49:08  flydba
  *	Circuit breaker resource added to the LEM panel code.
  *	
  *	Revision 1.45  2006/03/04 13:41:48  flydba
  *	Switches added on LEM panels 3 & 4
  *	
  *	Revision 1.44  2006/01/29 00:47:23  flydba
  *	Switches added on LEM panel 2.
  *	
  *	Revision 1.43  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.42  2006/01/22 16:34:01  flydba
  *	Switches added on LEM panel 1.
  *	
  *	Revision 1.41  2006/01/19 14:47:43  tschachim
  *	Bugfix abort buttons.
  *	
  *	Revision 1.40  2006/01/18 20:11:28  flydba
  *	Contact lights added.
  *	
  *	Revision 1.39  2005/11/28 01:13:53  movieman523
  *	Added LEM right-hand panel.
  *	
  *	Revision 1.38  2005/10/31 10:16:10  tschachim
  *	LEM rendezvous window camera direction.
  *	
  *	Revision 1.37  2005/10/13 15:48:38  tschachim
  *	Removed comment regarding the panel change bug.
  *	
  *	Revision 1.36  2005/09/30 11:22:00  tschachim
  *	New panel event handler.
  *	
  *	Revision 1.35  2005/09/22 00:39:09  flydba
  *	AOT - Alignment Optical Telescope view and left instrument panel added to the lem
  *	
  *	Revision 1.34  2005/09/18 23:15:20  flydba
  *	Lem overhead rendezvous window added...
  *	
  *	Revision 1.33  2005/08/31 10:24:51  spacex15
  *	fixed display problems on lem switches imu cage and Descent Engine command override
  *	
  *	Revision 1.32  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.31  2005/08/29 21:53:38  spacex15
  *	fixed broken LM switch display
  *	
  *	Revision 1.30  2005/08/29 19:14:13  tschachim
  *	Rendering of the DSKY keys.
  *	
  *	Revision 1.29  2005/08/19 13:42:54  tschachim
  *	Added missing DSKY display elements.
  *	
  *	Revision 1.28  2005/08/14 16:08:20  tschachim
  *	LM is now a VESSEL2
  *	Changed panel restore mechanism because the CSM mechanism
  *	caused CTDs, reason is still unknown.
  *	
  *	Revision 1.27  2005/08/13 20:20:17  movieman523
  *	Created MissionTimer class and wired it into the LEM and CSM.
  *	
  *	Revision 1.26  2005/08/12 10:06:05  spacex15
  *	added connection of PNGS mode control switch to the agc input channel
  *	
  *	Revision 1.25  2005/08/11 16:29:33  spacex15
  *	Added PNGS and AGS mode control 3 pos switches
  *	
  *	Revision 1.24  2005/08/11 14:00:34  spacex15
  *	Added Descent Engine Command Override switch
  *	
  *	Revision 1.23  2005/08/11 01:27:26  movieman523
  *	Added initial Virtual AGC support.
  *	
  *	Revision 1.22  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.21  2005/08/10 20:29:49  spacex15
  *	added Agc input signal when eng arm switch changes
  *	
  *	Revision 1.20  2005/08/10 20:00:55  spacex15
  *	Activated 3 position lem eng arm switch
  *	
  *	Revision 1.19  2005/08/09 09:13:51  tschachim
  *	Introduced toggleswitch lib
  *	Added MFDs
  *	Added new LPD window with X-Pointer
  *	
  *	Revision 1.18  2005/08/06 00:03:48  movieman523
  *	Beginnings of support for AGC I/O channels in LEM.
  *	
  *	Revision 1.17  2005/08/05 21:45:48  spacex15
  *	reactivation of Abort and Abort stage buttons
  *	
  *	Revision 1.16  2005/08/04 01:06:03  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.15  2005/08/01 21:50:03  lazyd
  *	Added call to check which AGC program is running
  *	
  *	Revision 1.14  2005/07/16 20:41:26  lazyd
  *	Made the Abort and Abort Stage buttons run P70 and P71
  *	
  *	Revision 1.13  2005/06/17 18:28:46  lazyd
  *	Added crosspointer instrument, FDAI and contact light to leftwindow panel
  *	
  *	Revision 1.12  2005/06/15 20:30:47  lazyd
  *	Added code to save original FOV correctly the first time leftwindow panel is used and to restore the original FOV and view direction for any other panel
  *	
  *	Revision 1.11  2005/06/15 15:58:00  lazyd
  *	Change FOV to 70 degrees for left window panel - back to 60 for other panels
  *	This is a temporary thing until I figure out how do deal with generic panel etc.
  *	
  *	Revision 1.10  2005/06/14 15:41:58  henryhallam
  *	temporarily added LPD angle display
  *	
  *	Revision 1.9  2005/06/14 14:31:52  lazyd
  *	Changed view angle for leftwindow and ilmleftwindow to 30 degrees down
  *	Recoded view vector generation in a simpler way
  *	
  *	Revision 1.8  2005/06/14 13:11:30  henryhallam
  *	changed left window panel direction to be better for LPD
  *	
  *	Revision 1.7  2005/06/13 21:12:11  lazyd
  *	Changed camera direction in left panel to 45 degrees down for landing
  *	
  *	Revision 1.6  2005/05/27 16:54:42  tschachim
  *	"ugly but working" DSKY on the left LM panel
  *	
  *	Revision 1.5  2005/05/27 15:43:08  tschachim
  *	Fixed bug: virtual engines are always on
  *	
  *	Revision 1.4  2005/05/05 21:31:56  tschachim
  *	removed unused srf[1]
  *	
  *	Revision 1.3  2005/04/22 14:00:53  tschachim
  *	Fixed renamed surfaces
  *	
  *	Revision 1.2  2005/04/11 23:45:04  yogenfrutz
  *	splitted panel + panel_id
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
#include "resource.h"

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


//
// Initialise panel to default state.
//

void LEM::InitPanel() {

	AbortSwitch.Register     (PSH, "AbortSwitch", false);
	AbortStageSwitch.Register(PSH, "AbortStageSwitch", false);
	AbortStageSwitchLight = false;

	EngineArmSwitch.Register(PSH, "EngineArmSwitch", THREEPOSSWITCH_CENTER);
	EngineDescentCommandOverrideSwitch.Register(PSH, "EngineDescentCommandOverrideSwitch", TOGGLESWITCH_DOWN);
	ModeControlPNGSSwitch.Register(PSH,"ModeControlPNGSSwitch", THREEPOSSWITCH_CENTER);
	ModeControlAGSSwitch.Register(PSH,"ModeControlAGSSwitch", THREEPOSSWITCH_CENTER);
	IMUCageSwitch.Register(PSH,"IMUCageSwitch", TOGGLESWITCH_DOWN);
	LeftXPointerSwitch.Register(PSH, "LeftXPointerSwitch", true);
	GuidContSwitch.Register(PSH, "GuidContSwitch", true);
	ModeSelSwitch.Register(PSH, "ModeSelSwitch", THREEPOSSWITCH_UP);
	AltRngMonSwitch.Register(PSH, "AltRngMonSwitch", true);
	RateErrorMonSwitch.Register(PSH, "RateErrorMonSwitch", true);
	AttitudeMonSwitch.Register(PSH, "AttitudeMonSwitch", true);
	ShiftTruSwitch.Register(PSH, "ShiftTruSwitch", true);
	RateScaleSwitch.Register(PSH, "RateScaleSwitch", false);
	ACAPropSwitch.Register(PSH, "ACAPropSwitch", true);
	RCSMainSovATB.Register(PSH, "RCSMainSovATB", true);
	RCSMainSovBTB.Register(PSH, "RCSMainSovBTB", true);
	RCSMainSovASwitch.Register(PSH, "RCSMainSOVASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSMainSovBSwitch.Register(PSH, "RCSMainSOVBSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	THRContSwitch.Register(PSH, "THRContSwitch", true);
	MANThrotSwitch.Register(PSH, "MANThrotSwitch", true);
	ATTTranslSwitch.Register(PSH, "ATTTranslSwitch", true);
	BALCPLSwitch.Register(PSH, "BALCPLSwitch", true);
	QTYMonSwitch.Register(PSH, "QTYMonSwitch", THREEPOSSWITCH_DOWN);
	TempPressMonSwitch.Register(PSH, "TempPressMonSwitch", THREEPOSSWITCH_UP);
	RightRateErrorMonSwitch.Register(PSH, "RightRateErrorMonSwitch", true);
	RightAttitudeMonSwitch.Register(PSH, "RightAttitudeMonSwitch", true);
	RightACAPropSwitch.Register(PSH, "RightACAPropSwitch", false);
	LandingAntSwitch.Register(PSH, "LandingAntSwitch",  THREEPOSSWITCH_UP);
	RadarTestSwitch.Register(PSH, "RadarTestSwitch",  THREEPOSSWITCH_UP);
	SlewRateSwitch.Register(PSH, "SlewRateSwitch", true);
	DeadBandSwitch.Register(PSH, "DeadBandSwitch", false);
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
	FloodSwitch.Register(PSH, "FloodSwitch",  THREEPOSSWITCH_CENTER);
	RightXPointerSwitch.Register(PSH, "RightXPointerSwitch", true);
	ExteriorLTGSwitch.Register(PSH, "ExteriorLTGSwitch", THREEPOSSWITCH_UP);
	LeftACA4JetSwitch.Register(PSH, "LeftACA4JetSwitch", false);
	LeftTTCATranslSwitch.Register(PSH, "LeftTTCATranslSwitch", false);
	RightACA4JetSwitch.Register(PSH, "RightACA4JetSwitch", false);
	RightTTCATranslSwitch.Register(PSH, "RightTTCATranslSwitch", false);

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
	ClycolRotary.Register(PSH, "ClycolRotary", 1);
	
	SuitFanRotary.AddPosition(0,  45);
	SuitFanRotary.AddPosition(1,  70);
	SuitFanRotary.AddPosition(2, 110);
	SuitFanRotary.Register(PSH, "SuitFanRotary", 0);

	QtyMonRotary.AddPosition(0, 340);
	QtyMonRotary.AddPosition(1,  20);
	QtyMonRotary.AddPosition(2,  45);
	QtyMonRotary.AddPosition(3,  70);
	QtyMonRotary.Register(PSH, "QtyMonRotary", 1);

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
	RendezvousRadarRotary.Register(PSH, "RendezvousRadarRotary", 0);

	TempMonitorRotary.AddPosition(0, 315);
	TempMonitorRotary.AddPosition(1, 340);
	TempMonitorRotary.AddPosition(2,  20);
	TempMonitorRotary.AddPosition(3,  45);
	TempMonitorRotary.AddPosition(4,  70);
	TempMonitorRotary.AddPosition(5, 110);
	TempMonitorRotary.AddPosition(6, 135);
	TempMonitorRotary.Register(PSH, "TempMonitorRotary", 0);

	FloodRotary.AddPosition(0, 240);
	FloodRotary.AddPosition(1, 270);
	FloodRotary.AddPosition(2, 300);
	FloodRotary.AddPosition(3, 330);
	FloodRotary.AddPosition(4, 360);
	FloodRotary.AddPosition(5,  30);
	FloodRotary.AddPosition(6,  60);
	FloodRotary.AddPosition(7,  90);
	FloodRotary.AddPosition(8, 120);
	FloodRotary.Register(PSH, "FloodRotary", 0);

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
	EPSMonitorSelectRotary.SetSource(1, Battery1);
	EPSMonitorSelectRotary.SetSource(2, Battery2);
	EPSMonitorSelectRotary.SetSource(3, Battery3);
	EPSMonitorSelectRotary.SetSource(4, Battery4);
	EPSMonitorSelectRotary.SetSource(5, Battery5);
	EPSMonitorSelectRotary.SetSource(6, Battery6);
	EPSMonitorSelectRotary.SetSource(7, &CDRs28VBus);
	EPSMonitorSelectRotary.SetSource(8, &LMPs28VBus);
	EPSMonitorSelectRotary.SetSource(9, &ACVoltsAttenuator);
	
	EPSMonitorSelectRotary.Register(PSH,"EPSMonitorSelectRotary",0);

	EPSDCVoltMeter.Register(PSH,"EPSDCVoltMeter", 19, 42, 3);
	EPSDCAmMeter.Register(PSH,"EPSDCAmMeter", 0, 120, 3);
	DSCBattery1TB.Register(PSH, "DSCBattery1TB", true);
	DSCBattery2TB.Register(PSH, "DSCBattery2TB", true);
	DSCBattery3TB.Register(PSH, "DSCBattery3TB", true);
	DSCBattery4TB.Register(PSH, "DSCBattery4TB", true);
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

	EDS_CB_LOGIC_B.Register(PSH,"EDS_CB_LOGIC_B",1);
	LMPInverter2CB.Register(PSH,"LMPInverter2CB",1);
	LMPBatteryFeedTieCB1.Register(PSH, "LMPBatteryFeedTieCB1", 1);
	LMPBatteryFeedTieCB2.Register(PSH, "LMPBatteryFeedTieCB2", 1);
	CDRBatteryFeedTieCB1.Register(PSH, "CDRBatteryFeedTieCB1", 1);
	CDRBatteryFeedTieCB2.Register(PSH, "CDRBatteryFeedTieCB2", 1);
	CDRInverter1CB.Register(PSH,"CDRInverter1CB",1);
	
	EDMasterArm.Register(PSH,"EDMasterArm",TOGGLESWITCH_DOWN);
	EDDesVent.Register(PSH,"EDDesVent",TOGGLESWITCH_DOWN);
	EDASCHeSel.Register(PSH,"EDASCHeSel",THREEPOSSWITCH_CENTER);
	EDDesPrpIsol.Register(PSH,"EDDesPrpIsol",TOGGLESWITCH_DOWN);
	EDLGDeploy.Register(PSH,"EDLGDeploy",TOGGLESWITCH_DOWN);
	EDHePressRCS.Register(PSH,"EDHePressRCS",TOGGLESWITCH_DOWN);
	EDHePressDesStart.Register(PSH,"EDHePressDesStart",TOGGLESWITCH_DOWN);
	EDHePressASC.Register(PSH,"EDHePressASC",TOGGLESWITCH_DOWN);
	EDStage.Register(PSH,"EDStage",TOGGLESWITCH_DOWN);
	EDStageRelay.Register(PSH,"EDStageRelay",TOGGLESWITCH_DOWN);
	EDDesFuelVent.Register(PSH,"EDDesFuelVent",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EDDesOxidVent.Register(PSH,"EDDesOxidVent",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EDLGTB.Register(PSH, "EDLGTB", true);
	EDDesFuelVentTB.Register(PSH, "EDDesFuelVentTB", true);
	EDDesOxidVentTB.Register(PSH, "EDDesOxidVentTB", true);

	AC_A_INV_1_FEED_CB.Register(PSH,"AC_A_INV_1_FEED_CB",1);
	AC_A_INV_2_FEED_CB.Register(PSH,"AC_A_INV_2_FEED_CB",1);
	AC_B_INV_1_FEED_CB.Register(PSH,"AC_B_INV_1_FEED_CB",1);
	AC_B_INV_2_FEED_CB.Register(PSH,"AC_B_INV_2_FEED_CB",1);

	CDR_FDAI_DC_CB.Register(PSH,"CDR_FDAI_DC_CB",1);
	CDR_FDAI_AC_CB.Register(PSH,"CDR_FDAI_AC_CB",1);

	EDS_CB_LOGIC_A.Register(PSH,"EDS_CB_LOGIC_A",1);
	EDS_CB_LG_FLAG.Register(PSH,"EDS_CB_LG_FLAG",1);

	IMU_OPR_CB.Register(PSH, "IMU_OPR_CB", 1);
	LGC_DSKY_CB.Register(PSH, "LGC_DSKY_CB", 1);

	LEMCoas1Enabled = false;
	LEMCoas2Enabled = true;
	
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
	RPswitch17=false;
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
			oapiBlt (surf, srf[3], size-5+(int)xlr, size-3+(int)ylr, 9*lb, 96, 9, 7, SURF_PREDEF_CK);
			oapiBlt (surf, srf[3], size-5+(int)xll, size-3+(int)yll, 9*lb, 96, 9, 7, SURF_PREDEF_CK);
		}
		xlr -= dsinb, ylr += dcosb;
		xll -= dsinb, yll += dcosb;
	}

	// now overlay markings with transparent blt
	oapiBlt (surf, srf[3], 0, 0, 0, 0, 96, 96, SURF_PREDEF_CK);
	oapiBlt (surf, srf[5], 0, 0, 0, 0, 96, 96, SURF_PREDEF_CK);
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

	oapiBlt(surf,srf[2],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
	oapiBlt(surf,srf[2],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
}

void LEM::RedrawPanel_XPointer (SURFHANDLE surf) {

	int ix, iy;
	double vx, vy;
	HDC hDC;

	//draw the crosspointers
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
		//srf[1]					= oapiCreateSurface (LOADBMP (IDB_INDICATORS1));
		srf[SRF_INDICATOR]			= oapiCreateSurface (LOADBMP (IDB_INDICATOR));
		srf[2]						= oapiCreateSurface (LOADBMP (IDB_NEEDLE1));
		srf[3]						= oapiCreateSurface (LOADBMP (IDB_HORIZON));
		srf[SRF_DIGITAL]			= oapiCreateSurface (LOADBMP (IDB_DIGITAL));
		srf[5]						= oapiCreateSurface (LOADBMP (IDB_FDAI));
		srf[6]						= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH1));
		srf[7]						= oapiCreateSurface (LOADBMP (IDB_SWLEVER));
		srf[8]						= oapiCreateSurface (LOADBMP (IDB_SECSWITCH));
		srf[9]						= oapiCreateSurface (LOADBMP (IDB_ABORT));
		srf[10]						= oapiCreateSurface (LOADBMP (IDB_ANNUN));
		srf[11]						= oapiCreateSurface (LOADBMP (IDB_LAUNCH));
		srf[12]						= oapiCreateSurface (LOADBMP (IDB_LV_ENG));
		srf[13]						= oapiCreateSurface (LOADBMP (IDB_LIGHTS2));
		srf[14]						= oapiCreateSurface (LOADBMP (IDB_ALTIMETER));
		srf[15]						= oapiCreateSurface (LOADBMP (IDB_ANLG_GMETER));
		srf[16]						= oapiCreateSurface (LOADBMP (IDB_THRUST));
		//srf[17]					= oapiCreateSurface (LOADBMP (IDB_HEADING));
		srf[18]						= oapiCreateSurface (LOADBMP (IDB_CONTACT));
		srf[SRF_LMTWOPOSLEVER]		= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH2));
		srf[20]						= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH3));
		srf[SRF_DSKY]				= oapiCreateSurface (LOADBMP (IDB_DSKY_LIGHTS));
		srf[SRF_LMABORTBUTTON]		= oapiCreateSurface (LOADBMP (IDB_LMABORTBUTTON));
		srf[SRF_LMMFDFRAME]			= oapiCreateSurface (LOADBMP (IDB_LMMFDFRAME));
		srf[SRF_LMTHREEPOSLEVER]	= oapiCreateSurface (LOADBMP (IDB_LMTHREEPOSLEVER));
		srf[SRF_LMTHREEPOSSWITCH]	= oapiCreateSurface (LOADBMP (IDB_LMTHREEPOSSWITCH));
		srf[SRF_DSKYDISP]			= oapiCreateSurface (LOADBMP (IDB_DSKY_DISP));		
		srf[SRF_FDAI]	        	= oapiCreateSurface (LOADBMP (IDB_FDAI));
		srf[SRF_FDAIROLL]       	= oapiCreateSurface (LOADBMP (IDB_FDAI_ROLL));
		srf[SRF_DSKYKEY]			= oapiCreateSurface (LOADBMP (IDB_DSKY_KEY));
		srf[SRF_SWITCHUP]			= oapiCreateSurface (LOADBMP (IDB_SWITCHUP));
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



		//
		// Set color keys where appropriate.
		//

		oapiSetSurfaceColourKey (srf[0], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[2], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[3], 0);
		oapiSetSurfaceColourKey (srf[5], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[14], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[15], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[16], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[18], g_Param.col[4]);
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

	MFDSPEC mfds_left  = {{  85, 1577,  395, 1886}, 6, 6, 55, 44};
	MFDSPEC mfds_right = {{1090, 1577, 1400, 1886}, 6, 6, 55, 44};

	switch (id) {
	case LMPANEL_MAIN: // LEM Main panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterMFD (MFD_LEFT,  mfds_left);
		oapiRegisterMFD (MFD_RIGHT, mfds_right);

		fdaiLeft.RegisterMe(AID_FDAI_LEFT, 135, 625);
		hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE (IDB_FDAI_ROLLINDICATOR));

		oapiRegisterPanelArea (AID_MFDLEFT,						    _R(  27, 1564,  452, 1918), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MFDRIGHT,					    _R(1032, 1564, 1457, 1918), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ABORT,							_R( 549,  870,  702,  942), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
        // 3 pos Engine Arm Lever
	    oapiRegisterPanelArea (AID_ENG_ARM,							_R( 166,  1078, 208, 1118), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		// 2 pos Descent Engine Command Override Lever
		oapiRegisterPanelArea (AID_DESCENT_ENGINE_SWITCH,			_R( 90,  1321,  132, 1361), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
        // 3 pos Mode control switches
	    oapiRegisterPanelArea (AID_MODECONTROL,						_R( 529,  1425, 777, 1470), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		// DSKY		
		oapiRegisterPanelArea (AID_DSKY_DISPLAY,					_R( 762, 1560,  867, 1736), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_LIGHTS,						_R( 618, 1565,  720, 1734), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_KEY,						_R( 598, 1755,  886, 1876), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_MISSION_CLOCK,					_R(  74,  287,  216,  309), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EVENT_TIMER,						_R( 291,  287,  372,  309), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUEL_DIGIT,						_R( 460,  245,  496,  319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_CONTACTLIGHT1,					_R( 324,  428,  369,  473), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CONTACTLIGHT2,					_R(1362, 1222, 1407, 1267), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTXPOINTERSWITCH,				_R( 330,  515,  364,  544), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GUIDCONTSWITCHROW,				_R( 661,  627,  696,  823), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMONITORSWITCHES,				_R(  51,  712,   85,  824), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FDAILOWERSWITCHROW,				_R( 138,  920,  314,  959), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ENGINETHRUSTCONTSWITCHES,		_R( 211, 1006,  320, 1117), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PROPELLANTSWITCHES,				_R( 484,  991,  546, 1112), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_HELIUMMONROTARY,					_R( 584,  992,  668, 1076), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTMONITORSWITCHES,			_R(1400,  712, 1434,  824), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPPRESSMONROTARY,				_R( 788, 1002,  872, 1086), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ACAPROPSWITCH,					_R(1012, 1012, 1046, 1051), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_SOV_TALKBACKS,				_R( 947,  871, 1040,  894), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_SOV_SWITCHES,			    _R( 941,  922, 1046,  961), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CLYCOLSUITFANROTARIES,			_R(1098,  926, 1182, 1130), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_QTYMONROTARY,					_R(1284,  986, 1368, 1070), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RADARANTTESTSWITCHES,			_R( 171, 1227,  206, 1361), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TESTMONITORROTARY,				_R( 116, 1410,  200, 1494), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SLEWRATESWITCH,				    _R( 272, 1364,  306, 1393), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RENDEZVOUSRADARROTARY,		    _R( 368, 1387,  452, 1471), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SATBCONTSWITCHES,				_R( 529, 1221,  754, 1260), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ATTITUDECONTROLSWITCHES,			_R( 529, 1322,  754, 1351), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPMONITORROTARY,				_R( 808, 1387,  892, 1471), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCSSYSQUADSWITCHES,				_R( 971, 1331, 1063, 1455), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LIGHTINGSWITCHES,				_R(1150, 1222, 1184, 1319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOODROTARY,						_R(1240, 1263, 1324, 1347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LAMPTONETESTROTARY,				_R(1172, 1405, 1256, 1489), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTXPOINTERSWITCH,				_R(1374, 1318, 1408, 1347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EXTERIORLTGSWITCH,				_R(1353, 1432, 1387, 1461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL4LEFTSWITCHROW,				_R( 486, 1614,  520, 1794), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL4RIGHTSWITCHROW,			_R( 964, 1614,  998, 1794), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_XPOINTERCDR,						_R(  65,  426,  202,  559), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	          PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_XPOINTERLMP,						_R(1302,  426, 1439,  559), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	          PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		
		//oapiRegisterPanelArea (AID_FUEL_DIGIT,					_R(1146,  135, 1183,  150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_FUEL_DIGIT,					_R(1146,  169, 1183,  184), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_FUEL_DIGIT2,					_R(1232,  163, 1285,  178), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DESCENT_HE,					_R(1090,  638, 1158,  690), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ENG_ARM,						_R( 972,  660,  995,  690), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_LGC_THRUSTER_QUADS,			_R(1360,  351, 1520,  490), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_RCS_SYSTEMA_SYSTEMB,			_R(1361,  255, 1518,  318), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_MAIN_SOV_SWITCHES,			_R(1435,  525, 1513,  585), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_LANDING_GEAR_SWITCH,			_R(  36, 1059,   61, 1126), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_EXPLOSIVE_DEVICES1,			_R( 102, 1027,  219, 1125), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SWITCH_SEP,					_R(  26, 1160,   51, 1205), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SWITCH_JET,					_R(  55, 1160,   80, 1205), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ENGINE_GIMBAL_SWITCH,		_R( 922,  750,  945,  780), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ASCENT_HE,					_R(1089,  555, 1158,  608), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_EXPLOSIVE_DEVICES2,			_R( 102, 1165,  219, 1195), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ATTITUDE_CONTROL_SWITCHES,	_R(1185,  813, 1322,  833), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ENG_FUEL_TEMP,				_R(1199,  271, 1235,  331), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ENG_FUEL_PRESS,				_R(1257,  256, 1293,  331), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_RCS_TEMP,					_R(1362,  157, 1398,  213), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_RCS_PRESS,					_R(1420,  139, 1456,  213), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_RCS_QTY,						_R(1478,  139, 1514,  213), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ENG_THRUST,					_R(1141,  256, 1177,  331), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_FDAI,						_R( 979,  401, 1075,  497), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
		//oapiRegisterPanelArea (AID_FDAI,						_R(1581,  401, 1677,  497), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
		//oapiRegisterPanelArea (AID_DSKY_KEY,					_R(1222, 1106, 1433, 1195), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DSKY_DISPLAY,				_R(1347,  962, 1452, 1138), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DSKY_LIGHTS,					_R(1203,  967, 1305, 1087), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CRSFD_SWITCH,				_R(1361,  525, 1384,  584), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CABIN_FAN_SWITCH,			_R(1616,  655, 1639,  685), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PTT_SWITCH,				    _R(2609, 1052, 2632, 1072), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_RCS_SYS_AB,					_R(1468,  820, 1537,  899), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_XPOINTER_SWITCH,				_R(1074,  311, 1098,  332), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PANEL1_1,					_R( 894,  433,  917,  511), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_GUID_CONT_SWITCH,			_R(1282,  367, 1306,  397), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ALT_RNG_MON,					_R(1282,  483, 1305,  503), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SHIFT_SWITCH,				_R( 951,  582,  974,  602), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ENGINE_THRUST_CONT,			_R( 997,  607, 1072,  690), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PROP_MON_SWITCHES,			_R(1167,  612, 1204,  687), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ACA_PROP_SWITCH,				_R(1490,  618, 1513,  648), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PANEL2_1,					_R(1739,  433, 1762,  511), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DESCENT_ENGINE_SWITCH,		_R( 916,  811,  939,  841), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SLEW_RATE_SWITCH,			_R(1038,  837, 1061,  857), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DEAD_BAND_SWITCH,			_R(1185,  754, 1208,  774), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_IMU_CAGE_SWITCH,				_R(1302,  875, 1325,  905), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SIDE_PANEL_SWITCH,			_R(1582,  752, 1605,  772), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_XPOINTER2_SWITCH,			_R(1723,  813, 1746,  833), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PANEL4_SWITCH1,				_R(1144,  996, 1167,  1026), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PANEL4_SWITCH2,				_R(1488,  996, 1511,  1026), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PANEL4_SWITCH3,				_R(1488, 1091, 1511,  1121), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PANEL4_SWITCH4,				_R(1144, 1092, 1167,  1122), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_AUDIO_CONT_SWITCH,			    _R(2413, 1060, 2436,  1080), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_RELAY_AUDIO_SWITCH,		    _R(2609,  998, 2632,  1028), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CABIN_PRESS_SWITCH,			_R(1616,  578, 1639,   608), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_HATCH_SWITCH,				_R( 263, 1044,  286,  1074), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_EVA_SWITCH,					_R( 263, 1103,  286,  1133), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ECS_GAUGES,					_R(1578,  139, 1723,   212), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_COAS,						_R( 334,  165,  639,   466), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ABORT,						_R(1210,  528, 1304,   572), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ABORT,						_R(555,  880, 695,   930), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		break;	
		
	case LMPANEL_RIGHTWINDOW: // LEM Right Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;

	case LMPANEL_LEFTWINDOW: // LEM Left Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		oapiRegisterPanelArea (AID_LEM_COAS2,				_R( 662, 0,  963, 301), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,					   PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;

	case LMPANEL_LPDWINDOW: // LDP Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		oapiRegisterPanelArea (AID_XPOINTER,		_R(838,  35, 973, 166), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE * RAD), cos(VIEWANGLE * RAD)));			
		break;

	case LMPANEL_RNDZWINDOW: // LEM Rendezvous Window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LEM_COAS1,				_R( 518, 0, 1053, 535), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,					   PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 1.0, 0.0));
		break;

	case LMPANEL_LEFTPANEL: // LEM Left Panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW1,					_R( 184,  85,  1433,  115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW2,					_R( 184,  258, 1433,  288), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW3,					_R( 184,  431, 1433,  461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW4,					_R( 184,  604, 1557,  634), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW5,					_R( 184,  777,  916,  807), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_LEM_PANEL_8,					    _R( 431,  916, 1574, 1258), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);		

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;

	case LMPANEL_RIGHTPANEL: // LEM Right Panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LM_EPS_DC_VOLTMETER,             _R( 110,  706, 209,  805), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EPS_DC_AMMETER,               _R( 110,  818, 209,  917), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EPS_LEFT_CONTROLS,            _R( 314,  728, 542,  913), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW2,					_R( 173,  258, 1415, 290), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW4,					_R( 173,  604, 1415, 634), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_BATTERY_TALKBACKS,	        _R( 573,  742, 888,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_HIGH_VOLTAGE_SWITCHES,	    _R( 568,  796, 895,  829), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_LOW_VOLTAGE_SWITCHES,	    _R( 568,  867, 824,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;

	case LMPANEL_AOTVIEW: // LEM Alignment Optical Telescope View
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
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
			break;

		case LMPANEL_RIGHTPANEL: // LEM Right Panel
			Panel16CB2SwitchRow.Init(AID_LEM_P16_CB_ROW2, MainPanel);
			EDS_CB_LOGIC_B.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
			
			Panel16CB4SwitchRow.Init(AID_LEM_P16_CB_ROW4, MainPanel);
			LMPInverter2CB.Init( 576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 30.0);
			// In reality, two of these are paralleled. I'll just use one.
			LMPBatteryFeedTieCB2.Init( 1211,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &ECA_1, 100.0);
			
			EPSP14VoltMeterSwitchRow.Init(AID_LM_EPS_DC_VOLTMETER,MainPanel);
			EPSDCVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], EPSP14VoltMeterSwitchRow, &EPSMonitorSelectRotary);
			EPSDCVoltMeter.SetSurface(srf[SRF_DCVOLTS], 99, 98);

			EPSP14AmMeterSwitchRow.Init(AID_LM_EPS_DC_AMMETER,MainPanel);
			EPSDCAmMeter.Init(g_Param.pen[4], g_Param.pen[4], EPSP14AmMeterSwitchRow, &EPSMonitorSelectRotary);
			EPSDCAmMeter.SetSurface(srf[SRF_DCAMPS], 99, 98);

			// 314, 728
			EPSLeftControlArea.Init(AID_LM_EPS_LEFT_CONTROLS,MainPanel);
			EPSInverterSwitch.Init( 142, 135, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39],EPSLeftControlArea, this, &INV_1, &INV_2);
			EPSEDVoltSelect.Init(319-314, 868-728, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EPSLeftControlArea);
			EPSMonitorSelectRotary.Init(117, 17, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], EPSLeftControlArea);

			DSCHiVoltageSwitchRow.Init(AID_DSC_HIGH_VOLTAGE_SWITCHES, MainPanel);
			DSCSEBat1HVSwitch.Init( 0,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_1,1);
			DSCSEBat2HVSwitch.Init( 69,  0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_1,3);
			DSCCDRBat3HVSwitch.Init(151,  0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_2,1);
			DSCCDRBat4HVSwitch.Init(220,  0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this,&ECA_2,3);

			DSCLoVoltageSwitchRow.Init(AID_DSC_LOW_VOLTAGE_SWITCHES, MainPanel);
			DSCSEBat1LVSwitch.Init( 0,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCLoVoltageSwitchRow, this,&ECA_1,2);
			DSCSEBat2LVSwitch.Init( 69,  0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29],DSCLoVoltageSwitchRow, this,&ECA_1,4);
			DSCCDRBat3LVSwitch.Init( 151,  0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29],DSCLoVoltageSwitchRow, this,&ECA_2,2);
			DSCCDRBat4LVSwitch.Init( 220,  0, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29],DSCLoVoltageSwitchRow, this,&ECA_2,4);

			DSCBatteryTBSwitchRow.Init(AID_DSC_BATTERY_TALKBACKS, MainPanel);
			DSCBattery1TB.Init(0,  0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);
			DSCBattery2TB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);
			DSCBattery3TB.Init(152,0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);
			DSCBattery4TB.Init(222,0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);

			break;

		case LMPANEL_LEFTPANEL:
			Panel11CB1SwitchRow.Init(AID_LEM_P11_CB_ROW1, MainPanel);
			AC_A_INV_1_FEED_CB.Init( 765, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_1, 5.0);
			AC_A_INV_2_FEED_CB.Init( 701, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_2, 5.0);
			AC_B_INV_1_FEED_CB.Init( 637, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_1, 5.0);
			AC_B_INV_2_FEED_CB.Init( 573, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_2, 5.0);

			Panel11CB2SwitchRow.Init(AID_LEM_P11_CB_ROW2, MainPanel);
			CDR_FDAI_DC_CB.Init( 893, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
			CDR_FDAI_AC_CB.Init( 1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &ACBusA, 2.0);

			Panel11CB3SwitchRow.Init(AID_LEM_P11_CB_ROW3, MainPanel); // 184,431 to 1433,459
			EDS_CB_LG_FLAG.Init(1021, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
			EDS_CB_LOGIC_A.Init(1085, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);

			Panel11CB4SwitchRow.Init(AID_LEM_P11_CB_ROW4, MainPanel);
			IMU_OPR_CB.Init( 1342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 20.0);
			LGC_DSKY_CB.Init( 1214,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);

			Panel11CB5SwitchRow.Init(AID_LEM_P11_CB_ROW5, MainPanel);
			// In reality, two of these are paralleled.
			// I'll just use one.
			CDRBatteryFeedTieCB2.Init( 66,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &ECA_2, 100.0);
			CDRInverter1CB.Init( 638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &ECA_2, 30.0);
			
			// Panel 8 is  431,916 to 1574,1258
			Panel8SwitchRow.Init(AID_LEM_PANEL_8, MainPanel);
			EDMasterArm.Init(861-431, 978-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDDesVent.Init(935-431, 978-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDASCHeSel.Init(1002-431, 978-916, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDDesPrpIsol.Init(643-431, 978-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDLGDeploy.Init(784-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDHePressRCS.Init(861-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDHePressDesStart.Init(935-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDHePressASC.Init(1002-431, 1078-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDStage.Init(783-431, 1175-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDStageRelay.Init(1002-431, 1182-916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
			EDDesFuelVent.Init(467-431, 1016-916, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			EDDesOxidVent.Init(540-431, 1016-916, 34, 29,srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
			EDLGTB.Init(790-431, 1033-916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow);
			EDDesFuelVentTB.Init(472-431, 960-916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow);
			EDDesOxidVentTB.Init(545-431, 960-916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow);
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
	case AID_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_DSKY]);
		return true;

	case AID_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP]);
		return true;

	case AID_DSKY_KEY:
		dsky.RenderKeys(surf, srf[SRF_DSKYKEY]);
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
			oapiBlt(surf, srf[SRF_LEM_COAS1], 0, 0, 0, 0, 535, 535, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_LEM_COAS1], 0, 0, 0, 535, 535, 535, SURF_PREDEF_CK);
		}
		return true;

	case AID_LEM_COAS2:
		if (LEMCoas2Enabled) {
			oapiBlt(surf, srf[SRF_LEM_COAS2], 0, 0, 0, 0, 300, 300, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_LEM_COAS2], 0, 0, 0, 300, 300, 300, SURF_PREDEF_CK);
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
	case AID_CONTACTLIGHT2:
		if (GroundContact()&& stage ==1){
			oapiBlt(surf,srf[18],0,0,0,0,45,45, SURF_PREDEF_CK);//
		}return true;

	case AID_SWITCH_SEP:
		if(Cswitch1){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(Sswitch1){
			oapiBlt(surf,srf[20],1,16,0,0,23,20);
			}else{
			oapiBlt(surf,srf[20],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
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
			// sprintf(oapiDebugString(),"LEM FDAI PAINT");
			fdaiLeft.PaintMe(attitude, no_att, euler_rates, errors, 0, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);			
		}
		return true;


	case AID_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_DIGITAL]);
		return true;

	case AID_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_DIGITAL]);
		return true;

	case AID_FUEL_DIGIT:
		double fuel;

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

		oapiBlt(surf, srf[SRF_DIGITAL], 0, 0, 16 * Curdigit, 0, 16, 19);
		oapiBlt(surf, srf[SRF_DIGITAL], 0, 54, 16 * Curdigit, 0, 16, 19);

		Curdigit2= (int) fuel;
		oapiBlt(surf, srf[SRF_DIGITAL], 18, 0, 16 * (Curdigit2 - (Curdigit*10)), 0, 16, 19);
		oapiBlt(surf, srf[SRF_DIGITAL], 18, 54, 16 * (Curdigit2 - (Curdigit*10)), 0, 16, 19);

		return true;

	case AID_DESCENT_HE:
		if(DESHE1switch){
			oapiBlt(surf,srf[6],1,30,0,0,23,20);
			oapiBlt(surf,srf[13],3,0,0,0,19,20);
		}
		else{
			oapiBlt(surf,srf[6],1,30,23,0,23,20);
			oapiBlt(surf,srf[13],3,0,38,0,19,20);
		}
		if(DESHE2switch){
			oapiBlt(surf,srf[6],44,30,0,0,23,20);
			oapiBlt(surf,srf[13],46,0,0,0,19,20);
		}
		else{
			oapiBlt(surf,srf[6],44,30,23,0,23,20);
			oapiBlt(surf,srf[13],46,0,38,0,19,20);
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
			oapiBlt(surf,srf[6],1,36,0,0,23,20);
			oapiBlt(surf,srf[13],3,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],1,36,23,0,23,20);
			oapiBlt(surf,srf[13],3,0,38,0,19,20);
		}
		if(QUAD2switch){
			oapiBlt(surf,srf[6],44,36,0,0,23,20);
			oapiBlt(surf,srf[13],46,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],44,36,23,0,23,20);
			oapiBlt(surf,srf[13],46,0,38,0,19,20);
		}
		if(QUAD3switch){
			oapiBlt(surf,srf[6],92,36,0,0,23,20);
			oapiBlt(surf,srf[13],94,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],92,36,23,0,23,20);
			oapiBlt(surf,srf[13],94,0,38,0,19,20);
		}
		if(QUAD4switch){
			oapiBlt(surf,srf[6],135,36,0,0,23,20);
			oapiBlt(surf,srf[13],137,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],135,36,23,0,23,20);
			oapiBlt(surf,srf[13],137,0,38,0,19,20);
		}
		if(QUAD5switch){
			oapiBlt(surf,srf[6],1,116,0,0,23,20);
			oapiBlt(surf,srf[13],3,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],1,116,23,0,23,20);
			oapiBlt(surf,srf[13],3,80,38,0,19,20);
		}
		if(QUAD6switch){
			oapiBlt(surf,srf[6],44,116,0,0,23,20);
			oapiBlt(surf,srf[13],46,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],44,116,23,0,23,20);
			oapiBlt(surf,srf[13],46,80,38,0,19,20);
		}
		if(QUAD7switch){
			oapiBlt(surf,srf[6],92,116,0,0,23,20);
			oapiBlt(surf,srf[13],94,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],92,116,23,0,23,20);
			oapiBlt(surf,srf[13],94,80,38,0,19,20);
		}
		if(QUAD8switch){
			oapiBlt(surf,srf[6],135,116,0,0,23,20);
			oapiBlt(surf,srf[13],137,80,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],135,116,23,0,23,20);
			oapiBlt(surf,srf[13],137,80,38,0,19,20);
		}
		return true;

	case AID_RCS_SYSTEMA_SYSTEMB:
		if(AFEED1switch){
			oapiBlt(surf,srf[19],0,33,0,0,23,30);
			oapiBlt(surf,srf[13],2,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],0,33,23,0,23,30);
			oapiBlt(surf,srf[13],2,0,38,0,19,20);
		}
		if(AFEED2switch){
			oapiBlt(surf,srf[19],43,33,0,0,23,30);
			oapiBlt(surf,srf[13],45,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],43,33,23,0,23,30);
			oapiBlt(surf,srf[13],45,0,38,0,19,20);
		}
		if(AFEED3switch){
			oapiBlt(surf,srf[19],91,33,0,0,23,30);
			oapiBlt(surf,srf[13],93,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],91,33,23,0,23,30);
			oapiBlt(surf,srf[13],93,0,38,0,19,20);
		}
		if(AFEED4switch){
			oapiBlt(surf,srf[19],134,33,0,0,23,30);
			oapiBlt(surf,srf[13],136,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],134,33,23,0,23,30);
			oapiBlt(surf,srf[13],136,0,38,0,19,20);
		}
		return true;

	/*
	case AID_MAIN_SOV_SWITCHES:
		if(MSOV1switch){
			oapiBlt(surf,srf[19],0,30,0,0,23,30);
			oapiBlt(surf,srf[13],2,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],0,30,23,0,23,30);
			oapiBlt(surf,srf[13],2,0,38,0,19,20);
		}
		if(MSOV2switch){
			oapiBlt(surf,srf[19],55,30,0,0,23,30);
			oapiBlt(surf,srf[13],57,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],55,30,23,0,23,30);
			oapiBlt(surf,srf[13],57,0,38,0,19,20);
		}
		return true;
	*/

	case AID_EXPLOSIVE_DEVICES1:
		if(ED1switch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		if(ED2switch){
			oapiBlt(surf,srf[19],47,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],47,0,23,0,23,30);
			}
		if(ED4switch){
			oapiBlt(surf,srf[19],00,68,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],00,68,23,0,23,30);
			}
		if(ED5switch){
			oapiBlt(surf,srf[19],47,68,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],47,68,23,0,23,30);
			}
		if(ED6switch){
			oapiBlt(surf,srf[19],94,68,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],94,68,23,0,23,30);
		}
		return true;

	case AID_ATTITUDE_CONTROL_SWITCHES:
		if(ATT1switch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		if(ATT2switch){
			oapiBlt(surf,srf[6],57,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],57,0,23,0,23,20);
			}
		if(ATT3switch){
			oapiBlt(surf,srf[6],114,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],114,0,23,0,23,20);
			}
		return true;

	case AID_ENGINE_GIMBAL_SWITCH:
		if(GMBLswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_ASCENT_HE:
		if(ASCHE1switch){
			oapiBlt(surf,srf[6],2,32,0,0,23,20);
			oapiBlt(surf,srf[13],4,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],2,32,23,0,23,20);
			oapiBlt(surf,srf[13],4,1,38,0,19,20);
		}
		if(ASCHE2switch){
			oapiBlt(surf,srf[6],45,32,0,0,23,20);
			oapiBlt(surf,srf[13],47,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],45,32,23,0,23,20);
			oapiBlt(surf,srf[13],47,1,38,0,19,20);
		}
		return true;

	case AID_EXPLOSIVE_DEVICES2:
			if(ED7switch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		if(ED8switch){
			oapiBlt(surf,srf[19],48,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],48,0,23,0,23,30);
			}
		if(ED9switch){
			oapiBlt(surf,srf[19],94,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],94,0,23,0,23,30);
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
		oapiBlt(surf,srf[2],1,(int)(47-(DispValue)*47),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_Asc){
		DispValue = GetPropellantMass(ph_Asc)/GetPropellantMaxMass(ph_Asc);
		}else{
			DispValue =1;
		}

		oapiBlt(surf,srf[2],29,(int)(47-(DispValue)*47),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_ENG_FUEL_PRESS:
		if(ph_Dsc){
			DispValue = GetPropellantMass(ph_Dsc)/8165;
		}else{
				DispValue =0;
		}
		oapiBlt(surf,srf[2],1,(int)(62-(DispValue)*62),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_Asc){
		DispValue = GetPropellantMass(ph_Asc)/GetPropellantMaxMass(ph_Asc);
		}else{
			DispValue =1;
		}

		oapiBlt(surf,srf[2],29,(int)(62-(DispValue)*62),8,0,7,7, SURF_PREDEF_CK);//
		return true;

		/* DEPRECIATED 
	case AID_RCS_TEMP:
		if(ph_rcslm0){
		DispValue = GetPropellantMass(ph_rcslm0)/100;
		//sprintf(oapiDebugString(),"rcs0lm = true");
		}else{
			DispValue =0;
		}
		oapiBlt(surf,srf[2],1,(int)(49-(DispValue)*49),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_rcslm1){
		DispValue = GetPropellantMass(ph_rcslm1)/100;
		}else{
			DispValue =1;
		}
		oapiBlt(surf,srf[2],29,(int)(49-(DispValue)*49),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_RCS_PRESS:
		if(ph_rcslm0){
		DispValue = GetPropellantMass(ph_rcslm0)/100;
		//sprintf(oapiDebugString(),"rcs0lm = true");
		}else{
			DispValue =0;
		}
		oapiBlt(surf,srf[2],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_rcslm1){
		DispValue = GetPropellantMass(ph_rcslm1)/100;
		}else{
			DispValue =1;
		}
		oapiBlt(surf,srf[2],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_RCS_QTY:
		if(ph_rcslm0){
		DispValue = GetPropellantMass(ph_rcslm0)/100;
		//sprintf(oapiDebugString(),"rcs0lm = true");
		}else{
			DispValue =0;
		}
		oapiBlt(surf,srf[2],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_rcslm1){
		DispValue = GetPropellantMass(ph_rcslm1)/100;
		}else{
			DispValue =1;
		}
		oapiBlt(surf,srf[2],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
		return true;
	*/

	case AID_LANDING_GEAR_SWITCH:
		if(LDGswitch){
			oapiBlt(surf,srf[19],1,37,0,0,23,30);
			oapiBlt(surf,srf[13],3,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],1,37,23,0,23,30);
			oapiBlt(surf,srf[13],3,0,38,0,19,20);
		}
		return true;

	case AID_CRSFD_SWITCH:
		if(CRSFDswitch){
			oapiBlt(surf,srf[19],0,29,0,0,23,30);
			oapiBlt(surf,srf[13],2,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[19],0,29,23,0,23,30);
			oapiBlt(surf,srf[13],2,0,38,0,19,20);
		}
		return true;

	case AID_CABIN_FAN_SWITCH:
		if(CABFswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PTT_SWITCH:
		if(PTTswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_RCS_SYS_AB:
		if(RCSS1switch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		if(RCSS2switch){
			oapiBlt(surf,srf[6],46,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],46,0,23,0,23,20);
		}
		if(RCSS3switch){
			oapiBlt(surf,srf[6],0,59,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,59,23,0,23,20);
		}
		if(RCSS4switch){
			oapiBlt(surf,srf[6],46,59,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],46,59,23,0,23,20);
		}
		return true;

	case AID_XPOINTER_SWITCH:
		if(X1switch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_PANEL1_1:
		if(RATE1switch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		if(AT1switch){
			oapiBlt(surf,srf[6],0,58,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,58,23,0,23,20);
		}
		return true;

	case AID_GUID_CONT_SWITCH:
		if(GUIDswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_ALT_RNG_MON:
		if(ALTswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_SHIFT_SWITCH:
		if(SHFTswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_ENGINE_THRUST_CONT:
		if(ETC1switch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		if(ETC2switch){
			oapiBlt(surf,srf[19],47,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],47,0,23,0,23,30);
		}
		if(ETC3switch){
			oapiBlt(surf,srf[6],14,58,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],14,58,23,0,23,20);
		}
		if(ETC4switch){
			oapiBlt(surf,srf[19],52,53,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],52,53,23,0,23,30);
		}
		return true;

	case AID_PROP_MON_SWITCHES:
		if(PMON1switch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		if(PMON2switch){
			oapiBlt(surf,srf[6],14,55,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],14,55,23,0,23,20);
		}
		return true;

	case AID_ACA_PROP_SWITCH:
		if(ACAPswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL2_1:
		if(RATE2switch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		if(AT2switch){
			oapiBlt(surf,srf[6],0,58,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,58,23,0,23,20);
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
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_DEAD_BAND_SWITCH:
		if(DBswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_IMU_CAGE_SWITCH:
		if(IMUCswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_SIDE_PANEL_SWITCH:
		if(SPLswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_XPOINTER2_SWITCH:
		if(X2switch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_PANEL4_SWITCH1:
		if(P41switch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL4_SWITCH2:
		if(P42switch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL4_SWITCH3:
		if(P43switch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_PANEL4_SWITCH4:
		if(P44switch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_AUDIO_CONT_SWITCH:
		if(AUDswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}
		else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_RELAY_AUDIO_SWITCH:
		if(RELswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_CABIN_PRESS_SWITCH:
		if(CPswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_HATCH_SWITCH:
		if(HATCHswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_EVA_SWITCH:
		if(EVAswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}
		else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

	case AID_ECS_GAUGES:
		if(CPswitch){
			oapiBlt(surf,srf[0],0,0,0,0,145,72);
		}
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
			oapiBlt(surf,srf[8],0,0,75,0,25,45);
			if(Sswitch2){
			oapiBlt(surf,srf[20],1,16,0,0,23,20);
			}else{
			oapiBlt(surf,srf[20],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,50,0,25,45);
			Sswitch2=false;
		}
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
	state.u.RPswitch17 = RPswitch17;

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
	RPswitch17 = state.u.RPswitch17;
}
