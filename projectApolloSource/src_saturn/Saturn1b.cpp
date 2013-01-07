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
  *	Revision 1.8  2012/12/13 19:45:05  meik84
  *	LVDC++: SIB- LVDC++ & new LVDC.cpp
  *	
  *	Revision 1.7  2012/01/14 22:24:06  tschachim
  *	CM Optics cover
  *	
  *	Revision 1.6  2010/08/25 17:48:42  tschachim
  *	Bugfixes Saturn autopilot.
  *	
  *	Revision 1.5  2010/07/16 17:14:42  tschachim
  *	Changes for Orbiter 2010 and bugfixes
  *	
  *	Revision 1.4  2010/02/22 14:23:30  tschachim
  *	Apollo 7 S-IVB on orbit attitude control, venting and Saturn takeover mode for the VAGC.
  *	
  *	Revision 1.3  2010/02/09 02:40:23  bluedragon8144
  *	Improved SIVB on orbit autopilot.  Now starts 20 seconds after cutoff.
  *	
  *	Revision 1.2  2010/01/04 12:31:15  tschachim
  *	Improved Saturn IB launch autopilot, bugfixes
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.86  2008/05/24 17:29:19  tschachim
  *	Improved autopilot/pitch table
  *	
  *	Revision 1.85  2008/04/11 12:18:56  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.84  2008/01/23 01:40:07  lassombra
  *	Implemented timestep functions and event management
  *	
  *	Events for Saturns are now fully implemented
  *	
  *	Removed all hardcoded checklists from Saturns.
  *	
  *	Automatic Checklists are coded into an excel file.
  *	
  *	Added function to get the name of the active checklist.
  *	
  *	ChecklistController is now 100% ready for Saturn.
  *	
  *	Revision 1.83  2008/01/18 05:57:23  movieman523
  *	Moved SIVB creation code into generic Saturn function, and made ASTP sort of start to work.
  *	
  *	Revision 1.82  2008/01/16 05:52:06  movieman523
  *	Removed all dockstate code.
  *	
  *	Revision 1.81  2008/01/16 04:14:23  movieman523
  *	Rewrote docking probe separation code and moved the CSM_LEM code into a single function in the Saturn class.
  *	
  *	Revision 1.80  2008/01/14 04:48:43  movieman523
  *	Fixed LEM separation when the LEM doesn't have the expected name.
  *	
  *	Revision 1.79  2008/01/14 04:31:07  movieman523
  *	Initial tidyup: ASTP should now work too.
  *	
  *	Revision 1.78  2008/01/14 01:17:04  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.77  2008/01/10 01:38:49  movieman523
  *	Updated 2008 projects, and calculate stage mass in Saturn 1b at startup for safety.
  *	
  *	Revision 1.76  2007/12/05 23:07:44  movieman523
  *	Revised to allow SLA panel rotaton to be specified up to 150 degrees. Also start of new connector-equipped vessel code which was mixed up with the rest!
  *	
  *	Revision 1.75  2007/12/04 20:26:31  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.74  2007/10/18 00:23:17  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.73  2007/08/13 16:06:07  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.72  2007/07/17 14:33:03  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.71  2007/06/08 20:08:29  tschachim
  *	Kill apex cover vessel.
  *	
  *	Revision 1.70  2007/06/06 15:02:10  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.69  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.68  2007/02/06 18:30:14  tschachim
  *	Bugfixes docking probe, CSM/LM separation. The ASTP stuff still needs fixing though.
  *	
  *	Revision 1.67  2007/01/28 17:04:26  tschachim
  *	Bugfix docking probe.
  *	
  *	Revision 1.66  2007/01/22 15:48:13  tschachim
  *	SPS Thrust Vector Control, RHC power supply, THC clockwise switch, bugfixes.
  *	
  *	Revision 1.65  2007/01/11 08:23:58  chode99
  *	Modified the lift function for the CM to include Mach dependency
  *	
  *	Revision 1.64  2006/12/16 22:34:01  tschachim
  *	Bugfix dll handle
  *	
  *	Revision 1.63  2006/12/07 18:52:43  tschachim
  *	New LC34, Bugfixes.
  *	
  *	Revision 1.62  2006/11/30 14:16:12  tschachim
  *	Bugfixes abort modes.
  *	
  *	Revision 1.61  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.60  2006/08/27 21:57:10  tschachim
  *	Bugfix RCS handing in CSM timestep.
  *	
  *	Revision 1.59  2006/08/01 18:46:50  tschachim
  *	Added checklist actions.
  *	
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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
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
}

