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
  *	Revision 1.11  2012/11/22 18:08:40  meik84
  *	LVDC++ Various improvements & bug-fixes
  *	
  *	Revision 1.10  2012/11/04 13:33:13  meik84
  *	LVDC++
  *	
  *	Revision 1.9  2012/03/24 19:14:26  tschachim
  *	Moved LVDC++ to PostStep
  *	
  *	Revision 1.8  2012/01/14 22:24:06  tschachim
  *	CM Optics cover
  *	
  *	Revision 1.7  2010/08/25 17:48:42  tschachim
  *	Bugfixes Saturn autopilot.
  *	
  *	Revision 1.6  2010/07/16 17:14:42  tschachim
  *	Changes for Orbiter 2010 and bugfixes
  *	
  *	Revision 1.5  2010/02/22 14:23:30  tschachim
  *	Apollo 7 S-IVB on orbit attitude control, venting and Saturn takeover mode for the VAGC.
  *	
  *	Revision 1.4  2009/12/05 17:28:29  tschachim
  *	Bugfix Apollo 8 S4B_EmptyMass
  *	
  *	Revision 1.3  2009/07/09 13:04:29  dseagrav
  *	Clobber LVIMU coarse-align flag at boot.
  *	
  *	Revision 1.2  2009/07/04 21:36:09  dseagrav
  *	LVDC++ update
  *	Should not break anything else
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.113  2008/04/11 12:18:57  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.112  2008/01/25 04:39:42  lassombra
  *	All switches now handle change of state through SwitchTo function which is vitual
  *	 and is called by existing mouse and connector handling methods.
  *	
  *	Support for delayed spring switches and other ChecklistController functionality following soon.
  *	
  *	Revision 1.111  2008/01/16 04:14:24  movieman523
  *	Rewrote docking probe separation code and moved the CSM_LEM code into a single function in the Saturn class.
  *	
  *	Revision 1.110  2008/01/14 04:48:43  movieman523
  *	Fixed LEM separation when the LEM doesn't have the expected name.
  *	
  *	Revision 1.109  2008/01/14 04:31:08  movieman523
  *	Initial tidyup: ASTP should now work too.
  *	
  *	Revision 1.108  2008/01/14 01:17:04  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.107  2007/12/04 20:26:31  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.106  2007/11/26 17:59:05  movieman523
  *	Assorted tidying up of state variable structures.
  *	
  *	Revision 1.105  2007/10/18 00:23:17  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
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
		{-180.*RAD,-160.*RAD,-150.*RAD,-120.*RAD,-90.*RAD,0*RAD,90.*RAD,120.*RAD,150.*RAD,160.*RAD,180.*RAD};
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
		thrust = 1012506;
		isp = 4152;
	
	} else if (ratio > 4.2 && ratio < 4.4) {	// 4.3
		thrust = 783617.4;
		isp = 4223.7;

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
		thrust = 901557.;
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

			//
			// Tower jettison at 36.2 seconds after SIC shutdown if not previously
			// specified.
			//

			if (LESJettisonTime > 999.0) {
				LESJettisonTime = MissionTime + 5.7;
			}
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
			// Nothing for now, the LVDC is called in PostStep
		} else {

			// AFTER LVDC++ WORKS SATISFACTORILY EVERYTHING IN THIS BLOCK
			// SHOULD BE SAFE TO DELETE. DO NOT ADD ANY LVDC++ CODE IN THIS BLOCK.

			if (bAbort && MissionTime > -300 && LESAttached) {
				SetEngineLevel(ENGINE_MAIN, 0);
				SeparateStage(CM_STAGE);
				SetStage(CM_STAGE);
				StartAbort();
				agc.SetInputChannelBit(030, 4, true); // Notify the AGC of the abort
				agc.SetInputChannelBit(030, 5, true); // and the liftoff, if it's not set already
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

	if (stage < CSM_LEM_STAGE) {
		// LVDC++
		if (use_lvdc) {
			lvdc_timestep(simt, simdt);
		}
	}
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
		if (VehicleNo == 503){
			// Apollo 8 -- For LVDC++ experiments.
			if (!S1_ThrustLoaded){
				// F1 thrust computed as follows:
				// 6782000 @ Sea Level, From AP8 LV Evaluation, averaged predicted value
				// Was making 8000000 @ S1C OECO				
				THRUST_FIRST_VAC = 8000100; 
				THRUST_SECOND_VAC = 1001000;
				THRUST_THIRD_VAC = 901557;
				// Masses from Apollo By The Numbers for AP8
				SI_EmptyMass = 139641.0; // Minus retro weight, that gets added seperately
				SI_FuelMass = 2038222.0;
				Interstage_Mass = 5641;
				SII_EmptyMass = 49744.0; // Includes S2/S4B interstage, does not include ullage jets
				SII_FuelMass = 430936.0; 
				S4B_EmptyMass = 22981.0; // Includes S4B stage, IU, LM adapter, but NOT the LTA
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

// ********** LVDC++ **********

// DS20070205 LVDC++ SETUP
void SaturnV::lvdc_init(){
	lvimu.Init();							// Initialize IMU
	lvrg.Init(this);						// LV Rate Gyro Package
	lvimu.SetVessel(this);					// set vessel pointer
	lvimu.CoarseAlignEnableFlag = false;	// Clobber this
	//presettings in order of boeing listing for easier maintainece
	//GENERAL
	e = 0;
	f = 0;
	C_3 = -60731530.2; // Stored as twice the etc etc.
	  //C_3 = -60903382.7388059;
	Direct_Ascent = false;					// flag for selecting direct ascent targeting; not used normally
	GATE = false;							// 'chi-freeze-gate': freezes steering commands when true
	GATE0 = false;							// allows single pass through restart preparations (TB6 start) when false
	GATE1 = false;							// allows single pass through out-of-orbit targeting calculations when false
	GATE2 = false;							// logic switch for first or second opportunity out-of-orbit targeting; false for first
	GATE3 = false;							// allows single pass through out-of-orbit IGM precalculations when false
	GATE4 = false;							// allows single pass through direct-staging guidance update when false
	GATE5 = false;							// allows single pass through HSL initialization when false
	INH = false;							// inhibits restart preparations; set by x-lunar inject/inhibit switch
	INH1 = false;							// inhibits first EPO roll/pitch maneuver
	INH2 = false;							// inhibits second EPO roll/pitch maneuver
	TA1 = 2700;								//time for TB5 start to first maneuver
	TA2 = 5160;								//time for TB5 start to second maneuver
	TB1 = TB2 = TB3 = TB4 = TB5 = TB6 = TB7 = -100000; //LVDC's elapsed timebase times; set to 0 when resp. TB starts
	T_LET = 35.1;							// LET Jettison Time, i.e. the time IGM starts after start of TB3
	TU = false;								// flag indicating target update has been received from ground
	TU10 = false;							// flag indicating 10-parameter target update has been received
	UP = 0;									// switching variable for Tt_t parameter update
	//alpha _d_op?
	i_op = true;							// flag for selecting method of EPO inclination calculation
	theta_N_op = true;						// flag for selecting method of EPO descending node calculation
	//PRE_IGM GUIDANCE
	B_11 = -0.62;							// Coefficients for determining freeze time after S1C engine failure
	B_12 = 40.9;							// dto.
	B_21 = -0.3611;							// dto.
	B_22 = 29.25;							// dto.
	// PITCH POLYNOMIAL (Apollo 9)
	Fx[1][0] =  3.19840;
	Fx[1][1] =  -0.544236;
	Fx[1][2] =  0.0351605;
	Fx[1][3] =  -0.00116379;
	Fx[1][4] =  0.000011386;
	Fx[2][0] =  -10.9607;
	Fx[2][1] =  0.946620;
	Fx[2][2] =  -0.0294206;
	Fx[2][3] =  0.000207717;
	Fx[2][4] =  -0.000000439036;
	Fx[3][0] =  78.7826;
	Fx[3][1] =  -2.83749;
	Fx[3][2] =  0.0289710;
	Fx[3][3] =  -0.000178363;
	Fx[3][4] =  0.000000463029;
	Fx[4][0] =  69.9191;
	Fx[4][1] =  -2.007490;
	Fx[4][2] =  0.0105367;
	Fx[4][3] =  -0.0000233163;
	Fx[4][4] =  0.0000000136702;
	t_1 = 13;								// Backup timer for Pre-IGM pitch maneuver
	t_2 = 25;								// Time to initiate pitch freeze for S1C engine failure
	t_3 = 36;								// Constant pitch freeze for S1C engine failure prior to t_2
	t_3i=0;									// Clock time at S4B ignition
	t_4 = 45;								// Upper bound of validity for first segment of pitch freeze
	t_5 = 81;								// Upper bound of validity for second segment of pitch freeze
	t_6 = 0;								// Time to terminate pitch freeze after S1C engine failure
	T_ar = 145;								// S1C Tilt Arrest Time	   
	T_S1 = 33.6;							// Pitch Polynomial Segment Times
	T_S2 = 68.6;							// dto.
	T_S3 = 95.6;							// dto.
	T_EO1 = 0;								// switching constant for SIC engine-out handling
	T_EO2 = 0;								// allows single pass through IGM engine-out presettings when 0
	dt = 1.7;								// Nominal powered-flight or coast-guidance computation-cycle interval
	dT_F=0;									// Period of frozen pitch in S1C
	dt_LET = 35.1;							// Nominal time between SII ign and LET jet
	t_fail =0;								// S1C Engine Failure time
	CommandRateLimits=_V(1*RAD,1*RAD,1*RAD);// Radians per second
	//IGM BOOST TO ORBIT
	Ct = 0;
	Ct_o = 10;
	Cf = 0.087996;
	// Inclination from azimuth polynomial
	fx[0] = 32.55754;  fx[1] = -15.84615; fx[2] = 11.64780; fx[3] = 9.890970;
	fx[4] = -5.111430; fx[5] = 0;         fx[6] = 0;
	// Descending Node Angle from azimuth polynomial
	gx[0] = 123.1935; gx[1] = -55.06485; gx[2] = -35.26208; gx[3] = 26.01324;
	gx[4] = -1.47591; gx[5] = 0;         gx[6] = 0;			
	MRS = false;							// MR Shift
	dotM_1 = 1219.299283;						// Mass flowrate of S2 from approximately LET jettison to second MRS
	dotM_2 = 961.8088872;						// Mass flowrate of S2 after second MRS
	dotM_3 = 222.4339038;					// Mass flowrate of S4B during first burn
	ROT = false;
	dV_B = 0.02; // AP11// dV_B = 2.0275; // AP9// Velocity cutoff bias for orbital insertion
	ROV = 1.48119724870249;//0.75-17
	ROVs = 1.5;
	phi_L = 28.608310*RAD;						// Geodetic Launch site latitude
	SMCG = 0.05*RAD;
	TS4BS = 13.5;
	TSMC1 = 20; TSMC2 = 5; // AP9
	// TSMC1 = 60.6 TSMC2 = 15 // AP11
	T_c = 8;// T_c = 6.5; 					// Coast time between S2 burnout and S4B ignition
	T_1 = 249.1;//T_1  = 237.796;			// Time left in first-stage IGM
	T_2 = 91.8;//T_2 = 111;					// Time left in second and fourth stage IGM
	T_3 = 0;								// Time left in third and fifth stage IGM
	T_1c = T_1 + T_2 + T_c;					// Sum of the burn times of IGM first, second, and coast guidance stages
	T_4N = 120.565;//T_4N = 120.565;		// Nominal time of S4B first burn
	Tt_3 = 188;//Tt_3 = 135.6;			// Estimated third or fifth stage burn time
	Tt_T = T_1c + Tt_3;						// Time-To-Go computed using Tt_3
	t = 0;									// Time from accelerometer reading to next steering command
	t_B1 = 4;								// Transition time for the S2 mixture ratio to shift from 5.5 to 4.7
	t_B3 = 0;								// Time from second S2 MRS signal
	//dt: not set; dependend on cycle time
	dT_LIM = 90;							// Limit to dT_4;
	V_ex1 = 4148.668555;
	V_ex2 = 4158.852692;
	V_ex3 = 4130.010682;
	V_S2T = 7007.18;
	V_TC = 300;
	eps_1 = 0;								// IGM range angle calculation selection
	eps_2 = 32;								// Time to begin chi bar steering
	eps_3 = 10000;							// Terminal condition freeze time
	eps_4 = 3;								// Time to enable HSL loop & chi freeze
	mu = 398600420000000;					// Product of G and Earth's mass
	tau2 = 308.95;							// Time to consume all fuel between MRS and S2 Cutoff
	tau3 = 748.7;							// Time to consume all fuel of SIVB
	tau3N = tau3;							// artificial tau3
	//rate limits: set in pre-igm
	alpha_1 = 0;									//orbital guidance pitch
	alpha_2 = 0;									//orbital guidance yaw
	K_P1 = 4.3 * RAD;							// restart attitude coefficients
	K_P2 = 0;
	K_Y1 = 0;
	K_Y2 = 0;
	first_op = true;

	//Not in boeing doc, but needed for nav:
	a = 6378137;							//earth's equatorial radius
	J = 0.0010826;							//first coefficient of earth's gravity

	//'real' software variable, i.e. those are computed at runtime
	// Software flags
	liftoff = false;
	poweredflight = false;
	HSL=false;								// High-Speed Loop flag
	BOOST=false;							// Boost To Orbit
	S4B_IGN=false;							// SIVB Ignition
	S2_BURNOUT=false;						// SII Burn Out
	LVDC_GRR = false;
	S1C_Engine_Out = false;		
	tau1=0;									// Time to consume all fuel before S2 MRS
	Fm=0;									// sensed total accel
	Azimuth = 72.124;
	Inclination=0;							// Inclination
	DescNodeAngle=0;						// Descending Node Angle -- THETA_N
	Azo=0; Azs=0;							// Variables for scaling the -from-azimuth polynomials
	CommandedAttitude=_V(0,0,0);			// Commanded Attitude (RADIANS)
	PCommandedAttitude=_V(0,0,0);			// Previous Commanded Attitude (RADIANS)
	CurrentAttitude=_V(0,0,0);				// Current Attitude   (RADIANS)
	F=0;									// Force in Newtons, I assume.	
	K_Y=0; K_P=0; D_P=0; D_Y=0;				// Intermediate variables in IGM
	P_1=0; P_2=0; P_3=0; P_12=0;	
	L_1=0; L_2=0; L_3=0; dL_3=0; Lt_3=0; L_12=0; L_P=0; L_Y=0; Lt_Y=0;
	J_1=0; J_2=0; J_3=0; J_12=0; Jt_3=0; J_Y=0; J_P=0; 
	S_1=0; S_2=0; S_3=0; S_12=0; S_P=0; S_Y=0; 
	U_1=0; U_2=0; U_3=0; U_12=0; 
	Q_1=0; Q_2=0; Q_3=0; Q_12=0; Q_Y=0; Q_P=0; 
	d2=0;
	f=0;									// True anomaly of the predicted cutoff radius vector
	e=0;									// Eccentricity of the transfer ellipse
	C_2=0; C_4=0;							// IGM coupling terms for pitch steering
	p=0;									// semilatus rectum of terminal ellipse
	K_1=0; K_2=0; K_3=0; K_4=0;				// Correction to chi-tilde steering angles, K_i
	K_5=0;									// IGM terminal velocity constant
	R=0;									// Instantaneous Radius Magnitude
	R_T=0;									// Desired terminal radius
	V=0;									// Instantaneous vehicle velocity
	V_T=0;									// Desired terminal velocity
	V_i=0; V_0=0; V_1=0; V_2=0;				// Parameters for cutoff velocity computation
	ups_T=0;								// Desired terminal flight-path angle
	MX_A=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to equatorial
	MX_B=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from equatorial to orbital coordinates
	MX_G=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to orbital
	MX_K=_M(0,0,0,0,0,0,0,0,0);				// Transform matrix from earth-centered plumbline to terminal
	MX_phi_T=_M(0,0,0,0,0,0,0,0,0);			// Matrix made from phi_T
	phi_T=0;								// Angle used to estimate location of terminal radius in orbital plane
	Pos4=_V(0,0,0);							// Position in the orbital reference system
	PosS=_V(0,0,0);							// Position in the earth-centered plumbline system. SPACE-FIXED.
	DotS=_V(0,0,0);							// VELOCITY in the earth-centered plumbline system
	DotM_act=_V(0,0,0);						// actual sensed velocity from platform
	ddotM_act =_V(0,0,0);
	ddotM_last =_V(0,0,0);
	ddotG_act=_V(0,0,0);					// actual computed acceleration from gravity
	DotG_act=_V(0,0,0);						// actual computed velocity from gravity
	DotM_last=_V(0,0,0);					// last sensed velocity from platform
	ddotG_last=_V(0,0,0);					// last computed acceleration from gravity
	DotG_last=_V(0,0,0);					// last computed velocity from gravity
	alpha_D=0;								// Angle from perigee to DN vector
	alpha_D_op=true;						// Option to determine alpha_D or load it
	G_T=0;									// Magnitude of desired terminal gravitational acceleration
	xi_T=0; eta_T=0; zeta_T=0;				// Desired position components in the terminal reference system
	PosXEZ=_V(0,0,0);						// Position components in the terminal reference system
	DotXEZ=_V(0,0,0);						// Instantaneous something
	deta=0; dxi=0;							// Position components to be gained in this axis
	dT_3=0;									// Correction to third or fifth stage burn time
	dT_4=0;									// Difference between nominal and actual 1st S4B burn time
	dTt_4=0;								// Limited value of above
	T_T=0;									// Time-To-Go computed using T_3
	tchi_y=0; tchi_p=0;						// Angles to null velocity deficiencies without regard to terminal data
	dot_zeta_T=0; dot_xi_T=0; dot_eta_T=0;	// I don't know.
	ddot_zeta_GT=0; ddot_xi_GT=0;
	DDotXEZ_G=_V(0,0,0);					// ???
	ddot_xi_G=0; ddot_eta_G=0; ddot_zeta_G=0;								
	dot_dxit=0; dot_detat=0; dot_dzetat=0; 	// Intermediate velocity deficiency used in time-to-go computation
	dot_dxi=0; dot_deta=0; dot_dzeta=0; 	// More Deltas
	Xtt_y=0; Xtt_p=0; 						// IGM computed steering angles in terminal system
	X_S1=0; X_S2=0; X_S3=0; 				// Direction cosines of the thrust vector
	sin_ups=0; cos_ups=0;					// Sine and cosine of upsilon (flight-path angle)
	dot_phi_1=0; dot_phi_T=0; 				// ???
	dtt_1=0; dtt_2=0;						// Used in TGO determination
	dt = 1.7;								// Nominal powered-flight or coast-guidance computation-cycle interval
	a_1=0; a_2=0;							// Acceleration terms used to determine TGO
	T_GO=0;									// Time before S4B shutdown
	T_CO=0;									// Predicted time of S4B shutdown, from GRR
	dV=0;
	t_21 = 0;
	TAS=0;
	t_clock = 0;
	// TABLE15 and TABLE25 (see saturnv.h)
			TABLE15_f = 360;								// EPO
			TABLE15_e = 0;									// EPO
			TABLE15_C_3 = -60731521.02;						// EPO
//			TABLE15_f = 0.08050500;							// Apollo 11 (1st Opty, Constant)
//			TABLE15_e = 0.9762203;							// Apollo 11 (1st Opty, Index 0)
//			TABLE15_C_3 = -1437084;							// Apollo 11 (1st Opty, Index 0)
//			TABLE15_e = 0.9762098;							// Apollo 11 (1st Opty, Index 1)
//			TABLE15_C_3 = -1437508;							// Apollo 11 (1st Opty, Index 1)
//			TABLE15_e = 0.9761908;							// Apollo 11 (1st Opty, Index 2)
//			TABLE15_C_3 = -1438535;							// Apollo 11 (1st Opty, Index 2)
//			TABLE15_e = 0.9761679;							// Apollo 11 (1st Opty, Index 3)
//			TABLE15_C_3 = -1439902;							// Apollo 11 (1st Opty, Index 3)
//			TABLE15_e = 0.9761432;							// Apollo 11 (1st Opty, Index 4)
//			TABLE15_C_3 = -1441497;							// Apollo 11 (1st Opty, Index 4)


	// Set up remainder
	LVDC_Timebase = -1;						// Start up halted in pre-launch pre-GRR loop
	LVDC_TB_ETime = 0;
	LVDC_GP_PC = 0;
	IGMCycle = 0;
	sinceLastIGM = 0;
	// INTERNAL (NON-REAL-LVDC) FLAGS
	LVDC_EI_On = false;
	S1C_Sep_Time = 0;
	CountPIPA = false;
	lvlog = fopen("lvlog5.txt","w+");
}
	
// DS20070205 LVDC++ EXECUTION
void SaturnV::lvdc_timestep(double simt, double simdt) {
		
	// Update timebase ET
	LVDC_TB_ETime += simdt;
	
	// Note that GenericTimestep will update MissionTime.

	/* **** LVDC GUIDANCE PROGRAM **** */
	switch(LVDC_Timebase){//this is the sequential event control logic
		case -1: // LOOP WAITING FOR PTL
			// Lock time accel to 100x
			if (oapiGetTimeAcceleration() > 100){ oapiSetTimeAcceleration(100); } 

			// Prelaunch tank venting between -3:00h and engine ignition
			// No clue if the venting start time is correct
			if (MissionTime < -10800){
				DeactivatePrelaunchVenting();
			}else{
				ActivatePrelaunchVenting();
			}

			// BEFORE PTL COMMAND (T-00:20:00) STOPS HERE
			if (MissionTime < -1200){
				double Source  = fabs(MissionTime);
				double Minutes = Source/60;
				double Hours   = (int)Minutes/60;				
				double Seconds = Source - ((int)Minutes*60);
				Minutes       -= Hours*60;
				sprintf(oapiDebugString(),"LVDC: T - %d:%d:%f | AWAITING PTL INTERRUPT",(int)Hours,(int)Minutes,Seconds);
				lvimu.ZeroIMUCDUFlag = true;					// Zero IMU CDUs
				break;
			}
		

			// Engine lights on at T-00:04:10
			if (MissionTime >= -250 && LVDC_EI_On == false) { LVDC_EI_On = true; }

			// Between PTL signal and GRR, we monitor the IMU for any failure signals and do vehicle self-tests.
			// At GRR we transfer control to the flight program and start TB0.

			// BEFORE GRR (T-00:00:17) STOPS HERE
			if (MissionTime < -17){
				sprintf(oapiDebugString(),"LVDC: T %f | IMU XYZ %d %d %d PIPA %d %d %d | TV %f | AWAITING GRR",MissionTime,
					lvimu.CDURegisters[LVRegCDUX],lvimu.CDURegisters[LVRegCDUY],lvimu.CDURegisters[LVRegCDUZ],
					lvimu.CDURegisters[LVRegPIPAX],lvimu.CDURegisters[LVRegPIPAY],lvimu.CDURegisters[LVRegPIPAZ],atan((double)45));
				break;
			}else{LVDC_Timebase = 0;
				  LVDC_TB_ETime = 0;
				  break;}
			

		case 0: 
			if(LVDC_GRR == false){
			lvimu.ZeroIMUCDUFlag = false;					// Release IMU CDUs
			lvimu.DriveGimbals((Azimuth - 90)*RAD,0,0);		// Now bring to alignment 
			lvimu.SetCaged(false);							// Release IMU
			CountPIPA = true;								// Enable PIPA storage			
			BOOST = true;
			LVDC_GRR = true;								// Mark event
			poweredflight = true;
			oapiSetTimeAcceleration (1);					// Set time acceleration to 1
			SetThrusterGroupLevel(thg_main, 0);				// Ensure off
			{	int i;
				for (i = 0; i < 5; i++) {						// Reconnect fuel to S1C engines
					SetThrusterResource(th_main[i], ph_1st);
				}
			}
			CreateStageOne();								// Create hidden stage one, for later use in staging
			}

			double thrst[3];	// Thrust Settings for 1-2-2 start (see below)

			// At 10 seconds, play the countdown sound.
			if (MissionTime >= -10.3) { // Was -10.9
				if (!UseATC && Scount.isValid()) {
					Scount.play();
					Scount.done();
				}
			}

			// Shut down venting at T - 9
			if(MissionTime > -9 && prelaunchvent[0] != NULL) { DeactivatePrelaunchVenting(); }

			// Engine startup was staggered 1-2-2, with engine 5 starting first, then 1+3, then 2+4. 
			// This happened by the starter solenoid operating at T-6.585 for engine 5.

			// Engine 5 combustion chamber ignition was at T-3.315, engines 1+3 at T-3.035, and engines 2+4 at T-2.615
			// The engines idled in low-range thrust (about 2.5% thrust) for about 0.3 seconds
			// and then rose to 93% thrust in 0.85 seconds.
			// The rise from 93 to 100 percent thrust took 0.75 second.
			// Total engine startup time was 1.9 seconds.

			// Source: Apollo 8 LV Flight Evaluation

			// Transition from seperate throttles to single throttle
			if(MissionTime < -0.715){ 
				int x=0; // Start Sequence Index
				double tm_1,tm_2,tm_3,tm_4; // CC light, 1st rise start, and 2nd rise start, and 100% thrust times.
				double SumThrust=0;

				while(x < 3){
					thrst[x] = 0;
					switch(x){
						case 0: // Engine 5
							tm_1 = -3.315; break;
						case 1: // Engine 1+3
							tm_1 = -3.035; break;
						case 2: // Engine 2+4
							tm_1 = -2.615; break;
					}
					tm_2 = tm_1 + 0.3;  // Start of 1st rise
					tm_3 = tm_2 + 0.85; // Start of 2nd rise
					tm_4 = tm_3 + 0.75; // End of 2nd rise
					if(MissionTime >= tm_1){
						// Light CC
						if(MissionTime < tm_2){
							// Idle at 2.5% thrust
							thrst[x] = 0.025;
						}else{
							if(MissionTime < tm_3){
								// Rise to 93% at a rate of 106 percent per second
								thrst[x] = 0.025+(1.06*(MissionTime-tm_2));
							}else{
								if(MissionTime < tm_4){
									// Rise to 100% at a rate of 9 percent per second.
									thrst[x] = 0.93+(0.09*(MissionTime-tm_3));
								}else{
									// Hold 100%
									thrst[x] = 1;
								}
							}
						}
					}
					x++; // Do next
				}
				SumThrust = thrst[0]+(thrst[1]*2)+(thrst[2]*2);
//				sprintf(oapiDebugString(),"LVDC: T %f | TB0 + %f | TH 0/1/2 = %f %f %f Sum %f",
//					MissionTime,LVDC_TB_ETime,thrst[0],thrst[1],thrst[2],SumThrust);
				if(SumThrust > 0){
					SetThrusterLevel(th_main[2],thrst[1]); // Engine 1
					SetThrusterLevel(th_main[1],thrst[2]); // Engine 2
					SetThrusterLevel(th_main[3],thrst[1]); // Engine 3
					SetThrusterLevel(th_main[0],thrst[2]); // Engine 4
					SetThrusterLevel(th_main[4],thrst[0]); // Engine 5

					contrailLevel = SumThrust/5;
					AddForce(_V(0, 0, -10. * THRUST_FIRST_VAC), _V(0, 0, 0)); // Maintain hold-down lock
				}
			}else{
				// Get 100% thrust on all engines.
				SetThrusterGroupLevel(thg_main,1);
				contrailLevel = 1;				
				AddForce(_V(0, 0, -10. * THRUST_FIRST_VAC), _V(0, 0, 0));
			}

			if(MissionTime >= 0){
				TB1 = -simdt;
				LVDC_Timebase = 1;
				LVDC_TB_ETime = 0;
				break;
			}
			break;

		case 1: // LIFTOFF TIME
			if(liftoff == false){
			liftoff = true;
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
			// Fall into TB1
			sinceLastIGM = 1.7-simdt; // Rig to pass on fall-in
			}

			// Soft-Release Pin Dragging
			if(MissionTime < 0.5){
			  double PinDragFactor = 1 - (MissionTime*2);
		      AddForce(_V(0, 0, -(THRUST_FIRST_VAC * PinDragFactor)), _V(0, 0, 0));
			}

			// S1C CECO TRIGGER:
			// I have multiple conflicting leads as to the CECO trigger.
			// One says it happens at 4G acceleration and another says it happens by a timer at T+135.5
			
			if(MissionTime > 125.9){ // Apollo 11
				SetThrusterResource(th_main[4], NULL); // Should stop the engine
				SShutS.play(NOLOOP,235);
				SShutS.done();
				// Clear liftoff light now - Apollo 15 checklist item
				ClearLiftoffLight();
				S1C_Engine_Out = true;
				// Begin timebase 2
				TB2 = -simdt;
				LVDC_Timebase = 2;
				LVDC_TB_ETime = 0;
				break;
			}
			break;
		case 2:
			// S1C OECO TRIGGER
			// Done by low-level sensor.
			// Apollo 8 cut off at 32877, Apollo 11 cut off at 31995.
			if (GetFuelMass() <= 0){
				// For S1C thruster calibration
				fprintf(lvlog,"[T+%f] S1C OECO - Thrust %f N @ Alt %f\r\n\r\n",
					MissionTime,GetThrusterMax(th_main[0]),GetAltitude());

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
				TB3 = - simdt;
				LVDC_Timebase = 3;
				LVDC_TB_ETime = 0;
				}
			break;
		case 3:
			// S1C SEPARATION TRIGGER
			if(stage == LAUNCH_STAGE_ONE  && LVDC_TB_ETime >= 0.5){
				// Drop old stage
				
				SeparateStage(LAUNCH_STAGE_TWO);
				SetStage(LAUNCH_STAGE_TWO);
				// Fire S2 ullage
				if (SII_UllageNum) {
					SetThrusterGroupLevel(thg_ull, 1.0);
					SepS.play(LOOP, 130);
				}
				ActivateStagingVent();
				S2_Startup = false;
			}
			// S2 ENGINE STARTUP
			if(stage == LAUNCH_STAGE_TWO  && LVDC_TB_ETime >= 2.4 && LVDC_TB_ETime < 4.4){
				S2_Startup = true;
				SIISepState = true;
				SetSIICMixtureRatio(5.5);
				DeactivateStagingVent();
				SetThrusterGroupLevel(thg_main, ((LVDC_TB_ETime-2.4)*0.45));
			}

			if(stage == LAUNCH_STAGE_TWO  && LVDC_TB_ETime >= 5 && S2_IGNITION == false){
				SetThrusterGroupLevel(thg_main, 1); // Full power
				if (SII_UllageNum)
					SetThrusterGroupLevel(thg_ull,0.0);
				SepS.stop();
				S2_IGNITION = true;
				S1C_Sep_Time = 0; // All done
			}

			// Drop Interstage Ring
			if(stage == LAUNCH_STAGE_TWO  && LVDC_TB_ETime >= 30.7 && SIISepState == true)
			{
				SeparateStage (LAUNCH_STAGE_TWO_ISTG_JET);
				SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
				SIISepState = false;
			}

			// And jettison LET
			if(stage == LAUNCH_STAGE_TWO_ISTG_JET  && LVDC_TB_ETime > dt_LET && LESAttached){
				T_LET = LVDC_TB_ETime;	// Update this. If the LET jettison never happens, the placeholder value
										// will start IGM anyway.
				JettisonLET();
				}			
			
			
			if(LVDC_TB_ETime > 284.4 && stage == LAUNCH_STAGE_TWO_ISTG_JET && MRS == false){
				// MR Shift
				fprintf(lvlog,"[TB%d+%f] MR Shift\r\n",LVDC_Timebase,LVDC_TB_ETime);
				// sprintf(oapiDebugString(),"LVDC: EMR SHIFT"); LVDC_GP_PC = 30; break;
				SetSIICMixtureRatio (4.5); // Is this 4.7 or 4.2? AP8 says 4.5
				SPUShiftS.play(NOLOOP,255); 
				SPUShiftS.done();
				MRS = true;
			}

			// After MRS, check for S2 OECO (was allowed to happen by itself)
			if(MRS == true){
				double oetl = GetThrusterLevel(th_main[0])+GetThrusterLevel(th_main[1])+GetThrusterLevel(th_main[2])+GetThrusterLevel(th_main[3]);
				if(oetl == 0){
					fprintf(lvlog,"[MT %f] TB4 Start\r\n",simt);
					// S2 OECO, start TB4
					S2_BURNOUT = true;
					MRS = false;
					TB4 = - simdt;
					LVDC_Timebase = 4;
					LVDC_TB_ETime = 0;					
				}
			}

			// TODO: MANUAL S2 STAGING CHECK
			/*
			if (SIISIVBSepSwitch.GetState()) { 		
				...
			}
			*/
			break;
		case 4:
			// S2 STAGE SEP
			if(LVDC_TB_ETime > 0.07 && stage == LAUNCH_STAGE_TWO_ISTG_JET){
				// S2ShutS.done(); No CECO on AP8
				fprintf(lvlog,"[%d+%f] S2/S4B STAGING\r\n",LVDC_Timebase,LVDC_TB_ETime);
				SPUShiftS.done(); // Make sure it's done
				ClearEngineIndicators();
				SeparateStage(LAUNCH_STAGE_SIVB);
				SetStage(LAUNCH_STAGE_SIVB);
				AddRCS_S4B();
				SetSIVBThrusters(true);
				SetThrusterGroupLevel(thg_ver,1.0);
				SetThrusterResource(th_main[0], ph_3rd);
				SetSIVbCMixtureRatio(5.0);
			}
			
			if(LVDC_TB_ETime >= 4 && LVDC_TB_ETime < 6.8 && stage == LAUNCH_STAGE_SIVB){
			SetThrusterGroupLevel(thg_main, ((LVDC_TB_ETime-4)*0.36));
			}
			if(LVDC_TB_ETime >= 8.6 && S4B_IGN == false && stage == LAUNCH_STAGE_SIVB){
			SetThrusterGroupLevel(thg_main, 1.0);
			SetThrusterGroupLevel(thg_ver, 0.0);
			S4B_IGN=true;
			}
			break;
		case 5:
				if (LVDC_TB_ETime >= 10 && LVDC_EI_On == true){
																SetStage(STAGE_ORBIT_SIVB);
																LVDC_EI_On = false;}
				//if(LVDC_TB_ETime < 87 && GetThrusterLevel(th_att_lin[0]) < 1){//ullage thrust on
				//SetThrusterLevel(th_att_lin[0],1);
				//SetThrusterLevel(th_att_lin[1],1);}
				//if(LVDC_TB_ETime >= 87 &&GetThrusterLevel(th_att_lin[0]) > 0){//ullage thrust off
				//SetThrusterLevel(th_att_lin[0],0);
				//SetThrusterLevel(th_att_lin[1],0);}
				if(LVDC_TB_ETime > 100){
					poweredflight = false;}//powered flight nav off
			break;

	}
	lvimu.Timestep(simt);								// Give a timestep to the LV IMU
	lvrg.Timestep(simdt);								// and RG
	CurrentAttitude = lvimu.GetTotalAttitude();			// Get current attitude	
	AttRate = lvrg.GetRates();							// Get rates	
	//This is the actual LVDC code & logic; has to be independent from any of the above events
	if(LVDC_GRR && init == false){
			
			fprintf(lvlog,"[T%f] GRR received!\r\n",MissionTime);

			// Initial Position & Velocity from Apollo 9 operational trajectory
			/*PosS.x = 6373324.5;
			PosS.y = 19255.8;
			PosS.z = 1174.4;*/
			/*DotM_act.x = -0.451;
			DotM_act.y = 125.646;
			DotM_act.z = 388.841;*/
			PosS.x = 6373418.5;
			PosS.y = 0;
			PosS.z = 0;
			/*DotS.x = 0;
			DotS.y = 126.08;
			DotS.z = 388.03;*/
			// Time into launch window = launch time from midnight - reference time of launch from midnight
			// azimuth = coeff. of azimuth polynomial * time into launch window

			// preset to fixed value to be independent from any external stuff
			Azimuth = 72.124;
			fprintf(lvlog,"Azimuth = %f\r\n",Azimuth);

			// Azo and Azs are used to scale the polys below. These numbers are from Apollo 11.
			// Dunno if this actually works. The numbers are in "PIRADS", whatever that is.
			Azo = 4; 
			Azs = 2;

			if(i_op == true){
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
			// Let's cheat a little. (Apollo 8)
			Inclination = 32.5031;
			fprintf(lvlog,"Inclination = %f\r\n",Inclination);

			if(theta_N_op == true){
				// CALCULATE DESCENDING NODAL ANGLE FROM AZIMUTH
				DescNodeAngle = 0;
				int x=0;
				while(x < 7){
					DescNodeAngle += gx[x] * pow((Azimuth-Azo)/Azs,x);
					x++;
				}
			}else{
				// CALCULATE DESCENDING NODAL ANGLE FROM TIME INTO LAUNCH WINDOW
				// DNA = coeff. for DNA-from-time polynomial * Time into launch window
			}
			
			// Cheat a little more. (Apollo 8)
			DescNodeAngle = 123.004; 
			fprintf(lvlog,"DescNodeAngle = %f\r\n",DescNodeAngle);

			// Need to make those into radians
			Azimuth *= RAD;
			Inclination *= RAD;
			DescNodeAngle *= RAD;

			fprintf(lvlog,"Rad Convert: Az / Inc / DNA = %f %f %f\r\n",Azimuth,Inclination,DescNodeAngle);

			if(Direct_Ascent){
				// angle from perigee vector to DNA vector = TABLE25 (time into launch window)
				// terminal guidance freeze time = 0
				sprintf(oapiDebugString(),"LVDC: DIRECT-ASCENT"); // STOP
			}

			// p is the semi-latus rectum of the desired terminal ellipse.
			p = (mu/C_3)*(pow(e,2)-1);
			fprintf(lvlog,"p = %f, mu = %f, e2 = %f, mu/C_3 = %f\r\n",p,mu,pow(e,2),mu/C_3);

			// K_5 is the IGM terminal velocity constant
			K_5 = sqrt(mu/p);
			fprintf(lvlog,"K_5 = %f\r\n",K_5);

			R_T = p/(1+(e*(cos(f))));
			V_T = K_5*sqrt((1+((2*e)*(cos(f)))+pow(e,2)));
			ups_T = atan2((e*(sin(f))),(1+(e*(cos(f)))));
			G_T = -mu/pow(R_T,2);
			fprintf(lvlog,"R_T = %f (Expecting 6,563,366), V_T = %f (Expecting 7793.0429), ups_T = %f\r\n",R_T,V_T,ups_T);

			// G MATRIX CALCULATION
			MX_A.m11 = cos(phi_L);  MX_A.m12 = sin(phi_L)*sin(Azimuth); MX_A.m13 = -(sin(phi_L)*cos(Azimuth));
			MX_A.m21 = -sin(phi_L); MX_A.m22 = cos(phi_L)*sin(Azimuth); MX_A.m23 = -(cos(phi_L)*cos(Azimuth));
			MX_A.m31 = 0;  MX_A.m32 = cos(Azimuth);  MX_A.m33 = sin(Azimuth);

			MX_B.m11 = cos(DescNodeAngle); MX_B.m12 = 0; MX_B.m13 = sin(DescNodeAngle);
			MX_B.m21 = sin(DescNodeAngle)*sin(Inclination); MX_B.m22 = cos(Inclination); MX_B.m23 = -cos(DescNodeAngle)*sin(Inclination);
			MX_B.m31 = -sin(DescNodeAngle)*cos(Inclination); MX_B.m32 = sin(Inclination);MX_B.m33 = cos(DescNodeAngle)*cos(Inclination);

			MX_G = mul(MX_B,MX_A); // Matrix Multiply
		
			Y_u= -(PosS.x*MX_A.m21+PosS.y*MX_A.m22+PosS.z*MX_A.m23);//position component south of equator
			R = pow(pow(PosS.x,2)+pow(PosS.y,2)+pow(PosS.z,2),0.5);//instantaneous distance from earth's center
			S = (-mu/pow(R,3))*(1+J*pow(a/R,2)*(1-5*pow(Y_u/R,2)));
			P = (mu/pow(R,2))*pow(a/R,2) *((2*J*Y_u)/R);
			ddotG_last.x = PosS.x*S+MX_A.m21*P;//gravity acceleration vector
			ddotG_last.y = PosS.y*S+MX_A.m22*P;
			ddotG_last.z = PosS.z*S+MX_A.m23*P;
			PCommandedAttitude.x = (1.5* PI) + Azimuth;
			PCommandedAttitude.y = 0;
			PCommandedAttitude.z = 0;
			lvimu.zeropipacounters();
			sinceLastIGM = 0;
			init = true;
			goto minorloop;
			fprintf(lvlog,"Initialization completed.\r\n\r\n");
			};
		// various clocks the LVDC needs...
		if (TB7 > -100000){TB7 += simdt;}else{//timebases
			if (TB6 > -100000){TB6 += simdt;}else{
				if (TB5 > -100000){TB5 += simdt;}else{
					if (TB4 > -100000){TB4 += simdt;}else{
						if (TB3 > -100000){TB3 += simdt;}else{
							if (TB2 > -100000){TB2 += simdt;}else{
								if (TB1 > -100000){TB1 += simdt;}};};};};};};
		if(LVDC_GRR == true){TAS += simdt;};//time since GRR
		if(liftoff == true){t_clock += simdt;};//time since liftoff
		if(S2_IGNITION == true && t_21 == 0){t_21 = t_clock;};//I hope this is the right way to determine t_21; the boeing doc is silent on that
		sinceLastIGM += simdt;
		if(sinceLastIGM < 1.7){ goto minorloop;}
		dt_c = sinceLastIGM;
		sinceLastIGM = 0;
		IGMCycle++;				// For debugging
		fprintf(lvlog,"[%d+%f] *** Major Loop %d ***\r\n",LVDC_Timebase,LVDC_TB_ETime,IGMCycle);
		//powered flight nav
		if(LVDC_GRR == true){
		if(poweredflight == true){
		DotM_act.x += (lvimu.CDURegisters[LVRegPIPAX]);//read the PIPA CDUs
		DotM_act.y += (lvimu.CDURegisters[LVRegPIPAY]);
		DotM_act.z += (lvimu.CDURegisters[LVRegPIPAZ]);}
		Fm = pow((pow(((DotM_act.x - DotM_last.x)/dt_c),2)+ pow(((DotM_act.y - DotM_last.y)/dt_c),2)+ pow(((DotM_act.z - DotM_last.z)/dt_c),2)),0.5);
		PosS.x += (DotM_act.x + DotM_last.x) * dt_c / 2 + (DotG_last.x + ddotG_last.x * dt_c / 2)*dt_c;//position vector
		PosS.y += (DotM_act.y + DotM_last.y) * dt_c / 2 + (DotG_last.y + ddotG_last.y * dt_c / 2)*dt_c + 126.08 * dt_c;
		PosS.z += (DotM_act.z + DotM_last.z) * dt_c / 2 + (DotG_last.z + ddotG_last.z * dt_c / 2)*dt_c + 388.03 * dt_c;
		Y_u= -(PosS.x*MX_A.m21+PosS.y*MX_A.m22+PosS.z*MX_A.m23);//position component south of equator
		R = pow(pow(PosS.x,2)+pow(PosS.y,2)+pow(PosS.z,2),0.5);//instantaneous distance from earth's center
		S = (-mu/pow(R,3))*(1+J*pow(a/R,2)*(1-5*pow(Y_u/R,2)));
		P = (mu/pow(R,2))*pow(a/R,2) *((2*J*Y_u)/R);
		ddotG_act.x = PosS.x*S+MX_A.m21*P;//gravity acceleration vector
		ddotG_act.y = PosS.y*S+MX_A.m22*P;
		ddotG_act.z = PosS.z*S+MX_A.m23*P;
		CG = pow((pow(ddotG_act.x,2)+ pow(ddotG_act.y,2)+ pow(ddotG_act.z,2)),0.5);
		DotG_act.x = DotG_last.x + (ddotG_act.x  + ddotG_last.x) * dt_c / 2;//gravity velocity vector
		DotG_act.y = DotG_last.y + (ddotG_act.y  + ddotG_last.y) * dt_c / 2;
		DotG_act.z = DotG_last.z + (ddotG_act.z  + ddotG_last.z) * dt_c / 2;
		DotS.x = DotM_act.x + DotG_act.x;//total velocity vector 
		DotS.y = DotM_act.y + DotG_act.y + 126.08;
		DotS.z = DotM_act.z + DotG_act.z + 388.03;
		V = pow(pow(DotS.x,2)+pow(DotS.y,2)+pow(DotS.z,2),0.5);
		//save the 'actual' variables as 'last' variables for the next step
		DotM_last = DotM_act;
		DotG_last = DotG_act;
		ddotG_last = ddotG_act;
		fprintf(lvlog,"Navigation \r\n");
		fprintf(lvlog,"Inertial Attitude: %f %f %f \r\n",CurrentAttitude.x*DEG,CurrentAttitude.y*DEG,CurrentAttitude.z*DEG);
		fprintf(lvlog,"DotM: %f %f %f \r\n", DotM_act.x,DotM_act.y,DotM_act.z);
		fprintf(lvlog,"Gravity velocity: %f %f %f \r\n", DotG_act.x,DotG_act.y,DotG_act.z);
		fprintf(lvlog,"EarthRel Position: %f %f %f \r\n",PosS.x,PosS.y,PosS.z);
		fprintf(lvlog,"EarthRel Velocity: %f %f %f \r\n",DotS.x,DotS.y,DotS.z);
		fprintf(lvlog,"Sensed Acceleration: %f \r\n",Fm);	
		fprintf(lvlog,"Gravity Acceleration: %f \r\n",CG);	
		fprintf(lvlog,"Total Velocity: %f \r\n",V);
		fprintf(lvlog,"Dist. from Earth's Center: %f \r\n",R);
		fprintf(lvlog,"S: %f \r\n",S);
		fprintf(lvlog,"P: %f \r\n",P);
		lvimu.zeropipacounters();
		};
		if(liftoff == false){//liftoff not received; initial roll command for FCC
				CommandedAttitude.x =  (1.5* PI) + Azimuth;
				CommandedAttitude.y =  0;
				CommandedAttitude.z =  0;
				fprintf(lvlog,"[%d+%f] Initial roll command: %f\r\n",LVDC_Timebase,LVDC_TB_ETime,CommandedAttitude.x*DEG);
				goto minorloop;}
			if(BOOST == false){//i.e. we're either in orbit or boosting out of orbit
				if(TB7<0){
					if(TB5 > 20){goto orbitalguidance;}else{goto minorloop;};
					//if(TB6-T_IGM<0){goto restartprep;}else{goto IGM;};
				}else{goto orbitalguidance;};
			};//TBD: 
			if(directstageint == true){//direct stage interrupt update
				if(GATE4 == false){//check if we haven't gone through it before
									Tt_3 = Tt_3 + Cf * (V_S2T - V) + dT_cost;
									T_1 = 0;
									T_2 = 0;
									T_c = 0;
									T_1c = 0;
									Tt_T = Tt_3;
									ROV = ROVs;
									S4B_IGN = true;
									GATE4 = true;
									fprintf(lvlog,"[%d+%f] Direct stage interrupt received! Guidance update executed!\r\n",LVDC_Timebase,LVDC_TB_ETime);
									}
				if(TB4A-TS4BS < 0){goto minorloop;} else {goto IGM;};						
									}
			if (TB3-T_LET < 0){//Pre-IGM guidance:
				if(S1C_Engine_Out == true && T_EO1 == 0){//S1C engine out interrupt handling
														T_EO1 = 1;
														t_fail = t_clock;
														fprintf(lvlog,"[%d+%f] S1C engine out interrupt received! t_fail = %f\r\n",LVDC_Timebase,LVDC_TB_ETime,t_fail);
														}
				
				if (Position[0] > 137 || t_clock > t_1){//roll/pitch program
					if (t_clock >= t_2 && T_EO1 > 0){//time to re-calculate freeze time?
														T_EO1 = -1; //allow only one pass thru re-calculation
														if (t_fail <= t_2){dT_F = t_3;}
														if (t_2 < t_clock && t_clock <= t_4){dT_F = (B_11 * t_fail) + B_12;};
														if (t_4 < t_clock && t_clock <= t_5){dT_F = (B_21 * t_fail) + B_22;};
														if (t_5 < t_clock){dT_F = 0;};
														t_6 = t_clock + dT_F;
														T_ar = T_ar + (0.25*(T_ar - t_fail));
														fprintf(lvlog,"[%d+%f] Freeze time recalculated! t_6 = %f T_ar = %f\r\n",LVDC_Timebase,LVDC_TB_ETime,t_6,T_ar);
														}
					if (t_clock >= t_6){
						if (t_clock > T_ar){//time for pitch freeze?
											if (S2_IGNITION && S2_ENGINE_OUT && T_EO2 == 0){//pre-IGM S2 engine fail handling
											T_EO2 = 1;
											T_0 = t_21 + dt_LET - t_clock;
											T_1 = (T_0/4) + ((5 * T_1)/4);
											T_2 = 5 * T_2 / 4;
											tau3 = 5 * tau2 / 4;
											CommandedAttitude.y = PCommandedAttitude.y;
											CommandedAttitude.x = 360 * RAD;
											CommandedAttitude.z = 0;
											fprintf(lvlog,"[%d+%f] Pre-IGM SII engine out interrupt received!\r\n",LVDC_Timebase,LVDC_TB_ETime);
											goto minorloop;
											}else{fprintf(lvlog,"[%d+%f] Pitch freeze! \r\n",LVDC_Timebase,LVDC_TB_ETime,T_ar);
												  CommandedAttitude.y = PCommandedAttitude.y;
												  CommandedAttitude.x = 360 * RAD;
												  CommandedAttitude.z = 0;
												  goto minorloop;};
						}else{//Pre-IGM steering
								int x=0,y=0;						
								double Tc = t_clock - dT_F, cmd = 0;  
								if(Tc < T_S1){               x = 1; }
								if(T_S1 <= Tc && Tc < T_S2){ x = 2; }
								if(T_S2 <= Tc && Tc < T_S3){ x = 3; }
								if(T_S3 <= Tc){              x = 4; }
								while(y <= 4){
								cmd += (Fx[x][y] * ((double)pow(Tc,y)));
									y++;
								};
								CommandedAttitude.y = cmd * RAD;
								CommandedAttitude.x = 360 * RAD;
								CommandedAttitude.z = 0;
								fprintf(lvlog,"[%d+%f] Roll/pitch programm %f \r\n",LVDC_Timebase,LVDC_TB_ETime,cmd);
								goto minorloop;};
					}else{CommandedAttitude.y = PCommandedAttitude.y;
						  CommandedAttitude.x = 360 * RAD;
						  CommandedAttitude.z = 0;
						  goto minorloop;};
				}
														else{//S-IC yaw maneuver
															fprintf(lvlog,"[%d+%f] Yaw maneuver\r\n",LVDC_Timebase,LVDC_TB_ETime);
															if(1 <= t_clock && t_clock < 8.75)//yaw command issued between t +1s and t+8.75s
																{CommandedAttitude.z = 1.25*RAD;}
															else{CommandedAttitude.z = 0;};
															CommandedAttitude.y = 0;//no pitch
															CommandedAttitude.x = (1.5* PI) + Azimuth;//no roll
															goto minorloop;
															}
			}//end of pre igm
IGM:		if(HSL == false){		// If we are not in the high-speed loop
				fprintf(lvlog,"HSL False\r\n");
				// IGM STAGE LOGIC
				// TBD: No S4B Relight				
				if(S4B_IGN == true){
									fprintf(lvlog,"S-IVB 1st BURN\n");
									if (Ct >= Ct_o){
													tau3 = V_ex3/Fm;
													fprintf(lvlog,"Normal Tau: tau3 = %f, F = %f, m = %f \r\n",tau3,GetThrusterMax(th_main[0])*GetThrusterLevel(th_main[0]),GetMass());
									}else{
											tau3 = tau3N + (V_ex3/Fm - dt_c/2 - tau3N)*pow((Ct/Ct_o),4);
											tau3N = tau3N - dt_c;
											Ct = Ct + dt_c;
											fprintf(lvlog,"Art. Tau Mode 3: tau3 = %f, tau3N = %f, Ct = %f, Diff = %f\r\n",tau3,tau3N,Ct,tau3-V_ex3/Fm);								
								}
				GATE = false;//end chi freeze
				T_c = 0;
				T_2 = 0;
				T_1 = 0;
				fprintf(lvlog,"GATE = false, T_c = 0, T_1 = 0, T_2 = 0\r\n");
				goto chitilde;
				}
				if (S2_BURNOUT == true){
					fprintf(lvlog,"SII CUTOFF\n");
					if (T_c < 0){//this prevents T_c from getting negative in case of late SIVB ignition
								T_c = 0;
								T_2 = 0;
								T_1 = 0;
								fprintf(lvlog,"T_c = 0, T_1 = 0, T_2 = 0\r\n");
								goto chitilde;
					}else{//chi freeze, kill the first two stage integrals
						GATE = true;
						T_2 = 0;
						T_1 = 0;
						fprintf(lvlog,"GATE = true, T_1 = 0, T_2 = 0\r\n");
						goto chitilde;
					}				
				}
				if(S2_ENGINE_OUT == true && T_EO2 == 0){T_1 = 5 * T_1 / 4;
														T_2 = 5 * T_2 / 4;
														tau2 = 5 * tau2 / 4;
														T_EO2 = 1;
														};
				if(MRS == true){
					fprintf(lvlog,"Post-MRS\n");
					if(t_B1 <= t_B3){
						tau2 = V_ex2/Fm;
						fprintf(lvlog,"Normal Tau: tau2 = %f, F/m = %f, m = %f \r\n",tau2,Fm,GetMass());
					}else{
						// This is the "ARTIFICIAL TAU" code.
						t_B3 += dt_c; 
						tau2 = tau2+(T_1*(dotM_1/dotM_2));
						fprintf(lvlog,"Art. Tau: tau2 = %f, T_1 = %f, dotM_1 = %f dotM_2 = %f \r\n",tau2,T_1,dotM_1,dotM_2);
						fprintf(lvlog,"Diff: %f \r\n",(tau2-V_ex2/Fm));
					}
					// This T_2 test is also tested after T_1 < 0 etc etc
					if(T_2 > 0){
						T_2 = T_2+T_1*(dotM_1/dotM_2);
						T_1 = 0;
						fprintf(lvlog,"T_1 = 0\r\nT_2 = %f, dotM_1 = %f, dotM_2 = %f \r\n",T_2,dotM_1,dotM_2);
						// Go to CHI-TILDE LOGIC
					}else{
						T_2 = 0;
						T_1 = 0;
						fprintf(lvlog,"T_1 = 0, T_2 = 0\r\n");
						// Go to CHI-TILDE LOGIC
					}
					if(T_2 < 11){GATE = true;}//pre SIVB-staging chi-freeze
				}else{
					fprintf(lvlog,"Pre-MRS\n");
					if(T_1 < 0){	// If we're out of first-stage IGM time
						// Artificial Tau
						tau2 = tau2+(T_1*(dotM_1/dotM_2));
						fprintf(lvlog,"Art. Tau: tau2 = %f, T_1 = %f, dotM_1 = %f, dotM_2 = %f \r\n",tau2,T_1,dotM_1,dotM_2);
						if(T_2 > 0){
							T_2 = T_2+T_1*(dotM_1/dotM_2);
							T_1 = 0;
							fprintf(lvlog,"T_2 = %f, T_1 = %f, dotM_1 = %f, dotM_2 = %f \r\n",T_2,T_1,dotM_1,dotM_2);
						}else{
							T_2 = 0;
							T_1 = 0;
							fprintf(lvlog,"T_2 = 0\r\n");
						}					
					}else{															
						tau1 = V_ex1/Fm; 
						fprintf(lvlog,"Normal Tau: tau1 = %f, F/m = %f m = %f\r\n",tau1,Fm, GetMass());
					}
				}
				fprintf(lvlog,"--- STAGE INTEGRAL LOGIC ---\r\n");

				// CHI-TILDE LOGIC
				// STAGE INTEGRAL CALCULATIONS				
chitilde:		Pos4 = mul(MX_G,PosS);
				fprintf(lvlog,"Pos4 = %f, %f, %f\r\n",Pos4.x,Pos4.y,Pos4.z);
				fprintf(lvlog,"T_1 = %f,T_2 = %f\r\n",T_1,T_2);
				L_1 = V_ex1 * log(tau1 / (tau1-T_1));
				J_1 = (L_1 * tau1) - (V_ex1 * T_1);
				S_1 = (L_1 * T_1) - J_1;
				Q_1 = (S_1 * tau1) - ((V_ex1 * pow(T_1,2)) / 2);
				P_1 = (J_1 * tau1) - ((V_ex1 * pow(T_1,2)) / 2);
				U_1 = (Q_1 * tau1) - ((V_ex1 * pow(T_1,3)) / 6);
				fprintf(lvlog,"L_1 = %f, J_1 = %f, S_1 = %f, Q_1 = %f, P_1 = %f, U_1 = %f\r\n",L_1,J_1,S_1,Q_1,P_1,U_1);

				L_2 = V_ex2 * log(tau2 / (tau2-T_2));
				J_2 = (L_2 * tau2) - (V_ex2 * T_2);
				S_2 = (L_2 * T_2) - J_2;
				Q_2 = (S_2 * tau2) - ((V_ex2 * pow(T_2,2)) / 2);
				P_2 = (J_2 * tau2) - ((V_ex2 * pow(T_2,2)) / 2);
				U_2 = (Q_2 * tau2) - ((V_ex2 * pow(T_2,3)) / 6);
				fprintf(lvlog,"L_2 = %f, J_2 = %f, S_2 = %f, Q_2 = %f, P_2 = %f, U_2 = %f\r\n",L_2,J_2,S_2,Q_2,P_2,U_2);

				L_12 = L_1 + L_2;
				J_12 = J_1 + J_2 + (L_2 * T_1);
				S_12 = S_1 - J_2 + (L_12 * (T_2 + T_c));
				Q_12 = Q_1 + Q_2 + (S_2 * T_1) + (J_1 * T_2);
				P_12 = P_1 + P_2 + (T_1 * ((2 * J_2) + (L_2 * T_1)));
				U_12 = U_1 + U_2 + (T_1 * ((2 * Q_2) + (S_2 * T_1))) + (T_2 * P_1);
				fprintf(lvlog,"L_12 = %f, J_12 = %f, S_12 = %f, Q_12 = %f, P_12 = %f, U_12 = %f\r\n",L_12,J_12,S_12,Q_12,P_12,U_12);


				Lt_3 = V_ex3 * log(tau3 / (tau3-Tt_3));
				fprintf(lvlog,"Lt_3 = %f, tau3 = %f, Tt_3 = %f\r\n",Lt_3,tau3,Tt_3);

				Jt_3 = (Lt_3 * tau3) - (V_ex3 * Tt_3);
				fprintf(lvlog,"Jt_3 = %f",Jt_3);
				Lt_Y = (L_12 + Lt_3);
				fprintf(lvlog,", Lt_Y = %f\r\n",Lt_Y);

				// SELECT RANGE OPTION				
gtupdate:		// Target of jump from further down
				fprintf(lvlog,"--- GT UPDATE ---\r\n");

				if(Tt_T <= eps_1){
					// RANGE ANGLE 2 (out-of orbit)
					fprintf(lvlog,"RANGE ANGLE 2\r\n");
					sprintf(oapiDebugString(),"LVDC: RANGE ANGLE 2: %f %f",Tt_T,eps_1); LVDC_GP_PC = 30; // STOP
					sin_ups = ((PosS.x*DotS.x)+(PosS.y*DotS.y)+(PosS.z*DotS.z))/R*V;
					cos_ups = pow(1-pow(sin_ups,2),0.5);
					dot_phi_1 = (V*cos_ups)/R;
					dot_phi_T = (V_T*cos(ups_T))/R_T;
					phi_T = ((atan(Pos4.z/Pos4.x))+(((dot_phi_1+dot_phi_T)/2)*Tt_T));
				}else{
					// RANGE ANGLE 1 (into orbit)
					fprintf(lvlog,"RANGE ANGLE 1\r\n");
					d2 = (V * Tt_T) - Jt_3 + (Lt_Y * Tt_3) - (ROV / V_ex3) * 
						((tau1 - T_1) * L_1 + (tau2 - T_2) * L_2 + (tau3 - Tt_3) * Lt_3) *
						(Lt_Y + V - V_T);
					phi_T = ((atan2(Pos4.z,Pos4.x))+(((1/R_T)*(S_12+d2))*(cos(ups_T))));
					fprintf(lvlog,"V = %f, d2 = %f, phi_T = %f\r\n",V,d2,phi_T);
				}
				// FREEZE TERMINAL CONDITIONS TEST
				if(!(Tt_T <= eps_3)){
					// UPDATE TERMINAL CONDITIONS
					fprintf(lvlog,"UPDATE TERMINAL CONDITIONS\r\n");
					f = phi_T + alpha_D;
					R_T = p/(1+((e*(cos(f)))));
					fprintf(lvlog,"f = %f, R_T = %f\r\n",f,R_T);
					V_T = K_5 * pow(1+((2*e)*(cos(f)))+pow(e,2),0.5);
					ups_T = atan2((e*(sin(f))),(1+(e*(cos(f)))));
					G_T = -mu/pow(R_T,2);
					fprintf(lvlog,"V_T = %f, ups_T = %f, G_T = %f\r\n",V_T,ups_T,G_T);
				}
				// ROT TEST
				if(ROT){
					// ROTATED TERMINAL CONDITIONS (out-of-orbit)
					fprintf(lvlog,"ROTATED TERMINAL CONDITIONS\r\n");
					sprintf(oapiDebugString(),"LVDC: ROTATED TERMINAL CNDS"); LVDC_GP_PC = 30; // STOP
				}else{
					// UNROTATED TERMINAL CONDITIONS (into-orbit)
					fprintf(lvlog,"UNROTATED TERMINAL CONDITIONS\r\n");
					xi_T = R_T;					
					dot_zeta_T = V_T * (cos(ups_T));
					dot_xi_T = V_T * (sin(ups_T));
					ddot_zeta_GT = 0;
					ddot_xi_GT = G_T;
					fprintf(lvlog,"xi_T = %f, dot_zeta_T = %f, dot_xi_T = %f\r\n",xi_T,dot_zeta_T,dot_xi_T);
					fprintf(lvlog,"ddot_zeta_GT = %f, ddot_xi_GT = %f\r\n",ddot_zeta_GT,ddot_xi_GT);
				}
				// ROTATION TO TERMINAL COORDINATES
				fprintf(lvlog,"--- ROTATION TO TERMINAL COORDINATES ---\r\n");
				// This is the last time PosS is referred to.
				MX_phi_T.m11 = (cos(phi_T));    MX_phi_T.m12 = 0; MX_phi_T.m13 = ((sin(phi_T)));
				MX_phi_T.m21 = 0;               MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
				MX_phi_T.m31 = (-sin(phi_T)); MX_phi_T.m32 = 0; MX_phi_T.m33 = (cos(phi_T));
				fprintf(lvlog,"MX_phi_T R1 = %f %f %f\r\n",MX_phi_T.m11,MX_phi_T.m12,MX_phi_T.m13);
				fprintf(lvlog,"MX_phi_T R2 = %f %f %f\r\n",MX_phi_T.m21,MX_phi_T.m22,MX_phi_T.m23);
				fprintf(lvlog,"MX_phi_T R3 = %f %f %f\r\n",MX_phi_T.m31,MX_phi_T.m32,MX_phi_T.m33);

				MX_K = mul(MX_phi_T,MX_G);
				fprintf(lvlog,"MX_K R1 = %f %f %f\r\n",MX_K.m11,MX_K.m12,MX_K.m13);
				fprintf(lvlog,"MX_K R2 = %f %f %f\r\n",MX_K.m21,MX_K.m22,MX_K.m23);
				fprintf(lvlog,"MX_K R3 = %f %f %f\r\n",MX_K.m31,MX_K.m32,MX_K.m33);

				PosXEZ = mul(MX_K,PosS);
				DotXEZ = mul(MX_K,DotS);	
				fprintf(lvlog,"PosXEZ = %f %f %f\r\n",PosXEZ.x,PosXEZ.y,PosXEZ.z);
				fprintf(lvlog,"DotXEZ = %f %f %f\r\n",DotXEZ.x,DotXEZ.y,DotXEZ.z);

				VECTOR3 RTT_T1,RTT_T2;
				RTT_T1.x = ddot_xi_GT; RTT_T1.y = 0;        RTT_T1.z = ddot_zeta_GT;
				RTT_T2 = ddotG_act;
				fprintf(lvlog,"RTT_T1 = %f %f %f\r\n",RTT_T1.x,RTT_T1.y,RTT_T1.z);
				fprintf(lvlog,"RTT_T2 = %f %f %f\r\n",RTT_T2.x,RTT_T2.y,RTT_T2.z);

				RTT_T2 = mul(MX_K,RTT_T2);
				fprintf(lvlog,"RTT_T2 (mul) = %f %f %f\r\n",RTT_T2.x,RTT_T2.y,RTT_T2.z);

				RTT_T1 = RTT_T1+RTT_T2;	  
				fprintf(lvlog,"RTT_T1 (add) = %f %f %f\r\n",RTT_T1.x,RTT_T1.y,RTT_T1.z);

				ddot_xi_G   = 0.5*RTT_T1.x;
				ddot_eta_G  = 0.5*RTT_T1.y;
				ddot_zeta_G = 0.5*RTT_T1.z;
				fprintf(lvlog,"ddot_XEZ_G = %f %f %f\r\n",ddot_xi_G,ddot_eta_G,ddot_zeta_G);

				// ESTIMATED TIME-TO-GO
				fprintf(lvlog,"--- ESTIMATED TIME-TO-GO ---\r\n");

				dot_dxit   = dot_xi_T - DotXEZ.x - (ddot_xi_G*Tt_T);
				dot_detat  = -DotXEZ.y - (ddot_eta_G * Tt_T);
				dot_dzetat = dot_zeta_T - DotXEZ.z - (ddot_zeta_G * Tt_T);
				fprintf(lvlog,"dot_XEZt = %f %f %f\r\n",dot_dxit,dot_detat,dot_dzetat);
				dV = pow((pow(dot_dxit,2)+pow(dot_detat,2)+pow(dot_dzetat,2)),0.5);
				dL_3 = (((pow(dot_dxit,2)+pow(dot_detat,2)+pow(dot_dzetat,2))/Lt_Y)-Lt_Y)/2;
				// if(dL_3 < 0){ sprintf(oapiDebugString(),"Est TTG: dL_3 %f (X/E/Z %f %f %f) @ Cycle %d (TB%d+%f)",dL_3,dot_dxit,dot_detat,dot_dzetat,IGMCycle,LVDC_Timebase,LVDC_TB_ETime);
				//	LVDC_GP_PC = 30; break; } 

				dT_3 = (dL_3*(tau3-Tt_3))/V_ex3;
				T_3 = Tt_3 + dT_3;
				T_T = Tt_T + dT_3;
				fprintf(lvlog,"dV = %f, dL_3 = %f, dT_3 = %f, T_3 = %f, T_T = %f\r\n",dV,dL_3,dT_3,T_3,T_T);

				// TARGET PARAMETER UPDATE
				if(!(UP > 0)){	
					fprintf(lvlog,"--- TARGET PARAMETER UPDATE ---\r\n");
					UP = 1; 
					Tt_3 = T_3;
					Tt_T = T_T;
					fprintf(lvlog,"UP = 1, Tt_3 = %f, Tt_T = %f\r\n",Tt_3,Tt_T);
					Lt_3 = Lt_3 + dL_3;
					Lt_Y = Lt_Y + dL_3;
					Jt_3 = Jt_3 + (dL_3*T_3);
					fprintf(lvlog,"Lt_3 = %f, Lt_Y = %f, Jt_3 = %f\r\n",Lt_3,Lt_Y,Jt_3);

					// NOTE: This is perfectly valid. Just because Dijkstra and Wirth think otherwise
					// does not mean it's gospel. I shouldn't have to defend my choice of instructions
					// because a bunch of people read the title of the paper with no context and take
					// it as a direct revelation from God with no further study into the issue.
					fprintf(lvlog,"RECYCLE\r\n");
					goto gtupdate; // Recycle. 
				}

				// tchi_y AND tchi_p CALCULATIONS
				fprintf(lvlog,"--- tchi_y/p CALCULATION ---\r\n");

				L_3 = Lt_3 + dL_3;
				J_3 = Jt_3 + (dL_3*T_3);
				S_3 = (L_3*T_3)-J_3;
				Q_3 = (S_3*tau3)-((V_ex3*pow(T_3,2))/2);
				P_3 = (J_3*(tau3+(2*T_1c)))-((V_ex3*pow(T_3,2))/2);
				U_3 = (Q_3*(tau3+(2*T_1c)))-((V_ex3*pow(T_3,3))/6);
				fprintf(lvlog,"L_3 = %f, J_3 = %f, S_3 = %f, Q_3 = %f, P_3 = %f, U_3 = %f\r\n",L_3,J_3,S_3,Q_3,P_3,U_3);

				// This is where velocity-to-be-gained is generated.

				dot_dxi   = dot_dxit   - (ddot_xi_G   * dT_3);
				dot_deta  = dot_detat  - (ddot_eta_G  * dT_3);
				dot_dzeta = dot_dzetat - (ddot_zeta_G * dT_3);
				fprintf(lvlog,"dot_dXEZ = %f %f %f\r\n",dot_dxi,dot_deta,dot_dzeta);

				
//				sprintf(oapiDebugString(),".dxi = %f | .deta %f | .dzeta %f | dT3 %f",
//					dot_dxi,dot_deta,dot_dzeta,dT_3);

				L_Y = L_12 + L_3;
				tchi_y_last = tchi_y;
				tchi_p_last = tchi_p;
				tchi_y = atan2(dot_deta,pow(pow(dot_dxi,2)+pow(dot_dzeta,2),0.5));
				tchi_p = atan2(dot_dxi,dot_dzeta);				
				UP = -1;
				fprintf(lvlog,"L_Y = %f, tchi_y = %f, tchi_p = %f, UP = -1\r\n",L_Y,tchi_y,tchi_p);

				// *** END OF CHI-TILDE LOGIC ***
				// Is it time for chi-tilde mode?
				if(Tt_T <= eps_2){
					fprintf(lvlog,"CHI BAR STERRING ON, REMOVE ALTITUDE CONSTRAINS (K_1-4 = 0)\r\n");
					// Yes
					// Go to the test that we would be testing if HSL was true
					K_1 = 0; K_2 = 0; K_3 = 0; K_4 = 0;
					// See the note above if the presence of this goto bothers you.
					// sprintf(oapiDebugString(),"LVDC: HISPEED LOOP ENTRY: Tt_T %f eps_2 %f", Tt_T,eps_2); LVDC_GP_PC = 30; break; // STOP
					goto hsl;
				}else{
					// No.
					// YAW STEERING PARAMETERS
					fprintf(lvlog,"--- YAW STEERING PARAMETERS ---\r\n");

					J_Y = J_12 + J_3 + (L_3*T_1c);
					S_Y = S_12 - J_3 + (L_Y*T_3);
					Q_Y = Q_12 + Q_3 + (S_3*T_1c) + ((T_c+T_3)*J_12);
					K_Y = L_Y/J_Y;
					D_Y = S_Y - (K_Y*Q_Y);
					fprintf(lvlog,"J_Y = %f, S_Y = %f, Q_Y = %f, K_Y = %f, D_Y = %f\r\n",J_Y,S_Y,Q_Y,K_Y,D_Y);

					deta = PosXEZ.y + (DotXEZ.y*T_T) + ((ddot_eta_G*pow(T_T,2))/2) + (S_Y*(sin(tchi_y)));
					K_3 = deta/(D_Y*(cos(tchi_y)));
					K_4 = K_Y*K_3;
					fprintf(lvlog,"deta = %f, K_3 = %f, K_4 = %f\r\n",deta,K_3,K_4);

					// PITCH STEERING PARAMETERS
					fprintf(lvlog,"--- PITCH STEERING PARAMETERS ---\r\n");

					L_P = L_Y*cos(tchi_y);
					C_2 = cos(tchi_y)+(K_3*sin(tchi_y));
					C_4 = K_4*sin(tchi_y);
					J_P = (J_Y*C_2) - (C_4*(P_12+P_3+(pow(T_1c,2)*L_3)));
					fprintf(lvlog,"L_P = %f, C_2 = %f, C_4 = %f, J_P = %f\r\n",L_P,C_2,C_4,J_P);

					S_P = (S_Y*C_2) - (C_4*Q_Y);
					Q_P = (Q_Y*C_2) - (C_4*(U_12+U_3+(pow(T_1c,2)*S_3)+((T_3+T_c)*P_12)));
					K_P = L_P/J_P;
					D_P = S_P - (K_P*Q_P);
					fprintf(lvlog,"S_P = %f, Q_P = %f, K_P = %f, D_P = %f\r\n",S_P,Q_P,K_P,D_P);

					dxi = PosXEZ.x - xi_T + (DotXEZ.x*T_T) + ((ddot_xi_G*pow(T_T,2))/2) + (S_P*(sin(tchi_p)));
					K_1 = dxi/(D_P*cos(tchi_p));
					K_2 = K_P*K_1;
					fprintf(lvlog,"dxi = %f, K_1 = %f, K_2 = %f, cos(tchi_p) = %f\r\n",dxi,K_1,K_2,cos(tchi_p));
				}
			}else{
hsl:			// HIGH-SPEED LOOP ENTRY				
				// CUTOFF VELOCITY EQUATIONS
				fprintf(lvlog,"--- CUTOFF VELOCITY EQUATIONS ---\r\n");
				V_0 = V_1;
				V_1 = V_2;
				//V_2 = 0.5 * (V+(pow(V_1,2)/V));
				V_2 = V;
				dtt_1 = dtt_2;
				dtt_2 = dt_c;					
				fprintf(lvlog,"V = %f, Tt_t = %f\r\n",V,Tt_T);
				fprintf(lvlog,"V = %f, V_0 = %f, V_1 = %f, V_2 = %f, dtt_1 = %f, dtt_2 = %f\r\n",V,V_0,V_1,V_2,dtt_1,dtt_2);
				if(Tt_T <= eps_4 && V + V_TC >= V_T){
					fprintf(lvlog,"--- HI SPEED LOOP ---\r\n");
					// TGO CALCULATION
					fprintf(lvlog,"--- TGO CALCULATION ---\r\n");
					if(GATE5 == false){
						fprintf(lvlog,"CHI FREEZE\r\n");
						// CHI FREEZE
						tchi_y = tchi_y_last;
						tchi_p = tchi_p_last;
						GATE = true;
						HSL = true;
						GATE5 = true;
						T_GO = T_3;
						fprintf(lvlog,"HSL = true, GATE5 = true, T_GO = %f\r\n",T_GO);
					}
					if(BOOST == true){
						fprintf(lvlog,"BOOST-TO-ORBIT ACTIVE\r\n");
						// dT_4 CALCULATION
						t_3i = TB4+T_c;
						dT_4 = TAS-t_3i-T_4N;
						fprintf(lvlog,"t_3i = %f, dT_4 = %f\r\n",t_3i,dT_4);
						if(fabs(dT_4) <= dT_LIM){							
							dTt_4 = dT_4;
						}else{
							fprintf(lvlog,"dTt_4 CLAMPED\r\n");
							dTt_4 = dT_LIM;
						}
						fprintf(lvlog,"dTt_4 = %f\r\n",dTt_4);
					}else{
						// TRANSLUNAR INJECTION VELOCITY
						fprintf(lvlog,"TRANSLUNAR INJECTION\r\n");
						sprintf(oapiDebugString(),"LVDC: HISPEED LOOP, TLI VELOCITY: %f %f %f %f %f",
							Tt_T,eps_4,V,V_TC,V_T); LVDC_GP_PC = 30; // STOP
					}
					// TGO DETERMINATION
					fprintf(lvlog,"--- TGO DETERMINATION ---\r\n");

					a_2 = (((V_2-V_1)*dtt_1)-((V_1-V_0)*dtt_2))/(dtt_2*dtt_1*(dtt_2+dtt_1));
					a_1 = ((V_2-V_1)/dtt_2)+(a_2*dtt_2);
					T_GO = ((V_T-dV_B)-V_2)/(a_1+a_2*T_GO);
					T_CO = TAS+T_GO;
					fprintf(lvlog,"a_2 = %f, a_1 = %f, T_GO = %f, T_CO = %f, V_T = %f\r\n",a_2,a_1,T_GO,T_CO,V_T);

					// S4B CUTOFF?
					if(S4B_IGN == false){
						fprintf(lvlog,"*** HSL EXIT SETTINGS ***\r\n");
						GATE5 = false;
						Tt_T = 1000;
						HSL = false;
						BOOST = false;
						goto minorloop;
					}
					// Done, go to navigation
					sprintf(oapiDebugString(),"TB%d+%f | CP/Y %f %f | -HSL- TGO %f",
						LVDC_Timebase,LVDC_TB_ETime,PITCH,YAW,T_GO);

					goto minorloop;
				}
			}
			// GUIDANCE TIME UPDATE
			fprintf(lvlog,"--- GUIDANCE TIME UPDATE ---\r\n");

			if(BOOST){
				if(S4B_IGN){		T_3 = T_3 - dt_c; }else{
				if(S2_BURNOUT){		T_c = T_c - dt_c; }else{
				if(MRS == false){	T_1 = T_1 - dt_c; }else{
				if(t_B1 <= t_B3){	T_2 = T_2 - dt_c; }else{
									// Here if t_B1 is bigger.
									fprintf(lvlog,"t_B1 = %f, t_B3 = %f\r\n",t_B1,t_B3);
									T_1 = (((dotM_1*(t_B3-t_B1))-(dotM_2*t_B3))*dt)/(dotM_1*t_B1);
				}}}}
				fprintf(lvlog,"T_1 = %f, T_2 = %f, T_3 = %f, T_c = %f dt_c = %f\r\n",T_1,T_2,T_3,T_c,dt_c);
			}else{
				// MRS TEST
				fprintf(lvlog,"MRS TEST\r\n");
				sprintf(oapiDebugString(),"LVDC: MRS TEST"); LVDC_GP_PC = 30; // STOP
			}

			Tt_3 = T_3;
			T_1c = T_1+T_2+T_c;			
			Tt_T = T_1c+Tt_3;
			fprintf(lvlog,"Tt_3 = %f, T_1c = %f, Tt_T = %f\r\n",Tt_3,T_1c,Tt_T);
			if(GATE){
				// FREEZE CHI
				fprintf(lvlog,"Thru GATE; CHI FREEZE\r\n");
				sprintf(oapiDebugString(),"LVDC: CHI FREEZE");
				goto minorloop;
				}else{
				// IGM STEERING ANGLES
				fprintf(lvlog,"--- IGM STEERING ANGLES ---\r\n");

				//sprintf(oapiDebugString(),"IGM: K_1 %f K_2 %f K_3 %f K_4 %f",K_1,K_2,K_3,K_4);
				Xtt_y = ((tchi_y) - K_3 + (K_4 * t));
				Xtt_p = ((tchi_p) - K_1 + (K_2 * t));
				fprintf(lvlog,"Xtt_y = %f, Xtt_p = %f\r\n",Xtt_y,Xtt_p);

				// -- COMPUTE INVERSE OF [K] --
				// Get Determinate
				double det = MX_K.m11 * ((MX_K.m22*MX_K.m33) - (MX_K.m32*MX_K.m23))
						   - MX_K.m12 * ((MX_K.m21*MX_K.m33) - (MX_K.m31*MX_K.m23))
						   + MX_K.m13 * ((MX_K.m21*MX_K.m32) - (MX_K.m31*MX_K.m22));
				// If the determinate is less than 0.0005, this is invalid.
				fprintf(lvlog,"det = %f (LESS THAN 0.0005 IS INVALID)\r\n",det);

				MATRIX3 MX_Ki; // TEMPORARY: Inverse of [K]
				MX_Ki.m11 =   ((MX_K.m22*MX_K.m33) - (MX_K.m23*MX_K.m32))  / det;
				MX_Ki.m12 =   ((MX_K.m13*MX_K.m32) - (MX_K.m12*MX_K.m33))  / det;
				MX_Ki.m13 =   ((MX_K.m12*MX_K.m23) - (MX_K.m13*MX_K.m22))  / det;
				MX_Ki.m21 =   ((MX_K.m23*MX_K.m31) - (MX_K.m21*MX_K.m33))  / det;
				MX_Ki.m22 =   ((MX_K.m11*MX_K.m33) - (MX_K.m13*MX_K.m31))  / det;
				MX_Ki.m23 =   ((MX_K.m13*MX_K.m21) - (MX_K.m11*MX_K.m23))  / det;
				MX_Ki.m31 =   ((MX_K.m21*MX_K.m32) - (MX_K.m22*MX_K.m31))  / det;
				MX_Ki.m32 =   ((MX_K.m12*MX_K.m31) - (MX_K.m11*MX_K.m32))  / det;
				MX_Ki.m33 =   ((MX_K.m11*MX_K.m22) - (MX_K.m12*MX_K.m21))  / det;
				fprintf(lvlog,"MX_Ki R1 = %f %f %f\r\n",MX_Ki.m11,MX_Ki.m12,MX_Ki.m13);
				fprintf(lvlog,"MX_Ki R2 = %f %f %f\r\n",MX_Ki.m21,MX_Ki.m22,MX_Ki.m23);
				fprintf(lvlog,"MX_Ki R3 = %f %f %f\r\n",MX_Ki.m31,MX_Ki.m32,MX_Ki.m33);

				// Done
				VECTOR3 VT; 
				VT.x = (sin(Xtt_p)*cos(Xtt_y));
				VT.y = (sin(Xtt_y));
				VT.z = (cos(Xtt_p)*cos(Xtt_y));
				fprintf(lvlog,"VT (set) = %f %f %f\r\n",VT.x,VT.y,VT.z);

				VT = mul(MX_Ki,VT);
				fprintf(lvlog,"VT (mul) = %f %f %f\r\n",VT.x,VT.y,VT.z);

				X_S1 = VT.x;
				X_S2 = VT.y;
				X_S3 = VT.z;
				fprintf(lvlog,"X_S1-3 = %f %f %f\r\n",X_S1,X_S2,X_S3);

				// FINALLY - COMMANDS!
				X_Zi = asin(X_S2);			// Yaw
				X_Yi = atan2(-X_S3,X_S1);	// Pitch
				fprintf(lvlog,"*** COMMAND ISSUED ***\r\n");
				fprintf(lvlog,"PITCH = %f, YAW = %f\r\n\r\n",X_Yi*DEG,X_Zi*DEG);
				// IGM is supposed to generate attitude directly.
				CommandedAttitude.x = 360 * RAD;    // ROLL
				CommandedAttitude.y = X_Yi; // PITCH
				CommandedAttitude.z = X_Zi; // YAW;	
				goto minorloop;
	}
			
			

				


orbitalguidance: //orbital guidance logic;

	fprintf(lvlog,"*** ORBITAL GUIDANCE ***\r\n");
	if(TB7<0){
		if(TB6<0){
			if(TB5-TA1<0){//presettings for orbital maneuver; don't know if we ever need them, but at least it's there...
				if(TB5-TA2<0){
					if(INH2){alpha_1 = -20 * RAD;
							 CommandedAttitude.x = 180 * RAD;
							 goto orbatt;
					}else{alpha_1 = 0 * RAD;
						  CommandedAttitude.x = 360 * RAD;
						  goto orbatt;};
				}else{if(INH1){alpha_1 = 0 * RAD;
							 CommandedAttitude.x = 360 * RAD;
							 goto orbatt;
					}else{alpha_1 = -20 * RAD;
						  CommandedAttitude.x = 180 * RAD;
						  goto orbatt;};};
			}else{alpha_1 = 0 * RAD;
				  CommandedAttitude.x = 360 * RAD;
				  goto orbatt;};
		}else{if(first_op){alpha_1 = K_P1 + K_P2 * dTt_4;//restart angles
						   alpha_2 = K_Y1 + K_Y2 * dTt_4;
						   goto orbatt;
						   }else{alpha_1 = 0 * RAD;
							     CommandedAttitude.x = 360 * RAD;
								 goto orbatt;}
				};
	}else{CommandedAttitude = XLunarAttitude;};//attitude for T&D					
	goto minorloop;

orbatt: Pos4 = mul(MX_G,PosS);//here we compute the steering angles...
		sin_chi_Yit = (Pos4.x * cos(alpha_1) + Pos4.z * sin(alpha_1))/(-R);
		cos_chi_Yit = (Pos4.z * cos(alpha_1) - Pos4.x * sin(alpha_1))/(-R);
		sin_chi_Zit = sin(alpha_2);
		cos_chi_Zit = cos(alpha_2);
		// -- COMPUTE INVERSE OF [G] -what an effort for those stupid angles!
		// Get Determinate
		double det = MX_G.m11 * ((MX_G.m22*MX_G.m33) - (MX_G.m32*MX_G.m23))
				   - MX_G.m12 * ((MX_G.m21*MX_G.m33) - (MX_G.m31*MX_G.m23))
				   + MX_G.m13 * ((MX_G.m21*MX_G.m32) - (MX_G.m31*MX_G.m22));
		// If the determinate is less than 0.0005, this is invalid.
		MATRIX3 MX_Gi; // TEMPORARY: Inverse of [K]
		MX_Gi.m11 =   ((MX_G.m22*MX_G.m33) - (MX_G.m23*MX_G.m32))  / det;
		MX_Gi.m12 =   ((MX_G.m13*MX_G.m32) - (MX_G.m12*MX_G.m33))  / det;
		MX_Gi.m13 =   ((MX_G.m12*MX_G.m23) - (MX_G.m13*MX_G.m22))  / det;
		MX_Gi.m21 =   ((MX_G.m23*MX_G.m31) - (MX_G.m21*MX_G.m33))  / det;
		MX_Gi.m22 =   ((MX_G.m11*MX_G.m33) - (MX_G.m13*MX_G.m31))  / det;
		MX_Gi.m23 =   ((MX_G.m13*MX_G.m21) - (MX_G.m11*MX_G.m23))  / det;
		MX_Gi.m31 =   ((MX_G.m21*MX_G.m32) - (MX_G.m22*MX_G.m31))  / det;
		MX_Gi.m32 =   ((MX_G.m12*MX_G.m31) - (MX_G.m11*MX_G.m32))  / det;
		MX_Gi.m33 =   ((MX_G.m11*MX_G.m22) - (MX_G.m12*MX_G.m21))  / det;
		VECTOR3 VT; 
		VT.x = (cos_chi_Yit * cos_chi_Zit);
		VT.y = (sin_chi_Zit);
		VT.z = (-sin_chi_Yit * cos_chi_Zit);
		fprintf(lvlog,"VT (set) = %f %f %f\r\n",VT.x,VT.y,VT.z);

		VT = mul(MX_Gi,VT);
		fprintf(lvlog,"VT (mul) = %f %f %f\r\n",VT.x,VT.y,VT.z);

		X_S1 = VT.x;
		X_S2 = VT.y;
		X_S3 = VT.z;
		fprintf(lvlog,"X_S1-3 = %f %f %f\r\n",X_S1,X_S2,X_S3);

		// FINALLY - COMMANDS!
		X_Zi = asin(X_S2);			// Yaw
		X_Yi = atan2(-X_S3,X_S1);	// Pitch
		fprintf(lvlog,"*** COMMAND ISSUED ***\r\n");
		fprintf(lvlog,"PITCH = %f, YAW = %f\r\n\r\n",X_Yi*DEG,X_Zi*DEG);
		CommandedAttitude.y = X_Yi; // PITCH
		CommandedAttitude.z = X_Zi; // YAW;				
		goto minorloop;

restartprep://TLI restart & targeting logic; TBD;
	

	
minorloop://minor loop; TBD: move IGM steering angles & HSL logic here
	if (T_GO - sinceLastIGM <= 0 && HSL == true && S4B_IGN == true){//Time for S4B cutoff? We need to check that here -IGM runs every 2 sec only, but cutoff has to be on the second
		SetThrusterLevel(th_main[0], 0);
		S4B_IGN = false;
		TB5 = - simdt;
		LVDC_Timebase = 5;
		LVDC_TB_ETime = 0;
		fprintf(lvlog,"SIVB CUTOFF! TAS = %f \r\n",TAS);
		};
	
	
	if(CommandedAttitude.z < -45 * RAD){CommandedAttitude.z = -45 * RAD;}//yaw limits
	if(CommandedAttitude.z > 45 * RAD){CommandedAttitude.z = 45 * RAD;}
	double diff;//aux variable for limit test
	diff = fmod((CommandedAttitude.x - PCommandedAttitude.x + TWO_PI),TWO_PI);
	if (diff > PI) {diff -= TWO_PI;} ;
	if(abs(diff/dt_c) > CommandRateLimits.x){
		if(diff > 0){
			CommandedAttitude.x = PCommandedAttitude.x + CommandRateLimits.x * dt_c;}
		else{CommandedAttitude.x = PCommandedAttitude.x - CommandRateLimits.x * dt_c;}}
	diff = fmod((CommandedAttitude.y - PCommandedAttitude.y + TWO_PI),TWO_PI);
	if (diff > PI) {diff -= TWO_PI;} ;
	if(abs(diff/dt_c) > CommandRateLimits.y){
		if(diff > 0){
			CommandedAttitude.y = PCommandedAttitude.y + CommandRateLimits.y * dt_c;}
		else{CommandedAttitude.y = PCommandedAttitude.y - CommandRateLimits.y * dt_c;}}
	diff = fmod((CommandedAttitude.z - PCommandedAttitude.z + TWO_PI),TWO_PI);
	if (diff > PI) {diff -= TWO_PI;} ;
	if(abs(diff/dt_c) > CommandRateLimits.z){
		if(diff > 0){
			CommandedAttitude.z = PCommandedAttitude.z + CommandRateLimits.z * dt_c;}
		else{CommandedAttitude.z = PCommandedAttitude.z - CommandRateLimits.z * dt_c;}}
	PCommandedAttitude = CommandedAttitude;
	/* **** LVDA **** */
	// ROLL ERROR
	AttitudeError.x = fmod((CommandedAttitude.x - CurrentAttitude.x + TWO_PI),TWO_PI);
	if (AttitudeError.x > PI) {AttitudeError.x -= TWO_PI;} ;
	// PITCH ERROR
	AttitudeError.y = fmod((CommandedAttitude.y - CurrentAttitude.y + TWO_PI),TWO_PI);
	if (AttitudeError.y > PI) {AttitudeError.y -= TWO_PI;} ;
	// YAW ERROR
	AttitudeError.z = fmod((CommandedAttitude.z - CurrentAttitude.z + TWO_PI),TWO_PI);
	if (AttitudeError.z > PI) {AttitudeError.z -= TWO_PI;} ;	

	/* **** FLIGHT CONTROL COMPUTER OPERATIONS **** */
	if(LVDC_Timebase == 1 && LVDC_TB_ETime < 105){
	a_0p = a_0y = 0.9;
	a_0r = 0.9;
	a_1p = a_1y = 0.69;
	a_1r = 0.69;
	}
	if(LVDC_Timebase == 1 && LVDC_TB_ETime >= 105 && LVDC_TB_ETime < 120){
	a_0p = a_0y = 0.45;
	a_0r = 0.45;
	a_1p = a_1y = 0.44;
	a_1r = 0.44;
	}
	if(LVDC_Timebase == 1 && LVDC_TB_ETime >= 120){
	a_0p = a_0y = 0.32;
	a_0r = 0.32;
	a_1p = a_1y = 0.30;
	a_1r = 0.30;
	}
	if(LVDC_Timebase == 3 && LVDC_TB_ETime < 60){
	a_0p = a_0y = 1.12;
	a_0r = 1.12;
	a_1p = a_1y = 1.9;
	a_1r = 1.9;
	}
	if(LVDC_Timebase == 3 && LVDC_TB_ETime >= 60 && LVDC_TB_ETime < 190){
	a_0p = a_0y = 0.65;
	a_0r = 0.65;
	a_1p = a_1y = 1.1;
	a_1r = 1.1;
	}
	if(LVDC_Timebase == 3 && LVDC_TB_ETime >= 190){
	a_0p = a_0y = 0.44;
	a_0r = 0.44;
	a_1p = a_1y = 0.74;
	a_1r = 0.74;
	}
	if(LVDC_Timebase == 4){
	a_0p = a_0y = 0.81;
	a_0r = 1;
	a_1p = a_1y = 0.97;
	a_1r = 5;
	}
	if(LVDC_Timebase == 5){
	a_0p = a_0y = 1;
	a_0r = 1;
	a_1p = a_1y = 5;
	a_1r = 5;
	}
	beta_pc = a_0p * AttitudeError.y + a_1p * AttRate.y;
	beta_yc = a_0y * AttitudeError.z + a_1y * AttRate.z;
	beta_rc = a_0r * AttitudeError.x + a_1r * AttRate.x;
	if(LVDC_Timebase < 4){
	beta_p1c = beta_pc + beta_rc/pow(2,0.5);//orbiter's engines are gimballed differently then the real one
	beta_p2c = beta_pc + beta_rc/pow(2,0.5);
	beta_p3c = beta_pc - beta_rc/pow(2,0.5);
	beta_p4c = beta_pc - beta_rc/pow(2,0.5);
	beta_y1c = beta_yc + beta_rc/pow(2,0.5);
	beta_y2c = beta_yc - beta_rc/pow(2,0.5);
	beta_y3c = beta_yc - beta_rc/pow(2,0.5);
	beta_y4c = beta_yc + beta_rc/pow(2,0.5);
	if(LVDC_Timebase < 3){//SIC
	SetThrusterDir(th_main[0],_V(beta_y4c,beta_p4c,1)); 
	SetThrusterDir(th_main[1],_V(beta_y2c,beta_p2c,1)); }
	else{//SII: engines 2 & 4 are flipped!
	SetThrusterDir(th_main[0],_V(beta_y2c,beta_p2c,1)); 
	SetThrusterDir(th_main[1],_V(beta_y4c,beta_p4c,1)); 
	};//1 & 3 are the same on both stages
	SetThrusterDir(th_main[2],_V(beta_y1c,beta_p1c,1)); 
	SetThrusterDir(th_main[3],_V(beta_y3c,beta_p3c,1)); 
	}
	if(LVDC_Timebase == 4){//SIVB powered flight
	beta_p1c = beta_pc;//gimbal angles
	beta_y1c = beta_yc;
	SetThrusterDir(th_main[0],_V(beta_y1c,beta_p1c,1));
	eps_p = 0;//we want neither the APS pitch thrusters to fire
	eps_ymr = -(a_0r * AttitudeError.x * DEG) - (a_1r * AttRate.x * DEG);//nor the yaw thrusters
	eps_ypr = (a_0r * AttitudeError.x * DEG) + (a_1r * AttRate.x * DEG);
	}
	if(LVDC_Timebase == 5){//SIVB coast flight; full APS control
	eps_p   = (a_0p * AttitudeError.y * DEG) + (a_1p * AttRate.y * DEG);//pitch thruster demand
	eps_ymr = (a_0y * AttitudeError.z * DEG) - (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) - (a_1r * AttRate.x * DEG);//yaw minus roll
	eps_ypr = (a_0y * AttitudeError.z * DEG) + (a_0r * AttitudeError.x * DEG) + (a_1y * AttRate.z * DEG) + (a_1r * AttRate.x * DEG);//yaw plus roll
	}
	if((LVDC_Timebase == 4 && S4B_IGN == true)|| LVDC_Timebase == 5){//APS thruster on/off control
		if(eps_p > 1){
			//fire+pitch
			if(eps_p >= 1.6){SetThrusterLevel(th_att_rot[1],1);}else{SetThrusterLevel(th_att_rot[1],(eps_p-1)/0.6);}
		}else{SetThrusterLevel(th_att_rot[1],0);}
		if(eps_p < -1){
			//fire-pitch
			if(eps_p <= -1.6){SetThrusterLevel(th_att_rot[0],1);}else{SetThrusterLevel(th_att_rot[0],(-eps_p-1)/0.6);}
		}else{SetThrusterLevel(th_att_rot[0],0);}
		if(eps_ymr > 1){
			//fire+yaw-roll;
			if(eps_ymr >= 1.6){SetThrusterLevel(th_att_rot[3],1);}else{SetThrusterLevel(th_att_rot[3],(eps_ymr-1)/0.6);}
		}else{SetThrusterLevel(th_att_rot[3],0);}
		if(eps_ymr < -1){
			//fire-yaw+roll;
			if(eps_ymr <= -1.6){SetThrusterLevel(th_att_rot[5],1);}else{SetThrusterLevel(th_att_rot[5],(-eps_ymr-1)/0.6);}
		}else{SetThrusterLevel(th_att_rot[5],0);}
		if(eps_ypr > 1){
			//fire+yaw+roll;
			if(eps_ypr >= 1.6){SetThrusterLevel(th_att_rot[4],1);}else{SetThrusterLevel(th_att_rot[4],(eps_ypr-1)/0.6);}
		}else{SetThrusterLevel(th_att_rot[4],0);}
		if(eps_ypr < -1){
			//fire-yaw-roll;
			if(eps_p <= -1.6){SetThrusterLevel(th_att_rot[2],1);}else{SetThrusterLevel(th_att_rot[2],(-eps_ypr-1)/0.6);}
		}else{SetThrusterLevel(th_att_rot[2],0);}
	}
	// Debug if we're launched
	if(LVDC_Timebase < 5){
		sprintf(oapiDebugString(),"TB%d+%f | T1 = %f | T2 = %f | T3 = %f | Tt_T = %f | ERR %f %f %f | V = %f R= %f",
			LVDC_Timebase,LVDC_TB_ETime,
			T_1,T_2,Tt_3,Tt_T,
			AttitudeError.x*DEG,AttitudeError.y*DEG,AttitudeError.z*DEG,
			V, R/1000
			);}else
		{sprintf(oapiDebugString(),"TB%d+%f |CMD %f %f %f | ERR %f %f %f | eps %f %f %f | V = %f R= %f",
			LVDC_Timebase,LVDC_TB_ETime,
			CommandedAttitude.x*DEG,CommandedAttitude.y*DEG,CommandedAttitude.z*DEG,
			AttitudeError.x*DEG,AttitudeError.y*DEG,AttitudeError.z*DEG,
			eps_p, eps_ymr, eps_ypr,V,R/1000);
		}
		/*
		sprintf(oapiDebugString(),"LVDC: TB%d + %f | PS %f %f %f | VS %f %f %f",
			LVDC_Timebase,LVDC_TB_ETime,
			PosS.x,PosS.y,PosS.z,
			DotS.x,DotS.y,DotS.z);
			*/
//			CurrentAttitude.x*DEG,CurrentAttitude.y*DEG,CurrentAttitude.z*DEG,V);								
	

	// Update engine indicators and failure flags
	if(LVDC_EI_On == true){
		double level;
		switch(stage){
			// 5-engine stages
			case PRELAUNCH_STAGE:
			case LAUNCH_STAGE_ONE:
				if(GetThrusterLevel(th_main[0]) >= 0.65  && ENGIND[3] == true){  ENGIND[3] = false; } 
				if(GetThrusterLevel(th_main[0]) < 0.65 && ENGIND[3] == false){  ENGIND[3] = true; }   
				if(GetThrusterLevel(th_main[1]) >= 0.65  && ENGIND[1] == true){  ENGIND[1] = false; } 
				if(GetThrusterLevel(th_main[1]) < 0.65 && ENGIND[1] == false){  ENGIND[1] = true; }   
				if(GetThrusterLevel(th_main[2]) >= 0.65  && ENGIND[0] == true){  ENGIND[0] = false; } 
				if(GetThrusterLevel(th_main[2]) < 0.65 && ENGIND[0] == false){  ENGIND[0] = true; }   
				if(GetThrusterLevel(th_main[3]) >= 0.65  && ENGIND[2] == true){  ENGIND[2] = false; } 
				if(GetThrusterLevel(th_main[3]) < 0.65 && ENGIND[2] == false){  ENGIND[2] = true; }   
				if(GetThrusterLevel(th_main[4]) >= 0.65  && ENGIND[4] == true){  ENGIND[4] = false; } 
				if(GetThrusterLevel(th_main[4]) < 0.65 && ENGIND[4] == false){  ENGIND[4] = true; }   
				break;
			case LAUNCH_STAGE_TWO:
			case LAUNCH_STAGE_TWO_ISTG_JET:
				if(GetThrusterLevel(th_main[0]) >= 0.65  && ENGIND[1] == true){  ENGIND[1] = false; } 
				if(GetThrusterLevel(th_main[0]) < 0.65 && ENGIND[1] == false){  ENGIND[1] = true; }   
				if(GetThrusterLevel(th_main[1]) >= 0.65  && ENGIND[3] == true){  ENGIND[3] = false; } 
				if(GetThrusterLevel(th_main[1]) < 0.65 && ENGIND[3] == false){  ENGIND[3] = true; }   
				if(GetThrusterLevel(th_main[2]) >= 0.65  && ENGIND[0] == true){  ENGIND[0] = false; } 
				if(GetThrusterLevel(th_main[2]) < 0.65 && ENGIND[0] == false){  ENGIND[0] = true; }   
				if(GetThrusterLevel(th_main[3]) >= 0.65  && ENGIND[2] == true){  ENGIND[2] = false; } 
				if(GetThrusterLevel(th_main[3]) < 0.65 && ENGIND[2] == false){  ENGIND[2] = true; }   
				if(GetThrusterLevel(th_main[4]) >= 0.65  && ENGIND[4] == true){  ENGIND[4] = false; } 
				if(GetThrusterLevel(th_main[4]) < 0.65 && ENGIND[4] == false){  ENGIND[4] = true; }
				break;
			// S4B only
			case LAUNCH_STAGE_SIVB:
			case STAGE_ORBIT_SIVB:
				level = GetThrusterLevel(th_main[0]);
				if(level >= 0.65  && ENGIND[0] == true){  ENGIND[0] = false; } // UNLIGHT
				if(level < 0.65 && ENGIND[0] == false){  ENGIND[0] = true; }   // LIGHT
				break;	
			// Error
			default:
				LVDC_EI_On = false;
				break;
		}
	}else{
	ENGIND[0] = false;
	ENGIND[1] = false;
	ENGIND[2] = false;
	ENGIND[3] = false;
	ENGIND[4] = false;
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
		SetEngineLevel(ENGINE_MAIN, 0);					// Kill the engines
		agc.SetInputChannelBit(030, 4, true);			// Notify the AGC of the abort
		agc.SetInputChannelBit(030, 5, true);			// and the liftoff, if it's not set already
		sprintf(oapiDebugString(),"");					// Clear the LVDC debug line
		LVDC_GP_PC = 30;								// Stop LVDC program
		// ABORT MODE 1 - Use of LES to extract CM
		// Allowed from T - 5 minutes until LES jettison.
		if(MissionTime > -300 && LESAttached){			
			SetEngineLevel(ENGINE_MAIN, 0);
			SeparateStage(CM_STAGE);
			SetStage(CM_STAGE);
			StartAbort();			// Resets MT, fires LET if attached
			bAbort = false;			// No further processing required
			return; 
		}
		// ABORT MODE 2/3/4 - Eject CSM from LV
		if(stage == LAUNCH_STAGE_ONE){
			// The only way we will get here is if the LET was jettisoned early for some reason.
			SeparateStage(LAUNCH_STAGE_TWO);
			SetStage(LAUNCH_STAGE_TWO);
			return;
		}
		if(stage == LAUNCH_STAGE_TWO){
			// The only way we will get here is if the LET was jettisoned early for some reason.
			SeparateStage (LAUNCH_STAGE_TWO_ISTG_JET);
			SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
			return;
		}
		if(stage == LAUNCH_STAGE_TWO_ISTG_JET){
			// This is the most likely entry point
			SeparateStage(LAUNCH_STAGE_SIVB);
			SetStage(LAUNCH_STAGE_SIVB);
		}
		if(stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB){
			// Eject CSM
			SeparateStage(CSM_LEM_STAGE);
			SetStage(CSM_LEM_STAGE);
			// Staging finished.
			StartAbort();			// Resets MT, sets abort light, resets engine lights, etc.
		}
		// Done with the abort request.
		bAbort = false;
	}
}
