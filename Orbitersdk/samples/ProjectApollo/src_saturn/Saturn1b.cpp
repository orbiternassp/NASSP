/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn IB

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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "ioChannels.h"

#include "saturn.h"
#include "saturn1b.h"

#include "s1b.h"
#include "LVDC.h"

#include "tracer.h"

//
// Random functions from Yaagc.
//

extern "C" {
	void srandom(unsigned int x);
	long int random();
}

char trace_file[] = "ProjectApollo Saturn1b.log";

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

// ==============================================================
// Global parameters
// ==============================================================

GDIParams g_Param;

static int refcount = 0;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hastp;

Saturn1b::Saturn1b (OBJHANDLE hObj, int fmodel)
: Saturn (hObj, fmodel)

{
	hMaster = hObj;
	initSaturn1b();
}

Saturn1b::~Saturn1b()

{
	ReleaseSurfaces();

	delete iu;
}


void Saturn1b::initSaturn1b()

{
	// Save DLL instance handle for later abuse
	dllhandle = g_Param.hDLL;
	
	//
	// Do the basic initialisation from the
	// generic Saturn class first.
	//

	initSaturn();

	SaturnType = SAT_SATURN1B;
	RelPos = _V(0.0,0.0,0.0);
	hSoyuz = 0;
	hAstpDM = 0;
	hNosecapVessel = 0;
	Burned = false;

	if (strcmp(GetName(), "AS-211")==0)
	{
		ASTPMission = true;
	}

	//
	// Apollo 7 ISP and thrust values.
	// Note: thrust values are _per engine_, not per stage.
	/// \todo other Saturn 1b missions
	//

	ISP_FIRST_SL     = 262*G;	// See Apollo 7 Saturn IB Report, NTRS ID 19900067467
	ISP_FIRST_VAC    = 294*G; 
	THRUST_FIRST_VAC = 1008000;  // between 875.000-1.025.000 N, calibrated to meet staging target

	ISP_SECOND_SL   = 424*G;
	ISP_SECOND_VAC  = 424*G;

	THRUST_SECOND_VAC  = 1009902;

	SM_EmptyMass = 3900;						// Calculated from Apollo 7 Mission Report and "Apollo by the numbers"
	SM_FuelMass = 4430;							// Apollo 7 (according to Mission Report), 

	CM_EmptyMass = 5430;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
	CM_FuelMass = CM_RCS_FUEL_PER_TANK * 2.;	// The CM has 2 tanks

	SII_EmptyMass = 12495;
	SII_FuelMass = 105795;

	SI_EmptyMass = 41874;
	SI_FuelMass = 411953;

	CalculateStageMass();

	//
	// Save the S4B information in the place where the Saturn class expects to find it. Maybe we should switch to
	// using the S4B variables rather than the 'stage 2' variables?
	//
	S4B_EmptyMass = SII_EmptyMass;
	S4B_FuelMass = SII_FuelMass;
	S4B_Mass = SII_Mass;

	//
	// Engines per stage.
	//

	SI_EngineNum = 8;
	SII_EngineNum = 1;
	SIII_EngineNum = 1;

	//
	// Default to not separating SLA panels.
	//
	SLAWillSeparate = false;

	// Moved to instantiation time
	// lvdc.init(this);
}

void CoeffFunc (double aoa, double M, double Re, double *cl, double *cm, double *cd)

{
	const int nlift = 11;
	double factor,dfact,lfact,frac,drag,lift;
	static const double AOA[nlift] =
		{-180.*RAD,-160.*RAD,-150.*RAD,-120.*RAD,-90.*RAD,0*RAD,90.*RAD,120.*RAD,150.*RAD,160.*RAD,180.*RAD};
	static const double Mach[17] = {0.0,0.7,0.9,1.1,1.2,1.35,1.65,2.0,3.0,5.0,8.0,10.5,13.5,18.2,21.5,31.0,50.0};
	static const double LFactor[17] = {0.3,0.392,0.466,0.607,0.641,0.488,0.446,0.435,0.416,0.415,0.405,0.400,0.385,0.385,0.375,0.35,0.33};
	static const double DFactor[17] = {0.9,0.944,0.991,1.068,1.044,1.270,1.28,1.267,1.213,1.134,1.15,1.158,1.18,1.18,1.193,1.224,1.25};
	static const double CL[nlift]  = {0.0,-0.9,-1.1,-0.5,0.0,0.0,0.0,0.5,1.1,0.9,0.0};
	static const double CM[nlift]  = {0.0,0.004,0.006,0.012,0.015,0.0,-0.015,-0.012,-0.006,-0.004,0.};
	static const double CD[nlift]  = {1.143,1.0,1.0,0.8,0.8,0.8,0.8,0.8,1.0,1.0,1.143};
	int j;
	factor = -5.0;
	dfact = 1.05;
	lfact = 0.94;
	for(j = 0; (j < 16) && (Mach[j+1] < M); j++);
	frac = (M-Mach[j])/(Mach[j+1]-Mach[j]);
	drag = dfact*(frac*DFactor[j+1]+(1.0-frac)*DFactor[j]);
	lift = drag * lfact*(frac*LFactor[j+1]+(1.0-frac)*LFactor[j]);
	for(j = 0; (j < nlift-1) && (AOA[j+1] < aoa); j++);
	frac = (aoa-AOA[j])/(AOA[j+1]-AOA[j]);
	*cd = drag*(frac*CD[j+1]+(1.0-frac)*CD[j]);
	*cl = lift*(frac*CL[j+1]+(1.0-frac)*CL[j]);
	*cm = factor*(frac*CM[j+1]+(1.0-frac)*CM[j]);
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		Saturn1bLoadMeshes();
		SaturnInitMeshes();
	}

	BaseInit();
	return new Saturn1b (hvessel, flightmodel);
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	--refcount;
	if (vessel) delete (Saturn1b *) vessel;
}

