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
  *	Revision 1.2  2005/02/24 00:26:35  movieman523
  *	Revised to delay switch to LEVA to work around Orbitersound getting upset if we switch immediately after creation.
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"

#include "landervessel.h"
#include "sat5_lmpkd.h"
#include "saturn5_leva.h"

static MESHHANDLE hLMPKD ;
static MESHHANDLE hLMVessel ;
static MESHHANDLE hLMDescent;
static MESHHANDLE hLMAscent ;
static MESHHANDLE hLMAscent2 ;
static MESHHANDLE hAstro1 ;


void sat5_lmpkd::ToggleEVA()

{
	ToggleEva=false;	
	
	if (EVA_IP){
		EVA_IP =false;
		ClearMeshes();
		ClearExhaustRefs();
		ClearAttExhaustRefs();
		VECTOR3 mesh_dir=_V(-0.08,0,0);
		AddMesh (hLMVessel, &mesh_dir);
	}
	else{
		EVA_IP =true;

		VESSELSTATUS vs1;
		GetStatus(vs1);

		VECTOR3 ofs1 = _V(0,0,0);
		VECTOR3 vel1 = _V(0,0,0);
		VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};

		Local2Rel (ofs1, vs1.rpos);
		GlobalRot (vel1, rofs1);
		vs1.rvel.x = rvel1.x+rofs1.x;
		vs1.rvel.y = rvel1.y+rofs1.y;
		vs1.rvel.z = rvel1.z+rofs1.z;
		//vs1.vdata[0].z = vs1.vdata[0].z +PI;

		char VName[256]="";
		strcpy (VName, GetName()); strcat (VName, "-LEVA");
		hLEVA = oapiCreateVessel(VName,"Saturn5_LEVA",vs1);

		ClearMeshes();
		ClearExhaustRefs();
		ClearAttExhaustRefs();
		VECTOR3 mesh_dir=_V(-0.08,-0.15,-4.3);
		AddMesh (hLMVessel, &mesh_dir);

		SwitchFocusToLeva = 10;

		Saturn5_LEVA *leva = (Saturn5_LEVA *) oapiGetVesselInterface(hLEVA);

		if (leva) {
			EVASettings evas;

			evas.MissionNo = agc.GetApolloNo();
			leva->SetEVAStats(evas);
		}
	}
}


void sat5_lmpkd::SetupEVA()

{
	if (EVA_IP){
		EVA_IP =true;
		ClearMeshes();
		VECTOR3 mesh_dir=_V(-0.08,-0.15,-4.3);
		AddMesh (hLMVessel, &mesh_dir);
	if(high){
		}else{
			high=true;
			SetTouchdownPoints (_V(0,-5,10), _V(-1,-5,-10), _V(1,-5,-10));
		}
	}
}

void sat5_lmpkd::SetLmVesselDockStage()

{	
	ClearThrusterDefinitions();
	agc.SetVesselStats(DPS_ISP, DPS_THRUST, true);
	SetSize (6);
	SetEmptyMass (6431.0);
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
	SetTouchdownPoints (_V(0,0,10), _V(-1,0,-10), _V(1,0,-10));

	VECTOR3 mesh_dir=_V(0.0,-0.2,0);//
	AddMesh (hLMPKD, &mesh_dir);
    if (!ph_Dsc)  
		ph_Dsc  = CreatePropellantResource(8165); //2nd stage Propellant
	SetDefaultPropellantResource (ph_Dsc); // display 2nd stage propellant level in generic HUD

	if (!ph_rcslm0) {
		ph_rcslm0 = CreatePropellantResource(100);
	}

	// orbiter main thrusters
	th_hover[0] =CreateThruster (_V( 0,-2.5,0), _V( 0,1,0), 44910, ph_Dsc, 3107);
	DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup (th_hover, 1, THGROUP_HOVER);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust_atrcs");//"Exhaust2"
	AddExhaust (th_hover[0], 15.0, 0.65);//

	//vessel->SetMaxThrust (ENGINE_ATTITUDE, 480);
	SetCameraOffset (_V(-1,1.0,0.0));
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-1.90);
	status = 0;
	stage = 0;
	bModeDocked=true;

	VECTOR3 dockpos = {0.0 ,2.4, 0};
	VECTOR3 dockdir = {0,1,0};
	VECTOR3 dockrot = {-0.7045, 0, 0.7045};
	SetDockParams(dockpos, dockdir, dockrot);
	InitNavRadios (4);
	SetEnableFocus(true);
    LDGswitch=false;
	ATT2switch=true;
	ATT3switch=true;
	ATT1switch=true;
	AFEED1switch=false;
	AFEED2switch=false;
	AFEED3switch=false;
	AFEED4switch=false;
}

