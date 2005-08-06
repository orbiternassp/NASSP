/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn V Module Parked/Docked mode

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
  *	Revision 1.20  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.19  2005/08/06 00:03:48  movieman523
  *	Beginnings of support for AGC I/O channels in LEM.
  *	
  *	Revision 1.18  2005/08/05 23:37:21  movieman523
  *	Added AGC I/O channel simulation to make integrating Virtual AGC easier.
  *	
  *	Revision 1.17  2005/08/03 10:44:33  spacex15
  *	improved audio landing synchro
  *	
  *	Revision 1.16  2005/08/01 21:52:06  lazyd
  *	Added code for AbortStage
  *	
  *	Revision 1.15  2005/07/16 20:44:23  lazyd
  *	Added call to start P71 when the descent stage runs out of fuel
  *	
  *	Revision 1.14  2005/07/14 10:06:14  spacex15
  *	Added full apollo11 landing sound
  *	initial release
  *	
  *	Revision 1.13  2005/07/07 23:47:12  movieman523
  *	Fixed bug when saving audio language to scenario file.
  *	
  *	Revision 1.12  2005/07/06 14:35:03  lazyd
  *	Rewrote key-handler to get key events
  *	
  *	Revision 1.11  2005/06/17 18:32:52  lazyd
  *	Tried to fix generic cockpit view, doesn't work
  *	
  *	Revision 1.10  2005/06/16 13:33:03  lazyd
  *	Fixed mistake on right redesignation
  *	
  *	Revision 1.9  2005/06/16 13:13:10  lazyd
  *	Speed up response to ROD and redesignation key hits
  *	
  *	Revision 1.8  2005/06/15 20:33:51  lazyd
  *	Added code to initialize InFOV
  *	
  *	Revision 1.7  2005/06/13 18:48:44  lazyd
  *	Added code to catch key hits for autoland P64 and P66
  *	
  *	Revision 1.6  2005/06/09 14:18:23  lazyd
  *	Added code for function SetGimbal which sets the value of GMBLswitch
  *	
  *	Revision 1.5  2005/05/27 15:43:08  tschachim
  *	Fixed bug: virtual engines are always on
  *	
  *	Revision 1.4  2005/05/21 16:14:36  movieman523
  *	Pass Realism and AudioLanguage correctly from CSM to LEM.
  *	
  *	Revision 1.3  2005/04/11 23:43:21  yogenfrutz
  *	Added LEM Panel_ID
  *	
  *	Revision 1.2  2005/02/24 00:27:56  movieman523
  *	Revised to make LEVA sounds work with Orbitersound 3.
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

#include "tracer.h"


char trace_file[] = "NASP-trace.txt";


// ==============================================================
// Global parameters
// ==============================================================

static int refcount;

const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};

const int TO_EVA=1;


// Modif x15 to manage landing sound
static SoundEvent sevent        ;
static double NextEventTime = 0.0;


//Begin code

LanderVessel::LanderVessel(OBJHANDLE hObj, int fmodel) : VESSEL (hObj, fmodel)

{
	// Nothing special to do.
}

sat5_lmpkd::sat5_lmpkd(OBJHANDLE hObj, int fmodel) : VESSEL (hObj, fmodel), dsky(soundlib, agc), agc(soundlib, dsky)

{
	// VESSELSOUND **********************************************************************
	// initialisation

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	Init();
}

sat5_lmpkd::~sat5_lmpkd()

{

    sevent.Stop();
	sevent.Done();
}

void sat5_lmpkd::Init()

{
	//bAbort =false;
	RCS_Full=true;
	Eds=true;
	
	toggleRCS =false;
	InFOV=true;

	InitPanel();

	ABORT_IND=false;

	high=false;
	bToggleHatch=false;
	bModeDocked=false;
	bModeHover=false;
	HatchOpen=false;
	bManualSeparate=false;
	ToggleEva=false;
	EVA_IP=false;
	refcount = 0;
	lmpview=true;
	cdrview=true;
	startimer=false;
	ContactOK = false;
	stage = 0;
	status = 0;

	InVC = false;
	Crewed = true;
	AutoSlow = false;

	MissionTime = 0;
	FirstTimestep = true;

	SwitchFocusToLeva = 0;

	agc.ControlVessel(this);
	dsky.Init();

	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);

	ph_Dsc = 0;
	ph_Asc = 0;
	ph_rcslm0 = 0;
	ph_rcslm1 = 0;

	Realism = REALISM_DEFAULT;
	ApolloNo = 0;

	strncpy(AudioLanguage, "English", 64);
	soundlib.SetLanguage(AudioLanguage);

	SoundsLoaded = false;

	SetLmVesselDockStage();

	//
	// Default channel setup.
	//

	agc.SetInputChannelBit(030, 2, true);	// Descent stage attached.
	agc.SetInputChannelBit(030, 15, true);	// Temperature in limits.
}