void Saturn1b::DoFirstTimestep(double simt)

{
	//
	// Do housekeeping on the first timestep after
	// initialisation. This allows us to pick up any
	// data that isn't saved and must be recreated after
	// loading... but which can only be done when the
	// whole system has been initialised.
	//

	//
	// Get the handles for any odds and ends that are out there.
	//

	char VName[256];

	habort = oapiGetVesselByName("Saturn_Abort");

	GetApolloName(VName); strcat (VName, "-TWR");
	hesc1 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-STG1");
	hstg1 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4BSTG");
	hs4bM = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B1");
	hs4b1 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B2");
	hs4b2 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B3");
	hs4b3 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B4");
	hs4b4=oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-SM");
	hSMJet = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-ASTPDM");
	hAstpDM = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-DCKPRB");
	hPROBE = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-EVA");
	hEVA = oapiGetVesselByName(VName);
	hSoyuz = oapiGetVesselByName("SOYUZ19");
	GetApolloName(VName); strcat (VName, "-INTSTG");
	hintstg = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-APEX");
	hApex = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-DROGUECHUTE");
	hDrogueChute = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-MAINCHUTE");
	hMainChute = oapiGetVesselByName(VName);	
	GetApolloName(VName); strcat (VName, "-OPTICSCOVER");
	hOpticsCover = oapiGetVesselByName(VName);	
	GetApolloName(VName); strcat(VName, "-NOSECAP");
	hNosecapVessel = oapiGetVesselByName(VName);
}

//
// Adjust the mixture ratio of the engine on the SIVB stage. This occured late in
// the flight to ensure that the fuel was fully burnt before the stage was dropped.
//

void Saturn1b::SetSIVBMixtureRatio (double ratio)

{
	double isp, thrust;

	// Hardcoded ISP and thrust according to the the Apollo 7 Saturn IB Report, NTRS ID 19900067467

	if (ratio >= 5.0) {
		thrust = 1009902;
		isp = 424*G;
	
	} else {
		thrust = 770000.;
		isp = 428*G;
	}

	//
	// For simplicity assume no ISP change at sea-level: SIVb stage should always
	// be in near-vacuum anyway.
	//

	SetThrusterIsp (th_main[0], isp, isp);
	SetThrusterMax0 (th_main[0], thrust);

	MixtureRatio = ratio;
}

void Saturn1b::Timestep (double simt, double simdt, double mjd)

