/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn CSM panel code

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
  *	Revision 1.30  2005/08/04 01:06:04  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.29  2005/07/30 16:12:13  tschachim
  *	Bugfix O2 FLOW meter
  *	
  *	Revision 1.28  2005/07/19 16:25:43  tschachim
  *	New switches
  *	
  *	Revision 1.27  2005/07/06 22:11:31  tschachim
  *	Finished SequencerSwitchesRow, no functionality yet
  *	
  *	Revision 1.26  2005/07/05 17:56:59  tschachim
  *	Fixed some switches because of spring-load
  *	
  *	Revision 1.25  2005/06/06 12:31:29  tschachim
  *	New switches, PanelSwitchScenarioHandler
  *	
  *	Revision 1.24  2005/05/26 15:58:40  tschachim
  *	New fuel cell displays and controls
  *	Some code moved for better readability
  *	
  *	Revision 1.23  2005/05/12 00:26:58  movieman523
  *	Added O2 tank quantity failure on Apollo 13.
  *	
  *	Revision 1.22  2005/05/05 21:40:56  tschachim
  *	Introduced cryo fans, fuel cell indicators and cabin indicators
  *	
  *	Revision 1.21  2005/04/22 16:01:54  tschachim
  *	Removed fuel cell test-code
  *	
  *	Revision 1.20  2005/04/22 14:10:03  tschachim
  *	PanelSDK introduced
  *	Panel id defines
  *	New switches and indicators
  *	ToggleSwitch.Init changed
  *	SwitchListener introduced
  *	
  *	Revision 1.19  2005/04/16 00:14:10  tschachim
  *	fixed dsky keyboard and g&n panel lights
  *	
  *	Revision 1.18  2005/04/10 19:27:04  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.17  2005/04/10 16:01:40  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.16  2005/04/10 14:29:41  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.15  2005/04/10 03:00:47  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.14  2005/04/10 01:52:04  flydba
  *	*** empty log message ***
  *	
  *	Revision 1.13  2005/04/01 15:32:51  tschachim
  *	Added RCSIndicatorsSwitch
  *	
  *	Revision 1.12  2005/03/16 19:40:42  tschachim
  *	Rechanged MFDSPEC lines as in revision 1.2
  *	
  *	Revision 1.11  2005/03/16 16:04:58  yogenfrutz
  *	changed for splitted csm panel (resolves ATI bug)
  *	
  *	Revision 1.10  2005/03/16 13:39:06  yogenfrutz
  *	changed position of docking panel MFD by 1pixel lower and 1 pixel right for better look
  *	
  *	Revision 1.9  2005/03/14 17:46:27  yogenfrutz
  *	Added changes for "light off" docking panel MFD,yogenfrutz
  *	
  *	Revision 1.8  2005/03/13 21:23:02  chode99
  *	G-gauge displays the new calculation of g (aZAcc).
  *	
  *	Revision 1.7  2005/03/11 17:54:00  tschachim
  *	Introduced GuardedToggleSwitch and GuardedThreePosSwitch
  *	
  *	Revision 1.6  2005/03/10 19:06:24  tschachim
  *	fixed "one pixel" bug
  *	
  *	Revision 1.5  2005/03/09 22:09:02  tschachim
  *	Docking panel camera without changing direction from Yogenfrutz
  *	Switchable docking panel MFD
  *	
  *	Revision 1.4  2005/03/05 02:03:40  tschachim
  *	Docking MFD is now the right MFD,
  *	button click sound added
  *	
  *	Revision 1.3  2005/03/03 18:00:18  tschachim
  *	docking panel and MFD
  *	
  *	Revision 1.2  2005/03/02 01:19:03  chode99
  *	Changed MFDSPEC lines to be same size as LM, and added button definitions, like the LM.
  *	For some unknown reason, this seems to prevent the CTDs that were occuring when issuing certain MFD commands.
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
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

extern GDIParams g_Param;

void BaseInit() {

	// need to init device-dependent resources here in case the screen mode has changed
	g_Param.col[2] = oapiGetColour(154, 154, 154);
	g_Param.col[3] = oapiGetColour(3, 3, 3);
	g_Param.col[4] = oapiGetColour(255, 0, 255);
	g_Param.col[5] = oapiGetColour(255, 0, 255);
}

//Needle function by Rob Conley from Mercury code
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

