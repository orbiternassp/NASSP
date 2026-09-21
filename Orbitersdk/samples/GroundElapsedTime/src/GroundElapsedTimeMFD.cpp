/***************************************************************************
  This file is part of the GroundElapsedTime MFD addon for NASSP.

  This is the "real" implementation of the GroundElapsedTimeMFD class that
  GroundElapsedTimeMFD.h declared. If you're new to C++: this file is where
  we actually write out *what happens* when each function runs.

  It also contains the handful of special functions ("opcDLLInit",
  "opcDLLExit", "opcPreStep") that Orbiter looks for by name in every addon
  DLL it loads - these are how Orbiter and an addon talk to each other at
  the whole-plugin level (as opposed to MsgProc, which is specific to one
  MFD mode). See ProjectApollo's ProjectApolloPlugin.cpp for the same
  pattern used by NASSP's main addon.
  ***************************************************************************/

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "GroundElapsedTimeMFD.h"
#include "GroundElapsedTimeCommon.h"
#include "GroundElapsedTimeExport.h"

// A DLL-wide (global) variable that remembers the ID Orbiter assigns to
// our MFD mode once we register it. We need this later so we can
// *un*register the same mode when the DLL is unloaded.
static int g_MFDmode;

// ==============================================================
// Orbiter plugin entry points
// ==============================================================

// opcDLLInit runs exactly once, right when Orbiter loads this DLL (for
// example, when Orbiter itself starts up, since this DLL lives in
// Modules/Plugin). This is where we register our new MFD mode so it shows
// up in the list of MFD modes the player can select in-game.
DLLCLBK void opcDLLInit(HINSTANCE hDLL)
{
	static char *name = "Ground Elapsed Time"; // Full name shown to the player
	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_G;          // Suggested keyboard shortcut (Ctrl+F4 cycles through modes; this key is a fallback/legacy hint)
	spec.context = NULL;
	spec.msgproc = GroundElapsedTimeMFD::MsgProc; // Our MFD mode's callback function (see MsgProc below)

	// Ask Orbiter to add our MFD mode to its list of available modes.
	g_MFDmode = oapiRegisterMFDMode(spec);

	// Get the file-export feature ready (see GroundElapsedTimeExport.cpp).
	GroundElapsedTimeExportInit();
}

// opcDLLExit runs once when Orbiter unloads this DLL (normally when
// Orbiter itself closes). We must undo whatever opcDLLInit registered.
DLLCLBK void opcDLLExit(HINSTANCE hDLL)
{
	oapiUnregisterMFDMode(g_MFDmode);
	GroundElapsedTimeExportExit();
}

// opcPreStep runs once per simulation timestep, for every timestep,
// regardless of which MFD page (if any) is currently on screen for any
// vessel. We use it purely to drive the file-export feature, so that
// GroundElapsedTime.txt keeps updating even while the player is looking
// at a totally different MFD.
DLLCLBK void opcPreStep(double simt, double simdt, double mjd)
{
	GroundElapsedTimeExportStep(simt);
}

// ==============================================================
// GroundElapsedTimeMFD implementation
// ==============================================================

// The constructor runs once when the player opens this MFD mode. MFD2's
// own constructor (called here via ": MFD2(w, h, vessel)") takes care of
// the generic Orbiter MFD bookkeeping; we just remember the width/height
// and which vessel we're attached to for later use in Update().
GroundElapsedTimeMFD::GroundElapsedTimeMFD(DWORD w, DWORD h, VESSEL *vessel) : MFD2(w, h, vessel)
{
	width = w;
	height = h;
	ourVessel = vessel;
}

// The destructor runs once when the MFD is closed. We have nothing of our
// own to clean up (no memory allocated, no files left open), so this can
// be empty - but MFD2 still expects us to declare and define it.
GroundElapsedTimeMFD::~GroundElapsedTimeMFD()
{
}

// Update() is called repeatedly (many times per second) by Orbiter while
// this MFD page is the one being displayed, and is where we draw
// everything using the Sketchpad ("skp") drawing API.
bool GroundElapsedTimeMFD::Update(oapi::Sketchpad *skp)
{
	// Title() draws Orbiter's standard MFD title bar for us, using
	// whatever text we give it.
	Title(skp, "Ground Elapsed Time");

	skp->SetFont(GetDefaultFont(0));
	skp->SetBackgroundMode(oapi::Sketchpad::BK_TRANSPARENT);
	skp->SetTextAlign(oapi::Sketchpad::CENTER);

	double mt = 0.0;
	if (!ComputeGroundElapsedTime(ourVessel, mt))
	{
		// ComputeGroundElapsedTime() only returns false for vessel types
		// NASSP doesn't have a mission-time reading for (i.e. not a
		// Saturn/LEM/S-IVB/Crawler), so let the player know why nothing
		// useful is being shown.
		skp->SetTextColor(RGB(255, 0, 0));
		skp->Text(width / 2, (int)(height * 0.45), "Unsupported vessel", 18);
		return true;
	}

	// From here on, this is the exact same "seconds -> H:MM:SS" formatting
	// that ProjectApolloMFD's "Draw mission time" code and
	// ProjectApolloChecklistMFD's DisplayMissionElapsedTime() both use.
	char buffer[100];
	int secs = abs((int)mt);
	int hours = (secs / 3600);
	secs -= (hours * 3600);
	int minutes = (secs / 60);
	secs -= 60 * minutes;
	if (mt < 0)
		sprintf(buffer, "-%d:%02d:%02d", hours, minutes, secs);
	else
		sprintf(buffer, "%d:%02d:%02d", hours, minutes, secs);

	skp->SetTextColor(RGB(0, 255, 0));
	skp->Text(width / 2, (int)(height * 0.4), buffer, (int)strlen(buffer));

	return true;
}

// MsgProc is Orbiter's way of telling our MFD mode about important
// events. The only one we care about is "the player just opened this MFD
// mode" (OAPI_MSG_MFD_OPENED), at which point we must construct our MFD
// object and hand Orbiter a pointer to it.
int GroundElapsedTimeMFD::MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		// LOWORD(wparam)/HIWORD(wparam) are the MFD's pixel width/height,
		// and lparam is a pointer to the vessel the MFD is attached to -
		// this is simply how Orbiter packs these three values together
		// for this particular message.
		return (int)(new GroundElapsedTimeMFD(LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam));
	}
	return 0;
}
