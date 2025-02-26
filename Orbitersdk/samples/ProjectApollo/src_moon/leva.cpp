/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: LEVA

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

#define ORBITER_MODULE

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "nasspsound.h"
#include "nasspdefs.h"
#include "soundlib.h"

#include "toggleswitch.h"
#include "LEM.h"

#include "leva.h"
#include "LRV.h"
#include "lrv_console.h"
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
// Astronaut and rover movement capabilities
//

#define ROVER_SPEED_M_S 3.6  // max rover speed in m/s (http://en.wikipedia.org/wiki/Lunar_rover; values on the web range from 7 mph to 10.56 mph)
#define ROVER_ACC_M_S2 4.0   // rover acceleration in m/s^2 (not based on real data (yet))
#define ASTRO_SPEED_M_S 1.8  // astronaut speed in m/s (arbitrarily set to 1/2 the rover speed)
#define ASTRO_ACC_M_S2 2.0   // astronaut acceleration in m/s^2 (arbitrarily set to 1/2 the rover acceleration)
#define ASTRO_TURN_DEG_PER_SEC 40.0  // astronaut can turn x degrees / sec (just a convenient number)
#define ROVER_TURN_DEG_PER_SEC 20.0  // rover can turn x degrees / sec (just a convenient number)

//
// Variables that really should be global variables.
//

static 	int refcount = 0;
static MESHHANDLE hCDREVA;
static MESHHANDLE hLMPEVA;
static MESHHANDLE hCDRLRV;
static MESHHANDLE hLMPLRV;
static MESHHANDLE hLRV;
static MESHHANDLE hLRVConsole;
static MESHHANDLE hTv;
static MESHHANDLE hEasepSWC;
static MESHHANDLE hEasepPSEP;
static MESHHANDLE hEasepLR3;


LEVA::LEVA(OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	init();
}

LEVA::~LEVA()

{
	// Nothing for now.
}

void LEVA::init()

{
	hMaster = NULL;
	GoDock1=false;
	starthover=false;
	Astro=true;		
	isCDR = false;
	MotherShip=false;
	LEMName[0]=0;
	KEY1 = false;
	KEY2 = false;
	KEY3 = false;
	KEY4 = false;
	KEY5 = false;
	KEY6 = false;
	KEY7 = false;
	KEY8 = false;
	KEY9 = false;
	KEYADD = false;
	KEYSUBTRACT = false;

	GoFlag = false;		 
	FlagPlanted = false;

	GoEquip = false;
	TvPlanted = false;
	EasepSwcPlanted = false;
	EasepPsepPlanted = false;
	EasepLr3Planted = false;

	FirstTimestep = true;
	SLEVAPlayed = false;
	StateSet = false;

	lastLatLongSet = false;
	lastLat = 0;
	lastLong = 0;

	speed = 0.0;

	ApolloNo = 0;


	// touchdown point test
	// touchdownPointHeight = -0.8;
}

void LEVA::clbkSetClassCaps (FILEHANDLE cfg)
{
	init();
	SetAstroStage();
}
		 
void LEVA::SetAstroStage ()
{
	SetEmptyMass(115);
	SetSize(2);
	SetPMI(_V(5,5,5));

	SetSurfaceFrictionCoeff(0.005, 0.5);
	SetRotDrag (_V(0, 0, 0));
	SetCW(0, 0, 0, 0);
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0); 
		
	ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	SetCameraOffset(_V(0,0.632,0.216));
	
	double ypos = -0.161;
	double zpos = 0.357;
	lrvSeat = CreateAttachment(true, _V(0, ypos, zpos), _V(0, -1, 0), _V(0, 0, 1), "SEAT", false);

	double tdph = -1.06;
	SetTouchdownPoints (_V(0, tdph, 1), _V(-1, tdph, -1), _V(1, tdph, -1));
	Astro = true;
}

