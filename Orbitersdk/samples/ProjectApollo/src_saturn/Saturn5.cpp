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
#include "csmcomputer.h"
#include "ioChannels.h"

#include "saturn.h"
#include "saturnv.h"
#include "s1c.h"
#include "LVDC.h"
#include "tracer.h"

//
// Random functions from Yaagc.
//

extern "C" {
	void srandom(unsigned int x);
	long int random();
}

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
	iu = new IUSV;
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
}

SaturnV::~SaturnV()

{
	TRACESETUP("~SaturnV");

	ReleaseSurfaces();

	delete iu;
}

void SaturnV::CalculateStageMass ()

{
	if (SaturnType != SAT_INT20)
	{
		SI_Mass = SI_EmptyMass + SI_FuelMass + (SI_RetroNum * 125);
		SII_Mass = SII_EmptyMass + SII_FuelMass + (SII_UllageNum * 175);
		S4B_Mass = S4B_EmptyMass + S4B_FuelMass;
		SM_Mass = SM_EmptyMass + SM_FuelMass + RCS_FUEL_PER_QUAD * 4.;
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
		SM_Mass = SM_EmptyMass + SM_FuelMass + RCS_FUEL_PER_QUAD * 4.;
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
	oapiWriteScenario_float(scn, "INTERSTAGE", Interstage_Mass);
}

void SaturnV::clbkLoadStateEx (FILEHANDLE scn, void *status)

{
	TRACESETUP("SaturnV::clbkLoadStateEx");

	GetScenarioState(scn, status);

	ClearMeshes();
	SetupMeshes();

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
		// Clear liftoff light now - Apollo 15 checklist item
		ClearLiftoffLight();
		SetThrusterResource(th_main[4], NULL); // Should stop the engine
		SShutS.play(NOLOOP, 235);
		SShutS.done();
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
		//JettisonLET();
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
	case 25:
		// SII OECO
		break;
	case 26:
		// SII/SIVB Direct Staging
		break;
	case 27:
		//Second Staging
		SPUShiftS.done(); // Make sure it's done
		ClearEngineIndicators();
		SeparateStage(LAUNCH_STAGE_SIVB);
		SetStage(LAUNCH_STAGE_SIVB);
		AddRCS_S4B();
		SetSIVBThrusters(true);
		SetThrusterGroupLevel(thg_ver, 1.0);
		SetThrusterResource(th_main[0], ph_3rd);
		break;
	}
}

void SaturnV::SISwitchSelector(int channel)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	switch (channel)
	{
	case 1: //Telemeter Calibrate Off
		break;
	case 2: //Telemeter Calibrate On
		break;
	case 3: //Multiple Engine Cutoff Enable
		break;
	case 4: //LOX Tank Strobe Lights Off
		break;
	case 5: //Fuel Pressurizing Valve No. 2 Open & Tape Recorder Record
		break;
	case 6: //Fuel Pressurizing Valve No. 3 Open
		break;
	case 7: //Fuel Pressurizing Valve No. 4 Open
		break;
	case 8: //Inboard Engine Cutoff
		break;
	case 9: //Outboard Engines Cutoff Enable
		break;
	case 10: //Separation and Retro No. 1 EBW Firing Units Arm
		break;
	case 12: //Separation Camera On
		break;
	case 13: //Telemetry Measurement Switchover
		break;
	case 14: //Outboard Engines Cutoff Enable
		break;
	case 15: //S-IC/S-II Separation (No. 1)
		if (stage == LAUNCH_STAGE_ONE)
		{
			// Drop old stage
			SeparateStage(LAUNCH_STAGE_TWO);
			SetStage(LAUNCH_STAGE_TWO);
			ActivateStagingVent();
			if (SII_UllageNum) {
				SetThrusterGroupLevel(thg_ull, 1.0);
				SepS.play(LOOP, 130);
			}
		}
		break;
		break;
	case 16: //Inboard Engine Cutoff Backup
		break;
	case 17: //Two Adjacent Outboard Engines Out Cutoff Enable
		break;
	case 19: //S-IC/S-II Separation (No. 2)
		if (stage == LAUNCH_STAGE_ONE)
		{
			// Drop old stage
			SeparateStage(LAUNCH_STAGE_TWO);
			SetStage(LAUNCH_STAGE_TWO);
			ActivateStagingVent();
			if (SII_UllageNum) {
				SetThrusterGroupLevel(thg_ull, 1.0);
				SepS.play(LOOP, 130);
			}
		}
		break;
	case 20: //Separation and Retro No. 2 EBW Firing Units Arm
		break;
	default:
		break;
	}
}

