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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.29  2006/01/11 02:16:25  movieman523
  *	Added RCS propellant quantity gauge.
  *	
  *	Revision 1.28  2006/01/10 23:45:35  movieman523
  *	Revised RCS ISP and thrust to historical values.
  *	
  *	Revision 1.27  2006/01/10 23:20:51  movieman523
  *	SM RCS is now enabled per quad.
  *	
  *	Revision 1.26  2006/01/08 17:11:41  movieman523
  *	Added seperation particles to SII/SIVb sep.
  *	
  *	Revision 1.25  2006/01/08 04:00:24  movieman523
  *	Added first two engineering cameras.
  *	
  *	Revision 1.24  2006/01/05 12:02:26  tschachim
  *	Fixed SIVB separation offset (hopefully)
  *	
  *	Revision 1.23  2006/01/04 23:06:03  movieman523
  *	Moved meshes into ProjectApollo directory and renamed a few.
  *	
  *	Revision 1.22  2006/01/04 19:51:54  movieman523
  *	Updated config file names.
  *	
  *	Revision 1.21  2005/12/28 16:19:10  movieman523
  *	Should now be getting all config files from ProjectApollo directory.
  *	
  *	Revision 1.20  2005/11/24 01:07:54  movieman523
  *	Removed code for panel lights which were being set incorrectly. Plus a bit of tidying.
  *	
  *	Revision 1.19  2005/11/21 23:08:15  movieman523
  *	Moved more mesh files into the ProjectApollo directory.
  *	
  *	Revision 1.18  2005/10/19 11:41:43  tschachim
  *	Improved logging.
  *	
  *	Revision 1.17  2005/10/11 16:42:01  tschachim
  *	Renamed LPswitch5.
  *	
  *	Revision 1.16  2005/08/24 00:30:00  movieman523
  *	Revised CM RCS code, and removed a load of switches that aren't used anymore.
  *	
  *	Revision 1.15  2005/08/21 22:21:00  movieman523
  *	Fixed SM RCS and activated SIVB RCS at all times for now.
  *	
  *	Revision 1.14  2005/08/20 11:14:52  movieman523
  *	Added Rot Contr Pwr switches and removed a number of old switches which aren't used anymore.
  *	
  *	Revision 1.13  2005/08/15 19:25:03  movieman523
  *	Added CSM attitude control switches and removed old ones.
  *	
  *	Revision 1.12  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.11  2005/08/01 19:07:47  movieman523
  *	Genericised code to deal with SM destruction on re-entry, and did some tidying up of Saturn 1b code.
  *	
  *	Revision 1.10  2005/07/31 01:43:13  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.9  2005/06/06 12:32:08  tschachim
  *	New switch
  *	
  *	Revision 1.8  2005/03/16 17:30:41  yogenfrutz
  *	corrected missing crew in csm stage
  *	
  *	Revision 1.7  2005/03/12 20:51:30  chode99
  *	Reentry airfoil is now deleted when first drogue opens.
  *	By not deleting, the aerodynamics of the chutes were ignored.
  *	Also tweaked the drag of the chutes a bit to match the real velocities.
  *	
  *	Revision 1.6  2005/03/09 05:05:00  chode99
  *	Fixed CSM thruster positions in SetCSM2Stage
  *	
  *	Revision 1.5  2005/03/03 17:58:43  tschachim
  *	panel handling for generic cockpit
  *	
  *	Revision 1.4  2005/02/20 20:20:45  chode99
  *	Changed touchdown points for recovery stage so it is also "above water".
  *	
  *	Revision 1.3  2005/02/20 05:24:58  chode99
  *	Changes to implement realistic CM aerodynamics. Created callback function "CoeffFunc" in Saturn1b.cpp and Saturn5.cpp. Substituted CreateAirfoil for older lift functions.
  *	
  *	Revision 1.2  2005/02/19 19:32:55  chode99
  *	Adjusted touchdown points in splashdown stage so it is no longer "underwater".
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"
#include "tracer.h"

MESHHANDLE hSM;
MESHHANDLE hSMhga;
MESHHANDLE hCM;
MESHHANDLE hCM2;
MESHHANDLE hCMP;
MESHHANDLE hCREW;
MESHHANDLE hFHO;
MESHHANDLE hFHC;
MESHHANDLE hCM2B;
MESHHANDLE hprobe;
MESHHANDLE hCMBALLOON;
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

