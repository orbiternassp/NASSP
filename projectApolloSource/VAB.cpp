/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  VAB Transporter vessel

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

  **************************** Revision History ****************************
  *	$Log$
  **************************************************************************/

#define ORBITER_MODULE

#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "nasspsound.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"
#include "tracer.h"

#include "VAB.h"

#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "saturn.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo VAB.log";


DLLCLBK void InitModule(HINSTANCE hModule) {
	g_hDLL = hModule;
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new VAB(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (VAB*)vessel;
}


VAB::VAB(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	firstTimestepDone = false;
	meshcountSaturn = 0;
	highBay1Door_Status = DOOR_CLOSED;
	highBay1Door_Proc = 0;
	highBay3Door_Status = DOOR_CLOSED;
	highBay3Door_Proc = 0;
	crane_Status = CRANE_BEGIN;
	crane_Proc = 0.00001;
	saturnName[0] = '\0';
	saturnVisible = false;

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);
}

VAB::~VAB() {
}

void VAB::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(450000000);
	SetSize(111);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0,0,0);
    meshindexVAB = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\ApolloVAB"), &meshoffset);
	SetMeshVisibilityMode(meshindexVAB, MESHVIS_ALWAYS);

	meshcountSaturn = 2;
	meshoffset = _V(-24.8, -60, -109);
	meshindexSaturn[0] = AddMesh(oapiLoadMeshGlobal("sat5stg2"), &meshoffset);
	meshoffset = _V(-24.8, -73.3, -109);
	meshindexSaturn[1] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5intstg4"), &meshoffset);
	
	for (int i = 0; i < meshcountSaturn; i++) 
		SetMeshVisibilityMode(meshindexSaturn[i], MESHVIS_NEVER);

	DefineAnimations();
	SetTouchdownPoints(_V(0, -78, 9), _V(-4, -78, -9), _V(4, -78, -9));
	SetCameraOffset(_V(-27.0, -12.0, -48.0));
}

