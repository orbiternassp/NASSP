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
  *	Revision 1.104  2007/09/04 13:51:56  dseagrav
  *	IGM first stage. It doesn't work yet.
  *	
  *	Revision 1.103  2007/08/11 16:31:24  jasonims
  *	Changed name of BuildFirstStage to ChangeSatVBuildState  to clarify function...
  *	A little more Capcom...
  *	
  *	Revision 1.102  2007/07/17 14:33:03  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.101  2007/06/23 21:20:36  dseagrav
  *	LVDC++ Update: Now with Pre-IGM guidance
  *	
  *	Revision 1.100  2007/06/08 20:08:29  tschachim
  *	Kill apex cover vessel.
  *	
  *	Revision 1.99  2007/06/06 15:02:10  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.98  2007/04/25 18:48:07  tschachim
  *	EMS dV functions.
  *	
  *	Revision 1.97  2007/02/19 16:24:43  tschachim
  *	VC6 MCC fixes.
  *	
  *	Revision 1.96  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.95  2007/02/06 18:30:16  tschachim
  *	Bugfixes docking probe, CSM/LM separation. The ASTP stuff still needs fixing though.
  *	
  *	Revision 1.94  2007/01/22 15:48:14  tschachim
  *	SPS Thrust Vector Control, RHC power supply, THC clockwise switch, bugfixes.
  *	
  *	Revision 1.93  2007/01/11 23:08:21  chode99
  *	New CoeffFunc lift model for CM includes Mach dependence
  *	
  *	Revision 1.92  2006/11/30 14:16:12  tschachim
  *	Bugfixes abort modes.
  *	
  *	Revision 1.91  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.90  2006/08/11 19:34:47  movieman523
  *	Added code to take the docking probe with the LES on a post-abort jettison.
  *	
  *	Revision 1.89  2006/07/28 02:06:57  movieman523
  *	Now need to hard dock to get the connectors connected.
  *	
  *	Revision 1.88  2006/06/23 11:56:48  tschachim
  *	New Project Apollo MFD for TLI burn control.
  *	
  *	Revision 1.87  2006/06/17 18:15:27  tschachim
  *	Bugfix clbkConsumeBufferedKey.
  *	
  *	Revision 1.86  2006/06/11 14:45:36  movieman523
  *	Quick fix for Apollo 4. Will need more work in the future.
  *	
  *	Revision 1.85  2006/06/10 23:27:40  movieman523
  *	Updated abort code.
  *	
  *	Revision 1.84  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.83  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.82  2006/05/27 11:50:04  movieman523
  *	Improved INT20 support, and made LET jettison work any time during launch on Saturn V.
  *	
  *	Revision 1.81  2006/05/27 00:54:28  movieman523
  *	Simplified Saturn V mesh code a lot, and added beginnings ot INT-20.
  *	
  *	Revision 1.80  2006/05/26 22:01:50  movieman523
  *	Revised stage handling some. Removed two of the three second-stage functions and split out the mesh and engine code.
  *	
  *	Revision 1.79  2006/05/19 13:46:56  tschachim
  *	Smoother S-IC staging.
  *	
  *	Revision 1.78  2006/05/17 02:11:51  movieman523
  *	Removed BODGE_FACTOR.
  *	
  *	Revision 1.77  2006/04/25 13:48:02  tschachim
  *	New first stage exhaust.
  *	
  *	Revision 1.76  2006/04/06 19:32:47  movieman523
  *	More Apollo 13 support.
  *	
  *	Revision 1.75  2006/04/06 00:54:46  movieman523
  *	Fixed bug in saving Apollo 13 state and added blowing off of panel 4.
  *	
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
#include "OrbiterSoundSDK35.h"
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
#include "lvimu.h"

#include "saturn.h"
#include "saturnv.h"
#include "s1c.h"
#include "tracer.h"

