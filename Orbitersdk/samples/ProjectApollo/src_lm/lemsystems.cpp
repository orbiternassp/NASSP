/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant
  Copyright 2002-2005 Chris Knestrick


  ORBITER vessel module: LEM systems code

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

// To force Orbitersdk.h to use <fstream> in any compiler version
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
#include "lm_channels.h"
#include "dsky.h"
#include "PanelSDK/Internals/Esystems.h"

#include "LEM.h"

#include "papi.h"
#include "Mission.h"

void LEM::ResetThrusters()

{
	SetAttitudeRotLevel(0, 0);
	SetAttitudeRotLevel(1, 0);
	SetAttitudeRotLevel(2, 0);
	SetAttitudeLinLevel(0, 0);
	SetAttitudeLinLevel(1, 0);
	SetAttitudeLinLevel(2, 0);
	ActivateNavmode(NAVMODE_KILLROT);
}

void LEM::AddRCS_LMH(double TRANY)
{
	const double ATTCOOR = 66.1*0.0254;
	const double ATTCOORY = 254.0*0.0254;
	const double RCSOFFSET = 4.6*0.0254;
	const double QUADTOTHRUSTER1 = 5.3*0.0254;
	const double QUADTOTHRUSTER2 = 4.8*0.0254;
	const double QUADTOTHRUSTER3 = 0.25*0.0254;
	const double ATTWIDTH = .15;
	const double ATTHEIGHT = 3;
	const double LOFS = 0.12;

	double MaxThrust = 444.82;
	double RCSISP = 2840.0;

	// A1U
	th_rcs[0] = CreateThruster(_V(-ATTCOOR, ATTCOORY + TRANY + QUADTOTHRUSTER2, ATTCOOR), _V(0, -1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[0], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);

	// A1F
	th_rcs[1] = CreateThruster(_V(-ATTCOOR + RCSOFFSET, ATTCOORY + TRANY, ATTCOOR + QUADTOTHRUSTER3), _V(0, 0, -1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[1], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// B1L
	th_rcs[2] = CreateThruster(_V(-ATTCOOR - QUADTOTHRUSTER3, ATTCOORY + TRANY, ATTCOOR - RCSOFFSET), _V(1, 0, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[2], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// B1D
	th_rcs[3] = CreateThruster(_V(-ATTCOOR, ATTCOORY + TRANY - QUADTOTHRUSTER1, ATTCOOR), _V(0, 1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[3], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);

	// B2U
	th_rcs[4] = CreateThruster(_V(-ATTCOOR, ATTCOORY + TRANY + QUADTOTHRUSTER2, -ATTCOOR), _V(0, -1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[4], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// B2L
	th_rcs[5] = CreateThruster(_V(-ATTCOOR - QUADTOTHRUSTER3, ATTCOORY + TRANY, -ATTCOOR + RCSOFFSET), _V(1, 0, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[5], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// A2A
	th_rcs[6] = CreateThruster(_V(-ATTCOOR + RCSOFFSET, ATTCOORY + TRANY, -ATTCOOR - QUADTOTHRUSTER3), _V(0, 0, 1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[6], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// A2D
	th_rcs[7] = CreateThruster(_V(-ATTCOOR, ATTCOORY + TRANY - QUADTOTHRUSTER1, -ATTCOOR), _V(0, 1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[7], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);

	// A3U
	th_rcs[8] = CreateThruster(_V(ATTCOOR, ATTCOORY + TRANY + QUADTOTHRUSTER2, -ATTCOOR), _V(0, -1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[8], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// A3R
	th_rcs[9] = CreateThruster(_V(ATTCOOR + QUADTOTHRUSTER3, ATTCOORY + TRANY, -ATTCOOR + RCSOFFSET), _V(-1, 0, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[9], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// B3A
	th_rcs[10] = CreateThruster(_V(ATTCOOR - RCSOFFSET, ATTCOORY + TRANY, -ATTCOOR - QUADTOTHRUSTER3), _V(0, 0, 1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[10], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// B3D
	th_rcs[11] = CreateThruster(_V(ATTCOOR, ATTCOORY + TRANY - QUADTOTHRUSTER1, -ATTCOOR), _V(0, 1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[11], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);

	// B4U
	th_rcs[12] = CreateThruster(_V(ATTCOOR, ATTCOORY + TRANY + QUADTOTHRUSTER2, ATTCOOR), _V(0, -1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[12], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// B4F
	th_rcs[13] = CreateThruster(_V(ATTCOOR - RCSOFFSET, ATTCOORY + TRANY, ATTCOOR + QUADTOTHRUSTER3), _V(0, 0, -1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[13], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// A4R
	th_rcs[14] = CreateThruster(_V(ATTCOOR + QUADTOTHRUSTER3, ATTCOORY + TRANY, ATTCOOR - RCSOFFSET), _V(-1, 0, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[14], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);
	// A4D
	th_rcs[15] = CreateThruster(_V(ATTCOOR, ATTCOORY + TRANY - QUADTOTHRUSTER1, ATTCOOR), _V(0, 1, 0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[15], ATTHEIGHT, ATTWIDTH, LOFS, exhaustTex);

	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/

	//	CreateThrusterGroup (th_rcs,   1, THGROUP_ATT_YAWLEFT);
}

void LEM::SetLmDockingPort(double offs)
{
	VECTOR3 dockpos = { 0.0 ,offs, 0.0 };
	VECTOR3 dockdir = { 0,1,0 };

	VECTOR3 dockrot = { -0.8660254, 0, 0.5 };
	SetDockParams(dockpos, dockdir, dockrot);
	hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
}

void LEM::SystemsInit()

{
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo/LEMSystems");

	// DS20060407 Start wiring things together

	// Batteries
	Battery1 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_A");
	Battery2 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_B");
	Battery3 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_C");
	Battery4 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_D");
	Battery5 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_A");
	Battery6 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_B");
	//LunarBattery = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:LUNAR_BATTERY");  //Lunar battery not used on early LM's and will stay commented out until a J Mission LM is created
	EDBatteryA = (Battery *)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_A");
	EDBatteryB = (Battery *)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_B");

	// Batteries 1-4 and the Lunar Stay Battery are jettisoned with the descent stage.

	// ECA #1 (DESCENT stage, LMP 28V DC bus)
	ECA_1.Init(this, Battery1, Battery2, &DSCSEBat1HVSwitch, &DSCSEBat1LVSwitch, &DSCSEBat2HVSwitch, &DSCSEBat2LVSwitch, 2);

	// ECA #2 (DESCENT stage, CDR 28V DC bus)
	ECA_2.Init(this, Battery3, Battery4, &DSCCDRBat3HVSwitch, &DSCCDRBat3LVSwitch, &DSCCDRBat4HVSwitch, &DSCCDRBat4LVSwitch, 2);

	// ECA #1 and #2 are JETTISONED with the descent stage.
	// ECA #3 and #4 have no low voltage taps and can feed either bus.
	ECA_3.Init(this, &ASCBat5SESwitch, &ASCBat5CDRSwitch, Battery5);
	ECA_4.Init(this, &ASCBat6CDRSwitch, &ASCBat6SESwitch, Battery6);

	// Descent Stage Deadface Bus Stubs wire to the ECAs
	// stage is not defined here, so we can't do this.

	// Bus Tie Blocks (Not real objects)
	BTB_LMP_B.Init(this,&DES_LMPs28VBusA,&ECA_4.AFChannel);
	BTB_LMP_C.Init(this,&DES_LMPs28VBusB,&ECA_3.MFChannel);
	BTB_CDR_B.Init(this,&DES_CDRs28VBusA,&ECA_3.AFChannel);
	BTB_CDR_C.Init(this,&DES_CDRs28VBusB,&ECA_4.MFChannel);

	// Bus feed tie breakers are sourced from the descent busses AND from ECA 3/4
	// via ficticious Bus Tie Blocks
	LMPBatteryFeedTieCB1.MaxAmps = 100.0;
	LMPBatteryFeedTieCB1.WireTo(&BTB_LMP_B);
	LMPBatteryFeedTieCB2.MaxAmps = 100.0;
	LMPBatteryFeedTieCB2.WireTo(&BTB_LMP_C);
	CDRBatteryFeedTieCB1.MaxAmps = 100.0;
	CDRBatteryFeedTieCB1.WireTo(&BTB_CDR_B);
	CDRBatteryFeedTieCB2.MaxAmps = 100.0;
	CDRBatteryFeedTieCB2.WireTo(&BTB_CDR_C);

	// Set up XLunar system
	BTC_XLunar.Init(this);

	//Relay Junction Box and Deadface Relay Box
	rjb.Init(this, true);
	drb.Init(this, true);

	// Main busses can be fed from the ECAs via the BAT FEED TIE CBs,
	// the other bus via the CROSS TIE BUS / CROSS TIE BAL LOADS CBs,
	// or the CSM via the XLUNAR bus floating ground return.
	// At this point, we have the sum of all battery feeds on BTB D and the XLUNAR feeds on BTB A
	BTB_CDR_D.Init(this,&CDRBatteryFeedTieCB1,&CDRBatteryFeedTieCB2); // Tie dual CBs together for CDR bus
	BTB_LMP_D.Init(this,&LMPBatteryFeedTieCB1,&LMPBatteryFeedTieCB2); // Tie dual CBs together for LMP bus

	BTB_CDR_A.Init(this,&BTB_CDR_D,&BTC_XLunar.dc_output);		      // Tie batteries and XLUNAR feed for CDR bus
	BTB_LMP_A.Init(this,&BTB_LMP_D,NULL);							  // Not really necessary but keeps things even

	// Bus cross-tie breakers
	CDRCrossTieBalCB.MaxAmps = 30.0;
	CDRCrossTieBalCB.WireTo(&CDRs28VBus);
	CDRCrossTieBusCB.MaxAmps = 100.0;
	CDRCrossTieBusCB.WireTo(&CDRs28VBus);
	LMPCrossTieBalCB.MaxAmps = 30.0;
	LMPCrossTieBalCB.WireTo(&LMPs28VBus);
	LMPCrossTieBusCB.MaxAmps = 100.0;
	LMPCrossTieBusCB.WireTo(&LMPs28VBus);

	// Bus cross-tie setup: Wire the two busses to the multiplexer and tell it
	// where the CBs are. The multiplexer will do the rest.
	BTC_MPX.Init(this,&LMPs28VBus,&CDRs28VBus,&LMPCrossTieBalCB,&LMPCrossTieBusCB,&CDRCrossTieBalCB,&CDRCrossTieBusCB);
	
	// At this point, the sum of bus feeds are on BTB A, and the cross-tie sources are on the mpx.

	// Join cross-ties and main-ties for bus source
	BTB_LMP_E.Init(this,&BTB_LMP_A,&BTC_MPX.dc_output_lmp);
	BTB_CDR_E.Init(this,&BTB_CDR_A,&BTC_MPX.dc_output_cdr);

	// EPS Displays
	EPS_DISP_CB.MaxAmps = 2.0;
	EPS_DISP_CB.WireTo(&LMPs28VBus);
	EPSMonitorSelectRotary.WireTo(&EPS_DISP_CB);

	// Descent battery TBs
	DSCBattery1TB.WireTo(&EPS_DISP_CB);
	DSCBattery2TB.WireTo(&EPS_DISP_CB);
	DSCBattery3TB.WireTo(&EPS_DISP_CB);
	DSCBattery4TB.WireTo(&EPS_DISP_CB);
	DSCBattFeedTB.WireTo(&EPS_DISP_CB);
	// Ascent battery TBs
	ASCBattery5ATB.WireTo(&EPS_DISP_CB);
	ASCBattery5BTB.WireTo(&EPS_DISP_CB);
	ASCBattery6ATB.WireTo(&EPS_DISP_CB);
	ASCBattery6BTB.WireTo(&EPS_DISP_CB);

	// ECA stuff
	CDRAscECAContCB.MaxAmps = 3.0;
	CDRAscECAContCB.WireTo(&CDRs28VBus);
	CDRAscECAMainCB.MaxAmps = 2.0;
	CDRAscECAMainCB.WireTo(&CDRs28VBus);
	CDRDesECAContCB.MaxAmps = 5.0;
	CDRDesECAContCB.WireTo(&CDRs28VBus);
	CDRDesECAMainCB.MaxAmps = 3.0;
	CDRDesECAMainCB.WireTo(&CDRs28VBus);
	LMPAscECAContCB.MaxAmps = 3.0;
	LMPAscECAContCB.WireTo(&LMPs28VBus);
	LMPAscECAMainCB.MaxAmps = 2.0;
	LMPAscECAMainCB.WireTo(&LMPs28VBus);
	LMPDesECAContCB.MaxAmps = 5.0;
	LMPDesECAContCB.WireTo(&LMPs28VBus);
	LMPDesECAMainCB.MaxAmps = 3.0;
	LMPDesECAMainCB.WireTo(&LMPs28VBus);

	// REACTION CONTROL SYSTEM
	RCS_B_PQGS_DISP_CB.MaxAmps = 2.0;
	RCS_B_PQGS_DISP_CB.WireTo(&LMPs28VBus);
	LMRCSATempInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSBTempInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSAPressInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSBPressInd.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	LMRCSAQtyInd.WireTo(&RCS_B_PQGS_DISP_CB);
	LMRCSBQtyInd.WireTo(&RCS_B_PQGS_DISP_CB);
	
	// CDR and LMP 28V DC busses.
	// Wire to ficticious bus tie block
	CDRs28VBus.WireTo(&BTB_CDR_E); 
	LMPs28VBus.WireTo(&BTB_LMP_E);

	// DC voltmeter CBs
	CDRDCBusVoltCB.MaxAmps = 2.0;
	CDRDCBusVoltCB.WireTo(&CDRs28VBus);
	LMPDCBusVoltCB.MaxAmps = 2.0;
	LMPDCBusVoltCB.WireTo(&LMPs28VBus);

	// AC Inverter CBs
	CDRInverter1CB.MaxAmps = 30.0;
	CDRInverter1CB.WireTo(&CDRs28VBus);
	LMPInverter2CB.MaxAmps = 30.0;
	LMPInverter2CB.WireTo(&LMPs28VBus);
	// AC Inverters
	INV_1.dc_input = &CDRInverter1CB;	
	INV_2.dc_input = &LMPInverter2CB; 
	INV_1.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:INVHEAT"));
	INV_2.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:INVHEAT"));
	// AC bus voltmeter breaker
	AC_A_BUS_VOLT_CB.MaxAmps = 2.0;
	AC_A_BUS_VOLT_CB.WireTo(&ACBusA);
	// AC bus input breakers
	AC_A_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_1_FEED_CB.WireTo(&INV_1);
	AC_B_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_1_FEED_CB.WireTo(&INV_1);
	AC_A_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_2_FEED_CB.WireTo(&INV_2);
	AC_B_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_2_FEED_CB.WireTo(&INV_2);

	// AC busses
	ACBusA.Disconnect();
	ACBusB.Disconnect();
	// Situation load will wire these to their breakers later if needed

	// AC bus attenuator.
	ACVoltsAttenuator.WireTo(&AC_A_BUS_VOLT_CB);

	//Descent ECA power mergers
	DescentECAMainFeeder.WireToBuses(&CDRDesECAMainCB, &LMPDesECAMainCB);
	DescentECAContFeeder.WireToBuses(&CDRDesECAContCB, &LMPDesECAContCB);
	AscentECAMainFeeder.WireToBuses(&CDRAscECAMainCB, &LMPAscECAMainCB);
	AscentECAContFeeder.WireToBuses(&CDRAscECAContCB, &LMPAscECAContCB);

	// RCS valves
	RCSMainSovASwitch.WireTo(&RCS_A_MAIN_SOV_CB);
	RCSMainSovBSwitch.WireTo(&RCS_B_MAIN_SOV_CB);
	RCSMainSovATB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSMainSovBTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);

	RCSQuad1ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad2ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad3ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad4ACmdEnableSwitch.WireTo(&RCS_A_ISOL_VLV_CB);
	RCSQuad1BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad2BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad3BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad4BCmdEnableSwitch.WireTo(&RCS_B_ISOL_VLV_CB);
	RCSQuad1ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad2ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad3ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad4ACmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad1BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad2BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad3BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSQuad4BCmdEnableTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);

	RCSAscFeed1ASwitch.WireTo(&RCS_A_ASC_FEED_1_CB);
	RCSAscFeed2ASwitch.WireTo(&RCS_A_ASC_FEED_2_CB);
	RCSAscFeed1BSwitch.WireTo(&RCS_B_ASC_FEED_1_CB);
	RCSAscFeed2BSwitch.WireTo(&RCS_B_ASC_FEED_2_CB);
	RCSAscFeed1ATB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSAscFeed2ATB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSAscFeed1BTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSAscFeed2BTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);
	RCSXFeedSwitch.WireTo(&RCS_B_CRSFD_CB);
	RCSXFeedTB.WireTo(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB);

	// Lighting
	tle.Init(this, &LTG_TRACK_CB, &ExteriorLTGSwitch, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:TLEHEAT"));
	DockLights.Init(this, &ExteriorLTGSwitch);
	lca.Init(this, &CDR_LTG_ANUN_DOCK_COMPNT_CB, &LTG_ANUN_DOCK_COMPNT_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:LCAHEAT"));
	UtilLights.Init(this, &CDR_LTG_UTIL_CB, &UtilityLightSwitchCDR, &UtilityLightSwitchLMP, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:CABINHEAT"));
	COASLights.Init(this, &COAS_DC_CB, &CDRCOASSwitch, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:CABINHEAT"));
	FloodLights.Init(this, &LTG_FLOOD_CB, &FloodSwitch, &FloodRotary, &LtgFloodOhdFwdKnob, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:CABINHEAT"));
	AOTLampFeeder.WireToBuses(&AOT_LAMP_ACA_CB, &AOT_LAMP_ACB_CB);
	pfira.Init(this);

	// LGC and DSKY
	agc.WirePower(&LGC_DSKY_CB, NULL);
	// The DSKY brightness IS controlled by the ANUN/NUM knob on panel 5, but by means of an isolated section of it.
	// The source of the isolated section is coming from the LGC supply.
	NumDockCompLTGFeeder.WireToBuses(&CDR_LTG_ANUN_DOCK_COMPNT_CB, &LTG_ANUN_DOCK_COMPNT_CB); //This should be handled in the LCA, powermerger for temporary functionality
	dsky.Init(&NumDockCompLTGFeeder, &LGC_DSKY_CB, &LtgAnunNumKnob);
	agc.InitHeat((h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:LGCHEAT"));

	// AGS stuff
	asa.Init(this, &AGSOperateSwitch, (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-ASA-FastHeater"),
									  (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-ASA-FineHeater"),
									  (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-ASA-HSink"),
									  (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:ASAHEAT"));

	aea.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:AEAHEAT"));
	aea.WireToBuses(&CDR_SCS_AEA_CB, &SCS_AEA_CB, &AGSOperateSwitch);
	deda.Init(&SCS_AEA_CB);
	rga.Init(this, &SCS_ATCA_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:RGAHEAT"));

	// Set up IMU heater stuff
	imucase = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LM-IMU-Case");
	imucase->isolation = 0.0000001;
	imucase->Area = 3165.31625; // Surface area of 12.5 inch diameter sphere in cm
	imuheater = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LM-IMU-Heater");
	imublower = (h_HeatExchanger *)Panelsdk.GetPointerByString("HYDRAULIC:IMUBLOWER");

	//IMU
	imu.WireToBuses(&IMU_OPR_CB, NULL, NULL);
	imu.WireHeaterToBuses(imuheater, &IMU_SBY_CB, NULL);
	imu.InitThermals((h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:IMUHEAT"), imucase, (h_HeatLoad*)Panelsdk.GetPointerByString("HYDRAULIC:PTAHEAT"), (h_HeatLoad*)Panelsdk.GetPointerByString("HYDRAULIC:PSAHEAT"), (h_HeatLoad*)Panelsdk.GetPointerByString("HYDRAULIC:CDUHEAT"));

	// Main Propulsion
	PROP_DISP_ENG_OVRD_LOGIC_CB.MaxAmps = 2.0;
	PROP_DISP_ENG_OVRD_LOGIC_CB.WireTo(&LMPs28VBus);
	THRUST_DISP_CB.MaxAmps = 2.0;
	THRUST_DISP_CB.WireTo(&CDRs28VBus);
	MainFuelTempInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainFuelPressInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainOxidizerTempInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainOxidizerPressInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	EngineThrustInd.WireTo(&THRUST_DISP_CB);
	CommandedThrustInd.WireTo(&THRUST_DISP_CB);
	DPSOxidPercentMeter.WireTo(&HE_PQGS_PROP_DISP_AC_CB);
	DPSFuelPercentMeter.WireTo(&HE_PQGS_PROP_DISP_AC_CB);
	MainHeliumPressureMeter.WireTo(&HE_PQGS_PROP_DISP_AC_CB);

	// The FDAI has two CBs, AC and DC, and both are 2 amp CBs
	// CDR FDAI
	CDR_FDAI_DC_CB.MaxAmps = 2.0;
	CDR_FDAI_DC_CB.WireTo(&CDRs28VBus);
	CDR_FDAI_AC_CB.MaxAmps = 2.0;
	CDR_FDAI_AC_CB.WireTo(&ACBusA);
	// And the CDR FDAI itself	
	fdaiLeft.WireTo(&CDR_FDAI_DC_CB,&CDR_FDAI_AC_CB);
	// LMP FDAI stuff
	LMP_EVT_TMR_FDAI_DC_CB.MaxAmps = 2.0;
	LMP_EVT_TMR_FDAI_DC_CB.WireTo(&LMPs28VBus);
	LMP_FDAI_AC_CB.MaxAmps = 2.0;
	LMP_FDAI_AC_CB.WireTo(&ACBusB);
	fdaiRight.WireTo(&LMP_EVT_TMR_FDAI_DC_CB,&LMP_FDAI_AC_CB);
	EventTimerDisplay.Init(&LMP_EVT_TMR_FDAI_DC_CB, NULL, &LtgAnunNumKnob, &NUM_LTG_AC_CB, &LtgORideNumSwitch);

	// HEATERS
	TempMonitorInd.WireTo(&HTR_DISP_CB);

	// Landing Radar
	LR.Init(this, &PGNS_LDG_RDR_CB, (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-LR-Antenna"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-LR-Antenna-Heater"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:LRHEAT"));

	// Rdz Radar
	RR.Init(this, &PGNS_RNDZ_RDR_CB, &RDZ_RDR_AC_CB, (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-RR-Antenna"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-Heater"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-StbyHeater"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:RREHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:RRHEAT"));
	crossPointerLeft.Init(this, &CDR_XPTR_CB, &LeftXPointerSwitch, &RateErrorMonSwitch);
	crossPointerRight.Init(this, &SE_XPTR_DC_CB, &RightXPointerSwitch, &RightRateErrorMonSwitch);

	// CWEA
	CWEA.Init(this, &INST_CWEA_CB, &LTG_MASTER_ALARM_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:CWEAHEAT"));

	// COMM
	omni_fwd.Init(this);
	omni_aft.Init(this);
	// S-Band Steerable Ant
	SBandSteerable.Init(this, (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LEM-SBand-Steerable-Antenna"), (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:LEM-SBand-Steerable-Antenna-Heater"), (h_HeatLoad*)Panelsdk.GetPointerByString("HYDRAULIC:SBDANTHEAT"));
	// SBand System
	SBand.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SBXHEAT"), (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SBPHEAT"));
	// VHF System
	VHF.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:VHFHEAT"));
	// PCM
	PCM.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:PCMHEAT"));
	// DSEA
	DSEA.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:DSEHEAT"));
	TapeRecorderTB.WireTo(&INST_PCMTEA_CB); //Tape Recorder TB powered by PCM/TE cb

	// CBs
	INST_SIG_CONDR_1_CB.MaxAmps = 2.0;
	INST_SIG_CONDR_1_CB.WireTo(&CDRs28VBus);
	INST_SIG_CONDR_2_CB.MaxAmps = 2.0;
	INST_SIG_CONDR_2_CB.WireTo(&LMPs28VBus);
	COMM_VHF_RCVR_A_CB.MaxAmps = 2.0;
	COMM_VHF_RCVR_A_CB.WireTo(&CDRs28VBus);
	COMM_VHF_XMTR_A_CB.MaxAmps = 5.0;
	COMM_VHF_XMTR_A_CB.WireTo(&LMPs28VBus);
	COMM_VHF_RCVR_B_CB.MaxAmps = 2.0;
	COMM_VHF_RCVR_B_CB.WireTo(&LMPs28VBus);
	COMM_VHF_XMTR_B_CB.MaxAmps = 5.0;
	COMM_VHF_XMTR_B_CB.WireTo(&CDRs28VBus);
	COMM_PRIM_SBAND_PA_CB.MaxAmps = 7.5;
	COMM_PRIM_SBAND_PA_CB.WireTo(&LMPs28VBus);
	COMM_PRIM_SBAND_XCVR_CB.MaxAmps = 5.0;
	COMM_PRIM_SBAND_XCVR_CB.WireTo(&LMPs28VBus);
	COMM_SEC_SBAND_PA_CB.MaxAmps = 7.5;
	COMM_SEC_SBAND_PA_CB.WireTo(&CDRs28VBus);
	COMM_SEC_SBAND_XCVR_CB.MaxAmps = 5.0;
	COMM_SEC_SBAND_XCVR_CB.WireTo(&CDRs28VBus);
	COMM_CDR_AUDIO_CB.MaxAmps = 2.0;
	COMM_CDR_AUDIO_CB.WireTo(&CDRs28VBus);
	COMM_SE_AUDIO_CB.MaxAmps = 2.0;
	COMM_SE_AUDIO_CB.WireTo(&LMPs28VBus);
	INST_SIG_SENSOR_CB.MaxAmps = 2.0;
	INST_SIG_SENSOR_CB.WireTo(&LMPs28VBus);
	INST_PCMTEA_CB.MaxAmps = 2.0;
	INST_PCMTEA_CB.WireTo(&LMPs28VBus);
	COMM_PMP_CB.MaxAmps = 2.0;
	COMM_PMP_CB.WireTo(&LMPs28VBus);
	COMM_SBAND_ANT_CB.MaxAmps = 2.0;
	COMM_SBAND_ANT_CB.WireTo(&LMPs28VBus);
	COMM_DISP_CB.MaxAmps = 2.0;
	COMM_DISP_CB.WireTo(&LMPs28VBus);

	// EXPLOSIVE DEVICES SUPPLY CBs
	EDS_CB_LG_FLAG.MaxAmps = 2.0;
	EDS_CB_LG_FLAG.WireTo(&CDRs28VBus);
	EDS_CB_LOGIC_A.MaxAmps = 2.0;
	EDS_CB_LOGIC_A.WireTo(&CDRs28VBus);
	EDS_CB_LOGIC_B.MaxAmps = 2.0;
	EDS_CB_LOGIC_B.WireTo(&LMPs28VBus);

	// EXPLOSIVE DEVICES SYSTEMS
	EDLGTB.WireTo(&EDS_CB_LG_FLAG);

	// ABORT GUIDANCE SYSTEM
	SCS_ASA_CB.MaxAmps = 20.0;
	SCS_ASA_CB.WireTo(&LMPs28VBus);
	SCS_AEA_CB.MaxAmps = 10.0;
	SCS_AEA_CB.WireTo(&LMPs28VBus);
	CDR_SCS_AEA_CB.MaxAmps = 10.0;
	CDR_SCS_AEA_CB.WireTo(&CDRs28VBus);
	SCS_ATCA_AGS_CB.MaxAmps = 3.0;
	SCS_ATCA_AGS_CB.WireTo(&LMPs28VBus);

	// ENVIRONMENTAL CONTROL SYSTEM
	LMSuitTempMeter.WireTo(&ECS_DISP_CB);
	LMCabinTempMeter.WireTo(&ECS_DISP_CB);
	LMSuitPressMeter.WireTo(&ECS_DISP_CB);
	LMCabinPressMeter.WireTo(&ECS_DISP_CB);
	LMCO2Meter.WireTo(&ECS_DISP_CB);
	LMGlycolTempMeter.WireTo(&ECS_DISP_CB);
	LMGlycolPressMeter.WireTo(&ECS_DISP_CB);
	LMOxygenQtyMeter.WireTo(&ECS_DISP_CB);
	LMWaterQtyMeter.WireTo(&ECS_DISP_CB);

	CrewInCabin = (h_crew *)Panelsdk.GetPointerByString("HYDRAULIC:CREW");
	CDRSuited = (h_crew *)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUITED");
	LMPSuited = (h_crew *)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUITED");

	//Initialize LM ECS
	DesO2Tank = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK");
	AscO2Tank1 = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK1");
	AscO2Tank2 = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK2");
	DesO2Manifold = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD");
	O2Manifold = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD");
	PressRegA = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA");
	PressRegB = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB");
	DesH2OTank = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESH2OTANK");
	DesBatCooling = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING");
	CabinFan1 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:CABINFAN");
	CabinHeat = (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:CABINHEAT");
	SuitFan1 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:SUITFAN1");
	SuitFan2 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:SUITFAN2");
	SuitFan1Heat = (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SUITFAN1HEAT");
	SuitFan2Heat = (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SUITFAN2HEAT");
	PrimGlyPump1 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP1");
	PrimGlyPump2 = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP2");
	SecGlyPump = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:SECGLYCOLPUMP");
	SecGlyPumpHeat = (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:GLYPUMPSECHEAT");
	LCGPump = (Pump *)Panelsdk.GetPointerByString("ELECTRIC:LCGPUMP");

	PrimGlyPump1->WireTo(&ECS_GLYCOL_PUMP_1_CB);
	PrimGlyPump2->WireTo(&ECS_GLYCOL_PUMP_2_CB);
	CabinFan1->WireTo(&ECS_CABIN_FAN_1_CB);
	SuitFan1->WireTo(&ECS_SUIT_FAN_1_CB);
	SuitFan2->WireTo(&ECS_SUIT_FAN_2_CB);
	SecGlyPump->WireTo(&ECS_GLYCOL_PUMP_SEC_CB);
	LCGPump->WireTo(&ECS_LGC_PUMP_CB);

	//Initialize LM ECS Tanks
	DesO2Tank->BoilAllAndSetTemp(294.261);
	AscO2Tank1->BoilAllAndSetTemp(294.261);
	AscO2Tank2->BoilAllAndSetTemp(294.261);
	DesO2Manifold->BoilAllAndSetTemp(283.15);
	O2Manifold->BoilAllAndSetTemp(283.15);
	PressRegA->BoilAllAndSetTemp(277.594);
	PressRegB->BoilAllAndSetTemp(277.594);

	//Tunnel Connection Initialization   
	SetPipeMaxFlow("HYDRAULIC:LMTUNNELUNDOCKED", 1000.0 / LBH);

	//Oxygen Pipe Initialization   
	SetPipeMaxFlow("HYDRAULIC:DESO2PIPE1", 660.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:DESO2PIPE2", 660.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRESSREGAOUT", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRESSREGBOUT", 6.75 / LBH);
	SetPipeMaxFlow("HYDRAULIC:SUITCIRCUITRELIEFVALVE", 7.8 / LBH);

	//Cabin Fan Initialization  
	SetPipeMaxFlow("HYDRAULIC:CABINFANPIPE", 300.0 / LBH);

	//Primary Glycol Pipe Initialization   
	//SetPipeMaxFlow("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT1", 120.0 / LBH);
	//SetPipeMaxFlow("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT2", 85.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:PRIMGLYCOLCOOLINGOUT", 120.0 / LBH);
	//SetPipeMaxFlow("HYDRAULIC:WATERGLYCOLHXOUT", 85.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:HXFLOWCONTROL", 120.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:HXFLOWCONTROLBYPASS", 290.0 / LBH);
	//SetPipeMaxFlow("HYDRAULIC:HXHOUTFLOW", 120.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:ASCBATINLET", 116.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:DESBATINLET", 174.0 / LBH); 

	//Secondary Glycol Pipe Initialization  
	SetPipeMaxFlow("HYDRAULIC:SECGLYFLOWREG2", 300.0 / LBH);

	//LCG Pipe Initialization   
	SetPipeMaxFlow("HYDRAULIC:LCGACCUMULATOROUT", 240.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:LCGFLOWCONTROLBYPASS", 240.0 / LBH);
	SetPipeMaxFlow("HYDRAULIC:LCGFLOWCONTROL", 240.0 / LBH);

	// Mission timer.
	MISSION_TIMER_CB.MaxAmps = 2.0;
	MISSION_TIMER_CB.WireTo(&CDRs28VBus);
	MissionTimerDisplay.Init(&MISSION_TIMER_CB, NULL, &LtgAnunNumKnob, &NUM_LTG_AC_CB, &LtgORideNumSwitch);

	// Pyro Buses
	Panelsdk.AddElectrical(&ED28VBusA, false);
	Panelsdk.AddElectrical(&ED28VBusB, false);

	// Pyros
	LandingGearPyros.WireTo(&LandingGearPyrosFeeder);
	CableCuttingPyros.WireTo(&CableCuttingPyrosFeeder);
	DescentPropVentPyros.WireTo(&DescentPropVentPyrosFeeder);
	DescentEngineStartPyros.WireTo(&DescentEngineStartPyrosFeeder);
	DescentEngineOnPyros.WireTo(&DescentEngineOnPyrosFeeder);
	DescentPropIsolPyros.WireTo(&DescentPropIsolPyrosFeeder);
	AscentHeliumIsol1Pyros.WireTo(&AscentHeliumIsol1PyrosFeeder);
	AscentHeliumIsol2Pyros.WireTo(&AscentHeliumIsol2PyrosFeeder);
	AscentOxidCompValvePyros.WireTo(&AscentOxidCompValvePyrosFeeder);
	AscentFuelCompValvePyros.WireTo(&AscentFuelCompValvePyrosFeeder);
	RCSHeliumSupplyAPyros.WireTo(&RCSHeliumSupplyAPyrosFeeder);
	RCSHeliumSupplyBPyros.WireTo(&RCSHeliumSupplyBPyrosFeeder);

	// Update ECA ties
	Panelsdk.AddElectrical(&DES_CDRs28VBusA, false);
	Panelsdk.AddElectrical(&DES_CDRs28VBusB, false);
	Panelsdk.AddElectrical(&DES_LMPs28VBusA, false); 
	Panelsdk.AddElectrical(&DES_LMPs28VBusB, false); 

	// Arrange for updates of ECAs
	Panelsdk.AddElectrical(&ECA_1.ECAChannelA, false);
	Panelsdk.AddElectrical(&ECA_1.ECAChannelB, false);
	Panelsdk.AddElectrical(&ECA_2.ECAChannelA, false);
	Panelsdk.AddElectrical(&ECA_2.ECAChannelB, false);
	Panelsdk.AddElectrical(&ECA_3.MFChannel, false);
	Panelsdk.AddElectrical(&ECA_3.AFChannel, false);
	Panelsdk.AddElectrical(&ECA_4.MFChannel, false);
	Panelsdk.AddElectrical(&ECA_4.AFChannel, false);

	// Arrange for updates of tie points and bus balancer

	// Sum of ascent and descent feed lines
	Panelsdk.AddElectrical(&BTB_CDR_B, false);
	Panelsdk.AddElectrical(&BTB_CDR_C, false);
	Panelsdk.AddElectrical(&BTB_LMP_B, false);
	Panelsdk.AddElectrical(&BTB_LMP_C, false);

	// XLUNAR source
	Panelsdk.AddElectrical(&BTC_XLunar, false);
	// Sum of battery feed ties
	Panelsdk.AddElectrical(&BTB_CDR_D, false);
	Panelsdk.AddElectrical(&BTB_LMP_D, false);

	// Sum of BTB-D and XLUNAR power
	Panelsdk.AddElectrical(&BTB_CDR_A, false);
	Panelsdk.AddElectrical(&BTB_LMP_A, false);
	// The multiplexer will update the main 28V busses
	Panelsdk.AddElectrical(&BTC_MPX, false);

	// Sum of BTB-A and bus cross-tie-balancer
	Panelsdk.AddElectrical(&BTB_CDR_E, false);
	Panelsdk.AddElectrical(&BTB_LMP_E, false);

	// Arrange for updates of main busses, AC inverters, and the bus balancer
	Panelsdk.AddElectrical(&ACBusA, false);
	Panelsdk.AddElectrical(&ACBusB, false);
	Panelsdk.AddElectrical(&ACVoltsAttenuator, false);
	Panelsdk.AddElectrical(&INV_1, false);
	Panelsdk.AddElectrical(&INV_2, false);

	//Lighting Control Assembly
	Panelsdk.AddElectrical(&lca, false);

	// ECS
	CabinPressureSwitch.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"), 4.70/PSI, 4.07/PSI);
	SuitPressureSwitch.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT"), 3.50/PSI, 2.90/PSI);
	CabinRepressValve.Init(this, (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESS"),
		&ECS_CABIN_REPRESS_CB, &CabinRepressValveSwitch, &PressRegAValve, &PressRegBValve);
	CDRIsolValve.Init(this, &CDRSuitIsolValve, &CDRActuatorOvrd);
	LMPIsolValve.Init(this, &LMPSuitIsolValve, &LMPActuatorOvrd);
	SuitCircuitPressureRegulatorA.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGAOUT"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT"), &PressRegAValve);
	SuitCircuitPressureRegulatorB.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGBOUT"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT"), &PressRegBValve);
	OverheadHatch.Init(this, &UpperHatchHandle, &UpperHatchReliefValve, (h_Pipe*)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE"));
	OVHDCabinReliefDumpValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE"),
		&UpperHatchReliefValve, &OverheadHatch);
	ForwardHatch.Init(this, &ForwardHatchHandle, &ForwardHatchReliefValve, (h_Tank*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"));
	FWDCabinReliefDumpValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINFWDHATCHVALVE"),
		&ForwardHatchReliefValve, &ForwardHatch);
	SuitCircuitReliefValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRELIEFVALVE"),
		&SuitCircuitReliefValveSwitch);
	SuitGasDiverter.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"),
		&SuitGasDiverterSwitch, &ECS_DIVERT_VLV_CB, &PressRegAValve, &PressRegBValve);
	CabinGasReturnValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINGASRETURN"),
		&CabinGasReturnValveSwitch);
	CO2CanisterSelect.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER"),
		&CO2CanisterSelectSwitch);
	PrimCO2CanisterVent.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER"),
		&CO2CanisterPrimVent);
	SecCO2CanisterVent.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER"),
		&CO2CanisterSecVent);
	WaterSeparationSelector.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLDIN"),
		&WaterSepSelectSwitch);
	CabinFan.Init(&ECS_CABIN_FAN_1_CB, &ECS_CABIN_FAN_CONT_CB, &PressRegAValve, &PressRegBValve, (Pump *)Panelsdk.GetPointerByString("ELECTRIC:CABINFAN"),
		(h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:CABINFANHEAT"));
	WaterTankSelect.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK"),
		&WaterTankSelectValve);
	PrimGlycolPumpController.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD"),
		(Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP1"),
		(Pump *)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP2"),
		&GlycolRotary, &ECS_GLYCOL_PUMP_1_CB, &ECS_GLYCOL_PUMP_2_CB, &ECS_GLYCOL_PUMP_AUTO_XFER_CB,
		(h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:GLYPUMP1HEAT"),
		(h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:GLYPUMP2HEAT"));
	SuitFanDPSensor.Init((h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDIN"),
		(h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDOUT"),
		&ECS_SUIT_FAN_DP_CB);
	CrewStatus.Init(this);
	ecs.Init(this);

	// EDS initialization
	eds.Init(this);
	EDDesFuelVent.WireTo(&PROP_DES_HE_REG_VENT_CB);
	EDDesOxidVent.WireTo(&PROP_DES_HE_REG_VENT_CB);
	EDDesFuelVentTB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	EDDesOxidVentTB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);

	// S&C Control Assemblies
	scca1.Init(this);
	scca2.Init(this);
	scca3.Init(this);

	// DPS and APS
	DPSPropellant.Init(this, &PROP_PQGS_CB);
	DPS.Init(this);
	DPS.pitchGimbalActuator.Init(this, &EngGimbalEnableSwitch, &DECA_GMBL_AC_CB);
	DPS.rollGimbalActuator.Init(this, &EngGimbalEnableSwitch, &DECA_GMBL_AC_CB);
	DESHeReg1Switch.WireTo(&PROP_DES_HE_REG_VENT_CB);
	DESHeReg2Switch.WireTo(&PROP_DES_HE_REG_VENT_CB);
	DESHeReg1TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	DESHeReg2TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	APS.Init(this);
	ASCHeReg1Switch.WireTo(&PROP_ASC_HE_REG_CB);
	ASCHeReg2Switch.WireTo(&PROP_ASC_HE_REG_CB);
	ASCHeReg1TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	ASCHeReg2TB.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);

	//RCS
	RCSA.Init(th_rcs, &RCSHeliumSupplyAPyros, &RCSB, 0, 6, 8, 14);
	RCSB.Init(th_rcs, &RCSHeliumSupplyBPyros, &RCSA, 2, 4, 10, 12);
	tca1A.Init(this, 7);
	tca2A.Init(this, 5);
	tca3A.Init(this, 3);
	tca4A.Init(this, 1);
	tca1B.Init(this, 8);
	tca2B.Init(this, 6);
	tca3B.Init(this, 4);
	tca4B.Init(this, 2);

	RCSHtr1Quad1 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD1HTRSYS1");
	RCSHtr1Quad2 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD2HTRSYS1");
	RCSHtr1Quad3 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD3HTRSYS1");
	RCSHtr1Quad4 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD4HTRSYS1");
	RCSHtr1Quad1->WireTo(&RCS_QUAD_1_CDR_HTR_CB);
	RCSHtr1Quad2->WireTo(&RCS_QUAD_2_CDR_HTR_CB);
	RCSHtr1Quad3->WireTo(&RCS_QUAD_3_CDR_HTR_CB);
	RCSHtr1Quad4->WireTo(&RCS_QUAD_4_CDR_HTR_CB);

	RCSHtr2Quad1 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD1HTRSYS2");
	RCSHtr2Quad2 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD2HTRSYS2");
	RCSHtr2Quad3 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD3HTRSYS2");
	RCSHtr2Quad4 = (Boiler *)Panelsdk.GetPointerByString("ELECTRIC:QUAD4HTRSYS2");
	RCSHtr2Quad1->WireTo(&RCS_QUAD_1_LMP_HTR_CB);
	RCSHtr2Quad2->WireTo(&RCS_QUAD_2_LMP_HTR_CB);
	RCSHtr2Quad3->WireTo(&RCS_QUAD_3_LMP_HTR_CB);
	RCSHtr2Quad4->WireTo(&RCS_QUAD_4_LMP_HTR_CB);

	LMQuad1RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD1");
	LMQuad2RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD2");
	LMQuad3RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD3");
	LMQuad4RCS = (h_Radiator *)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD4");

	//ACA and TTCA
	CDR_ACA.Init(this, &ACAPropSwitch);
	CDR_TTCA.Init(this);

	//DECA
	deca.Init(this, &SCS_DECA_PWR_CB);

	//GASTA
	gasta.Init(this, &GASTA_DC_CB, &GASTA_AC_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:GASTAHEAT"), &imu);

	//ORDEAL
	ordeal.Init(&ORDEALEarthSwitch, &ORDEAL_AC_CB, &ORDEAL_DC_CB, &ORDEALAltSetRotary, &ORDEALModeSwitch, &ORDEALSlewSwitch, &ORDEALFDAI1Switch, &ORDEALFDAI2Switch, &ORDEALLightingSwitch);

	//LM Mission Programer
	lmp.Init(this);

	//Mechanical Accelerometer
	mechanicalAccelerometer.Init(this);

	//Instrumentation
	scera1.Init(this, &INST_SIG_CONDR_1_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SCERAHEAT"));
	scera2.Init(this, &INST_SIG_CONDR_2_CB, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:SCERAHEAT"));

	// DS20060413 Initialize joystick
	js_enabled = 0;  // Disabled
	rhc_id = -1;     // Disabled
	rhc_rot_id = -1; // Disabled
	rhc_sld_id = -1; // Disabled
	rhc_rzx_id = -1; // Disabled
	thc_id = -1;     // Disabled
	thc_rot_id = -1; // Disabled
	thc_rzx_id = -1; // Disabled
	thc_tjt_id = -1; // Disabled
	thc_debug = -1;
	rhc_debug = -1;
	rhc_thctoggle = false;
	rhc_thctoggle_id = -1;
	rhc_thctoggle_pressed = false;

	ttca_throttle_pos = 0;
	ttca_throttle_vel = 0;
	ttca_throttle_pos_dig = 0;
	
	// Initialize other systems
	atca.Init(this, (h_HeatLoad *)Panelsdk.GetPointerByString("HYDRAULIC:ATCAHEAT"));
}

void LEM::JoystickTimestep(double simdt)
{
	// Joystick read
	if (oapiGetFocusInterface() == this) {
		// Invert joystick configuration according to navmode in case of one joystick
		int tmp_id, tmp_rot_id, tmp_sld_id, tmp_rzx_id, tmp_pov_id, tmp_debug;
		if (rhc_thctoggle && ((rhc_id != -1 && thc_id == -1 && GetAttitudeMode() == RCS_LIN) ||
			(rhc_id == -1 && thc_id != -1 && GetAttitudeMode() == RCS_ROT))) {

			tmp_id = rhc_id;
			tmp_rot_id = rhc_rot_id;
			tmp_sld_id = rhc_sld_id;
			tmp_rzx_id = rhc_rzx_id;
			tmp_pov_id = rhc_pov_id;
			tmp_debug = rhc_debug;

			rhc_id = thc_id;
			rhc_rot_id = thc_rot_id;
			rhc_sld_id = thc_tjt_id;
			rhc_rzx_id = thc_rzx_id;
			rhc_pov_id = thc_pov_id;
			rhc_debug = thc_debug;

			thc_id = tmp_id;
			thc_rot_id = tmp_rot_id;
			thc_tjt_id = tmp_sld_id;
			thc_rzx_id = tmp_rzx_id;
			thc_pov_id = tmp_pov_id;
			thc_debug = tmp_debug;
		}

		if (thc_id != -1 && !(thc_id < js_enabled)) {
			sprintf(oapiDebugString(), "DX8JS: Joystick selected as THC does not exist.");
		}
		if (rhc_id != -1 && !(rhc_id < js_enabled)) {
			sprintf(oapiDebugString(), "DX8JS: Joystick selected as RHC does not exist.");
		}

		// Zero ACA and TTCA bits in channel 31
		ChannelValue val31;
		val31 = agc.GetInputChannel(031);
		val31 &= 030000; // Leaves AttitudeHold and AutomaticStab alone

		int ttca_pos[3];
		int thc_x_pos = 0;
		int thc_y_pos = 0;
		int thc_z_pos = 0;
		int thc_rot_pos = 32768; // Initialize to centered
		int thc_tjt_pos = 32768; // Initialize to centered
		bool ttca_realistic_throttle = false;

		int rhc_pos[3];     // RHC x/y/z positions
		rhc_pos[0] = 32768; // Initialize
		rhc_pos[1] = 32768;
		rhc_pos[2] = 32768;


		/* ACA OPERATION:

		The LM ACA is a lot different from the CM RHC.
		The ACA works on a D/A converter.
		The OUT OF DETENT switch closes at .5 degrees of travel, and enables the proportional
		voltage circuit ("A" CIRCUIT) to operate.
		The hand controller must be moved 2 degrees to generate a count.
		8 degrees of usable travel, at .190 degrees per count.
		10 degrees total travel = 42 counts.

		*/
		// Axes have 32768 points of travel for the 13 degrees to hard stop
		// 2520 points per degree. It breaks out of detent at .5 degres, or 1260 pulses.
		// 480 points per count.

		// No JS

		// Roll
		if (GetManualControlLevel(THGROUP_ATT_BANKLEFT) > 0) {
			rhc_pos[0] = (int)(32768 - GetManualControlLevel(THGROUP_ATT_BANKLEFT) * 32768);
		}
		else if (GetManualControlLevel(THGROUP_ATT_BANKRIGHT) > 0) {
			rhc_pos[0] = (int)(32768 + GetManualControlLevel(THGROUP_ATT_BANKRIGHT) * 32768);
		}
		// Pitch
		if (GetManualControlLevel(THGROUP_ATT_PITCHDOWN) > 0) {
			rhc_pos[1] = (int)(32768 - GetManualControlLevel(THGROUP_ATT_PITCHDOWN) * 32768);
		}
		else if (GetManualControlLevel(THGROUP_ATT_PITCHUP) > 0) {
			rhc_pos[1] = (int)(32768 + GetManualControlLevel(THGROUP_ATT_PITCHUP) * 32768);
		}
		// Yaw
		if (GetManualControlLevel(THGROUP_ATT_YAWLEFT) > 0) {
			rhc_pos[2] = (int)(32768 + GetManualControlLevel(THGROUP_ATT_YAWLEFT) * 32768);
		}
		else if (GetManualControlLevel(THGROUP_ATT_YAWRIGHT) > 0) {
			rhc_pos[2] = (int)(32768 - GetManualControlLevel(THGROUP_ATT_YAWRIGHT) * 32768);
		}

		CDR_ACA.Timestep(rhc_pos);

		if (CDR_ACA.GetOutOfDetent())
		{
			val31[ACAOutOfDetent] = 1;
		}

		if (YawSwitch.IsUp())
		{
			if (CDR_ACA.GetMinusYawBreakout())
			{
				val31[MinusYaw] = 1;
			}
			if (CDR_ACA.GetPlusYawBreakout())
			{
				val31[PlusYaw] = 1;
			}
		}

		if (PitchSwitch.IsUp())
		{
			if (CDR_ACA.GetMinusPitchBreakout())
			{
				val31[MinusElevation] = 1;
			}
			if (CDR_ACA.GetPlusPitchBreakout())
			{
				val31[PlusElevation] = 1;
			}
		}

		if (RollSwitch.IsUp())
		{
			if (CDR_ACA.GetMinusRollBreakout())
			{
				val31[MinusAzimuth] = 1;
			}
			if (CDR_ACA.GetPlusRollBreakout())
			{
				val31[PlusAzimuth] = 1;
			}
		}

		//
		// HARDOVER
		//

		if (LeftACA4JetSwitch.IsUp() && SCS_ATT_DIR_CONT_CB.Voltage() > SP_MIN_DCVOLTAGE)
		{
			if (CDR_ACA.GetMinusRollHardover()) {
				// MINUS ROLL
				SetRCSJet(0, 1);
				SetRCSJet(4, 1);
				SetRCSJet(11, 1);
				SetRCSJet(15, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184); // Four thrusters worth
			}
			if (CDR_ACA.GetPlusRollHardover()) {
				// PLUS ROLL
				SetRCSJet(3, 1);
				SetRCSJet(7, 1);
				SetRCSJet(8, 1);
				SetRCSJet(12, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetMinusPitchHardover()) {
				// MINUS PITCH
				SetRCSJet(0, 1);
				SetRCSJet(7, 1);
				SetRCSJet(11, 1);
				SetRCSJet(12, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetPlusPitchHardover()) {
				// PLUS PITCH
				SetRCSJet(3, 1);
				SetRCSJet(4, 1);
				SetRCSJet(8, 1);
				SetRCSJet(15, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetMinusYawHardover()) {
				// MINUS YAW
				SetRCSJet(2, 1);
				SetRCSJet(6, 1);
				SetRCSJet(9, 1);
				SetRCSJet(13, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
			if (CDR_ACA.GetPlusYawHardover()) {
				// PLUS YAW
				SetRCSJet(1, 1);
				SetRCSJet(5, 1);
				SetRCSJet(10, 1);
				SetRCSJet(14, 1);

				SCS_ATT_DIR_CONT_CB.DrawPower(184);
			}
		}

		//
		// DIRECT
		//

		if (SCS_ATT_DIR_CONT_CB.Voltage() > SP_MIN_DCVOLTAGE)
		{
			if (RollSwitch.IsDown())
			{
				if (CDR_ACA.GetMinusRollBreakout()) {
					// MINUS ROLL
					SetRCSJet(4, 1);
					SetRCSJet(11, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
				if (CDR_ACA.GetPlusRollBreakout()) {
					// PLUS ROLL
					SetRCSJet(3, 1);
					SetRCSJet(12, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
			}

			if (PitchSwitch.IsDown())
			{
				if (CDR_ACA.GetMinusPitchBreakout()) {
					// MINUS PITCH
					SetRCSJet(11, 1);
					SetRCSJet(12, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
				if (CDR_ACA.GetPlusPitchBreakout()) {
					// PLUS PITCH
					SetRCSJet(3, 1);
					SetRCSJet(4, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
			}

			if (YawSwitch.IsDown())
			{
				if (CDR_ACA.GetMinusYawBreakout()) {
					// MINUS YAW
					SetRCSJet(2, 1);
					SetRCSJet(9, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
				if (CDR_ACA.GetPlusYawBreakout()) {
					// PLUS YAW
					SetRCSJet(1, 1);
					SetRCSJet(10, 1);

					SCS_ATT_DIR_CONT_CB.DrawPower(92);
				}
			}
		}

		//Debug
		if (rhc_debug != -1)
		{
			sprintf(oapiDebugString(), "RHC: X/Y/Z = %d / %d / %d | rzx_id %d rot_id %d sld_id %d", rhc_pos[0], rhc_pos[1], rhc_pos[2], rhc_rzx_id, rhc_rot_id, rhc_sld_id);
		}

		//
		// +X TRANSLATION
		//

		if (PlusXTranslationButton.GetState() == 1 && SCS_ATT_DIR_CONT_CB.IsPowered()) {
			SetRCSJet(7, true);
			SetRCSJet(15, true);
			SetRCSJet(11, true);
			SetRCSJet(3, true);
		}

		
		// No JS

		// Up/down
		if (GetManualControlLevel(THGROUP_ATT_DOWN) > 0) {
			thc_y_pos = (int)((-GetManualControlLevel(THGROUP_ATT_DOWN)) * 32768.);
		}
		else if (GetManualControlLevel(THGROUP_ATT_UP) > 0) {
			thc_y_pos = (int)(GetManualControlLevel(THGROUP_ATT_UP) * 32768.);
		}
		// Left/right
		if (GetManualControlLevel(THGROUP_ATT_LEFT) > 0) {
			thc_x_pos = (int)((-GetManualControlLevel(THGROUP_ATT_LEFT)) * 32768.);
		}
		else if (GetManualControlLevel(THGROUP_ATT_RIGHT) > 0) {
			thc_x_pos = (int)(GetManualControlLevel(THGROUP_ATT_RIGHT) * 32768.);
		}
		// Forward/Back
		if (GetManualControlLevel(THGROUP_ATT_BACK) > 0) {
			thc_z_pos = (int)((-GetManualControlLevel(THGROUP_ATT_BACK)) * 32768.);
		}
		else if (GetManualControlLevel(THGROUP_ATT_FORWARD) > 0) {
			thc_z_pos = (int)(GetManualControlLevel(THGROUP_ATT_FORWARD) * 32768.);
		}

		//sprintf(oapiDebugString(), "%f %f", ttca_throttle_pos_dig, ttca_thrustcmd);

		ttca_pos[0] = thc_y_pos;
		ttca_pos[1] = thc_x_pos;
		ttca_pos[2] = thc_z_pos;

		CDR_TTCA.Timestep(ttca_pos, thc_tjt_pos, ttca_realistic_throttle, ttca_throttle_pos_dig);

		if (LeftTTCATranslSwitch.IsUp())
		{
			if (CDR_TTCA.GetMinusXTrans()) {
				val31[MinusX] = 1;
			}
			if (CDR_TTCA.GetMinusYTrans()) {
				val31[MinusY] = 1;
			}
			if (CDR_TTCA.GetPlusXTrans()) {
				val31[PlusX] = 1;
			}
			if (CDR_TTCA.GetPlusYTrans()) {
				val31[PlusY] = 1;
			}
			if (CDR_TTCA.GetMinusZTrans()) {
				val31[MinusZ] = 1;
			}
			if (CDR_TTCA.GetPlusZTrans()) {
				val31[PlusZ] = 1;
			}
		}

		if (ttca_throttle_vel == 1)
		{
			ttca_throttle_pos_dig += 0.25*simdt;
		}
		else if (ttca_throttle_vel == -1)
		{
			ttca_throttle_pos_dig -= 0.25*simdt;
		}
		if (ttca_throttle_pos_dig > 1)
		{
			ttca_throttle_pos_dig = 1;
		}
		else if (ttca_throttle_pos_dig < 0)
		{
			ttca_throttle_pos_dig = 0;
		}

		//LM Programer
		if (pMission->HasLMProgramer())
		{
			if (lmp.GetPlusXTrans())
			{
				val31[PlusX] = 1;
			}
		}

		// Write back channel data
		agc.SetInputChannel(031, val31);
	}
}

void LEM::SystemsInternalTimestep(double simdt)
{
	double mintFactor = __max(simdt / 50.0, 0.02);
	double tFactor = __min(mintFactor, simdt);
	while (simdt > 0) {

		// Each Timestep is passed to the SPSDK
		// to perform internal computations on the 
		// systems.

		Panelsdk.SimpleTimestep(tFactor);

		agc.SystemTimestep(tFactor);								// Draw power & generate heat
		dsky.SystemTimestep(tFactor);								// This can draw power now.
		asa.SystemTimestep(tFactor);
		aea.SystemTimestep(tFactor);
		deda.SystemTimestep(tFactor);
		imu.SystemTimestep(tFactor);								// Draw power
		atca.SystemTimestep(tFactor);
		rga.SystemTimestep(tFactor);
		ordeal.SystemTimestep(tFactor);
		fdaiLeft.SystemTimestep(tFactor);							// Draw Power
		fdaiRight.SystemTimestep(tFactor);
		LR.SystemTimestep(tFactor);
		RR.SystemTimestep(tFactor);
		RadarTape.SystemTimestep(tFactor);
		crossPointerLeft.SystemTimestep(tFactor);
		crossPointerRight.SystemTimestep(tFactor);
		SBandSteerable.SystemTimestep(tFactor);
		VHF.SystemTimestep(tFactor);
		PCM.SystemTimestep(tFactor);
		SBand.SystemTimestep(tFactor);
		DSEA.SystemTimestep(tFactor);
		CabinPressureSwitch.SystemTimestep(tFactor);
		SuitPressureSwitch.SystemTimestep(tFactor);
		CabinRepressValve.SystemTimestep(tFactor);
		SuitCircuitPressureRegulatorA.SystemTimestep(tFactor);
		SuitCircuitPressureRegulatorB.SystemTimestep(tFactor);
		CDRIsolValve.SystemTimestep(tFactor);
		LMPIsolValve.SystemTimestep(tFactor);
		OVHDCabinReliefDumpValve.SystemTimestep(tFactor);
		FWDCabinReliefDumpValve.SystemTimestep(tFactor);
		SuitCircuitReliefValve.SystemTimestep(tFactor);
		SuitGasDiverter.SystemTimestep(tFactor);
		CabinGasReturnValve.SystemTimestep(tFactor);
		CO2CanisterSelect.SystemTimestep(tFactor);
		PrimCO2CanisterVent.SystemTimestep(tFactor);
		SecCO2CanisterVent.SystemTimestep(tFactor);
		WaterSeparationSelector.SystemTimestep(tFactor);
		WaterTankSelect.SystemTimestep(tFactor);
		CabinFan.SystemTimestep(tFactor);
		PrimGlycolPumpController.SystemTimestep(tFactor);
		SuitFanDPSensor.SystemTimestep(tFactor);
		DPSPropellant.SystemTimestep(tFactor);
		DPS.SystemTimestep(tFactor);
		deca.SystemTimestep(tFactor);
		gasta.SystemTimestep(tFactor);
		scera1.SystemTimestep(tFactor);
		scera2.SystemTimestep(tFactor);
		MissionTimerDisplay.SystemTimestep(tFactor);
		EventTimerDisplay.SystemTimestep(tFactor);
		CWEA.SystemTimestep(tFactor);
		if (stage < 2)
		{
			ECA_1.SystemTimestep(tFactor);
			ECA_2.SystemTimestep(tFactor);
		}
		ECA_3.SystemTimestep(tFactor);
		ECA_4.SystemTimestep(tFactor);
		tle.SystemTimestep(tFactor);
		DockLights.SystemTimestep(tFactor);
		lca.SystemTimestep(tFactor);
		UtilLights.SystemTimestep(tFactor);
		COASLights.SystemTimestep(tFactor);
		FloodLights.SystemTimestep(tFactor);
		INV_1.SystemTimestep(tFactor);
		INV_2.SystemTimestep(tFactor);

		simdt -= tFactor;
		tFactor = __min(mintFactor, simdt);
	}
}

void LEM::SystemsTimestep(double simt, double simdt)

{

	// Clear debug line when timer runs out
	if(DebugLineClearTimer > 0){
		DebugLineClearTimer -= simdt;
		if(DebugLineClearTimer < 0){
			sprintf(oapiDebugString(),"");
			DebugLineClearTimer = 0;
		}
	}

	SystemsInternalTimestep(simdt);

	// After that come all other systems simesteps
	agc.Timestep(MissionTime, simdt);						// Do work
	dsky.Timestep(MissionTime);								// Do work
	asa.Timestep(simdt);									// Do work
	aea.Timestep(MissionTime, simdt);
	deda.Timestep(simdt);
	imu.Timestep(simdt);								// Do work
	tcdu.Timestep(simdt);
	scdu.Timestep(simdt);
	// Manage IMU standby heater and temperature
	if(IMU_OPR_CB.Voltage() > 0){
		// IMU is operating.
		if(imublower->h_pump != 1){ imublower->SetPumpAuto(); }
	}else{
		// IMU is not operating.
		if(imublower->h_pump != 0){ imublower->SetPumpOff(); }
	}

	// FIXME: Draw power for lighting system.
	// This will be done in the LCA and individual lighting components

	// Allow ATCA to operate between the FDAI and AGC/AEA so that any changes the FDAI makes
	// can be shown on the FDAI, but any changes the AGC/AEA make are visible to the ATCA.
	atca.Timestep(simt, simdt);
	rga.Timestep(simdt);
	ordeal.Timestep(simdt);
	mechanicalAccelerometer.Timestep(simdt);

	//Move this to panel code or wherever
	VECTOR3 attitude;
	if (AttitudeMonSwitch.IsUp())	//PGNS
	{
		attitude = gasta.GetTotalAttitude();
	}
	else							//AGS
	{
		attitude = aea.GetTotalAttitude();
	}

	// ORDEAL
	attitude.y += ordeal.GetFDAI1PitchAngle();
	if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
	fdaiLeft.SetAttitude(attitude);
	fdaiLeft.Timestep(MissionTime, simdt);

	if (RightAttitudeMonSwitch.IsUp())	//PGNS
	{
		attitude = gasta.GetTotalAttitude();
	}
	else							//AGS
	{
		attitude = aea.GetTotalAttitude();
	}
	// ORDEAL
	attitude.y += ordeal.GetFDAI2PitchAngle();
	if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
	fdaiRight.SetAttitude(attitude);
	fdaiRight.Timestep(MissionTime, simdt);

	MissionTimerDisplay.Timestep(MissionTime, simdt, false);
	EventTimerDisplay.Timestep(MissionTime, simdt, false);
	JoystickTimestep(simdt);
	eds.Timestep(simdt);
	optics.Timestep(simdt);
	LR.Timestep(simdt);
	RR.Timestep(simdt);
	RadarTape.Timestep(simdt);
	crossPointerLeft.Timestep(simdt);
	crossPointerRight.Timestep(simdt);
	SBandSteerable.Timestep(simdt);
	omni_fwd.Timestep();
	omni_aft.Timestep();
	SBand.Timestep(simt);
	VHF.Timestep(simt);
	ecs.Timestep(simdt);
	OverheadHatch.Timestep(simdt);
	ForwardHatch.Timestep(simdt);
	CrewStatus.Timestep(simdt);
	scca1.Timestep(simdt);
	scca2.Timestep(simdt);
	scca3.Timestep(simdt);
	DPSPropellant.Timestep(simt, simdt);
	DPS.Timestep(simt, simdt);
	APSPropellant.Timestep(simt, simdt);
	APS.Timestep(simdt);
	RCSA.Timestep(simt, simdt);
	RCSB.Timestep(simt, simdt);
	tca1A.Timestep(simdt);
	tca2A.Timestep(simdt);
	tca3A.Timestep(simdt);
	tca4A.Timestep(simdt);
	tca1B.Timestep(simdt);
	tca2B.Timestep(simdt);
	tca3B.Timestep(simdt);
	tca4B.Timestep(simdt);
	deca.Timestep(simdt);
	gasta.Timestep(simt);
	//TBD: Apollo 14 version with EMER CM PWR circuit breaker. Also, move these relays into a class for the panel
	if (pMission->GetCMtoLMPowerConnectionVersion() == 0)
	{
		//Apollo 13 and earlier: CM to LM power doesn't work after staging
		CMPowerToCDRBusRelayA = stage < 2 && (CSMToLEMPowerConnector.GetBatteriesLVHVOffA() || CSMToLEMPowerConnector.GetBatteriesLVHVOffB()) && !ECA_1.GetSectALVOn() && !ECA_1.GetSectBLVOn() && !ECA_2.GetSectALVOn() && !ECA_2.GetSectBLVOn();
	}
	else
	{
		//Apollo 15 and later: circuit to descent stage bypassed after staging
		if (eds.GetDeadface())
		{
			CMPowerToCDRBusRelayA = (CSMToLEMPowerConnector.GetBatteriesLVHVOffA() || CSMToLEMPowerConnector.GetBatteriesLVHVOffB());
		}
		else
		{
			CMPowerToCDRBusRelayA = (CSMToLEMPowerConnector.GetBatteriesLVHVOffA() || CSMToLEMPowerConnector.GetBatteriesLVHVOffB()) && !ECA_1.GetSectALVOn() && !ECA_1.GetSectBLVOn() && !ECA_2.GetSectALVOn() && !ECA_2.GetSectBLVOn();
		}
	}
	CMPowerToCDRBusRelayB = CMPowerToCDRBusRelayA;
	rjb.Timestep();
	drb.Timestep();
	if (stage < 2)
	{
		ECA_1.Timestep(simdt);
		ECA_2.Timestep(simdt);
	}
	ECA_3.Timestep(simdt);
	ECA_4.Timestep(simdt);
	tle.Timestep(simdt);
	DockLights.Timestep(simdt);
	UtilLights.Timestep(simdt);
	COASLights.Timestep(simdt);
	FloodLights.Timestep(simdt);
	pfira.Timestep(simdt);

	// Do this toward the end so we can see current system state
	scera1.Timestep();
	scera2.Timestep();
	CWEA.Timestep(simdt);
	DSEA.Timestep(simt, simdt);

	//Treat LM O2 as gas every timestep
	DesO2Tank->BoilAllAndSetTemp(294.261);
	AscO2Tank1->BoilAllAndSetTemp(294.261);
	AscO2Tank2->BoilAllAndSetTemp(294.261);
	DesO2Manifold->BoilAllAndSetTemp(294.261);
	O2Manifold->BoilAllAndSetTemp(294.261);
	PressRegA->BoilAllAndSetTemp(285.928);
	PressRegB->BoilAllAndSetTemp(285.928);

	//System Generated Heat

	//Secondary Glycol Pump Heat
	if (SecGlyPump->pumping) {
		SecGlyPumpHeat->GenerateHeat(30.5);
	}

	//Suit Fan Heat
	if (SuitFan1->pumping) {
		SuitFan1Heat->GenerateHeat(114.1); //70% of 163W
	}
	if (SuitFan2->pumping) {
		SuitFan2Heat->GenerateHeat(114.1); //70% of 163W
	}

	//Seq Camera Power/Heat
	if (CAMR_SEQ_CB.Voltage() > SP_MIN_DCVOLTAGE) {
		CAMR_SEQ_CB.DrawPower(14.0);
		CabinHeat->GenerateHeat(12.6);	//This should only generate heat when the camera is active, as it has it's own on/off switch, not sure if heat goes into cabin or power
	}

	//Cabin Window Heaters
	//We will assume heat generated is radiated into space

	//Overhead Docking Window
	if (HTR_DOCK_WINDOW_CB.Voltage() > SP_MIN_DCVOLTAGE) {
		HTR_DOCK_WINDOW_CB.DrawPower(24.0);
	}
	//CDR Forward Window
	if (CDR_WND_HTR_AC_CB.Voltage() > SP_MIN_ACVOLTAGE) {
		CDR_WND_HTR_AC_CB.DrawPower(61.8);
	}
	//LMP Forward Window
	if (SE_WND_HTR_AC_CB.Voltage() > SP_MIN_ACVOLTAGE) {
		SE_WND_HTR_AC_CB.DrawPower(61.8);
	}

	//Misc Sounds

	//Suit Fan Sound
	if (SuitFan1->pumping || SuitFan2->pumping) {
		SuitFanSound.play(220);
	}
	else
	{
		SuitFanSound.stop();
	}

	//Glycol Pump Sound
	if (SecGlyPump->pumping || PrimGlycolPumpController.GetGlycolPumpState(1) || PrimGlycolPumpController.GetGlycolPumpState(2)) {
		GlycolPumpSound.play();
	}
	else
	{
		GlycolPumpSound.stop();
	}
	
	/*
	// Debug tests //

	// Mesh Index Order
	//sprintf(oapiDebugString(), "ascidx %i vcidx %i dscidx %i", ascidx, vcidx, dscidx);

	//sprintf(oapiDebugString(), "CDRinPLSS %i CDRsuited %i LMPinPLSS %i LMPsuited %i CrewInCabin %i", CDRinPLSS, CDRSuited->number, LMPinPLSS, LMPSuited->number, CrewInCabin->number);
	//sprintf(oapiDebugString(), "InPanel %d InVC %d ExtView %d InFOV %d", InPanel, InVC, ExtView, InFOV);

	//ECS Debug Lines//
	
	double *O2ManifoldPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD:PRESS");
	double *O2ManifoldMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD:MASS");
	double *O2ManifoldTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD:TEMP");
	double *DESO2ManifoldPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:PRESS");
	double *DESO2ManifoldMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:MASS");
	double *DESO2ManifoldTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:TEMP");

	int *deso2outvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:OUT:ISOPEN");
	int *deso2manifoldinvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:IN:ISOPEN");
	int *deso2manifoldoutvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:OUT:ISOPEN");
	int *asco2out1vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:OUT:ISOPEN");
	int *asco2out2vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:OUT:ISOPEN");

	double *DesO2pipeflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE1:FLOW");
	double *DesO2pipeflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE1:FLOWMAX");
	float *DesO2vlvoutsize = (float*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:OUT:SIZE");
	float *DesO2Manifoldvlvinsize = (float*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:IN:SIZE");

	double *DESO2TankTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:TEMP");
	double *DESO2TankMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:MASS");
	double *DESO2VapMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:O2_VAPORMASS");
	double *DESO2TankEnergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:ENERGY");
	double *DESO2ManifoldEnergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:ENERGY");
	double *DESO2PP = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2TANK:O2_PPRESS");

	double *asco2tk1press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:PRESS");
	double *asco2tk2press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:PRESS");

	int *pressRegAvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:OUT:ISOPEN");
	int *pressRegBvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:OUT:ISOPEN");
	double *PressRegAPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:PRESS");
	double *PressRegBPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:PRESS");
	double *PressRegAMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:MASS");
	double *PressRegBMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:MASS");
	double *PressRegATemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGA:TEMP");
	double *PressRegBTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGB:TEMP");

	double *SuitCircuitPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:PRESS");
	double *SuitCircuitMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:MASS");
	double *SuitCircuitTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:TEMP");
	double *SuitCircuitOutFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITOUT:FLOW");

	int *suitReliefvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:OUT2:ISOPEN");
	double *suitReliefflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRELIEFVALVE:FLOW");
	double *suitReliefflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRELIEFVALVE:FLOWMAX");

	double *deso2flow2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE2:FLOW");
	double *deso2flowmax2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PIPE2:FLOWMAX");

	double *repressFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESS:FLOW");
	double *repressFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESS:FLOWMAX");
	double *pressRegAFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGAOUT:FLOW");
	double *pressRegAFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGAOUT:FLOWMAX");
	double *pressRegBFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRESSREGBOUT:FLOW");

	int *fwdHatchvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:ISOPEN");
	int *ovhdHatchvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT2:ISOPEN");
	double *ovhdHatchFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE:FLOW");
	double *ovhdHatchFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE:FLOWMAX");
	float *ovhdHatchSize = (float*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT2:SIZE");
	double *fwdHatchFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINFWDHATCHVALVE:FLOW");
	double *fwdHatchFlowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINFWDHATCHVALVE:FLOWMAX");

	double *cabinventpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINVENT:PRESS");

	double *CabinMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:MASS");
	double *CabinPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	
	double *CabinTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	double *CabinStructureTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINSTRUCTURE:TEMP");
	
	int *suitGasDiverterCabinVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:OUT:ISOPEN");
	double *suitGasDiverterCabinFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTERCABINOUT:FLOW");
	double * suitGasDiverterCO2ManifoldFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTEREGRESSOUT:FLOW");

	int *suitGasDiverterEgressVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:OUT2:ISOPEN");

	int *cabinGasReturnVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:LEAK:ISOPEN");
	double *cabinGasReturnFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINGASRETURN:FLOW");
	double *CDRSuitFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUITISOL:FLOW");
	double *LMPSuitFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUITISOL:FLOW");

	int *primCO2InVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:IN:ISOPEN");
	int *primCO2OutVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:OUT:ISOPEN");
	int *secCO2InVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:IN:ISOPEN");
	int *secCO2OutVLV = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:OUT:ISOPEN");
	double *primCO2CanisterPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:PRESS");
	double* primCO2CanisterTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:TEMP");
	double *secCO2CanisterPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:PRESS");
	double* secCO2CanisterTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:TEMP");
	double *primCO2CanisterMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:MASS");
	double *secCO2CanisterMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:MASS");
	int *primCO2Vent = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:OUT2:ISOPEN");
	int *secCO2Vent = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:OUT2:ISOPEN");
	double *primCO2VentFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2VENT:FLOW");
	double *secCO2VentFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2VENT:FLOW");

	double* suitfanmanifoldinPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDIN:PRESS");
	double *suitfanmanifoldoutPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDOUT:PRESS");
	double* suitfanmanifoldinTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDIN:TEMP");
	double* suitfanmanifoldoutTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDOUT:TEMP");
	double *hxheatingPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:PRESS");
	double* SuitFanOutFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDOUTPIPE:FLOW");
	
	double *hxheatingTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:TEMP");
	
	double *hxheatingMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:MASS");
	double *hxheatingPower = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEAT:POWER");
	double *SGDPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:PRESS");
	double *SGDMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:MASS");
	double *SGDTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:TEMP");
	double *hxcoolingMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:MASS");
	double *hxcoolingPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:PRESS");
	double *hxcoolingTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:TEMP");
	double *hxcoolingPower = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOL:POWER");
	double *WSMinMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLDIN:MASS");
	double *WSMinPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLDIN:PRESS");
	double *WSMinTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLDIN:TEMP");
	double* WSMoutPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLDOUT:PRESS");
	double* WSMoutTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPMANIFOLDOUT:TEMP");

	double *desO2burstflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2BURSTDISK:FLOW");
	double *desO2reliefflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2PRESSURERELIEFVALVE:FLOW");

	double *primglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:TEMP");
	double *primglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:PRESS");
	double *primglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:MASS");

	//double *primglycolaccumpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATORACTUAL:PRESS");
	//double* primglycolaccumvolume = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATORACTUAL:VOLUME");

	int *primevapPump = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:PUMP");
	int *primevapValve = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:VALVE");
	double *primevapThrottle = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:THROTTLE");
	double *primevapSteam = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:STEAMPRESSURE");

	double *primCO2Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2ABSORBER:FLOW");
	double *primCO2FlowMax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2ABSORBER:FLOWMAX");
	double *primCO2Removal = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2ABSORBER:CO2REMOVALRATE");
	double *CO2ManifoldPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:PRESS");
	double *CO2ManifoldMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:MASS");
	double *CO2ManifoldTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:TEMP");
	int *gasreturnvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:LEAK:ISOPEN");

	double *WS1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEP1:FLOW");
	double *WS1FlowMax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEP1:FLOWMAX");
	double *WS1H2ORemoval = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEP1:H2OREMOVALRATE");
	int *WS1vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:OUT:ISOPEN");
	int *WS2vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:OUT2:ISOPEN");

	double *primCO2Mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:MASS");
	double *suitfanmanifoldoutMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDOUT:MASS");

	double *DesH2OMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESH2OTANK:MASS");
	int *DesH2Ovlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:DESH2OTANK:OUT2:ISOPEN");

	double *WTSMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT:MASS");
	int *WTSdesinvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT:IN:ISOPEN");
	int *WTSdesoutvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OTANKSELECT:OUT2:ISOPEN");
	double *STMass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:MASS");
	double *STPress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:PRESS");
	int *surgeprimvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:OUT:ISOPEN");
	int *surgesecvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2OSURGETANK:OUT2:ISOPEN");
	double *SToutflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERSEPPRIMOUT:FLOW");

	double *primregmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMREG:MASS");
	int *primevap1vlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMREG:OUT:ISOPEN");

	double *primwbmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:MASS");
	double *primwbpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:PRESS");
	int *primwbinvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:IN:ISOPEN");
	int *primwboutvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMWATERBOILER:OUT:ISOPEN");

	double *primloop1mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP1:MASS");
	
	double *primloop1temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP1:TEMP");
	
	double *primloop1press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP1:PRESS");
	double *primloop2mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP2:MASS");
	
	double *primloop2temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP2:TEMP");
	//double *primloop2press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLLOOP2:PRESS");
	
	double *primevapinmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:MASS");
	double *primevaptempin = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:TEMP");
	double *primevapinpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPINLET:PRESS");
	double *primevapoutmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:MASS");
	double *primevaptempout = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:TEMP");
	double *primevapoutpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPOUTLET:PRESS");

	double *Pump1Flow = (double*)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP1:FLOW");
	double *Pump2Flow = (double*)Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP2:FLOW");
	double *Pump1OutFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT1:FLOW");
	double *Pump2OutFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYPUMPMANIFOLDOUT2:FLOW");
	double *primGlyReg1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYFLOWREG1:FLOW");
	double *waterGlyHXFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHXOUT:FLOW");
	double *suitHXGlyFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROL:FLOW");
	double *suitHXGlyFlowMax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROL:FLOWMAX");
	double *suitHXGlyBypassFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROLBYPASS:FLOW");

	double *secglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:MASS");
	double *secglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:PRESS");
	double *secglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:TEMP");
	double *secpumpmanifoldmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:MASS");
	double *secpumpmanifoldpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:PRESS");
	double *secpumpmanifoldtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLPUMPFANMANIFOLD:TEMP");
	double *secloop1mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP1:MASS");
	double *secloop1press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP1:PRESS");
	double *secloop1temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP1:TEMP");
	double *secloop2mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP2:MASS");
	double *secloop2press = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP2:PRESS");
	double *secloop2temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLLOOP2:TEMP");
	double *secevapinmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPINLET:MASS");
	double *secevapinpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPINLET:PRESS");
	double *secevaptempin = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPINLET:TEMP");
	double *secevapoutmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:MASS");
	double *secevapoutpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:PRESS");
	double *secevaptempout = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:TEMP");
	double *secascbatmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASCBATCOOLING:MASS");
	double *secascbatpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASCBATCOOLING:PRESS");
	double *secascbattemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASCBATCOOLING:TEMP");
	double *secGlyReg1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYFLOWREG1:FLOW");
	double *secGlyReg2Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYFLOWREG2:FLOW");

	int *secevapPump = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:PUMP");
	int *secevapValve = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:VALVE");
	double *secevapThrottle = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:THROTTLE");
	double *secevapSteam = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:STEAMPRESSURE");

	int *slevapPump = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:PUMP");
	int *slevapValve = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:VALVE");
	double *slevapThrottle = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:THROTTLE");
	double *slevapSteam = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITLOOPEVAPORATOR:STEAMPRESSURE");

	double *secreg2mass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECREG2:MASS");
	int *secevapvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:SECREG1MANIFOLD:OUT2:ISOPEN");
	double *secevapmassout = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPOUTLET:MASS");

	double *lcgaccumass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGACCUMULATOR:MASS");
	double *lcgaccupress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGACCUMULATOR:PRESS");
	double *lcgaccutemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGACCUMULATOR:TEMP");
	double *lcghxmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGHX:MASS");
	double *lcghxpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGHX:PRESS");
	double *lcghxtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGHX:TEMP");
	double *lcgmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCG:MASS");
	double *lcgpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCG:PRESS");
	double *lcgtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCG:TEMP"); 

	double *lcghxflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROL:FLOW");
	double *lcghxflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROL:FLOWMAX");
	double *lcgaccflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROLBYPASS:FLOW");
	double *lcgaccflowmax = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROLBYPASS:FLOWMAX");

	double *glycolsuitcooltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXCOOLING:TEMP");
	double *glycolsuitheattemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXHEATING:TEMP");
	double *glycolpumpmanifoldtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:TEMP");
	double *waterglycolhxtemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHX:TEMP");
	double *ascbatglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBATCOOLING:TEMP");
	double *desbatglycoltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING:TEMP");

	double *glycolsuitcoolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXCOOLING:MASS");
	double *glycolsuitcoolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXCOOLING:PRESS");
	double *glycolsuitheatmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXHEATING:MASS");
	double *glycolsuitheatpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLSUITHXHEATING:PRESS");
	double *glycolpumpmanifoldmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:MASS");
	double *glycolpumpmanifoldpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLPUMPMANIFOLD:PRESS");

	double *waterglycolhxmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHX:MASS");
	double *waterglycolhxpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:WATERGLYCOLHX:PRESS");
	double *ascbatglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBATCOOLING:MASS");
	double *ascbatglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBATCOOLING:PRESS");
	double *desbatglycolmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING:MASS");
	double *desbatglycolpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATCOOLING:PRESS");

	double *cdrsuitmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:MASS");
	double *cdrsuitpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:PRESS");
	double *cdrsuittemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:TEMP");
	double *cdrsuitenergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:ENERGY");
	double *lmpsuitmass = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:MASS");
	double *lmpsuitpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:PRESS");
	double *lmpsuittemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:TEMP");
	double *lmpsuitenergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:ENERGY");

	double *SuitHXHCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:CO2_PPRESS");
	double *CDRSuitCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:CO2_PPRESS");
	double *LMPSuitCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:CO2_PPRESS");
	double *SuitGasDiverterCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITGASDIVERTER:CO2_PPRESS");
	double *SuitCircuitCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUIT:CO2_PPRESS");
	double *CabinCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	double *CanisterMFCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CO2CANISTERMANIFOLD:CO2_PPRESS");
	double *PrimCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMCO2CANISTER:CO2_PPRESS");
	double *SecCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECCO2CANISTER:CO2_PPRESS");
	double *SuitFanCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITFANMANIFOLDIN:CO2_PPRESS");
	double *SuitHXCCO2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERCOOLING:CO2_PPRESS");

	double *CabFan = (double*)Panelsdk.GetPointerByString("ELECTRIC:CABINFAN:ISON");

	//Prim Loop 1 Heat
	double *LGCHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LGCHEAT:HEAT");
	double *CDUHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CDUHEAT:HEAT");
	double *PSAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PSAHEAT:HEAT");
	double *TLEHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:TLEHEAT:HEAT");
	double *GASTAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:GASTAHEAT:HEAT");
	double *LCAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LCAHEAT:HEAT");
	double *DSEHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DSEHEAT:HEAT");
	double *ASAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASAHEAT:HEAT");
	double *PTAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PTAHEAT:HEAT");
	double *IMUHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:IMUHEAT:HEAT");
	double *RGAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:RGAHEAT:HEAT");

	//Prim Loop 1 Plates
	double* LGCRad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-LGC-Plate:TEMP");
	double* CDURad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-CDU-Plate:TEMP");
	double* PSARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-PSA-Plate:TEMP");
	double* TLERad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-TLE-Plate:TEMP");
	double* GASTARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-GASTA-Plate:TEMP");
	double* LCARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-LCA-Plate:TEMP");
	double* DSERad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-DSE-Plate:TEMP");
	
	double* ASARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-ASA-HSink:TEMP");
	
	double* PTARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-PTA-Plate:TEMP");
	
	double* IMURad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-IMU-Case:TEMP");
	
	double* RGARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-RGA-Plate:TEMP");
	
	//Prim Loop 2 Heat
	double* SBPHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SBPHEAT:HEAT");
	double* SBXHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SBXHEAT:HEAT");
	double* SPHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SPHEAT:HEAT");
	double* AEAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:AEAHEAT:HEAT");
	double* ATCAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ATCAHEAT:HEAT");
	double* SCERAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SCERAHEAT:HEAT");
	double* CWEAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CWEAHEAT:HEAT");
	double* RREHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:RREHEAT:HEAT");
	double* VHFHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:VHFHEAT:HEAT");
	double* INVHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:INVHEAT:HEAT");
	double* ECAHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ECAHEAT:HEAT");
	double* PCMHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PCMHEAT:HEAT");

	//Prim Loop 2 Plates
	double* SBDRad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-SBD-Plate:TEMP");
	double* AEAVHFRad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-AEA-VHF-Plate:TEMP");
	double* ATCAINVRad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-ATCA-INV-Plate:TEMP");
	double* SCERARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-SCERA-Plate:TEMP");
	double* CWEAPCMRad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-CWEA-PCM-Plate:TEMP");
	double* RRERad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-RRE-Plate:TEMP");
	double* SCERAECARad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-SCERA-ECA-Plate:TEMP");

	double* PRIMSECRad = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-PRIM-SEC-Plate:TEMP");
	double* PRIMSECHX1 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMSECPlateHX:POWER");
	double* PRIMSECHX2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMSECPlateHXSEC:POWER");

	double *SBDTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-SBand-Steerable-Antenna:TEMP");
	double *RRTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-RR-Antenna:TEMP");
	double *LRTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LEM-LR-Antenna:TEMP");
	double *SBDHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-SBand-Steerable-Antenna-Heater:ISON");
	double *RRStbyHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-StbyHeater:ISON");
	double *RRHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-RR-Antenna-Heater:ISON");
	double *LRHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-LR-Antenna-Heater:ISON");

	double *QD1Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD1:TEMP");
	double *QD2Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD2:TEMP");
	double *QD3Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD3:TEMP");
	double *QD4Temp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMRCSQUAD4:TEMP");

	double *CabinEnergy = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:ENERGY");
	double *CabinHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINHEAT:HEAT");

	double* CabinPlate = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-CABIN-Plate:TEMP");
	double* CabinHX1 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINHX1:POWER");
	double* CabinHX2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINHX2:POWER");
	double* CabinFanManifoldTemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINFANMANIFOLD:TEMP");
	double* CabinFanManifoldHX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINFANHX:POWER");
	double* CabinFanManifoldOutFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINFANPIPE:FLOW");

	//CSM LM Connection
	double *lmcabinpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	double *lmtunnelpress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNEL:PRESS");
	double *lmtunneltemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNEL:TEMP");
	double *lmtunnelflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNELUNDOCKED:FLOW");

	//Batteries
	double* bat1temp = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_A:TEMP");
	double* bat1heat = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_A:HEAT");
	double* desbat1HX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT1HX:POWER");
	double* bat2temp = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_B:TEMP");
	double* bat2heat = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_B:HEAT");
	double* desbat2HX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT2HX:POWER");
	double* bat3temp = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_C:TEMP");
	double* bat3heat = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_C:HEAT");
	double* desbat3HX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT3HX:POWER");
	double* bat4temp = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_D:TEMP");
	double* bat4heat = (double*)Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_D:HEAT");
	double* desbat4HX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT4HX:POWER");

	double* bat5temp = (double*)Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_A:TEMP");
	double* bat5heat = (double*)Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_A:HEAT");
	double* ascbat1HX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBAT1HX:POWER");
	double* bat5platetemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-ASCBAT1-Plate:TEMP");
	double* bat6temp = (double*)Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_B:TEMP");
	double* bat6heat = (double*)Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_B:HEAT");
	double* ascbat2HX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASCBAT2HX:POWER");
	double* bat6platetemp = (double*)Panelsdk.GetPointerByString("HYDRAULIC:LM-ASCBAT2-Plate:TEMP");

	double* edbatAtemp = (double*)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_A:TEMP");
	double* edbatAheat = (double*)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_A:HEAT");
	double* edbatAHX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:EDBATAHX:POWER");
	double* edbatBtemp = (double*)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_B:TEMP");
	double* edbatBheat = (double*)Panelsdk.GetPointerByString("ELECTRIC:BATTERY_ED_B:HEAT");
	double* edbatBHX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:EDBATBHX:POWER");

	//ASA
	
	double* ASAFastHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-ASA-FastHeater:ISON");
	double* ASAFineHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-ASA-FineHeater:ISON");
	
	double* ASAPrimHX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMASAGLYCOLHX:POWER");
	double* ASASecHX = (double*)Panelsdk.GetPointerByString("HYDRAULIC:SECASAGLYCOLHX:POWER");
	
	double* ASASelfHeat = (double*)Panelsdk.GetPointerByString("HYDRAULIC:ASAHEAT:HEAT");
	
	
	//IMU
	double* IMUHtr = (double*)Panelsdk.GetPointerByString("ELECTRIC:LM-IMU-Heater:ISON");
	
	double* ASAFineHtrPWM = (double*)Panelsdk.GetPointerByString("ELECTRIC:LEM-ASA-FineHeater:PWMCYC");
	
	//sprintf(oapiDebugString(), "Cabin T: %.4f Suit T: %.4f Loop T: %.4f ASA T: %.4f ASASelfHEat W: %.4f FastHtr: %1f FineHtr: %1f FineHtrPWM: %.4f IMUHtr: %1f IMU T: %.4f", KelvinToFahrenheit(*CabinTemp), KelvinToFahrenheit(*hxheatingTemp), 
		//KelvinToFahrenheit(*primloop1temp), KelvinToFahrenheit(*ASARad), *ASASelfHeat, *ASAFastHtr, *ASAFineHtr, *ASAFineHtrPWM, *IMUHtr, KelvinToFahrenheit(*IMURad));
	//sprintf(oapiDebugString(), "CFM T: %lf HX: %lf Out: %lf", KelvinToFahrenheit(*CabinFanManifoldTemp), *CabinFanManifoldHX, *CabinFanManifoldOutFlow* LBH);

	//sprintf(oapiDebugString(), "B1T %.3f B2T %.3f B3T %.3f B4T %.3f DGT %.3f B5T %.3f B6T %.3f B5PT %.3f B6PT %.3f AGT %.3f EDA %.3f EDB %.3f", KelvinToFahrenheit(*bat1temp), KelvinToFahrenheit(*bat2temp), KelvinToFahrenheit(*bat3temp), KelvinToFahrenheit(*bat4temp), KelvinToFahrenheit(*desbatglycoltemp), KelvinToFahrenheit(*bat5temp), KelvinToFahrenheit(*bat6temp), KelvinToFahrenheit(*bat5platetemp), KelvinToFahrenheit(*bat6platetemp), KelvinToFahrenheit(*ascbatglycoltemp), KelvinToFahrenheit(*edbatAtemp), KelvinToFahrenheit(*edbatBtemp));
	//sprintf(oapiDebugString(), "B5T %.3f B6T %.3f B5PT %.3f B6PT %.3f AGT %.3f EDB %.3f", KelvinToFahrenheit(*bat5temp), KelvinToFahrenheit(*bat6temp), KelvinToFahrenheit(*bat5platetemp), KelvinToFahrenheit(*bat6platetemp), KelvinToFahrenheit(*ascbatglycoltemp), KelvinToFahrenheit(*edbatBtemp));

	//sprintf(oapiDebugString(), "Primary Glycol Heat 1 %lf Primary Glycol Heat 2 %lf Cabin Heat %lf Battery Heat %lf", (*LGCHeat + *CDUHeat + *PSAHeat + *TLEHeat + *GASTAHeat + *LCAHeat + *DSEHeat + *ASAHeat + *PTAHeat + *IMUHeat + *RGAHeat), (*SBPHeat + *SBXHeat + *SPHeat + *AEAHeat + *ATCAHeat + *SCERAHeat + *CWEAHeat + *RREHeat + *VHFHeat + *INVHeat + *ECAHeat + *PCMHeat), *CabinHeat, (*bat1heat+*bat2heat+*bat3heat+*bat4heat+*bat5heat+*bat6heat+*edbatAheat+*edbatBheat));

	//sprintf(oapiDebugString(), "SCT %.4f SCF %.4f CT %.4f SGDF %.4f SGDT %.4f GRVF %.4f CO2MT %.4f GlyCT %.4f HXCT %.4f HXCPwr %.4f GlyHT %.4f HXHT %.4f HXHPwr %.4f", KelvinToFahrenheit(*SuitCircuitTemp), *SuitCircuitOutFlow*LBH, KelvinToFahrenheit(*CabinTemp), *suitGasDiverterCabinFlow*LBH, KelvinToFahrenheit(*SGDTemp), *cabinGasReturnFlow*LBH, KelvinToFahrenheit(*CO2ManifoldTemp), KelvinToFahrenheit(*glycolsuitcooltemp), KelvinToFahrenheit(*hxcoolingTemp), *hxcoolingPower, KelvinToFahrenheit(*glycolsuitheattemp), KelvinToFahrenheit(*hxheatingTemp), *hxheatingPower);
	//sprintf(oapiDebugString(), "SGDP %lf SGDV %d SGDF %lf CabP %lf GRV %d GRVF %lf CO2MP %lf", *SGDPress* PSI, *suitGasDiverterCabinVLV, *suitGasDiverterCabinFlow*LBH, *CabinPress* PSI, *gasreturnvlv, *cabinGasReturnFlow*LBH, *CO2ManifoldPress* PSI);

	//sprintf(oapiDebugString(), "LGC %.2f CDU %.2f PSA %.2f GAS %.2f LCA %.2f DSE %.2f TLE %.2f ASA %.2f PTA %.2f IMU %.2f RGA %.2f CT %.2f CR %.2f HX1 %.2f HX2 %.2f", KelvinToFahrenheit(*LGCRad), KelvinToFahrenheit(*CDURad), KelvinToFahrenheit(*PSARad), KelvinToFahrenheit(*GASTARad), KelvinToFahrenheit(*LCARad), KelvinToFahrenheit(*DSERad), KelvinToFahrenheit(*TLERad), KelvinToFahrenheit(*ASARad), KelvinToFahrenheit(*PTARad), KelvinToFahrenheit(*IMURad), KelvinToFahrenheit(*RGARad), KelvinToFahrenheit(*CabinTemp), KelvinToFahrenheit(*CabinPlate), *CabinHX1, *CabinHX2);
	//sprintf(oapiDebugString(), "SBD %lf AEAVHF %lf ATCAINV %lf SCERA %lf CWEAPCM %lf RRE %lf SCERAECA %lf P/S %lf Pwr1:%lf Pwr2:%lf", KelvinToFahrenheit(*SBDRad), KelvinToFahrenheit(*AEAVHFRad), KelvinToFahrenheit(*ATCAINVRad), KelvinToFahrenheit(*SCERARad), KelvinToFahrenheit(*CWEAPCMRad), KelvinToFahrenheit(*RRERad), KelvinToFahrenheit(*SCERAECARad), KelvinToFahrenheit(*PRIMSECRad), *PRIMSECHX1, *PRIMSECHX2);
	//sprintf(oapiDebugString(), "RR T: %.3f RRE T: %.3f", KelvinToFahrenheit(*RRTemp), KelvinToFahrenheit(*RRERad));

	//sprintf(oapiDebugString(), "CabinP %lf CabinT %lf CabinQ %lf CabinHeat %lf CabinPlate %lf HX %lf", ecs.GetCabinPressurePSI(), ecs.GetCabinTempF(), *CabinEnergy, *CabinHeat, KelvinToFahrenheit(*CabinPlate), *CabinHX1);
	//sprintf(oapiDebugString(), "LM Cabin: %lf LM Tunnel: %lf", *lmcabinpress*PSI, *lmtunnelpress*PSI);
	//sprintf(oapiDebugString(), "Quad 1 %lf Quad 2 %lf Quad 3 %lf Quad 4 %lf", KelvinToFahrenheit(*QD1Temp), KelvinToFahrenheit(*QD2Temp), KelvinToFahrenheit(*QD3Temp), KelvinToFahrenheit(*QD4Temp));
	//sprintf(oapiDebugString(), "PrimGlycolQty %lf SecGlycolQty %lf", ecs.GetPrimaryGlycolQuantity(), ecs.GetSecondaryGlycolQuantity());

	//sprintf(oapiDebugString(), "GlyCTmp %lf HXCTmp %lf HXCPwr %lf GlyHTmp %lf HXHTmp %lf HXHPwr %lf CT %lf LT %lf", KelvinToFahrenheit(*glycolsuitcooltemp), KelvinToFahrenheit(*hxcoolingTemp), *hxcoolingPower, KelvinToFahrenheit(*glycolsuitheattemp), KelvinToFahrenheit(*hxheatingTemp), *hxheatingPower, KelvinToFahrenheit(*cdrsuittemp), KelvinToFahrenheit(*lmpsuittemp));
	//sprintf(oapiDebugString(), "CO2CP %lf SFMP %lf CO2F %lf CO2REM %lf WS1F %lf H2OREM %lf SC Mass: %lf", *primCO2CanisterPress*PSI, *suitfanmanifoldoutPress*PSI, *primCO2Flow, *primCO2Removal, *WS1Flow, *WS1H2ORemoval, (*hxheatingMass + *cdrsuitmass + *lmpsuitmass + *SuitCircuitMass + *SGDMass + *CO2ManifoldMass + *primCO2CanisterMass + *secCO2CanisterMass + *suitfanmanifoldMass + *hxcoolingMass));
	//sprintf(oapiDebugString(), "Total: %lf HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf RV %d RVF %lf", (*hxheatingMass + *cdrsuitmass + *lmpsuitmass + *SuitCircuitMass + *SGDMass + *CO2ManifoldMass + *primCO2CanisterMass + *secCO2CanisterMass + *suitfanmanifoldoutMass + *hxcoolingMass), *hxheatingMass, *cdrsuitmass, *lmpsuitmass, *SuitCircuitMass, *SGDMass, *CO2ManifoldMass, *primCO2CanisterMass, *suitfanmanifoldMass, *hxcoolingMass, *suitReliefvlv, *suitReliefflow*LBH);
	//sprintf(oapiDebugString(), "HXH %lf CS %lf LS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf CO2F %lf CO2REM %lf WS1F %lf H2OREM %lf", *hxheatingPress*PSI, *cdrsuitpress*PSI, *lmpsuitpress*PSI, *SuitCircuitPress*PSI, *SGDPress*PSI, *CO2ManifoldPress*PSI, *primCO2CanisterPress*PSI, *suitfanmanifoldoutPress*PSI, *hxcoolingPress*PSI, *primCO2Flow, *primCO2Removal, *WS1Flow, *WS1H2ORemoval);
	//sprintf(oapiDebugString(), "CAB %lf SUIT %lf OVHDFLOW %lf OVHDFLOWMAX %lf OVHDSIZE %f TUNNELPRESS %lf TUNNELFLOW %lf", ecs.GetCabinPressurePSI(), (*SuitCircuitPress)*PSI, *ovhdHatchFlow*LBH, *ovhdHatchFlowmax*LBH, *ovhdHatchSize, *lmtunnelpress*PSI, *lmtunnelflow*LBH);

	///sprintf(oapiDebugString(), "HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFI %lf SFO %lf HXC %lf WSI %lf WSO %lf SFF %lf CDRF %lf LMPF %lf", *hxheatingPress* PSI, *cdrsuitpress* PSI, *lmpsuitpress* PSI, *SuitCircuitPress* PSI, *SGDPress* PSI, *CO2ManifoldPress* PSI, *primCO2CanisterPress* PSI, *suitfanmanifoldinPress* PSI, *suitfanmanifoldoutPress* PSI, *hxcoolingPress* PSI, *WSMinPress* PSI, *WSMoutPress* PSI, *SuitFanOutFlow*LBH, *CDRSuitFlow *LBH, *LMPSuitFlow*LBH);
	sprintf(oapiDebugString(), "HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFI %lf SFO %lf HXC %lf WSI %lf WSO %lf SFF %lf CDRF %lf LMPF %lf", KelvinToFahrenheit(*hxheatingTemp) , KelvinToFahrenheit(*cdrsuittemp), KelvinToFahrenheit(*lmpsuittemp), KelvinToFahrenheit(*SuitCircuitTemp), KelvinToFahrenheit(*SGDTemp), KelvinToFahrenheit(*CO2ManifoldTemp), KelvinToFahrenheit(*primCO2CanisterTemp), KelvinToFahrenheit(*suitfanmanifoldinTemp), KelvinToFahrenheit(*suitfanmanifoldoutTemp), KelvinToFahrenheit(*hxcoolingTemp), KelvinToFahrenheit(*WSMinTemp), KelvinToFahrenheit(*WSMoutTemp), *SuitFanOutFlow*LBH, *CDRSuitFlow *LBH, *LMPSuitFlow*LBH);
	//sprintf(oapiDebugString(), "HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf CO2F %lf CO2REM %lf GRV %d", *hxheatingPress*PSI, *cdrsuitpress*PSI, *lmpsuitpress*PSI, *SuitCircuitPress*PSI, *SGDPress*PSI, *CO2ManifoldPress*PSI, *primCO2CanisterPress*PSI, *suitfanmanifoldoutPress*PSI, *hxcoolingPress*PSI, *primCO2Flow, *primCO2Removal, *gasreturnvlv);
	//sprintf(oapiDebugString(), "CAB %lf RVF %lf RVFM %lf HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf", *CabinMass, *suitReliefflow, *suitReliefflowmax, *hxheatingMass, *cdrsuitmass, *lmpsuitmass, *SuitCircuitMass, *SGDMass, *CO2ManifoldMass, *primCO2Mass, *suitfanmanifoldoutMass, *hxcoolingMass);
	//sprintf(oapiDebugString(), "Diverter: Flow to Cabin %lf, Flow to Co2Manifold %lf", *suitGasDiverterCabinFlow * LBH, *suitGasDiverterCO2ManifoldFlow * LBH);

	//sprintf(oapiDebugString(), "CAB %lf RVF %lf HXH %lf CDRS %lf LMPS %lf SC %lf SGD %lf CO2M %lf PCO2 %lf SFM %lf HXC %lf", *CabinMass, *suitReliefflow, *hxheatingPress*PSI, *cdrsuitpress*PSI, *lmpsuitpress*PSI, *SuitCircuitPress*PSI, *SGDPress*PSI, *CO2ManifoldPress*PSI, *primCO2CanisterPress*PSI, *suitfanmanifoldoutPress*PSI, *hxcoolingPress*PSI);

	//sprintf(oapiDebugString(), "SBD: T %lf H %lf RR: T %lf SH %lf H %lf LR: T %lf H %lf", KelvinToFahrenheit(*SBDTemp), *SBDHtr, KelvinToFahrenheit(*RRTemp), *RRStbyHtr, *RRHtr, KelvinToFahrenheit(*LRTemp), *LRHtr);

	//sprintf(oapiDebugString(), "Sen %lf RM %lf HXH %lf CDR %lf LMP %lf SGD %lf SC %lf CAB %lf CAN %lf PRIM %lf SEC %lf SF %lf HXC %lf", ecs.GetSensorCO2MMHg(), *primCO2Removal, *SuitHXHCO2*MMHG, *CDRSuitCO2*MMHG, *LMPSuitCO2*MMHG, *SuitGasDiverterCO2*MMHG, *SuitCircuitCO2*MMHG, *CabinCO2*MMHG, *CanisterMFCO2*MMHG, *PrimCO2*MMHG, *SecCO2*MMHG, *SuitFanCO2*MMHG, *SuitHXCCO2*MMHG);
	//sprintf(oapiDebugString(), "GRV %d CO2MP %lf PCO2P %1f SFMP %lf SHXCP %lf SHXHP %lf CO2F %lf CO2REM %lf WS1F %lf H2OTM %lf H2OTOF %lf", *gasreturnvlv, (*CO2ManifoldPress)*PSI, (*primCO2CanisterPress)*PSI, (*suitfanmanifoldoutPress)*PSI, (*hxcoolingPress)*PSI, (*hxheatingPress)*PSI, *primCO2Flow, *primCO2Removal, *WS1Flow, (*STMass)*LBS, (*STPress)*PSI, *SToutflow);

	//sprintf(oapiDebugString(), "AcM %lf FMM %lf L1M %lf ABCM %lf L2M %lf EIM %lf EOM %lf Flow1 %lf Flow2 %lf", *secglycolmass, *secpumpmanifoldmass, *secloop1mass, *secascbatmass, *secloop2mass, *secevapinmass, *secevapoutmass, *secGlyReg1Flow*LBH, *secGlyReg2Flow*LBH);
	//sprintf(oapiDebugString(), "AcP %lf FMP %lf L1P %lf ABCP %lf L2P %lf EIP %lf EOP %lf Flow1 %lf Flow2 %lf", *secglycolpress*PSI, *secpumpmanifoldpress*PSI, *secloop1press*PSI, *secascbatpress*PSI, *secloop2press*PSI, *secevapinpress*PSI, *secevapoutpress*PSI, *secGlyReg1Flow*LBH, *secGlyReg2Flow*LBH);
	//sprintf(oapiDebugString(), "AcT %lf FMT %lf L1T %lf ABCT %lf L2T %lf ETI %lf ETO %lf SCT %lf SETh %lf SCTh %lf", *secglycoltemp* 1.8 - 459.67, *secpumpmanifoldtemp* 1.8 - 459.67, *secloop1temp* 1.8 - 459.67, *secascbattemp* 1.8 - 459.67, *secloop2temp* 1.8 - 459.67, *secevaptempin* 1.8 - 459.67, *secevaptempout* 1.8 - 459.67, *hxcoolingTemp* 1.8 - 459.67, *secevapThrottle, *slevapThrottle);
	
	//sprintf(oapiDebugString(), "AP %lf GP %lf PMP %lf HXCP %lf L1P %lf HXLP %lf L2P %lf HXHP %lf EIP %lf EOP %lf ACP %lf DBP %lf", *primglycolaccumpress* PSI, *primglycolpress*PSI, *glycolpumpmanifoldpress*PSI, *glycolsuitcoolpress*PSI, *primloop1press*PSI, *waterglycolhxpress*PSI, *primloop2press*PSI, *glycolsuitheatpress*PSI, *primevapinpress*PSI, *primevapoutpress*PSI, *ascbatglycolpress*PSI, *desbatglycolpress*PSI);
	//sprintf(oapiDebugString(), "AM %lf HXCM %lf L1M %lf HXLM %lf L2M %lf HXHM %lf EIM %lf EOM %lf ACM %lf DBM %lf", *primglycolmass, *glycolsuitcoolmass, *primloop1mass, *waterglycolhxmass, *primloop2mass, *glycolsuitheatmass, *primevapinmass, *primevapoutmass, *ascbatglycolmass, *desbatglycolmass);
	//sprintf(oapiDebugString(), "P1 %lf P2 %lf Reg1 %lf WGHX %lf SHX %lf SHXBP %lf", *Pump1OutFlow*LBH, *Pump2OutFlow*LBH, *primGlyReg1Flow*LBH, *waterGlyHXFlow*LBH, *suitHXGlyFlow*LBH, *suitHXGlyBypassFlow*LBH);
	//sprintf(oapiDebugString(), "AT %.3f PT %.3f GCT %.3f SCT %.3f HXCP %.3f L1 %.3f HXT %.3f L2 %.3f GHT %.3f SHT %.3f HXHP %.3f EI %.3f EO %.3f A %.3f D %.3f", KelvinToFahrenheit(*primglycoltemp), KelvinToFahrenheit(*glycolpumpmanifoldtemp), KelvinToFahrenheit(*glycolsuitcooltemp), KelvinToFahrenheit(*hxcoolingTemp), *hxcoolingPower, KelvinToFahrenheit(*primloop1temp), KelvinToFahrenheit(*waterglycolhxtemp), KelvinToFahrenheit(*primloop2temp), KelvinToFahrenheit(*glycolsuitheattemp), KelvinToFahrenheit(*hxheatingTemp), *hxheatingPower, KelvinToFahrenheit(*primevaptempin), KelvinToFahrenheit(*primevaptempout), KelvinToFahrenheit(*ascbatglycoltemp), KelvinToFahrenheit(*desbatglycoltemp));
	
	//sprintf(oapiDebugString(), "Cabin: %.3f�F Cabin Structure: %.3f�F Primary Glycol Loop %.3f�F ASA: %.3f�F ASAPWM %lf", KelvinToFahrenheit(*CabinTemp), KelvinToFahrenheit(*CabinStructureTemp), KelvinToFahrenheit(*primloop2temp), KelvinToFahrenheit(*ASARad), *ASAFineHtrPWM);
																											
	//sprintf(oapiDebugString(), "LCG %lf SEC %lf", LCGPump->Voltage(), SecGlyPump->Voltage());
	//sprintf(oapiDebugString(), "CM %lf CP %lf CT %lf CE %lf LM %lf LP %lf LT %lf LE %lf", *cdrsuitmass, (*cdrsuitpress)*PSI, (*cdrsuittemp)* 1.8 - 459.67, *cdrsuitenergy, *lmpsuitmass, (*lmpsuitpress)*PSI, (*lmpsuittemp)* 1.8 - 459.67, *lmpsuitenergy);
	//sprintf(oapiDebugString(), "PRAQ %lf PRAP %lf PRAT %lf PRBQ %lf PRBP %lf PRBT %lf", *PressRegAMass, (*PressRegAPress)*PSI, (*PressRegATemp)* 1.8 - 459.67, *PressRegBMass, (*PressRegBPress)*PSI, (*PressRegBTemp)* 1.8 - 459.67);
	//sprintf(oapiDebugString(), "GlyTmp %lf GlySuitCoolTmp %lf HXCTmp %lf HXHTmp %lf StTmp %lf CP %lf CT %lf CE %lf LP %lf LT %lf LE %lf", KelvinToFahrenheit(*primglycoltemp), KelvinToFahrenheit(*glycolsuitcooltemp), KelvinToFahrenheit(*hxcoolingTemp), KelvinToFahrenheit(*hxheatingTemp), KelvinToFahrenheit(*SuitCircuitTemp), (*cdrsuitpress)*PSI, KelvinToFahrenheit(*cdrsuittemp), *cdrsuitenergy, (*lmpsuitpress)*PSI, KelvinToFahrenheit(*lmpsuittemp), *lmpsuitenergy);
	//sprintf(oapiDebugString(), "LCGHXT %lf LCGT %lf CDRT %lf LMPT %lf WGHXT %lf HXFlow %lf BPFlow %lf HXFM %lf BPFM %lf", KelvinToFahrenheit(*lcghxtemp), KelvinToFahrenheit(*lcgtemp), KelvinToFahrenheit(*cdrsuittemp), KelvinToFahrenheit(*lmpsuittemp), KelvinToFahrenheit(*waterglycolhxtemp), *lcghxflow, *lcgaccflow, *lcghxflowmax, *lcgaccflowmax);
	//sprintf(oapiDebugString(), "LCGAM %lf LCGAP %lf LCGAT %lf LCGHXM %lf LCGHXP %lf LCGHXT %lf LCGM %lf LCGP %lf LCGT %lf WGHXT %lf HXF %lf BPF %lf", *lcgaccumass, *lcgaccupress*PSI, *lcgaccutemp* 1.8 - 459.67, *lcghxmass, *lcghxpress*PSI, *lcghxtemp* 1.8 - 459.67, *lcgmass, *lcgpress*PSI, *lcgtemp* 1.8 - 459.67, *waterglycolhxtemp* 1.8 - 459.67, *lcghxflow, *lcgaccflow);
	//sprintf(oapiDebugString(), "Press %lf Loop1Mass %lf Evap Mass %lf Loop1Temp %lf Loop2Temp %lf EvapInTemp %lf EvapOutTemp %lf, EPump %d EValve %d EThrot %lf ESteam %lf", ecs.GetSecondaryGlycolPressure(), *secloop1mass, *secevapmassout, *secloop1temp, *secloop2temp, *secevaptempin, *secevaptempout, *secevapPump, *secevapValve, *secevapThrottle, (*secevapSteam)*PSI);
	//sprintf(oapiDebugString(), "WB Press %lf Loop1Temp %lf Loop2Temp %lf EvapInTemp %lf EvapOutTemp %lf, EPump %d EValve %d EThrot %lf ESteam %lf", (*primwbpress)*PSI, *primloop1temp, *primloop2temp, *primevaptempin, *primevaptempout, *primevapPump, *primevapValve, *primevapThrottle, (*primevapSteam)*PSI);
	//sprintf(oapiDebugString(), "DH2O:M %lf WTS:M %lf ST:M %lf PR:M %lf vlv %d WB:M %lf Loop1 %lf EVAPin %lf EVAPout %lf CaseGly %lf EThrot %lf ESteam %lf", *DesH2OMass, *WTSMass, *STMass, *primregmass, *primevap1vlv, *primwbmass, *primevaptempin, *primevaptempout, *primloop1temp, *primevapThrottle, *primevapSteam*PSI);
	//sprintf(oapiDebugString(), "PCO2F %lf SFMP %lf HXCP %lf HXHP %lf WSF %lf WSFM %lf", *primCO2Flow, *suitfanmanifoldPress*PSI, *hxcoolingPress*PSI, *hxheatingPress*PSI, *WS1Flow, *WS1FlowMax);
	//sprintf(oapiDebugString(), "GTemp %lf GPress %lf GMass %lf EPump %d EValve %lf EThrot %lf ESteam %lf", ecs.GetPrimaryGlycolTempF(), ecs.GetPrimaryGlycolPressure(), *primglycolmass, *primevapPump, *primevapValve, *primevapThrottle, *primevapSteam*PSI);
	
	//sprintf(oapiDebugString(), "SCT %lf SCM %1f SCP %lf HXHM %1f HXHP %lf SGDM %1f SGDP %lf HXCM %1f HXCP %lf", ecs.GetSuitTempF(), *SuitCircuitMass, ecs.GetSuitPressurePSI(), *hxheatingMass, (*hxheatingPress)*PSI, *SGDMass, (*SGDPress)*PSI, *hxcoolingMass, (*hxcoolingPress)*PSI);
	//sprintf(oapiDebugString(), "BDF %lf RVF %lf CabinP %lf CabinT %lf SuitP %lf SuitT %lf", *desO2burstflow, *desO2reliefflow, ecs.GetCabinPressurePSI(), ecs.GetCabinTempF(), ecs.GetSuitPressurePSI(), ecs.GetSuitTempF());
	//sprintf(oapiDebugString(), "PRAQ %lf PRAP %lf PRAT %lf PRBQ %lf PRBP %lf PRBT %lf", *PressRegAMass, (*PressRegAPress)*PSI, *PressRegATemp, *PressRegBMass, (*PressRegBPress)*PSI, *PressRegBTemp);
	//sprintf(oapiDebugString(), "DO2Q %lf DO2P %lf DO2TT %lf DO2MQ %lf DO2MP %lf DO2MT %lf O2MQ %lf O2MP %lf O2MT %lf", ecs.DescentOxyTankQuantity(), ecs.DescentOxyTankPressurePSI(), *DESO2TankTemp, *DESO2ManifoldMass, (*DESO2ManifoldPress)*PSI, *O2ManifoldTemp, *O2ManifoldMass, (*O2ManifoldPress)*PSI, *O2ManifoldTemp);

	//sprintf(oapiDebugString(), "CO2 MP %lf PRIM CO2 %lf SEC CO2 %lf CAB %lf PV %d PF %lf SV %d", (*CO2ManifoldPress)*PSI, (*primCO2CanisterPress)*PSI, (*secCO2CanisterPress)*PSI, (*CabinPress)*PSI, *primCO2Vent, *primCO2Flow, *secCO2Vent);
	//sprintf(oapiDebugString(), "CAB %lf SUIT %lf VLV %d SRFLOW %lf SRFLOWMAX %lf FWDFLOW %lf FWDFLOWMAX %lf OVHDFLOW %lf OVHDFLOWMAX %lf TUNNELPRESS %lf TUNNELFLOW %lf", ecs.GetCabinPressurePSI(), (*SuitCircuitPress)*PSI, *suitReliefvlv, *suitReliefflow*LBH, *suitReliefflowmax*LBH, *fwdHatchFlow*LBH, *fwdHatchFlowmax*LBH, *ovhdHatchFlow*LBH, *ovhdHatchFlowmax*LBH, *lmtunnelpress*PSI, *lmtunnelflow*LBH);
	//sprintf(oapiDebugString(), "CabinP %lf CabinT %lf SuitP %lf SuitT %lf", ecs.GetCabinPressurePSI(), ecs.GetCabinTempF(), ecs.GetSuitPressurePSI(), ecs.GetSuitTempF());
	
	//sprintf(oapiDebugString(), "DO2Q %lf DO2P %lf DO2T %lf DO2VM %lf DO2E %lf DO2PP %lf", ecs.DescentOxyTankQuantity(), ecs.DescentOxyTankPressurePSI(), *DESO2TankTemp, *DESO2VapMass, *DESO2Energy, (*DESO2PP*PSI));
	//sprintf(oapiDebugString(), "DO2TP %lf DO2MP %lf O2MP %lf PREGA %lf SUITP %lf", ecs.DescentOxyTankPressurePSI(), (*DESO2ManifoldPress*PSI), (*O2ManifoldPress*PSI), (*PressRegAPress*PSI), ecs.GetSuitPressurePSI());
	//sprintf(oapiDebugString(), "DO2TP %lf DO2TM %lf DO2MP %lf DO2MM %lf O2MP %lf O2MM %lf DESO2 %d ASCO21 %d ASCO22 %d", ecs.DescentOxyTankPressurePSI(), ecs.DescentOxyTankQuantity(), (*DESO2ManifoldPress*PSI), *DESO2ManifoldMass, (*O2ManifoldPress*PSI), *O2ManifoldMass, *deso2manifoldoutvlv, *asco2out1vlv, *asco2out2vlv);
	

	/*
	double CDRAmps=0,LMPAmps=0;
	double CDRVolts = CDRs28VBus.Voltage(),LMPVolts = LMPs28VBus.Voltage();
	if(LMPVolts > 0){ LMPAmps = LMPs28VBus.PowerLoad()/LMPVolts; }
	if(CDRVolts > 0){ CDRAmps = CDRs28VBus.PowerLoad()/CDRVolts; }	
	sprintf(oapiDebugString(),"LM: LMP %f V/%f A CDR %f V/%f A | AC-A %f V AC-B %f V",LMPVolts,LMPAmps,
		CDRVolts,CDRAmps,ACBusA.Voltage(), ACBusB.Voltage());
	*/

}

void LEM::SetPipeMaxFlow(char *pipe, double flow) {

	h_Pipe *p = (h_Pipe *)Panelsdk.GetPointerByString(pipe);
	p->flowMax = flow;
}

h_Pipe* LEM::GetLMTunnelPipe()
{
	return (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNELUNDOCKED");
}

h_Valve* LEM::GetCSMO2HoseOutlet()
{
	return (h_Valve *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:IN");
}

void LEM::ConnectTunnelToCabinVent()
{
	h_Pipe *pipe = (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNELUNDOCKED");
	h_Tank *tank = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNEL");

	pipe->in = &tank->OUT_valve;
}

double LEM::GetRCSQuadTempF(int index)
{
	if (index >= LM_RCS_QUAD_1 && index <= LM_RCS_QUAD_4)
	{
		h_Radiator *quad = 0;
		switch (index)
		{
		case LM_RCS_QUAD_1:
			quad = LMQuad1RCS;
			break;

		case LM_RCS_QUAD_2:
			quad = LMQuad2RCS;
			break;

		case LM_RCS_QUAD_3:
			quad = LMQuad3RCS;
			break;

		case LM_RCS_QUAD_4:
			quad = LMQuad4RCS;
			break;
		}
		return (KelvinToFahrenheit(quad->GetTemp()));
	}
		return 0;
}



//
// ECS state for the ProjectApolloMFD.
//

void LEM::GetECSStatus(LEMECSStatus &ecs)

{
	// Crew

	if (EVA_IP[0])
	{
		ecs.cdrStatus = 2;
	}
	else if (CDRinPLSS > 1)
	{
		ecs.cdrStatus = 3;
	}
	else if (CDRSuited->number == 1)
	{
		ecs.cdrStatus = 1;
	}
	else
	{
		ecs.cdrStatus = 0;
	}

	if (EVA_IP[1])
	{
		ecs.lmpStatus = 2;
	}
	else if (LMPinPLSS > 1)
	{
		ecs.lmpStatus = 3;
	}
	else if (LMPSuited->number == 1)
	{
		ecs.lmpStatus = 1;
	}
	else
	{
		ecs.lmpStatus = 0;
	}

	ecs.crewNumber = CrewInCabin->number + CDRSuited->number + LMPSuited->number;
	ecs.crewStatus = CrewStatus.GetStatus();;


}

void LEM::SetCrewNumber(int number)
{
	int crewsuited = CDRSuited->number + LMPSuited->number;

	if (number + crewsuited + (EVA_IP[0] ? 1 : 0) + (EVA_IP[1] ? 1 : 0) <= 3)
	{
		CrewInCabin->number = number;
	}
}

void LEM::SetCDRInSuit()
{
	if (EVA_IP[0]) return;

	if (CDRinPLSS == 2) {
		CDRSuited->number = 1;
		CDRinPLSS = 0;
	}
	else if (CDRinPLSS == 1)
	{
		CDRSuited->number = 0;
		CDRinPLSS = 2;
	}
	else if (CrewInCabin->number >= 1 && CDRSuited->number == 0)
	{
		CrewInCabin->number--;
		CDRSuited->number = 1;
		CDRinPLSS = 1;
	}
	else if (CDRSuited->number == 1)
	{
		CrewInCabin->number++;
		CDRSuited->number = 0;
	}
	SetCrewMesh();
}

void LEM::SetLMPInSuit()
{
	if (EVA_IP[1]) return;

	if (LMPinPLSS == 2) {
		LMPSuited->number = 1;
		LMPinPLSS = 0;
	}
	else if (LMPinPLSS == 1)
	{
		LMPSuited->number = 0;
		LMPinPLSS = 2;
	}
	else if (CrewInCabin->number >= 1 && LMPSuited->number == 0)
	{
		CrewInCabin->number--;
		LMPSuited->number = 1;
		LMPinPLSS = 1;
	}
	else if (LMPSuited->number == 1)
	{
		CrewInCabin->number++;
		LMPSuited->number = 0;
	}
	SetCrewMesh();
}

void LEM::StartEVA()
{
	if (ForwardHatch.IsOpen() && GroundContact()) {
		ToggleEva = true;
	}
}

void LEM::CheckDescentStageSystems()
{
	rjb.CheckStatus();
	drb.CheckStatus();
	if (stage < 2) {

	}
	else {
		Battery1->Disable();
		Battery2->Disable();
		Battery3->Disable();
		Battery4->Disable();
		EDBatteryA->Disable();

		//ECS

		DesO2Tank->space.Void();
		DesO2Manifold->space.Void();
		DesO2Manifold->IN_valve.Close();

		DesH2OTank->space.Void();
		DesH2OTank->IN_valve.Close();
		DesH2OTank->OUT_valve.Close();
		DesH2OTank->OUT2_valve.Close();

		SetPipeMaxFlow("HYDRAULIC:ASCBATINLET", 290.0 / LBH);
		DesBatCooling->space.Void();
		DesBatCooling->IN_valve.Close();
		DesBatCooling->OUT_valve.Close();

		//DES HX

		h_HeatExchanger* desbat1HX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT1HX");
		h_HeatExchanger* desbat2HX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT2HX");
		h_HeatExchanger* desbat3HX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT3HX");
		h_HeatExchanger* desbat4HX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:DESBAT4HX");
		h_HeatExchanger* desplate1HX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATLeftPlateHX");
		h_HeatExchanger* desplate2HX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:DESBATRightPlateHX");
		desbat1HX->SetPumpOff();
		desbat2HX->SetPumpOff();
		desbat3HX->SetPumpOff();
		desbat4HX->SetPumpOff();
		desplate1HX->SetPumpOff();
		desplate2HX->SetPumpOff();

		h_HeatExchanger* edbatAHX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:EDBATAHX");
		edbatAHX->SetPumpOff();

		//h_HeatExchanger* staybatHX = (h_HeatExchanger*)Panelsdk.GetPointerByString("HYDRAULIC:STAYBATHX");
		//staybatHX->SetPumpOff();

		//LR

		Boiler* lrheater = (Boiler*)Panelsdk.GetPointerByString("ELECTRIC:LEM-LR-Antenna-Heater");
		lrheater->SetPumpOff();
	}
}

void LEM::CreateMissionSpecificSystems()
{
	//
	// Pass on the mission number and realism setting to the AGC.
	//

	agc.SetMissionInfo(pMission->GetLGCVersion());
	aea.SetMissionInfo(pMission->GetAEAVersion());
	if (pMission->LMHasAscEngArmAssy())
	{
		aeaa = new LEM_AEAA();
	}
}

// SYSTEMS COMPONENTS

LEM_RadarTape::LEM_RadarTape()
{
	lem = NULL;
	ac_source = NULL;
	dc_source = NULL;
	reqRange = 0;
	reqRate = 0;
	dispRange = 8321;	//Initializes range rate display to zero per LM closeout images
	dispRate = 3759;	//Initializes range rate display to a non zero number, in this case -240, per LM closeout images
	lgc_alt = 0;
	lgc_altrate = 0;
	ags_alt = 0;
	ags_altrate = 0;
	desRange = 0;
	desRate = 0;
	LGCaltUpdateTime = 0;
	LGCaltRateUpdateTime = 0;
	AGSaltUpdateTime = 0;
	AGSaltRateUpdateTime = 0;
}

void LEM_RadarTape::Init(LEM* s, e_object* dc_src, e_object* ac_src, SURFHANDLE surf1, SURFHANDLE surf2) {
	lem = s;
	dc_source = dc_src;
	ac_source = ac_src;
	tape1 = surf1;
	tape2 = surf2;
}

// Power signal monitor - The power signal monitor provides an alarm light in the event of:
// (a) alternating current voltage below 85 volts 
// (b) direct current voltage below 20 volts 
// (c) input data loss

//FIXME: Need to investigate how the signals were interpreted, seems that near zero signal or zero signal generates the logic condition as noted by the light going on with rate <5 ft/s

bool LEM_RadarTape::PowerSignalMonOn()
{
	if ((dc_source->Voltage() > 2 || ac_source->Voltage() > SP_MIN_ACVOLTAGE) && (PowerFailure() == true || SignalFailure() == true || TimingFailure() == true)) { //Checks if DC power (2V to light the lamp) is present and the logic for power/signal/timing present
		return true;
	}
	return false;
}

bool LEM_RadarTape::PowerFailure()
{
	if (ac_source->Voltage() < 85.0 || dc_source->Voltage() < 20.0) { //Checks AC <85V and DC <20V
		return true;
	}
	return false;
}

bool LEM_RadarTape::SignalFailure()
{
	if (lem->AltRngMonSwitch.GetState() == TOGGLESWITCH_UP)
	{
		if (lem->RR.IsRangeDataGood() == false || lem->RR.IsFrequencyDataGood() == false || lem->RR.GetRadarRange() == 0.0 || abs(lem->RR.GetRadarRate()) < 1.524) //5 ft/s = 1.524 m/s
		{
			return true; //Needs to check rendezvous radar rate and range signals and return true if not present
		}
	}
	else {
		if (lem->ModeSelSwitch.IsUp()) // LR
		{
			if (lem->LR.IsRangeDataGood() == false || lem->LR.IsVelocityDataGood() == false || lem->LR.GetAltitude() == 0.0 || abs(lem->LR.GetAltitudeRate()) < 1.524) //5 ft/s = 1.524 m/s
			{
				return true; //Needs to check landing radar rate and range signals and return true if not present
			}
		}
		else if (lem->ModeSelSwitch.IsCenter()) //PGNS
		{
			if ((LGCaltUpdateTime + 1.0) < oapiGetSimTime() || (LGCaltRateUpdateTime + 1.0) < oapiGetSimTime() || lgc_alt == 0.0 || abs(lgc_altrate) < 1.524) //5 ft/s = 1.524 m/s
			{
				return true; //Needs to check LGC rate and range signals and return true if not present
			}
		}
		else //AGS
		{
			if ((AGSaltUpdateTime + 1.0) < oapiGetSimTime() || (AGSaltRateUpdateTime + 1.0) < oapiGetSimTime() || ags_alt == 0.0 || abs(ags_altrate) < 1.524) //5 ft/s = 1.524 m/s
			{
				return true; //Needs to check AGS rate and range signals and return true if not present
			}
		}
		return false;
	}
	return false;
}

bool LEM_RadarTape::TimingFailure()
{
	if (lem->PCM.TimingSignal() == false)
	{
		return true; //Needs to check for 512 KHz PCMTEA timing signal
	}
		return false;
}


void LEM_RadarTape::Timestep(double simdt) {
	
	if (!IsPowered())
	{
		return;
	}
	
	if( lem->AltRngMonSwitch.GetState()==TOGGLESWITCH_UP ) {
		setRange(lem->RR.GetRadarRange());
		setRate(lem->RR.GetRadarRate());
	} 
	else {
		if (lem->ModeSelSwitch.IsUp()) // LR
		{
			if (lem->LR.IsRangeDataGood())
			{
				setRange(lem->LR.GetAltitude());
			}
			else
			{
				setRange(0);
			}
			if (lem->LR.IsVelocityDataGood())
			{
				setRate(lem->LR.GetAltitudeRate());
			}
			/*else
			{
				setRate(0);
			}*/
		}
		else if (lem->ModeSelSwitch.IsCenter()) //PGNS
		{
			setRange(lgc_alt);
			setRate(lgc_altrate);
		}
		else //AGS
		{
			setRange(ags_alt);
			setRate(ags_altrate);
		}

	}
	// Altitude/Range
	if (reqRange < (1000.0 * 0.3048))
	{
		desRange = 6317.0 + 2086.0 - 82.0 - ((reqRange * 3.2808399) * 40.0 * 50.0 / 1000.0);
	}
	else if (reqRange < (120000.0 * 0.3048) )
	{
		desRange = 6443.0 - 82.0 - ((reqRange * 3.2808399) * 40.0 / 1000.0);
	}
	else
	{
		desRange = 81.0 + 1642.0 - 82.0 - ((reqRange * 0.000539956803*100.0)  * 40.0 / 1000.0);
	}

	TapeDrive(dispRange, desRange, 500.0, simdt);
	if (dispRange < 0)
	{
		dispRange = 0;
	}
	else if (dispRange > 8486)
	{
		dispRange = 8486;
	}

	//Altitude Rate/Range Rate

	//AOH Volume 2: "Range rate of 100 fps can mean rate of 100, 1100, 2100, etc., fps. Also, if rate is between 701 to 999, 1700 to 1999,
	//etc., fps, the display will read 700 fps and recycle to zero when rate becomes 1000, 2000, etc., fps.

	while (reqRate > 304.8)
	{
		reqRate -= 304.8;
	}
	while (reqRate < -304.8)
	{
		reqRate += 304.8;
	}

	desRate  = 2881.0 - 82.0 -  (reqRate * 3.2808399 * 40.0 * 100.0 / 1000.0);
	TapeDrive(dispRate, desRate, 500.0, simdt);
	if (dispRate < 0)
	{
		dispRate = 0;
	}
	else if (dispRate > 5599.0)
	{
		dispRate = 5599.0;
	}
}

void LEM_RadarTape::TapeDrive(double &Angle, double AngleCmd, double RateLimit, double simdt)
{
	double dposcmd, dpos;

	dposcmd = AngleCmd - Angle;

	if (abs(dposcmd) > RateLimit*simdt)
	{
		dpos = sign(AngleCmd - Angle)*RateLimit*simdt;
	}
	else
	{
		dpos = dposcmd;
	}
	Angle += dpos;
}

void LEM_RadarTape::SystemTimestep(double simdt) {
	if (!IsPowered())
		return;

	if (ac_source)
		ac_source->DrawPower(2.0);

	if (dc_source)
		dc_source->DrawPower(2.1);

	//sprintf(oapiDebugString(), "SimTime %1f LGC Alt Time %.5f LGC AltRate Time %.5f AGS Alt Time %.5f AGS AltRate Time %.5f", oapiGetSimTime(), LGCaltUpdateTime, LGCaltRateUpdateTime, AGSaltUpdateTime, AGSaltRateUpdateTime);
}

bool LEM_RadarTape::IsPowered()
{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE || ac_source->Voltage() < SP_MIN_ACVOLTAGE) {
		return false;
	}
	return true;
}

void LEM_RadarTape::SetLGCAltitude(int val) {

	int pulses;

	if (!IsPowered()) { return; }

	//if (val & 040000) { // Negative
	//	pulses = -((~val) & 077777);
	//}
	//else {
		pulses = val & 077777;
	//}

	lgc_alt = (2.345*0.3048*pulses);

	LGCaltUpdateTime = oapiGetSimTime();
}

void LEM_RadarTape::SetLGCAltitudeRate(int val) {

	int pulses;

	if (!IsPowered()) { return; }

	if (val & 040000) { // Negative
		pulses = val & 077777;
		pulses = 040000 - pulses;
	}
	else {
		pulses = val & 077777;
	}

	lgc_altrate = -(0.5*0.3048*pulses);

	LGCaltRateUpdateTime = oapiGetSimTime();
}

void LEM_RadarTape::AGSAltitudeAltitudeRate(int Data) {

	if (!IsPowered()) { return; }

		int DataVal;

		AGSChannelValue40 val = lem->aea.GetOutputChannel(IO_ODISCRETES);

		if (val[AGSAltitude] == 0)
		{
			DataVal = Data & 0777777;

			ags_alt = (double)DataVal * ALTSCALEFACTOR;

			AGSaltUpdateTime = oapiGetSimTime();
		}
		else if (val[AGSAltitudeRate] == 0)
		{
			if (Data & 0400000) { // Negative
				DataVal = -((~Data) & 0777777);
				DataVal = -0400000 - DataVal;
			}
			else {
				DataVal = Data & 0777777;
			}

			ags_altrate = -(double)DataVal * ALTRATESCALEFACTOR;

			AGSaltRateUpdateTime = oapiGetSimTime();
		}
}

void LEM_RadarTape::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);
	papiWriteScenario_double(scn, "RDRTAPE_RANGE", dispRange);
	oapiWriteScenario_float(scn, "RDRTAPE_RATE", dispRate);
	oapiWriteLine(scn, end_str);
}

void LEM_RadarTape::LoadState(FILEHANDLE scn,char *end_str){
	char *line;
	int value = 0;
	double lfValue = 0.0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "RDRTAPE_RANGE", 13)) {
			sscanf(line + 13, "%lf", &lfValue);
			dispRange = lfValue;
		}
		if (!strnicmp (line, "RDRTAPE_RATE", 12)) {
			sscanf(line + 12, "%d", &value);
			dispRate = value;
		}
	}
}

void LEM_RadarTape::RenderRange(SURFHANDLE surf) {
	if (dispRange > 6321.0)
	{
		oapiBlt(surf, tape2, 0, 0, 0, (int)(dispRange) - 6317, 43, 163, SURF_PREDEF_CK);
	}
	else
	{
		oapiBlt(surf, tape1, 0, 0, 0, (int)(dispRange), 43, 163, SURF_PREDEF_CK);
	}
}

void LEM_RadarTape::RenderRate(SURFHANDLE surf)
{
    oapiBlt(surf,tape1,0,0,42, (int)(dispRate) ,35,163, SURF_PREDEF_CK);
}

void LEM_RadarTape::RenderRangeVC(SURFHANDLE surf, SURFHANDLE surf1a, SURFHANDLE surf1b, SURFHANDLE surf2, int TexMul) {
	if (dispRange > 6321.0)
	{
		oapiBlt(surf, surf2, 0, 0, 0, (int)(dispRange)*TexMul-6317*TexMul, 43*TexMul, 163*TexMul, SURF_PREDEF_CK);
	}
	else if (dispRange > 3181.0)
	{
		oapiBlt(surf, surf1b, 0, 0, 0, (int)(dispRange)*TexMul-3026*TexMul, 43*TexMul, 163*TexMul, SURF_PREDEF_CK);
	}
	else
	{
		oapiBlt(surf, surf1a, 0, 0, 0, (int)(dispRange)*TexMul, 43*TexMul, 163*TexMul, SURF_PREDEF_CK);
	}
}

void LEM_RadarTape::RenderRateVC(SURFHANDLE surf, SURFHANDLE surf1a, SURFHANDLE surf1b, int TexMul)
{
	if (dispRate > 3181.0) {

		oapiBlt(surf, surf1b, 0, 0, 42*TexMul, (int)(dispRate)*TexMul - 3026*TexMul, 35*TexMul, 163*TexMul, SURF_PREDEF_CK);
	}
	else {
		oapiBlt(surf, surf1a, 0, 0, 42*TexMul, (int)(dispRate)*TexMul, 35*TexMul, 163*TexMul, SURF_PREDEF_CK);
	}
}

//Cross Pointer

CrossPointer::CrossPointer()
{
	lem = NULL;
	rateErrMonSw = NULL;
	scaleSwitch = NULL;
	dc_source = NULL;
	vel_x = 0;
	vel_y = 0;
	lgc_forward = 0;
	lgc_lateral = 0;
	anim_xpointerx = -1;
	anim_xpointery = -1;
	xvector = _V(0, 0, 0);
	yvector = _V(0, 0, 0);
	grpX = 0;
	grpY = 0;
	xtrans = ytrans = NULL;
}

CrossPointer::~CrossPointer()
{
	if (xtrans) delete xtrans;
	if (ytrans) delete ytrans;
}

void CrossPointer::Init(LEM *s, e_object *dc_src, ToggleSwitch *scaleSw, ToggleSwitch *rateErrMon)
{
	lem = s;
	dc_source = dc_src;
	scaleSwitch = scaleSw;
	rateErrMonSw = rateErrMon;
}

bool CrossPointer::IsPowered()
{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE) {
		return false;
	}
	return true;
}

void CrossPointer::SystemTimestep(double simdt)
{
	if (IsPowered() && dc_source)
		dc_source->DrawPower(8.0);  // take DC power
}

void CrossPointer::Timestep(double simdt)
{
	if (!IsPowered())
	{
		vel_x = 0;
		vel_y = 0;
		return;
	}

	if (rateErrMonSw->IsUp()) //Rendezvous Radar
	{
		if (lem->RR.IsPowered())
		{
			vel_x = lem->RR.GetRadarShaftVel()*1000.0;
			vel_y = lem->RR.GetRadarTrunnionVel()*1000.0;
		}
		else
		{
			vel_x = 0;
			vel_y = 0;
		}
	}
	else //Landing Radar, Computer
	{
		double vx = 0, vy = 0;

		if (lem->ModeSelSwitch.IsUp()) //Landing Radar
		{
			if (lem->LR.IsVelocityDataGood())
			{
				vx = lem->LR.rate[2] * 0.3048;
				vy = lem->LR.rate[1] * 0.3048;

				//Apollo 15 and later had this inversed
				if (lem->pMission->GetCrossPointerReversePolarity())
				{
					vy *= -1.0;
				}
			}
			else
			{
				vx = 0;
				vy = 0;
			}
		}
		else if (lem->ModeSelSwitch.IsCenter())	//PGNS
		{
			lgc_forward = 0.5571*(double)lem->scdu.GetAltOutput();
			lgc_lateral = 0.5571*(double)lem->tcdu.GetAltOutput();

			vx = lgc_forward*0.3048;
			vy = lgc_lateral*0.3048;

			//Apollo 15 and later had this inversed
			if (lem->pMission->GetCrossPointerReversePolarity())
			{
				vy *= -1.0;
			}
		}
		else //AGS
		{
			vx = 0;
			vy = lem->aea.GetLateralVelocity()*0.3048;
		}
		vel_x = vx / 0.3048 * 20.0 / 200.0;
		vel_y = vy / 0.3048 * 20.0 / 200.0;
	}

	//10 times finer scale
	if (scaleSwitch->IsDown())
	{
		vel_x *= 10.0;
		vel_y *= 10.0;
	}

	//The output scaling is 20 for full deflection.
}

void CrossPointer::GetVelocities(double &vx, double &vy)
{
	vx = vel_x;
	vy = vel_y;
}

void CrossPointer::SetDirection(const VECTOR3 &xvec, const VECTOR3 &yvec)
{
	xvector = xvec;
	yvector = yvec;
}

void CrossPointer::DefineMeshGroup(UINT _grpX, UINT _grpY)
{
	grpX = _grpX;
	grpY = _grpY;
}

void CrossPointer::DefineVCAnimations(UINT vc_idx)
{
	xtrans = new MGROUP_TRANSLATE(vc_idx, &grpX, 1, xvector);
	ytrans = new MGROUP_TRANSLATE(vc_idx, &grpY, 1, yvector);
	anim_xpointerx = lem->CreateAnimation(0.5);
	anim_xpointery = lem->CreateAnimation(0.5);
	lem->AddAnimationComponent(anim_xpointerx, 0.0f, 1.0f, xtrans);
	lem->AddAnimationComponent(anim_xpointery, 0.0f, 1.0f, ytrans);
}

void CrossPointer::DrawSwitchVC(int id, int event, SURFHANDLE surf)
{
	if (anim_xpointerx != 1) lem->SetAnimation(anim_xpointerx, (vel_x / 40) + 0.5);
	if (anim_xpointery != 1) lem->SetAnimation(anim_xpointery, (vel_y / 40) + 0.5);
}

void CrossPointer::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, CROSSPOINTER_END_STRING);
}

void CrossPointer::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, CROSSPOINTER_END_STRING, sizeof(CROSSPOINTER_END_STRING))) {
			return;
		}

	}
}