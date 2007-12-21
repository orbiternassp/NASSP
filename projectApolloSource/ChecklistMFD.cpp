/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Checklist MFD

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
  **************************************************************************/

#define STRICT
#define ORBITER_MODULE
#include "math.h"
#include "windows.h"
#include "orbitersdk.h"

#include "nasspsound.h"
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"
#include "Crawler.h"
#include "papi.h"
#include <stdio.h>

#include "MFDResource.h"
#include "ChecklistMFD.h"


// ==============================================================
// Global variables

HINSTANCE g_hDLL;
int g_MFDmode; // identifier for new MFD mode


#define PROG_NONE		0
#define PROG_GNC		1
#define PROG_ECS		2
#define PROG_IMFD		3
#define PROG_IMFDTLI	4
//This program displays info on the current telcom socket.  For debugging only.
#define PROG_SOCK		5
#define PROG_DEBUG		6

#define PROGSTATE_NONE				0
#define PROGSTATE_TLI_START			1
#define PROGSTATE_TLI_REQUESTING	2
#define PROGSTATE_TLI_WAITING		3
#define PROGSTATE_TLI_RUNNING		4
#define PROGSTATE_TLI_ERROR			5

// Time to ejection when the IU is programmed
#define IUSTARTTIME 900

static struct {  // global data storage
	int prog;	
	int progState;  
	Saturn *progVessel;
	double nextRequestTime;

	IMFD_BURN_DATA burnData;
	bool isRequesting;
	bool isRequestingManually;
	double requestMjd;
	char *errorMessage;
} g_Data;
SOCKET close_Socket = INVALID_SOCKET;
char debugString[100];
char debugStringBuffer[100];
DLLCLBK void opcDLLInit (HINSTANCE hDLL)
{
	static char *name = "Project Apollo Checklist";      // MFD mode name
	MFDMODESPEC spec;
	spec.name = name;
	spec.key = OAPI_KEY_A;					   // MFD mode selection key is obsolete
	spec.msgproc = ChecklistMFD::MsgProc;  // MFD mode callback function

	// Register the new MFD mode with Orbiter
	g_MFDmode = oapiRegisterMFDMode (spec);
	g_hDLL = hDLL;

	g_Data.prog = PROG_NONE;
	g_Data.progState = 0;
	g_Data.progVessel = NULL;
	g_Data.nextRequestTime = 0;

	ZeroMemory(&g_Data.burnData, sizeof(IMFD_BURN_DATA));
	g_Data.isRequesting = false;
	g_Data.isRequestingManually = false;
	g_Data.requestMjd = 0;
	g_Data.errorMessage = "";
}

DLLCLBK void opcDLLExit (HINSTANCE hDLL)
{
	// Unregister the custom MFD mode when the module is unloaded
	oapiUnregisterMFDMode (g_MFDmode);
}

void StartIMFDRequest() {

	g_Data.isRequesting = true;
	if (!g_Data.progVessel->GetIMFDClient()->IsBurnDataRequesting())
		g_Data.progVessel->GetIMFDClient()->StartBurnDataRequests();
}

void StopIMFDRequest() {

	g_Data.isRequesting = false;
	if (!g_Data.isRequestingManually)
		g_Data.progVessel->GetIMFDClient()->StopBurnDataRequests();
}