extern void CoeffFunc(double aoa, double M, double Re ,double *cl ,double *cm  ,double *cd);
//extern double LiftCoeff (double aoa);

#define LOAD_MESH(var, name) var = oapiLoadMeshGlobal(name);

void SaturnInitMeshes()

{
	LOAD_MESH(hSM, "ProjectApollo/SM");
	LOAD_MESH(hSMhga, "ProjectApollo/SM_HGA");
	LOAD_MESH(hCM, "ProjectApollo/sat5CM");
	LOAD_MESH(hCM2, "ProjectApollo/sat5CM2");
	LOAD_MESH(hCMP, "ProjectApollo/SAT5CMP");
	LOAD_MESH(hCREW, "ProjectApollo/SAT5CREW");
	LOAD_MESH(hFHC, "ProjectApollo/SAT5HC");
	LOAD_MESH(hFHO, "ProjectApollo/SAT5HO");
	LOAD_MESH(hCM2B, "ProjectApollo/SAT5CM2B");
	LOAD_MESH(hprobe, "ProjectApollo/sat5probe");
	LOAD_MESH(hCMBALLOON, "ProjectApollo/CM_Balloons");
	LOAD_MESH(hCRB, "ProjectApollo/nSATURN1_CRB");
	LOAD_MESH(hCMB, "ProjectApollo/SAT5CMB");
	LOAD_MESH(hChute30, "ProjectApollo/Apollo_2chute");
	LOAD_MESH(hChute31, "ProjectApollo/Apollo_3chuteEX");
	LOAD_MESH(hChute32, "ProjectApollo/Apollo_3chuteHD");
	LOAD_MESH(hApollochute, "ProjectApollo/Apollo_3chute");
	LOAD_MESH(hFHC2, "ProjectApollo/SAT5HC2");
	LOAD_MESH(hsat5tower, "ProjectApollo/BoostCover");
	LOAD_MESH(hFHO2, "ProjectApollo/SAT5HO2");
	LOAD_MESH(hCMPEVA, "ProjectApollo/nSATURN1_CMP_EVA");
}

void Saturn::ToggelHatch()

{
	ClearMeshes();

	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
	meshidx = AddMesh (hSM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	//
	// Skylab SM and Apollo 7 have no HGA.
	//
	if (!NoHGA) {
		mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
		AddMesh (hSMhga, &mesh_dir);
	}

	mesh_dir=_V(0,0,34.4-12.25-21.5);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	//Don't Forget the Hatch
	if (HatchOpen){
		mesh_dir =_V(0.02,1.35,34.54-12.25-21.5);
		meshidx = AddMesh(hFHC, &mesh_dir);
		HatchOpen = false;
	}
	else{
		mesh_dir =_V(-0.7,1.75,34.85-12.25-21.5);
		meshidx = AddMesh(hFHO, &mesh_dir);
		HatchOpen = true;
	}

	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (dockstate <=4 ){
		mesh_dir =_V(0,0,35.90-12.25-21.5);
		probeidx = AddMesh (hprobe, &mesh_dir);
	}
}

void Saturn::ToggelHatch2()

{
	ClearMeshes();
	UINT meshidx ;
	VECTOR3 mesh_dir=_V(0,0,-1.2);
	if (Burned){
		meshidx = AddMesh (hCM2B, &mesh_dir);
	}else{
		meshidx = AddMesh (hCM2, &mesh_dir);
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,-0.1,-0.2);
	meshidx=AddMesh (hCMBALLOON, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir=_V(0,0.15,-1.35);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,-1.35);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	//Don't Forget the Hatch
	if (HatchOpen){
		mesh_dir=_V(0.02,1.35,-1.06);
		meshidx = AddMesh (hFHC, &mesh_dir);
		HatchOpen= false;
	}else{
		mesh_dir=_V(-0.7,1.75,0.45-1.2);
		meshidx = AddMesh (hFHO, &mesh_dir);
		HatchOpen= true;
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
}

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
			mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
			AddMesh (hSMhga, &mesh_dir);
		}

		mesh_dir=_V(0,0,34.4-12.25-21.5);
		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}

		//Don't Forget the Hatch
		mesh_dir=_V(-0.7,1.75,34.85-12.25-21.5);
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen = true;
		if (dockstate <=4 ){
			mesh_dir=_V(0,0,35.90-12.25-21.5);
			probeidx = AddMesh (hprobe, &mesh_dir);
		}
	}
	else{
		EVA_IP = true;

		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
		AddMesh (hSM, &mesh_dir);
		mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
		AddMesh (hSMhga, &mesh_dir);

		mesh_dir=_V(0,0,34.4-12.25-21.5);
		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}

		//Don't Forget the Hatch
		mesh_dir =_V(-0.7,1.75,34.85-12.25-21.5);
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen= true;

		if (dockstate <=4 ){
			mesh_dir=_V(0,0,35.90-12.25-21.5);
			probeidx=AddMesh (hprobe, &mesh_dir);
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
			mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
			AddMesh (hSMhga, &mesh_dir);
		}

		mesh_dir=_V(0,0,34.4-12.25-21.5);
		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}

		//Don't Forget the Hatch
		mesh_dir=_V(-0.7,1.75,34.85-12.25-21.5);
		meshidx = AddMesh (hFHO, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		HatchOpen = true;

		if (dockstate <=4 ){
			mesh_dir =_V(0,0,35.90-12.25-21.5);
			probeidx = AddMesh (hprobe, &mesh_dir);
		}
	}
}

