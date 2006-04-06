/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant, Matthias Mueller

  ORBITER vessel module: generic Saturn base class

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
  *	Revision 1.127  2006/03/30 01:59:37  movieman523
  *	Added RCS to SM DLL.
  *	
  *	Revision 1.126  2006/03/30 00:14:46  movieman523
  *	First pass at SM DLL.
  *	
  *	Revision 1.125  2006/03/29 19:06:49  movieman523
  *	First support for new SM.
  *	
  *	Revision 1.124  2006/03/29 16:34:00  jasonims
  *	GPFPI Meters added supporting proper LV Fuel Quantities and SPS Gimbel Position display.  LV Tank Quantities now accessable from outside Saturn class.
  *	
  *	Revision 1.123  2006/03/19 06:09:47  dseagrav
  *	GDC and ASCP save and load state.
  *	
  *	Revision 1.122  2006/03/17 02:17:30  dseagrav
  *	Routed SHIFT-NUMPAD keys to DSKY.
  *	
  *	Revision 1.121  2006/03/09 20:40:22  quetalsi
  *	Added Battery Relay Bus. Wired Inverter 1/2/3, EPS Sensor Unit DC A/B, EPS Sensor Unit AC 1/2 and Bat Rly Bus BAT A/B brakers.
  *	
  *	Revision 1.120  2006/03/03 05:12:37  dseagrav
  *	Added DirectInput code and THC/RHC interface. Changes 20060228-20060302
  *	
  *	Revision 1.119  2006/02/28 00:03:58  quetalsi
  *	MainBus A & B Switches and Talkbacks woks and wired.
  *	
  *	Revision 1.118  2006/02/23 15:49:24  tschachim
  *	Restored changes lost in last version.
  *	
  *	Revision 1.117  2006/02/23 14:13:49  dseagrav
  *	Split CM RCS into two systems, moved CM RCS thrusters (close to) proper positions, eliminated extraneous thrusters, set ISP and thrust values to match documentation, connected CM RCS to AGC IO channels 5 and 6 per DAP documentation, changes 20060221-20060223.
  *	
  *	Revision 1.116  2006/02/22 18:50:20  tschachim
  *	Bugfixes for Apollo 4-6.
  *	
  *	Revision 1.115  2006/02/22 01:03:02  movieman523
  *	Initial Apollo 5 support.
  *
  *	Revision 1.114  2006/02/21 12:22:39  tschachim
  *	Moved TLI sequence to the IU.
  *	
  *	Revision 1.113  2006/02/15 01:07:38  movieman523
  *	Revised TLI burn so hopefully it will work with the Virtual AGC.
  *	
  *	Revision 1.112  2006/02/09 14:37:42  tschachim
  *	Engine thrust build-up fixed.
  *	
  *	Revision 1.111  2006/02/08 12:08:31  tschachim
  *	Apply hold down force before lift-off.
  *	
  *	Revision 1.110  2006/02/07 14:06:25  tschachim
  *	Changed the Saturn parameters according to the Apollo 11 Saturn V flight evaluation report.
  *	
  *	Revision 1.109  2006/02/04 20:59:42  lazyd
  *	*** empty log message ***
  *	
  *	Revision 1.108  2006/02/02 18:54:15  tschachim
  *	Smoother acceleration calculations.
  *	
  *	Revision 1.107  2006/02/01 18:34:36  tschachim
  *	More REALISM 0 checklist actions.
  *	
  *	Revision 1.106  2006/01/31 00:51:39  lazyd
  *	Fixed vertical speed
  *	
  *	Revision 1.105  2006/01/27 22:11:38  movieman523
  *	Added support for low-res Saturn 1b.
  *	
  *	Revision 1.104  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.103  2006/01/26 03:31:57  movieman523
  *	Less hacky low-res mesh support for Saturn V.
  *	
  *	Revision 1.102  2006/01/24 13:56:03  tschachim
  *	Smoother staging with more eye-candy.
  *	Moved Timestep from clbkPostStep to clbkPreStep.
  *	
  *	Revision 1.101  2006/01/20 19:36:24  movieman523
  *	Fixed RCS disable bug with Quad D.
  *	
  *	Revision 1.100  2006/01/15 19:50:48  movieman523
  *	Don't delete launch site until we reach orbit.
  *	
  *	Revision 1.99  2006/01/15 02:38:59  movieman523
  *	Moved CoG and removed phantom thrusters. Also delete launch site when we get a reasonable distance away.
  *	
  *	Revision 1.98  2006/01/15 01:23:19  movieman523
  *	Put 'phantom' RCS thrusters back in and adjusted RCS thrust and ISP based on REALISM value.
  *	
  *	Revision 1.97  2006/01/14 20:58:16  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.96  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
  *	Revision 1.95  2006/01/14 00:54:35  movieman523
  *	Hacky wiring of sequential systems and pyro arm switches.
  *	
  *	Revision 1.94  2006/01/12 14:51:41  tschachim
  *	Added prelaunch tank venting.
  *	
  *	Revision 1.93  2006/01/11 22:34:20  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.92  2006/01/11 02:59:43  movieman523
  *	Valve talkbacks now check the valve state directlry. This means they barberpole on SM sep and can't then be changed.
  *	
  *	Revision 1.91  2006/01/11 02:16:25  movieman523
  *	Added RCS propellant quantity gauge.
  *	
  *	Revision 1.90  2006/01/10 23:45:35  movieman523
  *	Revised RCS ISP and thrust to historical values.
  *	
  *	Revision 1.89  2006/01/10 23:20:51  movieman523
  *	SM RCS is now enabled per quad.
  *	
  *	Revision 1.88  2006/01/10 20:49:50  movieman523
  *	Added CM RCS propellant dump and revised thrust display.
  *	
  *	Revision 1.87  2006/01/10 19:34:44  movieman523
  *	Fixed AC bus switches and added ELS Logic/Auto support.
  *	
  *	Revision 1.86  2006/01/09 21:56:44  movieman523
  *	Added support for LEM and CSM AGC PAD loads in scenario file.
  *	
  *	Revision 1.85  2006/01/08 19:04:30  movieman523
  *	Wired up AC bus switches in a quick and hacky manner.
  *	
  *	Revision 1.84  2006/01/08 16:15:20  movieman523
  *	For now, hard-wire batteries to buses when CM is seperated from SM.
  *	
  *	Revision 1.83  2006/01/08 14:51:24  movieman523
  *	Revised camera 3 position to be more photogenic, and added seperation particle effects.
  *	
  *	Revision 1.82  2006/01/08 04:37:50  movieman523
  *	Added camera 3.
  *	
  *	Revision 1.81  2006/01/08 04:00:24  movieman523
  *	Added first two engineering cameras.
  *	
  *	Revision 1.80  2006/01/07 03:28:28  movieman523
  *	Removed a lot of unused switches and wired up the FDAI power switch.
  *	
  *	Revision 1.79  2006/01/07 00:43:58  movieman523
  *	Added non-essential buses, though there's nothing connected to them at the moment.
  *	
  *	Revision 1.78  2006/01/06 22:55:53  movieman523
  *	Fixed SM seperation and cut off fuel cell power when it happens.
  *	
  *	Revision 1.77  2006/01/06 21:40:15  movieman523
  *	Quick hack for damping electrical meters.
  *	
  *	Revision 1.76  2006/01/05 11:59:59  tschachim
  *	New dockingprobe handling, bugfixes.
  *	
  *	Revision 1.75  2006/01/04 23:06:03  movieman523
  *	Moved meshes into ProjectApollo directory and renamed a few.
  *	
  *	Revision 1.74  2005/12/19 16:37:32  tschachim
  *	Realism modes, checklist actions.
  *	
  *	Revision 1.73  2005/12/13 10:12:20  tschachim
  *	Bugfix pointer initialization.
  *	
  *	Revision 1.72  2005/11/25 20:59:49  movieman523
  *	Added thrust decay for SIVb in TLI burn. Still needs tweaking.
  *	
  *	Revision 1.71  2005/11/25 02:03:47  movieman523
  *	Fixed mixture-ratio change code and made it more realistic.
  *	
  *	Revision 1.70  2005/11/25 00:02:16  movieman523
  *	Trying to make Apollo 11 work 'by the numbers'.
  *	
  *	Revision 1.69  2005/11/24 20:31:23  movieman523
  *	Added support for engine thrust decay during launch.
  *	
  *	Revision 1.68  2005/11/24 01:07:54  movieman523
  *	Removed code for panel lights which were being set incorrectly. Plus a bit of tidying.
  *	
  *	Revision 1.67  2005/11/23 01:43:13  movieman523
  *	Added SII stage DLL.
  *	
  *	Revision 1.66  2005/11/20 21:46:31  movieman523
  *	Added initial volume control support.
  *	
  *	Revision 1.65  2005/11/18 22:11:22  movieman523
  *	Added seperate heat and electrical power usage for boilers. Revised cabin fan code.
  *	
  *	Revision 1.64  2005/11/16 20:21:39  movieman523
  *	CSM/LEM renaming changes.
  *	
  *	Revision 1.63  2005/11/16 00:18:49  movieman523
  *	Added beginnings of really basic IU emulation. Added random failures of caution and warning lights on non-historical missions. Added initial support for Skylab CM and SM. Added LEM Name option in scenario file.
  *	
  *	Revision 1.62  2005/11/09 18:16:38  tschachim
  *	New Saturn assembly process.
  *	
  *	Revision 1.61  2005/10/19 11:40:40  tschachim
  *	FDAIs optionally disabled.
  *	
  *	Revision 1.60  2005/10/13 15:52:24  tschachim
  *	Fixed the panel change bug.
  *	
  *	Revision 1.59  2005/10/11 16:41:10  tschachim
  *	More REALISM 0 automatisms, added COAS, bugfixes.
  *	
  *	Revision 1.58  2005/09/30 11:24:37  tschachim
  *	Saving/Loading of lastSystemsMissionTime.
  *	
  *	Revision 1.57  2005/08/24 23:29:31  movieman523
  *	Fixed event timer reset.
  *	
  *	Revision 1.56  2005/08/24 00:30:00  movieman523
  *	Revised CM RCS code, and removed a load of switches that aren't used anymore.
  *	
  *	Revision 1.55  2005/08/23 22:18:47  movieman523
  *	SPS switch now works.
  *	
  *	Revision 1.54  2005/08/23 21:29:03  movieman523
  *	RCS state is now only checked when a stage event occurs or when a valve is opened or closed, not every timestep.
  *	
  *	Revision 1.53  2005/08/23 20:13:12  movieman523
  *	Added RCS talkbacks and changed AGC to use octal addresses for EMEM.
  *	
  *	Revision 1.52  2005/08/22 19:47:33  movieman523
  *	Fixed long timestep on startup, and added new Virtual AGC with EDRUPT fix.
  *	
  *	Revision 1.51  2005/08/21 22:21:00  movieman523
  *	Fixed SM RCS and activated SIVB RCS at all times for now.
  *	
  *	Revision 1.50  2005/08/21 17:21:10  movieman523
  *	Added event timer display.
  *	
  *	Revision 1.49  2005/08/21 16:23:32  movieman523
  *	Added more alarms.
  *	
  *	Revision 1.48  2005/08/21 13:13:43  movieman523
  *	Wired in a few caution and warning lights.
  *	
  *	Revision 1.47  2005/08/20 17:50:41  movieman523
  *	Added FDAI state save and load.
  *	
  *	Revision 1.46  2005/08/20 11:14:52  movieman523
  *	Added Rot Contr Pwr switches and removed a number of old switches which aren't used anymore.
  *	
  *	Revision 1.45  2005/08/19 21:33:20  movieman523
  *	Added initial random failure support.
  *	
  *	Revision 1.44  2005/08/19 20:05:44  movieman523
  *	Added abort switches. Wired in Tower Jett switches and SIVB Sep switch.
  *	
  *	Revision 1.43  2005/08/19 18:38:13  movieman523
  *	Wired up parachute switches properly, and added 'Comp Acty' to CSM AGC.
  *	
  *	Revision 1.42  2005/08/18 22:15:22  movieman523
  *	Wired up second DSKY, to accurately match the real hardware.
  *	
  *	Revision 1.41  2005/08/18 20:54:16  movieman523
  *	Added Main Release switch and wired it up to the parachutes.
  *	
  *	Revision 1.40  2005/08/18 19:12:21  movieman523
  *	Added Event Timer switches and null Event Timer class.
  *	
  *	Revision 1.39  2005/08/18 00:22:53  movieman523
  *	Wired in CM Uplink switch, removed some old code, added initial support for second DSKY.
  *	
  *	Revision 1.38  2005/08/17 00:01:59  movieman523
  *	Added ECS indicator switch, revised state saving, revised Timestep code to pass in the delta-time so we don't need to keep calculating it.
  *	
  *	Revision 1.37  2005/08/16 20:55:23  movieman523
  *	Added first saturn-specific switch for Xlunar Inject.
  *	
  *	Revision 1.36  2005/08/15 18:48:50  movieman523
  *	Moved the stage destroy code into a generic function for Saturn V and 1b.
  *	
  *	Revision 1.35  2005/08/15 02:37:57  movieman523
  *	SM RCS is now wired up.
  *	
  *	Revision 1.34  2005/08/14 15:25:43  movieman523
  *	Based on advice from ProjectApollo list, mission timer now starts running from zero at liftoff, and doesn't run on the pad.
  *	
  *	Revision 1.33  2005/08/13 20:20:17  movieman523
  *	Created MissionTimer class and wired it into the LEM and CSM.
  *	
  *	Revision 1.32  2005/08/13 14:59:24  movieman523
  *	Added initial null implementation of CSM caution and warning system, and removed 'master alarm' flag from Saturn class.
  *	
  *	Revision 1.31  2005/08/13 14:21:36  movieman523
  *	Added beginnings of caution and warning system.
  *	
  *	Revision 1.30  2005/08/12 23:15:49  movieman523
  *	Added switches to update mission time display.
  *	
  *	Revision 1.29  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.28  2005/08/09 02:28:27  movieman523
  *	Complete rewrite of the DSKY code to make it work with the real AGC I/O channels. That should now mean we can just hook up the Virtual AGC and have it work (with a few tweaks).
  *	
  *	Revision 1.27  2005/08/08 20:32:58  movieman523
  *	Added initial support for offsetting the mission timer and event timer from MissionTime: the real timers could be adjusted using the switches on the control panel (which aren't wired up yet), and the event timer would reset to zero on an abort.
  *	
  *	Revision 1.26  2005/08/06 01:25:27  movieman523
  *	Added Realism variable to AGC and fixed a bug with the APOLLONO scenario entry in the saturn class.
  *	
  *	Revision 1.25  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.24  2005/08/05 13:04:25  tschachim
  *	Fixed panel initialization
  *	
  *	Revision 1.23  2005/08/01 19:07:47  movieman523
  *	Genericised code to deal with SM destruction on re-entry, and did some tidying up of Saturn 1b code.
  *	
  *	Revision 1.22  2005/07/31 11:59:41  movieman523
  *	Added first mixture ratio shift to Saturn 1b.
  *	
  *	Revision 1.21  2005/07/31 01:43:13  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.20  2005/07/30 16:09:28  tschachim
  *	Added systemsState for the internal systems
  *	
  *	Revision 1.19  2005/07/30 02:05:48  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.18  2005/07/29 23:05:38  movieman523
  *	Added Inertial Guidance Mode start time to scenario file.
  *	
  *	Revision 1.17  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.16  2005/07/19 16:21:56  tschachim
  *	Docking radar sound only for CSM_LEM_STAGE
  *	
  *	Revision 1.15  2005/06/06 12:02:45  tschachim
  *	New switches, PanelSwitchScenarioHandler
  *	
  *	Revision 1.14  2005/05/31 02:12:08  movieman523
  *	Updated pre-entry burn variables and wrote most of the code to handle them.
  *	
  *	Revision 1.13  2005/05/31 00:17:33  movieman523
  *	Added CSMACCEL variables for unmanned flights which made burns just before re-entry to raise velocity to levels similar to a return from the moon.
  *	
  *	Revision 1.12  2005/05/18 23:34:23  movieman523
  *	Added roughly correct masses for the various Saturn payloads.
  *	
  *	Revision 1.11  2005/05/02 12:55:02  tschachim
  *	PanelsdkLogFile test code
  *	
  *	Revision 1.10  2005/04/22 13:58:19  tschachim
  *	Introduced PanelSDK
  *	Some changes because of the new panels
  *	
  *	Revision 1.9  2005/04/14 23:12:43  movieman523
  *	Added post-splashdown audio support. Unfortunately I can't test this at the moment as the control panel switches for getting out of the CM after splashdown aren't working :).
  *	
  *	However, it's pretty simple code, so 90+% likely to work.
  *	
  *	Revision 1.8  2005/03/19 03:39:13  chode99
  *	Fixed bug that prevented some switch states from being set by the scenario file.
  *	
  *	Revision 1.7  2005/03/14 01:40:30  chode99
  *	Fixed the positions of the SIVB RCS thrusters in Saturn V and Saturn IB
  *	
  *	Revision 1.6  2005/03/13 21:17:20  chode99
  *	Added code to compute accurate axial g force for panel display.
  *	
  *	Revision 1.5  2005/03/09 00:26:15  chode99
  *	Added code to support SII retros.
  *	
  *	Revision 1.4  2005/03/03 17:57:37  tschachim
  *	panel handling
  *	
  *	Revision 1.3  2005/02/19 00:03:57  movieman523
  *	Reduced volume of APS sound playback.
  *	
  *	Revision 1.2  2005/02/18 00:43:07  movieman523
  *	Added new Apollo 13 sound support.
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

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
#include "tracer.h"

