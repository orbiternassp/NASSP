/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Project Apollo MFD

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
  *	Revision 1.1  2006/06/23 11:56:48  tschachim
  *	New Project Apollo MFD for TLI burn control.
  *	
  **************************************************************************/

#define STRICT
#define ORBITER_MODULE
#include "math.h"
#include "windows.h"
#include "orbitersdk.h"

#include "nasspsound.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "saturn.h"

#include "MFDResource.h"
#include "ProjectApolloMFD.h"


// ==============================================================
// Global variables

HINSTANCE g_hDLL;
int g_MFDmode; // identifier for new MFD mode


#define PROG_NONE	0
#define PROG_TLI	1
#define PROG_STATUS	2

#define PROGSTATE_NONE			0
#define PROGSTATE_TLI_WAITING	1
#define PROGSTATE_TLI_RUNNING	2
#define PROGSTATE_TLI_ERROR		3

// Time to ejection when the IU is programmed
#define IUSTARTTIME 900

static struct {  // global data storage
	int prog;	
	int progState;  
	Saturn *progVessel;
	double tliTime;
	double tliVelocity;
} g_Data;


DLLCLBK void opcDLLInit (HINSTANCE hDLL)
{
	static char *name = "Project Apollo";      // MFD mode name
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_A;					   // MFD mode selection key is obsolete
	spec.msgproc = ProjectApolloMFD::MsgProc;  // MFD mode callback function

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);
	g_hDLL = hDLL;

	g_Data.prog = PROG_NONE;
	g_Data.progState = 0;
	g_Data.progVessel = NULL;
	g_Data.tliTime = 0;
	g_Data.tliVelocity = 0;
}

DLLCLBK void opcDLLExit (HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded
	oapiUnregisterMFDMode (g_MFDmode);
}

DLLCLBK void opcTimestep (double simt, double simdt, double mjd)
{

	if (g_Data.prog == PROG_TLI) {
		switch (g_Data.progState) {
		case PROGSTATE_TLI_WAITING:
			g_Data.tliTime -= simdt;
			if (g_Data.tliTime < IUSTARTTIME) {
				if (g_Data.progVessel->GetIU()->StartTLIBurn(g_Data.tliTime, g_Data.tliVelocity)) {
					g_Data.progState = PROGSTATE_TLI_RUNNING;
				} else {
					g_Data.progState = PROGSTATE_TLI_ERROR;
				}
			}
			break;

		case PROGSTATE_TLI_RUNNING:
			if (g_Data.progVessel->GetIU()->IsTLIInProgress()) {
				double mt = g_Data.progVessel->GetMissionTime();
				double t = g_Data.progVessel->GetIU()->GetTLIBurnStartTime();
				if (t > mt)
					g_Data.tliTime = mt - t;
				else
					g_Data.tliTime = g_Data.progVessel->GetIU()->GetTLIBurnEndTime() - mt;
			} else {
				g_Data.progState = PROGSTATE_NONE;
				g_Data.progVessel = NULL;
			}
			break;
		}
	}
}

// ==============================================================
// MFD class implementation

// Constructor
ProjectApolloMFD::ProjectApolloMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD (w, h, vessel)

{
	saturn = NULL;
	width = w;
	height = h;
	hBmpLogo = LoadBitmap(g_hDLL, MAKEINTRESOURCE (IDB_LOGO));
	screen = PROG_NONE;

	if (!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn5") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn1b")) {
		saturn = (Saturn *)vessel;
	}
}

// Destructor
ProjectApolloMFD::~ProjectApolloMFD ()
{
	// Add MFD cleanup code here
}

