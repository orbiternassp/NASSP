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
  *	Revision 1.58  2006/07/31 12:25:03  tschachim
  *	Smoother staging.
  *	
  *	Revision 1.57  2006/07/21 23:04:34  movieman523
  *	Added Saturn 1b engine lights on panel and beginnings of electrical connector work (couldn't disentangle the changes). Be sure to get the config file for the SIVb as well.
  *	
  *	Revision 1.56  2006/06/17 18:14:52  tschachim
  *	Bugfix clbkConsumeBufferedKey.
  *	
  *	Revision 1.55  2006/06/11 21:30:57  movieman523
  *	Fixed Saturn 1b SIVb exhaust.
  *	
  *	Revision 1.54  2006/06/10 23:27:40  movieman523
  *	Updated abort code.
  *	
  *	Revision 1.53  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.52  2006/05/27 11:50:04  movieman523
  *	Improved INT20 support, and made LET jettison work any time during launch on Saturn V.
  *	
  *	Revision 1.51  2006/05/27 00:54:28  movieman523
  *	Simplified Saturn V mesh code a lot, and added beginnings ot INT-20.
  *	
  *	Revision 1.50  2006/04/25 13:53:25  tschachim
  *	New KSC.
  *	
  *	Revision 1.49  2006/04/17 19:12:26  movieman523
  *	Removed some unused switches.
  *	
  *	Revision 1.48  2006/03/30 01:59:37  movieman523
  *	Added RCS to SM DLL.
  *	
  *	Revision 1.47  2006/03/30 00:14:46  movieman523
  *	First pass at SM DLL.
  *	
  *	Revision 1.46  2006/02/22 01:03:02  movieman523
  *	Initial Apollo 5 support.
  *	
  *	Revision 1.45  2006/02/21 11:57:09  tschachim
  *	Fixes to make code build with MS C++ 2005
  *	
  *	Revision 1.44  2006/01/27 22:11:37  movieman523
  *	Added support for low-res Saturn 1b.
  *	
  *	Revision 1.43  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.42  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
  *	Revision 1.41  2006/01/14 00:54:34  movieman523
  *	Hacky wiring of sequential systems and pyro arm switches.
  *	
  *	Revision 1.40  2006/01/09 19:26:03  tschachim
  *	More attempts to make code build on MS C++ 2005
  *	
  *	Revision 1.39  2006/01/07 03:28:28  movieman523
  *	Removed a lot of unused switches and wired up the FDAI power switch.
  *	
  *	Revision 1.38  2006/01/05 11:40:12  tschachim
  *	New dockingprobe handling.
  *	
  *	Revision 1.37  2005/12/19 17:05:33  tschachim
  *	Introduced clbkConsumeBufferedKey
  *	
  *	Revision 1.36  2005/11/25 20:59:49  movieman523
  *	Added thrust decay for SIVb in TLI burn. Still needs tweaking.
  *	
  *	Revision 1.35  2005/11/25 00:02:16  movieman523
  *	Trying to make Apollo 11 work 'by the numbers'.
  *	
  *	Revision 1.34  2005/11/24 20:31:23  movieman523
  *	Added support for engine thrust decay during launch.
  *	
  *	Revision 1.33  2005/11/24 01:07:54  movieman523
  *	Removed code for panel lights which were being set incorrectly. Plus a bit of tidying.
  *	
  *	Revision 1.32  2005/11/23 02:21:30  movieman523
  *	Added S1b stage.
  *	
  *	Revision 1.31  2005/11/19 20:54:47  movieman523
  *	Added SIVb DLL and wired it up to Saturn 1b.
  *	
  *	Revision 1.30  2005/11/16 20:21:39  movieman523
  *	CSM/LEM renaming changes.
  *	
  *	Revision 1.29  2005/11/16 00:18:49  movieman523
  *	Added beginnings of really basic IU emulation. Added random failures of caution and warning lights on non-historical missions. Added initial support for Skylab CM and SM. Added LEM Name option in scenario file.
  *	
  *	Revision 1.28  2005/10/31 10:22:17  tschachim
  *	SPSSwitch is now 2-pos, new ONPAD_STAGE.
  *	
  *	Revision 1.27  2005/10/19 11:29:55  tschachim
  *	Changed log file name.
  *	
  *	Revision 1.26  2005/10/11 16:38:50  tschachim
  *	Bugfix tower jettison and aborts.
  *	
  *	Revision 1.25  2005/08/30 14:53:00  spacex15
  *	Added conditionnally defined AGC_SOCKET_ENABLED to use an external socket connected virtual AGC
  *	
  *	Revision 1.24  2005/08/23 22:18:47  movieman523
  *	SPS switch now works.
  *	
  *	Revision 1.23  2005/08/23 21:29:03  movieman523
  *	RCS state is now only checked when a stage event occurs or when a valve is opened or closed, not every timestep.
  *	
  *	Revision 1.22  2005/08/21 22:20:59  movieman523
  *	Fixed SM RCS and activated SIVB RCS at all times for now.
  *	
  *	Revision 1.21  2005/08/20 11:14:52  movieman523
  *	Added Rot Contr Pwr switches and removed a number of old switches which aren't used anymore.
  *	
  *	Revision 1.20  2005/08/19 20:05:44  movieman523
  *	Added abort switches. Wired in Tower Jett switches and SIVB Sep switch.
  *	
  *	Revision 1.19  2005/08/17 00:01:59  movieman523
  *	Added ECS indicator switch, revised state saving, revised Timestep code to pass in the delta-time so we don't need to keep calculating it.
  *	
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

#include "s1b.h"

#include "tracer.h"

#include "CollisionSDK/CollisionSDK.h"

char trace_file[] = "ProjectApollo Saturn1b.log";


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

	SaturnType = SAT_SATURN1B;

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

	//
	// Engines per stage.
	//

	SI_EngineNum = 8;
	SII_EngineNum = 1;
	SIII_EngineNum = 1;
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
	int i;
	for (i = 0; i < nlift-1 && AOA[i+1] < aoa; i++);
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

	int i;
	for (i = 0; i < nlift-1 && AOA[i+1] < aoa; i++);
	return -(CL[i] + (aoa-AOA[i])*SCL[i]);
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
}