void Saturn::SetRecovery()

{
	ClearMeshes();
	UINT meshidx;

	VECTOR3 mesh_dir=_V(0,0,-1.2);
	if (Burned){
		meshidx = AddMesh (hCM2B, &mesh_dir);
	}
	else{
		meshidx = AddMesh (hCM2, &mesh_dir);
	}
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(-0.7,1.75,0.45-1.2);
	meshidx = AddMesh (hFHO, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	HatchOpen = true;
	mesh_dir =_V(0,-0.1,-0.2);
	meshidx=AddMesh (hCMBALLOON, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (Crewed) {
		mesh_dir =_V(2.7,1.8,-1.5);
		meshidx = AddMesh (hCRB, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	SetTouchdownPoints (_V(0,-1.0,-2.0), _V(-.7,.7,-2.0), _V(.7,.7,-2.0));
	SetView(-1.35);
}

void Saturn::SetCSMStage ()
{
	ClearMeshes();
    ClearThrusterDefinitions();

	if(ph_3rd) {
		DelPropellantResource(ph_3rd);
		ph_3rd = 0;
	}

	if (ph_sep) {
		DelPropellantResource(ph_sep);
		ph_sep = 0;
	}

	SetSize (20);
	SetCOG_elev (3.5);
	SetEmptyMass (CM_Mass + SM_EmptyMass);
	// ************************* propellant specs **********************************

	if (!ph_sps) {
		ph_sps  = CreatePropellantResource(SM_FuelMass, SM_FuelMass); //SPS stage Propellant
	}

	SetDefaultPropellantResource (ph_sps); // display SPS stage propellant level in generic HUD

	// *********************** thruster definitions ********************************


	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG2O};
	// orbiter main thrusters
	th_main[0] = CreateThruster (_V( 0,0,-6.5), _V( 0,0,1),100552.5 , ph_sps, 3778.5);
	DelThrusterGroup(THGROUP_MAIN,true);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	AddExhaust (th_main[0], 20.0, 2.25, SMExhaustTex);
	SetPMI (_V(12,12,7));
	SetCrossSections (_V(40,40,14));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.3));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	//ShiftCentreOfMass (_V(0,0,21.5));
	if (FIRSTCSM){
		//sprintf(oapiDebugString(), "shift %f", gaz);
		//ShiftCentreOfMass (_V(0,0,21.5));
		FIRSTCSM=false;
	}
	else if (bManualUnDock){
		dockstate = 4;
	}
	VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
	AddMesh (hSM, &mesh_dir);

	//
	// Skylab SM and Apollo 7 have no HGA.
	//
	if (!NoHGA) {
		mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
		AddMesh (hSMhga, &mesh_dir);
	}

	mesh_dir=_V(0,0,34.4-12.25-21.5);

	UINT meshidx;
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	//Don't Forget the Hatch
	mesh_dir=_V(0.02,1.35,34.54-12.25-21.5);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0,0,35.90-12.25-21.5);
	probeidx=AddMesh (hprobe, &mesh_dir);

	VECTOR3 dockpos = {0,0,35.90-12.25-21.5};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams(dockpos, dockdir, dockrot);

	AddRCSJets(-1.80, SM_RCS_THRUST);

	SetView(0.4);
	// **************************** NAV radios *************************************

	InitNavRadios (4);
	SetEnableFocus(true);
	EnableTransponder (true);

	OrbiterAttitudeToggle.SetActive(true);

	ThrustAdjust = 1.0;
	ActivateASTP = false;
}

