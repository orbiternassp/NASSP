/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LEM mesh code

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
  *	Revision 1.4  2010/05/01 12:55:15  dseagrav
  *	
  *	Cause LM mission timer to print value when adjusted. (Since you can't see it from the switches)
  *	Right-clicking causes the time to be printed but does not flip the switches.
  *	Left-clicking works as normal and prints the new value.
  *	The printed value is not updated and is removed after five seconds.
  *	
  *	Revision 1.3  2009/10/19 12:24:49  dseagrav
  *	LM checkpoint commit.
  *	Put back one FDAI for testing purposes (graphic is wrong)
  *	Messed around with mass properties
  *	LGC now runs Luminary 099 instead of 131
  *	Added LGC pad load, values need worked but addresses are checked.
  *	
  *	Revision 1.2  2009/07/11 13:40:19  jasonims
  *	DockingProbe Work
  *	
  *	Revision 1.1  2009/02/18 23:21:14  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.38  2009/02/02 18:44:55  tschachim
  *	Bugfix docking port position
  *	
  *	Revision 1.37  2008/04/11 11:49:40  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.36  2008/01/28 15:58:47  tschachim
  *	Bugfix EnableFocus
  *	
  *	Revision 1.35  2008/01/12 04:14:10  movieman523
  *	Pass payload information to SIVB and have LEM use the fuel masses passed to it.
  *	
  *	Revision 1.34  2007/10/18 00:23:20  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.33  2007/06/06 15:02:14  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.32  2006/08/20 08:28:06  dseagrav
  *	LM Stage Switch actually causes staging (VERY INCOMPLETE), Incorrect "Ascent RCS" removed, ECA outputs forced to 24V during initialization to prevent IMU/LGC failure on scenario load, Valves closed by default, EDS saves RCS valve states, would you like fries with that?
  *	
  *	Revision 1.31  2006/08/18 05:45:01  dseagrav
  *	LM EDS now exists. Talkbacks wired to a power source will revert to BP when they lose power.
  *	
  *	Revision 1.30  2006/08/13 16:55:35  movieman523
  *	Removed a bunch of unused files.
  *	
  *	Revision 1.29  2006/08/13 16:01:52  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  *	Revision 1.28  2006/08/12 15:05:35  movieman523
  *	Renamed EVA and LEVA files.
  *	
  *	Revision 1.27  2006/08/12 14:47:57  movieman523
  *	Basic dox for LEVA.
  *	
  *	Revision 1.26  2006/08/12 14:14:18  movieman523
  *	Renamed EVA and LEVA classes, and added ApexCoverAttached flag to Saturn.
  *	
  *	Revision 1.25  2006/07/26 19:05:20  tschachim
  *	RCS enabled by default until state is saved properly.
  *	
  *	Revision 1.24  2006/04/22 03:53:48  jasonims
  *	Began initial support for multiple EVA's (two astronauts), as well as improving upon the LRV controls.  No longer turns while standing still.  Throttle controlled via (NUM+ and NUM-).
  *	
  *	Revision 1.23  2006/04/12 06:27:19  dseagrav
  *	LM checkpoint commit. The LM is not airworthy at this point. Please be patient.
  *	
  *	Revision 1.22  2006/01/19 14:53:50  tschachim
  *	Initial Meshland support.
  *	
  *	Revision 1.21  2006/01/04 19:51:54  movieman523
  *	Updated config file names.
  *	
  *	Revision 1.20  2005/12/28 19:11:32  movieman523
  *	Changed some mesh paths.
  *	
  *	Revision 1.19  2005/12/28 16:19:10  movieman523
  *	Should now be getting all config files from ProjectApollo directory.
  *	
  *	Revision 1.18  2005/11/22 20:16:40  movieman523
  *	Revised to only use one LEM mesh and add the landing probes as required.
  *	
  *	Revision 1.17  2005/11/21 12:42:17  tschachim
  *	New LM landed and descent stage mesh.
  *	
  *	Revision 1.16  2005/08/14 16:08:20  tschachim
  *	LM is now a VESSEL2
  *	Changed panel restore mechanism because the CSM mechanism
  *	caused CTDs, reason is still unknown.
  *	
  *	Revision 1.15  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.14  2005/08/10 20:00:55  spacex15
  *	Activated 3 position lem eng arm switch
  *	
  *	Revision 1.13  2005/08/09 09:16:22  tschachim
  *	Introduced toggleswitch lib
  *	
  *	Revision 1.12  2005/08/07 19:27:15  lazyd
  *	No change
  *	
  *	Revision 1.11  2005/08/06 00:03:48  movieman523
  *	Beginnings of support for AGC I/O channels in LEM.
  *	
  *	Revision 1.10  2005/07/17 17:40:20  spacex15
  *	fixed lm sinking below the moon surface when landing
  *	
  *	Revision 1.9  2005/07/06 14:30:41  lazyd
  *	Added code to make vessel mass and fuel mass mission dependent
  *	
  *	Revision 1.8  2005/06/26 21:40:23  lazyd
  *	Added code to make LM mass and fuel mass correct for mission
  *	
  *	Revision 1.7  2005/04/11 23:47:42  yogenfrutz
  *	once more: fixed docking positions and flames position
  *	
  *	Revision 1.6  2005/03/17 16:21:46  yogenfrutz
  *	corrected a mistake i did in my previous version,sorry
  *	
  *	Revision 1.5  2005/03/17 06:41:28  yogenfrutz
  *	corrected wrongly placed docking positions,and adjusted once more the position of main engine exhaust flames
  *	
  *	Revision 1.4  2005/03/15 12:53:32  yogenfrutz
  *	Corrected position and size of lem ascent and descent main thruster exhaust flames
  *	
  *	Revision 1.3  2005/03/04 20:36:22  chode99
  *	Fixed touchdown points on ascent stage so it doesn't fall when released.
  *	
  *	Revision 1.2  2005/02/24 00:26:35  movieman523
  *	Revised to delay switch to LEVA to work around Orbitersound getting upset if we switch immediately after creation.
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
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"
#include "leva.h"

#include "CollisionSDK/CollisionSDK.h"

static MESHHANDLE hLMPKD ;
static MESHHANDLE hLMLanded ;
static MESHHANDLE hLMDescent;
static MESHHANDLE hLMAscent ;
static MESHHANDLE hLMAscent2 ;
static MESHHANDLE hAstro1 ;
static MESHHANDLE hLemProbes;


void LEM::ToggleEVA()

{
	ToggleEva = false;	
	
	if (CDREVA_IP) {
		// Nothing for now, the EVA is ended, when the LEVA vessel calls StopEVA
		/// \todo Support for 2 LEVA vessels
	}
	else {
		VESSELSTATUS vs1;
		GetStatus(vs1);

		// The LM must be in landed state
		if (vs1.status != 1) return;

		CDREVA_IP = true;

		OBJHANDLE hbody = GetGravityRef();
		double radius = oapiGetSize(hbody);
		vs1.vdata[0].x += 4.5 * sin(vs1.vdata[0].z) / radius;
		vs1.vdata[0].y += 4.5 * cos(vs1.vdata[0].z) / radius;

		char VName[256]="";
		strcpy (VName, GetName()); strcat (VName, "-LEVA");
		hLEVA = oapiCreateVessel(VName,"ProjectApollo/LEVA",vs1);
		
		SwitchFocusToLeva = 10;

		LEVA *leva = (LEVA *) oapiGetVesselInterface(hLEVA);
		if (leva) {
			LEVASettings evas;

			evas.MissionNo = agc.GetApolloNo();
			evas.Realism = Realism;
			leva->SetEVAStats(evas);
		}
	}
}


void LEM::SetupEVA()

{
	if (CDREVA_IP) {
		CDREVA_IP = true;
		// nothing for now...
	}
}

void LEM::StopEVA()

{
	// Called by LEVA vessel during destruction
	CDREVA_IP = false;
}

void LEM::SetLmVesselDockStage()

{	
	double fuelmass;
	int mnumber;
	ClearThrusterDefinitions();
	agc.SetVesselStats(DPS_ISP, DPS_THRUST, true);
	//
	// Changed to reflect mission-specific empty and fuel mass
	//
	// From "Apollo by the Numbers"
	//
	mnumber=agc.GetApolloNo();
	if(mnumber < 15) {
		SetEmptyMass(6565);		
		fuelmass=8375.;
	} else {
		SetEmptyMass(7334);
		fuelmass=8891.;
	}
	SetSize (6);
	// SetPMI (_V(2.8,2.29,2.37));
	// SetCrossSections (_V(21,23,17));
	SetPMI (_V(3.26,2.22,3.26));
	SetCrossSections (_V(24.53,21.92,24.40));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();
	SetTouchdownPoints (_V(0,0,10), _V(-1,0,-10), _V(1,0,-10));
    VECTOR3 mesh_dir=_V(0.0,-0.2,0.03);

	UINT meshidx = AddMesh (hLMPKD, &mesh_dir);	
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
    if (!ph_Dsc)  
		ph_Dsc  = CreatePropellantResource(fuelmass); //2nd stage Propellant
	SetDefaultPropellantResource (ph_Dsc); // display 2nd stage propellant level in generic HUD

	// 133.084001 kg is 293.4 pounds, which is the fuel + oxidizer capacity of one RCS tank.
	if (!ph_RCSA) {
		ph_RCSA = CreatePropellantResource(133.084001);
	}
	if (!ph_RCSB) {
		ph_RCSB = CreatePropellantResource(133.084001);
	}

	// orbiter main thrusters
	th_hover[0] = CreateThruster (_V(0.0  , -3.3,  0.0),  _V(0,1,0), 44910, ph_Dsc, 3107);
	th_hover[1] = CreateThruster (_V(0.013, -3.0, -0.03), _V(0,1,0),     0, ph_Dsc, 0);		//this is a "virtual engine",no thrust and no fuel
																							//needed for visual gimbaling for corrected engine flames
	DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup (th_hover, 2, THGROUP_HOVER);
	AddExhaust(th_hover[1], 10.0, 1.2, exhaustTex);

	SetCameraOffset (_V(-1,1.0,0.0));
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-1.85);
	status = 0;
	stage = 0;
	bModeDocked=true;

	VECTOR3 dockpos = {0.0 ,2.6, 0.0};
    VECTOR3 dockdir = {0,1,0};
	VECTOR3 dockrot = {-0.7045, 0, 0.7045};
	SetDockParams(dockpos, dockdir, dockrot);
	hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
	InitNavRadios (4);
    LDGswitch=false;
	ATT2switch=true;
	ATT3switch=true;
	ATT1switch=true;
	AFEED1switch=false;
	AFEED2switch=false;
	AFEED3switch=false;
	AFEED4switch=false;

	// Descent stage attached.
	agc.SetInputChannelBit(030, 2, true);

	CheckRCS();
}

