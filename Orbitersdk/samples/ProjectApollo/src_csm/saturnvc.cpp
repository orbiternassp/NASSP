/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5
  Virtual cockpit code

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
#include <stdio.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"
#include "papi.h"
#include "CM_VC_Resource.h"

extern GDIParams g_Param;

void Saturn::InitVC()
{
	//int i;
	TRACESETUP("Saturn::InitVC");

	ReleaseSurfacesVC();

	DWORD ck = oapiGetColour(255, 0, 255);

	// Load CM VC surfaces

	srf[SRF_VC_DIGITALDISP] = oapiLoadTexture("ProjectApollo/VC/digitaldisp.dds");
	srf[SRF_VC_DIGITALDISP2] = oapiLoadTexture("ProjectApollo/VC/digitaldisp_2.dds");
	srf[SRF_VC_DSKYDISP] = oapiLoadTexture("ProjectApollo/VC/dsky_disp.dds");
	srf[SRF_VC_DSKY_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/dsky_lights.dds");
	srf[SRF_VC_MASTERALARM] = oapiLoadTexture("ProjectApollo/VC/masteralarmbright.dds");
	srf[SRF_VC_CW_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/cw_lights.dds");
	srf[SRF_VC_LVENGLIGHTS] = oapiLoadTexture("ProjectApollo/VC/lv_eng.dds");
	srf[SRF_VC_EVENT_TIMER_DIGITS] = oapiLoadTexture("ProjectApollo/VC/event_timer.dds");
	srf[SRF_VC_EMS_SCROLL_LEO] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_LEO.dds");
	srf[SRF_VC_EMS_SCROLL_BORDER] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_border.dds");
	srf[SRF_VC_EMS_SCROLL_BUG] = oapiLoadTexture("ProjectApollo/VC/EMSscroll_bug.dds");
	srf[SRF_VC_EMS_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/ems_lights.dds");
	srf[SRF_VC_INDICATOR] = oapiLoadTexture("ProjectApollo/VC/Indicator.dds");
	srf[SRF_VC_ECSINDICATOR] = oapiLoadTexture("ProjectApollo/VC/ECSIndicator.dds");
	srf[SRF_VC_SEQUENCERSWITCHES] = oapiLoadTexture("ProjectApollo/VC/SequencerSwitches.dds");

	//srf[SRF_INDICATORVC] = oapiLoadTexture("ProjectApollo/VC/Indicator.dds");
	//srf[SRF_INDICATORREDVC] = oapiLoadTexture("ProjectApollo/VC/IndicatorRed.dds");

	// Set Colour Key

	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP2], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKYDISP], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKY_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_MASTERALARM], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_CW_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_LVENGLIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_EMS_SCROLL_BORDER], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_EMS_SCROLL_BUG], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_EMS_LIGHTS], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_ECSINDICATOR], ck);
	oapiSetSurfaceColourKey(srf[SRF_VC_SEQUENCERSWITCHES], ck);


	//
	// Register active areas for repainting here
	//
		
	SURFHANDLE MainPanelTex1 = oapiGetTextureHandle(hCMVC, 12);
	SURFHANDLE MainPanelTex2 = oapiGetTextureHandle(hCMVC, 1);

	// Panel 1

	oapiVCRegisterArea(AID_VC_MASTER_ALARM, _R(1489, 1183, 1534, 1219), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_LVENG_LIGHTS, _R(1868, 1421, 1969, 1565), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(1891, 1356, 1962, 1374), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMS_SCROLL_LEO, _R(1756, 982, 1900, 1134), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SPS_LIGHT, _R(1840, 1142, 1881, 1158), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_PT05G_LIGHT, _R(1774, 1142, 1815, 1158), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMSDVDISPLAY, _R(1768, 1204, 1925, 1225), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_EMS_RSI_BKGRND, _R(1627, 1149, 1715, 1236), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_SEQUENCERSWITCHES, _R(1847, 1606, 1886, 1644), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	// Panel 2
	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(254, 1235, 359, 1411), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS, _R(110, 1240, 212, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(849, 949, 992, 972), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_CWS_LIGHTS_LEFT, _R(555, 743, 767, 1851), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_CWS_LIGHTS_RIGHT, _R(810, 743, 1023, 851), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	oapiVCRegisterArea(AID_VC_RCS_HELIUM1_TB, _R(606, 1014, 756, 1037), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_HELIUM2_TB, _R(606, 1172, 756, 1195), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_PROP1_TB, _R(606, 1305, 756, 1328), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_PROP2_TB, _R(518, 1438, 754, 1461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_ECSRADIATORIND, _R(814, 1329, 837, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DOCKINGPROBEIND, _R(411, 825, 434, 875), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	// Panel 3
	oapiVCRegisterArea(AID_VC_MASTER_ALARM2, _R(433, 1339, 478, 1375), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex2);

}

void Saturn::ReleaseSurfacesVC()
{
	for (int i = 0; i < nsurfvc; i++)
		if (srf[i]) {
			oapiDestroySurface(srf[i]);
			srf[i] = 0;
		}
}

bool Saturn::clbkLoadVC (int id)
{
	TRACESETUP("Saturn::clbkLoadVC");

	InVC = true;
	InPanel = false;

	//if ((viewpos >= SATVIEW_ENG1) && (viewpos <= SATVIEW_ENG6))
	//	return true;

	// Test stuff
	//SURFHANDLE tex# = oapiGetTextureHandle (vcmeshidentifier, meshgroup#);
	//int i;
	//SetCameraDefaultDirection (_V(0,0,1));
	//default camera direction: forward
	//SetCameraShiftRange (_V(#,#,#), _V(#,#,#), _V(#,#,#));
	// leaning forward/left/right

	InitVC();
	RegisterActiveAreas();

	switch (id) {

	case SATVIEW_LEFTSEAT:
		viewpos = SATVIEW_LEFTSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, SATVIEW_CENTERSEAT, -1, -1);
		SetView(true);


		return true;

	case SATVIEW_CENTERSEAT:
		viewpos = SATVIEW_CENTERSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(SATVIEW_LEFTSEAT, SATVIEW_RIGHTSEAT, -1, -1);
		SetView(true);


		return true;

	case SATVIEW_RIGHTSEAT:
		viewpos = SATVIEW_RIGHTSEAT;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(SATVIEW_CENTERSEAT, -1, -1, -1);
		SetView(true);


		return true;

	case SATVIEW_GNPANEL:
		viewpos = SATVIEW_GNPANEL;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.8 * PI, 0.4 * PI);
		SetCameraShiftRange(_V(-0.4, 0, 0), _V(0, 0, 0), _V(0, 0, 0));

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		//oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);



		return true;

	case SATVIEW_LEFTDOCK:
		viewpos = SATVIEW_LEFTDOCK;
		SetCameraRotationRange(0, 0, 0, 0);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		//oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);



		return true;

	case SATVIEW_RIGHTDOCK: // Limited eye movement
		viewpos = SATVIEW_RIGHTDOCK;
		SetCameraRotationRange(0, 0, 0, 0);

		//SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		//oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);

		return true;

	default:
		return false;
	}
}

void Saturn::RegisterActiveAreas() {

	int i = 0;

	VECTOR3 ofs = _V(0.0, 0.0, 0.0);
	//if (vcidx != -1) GetMeshOffset(vcidx, ofs);

	if (stage < 12) {
		ofs.z = 43.65;
	} else if (stage < 20) {
		ofs.z = 28.5;
	} else if (stage < 30) {
		ofs.z = 15.25;
	} else if (stage < 40) {
		ofs.z = 2.1;
	} else if (stage < 42) {
		ofs.z = 0.0;
	} else {
		ofs.z = -1.2;
	}

	sprintf(oapiDebugString(), "OFS %lf", ofs.z);

	MainPanelVC.ClearSwitches();

	//
	// Register active areas for switches/animations here
	//

	// FDAI's
	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	// Panel 1
	for (i = 0; i < P1_SWITCHCOUNT_C; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P1_01 + i, P1_TOGGLE_POS_C[i] + P1_3_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P1_ROTCOUNT_C; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P1_01 + i, P1_ROT_POS_C[i] + ofs, 0.02);
	}

	for (i = 0; i < P1_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P1_01 + i, P1_PUSHB_POS[i] + ofs, 0.008);
	}

	oapiVCRegisterArea(AID_VC_EMS_DVSET, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_EMS_DVSET, _V(-0.507344, 0.732746, 0.370513) + ofs, 0.025);

	oapiVCSetAreaClickmode_Spherical(AID_VC_MASTER_ALARM, _V(-0.775435, 0.709185, 0.361746) + ofs, 0.008);

	/*MainPanelVC.AddSwitch(&EMSModeSwitch, AID_VC_SWITCH_P1_01, &anim_P1switch[0]);
	MainPanelVC.AddSwitch(&CMCAttSwitch, AID_VC_SWITCH_P1_02, &anim_P1switch[1]);
	MainPanelVC.AddSwitch(&FDAIScaleSwitch, AID_VC_SWITCH_P1_03, &anim_P1switch[2]);
	MainPanelVC.AddSwitch(&FDAISelectSwitch, AID_VC_SWITCH_P1_04, &anim_P1switch[3]);
	MainPanelVC.AddSwitch(&FDAISourceSwitch, AID_VC_SWITCH_P1_05, &anim_P1switch[4]);
	MainPanelVC.AddSwitch(&FDAIAttSetSwitch, AID_VC_SWITCH_P1_06, &anim_P1switch[5]);
	MainPanelVC.AddSwitch(&ManualAttRollSwitch, AID_VC_SWITCH_P1_07, &anim_P1switch[6]);
	MainPanelVC.AddSwitch(&ManualAttPitchSwitch, AID_VC_SWITCH_P1_08, &anim_P1switch[7]);
	MainPanelVC.AddSwitch(&ManualAttYawSwitch, AID_VC_SWITCH_P1_09, &anim_P1switch[8]);
	MainPanelVC.AddSwitch(&LimitCycleSwitch, AID_VC_SWITCH_P1_10, &anim_P1switch[9]);
	MainPanelVC.AddSwitch(&AttDeadbandSwitch, AID_VC_SWITCH_P1_11, &anim_P1switch[10]);
	MainPanelVC.AddSwitch(&AttRateSwitch, AID_VC_SWITCH_P1_12, &anim_P1switch[11]);
	MainPanelVC.AddSwitch(&TransContrSwitch, AID_VC_SWITCH_P1_13, &anim_P1switch[12]);
	MainPanelVC.AddSwitch(&RotPowerNormal1Switch, AID_VC_SWITCH_P1_14, &anim_P1switch[13]);
	MainPanelVC.AddSwitch(&RotPowerNormal2Switch, AID_VC_SWITCH_P1_15, &anim_P1switch[14]);
	MainPanelVC.AddSwitch(&RotPowerDirect1Switch, AID_VC_SWITCH_P1_16, &anim_P1switch[15]);
	MainPanelVC.AddSwitch(&RotPowerDirect2Switch, AID_VC_SWITCH_P1_17, &anim_P1switch[16]);
	MainPanelVC.AddSwitch(&SCContSwitch, AID_VC_SWITCH_P1_18, &anim_P1switch[17]);
	MainPanelVC.AddSwitch(&CMCModeSwitch, AID_VC_SWITCH_P1_19, &anim_P1switch[18]);
	MainPanelVC.AddSwitch(&BMAGRollSwitch, AID_VC_SWITCH_P1_20, &anim_P1switch[19]);
	MainPanelVC.AddSwitch(&BMAGPitchSwitch, AID_VC_SWITCH_P1_21, &anim_P1switch[20]);
	MainPanelVC.AddSwitch(&BMAGYawSwitch, AID_VC_SWITCH_P1_22, &anim_P1switch[21]);
	MainPanelVC.AddSwitch(&dVThrust1Switch, AID_VC_SWITCH_P1_23, &anim_P1switch[22]);
	MainPanelVC.AddSwitch(&dVThrust2Switch, AID_VC_SWITCH_P1_24, &anim_P1switch[23]);
	MainPanelVC.AddSwitch(&SCSTvcPitchSwitch, AID_VC_SWITCH_P1_25, &anim_P1switch[24]);
	MainPanelVC.AddSwitch(&SCSTvcYawSwitch, AID_VC_SWITCH_P1_26, &anim_P1switch[25]);
	MainPanelVC.AddSwitch(&Pitch1Switch, AID_VC_SWITCH_P1_27, &anim_P1switch[26]);
	MainPanelVC.AddSwitch(&Pitch2Switch, AID_VC_SWITCH_P1_28, &anim_P1switch[27]);
	MainPanelVC.AddSwitch(&Yaw1Switch, AID_VC_SWITCH_P1_29, &anim_P1switch[28]);
	MainPanelVC.AddSwitch(&Yaw2Switch, AID_VC_SWITCH_P1_30, &anim_P1switch[29]);
	MainPanelVC.AddSwitch(&CGSwitch, AID_VC_SWITCH_P1_31, &anim_P1switch[30]);
	MainPanelVC.AddSwitch(&ELSLogicSwitch, AID_VC_SWITCH_P1_32, &anim_P1switch[31]);
	MainPanelVC.AddSwitch(&ELSAutoSwitch, AID_VC_SWITCH_P1_33, &anim_P1switch[32]);
	MainPanelVC.AddSwitch(&CMRCSLogicSwitch, AID_VC_SWITCH_P1_34, &anim_P1switch[33]);
	MainPanelVC.AddSwitch(&CMPropDumpSwitch, AID_VC_SWITCH_P1_35, &anim_P1switch[34]);
	MainPanelVC.AddSwitch(&CMPropPurgeSwitch, AID_VC_SWITCH_P1_36, &anim_P1switch[35]);
	MainPanelVC.AddSwitch(&IMUGuardedCageSwitch, AID_VC_SWITCH_P1_37, &anim_P1switch[36]);
	MainPanelVC.AddSwitch(&EMSRollSwitch, AID_VC_SWITCH_P1_38, &anim_P1switch[37]);
	MainPanelVC.AddSwitch(&GSwitch, AID_VC_SWITCH_P1_39, &anim_P1switch[38]);
	MainPanelVC.AddSwitch(&LVSPSPcIndicatorSwitch, AID_VC_SWITCH_P1_40, &anim_P1switch[39]);
	MainPanelVC.AddSwitch(&LVFuelTankPressIndicatorSwitch, AID_VC_SWITCH_P1_41, &anim_P1switch[40]);
	MainPanelVC.AddSwitch(&TVCGimbalDrivePitchSwitch, AID_VC_SWITCH_P1_42, &anim_P1switch[41]);
	MainPanelVC.AddSwitch(&TVCGimbalDriveYawSwitch, AID_VC_SWITCH_P1_43, &anim_P1switch[42]);
	MainPanelVC.AddSwitch(&EventTimerUpDownSwitch, AID_VC_SWITCH_P1_44, &anim_P1switch[43]);
	MainPanelVC.AddSwitch(&EventTimerContSwitch, AID_VC_SWITCH_P1_45, &anim_P1switch[44]);
	MainPanelVC.AddSwitch(&EventTimerMinutesSwitch, AID_VC_SWITCH_P1_46, &anim_P1switch[45]);
	MainPanelVC.AddSwitch(&EventTimerSecondsSwitch, AID_VC_SWITCH_P1_47, &anim_P1switch[46]);
	MainPanelVC.AddSwitch(&GTASwitch, AID_VC_SWITCH_P1_48, &anim_P1switch[47]);

	MainPanelVC.AddSwitch(&EMSFunctionSwitch, AID_VC_ROT_P1_01, &anim_P1rot[0]);

	MainPanelVC.AddSwitch(&LiftoffNoAutoAbortSwitch, AID_VC_PUSHB_P1_01, &anim_P1pushbuttons[0]);
	MainPanelVC.AddSwitch(&LesMotorFireSwitch, AID_VC_PUSHB_P1_02, &anim_P1pushbuttons[1]);
	MainPanelVC.AddSwitch(&CanardDeploySwitch, AID_VC_PUSHB_P1_03, &anim_P1pushbuttons[2]);
	MainPanelVC.AddSwitch(&CsmLvSepSwitch, AID_VC_PUSHB_P1_04, &anim_P1pushbuttons[3]);
	MainPanelVC.AddSwitch(&ApexCoverJettSwitch, AID_VC_PUSHB_P1_05, &anim_P1pushbuttons[4]);
	MainPanelVC.AddSwitch(&DrogueDeploySwitch, AID_VC_PUSHB_P1_06, &anim_P1pushbuttons[5]);
	MainPanelVC.AddSwitch(&MainDeploySwitch, AID_VC_PUSHB_P1_07, &anim_P1pushbuttons[6]);
	MainPanelVC.AddSwitch(&CmRcsHeDumpSwitch, AID_VC_PUSHB_P1_08, &anim_P1pushbuttons[7]);

	//Panel 2

	for (i = 0; i < P2_SWITCHCOUNT_C; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P2_01 + i, P2_TOGGLE_POS_C[i] + P1_3_CLICK + ofs, 0.008);
	}

	for (i = 0; i < P2_ROTCOUNT_C; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P2_01 + i, P2_ROT_POS_C[i] + ofs, 0.02);
	}

	for (i = 0; i < P2_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P2_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P2_01 + i, P2_PUSHB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&EDSSwitch, AID_VC_SWITCH_P2_01, &anim_P2switch[0]);
	MainPanelVC.AddSwitch(&CsmLmFinalSep1Switch, AID_VC_SWITCH_P2_02, &anim_P2switch[1]);
	MainPanelVC.AddSwitch(&CsmLmFinalSep2Switch, AID_VC_SWITCH_P2_03, &anim_P2switch[2]);
	MainPanelVC.AddSwitch(&CmSmSep1Switch, AID_VC_SWITCH_P2_04, &anim_P2switch[3]);
	MainPanelVC.AddSwitch(&CmSmSep2Switch, AID_VC_SWITCH_P2_05, &anim_P2switch[4]);
	MainPanelVC.AddSwitch(&SIVBPayloadSepSwitch, AID_VC_SWITCH_P2_06, &anim_P2switch[5]);
	MainPanelVC.AddSwitch(&PropDumpAutoSwitch, AID_VC_SWITCH_P2_07, &anim_P2switch[6]);
	MainPanelVC.AddSwitch(&TwoEngineOutAutoSwitch, AID_VC_SWITCH_P2_08, &anim_P2switch[7]);
	MainPanelVC.AddSwitch(&LVRateAutoSwitch, AID_VC_SWITCH_P2_09, &anim_P2switch[8]);
	MainPanelVC.AddSwitch(&TowerJett1Switch, AID_VC_SWITCH_P2_10, &anim_P2switch[9]);
	MainPanelVC.AddSwitch(&TowerJett2Switch, AID_VC_SWITCH_P2_11, &anim_P2switch[10]);
	MainPanelVC.AddSwitch(&LVGuidanceSwitch, AID_VC_SWITCH_P2_12, &anim_P2switch[11]);
	MainPanelVC.AddSwitch(&SIISIVBSepSwitch, AID_VC_SWITCH_P2_13, &anim_P2switch[12]);
	MainPanelVC.AddSwitch(&TLIEnableSwitch, AID_VC_SWITCH_P2_14, &anim_P2switch[13]);
	MainPanelVC.AddSwitch(&MainReleaseSwitch, AID_VC_SWITCH_P2_15, &anim_P2switch[14]);
	MainPanelVC.AddSwitch(&DockingProbeExtdRelSwitch, AID_VC_SWITCH_P2_16, &anim_P2switch[15]);
	MainPanelVC.AddSwitch(&DockingProbeRetractPrimSwitch, AID_VC_SWITCH_P2_17, &anim_P2switch[16]);
	MainPanelVC.AddSwitch(&DockingProbeRetractSecSwitch, AID_VC_SWITCH_P2_18, &anim_P2switch[17]);
	MainPanelVC.AddSwitch(&RunEVALightSwitch, AID_VC_SWITCH_P2_19, &anim_P2switch[18]);
	MainPanelVC.AddSwitch(&RndzLightSwitch, AID_VC_SWITCH_P2_20, &anim_P2switch[19]);
	MainPanelVC.AddSwitch(&TunnelLightSwitch, AID_VC_SWITCH_P2_21, &anim_P2switch[20]);
	MainPanelVC.AddSwitch(&LMPowerSwitch, AID_VC_SWITCH_P2_22, &anim_P2switch[21]);
	MainPanelVC.AddSwitch(&SMRCSHelium1ASwitch, AID_VC_SWITCH_P2_23, &anim_P2switch[22]);
	MainPanelVC.AddSwitch(&SMRCSHelium1BSwitch, AID_VC_SWITCH_P2_24, &anim_P2switch[23]);
	MainPanelVC.AddSwitch(&SMRCSHelium1CSwitch, AID_VC_SWITCH_P2_25, &anim_P2switch[24]);
	MainPanelVC.AddSwitch(&SMRCSHelium1DSwitch, AID_VC_SWITCH_P2_26, &anim_P2switch[25]);
	MainPanelVC.AddSwitch(&CMUplinkSwitch, AID_VC_SWITCH_P2_27, &anim_P2switch[26]);
	MainPanelVC.AddSwitch(&IUUplinkSwitch, AID_VC_SWITCH_P2_28, &anim_P2switch[27]);
	MainPanelVC.AddSwitch(&CMRCSPressSwitch, AID_VC_SWITCH_P2_29, &anim_P2switch[28]);
	MainPanelVC.AddSwitch(&SMRCSIndSwitch, AID_VC_SWITCH_P2_30, &anim_P2switch[29]);
	MainPanelVC.AddSwitch(&SMRCSHelium2ASwitch, AID_VC_SWITCH_P2_31, &anim_P2switch[30]);
	MainPanelVC.AddSwitch(&SMRCSHelium2BSwitch, AID_VC_SWITCH_P2_32, &anim_P2switch[31]);
	MainPanelVC.AddSwitch(&SMRCSHelium2CSwitch, AID_VC_SWITCH_P2_33, &anim_P2switch[32]);
	MainPanelVC.AddSwitch(&SMRCSHelium2DSwitch, AID_VC_SWITCH_P2_34, &anim_P2switch[33]);
	MainPanelVC.AddSwitch(&SMRCSHeaterASwitch, AID_VC_SWITCH_P2_35, &anim_P2switch[34]);
	MainPanelVC.AddSwitch(&SMRCSHeaterBSwitch, AID_VC_SWITCH_P2_36, &anim_P2switch[35]);
	MainPanelVC.AddSwitch(&SMRCSHeaterCSwitch, AID_VC_SWITCH_P2_37, &anim_P2switch[36]);
	MainPanelVC.AddSwitch(&SMRCSHeaterDSwitch, AID_VC_SWITCH_P2_38, &anim_P2switch[37]);
	MainPanelVC.AddSwitch(&SMRCSProp1ASwitch, AID_VC_SWITCH_P2_39, &anim_P2switch[38]);
	MainPanelVC.AddSwitch(&SMRCSProp1BSwitch, AID_VC_SWITCH_P2_40, &anim_P2switch[39]);
	MainPanelVC.AddSwitch(&SMRCSProp1CSwitch, AID_VC_SWITCH_P2_41, &anim_P2switch[40]);
	MainPanelVC.AddSwitch(&SMRCSProp1DSwitch, AID_VC_SWITCH_P2_42, &anim_P2switch[41]);
	MainPanelVC.AddSwitch(&RCSCMDSwitch, AID_VC_SWITCH_P2_43, &anim_P2switch[42]);
	MainPanelVC.AddSwitch(&RCSTrnfrSwitch, AID_VC_SWITCH_P2_44, &anim_P2switch[43]);
	MainPanelVC.AddSwitch(&CMRCSProp1Switch, AID_VC_SWITCH_P2_45, &anim_P2switch[44]);
	MainPanelVC.AddSwitch(&CMRCSProp2Switch, AID_VC_SWITCH_P2_46, &anim_P2switch[45]);
	MainPanelVC.AddSwitch(&SMRCSProp2ASwitch, AID_VC_SWITCH_P2_47, &anim_P2switch[46]);
	MainPanelVC.AddSwitch(&SMRCSProp2BSwitch, AID_VC_SWITCH_P2_48, &anim_P2switch[47]);
	MainPanelVC.AddSwitch(&SMRCSProp2CSwitch, AID_VC_SWITCH_P2_49, &anim_P2switch[48]);
	MainPanelVC.AddSwitch(&SMRCSProp2DSwitch, AID_VC_SWITCH_P2_50, &anim_P2switch[49]);
	MainPanelVC.AddSwitch(&MissionTimerHoursSwitch, AID_VC_SWITCH_P2_51, &anim_P2switch[50]);
	MainPanelVC.AddSwitch(&MissionTimerMinutesSwitch, AID_VC_SWITCH_P2_52, &anim_P2switch[51]);
	MainPanelVC.AddSwitch(&MissionTimerSecondsSwitch, AID_VC_SWITCH_P2_53, &anim_P2switch[52]);
	MainPanelVC.AddSwitch(&CautionWarningModeSwitch, AID_VC_SWITCH_P2_54, &anim_P2switch[53]);
	MainPanelVC.AddSwitch(&CautionWarningCMCSMSwitch, AID_VC_SWITCH_P2_55, &anim_P2switch[54]);
	MainPanelVC.AddSwitch(&CautionWarningPowerSwitch, AID_VC_SWITCH_P2_56, &anim_P2switch[55]);
	MainPanelVC.AddSwitch(&CautionWarningLightTestSwitch, AID_VC_SWITCH_P2_57, &anim_P2switch[56]);
	MainPanelVC.AddSwitch(&MissionTimerSwitch, AID_VC_SWITCH_P2_58, &anim_P2switch[57]);
	MainPanelVC.AddSwitch(&CabinFan1Switch, AID_VC_SWITCH_P2_59, &anim_P2switch[58]);
	MainPanelVC.AddSwitch(&CabinFan2Switch, AID_VC_SWITCH_P2_60, &anim_P2switch[59]);
	MainPanelVC.AddSwitch(&H2Heater1Switch, AID_VC_SWITCH_P2_61, &anim_P2switch[60]);
	MainPanelVC.AddSwitch(&H2Heater2Switch, AID_VC_SWITCH_P2_62, &anim_P2switch[61]);
	MainPanelVC.AddSwitch(&O2Heater1Switch, AID_VC_SWITCH_P2_63, &anim_P2switch[62]);
	MainPanelVC.AddSwitch(&O2Heater2Switch, AID_VC_SWITCH_P2_64, &anim_P2switch[63]);
	MainPanelVC.AddSwitch(&O2PressIndSwitch, AID_VC_SWITCH_P2_65, &anim_P2switch[64]);
	MainPanelVC.AddSwitch(&H2Fan1Switch, AID_VC_SWITCH_P2_66, &anim_P2switch[65]);
	MainPanelVC.AddSwitch(&H2Fan2Switch, AID_VC_SWITCH_P2_67, &anim_P2switch[66]);
	MainPanelVC.AddSwitch(&O2Fan1Switch, AID_VC_SWITCH_P2_68, &anim_P2switch[67]);
	MainPanelVC.AddSwitch(&O2Fan2Switch, AID_VC_SWITCH_P2_69, &anim_P2switch[68]);
	MainPanelVC.AddSwitch(&EcsRadiatorsFlowContAutoSwitch, AID_VC_SWITCH_P2_70, &anim_P2switch[69]);
	MainPanelVC.AddSwitch(&EcsRadiatorsFlowContPwrSwitch, AID_VC_SWITCH_P2_71, &anim_P2switch[70]);
	MainPanelVC.AddSwitch(&EcsRadiatorsManSelSwitch, AID_VC_SWITCH_P2_72, &anim_P2switch[71]);
	MainPanelVC.AddSwitch(&EcsRadiatorsHeaterPrimSwitch, AID_VC_SWITCH_P2_73, &anim_P2switch[72]);
	MainPanelVC.AddSwitch(&EcsRadiatorsHeaterSecSwitch, AID_VC_SWITCH_P2_74, &anim_P2switch[73]);
	MainPanelVC.AddSwitch(&PotH2oHtrSwitch, AID_VC_SWITCH_P2_75, &anim_P2switch[74]);
	MainPanelVC.AddSwitch(&SuitCircuitH2oAccumAutoSwitch, AID_VC_SWITCH_P2_76, &anim_P2switch[75]);
	MainPanelVC.AddSwitch(&SuitCircuitH2oAccumOnSwitch, AID_VC_SWITCH_P2_77, &anim_P2switch[76]);
	MainPanelVC.AddSwitch(&SuitCircuitHeatExchSwitch, AID_VC_SWITCH_P2_78, &anim_P2switch[77]);
	MainPanelVC.AddSwitch(&SecCoolantLoopEvapSwitch, AID_VC_SWITCH_P2_79, &anim_P2switch[78]);
	MainPanelVC.AddSwitch(&SecCoolantLoopPumpSwitch, AID_VC_SWITCH_P2_80, &anim_P2switch[79]);
	MainPanelVC.AddSwitch(&H2oQtyIndSwitch, AID_VC_SWITCH_P2_81, &anim_P2switch[80]);
	MainPanelVC.AddSwitch(&GlycolEvapTempInSwitch, AID_VC_SWITCH_P2_82, &anim_P2switch[81]);
	MainPanelVC.AddSwitch(&GlycolEvapSteamPressAutoManSwitch, AID_VC_SWITCH_P2_83, &anim_P2switch[82]);
	MainPanelVC.AddSwitch(&GlycolEvapSteamPressIncrDecrSwitch, AID_VC_SWITCH_P2_84, &anim_P2switch[83]);
	MainPanelVC.AddSwitch(&GlycolEvapH2oFlowSwitch, AID_VC_SWITCH_P2_85, &anim_P2switch[84]);
	MainPanelVC.AddSwitch(&CabinTempAutoManSwitch, AID_VC_SWITCH_P2_86, &anim_P2switch[85]);
	MainPanelVC.AddSwitch(&GHATrackSwitch, AID_VC_SWITCH_P2_87, &anim_P2switch[86]);
	MainPanelVC.AddSwitch(&GHABeamSwitch, AID_VC_SWITCH_P2_88, &anim_P2switch[87]);
	MainPanelVC.AddSwitch(&GHAPowerSwitch, AID_VC_SWITCH_P2_89, &anim_P2switch[88]);
	MainPanelVC.AddSwitch(&GHAServoElecSwitch, AID_VC_SWITCH_P2_90, &anim_P2switch[89]);

	MainPanelVC.AddSwitch(&DskySwitchVerb, AID_VC_PUSHB_P2_01, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P2_02, NULL);
	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P2_03, NULL);
	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P2_04, NULL);
	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P2_05, NULL);
	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P2_06, NULL);
	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P2_07, NULL);
	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P2_08, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P2_09, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P2_10, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P2_11, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P2_12, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P2_13, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P2_14, NULL);
	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P2_15, NULL);
	MainPanelVC.AddSwitch(&DskySwitchProg, AID_VC_PUSHB_P2_16, NULL);
	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P2_17, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P2_18, NULL);
	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P2_19, NULL);

	MainPanelVC.AddSwitch(&RCSIndicatorsSwitch, AID_VC_ROT_P2_01, &anim_P2rot[0]);
	MainPanelVC.AddSwitch(&ECSIndicatorsSwitch, AID_VC_ROT_P2_02, &anim_P2rot[1]);
	MainPanelVC.AddSwitch(&HighGainAntennaPitchPositionSwitch, AID_VC_ROT_P2_03, &anim_P2rot[2]);
	MainPanelVC.AddSwitch(&HighGainAntennaYawPositionSwitch, AID_VC_ROT_P2_04, &anim_P2rot[3]);*/

	// Panel 3

	oapiVCSetAreaClickmode_Spherical(AID_VC_MASTER_ALARM2, _V(0.720346, 0.621423, 0.332349) + ofs, 0.008);

	// Switch Covers
	/*IMUGuardedCageSwitch.InitGuardVC(anim_switchcovers[0]);
	dVThrust1Switch.InitGuardVC(anim_switchcovers[1]);
	dVThrust2Switch.InitGuardVC(anim_switchcovers[2]);
	ELSLogicSwitch.InitGuardVC(anim_switchcovers[3]);
	CMPropDumpSwitch.InitGuardVC(anim_switchcovers[4]);
	CMPropPurgeSwitch.InitGuardVC(anim_switchcovers[5]);
	CsmLmFinalSep1Switch.InitGuardVC(anim_switchcovers[6]);
	CsmLmFinalSep2Switch.InitGuardVC(anim_switchcovers[7]);
	CmSmSep1Switch.InitGuardVC(anim_switchcovers[8]);
	CmSmSep2Switch.InitGuardVC(anim_switchcovers[9]);
	SIVBPayloadSepSwitch.InitGuardVC(anim_switchcovers[10]);
	TowerJett1Switch.InitGuardVC(anim_switchcovers[11]);
	TowerJett2Switch.InitGuardVC(anim_switchcovers[12]);
	LVGuidanceSwitch.InitGuardVC(anim_switchcovers[13]);
	SIISIVBSepSwitch.InitGuardVC(anim_switchcovers[14]);
	MainReleaseSwitch.InitGuardVC(anim_switchcovers[15]);
	DockingProbeExtdRelSwitch.InitGuardVC(anim_switchcovers[16]);
	CMRCSPressSwitch.InitGuardVC(anim_switchcovers[17]);
	GTASwitch.InitGuardVC(anim_switchcovers[18]);*/

	// SEQS switch covers
	/*LiftoffNoAutoAbortSwitch.InitGuardVC(anim_switchcovers[19]);
	LesMotorFireSwitch.InitGuardVC(anim_switchcovers[20]);
	CanardDeploySwitch.InitGuardVC(anim_switchcovers[21]);
	CsmLvSepSwitch.InitGuardVC(anim_switchcovers[22]);
	ApexCoverJettSwitch.InitGuardVC(anim_switchcovers[23]);
	DrogueDeploySwitch.InitGuardVC(anim_switchcovers[24]);
	MainDeploySwitch.InitGuardVC(anim_switchcovers[25]);
	CmRcsHeDumpSwitch.InitGuardVC(anim_switchcovers[26]);*/
}