void SaturnV::SIISwitchSelector(int channel)
{
	if (stage > LAUNCH_STAGE_TWO_ISTG_JET) return;

	switch (channel)
	{
	case 9: //Stop First PAM - FM/FM Calibration
		break;
	case 11: //S-II Ordnance Arm
		break;
	case 24: //S-II Ullage Trigger
		break;
	case 30: //Start First PAM - FM/FM Relays Reset
		break;
	case 38: //LH2 Tank High Pressure Vent Mode
		break;
	case 71: //Start Data Recorders
		break;
	default:
		break;
	}
}

void SaturnV::SIVBSwitchSelector(int channel)
{
	if (stage >= CSM_LEM_STAGE) return;

	switch (channel)
	{
	case 1: //Passivation Enable
		break;
	case 2: //Passivation Disable
		break;
	case 3: //LOX Tank Repressurization Control Valve Open On
		break;
	case 4: //LOX Tank Repressurization Control Valve Open Off
		break;
	case 7: //PU Inverter and DC Power On
		break;
	case 8: //PU Inverter and DC Power Off
		break;
	case 9: //S-IVB Engine Start On
		break;
	case 10: //Engine Ready Bypass
		break;
	case 11: //Fuel Injection Temperature OK Bypass
		break;
	case 12: //S-IVB Engine Cutoff
		break;
	case 13: //S-IVB Engine Cutoff Off
		break;
	case 14: //Engine Mainstage Control Valve Open On
		break;
	case 15: //Engine Mainstage Control Valve Open Off
		break;
	case 16: //Fuel Injector Temperature OK Bypass Reset
		break;
	case 17: //PU Valve Hardover Position On
		break;
	case 18: //PU Valve Hardover Position Off
		break;
	case 19: //S-IVB Engine EDS Cutoff No. 2 Disable
		break;
	case 22: //LOX Chilldown Pump On
		break;
	case 23: //LOX Chilldown Pump Off
		break;
	case 24: //Engine Pump Purge Control Valve Enable On
		break;
	case 25: //Engine Pump Purge Control Valve Enable Off
		break;
	case 26: //Burner LH2 Propellant Valve Open On
		break;
	case 27: //S-IVB Engine Start Off
		break;
	case 28: //Aux Hydraulic Pump Flight Mode On
		break;
	case 29: //Aux Hydraulic Pump Flight Mode Off
		break;
	case 30: //Start Bottle Vent Control Valve Open On
		break;
	case 31: //Start Bottle Vent Control Valve Open Off
		break;
	case 32: //Second Burn Relay On
		break;
	case 33: //Second Burn Relay Off
		break;
	case 36: //Repressurization System Mode Select On (Amb)
		break;
	case 37: //Repressurization System Mode Select Off (Amb)
		break;
	case 39: //LH2 Tank Repressurization Control Valve Open On
		break;
	case 42: //S-IVB Ullage Engine No. 1 On
		SetAPSUllageThrusterLevel(0, 1);
		break;
	case 43: //S-IVB Ullage Engine No. 1 Off
		SetAPSUllageThrusterLevel(0, 0);
		break;
	case 44: //LOX Tank NPV Valve Latch Open On
		break;
	case 45: //LOX Tank NPV Valve Latch Open Off
		break;
	case 46: //Single Sideband FM Transmitter On
		break;
	case 47: //Single Sideband FM Transmitter Off
		break;
	case 48: //Inflight Calibration Mode On
		break;
	case 49: //Inflight Calibration Mode Off
		break;
	case 50: //Heat-Exchanger Bypass Valve Control Enable
		break;
	case 52: //Measurement Transfer Mode Position "B"
		break;
	case 54: //Charge Ullage Ignition On
		break;
	case 55: //Charge Ullage Jettison On
		break;
	case 56: //Fire Ullage Ignition On
		break;
	case 57: //Fire Ullage Jettison On
		break;
	case 58: //Fuel Chilldown Pump On
		break;
	case 59: //Fuel Chilldown Pump Off
		break;
	case 60: //Burner LH2 Propellant Valve Close On
		break;
	case 61: //Burner LH2 Propellant Valve Close Off
		break;
	case 62: //TM Calibrate On
		break;
	case 63: //TM Calibrate Off
		break;
	case 64: //LH2 Tank Latching Relief Valve Latch On
		break;
	case 65: //LH2 Tank Latching Relief Valve Latch Off
		break;
	case 68: //First Burn Relay On
		break;
	case 69: //First Burn Relay Off
		break;
	case 70: //Burner Exciters On
		break;
	case 71: //Burner Exciters Off
		break;
	case 72: //Burner LH2 Propellant Valve Open Off
		break;
	case 73: //Ullage Firing Reset
		break;
	case 74: //Burner LOX Shutdown Valve Close On
		break;
	case 75: //Burner LOX Shutdown Valve Close Off
		break;
	case 77: //LH2 Tank Vent and Latching Relief Valve Boost Close On
		break;
	case 78: //LH2 Tank Vent and Latching Relief Valve Boost Close Off
		break;
	case 79: //LOX Tank Pressurization Shutoff Valves Close
		break;
	case 80: //LOX Tank Pressurization Shutoff Valves Open
		break;
	case 81: //LH2 Tank Repressurization Control Valve Open Off
		break;
	case 82: //Prevalves Close On
		break;
	case 83: //Prevalves Close Off
		break;
	case 84: //LH2 Tank Continuous Vent Valve Close On
		break;
	case 85: //Burner Automatic Cutoff System Arm
		break;
	case 86: //Burner Automatic Cutoff System Disarm
		break;
	case 87: //LH2 Tank Continuous Vent Valve Close Off
		break;
	case 88: //Ullage Charging Reset
		break;
	case 89: //Burner LOX Shutdown Valve Open On
		break;
	case 90: //Burner LOX Shutdown Valve Open Off
		break;
	case 95: //LOX Tank Vent and NPV Valv Boost Close On
		break;
	case 96: //LOX Tank Vent and NPV Valv Boost Close Off
		break;
	case 97: //Point Level Sensor Arming
		break;
	case 98: //Point Level Sensor Disarming
		break;
	case 99: //LH2 Tank Latching Relief Valve Open On
		break;
	case 100: //LH2 Tank Latching Relief Valve Open Off
		break;
	case 101: //S-IVB Ullage Engine No. 2 On
		SetAPSUllageThrusterLevel(1, 1);
		break;
	case 102: //S-IVB Ullage Engine No. 2 Off
		SetAPSUllageThrusterLevel(1, 0);
		break;
	case 103: //LOX Tank Flight Pressure System On
		break;
	case 104: //LOX Tank Flight Pressure System Off
		break;
	case 105: //LOX Tank NPV Valve Open On
		break;
	case 106: //LOX Tank NPV Valve Open Off
		break;
	case 107: //LH2 Tank Continous Vent Relief Override Shutoff Valve Open On
		break;
	case 108: //LH2 Tank Continous Vent Relief Override Shutoff Valve Open Off
		break;
	case 109: //Engine He Control Valve Open On
		break;
	case 110: //Engine He Control Valve Open Off
		break;
	case 111: //LH2 Tank Continuous Vent Orifice Shutoff Valve Open On
		break;
	case 112: //LH2 Tank Continuous Vent Orifice Shutoff Valve Open Off
		break;
	default:
		break;
	}
}

void SaturnV::SetRandomFailures()
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

		bool EarlySICutoff[5];
		double FirstStageFailureTime[5];

		//
		// Engine failure times for first stage.
		//

		bool EarlySIICutoff[5];
		double SecondStageFailureTime[5];

		//
		// Engine failure times
		//

		for (int i = 0;i < 5;i++)
		{
			EarlySICutoff[i] = 0;
			FirstStageFailureTime[i] = 0.0;
		}

		for (int i = 0;i < 5;i++)
		{
			EarlySIICutoff[i] = 0;
			SecondStageFailureTime[i] = 0.0;
		}

		for (int i = 0;i < 5;i++)
		{
			if (!(random() & 63))
			{
				EarlySICutoff[i] = 1;
				FirstStageFailureTime[i] = 20.0 + ((double)(random() & 1023) / 10.0);
			}
		}

		for (int i = 0;i < 5;i++)
		{
			if (!(random() & 63))
			{
				EarlySIICutoff[i] = 1;
				SecondStageFailureTime[i] = 180.0 + ((double)(random() & 3071) / 10.0);
			}
		}

		iu->GetEDS()->SetEngineFailureParameters(EarlySICutoff, FirstStageFailureTime, EarlySIICutoff, SecondStageFailureTime);

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