void sat5_lmpkd::SetLmVesselHoverStage()
{
	ClearThrusterDefinitions();
	agc.SetVesselStats(DPS_ISP, DPS_THRUST, true);
	SetSize (7);
	SetEmptyMass (6431.0);
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
	SetTouchdownPoints (_V(0,5,10), _V(-1,5,-10), _V(1,5,-10));
	VECTOR3 mesh_dir=_V(0.0,0,0);
	AddMesh (hLMVessel, &mesh_dir);
    
	if (!ph_Dsc)  
		ph_Dsc  = CreatePropellantResource(8165); //2nd stage Propellant
	SetDefaultPropellantResource (ph_Dsc); // display 2nd stage propellant level in generic HUD
	if (!ph_rcslm0){
		ph_rcslm0 = CreatePropellantResource(100);
	}
	
	// orbiter main thrusters
	th_hover[0] = CreateThruster (_V( 0,-2.5,0), _V( 0,1,0), 44910, ph_Dsc, 3107);
	DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup (th_hover, 1, THGROUP_HOVER);
	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust_atrcs");//"Exhaust2"
	AddExhaust (th_hover[0], 15.0, 0.65);//
	
	//vessel->SetMaxThrust (ENGINE_ATTITUDE, 480);
	
	SetCameraOffset (_V(-1,1.0,0.0));
	status = 1;
	stage = 1;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH(-1.90);
	bModeHover=true;
	VECTOR3 dockpos = {0.0 ,2.4, 0};
	VECTOR3 dockdir = {0,1,0};
	VECTOR3 dockrot = {-0.7045, 0, 0.7045};
	SetDockParams(dockpos, dockdir, dockrot);
	InitNavRadios (4);
	//vessel->SetEnableFocus(false);

	LDGswitch=true;
	ATT2switch=true;
	ATT3switch=true;
	ATT1switch=true;
	AFEED1switch=false;
	AFEED2switch=false;
	AFEED3switch=false;
	AFEED4switch=false;
}


void sat5_lmpkd::SetLmAscentHoverStage()

{
	ClearThrusterDefinitions();
	agc.SetVesselStats(APS_ISP, APS_THRUST, true);
	ShiftCentreOfMass(_V(0.0,3.0,0.0));
	SetSize (5);
	SetCOG_elev (5);
	SetEmptyMass (2089.0);
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
	SetTouchdownPoints (_V(0,-5,10), _V(-1,-5,-10), _V(1,-5,-10));
	VECTOR3 mesh_dir=_V(-0.25,0.0,+0.38);
	AddMesh (hLMAscent, &mesh_dir);
    if (!ph_Asc)  
		ph_Asc  = CreatePropellantResource(2345); //2nd stage Propellant
	SetDefaultPropellantResource (ph_Asc); // display 2nd stage propellant level in generic HUD
	if (!ph_rcslm1){
		ph_rcslm1 = CreatePropellantResource(100);
	}
	// orbiter main thrusters
	th_hover[0] = CreateThruster (_V( 0,-0.9,0), _V( 0,1,0), 15880, ph_Asc, 2921);
	DelThrusterGroup(THGROUP_HOVER,true);
	thg_hover = CreateThrusterGroup (th_hover, 1, THGROUP_HOVER);
	SURFHANDLE tex = oapiRegisterExhaustTexture ("Exhaust_atrcs");//"Exhaust2"
	AddExhaust (th_hover[0], 15.0, 0.65);//

	//vessel->SetMaxThrust (ENGINE_ATTITUDE, 480);
	SetCameraOffset (_V(-1,1.0,0.0));
	status = 2;
	stage = 2;
	SetEngineLevel(ENGINE_HOVER,0);
	AddRCS_LMH2(-1.90);
	bModeHover=true;

	if(ph_Dsc){
		DelPropellantResource(ph_Dsc);
		ph_Dsc = 0;
	}
	if(ph_rcslm0){
		DelPropellantResource(ph_rcslm0);
	}
	VECTOR3 dockpos = {0.0 ,0.6, -0.3};
	VECTOR3 dockdir = {0,1,0};
	VECTOR3 dockrot = {-0.7045, 0, 0.7045};
	SetDockParams(dockpos, dockdir, dockrot);
	InitNavRadios (4);
	SetEnableFocus(false);
	LDGswitch=false;
	AFEED1switch=true;
	AFEED2switch=true;
	AFEED3switch=true;
	AFEED4switch=true;
}

void sat5_lmpkd::SeparateStage (UINT stage)

{
	ResetThrusters();

	VESSELSTATUS vs1;
	VECTOR3 ofs1 = _V(0,-5,0);
	VECTOR3 vel1 = _V(0,0,0);

	if (stage == 1)
	{
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
		StageS.play(NOLOOP, 150);

		char VName[256];
		strcpy (VName, GetName()); strcat (VName, "-DESCENTSTG");
		hdsc = oapiCreateVessel(VName,"sat5LMDSC",vs1);

		SetLmAscentHoverStage();
		ENGARMswitch=true;
		//vessel->SetEngineLevel(ENGINE_HOVER,1);
	}
}

void LEMLoadMeshes()

{
	hLMPKD = oapiLoadMeshGlobal ("LM_NoWheel");
	hLMVessel = oapiLoadMeshGlobal ("LM_vessel_on");
	hLMDescent = oapiLoadMeshGlobal ("LM_descent");
	hLMAscent = oapiLoadMeshGlobal ("LM_ascent");
	hLMAscent2= oapiLoadMeshGlobal ("LM_ascent2");
	hAstro1= oapiLoadMeshGlobal ("Sat5AstroS");
}

//
// Debug routine.
//

void sat5_lmpkd::GetDockStatus()

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