void sat5_lmpkd::DoFirstTimestep()

{
	if (!SoundsLoaded) {
		LoadDefaultSounds();
	}
	NextEventTime = 0.0;

	if (CabinFansActive()) {
		CabinFans.play(LOOP,255);
	}

	char VName10[256]="";

	strcpy (VName10, GetName()); strcat (VName10, "-LEVA");
	hLEVA=oapiGetVesselByName(VName10);
}

void sat5_lmpkd::LoadDefaultSounds()

{
    char buffers[80];

	//
	// load sounds now that the audio language has been set up.
	//

	soundlib.LoadMissionSound(LunarAscent, LUNARASCENT_SOUND, LUNARASCENT_SOUND);
	soundlib.LoadSound(StageS, "Stagesep.wav");
	soundlib.LoadMissionSound(Scontact, LUNARCONTACT_SOUND, LUNARCONTACT_SOUND);
	soundlib.LoadSound(Sclick, CLICK_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Bclick, "button.wav", INTERNAL_ONLY);
	soundlib.LoadSound(Gclick, "guard.wav", INTERNAL_ONLY);
	soundlib.LoadSound(CabinFans, "cabin.wav", INTERNAL_ONLY);
	soundlib.LoadSound(Vox, "vox.wav");
	soundlib.LoadSound(Afire, "des_abort.wav");

// MODIF X15 manage landing sound

	sprintf(buffers, "Apollo%d", agc.GetApolloNo());
// TRACE (buffers);
    soundlib.SetSoundLibMissionPath(buffers);
    sevent.LoadMissionLandingSoundArray(soundlib,"sound.csv");
    sevent.InitDirectSound(soundlib);

	SoundsLoaded = true;
}

