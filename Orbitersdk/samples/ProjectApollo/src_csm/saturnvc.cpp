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


void Saturn::InitVC ()
{
	//int i;
	TRACESETUP("Saturn::InitVC");

	// Load CM VC surfaces

	//srf[SRF_VC_DIGITALDISP] = oapiLoadTexture("ProjectApollo/VC/digitaldisp.dds");

	//oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP], 0);
		
	//reset state flags (see DeltaGlider for similar)

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

	VECTOR3 ofs;
	ofs = _V(0.0, 0.0, CurrentViewOffset); // double check offset should be in 3rd bracket

	ReleaseSurfacesVC();

	MainPanelVC.ClearSwitches();

	SURFHANDLE MainPanelTex1 = oapiGetTextureHandle(hCMVC, 6);
	SURFHANDLE MainPanelTex2 = oapiGetTextureHandle(hCMVC, 8);

	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));
	oapiVCRegisterArea(AID_MASTER_ALARM, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_MASTER_ALARM, _V(-0.205, -0.243, 0.61), 0.015);
	oapiVCRegisterArea(AID_DSKY_KEY, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_DSKY_KEY, _V(0.162, -0.397, 0.557), _V(0.34, -0.397, 0.557), _V(0.162, -0.474, 0.53), _V(0.34, -0.474, 0.53));


	//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));
	oapiVCRegisterArea(AID_MASTER_ALARM, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_MASTER_ALARM, _V(0.804, 0.653, 0.463), 0.015);
	oapiVCRegisterArea(AID_DSKY_KEY, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Quadrilateral(AID_DSKY_KEY, _V(0.442, 0.5, 0.407), _V(0.258, 0.5, 0.407), _V(0.258, 0.424, 0.38), _V(0.442, 0.424, 0.38));


	//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));


	//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));


	//register areas and AreaClickmodes
		//oapiVCRegisterArea (areaidentifier, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_"event1"|PANEL_MOUSE_"event2");
		//oapiVCSetAreaClickmode_Spherical (areaidentifier, _V(#x,#y,#z,#radius);
		//oapiVCSetAreaClickmode_Quadrilateral (areaidentifier, _V(uplftvect), _V(uprtvect), _V(lwrlftvect), _V(lwrrtvect));

	InitVC();
}