void Saturn1b::StageOne(double simt, double simdt)

{
	VESSELSTATUS vs;
	GetStatus(vs);

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	if (MainLevel < 0.3 && MissionTime < 100 && EDSSwitch.GetState() && MissionTime > 10) {
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
			SetEngineIndicator(6);
			SetEngineIndicator(7);
			SetEngineIndicator(8);

			SetThrusterResource(th_main[4],NULL);
			SetThrusterResource(th_main[5],NULL);
			SetThrusterResource(th_main[6],NULL);
			SetThrusterResource(th_main[7],NULL);
			ClearLiftoffLight();
			SShutS.play(NOLOOP,235);
			SShutS.done();

			// Create hidden SIB vessel
			char VName[256];
			VESSELSTATUS vs;

			GetStatus(vs);
			strcpy (VName, GetName()); 
			strcat (VName, "-STG1");
			hstg1 = oapiCreateVessel(VName,"ProjectApollo/nsat1stg1", vs);

			// Load only the necessary meshes
			S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
			stage1->LoadMeshes(LowRes);

			StageState++;
		}
		break;

	//
	// We don't actually have time here to play the countdown before seperation after the center
	// engine shuts down: with the Saturn 1 there are only a few seconds between the two events.
	//

	case 1:
		if ((GetFuelMass() <= 0.001) || (MissionTime >= FirstStageShutdownTime) || bManualSeparate)
		{
			NextMissionEventTime = MissionTime + 0.7;
			SetEngineIndicators();
			StageState++;
		}
		break;

	case 2:
		if (MissionTime >= NextMissionEventTime)
		{
			SShutS.done();
			ClearEngineIndicators();
			SeparateStage (LAUNCH_STAGE_TWO);
			bManualSeparate = false;
			SeparationS.play(NOLOOP, 245);
			SetStage(LAUNCH_STAGE_TWO);
		}
		else
		{

			//
			// Engine thrust decay.
			//

			for (int i = 0; i < 4; i++)
			{
				double Level = GetThrusterLevel(th_main[i]);
				Level -= (simdt * 1.2);
				SetThrusterLevel(th_main[i], Level);
			}
		}
		return;
	}

	if (autopilot && CMCswitch)
	{
		AutoPilot(MissionTime);
	}
	else
	{
		AttitudeLaunch1();
	}
}