DLLCLBK void opcTimestep (double simt, double simdt, double mjd)
{

	// Recover if MFD was closed and TLI is in progress
	if (g_Data.progVessel) {
		if (g_Data.progVessel->GetIU()->IsTLIInProgress()) {
			if (g_Data.prog != PROG_IMFDTLI || g_Data.progState != PROGSTATE_TLI_RUNNING) {
				g_Data.prog = PROG_IMFDTLI;
				g_Data.progState = PROGSTATE_TLI_RUNNING;
				if (!g_Data.isRequesting) {
					StartIMFDRequest();
					g_Data.requestMjd = mjd;
				}
			}
   		}
	}
	
	if (g_Data.prog == PROG_IMFDTLI) {
		switch (g_Data.progState) {
		case PROGSTATE_TLI_START:
			StartIMFDRequest();
			g_Data.requestMjd = mjd;
			g_Data.progState = PROGSTATE_TLI_REQUESTING;
			break;

		case PROGSTATE_TLI_REQUESTING:
			if (g_Data.progVessel->GetIMFDClient()->IsBurnDataValid()) {
				g_Data.burnData = g_Data.progVessel->GetIMFDClient()->GetBurnData();
				StopIMFDRequest();
				if (!g_Data.burnData.p30mode || g_Data.burnData.impulsive) {
					g_Data.errorMessage ="IMFD not in Off-Axis, P30 Mode";
					g_Data.progState = PROGSTATE_TLI_ERROR;
				} else if ((g_Data.burnData.IgnMJD - mjd) * 24. * 3600. <= IUSTARTTIME) {
					g_Data.errorMessage ="Time to burn smaller than 900s";
					g_Data.progState = PROGSTATE_TLI_ERROR;
				} else {
					g_Data.progState = PROGSTATE_TLI_WAITING;
				}
			} else if ((mjd - g_Data.requestMjd) * 24. * 3600. > 10) {
				StopIMFDRequest();
				g_Data.errorMessage ="Request timeout";
				g_Data.progState = PROGSTATE_TLI_ERROR;
			}
			break;

		case PROGSTATE_TLI_WAITING:
			if ((g_Data.burnData.IgnMJD - mjd) * 24. * 3600. <= IUSTARTTIME) {
				if (g_Data.progVessel->GetIU()->StartTLIBurn(g_Data.burnData._RIgn, g_Data.burnData._VIgn, g_Data.burnData._dV_LVLH, g_Data.burnData.IgnMJD)) {
					g_Data.nextRequestTime = 500;
					g_Data.progState = PROGSTATE_TLI_RUNNING;
				} else {
					g_Data.errorMessage ="S-IVB start error";
					g_Data.progState = PROGSTATE_TLI_ERROR;
				}
			}
			break;

		case PROGSTATE_TLI_RUNNING:
			if (!g_Data.isRequesting) {
				if (g_Data.nextRequestTime > 0 && (g_Data.burnData.IgnMJD - mjd) * 24. * 3600. <= g_Data.nextRequestTime) {
					StartIMFDRequest();
					g_Data.requestMjd = mjd;
				}
			} else {
				if (g_Data.progVessel->GetIMFDClient()->IsBurnDataValid()) {
					g_Data.burnData = g_Data.progVessel->GetIMFDClient()->GetBurnData();
					StopIMFDRequest();
					if (g_Data.burnData.p30mode && !g_Data.burnData.impulsive && g_Data.burnData.IgnMJD > mjd) {
						g_Data.progVessel->GetIU()->StartTLIBurn(g_Data.burnData._RIgn, g_Data.burnData._VIgn, g_Data.burnData._dV_LVLH, g_Data.burnData.IgnMJD);
					}
				} else if ((mjd - g_Data.requestMjd) * 24. * 3600. > 2) {
					StopIMFDRequest();
				}
				if (!g_Data.isRequesting) {
					g_Data.nextRequestTime -= 100;
					if (g_Data.nextRequestTime <= 10) 
						g_Data.nextRequestTime = 0;
					else if (g_Data.nextRequestTime <= 100) 
						g_Data.nextRequestTime = 4;
				}
			}
			if (!g_Data.progVessel->GetIU()->IsTLIInProgress()) {
				g_Data.prog = PROG_NONE;
				g_Data.progState = PROGSTATE_NONE;
   			}
		}
	}
}

// ==============================================================
// MFD class implementation

// Constructor
ChecklistMFD::ChecklistMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD (w, h, vessel)

{
	saturn = NULL;
	crawler = NULL;
	width = w;
	height = h;
	hBmpLogo = LoadBitmap(g_hDLL, MAKEINTRESOURCE (IDB_LOGO));
	screen = PROG_NONE;
	debug_frozen = false;

	//We need to find out what type of vessel it is, so we check for the class name.
	//Saturns have different functions than Crawlers.  But we have methods for both.
	if (!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn5") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn5") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo\\Saturn1b") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/Saturn1b")) {
		saturn = (Saturn *)vessel;
		g_Data.progVessel = saturn;
	}
	else if (!stricmp(vessel->GetClassName(), "ProjectApollo\\Crawler") ||
		!stricmp(vessel->GetClassName(), "ProjectApollo/Crawler"))  {
			crawler = (Crawler *)vessel;
	}
}

