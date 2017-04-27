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

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "ioChannels.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"
#include "saturnv.h"
#include "s1c.h"
#include "tracer.h"

//
// Set the file name for the tracer code.
//

char trace_file[] = "ProjectApollo Saturn5.log";

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

const double BASE_SII_MASS = 42400 + 3490;		// Stage + SII/SIVB interstage

GDIParams g_Param;

//
// Default pitch program.
//
/*
const double default_met[PITCH_TABLE_SIZE]    = { 0, 13.2, 58, 70, 80,  110, 130, 160, 170, 205, 450, 480, 490, 500, 535, 700};   // MET in sec
const double default_cpitch[PITCH_TABLE_SIZE] = {90, 88,   75, 60, 50,   40,  35,  30,  30,  30,  25,  20,  10 ,  5,  -2,   0};	// Commanded pitch in °

const double default_met[PITCH_TABLE_SIZE]    = { 0, 13.2, 58,   70, 80,  110,   130, 160, 170, 205, 450, 480, 490, 500, 535, 700};   // MET in sec
const double default_cpitch[PITCH_TABLE_SIZE] = {90, 88,   81.5, 56, 50,   35.5,  30,  28,  27,  25,  10,   10, 10 ,  5,  -2,   0};	// Commanded pitch in °
*/
const double default_met[PITCH_TABLE_SIZE]    = { 0, 13.2, 58,   70, 80,  110,   130, 160, 170, 205, 450, 480, 490, 500, 535, 700};   // MET in sec
const double default_cpitch[PITCH_TABLE_SIZE] = {90, 88,   81.6, 56, 50,   35.5,  30,  28,  27,  25,  10,   10, 10 ,  5,  -2,   0};	// Commanded pitch in °
// 3 was 80.5
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
	// DS20060301 Save DLL instance handle for later abuse
	dllhandle = g_Param.hDLL;

	//
	// Do the basic initialisation from the
	// generic Saturn class first.
	//

	initSaturn();

	strcpy(StagesString, "S1C:SII:SIVB:CSM");
	SaturnType = SAT_SATURNV;
	HasProbe = true;

	//
	// Default ISP and thrust values.
	//

	ISP_FIRST_SL    = 2601.3; // Average From AP8 Post Flight Eval p114 - Was 2594.4
	ISP_FIRST_VAC   = 2979.4;
	ISP_SECOND_SL   = 300*G;//300*G;
	ISP_SECOND_VAC  = 418*G;//421*G;
	ISP_THIRD_VAC   = 424*G;//421*G;

	//
	// Note: thrust values are _per engine_, not per stage. For now, assume that the second
	// and third stage thrusts are the same.
	//

	THRUST_FIRST_VAC	= 8062309;
	THRUST_SECOND_VAC   = 1023000;
	THRUST_THIRD_VAC    = 1023000;

	//
	// Engines per stage.
	//

	SI_EngineNum = 5;
	SII_EngineNum = 5;
	SIII_EngineNum = 1;

	//
	// State variables.
	//

	TLICapableBooster = true;
	GoHover = false;
	Burned = false;

	//
	// Default masses.
	//

	Interstage_Mass = 3982;

	S4B_EmptyMass = 13680 + 1200; // Stage + SLA
	S4B_FuelMass = 106100;

	SII_EmptyMass = BASE_SII_MASS;
	SII_FuelMass = 441600;
	SII_UllageNum = 8;

	SI_EmptyMass = 148000;			// Stage mass, approx
	SI_FuelMass = 2117000;

	SI_RetroNum = 8;
	SII_RetroNum = 4;

	SM_FuelMass = SPS_DEFAULT_PROPELLANT;
	SM_EmptyMass = 4100;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
												/// \todo The Apollo 15-17 SM was heavier, about 5500 kg
	CM_EmptyMass = 5430;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
	CM_FuelMass =  CM_RCS_FUEL_PER_TANK * 2.;	// The CM has 2 tanks

	CalculateStageMass();

	// load wave (Handle defined in begining of file)
	soundlib.LoadSound(SpeedS, "reentry.wav");
	soundlib.LoadSound(DockS, "docking.wav");

	soundlib.LoadSound(SRover, "LRover.WAV");

	//
	// Pitch program.
	//

	for (int i = 0; i < PITCH_TABLE_SIZE; i++) {
		met[i] = default_met[i];
		cpitch[i] = default_cpitch[i];
	}

	// Moved to instantiation time
	// lvdc_init();
}

