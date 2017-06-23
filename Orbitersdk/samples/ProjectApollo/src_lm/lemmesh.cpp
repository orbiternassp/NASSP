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
#include "lm_channels.h"

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
	SetPMI(_V(2.5428, 2.2871, 2.7566));
	// SetCrossSections (_V(21,23,17));
	//SetPMI(_V(3.26, 2.22, 3.26));
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
	th_hover[0] = CreateThruster (_V(0.0  , -3.3,  0.0),  _V(0,1,0), 46706.3, ph_Dsc, 3107);
	th_hover[1] = CreateThruster (_V(0.013, -3.0, -0.03), _V(0,1,0),     0, ph_Dsc, 0);		//this is a "virtual engine",no thrust and no fuel
																							//needed for visual gimbaling for corrected engine flames
	DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup(th_hover, 2, THGROUP_HOVER);
	AddExhaust(th_hover[1], 10.0, 1.2, exhaustTex);

	SetCameraOffset (_V(-1,1.0,0.0));
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-1.85);
	status = 0;
	stage = 0;
	bModeDocked=true;

	VECTOR3 dockpos = {0.0 ,2.6, 0.0};
    VECTOR3 dockdir = {0,1,0};
	VECTOR3 dockrot = { -0.8660254, 0, 0.5 };
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
	agc.SetInputChannelBit(030, DescendStageAttached, true);

	CheckRCS();
}

void LEM::SetLmVesselHoverStage()
{
	ClearThrusterDefinitions();

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

	double Mass = (DescentFuelMassKg * 0.05) + AscentFuelMassKg + 4374.0;
	double ro = 7;
	TOUCHDOWNVTX td[4];
	double x_target = -0.5;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = 0;
	td[0].pos.y = -3.86;
	td[0].pos.z = 1 * ro;
	td[1].pos.x = -cos(30 * RAD)*ro;
	td[1].pos.y = -3.86;
	td[1].pos.z = -sin(30 * RAD)*ro;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = -3.86;
	td[2].pos.z = -sin(30 * RAD)*ro;
	td[3].pos.x = 0;
	td[3].pos.y = 3.86;
	td[3].pos.z = 0;

	SetTouchdownPoints(td, 4);
	
	/*	static const DWORD ntdvtx = 4;
	static TOUCHDOWNVTX tdvtx[4] = {
		{ _V(0, -3.86, 5), 1e6, 1e5, 3.0, 3.0 },
		{ _V(-5, -3.86, -5), 1e6, 1e5, 3.0, 3.0 },
		{ _V(5, -3.86, -5), 1e6, 1e5, 3.0, 3.0 },
		{ _V(0, 3.86, 0), 2e4, 3e5, 0.5 }
	};
	SetTouchdownPoints(tdvtx, ntdvtx);*/

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
	th_hover[0] = CreateThruster (_V(0.0  , -2.0,  0.0),   _V(0,1,0), 46706.3, ph_Dsc, 3107);
	th_hover[1] = CreateThruster (_V(0.013, -2.8, -0.034), _V(0,1,0),     0, ph_Dsc, 0);	//this is a "virtual engine",no thrust and no fuel
																							//needed for visual gimbaling for corrected engine flames
    DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup(th_hover, 2, THGROUP_HOVER);
	AddExhaust (th_hover[1], 10.0, 1.5, exhaustTex);
		
	SetCameraOffset (_V(-1,1.0,0.0));
	status = 1;
	stage = 1;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-1.85);
	bModeHover=true;

	VECTOR3 dockpos = {0.0 ,2.6, 0.0};	
	VECTOR3 dockdir = {0,1,0};
	VECTOR3 dockrot = { -0.8660254, 0, 0.5 };
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
	agc.SetInputChannelBit(030, DescendStageAttached, true);

	CheckRCS();
}


void LEM::SetLmAscentHoverStage()

{
	ClearThrusterDefinitions();
	ShiftCentreOfMass(_V(0.0,3.0,0.0));
	SetSize (5);
	SetCOG_elev (5);
	SetEmptyMass (2150.0);
	SetPMI(_V(2.8, 2.29, 2.37));
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
    double Mass = AscentFuelMassKg + 2150.0;
	double ro = 5;
	TOUCHDOWNVTX td[4];
	double x_target = -0.5;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = 0;
	td[0].pos.y = tdph;
	td[0].pos.z = 1 * ro;
	td[1].pos.x = -cos(30 * RAD)*ro;
	td[1].pos.y = tdph;
	td[1].pos.z = -sin(30 * RAD)*ro;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = tdph;
	td[2].pos.z = -sin(30 * RAD)*ro;
	td[3].pos.x = 0;
	td[3].pos.y = 2.8;
	td[3].pos.z = 0;

	SetTouchdownPoints(td, 4);

    /*static const DWORD ntdvtx = 4;
	static TOUCHDOWNVTX tdvtx[4] = {
		{ _V(0, tdph, 5), 1e6, 1e5, 3.0, 3.0 },
		{ _V(-5, tdph, -5), 1e6, 1e5, 3.0, 3.0 },
		{ _V(5, tdph, -5), 1e6, 1e5, 3.0, 3.0 },
		{ _V(0, tdph + 5, 0), 2e4, 3e5, 0.5 }
	};
	SetTouchdownPoints(tdvtx, ntdvtx);*/

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

	VECTOR3 dockrot = { -0.8660254, 0, 0.5 };
	SetDockParams(dockpos, dockdir, dockrot);
	hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
	InitNavRadios (4);
	LDGswitch=false;
	AFEED1switch=true;
	AFEED2switch=true;
	AFEED3switch=true;
	AFEED4switch=true;

	// Descent stage detached.
	agc.SetInputChannelBit(030, DescendStageAttached, false);

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