// Destructor
ChecklistMFD::~ChecklistMFD ()
{
	// Add MFD cleanup code here
}

// Return button labels
char *ChecklistMFD::ButtonLabel (int bt)
{
	// The labels for the buttons used by our MFD mode
	//Additional button added to labelNone for testing socket work, be SURE to remove it.
	//Additional button added at the bottom right of none for the debug string.
	static char *labelNone[12] = {"GNC", "ECS", "IMFD", "SOCK","FLSH","","","","","","DBG"};
	static char *labelGNC[2] = {"BCK", "DMP"};
	static char *labelECS[4] = {"BCK", "CRW", "PRM", "SEC"};
	static char *labelIMFDTliStop[3] = {"BCK", "REQ", "SIVB"};
	static char *labelIMFDTliRun[3] = {"BCK", "REQ", "STP"};
	static char *labelSOCK[1] = {"BCK"};
	static char *labelDEBUG[12] = {"","","","","","","","","","CLR","FRZ","BCK"};

	//If we are working with an unsupported vehicle, we don't want to return any button labels.
	if (!saturn) {
		return 0;
	}
	if (screen == PROG_GNC) {
		return (bt < 2 ? labelGNC[bt] : 0);
	}
	else if (screen == PROG_ECS) {
		return (bt < 4 ? labelECS[bt] : 0);
	}
	else if (screen == PROG_IMFD) {
		if (g_Data.progState == PROGSTATE_NONE)
			return (bt < 3 ? labelIMFDTliStop[bt] : 0);
		else
			return (bt < 3 ? labelIMFDTliRun[bt] : 0);
	}
	else if (screen == PROG_SOCK) {
		return (bt < 1 ? labelSOCK[bt] : 0);
	}
	else if (screen == PROG_DEBUG) {
		return (bt < 12 ? labelDEBUG[bt] : 0);
	}
	return (bt < 12 ? labelNone[bt] : 0);
}

