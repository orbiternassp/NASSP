/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SIVb class

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
#include "orbiterSDK.h"

#include "nasspdefs.h"
#include "soundlib.h"
#include "OrbiterMath.h"

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "powersource.h"
#include "connector.h"
#include "iu.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "lemcomputer.h"
#include "imu.h"

#include "payload.h"
#include "sivb.h"
#include "astp.h"
#include "lem.h"

#include <stdio.h>
#include <string.h>

//
// Meshes are globally loaded.
//

static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg2low;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;
static MESHHANDLE hsat5stg3low;
static MESHHANDLE hsat5stg31low;
static MESHHANDLE hsat5stg32low;
static MESHHANDLE hsat5stg33low;
static MESHHANDLE hsat5stg34low;
static MESHHANDLE hastp;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hLMPKD;
static MESHHANDLE hapollo8lta;
static MESHHANDLE hlta_2r;
static MESHHANDLE hlm_1;

static SURFHANDLE SMMETex;

// "fuel venting" particle streams
static PARTICLESTREAMSPEC fuel_venting_spec = {
	0,		// flag
	0.8,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	20,		// lifetime
	0.15,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.6, 0.6,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

//
// Spew out particles to simulate the junk thrown out by stage
// seperation explosives.
//

static PARTICLESTREAMSPEC seperation_junk = {
	0,		// flag
	0.02,	// size
	1000,	// rate
	0.3,    // velocity
	5.0,    // velocity distribution
	100,	// lifetime
	0,	    // growthrate
	0,      // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

void SIVbLoadMeshes()

{
	//
	// Saturn 1b.
	//

	hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
	hSat1stg2low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg2");
	hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
	hastp = oapiLoadMeshGlobal ("ProjectApollo/nASTP3");
	hCOAStarget = oapiLoadMeshGlobal ("ProjectApollo/sat_target");
	hlm_1 = oapiLoadMeshGlobal ("ProjectApollo/LM_1");

	//
	// Saturn V
	//

	hsat5stg3 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg3");
	hsat5stg31 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg31");
	hsat5stg32 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg32");
	hsat5stg33 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg33");
	hsat5stg34 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg34");

	hsat5stg3low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg3");
	hsat5stg31low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg31");
	hsat5stg32low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg32");
	hsat5stg33low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg33");
	hsat5stg34low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg34");

	hLMPKD = oapiLoadMeshGlobal ("ProjectApollo/LM_Parked");
	hapollo8lta = oapiLoadMeshGlobal ("ProjectApollo/apollo8_lta");
	hlta_2r = oapiLoadMeshGlobal ("ProjectApollo/LTA_2R");

	SMMETex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_j2");
	seperation_junk.tex = oapiRegisterParticleTexture("ProjectApollo/junk");
}

SIVB::SIVB (OBJHANDLE hObj, int fmodel) : ProjectApolloConnectorVessel(hObj, fmodel),
		SIVBToCSMPowerDrain("SIVBToCSMPower", Panelsdk)

{
	PanelSDKInitalised = false;

	InitS4b();
}

SIVB::~SIVB()

{
	//
	// Delete LM PAD data.
	//
	if (LMPad) {
		delete[] LMPad;
		LMPad = 0;
	}
}

void SIVB::InitS4b()

{
	int i;

	PayloadType = PAYLOAD_EMPTY;
	PanelsHinged = false;
	PanelsOpened = false;
	State = SIVB_STATE_SETUP;
	LowRes = false;

	J2IsActive = false;
	FuelVenting = false;

	hDock = 0;
	ph_aps = 0;
	ph_main = 0;
	thg_aps = 0;
	thg_sep = 0;
	thg_sepPanel = 0;

	EmptyMass = 15000.0;
	PayloadMass = 0.0;
	MainFuel = 5000.0;
	Realism = REALISM_DEFAULT;

	THRUST_THIRD_VAC = 1000.0;
	ISP_THIRD_VAC = 300.0;

	CurrentThrust = 0;
	RotationLimit = 0.25;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	for (i = 0; i < 10; i++)
		th_att_rot[i] = 0;
	for (i = 0; i < 2; i++)
		th_att_lin[i] = 0;

	th_main[0] = 0;
	panelProc = 0;
	panelProcPlusX = 0;
	panelTimestepCount = 0;
    panelMesh1SaturnV = -1;
	panelMesh2SaturnV  = -1;
	panelMesh3SaturnV  = -1;
	panelMesh4SaturnV  = -1;
    panelMesh1SaturnVLow = -1;
	panelMesh2SaturnVLow  = -1;
	panelMesh3SaturnVLow  = -1;
	panelMesh4SaturnVLow  = -1;
    panelMesh1Saturn1b = -1;
	panelMesh2Saturn1b  = -1;
	panelMesh3Saturn1b  = -1;
	panelMesh4Saturn1b  = -1;
    panelAnim = 0;
	panelAnimPlusX = 0;

	meshSivbSaturnV = -1;
	meshSivbSaturnVLow = -1;
	meshSivbSaturn1b = -1;
	meshSivbSaturn1bLow = -1;
	meshASTP_A = -1;
	meshASTP_B = -1;
	meshCOASTarget_A = -1;
	meshCOASTarget_B = -1;
	meshCOASTarget_C = -1;
	meshLMPKD = -1;
	meshApollo8LTA = -1;
	meshLTA_2r = -1;

	//
	// Default payload name.
	//
	sprintf(PayloadName, "%s-PAYLOAD", GetName());

	LMDescentFuelMassKg = 8375.0;
	LMAscentFuelMassKg = 2345.0;
	Payloaddatatransfer = false;

	//
	// LM PAD data.
	//

	LMPadCount = 0;
	LMPad = 0;
	LMPadLoadCount = 0;
	LMPadValueCount = 0;

	//
	// Set up the connections.
	//
	SIVBToCSMConnector.SetType(CSM_SIVB_DOCKING);

	IUCommandConnector.SetSIVb(this);
	csmCommandConnector.SetSIVb(this);

	SIVBToCSMPowerConnector.SetType(CSM_SIVB_POWER);
	SIVBToCSMPowerConnector.SetPowerDrain(&SIVBToCSMPowerDrain);

	if (!PanelSDKInitalised)
	{
		Panelsdk.RegisterVessel(this);
		Panelsdk.InitFromFile("ProjectApollo\\SIVBSystems");
		PanelSDKInitalised = true;
	}

	MainBattery = static_cast<Battery *> (Panelsdk.GetPointerByString("ELECTRIC:POWER_BATTERY"));

	SIVBToCSMPowerDrain.WireTo(MainBattery);

	//
	// Register docking connector so the CSM can find it.
	//
	RegisterConnector(0, &SIVBToCSMConnector);
}

void SIVB::Boiloff()

{
	if (Realism < 2)
		return;

	//
	// The SIVB stage boils off a small amount of fuel while in orbit.
	//
	// For the time being we'll ignore any thrust created by the venting
	// of this fuel.
	//

	if (ph_main) {
		double NewFuelMass = GetPropellantMass(ph_main) * 0.99998193;
		SetPropellantMass(ph_main, NewFuelMass);
	}
}

void SIVB::SetS4b()

{
	double mass = EmptyMass;

	ClearThrusterDefinitions();
	SetSize (15);
	SetPMI (_V(94,94,20));
	SetCOG_elev (10);
	SetCrossSections (_V(267, 267, 97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	HideAllMeshes();

	VECTOR3 dockpos = {0,0.03, 12.6};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {-0.8660254, -0.5, 0 };

	if (SaturnVStage)
	{
		if (LowRes) {
			SetMeshVisibilityMode(meshSivbSaturnVLow, MESHVIS_EXTERNAL);
		}
		else {
			SetMeshVisibilityMode(meshSivbSaturnV, MESHVIS_EXTERNAL);
		}

        // Hide unneeded meshes
        if (PanelsHinged || !PanelsOpened) {
			if (LowRes) {
				SetMeshVisibilityMode(panelMesh1SaturnVLow, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh2SaturnVLow, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh3SaturnVLow, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh4SaturnVLow, MESHVIS_EXTERNAL);
			}
			else {
				SetMeshVisibilityMode(panelMesh1SaturnV, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh2SaturnV, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh3SaturnV, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh4SaturnV, MESHVIS_EXTERNAL);
			}
		}
	}
	else {
		if (LowRes) {
			SetMeshVisibilityMode(meshSivbSaturn1bLow, MESHVIS_EXTERNAL);
		}
		else {
			SetMeshVisibilityMode(meshSivbSaturn1b, MESHVIS_EXTERNAL);
		}

        // Hide unneeded meshes
        if (PanelsHinged || !PanelsOpened) {
			SetMeshVisibilityMode(panelMesh1Saturn1b, MESHVIS_EXTERNAL);
			SetMeshVisibilityMode(panelMesh2Saturn1b, MESHVIS_EXTERNAL);
			SetMeshVisibilityMode(panelMesh3Saturn1b, MESHVIS_EXTERNAL);
			SetMeshVisibilityMode(panelMesh4Saturn1b, MESHVIS_EXTERNAL);
       }
	}

	switch (PayloadType) {
	case PAYLOAD_LEM:
		SetMeshVisibilityMode(meshLMPKD, MESHVIS_EXTERNAL);
		SetDockParams(dockpos, dockdir, dockrot);
		hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
		mass += PayloadMass;
		break;

	case PAYLOAD_LTA:
	case PAYLOAD_LTA6:
		SetMeshVisibilityMode(meshLTA_2r, MESHVIS_EXTERNAL);
		ClearDockDefinitions();
		mass += PayloadMass;
		break;

	case PAYLOAD_LTA8:
		SetMeshVisibilityMode(meshApollo8LTA, MESHVIS_EXTERNAL);
		ClearDockDefinitions();
		mass += PayloadMass;
		break;

	//
	// For now the docking adapter for the SIVB to Venus test flights is simulated
	// with the ASTP mesh and COAS target.
	//

	case PAYLOAD_DOCKING_ADAPTER:
		SetMeshVisibilityMode(meshASTP_A, MESHVIS_EXTERNAL);
		SetMeshVisibilityMode(meshCOASTarget_A, MESHVIS_EXTERNAL);
		dockpos = _V(0.0, 0.0, 9.1);
		dockrot = _V(-1.0, 0.0, 0);
		SetDockParams(dockpos, dockdir, dockrot);
		hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
		mass += PayloadMass;
		break;

	case PAYLOAD_EMPTY:
		ClearDockDefinitions();
		break;

	case PAYLOAD_TARGET:
		SetMeshVisibilityMode(meshCOASTarget_B, MESHVIS_EXTERNAL);
		ClearDockDefinitions();
		mass += PayloadMass;
		break;

	case PAYLOAD_ASTP:
		SetMeshVisibilityMode(meshASTP_B, MESHVIS_EXTERNAL);
		dockpos = _V(0.0, 0.15, 9.9);
		dockrot = _V(-1.0, 0.0, 0);
		SetDockParams(dockpos, dockdir, dockrot);
		hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
		mass += PayloadMass;
		break;

	case PAYLOAD_LM1:
		SetMeshVisibilityMode(meshLM_1, MESHVIS_EXTERNAL);
		mass += PayloadMass;
		break;
	}

	SetEmptyMass(mass);
	SetAnimation(panelAnim, panelProc);
	SetAnimation(panelAnimPlusX, panelProcPlusX);

	AddRCS_S4B();

	if (PayloadType == PAYLOAD_DOCKING_ADAPTER)
	{
		iu.SetVesselStats(ISP_THIRD_VAC, THRUST_THIRD_VAC);
		iu.SetMissionInfo(true, true, Realism, 0.0, 0.0);

		//
		// Set up the IU connections.
		//

		iu.ConnectToMultiConnector(&SIVBToCSMConnector);
		SIVBToCSMConnector.AddTo(&SIVBToCSMPowerConnector);
	}
	iu.ConnectToLV(&IUCommandConnector);
	SIVBToCSMConnector.AddTo(&csmCommandConnector);
}

void SIVB::clbkPreStep(double simt, double simdt, double mjd)

{
	MissionTime += simdt;

	//
	// Seperate or open the SLA panels.
	//

	if (panelTimestepCount < 2) {
		panelTimestepCount++;
	} else {
		if (PanelsHinged) {
			if (panelProc < RotationLimit) {
				// Activate separation junk
				if (thg_sep)
					SetThrusterGroupLevel(thg_sep, 1);

				panelProc = min(RotationLimit, panelProc + simdt / 40.0);
				SetAnimation(panelAnim, panelProc);
			}
			// Special handling Apollo 7
			if (VehicleNo == 205) {
				// The +X (+Y in Apollo axes) moved to about 25° only at first, 
				// during the rendezvous in orbit 19 (about MET 30h) the panel was found 
				// hinged completely, so we do that at MET 15h (see Mission Report 11.7)
				if (MissionTime < 15. * 3600.) {
					if (panelProcPlusX < 0.1388) {
						panelProcPlusX = min(0.1388, panelProcPlusX + simdt / 40.0);
						SetAnimation(panelAnimPlusX, panelProcPlusX);
					}
				} else if (panelProcPlusX < RotationLimit) {
					panelProcPlusX = min(RotationLimit, panelProcPlusX + simdt / 40.0);
					SetAnimation(panelAnimPlusX, panelProcPlusX);
				}
			} else if (panelProcPlusX < RotationLimit) {
				panelProcPlusX = min(RotationLimit, panelProcPlusX + simdt / 40.0);
				SetAnimation(panelAnimPlusX, panelProcPlusX);
			}
		}
		else if (!PanelsOpened) {			
			if (panelProc < RotationLimit) {
				// Activate separation junk
				if (thg_sep)
					SetThrusterGroupLevel(thg_sep, 1);

				panelProc = min(RotationLimit, panelProc + simdt / 40.0);
				SetAnimation(panelAnim, panelProc);
			} 
			else {
				char VName[256];

				//
				// I'm not sure that all this code is really needed, but
				// it came from saturn1bmesh.cpp.
				//

				VESSELSTATUS vs2;
				VESSELSTATUS vs3;
				VESSELSTATUS vs4;
				VESSELSTATUS vs5;
				VECTOR3 ofs2 = _V(0,0,0);
				VECTOR3 ofs3 = _V(0,0,0);
				VECTOR3 ofs4 = _V(0,0,0);
				VECTOR3 ofs5 = _V(0,0,0);
				VECTOR3 vel2 = _V(0,0,0);
				VECTOR3 vel3 = _V(0,0,0);
				VECTOR3 vel4 = _V(0,0,0);
				VECTOR3 vel5 = _V(0,0,0);

				GetStatus (vs2);
				GetStatus (vs3);
				GetStatus (vs4);
				GetStatus (vs5);

				vs2.eng_main = vs2.eng_hovr = 0.0;
				vs3.eng_main = vs3.eng_hovr = 0.0;
				vs4.eng_main = vs4.eng_hovr = 0.0;
				vs5.eng_main = vs5.eng_hovr = 0.0;

				if (SaturnVStage) {
					ofs2 = _V(-3.25, -3.3, 12.2);
					vel2 = _V(-0.5, -0.5, -0.55);
					ofs3 = _V(3.25, -3.3, 12.2);
					vel3 = _V(0.5, -0.5, -0.55);
					ofs4 = _V(3.25, 3.3, 12.2);
					vel4 = _V(0.5, 0.5, -0.55);
					ofs5 = _V(-3.25, 3.3, 12.2);
					vel5 = _V(-0.5, 0.5, -0.55);
				}
				else {
					ofs2 = _V(2.5, 2.5, 15.0);
					vel2 = _V(0.5, 0.5, -0.55);
					ofs3 = _V(-2.5, 2.5, 15.0);
					vel3 = _V(-0.5, 0.5, -0.55);
					ofs4 = _V(2.5, -2.5, 15.0);
					vel4 = _V(0.5, -0.5, -0.55);
					ofs5 = _V(-2.5, -2.5, 15.0);
					vel5 = _V(-0.5, -0.5, -0.55);
				}

				VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};
				VECTOR3 rofs3, rvel3 = {vs3.rvel.x, vs3.rvel.y, vs3.rvel.z};
				VECTOR3 rofs4, rvel4 = {vs4.rvel.x, vs4.rvel.y, vs4.rvel.z};
				VECTOR3 rofs5, rvel5 = {vs5.rvel.x, vs5.rvel.y, vs5.rvel.z};
				Local2Rel (ofs2, vs2.rpos);
				Local2Rel (ofs3, vs3.rpos);
				Local2Rel (ofs4, vs4.rpos);
				Local2Rel (ofs5, vs5.rpos);
				GlobalRot (vel2, rofs2);
				GlobalRot (vel3, rofs3);
				GlobalRot (vel4, rofs4);
				GlobalRot (vel5, rofs5);
				vs2.rvel.x = rvel2.x+rofs2.x;
				vs2.rvel.y = rvel2.y+rofs2.y;
				vs2.rvel.z = rvel2.z+rofs2.z;
				vs3.rvel.x = rvel3.x+rofs3.x;
				vs3.rvel.y = rvel3.y+rofs3.y;
				vs3.rvel.z = rvel3.z+rofs3.z;
				vs4.rvel.x = rvel4.x+rofs4.x;
				vs4.rvel.y = rvel4.y+rofs4.y;
				vs4.rvel.z = rvel4.z+rofs4.z;
				vs5.rvel.x = rvel5.x+rofs5.x;
				vs5.rvel.y = rvel5.y+rofs5.y;
				vs5.rvel.z = rvel5.z+rofs5.z;
	
				//
				// This should be rationalised really to use the same parameters
				// with different config files.
				//

				if (SaturnVStage) {
					vs2.vrot.x = -0.1;
					vs2.vrot.y = 0.1;
					vs2.vrot.z = 0.0;
					vs3.vrot.x = -0.1;
					vs3.vrot.y = -0.1;
					vs3.vrot.z = 0.0;
					vs4.vrot.x = 0.1;
					vs4.vrot.y = -0.1;
					vs4.vrot.z = 0.0;
					vs5.vrot.x = 0.1;
					vs5.vrot.y = 0.1;
					vs5.vrot.z = 0.0;
				
					GetApolloName(VName); strcat (VName, "-S4B1");
					hs4b1 = oapiCreateVessel(VName, "ProjectApollo/sat5stg31", vs2);
					GetApolloName(VName); strcat (VName, "-S4B2");
					hs4b2 = oapiCreateVessel(VName, "ProjectApollo/sat5stg32", vs3);
					GetApolloName(VName); strcat (VName, "-S4B3");
					hs4b3 = oapiCreateVessel(VName, "ProjectApollo/sat5stg33", vs4);
					GetApolloName(VName); strcat (VName, "-S4B4");
					hs4b4 = oapiCreateVessel(VName, "ProjectApollo/sat5stg34", vs5);

					MATRIX3 rv, rx, ry, rz, rnx, rny, rnz;
					GetRotationMatrix(rv);
					GetRotMatrixX(34.5 * RAD, rx); 
					GetRotMatrixX(-34.5 * RAD, rnx); 
					GetRotMatrixY(30 * RAD, ry); 
					GetRotMatrixY(-30 * RAD, rny); 
					GetRotMatrixZ(9.5 * RAD, rz); 
					GetRotMatrixZ(-9.5 * RAD, rnz); 

					VESSEL *v = oapiGetVesselInterface(hs4b1);
					v->SetRotationMatrix(mul(rv, mul(rz, mul(ry, rnx))));			
					v = oapiGetVesselInterface(hs4b2);
					v->SetRotationMatrix(mul(rv, mul(rnz, mul(rny, rnx))));			
					v = oapiGetVesselInterface(hs4b3);
					v->SetRotationMatrix(mul(rv, mul(rz, mul(rny, rx))));			
					v = oapiGetVesselInterface(hs4b4);
					v->SetRotationMatrix(mul(rv, mul(rnz, mul(ry, rx))));

				    // Hide unneeded meshes
					SetMeshVisibilityMode(panelMesh1SaturnV, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh2SaturnV, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh3SaturnV, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh4SaturnV, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh1SaturnVLow, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh2SaturnVLow, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh3SaturnVLow, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh4SaturnVLow, MESHVIS_NEVER);
				}
				else {
					vs2.vrot.x = 0.1;
					vs2.vrot.y = -0.1;
					vs2.vrot.z = 0.0;
					vs3.vrot.x = 0.1;
					vs3.vrot.y = 0.1;
					vs3.vrot.z = 0.0;
					vs4.vrot.x = -0.1;
					vs4.vrot.y = -0.1;
					vs4.vrot.z = 0.0;
					vs5.vrot.x = -0.1;
					vs5.vrot.y = 0.1;
					vs5.vrot.z = 0.0;

					GetApolloName(VName); strcat (VName, "-S4B1");
					hs4b1 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg21", vs2);
					GetApolloName(VName); strcat (VName, "-S4B2");
					hs4b2 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg22", vs3);
					GetApolloName(VName); strcat (VName, "-S4B3");
					hs4b3 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg23", vs4);
					GetApolloName(VName); strcat (VName, "-S4B4");
					hs4b4 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg24", vs5);

					MATRIX3 rv, rx, ry, rz, rnx, rny, rnz;
					GetRotationMatrix(rv);
					GetRotMatrixX(34.5 * RAD, rx); 
					GetRotMatrixX(-34.5 * RAD, rnx); 
					GetRotMatrixY(30.8 * RAD, ry); 
					GetRotMatrixY(-30.8 * RAD, rny); 
					GetRotMatrixZ(8.5 * RAD, rz); 
					GetRotMatrixZ(-8.5 * RAD, rnz); 

					VESSEL *v = oapiGetVesselInterface(hs4b1);
					v->SetRotationMatrix(mul(rv, mul(rz, mul(rny, rx))));			
					v = oapiGetVesselInterface(hs4b2);
					v->SetRotationMatrix(mul(rv, mul(rnz, mul(ry, rx))));			
					v = oapiGetVesselInterface(hs4b3);
					v->SetRotationMatrix(mul(rv, mul(rnz, mul(rny, rnx))));			
					v = oapiGetVesselInterface(hs4b4);
					v->SetRotationMatrix(mul(rv, mul(rz, mul(ry, rnx))));			

				    // Hide unneeded meshes
					SetMeshVisibilityMode(panelMesh1Saturn1b, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh2Saturn1b, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh3Saturn1b, MESHVIS_NEVER);
					SetMeshVisibilityMode(panelMesh4Saturn1b, MESHVIS_NEVER);
 				}

				// Activate panel separation junk
				if (thg_sepPanel)
					SetThrusterGroupLevel(thg_sepPanel, 1);

				PanelsOpened = true;
			}
		}
	}

	//
	// Attitude control
	//

	// Special handling Apollo 7
	if (VehicleNo == 205) {
		if (MissionTime >= 11815){ // GRR+11820, GRR is 5 seconds before liftoff
			// retrograde LVLH orbital-rate
			iu.SetLVLHAttitude(_V(-1, 0, 0));			
		} else {
			iu.HoldAttitude();
		}
	} else {
		// In all other missions maintain initial attitude for now
		// \todo Correct behaviour of the S-IVB 
		iu.HoldAttitude();
	}	

	//
	// Now update whatever needs updating.
	//

	switch (State) {
	case SIVB_STATE_WAITING:

		//
		// If we still have fuel left, boil some off.
		//

		if (MissionTime >= NextMissionEventTime) {
			Boiloff();
			NextMissionEventTime = MissionTime + 10.0;
		}
		break;
	}

	//
	// For a Saturn V SIVB, at some point it will dump all remaining fuel out the engine nozzle to
	// thrust it out of the way of the CSM.
	//

	iu.Timestep(MissionTime, simdt, mjd);
	Panelsdk.Timestep(MissionTime);
}


void SIVB::GetApolloName(char *s)

{
	sprintf(s, "AS-%d", VehicleNo);
}


void SIVB::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "S4PL", PayloadType);
	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_int (scn, "REALISM", Realism);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "PMASS", PayloadMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "T3V", THRUST_THIRD_VAC);
	oapiWriteScenario_float (scn, "I3V", ISP_THIRD_VAC);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "CTR", CurrentThrust);
	oapiWriteScenario_float (scn, "PANELPROC", panelProc);
	oapiWriteScenario_float (scn, "PANELPROCPLUSX", panelProcPlusX);
	oapiWriteScenario_float (scn, "ROTL", RotationLimit);

	if (PayloadName[0])
	{
		oapiWriteScenario_string (scn, "PAYN", PayloadName);
	}

	if (!Payloaddatatransfer)
	{
		oapiWriteScenario_float (scn, "LMDSCFUEL", LMDescentFuelMassKg);
		oapiWriteScenario_float (scn, "LMASCFUEL", LMAscentFuelMassKg);
		if (LMPadCount > 0 && LMPad) {
			oapiWriteScenario_int (scn, "LMPADCNT", LMPadCount);
			char str[64];
			for (int i = 0; i < LMPadCount; i++) {
				sprintf(str, "%04o %05o", LMPad[i * 2], LMPad[i * 2 + 1]);
				oapiWriteScenario_string (scn, "LMPAD", str);
			}
		}
	}

	iu.SaveState(scn);
	Panelsdk.Save(scn);
}

