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

#include "saturn.h"

#include "saturn1b.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

// ==============================================================
// Global parameters
// ==============================================================

GDIParams g_Param;

static int refcount = 0;

const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;
const double  CSM_THRUST_ATT   = 200.34*KGF;
const double  CSM_ISP          = 773*SEC;


const double ISP_FIRST_SL    = 262*G;
const double ISP_FIRST_VAC   = 292*G;
const double ISP_SECOND_SL   = 300*G;//300*G;
const double ISP_SECOND_VAC  = 421*G;//421*G;
const double THRUST_FIRST_VAC	= 840426*G;
const double THRUST_SECOND_VAC  = 85200*G;//115200*G;

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
}

void Saturn1b::SeparateStage (int stage)

{
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;
	VESSELSTATUS vs3;
	VESSELSTATUS vs4;
	VESSELSTATUS vs5;
	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);
	VECTOR3 ofs3 = _V(0,0,0);
	VECTOR3 ofs4 = _V(0,0,0);
	VECTOR3 ofs5 = _V(0,0,0);
	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);
	VECTOR3 vel3 = _V(0,0,0);
	VECTOR3 vel4 = _V(0,0,0);
	VECTOR3 vel5 = _V(0,0,0);

	GetStatus (vs1);
	GetStatus (vs2);
	GetStatus (vs3);
	GetStatus (vs4);
	GetStatus (vs5);
	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;
	vs3.eng_main = vs3.eng_hovr = 0.0;
	vs4.eng_main = vs4.eng_hovr = 0.0;
	vs5.eng_main = vs5.eng_hovr = 0.0;

	if (stage == LAUNCH_STAGE_ONE && !bAbort)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0,0,-4.0);
	}
	if (stage == LAUNCH_STAGE_ONE && bAbort)
	{
		ofs1= OFS_ABORT;
		vel1 = _V(0,0,-4.0);
	}

	if ((stage == LAUNCH_STAGE_TWO || stage == CSM_ABORT_STAGE) && !bAbort)
	{
		ofs1 = OFS_TOWER;
		vel1 = _V(15.0,15.0,106.0);
	}

	if (stage == LAUNCH_STAGE_TWO && bAbort)
	{
		ofs1= OFS_ABORT2;
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)
	{
	 	ofs1 = OFS_STAGE2;
		vel1 = _V(0,0,-0.235);
		ofs2 = OFS_STAGE21;
		vel2 = _V(0.5,0.5,-0.55);
		ofs3 = OFS_STAGE22;
		vel3 = _V(-0.5,0.5,-0.55);
		ofs4 = OFS_STAGE23;
		vel4 = _V(0.5,-0.5,-0.55);
		ofs5 = OFS_STAGE24;
		vel5 = _V(-0.5,-0.5,-0.55);
	}

	if (stage == CSM_LEM_STAGE)
	{
	 	ofs1 = OFS_SM;
		vel1 = _V(0,0,-0.0);
		ofs2 = OFS_DOCKING;
		vel2 = _V(0.0,0.0,0.3);

	}
	if (stage == CM_STAGE)
	{
		ofs1 = OFS_CM_CONE;
		vel1 = _V(1.0,1.0,1.0);
	}

	if (stage == CSM_ABORT_STAGE)
	{
		ofs1 = OFS_ABORT_TOWER;
		vel1 = _V(15.0,15.0,50.0);
	}
	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};
	VECTOR3 rofs3, rvel3 = {vs3.rvel.x, vs3.rvel.y, vs3.rvel.z};
	VECTOR3 rofs4, rvel4 = {vs4.rvel.x, vs4.rvel.y, vs4.rvel.z};
	VECTOR3 rofs5, rvel5 = {vs5.rvel.x, vs5.rvel.y, vs5.rvel.z};
	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);
	Local2Rel (ofs3, vs3.rpos);
	Local2Rel (ofs4, vs4.rpos);
	Local2Rel (ofs5, vs5.rpos);
	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);
	GlobalRot (vel3, rofs3);
	GlobalRot (vel4, rofs4);
	GlobalRot (vel5, rofs5);
	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;
	vs3.rvel.x = rvel3.x+rofs3.x;
	vs3.rvel.y = rvel3.y+rofs3.y;
	vs3.rvel.z = rvel3.z+rofs3.z;
	vs4.rvel.x = rvel4.x+rofs4.x;
	vs4.rvel.y = rvel4.y+rofs4.y;
	vs4.rvel.z = rvel4.z+rofs4.z;
	vs5.rvel.x = rvel5.x+rofs5.x;
	vs5.rvel.y = rvel5.y+rofs5.y;
	vs5.rvel.z = rvel5.z+rofs5.z;
