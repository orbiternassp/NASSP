/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2006 

  Project Apollo Configurator

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
  *	Revision 1.9  2012/01/14 22:09:25  tschachim
  *	GN panel split as default
  *	
  *	Revision 1.8  2009/12/17 17:47:44  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.7  2009/09/17 17:48:11  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.6  2009/09/13 15:20:14  dseagrav
  *	LM Checkpoint Commit. Adds LM telemetry, fixed missing switch row init, initial LM support for PAMFD.
  *	
  *	Revision 1.5  2009/09/03 19:22:48  vrouleau
  *	Remove usage on Joystick.ini and VirtualAGC.ini. Moved to respective .launchpag.cfg files
  *	
  *	Revision 1.4  2009/09/02 18:26:46  vrouleau
  *	MultiThread support for vAGC
  *	
  *	Revision 1.3  2009/08/21 17:52:18  vrouleau
  *	Added configurable MaxTimeAcceleration value to cap simulator time acceleration
  *	
  *	Revision 1.2  2009/08/17 13:28:04  tschachim
  *	Enhancement of ChecklistMFD
  *	
  *	Revision 1.1  2009/02/18 23:20:56  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.15  2008/07/13 17:47:12  tschachim
  *	Rearranged realism levels, merged Standard and Quickstart Mode.
  *	
  *	Revision 1.14  2008/04/11 11:50:27  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.13  2007/12/17 21:49:11  lassombra
  *	changed stdcall to cdecl to incorporate stack cleanup.
  *	
  *	Revision 1.12  2007/12/17 15:09:13  lassombra
  *	Added function to alert Project Apollo's Configurator/MFD as to the identity of the Socket we are opening when we start the sim.  As a result, the configurator can now shutdown the socket when we close to launchpad, fixing the "TELECOM: BIND() FAILED" problem.  Also now, we have access to the socket in the MFD and can use it for various debugging purposes.
  *	
  *	Revision 1.11  2007/12/04 20:26:28  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.10  2007/11/17 02:40:31  lassombra
  *	Added config option for split lower equipment bay, will write to config file as "GNSPLIT"
  *	
  *	Revision 1.9  2007/11/16 06:46:50  lassombra
  *	Reverted to Version 1.7.  Was not intended to be committed, and after discussion on the forums, was decided that commit was not a good thing
  *	
  *	Revision 1.7  2007/10/18 00:23:28  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.6  2007/08/13 16:17:30  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.5  2007/04/25 18:50:00  tschachim
  *	Joystick configuration.
  *	
  *	Revision 1.4  2007/02/02 13:55:44  tschachim
  *	CSM RHC/THC Z axis autodetection.
  *	
  *	Revision 1.3  2007/01/29 18:58:42  tschachim
  *	Basic joystick configuration.
  *	
  *	Revision 1.2  2006/12/19 15:56:16  tschachim
  *	ECS test stuff, bugfixes.
  *	
  *	Revision 1.1  2006/11/13 14:47:35  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  **************************************************************************/

#pragma warning(disable : 4996 ) 

#define STRICT 1
#define ORBITER_MODULE
// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include <commctrl.h>
#include "resource.h"
#include <stdio.h>

// ==============================================================
// Some global parameters

#define MAX_TABNUM 4

// file name for storing custom parameters
static const char *cfgfile = "ProjectApollo/Saturn5.launchpad.cfg";

class ProjectApolloConfigurator;

///
/// \ingroup DLLsupport
///
static struct {
	HINSTANCE hInst;	
	HWND hDlgTabs[MAX_TABNUM];
	ProjectApolloConfigurator *item;

	int Saturn_MainPanelSplit;
	int Saturn_GNSplit;
	int Saturn_LowRes;
	int Saturn_FDAIDisabled;
	int Saturn_FDAISmooth;
	int Saturn_RHC;
	int Saturn_THC;
	int Saturn_RHCTHCToggle;
	int Saturn_RHCTHCToggleId;
	int Saturn_ChecklistAutoSlow;
	int Saturn_ChecklistAutoDisabled;
	int Saturn_OrbiterAttitudeDisabled;
	int Saturn_SequencerSwitchLightingDisabled;
	int Saturn_MaxTimeAcceleration;
	int Saturn_MultiThread;
	int Saturn_VAGCChecklistAutoSlow;
	int Saturn_VAGCChecklistAutoEnabled;
} gParams;


