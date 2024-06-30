/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008

  Project Apollo Checklist MFD

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

#include "math.h"
#include "windows.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "nassputils.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "LEMcomputer.h"
#include "saturn.h"
#include "LEM.h"
#include "Crawler.h"
#include "mccvessel.h"
#include "mcc.h"
#include "papi.h"
#include <stdio.h>
#include <string>

#include "MFDResource.h"
#include "ProjectApolloChecklistMFD.h"

using namespace nassp;

#define PROG_CHKLST			0
#define PROG_CHKLSTNAV		1
#define PROG_CHKLSTREV		2
#define PROG_CHKLSTINFO		3

#define LINE0	.10
#define LINE1	.14
#define LINE2	.18
#define LINE3	.22
#define LINE4	.26
#define LINE5	.30
#define LINE6	.34
#define LINE7	.38
#define LINE8	.42
#define LINE9	.46
#define LINE10	.50
#define LINE11  .54
#define LINE12  .58
#define LINE13  .62
#define LINE14  .66
#define LINE15  .70
#define LINE16  .74
#define LINE17  .78
#define LINE18  .82
#define LINE19  .86
#define NLINES  20
#define HLINE   .04

static int hMFD;
static HINSTANCE g_hDLL;

void ProjectApolloChecklistMFDopcDLLInit (HINSTANCE hDLL)
{
	static char* name = "Project Apollo Checklist";
	MFDMODESPECEX spec;
	spec.key = OAPI_KEY_C;
	spec.name = name;
	spec.context = NULL;
	spec.msgproc = ProjectApolloChecklistMFD::MsgProc;
	hMFD = oapiRegisterMFDMode(spec);
	g_hDLL = hDLL;
}
void ProjectApolloChecklistMFDopcDLLExit (HINSTANCE hDLL)
{
	oapiUnregisterMFDMode(hMFD);
}
void ProjectApolloChecklistMFDopcTimestep (double simt, double simdt, double mjd)
{
}
ProjectApolloChecklistMFD::ProjectApolloChecklistMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD2 (w,h,vessel)
{
	saturn = NULL;
	crawler = NULL;
	lem = NULL;
	conn.ConnectToVessel(vessel);
	width = w;
	height = h;
	screen = PROG_CHKLST;
	NumChkLsts = 0;
	MFDInit = false;
	CurrentStep = 0;
	TopStep = 0;
	HiLghtdLine = 0;
	HiLghtdLineDown = false;
	bDisplayMET = true;

	//We need to find out what type of vessel it is, so we check for the class name.
	//Saturns have different functions than Crawlers.  But we have methods for both.
	if (utils::IsVessel(vessel, utils::Saturn)) {
		saturn = (Saturn *)vessel;
	}
	else if (utils::IsVessel(vessel, utils::Crawler))  {
			crawler = (Crawler *)vessel;
	}
	else if (utils::IsVessel(vessel, utils::LEM)) {
			lem = (LEM *)vessel;
	}

	mcc = NULL;
	OBJHANDLE hMCC = oapiGetVesselByName("MCC");
	if (hMCC != NULL) {
		VESSEL* pVessel = oapiGetVesselInterface(hMCC);
		if (pVessel) {
			if (utils::IsVessel(pVessel, utils::MCC))
			{
				MCCVessel *pMCCVessel = static_cast<MCCVessel*>(pVessel);
				if (pMCCVessel->mcc)
				{
					mcc = pMCCVessel->mcc;
				}
			}
		}
	}

	HBITMAP hBmpLogo = LoadBitmap(g_hDLL, MAKEINTRESOURCE (IDB_LOGO));
	hLogo = oapiCreateSurface(hBmpLogo);
}