int SIVB::GetMainState()

{
	MainState state;

	state.word = 0;
	state.PanelsHinged = PanelsHinged;
	state.PanelsOpened = PanelsOpened;
	state.SaturnVStage = SaturnVStage;
	state.LowRes = LowRes;
	state.J2IsActive = J2IsActive;
	state.FuelVenting = FuelVenting;
	state.Payloaddatatransfer = Payloaddatatransfer;

	return state.word;
}

void SIVB::SetVentingThruster()

{
	//
	// Clear old thrusters.
	//
	if (thg_main)
		DelThrusterGroup(THGROUP_MAIN, true);

	th_main[0] = CreateThruster (mainExhaustPos, _V( 0,0,1), 1000.0, ph_main, 30.0, 30.0);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	AddExhaustStream(th_main[0], &fuel_venting_spec);

	J2IsActive = false;
}

void SIVB::SetActiveJ2Thruster()

{
	//
	// Clear old thrusters.
	//
	if (thg_main)
		DelThrusterGroup(THGROUP_MAIN, true);

	th_main[0] = CreateThruster (mainExhaustPos, _V( 0,0,1), THRUST_THIRD_VAC, ph_main, ISP_THIRD_VAC);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);
	AddExhaust (th_main[0], 25.0, 1.5, SMMETex);

	J2IsActive = true;
}