// ==============================================================
// A class defining the new launchpad parameter item
// This opens a dialog box for a user-defined item, and writes
// the value to a file to be read next time.
// ==============================================================

///
/// \ingroup Config
///
class ProjectApolloConfigurator: public LaunchpadItem {
public:
	ProjectApolloConfigurator();
	char *Name() { return "Project Apollo Configuration"; }
	char *Description() { return "Global configuration for Project Apollo - NASSP."; }
	bool clbkOpen (HWND hLaunchpad);
	int clbkWriteConfig ();
	static BOOL CALLBACK DlgProcFrame (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgProcVisual (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgProcControl (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void WriteConfig(FILEHANDLE hFile);
	static void UpdateControlState(HWND hWnd);
};

ProjectApolloConfigurator::ProjectApolloConfigurator (): LaunchpadItem ()
{
	char *line;
	// Read the current parameter values from file
	FILEHANDLE hFile = oapiOpenFile(cfgfile, FILE_IN, CONFIG);
	while (oapiReadScenario_nextline(hFile, line)) {
		if (!strnicmp (line, "MAINPANELSPLIT", 14)) {
			sscanf (line + 14, "%i", &gParams.Saturn_MainPanelSplit);
		} else if (!strnicmp (line, "LOWRES", 6)) {
			sscanf (line + 6, "%i", &gParams.Saturn_LowRes);
		} else if (!strnicmp (line, "FDAIDISABLED", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_FDAIDisabled);
		} else if (!strnicmp (line, "FDAISMOOTH", 10)) {
			sscanf (line + 10, "%i", &gParams.Saturn_FDAISmooth);
		} else if (!strnicmp (line, "CHECKLISTAUTOSLOW", 17)) {
			sscanf (line + 17, "%i", &gParams.Saturn_ChecklistAutoSlow);
		} else if (!strnicmp (line, "CHECKLISTAUTODISABLED", 21)) {
			sscanf (line + 21, "%i", &gParams.Saturn_ChecklistAutoDisabled);
		} else if (!strnicmp (line, "ORBITERATTITUDEDISABLED", 23)) {
			sscanf (line + 23, "%i", &gParams.Saturn_OrbiterAttitudeDisabled);
		} else if (!strnicmp (line, "SEQUENCERSWITCHLIGHTINGDISABLED", 31)) {
			sscanf (line + 31, "%i", &gParams.Saturn_SequencerSwitchLightingDisabled);
		} else if (!strnicmp (line, "GNSPLIT", 7)) {
			sscanf (line + 7, "%i", &gParams.Saturn_GNSplit);
		} else if (!strnicmp (line, "MAXTIMEACC", 10)) {
			sscanf (line + 10, "%i", &gParams.Saturn_MaxTimeAcceleration);
		} else if (!strnicmp (line, "MULTITHREAD", 11)) {
			sscanf (line + 11, "%i", &gParams.Saturn_MultiThread);
		} else if (!strnicmp (line, "VAGCCHECKLISTAUTOSLOW", 21)) {
			sscanf (line + 21, "%i", &gParams.Saturn_VAGCChecklistAutoSlow);
		} else if (!strnicmp (line, "VAGCCHECKLISTAUTOENABLED", 24)) {
			sscanf (line + 24, "%i", &gParams.Saturn_VAGCChecklistAutoEnabled);
		} else if (!strnicmp (line, "JOYSTICK_RHC", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_RHC);
		} else if (!strnicmp (line, "JOYSTICK_THC", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_THC);
		} else if (!strnicmp (line, "JOYSTICK_RTTID", 14)) {
			sscanf (line + 14, "%i", &gParams.Saturn_RHCTHCToggleId);
		} else if (!strnicmp (line, "JOYSTICK_RTT", 12)) {
			sscanf (line + 12, "%i", &gParams.Saturn_RHCTHCToggle);
		}
	}	
	oapiCloseFile (hFile, FILE_IN);
}

bool ProjectApolloConfigurator::clbkOpen (HWND hLaunchpad)
{
	// respond to user double-clicking the item in the list
	DialogBox(gParams.hInst, MAKEINTRESOURCE (IDD_MYFRAME), hLaunchpad, DlgProcFrame);
	return true;
}

int ProjectApolloConfigurator::clbkWriteConfig ()
{
	// called when orbiter needs to write its configuration to disk
	WriteConfig(oapiOpenFile(cfgfile, FILE_OUT, CONFIG));
	WriteConfig(oapiOpenFile("ProjectApollo/Saturn1b.launchpad.cfg", FILE_OUT, CONFIG));
	WriteConfig(oapiOpenFile("ProjectApollo/LEM.launchpad.cfg", FILE_OUT, CONFIG));

	return 0;
}

void ProjectApolloConfigurator::WriteConfig(FILEHANDLE hFile)
{
	char cbuf[1000];

	sprintf(cbuf, "LOWRES %d", gParams.Saturn_LowRes);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "MAINPANELSPLIT %d", gParams.Saturn_MainPanelSplit);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "GNSPLIT %d", gParams.Saturn_GNSplit);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "FDAIDISABLED %d", gParams.Saturn_FDAIDisabled);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "FDAISMOOTH %d", gParams.Saturn_FDAISmooth);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "CHECKLISTAUTOSLOW %d", gParams.Saturn_ChecklistAutoSlow);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "CHECKLISTAUTODISABLED %d", gParams.Saturn_ChecklistAutoDisabled);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "ORBITERATTITUDEDISABLED %d", gParams.Saturn_OrbiterAttitudeDisabled);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "SEQUENCERSWITCHLIGHTINGDISABLED %d", gParams.Saturn_SequencerSwitchLightingDisabled);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "MAXTIMEACC %d", gParams.Saturn_MaxTimeAcceleration);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "MULTITHREAD %d", gParams.Saturn_MultiThread);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "VAGCCHECKLISTAUTOSLOW %d", gParams.Saturn_VAGCChecklistAutoSlow);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "VAGCCHECKLISTAUTOENABLED %d", gParams.Saturn_VAGCChecklistAutoEnabled);
	oapiWriteLine(hFile, cbuf);

	sprintf(cbuf, "JOYSTICK_RHC %d", gParams.Saturn_RHC);
	oapiWriteLine(hFile, cbuf);

	oapiWriteLine(hFile, "JOYSTICK_RAUTO");	// Not configurable currently

	sprintf(cbuf, "JOYSTICK_RTT %d", gParams.Saturn_RHCTHCToggle);
	oapiWriteLine(hFile, cbuf);
	
	sprintf(cbuf, "JOYSTICK_RTTID %d", gParams.Saturn_RHCTHCToggleId);
	oapiWriteLine(hFile, cbuf);
	
	sprintf(cbuf, "JOYSTICK_THC %d", gParams.Saturn_THC);
	oapiWriteLine(hFile, cbuf);
	
	oapiWriteLine(hFile, "JOYSTICK_TAUTO");	// Not configurable currently
	oapiWriteLine(hFile, "JOYSTICK_TJT");	// Not configurable currently

	oapiCloseFile (hFile, FILE_OUT);
}