void LEM::SetLmVesselHoverStage()
{
	ClearThrusterDefinitions();
	agc.SetVesselStats(DPS_ISP, DPS_THRUST, true);

	SetEmptyMass(AscentFuelMassKg + 4374.0);

	SetSize (7);
	SetPMI (_V(3.26,2.22,3.26)); 
	SetCrossSections (_V(24.53,21.92,24.40));
	// SetPMI (_V(2.8,2.29,2.37));
	// SetCrossSections (_V(21,23,17));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	SetTouchdownPoints (_V(0, -3.86, 5), _V(-5, -3.86, -5), _V(5, -3.86, -5));
	VSSetTouchdownPoints(GetHandle(), _V(0, -3.86, 5), _V(-5, -3.86, -5), _V(5, -3.86, -5));

	VECTOR3 mesh_dir=_V(-0.003,-0.03,0.004);	
	UINT meshidx;
	if (Landed) {
		meshidx = AddMesh (hLMLanded, &mesh_dir);
	}else{
		UINT probeidx;
		meshidx = AddMesh (hLMLanded, &mesh_dir);
		probeidx = AddMesh (hLemProbes, &mesh_dir);
		SetMeshVisibilityMode (probeidx, MESHVIS_VCEXTERNAL);
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
    
	if (!ph_Dsc){  
		ph_Dsc  = CreatePropellantResource(DescentFuelMassKg); //2nd stage Propellant
	}

	SetDefaultPropellantResource (ph_Dsc); // display 2nd stage propellant level in generic HUD

	if (!ph_RCSA){
		ph_RCSA = CreatePropellantResource(133.084001);
	}
	if (!ph_RCSB){
		ph_RCSB = CreatePropellantResource(133.084001);
	}
	
	// orbiter main thrusters
	th_hover[0] = CreateThruster (_V(0.0  , -3.3,  0.0),   _V(0,1,0), 44910, ph_Dsc, 3107);
	th_hover[1] = CreateThruster (_V(0.013, -2.8, -0.034), _V(0,1,0),     0, ph_Dsc, 0);	//this is a "virtual engine",no thrust and no fuel
																							//needed for visual gimbaling for corrected engine flames
    DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup (th_hover, 2, THGROUP_HOVER);
	AddExhaust (th_hover[1], 10.0, 1.5, exhaustTex);
		
	SetCameraOffset (_V(-1,1.0,0.0));
	status = 1;
	stage = 1;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-1.85);
	bModeHover=true;

	VECTOR3 dockpos = {0.0 ,2.6, 0.0};	
	VECTOR3 dockdir = {0,1,0};
	VECTOR3 dockrot = {-0.7045, 0, 0.7045};
	SetDockParams(dockpos, dockdir, dockrot);
	hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
	InitNavRadios (4);

	LDGswitch=true;
	ATT2switch=true;
	ATT3switch=true;
	ATT1switch=true;
	AFEED1switch=false;
	AFEED2switch=false;
	AFEED3switch=false;
	AFEED4switch=false;

	// Descent stage attached.
	agc.SetInputChannelBit(030, 2, true);

	CheckRCS();
}