//
// Default pitch program (according to the Apollo 7 Saturn IB Report, NTRS ID 19900067467)
//

const double default_met[PITCH_TABLE_SIZE]    = { 0, 10, 20, 30, 40, 60, 80, 100, 120, 130,  135,  145,  200,  300,  400,  500};   // MET in sec
const double default_cpitch[PITCH_TABLE_SIZE] = {90, 89, 88, 85, 80, 70, 58,  47,  38,  33, 30.7, 30.7, 30.7, 30.7, 30.7, 30.7};   // Commanded pitch in °


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
	Burned = false;

	if (strcmp(GetName(), "AS-211")==0)
	{
		ASTPMission = true;
	}

	//
	// Pitch program.
	//

	for (int i = 0; i < PITCH_TABLE_SIZE; i++)
	{
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
	lvdc.init(this);
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
}

void Saturn1b::StageOne(double simt, double simdt)

{
	VESSELSTATUS vs;
	GetStatus(vs);

	double MainLevel = GetEngineLevel(ENGINE_MAIN);

	if (MainLevel < 0.3 && MissionTime < 100 && EDSSwitch.GetState() && MissionTime > 10)
	{
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
		// Shut down center engine at 2% fuel or at specified time.
		//

		if ((actualFUEL <= 2) || (MissionTime >= FirstStageCentreShutdownTime)) {
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

			// Move hidden SIB vessel
			if (hstg1)
			{
				VESSELSTATUS vs;
				GetStatus(vs);
				S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
				stage1->DefSetState(&vs);
			}				
			StageState++;
		}
		break;

	//
	// We don't actually have time here to play the countdown before seperation after the center
	// engine shuts down: with the Saturn 1 there are only a few seconds between the two events.
	//

	case 1:
		if (GetFuelMass() <= 0.001 || MissionTime >= FirstStageShutdownTime)
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
			SeparateStage (LAUNCH_STAGE_SIVB);
			SeparationS.play(NOLOOP, 245);
			SetStage(LAUNCH_STAGE_SIVB);

			// 
			// Tower jettison at 26.6 seconds after SIB shutdown if not previously
			// specified.
			//

			if (LESJettisonTime > 999.0) {
				LESJettisonTime = MissionTime + 26.6;
			}
		} else {

			//
			// Engine thrust decay.
			//

			for (int i = 0; i < 4; i++)	{
				double Level = GetThrusterLevel(th_main[i]);
				Level -= (simdt * 1.2);
				SetThrusterLevel(th_main[i], Level);
			}
		}
		return;
	}
}

void Saturn1b::StageLaunchSIVB(double simt)