ProjectApolloChecklistMFD::~ProjectApolloChecklistMFD ()
{
	/*
	Do not do that, at least not here. The MFD gets destroyed every time you change the 
	panel and is not constructed if the panel has no MFDs, so with this code active you cannot 
	flash switches on the left or right panel for example.

	int i = 0;
	item.group = -1;
	item.index = i;
	while (conn.GetChecklistItem(&item))
	{
		conn.SetFlashing(item.item,false);
		i++;
		item.group = -1;
		item.index = i;
	}
	*/
	oapiDestroySurface(hLogo);
}
char *ProjectApolloChecklistMFD::ButtonLabel (int bt)
{
	// 2nd was "MET", disabled for now, MET is shown by default
	static char *labelCHKLST[12] = {"NAV","","INFO","EXEC","FLSH","AUTO","PgUP","UP","BCK","GOTO","DN","PgDN"};
	static char *labelCHKLSTActiveItem[12] = {"NAV","UNDO","INFO","EXEC","FLSH","AUTO","PgUP","UP","PRO","FAIL","DN","PgDN"};
	static char *labelCHKLSTNAV[12] = {"BCK","","INFO","EXEC","FLSH","AUTO","PgUP","UP","SEL","REV","DN","PgDN"};
	static char *labelCHKLSTREV[12] = {"NAV","","INFO","","","AUTO","PgUP","UP","","","DN","PgDN"};
	static char *labelCHKLSTINFO[12] = {"BCK","","","","","","","","","","",""};

	if (screen == PROG_CHKLST) {
		if (CurrentStep == 0) {
			return (bt < 12 ? labelCHKLSTActiveItem[bt] : 0);
		} else {
			return (bt < 12 ? labelCHKLST[bt] : 0);
		}
	}
	else if (screen == PROG_CHKLSTNAV) {
		return (bt < 12 ? labelCHKLSTNAV[bt] : 0);
	}
	else if (screen == PROG_CHKLSTREV) {
		return (bt < 12 ? labelCHKLSTREV[bt] : 0);
	}
	else if (screen == PROG_CHKLSTINFO) {
		return (bt < 12 ? labelCHKLSTINFO[bt] : 0);
	}

	return 0;
}
int ProjectApolloChecklistMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	static const MFDBUTTONMENU mnuCHKLST[12] = {
		{"Go to","Checklist Navigation",'N'},
		// {"Toggle Display","Mission Elapsed Time",'M'},
		{0,0,0},
		{"More Information","About This Step",'N'},
		{"Toggle Automatic", "Checklist execution", 'E'},
		{"Toggle Flashing",0,'L'},
		{"Toggle AutoComplete",0,'A'},
		{"Scroll Up","One Page",'<'},
		{"Scroll Up","One Line",'U'},
		{"Back To", "Active Item", 'B'},
		{"Go To","Highlighted Step",'R'},
		{"Scroll Down","One Line",'D'},
		{"Scroll Down","One Page",'>'}
	};
	static const MFDBUTTONMENU mnuCHKLSTActiveItem[12] = {
		{"Go to","Checklist Navigation",'N'},
		// {"Toggle Display","Mission Elapsed Time",'M'},
		{"Undo","Last Step",'B'},
		{"More Information","About This Step",'N'},
		{"Toggle Automatic", "Checklist execution", 'E'},
		{"Toggle Flashing",0,'L'},
		{"Toggle AutoComplete",0,'A'},
		{"Scroll Up","One Page",'<'},
		{"Scroll Up","One Line",'U'},
		{"Step Succeeds",0,'S'},
		{"Step Fails",0,'F'},
		{"Scroll Down","One Line",'D'},
		{"Scroll Down","One Page",'>'}
	};
	static const MFDBUTTONMENU mnuCHKLSTNAV[12] = {
		{"Go back to","the Current Checklist",'C'},
		// {"Toggle Display","Mission Elapsed Time",'M'},
		{0,0,0},
		{"More Information","About This Checklist",'N'},
		{"Toggle Automatic", "Checklist execution", 'E'},
		{"Toggle Flashing",0,'L'},
		{"Toggle AutoComplete",0,'A'},
		{"Scroll Up","One Page",'<'},
		{"Scroll Up","One Line",'U'},
		{"Select Checklist",0,'S'},
		{"Review Checklist",0,'R'},
		{"Scroll Down","One Line",'D'},
		{"Scroll Down","One Page",'>'}
	};
	static const MFDBUTTONMENU mnuCHKLSTREV[12] = {
		{"Return to","Checklist Navigation",'C'},
		// {"Toggle Display","Mission Elapsed Time",'M'},
		{0,0,0},
		{"More Information","About This Step",'N'},
		{0,0,0},
		{0,0,0},
		{"Toggle Hints",0,'H'},
		{"Scroll Up","One Page",'<'},
		{"Scroll Up","One Line",'U'},
		{0,0,0},
		{0,0,0},
		{"Scroll Down","One Line",'D'},
		{"Scroll Down","One Page",'>'}
	};
	static const MFDBUTTONMENU mnuCHKLSTINFO[12] = {
		{"Back to","Checklist",'B'},
		// {"Toggle Display","Mission Elapsed Time",'M'},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0}
	};
	if (screen == PROG_CHKLST)
	{
		if (CurrentStep == 0) {
			if (menu) *menu = mnuCHKLSTActiveItem;
		} else {
			if (menu) *menu = mnuCHKLST;
		}
		return 12;
	}
	else if (screen == PROG_CHKLSTNAV)
	{
		if (menu) *menu = mnuCHKLSTNAV;
		return 12;
	}
	else if (screen == PROG_CHKLSTREV)
	{
		if (menu) *menu = mnuCHKLSTREV;
		return 12;
	}
	else if (screen == PROG_CHKLSTINFO)
	{
		if (menu) *menu = mnuCHKLSTINFO;
		return 12;
	}
	return 0;
}
bool ProjectApolloChecklistMFD::ConsumeButton (int bt, int event)
{
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;

	static const DWORD btkeyCHKLST[12] = { OAPI_KEY_C,0,OAPI_KEY_N,OAPI_KEY_E,OAPI_KEY_L,OAPI_KEY_A,OAPI_KEY_PRIOR,OAPI_KEY_U,OAPI_KEY_B,OAPI_KEY_R,OAPI_KEY_D,OAPI_KEY_NEXT };
	static const DWORD btkeyCHKLSTActiveItem[12] = { OAPI_KEY_C,OAPI_KEY_B,OAPI_KEY_N,OAPI_KEY_E,OAPI_KEY_L,OAPI_KEY_A,OAPI_KEY_PRIOR,OAPI_KEY_U,OAPI_KEY_S,OAPI_KEY_F,OAPI_KEY_D,OAPI_KEY_NEXT};
	static const DWORD btkeyCHKLSTNAV[12] = { OAPI_KEY_C,OAPI_KEY_M,OAPI_KEY_N,OAPI_KEY_E,OAPI_KEY_L,OAPI_KEY_A,OAPI_KEY_PRIOR,OAPI_KEY_U,OAPI_KEY_S,OAPI_KEY_R,OAPI_KEY_D,OAPI_KEY_NEXT};
	static const DWORD btkeyCHKLSTREV[12] = { OAPI_KEY_C,OAPI_KEY_M,OAPI_KEY_N,0,0,OAPI_KEY_A,OAPI_KEY_PRIOR,OAPI_KEY_U,0,0,OAPI_KEY_D,OAPI_KEY_NEXT};
	static const DWORD btkeyCHKLSTINFO[12] = { OAPI_KEY_B,OAPI_KEY_M,0,0,0,0,0,0,0,0,0,0};

	if (screen == PROG_CHKLST)
	{
		if (CurrentStep == 0) {
			if (bt < 12) return ConsumeKeyBuffered(btkeyCHKLSTActiveItem[bt]);
		}
		else {
			if (bt < 12) return ConsumeKeyBuffered(btkeyCHKLST[bt]);
		}
	}
	else if (screen == PROG_CHKLSTNAV)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyCHKLSTNAV[bt]);
	}
	else if (screen == PROG_CHKLSTREV)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyCHKLSTREV[bt]);
	}
	else if (screen == PROG_CHKLSTINFO)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyCHKLSTINFO[bt]);
	}
	
	return false;
}
bool ProjectApolloChecklistMFD::ConsumeKeyBuffered (DWORD key)
{
	ChecklistItem *item;

	if (screen == PROG_CHKLST)
	{
		if (key == OAPI_KEY_C)
		{
			screen = PROG_CHKLSTNAV;
			CurrentStep = 0;
			TopStep = 0;
			HiLghtdLine = 0;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_N)
		{
			screen = PROG_CHKLSTINFO;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_E)
		{
			conn.SetAutoExecute(!conn.GetAutoExecute());
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_A)
		{
			ChkLstAutoOn = conn.ChecklistAutocomplete();
			if (conn.ChecklistAutocomplete(!(ChkLstAutoOn)))
				ChkLstAutoOn = !(ChkLstAutoOn);
			
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_PRIOR)
		{
			for (int i = 0; i < 9; i++) {
				CurrentStep--;
				if (!conn.GetChecklistItem(-1, CurrentStep)) {
					CurrentStep++;
					break;
				}
			}
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_U)
		{
			CurrentStep--;
			if (!conn.GetChecklistItem(-1, CurrentStep))
				CurrentStep++;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_S)
		{
			if (CurrentStep == 0) {
				item = conn.GetChecklistItem(-1, CurrentStep);
				if (item)
					conn.completeChecklistItem(item);
			}
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_F)
		{
			if (CurrentStep == 0) {
				item = conn.GetChecklistItem(-1, CurrentStep);
				if (item)
					conn.failChecklistItem(item);
			}
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_R) {
			item = conn.GetChecklistItem(-1, CurrentStep);
			if (item) {
				conn.gotoChecklistItem(item);
				CurrentStep = 0;
			}

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_B && CurrentStep == 0) {
			// Ensure we have a current item
			if (conn.GetChecklistItem(-1, 0)) {
				conn.undoChecklistItem();
				// Let's reset our cursor, just so the user knows we actually did something.
				CurrentStep = 0;
			}
			
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_B && CurrentStep != 0) {
			CurrentStep = 0;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		
		if (key == OAPI_KEY_D)
		{
			CurrentStep++;
			if (!conn.GetChecklistItem(-1, CurrentStep))
				CurrentStep--;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_NEXT)
		{
			for (int i = 0; i < 9; i++) {
				CurrentStep++;
				if (!conn.GetChecklistItem(-1, CurrentStep)) {
					CurrentStep--;
					break;
				}
			}
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_L)
		{
			bool fl = conn.GetChecklistFlashing();
			conn.SetChecklistFlashing(!fl);
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_M)
		{
			if (bDisplayMET) {
				bDisplayMET = false;
			} else {
				bDisplayMET = true;
			}
			InvalidateDisplay();
			return true;
		}
	}
	else if (screen == PROG_CHKLSTNAV)
	{
		if (key == OAPI_KEY_C)
		{
			screen = PROG_CHKLST;
			CurrentStep = 0;
			TopStep = 0;
			HiLghtdLine = 0;
			
			//TODO: Reset MFD
			
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		else if (key == OAPI_KEY_N)
		{
			return true;
		}
		if (key == OAPI_KEY_E)
		{
			conn.SetAutoExecute(!conn.GetAutoExecute());
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_A)
		{
			ChkLstAutoOn = conn.ChecklistAutocomplete();
			if (conn.ChecklistAutocomplete(!(ChkLstAutoOn)))
				ChkLstAutoOn = !(ChkLstAutoOn);
			
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_PRIOR)
		{
			TopStep -= NLINES/2;
			CurrentStep -= NLINES/2;
			// Limit Movement
			if(TopStep < 0) {
				TopStep = 0;
				CurrentStep = 0;
				HiLghtdLine = 0;
			}
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_U)
		{
			if(HiLghtdLine > 0){
				HiLghtdLine--;
			} else {
				TopStep--;
			}
			CurrentStep--;
			// Limit Movement
			if(TopStep < 0)
				TopStep = 0;
			if(CurrentStep < 0)
				CurrentStep = 0;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_S)
		{			
			// This spawns the group, if possible
			conn.GetChecklistItem(CurrentStep, 0);

			screen = PROG_CHKLST;
			CurrentStep = 0;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_R)
		{
		
			// TODO SelectedGroup would be the checklist to be reviewed
			//SelectedGroup = CurrentStep;

			screen = PROG_CHKLSTREV;
			CurrentStep = 0;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_D)
		{
			if (!HiLghtdLineDown) {
				HiLghtdLine++;
			} else {
				TopStep++;
			}
			CurrentStep++;
			if (CurrentStep > NumChkLsts - 1) {
				CurrentStep--;
				if (!HiLghtdLineDown) {
					HiLghtdLine--;
				} else {
					TopStep--;
				}
			}
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_NEXT)
		{
			TopStep = TopStep + NLINES/2;
			CurrentStep = CurrentStep + NLINES/2;
			if (CurrentStep >= NumChkLsts) {
				CurrentStep = NumChkLsts - 1;
				HiLghtdLine = CurrentStep - TopStep;
			}
			if (TopStep >= NumChkLsts) {
				TopStep = NumChkLsts - 1;
				HiLghtdLine = 0;
			}
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_L)
		{
			bool fl = conn.GetChecklistFlashing();
			conn.SetChecklistFlashing(!fl);
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_M)
		{
			if (bDisplayMET)
			{
				bDisplayMET = false;
			}
			else
			{
				bDisplayMET = true;
			}
			InvalidateDisplay();
			return true;
		}
	}
	else if (screen == PROG_CHKLSTREV)
	{
		if (key == OAPI_KEY_C)
		{
			screen = PROG_CHKLSTNAV;
			CurrentStep = 0;
			TopStep = 0;
			HiLghtdLine = 0;

			//SelectedGroup = -1;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_N)
		{
			screen = PROG_CHKLSTINFO;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_E)
		{
			conn.SetAutoExecute(!conn.GetAutoExecute());
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_A)
		{
			ChkLstAutoOn = conn.ChecklistAutocomplete();
			if (conn.ChecklistAutocomplete(!(ChkLstAutoOn)))
				ChkLstAutoOn = !(ChkLstAutoOn);
			
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_PRIOR)
		{
			CurrentStep -= 6;
			if (CurrentStep < 0)
				CurrentStep = 0;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_U)
		{
			CurrentStep--;
			if (CurrentStep < 0)
				CurrentStep = 0;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_D)
		{
			CurrentStep++;
			//TODO: Prevent overrunning max number of items
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_NEXT)
		{
			CurrentStep += 9;
			//TODO: Prevent overrunning max number of items
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_M)
		{
			if (bDisplayMET)
			{
				bDisplayMET = false;
			}
			else
			{
				bDisplayMET = true;
			}
			InvalidateDisplay();
			return true;
		}
	}
	else if (screen == PROG_CHKLSTINFO)
	{
		if (key == OAPI_KEY_B)
		{
			screen = PROG_CHKLST;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}

		/*
		Disabled for now

		if (key == OAPI_KEY_F)
		{
			item = conn.GetChecklistItem(-1, CurrentStep);
			if (item)
				conn.SetFlashing(item->item, !conn.GetFlashing(item->item));	// TODO DSKY
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		*/

		if (key == OAPI_KEY_M)
		{
			if (bDisplayMET)
			{
				bDisplayMET = false;
			}
			else
			{
				bDisplayMET = true;
			}
			InvalidateDisplay();
			return true;
		}
	}

	return false;
}

// Find variables to replace in the given buffer
void ProjectApolloChecklistMFD::substituteVariables(char *buffer,int buflen) {
	if (buflen > 1000) { buflen = 1000; } // Don't overflow internal edit buffer
	if (strstr(buffer, "$") != NULL) {
		char expansion[32];
		char * destination_pos = 0;
		int destination_index = 0;
		int expansion_length = 0;
		int working_index = 0;
		// Obtain location of first variable to be substituted
		destination_pos = strchr(buffer, '$');
		while (destination_pos != NULL) {
			char variable_index_string[4];
			int variable_index = 0;
			// Ensure clobber
			variable_index_string[0] = 0;
			// Determine indexes
			destination_index = (destination_pos - buffer);
			working_index = destination_index + 1;
			// Find parameter ID
			while (working_index < (int)strlen(buffer) && buffer[working_index] >= '0' && buffer[working_index] <= '9' && (working_index - (destination_index + 1)) < 3) {
				variable_index_string[working_index - (destination_index + 1)] = buffer[working_index];
				working_index++;
			}
			variable_index_string[working_index - (destination_index + 1)] = 0; // Terminate
			variable_index = atoi(variable_index_string);
			if (variable_index > 15) { variable_index = 15; }
			if (variable_index < 0) { variable_index = 0; }
			// sprintf(oapiDebugString(), "VARIABLE-INDEX-STRING %s, VARIABLE-INDEX %d", variable_index_string,variable_index);
			// Find what we are substituting for this
			if (saturn) {
				if (saturn->Checklist_Variable[variable_index][0] != 0) {
					strncpy(expansion, saturn->Checklist_Variable[variable_index], 32);
				} else {
					expansion[0] = 0;
				}
			} else {
				if (lem) {
					if (lem->Checklist_Variable[variable_index][0] != 0) {
						strncpy(expansion, lem->Checklist_Variable[variable_index], 32);
					} else {
						expansion[0] = 0;
					}
				} else {
					expansion[0] = 0; // Shouldn't be able to get here, the crawler can't.
				}
			}
			// If it's emptystring, replace that with an ellipsis.
			if (expansion[0] == 0) {
				strncpy(expansion, "...", 32);
			}
			// Now do the substitution
			expansion_length = (int)strlen(expansion);
			// if (expansion_length > 0) { expansion_length -= 1;  } // Don't count terminating null
			// strncpy on Windows can't overwrite itself, so we have to use a temporary buffer.
			char tmp[1000];
			strncpy(tmp, buffer + working_index, buflen);
			strncpy(buffer + (destination_index + expansion_length), tmp, buflen - (destination_index + expansion_length));
			working_index = destination_index;
			while (working_index < (destination_index + expansion_length)) {
				buffer[working_index] = expansion[working_index - destination_index];
				working_index++;
			}
			// Find next variable to be substituted
			destination_pos = strchr((buffer + working_index), '$');
		}
	}
}

bool ProjectApolloChecklistMFD::Update (oapi::Sketchpad* skp)
{
	ChecklistItem *item;
	oapi::Brush *hBr;

	SURFHANDLE s = skp->GetSurface();
	RECT src{ 0, 0, 255, 255 };
	RECT dst{ 1, 1, (LONG)width - 2, (LONG)height - 2 };
	oapiBlt(s, hLogo, &dst, &src);

	// Retrieve Checklists
	vector<ChecklistGroup> *temp = conn.GetChecklistList();
	if (temp) {
		groups = *temp;
		NumChkLsts = groups.size();
	}

	if (screen == PROG_CHKLST)
	{
		skp->SetFont(GetDefaultFont(0));
		char *cn = conn.checklistName();
		if (cn)
			Title(skp, cn);
		
		// Display the MET after the header
		if (bDisplayMET)
		{
			skp->SetTextColor(RGB(225, 225, 255)); // blue 
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			line = DisplayMissionElapsedTime();
			skp->Text((int)(width * .5), (int)(height * 0.05), line.c_str(), line.size());
		}

		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->SetPen(GetDefaultPen(0));
		skp->MoveTo((int)(width * 0.02), (int)(height * 0.94));
		skp->LineTo((int)(width * 0.98), (int)(height * 0.94));

		// Display bottom flag row
		bool autocomplete = conn.ChecklistAutocomplete();
		bool autoexec = conn.GetAutoExecute();
		bool flash = conn.GetChecklistFlashing();
		skp->SetTextColor(RGB(0, 255, 0));
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text((int)(width * 0.05), (int)(height * .95), autocomplete ? "AUTO: ON " : "AUTO: OFF", strlen("AUTO: OFF"));
		skp->SetTextAlign(oapi::Sketchpad::CENTER);
		skp->Text(width / 2, (int)(height * .95), autoexec ? "EXEC: ON " : "EXEC: OFF", strlen("EXEC: OFF"));
		skp->SetTextAlign(oapi::Sketchpad::RIGHT);
		skp->Text(width - (int)(width * 0.05), (int)(height * .95), flash ? "FLASH: ON " : "FLASH: OFF", strlen("FLASH: OFF"));

		//Retrieve 15 visible checklist steps  (all are displayed on two lines)
		//Lines 1,2,3,4,5,6,7,8,9 (index 0/1,2/3,4/5,6/7,8/9,10/11,12/13,14/15,16/17)
		bool noChecklist = true;
		int StepCnt = 0;
		bool linebreakpanel = false;
		for (cnt = 0 ; cnt < 20 ; cnt++) {
			item = conn.GetChecklistItem(-1, CurrentStep + StepCnt);
			if (item) {
				noChecklist = false;
				//Heading 1
				if (strlen(item->heading1) > 0) {
					if (StepCnt != 0)
						cnt++; //go to next line
					if (cnt >= 20) break;
					skp->SetTextColor(RGB(225, 225, 255));
					skp->SetTextAlign(oapi::Sketchpad::CENTER);
					skp->Text((int)(width * .5), (int)(height * (LINE0 + cnt * HLINE)), item->heading1, strlen(item->heading1));
					cnt++;
					cnt++; //go to next line to write information
				}
				if (cnt >= 20) break;

				//Check to see if step is time dependent and print time if so
				if (item->relativeEvent != NO_TIME_DEF && item->relativeEvent != HIDDEN_DELAY) {
					if (strlen(item->heading1) == 0 && StepCnt != 0 && !linebreakpanel)
						cnt++; //go to next line
					if (cnt >= 20) break;
					skp->SetTextColor(RGB(225, 225, 255));
					skp->SetTextAlign(oapi::Sketchpad::LEFT);
					//display Current Checklist Mission Time
					line = DisplayChecklistMissionTime(item);
					skp->Text((int)(width * .01), (int)(height * (LINE0 + cnt * HLINE)), line.c_str(), line.size());
					cnt++; //go to next line to write information
				}
				if (cnt >= 20) break;

				//Heading 2
				if (strlen(item->heading2) > 0) {
					if (strlen(item->heading1) == 0 && (item->relativeEvent == NO_TIME_DEF || item->relativeEvent == HIDDEN_DELAY) && StepCnt != 0 && !linebreakpanel)
						cnt++; //go to next line
					if (cnt >= 20) break;
					skp->SetTextColor(RGB(225, 225, 255));
					skp->SetTextAlign(oapi::Sketchpad::LEFT);
					skp->Text((int)(width * .03), (int)(height * (LINE0 + cnt * HLINE)), item->heading2, strlen(item->heading2));
					cnt++; //go to next line to write information
				}
				if (cnt >= 20) break;

				//Line feed
				if (item->lineFeed)
					cnt++;
				if (cnt >= 20) break;

				//Display Highlighted box
				if (StepCnt == 0 || StepCnt == -CurrentStep) {
					if (StepCnt == -CurrentStep) {
						hBr = oapiCreateBrush(RGB(0, 100, 0));
					} else {
						hBr = oapiCreateBrush(RGB(0, 50, 0));
					}
					oapi::Brush *oldBrush = skp->SetBrush(hBr);
					oapi::Pen* oldPen = skp->SetPen(nullptr);
					skp->Rectangle((int)(width * .01), (int)(height * (LINE0 + cnt * HLINE + .008)), (int)(width * .99), (int)(height * (LINE1 + cnt * HLINE + .01)));
					skp->SetBrush(oldBrush);
					skp->SetPen(oldPen);
					oapiReleaseBrush(hBr);
				}

				//Print Step
				if (item->callGroup == -1) {
					if (item->automatic) {
						skp->SetTextColor(RGB(0, 255, 0));
					} else {
						skp->SetTextColor(RGB(255, 255, 0));
					}
				} else {
					skp->SetTextColor(RGB(0, 255, 255));
				}
				skp->SetTextAlign(oapi::Sketchpad::LEFT);

				// Line break
				char buffer[1000];
				strcpy(buffer, item->text);
				this->substituteVariables(buffer, 1000);
				if (strlen(buffer) > 34) {
					while (strlen(buffer) > 34) {
						int i, breakpos = 0;
						for (i = 33; i >= 0; i--) {
							if (buffer[i] == ' ') {
								breakpos = i;
								break;
							}
						}
						if (breakpos == 0) breakpos = 30;

						skp->Text((int)(width * .05), (int)(height * (LINE0 + cnt * HLINE)), buffer, breakpos);
						cnt++; //go to next line
						if (cnt >= 20) break;

						char tmp[1000];
						tmp[0] = '\0';
						strcat(tmp, "    ");
						strcat(tmp, buffer + breakpos + 1);
						strcpy(buffer, tmp);
					}
					if (cnt >= 20) break;
					skp->Text((int)(width * .05), (int)(height * (LINE0 + cnt * HLINE)), buffer, strlen(buffer));

				} else {
					skp->Text((int)(width * .05), (int)(height * (LINE0 + cnt * HLINE)), buffer, strlen(buffer));
				}

				//Print panel
				linebreakpanel = false;
				if (strlen(item->panel) > 0) {
					if (strlen(item->panel) + strlen(buffer) > 33) {
						cnt++; //go to next line
						if (cnt >= 20) break;
						linebreakpanel = true;
					}
					skp->SetTextAlign(oapi::Sketchpad::RIGHT);
					skp->Text((int)(width * .98), (int)(height* (LINE0 + cnt * HLINE)), item->panel, strlen(item->panel));
				}
			} else {
				if (StepCnt == 0 && CurrentStep != 0) {
					CurrentStep = 0;
					StepCnt--;
					cnt--;
				}
			}
			StepCnt++;
		}
		if (noChecklist) {
			skp->SetTextColor(RGB(0, 255, 0));
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			skp->Text((int)(width * .5), (int)(height * .4), "(No active checklist)", 21);
		}
	}
	else if (screen == PROG_CHKLSTNAV)
	{
		skp->SetFont(GetDefaultFont(0));
		Title(skp, "Checklist Navigation");

		// Display the MET after the header
		if (bDisplayMET)
		{
			skp->SetTextColor(RGB(225, 225, 255)); // blue 
			skp->SetTextAlign(oapi::Sketchpad::CENTER);
			line = DisplayMissionElapsedTime();
			skp->Text((int)(width * .5), (int)(height * 0.05), line.c_str(), line.size());
		}

		skp->SetPen(GetDefaultPen(0));
		skp->MoveTo((int)(width * 0.02), (int)(height * 0.94));
		skp->LineTo((int)(width * 0.98), (int)(height * 0.94));

		//display AutoToggle selection box.
		hBr = oapiCreateBrush(RGB(0, 150, 0));
		oapi::Brush* oldBrush = skp->SetBrush(hBr);
		oapi::Pen* oldPen = skp->SetPen(nullptr);
		if (!conn.ChecklistAutocomplete()) {
			skp->Rectangle((int)(width * 0.34), (int)(height * 0.955), (int)(width * 0.47), (int)(height - .999));
		} else {
			skp->Rectangle((int)(width * 0.24), (int)(height * 0.955), (int)(width * 0.34), (int)(height - .999));
		}
		skp->SetBrush(oldBrush);
		skp->SetPen(oldPen);
		skp->SetTextColor(RGB(0, 255, 0));
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text((int)(width * .05), (int)(height * .95), " AUTO:  ON  OFF", 15);

		//display flashing selection box.
		oldBrush = skp->SetBrush(hBr);
		oldPen = skp->SetPen(nullptr);
		if (!conn.GetChecklistFlashing()) {
			skp->Rectangle((int) (width * 0.82),(int) (height * 0.955),(int) (width * 0.95), (int) (height - .999));
		} else {
			skp->Rectangle((int) (width * 0.72),(int) (height * 0.955),(int) (width * 0.82), (int) (height - .999));
		}
		skp->SetBrush(oldBrush);
		skp->SetPen(oldPen);
		oapiReleaseBrush(hBr);
		skp->SetTextColor(RGB(0, 255, 0));
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		skp->Text((int)(width * .5), (int)(height * .95), " FLASH:  ON  OFF", 16);

		//TODO: Handle Writing Text
		/*
		sprintf(buffer, "%d", groups.size());
		line = buffer;
		line = line.append(" checklists found.");
		TextOut(hDC, (int) (width * .10), (int) (height * .05), line.c_str(), line.size());
		*/

		//sprintf(oapiDebugString(), "TopStep: %d  CurrentStep: %d  HighLightedStep: %d", TopStep,CurrentStep,HiLghtdLine);

		//Set up Type for checklist display
		skp->SetTextAlign(oapi::Sketchpad::LEFT);
		int cnt, grpcnt = 0;

		// Make sure that the HiLghtdLine is shown actually
		bool hiLghtdLineShown = false, redo = true;
		while (redo) {
			grpcnt = 0;
			for (cnt = 0; cnt < NLINES; cnt++) {
				if (grpcnt + TopStep >= NumChkLsts) break;
				ChecklistGroup *grp = &(groups[grpcnt + TopStep]);

				//Heading
				if (strlen(grp->heading) > 0) {
					if (grpcnt != 0)
						cnt++; //go to next line
					if (cnt >= NLINES) break;
					cnt++;
					cnt++; //go to next line to write information
				}
				if (cnt >= NLINES) break;

				//display Highlighted box
				if (grpcnt == HiLghtdLine) {
					hiLghtdLineShown = true;
				}
				grpcnt++;
			}
			if (!hiLghtdLineShown) {
				TopStep++;
				HiLghtdLine--;
			} else {
				redo = false;
			}
		}

		//
		// Following is the display loop:
		//		Compares Lines displayed (NumChkLsts - TopStep) to Lines able to display (NLINES)
		//		
		grpcnt = 0;
		for (cnt = 0; cnt < NLINES; cnt++) {
			if (grpcnt + TopStep >= NumChkLsts) break;
			ChecklistGroup *grp = &(groups[grpcnt + TopStep]);

			//Heading
			if (strlen(grp->heading) > 0) {
				if (grpcnt != 0)
					cnt++; //go to next line
				if (cnt >= NLINES) break;
				skp->SetTextColor(RGB(225, 225, 255));
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
				skp->Text((int)(width * .5), (int)(height * (LINE0 + cnt * HLINE)), grp->heading, strlen(grp->heading));
				cnt++;
				cnt++; //go to next line to write information
			}
			if (cnt >= NLINES) break;

			//display Highlighted box
			if (grpcnt == HiLghtdLine) {
				hBr = oapiCreateBrush(RGB(0, 100, 0));
				oapi::Brush* oldBrush = skp->SetBrush(hBr);
				oapi::Pen* oldPen = skp->SetPen(nullptr);
				skp->Rectangle((int)(width * .01), (int)(height * (LINE0 + cnt * HLINE + .01)), (int)(width * .99), (int)(height * (LINE1 + cnt * HLINE + .01)));
				skp->SetBrush(oldBrush);
				skp->SetPen(oldPen);
				oapiReleaseBrush(hBr);
				HiLghtdLineDown = true;
			} else {
				HiLghtdLineDown = false;
			}

			skp->SetTextColor(RGB(0, 255, 0));
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			skp->Text((int)(width * .05), (int)(height* (LINE0 + (cnt * HLINE))), grp->name, strlen(grp->name));

			grpcnt++;
		}
	}
	else if (screen == PROG_CHKLSTREV)
	{
		/*
		SelectDefaultFont(hDC, 0);
		Title(hDC, conn.checklistName());

		SetTextAlign (hDC, TA_LEFT);

		SelectDefaultPen(hDC, 1);
		MoveToEx (hDC, (int) (width * 0.02), (int) (height * 0.95), 0);
		LineTo (hDC, (int) (width * 0.98), (int) (height * 0.95));

		//display AutoToggle selection box.
		hBr = CreateSolidBrush( RGB(0, 150, 0));
		if (conn.ChecklistAutocomplete(ChkLstAutoOn)){
			SetRect(&ShadedBox,(int) (width * 0.35),(int) (height * 0.96),(int) (width * 0.47),height-1);
			ChkLstAutoOn = true;
		}else{
			SetRect(&ShadedBox,(int) (width * 0.25),(int) (height * 0.96),(int) (width * 0.34),height-1);
			ChkLstAutoOn = false;
		}
		FillRect(hDC, &ShadedBox, hBr);
		SetTextColor (hDC, RGB(0, 255, 0));
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * .05), (int) (height * .95), " AUTO:  ON  OFF", 15);

		//display Highlighted box
		hBr = CreateSolidBrush( RGB(0, 100, 0));
		SetRect(&ShadedBox,(int) (width * .05),(int) (height * (LINE0 + .01)),(int) (width * .95), (int) (height * (LINE2 + .01)));
		FillRect(hDC, &ShadedBox, hBr);

		//Retrieve 9 visible checklist steps  (all are displayed on two lines)

		//Lines 1,2,3,4,5,6,7,8,9 (index 0/1,2/3,4/5,6/7,8/9,10/11,12/13,14/15,16/17)
		for (cnt = 0 ; cnt < 9 ; cnt++) {
			item.group = -1;
			item.index = CurrentStep + cnt;
			if (item.index >= 0){
				if (conn.GetChecklistItem(&item)){
										
					SetTextAlign (hDC, TA_CENTER);
					sprintf(buffer, "%d", item.index);
					line = buffer;
					TextOut(hDC, (int) (width * .05), (int) (height * (LINE0+cnt*2*HLINE+HLINE/2)), line.c_str(), line.size());
					SetTextAlign (hDC, TA_LEFT);
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE0+cnt*2*HLINE)), item.text, strlen(item.text));
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE1+cnt*2*HLINE)), item.info, strlen(item.info));
				}
			}
		}
		*/
	}
	else if (screen == PROG_CHKLSTINFO)
	{
		item = conn.GetChecklistItem(-1, CurrentStep);
		if (item) {
			skp->SetFont(GetDefaultFont(0));
			Title(skp, "Checklist Information");
			// Display the MET after the header
			if (bDisplayMET) {
				skp->SetTextColor(RGB(225, 225, 255)); // blue 
				skp->SetTextAlign(oapi::Sketchpad::CENTER);
				line = DisplayMissionElapsedTime();
				skp->Text((int)(width * .5), (int)(height * 0.05), line.c_str(), line.size());
			}
			cnt = 1;
			skp->SetTextColor(RGB(225, 225, 255));
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			//display Current Checklist Mission Time
			if (item->relativeEvent != NO_TIME_DEF && item->relativeEvent != HIDDEN_DELAY) {
				line = DisplayChecklistMissionTime(item);
				skp->Text((int)(width * .02), (int)(height* (LINE0 + cnt * HLINE)), line.c_str(), line.size());
				cnt = cnt + 2;
			}
			//display Highlighted box
			hBr = oapiCreateBrush(RGB(0, 100, 0));
			oapi::Brush* oldBrush = skp->SetBrush(hBr);
			oapi::Pen* oldPen = skp->SetPen(nullptr);
			skp->Rectangle((int)(width * .01), (int)(height* (LINE0 + cnt * HLINE + .01)), (int)(width * .99), (int)(height* (LINE0 + (cnt + 1) * HLINE + .01)));
			skp->SetBrush(oldBrush);
			skp->SetPen(oldPen);
			oapiReleaseBrush(hBr);
			skp->SetPen(GetDefaultPen(0));
			skp->MoveTo((int)(width * 0.02), (int)(height * 0.94));
			skp->LineTo((int)(width * 0.98), (int)(height * 0.94));
			skp->SetFont(GetDefaultFont(0));
			skp->SetTextColor(RGB(0, 255, 0));
			skp->SetTextAlign(oapi::Sketchpad::LEFT);

			// Item
			char buffer[1000];
			strcpy(buffer, item->text);
			this->substituteVariables(buffer, 1000);
			if (strlen(buffer) > 34) {
				while (strlen(buffer) > 34) {
					int i, breakpos = 0;
					for (i = 33; i >= 0; i--) {
						if (buffer[i] == ' ') {
							breakpos = i;
							break;
						}
					}
					if (breakpos == 0) breakpos = 30;

					skp->Text((int)(width * .05), (int)(height* (LINE0 + cnt * HLINE)), buffer, breakpos);
					cnt++; //go to next line

					char tmp[1000];
					tmp[0] = '\0';
					strcat(tmp, "    ");
					strcat(tmp, buffer + breakpos + 1);
					strcpy(buffer, tmp);
				}
				skp->Text((int)(width * .05), (int)(height* (LINE0 + cnt * HLINE)), buffer, strlen(buffer));
			} else {
				skp->Text((int)(width * .05), (int)(height* (LINE0 + cnt * HLINE)), buffer, strlen(buffer));
			}
			// Line break
			cnt = cnt + 2;

			// Info
			skp->SetTextColor(RGB(0, 225, 0));
			skp->SetTextAlign(oapi::Sketchpad::LEFT);
			strcpy(buffer, item->info);
			// Variable substitution comes first, so we can include the variable expansion in line length calculations
			this->substituteVariables(buffer,1000);
			// Now output is
			if (strlen(buffer) > 34 || strstr(buffer, "\n") != NULL) {
				char * nlp = strchr(buffer, '\n');
				while (strlen(buffer) > 34 || nlp != NULL) {
					int i, breakpos = 0;
					for (i = 33; i >= 0; i--) {						
						if (i > (int)strlen(buffer)) { i = (int)strlen(buffer); } // Don't fall off end if we're just handling a newline
						// Which mode are we in?
						if (nlp != NULL) {
							breakpos = (nlp - (char *)&buffer);
							if (breakpos <= 34) { // Do we still need to wrap?
								break; // No, we're done here.
							}
						}
						if (buffer[i] == ' ') {
							breakpos = i;
							break;
						}
					}
					if (breakpos == 0) breakpos = 30;

					skp->Text((int)(width * .05), (int)(height* (LINE0 + cnt * HLINE)), buffer, breakpos);
					cnt++; //go to next line

					char tmp[1000];
					strcpy(tmp, buffer + breakpos + 1);
					strcpy(buffer, tmp);
					nlp = strchr(buffer, '\n');
				}
				skp->Text((int)(width * .05), (int)(height * (LINE0 + cnt * HLINE)), buffer, strlen(buffer));

			} else {
				skp->Text((int)(width * .05), (int)(height * (LINE0 + cnt * HLINE)), buffer, strlen(buffer));
			}

			// Line break
			cnt = cnt + 2;

			// Panel
			if (strlen(item->panel) > 0) {
				sprintf(buffer, "Panel: %s", item->panel);

				skp->SetTextColor(RGB(0, 225, 0));
				skp->SetTextAlign(oapi::Sketchpad::RIGHT);
				skp->Text((int)(width * .98), (int)(height * (LINE0 + cnt * HLINE)), buffer, strlen(buffer));
				cnt = cnt + 2;
			}
		}
	}
	return true;
}
std::string ProjectApolloChecklistMFD::DisplayChecklistMissionTime(ChecklistItem *item)
{
	char buffer[100];

	temptime.x = 0;
	temptime.y = 0;
	temptime.z = 0;

	switch (item->relativeEvent) {
		case MISSION_TIME:
			temptime.x = floor(fabs(item->time)/3600);
			temptime.y = floor((fabs(item->time)-(temptime.x*3600))/60);
			temptime.z = fabs(item->time)-(temptime.x*3600)-(temptime.y*60);
			if (item->time >= 0.0) {
				sprintf(buffer, "T+%d:%02d:%02d",(int) temptime.x,(int) temptime.y,(int) temptime.z);
			} else {
				sprintf(buffer, "T-%d:%02d:%02d",(int) temptime.x,(int) temptime.y,(int) temptime.z);
			}
			break;

		case LAST_ITEM_RELATIVE:
			temptime.y = floor((fabs(item->time)-(temptime.x*3600))/60);
			temptime.z = fabs(item->time)-(temptime.x*3600)-(temptime.y*60);
			if (temptime.y >= 1.0) {
				if (((int) temptime.z) == 0) {
					sprintf(buffer, "After %d min", (int) temptime.y);
				} else {
					sprintf(buffer, "After %d min %02d sec", (int) temptime.y, (int) temptime.z);
				}
			} else {
				sprintf(buffer, "After %d sec", (int) temptime.z);
			}
			break;

		case CHECKLIST_RELATIVE:
			temptime.y = floor((fabs(item->time) - (temptime.x * 3600)) / 60);
			temptime.z = fabs(item->time) - (temptime.x * 3600) - (temptime.y * 60);
			if (temptime.y >= 1.0) {
				if (((int)temptime.z) == 0) {
					sprintf(buffer, "After %d min", (int)temptime.y);
				}
				else {
					sprintf(buffer, "After %d min %02d sec", (int)temptime.y, (int)temptime.z);
				}
			}
			else {
				sprintf(buffer, "After %d sec", (int)temptime.z);
			}
			break;

		case SECOND_STAGE_STAGING:
			sprintf(buffer, "First Staging");
			break;

		case SIVB_STAGE_STAGING:
			sprintf(buffer, "Second Staging");
			break;

		case EARTH_ORBIT_INSERTION:
			sprintf(buffer, "Earth Orbit Insertion");
			break;
		case CSM_LV_SEPARATION:
		case CSM_LV_SEPARATION_DONE:
			sprintf(buffer, "CSM/LV Separation");
			break;
		case TLI:
			temptime.y = floor((fabs(item->time) - (temptime.x * 3600)) / 60);
			temptime.z = fabs(item->time) - (temptime.x * 3600) - (temptime.y * 60);
			if (temptime.y >= 1.0) {
				if (((int)temptime.z) == 0) {
					sprintf(buffer, "TB6 + %d min", (int)temptime.y);
				}
				else {
					sprintf(buffer, "TB6 + %d min %02d sec", (int)temptime.y, (int)temptime.z);
				}
			}
			else {
				sprintf(buffer, "TB6 + %d sec", (int)temptime.z);
			}
			break;
		case TLI_DONE:
			temptime.x = floor(fabs(item->time) / 3600);
			temptime.y = floor((fabs(item->time) - (temptime.x * 3600)) / 60);
			temptime.z = fabs(item->time) - (temptime.x * 3600) - (temptime.y * 60);
			if (temptime.x >= 1.0)
			{
				if (((int)temptime.y) == 0) {
					if (((int)temptime.z) == 0) {
						sprintf(buffer, "TLI + %d hrs", (int)temptime.x);
					}
					else
					{
						sprintf(buffer, "TLI + %d hrs %02d sec", (int)temptime.x, (int)temptime.z);
					}
				}
				else
				{
					if (((int)temptime.z) == 0) {
						sprintf(buffer, "TLI + %d hrs %02d min", (int)temptime.x, (int)temptime.z);
					}
					else
					{
						sprintf(buffer, "TLI + %d hrs %02d min %02d sec", (int)temptime.x, (int)temptime.y, (int)temptime.z);
					}
				}
			}
			else if (temptime.y >= 1.0) {
				if (((int)temptime.z) == 0) {
					sprintf(buffer, "TLI + %d min", (int)temptime.y);
				}
				else {
					sprintf(buffer, "TLI + %d min %02d sec", (int)temptime.y, (int)temptime.z);
				}
			}
			else {
				sprintf(buffer, "TLI + %d sec", (int)temptime.z);
			}
			break;
		case CM_SM_SEPARATION_DONE:
			temptime.y = floor((fabs(item->time) - (temptime.x * 3600)) / 60);
			temptime.z = fabs(item->time) - (temptime.x * 3600) - (temptime.y * 60);
			if (temptime.y >= 1.0) {
				if (((int)temptime.z) == 0) {
					sprintf(buffer, "CM/SM Separation + %d min", (int)temptime.y);
				}
				else {
					sprintf(buffer, "CM/SM Separation + %d min %02d sec", (int)temptime.y, (int)temptime.z);
				}
			}
			else {
				sprintf(buffer, "CM/SM Separation + %d sec", (int)temptime.z);
			}
			break;
		case SPLASHDOWN:
			temptime.y = floor((fabs(item->time) - (temptime.x * 3600)) / 60);
			temptime.z = fabs(item->time) - (temptime.x * 3600) - (temptime.y * 60);
			if (temptime.y >= 1.0) {
				if (((int)temptime.z) == 0) {
					sprintf(buffer, "Splashdown + %d min", (int)temptime.y);
				}
				else {
					sprintf(buffer, "Splashdown + %d min %02d sec", (int)temptime.y, (int)temptime.z);
				}
			}
			else {
				sprintf(buffer, "Splashdown + %d sec", (int)temptime.z);
			}
			break;
		default:
			sprintf(buffer, "(Unknown Event)");
			break;
	}
	std::string line = buffer;
	return line;

}
void ProjectApolloChecklistMFD::WriteStatus (FILEHANDLE scn) const
{
}
void ProjectApolloChecklistMFD::ReadStatus (FILEHANDLE scn)
{
}
void ProjectApolloChecklistMFD::StoreStatus (void) const
{
}
void ProjectApolloChecklistMFD::RecallStatus (void)
{
}
	
OAPI_MSGTYPE ProjectApolloChecklistMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (OAPI_MSGTYPE)(new ProjectApolloChecklistMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}
// Display MET depending if saturn, crawler or Lem
std::string ProjectApolloChecklistMFD::DisplayMissionElapsedTime (void)
{
	char buffer[20];
	double mt = 0;

	if (mcc)
	{
		mt = mcc->GetMissionTime();
	}

	if (mt <= 0.0) //Mission time from MCC might be nonsense before liftoff
	{
		if (saturn) { mt = saturn->GetMissionTime(); }
		if (crawler) { mt = crawler->GetMissionTime(); }
		if (lem) { mt = lem->GetMissionTime(); }
	}

	int secs = abs((int) mt);
	int hours = (secs / 3600);
	secs -= (hours * 3600);
	int minutes = (secs / 60);
	secs -= 60 * minutes;
	if (mt < 0)
		sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
	else
		sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);
	
	std::string line = buffer;
	return line;
}
