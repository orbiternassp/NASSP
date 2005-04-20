/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5

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
  *	Revision 1.5  2005/03/09 00:26:50  chode99
  *	Added code to support SII retros.
  *	
  *	Revision 1.4  2005/02/20 05:24:58  chode99
  *	Changes to implement realistic CM aerodynamics. Created callback function "CoeffFunc" in Saturn1b.cpp and Saturn5.cpp. Substituted CreateAirfoil for older lift functions.
  *	
  *	Revision 1.3  2005/02/19 00:03:28  movieman523
  *	Reduced volume of APS sound playback, and changed course correction logic to use times from header file, not hard-coded in C++ code.
  *	
  *	Revision 1.2  2005/02/18 00:41:54  movieman523
  *	Wired in new Apollo 13 sounds and set Scorrec so you can use time acceleration again after course correction!
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include <stdio.h>
#include <math.h>
#include "Orbitersdk.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"

#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"

//
// Set the file name for the tracer code.
//

char trace_file[] = "NASP-trace.txt";

//
// Various bits of real global data for the DLL. Most things should be vessel
// specific, but these aren't: most are set once at DLL creation and then never
// need to change.
//

const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;
const double  CSM_THRUST_ATT   = 200.34*KGF;
const double  CSM_ISP          = 773*SEC;

static int refcount = 0;

//
// The BODGE_FACTOR is subtracted from the empty mass of the SII
// stage to increase the amount of available fuel when the SIVB
// reaches orbit. This is totally unrealistic, but makes the mission
// easier to fly!
//

#define BODGE_FACTOR	10000 // 0

const double BASE_SII_MASS = 42400 + 3490 - BODGE_FACTOR;		// Stage + SII/SIVB interstage

GDIParams g_Param;

//
// SaturnV constructor, derived from basic Saturn class.
//

SaturnV::SaturnV (OBJHANDLE hObj, int fmodel)
: Saturn (hObj, fmodel)

{
	TRACESETUP("SaturnV");

	hMaster = hObj;
	initSaturnV();
}

//
// Do all the initialisation.
//

void SaturnV::initSaturnV()

{
	//
	// Do the basic initialisation from the
	// generic Saturn class first.
	//

	initSaturn();

	//
	// Default ISP and thrust values.
	//

	ISP_FIRST_SL    = 265*G;
	ISP_FIRST_VAC   = 304*G;
	ISP_SECOND_SL   = 300*G;//300*G;
	ISP_SECOND_VAC  = 418*G;//421*G;
	ISP_THIRD_VAC  = 424*G;//421*G;

	//
	// Note: thrust values are _per engine_, not per stage. For now, assume that the second
	// and third stage thrusts are the same.
	//

	THRUST_FIRST_VAC	= 8062309;
	THRUST_SECOND_VAC  = 1023000;
	THRUST_THIRD_VAC = 1023000;

	hstage1 = 0;
	hstage2 = 0;

	hLMV = 0;

	TLICapableBooster = true;

	stgSM = false;
	GoHover=false;

	S4Shoot=false;

	Scorrec=false;
	Resetjet=false;

	S4Sep=false;
	velDISP=false;

	Burned=false;

	S4Bset = false;

	ApolloExploded =false;
	CryoStir = false;
	KranzPlayed = false;

	EVA_IP=false;
	gaz=0;

	//
	// Some of these need to be zeroed, but I'm not sure which ones. So for
	// safety, do all of them.
	//

	ignition_S4time = 0;

	//
	// State variables.
	//

	TLIBurnDone = false;

	//
	// Default masses.
	//

	Interstage_Mass = 1905;

	S4B_EmptyMass = 13680 + 1200; // Stage + SLA
	S4B_FuelMass = 106100;

	SII_EmptyMass = BASE_SII_MASS;
	SII_FuelMass = 441600;
	SII_UllageNum = 8;

	SI_EmptyMass = 148000;			// Stage + wasted fuel, approx
	SI_FuelMass = 2117000;
	SI_RetroNum = 8;

	CalculateStageMass();

	// load wave (Handle defined in begining of file)
	soundlib.LoadSound(SpeedS, "reentry.wav");
	soundlib.LoadSound(DockS, "docking.wav");
	soundlib.LoadSound(SCorrection, "Correction.wav");

	soundlib.LoadSound(SRover, "LRover.WAV");

	LongestTimestep = 0;
	LongestTimestepLength = 0.0;
	CurrentTimestep = 0;
}

SaturnV::~SaturnV()

{
	TRACESETUP("~SaturnV");

	ReleaseSurfaces();
}

void SaturnV::CalculateStageMass ()

{
	SI_Mass = SI_EmptyMass + SI_FuelMass + (SI_RetroNum * 250);
	SII_Mass = SII_EmptyMass + SII_FuelMass + (SII_UllageNum * 175);
	S4B_Mass = S4B_EmptyMass + S4B_FuelMass;
	SM_Mass = SM_EmptyMass + SM_FuelMass;
	CM_Mass = CM_EmptyMass + CM_FuelMass;

	Stage3Mass = S4B_EmptyMass + LEM_Mass + SM_Mass + CM_Mass;
	Stage2Mass = Stage3Mass + SII_EmptyMass + S4B_FuelMass + Abort_Mass + Interstage_Mass;
	Stage1Mass = Stage2Mass + SI_EmptyMass + SII_FuelMass;
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

void SetS4B(OBJHANDLE hS4B)
{
	VESSEL *targetvessel;
	VESSELSTATUS S4;
	TRACESETUP("SetS4B");

	targetvessel = oapiGetVesselInterface(hS4B);
	targetvessel->GetStatus(S4);
	S4.vrot = _V(0.0,0.0,0.0);
	targetvessel->DefSetState(&S4);
}

//
// Adjust the mixture ratio of the engines on the SII stage. This occured late in
// the flight to ensure that the fuel was fully burnt before the stage was dropped.
//

void SaturnV::SetSIICMixtureRatio (double ratio)

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

	//
	// For simplicity assume no ISP change at sea-level: SII stage should always
	// be in near-vacuum anyway.
	//

	for (int i = 0; i < 5; i++) {
		SetThrusterIsp (th_main[i], isp, ISP_SECOND_SL);
		SetThrusterMax0 (th_main[i], THRUST_SECOND_VAC * ThrustAdjust);
	}
}