void sat5_lmpkd::AttitudeLaunch1()
{
	//Original code function by Richard Craig From MErcury Sample by Rob CONLEY
	// Modification for NASSP specific needs by JL Rocca-Serra
	VECTOR3 ang_vel;
	GetAngularVel(ang_vel);// gets current angular velocity for stabilizer and rate control
// variables to store each component deflection vector	
	VECTOR3 rollvectorl={0.0,0.0,0.0};
	VECTOR3 rollvectorr={0.0,0.0,0.0};
	VECTOR3 pitchvector={0.0,0.0,0.0};
	VECTOR3 yawvector={0.0,0.0,0.0};
	VECTOR3 yawvectorm={0.0,0.0,0.0};
	VECTOR3 pitchvectorm={0.0,0.0,0.0};
//************************************************************
// variables to store Manual control levels for each axis
	double tempP = 0.0;
	double tempY = 0.0;
	double tempR = 0.0; 
//************************************************************
// Variables to store correction factors for rate control
	double rollcorrect = 0.0;
	double yawcorrect= 0.0;
	double pitchcorrect = 0.0;
//************************************************************
// gets manual control levels in each axis, this code copied directly from Rob Conley's Mercury Atlas	
	if (GMBLswitch){
		tempP = GetManualControlLevel(THGROUP_ATT_PITCHDOWN, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_PITCHUP, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
	}
	if (GMBLswitch){
		tempR = GetManualControlLevel(THGROUP_ATT_BANKLEFT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE) - GetManualControlLevel(THGROUP_ATT_BANKRIGHT, MANCTRL_ANYDEVICE, MANCTRL_ANYMODE);
	}
	
	
	//sprintf (oapiDebugString(), "roll input: %f, roll vel: %f,pitch input: %f, pitch vel: %f", tempR, ang_vel.z,tempP, ang_vel.x);
	
//*************************************************************
//Creates correction factors for rate control in each axis as a function of input level
// and current angular velocity. Varies from 1 to 0 as angular velocity approaches command level
// multiplied by maximum rate desired
	if(tempR != 0.0)	{
		rollcorrect = (1/(fabs(tempR)*0.175))*((fabs(tempR)*0.175)-fabs(ang_vel.z));
			if((tempR > 0 && ang_vel.z > 0) || (tempR < 0 && ang_vel.z < 0))	{
						rollcorrect = 1;
					}
	}
	if(tempP != 0.0)	{
		pitchcorrect = (1/(fabs(tempP)*0.275))*((fabs(tempP)*0.275)-fabs(ang_vel.x));
		if((tempP > 0 && ang_vel.x > 0) || (tempP < 0 && ang_vel.x < 0))	{
						pitchcorrect = 1;
					}
	}
	
//*************************************************************	
// Create deflection vectors in each axis
	pitchvector = _V(0.0,0.0,0.05*tempP*pitchcorrect);
	pitchvectorm = _V(0.0,0.0,-0.2*tempP*pitchcorrect);
	yawvector = _V(0.05*tempY*yawcorrect,0.0,0.0);
	yawvectorm = _V(0.05*tempY*yawcorrect,0.0,0.0);
	rollvectorl = _V(0.0,0.60*tempR*rollcorrect,0.0);
	rollvectorr = _V(0.60*tempR*rollcorrect,0.0,0.0);

//*************************************************************
// create opposite vectors for "gyro stabilization" if command levels are 0
	if(tempP==0.0 && GMBLswitch) {
		pitchvectorm=_V(0.0,0.0,-0.8*ang_vel.x*3);
	}
	if(tempR==0.0 && GMBLswitch) {
		
		rollvectorr=_V(0.8*ang_vel.z*3,0.0,0.0);
	}
	
//**************************************************************	
// Sets thrust vectors by simply adding up all the axis deflection vectors and the 
// "neutral" default vector
	SetThrusterDir(th_hover[0],pitchvectorm+rollvectorr+_V( 0,1,0));//4
	SetThrusterDir(th_hover[1],pitchvectorm+rollvectorr+_V( 0,1,0));

//	sprintf (oapiDebugString(), "pitch vector: %f, roll vel: %f", tempP, ang_vel.z);

}

// ==============================================================
// API interface
// ==============================================================
// ==============================================================
// DLL entry point
// ==============================================================

BOOL WINAPI DllMain (HINSTANCE hModule,
					 DWORD ul_reason_for_call,
					 LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		InitGParam(hModule);
		break;
	case DLL_PROCESS_DETACH:
		FreeGParam();
		break;
	}
	return TRUE;
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	sat5_lmpkd *lem;

	if (!refcount++) {
		LEMLoadMeshes();
	}
	
	// VESSELSOUND 

	lem = new sat5_lmpkd(hvessel, flightmodel);
	return (VESSEL *) lem;
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	TRACESETUP("ovcExit LMPARKED");

	--refcount;

	if (!refcount) {
		TRACE("refcount == 0");

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) delete (sat5_lmpkd *)vessel;
}


// rewrote to get key events rather than monitor key state - LazyD
int sat5_lmpkd::ConsumeDirectKey (DWORD key)
{
	switch (key) {

	case OAPI_KEY_K:
		bToggleHatch = true;
		return 1;

	case OAPI_KEY_6:
		cdrview = true;
		return 1;

	//
	// Used by P64
	//


	case OAPI_KEY_COMMA:
		// move landing site left
		agc.RedesignateTarget(1,1.0);
		return 1;

	case OAPI_KEY_PERIOD:
		// move landing site right
		agc.RedesignateTarget(1,-1.0);
		return 1;

	case OAPI_KEY_HOME:
		//move the landing site downrange
		agc.RedesignateTarget(0,-1.0);
		return 1;

	case OAPI_KEY_END:
		//move the landing site closer
		agc.RedesignateTarget(0,1.0);
		return 1;

	//
	// Used by P66
	//

	case OAPI_KEY_MINUS:
		//increase descent rate
		agc.ChangeDescentRate(-0.3077);
		return 1;

	case OAPI_KEY_EQUALS:
		//decrease descent rate
		agc.ChangeDescentRate(0.3077);
		return 1;
	}
	return 0;

}

//
// Timestep code.
//

void sat5_lmpkd::PostStep(double simt, double simdt, double mjd)

