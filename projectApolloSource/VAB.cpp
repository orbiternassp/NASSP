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
  *	Revision 1.3  2005/11/21 13:31:34  tschachim
  *	New Saturn assembly meshes.
  *	
  *	Revision 1.2  2005/11/09 18:34:21  tschachim
  *	New Saturn assembly process.
  *	
  *	Revision 1.1  2005/10/31 11:59:22  tschachim
  *	New VAB.
  *	
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
	animCraneCount = 0;
	highBay1Door_Status = DOOR_CLOSED;
	highBay1Door_Proc = 0;
	highBay3Door_Status = DOOR_CLOSED;
	highBay3Door_Proc = 0;
	crane_Status = CRANE_BEGIN;
	crane_Proc = 0.00001;
	platform_Proc = 0;
	saturnStage1_Proc = 0;
	adjustSaturnStage1 = false;
	saturnName[0] = '\0';
	saturnVisible = false;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 16; j++) 
			mgroupCrane[i][j] = 0;
	}
	for (i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++) 
			mgroupSaturn[i][j] = 0;
	}
	for (i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) 
			mgroupCrane2[i][j] = 0;
	}

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);
}

VAB::~VAB() {

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 16; j++) 
			if (mgroupCrane[i][j]) delete mgroupCrane[i][j];
	}
	for (i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++) 
			if (mgroupSaturn[i][j]) delete mgroupSaturn[i][j];
	}
	for (i = 0; i < 5; i++) {
		for (int j = 0; j < 6; j++) 
			if (mgroupCrane2[i][j]) delete mgroupCrane2[i][j];
	}
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

	meshcountSaturn = 16;
	meshoffset = _V(75, -55, -66.4);
	meshindexSaturn[0] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg1"), &meshoffset);

	meshoffset = _V(75, -60, -66.4);
	meshindexSaturn[1] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg2base"), &meshoffset);
	meshoffset = _V(75, -73.3, -66.4);
	meshindexSaturn[2] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5intstg4"), &meshoffset);

	meshoffset = _V(75, -65, -66.4);
	meshindexSaturn[3] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg3base"), &meshoffset);
	meshoffset = _V(75, -84.2, -66.4);
	meshindexSaturn[4] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg2intstg"), &meshoffset);

	meshoffset = _V(75, -60, -66.4);
	meshindexSaturn[5] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5CM"), &meshoffset);
	meshoffset = _V(75 - 0.14, -60 - 4.15, -66.4);
	meshindexSaturn[6] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/nSaturn1_SM"), &meshoffset);
	meshoffset = _V(75 - 1.48, -60 - 8.7, -66.4 - 1.48);
	meshindexSaturn[7] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg31"), &meshoffset);
	meshoffset = _V(75 - 1.48, -60 - 8.7, -66.4 + 1.48);
	meshindexSaturn[8] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg32"), &meshoffset);
	meshoffset = _V(75 + 1.48, -60 - 8.7, -66.4 + 1.48);
	meshindexSaturn[9] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg33"), &meshoffset);
	meshoffset = _V(75 + 1.48, -60 - 8.7, -66.4 - 1.48);
	meshindexSaturn[10] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg34"), &meshoffset);
	meshoffset = _V(75.1, -60 - 11.25, -66.4 + 0.05);
	meshindexSaturn[11] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/LM_SLA"), &meshoffset);
	meshoffset = _V(75 + 1.35, -60 + 0.14, -66.4 + 0.02);
	meshindexSaturn[12] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SAT5HC"), &meshoffset);
	meshoffset = _V(75, -60 + 1.5, -66.4);
	meshindexSaturn[13] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5probe"), &meshoffset);
	meshoffset = _V(75, -60 - 21.25, -66.4);
	meshindexSaturn[14] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg3intstg"), &meshoffset);

	meshoffset = _V(75, -65, -66.4);
	meshindexSaturn[15] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5BPC"), &meshoffset);
	
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
	AddAnimationComponent(anim_HighBay1Door, 0,      0.3, &highBay1Door1);
	AddAnimationComponent(anim_HighBay1Door, 0,      0.3, &highBay1Door2);
	AddAnimationComponent(anim_HighBay1Door, 0.3,	 0.6, &highBay1Door3);
	AddAnimationComponent(anim_HighBay1Door, 0.3,    0.6, &highBay1Door4);
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
	AddAnimationComponent(anim_HighBay3Door, 0,      0.3, &highBay3Door1);
	AddAnimationComponent(anim_HighBay3Door, 0,      0.3, &highBay3Door2);
	AddAnimationComponent(anim_HighBay3Door, 0.3,    0.6, &highBay3Door3);
	AddAnimationComponent(anim_HighBay3Door, 0.3,    0.6, &highBay3Door4);
	AddAnimationComponent(anim_HighBay3Door, 0.7,    1.0, &highBay3Door5);
	AddAnimationComponent(anim_HighBay3Door, 0.7435, 1.0, &highBay3Door6);
	AddAnimationComponent(anim_HighBay3Door, 0.787,  1.0, &highBay3Door7);
	AddAnimationComponent(anim_HighBay3Door, 0.8305, 1.0, &highBay3Door8);
	AddAnimationComponent(anim_HighBay3Door, 0.87,   1.0, &highBay3Door9);
	AddAnimationComponent(anim_HighBay3Door, 0.91,   1.0, &highBay3Door10);
	AddAnimationComponent(anim_HighBay3Door, 0.95,   1.0, &highBay3Door11);

	// Saturn Stage 1
	DefineCraneAnimation(mgroupCrane[0], 110, 81.25);	
	DefineCrane2Animation(mgroupCrane2[0], 29, 78.5);

	mgroupSaturn[0][0] = new MGROUP_ROTATE    (meshindexSaturn[0], NULL, 0, _V(0, 0, 0), _V( 0, 1, 0), (float)(-0.5 * PI));
	mgroupSaturn[0][1] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(-78.5, 0, 0));	
	mgroupSaturn[0][2] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, 55.8, 0));
	mgroupSaturn[0][3] = new MGROUP_ROTATE    (meshindexSaturn[0], NULL, 0, _V(-135.2, 0, 0), _V(0, 0, 1), (float)(-0.5 * PI));
	mgroupSaturn[0][4] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, 55, 0));
	mgroupSaturn[0][5] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, 0, 42.5));
	mgroupSaturn[0][6] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, -(80.85 - 3.0), 0));
	mgroupSaturn[0][7] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, -3, 0));

	// Saturn Stage 2
	DefineCraneAnimation(mgroupCrane[1], 107.4, 56.9);
	DefineCrane2Animation(mgroupCrane2[1], 31.4, 99.7);
	DefineSaturnAnimation(mgroupSaturn[1], meshindexSaturn[1], 107.4, 56.9);
	DefineSaturnAnimation(mgroupSaturn[2], meshindexSaturn[2], 107.4, 56.9);

	// Saturn Stage 3
	DefineCraneAnimation(mgroupCrane[2], 111 + 2.8, 36.25 + 2.8);
	DefineCrane2Animation(mgroupCrane2[2], 35 + 2.8, 99.7 + 2.8);
	DefineSaturnAnimation(mgroupSaturn[3], meshindexSaturn[3], 111 + 2.8, 36.25 + 2.8);
	DefineSaturnAnimation(mgroupSaturn[4], meshindexSaturn[4], 111 + 2.8, 36.25 + 2.8);

	// Saturn Stage CSM/LTA
	DefineCraneAnimation(mgroupCrane[3], 113, 22);
	DefineCrane2Animation(mgroupCrane2[3], 37, 99.7);
	for (int i = 5; i < 15; i++) 
		DefineSaturnAnimation(mgroupSaturn[i], meshindexSaturn[i], 113, 22);

	// Saturn Stage Tower
	DefineCraneAnimation(mgroupCrane[4], 114.5, 13.5);
	DefineCrane2Animation(mgroupCrane2[4], 38, 99.7);
	DefineSaturnAnimation(mgroupSaturn[15], meshindexSaturn[15], 114.5, 13.5);

	// Create animations
	animCraneCount = 5;
	for (i = 0; i < animCraneCount; i++) {
		animCrane[i] = CreateAnimation(0.0);
		AddAnimationComponent(animCrane[i], 0,        0.000005, mgroupCrane[i][0]);
		AddAnimationComponent(animCrane[i], 0,        0.000005, mgroupCrane[i][1]);
		AddAnimationComponent(animCrane[i], 0.000005, 0.00001,  mgroupCrane[i][2]);
		AddAnimationComponent(animCrane[i], 0.1876,   0.2,      mgroupCrane[i][3]);
		AddAnimationComponent(animCrane[i], 0.2,  0.3, mgroupCrane[i][4]);
		AddAnimationComponent(animCrane[i], 0.2,  0.3, mgroupCrane[i][5]);
		AddAnimationComponent(animCrane[i], 0.3,  0.4, mgroupCrane[i][6]);
		AddAnimationComponent(animCrane[i], 0.3,  0.4, mgroupCrane[i][7]);
		AddAnimationComponent(animCrane[i], 0.4,  0.6, mgroupCrane[i][8]);
		AddAnimationComponent(animCrane[i], 0.6,  0.7, mgroupCrane[i][9]);
		AddAnimationComponent(animCrane[i], 0.6,  0.7, mgroupCrane[i][10]); 
		AddAnimationComponent(animCrane[i], 0.7,  0.8, mgroupCrane[i][11]); 
		AddAnimationComponent(animCrane[i], 0.7,  0.8, mgroupCrane[i][12]); 
		AddAnimationComponent(animCrane[i], 0.8,  0.9, mgroupCrane[i][13]); 
		AddAnimationComponent(animCrane[i], 0.8,  0.9, mgroupCrane[i][14]); 
		AddAnimationComponent(animCrane[i], 0.9,  1.0, mgroupCrane[i][15]); 

		AddAnimationComponent(animCrane[i], 0,       0.00001, mgroupCrane2[i][0]);
		AddAnimationComponent(animCrane[i], 0,       0.00001, mgroupCrane2[i][1]);
		AddAnimationComponent(animCrane[i], 0.00001, 0.2,     mgroupCrane2[i][2]);
		AddAnimationComponent(animCrane[i], 0.2,     0.20272, mgroupCrane2[i][3]);
		AddAnimationComponent(animCrane[i], 0.2,     0.20272, mgroupCrane2[i][4]);
		AddAnimationComponent(animCrane[i], 0.20272, 0.233,   mgroupCrane2[i][5]);
	}

	int j = 0;
	for (i = 1; i < meshcountSaturn; i++) {
		if (i <= 2) j = 1;
		else if (i <= 4) j = 2;
		else if (i <= 14) j = 3;
		else if (i <= 15) j = 4;

		AddAnimationComponent(animCrane[j], 0,        0.000005, mgroupSaturn[i][0]);
		AddAnimationComponent(animCrane[j], 0.000005, 0.00001,  mgroupSaturn[i][1]);
		AddAnimationComponent(animCrane[j], 0.00001,  0.2,      mgroupSaturn[i][2]);
		AddAnimationComponent(animCrane[j], 0.3,      0.4,      mgroupSaturn[i][3]);
		AddAnimationComponent(animCrane[j], 0.4,      0.6,      mgroupSaturn[i][4]);
		AddAnimationComponent(animCrane[j], 0.6,      0.7,      mgroupSaturn[i][5]);
		AddAnimationComponent(animCrane[j], 0.7,      0.8,      mgroupSaturn[i][6]);
	}
	currentAnimCrane = animCrane[0];

	// Special handling Saturn Stage 1
	animSaturnStage1 = CreateAnimation(0.0);
	AddAnimationComponent(animSaturnStage1, 0,        0.00001, mgroupSaturn[0][0]);
	AddAnimationComponent(animSaturnStage1, 0.00001,  0.2,     mgroupSaturn[0][1]);
	ANIMATIONCOMPONENT_HANDLE ach = 
    AddAnimationComponent(animSaturnStage1, 0.3,      0.35,    mgroupSaturn[0][2]);
	AddAnimationComponent(animSaturnStage1, 0.3,      0.35,    mgroupSaturn[0][3], ach);
	AddAnimationComponent(animSaturnStage1, 0.35,     0.4,     mgroupSaturn[0][4]);
	AddAnimationComponent(animSaturnStage1, 0.4,      0.6,     mgroupSaturn[0][5]);
	AddAnimationComponent(animSaturnStage1, 0.6,      0.7,     mgroupSaturn[0][6]);
	AddAnimationComponent(animSaturnStage1, 0.7,      0.8,     mgroupSaturn[0][7]);

	// Platforms
	static UINT platform1_groups[1] = {1969};
	static MGROUP_TRANSLATE platform1 (0, platform1_groups, 1, _V(-7.75, 0, 0));
	static UINT platform2_groups[1] = {1977};
	static MGROUP_TRANSLATE platform2 (0, platform2_groups, 1, _V(7.75, 0, 0));

	animPlatform = CreateAnimation(0.0);
	AddAnimationComponent(animPlatform, 0, 1.0, &platform1);
	AddAnimationComponent(animPlatform, 0, 1.0, &platform2);
}