void SaturnV::AccelS4B(OBJHANDLE hvessel, double time)

{
	TRACESETUP("AccelS4B");

	VESSEL *stg1vessel = oapiGetVesselInterface(hvessel);

	if (ignition_S4time == 0 && !S4Sep){
		ignition_S4time=time;
		S4Sep = true;
		VECTOR3 m_exhaust_pos1= {0,0,-STG2O-4};
		VECTOR3 m_exhaust_ref = {0,0,-1};
		stg1vessel->AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos1, 15.0, 1.5, &m_exhaust_ref);
		stg1vessel->SetAttitudeLinLevel(2,-1);
	}

	if (time > ignition_S4time+5){
		stg1vessel->SetAttitudeLinLevel(2,0);
		stg1vessel->SetAttitudeLinLevel(0,-1);
	}

	if (time > ignition_S4time+10){
		stg1vessel->SetAttitudeLinLevel(0,0);
	}

	if (time > ignition_S4time+180){
		stg1vessel->ActivateNavmode(NAVMODE_KILLROT);
		stg1vessel->DeactivateNavmode(NAVMODE_RETROGRADE);
		stg1vessel->SetEngineLevel(ENGINE_MAIN,1);
		S4Shoot = true;
	}

	if (time > ignition_S4time + 90){
		stg1vessel->ActivateNavmode(NAVMODE_RETROGRADE);
	}
}

void SaturnV::MoveEVA()

{
	TRACESETUP("MoveEVA");
	double cm ;
	double lat;
	double lon;
	double cap;
	double timeW=1;
	VESSELSTATUS eva;
	GetStatus(eva);
	oapiGetHeading(GetHandle(),&cap);
	timeW=oapiGetTimeAcceleration();
	lon=eva.vdata[0].x;
	lat=eva.vdata[0].y;

//	SRover->play(LOOP, 245);

	cm = 5.36e-8*timeW;

	if (KEY1){
		eva.vdata[0].z = eva.vdata[0].z - ((PI*0.1/180)*timeW/2);
		if(eva.vdata[0].z <=-2*PI){
			eva.vdata[0].z = eva.vdata[0].z + 2*PI;
		}
		KEY1=false;
	}
	else if (KEY3){
		eva.vdata[0].z = eva.vdata[0].z +(( PI*0.1/180)*timeW/2);
		if(eva.vdata[0].z >=2*PI){
			eva.vdata[0].z = eva.vdata[0].z - 2*PI;
		}
		KEY3=false;
	}
	else if (KEY2){
		lat = lat - cos(cap) * cm;
		lon = lon - sin(cap) * cm;
		KEY2=false;
	}
	else if (KEY4){
		lat = lat + sin(cap) * cm;
		lon = lon - cos(cap) * cm;
		KEY4=false;
	}
	else if (KEY5){
		KEY5=false;
	}
	else if (KEY6){
		lat = lat - sin(cap) * cm;
		lon = lon + cos(cap) * cm;
		KEY6=false;
	}
	else if (KEY7){
		KEY7=false;
	}
	else if (KEY8||GoHover){// we go ahead whatever our headign
		lat = lat + cos(cap) * cm;
		lon = lon + sin(cap) * cm;
		KEY8=false;
	}
	else if (KEY9){
		KEY9=false;
	}else{
		SRover.stop();
	}

	eva.vdata[0].x=lon;
	eva.vdata[0].y=lat;
	DefSetState(&eva);
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	TRACESETUP("ovcInit");

	VESSEL *v;

	if (!refcount++) {
		TRACE("refcount == 0");
		LoadSat5Meshes();
		SaturnInitMeshes();
	}

	TRACE("Meshes loaded");

	BaseInit();

	TRACE("Got colors");

	v = new SaturnV (hvessel, flightmodel);
	return v;
}

DLLCLBK void ovcExit (VESSEL *vessel)

{
	TRACESETUP("ovcExit");

	--refcount;

	if (!refcount) {
		TRACE("refcount == 0");

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) delete (SaturnV *)vessel;
}

void SaturnV::clbkSetClassCaps (FILEHANDLE cfg)

{
	//
	// For some reason things get screwy if we do all the initialisation in the constructor. I think
	// that Orbiter may be keeping a pool of objects and reusing them, so we have to reinitialise here
	// to ensure that it works correctly.
	//

	initSaturnV();
}

void SaturnV::StageOne(double simt)

{
	VESSELSTATUS vs;
	GetStatus(vs);

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	if (MainLevel > 0.65 ) {
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else {
		LAUNCHIND[7] = false;
		ENGIND[5] = false;
	}

	double amt = (MainLevel) * 0.4;
	JostleViewpoint(amt);

	if (vs.status == 1 ){
		MoveEVA();
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

		if ((actualFUEL <= 8)) {
			//
			// Set center engine light.
			//
			SetEngineIndicator(5);

			//
			// Clear liftoff light now - Apollo 15 checklist
			//
			ClearLiftoffLight();
			SetThrusterResource(th_main[4], NULL);
			StageState++;
		}
		break;

	case 1:
		if (actualFUEL <= 7) {
			if (SShutS.isValid()) {
				SShutS.play(NOLOOP,235);
				SShutS.done();
			}
			StageState++;
		}
		break;

	case 2:
		//
		// Begin shutdown countdown at 5% fuel.
		//

		if ((actualFUEL <= 5)){
			Sctdw.play(NOLOOP, 245);
			StageState++;
		}
		break;

	case 3:
		if (GetFuelMass() == 0 && buildstatus > 7)
		{
			NextMissionEventTime = MissionTime + 0.7;
			StageState++;
		}
		break;

	case 4:
		if (MissionTime >= NextMissionEventTime) {
			SetEngineLevel(ENGINE_MAIN, 0.0);
			SetEngineIndicators();
			if (SShutS.isValid()) {
				SShutS.done();
			}
			SeparateStage (stage);
			bManualSeparate = false;
			SeparationS.play(NOLOOP, 245);
			SetStage(LAUNCH_STAGE_TWO);
		}
		break;
	}

	if (AutopilotActive()) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunch1();
	}
}

