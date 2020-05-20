/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LEM virtual cockpit code

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
#include "LM_VC_Resource.h"
#include "Mission.h"

#include "LEM.h"

void LEM::JostleViewpoint(double amount)

{
	double j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetz += j;

	if (ViewOffsetx > 0.10)
		ViewOffsetx = 0.10;
	if (ViewOffsetx < -0.10)
		ViewOffsetx = -0.10;

	if (ViewOffsety > 0.10)
		ViewOffsety = 0.10;
	if (ViewOffsety < -0.10)
		ViewOffsety = -0.10;

	if (ViewOffsetz > 0.05)
		ViewOffsetz = 0.05;
	if (ViewOffsetz < -0.05)
		ViewOffsetz = -0.05;

	SetView();
}

void LEM::SetView() {

	VECTOR3 v;
	//
	// Set camera offset
	//
	if (InVC) {
		switch (viewpos) {
		case LMVIEW_CDR:
			if (stage == 2) {
				v = _V(-0.55, -0.07, 1.35);
			}
			else {
				v = _V(-0.55, 1.68, 1.35);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;

		case LMVIEW_LMP:
			if (stage == 2) {
				v = _V(0.55, -0.07, 1.35);
			}
			else {
				v = _V(0.55, 1.68, 1.35);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;
		}

		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;

		SetCameraOffset(v);

	} else {

		if(InPanel)
		{
			switch(PanelId)
			{
				case LMPANEL_MAIN:
					if (stage == 2) {
						SetCameraOffset(_V(0, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(0, 1.90, 1.26));
					}
					break;
				case LMPANEL_RIGHTWINDOW:
					if (stage == 2) {
						SetCameraOffset(_V(0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_LEFTWINDOW:
					if (stage == 2) {
						SetCameraOffset(_V(-0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(-0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_LPDWINDOW:
					if (stage == 2) {
						v = _V(-0.61, -0.125, 1.39);
					}
					else {
						v = _V(-0.61, 1.625, 1.39);
					}
					v.x += ViewOffsetx;
					v.y += ViewOffsety;
					v.z += ViewOffsetz;

					SetCameraOffset(v);
					break;
				case LMPANEL_RNDZWINDOW:
					if (stage == 2) {
						SetCameraOffset(_V(-0.598, 0.15, 1.106));
					}
					else {
						SetCameraOffset(_V(-0.598, 1.90, 1.106));
					}
					break;
				case LMPANEL_LEFTPANEL:
					if (stage == 2) {
						SetCameraOffset(_V(-0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(-0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_AOTVIEW:
					if (stage == 2) {
						SetCameraOffset(_V(0, 1.13, 1.26));
					}
					else {
						SetCameraOffset(_V(0, 2.88, 1.26));
					}
					break;
				case LMPANEL_AOTZOOM:
					if (stage == 2) {
						SetCameraOffset(_V(0, 1.13, 1.26));
					}
					else {
						SetCameraOffset(_V(0, 2.88, 1.26));
					}
					break;
				case LMPANEL_DOCKVIEW:
					if (stage == 2) {
						SetCameraOffset(_V(-0.598, 0.15, 1.106));
					}
					else {
						SetCameraOffset(_V(-0.598, 1.90, 1.106));
					}
					break;
				
				case LMPANEL_LEFTZOOM:
					if (stage == 2) {
						SetCameraOffset(_V(-0.576, 0.15, 1.26));
					}
					else {
						SetCameraOffset(_V(-0.576, 1.90, 1.26));
					}
					break;
				case LMPANEL_UPPERHATCH:
					if (stage == 2) {
						SetCameraOffset(_V(0, -0.55, 0));
					}
					else {
						SetCameraOffset(_V(0, 1.20, 0));
					}
					SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
					oapiCameraSetCockpitDir(180 * RAD, 0);
					break;
				case LMPANEL_FWDHATCH:
					if (stage == 2) {
						SetCameraOffset(_V(0, -1.4, 1.5));
					}
					else {
						SetCameraOffset(_V(0, 0.35, 1.5));
					}
					break;
			}
		}
		else
			SetCameraOffset (_V(0, 0, 0));
	}

	//
	// Change FOV for the LPD window and AOT zoom
	//
	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
	   // if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1080.0, 1.0)));
	}
	else if (InPanel && PanelId == LMPANEL_AOTZOOM) {
		// if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1050.0, 1.0)));
	}
    else {
		if(InFOV == false) {
			oapiCameraSetAperture(SaveFOV);
			InFOV = true;
		}
	}
}

bool LEM::clbkLoadVC (int id)
{
	LoadVC();

	switch (id) {
	case 0:
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		return true;

	default:
		return false;
	}
}
void LEM::LoadVC()
{
	VECTOR3 ofs;
	if (stage > 1) { ofs = _V(0.00, -0.76, 0.00); }
	else { ofs = _V(0.00, 0.99, 0.00); }

	RegisterActiveAreas(ofs);
}

void LEM::RegisterActiveAreas(VECTOR3 ofs)
{
	int i = 0;

	//
	// Release all surfaces
	//
	ReleaseSurfacesVC();

	SURFHANDLE MainPanelTex = oapiGetTextureHandle(hLMVC, 2);

	// Panel 1
	for (i = 0; i < P1_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P1_01 + i, P1_TOGGLE_POS[i] + ofs, 0.006);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P1_01 + i, P1_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_LM_CWS_LEFT, _R(238, 27, 559, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(60, 259, 202, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(276, 259, 357, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_RANGE_TAPE, _R(431, 633, 475, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_RATE_TAPE, _R(482, 633, 517, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_LEM_MA_LEFT, _R(30, 593, 77, 636), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_LEFT, _V(-0.415919, 0.599307, 1.65252) + ofs, 0.008);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_LEFT, _R(341, 891, 377, 1098), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_RIGHT, _R(415, 891, 451, 1098), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_MPS_OXID_QUANTITY_INDICATOR, _R(445, 218, 484, 239), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_MPS_FUEL_QUANTITY_INDICATOR, _R(445, 270, 484, 292), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_MPS_HELIUM_PRESS_INDICATOR, _R(577, 259, 658, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);

	oapiVCRegisterArea(AID_VC_XPOINTERCDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_CONTACTLIGHT1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PWRFAIL_LIGHTS_P1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL1_NEEDLES, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_THRUST_WEIGHT_IND, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 2
	for (i = 0; i < P2_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P2_01 + i, P2_TOGGLE_POS[i] + ofs, 0.006);
	}

	for (i = 0; i < P2_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P2_01 + i, P2_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_LM_CWS_RIGHT, _R(1075, 27, 1375, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_RCS_ASC_FEED_TALKBACKS, _R(794, 413, 1031, 436), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_14_TALKBACKS, _R(794, 562, 1031, 585), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_23_TALKBACKS, _R(794, 688, 1031, 711), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_RCS_XFEED_TALKBACK, _R(795, 844, 818, 867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_MAIN_SOV_TALKBACKS, _R(934, 844, 1027, 867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_LEM_MA_RIGHT, _R(1384, 593, 1431, 636), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_RIGHT, _V(0.414751, 0.59891, 1.65247) + ofs, 0.008);

	oapiVCRegisterArea(AID_VC_PANEL2_COMPLIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PWRFAIL_LIGHTS_P2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_XPOINTERLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL2_NEEDLES, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 3
	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + ofs, 0.006);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_RDR_SIG_STR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_RR_NOTRACK, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_CONTACTLIGHT2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 4
	for (i = 0; i < P4_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P4_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P4_01 + i, P4_PUSHB_POS[i] + ofs, 0.008);
	}

	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(309, 1520, 414, 1696), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS,  _R(165, 1525, 267, 1694), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex);

	MainPanelVC.ClearSwitches();

	// Panel 1
	MainPanelVC.AddSwitch(&EngGimbalEnableSwitch, AID_VC_SWITCH_P1_01, &anim_P1switch[0]);
	MainPanelVC.AddSwitch(&RateErrorMonSwitch, AID_VC_SWITCH_P1_02, &anim_P1switch[1]);
	MainPanelVC.AddSwitch(&LeftXPointerSwitch, AID_VC_SWITCH_P1_03, &anim_P1switch[2]);
	MainPanelVC.AddSwitch(&GuidContSwitch, AID_VC_SWITCH_P1_04, &anim_P1switch[3]);
	MainPanelVC.AddSwitch(&ModeSelSwitch, AID_VC_SWITCH_P1_05, &anim_P1switch[4]);
	MainPanelVC.AddSwitch(&AltRngMonSwitch, AID_VC_SWITCH_P1_06, &anim_P1switch[5]);
	MainPanelVC.AddSwitch(&ShiftTruSwitch, AID_VC_SWITCH_P1_07, &anim_P1switch[6]);
	MainPanelVC.AddSwitch(&RateScaleSwitch, AID_VC_SWITCH_P1_08, &anim_P1switch[7]);
	MainPanelVC.AddSwitch(&ACAPropSwitch, AID_VC_SWITCH_P1_09, &anim_P1switch[8]);
	MainPanelVC.AddSwitch(&THRContSwitch, AID_VC_SWITCH_P1_10, &anim_P1switch[9]);
	MainPanelVC.AddSwitch(&MANThrotSwitch, AID_VC_SWITCH_P1_11, &anim_P1switch[10]);
	MainPanelVC.AddSwitch(&EngineArmSwitch, AID_VC_SWITCH_P1_12, &anim_P1switch[11]);
	MainPanelVC.AddSwitch(&ATTTranslSwitch, AID_VC_SWITCH_P1_13, &anim_P1switch[12]);
	MainPanelVC.AddSwitch(&BALCPLSwitch, AID_VC_SWITCH_P1_14, &anim_P1switch[13]);
	MainPanelVC.AddSwitch(&ASCHeReg1Switch, AID_VC_SWITCH_P1_15, &anim_P1switch[14]);
	MainPanelVC.AddSwitch(&ASCHeReg2Switch, AID_VC_SWITCH_P1_16, &anim_P1switch[15]);
	MainPanelVC.AddSwitch(&DESHeReg1Switch, AID_VC_SWITCH_P1_17, &anim_P1switch[16]);
	MainPanelVC.AddSwitch(&DESHeReg2Switch, AID_VC_SWITCH_P1_18, &anim_P1switch[17]);
	MainPanelVC.AddSwitch(&QTYMonSwitch, AID_VC_SWITCH_P1_19, &anim_P1switch[18]);
	MainPanelVC.AddSwitch(&TempPressMonSwitch, AID_VC_SWITCH_P1_20, &anim_P1switch[19]);
	MainPanelVC.AddSwitch(&HeliumMonRotary, AID_VC_ROT_P1_01, &anim_P1_Rot[0]);

	// Panel 2
	MainPanelVC.AddSwitch(&RCSAscFeed1ASwitch, AID_VC_SWITCH_P2_01, &anim_P2switch[0]);
	MainPanelVC.AddSwitch(&RCSAscFeed2ASwitch, AID_VC_SWITCH_P2_02, &anim_P2switch[1]);
	MainPanelVC.AddSwitch(&RCSAscFeed1BSwitch, AID_VC_SWITCH_P2_03, &anim_P2switch[2]);
	MainPanelVC.AddSwitch(&RCSAscFeed2BSwitch, AID_VC_SWITCH_P2_04, &anim_P2switch[3]);
	MainPanelVC.AddSwitch(&RCSQuad1ACmdEnableSwitch, AID_VC_SWITCH_P2_05, &anim_P2switch[4]);
	MainPanelVC.AddSwitch(&RCSQuad4ACmdEnableSwitch, AID_VC_SWITCH_P2_06, &anim_P2switch[5]);
	MainPanelVC.AddSwitch(&RCSQuad1BCmdEnableSwitch, AID_VC_SWITCH_P2_07, &anim_P2switch[6]);
	MainPanelVC.AddSwitch(&RCSQuad4BCmdEnableSwitch, AID_VC_SWITCH_P2_08, &anim_P2switch[7]);
	MainPanelVC.AddSwitch(&RCSQuad2ACmdEnableSwitch, AID_VC_SWITCH_P2_09, &anim_P2switch[8]);
	MainPanelVC.AddSwitch(&RCSQuad3ACmdEnableSwitch, AID_VC_SWITCH_P2_10, &anim_P2switch[9]);
	MainPanelVC.AddSwitch(&RCSQuad2BCmdEnableSwitch, AID_VC_SWITCH_P2_11, &anim_P2switch[10]);
	MainPanelVC.AddSwitch(&RCSQuad3BCmdEnableSwitch, AID_VC_SWITCH_P2_12, &anim_P2switch[11]);
	MainPanelVC.AddSwitch(&RCSXFeedSwitch, AID_VC_SWITCH_P2_13, &anim_P2switch[12]);
	MainPanelVC.AddSwitch(&RCSMainSovASwitch, AID_VC_SWITCH_P2_14, &anim_P2switch[13]);
	MainPanelVC.AddSwitch(&RCSMainSovBSwitch, AID_VC_SWITCH_P2_15, &anim_P2switch[14]);
	MainPanelVC.AddSwitch(&RightACAPropSwitch, AID_VC_SWITCH_P2_16, &anim_P2switch[15]);
	MainPanelVC.AddSwitch(&RightRateErrorMonSwitch, AID_VC_SWITCH_P2_17, &anim_P2switch[16]);
	MainPanelVC.AddSwitch(&RightAttitudeMonSwitch, AID_VC_SWITCH_P2_18, &anim_P2switch[17]);
	MainPanelVC.AddSwitch(&TempPressMonRotary, AID_VC_ROT_P2_01, &anim_P2_Rot[0]);
	MainPanelVC.AddSwitch(&GlycolRotary, AID_VC_ROT_P2_02, &anim_P2_Rot[1]);
	MainPanelVC.AddSwitch(&SuitFanRotary, AID_VC_ROT_P2_03, &anim_P2_Rot[2]);
	MainPanelVC.AddSwitch(&QtyMonRotary, AID_VC_ROT_P2_04, &anim_P2_Rot[3]);

	// Panel 3
	MainPanelVC.AddSwitch(&EngGimbalEnableSwitch, AID_VC_SWITCH_P3_01, &anim_P3switch[0]);
	MainPanelVC.AddSwitch(&EngineDescentCommandOverrideSwitch, AID_VC_SWITCH_P3_02, &anim_P3switch[1]);
	MainPanelVC.AddSwitch(&LandingAntSwitch, AID_VC_SWITCH_P3_03, &anim_P3switch[2]);
	MainPanelVC.AddSwitch(&RadarTestSwitch, AID_VC_SWITCH_P3_04, &anim_P3switch[3]);
	MainPanelVC.AddSwitch(&SlewRateSwitch, AID_VC_SWITCH_P3_05, &anim_P3switch[4]);
	MainPanelVC.AddSwitch(&DeadBandSwitch, AID_VC_SWITCH_P3_06, &anim_P3switch[5]);
	MainPanelVC.AddSwitch(&GyroTestLeftSwitch, AID_VC_SWITCH_P3_07, &anim_P3switch[6]);
	MainPanelVC.AddSwitch(&GyroTestRightSwitch, AID_VC_SWITCH_P3_08, &anim_P3switch[7]);
	MainPanelVC.AddSwitch(&RollSwitch, AID_VC_SWITCH_P3_09, &anim_P3switch[8]);
	MainPanelVC.AddSwitch(&PitchSwitch, AID_VC_SWITCH_P3_10, &anim_P3switch[9]);
	MainPanelVC.AddSwitch(&YawSwitch, AID_VC_SWITCH_P3_11, &anim_P3switch[10]);
	MainPanelVC.AddSwitch(&ModeControlPGNSSwitch, AID_VC_SWITCH_P3_12, &anim_P3switch[11]);
	MainPanelVC.AddSwitch(&ModeControlAGSSwitch, AID_VC_SWITCH_P3_13, &anim_P3switch[12]);
	MainPanelVC.AddSwitch(&IMUCageSwitch, AID_VC_SWITCH_P3_14, &anim_P3switch[13]);
	MainPanelVC.AddSwitch(&EventTimerCtlSwitch, AID_VC_SWITCH_P3_15, &anim_P3switch[14]);
	MainPanelVC.AddSwitch(&EventTimerStartSwitch, AID_VC_SWITCH_P3_16, &anim_P3switch[15]);
	MainPanelVC.AddSwitch(&EventTimerMinuteSwitch, AID_VC_SWITCH_P3_17, &anim_P3switch[16]);
	MainPanelVC.AddSwitch(&EventTimerSecondSwitch, AID_VC_SWITCH_P3_18, &anim_P3switch[17]);
	MainPanelVC.AddSwitch(&RCSSysQuad1Switch, AID_VC_SWITCH_P3_19, &anim_P3switch[18]);
	MainPanelVC.AddSwitch(&RCSSysQuad4Switch, AID_VC_SWITCH_P3_20, &anim_P3switch[19]);
	MainPanelVC.AddSwitch(&RCSSysQuad2Switch, AID_VC_SWITCH_P3_21, &anim_P3switch[20]);
	MainPanelVC.AddSwitch(&RCSSysQuad3Switch, AID_VC_SWITCH_P3_22, &anim_P3switch[21]);
	MainPanelVC.AddSwitch(&SidePanelsSwitch, AID_VC_SWITCH_P3_23, &anim_P3switch[22]);
	MainPanelVC.AddSwitch(&FloodSwitch, AID_VC_SWITCH_P3_24, &anim_P3switch[23]);
	MainPanelVC.AddSwitch(&RightXPointerSwitch, AID_VC_SWITCH_P3_25, &anim_P3switch[24]);
	MainPanelVC.AddSwitch(&ExteriorLTGSwitch, AID_VC_SWITCH_P3_26, &anim_P3switch[25]);

	MainPanelVC.AddSwitch(&TestMonitorRotary, AID_VC_ROT_P3_01, &anim_P3_Rot[0]);
	MainPanelVC.AddSwitch(&RendezvousRadarRotary, AID_VC_ROT_P3_02, &anim_P3_Rot[1]);
	MainPanelVC.AddSwitch(&TempMonitorRotary, AID_VC_ROT_P3_03, &anim_P3_Rot[2]);
	MainPanelVC.AddSwitch(&LampToneTestRotary, AID_VC_ROT_P3_04, &anim_P3_Rot[3]);
	MainPanelVC.AddSwitch(&FloodRotary, AID_VC_ROT_P3_05, &anim_P3_Rot[4]);

	//Panel 4
	MainPanelVC.AddSwitch(&DskySwitchVerb, AID_VC_PUSHB_P4_01, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P4_02, NULL);
	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P4_03, NULL);
	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P4_04, NULL);
	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P4_05, NULL);
	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P4_06, NULL);
	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P4_07, NULL);
	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P4_08, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P4_09, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P4_10, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P4_11, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P4_12, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P4_13, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P4_14, NULL);
	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P4_15, NULL);
	MainPanelVC.AddSwitch(&DskySwitchProg, AID_VC_PUSHB_P4_16, NULL);
	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P4_17, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P4_18, NULL);
	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P4_19, NULL);

	//
	// Initialize surfaces and switches
	//
	InitPanelVC();
}

bool LEM::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
{
	switch (id) {
		case AID_VC_LEM_MA_LEFT:
		case AID_VC_LEM_MA_RIGHT:
			return CWEA.CheckMasterAlarmMouseClick(event);
	}
	return MainPanelVC.VCMouseEvent(id, event, p);
}

bool LEM::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	switch (id) {

	case AID_VC_LM_CWS_LEFT:
		CWEA.RedrawLeft(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_LM_CWS_RIGHT:
		CWEA.RedrawRight(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP]);
		return true;

	case AID_VC_DSKY_LIGHTS:
		if (pMission->GetLMDSKYVersion() == 3)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, true);
		}
		else if (pMission->GetLMDSKYVersion() == 2)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS]);
		}
		else
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, false);
		}
		return true;

	case AID_VC_RDR_SIG_STR:
		SetAnimation(anim_Needle_Radar, (RadarSignalStrengthMeter.GetDisplayValue() / 6.67) + 0.125);
		return true;

	case AID_VC_RANGE_TAPE:
		RadarTape.RenderRangeVC(surf, srf[SRF_VC_RADAR_TAPE], srf[SRF_VC_RADAR_TAPE2]);
		return true;

	case AID_VC_RATE_TAPE:
		RadarTape.RenderRateVC(surf, srf[SRF_VC_RADAR_TAPE]);
		return true;

	case AID_VC_XPOINTERCDR:
		RedrawPanel_XPointerVC(&crossPointerLeft, anim_xpointerx_cdr, anim_xpointery_cdr);
		return true;

	case AID_VC_XPOINTERLMP:
		RedrawPanel_XPointerVC(&crossPointerRight, anim_xpointerx_lmp, anim_xpointery_lmp);
		return true;

	case AID_VC_CONTACTLIGHT1:
		if (SCS_ENG_CONT_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(0, true); // Light On
		}
		else
		{
			SetContactLight(0, false); // Light On
		}
		return true;

	case AID_VC_CONTACTLIGHT2:
		if (SCS_ATCA_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(1, true); // Light On
		}
		else
		{
			SetContactLight(1, false); // Light On
		}
		return true;

	case AID_VC_FDAI_LEFT:
		if (fdaiLeft.IsPowered()) {
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (AttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

			if (RateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (AttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI1PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			AnimateFDAI(attitude, rates, errors, anim_fdaiR_cdr, anim_fdaiP_cdr, anim_fdaiY_cdr, anim_fdaiRerror_cdr, anim_fdaiPerror_cdr, anim_fdaiYerror_cdr, anim_fdaiRrate_cdr, anim_fdaiPrate_cdr, anim_fdaiYrate_cdr);
			SetAnimation(anim_attflag_cdr, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_cdr, 1.0);
		}
		return true;

	case AID_VC_FDAI_RIGHT:
		if (fdaiRight.IsPowered()) {
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (RightAttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

			if (RightRateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (RightAttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI2PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			AnimateFDAI(attitude, rates, errors, anim_fdaiR_lmp, anim_fdaiP_lmp, anim_fdaiY_lmp, anim_fdaiRerror_lmp, anim_fdaiPerror_lmp, anim_fdaiYerror_lmp, anim_fdaiRrate_lmp, anim_fdaiPrate_lmp, anim_fdaiYrate_lmp);
			SetAnimation(anim_attflag_lmp, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_lmp, 1.0);
		}
		return true;

	case AID_VC_RR_NOTRACK:
		if (lca.GetAnnunVoltage() > 2.25 && (RR.GetNoTrackSignal() || LampToneTestRotary.GetState() == 6)) { // The AC side is only needed for the transmitter
			SetCompLight(LM_VC_COMP_LIGHT_1, true); // Light On
		} else {
			SetCompLight(LM_VC_COMP_LIGHT_1, false); // Light Off
		}
		return true;

	case AID_VC_PANEL2_COMPLIGHTS:
		if (lca.GetAnnunVoltage() > 2.25 && (scera2.GetSwitch(12, 2)->IsClosed() || PrimGlycolPumpController.GetPressureSwitch() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_2, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_2, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && (SuitFanDPSensor.GetSuitFanFail() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_3, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_3, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25) {
			if (INST_CWEA_CB.IsPowered() && ECS_CO2_SENSOR_CB.IsPowered() && (scera1.GetVoltage(5, 2) >= (7.6 / 6))) {
				SetCompLight(LM_VC_COMP_LIGHT_4, true); // Light On
			}
			else if (CO2CanisterSelectSwitch.GetState() == 0 || LampToneTestRotary.GetState() == 6) {
				SetCompLight(LM_VC_COMP_LIGHT_4, true); // Light On
			} else {
				SetCompLight(LM_VC_COMP_LIGHT_4, false); // Light Off
			}
		} else {
			SetCompLight(LM_VC_COMP_LIGHT_4, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && INST_CWEA_CB.IsPowered() && (scera1.GetVoltage(5, 3) < (792.5 / 720.0) || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_5, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_5, false); // Light Off
		}
		return true;

	case AID_VC_RCS_ASC_FEED_TALKBACKS:
		RCSAscFeed1ATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSAscFeed2ATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSAscFeed1BTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSAscFeed2BTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_LGC_CMD_ENABLE_14_TALKBACKS:
		RCSQuad1ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad4ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad1BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad4BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		return true;

	case AID_VC_LGC_CMD_ENABLE_23_TALKBACKS:
		RCSQuad2ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad3ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad2BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad3BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		return true;

	case AID_VC_RCS_XFEED_TALKBACK:
		RCSXFeedTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_MAIN_SOV_TALKBACKS:
		RCSMainSovATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSMainSovBTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_MPS_REG_CONTROLS_LEFT:
		ASCHeReg1TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DESHeReg1TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_MPS_REG_CONTROLS_RIGHT:
		ASCHeReg2TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DESHeReg2TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P1:
		if (!pfira.GetCDRXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_1, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_1, false); // Light Off
		}

		if (!pfira.GetThrustIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_2, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_2, false); // Light Off
		}

		if (!pfira.GetPropPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_3, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_3, false); // Light Off
		}
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P2:

		if (!pfira.GetRCSPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_4, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_4, false); // Light Off
		}

		if (!pfira.GetRCSQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_5, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_5, false); // Light Off
		}

		if (!pfira.GetSuitCabinPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_6, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_6, false); // Light Off
		}

		if (!pfira.GetGlyTempPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_7, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_7, false); // Light Off
		}

		if (!pfira.GetO2H2OQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_8, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_8, false); // Light Off
		}

		if (!pfira.GetLMPXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_9, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_9, false); // Light Off
		}
		return true;

	case AID_VC_LEM_MA_LEFT:
	case AID_VC_LEM_MA_RIGHT:
		CWEA.RenderMasterAlarm(surf, srf[SRF_LEM_MASTERALARMVC], NULL);
		return true;

	case AID_VC_MPS_OXID_QUANTITY_INDICATOR:
		DPSOxidPercentMeter.DoDrawSwitchVC(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;
	case AID_VC_MPS_FUEL_QUANTITY_INDICATOR:
		DPSFuelPercentMeter.DoDrawSwitchVC(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;
	case AID_VC_MPS_HELIUM_PRESS_INDICATOR:
		MainHeliumPressureMeter.DoDrawSwitchVC(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_PANEL2_NEEDLES:
		LMRCSATempInd.DoDrawSwitchVC(anim_P2needles[0]);
		LMRCSBTempInd.DoDrawSwitchVC(anim_P2needles[1]);
		LMRCSAPressInd.DoDrawSwitchVC(anim_P2needles[2]);
		LMRCSBPressInd.DoDrawSwitchVC(anim_P2needles[3]);
		LMRCSAQtyInd.DoDrawSwitchVC(anim_P2needles[4]);
		LMRCSBQtyInd.DoDrawSwitchVC(anim_P2needles[5]);
		LMSuitTempMeter.DoDrawSwitchVC(anim_P2needles[6]);
		LMCabinTempMeter.DoDrawSwitchVC(anim_P2needles[7]);
		LMSuitPressMeter.DoDrawSwitchVC(anim_P2needles[8]);
		LMCabinPressMeter.DoDrawSwitchVC(anim_P2needles[9]);
		LMGlycolTempMeter.DoDrawSwitchVC(anim_P2needles[10]);
		LMGlycolPressMeter.DoDrawSwitchVC(anim_P2needles[11]);
		LMOxygenQtyMeter.DoDrawSwitchVC(anim_P2needles[12]);
		LMWaterQtyMeter.DoDrawSwitchVC(anim_P2needles[13]);
		LMCO2Meter.DoDrawSwitchVC(anim_P2needles[14]);
		return true;

	case AID_VC_PANEL1_NEEDLES:
		EngineThrustInd.DoDrawSwitchVC(anim_P1needles[0]);
		CommandedThrustInd.DoDrawSwitchVC(anim_P1needles[1]);
		MainFuelTempInd.DoDrawSwitchVC(anim_P1needles[2]);
		MainOxidizerTempInd.DoDrawSwitchVC(anim_P1needles[3]);
		MainFuelPressInd.DoDrawSwitchVC(anim_P1needles[4]);
		MainOxidizerPressInd.DoDrawSwitchVC(anim_P1needles[5]);
		return true;

	case AID_VC_THRUST_WEIGHT_IND:
		ThrustWeightInd.DoDrawSwitchVC(anim_TW_indicator);
		return true;
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
}

void LEM::DeleteVCAnimations()
{
	int i = 0;

	for (i = 0; i < P1_SWITCHCOUNT; i++) delete mgt_P1switch[i];

	for (i = 0; i < P1_ROTCOUNT; i++) delete mgt_P1Rot[i];

	for (i = 0; i < P1_NEEDLECOUNT; i++) delete mgt_P1needles[i];

	for (i = 0; i < P2_SWITCHCOUNT; i++) delete mgt_P2switch[i];

	for (i = 0; i < P2_ROTCOUNT; i++) delete mgt_P2Rot[i];

	for (i = 0; i < P2_NEEDLECOUNT; i++) delete mgt_P2needles[i];

	for (i = 0; i < P3_SWITCHCOUNT; i++) delete mgt_P3switch[i];

	for (i = 0; i < P3_ROTCOUNT; i++) delete mgt_P3Rot[i];
}

void LEM::InitVCAnimations()
{
	UINT mesh = vcidx;
	int i = 0;

	// Panel 1 switches/rotaries/needles
	static UINT meshgroup_P1switches[P1_SWITCHCOUNT], meshgroup_P1Rots[P1_ROTCOUNT], meshgroup_P1needles[P1_NEEDLECOUNT];
	for (int i = 0; i < P1_SWITCHCOUNT; i++)
	{
		meshgroup_P1switches[i] = VC_GRP_Sw_P1_01 + i;
		mgt_P1switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P1switches[i], 1, P1_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P1switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P1switch[i], 0.0f, 1.0f, mgt_P1switch[i]);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		meshgroup_P1Rots[i] = VC_GRP_Rot_P1_01 + i;
		mgt_P1Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P1Rots[i], 1, P1_ROT_POS[i], P1_ROT_AXIS, (float)(RAD * 360));
		anim_P1_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P1_Rot[i], 0.0f, 1.0f, mgt_P1Rot[i]);
	}

	for (int i = 0; i < P1_NEEDLECOUNT; i++)
	{
		const VECTOR3 xvector = { 0.00, 0.0625*cos(P1_TILT), 0.0625*sin(P1_TILT) };
		meshgroup_P1needles[i] = VC_GRP_Needle_P1_01 + i;
		mgt_P1needles[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P1needles[i], 1, xvector);
		anim_P1needles[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P1needles[i], 0.0f, 1.0f, mgt_P1needles[i]);
	}

	// Panel 2 switches/rotaries/needles
	static UINT meshgroup_P2switches[P2_SWITCHCOUNT], meshgroup_P2Rots[P2_ROTCOUNT], meshgroup_P2needles[P2_NEEDLECOUNT];
	for (int i = 0; i < P2_SWITCHCOUNT; i++)
	{
		meshgroup_P2switches[i] = VC_GRP_Sw_P2_01 + i;
		mgt_P2switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P2switches[i], 1, P2_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P2switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P2switch[i], 0.0f, 1.0f, mgt_P2switch[i]);
	}

	for (i = 0; i < P2_ROTCOUNT; i++)
	{
		meshgroup_P2Rots[i] = VC_GRP_Rot_P2_01 + i;
		mgt_P2Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P2Rots[i], 1, P2_ROT_POS[i], P2_ROT_AXIS, (float)(RAD * 360));
		anim_P2_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P2_Rot[i], 0.0f, 1.0f, mgt_P2Rot[i]);
	}

	for (int i = 0; i < P2_NEEDLECOUNT; i++)
	{
		const VECTOR3 xvector = { 0.00, 0.0625*cos(P1_TILT), 0.0625*sin(P1_TILT) };
		meshgroup_P2needles[i] = VC_GRP_Needle_P2_01 + i;
		mgt_P2needles[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P2needles[i], 1, xvector);
		anim_P2needles[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P2needles[i], 0.0f, 1.0f, mgt_P2needles[i]);
	}

	// Panel 3 switches/rotaries/needles
	static UINT meshgroup_P3switches[P3_SWITCHCOUNT], meshgroup_P3Rots[P3_ROTCOUNT];
	for (int i = 0; i < P3_SWITCHCOUNT; i++)
	{
		meshgroup_P3switches[i] = VC_GRP_Sw_P3_01 + i;
		mgt_P3switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3switches[i], 1, P3_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P3switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P3switch[i], 0.0f, 1.0f, mgt_P3switch[i]);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		meshgroup_P3Rots[i] = VC_GRP_Rot_P3_01 + i;
		mgt_P3Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3Rots[i], 1, P3_ROT_POS[i], P3_ROT_AXIS, (float)(RAD * 360));
		anim_P3_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P3_Rot[i], 0.0f, 1.0f, mgt_P3Rot[i]);
	}

	// Thrust-weight indicator
	const VECTOR3 tw_xvector = { 0.00, 0.0925*cos(P1_TILT), 0.0925*sin(P1_TILT) };
	static UINT meshgroup_TW_indicator = VC_GRP_Needle_P1_07;
	static MGROUP_TRANSLATE mgt_TW_indicator(mesh, &meshgroup_TW_indicator, 1, tw_xvector);
	anim_TW_indicator = CreateAnimation(0.0);
	AddAnimationComponent(anim_TW_indicator, 0.0f, 1.0f, &mgt_TW_indicator);

	// Radar strength meter
	static UINT meshgroup_Needle_Radar = VC_GRP_Needle_Radar;
	static MGROUP_ROTATE mgt_Needle_Radar(mesh, &meshgroup_Needle_Radar, 1, _V(-0.264142, 0.235721, 1.57832), P3_ROT_AXIS, (float)(RAD * 360));
	anim_Needle_Radar = CreateAnimation(0.5);
	AddAnimationComponent(anim_Needle_Radar, 0.0f, 1.0f, &mgt_Needle_Radar);

	// CDR & LMP X-pointer
	const VECTOR3 xvector = { 0.00, 0.075*cos(P1_TILT), 0.075*sin(P1_TILT) };
	const VECTOR3 yvector = { 0.075, 0, 0 };
	static UINT meshgroup_XpointerX_cdr = VC_GRP_XpointerX_cdr, meshgroup_XpointerX_lmp = VC_GRP_XpointerX_lmp;
	static UINT meshgroup_XpointerY_cdr = VC_GRP_XpointerY_cdr, meshgroup_XpointerY_lmp = VC_GRP_XpointerY_lmp;
	static MGROUP_TRANSLATE mgt_xpointerx_cdr(mesh, &meshgroup_XpointerX_cdr, 1, xvector);
	static MGROUP_TRANSLATE mgt_xpointery_cdr(mesh, &meshgroup_XpointerY_cdr, 1, yvector);
	static MGROUP_TRANSLATE mgt_xpointerx_lmp(mesh, &meshgroup_XpointerX_lmp, 1, xvector);
	static MGROUP_TRANSLATE mgt_xpointery_lmp(mesh, &meshgroup_XpointerY_lmp, 1, yvector);
	anim_xpointerx_cdr = CreateAnimation(0.5);
	anim_xpointery_cdr = CreateAnimation(0.5);
	anim_xpointerx_lmp = CreateAnimation(0.5);
	anim_xpointery_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_xpointerx_cdr, 0.0f, 1.0f, &mgt_xpointerx_cdr);
	AddAnimationComponent(anim_xpointery_cdr, 0.0f, 1.0f, &mgt_xpointery_cdr);
	AddAnimationComponent(anim_xpointerx_lmp, 0.0f, 1.0f, &mgt_xpointerx_lmp);
	AddAnimationComponent(anim_xpointery_lmp, 0.0f, 1.0f, &mgt_xpointery_lmp);

	InitFDAI(mesh);
}