//
// Random functions from Yaagc.
//

extern "C" {
	void srandom (unsigned int x);
	long int random ();
}

//extern FILE *PanelsdkLogFile;

Saturn::Saturn(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel), 
								agc(soundlib, dsky, dsky2, imu, Panelsdk, iu), 
								dsky(soundlib, agc, 015),
								dsky2(soundlib, agc, 016), 
								imu(agc, Panelsdk), 
								iu(soundlib, agc),
								cws(SMasterAlarm, Bclick, Panelsdk),
								dockingprobe(SDockingCapture, SDockingLatch, SDockingExtend, SUndock, CrashBumpS, Panelsdk),
								NonEssBus1("Non-Essential-Bus1", &InstrumentLightingNonESSCircuitBraker),
								NonEssBus2("Non-Essential-Bus2", &InstrumentLightingNonESSCircuitBraker),
								ACBus1PhaseA("AC-Bus1-PhaseA", 115, &ACBus1Source),
								ACBus1PhaseB("AC-Bus1-PhaseB", 115, &ACBus1Source),
								ACBus1PhaseC("AC-Bus1-PhaseC", 115, &ACBus1Source),
								ACBus2PhaseA("AC-Bus2-PhaseA", 115, &ACBus2Source),
								ACBus2PhaseB("AC-Bus2-PhaseB", 115, &ACBus2Source),
								ACBus2PhaseC("AC-Bus2-PhaseC", 115, &ACBus2Source),
								ACBus1Source(0, Panelsdk),
								ACBus2Source(0, Panelsdk),
								ACBus1("ACBus1", Panelsdk),
								ACBus2("ACBus2", Panelsdk),
								DCBusASource(0, Panelsdk),
								DCBusBSource(0, Panelsdk),
								BatteryBusA("Battery-Bus-A", Panelsdk),
								BatteryBusB("Battery-Bus-B", Panelsdk),
								PyroBusA("Pyro-Bus-A", Panelsdk),
								PyroBusB("Pyro-Bus-B", Panelsdk),
								BatteryRelayBus("Battery-Relay-Bus", Panelsdk),
								SECSLogicPower("SECS-Logic-Power", Panelsdk),
								PyroPower("Pyro-Power", Panelsdk),
								SwitchPower("Switch-Power", Panelsdk),
								SMQuadARCS(ph_rcs0),
								SMQuadBRCS(ph_rcs1),
								SMQuadCRCS(ph_rcs2),
								SMQuadDRCS(ph_rcs3),
								CMRCS1(ph_rcs_cm_1),CMRCS2(ph_rcs_cm_2),
								CMSMPyros("CM-SM-Pyros", Panelsdk)
{
	InitSaturnCalled = false;
	autopilot = false;
	LastTimestep = 0;

	//
	// VESSELSOUND initialisation
	// 
	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	cws.MonitorVessel(this);
	dockingprobe.RegisterVessel(this);
	iu.RegisterVessel(this);
}

Saturn::~Saturn()

{
	if (LMPad) {
		delete[] LMPad;
		LMPad = 0;
	}

	// DS20060302 release DirectX stuff
	if(js_enabled > 0){
		// Release joysticks
		while(js_enabled > 0){
			js_enabled--;
			dx8_joystick[js_enabled]->Unacquire();
			dx8_joystick[js_enabled]->Release();
		}
		dx8ppv->Release();
		dx8ppv = NULL;
	}

	//fclose(PanelsdkLogFile);
}

void Saturn::initSaturn()

{
	//
	// Default to the latest model Saturn
	//

	VehicleNo = 600;
	ApolloNo = 0;
	TCPO = 0.0;

	FirstTimestep = true;
	GenericFirstTimestep = true;

	TLICapableBooster = false;

	//
	// Do we have the Skylab-type SM and CM?
	//
	SkylabSM = false;
	SkylabCM = false;

	//
	// Do we have an HGA? This is a 'negative' variable for backward
	// compatibility with old scenarios... otherwise it would default
	// to having no HGA when the state was read from those files.
	//
	NoHGA = false;

	//
	// Or the S1b panel with 8 engine lights?
	//
	S1bPanel = false;

	bAbort = false;
	ABORT_IND = false;
	LEM_DISPLAY=false;
	bManualUnDock = false;
	bManualSeparate = false;
	ASTPMission = false;
	bToggleHatch = false;

	AutoSlow = false;
	Crewed = true;
	SIVBPayload = PAYLOAD_LEM;
	CSMSepSet = false;
	CSMSepTime = 0.0;

	CMSepSet = false;
	CMSepTime = 0.0;

	CSMAccelSet = false;
	CSMAccelTime = 0.0;
	CSMAccelEnd = 0.0;
	CSMAccelPitch = 0.0;

	SIVBBurnStart = 0.0;
	SIVBApogee = 0.0;

	CSMBurn = false;
	CSMBurnStart = 0.0;
	CSMApogee = 0.0;
	CSMPerigee = 0.0;

	HatchOpen = false;
	FIRSTCSM = false;
	ProbeJetison = false;
	LEMdatatransfer = false;
	PostSplashdownPlayed = false;

	DeleteLaunchSite = true;

	buildstatus = 6;

	ThrustAdjust = 1.0;
	MixtureRatio = 5.5;

	DockAngle = 0;

	AtempP  = 0;
	AtempY  = 0;
	AtempR  = 0;

	StopRot = false;
	SMSep = false;
	bStartS4B = false;
	IGMEnabled = false;

	LowRes = false;

	hStage1Mesh = 0;
	hStage2Mesh = 0;
	hStage3Mesh = 0;
	hInterstageMesh = 0;
	hStageSLA1Mesh = 0;
	hStageSLA2Mesh = 0;
	hStageSLA3Mesh = 0;
	hStageSLA4Mesh = 0;
	SPSidx = 0;

	refSaturn1B = 0;
	refPREV = 0;
	AltitudePREV = 0;

	hstg1 = 0;
	hstg2 = 0;
	hintstg = 0;
	hesc1 = 0;
	hPROBE = 0;
	hs4bM = 0;
	hs4b1 = 0;
	hs4b2 = 0;
	hs4b3 = 0;
	hs4b4 = 0;
	habort = 0;
	hSMJet = 0;
	hLMV = 0;
	hVAB = 0;
	hML = 0;
	hCrawler = 0;

	//
	// LM PAD data.
	//

	LMPadCount = 0;
	LMPad = 0;

	//
	// State for damping electrical meter display.
	//

	LastACVoltDisplay = 0.0;
	LastDCVoltDisplay = 0.0;
	LastDCAmpDisplay = 0.0;

	//
	// And thrust/AoA meter.
	//

	LastThrustDisplay = 0.0;

	//
	// Default mission time to an hour prior to launch.
	//

	MissionTime = (-3600);
	NextMissionEventTime = 0;
	LastMissionEventTime = 0;

	//
	// No point trying to destroy things if we haven't launched.
	//

	NextDestroyCheckTime = 0;
	NextFailureTime = MINUS_INFINITY;

	abortTimer = 0;
	release_time = 0;

	//
	// Defaults.
	//

	agc.SetDesiredApogee(220);
	agc.SetDesiredPerigee(215);
	agc.SetDesiredAzimuth(45);
	agc.SetVesselStats(SPS_ISP, SPS_THRUST, false);

	IGMStartTime = 204.1 ;

	//
	// Typical center engine shutdown times.
	//

	FirstStageCentreShutdownTime = 135.0;
	SecondStageCentreShutdownTime = 460.0;

	//
	// Stage shutdown times greater than stage burn times, to guarantee we burn all fuel.
	//

	FirstStageShutdownTime = 250.0;
	SecondStageShutdownTime = 1000.0;

	//
	// Same for interstage and LES jettison.
	//

	InterstageSepTime = 1000.0;
	LESJettisonTime = 1000.0;

	//
	// PU shift time. Default to 8:15
	//

	SecondStagePUShiftTime = 495.0;

	//
	// Failure modes.
	//

	LandFail.word = 0;
	LaunchFail.word = 0;
	SwitchFail.word = 0;

	//
	// Configure AGC and DSKY.
	//

	agc.ControlVessel(this);
	imu.SetVessel(this);
	dsky.Init();
	dsky2.Init();

	//
	// Propellant sources.
	//

	SMQuadARCS.SetVessel(this);
	SMQuadBRCS.SetVessel(this);
	SMQuadCRCS.SetVessel(this);
	SMQuadDRCS.SetVessel(this);
	CMRCS1.SetVessel(this);
	CMRCS2.SetVessel(this);

	//
	// Default masses.
	//

	CM_EmptyMass = 5440;
	CM_FuelMass = 123;

	SM_EmptyMass = 3110;
	SM_FuelMass = 18413 + 3000;

	S4PL_Mass = 14696;

	Abort_Mass = 4050;

	SI_MassLoaded = false;
	SII_MassLoaded = false;
	S4B_MassLoaded = false;

	S1_ThrustLoaded = false;
	S2_ThrustLoaded = false;
	S3_ThrustLoaded = false;

	//
	// Propellant handles.
	//

	ph_1st = 0;
	ph_2nd = 0;
	ph_3rd = 0;
	ph_rcs0 = 0;
	ph_rcs1 = 0;
	ph_rcs2 = 0;
	ph_rcs3 = 0;
	ph_rcs_cm_1 = 0;
	ph_rcs_cm_2 = 0;
	ph_sps = 0;
	ph_sep = 0;

	//
	// Thruster groups.
	//

	thg_main = 0;
	thg_ull = 0;
	thg_ver = 0;
	thg_retro1 = 0;
	thg_retro2 = 0;
	thg_aps = 0;

	//
	// Particle streams
	//
	int i;

	for (i = 0; i < 3; i++) {
		prelaunchvent[i] = NULL;
	}

	for (i = 0; i < 8; i++) {
		stagingvent[i] = NULL;
	}

	//
	// Random virtual cockpit motion.
	//

	ViewOffsetx = 0;
	ViewOffsety = 0;
	ViewOffsetz = 0;

	InVC = false;
	InPanel = false;
	CheckPanelIdInTimestep = false;

	//
	// Save the last view offset set.
	//

	CurrentViewOffset = 0;

	//
	// Zeroing arrays for safety.
	//

	for (i = 1; i < 6; i++){
		ClearEngineIndicator(i);
	}

	ClearLVGuidLight();
	ClearLVRateLight();

	for (i = 0; i < 8; i++) {
		LAUNCHIND[i] = false;
	}

	for (i = 0; i < nsurf; i++) {
		srf[i] = 0;
	}

	for (i = 0; i < 5; i++) {
		th_main[i] = 0;
	}

	for (i = 0; i < 8; i++) {
		th_ull[i] = 0;
		th_sep[i] = 0;
		stagingvent[i] = NULL;
	}

	for (i = 0; i < 3; i++) {
		th_ver[i] = 0;
		th_aps[i] = 0;
	}

	for (i = 0; i < 12; i++) {
		th_att_cm[i] = 0;
	}

	th_sps[0] = 0;

	//
	// SM RCS thrusters.
	//

	for (i = 0; i < 8; i++) {
		th_rcs_a[i] = 0;
		th_rcs_b[i] = 0;
		th_rcs_c[i] = 0;
		th_rcs_d[i] = 0;
	}

	for (i = 0; i < 24; i++) {
		th_att_rot[i] = 0;
		th_att_lin[i] = 0;
	}

	for (i = 0; i < 8; i++)
		ENGIND[i] = false;

	LEMName[0] = 0;

	UseATC = false;
	Realism = REALISM_DEFAULT;

	SIISepState = false;
	bRecovery = false;
	ActivateLEM = false;
	ActivateS4B = false;
	RCS_Full = true;
	ReadyAstp = false;
	Abort_Locked = false;

	stage = 0;
	dockstate = 0;

	KEY1=false;
	KEY2=false;
	KEY3=false;
	KEY4=false;
	KEY5=false;
	KEY6=false;
	KEY7=false;
	KEY8=false;
	KEY9=false;

	actualVEL = 0;
	actualALT = 0;
	actualFUEL = 0;
	aVAcc = 0;
	aVSpeed = 0;
	aHAcc = 0;
	aZAcc = 0;

	for (i = 0; i < LASTVELOCITYCOUNT; i++) {
		LastVelocity[i] = _V(0, 0, 0);
		LastVerticalVelocity[i] = 0;
		LastSimt[i] = 0;
	}
	LastVelocityFilled = -1;

	viewpos = SATVIEW_CDR;

	probeidx = 0;
	ToggleEva = false;
	ActivateASTP = false;

	//
	// LM landing data.
	//

	LMLandingLatitude = 0;
	LMLandingLongitude = 0;
	LMLandingAltitude = 0;

	//
	// Earth landing data.
	//

	EarthLandingLatitude = 0;
	EarthLandingLongitude = 0;

	//
	// Load textures that are used a lot. Should these be global
	// variables?
	//

	SMExhaustTex = oapiRegisterExhaustTexture ("Exhaust_atrcs");
	SMMETex = oapiRegisterExhaustTexture ("Exhaust_atsme");//"Exhaust2"
	CMTex =oapiRegisterReentryTexture("reentry");

	strncpy(AudioLanguage, "English", 64);

	hEVA = 0;

	//
	// Timestep tracking for debugging.
	//

	LongestTimestep = 0;
	LongestTimestepLength = 0.0;
	CurrentTimestep = 0;

	// call only once 
	if (!InitSaturnCalled) {

		// PanelSDK pointers.
		ClearPanelSDKPointers();

		// Initialize the internal systems
		SystemsInit();

		// Initialize the panel
		fdaiDisabled = false;
		fdaiSmooth = false;
		PanelId = SATPANEL_MAIN; 		// default panel
		InitSwitches();

		// "dummy" SetSwitches to enable the panel event handling
		SetSwitches(PanelId);
	}
	InitSaturnCalled = true;
}

void Saturn::GetLEMName(char *s)

{
	if (LEMName[0]) {
		strcpy (s, LEMName);
		return;
	}

	strcpy (s, GetName()); strcat (s, "-LM");
}

void Saturn::GetApolloName(char *s)

{
	sprintf(s, "AS-%d", VehicleNo);
}

void Saturn::UpdateLaunchTime(double t)

{
	if (t < 0)
		return;

	if (MissionTime < 0) {
		MissionTime = (-t);
	}
}

//
// Pitch program.
//

double Saturn::GetCPitch(double t)
{
	int i = 1;

	//
	// Make sure we don't run off the end.
	//

	if (t>met[PITCH_TABLE_SIZE - 1]) return cpitch[PITCH_TABLE_SIZE - 1];

	//
	// Find the first MET that's greater than our current time.
	//

	while (met[i]<t) i++;

	//
	// And calculate pitch as appropriate between those two times.
	//

	return cpitch[i-1]+(cpitch[i]-cpitch[i-1])/(met[i]-met[i-1])*(t-met[i-1]);
}

double Saturn::SetPitchApo()

{
	double cpitch;
	double SatApo1;
	double DiffApo;

	OBJHANDLE hbody = GetGravityRef();
	double bradius = oapiGetSize(hbody);

	GetApDist(SatApo1);
	DiffApo = SatApo1 - (agc.GetDesiredApogee() * 1000. + bradius);
	cpitch = (GetPitch() * DEG) - aVAcc - (aVSpeed / 50.) + ((agc.GetDesiredApogee() * 1000. - GetAltitude()) / 8000.);


	if (cpitch > 35){
		cpitch=35;
	}
	else if(cpitch < -35){
		cpitch = -35;
	}

	//sprintf(oapiDebugString(), "diff %f", cpitch );
	return cpitch;
}

//
// Kill a vessel (typically a stage) based on its distance from us.
//

void Saturn::KillDist(OBJHANDLE &hvessel, double kill_dist)

