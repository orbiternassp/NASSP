/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn5_LEVA

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
  *	Revision 1.6  2005/12/28 19:11:32  movieman523
  *	Changed some mesh paths.
  *	
  *	Revision 1.5  2005/12/28 16:19:10  movieman523
  *	Should now be getting all config files from ProjectApollo directory.
  *	
  *	Revision 1.4  2005/11/23 00:29:38  movieman523
  *	Added S1C DLL and revised LEVA code to look for NEP-specific flag if it exists.
  *	
  *	Revision 1.3  2005/10/19 11:31:10  tschachim
  *	Changed log file name.
  *	
  *	Revision 1.2  2005/02/24 00:30:36  movieman523
  *	Major revision for Orbitersound 3.0 and new Orbiter. Added new 'flag planting' sound support.
  *	
  *	Revision 1.1  2005/02/11 12:54:06  tschachim
  *	Initial version
  *	
  **************************************************************************/

#define ORBITER_MODULE

#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "nasspsound.h"
#include "nasspdefs.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"
#include "sat5_lmpkd.h"

#include "saturn5_leva.h"
#include "tracer.h"

#include "CollisionSDK/CollisionSDK.h"

//
// Set the file name for the tracer code.
//

char trace_file[] = "ProjectApollo LEVA.log";

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};

//
// Variables that really should be global variables.
//

static 	int refcount = 0;
static MESHHANDLE hCMPEVA;
static MESHHANDLE hLRV;

Saturn5_LEVA::Saturn5_LEVA(OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	hMaster = hObj;
	init();
}

Saturn5_LEVA::~Saturn5_LEVA()

{
	// Nothing for now.
}

void Saturn5_LEVA::init()

{
	GoDock1=false;
	GoHover=false;
	starthover=false;
	GoRover =false;
	Astro=true;						
	MotherShip=false;
	EVAName[0]=0;
	CSMName[0]=0;
	MSName[0]=0;
	KEY1 = false;
	KEY2 = false;
	KEY3 = false;
	KEY4 = false;
	KEY5 = false;
	KEY6 = false;
	KEY7 = false;
	KEY8 = false;
	KEY9 = false;

	GoFlag = false;		 
	FlagPlanted = false;

	FirstTimestep = true;
	SLEVAPlayed = false;
	StateSet = false;

	lastLatLongSet = false;
	lastLat = 0;
	lastLong = 0;

	ApolloNo = 0;
	Realism = 0;

	// touchdown point test
	// touchdownPointHeight = -0.8;
}

void Saturn5_LEVA::clbkSetClassCaps (FILEHANDLE cfg)
{
	init();
	VSRegVessel(GetHandle());
	SetAstroStage();
}
		 
void Saturn5_LEVA::SetAstroStage ()
{
	SetEmptyMass(115);
	SetSize(2);
	SetPMI(_V(5,5,5));

	SetSurfaceFrictionCoeff(0.005, 0.5);
	SetRotDrag (_V(0, 0, 0));
	SetCW(0, 0, 0, 0);
	SetPitchMomentScale(0);
	SetBankMomentScale(0);
	SetLiftCoeffFunc(0); 
		
	ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	VECTOR3 mesh_dir=_V(0,0,0);
    AddMesh (hCMPEVA, &mesh_dir);
	SetCameraOffset(_V(0,1.6,0));
	
	double tdph = -0.8;
	SetTouchdownPoints (_V(0, tdph, 1), _V(-1, tdph, -1), _V(1, tdph, -1));
	VSSetTouchdownPoints(GetHandle(), _V(0, tdph, 1), _V(-1, tdph, -1), _V(1, tdph, -1), -tdph); // GetCOG_elev());
	Astro = true;
}

void Saturn5_LEVA::SetRoverStage ()
{
	SetEmptyMass(250);
	SetSize(10);
	SetPMI(_V(15,15,15));

	SetSurfaceFrictionCoeff(0.005, 0.5);
	SetRotDrag(_V(0, 0, 0));
	SetCW(0, 0, 0, 0);
	SetPitchMomentScale(0);
	SetBankMomentScale(0);
	SetLiftCoeffFunc(0); 

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	VECTOR3 mesh_dir=_V(0,0.0,0);
    AddMesh(hLRV, &mesh_dir);
	SetCameraOffset(_V(0,0,0));

	double tdph = -0.9;
	SetTouchdownPoints(_V(0, tdph, 3), _V(-3, tdph, -3), _V(3, tdph, -3));
	VSSetTouchdownPoints(GetHandle(), _V(0, tdph, 3), _V(-3, tdph, -3), _V(3, tdph, -3), -tdph); // GetCOG_elev());

	Astro=false;
}

