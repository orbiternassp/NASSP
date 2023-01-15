/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  Mission Control Center Vessel Virtual cockpit code

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
#include <stdio.h>
#include <math.h>
#include "soundlib.h"
#include "nasspdefs.h"

#include "mccvessel.h"

MESHHANDLE hMCCVC;

GDIParams g_Param;

// MFD button area IDs
#define AID_MFD_L_BUTTONS		0
#define AID_MFD_R_BUTTONS		1
#define AID_MFD_1_BUTTONS		2
#define AID_MFD_2_BUTTONS		3

#define AID_MFD_L_BUTTON_1		4
#define AID_MFD_L_BUTTON_2		5
#define AID_MFD_L_BUTTON_3		6
#define AID_MFD_L_BUTTON_4		7
#define AID_MFD_L_BUTTON_5		8
#define AID_MFD_L_BUTTON_6		9
#define AID_MFD_L_BUTTON_7		10
#define AID_MFD_L_BUTTON_8		11
#define AID_MFD_L_BUTTON_9		12
#define AID_MFD_L_BUTTON_10		13
#define AID_MFD_L_BUTTON_11		14
#define AID_MFD_L_BUTTON_12		15
#define AID_MFD_L_BUTTON_13		16
#define AID_MFD_L_BUTTON_14		17
#define AID_MFD_L_BUTTON_15		18

#define AID_MFD_R_BUTTON_1		19
#define AID_MFD_R_BUTTON_2		20
#define AID_MFD_R_BUTTON_3		21
#define AID_MFD_R_BUTTON_4		22
#define AID_MFD_R_BUTTON_5		23
#define AID_MFD_R_BUTTON_6		24
#define AID_MFD_R_BUTTON_7		25
#define AID_MFD_R_BUTTON_8		26
#define AID_MFD_R_BUTTON_9		27
#define AID_MFD_R_BUTTON_10		28
#define AID_MFD_R_BUTTON_11		29
#define AID_MFD_R_BUTTON_12		30
#define AID_MFD_R_BUTTON_13		31
#define AID_MFD_R_BUTTON_14		32
#define AID_MFD_R_BUTTON_15		33

#define AID_MFD_1_BUTTON_1		34
#define AID_MFD_1_BUTTON_2		35
#define AID_MFD_1_BUTTON_3		36
#define AID_MFD_1_BUTTON_4		37
#define AID_MFD_1_BUTTON_5		38
#define AID_MFD_1_BUTTON_6		39
#define AID_MFD_1_BUTTON_7		40
#define AID_MFD_1_BUTTON_8		41
#define AID_MFD_1_BUTTON_9		42
#define AID_MFD_1_BUTTON_10		43
#define AID_MFD_1_BUTTON_11		44
#define AID_MFD_1_BUTTON_12		45
#define AID_MFD_1_BUTTON_13		46
#define AID_MFD_1_BUTTON_14		47
#define AID_MFD_1_BUTTON_15		48

#define AID_MFD_2_BUTTON_1		49
#define AID_MFD_2_BUTTON_2		50
#define AID_MFD_2_BUTTON_3		51
#define AID_MFD_2_BUTTON_4		52
#define AID_MFD_2_BUTTON_5		53
#define AID_MFD_2_BUTTON_6		54
#define AID_MFD_2_BUTTON_7		55
#define AID_MFD_2_BUTTON_8		56
#define AID_MFD_2_BUTTON_9		57
#define AID_MFD_2_BUTTON_10		58
#define AID_MFD_2_BUTTON_11		59
#define AID_MFD_2_BUTTON_12		60
#define AID_MFD_2_BUTTON_13		61
#define AID_MFD_2_BUTTON_14		62
#define AID_MFD_2_BUTTON_15		63