void LEVA::clbkPostCreation()
{
	if (oapiGetVesselByName("TV") == NULL) { TvPlanted = false; }
	else { TvPlanted = true; }

	if (oapiGetVesselByName("Flag") == NULL) { FlagPlanted = false; }
	else { FlagPlanted = true; }

	if (oapiGetVesselByName("SWC") == NULL) { EasepSwcPlanted = false; }
	else { EasepSwcPlanted = true; }

	if (oapiGetVesselByName("PSEP") == NULL) { EasepPsepPlanted = false; }
	else { EasepPsepPlanted = true; }

	if (oapiGetVesselByName("LR3") == NULL) { EasepLr3Planted = false; }
	else { EasepLr3Planted = true; }
}

void LEVA::ToggleLRV()

{
	if (GetLRVHandle() == NULL) {
		VESSELSTATUS vs1;
		GetStatus(vs1);

		// The LM must be in landed state
		if (vs1.status != 1) return;

		OBJHANDLE hbody = GetGravityRef();
		double radius = oapiGetSize(hbody);
		vs1.vdata[0].x += 2.5 * sin(vs1.vdata[0].z) / radius;
		vs1.vdata[0].y += 2.5 * cos(vs1.vdata[0].z) / radius;

		hLRV = oapiCreateVessel("LRV", "ProjectApollo/LRV", vs1);

		LRV* lrv = (LRV*)oapiGetVesselInterface(hLRV);
		if (lrv) {
			LRVSettings lrvs;

			lrvs.MissionNo = ApolloNo;
			lrv->SetLRVStats(lrvs);
		}
	}
	else {
		// Nothing for now, only one LRV per mission.
	}
}

OBJHANDLE LEVA::GetLRVHandle()
{
	double VessCount;
	int i = 0;
	OBJHANDLE lrvHandle = NULL;

	VessCount = oapiGetVesselCount();
	for (i = 0; i < VessCount; i++)
	{
		char vesselName[256] = "";
		oapiGetObjectName(oapiGetVesselByIndex(i), vesselName, 256);
		if (strcmp("LRV", vesselName) == 0) {
			lrvHandle = oapiGetVesselByIndex(i);
		}
	}
	return lrvHandle;
}

void LEVA::BoardLRV(OBJHANDLE lrvHandle)
{
	VECTOR3 rpos = _V(0, 0, 0);
	GetRelativePos(lrvHandle, rpos);
	rpos.y = 0;
	if (length(rpos) <= 1.75) {
		LRV* lrvvessel = (LRV*)oapiGetVesselInterface(lrvHandle);
		if (isCDR == true) {
			lrvvessel->AttachChild(GetHandle(), lrvvessel->GetAttachmentHandle(false, 0), lrvSeat);
		}
		else {
			lrvvessel->AttachChild(GetHandle(), lrvvessel->GetAttachmentHandle(false, 1), lrvSeat);
		}
		oapiSetFocusObject(lrvHandle);

		SetMeshVisibilityMode(0, MESHVIS_NEVER);
		SetMeshVisibilityMode(1, MESHVIS_EXTERNAL);
	}
}

