/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Lunar Rover

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
  *	Revision 1.2  2006/05/06 06:00:35  jasonims
  *	No more venting our Astronauts into space...and no more LRV popping out of an Astronauts pocket....well sorta.
  *	
  *	Revision 1.1  2006/05/05 21:30:06  movieman523
  *	Added beginnings of LRV code.
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

#include "lrv.h"
#include "lrv_console.h"
#include "tracer.h"

#include "CollisionSDK/CollisionSDK.h"

//
// Set the file name for the tracer code.
//

char trace_file[] = "ProjectApollo LRV.log";

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
#define ROVER_TURN_DEG_PER_SEC 20.0  // rover can turn x degrees / sec (just a convenient number)

//
// Variables that really should be global variables.
//

static 	int refcount = 0;
static MESHHANDLE hLRV;
static MESHHANDLE hLRVConsole;

LRV::LRV(OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	hMaster = hObj;
	init();
}

LRV::~LRV()

{
	// Nothing for now.
}

void LRV::init()

{
	//GoDock1=false;
	starthover=false;
	GoRover =true;
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
	KEYADD = false;
	KEYSUBTRACT = false;

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

void LRV::clbkSetClassCaps (FILEHANDLE cfg)
{
	init();
	VSRegVessel(GetHandle());
	SetRoverStage();
}
		 
void LRV::SetRoverStage ()
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
	// ???: The next two lines have been exchanged to make the console animations work
	//      with the buggy Orbiter 2006 MeshTransform(). This is a temporary fix which,
	//      as a side effect, will cause problems with all animations of the main LRV
	//      mesh (as soon as they are added ...).
	vccMeshIdx = AddMesh(hLRVConsole, &mesh_adjust);
	SetMeshVisibilityMode(AddMesh(hLRV, &mesh_adjust), MESHVIS_ALWAYS); 
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

}

void LRV::ScanMotherShip()

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
		strcat(CSMName,"-LRV");
		if (strcmp(CSMName, EVAName)==0){
			MotherShip=true;
			i=int(VessCount);
		}
		else{
			strcpy(CSMName,"");
		}
	}
}

void LRV::MoveLRV(double SimDT, VESSELSTATUS *eva, double heading)
{
	TRACESETUP("MoveLRV");

	double lat;
	double lon;
	double turn_spd = Radians(SimDT * ROVER_TURN_DEG_PER_SEC);
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

	if (speed != 0.0) //LRV
	{
		if (KEY1)  // turn left
		{
			eva->vdata[0].z = eva->vdata[0].z - (turn_spd*(speed/ROVER_SPEED_M_S));
			if(eva->vdata[0].z <=-2*PI)
				eva->vdata[0].z = eva->vdata[0].z + 2*PI;
		}
		else if (KEY3)  // turn right
		{
			eva->vdata[0].z = eva->vdata[0].z + (turn_spd*(speed/ROVER_SPEED_M_S));
			if(eva->vdata[0].z >=2*PI)
				eva->vdata[0].z = eva->vdata[0].z - 2*PI;
		}
	}


	// movement is different for astro and lrv

	if (GoRover) // LRV
	{
		if (KEYSUBTRACT)  // decelerate
		{
			speed = speed - SimDT * ROVER_ACC_M_S2; 
			if (speed < -ROVER_SPEED_M_S)
				speed = -ROVER_SPEED_M_S;
		}
		else if (KEY5)  // stop
		{
		    speed = 0.0;
		}
		else if (KEYADD)  // accelerate
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

int LRV::clbkConsumeDirectKey(char *kstate) {

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

int LRV::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	TRACESETUP("clbkConsumeBufferedKey");

	if (FirstTimestep) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0; 
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
		
	return 0;
}

void LRV::SetMissionPath()

{
	char MissionName[24];

	_snprintf(MissionName, 23, "Apollo%d", ApolloNo);
	soundlib.SetSoundLibMissionPath(MissionName);
}

void LRV::SetLRVStats(LRVSettings &lrvs)

{
	ApolloNo = lrvs.MissionNo;
	Realism = lrvs.Realism;
	StateSet = true;
}

void LRV::DoFirstTimestep()

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

void LRV::clbkPreStep (double SimT, double SimDT, double mjd)

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
	// values for lat and long that are updated in MoveLRV().
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
		/*if (GoDock1) {						
			if (dist <= 6.00550) {
				GoDock1 = false;
				lmvessel->StopEVA();
				oapiSetFocusObject(hMaster);
				oapiDeleteVessel(GetHandle());
				return;
			}
		}*/
	}

	//
	// update the VC console
	//
	if (vccVis) {
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


	MoveLRV(SimDT, &evaV, heading);

	if (GoRover){
		SetRoverStage();
	}

	// touchdown point test
	// sprintf(oapiDebugString(), "touchdownPointHeight %f", touchdownPointHeight);
}

DLLCLBK void ovcLoadState (VESSEL *vessel, FILEHANDLE scn, VESSELSTATUS *vs)

{
	LRV *sv = (LRV *)vessel;

	sv->LoadState(scn, vs);
}

void LRV::LoadState(FILEHANDLE scn, VESSELSTATUS *vs)

{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "STATE", 5)) {
			int	s;
			sscanf(line + 5, "%d", &s);
			SetMainState(s);

			GoRover = true;
			SetEngineLevel(ENGINE_HOVER,1);
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


	
	
void LRV::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	vccVis = vis;
}

void LRV::clbkVisualDestroyed (VISHANDLE vis, int refcount)
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
	LRV *sv = (LRV *)vessel;
	sv->SaveState(scn);
}

typedef union {
	struct {
		unsigned int StateSet:1;
		unsigned int SLEVAPlayed:1;
	} u;
	unsigned int word;
} MainLEVAState;

int LRV::GetMainState()

{
	MainLEVAState s;

	s.word = 0;
	s.u.StateSet = StateSet;
	s.u.SLEVAPlayed = SLEVAPlayed;

	return s.word;
}

void LRV::SetMainState(int n)

{
	MainLEVAState s;

	s.word = n;
	StateSet = (s.u.StateSet != 0);
	SLEVAPlayed = (s.u.SLEVAPlayed != 0);
}

void LRV::SaveState(FILEHANDLE scn)

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
		hLRV = oapiLoadMeshGlobal ("ProjectApollo/LRV");
		hLRVConsole = oapiLoadMeshGlobal ("ProjectApollo/LRV_console");
	}

	return new LRV (hvessel, flightmodel);
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	LRV *sv = (LRV *) vessel;

	if (sv)
		delete sv;
}

DLLCLBK void InitModule (HINSTANCE hModule)
{
	InitCollisionSDK();
}