// Return button labels
char *ProjectApolloMFD::ButtonLabel (int bt)
{
	// The labels for the buttons used by our MFD mode
	static char *labelNone[2] = {"TLI", "STA"};
	static char *labelTliStop[4] = {"BCK", "RUN", "T", "V"};
	static char *labelTliRun[4] = {"BCK", "STP", "T", "V"};
	static char *labelStatus[1] = {"BCK"};

	if (screen == PROG_TLI) {
		if (g_Data.progState == PROGSTATE_NONE)
			return (bt < 4 ? labelTliStop[bt] : 0);
		else
			return (bt < 4 ? labelTliRun[bt] : 0);
	}
	else if (screen == PROG_STATUS) {
		return (bt < 1 ? labelStatus[bt] : 0);
	}
	return (bt < 2 ? labelNone[bt] : 0);
}

// Return button menus
int ProjectApolloMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the buttons used by our MFD mode
	static const MFDBUTTONMENU mnuNone[2] = {
		{"IU TLI burn program", 0, 'T'},
		{"Status", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuTliStop[4] = {
		{"Back", 0, 'B'},
		{"Run", 0, 'R'},
		{"Time to ejection", 0, 'T'},
		{"Delta Velocity", 0, 'V'}
	};
	static const MFDBUTTONMENU mnuTliRun[4] = {
		{"Back", 0, 'B'},
		{"Stop", 0, 'S'},
		{"Time to ejection", 0, 'T'},
		{"Delta Velocity", 0, 'V'}
	};
	static const MFDBUTTONMENU mnuStatus[1] = {
		{"Back", 0, 'B'}
	};

	if (screen == PROG_TLI) {
		if (g_Data.progState == PROGSTATE_NONE) {
			if (menu) *menu = mnuTliStop;
			return 4;
		} else {
			if (menu) *menu = mnuTliRun;
			return 4;
		}
	} else if (screen == PROG_STATUS) {
		if (menu) *menu = mnuStatus;
		return 1; 
	} else {
		if (menu) *menu = mnuNone;
		return 2; 
	}
}

bool ProjectApolloMFD::ConsumeKeyBuffered (DWORD key) 
{
	char buffer[100];

	if (screen == PROG_NONE) {
		if (key == OAPI_KEY_T) {
			screen = PROG_TLI;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_S) {
			screen = PROG_STATUS;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
	} else if (screen == PROG_TLI) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;

		} else if (key == OAPI_KEY_R && g_Data.progState == PROGSTATE_NONE) {
			if (g_Data.tliTime >= IUSTARTTIME && g_Data.tliVelocity > 0 && saturn) {
				g_Data.prog = PROG_TLI;
				g_Data.progState = PROGSTATE_TLI_WAITING;
				g_Data.progVessel = saturn;
				InvalidateDisplay();
				InvalidateButtons();
				return true;
			}

		} else if (key == OAPI_KEY_S && (g_Data.progState == PROGSTATE_TLI_WAITING || g_Data.progState == PROGSTATE_TLI_ERROR)) {
			g_Data.prog = PROG_NONE;
			g_Data.progState = PROGSTATE_NONE;
			g_Data.progVessel = NULL;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
				
		} else if (key == OAPI_KEY_T) {
			if (g_Data.progState == PROGSTATE_NONE) {
				bool TLITimeInput (void *id, char *str, void *data);
				sprintf(buffer, "Time to ejection (seconds) [ >= %ds]:", IUSTARTTIME);
				oapiOpenInputBox (buffer, TLITimeInput, 0, 20, (void*)this);
			}
			return true;

		} else if (key == OAPI_KEY_V) {
			if (g_Data.progState == PROGSTATE_NONE) {
				bool TLIVelocityInput (void *id, char *str, void *data);
				oapiOpenInputBox ("Delta Velocity (meters/seconds):", TLIVelocityInput, 0, 20, (void*)this);
			}
			return true;
		}
	} else if (screen == PROG_STATUS) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
	}
	return false;
}

