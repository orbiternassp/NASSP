/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LC34 vessel

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

#define ORBITER_MODULE

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"

#include "IUUmbilical.h"
#include "IU_ESE.h"
#include "LC34.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo LC34.log";


// States
#define STATE_PRELAUNCH			0
#define STATE_CMARM1			1
#define STATE_CMARM2			2
#define STATE_LIFTOFFSTREAM		3
#define STATE_LIFTOFF			4
#define STATE_POSTLIFTOFF		5


PARTICLESTREAMSPEC liftoffStreamSpec = {
	0, 
	12.0,	// size
	50,		// rate
	120.0,	// velocity
	0.5,	// velocity distribution
	6.0,	// lifetime
	4,		// growthrate
	0.4,	// atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.7, 0.7
};


DLLCLBK void InitModule(HINSTANCE hModule) {

	g_hDLL = hModule;
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new LC34(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (LC34*)vessel;
}


LC34::LC34(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	firstTimestepDone = false;
	LVName[0] = '\0';
	touchdownPointHeight = -0.01; // pad height
	hLV = 0;
	state = STATE_PRELAUNCH;
	abort = false;

	mssProc = 0;
	cmarmProc = 0;
	swingarmProc = 0;

	int i;
	for (i = 0; i < 2; i++) {
		liftoffStream[i] = NULL;
	}
	liftoffStreamLevel = 0;

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	//meshoffsetMSS = _V(0,0,0);

	IuUmb = new IUUmbilical(this);
	IuESE = new IU_ESE(IuUmb);
}

LC34::~LC34() {
	delete IuUmb;
	delete IuESE;
}

void LC34::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(100000);
	SetSize(120);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0,0,0);
    meshindexLUT = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\CanaveralLC34LUT"), &meshoffset);
	SetMeshVisibilityMode(meshindexLUT, MESHVIS_ALWAYS);

	meshoffset = _V(0.74, -0.18, 2.61);
    meshindexMSS = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\CanaveralLC34MSS"), &meshoffset);
	SetMeshVisibilityMode(meshindexMSS, MESHVIS_ALWAYS);

	DefineAnimations();

	liftoffStream[0] = AddParticleStream(&liftoffStreamSpec, _V(0, 7, 0), _V(-1, 0.4,  1), &liftoffStreamLevel);
	liftoffStream[1] = AddParticleStream(&liftoffStreamSpec, _V(0, 7, 0), _V( 1, 0.4, -1), &liftoffStreamLevel);

	SetTouchdownPointHeight(touchdownPointHeight);
}

void LC34::clbkPostCreation() {
	
	SetAnimation(mssAnim, mssProc);
	SetAnimation(cmarmAnim, cmarmProc);
	SetAnimation(swingarmAnim, swingarmProc);
}