void VAB::DefineCraneAnimation(MGROUP_TRANSFORM *mgroup[], double pickupHeight, double releaseHeight) {

	// Crane
	static UINT crane1_groups[1] = {0};
	static UINT crane2_groups[2] = {1, 21};
	static UINT crane3_groups[4] = {0, 1, 8, 21};
	static UINT crane4_groups[6] = {0, 1, 8, 9, 10, 21};
	static UINT crane5_groups[1] = {0};
	static UINT crane6_groups[2] = {1, 21};
	static UINT crane7_groups[1] = {0};
	static UINT crane8_groups[2] = {1, 21};
	static UINT crane9_groups[6] = {0, 1, 8, 9, 10, 21};
	static UINT crane10_groups[1] = {0};
	static UINT crane11_groups[2] = {1, 21};
	static UINT crane12_groups[1] = {0};
	static UINT crane13_groups[2] = {1, 21};
	static UINT crane14_groups[1] = {0};
	static UINT crane15_groups[2] = {1, 21};
	static UINT crane16_groups[6] = {0, 1, 8, 9, 10, 21};

	mgroup[0]  = new MGROUP_SCALE     (0, crane1_groups,  1, _V(0,   70,    0), _V(1, 0.1, 1));
	mgroup[1]  = new MGROUP_TRANSLATE (0, crane2_groups,  2, _V(0,  122.7,  0));
	mgroup[2]  = new MGROUP_TRANSLATE (0, crane3_groups,  4, _V(-1.6, 0,  0));
	mgroup[3]  = new MGROUP_TRANSLATE (0, crane4_groups,  6, _V(0,    0, -6.2));
	mgroup[4]  = new MGROUP_SCALE     (0, crane5_groups,  1, _V(0,   70,  0), _V(1, (pickupHeight / 122.7) * 9.0 + 1.0, 1));
	mgroup[5]  = new MGROUP_TRANSLATE (0, crane6_groups,  2, _V(0,  -pickupHeight,    0));
	mgroup[6]  = new MGROUP_SCALE     (0, crane7_groups,  1, _V(0,   70,  0), _V(1, 1.0 / ((pickupHeight / 122.7) * 9.0 + 1.0), 1));
	mgroup[7]  = new MGROUP_TRANSLATE (0, crane8_groups,  2, _V(0,   pickupHeight,    0));
	mgroup[8]  = new MGROUP_TRANSLATE (0, crane9_groups,  6, _V(0,    0, 42.5));
	mgroup[9]  = new MGROUP_SCALE     (0, crane10_groups, 1, _V(0,   70,  0), _V(1, ((releaseHeight - 3.0) / 122.7) * 9.0 + 1.0, 1));
	mgroup[10] = new MGROUP_TRANSLATE (0, crane11_groups, 2, _V(0,  -(releaseHeight - 3.0), 0));
	mgroup[11] = new MGROUP_SCALE     (0, crane12_groups, 1, _V(0,   70,  0), _V(1, (releaseHeight + 13.63) / ((releaseHeight - 3.0) + 13.63), 1));
	mgroup[12] = new MGROUP_TRANSLATE (0, crane13_groups, 2, _V(0,   -3,  0));
	mgroup[13] = new MGROUP_SCALE     (0, crane14_groups, 1, _V(0,   70,  0), _V(1, 1.0 / ((releaseHeight / 122.7) * 9.0 + 1.0), 1));
	mgroup[14] = new MGROUP_TRANSLATE (0, crane15_groups, 2, _V(0,   releaseHeight, 0));
	mgroup[15] = new MGROUP_TRANSLATE (0, crane16_groups, 6, _V(0,    0, -36.3));
}