void LEVA::DeboardLRV(OBJHANDLE lrvHandle)
{
	double rt = oapiGetSize(GetSurfaceRef());
	double moon_circ = rt * 2 * PI;
	double each_deg = moon_circ / 360;
	VESSELSTATUS2 vs2;
	memset(&vs2, 0, sizeof(vs2));
	vs2.version = 2;
	vs2.status = 1;

	LRV* lrvvessel = (LRV*)oapiGetVesselInterface(lrvHandle);
	if (isCDR == true) {
		lrvvessel->DetachChild(lrvvessel->GetAttachmentHandle(false, 0), 0);
	}
	else {
		lrvvessel->DetachChild(lrvvessel->GetAttachmentHandle(false, 1), 0);
	}
	lrvvessel->GetStatusEx(&vs2);

	double zpos = -0.25;
	double xpos = -1.25;
	double d_heading = PI / 2;
	if (isCDR == false) {
		xpos *= -1;
		d_heading -= PI;
	}

	double d_lat1 = (zpos * cos(vs2.surf_hdg)) / each_deg;
	double d_lng1 = (zpos * sin(vs2.surf_hdg)) / each_deg;
	double d_lat2 = (xpos * -sin(vs2.surf_hdg)) / each_deg;
	double d_lng2 = (xpos * cos(vs2.surf_hdg)) / each_deg;
	vs2.surf_lat += ((d_lat1 + d_lat2) * RAD);
	vs2.surf_lng += ((d_lng1 + d_lng2) * RAD);

	vs2.surf_hdg += d_heading;
	if (vs2.surf_hdg > PI2) {
		vs2.surf_hdg -= PI2;
	}
	else if (vs2.surf_hdg < 0) {
		vs2.surf_hdg += PI2;
	}

	DefSetStateEx(&vs2);

	SetMeshVisibilityMode(0, MESHVIS_EXTERNAL);
	SetMeshVisibilityMode(1, MESHVIS_NEVER);
}

void LEVA::DrawName()
{
	SURFHANDLE hTex[2];
	if (isCDR == true) {
		hTex[0] = oapiGetTextureHandle(hCDREVA, 4);
		hTex[1] = oapiGetTextureHandle(hCDREVA, 5);
	}
	else {
		hTex[0] = oapiGetTextureHandle(hLMPEVA, 4);
		hTex[1] = oapiGetTextureHandle(hLMPEVA, 5);
	}

	SURFHANDLE plss_tex = oapiCreateTextureSurface(424, 64);
	SURFHANDLE rcu_tex = oapiCreateTextureSurface(48, 376);

	oapiSetSurfaceColourKey(plss_tex, 0xFF0000);
	oapiSetSurfaceColourKey(rcu_tex, 0xFF0000);

	RECT plss_src = { 150,640,362,704 };
	RECT plss_tgt = { 0,0,424,64 };
	RECT rcu_src = { 415,8,463,196 };
	RECT rcu_tgt = { 0,0,48,376 };

	if (hTex[0]) oapiBlt(plss_tex, hTex[0], &plss_tgt, &plss_src);
	if (hTex[1]) oapiBlt(rcu_tex, hTex[1], &rcu_tgt, &rcu_src);

	oapi::Font* plss_font = oapiCreateFont(58, true, "Sans", FONT_BOLD, 1800);
	oapi::Sketchpad* plss_skp = oapiGetSketchpad(plss_tex);
	if (plss_skp) {
		plss_skp->SetFont(plss_font);
		plss_skp->SetTextColor(0x000000);
		plss_skp->SetTextAlign(oapi::Sketchpad::CENTER);
		plss_skp->Text(212, 58, SuitName, strlen(SuitName));
		oapiReleaseSketchpad(plss_skp);
	}
	oapiReleaseFont(plss_font);

	oapi::Font* rcu_font = oapiCreateFont(52, true, "Sans", FONT_BOLD, 2700);
	oapi::Sketchpad* rcu_skp = oapiGetSketchpad(rcu_tex);
	if (rcu_skp) {
		rcu_skp->SetFont(rcu_font);
		rcu_skp->SetTextColor(0x000000);
		rcu_skp->SetTextAlign(oapi::Sketchpad::CENTER);
		rcu_skp->Text(52, 188, SuitName, strlen(SuitName));
		oapiReleaseSketchpad(rcu_skp);
	}
	oapiReleaseFont(rcu_font);

	if (hTex[0]) oapiBlt(hTex[0], plss_tex, &plss_src, &plss_tgt);
	if (hTex[1]) oapiBlt(hTex[1], rcu_tex, &rcu_src, &rcu_tgt);

	oapiDestroySurface(hTex[0]);
	oapiDestroySurface(hTex[1]);
	oapiDestroySurface(plss_tex);
	oapiDestroySurface(rcu_tex);
}

void LEVA::ScanMotherShip()

