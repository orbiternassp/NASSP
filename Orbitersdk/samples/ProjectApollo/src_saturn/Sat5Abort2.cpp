/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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
#include "orbitersdk.h"
#include "stdio.h"
#include "nasspdefs.h"
#include "papi.h"
#include "OrbiterMath.h"

#include "sat5abort2.h"

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};


static int refcount = 0;
static MESHHANDLE hsat5intstglow;
static MESHHANDLE hsat5intstg4;
static MESHHANDLE hsat5intstg8;
static MESHHANDLE hsat5intstg;
static MESHHANDLE hsat5stg2low;
static MESHHANDLE hsat5stg2;
static MESHHANDLE hsat5stg3low;
static MESHHANDLE hsat5stg31low;
static MESHHANDLE hsat5stg32low;
static MESHHANDLE hsat5stg33low;
static MESHHANDLE hsat5stg34low;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;
static MESHHANDLE hsat5tower;
static MESHHANDLE hSM;
static MESHHANDLE hLM;
static MESHHANDLE hLM1;
static MESHHANDLE hastp;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hapollo8lta;
static MESHHANDLE hlta_2r;

const double TCPO=0;
const double SMVO = -0.14;
const double STG0O= 12.4;
const double STG1O= 0;
const double STG2O= 20;
const double PLOFS = 2.0;

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

Sat5Abort2::Sat5Abort2 (OBJHANDLE hObj, int fmodel)
: VESSEL3 (hObj, fmodel)

{
	init();
}

Sat5Abort2::~Sat5Abort2 ()

{
	// Nothing for now.
}

void Sat5Abort2::init() {

	SmPresent = false;
	LowRes = false;
	PayloadType = PAYLOAD_EMPTY;
	InterStage = true;
	UllageNum = 0;

	thg_sep = 0;
	thg_sepPanel = 0;

	PanelsOpened = false;
	RotationLimit = 0.25;
	panelProc = 0;
	panelTimestepCount = 0;
	panelMesh1SaturnV = -1;
	panelMesh2SaturnV = -1;
	panelMesh3SaturnV = -1;
	panelMesh4SaturnV = -1;
	panelMesh1SaturnVLow = -1;
	panelMesh2SaturnVLow = -1;
	panelMesh3SaturnVLow = -1;
	panelMesh4SaturnVLow = -1;
	panelAnim = 0;
}

void Sat5Abort2::Setup()