SaturnV::~SaturnV()

{
	TRACESETUP("~SaturnV");

	ReleaseSurfaces();
}

void SaturnV::CalculateStageMass ()

{
	if (SaturnType != SAT_INT20)
	{
		SI_Mass = SI_EmptyMass + SI_FuelMass + (SI_RetroNum * 125);
		SII_Mass = SII_EmptyMass + SII_FuelMass + (SII_UllageNum * 175);
		S4B_Mass = S4B_EmptyMass + S4B_FuelMass;
		SM_Mass = SM_EmptyMass + SM_FuelMass;
		CM_Mass = CM_EmptyMass + CM_FuelMass;

		Stage3Mass = S4B_EmptyMass + S4PL_Mass + SM_Mass + CM_Mass;
		Stage2Mass = Stage3Mass + SII_EmptyMass + S4B_FuelMass + Abort_Mass + Interstage_Mass;
		Stage1Mass = Stage2Mass + SI_EmptyMass + SII_FuelMass;
	}
	else
	{
		SI_Mass = SI_EmptyMass + SI_FuelMass + (SI_RetroNum * 125);
		SII_Mass = 0;
		S4B_Mass = S4B_EmptyMass + S4B_FuelMass;
		SM_Mass = SM_EmptyMass + SM_FuelMass;
		CM_Mass = CM_EmptyMass + CM_FuelMass;

		Stage3Mass = S4B_EmptyMass + S4PL_Mass + SM_Mass + CM_Mass;
		Stage2Mass = 0;
		Stage1Mass = Stage3Mass + SI_EmptyMass + S4B_FuelMass + Abort_Mass;
	}
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

//
// Adjust the mixture ratio of the engines on the SII stage. This occured late in
// the flight to ensure that the fuel was fully burnt before the stage was dropped.
//

void SaturnV::SetSIICMixtureRatio (double ratio)

{
	double isp, thrust;

	// Hardcoded ISP and thrust according to the Apollo 11 Saturn V flight evaluation report.
	// http://klabs.org/history/history_docs/jsc_t/apollo_11_saturn_v.pdf

	//if (ratio > 5.4 && ratio < 5.6) {	// 5.5
	//	thrust = 1012506;
	//	isp = 4152;
	//
	//} else if (ratio > 4.2 && ratio < 4.4) {	// 4.3
	//	thrust = 783617.4;
	//	isp = 4223.7;
	//
	//} else {
		isp = GetJ2ISP(ratio);
		thrust = THRUST_SECOND_VAC * ThrustAdjust;
	//}

	//
	// For simplicity assume no ISP change at sea-level: SII stage should always
	// be in near-vacuum anyway.
	//

	for (int i = 0; i < 5; i++) {
		SetThrusterIsp (th_main[i], isp, isp);
		SetThrusterMax0 (th_main[i], thrust);
	}

	//
	// Give the AGC our new stats.
	//

	iu.SetVesselStats(isp, thrust);

	MixtureRatio = ratio;
}

//
// And SIVb.
//

void SaturnV::SetSIVbCMixtureRatio (double ratio)

{
	double isp, thrust;

	// Hardcoded ISP and thrust according to the Apollo 11 Saturn V flight evaluation report.
	// http://klabs.org/history/history_docs/jsc_t/apollo_11_saturn_v.pdf

	//if (ratio > 4.8 && ratio < 5.0) {			// 4.9
	//	thrust = 901557.;
	//	isp = 4202.;
	//
	//} else if (ratio > 4.4 && ratio < 4.6) {	// 4.5
	//	thrust = 799000.;
	//	isp = 4245.;
	//
	//} else {
		isp = GetJ2ISP(ratio);
		thrust = THRUST_THIRD_VAC * ThrustAdjust;
	//}

	//
	// For simplicity assume no ISP change at sea-level: SIVb stage should always
	// be in near-vacuum anyway.
	//

	SetThrusterIsp (th_main[0], isp, isp);
	SetThrusterMax0 (th_main[0], thrust);

	//
	// Give the AGC our new stats.
	//

	iu.SetVesselStats(isp, thrust);

	MixtureRatio = ratio;
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
	TRACESETUP("SaturnV::clbkSetClassCaps");
	//
	// For some reason things get screwy if we do all the initialisation in the constructor. I think
	// that Orbiter may be keeping a pool of objects and reusing them, so we have to reinitialise here
	// to ensure that it works correctly.
	//

	initSaturnV();

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

void SaturnV::StageOne(double simt, double simdt)

{
	VESSELSTATUS vs;
	GetStatus(vs);

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	double amt = (MainLevel) * 0.4;
	JostleViewpoint(amt);

	if (vs.status == 1) {
		MoveEVA();
	}

	if (GetEngineLevel(ENGINE_MAIN) < 0.3 && MissionTime < 100 && EDSSwitch.GetState() && MissionTime > 10) {
		bAbort = true;
	}

	// Control contrail
	if (MissionTime > 12)
		contrailLevel = 0;
	else if (MissionTime > 7)
		contrailLevel = (12.0 - MissionTime) / 100.0;
	else if (MissionTime > 2)
		contrailLevel = 1.38 - 0.95 / 5.0 * MissionTime;
	else
		contrailLevel = 1;
	//sprintf(oapiDebugString(), "contrailLevel %f", contrailLevel);


	switch (StageState) {
	case 0:
		//
		// Shut down center engine at 6% fuel or if acceleration goes
		// over 3.98g, or at planned shutdown time.
		//

		if ((actualFUEL <= 6) || (MissionTime >= FirstStageCentreShutdownTime)) 
		{
			if (SI_EngineNum < 5)
			{
				//
				// Shut down engines 2 and 4.
				//
				// Currently our engine thruster numbers don't match NASA engine numbers!
				//
				SetEngineIndicator(2);
				SetThrusterResource(th_main[1], NULL);
				SetEngineIndicator(4);
				SetThrusterResource(th_main[0], NULL);
			}
			else
			{
				//
				// Shut down center engine.
				//
				SetEngineIndicator(5);
				SetThrusterResource(th_main[4], NULL);
			}

			//
			// Clear liftoff light now - Apollo 15 checklist
			//
			ClearLiftoffLight();

			NextMissionEventTime = MissionTime + 1.0;
			StageState++;
		}
		break;

	case 1:
		if (MissionTime >= NextMissionEventTime) {
			SShutS.play(NOLOOP,235);
			SShutS.done();
			StageState++;
		}
		break;

	case 2:
		//
		// Begin shutdown countdown at 5% fuel.
		//

		if ((actualFUEL <= 5) || (MissionTime >= (FirstStageShutdownTime - 10.0))) {
			// Move hidden SIC vessel
			if (hstg1) {
				VESSELSTATUS vs;
				GetStatus(vs);
				S1C *stage1 = (S1C *) oapiGetVesselInterface(hstg1);
				stage1->DefSetState(&vs);
			}				
			// Play countdown
			Sctdw.play(NOLOOP, 245);
			StageState++;
		}
		break;

	case 3:
		if ((GetFuelMass() <= 0.001) || (MissionTime >= FirstStageShutdownTime))
		{
			SetEngineIndicators();
			NextMissionEventTime = MissionTime + 0.7;
			StageState++;
		}
		break;

	case 4:
		if (MissionTime >= NextMissionEventTime) {
			ClearEngineIndicators();
			SeparationS.play(NOLOOP, 245);

			if (SaturnType != SAT_INT20)
			{
				SeparateStage (LAUNCH_STAGE_TWO);
				SetStage(LAUNCH_STAGE_TWO);
			}
			else {
				SeparateStage (LAUNCH_STAGE_SIVB);
				SetStage(LAUNCH_STAGE_SIVB);
			}
		}
		else {

			//
			// Engine thrust decay.
			//

			for (int i = 0; i < 4; i++) {
				double Level = GetThrusterLevel(th_main[i]);
				Level -= (simdt * 1.2);
				Level = max(0, Level);
				SetThrusterLevel(th_main[i], Level);
			}
		}
		break;
	}

	if (AutopilotActive()) {
		AutoPilot(MissionTime);
	} else {
		AttitudeLaunch1();
	}
}

//
// Handle SII stage up to Interstage seperation.
//

void SaturnV::StageTwo(double simt)

{
	double MainLevel = GetEngineLevel(ENGINE_MAIN);

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
		ActivateStagingVent();

		NextMissionEventTime = MissionTime + 1.4;
		StageState++;
		break;

	case 1:
		if (MissionTime >= NextMissionEventTime) {
			SetEngineIndicators();
			SIISepState = true;
			SetSIICMixtureRatio(5.5);
			DeactivateStagingVent();

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
				if (GetThrusterLevel(th_main[i]) > 0.65) {
					ENGIND[i] = false;
				}
				else {
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
					SetThrusterGroupLevel(thg_ull, 0.0);

				SetThrusterGroupLevel(thg_main, 1.0);
				SepS.stop();
				NextMissionEventTime += 25.9;

				//
				// Override if required.
				//

				if (InterstageSepTime < NextMissionEventTime) {
					NextMissionEventTime = InterstageSepTime;
				}
				StageState++;
			}
		}
		break;

	case 4:

		//
		// Interstage jettisoned 30.5 seconds after SIC shutdown.
		//

		if (MissionTime >= NextMissionEventTime) { 
			SeparateStage (LAUNCH_STAGE_TWO_ISTG_JET);
			SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
			SIISepState = false;
		}
		break;
	}

	/// \todo Manual separation and abort handling, see StageFour
}

void SaturnV::StageFour(double simt, double simdt)

{
	LAUNCHIND[2] = true;

	int i;
	double Level;

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	double amt = (MainLevel) * 0.2;
	JostleViewpoint(amt);

	switch (StageState) {

	case 0:
		if (VehicleNo < 505) {
			StageState = 2;
		}
		else {
			StageState = 1;
		}
		break;

	case 1:

		//
		// Shut down center engine on Apollo 10 and later.
		//

		if ((actualFUEL < 15) || (MissionTime >= SecondStageCentreShutdownTime)) {
			SetThrusterResource(th_main[4],NULL);
			S2ShutS.play(NOLOOP,235);
			S2ShutS.done();
			SetEngineIndicator(5);
			StageState++;
		}
		break;

	case 2:

		//
		// Change mixture ratio to ensure full fuel burn.
		//

		if ((actualFUEL < 5) || (MissionTime >= SecondStagePUShiftTime)) {
			SetSIICMixtureRatio (4.3);
			if (Crewed) {
				SPUShiftS.play();
			}
			SPUShiftS.done();
			StageState++;
		}
		break;

	case 3:
		//
		// Begin shutdown countdown at 1.7% fuel.
		//

		if ((actualFUEL < 1.7) || (MissionTime >= (SecondStageShutdownTime - 10.0))) {
			Sctdw.play(NOLOOP,245);
			StageState++;
		}
		break;

	case 4:
		if ((GetFuelMass() <= 0) || (MissionTime >= SecondStageShutdownTime)) {
			SetEngineIndicators();
			NextMissionEventTime = MissionTime + 2.0;
			StageState++;
		}
		break;

	case 5:
		if (MissionTime >= NextMissionEventTime) {
			S2ShutS.done();
			SPUShiftS.done();
			ClearEngineIndicators();
			NextMissionEventTime = MissionTime;
			StageState++;

			if (!LaunchFail.SIIAutoSepFail) {
				SeparateStage(LAUNCH_STAGE_SIVB);
				SetStage(LAUNCH_STAGE_SIVB);
			}
		}
		else {

			//
			// Engine thrust decay.
			//

			for (i = 0; i < 4; i++) {
				Level = GetThrusterLevel(th_main[i]);
				Level -= (simdt * 1.2);
				Level = max(0, Level);
				SetThrusterLevel(th_main[i], Level);
			}
		}
		break;

	case 6:

		//
		// Engine thrust decay.			
		//

		for (i = 0; i < 4; i++) {
			Level = GetThrusterLevel(th_main[i]);
			Level -= (simdt * 1.2);
			Level = max(0, Level);
			SetThrusterLevel(th_main[i], Level);
		}

		if (Level <= 0) {
			StageState++;
		}
		break;
	}

    if (AutopilotActive()){
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunch2();
	}
	
	// Manual separation
	if (SIISIVBSepSwitch.GetState()) { 		
		SeparateStage(LAUNCH_STAGE_SIVB);
		SetStage(LAUNCH_STAGE_SIVB);
		ClearEngineIndicators();
		NextMissionEventTime = MissionTime;
	}

	// Abort handling
	if (bAbort && !LESAttached) {
		SeparateStage(LAUNCH_STAGE_SIVB);
		SetStage(LAUNCH_STAGE_SIVB);
		StartAbort();
		// Disable autopilot
		autopilot = false;
		bAbort = false;
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
		// Always enable SIVB RCS for now, once we hit orbit.
		//

		SetSIVBThrusters(true);
		break;
	}

	//
	// Get the handles for any odds and ends that are out there.
	//

	char VName[256];
	char ApolloName[64];

	GetApolloName(ApolloName);

	strcpy (VName, ApolloName); strcat (VName, "-TWR");
	hesc1= oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-STG1");
	hstg1= oapiGetVesselByName(VName);

	LookForSIVb();

	strcpy (VName, ApolloName); strcat (VName, "-S4B1");
	hs4b1 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-S4B2");
	hs4b2 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-S4B3");
	hs4b3 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-S4B4");
	hs4b4 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-SM");
	hSMJet = oapiGetVesselByName(VName);

	LookForLEM();

	strcpy (VName, ApolloName); strcat (VName, "-DCKPRB");
	hPROBE = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-EVA");
	hEVA = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-INTSTG");
	hintstg = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-STG2");
	hstg2 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-APEX");
	hApex = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-DROGUECHUTE");
	hDrogueChute = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-MAINCHUTE");
	hMainChute = oapiGetVesselByName(VName);	
	strcpy (VName, ApolloName); strcat (VName, "-OPTICSCOVER");
	hOpticsCover = oapiGetVesselByName(VName);	
	
	habort = oapiGetVesselByName("Saturn_Abort");
}

// Orbiter calls here via callback prior to every timestep.
// This function must call GenericTimestep() to operate the CSM.

void SaturnV::Timestep(double simt, double simdt, double mjd)

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

	GenericTimestep(simt, simdt, mjd);

	if (stage < CSM_LEM_STAGE) {

		// LVDC++
		if (use_lvdc) {
			if (stage < CSM_LEM_STAGE) {
				if (lvdc != NULL) {
					lvdc->TimeStep(simt, simdt);
				}
			}
			else {
				if (lvdc != NULL) {
					// At this point we are done with the LVDC, we can delete it.
					// This saves memory and declutters the scenario file.
					delete lvdc;
					lvdc = NULL;
					use_lvdc = false;
				}
			}
		} else {

			// AFTER LVDC++ WORKS SATISFACTORILY EVERYTHING IN THIS BLOCK
			// SHOULD BE SAFE TO DELETE. DO NOT ADD ANY LVDC++ CODE IN THIS BLOCK.

			if (bAbort && MissionTime > -300 && LESAttached) {
				SetEngineLevel(ENGINE_MAIN, 0);
				SeparateStage(CM_STAGE);
				SetStage(CM_STAGE);
				StartAbort();
				agc.SetInputChannelBit(030, SIVBSeperateAbort, true); // Notify the AGC of the abort
				agc.SetInputChannelBit(030, LiftOff, true); // and the liftoff, if it's not set already
				bAbort = false;
				return;
			}

			//
			// Do stage-specific processing.
			//

			if (hintstg) {
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

					dist2 *= (2.5 / simdt);

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
			}

			switch (stage) {

			case LAUNCH_STAGE_ONE:
				StageOne(simt, simdt);
				break;

			case LAUNCH_STAGE_TWO:
				StageTwo(simt);
				break;

			case LAUNCH_STAGE_TWO_ISTG_JET:
				StageFour(simt, simdt);
				break;

			case LAUNCH_STAGE_SIVB:
				StageLaunchSIVB(simt);
				break;

			case STAGE_ORBIT_SIVB:
				// We get here at around T5+9.8 or so.

				//
				// J-2 mixture ratio
				//

				if (GetPropellantMass(ph_3rd) / GetPropellantMaxMass(ph_3rd) > 0.51)
					SetSIVbCMixtureRatio(4.5);
				else
					/// \todo PU-Shift during burn disabled until the IU GNC (i.e. IMFD) can handle that
					// SetSIVbCMixtureRatio(4.9);
					SetSIVbCMixtureRatio(4.5);
			
				StageOrbitSIVB(simt, simdt);
				break;

			default:
				GenericTimestepStage(simt, simdt);
				break;
			}
		}
	} else {
		GenericTimestepStage(simt, simdt);
	}
	LastTimestep = simt;
}