// --------------------------------------------------------------
// Respond to virtual cockpit mouse event
// --------------------------------------------------------------
bool Saturn::clbkVCMouseEvent (int id, int event, VECTOR3 &p)
{
	TRACESETUP("Saturn::clbkVCMouseEvent");
	switch (id) {
	//case areaidentifier:
	    //event stuff here
		//return true;
	//case areaidentifier:
		//blah blah blah
		//return true;
	//}

	case AID_MASTER_ALARM:
	//case AID_MASTER_ALARM2:
	//case AID_MASTER_ALARM3:
		sprintf(oapiDebugString(), "Alarming...");
		return cws.CheckMasterAlarmMouseClick(event);

	case AID_DSKY_KEY:
		sprintf(oapiDebugString(), "DSKY-ing...");
		int mx;
		int my;

		mx = int(p.x * 288);
		my = int(p.y * 121);

		if (event & PANEL_MOUSE_LBDOWN) {
			dsky.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky.ProcessKeyRelease(mx, my);
		}
		return true;

	/*case AID_DSKY2_KEY:
		if (event & PANEL_MOUSE_LBDOWN) {
			dsky2.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky2.ProcessKeyRelease(mx, my);
		}
		return true;*/
	}
	return false;
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
		if (!fdaiDisabled) {  // Is this FDAI enabled?
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;

			euler_rates = eda.GetFDAI1AttitudeRate();
			attitude = eda.GetFDAI1Attitude();
			errors = eda.GetFDAI1AttitudeError();

			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			/*if (errors.x > 41) { errors.x = 41; }
			else { if (errors.x < -41) { errors.x = -41; } }
			if (errors.y > 41) { errors.y = 41; }
			else { if (errors.y < -41) { errors.y = -41; } }
			if (errors.z > 41) { errors.z = 41; }
			else { if (errors.z < -41) { errors.z = -41; } }
			fdaiLeft.PaintMe(attitude, 0, euler_rates, errors, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);*/

			AnimateFDAI(attitude, euler_rates, errors, anim_fdaiR_L, anim_fdaiP_L, anim_fdaiY_L, anim_fdaiRerror_L, anim_fdaiPerror_L, anim_fdaiYerror_L, anim_fdaiRrate_L, anim_fdaiPrate_L, anim_fdaiYrate_L);
		}
		return true;

	case AID_VC_FDAI_RIGHT:
		if (!fdaiDisabled) {  // Is this FDAI enabled?
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;

			euler_rates = eda.GetFDAI2AttitudeRate();
			attitude = eda.GetFDAI2Attitude();
			errors = eda.GetFDAI2AttitudeError();

			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			/*if (errors.x > 41) { errors.x = 41; }
			else { if (errors.x < -41) { errors.x = -41; } }
			if (errors.y > 41) { errors.y = 41; }
			else { if (errors.y < -41) { errors.y = -41; } }
			if (errors.z > 41) { errors.z = 41; }
			else { if (errors.z < -41) { errors.z = -41; } }
			fdaiRight.PaintMe(attitude, 0, euler_rates, errors, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);*/

			AnimateFDAI(attitude, euler_rates, errors, anim_fdaiR_R, anim_fdaiP_R, anim_fdaiY_R, anim_fdaiRerror_R, anim_fdaiPerror_R, anim_fdaiYerror_R, anim_fdaiRrate_R, anim_fdaiPrate_R, anim_fdaiYrate_R);
		}
		return true;




	/*
	
	case AID_DSKY_KEY:
		dsky.RenderKeys(surf, srf[SRF_DSKYKEY]);
		return true;

	case AID_ABORT_BUTTON:
		if (ABORT_IND) {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 62, 0, 62, 31);
		}
		else {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 0, 0, 62, 31);
		}
		return true;
		
	case AID_MASTER_ALARM:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_LEFT);
		return true;

	case AID_MASTER_ALARM2:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_RIGHT);
		return true;

	case AID_MASTER_ALARM3:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_NONE);
		return true;


	case AID_MASTER_ALARM:  //temp place holder so C++ doesn't complain
		return false;*/

	default:
		return false;
	}
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

	/*anim_P1switch[P1_SWITCHCOUNT] = -1;
	anim_P1_Rot[P1_ROTCOUNT] = -1;
	anim_P1needles[P1_NEEDLECOUNT] = -1;
	anim_P2switch[P2_SWITCHCOUNT] = -1;
	anim_P2_Rot[P2_ROTCOUNT] = -1;
	anim_P2needles[P2_NEEDLECOUNT] = -1;
	anim_P3switch[P3_SWITCHCOUNT] = -1;
	anim_P3_Rot[P3_ROTCOUNT] = -1;
	anim_P3needles[P3_NEEDLECOUNT] = -1;
	anim_P4switch[P4_SWITCHCOUNT] = -1;
	anim_P5switch[P5_SWITCHCOUNT] = -1;
	anim_P5_Rot[P5_ROTCOUNT] = -1;
	anim_P6switch[P6_SWITCHCOUNT] = -1;
	anim_P8switch[P8_SWITCHCOUNT] = -1;
	anim_P8thumbwheels[P8_TWCOUNT] = -1;
	anim_P11R1cbs[P11R1_CBCOUNT] = -1;
	anim_P11R2cbs[P11R2_CBCOUNT] = -1;
	anim_P11R3cbs[P11R3_CBCOUNT] = -1;
	anim_P11R4cbs[P11R4_CBCOUNT] = -1;
	anim_P11R5cbs[P11R5_CBCOUNT] = -1;
	anim_P12switch[P12_SWITCHCOUNT] = -1;
	anim_P12_Rot[P12_ROTCOUNT] = -1;
	anim_P12thumbwheels[P12_TWCOUNT] = -1;
	anim_P12needles[P12_NEEDLECOUNT] = -1;
	anim_P14switch[P14_SWITCHCOUNT] = -1;
	anim_P14_Rot[P14_ROTCOUNT] = -1;
	anim_P14needles[P14_NEEDLECOUNT] = -1;
	anim_P16R1cbs[P16R1_CBCOUNT] = -1;
	anim_P16R2cbs[P16R2_CBCOUNT] = -1;
	anim_P16R3cbs[P16R3_CBCOUNT] = -1;
	anim_P16R4cbs[P16R4_CBCOUNT] = -1;
	anim_TW_indicator = -1;
	anim_Needle_Radar = -1;
	anim_xpointerx_cdr = -1;
	anim_xpointery_cdr = -1;
	anim_xpointerx_lmp = -1;
	anim_xpointery_lmp = -1;
	anim_abortbutton = -1;
	anim_abortstagebutton = -1;
	anim_abortstagecover = -1;
	anim_rrslewsitch_x = -1;
	anim_rrslewsitch_y = -1;
	anim_stageswitch = -1;
	anim_stagecover = -1;
	anim_startbutton = -1;
	anim_stopbutton_cdr = -1;
	anim_stopbutton_lmp = -1;
	anim_plusxbutton = -1;*/
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
	/*int i = 0;

	for (i = 0; i < P1_SWITCHCOUNT; i++) delete mgt_P1switch[i];

	for (i = 0; i < P1_ROTCOUNT; i++) delete mgt_P1Rot[i];

	for (i = 0; i < P1_NEEDLECOUNT; i++) delete mgt_P1needles[i];

	for (i = 0; i < P2_SWITCHCOUNT; i++) delete mgt_P2switch[i];

	for (i = 0; i < P2_ROTCOUNT; i++) delete mgt_P2Rot[i];

	for (i = 0; i < P2_NEEDLECOUNT; i++) delete mgt_P2needles[i];

	for (i = 0; i < P3_SWITCHCOUNT; i++) delete mgt_P3switch[i];

	for (i = 0; i < P3_ROTCOUNT; i++) delete mgt_P3Rot[i];

	for (i = 0; i < P3_NEEDLECOUNT; i++) delete mgt_P3needles[i];

	for (i = 0; i < P4_SWITCHCOUNT; i++) delete mgt_P4switch[i];

	for (i = 0; i < P5_SWITCHCOUNT; i++) delete mgt_P5switch[i];

	for (i = 0; i < P5_ROTCOUNT; i++) delete mgt_P5Rot[i];

	for (i = 0; i < P6_SWITCHCOUNT; i++) delete mgt_P6switch[i];

	for (i = 0; i < P8_SWITCHCOUNT; i++) delete mgt_P8switch[i];

	for (i = 0; i < P8_TWCOUNT; i++) delete mgt_P8thumbwheels[i];

	for (i = 0; i < P11R1_CBCOUNT; i++) delete mgt_P11R1cbs[i];

	for (i = 0; i < P11R2_CBCOUNT; i++) delete mgt_P11R2cbs[i];

	for (i = 0; i < P11R3_CBCOUNT; i++) delete mgt_P11R3cbs[i];

	for (i = 0; i < P11R4_CBCOUNT; i++) delete mgt_P11R4cbs[i];

	for (i = 0; i < P11R5_CBCOUNT; i++) delete mgt_P11R5cbs[i];

	for (i = 0; i < P12_SWITCHCOUNT; i++) delete mgt_P12switch[i];

	for (i = 0; i < P12_ROTCOUNT; i++) delete mgt_P12Rot[i];

	for (i = 0; i < P12_TWCOUNT; i++) delete mgt_P12thumbwheels[i];

	for (i = 0; i < P12_NEEDLECOUNT; i++) delete mgt_P12needles[i];

	for (i = 0; i < P14_SWITCHCOUNT; i++) delete mgt_P14switch[i];

	for (i = 0; i < P14_ROTCOUNT; i++) delete mgt_P14Rot[i];

	for (i = 0; i < P14_NEEDLECOUNT; i++) delete mgt_P14needles[i];

	for (i = 0; i < P16R1_CBCOUNT; i++) delete mgt_P16R1cbs[i];

	for (i = 0; i < P16R2_CBCOUNT; i++) delete mgt_P16R2cbs[i];

	for (i = 0; i < P16R3_CBCOUNT; i++) delete mgt_P16R3cbs[i];

	for (i = 0; i < P16R4_CBCOUNT; i++) delete mgt_P16R4cbs[i];*/
}

