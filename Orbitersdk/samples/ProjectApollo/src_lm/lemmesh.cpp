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
#include "Sat5LMDSC.h"

#include "CollisionSDK/CollisionSDK.h"

static MESHHANDLE hLMPKD ;
static MESHHANDLE hLMLanded ;
static MESHHANDLE hLMDescent;
static MESHHANDLE hLMAscent ;
static MESHHANDLE hLMAscent2 ;
static MESHHANDLE hAstro1 ;
static MESHHANDLE hLemProbes;
static MESHHANDLE hLPDgret;
static MESHHANDLE hLPDgext;
static MESHHANDLE hFwdHatch;
static MESHHANDLE hOvhdHatch;
static MESHHANDLE hLM1;

static PARTICLESTREAMSPEC lunar_dust = {
	0,		// flag
	1,	    // size
	5,      // rate
	20,	    // velocity
	1,      // velocity distribution
	2.0,    // lifetime
	10,   	// growthrate
	2.0,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_LIN, 0, 1,
	PARTICLESTREAMSPEC::ATM_PLOG, -0.1, 0.1
};

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
	ClearThrusterDefinitions();
	SetEmptyMass(AscentFuelMassKg + AscentEmptyMassKg + DescentEmptyMassKg);
	SetSize (6);
	SetPMI(_V(2.5428, 2.2871, 2.7566));
	SetCrossSections (_V(24.53,21.92,24.40));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double Mass = 15876;
	double ro = 1;
	double ro1 = 4;
	TOUCHDOWNVTX td[7];
	double x_target = -0.25;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<7; i++) {
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
	td[3].pos.x = cos(30 * RAD)*ro1;
	td[3].pos.y = 0;
	td[3].pos.z = sin(30 * RAD)*ro1;
	td[4].pos.x = -cos(30 * RAD)*ro1;
	td[4].pos.y = 0;
	td[4].pos.z = sin(30 * RAD)*ro1;
	td[5].pos.x = 0;
	td[5].pos.y = 0;
	td[5].pos.z = -1 * ro1;
	td[6].pos.x = 0;
	td[6].pos.y = 3.86;
	td[6].pos.z = 0;

	SetTouchdownPoints(td, 7);

	VECTOR3 mesh_dir = _V(-0.003, -0.03, 0.004);

	UINT meshidx;
	if (NoLegs)
	{
		meshidx = AddMesh(hLMPKD, &mesh_dir);
	}
	else
	{
		meshidx = AddMesh(hLMPKD, &mesh_dir);
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	
	// Forward Hatch
	VECTOR3 hatch_dir = _V(-0.003, -0.03, 0.004);
	fwdhatch = AddMesh(hFwdHatch, &hatch_dir);
	SetFwdHatchMesh();

	// Drogue & Overhead hatch
	ovhdhatch = AddMesh(hOvhdHatch, &hatch_dir);
	SetOvhdHatchMesh();
	
	if (!ph_Dsc)
	{
		ph_Dsc = CreatePropellantResource(DescentFuelMassKg); //2nd stage Propellant
	}
	else
	{
		SetPropellantMaxMass(ph_Dsc, DescentFuelMassKg);
	}
	SetDefaultPropellantResource(ph_Dsc); // display 2nd stage propellant level in generic HUD

	// 133.084001 kg is 293.4 pounds, which is the fuel + oxidizer capacity of one RCS tank.
	if (!ph_RCSA) {
		ph_RCSA = CreatePropellantResource(LM_RCS_FUEL_PER_TANK);
	}
	if (!ph_RCSB) {
		ph_RCSB = CreatePropellantResource(LM_RCS_FUEL_PER_TANK);
	}

	// orbiter main thrusters
	th_hover[0] = CreateThruster (_V(0.0  , -2.0,  0.0),  _V(0,1,0), 46706.3, ph_Dsc, 3107);
	th_hover[1] = CreateThruster (_V(0.013, -2.8, -0.03), _V(0,1,0),     0, ph_Dsc, 0);		//this is a "virtual engine",no thrust and no fuel
																							//needed for visual gimbaling for corrected engine flames
	DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup(th_hover, 2, THGROUP_HOVER);
	
	EXHAUSTSPEC es_hover[1] = {
		{ th_hover[1], NULL, NULL, NULL, 10.0, 1.2, 0, 0.1, exhaustTex }
	};

	AddExhaust(es_hover);

	SetCameraOffset(_V(-0.68, 1.65, 1.35));
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

	// Descent stage attached.
	if (InvertStageBit)
	{
		agc.SetInputChannelBit(030, DescendStageAttached, false);
	}
	else
	{
		agc.SetInputChannelBit(030, DescendStageAttached, true);
	}

	//Set part of ascent stage mesh to be visible from LPD window
	VECTOR3 lpd_dir = _V(-0.191, 1.827, 0.383);
	lpdgret = AddMesh(hLPDgret, &lpd_dir);
	SetLPDMesh();
}

void LEM::SetLmVesselHoverStage()
{
	ClearThrusterDefinitions();

	SetEmptyMass(AscentFuelMassKg + AscentEmptyMassKg + DescentEmptyMassKg);

	SetSize (7);
	SetPMI(_V(2.5428, 2.2871, 2.7566));
	SetCrossSections (_V(24.53,21.92,24.40));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double Mass = 7137.75;
	double ro = 4;
	TOUCHDOWNVTX td[7];
	double x_target = -0.25;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<7; i++) {
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
	td[3].pos.x = cos(30 * RAD)*ro;
	td[3].pos.y = -3.86;
	td[3].pos.z = sin(30 * RAD)*ro;
	td[4].pos.x = -cos(30 * RAD)*ro;
	td[4].pos.y = -3.86;
	td[4].pos.z = sin(30 * RAD)*ro;
	td[5].pos.x = 0;
	td[5].pos.y = -3.86;
	td[5].pos.z = -1 * ro;
	td[6].pos.x = 0;
	td[6].pos.y = 3.86;
	td[6].pos.z = 0;

	SetTouchdownPoints(td, 7);
	
	VSSetTouchdownPoints(GetHandle(), _V(0, -3.86, 5), _V(-5, -3.86, -5), _V(5, -3.86, -5));

	VECTOR3 mesh_dir=_V(-0.003,-0.03,0.004);	
	UINT meshidx;
	if (NoLegs)
	{
		meshidx = AddMesh(hLMLanded, &mesh_dir);
	}
	else
	{
		if (Landed) {
			meshidx = AddMesh(hLMLanded, &mesh_dir);
		}
		else {
			UINT probeidx;
			meshidx = AddMesh(hLMLanded, &mesh_dir);
			probeidx = AddMesh(hLemProbes, &mesh_dir);
			SetMeshVisibilityMode(probeidx, MESHVIS_VCEXTERNAL);
		}
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// Forward Hatch
	VECTOR3 hatch_dir= _V(-0.003, -0.03, 0.004);
	fwdhatch = AddMesh(hFwdHatch, &hatch_dir);
	SetFwdHatchMesh();

	// Drogue & Overhead hatch
	ovhdhatch = AddMesh(hOvhdHatch, &hatch_dir);
	SetOvhdHatchMesh();

	if (!ph_Dsc){  
		ph_Dsc  = CreatePropellantResource(DescentFuelMassKg); //2nd stage Propellant
	}
	else
	{
		SetPropellantMaxMass(ph_Dsc, DescentFuelMassKg);
	}

	SetDefaultPropellantResource (ph_Dsc); // display 2nd stage propellant level in generic HUD

	if (!ph_RCSA){
		ph_RCSA = CreatePropellantResource(LM_RCS_FUEL_PER_TANK);
	}
	if (!ph_RCSB){
		ph_RCSB = CreatePropellantResource(LM_RCS_FUEL_PER_TANK);
	}
	
	// orbiter main thrusters
	th_hover[0] = CreateThruster (_V(0.0  , -2.0,  0.0),   _V(0,1,0), 46706.3, ph_Dsc, 3107);
	th_hover[1] = CreateThruster (_V(0.013, -2.8, -0.034), _V(0,1,0),     0, ph_Dsc, 0);	//this is a "virtual engine",no thrust and no fuel
																							//needed for visual gimbaling for corrected engine flames
    DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup(th_hover, 2, THGROUP_HOVER);
	
	EXHAUSTSPEC es_hover[1] = {
		{ th_hover[1], NULL, NULL, NULL, 10.0, 1.5, 0, 0.1, exhaustTex }
	};

	AddExhaust(es_hover);

	// Simulate the dust kicked up near
	// the lunar surface
	int i;

	VECTOR3	s_exhaust_pos1 = _V(0, -15, 0);
	VECTOR3 s_exhaust_pos2 = _V(0, -15, 0);
	VECTOR3	s_exhaust_pos3 = _V(0, -15, 0);
	VECTOR3 s_exhaust_pos4 = _V(0, -15, 0);

	th_dust[0] = CreateThruster(s_exhaust_pos1, _V(-1, 0, 1), 0, ph_Dsc);
	th_dust[1] = CreateThruster(s_exhaust_pos2, _V(1, 0, 1), 0, ph_Dsc);
	th_dust[2] = CreateThruster(s_exhaust_pos3, _V(1, 0, -1), 0, ph_Dsc);
	th_dust[3] = CreateThruster(s_exhaust_pos4, _V(-1, 0, -1), 0, ph_Dsc);

	for (i = 0; i < 4; i++) {
		AddExhaustStream(th_dust[i], &lunar_dust);
	}
	thg_dust = CreateThrusterGroup(th_dust, 4, THGROUP_USER);
		
	SetCameraOffset(_V(-0.68, 1.65, 1.35));
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

	// Descent stage attached.
	if (InvertStageBit)
	{
		agc.SetInputChannelBit(030, DescendStageAttached, false);
	}
	else
	{
		agc.SetInputChannelBit(030, DescendStageAttached, true);
	}

	//Set fwd footpad mesh to be visible from LPD window
	VECTOR3 lpd_dir = _V(-0.003, -0.03, 0.004);
	lpdgext = AddMesh(hLPDgext, &lpd_dir);
	SetLPDMesh();
}

void LEM::SetLmAscentHoverStage()

{
	ClearThrusterDefinitions();
	ShiftCentreOfMass(_V(0.0,3.0,0.0));
	SetSize (5);
	SetEmptyMass (AscentEmptyMassKg);
	SetPMI(_V(2.8, 2.29, 2.37));
	SetCrossSections (_V(21,23,17));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double tdph = -5.8;
    double Mass = 4495.0;
	double ro = 3;
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

	VSSetTouchdownPoints(GetHandle(), _V(0, tdph, 5), _V(-5, tdph, -5), _V(5, tdph, -5));

	VECTOR3 mesh_dir=_V(-0.191,-0.02,0.383);	
	UINT meshidx = AddMesh (hLMAscent, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// Forward Hatch
	VECTOR3 hatch_dir= _V(0, -1.88, 0);
	fwdhatch = AddMesh(hFwdHatch, &hatch_dir);
	SetFwdHatchMesh();

	// Drogue & Overhead hatch
	ovhdhatch = AddMesh(hOvhdHatch, &hatch_dir);
	SetOvhdHatchMesh();
	
	if (!ph_Asc)
	{
		ph_Asc = CreatePropellantResource(AscentFuelMassKg);	// 2nd stage Propellant
	}
	else
	{
		SetPropellantMaxMass(ph_Asc, AscentFuelMassKg);
	}
	SetDefaultPropellantResource (ph_Asc);			// Display 2nd stage propellant level in generic HUD
	// orbiter main thrusters
    th_hover[0] = CreateThruster (_V( 0.0,  -2.5, 0.0), _V( 0,1,0), APS_THRUST, ph_Asc, APS_ISP);
	th_hover[1] = CreateThruster (_V( 0.01, -2.0, 0.0), _V( 0,1,0), 0,          ph_Asc, 0);		// this is a "virtual engine",no thrust and no fuel
																								// needed for visual gimbaling for corrected engine flames
    DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup (th_hover, 2, THGROUP_HOVER);
	
	EXHAUSTSPEC es_hover[1] = {
		{ th_hover[1], NULL, NULL, NULL, 6.0, 0.8, 0, 0.1, exhaustTex }
	};

	AddExhaust(es_hover);
	
	SetCameraOffset(_V(-0.68, -0.195, 1.35));
	status = 2;
	stage = 2;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH2(-1.86);
	bModeHover=true;

	if(ph_Dsc){
		DelPropellantResource(ph_Dsc);
		ph_Dsc = 0;
	}
	
	VECTOR3 dockpos = {0.0 ,0.75, 0.0};
	VECTOR3 dockdir = {0,1,0};

	VECTOR3 dockrot = { -0.8660254, 0, 0.5 };
	SetDockParams(dockpos, dockdir, dockrot);
	hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
	InitNavRadios (4);

	// Descent stage attached.
	if (InvertStageBit)
	{
		agc.SetInputChannelBit(030, DescendStageAttached, true);
	}
	else
	{
		agc.SetInputChannelBit(030, DescendStageAttached, false);
	}

	//Set part of ascent stage mesh to be visible from LPD window
	VECTOR3 lpd_dir = _V(-0.191, -0.02, 0.383);
	lpdgret = AddMesh(hLPDgret, &lpd_dir);
	SetLPDMesh();
}

void LEM::SeparateStage (UINT stage)

{
	ResetThrusters();

	VESSELSTATUS2 vs2;
	memset(&vs2, 0, sizeof(vs2));
	vs2.version = 2;

	if (stage == 0) {
		ShiftCentreOfMass(_V(0.0, -1.155, 0.0));
		GetStatusEx(&vs2);
		char VName[256];
		strcpy(VName, GetName()); strcat(VName, "-DESCENTSTG");
		hdsc = oapiCreateVesselEx(VName, "ProjectApollo/Sat5LMDSC", &vs2);
		
		Sat5LMDSC *dscstage = static_cast<Sat5LMDSC *> (oapiGetVesselInterface(hdsc));
		dscstage->SetState(0);
		
		SetLmAscentHoverStage();
		}
	
	if (stage == 1)	{
		ShiftCentreOfMass(_V(0.0, -1.155, 0.0));
		GetStatusEx(&vs2);
		
		if (vs2.status == 1) {
			vs2.vrot.x = 2.7;
			char VName[256];
			strcpy(VName, GetName()); strcat(VName, "-DESCENTSTG");
			hdsc = oapiCreateVesselEx(VName, "ProjectApollo/Sat5LMDSC", &vs2);
			
			Sat5LMDSC *dscstage = static_cast<Sat5LMDSC *> (oapiGetVesselInterface(hdsc));
			if (Landed) {
				dscstage->SetState(1);
			}
			else {
				dscstage->SetState(11);
			}
			
			vs2.vrot.x = 5.8;
			DefSetStateEx(&vs2);
			SetLmAscentHoverStage();
		}
		else
		{
			char VName[256];
			strcpy(VName, GetName()); strcat(VName, "-DESCENTSTG");
			hdsc = oapiCreateVesselEx(VName, "ProjectApollo/Sat5LMDSC", &vs2);
			
			Sat5LMDSC *dscstage = static_cast<Sat5LMDSC *> (oapiGetVesselInterface(hdsc));
			if (Landed) {
				dscstage->SetState(1);
			}
			else {
				dscstage->SetState(11);
			}
			
			SetLmAscentHoverStage();
		}
	}

	CheckDescentStageSystems();
}

void LEM::SetLmLandedMesh() {

	ClearMeshes();
	VECTOR3 mesh_dir=_V(-0.003,-0.03,0.004);	
	UINT meshidx = AddMesh (hLMLanded, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	//Set fwd footpad mesh to be visible from LPD window
	lpdgext = AddMesh(hLPDgext, &mesh_dir);
	SetLPDMesh();

	Landed = true;
}

void LEM::SetLPDMesh() {
	
	if (stage == 0 || stage == 2) {
		if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
			SetMeshVisibilityMode(lpdgret, MESHVIS_COCKPIT);
		}
		else {
			SetMeshVisibilityMode(lpdgret, MESHVIS_NEVER);
		}
	}

	if (stage == 1) {
		if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
			SetMeshVisibilityMode(lpdgext, MESHVIS_COCKPIT);
		}
		else {
			SetMeshVisibilityMode(lpdgext, MESHVIS_NEVER);
		}
	}
}

void LEM::SetFwdHatchMesh() {
	
	if (ForwardHatch.IsOpen()) {
		SetMeshVisibilityMode(fwdhatch, MESHVIS_NEVER);
	}
	else {
		SetMeshVisibilityMode(fwdhatch, MESHVIS_VCEXTERNAL);
	}
}

void LEM::SetOvhdHatchMesh() {

	if (OverheadHatch.IsOpen()) {
		SetMeshVisibilityMode(ovhdhatch, MESHVIS_NEVER);
	}
	else {
		SetMeshVisibilityMode(ovhdhatch, MESHVIS_VCEXTERNAL);
	}
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
	hLPDgret = oapiLoadMeshGlobal("ProjectApollo/LPD_gret");
	hLPDgext = oapiLoadMeshGlobal("ProjectApollo/LPD_gext");
	hFwdHatch = oapiLoadMeshGlobal("ProjectApollo/LM_FwdHatch");
	hOvhdHatch = oapiLoadMeshGlobal("ProjectApollo/LM_Drogue");
	lunar_dust.tex = oapiRegisterParticleTexture("ProjectApollo/dust");
	hLM1 = oapiLoadMeshGlobal("ProjectApollo/LM_1");
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
