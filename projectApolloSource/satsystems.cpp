/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Code for simulation of Saturn hardware systems.

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
  *	Revision 1.7  2005/07/19 16:43:19  tschachim
  *	Some disabled debug prints
  *	
  *	Revision 1.6  2005/06/06 12:33:37  tschachim
  *	Fuel cells are no longer started, some disabled debug prints
  *	
  *	Revision 1.5  2005/05/26 16:02:30  tschachim
  *	Added fuel cell cooling functions and some (disabled) test code
  *	
  *	Revision 1.4  2005/05/05 21:49:40  tschachim
  *	Added some (disabled) debug code
  *	
  *	Revision 1.3  2005/05/02 12:56:24  tschachim
  *	various debug prints added for testing only
  *	
  *	Revision 1.2  2005/04/22 14:06:53  tschachim
  *	Introduced PanelSDK
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"

#include "saturn.h"

//FILE *PanelsdkLogFile;

void Saturn::SystemsInit() {

	// default state
	systemsState = SATSYSTEMS_NONE;

	// initialize SPSDK
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo\\SaturnSystems");

	//PanelsdkLogFile = fopen("NASSP-Systems.log", "w");  
}

void Saturn::SystemsTimestep(double simt) {

	//
	// Don't clock the computer unless we're actually at the pad.
	//

	if (stage >= PRELAUNCH_STAGE) {
		dsky.Timestep(MissionTime);
		agc.Timestep(MissionTime);
	}

	//
	// MasterAlarm
	//

	if (masterAlarm && (simt > masterAlarmCycleTime)) {
		masterAlarmLit = !masterAlarmLit;
		masterAlarmCycleTime = simt + 0.25;
		if (masterAlarmLit) {
			SMasterAlarm.play(NOLOOP,255);
		}
	}

	//
	// Each timestep is passed to the SPSDK
	// to perform internal computations on the 
	// systems.
	//

	Panelsdk.Timestep(simt);

	//
	// Systems state handling
	//

	double *pMax, *fancap, pCabin, pSuit;
	float *size;
	int *open, *number;

	switch (systemsState) {

	case SATSYSTEMS_NONE:
		
		// No crew 
		number = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CREW:NUMBER");
		*number = 0; 

		// No leak
		open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:OPEN");
		*open = SP_VALVE_CLOSE;

		// Cabin pressure regulator to 14.7 psi
		pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:PRESSMAX");
		*pMax = 14.7 / PSI;	
		open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT:OPEN");
		*open = SP_VALVE_OPEN;

		// O2 demand regulator to 5.5 inH2O higher than cabin pressure
		pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:PRESSMAX");
		*pMax = 14.7 / PSI + 5.5 / INH2O;	
		open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT2:OPEN");
		*open = SP_VALVE_OPEN;

		// Close cabin to suit circuit return value
		open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
		*open = SP_VALVE_CLOSE;

		// Turn off suit compressor
		SuitCompressor1Switch.SwitchTo(THREEPOSSWITCH_CENTER);
		SuitCompressor2Switch.SwitchTo(THREEPOSSWITCH_CENTER);

		// Next state
		systemsState = SATSYSTEMS_PRELAUNCH;
		break;

	case SATSYSTEMS_PRELAUNCH:
		if (MissionTime >= -6000) {	// 1h 40min before launch
			// Crew ingress
			number = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CREW:NUMBER");
			*number = 3; 

			// Cabin leak
			size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
			*size = (float)0.0005;
			open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:OPEN");
			*open = SP_VALVE_OPEN;

			// O2 demand regulator to 2.5 inH2O higher than cabin pressure
			pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:PRESSMAX");
			*pMax = 14.7 / PSI + 2.5 / INH2O;
			open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT2:OPEN");
			*open = SP_VALVE_OPEN;

			// Turn on suit compressor 1, prelaunch configuration
			SuitCompressor1Switch.SwitchTo(THREEPOSSWITCH_UP);
			SuitCompressor2Switch.SwitchTo(THREEPOSSWITCH_CENTER);
			fancap = (double*) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER:FANCAP");
			*fancap = 110000.0;

			// Next state
			systemsState = SATSYSTEMS_CABINCLOSEOUT;
		}	
		break;

	case SATSYSTEMS_CABINCLOSEOUT:
		if (GetAtmPressure() <= 6.0 / PSI) {
			// Cabin pressure relieve
			size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
			*size = (float)0.25;
			
			// Cabin pressure regulator to 5 psi
			pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:PRESSMAX");
			*pMax = 5.0 / PSI;

			// Close O2 demand regulator 
			open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT2:OPEN");
			*open = SP_VALVE_CLOSE;

			// Open cabin to suit circuit return value
			open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
			*open = SP_VALVE_OPEN;

			// Open suit pressure relieve
			open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:OPEN");
			*open = SP_VALVE_OPEN;

			// Open direct O2 valve
			open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:LEAK:OPEN");
			*open = SP_VALVE_OPEN;

			// Suit compressor to flight configuration
			fancap = (double*) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER:FANCAP");
			*fancap = 60000.0;	

			// Next state
			systemsState = SATSYSTEMS_CABINVENTING;
		}		
		break;

	case SATSYSTEMS_CABINVENTING:
		pCabin = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS") * PSI;
		pSuit = *(double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS") * PSI;

		size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
		open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:OPEN");

		if (pCabin <= 5.0) {
			// cabin leak
			*size = (float)0.001;	
		}
		if (pSuit <= 5.0) {
			// Close suit pressure relieve
			*open = SP_VALVE_CLOSE;

			// Close direct O2 valve
			*(int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:LEAK:OPEN") = SP_VALVE_CLOSE;
		}

		if (*size == (float)0.001 && *open == SP_VALVE_CLOSE) {
			// Next state
			systemsState = SATSYSTEMS_FLIGHT;
		}
		break;
	}


//------------------------------------------------------------------------------------
// Various debug prints
//------------------------------------------------------------------------------------

	double *massCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:MASS");
	double *tempCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	double *pressCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	double *pressCabinCO2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	double *co2removalrate=(double*)Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER:CO2REMOVALRATE");

	double *flowCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOW");
	double O2flow = *flowCabin + *(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOW");
	double *O2flowdirect=(double*)Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE:FLOW");

	double *pressSuit=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:PRESS");
	double *tempSuit=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:TEMP");
	double *pressSuitCO2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:CO2_PPRESS");
	double *pressSuitCRV=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS");
	double *tempSuitCRV=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:TEMP");

	double *tempCabinRad1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINRADIATOR1:TEMP");
	double *tempCabinRad2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINRADIATOR2:TEMP");

	double *voltageA=(double*)Panelsdk.GetPointerByString("ELECTRIC:DC_A:VOLTS");
	double *amperageA=(double*)Panelsdk.GetPointerByString("ELECTRIC:DC_A:AMPS");


/*	sprintf(oapiDebugString(), "Cabin - Mass [g] %.2f Temp [K] %.2f Press [psi] %.2f CO2 PPress [mmHg] %.2f  CO2ABSORBER - Co2Rate %.2f  DC-A - Volt %.2f Amp %.2f", 
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064,
		*co2removalrate,
		*voltageA, *amperageA); 
*/

	double *massO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:MASS");
	double *tempO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:TEMP");
	double *pressO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:PRESS");
	double *vapormassO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:O2_VAPORMASS");

	double *massO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:MASS");
	double *tempO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:TEMP");
	double *pressO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:PRESS");

/*	double *massO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:MASS");
	double *tempO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:TEMP");
	double *pressO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:PRESS");
*/

	double *massO2SMSupply=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY:MASS");
	double *tempO2SMSupply=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY:TEMP");
	double *pressO2SMSupply=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY:PRESS");

	double *massO2MainReg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:MASS");
	double *tempO2MainReg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:TEMP");
	double *pressO2MainReg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:PRESS");

	double *tempRad1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR1:TEMP");
	double *tempRad2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR2:TEMP");
	double *tempRad3=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR3:TEMP");
	double *tempRad4=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR4:TEMP");
	double tempRad = (*tempRad1 + *tempRad2 + *tempRad3 + *tempRad4) / 4.0;

	double *tempFCC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING:TEMP");
	double *tempFCC2=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2COOLING:TEMP");
	double *tempFCC3=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3COOLING:TEMP");

	double *massH2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:MASS");
	double *tempH2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:TEMP");
	double *pressH2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:PRESS");
	double *isonH2Tank1Heater=(double*)Panelsdk.GetPointerByString("ELECTRIC:H2TANK1HEATER:ISON");
	double *isonH2Tank1Fan=(double*)Panelsdk.GetPointerByString("ELECTRIC:H2TANK1FAN:ISON");

	double *massH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:MASS");
	double *tempH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:TEMP");
	double *pressH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:PRESS");
	double *vapormassH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:H2_VAPORMASS");

	double *massH2FCM=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELLMANIFOLD:MASS");
	double *tempH2FCM=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELLMANIFOLD:TEMP");
	double *pressH2FCM=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELLMANIFOLD:PRESS");

	double *massO2FCM=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELLMANIFOLD:MASS");
	double *tempO2FCM=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELLMANIFOLD:TEMP");
	double *pressO2FCM=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELLMANIFOLD:PRESS");

	double *massWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2O_WASTE:MASS");
	double *tempWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2O_WASTE:TEMP");
	double *pressWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2O_WASTE:PRESS");

	double *voltFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:VOLTS");
	double *ampFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:AMPS");
	double *tempFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:TEMP");
	double *dphFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:DPH");
	double *h2flowFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:H2FLOW");
	double *o2flowFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:O2FLOW");

	double *tempFC2=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:TEMP");
	double *tempFC3=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:TEMP");
	//double *ison=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING:ISON");

	// ECS Pressures
/*	sprintf(oapiDebugString(), "State %d SuitCompDp %.2f SuitCabinDp %.2f, O2Flow %.2f, DirectO2Flow %.2f Cabin-p %.2f T %.1f Suit-p %.2f T %.1f co2pp %.2f SuitCRV-p %.2f T %.1f", 
		systemsState, 
		(*pressSuit - *pressSuitCRV) * 0.000145038, (*pressSuitCRV - *pressCabin) * INH2O,
		O2flow * LBH, *O2flowdirect * LBH,
		*pressCabin * 0.000145038, *tempCabin,
		*pressSuit * 0.000145038, *tempSuit, *pressSuitCO2 * 0.00750064,
		*pressSuitCRV * 0.000145038, *tempSuitCRV);
*/
	// Cabin O2 supply
/*	sprintf(oapiDebugString(), "O2T1-m %.1f T %.1f p %.1f O2T2-m %.1f T %.1f p %.1f O2SM-m %.1f T %.1f p %4.1f O2M-m %.1f T %.1f p %5.1f CAB-m %.1f T %.1f p %.1f CO2PP %.2f RAD-T %.1f", 
		*massO2Tank1 / 1000.0, *tempO2Tank1, *pressO2Tank1 * 0.000145038,
		*massO2Tank2 / 1000.0, *tempO2Tank2, *pressO2Tank2 * 0.000145038,
		*massO2SMSupply / 1000.0, *tempO2SMSupply, *pressO2SMSupply * 0.000145038,
		*massO2MainReg / 1000.0, *tempO2MainReg, *pressO2MainReg * 0.000145038,
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064,
		*tempRad); 
*/

	// Fuel Cell, flow in lb/h
/*	sprintf(oapiDebugString(), "FC2-T %.1f FC3-T %.1f FC1-T %.1f V %.2f A %.2f H2Flow %.3f O2Flow %.3f Rad1-T %.1f Rad2-T %.1f Rad3-T %.1f Rad4-T %.1f", 
		*tempFC2, *tempFC3, *tempFC, *voltFC, *ampFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665,
		*tempRad1, *tempRad2, *tempRad3, *tempRad4);
*/		

	// Fuel Cell with tanks, flow in lb/h
/*	sprintf(oapiDebugString(), "O2T1-m %.2f vm %.2f T %.1f p %.1f H2T2-m %.2f vm %.2f T %.1f p %.1f FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f H2FCM %.1f T %.1f p %6.1f", 
		*massO2Tank1, *vapormassO2Tank1, *tempO2Tank1, *pressO2Tank1 * 0.000145038,
		*massH2Tank2, *vapormassH2Tank2, *tempH2Tank2, *pressH2Tank2 * 0.000145038,
		*voltFC, *ampFC, *tempFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665, 
		*massH2FCM, *tempH2FCM, *pressH2FCM * 0.000145038);
*/

	// Fuel Cell with manifolds, flow in lb/h
/*	sprintf(oapiDebugString(), "FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f H2FCM %.1f T %.1f p %6.1f O2FCM %.1f T %.1f p %6.1f", 
		*voltFC, *ampFC, *tempFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665, 
		*massH2FCM, *tempH2FCM, *pressH2FCM * 0.000145038,
		*massO2FCM, *tempO2FCM, *pressO2FCM * 0.000145038);
*/

	// Fuel Cell H2
/*	sprintf(oapiDebugString(), "H2T1-m %.2f T %.1f p %.1f H %.1f F %.1f H2T2-m %.2f T %.1f p %.1f H2FCM-m %.2f T %.1f p %.1f FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f dpH %f Waste-m %.1f T %.1f p %4.1f", 
		*massH2Tank1, *tempH2Tank1, *pressH2Tank1 * 0.000145038, *isonH2Tank1Heater, *isonH2Tank1Fan, 
		*massH2Tank2, *tempH2Tank2, *pressH2Tank2 * 0.000145038,
		*massH2FCM, *tempH2FCM, *pressH2FCM * 0.000145038,
		*voltFC, *ampFC, *tempFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665, *dphFC, 
		*massWaste, *tempWaste, *pressWaste * 0.000145038); 
*/

	// Fuel Cell H2 Tank2
/*	sprintf(oapiDebugString(), "H2T2-m %.2f vm %.2f T %.1f p %.1f FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f H2FCM-m %.2f T %.1f p %.1f", 
		*massH2Tank2, *vapormassH2Tank2, *tempH2Tank2, *pressH2Tank2 * 0.000145038,
		*voltFC, *ampFC, *tempFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665,
		*massH2FCM, *tempH2FCM, *pressH2FCM * 0.000145038);
*/

/*	fprintf(PanelsdkLogFile, "%f H2T1-m %.2f T %.1f p %.1f H2T2-m %.2f T %.1f p %.1f H2FCM-m %.2f T %.1f p %.1f O2T1-m %.2f T %.2f p %.2f O2T2-m %.2f T %.2f p %.2f O2SM-m %.2f T %.2f p %5.2f O2MAIN-m %.2f T %.2f p %5.2f Cabin-m %.2f T %.2f p %.2f CO2 PP %.2f Co2Rate %f Rad-T %.2f\n", 
		simt, 
		*massH2Tank1 / 1000.0, *tempH2Tank1, *pressH2Tank1 * 0.000145038,
		*massH2Tank2 / 1000.0, *tempH2Tank2, *pressH2Tank2 * 0.000145038,
		*massH2FCM,			   *tempH2FCM,   *pressH2FCM   * 0.000145038,
		*massO2Tank1 / 1000.0, *tempO2Tank1, *pressO2Tank1 * 0.000145038,
		*massO2Tank2 / 1000.0, *tempO2Tank2, *pressO2Tank2 * 0.000145038,
		*massO2SMSupply / 1000.0, *tempO2SMSupply, *pressO2SMSupply * 0.000145038,
		*massO2MainReg / 1000.0, *tempO2MainReg, *pressO2MainReg * 0.000145038,
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064, *co2removalrate,
		*tempRad);
	fflush(PanelsdkLogFile);
*/
}

bool Saturn::AutopilotActive()

{
	return autopilot && CMCswitch;
}

bool Saturn::CabinFansActive()

{
	return CabinFan1Switch || CabinFan2Switch;
}

//
// Forcibly activate the SIVB RCS for unmanned control.
//

void Saturn::ActivateS4RCS()

{
	RPswitch1 = true;
	RPswitch2 = true;
	RPswitch3 = true;
}

void Saturn::DeactivateS4RCS()

{
	RPswitch1 = false;
	RPswitch2 = false;
	RPswitch3 = false;
}

//
// And CSM
//

void Saturn::ActivateCSMRCS()

{
	LPswitch1 = true;
	LPswitch2 = true;
	LPswitch3 = true;

	RH11switch = false;
	RH12switch = false;
	RH13switch = false;
	RH14switch = false;
	RH21switch = false;
	RH22switch = false;
	RH23switch = false;
	RH24switch = false;

	PP1switch = true;
	PP2switch = true;
	PP3switch = true;
	PP4switch = true;
	SP1switch = true;
	SP2switch = true;
	SP3switch = true;
	SP4switch = true;
}

void Saturn::DeactivateCSMRCS()

{
	LPswitch1 = false;
	LPswitch2 = false;
	LPswitch3 = false;

	RH11switch = true;
	RH12switch = true;
	RH13switch = true;
	RH14switch = true;
	RH21switch = true;
	RH22switch = true;
	RH23switch = true;
	RH24switch = true;

	PP1switch = false;
	PP2switch = false;
	PP3switch = false;
	PP4switch = false;
	SP1switch = false;
	SP2switch = false;
	SP3switch = false;
	SP4switch = false;
}

void Saturn::ActivateSPS()

{
	SPSswitch = true;
}

void Saturn::DeactivateSPS()

{
	SPSswitch = false;
}

void Saturn::SetEngineIndicators()

{
	for (int i = 1; i <= 5; i++) {
		SetEngineIndicator(i);
	}
}

void Saturn::ClearEngineIndicators()

{
	for (int i = 1; i <= 5; i++) {
		ClearEngineIndicator(i);
	}
}

void Saturn::SetLiftoffLight()

{
	LAUNCHIND[0] = true;
}

void Saturn::ClearLiftoffLight()

{
	LAUNCHIND[0] = false;
}

void Saturn::SetAutopilotLight()

{
	AutopilotLight = true;
}

void Saturn::ClearAutopilotLight()

{
	AutopilotLight = false;
}

void Saturn::SetEngineIndicator(int i)

{
	if (i < 1 || i > 5)
		return;

	ENGIND[i - 1] = true;
}

void Saturn::ClearEngineIndicator(int i)

{
	if (i < 1 || i > 5)
		return;

	ENGIND[i - 1] = false;
}

void Saturn::FuelCellCoolingBypass(int fuelcell, bool bypassed) {

	// Bypass Radiator 2 and 4
	char buffer[100];

	sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:2:BYPASSED", fuelcell);
	bool *bp = (bool *) Panelsdk.GetPointerByString(buffer);
	*bp = bypassed;

	sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:4:BYPASSED", fuelcell);
	bp = (bool *) Panelsdk.GetPointerByString(buffer);
	*bp = bypassed;
}

bool Saturn::FuelCellCoolingBypassed(int fuelcell) {

	// It's bypassed when Radiator 2 is bypassed
	char buffer[100];

	sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:2:BYPASSED", fuelcell);
	bool *bypassed = (bool *) Panelsdk.GetPointerByString(buffer);
	return *bypassed;
}