{
	SetSize (15);
	SetCOG_elev (15.225);
	SetEmptyMass (233000);
	SetMaxFuelMass (471000);
	SetFuelMass(444760);
	//SetISP (ISP_SECOND_VAC);
	SetISP (4780);
	//slThrust = CalcSeaLevelThrust(THRUST_SECOND_VAC,ISP_SECOND_VAC,ISP_SECOND_SL, 106000);
	//vacThrust = THRUST_SECOND_VAC;
	//vessel->SetMaxThrust (ENGINE_MAIN, slThrust);
	SetMaxThrust (ENGINE_MAIN, 6008625);
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 5e4);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(374,374,80));
	SetCrossSections (_V(524,524,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	//vessel->ShiftCentreOfMass (_V(0,0,12.25));

	VECTOR3 mesh_dir = _V(0, 0, -17.2 - STG1O);
	if (LowRes) {
		AddMesh(hsat5stg2low, &mesh_dir);
		mesh_dir = _V(0, 0, 2. - STG1O);
		AddMesh(hsat5stg3low, &mesh_dir);
	}
	else
	{
		mesh_dir = _V(0, 0, -17.2 - STG1O);
		AddMesh(hsat5stg2, &mesh_dir);
		mesh_dir = _V(0, 0, 2. - STG1O);
		AddMesh(hsat5stg3, &mesh_dir);
	}

	// Interstage mesh
	if (InterStage) {
		mesh_dir = _V(0, 0, -30.5 - STG1O);
		if (LowRes) {
			AddMesh(hsat5intstglow, &mesh_dir);
		}
		else
		{
			switch (UllageNum)
			{
			case 4:
				AddMesh(hsat5intstg4, &mesh_dir);
				break;

			case 8:
				AddMesh(hsat5intstg8, &mesh_dir);
				break;

			default:
				AddMesh(hsat5intstg, &mesh_dir);
				break;
			}
		}
	}

	if (SmPresent)
	{
		mesh_dir = _V(0, SMVO, 19.1 - STG1O);
		AddMesh(hSM, &mesh_dir);
	}
	else
	{
		switch (PayloadType) {
		case PAYLOAD_LEM:
			mesh_dir = _V(0, 0, 9.8 + PLOFS);
			AddMesh(hLM, &mesh_dir);
			break;

		case PAYLOAD_LM1:
			mesh_dir = _V(0, 0, 9.8 + PLOFS);
			AddMesh(hLM1, &mesh_dir);
			break;

		case PAYLOAD_LTA:
		case PAYLOAD_LTA6:
			mesh_dir = _V(0, 0, 9.6 + PLOFS);
			AddMesh(hlta_2r, &mesh_dir);
			break;

		case PAYLOAD_LTA8:
			mesh_dir = _V(0.0, 0, 8.8 + PLOFS);
			AddMesh(hapollo8lta, &mesh_dir);
			break;

		case PAYLOAD_DOCKING_ADAPTER:
			mesh_dir = _V(0, -0.15, 7.8 + PLOFS);
			AddMesh(hastp, &mesh_dir);
			mesh_dir = _V(-1.04, 1.04, 9.1 + PLOFS);
			AddMesh(hCOAStarget, &mesh_dir);
			break;

		case PAYLOAD_EMPTY:
			break;

		case PAYLOAD_TARGET:
			mesh_dir = _V(-1.3, 0, 9.6 + PLOFS);
			AddMesh(hCOAStarget, &mesh_dir);
			break;

		case PAYLOAD_ASTP:
			mesh_dir = _V(0, 0, 8.6 + PLOFS);
			AddMesh(hastp, &mesh_dir);
			break;
		}
	}

	// SLA panel meshes
	mesh_dir = _V(-1.48, -1.48, 14.55 - STG1O);
	panelMesh1SaturnV = AddMesh(hsat5stg31, &mesh_dir);
	panelMesh1SaturnVLow = AddMesh(hsat5stg31low, &mesh_dir);
	mesh_dir = _V(1.48, -1.48, 14.55 - STG1O);
	panelMesh2SaturnV = AddMesh(hsat5stg32, &mesh_dir);
	panelMesh2SaturnVLow = AddMesh(hsat5stg32low, &mesh_dir);
	mesh_dir = _V(1.48, 1.48, 14.55 - STG1O);
	panelMesh3SaturnV = AddMesh(hsat5stg33, &mesh_dir);
	panelMesh3SaturnVLow = AddMesh(hsat5stg33low, &mesh_dir);
	mesh_dir = _V(-1.48, 1.48, 14.55 - STG1O);
	panelMesh4SaturnV = AddMesh(hsat5stg34, &mesh_dir);
	panelMesh4SaturnVLow = AddMesh(hsat5stg34low, &mesh_dir);

	HidePanelMeshes();

	if (!PanelsOpened) {
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

	DefineAnimations();

	VECTOR3 m_exhaust_ref = {0,0,-1};
	VECTOR3 m_exhaust_pos1= {0,0,-28.25-STG1O};
	VECTOR3 m_exhaust_pos2= {2,2,-28.25-STG1O};
	VECTOR3 m_exhaust_pos3= {-2,2,-28.25-STG1O};
	VECTOR3 m_exhaust_pos4 = {2,-2.0,-28.25-STG1O};
	VECTOR3 m_exhaust_pos5 = {-2,-2,-28.25-STG1O};
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos1, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos2, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos3, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos4, 25.0, 1.5, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos5, 25.0, 1.5, &m_exhaust_ref);
	status = 3;
	SetCameraOffset (_V(-1,1.0,32.4-STG1O));

	AddSepJunk();
}

void Sat5Abort2::clbkPreStep(double simt, double simdt, double mjd) {

	//
	// Seperate or open the SLA panels.
	//

	if (!SmPresent) {
		if (panelTimestepCount < 2) {
			panelTimestepCount++;
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

				//
				// I'm not sure that all this code is really needed, but
				// it came from saturn1bmesh.cpp.
				//

				VESSELSTATUS vs2;
				VESSELSTATUS vs3;
				VESSELSTATUS vs4;
				VESSELSTATUS vs5;
				VECTOR3 ofs2 = _V(0, 0, 0);
				VECTOR3 ofs3 = _V(0, 0, 0);
				VECTOR3 ofs4 = _V(0, 0, 0);
				VECTOR3 ofs5 = _V(0, 0, 0);
				VECTOR3 vel2 = _V(0, 0, 0);
				VECTOR3 vel3 = _V(0, 0, 0);
				VECTOR3 vel4 = _V(0, 0, 0);
				VECTOR3 vel5 = _V(0, 0, 0);

				GetStatus(vs2);
				GetStatus(vs3);
				GetStatus(vs4);
				GetStatus(vs5);

				vs2.eng_main = vs2.eng_hovr = 0.0;
				vs3.eng_main = vs3.eng_hovr = 0.0;
				vs4.eng_main = vs4.eng_hovr = 0.0;
				vs5.eng_main = vs5.eng_hovr = 0.0;


				ofs2 = _V(-3.25, -3.3, 12.2 + PLOFS);
				vel2 = _V(-0.5, -0.5, -0.55);
				ofs3 = _V(3.25, -3.3, 12.2 + PLOFS);
				vel3 = _V(0.5, -0.5, -0.55);
				ofs4 = _V(3.25, 3.3, 12.2 + PLOFS);
				vel4 = _V(0.5, 0.5, -0.55);
				ofs5 = _V(-3.25, 3.3, 12.2 + PLOFS);
				vel5 = _V(-0.5, 0.5, -0.55);

				VECTOR3 rofs2, rvel2 = { vs2.rvel.x, vs2.rvel.y, vs2.rvel.z };
				VECTOR3 rofs3, rvel3 = { vs3.rvel.x, vs3.rvel.y, vs3.rvel.z };
				VECTOR3 rofs4, rvel4 = { vs4.rvel.x, vs4.rvel.y, vs4.rvel.z };
				VECTOR3 rofs5, rvel5 = { vs5.rvel.x, vs5.rvel.y, vs5.rvel.z };
				Local2Rel(ofs2, vs2.rpos);
				Local2Rel(ofs3, vs3.rpos);
				Local2Rel(ofs4, vs4.rpos);
				Local2Rel(ofs5, vs5.rpos);
				GlobalRot(vel2, rofs2);
				GlobalRot(vel3, rofs3);
				GlobalRot(vel4, rofs4);
				GlobalRot(vel5, rofs5);
				vs2.rvel.x = rvel2.x + rofs2.x;
				vs2.rvel.y = rvel2.y + rofs2.y;
				vs2.rvel.z = rvel2.z + rofs2.z;
				vs3.rvel.x = rvel3.x + rofs3.x;
				vs3.rvel.y = rvel3.y + rofs3.y;
				vs3.rvel.z = rvel3.z + rofs3.z;
				vs4.rvel.x = rvel4.x + rofs4.x;
				vs4.rvel.y = rvel4.y + rofs4.y;
				vs4.rvel.z = rvel4.z + rofs4.z;
				vs5.rvel.x = rvel5.x + rofs5.x;
				vs5.rvel.y = rvel5.y + rofs5.y;
				vs5.rvel.z = rvel5.z + rofs5.z;

				//
				// This should be rationalised really to use the same parameters
				// with different config files.
				//

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


				hs4b1 = oapiCreateVessel("ABORT-S4B1", "ProjectApollo/sat5stg31", vs2);
				hs4b2 = oapiCreateVessel("ABORT-S4B2", "ProjectApollo/sat5stg32", vs3);
				hs4b3 = oapiCreateVessel("ABORT-S4B3", "ProjectApollo/sat5stg33", vs4);
				hs4b4 = oapiCreateVessel("ABORT-S4B4", "ProjectApollo/sat5stg34", vs5);

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

				// Activate panel separation junk
				if (thg_sepPanel)
					SetThrusterGroupLevel(thg_sepPanel, 1);

				PanelsOpened = true;
			}
		}
	}
}