// --------------------------------------------------------------
// Respond to virtual cockpit mouse event
// --------------------------------------------------------------
bool Saturn::clbkVCMouseEvent (int id, int event, VECTOR3 &p)
{
	TRACESETUP("Saturn::clbkVCMouseEvent");
	switch (id) {

	case AID_VC_MASTER_ALARM:
	case AID_VC_MASTER_ALARM2:
	//case AID_VC_MASTER_ALARM3:

		return cws.CheckMasterAlarmMouseClick(event);

	case AID_VC_EMS_DVSET:
		return EMSDvSetSwitch.CheckMouseClickVC(event, p);
	}
	return MainPanelVC.VCMouseEvent(id, event, p);
	//return false;
}

// --------------------------------------------------------------
// Respond to virtual cockpit area redraw request
// --------------------------------------------------------------
bool Saturn::clbkVCRedrawEvent (int id, int event, SURFHANDLE surf)
{
	TRACESETUP("Saturn::clbkVCRedrawEvent");
	//int i;

	switch (id) {
	//case areaidentifier
	//	Redraw Panel stuff
	//	return true if dynamic texture modified, false if not

	case AID_VC_FDAI_LEFT:

		VECTOR3 euler_rates_L;
		VECTOR3 attitude_L;
		VECTOR3 errors_L;

		euler_rates_L = eda.GetFDAI1AttitudeRate();
		attitude_L = eda.GetFDAI1Attitude();
		errors_L = eda.GetFDAI1AttitudeError();

		AnimateFDAI(attitude_L, euler_rates_L, errors_L, anim_fdaiR_L, anim_fdaiP_L, anim_fdaiY_L, anim_fdaiRerror_L, anim_fdaiPerror_L, anim_fdaiYerror_L, anim_fdaiRrate_L, anim_fdaiPrate_L, anim_fdaiYrate_L);

		return true;

	case AID_VC_FDAI_RIGHT:

		VECTOR3 euler_rates_R;
		VECTOR3 attitude_R;
		VECTOR3 errors_R;

		euler_rates_R = eda.GetFDAI2AttitudeRate();
		attitude_R = eda.GetFDAI2Attitude();
		errors_R = eda.GetFDAI2AttitudeError();

		AnimateFDAI(attitude_R, euler_rates_R, errors_R, anim_fdaiR_R, anim_fdaiP_R, anim_fdaiY_R, anim_fdaiRerror_R, anim_fdaiPerror_R, anim_fdaiYerror_R, anim_fdaiRrate_R, anim_fdaiPrate_R, anim_fdaiYrate_R);

		return true;

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP]);
		return true;

	case AID_VC_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS]);
		return true;

	/*case AID_ABORT_BUTTON:
		if (ABORT_IND) {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 62, 0, 62, 31);
		}
		else {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 0, 0, 62, 31);
		}
		return true;*/
		
	case AID_VC_MASTER_ALARM:
		cws.RenderMasterAlarm(surf, srf[SRF_VC_MASTERALARM], NULL, CWS_MASTERALARMPOSITION_LEFT);
		return true;

	case AID_VC_MASTER_ALARM2:
		cws.RenderMasterAlarm(surf, srf[SRF_VC_MASTERALARM], NULL, CWS_MASTERALARMPOSITION_RIGHT);
		return true;

	case AID_VC_LVENG_LIGHTS:
	{
		if (SI_EngineNum > 5)
		{
			/*RenderS1bEngineLight(ENGIND[0], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 64, 42);
			RenderS1bEngineLight(ENGIND[1], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 64, 98);
			RenderS1bEngineLight(ENGIND[2], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 8, 98);
			RenderS1bEngineLight(ENGIND[3], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 7, 43);
			RenderS1bEngineLight(ENGIND[4], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 36, 41);
			RenderS1bEngineLight(ENGIND[5], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 51, 69);
			RenderS1bEngineLight(ENGIND[6], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 36, 98);
			RenderS1bEngineLight(ENGIND[7], surf, srf[SRF_VC_LVENGLIGHTS_S1B], 22, 69);*/
		}
		else
		{
			if (ENGIND[0])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 44, 55, 44, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 44, 157, 44, 27, 27);
			}

			if (ENGIND[1])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 98, 55, 98, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 55, 98, 157, 98, 27, 27);
			}
			if (ENGIND[2])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 98, 20, 98, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 98, 122, 98, 27, 27);
			}
			if (ENGIND[3])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 44, 20, 44, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 20, 44, 122, 44, 27, 27);
			}
			if (ENGIND[4])
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 71, 37, 71, 27, 27);
			}
			else
			{
				oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 71, 140, 71, 27, 27);
			}
		}
	}

	if (LVRateLight)
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 6, 4, 6, 4, 27, 27);
	}
	else
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 6, 4, 108, 4, 27, 27);
	}

	//
	// Saturn 1b doesn't have an SII sep light.
	//

	if (SaturnType == SAT_SATURNV)
	{
		if (SIISepState)
		{
			oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 4, 37, 4, 27, 27);
		}
		else
		{
			oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 37, 4, 139, 4, 27, 27);
		}
	}

	if (LVGuidLight)
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 69, 4, 69, 4, 27, 27);
	}
	else
	{
		oapiBlt(surf, srf[SRF_VC_LVENGLIGHTS], 69, 4, 171, 4, 27, 27);
	}
	return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2], true);
		return true;

	/*case AID_MISSION_CLOCK306:
		MissionTimer306Display.Render90(surf, srf[SRF_DIGITAL90], true);
		return true;*/

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_EVENT_TIMER_DIGITS]);
		return true;

	case AID_VC_CWS_LIGHTS_LEFT:
		cws.RenderLights(surf, srf[SRF_VC_CW_LIGHTS], true);
		return true;

	case AID_VC_CWS_LIGHTS_RIGHT:
		cws.RenderLights(surf, srf[SRF_VC_CW_LIGHTS], false);
		return true;

	case AID_VC_SPS_LIGHT:
		if (ems.SPSThrustLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 41, 16, 41, 16);
		}
		else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 16, 41, 16);
		}
		return true;

	case AID_VC_PT05G_LIGHT:
		if (ems.pt05GLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 41, 0, 41, 16);
		}
		else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 0, 41, 16);
		}
		return true;


	case AID_VC_EMSDVDISPLAY:
		EMSDvDisplay.DoDrawSwitchVC(surf, EMSDvDisplay.QueryValue(), srf[SRF_VC_DIGITALDISP]);
		return true;

	case AID_VC_EMS_SCROLL_LEO:
	{

		HDC hDC;

		hDC = oapiGetDC(srf[SRF_VC_EMS_SCROLL_LEO]);

		SetBkMode(hDC, TRANSPARENT);
		HGDIOBJ oldObj = SelectObject(hDC, g_Param.pen[2]);

		Polyline(hDC, ems.ScribePntArray, ems.ScribePntCnt);

		SelectObject(hDC, oldObj);
		oapiReleaseDC(srf[SRF_VC_EMS_SCROLL_LEO], hDC);

		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_LEO], 5, 4, ems.GetScrollOffset(), 0, 132, 143);
		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_BUG], 42, ems.GetGScribe() + 2, 0, 0, 5, 5, SURF_PREDEF_CK);
		oapiBlt(surf, srf[SRF_VC_EMS_SCROLL_BORDER], 0, 0, 0, 0, 142, 150, SURF_PREDEF_CK);
		return true;
	}

	case AID_VC_EMS_RSI_BKGRND:
	{
		switch (ems.LiftVectLight()) {
		case 1:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 33, 8, 82, 6, 20, 6);
			break;
		case -1:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 32, 69, 82, 22, 22, 10);
			break;
		case 0:
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 33, 8, 82, 0, 20, 6);
			oapiBlt(surf, srf[SRF_VC_EMS_LIGHTS], 32, 69, 82, 12, 22, 10);
			break;
		}

		SetAnimation(anim_RSI_indicator, ems.GetRSIRotation() / PI2);
		return true;
	}

	case AID_VC_EMS_DVSET:
		switch ((int)EMSDvSetSwitch.GetPosition()) {
		case 1:
			SetAnimation(anim_emsdvsetswitch, 1.0);
			break;
		case 2:
			SetAnimation(anim_emsdvsetswitch, 0.75);
			break;
		case 3:
			SetAnimation(anim_emsdvsetswitch, 0.0);
			break;
		case 4:
			SetAnimation(anim_emsdvsetswitch, 0.25);
			break;
		default:
			SetAnimation(anim_emsdvsetswitch, 0.5);
			break;
		}
		return true;

	/*case AID_VC_RCS_HELIUM1_TB:
		SMRCSHelium1ATalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSHelium1BTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSHelium1CTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSHelium1DTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		return true;

	case AID_VC_RCS_HELIUM2_TB:
		SMRCSHelium2ATalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSHelium2BTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSHelium2CTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSHelium2DTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		return true;

	case AID_VC_RCS_PROP1_TB:
		SMRCSProp1ATalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSProp1BTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSProp1CTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSProp1DTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		return true;

	case AID_VC_RCS_PROP2_TB:
		CMRCSProp1Talkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		CMRCSProp2Talkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSProp2ATalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSProp2BTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSProp2CTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		SMRCSProp2DTalkback.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		return true;

	case AID_VC_ECSRADIATORIND:
		EcsRadiatorIndicator.DrawSwitchVC(surf, srf[SRF_VC_ECSINDICATOR]);
		return true;

	case AID_VC_DOCKINGPROBEIND:
		DockingProbeAIndicator.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		DockingProbeBIndicator.DrawSwitchVC(surf, srf[SRF_VC_INDICATOR]);
		return true;

	case AID_VC_SEQUENCERSWITCHES:
		LiftoffNoAutoAbortSwitch.RepaintSwitchVC(surf, srf[SRF_VC_SEQUENCERSWITCHES]);
		return true;

	/*case AID_MASTER_ALARM3:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_NONE);
		return true;*/

	/*default:
		return false;*/
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
	//return false;
}