//
// Handle SII stage up to Interstage seperation.
//

void SaturnV::StageTwo(double simt)

{
	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	if (MainLevel > 0.65 ) {
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else{
		LAUNCHIND[7] = false;
		ENGIND[5] = false;
	}

	double amt = (MainLevel) * 0.25;
	JostleViewpoint(amt);

    if (AutopilotActive()) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunch2();
	}

	switch (StageState) {

	case 0:
		if (SII_UllageNum) {
			SetThrusterGroupLevel(thg_ull, 1.0);
			SepS.play(LOOP, 130);
		}

		if (hstg1) {
			Retro1(hstg1, 5);
		}

		NextMissionEventTime = MissionTime + 1.4;
		SIISepState = true;
		StageState++;
		break;

	case 1:
		if (MissionTime >= NextMissionEventTime) {
			LastMissionEventTime = MissionTime;
			NextMissionEventTime += 3.0;
			StageState++;
		}
		break;

	case 2:
		{
			//
			// Build up thrust after seperation.
			//

			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterGroupLevel(thg_main, (0.3*deltat));

			if (MissionTime >= NextMissionEventTime) {
				LastMissionEventTime = NextMissionEventTime;
				NextMissionEventTime += 0.2;
				SetThrusterGroupLevel(thg_main, 0.9);
				StageState++;
			}

			for (int i = 0; i<5; i++){
				if (GetThrusterLevel(th_main[i]) > 0.65){
					ENGIND[i] = false;
				}
				else{
					ENGIND[i] = true;
				}
			}
		}
		break;

	case 3:
		{
			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterGroupLevel(thg_main, 0.9 + (0.5*deltat));

			if (MissionTime >= NextMissionEventTime) {

				//
				// Now at full thrust, and shut down ullage rockets.
				//

				if (SII_UllageNum)
					SetThrusterGroupLevel(thg_ull,0.0);

				SetThrusterGroupLevel(thg_main, 1.0);

				SepS.stop();
				LAUNCHIND[6] = false;

				NextMissionEventTime += 25.9;
				StageState++;
			}
		}
		break;

	case 4:

		//
		// Interstage jettisoned 30.5 seconds after SIC shutdown.
		//

		if (MissionTime >= NextMissionEventTime || bManualSeparate) {
			SeparateStage (stage);
			SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
			bManualSeparate = false;
			SIISepState = false;
			NextMissionEventTime += 5.7;
		}
		break;
	}
}

void SaturnV::StageThree(double simt)

{
	LAUNCHIND[6]=false;

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	if (MainLevel > 0.65 ){
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else{
		LAUNCHIND[7] = false;
		ENGIND[5] = false;
	}

	double amt = (MainLevel) * 0.2;
	JostleViewpoint(amt);

    if (AutopilotActive()) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunch2();
	}

	//
	// Tower jettison at 36.2 seconds after SIC shutdown.
	//

	if (MissionTime >= NextMissionEventTime || bManualSeparate || GetFuelMass() == 0 || TJ1switch && TJ2switch)
	{
		SeparateStage (stage);
		SetStage(LAUNCH_STAGE_TWO_TWR_JET);
		bManualSeparate=false;
	}
}

void SaturnV::StageFour(double simt)

{
	LAUNCHIND[2] = true;

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	if (MainLevel > 0.65 ) {
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}
	else{
		LAUNCHIND[7] = false;
		ENGIND[5] = false;
	}

	double amt = (MainLevel) * 0.2;
	JostleViewpoint(amt);

	switch (StageState) {

	case 0:
		if (VehicleNo < 505) {
			StageState = 3;
		}
		else if (ApolloNo == 13) {
			StageState = 1;
		}
		else {
			StageState = 2;
		}

		SwindowS.play();
		SwindowS.done();
		break;

	case 1:

		//
		// Apollo 13 engine shut down early at 5:30 into the mission.
		//

		if (MissionTime >= 330.0) {
			SetThrusterResource(th_main[4],NULL);
			S2ShutS.play(NOLOOP, 235);
			S2ShutS.done();
			SetEngineIndicator(5);
			StageState = 3;
		}
		break;

	case 2:

		//
		// Shut down center engine at 20% fuel.on Apollo 10 and later.
		//

		if ((actualFUEL < 20)) {
			SetThrusterResource(th_main[4],NULL);
			S2ShutS.play(NOLOOP,235);
			S2ShutS.done();
			SetEngineIndicator(5);
			StageState++;
		}
		break;

	case 3:

		//
		// Change mixture ratio to ensure full fuel burn. This is
		// arbitrarily set here to happen at 15% fuel.
		//

		if (actualFUEL < 15) {
			SetSIICMixtureRatio (4.5);
			if (Crewed) {
				SPUShiftS.play();
				SPUShiftS.done();
			}
			StageState++;
		}
		break;

	case 4:
		//
		// Begin shutdown countdown at 1.7% fuel.
		//

		if (actualFUEL < 1.7) {
			Sctdw.play(NOLOOP,245);
			StageState++;
		}
		break;

	case 5:
		if (GetFuelMass() <= 0) {
			SetEngineIndicators();
			NextMissionEventTime = MissionTime + 2.0;
			StageState++;
		}
		break;

	case 6:
		if (MissionTime >= NextMissionEventTime) {
			S2ShutS.done();
			SPUShiftS.done();
			ClearEngineIndicators();
			NextMissionEventTime = MissionTime;
			SeparateStage (stage);
			SetStage(LAUNCH_STAGE_SIVB);
		}
		break;
	}

    if (AutopilotActive()){
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunch2();
	}

	if (bManualSeparate) {
		bManualSeparate = false;
		SeparateStage (stage);
		SetEngineIndicators();
		NextMissionEventTime = MissionTime;
		SetStage(LAUNCH_STAGE_SIVB);
	}

	if (LVSswitch) {
		bManualSeparate = false;
		SeparateStage (stage);
		NextMissionEventTime = MissionTime;
		SetStage(LAUNCH_STAGE_SIVB);
	}

	if (bAbort) {
		SeparateStage (stage);
		SetEngineIndicators();
		SetStage(LAUNCH_STAGE_SIVB);
		ABORT_IND = true;
		bAbort = true;
	}
}

