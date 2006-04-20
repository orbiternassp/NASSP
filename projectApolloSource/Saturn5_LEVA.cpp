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
  *	Revision 1.9  2006/04/17 14:23:23  redburne
  *	First working version of new 3d LRV console
  *	
  *	Revision 1.8  2006/01/22 18:08:38  redburne
  *	- timestep processing moved to VESSEL2 method clbkPreStep()
  *	- semi-realistic movement and turn speed values (as #defines)
  *	- turn speed now affected by time acceleration (but only tripled when acc is multiplied by 10)
  *	- LRV mesh visible from cockpit view (camera at driver head)
  *	
  *	Revision 1.7  2006/01/19 14:58:33  tschachim
  *	Initial Meshland support.
  *	
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
#include "OrbiterMath.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"
#include "sat5_lmpkd.h"

#include "saturn5_leva.h"
#include "lrv_console.h"
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
static MESHHANDLE hCMPEVA;
static MESHHANDLE hLRV;
static MESHHANDLE hLRVConsole;

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

	speed = 0.0;

	ApolloNo = 0;
	Realism = 0;

	// LRV Console
	vccCompAngle = 0.0; // North
	vccBear001Angle = 0.0;
	vccBear010Angle = 0.0;
	vccBear100Angle = 0.0;
	vccDist001Angle = 0.0;
	vccDist010Angle = 0.0;
	vccDist100Angle = 0.0;
	vccRange001Angle = 0.0;
	vccRange010Angle = 0.0;
	vccRange100Angle = 0.0;
	vccSpeedAngle = 0.0;
	vccInitialized = false;
	vccInitLat = 0.0;
	vccInitLong = 0.0;
	vccDistance = 0.0;

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
	VECTOR3 mesh_adjust = _V(0.0, 0.15, 0.0);
	SetMeshVisibilityMode(AddMesh(hLRV, &mesh_adjust), MESHVIS_ALWAYS); 
	vccMeshIdx = AddMesh(hLRVConsole, &mesh_adjust);
	SetMeshVisibilityMode(vccMeshIdx, MESHVIS_ALWAYS);
	SetCameraOffset(_V(0.36, 0.54, -0.55));  // roughly at the driver's head

	//////////////////////////////////////////////////////////////////////////
	// With vccMeshIdx, we now have all data to initialize the LRV console
	// transformation matrices:
	//////////////////////////////////////////////////////////////////////////
    // Compass rose rotation
	mgtRotCompass.P.rotparam.ref = LRV_COMPASS_PIVOT;
	mgtRotCompass.P.rotparam.axis = Normalize(LRV_COMPASS_AXIS);
	mgtRotCompass.P.rotparam.angle = 0.0;  // dummy value
	mgtRotCompass.nmesh = vccMeshIdx;
	mgtRotCompass.ngrp = GEOM_COMPASS;
	mgtRotCompass.transform = MESHGROUP_TRANSFORM::ROTATE;
    // Bearing, distance or range drum (shared for all drums)
	mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_UPPER;  // dummy value
	mgtRotDrums.P.rotparam.axis = LRV_DRUM_AXIS;
	mgtRotDrums.P.rotparam.angle = 0.0;  // dummy value
	mgtRotDrums.nmesh = vccMeshIdx;
	mgtRotDrums.ngrp = GEOM_DRUM_BEAR_001;  // dummy value
	mgtRotDrums.transform = MESHGROUP_TRANSFORM::ROTATE;
    // Speed dial rotation
	mgtRotSpeed.P.rotparam.ref = LRV_SPEED_PIVOT;
	mgtRotSpeed.P.rotparam.axis = Normalize(LRV_SPEED_AXIS);
	mgtRotSpeed.P.rotparam.angle = 0.0;  // dummy value
	mgtRotSpeed.nmesh = vccMeshIdx;
	mgtRotSpeed.ngrp = GEOM_SPEEDDIAL;
	mgtRotSpeed.transform = MESHGROUP_TRANSFORM::ROTATE;

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

void Saturn5_LEVA::MoveEVA(double SimDT, VESSELSTATUS *eva, double heading)
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

	// turning is identical for both LRV and astronaut
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
	else  // LRV
	{
		if (KEY2)  // decelerate
		{
			speed = speed - SimDT * ROVER_ACC_M_S2; 
			if (speed < -ROVER_SPEED_M_S)
				speed = -ROVER_SPEED_M_S;
		}
		else if (KEY5)  // stop
		{
		    speed = 0.0;
		}
		else if (KEY8)  // accelerate
		{
			speed = speed + SimDT * ROVER_ACC_M_S2; 
			if (speed > ROVER_SPEED_M_S)
				speed = ROVER_SPEED_M_S;
		}

		// move forward / backward according to current speed
		move_spd =  SimDT * atan2(speed, oapiGetSize(eva->rbody));  // surface speed in radians
		lat = lat + cos(heading) * move_spd;
		lon = lon + sin(heading) * move_spd;
		vccDistance = vccDistance + SimDT * fabs(speed);  // the console distance always counts up!
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

void Saturn5_LEVA::clbkPreStep (double SimT, double SimDT, double mjd)

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

	if (!MotherShip)
		ScanMotherShip();
	
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
	if (!vccInitialized && (evaV.status == 1)) {
		vccInitLat = evaV.vdata[0].y;
		vccInitLong = evaV.vdata[0].x;
		vccInitialized = true;
	}

	if (hMaster){
		sat5_lmpkd *lmvessel = (sat5_lmpkd *) oapiGetVesselInterface(hMaster);					
		oapiGetRelativePos (GetHandle() ,hMaster, &posr);
		oapiGetRelativeVel (GetHandle() ,hMaster , &rvel);
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

	//
	// update the VC console
	//
	if (!Astro && vccVis) {
		// rotate the VC compass rose
		mgtRotCompass.P.rotparam.angle = float(heading - vccCompAngle);
		vccCompAngle = heading;
		MeshgroupTransform(vccVis, mgtRotCompass);

		// Rotate the speed dial.
		double abs_speed_kmh = fabs(speed * 3.6);  // absolute speed in km/h
		double speed_dial_rad = -(abs_speed_kmh - 10.0) * Radians(31.25) / 10.0;
		mgtRotSpeed.P.rotparam.angle = float(speed_dial_rad - vccSpeedAngle);
		vccSpeedAngle = speed_dial_rad;
		MeshgroupTransform(vccVis, mgtRotSpeed);

		// Display distance travelled (in km) since last initialization
		double distance = vccDistance / 1000.0;
		while (distance > 100.0) distance = distance - 100.0;
		double digit = floor(distance/10.0);  // tens
		double drum_rot = (digit * PI / 5.0) - vccDist100Angle;
		vccDist100Angle = digit * PI / 5.0;
		mgtRotDrums.P.rotparam.angle = float(drum_rot);
		mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_LOWER;
		mgtRotDrums.ngrp = GEOM_DRUM_DIST_10_0;
		MeshgroupTransform(vccVis, mgtRotDrums);
		distance = distance - 10.0 * digit;
		digit = floor(distance);  // ones
		drum_rot = (digit * PI / 5.0) - vccDist010Angle;
		vccDist010Angle = digit * PI / 5.0;
		mgtRotDrums.P.rotparam.angle = float(drum_rot);
		mgtRotDrums.ngrp = GEOM_DRUM_DIST_01_0;
		MeshgroupTransform(vccVis, mgtRotDrums);
		distance = distance - digit;
		digit = floor(10.0 * distance);  // tenths
		drum_rot = (digit * PI / 5.0) - vccDist001Angle;
		vccDist001Angle = digit * PI / 5.0;
		mgtRotDrums.P.rotparam.angle = float(drum_rot);
		mgtRotDrums.ngrp = GEOM_DRUM_DIST_00_1;
		MeshgroupTransform(vccVis, mgtRotDrums);

		if (vccInitialized) {  // we need the reference lat and long for this ...
			// Display bearing to last reference point (usually the LM)
			double bearing = Degree(CalcSphericalBearing(_V(vccInitLong, vccInitLat, 0.0), evaV.vdata[0]));
			bearing = bearing - 90.0;  // correct bearing to local North
			while (bearing < 0.0) bearing += 360.0;
			digit = floor(bearing/100.0);  // hundreds
			drum_rot = (digit * PI / 5.0) - vccBear100Angle;
			vccBear100Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_UPPER;
			mgtRotDrums.ngrp = GEOM_DRUM_BEAR_100;
			MeshgroupTransform(vccVis, mgtRotDrums);
			bearing = bearing - 100.0 * digit;
			digit = floor(bearing/10.0);  // tens
			drum_rot = (digit * PI / 5.0) - vccBear010Angle;
			vccBear010Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_BEAR_010;
			MeshgroupTransform(vccVis, mgtRotDrums);
			bearing = bearing - 10.0 * digit;
			digit = floor(bearing);  // ones
			drum_rot = (digit * PI / 5.0) - vccBear001Angle;
			vccBear001Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_BEAR_001;
			MeshgroupTransform(vccVis, mgtRotDrums);

			// Display range (in km) to last reference point (usually the LM)
			double range = CalcSphericalDistance(_V(vccInitLong, vccInitLat, 0.0), evaV.vdata[0], oapiGetSize(evaV.rbody))/1000.0;
			if (range < 0.0) range = -range;
			while (range > 100.0) range = range - 100.0;
			digit = floor(range/10.0);  // tens
			drum_rot = (digit * PI / 5.0) - vccRange100Angle;
			vccRange100Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_LOWER;
			mgtRotDrums.ngrp = GEOM_DRUM_RNGE_10_0;
			MeshgroupTransform(vccVis, mgtRotDrums);
			range = range - 10.0 * digit;
			digit = floor(range);  // ones
			drum_rot = (digit * PI / 5.0) - vccRange010Angle;
			vccRange010Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_RNGE_01_0;
			MeshgroupTransform(vccVis, mgtRotDrums);
			range = range - digit;
			digit = floor(10.0 * range);  // tenths
			drum_rot = (digit * PI / 5.0) - vccRange001Angle;
			vccRange001Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_RNGE_00_1;
			MeshgroupTransform(vccVis, mgtRotDrums);
		}
	}


	MoveEVA(SimDT, &evaV, heading);

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


	
	
void Saturn5_LEVA::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	vccVis = vis;
}

void Saturn5_LEVA::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	vccVis = NULL;
	// reset the variables keeping track of console mesh animation
	vccCompAngle = 0.0;
	vccDist100Angle = 0.0;
	vccDist010Angle = 0.0;
	vccDist001Angle = 0.0;
	vccBear100Angle = 0.0;
	vccBear010Angle = 0.0;
	vccBear001Angle = 0.0;
	vccRange100Angle = 0.0;
	vccRange010Angle = 0.0;
	vccRange010Angle = 0.0;
	vccSpeedAngle = 0.0;
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
		hLRVConsole = oapiLoadMeshGlobal ("ProjectApollo/LRV_console");
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
