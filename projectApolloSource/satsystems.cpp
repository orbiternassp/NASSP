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
  *	Revision 1.23  2005/08/23 22:18:47  movieman523
  *	SPS switch now works.
  *	
  *	Revision 1.22  2005/08/23 21:29:03  movieman523
  *	RCS state is now only checked when a stage event occurs or when a valve is opened or closed, not every timestep.
  *	
  *	Revision 1.21  2005/08/23 20:13:12  movieman523
  *	Added RCS talkbacks and changed AGC to use octal addresses for EMEM.
  *	
  *	Revision 1.20  2005/08/22 19:47:33  movieman523
  *	Fixed long timestep on startup, and added new Virtual AGC with EDRUPT fix.
  *	
  *	Revision 1.19  2005/08/21 22:21:00  movieman523
  *	Fixed SM RCS and activated SIVB RCS at all times for now.
  *	
  *	Revision 1.18  2005/08/21 16:23:32  movieman523
  *	Added more alarms.
  *	
  *	Revision 1.17  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.16  2005/08/15 19:25:03  movieman523
  *	Added CSM attitude control switches and removed old ones.
  *	
  *	Revision 1.15  2005/08/15 02:37:57  movieman523
  *	SM RCS is now wired up.
  *	
  *	Revision 1.14  2005/08/13 14:21:36  movieman523
  *	Added beginnings of caution and warning system.
  *	
  *	Revision 1.13  2005/08/12 21:42:14  movieman523
  *	Added support for 'SIVB Takeover' bit on launch.
  *	
  *	Revision 1.12  2005/08/10 22:31:57  movieman523
  *	IMU is now enabled when running Prog 01.
  *	
  *	Revision 1.11  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.10  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.9  2005/08/05 13:12:19  tschachim
  *	No systems during rollout
  *	
  *	Revision 1.8  2005/07/30 16:13:49  tschachim
  *	Added systemsState handling.
  *	
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
#include "IMU.h"

#include "saturn.h"

#include "ioChannels.h"

//FILE *PanelsdkLogFile;

void Saturn::SystemsInit() {

	// default state
	systemsState = SATSYSTEMS_NONE;

	// initialize SPSDK
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo\\SaturnSystems");

	//PanelsdkLogFile = fopen("NASSP-Systems.log", "w");

	//
	// Default valve states.
	//
	// Make sure switches and talkback state match.
	//

	SetValveState(CSM_He1_TANKA_VALVE, true);
	SetValveState(CSM_He1_TANKB_VALVE, true);
	SetValveState(CSM_He1_TANKC_VALVE, true);
	SetValveState(CSM_He1_TANKD_VALVE, true);

	SetValveState(CSM_He2_TANKA_VALVE, true);
	SetValveState(CSM_He2_TANKB_VALVE, true);
	SetValveState(CSM_He2_TANKC_VALVE, true);
	SetValveState(CSM_He2_TANKD_VALVE, true);

	SetValveState(CSM_PRIPROP_TANKA_VALVE, false);
	SetValveState(CSM_PRIPROP_TANKB_VALVE, false);
	SetValveState(CSM_PRIPROP_TANKC_VALVE, false);
	SetValveState(CSM_PRIPROP_TANKD_VALVE, false);

	SetValveState(CSM_SECPROP_TANKA_VALVE, false);
	SetValveState(CSM_SECPROP_TANKB_VALVE, false);
	SetValveState(CSM_SECPROP_TANKC_VALVE, false);
	SetValveState(CSM_SECPROP_TANKD_VALVE, false);

	SetValveState(CM_RCSPROP_TANKA_VALVE, false);
	SetValveState(CM_RCSPROP_TANKB_VALVE, false);
}