void LC34::clbkPreStep(double simt, double simdt, double mjd) {

	Saturn *sat;

	if (!firstTimestepDone) DoFirstTimestep();

	if (hLV && !abort) {
		sat = (Saturn *)oapiGetVesselInterface(hLV);
		abort = sat->GetAbort();
	}

	switch (state) {
	case STATE_PRELAUNCH:
		if (abort) break; // Don't do anything if we have aborted.

		// Move MSS, no clue if 20min are OK?
		if (mssProc < 1) {
			mssProc = min(1.0, mssProc + simdt / (60.0 * 20.0));
			SetAnimation(mssAnim, mssProc);
		}

		// T-33min or later?
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		if (sat->GetMissionTime() > -3 * 3600)
		{
			sat->ActivatePrelaunchVenting();
		}

		if (sat->GetMissionTime() > -33 * 60) {
			mssProc = 1;
			SetAnimation(mssAnim, mssProc);
			state = STATE_CMARM1;
		}
		break;

	case STATE_CMARM1:
		if (abort) break; // Don't do anything if we have aborted.

		// Move CM arm 12 deg, no clue if 60s are OK?
		if (cmarmProc < 12.0 / 180.0 * 0.7) {
			cmarmProc = min(12.0 / 180.0 * 0.7, cmarmProc + simdt / 60.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}

		// T-5min or later?
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		sat->ActivatePrelaunchVenting();

		if (sat->GetMissionTime() > -5 * 60) {
			cmarmProc = 12.0 / 180.0 * 0.7;
			SetAnimation(cmarmAnim, cmarmProc);
			state = STATE_CMARM2;
		}  
		break;

	case STATE_CMARM2:
		if (abort) break; // Don't do anything if we have aborted.

		// Move CM arm to retracted position, no clue if 60s are OK?
		if (cmarmProc < 1) {
			cmarmProc = min(1.0, cmarmProc + simdt / 60.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}

		// T-4.9s or later?
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		if (sat->GetMissionTime() < -9)
		{
			sat->ActivatePrelaunchVenting();
		}
		else
		{
			sat->DeactivatePrelaunchVenting();
		}

		if (sat->GetMissionTime() > -4.9) {
			cmarmProc = 1;
			SetAnimation(cmarmAnim, cmarmProc);
			state = STATE_LIFTOFFSTREAM;
		}
		break;


	case STATE_LIFTOFFSTREAM:
		if (!hLV) break;
		sat = (Saturn *)oapiGetVesselInterface(hLV);

		if (sat->GetMissionTime() > -3.1)
		{
			sat->SetSIEngineStart(5);
			sat->SetSIEngineStart(7);
		}
		if (sat->GetMissionTime() > -3.0)
		{
			sat->SetSIEngineStart(6);
			sat->SetSIEngineStart(8);
		}
		if (sat->GetMissionTime() > -2.9)
		{
			sat->SetSIEngineStart(2);
			sat->SetSIEngineStart(4);
		}
		if (sat->GetMissionTime() > -2.8)
		{
			sat->SetSIEngineStart(1);
			sat->SetSIEngineStart(3);
		}

		// T-1s or later?
		if (sat->GetMissionTime() > -1) {
			state = STATE_LIFTOFF;
		}

		if (abort) break; // Don't do anything if we have aborted.

		if (sat->GetMissionTime() < -2.0)
			liftoffStreamLevel = sat->GetSIThrustLevel()*(sat->GetMissionTime() + 4.9) / 2.9;
		else
			liftoffStreamLevel = sat->GetSIThrustLevel();
		break;
	
	case STATE_LIFTOFF:
		if (!hLV) break;
		sat = (Saturn *)oapiGetVesselInterface(hLV);

		// Disconnect IU Umbilical
		if (sat->GetMissionTime() >= -0.05) {
			IuUmb->Disconnect();
		}

		// T+4s or later?
		if (sat->GetMissionTime() > 4) {
			state = STATE_POSTLIFTOFF;
		}

		if (abort) break; // Don't do anything if we have aborted.

		// Move swingarms
		if (swingarmProc < 1) {
			swingarmProc = min(1.0, swingarmProc + simdt / 4.0);
			SetAnimation(swingarmAnim, swingarmProc);
		}

		liftoffStreamLevel = sat->GetSIThrustLevel();
	break;

	case STATE_POSTLIFTOFF:

		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		//Cutoff
		if (sat->GetMissionTime() > 6.0 && sat->GetStage() <= PRELAUNCH_STAGE)
		{
			sat->SIGSECutoff(true);
		}

		if (sat->GetMissionTime() < 10.0 && !abort)
			liftoffStreamLevel = sat->GetSIThrustLevel()*(sat->GetMissionTime() - 10.0) / -6.0;
		else {
			liftoffStreamLevel = 0;

			//
			// Once the stream is finished, forget about the vessel since we won't be
			// using it again. This prevents a crash if we later delete the vessel.
			//
			hLV = 0;
		}
		break;
	}
}

void LC34::clbkPostStep (double simt, double simdt, double mjd) {

	if (!firstTimestepDone) return;

	// Nothing for now
}

void LC34::DoFirstTimestep() {

	char buffer[256];

	if (swingarmProc == 0.0)
	{
		double vcount = oapiGetVesselCount();
		for (int i = 0; i < vcount; i++) {
			OBJHANDLE h = oapiGetVesselByIndex(i);
			oapiGetObjectName(h, buffer, 256);
			if (!strcmp(LVName, buffer)) {
				hLV = h;
				Saturn *sat = (Saturn *)oapiGetVesselInterface(hLV);
				if (sat->GetStage() < LAUNCH_STAGE_ONE)
				{
					IuUmb->Connect(sat->GetIU());
				}
			}
		}
	}

	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	firstTimestepDone = true;
}

void LC34::SetTouchdownPointHeight(double height) {

	touchdownPointHeight = height;
	SetTouchdownPoints(_V(  0, touchdownPointHeight,  10), 
					   _V(-10, touchdownPointHeight, -10), 
					   _V( 10, touchdownPointHeight, -10));
}

void LC34::DefineAnimations() {

	// MSS
	static MGROUP_TRANSLATE mgroupMSS(meshindexMSS, NULL, 0, _V(136, 0, 150));	
	mssAnim = CreateAnimation(0.0);
	AddAnimationComponent(mssAnim, 0, 1, &mgroupMSS);

	// CM access arm
	static UINT cmarm_groups[2] = {16, 20};
	static MGROUP_ROTATE mgroupCMArm(meshindexLUT, cmarm_groups, 2, _V(-9.6, 0, -6.8), _V( 0, 1, 0), (float)(-0.7 * PI));
	cmarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(cmarmAnim, 0, 1, &mgroupCMArm);

	// Swingarms
	static UINT swingarm_groups1[2] = {17, 19};
	static UINT swingarm_groups2[2] = {18, 21};
	static UINT swingarm_groups3[2] = {2, 4};
	static UINT swingarm_groups4[2] = {0, 1};
	static MGROUP_ROTATE mgroupSwingarm1(meshindexLUT, swingarm_groups1, 2, _V(-9.3, 0, -6.5), _V( 0,  1, 0), (float)(75.0 / 180.0 * PI));
	static MGROUP_ROTATE mgroupSwingarm2(meshindexLUT, swingarm_groups2, 2, _V(-9.0, 0, -6.3), _V( 0,  1, 0), (float)(75.0 / 180.0 * PI));
	static MGROUP_ROTATE mgroupSwingarm3(meshindexLUT, swingarm_groups3, 2, _V(-8.6, 0, -6.0), _V( 0,  1, 0), (float)(75.0 / 180.0 * PI));
	static MGROUP_ROTATE mgroupSwingarm4(meshindexLUT, swingarm_groups4, 2, _V(-8.6, 0, -6.0), _V( 0, -1, 0), (float)(90.0 / 180.0 * PI));
	swingarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm1);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm2);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm3);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm4);
}