BOOL CALLBACK ProjectApolloConfigurator::DlgProcFrame (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCITEM tabitem;
    HWND hTab;
    NMHDR *hdr;
	POINT pt;
	RECT rc;
	char buffer[100];
	int i;

	switch (uMsg) {
	case WM_INITDIALOG: // display the current value

	    hTab = GetDlgItem(hWnd, IDC_TAB); 
		memset(&tabitem, 0, sizeof(tabitem));	
    
		tabitem.mask = TCIF_TEXT;
		tabitem.pszText = "Visuals";
		SendMessage(hTab, TCM_INSERTITEM, 0, (LPARAM) &tabitem);

		tabitem.pszText = "Controls";
		SendMessage(hTab, TCM_INSERTITEM, 1, (LPARAM) &tabitem);

		tabitem.pszText = "Quickstart Mode";
		SendMessage(hTab, TCM_INSERTITEM, 2, (LPARAM) &tabitem);

		tabitem.pszText = "Virtual AGC Mode";
		SendMessage(hTab, TCM_INSERTITEM, 3, (LPARAM) &tabitem);

		// set tab control display area
		GetWindowRect(hTab, &rc);
		TabCtrl_AdjustRect(hTab, false, &rc);
		pt.x = rc.left;
		pt.y = rc.top;
		ScreenToClient(hWnd, &pt);

		// create the child windows
		gParams.hDlgTabs[0] = CreateDialog(gParams.hInst, MAKEINTRESOURCE(IDD_PAGEVISUAL), hWnd, (DLGPROC) DlgProcVisual);
		MoveWindow(gParams.hDlgTabs[0], pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, false);
		ShowWindow(gParams.hDlgTabs[0], SW_SHOW);

		gParams.hDlgTabs[1] = CreateDialog(gParams.hInst, MAKEINTRESOURCE(IDD_PAGECONTROL), hWnd, (DLGPROC) DlgProcControl);
		MoveWindow(gParams.hDlgTabs[1], pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, false);
		ShowWindow(gParams.hDlgTabs[1], SW_HIDE);

		gParams.hDlgTabs[2] = CreateDialog(gParams.hInst, MAKEINTRESOURCE(IDD_PAGEQUICKSTART), hWnd, (DLGPROC) DlgProcControl);
		MoveWindow(gParams.hDlgTabs[2], pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, false);
		ShowWindow(gParams.hDlgTabs[2], SW_HIDE);

		gParams.hDlgTabs[3] = CreateDialog(gParams.hInst, MAKEINTRESOURCE(IDD_PAGEVIRTUALAGC), hWnd, (DLGPROC) DlgProcControl);
		MoveWindow(gParams.hDlgTabs[3], pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top, false);
		ShowWindow(gParams.hDlgTabs[3], SW_HIDE);

		return TRUE;

	case WM_NOTIFY:
	     hdr = (LPNMHDR) lParam;
	     if (hdr->code == TCN_SELCHANGING || hdr->code == TCN_SELCHANGE) {
		   int index;
		   index = TabCtrl_GetCurSel(hdr->hwndFrom);
		   if (index >= 0 && index < MAX_TABNUM) 
			   ShowWindow(gParams.hDlgTabs[index], (hdr->code == TCN_SELCHANGE) ? SW_SHOW : SW_HIDE);
	     }
	     break;
 
	case WM_COMMAND:
			switch (LOWORD (wParam)) {
			case IDOK:    // store the value
				// Visuals Tab
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_RADIO_HIGHRES, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_LowRes = 1;
				} else {
					gParams.Saturn_LowRes = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_RADIO_SINGLEPANEL, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_MainPanelSplit = 1;
				} else {
					gParams.Saturn_MainPanelSplit = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_RADIO_GNSINGLEPANEL, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_GNSplit = 1;
				} else {
					gParams.Saturn_GNSplit = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_CHECK_FDAIDISABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_FDAIDisabled = 1;
				} else {
					gParams.Saturn_FDAIDisabled = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[0], IDC_CHECK_FDAISMOOTH, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_FDAISmooth = 1;
				} else {
					gParams.Saturn_FDAISmooth = 0;
				}

				// Controls Tab
				if (SendDlgItemMessage (gParams.hDlgTabs[1], IDC_CHECK_RHC, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_RHC = -1;
				} else {
					SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_RHC, WM_GETTEXT, 3, (LPARAM) (LPCTSTR) buffer);
					if (sscanf(buffer, "%i", &i) == 1) {
						gParams.Saturn_RHC = i;
					} else {
						gParams.Saturn_RHC = -1;
					}
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[1], IDC_CHECK_THC, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_THC = -1;
				} else {
					SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_THC, WM_GETTEXT, 3, (LPARAM) (LPCTSTR) buffer);
					if (sscanf(buffer, "%i", &i) == 1) {
						gParams.Saturn_THC = i;
					} else {
						gParams.Saturn_THC = -1;
					}
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[1], IDC_CHECK_RHCTHCTOGGLE, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
					gParams.Saturn_RHCTHCToggle = 0;
				} else {
					gParams.Saturn_RHCTHCToggle = 1;
				}

				SendDlgItemMessage(gParams.hDlgTabs[1], IDC_EDIT_RHCTHCTOGGLE, WM_GETTEXT, 3, (LPARAM) (LPCTSTR) buffer);
				if (sscanf(buffer, "%i", &i) == 1) {
					gParams.Saturn_RHCTHCToggleId = i;
				} else {
					gParams.Saturn_RHCTHCToggleId = -1;
				}

				// Quickstart Tab
				if (SendDlgItemMessage (gParams.hDlgTabs[2], IDC_CHECK_CHECKLISTAUTOSLOW, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_ChecklistAutoSlow = 1;
				} else {
					gParams.Saturn_ChecklistAutoSlow = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[2], IDC_CHECK_CHECKLISTAUTODISABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_ChecklistAutoDisabled = 1;
				} else {
					gParams.Saturn_ChecklistAutoDisabled = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[2], IDC_CHECK_ORBITERATTITUDEDISABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_OrbiterAttitudeDisabled = 1;
				} else {
					gParams.Saturn_OrbiterAttitudeDisabled = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[2], IDC_CHECK_SEQUENCERSWITCHLIGHTINGDISABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_SequencerSwitchLightingDisabled = 1;
				} else {
					gParams.Saturn_SequencerSwitchLightingDisabled = 0;
				}

				// VAGC Tab
				SendDlgItemMessage(gParams.hDlgTabs[3], IDC_EDIT_TIMEACC, WM_GETTEXT, 4, (LPARAM) (LPCTSTR) buffer);
				if (sscanf(buffer, "%i", &i) == 1) {
					gParams.Saturn_MaxTimeAcceleration = i;
				} else {
					gParams.Saturn_MaxTimeAcceleration = 100;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[3], IDC_CHECK_MULTITHREAD, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_MultiThread = 1;
				} else {
					gParams.Saturn_MultiThread = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[3], IDC_CHECK_VAGCCHECKLISTAUTOSLOW, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_VAGCChecklistAutoSlow = 1;
				} else {
					gParams.Saturn_VAGCChecklistAutoSlow = 0;
				}
				if (SendDlgItemMessage (gParams.hDlgTabs[3], IDC_CHECK_VAGCCHECKLISTAUTOENABLED, BM_GETCHECK, 0, 0) == BST_CHECKED) {
					gParams.Saturn_VAGCChecklistAutoEnabled = 1;
				} else {
					gParams.Saturn_VAGCChecklistAutoEnabled = 0;
				}

				EndDialog (hWnd, 0);
				return 0;

			case IDCANCEL:
				EndDialog (hWnd, 0);
				return 0;
			}
		
		break;
	}
	return 0;
}