void VAB::DefineAnimations() {

	// High bay 1 door
	static UINT highBay1Door1_groups[1] = {46};
	static MGROUP_TRANSLATE highBay1Door1 (0, highBay1Door1_groups, 1, _V(12, 0, 0));

	static UINT highBay1Door2_groups[1] = {47};
	static MGROUP_TRANSLATE highBay1Door2 (0, highBay1Door2_groups, 1, _V(-12, 0, 0));

	static UINT highBay1Door3_groups[2] = {47, 48};
	static MGROUP_TRANSLATE highBay1Door3 (0, highBay1Door3_groups, 2, _V(-12, 0, 0));

	static UINT highBay1Door4_groups[2] = {46, 49};
	static MGROUP_TRANSLATE highBay1Door4 (0, highBay1Door4_groups, 2, _V(12, 0, 0));

	static UINT highBay1Door5_groups[1] = {64};
	static MGROUP_TRANSLATE highBay1Door5 (0, highBay1Door5_groups, 1, _V(0, 100, 0));

	static UINT highBay1Door6_groups[1] = {63};
	static MGROUP_TRANSLATE highBay1Door6 (0, highBay1Door6_groups, 1, _V(0, 86, 0));

	static UINT highBay1Door7_groups[1] = {62};
	static MGROUP_TRANSLATE highBay1Door7 (0, highBay1Door7_groups, 1, _V(0, 72, 0));

	static UINT highBay1Door8_groups[1] = {61};
	static MGROUP_TRANSLATE highBay1Door8 (0, highBay1Door8_groups, 1, _V(0, 58, 0));

	static UINT highBay1Door9_groups[1] = {60};
	static MGROUP_TRANSLATE highBay1Door9 (0, highBay1Door9_groups, 1, _V(0, 44, 0));

	static UINT highBay1Door10_groups[1] = {59};
	static MGROUP_TRANSLATE highBay1Door10 (0, highBay1Door10_groups, 1, _V(0, 30, 0));

	static UINT highBay1Door11_groups[1] = {58};
	static MGROUP_TRANSLATE highBay1Door11 (0, highBay1Door11_groups, 1, _V(0, 16, 0));

	anim_HighBay1Door = CreateAnimation(0.0);
	AddAnimationComponent(anim_HighBay1Door, 0,      0.4, &highBay1Door1);
	AddAnimationComponent(anim_HighBay1Door, 0,      0.4, &highBay1Door2);
	AddAnimationComponent(anim_HighBay1Door, 0.55,	 0.7, &highBay1Door3);
	AddAnimationComponent(anim_HighBay1Door, 0.55,   0.7, &highBay1Door4);
	AddAnimationComponent(anim_HighBay1Door, 0.7,    1.0, &highBay1Door5);
	AddAnimationComponent(anim_HighBay1Door, 0.7435, 1.0, &highBay1Door6);
	AddAnimationComponent(anim_HighBay1Door, 0.787,  1.0, &highBay1Door7);
	AddAnimationComponent(anim_HighBay1Door, 0.8305, 1.0, &highBay1Door8);
	AddAnimationComponent(anim_HighBay1Door, 0.87,   1.0, &highBay1Door9);
	AddAnimationComponent(anim_HighBay1Door, 0.91,   1.0, &highBay1Door10);
	AddAnimationComponent(anim_HighBay1Door, 0.95,   1.0, &highBay1Door11);

	// High bay 3 door
	static UINT highBay3Door1_groups[1] = {43};
	static MGROUP_TRANSLATE highBay3Door1 (0, highBay3Door1_groups, 1, _V(12, 0, 0));

	static UINT highBay3Door2_groups[1] = {42};
	static MGROUP_TRANSLATE highBay3Door2 (0, highBay3Door2_groups, 1, _V(-12, 0, 0));

	static UINT highBay3Door3_groups[2] = {42, 45};
	static MGROUP_TRANSLATE highBay3Door3 (0, highBay3Door3_groups, 2, _V(-12, 0, 0));

	static UINT highBay3Door4_groups[2] = {43, 44};
	static MGROUP_TRANSLATE highBay3Door4 (0, highBay3Door4_groups, 2, _V(12, 0, 0));

	static UINT highBay3Door5_groups[1] = {71};
	static MGROUP_TRANSLATE highBay3Door5 (0, highBay3Door5_groups, 1, _V(0, 100, 0));

	static UINT highBay3Door6_groups[1] = {70};
	static MGROUP_TRANSLATE highBay3Door6 (0, highBay3Door6_groups, 1, _V(0, 86, 0));

	static UINT highBay3Door7_groups[1] = {69};
	static MGROUP_TRANSLATE highBay3Door7 (0, highBay3Door7_groups, 1, _V(0, 72, 0));

	static UINT highBay3Door8_groups[1] = {68};
	static MGROUP_TRANSLATE highBay3Door8 (0, highBay3Door8_groups, 1, _V(0, 58, 0));

	static UINT highBay3Door9_groups[1] = {67};
	static MGROUP_TRANSLATE highBay3Door9 (0, highBay3Door9_groups, 1, _V(0, 44, 0));

	static UINT highBay3Door10_groups[1] = {66};
	static MGROUP_TRANSLATE highBay3Door10 (0, highBay3Door10_groups, 1, _V(0, 30, 0));

	static UINT highBay3Door11_groups[1] = {65};
	static MGROUP_TRANSLATE highBay3Door11 (0, highBay3Door11_groups, 1, _V(0, 16, 0));

	anim_HighBay3Door = CreateAnimation(0.0);
	AddAnimationComponent(anim_HighBay3Door, 0,      0.4, &highBay3Door1);
	AddAnimationComponent(anim_HighBay3Door, 0,      0.4, &highBay3Door2);
	AddAnimationComponent(anim_HighBay3Door, 0.55,   0.7, &highBay3Door3);
	AddAnimationComponent(anim_HighBay3Door, 0.55,   0.7, &highBay3Door4);
	AddAnimationComponent(anim_HighBay3Door, 0.7,    1.0, &highBay3Door5);
	AddAnimationComponent(anim_HighBay3Door, 0.7435, 1.0, &highBay3Door6);
	AddAnimationComponent(anim_HighBay3Door, 0.787,  1.0, &highBay3Door7);
	AddAnimationComponent(anim_HighBay3Door, 0.8305, 1.0, &highBay3Door8);
	AddAnimationComponent(anim_HighBay3Door, 0.87,   1.0, &highBay3Door9);
	AddAnimationComponent(anim_HighBay3Door, 0.91,   1.0, &highBay3Door10);
	AddAnimationComponent(anim_HighBay3Door, 0.95,   1.0, &highBay3Door11);

	// Crane
	static UINT crane1_groups[1] = {0};
	static MGROUP_SCALE crane1 (0, crane1_groups, 1, _V(0, 70, 0), _V(1, 0.1, 1));
	static UINT crane2_groups[2] = {1, 21};
	static MGROUP_TRANSLATE crane2 (0, crane2_groups, 2, _V(0, 122.7, 0));

	static UINT crane3_groups[6] = {0, 1, 8, 9, 10, 21};
	static MGROUP_TRANSLATE crane3 (0, crane3_groups, 6, _V(0, 0, -50));

	static UINT crane4_groups[1] = {0};
	static MGROUP_SCALE crane4 (0, crane4_groups, 1, _V(0, 70, 0), _V(1, (102.0 / 122.7) * 9.0 + 1.0, 1));
	static UINT crane5_groups[2] = {1, 21};
	static MGROUP_TRANSLATE crane5 (0, crane5_groups, 2, _V(0, -102, 0));

	static UINT crane6_groups[1] = {0};
	static MGROUP_SCALE crane6 (0, crane6_groups, 1, _V(0, 70, 0), _V(1, 1.0 / ((102.0 / 122.7) * 9.0 + 1.0), 1));
	static UINT crane7_groups[2] = {1, 21};
	static MGROUP_TRANSLATE crane7 (0, crane7_groups, 2, _V(0, 102, 0));

	static UINT crane8_groups[6] = {0, 1, 8, 9, 10, 21};
	static MGROUP_TRANSLATE crane8 (0, crane8_groups, 6, _V(0, 0, 85.2));

	static UINT crane9_groups[1] = {0};
	static MGROUP_SCALE crane9 (0, crane9_groups, 1, _V(0, 70, 0), _V(1, (48.5 / 122.7) * 9.0 + 1.0, 1));
	static UINT crane10_groups[2] = {1, 21};
	static MGROUP_TRANSLATE crane10 (0, crane10_groups, 2, _V(0, -48.5, 0));

	static UINT crane11_groups[1] = {0};
//	static MGROUP_SCALE crane11 (0, crane11_groups, 1, _V(0, 70, 0), _V(1, (3.0 / 122.7) * 9.0 + 1.0, 1));
	static MGROUP_SCALE crane11 (0, crane11_groups, 1, _V(0, 70, 0), _V(1, 65.13 / 62.13, 1));
	static UINT crane12_groups[2] = {1, 21};
	static MGROUP_TRANSLATE crane12 (0, crane12_groups, 2, _V(0, -3, 0));

	static UINT crane13_groups[1] = {0};
	static MGROUP_SCALE crane13 (0, crane13_groups, 1, _V(0, 70, 0), _V(1, 1.0 / ((51.5 / 122.7) * 9.0 + 1.0), 1));
	static UINT crane14_groups[2] = {1, 21};
	static MGROUP_TRANSLATE crane14 (0, crane14_groups, 2, _V(0, 51.5, 0));

	static UINT crane15_groups[6] = {0, 1, 8, 9, 10, 21};
	static MGROUP_TRANSLATE crane15 (0, crane15_groups, 6, _V(0, 0, -35.2));

	static MGROUP_ROTATE saturn0 (1, NULL, 0, _V(0,0,0), _V(-1,0,0), (float)(0.5*PI));
	static MGROUP_ROTATE saturn1 (1, NULL, 0, _V(0,0,0), _V(0,1,0), (float)(-0.5*PI));
	static MGROUP_TRANSLATE saturn2 (1, NULL, 0, _V(0, 102, 0));
	static MGROUP_TRANSLATE saturn3 (1, NULL, 0, _V(0, 0, 85.2));
	static MGROUP_TRANSLATE saturn4 (1, NULL, 0, _V(0, -48.5, 0));
	static MGROUP_TRANSLATE saturn5 (1, NULL, 0, _V(0, -3, 0));

	static MGROUP_ROTATE saturn10 (2, NULL, 0, _V(0,0,0), _V(-1,0,0), (float)(0.5*PI));
	static MGROUP_ROTATE saturn11 (2, NULL, 0, _V(0,0,0), _V(0,1,0), (float)(-0.5*PI));
	static MGROUP_TRANSLATE saturn12 (2, NULL, 0, _V(0, 102, 0));
	static MGROUP_TRANSLATE saturn13 (2, NULL, 0, _V(0, 0, 85.2));
	static MGROUP_TRANSLATE saturn14 (2, NULL, 0, _V(0, -48.5, 0));
	static MGROUP_TRANSLATE saturn15 (2, NULL, 0, _V(0, -3, 0));

	anim_Crane = CreateAnimation(0.0);
	AddAnimationComponent(anim_Crane, 0,    0.00001, &crane1);
	AddAnimationComponent(anim_Crane, 0,    0.00001, &crane2);
	AddAnimationComponent(anim_Crane, 0.00001, 0.1, &crane3);
	AddAnimationComponent(anim_Crane, 0.1,  0.2, &crane4);
	AddAnimationComponent(anim_Crane, 0.1,  0.2, &crane5);
	AddAnimationComponent(anim_Crane, 0.2,  0.4, &crane6);
	AddAnimationComponent(anim_Crane, 0.2,  0.4, &crane7);
	AddAnimationComponent(anim_Crane, 0.4,  0.6, &crane8);
	AddAnimationComponent(anim_Crane, 0.6,  0.75, &crane9);
	AddAnimationComponent(anim_Crane, 0.6,  0.75, &crane10); 
	AddAnimationComponent(anim_Crane, 0.75, 0.8, &crane11); 
	AddAnimationComponent(anim_Crane, 0.75, 0.8, &crane12); 
	AddAnimationComponent(anim_Crane, 0.8,  0.9, &crane13); 
	AddAnimationComponent(anim_Crane, 0.8,  0.9, &crane14); 
	AddAnimationComponent(anim_Crane, 0.9,  1.0, &crane15); 

	AddAnimationComponent(anim_Crane, 0,    0.000005, &saturn0);
	AddAnimationComponent(anim_Crane, 0.000005, 0.00001, &saturn1);
	AddAnimationComponent(anim_Crane, 0.2,  0.4, &saturn2);
	AddAnimationComponent(anim_Crane, 0.4,  0.6, &saturn3);
	AddAnimationComponent(anim_Crane, 0.6,  0.75, &saturn4);
	AddAnimationComponent(anim_Crane, 0.75, 0.8, &saturn5);

	AddAnimationComponent(anim_Crane, 0,    0.000005, &saturn10);
	AddAnimationComponent(anim_Crane, 0.000005,    0.00001, &saturn11);
	AddAnimationComponent(anim_Crane, 0.2,  0.4, &saturn12);
	AddAnimationComponent(anim_Crane, 0.4,  0.6, &saturn13);
	AddAnimationComponent(anim_Crane, 0.6,  0.75, &saturn14);
	AddAnimationComponent(anim_Crane, 0.75, 0.8, &saturn15);
}

