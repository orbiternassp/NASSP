/****************************************************************************
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
  *	Revision 1.114  2006/06/27 11:25:57  tschachim
  *	Bugfix
  *	
  *	Revision 1.113  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.112  2006/06/17 18:18:00  tschachim
  *	Bugfixes SCS automatic modes,
  *	Changed quickstart separation key to J.
  *	
  *	Revision 1.111  2006/06/12 20:47:36  movieman523
  *	Made switch lighting optional based on REALISM, and fixed SII SEP light.
  *	
  *	Revision 1.110  2006/06/11 14:45:36  movieman523
  *	Quick fix for Apollo 4. Will need more work in the future.
  *	
  *	Revision 1.109  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.108  2006/06/08 17:02:24  tschachim
  *	Added SCS checklist actions.
  *	
  *	Revision 1.107  2006/06/08 15:30:18  tschachim
  *	Fixed ASCP and some default switch positions.
  *	
  *	Revision 1.106  2006/06/07 09:53:20  tschachim
  *	Improved ASCP and GDC align button, added cabin closeout sound, bugfixes.
  *	
  *	Revision 1.105  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.104  2006/05/30 14:40:21  tschachim
  *	Fixed fuel cell - dc bus connectivity, added battery charger
  *	
  *	Revision 1.103  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.102  2006/05/17 03:45:12  dseagrav
  *	Corrected GDC attitude determination in RATE1/RATE2 modes, corrected ECA handling of negative attitude input
  *	
  *	Revision 1.101  2006/04/25 13:48:53  tschachim
  *	Removed GetXXXSwitchState.
  *	
  *	Revision 1.100  2006/04/23 07:14:01  dseagrav
  *	Holding mouse key down causes ASCP to advance until mouse key is released.
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
#include "tracer.h"

//FILE *PanelsdkLogFile;

// DS20060302 DX8 callback for enumerating joysticks
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pSaturn)
{
	class Saturn * sat = (Saturn*)pSaturn; // Pointer to us
	HRESULT hr;

	if(sat->js_enabled > 1){  // Do we already have enough joysticks?
		return DIENUM_STOP; } // If so, stop enumerating additional devices.

	// Obtain an interface to the enumerated joystick.
    hr = sat->dx8ppv->CreateDevice(pdidInstance->guidInstance, &sat->dx8_joystick[sat->js_enabled], NULL);
	
	if(FAILED(hr)) {              // Did that work?
		return DIENUM_CONTINUE; } // No, keep enumerating (if there's more)

	sat->js_enabled++;      // Otherwise, Next!
	return DIENUM_CONTINUE; // and keep enumerating
}

void Saturn::SystemsInit() {

	// default state
	systemsState = SATSYSTEMS_NONE;
	lastSystemsMissionTime = MINUS_INFINITY;
	firstSystemsTimeStepDone = false;

	// initialize SPSDK
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo\\SaturnSystems");

	//PanelsdkLogFile = fopen("ProjectApollo Saturn Systems.log", "w");

	//
	// Electrical systems.
	// First wire up buses to the Panel SDK.
	//

	e_object *eo;

	//
	// Inverters
	//
	Inverter1 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_1");
	Inverter2 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_2");
	Inverter3 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_3");

	Inverter1->WireTo(&InverterControl1CircuitBraker);
	Inverter2->WireTo(&InverterControl2CircuitBraker);
	Inverter3->WireTo(&InverterControl3CircuitBraker);

	//
	// AC Bus 1
	//
	ACBus1Source.WireToBuses(&AcBus1Switch1, &AcBus1Switch2, &AcBus1Switch3);

	ACBus1PhaseA.WireTo(&ACBus1Source);
	ACBus1PhaseB.WireTo(&ACBus1Source);
	ACBus1PhaseC.WireTo(&ACBus1Source);
	ACBus1.WireToBuses(&ACBus1PhaseA, &ACBus1PhaseB, &ACBus1PhaseC);

	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:AC_1");
	eo->WireTo(&ACBus1);

	//
	// AC Bus 2
	//
	ACBus2Source.WireToBuses(&AcBus2Switch1, &AcBus2Switch2, &AcBus2Switch3);

	ACBus2PhaseA.WireTo(&ACBus2Source);
	ACBus2PhaseB.WireTo(&ACBus2Source);
	ACBus2PhaseC.WireTo(&ACBus2Source);
	ACBus2.WireToBuses(&ACBus2PhaseA, &ACBus2PhaseB, &ACBus2PhaseC);
	
	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:AC_2");
	eo->WireTo(&ACBus2);

	//
	// For now we register these systems so that the panel SDK will update
	// them each timestep. Otherwise we get infinitely increasing current
	// if we're not careful.
	//

	Panelsdk.AddElectrical(&ACBus1PhaseA, false);
	Panelsdk.AddElectrical(&ACBus1PhaseB, false);
	Panelsdk.AddElectrical(&ACBus1PhaseC, false);
	Panelsdk.AddElectrical(&ACBus2PhaseA, false);
	Panelsdk.AddElectrical(&ACBus2PhaseB, false);
	Panelsdk.AddElectrical(&ACBus2PhaseC, false);

	//
	// Fuel cells.
	//

	FuelCells[0] = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1");
	FuelCells[1] = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2");
	FuelCells[2] = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3");

	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING");
	eo->WireTo(&FuelCellPumps1Switch);
	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2COOLING");
	eo->WireTo(&FuelCellPumps2Switch);
	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3COOLING");
	eo->WireTo(&FuelCellPumps3Switch);

	//
	// O2 tanks.
	//

	O2Tanks[0] = (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1");
	O2Tanks[1] = (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2");

	//
	// Entry and landing batteries.
	//

	EntryBatteryA = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_A");
	EntryBatteryB = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_B");
	EntryBatteryC = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_C");

	//
	// Pyro batteries.
	//

	PyroBatteryA = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_PYRO_A");
	PyroBatteryB = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_PYRO_B");

	//
	// Wire battery buses to batteries.
	//

	BatteryBusA.WireToBuses(EntryBatteryA, NULL, NULL);		// TODO Bat C can be connected to BatBus A via the BAT C TO BAT BUS A cb on panel 250, which is currently not available 
	BatteryBusB.WireToBuses(EntryBatteryB, NULL, NULL);		// TODO Bat C can be connected to BatBus B via the BAT C TO BAT BUS B cb on panel 250, which is currently not available

	PyroBusA.WireToBuses(EntryBatteryA, PyroBatteryA);
	PyroBusB.WireToBuses(EntryBatteryB, PyroBatteryB);

	BatteryRelayBus.WireToBuses( &BATRLYBusBatACircuitBraker, &BATRLYBusBatBCircuitBraker);

	//
	// Main Buses
	//

	MainBusA = (DCbus *) Panelsdk.GetPointerByString("ELECTRIC:DC_A");
	MainBusB = (DCbus *) Panelsdk.GetPointerByString("ELECTRIC:DC_B");
	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_GSE");

	MainBusA->WireTo(MainBusAController.GetBusSource());
	MainBusB->WireTo(MainBusBController.GetBusSource());

	MainBusAController.Init(FuelCells[0], FuelCells[1], FuelCells[2],
		                    &BatteryBusA, EntryBatteryC, eo);	// TODO Bat C should be connected via the MAIN A - BAT C cb on panel 275, which is currently not available

	MainBusBController.Init(FuelCells[0], FuelCells[1], FuelCells[2],
		                    &BatteryBusB, EntryBatteryC, eo);	// TODO Bat C should be connected via the MAIN B - BAT C cb on panel 275, which is currently not available
	
	MainBusAController.ConnectFuelCell(2, true);	// Default state of MainBusASwitch2

	//
	// Battery Charger
	//

	BatteryCharger.Init(EntryBatteryA, EntryBatteryB, EntryBatteryC,
		                &BatteryChargerBatACircuitBraker, &BatteryChargerBatBCircuitBraker, EntryBatteryC,
						&BatteryChargerMnACircuitBraker, &BatteryChargerMnBCircuitBraker, &BatteryChargerAcPwrCircuitBraker);

	EntryBatteryA->WireTo(&BatteryChargerBatACircuitBraker);
	EntryBatteryB->WireTo(&BatteryChargerBatBCircuitBraker);

	//
	// Generic power source for switches, tied to both Bus A and
	// Bus B.
	//

	SwitchPower.WireToBuses(MainBusA, MainBusB);
	GaugePower.WireToBuses(MainBusA, MainBusB);

	PyroPower.WireToBuses(&PyroArmASwitch, &PyroArmBSwitch);
	SECSLogicPower.WireToBuses(&Logic1Switch, &Logic2Switch);

	//
	// ECS devices
	//

	PrimCabinHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMCABINHEATEXCHANGER");
	SecCabinHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:SECCABINHEATEXCHANGER");
	PrimEcsRadiatorExchanger1 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER1");
	PrimEcsRadiatorExchanger2 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER2");
	CabinHeater = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:CABINHEATER");

	SuitCompressor1 = (AtmRegen *) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER1");
	SuitCompressor1->WireTo(&SuitCompressor1Switch);
	SuitCompressor2 = (AtmRegen *) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER2");
	SuitCompressor2->WireTo(&SuitCompressor2Switch);

	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:SECGLYCOLPUMP");
	eo->WireTo(&SecCoolantLoopPumpSwitch);

	//
	// Wire up internal systems.
	//

	cws.WireTo(&CWMnaCircuitBraker, &CWMnbCircuitBraker);
	agc.WirePower(&GNComputerMnACircuitBraker, &GNComputerMnBCircuitBraker);
	imu.WireToBuses(&GNIMUMnACircuitBraker, &GNIMUMnBCircuitBraker);
	imu.WireHeaterToBuses((Boiler *) Panelsdk.GetPointerByString("ELECTRIC:IMUHEATER"), &GNIMUHTRMnACircuitBraker, &GNIMUHTRMnBCircuitBraker);
	dockingprobe.WireTo(&DockProbeMnACircuitBraker, &DockProbeMnBCircuitBraker);   

	//
	// Default valve states. For now, everything starts closed.
	//
	
	SMRCSHelium1ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium1BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSHelium1CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium1DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSHelium2ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium2BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSHelium2CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium2DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSProp1ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp1BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSProp1CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp1DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSProp2ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp2BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSProp2CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp2DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSHelium1ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium1BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSHelium1CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium1DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSHelium2ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium2BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSHelium2CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium2DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSProp1ATalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSProp1BTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSProp1CTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSProp1DTalkback.WireTo(&SMHeatersAMnBCircuitBraker);

	SMRCSProp2ATalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSProp2BTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSProp2CTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSProp2DTalkback.WireTo(&SMHeatersAMnBCircuitBraker);

	SMRCSHelium1ATalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSHelium1BTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSHelium1CTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSHelium1DTalkback.WireTo(&SMHeatersAMnBCircuitBraker);

	SMRCSHelium2ATalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSHelium2BTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSHelium2CTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSHelium2DTalkback.WireTo(&SMHeatersAMnBCircuitBraker);

	CMRCSIsolate1.WireTo(&PrplntIsolMnACircuitBraker);
	CMRCSIsolate2.WireTo(&PrplntIsolMnBCircuitBraker);

	CMRCSIsolate1Talkback.WireTo(&SMHeatersBMnACircuitBraker);
	CMRCSIsolate2Talkback.WireTo(&SMHeatersAMnBCircuitBraker);

	SetValveState(CSM_He1_TANKA_VALVE, false);
	SetValveState(CSM_He1_TANKB_VALVE, false);
	SetValveState(CSM_He1_TANKC_VALVE, false);
	SetValveState(CSM_He1_TANKD_VALVE, false);

	SetValveState(CSM_He2_TANKA_VALVE, false);
	SetValveState(CSM_He2_TANKB_VALVE, false);
	SetValveState(CSM_He2_TANKC_VALVE, false);
	SetValveState(CSM_He2_TANKD_VALVE, false);

	SetValveState(CSM_PRIOXID_INSOL_VALVE_A, false);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_B, false);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_C, false);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_D, false);

	SetValveState(CSM_SECOXID_INSOL_VALVE_A, false);
	SetValveState(CSM_SECOXID_INSOL_VALVE_B, false);
	SetValveState(CSM_SECOXID_INSOL_VALVE_C, false);
	SetValveState(CSM_SECOXID_INSOL_VALVE_D, false);

	SetValveState(CSM_PRIFUEL_INSOL_VALVE_A, false);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_B, false);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_C, false);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_D, false);

	SetValveState(CSM_SECFUEL_INSOL_VALVE_A, false);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_B, false);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_C, false);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_D, false);

	SetValveState(CSM_SECFUEL_PRESS_VALVE_A, false);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_B, false);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_C, false);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_D, false);

	SetValveState(CM_RCSPROP_TANKA_VALVE, false);
	SetValveState(CM_RCSPROP_TANKB_VALVE, false);
	// DS20060226 SPS Gimbal reset to zero
	sps_pitch_position = 0;
	sps_yaw_position = 0;

	// DS20060304 SCS initialization
	bmag1.Init(this, &SystemMnACircuitBraker, &StabContSystemAc1CircuitBraker, (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:BMAGHEATER1"));
	bmag2.Init(this, &SystemMnBCircuitBraker, &StabContSystemAc2CircuitBraker, (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:BMAGHEATER2"));
	gdc.Init(this);
	ascp.Init(this);
	eda.Init(this);
	rjec.Init(this);
	eca.Init(this);
	// DS20060326 Telecom initialization
	pcm.Init(this);

	// DS20060301 Initialize joystick
	HRESULT         hr;
	js_enabled = 0;  // Disabled
	rhc_id = -1;     // Disabled
	rhc_rot_id = -1; // Disabled
	rhc_sld_id = -1; // Disabled
	rhc_rzx_id = -1; // Disabled
	thc_id = -1;     // Disabled
	thc_rot_id = -1; // Disabled
	thc_sld_id = -1; // Disabled
	thc_rzx_id = -1; // Disabled
	thc_debug = -1;
	rhc_debug = -1;

	FILE *fd;
	// Open configuration file
	fd = fopen("Config\\ProjectApollo\\Joystick.INI","r");
	if(fd != NULL){ // Did that work?
		char dataline[256];
		char *token;
		char *parameter;
		rhc_id = 0; // Trap!
		while(!feof(fd)){
			fgets(dataline,256,fd); // Yes, so read a line
			// Get a token.
			token = strtok(dataline," \r\n");
			if(token != NULL){                                  // If it's not null, parse.
				if(strncmp(token,"RHC",3)==0){                  // RHC address?
					// Get next token, which should be JS number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_id = atoi(parameter);
						if(rhc_id > 1){ rhc_id = 1; } // Be paranoid
					}
				}
				if(strncmp(token,"RRT",3)==0){                  // RHC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_rot_id = atoi(parameter);
						if(rhc_rot_id > 2){ rhc_rot_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"RSL",3)==0){                  // RHC SLIDER address?
					// Get next token, which should be SLIDER number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_sld_id = atoi(parameter);
						if(rhc_sld_id > 2){ rhc_sld_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"RZX",3)==0){                  // RHC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						rhc_rzx_id = atoi(parameter);
						if(rhc_rzx_id > 1){ rhc_rzx_id = 1; } // Be paranoid
					}
				}
				/* *** THC *** */
				if(strncmp(token,"THC",3)==0){                  // THC address?
					// Get next token, which should be JS number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_id = atoi(parameter);
						if(thc_id > 1){ thc_id = 1; } // Be paranoid
					}
				}
				if(strncmp(token,"TRT",3)==0){                  // THC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_rot_id = atoi(parameter);
						if(thc_rot_id > 2){ thc_rot_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"TSL",3)==0){                  // THC SLIDER address?
					// Get next token, which should be SLIDER number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_sld_id = atoi(parameter);
						if(thc_sld_id > 2){ thc_sld_id = 2; } // Be paranoid
					}
				}
				if(strncmp(token,"TZX",3)==0){                  // THC ROTATOR address?
					// Get next token, which should be ROTATOR number
					parameter = strtok(NULL," \r\n");
					if(parameter != NULL){
						thc_rzx_id = atoi(parameter);
						if(thc_rzx_id > 1){ thc_rzx_id = 1; } // Be paranoid
					}
				}
				if(strncmp(token,"RDB",3)==0){					// RHC debug					
					rhc_debug = 1;
				}
				if(strncmp(token,"TDB",3)==0){					// THC debug					
					thc_debug = 1;
				}
			}			
		}		
		fclose(fd);
		// Having read the configuration file, set up DirectX...	
		hr = DirectInput8Create(dllhandle,DIRECTINPUT_VERSION,IID_IDirectInput8,(void **)&dx8ppv,NULL); // Give us a DirectInput context
		if(!FAILED(hr)){
			int x=0;
			// Enumerate attached joysticks until we find 2 or run out.
			dx8ppv->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
			if(js_enabled == 0){   // Did we get anything?			
				dx8ppv->Release(); // No. Close down DirectInput
				dx8ppv = NULL;     // otherwise it won't get closed later
				sprintf(oapiDebugString(),"DX8JS: No joysticks found");
			}else{
				while(x < js_enabled){                                // For each joystick
					dx8_joystick[x]->SetDataFormat(&c_dfDIJoystick2); // Use DIJOYSTATE2 structure to report data
					/* Can't do this because we don't own a window.
					dx8_joystick[x]->SetCooperativeLevel(dllhandle,   // We want data all the time,
						DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);		  // and we don't need exclusive joystick access.
						*/ 
					dx8_jscaps[x].dwSize = sizeof(dx8_jscaps[x]);     // Initialize size of capabilities data structure
					dx8_joystick[x]->GetCapabilities(&dx8_jscaps[x]); // Get capabilities
					x++;                                              // Next!
				}
			}
		}else{
			// We can't print an error message this early in initialization, so save this reason for later investigation.
			dx8_failure = hr;
		}
	}
}