void SaturnV::clbkPostStep (double simt, double simdt, double mjd) {

	Saturn::clbkPostStep(simt, simdt, mjd);
}

void SaturnV::SetVehicleStats(){
	//
	// Adjust performance as appropriate based on the vehicle number. Early Saturn V rockets
	// had less thrust than later models, and later models removed some retro and ullage
	// rockets to save weight.
	//

	SI_RetroNum = 8;
	SII_RetroNum = 8;

	if (VehicleNo > 500 && VehicleNo < 503) {
		if (!S1_ThrustLoaded)
			THRUST_FIRST_VAC = 7835000; // 7653000; /// \todo Temporary fix, otherwise the autopilot is not working properly, we should replace this when we figured out all parameters (masses, fuel masses etc.) of these vehicles
		if (!S2_ThrustLoaded)
			THRUST_SECOND_VAC = 1001000;
		if (!S3_ThrustLoaded)
			THRUST_THIRD_VAC = 1001000;

		SII_UllageNum = 8;
	}
	else if (VehicleNo >= 503 && VehicleNo < 510) {
		if (VehicleNo == 503){
			// Apollo 8 -- For LVDC++ experiments.
			if (!S1_ThrustLoaded){
				// F1 thrust computed as follows:
				// 6782000 @ Sea Level, From AP8 LV Evaluation, averaged predicted value
				// Was making 8000000 @ S1C OECO				
				THRUST_FIRST_VAC = 8000100; 
				THRUST_SECOND_VAC = 1017000;//1001000;
				THRUST_THIRD_VAC = 1024009;//1001000;//901557;
				// Masses from Apollo By The Numbers for AP8
				SI_EmptyMass = 139641.0; // Minus retro weight, that gets added seperately
				SI_FuelMass = 2038222.0;
				Interstage_Mass = 5641;
				SII_EmptyMass = 49744.0; // Includes S2/S4B interstage, does not include ullage jets; changed for testing
				SII_FuelMass = 430936.0; 
				S4B_EmptyMass = 16489.0;//22981.0; // Includes S4B stage, IU, LM adapter, but NOT the LTA
				S4B_FuelMass = 107318.0;
			}
		}else{
			if (!S1_ThrustLoaded)
				THRUST_FIRST_VAC = 7835000;
		}

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

	if (!SII_MassLoaded) {
		if (VehicleNo >= 508) {
			SII_EmptyMass = BASE_SII_MASS - 1500;
		}
		else {
			SII_EmptyMass = BASE_SII_MASS;
		}
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

	//
	// Stage masses.
	//

	oapiWriteScenario_float (scn, "SIEMPTYMASS", SI_EmptyMass);
	oapiWriteScenario_float (scn, "SIIEMPTYMASS", SII_EmptyMass);
	oapiWriteScenario_float (scn, "S4EMPTYMASS", S4B_EmptyMass);
}

void SaturnV::SaveLVDC(FILEHANDLE scn){
	if (use_lvdc && lvdc != NULL){ lvdc->SaveState(scn); }
}

void SaturnV::LoadLVDC(FILEHANDLE scn){
	if (use_lvdc){
		// If the LVDC does not yet exist, create it.
		if(lvdc == NULL){
			lvdc = new LVDC;
			lvdc->Init(this);
		}
		lvdc->LoadState(scn);
	}
}

void SaturnV::clbkLoadStateEx (FILEHANDLE scn, void *status)

{
	TRACESETUP("SaturnV::clbkLoadStateEx");

	GetScenarioState(scn, status);

	ClearMeshes();
	SetupMeshes();

	// DS20150720 LVDC++ ON WHEELS
	// If GetScenarioState has set the use_lvdc flag but not created the LVDC++, we need to do that here.
	// This happens if the USE_LVDC flag is set but no LVDC section is present.
	if(use_lvdc && lvdc == NULL){
		lvdc = new LVDC;
		lvdc->Init(this);
	}

	//
	// This code all needs to be fixed up.
	//

	switch (stage) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		if (buildstatus < 6){
			ChangeSatVBuildState(buildstatus);
		}
		else{
			SetFirstStage();
			SetFirstStageEngines();
		}
		break;

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SetSecondStage();
		SetSecondStageEngines(-STG1O);
		break;

	case LAUNCH_STAGE_SIVB:
		SetThirdStage();
		SetThirdStageEngines(-STG2O);
		if (StageState >= 4) {
			AddRCS_S4B();
		}
		break;

	case STAGE_ORBIT_SIVB:
		SetThirdStage();
		SetThirdStageEngines(-STG2O);
		AddRCS_S4B();
		//
		// Always enable SIVB RCS for now, once we hit orbit.
		//

		SetSIVBThrusters(true);
		break;

	default:
		SetGenericStageState();
		break;
	}

	//
	// Setup of the generic systems
	//

	GenericLoadStateSetup();

	if (stage < LAUNCH_STAGE_SIVB) {
		if (Crewed) {
			soundlib.LoadMissionSound(SPUShiftS, PUSHIFT_SOUND, PUSHIFT_SOUND);
		}
	}
}

void SaturnV::ConfigureStageMeshes(int stage_state)

{
	ClearMeshes();

	//
	// This code all needs to be fixed up.
	//

	switch (stage_state) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		if (buildstatus < 6){
			ChangeSatVBuildState(buildstatus);
		}
		else {
			SetFirstStage();
		}
		break;

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SetSecondStage();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetThirdStage();
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

void SaturnV::ConfigureStageEngines(int stage_state)
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

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SetSecondStageEngines (-STG1O);
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetThirdStageEngines(-STG2O);
		break;
	}
}