void LC34::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%i", &state);
		} else if (!strnicmp (line, "TOUCHDOWNPOINTHEIGHT", 20)) {
			sscanf (line + 20, "%lf", &touchdownPointHeight);
		} else if (!strnicmp (line, "MSSPROC", 7)) {
			sscanf (line + 7, "%lf", &mssProc);
		} else if (!strnicmp (line, "CMARMPROC", 9)) {
			sscanf (line + 9, "%lf", &cmarmProc);
		} else if (!strnicmp (line, "SWINGARMPROC", 12)) {
			sscanf (line + 12, "%lf", &swingarmProc);
		} else if (!strnicmp (line, "LVNAME", 6)) {
			strncpy (LVName, line + 7, 64);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
	SetTouchdownPointHeight(touchdownPointHeight);
}

void LC34::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "STATE", state);
	papiWriteScenario_double(scn, "TOUCHDOWNPOINTHEIGHT", touchdownPointHeight);
	papiWriteScenario_double(scn, "MSSPROC", mssProc);
	papiWriteScenario_double(scn, "CMARMPROC", cmarmProc);
	papiWriteScenario_double(scn, "SWINGARMPROC", swingarmProc);
	if (LVName[0])
		oapiWriteScenario_string(scn, "LVNAME", LVName);

}

