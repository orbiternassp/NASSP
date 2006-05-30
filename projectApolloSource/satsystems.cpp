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
  *	
  *	Revision 1.99  2006/04/06 19:32:49  movieman523
  *	More Apollo 13 support.
  *	
  *	Revision 1.98  2006/04/05 00:55:06  dseagrav
  *	Bugfix: Do not read joysticks when we don't have input focus.
  *	
  *	Revision 1.97  2006/04/04 23:36:14  dseagrav
  *	Added the beginnings of the telecom subsystem.
  *	
  *	Revision 1.96  2006/03/28 12:45:31  tschachim
  *	Bugfixes.
  *	
  *	Revision 1.95  2006/03/27 19:22:44  quetalsi
  *	Bugfix RCS PRPLNT switches and wired to brakers.
  *	
  *	Revision 1.94  2006/03/25 00:14:31  dseagrav
  *	Missed a debug print
  *	
  *	Revision 1.93  2006/03/25 00:12:42  dseagrav
  *	SCS ECA added.
  *	
  *	Revision 1.92  2006/03/19 17:06:13  dseagrav
  *	Fixed mistake with RCS TRNFR, it's a 3-position switch and is ignored for now.
  *	
  *	Revision 1.91  2006/03/19 06:09:45  dseagrav
  *	GDC and ASCP save and load state.
  *	
  *	Revision 1.90  2006/03/18 22:55:55  dseagrav
  *	Added more RJEC functionality.
  *	
  *	Revision 1.89  2006/03/16 04:53:21  dseagrav
  *	Added preliminary RJEC, connected CMC to RJEC.
  *	
  *	Revision 1.88  2006/03/16 01:28:24  dseagrav
  *	Changed RHC switch positions to match the real RHC, conditionalized RHC/THC CMC inputs on the position of the SC CONT switch.
  *	
  *	Revision 1.87  2006/03/15 03:43:18  dseagrav
  *	Corrected GDC BMAG logic, removed absurd attitude error on BMAG failure, changed failure simulation to more accurate method.
  *	
  *	Revision 1.86  2006/03/14 02:48:57  dseagrav
  *	Added ECA object, moved FDAI redraw stuff into ECA to clean up FDAI redraw mess.
  *	
  *	Revision 1.85  2006/03/12 01:13:28  dseagrav
  *	Added lots of SCS items and FDAI stuff.
  *	
  *	Revision 1.84  2006/03/09 20:40:21  quetalsi
  *	Added Battery Relay Bus. Wired Inverter 1/2/3, EPS Sensor Unit DC A/B, EPS Sensor Unit AC 1/2 and Bat Rly Bus BAT A/B brakers.
  *	
  *	Revision 1.83  2006/03/04 22:50:52  dseagrav
  *	Added FDAI RATE logic, SPS TVC travel limited to 5.5 degrees plus or minus, added check for nonexistent joystick selection in DirectInput code. I forgot to date most of these.
  *	
  *	Revision 1.82  2006/03/03 05:12:36  dseagrav
  *	Added DirectInput code and THC/RHC interface. Changes 20060228-20060302
  *	
  *	Revision 1.81  2006/02/28 20:40:32  quetalsi
  *	Bugfix and added CWS FC BUS DISCONNECT. Reset DC switches now work.
  *	
  *	Revision 1.80  2006/02/28 00:03:58  quetalsi
  *	MainBus A & B Switches and Talkbacks woks and wired.
  *	
  *	Revision 1.79  2006/02/27 00:57:48  dseagrav
  *	Added SPS thrust-vector control. Changes 20060225-20060226.
  *	
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
		iu.Timestep(MissionTime, simdt);
		imu.Timestep(MissionTime);

		// DS20060304 SCS updation
		bmag1.TimeStep();
		bmag2.TimeStep();
		ascp.TimeStep();
		gdc.TimeStep(MissionTime);
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

				// Activate FDAIs
				FDAIPowerRotarySwitch.SwitchTo(3);

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
				// Suit compressors running?
				if (!SuitCompressor1->pumping && !SuitCompressor2->pumping) {
					lastSystemsMissionTime = MissionTime; 
				} else {
					scdp = (atm.SuitReturnPressurePSI - atm.CabinPressurePSI) * (INH2O / PSI);
					if (scdp > 0.0 && MissionTime - lastSystemsMissionTime >= 50) {	// Suit Cabin delta p is established

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
				if (scdp > 1.3 && MissionTime - lastSystemsMissionTime >= 10) {	// Suit Cabin delta p is established

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

					// Slow down time acceleration
					if (Realism && oapiGetTimeAcceleration() > 1.0)
						oapiSetTimeAcceleration(1.0);


					//
					// Checklist actions
					//

					// Turn off cabin fans (AOH2 4.2.1.6)
					CabinFan1Switch.SwitchTo(TOGGLESWITCH_DOWN);
					CabinFan2Switch.SwitchTo(TOGGLESWITCH_DOWN);

					// Turn on BMAGs (AOH2 4.2.2.1)
					// TODO

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
	if(js_enabled > 0 && oapiGetFocusInterface() == this){
		e_object *direct_power1, *direct_power2;
		// Issue warnings for bad configuration
		if(thc_id != -1 && !(thc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as THC does not exist.");
		}
		if(rhc_id != -1 && !(rhc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as RHC does not exist.");
		}
		HRESULT hr;
		ChannelValue31 val31;
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

		// sprintf(oapiDebugString(),"DX8JS: RHC %d : THC %d : CH31 = %o",rhc_id,thc_id,val31.Value);
		// Submit data to the CPU.
		agc.SetInputChannel(031,val31.Value);
		//SetInputChannel(032,val32.Value);
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

bool Saturn::ELSActive()

{
	return (ELSAutoSwitch.Voltage() > SP_MIN_DCVOLTAGE);
}

bool Saturn::ELSAuto()

{
	return (ELSActive() && ELSAutoSwitch.IsUp());
}

bool Saturn::RCSLogicActive()

{
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
	return (PyroPower.Voltage() > SP_MIN_DCVOLTAGE);
}

bool Saturn::SECSLogicActive() 

{
	return (SECSLogicPower.Voltage() > SP_MIN_DCVOLTAGE);
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

// DS20060304 SCS OBJECTS
// BMAG
BMAG::BMAG(){
	sat = NULL;
	powered = FALSE;
	dc_source = NULL;
	ac_source = NULL;
	dc_bus = NULL;
	ac_bus = NULL;
	temperature=0;
	rates = _V(0,0,0);
}

void BMAG::Init(Saturn *vessel, e_object *dcbus, e_object *acbus, Boiler *h) {
	// Initialize
	sat = vessel;
	dc_bus = dcbus;
	ac_bus = acbus;
	heater = h;
}

void BMAG::TimeStep() {
	// For right now, simply get rates if both the heater and the gyro are powered.
	// Later, we should consider temperature and spin-up time and such.
	
	if(sat != NULL) {
		if(dc_source != NULL && dc_source->Voltage() > SP_MIN_DCVOLTAGE) {
			if(ac_source != NULL && ac_source->Voltage() > SP_MIN_ACVOLTAGE) {
				powered = TRUE;
				sat->GetAngularVel(rates); // From those, generate ROTATION RATE data.
			} else {
				powered = FALSE; // No data
			}
		} else {
			powered = FALSE; // No data
		}
	}
	heater->WireTo(dc_source);	// Take DC power to heat the gyro
}

void BMAG::SystemTimestep(double simdt) {

	if (powered && ac_source)
		ac_source->DrawPower(8.7);  // take AC power to spin the gyro
}


// GDC
GDC::GDC(){
	rates = _V(0,0,0);	
	attitude = _V(0,0,0);
	sat = NULL;
	Initialized = FALSE;
	fdai_err_ena=0;
	fdai_err_x=0;
	fdai_err_y=0;
	fdai_err_z=0;
	roll_bmag_failed=0;
	pitch_bmag_failed=0;
	yaw_bmag_failed=0;
}

void GDC::Init(Saturn *vessel){
	sat = vessel;
}

void GDC::SystemTimestep(double simdt) {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2) return;  // Switched off

	if (sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->StabContSystemAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) return;

	sat->StabContSystemAc1CircuitBraker.DrawPower(10.4); 
	sat->StabContSystemAc2CircuitBraker.DrawPower(3.4); 	
}
		
void GDC::TimeStep(double simt) {

	VESSELSTATUS vs;
	GDC_Matrix3 t;
	VECTOR3 newAngles;

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() != 2) return;  // Switched off

	if (sat->StabContSystemAc1CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->StabContSystemAc2CircuitBraker.Voltage() < SP_MIN_ACVOLTAGE) return;

	// Pull status
	sat->GetStatus(vs);
	// Get eccliptic-plane attitude
	double orbiterAttitudeX = vs.arot.x;
	double orbiterAttitudeY = vs.arot.y;
	double orbiterAttitudeZ = vs.arot.z;

	// Get rates from the appropriate BMAG
	// GDC attitude is based on RATE data, not ATT data.
	switch(sat->BMAGRollSwitch.GetState()){
		case THREEPOSSWITCH_UP:     // RATE2
			rates.z = sat->bmag2.rates.z;
			if(sat->bmag2.powered){
				if(roll_bmag_failed){ attitude.x = 0; Initialized=FALSE; roll_bmag_failed = 0; } // Force align to zero
			}else{
				if(!roll_bmag_failed){ roll_bmag_failed = 1; } // Fail it
			}
			break;
		case THREEPOSSWITCH_CENTER: // RATE2/ATT1
			rates.z = sat->bmag2.rates.z;
			if(sat->bmag2.powered){
				if(roll_bmag_failed){ attitude.x = 0; Initialized=FALSE; roll_bmag_failed = 0; } // Force align to zero
			}else{
				if(!roll_bmag_failed){ roll_bmag_failed = 1; } // Fail it
			}
			break;
		case THREEPOSSWITCH_DOWN:   // RATE1
			rates.z = sat->bmag1.rates.z;
			if(sat->bmag1.powered){
				if(roll_bmag_failed){ attitude.x = 0; Initialized=FALSE; roll_bmag_failed = 0; } // Force align to zero
			}else{
				if(!roll_bmag_failed){ roll_bmag_failed = 1; } // Fail it
			}
			break;			
	}
	switch(sat->BMAGPitchSwitch.GetState()){
		case THREEPOSSWITCH_UP:     // RATE2
			rates.x = sat->bmag2.rates.x;
			if(sat->bmag2.powered){
				if(pitch_bmag_failed){ attitude.y = 0; Initialized=FALSE; pitch_bmag_failed = 0; } // Force align to zero
			}else{
				if(!pitch_bmag_failed){ pitch_bmag_failed = 1; } // Fail it				
			}
			break;
		case THREEPOSSWITCH_CENTER: // RATE2/ATT1
			rates.x = sat->bmag2.rates.x;
			if(sat->bmag2.powered){
				if(pitch_bmag_failed){ attitude.y = 0; Initialized=FALSE; pitch_bmag_failed = 0; } // Force align to zero
			}else{
				if(!pitch_bmag_failed){ pitch_bmag_failed = 1; } // Fail it				
			}
			break;
		case THREEPOSSWITCH_DOWN:   // RATE1
			rates.x = sat->bmag1.rates.x;
			if(sat->bmag1.powered){
				if(pitch_bmag_failed){ attitude.y = 0; Initialized=FALSE; pitch_bmag_failed = 0; } // Force align to zero
			}else{
				if(!pitch_bmag_failed){ pitch_bmag_failed = 1; } // Fail it				
			}
			break;			
	}

	switch(sat->BMAGYawSwitch.GetState()){
		case THREEPOSSWITCH_UP:     // RATE2
			rates.y = sat->bmag2.rates.y;
			if(sat->bmag2.powered){
				if(yaw_bmag_failed){ attitude.z = 0; Initialized=FALSE; yaw_bmag_failed = 0; } // Force align to zero
			}else{ 
				if(!yaw_bmag_failed){ yaw_bmag_failed = 1; } // Fail it
			}
			break;
		case THREEPOSSWITCH_CENTER: // RATE2/ATT1
			rates.y = sat->bmag2.rates.y;
			if(sat->bmag2.powered){
				if(yaw_bmag_failed){ attitude.z = 0; Initialized=FALSE; yaw_bmag_failed = 0; } // Force align to zero
			}else{ 
				if(!yaw_bmag_failed){ yaw_bmag_failed = 1; } // Fail it
			}
			break;
		case THREEPOSSWITCH_DOWN:   // RATE1
			rates.y = sat->bmag1.rates.y;
			if(sat->bmag1.powered){
				if(yaw_bmag_failed){ attitude.z = 0; Initialized=FALSE; yaw_bmag_failed = 0; } // Force align to zero
			}else{ 
				if(!yaw_bmag_failed){ yaw_bmag_failed = 1; } // Fail it
			}
			break;
	}					
	
	if (!Initialized) {
		// Reset
//		attitude = _V(0,0,0); Don't reset, provided by alignment key
		Orbiter.AttitudeReference.m11 = 0;
		Orbiter.AttitudeReference.m12 = 0;
		Orbiter.AttitudeReference.m13 = 0;
		Orbiter.AttitudeReference.m21 = 0;
		Orbiter.AttitudeReference.m22 = 0;
		Orbiter.AttitudeReference.m23 = 0;
		Orbiter.AttitudeReference.m31 = 0;
		Orbiter.AttitudeReference.m32 = 0;
		Orbiter.AttitudeReference.m33 = 0;
		Orbiter.Attitude.X = orbiterAttitudeX;
		Orbiter.Attitude.Y = orbiterAttitudeY;
		Orbiter.Attitude.Z = orbiterAttitudeZ;
		SetOrbiterAttitudeReference();
		
		Orbiter.LastAttitude.X = orbiterAttitudeX;
		Orbiter.LastAttitude.Y = orbiterAttitudeY;
		Orbiter.LastAttitude.Z = orbiterAttitudeZ;	
		
		LastTime = simt;
		Initialized = true;
	}else{
		Orbiter.Attitude.X = orbiterAttitudeX;
		Orbiter.Attitude.Y = orbiterAttitudeY;
		Orbiter.Attitude.Z = orbiterAttitudeZ;
		// Gimbals
		t = Orbiter.AttitudeReference;
	  	t = multiplyMatrix(getRotationMatrixX(Orbiter.Attitude.X), t);
	  	t = multiplyMatrix(getRotationMatrixY(Orbiter.Attitude.Y), t);
	  	t = multiplyMatrix(getRotationMatrixZ(Orbiter.Attitude.Z), t);
	  	
	  	t = multiplyMatrix(getOrbiterLocalToNavigationBaseTransformation(), t);
	  	
		// calculate the new gimbal angles
		newAngles = getRotationAnglesXZY(t);

		// Correct new angles
		double OldGimbal;

		OldGimbal = attitude.x;
		attitude.x += (-newAngles.x - attitude.x);
		if (attitude.x >= TWO_PI) {
			attitude.x -= TWO_PI;
		}
		if (attitude.x < 0) {
			attitude.x += TWO_PI;
		}		
		OldGimbal = attitude.y;
		attitude.y += (-newAngles.y - attitude.y);
		if (attitude.y >= TWO_PI) {
			attitude.y -= TWO_PI;
		}
		if (attitude.y < 0) {
			attitude.y += TWO_PI;
		}
		OldGimbal = attitude.z;
		attitude.z += (-newAngles.z - attitude.z);
		if (attitude.z >= TWO_PI) {
			attitude.z -= TWO_PI;
		}
		if (attitude.z < 0) {
			attitude.z += TWO_PI;
		}


		Orbiter.LastAttitude.X = Orbiter.Attitude.X;
		Orbiter.LastAttitude.Y = Orbiter.Attitude.Y;
		Orbiter.LastAttitude.Z = Orbiter.Attitude.Z;
		LastTime = simt;
	}
	// Force zero in caged/failed axes
	if(roll_bmag_failed){  attitude.x = 0; } 
	if(pitch_bmag_failed){ attitude.y = 0; } 
	if(yaw_bmag_failed){   attitude.z = 0; } 
}

// Confusing mathematics blatantly copipe from IMU
GDC_Matrix3 GDC::multiplyMatrix(GDC_Matrix3 a, GDC_Matrix3 b) {
	GDC_Matrix3 r;
	
	r.m11 = (a.m11 * b.m11) + (a.m12 * b.m21) + (a.m13 * b.m31);
	r.m12 = (a.m11 * b.m12) + (a.m12 * b.m22) + (a.m13 * b.m32);
	r.m13 = (a.m11 * b.m13) + (a.m12 * b.m23) + (a.m13 * b.m33);
	r.m21 = (a.m21 * b.m11) + (a.m22 * b.m21) + (a.m23 * b.m31);
	r.m22 = (a.m21 * b.m12) + (a.m22 * b.m22) + (a.m23 * b.m32);
	r.m23 = (a.m21 * b.m13) + (a.m22 * b.m23) + (a.m23 * b.m33);
	r.m31 = (a.m31 * b.m11) + (a.m32 * b.m21) + (a.m33 * b.m31);
	r.m32 = (a.m31 * b.m12) + (a.m32 * b.m22) + (a.m33 * b.m32);
	r.m33 = (a.m31 * b.m13) + (a.m32 * b.m23) + (a.m33 * b.m33);	
	return r;
}

GDC_Matrix3 GDC::getRotationMatrixX(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the X axis (Pitch)
	
	GDC_Matrix3 RotMatrixX;
	
	RotMatrixX.m11 = 1;
	RotMatrixX.m12 = 0;
	RotMatrixX.m13 = 0;
	RotMatrixX.m21 = 0;
	RotMatrixX.m22 = cos(angle);
	RotMatrixX.m23 = -sin(angle);
	RotMatrixX.m31 = 0;
	RotMatrixX.m32 = sin(angle);
	RotMatrixX.m33 = cos(angle);
	
	return RotMatrixX;
}

GDC_Matrix3 GDC::getRotationMatrixY(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Y axis (Yaw)

	GDC_Matrix3 RotMatrixY;
	
	RotMatrixY.m11 = cos(angle);
	RotMatrixY.m12 = 0;
	RotMatrixY.m13 = sin(angle);
	RotMatrixY.m21 = 0;
	RotMatrixY.m22 = 1;
	RotMatrixY.m23 = 0;
	RotMatrixY.m31 = -sin(angle);
	RotMatrixY.m32 = 0;
	RotMatrixY.m33 = cos(angle);
	
	return RotMatrixY;
}

GDC_Matrix3 GDC::getRotationMatrixZ(double angle) {
	// Returns the rotation matrix for a rotation of a given angle around the Z axis (Roll)

	GDC_Matrix3 RotMatrixZ;
	
	RotMatrixZ.m11 = cos(angle);
	RotMatrixZ.m12 = -sin(angle);
	RotMatrixZ.m13 = 0;
	RotMatrixZ.m21 = sin(angle);
	RotMatrixZ.m22 = cos(angle);
	RotMatrixZ.m23 = 0;
	RotMatrixZ.m31 = 0;
	RotMatrixZ.m32 = 0;
	RotMatrixZ.m33 = 1;
	
	return RotMatrixZ;	
}

void GDC::SetOrbiterAttitudeReference(){
	GDC_Matrix3 t;

	// transformation to navigation base coordinates
	// CAUTION: gimbal angles are left-handed
	t = getRotationMatrixY(-attitude.y);
	t = multiplyMatrix(getRotationMatrixZ(-attitude.z), t);
	t = multiplyMatrix(getRotationMatrixX(-attitude.x), t);
	
	// tranform to orbiter local coordinates
	t = multiplyMatrix(getNavigationBaseToOrbiterLocalTransformation(), t);
	
	// tranform to orbiter global coordinates
	t = multiplyMatrix(getRotationMatrixZ(-Orbiter.Attitude.Z), t);
	t = multiplyMatrix(getRotationMatrixY(-Orbiter.Attitude.Y), t);
	t = multiplyMatrix(getRotationMatrixX(-Orbiter.Attitude.X), t);

	Orbiter.AttitudeReference = t;
}

GDC_Matrix3 GDC::getNavigationBaseToOrbiterLocalTransformation() {
	// This transformation assumes that spacecraft azimuth - orbiter heading = 180 deg
	GDC_Matrix3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	m.m12 = 1.0;	
	m.m23 = -1.0;
	m.m31 = 1.0;

	return m;
} 

GDC_Matrix3 GDC::getOrbiterLocalToNavigationBaseTransformation() {
	// This transformation assumes that spacecraft azimuth - orbiter heading = 180 deg
	GDC_Matrix3 m;
	int i;
	
	for (i = 0; i < 9; i++) {
		m.data[i] = 0.0;
	}
	m.m13 = 1.0;
	m.m21 = 1.0;	
	m.m32 = -1.0;

	return m;
}

VECTOR3 GDC::getRotationAnglesXZY(GDC_Matrix3 m) {
	
	VECTOR3 v;
	
	v.z = asin(-m.m12);
	
	if (m.m11 * cos(v.z) > 0) {		  	
		v.y = atan(m.m13 / m.m11);
	} else {
		v.y = atan(m.m13 / m.m11) + PI;
	}
	
	if (m.m22 * cos(v.z) > 0) {
		v.x = atan(m.m32 / m.m22);
	} else {
		v.x = atan(m.m32 / m.m22) + PI;
	}
	return v;
}

bool GDC::AlignGDC(){
	// User pushed the Align GDC button.
	// Set the GDC attitude to match what's on the ASCP.
	attitude.x = (sat->ascp.output.x * 0.017453); // Degrees to radians
	attitude.y = (sat->ascp.output.y * 0.017453); // Degrees to radians
	attitude.z = (sat->ascp.output.z * 0.017453); // Degrees to radians
	// Thwack!
	Initialized = FALSE; // Resets reference
	return(true);
}

void GDC::SaveState(FILEHANDLE scn){
	oapiWriteLine(scn, GDC_START_STRING);
	oapiWriteScenario_float(scn, "ATX", attitude.x);
	oapiWriteScenario_float(scn, "ATY", attitude.y);
	oapiWriteScenario_float(scn, "ATZ", attitude.z);
	oapiWriteScenario_float(scn, "OAX", Orbiter.Attitude.X);
	oapiWriteScenario_float(scn, "OAY", Orbiter.Attitude.Y);
	oapiWriteScenario_float(scn, "OAZ", Orbiter.Attitude.Z);
	oapiWriteScenario_float(scn, "LAX", Orbiter.LastAttitude.X);
	oapiWriteScenario_float(scn, "LAY", Orbiter.LastAttitude.Y);
	oapiWriteScenario_float(scn, "LAZ", Orbiter.LastAttitude.Z);
	oapiWriteScenario_float(scn, "M11", Orbiter.AttitudeReference.m11);
	oapiWriteScenario_float(scn, "M12", Orbiter.AttitudeReference.m12);
	oapiWriteScenario_float(scn, "M13", Orbiter.AttitudeReference.m13);
	oapiWriteScenario_float(scn, "M21", Orbiter.AttitudeReference.m21);
	oapiWriteScenario_float(scn, "M22", Orbiter.AttitudeReference.m22);
	oapiWriteScenario_float(scn, "M23", Orbiter.AttitudeReference.m23);
	oapiWriteScenario_float(scn, "M31", Orbiter.AttitudeReference.m31);
	oapiWriteScenario_float(scn, "M32", Orbiter.AttitudeReference.m32);
	oapiWriteScenario_float(scn, "M33", Orbiter.AttitudeReference.m33);
	oapiWriteScenario_float(scn, "LTM", LastTime);
	oapiWriteLine(scn, GDC_END_STRING);
}

void GDC::LoadState(FILEHANDLE scn){
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, GDC_END_STRING, sizeof(GDC_END_STRING))){
			Initialized = TRUE;
			return;
		}
		if (!strnicmp (line, "ATX", 3)) {
			sscanf(line + 3, "%f", &flt);
			attitude.x = flt;
		}
		else if (!strnicmp (line, "ATY", 3)) {
			sscanf(line + 3, "%f", &flt);
			attitude.y = flt;
		}
		else if (!strnicmp (line, "ATZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			attitude.z = flt;
		}
		else if (!strnicmp (line, "OAX", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.Attitude.X = flt;
		}
		else if (!strnicmp (line, "OAY", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.Attitude.Y = flt;
		}
		else if (!strnicmp (line, "OAZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.Attitude.Z = flt;
		}
		else if (!strnicmp (line, "LAX", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.LastAttitude.X = flt;
		}
		else if (!strnicmp (line, "LAY", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.LastAttitude.Y = flt;
		}
		else if (!strnicmp (line, "LAZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.LastAttitude.Z = flt;
		}
		else if (!strnicmp (line, "M11", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m11 = flt;
		}
		else if (!strnicmp (line, "M12", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m12 = flt;
		}
		else if (!strnicmp (line, "M13", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m13 = flt;
		}
		else if (!strnicmp (line, "M21", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m21 = flt;
		}
		else if (!strnicmp (line, "M22", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m22 = flt;
		}
		else if (!strnicmp (line, "M23", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m23 = flt;
		}
		else if (!strnicmp (line, "M31", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m31 = flt;
		}
		else if (!strnicmp (line, "M32", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m32 = flt;
		}
		else if (!strnicmp (line, "M33", 3)) {
			sscanf(line + 3, "%f", &flt);
			Orbiter.AttitudeReference.m33 = flt;
		}
		else if (!strnicmp (line, "LTM", 3)) {
			sscanf(line + 3, "%f", &flt);
			LastTime = flt;
		}		
	}
	// We're done, so call this initialized
	Initialized = TRUE;
}

// ASCP
ASCP::ASCP(){
	output.x = 0;
	output.y = 0;
	output.z = 0;
	sat = NULL;
}

void ASCP::Init(Saturn *vessel){
	sat = vessel;
}

void ASCP::TimeStep(){
	if(msgcounter > 0){
		msgcounter--;
		if(msgcounter==0){
			sprintf(oapiDebugString(),""); // Clear message
		}
	}
}

bool ASCP::RollDisplayClicked(){
	msgcounter = 50; // Keep for 50 timesteps
	sprintf(oapiDebugString(),"ASCP: Roll = %05.1f",output.x);
	return true;
}

bool ASCP::PitchDisplayClicked(){
	msgcounter = 50;
	sprintf(oapiDebugString(),"ASCP: Pitch = %05.1f",output.y);
	return true;
}

bool ASCP::YawDisplayClicked(){
	msgcounter = 50;
	sprintf(oapiDebugString(),"ASCP: Yaw = %05.1f",output.z);
	return true;
}

bool ASCP::RollUpClick(int Event){
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedowncounter++;
			if(mousedowncounter==1){
				changed = true;
				output.x++;				
			}else{
				if(mousedowncounter == 10){
					output.x++;
					changed = true;
					mousedowncounter = 5;
				}
			}
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.x += 0.1;
				changed = true;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0; return false; break;
	}
	// Wrap around
	if(output.x > 360){ output.x -= 360; }
	RollDisplayClicked();
	return changed;
}

bool ASCP::RollDnClick(int Event){
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedowncounter++;
			if(mousedowncounter==1){
				changed = true;
				output.x--;				
			}else{
				if(mousedowncounter == 10){
					output.x--;
					changed = true;
					mousedowncounter = 5;
				}
			}
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.x -= 0.1;
				changed = true;
				mousedowncounter = 2;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0; return false; break;
	}
	// Wrap around
	if(output.x < 0){ output.x += 360; }
	RollDisplayClicked();
	return changed;
}

bool ASCP::PitchUpClick(int Event){
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedowncounter++;
			if(mousedowncounter==1){
				changed = true;
				output.y++;				
			}else{
				if(mousedowncounter == 10){
					output.y++;
					changed = true;
					mousedowncounter = 5;
				}
			}
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.y += 0.1;
				changed = true;
				mousedowncounter = 2;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0; return false; break;
	}
	// Wrap around
	if(output.y >= 360){ output.y -= 360; }
	PitchDisplayClicked();
	return changed;
}

bool ASCP::PitchDnClick(int Event){
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedowncounter++;
			if(mousedowncounter==1){
				changed = true;
				output.y--;				
			}else{
				if(mousedowncounter == 10){
					output.y--;
					changed = true;
					mousedowncounter = 5;
				}
			}
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.y -= 0.1;
				changed = true;
				mousedowncounter = 2;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0; return false; break;
	}
	// Wrap around
	if(output.y < 0){ output.y += 360; }
	PitchDisplayClicked();
	return changed;
}

bool ASCP::YawUpClick(int Event){
	// Cannot click beyond 90 degrees.
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedowncounter++;
			if(mousedowncounter==1){
				changed = true;
				output.z++;				
			}else{
				if(mousedowncounter == 10){
					output.z++;
					changed = true;
					mousedowncounter = 5;
				}
			}
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.z += 0.1;
				changed = true;
				mousedowncounter = 2;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0; return false; break;
	}
	if(output.z > 90 && output.z < 270){ // Can't get here
		output.z = 90;
	}	
	// Wrap around zero
	if(output.z >= 360){ output.z -= 360; }
	YawDisplayClicked();
	return changed;
}

bool ASCP::YawDnClick(int Event){
	// Cannot click beyond 270 degrees.
	bool changed = false;
	switch(Event){
		case PANEL_MOUSE_LBPRESSED:
			mousedowncounter++;
			if(mousedowncounter==1){
				changed = true;
				output.z--;				
			}else{
				if(mousedowncounter == 10){
					output.z--;
					changed = true;
					mousedowncounter = 5;
				}
			}
			break;
		case PANEL_MOUSE_RBPRESSED:
			mousedowncounter++;
			if(mousedowncounter == 1){
				output.z -= 0.1;
				changed = true;
				mousedowncounter = 2;
			}
			break;
		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			mousedowncounter = 0; return false; break;
	}
	if(output.z < 270 && output.z > 90){ // Can't get here
		output.z = 270;
	}	
	// Wrap around zero
	if(output.z < 0){ output.z += 360; }
	YawDisplayClicked();
	return changed;
}



bool ASCP::PaintRollDisplay(SURFHANDLE surf, SURFHANDLE digits){
	char cheat[10];                       // Have plenty of room for this
	int srx,sry,beta,digit;
	sprintf(cheat,"%+06.1f",output.x);    // Arithmetic is for suckers!	
	srx = 8+((cheat[1]-0x30)*25);	      // Hint: 0x30 = ASCII "0"
	oapiBlt (surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8+((cheat[2]-0x30)*25);
	oapiBlt (surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	digit = cheat[3]-0x30; srx = 8+(digit*25); beta = cheat[5]-0x30; sry = (int)(beta*1.2);
	if(beta == 0){		
		oapiBlt (surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	}else{
		oapiBlt (surf, digits, 20, sry, srx, 33, 9, 12-sry, SURF_PREDEF_CK);			
		if(digit == 9){digit=0;}else{digit++;}
		srx = 8+(digit*25);			
		oapiBlt (surf, digits, 20, 0, srx, 45-sry, 9, sry, SURF_PREDEF_CK);
	}
	return true;
}

bool ASCP::PaintPitchDisplay(SURFHANDLE surf, SURFHANDLE digits){
	char cheat[10];                       // Have plenty of room for this
	int srx,sry,beta,digit;
	sprintf(cheat,"%+06.1f",output.y);    // Arithmetic is for suckers!	
	srx = 8+((cheat[1]-0x30)*25);	      // Hint: 0x30 = ASCII "0"
	oapiBlt (surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8+((cheat[2]-0x30)*25);
	oapiBlt (surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	digit = cheat[3]-0x30; srx = 8+(digit*25); beta = cheat[5]-0x30; sry = (int)(beta*1.2);
	if(beta == 0){		
		oapiBlt (surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	}else{
		oapiBlt (surf, digits, 20, sry, srx, 33, 9, 12-sry, SURF_PREDEF_CK);			
		if(digit == 9){digit=0;}else{digit++;}
		srx = 8+(digit*25);			
		oapiBlt (surf, digits, 20, 0, srx, 45-sry, 9, sry, SURF_PREDEF_CK);
	}
	return true;
}

bool ASCP::PaintYawDisplay(SURFHANDLE surf, SURFHANDLE digits){
	char cheat[10];                       // Have plenty of room for this
	int srx,sry,beta,digit;
	sprintf(cheat,"%+06.1f",output.z);    // Arithmetic is for suckers!	
	srx = 8+((cheat[1]-0x30)*25);	      // Hint: 0x30 = ASCII "0"
	oapiBlt (surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8+((cheat[2]-0x30)*25);
	oapiBlt (surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	digit = cheat[3]-0x30; srx = 8+(digit*25); beta = cheat[5]-0x30; sry = (int)(beta*1.2);
	if(beta == 0){		
		oapiBlt (surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	}else{
		oapiBlt (surf, digits, 20, sry, srx, 33, 9, 12-sry, SURF_PREDEF_CK);			
		if(digit == 9){digit=0;}else{digit++;}
		srx = 8+(digit*25);			
		oapiBlt (surf, digits, 20, 0, srx, 45-sry, 9, sry, SURF_PREDEF_CK);
	}
	return true;
}

void ASCP::SaveState(FILEHANDLE scn){
	oapiWriteLine(scn, ASCP_START_STRING);
	oapiWriteScenario_float(scn, "OPX", output.x);
	oapiWriteScenario_float(scn, "OPY", output.y);
	oapiWriteScenario_float(scn, "OPZ", output.z);
	oapiWriteLine(scn, ASCP_END_STRING);
}

void ASCP::LoadState(FILEHANDLE scn){
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, ASCP_END_STRING, sizeof(ASCP_END_STRING))){
			return;
		}
		if (!strnicmp (line, "OPX", 3)) {
			sscanf(line + 3, "%f", &flt);
			output.x = flt;
		}
		else if (!strnicmp (line, "OPY", 3)) {
			sscanf(line + 3, "%f", &flt);
			output.y = flt;
		}
		else if (!strnicmp (line, "OPZ", 3)) {
			sscanf(line + 3, "%f", &flt);
			output.z = flt;
		}
	}
}


// EDA
EDA::EDA(){
	sat = NULL; // Initialize
}

void EDA::Init(Saturn *vessel){
	sat = vessel;
}

VECTOR3 EDA::ReturnCMCErrorNeedles(){
	VECTOR3 errors;
	errors.x = sat->gdc.fdai_err_x * 0.106770; // CMC error value, CMC-scaled
	errors.y = sat->gdc.fdai_err_y * 0.106770; // CMC error value, CMC-scaled
	errors.z = sat->gdc.fdai_err_z * 0.106770; // CMC error value, CMC-scaled
	return(errors);
}

VECTOR3 EDA::ReturnASCPError(VECTOR3 attitude){
	VECTOR3 setting,target,errors;
	// Get ASCP setting in radians
	setting.x = sat->ascp.output.x * 0.017453;
	setting.y = sat->ascp.output.y * 0.017453;
	setting.z = sat->ascp.output.z * 0.017453;
	// And difference
	target.x = setting.x - attitude.x;
	target.y = setting.y - attitude.y;
	target.z = setting.z - attitude.z;							
	// Now process
	switch(sat->FDAIScaleSwitch.GetState()){
		case THREEPOSSWITCH_UP:
		case THREEPOSSWITCH_CENTER:
			// 5 degree rate
			if(target.x > 0){ // Positive Error
				if(target.x > PI){ 
					errors.x = -((TWO_PI-target.x) * 469.827882); }else{
						errors.x = (target.x * 469.827882);	}
			}else{
				if(target.x < -PI){
					errors.x = ((TWO_PI+target.x) * 469.827882); }else{
						errors.x = (target.x * 469.827882);	}
			}
			if(target.y > 0){ 
				if(target.y > PI){ 
					errors.y = ((TWO_PI-target.y) * 469.827882); }else{
						errors.y = -(target.y * 469.827882);	}
			}else{
				if(target.y < -PI){
					errors.y = -((TWO_PI+target.y) * 469.827882); }else{
						errors.y = -(target.y * 469.827882);	}
			}
			if(target.z > 0){ 
				if(target.z > PI){ 
					errors.z = -((TWO_PI-target.z) * 469.827882); }else{
						errors.z = (target.z * 469.827882);	}
			}else{
				if(target.z < -PI){
					errors.z = ((TWO_PI+target.z) * 469.827882); }else{
						errors.z = (target.z * 469.827882);	}
			}											
			break;
		case THREEPOSSWITCH_DOWN:
			// 50/15/15 degree rate
			if(target.x > 0){ // Positive Error
				if(target.x > PI){ 
					errors.x = -((TWO_PI-target.x) * 46.982572); }else{
						errors.x = (target.x * 46.982572);	}
			}else{
				if(target.x < -PI){
					errors.x = ((TWO_PI+target.x) * 46.982572); }else{
						errors.x = (target.x * 46.982572);	}
			}
			if(target.y > 0){ 
				if(target.y > PI){ 
					errors.y = ((TWO_PI-target.y) * 156.608695); }else{
						errors.y = -(target.y * 156.608695);	}
			}else{
				if(target.y < -PI){
					errors.y = -((TWO_PI+target.y) * 156.608695); }else{
						errors.y = -(target.y * 156.608695);	}
			}
			if(target.z > 0){ 
				if(target.z > PI){ 
					errors.z = -((TWO_PI-target.z) * 156.608695); }else{
						errors.z = (target.z * 156.608695);	}
			}else{
				if(target.z < -PI){
					errors.z = ((TWO_PI+target.z) * 156.608695); }else{
						errors.z = (target.z * 156.608695);	}
			}											
			break;
	}
	return(errors);
}

VECTOR3 EDA::ReturnBMAG1Error(){
	VECTOR3 errors;
	switch(sat->FDAIScaleSwitch.GetState()){
		case THREEPOSSWITCH_UP:
		case THREEPOSSWITCH_CENTER:
			// 5 degree rate
			if(sat->BMAGRollSwitch.GetState() != THREEPOSSWITCH_UP || sat->bmag1.powered != FALSE){
				if(sat->gdc.attitude.x > 3.141592){ // > 180?								
					errors.x = ((TWO_PI-sat->gdc.attitude.x) * 469.827882); // Convert to left error
				}else{
					errors.x = -(sat->gdc.attitude.x * 469.827882);
				}
			}
			if(sat->BMAGPitchSwitch.GetState() != THREEPOSSWITCH_UP || sat->bmag1.powered != FALSE){
				if(sat->gdc.attitude.y > 3.141592){ // > 180?								
					errors.y = -((TWO_PI-sat->gdc.attitude.y) * 469.827882); // Convert to left error
				}else{
					errors.y = (sat->gdc.attitude.y * 469.827882);
				}
			}
			if(sat->BMAGYawSwitch.GetState() != THREEPOSSWITCH_UP || sat->bmag1.powered != FALSE){
				if(sat->gdc.attitude.z > 3.141592){ // > 180?								
					errors.z = ((TWO_PI-sat->gdc.attitude.z) * 469.827882); // Convert to left error
				}else{
					errors.z = -(sat->gdc.attitude.z * 469.827882);
				}
			}
			break;
		case THREEPOSSWITCH_DOWN:
			// 50/15/15 degree rate
			if(sat->BMAGRollSwitch.GetState() != THREEPOSSWITCH_UP || sat->bmag1.powered != FALSE){
				if(sat->gdc.attitude.x > 3.141592){ // > 180?								
					errors.x = ((TWO_PI-sat->gdc.attitude.x) * 46.982572); // Convert to left error
				}else{
					errors.x = -(sat->gdc.attitude.x * 46.982572);
				}
			}
			if(sat->BMAGPitchSwitch.GetState() != THREEPOSSWITCH_UP || sat->bmag1.powered != FALSE){
				if(sat->gdc.attitude.y > 3.141592){ // > 180?								
					errors.y = -((TWO_PI-sat->gdc.attitude.y) * 156.608695); // Convert to left error
				}else{
					errors.y = (sat->gdc.attitude.y * 156.608695);
				}
			}
			if(sat->BMAGYawSwitch.GetState() != THREEPOSSWITCH_UP || sat->bmag1.powered != FALSE){
				if(sat->gdc.attitude.z > 3.141592){ // > 180?								
					errors.z = ((TWO_PI-sat->gdc.attitude.z) * 156.608695); // Convert to left error
				}else{
					errors.z = -(sat->gdc.attitude.z * 156.608695);
				}
			}
			break;
	}
	return(errors);
}

VECTOR3 EDA::AdjustErrorsForRoll(VECTOR3 attitude, VECTOR3 errors){
	VECTOR3 output_errors;
	double input_pitch = errors.y;
	double input_yaw = errors.z;
	double roll_percent,output_pitch,output_yaw,pitch_factor = 1;
	// In reality, PITCH and YAW are swapped around as needed to make the error needles  FLY-TO.
	// This does that.
	// ROLL IS LEFT-HANDED
	if(attitude.x == 0){ // If zero or inop, return unmodified to avoid SPECIAL CASE
		return(attitude);
	}
	if(attitude.x > 4.712388){                    // 0 thru 90 degrees
		roll_percent = fabs((attitude.x-TWO_PI) / 1.570796);				
		output_pitch = input_pitch * (1-roll_percent); 
		output_pitch += input_yaw * roll_percent;
		output_yaw = input_yaw * (1-roll_percent);
		output_yaw -=input_pitch * roll_percent;       
	}
	if(attitude.x > PI && attitude.x < 4.712388){ // 90 thru 180 degrees
		roll_percent = (attitude.x-PI) / 1.570796;					
		output_pitch = -(input_pitch * (1-roll_percent)); 
		output_pitch += input_yaw * roll_percent;
		output_yaw = -input_yaw * (1-roll_percent);
		output_yaw -=input_pitch * roll_percent;       
	}
	if(attitude.x > 1.570796 && attitude.x < PI){ // 180 thru 270 degrees
		roll_percent = fabs((attitude.x-PI) / 1.570796);
		output_pitch = -(input_pitch * (1-roll_percent)); 
		output_pitch -= input_yaw * roll_percent;
		output_yaw = -input_yaw * (1-roll_percent);
		output_yaw +=input_pitch * roll_percent;       
	}
	if(attitude.x > 0 && attitude.x < 1.570796){ // 270 thru 360 degrees
		roll_percent = attitude.x / 1.570796;					
		output_pitch = input_pitch * (1-roll_percent); 
		output_pitch -= input_yaw * roll_percent;
		output_yaw = input_yaw * (1-roll_percent);
		output_yaw +=input_pitch * roll_percent;       
	}

	//sprintf(oapiDebugString(),"Roll Att %f Percent = %f | P-I %f P-O %f | Y-I %f Y-O %f",
	//	attitude.x,roll_percent,input_pitch,output_pitch,input_yaw,output_yaw);

	output_errors.x = errors.x;
	output_errors.y = output_pitch;
	output_errors.z = output_yaw;
	return(output_errors);
}

// Reaction Jet / Engine Control
RJEC::RJEC(){
	sat = NULL;
	int x=0;
	while(x<20){
		ThrusterDemand[x] = 0;
		x++;
	}
}

void RJEC::Init(Saturn *vessel){
	sat = vessel;
}

void RJEC::SystemTimestep(double simdt) {

	// Ensure AC power
	if (sat->SIGCondDriverBiasPower1Switch.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->SIGCondDriverBiasPower2Switch.Voltage() < SP_MIN_ACVOLTAGE) return;

	sat->SIGCondDriverBiasPower1Switch.DrawPower(2.3); 
	sat->SIGCondDriverBiasPower2Switch.DrawPower(0.2); 	
}

void RJEC::TimeStep(){
	/* Thruster List:
	CM#		SM#		INDEX#		SWITCH GROUP		ROT AXIS

	1		C3		1			PITCH				+PITCH
	2		A4		2			PITCH				-PITCH
	3		A3		3			PITCH				+PITCH
	4		C4		4			PITCH				-PITCH
	5		D3		5			YAW					+YAW
	6		B4		6			YAW					-YAW
	7		B3		7			YAW					+YAW
	8		D4		8			YAW					-YAW
	9		B1		9			ROLL B/D			+ROLL
	10		D2		10			ROLL B/D			-ROLL
	11		D1		11			ROLL B/D			+ROLL
	12		B2		12			ROLL B/D			-ROLL
	xx		A1		13			ROLL A/C			+ROLL
	xx		A2		14			ROLL A/C			-ROLL
	xx		C1		15			ROLL A/C			+ROLL
	xx		C2		16			ROLL A/C			-ROLL

	*/

	// Ensure AC power
	if (sat->SIGCondDriverBiasPower1Switch.Voltage() < SP_MIN_ACVOLTAGE || 
	    sat->SIGCondDriverBiasPower2Switch.Voltage() < SP_MIN_ACVOLTAGE) return;
	
	ChannelValue30 val30;
	int sm_sep=0;
	val30.Value = sat->agc.GetInputChannel(030); 
	sm_sep = val30.Bits.CMSMSeperate; // There should probably be a way for the SCS to do this if VAGC is not running
	int thruster = 1;
	int thruster_lockout;

	while(thruster < 17){
		// THRUSTER LOCKOUT CHECKING
		thruster_lockout = 0;
		// If it's a pitch or yaw jet, lockout on SPS thrusting
		if(thruster < 9 && SPSActive != 0){ thruster_lockout = 1; } 
		// Lockout on direct axes.
		if(thruster < 5 && DirectPitchActive != 0){ thruster++; continue; } // Skip entirely
		if(thruster > 4 && thruster < 9 && DirectYawActive != 0){ thruster++; continue; } 
		if(thruster > 8 && DirectRollActive != 0){ thruster++; continue; } 
		// THRUSTER PROCESSING
		switch(thruster){
			case 1:
				if(sat->PitchC3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchC3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(0,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 3, 0); 
					}else{
						sat->SetCMRCSState(0,0);
					}
				}
				break;

			case 2:
				if(sat->PitchA4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchA4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(2,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 4, 0); 
					}else{
						sat->SetCMRCSState(2,0);
					}
				}
				break;

			case 3:
				if(sat->PitchA3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchA3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(1,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 3, 0); 
					}else{
						sat->SetCMRCSState(1,0);
					}
				}
				break;

			case 4:
				if(sat->PitchC4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->PitchC4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(3,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 4, 0); 
					}else{
						sat->SetCMRCSState(3,0);
					}
				}
				break;

			case 5:
				if(sat->YawD3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawD3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(4,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 3, 0); 
					}else{
						sat->SetCMRCSState(4,0);
					}
				}
				break;

			case 6:
				if(sat->YawB4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawB4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(6,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 4, 0); 
					}else{
						sat->SetCMRCSState(6,0);
					}
				}
				break;

			case 7:
				if(sat->YawB3Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawB3Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 3, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(5,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 3, 0); 
					}else{
						sat->SetCMRCSState(5,0);
					}
				}
				break;

			case 8:
				if(sat->YawD4Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->YawD4Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 4, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(7,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 4, 0); 
					}else{
						sat->SetCMRCSState(7,0);
					}
				}
				break;

			case 9:
				if(sat->BdRollB1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollB1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 1, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(8,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 1, 0); 
					}else{
						sat->SetCMRCSState(8,0);
					}
				}
				break;

			case 10:
				if(sat->BdRollD2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollD2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 2, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(10,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 2, 0); 
					}else{
						sat->SetCMRCSState(10,0);
					}
				}
				break;

			case 11:
				if(sat->BdRollD1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollD1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 1, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(9,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_D, 1, 0); 
					}else{
						sat->SetCMRCSState(9,0);
					}
				}
				break;

			case 12:
				if(sat->BdRollB2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->BdRollB2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 2, ThrusterDemand[thruster]); 
					}else{
						sat->SetCMRCSState(11,ThrusterDemand[thruster]);
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_B, 2, 0); 
					}else{
						sat->SetCMRCSState(11,0);
					}
				}
				break;

			case 13:
				if(sat->AcRollA1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollA1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 1, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 1, 0); 
					}
				}
				break;

			case 14:
				if(sat->AcRollA2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollA2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 2, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_A, 2, 0); 
					}
				}
				break;

			case 15:
				if(sat->AcRollC1Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollC1Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 1, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 1, 0); 
					}
				}
				break;

			case 16:
				if(sat->AcRollC2Switch.Voltage() > 20 && thruster_lockout == 0){
					if(ThrusterDemand[thruster] != 0){ sat->AcRollC2Switch.DrawPower(50); }
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 2, ThrusterDemand[thruster]); 
					}
				}else{
					if(!sm_sep){
						sat->SetRCSState(RCS_SM_QUAD_C, 2, 0); 
					}
				}
				break;
		}
		thruster++;
	}
		
}

