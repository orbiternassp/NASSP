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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.8  2005/11/23 23:52:12  movieman523
  *	Removed some unused variables.
  *	
  *	Revision 1.7  2005/11/20 20:41:47  movieman523
  *	Fixed Saturn V SIVb engine exhaust after seperation.
  *	
  *	Revision 1.6  2005/11/20 20:35:14  movieman523
  *	Moved mesh files into ProjectApollo directory, and fixed RCS on Saturn V SIVb after seperation.
  *	
  *	Revision 1.5  2005/11/20 01:06:27  movieman523
  *	Saturn V now uses SIVB DLL too.
  *	
  *	Revision 1.4  2005/11/19 22:58:32  movieman523
  *	Pass main fuel mass from Saturn 1b to SIVb and added main thrust from venting fuel.
  *	
  *	Revision 1.3  2005/11/19 22:19:07  movieman523
  *	Revised interface to update SIVB, and added payload mass and stage empty mass.
  *	
  *	Revision 1.2  2005/11/19 22:05:16  movieman523
  *	Added RCS to SIVb stage after seperation.
  *	
  *	Revision 1.1  2005/11/19 21:27:31  movieman523
  *	Initial SIVb implementation.
  *	
  **************************************************************************/

#include "orbiterSDK.h"

#include "nasspdefs.h"
#include "sivb.h"

#include <stdio.h>
#include <string.h>

//
// Meshes are globally loaded.
//

static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;
static MESHHANDLE hastp;
static MESHHANDLE hastp2;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hLMPKD;
static MESHHANDLE hapollo8lta;
static MESHHANDLE hlta_2r;

void SIVbLoadMeshes()

{
	//
	// Saturn 1b.
	//

	hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
	hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
	hastp = oapiLoadMeshGlobal ("ProjectApollo/nASTP3");
	hastp2 = oapiLoadMeshGlobal ("ProjectApollo/nASTP2");
	hCOAStarget = oapiLoadMeshGlobal ("ProjectApollo/sat_target");

	//
	// Saturn V
	//

	hsat5stg3 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg3");
	hsat5stg31 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg31");
	hsat5stg32 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg32");
	hsat5stg33 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg33");
	hsat5stg34 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg34");

	hLMPKD = oapiLoadMeshGlobal ("ProjectApollo/LM_Parked");
	hapollo8lta = oapiLoadMeshGlobal ("ProjectApollo/apollo8_lta");
	hlta_2r = oapiLoadMeshGlobal ("ProjectApollo/LTA_2R");
}

SIVB::SIVB (OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	InitS4b();
}

SIVB::~SIVB()

{
	// Nothing for now.
}

void SIVB::InitS4b()