{
	//
	// On the first timestep we just do basic setup
	// stuff and return. We seem to get called in at
	// least some cases before Orbiter is properly set
	// up, so the last thing we want to do is point the
	// engines in a wacky direction and then not be
	// called again for several seconds.
	//
	if (FirstTimestep){
		DoFirstTimestep(simt);
		FirstTimestep = false;
		return;
	}

	GenericTimestep(simt, simdt, mjd);

	if (stage < CSM_LEM_STAGE) {
	} else {
		GenericTimestepStage(simt, simdt);
	}

	//
	// CSM/LV separation
	//

	if (CSMLVPyros.Blown() && stage < CSM_LEM_STAGE) {
		SeparateStage(CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
	}

	//
	// CM/SM separation pyros
	//

	if (CMSMPyros.Blown() && stage < CM_STAGE)
	{
		SeparateStage(CM_STAGE);
		SetStage(CM_STAGE);
	}

	LastTimestep = simt;
}

void Saturn1b::clbkPostStep (double simt, double simdt, double mjd) {

	Saturn::clbkPostStep(simt, simdt, mjd);
}

/// 
/// \brief LVDC "Switch Selector" staging support utility function
/// 
void Saturn1b::SwitchSelector(int item){
	int i=0;

	switch(item){
	case 10:
		DeactivatePrelaunchVenting();
		break;
	case 11:
		ActivatePrelaunchVenting();
		break;
	case 12:
		SetThrusterGroupLevel(thg_main, 0);				// Ensure off
		for (i = 0; i < 5; i++) {						// Reconnect fuel to S1C engines
			SetThrusterResource(th_main[i], ph_1st);
		}
		CreateStageOne();								// Create hidden stage one, for later use in staging
		break;
	case 13:
		if (!UseATC && Scount.isValid()) {
			Scount.play();
			Scount.done();
		}
		break;
	case 14:
		DeactivatePrelaunchVenting();
		break;
	}
}

void Saturn1b::SISwitchSelector(int channel)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	switch (channel)
	{
	case 0: //Liftoff (NOT A REAL SWITCH SELECTOR EVENT)
		SetStage(LAUNCH_STAGE_ONE);								// Switch to stage one
		SetThrusterGroupLevel(thg_main, 1.0);				// Set full thrust, just in case
		contrailLevel = 1.0;
		if (LaunchS.isValid() && !LaunchS.isPlaying()) {	// And play launch sound
			LaunchS.play(NOLOOP, 255);
			LaunchS.done();
		}
		break;
	case 18: //Outboard Engines Cutoff
		// Move hidden S1B
		if (hstg1) {
			VESSELSTATUS vs;
			GetStatus(vs);
			S1B *stage1 = (S1B *)oapiGetVesselInterface(hstg1);
			stage1->DefSetState(&vs);
		}
		// Engine Shutdown
		for (int i = 0; i < 5; i++) {
			SetThrusterResource(th_main[i], NULL);
		}
		break;
	case 23: //S-IB/S-IVB Separation On
		SeparateStage(LAUNCH_STAGE_SIVB);
		SetStage(LAUNCH_STAGE_SIVB);
		AddRCS_S4B();
		SetSIVBThrusters(true);
		SetThrusterGroupLevel(thg_ver, 1.0);
		SetThrusterResource(th_main[0], ph_3rd);
		break;
	case 98: //Inboard Engines Cutoff
		SetThrusterResource(th_main[4], NULL);
		SetThrusterResource(th_main[5], NULL);
		SetThrusterResource(th_main[6], NULL);
		SetThrusterResource(th_main[7], NULL);
		SShutS.play(NOLOOP, 235);
		SShutS.done();
		break;
	default:
		break;
	}
}

void Saturn1b::SIVBSwitchSelector(int channel)
{
	if (stage >= CSM_LEM_STAGE) return;

	switch (channel)
	{
	case 32: //P.U. Mixture Ratio 4.5 On
		SetSIVBMixtureRatio(4.5); // Is this 4.7 or 4.2? AP8 says 4.5
		SPUShiftS.play(NOLOOP, 255);
		SPUShiftS.done();
		break;
	case 33: //P.U. Mixture Ratio 4.5 Off
		break;
	case 34: //P.U. Mixture Ratio 5.5 On
		SetSIVBMixtureRatio(5.5);
		break;
	case 35: //P.U. Mixture Ratio 5.5 Off
		break;
	default:
		break;
	}
}


//
// Save any state specific to the Saturn 1b.
//

void Saturn1b::SaveVehicleStats(FILEHANDLE scn){
	//
	// Fuel mass on launch. This could be made generic in saturn.cpp
	//

	oapiWriteScenario_float (scn, "SIFUELMASS", SI_FuelMass);
	oapiWriteScenario_float (scn, "SIIFUELMASS", SII_FuelMass);

	//
	// Stage masses.
	//

	oapiWriteScenario_float (scn, "SIEMPTYMASS", SI_EmptyMass);
	oapiWriteScenario_float (scn, "SIIEMPTYMASS", SII_EmptyMass);
}

void Saturn1b::SaveIU(FILEHANDLE scn)
{
	if (iu != NULL) { iu->SaveState(scn); }
}

void Saturn1b::LoadIU(FILEHANDLE scn)
{
	// If the IU does not yet exist, create it.
	if (iu == NULL) {
		iu = new IU1B;
	}
	iu->LoadState(scn);
}

void Saturn1b::clbkLoadStateEx (FILEHANDLE scn, void *vs){
	GetScenarioState(scn, vs);

	SetupMeshes();

	switch (stage) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStage();
		SetFirstStageEngines();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStage();
		SetSecondStageEngines();
		AddRCS_S4B();
		break;

	default:
		SetGenericStageState();
		break;
	}

	//
	// Setup of the generic systems
	//

	GenericLoadStateSetup();

	if (stage < STAGE_ORBIT_SIVB) {
		if (Crewed) {
			soundlib.LoadMissionSound(SPUShiftS, PUSHIFT_SOUND, PUSHIFT_SOUND);
		}
	}
}

