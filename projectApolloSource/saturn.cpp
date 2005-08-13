/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant, Matthias Müller

  ORBITER vessel module: generic Saturn base class

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
  *	Revision 1.32  2005/08/13 14:59:24  movieman523
  *	Added initial null implementation of CSM caution and warning system, and removed 'master alarm' flag from Saturn class.
  *	
  *	Revision 1.31  2005/08/13 14:21:36  movieman523
  *	Added beginnings of caution and warning system.
  *	
  *	Revision 1.30  2005/08/12 23:15:49  movieman523
  *	Added switches to update mission time display.
  *	
  *	Revision 1.29  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.28  2005/08/09 02:28:27  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.27  2005/08/08 20:32:58  movieman523
  *	Added initial support for offsetting the mission timer and event timer from MissionTime: the real timers could be adjusted using the switches on the control panel (which aren't wired up yet), and the event timer would reset to zero on an abort.
  *	
  *	Revision 1.26  2005/08/06 01:25:27  movieman523
  *	Added Realism variable to AGC and fixed a bug with the APOLLONO scenario entry in the saturn class.
  *	
  *	Revision 1.25  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.24  2005/08/05 13:04:25  tschachim
  *	Fixed panel initialization
  *	
  *	Revision 1.23  2005/08/01 19:07:47  movieman523
  *	Genericised code to deal with SM destruction on re-entry, and did some tidying up of Saturn 1b code.
  *	
  *	Revision 1.22  2005/07/31 11:59:41  movieman523
  *	Added first mixture ratio shift to Saturn 1b.
  *	
  *	Revision 1.21  2005/07/31 01:43:13  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.20  2005/07/30 16:09:28  tschachim
  *	Added systemsState for the internal systems
  *	
  *	Revision 1.19  2005/07/30 02:05:48  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.18  2005/07/29 23:05:38  movieman523
  *	Added Inertial Guidance Mode start time to scenario file.
  *	
  *	Revision 1.17  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.16  2005/07/19 16:21:56  tschachim
  *	Docking radar sound only for CSM_LEM_STAGE
  *	
  *	Revision 1.15  2005/06/06 12:02:45  tschachim
  *	New switches, PanelSwitchScenarioHandler
  *	
  *	Revision 1.14  2005/05/31 02:12:08  movieman523
  *	Updated pre-entry burn variables and wrote most of the code to handle them.
  *	
  *	Revision 1.13  2005/05/31 00:17:33  movieman523
  *	Added CSMACCEL variables for unmanned flights which made burns just before re-entry to raise velocity to levels similar to a return from the moon.
  *	
  *	Revision 1.12  2005/05/18 23:34:23  movieman523
  *	Added roughly correct masses for the various Saturn payloads.
  *	
  *	Revision 1.11  2005/05/02 12:55:02  tschachim
  *	PanelsdkLogFile test code
  *	
  *	Revision 1.10  2005/04/22 13:58:19  tschachim
  *	Introduced PanelSDK
  *	Some changes because of the new panels
  *	
  *	Revision 1.9  2005/04/14 23:12:43  movieman523
  *	Added post-splashdown audio support. Unfortunately I can't test this at the moment as the control panel switches for getting out of the CM after splashdown aren't working :).
  *	
  *	However, it's pretty simple code, so 90+% likely to work.
  *	
  *	Revision 1.8  2005/03/19 03:39:13  chode99
  *	Fixed bug that prevented some switch states from being set by the scenario file.
  *	
  *	Revision 1.7  2005/03/14 01:40:30  chode99
  *	Fixed the positions of the SIVB RCS thrusters in Saturn V and Saturn IB
  *	
  *	Revision 1.6  2005/03/13 21:17:20  chode99
  *	Added code to compute accurate axial g force for panel display.
  *	
  *	Revision 1.5  2005/03/09 00:26:15  chode99
  *	Added code to support SII retros.
  *	
  *	Revision 1.4  2005/03/03 17:57:37  tschachim
  *	panel handling
  *	
  *	Revision 1.3  2005/02/19 00:03:57  movieman523
  *	Reduced volume of APS sound playback.
  *	
  *	Revision 1.2  2005/02/18 00:43:07  movieman523
  *	Added new Apollo 13 sound support.
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"

//extern FILE *PanelsdkLogFile;

Saturn::Saturn(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel), agc(soundlib, dsky, imu), dsky(soundlib, agc), imu(agc), cws(SMasterAlarm)

{
	InitSaturnCalled = false;
	autopilot = false;
	LastTimestep = 0;

	// VESSELSOUND **********************************************************************
	// initialisation

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);
}

Saturn::~Saturn()

{
	//
	// Nothing for now.
	//
	//fclose(PanelsdkLogFile);
}

void Saturn::initSaturn()

{
	//
	// Default to the latest model Saturn
	//

	VehicleNo = 600;
	ApolloNo = 0;

	TCPO = 0.0;

	FirstTimestep = true;
	GenericFirstTimestep = true;
	TLICapableBooster = false;

	bAbort = false;
	ABORT_IND = false;
	LEM_DISPLAY=false;
	bManualUnDock = false;
	bManualSeparate = false;
	ASTPMission = false;
	probeOn = true;
	bToggleHatch = false;

	AutoSlow = false;
	Crewed = true;
	SIVBPayload = PAYLOAD_LEM;
	CSMSepSet = false;
	CSMSepTime = 0.0;

	CMSepSet = false;
	CMSepTime = 0.0;

	CSMAccelSet = false;
	CSMAccelTime = 0.0;
	CSMAccelEnd = 0.0;
	CSMAccelPitch = 0.0;

	SIVBBurn = false;
	SIVBBurnStart = 0.0;
	SIVBApogee = 0.0;

	CSMBurn = false;
	CSMBurnStart = 0.0;
	CSMApogee = 0.0;
	CSMPerigee = 0.0;

	HatchOpen = false;
	FIRSTCSM = false;
	ProbeJetison = false;
	LEMdatatransfer = false;
	PostSplashdownPlayed = false;

	stgSM = false;

	buildstatus = 8;
	ThrustAdjust = 1.0;
	DockAngle = 0;

	AtempP  = 0;
	AtempY  = 0;
	AtempR  = 0;

	StopRot = false;
	SMSep = false;
	bStartS4B = false;
	IGMEnabled = false;

	refSaturn1B = 0;
	refPREV = 0;
	AltitudePREV = 0;

	hstg1 = 0;
	hstg2 = 0;
	hintstg = 0;
	hesc1 = 0;
	hPROBE = 0;
	hs4bM = 0;
	hs4b1 = 0;
	hs4b2 = 0;
	hs4b3 = 0;
	hs4b4 = 0;
	habort = 0;
	hSMJet = 0;

	//
	// Default mission time to an hour prior to launch.
	//

	MissionTime = (-3600);
	NextMissionEventTime = 0;
	LastMissionEventTime = 0;

	abortTimer = 0;
	release_time = 0;
	ignition_SMtime = 0;

	//
	// Defaults.
	//

	agc.SetDesiredApogee(220);
	agc.SetDesiredPerigee(215);
	agc.SetDesiredAzimuth(45);
	agc.SetVesselStats(SPS_ISP, SPS_THRUST, false);

	IGMStartTime = 196.0;

	//
	// Typical center engine shutdown times.
	//

	FirstStageCentreShutdownTime = 135.0;
	SecondStageCentreShutdownTime = 460.0;

	//
	// PU shift time. Default to 8:15
	//

	SecondStagePUShiftTime = 495.0;

	//
	// Configure AGC and DSKY.
	//

	agc.ControlVessel(this);
	imu.SetVessel(this);
	dsky.Init();

	//
	// Default masses.
	//

	CM_EmptyMass = 5440;
	CM_FuelMass = 123;

	SM_EmptyMass = 3110;
	SM_FuelMass = 18413 + 3000;

	S4PL_Mass = 14696;

	Abort_Mass = 4050;

	//
	// Propellant handles.
	//

	ph_1st = 0;
	ph_2nd = 0;
	ph_3rd = 0;
	ph_rcs0 = 0;
	ph_rcs1 = 0;
	ph_sps = 0;
	ph_retro1 = 0;
	ph_retro2 = 0;

	//
	// Thruster groups.
	//

	thg_main = 0;
	thg_ull = 0;
	thg_ver = 0;
	thg_retro1 = 0;
	thg_retro2 = 0;
	thg_aps = 0;

	//
	// Random virtual cockpit motion.
	//

	ViewOffsetx = 0;
	ViewOffsety = 0;
	ViewOffsetz = 0;

	InVC = false;
	InPanel = false;

	//
	// Save the last view offset set.
	//

	CurrentViewOffset = 0;

	//
	// Zeroing arrays for safety.
	//

	int i;

	for (i = 1; i < 6; i++){
		ClearEngineIndicator(i);
	}

	ClearAutopilotLight();

	for (i = 0; i < 8; i++) {
		LAUNCHIND[i] = false;
	}

	for (i = 0; i < nsurf; i++) {
		srf[i] = 0;
	}

	for (i = 0; i < 5; i++) {
		th_main[i] = 0;
	}

	for (i = 0; i < 8; i++) {
		th_ull[i] = 0;
	}

	for (i = 0; i < 3; i++) {
		th_ver[i] = 0;
	}

	for (i = 0; i < 12; i++) {
		th_att_cm[i] = 0;
	}

	for (i = 0; i < 4; i++) {
		th_retro1[i] = 0;
	}

	for (i = 0; i < 4; i++) {
		th_retro2[i] = 0;
	}

	th_sps[0] = 0;

	for (i = 0; i < 24; i++) {
		th_att_rot[i] = 0;
		th_att_lin[i] = 0;
	}

	UseATC = false;
	Realism = REALISM_DEFAULT;

	SIISepState = false;
	autoDISP = false;
	bRecovery = false;
	ActivateLEM = false;
	ActivateS4B = false;
	RCS_Full = true;
	ReadyAstp = false;
	Abort_Locked = false;

	stage = 0;
	dockstate = 0;

	KEY1=false;
	KEY2=false;
	KEY3=false;
	KEY4=false;
	KEY5=false;
	KEY6=false;
	KEY7=false;
	KEY8=false;
	KEY9=false;

	actualVEL = 0;
	actualALT = 0;
	actualFUEL = 0;
	aVAcc = 0;
	aVSpeed = 0;
	aHAcc = 0;
	aZAcc = 0;
	ALTN1 = 0;
	SPEEDN1 = 0;
	VSPEEDN1 = 0;
	XSPEEDN1 = 0;
	YSPEEDN1 = 0;
	ZSPEEDN1 = 0;
	aTime = 0;

	viewpos = SATVIEW_CDR;

	probeidx = 0;
	ToggleEva = false;
	ActivateASTP = false;

	//
	// LM landing data.
	//

	LMLandingLatitude = 0;
	LMLandingLongitude = 0;
	LMLandingAltitude = 0;

	//
	// Earth landing data.
	//

	EarthLandingLatitude = 0;
	EarthLandingLongitude = 0;

	//
	// Load textures that are used a lot. Should these be global
	// variables?
	//

	SMExhaustTex = oapiRegisterExhaustTexture ("Exhaust_atrcs");
	SMMETex = oapiRegisterExhaustTexture ("Exhaust_atsme");//"Exhaust2"
	CMTex =oapiRegisterReentryTexture("reentry");

	strncpy(AudioLanguage, "English", 64);

	hEVA = 0;

	//
	// Timestep tracking for debugging.
	//

	LongestTimestep = 0;
	LongestTimestepLength = 0.0;
	CurrentTimestep = 0;

	// call only once 
	if (!InitSaturnCalled) {

		// Initialize the panel
		PanelId = SATPANEL_MAIN; 		// default panel
		InitSwitches();

		// "dummy" SetSwitches to enable the panel event handling
		SetSwitches(PanelId);

		// Initialize the internal systems
		SystemsInit();
	}

	InitSaturnCalled = true;
}

