/***************************************************************************
  This file is part of the GroundElapsedTime MFD addon for NASSP.

  See GroundElapsedTimeCommon.h for what ComputeGroundElapsedTime() does
  and why it exists as a shared helper.

  Beginner's note on why so many #include lines are needed below: Saturn,
  LEM, SIVB, Crawler and MCC are big, "real" NASSP classes with lots of
  systems (electrical, guidance computer, switches, etc.), and their own
  header files (saturn.h, LEM.h, ...) need all of *their* sub-system
  headers included first so the compiler knows the full shape of the
  class. This exact same list of includes, in this order, is what NASSP's
  own ProjectApolloMFD.cpp uses for the same reason - we're simply copying
  a combination that is already proven to compile correctly inside NASSP.

  Importantly, we do NOT need to link against Saturn.lib, LEM.lib, etc.
  Saturn::GetMissionTime()/LEM::GetMissionTime()/Crawler::GetMissionTime()
  are all declared directly inside their header files with their full
  body (called an "inline" function), so the compiler can use them
  straight from the header. MCC::GetMissionTime() and SIVB::GetMissionTime()
  are declared as "virtual" instead - that means the *actual* running
  Saturn.dll/LEM.dll/MCC.dll/SIVb.dll (already loaded into Orbiter's
  process by the time our MFD runs) supplies the real code to run, and
  C++ finds it automatically through the vessel object's "vtable" (a
  hidden table of function pointers every polymorphic C++ object carries).
  This is exactly the same trick ProjectApolloMFD.vcxproj relies on to
  read Saturn/LEM/etc. mission times without linking their .lib files.
  ***************************************************************************/

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"

#include "math.h"
#include "windows.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "LEMcomputer.h"
#include "IMU.h"
#include "saturn.h"
#include "LEM.h"
#include "Crawler.h"
#include "MCCVessel.h"
#include "MCC.h"
#include "sivb.h"
#include "nassputils.h"

#include "GroundElapsedTimeCommon.h"

using namespace nassp;

bool ComputeGroundElapsedTime(VESSEL *vessel, double &outSeconds)
{
	if (!vessel)
		return false;

	// Work out what kind of NASSP vessel we've been given, exactly the way
	// ProjectApolloMFD's constructor does it: by comparing the vessel's
	// Orbiter class name (utils::IsVessel checks VESSEL::GetClassName()
	// against NASSP's known class names).
	Saturn *saturn = NULL;
	LEM *lem = NULL;
	SIVB *sivb = NULL;
	Crawler *crawler = NULL;
	bool supported = false;

	if (utils::IsVessel(vessel, utils::Saturn))
	{
		saturn = (Saturn *)vessel;
		supported = true;
	}
	else if (utils::IsVessel(vessel, utils::LEM))
	{
		lem = (LEM *)vessel;
		supported = true;
	}
	else if (utils::IsVessel(vessel, utils::SIVB))
	{
		sivb = (SIVB *)vessel;
		supported = true;
	}
	else if (utils::IsVessel(vessel, utils::Crawler))
	{
		crawler = (Crawler *)vessel;
		supported = true;
	}

	if (!supported)
		return false;

	// Look for the simulated Mission Control vessel, exactly the way
	// ProjectApolloMFD's constructor does: by its fixed scenario name
	// "MCC". Not every scenario includes an MCC vessel, so this can
	// legitimately stay NULL.
	MCC *mcc = NULL;
	OBJHANDLE hMCC = oapiGetVesselByName("MCC");
	if (hMCC != NULL)
	{
		VESSEL *pVessel = oapiGetVesselInterface(hMCC);
		if (pVessel && utils::IsVessel(pVessel, utils::MCC))
		{
			MCCVessel *pMCCVessel = static_cast<MCCVessel *>(pVessel);
			if (pMCCVessel->mcc)
				mcc = pMCCVessel->mcc;
		}
	}

	// This is the exact same rule ProjectApolloMFD uses in its "Draw
	// mission time" block:
	double mt = 0.0;

	if (sivb)
	{
		// S-IVB stages aren't crewed, so there's no reason to prefer
		// Mission Control's clock over the stage's own mission time.
		mt = sivb->GetMissionTime();
	}
	else
	{
		if (mcc)
			mt = mcc->GetMissionTime();

		if (mt <= 0.0) // Mission time from MCC might be nonsense before liftoff
		{
			if (saturn) mt = saturn->GetMissionTime();
			if (crawler) mt = crawler->GetMissionTime();
			if (lem) mt = lem->GetMissionTime();
		}
	}

	outSeconds = mt;
	return true;
}