void VAB::DefineSaturnAnimation(MGROUP_TRANSFORM *mgroup[], int meshindex, double pickupHeight, double releaseHeight) {

	mgroup[0] = new MGROUP_ROTATE    (meshindex, NULL, 0, _V(0, 0, 0), _V(-1, 0, 0), (float)( 0.5 * PI));
	mgroup[1] = new MGROUP_ROTATE    (meshindex, NULL, 0, _V(0, 0, 0), _V( 0, 1, 0), (float)(-0.5 * PI));
	mgroup[2] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(-99.7, 0, 0));
	mgroup[3] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, pickupHeight, 0));
	mgroup[4] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, 0, 42.5));
	mgroup[5] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, -(releaseHeight - 3.0), 0));
	mgroup[6] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, -3, 0));
}

void VAB::DefineCrane2Animation(MGROUP_TRANSFORM *mgroup[], double pickupHeight, double length) {

	// Crane
	static UINT crane1_groups[1] = {4};
	static UINT crane2_groups[2] = {5, 23};
	static UINT crane3_groups[6] = {4, 5, 18, 19, 20, 23};
	static UINT crane4_groups[1] = {4};
	static UINT crane5_groups[2] = {5, 23};
	static UINT crane6_groups[6] = {4, 5, 18, 19, 20, 23};

	mgroup[0]  = new MGROUP_SCALE     (0, crane1_groups,  1, _V(0, -19.5, 0), _V(1, pickupHeight / 46.8, 1));
	mgroup[1]  = new MGROUP_TRANSLATE (0, crane2_groups,  2, _V(0, 46.8 - pickupHeight,  0));
	mgroup[2]  = new MGROUP_TRANSLATE (0, crane3_groups,  6, _V(-length, 0, 0));
	mgroup[3]  = new MGROUP_SCALE     (0, crane4_groups,  1, _V(0, -19.5, 0), _V(1, (pickupHeight - 3.0) / pickupHeight, 1));
	mgroup[4]  = new MGROUP_TRANSLATE (0, crane5_groups,  2, _V(0, 3, 0));
	mgroup[5]  = new MGROUP_TRANSLATE (0, crane6_groups,  6, _V(length, 0, 0));
}