//
// Start up the SIVB and separate the launch tower.
//

void Saturn1b::StageStartSIVB(double simt)

{
	if (autopilot && CMCswitch)
	{
		AutoPilot(MissionTime);
	}
	else
	{
		AttitudeLaunchSIVB();
	}

	switch (StageState)
	{

	case 0:
		SepS.play(LOOP, 130);
		SetThrusterGroupLevel(thg_ver,1.0);
		NextMissionEventTime = MissionTime + 2.0;
		SetSIVBMixtureRatio(5.0);
		StageState++;
		break;

	case 1:
		if (MissionTime >= NextMissionEventTime)
		{
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
		if (MissionTime  < NextMissionEventTime)
		{
			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterLevel(th_main[0], 0.9 * (deltat / 2.5));
		}
		else
		{
			SetThrusterLevel(th_main[0], 0.9);
			LastMissionEventTime = NextMissionEventTime;
			NextMissionEventTime += 2.1;
			StageState++;
		}
		if (GetThrusterLevel(th_main[0]) > 0.65) 
		{
			ClearEngineIndicator(1);
		}
		break;

	//
	// Bring engine to full power.
	//

	case 3:
		if (MissionTime  < NextMissionEventTime)
		{
			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterLevel(th_main[0], 0.9 + (deltat / 21.0));
		}
		else
		{
			SetThrusterLevel(th_main[0], 1.0);
			SepS.stop();
			AddRCS_S4B();
			SetThrusterGroupLevel(thg_ver, 0.0);


			//
			// Checklist actions
			//

			// EDS auto off
			EDSSwitch.SwitchTo(TOGGLESWITCH_DOWN);
			TwoEngineOutAutoSwitch.SwitchTo(TOGGLESWITCH_DOWN);
			LVRateAutoSwitch.SwitchTo(TOGGLESWITCH_DOWN);

			// Activate primary evaporator
			GlycolEvapSteamPressAutoManSwitch.SwitchTo(TOGGLESWITCH_UP);
			GlycolEvapH2oFlowSwitch.SwitchTo(THREEPOSSWITCH_UP);


			NextMissionEventTime = MissionTime + 2.05;
			StageState++;
		}
		break;

	//
	// First mixture ratio shift.
	//

	case 4:
		if (MissionTime >= NextMissionEventTime)
		{
			SetSIVBThrusters(true);
			SetSIVBMixtureRatio(5.5);
			NextMissionEventTime = MissionTime + 17.95;

			//
			// Override if required.
			//

			if (LESJettisonTime < NextMissionEventTime) {
				NextMissionEventTime = LESJettisonTime;
			}

			StageState++;
		}
		break;

	//
	// Jettison LET and move on.
	//

	case 5:
		if ((MissionTime >= NextMissionEventTime && (TowerJett1Switch.GetState() == THREEPOSSWITCH_DOWN || TowerJett2Switch.GetState() == THREEPOSSWITCH_DOWN)) || 
			bManualSeparate || 
			GetFuelMass() == 0 || 
			TowerJett1Switch.GetState() == THREEPOSSWITCH_UP || 
			TowerJett2Switch.GetState() == THREEPOSSWITCH_UP)
		{
			SeparateStage(LAUNCH_STAGE_SIVB);
			TowerJS.play();
			TowerJS.done();
			SetStage(LAUNCH_STAGE_SIVB);
			// Enable docking probe because the tower is gone
			dockingprobe.SetEnabled(true);
		}
		return;
	}

	if(CsmLvSepSwitch.GetState())
	{
		bManualSeparate = true;
	}

	if (bManualSeparate || bAbort)
	{
		SepS.stop();
		bManualSeparate = false;
		SeparateStage (CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
		if (bAbort)
		{
			SPSswitch.SetState(TOGGLESWITCH_UP);
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
    if (autopilot && CMCswitch)
	{
		AutoPilot(MissionTime);
	}
	else
	{
		AttitudeLaunchSIVB();
	}

	switch (StageState)
	{

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
			SetSIVBThrusters(true);
		}
		break;
	}

	if(CsmLvSepSwitch.GetState()) {
		bManualSeparate = true;
	}

	if (bManualSeparate || bAbort)
	{
		bManualSeparate = false;
		SeparateStage(CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
		if (bAbort){
			SPSswitch.SetState(TOGGLESWITCH_UP);
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

	if (hs4bM)
	{
		if (GetDockStatus(GetDockHandle(0)) == hs4bM)
		{
			if(ASTPMission)
				ReadyAstp=true;
			dockstate=2;
				//	sprintf(oapiDebugString() ,"S4B %f");
		}
		else
		{
			ReadyAstp=false;
				//	sprintf(oapiDebugString() ,"NOT S4B %f");
		}

		if (dockstate>=2 && !S4BASTP)
		{
			if(ASTPMission)
				S4BASTP=true;
		}
	}
	else
	{
		char VName[256];

		strcpy (VName, GetName()); strcat (VName, "-S4BSTG");
		hs4bM = oapiGetVesselByName(VName);
	}

	if (bAbort && stage <= LAUNCH_STAGE_TWO)
	{
		if (stage < LAUNCH_STAGE_ONE)
		{
			// No abort before launch
			bAbort = false;
		} 
		else
		{
			SetEngineLevel(ENGINE_MAIN, 0);
			SeparateStage(CM_ENTRY_STAGE);
			StartAbort();
			SetStage(CM_ENTRY_STAGE);
			bAbort = false;
		}
		return;
	}

	if (stage == CSM_LEM_STAGE)
	{
		if(simt>0.5)
			AttitudeLaunch4();

		if (SivbLmSepSwitch.GetState())
		{
			if (ASTPMission)
			{
				//sprintf(oapiDebugString() ,"click %f");
				SivbLmSepSwitch = false;
				if (ReadyAstp||ReadyAstp1||dockstate==3)
				{
					bManualUnDock = true;
				}
			}
		}
		if (CsmLmFinalSep1Switch.GetState() || CsmLmFinalSep2Switch.GetState()) {
			if (dockstate == 3) {
				ProbeJetison = true;
				bManualUnDock = true;
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

		if (CMSMPyros.Blown())
		{
			SeparateStage (CM_STAGE);
			bManualSeparate=false;
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
		if ((bManualSeparate ||  altlow) && PyrosArmed())
			{
				SeparateStage (CM_STAGE);
				SetStage(CM_STAGE);
				bManualSeparate=false;
				abortTimer = 0;

			}
		}
	}

	switch (stage) {

	case LAUNCH_STAGE_ONE:
		StageOne(simt, simdt);
		break;

	case LAUNCH_STAGE_TWO:
		StageStartSIVB(simt);
		break;

	case LAUNCH_STAGE_SIVB:
		StageLaunchSIVB(simt);
		break;

	case STAGE_ORBIT_SIVB:
		StageOrbitSIVB(simt, simdt);
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

void Saturn1b::clbkLoadStateEx (FILEHANDLE scn, void *vs)

{
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

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStage();
		SetSecondStageEngines();
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

	//
	// Enable or disable SPS and RCS.
	//

	CheckSPSState();
	CheckRCSState();
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

	case CSM_ABORT_STAGE:
		SetAbortStage();
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

	char *line;

	while (oapiReadScenario_nextline (cfg, line)) {
		ProcessConfigFileLine(cfg, line);
	}

	// Disable CollisionSDK for the moment
	VSRegVessel(GetHandle());
	VSDisableCollisions(GetHandle());
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
	SM_Mass = SM_EmptyMass + SM_FuelMass;
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