void LEM::InitFDAI(UINT mesh) {

	// 3D FDAI initialization

	// Constants
	const VECTOR3 fdairollaxis = { -0.00, sin(P1_TILT), -cos(P1_TILT) };
	const VECTOR3 fdaiyawvaxis = { -0.00, sin(P1_TILT + (90.0 * RAD)), -cos(P1_TILT + (90.0 * RAD)) };
	const VECTOR3 needlexvector = { 0.00, 0.05*cos(P1_TILT), 0.05*sin(P1_TILT) };
	const VECTOR3 needleyvector = { 0.05, 0, 0 };
	const VECTOR3 ratexvector = { 0.00, 0.062*cos(P1_TILT), 0.062*sin(P1_TILT) };
	const VECTOR3 rateyvector = { 0.062, 0, 0 };
	const VECTOR3 FDAI_PIVOTCDR = { -0.297851, 0.525802, 1.70639 }; // CDR FDAI Pivot Point
	const VECTOR3 FDAI_PIVOTLMP = { 0.297069, 0.525802, 1.70565 }; // LMP FDAI Pivot Point
	const VECTOR3 attflagvector = { 0.01, 0, 0 };

	// CDR FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_cdr, ach_FDAIpitch_cdr, ach_FDAIyaw_cdr;
	static UINT meshgroup_Fdai1_cdr = { VC_GRP_FDAIBall1_cdr }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_cdr = { VC_GRP_FDAIBall_cdr };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_cdr = { VC_GRP_FDAIBall2_cdr }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_cdr(mesh, &meshgroup_Fdai1_cdr, 1, FDAI_PIVOTCDR, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_cdr(mesh, &meshgroup_Fdai2_cdr, 1, FDAI_PIVOTCDR, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_cdr(mesh, &meshgroup_Fdai3_cdr, 1, FDAI_PIVOTCDR, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_cdr = CreateAnimation(0.0);
	anim_fdaiP_cdr = CreateAnimation(0.0);
	anim_fdaiY_cdr = CreateAnimation(0.0);
	ach_FDAIroll_cdr = AddAnimationComponent(anim_fdaiR_cdr, 0.0f, 1.0f, &mgt_FDAIRoll_cdr);
	ach_FDAIyaw_cdr = AddAnimationComponent(anim_fdaiY_cdr, 0.0f, 1.0f, &mgt_FDAIYaw_cdr, ach_FDAIroll_cdr);
	ach_FDAIpitch_cdr = AddAnimationComponent(anim_fdaiP_cdr, 0.0f, 1.0f, &mgt_FDAIPitch_cdr, ach_FDAIyaw_cdr);

	// CDR FDAI error needles
	static UINT meshgroup_RollError_cdr = VC_GRP_FDAI_rollerror_cdr;
	static UINT meshgroup_PitchError_cdr = VC_GRP_FDAI_pitcherror_cdr;
	static UINT meshgroup_YawError_cdr = VC_GRP_FDAI_yawerror_cdr;
	static MGROUP_TRANSLATE mgt_rollerror_cdr(mesh, &meshgroup_RollError_cdr, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_cdr(mesh, &meshgroup_PitchError_cdr, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_cdr(mesh, &meshgroup_YawError_cdr, 1, needleyvector);
	anim_fdaiRerror_cdr = CreateAnimation(0.5);
	anim_fdaiPerror_cdr = CreateAnimation(0.5);
	anim_fdaiYerror_cdr = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_cdr, 0.0f, 1.0f, &mgt_rollerror_cdr);
	AddAnimationComponent(anim_fdaiPerror_cdr, 0.0f, 1.0f, &mgt_pitcherror_cdr);
	AddAnimationComponent(anim_fdaiYerror_cdr, 0.0f, 1.0f, &mgt_yawerror_cdr);

	// CDR FDAI rate needles
	static UINT meshgroup_RollRate_cdr = VC_GRP_FDAI_rateR_cdr;
	static UINT meshgroup_PitchRate_cdr = VC_GRP_FDAI_rateP_cdr;
	static UINT meshgroup_YawRate_cdr = VC_GRP_FDAI_rateY_cdr;
	static MGROUP_TRANSLATE mgt_rollrate_cdr(mesh, &meshgroup_RollRate_cdr, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_cdr(mesh, &meshgroup_PitchRate_cdr, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_cdr(mesh, &meshgroup_YawRate_cdr, 1, rateyvector);
	anim_fdaiRrate_cdr = CreateAnimation(0.5);
	anim_fdaiPrate_cdr = CreateAnimation(0.5);
	anim_fdaiYrate_cdr = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_cdr, 0.0f, 1.0f, &mgt_rollrate_cdr);
	AddAnimationComponent(anim_fdaiPrate_cdr, 0.0f, 1.0f, &mgt_pitchrate_cdr);
	AddAnimationComponent(anim_fdaiYrate_cdr, 0.0f, 1.0f, &mgt_yawrate_cdr);

	// CDR FDAI no attitude flag
	static UINT meshgroup_attflag_cdr = VC_GRP_FDAIflag_cdr;
	static MGROUP_TRANSLATE mgt_attflag_cdr(mesh, &meshgroup_attflag_cdr, 1, attflagvector);
	anim_attflag_cdr = CreateAnimation(1.0);
	AddAnimationComponent(anim_attflag_cdr, 0.0f, 1.0f, &mgt_attflag_cdr);

	// LMP FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_lmp, ach_FDAIpitch_lmp, ach_FDAIyaw_lmp;
	static UINT meshgroup_Fdai1_lmp = { VC_GRP_FDAIBall1_lmp }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_lmp = { VC_GRP_FDAIBall_lmp };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_lmp = { VC_GRP_FDAIBall2_lmp }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_lmp(mesh, &meshgroup_Fdai1_lmp, 1, FDAI_PIVOTLMP, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_lmp(mesh, &meshgroup_Fdai2_lmp, 1, FDAI_PIVOTLMP, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_lmp(mesh, &meshgroup_Fdai3_lmp, 1, FDAI_PIVOTLMP, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_lmp = CreateAnimation(0.0);
	anim_fdaiP_lmp = CreateAnimation(0.0);
	anim_fdaiY_lmp = CreateAnimation(0.0);
	ach_FDAIroll_lmp = AddAnimationComponent(anim_fdaiR_lmp, 0.0f, 1.0f, &mgt_FDAIRoll_lmp);
	ach_FDAIyaw_lmp = AddAnimationComponent(anim_fdaiY_lmp, 0.0f, 1.0f, &mgt_FDAIYaw_lmp, ach_FDAIroll_lmp);
	ach_FDAIpitch_lmp = AddAnimationComponent(anim_fdaiP_lmp, 0.0f, 1.0f, &mgt_FDAIPitch_lmp, ach_FDAIyaw_lmp);

	// LMP FDAI error needles
	static UINT meshgroup_RollError_lmp = VC_GRP_FDAI_rollerror_lmp;
	static UINT meshgroup_PitchError_lmp = VC_GRP_FDAI_pitcherror_lmp;
	static UINT meshgroup_YawError_lmp = VC_GRP_FDAI_yawerror_lmp;
	static MGROUP_TRANSLATE mgt_rollerror_lmp(mesh, &meshgroup_RollError_lmp, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_lmp(mesh, &meshgroup_PitchError_lmp, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_lmp(mesh, &meshgroup_YawError_lmp, 1, needleyvector);
	anim_fdaiRerror_lmp = CreateAnimation(0.5);
	anim_fdaiPerror_lmp = CreateAnimation(0.5);
	anim_fdaiYerror_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_lmp, 0.0f, 1.0f, &mgt_rollerror_lmp);
	AddAnimationComponent(anim_fdaiPerror_lmp, 0.0f, 1.0f, &mgt_pitcherror_lmp);
	AddAnimationComponent(anim_fdaiYerror_lmp, 0.0f, 1.0f, &mgt_yawerror_lmp);

	// LMP FDAI rate needles
	static UINT meshgroup_RollRate_lmp = VC_GRP_FDAI_rateR_lmp;
	static UINT meshgroup_PitchRate_lmp = VC_GRP_FDAI_rateP_lmp;
	static UINT meshgroup_YawRate_lmp = VC_GRP_FDAI_rateY_lmp;
	static MGROUP_TRANSLATE mgt_rollrate_lmp(mesh, &meshgroup_RollRate_lmp, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_lmp(mesh, &meshgroup_PitchRate_lmp, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_lmp(mesh, &meshgroup_YawRate_lmp, 1, rateyvector);
	anim_fdaiRrate_lmp = CreateAnimation(0.5);
	anim_fdaiPrate_lmp = CreateAnimation(0.5);
	anim_fdaiYrate_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_lmp, 0.0f, 1.0f, &mgt_rollrate_lmp);
	AddAnimationComponent(anim_fdaiPrate_lmp, 0.0f, 1.0f, &mgt_pitchrate_lmp);
	AddAnimationComponent(anim_fdaiYrate_lmp, 0.0f, 1.0f, &mgt_yawrate_lmp);

	// LMP FDAI no attitude flag
	static UINT meshgroup_attflag_lmp = VC_GRP_FDAIflag_lmp;
	static MGROUP_TRANSLATE mgt_attflag_lmp(mesh, &meshgroup_attflag_lmp, 1, attflagvector);
	anim_attflag_lmp = CreateAnimation(1.0);
	AddAnimationComponent(anim_attflag_lmp, 0.0f, 1.0f, &mgt_attflag_lmp);
}

void LEM::SetCompLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - 6;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 0.878f;
		mat->emissive.b = 0.506f;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.25f;
		mat->emissive.g = 0.22f;
		mat->emissive.b = 0.127f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetContactLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - 8;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 0;
		mat->emissive.g = 0.541f;
		mat->emissive.b = 1;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0;
		mat->emissive.g = 0.135f;
		mat->emissive.b = 0.25f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetPowerFailureLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - 17;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 0;
		mat->emissive.b = 0;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.25f;
		mat->emissive.g = 0;
		mat->emissive.b = 0;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::AnimateFDAI(VECTOR3 attitude, VECTOR3 rates, VECTOR3 errors, UINT animR, UINT animP, UINT animY, UINT errorR, UINT errorP, UINT errorY, UINT rateR, UINT rateP, UINT rateY) {

	double fdai_proc[3];

	// Drive FDAI ball
	fdai_proc[0] = -attitude.x / PI2; // 1.0 - attitude.x / PI2;
	fdai_proc[1] = attitude.y / PI2;
	fdai_proc[2] = attitude.z / PI2;
	if (fdai_proc[0] < 0) fdai_proc[0] += 1.0;
	if (fdai_proc[1] < 0) fdai_proc[1] += 1.0;
	if (fdai_proc[2] < 0) fdai_proc[2] += 1.0;
	SetAnimation(animY, fdai_proc[2]);
	SetAnimation(animR, fdai_proc[0]);
	SetAnimation(animP, fdai_proc[1]);

	// Drive error needles
	SetAnimation(errorR, (errors.x + 46) / 92);
	SetAnimation(errorP, (-errors.y + 46) / 92);
	SetAnimation(errorY, (errors.z + 46) / 92);

	// Drive rate needles
	SetAnimation(rateR, (rates.z + 1) / 2);
	SetAnimation(rateP, (rates.x + 1) / 2);
	SetAnimation(rateY, (-rates.y + 1) / 2);
}