void Saturn5_LEVA::ScanMotherShip()

{
	double VessCount;
	int i=0;

	VessCount=oapiGetVesselCount();
	for ( i = 0 ; i< VessCount ; i++ ) 
	{
		hMaster=oapiGetVesselByIndex(i);
		strcpy(EVAName,GetName());
		oapiGetObjectName(hMaster,CSMName,256);
		strcpy(MSName,CSMName);
		strcat(CSMName,"-LEVA");
		if (strcmp(CSMName, EVAName)==0){
			MotherShip=true;
			i=int(VessCount);
		}
		else{
			strcpy(CSMName,"");
		}
	}
}

void Saturn5_LEVA::MoveEVA()

{
	TRACESETUP("MoveEVA");
	double cm ;
	double lat;
	double lon;
	double cap;
	double timeW = 1;
	VESSELSTATUS eva;

	GetStatus(eva);
	oapiGetHeading(GetHandle(),&cap);
	timeW=oapiGetTimeAcceleration();
	if (timeW > 100)
		oapiSetTimeAcceleration(100);
	
	if (eva.status == 1) {
		lon = eva.vdata[0].x;
		lat = eva.vdata[0].y;
	
	} else if (lastLatLongSet) {
		lon = lastLong;
		lat = lastLat;
		eva.vdata[0].z = cap;
	
	} else return;

	if (Astro){
		cm = 0.3e-7 * timeW;
	}
	else{
		cm = 0.9e-7 * timeW;
	}
	if (KEY1){
		eva.vdata[0].z = eva.vdata[0].z - PI*0.5/180;
		if(eva.vdata[0].z <=-2*PI){
			eva.vdata[0].z = eva.vdata[0].z + 2*PI;
		}
		KEY1=false;
	}
	else if (KEY3){
		eva.vdata[0].z = eva.vdata[0].z +( PI*0.5/180);
		if(eva.vdata[0].z >=2*PI){
			eva.vdata[0].z = eva.vdata[0].z - 2*PI;
		}
		KEY3=false;
	}

	if (KEY2){
		lat = lat - cos(cap) * cm;
		lon = lon - sin(cap) * cm;
		KEY2=false;
	}
	else if (KEY4 && Astro){
		lat = lat + sin(cap) * cm;
		lon = lon - cos(cap) * cm;
		KEY4=false;
	}
	else if (KEY5){
		KEY5=false;
	}
	else if (KEY6 && Astro){
		lat = lat - sin(cap) * cm;
		lon = lon + cos(cap) * cm;
		KEY6=false;
	}
	else if (KEY7){
		KEY7=false;
	}
	else if (KEY8||GoHover && !Astro){// we go ahead whatever our headign
		lat = lat + cos(cap) * cm;
		lon = lon + sin(cap) * cm;
		KEY8=false;
	}
	else if (KEY9){
		KEY9=false;
	} else {
		//StopVesselWave(HAstro);
		//StopVesselWave(HRover);
	}

	eva.vdata[0].x = lon;
	eva.vdata[0].y = lat;
	eva.status = 1;
	DefSetState(&eva);

	lastLat = lat;
	lastLong = lon;
	lastLatLongSet = true;
}

// ==============================================================
// API interface
// ==============================================================

int Saturn5_LEVA::clbkConsumeDirectKey(char *kstate) {

	TRACESETUP("clbkConsumeDirectKey");

	if (FirstTimestep) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0; 
	}

	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD1)) {
		KEY1 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		KEY2 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		KEY3 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {			
		KEY4 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);			
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD5)) {			
		KEY5 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD5);			
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {			
		KEY6 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD7)) {			
		KEY7 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD7);			
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		KEY8 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);						
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD9)) {
		KEY9 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD9);
	}


	// touchdown point test
	/* 
	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		touchdownPointHeight += 0.01;
		RESETKEY(kstate, OAPI_KEY_A);

		SetTouchdownPoints (_V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
		VSSetTouchdownPoints(GetHandle(), _V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1), -touchdownPointHeight); // GetCOG_elev());
	}

	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		touchdownPointHeight -= 0.01;
		RESETKEY(kstate, OAPI_KEY_S);

		SetTouchdownPoints (_V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
		VSSetTouchdownPoints(GetHandle(), _V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1), -touchdownPointHeight); // GetCOG_elev());
	}
	*/

	return 0;
}