{
	VECTOR3 posr  = {0,0,0};
	double dist = 0.0;

	oapiGetRelativePos (GetHandle() ,hvessel, &posr);
	dist = (posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);

	if (dist > (kill_dist * kill_dist)){
		oapiDeleteVessel(hvessel, GetHandle());
		hvessel = 0;
	}
}

//
// Kill a vessel (typically a stage) based on its current altitude.
//

void Saturn::KillAlt(OBJHANDLE &hvessel, double altVS)

{
	double CurrentAlt;
	int getit = oapiGetAltitude(hvessel, &CurrentAlt);

	if ((CurrentAlt < altVS) && CurrentAlt >= 0 ){
		oapiDeleteVessel(hvessel, GetHandle());
		hvessel = 0;
	}
}


void Saturn::clbkDockEvent(int dock, OBJHANDLE connected)

{
	dockingprobe.DockEvent(dock, connected); 
}

void Saturn::clbkPreStep(double simt, double simdt, double mjd)

{
	TRACESETUP("Saturn::clbkPreStep");
	//
	// You'll also die horribly if you set time acceleration at all in the
	// early parts of the launch.
	//
	if (stage == LAUNCH_STAGE_ONE && MissionTime < 50 &&
		oapiGetTimeAcceleration() > 1.0) {
		oapiSetTimeAcceleration(1.0);
	}

	//
	// Change to desired panel if necessary
	//
	if (CheckPanelIdInTimestep) {
		oapiSetPanel(PanelId);
		CheckPanelIdInTimestep = false;
	}

	Timestep(simt, simdt);
}

void Saturn::clbkPostStep (double simt, double simdt, double mjd)

{
	TRACESETUP("Saturn::clbkPostStep");

}

void Saturn::clbkSaveState(FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	int i = 1;
	char str[256];

	oapiWriteScenario_int (scn, "NASSPVER", NASSP_VERSION);
	oapiWriteScenario_int (scn, "STAGE", stage);
	oapiWriteScenario_int(scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "APOLLONO", ApolloNo);
	oapiWriteScenario_int (scn, "DOCKSTATE", dockstate);
	oapiWriteScenario_int (scn, "PANEL_ID", PanelId);
	oapiWriteScenario_float (scn, "TOAPO", agc.GetDesiredApogee());
	oapiWriteScenario_float (scn, "TOPER", agc.GetDesiredPerigee());
	oapiWriteScenario_float (scn, "TOHDG", agc.GetDesiredAzimuth());
	oapiWriteScenario_float (scn, "TCP", TCPO);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "NFAILTIME", NextFailureTime);
	oapiWriteScenario_float (scn, "MTD", MissionTimerDisplay.GetTime());
	oapiWriteScenario_float (scn, "ETD", EventTimerDisplay.GetTime());
	oapiWriteScenario_float (scn, "THRUSTA", ThrustAdjust);
	oapiWriteScenario_float (scn, "MR", MixtureRatio);

	oapiWriteScenario_int (scn, "DLS", DeleteLaunchSite ? 1 : 0);
	oapiWriteScenario_int (scn, "LOWRES", LowRes ? 1 : 0);

	if (Realism != REALISM_DEFAULT) {
		oapiWriteScenario_int (scn, "REALISM", Realism);
	}

	if (buildstatus < 6) {
		oapiWriteScenario_int (scn, "BUILDSTATUS", buildstatus);
	}

	oapiWriteScenario_int(scn, "STAGESTATUS", StageState);

	if (LEM_DISPLAY) {
		oapiWriteScenario_int (scn, "LEM_DISPLAY", int(LEM_DISPLAY));
	}

	oapiWriteScenario_int (scn, "MAINSTATE",  GetMainState());
	oapiWriteScenario_int (scn, "LIGHTSTATE",  GetLightState());
	oapiWriteScenario_int (scn, "CSWITCH",  GetCSwitchState());
	oapiWriteScenario_int (scn, "SSWITCH",  GetSSwitchState());
	oapiWriteScenario_int (scn, "LPSWITCH",  GetLPSwitchState());
	oapiWriteScenario_int (scn, "RPSWITCH",  GetRPSwitchState());

	//
	// Save vessel-specific stats.
	//

	SaveVehicleStats(scn);

	if (stage < LAUNCH_STAGE_ONE)
		oapiWriteScenario_int (scn, "PRELAUNCHATC",  int(UseATC));

	if (stage < LAUNCH_STAGE_TWO) {
		oapiWriteScenario_float (scn, "SICSHUT", FirstStageCentreShutdownTime);
		oapiWriteScenario_float (scn, "SISHUT", FirstStageShutdownTime);
		oapiWriteScenario_float (scn, "T1V", THRUST_FIRST_VAC);
		oapiWriteScenario_float (scn, "I1S", ISP_FIRST_SL);
		oapiWriteScenario_float (scn, "I1V", ISP_FIRST_VAC);
	}

	if (stage < STAGE_ORBIT_SIVB) {
		char fname[64];

		oapiWriteScenario_float (scn, "SIICSHUT", SecondStageCentreShutdownTime);
		oapiWriteScenario_float (scn, "SIIPUT", SecondStagePUShiftTime);
		oapiWriteScenario_float (scn, "SIISHUT", SecondStageShutdownTime);
		oapiWriteScenario_float (scn, "T2V", THRUST_SECOND_VAC);
		oapiWriteScenario_float (scn, "I2S", ISP_SECOND_SL);
		oapiWriteScenario_float (scn, "I2V", ISP_SECOND_VAC);
		oapiWriteScenario_float (scn, "T3V", THRUST_THIRD_VAC);
		oapiWriteScenario_float (scn, "I3V", ISP_THIRD_VAC);
		oapiWriteScenario_float (scn, "ISTGJT", InterstageSepTime);
		oapiWriteScenario_float (scn, "LESJT", LESJettisonTime);

		//
		// Save pitch program.
		//

		for (i = 0; i < PITCH_TABLE_SIZE; i++) {
			sprintf(fname, "PMET%03d", i);
			oapiWriteScenario_float (scn, fname, met[i]);
		}

		for (i = 0; i < PITCH_TABLE_SIZE; i++) {
			sprintf(fname, "CPITCH%03d", i);
			oapiWriteScenario_float (scn, fname, cpitch[i]);
		}

		//
		// IGM start time.
		//

		oapiWriteScenario_float (scn, "IGMST", IGMStartTime);
	}

	if (stage < CSM_LEM_STAGE) {
		oapiWriteScenario_int (scn, "LAUNCHSTATE", GetLaunchState());
	}

	if (stage < CM_STAGE) {
		oapiWriteScenario_float (scn, "SMFUELLOAD", SM_FuelMass);
		oapiWriteScenario_float (scn, "SMMASS", SM_EmptyMass);
	}

	oapiWriteScenario_float (scn, "CMFUELLOAD", CM_FuelMass);
	oapiWriteScenario_float (scn, "CMMASS", CM_EmptyMass);

	if (!LEMdatatransfer && iu.IsTLICapable()) {
		oapiWriteScenario_float (scn, "MOONLAT", LMLandingLatitude);
		oapiWriteScenario_float (scn, "MOONLONG", LMLandingLongitude);
		oapiWriteScenario_float (scn, "MOONALT", LMLandingAltitude);

		if (LMPadCount > 0) {
			oapiWriteScenario_int (scn, "LMPADCNT", LMPadCount);
			for (i = 0; i < LMPadCount; i++) {
				sprintf(str, "%04o %05o", LMPad[i * 2], LMPad[i * 2 + 1]);
				oapiWriteScenario_string (scn, "LMPAD", str);
			}
		}
	}

	if (!Crewed) {
		oapiWriteScenario_int (scn, "UNMANNED", 1);

		if (CSMSepSet && (CSMSepTime >= MissionTime))
		{
			oapiWriteScenario_float (scn, "CSMSEP", CSMSepTime);
		}

		if (CMSepSet && (CMSepTime >= MissionTime))
		{
			oapiWriteScenario_float (scn, "CMSEP", CMSepTime);
		}

		if (stage <= CSM_LEM_STAGE) {
			oapiWriteScenario_float (scn, "S4APO", SIVBApogee);
			oapiWriteScenario_float (scn, "S4BURN", SIVBBurnStart);
		}

		if (CSMBurn && (stage <= CM_STAGE)) {
			oapiWriteScenario_float (scn, "CSMAPO", CSMApogee);
			oapiWriteScenario_float (scn, "CSMPER", CSMPerigee);
			oapiWriteScenario_float (scn, "CSMBURN", CSMBurnStart);
		}

		if (CSMAccelSet && (stage <= CM_STAGE))
		{
			oapiWriteScenario_float (scn, "CSMACCEL", CSMAccelTime);
			oapiWriteScenario_float (scn, "CSMACCEND", CSMAccelEnd);
			oapiWriteScenario_float (scn, "CSMACCPITCH", CSMAccelPitch);
		}

		if (AutoSlow) {
			oapiWriteScenario_int (scn, "AUTOSLOW", 1);
		}
	}

	int valvestate = 0;
	int mask = 1;

	for (i = 1; i < N_CSM_VALVES; i++) {
		if (ValveState[i])
			valvestate |= mask;
		mask <<= 1;
	}

	oapiWriteScenario_int (scn, "VALVESTATE", valvestate);

	if (LandFail.word) {
		oapiWriteScenario_int (scn, "LANDFAIL", LandFail.word);
	}

	if (LaunchFail.word) {
		oapiWriteScenario_int (scn, "LAUNCHFAIL", LaunchFail.word);
	}

	if (SwitchFail.word) {
		oapiWriteScenario_int (scn, "SWITCHFAIL", SwitchFail.word);
	}

	if (ApolloNo == 13) {
		oapiWriteScenario_int (scn, "A13STATE", GetA13State());
	}

	if (SIVBPayload != PAYLOAD_LEM) {
		oapiWriteScenario_int (scn, "S4PL", SIVBPayload);
	}

	oapiWriteScenario_string (scn, "LANG", AudioLanguage);

	if (LEMName[0])
		oapiWriteScenario_string (scn, "LEMN", LEMName);

	oapiWriteScenario_int (scn, "COASENABLED", coasEnabled);
	oapiWriteScenario_int (scn, "FDAIDISABLED", fdaiDisabled);
	oapiWriteScenario_int (scn, "FDAISMOOTH", fdaiSmooth);

	dsky.SaveState(scn, DSKY_START_STRING, DSKY_END_STRING);
	dsky2.SaveState(scn, DSKY2_START_STRING, DSKY2_END_STRING);
	agc.SaveState(scn);
	imu.SaveState(scn);
	cws.SaveState(scn);
	iu.SaveState(scn);
	// DS20060318 SCS OBJECTS
	gdc.SaveState(scn);
	ascp.SaveState(scn);
	dockingprobe.SaveState(scn);
	fdaiLeft.SaveState(scn, FDAI_START_STRING, FDAI_END_STRING);
	fdaiRight.SaveState(scn, FDAI2_START_STRING, FDAI2_END_STRING);

	// save the internal systems 
	oapiWriteScenario_int(scn, "SYSTEMSSTATE", systemsState);
	oapiWriteScenario_float(scn, "LSYSTEMSMISSNTIME", lastSystemsMissionTime);
	Panelsdk.Save(scn);	

	// save the state of the switches
	PSH.SaveState(scn);	
}

//
// Scenario state functions.
//

typedef union {
	struct {
		unsigned MissionTimerRunning:1;
		unsigned SIISepState:1;
		unsigned TLIBurnDone:1;
		unsigned Scorrec:1;
		unsigned Burned:1;
		unsigned EVA_IP:1;
		unsigned ABORT_IND:1;
		unsigned HatchOpen:1;
		unsigned unused_1:1;
		unsigned unused_2:1;
		unsigned LEMdatatransfer:1;
		unsigned PostSplashdownPlayed:1;
		unsigned IGMEnabled:1;
		unsigned UNUSED:1;
		unsigned MissionTimerEnabled:1;
		unsigned EventTimerEnabled:1;
		unsigned EventTimerRunning:1;
		unsigned EventTimerCountUp:2;
		unsigned SkylabSM:1;
		unsigned SkylabCM:1;
		unsigned S1bPanel:1;
		unsigned NoHGA:1;
		unsigned viewpos:3;
	} u;
	unsigned long word;
} MainState;


int Saturn::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.MissionTimerRunning = MissionTimerDisplay.IsRunning();
	state.u.MissionTimerEnabled = MissionTimerDisplay.IsEnabled();
	state.u.EventTimerRunning = EventTimerDisplay.IsRunning();
	state.u.EventTimerEnabled = EventTimerDisplay.IsEnabled();
	state.u.EventTimerCountUp = EventTimerDisplay.GetCountUp();
	state.u.SIISepState = SIISepState;
	state.u.Scorrec = Scorrec;
	state.u.Burned = Burned;
	state.u.EVA_IP = EVA_IP;
	state.u.ABORT_IND = ABORT_IND;
	state.u.HatchOpen = HatchOpen;
	state.u.viewpos = viewpos;
	state.u.LEMdatatransfer = LEMdatatransfer;
	state.u.PostSplashdownPlayed = PostSplashdownPlayed;
	state.u.IGMEnabled = IGMEnabled;
	state.u.SkylabSM = SkylabSM;
	state.u.SkylabCM = SkylabCM;
	state.u.S1bPanel = S1bPanel;
	state.u.NoHGA = NoHGA;

	return state.word;
}

void Saturn::SetMainState(int s)

{
	MainState state;

	state.word = s;
	SIISepState = state.u.SIISepState;
	Scorrec = state.u.Scorrec;
	Burned = state.u.Burned;
	EVA_IP = state.u.EVA_IP;
	ABORT_IND = state.u.ABORT_IND;
	HatchOpen = state.u.HatchOpen;
	viewpos = state.u.viewpos;
	LEMdatatransfer = state.u.LEMdatatransfer;
	PostSplashdownPlayed = (state.u.PostSplashdownPlayed != 0);
	IGMEnabled = (state.u.IGMEnabled != 0);
	MissionTimerDisplay.SetRunning(state.u.MissionTimerRunning != 0);
	MissionTimerDisplay.SetEnabled(state.u.MissionTimerEnabled != 0);
	EventTimerDisplay.SetRunning(state.u.EventTimerRunning != 0);
	EventTimerDisplay.SetEnabled(state.u.EventTimerEnabled != 0);
	EventTimerDisplay.SetCountUp(state.u.EventTimerCountUp);
	SkylabSM = (state.u.SkylabSM != 0);
	SkylabCM = (state.u.SkylabCM != 0);
	S1bPanel = (state.u.S1bPanel != 0);
	NoHGA = (state.u.NoHGA != 0);
}

//
// State which is only required for Apollo 13
//

typedef union {
	struct {
		unsigned ApolloExploded:1;
		unsigned CryoStir:1;
		unsigned KranzPlayed:1;
	} u;
	unsigned long word;
} A13State;


int Saturn::GetA13State()

{
	A13State state;

	state.word = 0;
	state.u.ApolloExploded = ApolloExploded;
	state.u.CryoStir = CryoStir;
	state.u.KranzPlayed = KranzPlayed;

	return state.word;
}

void Saturn::SetA13State(int s)

{
	A13State state;

	state.word = s;
	ApolloExploded = (state.u.ApolloExploded != 0);
	CryoStir = (state.u.CryoStir != 0);
	KranzPlayed = (state.u.KranzPlayed != 0);
}

//
// State which is only required through the launch process.
//

typedef union {
	struct {
		unsigned autopilot:1;
		unsigned TLIEnabled:1;
	} u;
	unsigned long word;
} LaunchState;


int Saturn::GetLaunchState()

{
	LaunchState state;

	state.word = 0;
	state.u.autopilot = autopilot;

	return state.word;
}

void Saturn::SetLaunchState(int s)

{
	LaunchState state;

	state.word = s;
	autopilot = (state.u.autopilot != 0);
}

typedef union {
	struct {
		unsigned Engind0:1;
		unsigned Engind1:1;
		unsigned Engind2:1;
		unsigned Engind3:1;
		unsigned Engind4:1;
		unsigned Engind5:1;
		unsigned LVGuidLight:1;
		unsigned Launchind0:1;
		unsigned Launchind1:1;
		unsigned Launchind2:1;
		unsigned Launchind3:1;
		unsigned Launchind4:1;
		unsigned Launchind5:1;
		unsigned Launchind6:1;
		unsigned Launchind7:1;
		unsigned Engind6:1;
		unsigned Engind7:1;
		unsigned Engind8:1;
		unsigned LVRateLight:1;
	} u;
	unsigned long word;
} LightState;

int Saturn::GetLightState()

