/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn mesh code

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

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "ioChannels.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"
#include "tracer.h"
#include "sivb.h"

#include "LES.h"

MESHHANDLE hSM;
MESHHANDLE hSMRCS;
MESHHANDLE hSMRCSLow;
MESHHANDLE hSMSPS;
MESHHANDLE hSMPanel1;
MESHHANDLE hSMPanel2;
MESHHANDLE hSMPanel3;
MESHHANDLE hSMPanel4;
MESHHANDLE hSMPanel5;
MESHHANDLE hSMPanel6;
MESHHANDLE hSMhga;
MESHHANDLE hSMCRYO;
MESHHANDLE hSMSIMBAY;
MESHHANDLE hCM;
MESHHANDLE hCM2;
MESHHANDLE hCMP;
MESHHANDLE hCMInt;
MESHHANDLE hCMVC;
MESHHANDLE hCREW;
MESHHANDLE hFHO;
MESHHANDLE hFHC;
MESHHANDLE hCM2B;
MESHHANDLE hprobe;
MESHHANDLE hprobeext;
//MESHHANDLE hCMBALLOON;
MESHHANDLE hCRB;
MESHHANDLE hApollochute;
MESHHANDLE hCMB;
MESHHANDLE hChute30;
MESHHANDLE hChute31;
MESHHANDLE hChute32;
MESHHANDLE hFHC2;
MESHHANDLE hsat5tower;
MESHHANDLE hFHO2;
MESHHANDLE hCMPEVA;
MESHHANDLE hopticscover;

extern void CoeffFunc(double aoa, double M, double Re ,double *cl ,double *cm  ,double *cd);

#define LOAD_MESH(var, name) var = oapiLoadMeshGlobal(name);