void RJEC::SetThruster(int thruster,bool Active){
	if(thruster > 0 && thruster < 20){
		ThrusterDemand[thruster] = Active; // Next timestep does the work
	}
}

// Electronic Control Assembly
ECA::ECA(){

	rhc_x = 32768;
	rhc_y = 32768;
	rhc_z = 32768;
	thc_x = 32768;
	thc_y = 32768;
	thc_z = 32768;

	accel_roll_trigger = 0;
	mnimp_roll_trigger = 0;
	accel_pitch_trigger = 0;
	mnimp_pitch_trigger = 0;
	accel_yaw_trigger = 0;
	mnimp_yaw_trigger = 0;
	trans_x_trigger = 0;
	trans_y_trigger = 0;
	trans_z_trigger = 0;

	sat = NULL;
}

void ECA::Init(Saturn *vessel){
	sat = vessel;
}

void ECA::SystemTimestep(double simdt) {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return;  // Switched off

	// Ensure DC power
	if (sat->SystemMnACircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || 
	    sat->SystemMnBCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE) return;

	sat->SystemMnACircuitBraker.DrawPower(10);	// TODO real power is unknown
	sat->SystemMnBCircuitBraker.DrawPower(10);	// TODO real power is unknown
}

void ECA::TimeStep() {

	// Do we have power?
	if (sat->SCSElectronicsPowerRotarySwitch.GetState() == 0) return;  // Switched off

	if (sat->SystemMnACircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || 
	    sat->SystemMnBCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE) return;

	// SCS is in control if the THC CLOCKWISE line is high (not implemented)
	// or if the SC CONT switch is set to SCS.
	int accel_roll_flag = 0;
	int mnimp_roll_flag = 0;
	int accel_pitch_flag = 0;
	int mnimp_pitch_flag = 0;
	int accel_yaw_flag = 0;
	int mnimp_yaw_flag = 0;
	VECTOR3 cmd_rate = _V(0,0,0);
	VECTOR3 rate_err = _V(0,0,0);
	if(mnimp_roll_trigger){
		sat->rjec.SetThruster(9,0);
		sat->rjec.SetThruster(10,0);
		sat->rjec.SetThruster(11,0);
		sat->rjec.SetThruster(12,0);
		sat->rjec.SetThruster(13,0);
		sat->rjec.SetThruster(14,0);
		sat->rjec.SetThruster(15,0);
		sat->rjec.SetThruster(16,0); 
	}
	if(mnimp_pitch_trigger){
		sat->rjec.SetThruster(1,0);
		sat->rjec.SetThruster(2,0);
		sat->rjec.SetThruster(3,0);
		sat->rjec.SetThruster(4,0);
	}
	if(mnimp_yaw_trigger){
		sat->rjec.SetThruster(5,0);
		sat->rjec.SetThruster(6,0);
		sat->rjec.SetThruster(7,0);
		sat->rjec.SetThruster(8,0);
	}
	// ERROR DETERMINATION
	VECTOR3 setting,target,errors;
	if(sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN){
		// Get ASCP setting in radians
		setting.x = sat->ascp.output.x * 0.017453;
		setting.y = sat->ascp.output.y * 0.017453;
		setting.z = sat->ascp.output.z * 0.017453;
		// And difference from GDC attitude (plus rate)
		target.x = setting.x - (sat->gdc.attitude.x + sat->gdc.rates.z);
		target.y = setting.y - (sat->gdc.attitude.y + sat->gdc.rates.x);
		target.z = setting.z - (sat->gdc.attitude.z - sat->gdc.rates.y); // Yaw rate points the wrong way.
		// Now process
		if(target.x > 0){ // Positive Error
			if(target.x > PI){ 
				errors.x = -(TWO_PI-target.x); }else{ errors.x = target.x;	}
		}else{
			if(target.x < -PI){
				errors.x = TWO_PI+target.x; }else{ errors.x = target.x;	}
		}
		if(target.y > 0){ 
			if(target.y > PI){ 
				errors.y = TWO_PI-target.y; }else{ errors.y = -target.y;	}
		}else{
			if(target.y < -PI){
				errors.y = -(TWO_PI+target.y); }else{ errors.y = -target.y;	}
		}
		if(target.z > 0){ 
			if(target.z > PI){ 
				errors.z = -(TWO_PI-target.z); }else{ errors.z = target.z;	}
		}else{
			if(target.z < -PI){
				errors.z = TWO_PI+target.z; }else{ errors.z = target.z;	}
		}
		// Now adjust for rotation
		errors = sat->eda.AdjustErrorsForRoll(sat->gdc.attitude,errors);
		// Create demand for rate
		switch(sat->AttRateSwitch.GetState()){
			case TOGGLESWITCH_UP:   // HIGH RATE
				// Are we in or out of deadband?
				switch(sat->AttDeadbandSwitch.GetState()){
					case TOGGLESWITCH_UP:   // MAX
						// 8 degrees attitude deadband
						if(errors.x < -0.13962634){
							cmd_rate.x = -0.0610865238; 
						}
						if(errors.x > 0.13962634){
							cmd_rate.x = 0.0610865238; 
						}
						if(errors.y < -0.13962634){
							cmd_rate.y = 0.0436332313; 
						}
						if(errors.y > 0.13962634){
							cmd_rate.y = -0.0436332313; 
						}
						if(errors.z < -0.13962634){
							cmd_rate.z = 0.0436332313; 
						}
						if(errors.z > 0.13962634){
							cmd_rate.z = -0.0436332313; 
						}
						break;
					case TOGGLESWITCH_DOWN: // MIN
						// 4 degrees attitude deadband
						if(errors.x < -0.0698131701){
							cmd_rate.x = -0.0610865238; 
						}
						if(errors.x > 0.0698131701){
							cmd_rate.x = 0.0610865238; 
						}
						if(errors.y < -0.0698131701){
							cmd_rate.y = 0.0436332313; 
						}
						if(errors.y > 0.0698131701){
							cmd_rate.y = -0.0436332313; 
						}
						if(errors.z < -0.0698131701){
							cmd_rate.z = 0.0436332313; 
						}
						if(errors.z > 0.0698131701){
							cmd_rate.z = -0.0436332313; 
						}
						break;
				}
				break;
			case TOGGLESWITCH_DOWN: // LOW RATE
				// Are we in or out of deadband?
				switch(sat->AttDeadbandSwitch.GetState()){
					case TOGGLESWITCH_UP:   // MAX
						// 4.2 degrees attitude deadband
						if(errors.x < -0.0733038286){
							cmd_rate.x = -0.00872664626; 
						}
						if(errors.x > 0.0733038286){
							cmd_rate.x = 0.00872664626; 
						}
						if(errors.y < -0.0733038286){
							cmd_rate.y = 0.00872664626; 
						}
						if(errors.y > 0.0733038286){
							cmd_rate.y = -0.00872664626; 
						}
						if(errors.z < -0.0733038286){
							cmd_rate.z = 0.00523598776; 
						}
						if(errors.z > 0.0733038286){
							cmd_rate.z = -0.00523598776; 
						}
						break;
					case TOGGLESWITCH_DOWN: // MIN
						// 0.2 degrees attitude deadband
						if(errors.x < -0.0034906585){
							cmd_rate.x = -0.00872664626; 
						}
						if(errors.x > 0.0034906585){
							cmd_rate.x = 0.00872664626; 
						}
						if(errors.y < -0.0034906585){
							cmd_rate.y = 0.00872664626; 
						}
						if(errors.y > 0.0034906585){
							cmd_rate.y = -0.00872664626; 
						}
						if(errors.z < -0.0034906585){
							cmd_rate.z = 0.00523598776; 
						}
						if(errors.z > 0.0034906585){
							cmd_rate.z = -0.00523598776; 
						}
						break;
				}
				break;
		}
		// Proportional Rate Demand
		int x_def=0,y_def=0,z_def=0;
		if(rhc_x < 28673 && rhc_x > 2738){ // MINUS 
			x_def = 28673-rhc_x; // Results are 25935 - 0
		}
		if(rhc_x > 36863 && rhc_x < 62798){ // PLUS 
			x_def = (36863-rhc_x);
		}
		if(rhc_y < 28673 && rhc_y > 2738){ // MINUS 
			y_def = 28673-rhc_y; // Results are 25935 - 0
		}
		if(rhc_y > 36863 && rhc_y < 62798){ // PLUS 
			y_def = (36863-rhc_y);
		}
		if(rhc_z < 28673 && rhc_z > 2738){ // MINUS 
			z_def = 28673-rhc_z; // Results are 25935 - 0
		}
		if(rhc_z > 36863 && rhc_z < 62798){ // PLUS 
			z_def = (36863-rhc_z);
		}
		double axis_percent=0;
		switch(sat->AttRateSwitch.GetState()){
			case TOGGLESWITCH_UP:    // HIGH RATE
				// MAX RATE 7 dps pitch/yaw, 20 dps roll
				if(x_def != 0){ 
					axis_percent = (double)x_def / (double)25935;
					cmd_rate.x = -(0.34906585 * axis_percent);	// OVERRIDE
				}
				if(y_def != 0){ 
					axis_percent = (double)y_def / (double)25935;
					cmd_rate.y = -(0.122173048 * axis_percent);	// OVERRIDE
				}
				if(z_def != 0){ 
					axis_percent = (double)z_def / (double)25935;
					cmd_rate.z = (0.122173048 * axis_percent);	// OVERRIDE
				}
				break;
			case TOGGLESWITCH_DOWN:  // LOW RATE
				// MAX RATE .7 dps roll/pitch/yaw 
				if(x_def != 0){ 
					axis_percent = (double)x_def / (double)25935;
					cmd_rate.x = -(0.0122173048 * axis_percent);	// OVERRIDE
				}
				if(y_def != 0){ 
					axis_percent = (double)y_def / (double)25935;
					cmd_rate.y = -(0.0122173048 * axis_percent);	// OVERRIDE
				}
				if(z_def != 0){ 
					axis_percent = (double)z_def / (double)25935;
					cmd_rate.z = (0.0122173048 * axis_percent);	// OVERRIDE
				}
				break;
		}
		// RATE DAMPING
		// If not overridden by something else (Proportional mode or attitude errors)
		// and ATT mode is off, do this.
		// This used to check for BMAG switches being out of center. Now it checks for the presence of a failed
		// BMAG.
		if(cmd_rate.x == 0 && sat->gdc.roll_bmag_failed != 0){ 
			switch(sat->AttRateSwitch.GetState()){
				case TOGGLESWITCH_UP:    // HIGH RATE
					// MAX RATE 20 dps roll
					if(sat->gdc.rates.z >  0.34906585){	cmd_rate.x = 0.34906585 - sat->gdc.rates.z; break; }
					if(sat->gdc.rates.z < -0.34906585){ cmd_rate.x = -0.34906585- sat->gdc.rates.z; break; }
					cmd_rate.x = sat->gdc.rates.z; 
					break;
				case TOGGLESWITCH_DOWN:  // LOW RATE
					// MAX RATE .7 dps roll
					if(sat->gdc.rates.z >  0.0122173048){ cmd_rate.x = 0.0122173048 - sat->gdc.rates.z; break; }
					if(sat->gdc.rates.z < -0.0122173048){ cmd_rate.x = -0.0122173048- sat->gdc.rates.z; break; }
					cmd_rate.x = sat->gdc.rates.z; 
					break;
			}
		}
		if(cmd_rate.y == 0 && sat->gdc.pitch_bmag_failed != 0){ 
			switch(sat->AttRateSwitch.GetState()){
				case TOGGLESWITCH_UP:    // HIGH RATE
					// MAX RATE 7 dps
					if(sat->gdc.rates.x >  0.122173048){ cmd_rate.y = 0.122173048 - sat->gdc.rates.x; break; }
					if(sat->gdc.rates.x < -0.122173048){ cmd_rate.y = -0.122173048- sat->gdc.rates.x; break; }
					cmd_rate.y = sat->gdc.rates.x; 
					break;
				case TOGGLESWITCH_DOWN:  // LOW RATE
					// MAX RATE .7 dps
					if(sat->gdc.rates.x >  0.0122173048){ cmd_rate.y = 0.0122173048 - sat->gdc.rates.x; break; }
					if(sat->gdc.rates.x < -0.0122173048){ cmd_rate.y = -0.0122173048- sat->gdc.rates.x; break; }
					cmd_rate.y = sat->gdc.rates.x; 
					break;
			}
		}
		if(cmd_rate.z == 0 && sat->gdc.yaw_bmag_failed != 0){ 
			switch(sat->AttRateSwitch.GetState()){
				case TOGGLESWITCH_UP:    // HIGH RATE
					// MAX RATE 7 dps
					if(sat->gdc.rates.y >  0.122173048){ cmd_rate.z = 0.122173048 - sat->gdc.rates.y; break; }
					if(sat->gdc.rates.y < -0.122173048){ cmd_rate.z = -0.122173048- sat->gdc.rates.y; break; }
					cmd_rate.z = sat->gdc.rates.y; 
					break;
				case TOGGLESWITCH_DOWN:  // LOW RATE
					// MAX RATE .7 dps
					if(sat->gdc.rates.y >  0.0122173048){ cmd_rate.z = 0.0122173048 - sat->gdc.rates.y; break; }
					if(sat->gdc.rates.y < -0.0122173048){ cmd_rate.z = -0.0122173048- sat->gdc.rates.y; break; }
					cmd_rate.z = sat->gdc.rates.y; 
					break;
			}
		}
		VECTOR3 pseudorate = _V(0,0,0);
		// PSEUDORATE FEEDBACK
		if(sat->LimitCycleSwitch.GetState() == TOGGLESWITCH_UP){
			// ROLL MINPULSE  = .000550 radians (4 jets, .0001375 per jet)
			// PITCH MINPULSE = .000200 radians (2 jets, .000100 per jet)
			// YAW MINPULSE   = .000150 radians (2 jets, .000075 per jet)
			if(sat->rjec.ThrusterDemand[1] != 0){ pseudorate.y += .000200; }
			if(sat->rjec.ThrusterDemand[2] != 0){ pseudorate.y -= .000200; }
			if(sat->rjec.ThrusterDemand[3] != 0){ pseudorate.y += .000200; }
			if(sat->rjec.ThrusterDemand[4] != 0){ pseudorate.y -= .000200; }

			if(sat->rjec.ThrusterDemand[5] != 0){ pseudorate.z -= .000150; }
			if(sat->rjec.ThrusterDemand[6] != 0){ pseudorate.z += .000150; }
			if(sat->rjec.ThrusterDemand[7] != 0){ pseudorate.z -= .000150; }
			if(sat->rjec.ThrusterDemand[8] != 0){ pseudorate.z += .000150; }

			if(sat->rjec.ThrusterDemand[9]  != 0){ pseudorate.x += .000275; }
			if(sat->rjec.ThrusterDemand[10] != 0){ pseudorate.x -= .000275; }
			if(sat->rjec.ThrusterDemand[11]  != 0){ pseudorate.x += .000275; }
			if(sat->rjec.ThrusterDemand[12] != 0){ pseudorate.x -= .000275; }
			if(sat->rjec.ThrusterDemand[13]  != 0){ pseudorate.x += .000275; }
			if(sat->rjec.ThrusterDemand[14] != 0){ pseudorate.x -= .000275; }
			if(sat->rjec.ThrusterDemand[15]  != 0){ pseudorate.x += .000275; }
			if(sat->rjec.ThrusterDemand[16] != 0){ pseudorate.x -= .000275; }
			// sprintf(oapiDebugString(),"SCS: PR: %f+%f %f+%f %f+%f",sat->gdc.rates.z,pseudorate.x,sat->gdc.rates.x,pseudorate.y,
			//	sat->gdc.rates.y,pseudorate.z);
//			sprintf(oapiDebugString(),"SCS: PSEUDORATE CALBRATION: %f %f %f",sat->gdc.rates.z,sat->gdc.rates.x,sat->gdc.rates.y);
		}

		// Command rates done, generate rate error values
		// GDC RATES are Z = ROLL, X = PITCH, Y = YAW
		rate_err.x = cmd_rate.x - (sat->gdc.rates.z + pseudorate.x);
		rate_err.y = cmd_rate.y - (sat->gdc.rates.x + pseudorate.y);
		rate_err.z = cmd_rate.z - (sat->gdc.rates.y + pseudorate.z);
		// sprintf(oapiDebugString(),"SCS: RATE CMD %f %f %f ERR %f %f %f",cmd_rate.x,cmd_rate.y,cmd_rate.z,rate_err.x,rate_err.y,rate_err.z);	
	}
	// ROTATION
	if(sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN){
		switch(sat->ManualAttRollSwitch.GetState()){
			case THREEPOSSWITCH_UP:      // ACCEL CMD
				// ECA auto-control is inhibited. Auto fire commands are generated from the breakout switches.
				if(rhc_x < 28673 && rhc_x > 2738){  // MINUS
					sat->rjec.SetThruster(10,1);
					sat->rjec.SetThruster(12,1);
					sat->rjec.SetThruster(14,1);
					sat->rjec.SetThruster(16,1);
					accel_roll_trigger=1; accel_roll_flag=-1;
				}
				if(rhc_x > 36863 && rhc_x < 62798){ // PLUS
					sat->rjec.SetThruster(9,1);
					sat->rjec.SetThruster(11,1);
					sat->rjec.SetThruster(13,1);
					sat->rjec.SetThruster(15,1);
					accel_roll_trigger=1; accel_roll_flag=1;
				}
				break;
			case THREEPOSSWITCH_CENTER:  // RATE CMD
				// Automatic mode and proportional-rate mode
				switch(sat->AttRateSwitch.GetState()){
					case TOGGLESWITCH_UP:    // HIGH RATE
						if(rate_err.x > 0.034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(9,1);
							sat->rjec.SetThruster(11,1);
							sat->rjec.SetThruster(13,1);
							sat->rjec.SetThruster(15,1);
							sat->rjec.SetThruster(10,0);
							sat->rjec.SetThruster(12,0);
							sat->rjec.SetThruster(14,0);
							sat->rjec.SetThruster(16,0);
							accel_roll_trigger=1; accel_roll_flag=1;
						}
						if(rate_err.x < -0.034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(10,1);
							sat->rjec.SetThruster(12,1);
							sat->rjec.SetThruster(14,1);
							sat->rjec.SetThruster(16,1);
							sat->rjec.SetThruster(9,0);
							sat->rjec.SetThruster(11,0);
							sat->rjec.SetThruster(13,0);
							sat->rjec.SetThruster(15,0);
							accel_roll_trigger=1; accel_roll_flag=-1;						
						}							
						break;
					case TOGGLESWITCH_DOWN:  // LOW RATE
						if(rate_err.x > 0.0034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(9,1);
							sat->rjec.SetThruster(11,1);
							sat->rjec.SetThruster(13,1);
							sat->rjec.SetThruster(15,1);
							sat->rjec.SetThruster(10,0);
							sat->rjec.SetThruster(12,0);
							sat->rjec.SetThruster(14,0);
							sat->rjec.SetThruster(16,0);
							accel_roll_trigger=1; accel_roll_flag=1;
						}
						if(rate_err.x < -0.0034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(10,1);
							sat->rjec.SetThruster(12,1);
							sat->rjec.SetThruster(14,1);
							sat->rjec.SetThruster(16,1);
							sat->rjec.SetThruster(9,0);
							sat->rjec.SetThruster(11,0);
							sat->rjec.SetThruster(13,0);
							sat->rjec.SetThruster(15,0);
							accel_roll_trigger=1; accel_roll_flag=-1;						
						}
						break;
				}
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				if(rhc_x < 28673 && rhc_x > 2738){  // MINUS
					if(!mnimp_roll_trigger){
						sat->rjec.SetThruster(10,1);
						sat->rjec.SetThruster(12,1);
						sat->rjec.SetThruster(14,1);
						sat->rjec.SetThruster(16,1);
					}
					mnimp_roll_trigger=1; mnimp_roll_flag=1;
				}
				if(rhc_x > 36863 && rhc_x < 62798){ // PLUS
					if(!mnimp_roll_trigger){
						sat->rjec.SetThruster(9,1);
						sat->rjec.SetThruster(11,1);
						sat->rjec.SetThruster(13,1);
						sat->rjec.SetThruster(15,1);
					}
					mnimp_roll_trigger=1; mnimp_roll_flag=1;
				}
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				break;
		}
		switch(sat->ManualAttPitchSwitch.GetState()){
			case THREEPOSSWITCH_UP:      // ACCEL CMD
				// ECA auto-control is inhibited. Auto fire commands are generated from the breakout switches.
				if(rhc_y < 28673 && rhc_y > 2738){  // MINUS
					sat->rjec.SetThruster(2,1);
					sat->rjec.SetThruster(4,1);
					accel_pitch_trigger=1; accel_pitch_flag=-1;
				}
				if(rhc_y > 36863 && rhc_y < 62798){ // PLUS
					sat->rjec.SetThruster(1,1);
					sat->rjec.SetThruster(3,1);
					accel_pitch_trigger=1; accel_pitch_flag=1;
				}
				break;
			case THREEPOSSWITCH_CENTER:  // RATE CMD
				// Automatic mode and proportional-rate mode
				switch(sat->AttRateSwitch.GetState()){
					case TOGGLESWITCH_UP:    // HIGH RATE
						if(rate_err.y > 0.034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(1,1);
							sat->rjec.SetThruster(3,1);
							sat->rjec.SetThruster(2,0);
							sat->rjec.SetThruster(4,0);
							accel_pitch_trigger=1; accel_pitch_flag=1;
						}
						if(rate_err.y < -0.034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(2,1);
							sat->rjec.SetThruster(4,1);
							sat->rjec.SetThruster(1,0);
							sat->rjec.SetThruster(3,0);
							accel_pitch_trigger=1; accel_pitch_flag=-1;
						}							
						break;
					case TOGGLESWITCH_DOWN:  // LOW RATE
						if(rate_err.y > 0.0034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(1,1);
							sat->rjec.SetThruster(3,1);
							sat->rjec.SetThruster(2,0);
							sat->rjec.SetThruster(4,0);
							accel_pitch_trigger=1; accel_pitch_flag=1;
						}
						if(rate_err.y < -0.0034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(2,1);
							sat->rjec.SetThruster(4,1);
							sat->rjec.SetThruster(1,0);
							sat->rjec.SetThruster(3,0);
							accel_pitch_trigger=1; accel_pitch_flag=-1;
						}							
						break;
				}
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				if(rhc_y < 28673 && rhc_y > 2738){  // MINUS
					if(!mnimp_pitch_trigger){
						sat->rjec.SetThruster(2,1);
						sat->rjec.SetThruster(4,1);
					}
					mnimp_pitch_trigger=1; mnimp_pitch_flag=1;
				}
				if(rhc_y > 36863 && rhc_y < 62798){ // PLUS
					if(!mnimp_pitch_trigger){
						sat->rjec.SetThruster(1,1);
						sat->rjec.SetThruster(3,1);
					}
					mnimp_pitch_trigger=1; mnimp_pitch_flag=1;
				}
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				break;
		}
		switch(sat->ManualAttYawSwitch.GetState()){
			case THREEPOSSWITCH_UP:      // ACCEL CMD
				// ECA auto-control is inhibited. Auto fire commands are generated from the breakout switches.
				if(rhc_z < 28673 && rhc_z > 2738){  // MINUS
					sat->rjec.SetThruster(6,1);
					sat->rjec.SetThruster(8,1);
					accel_yaw_trigger=1; accel_yaw_flag=-1;
				}
				if(rhc_z > 36863 && rhc_z < 62798){ // PLUS
					sat->rjec.SetThruster(5,1);
					sat->rjec.SetThruster(7,1);
					accel_yaw_trigger=1; accel_yaw_flag=1;
				}
				break;
			case THREEPOSSWITCH_CENTER:  // RATE CMD
				// Automatic mode and proportional-rate mode
				switch(sat->AttRateSwitch.GetState()){
					case TOGGLESWITCH_UP:    // HIGH RATE
						if(rate_err.z > 0.034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(6,1);
							sat->rjec.SetThruster(8,1);
							sat->rjec.SetThruster(5,0);
							sat->rjec.SetThruster(7,0);
							accel_yaw_trigger=1; accel_yaw_flag=-1;
						}
						if(rate_err.z < -0.034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(5,1);
							sat->rjec.SetThruster(7,1);
							sat->rjec.SetThruster(6,0);
							sat->rjec.SetThruster(8,0);
							accel_yaw_trigger=1; accel_yaw_flag=1;
						}							
						break;
					case TOGGLESWITCH_DOWN:  // LOW RATE
						if(rate_err.z > 0.0034906585){
							// ACCEL PLUS
							sat->rjec.SetThruster(6,1);
							sat->rjec.SetThruster(8,1);
							sat->rjec.SetThruster(5,0);
							sat->rjec.SetThruster(7,0);
							accel_yaw_trigger=1; accel_yaw_flag=-1;
						}
						if(rate_err.z < -0.0034906585){
							// ACCEL MINUS
							sat->rjec.SetThruster(5,1);
							sat->rjec.SetThruster(7,1);
							sat->rjec.SetThruster(6,0);
							sat->rjec.SetThruster(8,0);
							accel_yaw_trigger=1; accel_yaw_flag=1;
						}							
						break;
				}
				break;
			case THREEPOSSWITCH_DOWN:    // MIN IMP
				if(rhc_z < 28673 && rhc_z > 2738){  // MINUS
					if(!mnimp_yaw_trigger){
						sat->rjec.SetThruster(6,1);
						sat->rjec.SetThruster(8,1);
					}
					mnimp_yaw_trigger=1; mnimp_yaw_flag=1;
				}
				if(rhc_z > 36863 && rhc_z < 62798){ // PLUS
					if(!mnimp_yaw_trigger){
						sat->rjec.SetThruster(5,1);
						sat->rjec.SetThruster(7,1);
					}
					mnimp_yaw_trigger=1; mnimp_yaw_flag=1;
				}
				// ECA auto-control is inhibited. Auto fire one-shot commands are generated from the breakout switches.
				break;
		}
	}
	// If accel thrust fired and is no longer needed, kill it.
	if(accel_roll_flag == 0 && accel_roll_trigger){
		sat->rjec.SetThruster(9,0);
		sat->rjec.SetThruster(10,0);
		sat->rjec.SetThruster(11,0);
		sat->rjec.SetThruster(12,0);
		sat->rjec.SetThruster(13,0);
		sat->rjec.SetThruster(14,0);
		sat->rjec.SetThruster(15,0);
		sat->rjec.SetThruster(16,0);
		accel_roll_trigger=0;
	}
	if(accel_pitch_flag == 0 && accel_pitch_trigger){
		sat->rjec.SetThruster(1,0);
		sat->rjec.SetThruster(2,0);
		sat->rjec.SetThruster(3,0);
		sat->rjec.SetThruster(4,0);
		accel_pitch_trigger=0;
	}
	if(accel_yaw_flag == 0 && accel_yaw_trigger){
		sat->rjec.SetThruster(5,0);
		sat->rjec.SetThruster(6,0);
		sat->rjec.SetThruster(7,0);
		sat->rjec.SetThruster(8,0);
		accel_yaw_trigger=0;
	}
	// If the joystick has gone back to center after sending our min pulse, reset the one-shot
	if(mnimp_roll_trigger && !mnimp_roll_flag){
		mnimp_roll_trigger=0;
	}
	if(mnimp_pitch_trigger && !mnimp_pitch_flag){
		mnimp_pitch_trigger=0;
	}
	if(mnimp_yaw_trigger && !mnimp_yaw_flag){
		mnimp_yaw_trigger=0;
	}
	// TRANSLATION HANDLING
	int trans_x_flag=0,trans_y_flag=0,trans_z_flag=0;
	int sm_sep=0;
	ChannelValue30 val30;
	val30.Value = sat->agc.GetInputChannel(030); 
	if(sat->SCContSwitch.GetState() == TOGGLESWITCH_DOWN && !sm_sep){
		if(thc_x < 16384){ // PLUS X
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(14,1); }else{ sat->rjec.SetThruster(14,0); }
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(15,1); }else{ sat->rjec.SetThruster(15,0); }
			trans_x_trigger=1; trans_x_flag=1;
		}
		if(thc_x > 49152){ // MINUS X
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(16,1); }else{ sat->rjec.SetThruster(16,0); }
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(13,1); }else{ sat->rjec.SetThruster(13,0); }
			trans_x_trigger=1; trans_x_flag=1;
		}
		if(thc_y < 16384){ // MINUS Y (FORWARD)
			if(accel_pitch_flag > -1){ sat->rjec.SetThruster(1,1); }else{ sat->rjec.SetThruster(1,0); }
			if(accel_pitch_flag < 1 ){ sat->rjec.SetThruster(2,1); }else{ sat->rjec.SetThruster(2,0); }
			if(accel_yaw_flag   > -1){ sat->rjec.SetThruster(5,1); }else{ sat->rjec.SetThruster(5,0); }
			if(accel_yaw_flag   < 1 ){ sat->rjec.SetThruster(6,1); }else{ sat->rjec.SetThruster(6,0); }
			trans_y_trigger=1; trans_y_flag=1;
		}
		if(thc_y > 49152){ // PLUS Y (BACKWARD)
			if(accel_pitch_flag > -1){ sat->rjec.SetThruster(3,1); }else{ sat->rjec.SetThruster(3,0); }
			if(accel_pitch_flag < 1 ){ sat->rjec.SetThruster(4,1); }else{ sat->rjec.SetThruster(4,0); }
			if(accel_yaw_flag   > -1){ sat->rjec.SetThruster(7,1); }else{ sat->rjec.SetThruster(7,0); }
			if(accel_yaw_flag   < 1 ){ sat->rjec.SetThruster(8,1); }else{ sat->rjec.SetThruster(8,0); }
			trans_y_trigger=1; trans_y_flag=1;
		}
		if(thc_z < 16384){ // MINUS Z (UP)
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(11,1); }else{ sat->rjec.SetThruster(11,0); }
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(12,1); }else{ sat->rjec.SetThruster(12,0); }
			trans_z_trigger=1; trans_z_flag=1;
		}
		if(thc_z > 49152){ // PLUS Z (DOWN)
			if(accel_roll_flag > -1){ sat->rjec.SetThruster(9,1);  }else{ sat->rjec.SetThruster(9,0);  }
			if(accel_roll_flag < 1 ){ sat->rjec.SetThruster(10,1); }else{ sat->rjec.SetThruster(10,0); }
			trans_z_trigger=1; trans_z_flag=1;
		}
	}
	if(!trans_x_flag && trans_x_trigger){
		sat->rjec.SetThruster(13,0);
		sat->rjec.SetThruster(14,0);
		sat->rjec.SetThruster(15,0);
		sat->rjec.SetThruster(16,0);
		trans_x_trigger=0;
	}
	if(!trans_y_flag && trans_y_trigger){
		sat->rjec.SetThruster(3,0); 
		sat->rjec.SetThruster(7,0); 
		sat->rjec.SetThruster(4,0); 
		sat->rjec.SetThruster(8,0); 
		sat->rjec.SetThruster(2,0);
		sat->rjec.SetThruster(6,0);
		sat->rjec.SetThruster(1,0); 
		sat->rjec.SetThruster(5,0); 
		trans_y_trigger=0;
	}
	if(!trans_z_flag && trans_z_trigger){
		sat->rjec.SetThruster(9,0);
		sat->rjec.SetThruster(10,0);
		sat->rjec.SetThruster(11,0);
		sat->rjec.SetThruster(12,0);
		trans_z_trigger=0;
	}
}