void Saturn::JostleViewpoint(double amount)

{
	double j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
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

void Saturn::SetView()

{
	SetView(CurrentViewOffset, false);
}

void Saturn::SetView(double offset)

{
	SetView(offset, false);
}

void Saturn::SetView(bool update_direction)

{
	SetView(CurrentViewOffset, update_direction);
}

void Saturn::SetView(double offset, bool update_direction)

{
	VECTOR3 v;
	TRACESETUP("Saturn::SetView");
	CurrentViewOffset = offset;

	//
	// Engineering cameras
	//
	/*if (viewpos >= SATVIEW_ENG1)
	{
		VECTOR3 e1 = _V(0, 0, 0), e2 = _V(0, 0, 0), e3 = _V(0, 0, 0), e4 = _V(0, 0, 0), e5 = _V(0, 0, 0), e6 = _V(0, 0, 0);	
		VECTOR3 v1 = _V(0, 0, 0), v2 = _V(0, 0, 0), v3 = _V(0, 0, 0), v4 = _V(0, 0, 0), v5 = _V(0, 0, 0), v6 = _V(0, 0, 0);
		VECTOR3 cd;

		//
		// We really need different cameras for Saturn V and 1b.
		//

		switch (stage) {
		case PRELAUNCH_STAGE:
			e3 = _V(0.0, 7.5, -10.0+STG0O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0+STG0O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0+STG0O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0+STG0O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		case LAUNCH_STAGE_ONE:
			e1 = _V(4.0, 0.0, -39.0+STG0O);
			v1 = _V(-0.15, 0, 1.0);
			e2 = _V(3.5, 0.0, -31.0+STG0O);
			v2 = _V(-0.15, 0, -1.0);
			e3 = _V(0.0, 7.5, -10.0+STG0O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0+STG0O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0+STG0O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0+STG0O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		case LAUNCH_STAGE_TWO:
		case LAUNCH_STAGE_TWO_ISTG_JET:
			e2 = _V(3.5, 0.0, -31.0-STG1O);
			v2 = _V(-0.15, 0, -1.0);
			e3 = _V(0.0, 7.5, -10.0-STG1O);
			v3 = _V(0.0, -0.1, -1.0);
			e4 = _V(7.5, 0.0, -10.0-STG1O);
			v4 = _V(-0.1, 0.0, -1.0);
			e5 = _V(0.0, -7.5, -10.0-STG1O);
			v5 = _V(0.0, 0.1, -1.0);
			e6 = _V(-7.5, 0.0, -10.0-STG1O);
			v6 = _V(0.1, 0.0, -1.0);
			break;

		//
		// Switch back to commander view if we're past the point where we can
		// display anything useful.
		//

		case LAUNCH_STAGE_SIVB:
			viewpos = SATVIEW_LEFTSEAT;
			SetView(offset, true);
			return;
		}

		switch (viewpos) {
		case SATVIEW_ENG1:
			v = e1;
			cd = v1;
			break;

		case SATVIEW_ENG2:
			v = e2;
			cd = v2;
			break;

		case SATVIEW_ENG3:
			v = e3;
			cd = v3;
			break;

		case SATVIEW_ENG4:
			v = e4;
			cd = v4;
			break;

		case SATVIEW_ENG5:
			v = e5;
			cd = v5;
			break;

		case SATVIEW_ENG6:
			v = e6;
			cd = v6;
			break;
		}

		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
		SetCameraDefaultDirection(cd);
		oapiCameraSetCockpitDir(0,0);
	}

	// 
	// 2D panel 
	// Direction/rotation range is in clbkLoadPanel
	//
	else*/ if (InPanel) {
		if (PanelId == SATPANEL_LEFT_RNDZ_WINDOW) {
			v = _V(-0.605, 1.045, offset - 3.0); // Adjusted to line up with docking target

		} else if (PanelId == SATPANEL_LEFT_317_WINDOW) {
			v = _V(-0.710051624, 1.305366408, offset - 3.0);

		} else if (PanelId == SATPANEL_RIGHT_RNDZ_WINDOW) {
			v = _V(0.710051624, 1.305366408, offset - 3.0);

		} else if (PanelId == SATPANEL_HATCH_WINDOW) {
			v = _V(0, 0, offset + 1.0);

		} else if (PanelId == SATPANEL_TUNNEL) {
			// Calibrated for best view with forward hatch open
			v = _V(0, 0, offset + 0.4);

		} else {
			// "Calibrated" for optics cover jettison as seen through the scanning telescope 
			v = _V(0, 0, offset + 0.2);
		}
	} 

	//
	// Generic cockpit
	// Direction/rotation range is in clbkLoadGenericCockpit
	//
	else if (!InVC) {		
		v = _V(0, 0, offset - 3.0);
	} 

	//
	// Virtual cockpit
	//
	else {
		switch (viewpos) {
			case SATVIEW_LEFTSEAT:
				v = _V(-0.6, 0.9, offset);
				break;

			case SATVIEW_CENTERSEAT:
				v = _V(0, 0.9, offset);
				break;

			case SATVIEW_RIGHTSEAT:
				v = _V(0.6, 0.9, offset);
				break;

			case SATVIEW_LEFTDOCK:
				v = _V(-0.65, 1.05, 0.25 + offset);
				break;
			
			case SATVIEW_RIGHTDOCK:
				v = _V(0.65, 1.05, 0.25 + offset);
				break;

			case SATVIEW_GNPANEL:
				v = _V(0.0, -0.15, 0.5 + offset);
				break;
		}

		if (update_direction) {
			SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
			if (viewpos == SATVIEW_GNPANEL) {
				SetCameraDefaultDirection(_V(0.0,-1.0, 0.0));
			} else {
				SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			}
			oapiCameraSetCockpitDir(0,0);
		}

		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;
	}
	SetCameraOffset(v);

	VCCameraOffset.x = v.x - VCMeshOffset.x;
	VCCameraOffset.y = v.y - VCMeshOffset.y;
	VCCameraOffset.z = v.z - VCMeshOffset.z;

	//
	// FOV handling
	//

	if (!FovExternal && !GenericFirstTimestep) {
		if (InPanel && PanelId == SATPANEL_SEXTANT) { // Sextant
			if (!FovFixed) {
				FovSave = oapiCameraAperture();
				FovFixed = true;
			}
			oapiCameraSetAperture(1 * RAD);

		}
		else if (InPanel && PanelId == SATPANEL_TELESCOPE) { // Telescope
			if (!FovFixed) {
				FovSave = oapiCameraAperture();
				FovFixed = true;
			}
			oapiCameraSetAperture(30 * RAD);
		}
		else {
			if (FovFixed) {
				oapiCameraSetAperture(FovSave);
				FovFixed = false;
			}
		}
	}
}

void Saturn::InitVCAnimations() {
	//anim_P2needles[P2_NEEDLECOUNT] = -1;
	//anim_P8thumbwheels[P8_TWCOUNT] = -1;
	//anim_P11R1cbs[P11R1_CBCOUNT] = -1;

	anim_P1switch[P1_SWITCHCOUNT_C] = -1;
	anim_P1rot[P1_ROTCOUNT_C] = -1;
	anim_P1pushbuttons[P1_PUSHBCOUNT] = -1;
	anim_P2switch[P2_SWITCHCOUNT_C] = -1;
	anim_P2rot[P2_ROTCOUNT_C] = -1;
	anim_RSI_indicator = -1;
	anim_emsdvsetswitch = -1;
	anim_switchcovers[SWITCHCOVERCOUNT_C] = -1;
	anim_fdaiR_L = anim_fdaiR_R = -1;
	anim_fdaiP_L = anim_fdaiP_R = -1;
	anim_fdaiY_L = anim_fdaiY_R = -1;
	anim_fdaiRerror_L = anim_fdaiRerror_R = -1;
	anim_fdaiPerror_L = anim_fdaiPerror_R = -1;
	anim_fdaiYerror_L = anim_fdaiYerror_R = -1;
	anim_fdaiRrate_L = anim_fdaiRrate_R = -1;
	anim_fdaiPrate_L = anim_fdaiPrate_R = -1;
	anim_fdaiYrate_L = anim_fdaiYrate_R = -1;
}

void Saturn::DeleteVCAnimations()
{
	int i = 0;

	//for (i = 0; i < P2_NEEDLECOUNT; i++) delete mgt_P2needles[i];

	//for (i = 0; i < P8_TWCOUNT; i++) delete mgt_P8thumbwheels[i];

	//for (i = 0; i < P11R1_CBCOUNT; i++) delete mgt_P11R1cbs[i];

	for (i = 0; i < P1_SWITCHCOUNT_C; i++) delete mgt_P1switch[i];

	for (i = 0; i < P1_ROTCOUNT_C; i++) delete mgt_P1Rot[i];

	for (i = 0; i < P1_PUSHBCOUNT; i++) delete mgt_P1pushbuttons[i];

	for (i = 0; i < P2_SWITCHCOUNT_C; i++) delete mgt_P2switch[i];

	for (i = 0; i < P2_ROTCOUNT_C; i++) delete mgt_P2Rot[i];

	for (i = 0; i < SWITCHCOVERCOUNT_C; i++) delete mgt_switchcovers[i];
}

void Saturn::DefineVCAnimations()

{
	UINT mesh = vcidx;

	// Panel 1 switches/needles/rotaries/pushbuttons
	static UINT meshgroup_P1switches[P1_SWITCHCOUNT_C], meshgroup_P1pushbuttons[P1_PUSHBCOUNT], meshgroup_P1_Rots[P1_ROTCOUNT_C];
	for (int i = 0; i < P1_SWITCHCOUNT_C; i++)
	{
		meshgroup_P1switches[i] = VC_GRP_Sw_P1_01 + i;
		mgt_P1switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P1switches[i], 1, P1_TOGGLE_POS_C[i], _V(1, 0, 0), (float)PI / 4);
		anim_P1switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P1switch[i], 0.0f, 1.0f, mgt_P1switch[i]);
	}

	for (int i = 0; i < P1_PUSHBCOUNT; i++)
	{
		meshgroup_P1pushbuttons[i] = VC_GRP_PB_P1_01 + i;
		mgt_P1pushbuttons[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P1pushbuttons[i], 1, P1_3_PB_VECT);
		anim_P1pushbuttons[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P1pushbuttons[i], 0.0f, 1.0f, mgt_P1pushbuttons[i]);
	}

	for (int i = 0; i < P1_ROTCOUNT_C; i++)
	{
		meshgroup_P1_Rots[i] = VC_GRP_Rot_P1_01 + i;
		mgt_P1Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P1_Rots[i], 1, P1_ROT_POS_C[i], P1_3_ROT_AXIS, (float)(RAD * 360));
		anim_P1rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P1rot[i], 0.0f, 1.0f, mgt_P1Rot[i]);
	}

	// Panel 2 switches/needles/rotaries
	static UINT meshgroup_P2switches[P2_SWITCHCOUNT_C], meshgroup_P2_Rots[P2_ROTCOUNT_C];
	for (int i = 0; i < P2_SWITCHCOUNT_C; i++)
	{
		meshgroup_P2switches[i] = VC_GRP_Sw_P2_01 + i;
		mgt_P2switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P2switches[i], 1, P2_TOGGLE_POS_C[i], _V(1, 0, 0), (float)PI / 4);
		anim_P2switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P2switch[i], 0.0f, 1.0f, mgt_P2switch[i]);
	}

	for (int i = 0; i < P2_ROTCOUNT_C; i++)
	{
		meshgroup_P2_Rots[i] = VC_GRP_Rot_P2_01 + i;
		mgt_P2Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P2_Rots[i], 1, P2_ROT_POS_C[i], P1_3_ROT_AXIS, (float)(RAD * 360));
		anim_P2rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P2rot[i], 0.0f, 1.0f, mgt_P2Rot[i]);
	}

	// Panel 1-3 Switch covers

	static UINT meshgroup_switchcovers[SWITCHCOVERCOUNT_C];
	for (int i = 0; i < 19; i++) // Normal switch covers are numbered 1-19
	{
		meshgroup_switchcovers[i] = VC_GRP_SwitchCover_P1_3_01 + i;
		mgt_switchcovers[i] = new MGROUP_ROTATE(mesh, &meshgroup_switchcovers[i], 1, COVERS_POS_C[i], _V(1, 0, 0), (float)(RAD * 90));
		anim_switchcovers[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_switchcovers[i], 0.0f, 1.0f, mgt_switchcovers[i]);
	}

	for (int i = 19; i < 23; i++) // 20-23 are SEQS button covers (left)
	{
		meshgroup_switchcovers[i] = VC_GRP_SwitchCover_P1_3_01 + i;
		mgt_switchcovers[i] = new MGROUP_ROTATE(mesh, &meshgroup_switchcovers[i], 1, COVERS_POS_C[i], SEQS_COVER_AXIS_L, (float)(RAD * 70));
		anim_switchcovers[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_switchcovers[i], 0.0f, 1.0f, mgt_switchcovers[i]);
	}

	for (int i = 23; i < SWITCHCOVERCOUNT_C; i++) // 24-27 are SEQS button covers (right)
	{
		meshgroup_switchcovers[i] = VC_GRP_SwitchCover_P1_3_01 + i;
		mgt_switchcovers[i] = new MGROUP_ROTATE(mesh, &meshgroup_switchcovers[i], 1, COVERS_POS_C[i], SEQS_COVER_AXIS_R, (float)(RAD * 70));
		anim_switchcovers[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_switchcovers[i], 0.0f, 1.0f, mgt_switchcovers[i]);
	}

	// RSI indicator
	static UINT meshgroup_RSI_indicator = VC_GRP_RSI_Indicator;
	static MGROUP_ROTATE mgt_RSI_indicator(mesh, &meshgroup_RSI_indicator, 1, _V(-0.680414, 0.71462, 0.365469), P1_3_ROT_AXIS, (float)(RAD * 360));
	anim_RSI_indicator = CreateAnimation(0.0);
	AddAnimationComponent(anim_RSI_indicator, 0.0f, 1.0f, &mgt_RSI_indicator);

	// EMS Set switch
	static UINT  meshgroup_EMS_DVSet_Switch = VC_GRP_EMSDV_Set_switch;
	static MGROUP_ROTATE mgt_EMS_DVSet_Switch(mesh, &meshgroup_EMS_DVSet_Switch, 1, _V(-0.507344, 0.732746, 0.370513), _V(1, 0, 0), (float)(RAD * 10));
	anim_emsdvsetswitch = CreateAnimation(0.5);
	AddAnimationComponent(anim_emsdvsetswitch, 0.0f, 1.0f, &mgt_EMS_DVSet_Switch);

	InitFDAI(mesh);
}

