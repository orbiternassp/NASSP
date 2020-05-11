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
		oapiVCRegisterArea(AID_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical(AID_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + ofs, 0.012);
	}

	for (i = 0; i < P3_DIALCOUNT; i++)
	{
		oapiVCRegisterArea(AID_DIAL_P3_00 + i, PANEL_REDRAW_MOUSE, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_DIAL_P3_00 + i, P3_DIAL_POS[i] + ofs, 0.02);
	}
}

bool LEM::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
{
	switch (id) {
	case AID_SWITCH_P3_01:
		EngGimbalEnableSwitch.ProcessMouseVC(event, p);
		return true;

	case AID_DIAL_P3_00:
		TestMonitorRotary.ProcessMouseVC(event, p);
		return true;
		//case ... // place response to other areas here
	}
	return false;
}

bool LEM::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	switch (id) {
	case AID_SWITCH_P3_01:
		EngGimbalEnableSwitch.RedrawVC(anim_P3switch[0]);
		return true;

	case AID_DIAL_P3_00:
		TestMonitorRotary.RedrawVC(anim_P3_Dial[0]);
		return true;
		//case ... // place response to other areas here
	}
	return false;
}

void LEM::InitSwitchesVC()
{
	int i = 0;

	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		anim_P3switch[P3_SWITCHCOUNT] = -1;
	}

	for (i = 0; i < P3_DIALCOUNT; i++)
	{
		anim_P3_Dial[P3_DIALCOUNT] = -1;
	}
}

void LEM::DeleteSwitchesVC()
{
	int i = 0;

	for (i = 0; i < P3_SWITCHCOUNT; i++) delete mgt_P3switch[i];

	for (i = 0; i < P3_DIALCOUNT; i++) delete mgt_P3Dial[i];
}

void LEM::InitVCAnimations()
{
	UINT mesh = vcidx;
	int i = 0;

	// Define panel 3 animations
	static UINT meshgroup_P3switches[P3_SWITCHCOUNT], meshgroup_P3dials[P3_DIALCOUNT];
	for (int i = 0; i < P3_SWITCHCOUNT; i++)
	{
		meshgroup_P3switches[i] = 21 + i;

		mgt_P3switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3switches[i], 1, P3_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P3switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P3switch[i], 0.0f, 1.0f, mgt_P3switch[i]);
	}

	for (i = 0; i < P3_DIALCOUNT; i++)
	{
		meshgroup_P3dials[i] = 22 + i;

		mgt_P3Dial[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3dials[i], 1, P3_DIAL_POS[i], P3_DIAL_AXIS, (float)(RAD * 360));
		anim_P3_Dial[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P3_Dial[i], 0.0f, 1.0f, mgt_P3Dial[i]);
	}
}