bool ProjectApolloMFD::ConsumeButton (int bt, int event)
{
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;

	static const DWORD btkeyNone[2] = { OAPI_KEY_T, OAPI_KEY_S};
	static const DWORD btkeyTliStop[4] = { OAPI_KEY_B, OAPI_KEY_R, OAPI_KEY_T, OAPI_KEY_V };
	static const DWORD btkeyTliRun[4] = { OAPI_KEY_B, OAPI_KEY_S, OAPI_KEY_T, OAPI_KEY_V };
	static const DWORD btkeyStatus[1] = { OAPI_KEY_B, };

	if (screen == PROG_TLI) {
		if (g_Data.progState == PROGSTATE_NONE) {
			if (bt < 4) return ConsumeKeyBuffered (btkeyTliStop[bt]);
		} else {
			if (bt < 4) return ConsumeKeyBuffered (btkeyTliRun[bt]);
		}
	} else if (screen == PROG_STATUS) {
		if (bt < 1) return ConsumeKeyBuffered (btkeyStatus[bt]);
	} else {		
		if (bt < 2) return ConsumeKeyBuffered (btkeyNone[bt]);
	}
	return false;
}

// Repaint the MFD
void ProjectApolloMFD::Update (HDC hDC)
{
	char buffer[100];

	HDC hDCTemp = CreateCompatibleDC(hDC);
	HBITMAP hBmpTemp = (HBITMAP) SelectObject(hDCTemp, hBmpLogo);
	StretchBlt(hDC, 1, 1, width - 2, height - 2, hDCTemp, 0, 0, 256, 256, SRCCOPY);
	DeleteObject(hBmpTemp);
	DeleteDC(hDCTemp);

	// Draws the MFD title
	Title (hDC, "Project Apollo");

	SelectDefaultFont(hDC, 0);
	SetBkMode (hDC, TRANSPARENT);
	SetTextAlign (hDC, TA_CENTER);

	if (!saturn) {
		SetTextColor (hDC, RGB(255, 0, 0));
		TextOut(hDC, width / 2, (int) (height * 0.5), "Unsupported vessel", 18);
		return;
	}

	// Draw mission time
	SetTextColor (hDC, RGB(0, 255, 0));
	TextOut(hDC, width / 2, (int) (height * 0.15), "Mission Time", 12);

	double mt = saturn->GetMissionTime();
	int secs = abs((int) mt);
	int hours = (secs / 3600);
	secs -= (hours * 3600);
	int minutes = (secs / 60);
	secs -= 60 * minutes;
	if (mt < 0)
		sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
	else
		sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);
	TextOut(hDC, width / 2, (int) (height * 0.2), buffer, strlen(buffer));

	SelectDefaultPen(hDC, 1);
	MoveToEx (hDC, (int) (width * 0.05), (int) (height * 0.3), 0);
	LineTo (hDC, (int) (width * 0.95), (int) (height * 0.3));

	// Draw TLI stuff
	if (screen == PROG_TLI) {
		TextOut(hDC, width / 2, (int) (height * 0.35), "IU TLI burn program", 19);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45), "Status:", 7);
		if (g_Data.progState != PROGSTATE_TLI_RUNNING)
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), "Time to ejection:", 17);
		else
			TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), "Time to burn:", 13);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), "Delta V:", 8);

		SetTextAlign (hDC, TA_CENTER);
		if (g_Data.progState == PROGSTATE_NONE) {
			TextOut(hDC, (int) (width * 0.5), (int) (height * 0.45), "STOP", 3);

		} else if (g_Data.progState == PROGSTATE_TLI_WAITING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.5), (int) (height * 0.45), "WAITING", 7);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_RUNNING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.5), (int) (height * 0.45), "RUNNING", 7);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_ERROR) {
			SetTextColor (hDC, RGB(255, 0, 0));
			TextOut(hDC, (int) (width * 0.5), (int) (height * 0.45), "ERROR", 5);
			SetTextColor (hDC, RGB(0, 255, 0));
		}

		SetTextAlign (hDC, TA_LEFT);
		sprintf(buffer, "%.0lfs", g_Data.tliTime);
		TextOut(hDC, (int) (width * 0.7), (int) (height * 0.55), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfm/s", g_Data.tliVelocity);
		TextOut(hDC, (int) (width * 0.7), (int) (height * 0.6), buffer, strlen(buffer));

	// Draw status
	} else if (screen == PROG_STATUS) {
		TextOut(hDC, width / 2, (int) (height * 0.35), "Status", 6);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45), "Velocity:", 9);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.5), "Vert. Velocity:", 15);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), "Altitude:", 9);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), "Apoapsis Alt.:", 14);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.7), "Periapsis Alt.:", 15);

		VECTOR3 vel, hvel;
		double vvel = 0, apDist, peDist;
		OBJHANDLE planet = saturn->GetGravityRef();
		saturn->GetRelativeVel(planet, vel); 
		if (saturn->GetHorizonAirspeedVector(hvel)) {
			vvel = hvel.y * 3.2808399;
		}
		saturn->GetApDist(apDist);
		saturn->GetPeDist(peDist);
		apDist -= 6.373338e6;
		peDist -= 6.373338e6;

		SetTextAlign (hDC, TA_RIGHT);
		sprintf(buffer, "%.0lfft/s", length(vel) * 3.2808399);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.45), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfft/s", vvel);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.5), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", saturn->GetAltitude() * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.55), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", apDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.65), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", peDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.7), buffer, strlen(buffer));
	}
}