{
	LightState state;

	state.word = 0;
	state.u.Engind0 = ENGIND[0];
	state.u.Engind1 = ENGIND[1];
	state.u.Engind2 = ENGIND[2];
	state.u.Engind3 = ENGIND[3];
	state.u.Engind4 = ENGIND[4];
	state.u.Engind5 = ENGIND[5];
	state.u.Engind6 = ENGIND[6];
	state.u.Engind7 = ENGIND[7];
	state.u.Engind8 = ENGIND[8];
	state.u.LVGuidLight = LVGuidLight;
	state.u.Launchind0 = LAUNCHIND[0];
	state.u.Launchind1 = LAUNCHIND[1];
	state.u.Launchind2 = LAUNCHIND[2];
	state.u.Launchind3 = LAUNCHIND[3];
	state.u.Launchind4 = LAUNCHIND[4];
	state.u.Launchind5 = LAUNCHIND[5];
	state.u.Launchind6 = LAUNCHIND[6];
	state.u.Launchind7 = LAUNCHIND[7];
	state.u.LVRateLight = LVRateLight;

	return state.word;
}

void Saturn::SetLightState(int s)

{
	LightState state;

	state.word = s;
	ENGIND[0] = (state.u.Engind0 != 0);
	ENGIND[1] = (state.u.Engind1 != 0);
	ENGIND[2] = (state.u.Engind2 != 0);
	ENGIND[3] = (state.u.Engind3 != 0);
	ENGIND[4] = (state.u.Engind4 != 0);
	ENGIND[5] = (state.u.Engind5 != 0);
	ENGIND[6] = (state.u.Engind6 != 0);
	ENGIND[7] = (state.u.Engind7 != 0);
	ENGIND[8] = (state.u.Engind8 != 0);
	LVGuidLight = (state.u.LVGuidLight != 0);
	LAUNCHIND[0] = (state.u.Launchind0 != 0);
	LAUNCHIND[1] = (state.u.Launchind1 != 0);
	LAUNCHIND[2] = (state.u.Launchind2 != 0);
	LAUNCHIND[3] = (state.u.Launchind3 != 0);
	LAUNCHIND[4] = (state.u.Launchind4 != 0);
	LAUNCHIND[5] = (state.u.Launchind5 != 0);
	LAUNCHIND[6] = (state.u.Launchind6 != 0);
	LAUNCHIND[7] = (state.u.Launchind7 != 0);
	LVRateLight = (state.u.LVRateLight != 0);
}

bool Saturn::ProcessConfigFileLine(FILEHANDLE scn, char *line)