{
	double VessCount;
	int i=0;

	VessCount=oapiGetVesselCount();
	for ( i = 0 ; i< VessCount ; i++ ) 
	{
		char vesselName[256] = "";
		hMaster=oapiGetVesselByIndex(i);
		oapiGetObjectName(hMaster, vesselName, 256);
		if (strcmp(LEMName, vesselName) == 0) {
			MotherShip = true;
			i = int(VessCount);
		}
	}

	if (MotherShip == false) { //This is probably a legacy scenario
		char EVAName[256];
		for (i = 0; i < VessCount; i++)
		{
			hMaster = oapiGetVesselByIndex(i);
			strcpy(EVAName, GetName());
			oapiGetObjectName(hMaster, LEMName, 256);
			if (isCDR)
			{
				strcat(LEMName, "-LEVA-CDR");
			}
			else
			{
				strcat(LEMName, "-LEVA-LMP");
			}
			if (strcmp(LEMName, EVAName) == 0) {
				MotherShip = true;
				i = int(VessCount);
				oapiGetObjectName(hMaster, LEMName, 256);
			}
			else {
				strcpy(LEMName, "");
			}
		}
	}
}

void LEVA::MoveEVA(double SimDT, VESSELSTATUS *eva, double heading)
{
	TRACESETUP("MoveEVA");

	double lat;
	double lon;
	double turn_spd = Radians(SimDT * (Astro ? ASTRO_TURN_DEG_PER_SEC : ROVER_TURN_DEG_PER_SEC));
	double move_spd;

	// limit time acceleration (todo: turn limit off if no movement occurs)
	double timeW = oapiGetTimeAcceleration();
		if (timeW > 100)
			oapiSetTimeAcceleration(100);

	// turn speed is only tripled when time acc is multiplied by ten:
	turn_spd = (pow(3.0, log10(timeW))) * turn_spd / timeW;
	
	if (eva->status == 1) {
		lon = eva->vdata[0].x;
		lat = eva->vdata[0].y;
	
	} else if (lastLatLongSet) {
		lon = lastLong;
		lat = lastLat;
		eva->vdata[0].z = heading;
	
	} else return;

	if (Astro)
	{
		if (KEY1)  // turn left
		{
			eva->vdata[0].z = eva->vdata[0].z - turn_spd;
			if(eva->vdata[0].z <=-2*PI)
				eva->vdata[0].z = eva->vdata[0].z + 2*PI;
		}
		else if (KEY3)  // turn right
		{
			eva->vdata[0].z = eva->vdata[0].z + turn_spd;
			if(eva->vdata[0].z >=2*PI)
				eva->vdata[0].z = eva->vdata[0].z - 2*PI;
		}
	}

	// movement is different for astro and lrv
	if (Astro)
	{
		move_spd =  SimDT * atan2(ASTRO_SPEED_M_S, oapiGetSize(eva->rbody));  // surface speed in radians
		if (KEY2)  // backward
		{
			lat = lat - cos(heading) * move_spd;
			lon = lon - sin(heading) * move_spd;
		}
		else if (KEY4)  // step left
		{
			lat = lat + sin(heading) * move_spd;
			lon = lon - cos(heading) * move_spd;
		}
		else if (KEY6)  // step right
		{
			lat = lat - sin(heading) * move_spd;
			lon = lon + cos(heading) * move_spd;
		}
		else if (KEY8)  // forward
		{
			lat = lat + cos(heading) * move_spd;
			lon = lon + sin(heading) * move_spd;
		}
	}


	// reset all keys
	KEY1 = false;
	KEY2 = false;
	KEY3 = false;
	KEY4 = false;
	KEY5 = false;
	KEY6 = false;
	KEY7 = false;
	KEY8 = false;
	KEY9 = false;
	KEYADD = false;
	KEYSUBTRACT = false;

	// move the vessel
	eva->vdata[0].x = lon;
	eva->vdata[0].y = lat;
	eva->status = 1;
	DefSetState(eva);

	// remember last coordinates
	lastLat = lat;
	lastLong = lon;
	lastLatLongSet = true;
}