// MFD button positions
const VECTOR3 MFD_L_BUTTONPOS[15] = {
{-1.3348, -2.0244, 0.5637}, {-1.3348, -2.0901, 0.5339}, {-1.3348, -2.1558, 0.5041}, {-1.3348, -2.2215, 0.4744}, {-1.3348, -2.2872, 0.4446},
{-1.3348, -2.3529, 0.4148}, {-0.7605, -2.0244, 0.5637}, {-0.7605, -2.0901, 0.5339}, {-0.7605, -2.1558, 0.5041}, {-0.7605, -2.2215, 0.4744},
{-0.7605, -2.2872, 0.4446}, {-0.7605, -2.3529, 0.4148}, {-1.2480, -2.4408, 0.3750}, {-0.9127, -2.4407, 0.3751}, {-0.8444, -2.4407, 0.3751}
};

const VECTOR3 MFD_R_BUTTONPOS[15] = {
{-0.6350, -2.0244, 0.5637}, {-0.6350, -2.0901, 0.5339}, {-0.6350, -2.1558, 0.5041}, {-0.6350, -2.2215, 0.4744}, {-0.6350, -2.2872, 0.4446},
{-0.6350, -2.3529, 0.4148}, {-0.0607, -2.0244, 0.5637}, {-0.0607, -2.0901, 0.5339}, {-0.0607, -2.1558, 0.5041}, {-0.0607, -2.2215, 0.4744},
{-0.0607, -2.2872, 0.4446}, {-0.0607, -2.3529, 0.4148}, {-0.5481, -2.4408, 0.3750}, {-0.2129, -2.4407, 0.3751}, {-0.1446, -2.4407, 0.3751}
};

const VECTOR3 MFD_1_BUTTONPOS[15] = {
{0.0653, -2.0244, 0.5637}, {0.0653, -2.0901, 0.5339}, {0.0653, -2.1558, 0.5041}, {0.0653, -2.2215, 0.4744}, {0.0653, -2.2872, 0.4446},
{0.0653, -2.3529, 0.4148}, {0.6396, -2.0244, 0.5637}, {0.6396, -2.0901, 0.5339}, {0.6396, -2.1558, 0.5041}, {0.6396, -2.2215, 0.4744},
{0.6396, -2.2872, 0.4446}, {0.6396, -2.3529, 0.4148}, {0.1522, -2.4408, 0.3750}, {0.4874, -2.4407, 0.3751}, {0.5557, -2.4407, 0.3751}
};

const VECTOR3 MFD_2_BUTTONPOS[15] = {
{0.7640, -2.0244, 0.5637}, {0.7640, -2.0901, 0.5339}, {0.7640, -2.1558, 0.5041}, {0.7640, -2.2215, 0.4744}, {0.7640, -2.2872, 0.4446},
{0.7640, -2.3529, 0.4148}, {1.3383, -2.0244, 0.5637}, {1.3383, -2.0901, 0.5339}, {1.3383, -2.1558, 0.5041}, {1.3383, -2.2215, 0.4744},
{1.3383, -2.2872, 0.4446}, {1.3383, -2.3529, 0.4148}, {0.8509, -2.4408, 0.3750}, {1.1861, -2.4407, 0.3751}, {1.2544, -2.4407, 0.3751}
};

void MCCVessel::LoadVC() 
{
	int vcidx = AddMesh(hMCCVC);
	SetMeshVisibilityMode(vcidx, MESHVIS_VC);
}