int LC34::clbkConsumeDirectKey(char *kstate) {

	if (KEYMOD_SHIFT(kstate)) {
		return 0; 
	}

/*	DelMesh(meshindexMSS);

	double step = 0.01;
	if (KEYMOD_CONTROL(kstate))
		step = 0.001;

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {
		meshoffsetMSS.x -= step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {
		meshoffsetMSS.x += step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		meshoffsetMSS.y -= step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		meshoffsetMSS.y += step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD1)) {
		meshoffsetMSS.z -= step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		meshoffsetMSS.z += step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}

    meshindexMSS = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\CanaveralLC34MSS"), &meshoffsetMSS);
	SetMeshVisibilityMode(meshindexMSS, MESHVIS_ALWAYS);
	sprintf(oapiDebugString(), "x %f y %f z %f", meshoffsetMSS.x, meshoffsetMSS.y, meshoffsetMSS.z);
*/

/*	ATTACHMENTHANDLE ah = GetAttachmentHandle(true, 0);
	VECTOR3 pos, dir, rot;
	GetAttachmentParams (ah, pos, dir, rot);

	double step = 0.01;
	if (KEYMOD_CONTROL(kstate))
		step = 0.001;

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {
		pos.x -= step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {
		pos.x += step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		pos.y -= step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		pos.y += step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD1)) {
		pos.z -= step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		pos.z += step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}

	sprintf(oapiDebugString(), "x %f y %f z %f", pos.x, pos.y, pos.z);
*/
/*
	VESSELSTATUS vs;
	GetStatus(vs);
	double moveStep = 1.0e-7;
	if (KEYMOD_CONTROL(kstate))
		moveStep = 1.0e-9;

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		vs.vdata[0].x += moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {			
		vs.vdata[0].y -= moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);			
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {			
		vs.vdata[0].y += moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		vs.vdata[0].x -= moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD1)) {
		vs.vdata[0].z -= 1.0e-4;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		vs.vdata[0].z += 1.0e-4;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		SetTouchdownPoints (_V(0, -GetCOG_elev() + 0.01, 1), _V(-1, -GetCOG_elev() + 0.01, -1), _V(1, -GetCOG_elev() + 0.01, -1));
		RESETKEY(kstate, OAPI_KEY_A);
	}
	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		SetTouchdownPoints (_V(0, -GetCOG_elev() - 0.01, 1), _V(-1, -GetCOG_elev() - 0.01, -1), _V(1, -GetCOG_elev() - 0.01, -1));
		RESETKEY(kstate, OAPI_KEY_S);
	}
	sprintf(oapiDebugString(), "GetCOG_elev %f", GetCOG_elev());
*/

	return 0;
}

int LC34::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0;
	}
	return 0;
}

bool LC34::ESEGetCommandVehicleLiftoffIndicationInhibit()
{
	return IuESE->GetCommandVehicleLiftoffIndicationInhibit();
}

bool LC34::ESEGetAutoAbortInhibit()
{
	return IuESE->GetAutoAbortInhibit();
}

bool LC34::ESEGetGSEOverrateSimulate()
{
	return IuESE->GetOverrateSimulate();
}

bool LC34::ESEGetEDSPowerInhibit()
{
	return IuESE->GetEDSPowerInhibit();
}

bool LC34::ESEPadAbortRequest()
{
	return IuESE->GetEDSPadAbortRequest();
}

bool LC34::ESEGetThrustOKIndicateEnableInhibitA()
{
	return IuESE->GetThrustOKIndicateEnableInhibitA();
}

bool LC34::ESEGetThrustOKIndicateEnableInhibitB()
{
	return IuESE->GetThrustOKIndicateEnableInhibitB();
}

bool LC34::ESEEDSLiftoffInhibitA()
{
	return IuESE->GetEDSLiftoffInhibitA();
}

bool LC34::ESEEDSLiftoffInhibitB()
{
	return IuESE->GetEDSLiftoffInhibitB();
}

bool LC34::ESEAutoAbortSimulate()
{
	return IuESE->GetAutoAbortSimulate();
}

bool LC34::ESEGetSIBurnModeSubstitute()
{
	return IuESE->GetSIBurnModeSubstitute();
}