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

#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "nasspsound.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "saturn5_leva.h"
#include "tracer.h"

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

Saturn5_LEVA::Saturn5_LEVA(OBJHANDLE hObj, int fmodel)
: VESSEL (hObj, fmodel)

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

	ApolloNo = 0;
}
		 
void Saturn5_LEVA::SetAstroStage ()
{
	SetEmptyMass (115);
	SetISP (23e6);
	SetCOG_elev(3);
	SetSize (2);
	SetPMI (_V(5,5,5));
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0); 
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
    ShiftCentreOfMass (_V(0,0,0));
	VECTOR3 mesh_dir=_V(0,0,0);
    AddMesh (hCMPEVA, &mesh_dir);
	SetCameraOffset(_V(0,1.6,0));
	SetTouchdownPoints (_V(0,-1,1), _V(-1,-1,-1), _V(1,-1,-1));
	Astro = true;
}


void Saturn5_LEVA::SetRoverStage ()
{
	SetEmptyMass (250);
	SetSize (2);
	SetCOG_elev(3);
	SetPMI (_V(15,15,15));
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0); 
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
    ShiftCentreOfMass (_V(0,0.0,0));
	VECTOR3 mesh_dir=_V(0,0.0,0);
    AddMesh (hLRV, &mesh_dir);
	SetCameraOffset(_V(0,0,0));
	SetTouchdownPoints (_V(0,-2,1), _V(-1,-2,-1), _V(1,-2,-1));
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
	double timeW=1;
	VESSELSTATUS eva;

	GetStatus(eva);
	oapiGetHeading(GetHandle(),&cap);
	timeW=oapiGetTimeAcceleration();
	if (timeW >100)
		oapiSetTimeAcceleration(100);
	lon=eva.vdata[0].x;
	lat=eva.vdata[0].y;

	if (Astro){
		cm = 0.3e-7*timeW;
	}
	else{
		cm = 0.9e-7*timeW;
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
	else if (KEY2){
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
	}else{
		//StopVesselWave(HAstro);
		//StopVesselWave(HRover);
	}

	eva.vdata[0].x=lon;
	eva.vdata[0].y=lat;
	DefSetState(&eva);
}

// ==============================================================
// API interface
// ==============================================================


DLLCLBK int ovcConsumeKey (VESSEL *vessel, const char *keystate)
{
	Saturn5_LEVA *sv = (Saturn5_LEVA *)vessel;
	return sv->ConsumeKey(keystate);
}

int Saturn5_LEVA::ConsumeKey(const char *keystate)

{
	TRACESETUP("ConsumeKey");

	if (KEYMOD_SHIFT (keystate)) 
	{
		return 0; 
	}
	else if (KEYMOD_CONTROL (keystate)) 
	{
	
	}
	else 
	{ 
		if (KEYDOWN (keystate, OAPI_KEY_E)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_E, 1)){
				
				if (Astro){
				GoDock1 = !GoDock1;
				}
				
			return 1;
			}
		}

		//
		// V switches to rover if applicable.
		//

		if (KEYDOWN (keystate, OAPI_KEY_V)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_V, 1.0)) {
				if ((ApolloNo > 14) || (ApolloNo == 0)) {
					GoRover = !GoRover;
					return 1;
				}
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_G)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_G, 1.0)){
				if (!Astro)
					GoHover = !GoHover;	
				return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_F)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_F, 1.0)){
				if (Astro && !FlagPlanted )
				GoFlag = true;	
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD1)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD1, .001)){
				KEY1 = true;
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD3)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD3, 0.001)){
				KEY3 = true;
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD2)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD2, .001)){
				KEY2 = true;
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD4)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD4, 0.001)){
				
				KEY4 = true;
				
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD5)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD5, .001)){
				
				KEY5 = true;
				
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD6)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD6, .001)){
				
				KEY6 = true;
				
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD7)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD7, .001)){
				
				KEY7 = true;
				
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD8)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD8, .001)){
				
				KEY8 = true;
				
			return 1;
			}
		}
		if (KEYDOWN (keystate, OAPI_KEY_NUMPAD9)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_NUMPAD9, .001)){	
				KEY9 = true;
				return 1;
			}
		}
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
		oapiCreateVessel(VName,"Sat5flag",vs1);
	}
	else {
		oapiCreateVessel(VName,"Sat5flag",vs1);
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

		FirstTimestep = false;
	}
}

void Saturn5_LEVA::Timestep(double simt)

{
	VESSELSTATUS csmV;
	VESSELSTATUS evaV;
	VESSEL *csmvessel;
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
		csmvessel = oapiGetVesselInterface(hMaster);					
		oapiGetRelativePos (GetHandle() ,hMaster, &posr);
		oapiGetRelativeVel (GetHandle() ,hMaster , &rvel);
		GetStatus(evaV);
		csmvessel->GetStatus(csmV);
		GlobalRot (posr, RelRot);
		dist = sqrt(posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);
		Vel = sqrt(rvel.x * rvel.x + rvel.y * rvel.y + rvel.z * rvel.z);
		if (GoDock1){						
			if (dist <= 6.00550){
				GoDock1 =false;
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
		else if (!strnicmp (line, "MISSIONNO", 8)) {
			sscanf(line + 8, "%d", &ApolloNo);
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
	
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hCMPEVA = oapiLoadMeshGlobal ("Sat5AstroS");
		hLRV = oapiLoadMeshGlobal ("LRV");
	}

	return new Saturn5_LEVA (hvessel, flightmodel);
}


DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	Saturn5_LEVA *sv = (Saturn5_LEVA *)vessel;

	sv->init();
	sv->SetAstroStage ();
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	Saturn5_LEVA *sv = (Saturn5_LEVA *) vessel;

	if (sv)
		delete sv;
}