void Saturn::SetCSM2Stage ()
{	ClearMeshes();
	DelThrusterGroup(THGROUP_MAIN,true);
    ClearThrusterDefinitions();

	if(ph_3rd) {
		DelPropellantResource(ph_3rd);
		ph_3rd = 0;
	}

	if (ph_sep) {
		DelPropellantResource(ph_sep);
		ph_sep = 0;
	}

	SetSize (7);
	SetCOG_elev (3.5);
	SetEmptyMass (CM_Mass + SM_EmptyMass);
	// ************************* propellant specs **********************************

	if (!ph_sps) {
		ph_sps  = CreatePropellantResource(SM_FuelMass, SM_FuelMass); //SPS stage Propellant
	}

	SetDefaultPropellantResource (ph_sps); // display SPS stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG2O};
	// orbiter main thrusters
	th_main[0] = CreateThruster (_V( 0,0,-6.5), _V( 0,0,1),100552.5 , ph_sps, 3778.5);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	AddExhaust (th_main[0], 20.0, 2.25, SMExhaustTex);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(12,12,7));
	SetCrossSections (_V(40,40,14));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.3));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);

	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,SMVO,30.25-12.25-21.5);
	AddMesh (hSM, &mesh_dir);

	//
	// Skylab SM and Apollo 7 have no HGA.
	//
	if (!NoHGA) {
		mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
		AddMesh (hSMhga, &mesh_dir);
	}

	mesh_dir=_V(0,0,34.4-12.25-21.5);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	//Don't Forget the Hatch
	mesh_dir=_V(0.02,1.35,34.54-12.25-21.5);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,34.25-12.25-21.5);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	SetEngineLevel(ENGINE_MAIN, 0.0);

	AddRCSJets(-1.80, SM_RCS_THRUST);

	SetView(0.4);

	EnableTransponder (true);

	// **************************** NAV radios *************************************

	InitNavRadios (4);
	probeidx=0;

	SetEnableFocus(true);

	OrbiterAttitudeToggle.SetActive(true);

	ThrustAdjust = 1.0;
	ActivateASTP = false;
}

void Saturn::SetReentryStage ()

{
	ClearMeshes();
    ClearThrusterDefinitions();
	SetSize (6.0);
	SetCOG_elev (2.0);
	SetEmptyMass (5500);
	SetPMI (_V(12,12,7));
	//SetPMI (_V(1.5,1.35,1.35));
	SetCrossSections (_V(9.17,7.13,7.0));
	SetCW (5.5, 0.1, 3.4, 3.4);
	SetRotDrag (_V(0.07,0.07,0.003));
	if (GetFlightModel() >= 1)
	{
		CreateAirfoil(LIFT_VERTICAL, _V(0.0,0.16,1.12), CoeffFunc, 3.5 ,11.95, 1.0);
    }
  	ShiftCentreOfMass (_V(0,0,0.5));

	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,0,0);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir = _V(0.02,1.35,0.14);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,-.15);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir = _V(0,0.15,-.15);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	SetView(-0.15);

	if (ph_sps) DelPropellantResource(ph_sps);
	if (ph_rcs0) DelPropellantResource(ph_rcs0);
	if (ph_rcs1) DelPropellantResource(ph_rcs1);
	if (ph_rcs2) DelPropellantResource(ph_rcs2);
	if (ph_rcs3) DelPropellantResource(ph_rcs3);

	DelThrusterGroup(THGROUP_MAIN,true);

	if (CMTex) SetReentryTexture(CMTex,1e6,5,0.7);

	AddRCS_CM(CM_RCS_THRUST);

	VECTOR3 dockpos = {0,0,1.5};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};

	SetDockParams(dockpos, dockdir, dockrot);

	stgSM = true;
}

void Saturn::StageSeven(double simt)