bool Saturn::SIVBStart()

{
	return false;
}

void Saturn::SIVBStop()

{
}

void Saturn::UpdateLaunchTime(double t)

{
	if (t < 0)
		return;

	if (MissionTime < 0) {
		MissionTime = (-t);
	}
}

//
// Pitch program.
//

double Saturn::GetCPitch(double t)
{
	int i = 1;

	//
	// Make sure we don't run off the end.
	//

	if (t>met[PITCH_TABLE_SIZE - 1]) return cpitch[PITCH_TABLE_SIZE - 1];

	//
	// Find the first MET that's greater than our current time.
	//

	while (met[i]<t) i++;

	//
	// And calculate pitch as appropriate between those two times.
	//

	return cpitch[i-1]+(cpitch[i]-cpitch[i-1])/(met[i]-met[i-1])*(t-met[i-1]);
}

double Saturn::SetPitchApo()

{
	double cpitch;
	double SatApo1;
	double DiffApo;

	GetApDist(SatApo1);
	DiffApo = SatApo1 - ((agc.GetDesiredApogee()+ERADIUS)*1000);
	cpitch=(GetPitch()*DEG)-aVAcc-(aVSpeed/50)+((agc.GetDesiredApogee()*1000-GetAltitude())/8000) ;

	if (cpitch>35){
		cpitch=35;
	}
	else if(cpitch<-35){
		cpitch=-35;
	}

	//sprintf(oapiDebugString(), "diff %f", cpitch );
	return cpitch;
}

void Saturn::KillDist(OBJHANDLE &hvessel)

{
	VECTOR3 posr  = {0,0,0};
	double dist = 0.0;

	oapiGetRelativePos (GetHandle() ,hvessel, &posr);
	dist = (posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);

	if (dist > (5000 * 5000)){
		oapiDeleteVessel(hvessel, GetHandle());
		hvessel = 0;
	}
}

void Saturn::KillAlt(OBJHANDLE &hvessel, double altVS)

{
	int getit = oapiGetAltitude(hvessel, &altVS);

	if ((altVS < 90) && altVS >= 0 ){
		oapiDeleteVessel(hvessel, GetHandle());
		hvessel = 0;
	}
}


void Saturn::clbkDockEvent(int dock, OBJHANDLE connected)
{

}

void Saturn::clbkPreStep(double simt, double simdt, double mjd)

{
	//
	// You'll also die horribly if you set time acceleration at all in the
	// early parts of the launch.
	//

	if (stage == LAUNCH_STAGE_ONE && MissionTime < 50 &&
		oapiGetTimeAcceleration() > 1.0) {
		oapiSetTimeAcceleration(1.0);
	}
}

void Saturn::clbkPostStep (double simt, double simdt, double mjd)

{
	Timestep(simt);
}

void Saturn::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	int i = 1;

	oapiWriteScenario_int (scn, "NASSPVER", NASSP_VERSION);
	oapiWriteScenario_int (scn, "STAGE", stage);
	oapiWriteScenario_int(scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "APOLLONO", ApolloNo);
	oapiWriteScenario_int (scn, "DOCKSTATE", dockstate);
	oapiWriteScenario_int (scn, "PANEL_ID", PanelId);
	oapiWriteScenario_float (scn, "TOAPO", agc.GetDesiredApogee());
	oapiWriteScenario_float (scn, "TOPER", agc.GetDesiredPerigee());
	oapiWriteScenario_float (scn, "TOHDG", agc.GetDesiredAzimuth());
	oapiWriteScenario_float (scn, "TCP", TCPO);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "MTD", MissionTimerDisplay.GetTime()); // MUST FOLLOW MISSION TIME: see comments in LoadState();
	oapiWriteScenario_float (scn, "ETDO", EventTimerOffset);

	if (Realism != REALISM_DEFAULT) {
		oapiWriteScenario_int (scn, "REALISM", Realism);
	}

	if (buildstatus < 8) {
		oapiWriteScenario_int (scn, "BUILDSTATUS", buildstatus);
	}

	oapiWriteScenario_int(scn, "STAGESTATUS", StageState);

	if (LEM_DISPLAY) {
		oapiWriteScenario_int (scn, "LEM_DISPLAY", int(LEM_DISPLAY));
	}

	oapiWriteScenario_int (scn, "MAINSTATE",  GetMainState());
	oapiWriteScenario_int (scn, "LIGHTSTATE",  GetLightState());
	oapiWriteScenario_int (scn, "CSWITCH",  GetCSwitchState());
	oapiWriteScenario_int (scn, "SSWITCH",  GetSSwitchState());
	oapiWriteScenario_int (scn, "LPSWITCH",  GetLPSwitchState());
	oapiWriteScenario_int (scn, "RPSWITCH",  GetRPSwitchState());
	oapiWriteScenario_int (scn, "CPSWITCH", GetCPSwitchState());
	oapiWriteScenario_int (scn, "CP2SWITCH", GetCP2SwitchState());
	oapiWriteScenario_int (scn, "CP3SWITCH", GetCP3SwitchState());
	oapiWriteScenario_int (scn, "CP4SWITCH", GetCP4SwitchState());
	oapiWriteScenario_int (scn, "CP5SWITCH", GetCP5SwitchState());

	if (stage < LAUNCH_STAGE_ONE)
		oapiWriteScenario_int (scn, "PRELAUNCHATC",  int(UseATC));

	if (stage < LAUNCH_STAGE_TWO)
		oapiWriteScenario_float (scn, "SICSHUT", FirstStageCentreShutdownTime);

	if (stage < LAUNCH_STAGE_SIVB) {
		oapiWriteScenario_float (scn, "SIICSHUT", SecondStageCentreShutdownTime);
		oapiWriteScenario_float (scn, "SIIPUT", SecondStagePUShiftTime);
	}

	if (stage < STAGE_ORBIT_SIVB) {
		char fname[64];

		//
		// Save pitch program.
		//

		for (i = 0; i < PITCH_TABLE_SIZE; i++) {
			sprintf(fname, "PMET%03d", i);
			oapiWriteScenario_float (scn, fname, met[i]);
		}

		for (i = 0; i < PITCH_TABLE_SIZE; i++) {
			sprintf(fname, "CPITCH%03d", i);
			oapiWriteScenario_float (scn, fname, cpitch[i]);
		}

		//
		// IGM start time.
		//

		oapiWriteScenario_float (scn, "IGMST", IGMStartTime);
	}

	if (stage < CM_STAGE) {
		oapiWriteScenario_float (scn, "SMFUELLOAD", SM_FuelMass);
		oapiWriteScenario_float (scn, "SMMASS", SM_EmptyMass);
	}

	oapiWriteScenario_float (scn, "CMFUELLOAD", CM_FuelMass);
	oapiWriteScenario_float (scn, "CMMASS", CM_EmptyMass);

	if (!LEMdatatransfer && isTLICapable()) {
		oapiWriteScenario_float (scn, "MOONLAT", LMLandingLatitude);
		oapiWriteScenario_float (scn, "MOONLONG", LMLandingLongitude);
		oapiWriteScenario_float (scn, "MOONALT", LMLandingAltitude);
	}

	if (!Crewed) {
		oapiWriteScenario_int (scn, "UNMANNED", 1);

		if (CSMSepSet && (CSMSepTime >= MissionTime))
		{
			oapiWriteScenario_float (scn, "CSMSEP", CSMSepTime);
		}

		if (CMSepSet && (CMSepTime >= MissionTime))
		{
			oapiWriteScenario_float (scn, "CMSEP", CMSepTime);
		}

		if (SIVBBurn && (stage <= CSM_LEM_STAGE)) {
			oapiWriteScenario_float (scn, "S4APO", SIVBApogee);
			oapiWriteScenario_float (scn, "S4BURN", SIVBBurnStart);
		}

		if (CSMBurn && (stage <= CM_STAGE)) {
			oapiWriteScenario_float (scn, "CSMAPO", CSMApogee);
			oapiWriteScenario_float (scn, "CSMPER", CSMPerigee);
			oapiWriteScenario_float (scn, "CSMBURN", CSMBurnStart);
		}

		if (CSMAccelSet && (stage <= CM_STAGE))
		{
			oapiWriteScenario_float (scn, "CSMACCEL", CSMAccelTime);
			oapiWriteScenario_float (scn, "CSMACCEND", CSMAccelEnd);
			oapiWriteScenario_float (scn, "CSMACCPITCH", CSMAccelPitch);
		}

		if (AutoSlow) {
			oapiWriteScenario_int (scn, "AUTOSLOW", 1);
		}
	}

	if (SIVBPayload != PAYLOAD_LEM) {
		oapiWriteScenario_int (scn, "S4PL", SIVBPayload);
	}

	oapiWriteScenario_string (scn, "LANG", AudioLanguage);

	dsky.SaveState(scn);
	agc.SaveState(scn);
	imu.SaveState(scn);
	cws.SaveState(scn);

	// save the internal systems 
	oapiWriteScenario_int (scn, "SYSTEMSSTATE", systemsState);
	Panelsdk.Save(scn);	

	// save the state of the switches
	PSH.SaveState(scn);	
}

//
// Scenario state functions.
//

typedef union {
	struct {
		unsigned MissionTimerRunning:1;
		unsigned SIISepState:1;
		unsigned autopilot:1;
		unsigned TLIBurnDone:1;
		unsigned Scorrec:1;
		unsigned ApolloExploded:1;
		unsigned Burned:1;
		unsigned EVA_IP:1;
		unsigned ABORT_IND:1;
		unsigned HatchOpen:1;
		unsigned CryoStir:1;
		unsigned viewpos:2;
		unsigned LEMdatatransfer:1;
		unsigned KranzPlayed:1;
		unsigned PostSplashdownPlayed:1;
		unsigned IGMEnabled:1;
		unsigned stgSM:1;
	} u;
	unsigned long word;
} MainState;