void Saturn1b::ConfigureStageMeshes(int stage_state)
{
	//
	// This code all needs to be fixed up.
	//

	ClearMeshes();

	switch (stage_state) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStage();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStage();
		break;

	case CSM_LEM_STAGE:
		SetCSMStage();
		break;

	case CM_STAGE:
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_TWO:
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_THREE:
		SetChuteStage1();
		break;

	case CM_ENTRY_STAGE_FOUR:
		SetChuteStage2();
		break;

	case CM_ENTRY_STAGE_FIVE:
		SetChuteStage3();
		break;

	case CM_ENTRY_STAGE_SIX:
		SetChuteStage4();
		break;

	case CM_ENTRY_STAGE_SEVEN:
		SetSplashStage();
		break;

	case CM_RECOVERY_STAGE:
		SetRecovery();
		break;

	case CM_ENTRY_STAGE:
		SetReentryStage();
		break;
	}
}

void Saturn1b::ConfigureStageEngines(int stage_state)
{
	//
	// This code all needs to be fixed up.
	//

	switch (stage_state) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStageEngines();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStageEngines();
		break;
	}
}

void Saturn1b::clbkSetClassCaps (FILEHANDLE cfg)

{
	//
	// Scan the config file for specific information about this class.
	//

	char *line, buffer[1000];

	while (oapiReadScenario_nextline (cfg, line)) {
		ProcessConfigFileLine(cfg, line);
	}

	//
	// Scan the launchpad config file.
	//

	sprintf(buffer, "%s.launchpad.cfg", GetClassName());
	FILEHANDLE hFile = oapiOpenFile(buffer, FILE_IN, CONFIG);

	while (oapiReadScenario_nextline(hFile, line)) {
		ProcessConfigFileLine(hFile, line);
	}
	oapiCloseFile(hFile, FILE_IN);
}

void Saturn1b::SetVehicleStats()

{
	switch (VehicleNo)
	{
	case 206:
	case 207:
	case 208:
		SkylabSM = true;
		SkylabCM = true;

	//
	// Fall through. I think that Apollo 7 would have the
	// S1b panel with eight lights.
	//
	case 205:
		S1bPanel = true;
		break;
	}
}

void Saturn1b::CalculateStageMass()

{
	SI_Mass = SI_EmptyMass + SI_FuelMass;
	SII_Mass = SII_EmptyMass + SII_FuelMass;
	SM_Mass = SM_EmptyMass + SM_FuelMass + RCS_FUEL_PER_QUAD * 4.;
	CM_Mass = CM_EmptyMass + CM_FuelMass;

	//
	// This needs fixing.
	//

	if (SaturnHasCSM()) {
		Stage3Mass = SM_Mass + CM_Mass;
	}
	else {
		Stage3Mass = 0.0;
	}

	Stage2Mass = Stage3Mass + SII_EmptyMass + S4PL_Mass;
	Stage1Mass = Stage2Mass + SI_EmptyMass + SII_FuelMass + Abort_Mass;
}

int Saturn1b::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	// Nothing for now

	return Saturn::clbkConsumeBufferedKey(key, down, kstate);
}

void Saturn1b::SetRandomFailures()
{
	Saturn::SetRandomFailures();
	//
	// Set up launch failures.
	//

	if (!LaunchFail.Init)
	{
		LaunchFail.Init = 1;

		//
		// Engine failure times for first stage.
		//

		bool EarlySICutoff[8];
		double FirstStageFailureTime[8];

		for (int i = 0;i < 8;i++)
		{
			EarlySICutoff[i] = 0;
			FirstStageFailureTime[i] = 0.0;
		}

		for (int i = 0;i < 8;i++)
		{
			if (!(random() & (int)(127.0 / FailureMultiplier)))
			{
				EarlySICutoff[i] = 1;
				FirstStageFailureTime[i] = 20.0 + ((double)(random() & 1023) / 10.0);
			}
		}

		iu->GetEDS()->SetEngineFailureParameters(EarlySICutoff, FirstStageFailureTime, NULL, NULL);

		if (!(random() & 127))
		{
			LaunchFail.LETAutoJetFail = 1;
		}
		if (!(random() & 63))
		{
			LaunchFail.SIIAutoSepFail = 1;
		}
		if (!(random() & 255))
		{
			LaunchFail.LESJetMotorFail = 1;
		}
	}
}