/***************************************************************************
  This file is part of the GroundElapsedTime MFD addon for NASSP.

  This header declares the three small functions that make up the
  "export GET to a text file" feature described in this addon's README:
  once per second of simulation time, we write the current Ground Elapsed
  Time into a plain text file, GroundElapsedTime.txt, sitting right next
  to Orbiter.exe. Another program (completely separate from Orbiter) can
  then watch that file and use it however you like - for example, to play
  back a mission transcript in sync with the simulation.

  These three functions are called from GroundElapsedTimeMFD.cpp's
  opcDLLInit/opcDLLExit/opcPreStep, which are the standard Orbiter plugin
  entry points (see the comments there for more on what those do).
  ***************************************************************************/

#ifndef __GROUNDELAPSEDTIMEEXPORT_H
#define __GROUNDELAPSEDTIMEEXPORT_H

// Called once, when the DLL is first loaded by Orbiter. Resets our
// "when did we last write the file" bookkeeping.
void GroundElapsedTimeExportInit();

// Called once, when the DLL is being unloaded by Orbiter. Nothing to
// clean up today (we open and close the file fresh each time we write
// it), but kept symmetrical with GroundElapsedTimeExportInit() in case
// that changes later.
void GroundElapsedTimeExportExit();

// Called once per simulation timestep (from opcPreStep). "simt" is the
// current simulation time, in seconds, since Orbiter started the
// scenario - NOT the same thing as Ground Elapsed Time, just Orbiter's
// own clock used for pacing. We use it only to decide "has at least one
// second of simulation time gone by since we last wrote the file?".
void GroundElapsedTimeExportStep(double simt);

#endif // !__GROUNDELAPSEDTIMEEXPORT_H