void SaturnV::StageSix(double simt)

{
	if (GetNavmodeState(NAVMODE_KILLROT)&& !LPswitch1 && !LPswitch2){
		if (GetThrusterLevel(th_att_rot[10]) <0.00001 && GetThrusterLevel(th_att_rot[18]) <0.00001 ){
			DeactivateNavmode(NAVMODE_KILLROT);
		}
	}

	if(simt>0.5)
		AttitudeLaunch4();

	if(RPswitch13 && !HatchOpen){
		bToggleHatch = true;
	}
	else if(!RPswitch13 && HatchOpen){
		bToggleHatch = true;
	}

	if(RPswitch17){
		if (ActivateLEM && hLMV){
			ActivateLEM=false;
			RPswitch17=false;
			VESSEL *lmvessel;
			lmvessel=oapiGetVesselInterface(hLMV);
			lmvessel->SetEnableFocus(true);
		}
	}

	if (RPswitch14 && HatchOpen){
		ToggleEva = true;
		RPswitch14 = false;
	}

	if(!probeOn){
		if(GetDockStatus(GetDockHandle(0))!=NULL){
			Undock(0);
			CrashBumpS.play(NOLOOP,150);
		}
	}

	//
	// Should we be turning off these lights here?
	//

	for (int i=0 ;i<6;i++){
		LAUNCHIND[i]=false;
	}

	if (hLMV && dockstate <5){
		if (GetDockStatus(GetDockHandle(0))==hLMV){
			ActivateLEM=true;
			VESSEL *lmvessel;
			dockstate=3;
			lmvessel=oapiGetVesselInterface(hLMV);
			lmvessel->SetEnableFocus(true);
		}
		else{
			VESSEL *lmvessel;
			lmvessel=oapiGetVesselInterface(hLMV);
			lmvessel->SetEnableFocus(true);
			ActivateLEM=false;
			dockstate=4;
		}
	}
	//sprintf(oapiDebugString(), "dockstate= %d",dockstate);
	if (hs4bM && hLMV){
		if (GetDockStatus(GetDockHandle(0))==hLMV){
			ActivateLEM=true;
		}
		if (GetDockStatus(GetDockHandle(0))==hs4bM){
			ActivateS4B=true;
		}
		else{
			ActivateS4B=false;
		}
	}

	if (Sswitch2){
		Undock(0);
	}

	if (Sswitch1){
		if (ActivateLEM){
			ProbeJetison=true;
			bManualUnDock = true;
		}
	}

	if (Sswitch3 && Sswitch4){
		bManualSeparate=true;
	}

	if (RPswitch16){
		bManualUnDock = true;
	}

	if (ApolloNo == 13) {

		//
		// Play cryo-stir audio.
		//

		if (!CryoStir && MissionTime >= (APOLLO_13_EXPLOSION_TIME - 30))
		{
			double TimeW = oapiGetTimeAcceleration ();
			if (TimeW > 1){
				oapiSetTimeAcceleration (1);
			}

			SApollo13.play(NOLOOP, 255);
			CryoStir = true;
		}

		//
		// Play explosion audio.
		//

		if (CryoStir && !ApolloExploded && MissionTime >= APOLLO_13_EXPLOSION_TIME) {
			double TimeW = oapiGetTimeAcceleration ();
			if (TimeW > 1){
				oapiSetTimeAcceleration (1);
			}

			if (SApollo13.isValid()) {
				SApollo13.done();
			}

			SExploded.play(NOLOOP,255);
			SExploded.done();

			MasterAlarm();

			//
			// AGC restarted as the explosion occured.
			//

			agc.ForceRestart();

			ApolloExploded = true;

			SetPropellantMass(ph_rcs0,0);
			SetPropellantMass(ph_sps,0);

			VESSELSTATUS vs13;
			GetStatus(vs13);

			vs13.vrot = _V(-1,-1,-1);
			DefSetState(&vs13);
		}

		//
		// Play Kranz comments in the background.
		//

		if (!KranzPlayed && (MissionTime >= APOLLO_13_EXPLOSION_TIME + 30)) {

			if (SExploded.isValid()) {
				SExploded.stop();
				SExploded.done();
			}

			SKranz.play(NOLOOP, 150);
			SKranz.done();

			KranzPlayed = true;
		}
	}

	//
	// Check for course correction time and shut down time acceleration if appropriate.
	//

	if (!Scorrec && MissionTime >= COURSE_CORRECTION_START_TIME && MissionTime < COURSE_CORRECTION_END_TIME){
		double TimeW = oapiGetTimeAcceleration ();
		if (TimeW > 1.0){
			oapiSetTimeAcceleration (1.0);
		}
		SCorrection.play(NOLOOP,255);
		SCorrection.done();
		Scorrec = true;
	}

	if (EVA_IP){
		if(!hEVA){
			ToggleEVA();
		}
	}

	if ((simt-(2+release_time))>=0 && Resetjet) {
		SetAttitudeLinLevel(_V(0.0,0.0,0.0));
		Resetjet =false;
		release_time=0;
	}

	if (ToggleEva){
		ToggleEVA();
	}

	if (bToggleHatch){
		ToggelHatch();
		bToggleHatch=false;
	}

	//
	// Handle automation of unmanned launches.
	//

	if (!Crewed) {
		switch (StageState)
		{
		case 0:
			if (CSMBurn) {
				NextMissionEventTime = CSMBurnStart - 200.0;
				StageState++;
			}
			break;

		case 1:
			if (MissionTime >= NextMissionEventTime) {
				ActivateCSMRCS();
				StageState++;
			}
			break;

		case 2:
			SlowIfDesired();
			ActivateNavmode(NAVMODE_PROGRADE);
			NextMissionEventTime = CSMBurnStart;
			StageState++;
			break;

		case 3:
			if (MissionTime >= NextMissionEventTime) {
				ActivateSPS();
				SetThrusterGroupLevel(thg_main, 1.0);
				NextMissionEventTime = MissionTime + 0.25;
				StageState++;
			}
			break;

		case 4:
			if (MissionTime >= NextMissionEventTime) {
				OBJHANDLE hPlanet = GetGravityRef();
				double prad = oapiGetSize(hPlanet);
				double ap;
				GetApDist(ap);

				ActivateNavmode(NAVMODE_PROGRADE);
				SetThrusterGroupLevel(thg_main, 1.0);

				NextMissionEventTime = MissionTime + 0.25;

				//
				// Burn until the orbit is about right or we're out of fuel.
				//

				if ((ap >= (prad + (CSMApogee * 1000.0))) || (actualFUEL <= 0.1)) {
					StageState++;
					SetThrusterGroupLevel(thg_main, 0.0);
					DeactivateNavmode(NAVMODE_PROGRADE);
					DeactivateCSMRCS();
					DeactivateSPS();
					NextMissionEventTime = MissionTime + CalculateApogeeTime() - 800;
					StageState++;
				}
			}
			break;

		//
		// Get a more accurate apogee time.
		//

		case 5:
			if (MissionTime >= NextMissionEventTime) {
				NextMissionEventTime = MissionTime + CalculateApogeeTime() - 400;
				StageState++;
			}
			break;

		case 6:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				ActivateCSMRCS();
				ActivateSPS();
				ActivateNavmode(NAVMODE_RETROGRADE);
				NextMissionEventTime = MissionTime + CalculateApogeeTime() - 15;
				StageState++;
			}
			break;

		case 7:
			if (MissionTime >= NextMissionEventTime) {
				OBJHANDLE hPlanet = GetGravityRef();
				double prad = oapiGetSize(hPlanet);
				double pe;
				GetPeDist(pe);

				SetThrusterGroupLevel(thg_main, 1.0);
				ActivateNavmode(NAVMODE_RETROGRADE);

				NextMissionEventTime = MissionTime + 0.25;

				if ((pe <= (prad + (CSMPerigee * 1000.0))) || (actualFUEL <= 0.1)) {
					StageState++;
					SetThrusterGroupLevel(thg_main, 0.0);
					ActivateNavmode(NAVMODE_PROGRADE);
					DeactivateSPS();
					CSMBurn = false;
					NextMissionEventTime = MissionTime + 200.0;
					StageState++;
				}
			}
			break;

		case 8:
			if (MissionTime >= NextMissionEventTime) {
				DeactivateNavmode(NAVMODE_PROGRADE);
				ActivateNavmode(NAVMODE_KILLROT);
				NextMissionEventTime = MissionTime + 100.0;
				StageState++;
			}
			break;

		case 9:
			if (MissionTime >= NextMissionEventTime) {
				DeactivateNavmode(NAVMODE_PROGRADE);
				DeactivateNavmode(NAVMODE_KILLROT);
				DeactivateCSMRCS();
				StageState++;
			}
			break;
		}

		if (CMSepSet && (MissionTime >= CMSepTime)) {
			SlowIfDesired();
			bManualSeparate = true;
		}
	}

	if (bManualUnDock)
	{
		if (GetDockStatus(GetDockHandle(0))== hs4bM)//this check is for docking status if doked we cannot jetison ASTP
		{
			dockstate = 2;
			release_time = simt;
			Resetjet =true;
			DockStage (dockstate);
			bManualUnDock=false;
		}
		else if(GetDockStatus(GetDockHandle(0))==hLMV && dockstate ==3 && ProbeJetison){
			DockStage (dockstate);
			Undock(0);
			bManualUnDock=false;
		}
		else{
			bManualUnDock=false;
		}
	}

	if (dockstate == 3 && !S4Shoot) {
		AccelS4B(hs4bM,simt);
	}

	if (LPswitch1 && !RH11switch && !RH12switch && !RH13switch && !RH14switch && !RH21switch && !RH22switch && !RH23switch && !RH24switch && PP1switch && PP2switch && PP3switch && PP4switch && SP1switch && SP2switch && SP3switch && SP4switch){
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

	if(LPswitch2 && !RH11switch && !RH12switch && !RH13switch && !RH14switch && !RH21switch && !RH22switch && !RH23switch && !RH24switch && PP1switch && PP2switch && PP3switch && PP4switch && SP1switch && SP2switch && SP3switch && SP4switch){
		SetThrusterResource(th_att_rot[4],ph_rcs0);
		SetThrusterResource(th_att_rot[5],ph_rcs0);
		SetThrusterResource(th_att_rot[7],ph_rcs0);
		SetThrusterResource(th_att_rot[6],ph_rcs0);
		SetThrusterResource(th_att_lin[4],ph_rcs0);
		SetThrusterResource(th_att_lin[5],ph_rcs0);
		SetThrusterResource(th_att_lin[7],ph_rcs0);
		SetThrusterResource(th_att_lin[6],ph_rcs0);
	}
	else{
		SetThrusterResource(th_att_rot[4],NULL);
		SetThrusterResource(th_att_rot[5],NULL);
		SetThrusterResource(th_att_rot[6],NULL);
		SetThrusterResource(th_att_rot[7],NULL);
		SetThrusterResource(th_att_lin[4],NULL);
		SetThrusterResource(th_att_lin[5],NULL);
		SetThrusterResource(th_att_lin[6],NULL);
		SetThrusterResource(th_att_lin[7],NULL);
	}

	if(LPswitch3 && !RH11switch && !RH12switch && !RH13switch && !RH14switch && !RH21switch && !RH22switch && !RH23switch && !RH24switch && PP1switch && PP2switch && PP3switch && PP4switch && SP1switch && SP2switch && SP3switch && SP4switch){
		for(int i=8;i<24;i++){
			SetThrusterResource(th_att_rot[i],ph_rcs0);
			SetThrusterResource(th_att_lin[i],ph_rcs0);
		}
	}
	else{
		for(int i=8;i<24;i++){
			SetThrusterResource(th_att_rot[i],NULL);
			SetThrusterResource(th_att_lin[i],NULL);
		}
	}

	if (LPswitch4 && RCS_Full){
		for(int i=0;i<24;i++){
			DelThruster(th_att_rot[i]);
			DelThruster(th_att_lin[i]);
		}
		DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
		DelThrusterGroup(THGROUP_ATT_FORWARD,true);
		DelThrusterGroup(THGROUP_ATT_BACK,true);
		DelThrusterGroup(THGROUP_ATT_PITCHDOWN,true);
		DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
		DelThrusterGroup(THGROUP_ATT_YAWRIGHT,true);
		DelThrusterGroup(THGROUP_ATT_YAWLEFT,true);
		AddRCSJets(-1.80,995);
		RCS_Full=false;
	}
	else if (!LPswitch4 && !RCS_Full){
		for(int i=0;i<24;i++){
			DelThruster(th_att_rot[i]);
			DelThruster(th_att_lin[i]);
		}
		DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
		DelThrusterGroup(THGROUP_ATT_FORWARD,true);
		DelThrusterGroup(THGROUP_ATT_BACK,true);
		DelThrusterGroup(THGROUP_ATT_PITCHDOWN,true);
		DelThrusterGroup(THGROUP_ATT_PITCHUP,true);
		DelThrusterGroup(THGROUP_ATT_YAWRIGHT,true);
		DelThrusterGroup(THGROUP_ATT_YAWLEFT,true);
		AddRCSJets(-1.80,1990);
		RCS_Full=true;
	}

	//
	// Enable or disable SPS.
	//

	if (SPSswitch){
		SetThrusterResource(th_main[0],ph_sps);
	}
	else{
		SetThrusterResource(th_main[0],NULL);
	}

	if (bManualSeparate)
	{
		if (dockstate <= 1 || dockstate >= 3) {
			SeparateStage (stage);
			ignition_SMtime = simt;
			bManualSeparate = false;
			setupSM(hSMJet);
			SetStage(CM_STAGE);
		}
		else {
			bManualSeparate=false;
		}
	}
}

