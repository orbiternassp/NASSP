/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2018

  LC37 vessel

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

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"

#include "IU_ESE.h"
#include "SCMUmbilical.h"
#include "SIB_ESE.h"
#include "IUUmbilical.h"
#include "LC37.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "sivbsystems.h"
#include "s1b.h"
#include "sivb.h"
#include "papi.h"
#include "RCA110A.h"

HINSTANCE g_hDLL;


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

	return new LC37(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (LC37*)vessel;
}


LC37::LC37(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	firstTimestepDone = false;
	SIBName[0] = '\0';
	SIVBName[0] = '\0';
	touchdownPointHeight = -0.01; // pad height
	state = STATE_PRELAUNCH;
	abort = false;
	LaunchMJD = 99999.9;
	MissionTime = 0.0;
	s1b = NULL;
	sivb = NULL;

	int i;
	for (i = 0; i < 2; i++) {
		liftoffStream[i] = NULL;
	}
	liftoffStreamLevel = 0;

	//meshoffsetMSS = _V(0,0,0);

	IuUmb = new IUUmbilical(this);
	IuESE = new IU_ESE(IuUmb, this);
	SCMUmb = new SCMUmbilical(this);
	SIBESE = new SIB_ESE(SCMUmb, this);
	rca110a = new RCA110AM(this);
}

LC37::~LC37() {
	delete IuUmb;
	delete IuESE;
	delete SCMUmb;
	delete SIBESE;
	delete rca110a;
}

void LC37::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(100000);
	SetSize(120);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	DefineAnimations();

	liftoffStream[0] = AddParticleStream(&liftoffStreamSpec, _V(0, 7, 0), _V(-1, 0.4,  1), &liftoffStreamLevel);
	liftoffStream[1] = AddParticleStream(&liftoffStreamSpec, _V(0, 7, 0), _V( 1, 0.4, -1), &liftoffStreamLevel);

	SetTouchdownPointHeight(touchdownPointHeight);
}

void LC37::clbkPostCreation()
{	
	char buffer[256];

	double vcount = oapiGetVesselCount();
	for (int i = 0; i < vcount; i++) {
		OBJHANDLE h = oapiGetVesselByIndex(i);
		oapiGetObjectName(h, buffer, 256);
		if (!strcmp(SIBName, buffer)) {
			s1b = (S1B *)oapiGetVesselInterface(h);
			SCMUmb->Connect(s1b->GetSIB());
		}
		else if (!strcmp(SIVBName, buffer)) {
			sivb = (SIVB *)oapiGetVesselInterface(h);
			IuUmb->Connect(sivb->GetIU());
		}
	}

	soundlib.InitSoundLib(this, SOUND_DIRECTORY);
}