void Sat5Abort2::clbkPostStep(double simt, double simdt, double mjd) {

	// Nothing for now
}

void Sat5Abort2::SetState(bool sm, bool lowres, int payload, bool interstage, int ullagenum)
{
	SmPresent = sm;
	LowRes = lowres;
	PayloadType = payload;
	InterStage = interstage;
	UllageNum = ullagenum;

	Setup();
}

void Sat5Abort2::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState(scn);

	papiWriteScenario_bool(scn, "SM", SmPresent);
	papiWriteScenario_bool(scn, "LOWRES", LowRes);
	oapiWriteScenario_int(scn, "PAYLOAD", PayloadType);
	papiWriteScenario_bool(scn, "INTERSTAGE", InterStage);
	oapiWriteScenario_int(scn, "ULLAGENUM", UllageNum);
	oapiWriteScenario_float(scn, "PANELPROC", panelProc);
	papiWriteScenario_bool(scn, "PANELSOPENED", PanelsOpened);
}

void Sat5Abort2::clbkLoadStateEx(FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!_strnicmp(line, "SM", 2))
		{
			int i;
			sscanf(line + 2, "%d", &i);
			SmPresent = (i != 0);
		}
		else if (!_strnicmp(line, "LOWRES", 6))
		{
			int i;
			sscanf(line + 6, "%d", &i);
			LowRes = (i != 0);
		}
		else if (!_strnicmp(line, "PAYLOAD", 7))
		{
			sscanf(line + 7, "%d", &PayloadType);
		}
		else if (!_strnicmp(line, "INTERSTAGE", 10))
		{
			int i;
			sscanf(line + 10, "%d", &i);
			InterStage = (i != 0);
		}
		else if (!_strnicmp(line, "ULLAGENUM", 9))
		{
			sscanf(line + 9, "%d", &UllageNum);
		}
		else if (!strnicmp(line, "PANELPROC", 9))
		{
			sscanf(line + 9, "%f", &flt);
			panelProc = flt;
		}
		else if (!_strnicmp(line, "PANELSOPENED", 12))
		{
			int i;
			sscanf(line + 12, "%d", &i);
			PanelsOpened = (i != 0);
		}
		else
		{
			ParseScenarioLineEx(line, vstatus);
		}
	}

	Setup();
}