{
	float ftcp;
	double autopTime;
	int SwitchState = 0;
	int nasspver = 0, status = 0;
	int n, DummyLoad;
	int LMPadLoadCount = 0, LMPadValueCount = 0;

   if (!strnicmp (line, "CONFIGURATION", 13)) {
        sscanf (line+13, "%d", &status);
	}
	else if (!strnicmp (line, "NASSPVER", 8)) {
		sscanf (line + 8, "%d", &nasspver);
	}
	else if (!strnicmp (line, "TOALT", 5)) {
        int toalt;
		sscanf (line+5, "%d", &toalt);
		agc.SetDesiredApogee(toalt);
		agc.SetDesiredPerigee(toalt * 0.98);
	}
	else if (!strnicmp (line, "TOAPO", 5)) {
        sscanf (line+5, "%f", &ftcp);
		agc.SetDesiredApogee(ftcp);
	}
	else if (!strnicmp (line, "TOPER", 5)) {
        sscanf (line+5, "%f", &ftcp);
		agc.SetDesiredPerigee(ftcp);
	}
	else if (!strnicmp (line, "BUILDSTATUS", 11)) {
		sscanf (line+11, "%d", &buildstatus);
	}
	else if (!strnicmp (line, "PANEL_ID", 8)) {
		sscanf (line+8, "%d", &PanelId);
	}
	else if (!strnicmp (line, "STAGESTATUS", 11)) {
		sscanf (line+11, "%d", &StageState);
	}
	else if (!strnicmp (line, "DOCKANGLE", 9)) {
		sscanf (line+5, "%g", &ftcp);
		DockAngle = ftcp;
	}
	else if (!strnicmp (line, "STAGE", 5)) {
		sscanf (line+5, "%d", &stage);
	}
	else if (!strnicmp (line, "TOHDG", 5)) {
		sscanf (line+5, "%g", &ftcp);

		//
		// Tell the AGC the heading we want.
		//

		agc.SetDesiredAzimuth(ftcp);
	}
	else if (!strnicmp (line, "VECHNO", 6)) {
        int numb;
		sscanf (line+6, "%d", &numb);
		VehicleNo = numb;
		SetVehicleStats();
	}
	else if (!strnicmp (line, "TCP", 3)) {
        sscanf (line+3, "%f", &ftcp);
		TCPO=ftcp;
	}
	else if (!strnicmp (line, "DLS", 3)) {
        sscanf (line+3, "%d", &DummyLoad);
		DeleteLaunchSite = (DummyLoad != 0);
	}
	else if (!strnicmp (line, "LOWRES", 6)) {
        sscanf (line+6, "%d", &DummyLoad);
		LowRes = (DummyLoad != 0);
	}
	else if (!strnicmp (line, "SICSHUT", 7)) {
		sscanf (line + 7, "%f", &ftcp);
		FirstStageCentreShutdownTime = ftcp;
	}
	else if (!strnicmp (line, "SIICSHUT", 8)) {
		sscanf (line + 8, "%f", &ftcp);
		SecondStageCentreShutdownTime = ftcp;
	}
	else if (!strnicmp (line, "SISHUT", 6)) {
		sscanf (line + 6, "%f", &ftcp);
		FirstStageShutdownTime = ftcp;
	}
	else if (!strnicmp (line, "SIISHUT", 7)) {
		sscanf (line + 7, "%f", &ftcp);
		SecondStageShutdownTime = ftcp;
	}
	else if (!strnicmp (line, "ISTGJT", 6)) {
		sscanf (line + 6, "%f", &ftcp);
		InterstageSepTime = ftcp;
	}
	else if (!strnicmp (line, "LESJT", 5)) {
		sscanf (line + 5, "%f", &ftcp);
		LESJettisonTime = ftcp;
	}
	else if (!strnicmp (line, "SIIPUT", 6)) {
		sscanf (line + 6, "%f", &ftcp);
		SecondStagePUShiftTime = ftcp;
	}
	else if (!strnicmp (line, "IGMST", 5)) {
		sscanf (line + 5, "%f", &ftcp);
		IGMStartTime = ftcp;
	}
	else if (!strnicmp (line, "THRUSTA", 7)) {
		sscanf (line + 7, "%f", &ftcp);
		ThrustAdjust = ftcp;
	}
	else if (!strnicmp (line, "LEM_DISPLAY", 11)) {
		LEM_DISPLAY = true;
	}
	else if (!strnicmp (line, "MAINSTATE", 9)) {
        SwitchState = 0;
		sscanf (line+9, "%d", &SwitchState);
		SetMainState(SwitchState);
	}
	else if (!strnicmp (line, "A13STATE", 8)) {
        SwitchState = 0;
		sscanf (line+8, "%d", &SwitchState);
		SetA13State(SwitchState);
	}
	else if (!strnicmp (line, "LAUNCHSTATE", 11)) {
        SwitchState = 0;
		sscanf (line+11, "%d", &SwitchState);
		SetLaunchState(SwitchState);
	}
	else if (!strnicmp (line, "LIGHTSTATE", 10)) {
        SwitchState = 0;
		sscanf (line+10, "%d", &SwitchState);
		SetLightState(SwitchState);
	}
	else if (!strnicmp (line, "VALVESTATE", 10)) {
		int valvestate = 0;
		int mask = 1;
		sscanf (line+10, "%d", &valvestate);
		for (n = 1; n < N_CSM_VALVES; n++) {
			ValveState[n] = ((valvestate & mask) != 0);
			mask <<= 1;
		}
	}
	else if (!strnicmp (line, "LMPADCNT", 8)) {
		if (!LMPad) {
			sscanf (line+8, "%d", &LMPadCount);
			if (LMPadCount > 0) {
				LMPad = new unsigned int[LMPadCount * 2];
			}
		}
	}
	else if (!strnicmp (line, "LMPAD", 5)) {
		unsigned int addr, val;
		sscanf (line+5, "%o %o", &addr, &val);
		LMPadValueCount++;
		if (LMPad && LMPadLoadCount < (LMPadCount * 2)) {
			LMPad[LMPadLoadCount++] = addr;
			LMPad[LMPadLoadCount++] = val;
		}
	}
	else if (!strnicmp (line, "CMPAD", 5)) {
		unsigned int addr, val;

		//
		// CM PAD value aren't saved, as the AGC will save them itself. They're only used to load values in
		// a starting scenario.
		//
		// Be sure that you put CMPAD lines _after_ the AGC entries, so that the AGC will know whether it's
		// running Virtual AGC or not. PAD loads are ignored if it's not a Virtual AGC.
		//

		sscanf (line+5, "%o %o", &addr, &val);
		agc.PadLoad(addr, val);
	}
	else if (!strnicmp (line, "REALISM", 7)) {
		sscanf (line+7, "%d", &Realism);
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
	}
	else if (!strnicmp (line, "LPSWITCH", 8)) {
        SwitchState = 0;
		sscanf (line+8, "%d", &SwitchState);
		SetLPSwitchState(SwitchState);
	}
	else if (!strnicmp (line, "RPSWITCH", 8)) {
        SwitchState = 0;
		sscanf (line+8, "%d", &SwitchState);
		SetRPSwitchState(SwitchState);
	}
	else if (!strnicmp (line, "APOLLONO", 8)) {
		sscanf (line+8, "%d", &ApolloNo);
	}
	else if (!strnicmp (line, "DOCKSTATE", 9)) {
        sscanf (line+9, "%d", &dockstate);
	}
	else if (!strnicmp (line, "AUTOTIMER", 9)) {
        sscanf (line+9, "%f", &ftcp);
		autopTime = ftcp;
	}
	else if (!strnicmp(line, "MISSNTIME", 9)) {
        sscanf (line+9, "%f", &ftcp);
		MissionTime = ftcp;
	}
	else if (!strnicmp(line, "MTD", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		MissionTimerDisplay.SetTime(ftcp);
	}
	else if (!strnicmp(line, "ETD", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		EventTimerDisplay.SetTime(ftcp);
	}
	else if (!strnicmp(line, "NMISSNTIME", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		NextMissionEventTime = ftcp;
	}
	else if (!strnicmp(line, "LMISSNTIME", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		LastMissionEventTime = ftcp;
	}
	else if (!strnicmp(line, "NFAILTIME", 9)) {
        sscanf (line + 9, "%f", &ftcp);
		NextFailureTime = ftcp;
	}
	else if (!strnicmp(line, "SIFUELMASS", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		SI_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "SIIFUELMASS", 11)) {
        sscanf (line + 11, "%f", &ftcp);
		SII_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "S4FUELMASS", 10)) {
        sscanf (line + 10, "%f", &ftcp);
		S4B_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "S4EMPTYMASS", 11)) {
        sscanf (line + 11, "%f", &ftcp);
		S4B_EmptyMass = ftcp;
		S4B_MassLoaded = true;
	}
	else if (!strnicmp(line, "SIEMPTYMASS", 11)) {
        sscanf (line + 11, "%f", &ftcp);
		SI_EmptyMass = ftcp;
		SI_MassLoaded = true;
	}
	else if (!strnicmp(line, "SIIEMPTYMASS", 12)) {
        sscanf (line + 12, "%f", &ftcp);
		SII_EmptyMass = ftcp;
		SII_MassLoaded = true;
	}
	else if (!strnicmp(line, "T1V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		THRUST_FIRST_VAC = ftcp;
		S1_ThrustLoaded = true;
	}
	else if (!strnicmp(line, "I1S", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_FIRST_SL = ftcp;
	}
	else if (!strnicmp(line, "I1V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_FIRST_VAC = ftcp;
	}
	else if (!strnicmp(line, "T2V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		THRUST_SECOND_VAC = ftcp;
		S2_ThrustLoaded = true;
	}
	else if (!strnicmp(line, "I2S", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_SECOND_SL = ftcp;
	}
	else if (!strnicmp(line, "I2V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_SECOND_VAC = ftcp;
	}
	else if (!strnicmp(line, "T3V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		THRUST_THIRD_VAC = ftcp;
		S3_ThrustLoaded = true;
	}
	else if (!strnicmp(line, "I3V", 3)) {
        sscanf (line + 3, "%f", &ftcp);
		ISP_THIRD_VAC = ftcp;
	}
	else if (!strnicmp(line, "PRELAUNCHATC", 12)) {
		int i;
		sscanf (line + 12, "%d", &i);
		UseATC = (i != 0);
	}
	else if (!strnicmp (line, "PMET", 4)) {
		sscanf(line+4, "%d", &n);
		sscanf(line+8, "%f", &ftcp);
		if (n >= 0 && n < PITCH_TABLE_SIZE) {
			met[n] = ftcp;
		}
	}
	else if (!strnicmp (line, "CPITCH", 6)) {
		sscanf(line+6, "%d", &n);
		sscanf(line+10, "%f", &ftcp);
		if (n >= 0 && n < PITCH_TABLE_SIZE) {
			cpitch[n] = ftcp;
		}
	}
	else if (!strnicmp(line, "MOONLAT", 7)) {
		sscanf(line + 7, "%f", &ftcp);
		LMLandingLatitude = ftcp;
	}
	else if (!strnicmp(line, "MOONLONG", 8)) {
		sscanf(line + 8, "%f", &ftcp);
		LMLandingLongitude = ftcp;
	}
	else if (!strnicmp(line, "MOONALT", 7)) {
		sscanf(line + 7, "%f", &ftcp);
		LMLandingAltitude = ftcp;
	}
	else if (!strnicmp(line, "UNMANNED", 8)) {
		int i;
		sscanf(line + 8, "%d", &i);
		Crewed = (i == 0);
	}
	else if (!strnicmp(line, "AUTOSLOW", 8)) {
		int i;
		sscanf(line + 8, "%d", &i);
		AutoSlow = (i != 0);
	}
	else if (!strnicmp(line, "S4PL", 4)) {
		sscanf(line + 4, "%d", &SIVBPayload);
	}
	else if (!strnicmp(line, "CSMSEP", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CSMSepTime = ftcp;
		CSMSepSet = true;
	}
	else if (!strnicmp(line, "CMSEP", 5)) {
		sscanf(line + 5, "%f", &ftcp);
		CMSepTime = ftcp;
		CMSepSet = true;
	}
	else if (!strnicmp(line, "S4APO", 5)) {
		sscanf(line + 5, "%f", &ftcp);
		SIVBApogee = ftcp;
	}
	else if (!strnicmp(line, "S4BURN", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		SIVBBurnStart = ftcp;
	}
	else if (!strnicmp(line, "CSMAPO", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CSMApogee = ftcp;
	}
	else if (!strnicmp(line, "CSMPER", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CSMPerigee = ftcp;
	}
	else if (!strnicmp(line, "CSMBURN", 7)) {
		sscanf(line + 7, "%f", &ftcp);
		CSMBurnStart = ftcp;
	}
	else if (!strnicmp(line, "CSMACCEL", 8)) {
		sscanf(line + 8, "%f", &ftcp);
		CSMAccelSet = true;
		CSMAccelTime = ftcp;
	}
	else if (!strnicmp(line, "CSMACCEND", 9)) {
		sscanf(line + 9, "%f", &ftcp);
		CSMAccelEnd = ftcp;
	}
	else if (!strnicmp(line, "CSMACCPITCH", 11)) {
		sscanf(line + 11, "%f", &ftcp);
		CSMAccelPitch = ftcp;
	}
	else if (!strnicmp(line, "SMFUELLOAD", 10)) {
		sscanf(line + 10, "%f", &ftcp);
		SM_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "CMFUELLOAD", 10)) {
		sscanf(line + 10, "%f", &ftcp);
		CM_FuelMass = ftcp;
	}
	else if (!strnicmp(line, "SMMASS", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		SM_EmptyMass = ftcp;
	}
	else if (!strnicmp(line, "CMMASS", 6)) {
		sscanf(line + 6, "%f", &ftcp);
		CM_EmptyMass = ftcp;
	}
	else if (!strnicmp(line, "LANDFAIL", 8)) {
		sscanf(line + 8, "%d", &LandFail.word);
	}
	else if (!strnicmp(line, "LAUNCHFAIL", 10)) {
		sscanf(line + 10, "%d", &LaunchFail.word);
	}
	else if (!strnicmp(line, "SWITCHCHFAIL", 10)) {
		sscanf(line + 10, "%d", &SwitchFail.word);
	}
	else if (!strnicmp(line, "LANG", 4)) {
		strncpy (AudioLanguage, line + 5, 64);
	}
	else if (!strnicmp(line, "LEMN", 4)) {
		strncpy (LEMName, line + 5, 64);
	}
	else if (!strnicmp(line, DSKY_START_STRING, sizeof(DSKY_START_STRING))) {
		dsky.LoadState(scn, DSKY_END_STRING);
	}
	else if (!strnicmp(line, DSKY2_START_STRING, sizeof(DSKY2_START_STRING))) {
		dsky2.LoadState(scn, DSKY2_END_STRING);
	}
	else if (!strnicmp(line, FDAI_START_STRING, sizeof(FDAI_START_STRING))) {
		fdaiLeft.LoadState(scn, FDAI_END_STRING);
	}
	else if (!strnicmp(line, FDAI2_START_STRING, sizeof(FDAI2_START_STRING))) {
		fdaiRight.LoadState(scn, FDAI2_END_STRING);
	}
	else if (!strnicmp(line, AGC_START_STRING, sizeof(AGC_START_STRING))) {
		agc.LoadState(scn);
	}
	else if (!strnicmp(line, IMU_START_STRING, sizeof(IMU_START_STRING))) {
		imu.LoadState(scn);
	}
	else if (!strnicmp(line, GDC_START_STRING, sizeof(GDC_START_STRING))) {
		gdc.LoadState(scn);
	}
	else if (!strnicmp(line, ASCP_START_STRING, sizeof(ASCP_START_STRING))) {
		ascp.LoadState(scn);
	}
	else if (!strnicmp(line, IU_START_STRING, sizeof(IU_START_STRING))) {
		iu.LoadState(scn);
	}
	else if (!strnicmp(line, CWS_START_STRING, sizeof(CWS_START_STRING))) {
		cws.LoadState(scn);
	}
	else if (!strnicmp(line, DOCKINGPROBE_START_STRING, sizeof(DOCKINGPROBE_START_STRING))) {
		dockingprobe.LoadState(scn);
	}
	else if (!strnicmp (line, "SYSTEMSSTATE", 12)) {
		sscanf (line + 12, "%d", &systemsState);
	}
	else if (!strnicmp (line, "LSYSTEMSMISSNTIME", 17)) {
		sscanf (line + 17, "%f", &ftcp);
		lastSystemsMissionTime = ftcp;
	}
    else if (!strnicmp (line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
		Panelsdk.Load(scn);			//send the loading to the Panelsdk
	}
    else if (!strnicmp (line, PANELSWITCH_START_STRING, strlen(PANELSWITCH_START_STRING))) { 
		PSH.LoadState(scn);	
	}
	else if (!strnicmp (line, "COASENABLED", 11)) {
		sscanf (line + 11, "%i", &coasEnabled);
	}
	else if (!strnicmp (line, "FDAIDISABLED", 12)) {
		sscanf (line + 12, "%i", &fdaiDisabled);
	}
	else if (!strnicmp (line, "FDAISMOOTH", 10)) {
		sscanf (line + 10, "%i", &fdaiSmooth);
	}
	else if (!strnicmp (line, "MR", 2)) {
		sscanf (line + 2, "%f", &ftcp);
		MixtureRatio = ftcp;
	}
	else
		return false;

	return true;
}

void Saturn::GetScenarioState (FILEHANDLE scn, void *vstatus)

{
    char VNameApollo[256];
	char *line;
	int n;

	//
	// If the name of the spacecraft is "AS-xxx" then get the vehicle
	// number from that.
	//

	strncpy (VNameApollo, GetName(), 255);
	if (!strnicmp (VNameApollo, "AS-", 3)) {
		sscanf(VNameApollo+3, "%d", &n);
		if (n > 200) {
			VehicleNo = n;
			SetVehicleStats();
		}
	}

	//
	// Seed the random number generator with whatever junk we can
	// find.
	//

	srandom(VehicleNo + (int) vstatus + time(0));

	//
	// At some point we should reorder these checks by length, to minimise the chances
	// of accidentally matching a longer string.
	//

	while (oapiReadScenario_nextline (scn, line)) {
		if (!ProcessConfigFileLine(scn, line)) {
			ParseScenarioLineEx (line, vstatus);
        }
    }

	if (!Crewed && (CSMApogee > 0.0) && (CSMBurnStart > 0) && (stage < CM_STAGE)) {
		CSMBurn = true;
	}

	soundlib.SetLanguage(AudioLanguage);
	LoadDefaultSounds();

	//
	// Recalculate stage masses.
	//

	UpdatePayloadMass();
	CalculateStageMass ();

	//
	// And pass it the mission number and realism settings.
	//

	agc.SetMissionInfo(ApolloNo, Realism, LEMName);

	//
	// Tell various systems the realism setting
	//

	MainPanel.SetRealism(Realism);
	dockingprobe.SetRealism(Realism);

	//
	// Set random failures if appropriate.
	//

	if (!ApolloNo && (Realism > 4)) {
		SetRandomFailures();
	}
}

//
// Set the appropriate mass based on the SIVB payload. I believe these are roughly the
// correct numbers.
//

void Saturn::UpdatePayloadMass()

{
	switch (SIVBPayload) {
	case PAYLOAD_LEM:
		// default, do nothing.
		break;

	case PAYLOAD_ASTP:
		S4PL_Mass = 2012;
		break;

	case PAYLOAD_LTA:
		S4PL_Mass = 13381;
		break;

	case PAYLOAD_LTA6:
		S4PL_Mass = 11794;
		break;

	case PAYLOAD_LM1:
		S4PL_Mass = 14360;
		break;

	case PAYLOAD_LTA8:
		S4PL_Mass = 9026;
		break;

	case PAYLOAD_TARGET:
		S4PL_Mass = 1000; // Guess
		break;

	default:
		S4PL_Mass = 0;
		break;
	}
}

//
// Destroy any old stages that we don't want to keep around.
//

void Saturn::DestroyStages(double simt)

{
	if (hstg1) {
		KillAlt(hstg1, 60);
	}

	if (hstg2) {
		KillAlt(hstg2, 1000);
	}

	if (hintstg) {
		KillAlt(hintstg, 1000);
	}

	if (hesc1) {
		KillAlt(hesc1,90);
	}

	if (hPROBE) {
		KillDist(hPROBE);
	}

	if (hs4b1) {
		KillDist(hs4b1);
	}
	if (hs4b2) {
		KillDist(hs4b2);
	}
	if (hs4b3) {
		KillDist(hs4b3);
	}

	if (hs4b4) {
		KillDist(hs4b4);
	}

	//
	// In most missions we can delete the VAB, Crawler and mobile launcher when we can no longer
	// see them.
	//

	if (DeleteLaunchSite && stage >= STAGE_ORBIT_SIVB) {
		if (hVAB) {
			KillDist(hVAB, 100000.0);
		}

		if (hCrawler) {
			KillDist(hCrawler, 10000.0);
		}

		if (hML) {
			KillDist(hML, 50000.0);
		}
	}

	//
	// Destroy seperated SM when it drops too low in the atmosphere.
	//

	if (hSMJet) {
		KillAlt(hSMJet, 45000);
	}
}

void Saturn::CheckSMSystemsState()

{
	//
	// If we've seperated from the SM, disable the fuel cells so they appear to have
	// been disconnected.
	//

	if (stage >= CM_STAGE) {
		int i;
		for (i = 0; i < 3; i++) {
			if (FuelCells[i])
				FuelCells[i]->Disable();
		}

		//
		// HACK: For now, hard-wire the buses to the batteries.
		//

		if (MainBusA) {
			MainBusA->WireTo(EntryBatteryA);
			MainBusB->WireTo(EntryBatteryB);
		}
	}
}

void Saturn::SetStage(int s)

{
	stage = s;
	StageState = 0;

	CheckSMSystemsState();
	CheckRCSState();
	CheckSPSState();
}

void Saturn::DoLaunch(double simt)

{
	//
	// Uncage IMU
	//
	IMUGuardedCageSwitch.SwitchTo(TOGGLESWITCH_DOWN); 

	//
	// Light the liftoff indicator for the crew.
	//

	SetLiftoffLight();

	//
	// Switch to the first launch stage.
	//

	SetStage(LAUNCH_STAGE_ONE);

	//
	// For now, we'll reset the mission timer to zero and enable it.
	//
	// The people on the ProjectApollo mailing list believe that this is the correct
	// behaviour for the Mission Timer, and it shouldn't run at all until liftoff.
	//

	MissionTimerDisplay.Reset();
	MissionTimerDisplay.SetEnabled(true);
	EventTimerDisplay.Reset();
	EventTimerDisplay.SetEnabled(true);
	EventTimerDisplay.SetRunning(true);

	//
	// Tell the AGC that we've lifted off.
	//

	agc.SetInputChannelBit(030, 5, true);

	//
	// Set full thrust, just in case.
	//

	SetThrusterGroupLevel(thg_main, 1.0);

	//
	// And play the launch sound.
	//

	if (LaunchS.isValid() && !LaunchS.isPlaying()){
		LaunchS.play(NOLOOP,255);
		LaunchS.done();
	}

	//
	// Ensure autopilot is on at launch.
	//

	autopilot = true;
}

void Saturn::GenericTimestep(double simt, double simdt)

{
	if (GenericFirstTimestep) {
		//
		// Do any generic setup.
		//

		//
		// Get the handles for any odds and ends that are out there.
		//

		char VName[256];
		char ApolloName[64];

		GetApolloName(ApolloName);
		strcpy (VName, ApolloName); strcat (VName, "-ML");
		hML = oapiGetVesselByName(VName);
		strcpy (VName, ApolloName); strcat (VName, "-CT");
		hCrawler = oapiGetVesselByName(VName);
		hVAB = oapiGetVesselByName("VAB");

		GenericFirstTimestep = false;
	}

	//
	// Update mission time and mission timer.
	//

	MissionTime += simdt;
	MissionTimerDisplay.Timestep(simt, simdt);
	EventTimerDisplay.Timestep(simt, simdt);

	//
	// Timestep tracking.
	//

#ifdef TRACK_TIMESTEPS
	CurrentTimestep++;
	if (oapiGetSimStep() > LongestTimestep) {
		LongestTimestep = CurrentTimestep;
		LongestTimestepLength = oapiGetSimStep();
	}
#endif // TRACK_TIMESTEPS

	//
	// Reduce jostle.
	//

	ViewOffsetx *= 0.95;
	ViewOffsety *= 0.95;
	ViewOffsetz *= 0.95;

	//
	// And update for acceleration.
	//

	double amt = fabs(aHAcc / 25.0) - 0.1;
	if (amt > 0.25)
		amt = 0.25;

	if (amt > 0)
		JostleViewpoint(amt);

	//
	// Velocity calculations
	//

	VESSELSTATUS status;
	GetStatus(status);
	
	double aSpeed = length(status.rvel);
	actualVEL = (aSpeed / 1000.0 * 3600.0);
	actualALT = GetAltitude();
	actualFUEL = ((GetFuelMass() * 100.0) / GetMaxFuelMass());
		
	VECTOR3 hvel;
	GetHorizonAirspeedVector(hvel);
	aVSpeed = hvel.y;

	// Manage velocity cache
	for (int i = LASTVELOCITYCOUNT - 1; i > 0; i--) {
		LastVelocity[i] = LastVelocity[i - 1];
		LastVerticalVelocity[i] = LastVerticalVelocity[i -1];
		LastSimt[i] = LastSimt[i - 1];
	}
	if (LastVelocityFilled < LASTVELOCITYCOUNT - 1)	LastVelocityFilled++;

	// Store current velocities
	LastVelocity[0] = status.rvel;
	LastVerticalVelocity[0] = aVSpeed;
	LastSimt[0] = simt;

	// Calculate accelerations
	if (LastVelocityFilled > 0) {
		aHAcc = (aSpeed - length(LastVelocity[LastVelocityFilled])) / (simt - LastSimt[LastVelocityFilled]);
		aVAcc = (aVSpeed - LastVerticalVelocity[LastVelocityFilled]) / (simt - LastSimt[LastVelocityFilled]);

		//  This stuff is to compute the component of the total acceleration
		//	along the z axis. This supports the "ACCEL G meter" on the panel.
		double agrav, radius, mass, calpha, salpha, cbeta, sbeta, radius2, DVX, DVY, DVZ;
		OBJHANDLE hPlanet;

		calpha = cos(status.arot.x);
		cbeta = cos(status.arot.y);
		salpha = sin(status.arot.x);
		sbeta = sin(status.arot.y);

		DVX = status.rvel.x - LastVelocity[LastVelocityFilled].x;
		DVY = status.rvel.y - LastVelocity[LastVelocityFilled].y;
		DVZ = status.rvel.z - LastVelocity[LastVelocityFilled].z;
		DVZ = cbeta * (DVY * salpha + DVZ * calpha) - DVX * sbeta;
		aZAcc = DVZ / (simt - LastSimt[LastVelocityFilled]);

		hPlanet = GetSurfaceRef();
		mass = oapiGetMass(hPlanet);
		radius2 = status.rpos.x * status.rpos.x + status.rpos.y * status.rpos.y + status.rpos.z * status.rpos.z;
		radius = sqrt(radius2);
		agrav = cbeta * (status.rpos.y * salpha + status.rpos.z * calpha) - status.rpos.x * sbeta;
		agrav *= GKSI * mass / (radius * radius2);
		aZAcc += agrav;
	}

	//
	// Update parameters for AGC processing? Should these be here?
	//

	agc.SetFuel(actualFUEL);
	agc.SetRVel(aSpeed);

	SystemsTimestep(simt, simdt);

	if(stage < LAUNCH_STAGE_SIVB) {
		if (GetNavmodeState(NAVMODE_KILLROT)) {
			DeactivateNavmode(NAVMODE_KILLROT);
		}
	}

	//
	// We die horribly if you set 100x or higher acceleration during launch.
	//

	if (stage >= LAUNCH_STAGE_ONE && stage <= LAUNCH_STAGE_SIVB &&
		oapiGetTimeAcceleration() > 10.0) {
		oapiSetTimeAcceleration(10.0);
	}

	//
	// And you'll also die horribly if you set time acceleration at all in the
	// early parts of the launch.
	//

	if (stage == LAUNCH_STAGE_ONE && MissionTime < 50 &&
		oapiGetTimeAcceleration() > 1.0) {
		oapiSetTimeAcceleration(1.0);
	}

	//
	// Only the SM has linear thrusters.
	//

	if (stage != CSM_LEM_STAGE) {
		if (GetAttitudeMode() == ATTMODE_LIN){
			SetAttitudeMode(ATTMODE_ROT);
		}
	}

	if (GetAltitude() < 1470) {
		actualVEL = actualVEL-1470+GetAltitude();
	}

	if (GroundContact()) {
		actualVEL = 0;
	}

	if (habort) {
		double altabort;
		int getIT;

		getIT = oapiGetAltitude(habort,&altabort);

		if (altabort < 100 && getIT > 0) {
			oapiDeleteVessel(habort,GetHandle());
			habort = NULL;
		}
	}

	// Docking radar sound only for CSM_LEM_STAGE when nothing docked
	if (stage == CSM_LEM_STAGE) {
		if (!dockingprobe.GetDocked())
			soundlib.SoundOptionOnOff(PLAYRADARBIP, TRUE);
		else
			soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	}
	else {
		soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	}

	if (MissionTime >= NextDestroyCheckTime) {
		DestroyStages(simt);
		NextDestroyCheckTime = MissionTime + 1.0;
	}
}

void StageTransform(VESSEL *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel)
{
	VECTOR3 rofs, rvel = {vs->rvel.x, vs->rvel.y, vs->rvel.z};

	rofs.x = 0;
	rofs.y = 0;
	rofs.z = 0;

	// Staging Velocity represents
	// Need to do some transforms to get the correct vector to eject the stage

	vessel->Local2Rel (ofs, vs->rpos);
	vessel->GlobalRot (vel, rofs);
	vs->rvel.x = rvel.x+rofs.x;
	vs->rvel.y = rvel.y+rofs.y;
	vs->rvel.z = rvel.z+rofs.z;
}

int Saturn::clbkConsumeDirectKey(char *keystate)

{
	if (KEYMOD_SHIFT (keystate))
	{
		return 0;
	}
	else if (KEYMOD_CONTROL (keystate))
	{
	}
	else
	{
		if (KEYDOWN (keystate, OAPI_KEY_R)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_R, 1.0)) {
				{
					if (stage == CM_ENTRY_STAGE_SEVEN && HatchOpen) {
						bRecovery = true;
					}
				}
			return 1;
			}
		}

		if (KEYDOWN (keystate, OAPI_KEY_5)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_5, 1.0)){
				if (ActivateLEM){
					ActivateLEM=false;
				}
				return 1;
			}
		}

		if (KEYDOWN (keystate, OAPI_KEY_9) && (stage == CSM_LEM_STAGE || stage == CM_RECOVERY_STAGE)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_9, 1.0)) {
				viewpos = SATVIEW_DOCK;
				SetView(true);
			}
			return 1;
		}

		if (KEYDOWN (keystate, OAPI_KEY_8)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_8, 1.0)) {
				viewpos = SATVIEW_DMP;
				SetView(true);
			}
			return 1;
		}

		if (KEYDOWN (keystate, OAPI_KEY_7)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_7, 1.0)) {
				viewpos = SATVIEW_CMP;
				SetView(true);
			}
			return 1;
		}

		if (KEYDOWN (keystate, OAPI_KEY_6)) {
			if (oapiAcceptDelayedKey (OAPI_KEY_6, 1.0)) {
				viewpos = SATVIEW_CDR;
				SetView(true);
			}
			return 1;
		}

		//
		// We only allow this switch in VC mode, as we need to disable the panel when selecting these
		// cameras.
		//
		// For now this is limited to the Saturn V.
		//

		if (InVC && KEYDOWN (keystate, OAPI_KEY_1) && iu.IsTLICapable() && stage < LAUNCH_STAGE_SIVB && stage >= LAUNCH_STAGE_ONE) {
			if (oapiAcceptDelayedKey (OAPI_KEY_1, 1.0)) {
				viewpos = SATVIEW_ENG1;
				SetView();
				oapiCameraAttach(GetHandle(), CAM_COCKPIT);
			}
			return 1;
		}

		if (InVC && KEYDOWN (keystate, OAPI_KEY_2) && iu.IsTLICapable() && stage < LAUNCH_STAGE_SIVB && stage >= LAUNCH_STAGE_ONE) {
			if (oapiAcceptDelayedKey (OAPI_KEY_2, 1.0)) {
				viewpos = SATVIEW_ENG2;
				oapiCameraAttach(GetHandle(), CAM_COCKPIT);
				SetView();
			}
			return 1;
		}

		if (InVC && KEYDOWN (keystate, OAPI_KEY_3) && iu.IsTLICapable() && stage < STAGE_ORBIT_SIVB && stage >= PRELAUNCH_STAGE) {
			if (oapiAcceptDelayedKey (OAPI_KEY_3, 1.0)) {
				viewpos = SATVIEW_ENG3;
				oapiCameraAttach(GetHandle(), CAM_COCKPIT);
				SetView();
			}
			return 1;
		}

	}
	return 0;
}

int Saturn::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	if (KEYMOD_SHIFT(kstate)){
		// Do DSKY stuff
		if(down){
			switch(key){
				case OAPI_KEY_PRIOR:
					dsky.ResetPressed();
					break;
				case OAPI_KEY_NEXT:
					dsky.KeyRel();
					break;
				case OAPI_KEY_NUMPADENTER:
					dsky.EnterPressed();
					break;
				case OAPI_KEY_DIVIDE:
					dsky.VerbPressed();
					break;
				case OAPI_KEY_MULTIPLY:
					dsky.NounPressed();
					break;
				case OAPI_KEY_ADD:
					dsky.PlusPressed();
					break;
				case OAPI_KEY_SUBTRACT:
					dsky.MinusPressed();
					break;
				case OAPI_KEY_DECIMAL:
					dsky.ProgPressed();
					break;
				case OAPI_KEY_NUMPAD1:
					dsky.NumberPressed(1);
					break;
				case OAPI_KEY_NUMPAD2:
					dsky.NumberPressed(2);
					break;
				case OAPI_KEY_NUMPAD3:
					dsky.NumberPressed(3);
					break;
				case OAPI_KEY_NUMPAD4:
					dsky.NumberPressed(4);
					break;
				case OAPI_KEY_NUMPAD5:
					dsky.NumberPressed(5);
					break;
				case OAPI_KEY_NUMPAD6:
					dsky.NumberPressed(6);
					break;
				case OAPI_KEY_NUMPAD7:
					dsky.NumberPressed(7);
					break;
				case OAPI_KEY_NUMPAD8:
					dsky.NumberPressed(8);
					break;
				case OAPI_KEY_NUMPAD9:
					dsky.NumberPressed(9);
					break;
				case OAPI_KEY_NUMPAD0:
					dsky.NumberPressed(0);
					break;
			}
		}else{
			// KEY UP
			switch(key){
				case OAPI_KEY_DECIMAL:
					dsky.ProgReleased();
					break;
			}
		}
		return 0;
	}
	if (KEYMOD_CONTROL(kstate)) {
		return 0; 
	}

	// Separate stages and undock with keypress if REALISM 0
	if (!Realism && key == OAPI_KEY_S && down == true) {

		if (stage == CSM_LEM_STAGE) {			
			bManualUnDock = true;
		
		} else {
			bManualSeparate = true;
		}
	}
	return 0;
}

void Saturn::AddRCSJets(double TRANZ, double MaxThrust)

{
	int i;
	const double ATTCOOR = 0;
	const double ATTCOOR2 = 2.05;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.25;
	const double RCSOFFSET2=-0.25;
	const double RCSOFFSETM=-0.05;
	const double RCSOFFSETM2=0.05;

	//
	// Clear any old thrusters.
	//

	for (i = 0; i < 24; i++) {
		th_att_lin[i] = 0;
		th_att_rot[i] = 0;
	}

	//
	// Adjust ISP and thrust based on realism level.
	//

	double RCS_ISP = (SM_RCS_ISP * (15.0 - Realism)) / 5.0;
	double RCS_Thrust = (MaxThrust * (15.0 - Realism)) / 5.0;

	//
	// CM RCS Propellant tanks
	//

	//
	// RCS0 = quad A
	// RCS1 = quad B
	// RCS2 = quad C
	// RCS3 = quad D
	//

	if (!ph_rcs0)
		ph_rcs0  = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcs1)
		ph_rcs1  = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcs2)
		ph_rcs2  = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcs3)
		ph_rcs3  = CreatePropellantResource(RCS_FUEL_PER_QUAD);

	const double CENTEROFFS = 0.25;

	th_att_lin[0]=th_att_rot[0]=CreateThruster (_V(-CENTEROFFS,ATTCOOR2,TRANZ+RCSOFFSET2), _V(0,-0.1,1), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[1]=th_att_rot[3]=CreateThruster (_V(CENTEROFFS,-ATTCOOR2,TRANZ+RCSOFFSET2), _V(0,0.1,1), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[2]=th_att_rot[4]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS,TRANZ+RCSOFFSET2), _V(0.1,0,1), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[3]=th_att_rot[7]=CreateThruster (_V(ATTCOOR2,CENTEROFFS,TRANZ+RCSOFFSET2), _V(-0.1,0,1), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[4]=th_att_rot[2]=CreateThruster (_V(-CENTEROFFS,ATTCOOR2,TRANZ+RCSOFFSET), _V(0,-0.1,-1), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[5]=th_att_rot[1]=CreateThruster (_V(CENTEROFFS,-ATTCOOR2,TRANZ+RCSOFFSET), _V(0,0.1,-1), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[6]=th_att_rot[6]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS,TRANZ+RCSOFFSET), _V(0.1,0,-1), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[7]=th_att_rot[5]=CreateThruster (_V(ATTCOOR2,CENTEROFFS,TRANZ+RCSOFFSET), _V(-0.1,0,-1), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[8]=th_att_rot[16]=th_att_rot[17]=CreateThruster (_V(-CENTEROFFS - 0.2,ATTCOOR2,TRANZ+RCSOFFSETM), _V(1,-0.1,0), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[9]=th_att_rot[8]=th_att_rot[9]=CreateThruster (_V(CENTEROFFS -0.2,-ATTCOOR2,TRANZ+RCSOFFSETM2), _V(1,0.1,0), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[12]=th_att_rot[10]=th_att_rot[11]=CreateThruster (_V(-CENTEROFFS + 0.2,ATTCOOR2,TRANZ+RCSOFFSETM2), _V(-1,-0.1,0), RCS_Thrust, ph_rcs0, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[13]=th_att_rot[18]=th_att_rot[19]=CreateThruster (_V(CENTEROFFS + 0.2,-ATTCOOR2,TRANZ+RCSOFFSETM), _V(-1,0.1,0), RCS_Thrust, ph_rcs2, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[16]=th_att_rot[14]=th_att_rot[15]=CreateThruster (_V(ATTCOOR2,CENTEROFFS -0.2,TRANZ+RCSOFFSETM2), _V(-0.1,1,0), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[17]=th_att_rot[22]=th_att_rot[23]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS -0.2,TRANZ+RCSOFFSETM), _V(-0.1,1,0), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[20]=th_att_rot[20]=th_att_rot[21]=CreateThruster (_V(ATTCOOR2,CENTEROFFS + 0.2,TRANZ+RCSOFFSETM), _V(-0.1,-1,0), RCS_Thrust, ph_rcs1, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[21]=th_att_rot[12]=th_att_rot[13]=CreateThruster (_V(-ATTCOOR2,-CENTEROFFS + 0.2,TRANZ+RCSOFFSETM2), _V(0.1,-1,0), RCS_Thrust, ph_rcs3, RCS_ISP, SM_RCS_ISP_SL);

	CreateThrusterGroup (th_att_lin,   4, THGROUP_ATT_FORWARD);
	CreateThrusterGroup (th_att_lin+4, 4, THGROUP_ATT_BACK);
	CreateThrusterGroup (th_att_lin+8,   2, THGROUP_ATT_RIGHT);
	CreateThrusterGroup (th_att_lin+12, 2, THGROUP_ATT_LEFT);
	CreateThrusterGroup (th_att_lin+16,   2, THGROUP_ATT_UP);
	CreateThrusterGroup (th_att_lin+20,   2, THGROUP_ATT_DOWN);

	CreateThrusterGroup (th_att_rot,   2, THGROUP_ATT_PITCHDOWN);
	CreateThrusterGroup (th_att_rot+2,   2, THGROUP_ATT_PITCHUP);
	CreateThrusterGroup (th_att_rot+4,   2, THGROUP_ATT_YAWRIGHT);
	CreateThrusterGroup (th_att_rot+6,   2, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+8,   8, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+16,   8, THGROUP_ATT_BANKRIGHT);

	for (i = 0; i < 24; i++) {
		if (th_att_lin[i])
			AddExhaust (th_att_lin[i], 1.2, 0.18, SMExhaustTex);
	}

	//
	// Map thrusters to RCS quads. Note that we don't use entry zero, we're matching the array to
	// Apollo numbering for simplicity... we also have to include the fake thrusters here so we
	// can enable and disable them.
	//

	th_rcs_a[1] = th_att_rot[16];
	th_rcs_a[2] = th_att_rot[10];
	th_rcs_a[3] = th_att_rot[2];
	th_rcs_a[4] = th_att_rot[0];

	th_rcs_b[1] = th_att_rot[20];
	th_rcs_b[2] = th_att_rot[14];
	th_rcs_b[3] = th_att_rot[5];
	th_rcs_b[4] = th_att_rot[7];

	th_rcs_c[1] = th_att_rot[18];
	th_rcs_c[2] = th_att_rot[8];
	th_rcs_c[3] = th_att_rot[3];
	th_rcs_c[4] = th_att_rot[1];

	th_rcs_d[1] = th_att_rot[22];
	th_rcs_d[2] = th_att_rot[12];
	th_rcs_d[3] = th_att_rot[4];
	th_rcs_d[4] = th_att_rot[6];
}

void Saturn::AddRCS_CM(double MaxThrust)

{	
	// DS20060222 Extensively Modified, see comments below
	const double ATTCOOR = 0.95;
	const double ATTCOOR2 = 1.92;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double TRANZ=-0.65;
	const double ATTWIDTH=.15;
	const double ATTHEIGHT=.2;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=.6;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;
	VECTOR3 m_exhaust_pos2= {0,ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos3= {0,-ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos4= {-ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_pos5= {ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_ref2 = {0,0.1,-1};
	VECTOR3 m_exhaust_ref3 = {0,-0.1,-1};
	VECTOR3 m_exhaust_ref4 = {-0.1,0,-1};
	VECTOR3 m_exhaust_ref5 = {0.1,0,-1};

	// DS20060223 The number 154.4482019 is the combined fuel + oxidizer capacity of one pair of CM RCS tanks.
	if (!ph_rcs_cm_1)
		ph_rcs_cm_1 = CreatePropellantResource(154.4482019); // Was RCS_FUEL_CM
	if (!ph_rcs_cm_2)
		ph_rcs_cm_2 = CreatePropellantResource(154.4482019);

	//
	// display rcs stage propellant level in generic HUD
	//

	SetDefaultPropellantResource (ph_rcs_cm_1);
	ClearThrusterDefinitions();

	//
	// Adjust ISP and thrust based on realism level.
	//

	// These will need to be re-adjusted for realism
	double RCS_ISP = 290*G;    // (CM_RCS_ISP * (15.0 - Realism)) / 5.0;
	double RCS_Thrust = 410; // (MaxThrust * (15.0 - Realism)) / 5.0;

	// DS20060221 Multiple Edits As Follows:
	// A) Rearrange these so that they make more sense	
	// B) Delete the extra thrusters that shouldn't be here
	// C) Reposition the thrusters to their original correct positions

	// For thrusters - X means LEFT/RIGHT, Y means IN/OUT, Z means UP/DOWN)

	// Jet #1 -- This used to be at _V(2,2,-2), _V(0,-1,0)
	th_att_cm[0]=CreateThruster (_V(0.1,ATTCOOR2,TRANZ +0.05), _V(0,-1,0), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[0],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	// Jet #3 
	th_att_cm[1]=CreateThruster (_V(-0.1,ATTCOOR2,TRANZ +0.05), _V(0,-1,0), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[1],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	CreateThrusterGroup (th_att_cm,   2, THGROUP_ATT_PITCHUP);

	// Jet #2 -- Used to be at (_V(2,2,2), _V(0,-1,0)
	th_att_cm[2]=CreateThruster (_V(0.1,ATTCOOR+0.1,TRANZ+1.4), _V(0,-1,0), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[2],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	// Jet #4
	th_att_cm[3]=CreateThruster (_V(-0.1,ATTCOOR+0.1,TRANZ+1.4), _V(0,-1,0), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[3],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	CreateThrusterGroup (th_att_cm+2,   2, THGROUP_ATT_PITCHDOWN);

	// Jet #5 -- Used to be at _V(-2,2,2), _V(0,0,-1)
	th_att_cm[4]=CreateThruster (_V(ATTCOOR2,0.1,TRANZ), _V(0,0,-1), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[4],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	// Jet #7
	th_att_cm[5]=CreateThruster (_V(ATTCOOR2,-0.1,TRANZ), _V(0,0,-1), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[5],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	CreateThrusterGroup (th_att_cm+4,   2, THGROUP_ATT_YAWRIGHT);

	// Jet #6 -- Used to be at _V(-2,2,-2), _V(0,0,1)
	th_att_cm[6]=CreateThruster (_V(-ATTCOOR2,0.1,TRANZ), _V(0,0,-1), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[6],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	// Jet #8
	th_att_cm[7]=CreateThruster (_V(-ATTCOOR2,-0.1,TRANZ), _V(0,0,-1), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[7],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	CreateThrusterGroup (th_att_cm+6,   2, THGROUP_ATT_YAWLEFT);

	// The roll jets introduce a slight upward pitch if not corrected for.
	// Apparently the AGC expects this.

	// Jet #9 -- Used to be at _V(2,2,-2)
	th_att_cm[8]=CreateThruster (_V(ATTCOOR2/1.4,ATTCOOR2/1.4,TRANZ), _V(0,-1,0), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[8],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	// Jet #11
	th_att_cm[9]=CreateThruster (_V(-ATTCOOR2/1.4,(ATTCOOR2/1.4)+0.1,TRANZ), _V(0,1,0), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[9],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	CreateThrusterGroup (th_att_cm+8,   2, THGROUP_ATT_BANKRIGHT);
		
	// Jet #10 -- Used to be at _V(-2,2,-2), _V(0,-1,0)
	th_att_cm[10]=CreateThruster (_V(-ATTCOOR2/1.4,ATTCOOR2/1.4,TRANZ), _V(0,-1,0), RCS_Thrust, ph_rcs_cm_2, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[10],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	// Jet #12
	th_att_cm[11]=CreateThruster (_V(ATTCOOR2/1.4,(ATTCOOR2/1.4)+0.1,TRANZ), _V(0,1,0), RCS_Thrust, ph_rcs_cm_1, RCS_ISP, CM_RCS_ISP_SL);
	AddExhaust(th_att_cm[11],0.01,0.01); // ATTWIDTH,ATTHEIGHT);
	CreateThrusterGroup (th_att_cm+10,   2, THGROUP_ATT_BANKLEFT);
}

void Saturn::AddRCS_S4B()

{
	const double ATTCOOR = -10;
	const double ATTCOOR2 = 3.61;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double TRANZ=-3.2-STG2O;
	const double ATTWIDTH=.2;
	const double ATTHEIGHT=.5;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;
	const double RCSX=0.35;
	VECTOR3 m_exhaust_pos2= {0,ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos3= {0,-ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos4= {-ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_pos5= {ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_ref2 = {0,0.1,-1};
	VECTOR3 m_exhaust_ref3 = {0,-0.1,-1};
	VECTOR3 m_exhaust_ref4 = {-0.1,0,-1};
	VECTOR3 m_exhaust_ref5 = {0.1,0,-1};
	double offset;
	offset = 0.0;
	if((ApolloNo<8)&&(ApolloNo!=6)&&(ApolloNo!=4))offset=7.7;

	th_att_rot[0] = CreateThruster (_V(0,ATTCOOR2+0.15,TRANZ-0.25+offset), _V(0, -1,0), 20740.0, ph_3rd,5000000, 4000000);
	th_att_rot[1] = CreateThruster (_V(0,-ATTCOOR2-0.15,TRANZ-0.25+offset), _V(0,1,0),20740.0, ph_3rd,5000000, 4000000);
	
	AddExhaust (th_att_rot[0], 0.6, 0.078);
	AddExhaust (th_att_rot[1], 0.6, 0.078);
	CreateThrusterGroup (th_att_rot,   1, THGROUP_ATT_PITCHUP);
	CreateThrusterGroup (th_att_rot+1, 1, THGROUP_ATT_PITCHDOWN);


	th_att_rot[2] = CreateThruster (_V(RCSX,ATTCOOR2-0.2,TRANZ-0.25+offset), _V(-1,0,0),17400.0, ph_3rd,250000, 240000);
	th_att_rot[3] = CreateThruster (_V(-RCSX,-ATTCOOR2+0.2,TRANZ-0.25+offset), _V( 1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[4] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V( 1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[5] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0),17400.0, ph_3rd,250000, 240000);

	AddExhaust (th_att_rot[2], 0.6, 0.078);
	AddExhaust (th_att_rot[3], 0.6, 0.078);
	AddExhaust (th_att_rot[4], 0.6, 0.078);
	AddExhaust (th_att_rot[5], 0.6, 0.078);
	CreateThrusterGroup (th_att_rot+2,   2, THGROUP_ATT_BANKLEFT);
	CreateThrusterGroup (th_att_rot+4, 2, THGROUP_ATT_BANKRIGHT);


	th_att_rot[6] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[7] = CreateThruster (_V(-RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[8] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), 17400.0, ph_3rd,250000, 240000);
	th_att_rot[9] = CreateThruster (_V(RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V(-1,0,0), 17400.0, ph_3rd,250000, 240000);

	AddExhaust (th_att_rot[6], 0.6, 0.078);
	AddExhaust (th_att_rot[7], 0.6, 0.078);
	AddExhaust (th_att_rot[8], 0.6, 0.078);
	AddExhaust (th_att_rot[9], 0.6, 0.078);

	CreateThrusterGroup (th_att_rot+6,   2, THGROUP_ATT_YAWLEFT);
	CreateThrusterGroup (th_att_rot+8, 2, THGROUP_ATT_YAWRIGHT);

	//
	// APS thrusters are only 320N (72 pounds) thrust
	//

	th_att_lin[0] = CreateThruster (_V(0,ATTCOOR2-0.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_3rd,250000, 240000);
	th_att_lin[1] = CreateThruster (_V(0,-ATTCOOR2+.15,TRANZ-.25+offset), _V(0,0,1), 320.0, ph_3rd,250000, 240000);
	AddExhaust (th_att_lin[0], 7, 0.15);
	AddExhaust (th_att_lin[1], 7, 0.15);

	thg_aps = CreateThrusterGroup (th_att_lin,   2, THGROUP_ATT_FORWARD);
}

// ==============================================================
// DLL entry point
// ==============================================================

BOOL WINAPI DllMain (HINSTANCE hModule,
					 DWORD ul_reason_for_call,
					 LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		SetupgParam(hModule);
		break;

	case DLL_PROCESS_DETACH:
		DeletegParam();
		break;
	}
	return TRUE;
}

void Saturn::LaunchCountdown(double simt)
{
	if (GetEngineLevel(ENGINE_MAIN) > 0 && MissionTime <= (-8.9)) {
		SetThrusterGroupLevel(thg_main, 0);
	}

	if (oapiGetTimeAcceleration() > 100)
		oapiSetTimeAcceleration(100);

	if (MissionTime >= 0) {
		DoLaunch(simt);
		return;
	}

	// Prelaunch tank venting between -3:00h and engine ignition
	// No clue if the venting start time is correct
	if (MissionTime < -10800 || MissionTime > -9) {
		DeactivatePrelaunchVenting();
	}
	else {
		ActivatePrelaunchVenting();
	}

	switch (StageState) {

	case 0:
		if (MissionTime >= -((4 * 60) + 10)) {
			//
			// Engine lights on.
			//

			for (int i = 0; i < 8; i++) {
				ENGIND[i] = true;
			}
			StageState++;
		}
		break;

	case 1:

		//
		// Reset time acceleration to normal at
		// 20 seconds, and reconnect the fuel to
		// the main engines.
		//

		if (MissionTime >= -20.0) {
			oapiSetTimeAcceleration (1);
			for (int i = 0; i < 5; i++) {
				SetThrusterResource(th_main[i], ph_1st);
			}
			StageState++;
		}
		break;

	case 2:

		//
		// Play the countdown sound at 10 seconds,
		//

		if (MissionTime >= -10.9) {
			if (!UseATC && Scount.isValid()) {
				Scount.play();
				Scount.done();
			}
			StageState++;
		}
		break;

	case 3:
		if (MissionTime >= -4.9) {
			StageState++;
		}
		break;

	case 4:
		//
		// Build up engine thrust. Slower at first so we don't
		// leave the ground before we should.
		//

		double thrst;

		if (MissionTime > (-2.0)) {
			thrst = 0.9 + (0.05 * (MissionTime + 2.0));

			//
			// Engine lights off.
			//

			for (int i = 0; i < 5; i++) {
				ENGIND[i] = false;
			}
		}
		else {
			thrst = (0.9 / 2.9) * (MissionTime + 4.9);
		}
		SetThrusterGroupLevel(thg_main, thrst);

		double amt = (thrst) * 0.1;
		JostleViewpoint(amt);

		AddForce(_V(0, 0, -10. * THRUST_FIRST_VAC), _V(0, 0, 0));
		break;
	}
}

void Saturn::GenericTimestepStage(double simt, double simdt)

{
	//
	// Do stage-specific processing.
	//

	bool deploy = false;

	switch (stage) {

	case PRELAUNCH_STAGE:
		LaunchCountdown(simt);
		break;

	case CM_STAGE:
		if (ELSAuto() && GetAtmPressure() > 38000 && !LandFail.u.CoverFail)
			deploy = true;

		if (ELSActive() && ApexCoverJettSwitch.GetState())
			deploy = true;

		if (deploy && PyrosArmed())
			StageEight(simt);
		else
			StageSeven(simt);
		break;

	case CM_ENTRY_STAGE:
		if (ELSAuto() && GetAtmPressure() > 37680 && !LandFail.u.DrogueFail)
			deploy = true;

		if (ELSActive() && ApexCoverJettSwitch.GetState())
			deploy = true;

		if (deploy && PyrosArmed())
			StageEight(simt);
		break;

	case CM_ENTRY_STAGE_TWO:
		if (ELSAuto() && GetAtmPressure() > 39000 && !LandFail.u.DrogueFail)
			deploy = true;

		if (ELSActive() && DrogueDeploySwitch.GetState()) 
			deploy = true;

		if (deploy && PyrosArmed()) {
			SetChuteStage1();
			LAUNCHIND[3] = true;
			SetStage(CM_ENTRY_STAGE_THREE);
		}
		break;

	//
	// We should really make the time taken to open the main chutes based on
	// the dynamic air pressure.
	//

	case CM_ENTRY_STAGE_THREE:
		if ((GetAtmPressure() > 66000 && !LandFail.u.MainFail) || (ELSActive() && MainDeploySwitch.GetState())) {
			SetChuteStage2();
			SetStage(CM_ENTRY_STAGE_FOUR);
			NextMissionEventTime = MissionTime + 2.5;
		}
		break;

	case CM_ENTRY_STAGE_FOUR:
		if (MissionTime >= NextMissionEventTime) {
			SetChuteStage3();
			SetStage(CM_ENTRY_STAGE_FIVE);
			NextMissionEventTime = MissionTime + 2.5;
		}
		break;

	case CM_ENTRY_STAGE_FIVE:
		if (MissionTime >= NextMissionEventTime) {
			SetChuteStage4();
			SetStage(CM_ENTRY_STAGE_SIX);
			LAUNCHIND[5] = true;
		}
		break;

	case CM_ENTRY_STAGE_SIX:
		if (GetAltitude() < 2) {
			SplashS.play(NOLOOP, 180);
			SplashS.done();
			SetSplashStage();
			SetStage(CM_ENTRY_STAGE_SEVEN);
		}
		break;

	case CM_ENTRY_STAGE_SEVEN:

		if (!Swater.isPlaying())
			Swater.play(LOOP,190);

		if (bToggleHatch){
			ToggelHatch2();
			bToggleHatch=false;
		}

		if (bRecovery){
			SetRecovery();
			bRecovery = false;
			soundlib.LoadSound(Swater, WATERLOOP_SOUND);
			soundlib.LoadMissionSound(PostSplashdownS, POSTSPLASHDOWN_SOUND, POSTSPLASHDOWN_SOUND);
			NextMissionEventTime = MissionTime + 10.0;
			SetStage(CM_RECOVERY_STAGE);
		}
		break;

	case CM_RECOVERY_STAGE:
		if (!PostSplashdownPlayed && MissionTime >= NextMissionEventTime) {
			PostSplashdownS.play();
			PostSplashdownS.done();
			PostSplashdownPlayed = true;
		}
		break;
	}

	//
	// Generic state for CM.
	//

	if (stage >= CM_STAGE) {
		if (RCSDumpActive()) {
			//
			// Clear toggle state.
			//
			CMPropDumpSwitch.ClearToggled();

			//
			// Start burning RCS propellant. This apparently uses all RCS thrusters other
			// than pitch up - Apollo Training: Sequential Events Control
			//
			// Note that before 61 seconds after launch, we should just dump the oxidiser
			// out of the RCS, and not burn them. There's not enough time to burn up all the
			// fuel before landing.
			//

			if (MissionTime < 61.0) {
			}
			else {
				SetThrusterGroupLevel(THGROUP_ATT_PITCHDOWN, 1.0);
				SetThrusterGroupLevel(THGROUP_ATT_YAWLEFT, 1.0);
				SetThrusterGroupLevel(THGROUP_ATT_YAWRIGHT, 1.0);
				SetThrusterGroupLevel(THGROUP_ATT_BANKLEFT, 1.0);
				SetThrusterGroupLevel(THGROUP_ATT_BANKRIGHT, 1.0);
			}
		}
		else if (CMPropDumpSwitch.Toggled()) {
			//
			// Clear toggled state so we don't end up here again.
			//
			CMPropDumpSwitch.ClearToggled();
			//
			// Stop burning RCS propellant.
			//
			SetThrusterGroupLevel(THGROUP_ATT_PITCHDOWN, 0.0);
			SetThrusterGroupLevel(THGROUP_ATT_YAWLEFT, 0.0);
			SetThrusterGroupLevel(THGROUP_ATT_YAWRIGHT, 0.0);
			SetThrusterGroupLevel(THGROUP_ATT_BANKLEFT, 0.0);
			SetThrusterGroupLevel(THGROUP_ATT_BANKRIGHT, 0.0);
		}
	}
}

void Saturn::GenericLoadStateSetup()

{
	//
	// Some switches only work in the CSM/LEM stage. Disable them
	// otherwise.
	//

	if (stage != CSM_LEM_STAGE) {
		OrbiterAttitudeToggle.SetActive(false);
	}
	else {
		OrbiterAttitudeToggle.SetActive(true);
	}

	//
	// Disable cabin fans.
	//

	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);

	//
	// We do our own countdown, so ignore the standard one.
	//

	if (!UseATC)
		soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);

	//
	// Load mission-based sound files. Some of these are just being
	// preloaded here for the CSM computer.
	//

	char MissionName[24];

	_snprintf(MissionName, 23, "Apollo%d", ApolloNo);
	soundlib.SetSoundLibMissionPath(MissionName);

	//
	// Set up options for prelaunch stage.
	//

	if (MissionTime < 0) {
		agc.SetBurnTime(0);

		//
		// Open the countdown sound file.
		//

		if (!UseATC) {
			soundlib.LoadMissionSound(Scount, LAUNCH_COUNT_10_SOUND, DEFAULT_LAUNCH_COUNT_SOUND);
			soundlib.LoadMissionSound(LaunchS, LAUNCH_SOUND, LAUNCH_SOUND);
			Scount.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
			LaunchS.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
		}
	}

	//
	// Only the CSM and LEM have translational thrusters, so disable the message
	// telling us that they're being switched in other stages.
	//

	if (stage != CSM_LEM_STAGE) {
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, FALSE);
	}
	else {
		soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
	}

	if (stage < PRELAUNCH_STAGE) {
		agc.BlankAll();
	}

	if (stage < LAUNCH_STAGE_TWO) {
		soundlib.LoadMissionSound(SShutS, SI_CUTOFF_SOUND, SISHUTDOWN_SOUND);
	}

	if (stage < LAUNCH_STAGE_TWO_TWR_JET) {
		soundlib.LoadSound(TowerJS, TOWERJET_SOUND);
	}

	if (stage < STAGE_ORBIT_SIVB) {

		//
		// We'll do our own radio playback during launch.
		//

		if (!UseATC)
			soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);

		soundlib.LoadMissionSound(SwindowS, WINDOW_SOUND, POST_TOWER_JET_SOUND);
		soundlib.LoadMissionSound(S2ShutS, SII_CUTOFF_SOUND, SIISHUTDOWN_SOUND);
		soundlib.LoadMissionSound(S4CutS, GO_FOR_ORBIT_SOUND, SIVBSHUTDOWN_SOUND);

		SwindowS.setFlags(SOUNDFLAG_COMMS);
		S4CutS.setFlags(SOUNDFLAG_COMMS);
		S2ShutS.setFlags(SOUNDFLAG_COMMS);
	}

	if (stage < CSM_LEM_STAGE) {
		soundlib.LoadSound(SepS, SEPMOTOR_SOUND, INTERNAL_ONLY);
	}

	if (stage <= CSM_LEM_STAGE) {
		soundlib.LoadMissionSound(SMJetS, SM_SEP_SOUND, DEFAULT_SM_SEP_SOUND);
	}

	if (stage == CM_RECOVERY_STAGE)
	{
		soundlib.LoadSound(Swater, WATERLOOP_SOUND);
		soundlib.LoadMissionSound(PostSplashdownS, POSTSPLASHDOWN_SOUND, POSTSPLASHDOWN_SOUND);
	}

	//
	// Load Apollo-13 specific sounds.
	//

	if (ApolloNo == 13) {
		if (!KranzPlayed)
			soundlib.LoadMissionSound(SKranz, A13_KRANZ, NULL, INTERNAL_ONLY);
		if (!CryoStir)
			soundlib.LoadMissionSound(SApollo13, A13_CRYO_STIR, NULL);
		if (!ApolloExploded)
			soundlib.LoadMissionSound(SExploded, A13_PROBLEM, NULL);

		if (stage <= CSM_LEM_STAGE) {
			soundlib.LoadMissionSound(SSMSepExploded, A13_SM_SEP_SOUND, NULL);
		}

		SKranz.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
		SApollo13.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
		SExploded.setFlags(SOUNDFLAG_1XORLESS|SOUNDFLAG_COMMS);
	}

	//
	// Turn off the timer display on launch.
	//
	
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	//
	// Turn off docking sound
	//
	
	soundlib.SoundOptionOnOff(PLAYDOCKINGSOUND, FALSE);

	//
	// Initialize the IU
	//

	iu.SetMissionInfo(TLICapableBooster, Crewed, Realism, &th_main[0], &ph_3rd, &thg_aps, SIVBBurnStart, SIVBApogee); 
}

bool Saturn::CheckForLaunchShutdown()

{
	//
	// Shut down the engine when we're close to the desired apogee and perigee.
	//

	double apogee, perigee;

	OBJHANDLE ref = GetGravityRef();
	GetElements(elemSaturn1B, refSaturn1B);
	GetApDist(apogee);
	GetPeDist(perigee);
	apogee = (apogee - oapiGetSize(ref)) / 1000.;
	perigee = (perigee - oapiGetSize(ref)) / 1000.;

	if ((refSaturn1B - refPREV) >= 0 &&
		(stage == LAUNCH_STAGE_SIVB) &&
		((elemSaturn1B.e > elemPREV.e && elemSaturn1B.e < 0.03) ||
		(apogee >= agc.GetDesiredApogee() && perigee >= agc.GetDesiredPerigee() - 0.8)) &&
		(SCswitch||CMCswitch))
	{
		if (GetEngineLevel(ENGINE_MAIN) > 0){
			SetEngineLevel(ENGINE_MAIN,0);
			if (oapiGetTimeAcceleration() > 1.0)
				oapiSetTimeAcceleration(1.0);

			S4CutS.play(NOLOOP, 255);
			S4CutS.done();

			ActivateNavmode(NAVMODE_KILLROT);

			agc.LaunchShutdown();

			//
			// Checklist actions
			//

			// Unlatch FC valves
			FCReacsValvesSwitch.SwitchTo(TOGGLESWITCH_UP);
			CautionWarningModeSwitch.SwitchTo(THREEPOSSWITCH_UP);
			
			// Turn on cyro fans
			H2Fan1Switch.SwitchTo(THREEPOSSWITCH_UP);
			H2Fan2Switch.SwitchTo(THREEPOSSWITCH_UP);
			O2Fan1Switch.SwitchTo(THREEPOSSWITCH_UP);
			O2Fan2Switch.SwitchTo(THREEPOSSWITCH_UP);

			// ECS flight configuration
			EcsRadiatorsFlowContPwrSwitch.SwitchTo(THREEPOSSWITCH_UP);
			GlycolToRadiatorsLever.SwitchTo(TOGGLESWITCH_UP);
			EcsRadiatorsHeaterPrimSwitch.SwitchTo(THREEPOSSWITCH_UP);
			PotH2oHtrSwitch.SwitchTo(THREEPOSSWITCH_UP);
			GlycolEvapTempInSwitch.SwitchTo(TOGGLESWITCH_UP);

			// Turn on cabin fans
			CabinFan1Switch.SwitchTo(TOGGLESWITCH_UP);
			CabinFan2Switch.SwitchTo(TOGGLESWITCH_UP);

			// Avoid O2 flow high alarm 
			if (!Realism)
				cws.SetInhibitNextMasterAlarm(true);
			
			// Extend docking probe
			DockingProbeExtdRelSwitch.SwitchTo(THREEPOSSWITCH_UP);
			DockingProbeExtdRelSwitch.SwitchTo(THREEPOSSWITCH_DOWN);			
		}
		return true;
	}

	elemPREV = elemSaturn1B;
	refPREV = refSaturn1B;

	return false;
}

void Saturn::SetGenericStageState()

{
	switch(stage) {

	case CM_STAGE:
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_TWO:
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_THREE:
		SetChuteStage1();
		break;

	case CM_ENTRY_STAGE_FOUR:
		SetChuteStage2();
		break;

	case CM_ENTRY_STAGE_FIVE:
		SetChuteStage3();
		break;

	case CM_ENTRY_STAGE_SIX:
		SetChuteStage4();
		break;

	case CM_ENTRY_STAGE_SEVEN:
		SetSplashStage();
		break;

	case CM_RECOVERY_STAGE:
		SetRecovery();
		break;

	case CM_ENTRY_STAGE:
		SetReentryStage();
		break;

	case CSM_ABORT_STAGE:
		SetAbortStage();
		break;
	}

	CheckSMSystemsState();
}

//
// Do we have a CSM on this launcher?
//

bool Saturn::SaturnHasCSM()

{

	//
	// LM1 has a nosecap rather than a CSM.
	//

	if (SIVBPayload != PAYLOAD_LM1)
		return true;

	return false;
}

//
// Set thruster state on or off. This should really turn off all but roll
// thrusters until we're in orbit.
//
// Also, they should have their own fuel tank rather than use the main SIVB
// fuel!
//

void Saturn::SetSIVBThrusters(bool active)

{
	if (active) {
		SetThrusterResource(th_att_rot[0], ph_3rd);
		SetThrusterResource(th_att_rot[1], ph_3rd);
		SetThrusterResource(th_att_rot[2], ph_3rd);
		SetThrusterResource(th_att_rot[3], ph_3rd);
		SetThrusterResource(th_att_rot[4], ph_3rd);
		SetThrusterResource(th_att_rot[5], ph_3rd);
		SetThrusterResource(th_att_rot[7], ph_3rd);
		SetThrusterResource(th_att_rot[6], ph_3rd);
		SetThrusterResource(th_att_rot[8], ph_3rd);
		SetThrusterResource(th_att_rot[9], ph_3rd);
	}
	else{
		SetThrusterResource(th_att_rot[0], NULL);
		SetThrusterResource(th_att_rot[1], NULL);
		SetThrusterResource(th_att_rot[2], NULL);
		SetThrusterResource(th_att_rot[3], NULL);
		SetThrusterResource(th_att_rot[4], NULL);
		SetThrusterResource(th_att_rot[5], NULL);
		SetThrusterResource(th_att_rot[6], NULL);
		SetThrusterResource(th_att_rot[7], NULL);
		SetThrusterResource(th_att_rot[8], NULL);
		SetThrusterResource(th_att_rot[9], NULL);
	}
}

//
// Thrusters can't turn more than five degrees from straight ahead. Check for that,
// and alarm if they try to go beyond it.
//

void Saturn::LimitSetThrusterDir (THRUSTER_HANDLE th, const VECTOR3 &dir)

{
	VECTOR3 realdir = dir;
	bool alarm = false;

	if (realdir.x > 0.1) {
		realdir.x = 0.1;
		alarm = true;
	}

	if (realdir.x < (-0.1)) {
		realdir.x = -0.1;
		alarm = true;
	}

	if (realdir.y > 0.1) {
		realdir.y = 0.1;
		alarm = true;
	}

	if (realdir.y < (-0.1)) {
		realdir.y = (-0.1);
		alarm = true;
	}

//
//	I don't think this is historically correct, so I disabled it
//    
/*	if (alarm && autopilot) {
		MasterAlarm();
	}
*/
	SetThrusterDir(th, realdir);
}

void Saturn::LoadDefaultSounds()

{
	//
	// Remember that you can't load mission sounds at this point as the
	// mission path hasn't been set up!
	//

	soundlib.LoadSound(SeparationS, SEPARATION_SOUND);
	soundlib.LoadSound(Sctdw, COUNT10_SOUND);
	soundlib.LoadSound(Sclick, CLICK_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Bclick, BUTTON_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Gclick, GUARD_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Psound, PROBE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(CabinFans, CMCABIN_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SMasterAlarm, MASTERALARM_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SplashS, SPLASH_SOUND);
	soundlib.LoadSound(StageS, "Stagesep.wav");
	soundlib.LoadSound(CrashBumpS, "Crash.wav");
	soundlib.LoadSound(SDockingCapture, DOCKINGCAPTURE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SDockingLatch, DOCKINGLATCH_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SDockingExtend, DOCKINGEXTEND_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(SUndock, UNDOCK_SOUND, INTERNAL_ONLY);

	Sctdw.setFlags(SOUNDFLAG_1XONLY|SOUNDFLAG_COMMS);
}

void Saturn::StageOrbitSIVB(double simt, double simdt)

{
	//
	// Post-shutdown, pre-TLI code goes here.
	//
	AttitudeLaunchSIVB();

	//
	// Enable random ATC chatter.
	//

	if (!UseATC)
		soundlib.SoundOptionOnOff(PLAYRADIOATC, TRUE);

	//
	// For unmanned launches, seperate the CSM on timer.
	//

	if (!Crewed && CSMSepSet && (MissionTime >= CSMSepTime - 10.)) {
		SlowIfDesired();
	}

	if (!Crewed && CSMSepSet && (MissionTime >= CSMSepTime)) {
		SlowIfDesired();
		bManualSeparate = true;
		CSMSepSet = false;
	}

	if (CsmLvSepSwitch.GetState()) {
		bManualSeparate = true;
	}

	if ((bManualSeparate || bAbort))
	{
		if (SECSLogicActive() && PyrosArmed()) {
			bManualSeparate = false;
			SeparateStage(stage);
			SetStage(CSM_LEM_STAGE);
			soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, TRUE);
			if (bAbort) {
				SPSswitch.SetState(true);
				ABORT_IND = true;
				SetThrusterGroupLevel(thg_main, 1.0);
				bAbort = false;
				autopilot= false;
				StartAbort();
			}
			return;
		}
		else {
			bManualSeparate = false;
			bAbort = false;
		}
	}
}

void Saturn::StartAbort()

{
	//
	// Event timer resets to zero on abort.
	//

	EventTimerDisplay.Reset();
}

void Saturn::SlowIfDesired()

{
	if (!Crewed && AutoSlow && (oapiGetTimeAcceleration() > 1.0)) {
		oapiSetTimeAcceleration(1.0);
	}
}

//
// Allows Launch Vehicle Quantities to be accessed outside of class
//
void Saturn::GetLVTankQuantities(LVTankQuantities &LVq)
{
	//
	// Clear to defaults.
	//
	LVq.SICQuantity = 0.0;  //current quantities
	LVq.SIIQuantity = 0.0;
	LVq.SIVBOxQuantity = 0.0;
	LVq.SIVBFuelQuantity = 0.0;
	LVq.SICFuelMass = SI_FuelMass;  //Initial amounts
	LVq.SIIFuelMass = SII_FuelMass;
	LVq.S4BOxMass = S4B_FuelMass;
	LVq.S4BFuelMass = S4B_FuelMass;

	
	//
	// No tanks if we've seperated from the different stages of LV
	//

	if (stage >= CSM_LEM_STAGE) {
		return;
	}
	else if (stage >= LAUNCH_STAGE_SIVB) {
		if (!ph_3rd){
			return;
		}
		else if(ph_3rd){
			//Someday we'll need to simulate SIVB Ox and Fuel Tanks seperately for true Guage support for now it's just done with an correction value that roughly equates to 94% fuel burned for 100% ox burned
			LVq.SIVBOxQuantity = GetPropellantMass(ph_3rd);  
			LVq.SIVBFuelQuantity = (GetPropellantMass(ph_3rd) + ((.0638 * S4B_FuelMass) * (1 - (GetPropellantMass(ph_3rd) / S4B_FuelMass))));
			return;
		}
	}
	else if (stage >= LAUNCH_STAGE_TWO) {
		LVq.SIVBOxQuantity = S4B_FuelMass;
		LVq.SIVBFuelQuantity = S4B_FuelMass;
		if (!ph_2nd){
			return;
		}
		else if(ph_2nd){
			LVq.SIIQuantity = GetPropellantMass(ph_2nd);
			return;
		}
	}
	else if (stage >= LAUNCH_STAGE_ONE) {
		LVq.SIVBOxQuantity = S4B_FuelMass;
		LVq.SIVBFuelQuantity = S4B_FuelMass;
		LVq.SIIQuantity = SII_FuelMass;
		if (!ph_1st){
			return;
		}
		else if(ph_1st){
			LVq.SICQuantity = GetPropellantMass(ph_1st);
			return;
		}
	}
	else {
		LVq.SICQuantity = SI_FuelMass;
		LVq.SIIQuantity = SII_FuelMass;
		LVq.SIVBOxQuantity = S4B_FuelMass;
		LVq.SIVBFuelQuantity = S4B_FuelMass;
		return;
	}
}

//
// Get the J2 ISP from the mixture ratio and calculate the thrust adjustment.
//

#define MR_STATS 5

static double MixtureRatios[MR_STATS] = {6.0, 5.5, 5.0, 4.3, 4.0 };
static double MRISP[MR_STATS] = { 416*G, 418*G, 421*G, 427*G, 432*G };
static double MRThrust[MR_STATS] = { 1.1, 1.0, .898, .7391, .7 };

double Saturn::GetJ2ISP(double ratio)

{
	double isp = 421*G;

	// From Usenet:
	// It had roughly three stops. 178,000 lbs at 425s Isp and an O/F of 4.5,
	// 207,000 lbs at 421s Isp and an O/F of 5.0, and 230,500 lbs at 418s Isp
	// and an O/F of 5.5.

	for (int i = 0; i < MR_STATS; i++) {
		if (ratio >= MixtureRatios[i]) {
			double delta = (ratio - MixtureRatios[i]) / (MixtureRatios[i - 1] - MixtureRatios[i]);

			isp = MRISP[i] + ((MRISP[i - 1] - MRISP[i]) * delta);
			ThrustAdjust = MRThrust[i] + ((MRThrust[i - 1] - MRThrust[i]) * delta);

			return isp;
		}
	}

	return isp;
}

//
// Set up random failures if required.
//

void Saturn::SetRandomFailures()

{
	//
	// I'm not sure how reliable the random number generator is. We may want to get a
	// number from 0-1000 and then see if that's less than some threshold, rather than
	// checking for the bottom bits being zero.
	//

	//
	// Set up launch failures.
	//

	if (!LaunchFail.u.Init) {
		LaunchFail.u.Init = 1;
		if (!(random() & 15)) {
			LaunchFail.u.EarlySICenterCutoff = 1;
			FirstStageCentreShutdownTime = 20.0 + ((double) (random() & 1023) / 10.0);
		}
		if (!(random() & 15)) {
			LaunchFail.u.EarlySIICenterCutoff = 1;
			SecondStageCentreShutdownTime = 200.0 + ((double) (random() & 2047) / 10.0);
		}
		if (!(random() & 127)) {
			LaunchFail.u.LETAutoJetFail = 1;
		}
		if (!(random() & 63)) {
			LaunchFail.u.SIIAutoSepFail = 1;
		}
	}

	//
	// Set up landing failures.
	//

	if (!LandFail.u.Init) {
		LandFail.u.Init = 1;
		if (!(random() & 127)) {
			LandFail.u.CoverFail = 1;
		}
		if (!(random() & 127)) {
			LandFail.u.DrogueFail = 1;
		}
		if (!(random() & 127)) {
			LandFail.u.MainFail = 1;
		}
	}

	//
	// Set up switch failures.
	//

	if (!SwitchFail.u.Init) {
		SwitchFail.u.Init = 1;
		if (!(random() & 127)) {
			SwitchFail.u.TowerJett1Fail = 1;
		}
		else if (!(random() & 127)) {
			SwitchFail.u.TowerJett2Fail = 1;
		}
		if (!(random() & 127)) {
			SwitchFail.u.SMJett1Fail = 1;
		}
		else if (!(random() & 127)) {
			SwitchFail.u.SMJett2Fail = 1;
		}

		//
		// Random CWS light failures.
		//
		if (!(random() & 15)) 
		{
			int i, n = (random() & 7) + 1;

			for (i = 0; i < n; i++) {
				cws.FailLight(random() & 63, true);
			}
		}
	}

}

//
// Most of this calculation code is lifted from the Soyuz guidance MFD.
//

extern double AbsOfVector(const VECTOR3 &Vec);

double Saturn::CalculateApogeeTime()

{
	OBJHANDLE hSetGbody;
	double GbodyMass, GbodySize;
	double p, v, R, RDotV, Mu_Planet, J2000, E, Me, T, tsp;
	double TtPeri, TtApo;
	double OrbitApo;
	VECTOR3 RelPosition, RelVelocity;
	ELEMENTS Elements;

	// Planet parameters
	hSetGbody = GetApDist(OrbitApo);
	GbodyMass = oapiGetMass(hSetGbody);
	GbodySize = oapiGetSize(hSetGbody) / 1000.;
	Mu_Planet = GK * GbodyMass;

	// Get eccentricity and orbital radius
	GetElements(Elements, J2000);
	GetRelativePos(hSetGbody, RelPosition);
	GetRelativeVel(hSetGbody, RelVelocity);

	R = AbsOfVector(RelPosition) / 1000.;

	// Calculate semi-latus rectum and true anomaly
	p = Elements.a / 1000. * (1. - Elements.e * Elements.e);
	v = acos((1. / Elements.e) * (p / R - 1.));

	RDotV = dotp(RelVelocity, RelPosition);
	if (RDotV < 0)
	{
		v = 2. * PI - v;
	}

	// Determine the time since periapsis
	//   - Eccentric anomaly
	E = 2. * atan(sqrt((1. - Elements.e) / (1. + Elements.e)) * tan(v / 2.));
	//   - Mean anomaly
	Me = E - Elements.e * sin(E);
	//   - Period of orbit
	T = 2. * PI * sqrt((Elements.a * Elements.a * Elements.a / 1e9) / Mu_Planet);

	// Time since periapsis is
	tsp = Me / (2.* PI) * T;

	// Time to next periapsis & apoapsis
	TtPeri = T - tsp;
	if (RDotV < 0) {
		TtPeri = -1. * tsp;
	}

	if (TtPeri > (T / 2.)) {
		TtApo = fabs((T / 2.) - TtPeri);
	}
	else {
		TtApo = fabs(TtPeri + (T / 2.));
	}

	return TtApo;
}