// "O2 venting" particle streams
PARTICLESTREAMSPEC o2_venting_spec = {
	0,		// flag
	0.3,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	2,		// lifetime
	0.2,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

static PARTICLESTREAMSPEC let_exhaust = {
	0,		// flag
	0.5,	// size
	100.0, 	// rate
	25.0,	// velocity
	0.1,	// velocity distribution
	0.3, 	// lifetime
	3.0,	// growthrate
	0.0,	// atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC dyemarker_spec = {
	0,		// flag
	0.15,	// size
	15,	    // rate
	1,	    // velocity
	0.3,    // velocity distribution
	3,		// lifetime
	0.2,	// growthrate
	0.2,    // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC wastewaterdump_spec = {
	0,		// flag
	0.005,	// size
	1000,	// rate
	1.5,    // velocity
	0.2,    // velocity distribution
	100,	// lifetime
	0.001,	// growthrate
	0,      // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC urinedump_spec = {
	0,		// flag
	0.005,	// size
	1000,	// rate
	1.5,    // velocity
	0.2,    // velocity distribution
	100,	// lifetime
	0.001,	// growthrate
	0,		// atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};


void SaturnInitMeshes()

{
	LOAD_MESH(hSM, "ProjectApollo/SM-core");
	LOAD_MESH(hSMRCS, "ProjectApollo/SM-RCSHI");
	LOAD_MESH(hSMRCSLow, "ProjectApollo/SM-RCSLO");
	LOAD_MESH(hSMSPS, "ProjectApollo/SM-SPS");
	LOAD_MESH(hSMPanel1, "ProjectApollo/SM-Panel1");
	LOAD_MESH(hSMPanel2, "ProjectApollo/SM-Panel2");
	LOAD_MESH(hSMPanel3, "ProjectApollo/SM-Panel3");
	LOAD_MESH(hSMPanel4, "ProjectApollo/SM-Panel4");
	LOAD_MESH(hSMPanel5, "ProjectApollo/SM-Panel5");
	LOAD_MESH(hSMPanel6, "ProjectApollo/SM-Panel6");
	LOAD_MESH(hSMhga, "ProjectApollo/SM-HGA");
	LOAD_MESH(hSMCRYO, "ProjectApollo/SM-CRYO");
	LOAD_MESH(hSMSIMBAY, "ProjectApollo/SM-SIMBAY");
	LOAD_MESH(hCM, "ProjectApollo/CM");
	LOAD_MESH(hCM2, "ProjectApollo/CM-Recov");
	LOAD_MESH(hCMP, "ProjectApollo/CM-CMP");
	LOAD_MESH(hCMInt, "ProjectApollo/CM-Interior");
	LOAD_MESH(hCMVC, "ProjectApollo/CM-VC");
	LOAD_MESH(hCREW, "ProjectApollo/CM-CREW");
	LOAD_MESH(hFHC, "ProjectApollo/CM-HatchC");
	LOAD_MESH(hFHO, "ProjectApollo/CM-HatchO");
	LOAD_MESH(hCM2B, "ProjectApollo/CMB-Recov");
	LOAD_MESH(hprobe, "ProjectApollo/CM-Probe");
	LOAD_MESH(hprobeext, "ProjectApollo/CM-ProbeExtended");
	LOAD_MESH(hCRB, "ProjectApollo/CM-CrewRecovery");
	LOAD_MESH(hCMB, "ProjectApollo/CMB");
	LOAD_MESH(hChute30, "ProjectApollo/Apollo_2chute");
	LOAD_MESH(hChute31, "ProjectApollo/Apollo_3chuteEX");
	LOAD_MESH(hChute32, "ProjectApollo/Apollo_3chuteHD");
	LOAD_MESH(hApollochute, "ProjectApollo/Apollo_3chute");
	LOAD_MESH(hFHC2, "ProjectApollo/CMB-HatchC");
	LOAD_MESH(hsat5tower, "ProjectApollo/BoostCover");
	LOAD_MESH(hFHO2, "ProjectApollo/CMB-HatchO");
	LOAD_MESH(hCMPEVA, "ProjectApollo/CM-CMPEVA");
	LOAD_MESH(hopticscover, "ProjectApollo/CM-OpticsCover");

	SURFHANDLE contrail_tex = oapiRegisterParticleTexture("Contrail2");
	let_exhaust.tex = contrail_tex;
}

void Saturn::AddSM(double offset, bool showSPS)

{
	VECTOR3 mesh_dir=_V(0, SMVO, offset);

	AddMesh (hSM, &mesh_dir);

	if (LowRes)
		AddMesh(hSMRCSLow, &mesh_dir);
	else
		AddMesh (hSMRCS, &mesh_dir);

	AddMesh (hSMPanel1, &mesh_dir);
	AddMesh (hSMPanel2, &mesh_dir);
	AddMesh (hSMPanel3, &mesh_dir);

	if (!ApolloExploded)
		AddMesh (hSMPanel4, &mesh_dir);
	else
		AddMesh (hSMCRYO, &mesh_dir);

	AddMesh (hSMPanel5, &mesh_dir);
	AddMesh (hSMPanel6, &mesh_dir);
	AddMesh (hSMSIMBAY, &mesh_dir);

	if (showSPS) {
		mesh_dir = _V(0, SMVO, offset - 1.654);
		SPSidx = AddMesh(hSMSPS, &mesh_dir);
	}
}




///\todo needs to be redesigned

/*
void Saturn::ToggleEVA()

{
	UINT meshidx;

	//
	// EVA does nothing if we're unmanned.
	//

	if (!Crewed)
		return;

	ToggleEva = false;

	if (EVA_IP){
		EVA_IP =false;

		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
		AddMesh (hSM, &mesh_dir);

		//
		// Skylab SM and Apollo 7 have no HGA.
		//
		if (!NoHGA) {
			mesh_dir=_V(-1.308,-1.18,29.042-12.25-21.5);
			AddMesh (hSMhga, &mesh_dir);
		}

		mesh_dir=_V(0,0,34.4-12.25-21.5);

		// And the Crew
		if (Crewed) {
			cmpidx = AddMesh (hCMP, &mesh_dir);
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		} else {
			cmpidx = -1;
			crewidx = -1;
		}

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

		meshidx = AddMesh (hCMVC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VC);
		VCMeshOffset = mesh_dir;

		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//Don't Forget the Hatch
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen = true;
		if (HasProbe) {
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		} else {
			probeidx = -1;
			probeextidx = -1;
		}
	}
	else 
	{
		EVA_IP = true;

		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
		AddMesh (hSM, &mesh_dir);
		mesh_dir=_V(-1.308,-1.18,29.042-12.25-21.5);
		AddMesh (hSMhga, &mesh_dir);

		mesh_dir=_V(0,0,34.4-12.25-21.5);

		// And the Crew, CMP is outside
		if (Crewed) {
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		} else {
			crewidx = -1;
		}
		cmpidx = -1;

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

		meshidx = AddMesh (hCMVC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VC);
		VCMeshOffset = mesh_dir;

		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//Don't Forget the Hatch
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen= true;

		if (HasProbe) {
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		} else {
			probeidx = -1;
			probeextidx = -1;
		}

		VESSELSTATUS vs1;
		GetStatus(vs1);
		VECTOR3 ofs1 = _V(0,0.15,34.25-12.25-21.5);
		VECTOR3 vel1 = _V(0,0,0);
		VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
		Local2Rel (ofs1, vs1.rpos);
		GlobalRot (vel1, rofs1);
		vs1.rvel.x = rvel1.x+rofs1.x;
		vs1.rvel.y = rvel1.y+rofs1.y;
		vs1.rvel.z = rvel1.z+rofs1.z;
		char VName[256]="";
		strcpy (VName, GetName()); strcat (VName, "-EVA");
		hEVA = oapiCreateVessel(VName,"ProjectApollo/EVA",vs1);
		oapiSetFocusObject(hEVA);
	}
}

void Saturn::SetupEVA()

{
	UINT meshidx;

	if (EVA_IP){
		EVA_IP =true;

		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
		AddMesh (hSM, &mesh_dir);

		//
		// Skylab SM and Apollo 7 have no HGA.
		//
		if (!NoHGA) {
			mesh_dir=_V(-1.308,-1.18,29.042-12.25-21.5);
			AddMesh (hSMhga, &mesh_dir);
		}

		mesh_dir=_V(0,0,34.4-12.25-21.5);

		// And the Crew, CMP is outside
		if (Crewed) {
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		} else {
			crewidx = -1;
		}
		cmpidx = -1;

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

		meshidx = AddMesh (hCMVC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VC);
		VCMeshOffset = mesh_dir;

		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//Don't Forget the Hatch
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen = true;

		if (HasProbe) {
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		} else {
			probeidx = -1;
			probeextidx = -1;
		}
	}
}
*/


void Saturn::SetCSMStage ()

{
	ClearMeshes();
    ClearThrusterDefinitions();

	//
	// Delete any dangling propellant resources.
	//

	if (ph_ullage1)
	{
		DelPropellantResource(ph_ullage1);
		ph_ullage1 = 0;
	}

	if (ph_ullage2)
	{
		DelPropellantResource(ph_ullage2);
		ph_ullage2 = 0;
	}

	if (ph_ullage3)
	{
		DelPropellantResource(ph_ullage3);
		ph_ullage3 = 0;
	}

	if(ph_3rd) {
		DelPropellantResource(ph_3rd);
		ph_3rd = 0;
	}

	if (ph_sep) {
		DelPropellantResource(ph_sep);
		ph_sep = 0;
	}

	if (ph_sep2) {
		DelPropellantResource(ph_sep2);
		ph_sep2 = 0;
	}

	SetSize(10);
	SetCOG_elev(3.5);
	SetEmptyMass(CM_Mass + SM_EmptyMass);

	// ************************* propellant specs **********************************
	if (!ph_sps) {
		ph_sps  = CreatePropellantResource(SM_FuelMass, SM_FuelMass); //SPS stage Propellant
	}

	if (ApolloExploded && !ph_o2_vent) {

		double tank_mass = CSM_O2TANK_CAPACITY / 500.0;

		ph_o2_vent = CreatePropellantResource(tank_mass, tank_mass); //SPS stage Propellant

		TankQuantities t;
		GetTankQuantities(t);

		SetPropellantMass(ph_o2_vent, t.O2Tank1QuantityKg + t.O2Tank2QuantityKg);
	}

	SetDefaultPropellantResource (ph_sps); // display SPS stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	th_main[0] = CreateThruster(_V(-SPS_YAW_OFFSET * RAD * 5.0, -SPS_PITCH_OFFSET * RAD * 5.0, -5.0), _V(0, 0, 1), SPS_THRUST, ph_sps, SPS_ISP);

	DelThrusterGroup(THGROUP_MAIN, true);
	thg_main = CreateThrusterGroup(th_main, 1, THGROUP_MAIN);

	AddExhaust(th_main[0], 20.0, 2.25, SMExhaustTex);
	//SetPMI(_V(12, 12, 7));
	SetPMI(_V(4.3972, 4.6879, 1.6220));
	SetCrossSections(_V(40,40,14));
	SetCW(0.1, 0.3, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,0.3));
	SetPitchMomentScale(0);
	SetBankMomentScale(0);
	SetLiftCoeffFunc(0);

	const double CGOffset = 12.25+21.5-1.8+0.35;
	AddSM(30.25 - CGOffset, true);

	VECTOR3 mesh_dir;

	//
	// Skylab SM and Apollo 7 have no HGA.
	//
	if (!NoHGA) {
		mesh_dir=_V(-1.308,-1.18,29.042-CGOffset);
		AddMesh (hSMhga, &mesh_dir);
	}

	mesh_dir=_V(0, 0, 34.4 - CGOffset);

	UINT meshidx;
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		cmpidx = AddMesh (hCMP, &mesh_dir);
		crewidx = AddMesh (hCREW, &mesh_dir);
		SetCrewMesh();
	} else {
		cmpidx = -1;
		crewidx = -1;
	}

	meshidx = AddMesh (hCMInt, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

	//Don't Forget the Hatch
	sidehatchidx = AddMesh (hFHC, &mesh_dir);
	sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
	SetSideHatchMesh();

	meshidx = AddMesh (hCMVC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VC);
	VCMeshOffset = mesh_dir;

	// Docking probe
	if (HasProbe) {
		probeidx = AddMesh(hprobe, &mesh_dir);
		probeextidx = AddMesh(hprobeext, &mesh_dir);
		SetDockingProbeMesh();
	} else {
		probeidx = -1;
		probeextidx = -1;
	}

	// Optics Cover
	opticscoveridx = AddMesh (hopticscover, &mesh_dir);
	SetOpticsCoverMesh();

	// Docking port
	VECTOR3 dockpos = {0,0,35.90-CGOffset};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams(dockpos, dockdir, dockrot);

	//
	// SM RCS
	//
	AddRCSJets(-0.18, SM_RCS_THRUST);

	//
	// CM RCS
	//
	AddRCS_CM(CM_RCS_THRUST, 34.4 - CGOffset, false);

	//
	// Waste dump streams
	//
	wastewaterdump_spec.tex = oapiRegisterParticleTexture("ProjectApollo/WaterDump");
	if (wastewaterdump) DelExhaustStream(wastewaterdump);
	wastewaterdump = AddParticleStream(&wastewaterdump_spec, _V(-1.258, 1.282, 33.69 - CGOffset), _V(-0.57, 0.57, 0.59), WaterController.GetWasteWaterDumpLevelRef());

	urinedump_spec.tex = oapiRegisterParticleTexture("ProjectApollo/UrineDump");
	if (urinedump) DelExhaustStream(urinedump);
	urinedump = AddParticleStream(&urinedump_spec, _V(-1.358, 1.192, 33.69 - CGOffset), _V(-0.57, 0.57, 0.59), WaterController.GetUrineDumpLevelRef());

	//
	// Apollo 13 special handling
	//
	if (ApolloExploded) {
		VECTOR3 vent_pos = {0, 1.5, 30.25 - CGOffset};
		VECTOR3 vent_dir = {0.5, 1, 0};

		th_o2_vent = CreateThruster (vent_pos, vent_dir, 450.0, ph_o2_vent, 300.0);
		AddExhaustStream(th_o2_vent, &o2_venting_spec);
	}

	SetView(0.4 + 1.8 - 0.35);

	// **************************** NAV radios *************************************
	InitNavRadios (4);
	EnableTransponder (true);
	OrbiterAttitudeToggle.SetActive(true);

	ThrustAdjust = 1.0;
}

void Saturn::CreateSIVBStage(char *config, VESSELSTATUS &vs1, bool SaturnVStage)

{
	char VName[256]="";

	GetApolloName(VName); strcat (VName, "-S4BSTG");
	hs4bM = oapiCreateVessel(VName, config, vs1);

	SIVBSettings S4Config;

	//
	// For now we'll only seperate the panels on ASTP.
	//

	S4Config.SettingsType.word = 0;
	S4Config.SettingsType.SIVB_SETTINGS_FUEL = 1;
	S4Config.SettingsType.SIVB_SETTINGS_GENERAL = 1;
	S4Config.SettingsType.SIVB_SETTINGS_MASS = 1;
	S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD = 1;
	S4Config.SettingsType.SIVB_SETTINGS_ENGINES = 1;
	S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD_INFO = 1;
	S4Config.Payload = SIVBPayload;
	S4Config.VehicleNo = VehicleNo;
	S4Config.EmptyMass = S4B_EmptyMass;
	S4Config.MainFuelKg = GetPropellantMass(ph_3rd);
	S4Config.PayloadMass = S4PL_Mass;
	S4Config.SaturnVStage = SaturnVStage;
	S4Config.MissionTime = MissionTime;
	S4Config.LowRes = LowRes;
	S4Config.ISP_VAC = ISP_THIRD_VAC;
	S4Config.THRUST_VAC = THRUST_THIRD_VAC;
	S4Config.PanelsHinged = !SLAWillSeparate;
	S4Config.SLARotationLimit = (double) SLARotationLimit;

	GetPayloadName(S4Config.PayloadName);

	S4Config.LMAscentFuelMassKg = LMAscentFuelMassKg;
	S4Config.LMDescentFuelMassKg = LMDescentFuelMassKg;
	S4Config.LMPad = LMPad;
	S4Config.LMPadCount = LMPadCount;
	sprintf(S4Config.LEMCheck, LEMCheck);
	S4Config.LEMCheckAuto = LEMCheckAuto;

	SIVB *SIVBVessel = static_cast<SIVB *> (oapiGetVesselInterface(hs4bM));
	SIVBVessel->SetState(S4Config);

	PayloadDataTransfer = true;
}

void Saturn::SetDockingProbeMesh() {

	if (probeidx == -1 || probeextidx == -1)
		return;

	if (HasProbe) {
		if (!dockingprobe.IsRetracted()) {
			SetMeshVisibilityMode(probeidx, MESHVIS_NEVER);
			SetMeshVisibilityMode(probeextidx, MESHVIS_VCEXTERNAL);
		} else {
			SetMeshVisibilityMode(probeidx, MESHVIS_VCEXTERNAL);
			SetMeshVisibilityMode(probeextidx, MESHVIS_NEVER);
		}
	} else {
		SetMeshVisibilityMode(probeidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(probeextidx, MESHVIS_NEVER);
	}
}

void Saturn::SetSideHatchMesh() {

	if (sidehatchidx == -1 || sidehatchopenidx == -1)
		return;

	if (SideHatch.IsOpen()) {
		SetMeshVisibilityMode(sidehatchidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(sidehatchopenidx, MESHVIS_VCEXTERNAL);
	} else {
		SetMeshVisibilityMode(sidehatchidx, MESHVIS_VCEXTERNAL);
		SetMeshVisibilityMode(sidehatchopenidx, MESHVIS_NEVER);
	}

	if (sidehatchburnedidx == -1 || sidehatchburnedopenidx == -1)
		return;

	if (!Burned) {
		SetMeshVisibilityMode(sidehatchburnedidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(sidehatchburnedopenidx, MESHVIS_NEVER);
		return;
	}

	SetMeshVisibilityMode(sidehatchidx, MESHVIS_NEVER);
	SetMeshVisibilityMode(sidehatchopenidx, MESHVIS_NEVER);

	if (SideHatch.IsOpen()) {
		SetMeshVisibilityMode(sidehatchburnedidx, MESHVIS_NEVER);
		SetMeshVisibilityMode(sidehatchburnedopenidx, MESHVIS_VCEXTERNAL);
	} else {
		SetMeshVisibilityMode(sidehatchburnedidx, MESHVIS_VCEXTERNAL);
		SetMeshVisibilityMode(sidehatchburnedopenidx, MESHVIS_NEVER);
	}
}


void Saturn::SetCrewMesh() {

	if (cmpidx != -1) {
		if (Crewed && (Crew->number == 1 || Crew->number >= 3)) {
			SetMeshVisibilityMode(cmpidx, MESHVIS_VCEXTERNAL);
		} else {
			SetMeshVisibilityMode(cmpidx, MESHVIS_NEVER);
		}
	}
	if (crewidx != -1) {
		if (Crewed && Crew->number >= 2) {
			SetMeshVisibilityMode(crewidx, MESHVIS_VCEXTERNAL);
		} else {
			SetMeshVisibilityMode(crewidx, MESHVIS_NEVER);
		}
	}
}

void Saturn::SetOpticsCoverMesh() {

	if (opticscoveridx == -1)
		return;
	
	if (optics.OpticsCovered) {
		SetMeshVisibilityMode(opticscoveridx, MESHVIS_EXTERNAL);
	} else {
		SetMeshVisibilityMode(opticscoveridx, MESHVIS_NEVER);
	}
}

void Saturn::SetNosecapMesh() {

	if (nosecapidx == -1)
		return;

	if (NosecapAttached) {
		SetMeshVisibilityMode(nosecapidx, MESHVIS_EXTERNAL);
	}
	else {
		SetMeshVisibilityMode(nosecapidx, MESHVIS_NEVER);
	}
}

void Saturn::SetReentryStage ()

{
    ClearThrusters();
	ClearPropellants();
	ClearAirfoilDefinitions();

	//
	// Tell AGC the CM has seperated from the SM.
	//

	agc.SetInputChannelBit(030, CMSMSeperate, true);

	double EmptyMass = CM_EmptyMass + (LESAttached ? 2000.0 : 0.0);

	SetSize(6.0);
	if (ApexCoverAttached) {
		SetCOG_elev(1);
		SetTouchdownPoints(_V(0, -10, -1), _V(-10, 10, -1), _V(10, 10, -1));
	} else {
		SetCOG_elev(2.2);
		SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));
	}
	SetEmptyMass (EmptyMass);
	SetPMI(_V(1.25411, 1.11318, 1.41524)); //Calculated from CSM-109 Mass Properties at CM/SM Separation
	//SetPMI (_V(12, 12, 7));
	//SetPMI (_V(1.5,1.35,1.35));
	SetCrossSections (_V(9.17,7.13,7.0));
	SetCW (5.5, 0.1, 3.4, 3.4);
	SetRotDrag (_V(0.07,0.07,0.003));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1 && !LESAttached) {
		CreateAirfoil(LIFT_VERTICAL, _V(0.0, 0.12, 1.12), CoeffFunc, 3.5, 11.95, 1.0);
    }

	SetReentryMeshes();
	if (ApexCoverAttached) {
		SetView(-0.15);
	} else {
		SetView(-1.35);
	}
	if (CMTex) SetReentryTexture(CMTex, 1e6, 5, 0.7);

	// CM RCS
	double CGOffset = 34.4;
	if (ApexCoverAttached) {
		AddRCS_CM(CM_RCS_THRUST);
	} else {
		AddRCS_CM(CM_RCS_THRUST, -1.2);
		CGOffset += 1.2;
	}

	if (LESAttached) {
		if (!ph_let)
			ph_let  = CreatePropellantResource(1405.0);

		SetDefaultPropellantResource (ph_let); // display LET propellant level in generic HUD

		//
		// *********************** thruster definitions ********************************
		//

		VECTOR3 m_exhaust_pos1 = _V(0.0, -0.5, TowerOffset-2.2);
		VECTOR3 m_exhaust_pos2 = _V(0.0, 0.5, TowerOffset-2.2);
		VECTOR3 m_exhaust_pos3 = _V(-0.5, 0.0, TowerOffset-2.2);
		VECTOR3 m_exhaust_pos4 = _V(0.5, 0.0, TowerOffset-2.2);

		//
		// Main thrusters.
		//

		th_let[0] = CreateThruster (m_exhaust_pos1, _V(0.0, 0.4, 0.7), THRUST_VAC_LET, ph_let, ISP_LET_VAC, ISP_LET_SL);
		th_let[1] = CreateThruster (m_exhaust_pos2, _V(0.0, -0.4, 0.7),  THRUST_VAC_LET, ph_let, ISP_LET_VAC, ISP_LET_SL);
		th_let[2] = CreateThruster (m_exhaust_pos3, _V(0.4, 0.0, 0.7), THRUST_VAC_LET, ph_let, ISP_LET_VAC, ISP_LET_SL);
		th_let[3] = CreateThruster (m_exhaust_pos4, _V(-0.4, 0.0, 0.7), THRUST_VAC_LET, ph_let, ISP_LET_VAC, ISP_LET_SL);

		//
		// Add exhausts
		//

		int i;
		for (i = 0; i < 4; i++)
		{
			AddExhaust (th_let[i], 8.0, 0.5, SIVBRCSTex);
			AddExhaustStream (th_let[i], &let_exhaust);
		}

		thg_let = CreateThrusterGroup (th_let, 4, THGROUP_MAIN);
	}

	VECTOR3 dockpos = {0, 0, 1.5};
	VECTOR3 dockdir = {0, 0, 1};
	VECTOR3 dockrot = {0, 1, 0};
	SetDockParams(dockpos, dockdir, dockrot);

	if (!DrogueS.isValid())
		soundlib.LoadMissionSound(DrogueS, DROGUES_SOUND);

	//
	// Waste dump streams
	//

	wastewaterdump_spec.tex = oapiRegisterParticleTexture("ProjectApollo/WaterDump");
	if (wastewaterdump) DelExhaustStream(wastewaterdump);
	wastewaterdump = AddParticleStream(&wastewaterdump_spec, _V(-1.258, 1.282, 33.69 - CGOffset), _V(-0.57, 0.57, 0.59), WaterController.GetWasteWaterDumpLevelRef());

	urinedump_spec.tex = oapiRegisterParticleTexture("ProjectApollo/UrineDump");
	if (urinedump) DelExhaustStream(urinedump);
	urinedump = AddParticleStream(&urinedump_spec, _V(-1.358, 1.192, 33.69 - CGOffset), _V(-0.57, 0.57, 0.59), WaterController.GetUrineDumpLevelRef());
}

void Saturn::SetReentryMeshes() {

	ClearMeshes();

	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,0,0);
	if (Burned)	{
		if (ApexCoverAttached) {
			meshidx = AddMesh (hCMB, &mesh_dir);
		} else {
			mesh_dir=_V(0, 0, -1.2);
			meshidx = AddMesh (hCM2B, &mesh_dir);
		}
	} else {
		if (ApexCoverAttached) {
			meshidx = AddMesh (hCM, &mesh_dir);
		} else {
			mesh_dir=_V(0, 0, -1.2);
			meshidx = AddMesh (hCM2, &mesh_dir);
		}
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (LESAttached) {
		TowerOffset = 4.95;
		VECTOR3 mesh_dir_tower = mesh_dir + _V(0, 0, TowerOffset);

		meshidx = AddMesh (hsat5tower, &mesh_dir_tower);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	// And the Crew
	if (Crewed) {		
		cmpidx = AddMesh (hCMP, &mesh_dir);
		crewidx = AddMesh (hCREW, &mesh_dir);
		SetCrewMesh();
	} else {
		cmpidx = -1;
		crewidx = -1;
	}

	meshidx = AddMesh (hCMInt, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

	// Hatch
	sidehatchidx = AddMesh (hFHC, &mesh_dir);
	sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
	sidehatchburnedidx = AddMesh (hFHC2, &mesh_dir);
	sidehatchburnedopenidx = AddMesh (hFHO2, &mesh_dir);
	SetSideHatchMesh();

	meshidx = AddMesh (hCMVC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VC);

	//
	// Docking probe
	//

	if (HasProbe)
	{
		probeidx = AddMesh(hprobe, &mesh_dir);
		probeextidx = AddMesh(hprobeext, &mesh_dir);
		SetDockingProbeMesh();
	} else
	{
		probeidx = -1;
		probeextidx = -1;
	}
	VCMeshOffset = mesh_dir;
}

void Saturn::StageSeven(double simt)

{
	if (!Crewed)
	{
		switch (StageState) {
		case 0:
			if (GetAltitude() < 350000) {
				SlowIfDesired();
				ActivateCMRCS();
				ActivateNavmode(NAVMODE_RETROGRADE);
				StageState++;
			}
			break;
		}
	}

	// Entry heat according to Orbiter reference manual
	double entryHeat = 0.5 * GetAtmDensity() * pow(GetAirspeed(), 3);
	if (entryHeat > 2e7 ) { // We 're looking wether the CM has burned or not
		Burned = true;
		SetReentryMeshes();

		ClearThrusters();
		AddRCS_CM(CM_RCS_THRUST);

		SetStage(CM_ENTRY_STAGE);
		SetView(-0.15);
	}
}

void Saturn::StageEight(double simt)

{
	SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));

	// Mark apex as detached
	ApexCoverAttached = false;
	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);

	SetView(-1.35);

	if (!Crewed)
	{
		switch (StageState) {
		case 0:
			if (GetAltitude() < 50000) {
				SlowIfDesired();
				DeactivateNavmode(NAVMODE_RETROGRADE);
				DeactivateCMRCS();
				StageState++;
			}
			break;
		}
	}

	//
	// Create the apex cover vessel
	//
	VECTOR3 posOffset = _V(0, 0, 0);
	VECTOR3 velOffset = _V(0, 0, 3);

	VESSELSTATUS vs;
	GetStatus(vs);
	Local2Rel(posOffset, vs.rpos);
	VECTOR3 vog;
	GlobalRot(velOffset, vog);
	vs.rvel += vog;

	char VName[256]="";
	GetApolloName(VName);
	strcat(VName, "-APEX");
	if (Burned) {
		hApex = oapiCreateVessel(VName,"ProjectApollo/CMBapex", vs);
	} else {
		hApex = oapiCreateVessel(VName,"ProjectApollo/CMapex", vs);
	}

	// New stage
	SetStage(CM_ENTRY_STAGE_TWO);
}