int Saturn::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.MissionTimerRunning = MissionTimerDisplay.IsRunning();
	state.u.SIISepState = SIISepState;
	state.u.autopilot = autopilot;
	state.u.TLIBurnDone = TLIBurnDone;
	state.u.Scorrec = Scorrec;
	state.u.ApolloExploded = ApolloExploded;
	state.u.Burned = Burned;
	state.u.EVA_IP = EVA_IP;
	state.u.ABORT_IND = ABORT_IND;
	state.u.HatchOpen = HatchOpen;
	state.u.CryoStir = CryoStir;
	state.u.viewpos = viewpos;
	state.u.LEMdatatransfer = LEMdatatransfer;
	state.u.KranzPlayed = KranzPlayed;
	state.u.PostSplashdownPlayed = PostSplashdownPlayed;
	state.u.IGMEnabled = IGMEnabled;
	state.u.stgSM = stgSM;

	return state.word;
}

void Saturn::SetMainState(int s)

{
	MainState state;

	state.word = s;
	SIISepState = state.u.SIISepState;
	autopilot = state.u.autopilot;
	TLIBurnDone = state.u.TLIBurnDone;
	Scorrec = state.u.Scorrec;
	ApolloExploded = state.u.ApolloExploded;
	Burned = state.u.Burned;
	EVA_IP = state.u.EVA_IP;
	ABORT_IND = state.u.ABORT_IND;
	HatchOpen = state.u.HatchOpen;
	CryoStir = state.u.CryoStir;
	viewpos = state.u.viewpos;
	LEMdatatransfer = state.u.LEMdatatransfer;
	KranzPlayed = (state.u.KranzPlayed != 0);
	PostSplashdownPlayed = (state.u.PostSplashdownPlayed != 0);
	IGMEnabled = (state.u.IGMEnabled != 0);
	stgSM = (state.u.stgSM != 0);
	MissionTimerDisplay.SetRunning(state.u.MissionTimerRunning != 0);
}


typedef union {
	struct {
		unsigned Engind0:1;
		unsigned Engind1:1;
		unsigned Engind2:1;
		unsigned Engind3:1;
		unsigned Engind4:1;
		unsigned Engind5:1;
		unsigned Autopilot:1;
		unsigned Launchind0:1;
		unsigned Launchind1:1;
		unsigned Launchind2:1;
		unsigned Launchind3:1;
		unsigned Launchind4:1;
		unsigned Launchind5:1;
		unsigned Launchind6:1;
		unsigned Launchind7:1;
	} u;
	unsigned long word;
} LightState;

int Saturn::GetLightState()

{
	LightState state;

	state.word = 0;
	state.u.Engind0 = ENGIND[0];
	state.u.Engind1 = ENGIND[1];
	state.u.Engind2 = ENGIND[2];
	state.u.Engind3 = ENGIND[3];
	state.u.Engind4 = ENGIND[4];
	state.u.Engind5 = ENGIND[5];
	state.u.Autopilot = AutopilotLight;
	state.u.Launchind0 = LAUNCHIND[0];
	state.u.Launchind1 = LAUNCHIND[1];
	state.u.Launchind2 = LAUNCHIND[2];
	state.u.Launchind3 = LAUNCHIND[3];
	state.u.Launchind4 = LAUNCHIND[4];
	state.u.Launchind5 = LAUNCHIND[5];
	state.u.Launchind6 = LAUNCHIND[6];
	state.u.Launchind7 = LAUNCHIND[7];

	return state.word;
}

void Saturn::SetLightState(int s)

{
	LightState state;

	state.word = s;
	ENGIND[0] = (state.u.Engind0 != 0);
	ENGIND[1] = (state.u.Engind1 != 0);
	ENGIND[2] = (state.u.Engind2 != 0);
	ENGIND[3] = (state.u.Engind3 != 0);
	ENGIND[4] = (state.u.Engind4 != 0);
	ENGIND[5] = (state.u.Engind5 != 0);
	AutopilotLight = (state.u.Autopilot != 0);
	LAUNCHIND[0] = (state.u.Launchind0 != 0);
	LAUNCHIND[1] = (state.u.Launchind1 != 0);
	LAUNCHIND[2] = (state.u.Launchind2 != 0);
	LAUNCHIND[3] = (state.u.Launchind3 != 0);
	LAUNCHIND[4] = (state.u.Launchind4 != 0);
	LAUNCHIND[5] = (state.u.Launchind5 != 0);
	LAUNCHIND[6] = (state.u.Launchind6 != 0);
	LAUNCHIND[7] = (state.u.Launchind7 != 0);
}

void Saturn::GetScenarioState (FILEHANDLE scn, void *vstatus)