{
	if (FirstTimestep)
	{
		DoFirstTimestep();
		FirstTimestep = false;
		return;
	}

	//
	// If we switch focus to the astronaut immediately after creation, Orbitersound doesn't
	// play any sounds, or plays LEM sounds rather than astronauts sounds. We need to delay
	// the focus switch a few timesteps to allow it to initialise properly in the background.
	//

	if (SwitchFocusToLeva > 0 && hLEVA) {
		SwitchFocusToLeva--;
		if (!SwitchFocusToLeva) {
			oapiSetFocusObject(hLEVA);
		}
	}

	SetView();
	
	VECTOR3 RVEL = _V(0.0,0.0,0.0);
	GetRelativeVel(GetGravityRef(),RVEL);

	MissionTime = MissionTime + oapiGetSimStep();

	agc.Timestep(MissionTime);
	dsky.Timestep(MissionTime);
	
	actualVEL = (sqrt(RVEL.x *RVEL.x + RVEL.y * RVEL.y + RVEL.z * RVEL.z)/1000*3600);
	actualALT = GetAltitude() ;
	if (actualALT < 1470){
		actualVEL = actualVEL-1470+actualALT;
	}
	if (GroundContact()){
	actualVEL =0;
	}
	if (status !=0 && Sswitch2){
				bManualSeparate = true;
	}
	actualFUEL = GetFuelMass()/GetMaxFuelMass()*100;	
	double dTime,aSpeed,DV,aALT,DVV,DVA;//aVAcc;aHAcc;ALTN1;SPEEDN1;aTime aVSpeed;
		aSpeed = actualVEL/3600*1000;
		aALT=actualALT;
		dTime=simt-aTime;
		if(dTime > 0.1){
			DV= aSpeed - SPEEDN1;
			aHAcc= (DV / dTime);
			DVV = aALT - ALTN1;
			aVSpeed = DVV / dTime;
			DVA = aVSpeed- VSPEEDN1;
			

			aVAcc=(DVA/ dTime);
			aTime = simt;
			VSPEEDN1 = aVSpeed;
			ALTN1 = aALT;
			SPEEDN1= aSpeed;
		}
	AttitudeLaunch1();
	if( toggleRCS){
			if(P44switch){
			SetAttitudeMode(2);
			toggleRCS =false;
			}
			else if (!P44switch){
			SetAttitudeMode(1);
			toggleRCS =false;
			}
		}
		if (GetAttitudeMode()==1){
		P44switch=false;
		}
		else if (GetAttitudeMode()==2 ){
		P44switch=true;
		}
	if(Sswitch1){
		Sswitch1=false;
		Undock(0);
		}
	if (GetNavmodeState(NAVMODE_KILLROT)&& !ATT2switch && !ATT3switch){
		if (GetThrusterLevel(th_att_rot[10]) <0.00001 && GetThrusterLevel(th_att_rot[18]) <0.00001 ){
			DeactivateNavmode(NAVMODE_KILLROT);
		}
	}
	if (stage == 0)	{
		if (ENGARMswitch && !DESHE1switch && !DESHE2switch && ED1switch && ED2switch && ED5switch){
			SetThrusterResource(th_hover[0], ph_Dsc);
			SetThrusterResource(th_hover[1], ph_Dsc);
			agc.SetInputChannelBit(030, 3, true);
		} else {
			SetThrusterResource(th_hover[0], NULL);
			SetThrusterResource(th_hover[1], NULL);
			agc.SetInputChannelBit(030, 3, false);
		}
		
		if(!AFEED1switch && !AFEED2switch && !AFEED3switch && !AFEED4switch){
			SetRCS(ph_rcslm0);
		} else {
			SetRCS(ph_rcslm1);
		}
	}else if (stage == 1 || stage == 5)	{
		if(!AFEED1switch && !AFEED2switch && !AFEED3switch && !AFEED4switch){
			SetRCS(ph_rcslm0);
		}
		else{
			SetRCS(ph_rcslm1);
		}
		if (ENGARMswitch && !DESHE1switch && !DESHE2switch && ED1switch && ED2switch && ED5switch){
			SetThrusterResource(th_hover[0], ph_Dsc);
			SetThrusterResource(th_hover[1], ph_Dsc);
			agc.SetInputChannelBit(030, 3, true);
		} else {
			SetThrusterResource(th_hover[0], NULL);
			SetThrusterResource(th_hover[1], NULL);
			agc.SetInputChannelBit(030, 3, false);
		}
		if (EVA_IP){
			if(!hLEVA){
				ToggleEVA();
			}
			else{
				if(high){
				}
				else{
					high=true;
					SetTouchdownPoints (_V(0,-5,10), _V(-1,-5,-10), _V(1,-5,-10));
				}
			}
		}
		if (ToggleEva && GroundContact()){
			ToggleEVA();
		}
		
		if (bToggleHatch){
			VESSELSTATUS vs;
			GetStatus(vs);
			if (vs.status == 1){
				//PlayVesselWave(Scontact,NOLOOP,255);
				//SetLmVesselHoverStage2(vessel);
				if(high){
					high=false;
					SetTouchdownPoints (_V(0,-0,10), _V(-1,-0,-10), _V(1,-0,-10));
				}
				else{
					high=true;
					SetTouchdownPoints (_V(0,-5,10), _V(-1,-5,-10), _V(1,-5,-10));
				}
			}
			bToggleHatch=false;
		}

		if (actualALT < 5.3 && !ContactOK && actualALT > 5.0){
			if (!sevent.isValid())
				Scontact.play();
			SetEngineLevel(ENGINE_HOVER,0);
			ContactOK = true;

		}
		if (CPswitch && HATCHswitch && EVAswitch && GroundContact()){
			ToggleEva = true;
			EVAswitch = false;
		}
		//
		// This does an abort stage if the descent stage runs out of fuel,
		// probably should start P71
		//
		if (GetPropellantMass(ph_Dsc)<=50 && actualALT > 10){
			Abortswitch=true;
			SeparateStage(stage);
			SetEngineLevel(ENGINE_HOVER,1);
			stage = 2;
			AbortFire();
			agc.SetInputChannelBit(030, 4, true); // Abort with ascent stage.
		}
		if (bManualSeparate && !startimer){
			VESSELSTATUS vs;
			GetStatus(vs);

			if (vs.status == 1){
				countdown=simt;
				LunarAscent.play(NOLOOP,200);
				startimer=true;
				//vessel->SetTouchdownPoints (_V(0,-0,10), _V(-1,-0,-10), _V(1,-0,-10));
			}
			else{
				SeparateStage(stage);
				stage = 2;
			}
		}
		if ((simt-(10+countdown))>=0 && startimer ){
			StartAscent();
		}
		//sprintf (oapiDebugString(),"FUEL %d",GetPropellantMass(ph_Dsc));
	}
	else if (stage == 2) {
		if (AscentEngineArmed()) {
			SetThrusterResource(th_hover[0], ph_Asc);
			SetThrusterResource(th_hover[1], ph_Asc);
			agc.SetInputChannelBit(030, 3, true);
		} else {
			SetThrusterResource(th_hover[0], NULL);
			SetThrusterResource(th_hover[1], NULL);
			agc.SetInputChannelBit(030, 3, false);
		}
		if (!AscentRCSArmed()) {
			SetRCS(NULL);
		}
		else{
			SetRCS(ph_rcslm1);
		}
	}
	else if (stage == 3){
		if (AscentEngineArmed()) {
			SetThrusterResource(th_hover[0], ph_Asc);
			SetThrusterResource(th_hover[1], ph_Asc);
			agc.SetInputChannelBit(030, 3, true);
		} else {
			SetThrusterResource(th_hover[0], NULL);
			SetThrusterResource(th_hover[1], NULL);
			agc.SetInputChannelBit(030, 3, false);
		}
		if(!AscentRCSArmed()){
			SetRCS(NULL);
		}
		else{
			SetRCS(ph_rcslm1);
		}
	}
	else if (stage == 4)
	{	
	}

    // x15 landing sound management

    double     simtime       ;
	int        mode          ;
	double     timeremaining ;
	double     timeafterpdi  ;
	double     timetoapproach;
	char names [255]         ;
	int        todo          ;
	double     offset        ;
	int        newbuffer     ;

	
	if(simt >NextEventTime)
	{
        NextEventTime=simt+0.1;
	    agc.GetStatus(&simtime,&mode,&timeremaining,&timeafterpdi,&timetoapproach);
    	todo = sevent.play(soundlib,
			    this,
				names,
				&offset,
				&newbuffer,
		        simtime,
				MissionTime,
				mode,
				timeremaining,
				timeafterpdi,
				timetoapproach,
				NOLOOP,
				255);
        if (todo)
		{
           if(offset > 0.)
                sevent.PlaySound( names,newbuffer,offset);
		   else sevent.PlaySound( names,true,0);
		}
	}
}

