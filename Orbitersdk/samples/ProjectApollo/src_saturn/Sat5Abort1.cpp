/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn1Abort

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

#include "sat5abort1.h"

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};


static int refcount = 0;
static MESHHANDLE hsat5stg1low;
static MESHHANDLE hsat5stg1;
static MESHHANDLE hsat5intstglow;
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
const double STG0O= 0;
const double STG1O= 10.25;
const double STG2O= 20;
const double PLOFS = 2.0;

Sat5Abort1::Sat5Abort1 (OBJHANDLE hObj, int fmodel)
: VESSEL3 (hObj, fmodel)

{
	init();
}

Sat5Abort1::~Sat5Abort1 ()

{
	// Nothing for now.
}

void Sat5Abort1::init()

{
	SmPresent = false;
	LowRes = false;
	PayloadType = PAYLOAD_EMPTY;

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

void Sat5Abort1::Setup()

{
	double Offset1st;

	SetSize (55);
	SetEmptyMass (704000.0);
	SetMaxFuelMass (2396000);
	SetFuelMass(2396000);
	//SetISP (ISP_FIRST_VAC);
	SetISP (2303.05);
	//slThrust = CalcSeaLevelThrust(THRUST_FIRST_VAC,ISP_FIRST_VAC,ISP_FIRST_SL, 407100);
	//vacThrust = THRUST_FIRST_VAC;
	SetMaxThrust (ENGINE_MAIN,0 );//slThrust
	SetMaxThrust (ENGINE_RETRO, 0);
	SetMaxThrust (ENGINE_HOVER, 0);
	SetMaxThrust (ENGINE_ATTITUDE, 0);
	SetPMI (_V(1147,1147,216.68));
	SetCrossSections (_V(1129,1133,152.4));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();
	
	double TCP = -102.5 + STG0O - TCPO;
	double Mass = 3100000;
	double ro = 5;
	TOUCHDOWNVTX td[4];
	double x_target = -0.05;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = -cos(30 * RAD)*ro;
	td[0].pos.y = -sin(30 * RAD)*ro;
	td[0].pos.z = TCP;
	td[1].pos.x = 0;
	td[1].pos.y = 1 * ro;
	td[1].pos.z = TCP;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = -sin(30 * RAD)*ro;
	td[2].pos.z = TCP;
	td[3].pos.x = 0;
	td[3].pos.y = 0;
	td[3].pos.z = TCP + 105;

	SetTouchdownPoints(td, 4);
	
	VECTOR3 mesh_dir=_V(0,0,-54+STG0O);
	if (LowRes) {
		AddMesh(hsat5stg1low, &mesh_dir);
		mesh_dir = _V(0, 0, -30.5 + STG0O);
		AddMesh(hsat5intstglow, &mesh_dir);
		mesh_dir = _V(0, 0, -17.2 + STG0O);
		AddMesh(hsat5stg2low, &mesh_dir);
		mesh_dir = _V(0, 0, 2. + STG0O);
		AddMesh(hsat5stg3low, &mesh_dir);
	}
	else
	{
		AddMesh(hsat5stg1, &mesh_dir);
		mesh_dir = _V(0, 0, -30.5 + STG0O);
		AddMesh(hsat5intstg, &mesh_dir);
		mesh_dir = _V(0, 0, -17.2 + STG0O);
		AddMesh(hsat5stg2, &mesh_dir);
		mesh_dir = _V(0, 0, 2. + STG0O);
		AddMesh(hsat5stg3, &mesh_dir);
	}

	if (SmPresent)
	{
		mesh_dir = _V(0, SMVO, 19.1 + STG0O);
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
	mesh_dir = _V(-1.48, -1.48, 14.55 + STG0O);
	panelMesh1SaturnV = AddMesh(hsat5stg31, &mesh_dir);
	panelMesh1SaturnVLow = AddMesh(hsat5stg31low, &mesh_dir);
	mesh_dir = _V(1.48, -1.48, 14.55 + STG0O);
	panelMesh2SaturnV = AddMesh(hsat5stg32, &mesh_dir);
	panelMesh2SaturnVLow = AddMesh(hsat5stg32low, &mesh_dir);
	mesh_dir = _V(1.48, 1.48, 14.55 + STG0O);
	panelMesh3SaturnV = AddMesh(hsat5stg33, &mesh_dir);
	panelMesh3SaturnVLow = AddMesh(hsat5stg33low, &mesh_dir);
	mesh_dir = _V(-1.48, 1.48, 14.55 + STG0O);
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

	mesh_dir=_V(0,0,23.5+STG0O);
	Offset1st = -80.1+STG0O;
	VECTOR3 m_exhaust_pos1= {3,3,Offset1st};
    VECTOR3 m_exhaust_pos2= {-3,-3,Offset1st};
	VECTOR3 m_exhaust_pos3= {-3,3,Offset1st};
	VECTOR3 m_exhaust_pos4= {3,-3,Offset1st};
	VECTOR3 m_exhaust_pos5= {0,0,Offset1st};

	VECTOR3 m_exhaust_ref = {0,0,-1};
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos1, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos2, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos3, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos4, 120.0, 5.80, &m_exhaust_ref);
	AddExhaustRef (EXHAUST_MAIN, m_exhaust_pos5, 120.0, 5.80, &m_exhaust_ref);
	SetCameraOffset (_V(-1,1.0,32.4+STG0O));
}

void Sat5Abort1::clbkPreStep(double simt, double simdt, double mjd) {

	//
	// Seperate or open the SLA panels.
	//

	if (!SmPresent) {
		if (panelTimestepCount < 2) {
			panelTimestepCount++;
		}
		else if (!PanelsOpened) {
			if (panelProc < RotationLimit) {
				panelProc = min(RotationLimit, panelProc + simdt / 5.0);
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

				PanelsOpened = true;
			}
		}
	}
}

void Sat5Abort1::clbkPostStep(double simt, double simdt, double mjd) {

	// Nothing for now
}

void Sat5Abort1::SetState(bool sm, bool lowres, int payload)
{
	SmPresent = sm;
	LowRes = lowres;
	PayloadType = payload;
	Setup();
}

void Sat5Abort1::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState(scn);

	papiWriteScenario_bool(scn, "SM", SmPresent);
	papiWriteScenario_bool(scn, "LOWRES", LowRes);
	oapiWriteScenario_int(scn, "PAYLOAD", PayloadType);
	oapiWriteScenario_float(scn, "PANELPROC", panelProc);
	papiWriteScenario_bool(scn, "PANELSOPENED", PanelsOpened);
}