// ==============================================================
// API interface
// ==============================================================

int LEVA::clbkConsumeDirectKey(char *kstate) {

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

	if (KEYDOWN (kstate, OAPI_KEY_ADD)) {
		KEYADD = true;
		RESETKEY(kstate, OAPI_KEY_ADD);
	}

	if (KEYDOWN (kstate, OAPI_KEY_SUBTRACT)) {
		KEYSUBTRACT = true;
		RESETKEY(kstate, OAPI_KEY_SUBTRACT);
	}


	// touchdown point test
	/* 
	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		touchdownPointHeight += 0.01;
		RESETKEY(kstate, OAPI_KEY_A);

		SetTouchdownPoints (_V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
		VSSetTouchdownPoints(GetHandle(), _V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1)); 
	}

	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		touchdownPointHeight -= 0.01;
		RESETKEY(kstate, OAPI_KEY_S);

		SetTouchdownPoints (_V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
		VSSetTouchdownPoints(GetHandle(), _V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1)); 
	}
	*/

	return 0;
}

int LEVA::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

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
		if (isCDR && ((ApolloNo > 14) || (ApolloNo == 0))) {
			ToggleLRV();
			return 1;
		}
	}
		
	if (key == OAPI_KEY_F && down == true) {				
		if (Astro && isCDR && !FlagPlanted )
			GoFlag = true;	
		return 1;
	}

	if (key == OAPI_KEY_K && down == true) {
		if (Astro)
			GoEquip = true;
		return 1;
	}

	if (key == OAPI_KEY_B && down == true) {
		OBJHANDLE lrvHandle = GetLRVHandle();
		if (lrvHandle == NULL) {}
		else {
			if (GetAttachmentStatus(lrvSeat) == NULL) {
				BoardLRV(lrvHandle);
			}
			else {
				DeboardLRV(lrvHandle);
			}
		}
		return 1;
	}

	return 0;
}

void LEVA::SetFlag()

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
	
	char FName[256];

	sprintf(FName, "ProjectApollo/Apollo%d/sat5flag.cfg", ApolloNo);

	FILE *fp = fopen(FName, "rt");
	if (fp) {
		fclose(fp);
		sprintf(FName, "ProjectApollo/Apollo%d/sat5flag", ApolloNo);
		oapiCreateVessel("Flag", FName, vs1);
	}
	else {
		oapiCreateVessel("Flag","ProjectApollo/sat5flag",vs1);
	}

	FlagPlanted = true;
	GoFlag = false;

	FlagSound.play();
	FlagSound.done();
}

void LEVA::SetEquip()
{
	double distance = 1.1;

	VESSELSTATUS vs1;
	GetStatus(vs1);
	OBJHANDLE hbody = GetGravityRef();
	double radius = oapiGetSize(hbody);
	vs1.vdata[0].x += distance * sin(vs1.vdata[0].z) / radius;
	vs1.vdata[0].y += distance * cos(vs1.vdata[0].z) / radius;

	if (isCDR) {
		if (ApolloNo <= 14) {
			if (TvPlanted == false) {
				oapiCreateVessel("TV", "ProjectApollo/EasepTV", vs1);
				TvPlanted = true;
			}
		}
	}
	else {
		if (ApolloNo == 11) {
			if (EasepSwcPlanted == false) {
				oapiCreateVessel("SWC", "ProjectApollo/EasepSwc", vs1);
				EasepSwcPlanted = true;
			}
			else if (EasepPsepPlanted == false) {
				oapiCreateVessel("PSEP", "ProjectApollo/EasepPsep", vs1);
				EasepPsepPlanted = true;
			}
			else if (EasepLr3Planted == false) {
				oapiCreateVessel("LR3", "ProjectApollo/EasepLr3", vs1);
				EasepLr3Planted = true;
			}
		}
	}

	GoEquip = false;
}