void Saturn::RedrawPanel_G (SURFHANDLE surf)
{
	double alpha;
	double range;

	alpha = aZAcc/G;

	//sprintf(oapiDebugString(), "Accel %f", alpha);
	if (alpha > 15)
		alpha = 15;
	if (alpha < -1)
		alpha = -1;

	range = 235 * RAD;
	range = range / 15;
	alpha = 15 - alpha;
	HDC hDC = oapiGetDC (surf);
	DrawNeedle (hDC, 27, 27, 26.0, (alpha*range)-50*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
	oapiReleaseDC (surf, hDC);

	oapiBlt (surf, srf[15], 0, 0, 0, 0, 56, 57, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_Thrust (SURFHANDLE surf)
{
	double alpha;
	double range;

	alpha = GetEngineLevel(ENGINE_MAIN) * 100 * ThrustAdjust;

	if (alpha > 100)
		alpha = 100;

	range = 270 * RAD;
	range = range / 100;
	alpha = 100 - alpha;
	HDC hDC = oapiGetDC (surf);
	DrawNeedle (hDC, 31, 30, 19.0, (alpha*range)-45*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
	oapiReleaseDC (surf, hDC);

	oapiBlt (surf, srf[16], 0, 0, 0, 0, 62, 60, SURF_PREDEF_CK);
}

// Altimeter Needle function by Rob Conley from Mercury code, Heavily modified to have non linear gauge range... :):)

void Saturn::RedrawPanel_Alt (SURFHANDLE surf)
{
	double alpha;
	double range;

	alpha = GetAltitude();
	alpha = alpha / 0.305;

	//sprintf(oapiDebugString(), "altitude %f", alpha);
	if (alpha > 50000) alpha = 50000;

	if (alpha < 4001){
		range = 120 * RAD;
		range = range / 4000;
		alpha = 4000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 44, 44, 40.0, (alpha*range)+150*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 4001 && alpha < 6001){
		range = 35 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 44, 44, 40.0, (alpha*range)+185*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 6001 && alpha < 8001){
		range = 25 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 44, 44, 40.0, (alpha*range)+165*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 8001 && alpha < 10001){
		range = 20 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 44, 44, 40.0, (alpha*range)+150*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 10001 && alpha < 20001){
		range = 55 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 44, 44, 40.0, (alpha*range)+70*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 20001 && alpha < 40001){
		range = 65 * RAD;
		range = range / 20000;
		alpha = 20000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 44, 44, 40.0, (alpha*range)+15*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else {
		range = 20 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, 45, 45, 45.0, (alpha*range)+10*RAD, g_Param.pen[2], g_Param.pen[3] );//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	oapiBlt (surf, srf[14], 0, 0, 0, 0, 91, 90, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_Horizon (SURFHANDLE surf)
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

void Saturn::RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset) {

	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, g_Param.font[2]);
	SetTextColor (hDC, RGB(196, 196, 196));
	SetTextAlign (hDC, TA_CENTER);
	SetBkMode (hDC, TRANSPARENT);
	const char *label;
	for (int bt = 0; bt < 6; bt++) {
		if (label = oapiMFDButtonLabel (mfd, bt+side*6))
			TextOut (hDC, 10 + xoffset, 3 + 31 * bt + yoffset, label, strlen(label));
		else break;
	}
	oapiReleaseDC (surf, hDC);
}

void Saturn::RedrawPanel_SuitCabinDeltaPMeter (SURFHANDLE surf) {

	// Suit cabin pressure difference
	double scdp = (*(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS") -
			       *(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS")) * INH2O;
	scdp = scdp / 5.0 * 60.0;
	if (scdp > 90) scdp = 90;
	if (scdp < -90) scdp = -90;

	// O2 main regulator output flow 
	double cf =  *(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOW") * LBH;
		   cf += *(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOW") * LBH;
		   cf += *(double*)Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE:FLOW") * LBH;
	cf = (cf - .6) / .4 * 60.0;
	if (cf > 90) cf = 90;
	if (cf < -90) cf = -90;

	HDC hDC = oapiGetDC (surf);
	DrawNeedle (hDC,  0, 22, 20.0, scdp * RAD, g_Param.pen[4], g_Param.pen[4]);
	DrawNeedle (hDC, 45, 22, 20.0, (180.0 - cf) * RAD, g_Param.pen[4], g_Param.pen[4]);
	oapiReleaseDC (surf, hDC);

	oapiBlt (surf, srf[SRF_SUITCABINDELTAPMETER], 0, 13, 0, 0, 46, 18, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_SuitComprDeltaPMeter (SURFHANDLE surf) {

	// O2 main regulator output flow 
	// TODO: Is this the correct flow for that meter? No documentation found yet...
	double cf =  *(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOW") * LBH;
		   cf += *(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOW") * LBH;
		   cf += *(double*)Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE:FLOW") * LBH;
	cf = (cf - .6) / .4 * 60.0;
	if (cf > 90) cf = 90;
	if (cf < -90) cf = -90;
	
	// Suit compressor pressure difference
	double scdp = (*(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:PRESS") -
			       *(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS")) * PSI;
	scdp = (scdp - .5) / .5 * 60.0;
	if (scdp > 90) scdp = 90;
	if (scdp < -90) scdp = -90;

	HDC hDC = oapiGetDC (surf);
	DrawNeedle (hDC,  0, 22, 20.0, cf * RAD, g_Param.pen[4], g_Param.pen[4]);
	DrawNeedle (hDC, 45, 22, 20.0, (180.0 - scdp) * RAD, g_Param.pen[4], g_Param.pen[4]);
	oapiReleaseDC (surf, hDC);
}

void Saturn::RedrawPanel_O2CryoTankPressureIndicator(SURFHANDLE surf, SURFHANDLE needle, double value, int xOffset, int xNeedle) {

	if (value < 100.0)
		oapiBlt(surf, needle, xOffset, 110, xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 500.0) 
		oapiBlt(surf, needle, xOffset, 110 - (int)((value - 100.0) * 0.065), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 850.0)
		oapiBlt(surf, needle, xOffset, 84 - (int)((value - 500.0) * 0.07714), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 900.0)
		oapiBlt(surf, needle, xOffset, 57 - (int)((value - 850.0) * 0.38), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 950.0)
		oapiBlt(surf, needle, xOffset, 38 - (int)((value - 900.0) * 0.42), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 1050.0)
		oapiBlt(surf, needle, xOffset, 17 - (int)((value - 950.0) * 0.13), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else
		oapiBlt(surf, needle, xOffset, 4, xNeedle, 0, 10, 10, SURF_PREDEF_CK);

}

void Saturn::RedrawPanel_CryoTankIndicators(SURFHANDLE surf) {

	// H2Tank1 pressure
	double value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:PRESS") * PSI;
	if (value < 0.0) value = 0.0;
	if (value > 400.0) value = 400.0;
	oapiBlt(surf, srf[SRF_NEEDLE],  0, (110 - (int)(value / 400.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);

	// H2Tank2 pressure
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:PRESS") * PSI;
	if (value < 0.0) value = 0.0;
	if (value > 400.0) value = 400.0;
	oapiBlt(surf, srf[SRF_NEEDLE], 53, (110 - (int)(value / 400.0 * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);

	// O2Tank1 / O2SurgeTank pressure
	if (O2PressIndSwitch)  
		value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:PRESS") * PSI;
	else
		value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:PRESS") * PSI;
	RedrawPanel_O2CryoTankPressureIndicator(surf, srf[SRF_NEEDLE], value, 86, 0);

	// O2Tank2 pressure
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:PRESS") * PSI;
	RedrawPanel_O2CryoTankPressureIndicator(surf, srf[SRF_NEEDLE], value, 139, 10);

	// H2Tank1 quantity
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:MASS") / CSM_H2TANK_CAPACITY ;
	if (value < 0.0) value = 0.0;
	if (value > 1.0) value = 1.0;
	oapiBlt(surf, srf[SRF_NEEDLE],  172, (110 - (int)(value * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);

	// H2Tank2 quantity
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:MASS") / CSM_H2TANK_CAPACITY ;
	if (value < 0.0) value = 0.0;
	if (value > 1.0) value = 1.0;
	oapiBlt(surf, srf[SRF_NEEDLE],  225, (110 - (int)(value * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);

	// O2Tank1 quantity
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:MASS") / CSM_O2TANK_CAPACITY ;
	if (value < 0.0) value = 0.0;
	if (value > 1.0) value = 1.0;
	oapiBlt(surf, srf[SRF_NEEDLE],  258, (110 - (int)(value * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);

	// O2Tank2 quantity
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:MASS") / CSM_O2TANK_CAPACITY ;
	if (value < 0.0) value = 0.0;
	if (value > 1.0) value = 1.0;

	//
	// Apollo 13 O2 tank 2 quantity display failed offscale high around 46:45.
	//

#define O2FAILURETIME	(46.0 * 3600.0 + 45.0 * 60.0)

	if (ApolloNo == 13) {
		if (MissionTime >= (O2FAILURETIME + 5.0)) {
			value = 1.05;
		}
		else if (MissionTime >= O2FAILURETIME) {
			value += (1.05 - value) * ((MissionTime - O2FAILURETIME) / 5.0);
		}
	}
	oapiBlt(surf, srf[SRF_NEEDLE],  311, (110 - (int)(value * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_CabinIndicators (SURFHANDLE surf) {

	// Suit temperature
	double value = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("HYDRAULIC:SUIT:TEMP"));
	if (value < 20.0) value = 20.0;
	if (value > 95.0) value = 95.0;
	oapiBlt(surf, srf[SRF_NEEDLE],  1, (110 - (int)((value - 20.0) / 75.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);

	// Cabin temperature
	value = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP"));
	if (value < 40.0) value = 40.0;
	if (value > 120.0) value = 120.0;
	oapiBlt(surf, srf[SRF_NEEDLE],  53, (110 - (int)((value - 40.0) / 80.0 * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);

	// Suit pressure
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS") * PSI;
	if (value < 0.0) value = 0.0;
	if (value > 16.0) value = 16.0;
	if (value < 6.0)
		oapiBlt(surf, srf[SRF_NEEDLE],  101, (108 - (int)(value / 6.0 * 55.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(surf, srf[SRF_NEEDLE],  101, (53 - (int)((value - 6.0) / 10.0 * 45.0)), 0, 0, 10, 10, SURF_PREDEF_CK);

	// Cabin pressure
	value = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS") * PSI;
	if (value < 0.0) value = 0.0;
	if (value > 16.0) value = 16.0;
	if (value < 6.0)
		oapiBlt(surf, srf[SRF_NEEDLE],  153, (108 - (int)(value / 6.0 * 55.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(surf, srf[SRF_NEEDLE],  153, (53 - (int)((value - 6.0) / 10.0 * 45.0)), 10, 0, 10, 10, SURF_PREDEF_CK);

	// Cabin CO2 partial pressure
	value = *(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:CO2_PPRESS") * MMHG;
	if (value < 0.0) value = 0.0;
	if (value > 30.0) value = 30.0;	
	if (value < 10.0)
		oapiBlt(surf, srf[SRF_NEEDLE],  215, (109 - (int)(value / 10.0 * 55.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (value < 15.0)
		oapiBlt(surf, srf[SRF_NEEDLE],  215, (54 - (int)((value - 10.0) / 5.0 * 19.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (value < 20.0)
		oapiBlt(surf, srf[SRF_NEEDLE],  215, (35 - (int)((value - 15.0) / 5.0 * 15.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(surf, srf[SRF_NEEDLE],  215, (20 - (int)((value - 20.0) / 10.0 * 14.0)), 10, 0, 10, 10, SURF_PREDEF_CK);

}

void Saturn::RedrawPanel_FuelCellIndicators (SURFHANDLE surf) {

	double h2flow, o2flow, temp, condenserTemp;

	// read values of the selected fuel cell
	if (FuelCellIndicatorsSwitch == 1) {
		h2flow = *(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:H2FLOW") * LBH;
		o2flow = *(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:O2FLOW") * LBH;
		temp = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:TEMP"));
		condenserTemp = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:CONDENSERTEMP"));

	} else if (FuelCellIndicatorsSwitch == 2) {
		h2flow = *(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:H2FLOW") * LBH;
		o2flow = *(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:O2FLOW") * LBH;
		temp = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:TEMP"));
		condenserTemp = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:CONDENSERTEMP"));

	} else {
		h2flow = *(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:H2FLOW") * LBH;
		o2flow = *(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:O2FLOW") * LBH;
		temp = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:TEMP"));
		condenserTemp = KelvinToFahrenheit(*(double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:CONDENSERTEMP"));
	}

	// Fuel Cell H2 flow
	if (h2flow < 0.0) h2flow = 0.0;
	if (h2flow > 0.2) h2flow = 0.2;
	if (h2flow < 0.05)
		oapiBlt(surf, srf[SRF_NEEDLE],  0, (111 - (int)(h2flow / 0.05 * 21.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else if (h2flow < 0.15)
		oapiBlt(surf, srf[SRF_NEEDLE],  0, (90 - (int)((h2flow - 0.05) / 0.1 * 65.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(surf, srf[SRF_NEEDLE],  0, (25 - (int)((h2flow - 0.15) / 0.05 * 21.0)), 0, 0, 10, 10, SURF_PREDEF_CK);

	// Fuel Cell O2 flow
	if (o2flow < 0.0) o2flow = 0.0;
	if (o2flow > 1.6) o2flow = 1.6;
	if (o2flow < 0.4)
		oapiBlt(surf, srf[SRF_NEEDLE],  53, (111 - (int)(o2flow / 0.4 * 21.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (o2flow < 1.2)
		oapiBlt(surf, srf[SRF_NEEDLE],  53, (90 - (int)((o2flow - 0.4) / 0.8 * 65.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(surf, srf[SRF_NEEDLE],  53, (25 - (int)((o2flow - 1.2) / 0.4 * 21.0)), 10, 0, 10, 10, SURF_PREDEF_CK);

	// Fuel Cell skin temperature
	if (temp < 100.0) temp = 100.0;
	if (temp > 550.0) temp = 550.0;
	if (temp < 400.0)
		oapiBlt(surf, srf[SRF_NEEDLE],  86, (109 - (int)((temp - 100.0) / 300.0 * 53.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else if (temp < 500.0)
		oapiBlt(surf, srf[SRF_NEEDLE],  86, (56 - (int)((temp - 400.0) / 100.0 * 40.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(surf, srf[SRF_NEEDLE],  86, (16 - (int)((temp - 500.0) / 50.0 * 12.0)), 0, 0, 10, 10, SURF_PREDEF_CK);

	// Fuel Cell condenser exhaust temperature
	if (condenserTemp < 150.0) condenserTemp = 150.0;
	if (condenserTemp > 250.0) condenserTemp = 250.0;
	oapiBlt(surf, srf[SRF_NEEDLE],  139, (109 - (int)((condenserTemp - 150.0) / 100.0 * 103.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


//
// Free all allocated surfaces.
//

void Saturn::ReleaseSurfaces ()
{
	for (int i = 0; i < nsurf; i++) {
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
	}
}

//
// Load all the surfaces and initialise the panel.
//

void Saturn::InitPanel (int panel)

{
// Panel dependency temporary disabled because of the new panels	
/*	switch (panel) {

	case 0:// ilumination panel left
    case 2:
    case 4://added for splitted panel
		srf[0] = oapiCreateSurface (LOADBMP (IDB_ILMFCSM));
		srf[1] = oapiCreateSurface (LOADBMP (IDB_ILMINDICATORS1));
		srf[2] = oapiCreateSurface (LOADBMP (IDB_NEEDLE1));
		srf[3] = oapiCreateSurface (LOADBMP (IDB_HORIZON));
		srf[4] = oapiCreateSurface (LOADBMP (IDB_DIGITAL));
		srf[5] = oapiCreateSurface (LOADBMP (IDB_HORIZON2));
		srf[6] = oapiCreateSurface (LOADBMP (IDB_ILMSWITCHUP));
		srf[7] = oapiCreateSurface (LOADBMP (IDB_ILMSWLEVER));
		srf[8] = oapiCreateSurface (LOADBMP (IDB_ILMSECSWITCH));
		srf[9] = oapiCreateSurface (LOADBMP (IDB_ILMABORT));
		srf[10] = oapiCreateSurface (LOADBMP (IDB_ILMANNUN));
		srf[11] = oapiCreateSurface (LOADBMP (IDB_ILMLAUNCH));
		srf[12] = oapiCreateSurface (LOADBMP (IDB_ILM_LV_ENG));
		srf[13] = oapiCreateSurface (LOADBMP (IDB_ILMLIGHTS2));
		srf[14] = oapiCreateSurface (LOADBMP (IDB_ANLG_ALT));
		srf[15] = oapiCreateSurface (LOADBMP (IDB_ANLG_GMETER));
		srf[16] = oapiCreateSurface (LOADBMP (IDB_THRUST));
		srf[17] = oapiCreateSurface (LOADBMP (IDB_ILMGUARDSWITCH));
		srf[18] = oapiCreateSurface (LOADBMP (IDB_ILMMASTER_ALARM));
		srf[19] = oapiCreateSurface (LOADBMP (IDB_MASTER_ALARM_BRIGHT));
		//srf[20] = oapiCreateSurface (LOADBMP (IDB_BUTTON));
		srf[SRF_DSKY] = oapiCreateSurface (LOADBMP (IDB_ILMDSKY_LIGHTS));
		srf[22] = oapiCreateSurface (LOADBMP (IDB_ILMALLROUND));
		srf[23] = oapiCreateSurface (LOADBMP (IDB_ILMTHREEPOSSWITCH));
		srf[24] = oapiCreateSurface (LOADBMP (IDB_MFDFRAME_ILM));
		srf[25] = oapiCreateSurface (LOADBMP (IDB_MFDPOWER_ILM));
		srf[26] = oapiCreateSurface (LOADBMP (IDB_DOCKINGSWITCHES_ILM));
		oapiSetSurfaceColourKey (srf[2], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[3], 0);
		oapiSetSurfaceColourKey (srf[5], g_Param.col[5]);
		oapiSetSurfaceColourKey (srf[14], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[15], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[16], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[22], g_Param.col[4]);
		break;

	case 1: // panel
	case 3:
    case 5://added for splitted panel
*/		srf[0]							= oapiCreateSurface (LOADBMP (IDB_FCSM));
		srf[SRF_INDICATOR]				= oapiCreateSurface (LOADBMP (IDB_INDICATOR));
		srf[SRF_NEEDLE]					= oapiCreateSurface (LOADBMP (IDB_NEEDLE));
		srf[3]							= oapiCreateSurface (LOADBMP (IDB_HORIZON));
		srf[4]							= oapiCreateSurface (LOADBMP (IDB_DIGITAL));
		srf[5]							= oapiCreateSurface (LOADBMP (IDB_HORIZON2));
		srf[SRF_SWITCHUP]				= oapiCreateSurface (LOADBMP (IDB_SWITCHUP));
		srf[7]							= oapiCreateSurface (LOADBMP (IDB_SWLEVER));
		srf[SRF_SWITCHGUARDS]			= oapiCreateSurface (LOADBMP (IDB_SWITCHGUARDS));
		srf[SRF_ABORT]					= oapiCreateSurface (LOADBMP (IDB_ABORT));
		srf[10]							= oapiCreateSurface (LOADBMP (IDB_ANNUN));
		srf[11]							= oapiCreateSurface (LOADBMP (IDB_LAUNCH));
		srf[12]							= oapiCreateSurface (LOADBMP (IDB_LV_ENG));
		srf[13]							= oapiCreateSurface (LOADBMP (IDB_LIGHTS2));
		srf[14]							= oapiCreateSurface (LOADBMP (IDB_ANLG_ALT));
		srf[15]							= oapiCreateSurface (LOADBMP (IDB_ANLG_GMETER));
		srf[16]							= oapiCreateSurface (LOADBMP (IDB_THRUST));
		srf[SRF_SEQUENCERSWITCHES]		= oapiCreateSurface (LOADBMP (IDB_SEQUENCERSWITCHES));
		srf[18]							= oapiCreateSurface (LOADBMP (IDB_MASTER_ALARM));
		srf[19]							= oapiCreateSurface (LOADBMP (IDB_MASTER_ALARM_BRIGHT));
		//srf[20]					    = oapiCreateSurface (LOADBMP (IDB_BUTTON));
		srf[SRF_DSKY]					= oapiCreateSurface (LOADBMP (IDB_DSKY_LIGHTS));
		srf[22]							= oapiCreateSurface (LOADBMP (IDB_ALLROUND));
		srf[SRF_THREEPOSSWITCH]			= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH));
		srf[24]							= oapiCreateSurface (LOADBMP (IDB_MFDFRAME));
		srf[25]							= oapiCreateSurface (LOADBMP (IDB_MFDPOWER));
		srf[26]							= oapiCreateSurface (LOADBMP (IDB_DOCKINGSWITCHES));
		srf[SRF_ROTATIONALSWITCH]		= oapiCreateSurface (LOADBMP (IDB_ROTATIONALSWITCH));
		srf[SRF_SUITCABINDELTAPMETER]	= oapiCreateSurface (LOADBMP (IDB_SUITCABINDELTAPMETER));
		srf[SRF_THREEPOSSWITCH305]		= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH305));
				
		oapiSetSurfaceColourKey (srf[SRF_NEEDLE],				g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[3],						0);
		oapiSetSurfaceColourKey (srf[5],						g_Param.col[5]);
		oapiSetSurfaceColourKey (srf[SRF_SWITCHUP],				g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_SWITCHGUARDS],			g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[14],						g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[15],						g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[16],						g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_SEQUENCERSWITCHES],	g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[22],						g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_ROTATIONALSWITCH],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_SUITCABINDELTAPMETER],	g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH305],	g_Param.col[4]);
/*		break;
	}
*/
	SetSwitches(panel);
}

bool Saturn::clbkLoadPanel (int id) {

	static HBITMAP hBmpMain = 0;
	static HBITMAP hBmpLeft = 0;
	static HBITMAP hBmpRight = 0;
	static HBITMAP hBmpLower = 0;
	static HBITMAP hBmpHatch = 0;
	static HBITMAP hBmpRNDZ_Left = 0;
	static HBITMAP hBmpRNDZ_Right = 0;
	static bool recursion;

	// avoid recursive calls
	if (recursion) return true;
	recursion = true;

	if (!InPanel && id != PanelId) {
		// sometimes clbkLoadPanel is called inside oapiSetPanel, 
		// sometimes not, so ignore the recursive call
		oapiSetPanel(PanelId);
		id = PanelId;
	}
	recursion = false;

	ReleaseSurfaces();
	HBITMAP hBmp;

	//
	// Should we display a panel for unmanned flights?
	//

	if (!Crewed)
		return false;

	switch(id) {
	case SATPANEL_LOWER:
//		if (!hBmpLower)
			hBmpLower = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL));
		hBmp = hBmpLower;
		oapiSetPanelNeighbours(-1, -1, SATPANEL_MAIN, -1);
		break;

	case SATPANEL_MAIN:
//		if (!hBmpMain)
			hBmpMain = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_PANEL));
		hBmp = hBmpMain;
		oapiSetPanelNeighbours(SATPANEL_LEFT, SATPANEL_RIGHT, SATPANEL_HATCH_WINDOW, SATPANEL_LOWER);

		break;

	case SATPANEL_LEFT:
//		if (!hBmpLeft)
			hBmpLeft = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_PANEL));
		hBmp = hBmpLeft;
		oapiSetPanelNeighbours(-1, SATPANEL_MAIN, -1, -1);

		break;

	case SATPANEL_RIGHT:
//		if (!hBmpRight)
			hBmpRight = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_PANEL));
		hBmp = hBmpRight;
		oapiSetPanelNeighbours(SATPANEL_MAIN, -1, -1, -1);
		break;

	case SATPANEL_LEFT_RNDZ_WINDOW:
//		if (!hBmpRNDZ_Left)
			hBmpRNDZ_Left = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_RNDZ_WINDOW));
		hBmp = hBmpRNDZ_Left;
		oapiSetPanelNeighbours(-1, SATPANEL_HATCH_WINDOW, -1, SATPANEL_MAIN);
		break;

	case SATPANEL_RIGHT_RNDZ_WINDOW:
//		if (!hBmpRNDZ_Right)
			hBmpRNDZ_Right = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_RNDZ_WINDOW));
		hBmp = hBmpRNDZ_Right;
		oapiSetPanelNeighbours(SATPANEL_HATCH_WINDOW, -1, -1, SATPANEL_MAIN);
		break;

	case SATPANEL_HATCH_WINDOW:
//		if (!hBmpHatch)
			hBmpHatch = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_HATCH_WINDOW));
		hBmp = hBmpHatch;
		oapiSetPanelNeighbours(SATPANEL_LEFT_RNDZ_WINDOW, SATPANEL_RIGHT_RNDZ_WINDOW, -1, SATPANEL_MAIN);
		break;

	}

//old areas as backup and reference to create the new panel areas

		//MFDSPEC mfds_left_l  = {{1012, 770, 1290, 1048}, 6, 6, 41, 27};
		//MFDSPEC mfds_right_l = {{1305, 770, 1583, 1048}, 6, 6, 41, 27};
		//MFDSPEC mfds_left_r  = {{253, 770, 531, 1048}, 6, 6, 41, 27};
		//MFDSPEC mfds_right_r = {{546, 770, 824, 1048}, 6, 6, 41, 27};

		//MFDSPEC mfds_dock = {{893, 627, 1112, 842}, 6, 6, 31, 31};;

		//oapiRegisterMFD (MFD_LEFT,  mfds_left_l);
		//oapiRegisterMFD (MFD_RIGHT, mfds_right_l);

        //oapiRegisterPanelArea (AID_ALTITUDE1,							_R( 547, 425,  643,  440), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ALTIMETER,							_R( 626, 109,  717,  199), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_MASTER_ALARM,						_R( 345, 409,  378,  436), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		//oapiRegisterPanelArea (AID_MASTER_ALARM,						_R(2199, 524, 2232,  551), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		//oapiRegisterPanelArea (AID_GMETER,                              _R( 301, 491,  357,  548), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_HORIZON,								_R( 853, 294,  949,  390), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
		//oapiRegisterPanelArea (AID_HORIZON2,							_R( 440, 537,  536,  633), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
		//oapiRegisterPanelArea (AID_P11_SWITCH,							_R( 256, 512,  279,  532), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SWITCH_HUD,                          _R( 194, 585,  329,  617), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SM_RCS_PANEL1,                       _R( 138, 656,  232,  689), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P12_SWITCH,							_R( 332, 585,  361,  617), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P13,							        _R( 234, 656,  295,  689), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SM_RCS_MODE,                         _R( 298, 656,  360,  689), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SPS_GIMBAL_SWITCHES,                 _R( 343, 905,  487,  937), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_TLI_SWITCH,							_R( 847, 903,  870,  923), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SPS,									_R( 221, 821,  251,  860), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CM_RCS_SWITCH,                       _R(1112, 464, 1137,  509), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CMC_SWITCH,                          _R( 318, 745,  342,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SC_SWITCH,                           _R( 285, 745,  308,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DVA_SWITCH,                          _R( 328, 822,  353,  867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DVB_SWITCH,                          _R( 400, 822,  425,  867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P16,									_R(  75, 739,  191,  770), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SIVB_RCS,                            _R(  94, 810,  190,  830), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ROT_PWR,								_R( 216, 745,  239,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SIVB_GIMBAL,                         _R( 284, 912,  338,  932), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P17,                                 _R( 523, 912,  546,  932), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ELS_SWITCH,                          _R( 554, 899,  579,  944), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P18,                                 _R( 588, 912,  611,  932), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CM_RCS_LOGIC,                        _R( 616, 905,  645,  937), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CMD_SWITCH,                          _R( 651, 899,  676,  944), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CMP_SWITCH,                          _R( 684, 899,  709,  944), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_IMU_SWITCH,                          _R( 215, 964,  240,  1009), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_FCSM_SWITCH,                         _R( 515, 970,  584,  1004), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P19,                                 _R( 247, 970,  436,  1002), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DIRECT_ULLAGE_THRUST_ON_LIGHT,		_R( 273, 808,  303,  878), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_LIGHTS_LAUNCHER,						_R( 612, 727,  718,  817), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_LV_TANK_GAUGES,                      _R( 466, 728,  589,  807), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_GDC_BUTTON,                          _R( 217, 909,  243,  935), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_THRUSTMETER,                         _R( 374, 727,  436,  787), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DSKY_KEY,                            _R( 799, 622, 1010,  711), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SWITCH_JET,                          _R( 841, 739,  964,  784), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_EDS,                                 _R( 808, 752,  831,  772), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_ABORT_ROW,                           _R( 771, 815,  882,  844), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_MAIN_RELEASE_SWITCH,                 _R( 774, 957,  799,  1003), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_TOWER_JET_SWITCH1,                   _R( 900, 808,  925,  853), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_TOWER_JET_SWITCH2,                   _R( 934, 808,  959,  853), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_IU_GUIDANCE_SWITCH,                  _R( 776, 890,  801,  935), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_LV_STAGE_SWITCH,						_R( 810, 890,  835,  935), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_DOCKING_PROBE_SWITCH,				_R(1029, 171, 1055,  280), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P21,                                 _R(1061, 241, 1236,  273), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CSM_SIVB_SEP_SWITCH,                 _R( 595, 825,  664,  856), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_LEM_POWER_SWITCH,					_R(1258, 248, 1281,  268), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SIVB_LEM_SEP_SWITCH,					_R( 968, 739,  994,  785), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SMRCS_HELIUM1_SWITCHES,				_R(1175, 311, 1297,  385), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P22,									_R(1045, 470, 1106,  501), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P23,									_R(1146, 476, 1169,  507), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SMRCS_HELIUM2_SWITCHES,				_R(1175, 428, 1300,  501), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_PRIM_PRPLNT_SWITCHES,				_R(1175, 527, 1300,  609), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P24,									_R(1047, 567, 1172,  609), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P25,									_R(1047, 664, 1108,  705), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SEC_PRPLT_SWITCHES,					_R(1111, 626, 1300,  705), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CAUTION_PANEL,						_R(1420, 331, 1553,  363), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P27,									_R(1502, 398, 1848,  429), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P28,									_R(1324, 586, 1625,  695), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P29,									_R(1620, 735, 1681,  766), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P30,									_R(1766, 894, 1827,  925), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CABIN_PRESS,							_R(1629, 664, 1729,  695), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_FUEL_CELL_RADIATORS,					_R(2089, 438, 2182,  516), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P31,									_R(2090, 550, 2181,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_FUEL_CELL_BUS_SWITCHES,				_R(2184, 601, 2288,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P32,									_R(2294, 648, 2317,  668), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SPS_HELIUM_SWITCHES,					_R(1923, 703, 1984,  775), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P33,									_R(1987, 743, 2074,  775), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P34,									_R(2301, 750, 2324,  770), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_FUEL_CELL_SWITCHES,					_R(2077, 701, 2285,  775), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P35,									_R(1923, 814, 2281,  921), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_P36,									_R(1923, 963, 2326,  994), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CABIN_FAN_SWITCHES,					_R(1417, 398, 1490,  430), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CW,									_R(1141, 109, 1490,  194), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SMRCS_FUEL,                          _R(1034, 328, 1146,  417), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SPS_FUEL,                            _R(1920, 279, 2027,  367), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_EMS_SWITCH,							_R( 437, 333,  467,  364), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_BUS_PANEL,							_R(2361, 814, 2488,  994), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_05G_LIGHT,							_R( 561, 387,  585,  401), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SPS_LIGHT,							_R( 604, 387,  628,  401), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SPS_INJ_VLV,							_R(1924, 425, 2065,  457), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_AUTO_LIGHT,							_R( 464, 969,  480,  995), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CMRCS_HE_DUMP_SWITCH,				_R( 666, 825,  734,  855), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_BUS_LIGHT,							_R(2445, 635, 2461,  651), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CABIN_LIGHT,							_R(1781, 595, 1797,  611), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CABIN_GAUGES,						_R(1693, 483, 1857,  571), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_EMS_KNOB,							_R( 442, 248,  510,  316), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_EMS_DISPLAY,							_R( 545, 264,  647,  369), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_RCS_INDICATORS,					_R(1327, 371, 1392,  436), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);		
        //oapiRegisterMFD (MFD_RIGHT, mfds_dock);	// MFD_USER1
		//oapiRegisterPanelArea (AID_MFDDOCK,	        _R( 851,  613, 1152      ,  864     ), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN, PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_MFDDOCK_POWER,   _R( 635,  845,  655      ,  860     ), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN, PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_SM_RCS_MODE,     _R( 719,  791,  719 + 133,  791 + 73), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN, PANEL_MAP_BACKGROUND);		
	
	switch (id) {
	
	case SATPANEL_LOWER: // guidance & navigation lower equipment bay
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);
		
		oapiRegisterPanelArea (AID_DSKY_DISPLAY,								_R(1582,  341, 1687,  517), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_LIGHTS,									_R(1438,  346, 1540,  466), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_KEY,			                        _R(1418,  536, 1705,  657), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;    
	
	case SATPANEL_MAIN: // main instrument panel
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);
		
		oapiRegisterPanelArea (AID_ABORT_BUTTON,								_R( 862,  600,  924,  631), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SEQUENCERSWITCHES,							_R( 802,  918,  990, 1100), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LV_ENGINE_LIGHTS,							_R( 843,  735,  944,  879), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SEPARATIONSWITCHES,		    				_R(1087,  942, 1340, 1004), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCABINDELTAPMETER,	    				_R(1445,  106, 1491,  150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MISSION_CLOCK,								_R(1835,  305, 1973,  324), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CYROTANKSWITCHES,        					_R(1912,  490, 2488,  520), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CYROTANKINDICATORS,        					_R(2173,  315, 2495,  439), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRDELTAPMETER,       					_R(2069,  726, 2115,  770), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_CABININDICATORS,        						_R(2278,  593, 2504,  717), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLINDICATORS,		    				_R(2763,  319, 2913,  443), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_FUELCELLPHRADTEMPINDICATORS,	  				_R(2822,  490, 3019,  513), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLRADIATORSINDICATORS,    				_R(2822,  539, 2931,  562), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLRADIATORSSWITCHES,    				_R(2816,  607, 2937,  637), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLINDICATORSSWITCH,    				_R(3030,  630, 3114,  714), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLHEATERSSWITCHES,	    				_R(2817,  695, 2938,  725), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_FUELCELLPURGESWITCHES,	    				_R(2815,  817, 3123,  846), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLREACTANTSINDICATORS,    				_R(2823,  893, 3061,  917), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLREACTANTSSWITCHES,    				_R(2757,  955, 3131,  984), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FUELCELLLATCHSWITCHES,	    				_R(2593, 1251, 2670, 1280), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SBAND_NORMAL_SWITCHES,						_R(2593, 1050, 2858, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		
		// display & keyboard (DSKY):		
		oapiRegisterPanelArea (AID_DSKY_DISPLAY,								_R(1239,  589, 1344,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_LIGHTS,									_R(1095,  594, 1197,  714), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_KEY,			                        _R(1075,  784, 1363,  905), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;    

	case SATPANEL_LEFT: // left instrument panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		//new areas to be added soon...
		
		SetCameraDefaultDirection(_V(-1.0, 0.0, 0.0)); 
		break;    

	case SATPANEL_RIGHT: // right instrument panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		oapiRegisterPanelArea (AID_FUELCELLPUMPSSWITCHES,      					_R( 451,  881,  680,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSWITCHES,      				_R( 965, 1428, 1041, 1519), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(1.0, 0.0, 0.0));
		break;    

	case SATPANEL_LEFT_RNDZ_WINDOW: // left rendezvous window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		//the new animated COAS has to be added soon...

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;    

	case SATPANEL_RIGHT_RNDZ_WINDOW: // right rendezvous window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		break;    

	case SATPANEL_HATCH_WINDOW: // hatch window
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0)); // TODO
		break;    	
	}

	InitPanel (id);

	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);

	InVC = false;
	InPanel = true;
	PanelId = id;
	SetView();

	return hBmp != NULL;
}

//
// Wire up all the control panel switches. Note that this can
// be called multiple times, so be sure to initialise all the panel
// and row variables at the start of the function to reset internal
// state..
//

void Saturn::SetSwitches(int panel) {

	MainPanel.Init(0, this, &soundlib, this);

	// SATPANEL_MAIN
	SequencerSwitchesRow.Init(AID_SEQUENCERSWITCHES, MainPanel);
	LiftoffNoAutoAbortSwitch.Init     ( 20,   3, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 81);
	LiftoffNoAutoAbortSwitch.InitGuard(  0,   1, 92, 40, srf[SRF_SEQUENCERSWITCHES]);
	LesMotorFireSwitch.Init			  ( 20,  49, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 119);
	LesMotorFireSwitch.InitGuard      (  0,  47, 92, 40, srf[SRF_SEQUENCERSWITCHES]);
	CanardDeploySwitch.Init           ( 20,  95, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 157);
	CanardDeploySwitch.InitGuard      (  0,  93, 92, 40, srf[SRF_SEQUENCERSWITCHES]);
	CsmLvSepSwitch.Init				  ( 20, 141, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 195);
	CsmLvSepSwitch.InitGuard		  (  0, 139, 92, 40, srf[SRF_SEQUENCERSWITCHES]);
	ApexCoverJettSwitch.Init          (127,   3, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 233);
	ApexCoverJettSwitch.InitGuard     ( 94,   1, 92, 40, srf[SRF_SEQUENCERSWITCHES], 0, 40);
	DrogueDeploySwitch.Init			  (127,  49, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 271);
	DrogueDeploySwitch.InitGuard      ( 94,  47, 92, 40, srf[SRF_SEQUENCERSWITCHES], 0, 40);
	MainDeploySwitch.Init             (127,  95, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 309);
	MainDeploySwitch.InitGuard        ( 94,  93, 92, 40, srf[SRF_SEQUENCERSWITCHES], 0, 40);		
	CmRcsHeDumpSwitch.Init			  (127, 141, 39, 38, srf[SRF_SEQUENCERSWITCHES], SequencerSwitchesRow, 0, 347);
	CmRcsHeDumpSwitch.InitGuard		  ( 94, 139, 92, 40, srf[SRF_SEQUENCERSWITCHES], 0, 40);
	 
	SeparationSwitchesRow.Init(AID_SEPARATIONSWITCHES, MainPanel);
	EDSSwitch.Init				  (  0,	16, 34, 29, srf[SRF_SWITCHUP], SeparationSwitchesRow); 
	CsmLmFinalSep1Switch.Init	  ( 43, 19, 34, 29, srf[SRF_SWITCHUP], SeparationSwitchesRow); 
	CsmLmFinalSep1Switch.InitGuard( 43,  0, 34, 61, srf[SRF_SWITCHGUARDS]);
	CsmLmFinalSep2Switch.Init	  ( 87, 19, 34, 29, srf[SRF_SWITCHUP], SeparationSwitchesRow); 
	CsmLmFinalSep2Switch.InitGuard( 87,  0, 34, 61, srf[SRF_SWITCHGUARDS]);
	CmSmSep1Switch.Init			  (131, 19, 34, 29, srf[SRF_SWITCHUP], SeparationSwitchesRow); 
	CmSmSep1Switch.InitGuard      (131,  0, 34, 61, srf[SRF_SWITCHGUARDS], 68);
	CmSmSep2Switch.Init			  (175, 19, 34, 29, srf[SRF_SWITCHUP], SeparationSwitchesRow); 
	CmSmSep2Switch.InitGuard      (175,  0, 34, 61, srf[SRF_SWITCHGUARDS], 68);
	SivbLmSepSwitch.Init		  (219, 19, 34, 29, srf[SRF_SWITCHUP], SeparationSwitchesRow); 
	SivbLmSepSwitch.InitGuard     (219,  0, 34, 61, srf[SRF_SWITCHGUARDS]);

	CryoTankSwitchesRow.Init(AID_CYROTANKSWITCHES, MainPanel);
	CabinFan1Switch.Init (  0, 0, 34, 29, srf[SRF_SWITCHUP],       CryoTankSwitchesRow); 
	CabinFan2Switch.Init ( 59, 0, 34, 29, srf[SRF_SWITCHUP],       CryoTankSwitchesRow);
	H2Heater1Switch.Init (114, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);
	H2Heater2Switch.Init (157, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);
	O2Heater1Switch.Init (200, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);
	O2Heater2Switch.Init (250, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);
	O2PressIndSwitch.Init(293, 0, 34, 29, srf[SRF_SWITCHUP],       CryoTankSwitchesRow); 
	H2Fan1Switch.Init    (349, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);
	H2Fan2Switch.Init    (413, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);
	O2Fan1Switch.Init    (478, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);
	O2Fan2Switch.Init    (541, 0, 34, 29, srf[SRF_THREEPOSSWITCH], CryoTankSwitchesRow);

	FuelCellPhRadTempIndicatorsRow.Init(AID_FUELCELLPHRADTEMPINDICATORS, MainPanel);
	FuelCellPhIndicator.Init     (  0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellPhRadTempIndicatorsRow);
	FuelCellRadTempIndicator.Init(174, 0, 23, 23, srf[SRF_INDICATOR], FuelCellPhRadTempIndicatorsRow);

	FuelCellRadiatorsIndicatorsRow.Init(AID_FUELCELLRADIATORSINDICATORS, MainPanel);
	FuelCellRadiators1Indicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);
	FuelCellRadiators2Indicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);
	FuelCellRadiators3Indicator.Init(86, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);
	
	FuelCellRadiatorsSwitchesRow.Init(AID_FUELCELLRADIATORSSWITCHES, MainPanel); 
	FuelCellRadiators1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellRadiatorsSwitchesRow); 
	FuelCellRadiators2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellRadiatorsSwitchesRow); 
	FuelCellRadiators3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellRadiatorsSwitchesRow); 

	FuelCellIndicatorsSwitchRow.Init(AID_FUELCELLINDICATORSSWITCH, MainPanel);
	FuelCellIndicatorsSwitch.Init(0, 0, 84, 84, srf[SRF_ROTATIONALSWITCH], FuelCellIndicatorsSwitchRow);

	FuelCellHeatersSwitchesRow.Init(AID_FUELCELLHEATERSSWITCHES, MainPanel);
	FuelCellHeater1Switch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], FuelCellHeatersSwitchesRow); 
	FuelCellHeater2Switch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], FuelCellHeatersSwitchesRow); 
	FuelCellHeater3Switch.Init(86, 0, 34, 29, srf[SRF_SWITCHUP], FuelCellHeatersSwitchesRow); 

	FuelCellPurgeSwitchesRow.Init(AID_FUELCELLPURGESWITCHES, MainPanel);
	FuelCellPurge1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellPurgeSwitchesRow); 
	FuelCellPurge2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellPurgeSwitchesRow); 
	FuelCellPurge3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellPurgeSwitchesRow); 

	FuelCellReactantsIndicatorsRow.Init(AID_FUELCELLREACTANTSINDICATORS, MainPanel);
	FuelCellReactants1Indicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);
	FuelCellReactants2Indicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);
	FuelCellReactants3Indicator.Init(86, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);

	FuelCellReactantsSwitchesRow.Init(AID_FUELCELLREACTANTSSWITCHES, MainPanel); 
	FuelCellReactants1Switch.Init( 43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellReactantsSwitchesRow); 
	FuelCellReactants2Switch.Init( 86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellReactantsSwitchesRow); 
	FuelCellReactants3Switch.Init(129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellReactantsSwitchesRow); 

	FuelCellLatchSwitchesRow.Init(AID_FUELCELLLATCHSWITCHES, MainPanel);
	FCReacsValvesSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], FuelCellLatchSwitchesRow); 
	H2PurgeLineSwitch.Init  (43, 0, 34, 29, srf[SRF_SWITCHUP], FuelCellLatchSwitchesRow); 


	SBandNormalSwitchesRow.Init(AID_SBAND_NORMAL_SWITCHES, MainPanel);
	SBandNormalXPDRSwitch.Init(		  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], SBandNormalSwitchesRow);
	SBandNormalPwrAmpl1Switch.Init(  43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], SBandNormalSwitchesRow);
	SBandNormalPwrAmpl2Switch.Init(  86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], SBandNormalSwitchesRow);
	SBandNormalMode1Switch.Init(	145, 0, 34, 29, srf[SRF_THREEPOSSWITCH], SBandNormalSwitchesRow);
	SBandNormalMode2Switch.Init(	188, 0, 34, 29, srf[SRF_THREEPOSSWITCH], SBandNormalSwitchesRow);
	SBandNormalMode3Switch.Init(	231, 0, 34, 29, srf[SRF_SWITCHUP],		 SBandNormalSwitchesRow);

	
	// SATPANEL_RIGHT
	FuelCellPumpsSwitchesRow.Init(AID_FUELCELLPUMPSSWITCHES, MainPanel);
	FuelCellPumps1Switch.Init(  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellPumpsSwitchesRow); 
	FuelCellPumps2Switch.Init( 65, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellPumpsSwitchesRow); 
	FuelCellPumps3Switch.Init(130, 0, 34, 29, srf[SRF_THREEPOSSWITCH], FuelCellPumpsSwitchesRow); 

	SuitCompressorSwitchesRow.Init(AID_SUITCOMPRESSORSWITCHES, MainPanel);
	SuitCompressor1Switch.Init(  0, 58, 34, 33, srf[SRF_THREEPOSSWITCH305], SuitCompressorSwitchesRow); 
	SuitCompressor2Switch.Init( 42,  0, 34, 33, srf[SRF_THREEPOSSWITCH305], SuitCompressorSwitchesRow); 



	// old stuff
	SPSRow.Init(AID_SPS, MainPanel);
	//EDSRow.Init(AID_EDS, MainPanel);
	LPRow.Init(AID_SWITCH_PANEL_LEFT, MainPanel);
	HUDRow.Init(AID_SWITCH_HUD, MainPanel);
	NAVRow1.Init(AID_SWITCH_AUTO, MainPanel);
	NAVRow2.Init(AID_SWITCH_AUTO2, MainPanel);

	ABTRow.Init(AID_ABORT_ROW, MainPanel);

	P11Row.Init(AID_P11_SWITCH, MainPanel);
	SRP1Row.Init(AID_SM_RCS_PANEL1, MainPanel);
	P12Row.Init(AID_P12_SWITCH, MainPanel);
	P13Row.Init(AID_P13, MainPanel);
	P14Row.Init(AID_SM_RCS_MODE, MainPanel);
	P15Row.Init(AID_SPS_GIMBAL_SWITCHES, MainPanel);
	P16Row.Init(AID_ROT_PWR, MainPanel);
	P17Row.Init(AID_P17, MainPanel);
	P18Row.Init(AID_P18, MainPanel);
	P19Row.Init(AID_P19, MainPanel);
	P21Row.Init(AID_P21, MainPanel);
	P22Row.Init(AID_P22, MainPanel);
	P23Row.Init(AID_P23, MainPanel);
	P24Row.Init(AID_P24, MainPanel);
	P25Row.Init(AID_P25, MainPanel);
	P26Row.Init(AID_CAUTION_PANEL, MainPanel);
	P27Row.Init(AID_P27, MainPanel);
	P28Row.Init(AID_P28, MainPanel);
	P29Row.Init(AID_P29, MainPanel);
	P30Row.Init(AID_P30, MainPanel);
	P31Row.Init(AID_P31, MainPanel);
	P32Row.Init(AID_P32, MainPanel);
	P33Row.Init(AID_P33, MainPanel);
	P34Row.Init(AID_P34, MainPanel);
	P35Row.Init(AID_P35, MainPanel);
	P36Row.Init(AID_P36, MainPanel);
	P37Row.Init(AID_BUS_PANEL, MainPanel);

	EMS1Row.Init(AID_EMS_SWITCH, MainPanel);

	LPSRow.Init(AID_LEM_POWER_SWITCH, MainPanel);

	SPSswitch.Init(0, 0, 30, 39, srf[7], SPSRow);
	//EDSswitch.Init(0, 0, 23, 20, srf[6], EDSRow);

	LPswitch1.Init(36, 7, 23, 20, srf[6], SRP1Row);
	LPswitch2.Init(68, 7, 23, 20, srf[6], SRP1Row);
	LPswitch3.Init(04, 7, 23, 20, srf[6], SRP1Row);

	if (panel == SATPANEL_LEFT_RNDZ_WINDOW) {
		LPswitch4.Init(55, 33, 23, 20, srf[6], P14Row);
		LPswitch5.Init(87, 33, 23, 20, srf[6], P14Row);
	} else {
		LPswitch4.Init(  4, 7, 23, 20, srf[6], P14Row);
		LPswitch5.Init( 36, 7, 23, 20, srf[6], P14Row);
	}

	LPswitch6.Init( 45, 7, 23, 20, srf[6], P15Row);
	P15switch.Init(  9, 7, 23, 20, srf[6], P15Row);
	LPswitch7.Init(119, 7, 23, 20, srf[6], P15Row);
	P16switch.Init( 83, 7, 23, 20, srf[6], P15Row);

	HUDswitch1.Init(  4, 7, 23, 20, srf[6], HUDRow, HUD_DOCKING, soundlib);
	HUDswitch2.Init( 36, 7, 23, 20, srf[6], HUDRow, HUD_SURFACE, soundlib);
	HUDswitch3.Init(110, 7, 23, 20, srf[6], HUDRow, HUD_ORBIT, soundlib);

	NavToggleKillrot.Init(2, 8, 23, 20, srf[6], NAVRow1, this, NAVMODE_KILLROT, soundlib);
	NavTogglePrograde.Init(31, 8, 23, 20, srf[6], NAVRow1, this, NAVMODE_PROGRADE, soundlib);
	NavToggleRetrograde.Init(61, 8, 23, 20, srf[6], NAVRow1, this, NAVMODE_RETROGRADE, soundlib);
	NavToggleNormal.Init(102, 8, 23, 20, srf[6], NAVRow1, this, NAVMODE_NORMAL, soundlib);
	NavToggleAntiNormal.Init(138, 8, 23, 20, srf[6], NAVRow1, this, NAVMODE_ANTINORMAL, soundlib);

	NavToggleHLevel.Init(2, 4, 23, 20, srf[6], NAVRow2, this, NAVMODE_HLEVEL, soundlib);
	NavToggleHAlt.Init(31, 4, 23, 20, srf[6], NAVRow2, this, NAVMODE_HOLDALT, soundlib);

	P11switch.Init( 0, 0, 23, 20, srf[6], P11Row);

	P12switch.Init( 3, 7, 23, 20, srf[6], P12Row);

	P13switch.Init( 4, 7, 23, 20, srf[6], P13Row);
	P14switch.Init(36, 7, 23, 20, srf[6], P13Row);

	P111switch.Init(0, 0, 23, 20, srf[6], P17Row);

	P112switch.Init(0, 0, 23, 20, srf[6], P18Row);

	ROTPswitch.Init(0, 0, 23, 20, srf[23], P16Row);

	P114switch.Init(  4, 6, 23, 20, srf[6], P19Row);
	P115switch.Init( 36, 6, 23, 20, srf[6], P19Row);
	P116switch.Init( 67, 6, 23, 20, srf[6], P19Row);
	P117switch.Init( 99, 6, 23, 20, srf[6], P19Row);
	P118switch.Init(131, 6, 23, 20, srf[23], P19Row);
	P119switch.Init(163, 6, 23, 20, srf[23], P19Row);

	P21switch.Init(04, 6, 23, 20, srf[6], ABTRow);
	P22switch.Init(40, 6, 23, 20, srf[6], ABTRow);
	P23switch.Init(86, 6, 23, 20, srf[6], ABTRow);

	P24switch.Init(  3, 7, 23, 20, srf[23], P21Row);
	P25switch.Init( 35, 7, 23, 20, srf[23], P21Row);
	P26switch.Init( 67, 7, 23, 20, srf[23], P21Row);
	P27switch.Init(110, 7, 23, 20, srf[23], P21Row);
	P28switch.Init(150, 7, 23, 20, srf[23], P21Row);

	P29switch.Init( 3, 7, 23, 20, srf[6], P22Row);
	P210switch.Init(36, 7, 23, 20, srf[6], P22Row);

	P211switch.Init(0, 1, 23, 20, srf[6], P23Row);

	P212switch.Init( 3, 7, 23, 20, srf[23], P24Row);
	P213switch.Init(35, 7, 23, 20, srf[23], P24Row);
	P214switch.Init(67, 7, 23, 20, srf[23], P24Row);
	P215switch.Init(99, 7, 23, 20, srf[23], P24Row);

	P216switch.Init( 3, 7, 23, 20, srf[23], P25Row);
	P217switch.Init(35, 7, 23, 20, srf[23], P25Row);

	P218switch.Init(  3, 7, 23, 20, srf[23], P26Row);
	P219switch.Init( 40, 7, 23, 20, srf[23], P26Row);
	P220switch.Init( 75, 7, 23, 20, srf[23], P26Row);
	P221switch.Init(107, 7, 23, 20, srf[6], P26Row);

	P222switch.Init(  3, 7, 23, 20, srf[23], P27Row);
	P223switch.Init( 35, 7, 23, 20, srf[23], P27Row);
	P224switch.Init( 67, 7, 23, 20, srf[23], P27Row);
	P225switch.Init(104, 7, 23, 20, srf[23], P27Row);
	P226switch.Init(136, 7, 23, 20, srf[23], P27Row);
	P227switch.Init(178, 7, 23, 20, srf[23], P27Row);
	P228switch.Init(226, 7, 23, 20, srf[23], P27Row);
	P229switch.Init(274, 7, 23, 20, srf[23], P27Row);
	P230switch.Init(320, 7, 23, 20, srf[23], P27Row);

	P231switch.Init( 10, 6, 23, 20, srf[23], P28Row);
	P232switch.Init( 48, 6, 23, 20, srf[23], P28Row);
	P233switch.Init( 85, 6, 23, 20, srf[23], P28Row);
	P234switch.Init(122, 6, 23, 20, srf[23], P28Row);
	P235switch.Init(154, 6, 23, 20, srf[23], P28Row);
	P236switch.Init(  4, 84, 23, 20, srf[23], P28Row);
	P237switch.Init( 36, 84, 23, 20, srf[23], P28Row);
	P238switch.Init( 68, 84, 23, 20, srf[23], P28Row);
	P239switch.Init(100, 84, 23, 20, srf[23], P28Row);
	P240switch.Init(132, 84, 23, 20, srf[23], P28Row);
	P241switch.Init(168, 84, 23, 20, srf[23], P28Row);
	P242switch.Init(204, 84, 23, 20, srf[23], P28Row);
	P243switch.Init(236, 84, 23, 20, srf[23], P28Row);
	P244switch.Init(275, 84, 23, 20, srf[23], P28Row);

	P246switch.Init( 3, 6, 23, 20, srf[23], P29Row);
	P247switch.Init(36, 6, 23, 20, srf[23], P29Row);

	P248switch.Init( 3, 6, 23, 20, srf[23], P30Row);
	P249switch.Init(35, 6, 23, 20, srf[23], P30Row);

	P31switch.Init( 3,  8, 23, 20, srf[6], P31Row);
	P32switch.Init(35,  8, 23, 20, srf[6], P31Row);
	P33switch.Init(67,  8, 23, 20, srf[6], P31Row);
	P34switch.Init( 2, 98, 23, 20, srf[23], P31Row);
	P35switch.Init(33, 98, 23, 20, srf[23], P31Row);
	P36switch.Init(66, 98, 23, 20, srf[23], P31Row);

	P37switch.Init(0, 0, 23, 20, srf[23], P32Row);

	P38switch.Init( 3, 7, 23, 20, srf[23], P33Row);
	P39switch.Init(61, 7, 23, 20, srf[23], P33Row);

	P310switch.Init(0, 0, 23, 20, srf[23], P34Row);

	P311switch.Init(  3, 6, 23, 20, srf[23], P35Row);
	P312switch.Init( 36, 6, 23, 20, srf[23], P35Row);
	P313switch.Init( 67, 6, 23, 20, srf[23], P35Row);
	P314switch.Init(112, 6, 23, 20, srf[23], P35Row);
	P315switch.Init(143, 6, 23, 20, srf[23], P35Row);
	P316switch.Init(175, 6, 23, 20, srf[23], P35Row);
	P317switch.Init(214, 6, 23, 20, srf[23], P35Row);
	P318switch.Init(254, 6, 23, 20, srf[23], P35Row);
	P319switch.Init(293, 6, 23, 20, srf[23], P35Row);
	P320switch.Init(333, 6, 23, 20, srf[23], P35Row);
	P321switch.Init(  3, 84, 23, 20, srf[23], P35Row);
	P322switch.Init( 35, 84, 23, 20, srf[23], P35Row);
	P323switch.Init(185, 82, 23, 20, srf[23], P35Row);
	P324switch.Init(224, 82, 23, 20, srf[23], P35Row);
	P325switch.Init(261, 82, 23, 20, srf[23], P35Row);
	P326switch.Init(293, 82, 23, 20, srf[23], P35Row);
	P327switch.Init(325, 82, 23, 20, srf[23], P35Row);

	P328switch.Init(  4, 6, 23, 20, srf[6], P36Row);
	P329switch.Init( 35, 6, 23, 20, srf[6], P36Row);
	P330switch.Init(185, 6, 23, 20, srf[23], P36Row);
	P331switch.Init(218, 6, 23, 20, srf[23], P36Row);
	P332switch.Init(249, 6, 23, 20, srf[23], P36Row);
	P333switch.Init(282, 6, 23, 20, srf[23], P36Row);
	P334switch.Init(313, 6, 23, 20, srf[23], P36Row);
	P335switch.Init(345, 6, 23, 20, srf[23], P36Row);
	P336switch.Init(377, 6, 23, 20, srf[23], P36Row);

	P337switch.Init( 3,   6, 23, 20, srf[23], P37Row);
	P338switch.Init(50,   6, 23, 20, srf[23], P37Row);
	P339switch.Init(96,   6, 23, 20, srf[23], P37Row);
	P340switch.Init( 3,  81, 23, 20, srf[23], P37Row);
	P341switch.Init(35,  81, 23, 20, srf[23], P37Row);
	P342switch.Init(67,  81, 23, 20, srf[23], P37Row);
	P343switch.Init(99,  81, 23, 20, srf[23], P37Row);
	P344switch.Init( 3, 155, 23, 20, srf[23], P37Row);
	P345switch.Init(35, 155, 23, 20, srf[23], P37Row);
	P346switch.Init(67, 155, 23, 20, srf[23], P37Row);
	P347switch.Init(99, 155, 23, 20, srf[23], P37Row);

	EMSswitch.Init(4, 6, 23, 20, srf[23], EMS1Row);

	RPswitch15.Init(0, 0, 23, 20, srf[23], LPSRow);

	IMUswitchRow.Init(AID_IMU_SWITCH, MainPanel);
	IMUswitch.Init( 1, 16, 23, 20, srf[6], IMUswitchRow);	// ToggleSwitch
	//IMUswitch.Init( 1, 16, 23, 20, srf[23], IMUswitchRow);	// ThreePosSwitch
	IMUswitch.InitGuard(0, 0, 25, 45, srf[8]);

	RCSIndicatorsSwitchRow.Init(AID_RCS_INDICATORS, MainPanel);
	RCSIndicatorsSwitch.Init(0, 0, 64, 64, srf[27], RCSIndicatorsSwitchRow);
}

void SetupgParam(HINSTANCE hModule) {

	g_Param.hDLL = hModule;
	// allocate GDI resources
	g_Param.font[0]  = CreateFont (-13, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[2]  = CreateFont (-8, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
	g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
	g_Param.brush[2] = CreateSolidBrush (RGB(154,154,154));  // Grey
	g_Param.brush[3] = CreateSolidBrush (RGB(3,3,3));  // Black
	g_Param.pen[0] = CreatePen (PS_SOLID, 1, RGB(224, 224, 224));
	g_Param.pen[1] = CreatePen (PS_SOLID, 3, RGB(164, 164, 164));
	g_Param.pen[2] = CreatePen (PS_SOLID, 1, RGB( 77,  77,  77));
	g_Param.pen[3] = CreatePen (PS_SOLID, 3, RGB( 77,  77,  77));
	g_Param.pen[4] = CreatePen (PS_SOLID, 3, RGB(  0,   0,   0));
}

void DeletegParam() {

	int i;

	// deallocate GDI resources
	for (i = 0; i < 3; i++) DeleteObject (g_Param.font[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.brush[i]);
	for (i = 0; i < 5; i++) DeleteObject (g_Param.pen[i]);
}

bool Saturn::clbkPanelMouseEvent (int id, int event, int mx, int my)

{
	static int ctrl = 0;

	if (MainPanel.CheckMouseClick(id, event, mx, my))
		return true;

	switch (id) {
	case AID_MASTER_ALARM:
		StopMasterAlarm();
		ButtonClick();
		return true;

	case AID_DSKY_KEY:
		dsky.ProcessKeypress(mx, my);
		return true;

	case AID_GDC_BUTTON:
		if(event & PANEL_MOUSE_LBDOWN){
			if(mx <26 ){
				GDCswitch = !GDCswitch;
				ButtonClick();
				GDCswitch = true;
			}
		}else if(event & PANEL_MOUSE_LBUP){
			if(mx <26 ){
				GDCswitch = !GDCswitch;
				ButtonClick();
				GDCswitch = false;
			}
		}
		return true;

	case AID_DVA_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				DVCswitch = !DVCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && DVCswitch){
				if(my >16 && my <27 && !DVAswitch){
					DVAswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && DVAswitch && DVCswitch){
					DVAswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_DVB_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				DVBCswitch = !DVBCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && DVBCswitch){
				if(my >16 && my <27 && !DVBswitch){
					DVBswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && DVBswitch && DVBCswitch){
					DVBswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_ELS_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				ELSCswitch = !ELSCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && ELSCswitch){
				if(my >16 && my <27 && !ELSswitch){
				ELSswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && ELSswitch && ELSCswitch){
				ELSswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_CMD_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				CMDCswitch = !CMDCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && CMDCswitch){
				if(my >16 && my <27 && !CMDswitch){
					CMDswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && CMDswitch && CMDCswitch){
					CMDswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_CMP_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				CMPCswitch = !CMPCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && CMPCswitch){
				if(my >16 && my <27 && !CMPswitch){
					CMPswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && CMPswitch && CMPCswitch){
					CMPswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

/*	case AID_IMU_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				IMUCswitch = !IMUCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && IMUCswitch){
				if(my >16 && my <27 && !IMUswitch){
					IMUswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && IMUswitch && IMUCswitch){
					IMUswitch = false;
					SwitchClick();
				}
			}
		}
		return true;
*/
	case AID_MAIN_RELEASE_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				MRCswitch = !MRCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && MRCswitch){
				if(my >16 && my <27 && !MRswitch){
					MRswitch = true;
					SwitchClick();
					SetChuteStage4 ();
				}else if(my >27 && my <37 && MRswitch && MRCswitch){
					MRswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_TOWER_JET_SWITCH1:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				TJ1Cswitch = !TJ1Cswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && TJ1Cswitch){
				if(my >16 && my <27 && !TJ1switch){
					TJ1switch = true;
					SwitchClick();
				}else if(my >27 && my <37 && TJ1switch && TJ1Cswitch){
					TJ1switch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_TOWER_JET_SWITCH2:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				TJ2Cswitch = !TJ2Cswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && TJ2Cswitch){
				if(my >16 && my <27 && !TJ2switch){
					TJ2switch = true;
					SwitchClick();
				}else if(my >27 && my <37 && TJ2switch && TJ2Cswitch){
					TJ2switch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_IU_GUIDANCE_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				IUCswitch = !IUCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && IUCswitch){
				if(my >16 && my <27 && !IUswitch){
					IUswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && IUswitch && IUCswitch){
					IUswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_LV_STAGE_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				LVSCswitch = !LVSCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && LVSCswitch){
				if(my >16 && my <27 && !LVSswitch){
					LVSswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && LVSswitch && LVSCswitch){
					LVSswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_SMRCS_HELIUM1_SWITCHES:
		if (my >=49 && my <=60 ){
			if (mx > 9 && mx < 21 && !RH11switch){
				SwitchClick();
				RH11switch=true;
			}else if (mx > 41 && mx < 53 && !RH12switch){
				SwitchClick();
				RH12switch=true;
			}else if (mx > 73 && mx < 85 && !RH13switch){
				SwitchClick();
				RH13switch=true;
			}else if (mx > 105 && mx < 117 && !RH14switch){
				SwitchClick();
				RH14switch=true;
			}
		}else if (my >=59 && my <=70 ){
			if (mx > 9 && mx < 21 && RH11switch){
				SwitchClick();
				RH11switch=false;
			}else if (mx > 41 && mx < 53 && RH12switch){
				SwitchClick();
				RH12switch=false;
			}else if (mx > 73 && mx < 85 && RH13switch){
				SwitchClick();
				RH13switch=false;
			}else if (mx > 105 && mx < 117 && RH14switch){
				SwitchClick();
				RH14switch=false;
			}
		}
		return true;

	case AID_SMRCS_HELIUM2_SWITCHES:
		if (my >=48 && my <=59 ){
			if (mx > 9 && mx < 21 && !RH21switch){
				SwitchClick();
				RH21switch=true;
			}else if (mx > 41 && mx < 53 && !RH22switch){
				SwitchClick();
				RH22switch=true;
			}else if (mx > 73 && mx < 85 && !RH23switch){
				SwitchClick();
				RH23switch=true;
			}else if (mx > 105 && mx < 117 && !RH24switch){
				SwitchClick();
				RH24switch=true;
			}
		}else if (my >=58 && my <=69 ){
			if (mx > 9 && mx < 21 && RH21switch){
				SwitchClick();
				RH21switch=false;
			}else if (mx > 41 && mx < 53 && RH22switch){
				SwitchClick();
				RH22switch=false;
			}else if (mx > 73 && mx < 85 && RH23switch){
				SwitchClick();
				RH23switch=false;
			}else if (mx > 105 && mx < 117 && RH24switch){
				SwitchClick();
				RH24switch=false;
			}
		}
		return true;

	case AID_SEC_PRPLT_SWITCHES:
		if (my >=44 && my <=55 ){
			if (mx > 9 && mx < 21 && !CR1switch){
				SwitchClick();
				CR1switch=true;
			}else if (mx > 41 && mx < 53 && !CR2switch){
				SwitchClick();
				CR2switch=true;
			}else if (mx > 73 && mx < 85 && !SP1switch){
				SwitchClick();
				SP1switch=true;
			}else if (mx > 105 && mx < 117 && !SP2switch){
				SwitchClick();
				SP2switch=true;
			}else if (mx > 137 && mx < 149 && !SP3switch){
				SwitchClick();
				SP3switch=true;
			}else if (mx > 169 && mx < 181 && !SP4switch){
				SwitchClick();
				SP4switch=true;
			}
		}else if (my >=54 && my <=65 ){
			if (mx > 9 && mx < 21 && CR1switch){
				SwitchClick();
				CR1switch=false;
			}else if (mx > 41 && mx < 53 && CR2switch){
				SwitchClick();
				CR2switch=false;
			}else if (mx > 73 && mx < 85 && SP1switch){
				SwitchClick();
				SP1switch=false;
			}else if (mx > 105 && mx < 117 && SP2switch){
				SwitchClick();
				SP2switch=false;
			}else if (mx > 137 && mx < 149 && SP3switch){
				SwitchClick();
				SP3switch=false;
			}else if (mx > 169 && mx < 181 && SP4switch){
				SwitchClick();
				SP4switch=false;
			}
		}
		return true;

	case AID_PRIM_PRPLNT_SWITCHES:
		if (my >=47 && my <=58 ){
			if (mx > 9 && mx < 21 && !PP1switch){
				SwitchClick();
				PP1switch=true;
			}else if (mx > 41 && mx < 53 && !PP2switch){
				SwitchClick();
				PP2switch=true;
			}else if (mx > 73 && mx < 85 && !PP3switch){
				SwitchClick();
				PP3switch=true;
			}else if (mx > 105 && mx < 117 && !PP4switch){
				SwitchClick();
				PP4switch=true;
			}
		}else if (my >=57 && my <=68 ){
			if (mx > 9 && mx < 21 && PP1switch){
				SwitchClick();
				PP1switch=false;
			}else if (mx > 41 && mx < 53 && PP2switch){
				SwitchClick();
				PP2switch=false;
			}else if (mx > 73 && mx < 85 && PP3switch){
				SwitchClick();
				PP3switch=false;
			}else if (mx > 105 && mx < 117 && PP4switch){
				SwitchClick();
				PP4switch=false;
			}
		}
		return true;

	case AID_CABIN_PRESS:
		if (my >=6 && my <=16 ){
			if (mx >75 && mx < 99 && !RPswitch12){
				SwitchClick();
				RPswitch12=true;
			}else if (mx >38 && mx < 62 && !RPswitch13 && RPswitch12){
				SwitchClick();
				RPswitch13=true;
			}else if (mx >5 && mx < 29 && !RPswitch14){
				SwitchClick();
				RPswitch14=true;
			}
	}else if (my >=16 && my <=27 ){
			if (mx >75 && mx < 99 && RPswitch12 && !RPswitch13){
				SwitchClick();
				RPswitch12=false;
			}else if (mx >38 && mx < 62 && RPswitch13 ){
				SwitchClick();
				RPswitch13=false;
			}else if (mx >5 && mx < 29 && RPswitch14){
				SwitchClick();
				RPswitch14=false;
				}
			}

			return true;

	case AID_DOCKING_PROBE_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				DPCswitch = !DPCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && DPCswitch){
				if(my >80 && my <91 && !DPswitch){
					DPswitch = true;
					SwitchClick();
					ProbeSound();
				}else if(my >91 && my <101 && DPswitch && DPCswitch){
					DPswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

/*	case AID_CSM_SIVB_SEP_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <69 ){
				Cswitch5 = !Cswitch5;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <69 && Cswitch5){
				if(my >3 && my <16 && !Sswitch5){
					Sswitch5 = true;
					ButtonClick();
				}else if(my >16 && my <28 && Sswitch5 && Cswitch5){
					Sswitch5 = false;
					ButtonClick();
				}
			}
		}
		return true;

	case AID_CMRCS_HE_DUMP_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <69 ){
				CMRHGswitch = !CMRHGswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <69 && CMRHGswitch){
				if(my >3 && my <16 && !CMRHDswitch){
					CMRHDswitch = true;
					ButtonClick();
				}else if(my >16 && my <28 && CMRHDswitch && CMRHGswitch){
					CMRHDswitch = false;
					ButtonClick();
				}
			}
		}
		return true;

	case AID_SIVB_LEM_SEP_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				RPCswitch = !RPCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && RPCswitch){
				if(my >16 && my <27 && !RPswitch16){
					RPswitch16 = true;
					SwitchClick();
				}else if(my >27 && my <37 && RPswitch16 && RPCswitch){
					RPCswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_SWITCH_JET:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				Cswitch1 = !Cswitch1;
				GuardClick();
			}
			if(mx <57 && mx>33 ){
				Cswitch2 = !Cswitch2;
				GuardClick();
			}
			if(mx <89 && mx>65 ){
				Cswitch3 = !Cswitch3;
				GuardClick();
			}
			if(mx <121 && mx>97 ){
				Cswitch4 = !Cswitch4;
				GuardClick();
			}

		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && Cswitch1){
				if(my >16 && my <26 && !Sswitch1){
					Sswitch1 = true;
					DPswitch = false;
					SwitchClick();
				}else if(my >26 && my <36 && Sswitch1 && Cswitch1){
					Sswitch1 = false;
					SwitchClick();
				}
			}
			if(mx <57 && mx>33 && Cswitch2){
				if(my >16 && my <26 && !Sswitch2){
					Sswitch2 = true;
					DPswitch = false;
					SwitchClick();
				}else if(my >26 && my <36 && Sswitch2 && Cswitch2){
				Sswitch2 = false;
					SwitchClick();
				}
			}
			if(mx <89 && mx>65 && Cswitch3){
				if(my >16 && my <26 && !Sswitch3){
				Sswitch3 = true;
					SwitchClick();
				}else if(my >26 && my <36 && Sswitch3 && Cswitch3){
				Sswitch3 = false;
					SwitchClick();
				}
			}
			if(mx <121 && mx>97 && Cswitch4){
				if(my >16 && my <26 && !Sswitch4){
				Sswitch4 = true;
					SwitchClick();
				}else if(my >26 && my <36 && Sswitch4 && Cswitch4){
				Sswitch4 = false;
					SwitchClick();
				}
			}
		}
		return true;
*/
	case AID_TLI_SWITCH:
	if (my >=0 && my <=10 ){
			    if (mx > 7 && mx < 18 && !TLIswitch){
				SwitchClick();
				TLIswitch=true;
				if (stage != STAGE_ORBIT_SIVB)
					MasterAlarm();
			}
	}else if (my >=10 && my <=20 ){
				if (mx >7 && mx < 18 && TLIswitch){
				SwitchClick();
				TLIswitch=false;
				}
		}
		return true;

	case AID_FCSM_SWITCH:
	if (my >=7 && my <=18 ){
			    if (mx > 14 && mx < 55 && !FCSMswitch){
				SwitchClick();
				FCSMswitch=true;
			}
	}else if (my >=18 && my <=28 ){
				if (mx >14 && mx < 55 && FCSMswitch){
				SwitchClick();
				FCSMswitch=false;
				}
		}
		return true;

	case AID_CM_RCS_SWITCH:
		if(event & PANEL_MOUSE_RBDOWN){
			if(mx <25 ){
				CMRCswitch = !CMRCswitch;
				GuardClick();
			}
		}else if(event & PANEL_MOUSE_LBDOWN){
			if(mx <25 && CMRCswitch){
				if(my >16 && my <27 && !CMRswitch){
					CMRswitch = true;
					SwitchClick();
				}else if(my >27 && my <37 && CMRswitch && CMRCswitch){
					CMRCswitch = false;
					SwitchClick();
				}
			}
		}
		return true;

	case AID_CMC_SWITCH:
		if (my >=0 && my <=10 ){
			if (mx > 7 && mx < 18 && !CMCswitch){
				SwitchClick();
				CMCswitch=true;
			}
			}else if (my >=10 && my <=20 ){
				if (mx >7 && mx < 18 && CMCswitch){
				SwitchClick();
				CMCswitch=false;
				}
			}
		return true;

	case AID_SC_SWITCH:
		if (my >=0 && my <=10 ){
			if (mx > 7 && mx < 18 && !SCswitch){
				SwitchClick();
				SCswitch=true;
			}
			}else if (my >=10 && my <=20 ){
				if (mx >7 && mx < 18 && SCswitch){
				SwitchClick();
				SCswitch=false;
				}
			}
		return true;

	case AID_SIVB_RCS:
		if (my >=0 && my <=10 ){
			if (mx > 44 && mx < 54 && !RPswitch1){
				SwitchClick();
				RPswitch1=true;
			}else if (mx > 80 && mx < 90 && !RPswitch2){
				SwitchClick();
				RPswitch2=true;
			}else if (mx > 7 && mx < 18 && !RPswitch3){
				SwitchClick();
				RPswitch3=true;
			}
		}else if (my >=10 && my <=20 ){
			if (mx >44 && mx < 54 && RPswitch1){
				SwitchClick();
				RPswitch1=false;
			}else if (mx >80 && mx < 90 && RPswitch2){
				SwitchClick();
				RPswitch2=false;
			}else if (mx >7 && mx < 18 && RPswitch3){
				SwitchClick();
				RPswitch3=false;
			}
		}
		return true;

	case AID_SIVB_GIMBAL:
		if (my >=0 && my <=10 ){
			if (mx > 7 && mx < 17 && !RPswitch4){
				SwitchClick();
				RPswitch4=true;
			}else if (mx > 38  && mx < 48 && !RPswitch5){
				SwitchClick();
				RPswitch5=true;
			}
		}else if (my >=10 && my <=20 ){
			if (mx >7 && mx < 17 && RPswitch4){
				SwitchClick();
				RPswitch4=false;
			}else if (mx >38 && mx < 48 && RPswitch5){
				SwitchClick();
				RPswitch5=false;
			}
		}
		return true;

	case AID_ABORT_BUTTON:
		if (mx > 1 && mx < 62 && my > 1 && my < 31){
			bAbort =true;
			ButtonClick();
		}
		return true;

	case AID_P16:
		if (my >=6 && my <=17 ){
			if (mx >9 && mx < 21 && !RPswitch9){
				SwitchClick();
				RPswitch9=true;
			}else if (mx >53 && mx < 65 && !RPswitch7){
				SwitchClick();
				RPswitch7=true;
			}else if (mx >97 && mx < 109 && !RPswitch8){
				SwitchClick();
				RPswitch8=true;
			}
		}else if (my >=16 && my <=27 ){
			if (mx >9 && mx < 21 && RPswitch9){
				SwitchClick();
				RPswitch9=false;
			}else if (mx >53 && mx < 65 && RPswitch7){
				SwitchClick();
				RPswitch7=false;
			}else if (mx >97 && mx < 109 && RPswitch8){
				SwitchClick();
				RPswitch8=false;
			}
		}
		return true;

/*	case AID_FUEL_CELL_RADIATORS:
		if (my >=53 && my <=64 ){
			if (mx > 9 && mx < 21 && !FCRswitch1){
				SwitchClick();
				FCRswitch1=true;
			}else if (mx > 41 && mx < 53 && !FCRswitch2){
				SwitchClick();
				FCRswitch2=true;
			}else if (mx > 74 && mx < 86 && !FCRswitch3){
				SwitchClick();
				FCRswitch3=true;
			}
		}else if (my >=63 && my <=74 ){
			if (mx > 9 && mx < 21 && FCRswitch1){
				SwitchClick();
				FCRswitch1=false;
			}else if (mx > 41 && mx < 53 && FCRswitch2){
				SwitchClick();
				FCRswitch2=false;
			}else if (mx > 74 && mx < 86 && FCRswitch3){
				SwitchClick();
				FCRswitch3=false;
			}
		}
		return true;
*/
	case AID_FUEL_CELL_BUS_SWITCHES:
		if (my >=47 && my <=58 ){
			if (mx > 9 && mx < 21 && !FCBswitch1){
				SwitchClick();
				FCBswitch1=true;
			}else if (mx > 46 && mx < 58 && !FCBswitch2){
				SwitchClick();
				FCBswitch2=true;
			}else if (mx > 84 && mx < 96 && !FCBswitch3){
				SwitchClick();
				FCBswitch3=true;
			}
		}else if (my >=57 && my <=68 ){
			if (mx > 9 && mx < 21 && FCBswitch1){
				SwitchClick();
				FCBswitch1=false;
			}else if (mx > 46 && mx < 58 && FCBswitch2){
				SwitchClick();
				FCBswitch2=false;
			}else if (mx > 84 && mx < 96 && FCBswitch3){
				SwitchClick();
				FCBswitch3=false;
			}
		}
		return true;

	case AID_SPS_HELIUM_SWITCHES:
		if (my >=47 && my <=58 ){
			if (mx > 9 && mx < 21 && !SRHswitch1){
				SwitchClick();
				SRHswitch1=true;
			}else if (mx > 41 && mx < 53 && !SRHswitch2){
				SwitchClick();
				SRHswitch2=true;
			}
		}else if (my >=57 && my <=68 ){
			if (mx > 9 && mx < 21 && SRHswitch1){
				SwitchClick();
				SRHswitch1=false;
			}else if (mx > 41 && mx < 53 && SRHswitch2){
				SwitchClick();
				SRHswitch2=false;
			}
		}
		return true;

/*	case AID_FUEL_CELL_SWITCHES:
		if (my >=49 && my <=60 ){
			if (mx > 9 && mx < 21 && !FCswitch1){
				SwitchClick();
				FCswitch1=true;
			}else if (mx > 41 && mx < 53 && !FCswitch2){
				SwitchClick();
				FCswitch2=true;
			}else if (mx > 74 && mx < 86 && !FCswitch3){
				SwitchClick();
				FCswitch3=true;
			}else if (mx > 105 && mx < 117 && !FCswitch4){
				SwitchClick();
				FCswitch4=true;
			}else if (mx > 146 && mx < 158 && !FCswitch5){
				SwitchClick();
				FCswitch5=true;
			}else if (mx > 189 && mx < 201 && !FCswitch6){
				SwitchClick();
				FCswitch6=true;
			}
		}else if (my >=59 && my <=70 ){
			if (mx > 9 && mx < 21 && FCswitch1){
				SwitchClick();
				FCswitch1=false;
			}else if (mx > 41 && mx < 53 && FCswitch2){
				SwitchClick();
				FCswitch2=false;
			}else if (mx > 74 && mx < 86 && FCswitch3){
				SwitchClick();
				FCswitch3=false;
			}else if (mx > 105 && mx < 117 && FCswitch4){
				SwitchClick();
				FCswitch4=false;
			}else if (mx > 146 && mx < 158 && FCswitch5){
				SwitchClick();
				FCswitch5=false;
			}else if (mx > 189 && mx < 201 && FCswitch6){
				SwitchClick();
				FCswitch6=false;
			}
		}
		return true;
*/
	case AID_CM_RCS_LOGIC:
		if (my >=7 && my <=18 ){
			if (mx > 9 && mx < 21 && !P113switch){
				SwitchClick();
				P113switch=true;
			}
		}else if (my >=17 && my <=28 ){
			if (mx > 9 && mx < 21 && P113switch){
				SwitchClick();
				P113switch=false;
			}
		}
		return true;

	case AID_EMS_KNOB:
		if (my >=1 && my <=34 ){
			if (mx > 1 && mx < 67 && !EMSKswitch){
				SwitchClick();
				EMSKswitch=true;
			}
		}
		else if (my >=34 && my <=67 ){
			if (mx > 1 && mx < 67 && EMSKswitch){
				SwitchClick();
				EMSKswitch=false;
			}
		}
		return true;

	case AID_MFDDOCK:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
			if (my > 234 && my < 249) {	//&& event == PANEL_MOUSE_LBDOWN
				if (mx > 47 && mx < 68) {
					ButtonClick();
					oapiToggleMFD_on (MFD_RIGHT);	// MFD_USER1
				} else if (mx > 208 && mx < 229) {
					ButtonClick();
					oapiSendMFDKey (MFD_RIGHT, OAPI_KEY_F1);		// MFD_USER1
				} else if (mx > 239 && mx < 252) {
					ButtonClick();
					oapiSendMFDKey (MFD_RIGHT, OAPI_KEY_GRAVE);		// MFD_USER1
				}
			} else if (mx > 5 && mx < 26 && my > 38 && my < 208) {
				if ((my - 38) % 31 < 14) {
					int bt = (my - 38) / 31 + 0;
					ButtonClick();
					oapiProcessMFDButton (MFD_RIGHT, bt, event);	// MFD_USER1
				}
			} else if (mx > 273 && mx < 294 && my > 38 && my < 208) {
				if ((my - 38) % 31 < 14) {
					int bt = (my - 38) / 31 + 6;
					ButtonClick();
					oapiProcessMFDButton (MFD_RIGHT, bt, event);	// MFD_USER1
				}
			}
		}
		return true;

	case AID_MFDDOCK_POWER:
		if (oapiGetMFDMode(MFD_RIGHT) == MFD_NONE) {	// MFD_USER1
			ButtonClick();
			oapiToggleMFD_on (MFD_RIGHT);	// MFD_USER1
		}
		return true;
		break;
	}
	return false;
}


//#include "PanelSDK/Internals/Esystems.h"

void Saturn::PanelSwitchToggled(ToggleSwitch *s) {

	if (s == &CabinFan1Switch || s == &CabinFan2Switch) {
		if (CabinFansActive()) {
			CabinFanSound();
		} else {
			StopCabinFanSound();
		}

		// TEST
		/*if (s == &CabinFan1Switch) {
			FCell *fc = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1");
			if (CabinFan1Switch) {
				fc->PLOAD(20.0);
			} else {
				fc->PUNLOAD(20.0);
			}
		}

		if (s == &CabinFan2Switch) {
			FCell *fc = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2");
			if (CabinFan2Switch) {
				fc->PLOAD(40.0);
			} else {
				fc->PUNLOAD(40.0);
			}
		}*/

	} else if (s == &O2Heater1Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1HEATER:PUMP"));

	} else if (s == &O2Heater2Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2HEATER:PUMP"));

	} else if (s == &H2Heater1Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1HEATER:PUMP"));

	} else if (s == &H2Heater2Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2HEATER:PUMP"));

	} else if (s == &O2Fan1Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1FAN:PUMP"));

	} else if (s == &O2Fan2Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2FAN:PUMP"));

	} else if (s == &H2Fan1Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1FAN:PUMP"));

	} else if (s == &H2Fan2Switch) {
		CryoTankHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2FAN:PUMP"));


	} else if (s == &FuelCellRadiators1Switch) {
		if (FuelCellRadiators1Switch.IsUp())
			FuelCellCoolingBypass(1, false);
		else if (FuelCellRadiators1Switch.IsDown())
			FuelCellCoolingBypass(1, true);

	} else if (s == &FuelCellRadiators2Switch) {
		if (FuelCellRadiators2Switch.IsUp())
			FuelCellCoolingBypass(2, false);
		else if (FuelCellRadiators2Switch.IsDown())
			FuelCellCoolingBypass(2, true);

	} else if (s == &FuelCellRadiators3Switch) {
		if (FuelCellRadiators3Switch.IsUp())
			FuelCellCoolingBypass(3, false);
		else if (FuelCellRadiators3Switch.IsDown())
			FuelCellCoolingBypass(3, true);
		

	} else if (s == &FuelCellHeater1Switch) {
		FuelCellHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1HEATER:PUMP"));

	} else if (s == &FuelCellHeater2Switch) {
		FuelCellHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2HEATER:PUMP"));

	} else if (s == &FuelCellHeater3Switch) {
		FuelCellHeaterSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3HEATER:PUMP"));


	} else if (s == &FuelCellPurge1Switch || s == &FuelCellPurge2Switch || s == &FuelCellPurge3Switch || s == &H2PurgeLineSwitch) {
		if (s == &FuelCellPurge1Switch || s == &H2PurgeLineSwitch) {
			FuelCellPurgeSwitchToggled(&FuelCellPurge1Switch, 
				(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:PURGE"));		
		} 
		if (s == &FuelCellPurge2Switch || s == &H2PurgeLineSwitch) {
			FuelCellPurgeSwitchToggled(&FuelCellPurge2Switch, 
				(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:PURGE"));		
		} 
		if (s == &FuelCellPurge3Switch || s == &H2PurgeLineSwitch) {
			FuelCellPurgeSwitchToggled(&FuelCellPurge3Switch, 
				(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:PURGE"));
		}


	} else if (s == &FuelCellReactants1Switch) {
		FuelCellReactantsSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:START"));
	
	} else if (s == &FuelCellReactants2Switch) {
		FuelCellReactantsSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:START"));
	
	} else if (s == &FuelCellReactants3Switch) {
		FuelCellReactantsSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:START"));


	} else if (s == &FuelCellPumps1Switch) {
		FuelCellPumpsSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING:PUMP"));
		
	} else if (s == &FuelCellPumps2Switch) {
		FuelCellPumpsSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2COOLING:PUMP"));

	} else if (s == &FuelCellPumps3Switch) {
		FuelCellPumpsSwitchToggled(s, 
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3COOLING:PUMP"));

		
	} else if (s == &SuitCompressor1Switch || s == & SuitCompressor2Switch) {
		int *pump = (int*) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER:PUMP");
		// No busses at the moment
		if (SuitCompressor1Switch.IsCenter() && SuitCompressor2Switch.IsCenter())
			*pump = SP_PUMP_OFF;
		else
			*pump = SP_PUMP_AUTO;
	}
}

void Saturn::PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) {

	double *running;

	if (s == &FuelCellPhIndicator) {
		FuelCellPhIndicator = true;	// Not simulated at the moment

	} else if (s == &FuelCellRadTempIndicator) {
		double *temp;
		if (FuelCellIndicatorsSwitch == 1)
			temp = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING:TEMP");
		else if (FuelCellIndicatorsSwitch == 2)
			temp = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2COOLING:TEMP");
		else if (FuelCellIndicatorsSwitch == 3)
			temp = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3COOLING:TEMP");

		if (KelvinToFahrenheit(*temp) <= -30.0)	// indication if temperature below -30°F 
			FuelCellRadTempIndicator = false;
		else
			FuelCellRadTempIndicator = true;
			

	} else if (s == &FuelCellRadiators1Indicator) {
		FuelCellRadiators1Indicator = !FuelCellCoolingBypassed(1);

	} else if (s == &FuelCellRadiators2Indicator) {
		FuelCellRadiators2Indicator = !FuelCellCoolingBypassed(2);

	} else if (s == &FuelCellRadiators3Indicator) {
		FuelCellRadiators3Indicator = !FuelCellCoolingBypassed(3);


	} else if (s == &FuelCellReactants1Indicator) {
		running = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:RUNNING");
		if (*running) FuelCellReactants1Indicator = false; 
		         else FuelCellReactants1Indicator = true;

	} else if (s == &FuelCellReactants2Indicator) {
		running = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:RUNNING");
		if (*running) FuelCellReactants2Indicator = false; 
		         else FuelCellReactants2Indicator = true;

	} else if (s == &FuelCellReactants3Indicator) {
		running = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:RUNNING");
		if (*running) FuelCellReactants3Indicator = false; 
		         else FuelCellReactants3Indicator = true;
	
	} 
}

void Saturn::CryoTankHeaterSwitchToggled(ToggleSwitch *s, int *pump) {

	if (s->IsUp())
		*pump = SP_PUMP_AUTO;
	else if (s->IsCenter())
		*pump = SP_PUMP_OFF;
	else if (s->IsDown())
		*pump = SP_PUMP_ON;
}

void Saturn::FuelCellHeaterSwitchToggled(ToggleSwitch *s, int *pump) {

	if (s->IsUp())
		*pump = SP_PUMP_AUTO;
	else 
		*pump = SP_PUMP_OFF;
}

void Saturn::FuelCellReactantsSwitchToggled(ToggleSwitch *s, int *start) {

	// Is the reactants valve latched?
	if (!FCReacsValvesSwitch) return;

	if (s->IsUp())
		*start = SP_FUELCELL_START;
	else if (s->IsCenter())
		*start = SP_FUELCELL_NONE;
	else if (s->IsDown())
		*start = SP_FUELCELL_STOP;
}

void Saturn::FuelCellPurgeSwitchToggled(ToggleSwitch *s, int *start) {

	if (s->IsUp()) {
		if (H2PurgeLineSwitch.GetState())
			*start = SP_FUELCELL_H2PURGE;
		else 
			*start = SP_FUELCELL_NOPURGE;
	} else if (s->IsCenter())
		*start = SP_FUELCELL_NOPURGE;
	else if (s->IsDown())
		*start = SP_FUELCELL_O2PURGE;
}

void Saturn::FuelCellPumpsSwitchToggled(ToggleSwitch *s, int *pump) {

	// No busses at the moment
	if (s->IsUp() || s->IsDown())
		*pump = SP_PUMP_AUTO;
	else if (s->IsCenter())
		*pump = SP_PUMP_OFF;
}

void Saturn::SwitchClick()

{
	Sclick.play(NOLOOP,255);
}

void Saturn::ButtonClick()

{
	Bclick.play(NOLOOP,255);
}

void Saturn::GuardClick()

{
	Gclick.play(NOLOOP,255);
}

void Saturn::ProbeSound()

{
	Psound.play(NOLOOP,255);
}

void Saturn::CabinFanSound()

{
	CabinFans.play(LOOP,255);
}

void Saturn::StopCabinFanSound()

{
	CabinFans.stop();
}

//
// Signal a master alarm until reset.
//

void Saturn::MasterAlarm()

{
	if (!masterAlarm) {

		//
		// Enable master alarm, and set it to unlit with cycle time at zero. The next
		// timestep call will automatically set it to lit.
		//

		masterAlarm = true;
		masterAlarmLit = false;
		masterAlarmCycleTime = 0;
	}
}

//
// And stop it.
//

void Saturn::StopMasterAlarm()

{
	if (masterAlarm) {
		masterAlarm = false;
		masterAlarmLit = false;
		SMasterAlarm.stop();
	}
}

bool Saturn::clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf) {

	//
	// Special handling illuminated "sequencer switches"
	//

	if (LAUNCHIND[0]){
		if (EDSSwitch.GetState())
			LiftoffNoAutoAbortSwitch.SetOffset(78, 81);
		else
			LiftoffNoAutoAbortSwitch.SetOffset(234, 81);

	} else {
		LiftoffNoAutoAbortSwitch.SetOffset(0, 81);
	}

	//
	// Special handling for docking panel
	//

	if (id == AID_SM_RCS_MODE) {
		if (PanelId == SATPANEL_LEFT_RNDZ_WINDOW) {
			if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
				oapiBlt(surf, srf[26], 0, 0, 0, 0, 133, 73);
				LPswitch4.SetVisible(true);
				LPswitch5.SetVisible(true);
			} else {
				LPswitch4.SetVisible(false);
				LPswitch5.SetVisible(false);
			}
		} else {
			LPswitch4.SetVisible(true);
			LPswitch5.SetVisible(true);
		}

	}

	//
	// Process all the generic switches.
	//

	if (MainPanel.DrawRow(id, surf))
		return true;

	//
	// Now special case the rest.
	//

	switch (id) {
	case AID_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_DSKY]);
		return true;

	case AID_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[4]);
		return true;

	case AID_ABORT_BUTTON:
		if (ABORT_IND) {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 62, 0, 62, 31);
		} else {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 0, 0, 62, 31);
		}
		return true;

	case AID_SUITCABINDELTAPMETER:
		RedrawPanel_SuitCabinDeltaPMeter(surf);

	case AID_CYROTANKINDICATORS:
		RedrawPanel_CryoTankIndicators(surf);
		return true;

	case AID_SUITCOMPRDELTAPMETER:
		RedrawPanel_SuitComprDeltaPMeter(surf);
		return true;

	case AID_CABININDICATORS:
		RedrawPanel_CabinIndicators(surf);
		return true;

	case AID_FUELCELLINDICATORS:
		RedrawPanel_FuelCellIndicators(surf);
		return true;

	case AID_LV_ENGINE_LIGHTS:
		if (ENGIND[0]) {
			oapiBlt(surf,srf[12],55,44,55,44,27,27);
		} else {
			oapiBlt(surf,srf[12],55,44,157,44,27,27);
		}
		if (ENGIND[1]) {
			oapiBlt(surf,srf[12],55,98,55,98,27,27);
		} else {
			oapiBlt(surf,srf[12],55,98,157,98,27,27);
		}
		if (ENGIND[2]) {
			oapiBlt(surf,srf[12],20,98,20,98,27,27);
		} else {
			oapiBlt(surf,srf[12],20,98,122,98,27,27);
		}
		if (ENGIND[3]) {
			oapiBlt(surf,srf[12],20,44,20,44,27,27);
		} else {
			oapiBlt(surf,srf[12],20,44,122,44,27,27);
		}
		if (ENGIND[4]) {
			oapiBlt(surf,srf[12],37,71,37,71,27,27);
		} else {
			oapiBlt(surf,srf[12],37,71,140,71,27,27);
		}
		if (ENGIND[5]) {
			oapiBlt(surf,srf[12],6,4,6,4,27,27);
		} else {
			oapiBlt(surf,srf[12],6,4,108,4,27,27);
		}
		if (SIISepState) {
			oapiBlt(surf,srf[12],37,4,37,4,27,27);
		} else {
			oapiBlt(surf,srf[12],37,4,139,4,27,27);
		}
		if (AutopilotLight) {
			oapiBlt(surf,srf[12],69,4,69,4,27,27);
		} else {
			oapiBlt(surf,srf[12],69,4,171,4,27,27);
		}
		return true;

	case AID_MISSION_CLOCK:
		int TmpCLKHR, TmpCLKMNT, TmpCLKsec;
		int Curdigit, Curdigit2;
		double cTime;

		cTime = MissionTime;
		if (cTime < 0)
			cTime = (-cTime);

		TmpCLKHR = (int)(cTime/3600.0);
		TmpCLKMNT = (((int)cTime-(TmpCLKHR*3600))/60);
		TmpCLKsec = (((int)cTime-((TmpCLKHR*3600)+TmpCLKMNT*60)));

		// Hour display on three digit
		Curdigit=TmpCLKHR/100;
		Curdigit2=TmpCLKHR/1000;
		oapiBlt(surf,srf[4],0,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
		Curdigit=TmpCLKHR/10;
		Curdigit2=TmpCLKHR/100;
		oapiBlt(surf,srf[4],0+17,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
		Curdigit=TmpCLKHR;
		Curdigit2=TmpCLKHR/10;
		oapiBlt(surf,srf[4],0+34,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
		oapiBlt(surf,srf[4],0+54,0, 192,0,4,19);
		// Minute display on five digit
		Curdigit=TmpCLKMNT/10;
		Curdigit2=TmpCLKMNT/100;
		oapiBlt(surf,srf[4],0+61,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
		Curdigit=TmpCLKMNT;
		Curdigit2=TmpCLKMNT/10;
		oapiBlt(surf,srf[4],0+78,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
		oapiBlt(surf,srf[4],0+98,0, 192,0,4,19);
		// second display on five digit
		Curdigit=TmpCLKsec/10;
		Curdigit2=TmpCLKsec/100;
		oapiBlt(surf,srf[4],0+105,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);
		Curdigit=TmpCLKsec;
		Curdigit2=TmpCLKsec/10;
		oapiBlt(surf,srf[4],0+122,0, 16*(Curdigit-(Curdigit2*10)),0,16,19);

		return true;
		


	// old stuff
	case AID_HORIZON:
		RedrawPanel_Horizon (surf);
		return true;

	case AID_HORIZON2:
		RedrawPanel_Horizon (surf);
		return true;

	case AID_ALTIMETER:
		RedrawPanel_Alt(surf);
		return true;

	case AID_GMETER  :
		RedrawPanel_G(surf);
		return true;

	case AID_THRUSTMETER  :
		RedrawPanel_Thrust(surf);
		return true;

	case AID_DVA_SWITCH:
			if(DVCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(DVAswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			DVAswitch=false;
		}
		return true;

	case AID_DVB_SWITCH:
			if(DVBCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(DVBswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			DVBswitch=false;
		}
		return true;

	case AID_ELS_SWITCH:
			if(ELSCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(ELSswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			ELSswitch=false;
		}
		return true;

	case AID_CMD_SWITCH:
			if(CMDCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(CMDswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			CMDswitch=false;
		}
		return true;

	case AID_CMP_SWITCH:
			if(CMPCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(CMPswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			CMPswitch=false;
		}
		return true;

/*	case AID_IMU_SWITCH:
			if(IMUCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(IMUswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			IMUswitch=false;
		}
		return true;
*/
	case AID_MAIN_RELEASE_SWITCH:
			if(MRCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(MRswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			MRswitch=false;
		}
		return true;

	case AID_TOWER_JET_SWITCH1:
			if(TJ1Cswitch){
			oapiBlt(surf,srf[8],0,0,125,0,25,45);
			if(TJ1switch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,100,0,25,45);
			TJ1switch=false;
		}
		return true;

	case AID_TOWER_JET_SWITCH2:
			if(TJ2Cswitch){
			oapiBlt(surf,srf[8],0,0,125,0,25,45);
			if(TJ2switch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}
			else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,100,0,25,45);
			TJ2switch=false;
		}
		return true;

	case AID_IU_GUIDANCE_SWITCH:
		if(IUCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(IUswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}
			else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}
		else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			IUswitch=false;
		}
		return true;

	case AID_LV_STAGE_SWITCH:
			if(LVSCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(LVSswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			LVSswitch=false;
		}
		return true;

	case AID_DOCKING_PROBE_SWITCH:
			if(DPCswitch){
			oapiBlt(surf,srf[8],0,64,125,0,25,45);
			oapiBlt(surf,srf[13],7,0,0,0,19,20);
			oapiBlt(surf,srf[13],7,21,0,0,19,20);
			if(DPswitch){
				oapiBlt(surf,srf[6],1,80,0,0,23,20);
				oapiBlt(surf,srf[13],7,0,38,0,19,20);
				oapiBlt(surf,srf[13],7,21,38,0,19,20);
			}else{
				oapiBlt(surf,srf[6],1,80,23,0,23,20);
				oapiBlt(surf,srf[13],7,0,0,0,19,20);
				oapiBlt(surf,srf[13],7,21,0,0,19,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,64,100,0,25,45);
			oapiBlt(surf,srf[13],7,0,0,0,19,20);
			oapiBlt(surf,srf[13],7,21,0,0,19,20);
			DPswitch=false;
		}
		return true;

/*	case AID_CSM_SIVB_SEP_SWITCH:
			if(Cswitch5){
			oapiBlt(surf,srf[17],0,0,0,32,69,30);
			if(Sswitch5){
				oapiBlt(surf,srf[17],17,3,70,25,26,24);
			}else{
				oapiBlt(surf,srf[17],17,3,70,0,26,24);
			}
		}else{
			oapiBlt(surf,srf[17],0,0,0,0,69,31);
			Sswitch5=false;
		}
		return true;

	case AID_CMRCS_HE_DUMP_SWITCH:
			if(CMRHGswitch){
			oapiBlt(surf,srf[17],0,0,124,32,68,30);
			if(CMRHDswitch){
				oapiBlt(surf,srf[17],26,3,97,25,26,24);
			}else{
				oapiBlt(surf,srf[17],26,3,97,0,26,24);
			}
		}else{
			oapiBlt(surf,srf[17],0,0,124,0,68,31);
			CMRHDswitch=false;
		}
		return true;

	case AID_SIVB_LEM_SEP_SWITCH:
		if(RPCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(RPswitch16){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			RPswitch16=false;
		}
		return true;

	case AID_SWITCH_JET:
		if(Cswitch1){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(Sswitch1){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			Sswitch1=false;
		}
		if(Cswitch2){
			oapiBlt(surf,srf[8],32,0,25,0,25,45);
			if(Sswitch2){
			oapiBlt(surf,srf[6],33,16,0,0,23,20);
			}else{
			oapiBlt(surf,srf[6],33,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],32,0,0,0,25,45);
			Sswitch2=false;
		}
		if(Cswitch3){
			oapiBlt(surf,srf[8],64,0,75,0,25,45);
			if(Sswitch3){
			oapiBlt(surf,srf[6],65,16,0,0,23,20);
			}else{
			oapiBlt(surf,srf[6],65,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],64,0,50,0,25,45);
			Sswitch3=false;
		}
		if(Cswitch4){
			oapiBlt(surf,srf[8],96,0,75,0,25,45);
			if(Sswitch4){
			oapiBlt(surf,srf[6],97,16,0,0,23,20);
			}else{
			oapiBlt(surf,srf[6],97,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],96,0,50,0,25,45);
			Sswitch4=false;
		}
		return true;

	case AID_INDICATOR1:
		if(stage < CSM_LEM_STAGE){
			if (autopilot){
				oapiBlt(surf,srf[1],0,0,105,0,105,18);
			}else{
				oapiBlt(surf,srf[1],0,0,210,0,105,18);
			}
		}else{
			oapiBlt(surf,srf[1],0,0,0,0,105,18);
		}
		if(GetNavmodeState(NAVMODE_KILLROT)){
			oapiBlt(surf,srf[1],0,91,105,95,105,18);
		}else{
			oapiBlt(surf,srf[1],0,91,0,95,105,18);
		}
		if(GetNavmodeState(NAVMODE_PROGRADE)){
			oapiBlt(surf,srf[1],0,19,105,21,105,18);
		}else{
			oapiBlt(surf,srf[1],0,19,0,21,105,18);
		}if(GetNavmodeState(NAVMODE_RETROGRADE)){
			oapiBlt(surf,srf[1],0,37,105,39,105,18);
		}else{
			oapiBlt(surf,srf[1],0,37,0,39,105,18);
		}if(GetNavmodeState(NAVMODE_NORMAL)){
			oapiBlt(surf,srf[1],0,54,105,56,105,18);
		}else{
			oapiBlt(surf,srf[1],0,54,0,56,105,18);
		}if(GetNavmodeState(NAVMODE_ANTINORMAL)){
			oapiBlt(surf,srf[1],0,72,105,74,105,18);
		}else{
			oapiBlt(surf,srf[1],0,72,0,74,105,18);
		}
		if(GetNavmodeState(NAVMODE_HLEVEL)){
			oapiBlt(surf,srf[1],0,108,105,112,105,18);
		}else{
			oapiBlt(surf,srf[1],0,108,0,112,105,18);
		}
		if(GetNavmodeState(NAVMODE_HOLDALT)){
			oapiBlt(surf,srf[1],0,126,105,129,105,14);
		}else{
			oapiBlt(surf,srf[1],0,126,0,129,105,14);
		}
		return true;

	case AID_INDICATOR2:

		if(GetNavmodeState(NAVMODE_KILLROT)){
			oapiBlt(surf,srf[1],0,69,105,95,105,18);
		}else{
			oapiBlt(surf,srf[1],0,69,0,95,105,18);
		}
		if(GetNavmodeState(NAVMODE_PROGRADE)){
			oapiBlt(surf,srf[1],0,1,105,21,105,18);
		}else{
			oapiBlt(surf,srf[1],0,1,0,21,105,18);
		}
		if(GetNavmodeState(NAVMODE_RETROGRADE)){
			oapiBlt(surf,srf[1],0,19,105,39,105,18);
		}else{
			oapiBlt(surf,srf[1],0,19,0,39,105,18);
		}
		if(GetNavmodeState(NAVMODE_NORMAL)){
			oapiBlt(surf,srf[1],0,35,105,56,105,18);
		}else{
			oapiBlt(surf,srf[1],0,35,0,56,105,18);
		}
		if(GetNavmodeState(NAVMODE_ANTINORMAL)){
			oapiBlt(surf,srf[1],0,52,105,74,105,18);
		}else{
			oapiBlt(surf,srf[1],0,52,0,74,105,18);
		}

		return true;
*/
	case AID_GAUGES1:
		double DispValue;
		if (!ph_sps){
			if(stage < CSM_LEM_STAGE){
				DispValue = 1;
			}else{
				DispValue = 0;
			}
		}else{
			DispValue = GetPropellantMass(ph_sps)/20500;
		}
		oapiBlt(surf,srf[2],0,(90-(int)(DispValue*90.0)),0,0,6,4);//
		oapiBlt(surf,srf[2],31,(90-(int)(DispValue*90.0)),10,0,6,4);//
		if (!ph_rcs0){
			if(stage < CSM_LEM_STAGE){
				DispValue = 1;
			}else{
				DispValue = 0;
			}
		}else{
			DispValue = GetPropellantMass(ph_rcs0)/500;
		}
		oapiBlt(surf,srf[2],65,(90-(int)(DispValue*90.0)),0,0,6,4);//
		if (!ph_rcs1){
			if(stage < CM_ENTRY_STAGE){
				DispValue = 1;
			}else{
				DispValue = 0;
			}
		}else{
		DispValue = GetPropellantMass(ph_rcs1)/500;
		}
		oapiBlt(surf,srf[2],96,(int)(90-(DispValue*90.0)),10,0,6,4);//
		return true;

	case AID_LV_TANK_GAUGES:
		if (!ph_2nd){
			if(stage < LAUNCH_STAGE_SIVB){
				DispValue = 1;
			}else{
				DispValue = 0;
			}
		}else{
			DispValue = GetPropellantMass(ph_2nd)/SII_FuelMass;
		}
		oapiBlt(surf,srf[2],11,(67-(int)(DispValue*67.0))+3,8,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],21,(67-(int)(DispValue*67.0))+3,0,0,7,7, SURF_PREDEF_CK);//
		if (!ph_3rd){
			if(stage < LAUNCH_STAGE_SIVB){
				DispValue = 1;
			}else{
				DispValue = 0;
			}
		}else{
			DispValue = GetPropellantMass(ph_3rd)/S4B_FuelMass;
		}
		oapiBlt(surf,srf[2],39,(67-(int)(DispValue*67.0))+3,8,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],49,(67-(int)(DispValue*67.0))+3,0,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],67,(67-(int)(DispValue*67.0))+3,8,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],77,(67-(int)(DispValue*67.0))+3,0,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],96,(67-(int)(DispValue*67.0))+3,8,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],105,(67-(int)(DispValue*67.0))+3,0,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_SMRCS_FUEL:
		if (!ph_rcs0){
			if(stage < CSM_LEM_STAGE){
				DispValue = 1;
			}else{
				DispValue = 0;
			}
		}else{
			DispValue = GetPropellantMass(ph_rcs0)/500;
		}
		oapiBlt(surf,srf[2],  0,(38-(int)(DispValue*38.0))+40,0,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2], 36,(60-(int)(DispValue*60.0))+17,8,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2], 67,(75-(int)(DispValue*75.0))+ 2,0,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],104,(75-(int)(DispValue*75.0))+ 2,8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_SPS_FUEL:
		if (!ph_sps){
			if(stage < CSM_LEM_STAGE){
				DispValue = 1;
			}else{
				DispValue = 0;
			}
		}else{
			DispValue = GetPropellantMass(ph_sps)/20500;
		}
		oapiBlt(surf,srf[2],  0,(40-(int)(DispValue*40.0))+40,0,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2], 36,(63-(int)(DispValue*63.0))+17,8,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2], 64,(78-(int)(DispValue*78.0))+ 2,0,0,7,7, SURF_PREDEF_CK);//
		oapiBlt(surf,srf[2],100,(78-(int)(DispValue*78.0))+ 2,8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_ALTITUDE1:
		int TmpALT;
		double tmpALTdec;
		int tmpalt2;
		if (actualALT > 999999){
		actualALT = actualALT /1000000;
		oapiBlt(surf,srf[4],85,0,130,0,10,15);
		}else if (actualALT > 9999){
		actualALT =actualALT /1000;
		oapiBlt(surf,srf[4],85,0,120,0,10,15);
		}

		TmpALT = (int)actualALT;
		tmpALTdec = actualALT-TmpALT;
		tmpalt2 = (int)(tmpALTdec *100.0);

		Curdigit=(int)actualALT/10000;
		oapiBlt(surf,srf[4],0,0,10*Curdigit,0,10,15);
		Curdigit=(int)actualALT/1000;
		Curdigit2=(int)actualALT/10000;
		oapiBlt(surf,srf[4],10,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=(int)actualALT/100;
		Curdigit2=(int)actualALT/1000;
		oapiBlt(surf,srf[4],20,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=(int)actualALT/10;
		Curdigit2=(int)actualALT/100;
		oapiBlt(surf,srf[4],30,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=(int)actualALT;
		Curdigit2=(int)actualALT/10;
		oapiBlt(surf,srf[4],40,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		oapiBlt(surf,srf[4],50,11,140,8,10,4);//dot display

		Curdigit=tmpalt2/10 ;
		Curdigit2=tmpalt2 /100;
		oapiBlt(surf,srf[4],60,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=tmpalt2 ;
		Curdigit2=tmpalt2 /10;
		oapiBlt(surf,srf[4],70,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		return true;

	case AID_TLI_SWITCH:
		if(TLIswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_CM_RCS_SWITCH:
		if(CMRCswitch){
			oapiBlt(surf,srf[8],0,0,25,0,25,45);
			if(CMRswitch){
				oapiBlt(surf,srf[6],1,16,0,0,23,20);
			}else{
				oapiBlt(surf,srf[6],1,16,23,0,23,20);
			}
		}else{
			oapiBlt(surf,srf[8],0,0,0,0,25,45);
			CMRswitch=false;
		}
		return true;

	case AID_CMC_SWITCH:
		if(CMCswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_SC_SWITCH:
		if(SCswitch){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_SIVB_RCS:
		if(RPswitch1){
			oapiBlt(surf,srf[6],37,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],37,0,23,0,23,20);
		}
		if(RPswitch2){
			oapiBlt(surf,srf[6],73,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],73,0,23,0,23,20);
		}
		if(RPswitch3){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		return true;

	case AID_SIVB_GIMBAL:
		if(RPswitch4){
			oapiBlt(surf,srf[6],0,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],0,0,23,0,23,20);
		}
		if(RPswitch5){
			oapiBlt(surf,srf[6],31,0,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],31,0,23,0,23,20);
		}
		return true;

	case AID_P16:
		if(RPswitch9){
			oapiBlt(surf,srf[6],3,6,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],3,6,23,0,23,20);
		}
		if(RPswitch7){
			oapiBlt(surf,srf[6],47,6,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],47,6,23,0,23,20);
		}
		if(RPswitch8){
			oapiBlt(surf,srf[6],91,6,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],91,6,23,0,23,20);
		}
		return true;

	case AID_SMRCS_HELIUM1_SWITCHES:
		if(RH11switch){
			oapiBlt(surf,srf[6],3,49,0,0,23,20);
			oapiBlt(surf,srf[13],5,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,49,23,0,23,20);
			oapiBlt(surf,srf[13],5,1,38,0,19,20);
		}
		if(RH12switch){
			oapiBlt(surf,srf[6],35,49,0,0,23,20);
			oapiBlt(surf,srf[13],36,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],35,49,23,0,23,20);
			oapiBlt(surf,srf[13],36,1,38,0,19,20);
		}
		if(RH13switch){
			oapiBlt(surf,srf[6],67,49,0,0,23,20);
			oapiBlt(surf,srf[13],68,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],67,49,23,0,23,20);
			oapiBlt(surf,srf[13],68,1,38,0,19,20);
		}
		if(RH14switch){
			oapiBlt(surf,srf[6],99,49,0,0,23,20);
			oapiBlt(surf,srf[13],99,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],99,49,23,0,23,20);
			oapiBlt(surf,srf[13],99,1,38,0,19,20);
		}
		return true;

	case AID_SMRCS_HELIUM2_SWITCHES:
		if(RH21switch){
			oapiBlt(surf,srf[6],3,49,0,0,23,20);
			oapiBlt(surf,srf[13],5,2,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,49,23,0,23,20);
			oapiBlt(surf,srf[13],5,2,38,0,19,20);
		}
		if(RH22switch){
			oapiBlt(surf,srf[6],35,49,0,0,23,20);
			oapiBlt(surf,srf[13],36,2,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],35,49,23,0,23,20);
			oapiBlt(surf,srf[13],36,2,38,0,19,20);
		}
		if(RH23switch){
			oapiBlt(surf,srf[6],67,49,0,0,23,20);
			oapiBlt(surf,srf[13],68,2,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],67,49,23,0,23,20);
			oapiBlt(surf,srf[13],68,2,38,0,19,20);
		}
		if(RH24switch){
			oapiBlt(surf,srf[6],99,49,0,0,23,20);
			oapiBlt(surf,srf[13],99,2,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],99,49,23,0,23,20);
			oapiBlt(surf,srf[13],99,2,38,0,19,20);
		}
		return true;

	case AID_SEC_PRPLT_SWITCHES:
		if(CR1switch){
			oapiBlt(surf,srf[6],3,44,0,0,23,20);
			oapiBlt(surf,srf[13],4,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,44,23,0,23,20);
			oapiBlt(surf,srf[13],4,1,38,0,19,20);
		}
		if(CR2switch){
			oapiBlt(surf,srf[6],35,44,0,0,23,20);
			oapiBlt(surf,srf[13],35,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],35,44,23,0,23,20);
			oapiBlt(surf,srf[13],35,1,38,0,19,20);
		}
		if(SP1switch){
			oapiBlt(surf,srf[6],67,44,0,0,23,20);
			oapiBlt(surf,srf[13],67,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],67,44,23,0,23,20);
			oapiBlt(surf,srf[13],67,1,38,0,19,20);
		}
		if(SP2switch){
			oapiBlt(surf,srf[6],99,44,0,0,23,20);
			oapiBlt(surf,srf[13],99,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],99,44,23,0,23,20);
			oapiBlt(surf,srf[13],99,1,38,0,19,20);
		}
		if(SP3switch){
			oapiBlt(surf,srf[6],131,44,0,0,23,20);
			oapiBlt(surf,srf[13],130,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],131,44,23,0,23,20);
			oapiBlt(surf,srf[13],130,1,38,0,19,20);
		}
		if(SP4switch){
			oapiBlt(surf,srf[6],163,44,0,0,23,20);
			oapiBlt(surf,srf[13],162,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],163,44,23,0,23,20);
			oapiBlt(surf,srf[13],162,1,38,0,19,20);
		}
		return true;

	case AID_PRIM_PRPLNT_SWITCHES:
		if(PP1switch){
			oapiBlt(surf,srf[6],3,47,0,0,23,20);
			oapiBlt(surf,srf[13],5,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,47,23,0,23,20);
			oapiBlt(surf,srf[13],5,1,38,0,19,20);
		}
		if(PP2switch){
			oapiBlt(surf,srf[6],35,47,0,0,23,20);
			oapiBlt(surf,srf[13],36,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],35,47,23,0,23,20);
			oapiBlt(surf,srf[13],36,1,38,0,19,20);
		}
		if(PP3switch){
			oapiBlt(surf,srf[6],67,47,0,0,23,20);
			oapiBlt(surf,srf[13],68,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],67,47,23,0,23,20);
			oapiBlt(surf,srf[13],68,1,38,0,19,20);
		}
		if(PP4switch){
			oapiBlt(surf,srf[6],99,47,0,0,23,20);
			oapiBlt(surf,srf[13],99,1,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],99,47,23,0,23,20);
			oapiBlt(surf,srf[13],99,1,38,0,19,20);
		}
		return true;

	case AID_CABIN_PRESS:
		if(RPswitch12){
			oapiBlt(surf,srf[6],75,6,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],75,6,23,0,23,20);
		}
		if(RPswitch13){
			oapiBlt(surf,srf[6],38,6,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],38,6,23,0,23,20);
		}
		if(RPswitch14){
			oapiBlt(surf,srf[6],5,6,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],5,6,23,0,23,20);
		}
		return true;

/*	case AID_CABIN_GAUGES:
		if(RPswitch12){
			oapiBlt(surf,srf[22],0,0,0,0,164,88);
		}
		return true;

	case AID_FUEL_CELL_RADIATORS:
		if(FCRswitch1){
			oapiBlt(surf,srf[6],3,53,0,0,23,20);
			oapiBlt(surf,srf[13],6,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,53,23,0,23,20);
			oapiBlt(surf,srf[13],6,0,38,0,19,20);
		}
		if(FCRswitch2){
			oapiBlt(surf,srf[6],35,53,0,0,23,20);
			oapiBlt(surf,srf[13],37,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],35,53,23,0,23,20);
			oapiBlt(surf,srf[13],37,0,38,0,19,20);
		}
		if(FCRswitch3){
			oapiBlt(surf,srf[6],68,53,0,0,23,20);
			oapiBlt(surf,srf[13],69,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],68,53,23,0,23,20);
			oapiBlt(surf,srf[13],69,0,38,0,19,20);
		}
		return true;
*/
	case AID_FUEL_CELL_BUS_SWITCHES:
		if(FCBswitch1){
			oapiBlt(surf,srf[6],3,47,0,0,23,20);
			oapiBlt(surf,srf[13],8,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,47,23,0,23,20);
			oapiBlt(surf,srf[13],8,0,38,0,19,20);
		}
		if(FCBswitch2){
			oapiBlt(surf,srf[6],40,47,0,0,23,20);
			oapiBlt(surf,srf[13],39,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],40,47,23,0,23,20);
			oapiBlt(surf,srf[13],39,0,38,0,19,20);
		}
		if(FCBswitch3){
			oapiBlt(surf,srf[6],78,47,0,0,23,20);
			oapiBlt(surf,srf[13],71,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],78,47,23,0,23,20);
			oapiBlt(surf,srf[13],71,0,38,0,19,20);
		}
		return true;

	case AID_SPS_HELIUM_SWITCHES:
		if(SRHswitch1){
			oapiBlt(surf,srf[6],3,47,0,0,23,20);
			oapiBlt(surf,srf[13],6,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,47,23,0,23,20);
			oapiBlt(surf,srf[13],6,0,38,0,19,20);
		}
		if(SRHswitch2){
			oapiBlt(surf,srf[6],35,47,0,0,23,20);
			oapiBlt(surf,srf[13],36,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],35,47,23,0,23,20);
			oapiBlt(surf,srf[13],36,0,38,0,19,20);
		}
		return true;

/*	case AID_FUEL_CELL_SWITCHES:
		if(FCswitch1){
			oapiBlt(surf,srf[6],3,49,0,0,23,20);
			oapiBlt(surf,srf[13],19,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],3,49,23,0,23,20);
			oapiBlt(surf,srf[13],19,0,38,0,19,20);
		}
		if(FCswitch2){
			oapiBlt(surf,srf[6],35,49,0,0,23,20);
			oapiBlt(surf,srf[13],50,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],35,49,23,0,23,20);
			oapiBlt(surf,srf[13],50,0,38,0,19,20);
		}
		if(FCswitch3){
			oapiBlt(surf,srf[6],68,49,0,0,23,20);
			oapiBlt(surf,srf[13],81,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],68,49,23,0,23,20);
			oapiBlt(surf,srf[13],81,0,38,0,19,20);
		}
		if(FCswitch4){
			oapiBlt(surf,srf[6],99,49,0,0,23,20);
			oapiBlt(surf,srf[13],113,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],99,49,23,0,23,20);
			oapiBlt(surf,srf[13],113,0,38,0,19,20);
		}
		if(FCswitch5){
			oapiBlt(surf,srf[6],140,49,0,0,23,20);
			oapiBlt(surf,srf[13],145,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],140,49,23,0,23,20);
			oapiBlt(surf,srf[13],145,0,38,0,19,20);
		}
		if(FCswitch6){
			oapiBlt(surf,srf[6],183,49,0,0,23,20);
			oapiBlt(surf,srf[13],177,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],183,49,23,0,23,20);
			oapiBlt(surf,srf[13],177,0,38,0,19,20);
		}
		return true;
*/
	case AID_CW:
		if(P221switch){
			oapiBlt(surf,srf[22],0,0,165,0,349,84);
		}else{
			oapiBlt(surf,srf[22],0,0,165,85,349,84);
		}
		return true;

	case AID_05G_LIGHT:
		if(P115switch){
			oapiBlt(surf,srf[22],0,0,600,68,24,13);
		}else{
			oapiBlt(surf,srf[22],0,0,600,82,24,13);
		}
		return true;

	case AID_SPS_LIGHT:
		if(SPSswitch){
			oapiBlt(surf,srf[22],0,0,625,68,24,13);
		}else{
			oapiBlt(surf,srf[22],0,0,625,82,24,13);
		}
		return true;

	case AID_SPS_INJ_VLV:
		if(SPSswitch){
			oapiBlt(surf,srf[22],0,0,0,168,141,32);
		}else{
			oapiBlt(surf,srf[22],0,0,0,135,141,32);
		}
		return true;

	case AID_AUTO_LIGHT:
		if(P221switch){
			oapiBlt(surf,srf[22],0,0,142,152,16,16);
		}else{
			oapiBlt(surf,srf[22],0,0,142,135,16,16);
		}
		return true;

	case AID_BUS_LIGHT:
		if(P221switch){
			oapiBlt(surf,srf[22],0,0,142,152,16,16);
		}else{
			oapiBlt(surf,srf[22],0,0,142,135,16,16);
		}
		return true;

	case AID_CABIN_LIGHT:
		if(P221switch){
			oapiBlt(surf,srf[22],0,0,142,152,16,16);
		}else{
			oapiBlt(surf,srf[22],0,0,142,135,16,16);
		}
		return true;

	case AID_CM_RCS_LOGIC:
		if(P113switch){
			oapiBlt(surf,srf[6],3,7,0,0,23,20);
		}else{
			oapiBlt(surf,srf[6],3,7,23,0,23,20);
		}
		return true;

	case AID_EMS_KNOB:
		if(EMSKswitch){
			oapiBlt(surf,srf[22],0,0,516,0,67,67);
		}else{
			oapiBlt(surf,srf[22],0,0,583,0,67,67);
		}
		return true;

	case AID_EMS_DISPLAY:
		if(EMSKswitch){
			oapiBlt(surf,srf[22],0,0,651,0,100,104);
		}
		return true;

	case AID_FCSM_SWITCH:
		if(FCSMswitch){
			oapiBlt(surf,srf[0],0,0,0,0,68,33);
		}else{
			oapiBlt(surf,srf[0],0,0,68,0,68,33);
		}
		return true;

	case AID_MASTER_ALARM:
		if (masterAlarmLit) {
			oapiBlt (surf, srf[19], 0, 0, 0, 0, 33, 27);
		}
		else {
			oapiBlt (surf, srf[18], 0, 0, 0, 0, 33, 27);
		}
		return true;

	case AID_DIRECT_ULLAGE_THRUST_ON_LIGHT:
		if (LAUNCHIND[6]){
			oapiBlt(surf,srf[10],3,3,27,0,26,26);
		}else{
			oapiBlt(surf,srf[10],3,3,0,0,26,26);
		}
		if (LAUNCHIND[7]){
			oapiBlt(surf,srf[10],3,42,27,27,26,26);
		}else{
			oapiBlt(surf,srf[10],3,42,0,27,26,26);
		}
		return true;

/*	case AID_LIGHTS_LAUNCHER:
		if (LAUNCHIND[0]){
			oapiBlt(surf,srf[11],0,0,56,0,27,22);
		}else{
			oapiBlt(surf,srf[11],0,0,0,0,27,22);
		}
		if (LAUNCHIND[1]){
			oapiBlt(surf,srf[11],79,0,84,0,27,22);
		}else{
			oapiBlt(surf,srf[11],79,0,28,0,27,22);
		}
		if (LAUNCHIND[2]){
			oapiBlt(surf,srf[11],0,34,56,23,27,22);
		}else{
			oapiBlt(surf,srf[11],0,34,0,23,27,22);
		}
		if (LAUNCHIND[3]){
		oapiBlt(surf,srf[11],79,34,84,23,27,22);
		}else{
		oapiBlt(surf,srf[11],79,34,28,23,27,22);
		}
		if (LAUNCHIND[4]){
		oapiBlt(surf,srf[11],0,68,56,46,27,22);
		}else{
		oapiBlt(surf,srf[11],0,68,0,46,27,22);
		}
		if (LAUNCHIND[5]){
		oapiBlt(surf,srf[11],79,68,84,46,27,22);
		}else{
		oapiBlt(surf,srf[11],79,68,28,46,27,22);
		}
		return true;
*/
	case AID_MFDDOCK:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
			oapiBlt(surf,srf[24], 0, 0, 0, 0, 301, 251);

			RedrawPanel_MFDButton (surf, MFD_RIGHT, 0, 7, 38);	// MFD_USER1
			RedrawPanel_MFDButton (surf, MFD_RIGHT, 1, 274, 38);	// MFD_USER1
		}
		return true;

	case AID_MFDDOCK_POWER:
		if (oapiGetMFDMode(MFD_RIGHT) == MFD_NONE) {	// MFD_USER1
			oapiBlt(surf, srf[25], 0, 0, 0, 0, 20, 15);
		}
		return true;
	}
	return false;
}

void Saturn::clbkMFDMode (int mfd, int mode) {

	switch (mfd) {
	case MFD_RIGHT:		// MFD_USER1
		oapiTriggerPanelRedrawArea (SATPANEL_LEFT_RNDZ_WINDOW, AID_MFDDOCK);
		break;
	}
}

//
// Set switches to default state.
//

void Saturn::InitSwitches() {

	LiftoffNoAutoAbortSwitch.Register(PSH, "LiftoffNoAutoAbortSwitch", false, false);
	LesMotorFireSwitch.Register(PSH, "LesMotorFireSwitch", false, false);
	CanardDeploySwitch.Register(PSH, "CanardDeploySwitch", false, false);
	CsmLvSepSwitch = false;						// saved in SSwitchState.Sswitch5
	CsmLvSepSwitch.SetGuardState(false);		// saved in CSwitchState.Cswitch5
	ApexCoverJettSwitch.Register(PSH, "ApexCoverJettSwitch", false, false);
	DrogueDeploySwitch.Register(PSH, "DrogueDeploySwitch", false, false);
	MainDeploySwitch.Register(PSH, "MainDeploySwitch", false, false);
	CmRcsHeDumpSwitch = false;					// saved in SSwitchState.CMRHDswitch
	CmRcsHeDumpSwitch.SetGuardState(false);		// saved in CSwitchState.CMRHGswitch

	EDSSwitch = true;							// saved in LPSwitchState.EDSswitch
	CsmLmFinalSep1Switch = false;				// saved in SSwitchState.Sswitch1
	CsmLmFinalSep1Switch.SetGuardState(false);	// saved in CSwitchState.Cswitch1
	CsmLmFinalSep1Switch.SetSpringLoaded(SPRINGLOADEDSWITCH_DOWN);
	CsmLmFinalSep2Switch = false;				// saved in SSwitchState.Sswitch2
	CsmLmFinalSep2Switch.SetGuardState(false);	// saved in CSwitchState.Cswitch2
	CsmLmFinalSep2Switch.SetSpringLoaded(SPRINGLOADEDSWITCH_DOWN);
	CmSmSep1Switch = false;						// saved in SSwitchState.Sswitch3
	CmSmSep1Switch.SetGuardState(false);		// saved in CSwitchState.Cswitch3
	CmSmSep1Switch.SetSpringLoaded(SPRINGLOADEDSWITCH_DOWN);
	CmSmSep2Switch = false;						// saved in SSwitchState.Sswitch4
	CmSmSep2Switch.SetGuardState(false);		// saved in CSwitchState.Cswitch4
	CmSmSep2Switch.SetSpringLoaded(SPRINGLOADEDSWITCH_DOWN);
	SivbLmSepSwitch = false;					// saved in RPSwitchState.RPswitch16
	SivbLmSepSwitch.SetGuardState(false);		// saved in RPSwitchState.RPCswitch
	SivbLmSepSwitch.SetSpringLoaded(SPRINGLOADEDSWITCH_DOWN);

	CabinFan1Switch = false;					// saved in CP2SwitchState.CFswitch1
	CabinFan2Switch = false;					// saved in CP2SwitchState.CFswitch2
	H2Heater1Switch.Register(PSH, "H2Heater1Switch", THREEPOSSWITCH_UP);
	H2Heater2Switch.Register(PSH, "H2Heater2Switch", THREEPOSSWITCH_UP);
	O2Heater1Switch.Register(PSH, "O2Heater1Switch", THREEPOSSWITCH_UP);
	O2Heater2Switch.Register(PSH, "O2Heater2Switch", THREEPOSSWITCH_UP);
	O2PressIndSwitch.Register(PSH, "O2PressIndSwitch", true);
	H2Fan1Switch.Register(PSH, "H2Fan1Switch", THREEPOSSWITCH_UP);
	H2Fan2Switch.Register(PSH, "H2Fan2Switch", THREEPOSSWITCH_UP);
	O2Fan1Switch.Register(PSH, "O2Fan1Switch", THREEPOSSWITCH_UP);
	O2Fan2Switch.Register(PSH, "O2Fan2Switch", THREEPOSSWITCH_UP);

	FuelCellPhIndicator.Register(PSH, "FuelCellPhIndicator", false);
	FuelCellRadTempIndicator.Register(PSH, "FuelCellRadTempIndicator", false);

	FuelCellRadiators1Indicator.Register(PSH, "FuelCellRadiators1Indicator", false);
	FuelCellRadiators2Indicator.Register(PSH, "FuelCellRadiators2Indicator", false);
	FuelCellRadiators3Indicator.Register(PSH, "FuelCellRadiators3Indicator", false);
	FuelCellRadiators1Switch.Register(PSH, "FuelCellRadiators1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER); 
	FuelCellRadiators2Switch.Register(PSH, "FuelCellRadiators2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER); 
	FuelCellRadiators3Switch.Register(PSH, "FuelCellRadiators3Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER); 

	FuelCellIndicatorsSwitch.AddPosition(1, 330);
	FuelCellIndicatorsSwitch.AddPosition(2,   0);
	FuelCellIndicatorsSwitch.AddPosition(3,  30);
	FuelCellIndicatorsSwitch.Register(PSH, "FuelCellIndicatorsSwitch", 1);

	FuelCellHeater1Switch.Register(PSH, "FuelCellHeater1Switch", true);
	FuelCellHeater2Switch.Register(PSH, "FuelCellHeater2Switch", true);
	FuelCellHeater3Switch.Register(PSH, "FuelCellHeater3Switch", true);

	FuelCellPurge1Switch.Register(PSH, "FuelCellPurge1Switch", THREEPOSSWITCH_CENTER);
	FuelCellPurge2Switch.Register(PSH, "FuelCellPurge2Switch", THREEPOSSWITCH_CENTER);
	FuelCellPurge3Switch.Register(PSH, "FuelCellPurge3Switch", THREEPOSSWITCH_CENTER);

	FuelCellReactants1Indicator.Register(PSH, "FuelCellReactants1Indicator", false);
	FuelCellReactants2Indicator.Register(PSH, "FuelCellReactants2Indicator", false);
	FuelCellReactants3Indicator.Register(PSH, "FuelCellReactants3Indicator", false);
	FuelCellReactants1Switch.Register(PSH, "FuelCellReactants1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellReactants2Switch.Register(PSH, "FuelCellReactants2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellReactants3Switch.Register(PSH, "FuelCellReactants3Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	FCReacsValvesSwitch.Register(PSH, "FCReacsValvesSwitch", true);
	H2PurgeLineSwitch.Register  (PSH, "H2PurgeLineSwitch",   false);

	FuelCellPumps1Switch.Register(PSH, "FuelCellPumps1Switch", THREEPOSSWITCH_UP); 
	FuelCellPumps2Switch.Register(PSH, "FuelCellPumps2Switch", THREEPOSSWITCH_UP); 
	FuelCellPumps3Switch.Register(PSH, "FuelCellPumps3Switch", THREEPOSSWITCH_UP); 

	SuitCompressor1Switch.Register(PSH, "SuitCompressor1Switch", THREEPOSSWITCH_CENTER);   
	SuitCompressor2Switch.Register(PSH, "SuitCompressor2Switch", THREEPOSSWITCH_CENTER);   
	
	SBandNormalXPDRSwitch.Register(PSH, "SBandNormalXPDRSwitch", THREEPOSSWITCH_CENTER);
	SBandNormalPwrAmpl1Switch.Register(PSH, "SBandNormalPwrAmpl1Switch", THREEPOSSWITCH_CENTER);
	SBandNormalPwrAmpl2Switch.Register(PSH, "SBandNormalPwrAmpl2Switch", THREEPOSSWITCH_CENTER);
	SBandNormalMode1Switch.Register(PSH, "SBandNormalMode1Switch", THREEPOSSWITCH_CENTER);
	SBandNormalMode2Switch.Register(PSH, "SBandNormalMode2Switch", THREEPOSSWITCH_CENTER);
	SBandNormalMode3Switch.Register(PSH, "SBandNormalMode3Switch", false);
	
	
	// old stuff

	//Cswitch1=false;
	//Cswitch2=false;
	//Cswitch3=false;
	//Cswitch4=false;
	//Cswitch5=false;
	Cswitch6=false;
	Cswitch7=false;
	Cswitch8=false;
	Cswitch9=false;
	//Sswitch1=false;
	//Sswitch2=false;
	//Sswitch3=false;
	//Sswitch4=false;
	//Sswitch5=false;
	Sswitch6=false;
	Sswitch7=false;
	Sswitch8=false;
	Sswitch9=false;
	RPswitch1=false;
	RPswitch2=false;
	RPswitch3=true;
	RPswitch4=true;
	RPswitch5=true;
	RPswitch6=false;
	CMRswitch=false;
	CMRCswitch=false;

	RPswitch7 = false;
	RPswitch8 = false;
	RPswitch9 = false;
	RPswitch10=false;
	RPswitch11=false;
	RPswitch12=false;
	RPswitch13=false;
	RPswitch14=false;
	RPswitch15=1;
	//RPswitch16=false;
	RPswitch17 = false;
	//RPCswitch=false;

	DPSwitch1 = false;
	DPSwitch2 = false;
	DPSwitch3 = false;
	DPSwitch4 = false;
	DPSwitch5 = false;
	DPSwitch6 = false;
	DPSwitch7 = false;
	DPSwitch8 = false;
	DPSwitch9 = false;
	DPSwitch10 = false;

	GDCswitch = false;

	LPswitch1 = false;
	LPswitch2 = false;
	LPswitch3 = false;
	LPswitch4 = true;

	LPswitch5.SetActive(false);

	SPSswitch = false;
	//EDSswitch = true;

	P11switch = false;
	P12switch = false;
	P13switch = false;

	TLIswitch = false;

	DVAswitch = false;
	DVCswitch = false;
	DVBswitch = false;
	DVBCswitch = false;

	ELSswitch = false;
	ELSCswitch = false;

	CMDswitch = false;
	CMDCswitch = false;

	CMPswitch = false;
	CMPCswitch = false;

	IMUswitch = false;
	IMUswitch.SetGuardState(false);
	//IMUCswitch = false;

	P111switch = false;
	P112switch = false;
	P113switch = false;

	ROTPswitch = 1;

	P114switch = false;
	P115switch = false;
	P116switch = false;
	P117switch = false;
	P118switch = 1;
	P119switch = 1;

	P21switch = true;
	P22switch = true;
	P23switch = true;

	P24switch = 1;
	P25switch = 1;
	P26switch = 0;
	P27switch = 1;
	P28switch = 0;

	P29switch = false;
	P210switch = false;

	P211switch = false;

	P212switch = 1;
	P213switch = 1;
	P214switch = 1;
	P215switch = 1;

	P216switch = 1;
	P217switch = 1;

	P218switch = 1;
	P219switch = 0;
	P220switch = 1;
	P221switch = false;

	P222switch = 1;
	P223switch = 1;
	P224switch = 1;
	P225switch = 1;
	P226switch = 0;
	P227switch = 1;
	P228switch = 1;
	P229switch = 1;
	P230switch = 1;

	P231switch = 1;
	P232switch = 1;
	P233switch = 1;
	P234switch = 1;
	P235switch = 0;
	P236switch = 1;
	P237switch = 1;
	P238switch = 1;
	P239switch = 1;
	P240switch = 1;
	P241switch = 1;
	P242switch = 0;
	P243switch = 0;
	P244switch = 0;

	P246switch = 1;
	P247switch = 1;

	P248switch = 1;
	P249switch = 0;

	P31switch = false;
	P32switch = false;
	P33switch = false;
	P34switch = 1;
	P35switch = 1;
	P36switch = 1;

	P37switch = 1;

	P38switch = 1;
	P39switch = 1;

	P310switch = 1;

	P311switch = 1;
	P312switch = 0;
	P313switch = 1;
	P314switch = 1;
	P315switch = 1;
	P316switch = 0;
	P317switch = 1;
	P318switch = 1;
	P319switch = 0;
	P320switch = 1;
	P321switch = 1;
	P322switch = 1;
	P323switch = 1;
	P324switch = 1;
	P325switch = 1;
	P326switch = 0;
	P327switch = 0;
	P328switch = true;
	P329switch = true;
	P330switch = 0;
	P331switch = 1;
	P332switch = 1;
	P333switch = 1;
	P334switch = 1;
	P335switch = 0;
	P336switch = 1;

	P337switch = 1;
	P338switch = 1;
	P339switch = 1;
	P340switch = 1;
	P341switch = 1;
	P342switch = 1;
	P343switch = 1;
	P344switch = 1;
	P345switch = 1;
	P346switch = 1;
	P347switch = 1;

	FCSMswitch = false;

	MRswitch = false;
	MRCswitch = false;

	TJ1switch = false;
	TJ1Cswitch = false;
	TJ2switch = false;
	TJ2Cswitch = false;

	LVSswitch = false;
	LVSCswitch = false;

	IUswitch = false;
	IUCswitch = false;

	DPswitch = false;
	DPCswitch = false;

	RH11switch = true;
	RH12switch = true;
	RH13switch = true;
	RH14switch = true;

	RH21switch = true;
	RH22switch = true;
	RH23switch = true;
	RH24switch = true;

	PP1switch = false;
	PP2switch = false;
	PP3switch = false;
	PP4switch = false;

	CR1switch = false;
	CR2switch = false;
	SP1switch = false;
	SP2switch = false;
	SP3switch = false;
	SP4switch = false;

	FCRswitch1 = true;
	FCRswitch2 = true;
	FCRswitch3 = true;

	FCBswitch1 = true;
	FCBswitch2 = true;
	FCBswitch3 = true;

	FCswitch1 = true;
	FCswitch2 = true;
	FCswitch3 = true;
	FCswitch4 = true;
	FCswitch5 = true;
	FCswitch6 = true;

	SRHswitch1 = true;
	SRHswitch2 = true;

	//CMRHDswitch = false;
	//CMRHGswitch = false;

	CMCswitch = true;

	EMSswitch = 1;

	EMSKswitch = false;

	RCSIndicatorsSwitch.AddPosition(1, 20);
	RCSIndicatorsSwitch.AddPosition(2, 45);
	RCSIndicatorsSwitch.AddPosition(3, 90);	
	RCSIndicatorsSwitch.AddPosition(4, 270);	
	RCSIndicatorsSwitch.AddPosition(5, 315);	
	RCSIndicatorsSwitch.AddPosition(6, 340);
	RCSIndicatorsSwitch = 1;

}

//
// Scenario state functions.
//

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
		unsigned Cswitch3:1;
		unsigned Cswitch4:1;
		unsigned Cswitch5:1;
		unsigned Cswitch6:1;
		unsigned Cswitch7:1;
		unsigned Cswitch8:1;
		unsigned Cswitch9:1;
		unsigned DVCswitch:1;
		unsigned DVBCswitch:1;
		unsigned ELSCswitch:1;
		unsigned CMDCswitch:1;
		unsigned CMPCswitch:1;
		unsigned IMUCswitch:1;
		unsigned MRswitch:1;
		unsigned MRCswitch:1;
		unsigned TJ1switch:1;
		unsigned TJ1Cswitch:1;
		unsigned TJ2switch:1;
		unsigned TJ2Cswitch:1;
		unsigned IUswitch:1;
		unsigned IUCswitch:1;
		unsigned LVSswitch:1;
		unsigned LVSCswitch:1;
		unsigned DPswitch:1;
		unsigned DPCswitch:1;
		unsigned CMRHGswitch:1;
	} u;
	unsigned long word;
} CSwitchState;

int Saturn::GetCSwitchState()

{
	CSwitchState state;

	state.word = 0;
	state.u.Cswitch1 = CsmLmFinalSep1Switch.GetGuardState();
	state.u.Cswitch2 = CsmLmFinalSep2Switch.GetGuardState();
	state.u.Cswitch3 = CmSmSep1Switch.GetGuardState();
	state.u.Cswitch4 = CmSmSep2Switch.GetGuardState();
	state.u.Cswitch5 = CsmLvSepSwitch.GetGuardState();
	state.u.Cswitch6 = Cswitch6;
	state.u.Cswitch7 = Cswitch7;
	state.u.Cswitch8 = Cswitch8;
	state.u.Cswitch9 = Cswitch9;
	state.u.DVCswitch = DVCswitch;
	state.u.DVBCswitch = DVBCswitch;
	state.u.ELSCswitch = ELSCswitch;
	state.u.CMDCswitch = CMDCswitch;
	state.u.CMPCswitch = CMPCswitch;
	state.u.IMUCswitch = IMUswitch.GetGuardState(); 
	state.u.MRswitch = MRswitch;
	state.u.MRCswitch = MRCswitch;
	state.u.TJ1switch = TJ1switch;
	state.u.TJ1Cswitch = TJ1Cswitch;
	state.u.TJ2switch = TJ2switch;
	state.u.TJ2Cswitch = TJ2Cswitch;
	state.u.IUswitch = IUswitch;
	state.u.IUCswitch = IUCswitch;
	state.u.LVSswitch = LVSswitch;
	state.u.LVSCswitch = LVSCswitch;
	state.u.DPswitch = DPswitch;
	state.u.DPCswitch = DPCswitch;
	state.u.CMRHGswitch = CmRcsHeDumpSwitch.GetGuardState();

	return state.word;
}

void Saturn::SetCSwitchState(int s)

{
	CSwitchState state;

	state.word = s;
	CsmLmFinalSep1Switch.SetGuardState(state.u.Cswitch1);
	CsmLmFinalSep2Switch.SetGuardState(state.u.Cswitch2);
	CmSmSep1Switch.SetGuardState(state.u.Cswitch3);
	CmSmSep2Switch.SetGuardState(state.u.Cswitch4);
	CsmLvSepSwitch.SetGuardState(state.u.Cswitch5);
	Cswitch6 = state.u.Cswitch6;
	Cswitch7 = state.u.Cswitch7;
	Cswitch8 = state.u.Cswitch8;
	Cswitch9 = state.u.Cswitch9;
	DVCswitch = state.u.DVCswitch;
	DVBCswitch = state.u.DVBCswitch;
	ELSCswitch = state.u.ELSCswitch;
	CMDCswitch = state.u.CMDCswitch;
	CMPCswitch = state.u.CMPCswitch;
	IMUswitch.SetGuardState(state.u.IMUCswitch);
	MRswitch = state.u.MRswitch;
	MRCswitch = state.u.MRCswitch;
	TJ1switch = state.u.TJ1switch;
	TJ1Cswitch = state.u.TJ1Cswitch;
	TJ2switch = state.u.TJ2switch;
	TJ2Cswitch = state.u.TJ2Cswitch;
	IUswitch = state.u.IUswitch;
	IUCswitch = state.u.IUCswitch;
	LVSswitch = state.u.LVSswitch;
	LVSCswitch = state.u.LVSCswitch;
	DPswitch = state.u.DPswitch;
	DPCswitch = state.u.DPCswitch;
	CmRcsHeDumpSwitch.SetGuardState(state.u.CMRHGswitch);
}

typedef union {
	struct {
		unsigned Sswitch1:1;
		unsigned Sswitch2:1;
		unsigned Sswitch3:1;
		unsigned Sswitch4:1;
		unsigned Sswitch5:1;
		unsigned Sswitch6:1;
		unsigned Sswitch7:1;
		unsigned Sswitch8:1;
		unsigned Sswitch9:1;
		unsigned DVAswitch:1;
		unsigned DVBswitch:1;
		unsigned ELSswitch:1;
		unsigned CMDswitch:1;
		unsigned CMPswitch:1;
		unsigned IMUswitch:1;
		unsigned CMRHDswitch:1;
	} u;
	unsigned long word;
} SSwitchState;

int Saturn::GetSSwitchState()

{
	SSwitchState state;

	state.word = 0;
	state.u.Sswitch1 = CsmLmFinalSep1Switch;
	state.u.Sswitch2 = CsmLmFinalSep2Switch;
	state.u.Sswitch3 = CmSmSep1Switch;
	state.u.Sswitch4 = CmSmSep2Switch;
	state.u.Sswitch5 = CsmLvSepSwitch;
	state.u.Sswitch6 = Sswitch6;
	state.u.Sswitch7 = Sswitch7;
	state.u.Sswitch8 = Sswitch8;
	state.u.Sswitch9 = Sswitch9;
	state.u.DVAswitch = DVAswitch;
	state.u.DVBswitch = DVBswitch;
	state.u.ELSswitch = ELSswitch;
	state.u.CMDswitch = CMDswitch;
	state.u.CMPswitch = CMPswitch;
	state.u.IMUswitch = IMUswitch;
	state.u.CMRHDswitch = CmRcsHeDumpSwitch;

	return state.word;
}

void Saturn::SetSSwitchState(int s)

{
	SSwitchState state;

	state.word = s;
	CsmLmFinalSep1Switch = state.u.Sswitch1;
	CsmLmFinalSep2Switch = state.u.Sswitch2;
	CmSmSep1Switch = state.u.Sswitch3;
	CmSmSep2Switch = state.u.Sswitch4;
	CsmLvSepSwitch = state.u.Sswitch5;
	Sswitch6 = state.u.Sswitch6;
	Sswitch7 = state.u.Sswitch7;
	Sswitch8 = state.u.Sswitch8;
	Sswitch9 = state.u.Sswitch9;
	DVAswitch = state.u.DVAswitch;
	DVBswitch = state.u.DVBswitch;
	ELSswitch = state.u.ELSswitch;
	CMDswitch = state.u.CMDswitch;
	CMPswitch = state.u.CMPswitch;
	IMUswitch = state.u.IMUswitch;
	CmRcsHeDumpSwitch = state.u.CMRHDswitch;
}

typedef union {
	struct {
		unsigned LPswitch1:1;
		unsigned LPswitch2:1;
		unsigned LPswitch3:1;
		unsigned LPswitch4:1;
		unsigned LPswitch5:1;
		unsigned LPswitch6:1;
		unsigned LPswitch7:1;
		unsigned SPSswitch:1;
		unsigned EDSswitch:1;
		unsigned P11switch:1;
		unsigned P12switch:1;
		unsigned P13switch:1;
		unsigned P14switch:1;
		unsigned P15switch:1;
		unsigned P16switch:1;
		unsigned SCswitch:1;
		unsigned TLIswitch:1;
		unsigned P111switch:1;
		unsigned P112switch:1;
		unsigned P113switch:1;
		unsigned FCSMswitch:1;
		unsigned EMSKswitch:1;
	} u;
	unsigned long word;
} LPSwitchState;

int Saturn::GetLPSwitchState()

{
	LPSwitchState state;

	state.word = 0;
	state.u.LPswitch1 = LPswitch1;
	state.u.LPswitch2 = LPswitch2;
	state.u.LPswitch3 = LPswitch3;
	state.u.LPswitch4 = LPswitch4;
	state.u.LPswitch5 = LPswitch5;
	state.u.LPswitch6 = LPswitch6;
	state.u.LPswitch7 = LPswitch7;
	state.u.SPSswitch = SPSswitch;
	state.u.EDSswitch = EDSSwitch;
	state.u.P11switch = P11switch;
	state.u.P12switch = P12switch;
	state.u.P13switch = P13switch;
	state.u.P14switch = P14switch;
	state.u.P15switch = P15switch;
	state.u.P16switch = P16switch;
	state.u.SCswitch = SCswitch;
	state.u.TLIswitch = TLIswitch;
	state.u.P111switch = P111switch;
	state.u.P112switch = P112switch;
	state.u.P113switch = P113switch;
	state.u.FCSMswitch = FCSMswitch;
	state.u.EMSKswitch = EMSKswitch;

	return state.word;
}

void Saturn::SetLPSwitchState(int s)

{
	LPSwitchState state;

	state.word = s;
	LPswitch1 = state.u.LPswitch1;
	LPswitch2 = state.u.LPswitch2;
	LPswitch3 = state.u.LPswitch3;
	LPswitch4 = state.u.LPswitch4;
	LPswitch5 = state.u.LPswitch5;
	LPswitch6 = state.u.LPswitch6;
	LPswitch7 = state.u.LPswitch7;
	SPSswitch = state.u.SPSswitch;
	EDSSwitch = state.u.EDSswitch;
	P11switch = state.u.P11switch;
	P12switch = state.u.P12switch;
	P13switch = state.u.P13switch;
	P14switch = state.u.P14switch;
	P15switch = state.u.P15switch;
	P16switch = state.u.P16switch;
	SCswitch = state.u.SCswitch;
	TLIswitch = state.u.TLIswitch;
	P111switch = state.u.P111switch;
	P112switch = state.u.P112switch;
	P113switch = state.u.P113switch;
	FCSMswitch = state.u.FCSMswitch;
	EMSKswitch = state.u.EMSKswitch;
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
		unsigned RPswitch16:1;
		unsigned RPCswitch:1;
		unsigned CMRswitch:1;
		unsigned CMRCswitch:1;
		unsigned CMCswitch:1;
	} u;
	unsigned long word;
} RPSwitchState;

int Saturn::GetRPSwitchState()

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
	state.u.RPswitch16 = SivbLmSepSwitch;
	state.u.RPCswitch = SivbLmSepSwitch.GetGuardState();
	state.u.CMRswitch = CMRswitch;
	state.u.CMRCswitch = CMRCswitch;
	state.u.CMCswitch = CMCswitch;

	return state.word;
}

void Saturn::SetRPSwitchState(int s)

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
	SivbLmSepSwitch = state.u.RPswitch16;
	SivbLmSepSwitch.SetGuardState(state.u.RPCswitch);
	CMRswitch = state.u.CMRswitch;
	CMRCswitch = state.u.CMRCswitch;
	CMCswitch = state.u.CMCswitch;
}

typedef union {
	struct {
		unsigned RH11switch:1;
		unsigned RH12switch:1;
		unsigned RH13switch:1;
		unsigned RH14switch:1;
		unsigned RH21switch:1;
		unsigned RH22switch:1;
		unsigned RH23switch:1;
		unsigned RH24switch:1;
		unsigned PP1switch:1;
		unsigned PP2switch:1;
		unsigned PP3switch:1;
		unsigned PP4switch:1;
		unsigned P21switch:1;
		unsigned P22switch:1;
		unsigned P23switch:1;
		unsigned P29switch:1;
		unsigned P210switch:1;
		unsigned P211switch:1;
	} u;
	unsigned long word;
} CPSwitchState;

int Saturn::GetCPSwitchState()

{
	CPSwitchState state;

	state.word = 0;
	state.u.RH11switch = RH11switch;
	state.u.RH12switch = RH12switch;
	state.u.RH13switch = RH13switch;
	state.u.RH14switch = RH14switch;
	state.u.RH21switch = RH21switch;
	state.u.RH22switch = RH22switch;
	state.u.RH23switch = RH23switch;
	state.u.RH24switch = RH24switch;
	state.u.PP1switch = PP1switch;
	state.u.PP2switch = PP2switch;
	state.u.PP3switch = PP3switch;
	state.u.PP4switch = PP4switch;
	state.u.P21switch = P21switch;
	state.u.P22switch = P22switch;
	state.u.P23switch = P23switch;
	state.u.P29switch = P29switch;
	state.u.P210switch = P210switch;
	state.u.P211switch = P211switch;

	return state.word;
}

void Saturn::SetCPSwitchState(int s)

{
	CPSwitchState state;

	state.word = s;
	RH11switch = state.u.RH11switch;
	RH12switch = state.u.RH12switch;
	RH13switch = state.u.RH13switch;
	RH14switch = state.u.RH14switch;
	RH21switch = state.u.RH21switch;
	RH22switch = state.u.RH22switch;
	RH23switch = state.u.RH23switch;
	RH24switch = state.u.RH24switch;
	PP1switch = state.u.PP1switch;
	PP2switch = state.u.PP2switch;
	PP3switch = state.u.PP3switch;
	PP4switch = state.u.PP4switch;
	P21switch = state.u.P21switch;
	P22switch = state.u.P22switch;
	P23switch = state.u.P23switch;
	P29switch = state.u.P29switch;
	P210switch = state.u.P210switch;
	P211switch = state.u.P211switch;

}

typedef union {
	struct {
		unsigned CR1switch:1;
		unsigned CR2switch:1;
		unsigned SP1switch:1;
		unsigned SP2switch:1;
		unsigned SP3switch:1;
		unsigned SP4switch:1;
		unsigned P221switch:1;
		unsigned CFswitch1:1;
		unsigned CFswitch2:1;
	} u;
	unsigned long word;
} CP2SwitchState;

int Saturn::GetCP2SwitchState()

{
	CP2SwitchState state;

	state.word = 0;
	state.u.CR1switch = CR1switch;
	state.u.CR2switch = CR2switch;
	state.u.SP1switch = SP1switch;
	state.u.SP2switch = SP2switch;
	state.u.SP3switch = SP3switch;
	state.u.SP4switch = SP4switch;
	state.u.P221switch = P221switch;
	state.u.CFswitch1 = CabinFan1Switch;
	state.u.CFswitch2 = CabinFan2Switch;

	return state.word;
}

void Saturn::SetCP2SwitchState(int s)

{
	CP2SwitchState state;

	state.word = s;
	CR1switch = state.u.CR1switch;
	CR2switch = state.u.CR2switch;
	SP1switch = state.u.SP1switch;
	SP2switch = state.u.SP2switch;
	SP3switch = state.u.SP3switch;
	SP4switch = state.u.SP4switch;
	P221switch = state.u.P221switch;
	CabinFan1Switch = state.u.CFswitch1;
	CabinFan2Switch = state.u.CFswitch2;

}

typedef union {
	struct {
		unsigned P31switch:1;
		unsigned P32switch:1;
		unsigned P33switch:1;
		unsigned FCRswitch1:1;
		unsigned FCRswitch2:1;
		unsigned FCRswitch3:1;
		unsigned FCBswitch1:1;
		unsigned FCBswitch2:1;
		unsigned FCBswitch3:1;
	} u;
	unsigned long word;
} CP3SwitchState;

int Saturn::GetCP3SwitchState()

{
	CP3SwitchState state;

	state.word = 0;
	state.u.P31switch = P31switch;
	state.u.P32switch = P32switch;
	state.u.P33switch = P33switch;
	state.u.FCRswitch1 = FCRswitch1;
	state.u.FCRswitch2 = FCRswitch2;
	state.u.FCRswitch3 = FCRswitch3;
	state.u.FCBswitch1 = FCBswitch1;
	state.u.FCBswitch2 = FCBswitch2;
	state.u.FCBswitch3 = FCBswitch3;

	return state.word;
}

void Saturn::SetCP3SwitchState(int s)

{
	CP3SwitchState state;

	state.word = s;
	P31switch = state.u.P31switch;
	P32switch = state.u.P32switch;
	P33switch = state.u.P33switch;
	FCRswitch1 = state.u.FCRswitch1;
	FCRswitch2 = state.u.FCRswitch2;
	FCRswitch3 = state.u.FCRswitch3;
	FCBswitch1 = state.u.FCBswitch1;
	FCBswitch2 = state.u.FCBswitch2;
	FCBswitch3 = state.u.FCBswitch3;

}

typedef union {
	struct {
		unsigned SRHswitch1:1;
		unsigned SRHswitch2:1;
		unsigned FCswitch1:1;
		unsigned FCswitch2:1;
		unsigned FCswitch3:1;
		unsigned FCswitch4:1;
		unsigned FCswitch5:1;
		unsigned FCswitch6:1;
		unsigned P114switch:1;
		unsigned P115switch:1;
		unsigned P116switch:1;
		unsigned P117switch:1;
	} u;
	unsigned long word;
} CP4SwitchState;

int Saturn::GetCP4SwitchState()

{
	CP4SwitchState state;

	state.word = 0;
	state.u.SRHswitch1 = SRHswitch1;
	state.u.SRHswitch2 = SRHswitch2;
	state.u.FCswitch1 = FCswitch1;
	state.u.FCswitch2 = FCswitch2;
	state.u.FCswitch3 = FCswitch3;
	state.u.FCswitch4 = FCswitch4;
	state.u.FCswitch5 = FCswitch5;
	state.u.FCswitch6 = FCswitch6;
	state.u.P114switch = P114switch;
	state.u.P115switch = P115switch;
	state.u.P116switch = P116switch;
	state.u.P117switch = P117switch;

	return state.word;
}

void Saturn::SetCP4SwitchState(int s)

{
	CP4SwitchState state;

	state.word = s;
	SRHswitch1 = state.u.SRHswitch1;
	SRHswitch2 = state.u.SRHswitch2;
	FCswitch1 = state.u.FCswitch1;
	FCswitch2 = state.u.FCswitch2;
	FCswitch3 = state.u.FCswitch3;
	FCswitch4 = state.u.FCswitch4;
	FCswitch5 = state.u.FCswitch5;
	FCswitch6 = state.u.FCswitch6;
	P114switch = state.u.P114switch;
	P115switch = state.u.P115switch;
	P116switch = state.u.P116switch;
	P117switch = state.u.P117switch;

}

typedef union {
	struct {
		unsigned P328switch:1;
		unsigned P329switch:1;
	} u;
	unsigned long word;
} CP5SwitchState;

int Saturn::GetCP5SwitchState()

{
	CP5SwitchState state;

	state.word = 0;
	state.u.P328switch = P328switch;
	state.u.P329switch = P329switch;

	return state.word;
}

void Saturn::SetCP5SwitchState(int s)

{
	CP5SwitchState state;

	state.word = s;
	P328switch = state.u.P328switch;
	P329switch = state.u.P329switch;

}