{
    char VNameApollo[256];
	char *line;
	float ftcp;
	double autopTime;
	int SwitchState = 0;
	int nasspver = 0, status = 0;
	int n;
	double tohdg = 45;

	//
	// If the name of the spacecraft is "AS-xxx" then get the vehicle
	// number from that.
	//

	strncpy (VNameApollo, GetName(), 255);
	if (!strnicmp (VNameApollo, "AS-", 3)) {
		sscanf(VNameApollo+3, "%d", &n);
		if (n > 500) {
			VehicleNo = n;
			SetVehicleStats();
		}
	}

	//
	// Seed the random number generator with whatever junk we can
	// find.
	//

	srand(VehicleNo + (int) vstatus);

	while (oapiReadScenario_nextline (scn, line)) {
        if (!strnicmp (line, "CONFIGURATION", 13)) {
            sscanf (line+13, "%d", &status);
		}
		else if (!strnicmp (line, "NASSPVER", 8)) {
			sscanf (line + 8, "%d", &nasspver);
		}
		else if (!strnicmp (line, "TOALT", 5)) {
            int toalt;
			sscanf (line+5, "%d", &toalt);
			agc.SetDesiredApogee(toalt);
			agc.SetDesiredPerigee(toalt * 0.98);
		}
		else if (!strnicmp (line, "TOAPO", 5)) {
            sscanf (line+5, "%f", &ftcp);
			agc.SetDesiredApogee(ftcp);
		}
		else if (!strnicmp (line, "TOPER", 5)) {
            sscanf (line+5, "%f", &ftcp);
			agc.SetDesiredPerigee(ftcp);
		}
		else if (!strnicmp (line, "BUILDSTATUS", 11)) {
			sscanf (line+11, "%d", &buildstatus);
		}else if (!strnicmp (line, "PANEL_ID", 8)) {
			sscanf (line+8, "%d", &PanelId);
		}else if (!strnicmp (line, "STAGESTATUS", 11)) {
			sscanf (line+11, "%d", &StageState);
		}
		else if (!strnicmp (line, "DOCKANGLE", 9)) {
			sscanf (line+5, "%g", &ftcp);
			DockAngle = ftcp;
		}
		else if (!strnicmp (line, "STAGE", 5)) {
			sscanf (line+5, "%d", &stage);
		}
		else if (!strnicmp (line, "TOHDG", 5)) {
			sscanf (line+5, "%g", &ftcp);
			tohdg = ftcp;
		}
		else if (!strnicmp (line, "VECHNO", 6)) {
            int numb;
			sscanf (line+6, "%d", &numb);
			VehicleNo = numb;
			SetVehicleStats();
		}
		else if (!strnicmp (line, "TCP", 3)) {
            sscanf (line+3, "%f", &ftcp);
			TCPO=ftcp;
		}
		else if (!strnicmp (line, "SICSHUT", 7)) {
			sscanf (line + 7, "%f", &ftcp);
			FirstStageCentreShutdownTime = ftcp;
		}
		else if (!strnicmp (line, "SIICSHUT", 8)) {
			sscanf (line + 8, "%f", &ftcp);
			SecondStageCentreShutdownTime = ftcp;
		}
		else if (!strnicmp (line, "SIIPUT", 6)) {
			sscanf (line + 6, "%f", &ftcp);
			SecondStagePUShiftTime = ftcp;
		}
		else if (!strnicmp (line, "IGMST", 5)) {
			sscanf (line + 5, "%f", &ftcp);
			IGMStartTime = ftcp;
		}
		else if (!strnicmp (line, "LEM_DISPLAY", 11)) {
			LEM_DISPLAY = true;
		}
		else if (!strnicmp (line, "MAINSTATE", 9)) {
            SwitchState = 0;
			sscanf (line+9, "%d", &SwitchState);
			SetMainState(SwitchState);
		}
		else if (!strnicmp (line, "LIGHTSTATE", 10)) {
            SwitchState = 0;
			sscanf (line+10, "%d", &SwitchState);
			SetLightState(SwitchState);
		}
		else if (!strnicmp (line, "REALISM", 7)) {
			sscanf (line+7, "%d", &Realism);
		}
		else if (!strnicmp (line, "CSWITCH", 7)) {
            SwitchState = 0;
			sscanf (line+7, "%d", &SwitchState);
			SetCSwitchState(SwitchState);
		}
		else if (!strnicmp (line, "SSWITCH", 7)) {
            SwitchState = 0;
			sscanf (line+7, "%d", &SwitchState);
			SetSSwitchState(SwitchState);
		}
		else if (!strnicmp (line, "LPSWITCH", 8)) {
            SwitchState = 0;
			sscanf (line+8, "%d", &SwitchState);
			SetLPSwitchState(SwitchState);
		}
		else if (!strnicmp (line, "RPSWITCH", 8)) {
            SwitchState = 0;
			sscanf (line+8, "%d", &SwitchState);
			SetRPSwitchState(SwitchState);
		}
		else if (!strnicmp (line, "APOLLONO", 8)) {
			sscanf (line+8, "%d", &ApolloNo);
		}
		else if (!strnicmp (line, "CPSWITCH", 8)) {
            SwitchState = 0;
			sscanf (line+8, "%d", &SwitchState);
			SetCPSwitchState(SwitchState);
		}
		else if (!strnicmp (line, "CP2SWITCH", 9)) {
            SwitchState = 0;
			sscanf (line+9, "%d", &SwitchState);
			SetCP2SwitchState(SwitchState);
		}
		else if (!strnicmp (line, "CP3SWITCH", 9)) {
            SwitchState = 0;
			sscanf (line+9, "%d", &SwitchState);
			SetCP3SwitchState(SwitchState);
		}
		else if (!strnicmp (line, "CP4SWITCH", 9)) {
            SwitchState = 0;
			sscanf (line+9, "%d", &SwitchState);
			SetCP4SwitchState(SwitchState);
		}
		else if (!strnicmp (line, "CP5SWITCH", 9)) {
            SwitchState = 0;
			sscanf (line+9, "%d", &SwitchState);
			SetCP5SwitchState(SwitchState);
		}
		else if (!strnicmp (line, "DOCKSTATE", 9)) {
            sscanf (line+9, "%d", &dockstate);
		}
		else if (!strnicmp (line, "AUTOTIMER", 9)) {
            sscanf (line+9, "%f", &ftcp);
			autopTime = ftcp;
		}
		else if (!strnicmp(line, "MISSNTIME", 9)) {
            sscanf (line+9, "%f", &ftcp);
			MissionTime = ftcp;
		}
		else if (!strnicmp(line, "MTD", 3)) {
            sscanf (line + 3, "%f", &ftcp);
			MissionTimerDisplay.SetTime(ftcp);
		}
		else if (!strnicmp(line, "ETDO", 4)) {
            sscanf (line + 4, "%f", &ftcp);
			EventTimerOffset = ftcp;
		}
		else if (!strnicmp(line, "NMISSNTIME", 10)) {
            sscanf (line+10, "%f", &ftcp);
			NextMissionEventTime = ftcp;
		}
		else if (!strnicmp(line, "LMISSNTIME", 10)) {
            sscanf (line+10, "%f", &ftcp);
			LastMissionEventTime = ftcp;
		}
		else if (!strnicmp(line, "SIFUELMASS", 10)) {
            sscanf (line+10, "%f", &ftcp);
			SI_FuelMass = ftcp;
		}
		else if (!strnicmp(line, "SIIFUELMASS", 11)) {
            sscanf (line+11, "%f", &ftcp);
			SII_FuelMass = ftcp;
		}
		else if (!strnicmp(line, "S4FUELMASS", 10)) {
            sscanf (line+10, "%f", &ftcp);
			S4B_FuelMass = ftcp;
		}
		else if (!strnicmp(line, "PRELAUNCHATC", 12)) {
			int i;
			sscanf (line +12, "%d", &i);
			UseATC = (i != 0);
		}
		else if (!strnicmp (line, "PMET", 4)) {
			sscanf(line+4, "%d", &n);
			sscanf(line+8, "%f", &ftcp);
			if (n >= 0 && n < PITCH_TABLE_SIZE) {
				met[n] = ftcp;
			}
		}
		else if (!strnicmp (line, "CPITCH", 6)) {
			sscanf(line+6, "%d", &n);
			sscanf(line+10, "%f", &ftcp);
			if (n >= 0 && n < PITCH_TABLE_SIZE) {
				cpitch[n] = ftcp;
			}
		}
		else if (!strnicmp(line, "MOONLAT", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			LMLandingLatitude = ftcp;
		}
		else if (!strnicmp(line, "MOONLONG", 8)) {
			sscanf(line + 8, "%f", &ftcp);
			LMLandingLongitude = ftcp;
		}
		else if (!strnicmp(line, "MOONALT", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			LMLandingAltitude = ftcp;
		}
		else if (!strnicmp(line, "UNMANNED", 8)) {
			int i;
			sscanf(line + 8, "%d", &i);
			Crewed = (i == 0);
		}
		else if (!strnicmp(line, "AUTOSLOW", 8)) {
			int i;
			sscanf(line + 8, "%d", &i);
			AutoSlow = (i != 0);
		}
		else if (!strnicmp(line, "S4PL", 4)) {
			sscanf(line + 4, "%d", &SIVBPayload);
		}
		else if (!strnicmp(line, "CSMSEP", 6)) {
			sscanf(line + 6, "%f", &ftcp);
			CSMSepTime = ftcp;
			CSMSepSet = true;
		}
		else if (!strnicmp(line, "CMSEP", 5)) {
			sscanf(line + 5, "%f", &ftcp);
			CMSepTime = ftcp;
			CMSepSet = true;
		}
		else if (!strnicmp(line, "S4APO", 5)) {
			sscanf(line + 5, "%f", &ftcp);
			SIVBApogee = ftcp;
		}
		else if (!strnicmp(line, "S4BURN", 6)) {
			sscanf(line + 6, "%f", &ftcp);
			SIVBBurnStart = ftcp;
		}
		else if (!strnicmp(line, "CSMAPO", 6)) {
			sscanf(line + 6, "%f", &ftcp);
			CSMApogee = ftcp;
		}
		else if (!strnicmp(line, "CSMPER", 6)) {
			sscanf(line + 6, "%f", &ftcp);
			CSMPerigee = ftcp;
		}
		else if (!strnicmp(line, "CSMBURN", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			CSMBurnStart = ftcp;
		}
		else if (!strnicmp(line, "CSMACCEL", 8)) {
			sscanf(line + 8, "%f", &ftcp);
			CSMAccelSet = true;
			CSMAccelTime = ftcp;
		}
		else if (!strnicmp(line, "CSMACCEND", 9)) {
			sscanf(line + 9, "%f", &ftcp);
			CSMAccelEnd = ftcp;
		}
		else if (!strnicmp(line, "CSMACCPITCH", 11)) {
			sscanf(line + 11, "%f", &ftcp);
			CSMAccelPitch = ftcp;
		}
		else if (!strnicmp(line, "SMFUELLOAD", 10)) {
			sscanf(line + 10, "%f", &ftcp);
			SM_FuelMass = ftcp;
		}
		else if (!strnicmp(line, "CMFUELLOAD", 10)) {
			sscanf(line + 10, "%f", &ftcp);
			CM_FuelMass = ftcp;
		}
		else if (!strnicmp(line, "SMMASS", 6)) {
			sscanf(line + 6, "%f", &ftcp);
			SM_EmptyMass = ftcp;
		}
		else if (!strnicmp(line, "CMMASS", 6)) {
			sscanf(line + 6, "%f", &ftcp);
			CM_EmptyMass = ftcp;
		}
		else if (!strnicmp(line, "LANG", 4)) {
			strncpy (AudioLanguage, line + 5, 64);
		}
		else if (!strnicmp(line, DSKY_START_STRING, sizeof(DSKY_START_STRING))) {
			dsky.LoadState(scn);
		}
		else if (!strnicmp(line, AGC_START_STRING, sizeof(AGC_START_STRING))) {
			agc.LoadState(scn);
		}
		else if (!strnicmp(line, IMU_START_STRING, sizeof(IMU_START_STRING))) {
			imu.LoadState(scn);
		}
		else if (!strnicmp(line, CWS_START_STRING, sizeof(CWS_START_STRING))) {
			cws.LoadState(scn);
		}
		else if (!strnicmp (line, "SYSTEMSSTATE", 12)) {
			sscanf (line + 12, "%d", &systemsState);
		}
        else if (!strnicmp (line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
			Panelsdk.Load(scn);			//send the loading to the Panelsdk
		}
        else if (!strnicmp (line, PANELSWITCH_START_STRING, strlen(PANELSWITCH_START_STRING))) { 
			PSH.LoadState(scn);	
		}
		else {
			ParseScenarioLineEx (line, vstatus);
        }
    }

	if (!Crewed && (SIVBApogee > 0.0) && (SIVBBurnStart > 0) && (stage < CSM_LEM_STAGE)) {
		SIVBBurn = true;
	}

	if (!Crewed && (CSMApogee > 0.0) && (CSMBurnStart > 0) && (stage < CM_STAGE)) {
		CSMBurn = true;
	}

	soundlib.SetLanguage(AudioLanguage);
	LoadDefaultSounds();

	//
	// Recalculate stage masses.
	//

	UpdatePayloadMass();
	CalculateStageMass ();

	//
	// Tell the AGC the heading we want.
	//

	agc.SetDesiredAzimuth(tohdg);

	//
	// And pass it the mission number and realism settings.
	//

	agc.SetMissionInfo(ApolloNo, Realism);
}

//
// Set the appropriate mass based on the SIVB payload. I believe these are roughly the
// correct numbers.
//

void Saturn::UpdatePayloadMass()

{
	switch (SIVBPayload) {
	case PAYLOAD_LEM:
		// default, do nothing.
		break;

	case PAYLOAD_ASTP:
		S4PL_Mass = 2012;
		break;

	case PAYLOAD_LTA:
		S4PL_Mass = 13381;
		break;

	case PAYLOAD_LTA6:
		S4PL_Mass = 11794;
		break;

	case PAYLOAD_LM1:
		S4PL_Mass = 14360;
		break;

	case PAYLOAD_LTA8:
		S4PL_Mass = 9026;
		break;

	case PAYLOAD_TARGET:
		S4PL_Mass = 1000; // Guess
		break;

	default:
		S4PL_Mass = 0;
		break;
	}
}


void Saturn::SetStage(int s)

{
	stage = s;
	StageState = 0;

	//
	// Reset thrust.
	//

	ThrustAdjust = 1.0;
}

void Saturn::DoLaunch(double simt)

{
	SetLiftoffLight();

	SetStage(LAUNCH_STAGE_ONE);

	//
	// For now, we'll reset the mission timer to zero.
	//

	MissionTimerDisplay.Reset();

	//
	// Tell the AGC that we've lifted off.
	//

	agc.SetInputChannelBit(030, 5, true);

	//
	// Set full thrust.
	//

	SetThrusterGroupLevel(thg_main, 1.0);

	if (LaunchS.isValid() && !LaunchS.isPlaying()){
		LaunchS.play(NOLOOP,255);
		LaunchS.done();
	}
}

void Saturn::GenericTimestep(double simt)

{
	if (GenericFirstTimestep) {
		//
		// Do any generic setup.
		//

		if (CabinFansActive()) {
			CabinFanSound();
		}

		GenericFirstTimestep = false;
	}

	//
	// Update mission time.
	//

	double deltat = oapiGetSimStep();

	MissionTime += deltat;
	MissionTimerDisplay.Timestep(simt, deltat);

	//
	// Timestep tracking.
	//

#ifdef TRACK_TIMESTEPS
	CurrentTimestep++;
	if (oapiGetSimStep() > LongestTimestep) {
		LongestTimestep = CurrentTimestep;
		LongestTimestepLength = oapiGetSimStep();
	}
#endif // TRACK_TIMESTEPS

	//
	// Reduce jostle.
	//

	ViewOffsetx *= 0.95;
	ViewOffsety *= 0.95;
	ViewOffsetz *= 0.95;

	//
	// And update for acceleration.
	//

	double amt = fabs(aHAcc / 25.0) - 0.1;
	if (amt > 0.25)
		amt = 0.25;

	if (amt > 0)
		JostleViewpoint(amt);

	VECTOR3 RVEL = _V(0.0,0.0,0.0);
	GetRelativeVel(GetGravityRef(),RVEL);

	double velMS = sqrt(RVEL.x *RVEL.x + RVEL.y * RVEL.y + RVEL.z * RVEL.z);

	actualVEL = (velMS / 1000 * 3600);
	actualALT = GetAltitude();

	actualFUEL = ((GetFuelMass()*100)/GetMaxFuelMass());

	double aSpeed, DV, aALT, DVV, DVA, DVX, DVY, DVZ;
	double agrav,radius,mass,calpha,salpha,cbeta,sbeta,radius2;
	OBJHANDLE hPlanet;
	VESSELSTATUS status;

	aSpeed = actualVEL/3600*1000;
	aALT = actualALT;

	dTime = simt - aTime;

	if(dTime > 0.2) {

		DV= aSpeed - SPEEDN1;
		aHAcc= (DV / dTime);
		DVV = aALT - ALTN1;
		aVSpeed = DVV / dTime;
		DVA = aVSpeed- VSPEEDN1;

		aVAcc = (DVA/dTime);
		aTime = simt;
		VSPEEDN1 = aVSpeed;
		ALTN1 = aALT;
		SPEEDN1= aSpeed;

//  This stuff is to compute the component of the total acceleration
//		 along the z axis. This supports the "G-gauge" on the panel.

		GetStatus(status);
		calpha = cos(status.arot.x);
		cbeta = cos(status.arot.y);
		salpha = sin(status.arot.x);
		sbeta = sin(status.arot.y);
		DVX = status.rvel.x - XSPEEDN1;
		DVY = status.rvel.y - YSPEEDN1;
		DVZ = status.rvel.z - ZSPEEDN1;
		DVZ = cbeta*(DVY*salpha+DVZ*calpha)-DVX*sbeta;
		aZAcc = (DVZ / dTime);
		hPlanet = GetSurfaceRef();
		mass = oapiGetMass(hPlanet);
		radius2 = status.rpos.x*status.rpos.x+status.rpos.y*status.rpos.y+status.rpos.z*status.rpos.z;
		radius = sqrt(radius2);
		agrav = cbeta*(status.rpos.y*salpha+status.rpos.z*calpha)-status.rpos.x*sbeta;
		agrav *= 6.672e-11 * mass/(radius * radius2);
		aZAcc += agrav;
		XSPEEDN1 = status.rvel.x;
		YSPEEDN1 = status.rvel.y;
		ZSPEEDN1 = status.rvel.z;
	}

	//
	// Update parameters for AGC processing? Should these be here?
	//

	agc.SetFuel(actualFUEL);
	agc.SetRVel(velMS);

	SystemsTimestep(simt);

	if (stage < CSM_LEM_STAGE && AutopilotActive())	{
		SetAutopilotLight();
	} else {
		ClearAutopilotLight();
	}

	if(stage < LAUNCH_STAGE_SIVB) {
		if (GetNavmodeState(NAVMODE_KILLROT)) {
			DeactivateNavmode(NAVMODE_KILLROT);
		}
	}

	//
	// We die horribly if you set 100x or higher acceleration during launch.
	//

	if (stage >= LAUNCH_STAGE_ONE && stage <= LAUNCH_STAGE_SIVB &&
		oapiGetTimeAcceleration() > 10.0) {
		oapiSetTimeAcceleration(10.0);
	}

	//
	// And you'll also die horribly if you set time acceleration at all in the
	// early parts of the launch.
	//

	if (stage == LAUNCH_STAGE_ONE && MissionTime < 50 &&
		oapiGetTimeAcceleration() > 1.0) {
		oapiSetTimeAcceleration(1.0);
	}

	//
	// Only the SM has linear thrusters.
	//

	if (stage != CSM_LEM_STAGE) {
		if (GetAttitudeMode() == ATTMODE_LIN){
			SetAttitudeMode(ATTMODE_ROT);
		}
	}

	if (GetAltitude() < 1470) {
		actualVEL = actualVEL-1470+GetAltitude();
	}

	if (GroundContact()) {
		actualVEL = 0;
	}

	if (habort) {
		double altabort;
		int getIT;

		getIT = oapiGetAltitude(habort,&altabort);

		if (altabort < 100 && getIT > 0) {
			oapiDeleteVessel(habort,GetHandle());
			habort = NULL;
		}
	}

	// Docking radar sound only for CSM_LEM_STAGE
	if (stage == CSM_LEM_STAGE) {
		soundlib.SoundOptionOnOff(PLAYRADARBIP, TRUE);
	}
	else {
		soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	}

	//
	// Destroy seperated SM when it drops too low in the atmosphere.
	//

	if (hSMJet && !ApolloExploded) {
		if ((simt-(20+ignition_SMtime))>=0 && stgSM) {
			UllageSM(hSMJet,0,simt);
			stgSM = false;
		}
		else if (!SMSep){
			UllageSM(hSMJet,5,simt);
		}
		KillAlt(hSMJet,350000);
	}
}

void StageTransform(VESSEL *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel)
{
	VECTOR3 rofs, rvel = {vs->rvel.x, vs->rvel.y, vs->rvel.z};

	rofs.x = 0;
	rofs.y = 0;
	rofs.z = 0;

	// Staging Velocity represents
	// Need to do some transforms to get the correct vector to eject the stage

	vessel->Local2Rel (ofs, vs->rpos);
	vessel->GlobalRot (vel, rofs);
	vs->rvel.x = rvel.x+rofs.x;
	vs->rvel.y = rvel.y+rofs.y;
	vs->rvel.z = rvel.z+rofs.z;
}

int Saturn::clbkConsumeDirectKey(char *keystate)

{
	if (KEYMOD_SHIFT (keystate))
	{
		return 0;
	}
	else if (KEYMOD_CONTROL (keystate))
	{
	}
	else
	{
		if (KEYDOWN (keystate, OAPI_KEY_R)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_R, 1.0)) {
				{
					if (stage == CM_ENTRY_STAGE_SEVEN && HatchOpen) {
						bRecovery = true;
					}
				}
			return 1;
			}
		}

		if (KEYDOWN (keystate, OAPI_KEY_5)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_5, 1.0)){
				if (ActivateLEM){
					ActivateLEM=false;
				}
				return 1;
			}
		}

		if (KEYDOWN (keystate, OAPI_KEY_S)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_S, 1.0)){
				if (!autoDISP){
					autoDISP = true;
				}else{
					autoDISP = false;
				}
			return 1;
			}
		}

		if (KEYDOWN (keystate, OAPI_KEY_9) && (stage == CSM_LEM_STAGE || stage == CM_RECOVERY_STAGE)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_9, 1.0)) {
				viewpos = SATVIEW_DOCK;
				SetView();
			}
			return 1;
		}

		if (KEYDOWN (keystate, OAPI_KEY_8)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_8, 1.0)) {
				viewpos = SATVIEW_DMP;
				SetView();
			}
			return 1;
		}

		if (KEYDOWN (keystate, OAPI_KEY_7)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_7, 1.0)) {
				viewpos = SATVIEW_CMP;
				SetView();
			}
			return 1;
		}

		if (KEYDOWN (keystate, OAPI_KEY_6)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_6, 1.0)) {
				viewpos = SATVIEW_CDR;
				SetView();
			}
			return 1;
		}

	}
	return 0;
}