{
	int i;

	Payload = PAYLOAD_EMPTY;
	PanelsHinged = false;
	PanelsOpened = false;
	State = SIVB_STATE_WAITING;

	hDock = 0;
	ph_aps = 0;
	ph_main = 0;
	thg_aps = 0;

	EmptyMass = 15000.0;
	PayloadMass = 0.0;
	MainFuel = 5000.0;
	Realism = REALISM_DEFAULT;

	CurrentThrust = 0;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	for (i = 0; i < 10; i++)
		th_att_rot[i] = 0;
	for (i = 0; i < 2; i++)
		th_att_lin[i] = 0;
	th_main[0] = 0;
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
	ClearMeshes();
	VECTOR3 mesh_dir=_V(0,0,0);

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
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	if (SaturnVStage) {
		VECTOR3 dockpos = {0,0.03, 12.4};
		VECTOR3 dockdir = {0,0,1};
		VECTOR3 dockrot = {-0.705,-0.705,0};

		ShiftCentreOfMass (_V(0, 0, -15.0));

		AddMesh (hsat5stg3, &mesh_dir);

		switch (Payload) {
		case PAYLOAD_LEM:
			mesh_dir=_V(-0.0,0, 9.8);
			AddMesh (hLMPKD, &mesh_dir);
			SetDockParams(dockpos, dockdir, dockrot);
			mass += PayloadMass;
			break;

		case PAYLOAD_LTA:
		case PAYLOAD_LTA6:
			mesh_dir=_V(0.0,0, 9.6);
			AddMesh (hlta_2r, &mesh_dir);
			ClearDockDefinitions();
			mass += PayloadMass;
			break;

		case PAYLOAD_LTA8:
			mesh_dir=_V(0.0, 0, 8.8);
			AddMesh (hapollo8lta, &mesh_dir);
			ClearDockDefinitions();
			mass += PayloadMass;
			break;

		case PAYLOAD_EMPTY:
			ClearDockDefinitions();
			break;
		}
	}
	else {
		ShiftCentreOfMass (_V(0, 0, 2.9));
		AddMesh (hSat1stg2, &mesh_dir);

		switch (Payload) {

		case PAYLOAD_TARGET:
			mesh_dir=_V(-1.0,-1.1,13.3);
			AddMesh (hCOAStarget, &mesh_dir);
			ClearDockDefinitions();
			mass += PayloadMass;
			break;

		case PAYLOAD_ASTP:
			mesh_dir=_V(0,0,13.3);
			AddMesh (hastp, &mesh_dir);
			mass += PayloadMass;
			break;

		case PAYLOAD_LM1:
			mesh_dir=_V(0,0,13.3);
			AddMesh (hCOAStarget, &mesh_dir);
			mass += PayloadMass;
			break;

		case  PAYLOAD_EMPTY:
			ClearDockDefinitions();
			break;
		}
	}

	SetEmptyMass (mass);

	AddRCS_S4B();
}

const VECTOR3 OFS_STAGE21 =  { 1.85,1.85, 15.3};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85, 15.3};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85, 15.3};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85, 15.3};

const VECTOR3 OFS_STAGE31 =  { -1.48,-1.48, 12.7};
const VECTOR3 OFS_STAGE32 =  { 1.48,-1.48, 12.7};
const VECTOR3 OFS_STAGE33 =  { 1.48,1.48, 12.7};
const VECTOR3 OFS_STAGE34 =  { -1.48,1.48, 12.7};

void SIVB::clbkPreStep(double simt, double simdt, double mjd)

{

	MissionTime += simdt;

	//
	// Seperate the SLA panels.
	//

	if (!PanelsOpened) {
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
			ofs2 = OFS_STAGE31;
			vel2 = _V(-0.5,-0.5,-0.55);
			ofs3 = OFS_STAGE32;
			vel3 = _V(0.5,-0.5,-0.55);
			ofs4 = OFS_STAGE33;
			vel4 = _V(0.5,0.5,-0.55);
			ofs5 = OFS_STAGE34;
			vel5 = _V(-0.5,0.5,-0.55);
		}
		else {
			ofs2 = OFS_STAGE21;
			vel2 = _V(0.5,0.5,-0.55);
			ofs3 = OFS_STAGE22;
			vel3 = _V(-0.5,0.5,-0.55);
			ofs4 = OFS_STAGE23;
			vel4 = _V(0.5,-0.5,-0.55);
			ofs5 = OFS_STAGE24;
			vel5 = _V(-0.5,-0.5,-0.55);
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
			hs4b1 = oapiCreateVessel(VName, "sat5stg31", vs2);
			GetApolloName(VName); strcat (VName, "-S4B2");
			hs4b2 = oapiCreateVessel(VName, "sat5stg32", vs3);
			GetApolloName(VName); strcat (VName, "-S4B3");
			hs4b3 = oapiCreateVessel(VName, "sat5stg33", vs4);
			GetApolloName(VName); strcat (VName, "-S4B4");
			hs4b4 = oapiCreateVessel(VName, "sat5stg34", vs5);
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
			hs4b1 = oapiCreateVessel(VName, "nsat1stg21", vs2);
			GetApolloName(VName); strcat (VName, "-S4B2");
			hs4b2 = oapiCreateVessel(VName, "nsat1stg22", vs3);
			GetApolloName(VName); strcat (VName, "-S4B3");
			hs4b3 = oapiCreateVessel(VName, "nsat1stg23", vs4);
			GetApolloName(VName); strcat (VName, "-S4B4");
			hs4b4 = oapiCreateVessel(VName, "nsat1stg24", vs5);
		}

		PanelsOpened = true;
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
}