void LEVA::SetMissionPath()

{
	char MissionName[24];

	_snprintf(MissionName, 23, "Apollo%d", ApolloNo);
	soundlib.SetSoundLibMissionPath(MissionName);
}

void LEVA::SetEVAStats(LEVASettings &evas)

{
	ApolloNo = evas.MissionNo;
	isCDR = evas.isCDR;
	strcpy(LEMName, evas.LEMName);
	strcpy(SuitName, evas.SuitName);
	StateSet = true;
}

void LEVA::DoFirstTimestep()

{
	//
	// Load mission-specific sounds _AFTER_ the LEM has called us to set the Apollo mission number.
	//

	if (StateSet) {
		soundlib.InitSoundLib(this, SOUND_DIRECTORY);
		SetMissionPath();

		//
		// Load sounds if they may be required.
		//

		if (!SLEVAPlayed)
			soundlib.LoadMissionSound(SLEVA, LEVA_SOUND, LEVA_SOUND);
		if (!FlagPlanted)
			soundlib.LoadMissionSound(FlagSound, FLAG_SPEECH, FLAG_SPEECH);


		VECTOR3 mesh_dir = _V(0, 0, 0);
		if (isCDR == true) {
			AddMesh(hCDREVA, &mesh_dir);
			AddMesh(hCDRLRV, &mesh_dir);
		}
		else {
			AddMesh(hLMPEVA, &mesh_dir);
			AddMesh(hLMPLRV, &mesh_dir);
		}
		if (GetAttachmentStatus(lrvSeat) == NULL) {
			SetMeshVisibilityMode(0, MESHVIS_EXTERNAL);
			SetMeshVisibilityMode(1, MESHVIS_NEVER);
		}
		else {
			SetMeshVisibilityMode(0, MESHVIS_NEVER);
			SetMeshVisibilityMode(1, MESHVIS_EXTERNAL);
		}
		DrawName();

		FirstTimestep = false;
	}
}

void LEVA::clbkPreStep (double SimT, double SimDT, double mjd)

{
	VESSELSTATUS csmV;
	VESSELSTATUS evaV;
	VECTOR3 rdist = {0,0,0};
	VECTOR3 posr  = {0,0,0};
	VECTOR3 rvel  = {0,0,0};
	VECTOR3 RelRot  = {0,0,0};
	double dist = 0.0;
	double Vel = 0.0;
	double heading;

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

	if (MotherShip == false) {
		ScanMotherShip();
	}
	
	GetStatus(evaV);
	oapiGetHeading(GetHandle(),&heading);
	//
	// if the VESSELSTATUS is not in the state "landed", force it, using stored
	// values for lat and long that are updated in MoveEVA().
	//
	if ((evaV.status != 1) && lastLatLongSet) {
		evaV.vdata[0].x = lastLong;
		evaV.vdata[0].y = lastLat;
		evaV.vdata[0].z = heading;
		evaV.status = 1;
	}
	//
	// Get reference lat and long for the VC console, as soon as we have "landed" status
	//

	if (MotherShip && hMaster){
		LEM *lmvessel = (LEM *) oapiGetVesselInterface(hMaster);					
		oapiGetRelativePos (GetHandle() ,hMaster, &posr);
		oapiGetRelativeVel (GetHandle() ,hMaster , &rvel);
		lmvessel->GetStatus(csmV);
		GlobalRot (posr, RelRot);
		dist = sqrt(posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);
		Vel = sqrt(rvel.x * rvel.x + rvel.y * rvel.y + rvel.z * rvel.z);
		if (GoDock1) {						
			if (lmvessel->IsForwardHatchOpen() && dist <= 6.00550) {
				GoDock1 = false;
				lmvessel->StopEVA(isCDR);
				oapiSetFocusObject(hMaster);
				oapiDeleteVessel(GetHandle());
				return;
			}
		}
	}

	MoveEVA(SimDT, &evaV, heading);

	if(!Astro){
		SetAstroStage();
	}

	if (GoFlag && !FlagPlanted) {
		SetFlag();
	}

	if (GoEquip) {
		SetEquip();
	}

	// touchdown point test
	// sprintf(oapiDebugString(), "touchdownPointHeight %f", touchdownPointHeight);
}

