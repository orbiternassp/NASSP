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
#include "LM_AscentStageResource.h"
#include "LM_DescentStageResource.h"

#include "CollisionSDK/CollisionSDK.h"

static MESHHANDLE hLMDescent;
static MESHHANDLE hLMDescentNoLeg;
static MESHHANDLE hLMAscent ;
static MESHHANDLE hAstro1 ;
static MESHHANDLE hLMVC;

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
		CDRSuited->number = 0;

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
	CDRSuited->number = 1;
}

void LEM::SetLmVesselDockStage()

{
	ClearThrusterDefinitions();
	SetEmptyMass(AscentFuelMassKg + AscentEmptyMassKg + DescentEmptyMassKg);
	SetSize (6);
	SetVisibilityLimit(1e-3, 4.6401e-4);
	SetPMI(_V(2.5428, 2.2871, 2.7566));
	SetCrossSections (_V(24.53,21.92,24.40));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearMeshes(true);
	ClearBeacons();
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

	VECTOR3 mesh_asc = _V(0.00, 0.99, 0.00);
	VECTOR3 mesh_dsc = _V(0.00, -1.25, 0.00);

	// Ascent Stage Mesh
	ascidx = AddMesh(hLMAscent, &mesh_asc);

	// VC Mesh
	vcidx = AddMesh(hLMVC, &mesh_asc);

	// Descent Stage Mesh
	if (NoLegs)
	{
		dscidx = AddMesh(hLMDescentNoLeg, &mesh_dsc);
	}
	else
	{
		dscidx = AddMesh(hLMDescent, &mesh_dsc);
	}

	SetLMMeshVis();
	
	if (!ph_Dsc)
	{
		ph_Dsc = CreatePropellantResource(DescentFuelMassKg); //2nd stage Propellant
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

	SetCameraOffset(_V(-0.61, 1.625, 1.39)); // Has to be the same as LPD view
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-5.4516);
	status = 0;
	stage = 0;

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

	// Exterior lights
	SetTrackLight();
	SetDockingLights();
}

void LEM::SetLmVesselHoverStage()
{
	ClearThrusterDefinitions();

	SetEmptyMass(AscentFuelMassKg + AscentEmptyMassKg + DescentEmptyMassKg);

	SetSize (7);
	SetVisibilityLimit(1e-3, 5.4135e-4);
	SetPMI(_V(2.5428, 2.2871, 2.7566));
	SetCrossSections (_V(24.53,21.92,24.40));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearBeacons();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double Mass = 7137.75;
	double ro = 4.25;
	TOUCHDOWNVTX td[8];
	double x_target = -0.25;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<8; i++) {
		if (i < 5) {
			td[i].damping = damping;
			td[i].stiffness = stiffness;
		}
		else {
			td[i].damping = damping / 100;
			td[i].stiffness = stiffness / 100;
		}
		td[i].mu = 3;
		td[i].mu_lng = 3;
	}

	td[0].pos.x = 0;
	td[0].pos.y = -3.86;
	td[0].pos.z = ro;
	td[1].pos.x = -ro;
	td[1].pos.y = -3.86;
	td[1].pos.z = 0;
	td[2].pos.x = 0;
	td[2].pos.y = -3.86;
	td[2].pos.z = -ro;
	td[3].pos.x = ro;
	td[3].pos.y = -3.86;
	td[3].pos.z = 0;
	td[4].pos.x = 0;
	td[4].pos.y = 3.86;
	td[4].pos.z = 0;
	td[5].pos.x = -ro;
	td[5].pos.y = -5.57;
	td[5].pos.z = 0;
	td[6].pos.x = 0;
	td[6].pos.y = -5.57;
	td[6].pos.z = -ro;
	td[7].pos.x = ro;
	td[7].pos.y = -5.57;
	td[7].pos.z = 0;

	SetTouchdownPoints(td, 8);

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
		
	SetCameraOffset(_V(-0.61, 1.625, 1.39)); // Has to be the same as LPD view
	status = 1;
	stage = 1;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-5.4516);

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

	// Exterior lights
	SetTrackLight();
	SetDockingLights();
}

void LEM::SetLmAscentHoverStage()