//
// Set GMBLswitch
//

void sat5_lmpkd::SetGimbal(bool setting)
{
	agc.SetInputChannelBit(032, 9, setting);
	GMBLswitch = setting;
}

//
// Get Mission Time
//

void sat5_lmpkd::GetMissionTime(double &Met)
{
	Met = MissionTime;
	return;
}

//
// Perform the stage separation as done when P12 is running and Abort Stage is pressed
//

void sat5_lmpkd::AbortStage()
{
	ButtonClick();
	AbortFire();
	Abortswitch = true;
	SeparateStage(stage);
	SetThrusterResource(th_hover[0], ph_Asc);
	SetThrusterResource(th_hover[1], ph_Asc);
	stage = 2;
	startimer = false;
	Abortswitch = true;
}

//
// Initiate ascent.
//

void sat5_lmpkd::StartAscent()

{
	SeparateStage(stage);
	stage = 2;
	SetEngineLevel(ENGINE_HOVER,1);
	startimer = false;

	LunarAscent.done();
}

//
// Scenario state functions.
//

void sat5_lmpkd::LoadStateEx (FILEHANDLE scn, void *vs)

{
    char *line;
	int	SwitchState;
	float ftcp;

		// default panel
	PanelId = 1;
	
	while (oapiReadScenario_nextline (scn, line)) {
        if (!strnicmp (line, "CONFIGURATION", 13)) {
            sscanf (line+13, "%d", &status);
		}
		else if (!strnicmp (line, "EVA", 3)) {
			EVA_IP = true;
		} 
		else if (!strnicmp (line, "CSWITCH", 7)) {
            SwitchState = 0;
			sscanf (line+7, "%d", &SwitchState);
			SetCSwitchState(SwitchState);
		} 
		else if (!strnicmp (line, "SSWITCH", 7)) {
            SwitchState = 0;
			sscanf (line+7, "%d", &SwitchState);
			SetSSwitchState(SwitchState);
		} else if (!strnicmp (line, "LPSWITCH", 8)) {
            SwitchState = 0;
			sscanf (line+8, "%d", &SwitchState);
			SetLPSwitchState(SwitchState);
		} else if (!strnicmp (line, "RPSWITCH", 8)) {
            SwitchState = 0;
			sscanf (line+8, "%d", &SwitchState);
			SetRPSwitchState(SwitchState);
		} else if (!strnicmp(line, "MISSNTIME", 9)) {
            sscanf (line+9, "%f", &ftcp);
			MissionTime = ftcp;
		}
		else if (!strnicmp(line, "UNMANNED", 8)) {
			int i;
			sscanf(line + 8, "%d", &i);
			Crewed = (i == 0);
		}
		else if (!strnicmp (line, "LANG", 4)) {
			strncpy (AudioLanguage, line + 5, 64);
		}
		else if (!strnicmp (line, "REALISM", 7)) {
			sscanf (line+7, "%d", &Realism);
		}
		else if (!strnicmp (line, "APOLLONO", 8)) {
			sscanf (line+8, "%d", &ApolloNo);
		}
		else if (!strnicmp(line, DSKY_START_STRING, sizeof(DSKY_START_STRING))) {
			dsky.LoadState(scn);
		}
		else if (!strnicmp(line, AGC_START_STRING, sizeof(AGC_START_STRING))) {
			agc.LoadState(scn);
		}
		else if (!strnicmp (line, "PANEL_ID", 8)) { 
			sscanf (line+8, "%d", &PanelId);
		} 
		else 
		{
            ParseScenarioLineEx (line, vs);
        }
    }

	switch (status) {
	case 0:
		stage=0;
		SetLmVesselDockStage();
		break;

	case 1:
		stage=1;
		SetLmVesselHoverStage();
		if (EVA_IP){
			SetupEVA();
		}
		break;
	case 2:
		stage=2;
		SetLmAscentHoverStage();
		break;
	}

	//
	// This may not be the best place for loading sounds, but it's the best place available
	// for now!
	//

	soundlib.SetLanguage(AudioLanguage);
	LoadDefaultSounds();

	//
	// Pass on the mission number and realism setting to the AGC.
	//

	agc.SetMissionInfo(ApolloNo, Realism);
}