void Saturn::SetChuteStage1()
{
	SetSize(15);
	SetCOG_elev(2.2);
	SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));
	SetEmptyMass(CM_EmptyMass);
	ClearAirfoilDefinitions();
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,80.0));
	SetCW(1.0, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale(-5e-3);
		SetBankMomentScale(-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();
	
	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);

	DeactivateNavmode(NAVMODE_KILLROT);
}

void Saturn::SetChuteStage2()
{
	SetSize(22);
	SetCOG_elev(2.2);
	SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));
	SetEmptyMass (CM_EmptyMass);
	SetPMI (_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,140.0));
	SetCW (1.0, 1.5, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);
}

void Saturn::SetChuteStage3()
{
	SetSize(22);
	SetCOG_elev(2.2);
	SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));
	SetEmptyMass (CM_EmptyMass);
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,480.0));
	SetCW(0.7, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale(-5e-3);
		SetBankMomentScale(-5e-3);
	}
	SetLiftCoeffFunc (0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);
}

void Saturn::SetChuteStage4()
{
	SetSize(22);
	SetCOG_elev(2.2);
	SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));
	SetEmptyMass(CM_EmptyMass);
	SetPMI(_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,3280.0));
	SetCW (0.7, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);
	SetView(-1.35);
}

void Saturn::SetSplashStage()
{
	SetSize(6.0);
	SetCOG_elev(2.2);
	SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));
	SetEmptyMass(CM_EmptyMass);
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,7.0));
	SetCW(0.5, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetReentryMeshes();

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);

	dyemarker_spec.tex = oapiRegisterParticleTexture("ProjectApollo/Dyemarker");
	if (dyemarker) DelExhaustStream(dyemarker);
	dyemarker = AddParticleStream(&dyemarker_spec, _V(-0.5, 1.5, -2), _V(-0.8660254, 0.5, 0), els.GetDyeMarkerLevelRef());

	SetView(-1.35);
}

