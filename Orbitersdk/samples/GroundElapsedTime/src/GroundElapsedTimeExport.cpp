/***************************************************************************
  This file is part of the GroundElapsedTime MFD addon for NASSP.

  See GroundElapsedTimeExport.h for the big-picture explanation. This file
  is the actual implementation.
  ***************************************************************************/

#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

#include <stdio.h>

#include "GroundElapsedTimeCommon.h"
#include "GroundElapsedTimeExport.h"

// The name of the file we write. It's a plain relative path (no folder),
// which lands in Orbiter's own installation folder (next to Orbiter.exe)
// because that's the "current directory" Orbiter itself runs in, and
// every relative path an Orbiter addon uses is resolved from there.
static const char *GET_EXPORT_FILE_NAME = "GroundElapsedTime.txt";

// How often (in seconds of SIMULATION time, not real/wall-clock time) we
// rewrite the export file. One second matches what was asked for: a
// steady, once-a-second readout that an external transcript-matching
// program can poll or watch for changes.
static const double GET_EXPORT_INTERVAL_SECONDS = 1.0;

// Remembers the simulation time ("simt") the last time we wrote the file,
// so GroundElapsedTimeExportStep() knows when a full second has passed.
// A negative starting value guarantees we write immediately the first
// time we're called, instead of waiting a second after the scenario
// loads.
static double g_lastExportSimt = -1.0;

void GroundElapsedTimeExportInit()
{
	g_lastExportSimt = -1.0;
}

void GroundElapsedTimeExportExit()
{
	// Nothing to release - see the comment in the header for why this
	// function still exists.
}

void GroundElapsedTimeExportStep(double simt)
{
	// Only do any work once a second of simulation time has actually
	// gone by since our last write. This keeps us from re-opening and
	// rewriting the file dozens of times per second for no benefit.
	if (g_lastExportSimt >= 0.0 && (simt - g_lastExportSimt) < GET_EXPORT_INTERVAL_SECONDS)
		return;

	g_lastExportSimt = simt;

	// oapiGetFocusInterface() gives us whichever vessel the player is
	// currently flying/viewing - the same vessel a newly opened MFD would
	// attach to. If there's no scenario loaded yet, this can be NULL.
	VESSEL *focusVessel = oapiGetFocusInterface();

	double mt = 0.0;
	bool haveGET = (focusVessel != NULL) && ComputeGroundElapsedTime(focusVessel, mt);

	// Before liftoff (or if we don't have a supported vessel at all), we
	// simply write all zeroes rather than a negative countdown - this is
	// simplest for an external program matching against a transcript,
	// where "GET has not started yet" and "GET is exactly zero" can be
	// treated the same way.
	if (!haveGET || mt < 0.0)
		mt = 0.0;

	int totalSeconds = (int)mt;
	int days = totalSeconds / 86400;
	int hours = (totalSeconds / 3600) % 24;
	int minutes = (totalSeconds / 60) % 60;
	int seconds = totalSeconds % 60;

	// "w" mode means "create this file if it doesn't exist yet, and
	// erase/overwrite whatever was in it before" - exactly what we want,
	// since we only ever care about the *current* GET, not a history of
	// past values.
	FILE *f = fopen(GET_EXPORT_FILE_NAME, "w");
	if (f)
	{
		fprintf(f, "%02d %02d %02d %02d\n", days, hours, minutes, seconds);
		fclose(f);
	}
}