//

	if (stage == CM_STAGE)
	{
		if (GetAtmPressure()>35000){
		}
		SetChuteStage1 ();
	}

    if (stage == LAUNCH_STAGE_ONE && !bAbort )
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;
		StageS.play();

		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-STG1");
		hstg1 = oapiCreateVessel(VName,"nsat1stg1", vs1);
		AddStageOneInterstage();
		SetSecondStage1 ();
	}

	if (stage == LAUNCH_STAGE_TWO && !bAbort )
	{

		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		TowerJS.play();
		TowerJS.done();
		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-TWR");

		hesc1 = oapiCreateVessel(VName,"nsat1btower",vs1);
		SetSecondStage2 ();
	}

	if (stage == LAUNCH_STAGE_SIVB)
	{
		AddRCS_S4B();
	}

	if (stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)
	{
		char VName[256]="";
		char VName1[256]="";
		char VName2[256]="";
		char VName3[256]="";
		char VName4[256]="";

		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.1;
		vs2.vrot.y = -0.1;
		vs2.vrot.z = 0.0;
		vs3.vrot.x = 0.1;
		vs3.vrot.y = 0.1;
		vs3.vrot.z = 0.0;
		vs4.vrot.x = -0.1;
		vs4.vrot.y = -0.1;
		vs4.vrot.z = 0.0;
		vs5.vrot.x = -0.1;
		vs5.vrot.y = 0.1;
		vs5.vrot.z = 0.0;
		strcpy (VName, GetName()); strcat (VName, "-S4BSTG");
		hs4bM = oapiCreateVessel(VName, "nsat1astp", vs1);
		if(ASTPMission) {
			SetupStage(hs4bM);
		}
		strcpy (VName1, GetName()); strcat (VName1, "-S4B1");
		hs4b1 = oapiCreateVessel(VName1, "nsat1stg21", vs2);
		strcpy (VName2, GetName()); strcat (VName2, "-S4B2");
		hs4b2 = oapiCreateVessel(VName2, "nsat1stg22", vs3);
		strcpy (VName3, GetName()); strcat (VName3, "-S4B3");
		hs4b3 = oapiCreateVessel(VName3, "nsat1stg23", vs4);
		strcpy (VName4, GetName()); strcat (VName4, "-S4B4");
		hs4b4 = oapiCreateVessel(VName4, "nsat1stg24", vs5);
		SeparationS.play();
		//oapiDeleteVessel(hesc1,vessel->GetHandle());
		ShiftCentreOfMass (_V(0,0,21.5));
		SetCSMStage ();
		if(ASTPMission){
			SetupStage(hs4bM);
			dockstate = 1;
		}
		else{
			dockstate = 4;
		}
	}

	if (stage == CSM_LEM_STAGE)
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		SMJetS.play();
		SMJetS.done();
		if(dockstate !=5){
			VECTOR3 ofs = OFS_DOCKING2;
			VECTOR3 vel = {0.0,0.0,0.1};
			VESSELSTATUS vs4b;
			GetStatus (vs4b);
			StageTransform(this, &vs4b,ofs,vel);
			vs4b.vrot.x = 0.0;
			vs4b.vrot.y = 0.0;
			vs4b.vrot.z = 0.0;
			strcpy (VName, GetName()); strcat (VName, "-DCKPRB");
			hPROBE = oapiCreateVessel(VName, "nsat1probe", vs4b);
		}
		strcpy (VName, GetName()); strcat (VName, "-SM");
		hSMJet = oapiCreateVessel(VName, "nSAT1_SM", vs1);

		SetReentryStage ();
	}

	if (stage == CM_STAGE)
	{
		SetChuteStage2 ();
	}
	if (stage == CM_ENTRY_STAGE_TWO)
	{
		SetChuteStage3 ();
	}
	if (stage == CM_ENTRY_STAGE_FOUR)
	{
		SetChuteStage4 ();
	}
	if (stage == CM_ENTRY_STAGE_FIVE)
	{
		SetSplashStage ();
	}

	if (stage == LAUNCH_STAGE_ONE && bAbort )
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel ("Saturn_Abort", "nSaturn1Abort1", vs1);
		SetAbortStage ();
	}

	if (stage == LAUNCH_STAGE_TWO && bAbort )
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel ("Saturn_Abort", "nSaturn1Abort2", vs1);
		SetAbortStage ();
	}

	if (stage == CSM_ABORT_STAGE)
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		TowerJS.play();
		TowerJS.done();
		strcpy (VName, GetName()); strcat (VName, "-TWR");
		hesc1 = oapiCreateVessel (VName, "sat5btower", vs1);
		SetReentryStage ();
		ActivateNavmode(NAVMODE_KILLROT);
	}
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

	if (GetEngineLevel(ENGINE_MAIN) <0.3 && MissionTime <100 && EDSswitch.GetState() && MissionTime > 10){
		bAbort = true;
	}

	switch (StageState) {

	case 0:

		//
		// Shut down center engine at 8% fuel or if acceleration goes
		// over 3.98g.
		//

		if ((actualFUEL <= 8)) { // || (aHAcc > (3.98*G)))) {
			SetEngineIndicator(5);
			SetThrusterResource(th_main[4],NULL);
			SetThrusterResource(th_main[5],NULL);
			SetThrusterResource(th_main[6],NULL);
			SetThrusterResource(th_main[7],NULL);
			ClearLiftoffLight();
			StageState++;
		}
		break;

	case 1:
		if (actualFUEL <= 7) {
			SShutS.play(NOLOOP,235);
			SShutS.done();
			StageState++;
		}
		break;

	case 2:
		//
		// Begin shutdown countdown at 5% fuel.
		//

		if ((actualFUEL <= 3.7)){
			Sctdw.play(NOLOOP, 245);
			StageState++;
		}
		break;

	case 3:
		if (GetFuelMass() == 0 || bManualSeparate)
		{
			NextMissionEventTime = MissionTime + 0.7;
			SetEngineIndicators();
			StageState++;
		}
		break;

	case 4:
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
		SepS.play(LOOP,255);
		SetThrusterGroupLevel(thg_ver,1.0);
		NextMissionEventTime = MissionTime + 2.0;
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
			NextMissionEventTime = MissionTime + 20;
			StageState++;
		}
		break;

	case 4:
		if (MissionTime > NextMissionEventTime) {
			SeparateStage(stage);
			TowerJS.play();
			TowerJS.done();
			SetStage(LAUNCH_STAGE_SIVB);
		}
		return;
	}

	if(Sswitch5) {
		bManualSeparate =true;
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
			SPSswitch.SetState(true);;
			ABORT_IND = true;
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
		SwindowS.play();
		SwindowS.done();
		StageState++;
		break;

	case 1:
		if (GetEngineLevel(ENGINE_MAIN) <= 0) {
			NextMissionEventTime = MissionTime + 10.0;
			S2ShutS.done();
			SwindowS.done();
			SetStage(STAGE_ORBIT_SIVB);
		}
		break;
	}

	if(Sswitch5) {
		bManualSeparate = true;
	}

	if (StageState > 3)
		SetSIVBThrusters();
	if (bManualSeparate || bAbort)
	{
		bManualSeparate = false;
		SeparateStage(stage);
		SetStage(CSM_LEM_STAGE);
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
		if (bAbort){
			SPSswitch.SetState(true);
			ABORT_IND = true;
			SetThrusterGroupLevel(thg_main, 1.0);
			bAbort = false;
			autopilot= false;
		}
		return;
	}
}