void Saturn::SetRecovery()

{
	SetSize(10.0);
	SetCOG_elev(2.2);
	SetTouchdownPoints(_V(0, -10, -2.2), _V(-10, 10, -2.2), _V(10, 10, -2.2));
	SetEmptyMass(CM_EmptyMass);
	SetPMI(_V(20,20,12));
	SetCrossSections(_V(2.8,2.8,7.0));
	SetCW(0.5, 1.5, 1.4, 1.4);
	SetRotDrag(_V(0.7,0.7,1.2));
	SetSurfaceFrictionCoeff(1, 1);
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc(0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	// Meshes
	ClearMeshes();

	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,0,-1.2);
	if (Burned)	{
		meshidx = AddMesh (hCM2B, &mesh_dir);
	} else {
		meshidx = AddMesh (hCM2, &mesh_dir);
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// Hatch
	sidehatchidx = AddMesh (hFHC, &mesh_dir);
	sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
	sidehatchburnedidx = AddMesh (hFHC2, &mesh_dir);
	sidehatchburnedopenidx = AddMesh (hFHO2, &mesh_dir);
	SetSideHatchMesh();

	meshidx = AddMesh (hCMInt, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

	meshidx = AddMesh (hCMVC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VC);
	VCMeshOffset = mesh_dir;

	if (Crewed) {
		mesh_dir =_V(2.7,1.8,-1.5);
		meshidx = AddMesh (hCRB, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	ClearThrusters();
	AddRCS_CM(CM_RCS_THRUST, -1.2);

	dyemarker_spec.tex = oapiRegisterParticleTexture("ProjectApollo/Dyemarker");
	if (dyemarker) DelExhaustStream(dyemarker);
	dyemarker = AddParticleStream(&dyemarker_spec, _V(-0.5, 1.5, -2), _V(-0.8660254, 0.5, 0), els.GetDyeMarkerLevelRef());

	SetView(-1.35);
}

bool Saturn::clbkLoadGenericCockpit ()

{
	TRACESETUP("Saturn::clbkLoadGenericCockpit");

	//
	// VC-only in engineering camera view.
	//

	if (viewpos == SATVIEW_ENG1 || viewpos == SATVIEW_ENG2 || viewpos == SATVIEW_ENG3)
		return false;

	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
	oapiCameraSetCockpitDir(0,0);
	InVC = false;
	InPanel = false;

	SetView();
	return true;
}

//
// Generic function to jettison the escape tower.
//

void Saturn::JettisonLET(bool UseMain, bool AbortJettison)

{
	//
	// Don't do anything if the tower isn't attached!
	//
	if (!LESAttached)
		return;

	//
	// If the jettison motor fails and we're trying to
	// use it for the jettison, return.
	//
	// We'll always give them one way to jettison the LES as
	// being unable to jettison it is fatal.
	//
	if (!UseMain && LaunchFail.LESJetMotorFail)
		return;

	//
	// Otherwise jettison the LES.
	//
	VECTOR3 ofs1 = _V(0.0, 0.0, TowerOffset);
	VECTOR3 vel1 = _V(0.0,0.0,0.5);

	VESSELSTATUS vs1;
	GetStatus (vs1);

	vs1.eng_main = vs1.eng_hovr = 0.0;

	//
	// We must set status to zero to ensure the LET is in 'free flight'. Otherwise if we jettison
	// on the pad, the LET thinks it's on the ground!
	//

	vs1.status = 0;

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};

	Local2Rel (ofs1, vs1.rpos);

	GlobalRot (vel1, rofs1);

	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;

	TowerJS.play();
	TowerJS.done();

	char VName[256];

	GetApolloName(VName);
	strcat (VName, "-TWR");

	hesc1 = oapiCreateVessel(VName, "ProjectApollo/LES", vs1);
	LESAttached = false;

	LESSettings LESConfig;

	LESConfig.SettingsType.word = 0;
	LESConfig.SettingsType.LES_SETTINGS_GENERAL = 1;
	LESConfig.SettingsType.LES_SETTINGS_ENGINES = 1;
	LESConfig.SettingsType.LES_SETTINGS_THRUST = 1;

	//
	// Pressing the LES jettison button fires the main LET engine. The TWR JETT
	// switches jettison the LES and fire the jettison engines.
	//
	/// \todo If the LES jettison button is pressed before using the TWR JETT switches,
	/// the explosive bolts won't fire, so the main LET motor will fire while
	/// still attached to the CM!
	///
	/// See: AOH 2.9.4.8.4
	///

	LESConfig.FireMain = UseMain;

	LESConfig.MissionTime = MissionTime;
	LESConfig.VehicleNo = VehicleNo;
	LESConfig.LowRes = LowRes;
	LESConfig.ProbeAttached = AbortJettison && HasProbe;
	LESConfig.ISP_LET_SL = ISP_LET_SL;
	LESConfig.ISP_LET_VAC = ISP_LET_VAC;
	LESConfig.THRUST_VAC_LET = THRUST_VAC_LET;

	//
	// If this is the CSM abort stage, we need to transfer fuel information from
	// the CSM LET.
	//
	// Usually this will be zero, so you'd better use the right jettison button!
	//
	/// \todo At  some point we should expand this so that we can jettison the LES
	/// while the main abort motor is running.
	///

	if (ph_let)
	{
		LESConfig.MainFuelKg = GetPropellantMass(ph_let);
		LESConfig.SettingsType.LES_SETTINGS_MAIN_FUEL = 1;
	}

	LES *les_vessel = (LES *) oapiGetVesselInterface(hesc1);
	les_vessel->SetState(LESConfig);

	//
	// AOH SECS page 2.9-8 says that in the case of an abort, the docking probe is pulled away
	// from the CM by the LES when it's jettisoned.
	//
	if (AbortJettison)
	{
		dockingprobe.SetEnabled(false);
		HasProbe = false;
	}
	else
	{
		//
		// Enable docking probe because the tower is gone
		//
		dockingprobe.SetEnabled(HasProbe);			
	}

	ConfigureStageMeshes(stage);

	if (Crewed)
	{
		SwindowS.play();
	}
	SwindowS.done();

	//
	// Event management
	//

	if (eventControl.TOWER_JETTISON == MINUS_INFINITY)
		eventControl.TOWER_JETTISON = MissionTime;
}

void Saturn::JettisonDockingProbe() 

{
	char VName[256];

	// Use VC offset to calculate the docking probe offset
	VECTOR3 ofs = _V(0, 0, CurrentViewOffset + 0.25);
	VECTOR3 vel = {0.0, 0.0, 2.5};
	VESSELSTATUS vs4b;
	GetStatus (vs4b);
	StageTransform(this, &vs4b,ofs,vel);
	vs4b.vrot.x = 0.0;
	vs4b.vrot.y = 0.0;
	vs4b.vrot.z = 0.0;
	GetApolloName(VName); 
	strcat (VName, "-DCKPRB");
	hPROBE = oapiCreateVessel(VName, "ProjectApollo/CMprobe", vs4b);
}

void Saturn::JettisonOpticsCover() 

{
	char VName[256];

	// Use VC offset to calculate the optics cover offset
	VECTOR3 ofs = _V(0, 0, CurrentViewOffset + 0.25);
	VECTOR3 vel = {0.0, -0.16, 0.1};
	VESSELSTATUS vs4b;
	GetStatus (vs4b);
	StageTransform(this, &vs4b, ofs, vel);
	vs4b.vrot.x = 0.05;
	vs4b.vrot.y = 0.0;
	vs4b.vrot.z = 0.0;
	GetApolloName(VName); 
	strcat (VName, "-OPTICSCOVER");
	hOpticsCover = oapiCreateVessel(VName, "ProjectApollo/CMOpticsCover", vs4b);
}

void Saturn::JettisonNosecap()

{
	char VName[256];

	// Use VC offset to calculate the optics cover offset
	VECTOR3 ofs = _V(0, 0, CurrentViewOffset + 0.25);
	VECTOR3 vel = { 0.0, 0.0, 2.5 };
	VESSELSTATUS vs4b;
	GetStatus(vs4b);
	StageTransform(this, &vs4b, ofs, vel);
	vs4b.vrot.x = 0.0;
	vs4b.vrot.y = 0.0;
	vs4b.vrot.z = 0.0;
	GetApolloName(VName);
	strcat(VName, "-NOSECAP");
	hNosecapVessel = oapiCreateVessel(VName, "ProjectApollo/Sat1Aerocap", vs4b);
}
