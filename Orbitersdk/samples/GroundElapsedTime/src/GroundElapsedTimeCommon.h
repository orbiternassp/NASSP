/***************************************************************************
  This file is part of the GroundElapsedTime MFD addon for NASSP.

  This header declares one small, shared helper function that both the
  on-screen MFD (GroundElapsedTimeMFD.cpp) and the background file-export
  feature (GroundElapsedTimeExport.cpp) use to answer the same question:
  "for this vessel, what is the current Ground Elapsed Time, in seconds?"

  Keeping this logic in exactly one place means the number shown on the MFD
  screen and the number written to GroundElapsedTime.txt can never
  disagree with each other.
  ***************************************************************************/

#ifndef __GROUNDELAPSEDTIMECOMMON_H
#define __GROUNDELAPSEDTIMECOMMON_H

// ComputeGroundElapsedTime looks at "vessel" (any vessel Orbiter knows
// about - it might be a Saturn, a LEM, an S-IVB stage, a Crawler, or
// something NASSP doesn't recognize at all) and tries to work out its
// Ground Elapsed Time, in seconds, using the exact same rules as NASSP's
// own ProjectApolloMFD ("Draw mission time" section) and
// ProjectApolloChecklistMFD (DisplayMissionElapsedTime()):
//
//   1. If the vessel is an S-IVB third stage, use its own mission time
//      directly (it isn't a crewed vessel, so there's no reason to prefer
//      Mission Control's clock for it).
//   2. Otherwise, prefer the simulated Mission Control ("MCC") vessel's
//      mission time, if one exists in the scenario and its time is > 0.
//   3. If MCC's time looks like nonsense (<= 0, which happens before
//      liftoff), fall back to whichever of Saturn/Crawler/LEM the vessel
//      actually is.
//
// Returns true if "vessel" is a NASSP type we know how to read a mission
// time from, and writes that time (in seconds - can be negative before
// liftoff) into "outSeconds". Returns false (and leaves outSeconds
// untouched) for any vessel type we don't recognize, so callers can show
// an "Unsupported vessel" message instead of a meaningless number.
bool ComputeGroundElapsedTime(VESSEL *vessel, double &outSeconds);

#endif // !__GROUNDELAPSEDTIMECOMMON_H