BOOL CALLBACK ProjectApolloConfigurator::DlgProcVisual (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG: // display the current value
		SendDlgItemMessage(hWnd, IDC_RADIO_HIGHRES,   BM_SETCHECK, gParams.Saturn_LowRes?BST_UNCHECKED:BST_CHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_RADIO_LOWRES, BM_SETCHECK, gParams.Saturn_LowRes?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_RADIO_SINGLEPANEL,   BM_SETCHECK, gParams.Saturn_MainPanelSplit?BST_UNCHECKED:BST_CHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_RADIO_SPLITTEDPANEL, BM_SETCHECK, gParams.Saturn_MainPanelSplit?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_RADIO_GNSINGLEPANEL, BM_SETCHECK, gParams.Saturn_GNSplit?BST_UNCHECKED:BST_CHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_RADIO_GNSPLITTEDPANEL, BM_SETCHECK, gParams.Saturn_GNSplit?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_FDAIDISABLED, BM_SETCHECK, gParams.Saturn_FDAIDisabled?BST_CHECKED:BST_UNCHECKED, 0);
		SendDlgItemMessage(hWnd, IDC_CHECK_FDAISMOOTH, BM_SETCHECK, gParams.Saturn_FDAISmooth?BST_CHECKED:BST_UNCHECKED, 0);
		
		return TRUE;
	}
	return 0;
}