void SIVB::StartVenting()

{
	if (!J2IsActive && th_main[0])
	{
		FuelVenting = true;

		EnableDisableJ2(true);
		SetThrusterLevel(th_main[0], 1.0);
	}
}

void SIVB::StopVenting()

{
	if (!J2IsActive && th_main[0])
	{
		FuelVenting = false;

		SetThrusterLevel(th_main[0], 0.0);
		EnableDisableJ2(false);
	}
}

double SIVB::GetMainBatteryPower()

{
	if (MainBattery)
	{
		return MainBattery->Capacity();
	}

	return 0.0;
}


double SIVB::GetMainBatteryPowerDrain()

{
	if (MainBattery)
	{
		return MainBattery->Voltage() * MainBattery->Current();
	}

	return 0.0;
}

double SIVB::GetMainBatteryVoltage()

{
	if (MainBattery)
	{
		return MainBattery->Voltage();
	}

	return 0.0;
}


double SIVB::GetMainBatteryCurrent()

{
	if (MainBattery)
	{
		return MainBattery->Current();
	}

	return 0.0;
}

void SIVB::AddRCS_S4B()

{
	const double ATTCOOR2 = 3.61;
	const double TRANZ=-3.2;
	const double RCSX=0.35;
	VECTOR3 m_exhaust_pos2= {0,ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos3= {0,-ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos4= {-ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_pos5= {ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_ref2 = {0,0.1,-1};
	VECTOR3 m_exhaust_ref3 = {0,-0.1,-1};
	VECTOR3 m_exhaust_ref4 = {-0.1,0,-1};
	VECTOR3 m_exhaust_ref5 = {0.1,0,-1};
	double offset = -2.05;

	if (!ph_aps)
		ph_aps  = CreatePropellantResource(275.0);

	if (!ph_main)
		ph_main = CreatePropellantResource(MainFuel);

	SetDefaultPropellantResource (ph_main);
	
	mainExhaustPos = _V(0, 0, -11.7);

	//
	// Unless this is dockable, the main engine is just venting fuel through the exhaust: low thrust and low ISP.
	//

	if (J2IsActive)
	{
		SetActiveJ2Thruster();
	}
	else
	{
		SetVentingThruster();
	}

	//
	// Rotational thrusters are 150lb (666N) thrust. ISP is estimated at 3000.0.
	//
	// Unfortunately Orbiter can't handle the real figures, so for rotation we use fake ones that are far
	// more powerful.
	//

	double THRUST_APS_ROT = 5000.0; // 666.0;
	double ISP_APS_ROT = 100000.0; // 3000.0;

	SURFHANDLE SIVBRCSTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");

	th_att_rot[0] = CreateThruster (_V(0,ATTCOOR2+0.15,TRANZ-0.25+offset), _V(0, -1,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	th_att_rot[1] = CreateThruster (_V(0,-ATTCOOR2-0.15,TRANZ-0.25+offset), _V(0,1,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	
	AddExhaust (th_att_rot[0], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[1], 0.6, 0.078, SIVBRCSTex);
	CreateThrusterGroup (th_att_rot,   1, THGROUP_ATT_PITCHUP);
	CreateThrusterGroup (th_att_rot+1, 1, THGROUP_ATT_PITCHDOWN);

	th_att_rot[2] = CreateThruster (_V(RCSX,ATTCOOR2-0.2,TRANZ-0.25+offset), _V(-1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	th_att_rot[3] = CreateThruster (_V(-RCSX,-ATTCOOR2+0.2,TRANZ-0.25+offset), _V( 1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	th_att_rot[4] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V( 1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	th_att_rot[5] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);

	AddExhaust (th_att_rot[2], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[3], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[4], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[5], 0.6, 0.078, SIVBRCSTex);
	CreateThrusterGroup (th_att_rot+2, 2, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+4, 2, THGROUP_ATT_BANKRIGHT);

	th_att_rot[6] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	th_att_rot[7] = CreateThruster (_V(-RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	th_att_rot[8] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);
	th_att_rot[9] = CreateThruster (_V(RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(-1,0,0), THRUST_APS_ROT, ph_aps, ISP_APS_ROT, ISP_APS_ROT);

	AddExhaust (th_att_rot[6], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[7], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[8], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_att_rot[9], 0.6, 0.078, SIVBRCSTex);

	CreateThrusterGroup (th_att_rot+6, 2, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+8, 2, THGROUP_ATT_YAWRIGHT);

	//
	// APS linear thrusters are 320N (72 pounds) thrust
	//
	th_att_lin[0] = CreateThruster (_V(0,ATTCOOR2-0.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_aps, 3000.0, 3000.0);
	th_att_lin[1] = CreateThruster (_V(0,-ATTCOOR2+.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_aps, 3000.0, 3000.0);
	AddExhaust (th_att_lin[0], 7, 0.15, SIVBRCSTex);
	AddExhaust (th_att_lin[1], 7, 0.15, SIVBRCSTex);

	thg_aps = CreateThrusterGroup (th_att_lin, 2, THGROUP_ATT_FORWARD);

	if (FuelVenting)
	{
		StartVenting();
	}

	//
	// Seperation junk 'thrusters'.
	//

	int i;
	double junkOffset;
	junkOffset = 16;

	VECTOR3	s_exhaust_pos1= _V(1.41,1.41,junkOffset);
	VECTOR3 s_exhaust_pos2= _V(1.41,-1.41,junkOffset);
	VECTOR3	s_exhaust_pos3= _V(-1.41,1.41,junkOffset);
	VECTOR3 s_exhaust_pos4= _V(-1.41,-1.41,junkOffset);
	VECTOR3	s_exhaust_pos6= _V(0,2.0,junkOffset);
	VECTOR3 s_exhaust_pos7= _V(0,-2.0,junkOffset);
	VECTOR3	s_exhaust_pos8= _V(2.0,0,junkOffset);
	VECTOR3 s_exhaust_pos9= _V(-2.0,0,junkOffset);

	PROPELLANT_HANDLE ph_sep = CreatePropellantResource(0.2);

	THRUSTER_HANDLE th_sep[8];
	th_sep[0] = CreateThruster (s_exhaust_pos1, _V( -1,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[1] = CreateThruster (s_exhaust_pos2, _V( -1,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[2] = CreateThruster (s_exhaust_pos3, _V( 1,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[3] = CreateThruster (s_exhaust_pos4, _V( 1,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[4] = CreateThruster (s_exhaust_pos6, _V( 0,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[5] = CreateThruster (s_exhaust_pos7, _V( 0,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[6] = CreateThruster (s_exhaust_pos8, _V( -1,0,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[7] = CreateThruster (s_exhaust_pos9, _V( 1,0,0), 1.0, ph_sep, 10.0, 10.0);

	for (i = 0; i < 8; i++) {
		AddExhaustStream(th_sep[i], &seperation_junk);
	}
	thg_sep = CreateThrusterGroup(th_sep, 8, THGROUP_USER);

	//
	// Panel seperation junk 'thrusters'.
	//

	junkOffset = 9.4;

	double r = 2.15;
	VECTOR3	sPanel_exhaust_pos1= _V(r,r,junkOffset);
	VECTOR3 sPanel_exhaust_pos2= _V(r,-r,junkOffset);
	VECTOR3	sPanel_exhaust_pos3= _V(-r,r,junkOffset);
	VECTOR3 sPanel_exhaust_pos4= _V(-r,-r,junkOffset);

	PROPELLANT_HANDLE ph_sepPanel = CreatePropellantResource(0.1);

	THRUSTER_HANDLE th_sepPanel[4];
	th_sepPanel[0] = CreateThruster (sPanel_exhaust_pos1, _V( -1,-1,0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[1] = CreateThruster (sPanel_exhaust_pos2, _V( -1,1,0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[2] = CreateThruster (sPanel_exhaust_pos3, _V( 1,-1,0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[3] = CreateThruster (sPanel_exhaust_pos4, _V( 1,1,0), 1.0, ph_sepPanel, 10.0, 10.0);

	for (i = 0; i < 4; i++) {
		AddExhaustStream(th_sepPanel[i], &seperation_junk);
	}
	thg_sepPanel = CreateThrusterGroup(th_sepPanel, 4, THGROUP_USER);
}

void SIVB::SetMainState(int s)

{
	MainState state;

	state.word = s;

	SaturnVStage = (state.SaturnVStage != 0);
	PanelsHinged = (state.PanelsHinged != 0);
	PanelsOpened = (state.PanelsOpened != 0);
	LowRes = (state.LowRes != 0);
	J2IsActive = (state.J2IsActive);
	FuelVenting = (state.FuelVenting);
	Payloaddatatransfer = (state.Payloaddatatransfer != 0);
}

void SIVB::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline (scn, line))
	{
		if (!strnicmp(line, "S4PL", 4))
		{
			sscanf(line + 4, "%d", &PayloadType);
		}
		else if (!strnicmp (line, "MAINSTATE", 9))
		{
            int MainState = 0;;
			sscanf (line+9, "%d", &MainState);
			SetMainState(MainState);
		}
		else if (!strnicmp (line, "VECHNO", 6))
		{
			sscanf (line+6, "%d", &VehicleNo);
		}
		else if (!strnicmp (line, "EMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			EmptyMass = flt;
		}
		else if (!strnicmp (line, "PMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			PayloadMass = flt;
		}
		else if (!strnicmp (line, "FMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			MainFuel = flt;
		}
		else if (!strnicmp(line, "T3V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			THRUST_THIRD_VAC = flt;
		}
		else if (!strnicmp(line, "I3V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_THIRD_VAC = flt;
		}
		else if (!strnicmp(line, "MISSNTIME", 9))
		{
            sscanf (line+9, "%f", &flt);
			MissionTime = flt;
		}
		else if (!strnicmp(line, "NMISSNTIME", 10))
		{
            sscanf (line + 10, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp(line, "LMISSNTIME", 10))
		{
            sscanf (line + 10, "%f", &flt);
			LastMissionEventTime = flt;
		}
		else if (!strnicmp(line, "CTR", 3))
		{
            sscanf (line + 3, "%f", &flt);
			CurrentThrust = flt;
		}
		else if (!strnicmp(line, "ROTL", 4))
		{
            sscanf (line + 4, "%f", &flt);
			RotationLimit = flt;
		}
		else if (!strnicmp(line, "PANELPROCPLUSX", 14))
		{
            sscanf (line + 14, "%f", &flt);
			panelProcPlusX = flt;
		}
		else if (!strnicmp(line, "PANELPROC", 9))
		{
            sscanf (line + 9, "%f", &flt);
			panelProc = flt;
		}
		else if (!strnicmp (line, "STATE", 5))
		{
			int i;
			sscanf (line+5, "%d", &i);
			State = (SIVbState) i;
		}
		else if (!strnicmp(line, "PAYN", 4)) {
			strncpy (PayloadName, line + 5, 64);
		}
		else if (!strnicmp(line, "LMDSCFUEL", 9)) {
			sscanf(line + 9, "%f", &flt);
			LMDescentFuelMassKg = flt;
		}
		else if (!strnicmp(line, "LMASCFUEL", 9)) {
			sscanf(line + 9, "%f", &flt);
			LMAscentFuelMassKg = flt;
		}
		else if (!strnicmp (line, "LMPADCNT", 8)) {
			if (!LMPad) {
				sscanf (line+8, "%d", &LMPadCount);
				if (LMPadCount > 0) {
					LMPad = new unsigned int[LMPadCount * 2];
				}
			}
		}
		else if (!strnicmp (line, "LMPAD", 5)) {
			unsigned int addr, val;
			sscanf (line+5, "%o %o", &addr, &val);
			LMPadValueCount++;
			if (LMPad && LMPadLoadCount < (LMPadCount * 2)) {
				LMPad[LMPadLoadCount++] = addr;
				LMPad[LMPadLoadCount++] = val;
			}
		}
		else if (!strnicmp (line, "REALISM", 7))
		{
			sscanf (line+7, "%d", &Realism);
		}
		else if (!strnicmp(line, IU_START_STRING, sizeof(IU_START_STRING))) {
			iu.LoadState(scn);
		}
		else if (!strnicmp (line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
			Panelsdk.Load(scn);			//send the loading to the Panelsdk
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}

	SetS4b();
}

void SIVB::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	// Switch to compatible dock mode 
	SetDockMode(0);

	InitS4b();
    
    // Define all animations here and delete the unneeded later,
    // otherwise Orbiter crashes
    ClearMeshes();
	panelAnim = CreateAnimation(0.0);
	panelAnimPlusX = CreateAnimation(0.0);

    // SaturnV panel animations
	VECTOR3 mesh_dir = _V(-1.48, -1.48, 12.55);
	panelMesh1SaturnV = AddMesh(hsat5stg31, &mesh_dir);
	panelMesh1SaturnVLow = AddMesh(hsat5stg31low, &mesh_dir);
	mesh_dir = _V(1.48, -1.48, 12.55);
	panelMesh2SaturnV = AddMesh(hsat5stg32, &mesh_dir);
	panelMesh2SaturnVLow = AddMesh(hsat5stg32low, &mesh_dir);
	mesh_dir = _V(1.48, 1.48, 12.55);
	panelMesh3SaturnV = AddMesh(hsat5stg33, &mesh_dir);
	panelMesh3SaturnVLow = AddMesh(hsat5stg33low, &mesh_dir);
	mesh_dir = _V(-1.48, 1.48, 12.55);
	panelMesh4SaturnV  = AddMesh(hsat5stg34, &mesh_dir);
	panelMesh4SaturnVLow  = AddMesh(hsat5stg34low, &mesh_dir);

	static MGROUP_ROTATE panel1SaturnV(panelMesh1SaturnV, NULL, 0, _V(-0.6, -0.6, -3.2), _V(  1, -1, 0) / length(_V(  1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2SaturnV(panelMesh2SaturnV, NULL, 0, _V( 0.6, -0.6, -3.2), _V(  1,  1, 0) / length(_V(  1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3SaturnV(panelMesh3SaturnV, NULL, 0, _V( 0.6,  0.6, -3.2), _V( -1,  1, 0) / length(_V( -1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4SaturnV(panelMesh4SaturnV, NULL, 0, _V(-0.6,  0.6, -3.2), _V( -1, -1, 0) / length(_V( -1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel1SaturnVLow(panelMesh1SaturnVLow, NULL, 0, _V(-0.6, -0.6, -3.2), _V(  1, -1, 0) / length(_V(  1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2SaturnVLow(panelMesh2SaturnVLow, NULL, 0, _V( 0.6, -0.6, -3.2), _V(  1,  1, 0) / length(_V(  1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3SaturnVLow(panelMesh3SaturnVLow, NULL, 0, _V( 0.6,  0.6, -3.2), _V( -1,  1, 0) / length(_V( -1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4SaturnVLow(panelMesh4SaturnVLow, NULL, 0, _V(-0.6,  0.6, -3.2), _V( -1, -1, 0) / length(_V( -1, -1, 0)), (float)(1.0 * PI));

	AddAnimationComponent(panelAnim, 0, 1, &panel1SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel2SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel3SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel4SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel1SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel2SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel3SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel4SaturnVLow);

    // Saturn1b panel animations
	mesh_dir = _V(2.45, 0, 10.55);
    panelMesh1Saturn1b = AddMesh(hSat1stg21, &mesh_dir);
	mesh_dir = _V(0, 2.45, 10.55);
	panelMesh2Saturn1b = AddMesh(hSat1stg22, &mesh_dir);
	mesh_dir = _V(0, -2.45, 10.55);
	panelMesh3Saturn1b = AddMesh(hSat1stg23, &mesh_dir);
	mesh_dir = _V(-2.45, 0, 10.55);
	panelMesh4Saturn1b = AddMesh(hSat1stg24, &mesh_dir);

	static MGROUP_ROTATE panel1Saturn1b(panelMesh1Saturn1b, NULL, 0, _V( 0.37,  0,    -1.2), _V(  0,  1, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2Saturn1b(panelMesh2Saturn1b, NULL, 0, _V( 0,     0.37, -1.2), _V( -1,  0, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3Saturn1b(panelMesh3Saturn1b, NULL, 0, _V( 0,    -0.37, -1.2), _V(  1,  0, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4Saturn1b(panelMesh4Saturn1b, NULL, 0, _V(-0.37,  0,    -1.2), _V(  0, -1, 0), (float)(1.0 * PI));
    	
	AddAnimationComponent(panelAnimPlusX, 0, 1, &panel1Saturn1b);
	AddAnimationComponent(panelAnim,	  0, 1, &panel2Saturn1b);
	AddAnimationComponent(panelAnim,	  0, 1, &panel3Saturn1b);
	AddAnimationComponent(panelAnim,	  0, 1, &panel4Saturn1b);

	// All other meshes, Add/DelMesh in SetS4b wasn't working...

	//
	// All SIVB meshes are aligned to have the same origin. So we use the same offset
	// for both.
	//
	mesh_dir = _V(0, 0, 0);
	meshSivbSaturnVLow = AddMesh(hsat5stg3low, &mesh_dir);
	meshSivbSaturnV = AddMesh(hsat5stg3, &mesh_dir);
	meshSivbSaturn1bLow = AddMesh(hSat1stg2low, &mesh_dir);
	meshSivbSaturn1b = AddMesh(hSat1stg2, &mesh_dir);

	mesh_dir = _V(0, 0, 9.8);
	meshLMPKD = AddMesh(hLMPKD, &mesh_dir);

	mesh_dir = _V(0, 0, 9.8);
	meshLM_1 = AddMesh(hlm_1, &mesh_dir);

	mesh_dir = _V(0, 0, 9.6);	
	meshLTA_2r = AddMesh(hlta_2r, &mesh_dir);

	mesh_dir = _V(0.0, 0, 8.8);
	meshApollo8LTA = AddMesh(hapollo8lta, &mesh_dir);

	// ShiftMesh in SetS4b wasn't working...
	mesh_dir = _V(0, -0.15, 7.8);
	meshASTP_A = AddMesh(hastp, &mesh_dir);

	mesh_dir = _V(0, 0, 8.6);
	meshASTP_B = AddMesh(hastp, &mesh_dir);

	// ShiftMesh in SetS4b wasn't working...
	mesh_dir = _V(-1.04, 1.04, 9.1);
	meshCOASTarget_A = AddMesh(hCOAStarget, &mesh_dir);

	mesh_dir = _V(-1.3, 0, 9.6);
	meshCOASTarget_B = AddMesh(hCOAStarget, &mesh_dir);

	mesh_dir = _V(0, 0, 8.6);
	meshCOASTarget_C = AddMesh(hCOAStarget, &mesh_dir);
}

void SIVB::clbkDockEvent(int dock, OBJHANDLE connected)

{
}

void SIVB::SetState(SIVBSettings &state)

{
	if (state.SettingsType.SIVB_SETTINGS_PAYLOAD)
	{
		PayloadType = state.Payload;

		if (PayloadType == PAYLOAD_DOCKING_ADAPTER)
		{
			J2IsActive = true;
		}
	}

	if (state.SettingsType.SIVB_SETTINGS_PAYLOAD_INFO)
	{
		if (state.PayloadName[0])
		{
			strcpy(PayloadName, state.PayloadName);
		}

		LMDescentFuelMassKg = state.LMDescentFuelMassKg;
		LMAscentFuelMassKg = state.LMAscentFuelMassKg;

		//
		// Copy LM PAD data across.
		//
		LMPadCount = state.LMPadCount;

		if (LMPadCount > 0) {
			int i;
			LMPad = new unsigned int[LMPadCount * 2];
			for (i = 0; i < (LMPadCount * 2); i++)
			{
				LMPad[i] = state.LMPad[i];
			}
		}
	}

	if (state.SettingsType.SIVB_SETTINGS_GENERAL)
	{
		MissionTime = state.MissionTime;
		SaturnVStage = state.SaturnVStage;
		PanelsHinged = state.PanelsHinged;
		VehicleNo = state.VehicleNo;
		Realism = state.Realism;
		LowRes = state.LowRes;

		//
		// Limit rotation angle to 0-150 degrees.
		//
		double RotationAngle = min(150.0, state.SLARotationLimit);
		RotationLimit = max(0.0, (RotationAngle / 180.0));

		//
		// \todo For now, only allow 45 degrees if it's not hinged. The panel release code needs to be updated for other angles.
		//
		if (!PanelsHinged)
		{
			RotationLimit = 0.25;
		}
	}

	if (state.SettingsType.SIVB_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
		PayloadMass = state.PayloadMass;
	}

	if (state.SettingsType.SIVB_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType.SIVB_SETTINGS_ENGINES)
	{
		THRUST_THIRD_VAC = state.THRUST_VAC;
		ISP_THIRD_VAC = state.ISP_VAC;
	}

	State = SIVB_STATE_WAITING;
	SetS4b();
}

void SIVB::EnableDisableJ2(bool Enable)

{
	if (Enable || FuelVenting)
	{
		SetThrusterResource(th_main[0], ph_main);
	}
	else
	{
		SetThrusterResource(th_main[0], NULL);
	}
}

bool SIVB::IsVenting()

{
	return FuelVenting;
}

void SIVB::SetJ2ThrustLevel(double thrust)

{
	if (th_main[0])
		SetThrusterLevel(th_main[0], thrust);
}

void SIVB::SetAPSThrustLevel(double thrust)

{
	if (thg_aps)
		SetThrusterGroupLevel(thg_aps, thrust);
}

double SIVB::GetJ2ThrustLevel()

{
	if (th_main[0])
		return GetThrusterLevel(th_main[0]);

	return 0.0;
}

double SIVB::GetMissionTime()

{
	return MissionTime;
}

double SIVB::GetSIVbPropellantMass()

{
	return GetPropellantMass(ph_main);
}

double SIVB::GetTotalMass()

{
	double mass = GetMass();

	hDock = GetDockHandle(0);

	if (hDock)
	{
		OBJHANDLE hVessel = GetDockStatus(hDock);
		if (hVessel)
		{
			VESSEL *v = oapiGetVesselInterface(hVessel);
			mass += v->GetMass();
		}
	}

	return mass;
}

bool SIVB::PayloadIsDetachable()

{
	switch (PayloadType)
	{
	case PAYLOAD_LEM:
	case PAYLOAD_ASTP:
	case PAYLOAD_LM1:
		return true;

	default:
		return false;
	}
}

void SIVB::StartSeparationPyros()

{
	//
	// Start separation. For now this function will probably do all the work.
	//

	if (!PayloadIsDetachable())
	{
		return;
	}

	//
	// Do stuff to create a new payload vessel and dock it with the CSM.
	//

	//
	// Try to get payload settings, or return if we fail.
	//
	PayloadSettings ps;

	if (!csmCommandConnector.GetPayloadSettings(ps))
	{
		return;
	}

	//
	// Now separate the payload.
	//

	OBJHANDLE hCSM = GetDockStatus(GetDockHandle(0));

	if (!hCSM)
	{
		return;
	}

	VESSEL *CSMvessel = static_cast<VESSEL *> (oapiGetVesselInterface(hCSM));

	if (!CSMvessel)
	{
		return;
	}

	char *plName = 0;

	//
	// Get the payload config file name.
	//

	switch (PayloadType)
	{
	case PAYLOAD_LEM:
	case PAYLOAD_LM1:
		plName = "ProjectApollo/LEM";
		break;

	case PAYLOAD_ASTP:
		plName = "ProjectApollo/ASTP";
		break;

	default:
		return;
	}

	//
	// Now we know we have a valid payload. Create it and
	// dock it.
	//

	Payload *payloadvessel;
	VESSELSTATUS2 vslm2;
	VESSELSTATUS2::DOCKINFOSPEC dckinfo;

	//
	// Now Lets create a real LEM and dock it
	//

	//
	// The CSM docking probe has to ignore both the undock and dock
	// events. Otherwise it will prevent us from 'docking' to the
	// newly created LEM.
	//
	csmCommandConnector.SetIgnoreNextDockEvents(2);

	CSMvessel->Undock(0);

	vslm2.version = 2;
	vslm2.flag = 0;
	vslm2.fuel = 0;
	vslm2.thruster = 0;
	vslm2.ndockinfo = 1;
	vslm2.dockinfo = &dckinfo;

	CSMvessel->GetStatusEx(&vslm2);

	vslm2.dockinfo[0].idx = 0;
	vslm2.dockinfo[0].ridx = 0;
	vslm2.dockinfo[0].rvessel = hCSM;
	vslm2.ndockinfo = 1;
	vslm2.flag = VS_DOCKINFOLIST;
	vslm2.version = 2;

	OBJHANDLE hPayload = oapiCreateVesselEx(PayloadName, plName, &vslm2);

	//
	// Initialise the state of the LEM AGC information.
	//

	payloadvessel = static_cast<Payload *> (oapiGetVesselInterface(hPayload));
	payloadvessel->SetupPayload(ps);
	Payloaddatatransfer = true;

	CSMvessel->GetStatusEx(&vslm2);

	vslm2.dockinfo = &dckinfo;
	vslm2.dockinfo[0].idx = 0;
	vslm2.dockinfo[0].ridx = 0;
	vslm2.dockinfo[0].rvessel = hPayload;
	vslm2.ndockinfo = 1;
	vslm2.flag = VS_DOCKINFOLIST;
	vslm2.version = 2;

	CSMvessel->DefSetStateEx(&vslm2);

	//
	// Finally, do any special case configuration.
	//

	switch (PayloadType)
	{
	case PAYLOAD_LEM:
	case PAYLOAD_LM1:
		{
			//
			// PAD load.
			//

			LEM *lmvessel = static_cast<LEM *> (payloadvessel);

			if (LMPad && LMPadCount > 0)
			{
				int i;
				for (i = 0; i < LMPadCount; i++) {
					lmvessel->PadLoad(LMPad[i * 2], LMPad[i * 2 + 1]);
				}
			}

		}
		break;

	default:
		break;
	}

	//
	// Set the payload to none.
	//

	PayloadType = PAYLOAD_EMPTY;

	SetS4b();
}

void SIVB::StopSeparationPyros()

{
	//
	// Stop separation. For now this will probably do nothing.
	//
}

void SIVB::HideAllMeshes()

{
	SetMeshVisibilityMode(panelMesh1SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh1SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh1Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturn1bLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshLMPKD, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshLTA_2r, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshApollo8LTA, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshASTP_A, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshASTP_B, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshCOASTarget_A, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshCOASTarget_B, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshCOASTarget_C, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshLM_1, MESHVIS_NEVER);
}


static int refcount = 0;

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	VESSEL *v;

	if (!refcount++)
	{
		SIVbLoadMeshes();
	}

	v = new SIVB (hvessel, flightmodel);
	return v;
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	--refcount;

	if (!refcount)
	{

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) 
		delete static_cast<SIVB *> (vessel);
}

SIVbConnector::SIVbConnector()

{
	OurVessel = 0;
}

SIVbConnector::~SIVbConnector()

{
}

SIVbToIUCommandConnector::SIVbToIUCommandConnector()

{
	type = LV_IU_COMMAND;
}

SIVbToIUCommandConnector::~SIVbToIUCommandConnector()

{
}

bool SIVbToIUCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IULVMessageType messageType;

	messageType = (IULVMessageType) m.messageType;

	switch (messageType)
	{
	case IULV_GET_J2_THRUST_LEVEL:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetJ2ThrustLevel();
			return true;
		}
		break;

	case IULV_GET_STAGE:
		m.val1.iValue = STAGE_ORBIT_SIVB;
		return true;

	case IULV_GET_ALTITUDE:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetAltitude();
			return true;
		}
		break;

	case IULV_GET_PROPELLANT_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSIVbPropellantMass();
			return true;
		}
		break;

	case IULV_GET_STATUS:
		if (OurVessel)
		{
			VESSELSTATUS *status = static_cast<VESSELSTATUS *> (m.val1.pValue);
			VESSELSTATUS stat;

			OurVessel->GetStatus(stat);

			*status = stat;
			return true;
		}
		break;

	case IULV_GET_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetTotalMass();
			return true;
		}
		break;

	case IULV_GET_GRAVITY_REF:
		if (OurVessel)
		{
			m.val1.hValue = OurVessel->GetGravityRef();
			return true;
		}
		break;

	case IULV_GET_AP_DIST:
		if (OurVessel)
		{
			OurVessel->GetApDist(m.val1.dValue);
			return true;
		}
		break;

	case IULV_GET_MAX_FUEL_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetMaxFuelMass();
			return true;
		}
		break;

	case IULV_GET_RELATIVE_POS:
		if (OurVessel)
		{
			VECTOR3 pos;
			VECTOR3 *v = (VECTOR3 *) m.val2.pValue;

			OurVessel->GetRelativePos(m.val1.hValue, pos);

			v->data[0] = pos.data[0];
			v->data[1] = pos.data[1];
			v->data[2] = pos.data[2];

			return true;
		}
		break;

	case IULV_GET_RELATIVE_VEL:
		if (OurVessel)
		{
			VECTOR3 vel;
			VECTOR3 *v = (VECTOR3 *) m.val2.pValue;

			OurVessel->GetRelativeVel(m.val1.hValue, vel);

			v->data[0] = vel.data[0];
			v->data[1] = vel.data[1];
			v->data[2] = vel.data[2];

			return true;
		}
		break;

	case IULV_GET_GLOBAL_VEL:
		if (OurVessel)
		{
			OurVessel->GetGlobalVel(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_ELEMENTS:
		if (OurVessel)
		{
			ELEMENTS el;
			ELEMENTS *e = (ELEMENTS *) m.val1.pValue;

			m.val3.hValue = OurVessel->GetElements(el, m.val2.dValue);

			*e = el;

			return true;
		}
		break;

	case IULV_GET_PMI:
		if (OurVessel)
		{
			OurVessel->GetPMI(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_SIZE:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSize();
			return true;
		}
		break;

	case IULV_GET_MAXTHRUST:
		if (OurVessel)
		{
			m.val2.dValue = OurVessel->GetMaxThrust((ENGINETYPE) m.val1.iValue);
			return true;
		}
		break;

	case IULV_LOCAL2GLOBAL:
		if (OurVessel)
		{
			OurVessel->Local2Global(*(VECTOR3 *) m.val1.pValue, *(VECTOR3 *) m.val2.pValue);
			return true;
		}
		break;

	case IULV_GET_WEIGHTVECTOR:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->GetWeightVector(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_FORCEVECTOR:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->GetForceVector(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_ROTATIONMATRIX:
		if (OurVessel)
		{
			OurVessel->GetRotationMatrix(*(MATRIX3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_PITCH:
		if (OurVessel)
		{	
			m.val1.dValue = OurVessel->GetPitch();
			return true;
		}
		break;
			
	case IULV_GET_BANK:
		if (OurVessel)
		{	
			m.val1.dValue = OurVessel->GetBank();
			return true;
		}
		break;
			
	case IULV_GET_SLIP_ANGLE:
		if (OurVessel)
		{	
			m.val1.dValue = OurVessel->GetSlipAngle();
			return true;
		}
		break;


	case IULV_ACTIVATE_NAVMODE:
		if (OurVessel)
		{
			OurVessel->ActivateNavmode(m.val1.iValue);
			return true;
		}
		break;

	case IULV_DEACTIVATE_NAVMODE:
		if (OurVessel)
		{
			OurVessel->DeactivateNavmode(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SET_J2_THRUST_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetJ2ThrustLevel(m.val1.dValue);
			return true;
		}
		break;

	case IULV_SET_APS_THRUST_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetAPSThrustLevel(m.val1.dValue);
			return true;
		}
		break;

	case IULV_SET_ATTITUDE_LIN_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetAttitudeLinLevel(m.val1.iValue, m.val2.iValue);
			return true;
		}
		break;

	case IULV_SET_ATTITUDE_ROT_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetAttitudeRotLevel(m.val1.vValue);
			return true;
		}
		break;

	case IULV_ENABLE_J2:
		if (OurVessel)
		{
			OurVessel->EnableDisableJ2(m.val1.bValue);
			return true;
		}
		break;

	case IULV_J2_DONE:
		if (OurVessel)
		{
			OurVessel->SetVentingThruster();
		}
		break;

	//
	// The RCS is always enabled, so don't bother turning it on and off.
	//

	case IULV_DEACTIVATE_S4RCS:
	case IULV_ACTIVATE_S4RCS:
		return true;
	}

	return false;
}

CSMToSIVBCommandConnector::CSMToSIVBCommandConnector()

{
	type = CSM_SIVB_COMMAND;
}

CSMToSIVBCommandConnector::~CSMToSIVBCommandConnector()

{
}

void CSMToSIVBCommandConnector::SetIgnoreNextDockEvent()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SIVBCSM_IGNORE_DOCK_EVENT;

	SendMessage(cm);
}

void CSMToSIVBCommandConnector::SetIgnoreNextDockEvents(int n)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SIVBCSM_IGNORE_DOCK_EVENTS;
	cm.val1.iValue = n;

	SendMessage(cm);
}

bool CSMToSIVBCommandConnector::GetPayloadSettings(PayloadSettings &p)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SIVBCSM_GET_PAYLOAD_SETTINGS;
	cm.val1.pValue = &p;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool CSMToSIVBCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	CSMSIVBMessageType messageType;

	messageType = (CSMSIVBMessageType) m.messageType;

	switch (messageType)
	{
	case CSMSIVB_IS_VENTING:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->IsVenting();
			return true;
		}
		break;

	case CSMSIVB_IS_VENTABLE:
		if (OurVessel)
		{
			m.val1.bValue = true;
			return true;
		}
		break;

	case CSMSIVB_START_VENTING:
		if (OurVessel)
		{
			OurVessel->StartVenting();
			return true;
		}
		break;

	case CSMSIVB_STOP_VENTING:
		if (OurVessel)
		{
			OurVessel->StopVenting();
			return true;
		}
		break;

	case CSMSIVB_START_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StartSeparationPyros();
			return true;
		}
		break;

	case CSMSIVB_STOP_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StopSeparationPyros();
			return true;
		}
		break;

	case CSMSIVB_GET_VESSEL_FUEL:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSIVbPropellantMass();
			return true;
		}
		break;

	case CSMSIVB_GET_MAIN_BATTERY_POWER:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetMainBatteryPower();
			m.val2.dValue = OurVessel->GetMainBatteryPowerDrain();
			return true;
		}
		break;

	case CSMSIVB_GET_MAIN_BATTERY_ELECTRICS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetMainBatteryVoltage();
			m.val2.dValue = OurVessel->GetMainBatteryCurrent();
			return true;
		}
		break;
	}

	return false;
}