void sat5_lmpkd::SetClassCaps (FILEHANDLE cfg)
{
	SetLmVesselDockStage();
}

void sat5_lmpkd::PostCreation ()

{
	soundlib.SetLanguage(AudioLanguage);
	LoadDefaultSounds();
}

void sat5_lmpkd::SetStateEx(const void *status)

{
	VESSELSTATUS2 *vslm = (VESSELSTATUS2 *) status;

	DefSetStateEx(status);
}

void sat5_lmpkd::SaveState (FILEHANDLE scn)

{
	SaveDefaultState (scn);	
	oapiWriteScenario_int (scn, "CONFIGURATION", status);
	if (EVA_IP){
		oapiWriteScenario_int (scn, "EVA", int(TO_EVA));
	}

	oapiWriteScenario_int (scn, "CSWITCH",  GetCSwitchState());
	oapiWriteScenario_int (scn, "SSWITCH",  GetSSwitchState());
	oapiWriteScenario_int (scn, "LPSWITCH",  GetLPSwitchState());
	oapiWriteScenario_int (scn, "RPSWITCH",  GetRPSwitchState());
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_string (scn, "LANG", AudioLanguage);
	oapiWriteScenario_int (scn, "PANEL_ID", PanelId);

	if (Realism != REALISM_DEFAULT) {
		oapiWriteScenario_int (scn, "REALISM", Realism);
	}

	oapiWriteScenario_int (scn, "APOLLONO", ApolloNo);

	if (!Crewed) {
		oapiWriteScenario_int (scn, "UNMANNED", 1);
	}

	dsky.SaveState(scn);
	agc.SaveState(scn);
}