void Saturn::AddRCSJets(double TRANZ,double MaxThrust)
{
	int i;
	const double ATTCOOR = 0;
	const double ATTCOOR2 = 2.15;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.6;
	const double RCSOFFSET2=0.10;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.40;

	th_att_lin[0]=th_att_rot[0]=CreateThruster (_V(0,ATTCOOR2,TRANZ+RCSOFFSET2), _V(0,-0.1,1),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[1]=th_att_rot[3]=CreateThruster (_V(0,-ATTCOOR2,TRANZ+RCSOFFSET2), _V(0,0.1,1),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[2]=th_att_rot[4]=CreateThruster (_V(-ATTCOOR2,0,TRANZ+RCSOFFSET2), _V(0.1,0,1),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[3]=th_att_rot[7]=CreateThruster (_V(ATTCOOR2,0,TRANZ+RCSOFFSET2), _V(-0.1,0,1),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[4]=th_att_rot[2]=CreateThruster (_V(0,ATTCOOR2,TRANZ+RCSOFFSET), _V(0,-0.1,-1),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[5]=th_att_rot[1]=CreateThruster (_V(0,-ATTCOOR2,TRANZ+RCSOFFSET), _V(0,0.1,-1),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[6]=th_att_rot[6]=CreateThruster (_V(-ATTCOOR2,0,TRANZ+RCSOFFSET), _V(0.1,0,-1),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[7]=th_att_rot[5]=CreateThruster (_V(ATTCOOR2,0,TRANZ+RCSOFFSET), _V(-0.1,0,-1),MaxThrust, ph_rcs0,15000, 15000);

	CreateThrusterGroup (th_att_lin,   4, THGROUP_ATT_FORWARD);
	CreateThrusterGroup (th_att_lin+4, 4, THGROUP_ATT_BACK);
	CreateThrusterGroup (th_att_rot,   2, THGROUP_ATT_PITCHDOWN);
	CreateThrusterGroup (th_att_rot+2,   2, THGROUP_ATT_PITCHUP);
	CreateThrusterGroup (th_att_rot+4,   2, THGROUP_ATT_YAWRIGHT);
	CreateThrusterGroup (th_att_rot+6,   2, THGROUP_ATT_YAWLEFT);

	for (i = 0; i < 8; i++)
		AddExhaust (th_att_lin[i], 1.2, 0.18, SMExhaustTex);

	th_att_lin[8]=th_att_rot[16]=th_att_rot[17]=CreateThruster (_V(-0.2,ATTCOOR2,TRANZ+RCSOFFSETM), _V(1,-0.1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[9]=th_att_rot[8]=th_att_rot[9]=CreateThruster (_V(-0.2,-ATTCOOR2,TRANZ+RCSOFFSETM2), _V(1,0.1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[10]=CreateThruster (_V(-0.2,ATTCOOR2,-TRANZ-RCSOFFSETM), _V(1,-0.1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[11]=CreateThruster (_V(-0.2,-ATTCOOR2,-TRANZ-RCSOFFSETM2), _V(1,0.1,0),MaxThrust, ph_rcs0,15000, 15000);

	th_att_lin[12]=th_att_rot[10]=th_att_rot[11]=CreateThruster (_V(0.2,ATTCOOR2,TRANZ+RCSOFFSETM2), _V(-1,-0.1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[13]=th_att_rot[18]=th_att_rot[19]=CreateThruster (_V(0.2,-ATTCOOR2,TRANZ+RCSOFFSETM), _V(-1,0.1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[14]=CreateThruster (_V(0.2,ATTCOOR2,-TRANZ-RCSOFFSETM2), _V(-1,-0.1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[15]=CreateThruster (_V(0.2,-ATTCOOR2,-TRANZ-RCSOFFSETM), _V(-1,0.1,0),MaxThrust, ph_rcs0,15000, 15000);

	th_att_lin[16]=th_att_rot[14]=th_att_rot[15]=CreateThruster (_V(ATTCOOR2,-0.2,TRANZ+RCSOFFSETM2), _V(-0.1,1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[17]=th_att_rot[22]=th_att_rot[23]=CreateThruster (_V(-ATTCOOR2,-0.2,TRANZ+RCSOFFSETM), _V(-0.1,1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[18]=CreateThruster (_V(ATTCOOR2,-0.2,-TRANZ-RCSOFFSETM2), _V(-0.1,1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[19]=CreateThruster (_V(-ATTCOOR2,-0.2,-TRANZ-RCSOFFSETM), _V(-0.1,1,0),MaxThrust, ph_rcs0,15000, 15000);

	th_att_lin[20]=th_att_rot[20]=th_att_rot[21]=CreateThruster (_V(ATTCOOR2,0.2,TRANZ+RCSOFFSETM), _V(-0.1,-1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[21]=th_att_rot[12]=th_att_rot[13]=CreateThruster (_V(-ATTCOOR2,0.2,TRANZ+RCSOFFSETM2), _V(0.1,-1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[22]=CreateThruster (_V(ATTCOOR2,0.2,-TRANZ-RCSOFFSETM), _V(-0.1,-1,0),MaxThrust, ph_rcs0,15000, 15000);
	th_att_lin[23]=CreateThruster (_V(-ATTCOOR2,0.2,-TRANZ-RCSOFFSETM2), _V(0.1,-1,0),MaxThrust, ph_rcs0,15000, 15000);

	CreateThrusterGroup (th_att_lin+8,   4, THGROUP_ATT_RIGHT);
	CreateThrusterGroup (th_att_lin+12, 4, THGROUP_ATT_LEFT);
	CreateThrusterGroup (th_att_lin+16,   4, THGROUP_ATT_UP);
	CreateThrusterGroup (th_att_lin+20,   4, THGROUP_ATT_DOWN);
	CreateThrusterGroup (th_att_rot+8,   8, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+16,   8, THGROUP_ATT_BANKRIGHT);

	for (i = 8; i < 24; i++)
		AddExhaust (th_att_rot[i], 1.2, 0.18, SMExhaustTex);
}

void Saturn::AddRCS_S4B()

{
	const double ATTCOOR = -10;
	const double ATTCOOR2 = 3.61;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double TRANZ=-3.2-STG2O;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;
	const double RCSX=0.35;
	VECTOR3 m_exhaust_pos2= {0,ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos3= {0,-ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos4= {-ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_pos5= {ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_ref2 = {0,0.1,-1};
	VECTOR3 m_exhaust_ref3 = {0,-0.1,-1};
	VECTOR3 m_exhaust_ref4 = {-0.1,0,-1};
	VECTOR3 m_exhaust_ref5 = {0.1,0,-1};
	double offset;
	offset = 0.0;
	if((ApolloNo<8)&&(ApolloNo!=6)&&(ApolloNo!=4))offset=7.7;

	th_att_rot[0] = CreateThruster (_V(0,ATTCOOR2+0.15,TRANZ-0.25+offset), _V(0, -1,0), 20740.0, ph_3rd,5000000, 4000000);
	th_att_rot[1] = CreateThruster (_V(0,-ATTCOOR2-0.15,TRANZ-0.25+offset), _V(0,1,0),20740.0, ph_3rd,5000000, 4000000);
	
	AddExhaust (th_att_rot[0], 0.6, 0.078);
	AddExhaust (th_att_rot[1], 0.6, 0.078);
	CreateThrusterGroup (th_att_rot,   1, THGROUP_ATT_PITCHUP);
	CreateThrusterGroup (th_att_rot+1, 1, THGROUP_ATT_PITCHDOWN);


	th_att_rot[2] = CreateThruster (_V(RCSX,ATTCOOR2-0.2,TRANZ-0.25+offset), _V(-1,0,0),17400.0, ph_3rd,250000, 240000);
	th_att_rot[3] = CreateThruster (_V(-RCSX,-ATTCOOR2+0.2,TRANZ-0.25+offset), _V( 1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[4] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V( 1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[5] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0),17400.0, ph_3rd,250000, 240000);

	AddExhaust (th_att_rot[2], 0.6, 0.078);
	AddExhaust (th_att_rot[3], 0.6, 0.078);
	AddExhaust (th_att_rot[4], 0.6, 0.078);
	AddExhaust (th_att_rot[5], 0.6, 0.078);
	CreateThrusterGroup (th_att_rot+2,   2, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+4, 2, THGROUP_ATT_BANKRIGHT);


	th_att_rot[6] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[7] = CreateThruster (_V(-RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[8] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[9] = CreateThruster (_V(RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(-1,0,0), 17400.0, ph_3rd,250000, 240000);

	AddExhaust (th_att_rot[6], 0.6, 0.078);
	AddExhaust (th_att_rot[7], 0.6, 0.078);
	AddExhaust (th_att_rot[8], 0.6, 0.078);
	AddExhaust (th_att_rot[9], 0.6, 0.078);

	CreateThrusterGroup (th_att_rot+6,   2, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+8, 2, THGROUP_ATT_YAWRIGHT);

	//
	// APS thrusters are only 320N (72 pounds) thrust
	//

	th_att_lin[0] = CreateThruster (_V(0,ATTCOOR2-0.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_3rd,250000, 240000);
	th_att_lin[1] = CreateThruster (_V(0,-ATTCOOR2+.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_3rd,250000, 240000);
	AddExhaust (th_att_lin[0], 7, 0.15);
	AddExhaust (th_att_lin[1], 7, 0.15);

	thg_aps = CreateThrusterGroup (th_att_lin,   2, THGROUP_ATT_FORWARD);
}

// ==============================================================
// DLL entry point
// ==============================================================

BOOL WINAPI DllMain (HINSTANCE hModule,
					 DWORD ul_reason_for_call,
					 LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		SetupgParam(hModule);
		break;

	case DLL_PROCESS_DETACH:
		DeletegParam();
		break;
	}
	return TRUE;
}

void Saturn::LaunchCountdown(double simt)
{
	if (GetEngineLevel(ENGINE_MAIN) > 0 && MissionTime <= (-8.9)) {
		SetThrusterGroupLevel(thg_main, 0);
	}

	if (oapiGetTimeAcceleration() > 100)
		oapiSetTimeAcceleration(100);

	if (MissionTime >= 0) {
		DoLaunch(simt);
		return;
	}

	switch (StageState) {

	case 0:
		if (MissionTime >= -((4 * 60) + 10)) {
			//
			// Engine lights on.
			//

			for (int i = 0; i < 5; i++) {
				ENGIND[i] = true;
			}
			StageState++;
		}
		break;

	case 1:

		//
		// Reset time acceleration to normal at
		// 20 seconds, and reconnect the fuel to
		// the main engines.
		//

		if (MissionTime >= -20.0) {
			oapiSetTimeAcceleration (1);
			for (int i = 0; i < 5; i++) {
				SetThrusterResource(th_main[i], ph_1st);
			}
			StageState++;
		}
		break;

	case 2:

		//
		// Play the countdown sound at 10 seconds,
		//

		if (MissionTime >= -10.9) {
			if (!UseATC && Scount.isValid()) {
				Scount.play();
				Scount.done();
			}
			StageState++;
		}
		break;

	case 3:
		if (MissionTime >= -8.9) {
			StageState++;
		}
		break;

	case 4:
		//
		// Build up engine thrust. Slower at first so we don't
		// leave the ground before we should.
		//

		double thrst;

		if (MissionTime > (-1.0)) {
			thrst = 0.7 + (0.3 * (MissionTime + 1.0));

			//
			// Engine lights off.
			//

			for (int i = 0; i < 5; i++) {
				ENGIND[i] = false;
			}
		}
		else {
			thrst = (0.7 / 8.9) * (MissionTime + 8.9);
		}
		SetThrusterGroupLevel(thg_main, thrst);


		double amt = (thrst) * 0.1;
		JostleViewpoint(amt);

		break;
	}
}

void Saturn::GenericTimestepStage(double simt)

{
	//
	// Do stage-specific processing.
	//

	switch (stage) {

	case PRELAUNCH_STAGE:
		LaunchCountdown(simt);
		break;

	case CM_STAGE:
		if (GetAtmPressure() > 38000)
			StageEight(simt);
		else
			StageSeven(simt);
		break;

	case CM_ENTRY_STAGE:
		if (GetAtmPressure() > 37680)
			StageEight(simt);
		break;

	case CM_ENTRY_STAGE_TWO:
		if (GetAtmPressure() > 39000) {
			SetChuteStage1();
			LAUNCHIND[3] = true;
			SetStage(CM_ENTRY_STAGE_THREE);
		}
		break;

	case CM_ENTRY_STAGE_THREE:
		if (GetAtmPressure() > 66000) {
			SetChuteStage2();
			SetStage(CM_ENTRY_STAGE_FOUR);
		}
		break;

	case CM_ENTRY_STAGE_FOUR:
		if (GetAtmPressure() > 68000) {
			SetChuteStage3();
			SetStage(CM_ENTRY_STAGE_FIVE);
		}
		break;

	case CM_ENTRY_STAGE_FIVE:
		if (GetAtmPressure() > 69000) {
			SetChuteStage4();
			SetStage(CM_ENTRY_STAGE_SIX);
			LAUNCHIND[5] = true;
		}
		break;

	case CM_ENTRY_STAGE_SIX:
		if (GetAltitude() < 2) {
			SplashS.play(NOLOOP, 180);
			SplashS.done();
			SetSplashStage();
			SetStage(CM_ENTRY_STAGE_SEVEN);
		}
		break;

	case CM_ENTRY_STAGE_SEVEN:
		if(RPswitch13 && !HatchOpen){
			bToggleHatch = true;
		}
		else if(!RPswitch13 && HatchOpen){
			bToggleHatch = true;
		}
		if (RPswitch14 && HatchOpen){
			bRecovery = true;
		}

		if (!Swater.isPlaying())
			Swater.play(LOOP,190);

		if (bToggleHatch){
			ToggelHatch2();
			bToggleHatch=false;
		}

		if (bRecovery){
			SetRecovery();
			bRecovery = false;
			soundlib.LoadSound(Swater, WATERLOOP_SOUND);
			soundlib.LoadMissionSound(PostSplashdownS, POSTSPLASHDOWN_SOUND, POSTSPLASHDOWN_SOUND);
			NextMissionEventTime = MissionTime + 10.0;
			SetStage(CM_RECOVERY_STAGE);
		}
		break;

	case CM_RECOVERY_STAGE:
		if (!PostSplashdownPlayed && MissionTime >= NextMissionEventTime) {
			PostSplashdownS.play();
			PostSplashdownS.done();
			PostSplashdownPlayed = true;
		}
		break;
	}
}

void Saturn::GenericLoadStateSetup()

{
	//
	// Some switches only work in the CSM/LEM stage. Disable them
	// otherwise.
	//

	if (stage != CSM_LEM_STAGE) {
		LPswitch5.SetActive(false);
	}
	else {
		LPswitch5.SetActive(true);
	}

	//
	// Disable cabin fans.
	//

	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);

	//
	// We do our own countdown, so ignore the standard one.
	//

	if (!UseATC)
		soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);

	//
	// Load mission-based sound files. Some of these are just being
	// preloaded here for the CSM computer.
	//

	char MissionName[24];

	_snprintf(MissionName, 23, "Apollo%d", ApolloNo);
	soundlib.SetSoundLibMissionPath(MissionName);

	//
	// Set up options for prelaunch stage.
	//

	if (MissionTime < 0) {
		agc.SetBurnTime(0);

		//
		// Open the countdown sound file.
		//

		if (!UseATC) {
			soundlib.LoadMissionSound(Scount, LAUNCH_COUNT_10_SOUND, DEFAULT_LAUNCH_COUNT_SOUND);
			soundlib.LoadMissionSound(LaunchS, LAUNCH_SOUND, LAUNCH_SOUND);
			Scount.setFlags(SOUNDFLAG_1XORLESS);
			LaunchS.setFlags(SOUNDFLAG_1XORLESS);
		}
	}

	//
	// Only the CSM and LEM have translational thrusters, so disable the message
	// telling us that they're being switched in other stages.
	//

	if (stage != CSM_LEM_STAGE) {
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, FALSE);
	}
	else {
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
	}

	if (stage < PRELAUNCH_STAGE) {
		agc.BlankAll();
	}

	if (stage < LAUNCH_STAGE_TWO) {
		soundlib.LoadMissionSound(SShutS, SI_CUTOFF_SOUND, SISHUTDOWN_SOUND);
	}

	if (stage < LAUNCH_STAGE_TWO_TWR_JET) {
		soundlib.LoadSound(TowerJS, TOWERJET_SOUND);
	}

	if (stage < STAGE_ORBIT_SIVB) {

		//
		// We'll do our own radio playback during launch.
		//

		if (!UseATC)
			soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);

		soundlib.LoadMissionSound(SwindowS, WINDOW_SOUND, POST_TOWER_JET_SOUND);
		soundlib.LoadMissionSound(S2ShutS, SII_CUTOFF_SOUND, SIISHUTDOWN_SOUND);
		soundlib.LoadMissionSound(S4CutS, GO_FOR_ORBIT_SOUND, SIVBSHUTDOWN_SOUND);
	}

	if (stage < CSM_LEM_STAGE) {
		soundlib.LoadSound(SepS, SEPMOTOR_SOUND, INTERNAL_ONLY);
	}

	if (stage <= CSM_LEM_STAGE) {
		soundlib.LoadMissionSound(SMJetS, SM_SEP_SOUND, DEFAULT_SM_SEP_SOUND);
	}

	if (stage == CM_RECOVERY_STAGE)
	{
		soundlib.LoadSound(Swater, WATERLOOP_SOUND);
		soundlib.LoadMissionSound(PostSplashdownS, POSTSPLASHDOWN_SOUND, POSTSPLASHDOWN_SOUND);
	}

	//
	// Load Apollo-13 specific sounds.
	//

	if (ApolloNo == 13) {
		if (!KranzPlayed)
			soundlib.LoadMissionSound(SKranz, A13_KRANZ, NULL, INTERNAL_ONLY);
		if (!CryoStir)
			soundlib.LoadMissionSound(SApollo13, A13_CRYO_STIR, NULL);
		if (!ApolloExploded)
			soundlib.LoadMissionSound(SExploded, A13_PROBLEM, NULL);

		if (stage <= CSM_LEM_STAGE) {
			soundlib.LoadMissionSound(SSMSepExploded, A13_SM_SEP_SOUND, NULL);
		}

		SApollo13.setFlags(SOUNDFLAG_1XORLESS);
		SExploded.setFlags(SOUNDFLAG_1XORLESS);
	}

	//
	// Turn of the timer display on launch.
	//

	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);
}

void Saturn::UllageSM(OBJHANDLE hvessel,double gaz1, double time)

{
	if (ignition_SMtime == 0 && !SMSep) {
		setupSM(hvessel);
		ignition_SMtime=time;
		SMSep = true;
	}

	VESSEL *stg1vessel = oapiGetVesselInterface(hvessel);

	if (gaz1 >0) {
		stg1vessel->SetAttitudeLinLevel(2,-1);
		if (time > ignition_SMtime+5){
			stg1vessel->SetAttitudeRotLevel(2,-1);
		}
		if (time > ignition_SMtime+7){
			stg1vessel->SetAttitudeRotLevel(2,0);
		}
	}
	else {
		stg1vessel->SetAttitudeLinLevel(2,0);
		stg1vessel->SetAttitudeRotLevel(2,0);
		stg1vessel->SetAttitudeRotLevel(1,0);
	}
}

bool Saturn::CheckForLaunchShutdown()

{
	//
	// Shut down the engine when we're close to the desired apogee and perigee.
	//

	GetElements(elemSaturn1B, refSaturn1B);

	double apogee = ((elemSaturn1B.a * (1.0 + elemSaturn1B.e)) / 1000) - ERADIUS;
	double perigee = ((elemSaturn1B.a * (1.0 - elemSaturn1B.e)) / 1000) - ERADIUS;

	if ((refSaturn1B - refPREV) >= 0 &&
		(stage == LAUNCH_STAGE_SIVB) &&
		((elemSaturn1B.e > elemPREV.e && elemSaturn1B.e < 0.03) ||
		(apogee >= agc.GetDesiredApogee() && perigee >= agc.GetDesiredPerigee())) &&
		(SCswitch||CMCswitch))
	{
		if (GetEngineLevel(ENGINE_MAIN) > 0){
			SetEngineLevel(ENGINE_MAIN,0);
			if (oapiGetTimeAcceleration() > 1.0)
				oapiSetTimeAcceleration(1.0);

			S4CutS.play(NOLOOP, 255);
			S4CutS.done();

			ActivateNavmode(NAVMODE_KILLROT);

			agc.LaunchShutdown();
		}
		return true;
	}

	elemPREV = elemSaturn1B;
	refPREV = refSaturn1B;

	return false;
}

void Saturn::SetGenericStageState()

{
	switch(stage) {

	case CM_STAGE:
		SetCSMStage();
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_TWO:
		SetCSMStage();
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_THREE:
		SetCSMStage();
		SetChuteStage1();
		break;

	case CM_ENTRY_STAGE_FOUR:
		SetCSMStage();
		SetChuteStage2();
		break;

	case CM_ENTRY_STAGE_FIVE:
		SetCSMStage();
		SetChuteStage3();
		break;

	case CM_ENTRY_STAGE_SIX:
		SetCSMStage();
		SetChuteStage4();
		break;

	case CM_ENTRY_STAGE_SEVEN:
		SetCSMStage();
		SetSplashStage();
		break;

	case CM_RECOVERY_STAGE:
		SetRecovery();
		break;

	case CM_ENTRY_STAGE:
		SetCSMStage();
		SetReentryStage();
		break;

	case CSM_ABORT_STAGE:
		SetAbortStage();
		break;

	}
}

void Saturn::SIVBBoiloff()

{
	if (Realism < 2)
		return;

	//
	// The SIVB stage boils off a small amount of fuel while in orbit.
	//
	// For the time being we'll ignore any thrust created by the venting
	// of this fuel.
	//

	double FuelMass = GetPropellantMass(ph_3rd) * 0.99998193;
	SetPropellantMass(ph_3rd, FuelMass);
}

//
// Set thruster state based on panel switches.
//

void Saturn::SetSIVBThrusters()

{
	if (GetNavmodeState(NAVMODE_KILLROT)){
		if (GetThrusterLevel(th_att_rot[2]) == 0 &&
			GetThrusterLevel(th_att_rot[4]) == 0 &&
			!RPswitch1 && !RPswitch2){
			DeactivateNavmode(NAVMODE_KILLROT);
		}
	}

	if(RPswitch1) {
		SetThrusterResource(th_att_rot[0],ph_3rd);
		SetThrusterResource(th_att_rot[1],ph_3rd);
	}
	else{
		SetThrusterResource(th_att_rot[0],NULL);
		SetThrusterResource(th_att_rot[1],NULL);
	}

	if(RPswitch3){
		SetThrusterResource(th_att_rot[2],ph_3rd);
		SetThrusterResource(th_att_rot[3],ph_3rd);
		SetThrusterResource(th_att_rot[4],ph_3rd);
		SetThrusterResource(th_att_rot[5],ph_3rd);
	}
	else{
		SetThrusterResource(th_att_rot[2],NULL);
		SetThrusterResource(th_att_rot[3],NULL);
		SetThrusterResource(th_att_rot[4],NULL);
		SetThrusterResource(th_att_rot[5],NULL);
	}

	if(RPswitch2){
		SetThrusterResource(th_att_rot[7],ph_3rd);
		SetThrusterResource(th_att_rot[6],ph_3rd);
		SetThrusterResource(th_att_rot[8],ph_3rd);
		SetThrusterResource(th_att_rot[9],ph_3rd);
	}
	else{
		SetThrusterResource(th_att_rot[6],NULL);
		SetThrusterResource(th_att_rot[7],NULL);
		SetThrusterResource(th_att_rot[8],NULL);
		SetThrusterResource(th_att_rot[9],NULL);
	}
}

//
// Thrusters can't turn more than five degrees from straight ahead. Check for that,
// and alarm if they try to go beyond it.
//

void Saturn::LimitSetThrusterDir (THRUSTER_HANDLE th, const VECTOR3 &dir)

{
	VECTOR3 realdir = dir;
	bool alarm = false;

	if (realdir.x > 0.1) {
		realdir.x = 0.1;
		alarm = true;
	}

	if (realdir.x < (-0.1)) {
		realdir.x = -0.1;
		alarm = true;
	}

	if (realdir.y > 0.1) {
		realdir.y = 0.1;
		alarm = true;
	}

	if (realdir.y < (-0.1)) {
		realdir.y = (-0.1);
		alarm = true;
	}

	if (alarm && autopilot) {
		MasterAlarm();
	}

	SetThrusterDir(th, realdir);
}

void Saturn::LoadDefaultSounds()

{
	//
	// Remember that you can't load mission sounds at this point as the
	// mission path hasn't been set up!
	//

	soundlib.LoadSound(SeparationS, SEPARATION_SOUND);
	soundlib.LoadSound(Sctdw, COUNT10_SOUND);
	soundlib.LoadSound(Sclick, CLICK_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Bclick, BUTTON_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Gclick, GUARD_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Psound, PROBE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(CabinFans, CMCABIN_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SMasterAlarm, MASTERALARM_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SplashS, SPLASH_SOUND);
	soundlib.LoadSound(StageS, "Stagesep.wav");
	soundlib.LoadSound(CrashBumpS, "Crash.wav");

	Sctdw.setFlags(SOUNDFLAG_1XONLY);
}

void Saturn::StageOrbitSIVB(double simt)

{
	//
	// Post-shutdown, pre-TLI code goes here.
	//
	AttitudeLaunchSIVB();

	if (TLICapableBooster && !(bStartS4B || TLIBurnDone)) {
		if (TLIswitch) {
			(void) SIVBStart();
		}
		if (agc.GetOutputChannelBit(012, 13)) {
			if (SIVBStart())
				agc.SetOutputChannelBit(012, 13, false);
		}
	}

	if (GetEngineLevel(ENGINE_MAIN) > 0.65 ){
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else{
		ENGIND[5] = false;
		LAUNCHIND[7] = false;
	}

	if(TLICapableBooster){
		switch (StageState) {

		case 0:

			//
			// Enable random ATC chatter.
			//

			if (!UseATC)
				soundlib.SoundOptionOnOff(PLAYRADIOATC, TRUE);

			if (Crewed || !SIVBBurn) {
				StageState = 100;
			}
			else {
				NextMissionEventTime = SIVBBurnStart - 100.0;
				StageState++;
			}
			break;

		case 1:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				ActivateS4RCS();
				StageState++;
			}
			break;

		case 2:
			ActivateNavmode(NAVMODE_PROGRADE);
			NextMissionEventTime = SIVBBurnStart;
			StageState++;
			break;

		case 3:
			if (MissionTime >= NextMissionEventTime) {
				ActivateNavmode(NAVMODE_PROGRADE);
				SIVBStart();
				StageState = 100;
			}
			break;

		case 100:
			if (bStartS4B && TLICapableBooster) {
				SetAttitudeLinLevel(2,1);

				//
				// We'll treat the switch at the command to start the ullage engines.
				//

				NextMissionEventTime = MissionTime + 1.0;
				StageState++;
			}

			//
			// Fuel boiloff every ten seconds.
			//

			if (MissionTime >= NextMissionEventTime) {
				if(TLICapableBooster){
					SIVBBoiloff();
				}
				NextMissionEventTime = MissionTime + 10.0;
			}
			break;

		case 101:
			//
			// Start ullage engines after 1 second.
			//

			if (MissionTime >= NextMissionEventTime) {
				SetThrusterGroupLevel(thg_aps, 1.0);
				SepS.play(LOOP, 130);
				NextMissionEventTime += 16.7;
				StageState++;
			}
			break;

		case 102:

			//
			// Ullage cutoff after 17.7 seconds.
			//

			if (MissionTime >= NextMissionEventTime) {
				SetThrusterGroupLevel(thg_aps, 0.0);
				SepS.stop();
				SepS.done();
				NextMissionEventTime += 4.8;
				StageState++;
			}
			break;

		case 103:
			if (MissionTime >= NextMissionEventTime) {
				if (Realism)
					SetThrusterResource(th_main[0], ph_3rd);
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 2.5;
				StageState++;
			}
			break;

		case 104:

			//
			// Engine ramps up to 90% thrust from 22.5 to 25 seconds.
			//
			if (MissionTime < NextMissionEventTime) {
				double deltat = (MissionTime - LastMissionEventTime);
				SetThrusterLevel(th_main[0], (0.9 * deltat) * 0.4);
			}
			else {
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 0.5;
				StageState++;
			}
			break;

		case 105:

			//
			// Then up to 100%.
			//

			if (MissionTime < NextMissionEventTime) {
				double deltat = (MissionTime - LastMissionEventTime);
				SetThrusterLevel(th_main[0], 0.9 + (deltat * 0.2));
			}
			else {
				NextMissionEventTime += 0.5;
				StageState++;
			}
			break;

		case 106:

			//
			// Engine will be at 100% thrust after around 26 seconds.
			//

			if (MissionTime >= NextMissionEventTime) {
				TLIswitch = false;
				SetThrusterLevel(th_main[0], 1.0);
				SetThrusterGroupLevel(thg_ver, 0.0);
				bStartS4B = false;
				SetAttitudeLinLevel(2,0);
				if (!SIVBBurn || Crewed) {
					StageState = 200;
				}
				else {
					StageState = 150;
				}
			}
			break;

		//
		// Wait for the right apogee.
		//

		case 150:
			if (MissionTime >= NextMissionEventTime) {
				OBJHANDLE hPlanet = GetGravityRef();
				double prad = oapiGetSize(hPlanet);
				double ap;
				GetApDist(ap);

				ActivateNavmode(NAVMODE_PROGRADE);

				//
				// Burn until the orbit is about right or we're out of fuel.
				//

				if ((ap >= (prad + (SIVBApogee * 1000.0))) || (actualFUEL <= 0.1)) {
					StageState = 200;
					SIVBBurn = false;
					SIVBStop();
					DeactivateNavmode(NAVMODE_PROGRADE);
					DeactivateS4RCS();
				}

				NextMissionEventTime = MissionTime + 0.25;
			}
			break;

		case 200:

			//
			// Wait for shutdown and disable thrust.
			//

			if (agc.GetOutputChannelBit(012, 14)) {
				agc.SetOutputChannelBit(012, 14, false);
				SIVBStop();
			}

			if (GetEngineLevel(ENGINE_MAIN) <= 0) {
				if (Realism)
					SetThrusterResource(th_main[0], NULL);
				NextMissionEventTime = MissionTime + 10.0;
				StageState++;
			}
			break;

		case 201:

			if (Realism < 2)
				StageState = 100;
			else
				StageState++;
			break;

		case 202:

			//
			// Engine is now dead. Just boil off the remaining
			// fuel.
			//

			if (MissionTime >= NextMissionEventTime) {
				SIVBBoiloff();
				NextMissionEventTime = MissionTime + 10.0;
			}
			break;
		}
	}
	else{
		if (GetEngineLevel(ENGINE_MAIN) <= 0) {
			if (Realism)
				SetThrusterResource(th_main[0], NULL);
		}
	}

	if (thg_ver && GetThrusterGroupLevel(thg_ver) > 0) {
		LAUNCHIND[6] = true;
	}
	else {
		LAUNCHIND[6] = false;
	}

	//
	// For unmanned launches, seperate the CSM on timer.
	//

	if (!Crewed && CSMSepSet && (MissionTime >= CSMSepTime)) {
		SlowIfDesired();
		bManualSeparate = true;
		CSMSepSet = false;
	}

	if (CsmLvSepSwitch.GetState()) {
		bManualSeparate = true;
	}

	if (bManualSeparate || bAbort)
	{
		bManualSeparate = false;
		SeparateStage(stage);
		SetStage(CSM_LEM_STAGE);
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
		if (bAbort) {
			SPSswitch.SetState(true);
			ABORT_IND = true;
			SetThrusterGroupLevel(thg_main, 1.0);
			bAbort = false;
			autopilot= false;
			StartAbort();
		}
		return;
	}

	SetSIVBThrusters();
}

void Saturn::StartAbort()

{
	//
	// Event timer resets to zero on abort.
	//

	EventTimerOffset = (-MissionTime);
}

void Saturn::SlowIfDesired()

{
	if (!Crewed && AutoSlow && (oapiGetTimeAcceleration() > 1.0)) {
		oapiSetTimeAcceleration(1.0);
	}
}

//
// Get the J2 ISP from the mixture ratio and calculate the thrust adjustment.
//

double Saturn::GetJ2ISP(double ratio)

{
	double isp;

	// From Usenet:
	// It had roughly three stops. 178,000 lbs at 425s Isp and an O/F of 4.5,
	// 207,000 lbs at 421s Isp and an O/F of 5.0, and 230,500 lbs at 418s Isp
	// and an O/F of 5.5.

	if (ratio >= 5.5) {
		isp = 418*G;
		ThrustAdjust = 1.0;
	}
	else if (ratio >= 5.0) {
		isp = 421*G;
		ThrustAdjust = 0.898;
	}
	else {
		isp = 425*G;
		ThrustAdjust = 0.772;
	}

	return isp;
}

//
// Most of this calculation code is lifted from the Soyuz guidance MFD.
//

extern double AbsOfVector(const VECTOR3 &Vec);

double Saturn::CalculateApogeeTime()

{
	OBJHANDLE hSetGbody;
	double GbodyMass, GbodySize;
	double p, v, R, RDotV, Mu_Planet, J2000, E, Me, T, tsp;
	double TtPeri, TtApo;
	double OrbitApo;
	VECTOR3 RelPosition, RelVelocity;
	ELEMENTS Elements;

	// Planet parameters
	hSetGbody = GetApDist(OrbitApo);
	GbodyMass = oapiGetMass(hSetGbody);
	GbodySize = oapiGetSize(hSetGbody) / 1000;
	Mu_Planet = GK * GbodyMass;

	// Get eccentricity and orbital radius
	GetElements(Elements, J2000);
	GetRelativePos(hSetGbody, RelPosition);
	GetRelativeVel(hSetGbody, RelVelocity);

	R = AbsOfVector(RelPosition) / 1000;

	// Calculate semi-latus rectum and true anomaly
	p = Elements.a/1000 *(1 - Elements.e*Elements.e);
	v = acos((1/Elements.e)*(p/R - 1));

	RDotV = dotp(RelVelocity, RelPosition);
	if (RDotV < 0)
	{
		v = 2*PI - v;
	}

	// Determine the time since periapsis
	//   - Eccentric anomaly
	E = 2 * atan(sqrt((1-Elements.e)/(1+Elements.e))*tan(v/2));
	//   - Mean anomaly
	Me = E - Elements.e*sin(E);
	//   - Period of orbit
	T = 2*PI*sqrt((Elements.a*Elements.a*Elements.a/1e9)/Mu_Planet);

	// Time since periapsis is
	tsp = Me/(2*PI)*T;

	// Time to next periapsis & apoapsis
	TtPeri = T - tsp;
	if (RDotV < 0) {
		TtPeri = -1 * tsp;
	}

	if (TtPeri > (T / 2)) {
		TtApo = fabs((T/2) - TtPeri);
	}
	else {
		TtApo = fabs(TtPeri + (T/2));
	}

	return TtApo;
}