void Saturn::DefineVCAnimations()

{
	UINT mesh = vcidx;
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
	const VECTOR3 FDAI_PIVOTCDR = { -0.673236, 0.563893, 0.385934 }; // CDR FDAI Pivot Point
	const VECTOR3 FDAI_PIVOTLMP = { -0.340246, 0.750031, 0.44815 }; // LMP FDAI Pivot Point

	// L FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_L, ach_FDAIpitch_L, ach_FDAIyaw_L;
	static UINT meshgroup_Fdai1_L = { VC_GRP_FDAIBall1_L }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_L = { VC_GRP_FDAIBall_L };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_L = { VC_GRP_FDAIBall2_L }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_L(mesh, &meshgroup_Fdai1_L, 1, FDAI_PIVOTCDR, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_L(mesh, &meshgroup_Fdai2_L, 1, FDAI_PIVOTCDR, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_L(mesh, &meshgroup_Fdai3_L, 1, FDAI_PIVOTCDR, fdaiyawvaxis, (float)(RAD * 360));
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
	static MGROUP_ROTATE mgt_FDAIRoll_R(mesh, &meshgroup_Fdai1_R, 1, FDAI_PIVOTLMP, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_R(mesh, &meshgroup_Fdai2_R, 1, FDAI_PIVOTLMP, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_R(mesh, &meshgroup_Fdai3_R, 1, FDAI_PIVOTLMP, fdaiyawvaxis, (float)(RAD * 360));
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