void LEM::SetLmAscentHoverStage()

{
	ClearThrusterDefinitions();
	agc.SetVesselStats(APS_ISP, APS_THRUST, true);
	ShiftCentreOfMass(_V(0.0,3.0,0.0));
	SetSize (5);
	SetCOG_elev (5);
	SetEmptyMass (1920.0);
	SetPMI (_V(2.8,2.29,2.37));
	SetCrossSections (_V(21,23,17));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double tdph = -5.8;
	SetTouchdownPoints (_V(0, tdph, 5), _V(-5, tdph, -5), _V(5, tdph, -5));
	VSSetTouchdownPoints(GetHandle(), _V(0, tdph, 5), _V(-5, tdph, -5), _V(5, tdph, -5));

	VECTOR3 mesh_dir=_V(-0.191,-0.02,+0.383);	
	UINT meshidx = AddMesh (hLMAscent, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

    if (!ph_Asc)  
		ph_Asc  = CreatePropellantResource(AscentFuelMassKg);	// 2nd stage Propellant
	SetDefaultPropellantResource (ph_Asc);			// Display 2nd stage propellant level in generic HUD
	// orbiter main thrusters
    th_hover[0] = CreateThruster (_V( 0.0,  -2.5, 0.0), _V( 0,1,0), APS_THRUST, ph_Asc, APS_ISP);
	th_hover[1] = CreateThruster (_V( 0.01, -2.0, 0.0), _V( 0,1,0), 0,          ph_Asc, 0);		// this is a "virtual engine",no thrust and no fuel
																								// needed for visual gimbaling for corrected engine flames
    DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup (th_hover, 2, THGROUP_HOVER);
	AddExhaust (th_hover[1], 6.0, 0.8, exhaustTex);
	
	SetCameraOffset (_V(-1,1.0,0.0));
	status = 2;
	stage = 2;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH2(-1.86);
	bModeHover=true;

	if(ph_Dsc){
		DelPropellantResource(ph_Dsc);
		ph_Dsc = 0;
	}
	
	VECTOR3 dockpos = {0.0 ,0.58, 0.0};
	VECTOR3 dockdir = {0,1,0};

	VECTOR3 dockrot = {-0.7045, 0, 0.7045};
	SetDockParams(dockpos, dockdir, dockrot);
	hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
	InitNavRadios (4);
	LDGswitch=false;
	AFEED1switch=true;
	AFEED2switch=true;
	AFEED3switch=true;
	AFEED4switch=true;

	// Descent stage detached.
	agc.SetInputChannelBit(030, 2, false);

	CheckRCS();
}

void LEM::SeparateStage (UINT stage)

{
	ResetThrusters();

	VESSELSTATUS vs1;
	VECTOR3 ofs1 = _V(0,-5,0);
	VECTOR3 vel1 = _V(0,0,0);

	if (stage == 1)	{
	    GetStatus (vs1);
		vs1.eng_main = vs1.eng_hovr = 0.0;
		VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
		Local2Global (ofs1, vs1.rpos);
		GlobalRot (vel1, rofs1);
		vs1.rvel.x = rvel1.x+rofs1.x;
		vs1.rvel.y = rvel1.y+rofs1.y;
		vs1.rvel.z = rvel1.z+rofs1.z;
	    vs1.vrot.x = 0.0;	
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		GetStatus (vs1);
		// Wrong sound, is Saturn staging
		// StageS.play(NOLOOP, 150);

		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-DESCENTSTG");
		hdsc = oapiCreateVessel(VName, "ProjectApollo/sat5LMDSC", vs1);

		SetLmAscentHoverStage();
	}
}

void LEM::SetLmLandedMesh() {

	ClearMeshes();
	VECTOR3 mesh_dir=_V(-0.003,-0.03,0.004);	
	UINT meshidx = AddMesh (hLMLanded, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	Landed = true;
}

void LEMLoadMeshes()

{
	hLMPKD = oapiLoadMeshGlobal ("ProjectApollo/LM_NoWheel");
	hLMLanded = oapiLoadMeshGlobal ("ProjectApollo/LM_Landed");
	hLMDescent = oapiLoadMeshGlobal ("ProjectApollo/LM_descent");
	hLMAscent = oapiLoadMeshGlobal ("ProjectApollo/LM_ascent");
	hLMAscent2= oapiLoadMeshGlobal ("ProjectApollo/LM_ascent2");
	hAstro1= oapiLoadMeshGlobal ("ProjectApollo/Sat5AstroS");
	hLemProbes = oapiLoadMeshGlobal ("ProjectApollo/LM_ContactProbes");
}

//
// Debug routine.
//

/*
void LEM::GetDockStatus()

{
	VESSELSTATUS2 vslm;
	VESSELSTATUS2::DOCKINFOSPEC dckinfo;

	memset(&vslm, 0, sizeof(vslm));
	memset(&dckinfo, 0, sizeof(dckinfo));

	vslm.flag = 0; // VS_DOCKINFOLIST;
	vslm.fuel = 0;
	vslm.thruster = 0;
	vslm.ndockinfo = 1;
	vslm.dockinfo = &dckinfo;
	vslm.version = 2;

//	GetStatusEx(&vslm);
}
*/