void SaturnV::DoFirstTimestep(double simt)

{
	//
	// Do housekeeping on the first timestep after
	// initialisation. This allows us to pick up any
	// data that isn't saved and must be recreated after
	// loading... but which can only be done when the
	// whole system has been initialised.
	//

	switch (stage) {

	case STAGE_ORBIT_SIVB:
		//
		// In TLI mode, disable the engines if we're waiting for
		// the user to start the TLI burn or if it's been done.
		//
		if (Realism && (StageState == 0 || StageState == 8)) {
			SetThrusterResource(th_main[0], 0);
		}
		break;
	}

	//
	// Get the handles for any odds and ends that are out there.
	//

	char VName[256];

	strcpy (VName, GetName()); strcat (VName, "-TWR");
	hesc1= oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-STG1");
	hstg1= oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4BSTG");
	hs4bM= oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B1");
	hs4b1 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B2");
	hs4b2 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B3");
	hs4b3 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-S4B4");
	hs4b4 = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-SM");
	hSMJet = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-LM");
	hLMV = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-DCKPRB");
	hPROBE = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-EVA");
	hEVA = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-INTSTG");
	hintstg = oapiGetVesselByName(VName);
	strcpy (VName, GetName()); strcat (VName, "-STG2");
	hstg2 = oapiGetVesselByName(VName);

	habort = oapiGetVesselByName("Saturn_Abort");
}