{
	if (CsmLmFinalSep1Switch.GetState() || CsmLmFinalSep2Switch.GetState()) {
		Undock(0);
		dockingprobe.SetEnabled(false);
	}

	if (!Crewed) {
		switch (StageState) {
		case 0:
			if (GetAltitude() < 145000) {
				SlowIfDesired();
				ActivateCMRCS();
				ActivateNavmode(NAVMODE_RETROGRADE);
				StageState++;
			}
			break;
		}
	}

	if (GetAtmPressure() > 300) { // We 're looking wether the CM has burned or not
		ClearMeshes();

		UINT meshidx;
		VECTOR3 mesh_dir=_V(0,0,0);
		meshidx = AddMesh (hCMB, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,0.14);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,-.15);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,-.15);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}

		AddRCS_CM(CM_RCS_THRUST);

		SetStage(CM_ENTRY_STAGE);
		Burned = true;

		SetView(-0.15);
	}
}

void Saturn::StageEight(double simt)

{
	UINT meshidx;
	VECTOR3 mesh_dir=_V(0,0,-1);

	if (Burned) {
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5);
		meshidx = AddMesh (hCM2B, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}
	else {
		ClearMeshes();
		VECTOR3 mesh_dir=_V(0,0,34.40-12.25-21.5);
		meshidx = AddMesh (hCM2, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}

	AddRCS_CM(CM_RCS_THRUST);

	SetView(0.5);

	if (!Crewed) {
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

	LAUNCHIND[1] = true;
	SetStage(CM_ENTRY_STAGE_TWO);
}

void Saturn::SetChuteStage1()
{
	UINT meshidx;
	SetSize (15);
	SetCOG_elev (1);
	SetEmptyMass (5500);
	SetMaxFuelMass (100);
	SetFuelMass (0);
	ClearAirfoilDefinitions();
	SetMaxThrust (ENGINE_MAIN,  0);
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 0);
	SetEngineLevel(ENGINE_ATTITUDE,0);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,80.0));
	SetCW (1.0, 1.5, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 mesh_dir=_V(0,0,34.40-12.25-16.5-6.5-6.25);
	if (Burned){
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2B, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}
	else {
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir =_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}

	AddRCS_CM(CM_RCS_THRUST);

	mesh_dir=_V(0,0,44.00-12.25-21.5-7.75);
	meshidx = AddMesh (hChute30, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);

	SetView(-7.25);
	DeactivateNavmode(NAVMODE_KILLROT);
	SetTouchdownPoints (_V(0,-1.0,0), _V(-.7,.7,0), _V(.7,.7,0));
	LAUNCHIND[3] = true;
	LAUNCHIND[1] = true;
}

void Saturn::SetChuteStage2()
{
	UINT meshidx;

	SetCOG_elev (1);
	SetEmptyMass (5500);
	SetMaxFuelMass (100);
	SetFuelMass (0);
	SetMaxThrust (ENGINE_MAIN,  0);
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 0);
	SetEngineLevel(ENGINE_ATTITUDE,0);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,140.0));
	SetCW (1.0, 1.5, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//ShiftCentreOfMass (_V(0,0,6.25));

	VECTOR3 mesh_dir=_V(0,0,34.40-12.25-16.5-6.5-6.25);

	if (Burned) {
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2B, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}
	else {
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}

	AddRCS_CM(CM_RCS_THRUST);

	mesh_dir=_V(0,-0.25,39.7-12.25-21.5-7.75);
	meshidx=AddMesh (hChute31, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);

	SetView(-7.25);
	SetTouchdownPoints (_V(0,-1.0,0), _V(-.7,.7,0), _V(.7,.7,0));
	LAUNCHIND[3] = true;
	LAUNCHIND[1] = true;
}

void Saturn::SetChuteStage3()
{
	UINT meshidx;
	SetSize (12);
	SetCOG_elev (1);
	SetEmptyMass (5500);
	SetMaxFuelMass (100);
	SetFuelMass (0);
	SetMaxThrust (ENGINE_MAIN,  0);
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 0);
	SetEngineLevel(ENGINE_ATTITUDE,0);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,480.0));
	SetCW (0.7, 1.5, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//ShiftCentreOfMass (_V(0,0,6.25));
	VECTOR3 mesh_dir=_V(0,0,34.40-12.25-16.5-6.5-6.25);
	if (Burned){
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2B, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}
	else {
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}

	mesh_dir=_V(0,-1.3,17);
	meshidx = AddMesh (hChute32, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);

	AddRCS_CM(CM_RCS_THRUST);

	SetView(-7.25);
	SetTouchdownPoints (_V(0,-1.0,0), _V(-.7,.7,0), _V(.7,.7,0));
	LAUNCHIND[3] = true;
	LAUNCHIND[1] = true;
}

