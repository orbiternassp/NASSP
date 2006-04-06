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
  *	Revision 1.74  2006/04/05 19:48:05  movieman523
  *	Added low-res SM RCS meshes and updated Apollo 13.
  *	
  *	Revision 1.73  2006/03/30 01:59:37  movieman523
  *	Added RCS to SM DLL.
  *	
  *	Revision 1.72  2006/03/30 00:14:46  movieman523
  *	First pass at SM DLL.
  *	
  *	Revision 1.71  2006/03/03 05:12:36  dseagrav
  *	Added DirectInput code and THC/RHC interface. Changes 20060228-20060302
  *	
  *	Revision 1.70  2006/02/27 00:57:48  dseagrav
  *	Added SPS thrust-vector control. Changes 20060225-20060226.
  *	
  *	Revision 1.69  2006/02/22 18:52:10  tschachim
  *	Bugfixes for Apollo 4-6.
  *	
  *	Revision 1.68  2006/02/21 11:55:49  tschachim
  *	Moved TLI sequence to the IU.
  *	
  *	Revision 1.67  2006/02/10 21:09:22  tschachim
  *	Bugfix SivbLmSepSwitch.
  *	
  *	Revision 1.66  2006/02/09 18:28:12  lazyd
  *	Another pitch table change...
  *	
  *	Revision 1.65  2006/02/09 14:40:02  tschachim
  *	Again fixed SIC thrust.
  *	
  *	Revision 1.64  2006/02/08 12:07:03  tschachim
  *	Fixed SIC thrust.
  *	
  *	Revision 1.63  2006/02/07 19:33:30  lazyd
  *	Changed pitch table for different masses
  *	
  *	Revision 1.62  2006/02/07 14:05:36  tschachim
  *	Changed the Saturn parameters according to the Apollo 11 Saturn V flight evaluation report.
  *	
  *	Revision 1.61  2006/02/04 20:57:59  lazyd
  *	Pitch table change
  *	
  *	Revision 1.60  2006/02/03 19:46:37  tschachim
  *	Bugfix engine thrust decay.
  *	
  *	Revision 1.59  2006/02/01 18:35:47  tschachim
  *	More REALISM 0 checklist actions.
  *	
  *	Revision 1.58  2006/01/31 21:29:20  lazyd
  *	New pitch table
  *	
  *	Revision 1.57  2006/01/31 00:50:32  lazyd
  *	added stuff for new autopilot
  *	
  *	Revision 1.56  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.55  2006/01/26 03:59:03  movieman523
  *	Resolution can now be read from config file.
  *	
  *	Revision 1.54  2006/01/26 03:31:57  movieman523
  *	Less hacky low-res mesh support for Saturn V.
  *	
  *	Revision 1.53  2006/01/24 13:56:46  tschachim
  *	Smoother staging with more eye-candy.
  *	
  *	Revision 1.52  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
  *	Revision 1.51  2006/01/14 00:54:35  movieman523
  *	Hacky wiring of sequential systems and pyro arm switches.
  *	
  *	Revision 1.50  2006/01/09 19:26:03  tschachim
  *	More attempts to make code build on MS C++ 2005
  *	
  *	Revision 1.49  2006/01/07 03:28:28  movieman523
  *	Removed a lot of unused switches and wired up the FDAI power switch.
  *	
  *	Revision 1.48  2006/01/05 11:42:44  tschachim
  *	New dockingprobe handling, bugfix.
  *	
  *	Revision 1.47  2005/12/20 16:10:22  tschachim
  *	Improved timing of the engine indicators during SIC/SII staging.
  *	
  *	Revision 1.46  2005/12/19 17:04:59  tschachim
  *	checklist actions, engine indicators.
  *	
  *	Revision 1.45  2005/11/25 20:59:49  movieman523
  *	Added thrust decay for SIVb in TLI burn. Still needs tweaking.
  *	
  *	Revision 1.44  2005/11/25 02:03:47  movieman523
  *	Fixed mixture-ratio change code and made it more realistic.
  *	
  *	Revision 1.43  2005/11/25 00:02:16  movieman523
  *	Trying to make Apollo 11 work 'by the numbers'.
  *	
  *	Revision 1.42  2005/11/24 20:31:23  movieman523
  *	Added support for engine thrust decay during launch.
  *	
  *	Revision 1.41  2005/11/24 01:07:54  movieman523
  *	Removed code for panel lights which were being set incorrectly. Plus a bit of tidying.
  *	
  *	Revision 1.40  2005/11/23 01:43:13  movieman523
  *	Added SII stage DLL.
  *	
  *	Revision 1.39  2005/11/23 00:29:38  movieman523
  *	Added S1C DLL and revised LEVA code to look for NEP-specific flag if it exists.
  *	
  *	Revision 1.38  2005/11/20 01:06:27  movieman523
  *	Saturn V now uses SIVB DLL too.
  *	
  *	Revision 1.37  2005/11/16 20:21:39  movieman523
  *	CSM/LEM renaming changes.
  *	
  *	Revision 1.36  2005/11/16 00:18:49  movieman523
  *	Added beginnings of really basic IU emulation. Added random failures of caution and warning lights on non-historical missions. Added initial support for Skylab CM and SM. Added LEM Name option in scenario file.
  *	
  *	Revision 1.35  2005/11/09 18:15:48  tschachim
  *	New Saturn assembly process.
  *	
  *	Revision 1.34  2005/10/31 10:23:26  tschachim
  *	SPSSwitch is now 2-pos, new ONPAD_STAGE.
  *	
  *	Revision 1.33  2005/10/19 11:31:10  tschachim
  *	Changed log file name.
  *	
  *	Revision 1.32  2005/10/11 16:39:27  tschachim
  *	Bugfix tower jettison and aborts.
  *	
  *	Revision 1.31  2005/08/24 00:30:00  movieman523
  *	Revised CM RCS code, and removed a load of switches that aren't used anymore.
  *	
  *	Revision 1.30  2005/08/23 22:18:47  movieman523
  *	SPS switch now works.
  *	
  *	Revision 1.29  2005/08/23 21:29:03  movieman523
  *	RCS state is now only checked when a stage event occurs or when a valve is opened or closed, not every timestep.
  *	
  *	Revision 1.28  2005/08/21 22:21:00  movieman523
  *	Fixed SM RCS and activated SIVB RCS at all times for now.
  *	
  *	Revision 1.27  2005/08/20 11:14:52  movieman523
  *	Added Rot Contr Pwr switches and removed a number of old switches which aren't used anymore.
  *	
  *	Revision 1.26  2005/08/19 21:33:20  movieman523
  *	Added initial random failure support.
  *	
  *	Revision 1.25  2005/08/19 20:05:44  movieman523
  *	Added abort switches. Wired in Tower Jett switches and SIVB Sep switch.
  *	
  *	Revision 1.24  2005/08/17 00:01:59  movieman523
  *	Added ECS indicator switch, revised state saving, revised Timestep code to pass in the delta-time so we don't need to keep calculating it.
  *	
  *	Revision 1.23  2005/08/16 20:55:23  movieman523
  *	Added first saturn-specific switch for Xlunar Inject.
  *	
  *	Revision 1.22  2005/08/15 19:25:03  movieman523
  *	Added CSM attitude control switches and removed old ones.
  *	
  *	Revision 1.21  2005/08/15 18:48:50  movieman523
  *	Moved the stage destroy code into a generic function for Saturn V and 1b.
  *	
  *	Revision 1.20  2005/08/15 02:37:57  movieman523
  *	SM RCS is now wired up.
  *	
  *	Revision 1.19  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.18  2005/08/08 20:32:57  movieman523
  *	Added initial support for offsetting the mission timer and event timer from MissionTime: the real timers could be adjusted using the switches on the control panel (which aren't wired up yet), and the event timer would reset to zero on an abort.
  *	
  *	Revision 1.17  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.16  2005/08/05 13:08:35  tschachim
  *	Added crawler callback function LaunchVesselRolloutEnd,
  *	added keyboard handling for BuildFirstStage (B and U keys)
  *	
  *	Revision 1.15  2005/08/01 19:07:46  movieman523
  *	Genericised code to deal with SM destruction on re-entry, and did some tidying up of Saturn 1b code.
  *	
  *	Revision 1.14  2005/07/31 01:43:12  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.13  2005/07/30 02:05:47  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.12  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.11  2005/07/19 19:25:09  movieman523
  *	For now, always enable SIVB RCS by default when we're in orbit since we don't have switches to do so at the moment :).
  *	
  *	Revision 1.10  2005/07/05 17:55:29  tschachim
  *	Fixed behavior of the CmSmSep1/2Switches
  *	
  *	Revision 1.9  2005/06/06 12:09:31  tschachim
  *	New switches
  *	
  *	Revision 1.8  2005/05/31 02:12:08  movieman523
  *	Updated pre-entry burn variables and wrote most of the code to handle them.
  *	
  *	Revision 1.7  2005/05/18 23:34:23  movieman523
  *	Added roughly correct masses for the various Saturn payloads.
  *	
  *	Revision 1.6  2005/04/20 17:57:16  movieman523
  *	Added AGC restart to Apollo 13.
  *	
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
#include "IMU.h"