bool MCCVessel::clbkLoadVC(int id) 
{
	SURFHANDLE FrontTex = oapiGetTextureHandle(hMCCVC, 2);

	static VCMFDSPEC mfds_left = { 0, 2 };
	oapiVCRegisterMFD(MFD_LEFT, &mfds_left);

	static VCMFDSPEC mfds_right = { 0, 3 };
	oapiVCRegisterMFD(MFD_RIGHT, &mfds_right);

	static VCMFDSPEC mfds_user1 = { 0, 4 };
	oapiVCRegisterMFD(MFD_USER1, &mfds_user1);

	static VCMFDSPEC mfds_user2 = { 0, 5 };
	oapiVCRegisterMFD(MFD_USER2, &mfds_user2);

	oapiVCRegisterArea(AID_MFD_L_BUTTONS, _R(1812, 5, 2043, 77), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, FrontTex);
	oapiVCRegisterArea(AID_MFD_R_BUTTONS, _R(1812, 87, 2043, 159), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, FrontTex);
	oapiVCRegisterArea(AID_MFD_1_BUTTONS, _R(1812, 169, 2043, 241), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, FrontTex);
	oapiVCRegisterArea(AID_MFD_2_BUTTONS, _R(1812, 251, 2043, 323), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, FrontTex);

	for (int i = 0; i < 15; i++) {
		oapiVCRegisterArea(AID_MFD_L_BUTTON_1 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical(AID_MFD_L_BUTTON_1 + i, MFD_L_BUTTONPOS[i], 0.02);

		oapiVCRegisterArea(AID_MFD_R_BUTTON_1 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical(AID_MFD_R_BUTTON_1 + i, MFD_R_BUTTONPOS[i], 0.02);

		oapiVCRegisterArea(AID_MFD_1_BUTTON_1 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical(AID_MFD_1_BUTTON_1 + i, MFD_1_BUTTONPOS[i], 0.02);

		oapiVCRegisterArea(AID_MFD_2_BUTTON_1 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN);
		oapiVCSetAreaClickmode_Spherical(AID_MFD_2_BUTTON_1 + i, MFD_2_BUTTONPOS[i], 0.02);
	}

	switch (id) {

	case 0:
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraDefaultDirection(_V(0.0, -sin(20 * RAD), cos(20 * RAD)));
		SetCameraMovement(_V(0, 0, 0), 0, 0, _V(-0.7, 0, 0), 0, 0, _V(0.7, 0, 0), 0, 0);
		SetCameraOffset(_V(0, -1.8, -0.31));
		return true;

	default:
		return false;
	}
}

bool MCCVessel::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	switch (id) {

	case AID_MFD_L_BUTTONS:
		RedrawPanel_MFDButton(surf, MFD_LEFT, 0, 0, 0, 40);
		RedrawPanel_MFDButton(surf, MFD_LEFT, 1, 0, 41, 40);
		return true;
	case AID_MFD_R_BUTTONS:
		RedrawPanel_MFDButton(surf, MFD_RIGHT, 0, 0, 0, 40);
		RedrawPanel_MFDButton(surf, MFD_RIGHT, 1, 0, 41, 40);
		return true;
	case AID_MFD_1_BUTTONS:
		RedrawPanel_MFDButton(surf, MFD_USER1, 0, 0, 0, 40);
		RedrawPanel_MFDButton(surf, MFD_USER1, 1, 0, 41, 40);
		return true;
	case AID_MFD_2_BUTTONS:
		RedrawPanel_MFDButton(surf, MFD_USER2, 0, 0, 0, 40);
		RedrawPanel_MFDButton(surf, MFD_USER2, 1, 0, 41, 40);
		return true;

	default:
		return false;
	}
}

bool MCCVessel::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
{
	if (id >= AID_MFD_L_BUTTON_1 && id < AID_MFD_L_BUTTON_1 + 12) {
		oapiProcessMFDButton(MFD_LEFT, id - AID_MFD_L_BUTTON_1, event);
		return true;
	}
	if (id >= AID_MFD_R_BUTTON_1 && id < AID_MFD_R_BUTTON_1 + 12) {
		oapiProcessMFDButton(MFD_RIGHT, id - AID_MFD_R_BUTTON_1, event);
		return true;
	}
	if (id >= AID_MFD_1_BUTTON_1 && id < AID_MFD_1_BUTTON_1 + 12) {
		oapiProcessMFDButton(MFD_USER1, id - AID_MFD_1_BUTTON_1, event);
		return true;
	}
	if (id >= AID_MFD_2_BUTTON_1 && id < AID_MFD_2_BUTTON_1 + 12) {
		oapiProcessMFDButton(MFD_USER2, id - AID_MFD_2_BUTTON_1, event);
		return true;
	}

	// Make power button always open the RTCC MFD by default
	char name[] = "Apollo RTCC MFD";
	int mfdId = oapiGetMFDModeSpecEx(name);

	if (id == AID_MFD_L_BUTTON_13) {
		if (oapiGetMFDMode(MFD_LEFT) == mfdId)
		{
			oapiToggleMFD_on(MFD_LEFT);
		}
		else
		{
			oapiOpenMFD(mfdId, MFD_LEFT);
		}
		return true;
	}
	if (id == AID_MFD_R_BUTTON_13) {
		if (oapiGetMFDMode(MFD_RIGHT) == mfdId)
		{
			oapiToggleMFD_on(MFD_RIGHT);
		}
		else
		{
			oapiOpenMFD(mfdId, MFD_RIGHT);
		}
		return true;
	}
	if (id == AID_MFD_1_BUTTON_13) {
		if (oapiGetMFDMode(MFD_USER1) == mfdId)
		{
			oapiToggleMFD_on(MFD_USER1);
		}
		else
		{
			oapiOpenMFD(mfdId, MFD_USER1);
		}
		return true;
	}
	if (id == AID_MFD_2_BUTTON_13) {
		if (oapiGetMFDMode(MFD_USER2) == mfdId)
		{
			oapiToggleMFD_on(MFD_USER2);
		}
		else
		{
			oapiOpenMFD(mfdId, MFD_USER2);
		}
		return true;
	}

	if (id == AID_MFD_L_BUTTON_14) {
		oapiSendMFDKey(MFD_LEFT, OAPI_KEY_F1);
		return true;
	}
	if (id == AID_MFD_R_BUTTON_14) {
		oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_F1);
		return true;
	}
	if (id == AID_MFD_1_BUTTON_14) {
		oapiSendMFDKey(MFD_USER1, OAPI_KEY_F1);
		return true;
	}
	if (id == AID_MFD_2_BUTTON_14) {
		oapiSendMFDKey(MFD_USER2, OAPI_KEY_F1);
		return true;
	}

	if (id == AID_MFD_L_BUTTON_15) {
		oapiSendMFDKey(MFD_LEFT, OAPI_KEY_GRAVE);
		return true;
	}
	if (id == AID_MFD_R_BUTTON_15) {
		oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_GRAVE);
		return true;
	}
	if (id == AID_MFD_1_BUTTON_15) {
		oapiSendMFDKey(MFD_USER1, OAPI_KEY_GRAVE);
		return true;
	}
	if (id == AID_MFD_2_BUTTON_15) {
		oapiSendMFDKey(MFD_USER2, OAPI_KEY_GRAVE);
		return true;
	}
	return false;
}

