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
}
ProjectApolloChecklistMFD::~ProjectApolloChecklistMFD ()
{
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
		if (conn.GetChecklistList(&groups))
			return true;
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
	return false;
}
bool ProjectApolloChecklistMFD::ConsumeKeyBuffered (DWORD key)
{
	return false;
}
void ProjectApolloChecklistMFD::Update (HDC hDC)
{
	// An example of how to simply output the text element of the "current" item.  Outputs nothing if no checklists are active yet.
	item.group = -1;
	item.index = -1;
	if (conn.GetChecklistItem(&item))
		TextOut(hDC, 0,0,item.text.c_str(),item.text.size());
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