BOOL CALLBACK ProjectApolloConfigurator::DlgProcControl (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char buffer[100];

	switch (uMsg) {
	case WM_INITDIALOG: // display the current value
		if (gParams.Saturn_RHC == -1) {
			SendDlgItemMessage(hWnd, IDC_CHECK_RHC, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(hWnd, IDC_EDIT_RHC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) "0");
		} else {
			SendDlgItemMessage(hWnd, IDC_CHECK_RHC, BM_SETCHECK, BST_CHECKED, 0);
			sprintf(buffer, "%i", gParams.Saturn_RHC);
			SendDlgItemMessage(hWnd, IDC_EDIT_RHC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);
		}

		if (gParams.Saturn_THC == -1) {
			SendDlgItemMessage(hWnd, IDC_CHECK_THC, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(hWnd, IDC_EDIT_THC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) "1");
		} else {
			SendDlgItemMessage(hWnd, IDC_CHECK_THC, BM_SETCHECK, BST_CHECKED, 0);
			sprintf(buffer, "%i", gParams.Saturn_THC);
			SendDlgItemMessage(hWnd, IDC_EDIT_THC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);
		}


		SendDlgItemMessage(hWnd, IDC_CHECK_RHCTHCTOGGLE, BM_SETCHECK, gParams.Saturn_RHCTHCToggle?BST_CHECKED:BST_UNCHECKED, 0);

		if (gParams.Saturn_RHCTHCToggleId == -1) {
			SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) "0");
		} else {
			sprintf(buffer, "%i", gParams.Saturn_RHCTHCToggleId);
			SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);
		}

		SendDlgItemMessage(hWnd, IDC_CHECK_CHECKLISTAUTOSLOW, BM_SETCHECK, gParams.Saturn_ChecklistAutoSlow?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_CHECKLISTAUTODISABLED, BM_SETCHECK, gParams.Saturn_ChecklistAutoDisabled?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_ORBITERATTITUDEDISABLED, BM_SETCHECK, gParams.Saturn_OrbiterAttitudeDisabled?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_SEQUENCERSWITCHLIGHTINGDISABLED, BM_SETCHECK, gParams.Saturn_SequencerSwitchLightingDisabled?BST_CHECKED:BST_UNCHECKED, 0);

		sprintf(buffer,"%i",gParams.Saturn_MaxTimeAcceleration);
		SendDlgItemMessage(hWnd, IDC_EDIT_TIMEACC, WM_SETTEXT, 0, (LPARAM) (LPCTSTR) buffer);

		SendDlgItemMessage(hWnd, IDC_CHECK_MULTITHREAD, BM_SETCHECK, gParams.Saturn_MultiThread?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_VAGCCHECKLISTAUTOSLOW, BM_SETCHECK, gParams.Saturn_VAGCChecklistAutoSlow?BST_CHECKED:BST_UNCHECKED, 0);

		SendDlgItemMessage(hWnd, IDC_CHECK_VAGCCHECKLISTAUTOENABLED, BM_SETCHECK, gParams.Saturn_VAGCChecklistAutoEnabled?BST_CHECKED:BST_UNCHECKED, 0);

		UpdateControlState(hWnd);
		return TRUE;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_RHC)) {
			UpdateControlState(hWnd);

		} else if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_THC)) {
			UpdateControlState(hWnd);

		} else if (HIWORD(wParam) == BN_CLICKED && (HWND)lParam == GetDlgItem(hWnd, IDC_CHECK_RHCTHCTOGGLE)) {
			UpdateControlState(hWnd);
		} 
		break;

	case WM_CTLCOLORSTATIC:
		// Set the color of the text
		if ((HWND)lParam == GetDlgItem(hWnd, IDC_STATIC_JOYATT) || (HWND)lParam == GetDlgItem(hWnd, IDC_STATIC_ORBITERATT)) {
			// were about to draw the static
			// set the text color in (HDC)lParam
			SetBkMode((HDC)wParam,TRANSPARENT);
			SetTextColor((HDC)wParam, RGB(255,0,0));
			return (BOOL)CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		}
		break;
	}
	return 0;
}