#include "saturn.h"
#include "saturnv.h"
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

//
// The BODGE_FACTOR is subtracted from the empty mass of the SII
// stage to increase the amount of available fuel when the SIVB
// reaches orbit. This is totally unrealistic, but makes the mission
// easier to fly!
//
/*
#define BODGE_FACTOR	10000 // 0
*/
#define BODGE_FACTOR	0 // 0
const double BASE_SII_MASS = 42400 + 3490 - BODGE_FACTOR;		// Stage + SII/SIVB interstage

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

	//
	// Default ISP and thrust values.
	//

	ISP_FIRST_SL    = 2594.4;
	ISP_FIRST_VAC   = 2979.4;
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

	//
	// State variables.
	//

	TLICapableBooster = true;
	GoHover = false;
	Scorrec = false;
	Resetjet = false;
	Burned = false;
	ApolloExploded = false;
	CryoStir = false;
	KranzPlayed = false;
	EVA_IP = false;

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

	SM_EmptyMass = 6100;
	SM_FuelMass = 20500;

	CM_EmptyMass = 5700;
	CM_FuelMass = 75;

	CalculateStageMass();

	// load wave (Handle defined in begining of file)
	soundlib.LoadSound(SpeedS, "reentry.wav");
	soundlib.LoadSound(DockS, "docking.wav");
	soundlib.LoadSound(SCorrection, "Correction.wav");

	soundlib.LoadSound(SRover, "LRover.WAV");

	//
	// Pitch program.
	//

	for (int i = 0; i < PITCH_TABLE_SIZE; i++) {
		met[i] = default_met[i];
		cpitch[i] = default_cpitch[i];
	}
}