void Sat5Abort2::DefineAnimations() {

	panelAnim = CreateAnimation(0.0);

	static MGROUP_ROTATE panel1SaturnV(panelMesh1SaturnV, NULL, 0, _V(-0.6, -0.6, -3.2), _V(1, -1, 0) / length(_V(1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2SaturnV(panelMesh2SaturnV, NULL, 0, _V(0.6, -0.6, -3.2), _V(1, 1, 0) / length(_V(1, 1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3SaturnV(panelMesh3SaturnV, NULL, 0, _V(0.6, 0.6, -3.2), _V(-1, 1, 0) / length(_V(-1, 1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4SaturnV(panelMesh4SaturnV, NULL, 0, _V(-0.6, 0.6, -3.2), _V(-1, -1, 0) / length(_V(-1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel1SaturnVLow(panelMesh1SaturnVLow, NULL, 0, _V(-0.6, -0.6, -3.2), _V(1, -1, 0) / length(_V(1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2SaturnVLow(panelMesh2SaturnVLow, NULL, 0, _V(0.6, -0.6, -3.2), _V(1, 1, 0) / length(_V(1, 1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3SaturnVLow(panelMesh3SaturnVLow, NULL, 0, _V(0.6, 0.6, -3.2), _V(-1, 1, 0) / length(_V(-1, 1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4SaturnVLow(panelMesh4SaturnVLow, NULL, 0, _V(-0.6, 0.6, -3.2), _V(-1, -1, 0) / length(_V(-1, -1, 0)), (float)(1.0 * PI));

	AddAnimationComponent(panelAnim, 0, 1, &panel1SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel2SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel3SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel4SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel1SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel2SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel3SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel4SaturnVLow);

	SetAnimation(panelAnim, panelProc);
}

void Sat5Abort2::AddSepJunk(){

	//
	// Seperation junk 'thrusters'.
	//

	int i;
	double junkOffset;
	junkOffset = 16 - STG1O + PLOFS;

	VECTOR3	s_exhaust_pos1 = _V(1.41, 1.41, junkOffset);
	VECTOR3 s_exhaust_pos2 = _V(1.41, -1.41, junkOffset);
	VECTOR3	s_exhaust_pos3 = _V(-1.41, 1.41, junkOffset);
	VECTOR3 s_exhaust_pos4 = _V(-1.41, -1.41, junkOffset);
	VECTOR3	s_exhaust_pos6 = _V(0, 2.0, junkOffset);
	VECTOR3 s_exhaust_pos7 = _V(0, -2.0, junkOffset);
	VECTOR3	s_exhaust_pos8 = _V(2.0, 0, junkOffset);
	VECTOR3 s_exhaust_pos9 = _V(-2.0, 0, junkOffset);

	PROPELLANT_HANDLE ph_sep = CreatePropellantResource(0.2);

	THRUSTER_HANDLE th_sep[8];
	th_sep[0] = CreateThruster(s_exhaust_pos1, _V(-1, -1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[1] = CreateThruster(s_exhaust_pos2, _V(-1, 1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[2] = CreateThruster(s_exhaust_pos3, _V(1, -1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[3] = CreateThruster(s_exhaust_pos4, _V(1, 1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[4] = CreateThruster(s_exhaust_pos6, _V(0, -1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[5] = CreateThruster(s_exhaust_pos7, _V(0, 1, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[6] = CreateThruster(s_exhaust_pos8, _V(-1, 0, 0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[7] = CreateThruster(s_exhaust_pos9, _V(1, 0, 0), 1.0, ph_sep, 10.0, 10.0);

	for (i = 0; i < 8; i++) {
		AddExhaustStream(th_sep[i], &seperation_junk);
	}
	thg_sep = CreateThrusterGroup(th_sep, 8, THGROUP_USER);

	//
	// Panel seperation junk 'thrusters'.
	//

	junkOffset = 9.4 - STG1O + PLOFS;

	double r = 2.15;
	VECTOR3	sPanel_exhaust_pos1 = _V(r, r, junkOffset);
	VECTOR3 sPanel_exhaust_pos2 = _V(r, -r, junkOffset);
	VECTOR3	sPanel_exhaust_pos3 = _V(-r, r, junkOffset);
	VECTOR3 sPanel_exhaust_pos4 = _V(-r, -r, junkOffset);

	PROPELLANT_HANDLE ph_sepPanel = CreatePropellantResource(0.1);

	THRUSTER_HANDLE th_sepPanel[4];
	th_sepPanel[0] = CreateThruster(sPanel_exhaust_pos1, _V(-1, -1, 0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[1] = CreateThruster(sPanel_exhaust_pos2, _V(-1, 1, 0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[2] = CreateThruster(sPanel_exhaust_pos3, _V(1, -1, 0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[3] = CreateThruster(sPanel_exhaust_pos4, _V(1, 1, 0), 1.0, ph_sepPanel, 10.0, 10.0);

	for (i = 0; i < 4; i++) {
		AddExhaustStream(th_sepPanel[i], &seperation_junk);
	}
	thg_sepPanel = CreateThrusterGroup(th_sepPanel, 4, THGROUP_USER);
}

void Sat5Abort2::HidePanelMeshes()

{
	SetMeshVisibilityMode(panelMesh1SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh1SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4SaturnVLow, MESHVIS_NEVER);
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hsat5intstglow = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5intstg");
		hsat5intstg4 = oapiLoadMeshGlobal("ProjectApollo/sat5intstg4");
		hsat5intstg8 = oapiLoadMeshGlobal("ProjectApollo/sat5intstg8");
		hsat5intstg = oapiLoadMeshGlobal("ProjectApollo/sat5intstg");
		hsat5stg2low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg2");
		hsat5stg2 = oapiLoadMeshGlobal("ProjectApollo/sat5stg2");
		hsat5stg3low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg3");
		hsat5stg31low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg31");
		hsat5stg32low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg32");
		hsat5stg33low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg33");
		hsat5stg34low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg34");
		hsat5stg3 = oapiLoadMeshGlobal("ProjectApollo/sat5stg3");
		hsat5stg31 = oapiLoadMeshGlobal("ProjectApollo/sat5stg31");
		hsat5stg32 = oapiLoadMeshGlobal("ProjectApollo/sat5stg32");
		hsat5stg33 = oapiLoadMeshGlobal("ProjectApollo/sat5stg33");
		hsat5stg34 = oapiLoadMeshGlobal("ProjectApollo/sat5stg34");
		hSM = oapiLoadMeshGlobal("ProjectApollo/SM_uext");
		hLM = oapiLoadMeshGlobal("ProjectApollo/LM_SLA");
		hLM1 = oapiLoadMeshGlobal("ProjectApollo/LM_1");
		hapollo8lta = oapiLoadMeshGlobal("ProjectApollo/apollo8_lta");
		hlta_2r = oapiLoadMeshGlobal("ProjectApollo/LTA_2R");
		hastp = oapiLoadMeshGlobal("ProjectApollo/nASTP3");
		hCOAStarget = oapiLoadMeshGlobal("ProjectApollo/sat_target");

		seperation_junk.tex = oapiRegisterParticleTexture("ProjectApollo/junk");

	}
	return new Sat5Abort2 (hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat5Abort2*)vessel;
}

void Sat5Abort2::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL2::clbkSetClassCaps(cfg);
	init();
}