void SaturnV::Timestep(double simt)

{
	//
	// On the first timestep we just do basic setup
	// stuff and return. We seem to get called in at
	// least some cases before Orbiter is properly set
	// up, so the last thing we want to do is point the
	// engines in a wacky direction and then not be
	// called again for several seconds.
	//

	if (FirstTimestep) {
		DoFirstTimestep(simt);
		LastTimestep = simt;
		FirstTimestep = false;
		return;
	}

#if 1 // def TRACK_TIMESTEPS
	CurrentTimestep++;
	if (oapiGetSimStep() > LongestTimestep) {
		LongestTimestep = CurrentTimestep;
		LongestTimestepLength = oapiGetSimStep();
	}
#endif // TRACK_TIMESTEPS

	GenericTimestep(simt);

	if (hSMJet && !ApolloExploded){
		if ((simt-(20+ignition_SMtime))>=0 && stgSM){
			UllageSM(hSMJet,0,simt);
			stgSM = false;
		}
		else if (!SMSep){
			UllageSM(hSMJet,5,simt);
		}
		KillAlt(hSMJet,350000);
	}

	if (hs4bM){
		if (!S4Bset){
			setupS4B(hs4bM);
			S4Bset = true;
		}
		if (dockstate > 2){
			double TimeW1;
			TimeW1 = oapiGetTimeAcceleration ();
			if (TimeW1 > 10){
				SetS4B(hs4bM);
			}
		}
	}

	if (bAbort && stage < LAUNCH_STAGE_TWO_TWR_JET){
		SetEngineLevel(ENGINE_MAIN,0);
		SeparateStage (stage);
		SetStage(CSM_ABORT_STAGE);
		bAbort=false;
		return;
	}

	//
	// Do stage-specific processing.
	//
	if (hstg1){
		KillAlt(hstg1,60);
	}

	if (hstg2){
		KillAlt(hstg2,60);
	}

	if (hintstg){
		//
		// Really we want to model the effect of the engine force on the
		// interstage, so it spins as it moves away. Currently we just throw
		// on a random rotation.
		//

		VECTOR3 posr;
		oapiGetRelativePos (GetHandle(), hintstg, &posr);

		double dist2 = (posr.x * posr.x) + (posr.y * posr.y) + (posr.z * posr.z);

		if (dist2 > 25 && dist2 < 5000) {
			VECTOR3 f;

			//
			// Scale distance appropriately for timestep time.
			//

			dist2 *= (2.5 / dTime);

			f.x = (double)(rand() & 1023 - 512) / dist2;
			f.y = (double)(rand() & 1023 - 512) / dist2;

			VESSEL *vistg = oapiGetVesselInterface (hintstg);

			VESSELSTATUS vsistg;
			vistg->GetStatus(vsistg);

			//
			// And add random amounts to rotation.
			//

			vsistg.vrot.x += f.x;
			vsistg.vrot.y += f.y;
			vistg->DefSetState(&vsistg);
		}

		KillAlt(hintstg,60);
	}

	if (hesc1) {
		KillAlt(hesc1,90);
	}

	if (hPROBE){
		KillDist(hPROBE);
	}

	if (hs4b1) {
		KillDist(hs4b1);
	}
	if (hs4b2) {
		KillDist(hs4b2);
	}
	if (hs4b3) {
		KillDist(hs4b3);
	}

	if (hs4b4) {
		KillDist(hs4b4);
	}
	switch (stage) {

	case LAUNCH_STAGE_ONE:
		StageOne(simt);
		break;

	case LAUNCH_STAGE_TWO:
		StageTwo(simt);
		break;

	case LAUNCH_STAGE_TWO_ISTG_JET:
		StageThree(simt);
		break;

	case LAUNCH_STAGE_TWO_TWR_JET:
		StageFour(simt);
		break;

	case LAUNCH_STAGE_SIVB:
		StageLaunchSIVB(simt);
		break;

	case STAGE_ORBIT_SIVB:
		StageOrbitSIVB(simt);
		break;

	case CSM_LEM_STAGE:
		StageSix(simt);
		break;

	case CSM_ABORT_STAGE:
		SetEngineLevel(ENGINE_MAIN,1);
		SetRCS_CM();

		ClearAutopilotLight();

		//sprintf(oapiDebugString(), "Mode Abort 1B%f", abortTimer);

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
				pitch=GetPitch();
				if (pitch >0) {
					SetPitchMomentScale (+5e-3);
					SetBankMomentScale (+5e-3);
					SetLiftCoeffFunc (0);
				}
			}

			if (simt-(20+abortTimer)>=0)
			{
				bManualSeparate = true;
				LAUNCHIND[2] = true;
				abortTimer = 0;
			}

			if (bManualSeparate || GetAltitude() < 500)
			{
				SeparateStage (stage);
				SetStage(CM_STAGE);
				bManualSeparate=false;
				abortTimer = 0;
			}
		}
		break;

	default:
		GenericTimestepStage(simt);
		break;
	}

	LastTimestep = simt;
}