void LC37::clbkPreStep(double simt, double simdt, double mjd)
{
	if (!firstTimestepDone) DoFirstTimestep();

	MissionTime = (oapiGetSimMJD() - LaunchMJD)*24.0*3600.0;

	switch (state) {
	case STATE_PRELAUNCH:
		if (abort) break; // Don't do anything if we have aborted.

		// T-3h or later?
		if (MissionTime > -3 * 3600)
		{
			//sat->ActivatePrelaunchVenting();
		}

		// T-33min or later?
		if (MissionTime > -33 * 60) {
			state = STATE_CMARM1;
		}
		break;

	case STATE_CMARM1:
		if (abort) break; // Don't do anything if we have aborted.

		//sat->ActivatePrelaunchVenting();

		// T-5min or later?
		if (MissionTime > -5 * 60) {
			state = STATE_CMARM2;
		}  
		break;

	case STATE_CMARM2:
		if (abort) break; // Don't do anything if we have aborted.

		//Enforce 1.0x time acceleration for GRR
		if (MissionTime >= -30.0 && oapiGetTimeAcceleration() > 1.0)
		{
			oapiSetTimeAcceleration(1.0);
		}

		//Send Prepare to Launch signal and then at T-17 seconds the GRR signal
		if (MissionTime >= -17.0)
		{
			IuESE->SetGuidanceReferenceRelease(true);
		}
		else
		{
			IuUmb->LVDCPrepareToLaunch();
		}

		if (MissionTime < -9)
		{
			//sat->ActivatePrelaunchVenting();
		}
		else
		{
			//sat->DeactivatePrelaunchVenting();
		}

		// T-4.9s or later?
		if (MissionTime > -4.9) {
			state = STATE_LIFTOFFSTREAM;
		}
		break;


	case STATE_LIFTOFFSTREAM:

		if (MissionTime > -3.1)
		{
			SCMUmb->SetEngineStart(5);
			SCMUmb->SetEngineStart(7);
		}
		if (MissionTime > -3.0)
		{
			SCMUmb->SetEngineStart(6);
			SCMUmb->SetEngineStart(8);
		}
		if (MissionTime > -2.9)
		{
			SCMUmb->SetEngineStart(2);
			SCMUmb->SetEngineStart(4);
		}
		if (MissionTime > -2.8)
		{
			SCMUmb->SetEngineStart(1);
			SCMUmb->SetEngineStart(3);
		}

		// T-1s or later?
		if (MissionTime > -1) {
			state = STATE_LIFTOFF;
		}

		if (abort) break; // Don't do anything if we have aborted.

		if (MissionTime < -2.0)
			liftoffStreamLevel = (MissionTime + 4.9) / 2.9;
		else
			liftoffStreamLevel = 1;

		if (s1b)
		{
			if (MissionTime > -2.0 && MissionTime < -1.0)
				liftoffStreamLevel = s1b->GetThrusterGroupLevel(THGROUP_MAIN) *(MissionTime + 2.0) / 1.0;
			else
				liftoffStreamLevel = s1b->GetThrusterGroupLevel(THGROUP_MAIN);
		}
		else
		{
			liftoffStreamLevel = 0.0;
		}

		//Hold-down force
		if (MissionTime > -4.0) {
			if (s1b) s1b->AddForce(_V(0, 0, -8. * 1008000), _V(0, 0, 0)); // Maintain hold-down lock
		}
		break;
	case STATE_LIFTOFF:
		//if (!hLV) break;
		//sat = (LEMSaturn *)oapiGetVesselInterface(hLV);

		// Disconnect IU Umbilical
		if (MissionTime >= -0.05) {
			// Activate liftoff circuit
			IuUmb->SetEDSLiftoffEnableA();
			IuUmb->SetEDSLiftoffEnableB();
			// Disconnect Umbilicals
			IuUmb->Disconnect();
			SCMUmb->Disconnect();
		}

		// T+4s or later?
		if (MissionTime > 4) {
			state = STATE_POSTLIFTOFF;
		}

		if (abort) break; // Don't do anything if we have aborted.

		// Soft-Release Pin Dragging
		if (MissionTime < 0.5) {
			double PinDragFactor = min(1.0, 1.0 - (MissionTime * 2.0));
			if (s1b) s1b->AddForce(_V(0, 0, -(8*1008000 * PinDragFactor)), _V(0, 0, 0));
		}
		if (s1b)
		{
			liftoffStreamLevel = s1b->GetThrusterGroupLevel(THGROUP_MAIN);
		}
		else
		{
			liftoffStreamLevel = 0;
		}
	break;

	case STATE_POSTLIFTOFF:

		//if (!hLV) break;
		//sat = (LEMSaturn *) oapiGetVesselInterface(hLV);
		if (s1b && MissionTime < 10.0 && !abort)
			liftoffStreamLevel = s1b->GetThrusterGroupLevel(THGROUP_MAIN)*(MissionTime - 10.0) / -6.0;
		else {
			liftoffStreamLevel = 0;

			//
			// Once the stream is finished, forget about the vessel since we won't be
			// using it again. This prevents a crash if we later delete the vessel.
			//
			SIBName[0] = '\0';
			SIVBName[0] = '\0';
			s1b = NULL;
			sivb = NULL;
		}
		break;
	}

	//IU ESE
	IuESE->Timestep(MissionTime, simdt);
	SIBESE->Timestep();
}

void LC37::clbkPostStep (double simt, double simdt, double mjd) {

	if (!firstTimestepDone) return;

	// Nothing for now
}

void LC37::DoFirstTimestep() {



	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	firstTimestepDone = true;
}

void LC37::SetTouchdownPointHeight(double height) {

	touchdownPointHeight = height;
	SetTouchdownPoints(_V(  0, touchdownPointHeight,  10), 
					   _V(-10, touchdownPointHeight, -10), 
					   _V( 10, touchdownPointHeight, -10));
}