void Saturn::InitFDAI(UINT mesh)

{
	// 3D FDAI initialization

	// Constants
	const VECTOR3 fdairollaxis = { -0.00, sin(P1_3_TILT), -cos(P1_3_TILT) };
	const VECTOR3 fdaiyawvaxis = { -0.00, sin(P1_3_TILT + (90.0 * RAD)), -cos(P1_3_TILT + (90.0 * RAD)) };
	const VECTOR3 needlexvector = { 0.00, 0.05*cos(P1_3_TILT), 0.05*sin(P1_3_TILT) };
	const VECTOR3 needleyvector = { 0.05, 0, 0 };
	const VECTOR3 ratexvector = { 0.00, 0.062*cos(P1_3_TILT), 0.062*sin(P1_3_TILT) };
	const VECTOR3 rateyvector = { 0.062, 0, 0 };
	const VECTOR3 FDAI_PIVOT_L = { -0.673236, 0.563893, 0.385934 }; // L FDAI Pivot Point
	const VECTOR3 FDAI_PIVOT_R = { -0.340246, 0.750031, 0.44815 }; // R FDAI Pivot Point

	// L FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_L, ach_FDAIpitch_L, ach_FDAIyaw_L;
	static UINT meshgroup_Fdai1_L = { VC_GRP_FDAIBall1_L }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_L = { VC_GRP_FDAIBall_L };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_L = { VC_GRP_FDAIBall2_L }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_L(mesh, &meshgroup_Fdai1_L, 1, FDAI_PIVOT_L, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_L(mesh, &meshgroup_Fdai2_L, 1, FDAI_PIVOT_L, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_L(mesh, &meshgroup_Fdai3_L, 1, FDAI_PIVOT_L, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_L = CreateAnimation(0.0);
	anim_fdaiP_L = CreateAnimation(0.0);
	anim_fdaiY_L = CreateAnimation(0.0);
	ach_FDAIroll_L = AddAnimationComponent(anim_fdaiR_L, 0.0f, 1.0f, &mgt_FDAIRoll_L);
	ach_FDAIyaw_L = AddAnimationComponent(anim_fdaiY_L, 0.0f, 1.0f, &mgt_FDAIYaw_L, ach_FDAIroll_L);
	ach_FDAIpitch_L = AddAnimationComponent(anim_fdaiP_L, 0.0f, 1.0f, &mgt_FDAIPitch_L, ach_FDAIyaw_L);

	// L FDAI error needles
	static UINT meshgroup_RollError_L = VC_GRP_FDAI_rollerror_L;
	static UINT meshgroup_PitchError_L = VC_GRP_FDAI_pitcherror_L;
	static UINT meshgroup_YawError_L = VC_GRP_FDAI_yawerror_L;
	static MGROUP_TRANSLATE mgt_rollerror_L(mesh, &meshgroup_RollError_L, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_L(mesh, &meshgroup_PitchError_L, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_L(mesh, &meshgroup_YawError_L, 1, needleyvector);
	anim_fdaiRerror_L = CreateAnimation(0.5);
	anim_fdaiPerror_L = CreateAnimation(0.5);
	anim_fdaiYerror_L = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_L, 0.0f, 1.0f, &mgt_rollerror_L);
	AddAnimationComponent(anim_fdaiPerror_L, 0.0f, 1.0f, &mgt_pitcherror_L);
	AddAnimationComponent(anim_fdaiYerror_L, 0.0f, 1.0f, &mgt_yawerror_L);

	// L FDAI rate needles
	static UINT meshgroup_RollRate_L = VC_GRP_FDAI_rateR_L;
	static UINT meshgroup_PitchRate_L = VC_GRP_FDAI_rateP_L;
	static UINT meshgroup_YawRate_L = VC_GRP_FDAI_rateY_L;
	static MGROUP_TRANSLATE mgt_rollrate_L(mesh, &meshgroup_RollRate_L, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_L(mesh, &meshgroup_PitchRate_L, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_L(mesh, &meshgroup_YawRate_L, 1, rateyvector);
	anim_fdaiRrate_L = CreateAnimation(0.5);
	anim_fdaiPrate_L = CreateAnimation(0.5);
	anim_fdaiYrate_L = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_L, 0.0f, 1.0f, &mgt_rollrate_L);
	AddAnimationComponent(anim_fdaiPrate_L, 0.0f, 1.0f, &mgt_pitchrate_L);
	AddAnimationComponent(anim_fdaiYrate_L, 0.0f, 1.0f, &mgt_yawrate_L);

	// R FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_R, ach_FDAIpitch_R, ach_FDAIyaw_R;
	static UINT meshgroup_Fdai1_R = { VC_GRP_FDAIBall1_R }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_R = { VC_GRP_FDAIBall_R };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_R = { VC_GRP_FDAIBall2_R }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_R(mesh, &meshgroup_Fdai1_R, 1, FDAI_PIVOT_R, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_R(mesh, &meshgroup_Fdai2_R, 1, FDAI_PIVOT_R, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_R(mesh, &meshgroup_Fdai3_R, 1, FDAI_PIVOT_R, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_R = CreateAnimation(0.0);
	anim_fdaiP_R = CreateAnimation(0.0);
	anim_fdaiY_R = CreateAnimation(0.0);
	ach_FDAIroll_R = AddAnimationComponent(anim_fdaiR_R, 0.0f, 1.0f, &mgt_FDAIRoll_R);
	ach_FDAIyaw_R = AddAnimationComponent(anim_fdaiY_R, 0.0f, 1.0f, &mgt_FDAIYaw_R, ach_FDAIroll_R);
	ach_FDAIpitch_R = AddAnimationComponent(anim_fdaiP_R, 0.0f, 1.0f, &mgt_FDAIPitch_R, ach_FDAIyaw_R);

	// R FDAI error needles
	static UINT meshgroup_RollError_R = VC_GRP_FDAI_rollerror_R;
	static UINT meshgroup_PitchError_R = VC_GRP_FDAI_pitcherror_R;
	static UINT meshgroup_YawError_R = VC_GRP_FDAI_yawerror_R;
	static MGROUP_TRANSLATE mgt_rollerror_R(mesh, &meshgroup_RollError_R, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_R(mesh, &meshgroup_PitchError_R, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_R(mesh, &meshgroup_YawError_R, 1, needleyvector);
	anim_fdaiRerror_R = CreateAnimation(0.5);
	anim_fdaiPerror_R = CreateAnimation(0.5);
	anim_fdaiYerror_R = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_R, 0.0f, 1.0f, &mgt_rollerror_R);
	AddAnimationComponent(anim_fdaiPerror_R, 0.0f, 1.0f, &mgt_pitcherror_R);
	AddAnimationComponent(anim_fdaiYerror_R, 0.0f, 1.0f, &mgt_yawerror_R);

	// R FDAI rate needles
	static UINT meshgroup_RollRate_R = VC_GRP_FDAI_rateR_R;
	static UINT meshgroup_PitchRate_R = VC_GRP_FDAI_rateP_R;
	static UINT meshgroup_YawRate_R = VC_GRP_FDAI_rateY_R;
	static MGROUP_TRANSLATE mgt_rollrate_R(mesh, &meshgroup_RollRate_R, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_R(mesh, &meshgroup_PitchRate_R, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_R(mesh, &meshgroup_YawRate_R, 1, rateyvector);
	anim_fdaiRrate_R = CreateAnimation(0.5);
	anim_fdaiPrate_R = CreateAnimation(0.5);
	anim_fdaiYrate_R = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_R, 0.0f, 1.0f, &mgt_rollrate_R);
	AddAnimationComponent(anim_fdaiPrate_R, 0.0f, 1.0f, &mgt_pitchrate_R);
	AddAnimationComponent(anim_fdaiYrate_R, 0.0f, 1.0f, &mgt_yawrate_R);
}

void Saturn::AnimateFDAI(VECTOR3 attitude, VECTOR3 rates, VECTOR3 errors, UINT animR, UINT animP, UINT animY, UINT errorR, UINT errorP, UINT errorY, UINT rateR, UINT rateP, UINT rateY) {

	double fdai_proc[3];
	double rate_proc[3];

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
	rate_proc[0] = (rates.z + 1) / 2;
	rate_proc[1] = (rates.x + 1) / 2;
	rate_proc[2] = (-rates.y + 1) / 2;
	if (rate_proc[0] < 0) rate_proc[0] = 0;
	if (rate_proc[1] < 0) rate_proc[1] = 0;
	if (rate_proc[2] < 0) rate_proc[2] = 0;
	if (rate_proc[0] > 1) rate_proc[0] = 1;
	if (rate_proc[1] > 1) rate_proc[1] = 1;
	if (rate_proc[2] > 1) rate_proc[1] = 1;
	SetAnimation(rateR, rate_proc[0]);
	SetAnimation(rateP, rate_proc[1]);
	SetAnimation(rateY, rate_proc[2]);
}