{
	switch (StageState)	{
	case 0:
		SepS.play(LOOP, 130);
		SetThrusterGroupLevel(thg_ver,1.0);
		NextMissionEventTime = MissionTime + 2.0;
		SetSIVBMixtureRatio(5.0);
		ActivateStagingVent();
		StageState++;
		break;

	case 1:
		if (MissionTime >= NextMissionEventTime - 1.0)
			DeactivateStagingVent();

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
		if (MissionTime  < NextMissionEventTime) {
			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterLevel(th_main[0], 0.9 * (deltat / 2.5));
		} else {
			SetThrusterLevel(th_main[0], 0.9);
			SetThrusterGroupLevel(thg_ver, 0.0);
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
		} else {
			SetThrusterLevel(th_main[0], 1.0);
			SepS.stop();
			if (th_att_rot[0] == 0) {
				AddRCS_S4B();
			}

			NextMissionEventTime = MissionTime + 2.05;
			StageState++;
		}
		break;

	//
	// First mixture ratio shift.
	//

	case 4:
		if (MissionTime >= NextMissionEventTime) {
			SetSIVBThrusters(true);
			SetSIVBMixtureRatio(5.5);

			StageState++;
		}
		break;

	//
	// Second mixture ratio shift.
	//

	case 5:
		if (MissionTime >= SecondStagePUShiftTime) {
			if (Crewed)	{
				SPUShiftS.play();
				SPUShiftS.done();
			}
			SetSIVBMixtureRatio(4.5);
			StageState++;
		}
		break;

	//
	// Throttle down engine to achieve a more precise shutdown

	case 6: {
		double apDist, peDist;
		GetApDist(apDist);
		GetPeDist(peDist);
		OBJHANDLE ref = GetGravityRef();
		if (apDist - oapiGetSize(ref) >= agc.GetDesiredApogee() * 1000. - 30000. && peDist - oapiGetSize(ref) >= agc.GetDesiredPerigee() * 1000. - 1000.) {
			LastMissionEventTime = MissionTime;
			StageState++;
		} }
		break;

	case 7:
		if (GetThrusterLevel(th_main[0]) <= 0.05) {
			StageState++;
		} else {
			double deltat = MissionTime - LastMissionEventTime;
			SetThrusterLevel(th_main[0], max(0.05, 1. - (deltat / 0.4)));
		}
		break;

	//
	// Shutdown.
	//

	case 8:
		if (GetThrusterLevel(th_main[0]) <= 0) {
			NextMissionEventTime = MissionTime + 10.0;
			SIVBCutoffTime = MissionTime;

			ActivateNavmode(NAVMODE_KILLROT);

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

	// Abort handling
	if (CSMLVPyros.Blown() || (bAbort && !LESAttached))	{		
		SeparateStage(CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
		if (bAbort) {
			/// \todo SPS abort handling			
			StartAbort();
			bAbort = false;
			autopilot = false;
		}
	}
	// sprintf(oapiDebugString(), "StageLaunchSIVB state %d thrust %f", StageState, GetThrusterLevel(th_main[0]));
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

	//
	// Give the AGC our new stats.
	//

	agc.SetVesselStats(isp, thrust, false);
	iu.SetVesselStats(isp, thrust);

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
	if (FirstTimestep) 
	{
		DoFirstTimestep(simt);
		FirstTimestep = false;
		return;
	}

	GenericTimestep(simt, simdt, mjd);

	/// \todo LVDC++ as in Saturn V

	//
	// Abort handling
	//
	if (use_lvdc) {
			// Nothing for now, the LVDC is called in PostStep
	} else {
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

		switch (stage) {

		case LAUNCH_STAGE_ONE:
			StageOne(simt, simdt);
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
	}
	LastTimestep = simt;
}

void Saturn1b::clbkPostStep (double simt, double simdt, double mjd) {

	Saturn::clbkPostStep(simt, simdt, mjd);

	if (use_lvdc) {
		lvdc.timestep(simt, simdt);	
	} else {
		// Run the autopilot post step to have stable dynamic data
		switch (stage) {
		case LAUNCH_STAGE_ONE:
			if (autopilot) {
				AutoPilot(MissionTime);
			} else {
				AttitudeLaunch1();
			}
			break;

		case LAUNCH_STAGE_SIVB:
			if (autopilot) {
				AutoPilot(MissionTime);
			} else {
				AttitudeLaunchSIVB();
			}
			break;
		}
	}
}

//
// Save any state specific to the Saturn 1b.
//

void Saturn1b::SaveVehicleStats(FILEHANDLE scn)

{
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