// Return button menus
int ChecklistMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	// The menu descriptions for the buttons used by our MFD mode
	static const MFDBUTTONMENU mnuNone[12] = {
		{"Guidance, Navigation & Control", 0, 'G'},
		{"Environmental Control System", 0, 'E'},
		{"IMFD Support", 0, 'I'},
		{"Socket info", 0, 'S'},
		{"Flash", 0, 'F'},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{"Debug String",0,'D'}
	};
	static const MFDBUTTONMENU mnuGNC[2] = {
		{"Back", 0, 'B'},
		{"Virtual AGC core dump", 0, 'D'}
	};
	static const MFDBUTTONMENU mnuECS[4] = {
		{"Back", 0, 'B'},
		{"Crew number", 0, 'C'},
		{"Primary coolant loop test heating", 0, 'P'},
		{"Secondary coolant loop test heating", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuIMFDTliStop[3] = {
		{"Back", 0, 'B'},
		{"Toggle burn data requests", 0, 'R'},
		{"Start S-IVB burn", 0, 'S'}
	};
	static const MFDBUTTONMENU mnuIMFDTliRun[3] = {
		{"Back", 0, 'B'},
		{"Toggle burn data requests", 0, 'R'},
		{"Start S-IVB burn", 0, 'S'}
	};
	//This menu set is just for the Socket program, remove before release.
	static const MFDBUTTONMENU mnuSOCK[1] = {
		{"Back", 0, 'B'}
	};
	static const MFDBUTTONMENU mnuDebug[12] = {
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{"Clear the Debug Line",0,'C'},
		{"Freeze debug line",0,'F'},
		{"Back",0,'B'}
	};
	// We don't want to display a menu if we are in an unsupported vessel.
	if (!saturn) {
		menu = 0;
		return 0;
	}

	if (screen == PROG_GNC) {
		if (menu) *menu = mnuGNC;
		return 2; 
	} else if (screen == PROG_ECS) {
		if (menu) *menu = mnuECS;
		return 4; 
	} else if (screen == PROG_IMFD) {
		if (g_Data.progState == PROGSTATE_NONE) {
			if (menu) *menu = mnuIMFDTliStop;
			return 3;
		} else {
			if (menu) *menu = mnuIMFDTliRun;
			return 3;
		}
	}
	else if (screen == PROG_SOCK)
	{
		if (menu) *menu = mnuSOCK;
		return 1;
	}
	else if (screen == PROG_DEBUG)
	{
		if (menu) *menu = mnuDebug;
		return 12;
	}
	else {
		if (menu) *menu = mnuNone;
		return 12; 
	}
}

bool ChecklistMFD::ConsumeKeyBuffered (DWORD key) 
{
	//We don't want to accept keyboard commands from the wrong vessels.
	if (!saturn)
		return false;

	if (key == OAPI_KEY_F)
	{
		//
		// Debug on for the minute, to help with crashes :). Remove later.
		//
		sprintf(oapiDebugString(), "F pressed");
		static bool flash = true;

		//
		// Note that you really only need to do this once, when you decide which vessel you
		// want to talk to. We do it every time here as I'm not sure of the best place to
		// put it.
		//
		bool result = PanelConnector.ConnectToVessel(saturn);

		sprintf(oapiDebugString(), "ConnectToVessel = %d", result ? 1 : 0);

		result = PanelConnector.SetFlashing("MissionTimerSwitch", flash);

		sprintf(oapiDebugString(), "SetFlashing = %d", result ? 1 : 0);

		flash = !flash;
	}

	if (screen == PROG_NONE) {
		if (key == OAPI_KEY_G) {
			screen = PROG_GNC;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_E) {
			screen = PROG_ECS;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_I) {
			screen = PROG_IMFD;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_S) {
			screen = PROG_SOCK;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_D) {
			screen = PROG_DEBUG;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
	} else if (screen == PROG_GNC) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} else if (key == OAPI_KEY_D) {
			if (saturn)
				saturn->VirtualAGCCoreDump();
			return true;
		}
	} else if (screen == PROG_ECS) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;

		} else if (key == OAPI_KEY_C) {
			bool CrewNumberInput (void *id, char *str, void *data);
			oapiOpenInputBox ("Crew number [0-3]:", CrewNumberInput, 0, 20, (void*)this);			
			return true;
		
		} else if (key == OAPI_KEY_P) {
			bool PrimECSTestHeaterPowerInput (void *id, char *str, void *data);
			oapiOpenInputBox ("Primary coolant loop test heater power [-3000 to 3000 Watt]:", PrimECSTestHeaterPowerInput, 0, 20, (void*)this);			
			return true;

		} else if (key == OAPI_KEY_S) {
			bool SecECSTestHeaterPowerInput (void *id, char *str, void *data);
			oapiOpenInputBox ("Secondary coolant loop test heater power [-3000 to 3000 Watt]:", SecECSTestHeaterPowerInput, 0, 20, (void*)this);			
			return true;
		}
	} else if (screen == PROG_IMFD) {
		if (key == OAPI_KEY_B) {
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;

		} else if (key == OAPI_KEY_R && !g_Data.isRequestingManually) {						
			if (!saturn->GetIMFDClient()->IsBurnDataRequesting()) {
				saturn->GetIMFDClient()->StartBurnDataRequests();
			}
			g_Data.isRequestingManually = true;

		} else if (key == OAPI_KEY_R && g_Data.isRequestingManually) {
			if (!g_Data.isRequesting) {
				saturn->GetIMFDClient()->StopBurnDataRequests();
			}
			g_Data.isRequestingManually = false;
		
		} else if (key == OAPI_KEY_S && g_Data.progState == PROGSTATE_NONE) {
			g_Data.prog = PROG_IMFDTLI;
			g_Data.progState = PROGSTATE_TLI_START;
			g_Data.progVessel = saturn;
			InvalidateDisplay();
			InvalidateButtons();
			return true;

		} else if (key == OAPI_KEY_S && (g_Data.progState == PROGSTATE_TLI_WAITING || g_Data.progState == PROGSTATE_TLI_ERROR)) {
			g_Data.prog = PROG_NONE;
			g_Data.progState = PROGSTATE_NONE;
			g_Data.errorMessage = "";
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		} 
	}
	//This program is for the socket, remove before release.
	else if (screen == PROG_SOCK)
	{
		if (key == OAPI_KEY_B)
		{
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
	}
	else if (screen == PROG_DEBUG)
	{
		if (key == OAPI_KEY_B)
		{
			screen = PROG_NONE;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		else if (key == OAPI_KEY_F)
		{
			if (debug_frozen)
				debug_frozen = false;
			else
				debug_frozen = true;
			return true;
		}
		else if (key == OAPI_KEY_C)
		{
			sprintf(debugString,"");
			return true;
		}
	}
	return false;
}

bool ChecklistMFD::ConsumeButton (int bt, int event)
{
	//We don't have to implement a test for the correct vessel here, as it checks this already in the consume key method, which we call anyways.
	//We only want to accept left mouse button clicks.
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;

	static const DWORD btkeyNone[12] = { OAPI_KEY_G, OAPI_KEY_E, OAPI_KEY_I, OAPI_KEY_S, OAPI_KEY_F, 0, 0, 0, 0, 0, 0, OAPI_KEY_D };
	static const DWORD btkeyGNC[2] = { OAPI_KEY_B, OAPI_KEY_D };
	static const DWORD btkeyECS[4] = { OAPI_KEY_B, OAPI_KEY_C, OAPI_KEY_P, OAPI_KEY_S };
	static const DWORD btkeyIMFD[3] = { OAPI_KEY_B, OAPI_KEY_R, OAPI_KEY_S };
	static const DWORD btkeySock[1] = { OAPI_KEY_B };
	static const DWORD btkeyDEBUG[12] = { 0,0,0,0,0,0,0,0,0,OAPI_KEY_C,OAPI_KEY_F,OAPI_KEY_B};

	if (screen == PROG_GNC) {
		if (bt < 2) return ConsumeKeyBuffered (btkeyGNC[bt]);
	} else if (screen == PROG_ECS) {
		if (bt < 4) return ConsumeKeyBuffered (btkeyECS[bt]);
	} else if (screen == PROG_IMFD) {
		if (bt < 3) return ConsumeKeyBuffered (btkeyIMFD[bt]);		
	} 
	// This program is the socket data.  Remove before release.
	else if (screen == PROG_SOCK)
	{
		if (bt < 1) return ConsumeKeyBuffered (btkeySock[bt]);
	}
	else if (screen == PROG_DEBUG)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyDEBUG[bt]);
	}
	else {		
		if (bt < 12) return ConsumeKeyBuffered (btkeyNone[bt]);
	}
	return false;
}