void ProjectApolloConfigurator::UpdateControlState(HWND hWnd) {

	long rhcChecked, thcChecked;

	rhcChecked = SendDlgItemMessage (hWnd, IDC_CHECK_RHC, BM_GETCHECK, 0, 0);
	thcChecked = SendDlgItemMessage (hWnd, IDC_CHECK_THC, BM_GETCHECK, 0, 0);

	if (rhcChecked == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHC, EM_SETREADONLY, (WPARAM) (BOOL) false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHC), TRUE);
	} else {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHC, EM_SETREADONLY, (WPARAM) (BOOL) true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHC), FALSE);
	}

	if (thcChecked == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_THC, EM_SETREADONLY, (WPARAM) (BOOL) false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_THC), TRUE);
	} else {
		SendDlgItemMessage(hWnd, IDC_EDIT_THC, EM_SETREADONLY, (WPARAM) (BOOL) true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_THC), FALSE);
	}

	if (rhcChecked == BST_CHECKED && thcChecked == BST_UNCHECKED) {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_RHCTHCTOGGLE), TRUE);
	} else {
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_RHCTHCTOGGLE), FALSE);
		SendDlgItemMessage(hWnd, IDC_CHECK_RHCTHCTOGGLE, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	if (SendDlgItemMessage (hWnd, IDC_CHECK_RHCTHCTOGGLE, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, EM_SETREADONLY, (WPARAM) (BOOL) false, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHCTHCTOGGLE), TRUE);
	} else {
		SendDlgItemMessage(hWnd, IDC_EDIT_RHCTHCTOGGLE, EM_SETREADONLY, (WPARAM) (BOOL) true, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_STATIC_RHCTHCTOGGLE), FALSE);
	}

}

