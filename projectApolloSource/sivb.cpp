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
static MESHHANDLE hastp;
static MESHHANDLE hastp2;
static MESHHANDLE hCOAStarget;

void SIVbLoadMeshes()

{
	hSat1stg2 = oapiLoadMeshGlobal ("nsat1stg2");
	hSat1stg21 = oapiLoadMeshGlobal ("nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("nsat1stg24");
	hastp = oapiLoadMeshGlobal ("nASTP3");
	hastp2 = oapiLoadMeshGlobal ("nASTP2");
	hCOAStarget = oapiLoadMeshGlobal ("sat_target");
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

	hDock = 0;
	ph_aps = 0;
	ph_main = 0;
	thg_aps = 0;

	EmptyMass = 15000.0;

	for (i = 0; i < 10; i++)
		th_att_rot[i] = 0;
	for (i = 0; i < 2; i++)
		th_att_lin[i] = 0;
}

void SIVB::SetS4b()

{
	ClearMeshes();
	VECTOR3 mesh_dir=_V(0,0,0);

	ClearThrusterDefinitions();
	SetSize (15);
	SetEmptyMass (EmptyMass);
	SetPMI (_V(94,94,20));
	SetCOG_elev (10);
	SetCrossSections (_V(267, 267, 97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	ShiftCentreOfMass (_V(0, 0, 2.9));
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	AddMesh (hSat1stg2, &mesh_dir);
	if(Payload == PAYLOAD_TARGET){
		mesh_dir=_V(-1.0,-1.1,13.3);
		AddMesh (hCOAStarget, &mesh_dir);
		ClearDockDefinitions();
	}
	else if(Payload == PAYLOAD_ASTP){
		mesh_dir=_V(0,0,13.3);
		AddMesh (hastp, &mesh_dir);
	}
	else if(Payload == PAYLOAD_LM1){
		mesh_dir=_V(0,0,13.3);
		AddMesh (hCOAStarget, &mesh_dir);
	}
	else if (Payload == PAYLOAD_EMPTY) {
		ClearDockDefinitions();
	}

	AddRCS_S4B();
}

const VECTOR3 OFS_STAGE21 =  { 1.85,1.85, 15.3};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85, 15.3};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85, 15.3};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85, 15.3};

void SIVB::clbkPreStep(double simt, double simdt, double mjd)

{
	//
	// The only thing we'll do for now is seperate the SLA panels.
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

		ofs2 = OFS_STAGE21;
		vel2 = _V(0.5,0.5,-0.55);
		ofs3 = OFS_STAGE22;
		vel3 = _V(-0.5,0.5,-0.55);
		ofs4 = OFS_STAGE23;
		vel4 = _V(0.5,-0.5,-0.55);
		ofs5 = OFS_STAGE24;
		vel5 = _V(-0.5,-0.5,-0.55);

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

		PanelsOpened = true;
	}
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
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
}

typedef union {
	struct {
		unsigned PanelsHinged:1;
		unsigned PanelsOpened:1;
	} u;
	unsigned long word;
} MainState;

int SIVB::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.PanelsHinged = PanelsHinged;
	state.u.PanelsOpened = PanelsOpened;

	return state.word;
}

void SIVB::AddRCS_S4B()

{
	const double ATTCOOR = -10;
	const double ATTCOOR2 = 3.61;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double TRANZ=-3.2;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;
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
	offset = 2.65;

	if (!ph_aps)
		ph_aps  = CreatePropellantResource(275.0);

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
	Payload = state.Payload;
	PanelsHinged = state.PanelsHinged;
	VehicleNo = state.VehicleNo;
	EmptyMass = state.EmptyMass;

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