void Sat5Abort1::clbkLoadStateEx(FILEHANDLE scn, void *vstatus)

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

void Sat5Abort1::DefineAnimations() {

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

void Sat5Abort1::HidePanelMeshes()

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
		hsat5stg1low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg1");
		hsat5stg1 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg1");
		hsat5intstglow = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5intstg");
		hsat5intstg = oapiLoadMeshGlobal ("ProjectApollo/sat5intstg");
		hsat5stg2low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg2");
		hsat5stg2 = oapiLoadMeshGlobal("ProjectApollo/sat5stg2");
		hsat5stg3low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg3");
		hsat5stg31low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg31");
		hsat5stg32low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg32");
		hsat5stg33low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg33");
		hsat5stg34low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg34");
		hsat5stg3 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg3");
		hsat5stg31 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg31");
		hsat5stg32 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg32");
		hsat5stg33 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg33");
		hsat5stg34 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg34");
		hSM = oapiLoadMeshGlobal ("ProjectApollo/SM_uext");
		hLM = oapiLoadMeshGlobal("ProjectApollo/LM_SLA");
		hLM1 = oapiLoadMeshGlobal("ProjectApollo/LM_1");
		hapollo8lta = oapiLoadMeshGlobal("ProjectApollo/apollo8_lta");
		hlta_2r = oapiLoadMeshGlobal("ProjectApollo/LTA_2R");
		hastp = oapiLoadMeshGlobal("ProjectApollo/nASTP3");
		hCOAStarget = oapiLoadMeshGlobal("ProjectApollo/sat_target");
	}
	return new Sat5Abort1 (hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat5Abort1*)vessel;
}

void Sat5Abort1::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL2::clbkSetClassCaps(cfg);
	init();
}