// ==============================================================
// The DLL entry point
// ==============================================================

DLLCLBK void opcDLLInit (HINSTANCE hDLL)
{
	int i;

	// create the new config item
	gParams.hInst = hDLL;

	gParams.Saturn_LowRes = 0;
	gParams.Saturn_MainPanelSplit = 0;
	gParams.Saturn_GNSplit = 1;
	gParams.Saturn_FDAIDisabled = 0;
	gParams.Saturn_FDAISmooth = 0;
	gParams.Saturn_RHC = -1;
	gParams.Saturn_THC = -1;
	gParams.Saturn_RHCTHCToggle = 0;
	gParams.Saturn_RHCTHCToggleId = -1;
	gParams.Saturn_ChecklistAutoSlow = 0;
	gParams.Saturn_ChecklistAutoDisabled = 0;
	gParams.Saturn_OrbiterAttitudeDisabled = 0;
	gParams.Saturn_SequencerSwitchLightingDisabled = 0;
	gParams.Saturn_MaxTimeAcceleration = 0;
	gParams.Saturn_MultiThread = 0;
	gParams.Saturn_VAGCChecklistAutoSlow = 1;
	gParams.Saturn_VAGCChecklistAutoEnabled = 0;

	gParams.item = new ProjectApolloConfigurator;
	for (i = 0; i < MAX_TABNUM; i++)
		gParams.hDlgTabs[i] = NULL;

	// find the config root entry provided by orbiter
	LAUNCHPADITEM_HANDLE root = oapiFindLaunchpadItem ("Vessel configuration");
	// register the config entry
	oapiRegisterLaunchpadItem (gParams.item, root);
}

// ==============================================================
// The DLL exit point
// ==============================================================

DLLCLBK void opcDLLExit (HINSTANCE hDLL)
{
	// Unregister the launchpad items
	oapiUnregisterLaunchpadItem (gParams.item);
	delete gParams.item;
}

// 0 = 4:3
// 1 = 16:10
// 2 = 16:9
static int renderViewportIsWideScreen = 0;

DLLCLBK void opcOpenRenderViewport(HWND renderWnd, DWORD width, DWORD height, BOOL fullscreen)

{
	if (((double) width) / ((double) height) < 1.47) 
		renderViewportIsWideScreen = 0;
	else if (((double) width) / ((double) height) < 1.69) 
		renderViewportIsWideScreen = 1;
	else
		renderViewportIsWideScreen = 2;
}

DLLCLBK int pacRenderViewportIsWideScreen() 
{
	return renderViewportIsWideScreen;
}

static SOCKET close_Socket = INVALID_SOCKET;
static SOCKET close_Socket_LM = INVALID_SOCKET;

DLLCLBK bool pacDefineSocket(SOCKET sockettoclose)
{
	close_Socket = sockettoclose;
	HMODULE hpamfd = GetModuleHandle("modules//plugin//ProjectApolloMFD.dll");
	if (hpamfd)	{
		bool (__cdecl *defineSocketext)(SOCKET);
		defineSocketext = (bool (_cdecl *)(SOCKET))GetProcAddress(hpamfd, "pacDefineSocket");
		if (defineSocketext) {
			defineSocketext(sockettoclose);
		}
	}
	return true;
}

DLLCLBK bool pacDefineSocketLM(SOCKET sockettoclose)
{
	close_Socket_LM = sockettoclose;
	HMODULE hpamfd = GetModuleHandle("modules//plugin//ProjectApolloMFD.dll");
	if (hpamfd)	{
		bool (__cdecl *defineSocketext)(SOCKET);
		defineSocketext = (bool (_cdecl *)(SOCKET))GetProcAddress(hpamfd, "pacDefineSocketLM");
		if (defineSocketext) {
			defineSocketext(sockettoclose);
		}
	}
	return true;
}

DLLCLBK void opcCloseRenderViewport()
{
	if (close_Socket != INVALID_SOCKET)	{
		shutdown(close_Socket, 2);	//Shutdown, NOW!!!
		closesocket(close_Socket);
	}
	if (close_Socket_LM != INVALID_SOCKET)	{
		shutdown(close_Socket_LM, 2);	// LM socket too
		closesocket(close_Socket_LM);
	}
}
