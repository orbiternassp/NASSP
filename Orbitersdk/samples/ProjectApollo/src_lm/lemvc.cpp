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

	// Panel 3 switches
	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + ofs, 0.012);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_ROT_P3_01 + i, PANEL_REDRAW_MOUSE, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, 0.02);
	}

	MainPanelVC.ClearSwitches();
	MainPanelVC.AddSwitch(&EngGimbalEnableSwitch, AID_SWITCH_P3_01, &anim_P3switch[0]);
	MainPanelVC.AddSwitch(&EngineDescentCommandOverrideSwitch, AID_SWITCH_P3_02, &anim_P3switch[1]);
	MainPanelVC.AddSwitch(&LandingAntSwitch, AID_SWITCH_P3_03, &anim_P3switch[2]);
	MainPanelVC.AddSwitch(&RadarTestSwitch, AID_SWITCH_P3_04, &anim_P3switch[3]);
	MainPanelVC.AddSwitch(&SlewRateSwitch, AID_SWITCH_P3_05, &anim_P3switch[4]);
	MainPanelVC.AddSwitch(&DeadBandSwitch, AID_SWITCH_P3_06, &anim_P3switch[5]);
	MainPanelVC.AddSwitch(&GyroTestLeftSwitch, AID_SWITCH_P3_07, &anim_P3switch[6]);
	MainPanelVC.AddSwitch(&GyroTestRightSwitch, AID_SWITCH_P3_08, &anim_P3switch[7]);
	MainPanelVC.AddSwitch(&RollSwitch, AID_SWITCH_P3_09, &anim_P3switch[8]);
	MainPanelVC.AddSwitch(&PitchSwitch, AID_SWITCH_P3_10, &anim_P3switch[9]);
	MainPanelVC.AddSwitch(&YawSwitch, AID_SWITCH_P3_11, &anim_P3switch[10]);
	MainPanelVC.AddSwitch(&ModeControlPGNSSwitch, AID_SWITCH_P3_12, &anim_P3switch[11]);
	MainPanelVC.AddSwitch(&ModeControlAGSSwitch, AID_SWITCH_P3_13, &anim_P3switch[12]);
	MainPanelVC.AddSwitch(&IMUCageSwitch, AID_SWITCH_P3_14, &anim_P3switch[13]);
	MainPanelVC.AddSwitch(&EventTimerCtlSwitch, AID_SWITCH_P3_15, &anim_P3switch[14]);
	MainPanelVC.AddSwitch(&EventTimerStartSwitch, AID_SWITCH_P3_16, &anim_P3switch[15]);
	MainPanelVC.AddSwitch(&EventTimerMinuteSwitch, AID_SWITCH_P3_17, &anim_P3switch[16]);
	MainPanelVC.AddSwitch(&EventTimerSecondSwitch, AID_SWITCH_P3_18, &anim_P3switch[17]);
	MainPanelVC.AddSwitch(&RCSSysQuad1Switch, AID_SWITCH_P3_19, &anim_P3switch[18]);
	MainPanelVC.AddSwitch(&RCSSysQuad4Switch, AID_SWITCH_P3_20, &anim_P3switch[19]);
	MainPanelVC.AddSwitch(&RCSSysQuad2Switch, AID_SWITCH_P3_21, &anim_P3switch[20]);
	MainPanelVC.AddSwitch(&RCSSysQuad3Switch, AID_SWITCH_P3_22, &anim_P3switch[21]);
	MainPanelVC.AddSwitch(&SidePanelsSwitch, AID_SWITCH_P3_23, &anim_P3switch[22]);
	MainPanelVC.AddSwitch(&FloodSwitch, AID_SWITCH_P3_24, &anim_P3switch[23]);
	MainPanelVC.AddSwitch(&RightXPointerSwitch, AID_SWITCH_P3_25, &anim_P3switch[24]);
	MainPanelVC.AddSwitch(&ExteriorLTGSwitch, AID_SWITCH_P3_26, &anim_P3switch[25]);

	MainPanelVC.AddSwitch(&TestMonitorRotary, AID_ROT_P3_01, &anim_P3_Rot[0]);
	MainPanelVC.AddSwitch(&RendezvousRadarRotary, AID_ROT_P3_02, &anim_P3_Rot[1]);
	MainPanelVC.AddSwitch(&TempMonitorRotary, AID_ROT_P3_03, &anim_P3_Rot[2]);
	MainPanelVC.AddSwitch(&LampToneTestRotary, AID_ROT_P3_04, &anim_P3_Rot[3]);
	MainPanelVC.AddSwitch(&FloodRotary, AID_ROT_P3_05, &anim_P3_Rot[4]);
}

bool LEM::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
{
	return MainPanelVC.VCMouseEvent(id, event, p);
}

bool LEM::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	return MainPanelVC.VCRedrawEvent(id, event, surf);
}

void LEM::InitSwitchesVC()
{
	int i = 0;

	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		anim_P3switch[P3_SWITCHCOUNT] = -1;
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		anim_P3_Rot[P3_ROTCOUNT] = -1;
	}
}

void LEM::DeleteSwitchesVC()
{
	int i = 0;

	for (i = 0; i < P3_SWITCHCOUNT; i++) delete mgt_P3switch[i];

	for (i = 0; i < P3_ROTCOUNT; i++) delete mgt_P3Rot[i];
}

void LEM::InitVCAnimations()
{
	UINT mesh = vcidx;
	int i = 0;

	// Define panel 3 animations
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
}