SaturnV::~SaturnV()

{
	TRACESETUP("~SaturnV");

	ReleaseSurfaces();
}

void SaturnV::CalculateStageMass ()

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

//
// Adjust the mixture ratio of the engines on the SII stage. This occured late in
// the flight to ensure that the fuel was fully burnt before the stage was dropped.
//

void SaturnV::SetSIICMixtureRatio (double ratio)

{
	double isp, thrust;

	// Hardcoded ISP and thrust according to the Apollo 11 Saturn V flight evaluation report.
	// http://klabs.org/history/history_docs/jsc_t/apollo_11_saturn_v.pdf

	if (ratio == 5.5) {
		thrust = 1028303.;
		isp = 4165.;
	
	} else if (ratio == 4.3) {
		thrust = 770692.;
		isp = 4180.;

	} else {
		isp = GetJ2ISP(ratio);
		thrust = THRUST_SECOND_VAC * ThrustAdjust;
	}

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

	agc.SetVesselStats(isp, thrust, false);

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

	if (ratio == 4.9) {
		thrust = 901223.;
		isp = 4202.;
	
	} else {
		isp = GetJ2ISP(ratio);
		thrust = THRUST_THIRD_VAC * ThrustAdjust;
	}
	isp = GetJ2ISP(ratio);

	//
	// For simplicity assume no ISP change at sea-level: SIVb stage should always
	// be in near-vacuum anyway.
	//

	SetThrusterIsp (th_main[0], isp, isp);
	SetThrusterMax0 (th_main[0], thrust);

	//
	// Give the AGC our new stats.
	//

	agc.SetVesselStats(isp, thrust, false);

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

	char *line;

	while (oapiReadScenario_nextline (cfg, line)) {
		ProcessConfigFileLine(cfg, line);
	}
}