int Saturn5_LEVA::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	TRACESETUP("clbkConsumeBufferedKey");

	if (FirstTimestep) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0; 
	}

	if (key == OAPI_KEY_E && down == true) {				
		if (Astro) {
			GoDock1 = !GoDock1;
		}				
		return 1;
	}

	//
	// V switches to rover if applicable.
	//

	if (key == OAPI_KEY_V && down == true) {				
		if ((ApolloNo > 14) || (ApolloNo == 0) || (!Realism)) {
			GoRover = !GoRover;
			return 1;
		}
	}
		
	if (key == OAPI_KEY_G && down == true) {				
		if (!Astro)
			GoHover = !GoHover;	
		return 1;
	}

	if (key == OAPI_KEY_F && down == true) {				
		if (Astro && !FlagPlanted )
			GoFlag = true;	
		return 1;
	}

	return 0;
}

void Saturn5_LEVA::SetFlag()

{
	VESSELSTATUS vs1;

	GetStatus(vs1);
	VECTOR3 ofs1 = _V(3,0,0);
	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	Local2Rel (ofs1, vs1.rpos);
	GlobalRot (vel1, rofs1);
	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;

	double cm ;
	double lat;
	double lon;
	double cap;

	oapiGetHeading(GetHandle(),&cap);
	lon=vs1.vdata[0].x;
	lat=vs1.vdata[0].y;
	cm = 5.36e-7;
	lat = lat - sin(cap) * cm;
	lon = lon + cos(cap) * cm;
	vs1.vdata[0].x=lon;
	vs1.vdata[0].y=lat;

	//
	// Create the flag. For NEP support we load per-mission config if it exists.
	//
	
	char VName[256]="";
	char FName[256];

	sprintf(FName, "ProjectApollo/Apollo%d/sat5flag.cfg", ApolloNo);
	strcpy (VName, GetName()); strcat (VName, "-FLAG");

	FILE *fp = fopen(FName, "rt");
	if (fp) {
		fclose(fp);
		sprintf(FName, "ProjectApollo/Apollo%d/sat5flag", ApolloNo);
		oapiCreateVessel(VName, FName, vs1);
	}
	else {
		oapiCreateVessel(VName,"ProjectApollo/sat5flag",vs1);
	}

	FlagPlanted = true;
	GoFlag = false;

	FlagSound.play();
	FlagSound.done();
}

DLLCLBK void ovcTimestep (VESSEL *vessel, double simt)

{
	Saturn5_LEVA *sv = (Saturn5_LEVA *)vessel;

	sv->Timestep(simt);
}

void Saturn5_LEVA::SetMissionPath()

{
	char MissionName[24];

	_snprintf(MissionName, 23, "Apollo%d", ApolloNo);
	soundlib.SetSoundLibMissionPath(MissionName);
}

void Saturn5_LEVA::SetEVAStats(EVASettings &evas)

{
	ApolloNo = evas.MissionNo;
	Realism = evas.Realism;
	StateSet = true;
}

void Saturn5_LEVA::DoFirstTimestep()

{
	//
	// Load mission-specific sounds _AFTER_ the LEM has called us to set the Apollo mission number.
	//

	if (StateSet) {
		soundlib.InitSoundLib(GetHandle(), SOUND_DIRECTORY);
		SetMissionPath();

		//
		// Load sounds if they may be required.
		//

		if (!SLEVAPlayed)
			soundlib.LoadMissionSound(SLEVA, LEVA_SOUND, LEVA_SOUND);
		if (!FlagPlanted)
			soundlib.LoadMissionSound(FlagSound, FLAG_SPEECH, FLAG_SPEECH);

		//
		// Turn off pretty much everything that Orbitersound does by default.
		//

		soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
		soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
		soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
		soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);
		soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
		soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

		FirstTimestep = false;
	}
}

void Saturn5_LEVA::Timestep(double simt)