void Saturn::SetChuteStage4()
{
	UINT meshidx;
	SetSize (12);
	SetCOG_elev (1);
	SetEmptyMass (5500);
	SetMaxFuelMass (500);
	SetFuelMass (0);
	SetTouchdownPoints (_V(0,0.0,0), _V(-1,0,0), _V(1,0,0));
	SetPMI (_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,3280.0));
	SetCW (0.7, 1.5, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//ShiftCentreOfMass (_V(0,0,6.25));
	VECTOR3 mesh_dir=_V(0,0,34.40-12.25-16.5-6.5-6.25);
	if (Burned){
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2B, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}
	else{
		ClearMeshes();
		mesh_dir=_V(0,0,34.40-12.25-21.5-7.75);
		meshidx = AddMesh (hCM2, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-7.75);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		// And the Crew
		if (Crewed) {
			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCMP, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			mesh_dir=_V(0,0.15,34.25-12.25-21.5-7.75);
			meshidx = AddMesh (hCREW, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
		}
	}
	mesh_dir = OFS_MAINCHUTE;
	meshidx=AddMesh (hApollochute, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);

	AddRCS_CM(CM_RCS_THRUST);

	SetView(-7.25);
	SetTouchdownPoints (_V(0,-1.0,0), _V(-.7,.7,0), _V(.7,.7,0));
	LAUNCHIND[5]=true;
	LAUNCHIND[3]=true;
	LAUNCHIND[1]=true;
}

void Saturn::SetSplashStage()
{
	UINT meshidx;
	SetSize (4);
	SetCOG_elev (2);
	SetEmptyMass (5500);
	SetMaxFuelMass (0);
	SetFuelMass (0);
	SetMaxThrust (ENGINE_MAIN,  0);
	SetMaxThrust (ENGINE_ATTITUDE, 0);
	SetEngineLevel(ENGINE_ATTITUDE,0);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(20,20,12));
	SetCrossSections (_V(2.8,2.8,7.0));
	SetCW (0.5, 1.5, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetTouchdownPoints (_V(0,-1.0,-2.0), _V(-.7,.7,-2.0), _V(.7,.7,-2.0));
	if (GetFlightModel() >= 1)
	{
		SetPitchMomentScale (-5e-3);
		SetBankMomentScale (-5e-3);
	}
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//ShiftCentreOfMass (_V(0,0,6.25));

	VECTOR3 mesh_dir=_V(0,0,-1.2);
	if (Burned){
		meshidx = AddMesh (hCM2B, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,-1.06);
		meshidx = AddMesh (hFHC2, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}
	else {
		meshidx = AddMesh (hCM2, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0.02,1.35,-1.06);
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	mesh_dir=_V(0,-0.1,-0.2);
	meshidx=AddMesh (hCMBALLOON, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);

	if (Crewed) {
		mesh_dir=_V(0,0.15,-1.35);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,-1.35);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	AddRCS_CM(CM_RCS_THRUST);

	SetView(-1.35);
}

void Saturn::SetAbortStage ()
{

	ClearMeshes();
    ClearThrusterDefinitions();
	UINT meshidx;
	SetSize (8);
	SetCOG_elev (2.0);
	SetEmptyMass (6718);
	SetPMI (_V(12,12,7));
	//SetPMI (_V(1.5,1.35,1.35));
	SetCrossSections (_V(9.17,7.13,7.0));
	SetCW (5.5, 0.1, 3.4, 3.4);
	SetRotDrag (_V(0.07,0.07,0.003));
	if (GetFlightModel() >= 1)
	{
//		SetPitchMomentScale (-1e-5);
//		SetBankMomentScale (-1e-5);
//		SetLiftCoeffFunc (LiftCoeff); 
//		CreateAirfoil(LIFT_VERTICAL, _V(-0.014,0.107,0.75), CoeffFunc, 3.5 ,11.95, 1.0);
		CreateAirfoil(LIFT_VERTICAL, _V(0.0,0.16,1.12), CoeffFunc, 3.5 ,11.95, 1.0);
    }
	ShiftCentreOfMass (_V(0,0,1.5));
	VECTOR3 mesh_dir=_V(0,0,33.0-12.25-21.5-1.5+1);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	mesh_dir=_V(0,0,38.0-12.25-21.5-1.5+1);
	meshidx = AddMesh (hsat5tower, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	//Don't Forget the Hatch
	mesh_dir=_V(0.02,1.35,34.54-12.25-21.5-1.5+1);
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		mesh_dir=_V(0,0.15,34.25-12.25-21.5-1.5+1);
		meshidx = AddMesh (hCMP, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		mesh_dir=_V(0,0.15,34.25-12.25-21.5-1.5+1);
		meshidx = AddMesh (hCREW, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}

	//VECTOR3 bdir = _V(0,0,1);
	//ReEntryID = AddExhaustRef(EXHAUST_CUSTOM,_V(0,0,-0.5), 0, 0, &bdir);
	if (ph_3rd)  {
		DelPropellantResource(ph_3rd); //SPS stage Propellant
		ph_3rd = 0;
	}
	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}
	if(ph_2nd) {
		DelPropellantResource(ph_2nd);
		ph_2nd = 0;
	}

	if (ph_sps)
		DelPropellantResource(ph_sps); //SPS stage Propellant
		ph_sps = 0;
	if (!ph_sps)
		ph_sps  = CreatePropellantResource(2500); //SPS stage Propellant
	SetDefaultPropellantResource (ph_sps); // display SPS stage propellant level in generic HUD

	AddRCS_CM(CM_RCS_THRUST);

	// *********************** thruster definitions ********************************

	// orbiter main thrusters
	th_main[0] = CreateThruster (_V( 0,0,-6.5), _V( 0,0,1),721035 , ph_sps, 900);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	SetThrusterLevel(th_main[0], 1.0);

	VECTOR3 m_exhaust_pos1= {0.4,0.0,1.6};
    VECTOR3 m_exhaust_pos2= {-0.4,0.0,1.6};
	VECTOR3 m_exhaust_pos3= {0.0,0.1,6.8};
	VECTOR3 m_exhaust_pos4= {0.0,0.4,1.6};
	VECTOR3 m_exhaust_pos5= {0.0,-0.4,1.6};
	VECTOR3 m_exhaust_ref1 = {0.65,0,-1};
	VECTOR3 m_exhaust_ref2 = {-0.65,0,-1};
	VECTOR3 m_exhaust_ref3 = {0,0.5,-1};
	VECTOR3 m_exhaust_ref4 = {0.0,0.65,-1};
	VECTOR3 m_exhaust_ref5 = {0.0,-0.65,-1};

	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos1, 5.0, 0.15, &m_exhaust_ref1);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos2, 5.0, 0.15, &m_exhaust_ref2);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos3, 3.0, 0.10, &m_exhaust_ref3);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos4, 5.0, 0.15, &m_exhaust_ref4);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos5, 5.0, 0.15, &m_exhaust_ref5);

	//AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos4, 5.0, 0.25, &m_exhaust_ref4);

	SetView(0.0);

	ABORT_IND = true;

	OrbiterAttitudeToggle.SetState(false);
	LPswitch6.SetState(false);
}

