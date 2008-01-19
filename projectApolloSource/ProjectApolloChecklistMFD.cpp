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
#include <string>

//#include "MFDResource.h"
#include "ProjectApolloChecklistMFD.h"

#define PROG_CHKLSTNAV		0
#define PROG_CHKLST			1
#define PROG_CHKLSTINFO		2

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

int hMFD;
void ProjectApolloChecklistMFDopcDLLInit (HINSTANCE hDLL)
{
	static char* name = "Project Apollo Checklist";
	MFDMODESPEC spec;
	spec.key = OAPI_KEY_C;
	spec.name = name;
	spec.msgproc = ProjectApolloChecklistMFD::MsgProc;
	hMFD = oapiRegisterMFDMode(spec);
}
void ProjectApolloChecklistMFDopcDLLExit (HINSTANCE hDLL)
{
	oapiUnregisterMFDMode(hMFD);
}
void ProjectApolloChecklistMFDopcTimestep (double simt, double simdt, double mjd)
{
}
ProjectApolloChecklistMFD::ProjectApolloChecklistMFD (DWORD w, DWORD h, VESSEL *vessel) : MFD (w,h,vessel)
{
	conn.ConnectToVessel(vessel);
	width = w;
	height = h;
	screen = PROG_CHKLSTNAV;
	NumChkLsts = 0;
	MFDInit = false;
	CurrentStep = 0;
	TopStep = 0;
	HiLghtdLine = 0;

}
ProjectApolloChecklistMFD::~ProjectApolloChecklistMFD ()
{
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
}
char *ProjectApolloChecklistMFD::ButtonLabel (int bt)
{

	static char *labelCHKLSTNAV[12] = {"INIT","","INFO","","","AUTO","PgUP","UP","SEL","","DN","PgDN"};
	static char *labelCHKLST[12] = {"CHK","","INFO","","","AUTO","PgUP","UP","PRO","FAIL","DN","PgDN"};
	static char *labelCHKLSTINFO[12] = {"BCK","","","","","","","","FLSH","","",""};

	if (screen == PROG_CHKLSTNAV) {
		return (bt < 12 ? labelCHKLSTNAV[bt] : 0);
	}
	else if (screen == PROG_CHKLST) {
		return (bt < 12 ? labelCHKLST[bt] : 0);
	}
	else if (screen == PROG_CHKLSTINFO) {
		return (bt < 12 ? labelCHKLSTINFO[bt] : 0);
	}

	return 0;
}
int ProjectApolloChecklistMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	static const MFDBUTTONMENU mnuCHKLSTNAV[12] = {
		{"Load List of","Available Checklists",'I'},
		{0,0,0},
		{"More Information","About This Checklist",'N'},
		{0,0,0},
		{0,0,0},
		{"Toggle AutoComplete",0,'A'},
		{"Scroll Up","One Page",'<'},
		{"Scroll Up","One Line",'U'},
		{"Select Checklist",0,'S'},
		{0,0,0},
		{"Scroll Down","One Line",'D'},
		{"Scroll Down","One Page",'>'}
	};
	static const MFDBUTTONMENU mnuCHKLST[12] = {
		{"Return to","Checklist Navigation",'C'},
		{0,0,0},
		{"More Information","About This Step",'N'},
		{0,0,0},
		{0,0,0},
		{"Toggle Hints",0,'H'},
		{"Scroll Up","One Page",'<'},
		{"Scroll Up","One Line",'U'},
		{"Step Succeeds",0,'S'},
		{"Step Fails",0,'F'},
		{"Scroll Down","One Line",'D'},
		{"Scroll Down","One Page",'>'}
	};
	static const MFDBUTTONMENU mnuCHKLSTINFO[12] = {
		{"Back to","Checklist",'B'},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{"Toggle Flashing",0,'F'},
		{0,0,0},
		{0,0,0},
		{0,0,0}
	};

	if (screen == PROG_CHKLSTNAV)
	{
		if (menu) *menu = mnuCHKLSTNAV;
		return 12;
	}
	else if (screen == PROG_CHKLST)
	{
		if (menu) *menu = mnuCHKLST;
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
	
	static const DWORD btkeyCHKLSTNAV[12] = { OAPI_KEY_I,0,OAPI_KEY_N,0,0,OAPI_KEY_A,OAPI_KEY_PRIOR,OAPI_KEY_U,OAPI_KEY_S,OAPI_KEY_F,OAPI_KEY_D,OAPI_KEY_NEXT};
	static const DWORD btkeyCHKLST[12] = { OAPI_KEY_C,0,OAPI_KEY_N,0,0,OAPI_KEY_A,OAPI_KEY_PRIOR,OAPI_KEY_U,OAPI_KEY_S,OAPI_KEY_F,OAPI_KEY_D,OAPI_KEY_NEXT};
	static const DWORD btkeyCHKLSTINFO[12] = { OAPI_KEY_B,0,0,0,0,0,0,0,OAPI_KEY_F,0,0,0};

	if (screen == PROG_CHKLSTNAV)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyCHKLSTNAV[bt]);
	}
	else if (screen == PROG_CHKLST)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyCHKLST[bt]);
	}
	else if (screen == PROG_CHKLSTINFO)
	{
		if (bt < 12) return ConsumeKeyBuffered (btkeyCHKLSTINFO[bt]);
	}
	
	return false;
}
bool ProjectApolloChecklistMFD::ConsumeKeyBuffered (DWORD key)
{
	/*
	// Starts the group 0 (which is right now a testing group)
	if (bt == 0)
	{
		item.group = item.index = -1;
		item.group = 0;
		if (conn.GetChecklistItem(&item))
			return true;
		return true;
	}
	// Fails the current checklist item if possible.
	if (bt == 1)
	{
		item.group = item.index = -1;
		if (conn.GetChecklistItem(&item))
			conn.failChecklistItem(&item);
		return true;
	}
	// Completes the current checklist item if possible.
	if (bt == 2)
	{
		item.group = item.index = -1;
		if (conn.GetChecklistItem(&item))
			conn.completeChecklistItem(&item);
		return true;
	}
	// Gets the current list of available checklists.
	if (bt == 3)
	{
		vector<ChecklistGroup> *temp = conn.GetChecklistList();
		if (temp)
			groups = *temp;
		return true;
	}
	// Gets the current checklist item (not sure why it would be useful here).
	if (bt == 4)
	{
		item.group = item.index = -1;
		if (conn.GetChecklistItem(&item))
			return true;
		return true;
	}
	if (bt == 6)
	{
		item.group = 1;
		item.index = -1;
		if (conn.GetChecklistItem(&item))
			return true;
		return true;
	}
	if (bt == 11)
	{
		item.group = -1;
		item.index = 0;
		if (conn.GetChecklistItem(&item))
			conn.SetFlashing(item.item,true);
		return true;
	}
	*/

	if (screen == PROG_CHKLSTNAV)
	{
		if (key == OAPI_KEY_I)
		{
			screen = PROG_CHKLSTNAV;
			CurrentStep = 0;
			TopStep = 0;
			HiLghtdLine = 0;
			
			//TODO: Reset MFD

			// Get Available Checklists
			vector<ChecklistGroup> *temp = conn.GetChecklistList();
			if (temp) {
				groups = *temp;
				NumChkLsts = groups.size();
			}
			MFDInit = true;
			
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		else if (key == OAPI_KEY_N)
		{
			return true;
		}
		if (key == OAPI_KEY_A)
		{
			ChkLstAutoOn = !(ChkLstAutoOn);
			InvalidateDisplay();
			return true;
		}
		if (key == OAPI_KEY_PRIOR)
		{
			TopStep -= NLINES;
			CurrentStep -= NLINES;
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
			
			SelectedGroup = CurrentStep;
			SelectedGroupName = groups[CurrentStep].name;

			screen = PROG_CHKLST;
			CurrentStep = 0;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_D)
		{
			if (HiLghtdLine < NLINES - 1){
				HiLghtdLine++;
			} else {
				TopStep++;
			}
			CurrentStep++;
			if (CurrentStep > NumChkLsts - 1) {
				CurrentStep--;
				if (HiLghtdLine < NLINES){
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
			TopStep = TopStep + NLINES;
			CurrentStep = CurrentStep + NLINES;
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
	}
	else if (screen == PROG_CHKLST)
	{
		if (key == OAPI_KEY_C)
		{
			screen = PROG_CHKLSTNAV;
			CurrentStep = 0;
			TopStep = 0;
			HiLghtdLine = 0;

			SelectedGroup = -1;

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
		if (key == OAPI_KEY_A)
		{
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
		if (key == OAPI_KEY_S)
		{
			item.group = SelectedGroup;
			item.index = CurrentStep;
			if (conn.GetChecklistItem(&item))
				conn.failChecklistItem(&item);
			return true;

			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
		if (key == OAPI_KEY_F)
		{
			item.group = SelectedGroup;
			item.index = CurrentStep;
			if (conn.GetChecklistItem(&item))
				conn.completeChecklistItem(&item);
			return true;

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
		if (key == OAPI_KEY_F)
		{
			item.group = SelectedGroup;
			item.index = CurrentStep;
			if (conn.GetChecklistItem(&item))
				conn.SetFlashing(item.item,true);
			return true;
			InvalidateDisplay();
			InvalidateButtons();
			return true;
		}
	}

	return false;
}
void ProjectApolloChecklistMFD::Update (HDC hDC)
{
	char buffer[100];

	static RECT ShadedBox;
	HBRUSH hBr;

	if (screen == PROG_CHKLSTNAV)
	{
		SelectDefaultPen(hDC, 1);
		MoveToEx (hDC, (int) (width * 0.05), (int) (height * 0.94), 0);
		LineTo (hDC, (int) (width * 0.95), (int) (height * 0.94));

		//display AutoToggle selection box.
		hBr = CreateSolidBrush( RGB(0, 150, 0));
		if (ChkLstAutoOn){
			SetRect(&ShadedBox,(int) (width * 0.35),(int) (height * 0.96),(int) (width * 0.47),height-1);
		}else{
			SetRect(&ShadedBox,(int) (width * 0.25),(int) (height * 0.96),(int) (width * 0.34),height-1);
		}
		FillRect(hDC, &ShadedBox, hBr);
		SetTextColor (hDC, RGB(0, 255, 0));
		SetTextAlign (hDC, TA_LEFT);
		TextOut(hDC, (int) (width * .05), (int) (height * .95), " AUTO:  ON  OFF", 15);
		
		//display Highlighted box
		hBr = CreateSolidBrush( RGB(0, 100, 0));
		SetRect(&ShadedBox,(int) (width * .05),(int) (height * (LINE0 + HiLghtdLine * HLINE)),(int) (width * .95), (int) (height * (LINE1 + HiLghtdLine * HLINE)));
		FillRect(hDC, &ShadedBox, hBr);

		SelectDefaultFont(hDC, 0);

		//TODO: Handle Writing Text

		sprintf(buffer, "%d", groups.size());
		line = buffer;
		line = line.append(" checklists found.");
		TextOut(hDC, (int) (width * .10), (int) (height * .05), line.c_str(), line.size());
		
		//Set up Type for checklist display
		SelectDefaultFont(hDC, 1);
		SetTextAlign (hDC, TA_LEFT);

		sprintf(oapiDebugString(), "TopStep: %d  CurrentStep: %d  HighLightedStep: %d", TopStep,CurrentStep,HiLghtdLine);
		int cnt;

		//Following is the display loop:
		//		Compares Lines displayed (NumChkLsts - TopStep) to Lines able to display (NLINES)
		//		
		for (cnt = 0; cnt < (((NumChkLsts-TopStep) < NLINES) ? (NumChkLsts-TopStep) : NLINES); cnt++) {

			TextOut(hDC, (int) (width * .05), (int) (height * (LINE0 + (cnt * HLINE))),groups[cnt+TopStep].name,strlen(groups[cnt+TopStep].name));

		}

	}
	else if (screen == PROG_CHKLST)
	{

		SelectDefaultFont(hDC, 0);
		Title(hDC, SelectedGroupName);

		SetTextAlign (hDC, TA_LEFT);

		SelectDefaultPen(hDC, 1);
		MoveToEx (hDC, (int) (width * 0.05), (int) (height * 0.95), 0);
		LineTo (hDC, (int) (width * 0.95), (int) (height * 0.95));

		//display AutoToggle selection box.
		hBr = CreateSolidBrush( RGB(0, 150, 0));
		if (ChkLstAutoOn && conn.ChecklistAutocomplete(ChkLstAutoOn)){
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
		SetRect(&ShadedBox,(int) (width * .05),(int) (height * (LINE7 + .01)),(int) (width * .95), (int) (height * (LINE9 + .01)));
		FillRect(hDC, &ShadedBox, hBr);

		//Retrieve 14 visible checklist steps  (5 are displayed as two lines)

		//Lines 1,2,3 (index 0,1,2)
		for (cnt = 0 ; cnt < 3 ; cnt++) {
			item.group = SelectedGroup;
			item.index = CurrentStep - 5 + cnt;
			if (item.index >= 0){
				if (conn.GetChecklistItem(&item)){
					/*
					//Display Status if available
					hBr = NULL;
					if (item.complete)
                        hBr = CreateSolidBrush( RGB(0, 200, 0));
					else if (item.failed)
						hBr = CreateSolidBrush( RGB(200, 0, 0));
					if (hBr){
						SetRect(&ShadedBox,(int) (width * .025),(int) (height * (LINE0+cnt*HLINE)),(int) (width * .075), (int) (height * (LINE1+cnt*HLINE)));
						FillRect(hDC, &ShadedBox, hBr);
					}
					*/

					SetTextAlign (hDC, TA_CENTER);
					sprintf(buffer, "%d", item.index);
					line = buffer;
					TextOut(hDC, (int) (width * .05), (int) (height * (LINE0+cnt*HLINE)), line.c_str(), line.size());
					SetTextAlign (hDC, TA_LEFT);
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE0+cnt*HLINE)), item.text, strlen(item.text));
				}
			}
		}
		//Lines 4,5,6 (line index 3/4,5/6,7/8
		for (cnt = 3 ; cnt < 6  ; cnt++) {
			item.group = SelectedGroup;
			item.index = CurrentStep - 5 + cnt;
			if (item.index >= 0){
				if (conn.GetChecklistItem(&item)) {
					//TODO:  Write proper format
					SetTextAlign (hDC, TA_CENTER);
					sprintf(buffer, "%d", item.index);
					line = buffer;
					TextOut(hDC, (int) (width * .05), (int) (height * (LINE3+(cnt-3)*2*HLINE+HLINE/2)), line.c_str(), line.size());
					SetTextAlign (hDC, TA_LEFT);
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE3+(cnt-3)*2*HLINE)), item.text, strlen(item.text));
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE4+(cnt-3)*2*HLINE)), item.text, strlen(item.text));
				}
			}
		}
		
		//Lines 7,8,9 (line index 9/10,11/12,13/14)
		for (cnt = 6 ; cnt < 9  ; cnt++) {
			item.group = SelectedGroup;
			item.index = CurrentStep - 5 + cnt;
			if (item.index >= 0){ //TODO: Check to see if we're displaying more than exist
				if (conn.GetChecklistItem(&item)) {
					//TODO:  Write proper format
					SetTextAlign (hDC, TA_CENTER);
					sprintf(buffer, "%d", item.index);
					line = buffer;
					TextOut(hDC, (int) (width * .05), (int) (height * (LINE9+(cnt-6)*2*HLINE+HLINE/2)), line.c_str(), line.size());
					SetTextAlign (hDC, TA_LEFT);
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE9+(cnt-6)*2*HLINE)), item.text, strlen(item.text));
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE10+(cnt-6)*2*HLINE)), item.text, strlen(item.text));
				}
			}
		}
		//Lines 10,11,12,13,14 (line index 15,16,17,18,19)
		for (cnt = 9 ; cnt < 15  ; cnt++) {
			item.group = SelectedGroup;
			item.index = CurrentStep - 5 + cnt;
			if (item.index >= 0){ //TODO: Check to see if we're displaying more than exist
				if (conn.GetChecklistItem(&item)) {
					SetTextAlign (hDC, TA_CENTER);
					sprintf(buffer, "%d", item.index);
					line = buffer;
					TextOut(hDC, (int) (width * .05), (int) (height * (LINE15+(cnt-9)*HLINE)), line.c_str(), line.size());
					SetTextAlign (hDC, TA_LEFT);
					TextOut(hDC, (int) (width * .1), (int) (height * (LINE15+(cnt-9)*HLINE)), item.text, strlen(item.text));
				}
			}
		}
		
	}
	else if (screen == PROG_CHKLSTINFO)
	{
		//TODO: Read and Display Info on current checklist step
	}

	/*
	// An example of how to simply output the text element of the "current" item.  Outputs nothing if no checklists are active yet.
	if ((item.group = item.index = -1,conn.GetChecklistItem(&item)))
		TextOut(hDC, 0,0,item.text,strlen(item.text));
	// Further output the 2nd, 3rd, and 4th items.
	if ((item.group = -1, item.index = 1,conn.GetChecklistItem(&item)))
		TextOut(hDC, 0, (int)(height*0.05), item.text,strlen(item.text));
	if ((item.group = -1, item.index = 2,conn.GetChecklistItem(&item)))
		TextOut(hDC, 0, (int)(height*0.1), item.text,strlen(item.text));
	if ((item.group = -1, item.index = 3,conn.GetChecklistItem(&item)))
		TextOut(hDC, 0, (int)(height*0.15), item.text,strlen(item.text));
	*/
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
	
int ProjectApolloChecklistMFD::MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// Our new MFD mode has been selected, so we create the MFD and
		// return a pointer to it.
		return (int)(new ProjectApolloChecklistMFD (LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}