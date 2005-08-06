/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant, Matthias Müller

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
#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"

#include "landervessel.h"
#include "sat5_lmpkd.h"

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

void sat5_lmpkd::InitPanel()

{
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
	ENGARMswitch=false;
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

	MSOV1switch=false;
	MSOV2switch=false;

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

	DESEswitch=false;

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

	Abortswitch=false;

	for (int i = 0; i < nsurf; i++)
		srf[i] = 0;
}

void sat5_lmpkd::RedrawPanel_Horizon (SURFHANDLE surf)
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

void sat5_lmpkd::RedrawPanel_Thrust (SURFHANDLE surf)

{
	double DispValue;

	DispValue = GetEngineLevel(ENGINE_HOVER);

	oapiBlt(surf,srf[2],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
	oapiBlt(surf,srf[2],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
}

void sat5_lmpkd::ReleaseSurfaces ()

{
	for (int i = 0; i < nsurf; i++)
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
}

void sat5_lmpkd::InitPanel (int panel)

{

	switch (panel) {
	case 0: // LEM Main Panel
		srf[0] = oapiCreateSurface (LOADBMP (IDB_ECSG));
		//srf[1] = oapiCreateSurface (LOADBMP (IDB_INDICATORS1));
		srf[2] = oapiCreateSurface (LOADBMP (IDB_NEEDLE1));
		srf[3] = oapiCreateSurface (LOADBMP (IDB_HORIZON));
		srf[4] = oapiCreateSurface (LOADBMP (IDB_DIGITAL));
		srf[5] = oapiCreateSurface (LOADBMP (IDB_FDAI));
		srf[6] = oapiCreateSurface (LOADBMP (IDB_LEMSWITCH1));
		srf[7] = oapiCreateSurface (LOADBMP (IDB_SWLEVER));
		srf[8] = oapiCreateSurface (LOADBMP (IDB_SECSWITCH));
		srf[9] = oapiCreateSurface (LOADBMP (IDB_ABORT));
		srf[10] = oapiCreateSurface (LOADBMP (IDB_ANNUN));
		srf[11] = oapiCreateSurface (LOADBMP (IDB_LAUNCH));
		srf[12] = oapiCreateSurface (LOADBMP (IDB_LV_ENG));
		srf[13] = oapiCreateSurface (LOADBMP (IDB_LIGHTS2));
		srf[14] = oapiCreateSurface (LOADBMP (IDB_ANLG_ALT));
		srf[15] = oapiCreateSurface (LOADBMP (IDB_ANLG_GMETER));
		srf[16] = oapiCreateSurface (LOADBMP (IDB_THRUST));
		srf[17] = oapiCreateSurface (LOADBMP (IDB_HEADING));
		srf[18] = oapiCreateSurface (LOADBMP (IDB_CONTACT));
		srf[19] = oapiCreateSurface (LOADBMP (IDB_LEMSWITCH2));
		srf[20] = oapiCreateSurface (LOADBMP (IDB_LEMSWITCH3));
		srf[SRF_DSKY] = oapiCreateSurface (LOADBMP (IDB_DSKY_LIGHTS));
		
		oapiSetSurfaceColourKey (srf[0], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[2], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[3], 0);
		oapiSetSurfaceColourKey (srf[5], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[14], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[15], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[16], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[18], g_Param.col[4]);
		
		break;
	
	case 1: // LEM Right Window 
	case 2: // LEM Left Window 
		
		break;

	}

}

bool sat5_lmpkd::LoadPanel (int id)

{
	ReleaseSurfaces();

	HBITMAP hBmp;


      static bool recursion;   //yogen 
 
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

	switch(id) {

    case 0:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_MAIN_PANEL));
		oapiSetPanelNeighbours(2, 1, -1, -1);
		break;

	case 1:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_RIGHT_WINDOW));
		oapiSetPanelNeighbours(0, -1, -1, -1);
		break;

    case 2:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_LEFT_WINDOW));
		oapiSetPanelNeighbours(-1, 0, -1, -1);
		break;

	}

	//MFDSPEC mfds_left_l  = {{ 850, 944, 1129, 1223}, 6, 6, 41, 27};
	//MFDSPEC mfds_right_l = {{1525, 944, 1804, 1223}, 6, 6, 41, 27};
	//MFDSPEC mfds_left_r  = {{ 15, 944, 294, 1223}, 6, 6, 41, 27};
	//MFDSPEC mfds_right_r = {{690, 944, 969, 1223}, 6, 6, 41, 27};

	switch (id) {
	case 0: // LEM Main panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		//oapiRegisterMFD (MFD_LEFT,  mfds_left_l);
		//oapiRegisterMFD (MFD_RIGHT, mfds_right_l);

		//oapiRegisterPanelArea (AID_MISSION_CLOCK,						_R( 908,  163, 1007,  178), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
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
		//oapiRegisterPanelArea (AID_CONTACT,						_R(1069,  249, 1099,  279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
		//oapiRegisterPanelArea (AID_CONTACT,						_R(1715,  749, 1745,  779), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,PANEL_MAP_BACKGROUND);
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
		oapiRegisterPanelArea (AID_ABORT,						_R(555,  880, 695,   930), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		
		// DSKY
		
		oapiRegisterPanelArea (AID_DSKY_DISPLAY,					_R( 762, 1560,  867, 1736), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_LIGHTS,						_R( 618, 1565,  720, 1734), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_KEY,						_R( 598, 1755,  886, 1867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,PANEL_MAP_BACKGROUND);
		
		break;	
		
		case 1: // LEM Right Window
			oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	
		
		break;

		case 2: // LEM Left Window
			oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
			// Animated LEM COAS will be added here soon...

		break;
   }
	 
   InitPanel (id);
   //
   // Changed camera direction for the left LEM window
   //
   if(id >= 2) {
	   // if this is the first time we've been here, save the current FOV
		if(InFOV) {
		   SaveFOV=oapiCameraAperture();
		   InFOV=false;
		}
		//set FOV to 70 degrees
		oapiCameraSetAperture(RAD*35.0);
		SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE*RAD), cos(VIEWANGLE*RAD)));
   } else {
		 if(InFOV == false) {
		   oapiCameraSetAperture(SaveFOV);
		   InFOV=true;

		}
	}
   //
   // Changed camera direction for the right LEM window
   //
	if(id >= 1) {
	   // if this is the first time we've been here, save the current FOV
		if(InFOV) {
		   SaveFOV=oapiCameraAperture();
		   InFOV=false;
		}
		//set FOV to 70 degrees
		oapiCameraSetAperture(RAD*35.0);
		SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE*RAD), cos(VIEWANGLE*RAD)));
   } else {
		 if(InFOV == false) {
		   oapiCameraSetAperture(SaveFOV);
		   InFOV=true;
		}
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
   }
	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	InVC = false;
	InPanel = true; //yogen
    PanelId = id;  //yogen


	return hBmp != NULL;

}

