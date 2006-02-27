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
  *	Revision 1.78  2006/02/23 22:46:41  quetalsi
  *	Added AC ovevoltage control and Bugfix
  *	
  *	Revision 1.77  2006/02/23 15:48:30  tschachim
  *	Restored changes lost in last version.
  *	
  *	Revision 1.76  2006/02/23 14:13:49  dseagrav
  *	Split CM RCS into two systems, moved CM RCS thrusters (close to) proper positions, eliminated extraneous thrusters, set ISP and thrust values to match documentation, connected CM RCS to AGC IO channels 5 and 6 per DAP documentation, changes 20060221-20060223.
  *	
  *	Revision 1.75  2006/02/22 20:14:46  quetalsi
  *	C&W  AC_BUS1/2 light and AC RESET SWITCH now woks.
  *	
  *	Revision 1.74  2006/02/22 18:53:48  tschachim
  *	Bugfixes for Apollo 4-6.
  *
  *	Revision 1.73  2006/02/21 11:53:17  tschachim
  *	Bugfix FDAI.
  *	
  *	Revision 1.72  2006/02/13 21:36:21  tschachim
  *	C/W ISS light, Bugfix.
  *	
  *	Revision 1.71  2006/02/01 18:33:21  tschachim
  *	More REALISM 0 checklist actions.
  *	
  *	Revision 1.70  2006/01/15 01:23:19  movieman523
  *	Put 'phantom' RCS thrusters back in and adjusted RCS thrust and ISP based on REALISM value.
  *	
  *	Revision 1.69  2006/01/14 20:58:16  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.68  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
  *	Revision 1.67  2006/01/14 00:54:35  movieman523
  *	Hacky wiring of sequential systems and pyro arm switches.
  *	
  *	Revision 1.66  2006/01/11 22:34:20  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.65  2006/01/11 03:09:40  movieman523
  *	Changed default RCS valve states to closed.
  *	
  *	Revision 1.64  2006/01/11 02:59:43  movieman523
  *	Valve talkbacks now check the valve state directlry. This means they barberpole on SM sep and can't then be changed.
  *	
  *	Revision 1.63  2006/01/11 02:16:25  movieman523
  *	Added RCS propellant quantity gauge.
  *	
  *	Revision 1.62  2006/01/10 23:20:51  movieman523
  *	SM RCS is now enabled per quad.
  *	
  *	Revision 1.61  2006/01/10 21:09:30  movieman523
  *	Improved AoA/thrust meter.
  *	
  *	Revision 1.60  2006/01/10 20:49:50  movieman523
  *	Added CM RCS propellant dump and revised thrust display.
  *	
  *	Revision 1.59  2006/01/10 19:34:44  movieman523
  *	Fixed AC bus switches and added ELS Logic/Auto support.
  *	
  *	Revision 1.58  2006/01/09 17:55:26  tschachim
  *	Connected the dockingprobe to the EPS.
  *	
  *	Revision 1.57  2006/01/08 21:43:34  movieman523
  *	First phase of implementing inverters, and stopped PanelSDK trying to delete objects which weren't allocated with new().
  *	
  *	Revision 1.56  2006/01/08 19:08:20  movieman523
  *	Disabled debug output.
  *	
  *	Revision 1.55  2006/01/08 19:04:30  movieman523
  *	Wired up AC bus switches in a quick and hacky manner.
  *	
  *	Revision 1.54  2006/01/08 16:59:59  flydba
  *	Tried to set up talkbacks for switches on panel 3 but it was unsuccessful.
  *	
  *	Revision 1.53  2006/01/08 16:15:19  movieman523
  *	For now, hard-wire batteries to buses when CM is seperated from SM.
  *	
  *	Revision 1.52  2006/01/07 19:11:44  tschachim
  *	Checklist actions for FDAIPowerRotarySwitch.
  *	
  *	Revision 1.51  2006/01/07 18:12:43  tschachim
  *	Bugfix
  *	
  *	Revision 1.50  2006/01/07 03:28:28  movieman523
  *	Removed a lot of unused switches and wired up the FDAI power switch.
  *	
  *	Revision 1.49  2006/01/07 01:34:08  movieman523
  *	Added AC bus overvoltage and main bus undervolt lights.
  *	
  *	Revision 1.48  2006/01/06 20:37:18  movieman523
  *	Made the voltage and current meters work. Currently hard-coded to main bus A and AC bus 1.
  *	
  *	Revision 1.47  2006/01/05 11:28:28  tschachim
  *	New dockingprobe handling.
  *	
  *	Revision 1.46  2005/12/19 17:09:25  tschachim
  *	Bugfixes, checklist actions.
  *	
  *	Revision 1.45  2005/12/02 20:44:35  movieman523
  *	Wired up buses and batteries directly rather than through PowerSource objects.
  *	
  *	Revision 1.44  2005/12/02 19:29:24  movieman523
  *	Started integrating PowerSource code into PanelSDK.
  *	
  *	Revision 1.43  2005/11/24 01:07:54  movieman523
  *	Removed code for panel lights which were being set incorrectly. Plus a bit of tidying.
  *	
  *	Revision 1.42  2005/11/23 23:59:24  movieman523
  *	Added fuel cells.
  *	
  *	Revision 1.41  2005/11/23 19:20:51  movieman523
  *	Made boilers work again!
  *	
  *	Revision 1.40  2005/11/18 22:11:22  movieman523
  *	Added seperate heat and electrical power usage for boilers. Revised cabin fan code.
  *	
  *	Revision 1.39  2005/11/18 20:38:59  movieman523
  *	Revised condensor output from fuel cell to eliminate master alarms.
  *	
  *	Revision 1.38  2005/11/18 02:40:55  movieman523
  *	Major revamp of PanelSDK electrical code, and various modifications to run off fuel cells.
  *	
  *	Revision 1.37  2005/11/17 22:06:47  movieman523
  *	Added other electrical buses and revised cabin fan code.
  *	
  *	Revision 1.36  2005/11/17 21:04:52  movieman523
  *	IMU and AGC now start powered-down. Revised battery code, and wired up all batteries in CSM.
  *	
  *	Revision 1.35  2005/11/17 19:19:12  movieman523
  *	Added three-phase AC bus and battery buses.
  *	
  *	Revision 1.34  2005/11/17 01:52:29  movieman523
  *	Simplified setup for circuit breakers, and added battery buses.
  *	
  *	Revision 1.33  2005/11/17 01:23:11  movieman523
  *	Revised circuit breaker code. Now all switchers are PowerSources, so no need for the seperate PowerBreaker class.
  *	
  *	Revision 1.32  2005/11/17 00:28:36  movieman523
  *	Wired in AGC circuit breakers.
  *	
  *	Revision 1.31  2005/11/16 23:14:01  movieman523
  *	Initial support for wiring in the circuit breakers.
  *	
  *	Revision 1.30  2005/11/16 00:18:49  movieman523
  *	Added beginnings of really basic IU emulation. Added random failures of caution and warning lights on non-historical missions. Added initial support for Skylab CM and SM. Added LEM Name option in scenario file.
  *	
  *	Revision 1.29  2005/10/31 10:20:51  tschachim
  *	SPSSwitch is now 2-pos, new ONPAD_STAGE.
  *	
  *	Revision 1.28  2005/10/19 11:29:22  tschachim
  *	Bugfixes for high time accelerations.
  *	
  *	Revision 1.27  2005/10/12 19:38:34  tschachim
  *	Added a little bit more power to the suit compressor to have
  *	more stability with high time accelerations.
  *	
  *	Revision 1.26  2005/10/11 16:37:56  tschachim
  *	More REALISM 0 automatisms, bugfix stage 1 aborts, more C/W alarms
  *	
  *	Revision 1.25  2005/09/30 11:23:28  tschachim
  *	Added ECS water-glycol coolant loop.
  *	
  *	Revision 1.24  2005/08/24 00:30:00  movieman523
  *	Revised CM RCS code, and removed a load of switches that aren't used anymore.
  *	
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

	Inverter1 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_1");
	Inverter2 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_2");
	Inverter3 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_3");

	Inverter1->WireTo(&MnA1Switch);
	Inverter2->WireTo(&MnB2Switch);
	Inverter3->WireTo(&MnA3Switch);

	MainBusA = (DCbus *) Panelsdk.GetPointerByString("ELECTRIC:DC_A");
	MainBusB = (DCbus *) Panelsdk.GetPointerByString("ELECTRIC:DC_B");

	ACBus1Source.WireToBuses(&AcBus1Switch1, &AcBus1Switch2, &AcBus1Switch3);
	ACBus2Source.WireToBuses(&AcBus2Switch1, &AcBus2Switch2, &AcBus2Switch3);

	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:AC_A");
	eo->WireTo(&ACBus1);

	ACBus1PhaseA.WireTo(&ACBus1Source);
	ACBus1PhaseB.WireTo(&ACBus1Source);
	ACBus1PhaseC.WireTo(&ACBus1Source);

	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:AC_B");
	eo->WireTo(&ACBus2);

	ACBus2PhaseA.WireTo(&ACBus2Source);
	ACBus2PhaseB.WireTo(&ACBus2Source);
	ACBus2PhaseC.WireTo(&ACBus2Source);

	ACBus1.WireToBuses(&ACBus1PhaseA, &ACBus1PhaseB, &ACBus1PhaseC);
	ACBus2.WireToBuses(&ACBus2PhaseA, &ACBus2PhaseB, &ACBus2PhaseC);

	PyroPower.WireToBuses(&PyroArmASwitch, &PyroArmBSwitch);
	SECSLogicPower.WireToBuses(&Logic1Switch, &Logic2Switch);

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

	BatteryBusA.WireToBuses(EntryBatteryA, EntryBatteryB, EntryBatteryC);
	BatteryBusB.WireToBuses(EntryBatteryA, EntryBatteryB, EntryBatteryC);

	PyroBusA.WireToBuses(EntryBatteryA, PyroBatteryA);
	PyroBusB.WireToBuses(EntryBatteryB, PyroBatteryB);

	//
	// Generic power source for switches, tied to both Bus A and
	// Bus B.
	//

	SwitchPower.WireToBuses(MainBusA, MainBusB);

	//
	// ECS devices
	//

	PrimCabinHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMCABINHEATEXCHANGER");
	SecCabinHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:SECCABINHEATEXCHANGER");
	CabinHeater = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:CABINHEATER");

	PrimEcsRadiatorExchanger1 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER1");
	PrimEcsRadiatorExchanger2 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER2");

	//
	// Wire up internal systems.
	//

	cws.WireTo(&CWMnaCircuitBraker, &CWMnbCircuitBraker);
	agc.WirePower(&GNComputerMnACircuitBraker, &GNComputerMnBCircuitBraker);
	imu.WireToBuses(&GNIMUMnACircuitBraker, &GNIMUMnBCircuitBraker);
	dockingprobe.WireTo(&DockProbeMnACircuitBraker, &DockProbeMnBCircuitBraker);   

	//
	// FDAI power. This is almost certainly wired to the wrong bus, but it will do for
	// now.
	//

	FDAIPowerRotarySwitch.WireTo(&SwitchPower);

	//
	// Default valve states. For now, everything starts closed.
	//
	
	SetValveState(CSM_He1_TANKA_VALVE, false);
	SetValveState(CSM_He1_TANKB_VALVE, false);
	SetValveState(CSM_He1_TANKC_VALVE, false);
	SetValveState(CSM_He1_TANKD_VALVE, false);

	SetValveState(CSM_He2_TANKA_VALVE, false);
	SetValveState(CSM_He2_TANKB_VALVE, false);
	SetValveState(CSM_He2_TANKC_VALVE, false);
	SetValveState(CSM_He2_TANKD_VALVE, false);

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
	// DS20060226 SPS Gimbal reset to zero
	sps_pitch_position = 0;
	sps_yaw_position = 0;	
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

		// Each timestep is passed to the SPSDK
		// to perform internal computations on the 
		// systems.

		Panelsdk.Timestep(simt);

		//
		// Do all updates after the SDK has updated, so that power use
		// will feed back to it.
		//

		dsky.Timestep(MissionTime);
		dsky2.Timestep(MissionTime);
		agc.Timestep(MissionTime, simdt);
		iu.Timestep(MissionTime, simdt);
		imu.Timestep(MissionTime);
		cws.TimeStep(MissionTime);
		dockingprobe.TimeStep(MissionTime, simdt);
		secs.Timestep(MissionTime, simdt);
		fdaiLeft.Timestep(MissionTime, simdt);
		fdaiRight.Timestep(MissionTime, simdt);

		//
		// General checks.
		//

		CheckCabinFans();

		//
		// Systems state handling
		//
		if (!firstSystemsTimeStepDone) {
			firstSystemsTimeStepDone = true;
		}
		else {
			AtmosStatus atm;
			GetAtmosStatus(atm);

			double *pMax, *fMax, *fancap, scdp, *ison;
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

				// O2 demand regulator to 5.5 inH2O higher than cabin pressure
				pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:PRESSMAX");
				*pMax = 14.7 / PSI + 5.5 / INH2O;	
				open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT2:OPEN");
				*open = SP_VALVE_OPEN;

				// Close cabin to suit circuit return value
				open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
				*open = SP_VALVE_CLOSE;


				//
				// Checklist actions
				//
				
				// Temporary fix because of too low power load
				FuelCellRadiators1Switch.SwitchTo(THREEPOSSWITCH_DOWN);  
				FuelCellRadiators2Switch.SwitchTo(THREEPOSSWITCH_DOWN);  
				FuelCellRadiators3Switch.SwitchTo(THREEPOSSWITCH_DOWN);  

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

				// Activate FDAIs
				FDAIPowerRotarySwitch.SwitchTo(3);

				// Start mission timer
				MissionTimerSwitch.SwitchTo(THREEPOSSWITCH_UP);
				

				// Next state
				systemsState = SATSYSTEMS_PRELAUNCH;
				lastSystemsMissionTime = MissionTime; 
				break;

			case SATSYSTEMS_PRELAUNCH:
				if (MissionTime >= -6000) {	// 1h 40min before launch

					// Slow down time acceleration
					if (Realism && oapiGetTimeAcceleration() > 1.0)
						oapiSetTimeAcceleration(1.0);

					// Crew ingress
					number = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CREW:NUMBER");
					*number = 3; 

					// Close cabin pressure regulator 
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT:OPEN");
					*open = SP_VALVE_CLOSE;

					// O2 demand regulator to 2.5 inH2O higher than cabin pressure
					pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:PRESSMAX");
					*pMax = 14.7 / PSI + 2.5 / INH2O;
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT2:OPEN");
					*open = SP_VALVE_OPEN;

					// O2 demand regulator max flow to 0.8 lb/h 
					// TODO: This should be done with the direct O2 valve manually, at the moment it doesn't matter
					fMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOWMAX");
					*fMax = 0.8 / LBH;

					// Suit compressor 1 to prelaunch configuration
					fancap = (double*) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER:FANCAP");
					*fancap = 110000.0;

					// Open cabin to suit circuit return value
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:OUT:OPEN");
					*open = SP_VALVE_OPEN;
					pMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNINLET:PRESSMAX");
					*pMax = 100.0 / PSI;	// that's like disabling PREG
				

					//
					// Checklist actions
					//
		
					// Turn on suit compressor 1
					SuitCompressor1Switch.SwitchTo(THREEPOSSWITCH_UP);

					// Turn on cabin fans
					CabinFan1Switch.SwitchTo(TOGGLESWITCH_UP);
					CabinFan2Switch.SwitchTo(TOGGLESWITCH_UP);
					SuitCircuitH2oAccumAutoSwitch.SwitchTo(THREEPOSSWITCH_UP);


					// Next state
					systemsState = SATSYSTEMS_CREWINGRESS_1;
					lastSystemsMissionTime = MissionTime; 
				}	
				break;

			case SATSYSTEMS_CREWINGRESS_1:
				// Suit compressor running?
				ison = (double*) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER:ISON");
				if (!*ison) {
					lastSystemsMissionTime = MissionTime; 
				} else {
					scdp = (atm.SuitReturnPressurePSI - atm.CabinPressurePSI) * (INH2O / PSI);
					if (scdp > 0.0 && MissionTime - lastSystemsMissionTime >= 50) {	// Suit Cabin delta p is established

						// Open cabin pressure regulator, max flow to 0.4 lb/h  
						open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:OUT:OPEN");
						*open = SP_VALVE_OPEN;
						fMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOWMAX");
						*fMax = 0.4 / LBH;

						// O2 demand regulator max flow to 0.5 lb/h 
						fMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOWMAX");
						*fMax = 0.5 / LBH;

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
				if (scdp > 1.5 && MissionTime - lastSystemsMissionTime >= 10) {	// Suit Cabin delta p is established
					// Cabin leak
					size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
					*size = (float)0.0005;
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:OPEN");
					*open = SP_VALVE_OPEN;

					// Disable cabin pressure regulator and O2 demand regulator max flow
					fMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOWMAX");
					*fMax = 0;
					fMax = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOWMAX");
					*fMax = 0;

					// Next state
					systemsState = SATSYSTEMS_CABINCLOSEOUT;
					lastSystemsMissionTime = MissionTime; 
				}	
				break;

			case SATSYSTEMS_CABINCLOSEOUT:
				if (MissionTime >= -1200) {	// 20min before launch

					// Slow down time acceleration
					if (Realism && oapiGetTimeAcceleration() > 1.0)
						oapiSetTimeAcceleration(1.0);

					//
					// Checklist actions
					//

					// Turn on SM RCS
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

					// Turn on sequencial logic and arm pryros
					ArmBatACircuitBraker.SwitchTo(TOGGLESWITCH_UP);
					ArmBatBCircuitBraker.SwitchTo(TOGGLESWITCH_UP);					
					LogicBatACircuitBraker.SwitchTo(TOGGLESWITCH_UP);
					LogicBatBCircuitBraker.SwitchTo(TOGGLESWITCH_UP);

					Logic1Switch.SwitchTo(TOGGLESWITCH_UP);
					Logic2Switch.SwitchTo(TOGGLESWITCH_UP);
					PyroArmASwitch.SwitchTo(TOGGLESWITCH_UP);
					PyroArmBSwitch.SwitchTo(TOGGLESWITCH_UP);
						
					// Turn off cabin fans
					CabinFan1Switch.SwitchTo(TOGGLESWITCH_DOWN);
					CabinFan2Switch.SwitchTo(TOGGLESWITCH_DOWN);

					// Turn off cyro fans
					H2Fan1Switch.SwitchTo(THREEPOSSWITCH_CENTER);
					H2Fan2Switch.SwitchTo(THREEPOSSWITCH_CENTER);
					O2Fan1Switch.SwitchTo(THREEPOSSWITCH_CENTER);
					O2Fan2Switch.SwitchTo(THREEPOSSWITCH_CENTER);

					// EDS auto on
					EDSSwitch.SwitchTo(TOGGLESWITCH_UP);
					
					// Latch FC valves
					FCReacsValvesSwitch.SwitchTo(TOGGLESWITCH_DOWN);

					// Bypass primary radiators
					GlycolToRadiatorsLever.SwitchTo(TOGGLESWITCH_DOWN);

					// Temporary solution to enable medium heating of the primary coolant loop. 
					HighGainAntennaPitchPositionSwitch.SwitchTo(3);

					// Next state
					systemsState = SATSYSTEMS_READYTOLAUNCH;
					lastSystemsMissionTime = MissionTime; 
				}
				break;	

			case SATSYSTEMS_READYTOLAUNCH:
				if (GetAtmPressure() <= 6.0 / PSI) {
					// Cabin pressure relieve
					size = (float*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:SIZE");
					*size = (float)0.25;
					open = (int*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:LEAK:OPEN");
					*open = SP_VALVE_OPEN;
					
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
					*fancap = 65000.0;	

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
					*size = (float)0.001;	
				}
				if (atm.SuitReturnPressurePSI <= 4.87) {	
					// Close suit pressure relieve
					if (*isopen && !*pz)
						*open = SP_VALVE_CLOSE;

					// Close direct O2 valve
					*(int*) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:LEAK:OPEN") = SP_VALVE_CLOSE;
				}

				if (*size == (float)0.001 && !*isopen) {
					// Primary ECS radiators now working normally, TODO secondary
					PrimEcsRadiatorExchanger1->SetLength(10.0);
					PrimEcsRadiatorExchanger2->SetLength(10.0);
					
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

//		sprintf(oapiDebugString(), "Bus A = %3.3fA/%3.3fV, Bus B = %3.3fA/%3.3fV, AC Bus 1 = %3.3fA/%3.3fV, AC Bus 2 = %3.3fA/%3.3fV, Batt A = %3.3fV, Batt B = %3.3fV", 
//			MainBusA->Current(), MainBusA->Voltage(), MainBusB->Current(), MainBusB->Voltage(),
//			ACBus1.Current(), ACBus1.Voltage(), ACBus2.Current(), ACBus2.Voltage(), EntryBatteryA->Voltage(), EntryBatteryB->Voltage());
//		sprintf(oapiDebugString(), "FC1 %3.3fV/%3.3fA/%3.3fW FC2 %3.3fV/%3.3fA/%3.3fW FC3 %3.3fV/%3.3fA/%3.3fW",
//			FuelCells[0]->Voltage(), FuelCells[0]->Current(), FuelCells[0]->PowerLoad(),
//			FuelCells[1]->Voltage(), FuelCells[1]->Current(), FuelCells[1]->PowerLoad(),
//			FuelCells[2]->Voltage(), FuelCells[2]->Current(), FuelCells[2]->PowerLoad());


	double *massCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:MASS");
	double *tempCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	double *pressCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	double *pressCabinCO2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	double *co2removalrate=(double*)Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER:CO2REMOVALRATE");

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

void Saturn::CheckCabinFans()

{
	// Both cabin fans blow through both heat exchangers,
	// so one fan is enough to turn them on both

	if (CabinFansActive()) {
		if (CabinFan1Active()) {
			ACBus1.DrawPower(30.0);
		}

		if (CabinFan2Active()) {
			ACBus2.DrawPower(30.0);
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

	bool PowerFan1 = (ECSCabinFanAC1ACircuitBraker.Voltage() > 20.0) || (ECSCabinFanAC1BCircuitBraker.Voltage() > 20.0) || (ECSCabinFanAC1CCircuitBraker.Voltage() > 20.0);

	return (CabinFan1Switch && PowerFan1);
}

bool Saturn::CabinFan2Active()

{
	//
	// For now, if any power breaker is enabled, then run the fans.
	//

	bool PowerFan2 = (ECSCabinFanAC2ACircuitBraker.Voltage() > 20.0) || (ECSCabinFanAC2BCircuitBraker.Voltage() > 20.0) || (ECSCabinFanAC2CCircuitBraker.Voltage() > 20.0);

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
	SetValveState(CSM_He1_TANKA_VALVE, false);
	SetValveState(CSM_He1_TANKB_VALVE, false);
	SetValveState(CSM_He1_TANKC_VALVE, false);
	SetValveState(CSM_He1_TANKD_VALVE, false);

	SetValveState(CSM_He2_TANKA_VALVE, false);
	SetValveState(CSM_He2_TANKB_VALVE, false);
	SetValveState(CSM_He2_TANKC_VALVE, false);
	SetValveState(CSM_He2_TANKD_VALVE, false);

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
	return (GetValveState(CSM_He1_TANKA_VALVE) && GetValveState(CSM_He2_TANKA_VALVE) && GetValveState(CSM_PRIPROP_TANKA_VALVE) && GetValveState(CSM_SECPROP_TANKA_VALVE));
}

bool Saturn::SMRCSBActive()

{
	return (GetValveState(CSM_He1_TANKB_VALVE) && GetValveState(CSM_He2_TANKB_VALVE) && GetValveState(CSM_PRIPROP_TANKB_VALVE) && GetValveState(CSM_SECPROP_TANKB_VALVE));
}

bool Saturn::SMRCSCActive()

{
	return (GetValveState(CSM_He1_TANKC_VALVE) && GetValveState(CSM_He2_TANKC_VALVE) && GetValveState(CSM_PRIPROP_TANKC_VALVE) && GetValveState(CSM_SECPROP_TANKC_VALVE));
}

bool Saturn::SMRCSDActive()

{
	return (GetValveState(CSM_He1_TANKD_VALVE) && GetValveState(CSM_He2_TANKD_VALVE) && GetValveState(CSM_PRIPROP_TANKD_VALVE) && GetValveState(CSM_SECPROP_TANKD_VALVE));
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
	}

	if (!pO2Tank2Quantity) {
		pO2Tank2Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:MASS");
	}
	if (pO2Tank2Quantity) {
		q.O2Tank2Quantity = (*pO2Tank2Quantity) / CSM_O2TANK_CAPACITY;
	}
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

	if (MainBusA) {
		ms.MainBusAVoltage = MainBusA->Voltage();
	}

	if (MainBusB) {
		ms.MainBusBVoltage = MainBusB->Voltage();
	}
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
		as.Enabled_AC_CWS = AcBus1ResetSwitch.IsCenter();
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
		as.Enabled_AC_CWS = AcBus2ResetSwitch.IsCenter();
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
// Switch status
//

int Saturn::GetSwitchState(char *switchName) {

	ToggleSwitch *s = (ToggleSwitch *) PSH.GetSwitch(switchName);
	return s->GetState();
}

	
int Saturn::GetRotationalSwitchState(char *switchName) {

	RotationalSwitch *s = (RotationalSwitch *) PSH.GetSwitch(switchName);
	return s->GetState();
}

//
// Check whether the ELS is active and whether it's in auto mode.
//

bool Saturn::ELSActive()

{
	return (ELSAutoSwitch.Voltage() > 20.0);
}

bool Saturn::ELSAuto()

{
	return (ELSActive() && ELSAutoSwitch.IsUp());
}

bool Saturn::RCSLogicActive()

{
	return (CMPropDumpSwitch.Voltage() > 20.0);
}

bool Saturn::RCSDumpActive()

{
	return (RCSLogicActive() && CMPropDumpSwitch.IsUp());
}

bool Saturn::RCSPurgeActive()

{
	return ((CPPropPurgeSwitch.Voltage() > 20.0) && CPPropPurgeSwitch.IsUp());
}

bool Saturn::PyrosArmed()

{
	return (PyroPower.Voltage() > 20.0);
}

bool Saturn::SECSLogicActive() 

{
	return (SECSLogicPower.Voltage() > 20.0);
}

bool Saturn::LETAttached()

{
	return (stage < LAUNCH_STAGE_TWO_TWR_JET);
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
	SetThrusterLevel(th_att_cm[Thruster], Level);
}

void Saturn::SetSPSState(bool Active)

{
	if (stage == CSM_LEM_STAGE) {
		SetThrusterGroupLevel(THGROUP_MAIN, Active ? 1.0 : 0.0);
	}
}

// DS20060226 Added below
// Should return error between commanded value and current value
double Saturn::SetSPSPitch(double direction){
	VECTOR3 spsvector;
	double error = sps_pitch_position - direction;	
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