{
	VESSELSTATUS csmV;
	VESSELSTATUS evaV;
	VECTOR3 rdist = {0,0,0};
	VECTOR3 posr  = {0,0,0};
	VECTOR3 rvel  = {0,0,0};
	VECTOR3 RelRot  = {0,0,0};
	double dist = 0.0;
	double Vel = 0.0;

	StepCount++;

	if (FirstTimestep) {
		DoFirstTimestep();
		return;
	}
	else if (!SLEVAPlayed && StepCount > 20) {
		//
		// We can't play this immediately on creation, otherwise Orbitersound gets
		// confused by the focus change. Instead we have to wait a few timsteps.
		//
		SLEVA.play();
		SLEVA.done();
		SLEVAPlayed = true;
	}

	if (!MotherShip){
		ScanMotherShip();
	}
	else {
	}
	
	if (hMaster){
		sat5_lmpkd *lmvessel = (sat5_lmpkd *) oapiGetVesselInterface(hMaster);					
		oapiGetRelativePos (GetHandle() ,hMaster, &posr);
		oapiGetRelativeVel (GetHandle() ,hMaster , &rvel);
		GetStatus(evaV);
		lmvessel->GetStatus(csmV);
		GlobalRot (posr, RelRot);
		dist = sqrt(posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);
		Vel = sqrt(rvel.x * rvel.x + rvel.y * rvel.y + rvel.z * rvel.z);
		if (GoDock1) {						
			if (dist <= 6.00550) {
				GoDock1 = false;
				lmvessel->StopEVA();
				oapiSetFocusObject(hMaster);
				oapiDeleteVessel(GetHandle());
				return;
			}
		}
		
	}

	MoveEVA();

	if (GoRover && Astro){
		SetRoverStage();
	}
	else if(!GoRover && !Astro){
		SetAstroStage();
	}

	if (GoFlag && !FlagPlanted) {
		SetFlag();
	}

	// touchdown point test
	// sprintf(oapiDebugString(), "touchdownPointHeight %f", touchdownPointHeight);
}

DLLCLBK void ovcLoadState (VESSEL *vessel, FILEHANDLE scn, VESSELSTATUS *vs)

{
	Saturn5_LEVA *sv = (Saturn5_LEVA *)vessel;

	sv->LoadState(scn, vs);
}

void Saturn5_LEVA::LoadState(FILEHANDLE scn, VESSELSTATUS *vs)

{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "STATE", 5)) {
			int	s;
			sscanf(line + 5, "%d", &s);
			SetMainState(s);

			if (!Astro) {
				GoRover = true;
				SetEngineLevel(ENGINE_HOVER,1);
			}
		}
		else if (!strnicmp (line, "MISSIONNO", 9)) {
			sscanf(line + 9, "%d", &ApolloNo);
		}
		else if (!strnicmp (line, "REALISM", 7)) {
			sscanf(line + 7, "%d", &Realism);
		}
		else {
            ParseScenarioLine (line, vs);
        }
    }
}


DLLCLBK void ovcSaveState (VESSEL *vessel, FILEHANDLE scn)
{
	Saturn5_LEVA *sv = (Saturn5_LEVA *)vessel;
	sv->SaveState(scn);
}

typedef union {
	struct {
		unsigned int FlagPlanted:1;
		unsigned int StateSet:1;
		unsigned int SLEVAPlayed:1;
		unsigned int Astro:1;
	} u;
	unsigned int word;
} MainLEVAState;

int Saturn5_LEVA::GetMainState()

{
	MainLEVAState s;

	s.word = 0;
	s.u.FlagPlanted = FlagPlanted;
	s.u.StateSet = StateSet;
	s.u.SLEVAPlayed = SLEVAPlayed;
	s.u.Astro = Astro;

	return s.word;
}

void Saturn5_LEVA::SetMainState(int n)

{
	MainLEVAState s;

	s.word = n;
	FlagPlanted = (s.u.FlagPlanted != 0);
	StateSet = (s.u.StateSet != 0);
	SLEVAPlayed = (s.u.SLEVAPlayed != 0);
	Astro = (s.u.Astro != 0);
}

void Saturn5_LEVA::SaveState(FILEHANDLE scn)

{
	SaveDefaultState (scn);

	int s = GetMainState();
	if (s) {
		oapiWriteScenario_int (scn, "STATE", s);
	}

	if (ApolloNo != 0) {
		oapiWriteScenario_int (scn, "MISSIONNO", ApolloNo);
	}
	oapiWriteScenario_int (scn, "REALISM", Realism);
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hCMPEVA = oapiLoadMeshGlobal ("ProjectApollo/Sat5AstroS");
		hLRV = oapiLoadMeshGlobal ("ProjectApollo/LRV");
	}

	return new Saturn5_LEVA (hvessel, flightmodel);
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	Saturn5_LEVA *sv = (Saturn5_LEVA *) vessel;

	if (sv)
		delete sv;
}

DLLCLBK void InitModule (HINSTANCE hModule)
{
	InitCollisionSDK();
}