void LC37::DefineAnimations() {

}

void LC37::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		papiReadScenario_double(line, "LAUNCHMJD", LaunchMJD);
		papiReadScenario_string(line, "SIBNAME", SIBName);
		papiReadScenario_string(line, "SIVBNAME", SIVBName);

		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%i", &state);
		} else if (!strnicmp (line, "TOUCHDOWNPOINTHEIGHT", 20)) {
			sscanf (line + 20, "%lf", &touchdownPointHeight);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
	SetTouchdownPointHeight(touchdownPointHeight);
}

void LC37::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "STATE", state);
	papiWriteScenario_double(scn, "TOUCHDOWNPOINTHEIGHT", touchdownPointHeight);
	if (SIBName[0])
		oapiWriteScenario_string(scn, "SIBNAME", SIBName);
	if (SIVBName[0])
		oapiWriteScenario_string(scn, "SIVBNAME", SIVBName);
	papiWriteScenario_double(scn, "LAUNCHMJD", LaunchMJD);
}

int LC37::clbkConsumeDirectKey(char *kstate) {

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

int LC37::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0;
	}
	return 0;
}

bool LC37::ESEGetCommandVehicleLiftoffIndicationInhibit()
{
	return IuESE->GetCommandVehicleLiftoffIndicationInhibit();
}

bool LC37::ESEGetExcessiveRollRateAutoAbortInhibit(int n)
{
	return IuESE->GetExcessiveRollRateAutoAbortInhibit(n);
}

bool LC37::ESEGetExcessivePitchYawRateAutoAbortInhibit(int n)
{
	return IuESE->GetExcessivePitchYawRateAutoAbortInhibit(n);
}

bool LC37::ESEGetTwoEngineOutAutoAbortInhibit(int n)
{
	return IuESE->GetTwoEngineOutAutoAbortInhibit(n);
}

bool LC37::ESEGetGSEOverrateSimulate(int n)
{
	return IuESE->GetOverrateSimulate(n);
}

bool LC37::ESEGetEDSPowerInhibit()
{
	return IuESE->GetEDSPowerInhibit();
}

bool LC37::ESEPadAbortRequest()
{
	return IuESE->GetEDSPadAbortRequest();
}

bool LC37::ESEGetThrustOKIndicateEnableInhibitA()
{
	return IuESE->GetThrustOKIndicateEnableInhibitA();
}

bool LC37::ESEGetThrustOKIndicateEnableInhibitB()
{
	return IuESE->GetThrustOKIndicateEnableInhibitB();
}

bool LC37::ESEEDSLiftoffInhibitA()
{
	return IuESE->GetEDSLiftoffInhibitA();
}

bool LC37::ESEEDSLiftoffInhibitB()
{
	return IuESE->GetEDSLiftoffInhibitB();
}

bool LC37::ESEGetEDSAutoAbortSimulate(int n)
{
	return IuESE->GetEDSAutoAbortSimulate(n);
}

bool LC37::ESEGetEDSLVCutoffSimulate(int n)
{
	return IuESE->GetEDSLVCutoffSimulate(n);
}

bool LC37::ESEGetSIBurnModeSubstitute()
{
	return IuESE->GetSIBurnModeSubstitute();
}

bool LC37::ESEGetGuidanceReferenceRelease()
{
	return IuESE->GetGuidanceReferenceRelease();
}

bool LC37::ESEGetQBallSimulateCmd()
{
	return IuESE->GetQBallSimulateCmd();
}

bool LC37::ESEGetSIThrustOKSimulate(int eng, int n)
{
	return SIBESE->GetSIThrustOKSimulate(eng, n);
}

void LC37::SLCCCheckDiscreteInput(RCA110A *c)
{
	c->SetInput(0, true);
	c->SetInput(1, false);
	c->SetInput(861, IuESE->GetFCCPowerIsOn());
}

bool LC37::SLCCGetOutputSignal(size_t n)
{
	return rca110a->GetOutputSignal(n);
}

void LC37::ConnectGroundComputer(RCA110A *c)
{
	rca110a->Connect(c);
}

void LC37::IssueSwitchSelectorCmd(int stage, int chan)
{
	IuUmb->SwitchSelector(stage, chan);
}