// Repaint the MFD
void ChecklistMFD::Update (HDC hDC)
{
	char buffer[100];

	HDC hDCTemp = CreateCompatibleDC(hDC);
	HBITMAP hBmpTemp = (HBITMAP) SelectObject(hDCTemp, hBmpLogo);
	StretchBlt(hDC, 1, 1, width - 2, height - 2, hDCTemp, 0, 0, 256, 256, SRCCOPY);
	DeleteObject(hBmpTemp);
	DeleteDC(hDCTemp);

	// Draws the MFD title
	Title (hDC, "Project Apollo Checklist");

	SelectDefaultFont(hDC, 0);
	SetBkMode (hDC, TRANSPARENT);
	SetTextAlign (hDC, TA_CENTER);

	if (!saturn) {
		SetTextColor (hDC, RGB(255, 0, 0));
		TextOut(hDC, width / 2, (int) (height * 0.5), "Unsupported vessel", 18);
		if (!crawler)
			return;
	}

	// Draw mission time
	SetTextColor (hDC, RGB(0, 255, 0));
	TextOut(hDC, width / 2, (int) (height * 0.1), "Ground Elapsed Time", 19);

	double mt = 0;
	if (!crawler)
		mt = saturn->GetMissionTime();
	else
		mt = crawler->GetMissionTime();
	int secs = abs((int) mt);
	int hours = (secs / 3600);
	secs -= (hours * 3600);
	int minutes = (secs / 60);
	secs -= 60 * minutes;
	if (mt < 0)
		sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
	else
		sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);
	TextOut(hDC, width / 2, (int) (height * 0.15), buffer, strlen(buffer));
	//If this is the crawler and not the actual Saturn, do NOTHING else!
	if (!saturn)
		return;

	SelectDefaultPen(hDC, 1);
	MoveToEx (hDC, (int) (width * 0.05), (int) (height * 0.25), 0);
	LineTo (hDC, (int) (width * 0.95), (int) (height * 0.25));

	// Draw GNC
	if (screen == PROG_GNC) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "Guidance, Navigation & Control", 30);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.4), "Velocity:", 9);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45), "Vert. Velocity:", 15);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.5), "Altitude:", 9);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), "Apoapsis Alt.:", 14);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), "Periapsis Alt.:", 15);

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
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.4), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfft/s", vvel);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.45), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", saturn->GetAltitude() * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.5), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", apDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.6), buffer, strlen(buffer));
		sprintf(buffer, "%.1lfnm", peDist * 0.000539957);
		TextOut(hDC, (int) (width * 0.9), (int) (height * 0.65), buffer, strlen(buffer));

	//Draw Socket details.
	}
	else if (screen == PROG_SOCK)
	{
		TextOut(hDC, width / 2, (int) (height * 0.3), "Socket details", 14);
		sprintf(buffer, "Socket: %i", close_Socket);
		TextOut(hDC, width / 2, (int) (height * 0.4), buffer, strlen(buffer));
	}
	// Draw ECS
	else if (screen == PROG_ECS) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "Environmental Control System", 28);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.4), "Crew status:", 12);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45),  "Crew number:", 12);

		ECSStatus ecs;
		saturn->GetECSStatus(ecs);

		SetTextAlign (hDC, TA_CENTER);
		if (ecs.crewStatus == ECS_CREWSTATUS_OK) {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.4), "OK", 2);	
		} else if (ecs.crewStatus == ECS_CREWSTATUS_CRITICAL) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.4), "CRITICAL", 8);	
			SetTextColor (hDC, RGB(0, 255, 0));
		} else {
			SetTextColor (hDC, RGB(255, 0, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.4), "DEAD", 4);	
			SetTextColor (hDC, RGB(0, 255, 0));
		}

		sprintf(buffer, "%d", ecs.crewNumber);
		TextOut(hDC, (int) (width * 0.7), (int) (height * 0.45), buffer, strlen(buffer)); 

		TextOut(hDC, (int) (width * 0.5), (int) (height * 0.525), "Glycol Coolant Loops", 20);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.6), "Prim.", 5);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.6), "Sec.", 4);

		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), "Heating:", 8);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.65), "Actual:", 7);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.7), "Test:", 5);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), "Total:", 6);

		SetTextAlign (hDC, TA_CENTER);
		sprintf(buffer, "%.0lfW", ecs.PrimECSHeating);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.65), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.PrimECSTestHeating);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.7), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.PrimECSHeating + ecs.PrimECSTestHeating);
		TextOut(hDC, (int) (width * 0.6), (int) (height * 0.8), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.SecECSHeating);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.65), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.SecECSTestHeating);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.7), buffer, strlen(buffer));
		sprintf(buffer, "%.0lfW", ecs.SecECSHeating + ecs.SecECSTestHeating);
		TextOut(hDC, (int) (width * 0.8), (int) (height * 0.8), buffer, strlen(buffer));

		MoveToEx (hDC, (int) (width * 0.5), (int) (height * 0.775), 0);
		LineTo (hDC, (int) (width * 0.9), (int) (height * 0.775));
	// Draw IMFD
	} else if (screen == PROG_IMFD) {
		TextOut(hDC, width / 2, (int) (height * 0.3), "IMFD Burn Data", 14);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.35), "Status:", 7);
		SetTextAlign (hDC, TA_CENTER);
		if (g_Data.isRequestingManually) {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.35), "REQUESTING", 10);	
		} else {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.35), "NONE", 4);
		}
		if (saturn->GetIMFDClient()->IsBurnDataValid() && g_Data.isRequestingManually) {
			IMFD_BURN_DATA bd = saturn->GetIMFDClient()->GetBurnData();
			if (bd.p30mode || bd.impulsive) {
				SetTextAlign (hDC, TA_LEFT);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.45), "GET Ignition:", 13);

				SetTextAlign (hDC, TA_RIGHT);				
				mt = ((bd.IgnMJD - oapiGetSimMJD()) * 24. * 3600.) + saturn->GetMissionTime();
				secs = abs((int) mt);
				hours = (secs / 3600);
				secs -= (hours * 3600);
				minutes = (secs / 60);
				secs -= 60 * minutes;
				if (mt < 0)
					sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
				else
					sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.45), buffer, strlen(buffer));

				SetTextAlign (hDC, TA_LEFT);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.5), "dV x:", 5);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.55), "dV y:", 5);
				TextOut(hDC, (int) (width * 0.1), (int) (height * 0.6), "dV z:", 5);

				SetTextAlign (hDC, TA_RIGHT);				
				sprintf(buffer, "%.1lf ft/s", bd._dV_LVLH.x * FPS);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.5), buffer, strlen(buffer));
				sprintf(buffer, "%.1lf ft/s", bd._dV_LVLH.y * FPS);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.55), buffer, strlen(buffer));
				sprintf(buffer, "%.1lf ft/s", bd._dV_LVLH.z * FPS);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.6), buffer, strlen(buffer));
			} else {
				SetTextAlign (hDC, TA_CENTER);
				SetTextColor (hDC, RGB(255, 0, 0));
				TextOut(hDC, (int) (width * 0.5), (int) (height * 0.5), "IMFD not in P30 Mode", 20);
				SetTextColor (hDC, RGB(0, 255, 0));
			}
		}

		SetTextAlign (hDC, TA_CENTER);
		TextOut(hDC, width / 2, (int) (height * 0.7), "S-IVB Burn Program", 18);
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * 0.1), (int) (height * 0.75), "Status:", 7);

		SetTextAlign (hDC, TA_CENTER);
		if (g_Data.progState == PROGSTATE_NONE) {
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "IDLE", 4);

		} else if (g_Data.progState == PROGSTATE_TLI_REQUESTING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "REQUESTING", 10);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_WAITING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "WAITING", 7);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_RUNNING) {
			SetTextColor (hDC, RGB(255, 255, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "RUNNING", 7);
			SetTextColor (hDC, RGB(0, 255, 0));

		} else if (g_Data.progState == PROGSTATE_TLI_ERROR) {
			SetTextColor (hDC, RGB(255, 0, 0));
			TextOut(hDC, (int) (width * 0.7), (int) (height * 0.75), "ERROR", 5);
			TextOut(hDC, (int) (width * 0.5), (int) (height * 0.8), g_Data.errorMessage, strlen(g_Data.errorMessage));
			SetTextColor (hDC, RGB(0, 255, 0));
		}
		if (g_Data.progState == PROGSTATE_TLI_WAITING || g_Data.progState == PROGSTATE_TLI_RUNNING) {
			SetTextAlign (hDC, TA_LEFT);
			if (g_Data.burnData.IgnMJD != 0) {
				if (oapiGetSimMJD() < g_Data.burnData.IgnMJD) {
					TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), "Time to Ignition:", 17);
					sprintf(buffer, "%.0lf s", (oapiGetSimMJD() - g_Data.burnData.IgnMJD) * 24. * 3600.);
				} else {
					TextOut(hDC, (int) (width * 0.1), (int) (height * 0.8), "Remaining Burn Time:", 20);
					sprintf(buffer, "%.0lf s", g_Data.burnData.BT - ((oapiGetSimMJD() - g_Data.burnData.IgnMJD) * 24. * 3600.));
				}
				SetTextAlign (hDC, TA_RIGHT);
				TextOut(hDC, (int) (width * 0.9), (int) (height * 0.8), buffer, strlen(buffer));
			}
		}
	}
	else if (screen == PROG_DEBUG)
	{

		if ((strcmp(debugString,debugStringBuffer)!= 0) && (strlen(debugStringBuffer) != 0) && !debug_frozen)
		{
			strcpy(debugString, debugStringBuffer);
			sprintf(debugStringBuffer,"");
		}
		TextOut(hDC, width / 2, (int)(height * 0.3), "Debug Data",10);
		if (strlen(debugString) > 35)
		{
			int i = 0;
			double h = 0.4;
			bool done = false;
			while (!done)
			{
				if (strlen(&debugString[i]) > 35)
				{
					TextOut(hDC, width / 2, (int) (height * h), &debugString[i], 35);
					i = i + 35;
					h = h + 0.05;
				}
				else
				{
					TextOut(hDC, width / 2, (int) (height * h), &debugString[i], strlen(&debugString[i]));
					done = true;
				}
			}
		}
		else TextOut(hDC, width / 2, (int) (height * 0.4), debugString, strlen(debugString));
	}
}