#include "CollisionSDK/CollisionSDK.h"

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
	Scorrec = false;
	Resetjet = false;
	Burned = false;

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

	SM_FuelMass = SPS_DEFAULT_PROPELLANT;
	SM_EmptyMass = 4100;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
												/// \todo The Apollo 15-17 SM was heavier, about 5500 kg
	CM_EmptyMass = 5430;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
	CM_FuelMass =  CM_RCS_FUEL_PER_TANK * 2.;	// The CM has 2 tanks

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

	lvdc_init();
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
		{-180*RAD,-160*RAD,-150*RAD,-120*RAD,-90*RAD,0*RAD,90*RAD,120*RAD,150*RAD,160*RAD,180*RAD};
	static const double Mach[17] = {0.0,0.7,0.9,1.1,1.2,1.35,1.65,2.0,3.0,5.0,8.0,10.5,13.5,18.2,21.5,31.0,50.0};
	static const double LFactor[17] = {0.3,0.392,0.466,0.607,0.641,0.488,0.446,0.435,0.416,0.415,0.405,0.400,0.385,0.385,0.375,0.35,0.33};
	static const double DFactor[17] = {0.9,0.944,0.991,1.068,1.044,1.270,1.28,1.267,1.213,1.134,1.15,1.158,1.8,1.8,1.193,1.224,1.25};
	static const double CL[nlift]  = {0.0,-0.9,-1.1,-0.5,0.0,0.0,0.0,0.5,1.1,0.9,0.0};
	static const double CM[nlift]  = {0.0,0.004,0.006,0.012,0.015,0.0,-0.015,-0.012,-0.006,-0.004,0.};
	static const double CD[nlift]  = {1.143,1.0,1.0,0.8,0.8,0.8,0.8,0.8,1.0,1.0,1.143};
	int j;
	factor = 0.0;
	dfact = 0.9;
	lfact = 1.0;
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

	if (ratio > 5.4 && ratio < 5.6) {	// 5.5
		thrust = 1028303.;
		isp = 4165.;
	
	} else if (ratio > 4.2 && ratio < 4.4) {	// 4.3
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

	if (ratio > 4.8 && ratio < 5.0) {			// 4.9
		thrust = 901223.;
		isp = 4202.;
	
	} else if (ratio > 4.4 && ratio < 4.6) {	// 4.5
		thrust = 799000.;
		isp = 4245.;

	} else {
		isp = GetJ2ISP(ratio);
		thrust = THRUST_THIRD_VAC * ThrustAdjust;
	}

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

	// Disable CollisionSDK for the moment
	VSRegVessel(GetHandle());
	VSDisableCollisions(GetHandle());
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

			//
			// Checklist actions
			//

			//
			// EDS auto off
			//
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
			bManualSeparate = false;
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

			SeparateStage (LAUNCH_STAGE_TWO_TWR_JET);
			SetStage(LAUNCH_STAGE_TWO_TWR_JET);
			bManualSeparate = false;
			SIISepState = false;

			//
			// Tower jettison at 36.2 seconds after SIC shutdown if not previously
			// specified.
			//

			if (LESJettisonTime > 999.0)
			{
				LESJettisonTime = MissionTime + 5.7;
			}
		}
		break;
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
				SeparateStage (LAUNCH_STAGE_SIVB);
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
		SeparateStage (LAUNCH_STAGE_SIVB);
		ClearEngineIndicators();
		NextMissionEventTime = MissionTime;
		SetStage(LAUNCH_STAGE_SIVB);
	}

	if (bAbort) {
		SeparateStage (LAUNCH_STAGE_SIVB);
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
		SivbLmSepSwitch.SetState(false); // temporary bugfix.
		/// \todo Get rid of bManualUnDock, use pyros, connect this switch and fix REALSIM 0 "S" key press
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

		if (ApolloExploded && ph_o2_vent) {
			TankQuantities t;
			GetTankQuantities(t);

			SetThrusterLevel(th_o2_vent, t.O2Tank1Quantity + 0.1);
			SetO2TankQuantities(GetPropellantMass(ph_o2_vent) / 2.0);
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
				NextMissionEventTime = CSMBurnStart - 300.0;
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
				SPSEngine.EnforceBurn(true);
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
				SPSEngine.EnforceBurn(true);

				NextMissionEventTime = MissionTime + 0.25;

				//
				// Burn until the orbit is about right or we're out of fuel.
				//

				if ((ap >= (prad + (CSMApogee * 1000.0))) || (actualFUEL <= 0.1)) {
					StageState++;
					DeactivateNavmode(NAVMODE_PROGRADE);
					DeactivateCSMRCS();
					SPSEngine.EnforceBurn(false);
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

				SPSEngine.EnforceBurn(true);
				ActivateNavmode(NAVMODE_RETROGRADE);

				NextMissionEventTime = MissionTime + 0.25;

				if ((pe <= (prad + (CSMPerigee * 1000.0))) || (actualFUEL <= 0.1)) {
					StageState++;
					ActivateNavmode(NAVMODE_PROGRADE);
					SPSEngine.EnforceBurn(false);
					CSMBurn = false;
					NextMissionEventTime = MissionTime + 500.0;
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
//				SPSEngine.EnforceBurn(true);
				ActivateNavmode(NAVMODE_PROGRADE);
				NextMissionEventTime = CSMAccelEnd;
				StageState++;
			}
			break;

		case 13:
			if (MissionTime >= NextMissionEventTime) {
				ActivateNavmode(NAVMODE_PROGRADE);
				SPSEngine.EnforceBurn(false);
				CSMAccelSet = false;
				NextMissionEventTime = MissionTime + 200.0;
				StageState++;
			}
			break;

		case 14:
			if (MissionTime >= NextMissionEventTime) {
				ActivateNavmode(NAVMODE_KILLROT);
				NextMissionEventTime = MissionTime + 50.0;
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
				DockStage(dockstate);
			}
			bManualUnDock = false;
		}
		else if (GetDockStatus(GetDockHandle(0)) == hLMV) {
			// Final LM separation
			if (dockstate == 3 && ProbeJetison) {
				// Undock
				Undock(0);
				// Auto translation manoever
				SetAttitudeLinLevel(2,-1);
				release_time = simt;
				Resetjet = true;
				//Time to hear the Stage separation
				SMJetS.play(NOLOOP);
				// Disable docking probe because it's jettisoned 
				dockingprobe.SetEnabled(false);
				HasProbe = false;
				SetDockingProbeMesh();

				dockstate=5;
				ProbeJetison = false;
			}
			// Normal LM undocking
			else {
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
			SeparateStage(CM_STAGE);
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
	
	habort = oapiGetVesselByName("Saturn_Abort");
}

// Orbiter calls here via callback prior to every timestep.
// This function must call GenericTimestep() to operate the CSM.

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

	// DS20070205 LVDC++
	if(use_lvdc){
		// For other stages
		if(stage < CSM_LEM_STAGE){
			lvdc_timestep(simt,simdt);
			return;
		}
	}
	// AFTER LVDC++ WORKS SATISFACTORILY EVERYTHING BELOW THIS LINE
	// SHOULD BE SAFE TO DELETE. DO NOT ADD ANY LVDC++ CODE BELOW THIS LINE.

	if (bAbort && LESAttached)
	{
		if (stage < LAUNCH_STAGE_ONE) 
		{
			//
			// No abort before launch.
			//
			// \todo In reality, we could abort from the pad, at least from five minutes
			// before liftoff.
			//
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
	case LAUNCH_STAGE_TWO_TWR_JET:
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
			SetSIVbCMixtureRatio(4.9);
			
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
				SeparateStage (CM_STAGE);
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
			THRUST_FIRST_VAC = 7835000; // 7653000; /// \todo Temporary fix, otherwise the autopilot is not working properly, we should replace this when we figured out all parameters (masses, fuel masses etc.) of these vehicles
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
			//ShiftCentreOfMass (_V(0,0,STG0O));	// Seems to be useless...
		}

/*		if (GetEngineLevel(ENGINE_MAIN)>=0.5){		// Seems to be useless...
			SetMaxThrust (ENGINE_ATTITUDE, 8e5);
		}
*/
		break;

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
	case LAUNCH_STAGE_TWO_TWR_JET:
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

	case CSM_LEM_STAGE:
		SetCSMStage();

		if (EVA_IP){
			SetupEVA();
		}
		break;

	default:
		SetGenericStageState();
		break;
	}

	//
	// To be perfectly honest, I'm not 100% sure what
	// dockstate does anymore.
	//

	// Seems to be useless...
	/* switch (dockstate) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			/// \todo SetCSM2Stage is buggy
			// SetCSM2Stage ();
			break;
	} */
	
	GenericLoadStateSetup();

	if (stage < LAUNCH_STAGE_SIVB) {
		if (Crewed) {
			soundlib.LoadMissionSound(SPUShiftS, PUSHIFT_SOUND, PUSHIFT_SOUND);
		}
	}

	//
	// Enable or disable RCS.
	//
	CheckRCSState();
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
	case LAUNCH_STAGE_TWO_TWR_JET:
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

	case CSM_ABORT_STAGE:
		SetAbortStage();
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
	case LAUNCH_STAGE_TWO_TWR_JET:
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
		SeparateStage (CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
		if (bAbort) {
			/// \todo SPS abort handling
			StartAbort();
			ABORT_IND = true;
			bAbort = false;
			autopilot=false;
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

// ********** LVDC++ **********

// DS20070205 LVDC++ SETUP
void SaturnV::lvdc_init(){
	lvimu.Init();					// Initialize IMU
	lvrg.Init(this);				// LV Rate Gyro Package
	lvimu.SetVessel(this);			// set vessel pointer
	LVDC_Timebase = -1;				// Start up halted in pre-launch pre-GRR loop
	LVDC_TB_ETime = 0;
	LVDC_GP_PC = 0;
	// INTERNAL (NON-REAL-LVDC) FLAGS
	LVDC_EI_On = false;
	S1C_Sep_Time = 0;
	// REAL LVDC FLAGS
	LVDC_GRR = false;
	S1C_Engine_Out = false;
	// Flags in the pad-load.
	Aziumuth_Inclination_Mode = true;
	Azimuth_DscNodAngle_Mode = true;
	Direct_Ascent = false; 
	CountPIPA = false;
}
	
// DS20070205 LVDC++ EXECUTION
void SaturnV::lvdc_timestep(double simt, double simdt) {
	/* **** LVDC GUIDANCE AND NAVIGATION PROGRAM **** */	
	lvimu.Timestep(simt);								// Give a timestep to the LV IMU
	lvrg.Timestep(simdt);								// and RG
	CurrentAttitude = lvimu.GetTotalAttitude();			// Get current attitude	
	AttRate = lvrg.GetRates();							// Get rates
	
	// If enabled, pick up the IMU data and perform powered-flight navigation.
	if(CountPIPA){
		VECTOR3 Accel;
		// Add detected acceleration
		Accel.x = (lvimu.CDURegisters[LVRegPIPAX] * 0.0585);
		Accel.y = (lvimu.CDURegisters[LVRegPIPAY] * 0.0585);
		Accel.z = (lvimu.CDURegisters[LVRegPIPAZ] * 0.0585);
		
		// Gravitation by new method
		// Get weight vector, it is in newtons (force)
		GetWeightVector(WV);
		// Divide by mass to get acceleration
		WV.x /= GetMass(); WV.y /= GetMass(); WV.z /= GetMass();
		// Transform to platform orientation
		WV = mul(lvimu.getOrbiterLocalToNavigationBaseTransformation(),WV);
		// Now cancel our attitude change
		WV = mul(lvimu.getRotationMatrixX(CurrentAttitude.x), WV);
		WV = mul(lvimu.getRotationMatrixY(CurrentAttitude.y), WV);
  		WV = mul(lvimu.getRotationMatrixZ(CurrentAttitude.z), WV);

		// Remove gravity acceleration from IMU accel
		Accel.x += WV.x*simdt;
		Accel.y += WV.y*simdt;
		Accel.z += WV.z*simdt;

		// "Instantaneous" Velocity -- I think this is +X...
		{
			VECTOR3 Vtemp; Vtemp = Accel;
			Vtemp = mul(lvimu.getRotationMatrixX(-CurrentAttitude.x),Vtemp);
			Vtemp = mul(lvimu.getRotationMatrixY(-CurrentAttitude.y),Vtemp);
			Vtemp = mul(lvimu.getRotationMatrixZ(-CurrentAttitude.z),Vtemp);
			V += Vtemp.x;
		}

		// Form up gravity variables
		ddot_X_g = WV.x;
		ddot_Y_g = WV.y;
		ddot_Z_g = WV.z;

		// Update IMU velocity
		Velocity[0] += Accel.x;
		Velocity[1] += Accel.y;
		Velocity[2] += Accel.z;

		// Update earth-relative velocity
		DotS.x += Accel.x; 
		DotS.y += Accel.y; 
		DotS.z += Accel.z; 		

		// Position launch-pad-relative
		Position[0] += Velocity[0]*simdt;
		Position[1] += Velocity[1]*simdt;
		Position[2] += Velocity[2]*simdt;
				
		// Position earth-centered
		PosS.x += DotS.x*simdt;
		PosS.y += DotS.y*simdt;
		PosS.z += DotS.z*simdt;
	}
	// Update timebase ET
	LVDC_TB_ETime += simdt;

	// Note that GenericTimestep will update MissionTime.

	/* **** LVDC GUIDANCE PROGRAM **** */
	switch(LVDC_GP_PC){
		case 0: // LOOP WAITING FOR PTL
			// Lock time accel to 100x
			if (oapiGetTimeAcceleration() > 100){ oapiSetTimeAcceleration(100); } 

			// Greater than 8.9 seconds out ensure thrust stays at zero.
			if (GetEngineLevel(ENGINE_MAIN) > 0 && MissionTime <= (-8.9)) {
				SetThrusterGroupLevel(thg_main, 0);
				contrailLevel = 0;
			}

			// Prelaunch tank venting between -3:00h and engine ignition
			// No clue if the venting start time is correct
			if (MissionTime < -10800){
				DeactivatePrelaunchVenting();
			}else{
				ActivatePrelaunchVenting();
			}

			// BEFORE PTL COMMAND (T-00:20:00) STOPS HERE
			if (MissionTime < -1200){
				sprintf(oapiDebugString(),"LVDC: T %f | AWAITING PTL INTERRUPT",MissionTime);
				lvimu.ZeroIMUCDUFlag = true;					// Zero IMU CDUs
				break;
			}
			// Otherwise fall into next step
			LVDC_GP_PC = 1;

		case 1: // "PREPARE TO LAUNCH" INTERRUPT RECIEVED FROM GCC
			{
				lvimu.ZeroIMUCDUFlag = false;					// Release IMU CDUs
				double TO_HDG = agc.GetDesiredAzimuth();		// Get aziumuth in degrees
				Azimuth = TO_HDG;								// Store azimuth
				TO_HDG = TO_HDG - 90;							// Correct for pad orientation (Trust me, it's correct)
				lvimu.DriveGimbals(TO_HDG*RAD,0,0);				// Now bring to alignment 
			}
			// The IMU is STILL CAGED until GRR is released.
			LVDC_GP_PC = 2;

		case 2: // WAIT FOR GRR
			// Engine lights on at T-00:04:10
			if (MissionTime >= -250 && LVDC_EI_On == false) { LVDC_EI_On = true; }

			// Between PTL signal and GRR, we monitor the IMU for any failure signals and do vehicle self-tests.
			// At GRR we transfer control to the flight program and start TB0.

			// BEFORE GRR (T-00:00:17) STOPS HERE
			if (MissionTime < -17){
				sprintf(oapiDebugString(),"LVDC: T %f | IMU XYZ %d %d %d PIPA %d %d %d | AWAITING GRR",MissionTime,
					lvimu.CDURegisters[LVRegCDUX],lvimu.CDURegisters[LVRegCDUY],lvimu.CDURegisters[LVRegCDUZ],
					lvimu.CDURegisters[LVRegPIPAX],lvimu.CDURegisters[LVRegPIPAY],lvimu.CDURegisters[LVRegPIPAZ]);
				break;
			}			
			LVDC_GP_PC = 3; // Fall into next

		case 3: // POST-GRR INITIALIZATION
			// DEFAULT NON-PAD-LOAD AREA
			R_T = 0; f = 0;	V_T = 0; ups_T = 0; G_T = 0;

			/* **** PAD-LOADED VARIABLE INITIALIZATION (TEMPORARY) **** */
			// PRE-IGM PITCH POLYNOMIAL
			Fx[0][0] = 3.19840;			Fx[0][1] = -10.9607;		Fx[0][2] = 78.7826;			Fx[0][3] = 69.9191;
			Fx[1][0] = -0.544236;		Fx[1][1] = 0.946620;		Fx[1][2] = -2.83749;		Fx[1][3] = -2.007490;
			Fx[2][0] = 0.0351605;		Fx[2][1] = -0.0294206;		Fx[2][2] = 0.0289710;		Fx[2][3] = 0.0105367;
			Fx[3][0] = -0.00116379;		Fx[3][1] = 0.000207717;		Fx[3][2] = -0.000178363;	Fx[3][3] = -0.0000233163;
			Fx[4][0] = 0.0000113886;	Fx[4][1] = -0.000000439036;	Fx[4][2] = 0.000000463029;	Fx[4][3] = 0.0000000136702;

			// Inclination from azimuth polynomial
			fx[0] = 32.5597; fx[1] = -16.2615; fx[2] = 15.6919; fx[3] = -6.7370;
			fx[4] = 26.9593; fx[5] = -28.9526; fx[6] = 9.8794;

			// Descending Node Angle from azimuth polynomial
			gx[0] = 123.2094; gx[1] = -56.5034; gx[2] = -21.6675; gx[3] = -14.5228;
			gx[4] = 47.532;   gx[5] = -22.5502; gx[6] = 1.8946;

			// FAILURE FREEZE COEFFICIENTS
			B_11 = -0.62;   B_12 = 40.9;
			B_21 = -0.3611; B_22 = 29.25;

			// IGM
			// Raising tau2 will crash the program.
			// tau3 was 665,86. Raising tau3 doesn't prevent T_T from negating early
			tau2 = 309.23; tau3 = 754; eps_1 = 0; ROV = 1.5;

			// MISC. TIMES
			T_ar = 153; dT_F = 0; T_LET = 40.671;
			t_1  = 13; t_2 = 25; t_3 = 36; t_4 = 45; t_5 = 81; t_6 = 0; T_c = 4.718;
			// T_1 = 237.796;
			T_1 = 337.796; T_2 = 99.886; Tt_3 = 120.565;
			T_S1 = 35; T_S2 = 80; T_S3 = 115; eps_2 = 10; 
			eps_3 = 10000; eps_4 = 8; T_1c = 342.4; 
			t_B1 = 50; t_B3 = 0; 
			t = 2; Tt_T = 462.965;

			// MISC
			mu = 398603200000000; // Checked correct
			cos_phi_L = 0.877916; sin_phi_L = 0.478814; ROT = false; V_TC = 300;
			phi_L = 28.626536; phi_M = -80.620811; LS_ALT = 6373340; LV_ALT = 80.51; UP = 0; BOOST = true;

			// EXHAUST VELOCITIES AND FLOW RATES
			V_ex1 = 4169.23; V_ex2 = 4204.26; V_ex3 = 4170.57;			// Apollo 9
			dotM_1 = 1243.77; dotM_2 = 1009.04; dotM_3 = 248.882;

			// TABLE15			
//			TABLE15_f = 0.07725203;							// Determined by spreadsheet based on AS-507
//			TABLE15_e = 0.974660815;						// same
//			TABLE15_C_3 = -1541156;							// same			

			TABLE15_f = 0.07725203;							// AS-507
			TABLE15_e = 0.9745275;							// same
			TABLE15_C_3 = -1541156;							// same			

			// TABLE25

			// GUIDANCE REF RELEASE - GO TO TB0
			lvimu.SetCaged(false);							// Release IMU
			LVDC_GRR = true;								// Mark event
			oapiSetTimeAcceleration (1);					// Set time acceleration to 1
			SetThrusterGroupLevel(thg_main, 0);				// Ensure off
			{	int i;
				for (i = 0; i < 5; i++) {						// Reconnect fuel to S1C engines
					SetThrusterResource(th_main[i], ph_1st);
				}
			}
			CreateStageOne();								// Create hidden stage one, for later use in staging
			LVDC_Timebase = 0;								// Start TB0
			LVDC_TB_ETime = 0;
			LVDC_GP_PC = 10;								// FALL INTO TB0

		case 10: // AWAIT LAUNCH
			// This is timebase 0. At this point, the LVDC is running the flight program.
			// All diagnostics are completed.
			
			// Perform Ground Targeting Calculations
			// Time into launch window = launch time from midnight - reference time of launch from midnight
			// azimuth = coeff. of azimuth polynomial * time into launch window

			// We already have an azimuth from the AGC, we'll use that for now.
			Azimuth = agc.GetDesiredAzimuth();

			// COMPUTE Azo and Azs
			// ** FIXME ** I NEVER DID FIND THE PROPER VALUES FOR THESE
			// I HACKED THESE NUMBERS UP USING AN EXCEL SPREADSHEET TO GENERATE CLOSE-TO-CORRECT VALUES
			// I HAVE NO IDEA WHAT THEY ARE SUPPOSED TO BE
			Azo = (int)Azimuth; // Round off
			Azs = Azimuth;		// Obviously wrong but is close to proper

			// Odd - AS-507 has AZO/AZS...

			if(Aziumuth_Inclination_Mode = true){
				// CALCULATE INCLINATION FROM AZIMUTH
				Inclination = 0;
				int x=0;
				while(x < 7){
					Inclination += fx[x] * pow((Azimuth-Azo)/Azs,x);
					x++;
				}
			}else{
				// CALCULATE INCLINATION FROM TIME INTO LAUNCH WINDOW
				// inclination = coeff. for inclination-from-time polynomial * Time into launch window
			}
			if(Azimuth_DscNodAngle_Mode == true){
				// CALCULATE DESCENDING NODAL ANGLE FROM AZIMUTH
				DescNodeAngle = 0;
				int x=0;
				while(x < 7){
					DescNodeAngle += gx[x] * pow((Azimuth-Azo)/Azs,x);
					x++;
				}
			}else{
				// CALCULATE DESCNEING NODAL ANGLE FROM TIME INTO LAUNCH WINDOW
				// DNA = coeff. for DNA-from-time polynomial * Time into launch window
			}
			f = TABLE15_f;
			e = TABLE15_e;
			C_3 = TABLE15_C_3;

			if(Direct_Ascent){
				// angle from perigee vector to DNA vector = TABLE25 (time into launch window)
				// terminal guidance freeze time = 0
				sprintf(oapiDebugString(),"LVDC: DIRECT-ASCENT"); LVDC_GP_PC = 30; break; // STOP
			}
			p = (mu/C_3)*(pow(e,2)-1);
			// p is the semi-latus rectum of the desired terminal ellipse.
			//sprintf(oapiDebugString(),"LVDC: p %f mu %f C_3 %f e %f",
			//	p,mu,C_3,e); LVDC_GP_PC = 30; break; // STOP

			K_5 = pow((mu/p),0.5);

			// R_T is desired terminal radius. It should be about 6,553,340 (180km alt)
			// This gets garbaged because p is garbage.
			R_T = p/(1+(e * cos(f)));
//			sprintf(oapiDebugString(),"LVDC: R_T %f p %f e %f f %f",
//				R_T,p,e,f); LVDC_GP_PC = 30; break; // STOP

//			R_T = 6553340; // OVERRIDE FOR NOW

			V_T = K_5*pow((1+((2*e)*cos(f))+pow(e,2)),0.5);
//			sprintf(oapiDebugString(),"LVDC: V_T %f K_5 %f e %f f %f M-RES %f",
//				V_T,K_5,e,f,pow((1+((2*e)*cos(f))+pow(e,2)),0.5)); LVDC_GP_PC = 30; break; // STOP

			// For some reason the generated terminal velocity is worthless.
			// Let's sub in this and see if it helps.
//			V_T = 7790;

			ups_T = atan((e*sin(f))/(1+(e*cos(f))));

			// G MATRIX CALCULATION
			MX_A.m11 = cos_phi_L;  MX_A.m12 = sin_phi_L*sin(Azimuth); MX_A.m13 = -(sin_phi_L*cos(Azimuth));
			MX_A.m21 = -sin_phi_L; MX_A.m22 = cos_phi_L*sin(Azimuth); MX_A.m23 = -(cos_phi_L*cos(Azimuth));
			MX_A.m31 = 0;          MX_A.m32 = cos(Azimuth);           MX_A.m33 = sin(Azimuth);

			MX_B.m11 = cos(DescNodeAngle); MX_B.m12 = 0; MX_B.m13 = sin(DescNodeAngle);
			MX_B.m21 = sin(DescNodeAngle)*sin(Inclination); MX_B.m22 = cos(Inclination); 
			MX_B.m23 = -(cos(DescNodeAngle)*sin(Inclination));
			MX_B.m31 = -(sin(DescNodeAngle)*cos(Inclination)); MX_B.m32 = sin(Inclination);
			MX_B.m33 = cos(DescNodeAngle)*cos(Inclination);

			MX_G = mul(MX_B,MX_A); // Matrix Multiply

			LVDC_GP_PC = 11;

		case 11: // MORE TB0
			// At 10 seconds, play the countdown sound.
			if (MissionTime >= -10.3) { // Was -10.9
				if (!UseATC && Scount.isValid()) {
					Scount.play();
					Scount.done();
				}
			}

			// Shut down venting at T - 9
			if(MissionTime > -9 && prelaunchvent[0] != NULL) { DeactivatePrelaunchVenting(); }

			// Starting at 4.9 seconds, start to throttle up.
			{
				double thrst=0;				
				if (MissionTime >= -4.9) {
					thrst = (0.9 / 2.9) * (MissionTime + 4.9);
				}else{
					if (MissionTime > -2.0) {
						thrst = 0.9 + (0.05 * (MissionTime + 2.0));
					}
				}
				if(thrst > 0){					
					SetThrusterGroupLevel(thg_main, thrst);
					AddForce(_V(0, 0, -10. * THRUST_FIRST_VAC), _V(0, 0, 0)); // Maintain hold-down lock
					contrailLevel = thrst;				
				}
			}

			if(MissionTime < 0){								
				// mu is suspect
				sprintf(oapiDebugString(),"LVDC: T %f | TB0 + %f | p %f %f %f %f",
					MissionTime,LVDC_TB_ETime,
					p,mu,C_3,e);
				break;
			}
			LVDC_GP_PC = 15;

		case 15: // LIFTOFF TIME
			IMUGuardedCageSwitch.SwitchTo(TOGGLESWITCH_DOWN);		// Uncage CM IMU
			SetLiftoffLight();										// And light liftoff lamp
			SetStage(LAUNCH_STAGE_ONE);								// Switch to stage one
			// Start mission and event timers
			MissionTimerDisplay.Reset();
			MissionTimerDisplay.SetEnabled(true);
			EventTimerDisplay.Reset();
			EventTimerDisplay.SetEnabled(true);
			EventTimerDisplay.SetRunning(true);
			agc.SetInputChannelBit(030, 5, true);					// Inform AGC of liftoff
			SetThrusterGroupLevel(thg_main, 1.0);					// Set full thrust, just in case
			contrailLevel = 1.0;
			if (LaunchS.isValid() && !LaunchS.isPlaying())			// And play launch sound
			{
				LaunchS.play(NOLOOP,255);
				LaunchS.done();
			}

			// COMPUTE INITIAL POSITIONS
			Velocity[0] = 0; Velocity[1] = 0; Velocity[2] = 0; V = 0;
			// PAD RELATIVE
			Position[0] = 0; Position[1] = 0; Position[2] = 0;
			// EARTH-CENTERED PLUMBLINE
			
			// FIXME: DETERMINE WHAT THESE NUMBERS ARE SOURCED FROM
			PosS.x = LS_ALT+LV_ALT;	// Distance from Earth's center
			PosS.y = 19228;			// ?? From Apollo 10
			PosS.z = 1051;			// ?? From Apollo 10
			DotS.x = 0;
			DotS.y = 125.8;
			DotS.z = 388.7;

			// Enable PIPA storage
			CountPIPA = true;
			// Store current attitude as commands
			CommandedAttitude.x = CurrentAttitude.x*DEG;
			CommandedAttitude.y = CurrentAttitude.y*DEG;
			CommandedAttitude.z = CurrentAttitude.z*DEG;
			// Fall into TB1
			LVDC_Timebase = 1;
			LVDC_TB_ETime = 0;
			LVDC_GP_PC = 20; // GO TO PRE-IGM

		case 20: // PRE-IGM PROGRAM
			if(S1C_Engine_Out && T_EO1 == 0){	// If S1C Engine Out and not flagged
				T_EO1 = 1;
				T_FAIL = MissionTime;
			}
			if(Position[0] > 137 || MissionTime > t_1){
				// We're clear of the tower
				if(MissionTime >= t_2 && T_EO1 > 0){
					// Engine has failed, recalculate freeze time
					if(T_FAIL <= t_2){ dT_F = t_3; }
					if(t_2 < MissionTime && MissionTime <= t_4){ dT_F = B_11 * T_FAIL + B_12; }
					if(t_4 < MissionTime && MissionTime <= t_5){ dT_F = B_21 * T_FAIL + B_22; }
					if(t_5 < MissionTime){ dT_F = 0; }
					t_6 = MissionTime + dT_F;
					T_ar = T_ar + (0.25 * (T_ar - T_FAIL));
				}
				// Determine Pitch Command
				if(MissionTime >= t_6){
					// Continue
					if(MissionTime > T_ar){
						// NORMAL PITCH FREEZE
					}else{
						// PITCH FROM POLYNOMIAL
						int x=0,y;						
						double Tc = MissionTime - dT_F;						
						CommandedAttitude.y = 0;
						if(Tc < T_S1){               y = 0; }
						if(T_S1 <= Tc && Tc < T_S2){ y = 1; }
						if(T_S2 <= Tc && Tc < T_S3){ y = 2; }
						if(T_S3 <= Tc){              y = 3; }
						while(x < 5){
							CommandedAttitude.y += (Fx[x][y] * ((double)pow(Tc,x)));
							x++;
						}
						// CommandedAttitude.y *= RAD;
					}
				}else{
					// >= T6 PITCH FREEZE
				}
				// Now Roll
				if(CommandedAttitude.x != 0){ CommandedAttitude.x = 0; }
				// And Yaw
				if(CommandedAttitude.z != 0){ CommandedAttitude.z = 0; }
			}else{
				// Not clear of the tower.
				// S1C YAW MANEUVER
				if(MissionTime > 1 && MissionTime < 8.75){
					CommandedAttitude.z = 5;
				}else{
					CommandedAttitude.z = 0;
				}
				// MAINTAIN CURRENT ROLL AND PITCH COMMANDS				
			}
			
			/*
			// ENGINE FAIL TEST:
			if(MissionTime > 22.5 && S1C_Engine_Out == false){
				SetThrusterResource(th_main[1], NULL); // Should stop the engine
				S1C_Engine_Out = true;
			}
			*/

			// S1C CECO TRIGGER:
			// I have two conflicting leads as to the CECO trigger.
			// One says it happens at 4G acceleration and the other says it happens by a timer at T+135.5
			if(LVDC_Timebase == 1 && MissionTime > 135.5){ // this->GetAccelG() > 4){
				SetThrusterResource(th_main[4], NULL); // Should stop the engine
				// Clear liftoff light now - Apollo 15 checklist
				ClearLiftoffLight();
				// Begin timebase 2
				LVDC_Timebase = 2;
				LVDC_TB_ETime = 0;
			}

			// S1C OECO TRIGGER
			// Because of potential failures above changing the engine burn rates, this can't be reliably done on
			// a timer anymore.
			if (stage == LAUNCH_STAGE_ONE && LVDC_Timebase == 2 && GetFuelMass() <= 0.001){
				bManualSeparate = false;
				// Move hidden S1C
				if (hstg1) {
					VESSELSTATUS vs;
					GetStatus(vs);
					S1C *stage1 = (S1C *) oapiGetVesselInterface(hstg1);
					stage1->DefSetState(&vs);
				}				
				// Set timer
				S1C_Sep_Time = MissionTime;
				// Engine Shutdown
				int i;
				for (i = 0; i < 5; i++){
					SetThrusterResource(th_main[i], NULL);
				}
				// Begin timebase 3
				LVDC_Timebase = 3;
				LVDC_TB_ETime = 0;
			}
			// S1C SEPARATION TRIGGER
			if(stage == LAUNCH_STAGE_ONE && S1C_Sep_Time != 0 && MissionTime >= (S1C_Sep_Time+0.7)){
				// Drop old stage
				SeparationS.play(NOLOOP, 245);
				SeparateStage(LAUNCH_STAGE_TWO);
				SetStage(LAUNCH_STAGE_TWO);
				// Fire S2 ullage
				if (SII_UllageNum) {
					SetThrusterGroupLevel(thg_ull, 1.0);
					SepS.play(LOOP, 130);
				}
				ActivateStagingVent();
			}
			// S2 ENGINE STARTUP
			if(stage == LAUNCH_STAGE_TWO && S1C_Sep_Time != 0 && MissionTime >= (S1C_Sep_Time+1.4) && S2_Startup == false){
				S2_Startup = true;
				SIISepState = true;
				SetSIICMixtureRatio(5.5);
				DeactivateStagingVent();
				SetThrusterGroupLevel(thg_main, 0.9);
			}

			if(stage == LAUNCH_STAGE_TWO && S1C_Sep_Time != 0 && MissionTime >= (S1C_Sep_Time+1.7) && S2_Startup == true){
				SetThrusterGroupLevel(thg_main, 1); // Full power
				if (SII_UllageNum)
					SetThrusterGroupLevel(thg_ull,0.0);
				SepS.stop();
				S1C_Sep_Time = 0; // All done
			}

			// Drop Interstage Ring
			if(stage == LAUNCH_STAGE_TWO && S1C_Sep_Time == 0 && LVDC_Timebase == 3 && LVDC_TB_ETime > 30.7
				&& SIISepState == true)
			{
				SeparateStage (LAUNCH_STAGE_TWO_TWR_JET);
				SetStage(LAUNCH_STAGE_TWO_TWR_JET);
				bManualSeparate = false;
				SIISepState = false;
			}

			// And jettison LET
			if(stage == LAUNCH_STAGE_TWO_TWR_JET && LVDC_Timebase == 3 && LVDC_TB_ETime > T_LET && LESAttached){
				JettisonLET();
			}

			// PRE-IGM TERMINATE CONDITION
			if(stage == LAUNCH_STAGE_TWO_TWR_JET && LVDC_Timebase == 3 && LVDC_TB_ETime > T_LET+5){
				LVDC_GP_PC = 25; // GO TO IGM
				IGMCycle = 0;
				sprintf(oapiDebugString(),"TB%d+%f: ** IGM STARTING **",LVDC_Timebase,LVDC_TB_ETime);
			}
			// -- END OF PRE-IGM --			
			break;

		case 25: // ITERATIVE GUIDANCE MODE
			double TimeStep; // Time since last run
			// STAGE CHANGE AND FLAG SET LOGIC

			// Should happen at 289.62 for Apollo 8
			if(LVDC_Timebase == 3 && LVDC_TB_ETime > 336.3 && stage == LAUNCH_STAGE_TWO && MRS == false){
				// MR Shift
				sprintf(oapiDebugString(),"LVDC: EMR SHIFT"); LVDC_GP_PC = 30; break; // STOP
				SetSIICMixtureRatio (4.3);
				if (Crewed){ SPUShiftS.play(); }
				SPUShiftS.done();
			}

			// IGM ONLY RUNS EVERY TWO SECONDS
			sinceLastIGM += simdt; if(sinceLastIGM < 2){ break; } TimeStep = sinceLastIGM; sinceLastIGM = 0;
			IGMCycle++;				// For debugging

			if(HSL == false){		// If we are not in the high-speed loop
				// IGM STAGE LOGIC
				// No S4B Relight, No S4B Light, No S2 Cutoff, No Engine Out
				// No MRS
				if(T_1 < 0){	// If we're out of first-stage IGM time
					// Do stuff
					sprintf(oapiDebugString(),"LVDC: T_1 LT ZERO: %f @ %d (TB%d+%f)",T_1,IGMCycle,LVDC_Timebase,LVDC_TB_ETime); 
					LVDC_GP_PC = 30; break; // STOP
				}else{															
					int i; F = 0;
					for (i = 0; i <= 4; i++){ F += GetThrusterMax(th_main[i])*GetThrusterLevel(th_main[i]);	}
					tau1 = V_ex1 * (GetMass()/F); 
				}

				// CHI-TILDE LOGIC
				// STAGE INTEGRAL CALCULATIONS				
				Pos4 = mul(MX_G,PosS);
				L_1 = V_ex1 * log(tau1 / (tau1-T_1));
				J_1 = (L_1 * tau1) - (V_ex1 * T_1);
				S_1 = (L_1 * T_1) - J_1;
				Q_1 = (S_1 * tau1) - ((V_ex1 * pow(T_1,2)) / 2);
				P_1 = (J_1 * tau1) - ((V_ex1 * pow(T_1,2)) / 2);
				U_1 = (Q_1 * tau1) - ((V_ex1 * pow(T_1,3)) / 6);
				L_2 = V_ex2 * log(tau2 / (tau2-T_2));
				J_2 = (L_2 * tau2) - (V_ex2 * T_2);
				S_2 = (L_2 * T_2) - J_2;
				Q_2 = (S_2 * tau2) - ((V_ex2 * pow(T_2,2)) / 2);
				P_2 = (J_2 * tau2) - ((V_ex2 * pow(T_2,2)) / 2);
				U_2 = (Q_2 * tau2) - ((V_ex2 * pow(T_2,3)) / 6);
				L_12 = L_1 + L_2;
				J_12 = J_1 + J_2 + (L_2 * T_1);
				S_12 = S_1 - J_2 + (L_12 * (T_2 + T_c));
				Q_12 = Q_1 + Q_2 + (S_2 * T_1) + (J_1 * T_2);
				P_12 = P_1 + P_2 + (T_1 * ((2 * J_2) + (L_2 * T_1)));
				U_12 = U_1 + U_2 + (T_1 * ((2 * Q_2) + (S_2 * T_1))) + (T_2 * P_1);
				Lt_3 = V_ex3 * log(tau3 / (tau3-Tt_3));
				Jt_3 = (Lt_3 * tau3) - (V_ex3 * Tt_3);
				Lt_Y = (L_12 + Lt_3);

				// SELECT RANGE OPTION				
gtupdate:		// Target of jump from further down
				if(Tt_T <= eps_1){
					// RANGE ANGLE 2 (out-of orbit)
					sprintf(oapiDebugString(),"LVDC: RANGE ANGLE 2: %f %f",Tt_T,eps_1); LVDC_GP_PC = 30; break; // STOP
					V = pow(pow(DotS.x,2)+pow(DotS.y,2)+pow(DotS.z,2),0.5);
					R = pow(pow(PosS.x,2)+pow(PosS.y,2)+pow(PosS.z,2),0.5);
					sin_ups = ((PosS.x*DotS.x)+(PosS.y*DotS.y)+(PosS.z*DotS.z))/R*V;
					cos_ups = pow(1-pow(sin_ups,2),0.5);
					dot_phi_1 = (V*cos_ups)/R;
					dot_phi_T = (V_T*cos(ups_T))/R_T;
					phi_T = atan(Pos4.z/Pos4.x)+(((dot_phi_1+dot_phi_T)/2)*Tt_T);
				}else{
					// RANGE ANGLE 1 (into orbit)
					d2 = (V * Tt_T) - Jt_3 + (Lt_Y * Tt_3) - ((ROV / V_ex3) * 
						(((tau1 - T_1) * L_1) + ((tau2 - T_2) * L_2) + ((tau3 - Tt_3) * Lt_3)) *
						(Lt_Y + V - V_T));
					phi_T = atan(Pos4.z/Pos4.x)+(((1/R_T)*(S_12+d2))*cos(ups_T));					
				}
				// FREEZE TERMINAL CONDITIONS TEST
				if(!(Tt_T <= eps_3)){
					// UPDATE TERMINAL CONDITIONS
					f = phi_T + alpha_D;
					R_T = p/(1+(e*cos(f)));

					// Stop here for debugging
					sprintf(oapiDebugString(),"LVDC: R_T %f p %f e %f f %f",
						R_T,p,e,f); LVDC_GP_PC = 30; break; // STOP

					V_T = K_5 * pow(1+((2*e)*cos(f))+pow(e,2),0.5);
					ups_T = atan((e*sin(f))/(1+(e*cos(f))));
					G_T = -(mu*pow(R_T,2));					
				}
				// ROT TEST
				if(ROT){
					// ROTATED TERMINAL CONDITIONS (out-of-orbit)
					sprintf(oapiDebugString(),"LVDC: ROTATED TERMINAL CNDS"); LVDC_GP_PC = 30; break; // STOP
				}else{
					// UNROTATED TERMINAL CONDITIONS (into-orbit)
					xi_T = R_T;					
					dot_zeta_T = V_T * cos(ups_T);
					dot_xi_T = V_T * sin(ups_T);
					ddot_zeta_GT = 0;
					ddot_xi_GT = G_T;
				}
				// ROTATION TO TERMINAL COORDINATES
				MX_phi_T.m11 = cos(phi_T);  MX_phi_T.m12 = 0; MX_phi_T.m13 = sin(phi_T);
				MX_phi_T.m21 = 0;           MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
				MX_phi_T.m31 = -sin(phi_T); MX_phi_T.m32 = 0; MX_phi_T.m33 = cos(phi_T);
				MX_K = mul(MX_phi_T,MX_G);
				PosXEZ = mul(MX_K,PosS);
				DotXEZ = mul(MX_K,DotS);	
				VECTOR3 RTT_T1,RTT_T2;
				RTT_T1.x = ddot_xi_GT; RTT_T1.y = 0;        RTT_T1.z = ddot_zeta_GT;
				RTT_T2.x = ddot_X_g;   RTT_T2.y = ddot_Y_g; RTT_T2.z = ddot_Z_g;
				RTT_T2 = mul(MX_K,RTT_T2);
				RTT_T1 = RTT_T1+RTT_T2;	  
				ddot_xi_G   = 0.5*RTT_T1.x;
				ddot_eta_G  = 0.5*RTT_T1.y;
				ddot_zeta_G = 0.5*RTT_T1.z;

				// ESTIMATED TIME-TO-GO
				dot_dxit   = dot_xi_T - DotXEZ.x - (ddot_xi_G * Tt_T);
				dot_detat  = (-DotXEZ.y) - (ddot_eta_G * Tt_T);
				dot_dzetat = dot_zeta_T - DotXEZ.z - (ddot_zeta_G * Tt_T);
				dL_3 = (((pow(dot_dxit,2)+pow(dot_detat,2)+pow(dot_dzetat,2))/Lt_Y)-Lt_Y)/2;
				dT_3 = (dL_3 * (tau3-Tt_3))/V_ex3;
				T_3 = Tt_3 + dT_3;
				T_T = Tt_T + dT_3;

				// TARGET PARAMETER UPDATE
				if(!(UP > 0)){	
					UP = 1; 
					Tt_3 = T_3;
					Tt_T = T_T;
					if(Tt_T < 0){ sprintf(oapiDebugString(),"TPU: Tt_T %f @ Cycle %d (TB%d+%f)",
						Tt_T,IGMCycle,LVDC_Timebase,LVDC_TB_ETime); LVDC_GP_PC = 30; break;	}
					Lt_3 = Lt_3 + dL_3;
					Lt_Y = Lt_Y + dL_3;
					Jt_3 = Jt_3 + (dL_3*T_3);
					// NOTE: This is perfectly valid. Just because Dijkstra and Wirth think otherwise
					// does not mean it's gospel. I shouldn't have to defend my choice of instructions
					// because a bunch of people read the title of the paper with no context and take
					// it as a direct revelation from God with no further study into the issue.
					goto gtupdate; // Recycle. 
				}

				// tchi_y AND tchi_p CALCULATIONS
				L_3 = Lt_3 + dL_3;
				J_3 = Jt_3 + (dL_3*T_3);
				S_3 = (L_3*T_3)-J_3;
				Q_3 = (S_3*tau3)-((V_ex3*pow(T_3,2))/2);
				P_3 = (J_3*(tau3+(2*T_1c)))-((V_ex3*pow(T_3,2))/2);
				U_3 = (Q_3*(tau3+(2*T_1c)))-((V_ex3*pow(T_3,3))/6);

				// This is where velocity-to-be-gained is generated.

				dot_dxi   = dot_dxit   - (ddot_xi_G   * dT_3);
				dot_deta  = dot_detat  - (ddot_eta_G  * dT_3);
				dot_dzeta = dot_dzetat - (ddot_zeta_G * dT_3);

				// The results really look quite suspect...
//				sprintf(oapiDebugString(),".dxi = %f | .dxit %f ..xiG %f dT_3 %f",
//					dot_dxi,dot_dxit,ddot_xi_G,dT_3);

				L_Y = L_12 + L_3;
				tchi_y = atan(dot_deta/pow(pow(dot_dxi,2)+pow(dot_dzeta,2),0.5));
				tchi_p = atan(dot_dxi/dot_dzeta);				
				UP = -1;

				// *** END OF CHI-TILDE LOGIC ***
				if(Tt_T <= eps_2){
					// Go to the test that we would be testing if HSL was true
					K_1 = 0; K_2 = 0; K_3 = 0; K_4 = 0;
					// See the note above if the presence of this goto bothers you.
					goto hsl;
				}else{
					// *** K_i CALCULATIONS ***
					// YAW STEERING PARAMETERS
					J_Y = J_12 + J_3 + (L_3*T_1c);
					S_Y = S_12 - J_3 + (L_Y*T_3);
					Q_Y = Q_12 + Q_3 + (S_3*T_1c) + ((T_c+T_3)*J_12);
					K_Y = L_Y/J_Y;
					D_Y = S_Y - (K_Y*Q_Y);
					deta = PosXEZ.y + (DotXEZ.y*T_T) + ((ddot_eta_G*pow(T_T,2))/2) + (S_Y*sin(tchi_y));
					K_3 = deta/(D_Y*cos(tchi_y));
					K_4 = K_Y*K_3;
					// PITCH STEERING PARAMETERS
					L_P = L_Y*cos(tchi_y);
					C_2 = cos(tchi_y)+(K_3*sin(tchi_y));
					C_4 = K_4*sin(tchi_y);
					J_P = (J_Y*C_2) - (C_4*(P_12+P_3+(pow(T_1c,2)*L_3)));
					S_P = (S_Y*C_2) - (C_4*Q_Y);
					Q_P = (Q_Y*C_2) - (C_4*(U_12+U_3+(pow(T_1c,2)*S_3)+((T_3+T_c)*P_12)));
					K_P = L_P/J_P;
					D_P = S_P - (K_P*Q_P);
					dxi = PosXEZ.x - xi_T + (DotXEZ.x*T_T) + ((ddot_xi_G*pow(T_T,2))/2) + (S_P*sin(tchi_p));
					K_1 = dxi/(D_P*cos(tchi_p));
					K_2 = K_P*K_1;
				}
			}else{
hsl:			// HIGH-SPEED LOOP ENTRY
				if(Tt_T <= eps_4 && V + V_TC >= V_T){
					// HIGH-SPEED LOOP
					sprintf(oapiDebugString(),"LVDC: HISPEED LOOP: %f %f %f %f %f",
						Tt_T,eps_4,V,V_TC,V_T); LVDC_GP_PC = 30; break; // STOP
				}
			}
			// GUIDANCE TIME UPDATE
			if(BOOST){
				if(S4B_IGN){		T_3 = T_3 - TimeStep; }else{
				if(S2_BURNOUT){		T_c = T_c - TimeStep; }else{
				if(MRS == false){	T_1 = T_1 - TimeStep; }else{
				if(t_B1 <= t_B3){	T_2 = T_2 - TimeStep; }else{
									// Here if t_B1 is bigger.
									T_1 = (((dotM_1*(t_B3-t_B1))-(dotM_2*t_B3))*TimeStep)/(dotM_1*t_B1);
				}}}}
			}else{
				// MRS TEST
				sprintf(oapiDebugString(),"LVDC: MRS TEST"); LVDC_GP_PC = 30; break; // STOP
			}			
			Tt_3 = T_3;			
			T_1c = T_1+T_2+T_c;			
			Tt_T = T_1c+Tt_3;
			if(Tt_T < 0){
				sprintf(oapiDebugString(),"GTU: Tt_T %f T_1c %f Tt_3 %f @ Cycle %d",Tt_T,T_1c,Tt_3,IGMCycle);
				LVDC_GP_PC = 30; break;
			}

			if(GATE){
				// FREEZE CHI
				sprintf(oapiDebugString(),"LVDC: CHI FREEZE"); LVDC_GP_PC = 30; break; // STOP
			}else{
				// IGM STEERING ANGLES
				Xtt_y = tchi_y - K_3 + (K_4 * t);
				Xtt_p = tchi_p - K_1 + (K_2 * t);

				// -- COMPUTE INVERSE OF [K] --
				// Get Determinate
				double det = MX_K.m11 * ((MX_K.m22*MX_K.m33) - (MX_K.m32*MX_K.m23))
						   - MX_K.m12 * ((MX_K.m21*MX_K.m33) - (MX_K.m31*MX_K.m23))
						   + MX_K.m13 * ((MX_K.m21*MX_K.m32) - (MX_K.m31*MX_K.m22));
				// If the determinate is less than 0.0005, this is invalid.
				MATRIX3 MX_Ki; // TEMPORARY: Inverse of [K]
				MX_Ki.m11 =   (MX_K.m22*MX_K.m33) - (MX_K.m23*MX_K.m32)  / det;
				MX_Ki.m12 = -((MX_K.m12*MX_K.m33) - (MX_K.m32*MX_K.m13)) / det;
				MX_Ki.m13 =   (MX_K.m12*MX_K.m23) - (MX_K.m22*MX_K.m13)  / det;
				MX_Ki.m21 = -((MX_K.m21*MX_K.m33) - (MX_K.m23*MX_K.m31)) / det;
				MX_Ki.m22 =   (MX_K.m11*MX_K.m33) - (MX_K.m31*MX_K.m13)  / det;
				MX_Ki.m23 = -((MX_K.m11*MX_K.m23) - (MX_K.m21*MX_K.m13)) / det;
				MX_Ki.m31 =   (MX_K.m21*MX_K.m32) - (MX_K.m31*MX_K.m22)  / det;
				MX_Ki.m32 = -((MX_K.m11*MX_K.m32) - (MX_K.m31*MX_K.m12)) / det;
				MX_Ki.m33 =   (MX_K.m11*MX_K.m22) - (MX_K.m12*MX_K.m21)  / det;
				// Done
				VECTOR3 VT; 
				VT.x = sin(Xtt_p)*cos(Xtt_y);
				VT.y = sin(Xtt_y);
				VT.z = cos(Xtt_p)*cos(Xtt_y);
				VT = mul(MX_Ki,VT);
				X_S1 = VT.x; X_S2 = VT.y; X_S3 = VT.z;
				
				// FINALLY - COMMANDS!
				double YAW,PITCH;
				YAW   = asin(X_S2);
				PITCH = atan(-(X_S3/X_S1));				

				sprintf(oapiDebugString(),"TB%d+%f | CP/Y %f %f | .dXEZ %f %f %f",
					LVDC_Timebase,LVDC_TB_ETime,
					PITCH,YAW,
					dot_dxi,dot_deta,dot_dzeta);

				// NO TSMC
				CommandedAttitude.y += PITCH;
				CommandedAttitude.z += YAW;
			}
			break;

			case 30:
				// FREEZE FRAME
				break;


	}
	// STEERING ANGLE LIMIT TEST GOES HERE
	// YAW COMMAND LIMIT
	if(CommandedAttitude.z < -45){ CommandedAttitude.z = -45; }
	if(CommandedAttitude.z >  45){ CommandedAttitude.z =  45; }

	/* **** LVDA **** */

	// Compute attitude error
	AttitudeError.x = (CommandedAttitude.x*RAD) - CurrentAttitude.x;
	if (AttitudeError.x > 0) { 
		if (AttitudeError.x > PI){ AttitudeError.x = -(TWO_PI - AttitudeError.x); }
	} else {
		if (AttitudeError.x < -PI) { AttitudeError.x = TWO_PI + AttitudeError.x; }
	}
	// PITCH ERROR
	AttitudeError.y = (CommandedAttitude.y*RAD) - CurrentAttitude.y;
	if (AttitudeError.y > 0) { 
		if (AttitudeError.y > PI){ AttitudeError.y = -(TWO_PI - AttitudeError.y); }
	} else {
		if (AttitudeError.y < -PI) { AttitudeError.y = TWO_PI + AttitudeError.y; }
	}
	// YAW ERROR
	AttitudeError.z = (CommandedAttitude.z*RAD) - CurrentAttitude.z;
	if (AttitudeError.z > 0) { 
		if (AttitudeError.z > PI){ AttitudeError.z = -(TWO_PI - AttitudeError.z); }
	} else {
		if (AttitudeError.z < -PI) { AttitudeError.z = TWO_PI + AttitudeError.z; }
	}

	// Adjust error for roll
	AttitudeError = lvdc_AdjustErrorsForRoll(CurrentAttitude,AttitudeError);	

	/* **** FLIGHT CONTROL COMPUTER OPERATIONS **** */

	// Create gimbal demand.
	if(stage == LAUNCH_STAGE_ONE){
		if(MissionTime < 105){ ErrorGain = 0.9; RateGain =  0.69; }else{
			if(MissionTime < 130){ ErrorGain = 0.45; RateGain =  0.64; }else{
				ErrorGain = 0.32; RateGain =  0.30; }}}
	if(stage == LAUNCH_STAGE_TWO && LVDC_Timebase == 3){
		if(LVDC_TB_ETime < 60){ ErrorGain = 1.12; RateGain =  1.9; }else{
			if(LVDC_TB_ETime < 190){ ErrorGain = 0.65; RateGain =  1.1; }else{
				ErrorGain = 0.44; RateGain =  0.74; }}}

	// #4
	GPitch[0] = (ErrorGain*AttitudeError.y + RateGain*AttRate.y);
	GYaw[0] = (ErrorGain*AttitudeError.z + RateGain*AttRate.z);
	GYaw[0] -= -(ErrorGain*AttitudeError.x + RateGain*AttRate.x);	// Roll

	// #2
	GPitch[1] = (ErrorGain*AttitudeError.y + RateGain*AttRate.y);
	GYaw[1] = (ErrorGain*AttitudeError.z + RateGain*AttRate.z);
	GYaw[1] += -(ErrorGain*AttitudeError.x + RateGain*AttRate.x);	// Roll

	// #1
	GPitch[2] = (ErrorGain*AttitudeError.y + RateGain*AttRate.y);
	GYaw[2] = (ErrorGain*AttitudeError.z + RateGain*AttRate.z);
	GPitch[2] -= -(ErrorGain*AttitudeError.x + RateGain*AttRate.x); // Roll

	// #3
	GPitch[3] = (ErrorGain*AttitudeError.y + RateGain*AttRate.y);
	GYaw[3] = (ErrorGain*AttitudeError.z + RateGain*AttRate.z);
	GPitch[3] += -(ErrorGain*AttitudeError.x + RateGain*AttRate.x); // Roll
			
	// Gimbal angle limitation
	int x=0;
	while(x<4){
		if(GPitch[x] >  0.0872664626){ GPitch[x] =  0.0872664626; }
		if(GPitch[x] < -0.0872664626){ GPitch[x] = -0.0872664626; }
		if(GYaw[x] >  0.0872664626){ GYaw[x] =  0.0872664626; }
		if(GYaw[x] < -0.0872664626){ GYaw[x] = -0.0872664626; }
		x++;
	}

	// LEFT YAW is X, UP PITCH is Y, Z is always 1
	SetThrusterDir(th_main[0],_V(GYaw[0],GPitch[0],1)); //4 
	SetThrusterDir(th_main[1],_V(GYaw[1],GPitch[1],1)); //2 
	SetThrusterDir(th_main[2],_V(GYaw[2],GPitch[2],1)); //1 
	SetThrusterDir(th_main[3],_V(GYaw[3],GPitch[3],1)); //3

	// Debug if we're launched
	if(LVDC_Timebase > 0 && LVDC_GP_PC < 25){ // Pre-IGM		
		sprintf(oapiDebugString(),"LVDC: TB%d + %f | PS %f %f %f | VS %f %f %f",
			LVDC_Timebase,LVDC_TB_ETime,
			PosS.x,PosS.y,PosS.z,
			DotS.x,DotS.y,DotS.z);
//			CurrentAttitude.x*DEG,CurrentAttitude.y*DEG,CurrentAttitude.z*DEG,V);								
	}

	// AttitudeError.x*DEG,AttitudeError.y*DEG,AttitudeError.z*DEG,

	// Update engine indicators and failure flags
	if(LVDC_EI_On == true){
		double level;
		int i;
		for (i = 0; i <= 4; i++){
			level = GetThrusterLevel(th_main[i]);
			if(level > 0  && ENGIND[i] == true){  ENGIND[i] = false; } // UNLIGHT
			if(level == 0 && ENGIND[i] == false){  ENGIND[i] = true; }   // LIGHT
		}
	}
	if(stage == LAUNCH_STAGE_ONE && MissionTime < 12.5){
		// Control contrail
		if (MissionTime > 12)
			contrailLevel = 0;
		else if (MissionTime > 7)
			contrailLevel = (12.0 - MissionTime) / 100.0;
		else if (MissionTime > 2)
			contrailLevel = 1.38 - 0.95 / 5.0 * MissionTime;
		else
			contrailLevel = 1;
	}

	/* **** ABORT HANDLING **** */
	if(bAbort){
		// ABORT MODE 1 - Use of LES to extract CM
		// Allowed from T - 5 minutes until LES jettison.
		if(MissionTime > -300 && LESAttached){			
			SetEngineLevel(ENGINE_MAIN, 0);
			SeparateStage(CM_STAGE);
			SetStage(CM_STAGE);
			StartAbort();
			bAbort = false;
			agc.SetInputChannelBit(030, 4, true); // Notify the AGC of the abort
			agc.SetInputChannelBit(030, 5, true); // and the liftoff, if it's not set already
		}
		// All abort conditions failed, discard the abort request.
		bAbort = false;
	}

}

// Perform error adjustment.
VECTOR3 SaturnV::lvdc_AdjustErrorsForRoll(VECTOR3 attitude, VECTOR3 errors)
{
	VECTOR3 output_errors;
	double input_pitch = errors.y;
	double input_yaw = errors.z;
	double roll_percent,output_pitch,output_yaw,pitch_factor = 1;
	if(attitude.x == 0){ // If zero or inop, return unmodified to avoid SPECIAL CASE
		return(errors);
	}
	if(attitude.x > 4.712388){                    // 0 thru 90 degrees
		roll_percent = fabs((attitude.x-TWO_PI) / 1.570796);				
		output_pitch = input_pitch * (1-roll_percent); 
		output_pitch += input_yaw * roll_percent;
		output_yaw = input_yaw * (1-roll_percent);
		output_yaw -=input_pitch * roll_percent;       
	}
	if(attitude.x > PI && attitude.x < 4.712388){ // 90 thru 180 degrees
		roll_percent = (attitude.x-PI) / 1.570796;					
		output_pitch = -(input_pitch * (1-roll_percent)); 
		output_pitch += input_yaw * roll_percent;
		output_yaw = -input_yaw * (1-roll_percent);
		output_yaw -=input_pitch * roll_percent;       
	}
	if(attitude.x > 1.570796 && attitude.x < PI){ // 180 thru 270 degrees
		roll_percent = fabs((attitude.x-PI) / 1.570796);
		output_pitch = -(input_pitch * (1-roll_percent)); 
		output_pitch -= input_yaw * roll_percent;
		output_yaw = -input_yaw * (1-roll_percent);
		output_yaw +=input_pitch * roll_percent;       
	}
	if(attitude.x > 0 && attitude.x < 1.570796){ // 270 thru 360 degrees
		roll_percent = attitude.x / 1.570796;					
		output_pitch = input_pitch * (1-roll_percent); 
		output_pitch -= input_yaw * roll_percent;
		output_yaw = input_yaw * (1-roll_percent);
		output_yaw +=input_pitch * roll_percent;       
	}

	output_errors.x = errors.x;
	output_errors.y = output_pitch;
	output_errors.z = output_yaw;
	return(output_errors);
}