// DS20060326 TELECOM OBJECTS
// PCM SYSTEM
PCM::PCM(){
	sat = NULL;
	conn_state = 0;
	wsk_error = 0;
	last_update = 0;
}

void PCM::Init(Saturn *vessel){
	sat = vessel;
	conn_state = 0;
	wsk_error = 0;
	last_update = 0;
	word_addr = 0;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR ){
		sprintf(wsk_emsg,"TELECOM: Error at WSAStartup()");
		wsk_error = 1;
		return;
	}
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET ) {
		sprintf(wsk_emsg,"TELECOM: Error at socket(): %ld", WSAGetLastError());
		WSACleanup();
		wsk_error = 1;
		return;
	}
	// Be nonblocking
	int iMode = 1; // 0 = BLOCKING, 1 = NONBLOCKING
	if(ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode) != 0){
		sprintf(wsk_emsg,"TELECOM: ioctlsocket() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Set up incoming options
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	service.sin_port = htons( 14242 );

	if ( bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {
		sprintf(wsk_emsg,"TELECOM: bind() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	if ( listen( m_socket, 1 ) == SOCKET_ERROR ){
		wsk_error = 1;
		sprintf(wsk_emsg,"TELECOM: listen() failed: %ld", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	conn_state = 1; // INITIALIZED, LISTENING
}

void PCM::TimeStep(double simt){
	// This stuff has to happen every timestep, regardless of system status.
	if(wsk_error != 0){
		sprintf(oapiDebugString(),"%s",wsk_emsg);
		// return;
	}
	// For now, don't care about voltages and such.

	// Allow IO to check for connections, etc
	if(conn_state != 2){
		last_update = simt; // Don't care about rate
		perform_io();
		return;
	}

	// Generate PCM datastream
	switch(sat->PCMBitRateSwitch.GetState()){
		case TOGGLESWITCH_DOWN: // LOW			
			tx_size = (int)((simt - last_update) / 0.005);
			// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
			if(tx_size > 0){
				last_update = simt;
				if(tx_size < 1024){
					tx_offset = 0;
					while(tx_offset < tx_size){
						generate_stream_lbr();
						tx_offset++;
					}
					perform_io();
				}
			}	
			break;

		case TOGGLESWITCH_UP:   // HIGH
			tx_size = (int)((simt - last_update) / 0.00015625);
			// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
			if(tx_size > 0){
				last_update = simt;
				if(tx_size < 1024){
					tx_offset = 0;
					while(tx_offset < tx_size){
						generate_stream_hbr();
						tx_offset++;
					}			
					perform_io();
				}
			}	
			break;
	}
}

void PCM::generate_stream_lbr(){
	unsigned char data=0;
	// 40 words per frame, 5 frames, 1 frame per second
	switch(word_addr){
		case 0: // SYNC 1
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();
			// And continue
			tx_data[tx_offset] = 05;
			break;
		case 1: // SYNC 2
			tx_data[tx_offset] = 0171;
			break;
		case 2: // SYNC 3
			tx_data[tx_offset] = 0267;
			break;
		case 3: // SYNC 4 & FRAME COUNT
			tx_data[tx_offset] = (0300|frame_count);
			break;
		case 4: 
			switch(frame_count){
				case 0: // 11A1 ECS: SUIT MANF ABS PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A109 EPS: BAT B CURR
					tx_data[tx_offset] = (unsigned char)(sat->EntryBatteryB->Current() / 0.390625); 
					break;
				case 2: // 11A46 RCS: SM HE MANF C PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A154 CMI: SCE NEG SUPPLY VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A91 EPS: BAT BUS A VOLTS					
					tx_data[tx_offset] = (unsigned char)(sat->BatteryBusA.Voltage() / 0.17578125);
					break;
			}
			break;
		case 5:
			switch(frame_count){
				case 0: // 11A2 ECS: SUIT COMP DELTA P
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A110 EPS: BAT C CURR
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A47 EPS: LM HEATER CURRENT
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A155 RCS: CM HE TK A TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A92 RCS: SM FU MANF A PRESS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 6:
			switch(frame_count){
				case 0: // 11A3 ECS: GLY PUMP OUT PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A111 ECS: SM FU MANF C PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A48 PCM HI LEVEL 85 PCT REF
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A156 CM HE TK B TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A93 BAT BUS B VOLTS
					tx_data[tx_offset] = (unsigned char)(sat->BatteryBusB.Voltage() / 0.17578125);
					break;
			}
			break;
		case 7:
			switch(frame_count){
				case 0: // 11A4 ECS SURGE TANK PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A112 SM FU MANF D PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A49 PC HI LEVEL 15 PCT REF
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A157 SEC GLY PUMP OUT PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A94 SM FU MANF B PRESS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 8: // 51DS1A COMPUTER DIGITAL DATA (40 BITS) 
		case 28:
			unsigned char data;
			ChannelValue13 ch13;
			ch13.Value = sat->agc.GetOutputChannel(013);			
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			if(ch13.Bits.DownlinkWordOrderCodeBit){ data |= 0200; } // WORD ORDER BIT
			/*
			sprintf(oapiDebugString(),"CMC DATA: %o (%lo %lo)",data,sat->agc.GetOutputChannel(034),
				sat->agc.GetOutputChannel(035));
			*/			
			tx_data[tx_offset] = data; 
			break;
		case 9: // 51DS1B COMPUTER DIGITAL DATA (40 BITS)
		case 29:
			data = (sat->agc.GetOutputChannel(034)&0277);
			tx_data[tx_offset] = data; 
			break;
		case 10: // 51DS1C COMPUTER DIGITAL DATA (40 BITS) 
		case 30:
			// PARITY OF CH 34 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(035)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 11: // 51DS1D COMPUTER DIGITAL DATA (40 BITS) 
		case 31:
			data = (sat->agc.GetOutputChannel(035)&0277);
			tx_data[tx_offset] = data; 
			break;
		case 12: // 51DS1E COMPUTER DIGITAL DATA (40 BITS) 
		case 32:
			// PARITY OF CH 35 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 13: // 51DP2 UP-DATA-LINK VALIDITY BITS (4 BITS)
		case 33:
			tx_data[tx_offset] = 0; 
			break;
		case 14:
			switch(frame_count){
				case 0: // 10A123
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A126
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A129
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A132
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A135
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 15:
			switch(frame_count){
				case 0: // 10A138
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A141 H2 TK 1 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A144 H2 TK 2 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A147 O2 TK 1 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A150 O2 TK 1 PRESS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 16:
			switch(frame_count){
				case 0: // 10A3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A6
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A9
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A12
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A15
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 17:
			switch(frame_count){
				case 0: // 10A18
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A21
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A24
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A27
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A30
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 18:
			switch(frame_count){
				case 0: // 10A33 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A36 H2 TK 1 PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A39 H2 TK 2 PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A42 O2 TK 2 QTY
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A45
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 19:
			switch(frame_count){
				case 0: // 10A48 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A51
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A54 O2 TK 1 TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A57 O2 TK 2 TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A60 H2 TK 1 TEMP
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 20:
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();
			// and continue
			switch(frame_count){
				case 0: // 10DP1 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP6
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP27
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP15
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP20
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 21:
			switch(frame_count){
				case 0: // SRC 0
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP7
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP28
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP16
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP21
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 22:
			switch(frame_count){
				case 0: // 11A39
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A147 AC BUS 1 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A84
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A21
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A129
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 23:
			switch(frame_count){
				case 0: // 11A40
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A48
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A85
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A22
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A130
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 24:
			switch(frame_count){
				case 0: // 11A73
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A10
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A118
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A55
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A163
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 25:
			switch(frame_count){
				case 0: // 11A74
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A11
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A119
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A56 AC BUS 2 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A164
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 26:
			switch(frame_count){
				case 0: // 11A75
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A12
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A120
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A57
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A165
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 27:
			switch(frame_count){
				case 0: // 11A76
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A13
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A121
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A58
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A166
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 34:
			switch(frame_count){
				case 0: // 11DP3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP8
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP13
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP29
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP22
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 35:
			switch(frame_count){
				case 0: // SRC 1
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP9
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP14
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP17
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP23
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 36:
			switch(frame_count){
				case 0: // 10A63 H2 TK 2 TEMP
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A66 O2 TK 2 PRESS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A69
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A72
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A75
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 37:
			switch(frame_count){
				case 0: // 10A78
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A81
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A84
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A87
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A90
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 38:
			switch(frame_count){
				case 0: // 10A93
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A96
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A99
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A102
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A105
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 39:
			switch(frame_count){
				case 0: // 10A108
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 10A111
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 10A114
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 10A117
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 10A120
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 39){
		word_addr = 0;
		frame_addr++;
		if(frame_addr > 4){
			frame_addr = 0;
		}
		frame_count++;
		if(frame_count > 5){
			frame_count = 0;
		}
	}
}

void PCM::generate_stream_hbr(){
	unsigned char data=0;
	// 128 words per frame, 50 frames pre second
	switch(word_addr){
		case 0: // SYNC 1
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();
			// Continue
			tx_data[tx_offset] = 05;
			break;
		case 1: // SYNC 2
			tx_data[tx_offset] = 0171;
			break;
		case 2: // SYNC 3
			tx_data[tx_offset] = 0267;
			break;
		case 3: // SYNC 4 & FRAME COUNT
			tx_data[tx_offset] = (0300|frame_count);
			break;
		case 4: // 22A1
		case 36:
		case 68:
		case 100:
			tx_data[tx_offset] = 0;
			break;
		case 5: // 22A2
		case 37:
		case 69:
		case 101:
			tx_data[tx_offset] = 0;
			break;
		case 6: // 22A3
		case 38:
		case 70:
		case 102:
			tx_data[tx_offset] = 0;
			break;
		case 7: // 22A4
		case 39:
		case 71:
		case 103:
			tx_data[tx_offset] = 0;
			break;
		case 8:
			switch(frame_count){
				case 0: // 11A1
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A37
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A73
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A109
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A145
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 9: 
			switch(frame_count){
				case 0: // 11A2
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A38
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A74
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A110
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A146
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 10:
			switch(frame_count){
				case 0: // 11A3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A39
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A75
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A111
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A147 AC BUS 1 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 11:
			switch(frame_count){
				case 0: // 11A4
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A40
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A76
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A112
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A148
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 12: // 12A1
		case 76:
			tx_data[tx_offset] = 0;
			break;
		case 13: // 12A2
		case 77:
			tx_data[tx_offset] = 0;
			break;
		case 14: // 12A3
		case 78:
			tx_data[tx_offset] = 0;
			break;
		case 15: // 12A4
		case 79:
			tx_data[tx_offset] = 0;
			break;
		case 16:
			switch(frame_count){
				case 0: // 11A5
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A41
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A77
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A113
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A149
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 17: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 18: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 19: // MAGICAL WORD SRC-0
			tx_data[tx_offset] = 0;
			break;
		case 20: // 12A5
		case 84:
			tx_data[tx_offset] = 0;
			break;
		case 21: // 12A6
		case 85:
			tx_data[tx_offset] = 0;
			break;
		case 22: // 12A7
		case 86:
			tx_data[tx_offset] = 0;
			break;
		case 23: // 12A8
		case 87:
			tx_data[tx_offset] = 0;
			break;
		case 24:
			switch(frame_count){
				case 0: // 11A6
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A42
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A78
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A114
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A150
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 25:
			switch(frame_count){
				case 0: // 11A7
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A43
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A79
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A115
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A151
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 26:
			switch(frame_count){
				case 0: // 11A8
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A44
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A80
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A116
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A152
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 27:
			switch(frame_count){
				case 0: // 11A9
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A45
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A81
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A117
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A153
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 28: // 51A1
			tx_data[tx_offset] = 0;
			break;
		case 29: // 51A2
			tx_data[tx_offset] = 0;
			break;
		case 30: // 51A3
			tx_data[tx_offset] = 0;
			break;
		case 31: // 51DS1A COMPUTER DIGITAL DATA (40 BITS)
			ChannelValue13 ch13;
			ch13.Value = sat->agc.GetOutputChannel(013);			
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			if(ch13.Bits.DownlinkWordOrderCodeBit){ data |= 0200; } // WORD ORDER BIT
			/*
			sprintf(oapiDebugString(),"CMC DATA: %o (%lo %lo)",data,sat->agc.GetOutputChannel(034),
				sat->agc.GetOutputChannel(035));
			*/			
			tx_data[tx_offset] = data; 
			break;
		case 32: // 51DS1B COMPUTER DIGITAL DATA (40 BITS)
			data = (sat->agc.GetOutputChannel(034)&0277);
			tx_data[tx_offset] = data; 
			break;
		case 33: // 51DS1C COMPUTER DIGITAL DATA (40 BITS)
			// PARITY OF CH 34 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(035)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 34: // 51DS1C COMPUTER DIGITAL DATA (40 BITS)
			data = (sat->agc.GetOutputChannel(035)&0277);
			tx_data[tx_offset] = data; 
			break;
		case 35: // 51DS1E COMPUTER DIGITAL DATA (40 BITS)
			// PARITY OF CH 35 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 40:
			switch(frame_count){
				case 0: // 11A10
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A46
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A82
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A118
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A154
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 41:
			switch(frame_count){
				case 0: // 11A11
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A47
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A83
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A119
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A155
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 42:
			switch(frame_count){
				case 0: // 11A12
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A48
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A84
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A120
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A156
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 43:
			switch(frame_count){
				case 0: // 11A13
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A49
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A85
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A121
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A157
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 44: // 12A9
		case 108:
			tx_data[tx_offset] = 0;
			break;
		case 45: // 12A10
		case 109:
			tx_data[tx_offset] = 0;
			break;
		case 46: // 12A11
		case 110:
			tx_data[tx_offset] = 0;
			break;
		case 47: // 12A12
		case 111:
			tx_data[tx_offset] = 0;
			break;
		case 48:
			switch(frame_count){
				case 0: // 11A14
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A50
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A86
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A122
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A158
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 49: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 50: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 51: // MAGICAL WORD 2
			tx_data[tx_offset] = 0;
			break;
		case 52: // 12A13
		case 116:
			tx_data[tx_offset] = 0;
			break;
		case 53: // 12A14
		case 117:
			tx_data[tx_offset] = 0;
			break;
		case 54: // 12A15
		case 118:
			tx_data[tx_offset] = 0;
			break;
		case 55: // 12A16
		case 119:
			tx_data[tx_offset] = 0;
			break;
		case 56:
			switch(frame_count){
				case 0: // 11A15
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A51
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A87
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A123
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A159
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 57:
			switch(frame_count){
				case 0: // 11A16
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A52
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A88
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A124
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A160
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 58:
			switch(frame_count){
				case 0: // 11A17
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A53
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A89
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A125
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A161
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 59:
			switch(frame_count){
				case 0: // 11A18
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A54
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A90
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A126
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A162
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 60: // 51A4
			tx_data[tx_offset] = 0;
			break;
		case 61: // 51A5
			tx_data[tx_offset] = 0;
			break;
		case 62: // 51A6
			tx_data[tx_offset] = 0;
			break;
		case 63: // 51A7
			tx_data[tx_offset] = 0;
			break;
		case 64:
			switch(frame_count){
				case 0: // 11DP2A
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP6
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP13
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP20
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP27
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 65:
			switch(frame_count){
				case 0: // 11DP2B
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP7
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP14
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP21
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP28
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 66:
			switch(frame_count){
				case 0: // 11DP2C
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP8
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP15
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP22
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP29
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 67:
			switch(frame_count){
				case 0: // 11DP2D
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP9
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP16
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP23
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP30
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 72: 
			switch(frame_count){
				case 0: // 11A19
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A55
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A91
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A127
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A163
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 73:
			switch(frame_count){
				case 0: // 11A20
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A56 AC BUS 2 PH A VOLTS
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A92
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A128
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A164
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 74:
			switch(frame_count){
				case 0: // 11A21
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A57
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A93
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A129
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A165
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 75:
			switch(frame_count){
				case 0: // 11A22
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A58
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A94
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A130
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A166
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 80:
			switch(frame_count){
				case 0: // 11A23
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A59
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A95
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A131
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A167
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 81: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 82: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 83: // MAGICAL WORD 3
			tx_data[tx_offset] = 0;
			break;
		case 88:
			switch(frame_count){
				case 0: // 11A24
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A60
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A96
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A132
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A168
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 89:
			switch(frame_count){
				case 0: // 11A25
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A61
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A97
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A133
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A169
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 90:
			switch(frame_count){
				case 0: // 11A26
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A62
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A98
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A134
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A170
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 91:
			switch(frame_count){
				case 0: // 11A27
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A63
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A99
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A135
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A171
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 92: // 51A8
			tx_data[tx_offset] = 0;
			break;
		case 93: // 51A9
			tx_data[tx_offset] = 0;
			break;
		case 94: // 51A10
			tx_data[tx_offset] = 0;
			break;
		case 95: // 51A11
			tx_data[tx_offset] = 0;
			break;
		case 96:
			switch(frame_count){
				case 0: // 11DP3
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP10
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP17
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP24
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP31
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 97:
			switch(frame_count){
				case 0: // 11DP4
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP11
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP18
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP25
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP32
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 98:
			switch(frame_count){
				case 0: // 11DP5
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11DP12
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11DP19
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11DP26
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11DP33
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 99: // 51DP2
			tx_data[tx_offset] = 0;
			break;
		case 104:
			switch(frame_count){
				case 0: // 11A28
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A64
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A100
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A136
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A172
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 105:
			switch(frame_count){
				case 0: // 11A29
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A65
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A101
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A137
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A173
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 106:
			switch(frame_count){
				case 0: // 11A30
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A66
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A102
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A138
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A174
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 107:
			switch(frame_count){
				case 0: // 11A31
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A67
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A103
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A139
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A175
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 112:
			switch(frame_count){
				case 0: // 11A32 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A68
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A104
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A140
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A176
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 113: // 22DP1
			tx_data[tx_offset] = 0;
			break;
		case 114: // 22DP2
			tx_data[tx_offset] = 0;
			break;
		case 115: // MAGICAL WORD 4
			tx_data[tx_offset] = 0;
			break;
		case 120:
			switch(frame_count){
				case 0: // 11A33 
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A69
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A105
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A141
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A177
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 121:
			switch(frame_count){
				case 0: // 11A34
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A70
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A106
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A142
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A178
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 122:
			switch(frame_count){
				case 0: // 11A35
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A71
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A107
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A143
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A179
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 123:
			switch(frame_count){
				case 0: // 11A36
					tx_data[tx_offset] = 0; 
					break;
				case 1: // 11A72
					tx_data[tx_offset] = 0; 
					break;
				case 2: // 11A108
					tx_data[tx_offset] = 0; 
					break;
				case 3: // 11A143
					tx_data[tx_offset] = 0; 
					break;
				case 4: // 11A180
					tx_data[tx_offset] = 0; 
					break;
			}
			break;
		case 124: // 51A12
			tx_data[tx_offset] = 0;
			break;
		case 125: // 51A13
			tx_data[tx_offset] = 0;
			break;
		case 126: // 51A14
			tx_data[tx_offset] = 0;
			break;
		case 127: // 51A15
			tx_data[tx_offset] = 0;
			break;

		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 127){
		word_addr = 0;
		frame_addr++;
		if(frame_addr > 4){
			frame_addr = 0;
		}
		frame_count++;
		if(frame_count > 50){
			frame_count = 0;
		}
	}
}

void PCM::perform_io(){
	// Do TCP IO
	switch(conn_state){
		case 0: // UNINITIALIZED
			break;
		case 1: // INITALIZED, LISTENING
			// Try to accept
			AcceptSocket = accept( m_socket, NULL, NULL );
			if(AcceptSocket != INVALID_SOCKET){
				conn_state = 2; // Accept this!
				wsk_error = 0; // For now
			}
			// Otherwise loop and try again.
			break;
		case 2: // CONNECTED			
			int bytesSent;

			bytesSent = send(AcceptSocket, (char *)tx_data, tx_size, 0 );
			if(bytesSent == SOCKET_ERROR){
				wsk_error = 1;
				sprintf(wsk_emsg,"TELECOM: send() failed: %ld",WSAGetLastError());
				closesocket(AcceptSocket);
				conn_state = 1; // Accept another
			}
			break;			
	}
}