void ProjectApolloMFD::WriteStatus (FILEHANDLE scn) const
{
	oapiWriteScenario_int (scn, "SCREEN", screen);
	oapiWriteScenario_int (scn, "PROG", g_Data.prog);
	oapiWriteScenario_int (scn, "PROGSTATE", g_Data.progState);
	if (g_Data.progVessel)
		oapiWriteScenario_string (scn, "PROGVESSEL", g_Data.progVessel->GetName());
	oapiWriteScenario_float (scn, "TLITIME", g_Data.tliTime);
	oapiWriteScenario_float (scn, "TLIVELOCITY", g_Data.tliVelocity);
}

void ProjectApolloMFD::ReadStatus (FILEHANDLE scn)
{
    char *line, name[100];
	float flt;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, "END_MFD", 7))
			return;

		if (!strnicmp (line, "SCREEN", 6))
			sscanf (line + 6, "%d", &screen);
		else if (!strnicmp (line, "PROGSTATE", 9))
			sscanf (line + 9, "%d", &g_Data.progState);
		else if (!strnicmp (line, "PROGVESSEL", 10)) {
			sscanf (line + 10, "%s", name);
			OBJHANDLE h = oapiGetVesselByName(name);
			if (h != NULL)
				g_Data.progVessel = (Saturn *) oapiGetVesselInterface(h);
		} 
		else if (!strnicmp (line, "PROG", 4))
			sscanf (line + 4, "%d", &g_Data.prog);
		else if (!strnicmp (line, "TLITIME", 7)) {
			sscanf (line + 7, "%f", &flt);
			g_Data.tliTime = flt;
		} 
		else if (!strnicmp (line, "TLIVELOCITY", 11)) {
			sscanf (line + 11, "%f", &flt);
			g_Data.tliVelocity = flt;
		}
	}
}

bool ProjectApolloMFD::SetTLITime (char *rstr)
{
	double time;

	if (sscanf (rstr, "%lf", &time) == 1 && time >= IUSTARTTIME) {
		g_Data.tliTime = time;
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ProjectApolloMFD::SetTLIVelocity (char *rstr)
{
	double v;

	if (sscanf (rstr, "%lf", &v) == 1 && v > 0) {
		g_Data.tliVelocity = v;
		InvalidateDisplay();
		return true;
	}
	return false;
}

void ProjectApolloMFD::StoreStatus (void) const
{
	screenData.screen = screen;
}

void ProjectApolloMFD::RecallStatus (void)
{
	screen = screenData.screen;
}

// MFD message parser
int ProjectApolloMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new ProjectApolloMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

bool TLITimeInput (void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetTLITime(str);
}

bool TLIVelocityInput (void *id, char *str, void *data)
{
	return ((ProjectApolloMFD*)data)->SetTLIVelocity(str);
}

ProjectApolloMFD::ScreenData ProjectApolloMFD::screenData = {PROG_NONE};