void SaturnV::StageLaunchSIVB(double simt)

{
	double peDist;
	OBJHANDLE ref;
	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	double amt = (MainLevel) * 0.1;
	JostleViewpoint(amt);

    if (AutopilotActive()) {
		AutoPilot(MissionTime);
	}
	else {
		AttitudeLaunchSIVB();
	}

	switch (StageState) {

	case 0:
		SetSIVbCMixtureRatio(4.9);
		SetThrusterResource(th_main[0], ph_3rd);
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
			SetSIVBThrusters(true);
			ClearEngineIndicator(1);
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
			// This event makes this T5+0.04 or so

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
		ref = GetGravityRef();
		GetPeDist(peDist);
		if (peDist - oapiGetSize(ref) < agc.GetDesiredPerigee() * 1000. && MissionTime < NextMissionEventTime) {
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
		ref = GetGravityRef();
		GetPeDist(peDist);
		if (peDist - oapiGetSize(ref) >= agc.GetDesiredPerigee() * 1000. || MissionTime >= NextMissionEventTime) {
			//
			// Switch to TLI mode. Disable the ullage rockets
			// and switch stages.
			//

			SepS.stop();
			SetThrusterGroupLevel(thg_aps, 0);
			SIVBCutoffTime = MissionTime;

			NextMissionEventTime = MissionTime + 10.0;
			SetStage(STAGE_ORBIT_SIVB);
		}
		return;
	}

	// Abort handling
	if (CSMLVPyros.Blown() || (bAbort && !LESAttached))
	{
		SepS.stop();
		SeparateStage(CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
		if (bAbort) {			
			/// \todo SPS abort handling
			StartAbort();
			bAbort = false;
			autopilot = false;
		}
	}

	/* sprintf(oapiDebugString(), "SIVB thrust %.1f isp %.2f propellant %.1f", 
		GetThrusterLevel(th_main[0]) * GetThrusterMax(th_main[0]), GetThrusterIsp(th_main[0]), GetPropellantMass(ph_3rd));
	*/
}

int SaturnV::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	// Assembly tests
	/* 
	if (!KEYMOD_SHIFT(kstate) && !KEYMOD_CONTROL(kstate) && !KEYMOD_ALT(kstate)) {

		if (stage == ROLLOUT_STAGE) {
			if (key == OAPI_KEY_B && down == true) {
				LaunchVehicleBuild();
				return 1;
			}
			if (key == OAPI_KEY_U && down == true) {
				LaunchVehicleUnbuild();
				return 1;
			}
		}
	}
	*/
	return Saturn::clbkConsumeBufferedKey(key, down, kstate);
}

void SaturnV::LaunchVehicleRolloutEnd() {
	// called by crawler after arrival on launch pad

	SetFirstStage();
	SetFirstStageEngines();

	SetStage(ONPAD_STAGE);
}

void SaturnV::LaunchVehicleBuild() {
	// called by crawler
	
	if (stage == ROLLOUT_STAGE && buildstatus < 5) {
		buildstatus++;
		ChangeSatVBuildState(buildstatus);
	}
}

void SaturnV::LaunchVehicleUnbuild() {
	// called by crawler

	if (stage == ROLLOUT_STAGE && buildstatus > 0) {
		buildstatus--;
		ChangeSatVBuildState(buildstatus);
	}
}

// DS20150720 "SWITCH SELECTOR" STAGING SUPPORT FUNCTION
void SaturnV::SwitchSelector(int item){
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
	case 15:
		SetLiftoffLight();										// And light liftoff lamp
		SetStage(LAUNCH_STAGE_ONE);								// Switch to stage one
		// Start mission and event timers
		secs.LiftoffA();
		secs.LiftoffB();
		agc.SetInputChannelBit(030, LiftOff, true);					// Inform AGC of liftoff
		SetThrusterGroupLevel(thg_main, 1.0);					// Set full thrust, just in case
		contrailLevel = 1.0;
		if (LaunchS.isValid() && !LaunchS.isPlaying()){			// And play launch sound			
			LaunchS.play(NOLOOP,255);
			LaunchS.done();
		}
		break;
	case 16:
		SetThrusterResource(th_main[4], NULL); // Should stop the engine
		SShutS.play(NOLOOP,235);
		SShutS.done();
		// Clear liftoff light now - Apollo 15 checklist item
		ClearLiftoffLight();
		break;
	case 17:
		// Move hidden S1C
		if (hstg1) {
			VESSELSTATUS vs;
			GetStatus(vs);
			S1C *stage1 = (S1C *) oapiGetVesselInterface(hstg1);
			stage1->DefSetState(&vs);
		}				
		// Engine Shutdown
		for (i = 0; i < 5; i++){
			SetThrusterResource(th_main[i], NULL);
		}
		break;
	case 18:
		// Drop old stage
		SeparateStage(LAUNCH_STAGE_TWO);
		SetStage(LAUNCH_STAGE_TWO);
		// Fire S2 ullage
		if(SII_UllageNum){
			SetThrusterGroupLevel(thg_ull, 1.0);
			SepS.play(LOOP, 130);
		}
		ActivateStagingVent();
		break;
	case 19:
		// S2 Engine Startup
		SIISepState = true;
		SetSIICMixtureRatio(5.5);
		DeactivateStagingVent();
		break;
	case 20:
		// S2 Engine Startup P2
		SetThrusterGroupLevel(thg_main, 1); // Full power
		if(SII_UllageNum){ SetThrusterGroupLevel(thg_ull,0.0); }
		SepS.stop();
		break;
	case 21:
		SeparateStage (LAUNCH_STAGE_TWO_ISTG_JET);
		SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
		break;
	case 22:
		JettisonLET();
		break;
	case 23:
		// MR Shift
		SetSIICMixtureRatio(4.5); // Is this 4.7 or 4.2? AP8 says 4.5
		SPUShiftS.play(NOLOOP,255); 
		SPUShiftS.done();
		break;
	case 24:
		// SII IECO
		SetThrusterResource(th_main[4], NULL);
		S2ShutS.play(NOLOOP, 235);
		S2ShutS.done();
		break;
	case 5:
		// S4B Startup
		SetSIVbCMixtureRatio(4.946);
		break;
	case 6:
		// S4B restart
		SetSIVbCMixtureRatio(4.5);
		break;
	case 7:
		// S4B MRS
		SetSIVbCMixtureRatio(4.946);
		break;
	}
}