//
// We really want an SM class to handle the retros and roll after seperation.
//

void Saturn::setupSM(OBJHANDLE hvessel)

{
	VESSEL *stg1vessel = oapiGetVesselInterface(hvessel);

	stg1vessel->SetSize (4.0);
	stg1vessel->SetCOG_elev (3.5);
	stg1vessel->SetEmptyMass (10318);
	stg1vessel->SetMaxFuelMass (2222);
	stg1vessel->SetFuelMass(2222);
	stg1vessel->SetISP (3083);
	stg1vessel->SetMaxThrust (ENGINE_ATTITUDE, 10980);
	stg1vessel->SetPMI (_V(30,30,15.5));
	stg1vessel->SetRotDrag (_V(0.7,0.7,0.3));
	stg1vessel->ClearMeshes();
	VECTOR3 mesh_dir=_V(0,SMVO,0);
	stg1vessel->AddMesh (hSM, &mesh_dir);

	//
	// Skylab SM and Apollo 7 have no HGA.
	//
	if (!NoHGA) {
		mesh_dir=_V(-2.2,-1.7,-1.6);
		stg1vessel->AddMesh (hSMhga, &mesh_dir);
	}

	//Roll left
}

bool Saturn::clbkLoadGenericCockpit ()

{
	TRACESETUP("Saturn::clbkLoadGenericCockpit");

	//
	// VC-only in engineering camera view.
	//

	if (viewpos == SATVIEW_ENG1 || viewpos == SATVIEW_ENG2)
		return false;

	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
	InVC = false;
	InPanel = false;
	SetView();
	return true;
}