void SIVB::GetApolloName(char *s)

{
	sprintf(s, "AS-%d", VehicleNo);
}


void SIVB::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "S4PL", Payload);
	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_int (scn, "REALISM", Realism);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "PMASS", PayloadMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "CTR", CurrentThrust);
}

typedef union {
	struct {
		unsigned PanelsHinged:1;
		unsigned PanelsOpened:1;
		unsigned SaturnVStage:1;
	} u;
	unsigned long word;
} MainState;

int SIVB::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.PanelsHinged = PanelsHinged;
	state.u.PanelsOpened = PanelsOpened;
	state.u.SaturnVStage = SaturnVStage;

	return state.word;
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
	double offset;

	if (SaturnVStage) {
		offset = -2.05;
	}
	else {
		offset = 2.65;
	}

	if (!ph_aps)
		ph_aps  = CreatePropellantResource(275.0);

	if (!ph_main)
		ph_main = CreatePropellantResource(MainFuel);

	SetDefaultPropellantResource (ph_main);

	VECTOR3 m_exhaust_pos1;
	
	if (SaturnVStage) {
		m_exhaust_pos1 = _V(0, 0, -11.7);
	}
	else {
		m_exhaust_pos1 = _V(0, 0, -7);
	}

	//
	// The main engine is just venting fuel through the exhaust: low thrust and low ISP.
	//

	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), 1000.0, ph_main, 300.0, 300.0);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);
	AddExhaust (th_main[0], 1.5, .25);