void SaturnV::SetVehicleStats()

{
	//
	// Adjust performance as appropriate based on the vehicle number. Early Saturn V rockets
	// had less thrust than later models, and later models removed some retro and ullage
	// rockets to save weight.
	//

	SI_RetroNum = 8;
	SII_EmptyMass = BASE_SII_MASS;

	if (VehicleNo > 500 && VehicleNo < 503) {
		THRUST_FIRST_VAC = 7653000;
		THRUST_SECOND_VAC = 1001000;
		THRUST_THIRD_VAC = 1001000;
		SII_UllageNum = 8;
	}
	else if (VehicleNo >= 503 && VehicleNo < 510) {
		THRUST_FIRST_VAC = 7807000;
		SII_UllageNum = 4;
	}
	else {
		SII_UllageNum = 0;
		SI_RetroNum = 4;
	}

	//
	// Apollo 13 and later had lightweight SII stages, about 1500kg lighter
	// than earlier missions.
	//

	if (VehicleNo >= 508) {
		SII_EmptyMass = BASE_SII_MASS - 1500;
	}

	CalculateStageMass ();

	//
	// Calculate Apollo mission number.
	//

	if (!ApolloNo && (VehicleNo >= 503 && VehicleNo <= 512)) {
		ApolloNo = VehicleNo - 495;
	}
}

void SaturnV::SaveVehicleStats(FILEHANDLE scn)

{
	//
	// Fuel mass on launch.
	//

	oapiWriteScenario_float (scn, "SIFUELMASS", SI_FuelMass);
	oapiWriteScenario_float (scn, "SIIFUELMASS", SII_FuelMass);
	oapiWriteScenario_float (scn, "S4FUELMASS", S4B_FuelMass);
}

void SaturnV::clbkLoadStateEx (FILEHANDLE scn, void *status)

