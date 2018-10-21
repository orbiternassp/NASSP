/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Apollo RTCC MFD OAPI Module

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

#define STRICT
#define ORBITER_MODULE

#include "Orbitersdk.h"
#include "ApolloRTCCMFD.h"
#include "ARoapiModule.h"

//
// ==============================================================
// Pointers to Global variables in Apollo RTCC
// ==============================================================
//

extern ARoapiModule *g_coreMod;
extern ARCore *GCoreData[32];
extern VESSEL *GCoreVessel[32];
extern AR_GCore *g_SC;
extern int nGutsUsed;
extern int g_MFDmode;

//
// Static globals for this compilation file
//

static char *name = "Apollo RTCC MFD";

//
// ==============================================================
// Orbiter DLL API interface
// ==============================================================
//

DLLCLBK void InitModule(HINSTANCE hDLL) {          // Called by Orbiter when module selected in the Launchpad
	g_coreMod = new ARoapiModule(hDLL);           // Declare a single root class instance for ApolloRTCCMFD for this simulation run


	MFDMODESPECEX spec;
	spec.name = name;
	spec.key = OAPI_KEY_W;                            // MFD mode selection key (change as needed)
	spec.msgproc = ARoapiModule::MsgProc;         // MFD mode callback function
	spec.context = NULL;
	g_MFDmode = oapiRegisterMFDMode(spec);           // Tell Orbiter we are an MFD
	oapiRegisterModule(g_coreMod);                    // Register this whole module with Orbiter
	nGutsUsed = 0;
}

DLLCLBK void ExitModule(HINSTANCE hDLL) {          // Called by Orbiter when module deselected in the Launchpad
	oapiUnregisterMFDMode(g_MFDmode);                // Unregister use as an MFD. Note - don't kill the g_coreMod module (Orbiter does this)
	g_coreMod = NULL;
	nGutsUsed = 0;
}

int ARoapiModule::MsgProc(UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam) {  // Message parser, handling MFD open requests
	switch (msg) {
	case OAPI_MSG_MFD_OPENED:
		return (int)(new ApolloRTCCMFD(LOWORD(wparam), HIWORD(wparam), (VESSEL*)lparam, mfd));    // Open an ephemeral Glideslope instance each time we make a new Glideslope MFD, plus F8, etc/ 
	}
	return 0;
}

ARoapiModule::ARoapiModule(HINSTANCE hDLL) : oapi::Module(hDLL) {}
ARoapiModule::~ARoapiModule() {}

void ARoapiModule::clbkSimulationStart(RenderMode mode) {}

void ARoapiModule::clbkSimulationEnd() {                                      // When we exit sim back to Launchpad, make sure we tidy things up properly
	for (int i = 0;i<nGutsUsed;i++) {
		delete GCoreData[i];
		GCoreVessel[i] = NULL;
	}
	nGutsUsed = 0;
	if (g_SC)
	{
		delete g_SC;
		g_SC = 0;
	}
	return;
}
void ARoapiModule::clbkPreStep(double simt, double simdt, double mjd) {      // Called on each iteration of the calc engine (more often than the MFD Update
	for (int i = 0;i<nGutsUsed;i++) {
		GCoreData[i]->MinorCycle(simt, simdt, mjd);
	}
	return;
}

void ARoapiModule::clbkPostStep(double simt, double simdt, double mjd) {}

void ARoapiModule::clbkDeleteVessel(OBJHANDLE hVessel) {                     // Tidy up when a vessel is deleted (stops clbkPreStep calling a dead vessel)
	VESSEL *vessel = oapiGetVesselInterface(hVessel);
	for (int i = 0;i<nGutsUsed;i++) {
		if (GCoreVessel[i] == vessel) {
			delete GCoreData[i];
			for (int j = i; j<nGutsUsed - 1; j++) {
				GCoreVessel[j] = GCoreVessel[j + 1];
				GCoreData[j] = GCoreData[j + 1];
			}
			nGutsUsed--;
			break;
		}
	}
}