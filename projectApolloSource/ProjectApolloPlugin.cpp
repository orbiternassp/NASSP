#define STRICT
#define ORBITER_MODULE
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

#include "MFDResource.h"
#include "ProjectApolloMFD.h"
#include "ProjectApolloChecklistMFD.h"
#include "ProjectApolloPlugin.h"

DLLCLBK void opcDLLInit(HINSTANCE hDLL)
{
	ProjectApolloMFDopcDLLInit (hDLL);
	ProjectApolloChecklistMFDopcDLLInit (hDLL);
}

DLLCLBK void opcDLLExit(HINSTANCE hDLL)
{
	ProjectApolloMFDopcDLLExit (hDLL);
	ProjectApolloChecklistMFDopcDLLExit (hDLL);
}

DLLCLBK void opcTimestep(double simt, double simdt, double mjd)
{
	ProjectApolloMFDopcTimestep (simt,simdt,mjd);
	ProjectApolloChecklistMFDopcTimestep (simt,simdt,mjd);
}