void VAB::clbkPostCreation() {
	
	SetAnimation(anim_Crane, crane_Proc);
}

void VAB::clbkPreStep(double simt, double simdt, double mjd) {

	if (!firstTimestepDone) DoFirstTimestep();
}

void VAB::clbkPostStep (double simt, double simdt, double mjd) {

	// High bay 1 door
	if (highBay1Door_Status == DOOR_CLOSING || highBay1Door_Status == DOOR_OPENING) {
		double da = simdt / 350.0;
		if (highBay1Door_Status == DOOR_CLOSING) {
			if (highBay1Door_Proc > 0.0)
				highBay1Door_Proc = max(0.0, highBay1Door_Proc - da);
			else
				highBay1Door_Status = DOOR_CLOSED;

		} else { // door opening
			if (highBay1Door_Proc < 1.0)
				highBay1Door_Proc = min(1.0, highBay1Door_Proc + da);
			else
				highBay1Door_Status = DOOR_OPEN;
		}
		SetAnimation(anim_HighBay1Door, highBay1Door_Proc);
	}

	// High bay 3 door
	if (highBay3Door_Status == DOOR_CLOSING || highBay3Door_Status == DOOR_OPENING) {
		double da = simdt / 350.0;
		if (highBay3Door_Status == DOOR_CLOSING) {
			if (highBay3Door_Proc > 0.0)
				highBay3Door_Proc = max(0.0, highBay3Door_Proc - da);
			else
				highBay3Door_Status = DOOR_CLOSED;

		} else { // door opening
			if (highBay3Door_Proc < 1.0)
				highBay3Door_Proc = min(1.0, highBay3Door_Proc + da);
			else
				highBay3Door_Status = DOOR_OPEN;
		}
		SetAnimation(anim_HighBay3Door, highBay3Door_Proc);
	}

	// Crane
	if (crane_Status == CRANE_BUILDING || crane_Status == CRANE_UNBUILDING) {
		double da = simdt / 200.0;
		if (crane_Status == CRANE_BUILDING) {
			if (crane_Proc < 1.0)
				crane_Proc = min(1.0, crane_Proc + da);
			else
				crane_Status = CRANE_END;

			if (!saturnVisible && crane_Proc > 0.00001 && crane_Proc < 0.8) {
				for (int i = 0; i < meshcountSaturn; i++) 
					SetMeshVisibilityMode(meshindexSaturn[i], MESHVIS_ALWAYS);

				saturnVisible = true;
			}
			
			if (saturnVisible && crane_Proc >= 0.8) {
				for (int i = 0; i < meshcountSaturn; i++) 
					SetMeshVisibilityMode(meshindexSaturn[i], MESHVIS_NEVER);

				if (saturnName[0] != '\0') {
					OBJHANDLE hLV = oapiGetVesselByName(saturnName);
					if (hLV) {
						Saturn *lav = (Saturn *) oapiGetVesselInterface(hLV);
						lav->LaunchVesselBuild();
					}
				}
				saturnVisible = false;
			} 
		} else { // unbuilding
			if (crane_Proc > 0.00001)
				crane_Proc = max(0.00001, crane_Proc - da);
			else
				crane_Status = CRANE_BEGIN;

			if (!saturnVisible && crane_Proc < 0.8) {
				for (int i = 0; i < meshcountSaturn; i++) 
					SetMeshVisibilityMode(meshindexSaturn[i], MESHVIS_ALWAYS);

				if (saturnName[0] != '\0') {
					OBJHANDLE hLV = oapiGetVesselByName(saturnName);
					if (hLV) {
						Saturn *lav = (Saturn *) oapiGetVesselInterface(hLV);
						lav->LaunchVesselUnbuild();
					}
				}
				saturnVisible = true;
			} 
		
			if (saturnVisible && crane_Proc <= 0.00001) {
				for (int i = 0; i < meshcountSaturn; i++) 
					SetMeshVisibilityMode(meshindexSaturn[i], MESHVIS_NEVER);

				saturnVisible = false;
			}
		
		}
		SetAnimation(anim_Crane, crane_Proc);
	}
}

void VAB::DoFirstTimestep() {

	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	firstTimestepDone = true;
}

void VAB::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
	//	if (!strnicmp (line, "VELOCITY", 8)) {
	//		sscanf (line + 8, "%lf", &velocity);
	//	} else {
			ParseScenarioLineEx (line, status);
	//	}
	}
}

void VAB::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState (scn);

	//oapiWriteScenario_int (scn, "PADINDEX", padIndex);
}

int VAB::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (!firstTimestepDone) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0;
	}

	if (key == OAPI_KEY_1 && down == true) {
		ToggleHighBay1Door();			
		return 1;
	}
	if (key == OAPI_KEY_3 && down == true) {
		ToggleHighBay3Door();			
		return 1;
	}
	if (key == OAPI_KEY_B && down == true) {
		BuildSaturnStage();
		return 1;
	}
	if (key == OAPI_KEY_U && down == true) {
		UnbuildSaturnStage();
	}

	return 0;
}