{
	ClearThrusterDefinitions();
	ShiftCentreOfMass(_V(0.0,3.0,0.0));
	SetSize (5);
	SetVisibilityLimit(1e-3, 3.8668e-4);
	SetEmptyMass (AscentEmptyMassKg);
	SetPMI(_V(2.8, 2.29, 2.37));
	SetCrossSections (_V(21,23,17));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.7));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0); 
	ClearBeacons();
	ClearExhaustRefs();
	ClearAttExhaustRefs();
	eds.DeleteAnimations();
	DPS.DeleteAnimations();

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

	VECTOR3 mesh_asc=_V(0.00, -1.75, 0.00);

	// Vessel Meshes
	DelMesh(dscidx);
	dscidx = -1;
	ShiftMeshes(mesh_asc);

	if (!ph_Asc)
	{
		ph_Asc = CreatePropellantResource(AscentFuelMassKg);	// 2nd stage Propellant
	}
	else
	{
		SetPropellantMaxMass(ph_Asc, AscentFuelMassKg);
	}
	SetDefaultPropellantResource (ph_Asc);			// Display 2nd stage propellant level in generic HUD

													// 133.084001 kg is 293.4 pounds, which is the fuel + oxidizer capacity of one RCS tank.
	if (!ph_RCSA) {
		ph_RCSA = CreatePropellantResource(LM_RCS_FUEL_PER_TANK);
	}
	if (!ph_RCSB) {
		ph_RCSB = CreatePropellantResource(LM_RCS_FUEL_PER_TANK);
	}

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
	
	SetCameraOffset(_V(-0.61, -0.125, 1.39)); // Has to be the same as LPD view
	status = 2;
	stage = 2;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-7.2016);

	if(ph_Dsc){
		DelPropellantResource(ph_Dsc);
		ph_Dsc = 0;
	}
	
	SetLmDockingPort(0.85);
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

	// Exterior lights
	SetTrackLight();
	SetDockingLights();
}

void LEM::SeparateStage (UINT stage)

{
	ResetThrusters();

	VESSELSTATUS2 vs2;
	memset(&vs2, 0, sizeof(vs2));
	vs2.version = 2;

	if (stage == 0) {
		ShiftCentreOfMass(_V(0.0, -1.25, 0.0));
		GetStatusEx(&vs2);
		char VName[256];
		strcpy(VName, GetName()); strcat(VName, "-DESCENTSTG");
		hdsc = oapiCreateVesselEx(VName, "ProjectApollo/Sat5LMDSC", &vs2);

		Sat5LMDSC *dscstage = static_cast<Sat5LMDSC *> (oapiGetVesselInterface(hdsc));
		if (NoLegs)
		{
			dscstage->SetState(10);
		}
		else
		{
			dscstage->SetState(0);
		}

		SetLmAscentHoverStage();
	}
	
	if (stage == 1)	{
		ShiftCentreOfMass(_V(0.0, -1.25, 0.0));
		GetStatusEx(&vs2);
		
		if (vs2.status == 1) {
			vs2.vrot.x = 2.7;
			char VName[256];
			strcpy(VName, GetName()); strcat(VName, "-DESCENTSTG");
			hdsc = oapiCreateVesselEx(VName, "ProjectApollo/Sat5LMDSC", &vs2);
			
			Sat5LMDSC *dscstage = static_cast<Sat5LMDSC *> (oapiGetVesselInterface(hdsc));
			if (NoLegs)
			{
				dscstage->SetState(10);
			}
			else if (Landed)
			{
				dscstage->SetState(1);
			}
			else
			{
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
			if (NoLegs)
			{
				dscstage->SetState(10);
			}
			else if (Landed)
			{
				dscstage->SetState(1);
			}
			else
			{
				dscstage->SetState(11);
			}
			
			SetLmAscentHoverStage();
		}
	}

	CheckDescentStageSystems();
}

void LEM::SetLmLandedMesh() {

	Landed = true;
	HideProbes();
}

void LEM::SetLMMeshVis() {

	SetLMMeshVisAsc();
	SetLMMeshVisVC();
	SetLMMeshVisDsc();
}

void LEM::SetLMMeshVisAsc() {

	if (ascidx == -1)
		return;

	if (InPanel && PanelId == LMPANEL_AOTZOOM) {
		SetMeshVisibilityMode(ascidx, MESHVIS_ALWAYS);
	}
	else
	{
		SetMeshVisibilityMode(ascidx, MESHVIS_EXTERNAL);
	}
}

void LEM::SetLMMeshVisVC() {

	if (vcidx == -1)
		return;

	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
		SetMeshVisibilityMode(vcidx, MESHVIS_ALWAYS);
	}
	else
	{
		SetMeshVisibilityMode(vcidx, MESHVIS_VC);
	}
}

void LEM::SetLMMeshVisDsc() {
	
	if (dscidx == -1)
		return;

	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
		SetMeshVisibilityMode(dscidx, MESHVIS_ALWAYS);
	}
	else
	{
		SetMeshVisibilityMode(dscidx, MESHVIS_VCEXTERNAL);
	}
}

