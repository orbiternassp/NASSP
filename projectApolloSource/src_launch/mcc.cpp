/****************************************************************************
  This file is part of Project Apollo - NASSP

  Mission Control Center / Ground Support Network

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
  *	Revision 1.1  2009/02/18 23:21:14  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.8  2008/04/11 11:49:47  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.7  2007/09/04 14:22:14  jasonims
  *	Correcting my mistakes...
  *	
  *	Revision 1.6  2007/08/25 12:09:30  tschachim
  *	Bugfix.
  *	
  *	Revision 1.5  2007/08/11 16:31:25  jasonims
  *	Changed name of BuildFirstStage to ChangeSatVBuildState  to clarify function...
  *	A little more Capcom...
  *	
  *	Revision 1.4  2007/07/27 19:57:27  jasonims
  *	Created MCC master class and split individual functions into sub-classes.  Initial work on CapCom routines.
  *	
  *	Revision 1.3  2007/06/06 15:02:15  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.2  2007/02/19 16:24:44  tschachim
  *	VC6 MCC fixes.
  *	
  *	Revision 1.1  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"
#include "mcc.h"

// MCC MAIN CLASS

// CONS
MCC::MCC(){

	cm = NULL;
	
}

void MCC::Init(){

}

void MCC::TimeStep(double simdt){

}

MC_GroundTrack::MC_GroundTrack(){
	CM_DeepSpace = false;
	LastAOSUpdate=0;
	ClosestRange=0;
	ClosestStation=0;

	// Reset ground stations
	int x=0;
	while(x<MAX_GROUND_STATION){
		GroundStations[x].Active = false;
		GroundStations[x].Position[0] = 0;
		GroundStations[x].Position[1] = 0;
		GroundStations[x].DownTlmCaps = 0;
		GroundStations[x].GroundCommCaps = 0;
		GroundStations[x].HasAcqAid = false;
		GroundStations[x].HasRadar = false;
		GroundStations[x].SBandAntenna = 0;
		GroundStations[x].StationType = 0;
		GroundStations[x].TelemetryCaps = 0;
		GroundStations[x].TrackingCaps = 0;
		GroundStations[x].UpTlmCaps = 0;
		GroundStations[x].USBCaps = 0;
		GroundStations[x].StationPurpose = 0;
		sprintf(GroundStations[x].Name,"INVALID");
		sprintf(GroundStations[x].Code,"XXX");
		x++;
	}
}


void MC_GroundTrack::Init(Saturn *vessel) {

	cm = vessel;

	ClosestRange=0;
	ClosestStation=0;
	LastAOSUpdate=0;

	// Load ground station information.
	// Later this can be made dynamic, but this will work for now.

	// This particular ground station list is the stations that were on-net for Apollo 8.
	// The index numbers represent status as of 1968.
	
	sprintf(GroundStations[1].Name,"ANTIGUA"); sprintf(GroundStations[1].Code,"ANG");
	GroundStations[1].Position[0] = 28.40433; GroundStations[1].Position[1] = -80.60192;
	GroundStations[1].Active = true;

	sprintf(GroundStations[2].Name,"ASCENSION"); sprintf(GroundStations[2].Code,"ASC");
	GroundStations[2].Position[0] = -7.94354; GroundStations[2].Position[1] = -14.37105;
	GroundStations[2].Active = true;

	sprintf(GroundStations[3].Name,"BERMUDA"); sprintf(GroundStations[3].Code,"BDA");
	GroundStations[3].Position[0] = 32.36864; GroundStations[3].Position[1] = -64.68563;
	GroundStations[3].Active = true;

	sprintf(GroundStations[4].Name,"GRAND CANARY"); sprintf(GroundStations[4].Code,"CYI");
	GroundStations[4].Position[0] = 27.74055; GroundStations[4].Position[1] = -15.60077;
	GroundStations[4].Active = true;

	sprintf(GroundStations[5].Name,"HONEYSUCKLE"); sprintf(GroundStations[5].Code,"HSK");
	GroundStations[5].Position[0] = -35.40282; GroundStations[5].Position[1] = 148.98144;
	GroundStations[5].Active = true;

	sprintf(GroundStations[6].Name,"CARNARVON"); sprintf(GroundStations[6].Code,"CRO");
	GroundStations[6].Position[0] = -24.90619; GroundStations[6].Position[1] = 113.72595;
	GroundStations[6].Active = true;

	sprintf(GroundStations[7].Name,"CORPUS CHRISTI"); sprintf(GroundStations[7].Code,"TEX");
	GroundStations[7].Position[0] = 27.65273; GroundStations[7].Position[1] = -97.37588;
	GroundStations[7].Active = true;

	sprintf(GroundStations[8].Name,"GOLDSTONE"); sprintf(GroundStations[8].Code,"GDS");
	GroundStations[8].Position[0] = 35.33820; GroundStations[8].Position[1] = -116.87421;
	GroundStations[8].Active = true;

	sprintf(GroundStations[9].Name,"GRAND BAHAMA"); sprintf(GroundStations[9].Code,"GBM");
	GroundStations[9].Position[0] = 26.62022; GroundStations[9].Position[1] = -78.35825;
	GroundStations[9].Active = true;

	sprintf(GroundStations[10].Name,"GUAM"); sprintf(GroundStations[10].Code,"GWM");
	GroundStations[10].Position[0] = 13.30929; GroundStations[10].Position[1] = 144.73694;
	GroundStations[10].Active = true;

	sprintf(GroundStations[11].Name,"GUAYMAS"); sprintf(GroundStations[11].Code,"GYM");
	GroundStations[11].Position[0] = 27.95029; GroundStations[11].Position[1] = -110.90846;
	GroundStations[11].Active = true;

	sprintf(GroundStations[12].Name,"HAWAII"); sprintf(GroundStations[12].Code,"HAW");
	GroundStations[12].Position[0] = 21.44719; GroundStations[12].Position[1] = -157.76307;
	GroundStations[12].Active = true;

	sprintf(GroundStations[13].Name,"MADRID"); sprintf(GroundStations[13].Code,"MAD");
	GroundStations[13].Position[0] = 40.45443; GroundStations[13].Position[1] = -4.16990;
	GroundStations[13].Active = true;

	sprintf(GroundStations[14].Name,"MERRIT"); sprintf(GroundStations[14].Code,"MIL");
	GroundStations[14].Position[0] = 28.40433; GroundStations[14].Position[1] = -80.60192;
	GroundStations[14].Active = true;

	// 15 = USNS HUNTSVILLE
	// 16 = USNS MERCURY
	// 17 = USNS REDSTONE
	// 18 = USNS VANGUARD
	// 19 = USNS WATERTOWN
	// 20 = ARIA-1
	// 21 = ARIA-2
	// 22 = ARIA-3
	// 23 = ARIA-4
	// 24 = ARIA-5
	// 25 = ARIA-6
	// 26 = ARIA-7
	// 27 = ARIA-8
	// 28 = MSFNOC (??)
	// 29 = HSK WING
	// 30 = GDS WING
	// 31 = MAD WING
	// 32 = NTTF (PRELAUNCH?)

	sprintf(GroundStations[33].Name,"TANANARIVE"); sprintf(GroundStations[33].Code,"TAN");
	GroundStations[33].Position[0] = -19.00000; GroundStations[33].Position[1] = 47.27556;
	GroundStations[33].Active = true;

	// ALL SUBSEQUENT ARE DOD STATIONS

	// 34 = ANTIGUA
	// 35 = ASCENSION
	// 36 = CAPE KENNEDY
	// 37 = GRAND BAHAMA
	// 38 = MERRIT I.

	sprintf(GroundStations[39].Name,"PRETORIA"); sprintf(GroundStations[39].Code,"PRE");
	GroundStations[39].Position[0] = -30.78330; GroundStations[39].Position[1] = 28.58330;
	GroundStations[39].Active = true;

	sprintf(GroundStations[40].Name,"VANDENBERG"); sprintf(GroundStations[40].Code,"CAL");
	GroundStations[40].Position[0] = 34.74007; GroundStations[40].Position[1] = -120.61909;
	GroundStations[40].Active = true;

	sprintf(GroundStations[41].Name,"WHITE SANDS"); sprintf(GroundStations[41].Code,"WHS");
	GroundStations[41].Position[0] = 32.35637; GroundStations[41].Position[1] = -106.37826;
	GroundStations[41].Active = true;

}

// Timestep
void MC_GroundTrack::TimeStep(double simdt){

	/* AOS DETERMINATION */
	if((LastAOSUpdate+1) < simdt){
		int x=0;
		double PosDiff[2];
		double LateralRange;	
		double SlantRange;
		ClosestRange = 0;
		ClosestStation = 0;
		LastAOSUpdate = simdt;

		// Get CM position
		cm->GetEquPos(CM_Position[1],CM_Position[0],CM_Position[2]);
		// Convert from radians (sigh...)
		CM_Position[0] *= 57.2957795;
		CM_Position[1] *= 57.2957795;
		// Get altitude
		CM_Position[2] = cm->GetAltitude();

		while(x<MAX_GROUND_STATION){
			if(GroundStations[x].Active == true){
				// Get lateral range
				PosDiff[0] = fabs(GroundStations[x].Position[0]-CM_Position[0]);
				PosDiff[1] = fabs(GroundStations[x].Position[1]-CM_Position[1]);
				LateralRange = sqrt((PosDiff[0]*PosDiff[0])+(PosDiff[1]*PosDiff[1]));
				LateralRange *= 111123; // Nice number, isn't it? Meters per degree.
				// Figure slant range
				SlantRange = sqrt((LateralRange*LateralRange)+(CM_Position[2]*CM_Position[2]));
				// Check
				if(ClosestRange == 0){ ClosestRange = SlantRange; }
				if(SlantRange > ClosestRange){ x++; continue; }
				ClosestRange = SlantRange; ClosestStation = x;
			}
			x++;
		}
		/*
		sprintf(oapiDebugString(),"CM-POS: %f %f ALT %f CLOSEST STN %d '%s' DST %f",
			CM_Position[0],CM_Position[1],CM_Position[2],
			ClosestStation,GroundStations[ClosestStation].Name,ClosestRange);
		*/
	}

}

MC_CapCom::MC_CapCom(){

	strcpy (language,"_en");
	
	strcpy (capcomdb_fname, "capcomdb");
	strcat (capcomdb_fname, language);
	strcat (capcomdb_fname, ".npd");

	strcpy (trnscrpt_fname, "transcript");
	strcat (trnscrpt_fname, ".log");

}

MC_CapCom::~MC_CapCom(){

//	trnscrpt_h = fopen(trnscrpt_fname,"w");
//	fprintf(trnscrpt_h,"END_TRANSCRIPT\n");
//	fclose(trnscrpt_h);


}

void MC_CapCom::Init(){

//	trnscrpt_h = fopen(trnscrpt_fname,"a");
//	fprintf(trnscrpt_h,"BEGIN_TRANSCRIPT\n");
//	fclose(trnscrpt_h);
}

void MC_CapCom::TimeStep(double simdt){
	//trnscrpt_h = fopen(&trnscrpt_fname,"a");
	//fprintf(trnscrpt_h,"Current SimTime %f\n",simdt);
	//fclose(trnscrpt_h);

}

int MC_CapCom::Talk(char *ID, ...){

	va_list argptr;

	va_start(argptr, ID);

	va_end(argptr);

	return 0;
}