void ChecklistMFD::WriteStatus (FILEHANDLE scn) const
{
	oapiWriteScenario_int(scn, "SCREEN", screen);
	oapiWriteScenario_int(scn, "PROGNO", g_Data.prog);
	oapiWriteScenario_int(scn, "PROGSTATE", g_Data.progState);
	if (g_Data.progVessel)
		oapiWriteScenario_string(scn, "PROGVESSEL", g_Data.progVessel->GetName());
	papiWriteScenario_double(scn, "NEXTREQUESTTIME", g_Data.nextRequestTime);
	papiWriteScenario_double(scn, "BURNDATA_IGNMJD", g_Data.burnData.IgnMJD);
	papiWriteScenario_double(scn, "BURNDATA_BT", g_Data.burnData.BT);
	papiWriteScenario_vec(scn, "BURNDATA_RIGN", g_Data.burnData._RIgn);
	papiWriteScenario_vec(scn, "BURNDATA_VIGN", g_Data.burnData._VIgn);
	papiWriteScenario_vec(scn, "BURNDATA_DVLVLH", g_Data.burnData._dV_LVLH);
}

void ChecklistMFD::ReadStatus (FILEHANDLE scn)
{
    char *line, name[100];

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, "END_MFD", 7))
			return;

		if (!strnicmp (line, "PROGVESSEL", 10)) {
			sscanf (line + 10, "%s", name);
			OBJHANDLE h = oapiGetVesselByName(name);
			if (h != NULL)
				g_Data.progVessel = (Saturn *) oapiGetVesselInterface(h);
		} 
		papiReadScenario_int(line, "SCREEN", screen);
		papiReadScenario_int(line, "PROGNO", g_Data.prog);
		papiReadScenario_int(line, "PROGSTATE", g_Data.progState);
		papiReadScenario_double(line, "NEXTREQUESTTIME", g_Data.nextRequestTime);
		papiReadScenario_double(line, "BURNDATA_IGNMJD", g_Data.burnData.IgnMJD);
		papiReadScenario_double(line, "BURNDATA_BT", g_Data.burnData.BT);
		papiReadScenario_vec(line, "BURNDATA_RIGN", g_Data.burnData._RIgn);
		papiReadScenario_vec(line, "BURNDATA_VIGN", g_Data.burnData._VIgn);
		papiReadScenario_vec(line, "BURNDATA_DVLVLH", g_Data.burnData._dV_LVLH);
	}
}