void LEM::DrogueVis() {

	if (!drogue)
		return;

	static UINT meshgroup_Drogue = AS_GRP_Drogue;
	GROUPEDITSPEC ges;

	if (OverheadHatch.IsOpen()) {
		ges.flags = (GRPEDIT_ADDUSERFLAG);
		ges.UsrFlag = 3;
		oapiEditMeshGroup(drogue, meshgroup_Drogue, &ges);
	}
	else
	{
		ges.flags = (GRPEDIT_SETUSERFLAG);
		ges.UsrFlag = 0;
		oapiEditMeshGroup(drogue, meshgroup_Drogue, &ges);
	}
}

void LEM::HideProbes() {

	if (!probes)
		return;

	if (Landed && !NoLegs) {
		static UINT meshgroup_Probes1[3] = { DS_GRP_Probes1Aft, DS_GRP_Probes1Left, DS_GRP_Probes1Right };
		static UINT meshgroup_Probes2[3] = { DS_GRP_Probes2Aft, DS_GRP_Probes2Left, DS_GRP_Probes2Right };
		GROUPEDITSPEC ges;

		for (int i = 0; i < 3; i++) {
			ges.flags = (GRPEDIT_ADDUSERFLAG);
			ges.UsrFlag = 3;
			oapiEditMeshGroup(probes, meshgroup_Probes1[i], &ges);
			oapiEditMeshGroup(probes, meshgroup_Probes2[i], &ges);
		}
	}
}

void LEM::SetTrackLight() {
	
	static VECTOR3 beaconPos = _V(0.00, 1.38, 2.28);
	static VECTOR3 beaconPosAsc = _V(0.00, -0.38, 2.28);
	static VECTOR3 beaconCol = _V(1, 1, 1);
	trackLight.shape = BEACONSHAPE_STAR;
	if (stage == 2) {
		trackLight.pos = &beaconPosAsc;
	}
	else {
		trackLight.pos = &beaconPos;
	}
	trackLight.col = &beaconCol;
	trackLight.size = 0.5;
	trackLight.falloff = 0.5;
	trackLight.period = 1.0;
	trackLight.duration = 0.1;
	trackLight.tofs = 0;
	trackLight.active = false;
	AddBeacon(&trackLight);
}

void LEM::SetDockingLights() {

	int i;
	double yoffset = -1.76;
	static VECTOR3 beaconPos[5] = { { 0.28, 1.47, 2.27 },{ 0.00, 1.85,-1.81 },{ -0.28, 1.47, 2.27 },{ -2.52, 0.59, 0.20 },{ 1.91, 0.29, 0.22 } };
	static VECTOR3 beaconPosAsc[5] = { { 0.28, 1.47 + yoffset, 2.27 },{ 0.00, 1.85 + yoffset,-1.81 },{ -0.28, 1.47 + yoffset, 2.27 },{ -2.52, 0.59 + yoffset, 0.20 },{ 1.91, 0.29 + yoffset, 0.22 } };
	static VECTOR3 beaconCol[4] = { { 1.0, 1.0, 1.0 },{ 1.0, 1.0, 0.5 },{ 1.0, 0.5, 0.5 },{ 0.5, 1.0, 0.5 } };
	for (i = 0; i < 5; i++) {
		dockingLights[i].shape = BEACONSHAPE_DIFFUSE;
		if (stage == 2) {
			dockingLights[i].pos = beaconPosAsc+i;
		}
		else {
			dockingLights[i].pos = beaconPos+i;
		}
		dockingLights[i].col = (i < 2 ? beaconCol : i < 3 ? beaconCol+1 : i < 4 ? beaconCol+2 : beaconCol+3);
		dockingLights[i].size = 0.12;
		dockingLights[i].falloff = 0.8;
		dockingLights[i].period = 0.0;
		dockingLights[i].duration = 1.0;
		dockingLights[i].tofs = 0;
		dockingLights[i].active = false;
		AddBeacon(dockingLights+i);
	}
}

void LEMLoadMeshes()

{
	hLMDescent = oapiLoadMeshGlobal ("ProjectApollo/LM_DescentStage");
	hLMDescentNoLeg = oapiLoadMeshGlobal("ProjectApollo/LM_DescentStageNoLeg");
	hLMAscent = oapiLoadMeshGlobal ("ProjectApollo/LM_AscentStage");
	hAstro1= oapiLoadMeshGlobal ("ProjectApollo/Sat5AstroS");
	hLMVC = oapiLoadMeshGlobal("ProjectApollo/LM_VC");
	lunar_dust.tex = oapiRegisterParticleTexture("ProjectApollo/dust");
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