void LEVA::clbkLoadStateEx(FILEHANDLE scn, void *vs)
{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, "LEMNAME", 7)) {
			sscanf(line + 7, "%s", &LEMName);
		}
		else if (!strnicmp(line, "SUITNAME", 8)) {
			strcpy(SuitName, line + 9);
		}
		else if (!strnicmp(line, "STATE", 5)) {
			int	s;
			sscanf(line + 5, "%d", &s);
			SetMainState(s);

			if (!Astro) {
				Astro = true;
				SetEngineLevel(ENGINE_HOVER, 1);
			}
		}
		else if (!strnicmp(line, "MISSIONNO", 9)) {
			sscanf(line + 9, "%d", &ApolloNo);
		}
		else {
			ParseScenarioLineEx(line, vs);
		}
    }
}


	
	
void LEVA::clbkVisualCreated (VISHANDLE vis, int refcount)
{
}

void LEVA::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
}

typedef union {
	struct {
		unsigned int FlagPlanted:1;
		unsigned int StateSet:1;
		unsigned int SLEVAPlayed:1;
		unsigned int Astro:1;
		unsigned int isCDR:1;
	} u;
	unsigned int word;
} MainLEVAState;

int LEVA::GetMainState()

{
	MainLEVAState s;

	s.word = 0;
	s.u.FlagPlanted = FlagPlanted;
	s.u.StateSet = StateSet;
	s.u.SLEVAPlayed = SLEVAPlayed;
	s.u.Astro = Astro;
	s.u.isCDR = isCDR;

	return s.word;
}

void LEVA::SetMainState(int n)

{
	MainLEVAState s;

	s.word = n;
	FlagPlanted = (s.u.FlagPlanted != 0);
	StateSet = (s.u.StateSet != 0);
	SLEVAPlayed = (s.u.SLEVAPlayed != 0);
	Astro = (s.u.Astro != 0);
	isCDR = (s.u.isCDR != 0);
}

void LEVA::clbkSaveState(FILEHANDLE scn)
{
	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_string(scn, "LEMNAME", LEMName);

	oapiWriteScenario_string(scn, "SUITNAME", SuitName);

	int s = GetMainState();
	if (s) {
		oapiWriteScenario_int (scn, "STATE", s);
	}

	if (ApolloNo != 0) {
		oapiWriteScenario_int(scn, "MISSIONNO", ApolloNo);
	}
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hCDREVA = oapiLoadMeshGlobal("ProjectApollo/sat5AstroS_CDR");
		hLMPEVA = oapiLoadMeshGlobal("ProjectApollo/sat5AstroS_LMP");
		hCDRLRV = oapiLoadMeshGlobal("ProjectApollo/LRV_Astro_CDR");
		hLMPLRV = oapiLoadMeshGlobal("ProjectApollo/LRV_Astro_LMP");
		hLRV = oapiLoadMeshGlobal ("ProjectApollo/LRV");
		hLRVConsole = oapiLoadMeshGlobal ("ProjectApollo/LRV_console");
		hTv = oapiLoadMeshGlobal("ProjectApollo/EASEP_TV");
		hEasepSWC = oapiLoadMeshGlobal("ProjectApollo/EASEP_SWC");
		hEasepPSEP = oapiLoadMeshGlobal("ProjectApollo/EASEP_PSEP");
		hEasepLR3 = oapiLoadMeshGlobal("ProjectApollo/EASEP_LR3");
	}

	return new LEVA (hvessel, flightmodel);
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	LEVA *sv = (LEVA *) vessel;

	if (sv)
		delete sv;
}

DLLCLBK void InitModule (HINSTANCE hModule)
{

}
