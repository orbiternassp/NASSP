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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.18  2005/08/15 18:48:50  movieman523
  *	Moved the stage destroy code into a generic function for Saturn V and 1b.
  *	
  *	Revision 1.17  2005/08/15 02:37:57  movieman523
  *	SM RCS is now wired up.
  *	
  *	Revision 1.16  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.15  2005/08/08 20:32:56  movieman523
  *	Added initial support for offsetting the mission timer and event timer from MissionTime: the real timers could be adjusted using the switches on the control panel (which aren't wired up yet), and the event timer would reset to zero on an abort.
  *	
  *	Revision 1.14  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.13  2005/08/01 19:07:46  movieman523
  *	Genericised code to deal with SM destruction on re-entry, and did some tidying up of Saturn 1b code.
  *	
  *	Revision 1.12  2005/07/31 11:59:41  movieman523
  *	Added first mixture ratio shift to Saturn 1b.
  *	
  *	Revision 1.11  2005/07/31 01:43:12  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.10  2005/07/30 02:05:47  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.9  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.8  2005/07/05 17:55:29  tschachim
  *	Fixed behavior of the CmSmSep1/2Switches
  *	
  *	Revision 1.7  2005/06/06 12:19:46  tschachim
  *	New switches
  *	
  *	Revision 1.6  2005/04/14 23:10:03  movieman523
  *	Fixed compiler warning (mesh_dir not used).
  *	
  *	Revision 1.5  2005/03/28 05:50:08  chode99
  *	Added support for varying payloads as in the Saturn V.
  *	
  *	Revision 1.4  2005/03/25 21:27:17  chode99
  *	Added retro rockets to SIB first stage (interstage).
  *	
  *	Revision 1.3  2005/02/20 05:24:57  chode99
  *	Changes to implement realistic CM aerodynamics. Created callback function "CoeffFunc" in Saturn1b.cpp and Saturn5.cpp. Substituted CreateAirfoil for older lift functions.
  *	
  *	Revision 1.2  2005/02/19 00:02:38  movieman523
  *	Reduced volume of APS sound playback.
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "saturn.h"

#include "saturn1b.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

// ==============================================================
// Global parameters
// ==============================================================

GDIParams g_Param;

static int refcount = 0;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hastp;

const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;
const double  CSM_THRUST_ATT   = 200.34*KGF;
const double  CSM_ISP          = 773*SEC;

Saturn1b::Saturn1b (OBJHANDLE hObj, int fmodel)
: Saturn (hObj, fmodel)

{
	hMaster = hObj;
	initSaturn1b();
}

Saturn1b::~Saturn1b()

{
	ReleaseSurfaces();
}

//
// Default pitch program.
//

const double default_met[PITCH_TABLE_SIZE]    = { 0,  50, 75, 80,  90, 110, 140,   160, 170,  205, 450, 480, 490, 500, 535, 700};   // MET in sec
const double default_cpitch[PITCH_TABLE_SIZE] = {90,  80, 70, 60,  53,  45,  40,    35,   35,  35,  20,  20,  20,  20,  20,  20};	// Commanded pitch in °

//const double default_met[PITCH_TABLE_SIZE]    = { 0, 25, 60, 95, 130, 145, 152, 165, 200, 235, 270, 305, 330, 450, 500, 700};   // MET in sec
//const double default_cpitch[PITCH_TABLE_SIZE] = {90, 90, 80, 70,  60,  55,  55,  50,  40,  30,  20,  10,   0,   0,   0,   0};	// Commanded pitch in °


void Saturn1b::initSaturn1b()

{
	//
	// Do the basic initialisation from the
	// generic Saturn class first.
	//

	initSaturn();

	RelPos = _V(0.0,0.0,0.0);

	hSoyuz = 0;
	hAstpDM = 0;

	DestroyAstp = false;

	ReadyAstp = false;
	ReadyAstp1 = false;
	S4BASTP = false;

	MasterVessel = false;
	TargetDocked = false;

	if (strcmp(GetName(), "AS-211")==0) {
		ASTPMission = true;
	}

	//
	// Pitch program.
	//

	for (int i = 0; i < PITCH_TABLE_SIZE; i++) {
		met[i] = default_met[i];
		cpitch[i] = default_cpitch[i];
	}

	//
	// Typical center engine shutdown time.
	//

	FirstStageCentreShutdownTime = 140.0;

	//
	// PU shift time.
	//

	SecondStagePUShiftTime = 450.0;

	//
	// IGM Start time.
	//

	IGMStartTime = 170;

	//
	// Default ISP and thrust values.
	//


	ISP_FIRST_SL    = 262*G;
	ISP_FIRST_VAC   = 292*G;
	THRUST_FIRST_VAC	= 1030200;

	ISP_SECOND_SL   = 300*G;//300*G;
	ISP_SECOND_VAC  = 419*G;//421*G;

	//
	// Note: thrust values are _per engine_, not per stage.
	//

	THRUST_SECOND_VAC  = 1001000;

	SM_EmptyMass = 6100;
	SM_FuelMass = 2800;

	CM_EmptyMass = 5700;
	CM_FuelMass = 75;

	SII_EmptyMass = 12900;
	SII_FuelMass = 105900;

	SI_EmptyMass = 41594;
	SI_FuelMass = 407100;
}

void CoeffFunc (double aoa, double M, double Re, double *cl, double *cm, double *cd)

{
	const int nlift = 11;
	const double factor=0.0;
	static const double AOA[nlift] =
		{-180*RAD,-160*RAD,-150*RAD,-120*RAD,-90*RAD,0*RAD,90*RAD,120*RAD,150*RAD,160*RAD,180*RAD};
	static const double CL[nlift]  = {0.0,-0.3,-0.425,-0.215,0.0,0.0,0.0,0.215,0.425,0.3,0.0};
	static const double CM[nlift]  = {0.0,0.004,0.006,0.012,0.015,0.0,-0.015,-0.012,-0.006,-0.004,0.};
	static const double CD[nlift]  = {1.6,1.4,1.0,0.6,0.75,0,0.75,0.6,1.0,1.4,1.6};
	static double SCL[nlift-1];
	static double SCM[nlift-1];
	static double SCD[nlift-1];
	for(int j = 0; j < nlift-1; j++){
		SCL[j]= (CL[j+1]-CL[j])/(AOA[j+1]-AOA[j]);
		SCM[j]= (CM[j+1]-CM[j])/(AOA[j+1]-AOA[j]);
		SCD[j]= (CD[j+1]-CD[j])/(AOA[j+1]-AOA[j]);
	}
	for (int i = 0; i < nlift-1 && AOA[i+1] < aoa; i++);
	*cl = (CL[i] + (aoa-AOA[i])*SCL[i]);
	*cm = factor*(CM[i] + (aoa-AOA[i])*SCM[i]);
	*cd = (CD[i] + (aoa-AOA[i])*SCD[i]);
}

double LiftCoeff (double aoa)
{
	const int nlift = 9;
	static const double AOA[nlift] =
		{-180*RAD,-155*RAD,-154*RAD,-34*RAD,0*RAD,34*RAD,154*RAD,155*RAD,180*RAD};
//static const double CL[nlift]  = {0.06375,  0.375,       0,      0,    0,
//0,      0,      0.1275, 0.06375};
	static const double CL[nlift]  = {0.1275,  0.3575,       0.0975,      0.0375,
		0.0375,      0.0975, 0.3575, 0.1275};
	static const double SCL[nlift] = {(CL[1]-CL[0])/(AOA[1]-AOA[0]),
		(CL[2]-CL[1])/(AOA[2]-AOA[1]),
		(CL[3]-CL[2])/(AOA[3]-AOA[2]),
		(CL[4]-CL[3])/(AOA[4]-AOA[3]),
		(CL[5]-CL[4])/(AOA[5]-AOA[4]), (CL[6]-CL[5])/(AOA[6]-AOA[5]),
		(CL[7]-CL[6])/(AOA[7]-AOA[6]), (CL[8]-CL[7])/(AOA[8]-AOA[7])};

	for (int i = 0; i < nlift-1 && AOA[i+1] < aoa; i++);
	return -(CL[i] + (aoa-AOA[i])*SCL[i]);
}

//
// Are any of the functions from here to MemoVessel actually used?
//

void Saturn1b::SetupSlaveUnDockedmode(OBJHANDLE hTarget)

{
	VECTOR3 PMIO = _V(0,0,0);
	VECTOR3 PMIM = _V(0,0,0);
	VESSEL *targetvessel;
	targetvessel=oapiGetVesselInterface(hTarget);
	targetvessel->SetEmptyMass((targetvessel->GetMass()-targetvessel->GetFuelMass()) - GetMass());
	GetPMI(PMIO);
	targetvessel->GetPMI(PMIM);
	targetvessel->SetPMI((PMIM-PMIO));
}

void Saturn1b::SetupUnDockedmode(OBJHANDLE hTarget)

{
	VECTOR3 PMIO = _V(0,0,0);
	VECTOR3 PMIM = _V(0,0,0);
	VESSEL *targetvessel;
	targetvessel=oapiGetVesselInterface(hTarget);
	SetEmptyMass((GetMass() - GetFuelMass()) -targetvessel->GetMass());
	GetPMI(PMIO);
	targetvessel->GetPMI(PMIM);
	SetPMI((PMIO-PMIM));
}

void Saturn1b::SetupStage(OBJHANDLE hTarget)
{
	VESSEL *targetvessel;
	targetvessel=oapiGetVesselInterface(hTarget);

	targetvessel->SetEmptyMass (17000);
	targetvessel->SetMaxFuelMass (114000);
	targetvessel->SetFuelMass(GetFuelMass());
	targetvessel->SetISP (4160);
	targetvessel->SetMaxThrust (ENGINE_MAIN, 981000);
	targetvessel->SetMaxThrust (ENGINE_ATTITUDE, 3e3);
	targetvessel->SetPMI (_V(94,94,20));
	targetvessel->SetCrossSections (_V(267,267,97));
	targetvessel->SetCW (0.1, 0.3, 1.4, 1.4);
	targetvessel->SetRotDrag (_V(0.7,0.7,1.2));
	targetvessel->SetPitchMomentScale (0);
	targetvessel->SetBankMomentScale (0);
	targetvessel->SetLiftCoeffFunc (0);
/*	if(SIVBPayload == PAYLOAD_TARGET){
		mesh_dir=_V(-1.0,-1.1,13.3);
		targetvessel->AddMesh (hCOAStarget, &mesh_dir);
	}
	else if(SIVBPayload == PAYLOAD_ASTP){
		mesh_dir=_V(0,0,13.3);
		targetvessel->AddMesh (hastp, &mesh_dir);
	}
	else if(SIVBPayload == PAYLOAD_LM1){
		mesh_dir=_V(0,0,13.3);
		targetvessel->AddMesh (hCOAStarget, &mesh_dir);
	}*/
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

	strcpy (VName, GetName()); strcat (VName, "-TWR");
	hesc1 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-STG1");
	hstg1 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4BSTG");
	hs4bM = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B1");
	hs4b1 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B2");
	hs4b2 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B3");
	hs4b3 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B4");
	hs4b4=oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-SM");
	hSMJet = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-ASTPDM");
	hAstpDM = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-DCKPRB");
	hPROBE = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-EVA");
	hEVA = oapiGetVesselByName(VName);
	hSoyuz = oapiGetVesselByName("SOYUZ19");
	strcpy (VName, GetName()); strcat (VName, "-INTSTG");
	hintstg = oapiGetVesselByName(VName);

	//
	// Stage One mesh doesn't include the interstage, so add it if
	// the stage exists.
	//

	if (hstg1)
		AddStageOneInterstage();
}

void Saturn1b::StageOne(double simt)

{
	VESSELSTATUS vs;
	GetStatus(vs);

	if (GetEngineLevel(ENGINE_MAIN) > 0.65 ) {
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else {
		LAUNCHIND[7] = false;
		ENGIND[5] = false;
	}

	if (GetEngineLevel(ENGINE_MAIN) < 0.3 && MissionTime < 100 && EDSSwitch.GetState() && MissionTime > 10){
		bAbort = true;
	}

	switch (StageState) {

	case 0:

		//
		// Shut down center engine at 2% fuel or if acceleration goes
		// over 3.98g, or at specified time.
		//

		if ((actualFUEL <= 2) || (MissionTime >= FirstStageCentreShutdownTime)) { // || (aHAcc > (3.98*G)))) {
			SetEngineIndicator(5);
			SetThrusterResource(th_main[4],NULL);
			SetThrusterResource(th_main[5],NULL);
			SetThrusterResource(th_main[6],NULL);
			SetThrusterResource(th_main[7],NULL);
			ClearLiftoffLight();
			SShutS.play(NOLOOP,235);
			SShutS.done();
			StageState++;
		}
		break;

	//
	// We don't actually have time here to play the countdown before seperation after the center
	// engine shuts down: with the Saturn 1 there are only a few seconds between the two events.
	//

	case 1:
		if (GetFuelMass() == 0 || bManualSeparate)
		{
			NextMissionEventTime = MissionTime + 0.7;
			SetEngineIndicators();
			StageState++;
		}
		break;

	case 2:
		if (MissionTime >= NextMissionEventTime){
			SShutS.done();
			ClearEngineIndicators();
			SeparateStage (stage);
			bManualSeparate = false;
			SeparationS.play(NOLOOP, 245);
			SetStage(LAUNCH_STAGE_TWO);
		}
		return;
	}

	if (autopilot && CMCswitch) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunch1();
	}
}

//
// Start up the SIVB and separate the launch tower.
//

void Saturn1b::StageStartSIVB(double simt)

{
	if (GetEngineLevel(ENGINE_MAIN) > 0.65 ){
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else{
		ENGIND[5] = false;
		LAUNCHIND[7] = false;
	}

	if (autopilot && CMCswitch) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunchSIVB();
	}

	if(GetThrusterGroupLevel(thg_ver) > 0){
		LAUNCHIND[6] = true;
	}
	else{
		LAUNCHIND[6] = false;
	}

	switch (StageState) {

	case 0:
//		if (hstg2)
//			Ullage2(hstg2,5);
		SepS.play(LOOP, 130);
		SetThrusterGroupLevel(thg_ver,1.0);
		NextMissionEventTime = MissionTime + 2.0;
		SetSIVBMixtureRatio(5.0);
		StageState++;
		break;

	case 1:
		if (MissionTime >= NextMissionEventTime) {
			LastMissionEventTime = NextMissionEventTime;
			NextMissionEventTime += 2.5;
			SetEngineIndicator(1);
			StageState++;
		}
		break;

	//
	// Start bringing engine up to power.
	//

	case 2:
		if (MissionTime  < NextMissionEventTime) {
			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterLevel(th_main[0], 0.9 * (deltat / 2.5));
		}
		else {
			SetThrusterLevel(th_main[0], 0.9);
			LastMissionEventTime = NextMissionEventTime;
			NextMissionEventTime += 2.1;
			StageState++;
		}
		if (GetThrusterLevel(th_main[0]) > 0.65) {
			ClearEngineIndicator(1);
		}
		break;

	//
	// Bring engine to full power.
	//

	case 3:
		if (MissionTime  < NextMissionEventTime) {
			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterLevel(th_main[0], 0.9 + (deltat / 21.0));
		}
		else {
			SetThrusterLevel(th_main[0], 1.0);
			SepS.stop();
			AddRCS_S4B();
			SetThrusterGroupLevel(thg_ver, 0.0);
			LAUNCHIND[6] = false;
			NextMissionEventTime = MissionTime + 2.05;
			StageState++;
		}
		break;

	//
	// First mixture ratio shift.
	//

	case 4:
		if (MissionTime >= NextMissionEventTime) {
			SetSIVBMixtureRatio(5.5);
			NextMissionEventTime = MissionTime + 17.95;
			StageState++;
		}
		break;

	//
	// Jettison LET and move on.
	//

	case 5:
		if (MissionTime > NextMissionEventTime) {
			SeparateStage(stage);
			TowerJS.play();
			TowerJS.done();
			SetStage(LAUNCH_STAGE_SIVB);
		}
		return;
	}

	if(CsmLvSepSwitch.GetState()) {
		bManualSeparate = true;
	}

	if (StageState > 3)
		SetSIVBThrusters();

	if (bManualSeparate || bAbort)
	{
		SepS.stop();
		bManualSeparate = false;
		SeparateStage (stage);
		SetStage(CSM_LEM_STAGE);
		if (bAbort){
			SPSswitch.SetState(true);
			ABORT_IND = true;
			StartAbort();
			SetThrusterGroupLevel(thg_main, 1.0);
			bAbort = false;
			autopilot=false;
		}
	}
}

void Saturn1b::StageLaunchSIVB(double simt)

{
	if (GetEngineLevel(ENGINE_MAIN) > 0.65 )
	{
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else {
		ENGIND[5] = false;
		LAUNCHIND[7] = false;
	}

    if (autopilot && CMCswitch) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunchSIVB();
	}

	switch (StageState) {

	case 0:
		SetThrusterLevel(th_main[0], 1.0);
		SetSIVBMixtureRatio(5.5);
		if (Crewed)
			SwindowS.play();
		SwindowS.done();
		NextMissionEventTime = MissionTime + 8.65;
		StageState++;
		break;

	//
	// Second mixture ratio shift.
	//

	case 1:
		if (MissionTime >= SecondStagePUShiftTime) {
			if (Crewed) {
				SPUShiftS.play();
				SPUShiftS.done();
			}
			SetSIVBMixtureRatio(4.5);
			StageState++;
		}
		break;

	//
	// Shutdown.
	//

	case 2:
		if (GetEngineLevel(ENGINE_MAIN) <= 0) {
			NextMissionEventTime = MissionTime + 10.0;
			S4CutS.play();
			S4CutS.done();

			//
			// Make sure we clear out any sounds that haven't been played.
			//
			S2ShutS.done();
			SPUShiftS.done();

			ThrustAdjust = 1.0;
			SetStage(STAGE_ORBIT_SIVB);
			SetSIVBThrusters();
		}
		break;
	}

	if(CsmLvSepSwitch.GetState()) {
		bManualSeparate = true;
	}

	if (bManualSeparate || bAbort)
	{
		bManualSeparate = false;
		SeparateStage(stage);
		SetStage(CSM_LEM_STAGE);
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
		if (bAbort){
			SPSswitch.SetState(true);
			ABORT_IND = true;
			StartAbort();
			SetThrusterGroupLevel(thg_main, 1.0);
			bAbort = false;
			autopilot= false;
		}
		return;
	}
}

//
// Adjust the mixture ratio of the engine on the SIVB stage. This occured late in
// the flight to ensure that the fuel was fully burnt before the stage was dropped.
//

void Saturn1b::SetSIVBMixtureRatio (double ratio)

{
	double isp;

	isp = GetJ2ISP(ratio);

	//
	// For simplicity assume no ISP change at sea-level: SII stage should always
	// be in near-vacuum anyway.
	//

	SetThrusterIsp (th_main[0], isp, ISP_SECOND_SL);
	SetThrusterMax0 (th_main[0], THRUST_SECOND_VAC * ThrustAdjust);
}

void Saturn1b::Timestep (double simt, double simdt)

{
	//
	// On the first timestep we just do basic setup
	// stuff and return. We seem to get called in at
	// least some cases before Orbiter is properly set
	// up, so the last thing we want to do is point the
	// engines in a wacky direction and then not be
	// called again for several seconds.
	//
	//	sprintf (oapiDebugString(), "Dockstate: %d", dockstate);
	if (FirstTimestep) {
		DoFirstTimestep(simt);
		FirstTimestep = false;
		return;
	}

	GenericTimestep(simt, simdt);

	if (hAstpDM){
		if (DestroyAstp) {
			Undock(0);
			ReadyAstp1 = false;
			oapiDeleteVessel(hAstpDM);
			hAstpDM=NULL;
			DestroyAstp=false;
		}
		if (GetDockStatus(GetDockHandle(0)) == hAstpDM ){
			if(dockstate == 4){
				if(ASTPMission)
					ReadyAstp1=true;
			bManualUnDock = true;
			}
		}
	}
	else{
		if(ASTPMission) {
			char VName[256];
			strcpy (VName, GetName()); strcat (VName, "-ASTPDM");
			hAstpDM = oapiGetVesselByName(VName);
		}
	}

	if (hs4bM){
		if (GetDockStatus(GetDockHandle(0)) == hs4bM) {
			if(ASTPMission)
				ReadyAstp=true;
			dockstate=2;
				//	sprintf(oapiDebugString() ,"S4B %f");
		}
		else{
			ReadyAstp=false;
				//	sprintf(oapiDebugString() ,"NOT S4B %f");
		}
		if (dockstate>=2 && !S4BASTP){
			SetS4B();
			if(ASTPMission)
				S4BASTP=true;
		}
	}else {
		char VName[256];

		strcpy (VName, GetName()); strcat (VName, "-S4BSTG");
		hs4bM = oapiGetVesselByName(VName);
	}

	if (bAbort && stage <= LAUNCH_STAGE_TWO ){
		SetEngineLevel(ENGINE_MAIN, 0);
		SeparateStage (stage);
		StartAbort();
		stage = CSM_ABORT_STAGE;
		bAbort=false;
		return;
	}

	if (stage == CSM_LEM_STAGE)
	{
		if(simt>0.5) AttitudeLaunch4();
		if(RPswitch13 && !HatchOpen){
			bToggleHatch = true;
		}
		else if(!RPswitch13 && HatchOpen){
			bToggleHatch = true;
		}

		if (RPswitch15.GetState() && SivbLmSepSwitch.GetState()){
			if (ASTPMission) {
				//sprintf(oapiDebugString() ,"click %f");
				SivbLmSepSwitch = false;
				if (ReadyAstp||ReadyAstp1||dockstate==3) {
					bManualUnDock = true;
					RPswitch15 = false;
				}
			}
		}
		if (CsmLmFinalSep2Switch.GetState()) {
			Undock(0);
		}
		if (CsmLmFinalSep1Switch.GetState()) {
			if (dockstate == 3) {
				ProbeJetison = true;
				bManualUnDock = true;
			}
		}
		if (CmSmSep1Switch.GetState() || CmSmSep2Switch.GetState()) {
			bManualSeparate=true;
		}
		if (RPswitch14 && HatchOpen){
			ToggleEva = true;
			RPswitch14=false;
		}

		if(!probeOn){
			if (GetDockStatus(GetDockHandle(0))!=NULL){
				Undock(0);
				CrashBumpS.play(NOLOOP,150);
			}
		}
		else{
			if(GetDockStatus(GetDockHandle(0))==Saturn::hs4bM){

			//sprintf(oapiDebugString() ,"loade %f");
			}
			else{
				//sprintf(oapiDebugString() ,NULL);
			}
		}

		for (int i=0 ;i<6;i++){
			LAUNCHIND[i]=false;
		}

		if (EVA_IP){
			if(!hEVA){
			ToggleEVA();
			}
		}
		if ((simt-(2+release_time))>=0){
			SetAttitudeLinLevel(2,0);
			}
		if (ToggleEva){
		ToggleEVA();
		if(ASTPMission && dockstate == 3){
//			UINT meshidx;
//			VECTOR3 mesh_dir=_V(0.0,-0.2,37.40-12.25-21.5);
//			meshidx = AddMesh (hastp, &mesh_dir);
//			SetMeshVisibleInternal (meshidx, true);
		}

		}
		if (bToggleHatch){
		ToggelHatch();
		bToggleHatch=false;
		if(ASTPMission && dockstate == 3){
//			UINT meshidx;
//			VECTOR3 mesh_dir=_V(0.0,-0.2,37.40-12.25-21.5);
//			meshidx = AddMesh (hastp, &mesh_dir);
//			SetMeshVisibleInternal (meshidx, true);
		}



		}
		SetView();
		if (dockstate == 2){

		}
		if (bManualUnDock)
		{
			release_time = simt;
			DockStage (dockstate);
			bManualUnDock=false;
		}

		if (SMRCSActive()) {
			SetThrusterResource(th_att_lin[0],ph_rcs0);
			SetThrusterResource(th_att_lin[1],ph_rcs0);
			SetThrusterResource(th_att_lin[2],ph_rcs0);
			SetThrusterResource(th_att_lin[3],ph_rcs0);
			SetThrusterResource(th_att_rot[0],ph_rcs0);
			SetThrusterResource(th_att_rot[1],ph_rcs0);
			SetThrusterResource(th_att_rot[2],ph_rcs0);
			SetThrusterResource(th_att_rot[3],ph_rcs0);
		}
		else {
			SetThrusterResource(th_att_lin[0],NULL);
			SetThrusterResource(th_att_lin[1],NULL);
			SetThrusterResource(th_att_lin[2],NULL);
			SetThrusterResource(th_att_lin[3],NULL);
			SetThrusterResource(th_att_rot[0],NULL);
			SetThrusterResource(th_att_rot[1],NULL);
			SetThrusterResource(th_att_rot[2],NULL);
			SetThrusterResource(th_att_rot[3],NULL);
		}
		if (SMRCSActive()) {
			SetThrusterResource(th_att_rot[4],ph_rcs0);
			SetThrusterResource(th_att_rot[5],ph_rcs0);
			SetThrusterResource(th_att_rot[7],ph_rcs0);
			SetThrusterResource(th_att_rot[6],ph_rcs0);
			SetThrusterResource(th_att_lin[4],ph_rcs0);
			SetThrusterResource(th_att_lin[5],ph_rcs0);
			SetThrusterResource(th_att_lin[7],ph_rcs0);
			SetThrusterResource(th_att_lin[6],ph_rcs0);
		}
		else {
			SetThrusterResource(th_att_rot[4],NULL);
			SetThrusterResource(th_att_rot[5],NULL);
			SetThrusterResource(th_att_rot[6],NULL);
			SetThrusterResource(th_att_rot[7],NULL);
			SetThrusterResource(th_att_lin[4],NULL);
			SetThrusterResource(th_att_lin[5],NULL);
			SetThrusterResource(th_att_lin[6],NULL);
			SetThrusterResource(th_att_lin[7],NULL);
		}
		if(SMRCSActive()) {
			for(int i=8;i<24;i++){
				SetThrusterResource(th_att_rot[i],ph_rcs0);
				SetThrusterResource(th_att_lin[i],ph_rcs0);
			}
		}
		else {
			for(int i=8;i<24;i++){
				SetThrusterResource(th_att_rot[i],NULL);
				SetThrusterResource(th_att_lin[i],NULL);
			}
		}

		if(LPswitch4.GetState() && RCS_Full){
			for(int i=0;i<24;i++){
				DelThruster(th_att_rot[i]);
				DelThruster(th_att_lin[i]);
				DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
				DelThrusterGroup(THGROUP_ATT_FORWARD,true);
				DelThrusterGroup(THGROUP_ATT_BACK,true);
				DelThrusterGroup(THGROUP_ATT_PITCHDOWN,true);
				DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
				DelThrusterGroup(THGROUP_ATT_YAWRIGHT,true);
				DelThrusterGroup(THGROUP_ATT_YAWLEFT,true);
			}
			AddRCSJets(-1.80,995);
			//sprintf(oapiDebugString(), "RCS HALF");
			RCS_Full=false;
		}
		else if (!LPswitch4 && !RCS_Full) {
			for(int i=0;i<24;i++){
				DelThruster(th_att_rot[i]);
				DelThruster(th_att_lin[i]);
				DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
				DelThrusterGroup(THGROUP_ATT_FORWARD,true);
				DelThrusterGroup(THGROUP_ATT_BACK,true);
				DelThrusterGroup(THGROUP_ATT_PITCHDOWN,true);
				DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
				DelThrusterGroup(THGROUP_ATT_YAWRIGHT,true);
				DelThrusterGroup(THGROUP_ATT_YAWLEFT,true);
			}
			AddRCSJets(-1.80,1990);
			RCS_Full=true;
			//sprintf(oapiDebugString(), "RCS FULL");
		}

		CheckSPSState();

		if (bManualSeparate)
		{

			SeparateStage (stage);
			bManualSeparate=false;
			ignition_SMtime=simt;
			setupSM(hSMJet);
			SetStage(CM_STAGE);
		}
	}
	else  if (stage == CSM_ABORT_STAGE)
	{
		SetEngineLevel(ENGINE_MAIN,1);
	//sprintf(oapiDebugString(), "Mode Abort 1B%f", abortTimer);
		//if (bManualSeparate)
        if (GetFuelMass() == 0 && abortTimer == 0){
			LAUNCHIND[4]=true;
			abortTimer=simt;
		}
		if (abortTimer>0){
			if ((simt-(0.5+abortTimer))>=0){
				ActivateNavmode(NAVMODE_KILLROT);
			}

			if ((simt-(1+abortTimer))>=0){
				double pitch;
				pitch= GetPitch();
				if (pitch >0) {
					SetPitchMomentScale (+5e-3);
					SetBankMomentScale (+5e-3);
					SetLiftCoeffFunc (0);
				}
			}


			if (simt-(20+abortTimer)>=0)
			{
				LAUNCHIND[2]=true;
				bManualSeparate=true;
				abortTimer = 0;
			}
			bool altlow=false;

			double altitude=600;
			altitude=GetAltitude();
			if (altitude < 500 ){
				altlow = true;
			}
		if (bManualSeparate ||  altlow)
			{
				SeparateStage (stage);
				SetStage(CM_STAGE);
				bManualSeparate=false;
				abortTimer = 0;

			}
		}
	}

	switch (stage) {

	case LAUNCH_STAGE_ONE:
		StageOne(simt);
		break;

	case LAUNCH_STAGE_TWO:
		StageStartSIVB(simt);
		break;

	case LAUNCH_STAGE_SIVB:
		StageLaunchSIVB(simt);
		break;

	case STAGE_ORBIT_SIVB:
		StageOrbitSIVB(simt);
		break;

	default:
		GenericTimestepStage(simt, simdt);
		break;
	}

	LastTimestep = simt;
}

//
// Save any state specific to the Saturn 1b.
//

void Saturn1b::SaveVehicleStats(FILEHANDLE scn)

{
	if (dockstate==6) {
		oapiWriteScenario_float (scn, "DOCKANGLE", DockAngle);
	}
}

void Saturn1b::clbkLoadStateEx (FILEHANDLE scn, void *vs)

{
	GetScenarioState(scn, vs);

	switch (stage) {

	case ROLLOUT_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStage();
		break;

	case LAUNCH_STAGE_TWO:
		SetSecondStage1();
		break;

	case LAUNCH_STAGE_SIVB:
		SetSecondStage2();
		break;

	case STAGE_ORBIT_SIVB:
		SetSecondStage2();
		AddRCS_S4B();
		break;

	case CSM_LEM_STAGE:
		SetCSMStage();
		switch (dockstate) {
		case 1:


		case 2:
				break;
		case 3:
			if(ASTPMission)
				SetASTPStage();
			break;
		case 4:
			break;
		case 5:
			SetCSM2Stage();
				break;
		}

		if (EVA_IP){
			SetupEVA();
		}
		break;

		default:
			SetGenericStageState();
			break;
	}

	GenericLoadStateSetup();
	FirstTimestep = true;

	if (stage < STAGE_ORBIT_SIVB) {
		if (Crewed) {
			soundlib.LoadMissionSound(SPUShiftS, PUSHIFT_SOUND, PUSHIFT_SOUND);
		}
	}
}


void Saturn1b::SetVehicleStats()

{
}

void Saturn1b::CalculateStageMass()

{
	SI_Mass = SI_EmptyMass + SI_FuelMass;
	SII_Mass = SII_EmptyMass + SII_FuelMass;
	SM_Mass = SM_EmptyMass + SM_FuelMass;
	CM_Mass = CM_EmptyMass + CM_FuelMass;

	Stage3Mass = SM_Mass + CM_Mass;
	Stage2Mass = Stage3Mass + SII_EmptyMass;
	Stage1Mass = Stage2Mass + SI_EmptyMass + SII_FuelMass + Abort_Mass;
}