//	if((ApolloNo<8)&&(ApolloNo!=6)&&(ApolloNo!=4))offset=7.7;

	//
	// Rotational thrusters are 150lb (666N) thrust. ISP is estimated.
	//

	th_att_rot[0] = CreateThruster (_V(0,ATTCOOR2+0.15,TRANZ-0.25+offset), _V(0, -1,0), 666.0, ph_aps, 3000.0, 3000.0);
	th_att_rot[1] = CreateThruster (_V(0,-ATTCOOR2-0.15,TRANZ-0.25+offset), _V(0,1,0), 666.0, ph_aps, 3000.0, 3000.0);
	
	AddExhaust (th_att_rot[0], 0.6, 0.078);
	AddExhaust (th_att_rot[1], 0.6, 0.078);
	CreateThrusterGroup (th_att_rot,   1, THGROUP_ATT_PITCHUP);
	CreateThrusterGroup (th_att_rot+1, 1, THGROUP_ATT_PITCHDOWN);

	th_att_rot[2] = CreateThruster (_V(RCSX,ATTCOOR2-0.2,TRANZ-0.25+offset), _V(-1,0,0), 666.0, ph_aps, 3000.0, 3000.0);
	th_att_rot[3] = CreateThruster (_V(-RCSX,-ATTCOOR2+0.2,TRANZ-0.25+offset), _V( 1,0,0), 666.0, ph_aps, 3000.0, 3000.0);
	th_att_rot[4] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V( 1,0,0), 666.0, ph_aps, 3000.0, 3000.0);
	th_att_rot[5] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), 666.0, ph_aps, 3000.0, 3000.0);

	AddExhaust (th_att_rot[2], 0.6, 0.078);
	AddExhaust (th_att_rot[3], 0.6, 0.078);
	AddExhaust (th_att_rot[4], 0.6, 0.078);
	AddExhaust (th_att_rot[5], 0.6, 0.078);
	CreateThrusterGroup (th_att_rot+2,   2, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+4, 2, THGROUP_ATT_BANKRIGHT);

	th_att_rot[6] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(1,0,0), 666.0, ph_aps, 3000.0, 3000.0);
	th_att_rot[7] = CreateThruster (_V(-RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(1,0,0), 666.0, ph_aps, 3000.0, 3000.0);
	th_att_rot[8] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), 666.0, ph_aps, 3000.0, 3000.0);
	th_att_rot[9] = CreateThruster (_V(RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(-1,0,0), 666.0, ph_aps, 3000.0, 3000.0);

	AddExhaust (th_att_rot[6], 0.6, 0.078);
	AddExhaust (th_att_rot[7], 0.6, 0.078);
	AddExhaust (th_att_rot[8], 0.6, 0.078);
	AddExhaust (th_att_rot[9], 0.6, 0.078);

	CreateThrusterGroup (th_att_rot+6,   2, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+8, 2, THGROUP_ATT_YAWRIGHT);

	//
	// APS thrusters are 320N (72 pounds) thrust
	//

	th_att_lin[0] = CreateThruster (_V(0,ATTCOOR2-0.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_aps, 3000.0, 3000.0);
	th_att_lin[1] = CreateThruster (_V(0,-ATTCOOR2+.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_aps, 3000.0, 3000.0);
	AddExhaust (th_att_lin[0], 7, 0.15);
	AddExhaust (th_att_lin[1], 7, 0.15);

	thg_aps = CreateThrusterGroup (th_att_lin, 2, THGROUP_ATT_FORWARD);
}

void SIVB::SetMainState(int s)

{
	MainState state;

	state.word = s;

	SaturnVStage = (state.u.SaturnVStage != 0);
	PanelsHinged = (state.u.PanelsHinged != 0);
	PanelsOpened = (state.u.PanelsOpened != 0);
}

void SIVB::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, "S4PL", 4)) {
			sscanf(line + 4, "%d", &Payload);
		}
		else if (!strnicmp (line, "MAINSTATE", 9)) {
            int MainState = 0;;
			sscanf (line+9, "%d", &MainState);
			SetMainState(MainState);
		}
		else if (!strnicmp (line, "VECHNO", 6)) {
			sscanf (line+6, "%d", &VehicleNo);
		}
		else if (!strnicmp (line, "EMASS", 5)) {
			sscanf (line+5, "%g", &flt);
			EmptyMass = flt;
		}
		else if (!strnicmp (line, "PMASS", 5)) {
			sscanf (line+5, "%g", &flt);
			PayloadMass = flt;
		}
		else if (!strnicmp (line, "FMASS", 5)) {
			sscanf (line+5, "%g", &flt);
			MainFuel = flt;
		}
		else if (!strnicmp(line, "MISSNTIME", 9)) {
            sscanf (line+9, "%f", &flt);
			MissionTime = flt;
		}
		else if (!strnicmp(line, "NMISSNTIME", 10)) {
            sscanf (line + 10, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp(line, "LMISSNTIME", 10)) {
            sscanf (line + 10, "%f", &flt);
			LastMissionEventTime = flt;
		}
		else if (!strnicmp(line, "CTR", 3)) {
            sscanf (line + 3, "%f", &flt);
			CurrentThrust = flt;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			sscanf (line+5, "%d", &State);
		}
		else if (!strnicmp (line, "REALISM", 7)) {
			sscanf (line+7, "%d", &Realism);
		}
		else {
			ParseScenarioLineEx (line, vstatus);
        }
	}

	SetS4b();
}

void SIVB::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	InitS4b();
}

void SIVB::clbkDockEvent(int dock, OBJHANDLE connected)

{
}

void SIVB::SetState(SIVBSettings &state)

{
	if (state.SettingsType & SIVB_SETTINGS_PAYLOAD) {
		Payload = state.Payload;
	}

	if (state.SettingsType & SIVB_SETTINGS_GENERAL) {
		MissionTime = state.MissionTime;
		SaturnVStage = state.SaturnVStage;
		PanelsHinged = state.PanelsHinged;
		VehicleNo = state.VehicleNo;
		Realism = state.Realism;
	}

	if (state.SettingsType & SIVB_SETTINGS_MASS) {
		EmptyMass = state.EmptyMass;
		PayloadMass = state.PayloadMass;
	}

	if (state.SettingsType & SIVB_SETTINGS_FUEL) {
		MainFuel = state.MainFuelKg;
	}

	SetS4b();
}

static int refcount = 0;

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	VESSEL *v;

	if (!refcount++) {
		SIVbLoadMeshes();
	}

	v = new SIVB (hvessel, flightmodel);
	return v;
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	--refcount;

	if (!refcount) {

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) 
		delete (SIVB *)vessel;
}