void Saturn::SystemsTimestep(double simt, double simdt) {

	//
	// Don't clock the computer and the internal systems unless we're actually at pre-launch.
	//

	if (stage == ONPAD_STAGE && MissionTime >= -10800) {	// 3h 00min before launch
		// Slow down time acceleration
		if (Realism && oapiGetTimeAcceleration() > 1.0)
			oapiSetTimeAcceleration(1.0);

		stage = PRELAUNCH_STAGE;
	}
	else if (stage >= PRELAUNCH_STAGE) {

		//
		// Timestep the internal systems, there can be multiple systems timesteps in one Orbiter timestep
		//

		SystemsInternalTimestep(simdt);

		//
		// Do the "normal" Orbiter timestep
		//

		dsky.Timestep(MissionTime);
		dsky2.Timestep(MissionTime);
		agc.Timestep(MissionTime, simdt);
		imu.Timestep(MissionTime);

		//
		// If we've seperated from the SIVb, the IU is history.
		//
		if (stage < CSM_LEM_STAGE)
		{
			iu.Timestep(MissionTime, simdt);
		}	

		// DS20060304 SCS updation
		bmag1.Timestep(simdt);
		bmag2.Timestep(simdt);
		ascp.TimeStep();
		gdc.Timestep(simdt);
		eca.TimeStep();
		rjec.TimeStep();

		cws.TimeStep(MissionTime);
		dockingprobe.TimeStep(MissionTime, simdt);
		secs.Timestep(MissionTime, simdt);
		fdaiLeft.Timestep(MissionTime, simdt);
		fdaiRight.Timestep(MissionTime, simdt);
		JoystickTimestep();

		// DS20060326 Telecom updation - This is last so telemetry reflects the current state.
		pcm.TimeStep(MissionTime);

		//
		// Systems state handling
		//
		if (!firstSystemsTimeStepDone) {
			firstSystemsTimeStepDone = true;
		}
		else {
			AtmosStatus atm;
			GetAtmosStatus(atm);

			double *pMax, *fMax, scdp;
			float *size, *pz;
			int *open, *number, *isopen;

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
				
				// Close O2 demand regulator
				open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT2:OPEN");
				*open = SP_VALVE_CLOSE;

				// Close cabin to suit circuit return value
				open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
				*open = SP_VALVE_CLOSE;

				// Open suit pressure relieve
				open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:OPEN");
				*open = SP_VALVE_OPEN;

				// Primary ECS radiators in prelaunch configuration, TODO secondary
				PrimEcsRadiatorExchanger1->SetLength(8.0);
				PrimEcsRadiatorExchanger2->SetLength(8.0);

				// GSE provides electrical power
				MainBusAController.SetGSEState(1);
				MainBusBController.SetGSEState(1);

				// Reduce fuel cell cooling power because of low fuel cell load
				*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR1:RAD") = 3.0;
				*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR2:RAD") = 3.0;
				*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR3:RAD") = 3.0;


				//
				// Checklist actions
				//
				
				// Activate CMC
				GNComputerMnACircuitBraker.SwitchTo(TOGGLESWITCH_UP);
				GNComputerMnBCircuitBraker.SwitchTo(TOGGLESWITCH_UP);

				// Activate IMU
				GNIMUHTRMnACircuitBraker.SwitchTo(TOGGLESWITCH_UP);
				GNIMUHTRMnBCircuitBraker.SwitchTo(TOGGLESWITCH_UP);
				GNIMUMnACircuitBraker.SwitchTo(TOGGLESWITCH_UP);
				GNIMUMnBCircuitBraker.SwitchTo(TOGGLESWITCH_UP);

				CautionWarningPowerSwitch.SwitchTo(THREEPOSSWITCH_UP);
				// Avoid master alarm because of power on
				cws.SetMasterAlarm(false);
				// Avoid suit compressor alarm 
				if (!Realism)
					cws.SetInhibitNextMasterAlarm(true);
				// Switch directly to launch configuration
				// To do the complete checklist next would be the lamp tests... 
				CautionWarningModeSwitch.SwitchTo(THREEPOSSWITCH_CENTER);

				// Activate primary water-glycol coolant loop
				EcsGlycolPumpsSwitch.SwitchTo(1);
				EcsRadiatorsFlowContPwrSwitch.SwitchTo(THREEPOSSWITCH_UP);
				EcsRadiatorsFlowContPwrSwitch.SwitchTo(THREEPOSSWITCH_CENTER);
				SuitCircuitHeatExchSwitch.SwitchTo(THREEPOSSWITCH_UP);
				SuitCircuitHeatExchSwitch.SwitchTo(THREEPOSSWITCH_CENTER);

				// Activate FDAIs etc.
				FDAIPowerRotarySwitch.SwitchTo(3);
				SCSElectronicsPowerRotarySwitch.SwitchTo(2);
				SIGCondDriverBiasPower1Switch.SwitchTo(THREEPOSSWITCH_UP);
				SIGCondDriverBiasPower2Switch.SwitchTo(THREEPOSSWITCH_DOWN);

				// Start mission timer
				MissionTimerSwitch.SwitchTo(THREEPOSSWITCH_UP);

				// Open Direct O2 valve
				DirectO2RotarySwitch.SwitchTo(2);

				
				// Next state
				systemsState = SATSYSTEMS_PRELAUNCH;
				lastSystemsMissionTime = MissionTime; 
				break;

			case SATSYSTEMS_PRELAUNCH:
				if (MissionTime >= -6000) {	// 1h 40min before launch

					// Slow down time acceleration
					if (Realism && oapiGetTimeAcceleration() > 1.0)
						oapiSetTimeAcceleration(1.0);

					// Play cabin closeout sound
					CabincloseoutS.play();
					CabincloseoutS.done();

					// Crew ingress
					number = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CREW:NUMBER");
					*number = 3; 

					// Close cabin pressure regulator 
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT:OPEN");
					*open = SP_VALVE_CLOSE;

					// Suit compressors to prelaunch configuration
					SuitCompressor1->fan_cap = 110000.0;
					SuitCompressor2->fan_cap = 110000.0;

					// Open cabin to suit circuit return value
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
					*open = SP_VALVE_OPEN;
					pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNINLET:PRESSMAX");
					*pMax = 100.0 / PSI;	// that's like disabling PREG
				

					//
					// Checklist actions
					//
		
					// Open Direct O2 valve
					DirectO2RotarySwitch.SwitchTo(1);

					// Turn on suit compressor 1
					SuitCompressor1Switch.SwitchTo(THREEPOSSWITCH_UP);

					// Turn on water accumulator
					SuitCircuitH2oAccumAutoSwitch.SwitchTo(THREEPOSSWITCH_UP);


					// Next state
					systemsState = SATSYSTEMS_CREWINGRESS_1;
					lastSystemsMissionTime = MissionTime; 
				}	
				break;

			case SATSYSTEMS_CREWINGRESS_1:
				// Suit compressors running?
				if (!SuitCompressor1->pumping && !SuitCompressor2->pumping) {
					lastSystemsMissionTime = MissionTime; 
				} else {
					scdp = (atm.SuitReturnPressurePSI - atm.CabinPressurePSI) * (INH2O / PSI);
					if ((scdp > 0.0 && MissionTime - lastSystemsMissionTime >= 50) || stage > PRELAUNCH_STAGE) {	// Suit Cabin delta p is established

						// Open cabin pressure regulator, max flow to 0.25 lb/h  
						open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT:OPEN");
						*open = SP_VALVE_OPEN;
						fMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOWMAX");
						*fMax = 0.25 / LBH; 

						// Close cabin to suit circuit return value
						open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
						*open = SP_VALVE_CLOSE;
						pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNINLET:PRESSMAX");
						*pMax = 4.95 / PSI;

						// Next state
						systemsState = SATSYSTEMS_CREWINGRESS_2;
						lastSystemsMissionTime = MissionTime; 
					}
				}
				break;

			case SATSYSTEMS_CREWINGRESS_2:
				scdp = (atm.SuitReturnPressurePSI - atm.CabinPressurePSI) * (INH2O / PSI);
				if ((scdp > 1.3 && MissionTime - lastSystemsMissionTime >= 10) || stage > PRELAUNCH_STAGE) {	// Suit Cabin delta p is established

					// Cabin leak
					size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
					*size = (float) 0.0002; 
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:OPEN");
					*open = SP_VALVE_OPEN;


					// Next state
					systemsState = SATSYSTEMS_CABINCLOSEOUT;
					lastSystemsMissionTime = MissionTime; 
				}	
				break;

			case SATSYSTEMS_CABINCLOSEOUT:
				if (MissionTime >= -1200) {	// 20min before launch

					//
					// Checklist actions
					//

					// Turn on BMAGs (AOH2 4.2.2.1)
					BMAGPowerRotary1Switch.SwitchTo(2);
					BMAGPowerRotary2Switch.SwitchTo(2);

					// TVC check, power buses (AOH2 4.2.2.2)
					MainBusASwitch1.SwitchTo(THREEPOSSWITCH_UP);
					MainBusASwitch2.SwitchTo(THREEPOSSWITCH_UP);
					MainBusBSwitch3.SwitchTo(THREEPOSSWITCH_UP);

					// Turn on sequencial logic and arm pyros (AOH2 4.2.2.6)
					ArmBatACircuitBraker.SwitchTo(TOGGLESWITCH_UP);
					ArmBatBCircuitBraker.SwitchTo(TOGGLESWITCH_UP);					
					LogicBatACircuitBraker.SwitchTo(TOGGLESWITCH_UP);
					LogicBatBCircuitBraker.SwitchTo(TOGGLESWITCH_UP);

					Logic1Switch.SwitchTo(TOGGLESWITCH_UP);
					Logic2Switch.SwitchTo(TOGGLESWITCH_UP);
					PyroArmASwitch.SwitchTo(TOGGLESWITCH_UP);
					PyroArmBSwitch.SwitchTo(TOGGLESWITCH_UP);

					// Turn off cyro fans
					H2Fan1Switch.SwitchTo(THREEPOSSWITCH_CENTER);
					H2Fan2Switch.SwitchTo(THREEPOSSWITCH_CENTER);
					O2Fan1Switch.SwitchTo(THREEPOSSWITCH_CENTER);
					O2Fan2Switch.SwitchTo(THREEPOSSWITCH_CENTER);

					// Turn on SM RCS (AOH2 4.2.2.7)
					SMRCSHelium1ASwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSHelium1BSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSHelium1CSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSHelium1DSwitch.SwitchTo(THREEPOSSWITCH_UP); 

					SMRCSHelium2ASwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSHelium2BSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSHelium2CSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSHelium2DSwitch.SwitchTo(THREEPOSSWITCH_UP); 

					SMRCSProp1ASwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSProp1BSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSProp1CSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSProp1DSwitch.SwitchTo(THREEPOSSWITCH_UP); 

					SMRCSProp2ASwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSProp2BSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSProp2CSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					SMRCSProp2DSwitch.SwitchTo(THREEPOSSWITCH_UP); 
					

					// Next state
					systemsState = SATSYSTEMS_GSECONNECTED_1;
					lastSystemsMissionTime = MissionTime; 
				}
				break;	

			case SATSYSTEMS_GSECONNECTED_1:
				if (MissionTime >= -900) {	// 15min before launch

					// Disable GSE electrical power 
					// Reference: Apollo 15 Flight Journal (http://history.nasa.gov/ap15fj/01launch_to_earth_orbit.htm)
					MainBusAController.SetGSEState(0);
					MainBusBController.SetGSEState(0);

					// Set fuel cell cooling power to normal
					*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR1:RAD") = 6.8;
					*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR2:RAD") = 6.8;
					*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR3:RAD") = 6.8;


					//
					// Checklist actions
					//

					// EDS auto on (AOH2 4.2.3)
					EDSSwitch.SwitchTo(TOGGLESWITCH_UP);
					
					// Latch FC valves
					FCReacsValvesSwitch.SwitchTo(TOGGLESWITCH_DOWN);


					// Next state
					systemsState = SATSYSTEMS_GSECONNECTED_2;
					lastSystemsMissionTime = MissionTime; 
				}
				break;

			case SATSYSTEMS_GSECONNECTED_2:
				if (MissionTime >= -135) {	// 2min 15sec before launch
					// Disable GSE devices
					*(int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMGSEHEATEXCHANGER:PUMP") = SP_PUMP_OFF;
					*(int*) Panelsdk.GetPointerByString("HYDRAULIC:SECGSEHEATEXCHANGER:PUMP") = SP_PUMP_OFF;


					//
					// Checklist actions
					//

					// Bypass primary radiators
					GlycolToRadiatorsLever.SwitchTo(TOGGLESWITCH_DOWN);

					// Tie batteries to buses
					MainBusTieBatAcSwitch.SwitchTo(THREEPOSSWITCH_UP);
					MainBusTieBatBcSwitch.SwitchTo(THREEPOSSWITCH_UP);

					// Next state
					systemsState = SATSYSTEMS_READYTOLAUNCH;
					lastSystemsMissionTime = MissionTime; 
				}
				break;

			case SATSYSTEMS_READYTOLAUNCH:
				if (GetAtmPressure() <= 6.0 / PSI) {
					// Cabin pressure relieve
					size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
					*size = (float) 0.22; // 0.25;
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:OPEN");
					*open = SP_VALVE_OPEN;
					
					// Cabin pressure regulator to 5 psi
					pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:PRESSMAX");
					*pMax = 5.0 / PSI;

					// Disable cabin pressure regulator max flow
					fMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOWMAX");
					*fMax = 0;

					// Open cabin to suit circuit return value
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
					*open = SP_VALVE_OPEN;

					// Suit pressure relieve max./min. pressure
					pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITRELIEFVALVE:PRESSMAX");
					*pMax = 0;
					pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITRELIEFVALVE:PRESSMIN");
					*pMax = 0;
					size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:SIZE");
					*size = (float) 0.08;

					// Suit compressors to flight configuration
					SuitCompressor1->fan_cap = 65000.0;
					SuitCompressor2->fan_cap = 65000.0;

					// Primary ECS radiators now working normally, TODO secondary
					PrimEcsRadiatorExchanger1->SetLength(10.0);
					PrimEcsRadiatorExchanger2->SetLength(10.0);

					// Next state
					systemsState = SATSYSTEMS_CABINVENTING;
					lastSystemsMissionTime = MissionTime; 
				}		
				break;

			case SATSYSTEMS_CABINVENTING:

				size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
				open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:OPEN");
				isopen = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:ISOPEN");
				pz = (float*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:LEAK:PZ");

				if (atm.CabinPressurePSI <= 5.0) {
					// cabin leak
					*size = (float) 0.001;	
				}
				if (atm.SuitReturnPressurePSI <= 4.87) {	
					// Close suit pressure relieve
					if (*isopen && !*pz)
						*open = SP_VALVE_CLOSE;

				}

				if (*size == (float) 0.001 && !*isopen) {

					//
					// Checklist actions
					//

					// Close Direct O2 valve
					DirectO2RotarySwitch.SwitchTo(3);
									

					// Next state
					systemsState = SATSYSTEMS_FLIGHT;
					lastSystemsMissionTime = MissionTime; 
				}
				break;
			}
		}
	}


//------------------------------------------------------------------------------------
// Various debug prints
//------------------------------------------------------------------------------------

#ifdef _DEBUG

/*		sprintf(oapiDebugString(), "Bus A = %3.1fA/%3.1fV, Bus B = %3.1fA/%3.1fV, AC Bus 1 = %3.1fA/%3.1fV, AC Bus 2 = %3.1fA/%3.1fV, Batt A = %3.1fV, Batt B = %3.1fV FC1 %3.1fV/%3.1fA", 
			MainBusA->Current(), MainBusA->Voltage(), MainBusB->Current(), MainBusB->Voltage(),
			ACBus1Source.Current(), ACBus1Source.Voltage(), ACBus2Source.Current(), ACBus2Source.Voltage(), EntryBatteryA->Voltage(), EntryBatteryB->Voltage(), FuelCells[0]->Voltage(), FuelCells[0]->Current());
*/
/*		sprintf(oapiDebugString(), "Bus A %3.1fA/%3.1fV, Bus B %3.1fA/%3.1fV, Batt A %3.1fV/%3.1fA/%.3f, Batt B %3.1fV/%.3f Batt C %3.1fV/%.3f Charg %2.1fV/%3.1fA FC1 %3.1fV/%3.1fA", 
			MainBusA->Current(), MainBusA->Voltage(), MainBusB->Current(), MainBusB->Voltage(),
			EntryBatteryA->Voltage(), EntryBatteryA->Current(), EntryBatteryA->Capacity() / 5508000.0, EntryBatteryB->Voltage(), EntryBatteryB->Capacity() / 5508000.0, EntryBatteryC->Voltage(), EntryBatteryC->Capacity() / 5508000.0,
			BatteryCharger.Voltage(), BatteryCharger.Current(),
			FuelCells[0]->Voltage(), FuelCells[0]->Current());
*/
/*		sprintf(oapiDebugString(), "FC1 %3.3fV/%3.3fA/%3.3fW FC2 %3.3fV/%3.3fA/%3.3fW FC3 %3.3fV/%3.3fA/%3.3fW",
			FuelCells[0]->Voltage(), FuelCells[0]->Current(), FuelCells[0]->PowerLoad(),
			FuelCells[1]->Voltage(), FuelCells[1]->Current(), FuelCells[1]->PowerLoad(),
			FuelCells[2]->Voltage(), FuelCells[2]->Current(), FuelCells[2]->PowerLoad());
*/

	double *massCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:MASS");
	double *tempCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	double *pressCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	double *pressCabinCO2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	double *co2removalrate=(double*)Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER1:CO2REMOVALRATE");

	double *O2flowCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOW");
	double *O2flowDemand = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOW");
	double *O2flowDirect=(double*)Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE:FLOW");

	double *pressSuit=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:PRESS");
	double *tempSuit=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:TEMP");
	double *pressSuitCO2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:CO2_PPRESS");
	double *massSuitH2O=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:H2O_MASS");
	double *pressSuitCRV=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS");
	double *tempSuitCRV=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:TEMP");

	//double *tempCabinRad1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINRADIATOR1:TEMP");
	//double *tempCabinRad2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINRADIATOR2:TEMP");

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

	double *massWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:MASS");
	double *tempWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:TEMP");
	double *pressWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:PRESS");

	double *massPotable=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:MASS");
	double *tempPotable=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:TEMP");
	double *pressPotable=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:PRESS");

	double *voltFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:VOLTS");
	double *ampFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:AMPS");
	double *tempFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:TEMP");
	double *dphFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:DPH");
	double *h2flowFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:H2FLOW");
	double *o2flowFC=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:O2FLOW");

	double *tempFC2=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:TEMP");
	double *tempFC3=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:TEMP");
	//double *ison=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING:ISON");


	double *massAccu=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:MASS");
	double *tempAccu=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:TEMP");
	double *pressAccu=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:PRESS");

	double *massInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:MASS");
	double *tempInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:TEMP");
	double *pressInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:PRESS");

	double *massOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:MASS");
	double *tempOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:TEMP");
	double *pressOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:PRESS");

	double *tempEvapInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATORINLET:TEMP");
	double *pressEvapInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATORINLET:PRESS");

	double *tempEvapOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET:TEMP");
	double *pressEvapOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET:PRESS");

	double *tempECSRad1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:ECSRADIATOR1:TEMP");
	double *tempECSRad2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:ECSRADIATOR2:TEMP");
	double *mixerRatio=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLEVAPINLETTEMPVALVE:RATIO");
	double *evapThrottle=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:THROTTLE");
	double *exchanger1Power=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER1:POWER");
	double *exchanger2Power=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER2:POWER");
	//double *suitExchangerPower=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITHEATEXCHANGER:POWER");
	double *suitCRVExchangerPower=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITCIRCUITHEATEXCHANGER:POWER");

/*	sprintf(oapiDebugString(), "Pot-m %.1f T %.1f p %.1f Waste-m %.1f T %.1f p %.1f deltaP %.1f Evap %.2f", 
		*massPotable, *tempPotable, *pressPotable * 0.000145038,
		*massWaste, *tempWaste, *pressWaste * 0.000145038,
		(*pressPotable - *pressWaste) * 0.000145038, *evapThrottle);
*/
/*	sprintf(oapiDebugString(), "Acc-m %.0f T %.1f In-T %.1f p %5.1f Out-T %.1f Mix %.2f EIn-T %.1f EOut-T %.1f Th %.2f H2o %.1f Rd-T1 %.1f T2 %.1f C %.1f S %.1f H2o %.1f P %.1f", 
		*massAccu, *tempAccu,
		*tempInlet, *pressInlet * 0.000145038,
		*tempOutlet, *mixerRatio, 
		*tempEvapInlet, *tempEvapOutlet, *evapThrottle, *massWaste,
		*tempECSRad1, *tempECSRad2,
		*tempCabin, *tempSuit, *massSuitH2O, *suitCRVExchangerPower);
*/
/*	sprintf(oapiDebugString(), "Acc-m %8.2f T %.1f p %3.1f In-T %.1f p %5.1f Out-T %.1f p %5.1f EvIn-T %.1f p %5.1f HSrc-m %4.2f T %.1f p %5.1f Mixer-r %.4f Rad-T1 %.1f T2 %.1f", 
		*massAccu, *tempAccu, *pressAccu * 0.000145038,
		*tempInlet, *pressInlet * 0.000145038,
		*tempOutlet, *pressOutlet * 0.000145038,
		*tempEvapInlet, *pressEvapInlet * 0.000145038,
		*massHeatsource, *tempHeatsource, *pressHeatsource * 0.000145038,
		*mixerRatio, *tempECSRad1, *tempECSRad2);
*/
/*	sprintf(oapiDebugString(), "Acc-m %8.2f T %.1f p %3.1f In-T %.1f p %5.1f Out-T %.1f p %5.1f EvOut-T %.1f p %5.1f HSrc-m %4.2f T %.1f p %5.1f H2O-m %.2f T %.1f p %.1f Rad-T1 %.1f T2 %.1f", 
		*massAccu, *tempAccu, *pressAccu * 0.000145038,
		*tempInlet, *pressInlet * 0.000145038,
		*tempOutlet, *pressOutlet * 0.000145038,
		*tempEvapOutlet, *pressEvapOutlet * 0.000145038,
		*massHeatsource, *tempHeatsource, *pressHeatsource * 0.000145038,
		*massWaste, *tempWaste, *pressWaste * 0.000145038,
		*tempECSRad1, *tempECSRad2);
*/

	// ECS Pressures
/*	sprintf(oapiDebugString(), "%d SuitCompDp %.2f SuitCabDp %.2f, CabO2 %.2f, DemO2 %.2f DirO2 %.2f Cab-p %.2f T %.1f Suit-p %.2f T %.1f co2pp %.2f SuitCRV-p %.2f T %.1f", 
		systemsState, 
		(*pressSuit - *pressSuitCRV) * 0.000145038, (*pressSuitCRV - *pressCabin) * INH2O,
		*O2flowCabin * LBH, *O2flowDemand * LBH, *O2flowDirect * LBH,
		*pressCabin * 0.000145038, *tempCabin,
		*pressSuit * 0.000145038, *tempSuit, *pressSuitCO2 * 0.00750064,
		*pressSuitCRV * 0.000145038, *tempSuitCRV);
*/
	// Cabin O2 supply
/*	sprintf(oapiDebugString(), "O2T1-m %.1f T %.1f p %.1f O2T2-m %.1f T %.1f p %.1f O2SM-m %.1f T %.1f p %4.1f O2M-m %.1f T %.1f p %5.1f CAB-m %.1f T %.1f p %.1f CO2PP %.2f", 
		*massO2Tank1 / 1000.0, *tempO2Tank1, *pressO2Tank1 * 0.000145038,
		*massO2Tank2 / 1000.0, *tempO2Tank2, *pressO2Tank2 * 0.000145038,
		*massO2SMSupply / 1000.0, *tempO2SMSupply, *pressO2SMSupply * 0.000145038,
		*massO2MainReg / 1000.0, *tempO2MainReg, *pressO2MainReg * 0.000145038,
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064); 
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

#endif
}

void Saturn::SystemsInternalTimestep(double simdt) 

{
	TRACESETUP("Saturn::SystemsInternalTimestep");

	double mintFactor = __max(simdt / 100.0, 0.5);
	double tFactor = __min(mintFactor, simdt);
	while (simdt > 0) {

		// Each timestep is passed to the SPSDK
		// to perform internal computations on the 
		// systems.

		Panelsdk.SimpleTimestep(tFactor);

		//
		// Do all updates after the SDK has updated, so that power use
		// will feed back to it.
		//

		fdaiLeft.SystemTimestep(tFactor);
		fdaiRight.SystemTimestep(tFactor);
		agc.SystemTimestep(tFactor);
		dsky.SystemTimestep(tFactor);
		cws.SystemTimestep(tFactor);
		dockingprobe.SystemTimestep(tFactor);
		imu.SystemTimestep(tFactor);
		bmag1.SystemTimestep(tFactor);
		bmag2.SystemTimestep(tFactor);
		gdc.SystemTimestep(tFactor);
		eca.SystemTimestep(tFactor);
		rjec.SystemTimestep(tFactor);
		CabinFansSystemTimestep();

		simdt -= tFactor;
		tFactor = __min(mintFactor, simdt);
		TRACE("Internal timestep done");
	}
}

void Saturn::JoystickTimestep()

{
	// DS20060302 Read joysticks and feed data to the computer
	// DS20060404 Do not do this if we aren't the active vessel.
	if(js_enabled > 0 && oapiGetFocusInterface() == this) {

		// Invert joystick configuration according to navmode in case of one joystick
		int tmp_id, tmp_rot_id, tmp_sld_id, tmp_rzx_id, tmp_debug;
		if ((rhc_id != -1 && thc_id == -1 && GetAttitudeMode() == RCS_LIN) ||
			(rhc_id == -1 && thc_id != -1 && GetAttitudeMode() == RCS_ROT)) {

			tmp_id = rhc_id;
			tmp_rot_id = rhc_rot_id;
			tmp_sld_id = rhc_sld_id;
			tmp_rzx_id = rhc_rzx_id;
			tmp_debug = rhc_debug;

			rhc_id = thc_id;
			rhc_rot_id = thc_rot_id;
			rhc_sld_id = thc_sld_id;
			rhc_rzx_id = thc_rzx_id;
			rhc_debug = thc_debug;

			thc_id = tmp_id;
			thc_rot_id = tmp_rot_id;
			thc_sld_id = tmp_sld_id;
			thc_rzx_id = tmp_rzx_id;
			thc_debug = tmp_debug;
		}

		// Issue warnings for bad configuration
		if (thc_id != -1 && !(thc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as THC does not exist.");
		}
		if (rhc_id != -1 && !(rhc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as RHC does not exist.");
		}

		HRESULT hr;
		ChannelValue31 val31;
		e_object *direct_power1, *direct_power2;
		// I thought that minimum-impulse meant the controller wasn't all the way over but that's not the case.
		// ChannelValue32 val32;
		val31.Value = agc.GetInputChannel(031); // Get current data
		//val32.Value = agc.GetInputChannel(032);
		// Mask off joystick bits
		val31.Value &= 070000;
		//val32.Value &= 077700;

		// We'll do this with a RHC first. 
		
		if(rhc_id != -1 && rhc_id < js_enabled){
			int rhc_voltage1 = 0,rhc_voltage2 = 0;
			int rhc_directv1 = 0,rhc_directv2 = 0;
			// Since we are feeding the AGC, the RHC NORMAL power must be on.
			// There's more than one RHC in the real ship, but ours is "both" - having the power on for either one will work.
			// The manual says this is right, the RHC is powered from MNA for DC, and AC1/AC2 for AC
			switch(RotPowerNormal1Switch.GetState()){
				case THREEPOSSWITCH_UP:       // AC1
					rhc_voltage1 = (int)ACBus1.Voltage();
					break;
				case THREEPOSSWITCH_DOWN:     // MNA
					rhc_voltage1 = (int)MainBusA->Voltage();
					break;
			}
			switch(RotPowerNormal2Switch.GetState()){
				case THREEPOSSWITCH_UP:       // AC2
					rhc_voltage2 = (int)ACBus2.Voltage();
					break;
				case THREEPOSSWITCH_DOWN:     // MNB
					rhc_voltage2 = (int)MainBusB->Voltage();
					break;
			}
			switch(RotPowerDirect1Switch.GetState()){
				case THREEPOSSWITCH_UP:       // MNB
					rhc_directv1 = (int)MainBusB->Voltage();
					direct_power1 = MainBusB;
					break;
				case THREEPOSSWITCH_DOWN:     // MNA
					rhc_directv1 = (int)MainBusA->Voltage();
					direct_power1 = MainBusA;
					break;
			}
			switch(RotPowerDirect2Switch.GetState()){
				case THREEPOSSWITCH_UP:       // MNA
					rhc_directv2 = (int)MainBusA->Voltage();
					direct_power2 = MainBusA;
					break;
				case THREEPOSSWITCH_DOWN:     // MNB
					rhc_directv2 = (int)MainBusB->Voltage();
					direct_power2 = MainBusB;
					break;
			}
			hr=dx8_joystick[rhc_id]->Poll();
			if(FAILED(hr)){ // Did that work?
				// Attempt to acquire the device
				hr = dx8_joystick[rhc_id]->Acquire();
				if(FAILED(hr)){
					sprintf(oapiDebugString(),"DX8JS: Cannot aquire RHC");
				}else{
					hr=dx8_joystick[rhc_id]->Poll();
				}
			}		
			// Read data
			dx8_joystick[rhc_id]->GetDeviceState(sizeof(dx8_jstate[rhc_id]),&dx8_jstate[rhc_id]);
			// X and Y are well-duh kinda things. X=0 for full-left, Y = 0 for full-down
			// Set bits according to joystick state. 32768 is center, so 16384 is the left half.
			// The real RHC had a 12 degree travel. Our joystick travels 32768 points to full deflection.
			// This means 2730 points per degree travel. The RHC breakout switches trigger at 1.5 degrees deflection and
			// stop at 11. So from 36863 to 62798, we trigger plus, and from 28673 to 2738 we trigger minus.
			// The last degree of travel is reserved for the DIRECT control switches.
			if(rhc_voltage1 > 0 || rhc_voltage2 > 0){ // NORMAL
				if(dx8_jstate[rhc_id].lX < 28673 && dx8_jstate[rhc_id].lX > 2738){
					switch(SCContSwitch.GetState()){
					case TOGGLESWITCH_UP: // CMC
						val31.Bits.MinusRollManualRotation = 1;
						break;
					case TOGGLESWITCH_DOWN: // SCS
						break;
					}
				}	
				if(dx8_jstate[rhc_id].lY < 28673 && dx8_jstate[rhc_id].lY > 2738){
					switch(SCContSwitch.GetState()){
					case TOGGLESWITCH_UP: // CMC
						val31.Bits.MinusPitchManualRotation = 1;
						break;
					case TOGGLESWITCH_DOWN: // SCS
						break;
					}
				}
				if(dx8_jstate[rhc_id].lX > 36863 && dx8_jstate[rhc_id].lX < 62798){
					switch(SCContSwitch.GetState()){
					case TOGGLESWITCH_UP: // CMC
						val31.Bits.PlusRollManualRotation = 1;
						break;
					case TOGGLESWITCH_DOWN: // SCS
						break;
					}
				}
				if(dx8_jstate[rhc_id].lY > 36863 && dx8_jstate[rhc_id].lY < 62798){
					switch(SCContSwitch.GetState()){
					case TOGGLESWITCH_UP: // CMC
						val31.Bits.PlusPitchManualRotation = 1;
						break;
					case TOGGLESWITCH_DOWN: // SCS
						break;
					}
				}
			}
			// Z-axis read.
			int rhc_rot_pos = 32768; // Initialize to centered
			if(rhc_rot_id != -1){ // If this is a rotator-type axis
				switch(rhc_rot_id){
					case 0:
						rhc_rot_pos = dx8_jstate[rhc_id].lRx; break;
					case 1:
						rhc_rot_pos = dx8_jstate[rhc_id].lRy; break;
					case 2:
						rhc_rot_pos = dx8_jstate[rhc_id].lRz; break;
				}
			}
			if(rhc_sld_id != -1){ // If this is a slider
				rhc_rot_pos = dx8_jstate[rhc_id].rglSlider[rhc_sld_id];
			}
			if(rhc_rzx_id != -1){ // If we use the native Z-axis
				rhc_rot_pos = dx8_jstate[rhc_id].lZ;
			}
			if(rhc_voltage1 > 0 || rhc_voltage2 > 0){ // NORMAL
				if(rhc_rot_pos < 28673 && rhc_rot_pos > 2738){
					switch(SCContSwitch.GetState()){
					case TOGGLESWITCH_UP: // CMC
						val31.Bits.MinusYawManualRotation = 1;
						break;
					case TOGGLESWITCH_DOWN: // SCS
						break;
					}
				}
				if(rhc_rot_pos > 36863 && rhc_rot_pos < 62798){
					switch(SCContSwitch.GetState()){
					case TOGGLESWITCH_UP: // CMC
						val31.Bits.PlusYawManualRotation = 1;
						break;
					case TOGGLESWITCH_DOWN: // SCS
						break;
					}
				}
			}
			// Copy data to the ECA
			if(rhc_voltage1 > 0 || rhc_voltage2 > 0){
				eca.rhc_x = dx8_jstate[rhc_id].lX;
				eca.rhc_y = dx8_jstate[rhc_id].lY;
				eca.rhc_z = rhc_rot_pos;
			}else{
				eca.rhc_x = 32768;
				eca.rhc_y = 32768;
				eca.rhc_z = 32768;
			}
			int rflag=0,pflag=0,yflag=0; // Direct Fire Untriggers
			int sm_sep=0;
			ChannelValue30 val30;
			val30.Value = agc.GetInputChannel(030); 
			sm_sep = val30.Bits.CMSMSeperate; // There should probably be a way for the SCS to do this if VAGC is not running
			// DIRECT
			if((rhc_directv1 > 12 || rhc_directv2 > 5)){
				if(dx8_jstate[rhc_id].lX < 2738){
					// MINUS ROLL
					if(!sm_sep){						
						SetRCSState(RCS_SM_QUAD_A, 2, 1);
						SetRCSState(RCS_SM_QUAD_B, 2, 1); 
						SetRCSState(RCS_SM_QUAD_C, 2, 1);
						SetRCSState(RCS_SM_QUAD_D, 2, 1);
						SetRCSState(RCS_SM_QUAD_A, 1, 0);
						SetRCSState(RCS_SM_QUAD_B, 1, 0); 
						SetRCSState(RCS_SM_QUAD_C, 1, 0);
						SetRCSState(RCS_SM_QUAD_D, 1, 0); 
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(200); // Four thrusters worth
						}else{
							direct_power2->DrawPower(200);
						}
					}else{
						SetCMRCSState(8,1); 
						SetCMRCSState(9,1);
						SetCMRCSState(11,0);
						SetCMRCSState(10,0);
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}
					rjec.DirectRollActive = 1; rflag = 1;
				}
				if(dx8_jstate[rhc_id].lX > 62798){
					// PLUS ROLL
					if(!sm_sep){
						SetRCSState(RCS_SM_QUAD_A, 2, 0); 
						SetRCSState(RCS_SM_QUAD_B, 2, 0); 
						SetRCSState(RCS_SM_QUAD_C, 2, 0);
						SetRCSState(RCS_SM_QUAD_D, 2, 0);
						SetRCSState(RCS_SM_QUAD_A, 1, 1);
						SetRCSState(RCS_SM_QUAD_B, 1, 1); 
						SetRCSState(RCS_SM_QUAD_C, 1, 1);
						SetRCSState(RCS_SM_QUAD_D, 1, 1); 
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(200);
						}else{
							direct_power2->DrawPower(200);
						}
					}else{
						SetCMRCSState(8,0); 
						SetCMRCSState(9,0);
						SetCMRCSState(11,1);
						SetCMRCSState(10,1);
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}
					rjec.DirectRollActive = 1; rflag = 1;
				}
				if(dx8_jstate[rhc_id].lY < 2738){
					// MINUS PITCH
					if(!sm_sep){
						SetRCSState(RCS_SM_QUAD_C, 4, 1);
						SetRCSState(RCS_SM_QUAD_A, 4, 1); 
						SetRCSState(RCS_SM_QUAD_C, 3, 0);
						SetRCSState(RCS_SM_QUAD_A, 3, 0); 
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}else{
						SetCMRCSState(0,0);
						SetCMRCSState(1,0);
						SetCMRCSState(2,1);
						SetCMRCSState(3,1);
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}
					rjec.DirectPitchActive = 1; pflag = 1;
				}
				if(dx8_jstate[rhc_id].lY > 62798){
					// PLUS PITCH
					if(!sm_sep){
						SetRCSState(RCS_SM_QUAD_C, 4, 0);
						SetRCSState(RCS_SM_QUAD_A, 4, 0); 
						SetRCSState(RCS_SM_QUAD_C, 3, 1);
						SetRCSState(RCS_SM_QUAD_A, 3, 1); 
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}else{
						SetCMRCSState(0,1);
						SetCMRCSState(1,1);
						SetCMRCSState(2,0);
						SetCMRCSState(3,0);
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}
					rjec.DirectPitchActive = 1; pflag = 1;
				}
				if(rhc_rot_pos < 2738){
					// MINUS YAW
					if(!sm_sep){
						SetRCSState(RCS_SM_QUAD_B, 4, 1);
						SetRCSState(RCS_SM_QUAD_D, 4, 1); 
						SetRCSState(RCS_SM_QUAD_D, 3, 0);
						SetRCSState(RCS_SM_QUAD_B, 3, 0); 
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}else{
						SetCMRCSState(4,0);
						SetCMRCSState(5,0);
						SetCMRCSState(6,1);
						SetCMRCSState(7,1);
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}
					rjec.DirectYawActive = 1; yflag = 1;
				}
				if(rhc_rot_pos > 62798){
					// PLUS YAW
					if(!sm_sep){
						SetRCSState(RCS_SM_QUAD_D, 3, 1);
						SetRCSState(RCS_SM_QUAD_B, 3, 1); 
						SetRCSState(RCS_SM_QUAD_B, 4, 0);
						SetRCSState(RCS_SM_QUAD_D, 4, 0); 
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}else{
						SetCMRCSState(4,1);
						SetCMRCSState(5,1);
						SetCMRCSState(6,0);
						SetCMRCSState(7,0);
						if(rhc_directv1 > 12){
							direct_power1->DrawPower(100);
						}else{
							direct_power2->DrawPower(100);
						}
					}
					rjec.DirectYawActive = 1; yflag = 1;
				}
			}
			if(rjec.DirectRollActive != 0 && rflag == 0){ // Turn off direct roll
				if(!sm_sep){
					SetRCSState(RCS_SM_QUAD_A, 1, 0);
					SetRCSState(RCS_SM_QUAD_B, 1, 0); 
					SetRCSState(RCS_SM_QUAD_C, 1, 0);
					SetRCSState(RCS_SM_QUAD_D, 1, 0); 
					SetRCSState(RCS_SM_QUAD_A, 2, 0);
					SetRCSState(RCS_SM_QUAD_B, 2, 0); 
					SetRCSState(RCS_SM_QUAD_C, 2, 0);
					SetRCSState(RCS_SM_QUAD_D, 2, 0); 
				}else{
					SetCMRCSState(8,0);
					SetCMRCSState(9,0);
					SetCMRCSState(10,0);
					SetCMRCSState(11,0);
				}
				rjec.DirectRollActive = 0;
			}
			if(rjec.DirectPitchActive != 0 && pflag == 0){ // Turn off direct pitch
				if(!sm_sep){
					SetRCSState(RCS_SM_QUAD_C, 3, 0);
					SetRCSState(RCS_SM_QUAD_A, 3, 0); 
					SetRCSState(RCS_SM_QUAD_C, 4, 0);
					SetRCSState(RCS_SM_QUAD_A, 4, 0); 
				}else{
					SetCMRCSState(0,0);
					SetCMRCSState(1,0);
					SetCMRCSState(2,0);
					SetCMRCSState(3,0);
				}
				rjec.DirectPitchActive = 0;
			}
			if(rjec.DirectYawActive != 0 && yflag == 0){ // Turn off direct yaw
				if(!sm_sep){
					SetRCSState(RCS_SM_QUAD_D, 3, 0);
					SetRCSState(RCS_SM_QUAD_B, 3, 0); 
					SetRCSState(RCS_SM_QUAD_B, 4, 0);
					SetRCSState(RCS_SM_QUAD_D, 4, 0); 
				}else{
					SetCMRCSState(4,0);
					SetCMRCSState(5,0);
					SetCMRCSState(6,0);
					SetCMRCSState(7,0);
				}
				rjec.DirectYawActive = 0;
			}
			
			if(rhc_debug != -1){ sprintf(oapiDebugString(),"RHC: X/Y/Z = %d / %d / %d",dx8_jstate[rhc_id].lX,dx8_jstate[rhc_id].lY,
				rhc_rot_pos); }
		}
		// And now the THC...
		if(thc_id != -1 && thc_id < js_enabled){
			int thc_voltage = 0; 
			switch(TransContrSwitch.GetState()){
				case TOGGLESWITCH_UP: // The THC is powered from MNA or MNB automatically.
					thc_voltage = (int)MainBusA->Voltage();
					if(thc_voltage < 5){
						thc_voltage = (int)MainBusB->Voltage();
					}
					break;
				case TOGGLESWITCH_DOWN:
					break;			
			}
			hr=dx8_joystick[thc_id]->Poll();
			if(FAILED(hr)){ // Did that work?
				// Attempt to acquire the device
				hr = dx8_joystick[thc_id]->Acquire();
				if(FAILED(hr)){
					sprintf(oapiDebugString(),"DX8JS: Cannot aquire THC");
				}else{
					hr=dx8_joystick[thc_id]->Poll();
				}
			}		
			// Read data
			dx8_joystick[thc_id]->GetDeviceState(sizeof(dx8_jstate[thc_id]),&dx8_jstate[thc_id]);
			// The THC layout is wierd. I'm going to change it so the axes are represenative of directions that make sense.
			// This is correct for the Space Shuttle THC anyway...
			if(thc_voltage > 0){
				if(dx8_jstate[thc_id].lX < 16384){							
					switch(SCContSwitch.GetState()){
						case TOGGLESWITCH_UP: // CMC
							val31.Bits.MinusYTranslation = 1;
							break;
						case TOGGLESWITCH_DOWN: // SCS
							break;
					}
				}
				if(dx8_jstate[thc_id].lY < 16384){
					switch(SCContSwitch.GetState()){
						case TOGGLESWITCH_UP: // CMC
							val31.Bits.PlusXTranslation = 1;
							break;
						case TOGGLESWITCH_DOWN: // SCS
							break;
					}
				}
				if(dx8_jstate[thc_id].lX > 49152){
					switch(SCContSwitch.GetState()){
						case TOGGLESWITCH_UP: // CMC
							val31.Bits.PlusYTranslation = 1;
							break;
						case TOGGLESWITCH_DOWN: // SCS
							break;
					}
				}
				if(dx8_jstate[thc_id].lY > 49152){
					switch(SCContSwitch.GetState()){
						case TOGGLESWITCH_UP: // CMC
							val31.Bits.MinusXTranslation = 1;
							break;
						case TOGGLESWITCH_DOWN: // SCS
							break;
					}
				}
				// Z-axis read.
				int thc_rot_pos = 32768; // Initialize to centered
				if(thc_rot_id != -1){ // If this is a rotator-type axis
					switch(thc_rot_id){
						case 0:
							thc_rot_pos = dx8_jstate[thc_id].lRx; break;
						case 1:
							thc_rot_pos = dx8_jstate[thc_id].lRy; break;
						case 2:
							thc_rot_pos = dx8_jstate[thc_id].lRz; break;
					}
				}
				if(thc_sld_id != -1){ // If this is a slider
					thc_rot_pos = dx8_jstate[thc_id].rglSlider[thc_sld_id];
				}
				if(thc_rzx_id != -1){ // If we use the native Z-axis
					thc_rot_pos = dx8_jstate[thc_id].lZ;
				}
				if(thc_rot_pos < 16384){
					switch(SCContSwitch.GetState()){
						case TOGGLESWITCH_UP: // CMC
							val31.Bits.MinusZTranslation = 1;
							break;
						case TOGGLESWITCH_DOWN: // SCS
							break;
					}
				}
				if(thc_rot_pos > 49152){
					switch(SCContSwitch.GetState()){
						case TOGGLESWITCH_UP: // CMC
							val31.Bits.PlusZTranslation = 1;
							break;
						case TOGGLESWITCH_DOWN: // SCS
							break;
					}
				}
				if(thc_debug != -1){ sprintf(oapiDebugString(),"THC: X/Y/Z = %d / %d / %d",dx8_jstate[thc_id].lX,dx8_jstate[thc_id].lY,
					thc_rot_pos); }
				// Update ECA
				eca.thc_x = dx8_jstate[thc_id].lX;
				eca.thc_y = dx8_jstate[thc_id].lY;
				eca.thc_z = thc_rot_pos;
			}else{
				// Power off
				eca.thc_x = 32768;
				eca.thc_y = 32768;
				eca.thc_z = 32768;
			}
		}
		// Submit data to the CPU.
		agc.SetInputChannel(031,val31.Value);
		//SetInputChannel(032,val32.Value);

		//sprintf(oapiDebugString(),"DX8JS: RHC %d : THC %d : CH31 = %o",rhc_id,thc_id,val31.Value);
	}
}

void Saturn::CabinFansSystemTimestep()

{
	// Both cabin fans blow through both heat exchangers,
	// so one fan is enough to turn them on both

	if (CabinFansActive()) {
		if (CabinFan1Active()) {
			ACBus1.DrawPower(19.3);
		}

		if (CabinFan2Active()) {
			ACBus2.DrawPower(19.3);
		}

		PrimCabinHeatExchanger->SetPumpAuto();
		SecCabinHeatExchanger->SetPumpAuto();
		CabinHeater->SetPumpAuto(); 

		CabinFanSound();
	} 
	else {
		PrimCabinHeatExchanger->SetPumpOff();
		SecCabinHeatExchanger->SetPumpOff();
		CabinHeater->SetPumpOff(); 

		StopCabinFanSound();
	}
}

bool Saturn::AutopilotActive()

{
	ChannelValue12 val12;
	val12.Value = agc.GetOutputChannel(012);

	return (autopilot && CMCswitch) && !val12.Bits.EnableSIVBTakeover;
}

bool Saturn::CabinFansActive()

{
	return CabinFan1Active() || CabinFan2Active();
}


bool Saturn::CabinFan1Active()

{
	//
	// For now, if any power breaker is enabled, then run the fans.
	//

	bool PowerFan1 = (ECSCabinFanAC1ACircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) || (ECSCabinFanAC1BCircuitBraker.Voltage() > 20.0) || (ECSCabinFanAC1CCircuitBraker.Voltage() > SP_MIN_ACVOLTAGE);

	return (CabinFan1Switch && PowerFan1);
}

bool Saturn::CabinFan2Active()

{
	//
	// For now, if any power breaker is enabled, then run the fans.
	//

	bool PowerFan2 = (ECSCabinFanAC2ACircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) || (ECSCabinFanAC2BCircuitBraker.Voltage() > 20.0) || (ECSCabinFanAC2CCircuitBraker.Voltage() > SP_MIN_ACVOLTAGE);

	return (CabinFan2Switch && PowerFan2);
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
	SetValveState(CSM_He1_TANKA_VALVE, true);
	SetValveState(CSM_He1_TANKB_VALVE, true);
	SetValveState(CSM_He1_TANKC_VALVE, true);
	SetValveState(CSM_He1_TANKD_VALVE, true);

	SetValveState(CSM_He2_TANKA_VALVE, true);
	SetValveState(CSM_He2_TANKB_VALVE, true);
	SetValveState(CSM_He2_TANKC_VALVE, true);
	SetValveState(CSM_He2_TANKD_VALVE, true);

	SetValveState(CSM_PRIOXID_INSOL_VALVE_A, true);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_B, true);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_C, true);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_D, true);

	SetValveState(CSM_SECOXID_INSOL_VALVE_A, true);
	SetValveState(CSM_SECOXID_INSOL_VALVE_B, true);
	SetValveState(CSM_SECOXID_INSOL_VALVE_C, true);
	SetValveState(CSM_SECOXID_INSOL_VALVE_D, true);

	SetValveState(CSM_PRIFUEL_INSOL_VALVE_A, true);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_B, true);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_C, true);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_D, true);

	SetValveState(CSM_SECFUEL_INSOL_VALVE_A, true);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_B, true);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_C, true);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_D, true);

	SetValveState(CSM_SECFUEL_PRESS_VALVE_A, true);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_B, true);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_C, true);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_D, true);
}

void Saturn::DeactivateCSMRCS()

{
	SetValveState(CSM_He1_TANKA_VALVE, false);
	SetValveState(CSM_He1_TANKB_VALVE, false);
	SetValveState(CSM_He1_TANKC_VALVE, false);
	SetValveState(CSM_He1_TANKD_VALVE, false);

	SetValveState(CSM_He2_TANKA_VALVE, false);
	SetValveState(CSM_He2_TANKB_VALVE, false);
	SetValveState(CSM_He2_TANKC_VALVE, false);
	SetValveState(CSM_He2_TANKD_VALVE, false);

	SetValveState(CSM_PRIOXID_INSOL_VALVE_A, false);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_B, false);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_C, false);
	SetValveState(CSM_PRIOXID_INSOL_VALVE_D, false);

	SetValveState(CSM_SECOXID_INSOL_VALVE_A, false);
	SetValveState(CSM_SECOXID_INSOL_VALVE_B, false);
	SetValveState(CSM_SECOXID_INSOL_VALVE_C, false);
	SetValveState(CSM_SECOXID_INSOL_VALVE_D, false);

	SetValveState(CSM_PRIFUEL_INSOL_VALVE_A, false);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_B, false);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_C, false);
	SetValveState(CSM_PRIFUEL_INSOL_VALVE_D, false);

	SetValveState(CSM_SECFUEL_INSOL_VALVE_A, false);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_B, false);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_C, false);
	SetValveState(CSM_SECFUEL_INSOL_VALVE_D, false);

	SetValveState(CSM_SECFUEL_PRESS_VALVE_A, false);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_B, false);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_C, false);
	SetValveState(CSM_SECFUEL_PRESS_VALVE_D, false);
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
	return ((GetValveState(CSM_He1_TANKA_VALVE) || GetValveState(CSM_He2_TANKA_VALVE)) && 
		(GetValveState(CSM_PRIOXID_INSOL_VALVE_A) || GetValveState(CSM_SECOXID_INSOL_VALVE_A)) &&
		(GetValveState(CSM_PRIFUEL_INSOL_VALVE_A) || (GetValveState(CSM_SECFUEL_PRESS_VALVE_A) && GetValveState(CSM_SECOXID_INSOL_VALVE_A))));
}

bool Saturn::SMRCSBActive()

{
	return ((GetValveState(CSM_He1_TANKB_VALVE) || GetValveState(CSM_He2_TANKB_VALVE)) && 
		(GetValveState(CSM_PRIOXID_INSOL_VALVE_B) || GetValveState(CSM_SECOXID_INSOL_VALVE_B)) &&
		(GetValveState(CSM_PRIFUEL_INSOL_VALVE_B) || (GetValveState(CSM_SECFUEL_PRESS_VALVE_B) && GetValveState(CSM_SECOXID_INSOL_VALVE_B))));

}

bool Saturn::SMRCSCActive()

{
	return ((GetValveState(CSM_He1_TANKC_VALVE) || GetValveState(CSM_He2_TANKC_VALVE)) && 
		(GetValveState(CSM_PRIOXID_INSOL_VALVE_C) || GetValveState(CSM_SECOXID_INSOL_VALVE_C)) &&
		(GetValveState(CSM_PRIFUEL_INSOL_VALVE_C) || (GetValveState(CSM_SECFUEL_PRESS_VALVE_C) && GetValveState(CSM_SECOXID_INSOL_VALVE_C))));

}

bool Saturn::SMRCSDActive()

{
	return ((GetValveState(CSM_He1_TANKD_VALVE) || GetValveState(CSM_He2_TANKD_VALVE)) && 
		(GetValveState(CSM_PRIOXID_INSOL_VALVE_D) || GetValveState(CSM_SECOXID_INSOL_VALVE_D)) &&
		(GetValveState(CSM_PRIFUEL_INSOL_VALVE_D) || (GetValveState(CSM_SECFUEL_PRESS_VALVE_D) && GetValveState(CSM_SECOXID_INSOL_VALVE_D))));
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

/*
// DS20060222 Removed
bool Saturn::CMRCSActive()

{
	//
	// I don't think this is correct, but I'm not sure yet what the CM RCS switches do.
	//

	return GetValveState(CM_RCSPROP_TANKA_VALVE) && GetValveState(CM_RCSPROP_TANKB_VALVE);
}
*/

// DS20060222 Rewritten
void Saturn::SetRCS_CM()

{
	PROPELLANT_HANDLE sysa_ph,sysb_ph; // Temporary

	if (th_att_cm[0]) {
		if (GetValveState(CM_RCSPROP_TANKA_VALVE)) {		
			// CM RCS System A
			sysa_ph = ph_rcs_cm_1;
		}else{
			sysa_ph = NULL;
		}
		if(GetValveState(CM_RCSPROP_TANKB_VALVE)){
			// CM RCS System B
			sysb_ph = ph_rcs_cm_2;	
		}else{
			sysb_ph = NULL;
		}		
		// Now assign them accordingly
		SetThrusterResource(th_att_cm[0], sysa_ph);
		SetThrusterResource(th_att_cm[1], sysb_ph);
		SetThrusterResource(th_att_cm[2], sysa_ph);
		SetThrusterResource(th_att_cm[3], sysb_ph);
		SetThrusterResource(th_att_cm[4], sysa_ph);
		SetThrusterResource(th_att_cm[5], sysb_ph);
		SetThrusterResource(th_att_cm[6], sysb_ph);
		SetThrusterResource(th_att_cm[7], sysa_ph);
		SetThrusterResource(th_att_cm[8], sysa_ph);
		SetThrusterResource(th_att_cm[9], sysb_ph);
		SetThrusterResource(th_att_cm[10], sysb_ph);
		SetThrusterResource(th_att_cm[11], sysa_ph);
	}
}

//
// Check the state of the RCS, and enable/disable thrusters as required.
//

void Saturn::SetRCSThrusters(THRUSTER_HANDLE *th, PROPELLANT_HANDLE ph)

{
	int i;

	for (i = 0; i < 8; i++) {
		if (th[i])
			SetThrusterResource(th[i], ph);
	}
}

void Saturn::CheckRCSState()

{
	//
	// Enable and disable thrusters by quad.
	//
	//

	switch (stage) {
	case CSM_LEM_STAGE:
		SetRCSThrusters(th_rcs_a, SMRCSAActive() ? ph_rcs0 : 0);
		SetRCSThrusters(th_rcs_b, SMRCSBActive() ? ph_rcs1 : 0);
		SetRCSThrusters(th_rcs_c, SMRCSCActive() ? ph_rcs2 : 0);
		SetRCSThrusters(th_rcs_d, SMRCSDActive() ? ph_rcs3 : 0);
		break;

	case CM_STAGE:
	case CM_ENTRY_STAGE:
	case CM_ENTRY_STAGE_TWO:
	case CM_ENTRY_STAGE_THREE:
	case CM_ENTRY_STAGE_FOUR:
	case CM_ENTRY_STAGE_FIVE:
	case CM_ENTRY_STAGE_SIX:
	case CM_ENTRY_STAGE_SEVEN:
	case CSM_ABORT_STAGE:
		SetRCS_CM();
		break;
	}
}

void Saturn::ActivateSPS()

{
	SPSswitch.SetState(TOGGLESWITCH_UP);
}

void Saturn::DeactivateSPS()

{
	SPSswitch.SetState(TOGGLESWITCH_UP);
}

void Saturn::SetEngineIndicators()

{
	for (int i = 1; i <= 8; i++) {
		SetEngineIndicator(i);
	}
}

void Saturn::ClearEngineIndicators()

{
	for (int i = 1; i <= 8; i++) {
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

//
// These switches weren't lit for real, but can be useful in low-realism
// scenarios.
//

void Saturn::SetLESMotorLight(bool lit)

{
	if (lit && Realism > REALISM_PUSH_LIGHTS)
		lit = false;

	LesMotorFireSwitch.SetLit(lit);
}

void Saturn::SetCanardDeployLight(bool lit)

{
	if (lit && Realism > REALISM_PUSH_LIGHTS)
		lit = false;

	CanardDeploySwitch.SetLit(lit);
}

void Saturn::SetDrogueDeployLight(bool lit)

{
	if (lit && Realism > REALISM_PUSH_LIGHTS)
		lit = false;

	DrogueDeploySwitch.SetLit(lit);
}

void Saturn::SetCSMLVSepLight(bool lit)

{
	if (lit && Realism > REALISM_PUSH_LIGHTS)
		lit = false;

	CsmLvSepSwitch.SetLit(lit);
}

void Saturn::SetApexCoverLight(bool lit)

{
	if (lit && Realism > REALISM_PUSH_LIGHTS)
		lit = false;

	ApexCoverJettSwitch.SetLit(lit);
}

void Saturn::SetMainDeployLight(bool lit)

{
	if (lit && Realism > REALISM_PUSH_LIGHTS)
		lit = false;

	MainDeploySwitch.SetLit(lit);
}

void Saturn::SetLVGuidLight()

{
	LVGuidLight = true;
}

void Saturn::ClearLVGuidLight()

{
	LVGuidLight = false;
}

void Saturn::SetLVRateLight()

{
	LVRateLight = true;
}

void Saturn::ClearLVRateLight()

{
	LVRateLight = false;
}

void Saturn::SetEngineIndicator(int i)

{
	if (i < 1 || i > 8)
		return;

	ENGIND[i - 1] = true;
}

void Saturn::ClearEngineIndicator(int i)

{
	if (i < 1 || i > 8)
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
	pO2Tank1Quantity = 0;
	pO2Tank2Quantity = 0;
	pH2Tank1Quantity = 0;
	pH2Tank2Quantity = 0;
	pO2SurgeTankPress = 0;
	for (i = 0; i <= 3; i++) {
		pFCH2Flow[i] = 0;
		pFCO2Flow[i] = 0;
		pFCTemp[i] = 0;
		pFCCondenserTemp[i] = 0;
		pFCCoolingTemp[i] = 0;
	}
	pPrimECSRadiatorInletPressure = 0;
	pPrimECSRadiatorInletTemp = 0;
	pPrimECSRadiatorOutletTemp = 0;
	pPrimECSEvaporatorOutletTemp = 0;
	pPrimECSEvaporatorSteamPressure = 0;
	pPrimECSAccumulatorQuantity = 0;
	pSecECSRadiatorInletPressure = 0;
	pSecECSRadiatorInletTemp = 0;
	pSecECSRadiatorOutletTemp = 0;
	pSecECSEvaporatorOutletTemp = 0;
	pSecECSEvaporatorSteamPressure = 0;
	pSecECSAccumulatorQuantity = 0;
	pPotableH2oTankQuantity = 0;
	pWasteH2oTankQuantity = 0;

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
// Get displayed atmosphere status for caution & warning system
//

void Saturn::GetDisplayedAtmosStatus(DisplayedAtmosStatus &atm) 

{
	atm.DisplayedO2FlowLBH = RightO2FlowMeter.GetDisplayValue();
	atm.DisplayedSuitComprDeltaPressurePSI = SuitComprDeltaPMeter.GetDisplayValue();
	atm.DisplayedEcsRadTempPrimOutletMeterTemperatureF = EcsRadTempPrimOutletMeter.GetDisplayValue();
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
	press.O2SurgeTankPressurePSI = 0.0;

	//
	// Oxygen surge tank.
	//

	if (!pO2SurgeTankPress) {
		pO2SurgeTankPress = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:PRESS");
	}
	if (pO2SurgeTankPress) {
		press.O2SurgeTankPressurePSI = (*pO2SurgeTankPress) * PSI;
	}


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
// Get H2/O2 tank quantities.
//

void Saturn::GetTankQuantities(TankQuantities &q)

{
	//
	// Clear to defaults.
	//

	q.H2Tank1Quantity = 0.0;
	q.H2Tank2Quantity = 0.0;
	q.O2Tank1QuantityKg = 0.0;
	q.O2Tank2QuantityKg = 0.0;
	q.O2Tank1Quantity = 0.0;
	q.O2Tank2Quantity = 0.0;

	//
	// No tanks if we've seperated from the SM
	//

	if (stage >= CM_STAGE) {
		return;
	}

	//
	// Hydrogen tanks.
	//

	if (!pH2Tank1Quantity) {
		pH2Tank1Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:MASS");
	}
	if (pH2Tank1Quantity) {
		q.H2Tank1Quantity = (*pH2Tank1Quantity) / CSM_H2TANK_CAPACITY;
	}

	if (!pH2Tank2Quantity) {
		pH2Tank2Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:MASS");
	}
	if (pH2Tank2Quantity) {
		q.H2Tank2Quantity = (*pH2Tank2Quantity) / CSM_H2TANK_CAPACITY;
	}

	//
	// Oxygen tanks.
	//

	if (!pO2Tank1Quantity) {
		pO2Tank1Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:MASS");
	}

	if (pO2Tank1Quantity) {
		q.O2Tank1Quantity = (*pO2Tank1Quantity) / CSM_O2TANK_CAPACITY;
		q.O2Tank1QuantityKg = (*pO2Tank1Quantity) / 1000.0;
	}

	if (!pO2Tank2Quantity) {
		pO2Tank2Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:MASS");
	}
	if (pO2Tank2Quantity) {
		q.O2Tank2Quantity = (*pO2Tank2Quantity) / CSM_O2TANK_CAPACITY;
		q.O2Tank2QuantityKg = (*pO2Tank2Quantity) / 1000.0;
	}
}

//
// Set O2 tank quantities. For now, just use one fixed input.
//

void Saturn::SetO2TankQuantities(double q)

{
	O2Tanks[0]->space.composition[SUBSTANCE_O2].mass = q * 1000.0;
	O2Tanks[1]->space.composition[SUBSTANCE_O2].mass = q * 1000.0;
}

//
// Get fuel cell status. 
//

void Saturn::GetFuelCellStatus(int index, FuelCellStatus &fc)

{
	//
	// Set defaults.
	//

	fc.H2FlowLBH  = 0.0;
	fc.O2FlowLBH = 0.0;
	fc.TempF = 0.0;
	fc.CondenserTempF = 0.0;
	fc.CoolingTempF = 0.0;
	fc.Voltage = 0.0;
	fc.Current = 0.0;
	fc.PowerOutput = 0.0;

	//
	// No fuel cells if we've seperated from the SM.
	//

	if (stage >= CM_STAGE) {
		return;
	}

	FCell *f = FuelCells[index - 1];

	//
	// Fuel cell.
	//

	char buffer[1000];

	fc.H2FlowLBH = f->H2_flowPerSecond * LBH;
	fc.O2FlowLBH = f->O2_flowPerSecond * LBH;
	fc.TempF = KelvinToFahrenheit(f->Temp);
	fc.CondenserTempF = KelvinToFahrenheit(f->condenserTemp);

	if (!pFCCoolingTemp[index]) {
		sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:TEMP", index);
		pFCCoolingTemp[index] = (double*) Panelsdk.GetPointerByString(buffer);
	}
	if (pFCCoolingTemp[index]) {
		fc.CoolingTempF = KelvinToFahrenheit(*pFCCoolingTemp[index]);
	}

	fc.Voltage = FuelCells[index - 1]->Voltage();
	fc.Current = FuelCells[index - 1]->Current();
	fc.PowerOutput = FuelCells[index - 1]->PowerLoad();
}


//
// Get primary ECS water-glycol cooling status. 
//

void Saturn::GetPrimECSCoolingStatus(PrimECSCoolingStatus &pcs)

{
	//
	// Set defaults.
	//

	pcs.RadiatorInletPressurePSI = 0.0;
	pcs.RadiatorInletTempF = 0.0;
	pcs.RadiatorOutletTempF = 0.0;
	pcs.EvaporatorOutletTempF = 0.0;
	pcs.EvaporatorSteamPressurePSI = 0.0;

	//
	// Get values
	//

	if (!pPrimECSRadiatorInletPressure) {
		pPrimECSRadiatorInletPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:PRESS");
	}
	if (pPrimECSRadiatorInletPressure) {
		pcs.RadiatorInletPressurePSI = (*pPrimECSRadiatorInletPressure) * PSI;
	}

	if (!pPrimECSRadiatorInletTemp) {
		pPrimECSRadiatorInletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:TEMP");
	}
	if (pPrimECSRadiatorInletTemp) {
		pcs.RadiatorInletTempF = KelvinToFahrenheit(*pPrimECSRadiatorInletTemp);
	}

	if (!pPrimECSRadiatorOutletTemp) {
		pPrimECSRadiatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:TEMP");
	}
	if (pPrimECSRadiatorOutletTemp) {
		pcs.RadiatorOutletTempF = KelvinToFahrenheit(*pPrimECSRadiatorOutletTemp);
	}

	if (!pPrimECSEvaporatorOutletTemp) {
		pPrimECSEvaporatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET:TEMP");
	}
	if (pPrimECSEvaporatorOutletTemp) {
		pcs.EvaporatorOutletTempF = KelvinToFahrenheit(*pPrimECSEvaporatorOutletTemp);
	}

	if (!pPrimECSEvaporatorSteamPressure) {
		pPrimECSEvaporatorSteamPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:STEAMPRESSURE");
	}
	if (pPrimECSEvaporatorSteamPressure) {
		pcs.EvaporatorSteamPressurePSI = (*pPrimECSEvaporatorSteamPressure) * PSI;
	}

	if (!pPrimECSAccumulatorQuantity) {
		pPrimECSAccumulatorQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:MASS");
	}
	if (pPrimECSAccumulatorQuantity) {
		pcs.AccumulatorQuantityPercent = (*pPrimECSAccumulatorQuantity) / 10000.0;
	}
}


//
// Get secondary ECS water-glycol cooling status. 
//

void Saturn::GetSecECSCoolingStatus(SecECSCoolingStatus &scs)

{
	//
	// Set defaults.
	//

	scs.RadiatorInletPressurePSI = 0.0;
	scs.RadiatorInletTempF = 0.0;
	scs.RadiatorOutletTempF = 0.0;
	scs.EvaporatorOutletTempF = 0.0;
	scs.EvaporatorSteamPressurePSI = 0.0;

	//
	// Get values
	//

	if (!pSecECSRadiatorInletPressure) {
		pSecECSRadiatorInletPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATORINLET:PRESS");
	}
	if (pSecECSRadiatorInletPressure) {
		scs.RadiatorInletPressurePSI = (*pSecECSRadiatorInletPressure) * PSI;
	}

	if (!pSecECSRadiatorInletTemp) {
		pSecECSRadiatorInletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATORINLET:TEMP");
	}
	if (pSecECSRadiatorInletTemp) {
		scs.RadiatorInletTempF = KelvinToFahrenheit(*pSecECSRadiatorInletTemp);
	}

	if (!pSecECSRadiatorOutletTemp) {
		pSecECSRadiatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATOROUTLET:TEMP");
	}
	if (pSecECSRadiatorOutletTemp) {
		scs.RadiatorOutletTempF = KelvinToFahrenheit(*pSecECSRadiatorOutletTemp);
	}

	if (!pSecECSEvaporatorOutletTemp) {
		pSecECSEvaporatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOROUTLET:TEMP");
	}
	if (pSecECSEvaporatorOutletTemp) {
		scs.EvaporatorOutletTempF = KelvinToFahrenheit(*pSecECSEvaporatorOutletTemp);
	}

	if (!pSecECSEvaporatorSteamPressure) {
		pSecECSEvaporatorSteamPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:STEAMPRESSURE");
	}
	if (pSecECSEvaporatorSteamPressure) {
		scs.EvaporatorSteamPressurePSI = (*pSecECSEvaporatorSteamPressure) * PSI;
	}

	if (!pSecECSAccumulatorQuantity) {
		pSecECSAccumulatorQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:MASS");
	}
	if (pSecECSAccumulatorQuantity) {
		scs.AccumulatorQuantityPercent = (*pSecECSAccumulatorQuantity) / 10000.0;
	}
}


//
// Get ECS water status. 
//

void Saturn::GetECSWaterStatus(ECSWaterStatus &ws)

{
	//
	// Set defaults.
	//

	ws.PotableH2oTankQuantityPercent = 0;
	ws.WasteH2oTankQuantityPercent = 0;

	//
	// Get values
	//

	if (!pPotableH2oTankQuantity) {
		pPotableH2oTankQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:MASS");
	}
	if (pPotableH2oTankQuantity) {
		ws.PotableH2oTankQuantityPercent = (*pPotableH2oTankQuantity) / 16000.0;
	}

	if (!pWasteH2oTankQuantity) {
		pWasteH2oTankQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:MASS");
	}
	if (pWasteH2oTankQuantity) {
		ws.WasteH2oTankQuantityPercent = (*pWasteH2oTankQuantity) / 26500.0;
	}
}

//
// Main bus state.
//

void Saturn::GetMainBusStatus(MainBusStatus &ms)

{
	ms.MainBusAVoltage = 0.0;
	ms.MainBusBVoltage = 0.0;
	
	ms.Enabled_DC_A_CWS = (MainBusAResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
	ms.Enabled_DC_B_CWS = (MainBusBResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
	ms.Reset_DC_A_CWS = MainBusAResetSwitch.IsUp();
	ms.Reset_DC_B_CWS = MainBusBResetSwitch.IsUp();
	
	if (&MainBusA) {
		ms.MainBusAVoltage = MainBusA->Voltage();}

	if (&MainBusB) {
		ms.MainBusBVoltage = MainBusB->Voltage();}

	ms.Fc_Disconnected = MainBusAController.IsFuelCellDisconnectAlarm() || 
		                 MainBusBController.IsFuelCellDisconnectAlarm();
}

//
// AC bus state.
//

void Saturn::GetACBusStatus(ACBusStatus &as, int busno)

{
	as.ACBusCurrent = 0.0;
	as.ACBusVoltage = 0.0;
	as.Phase1Current = 0.0;
	as.Phase2Current = 0.0;
	as.Phase3Current = 0.0;
	as.Phase1Voltage = 0.0;
	as.Phase2Voltage = 0.0;
	as.Phase3Voltage = 0.0;
	as.Enabled_AC_CWS = true;
	as.Reset_AC_CWS = false;

	switch (busno) {
	case 1:
		as.ACBusCurrent = ACBus1.Current();
		as.ACBusVoltage = ACBus1.Voltage();
		as.Phase1Current = ACBus1PhaseA.Current();
		as.Phase2Current = ACBus1PhaseB.Current();
		as.Phase3Current = ACBus1PhaseC.Current();
		as.Phase1Voltage = ACBus1PhaseA.Voltage();
		as.Phase2Voltage = ACBus1PhaseB.Voltage();
		as.Phase3Voltage = ACBus1PhaseC.Voltage();
		as.Enabled_AC_CWS = (AcBus1ResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
		as.Reset_AC_CWS = AcBus1ResetSwitch.IsUp();
		break;

	case 2:
		as.ACBusCurrent = ACBus2.Current();
		as.ACBusVoltage = ACBus2.Voltage();
		as.Phase1Current = ACBus2PhaseA.Current();
		as.Phase2Current = ACBus2PhaseB.Current();
		as.Phase3Current = ACBus2PhaseC.Current();
		as.Phase1Voltage = ACBus2PhaseA.Voltage();
		as.Phase2Voltage = ACBus2PhaseB.Voltage();
		as.Phase3Voltage = ACBus2PhaseC.Voltage();
		as.Enabled_AC_CWS = (AcBus2ResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
		as.Reset_AC_CWS = AcBus2ResetSwitch.IsUp();
		break;
	}
}

void Saturn::DisconectInverter(bool disc, int busno)

{
	if (disc) {
		if (busno == 1) ACBus1Source.WireToBuses(0,0,0);
		else ACBus2Source.WireToBuses(0,0,0);
	}
	else {
		if (busno == 1) ACBus1Source.WireToBuses(&AcBus1Switch1, &AcBus1Switch2, &AcBus1Switch3);
		else ACBus2Source.WireToBuses(&AcBus2Switch1, &AcBus2Switch2, &AcBus2Switch3);
	}
}

//
// ISS warning state.
//

void Saturn::GetAGCWarningStatus(AGCWarningStatus &aws)

{
	ChannelValue11 val11;
	ChannelValue13 val13;

	val11.Value = agc.GetOutputChannel(011);
	if (val11.Bits.ISSWarning) 
		aws.ISSWarning = true;
	else
		aws.ISSWarning = false;

	val13.Value = agc.GetOutputChannel(013);
	if (val13.Bits.TestAlarms)  
		aws.TestAlarms = true;
	else
		aws.TestAlarms = false;
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
	//
	// First check whether the valve still exists!
	//

	if (valve < CM_VALVES_START) {
		if (stage > CSM_LEM_STAGE)
			return false;
	}

	if (pCSMValves[valve])
		return (*pCSMValves[valve] == SP_VALVE_OPEN);

	return ValveState[valve];
}


//
// Check whether the ELS is active and whether it's in auto mode.
//
// TODO: for now, if we're flying an unmanned mission we just assume
// this is always valid. We need to do a more accurate simulation later.
//

bool Saturn::ELSActive()

{
	if (!Crewed)
		return true;

	return (ELSAutoSwitch.Voltage() > SP_MIN_DCVOLTAGE);
}

bool Saturn::ELSAuto()

{
	if (!Crewed)
		return true;

	return (ELSActive() && ELSAutoSwitch.IsUp());
}

bool Saturn::RCSLogicActive()

{
	if (!Crewed)
		return true;

	return (CMPropDumpSwitch.Voltage() > SP_MIN_DCVOLTAGE);
}

bool Saturn::RCSDumpActive()

{
	return (RCSLogicActive() && CMPropDumpSwitch.IsUp());
}

bool Saturn::RCSPurgeActive()

{
	return ((CPPropPurgeSwitch.Voltage() > SP_MIN_DCVOLTAGE) && CPPropPurgeSwitch.IsUp());
}

bool Saturn::PyrosArmed()

{
	if (!Crewed)
		return true;

	return (PyroPower.Voltage() > SP_MIN_DCVOLTAGE);
}

bool Saturn::SECSLogicActive() 

{
	if (!Crewed)
		return true;

	return (SECSLogicPower.Voltage() > SP_MIN_DCVOLTAGE);
}

bool Saturn::LETAttached()

{
	return LESAttached;
}

bool Saturn::DisplayingPropellantQuantity()

{
	return SMRCSIndSwitch.IsDown();
}

//
// Engine control functions for AGC.
//

void Saturn::SetRCSState(int Quad, int Thruster, bool Active)

{
	//
	// Sanity check.
	//

	if (stage < CSM_LEM_STAGE)
		return;

	if (stage > CSM_LEM_STAGE && Quad < RCS_CM_RING_1)
		return;

	THRUSTER_HANDLE th = 0;

	switch (Quad) {
	case RCS_SM_QUAD_A:
		th = th_rcs_a[Thruster];
		break;

	case RCS_SM_QUAD_B:
		th = th_rcs_b[Thruster];
		break;

	case RCS_SM_QUAD_C:
		th = th_rcs_c[Thruster];
		break;

	case RCS_SM_QUAD_D:
		th = th_rcs_d[Thruster];
		break;
	}

	double Level = Active ? 1.0 : 0.0;

	if (th)
		SetThrusterLevel(th, Level);
}

// DS20060221 Added function
void Saturn::SetCMRCSState(int Thruster, bool Active)

{
	double Level = Active ? 1.0 : 0.0;
	if(th_att_cm[Thruster] == NULL){ return; } // Sanity check
	SetThrusterLevel(th_att_cm[Thruster], Level);
}

void Saturn::SetSPSState(bool Active)

{
	rjec.SPSActive = Active;
	if (stage == CSM_LEM_STAGE) {
		SetThrusterGroupLevel(THGROUP_MAIN, Active ? 1.0 : 0.0);
	}
}

// DS20060226 Added below
// Should return error between commanded value and current value
double Saturn::SetSPSPitch(double direction){
	VECTOR3 spsvector;
	double error = sps_pitch_position - direction;	
	// Only 5.5 degrees of travel allowed.
	if(direction > 5.5){ direction = 5.5; }
	if(direction < -5.5){ direction = -5.5; }
	sps_pitch_position += direction; // Instant positioning	
	// Directions X,Y,Z = YAW (+ = left),PITCH (+ = DOWN),FORE/AFT
	spsvector.x = sps_yaw_position * 0.017453; // Convert deg to rad
	spsvector.y = sps_pitch_position * 0.017453;
	spsvector.z = 1;
	SetThrusterDir(th_main[0],spsvector);
	// If out of zero posiiton
	if(sps_pitch_position != 0 || sps_yaw_position != 0){
		sprintf(oapiDebugString(),"SPS: Vector P:%f Y:%f",sps_pitch_position,sps_yaw_position);
	}else{
		// Clear the message
		sprintf(oapiDebugString(),"");
	}
	return(error);
}

double Saturn::SetSPSYaw(double direction){
	VECTOR3 spsvector;
	double error = sps_yaw_position - direction;	
	// Only 5.5 degrees of travel allowed.
	if(direction > 5.5){ direction = 5.5; }
	if(direction < -5.5){ direction = -5.5; }
	sps_yaw_position += direction; // Instant positioning	
	spsvector.x = sps_yaw_position * 0.017453; // Convert deg to rad
	spsvector.y = sps_pitch_position * 0.017453;
	spsvector.z = 1;
	SetThrusterDir(th_main[0],spsvector);
	// If out of zero posiiton
	if(sps_pitch_position != 0 || sps_yaw_position != 0){
		sprintf(oapiDebugString(),"SPS: Vector P:%f Y:%f",sps_pitch_position,sps_yaw_position);
	}else{
		// Clear the message
		sprintf(oapiDebugString(),"");
	}
	return(error);
}