void Saturn1b::Timestep (double simt)
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

	if (hstg1){
		KillAlt(hstg1,60);
	}

	if (hesc1){
		KillAlt(hesc1,90);
	}
	if (hPROBE){
		KillDist(hPROBE);
	}
	if (hs4b1){
		KillDist(hs4b1);
	}
	if (hs4b2){
		KillDist(hs4b2);
	}
	if (hs4b3){
		KillDist(hs4b3);
	}
	if (hs4b4){
		KillDist(hs4b4);
	}

	GenericTimestep(simt);

	if (hAstpDM){
		if (DestroyAstp){
			Undock(0);
			ReadyAstp1 = false;
			oapiDeleteVessel(hAstpDM);
			hAstpDM=NULL;
			DestroyAstp=false;
		}
		if (GetDockStatus(GetDockHandle(0)) == hAstpDM ){
			if(dockstate == 4){
			ReadyAstp1=true;
			bManualUnDock = true;
			}
		}
	}else{
		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-ASTPDM");
		hAstpDM = oapiGetVesselByName(VName);
	}

	if (hs4bM){
		if (GetDockStatus(GetDockHandle(0)) == hs4bM){
			ReadyAstp=true;
			dockstate=2;
				//	sprintf(oapiDebugString() ,"S4B %f");
		}
		else{
			ReadyAstp=false;
				//	sprintf(oapiDebugString() ,"NOT S4B %f");
		}
		if (dockstate>=3 && !S4BASTP){
			SetS4B();
			S4BASTP=true;
		}
	}else{
		char VName[256];

	strcpy (VName, GetName()); strcat (VName, "-S4BSTG");
	hs4bM = oapiGetVesselByName(VName);

	}

	if (hSMJet){
		if ((simt-(20+ignition_SMtime))>=0){
			UllageSM(hSMJet,0,simt);
		}
		else if (!SMSep){
			UllageSM(hSMJet,5,simt);
		}
		KillAlt(hSMJet,35000);
	}

	if (bAbort && stage <= LAUNCH_STAGE_TWO ){
		SetEngineLevel(ENGINE_MAIN, 0);
		SeparateStage (stage);
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

		if (RPswitch15 && RPswitch16){
			if (ASTPMission){
				//sprintf(oapiDebugString() ,"click %f");
			RPswitch16=false;
				if (ReadyAstp||ReadyAstp1||dockstate==3){
				bManualUnDock = true;

				RPswitch15=false;
				}
			}
		}
		if (Sswitch2){
			Undock(0);
		}
		if (Sswitch1){
			if (dockstate==3){
				ProbeJetison=true;
				bManualUnDock = true;
			}
		}
		if (Sswitch3 && Sswitch4){
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
		if(LPswitch1){
			SetThrusterResource(th_att_lin[0],ph_rcs0);
			SetThrusterResource(th_att_lin[1],ph_rcs0);
			SetThrusterResource(th_att_lin[2],ph_rcs0);
			SetThrusterResource(th_att_lin[3],ph_rcs0);
			SetThrusterResource(th_att_rot[0],ph_rcs0);
			SetThrusterResource(th_att_rot[1],ph_rcs0);
			SetThrusterResource(th_att_rot[2],ph_rcs0);
			SetThrusterResource(th_att_rot[3],ph_rcs0);
		}
		else{
			SetThrusterResource(th_att_lin[0],NULL);
			SetThrusterResource(th_att_lin[1],NULL);
			SetThrusterResource(th_att_lin[2],NULL);
			SetThrusterResource(th_att_lin[3],NULL);
			SetThrusterResource(th_att_rot[0],NULL);
			SetThrusterResource(th_att_rot[1],NULL);
			SetThrusterResource(th_att_rot[2],NULL);
			SetThrusterResource(th_att_rot[3],NULL);
		}
		if(LPswitch2){
			SetThrusterResource(th_att_rot[4],ph_rcs0);
			SetThrusterResource(th_att_rot[5],ph_rcs0);
			SetThrusterResource(th_att_rot[7],ph_rcs0);
			SetThrusterResource(th_att_rot[6],ph_rcs0);
			SetThrusterResource(th_att_lin[4],ph_rcs0);
			SetThrusterResource(th_att_lin[5],ph_rcs0);
			SetThrusterResource(th_att_lin[7],ph_rcs0);
			SetThrusterResource(th_att_lin[6],ph_rcs0);
		}else{
			SetThrusterResource(th_att_rot[4],NULL);
			SetThrusterResource(th_att_rot[5],NULL);
			SetThrusterResource(th_att_rot[6],NULL);
			SetThrusterResource(th_att_rot[7],NULL);
			SetThrusterResource(th_att_lin[4],NULL);
			SetThrusterResource(th_att_lin[5],NULL);
			SetThrusterResource(th_att_lin[6],NULL);
			SetThrusterResource(th_att_lin[7],NULL);
		}
		if(LPswitch3){
			for(int i=8;i<24;i++){
				SetThrusterResource(th_att_rot[i],ph_rcs0);
				SetThrusterResource(th_att_lin[i],ph_rcs0);
			}
		}else{
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
		}else if (!LPswitch4 && !RCS_Full){
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
		if (SPSswitch){
			SetThrusterResource(th_main[0],ph_sps);
		}else{
			SetThrusterResource(th_main[0],NULL);
		}
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
		GenericTimestepStage(simt);
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
}


void Saturn1b::SetVehicleStats()

{
}

void Saturn1b::CalculateStageMass()

{
}
