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

//#include "MFDResource.h"
#include "ProjectApolloChecklistMFD.h"

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
	return 0;
}
int ProjectApolloChecklistMFD::ButtonMenu (const MFDBUTTONMENU **menu) const
{
	menu = 0;
	return 0;
}
bool ProjectApolloChecklistMFD::ConsumeButton (int bt, int event)
{
	if (!(event & PANEL_MOUSE_LBDOWN)) return false;
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
	return false;
}
bool ProjectApolloChecklistMFD::ConsumeKeyBuffered (DWORD key)
{
	return false;
}
void ProjectApolloChecklistMFD::Update (HDC hDC)
{
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

	if (groups.size() > 0)
		TextOut(hDC, 0, (int)(height*0.4), groups[0].name,strlen(groups[0].name));
	if (groups.size() > 1)
		TextOut(hDC, 0, (int)(height*0.45), groups[1].name,strlen(groups[0].name));
	if (groups.size() > 2)
		TextOut(hDC, 0, (int)(height*0.5), groups[2].name,strlen(groups[0].name));
	if (groups.size() > 3)
		TextOut(hDC, 0, (int)(height*0.55), groups[3].name,strlen(groups[0].name));
	if (groups.size() > 4)
		TextOut(hDC, 0, (int)(height*0.6), groups[4].name,strlen(groups[0].name));
	if (groups.size() > 5)
		TextOut(hDC, 0, (int)(height*0.65), groups[5].name,strlen(groups[0].name));
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