{
	GetScenarioState(scn, status);

	//
	// This code all needs to be fixed up.
	//

	switch (stage) {

	case ROLLOUT_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		if (buildstatus < 8){
			BuildFirstStage(buildstatus);
			double TCP=-101.5+STG0O+12;//-TCPO;
			SetTouchdownPoints (_V(0,-1.0,TCP), _V(-.7,.7,TCP), _V(.7,.7,TCP));
		}
		else{
			SetFirstStage();
			ShiftCentreOfMass (_V(0,0,STG0O));
		}

		if (GetEngineLevel(ENGINE_MAIN)>=0.5){
			SetMaxThrust (ENGINE_ATTITUDE, 8e5);
		}

		break;

	case LAUNCH_STAGE_TWO:
		SetSecondStage();
		break;

	case LAUNCH_STAGE_TWO_ISTG_JET:
		SetSecondStage1();
		break;

	case LAUNCH_STAGE_TWO_TWR_JET:
		SetSecondStage2();
		break;

	case LAUNCH_STAGE_SIVB:
		SetThirdStage();
		if (StageState >= 4) {
			AddRCS_S4B();
		}
		break;

	case STAGE_ORBIT_SIVB:
		SetThirdStage();
		AddRCS_S4B();
		break;

	case CSM_LEM_STAGE:
		SetCSMStage();
		break;

	default:
		SetGenericStageState();
		break;
	}

	//
	// To be perfectly honest, I'm not 100% sure what
	// dockstate does anymore.
	//

	switch (dockstate) {
		case 1:
				break;
		case 2:

				break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			SetCSM2Stage();
			break;
			if (EVA_IP){
				SetupEVA();
			}
			break;
	}

	GenericLoadStateSetup();

	if (stage < LAUNCH_STAGE_SIVB) {
		if (Crewed) {
			soundlib.LoadMissionSound(SPUShiftS, PUSHIFT_SOUND, PUSHIFT_SOUND);
		}
	}

	if (bStartS4B && !TLIBurnDone) {
		soundlib.LoadMissionSound(SecoSound, SECO_SOUND, SECO_SOUND);
	}
}

bool SaturnV::SIVBStart()

{
	if (bStartS4B || TLIBurnDone)
		return false;

	if (stage != STAGE_ORBIT_SIVB)
		return false;

	bStartS4B = true;

	if (Realism)
		SetThrusterResource(th_main[0], ph_3rd);

	soundlib.LoadMissionSound(SecoSound, SECO_SOUND, SECO_SOUND);

	return true;
}

void SaturnV::SIVBStop()

{
	if (stage != STAGE_ORBIT_SIVB)
		return;

	SetEngineLevel(ENGINE_MAIN,0);

	if (Realism)
		SetThrusterResource(th_main[0], 0);

	SecoSound.play();
	SecoSound.done();

	TLIBurnDone = true;
}

void SaturnV::StageLaunchSIVB(double simt)

{
	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	if (MainLevel > 0.65 ){
		LAUNCHIND[7] = true;
		ENGIND[5] = true;
	}else{
		ENGIND[5] = false;
		LAUNCHIND[7] = false;
	}

	double amt = (MainLevel) * 0.1;
	JostleViewpoint(amt);

    if (AutopilotActive()) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunchSIVB();
	}

	//
	// Update the ullage indicator as appropriate.
	//

	LAUNCHIND[6] = false;
	if (StageState < 5) {
		if (GetThrusterGroupLevel(thg_ver) > 0) {
			LAUNCHIND[6] = true;
		}
	}
	else {
		if (GetThrusterGroupLevel(thg_aps) > 0) {
			LAUNCHIND[6] = true;
		}
	}

	switch (StageState) {

	case 0:
		SetThrusterResource(th_main[0], ph_3rd);
		if (hstg2)
			Retro2(hstg2,5);
		SepS.play(LOOP, 130);
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
		if (GetThrusterLevel(th_main[0]) > 0.65)
			ClearEngineIndicator(1);
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
			ClearEngineIndicator(1);
			LAUNCHIND[6] = false;
			LastMissionEventTime = NextMissionEventTime;
			NextMissionEventTime += 2.1;
			StageState++;
		}

	case 4:
		if (MainLevel <= 0) {
			//
			// When the engine shuts down, the ullage rockets
			// fire to settle the fuel.
			//

			if (Realism)
				SetThrusterResource(th_main[0], NULL);

			SetEngineIndicator(1);
			SetThrusterGroupLevel(thg_aps, 1.0);
			SepS.play(LOOP, 130);
			NextMissionEventTime = MissionTime + 10.0;
			StageState++;
		}
		break;

		//
		// If we save and reload while the ullage thrusters are running, then the
		// sound won't start up again when the reload the scenario. So check here
		// and do that.
		//

	case 5:
		if (MissionTime < NextMissionEventTime) {
			if (!SepS.isPlaying()) {
				SepS.play(LOOP,255);
				SetThrusterGroupLevel(thg_aps, 1.0);
			}
		}
		else {
			NextMissionEventTime = MissionTime + 40.0;
			ClearEngineIndicator(1);
			StageState++;
		}
		break;

	case 6:
		if (MissionTime < NextMissionEventTime) {
			if (!SepS.isPlaying()) {
				SepS.play(LOOP, 130);
				SetThrusterGroupLevel(thg_aps, 1.0);
			}
		}
		else {
			StageState++;
		}
		break;

	case 7:
		if (MissionTime >= NextMissionEventTime) {
			//
			// Switch to TLI mode. Disable the ullage rockets
			// and switch stages.
			//

			SepS.stop();
			SetThrusterGroupLevel(thg_aps, 0);
			NextMissionEventTime = MissionTime + 10.0;
			SetStage(STAGE_ORBIT_SIVB);
			bAbtlocked =true;
		}
		return;
	}

	if(Sswitch5){
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