void VAB::clbkPostCreation() {
	
	SetAnimation(currentAnimCrane, crane_Proc);
	SetAnimation(animPlatform, platform_Proc);
	SetAnimation(animSaturnStage1, saturnStage1_Proc);
}

void VAB::clbkPreStep(double simt, double simdt, double mjd) {

	if (!firstTimestepDone) DoFirstTimestep();
}

void VAB::clbkPostStep (double simt, double simdt, double mjd) {

	// High bay 1 door
	if (highBay1Door_Status == DOOR_CLOSING || highBay1Door_Status == DOOR_OPENING) {
		double da = simdt / 300.0;
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
		double da = simdt / 300.0;
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
		// Platforms
		if (currentAnimCrane == -1) {
			if (crane_Status == CRANE_BUILDING) {
				if (platform_Proc > 0) 
					platform_Proc = max(0.0, platform_Proc - simdt / 1000.0);
				else 
					crane_Status = CRANE_END;

			} else { // unbuilding
				if (platform_Proc < 1.0) 
					platform_Proc = min(1.0, platform_Proc + simdt / 1000.0);
				else 
					crane_Status = CRANE_BEGIN;
			} 
			SetAnimation(animPlatform, platform_Proc);

		} else {
			// Get the saturn
			Saturn *lav = 0;
			if (saturnName[0] != '\0') {
				OBJHANDLE hLV = oapiGetVesselByName(saturnName);
				if (hLV) {
					lav = (Saturn *) oapiGetVesselInterface(hLV);
				}
			}

			// Calculate animation speed
			double da;
			if (crane_Proc < 0.2) da = 0.2 * simdt / 2000.0;
			else if (crane_Proc < 0.4) da = 0.1 * simdt / 6600.0;
			else if (crane_Proc < 0.6) da = 0.2 * simdt / 850.0;
			else if (crane_Proc < 0.7) {
				if (currentAnimCrane == animCrane[0]) da = 0.1 * simdt / 4695.0;
				if (currentAnimCrane == animCrane[1]) da = 0.1 * simdt / 2910.0;
				if (currentAnimCrane == animCrane[2]) da = 0.1 * simdt / 1995.0;
				if (currentAnimCrane == animCrane[3]) da = 0.1 * simdt / 1140.0;
				if (currentAnimCrane == animCrane[4]) da = 0.1 * simdt / 630.0;
			} else if (crane_Proc < 0.8) da = 0.1 * simdt / 720.0;
			else if (crane_Proc < 0.9) {
				if (currentAnimCrane == animCrane[0]) da = 0.1 * simdt / (4695.0 + 180.0);
				if (currentAnimCrane == animCrane[1]) da = 0.1 * simdt / (2910.0 + 180.0);
				if (currentAnimCrane == animCrane[2]) da = 0.1 * simdt / (1995.0 + 180.0);
				if (currentAnimCrane == animCrane[3]) da = 0.1 * simdt / (1140.0 + 180.0);
				if (currentAnimCrane == animCrane[4]) da = 0.1 * simdt / (630.0 + 180.0);					
			} else da = 0.1 * simdt / 726.0;

			if (crane_Status == CRANE_BUILDING) {
				if (crane_Proc < 1.0)
					crane_Proc = min(1.0, crane_Proc + da);
				else
					crane_Status = CRANE_END;

				if (!saturnVisible && crane_Proc > 0.00001 && crane_Proc < 0.8 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_ALWAYS);
					saturnVisible = true;
				}
				
				if (saturnVisible && crane_Proc >= 0.8 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_NEVER);
					lav->LaunchVesselBuild();
					saturnVisible = false;
				} 

				// Platforms
				if (lav && lav->GetBuildStatus() == 1 && crane_Proc > 0.8 && platform_Proc < 1.0)
					platform_Proc = min(1.0, platform_Proc + simdt / 1000.0);

				// Saturn Stage 1
				if (currentAnimCrane == animCrane[0])
					saturnStage1_Proc = crane_Proc;

			} else { // unbuilding
				if (crane_Proc > 0.00001)
					crane_Proc = max(0.00001, crane_Proc - da);
				else
					crane_Status = CRANE_BEGIN;

				if (!saturnVisible && crane_Proc > 0.00001 && crane_Proc < 0.8 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus() - 1, MESHVIS_ALWAYS);
					lav->LaunchVesselUnbuild();
					saturnVisible = true;
				} 
			
				if (saturnVisible && crane_Proc <= 0.00001 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_NEVER);
					saturnVisible = false;
				}

				// Platforms
				if (lav && lav->GetBuildStatus() == 1 && crane_Proc < 0.9 && crane_Proc > 0.8 && platform_Proc > 0)
					platform_Proc = max(0, platform_Proc - simdt / 1000.0);

				// Saturn Stage 1
				if (currentAnimCrane == animCrane[0]) {
					if (adjustSaturnStage1) {
						if (saturnStage1_Proc <= 0.35) {
							saturnStage1_Proc += da / 3.9;
						} else {
							saturnStage1_Proc = 1.0;
							adjustSaturnStage1 = false;
						}
					} else {
						saturnStage1_Proc = crane_Proc;
					}
				}
			}
			SetAnimation(currentAnimCrane, crane_Proc);
			SetAnimation(animPlatform, platform_Proc);
			SetAnimation(animSaturnStage1, saturnStage1_Proc);
		}
	}
}

void VAB::SetSaturnMeshVisibilityMode(int buildStatus, WORD mode) {

	if (buildStatus == 0) {
		SetMeshVisibilityMode(meshindexSaturn[0], mode);
	}
	else if (buildStatus == 1) {
		SetMeshVisibilityMode(meshindexSaturn[1], mode);
		SetMeshVisibilityMode(meshindexSaturn[2], mode);
	}
	else if (buildStatus == 2) {
		SetMeshVisibilityMode(meshindexSaturn[3], mode);
		SetMeshVisibilityMode(meshindexSaturn[4], mode);
	}
	else if (buildStatus == 3) {
		for (int i = 5; i < 15; i++) 
			SetMeshVisibilityMode(meshindexSaturn[i], mode);
	}
	else if (buildStatus == 4) {
		SetMeshVisibilityMode(meshindexSaturn[15], mode);
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