void sat5_lmpkd::SwitchClick()

{
	Sclick.play();
}

void sat5_lmpkd::ButtonClick()

{
	Bclick.play(NOLOOP,255);
}

void sat5_lmpkd::GuardClick()

{
	Gclick.play(NOLOOP,255);
}

void sat5_lmpkd::CabinFanSound()

{
	CabinFans.play(LOOP,255);
}

void sat5_lmpkd::VoxSound()

{
	Vox.play(LOOP,255);
}

void sat5_lmpkd::AbortFire()

{
	Afire.play(NOLOOP,255);
}

bool sat5_lmpkd::PanelMouseEvent (int id, int event, int mx, int my)

{
	static int ctrl = 0;

	switch (id) {
	// panel 0 events:

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

	case AID_DSKY_KEY:
		dsky.ProcessKeypress(mx, my);
		return true;

	case AID_ABORT:
		if (mx > 0 && mx < 80 && my > 0 && my < 50){
			// This is the "ABORT" button
				ButtonClick();
				AbortFire();
				SetEngineLevel(ENGINE_HOVER, 1);
//				SetThrusterResource(th_hover[0], ph_Asc);
//				SetThrusterResource(th_hover[1], ph_Asc);
//				stage = 2;
				startimer = false;
				agc.SetInputChannelBit(030, 1, true);
		}
		else if (mx > 85 && mx < 130 && my > 0 && my < 50){
			// This is the "ABORT STAGE" button
				ButtonClick();
				AbortFire();
				Abortswitch = true;
				SeparateStage(stage);
				SetThrusterResource(th_hover[0], ph_Asc);
				SetThrusterResource(th_hover[1], ph_Asc);
				stage = 2;
				startimer = false;
				Abortswitch = true;
				if(agc.GetProgRunning() > 14 ) {
					SetEngineLevel(ENGINE_HOVER, 1);
					agc.SetInputChannelBit(030, 4, true);
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
bool sat5_lmpkd::PanelRedrawEvent (int id, int event, SURFHANDLE surf)

{
	switch (id) {
	// panel 0 events:
	case AID_ABORT:
		if (Abortswitch){
			oapiBlt(surf,srf[9],0,0,0,73,94,44);
		}
		return true;

	case AID_CONTACT:
		if (GroundContact()&& stage ==1){
			oapiBlt(surf,srf[18],0,0,0,0,30,30, SURF_PREDEF_CK);//
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

	case AID_MISSION_CLOCK:
		int TmpCLKHR;
		int TmpCLKMNT;
		int TmpCLKsec;
		int Curdigit;
		int Curdigit2;

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
		oapiBlt(surf,srf[4],0,0, 10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=TmpCLKHR/10;
		Curdigit2=TmpCLKHR/100;
		oapiBlt(surf,srf[4],0+12,0, 10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=TmpCLKHR;
		Curdigit2=TmpCLKHR/10;
		oapiBlt(surf,srf[4],0+24,0, 10*(Curdigit-(Curdigit2*10)),0,10,15);
		oapiBlt(surf,srf[4],0+34,0, 140,0,10,15);
		// Minute display on five digit
		Curdigit=TmpCLKMNT/10;
		Curdigit2=TmpCLKMNT/100;
		oapiBlt(surf,srf[4],0+44,0, 10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=TmpCLKMNT;
		Curdigit2=TmpCLKMNT/10;
		oapiBlt(surf,srf[4],0+56,0, 10*(Curdigit-(Curdigit2*10)),0,10,15);
		oapiBlt(surf,srf[4],0+66,0, 140,0,10,15);
		// second display on five digit
		Curdigit=TmpCLKsec/10;
		Curdigit2=TmpCLKsec/100;
		oapiBlt(surf,srf[4],0+76,0, 10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=TmpCLKsec;
		Curdigit2=TmpCLKsec/10;
		oapiBlt(surf,srf[4],0+88,0, 10*(Curdigit-(Curdigit2*10)),0,10,15);

		return true;

	case AID_FUEL_DIGIT:
		int TmpFUEL;
		double tmpFUELdec;

		TmpFUEL = (int) actualFUEL;
		tmpFUELdec = actualFUEL-TmpFUEL;

		Curdigit= (int) actualFUEL/100;
		oapiBlt(surf,srf[4],0,0,10*Curdigit,0,10,15);
		Curdigit= (int) actualFUEL/10;
		Curdigit2=(int) actualFUEL/100;
		oapiBlt(surf,srf[4],13,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		Curdigit=(int) actualFUEL;
		Curdigit2=(int) actualFUEL/10;
		oapiBlt(surf,srf[4],26,0,10*(Curdigit-(Curdigit2*10)),0,10,15);

		return true;

	case AID_FUEL_DIGIT2:
		int tmpfuel2;

		TmpFUEL = (int) actualFUEL;
		tmpFUELdec = actualFUEL-TmpFUEL;
		tmpfuel2 = (int) tmpFUELdec *100;

		Curdigit=(int) actualFUEL/100;
		oapiBlt(surf,srf[4],0,0,10*Curdigit,0,10,15);
		Curdigit=(int) actualFUEL/10;
		Curdigit2=(int) actualFUEL/100;
		oapiBlt(surf,srf[4],13,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
   	    Curdigit=(int) actualFUEL;
		Curdigit2=(int) actualFUEL/10;
		oapiBlt(surf,srf[4],26,0,10*(Curdigit-(Curdigit2*10)),0,10,15);

		oapiBlt(surf,srf[4],36,0,140,0,5,5);//dot display

		Curdigit=tmpfuel2/10 ;
		Curdigit2=tmpfuel2 /100;
		oapiBlt(surf,srf[4],42,0,10*(Curdigit-(Curdigit2*10)),0,10,15);
		return true;

	case AID_DESCENT_HE:
		if(DESHE1switch){
			oapiBlt(surf,srf[6],1,30,0,0,23,20);
			oapiBlt(surf,srf[13],3,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],1,30,23,0,23,20);
			oapiBlt(surf,srf[13],3,0,38,0,19,20);
		}
		if(DESHE2switch){
			oapiBlt(surf,srf[6],44,30,0,0,23,20);
			oapiBlt(surf,srf[13],46,0,0,0,19,20);
		}else{
			oapiBlt(surf,srf[6],44,30,23,0,23,20);
			oapiBlt(surf,srf[13],46,0,38,0,19,20);
		}
		return true;

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

	case AID_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_DSKY]);
		return true;


	case AID_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[4]);
		return true;

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

	case AID_DESCENT_ENGINE_SWITCH:
		if(DESEswitch){
			oapiBlt(surf,srf[19],0,0,0,0,23,30);
		}else{
			oapiBlt(surf,srf[19],0,0,23,0,23,30);
		}
		return true;

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

	case AID_XPOINTER:
		int ix, iy;
		double vx, vy;
		//draw the crosspointers
		agc.GetHorizVelocity(vx, vy);
		ix=(int)(-3.9*vx);
		if(ix < -39) ix=-39;
		if(ix > 39) ix=39;
		iy=(int)(3.9*vy);
		if(iy < -39) iy=-39;
		if(iy > 39 ) iy=39;
        oapiColourFill(surf, oapiGetColour(255, 255, 255), 0, 0, 79, 79);
		HDC hDC=oapiGetDC(surf);
		SelectObject(hDC, GetStockObject(BLACK_PEN));
		MoveToEx(hDC, 0, 40+ix, NULL);
		LineTo(hDC, 80, 40+ix);
		MoveToEx(hDC, 40+iy, 0, NULL);
		LineTo(hDC, 40+iy, 80);
		oapiReleaseDC(surf, hDC);
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

int sat5_lmpkd::GetCSwitchState()

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
	state.u.Abortswitch = Abortswitch;

	return state.word;
}

void sat5_lmpkd::SetCSwitchState(int s)

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
	Abortswitch = state.u.Abortswitch;
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
		unsigned DESEswitch:1;
		unsigned SLWRswitch:1;
		unsigned DBswitch:1;
		unsigned IMUCswitch:1;
		unsigned SPLswitch:1;
		unsigned X2switch:1;
	} u;
	unsigned long word;
} SSwitchState;

int sat5_lmpkd::GetSSwitchState()

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
	state.u.DESEswitch = DESEswitch;
	state.u.SLWRswitch = SLWRswitch;
	state.u.DBswitch = DBswitch;
	state.u.IMUCswitch = IMUCswitch;
	state.u.SPLswitch = SPLswitch;
	state.u.X2switch = X2switch;

	return state.word;
}

void sat5_lmpkd::SetSSwitchState(int s)

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
	DESEswitch = state.u.DESEswitch;
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
		unsigned ENGARMswitch:1;
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

int sat5_lmpkd::GetLPSwitchState()

{
	LPSwitchState state;

	state.word = 0;
	state.u.ATT2switch = ATT2switch;
	state.u.ATT3switch = ATT3switch;
	state.u.ATT1switch = ATT1switch;
	state.u.LPswitch4 = LPswitch4;
	state.u.LPswitch5 = LPswitch5;
	state.u.GMBLswitch = GMBLswitch;
	state.u.ENGARMswitch = ENGARMswitch;
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
	state.u.MSOV1switch = MSOV1switch;
	state.u.MSOV2switch = MSOV2switch;
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

void sat5_lmpkd::SetLPSwitchState(int s)

{
	LPSwitchState state;

	state.word = s;
	ATT2switch = state.u.ATT2switch;
	ATT3switch = state.u.ATT3switch;
	ATT1switch = state.u.ATT1switch;
	LPswitch4 = state.u.LPswitch4;
	LPswitch5 = state.u.LPswitch5;
	GMBLswitch = state.u.GMBLswitch;
	ENGARMswitch = state.u.ENGARMswitch;
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
	MSOV1switch = state.u.MSOV1switch;
	MSOV2switch = state.u.MSOV2switch;
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

int sat5_lmpkd::GetRPSwitchState()

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

void sat5_lmpkd::SetRPSwitchState(int s)

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
