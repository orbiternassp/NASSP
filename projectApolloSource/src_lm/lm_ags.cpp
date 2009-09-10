/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Abort Guidance System

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
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"
#include "tracer.h"
#include "papi.h"
#include "CollisionSDK/CollisionSDK.h"

#include "connector.h"

// Abort Sensor Assembly
LEM_ASA::LEM_ASA() : hsink("LEM-ASA-HSink",_vector3(0.013, 3.0, 0.03),0.03,0.04),
	heater("LEM-ASA-Heater",1,NULL,15,20,0,272,274,&hsink)
{
	lem = NULL;	
}

void LEM_ASA::Init(LEM *s){
	lem = s;
	// Therm setup
	hsink.isolation = 1.0; 
	hsink.Area = 975.0425; 
	hsink.mass = 9389.36206;
	hsink.SetTemp(270);
	if(lem != NULL){
		heater.WireTo(&lem->SCS_ASA_CB);
		lem->Panelsdk.AddHydraulic(&hsink);
		lem->Panelsdk.AddElectrical(&heater,false);
		heater.Enable();
		heater.SetPumpAuto();
	}
}

void LEM_ASA::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// AGS OFF  = ASA heaters active (OFF mode)
	// AGS STBY = ASA fully active   (WARMUP mode, becomes OPERATE mode when temp allows)
	// ASA OPR  = ASA fully active   (ditto)

	// ASA is 11.5x8x5.125 inches and weighs 20.7 pounds
	// ASA draws 74 watts operating? Need more info

	// ASA wants to stay at 120F.
	// Fast Warmup can get the ASA from 30F to 116F in 40 minutes.
	// Fast Warmup is active below 116F.
	// At 116F the Fine Warmup circuit takes over and gets to 120F and maintains it to within 0.2 degree F

	// There is no information on what the "OFF" mode does other than run the ASA heaters.
	// My guess is that some small heater keeps the ASA at 30F until standby happens.
	// sprintf(oapiDebugString(),"ASA Temp: %f AH %f",hsink.Temp,heater.pumping);

	// FIXME: ASA goes here	
}

void LEM_ASA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_ASA::LoadState(FILEHANDLE scn,char *end_str){

}

// Abort Electronics Assembly
LEM_AEA::LEM_AEA(){
	lem = NULL;	
}

void LEM_AEA::Init(LEM *s){
	lem = s;
}

void LEM_AEA::TimeStep(double simdt){
	if(lem == NULL){ return; }
}

void LEM_AEA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_AEA::LoadState(FILEHANDLE scn,char *end_str){

}

// Data Entry and Display Assembly
LEM_DEDA::LEM_DEDA(){
	lem = NULL;	
}

void LEM_DEDA::Init(LEM *s){
	lem = s;
}

void LEM_DEDA::TimeStep(double simdt){
	if(lem == NULL){ return; }
}

void LEM_DEDA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_DEDA::LoadState(FILEHANDLE scn,char *end_str){

}