bool ChecklistMFD::SetCrewNumber (char *rstr)
{
	int n;

	if (sscanf (rstr, "%d", &n) == 1 && n >= 0 && n <= 3) {
		if (saturn)
			saturn->SetCrewNumber(n);
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ChecklistMFD::SetPrimECSTestHeaterPower (char *rstr)
{
	double v;

	if (sscanf (rstr, "%lf", &v) == 1 && v >= -3000. && v <= 3000.) {
		if (saturn)
			saturn->SetPrimECSTestHeaterPowerW(v);
		InvalidateDisplay();
		return true;
	}
	return false;
}

bool ChecklistMFD::SetSecECSTestHeaterPower (char *rstr)
{
	double v;

	if (sscanf (rstr, "%lf", &v) == 1 && v >= -3000. && v <= 3000.) {
		if (saturn)
			saturn->SetSecECSTestHeaterPowerW(v);
		InvalidateDisplay();
		return true;
	}
	return false;
}

void ChecklistMFD::StoreStatus (void) const
{
	screenData.screen = screen;
}

void ChecklistMFD::RecallStatus (void)
{
	screen = screenData.screen;
}

// MFD message parser
int ChecklistMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new ChecklistMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}

bool CrewNumberInput (void *id, char *str, void *data)
{
	return ((ChecklistMFD*)data)->SetCrewNumber(str);
}

bool PrimECSTestHeaterPowerInput (void *id, char *str, void *data)
{
	return ((ChecklistMFD*)data)->SetPrimECSTestHeaterPower(str);
}

bool SecECSTestHeaterPowerInput (void *id, char *str, void *data)
{
	return ((ChecklistMFD*)data)->SetSecECSTestHeaterPower(str);
}

ChecklistMFD::ScreenData ChecklistMFD::screenData = {PROG_NONE};


DLLCLBK bool defineSocket(SOCKET sockettoclose)
{
	close_Socket = sockettoclose;
	return true;
}

DLLCLBK char *apolloMFDGetDebugString()
{
	return debugStringBuffer;
}