void MCCVessel::RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset, int xdist) {

	oapi::Sketchpad* skp = oapiGetSketchpad(surf);
	skp->SetFont(g_Param.font[2]);
	skp->SetTextColor(RGB(196, 196, 196));
	skp->SetTextAlign(oapi::Sketchpad::CENTER);
	skp->SetBackgroundMode(oapi::Sketchpad::BK_TRANSPARENT);
	const char* label;
	for (int bt = 0; bt < 6; bt++) {
		if (label = oapiMFDButtonLabel(mfd, bt + side * 6))
			skp->Text(16 + (xdist * bt) + xoffset, 7 + yoffset, label, strlen(label));
		else break;
	}
	oapiReleaseSketchpad(skp);
}

void MCCVessel::clbkMFDMode(int mfd, int mode)
{
	switch (mfd) {
	case MFD_LEFT:
		oapiVCTriggerRedrawArea(-1, AID_MFD_L_BUTTONS);
		break;
	case MFD_RIGHT:
		oapiVCTriggerRedrawArea(-1, AID_MFD_R_BUTTONS);
		break;
	case MFD_USER1:
		oapiVCTriggerRedrawArea(-1, AID_MFD_1_BUTTONS);
		break;
	case MFD_USER2:
		oapiVCTriggerRedrawArea(-1, AID_MFD_2_BUTTONS);
		break;
	}
}

void LoadMeshes()
{
	hMCCVC = oapiLoadMeshGlobal("ProjectApollo/MCC_VC");
}