void SaturnV::StageOne(double simt, double simdt)

{
	VESSELSTATUS vs;
	GetStatus(vs);

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	double amt = (MainLevel) * 0.4;
	JostleViewpoint(amt);

	if (vs.status == 1 ){
		MoveEVA();
	}

	if (GetEngineLevel(ENGINE_MAIN) <0.3 && MissionTime <100 && EDSSwitch.GetState() && MissionTime > 10){
		bAbort = true;
	}

	switch (StageState) {

	case 0:

		//
		// Shut down center engine at 6% fuel or if acceleration goes
		// over 3.98g, or at planned shutdown time.
		//

		if ((actualFUEL <= 6) || (MissionTime >= FirstStageCentreShutdownTime)) {
			//
			// Set center engine light.
			//
			SetEngineIndicator(5);

			//
			// Clear liftoff light now - Apollo 15 checklist
			//
			ClearLiftoffLight();
			SetThrusterResource(th_main[4], NULL);


			//
			// Checklist actions
			//

			// EDS auto off
			EDSSwitch.SwitchTo(TOGGLESWITCH_DOWN);
			TwoEngineOutAutoSwitch.SwitchTo(TOGGLESWITCH_DOWN);
			LVRateAutoSwitch.SwitchTo(TOGGLESWITCH_DOWN);


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
			SeparateStage (stage);
			bManualSeparate = false;
			SeparationS.play(NOLOOP, 245);
			SetStage(LAUNCH_STAGE_TWO);
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


				//
				// Checklist actions
				//

				// Activate primary evaporator
				GlycolEvapSteamPressAutoManSwitch.SwitchTo(TOGGLESWITCH_UP);
				GlycolEvapH2oFlowSwitch.SwitchTo(THREEPOSSWITCH_UP);


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

		if (MissionTime >= NextMissionEventTime || bManualSeparate) {

			SeparateStage (stage);
			SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
			bManualSeparate = false;
			SIISepState = false;

			NextMissionEventTime += 5.7;

			//
			// Override if required.
			//

			if (LESJettisonTime < NextMissionEventTime) {
				NextMissionEventTime = LESJettisonTime;
			}
		}
		break;
	}
}

void SaturnV::StageThree(double simt)

{
	double MainLevel = GetEngineLevel(ENGINE_MAIN);

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

	if ((MissionTime >= NextMissionEventTime && (TowerJett1Switch.GetState() == THREEPOSSWITCH_DOWN || TowerJett2Switch.GetState() == THREEPOSSWITCH_DOWN)) || 
		bManualSeparate || 
		GetFuelMass() == 0 || 
		TowerJett1Switch.GetState() == THREEPOSSWITCH_UP || 
		TowerJett2Switch.GetState() == THREEPOSSWITCH_UP)
	{
		SeparateStage (stage);
		SetStage(LAUNCH_STAGE_TWO_TWR_JET);
		bManualSeparate = false;
		// Enable docking probe because the tower is gone
		dockingprobe.SetEnabled(true);
	}
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

		if (Crewed) {
			SwindowS.play();
		}
		SwindowS.done();
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

			if (!LaunchFail.u.SIIAutoSepFail) {
				SeparateStage (stage);
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

	if (bManualSeparate || SIISIVBSepSwitch.GetState()) {
		bManualSeparate = false;
		SeparateStage (stage);
		ClearEngineIndicators();
		NextMissionEventTime = MissionTime;
		SetStage(LAUNCH_STAGE_SIVB);
	}

	if (bAbort) {
		SeparateStage (stage);
		SetEngineIndicators();
		StartAbort();
		SetStage(LAUNCH_STAGE_SIVB);
		ABORT_IND = true;
		bAbort = true;
	}
}

void SaturnV::StageSix(double simt)

{
	// Call to AttitudeLaunch4 deleted - DS20060226

	if(RPswitch17){
		if (ActivateLEM && hLMV){
			ActivateLEM=false;
			RPswitch17=false;
			VESSEL *lmvessel;
			lmvessel=oapiGetVesselInterface(hLMV);
			lmvessel->SetEnableFocus(true);
		}
	}

	//
	// Should we be turning off these lights here?
	//

	for (int i=0 ;i<6;i++){
		LAUNCHIND[i]=false;
	}

	if (hLMV && dockstate <5){
		if (GetDockStatus(GetDockHandle(0)) == hLMV){
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

	if (CsmLmFinalSep1Switch.GetState() || CsmLmFinalSep2Switch.GetState()) {
		if (ActivateLEM) {
			ProbeJetison = true;
			bManualUnDock = true;
		}
	}

	if (SivbLmSepSwitch.GetState()) {
		bManualUnDock = true;
		SivbLmSepSwitch.SetState(false); // temporary bugfix, TODO get rid of bManualUnDock, use pyros, connect this switch and fix REALSIM 0 "S" key press
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

			//
			// Update the mesh.
			//

			SetCSMStage();

			//
			// Blow off Panel 4.
			//

			VESSELSTATUS vs1;

			const double CGOffset = 12.25+21.5-1.8+0.35;

			VECTOR3 vel1 = { 0.0, -0.25, 0.15 };
			VECTOR3 ofs1 = { 0, 0, 30.25 - CGOffset};

			GetStatus (vs1);

			VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};

			Local2Rel (ofs1, vs1.rpos);
			GlobalRot (vel1, rofs1);

			vs1.rvel.x = rvel1.x+rofs1.x;
			vs1.rvel.y = rvel1.y+rofs1.y;
			vs1.rvel.z = rvel1.z+rofs1.z;
			vs1.vrot.x = 0.005;
			vs1.vrot.y = 0.05;
			vs1.vrot.z = 0.01;

			char VName[256];

			GetApolloName(VName);
			strcat (VName, "-PANEL4");

			oapiCreateVessel(VName,"ProjectApollo/SM-Panel4",vs1);

			//
			// This is actually wrong because it will give us an
			// artificially low mass for the CSM. We should just disable
			// the engines.
			//

			SetPropellantMass(ph_rcs0,0);
			SetPropellantMass(ph_sps,0);
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
				NextMissionEventTime = CSMBurnStart - 100.0;
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
				SlowIfDesired();
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
				NextMissionEventTime = MissionTime + CalculateApogeeTime() - 100;
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
				SlowIfDesired();

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

			//
			// Final acceleration burn prior to entry.
			//

		case 10:
			if (CSMAccelSet) {
				NextMissionEventTime = CSMAccelTime - 180;
				StageState++;
			}
			break;

		case 11:
			if (MissionTime >= NextMissionEventTime) {
				SlowIfDesired();
				ActivateCSMRCS();
				ActivateNavmode(NAVMODE_PROGRADE);
				NextMissionEventTime = CSMAccelTime;
				StageState++;
			}
			break;

		//
		// What we really need to do here is ensure that we stay pitched down at the appropriate
		// level relative to the local horizon. Currently this code will leave us with a perigee
		// that's way too high, so disable it for now.
		//

		case 12:
			if (MissionTime >= NextMissionEventTime) {
//				SlowIfDesired();
				ActivateNavmode(NAVMODE_PROGRADE);
				ActivateSPS();
//				SetThrusterGroupLevel(thg_main, 1.0);
				NextMissionEventTime = CSMAccelEnd;
				StageState++;
			}
			break;

		case 13:
			if (MissionTime >= NextMissionEventTime) {
				ActivateNavmode(NAVMODE_PROGRADE);
				SetThrusterGroupLevel(thg_main, 0.0);
				DeactivateSPS();
				CSMAccelSet = false;
				NextMissionEventTime = MissionTime + 10.0;
				StageState++;
			}
			break;

		case 14:
			if (MissionTime >= NextMissionEventTime) {
				ActivateNavmode(NAVMODE_KILLROT);
				NextMissionEventTime = MissionTime + 10.0;
				StageState++;
			}
			break;

		case 15:
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
			CmSmSep1Switch.SwitchTo(TOGGLESWITCH_UP);
			CmSmSep2Switch.SwitchTo(TOGGLESWITCH_UP);
		}
	}

	if (bManualUnDock)
	{
		if (GetDockStatus(GetDockHandle(0)) == NULL) {
			bManualUnDock = false;
		}
		else if (GetDockStatus(GetDockHandle(0)) == hs4bM) { //this check is for docking status if docked we cannot jetison ASTP
			if (PyrosArmed() && SECSLogicActive()) {
				dockstate = 2;
				release_time = simt;
				Resetjet = true;
				DockStage (dockstate);
			}
			bManualUnDock = false;
		}
		else if (GetDockStatus(GetDockHandle(0)) == hLMV) {
			if (dockstate == 3 && ProbeJetison) {
				// Final separation
				DockStage (dockstate);
				Undock(0);
				// Disable docking probe because it's jettisoned 
				dockingprobe.SetEnabled(false);

			} else {
				// Normal undocking
				dockingprobe.Extend(); 
			}
			bManualUnDock = false;
		}
		else {
			bManualUnDock = false;
		}
	}

	if (CMSMPyros.Blown())
	{
		if (dockstate <= 1 || dockstate >= 3) {
			SeparateStage (stage);
			bManualSeparate = false;
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
	strcpy (VName, ApolloName); strcat (VName, "-S4BSTG");
	hs4bM= oapiGetVesselByName(VName);
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

	GetLEMName(VName);
	hLMV = oapiGetVesselByName(VName);

	strcpy (VName, ApolloName); strcat (VName, "-DCKPRB");
	hPROBE = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-EVA");
	hEVA = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-INTSTG");
	hintstg = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-STG2");
	hstg2 = oapiGetVesselByName(VName);

	habort = oapiGetVesselByName("Saturn_Abort");
}

void SaturnV::Timestep(double simt, double simdt)

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

	GenericTimestep(simt, simdt);

	if (bAbort && stage < LAUNCH_STAGE_TWO_TWR_JET) {
		if (stage < LAUNCH_STAGE_ONE) {
			// No abort before launch
			bAbort = false;
		} else {
			SetEngineLevel(ENGINE_MAIN, 0);
			SeparateStage(stage);
			StartAbort();
			SetStage(CSM_ABORT_STAGE);
			bAbort = false;
		}
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
		StageThree(simt);
		break;

	case LAUNCH_STAGE_TWO_TWR_JET:
		StageFour(simt, simdt);
		break;

	case LAUNCH_STAGE_SIVB:
		StageLaunchSIVB(simt);
		break;

	case STAGE_ORBIT_SIVB:
		StageOrbitSIVB(simt, simdt);
		break;
	
	case CSM_LEM_STAGE:
		StageSix(simt);
		break;

	case CSM_ABORT_STAGE:
		SetEngineLevel(ENGINE_MAIN,1);

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

			if ((bManualSeparate || GetAltitude() < 500) && PyrosArmed())
			{
				SeparateStage (stage);
				SetStage(CM_STAGE);
				bManualSeparate=false;
				abortTimer = 0;
			}
		}
		break;

	default:
		GenericTimestepStage(simt, simdt);
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
	SII_RetroNum = 8;

	if (VehicleNo > 500 && VehicleNo < 503) {
		if (!S1_ThrustLoaded)
			THRUST_FIRST_VAC = 7835000; // 7653000; // TODO: Temporary fix, otherwise the autopilot is not working properly, we should replace this when we figured out all parameters (masses, fuel masses etc.) of these vehicles
		if (!S2_ThrustLoaded)
			THRUST_SECOND_VAC = 1001000;
		if (!S3_ThrustLoaded)
			THRUST_THIRD_VAC = 1001000;

		SII_UllageNum = 8;
	}
	else if (VehicleNo >= 503 && VehicleNo < 510) {
		if (!S1_ThrustLoaded)
			THRUST_FIRST_VAC = 7835000;

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

void SaturnV::clbkLoadStateEx (FILEHANDLE scn, void *status)

{
	TRACESETUP("SaturnV::clbkLoadStateEx");

	GetScenarioState(scn, status);

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
			BuildFirstStage(buildstatus);
		}
		else{
			SetFirstStage();
			//ShiftCentreOfMass (_V(0,0,STG0O));	// Seems to be useless...
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
		//
		// Always enable SIVB RCS for now, once we hit orbit.
		//

		SetSIVBThrusters(true);
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

	//
	// Enable or disable SPS and RCS.
	//

	CheckSPSState();
	CheckRCSState();
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
			NextMissionEventTime = MissionTime + 10.0;
			SetStage(STAGE_ORBIT_SIVB);
			bAbtlocked =true;
		}
		return;
	}

	if(CsmLvSepSwitch.GetState()){
		bManualSeparate =true;
	}

	if (bManualSeparate || bAbort)
	{
		SepS.stop();
		bManualSeparate = false;
		SeparateStage (stage);
		SetStage(CSM_LEM_STAGE);
		if (bAbort){
			SPSswitch.SetState(TOGGLESWITCH_UP);
			StartAbort();
			ABORT_IND = true;
			SetThrusterGroupLevel(thg_main, 1.0);
			bAbort = false;
			autopilot=false;
		}
	}
}

int SaturnV::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	if (!KEYMOD_SHIFT(kstate) && !KEYMOD_CONTROL(kstate)) {

		if (stage == ROLLOUT_STAGE) {
			if (key == OAPI_KEY_B && down == true) {
				LaunchVesselBuild();
				return 1;
			}
			if (key == OAPI_KEY_U && down == true) {
				LaunchVesselUnbuild();
				return 1;
			}
		}
	}

	return Saturn::clbkConsumeBufferedKey(key, down, kstate);
}

void SaturnV::LaunchVesselRolloutEnd() {
	// called by crawler after arrival on launch pad

	SetFirstStage();
	SetStage(ONPAD_STAGE);
}

void SaturnV::LaunchVesselBuild() {
	// called by crawler
	
	if (stage == ROLLOUT_STAGE && buildstatus < 5) {
		buildstatus++;
		BuildFirstStage(buildstatus);
	}
}

void SaturnV::LaunchVesselUnbuild() {
	// called by crawler

	if (stage == ROLLOUT_STAGE && buildstatus > 0) {
		buildstatus--;
		BuildFirstStage(buildstatus);
	}
}