bool sat5_lmpkd::LoadGenericCockpit ()

{
	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
	InVC = false;
	InPanel = false;
	return true;
}

//
// Transfer important data from the CSM to the LEM when the LEM is first
// created.
//

void sat5_lmpkd::SetLanderData(LemSettings &ls)

{
    char buffers[80];

	MissionTime = ls.MissionTime;
	agc.SetDesiredLanding(ls.LandingLatitude, ls.LandingLongitude, ls.LandingAltitude);
	strncpy (AudioLanguage, ls.language, 64);

	Crewed = ls.Crewed;
	AutoSlow = ls.AutoSlow;
	Realism = ls.Realism;
	ApolloNo = ls.MissionNo;

	agc.SetMissionInfo(ApolloNo, Realism);

	soundlib.SetLanguage(AudioLanguage);
	sprintf(buffers, "Apollo%d", ApolloNo);
    soundlib.SetSoundLibMissionPath(buffers);
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK bool ovcLoadPanel (VESSEL *vessel, int id)
{
	sat5_lmpkd *lem = (sat5_lmpkd *)vessel;

	return lem->LoadPanel(id);
}


DLLCLBK bool ovcPanelMouseEvent (VESSEL *vessel, int id, int event, int mx, int my)

{
	sat5_lmpkd *lem = (sat5_lmpkd *)vessel;

	return lem->PanelMouseEvent(id, event, mx, my);
}


DLLCLBK bool ovcPanelRedrawEvent (VESSEL *vessel, int id, int event, SURFHANDLE surf)

{
	sat5_lmpkd *lem = (sat5_lmpkd *)vessel;
	return lem->PanelRedrawEvent(id, event, surf);
}

DLLCLBK int ovcConsumeBufferedKey (VESSEL *vessel, DWORD key, bool down, const char *keystate)

{
	if(down) {
		sat5_lmpkd *lem = (sat5_lmpkd *)vessel;
		return lem->ConsumeDirectKey(key);
	}
	return 0;
}

DLLCLBK void ovcTimestep (VESSEL *vessel, double simt)
{
	sat5_lmpkd *lem = (sat5_lmpkd *) vessel;
	lem->PostStep(simt, 0, 0);
}

DLLCLBK void ovcLoadStateEx (VESSEL *vessel, FILEHANDLE scn, VESSELSTATUS *vs)

{
	sat5_lmpkd *lem = (sat5_lmpkd *) vessel;
	lem->LoadStateEx(scn, (void *) vs);
}

DLLCLBK void ovcSetClassCaps (VESSEL *vessel, FILEHANDLE cfg)
{
	sat5_lmpkd *lem = (sat5_lmpkd *)vessel;

	lem->SetClassCaps(cfg);
}

DLLCLBK void ovcSaveState (VESSEL *vessel, FILEHANDLE scn)

{
	sat5_lmpkd *lem = (sat5_lmpkd *)vessel;
	lem->SaveState(scn);
}

DLLCLBK bool ovcGenericCockpit (VESSEL *vessel) { 

	sat5_lmpkd *lm = (sat5_lmpkd *)vessel; 
	lm->SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
	return lm->LoadGenericCockpit(); 
}