void Saturn::SystemsTimestep(double simt, double simdt) {

	//
	// Don't clock the computer and the internal systems unless we're actually at the pad.
	//

	if (stage >= PRELAUNCH_STAGE) {

		dsky.Timestep(MissionTime);
		dsky2.Timestep(MissionTime);
		agc.Timestep(MissionTime, simdt);
		imu.Timestep(MissionTime);
		cws.TimeStep(MissionTime);

		// Each timestep is passed to the SPSDK
		// to perform internal computations on the 
		// systems.

		Panelsdk.Timestep(simt);

		//
		// Systems state handling
		//

		AtmosStatus atm;
		GetAtmosStatus(atm);

		double *pMax, *fancap;
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

			size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
			open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:OPEN");

			if (atm.CabinPressurePSI <= 5.0) {
				// cabin leak
				*size = (float)0.001;	
			}
			if (atm.SuitPressurePSI <= 5.0) {
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
	ChannelValue12 val12;
	val12.Value = agc.GetOutputChannel(012);

	return (autopilot && CMCswitch) && !val12.Bits.EnableSIVBTakeover;
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
}

void Saturn::DeactivateS4RCS()

{
}

//
// And CSM
//

void Saturn::ActivateCSMRCS()

{
	SetValveState(CSM_He1_TANKA_VALVE, false);
	SetValveState(CSM_He1_TANKB_VALVE, false);
	SetValveState(CSM_He1_TANKC_VALVE, false);
	SetValveState(CSM_He1_TANKD_VALVE, false);

	SetValveState(CSM_He2_TANKA_VALVE, false);
	SetValveState(CSM_He2_TANKB_VALVE, false);
	SetValveState(CSM_He2_TANKC_VALVE, false);
	SetValveState(CSM_He2_TANKD_VALVE, false);

	SetValveState(CSM_PRIPROP_TANKA_VALVE, true);
	SetValveState(CSM_PRIPROP_TANKB_VALVE, true);
	SetValveState(CSM_PRIPROP_TANKC_VALVE, true);
	SetValveState(CSM_PRIPROP_TANKD_VALVE, true);

	SetValveState(CSM_SECPROP_TANKA_VALVE, true);
	SetValveState(CSM_SECPROP_TANKB_VALVE, true);
	SetValveState(CSM_SECPROP_TANKC_VALVE, true);
	SetValveState(CSM_SECPROP_TANKD_VALVE, true);
}

void Saturn::DeactivateCSMRCS()

{
	SetValveState(CSM_He1_TANKA_VALVE, true);
	SetValveState(CSM_He1_TANKB_VALVE, true);
	SetValveState(CSM_He1_TANKC_VALVE, true);
	SetValveState(CSM_He1_TANKD_VALVE, true);

	SetValveState(CSM_He2_TANKA_VALVE, true);
	SetValveState(CSM_He2_TANKB_VALVE, true);
	SetValveState(CSM_He2_TANKC_VALVE, true);
	SetValveState(CSM_He2_TANKD_VALVE, true);

	SetValveState(CSM_PRIPROP_TANKA_VALVE, false);
	SetValveState(CSM_PRIPROP_TANKB_VALVE, false);
	SetValveState(CSM_PRIPROP_TANKC_VALVE, false);
	SetValveState(CSM_PRIPROP_TANKD_VALVE, false);

	SetValveState(CSM_SECPROP_TANKA_VALVE, false);
	SetValveState(CSM_SECPROP_TANKB_VALVE, false);
	SetValveState(CSM_SECPROP_TANKC_VALVE, false);
	SetValveState(CSM_SECPROP_TANKD_VALVE, false);
}

//
// And CM
//

void Saturn::ActivateCMRCS()

{
	SetValveState(CM_RCSPROP_TANKA_VALVE, true);
	SetValveState(CM_RCSPROP_TANKB_VALVE, true);
}

void Saturn::DeactivateCMRCS()

{
	SetValveState(CM_RCSPROP_TANKA_VALVE, false);
	SetValveState(CM_RCSPROP_TANKB_VALVE, false);
}

bool Saturn::SMRCSAActive()

{
	return (!GetValveState(CSM_He1_TANKA_VALVE) && !GetValveState(CSM_He2_TANKA_VALVE) && GetValveState(CSM_PRIPROP_TANKA_VALVE) && GetValveState(CSM_SECPROP_TANKA_VALVE));
}

bool Saturn::SMRCSBActive()

{
	return (!GetValveState(CSM_He1_TANKB_VALVE) && !GetValveState(CSM_He2_TANKB_VALVE) && GetValveState(CSM_PRIPROP_TANKB_VALVE) && GetValveState(CSM_SECPROP_TANKB_VALVE));
}

bool Saturn::SMRCSCActive()

{
	return (!GetValveState(CSM_He1_TANKC_VALVE) && !GetValveState(CSM_He2_TANKC_VALVE) && GetValveState(CSM_PRIPROP_TANKC_VALVE) && GetValveState(CSM_SECPROP_TANKC_VALVE));
}

bool Saturn::SMRCSDActive()

{
	return (!GetValveState(CSM_He1_TANKD_VALVE) && !GetValveState(CSM_He2_TANKD_VALVE) && GetValveState(CSM_PRIPROP_TANKD_VALVE) && GetValveState(CSM_SECPROP_TANKD_VALVE));
}

bool Saturn::SMRCSActive()

{
	return SMRCSAActive() && SMRCSBActive() && SMRCSCActive() && SMRCSDActive();
}

void Saturn::CheckSPSState()

{
	switch (stage) {
	case CSM_LEM_STAGE:
		if (SPSswitch.IsUp()){
			SetThrusterResource(th_main[0],ph_sps);
			agc.SetInputChannelBit(030, 3, true);
		}
		else{
			SetThrusterResource(th_main[0],NULL);
			agc.SetInputChannelBit(030, 3, false);
		}
		break;
	}
}

//
// Check whether the CM RCS is active.
//

bool Saturn::CMRCSActive()

{
	//
	// I don't think this is correct, but I'm not sure yet what the CM RCS switches do.
	//

	return GetValveState(CM_RCSPROP_TANKA_VALVE) && GetValveState(CM_RCSPROP_TANKB_VALVE);
}

void Saturn::SetRCS_CM()

{
	int i;

	if (CMRCSActive()) {
		for (i = 0; i < 24; i++) 
			SetThrusterResource(th_att_cm[i], ph_rcs1);
	}
	else {
		for (i = 0; i < 24; i++)
			SetThrusterResource(th_att_cm[i], NULL);
	}

}

//
// Check the state of the RCS, and enable/disable thrusters as required.
//

void Saturn::CheckRCSState()

{
	int	i;

	//
	// For now we'll just turn all of the thrusters on or off. At some point
	// we should enable or disable them by thruster quad.
	//

	switch (stage) {
	case CSM_LEM_STAGE:
	case CSM_ABORT_STAGE:
		if (SMRCSActive()) {
			for (i = 0; i < 24; i++) {
				SetThrusterResource(th_att_rot[i], ph_rcs0);
				SetThrusterResource(th_att_lin[i], ph_rcs0);
			}
		}
		else {
			for (i = 0; i < 24; i++) {
				SetThrusterResource(th_att_rot[i], NULL);
				SetThrusterResource(th_att_lin[i], NULL);
			}
		}
		break;

	case CM_STAGE:
	case CM_ENTRY_STAGE:
	case CM_ENTRY_STAGE_TWO:
	case CM_ENTRY_STAGE_THREE:
	case CM_ENTRY_STAGE_FOUR:
	case CM_ENTRY_STAGE_FIVE:
	case CM_ENTRY_STAGE_SIX:
	case CM_ENTRY_STAGE_SEVEN:
		SetRCS_CM();
		break;
	}
}

void Saturn::ActivateSPS()

{
	SPSswitch.SetState(THREEPOSSWITCH_UP);
}

void Saturn::DeactivateSPS()

{
	SPSswitch.SetState(THREEPOSSWITCH_DOWN);
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

//
// Set all the saved Panel SDK pointers to 0.
//

void Saturn::ClearPanelSDKPointers()

{
	int	i;

	pCO2Level = 0;
	pCabinCO2Level = 0;
	pCabinPressure = 0;
	pSuitPressure = 0;
	pSuitReturnPressure = 0;
	pCabinTemp = 0;
	pSuitTemp = 0;
	pCabinRegulatorFlow = 0;
	pO2DemandFlow = 0;
	pDirectO2Flow = 0;
	pO2Tank1Press = 0;
	pO2Tank2Press = 0;
	pH2Tank1Press = 0;
	pH2Tank2Press = 0;
	pFC1Temp = 0;
	pFC2Temp = 0;
	pFC3Temp = 0;

	for (i = 0; i < N_CSM_VALVES; i++) {
		pCSMValves[i] = 0;
	}
}

//
// Functions that provide structured access to Saturn systems state.
//

//
// Get atmosphere status for CM.
//

void Saturn::GetAtmosStatus(AtmosStatus &atm)

{
	atm.CabinCO2MMHG = 0.0;
	atm.SuitCO2MMHG = 0.0;
	atm.CabinPressureMMHG = 0.0;
	atm.CabinPressurePSI = 0.0;
	atm.CabinTempK = 0.0;
	atm.SuitPressureMMHG = 0.0;
	atm.SuitPressurePSI = 0;
	atm.CabinRegulatorFlowLBH = 0.0;
	atm.DirectO2FlowLBH = 0.0;
	atm.O2DemandFlowLBH = 0.0;
	atm.SuitReturnPressureMMHG = 0.0;
	atm.SuitReturnPressurePSI = 0.0;

	if (!pCO2Level) {
		pCO2Level = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUIT:CO2_PPRESS");
	}

	if (pCO2Level) {
		atm.SuitCO2MMHG = (*pCO2Level) * MMHG;
	}

	if (!pCabinCO2Level) {
		pCabinCO2Level = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	}

	if (pCabinCO2Level) {
		atm.CabinCO2MMHG = (*pCO2Level) * MMHG;
	}

	if (!pCabinPressure) {
		pCabinPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	}

	if (pCabinPressure) {
		atm.CabinPressureMMHG = (*pCabinPressure) * MMHG;
		atm.CabinPressurePSI = (*pCabinPressure) * PSI;
	}

	if (!pSuitPressure) {
		pSuitPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUIT:PRESS");
	}

	if (pSuitPressure) {
		atm.SuitPressureMMHG = (*pSuitPressure) * MMHG;
		atm.SuitPressurePSI = (*pSuitPressure) * PSI;
	}

	if (!pSuitReturnPressure) {
		pSuitReturnPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS");
	}

	if (pSuitReturnPressure) {
		atm.SuitReturnPressureMMHG = (*pSuitReturnPressure) * MMHG;
		atm.SuitReturnPressurePSI = (*pSuitReturnPressure) * PSI;
	}

	if (!pSuitTemp) {
		pSuitTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUIT:TEMP");
	}

	if (pSuitTemp) {
		atm.SuitTempK = (*pSuitTemp);
	}

	if (!pCabinTemp) {
		pCabinTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	}

	if (pCabinTemp) {
		atm.CabinTempK = (*pCabinTemp);
	}

	if (!pCabinRegulatorFlow) {
		pCabinRegulatorFlow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOW");
	}

	if (pCabinRegulatorFlow) {
		atm.CabinRegulatorFlowLBH = (*pCabinRegulatorFlow) * LBH;
	}

	if (!pO2DemandFlow) {
		pO2DemandFlow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOW");
	}

	if (pO2DemandFlow) {
		atm.O2DemandFlowLBH = (*pO2DemandFlow) * LBH;
	}

	if (!pDirectO2Flow) {
		pDirectO2Flow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE:FLOW");
	}

	if (pDirectO2Flow) {
		atm.DirectO2FlowLBH = (*pDirectO2Flow) * LBH;
	}
}

//
// Get H2/O2 tank pressures.
//

void Saturn::GetTankPressures(TankPressures &press)

{
	//
	// Clear to defaults.
	//

	press.H2Tank1PressurePSI = 0.0;
	press.H2Tank2PressurePSI = 0.0;
	press.O2Tank1PressurePSI = 0.0;
	press.O2Tank2PressurePSI = 0.0;

	//
	// No tanks if we've seperated from the SM
	//

	if (stage >= CM_STAGE) {
		return;
	}

	//
	// Hydrogen tanks.
	//

	if (!pH2Tank1Press) {
		pH2Tank1Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:PRESS");
	}

	if (pH2Tank1Press) {
		press.H2Tank1PressurePSI = (*pH2Tank1Press) * PSI;
	}

	if (!pH2Tank2Press) {
		pH2Tank2Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:PRESS");
	}

	if (pH2Tank2Press) {
		press.H2Tank2PressurePSI = (*pH2Tank2Press) * PSI;
	}

	//
	// Oxygen tanks.
	//

	if (!pO2Tank1Press) {
		pO2Tank1Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:PRESS");
	}

	if (pO2Tank1Press) {
		press.O2Tank1PressurePSI = (*pO2Tank1Press) * PSI;
	}

	if (!pO2Tank2Press) {
		pO2Tank2Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:PRESS");
	}

	if (pO2Tank2Press) {
		press.O2Tank2PressurePSI = (*pO2Tank2Press) * PSI;
	}
}

//
// Get fuel cell status. We should expand this to also return pressure, etc.
//

void Saturn::GetFuelCellStatus(FuelCellStatus &fc)

{
	//
	// Set defaults.
	//

	fc.FC1TempK = 0.0;
	fc.FC2TempK = 0.0;
	fc.FC3TempK = 0.0;

	//
	// No fuel cells if we've seperated from the SM.
	//

	if (stage >= CM_STAGE) {
		return;
	}

	//
	// Get temperatures.
	//

	if (!pFC1Temp) {
		pFC1Temp = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:TEMP");
	}

	if (pFC1Temp) {
		fc.FC1TempK = (*pFC1Temp);
	}

	if (!pFC2Temp) {
		pFC2Temp = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:TEMP");
	}

	if (pFC2Temp) {
		fc.FC2TempK = (*pFC2Temp);
	}

	if (!pFC3Temp) {
		pFC3Temp = (double*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:TEMP");
	}

	if (pFC3Temp) {
		fc.FC3TempK = (*pFC3Temp);
	}
}

//
// Open and close valves.
//

void Saturn::SetValveState(int valve, bool open)

{
	ValveState[valve] = open;

	int valve_state = open ? SP_VALVE_OPEN : SP_VALVE_CLOSE;

	if (pCSMValves[valve])
		*pCSMValves[valve] = valve_state;

	CheckRCSState();
}

bool Saturn::GetValveState(int valve)

{
	if (pCSMValves[valve])
		return (*pCSMValves[valve] == SP_VALVE_OPEN);

	return ValveState[valve];
}

