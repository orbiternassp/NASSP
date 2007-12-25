/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn CSM panel code

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
  *	Revision 1.226  2007/12/21 09:38:43  jasonims
  *	EMS Implementation Step 5 - jasonims :   SCROLL: complete:  possible inaccuracy in velocity integration.   RSI: complete:  accurate readings, but unsure of authenticity of method, (lack of documentation found).    THRESHOLD CIRCUITRY:  complete: seems quite accurate.   CORRIDOR VERIFICATION: complete: needs testing, but appears to be correct.    RANGE:  working but inaccurate:  probable inaccuracy in range integration....unknown cause.   EMS State is saved as well.    Testing must insue.
  *	
  *	Revision 1.225  2007/12/05 19:49:55  tschachim
  *	Fixed max. 70A for the inverter power cbs.
  *	
  *	Revision 1.224  2007/12/05 19:23:29  jasonims
  *	EMS Implementation Step 4 - jasonims :   RSI is set up to rotate, but no actual controlling of it is done.
  *	
  *	Revision 1.223  2007/12/05 07:13:12  jasonims
  *	EMS Implementation Step 3b - jasonims :   EMS Scroll disappearance bug fixed.  No further implementation.
  *	
  *	Revision 1.222  2007/12/04 20:26:36  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.221  2007/11/29 21:53:20  movieman523
  *	Generising the Volt meters.
  *	
  *	Revision 1.220  2007/11/29 21:28:44  movieman523
  *	Electrical meters now use a common base class which handles the rendering.
  *	
  *	Revision 1.219  2007/11/29 04:56:09  movieman523
  *	Made the System Test meter work (though currently it's connected to the rotary switch, which isn't connected to anything, so just displays 0V).
  *	
  *	Revision 1.218  2007/11/29 01:43:05  flydba
  *	New bitmaps and working rotaries added for the left side wall panels (and a new altimeter for the left rendezvous window).
  *	
  *	Revision 1.217  2007/11/27 02:56:41  jasonims
  *	EMS Implementation Step 3 - jasonims :   EMS Scroll is functional and plots correctly, however .05G circuitry does not work yet and is commented out.  Manual  operation does work though.  Verification needed.
  *	
  *	Revision 1.216  2007/11/26 17:59:06  movieman523
  *	Assorted tidying up of state variable structures.
  *	
  *	Revision 1.215  2007/11/25 09:07:24  jasonims
  *	EMS Implementation Step 2 - jasonims :   EMS Scroll can slew, and some functionality set up for EMS.
  *	
  *	Revision 1.214  2007/11/24 21:28:46  jasonims
  *	EMS Implementation Step 1 - jasonims :   EMSdVSet Switch now works, preliminary EMS Scroll work being done.
  *	
  *	Revision 1.213  2007/11/17 02:49:54  lassombra
  *	Major overhaul of lower panel.  Nothing moved visibly, but moved all panel area definitions to separate methods that allowed for split panel work.  All items to the left of panel 101 should be in Left, while panel 101 (and all items in width of 101) should be in LeftCenter.  Everything between 101 and the DSKY (including all 4 MFD's) should be in the Center.  The DSKY (and anything in it's width) should be in RightCenter.  Anything to the right of the DSKY should be in Right.
  *	
  *	Revision 1.212  2007/10/18 00:23:23  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.211  2007/09/07 17:43:52  tschachim
  *	(Re)moved obsolete bitmaps.
  *	
  *	Revision 1.210  2007/08/20 20:57:19  tschachim
  *	(Re)moved obsolete bitmap.
  *	
  *	Revision 1.209  2007/08/13 16:06:19  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.208  2007/07/17 14:33:09  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.207  2007/06/06 15:02:18  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.206  2007/04/29 12:36:43  tschachim
  *	Panel bugfixes.
  *	
  *	Revision 1.205  2007/04/25 18:48:11  tschachim
  *	EMS dV functions.
  *	
  *	Revision 1.204  2007/03/24 03:19:04  flydba
  *	LEB and new side panels added.
  *	
  *	Revision 1.203  2007/02/18 01:35:30  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.202  2007/02/06 18:30:18  tschachim
  *	Bugfixes docking probe, CSM/LM separation. The ASTP stuff still needs fixing though.
  *	
  *	Revision 1.201  2007/01/22 15:48:16  tschachim
  *	SPS Thrust Vector Control, RHC power supply, THC clockwise switch, bugfixes.
  *	
  *	Revision 1.200  2007/01/14 13:02:43  dseagrav
  *	CM AC bus feed reworked. Inverter efficiency now varies, AC busses are 3-phase all the way to the inverter, inverter switching logic implemented to match the CM motor-switch lockouts. Original AC bus feeds deleted. Inverter overload detection enabled and correct.
  *	
  *	Revision 1.199  2007/01/06 23:08:32  dseagrav
  *	More telecom stuff. A lot of the S-band signal path exists now, albeit just to consume electricity.
  *	
  *	Revision 1.198  2007/01/06 07:34:35  dseagrav
  *	FLIGHT bus added, uptelemetry now draws power, UPTLM switches on MDC now operate
  *	
  *	Revision 1.197  2007/01/06 04:44:49  dseagrav
  *	Corrected CREW ALARM command behavior, PCM downtelemetry generator now draws power
  *	
  *	Revision 1.196  2007/01/02 01:38:25  dseagrav
  *	Digital uplink and associated stuff.
  *	
  *	Revision 1.195  2006/12/19 15:56:10  tschachim
  *	ECS test stuff, bugfixes.
  *	
  *	Revision 1.194  2006/12/10 00:47:27  dseagrav
  *	Optics code moved to class, now draws power, most switches work, manual-resolved mode not implemented
  *	
  *	Revision 1.193  2006/12/07 18:52:43  tschachim
  *	New LC34, Bugfixes.
  *	
  *	Revision 1.192  2006/11/24 22:42:44  dseagrav
  *	Enable changing bits in AGC channel 33, enable LEB optics switch, enable tracker switch as optics status debug switch.
  *	
  *	Revision 1.191  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.190  2006/09/23 01:57:13  flydba
  *	Final graphics update completed.
  *	
  *	Revision 1.189  2006/08/25 05:44:36  jasonims
  *	Passive Optics-orbiter interface is commited.  SextTrunion, TeleTrunion, and OpticsShaft are values that need to be updated in order to produce a visual change of view.
  *	
  *	Revision 1.188  2006/08/25 05:16:51  jasonims
  *	Passive Optics-orbiter interface is commited.  SextTrunion, TeleTrunion, and OpticsShaft are values that need to be updated in order to produce a visual change of view.
  *	
  *	Revision 1.187  2006/08/24 15:03:20  jasonims
  *	Corrected Optics base value to 32.524 degrees.
  *	
  *	Revision 1.186  2006/08/23 03:57:16  jasonims
  *	*** empty log message ***
  *	
  *	Revision 1.185  2006/08/09 00:45:21  flydba
  *	Panel 101 added and some corrections done.
  *	
  *	Revision 1.184  2006/08/08 20:23:50  jasonims
  *	More Optics stuff and changed the Aperture settings for interior views.
  *	
  *	Revision 1.183  2006/08/03 20:35:54  flydba
  *	Bitmaps updated, changes on some panel areas done.
  *	
  *	Revision 1.182  2006/07/31 15:58:31  jasonims
  *	*** empty log message ***
  *	
  *	Revision 1.181  2006/07/31 00:05:59  jasonims
  *	Set up Optics panels...
  *	
  *	Revision 1.180  2006/07/27 20:40:06  movieman523
  *	We can now draw power from the SIVb in the Apollo to Venus scenario.
  *	
  *	Revision 1.179  2006/07/21 23:04:35  movieman523
  *	Added Saturn 1b engine lights on panel and beginnings of electrical connector work (couldn't disentangle the changes). Be sure to get the config file for the SIVb as well.
  *	
  *	Revision 1.178  2006/07/16 17:43:07  flydba
  *	Switches and rotary on panel 13 (ORDEAL) now work.
  *	
  *	Revision 1.177  2006/06/30 11:53:50  tschachim
  *	Bugfix InstrumentLightingNonESSCircuitBraker and NonessBusSwitch.
  *	
  *	Revision 1.176  2006/06/21 13:40:15  tschachim
  *	Disabled testing code of the ECS water-glycol cooling
  *	
  *	Revision 1.175  2006/06/17 18:18:00  tschachim
  *	Bugfixes SCS automatic modes,
  *	Changed quickstart separation key to J.
  *	
  *	Revision 1.174  2006/06/11 22:05:41  movieman523
  *	Saturn 1b doesn't have SII Sep light.
  *	
  *	Revision 1.173  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.172  2006/06/08 15:30:18  tschachim
  *	Fixed ASCP and some default switch positions.
  *	
  *	Revision 1.171  2006/06/07 09:53:20  tschachim
  *	Improved ASCP and GDC align button, added cabin closeout sound, bugfixes.
  *	
  *	Revision 1.170  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.169  2006/05/30 14:40:21  tschachim
  *	Fixed fuel cell - dc bus connectivity, added battery charger
  *	
  *	Revision 1.168  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.167  2006/05/17 18:42:35  movieman523
  *	Partial fix for loading sound volume from scenario.
  *	
  *	Revision 1.166  2006/04/25 13:57:36  tschachim
  *	Removed GetXXXSwitchState.
  *	
  *	Revision 1.165  2006/04/24 21:06:33  quetalsi
  *	Buxfix in EXT RNDZ LTS switch
  *	
  *	Revision 1.164  2006/04/23 07:14:03  dseagrav
  *	Holding mouse key down causes ASCP to advance until mouse key is released.
  *	
  *	Revision 1.163  2006/04/18 13:49:10  tschachim
  *	Bugfix GPFPI.
  *	
  *	Revision 1.162  2006/04/17 19:12:27  movieman523
  *	Removed some unused switches.
  *	
  *	Revision 1.161  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  *	Revision 1.160  2006/04/17 15:16:16  movieman523
  *	Beginnings of checklist code, added support for flashing borders around control panel switches and updated a portion of the Saturn panel switches appropriately.
  *	
  *	Revision 1.159  2006/04/06 11:18:08  tschachim
  *	Bugfix GPFI meter.
  *	
  *	Revision 1.158  2006/03/29 16:34:00  jasonims
  *	GPFPI Meters added supporting proper LV Fuel Quantities and SPS Gimbel Position display.  LV Tank Quantities now accessable from outside Saturn class.
  *	
  *	Revision 1.157  2006/03/27 19:22:44  quetalsi
  *	Bugfix RCS PRPLNT switches and wired to brakers.
  *	
  *	Revision 1.156  2006/03/19 17:06:13  dseagrav
  *	Fixed mistake with RCS TRNFR, it's a 3-position switch and is ignored for now.
  *	
  *	Revision 1.155  2006/03/18 22:55:55  dseagrav
  *	Added more RJEC functionality.
  *	
  *	Revision 1.154  2006/03/14 02:48:57  dseagrav
  *	Added ECA object, moved FDAI redraw stuff into ECA to clean up FDAI redraw mess.
  *	
  *	Revision 1.153  2006/03/12 01:13:29  dseagrav
  *	Added lots of SCS items and FDAI stuff.
  *	
  *	Revision 1.152  2006/03/09 20:40:22  quetalsi
  *	Added Battery Relay Bus. Wired Inverter 1/2/3, EPS Sensor Unit DC A/B, EPS Sensor Unit AC 1/2 and Bat Rly Bus BAT A/B brakers.
  *	
  *	Revision 1.151  2006/03/09 00:27:25  movieman523
  *	Added SPS fuel quantity gauges
  *	
  *	Revision 1.150  2006/03/05 00:49:48  movieman523
  *	Wired up Auto RCS Select switches to bus A and B.
  *	
  *	Revision 1.149  2006/03/04 22:50:52  dseagrav
  *	Added FDAI RATE logic, SPS TVC travel limited to 5.5 degrees plus or minus, added check for nonexistent joystick selection in DirectInput code. I forgot to date most of these.
  *	
  *	Revision 1.148  2006/02/28 20:40:32  quetalsi
  *	Bugfix and added CWS FC BUS DISCONNECT. Reset DC switches now work.
  *	
  *	Revision 1.147  2006/02/28 00:03:58  quetalsi
  *	MainBus A & B Switches and Talkbacks woks and wired.
  *	
  *	Revision 1.146  2006/02/23 15:51:16  tschachim
  *	Restored changes lost in last version.
  *	
  *	Revision 1.145  2006/02/23 14:13:49  dseagrav
  *	Split CM RCS into two systems, moved CM RCS thrusters (close to) proper positions, eliminated extraneous thrusters, set ISP and thrust values to match documentation, connected CM RCS to AGC IO channels 5 and 6 per DAP documentation, changes 20060221-20060223.
  *	
  *	Revision 1.144  2006/02/22 20:14:46  quetalsi
  *	C&W  AC_BUS1/2 light and AC RESET SWITCH now woks.
  *	
  *	Revision 1.143  2006/02/21 23:20:40  quetalsi
  *	Bugfix in EVENT TIMER RESET/DOWN switch.
  *	
  *	Revision 1.142  2006/02/13 21:40:28  tschachim
  *	CMCModeSwitch up by default.
  *	
  *	Revision 1.141  2006/02/02 18:52:35  tschachim
  *	Improved Accel G meter.
  *	
  *	Revision 1.140  2006/02/01 18:29:41  tschachim
  *	Pyros and secs logic cb's, FDAI off flag.
  *	
  *	Revision 1.139  2006/01/14 20:03:35  movieman523
  *	Fixed some switch bugs.
  *	
  *	Revision 1.138  2006/01/14 18:57:49  movieman523
  *	First stages of pyro and SECS simulation.
  *	
  *	Revision 1.137  2006/01/14 12:34:49  flydba
  *	New panel added (325/326) for cabin press control.
  *	
  *	Revision 1.136  2006/01/14 00:54:35  movieman523
  *	Hacky wiring of sequential systems and pyro arm switches.
  *	
  *	Revision 1.135  2006/01/12 00:09:07  movieman523
  *	Few fixes: Program 40 now starts and stops the SPS engine, but doesn't orient the CSM first.
  *	
  *	Revision 1.134  2006/01/11 22:34:20  movieman523
  *	Wired Virtual AGC to RCS and SPS, and added some CMC guidance control switches.
  *	
  *	Revision 1.133  2006/01/11 02:59:43  movieman523
  *	Valve talkbacks now check the valve state directlry. This means they barberpole on SM sep and can't then be changed.
  *	
  *	Revision 1.132  2006/01/11 02:16:25  movieman523
  *	Added RCS propellant quantity gauge.
  *	
  *	Revision 1.131  2006/01/10 21:09:30  movieman523
  *	Improved AoA/thrust meter.
  *	
  *	Revision 1.130  2006/01/10 20:49:50  movieman523
  *	Added CM RCS propellant dump and revised thrust display.
  *	
  *	Revision 1.129  2006/01/10 19:34:45  movieman523
  *	Fixed AC bus switches and added ELS Logic/Auto support.
  *	
  *	Revision 1.128  2006/01/08 21:43:34  movieman523
  *	First phase of implementing inverters, and stopped PanelSDK trying to delete objects which weren't allocated with new().
  *	
  *	Revision 1.127  2006/01/08 19:04:30  movieman523
  *	Wired up AC bus switches in a quick and hacky manner.
  *	
  *	Revision 1.126  2006/01/08 17:50:38  movieman523
  *	Wired up electrical meter switches other than battery charger.
  *	
  *	Revision 1.125  2006/01/08 17:01:42  flydba
  *	Switches added on main panel 3.
  *	
  *	Revision 1.124  2006/01/08 04:00:24  movieman523
  *	Added first two engineering cameras.
  *	
  *	Revision 1.123  2006/01/07 18:14:37  tschachim
  *	Bugfix
  *	
  *	Revision 1.122  2006/01/07 03:28:28  movieman523
  *	Removed a lot of unused switches and wired up the FDAI power switch.
  *	
  *	Revision 1.121  2006/01/07 03:16:38  flydba
  *	Switches added on main panel 2.
  *	
  *	Revision 1.120  2006/01/07 00:43:58  movieman523
  *	Added non-essential buses, though there's nothing connected to them at the moment.
  *	
  *	Revision 1.119  2006/01/06 22:55:53  movieman523
  *	Fixed SM seperation and cut off fuel cell power when it happens.
  *	
  *	Revision 1.118  2006/01/06 21:40:15  movieman523
  *	Quick hack for damping electrical meters.
  *	
  *	Revision 1.117  2006/01/06 20:37:18  movieman523
  *	Made the voltage and current meters work. Currently hard-coded to main bus A and AC bus 1.
  *	
  *	Revision 1.116  2006/01/06 19:46:16  flydba
  *	Switches added on main panel 1.
  *	
  *	Revision 1.115  2006/01/06 02:06:52  flydba
  *	Some changes done on the main panel.
  *	
  *	Revision 1.114  2006/01/05 12:06:52  tschachim
  *	New dockingprobe handling.
  *	
  *	Revision 1.113  2006/01/05 04:54:30  flydba
  *	All clickable areas of the right & left hand side panels added now!
  *
  *	Revision 1.112  2006/01/04 01:57:39  flydba
  *	Switches on panel 8 now set.
  *
  *	Revision 1.111  2006/01/03 17:53:08  flydba
  *	Switches on panel 9 are set up now.
  *
  *	Revision 1.110  2006/01/03 13:02:31  flydba
  *	Some changes done on panel 15.
  *
  *	Revision 1.109  2006/01/03 03:59:48  flydba
  *	Some changes done on panel 15.
  *
  *	Revision 1.108  2005/12/19 17:14:20  tschachim
  *	Bugfixes, changes for the prelaunch checklist
  *
  *	Revision 1.107  2005/12/02 20:44:35  movieman523
  *	Wired up buses and batteries directly rather than through PowerSource objects.
  *
  *	Revision 1.106  2005/11/24 01:07:54  movieman523
  *	Removed code for panel lights which were being set incorrectly. Plus a bit of tidying.
  *
  *	Revision 1.105  2005/11/20 21:46:31  movieman523
  *	Added initial volume control support.
  *
  *	Revision 1.104  2005/11/19 01:06:01  flydba
  *	Switches set on the lower equipment bay.
  *
  *	Revision 1.103  2005/11/18 22:11:22  movieman523
  *	Added seperate heat and electrical power usage for boilers. Revised cabin fan code.
  *
  *	Revision 1.102  2005/11/18 04:45:28  flydba
  *	Window cover bitmap added.
  *
  *	Revision 1.101  2005/11/18 03:22:09  flydba
  *	Right hand side panel almost finished.
  *
  *	Revision 1.100  2005/11/17 23:32:46  movieman523
  *	Added support for specifying the maximum current for a circuit breaker. Exceed that current and the breaker pops.
  *
  **************************************************************************/

#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"

#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"
#include "tracer.h"

extern GDIParams g_Param;

void BaseInit() 

{
	//
	// need to init device-dependent resources here in case the screen mode has changed
	//

	g_Param.col[2] = oapiGetColour(154, 154, 154);
	g_Param.col[3] = oapiGetColour(3, 3, 3);
	g_Param.col[4] = oapiGetColour(255, 0, 255);
	g_Param.col[5] = oapiGetColour(255, 0, 255);
}

//
//Needle function by Rob Conley from Mercury code
//

void DrawNeedle (HDC hDC, int x, int y, double rad, double angle, HPEN pen0, HPEN pen1)
{
	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;

	oldObj = SelectObject (hDC, pen1);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, oldObj);
	oldObj = SelectObject (hDC, pen0);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
	SelectObject (hDC, oldObj);
}

//
// Altimeter Needle function by Rob Conley from Mercury code, Heavily modified to have non linear gauge range... :):)
//

void Saturn::RedrawPanel_Alt (SURFHANDLE surf)
{
	double alpha;
	double range;

	alpha = GetAltitude();
	alpha = alpha / 0.305;

#define ALTIMETER_X_CENTER	68
#define ALTIMETER_Y_CENTER	69
#define ALTIMETER_RADIUS	55.0

	//sprintf(oapiDebugString(), "altitude %f", alpha);
	if (alpha > 50000) alpha = 50000;

	if (alpha < 4001){
		range = 120 * RAD;
		range = range / 4000;
		alpha = 4000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+150*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 4001 && alpha < 6001){
		range = 35 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+185*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 6001 && alpha < 8001){
		range = 25 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+165*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 8001 && alpha < 10001){
		range = 20 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+150*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 10001 && alpha < 20001){
		range = 55 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+70*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 20001 && alpha < 40001){
		range = 65 * RAD;
		range = range / 20000;
		alpha = 20000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+15*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else {
		range = 20 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+10*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	oapiBlt(surf, srf[SRF_ALTIMETER], 0, 0, 0, 0, 137, 137, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_Alt2 (SURFHANDLE surf)
{
	double alpha;
	double range;

	alpha = GetAltitude();
	alpha = alpha / 0.305;

#define ALTIMETER2_X_CENTER	80
#define ALTIMETER2_Y_CENTER	80
#define ALTIMETER2_RADIUS	70.0

	//sprintf(oapiDebugString(), "altitude %f", alpha);
	if (alpha > 50000) alpha = 50000;

	if (alpha < 4001){
		range = 120 * RAD;
		range = range / 4000;
		alpha = 4000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+150*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 4001 && alpha < 6001){
		range = 35 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+185*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 6001 && alpha < 8001){
		range = 25 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+165*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 8001 && alpha < 10001){
		range = 20 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+150*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 10001 && alpha < 20001){
		range = 55 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+70*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 20001 && alpha < 40001){
		range = 65 * RAD;
		range = range / 20000;
		alpha = 20000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+15*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else {
		range = 20 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+10*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	oapiBlt(surf, srf[SRF_ALTIMETER2], 0, 0, 0, 0, 161, 161, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset, int ydist) {

	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, g_Param.font[2]);
	SetTextColor (hDC, RGB(196, 196, 196));
	SetTextAlign (hDC, TA_CENTER);
	SetBkMode (hDC, TRANSPARENT);
	const char *label;
	for (int bt = 0; bt < 6; bt++) {
		if (label = oapiMFDButtonLabel (mfd, bt+side*6))
			TextOut (hDC, 10 + xoffset, 3 + ydist * bt + yoffset, label, strlen(label));
		else break;
	}
	oapiReleaseDC (surf, hDC);
}


//
// Free all allocated surfaces.
//

void Saturn::ReleaseSurfaces ()
{
	for (int i = 0; i < nsurf; i++) {
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
	}
}

//
// Load all the surfaces and initialise the panel.
//

void Saturn::InitPanel (int panel)

{
	//
	// I'm pretty sure that some of these bitmaps aren't used anymore.
	// We need to go through these and delete the unused ones at some
	// point, and remove them from the resource.rc file so as not to
	// bloat the DLL.
	//

	srf[SRF_INDICATOR]								= oapiCreateSurface (LOADBMP (IDB_INDICATOR));
	srf[SRF_NEEDLE]									= oapiCreateSurface (LOADBMP (IDB_NEEDLE));
	srf[SRF_DIGITAL]								= oapiCreateSurface (LOADBMP (IDB_DIGITAL));
	srf[SRF_SWITCHUP]								= oapiCreateSurface (LOADBMP (IDB_SWITCHUP));
	srf[SRF_SWITCHLEVER]							= oapiCreateSurface (LOADBMP (IDB_SWLEVER));
	srf[SRF_SWITCHGUARDS]							= oapiCreateSurface (LOADBMP (IDB_SWITCHGUARDS));
	srf[SRF_SWITCHGUARDPANEL15]						= oapiCreateSurface (LOADBMP (IDB_SWITCHGUARDPANEL15));
	srf[SRF_ABORT]									= oapiCreateSurface (LOADBMP (IDB_ABORT));
	srf[SRF_LV_ENG]									= oapiCreateSurface (LOADBMP (IDB_LV_ENG));
	srf[SRF_ALTIMETER]								= oapiCreateSurface (LOADBMP (IDB_ALTIMETER));
	srf[SRF_THRUSTMETER]							= oapiCreateSurface (LOADBMP (IDB_THRUST));
	srf[SRF_DCVOLTS]								= oapiCreateSurface (LOADBMP (IDB_DCVOLTS));
	srf[SRF_DCVOLTS_PANEL101]						= oapiCreateSurface (LOADBMP (IDB_DCVOLTS_PANEL101));
	srf[SRF_DCAMPS]									= oapiCreateSurface (LOADBMP (IDB_DCAMPS));
	srf[SRF_ACVOLTS]								= oapiCreateSurface (LOADBMP (IDB_ACVOLTS));
	srf[SRF_SEQUENCERSWITCHES]						= oapiCreateSurface (LOADBMP (IDB_SEQUENCERSWITCHES));
	srf[SRF_MASTERALARM_BRIGHT]						= oapiCreateSurface (LOADBMP (IDB_MASTER_ALARM_BRIGHT));
	srf[SRF_DSKY]									= oapiCreateSurface (LOADBMP (IDB_DSKY_LIGHTS));
	srf[SRF_THREEPOSSWITCH]							= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH));
	srf[SRF_MFDFRAME]								= oapiCreateSurface (LOADBMP (IDB_MFDFRAME));
	srf[SRF_MFDPOWER]								= oapiCreateSurface (LOADBMP (IDB_MFDPOWER));
	srf[SRF_SM_RCS_MODE]							= oapiCreateSurface (LOADBMP (IDB_DOCKINGSWITCHES));
	srf[SRF_ROTATIONALSWITCH]						= oapiCreateSurface (LOADBMP (IDB_ROTATIONALSWITCH));
	srf[SRF_SUITCABINDELTAPMETER]					= oapiCreateSurface (LOADBMP (IDB_SUITCABINDELTAPMETER));
	srf[SRF_THREEPOSSWITCH305]						= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH305));
	srf[SRF_THREEPOSSWITCH305LEFT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH305LEFT));
	srf[SRF_SWITCH305LEFT]							= oapiCreateSurface (LOADBMP (IDB_SWITCH305LEFT));
	srf[SRF_DSKYDISP]       						= oapiCreateSurface (LOADBMP (IDB_DSKY_DISP));
	srf[SRF_FDAI]	        						= oapiCreateSurface (LOADBMP (IDB_FDAI));
	srf[SRF_FDAIROLL]       						= oapiCreateSurface (LOADBMP (IDB_FDAI_ROLL));
	srf[SRF_CWSLIGHTS]       						= oapiCreateSurface (LOADBMP (IDB_CWS_LIGHTS));
	srf[SRF_EVENT_TIMER_DIGITS]    					= oapiCreateSurface (LOADBMP (IDB_EVENT_TIMER));
	srf[SRF_DSKYKEY]		    					= oapiCreateSurface (LOADBMP (IDB_DSKY_KEY));
	srf[SRF_ECSINDICATOR]							= oapiCreateSurface (LOADBMP (IDB_ECSINDICATOR));
	srf[SRF_SWITCHUPSMALL]							= oapiCreateSurface (LOADBMP (IDB_SWITCHUPSMALL));
	srf[SRF_CMMFDFRAME]								= oapiCreateSurface (LOADBMP (IDB_CMMFDFRAME));
	srf[SRF_COAS]									= oapiCreateSurface (LOADBMP (IDB_COAS));
	srf[SRF_THUMBWHEEL_SMALLFONTS]					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALLFONTS));
	srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL]			= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALLFONTS_DIAGONAL));
	srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT]	= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT));
	srf[SRF_CIRCUITBRAKER]          				= oapiCreateSurface (LOADBMP (IDB_CIRCUITBRAKER));
	srf[SRF_THREEPOSSWITCH20]						= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH20));
	srf[SRF_THREEPOSSWITCH30]						= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH30));
	srf[SRF_THREEPOSSWITCH30LEFT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH30LEFT));
	srf[SRF_SWITCH20]								= oapiCreateSurface (LOADBMP (IDB_SWITCH20));
	srf[SRF_SWITCH30]								= oapiCreateSurface (LOADBMP (IDB_SWITCH30));
	srf[SRF_SWITCH30LEFT]							= oapiCreateSurface (LOADBMP (IDB_SWITCH30LEFT));
	srf[SRF_SWITCH20LEFT]							= oapiCreateSurface (LOADBMP (IDB_SWITCH20LEFT));
	srf[SRF_THREEPOSSWITCH20LEFT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH20LEFT));
	srf[SRF_GUARDEDSWITCH20]						= oapiCreateSurface (LOADBMP (IDB_GUARDEDSWITCH20));
	srf[SRF_FDAIPOWERROTARY]						= oapiCreateSurface (LOADBMP (IDB_FDAIPOWERROTARY));
	srf[SRF_DIRECTO2ROTARY]							= oapiCreateSurface (LOADBMP (IDB_DIRECTO2ROTARY));
	srf[SRF_ECSGLYCOLPUMPROTARY]					= oapiCreateSurface (LOADBMP (IDB_ECSGLYCOLPUMPROTARY));
	srf[SRF_GTACOVER]								= oapiCreateSurface (LOADBMP (IDB_GTACOVER));
	srf[SRF_POSTLDGVENTVLVLEVER]					= oapiCreateSurface (LOADBMP (IDB_POSTLDGVENTVLVLEVER));
	srf[SRF_SPSMAXINDICATOR]						= oapiCreateSurface (LOADBMP (IDB_SPSMAXINDICATOR));
	srf[SRF_SPSMININDICATOR]						= oapiCreateSurface (LOADBMP (IDB_SPSMININDICATOR));
	srf[SRF_ECSROTARY]								= oapiCreateSurface (LOADBMP (IDB_ECSROTARY));
	srf[SRF_CSM_MNPNL_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_MNPNL_WDW_LES));
	srf[SRF_CSM_RNDZ_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_RNDZ_WDW_LES));
	srf[SRF_CSM_RIGHT_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_RIGHT_WDW_LES));
	srf[SRF_CSM_LEFT_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_LEFT_WDW_LES));
	srf[SRF_GLYCOLLEVER]							= oapiCreateSurface (LOADBMP (IDB_GLYCOLLEVER));
	srf[SRF_FDAIOFFFLAG]       						= oapiCreateSurface (LOADBMP (IDB_FDAIOFFFLAG));
	srf[SRF_FDAINEEDLES]							= oapiCreateSurface (LOADBMP (IDB_FDAINEEDLES));
	srf[SRF_THUMBWHEEL_LARGEFONTS]					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_LARGEFONTS));
	srf[SRF_SPS_FONT_WHITE]							= oapiCreateSurface (LOADBMP (IDB_SPS_FUEL_FONT_WHITE));
	srf[SRF_SPS_FONT_BLACK]							= oapiCreateSurface (LOADBMP (IDB_SPS_FUEL_FONT_BLACK));
	srf[SRF_THUMBWHEEL_SMALL]						= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALL));
	srf[SRF_THUMBWHEEL_LARGEFONTSINV] 				= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_LARGEFONTSINV));
	srf[SRF_SWLEVERTHREEPOS] 						= oapiCreateSurface (LOADBMP (IDB_SWLEVERTHREEPOS));
	srf[SRF_ORDEAL_ROTARY] 							= oapiCreateSurface (LOADBMP (IDB_ORDEAL_ROTARY));
	srf[SRF_LV_ENG_S1B]								= oapiCreateSurface (LOADBMP (IDB_LV_ENGINE_LIGHTS_S1B));
	srf[SRF_SPS_INJ_VLV]						    = oapiCreateSurface (LOADBMP (IDB_SPS_INJ_VLV));
	srf[SRF_THUMBWHEEL_GPI_PITCH]  					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_GPI_PITCH));
	srf[SRF_THUMBWHEEL_GPI_YAW]  					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_GPI_YAW));
	srf[SRF_THC]				  					= oapiCreateSurface (LOADBMP (IDB_THC));
	srf[SRF_EMS_LIGHTS]			  					= oapiCreateSurface (LOADBMP (IDB_EMS_LIGHTS));
	srf[SRF_SUITRETURN_LEVER]	 					= oapiCreateSurface (LOADBMP (IDB_SUITRETURN_LEVER));
	srf[SRF_CABINRELIEFUPPERLEVER]	 				= oapiCreateSurface (LOADBMP (IDB_CABINRELIEFUPPERLEVER));
	srf[SRF_CABINRELIEFLOWERLEVER]	 				= oapiCreateSurface (LOADBMP (IDB_CABINRELIEFLOWERLEVER));
	srf[SRF_CABINRELIEFGUARDLEVER]	 				= oapiCreateSurface (LOADBMP (IDB_CABINRELIEFGUARDLEVER));
	srf[SRF_OPTICS_HANDCONTROLLER]	 				= oapiCreateSurface (LOADBMP (IDB_OPTICS_HANDCONTROLLER));
	srf[SRF_MARK_BUTTONS]	 						= oapiCreateSurface (LOADBMP (IDB_MARK_BUTTONS));
	srf[SRF_THREEPOSSWITCHSMALL]	 				= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCHSMALL));
	srf[SRF_OPTICS_DSKY]	 						= oapiCreateSurface (LOADBMP (IDB_OPTICS_DSKY));
	srf[SRF_MINIMPULSE_HANDCONTROLLER] 				= oapiCreateSurface (LOADBMP (IDB_MINIMPULSE_HANDCONTROLLER));
	srf[SRF_EMS_SCROLL_LEO]							= oapiCreateSurface (LOADBMP (IDB_EMS_SCROLL_LEO));
	srf[SRF_EMS_SCROLL_BORDER]						= oapiCreateSurface (LOADBMP (IDB_EMS_SCROLL_BORDER));
	srf[SRF_EMS_RSI_BKGRND]                         = oapiCreateSurface (LOADBMP (IDB_EMS_RSI_BKGRND));
	srf[SRF_EMSDVSETSWITCH]							= oapiCreateSurface (LOADBMP (IDB_EMSDVSETSWITCH));
	srf[SRF_ALTIMETER2]								= oapiCreateSurface (LOADBMP (IDB_ALTIMETER2));
	srf[SRF_OXYGEN_SURGE_TANK_VALVE]				= oapiCreateSurface (LOADBMP (IDB_OXYGEN_SURGE_TANK_VALVE));
	srf[SRF_GLYCOL_TO_RADIATORS_KNOB]				= oapiCreateSurface (LOADBMP (IDB_GLYCOL_TO_RADIATORS_KNOB));
	srf[SRF_ACCUM_ROTARY]							= oapiCreateSurface (LOADBMP (IDB_ACCUM_ROTARY));
	srf[SRF_GLYCOL_ROTARY]							= oapiCreateSurface (LOADBMP (IDB_GLYCOL_ROTARY));
	srf[SRF_TANK_VALVE]								= oapiCreateSurface (LOADBMP (IDB_TANK_VALVE));
	srf[SRF_PRESS_RELIEF_VALVE]						= oapiCreateSurface (LOADBMP (IDB_PRESS_RELIEF_VALVE));
	srf[SRF_CABIN_REPRESS_VALVE]					= oapiCreateSurface (LOADBMP (IDB_CABIN_REPRESS_VALVE));
	srf[SRF_SELECTOR_INLET_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_SELECTOR_INLET_ROTARY));							
	srf[SRF_SELECTOR_OUTLET_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_SELECTOR_OUTLET_ROTARY));
	srf[SRF_EMERGENCY_PRESS_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_EMERGENCY_PRESS_ROTARY));

	//
	// Flashing borders.
	//

	srf[SRF_BORDER_31x31]			= oapiCreateSurface (LOADBMP (IDB_BORDER_31x31));
	srf[SRF_BORDER_34x29]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x29));
	srf[SRF_BORDER_34x61]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x61));
	srf[SRF_BORDER_55x111]			= oapiCreateSurface (LOADBMP (IDB_BORDER_55x111));
	srf[SRF_BORDER_46x75]			= oapiCreateSurface (LOADBMP (IDB_BORDER_46x75));
	srf[SRF_BORDER_39x38]			= oapiCreateSurface (LOADBMP (IDB_BORDER_39x38));
	srf[SRF_BORDER_92x40]			= oapiCreateSurface (LOADBMP (IDB_BORDER_92x40));
	srf[SRF_BORDER_34x33]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x33));
	srf[SRF_BORDER_29x29]			= oapiCreateSurface (LOADBMP (IDB_BORDER_29x29));
	srf[SRF_BORDER_34x31]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x31));
	srf[SRF_BORDER_50x158]			= oapiCreateSurface (LOADBMP (IDB_BORDER_50x158));
	srf[SRF_BORDER_38x52]			= oapiCreateSurface (LOADBMP (IDB_BORDER_38x52));
	srf[SRF_BORDER_34x34]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x34));
	srf[SRF_BORDER_90x90]			= oapiCreateSurface (LOADBMP (IDB_BORDER_90x90));
	srf[SRF_BORDER_84x84]			= oapiCreateSurface (LOADBMP (IDB_BORDER_84x84));
	srf[SRF_BORDER_70x70]			= oapiCreateSurface (LOADBMP (IDB_BORDER_70x70));
	srf[SRF_BORDER_23x20]			= oapiCreateSurface (LOADBMP (IDB_BORDER_23x20));
	srf[SRF_BORDER_78x78]			= oapiCreateSurface (LOADBMP (IDB_BORDER_78x78));
	srf[SRF_BORDER_32x160]			= oapiCreateSurface (LOADBMP (IDB_BORDER_32x160));
	srf[SRF_BORDER_72x72]			= oapiCreateSurface (LOADBMP (IDB_BORDER_72x72));
	srf[SRF_BORDER_75x64]			= oapiCreateSurface (LOADBMP (IDB_BORDER_75x64));
	srf[SRF_BORDER_58x58]			= oapiCreateSurface (LOADBMP (IDB_BORDER_58x58));
	srf[SRF_BORDER_160x32]			= oapiCreateSurface (LOADBMP (IDB_BORDER_160x32));
	srf[SRF_BORDER_57x57]			= oapiCreateSurface (LOADBMP (IDB_BORDER_57x57));
	srf[SRF_BORDER_47x47]			= oapiCreateSurface (LOADBMP (IDB_BORDER_47x47));
	srf[SRF_BORDER_48x48]			= oapiCreateSurface (LOADBMP (IDB_BORDER_48x48));
	srf[SRF_BORDER_65x65]			= oapiCreateSurface (LOADBMP (IDB_BORDER_65x65));

	//
	// Set color keys where appropriate.
	//

	oapiSetSurfaceColourKey (srf[SRF_NEEDLE],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHLEVER],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHUP],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHGUARDS],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHGUARDPANEL15],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ALTIMETER],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THRUSTMETER],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SEQUENCERSWITCHES],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ROTATIONALSWITCH],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SUITCABINDELTAPMETER],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH305],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH305LEFT],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH305LEFT],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH20],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH20LEFT],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH20],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH20LEFT],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_GUARDEDSWITCH20],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH30],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH30LEFT],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH30],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH30LEFT],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_DSKYDISP],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_FDAI],									g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_FDAIROLL],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHUPSMALL],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_COAS],									g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_SMALLFONTS],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL],		g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT],	g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CIRCUITBRAKER],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_FDAIPOWERROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DIRECTO2ROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ECSGLYCOLPUMPROTARY],					g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_GTACOVER],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_POSTLDGVENTVLVLEVER],					g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SPSMAXINDICATOR],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SPSMININDICATOR],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ECSROTARY],							g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_CSM_MNPNL_WDW_LES],					g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_CSM_RNDZ_WDW_LES],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_RIGHT_WDW_LES],					g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_CSM_LEFT_WDW_LES],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_GLYCOLLEVER],							g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_FDAIOFFFLAG],							g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_FDAINEEDLES],							g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_LARGEFONTS],				g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ACVOLTS],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DCVOLTS],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DCAMPS],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DCVOLTS_PANEL101],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_SMALL],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_LARGEFONTSINV],				g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SWLEVERTHREEPOS],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ORDEAL_ROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SPS_INJ_VLV],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_GPI_PITCH],    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_GPI_YAW],    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THC],				    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SUITRETURN_LEVER],	    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABINRELIEFUPPERLEVER],   				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABINRELIEFLOWERLEVER],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABINRELIEFGUARDLEVER],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_OPTICS_HANDCONTROLLER],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_MARK_BUTTONS],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCHSMALL],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_MINIMPULSE_HANDCONTROLLER],			g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_EMS_SCROLL_BORDER],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ALTIMETER2],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SM_RCS_MODE],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_OXYGEN_SURGE_TANK_VALVE],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_GLYCOL_TO_RADIATORS_KNOB],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ACCUM_ROTARY],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_GLYCOL_ROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_TANK_VALVE],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_PRESS_RELIEF_VALVE],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABIN_REPRESS_VALVE],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SELECTOR_INLET_ROTARY],				g_Param.col[4]);							
	oapiSetSurfaceColourKey (srf[SRF_SELECTOR_OUTLET_ROTARY],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_EMERGENCY_PRESS_ROTARY],				g_Param.col[4]);
	
	//
	// Borders need to set the center color to transparent so only the outline
	// is visible.
	//

	oapiSetSurfaceColourKey	(srf[SRF_BORDER_31x31],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x29],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x61],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_55x111],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_46x75],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_39x38],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_92x40],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x33],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_29x29],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x31],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_50x158],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_38x52],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x34],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_90x90],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_84x84],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_70x70],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_23x20],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_78x78],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_32x160],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_72x72],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_75x64],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_58x58],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_160x32],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_57x57],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_47x47],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_48x48],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_65x65],		g_Param.col[4]);

	SetSwitches(panel);
}

bool Saturn::clbkLoadPanel (int id) {

	TRACESETUP("Saturn::clbkLoadPanel");

	//
	// Release all surfaces
	//
	ReleaseSurfaces();

	//
	// Should we display a panel for unmanned flights?
	//

	if (!Crewed)
		return false;

	//
	// No panel in engineering camera view.
	//

	if (viewpos == SATVIEW_ENG1 || viewpos == SATVIEW_ENG2 || viewpos == SATVIEW_ENG3)
		return false;

	//
	// Get screen info from the configurator
	//

	bool renderViewportIsWideScreen = false;
	HMODULE hpac = GetModuleHandle("Modules\\Startup\\ProjectApolloConfigurator.dll");
	if (hpac) {
		bool (__stdcall *pacRenderViewportIsWideScreen)();
		pacRenderViewportIsWideScreen = (bool (__stdcall *)()) GetProcAddress(hpac, "pacRenderViewportIsWideScreen");
		if (pacRenderViewportIsWideScreen) {
			renderViewportIsWideScreen = pacRenderViewportIsWideScreen();
		}
	}

	//
	// Load panel background image
	//
	HBITMAP hBmp;
	MFDSPEC mfds_dock		=     {{1019,  784, 1238,  999}, 6, 6, 31, 31};


	if ((id == SATPANEL_GN && !GNSplit) || (id == SATPANEL_GN_LEFT && !GNSplit) || (id == SATPANEL_GN_RIGHT && !GNSplit)) { // guidance & navigation lower equipment bay (unsplit)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(-1, -1, SATPANEL_LOWER_MAIN, SATPANEL_TELESCOPE);

		AddLeftLowerPanelAreas();
		AddLeftCenterLowerPanelAreas(0);
		AddCenterLowerPanelAreas(0);
		AddRightCenterLowerPanelAreas(0);
		AddRightLowerPanelAreas(0);

		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}
	if (id == SATPANEL_GN_LEFT && GNSplit) { // guidance & navigation lower equipment bay Left third (split)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL_LEFT));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(-1, SATPANEL_GN_CENTER, SATPANEL_LOWER_LEFT, SATPANEL_TELESCOPE);

		AddLeftLowerPanelAreas();
		AddLeftCenterLowerPanelAreas(0);
		
		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_GN_CENTER && GNSplit) { // guidance & navigation lower equipment bay Center third (split)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL_CENTER));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_GN_LEFT, SATPANEL_GN_RIGHT, SATPANEL_LOWER_MAIN, SATPANEL_TELESCOPE);

		AddLeftCenterLowerPanelAreas(-1140);
		AddCenterLowerPanelAreas(-1140);
		AddRightCenterLowerPanelAreas(-1140);
		

		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_GN_RIGHT && GNSplit) { // guidance & navigation lower equipment bay Right third (split)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL_RIGHT));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_GN_CENTER, -1, SATPANEL_RIGHT_CB, SATPANEL_TELESCOPE);

		AddRightCenterLowerPanelAreas(-2397);
		AddRightLowerPanelAreas(-2397);
		
		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (MainPanelSplit && id == SATPANEL_MAIN_LEFT) {
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_LEFT_PANEL));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_LEFT, SATPANEL_MAIN_MIDDLE, SATPANEL_LEFT_RNDZ_WINDOW, SATPANEL_LOWER_LEFT);

		AddLeftMainPanelAreas();
		AddLeftMiddleMainPanelAreas(0);

		// Dummy 1px MFDs in order to force Orbiter to load the MFD data from the scenario
		MFDSPEC mfds_user1 = {{ 0, 0, 1, 1}, 0, 0, 0, 0};
		MFDSPEC mfds_user2 = {{ 0, 0, 1, 1}, 0, 0, 0, 0};
		MFDSPEC mfds_right = {{ 0, 0, 1, 1}, 0, 0, 0, 0};
		oapiRegisterMFD(MFD_USER1, mfds_user1);
		oapiRegisterMFD(MFD_USER2, mfds_user2);
		oapiRegisterMFD(MFD_RIGHT, mfds_right);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (MainPanelSplit && id == SATPANEL_MAIN_MIDDLE) {
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_MIDDLE_PANEL));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_MAIN_LEFT, SATPANEL_MAIN_RIGHT, SATPANEL_HATCH_WINDOW, SATPANEL_LOWER_MAIN);

		AddLeftMiddleMainPanelAreas(-1022);
		AddRightMiddleMainPanelAreas(-1022);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (MainPanelSplit && id == SATPANEL_MAIN_RIGHT) {

		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_RIGHT_PANEL));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_MAIN_MIDDLE, SATPANEL_RIGHT, SATPANEL_RIGHT_RNDZ_WINDOW, SATPANEL_RIGHT_CB);
		
		AddRightMiddleMainPanelAreas(-1772);
		AddRightMainPanelAreas(-1772);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (!MainPanelSplit && id == SATPANEL_MAIN) { // main instrument panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_PANEL));
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_LEFT, SATPANEL_RIGHT, SATPANEL_HATCH_WINDOW, SATPANEL_LOWER_MAIN);
		
		AddLeftMainPanelAreas();
		AddLeftMiddleMainPanelAreas(0);
		AddRightMiddleMainPanelAreas(0);
		AddRightMainPanelAreas(0);

		// Dummy 1px MFDs in order to force Orbiter to load the MFD data from the scenario
		MFDSPEC mfds_user1 = {{ 0, 0, 1, 1}, 0, 0, 0, 0};
		MFDSPEC mfds_user2 = {{ 0, 0, 1, 1}, 0, 0, 0, 0};
		oapiRegisterMFD(MFD_USER1, mfds_user1);
		oapiRegisterMFD(MFD_USER2, mfds_user2);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LEFT) { // left instrument panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_PANEL));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_MAIN_LEFT, -1, SATPANEL_LOWER_LEFT);
		else
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_MAIN, -1, SATPANEL_LOWER_LEFT);

		oapiRegisterPanelArea (AID_CSM_LEFT_WDW_LES,							_R( 553,  244, 1062,  733), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,					PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_LEFTCOASSWITCH,								_R(1316,   63, 1350,   94), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTTUTILITYPOWERSWITCH,						_R(1425,   81, 1459,  112), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POSTLANDINGBCNLTSWITCH,						_R(1468,   88, 1502,  119), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POSTLANDINGDYEMARKERSWITCH,		    		_R(1508,   71, 1554,  146), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POSTLANDINGVENTSWITCH,						_R(1592,  109, 1626,  140), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMODEINTERCOMVOXSENSTHUMBWHEEL,			_R(1383,  304, 1416,  347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTPOWERMASTERVOLUMETHUMBWHEEL,				_R(1506,  279, 1539,  322), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTPADCOMMVOLUMETHUMBWHEEL,					_R(1340,  411, 1373,  454), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTINTERCOMVOLUMETHUMBWHEEL,				_R(1461,  392, 1494,  435), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTSBANDVOLUMETHUMBWHEEL,					_R(1297,  519, 1330,  562), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTVHFVOLUMETHUMBWHEEL,						_R(1418,  499, 1451,  542), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMODEINTERCOMSWITCH,						_R(1336,  292, 1370,  326), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTAUDIOPOWERSWITCH,						_R(1551,  304, 1585,  338), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTPADCOMMSWITCH,							_R(1293,  398, 1327,  432), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTINTERCOMSWITCH,							_R(1506,  417, 1540,  451), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTSBANDSWITCH,								_R(1250,  507, 1284,  541), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTVHFAMSWITCH,								_R(1463,  523, 1497,  557), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTAUDIOCONTROLSWITCH,						_R(1255,  632, 1289,  666), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTSUITPOWERSWITCH,							_R(1320,  658, 1354,  692), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_VHFRNGSWITCH,								_R(1385,  684, 1419,  718), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_STABCONTCIRCUITBREAKERS,						_R( 433,  898,  597,  927), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_AUTORCSSELECTSWITCHES,						_R( 659,  893, 1368,  922), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS,	_R( 454, 1001, 1015, 1030), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS2,	_R( 473, 1082,  844, 1111), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOODDIMSWITCH,								_R( 898, 1071,  932, 1100), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOODFIXEDSWITCH,							_R( 943, 1071,  977, 1100), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_REACTIONCONTROLSYSTEMCIRCUITBREAKERS,		_R( 494, 1161, 1017, 1190), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOATBAGSWITCHES,							_R(1037, 1123, 1191, 1175), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SEQEVENTSCONTSYSTEM,							_R(1200, 1123, 1371, 1175), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SERVICEPROPULSIONSYSCIRCUITBREAKERS,			_R( 505, 1243, 1052, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOATBAGCIRCUITBREAKERS,						_R(1062, 1243, 1167, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SEQEVENTSCONTSYSCIRCUITBREAKERS,				_R(1176, 1243, 1319, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EDSCIRCUITBREAKERS,							_R( 559, 1323,  664, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ELSCIRCUITBREAKERS,							_R( 673, 1323,  740, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PLVENTCIRCUITBREAKER,						_R( 749, 1323,  778, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EDSPOWERSWITCH,								_R(1099, 1399, 1133, 1432), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TVCSERVOPOWERSWITCHES,						_R( 996, 1432, 1055, 1500), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LOGICPOWERSWITCH,							_R( 941, 1505,  975, 1538), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SIGCONDDRIVERBIASPOWERSWITCHES,				_R( 857, 1546,  916, 1614), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTINTERIORLIGHTROTARIES,					_R(1032,  989, 1360, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FDAIPOWERROTARY,								_R( 866, 1401,  956, 1491), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SCSELECTRONICSPOWERROTARY,					_R( 739, 1408,  829, 1498), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BMAGPOWERROTARY1,							_R( 600, 1419,  690, 1509), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BMAGPOWERROTARY2,							_R( 666, 1511,  756, 1601), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DIRECTO2ROTARY,								_R( 765, 1575,  835, 1645), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCIRCUITRETURNVALVE,						_R(  65, 1252,  225, 1285), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OXYGEN_SURGE_TANK_VALVE,						_R( 121,  201,  155,  235), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(-1.0, 0.0, 0.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_RIGHT) { // right instrument panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_PANEL));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_MAIN_RIGHT, -1, -1, SATPANEL_RIGHT_CB);
		else
			oapiSetPanelNeighbours(SATPANEL_MAIN, -1, -1, SATPANEL_RIGHT_CB);

		oapiRegisterPanelArea (AID_CSM_RIGHT_WDW_LES,							_R( 621,  244, 1130,  733), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,					PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_FUELCELLPUMPSSWITCHES,      					_R( 311,  881,  475,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSWITCHES,      				_R( 825, 1428,  901, 1519), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSSWITCH,						_R( 734, 1525,  824, 1615), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EPSSENSORSIGNALDCCIRCUITBRAKERS,				_R( 856,  871,  923,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EPSSENSORSIGNALACCIRCUITBRAKERS,				_R( 962,  871, 1092,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CWCIRCUITBRAKERS,				            _R(1130,  871, 1197,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMPWRCIRCUITBRAKERS,				            _R(1206,  871, 1238,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INVERTERCONTROLCIRCUITBRAKERS,				_R( 856,  949,  961,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EPSSENSORUNITCIRCUITBRAKERS,					_R( 987,  949, 1127,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BATRLYBUSCIRCUITBRAKERS,					    _R(1135,  949, 1201,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSRADIATORSCIRCUITBRAKERS,					_R( 680, 1032,  955, 1061), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BATTERYCHARGERCIRCUITBRAKERS,				_R( 963, 1032, 1184, 1061), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INSTRUMENTLIGHTINGCIRCUITBRAKERS,			_R( 312, 1121,  588, 1150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSCIRCUITBRAKERS,			                _R( 616, 1121, 1191, 1150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSLOWERROWCIRCUITBRAKERS,			        _R( 358, 1206, 1158, 1235), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GUIDANCENAVIGATIONCIRCUITBRAKERS,			_R( 389, 1288,  780, 1317), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC1ACIRCUITBRAKER,			_R( 821, 1634,  850, 1663), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC1BCIRCUITBRAKER,			_R( 843, 1603,  872, 1632), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC1CCIRCUITBRAKER,			_R( 865, 1572,  894, 1601), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC2ACIRCUITBRAKER,			_R( 899, 1525,  928, 1554), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC2BCIRCUITBRAKER,			_R( 921, 1494,  950, 1523), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC2CCIRCUITBRAKER,			_R( 943, 1463,  972, 1492), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC1ACIRCUITBRAKER,				_R( 889, 1657,  918, 1686), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC1BCIRCUITBRAKER,				_R( 912, 1626,  941, 1655), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC1CCIRCUITBRAKER,				_R( 935, 1594,  964, 1623), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC2ACIRCUITBRAKER,				_R( 957, 1562,  986, 1591), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC2BCIRCUITBRAKER,				_R( 980, 1530, 1009, 1559), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC2CCIRCUITBRAKER,				_R(1003, 1499, 1032, 1528), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTCOASSWITCH,								_R( 330,   63,  364,   94), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MODEINTERCOMVOXSENSTHUMBWHEEL,				_R( 138,  280,  171,  323), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POWERMASTERVOLUMETHUMBWHEEL,					_R( 262,  299,  295,  342), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PADCOMMVOLUMETHUMBWHEEL,						_R( 181,  387,  214,  430), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INTERCOMVOLUMETHUMBWHEEL,					_R( 307,  412,  340,  455), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SBANDVOLUMETHUMBWHEEL,						_R( 224,  496,  257,  539), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_VHFVOLUMETHUMBWHEEL,							_R( 350,  519,  383,  562), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_AUDIOCONTROLSWITCH,							_R( 347,  649,  381,  683), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITPOWERSWITCH,								_R( 403,  627,  437,  661), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTUTILITYPOWERSWITCH,						_R( 221,   81,  255,  112), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTDOCKINGTARGETSWITCH,					_R(  54,  109,   88,  140), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTMODEINTERCOMSWITCH,						_R(  92,  305,  126,  339), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTAUDIOPOWERSWITCH,						_R( 308,  287,  342,  321), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTPADCOMMSWITCH,							_R( 135,  411,  169,  445), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTINTERCOMSWITCH,							_R( 353,  400,  387,  434), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTSBANDSWITCH,							_R( 178,  520,  212,  554), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTVHFAMSWITCH,							_R( 396,  507,  430,  541), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GNPOWERSWITCH,								_R( 506,  881,  540,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAINBUSTIESWITCHES,							_R( 608,  881,  687,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BATTERYCHARGERSWITCH,						_R( 698,  881,  732,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_NONESSBUSSWITCH,								_R( 763,  881,  797,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INTERIORLIGHTSFLOODSSWITCHES,				_R( 570, 1003,  649, 1032), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SPSGAUGINGSWITCH,							_R( 626, 1401,  660, 1434), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TELCOMSWITCHES,								_R( 672, 1416,  762, 1527), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTINTERIORLIGHTROTARIES,					_R( 319,  974,  542, 1064), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL275CIRCUITBRAKERS,				        _R(1467, 1092, 1496, 1717), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(1.0, 0.0, 0.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LEFT_RNDZ_WINDOW) { // left rendezvous window
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_RNDZ_WINDOW));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(-1, SATPANEL_HATCH_WINDOW, -1, SATPANEL_MAIN_LEFT);
		else
			oapiSetPanelNeighbours(-1, SATPANEL_HATCH_WINDOW, -1, SATPANEL_MAIN);

        oapiRegisterMFD (MFD_RIGHT, mfds_dock);	// MFD_USER1
		oapiRegisterPanelArea (AID_MFDDOCK,	        _R( 979,  773, 1280, 1024), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MFDDOCK_POWER,   _R( 958, 1004,  978, 1019), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,				       PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SM_RCS_MODE,     _R(1205,  700, 1280,  773), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,					   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_COAS,		    _R( 533,    0, 1216,  620), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,					   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ALTIMETER2,		_R( 787,  863,  948, 1024), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,					   PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_RIGHT_RNDZ_WINDOW) { // right rendezvous window
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_RNDZ_WINDOW));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_HATCH_WINDOW, -1, -1, SATPANEL_MAIN_RIGHT);
		else
			oapiSetPanelNeighbours(SATPANEL_HATCH_WINDOW, -1, -1, SATPANEL_MAIN);

		oapiRegisterPanelArea (AID_CSM_RNDZ_WDW_LES,	_R( 464,  103, 1105,  840), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_HATCH_WINDOW) { // hatch window
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_HATCH_WINDOW));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_LEFT_RNDZ_WINDOW, SATPANEL_RIGHT_RNDZ_WINDOW, -1, SATPANEL_MAIN_MIDDLE);
		else
			oapiSetPanelNeighbours(SATPANEL_LEFT_RNDZ_WINDOW, SATPANEL_RIGHT_RNDZ_WINDOW, -1, SATPANEL_MAIN);

		SetCameraDefaultDirection(_V(0.0, 0.83867, 0.544639));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_CABIN_PRESS_PANEL) { // cabin pressurization controls panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_CABIN_PRESS_PANEL));
		oapiSetPanelNeighbours(-1, SATPANEL_LEFT, -1, -1);
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		oapiRegisterPanelArea (AID_GLYCOLTORADIATORSLEVER,			_R(1488,   46, 1520,  206), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CABINPRESSURERELIEFLEVER1,		_R(1544,  412, 1695,  492), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CABINPRESSURERELIEFLEVER2,		_R(1431,  547, 1697,  635), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GLYCOLRESERVOIRROTARIES,			_R(1496,  705, 1574,  995), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OXYGENROTARIES,					_R(1498, 1146, 1788, 1224), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ORDEALSWITCHES,					_R( 503,   44,  746,  162), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ORDEALROTARY,					_R( 794,   64,  878,  148), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OXYGEN_SURGE_TANK_VALVE,			_R(1150,  201, 1184,  235), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GLYCOL_TO_RADIATORS_KNOB,		_R( 273,  362,  304,  393), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCIRCUITRETURNVALVE,			_R(1094, 1252, 1254, 1284), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GLYCOL_ROTARY,					_R( 117,  978,  189, 1050), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ACCUM_ROTARY,					_R( 669, 1025,  727, 1083), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL_352,						_R(  96, 2973,  383, 3191), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CABIN_REPRESS_VALVE,				_R( 612, 3096,  660, 3144), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_WATER_GLYCOL_TANKS_ROTARIES,		_R(1001, 2965, 1085, 3182), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EMERGENCY_CABIN_PRESSURE_ROTARY,	_R( 773, 3130,  838, 3195), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,			PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(-1.0, 0.0, 0.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LOWER_LEFT) { 
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_LEFT_PANEL));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (GNSplit) 
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_LOWER_MAIN, SATPANEL_LEFT, SATPANEL_GN_LEFT);
		else
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_LOWER_MAIN, SATPANEL_LEFT, SATPANEL_GN);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LOWER_MAIN) { 
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_MAIN_PANEL));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		int top, bottom;
		if (GNSplit) 
			bottom = SATPANEL_GN_CENTER;
		else
			bottom = SATPANEL_GN;

		if (MainPanelSplit)
			top = SATPANEL_MAIN_MIDDLE;
		else
			top = SATPANEL_MAIN;
		
		oapiSetPanelNeighbours(SATPANEL_LOWER_LEFT, SATPANEL_RIGHT_CB, top, bottom);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_RIGHT_CB) { 
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_CB_PANEL));
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (GNSplit) 
			oapiSetPanelNeighbours(SATPANEL_LOWER_MAIN, -1, SATPANEL_RIGHT, SATPANEL_GN_RIGHT);
		else
			oapiSetPanelNeighbours(SATPANEL_LOWER_MAIN, -1, SATPANEL_RIGHT, SATPANEL_GN);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_SEXTANT) { // Sextant
		if (renderViewportIsWideScreen) {
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_SEXTANT_WIDE));
		} else {
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_SEXTANT));
		}

		oapiSetPanelNeighbours(-1, SATPANEL_TELESCOPE, SATPANEL_GN, SATPANEL_GN);
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);
		
		if (renderViewportIsWideScreen) {
			oapiRegisterPanelArea (AID_OPTICS_HANDCONTROLLER,		_R( 980,  631, 1028,  689), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKBUTTON,					_R(1031,  645, 1058,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKREJECT,					_R(1069,  661, 1084,  681), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERSPEEDSWITCH,		_R( 604,  719,  627,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_GNMODESWITCH,				_R( 503,  719,  526,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,	_R( 687,  719,  710,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_OPTICS_DSKY,					_R( 926,    0, 1229,  349), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MINIMPULSE_HANDCONTROLLER,	_R( 192, 633,   240,  691), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		} else {
			oapiRegisterPanelArea (AID_OPTICS_HANDCONTROLLER,		_R( 878,  631,  926,  689), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKBUTTON,					_R( 929,  645,  956,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKREJECT,					_R( 967,  661,  982,  681), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERSPEEDSWITCH,		_R( 502,  719,  525,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_GNMODESWITCH,				_R( 401,  719,  424,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,	_R( 585,  719,  608,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_OPTICS_DSKY,					_R( 721,    0, 1024,  349), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MINIMPULSE_HANDCONTROLLER,	_R( 90,   633,  138,  691), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		}
		oapiRegisterPanelArea (AID_OPTICSCLKAREASEXT,		_R(   0,    0,   10,   10), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.53765284, 0.84316631));
		SetCameraRotationRange( PI/2., PI/2., PI/2., PI/2.);
	}

	if (id == SATPANEL_TELESCOPE) { // Telescope
		if (renderViewportIsWideScreen)
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_TELESCOPE_WIDE));
		else
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_TELESCOPE));
		oapiSetPanelNeighbours( SATPANEL_SEXTANT, -1, SATPANEL_GN, SATPANEL_GN);
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (renderViewportIsWideScreen) {
			oapiRegisterPanelArea (AID_OPTICS_HANDCONTROLLER,		_R( 980,  631, 1028,  689), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKBUTTON,					_R(1031,  645, 1058,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKREJECT,					_R(1069,  661, 1084,  681), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERSPEEDSWITCH,		_R( 604,  719,  627,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_GNMODESWITCH,				_R( 503,  719,  526,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,	_R( 687,  719,  710,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_OPTICS_DSKY,					_R( 926,    0, 1229,  349), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MINIMPULSE_HANDCONTROLLER,	_R( 192, 633,   240,  691), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		} else {
			oapiRegisterPanelArea (AID_OPTICS_HANDCONTROLLER,		_R( 878,  631,  926,  689), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKBUTTON,					_R( 929,  645,  956,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MARKREJECT,					_R( 967,  661,  982,  681), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERSPEEDSWITCH,		_R( 502,  719,  525,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_GNMODESWITCH,				_R( 401,  719,  424,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,	_R( 585,  719,  608,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_OPTICS_DSKY,					_R( 721,    0, 1024,  349), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_MINIMPULSE_HANDCONTROLLER,	_R( 90,   633,  138,  691), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		}
		oapiRegisterPanelArea (AID_OPTICSCLKAREATELE, _R(0, 0, 10, 10), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.53765284, 0.84316631));
		SetCameraRotationRange( PI/2., PI/2., PI/2., PI/2.);			
	}

	InitPanel (id);

	//
	// Change to desired panel next timestep.
	//
    if (!InPanel && id != PanelId && !(!MainPanelSplit && (PanelId == SATPANEL_MAIN_MIDDLE || PanelId == SATPANEL_MAIN_RIGHT))) {
		CheckPanelIdInTimestep = true;
	} else {
	    PanelId = id;
	}
	InVC = false;
	InPanel = true;

	//
	// Set view parameter
	//

	SetView();

	return hBmp != NULL;
}

void Saturn::AddLeftMainPanelAreas() {

	oapiRegisterPanelArea (AID_THC,											_R(   0, 1251,   72, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);	
	oapiRegisterPanelArea (AID_ABORT_BUTTON,								_R( 862,  600,  924,  631), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SEQUENCERSWITCHES,							_R( 802,  918,  990, 1100), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LV_ENGINE_LIGHTS,							_R( 843,  735,  944,  879), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_IMU_CAGE_SWITCH,								_R( 289, 1237,  325, 1306), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MASTER_ALARM,								_R( 464,  497,  509,  533), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ATTITUDE_CONTROL_SWITCHES,					_R( 190,  838,  482,  867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_BMAG_SWITCHES,								_R( 125, 1036,  258, 1065), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_THRUSTMETER,									_R( 498,  920,  593, 1011), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CMC_SWITCH,									_R( 343,  635,  377,  664), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FDAI_SWITCHES,								_R( 265,  742,  484,  771), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ALTIMETER,									_R( 836,   85,  973,  222), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ELS_SWITCHES,								_R( 702, 1150,  957, 1219), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EVENT_TIMER_SWITCHES,						_R( 701, 1260,  950, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ROT_PWR,										_R( 104,  948,  324,  977), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_dVTHRUST_SWITCHES,							_R( 442, 1046,  572, 1115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EVENT_TIMER,									_R( 866,  670,  937,  688), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS,											_R( 299, 1051,  337, 1103), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_GMETER,										_R( 403,  605,  482,  684), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SCCONTCMCMODESWITCHES,      					_R( 383,  948,  460,  977), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SCSTVCSWITCHES,      						_R( 380, 1173,  457, 1202), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSGIMBALMOTORSSWITCHES,      				_R( 472, 1173,  654, 1202), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ENTRYSWITCHES,      							_R( 336, 1260,  413, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LVSPSINDICATORSWITCHES,      				_R( 422, 1260,  499, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_TVCGIMBALDRIVESWITCHES,      				_R( 508, 1260,  585, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_GDCALIGNBUTTON,								_R( 290, 1169,  329, 1207), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DIRECT_ULLAGE_THRUST_ON,						_R( 370, 1037,  409, 1128), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
	// EMS
	oapiRegisterPanelArea (AID_EMSFUNCTIONSWITCH,      						_R( 595,  280,  685,  370), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_GTASWITCH,		    						_R( 904,  288,  959,  399), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ENTRY_MODE_SWITCH,							_R( 593,  402,  628,  432), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);	
	oapiRegisterPanelArea (AID_EMSDVSETSWITCH,								_R( 910,  431,  957,  517), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EMSDVDISPLAY,								_R( 743,  518,  900,  539), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_LIGHT,									_R( 816,  467,  846,  483), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PT05G_LIGHT,									_R( 758,  467,  788,  483), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EMS_SCROLL_LEO,								_R( 731,  296,  875,  448), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EMS_RSI_BKGRND,								_R( 602,  463,  690,  550), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
	// ASCP
	oapiRegisterPanelArea (AID_ASCPDISPLAYROLL,								_R( 199, 1144,  229, 1156), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,	                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPDISPLAYPITCH,							_R( 199, 1206,  229, 1218), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,	                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPDISPLAYYAW,								_R( 199, 1268,  229, 1280), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,	                PANEL_MAP_BACKGROUND);		
	oapiRegisterPanelArea (AID_ASCPROLL,									_R( 124, 1126,  142, 1163), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPPITCH,									_R( 124, 1188,  142, 1225), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPYAW,										_R( 124, 1250,  142, 1287), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	// GPFPI DISPLAYS
	oapiRegisterPanelArea (AID_GPFPI_METERS,								_R( 629,  927,  791, 1032), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSGIMBALPITCHTHUMBWHEEL,					_R( 655, 1058,  672, 1094), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSGIMBALYAWTHUMBWHEEL,						_R( 739, 1067,  775, 1084), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	// FDAI
	fdaiLeft.RegisterMe(AID_FDAI_LEFT, 533, 612);
	if (!hBmpFDAIRollIndicator)	hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE (IDB_FDAI_ROLLINDICATOR));
}

void Saturn::AddLeftMiddleMainPanelAreas(int offset) {

	oapiRegisterPanelArea (AID_SEPARATIONSWITCHES,		    				_R(1087 + offset,  935, 1341 + offset, 1004), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SUITCABINDELTAPMETER,	    				_R(1445 + offset,  106, 1491 + offset,  150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SMRCS_HELIUM1_SWITCHES,						_R(1585 + offset,  430, 1748 + offset,  459), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SMRCS_HELIUM2_SWITCHES,						_R(1411 + offset,  564, 1748 + offset,  633), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PRIM_PRPLNT_SWITCHES,						_R(1411 + offset,  718, 1748 + offset,  747), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SEC_PRPLT_SWITCHES,							_R(1411 + offset,  848, 1748 + offset,  877), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_GAUGES,									_R(1385 + offset,  385, 1548 + offset,  511), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LV_SWITCHES,									_R(1043 + offset, 1138, 1173 + offset, 1207), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MAIN_RELEASE_SWITCH,							_R(1042 + offset, 1228, 1078 + offset, 1297), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ABORT_ROW,									_R(1042 + offset, 1027, 1294 + offset, 1096), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CWS_LIGHTS_LEFT,								_R(1540 + offset,   97, 1752 + offset,  205), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_HELIUM1_TALKBACK,						_R(1591 + offset,  368, 1741 + offset,  391), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_HELIUM2_TALKBACK,						_R(1591 + offset,  526, 1741 + offset,  549), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_PROP1_TALKBACK,							_R(1591 + offset,  659, 1741 + offset,  682), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_PROP2_TALKBACK,							_R(1503 + offset,  792, 1739 + offset,  815), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DOCKINGPROBESWITCHES,    					_R(1388 + offset,  256, 1509 + offset,  325), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DOCKINGPROBEINDICATORS,      				_R(1396 + offset,  179, 1419 + offset,  229), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CSMLIGHTSWITCHES,      						_R(1518 + offset,  279, 1663 + offset,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LMPOWERSWITCH,      							_R(1692 + offset,  279, 1726 + offset,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);

	// Display & keyboard (DSKY), main panel uses the main DSKY.
	oapiRegisterPanelArea (AID_DSKY_DISPLAY,								_R(1239 + offset,  589, 1344 + offset,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY_LIGHTS,									_R(1095 + offset,  594, 1197 + offset,  714), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY_KEY,			                        _R(1075 + offset,  784, 1363 + offset,  905), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

	// FDAI
	fdaiRight.RegisterMe(AID_FDAI_RIGHT, 1090 + offset, 284);
	if (!hBmpFDAIRollIndicator)	hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE (IDB_FDAI_ROLLINDICATOR));

	// MFDs
	MFDSPEC mfds_mainleft = {{1462 + offset, 1075, 1721 + offset, 1330}, 6, 6, 37, 37};
    oapiRegisterMFD(MFD_LEFT, mfds_mainleft);
	oapiRegisterPanelArea (AID_MFDMAINLEFT,	    							_R(1413 + offset, 1060, 1772 + offset, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
}

void Saturn::AddRightMiddleMainPanelAreas(int offset) {

	oapiRegisterPanelArea (AID_MISSION_CLOCK,								_R(1835 + offset,  305, 1973 + offset,  324), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CYROTANKSWITCHES,        					_R(1912 + offset,  490, 2488 + offset,  520), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CYROTANKINDICATORS,        					_R(2173 + offset,  315, 2495 + offset,  439), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SUITCOMPRDELTAPMETER,       					_R(2069 + offset,  726, 2115 + offset,  770), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CABININDICATORS,        						_R(2278 + offset,  593, 2504 + offset,  717), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CAUTIONWARNING_SWITCHES,						_R(1908 + offset,  400, 2140 + offset,  434), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MISSION_TIMER_SWITCHES,						_R(2019 + offset,  299, 2139 + offset,  328), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_INDICATORS,								_R(1785 + offset,  445, 1875 + offset,  535), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECS_INDICATOR_SWITCH,						_R(1785 + offset,  582, 1875 + offset,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CWS_LIGHTS_RIGHT,							_R(1795 + offset,   97, 2008 + offset,  205), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSRADTEMPMETERS,							_R(1951 + offset,  604, 1997 + offset,  649), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSEVAPTEMPMETERS,							_R(2069 + offset,  604, 2115 + offset,  649), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSPRESSMETERS,								_R(2186 + offset,  604, 2232 + offset,  649), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSQUANTITYMETERS,							_R(2186 + offset,  726, 2232 + offset,  770), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSRADIATORINDICATOR,		 				_R(1799 + offset,  683, 1822 + offset,  706), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSRADIATORSWITCHES,         				_R(1796 + offset,  743, 2023 + offset,  772), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSSWITCHES,					 				_R(1787 + offset,  848, 2327 + offset,  877), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNAPITCHPOSITIONSWITCH,			_R(2268 + offset, 1016, 2358 + offset, 1116), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CABINTEMPAUTOCONTROLSWITCH,					_R(2441 + offset,  843, 2458 + offset,  879), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_POSTLANDINGVENTVALVELEVER,      				_R(2062 + offset,   76, 2112 + offset,  234), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNAUPPERSWITCHES,      			_R(2185 + offset,  943, 2262 + offset,  972), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNALOWERSWITCHES,      			_R(2381 + offset, 1157, 2458 + offset, 1186), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNAYAWPOSITIONSWITCH,      		_R(2398 + offset, 1016, 2488 + offset, 1116), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);

	// MFDs
	MFDSPEC mfds_mainright = {{1834 + offset, 1075, 2093 + offset, 1330}, 6, 6, 37, 37};
    oapiRegisterMFD(MFD_RIGHT, mfds_mainright);
	oapiRegisterPanelArea (AID_MFDMAINRIGHT,								_R(1785 + offset, 1060, 2144 + offset, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
}

void Saturn::AddRightMainPanelAreas(int offset) {

	oapiRegisterPanelArea (AID_CSM_MNPNL_WDW_LES,							_R(3071 + offset,    0, 3430 + offset,  160), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_FUELCELLINDICATORS,		    				_R(2763 + offset,  319, 2913 + offset,  443), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLPHRADTEMPINDICATORS,	  				_R(2822 + offset,  490, 3019 + offset,  513), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLRADIATORSINDICATORS,    				_R(2822 + offset,  539, 2931 + offset,  562), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLRADIATORSSWITCHES,    				_R(2816 + offset,  607, 2937 + offset,  637), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLINDICATORSSWITCH,    				_R(3027 + offset,  627, 3117 + offset,  717), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLHEATERSSWITCHES,	    				_R(2817 + offset,  695, 2938 + offset,  725), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLPURGESWITCHES,	    				_R(2815 + offset,  817, 3123 + offset,  846), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLREACTANTSINDICATORS,    				_R(2823 + offset,  893, 3061 + offset,  917), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLREACTANTSSWITCHES,    				_R(2800 + offset,  955, 3131 + offset,  984), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (ADI_MAINBUSAINDICATORS,		    				_R(2953 + offset,  758, 3062 + offset,  781), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLLATCHSWITCHES,	    				_R(2593 + offset, 1251, 2670 + offset, 1280), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_TELECOMTB,    								_R(3119 + offset, 1042, 3142 + offset, 1115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SBAND_NORMAL_SWITCHES,						_R(2593 + offset, 1050, 2858 + offset, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MASTER_ALARM2,								_R(2960 + offset,  652, 3005 + offset,  688), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DCVOLTS,										_R(3154 + offset,  761, 3253 + offset,  860), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DCAMPS,										_R(3135 + offset,  656, 3234 + offset,  755), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ACVOLTS,										_R(3365 + offset, 1069, 3464 + offset, 1168), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_VHFANTENNAROTARY,      						_R(2593 + offset,  184, 2683 + offset,  274), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSTESTSWITCH,								_R(2714 + offset,  711, 2748 + offset,  740), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSOXIDFLOWVALVESWITCHES,      				_R(2640 + offset,  798, 2717 + offset,  827), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSPUGMODESWITCH,      						_R(2726 + offset,  798, 2760 + offset,  827), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSSWITCHES,      							_R(2593 + offset,  955, 2791 + offset,  984), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DCINDICATORSROTARY,      					_R(3180 + offset,  896, 3270 + offset,  986), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_BATTERYCHARGEROTARY,      					_R(3336 + offset,  896, 3426 + offset,  986), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SBANDAUXSWITCHES,      						_R(2877 + offset, 1050, 2964 + offset, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_UPTLMSWITCHES,								_R(2983 + offset, 1050, 3070 + offset, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SBANDANTENNASWITCHES,      					_R(2593 + offset, 1155, 2670 + offset, 1184), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_VHFAMTHUMBWHEELS,      						_R(2745 + offset, 1149, 2762 + offset, 1283), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_VHFSWITCHES,      							_R(2838 + offset, 1152, 3060 + offset, 1181), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_TAPERECORDERSWITCHES,      					_R(2838 + offset, 1250, 2958 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_POWERSWITCHES,      							_R(2967 + offset, 1250, 3044 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PCMBITRATESWITCH,							_R(3053 + offset, 1250, 3130 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ACINVERTERSWITCHES,							_R(3182 + offset, 1050, 3345 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ACINDICATORROTARY,							_R(3386 + offset, 1205, 3476 + offset, 1295), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSOXIDFLOWVALVEINDICATOR,					_R(2605 + offset,  792, 2627 + offset,  834), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSOXIDUNBALMETER,       					_R(2619 + offset,  696, 2679 + offset,  757), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSHELIUMVALVEINDICATORS,    				_R(2598 + offset,  896, 2664 + offset,  919), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSMETERS,       		    				_R(2583 + offset,  319, 2733 + offset,  443), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_OXID_PERCENT_DISPLAY,					_R(2664 + offset,  628, 2702 + offset,  641), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_FUEL_PERCENT_DISPLAY,					_R(2664 + offset,  657, 2702 + offset,  670), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_INJ_VLV,                 				_R(2596 + offset,  523, 2774 + offset,  557), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
}

void Saturn::AddLeftLowerPanelAreas()
{
	// Panel 100
	oapiRegisterPanelArea (AID_PANEL100SWITCHES,      						_R( 613,  678,  964,  747), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PANEL100LIGHTINGROTARIES,					_R( 602,  813,  946,  903), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
		
		
	// Panel 163
	oapiRegisterPanelArea (AID_SCIUTILPOWERSWITCH,      					_R( 300, 1953,  334, 1982), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
}
void Saturn::AddLeftCenterLowerPanelAreas(int offset)
{
	// Panel 101
	oapiRegisterPanelArea (AID_DCVOLTS_PANEL101,							_R(1121 + offset,  65, 1231 + offset,  175), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SYSTEMTESTROTARIES,							_R(1069 + offset,  213, 1280 + offset,  304), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RNDZXPDRSWITCH,      						_R(1218 + offset,  350, 1252 + offset,  379), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PANEL101LOWERSWITCHES,      				    _R(1093 + offset,  486, 1251 + offset,  515), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
}
void Saturn::AddCenterLowerPanelAreas(int offset)
{
	MFDSPEC mfds_gnleft  =     {{ 1140 + 49 + offset,  1780 + 15, 1140 + 308 + offset, 1780 + 270}, 6, 6, 37, 37};
	MFDSPEC mfds_gnuser1 =     {{ 1510 + 49 + offset,  1780 + 15, 1510 + 308 + offset, 1780 + 270}, 6, 6, 37, 37};
	MFDSPEC mfds_gnuser2 =     {{ 1880 + 49 + offset,  1780 + 15, 1880 + 308 + offset, 1780 + 270}, 6, 6, 37, 37};
	MFDSPEC mfds_gnright =     {{ 2250 + 49 + offset,  1780 + 15, 2250 + 308 + offset, 1780 + 270}, 6, 6, 37, 37};
	oapiRegisterMFD(MFD_LEFT, mfds_gnleft);
	oapiRegisterMFD(MFD_USER1, mfds_gnuser1);
	oapiRegisterMFD(MFD_USER2, mfds_gnuser2);
	oapiRegisterMFD(MFD_RIGHT, mfds_gnright);		
	oapiRegisterPanelArea (AID_MFDGNLEFT,									_R(1140 + offset, 1780, 1140 + 359 + offset, 1780 + 300), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MFDGNUSER1,									_R(1510 + offset, 1780, 1510 + 359 + offset, 1780 + 300), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MFDGNUSER2,									_R(1880 + offset, 1780, 1880 + 359 + offset, 1780 + 300), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MFDGNRIGHT,									_R(2250 + offset, 1780, 2250 + 359 + offset, 1780 + 300), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_MASTER_ALARM3,								_R(2104 + offset, 1036, 2149 + offset, 1072), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_GNMODESWITCH,								_R(1365 + offset,  951, 1399 + offset,  980), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CONTROLLERSPEEDSWITCH,						_R(1496 + offset,  951, 1530 + offset,  980), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,					_R(1605 + offset,  951, 1639 + offset,  980), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CONTORLLERSWITCHES,							_R(1496 + offset, 1090, 1639 + offset, 1119), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CONDITIONLAMPSSWITCH,						_R(2230 + offset,  966, 2264 + offset,  995), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_UPLINKTELEMETRYSWITCH,						_R(2230 + offset, 1106, 2264 + offset, 1153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LOWEREQUIPMENTBAYCWLIGHTS,					_R(2100 + offset,  923, 2154 + offset,  999), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LOWEREQUIPMENTBAYOPTICSLIGHTS,				_R(1363 + offset, 1054, 1437 + offset, 1153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	PANEL_MAP_BACKGROUND);

	// "Accelerator" areas
	oapiRegisterPanelArea (AID_SWITCHTO_TELESCOPE1,      				    _R(2030 + offset,  580, 2165 + offset,  700), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SWITCHTO_TELESCOPE2,      				    _R(2100 + offset, 1190, 2360 + offset, 1445), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SWITCHTO_SEXTANT1,	     				    _R(1620 + offset,  585, 1760 + offset,  690), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SWITCHTO_SEXTANT2,	      				    _R(1340 + offset, 1190, 1605 + offset, 1445), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
}
void Saturn::AddRightCenterLowerPanelAreas(int offset)
{
	//
	// Lower panel uses the second DSKY.
	//
	oapiRegisterPanelArea (AID_DSKY2_DISPLAY,								_R(2602 + offset,  700, 2707 + offset,  876), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY2_LIGHTS,								_R(2458 + offset,  705, 2560 + offset,  825), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY2_KEY,			                        _R(2438 + offset,  895, 2725 + offset, 1016), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
}
void Saturn::AddRightLowerPanelAreas(int offset)
{
	return;
}
//
// Wire up all the control panel switches. Note that this can
// be called multiple times, so be sure to initialise all the panel
// and row variables at the start of the function to reset internal
// state..
//

void Saturn::SetSwitches(int panel) {

	MainPanel.Init(0, this, &soundlib, this);

	//
	// SATPANEL_MAIN
	//

	AccelGMeterRow.Init(AID_GMETER, MainPanel, &GaugePower);
	AccelGMeter.Init(g_Param.pen[4], g_Param.pen[4], AccelGMeterRow, this);

	THCRotaryRow.Init(AID_THC, MainPanel);
	THCRotary.Init(0, 0, 72, 109, srf[SRF_THC], 0, THCRotaryRow, this);

	SequencerSwitchesRow.Init(AID_SEQUENCERSWITCHES, MainPanel);
	LiftoffNoAutoAbortSwitch.Init     ( 20,   3, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 81);
	LiftoffNoAutoAbortSwitch.InitGuard(  0,   1, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	LesMotorFireSwitch.Init			  ( 20,  49, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, this, 0, 119, 117, 231);
	LesMotorFireSwitch.InitGuard      (  0,  47, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	CanardDeploySwitch.Init           ( 20,  95, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 157, 99, 281);
	CanardDeploySwitch.InitGuard      (  0,  93, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	CsmLvSepSwitch.Init				  ( 20, 141, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 195, 99, 319);
	CsmLvSepSwitch.InitGuard		  (  0, 139, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	ApexCoverJettSwitch.Init          (127,   3, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 233, 195, 193);
	ApexCoverJettSwitch.InitGuard     ( 94,   1, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);
	DrogueDeploySwitch.Init			  (127,  49, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 271, 195, 231);
	DrogueDeploySwitch.InitGuard      ( 94,  47, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);
	MainDeploySwitch.Init             (127,  95, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 309, 195, 269);
	MainDeploySwitch.InitGuard        ( 94,  93, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);
	CmRcsHeDumpSwitch.Init			  (127, 141, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 347, 195, 307);
	CmRcsHeDumpSwitch.InitGuard		  ( 94, 139, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);

	SeparationSwitchesRow.Init(AID_SEPARATIONSWITCHES, MainPanel);
	EDSSwitch.Init				  (  0,	23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CsmLmFinalSep1Switch.Init	  ( 43, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CsmLmFinalSep1Switch.InitGuard( 42,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	CsmLmFinalSep2Switch.Init	  ( 87, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CsmLmFinalSep2Switch.InitGuard( 86,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	CmSmSep1Switch.Init			  (131, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow, &CMSMPyros, 0);
	CmSmSep1Switch.InitGuard      (130,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61], 72);
	CmSmSep1Switch.SetFailed(SwitchFail.SMJett1Fail != 0);
	CmSmSep1Switch.WireTo(&PyroPower);
	CmSmSep2Switch.Init			  (175, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow, &CMSMPyros, 0);
	CmSmSep2Switch.InitGuard      (174,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61], 72);
	CmSmSep2Switch.SetFailed(SwitchFail.SMJett2Fail != 0);
	CmSmSep2Switch.WireTo(&PyroPower);

	if (!SkylabCM) {
		SivbLmSepSwitch.Init		  (219, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
		SivbLmSepSwitch.InitGuard     (218,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	}

	CryoTankSwitchesRow.Init(AID_CYROTANKSWITCHES, MainPanel);
	CabinFan1Switch.Init (  0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       CryoTankSwitchesRow);
	CabinFan2Switch.Init ( 59, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       CryoTankSwitchesRow);
	H2Heater1Switch.Init (114, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	H2Heater2Switch.Init (157, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Heater1Switch.Init (200, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Heater2Switch.Init (250, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2PressIndSwitch.Init(293, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       CryoTankSwitchesRow);
	H2Fan1Switch.Init    (349, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	H2Fan2Switch.Init    (413, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Fan1Switch.Init    (478, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Fan2Switch.Init    (541, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);

	SCContCMCModeSwitchesRow.Init(AID_SCCONTCMCMODESWITCHES, MainPanel);
	SCContSwitch.Init(  0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SCContCMCModeSwitchesRow, this);
	CMCModeSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SCContCMCModeSwitchesRow, &agc);
	
	SCSTvcSwitchesRow.Init(AID_SCSTVCSWITCHES, MainPanel);
	SCSTvcPitchSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SCSTvcSwitchesRow);
	SCSTvcYawSwitch.Init  (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SCSTvcSwitchesRow);
	
	SPSGimbalMotorsRow.Init(AID_SPSGIMBALMOTORSSWITCHES, MainPanel);
	Pitch1Switch.Init(  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);
	Pitch2Switch.Init( 49, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);
	Yaw1Switch.Init  ( 98, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);
	Yaw2Switch.Init  (148, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);

	EntrySwitchesRow.Init(AID_ENTRYSWITCHES, MainPanel);
	EMSRollSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EntrySwitchesRow); 
	GSwitch.Init    (43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EntrySwitchesRow); 
	
	LVSPSIndSwitchesRow.Init(AID_LVSPSINDICATORSWITCHES, MainPanel);
	LVSPSPcIndicatorSwitch.Init        ( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVSPSIndSwitchesRow); 
	LVFuelTankPressIndicatorSwitch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVSPSIndSwitchesRow);
	
	TVCGimbalDriveSwitchesRow.Init(AID_TVCGIMBALDRIVESWITCHES, MainPanel);
	TVCGimbalDrivePitchSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TVCGimbalDriveSwitchesRow);
	TVCGimbalDriveYawSwitch.Init  (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TVCGimbalDriveSwitchesRow);

	CSMLightSwitchesRow.Init(AID_CSMLIGHTSWITCHES, MainPanel);
	RunEVALightSwitch.Init(  0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CSMLightSwitchesRow);
	RndzLightSwitch.Init  ( 57, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CSMLightSwitchesRow);
	TunnelLightSwitch.Init(111, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CSMLightSwitchesRow);

	LMPowerSwitchRow.Init(AID_LMPOWERSWITCH, MainPanel);
	LMPowerSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], LMPowerSwitchRow);

	PostLDGVentValveLeverRow.Init(AID_POSTLANDINGVENTVALVELEVER, MainPanel);
	PostLDGVentValveLever.Init( 0, 0, 50, 158, srf[SRF_POSTLDGVENTVLVLEVER], srf[SRF_BORDER_50x158], PostLDGVentValveLeverRow);

	GDCAlignButtonRow.Init(AID_GDCALIGNBUTTON, MainPanel);
	GDCAlignButton.Init( 0, 0, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], GDCAlignButtonRow, 157, 119);

	HighGainAntennaUpperSwitchesRow.Init(AID_HIGHGAINANTENNAUPPERSWITCHES, MainPanel);
	GHATrackSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], HighGainAntennaUpperSwitchesRow); 
	GHABeamSwitch.Init (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], HighGainAntennaUpperSwitchesRow);
	
	HighGainAntennaLowerSwitchesRow.Init(AID_HIGHGAINANTENNALOWERSWITCHES, MainPanel);
	GHAPowerSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], HighGainAntennaLowerSwitchesRow); 
	GHAServoElecSwitch.Init (43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], HighGainAntennaLowerSwitchesRow);
	
	//
	// SM RCS Helium 1
	//

	SMRCSHelium1Row.Init(AID_SMRCS_HELIUM1_SWITCHES, MainPanel);
	SMRCSHelium1ASwitch.Init (0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row, this, CSM_He1_TANKA_VALVE, &SMRCSHelium1ATalkback);
	SMRCSHelium1BSwitch.Init (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row, this, CSM_He1_TANKB_VALVE, &SMRCSHelium1BTalkback);
	SMRCSHelium1CSwitch.Init (86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row, this, CSM_He1_TANKC_VALVE, &SMRCSHelium1CTalkback);
	SMRCSHelium1DSwitch.Init (129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row, this, CSM_He1_TANKD_VALVE, &SMRCSHelium1DTalkback);

	SMRCSHelium1TalkbackRow.Init(AID_RCS_HELIUM1_TALKBACK, MainPanel);
	SMRCSHelium1ATalkback.Init(0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, CSM_He1_TANKA_VALVE, this);
	SMRCSHelium1BTalkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, CSM_He1_TANKB_VALVE, this);
	SMRCSHelium1CTalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, CSM_He1_TANKC_VALVE, this);
	SMRCSHelium1DTalkback.Init(127, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, CSM_He1_TANKD_VALVE, this);

	//
	// SM RCS Helium 2
	//

	SMRCSHelium2Row.Init(AID_SMRCS_HELIUM2_SWITCHES, MainPanel);
	SMRCSHelium2ASwitch.Init (174, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row, this, CSM_He2_TANKA_VALVE, &SMRCSHelium2ATalkback);
	SMRCSHelium2BSwitch.Init (217, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row, this, CSM_He2_TANKB_VALVE, &SMRCSHelium2BTalkback);
	SMRCSHelium2CSwitch.Init (260, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row, this, CSM_He2_TANKC_VALVE, &SMRCSHelium2CTalkback);
	SMRCSHelium2DSwitch.Init (303, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row, this, CSM_He2_TANKD_VALVE, &SMRCSHelium2DTalkback);

	CMUplinkSwitch.Init(0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row, &agc);
	CMUplinkSwitch.SetChannelData(033, 10, true);	// Down is 'Block Uplink Input'

	if (!SkylabCM)
		IUUplinkSwitch.Init(43, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row);

	CMRCSPressSwitch.Init(87, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row);
	CMRCSPressSwitch.InitGuard(86, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	SMRCSIndSwitch.Init(131, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row);

	SMRCSHelium2TalkbackRow.Init(AID_RCS_HELIUM2_TALKBACK, MainPanel);
	SMRCSHelium2ATalkback.Init(0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, CSM_He2_TANKA_VALVE, this);
	SMRCSHelium2BTalkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, CSM_He2_TANKB_VALVE, this);
	SMRCSHelium2CTalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, CSM_He2_TANKC_VALVE, this);
	SMRCSHelium2DTalkback.Init(127, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, CSM_He2_TANKD_VALVE, this);

	//
	// RCS Gauges.
	//

	RCSGaugeRow.Init(AID_RCS_GAUGES, MainPanel, &GaugePower);
	RCSQuantityGauge.Init(srf[SRF_NEEDLE], RCSGaugeRow, &RCSIndicatorsSwitch, this);

	//
	// SM RCS Primary Propellant.
	//

	SMRCSProp1Row.Init(AID_PRIM_PRPLNT_SWITCHES, MainPanel);

	SMRCSHeaterASwitch.Init (2, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);
	SMRCSHeaterBSwitch.Init (45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);
	SMRCSHeaterCSwitch.Init (88, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);
	SMRCSHeaterDSwitch.Init (131, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);

	SMRCSProp1ASwitch.Init (174, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, this, CSM_PRIFUEL_INSOL_VALVE_A,
		CSM_SECFUEL_INSOL_VALVE_A, CSM_PRIOXID_INSOL_VALVE_A, CSM_SECOXID_INSOL_VALVE_A, &SMRCSProp1ATalkback, &SMRCSProp2ATalkback);
	SMRCSProp1BSwitch.Init (217, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, this, CSM_PRIFUEL_INSOL_VALVE_B,
		CSM_SECFUEL_INSOL_VALVE_B, CSM_PRIOXID_INSOL_VALVE_B, CSM_SECOXID_INSOL_VALVE_B, &SMRCSProp1BTalkback, &SMRCSProp2BTalkback);
	SMRCSProp1CSwitch.Init (260, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, this, CSM_PRIFUEL_INSOL_VALVE_C,
		CSM_SECFUEL_INSOL_VALVE_C, CSM_PRIOXID_INSOL_VALVE_C, CSM_SECOXID_INSOL_VALVE_C, &SMRCSProp1CTalkback, &SMRCSProp2CTalkback);
	SMRCSProp1DSwitch.Init (303, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, this, CSM_PRIFUEL_INSOL_VALVE_D,
		CSM_SECFUEL_INSOL_VALVE_D, CSM_PRIOXID_INSOL_VALVE_D, CSM_SECOXID_INSOL_VALVE_D, &SMRCSProp1DTalkback, &SMRCSProp2DTalkback);

	SMRCSProp1TalkbackRow.Init(AID_RCS_PROP1_TALKBACK, MainPanel);
	SMRCSProp1ATalkback.Init(0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, CSM_PRIFUEL_INSOL_VALVE_A, 
		CSM_PRIOXID_INSOL_VALVE_A, this);
	SMRCSProp1BTalkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, CSM_PRIFUEL_INSOL_VALVE_B, 
		CSM_PRIOXID_INSOL_VALVE_B, this);
	SMRCSProp1CTalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, CSM_PRIFUEL_INSOL_VALVE_C, 
		CSM_PRIOXID_INSOL_VALVE_C, this);
	SMRCSProp1DTalkback.Init(127, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, CSM_PRIFUEL_INSOL_VALVE_D, 
		CSM_PRIOXID_INSOL_VALVE_D, this);
	//
	// SM RCS Secondary Propellant.
	//

	SMRCSProp2Row.Init(AID_SEC_PRPLT_SWITCHES, MainPanel);

	RCSCMDSwitch.Init (2, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row);
	RCSTrnfrSwitch.Init (45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row); 
	CMRCSIsolate1.Init (88, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row, this, CM_RCSPROP_TANKA_VALVE, &CMRCSIsolate1Talkback);
	CMRCSIsolate2.Init (131, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row, this, CM_RCSPROP_TANKB_VALVE, &CMRCSIsolate2Talkback);

	SMRCSProp2ASwitch.Init (174, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row, this, CSM_SECFUEL_PRESS_VALVE_A, 0);
	SMRCSProp2BSwitch.Init (217, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row, this, CSM_SECFUEL_PRESS_VALVE_B, 0);
	SMRCSProp2CSwitch.Init (260, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row, this, CSM_SECFUEL_PRESS_VALVE_B, 0);
	SMRCSProp2DSwitch.Init (303, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row, this, CSM_SECFUEL_PRESS_VALVE_B, 0);

	SMRCSProp2TalkbackRow.Init(AID_RCS_PROP2_TALKBACK, MainPanel);
	SMRCSProp2ATalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CSM_SECFUEL_INSOL_VALVE_A, 
		CSM_SECOXID_INSOL_VALVE_A, this);
	SMRCSProp2BTalkback.Init(128, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CSM_SECFUEL_INSOL_VALVE_B, 
		CSM_SECOXID_INSOL_VALVE_B, this);
	SMRCSProp2CTalkback.Init(171, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CSM_SECFUEL_INSOL_VALVE_C, 
		CSM_SECOXID_INSOL_VALVE_C, this);
	SMRCSProp2DTalkback.Init(213, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CSM_SECFUEL_INSOL_VALVE_D, 
		CSM_SECOXID_INSOL_VALVE_D, this);
	CMRCSIsolate1Talkback.Init(0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CM_RCSPROP_TANKA_VALVE, this);
	CMRCSIsolate2Talkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CM_RCSPROP_TANKB_VALVE, this);

	//
	// Attitude control, etc.
	//

	AttitudeControlRow.Init(AID_ATTITUDE_CONTROL_SWITCHES, MainPanel);
	ManualAttRollSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlRow);
	ManualAttPitchSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlRow);
	ManualAttYawSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlRow);
	LimitCycleSwitch.Init(129, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);
	AttDeadbandSwitch.Init(172, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);
	AttRateSwitch.Init(215, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);
	TransContrSwitch.Init(258, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);

	//
	// BMAG
	//

	BMAGRow.Init(AID_BMAG_SWITCHES, MainPanel);
	BMAGRollSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BMAGRow);
	BMAGPitchSwitch.Init(49, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BMAGRow);
	BMAGYawSwitch.Init(99, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BMAGRow);

	//
	// CMC Att switch.
	//

	CMCAttRow.Init(AID_CMC_SWITCH, MainPanel);
	CMCAttSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CMCAttRow);

	//
	// FDAI switches.
	//

	// *** DANGER WILL ROBINSON: FDAISourceSwitch and FDAISelectSwitch ARE REVERSED! ***

	FDAISwitchesRow.Init(AID_FDAI_SWITCHES, MainPanel);
	FDAIScaleSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FDAISwitchesRow);
	FDAISelectSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FDAISwitchesRow);
	FDAISourceSwitch.Init(142, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FDAISwitchesRow);
	FDAIAttSetSwitch.Init(185, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FDAISwitchesRow);

	//
	// Caution and Warning switches.
	//

	CautionWarningRow.Init(AID_CAUTIONWARNING_SWITCHES, MainPanel);
	MissionTimerSwitch.Init(190, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &MissionTimerDisplay);
	CautionWarningModeSwitch.Init(7, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);
	CautionWarningCMCSMSwitch.Init(55, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);
	CautionWarningPowerSwitch.Init(104, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);
	CautionWarningLightTestSwitch.Init(147, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);

	//
	// Mission Timer switches.
	//

	MissionTimerSwitchesRow.Init(AID_MISSION_TIMER_SWITCHES, MainPanel);
	MissionTimerHoursSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MissionTimerSwitchesRow, TIME_UPDATE_HOURS, &MissionTimerDisplay);
	MissionTimerMinutesSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MissionTimerSwitchesRow, TIME_UPDATE_MINUTES, &MissionTimerDisplay);
	MissionTimerSecondsSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MissionTimerSwitchesRow, TIME_UPDATE_SECONDS, &MissionTimerDisplay);

	//
	// IMU Cage switch.
	//

	IMUCageSwitchRow.Init(AID_IMU_CAGE_SWITCH, MainPanel);
	IMUGuardedCageSwitch.Init(1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], IMUCageSwitchRow, &imu);
	IMUGuardedCageSwitch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);

	//
	// RCS Indicators rotary switch.
	//

	RCSIndicatorsSwitchRow.Init(AID_RCS_INDICATORS, MainPanel);
	RCSIndicatorsSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], RCSIndicatorsSwitchRow);
	RCSIndicatorsSwitch.SetSource(1, &CMRCS1);
	RCSIndicatorsSwitch.SetSource(2, &CMRCS2);
	RCSIndicatorsSwitch.SetSource(3, &SMQuadARCS);
	RCSIndicatorsSwitch.SetSource(4, &SMQuadBRCS);
	RCSIndicatorsSwitch.SetSource(5, &SMQuadCRCS);
	RCSIndicatorsSwitch.SetSource(6, &SMQuadDRCS);

	//
	// ECS Indicators rotary switch.
	//

	ECSIndicatorsSwitchRow.Init(AID_ECS_INDICATOR_SWITCH, MainPanel);
	ECSIndicatorsSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], ECSIndicatorsSwitchRow);

	//
	// LV switches.
	//

	LVRow.Init(AID_LV_SWITCHES, MainPanel);
	LVGuidanceSwitch.Init	  ( 1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVRow);
	LVGuidanceSwitch.InitGuard( 0,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61], 180);

	if (!SkylabCM) {
		SIISIVBSepSwitch.Init     (48, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVRow);
		SIISIVBSepSwitch.InitGuard(47,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
		TLIEnableSwitch.Init      (96, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVRow, this);
	}

	//
	// ELS Switches.
	//

	ELSRow.Init(AID_ELS_SWITCHES, MainPanel);
	CGSwitch.Init(0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow, &agc);
	CGSwitch.SetChannelData(32, 11, true);	// LM Attached flag.
	ELSLogicSwitch.Init(44, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow, &ELSAutoSwitch, 0);
	ELSLogicSwitch.InitGuard(43, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	ELSLogicSwitch.WireTo(&SECSLogicPower);
	ELSAutoSwitch.Init(88, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	CMRCSLogicSwitch.Init(131, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow, &CMPropDumpSwitch, 0);
	CMRCSLogicSwitch.WireTo(&SwitchPower);
	CMPropDumpSwitch.Init(175, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	CMPropDumpSwitch.InitGuard(174, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	CPPropPurgeSwitch.Init(220, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	CPPropPurgeSwitch.InitGuard(219, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	CPPropPurgeSwitch.WireTo(&CMPropDumpSwitch);

	//
	// Event Timer Switches
	//

	EventTimerRow.Init(AID_EVENT_TIMER_SWITCHES, MainPanel);
	// FCSMSPSASwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EventTimerRow);
	// FCSMSPSBSwitch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EventTimerRow);
	EventTimerUpDownSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, &EventTimerDisplay);
	EventTimerControlSwitch.Init(129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, &EventTimerDisplay);
	EventTimerMinutesSwitch.Init(172, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, TIME_UPDATE_MINUTES, &EventTimerDisplay);
	EventTimerSecondsSwitch.Init(215, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, TIME_UPDATE_SECONDS, &EventTimerDisplay);

	//
	// Main chute release.
	//

	MainReleaseRow.Init(AID_MAIN_RELEASE_SWITCH, MainPanel);
	MainReleaseSwitch.Init(1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], MainReleaseRow);
	MainReleaseSwitch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);

	//
	// Abort switches.
	//

	AbortRow.Init(AID_ABORT_ROW, MainPanel);
	PropDumpAutoSwitch.Init(0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AbortRow);
	TwoEngineOutAutoSwitch.Init(49, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AbortRow);
	LVRateAutoSwitch.Init(110, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AbortRow);
	TowerJett1Switch.Init(169, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AbortRow);
	TowerJett1Switch.InitGuard(168, 0, 36, 69, srf[SRF_SWITCHGUARDS], 180);
	TowerJett1Switch.SetFailed(SwitchFail.TowerJett1Fail != 0);
	TowerJett2Switch.Init(217, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AbortRow);
	TowerJett2Switch.InitGuard(216, 0, 36, 69, srf[SRF_SWITCHGUARDS], 180);
	TowerJett2Switch.SetFailed(SwitchFail.TowerJett2Fail != 0);

	//
	// Rotational controller power switches.
	//

	RotContrPowerRow.Init(AID_ROT_PWR, MainPanel);
	RotPowerNormal1Switch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);
	RotPowerNormal2Switch.Init(59, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);
	RotPowerDirect1Switch.Init(118, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);
	RotPowerDirect2Switch.Init(186, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);

	//
	// dV Thrust switches.
	//

	dvThrustRow.Init(AID_dVTHRUST_SWITCHES, MainPanel);
	dVThrust1Switch.Init(1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], dvThrustRow, &PilotValveMnACircuitBraker, NULL);
	dVThrust1Switch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);
	dVThrust2Switch.Init(95, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], dvThrustRow, &PilotValveMnBCircuitBraker, NULL);
	dVThrust2Switch.InitGuard(94, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61]);

	//
	// Fuel Cell Switches.
	//

	FuelCellPhRadTempIndicatorsRow.Init(AID_FUELCELLPHRADTEMPINDICATORS, MainPanel);
	FuelCellPhIndicator.Init     (  0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellPhRadTempIndicatorsRow);
	FuelCellRadTempIndicator.Init(174, 0, 23, 23, srf[SRF_INDICATOR], FuelCellPhRadTempIndicatorsRow);

	FuelCellRadiatorsIndicatorsRow.Init(AID_FUELCELLRADIATORSINDICATORS, MainPanel);
	FuelCellRadiators1Indicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);
	FuelCellRadiators2Indicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);
	FuelCellRadiators3Indicator.Init(86, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);

	FuelCellRadiatorsSwitchesRow.Init(AID_FUELCELLRADIATORSSWITCHES, MainPanel);
	FuelCellRadiators1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellRadiatorsSwitchesRow);
	FuelCellRadiators2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellRadiatorsSwitchesRow);
	FuelCellRadiators3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellRadiatorsSwitchesRow);

	FuelCellIndicatorsSwitchRow.Init(AID_FUELCELLINDICATORSSWITCH, MainPanel);
	FuelCellIndicatorsSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], FuelCellIndicatorsSwitchRow);

	FuelCellHeatersSwitchesRow.Init(AID_FUELCELLHEATERSSWITCHES, MainPanel);
	FuelCellHeater1Switch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellHeatersSwitchesRow);
	FuelCellHeater2Switch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellHeatersSwitchesRow);
	FuelCellHeater3Switch.Init(86, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellHeatersSwitchesRow);

	FuelCellPurgeSwitchesRow.Init(AID_FUELCELLPURGESWITCHES, MainPanel);
	FuelCellPurge1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);
	FuelCellPurge2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);
	FuelCellPurge3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);

	FuelCellReactantsIndicatorsRow.Init(AID_FUELCELLREACTANTSINDICATORS, MainPanel);
	FuelCellReactants1Indicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);
	FuelCellReactants2Indicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);
	FuelCellReactants3Indicator.Init(86, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);

	FuelCellReactantsSwitchesRow.Init(AID_FUELCELLREACTANTSSWITCHES, MainPanel);
	FuelCellReactants1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);
	FuelCellReactants2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);
	FuelCellReactants3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);

	FuelCellLatchSwitchesRow.Init(AID_FUELCELLLATCHSWITCHES, MainPanel);
	FCReacsValvesSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellLatchSwitchesRow);
	H2PurgeLineSwitch.Init  (43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellLatchSwitchesRow);

	SPSRow.Init(AID_SPS, MainPanel);
	SPSswitch.Init(0, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SPSRow, this);

	SPSGimbalPitchThumbwheelRow.Init(AID_SPSGIMBALPITCHTHUMBWHEEL, MainPanel);
	SPSGimbalPitchThumbwheel.Init(0, 0, 17, 36, srf[SRF_THUMBWHEEL_GPI_PITCH], SPSGimbalPitchThumbwheelRow);

	SPSGimbalYawThumbwheelRow.Init(AID_SPSGIMBALYAWTHUMBWHEEL, MainPanel);
	SPSGimbalYawThumbwheel.Init(0, 0, 36, 17, srf[SRF_THUMBWHEEL_GPI_YAW], SPSGimbalYawThumbwheelRow);

	//
	// Communication switches (s-band, vhf etc.)
	//

	TelecomTBRow.Init(AID_TELECOMTB, MainPanel);
	PwrAmplTB.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], TelecomTBRow);
	DseTapeTB.Init( 0,50, 23, 23, srf[SRF_INDICATOR], TelecomTBRow);

	SBandAuxSwitchesRow.Init(AID_SBANDAUXSWITCHES, MainPanel);
	SBandAuxSwitch1.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAuxSwitchesRow);
	SBandAuxSwitch2.Init(53, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAuxSwitchesRow);

	UPTLMSwitchesRow.Init(AID_UPTLMSWITCHES, MainPanel);
	UPTLMSwitch1.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], UPTLMSwitchesRow);
	UPTLMSwitch2.Init(53, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], UPTLMSwitchesRow);

	SBandAntennaSwitchesRow.Init(AID_SBANDANTENNASWITCHES, MainPanel);
	SBandAntennaSwitch1.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAntennaSwitchesRow);
	SBandAntennaSwitch2.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAntennaSwitchesRow);

	VHFAmThumbwheelsRow.Init(AID_VHFAMTHUMBWHEELS, MainPanel);
	SquelchAThumbwheel.Init(0,  0, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], VHFAmThumbwheelsRow);
	SquelchBThumbwheel.Init(0, 98, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], VHFAmThumbwheelsRow);

	VHFSwitchesRow.Init(AID_VHFSWITCHES, MainPanel);
	VHFAMASwitch.Init    (  0,  0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	VHFAMBSwitch.Init    ( 51,  0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	RCVOnlySwitch.Init   (102,  0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	VHFBeaconSwitch.Init (145,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	VHFRangingSwitch.Init(188,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], VHFSwitchesRow);

	TapeRecorderSwitchesRow.Init(AID_TAPERECORDERSWITCHES, MainPanel);
	TapeRecorder1Switch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], TapeRecorderSwitchesRow);
	TapeRecorder2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TapeRecorderSwitchesRow);
	TapeRecorder3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TapeRecorderSwitchesRow);

	PowerSwitchesRow.Init(AID_POWERSWITCHES, MainPanel);
	SCESwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], PowerSwitchesRow);
	PMPSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], PowerSwitchesRow);

	PCMBitRateSwitchRow.Init(AID_PCMBITRATESWITCH, MainPanel);
	PCMBitRateSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], PCMBitRateSwitchRow);
	DummySwitch.Init     (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], PCMBitRateSwitchRow);

	ACInverterSwitchesRow.Init(AID_ACINVERTERSWITCHES, MainPanel);
	MnA1Switch.Init       (  0,   0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &InverterPower1MainACircuitBraker, 0);
	MnB2Switch.Init       ( 63,   0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &InverterPower2MainBCircuitBraker, 0);
	MnA3Switch.Init       (126,   0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &InverterPower3MainACircuitBraker, 0, &InverterPower3MainBCircuitBraker);
	AcBus1Switch1.Init    (  0, 101, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 1,1, this);
	AcBus1Switch2.Init    ( 43, 101, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 1,2, this);
	AcBus1Switch3.Init    ( 86, 101, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 1,3, this);
	AcBus1ResetSwitch.Init(129, 101, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &EPSSensorUnitAcBus1CircuitBraker, &EPSSensorUnitAcBus1CircuitBraker, 0);
	AcBus2Switch1.Init    (  0, 200, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 2,1, this);
	AcBus2Switch2.Init    ( 43, 200, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 2,2, this);
	AcBus2Switch3.Init    ( 86, 200, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 2,3, this);
	AcBus2ResetSwitch.Init(129, 200, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &EPSSensorUnitAcBus2CircuitBraker, &EPSSensorUnitAcBus2CircuitBraker, 0);
	
	MainBusASwitch1.Init(129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow, this, 1, &MainBusAController);
	MainBusASwitch2.Init(179, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow, this, 2, &MainBusAController);
	MainBusASwitch3.Init(230, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow, this, 3, &MainBusAController);
	MainBusAResetSwitch.Init(273, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow, &EPSSensorUnitDcBusACircuitBraker, &EPSSensorUnitDcBusACircuitBraker, 0);
	MainBusAIndicatorsRow.Init(ADI_MAINBUSAINDICATORS, MainPanel);
	MainBusAIndicator1.Init(0, 0, 23, 23, srf[SRF_INDICATOR], MainBusAIndicatorsRow, &MainBusAController, 1);
	MainBusAIndicator2.Init(43, 0, 23, 23, srf[SRF_INDICATOR], MainBusAIndicatorsRow, &MainBusAController, 2);
	MainBusAIndicator3.Init(86, 0, 23, 23, srf[SRF_INDICATOR], MainBusAIndicatorsRow, &MainBusAController, 3);

	MainBusBSwitch1.Init(129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow, this, 1, &MainBusBController);
	MainBusBSwitch2.Init(185, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow, this, 2, &MainBusBController);
	MainBusBSwitch3.Init(241, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow, this, 3, &MainBusBController);
	MainBusBResetSwitch.Init(297, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow, &EPSSensorUnitDcBusBCircuitBraker, &EPSSensorUnitDcBusBCircuitBraker, 0);
	MainBusBIndicator1.Init(129, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow, &MainBusBController, 1);
	MainBusBIndicator2.Init(172, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow, &MainBusBController, 2);
	MainBusBIndicator3.Init(215, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow, &MainBusBController, 3);

	SBandNormalSwitchesRow.Init(AID_SBAND_NORMAL_SWITCHES, MainPanel);
	SBandNormalXPDRSwitch.Init    (	  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalPwrAmpl1Switch.Init(  43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalPwrAmpl2Switch.Init(  86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalMode1Switch.Init   (	145, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalMode2Switch.Init   (	188, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalMode3Switch.Init   (	231, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],		 SBandNormalSwitchesRow);

	ACVoltMeterRow.Init(AID_ACVOLTS, MainPanel);
	CSMACVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], ACVoltMeterRow, &ACIndicatorRotary);
	CSMACVoltMeter.SetSurface(srf[SRF_ACVOLTS], 99, 98);

	DCVoltMeterRow.Init(AID_DCVOLTS, MainPanel);
	CSMDCVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], DCVoltMeterRow, &DCIndicatorsRotary);
	CSMDCVoltMeter.SetSurface(srf[SRF_DCVOLTS], 99, 98);

	DCAmpMeterRow.Init(AID_DCAMPS, MainPanel);
	DCAmpMeter.Init(g_Param.pen[4], g_Param.pen[4], DCAmpMeterRow, this, &DCIndicatorsRotary);
	DCAmpMeter.SetSurface(srf[SRF_DCAMPS], 99, 98);

	CryoTankMetersRow.Init(AID_CYROTANKINDICATORS, MainPanel, &GaugePower);
	H2Pressure1Meter.Init(1, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	H2Pressure2Meter.Init(2, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	O2Pressure1Meter.Init(1, srf[SRF_NEEDLE], CryoTankMetersRow, this, &O2PressIndSwitch);
	O2Pressure2Meter.Init(2, srf[SRF_NEEDLE], CryoTankMetersRow, this, &O2PressIndSwitch);
	H2Quantity1Meter.WireTo(&ACBus1PhaseC);
	H2Quantity1Meter.Init("H2", 1, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	H2Quantity2Meter.WireTo(&ACBus2PhaseC);
	H2Quantity2Meter.Init("H2", 2, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	O2Quantity1Meter.WireTo(&ACBus1PhaseC);
	O2Quantity1Meter.Init("O2", 1, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	O2Quantity2Meter.WireTo(&ACBus2PhaseC);
	O2Quantity2Meter.Init("O2", 2, srf[SRF_NEEDLE], CryoTankMetersRow, this);

	FuelCellMetersRow.Init(AID_FUELCELLINDICATORS, MainPanel, &GaugePower);
	FuelCellH2FlowMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);
	FuelCellO2FlowMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);
	FuelCellTempMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);
	FuelCellCondenserTempMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);

	CabinMetersRow.Init(AID_CABININDICATORS, MainPanel, &GaugePower);
	SuitTempMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	CabinTempMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	SuitPressMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	CabinPressMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	PartPressCO2Meter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);

	SuitComprDeltaPMeterRow.Init(AID_SUITCOMPRDELTAPMETER, MainPanel, &GaugePower);
	SuitComprDeltaPMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitComprDeltaPMeterRow, this);
	LeftO2FlowMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitComprDeltaPMeterRow, this);

	SuitCabinDeltaPMeterRow.Init(AID_SUITCABINDELTAPMETER, MainPanel, &GaugePower);
	SuitCabinDeltaPMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitCabinDeltaPMeterRow, this);
	SuitCabinDeltaPMeter.FrameSurface = srf[SRF_SUITCABINDELTAPMETER];
	RightO2FlowMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitCabinDeltaPMeterRow, this);
	RightO2FlowMeter.FrameSurface = srf[SRF_SUITCABINDELTAPMETER];

	EcsRadTempMetersRow.Init(AID_ECSRADTEMPMETERS, MainPanel, &GaugePower);
	EcsRadTempInletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsRadTempMetersRow, this, &ECSIndicatorsSwitch);
	EcsRadTempPrimOutletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsRadTempMetersRow, this);

	EcsEvapTempMetersRow.Init(AID_ECSEVAPTEMPMETERS, MainPanel, &GaugePower);
	EcsRadTempSecOutletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsEvapTempMetersRow, this);
	GlyEvapTempOutletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsEvapTempMetersRow, this, &ECSIndicatorsSwitch);

	EcsPressMetersRow.Init(AID_ECSPRESSMETERS, MainPanel, &GaugePower);
	GlyEvapSteamPressMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsPressMetersRow, this, &ECSIndicatorsSwitch);
	GlycolDischPressMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsPressMetersRow, this, &ECSIndicatorsSwitch);

	EcsQuantityMetersRow.Init(AID_ECSQUANTITYMETERS, MainPanel, &GaugePower);
	AccumQuantityMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsQuantityMetersRow, this, &ECSIndicatorsSwitch);
	H2oQuantityMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsQuantityMetersRow, this, &H2oQtyIndSwitch);

	EcsRadiatorIndicatorRow.Init(AID_ECSRADIATORINDICATOR, MainPanel, &GaugePower);
	EcsRadiatorIndicator.Init( 0, 0, 23, 23, srf[SRF_ECSINDICATOR], EcsRadiatorIndicatorRow);

	EcsRadiatorSwitchesRow.Init(AID_ECSRADIATORSWITCHES, MainPanel);
	EcsRadiatorsFlowContAutoSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsFlowContPwrSwitch.Init( 50, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsManSelSwitch.Init(     100, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsHeaterPrimSwitch.Init( 150, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsHeaterSecSwitch.Init(  193, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsRadiatorSwitchesRow);

	EcsSwitchesRow.Init(AID_ECSSWITCHES, MainPanel);
	PotH2oHtrSwitch.Init                   (  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SuitCircuitH2oAccumAutoSwitch.Init     ( 43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SuitCircuitH2oAccumOnSwitch.Init       ( 86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SuitCircuitHeatExchSwitch.Init         (129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SecCoolantLoopEvapSwitch.Init          (172, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SecCoolantLoopPumpSwitch.Init          (221, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow, &ECSSecCoolLoopAc1CircuitBraker, NULL, &ECSSecCoolLoopAc2CircuitBraker);
	H2oQtyIndSwitch.Init                   (270, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);
	GlycolEvapTempInSwitch.Init            (313, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);
	GlycolEvapSteamPressAutoManSwitch.Init (364, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);
	GlycolEvapSteamPressIncrDecrSwitch.Init(411, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	GlycolEvapH2oFlowSwitch.Init           (456, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	CabinTempAutoManSwitch.Init            (506, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);

	CabinTempAutoControlSwitchRow.Init(AID_CABINTEMPAUTOCONTROLSWITCH, MainPanel);
	CabinTempAutoControlSwitch.Init(0, 0, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], CabinTempAutoControlSwitchRow);

	EcsGlycolPumpsSwitchRow.Init(AID_ECSGLYCOLPUMPSSWITCH, MainPanel);
	EcsGlycolPumpsSwitch.Init(0, 0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_84x84], EcsGlycolPumpsSwitchRow,
		                      (Pump *) Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP"),
							  &ECSGlycolPumpsAc1ACircuitBraker, &ECSGlycolPumpsAc1BCircuitBraker, &ECSGlycolPumpsAc1CCircuitBraker,
							  &ECSGlycolPumpsAc2ACircuitBraker, &ECSGlycolPumpsAc2BCircuitBraker, &ECSGlycolPumpsAc2CCircuitBraker);

	HighGainAntennaPitchPositionSwitchRow.Init(AID_HIGHGAINANTENNAPITCHPOSITIONSWITCH, MainPanel);
	HighGainAntennaPitchPositionSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], HighGainAntennaPitchPositionSwitchRow);

	HighGainAntennaYawPositionSwitchRow.Init(AID_HIGHGAINANTENNAYAWPOSITIONSWITCH, MainPanel);
	HighGainAntennaYawPositionSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], HighGainAntennaYawPositionSwitchRow);
	
	VHFAntennaRotaryRow.Init(AID_VHFANTENNAROTARY, MainPanel);
	VHFAntennaRotarySwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], VHFAntennaRotaryRow);
	
	//
	// SPS switches (panel 3)
	//
	
	SPSInjectorValveIndicatorsRow.Init(AID_SPS_INJ_VLV, MainPanel);
	SPSInjectorValve1Indicator.Init(  0, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);
	SPSInjectorValve2Indicator.Init( 48, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);
	SPSInjectorValve3Indicator.Init( 96, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);
	SPSInjectorValve4Indicator.Init(144, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);

	SPSTestSwitchRow.Init(AID_SPSTESTSWITCH, MainPanel);
	SPSTestSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSTestSwitchRow);

	SPSOxidFlowValveIndicatorsRow.Init(AID_SPSOXIDFLOWVALVEINDICATOR, MainPanel);
	SPSOxidFlowValveMaxIndicator.Init(0,  0, 22, 12, srf[SRF_SPSMAXINDICATOR], SPSOxidFlowValveIndicatorsRow);
	SPSOxidFlowValveMinIndicator.Init(0, 30, 22, 12, srf[SRF_SPSMININDICATOR], SPSOxidFlowValveIndicatorsRow);

	SPSOxidFlowValveSwitchesRow.Init(AID_SPSOXIDFLOWVALVESWITCHES, MainPanel);
	SPSOxidFlowValveSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSOxidFlowValveSwitchesRow);
	SPSOxidFlowValveSelectorSwitch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SPSOxidFlowValveSwitchesRow);

	SPSPugModeSwitchRow.Init(AID_SPSPUGMODESWITCH, MainPanel);
	SPSPugModeSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSPugModeSwitchRow);
	
	SPSHeliumValveIndicatorsRow.Init(AID_SPSHELIUMVALVEINDICATORS, MainPanel);
	SPSHeliumValveAIndicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], SPSHeliumValveIndicatorsRow);
	SPSHeliumValveBIndicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], SPSHeliumValveIndicatorsRow);

	SPSSwitchesRow.Init(AID_SPSSWITCHES, MainPanel);
	SPSHeliumValveASwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);
	SPSHeliumValveBSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);
	SPSLineHTRSSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);
	SPSPressIndSwitch.Init(164, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);

	SPSOxidPercentRow.Init(AID_SPS_OXID_PERCENT_DISPLAY, MainPanel);
	SPSOxidPercentMeter.Init(srf[SRF_SPS_FONT_BLACK], srf[SRF_SPS_FONT_WHITE], SPSOxidPercentRow, this);

	SPSFuelPercentRow.Init(AID_SPS_FUEL_PERCENT_DISPLAY, MainPanel);
	SPSFuelPercentMeter.Init(srf[SRF_SPS_FONT_BLACK], srf[SRF_SPS_FONT_WHITE], SPSFuelPercentRow, this);

	SPSOxidUnbalMeterRow.Init(AID_SPSOXIDUNBALMETER, MainPanel);
	SPSOxidUnbalMeter.Init(g_Param.pen[3], g_Param.pen[3], SPSOxidUnbalMeterRow, this);

	SPSMetersRow.Init(AID_SPSMETERS, MainPanel, &GaugePower);
	SPSTempMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this);
	SPSHeliumNitrogenPressMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this, &SPSPressIndSwitch);
	SPSFuelPressMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this, true);
	SPSOxidPressMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this, false);

	LVSPSPcMeterRow.Init(AID_THRUSTMETER, MainPanel, &GaugePower);
	LVSPSPcMeter.Init(g_Param.pen[4], g_Param.pen[4], LVSPSPcMeterRow, this, &LVSPSPcIndicatorSwitch, srf[SRF_THRUSTMETER]);

	GPFPIMeterRow.Init(AID_GPFPI_METERS, MainPanel);
	GPFPIPitch1Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, &LVFuelTankPressIndicatorSwitch, 15);
	GPFPIPitch2Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, &LVFuelTankPressIndicatorSwitch, 53);	
	GPFPIYaw1Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, &LVFuelTankPressIndicatorSwitch, 91);
	GPFPIYaw2Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, &LVFuelTankPressIndicatorSwitch, 129);

	DirectUllageThrustOnRow.Init(AID_DIRECT_ULLAGE_THRUST_ON, MainPanel);
	DirectUllageButton.Init( 0, 0, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], DirectUllageThrustOnRow, 79, 119);
	ThrustOnButton.Init( 0, 53, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], DirectUllageThrustOnRow, 79, 157);

	//
	// Electricals switches & indicators
	//

	DCIndicatorsRotaryRow.Init(AID_DCINDICATORSROTARY, MainPanel);
	DCIndicatorsRotary.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], DCIndicatorsRotaryRow);

	ACIndicatorRotaryRow.Init(AID_ACINDICATORROTARY, MainPanel);
	ACIndicatorRotary.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], ACIndicatorRotaryRow);
	
	BatteryChargeRotaryRow.Init(AID_BATTERYCHARGEROTARY, MainPanel);
	BatteryChargeRotary.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], BatteryChargeRotaryRow);
	
	//
	// Docking probe switches
	//
	DockingProbeSwitchesRow.Init(AID_DOCKINGPROBESWITCHES, MainPanel);
	DockingProbeExtdRelSwitch.Init(1, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], DockingProbeSwitchesRow);
	DockingProbeExtdRelSwitch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], 180);
	DockingProbeRetractPrimSwitch.Init(44, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], DockingProbeSwitchesRow);
	DockingProbeRetractSecSwitch.Init(87, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], DockingProbeSwitchesRow);

	DockingProbeIndicatorsRow.Init(AID_DOCKINGPROBEINDICATORS, MainPanel);
	DockingProbeAIndicator.Init(  0,  0, 23, 23, srf[SRF_INDICATOR], DockingProbeIndicatorsRow);
	DockingProbeBIndicator.Init(  0, 27, 23, 23, srf[SRF_INDICATOR], DockingProbeIndicatorsRow);

	//
	// EMS switches
	//

	EMSFunctionSwitchRow.Init(AID_EMSFUNCTIONSWITCH, MainPanel);
	EMSFunctionSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], EMSFunctionSwitchRow);

	EMSModeRow.Init(AID_ENTRY_MODE_SWITCH, MainPanel);
	EMSModeSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EMSModeRow);
	
	GTASwitchRow.Init(AID_GTASWITCH, MainPanel);
	GTASwitch.Init(10, 49, 34,  29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], GTASwitchRow);
	GTASwitch.InitGuard(0,  0, 55, 111, srf[SRF_GTACOVER], srf[SRF_BORDER_55x111]);
	
	EMSDvDisplayRow.Init(AID_EMSDVDISPLAY, MainPanel, &GaugePower);
	EMSDvDisplay.Init(srf[SRF_DIGITAL], EMSDvDisplayRow, this);

	//
	// SATPANEL_RIGHT
	//

	FuelCellPumpsSwitchesRow.Init(AID_FUELCELLPUMPSSWITCHES, MainPanel);
	FuelCellPumps1Switch.Init(  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPumpsSwitchesRow, &ACBus1, NULL, &ACBus2);
	FuelCellPumps2Switch.Init( 65, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPumpsSwitchesRow, &ACBus1, NULL, &ACBus2);
	FuelCellPumps3Switch.Init(130, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPumpsSwitchesRow, &ACBus1, NULL, &ACBus2);

	SuitCompressorSwitchesRow.Init(AID_SUITCOMPRESSORSWITCHES, MainPanel);
	SuitCompressor1Switch.Init( 1, 58, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], SuitCompressorSwitchesRow,
 							   &SuitCompressorsAc1ACircuitBraker, &SuitCompressorsAc1BCircuitBraker, &SuitCompressorsAc1CCircuitBraker,
							   &SuitCompressorsAc2ACircuitBraker, &SuitCompressorsAc2BCircuitBraker, &SuitCompressorsAc2CCircuitBraker);
	SuitCompressor2Switch.Init(42,  0, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], SuitCompressorSwitchesRow,
 							   &SuitCompressorsAc1ACircuitBraker, &SuitCompressorsAc1BCircuitBraker, &SuitCompressorsAc1CCircuitBraker,
							   &SuitCompressorsAc2ACircuitBraker, &SuitCompressorsAc2BCircuitBraker, &SuitCompressorsAc2CCircuitBraker);

	RightCOASPowerSwitchRow.Init(AID_RIGHTCOASSWITCH, MainPanel);
	RightCOASPowerSwitch.Init( 0, 0, 34, 31, srf[SRF_SWITCH20], srf[SRF_BORDER_34x31], RightCOASPowerSwitchRow);

	EpsSensorSignalDcCircuitBrakersRow.Init(AID_EPSSENSORSIGNALDCCIRCUITBRAKERS, MainPanel);
	EpsSensorSignalDcMnaCircuitBraker.Init( 0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalDcCircuitBrakersRow);
	EpsSensorSignalDcMnbCircuitBraker.Init(38, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalDcCircuitBrakersRow);

	EpsSensorSignalAcCircuitBrakersRow.Init(AID_EPSSENSORSIGNALACCIRCUITBRAKERS, MainPanel);
	EpsSensorSignalAc1CircuitBraker.Init(  0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalAcCircuitBrakersRow);
	EpsSensorSignalAc2CircuitBraker.Init(101, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalAcCircuitBrakersRow);

	CWCircuitBrakersRow.Init(AID_CWCIRCUITBRAKERS, MainPanel);
	CWMnaCircuitBraker.Init( 0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], CWCircuitBrakersRow, MainBusA, 5.0);
	CWMnbCircuitBraker.Init(38, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], CWCircuitBrakersRow, MainBusB, 5.0);

	LMPWRCircuitBrakersRow.Init(AID_LMPWRCIRCUITBRAKERS, MainPanel);
	MnbLMPWR1CircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], LMPWRCircuitBrakersRow);
	MnbLMPWR2CircuitBraker.Init(3, 78, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], LMPWRCircuitBrakersRow);

	InverterControlCircuitBrakersRow.Init(AID_INVERTERCONTROLCIRCUITBRAKERS, MainPanel);
	InverterControl1CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InverterControlCircuitBrakersRow, &MnA1Switch, 70.0);
	InverterControl2CircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InverterControlCircuitBrakersRow, &MnB2Switch, 70.0);
	InverterControl3CircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InverterControlCircuitBrakersRow, &MnA3Switch, 70.0);

	EPSSensorUnitCircuitBrakersRow.Init(AID_EPSSENSORUNITCIRCUITBRAKERS, MainPanel);
	EPSSensorUnitDcBusACircuitBraker.Init(  0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);
	EPSSensorUnitDcBusBCircuitBraker.Init( 37,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);
	EPSSensorUnitAcBus1CircuitBraker.Init( 74,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);
	EPSSensorUnitAcBus2CircuitBraker.Init(111,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);

	BATRLYBusCircuitBrakersRow.Init(AID_BATRLYBUSCIRCUITBRAKERS, MainPanel);
	BATRLYBusBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BATRLYBusCircuitBrakersRow, &BatteryBusA, 15.0);
	BATRLYBusBatBCircuitBraker.Init(37,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BATRLYBusCircuitBrakersRow, &BatteryBusB, 15.0);

	ECSRadiatorsCircuitBrakersRow.Init(AID_ECSRADIATORSCIRCUITBRAKERS, MainPanel);
	ControllerAc1CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	ControllerAc2CircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	CONTHTRSMnACircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	CONTHTRSMnBCircuitBraker.Init(172,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	HTRSOVLDBatACircuitBraker.Init(209,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	HTRSOVLDBatBCircuitBraker.Init(246,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);

	BatteryChargerCircuitBrakersRow.Init(AID_BATTERYCHARGERCIRCUITBRAKERS, MainPanel);
	BatteryChargerBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow);
	BatteryChargerBatBCircuitBraker.Init(37,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow);
	BatteryChargerMnACircuitBraker.Init( 74,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow, MainBusA);
	BatteryChargerMnBCircuitBraker.Init(111,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow, MainBusB);
	BatteryChargerAcPwrCircuitBraker.Init(192,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow, &BatCHGRSwitch);

	InstrumentLightingCircuitBrakersRow.Init(AID_INSTRUMENTLIGHTINGCIRCUITBRAKERS, MainPanel);
	InstrumentLightingESSMnACircuitBraker.Init(  0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingESSMnBCircuitBraker.Init( 38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingNonESSCircuitBraker.Init(133,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingSCIEquipSEP1CircuitBraker.Init(171,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingSCIEquipSEP2CircuitBraker.Init(209,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingSCIEquipHatchCircuitBraker.Init(247,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);

	ECSCircuitBrakersRow.Init(AID_ECSCIRCUITBRAKERS, MainPanel);
	ECSPOTH2OHTRMnACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSPOTH2OHTRMnBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSH2OAccumMnACircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSH2OAccumMnBCircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerWastePOTH2OMnACircuitBraker.Init(262,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerWastePOTH2OMnBCircuitBraker.Init(300,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerPressGroup1MnACircuitBraker.Init(338,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerPressGroup1MnBCircuitBraker.Init(376,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerPressGroup2MnACircuitBraker.Init(432,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerPressGroup2MnBCircuitBraker.Init(470,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerTempMnACircuitBraker.Init(508,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerTempMnBCircuitBraker.Init(546,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);

	ECSLowerRowCircuitBrakersRow.Init(AID_ECSLOWERROWCIRCUITBRAKERS, MainPanel);
	ECSSecCoolLoopAc1CircuitBraker.Init(  0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1);
	ECSSecCoolLoopAc2CircuitBraker.Init(102,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2);
	ECSSecCoolLoopRADHTRMnACircuitBraker.Init(186,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow);
	ECSSecCoolLoopXducersMnACircuitBraker.Init(224,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow);
	ECSSecCoolLoopXducersMnBCircuitBraker.Init(262,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow);
	ECSWasteH2OUrineDumpHTRMnACircuitBraker.Init(355,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow);
	ECSWasteH2OUrineDumpHTRMnBCircuitBraker.Init(393,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow);
	ECSCabinFanAC1ACircuitBraker.Init(585,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1PhaseA, 2.0);
	ECSCabinFanAC1BCircuitBraker.Init(622,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1PhaseB, 2.0);
	ECSCabinFanAC1CCircuitBraker.Init(659,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1PhaseC, 2.0);
	ECSCabinFanAC2ACircuitBraker.Init(696,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2PhaseA, 2.0);
	ECSCabinFanAC2BCircuitBraker.Init(733,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2PhaseB, 2.0);
	ECSCabinFanAC2CCircuitBraker.Init(771,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2PhaseC, 2.0);

	GNCircuitBrakersRow.Init(AID_GUIDANCENAVIGATIONCIRCUITBRAKERS, MainPanel);
	GNPowerAc1CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, &ACBus1, 2.0);
	GNPowerAc2CircuitBraker.Init(57,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, &ACBus2, 2.0);
	GNIMUMnACircuitBraker.Init(103,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 25.0);
	GNIMUMnBCircuitBraker.Init(140,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 25.0);
	GNIMUHTRMnACircuitBraker.Init(177,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 7.5);
	GNIMUHTRMnBCircuitBraker.Init(214,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 7.5);
	GNComputerMnACircuitBraker.Init(251,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 5.0);
	GNComputerMnBCircuitBraker.Init(288,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 5.0);
	GNOpticsMnACircuitBraker.Init(325,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 10.0);
	GNOpticsMnBCircuitBraker.Init(362,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 10.0);

	SuitCompressorsAc1ACircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC1ACIRCUITBRAKER, MainPanel);
	SuitCompressorsAc1ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc1ACircuitBrakerRow, &ACBus1PhaseA, 2.0);

	SuitCompressorsAc1BCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC1BCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc1BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc1BCircuitBrakerRow, &ACBus1PhaseB, 2.0);

	SuitCompressorsAc1CCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC1CCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc1CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc1CCircuitBrakerRow, &ACBus1PhaseC, 2.0);

	SuitCompressorsAc2ACircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC2ACIRCUITBRAKER, MainPanel);
	SuitCompressorsAc2ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc2ACircuitBrakerRow, &ACBus2PhaseA, 2.0);

	SuitCompressorsAc2BCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC2BCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc2BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc2BCircuitBrakerRow, &ACBus2PhaseB, 2.0);

	SuitCompressorsAc2CCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC2CCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc2CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc2CCircuitBrakerRow, &ACBus2PhaseC, 2.0);

	ECSGlycolPumpsAc1ACircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC1ACIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc1ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc1ACircuitBrakerRow, &ACBus1PhaseA, 2.0);

	ECSGlycolPumpsAc1BCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC1BCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc1BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc1BCircuitBrakerRow, &ACBus1PhaseB, 2.0);

	ECSGlycolPumpsAc1CCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC1CCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc1CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc1CCircuitBrakerRow, &ACBus1PhaseC, 2.0);

	ECSGlycolPumpsAc2ACircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC2ACIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc2ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc2ACircuitBrakerRow, &ACBus2PhaseA, 2.0);

	ECSGlycolPumpsAc2BCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC2BCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc2BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc2BCircuitBrakerRow, &ACBus2PhaseB, 2.0);
	
	ECSGlycolPumpsAc2CCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC2CCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc2CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc2CCircuitBrakerRow, &ACBus2PhaseC, 2.0);

	ModeIntercomVOXSensThumbwheelSwitchRow.Init(AID_MODEINTERCOMVOXSENSTHUMBWHEEL, MainPanel);
	ModeIntercomVOXSensThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], ModeIntercomVOXSensThumbwheelSwitchRow);

	PowerMasterVolumeThumbwheelSwitchRow.Init(AID_POWERMASTERVOLUMETHUMBWHEEL, MainPanel);
	PowerMasterVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], PowerMasterVolumeThumbwheelSwitchRow, VOLUME_COMMS, &soundlib);

	PadCommVolumeThumbwheelSwitchRow.Init(AID_PADCOMMVOLUMETHUMBWHEEL, MainPanel);
	PadCommVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], PadCommVolumeThumbwheelSwitchRow);

	IntercomVolumeThumbwheelSwitchRow.Init(AID_INTERCOMVOLUMETHUMBWHEEL, MainPanel);
	IntercomVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], IntercomVolumeThumbwheelSwitchRow);

	SBandVolumeThumbwheelSwitchRow.Init(AID_SBANDVOLUMETHUMBWHEEL, MainPanel);
	SBandVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], SBandVolumeThumbwheelSwitchRow);

	VHFAMVolumeThumbwheelSwitchRow.Init(AID_VHFVOLUMETHUMBWHEEL, MainPanel);
	VHFAMVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], VHFAMVolumeThumbwheelSwitchRow);

	AudioControlSwitchRow.Init(AID_AUDIOCONTROLSWITCH, MainPanel);
	AudioControlSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30], srf[SRF_BORDER_34x34], AudioControlSwitchRow);

	SuidPowerSwitchRow.Init(AID_SUITPOWERSWITCH, MainPanel);
	SuidPowerSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30], srf[SRF_BORDER_34x34], SuidPowerSwitchRow);

	RightUtilityPowerSwitchRow.Init(AID_RIGHTUTILITYPOWERSWITCH, MainPanel);
	RightUtilityPowerSwitch.Init(0, 0, 34, 31, srf[SRF_SWITCH20], srf[SRF_BORDER_34x31], RightUtilityPowerSwitchRow);

	RightDockingTargetSwitchRow.Init(AID_RIGHTDOCKINGTARGETSWITCH, MainPanel);
	RightDockingTargetSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH20], srf[SRF_BORDER_34x31], RightDockingTargetSwitchRow);

	RightModeIntercomSwitchRow.Init(AID_RIGHTMODEINTERCOMSWITCH, MainPanel);
	RightModeIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightModeIntercomSwitchRow);

	RightAudioPowerSwitchRow.Init(AID_RIGHTAUDIOPOWERSWITCH, MainPanel);
	RightAudioPowerSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightAudioPowerSwitchRow);

	RightPadCommSwitchRow.Init(AID_RIGHTPADCOMMSWITCH, MainPanel);
	RightPadCommSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightPadCommSwitchRow);

	RightIntercomSwitchRow.Init(AID_RIGHTINTERCOMSWITCH, MainPanel);
	RightIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightIntercomSwitchRow);

	RightSBandSwitchRow.Init(AID_RIGHTSBANDSWITCH, MainPanel);
	RightSBandSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightSBandSwitchRow);

	RightVHFAMSwitchRow.Init(AID_RIGHTVHFAMSWITCH, MainPanel);
	RightVHFAMSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightVHFAMSwitchRow);

	GNPowerSwitchRow.Init(AID_GNPOWERSWITCH, MainPanel);
	GNPowerSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], GNPowerSwitchRow);

	MainBusTieSwitchesRow.Init(AID_MAINBUSTIESWITCHES, MainPanel);
	MainBusTieBatAcSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MainBusTieSwitchesRow);
	MainBusTieBatBcSwitch.Init(45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MainBusTieSwitchesRow);

	BatCHGRSwitchRow.Init(AID_BATTERYCHARGERSWITCH, MainPanel);
	BatCHGRSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BatCHGRSwitchRow, &ACBus1, NULL, &ACBus2);

	NonessBusSwitchRow.Init(AID_NONESSBUSSWITCH, MainPanel);
	NonessBusSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], NonessBusSwitchRow, MainBusA, 0, MainBusB);

	InteriorLightsFloodSwitchesRow.Init(AID_INTERIORLIGHTSFLOODSSWITCHES, MainPanel);
	InteriorLightsFloodDimSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], InteriorLightsFloodSwitchesRow);
	InteriorLightsFloodFixedSwitch.Init(45, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], InteriorLightsFloodSwitchesRow);

	SPSGaugingSwitchRow.Init(AID_SPSGAUGINGSWITCH, MainPanel);
	SPSGaugingSwitch.Init(0, 0, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], SPSGaugingSwitchRow, &GaugingAc1CircuitBraker, NULL, &GaugingAc2CircuitBraker);

	TelcomSwitchesRow.Init(AID_TELCOMSWITCHES, MainPanel);
	TelcomGroup1Switch.Init(0, 78, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], TelcomSwitchesRow, &ACBus1, NULL, &ACBus2);
	TelcomGroup2Switch.Init(56, 0, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], TelcomSwitchesRow, &ACBus1, NULL, &ACBus2);

	RightInteriorLightRotariesRow.Init(AID_RIGHTINTERIORLIGHTROTARIES, MainPanel);
	RightIntegralRotarySwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], RightInteriorLightRotariesRow);
	RightFloodRotarySwitch.Init( 133,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], RightInteriorLightRotariesRow);

	SystemTestRotariesRow.Init(AID_SYSTEMTESTROTARIES, MainPanel);
	LeftSystemTestRotarySwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], SystemTestRotariesRow);
	RightSystemTestRotarySwitch.Init(120, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], SystemTestRotariesRow);

	SystemTestMeterRow.Init(AID_DCVOLTS_PANEL101, MainPanel);
	SystemTestVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], SystemTestMeterRow, &LeftSystemTestRotarySwitch);
	SystemTestVoltMeter.SetSurface(srf[SRF_DCVOLTS_PANEL101], 110, 110);

	RNDZXPDRSwitchRow.Init(AID_RNDZXPDRSWITCH, MainPanel);
	RNDZXPDRSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RNDZXPDRSwitchRow);
	
	Panel101LowerSwitchRow.Init(AID_PANEL101LOWERSWITCHES, MainPanel);
    CMRCSHTRSSwitch.Init   (  0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel101LowerSwitchRow);
	WasteH2ODumpSwitch.Init( 78, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel101LowerSwitchRow);
	UrineDumpSwitch.Init   (124, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel101LowerSwitchRow);
	
	Panel100SwitchesRow.Init(AID_PANEL100SWITCHES, MainPanel);
	UtilityPowerSwitch.Init      (  0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	Panel100FloodDimSwitch.Init  ( 48, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);	
	Panel100FloodFixedSwitch.Init(113, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	GNPowerOpticsSwitch.Init     (200, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	GNPowerIMUSwitch.Init        (260, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	GNPowerIMUSwitch.InitGuard   (259,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_34x61], 180);
	Panel100RNDZXPDRSwitch.Init  (317, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	
	Panel100LightingRoatariesRow.Init(AID_PANEL100LIGHTINGROTARIES, MainPanel);
	Panel100NumericRotarySwitch.Init (  0,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], Panel100LightingRoatariesRow);
	Panel100FloodRotarySwitch.Init   (137,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], Panel100LightingRoatariesRow);
	Panel100IntegralRotarySwitch.Init(254,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], Panel100LightingRoatariesRow);
	
	SCIUtilPowerSwitchRow.Init (AID_SCIUTILPOWERSWITCH, MainPanel);
	SCIUtilPowerSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SCIUtilPowerSwitchRow);

	Panel275CircuitBrakersRow.Init(AID_PANEL275CIRCUITBRAKERS, MainPanel);
	InverterPower3MainBCircuitBraker.Init     (  0,   0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusB, 70.0);
	InverterPower3MainACircuitBraker.Init     (  0,  59, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusA, 70.0);
	InverterPower2MainBCircuitBraker.Init     (  0, 104, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusB, 70.0);
	InverterPower1MainACircuitBraker.Init     (  0, 149, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusA, 70.0);
	FlightPostLandingMainBCircuitBraker.Init  (  0, 194, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusB);
	FlightPostLandingMainACircuitBraker.Init  (  0, 253, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusA);
	FlightPostLandingBatCCircuitBraker.Init   (  0, 298, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, EntryBatteryC);
	FlightPostLandingBatBusBCircuitBraker.Init(  0, 343, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, &BatteryBusB);
	FlightPostLandingBatBusACircuitBraker.Init(  0, 402, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, &BatteryBusA);
	MainBBatBusBCircuitBraker.Init            (  0, 447, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, &BatteryBusB);
	MainBBatCCircuitBraker.Init               (  0, 492, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, EntryBatteryC);
	MainABatCCircuitBraker.Init               (  0, 537, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, EntryBatteryC);
	MainABatBusACircuitBraker.Init            (  0, 596, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, &BatteryBusA);
	
	//
	// SATPANEL_LEFT
	//

	LeftCOASPowerSwitchRow.Init(AID_LEFTCOASSWITCH, MainPanel);
	LeftCOASPowerSwitch.Init(0, 0, 34, 31, srf[SRF_SWITCH20LEFT], srf[SRF_BORDER_34x31], LeftCOASPowerSwitchRow);

	LeftUtilityPowerSwitchRow.Init(AID_LEFTTUTILITYPOWERSWITCH, MainPanel);
	LeftUtilityPowerSwitch.Init(0, 0, 34, 31, srf[SRF_SWITCH20LEFT], srf[SRF_BORDER_34x31], LeftUtilityPowerSwitchRow);

	PostLandingBCNLTSwitchRow.Init(AID_POSTLANDINGBCNLTSWITCH, MainPanel);
	PostLandingBCNLTSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH20LEFT], srf[SRF_BORDER_34x31], PostLandingBCNLTSwitchRow);

	PostLandingDYEMarkerSwitchRow.Init(AID_POSTLANDINGDYEMARKERSWITCH, MainPanel);
	PostLandingDYEMarkerSwitch.Init		(6, 25, 34, 31, srf[SRF_GUARDEDSWITCH20], srf[SRF_BORDER_34x31], PostLandingDYEMarkerSwitchRow);
	PostLandingDYEMarkerSwitch.InitGuard(0,  0, 46, 75, srf[SRF_SWITCHGUARDPANEL15], srf[SRF_BORDER_46x75]);

	PostLandingVentSwitchRow.Init(AID_POSTLANDINGVENTSWITCH, MainPanel);
	PostLandingVentSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH20LEFT], srf[SRF_BORDER_34x31], PostLandingVentSwitchRow);

	LeftModeIntercomVOXSensThumbwheelSwitchRow.Init(AID_LEFTMODEINTERCOMVOXSENSTHUMBWHEEL, MainPanel);
	LeftModeIntercomVOXSensThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], LeftModeIntercomVOXSensThumbwheelSwitchRow);

	LeftPowerMasterVolumeThumbwheelSwitchRow.Init(AID_LEFTPOWERMASTERVOLUMETHUMBWHEEL, MainPanel);
	LeftPowerMasterVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], LeftPowerMasterVolumeThumbwheelSwitchRow, VOLUME_COMMS2, &soundlib);

	LeftPadCommVolumeThumbwheelSwitchRow.Init(AID_LEFTPADCOMMVOLUMETHUMBWHEEL, MainPanel);
	LeftPadCommVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], LeftPadCommVolumeThumbwheelSwitchRow);

	LeftIntercomVolumeThumbwheelSwitchRow.Init(AID_LEFTINTERCOMVOLUMETHUMBWHEEL, MainPanel);
	LeftIntercomVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], LeftIntercomVolumeThumbwheelSwitchRow);

	LeftSBandVolumeThumbwheelSwitchRow.Init(AID_LEFTSBANDVOLUMETHUMBWHEEL, MainPanel);
	LeftSBandVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], LeftSBandVolumeThumbwheelSwitchRow);

	LeftVHFAMVolumeThumbwheelSwitchRow.Init(AID_LEFTVHFVOLUMETHUMBWHEEL, MainPanel);
	LeftVHFAMVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], LeftVHFAMVolumeThumbwheelSwitchRow);

	LeftModeIntercomSwitchRow.Init(AID_LEFTMODEINTERCOMSWITCH, MainPanel);
	LeftModeIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftModeIntercomSwitchRow);

	LeftAudioPowerSwitchRow.Init(AID_LEFTAUDIOPOWERSWITCH, MainPanel);
	LeftAudioPowerSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftAudioPowerSwitchRow);

	LeftPadCommSwitchRow.Init(AID_LEFTPADCOMMSWITCH, MainPanel);
	LeftPadCommSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftPadCommSwitchRow);

	LeftIntercomSwitchRow.Init(AID_LEFTINTERCOMSWITCH, MainPanel);
	LeftIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftIntercomSwitchRow);

	LeftSBandSwitchRow.Init(AID_LEFTSBANDSWITCH, MainPanel);
	LeftSBandSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftSBandSwitchRow);

	LeftVHFAMSwitchRow.Init(AID_LEFTVHFAMSWITCH, MainPanel);
	LeftVHFAMSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftVHFAMSwitchRow);

	LeftAudioControlSwitchRow.Init(AID_LEFTAUDIOCONTROLSWITCH, MainPanel);
	LeftAudioControlSwitch.Init(0, 0, 34, 34, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftAudioControlSwitchRow);

	LeftSuitPowerSwitchRow.Init(AID_LEFTSUITPOWERSWITCH, MainPanel);
	LeftSuitPowerSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30LEFT], srf[SRF_BORDER_34x34], LeftSuitPowerSwitchRow);

	VHFRNGSwitchRow.Init(AID_VHFRNGSWITCH, MainPanel);
	VHFRNGSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30LEFT], srf[SRF_BORDER_34x34], VHFRNGSwitchRow);

	StabContSystemCircuitBrakerRow.Init(AID_STABCONTCIRCUITBREAKERS, MainPanel);
	StabContSystemTVCAc1CircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabContSystemCircuitBrakerRow, &ACBus1PhaseA);
	StabContSystemAc1CircuitBraker.Init( 52,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabContSystemCircuitBrakerRow, &ACBus1);
	StabContSystemAc2CircuitBraker.Init(135,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabContSystemCircuitBrakerRow, &ACBus2);

	AutoRCSSelectSwitchesRow.Init(AID_AUTORCSSELECTSWITCHES, MainPanel);
	AcRollA1Switch.Init(  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	AcRollC1Switch.Init( 45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	AcRollA2Switch.Init( 90, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	AcRollC2Switch.Init(135, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	BdRollB1Switch.Init(180, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	BdRollD1Switch.Init(225, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	BdRollB2Switch.Init(270, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	BdRollD2Switch.Init(315, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	PitchA3Switch.Init(360, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	PitchC3Switch.Init(405, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	PitchA4Switch.Init(450, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	PitchC4Switch.Init(495, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	YawB3Switch.Init(540, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	YawD3Switch.Init(585, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	YawB4Switch.Init(630, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);
	YawD4Switch.Init(675, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, MainBusA, 0, MainBusB);

	StabilizationControlSystemCircuitBrakerRow.Init(AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS, MainPanel);
	ECATVCAc2CircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &ACBus2PhaseA);
	DirectUllMnACircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusA);
	DirectUllMnBCircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusB);
	ContrDirectMnA1CircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusA);
	ContrDirectMnB1CircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusB);
	ContrDirectMnA2CircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusA);
	ContrDirectMnB2CircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusB);
	ACRollMnACircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusA);
	ACRollMnBCircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusB);
	BDRollMnACircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusA);
	BDRollMnBCircuitBraker.Init(380,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusB);
	PitchMnACircuitBraker.Init(418,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusA);
	PitchMnBCircuitBraker.Init(456,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusB);
	YawMnACircuitBraker.Init(494,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusA);
	YawMnBCircuitBraker.Init(532,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, MainBusB);

	StabilizationControlSystem2CircuitBrakerRow.Init(AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS2, MainPanel);
	OrdealAc2CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow);
	OrdealMnBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow);
	ContrAutoMnACircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusA);
	ContrAutoMnBCircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusB);
	LogicBus12MnACircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusA);
	LogicBus34MnACircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusA);
	LogicBus14MnBCircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusB);
	LogicBus23MnBCircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusB);
	SystemMnACircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusA, 15.0);
	SystemMnBCircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, MainBusB, 15.0);

	FloodDimSwitchRow.Init(AID_FLOODDIMSWITCH, MainPanel);
	FloodDimSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FloodDimSwitchRow);

	FloodFixedSwitchRow.Init(AID_FLOODFIXEDSWITCH, MainPanel);
	FloodFixedSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FloodFixedSwitchRow);

	ReactionControlSystemCircuitBrakerRow.Init(AID_REACTIONCONTROLSYSTEMCIRCUITBREAKERS, MainPanel);
	CMHeater1MnACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusA);
	CMHeater2MnBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusB);
	SMHeatersAMnBCircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusB, 7.5);
	SMHeatersCMnBCircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusB, 7.5);
	SMHeatersBMnACircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusA, 7.5);
	SMHeatersDMnACircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusA, 7.5);
	PrplntIsolMnACircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusA, 10.0);
	PrplntIsolMnBCircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusB, 10.0);
	RCSLogicMnACircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusA);
	RCSLogicMnBCircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusB);
	EMSMnACircuitBraker.Init(380,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusA);
	EMSMnBCircuitBraker.Init(418,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusB);
	DockProbeMnACircuitBraker.Init(456,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusA);
	DockProbeMnBCircuitBraker.Init(494,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, MainBusB);

	FloatBagSwitchRow.Init(AID_FLOATBAGSWITCHES, MainPanel);
	FloatBagSwitch1.Init(  0, 0, 38, 52, srf[SRF_SWLEVERTHREEPOS], srf[SRF_BORDER_38x52], FloatBagSwitchRow);
	FloatBagSwitch2.Init( 58, 0, 38, 52, srf[SRF_SWLEVERTHREEPOS], srf[SRF_BORDER_38x52], FloatBagSwitchRow);
	FloatBagSwitch3.Init(116, 0, 38, 52, srf[SRF_SWLEVERTHREEPOS], srf[SRF_BORDER_38x52], FloatBagSwitchRow);

	SeqEventsContSystemSwitchesRow.Init(AID_SEQEVENTSCONTSYSTEM, MainPanel);
	Logic1Switch.Init( 0, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow, &LogicBatACircuitBraker, 0);
	Logic2Switch.Init(40, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow, &LogicBatBCircuitBraker, 0);
	PyroArmASwitch.Init( 80, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow, &ArmBatACircuitBraker, 0);
	PyroArmBSwitch.Init(133, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow, &ArmBatBCircuitBraker, 0);

	EDSPowerSwitchRow.Init(AID_EDSPOWERSWITCH, MainPanel);
	EDSPowerSwitch.Init(0, 0, 34, 33, srf[SRF_SWITCH305LEFT], srf[SRF_BORDER_34x33], EDSPowerSwitchRow);

	TVCServoPowerSwitchesRow.Init(AID_TVCSERVOPOWERSWITCHES, MainPanel);
	TVCServoPower1Switch.Init( 0, 0, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], TVCServoPowerSwitchesRow);
	TVCServoPower2Switch.Init(25, 35, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], TVCServoPowerSwitchesRow);

	LogicPowerSwitchRow.Init(AID_LOGICPOWERSWITCH, MainPanel);
	LogicPowerSwitch.Init(0, 0, 34, 33, srf[SRF_SWITCH305LEFT], srf[SRF_BORDER_34x33], LogicPowerSwitchRow);

	SIGCondDriverBiasPowerSwitchesRow.Init(AID_SIGCONDDRIVERBIASPOWERSWITCHES, MainPanel);
	SIGCondDriverBiasPower1Switch.Init( 0,  0, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], SIGCondDriverBiasPowerSwitchesRow, 
		&StabContSystemAc1CircuitBraker, NULL, &StabContSystemAc2CircuitBraker);
	SIGCondDriverBiasPower2Switch.Init(25, 35, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], SIGCondDriverBiasPowerSwitchesRow,
		&StabContSystemAc1CircuitBraker, NULL, &StabContSystemAc2CircuitBraker);

	LeftInteriorLightRotariesRow.Init(AID_LEFTINTERIORLIGHTROTARIES, MainPanel);
	NumericRotarySwitch.Init(0,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], LeftInteriorLightRotariesRow);
	FloodRotarySwitch.Init(119,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], LeftInteriorLightRotariesRow);
	IntegralRotarySwitch.Init(238,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], LeftInteriorLightRotariesRow);
	
	FDAIPowerRotaryRow.Init(AID_FDAIPOWERROTARY, MainPanel);
	FDAIPowerRotarySwitch.Init(0,  0, 90, 90, srf[SRF_FDAIPOWERROTARY], srf[SRF_BORDER_84x84], FDAIPowerRotaryRow, &fdaiLeft, &fdaiRight,
		                       &SystemMnACircuitBraker, &SystemMnBCircuitBraker, &StabContSystemAc1CircuitBraker, &StabContSystemAc2CircuitBraker,
							   &GPFPIPitch1Meter, &GPFPIPitch2Meter, &GPFPIYaw1Meter, &GPFPIYaw2Meter);

	SCSElectronicsPowerRotaryRow.Init(AID_SCSELECTRONICSPOWERROTARY, MainPanel);
	SCSElectronicsPowerRotarySwitch.Init(0,  0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_84x84], SCSElectronicsPowerRotaryRow);

	BMAGPowerRotary1Row.Init(AID_BMAGPOWERROTARY1, MainPanel);
	BMAGPowerRotary1Switch.Init(0,  0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_84x84], BMAGPowerRotary1Row, &bmag1);

	BMAGPowerRotary2Row.Init(AID_BMAGPOWERROTARY2, MainPanel);
	BMAGPowerRotary2Switch.Init(0,  0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_84x84], BMAGPowerRotary2Row, &bmag2);

	DirectO2RotaryRow.Init(AID_DIRECTO2ROTARY, MainPanel);
	DirectO2RotarySwitch.Init(0,  0, 70, 70, srf[SRF_DIRECTO2ROTARY], srf[SRF_BORDER_70x70], DirectO2RotaryRow, (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE"));

	ServicePropulsionSysCircuitBrakerRow.Init(AID_SERVICEPROPULSIONSYSCIRCUITBREAKERS, MainPanel);
	GaugingMnACircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, MainBusA);
	GaugingMnBCircuitBraker.Init( 38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, MainBusB);
	GaugingAc1CircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &ACBus1);
	GaugingAc2CircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &ACBus2);
	HeValveMnACircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, MainBusA);
	HeValveMnBCircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, MainBusB);
	PitchBatACircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusA);
	PitchBatBCircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusB);
	YawBatACircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusA);
	YawBatBCircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusB);
	PilotValveMnACircuitBraker.Init(380,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, MainBusA);
	PilotValveMnBCircuitBraker.Init(418,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, MainBusB);

	FloatBagCircuitBrakerRow.Init(AID_FLOATBAGCIRCUITBREAKERS, MainPanel);
	FloatBag1BatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], FloatBagCircuitBrakerRow, &BatteryBusA);
	FloatBag2BatBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], FloatBagCircuitBrakerRow, &BatteryBusB);
	FloatBag3FLTPLCircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], FloatBagCircuitBrakerRow, &FlightPostLandingBus);

	SeqEventsContSysCircuitBrakerRow.Init(AID_SEQEVENTSCONTSYSCIRCUITBREAKERS, MainPanel);
	LogicBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &BatteryBusA);
	LogicBatBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &BatteryBusB);
	ArmBatACircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &PyroBusA);
	ArmBatBCircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &PyroBusB);

	EDSCircuitBrakerRow.Init(AID_EDSCIRCUITBREAKERS, MainPanel);
	EDS1BatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EDSCircuitBrakerRow);
	EDS2BatBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EDSCircuitBrakerRow);
	EDS3BatCCircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EDSCircuitBrakerRow);

	ELSCircuitBrakerRow.Init(AID_ELSCIRCUITBREAKERS, MainPanel);
	ELSBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ELSCircuitBrakerRow);
	ELSBatBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ELSCircuitBrakerRow);

	PLVentCircuitBrakerRow.Init(AID_PLVENTCIRCUITBREAKER, MainPanel);
	FLTPLCircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], PLVentCircuitBrakerRow, &FlightPostLandingBus);

	//RightWindowCoverRow.Init(AID_RIGHTWINDOWCOVER, MainPanel);
	//RightWindowCoverSwitch.Init(0, 0, 525, 496, srf[SRF_CSMRIGHTWINDOWCOVER], RightWindowCoverRow);

	//
	// SATPANEL_LEFT_RNDZ_WINDOW
	//

	OrbiterAttitudeToggleRow.Init(AID_SM_RCS_MODE, MainPanel);
	OrbiterAttitudeToggle.Init(28, 33, 23, 20, srf[SRF_SWITCHUPSMALL], srf[SRF_BORDER_23x20], OrbiterAttitudeToggleRow);

	/////////////////////////////
	// G&N lower equipment bay //
	/////////////////////////////

	ModeSwitchRow.Init(AID_GNMODESWITCH, MainPanel);
	ControllerSpeedSwitchRow.Init(AID_CONTROLLERSPEEDSWITCH, MainPanel);
	ControllerCouplingSwitchRow.Init(AID_CONTROLLERCOUPLINGSWITCH, MainPanel);

	if (panel == SATPANEL_SEXTANT || panel == SATPANEL_TELESCOPE) {
		ModeSwitch.Init(0, 0, 23, 20, srf[SRF_THREEPOSSWITCHSMALL], srf[SRF_BORDER_23x20], ModeSwitchRow, &agc);
		ControllerSpeedSwitch.Init(0, 0, 23, 20, srf[SRF_THREEPOSSWITCHSMALL], srf[SRF_BORDER_23x20], ControllerSpeedSwitchRow);
		ControllerCouplingSwitch.Init(0, 0, 23, 20, srf[SRF_SWITCHUPSMALL], srf[SRF_BORDER_23x20], ControllerCouplingSwitchRow);
	} else {
		ModeSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ModeSwitchRow, &agc);
		ControllerSpeedSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ControllerSpeedSwitchRow);
		ControllerCouplingSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ControllerCouplingSwitchRow);
	}

	ControllerSwitchesRow.Init(AID_CONTORLLERSWITCHES, MainPanel);
	ControllerTrackerSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ControllerSwitchesRow);
	ControllerTelescopeTrunnionSwitch.Init(109, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ControllerSwitchesRow);

	ConditionLampsSwitchRow.Init(AID_CONDITIONLAMPSSWITCH, MainPanel);
	ConditionLampsSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ConditionLampsSwitchRow);

	UPTLMSwitchRow.Init(AID_UPLINKTELEMETRYSWITCH, MainPanel);
	UPTLMSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], UPTLMSwitchRow);

	OpticsHandcontrollerSwitchRow.Init(AID_OPTICS_HANDCONTROLLER, MainPanel);
	OpticsHandcontrollerSwitch.Init(0, 0, 47, 57, srf[SRF_OPTICS_HANDCONTROLLER], OpticsHandcontrollerSwitchRow, this);

	OpticsMarkButtonRow.Init(AID_MARKBUTTON, MainPanel);
	OpticsMarkButton.Init(0, 0, 27, 27, srf[SRF_MARK_BUTTONS], NULL, OpticsMarkButtonRow);

	OpticsMarkRejectButtonRow.Init(AID_MARKREJECT, MainPanel);
	OpticsMarkRejectButton.Init(0, 0, 15, 20, srf[SRF_MARK_BUTTONS], NULL, OpticsMarkRejectButtonRow, 0, 28);

	MinImpulseHandcontrollerSwitchRow.Init(AID_MINIMPULSE_HANDCONTROLLER, MainPanel);
	MinImpulseHandcontrollerSwitch.Init(0, 0, 47, 57, srf[SRF_MINIMPULSE_HANDCONTROLLER], MinImpulseHandcontrollerSwitchRow, this);

	////////////////////////
	// Panel 325/326 etc. //
	////////////////////////
	
	GlycolToRadiatorsLeverRow.Init(AID_GLYCOLTORADIATORSLEVER, MainPanel);
	GlycolToRadiatorsLever.Init(0, 0, 32, 160, srf[SRF_GLYCOLLEVER], srf[SRF_BORDER_32x160], GlycolToRadiatorsLeverRow);

	CabinPressureReliefLever1Row.Init(AID_CABINPRESSURERELIEFLEVER1, MainPanel);
	CabinPressureReliefLever1.Init(0, 0, 150, 80, srf[SRF_CABINRELIEFUPPERLEVER], CabinPressureReliefLever1Row);

	CabinPressureReliefLever2Row.Init(AID_CABINPRESSURERELIEFLEVER2, MainPanel);
	CabinPressureReliefLever2.Init(66, 8, 200, 80, srf[SRF_CABINRELIEFLOWERLEVER], CabinPressureReliefLever2Row);
	CabinPressureReliefLever2.InitGuard(srf[SRF_CABINRELIEFGUARDLEVER], &soundlib);

	GlycolReservoirRotariesRow.Init(AID_GLYCOLRESERVOIRROTARIES, MainPanel);
	GlycolReservoirInletRotary.Init (0, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], GlycolReservoirRotariesRow);
	GlycolReservoirBypassRotary.Init(0, 106, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78],  GlycolReservoirRotariesRow);
	GlycolReservoirOutletRotary.Init(0, 212, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78],  GlycolReservoirRotariesRow);
	
	OxygenRotariesRow.Init(AID_OXYGENROTARIES, MainPanel);
	OxygenSurgeTankRotary.Init      (  0, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], OxygenRotariesRow);
	OxygenSMSupplyRotary.Init       (106, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], OxygenRotariesRow);
	OxygenRepressPackageRotary.Init (212, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], OxygenRotariesRow);

	ORDEALSwitchesRow.Init(AID_ORDEALSWITCHES, MainPanel);
	ORDEALFDAI1Switch.Init	 (  0,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALFDAI2Switch.Init	 (113,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALEarthSwitch.Init	 (209,  0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALLightingSwitch.Init(  0, 89, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow); 
	ORDEALModeSwitch.Init	 (160, 89, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);	
	ORDEALSlewSwitch.Init	 (209, 89, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);

	ORDEALRotaryRow.Init(AID_ORDEALROTARY, MainPanel);
	ORDEALAltSetRotary.Init(  0, 0, 84, 84, srf[SRF_ORDEAL_ROTARY], srf[SRF_BORDER_84x84], ORDEALRotaryRow);

	SuitCircuitReturnValveLeverRow.Init(AID_SUITCIRCUITRETURNVALVE, MainPanel);
	SuitCircuitReturnValveLever.Init(0, 0, 160, 32, srf[SRF_SUITRETURN_LEVER], srf[SRF_BORDER_160x32], SuitCircuitReturnValveLeverRow);

	///////////////////////////
	// Panel 375/377/378/379 //
	///////////////////////////
	
	OxygenSurgeTankValveRotaryRow.Init(AID_OXYGEN_SURGE_TANK_VALVE, MainPanel);
	OxygenSurgeTankValveRotary.Init(0, 0, 34, 34, srf[SRF_OXYGEN_SURGE_TANK_VALVE], srf[SRF_BORDER_34x34], OxygenSurgeTankValveRotaryRow);
	
	GlycolToRadiatorsRotaryRow.Init(AID_GLYCOL_TO_RADIATORS_KNOB, MainPanel);
	GlycolToRadiatorsRotary.Init(0, 0, 31, 31, srf[SRF_GLYCOL_TO_RADIATORS_KNOB], srf[SRF_BORDER_31x31], GlycolToRadiatorsRotaryRow);

	GlycolRotaryRow.Init(AID_GLYCOL_ROTARY, MainPanel);
	GlycolRotary.Init(0, 0, 72, 72, srf[SRF_GLYCOL_ROTARY], srf[SRF_BORDER_72x72], GlycolRotaryRow);
	
	AccumRotaryRow.Init(AID_ACCUM_ROTARY, MainPanel);
	AccumRotary.Init(0, 0, 58, 58, srf[SRF_ACCUM_ROTARY], srf[SRF_BORDER_58x58], AccumRotaryRow);

	///////////////
	// Panel 252 //
	///////////////
	
	WaterControlPanelRow.Init(AID_PANEL_352, MainPanel);
	PressureReliefRotary.Init    (121,   0, 57, 57, srf[SRF_PRESS_RELIEF_VALVE], srf[SRF_BORDER_58x58], WaterControlPanelRow);
	WasteTankInletRotary.Init    (  0, 171, 47, 47, srf[SRF_TANK_VALVE], srf[SRF_BORDER_47x47], WaterControlPanelRow);
	PotableTankInletRotary.Init  (119, 171, 47, 47, srf[SRF_TANK_VALVE], srf[SRF_BORDER_47x47], WaterControlPanelRow);
	WasteTankServicingRotary.Init(239, 170, 47, 47, srf[SRF_TANK_VALVE], srf[SRF_BORDER_47x47], WaterControlPanelRow);

	///////////////
	// Panel 251 //
	///////////////
	
	CabinRepressValveRotaryRow.Init(AID_CABIN_REPRESS_VALVE, MainPanel);
	CabinRepressValveRotary.Init(0, 0, 48, 48, srf[SRF_CABIN_REPRESS_VALVE], srf[SRF_BORDER_48x48], CabinRepressValveRotaryRow);

	WaterGlycolTanksRotariesRow.Init(AID_WATER_GLYCOL_TANKS_ROTARIES, MainPanel);
	SelectorInletValveRotary.Init (0,   0, 84, 84, srf[SRF_SELECTOR_INLET_ROTARY], srf[SRF_BORDER_84x84], WaterGlycolTanksRotariesRow);
	SelectorOutletValveRotary.Init(0, 133, 84, 84, srf[SRF_SELECTOR_OUTLET_ROTARY], srf[SRF_BORDER_84x84], WaterGlycolTanksRotariesRow);
	
	EmergencyCabinPressureRotaryRow.Init(AID_EMERGENCY_CABIN_PRESSURE_ROTARY, MainPanel);
	EmergencyCabinPressureRotary.Init(0, 0, 65, 65, srf[SRF_EMERGENCY_PRESS_ROTARY], srf[SRF_BORDER_65x65], EmergencyCabinPressureRotaryRow);
}

void SetupgParam(HINSTANCE hModule) {

	g_Param.hDLL = hModule;

	//
	// allocate GDI resources
	//

	g_Param.font[0]  = CreateFont (-13, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[2]  = CreateFont (-8, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
	g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
	g_Param.brush[2] = CreateSolidBrush (RGB(154,154,154));  // Grey
	g_Param.brush[3] = CreateSolidBrush (RGB(3,3,3));  // Black
	g_Param.pen[0] = CreatePen (PS_SOLID, 1, RGB(224, 224, 224));
	g_Param.pen[1] = CreatePen (PS_SOLID, 4, RGB(  0,   0,   0));
	g_Param.pen[2] = CreatePen (PS_SOLID, 1, RGB( 77,  77,  77));
	g_Param.pen[3] = CreatePen (PS_SOLID, 3, RGB( 77,  77,  77));
	g_Param.pen[4] = CreatePen (PS_SOLID, 3, RGB(  0,   0,   0));
}

void DeletegParam() {

	int i;

	//
	// deallocate GDI resources
	//

	for (i = 0; i < 3; i++) DeleteObject (g_Param.font[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.brush[i]);
	for (i = 0; i < 5; i++) DeleteObject (g_Param.pen[i]);
}

bool Saturn::clbkPanelMouseEvent (int id, int event, int mx, int my)

{
	//double dx,dy;

	static int ctrl = 0;

	if (MainPanel.CheckMouseClick(id, event, mx, my))
		return true;

	switch (id) {
	// ASCP
	case AID_ASCPDISPLAYROLL:
		ascp.RollDisplayClicked();
		return true;

	case AID_ASCPDISPLAYPITCH:
		ascp.PitchDisplayClicked();
		return true;

	case AID_ASCPDISPLAYYAW:
		ascp.YawDisplayClicked();
		return true;

	case AID_ASCPROLL:
		ascp.RollClick(event, mx, my);
		return true;

	case AID_ASCPPITCH:
		ascp.PitchClick(event, mx, my);
		return true;

	case AID_ASCPYAW:
		ascp.YawClick(event, mx, my);			
		return true;

	// EMS
	case AID_EMSDVSETSWITCH:
		return EMSDvSetSwitch.CheckMouseClick(event, mx, my);			

	// CWS
	case AID_MASTER_ALARM:
	case AID_MASTER_ALARM2:
	case AID_MASTER_ALARM3:
		return cws.CheckMasterAlarmMouseClick(event);

	case AID_DSKY_KEY:

		if (event & PANEL_MOUSE_LBDOWN) {
			dsky.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky.ProcessKeyRelease(mx, my);
		}
		return true;

	case AID_DSKY2_KEY:
		if (event & PANEL_MOUSE_LBDOWN) {
			dsky2.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky2.ProcessKeyRelease(mx, my);
		}
		return true;

	case AID_MFDMAINLEFT:
		MousePanel_MFDButton(MFD_LEFT, event, mx, my);
		return true;

	case AID_MFDMAINRIGHT:
		MousePanel_MFDButton(MFD_RIGHT, event, mx, my);
		return true;

	case AID_MFDGNLEFT:
		MousePanel_MFDButton(MFD_LEFT, event, mx, my);
		return true;

	case AID_MFDGNUSER1:
		MousePanel_MFDButton(MFD_USER1, event, mx, my);
		return true;

	case AID_MFDGNUSER2:
		MousePanel_MFDButton(MFD_USER2, event, mx, my);
		return true;

	case AID_MFDGNRIGHT:
		MousePanel_MFDButton(MFD_RIGHT, event, mx, my);
		return true;

	case AID_MFDDOCK:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
			if (my > 234 && my < 249) {
				if (event & PANEL_MOUSE_LBDOWN) {
					if (mx > 47 && mx < 68) {
						ButtonClick();
						oapiToggleMFD_on(MFD_RIGHT);	// MFD_USER1
					} else if (mx > 208 && mx < 229) {
						ButtonClick();
						oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_F1);		// MFD_USER1
					} else if (mx > 239 && mx < 252) {
						ButtonClick();
						oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_GRAVE);		// MFD_USER1
					}
				}
			} else if (mx > 5 && mx < 26 && my > 38 && my < 208) {
				if ((my - 38) % 31 < 14) {
					int bt = (my - 38) / 31 + 0;
					if (event & PANEL_MOUSE_LBDOWN)
						ButtonClick();
					oapiProcessMFDButton(MFD_RIGHT, bt, event);	// MFD_USER1
				}
			} else if (mx > 273 && mx < 294 && my > 38 && my < 208) {
				if ((my - 38) % 31 < 14) {
					int bt = (my - 38) / 31 + 6;
					if (event & PANEL_MOUSE_LBDOWN)
						ButtonClick();
					oapiProcessMFDButton(MFD_RIGHT, bt, event);	// MFD_USER1
				}
			}
		} else if (event & PANEL_MOUSE_LBDOWN) {
			ButtonClick();
			oapiToggleMFD_on(MFD_RIGHT); // MFD_USER1
		}
		return true;

	case AID_MFDDOCK_POWER:
		if (oapiGetMFDMode(MFD_RIGHT) == MFD_NONE) {	// MFD_USER1
			ButtonClick();
			oapiToggleMFD_on(MFD_RIGHT);	// MFD_USER1
		}
		return true;

	case AID_ABORT_BUTTON:
		if (mx > 1 && mx < 62 && my > 1 && my < 31){
			bAbort = true;
			ButtonClick();
		}
		return true;

	case AID_COAS:
		if (coasEnabled)
			coasEnabled = false;
		else
			coasEnabled = true;
		SwitchClick();
		return true;

	case AID_OPTICS_DSKY:
		if (mx >= 285 && my <= 100 && event == PANEL_MOUSE_LBDOWN) { 
			if (opticsDskyEnabled)
				opticsDskyEnabled = false;
			else
				opticsDskyEnabled = true;
			SwitchClick();
		}
		if (event & PANEL_MOUSE_LBDOWN) {
			dsky2.ProcessKeyPress(mx - 7, my - 218);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky2.ProcessKeyRelease(mx - 7, my - 218);
		}
		return true;

	case AID_SWITCHTO_TELESCOPE1:
	case AID_SWITCHTO_TELESCOPE2:
		if (event == PANEL_MOUSE_LBDOWN) { 
			PanelId = SATPANEL_TELESCOPE;
			CheckPanelIdInTimestep = true;
		}
		return true;

	case AID_SWITCHTO_SEXTANT1:
	case AID_SWITCHTO_SEXTANT2:
		if (event == PANEL_MOUSE_LBDOWN) { 
			PanelId = SATPANEL_SEXTANT;
			CheckPanelIdInTimestep = true;
		}
		return true;
	}
	return false;
}

void Saturn::PanelSwitchToggled(ToggleSwitch *s) {

	if (s == &O2Heater1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1HEATER:PUMP"));

	} else if (s == &O2Heater2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2HEATER:PUMP"));

	} else if (s == &H2Heater1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1HEATER:PUMP"));

	} else if (s == &H2Heater2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2HEATER:PUMP"));

	} else if (s == &O2Fan1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1FAN:PUMP"));

	} else if (s == &O2Fan2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2FAN:PUMP"));

	} else if (s == &H2Fan1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1FAN:PUMP"));

	} else if (s == &H2Fan2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2FAN:PUMP"));


	} else if (s == &FuelCellRadiators1Switch) {
		if (FuelCellRadiators1Switch.IsUp())
			FuelCellCoolingBypass(1, false);
		else if (FuelCellRadiators1Switch.IsDown())
			FuelCellCoolingBypass(1, true);

	} else if (s == &FuelCellRadiators2Switch) {
		if (FuelCellRadiators2Switch.IsUp())
			FuelCellCoolingBypass(2, false);
		else if (FuelCellRadiators2Switch.IsDown())
			FuelCellCoolingBypass(2, true);

	} else if (s == &FuelCellRadiators3Switch) {
		if (FuelCellRadiators3Switch.IsUp())
			FuelCellCoolingBypass(3, false);
		else if (FuelCellRadiators3Switch.IsDown())
			FuelCellCoolingBypass(3, true);


	} else if (s == &FuelCellHeater1Switch) {
		FuelCellHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1HEATER:PUMP"));

	} else if (s == &FuelCellHeater2Switch) {
		FuelCellHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2HEATER:PUMP"));

	} else if (s == &FuelCellHeater3Switch) {
		FuelCellHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3HEATER:PUMP"));


	} else if (s == &FuelCellPurge1Switch || s == &FuelCellPurge2Switch || s == &FuelCellPurge3Switch || s == &H2PurgeLineSwitch) {
		if (s == &FuelCellPurge1Switch || s == &H2PurgeLineSwitch) {
			FuelCellPurgeSwitchToggled(&FuelCellPurge1Switch,
				(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:PURGE"));
		}
		if (s == &FuelCellPurge2Switch || s == &H2PurgeLineSwitch) {
			FuelCellPurgeSwitchToggled(&FuelCellPurge2Switch,
				(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:PURGE"));
		}
		if (s == &FuelCellPurge3Switch || s == &H2PurgeLineSwitch) {
			FuelCellPurgeSwitchToggled(&FuelCellPurge3Switch,
				(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:PURGE"));
		}


	} else if (s == &FuelCellReactants1Switch) {
		FuelCellReactantsSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:START"));

	} else if (s == &FuelCellReactants2Switch) {
		FuelCellReactantsSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:START"));

	} else if (s == &FuelCellReactants3Switch) {
		FuelCellReactantsSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:START"));


	} else if (s == &MainBusTieBatAcSwitch) {
		MainBusAController.SetTieState(s->GetState());
		PanelRotationalSwitchChanged(&BatteryChargeRotary);

	} else if (s == &MainBusTieBatBcSwitch) {
		MainBusBController.SetTieState(s->GetState());
		PanelRotationalSwitchChanged(&BatteryChargeRotary);

	} else if (s == &GlycolToRadiatorsLever) {
		if (GlycolToRadiatorsLever.IsDown()) {
			// Radiators are bypassed
			PrimEcsRadiatorExchanger1->SetBypassed(true);
			PrimEcsRadiatorExchanger2->SetBypassed(true);
		} else {
			PrimEcsRadiatorExchanger1->SetBypassed(false);
			PrimEcsRadiatorExchanger2->SetBypassed(false);
		}

	} else if (s == &EcsRadiatorsFlowContPwrSwitch || s == &EcsRadiatorsManSelSwitch) {
		if (EcsRadiatorsFlowContPwrSwitch.IsUp()) {
			PrimEcsRadiatorExchanger1->SetPumpOn(); 
			PrimEcsRadiatorExchanger2->SetPumpOn(); 

		} else if (EcsRadiatorsFlowContPwrSwitch.IsDown()) {
			if (EcsRadiatorsManSelSwitch.IsUp()) {
				PrimEcsRadiatorExchanger1->SetPumpOn(); 
				PrimEcsRadiatorExchanger2->SetPumpOff(); 

			} else if (EcsRadiatorsManSelSwitch.IsCenter()) {
				PrimEcsRadiatorExchanger1->SetPumpOff(); 
				PrimEcsRadiatorExchanger2->SetPumpOff(); 

			} else if (EcsRadiatorsManSelSwitch.IsDown()) {
				PrimEcsRadiatorExchanger1->SetPumpOff(); 
				PrimEcsRadiatorExchanger2->SetPumpOn(); 
			}
		}

	} else if (s == &EcsRadiatorsHeaterPrimSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("ELECTRIC:PRIMECSRADIATORSHEATER:PUMP");
		if (EcsRadiatorsHeaterPrimSwitch.IsCenter())
			*pump = SP_PUMP_OFF;
		else
			*pump = SP_PUMP_AUTO;

	} else if (s == &EcsRadiatorsHeaterSecSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("ELECTRIC:SECECSRADIATORSHEATER:PUMP");

		// The heat exchangers should be controlled by the GLY TO RAD SEC valve on panel 377,
		// until we have that panel we switch them together with the heater
		int *pump1 = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SECECSRADIATOREXCHANGER1:PUMP");
		int *pump2 = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SECECSRADIATOREXCHANGER2:PUMP");

		if (EcsRadiatorsHeaterSecSwitch.IsDown()) {
			*pump = SP_PUMP_OFF;

			*pump1 = SP_PUMP_OFF;
			*pump2 = SP_PUMP_OFF;
		} else {
			*pump = SP_PUMP_AUTO;

			*pump1 = SP_PUMP_ON;
			*pump2 = SP_PUMP_ON;
		}

	} else if (s == &SuitCircuitH2oAccumAutoSwitch || s == &SuitCircuitH2oAccumOnSwitch) {
		if (SuitCircuitH2oAccumAutoSwitch.IsCenter() && SuitCircuitH2oAccumOnSwitch.IsCenter()) {
			SuitCompressor1->h_pumpH2o = SP_PUMP_OFF;
			SuitCompressor2->h_pumpH2o = SP_PUMP_OFF;
		} else {
			SuitCompressor1->h_pumpH2o = SP_PUMP_ON;
			SuitCompressor2->h_pumpH2o = SP_PUMP_ON;
		}

	} else if (s == &SuitCircuitHeatExchSwitch) {
		int *pump1 = (int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITHEATEXCHANGER:PUMP");
		int *pump2 = (int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITCIRCUITHEATEXCHANGER:PUMP");
		int *pump3 = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SECSUITHEATEXCHANGER:PUMP");
		int *pump4 = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SECSUITCIRCUITHEATEXCHANGER:PUMP");

		int *pump5 = (int*) Panelsdk.GetPointerByString("ELECTRIC:SUITHEATER:PUMP");
		int *pump6 = (int*) Panelsdk.GetPointerByString("ELECTRIC:SUITCIRCUITHEATER:PUMP");

		if (SuitCircuitHeatExchSwitch.IsDown()) {
			*pump1 = SP_PUMP_OFF;
			*pump2 = SP_PUMP_OFF;
			*pump3 = SP_PUMP_OFF;
			*pump4 = SP_PUMP_OFF;

			*pump5 = SP_PUMP_OFF;
			*pump6 = SP_PUMP_OFF;

		} else if (SuitCircuitHeatExchSwitch.IsUp()) {
			*pump1 = SP_PUMP_AUTO;
			*pump2 = SP_PUMP_AUTO;
			*pump3 = SP_PUMP_AUTO;
			*pump4 = SP_PUMP_AUTO;

			*pump5 = SP_PUMP_AUTO;
			*pump6 = SP_PUMP_AUTO;
		}

	} else if (s == &SecCoolantLoopEvapSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:PUMP");

		if (SecCoolantLoopEvapSwitch.IsUp()) {
			*pump = SP_PUMP_AUTO;
		} else if (SecCoolantLoopEvapSwitch.IsCenter()) {
			*pump = SP_PUMP_OFF;
		} else {
			*pump = SP_PUMP_ON;
			*((double*) Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:THROTTLE")) = 0;
		}

	} else if (s == &GlycolEvapTempInSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLEVAPINLETTEMPVALVE:PUMP");

		if (GlycolEvapTempInSwitch.IsUp())
			*pump = SP_PUMP_AUTO;
		else
			*pump = SP_PUMP_ON;

	} else if (s == &GlycolEvapSteamPressAutoManSwitch || s == &GlycolEvapH2oFlowSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:PUMP");

		if (GlycolEvapH2oFlowSwitch.IsCenter()) {
			*pump = SP_PUMP_OFF;

		} else if (GlycolEvapH2oFlowSwitch.IsDown()) {
			*pump = SP_PUMP_ON;
			*((double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:THROTTLE")) = 1;

		} else {
			if (GlycolEvapSteamPressAutoManSwitch.IsUp())
				*pump = SP_PUMP_AUTO;
			else
				*pump = SP_PUMP_ON;
		}

	} else if (s == &GlycolEvapSteamPressIncrDecrSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:VALVE");
		if (GlycolEvapSteamPressIncrDecrSwitch.IsUp())
			*pump = SP_VALVE_CLOSE;
		else if (GlycolEvapSteamPressIncrDecrSwitch.IsDown())
			*pump = SP_VALVE_OPEN;
		else
			*pump = SP_VALVE_NONE;

	} else if (s == &CabinTempAutoManSwitch) {
		CabinTempAutoSwitchToggled();

	} else if (s == &DockingProbeExtdRelSwitch) {
		if (DockingProbeExtdRelSwitch.IsUp()) {
			dockingprobe.Extend();

		} else if (DockingProbeExtdRelSwitch.IsDown()) {
			if (!DockingProbeRetractPrimSwitch.IsCenter() || !DockingProbeRetractSecSwitch.IsCenter()) {
				dockingprobe.Retract();
			}
		}

	} else if (s == &DockingProbeRetractPrimSwitch) {
		if (DockingProbeExtdRelSwitch.IsDown() && !DockingProbeRetractPrimSwitch.IsCenter()) {
			dockingprobe.Retract();
		}

	} else if (s == &DockingProbeRetractSecSwitch) {
		if (DockingProbeExtdRelSwitch.IsDown() && !DockingProbeRetractSecSwitch.IsCenter()) {
			dockingprobe.Retract();
		}	

	} else if (s == &GDCAlignButton) {
		if (s->GetState() == 1)
			gdc.AlignGDC();
	
	} else if (s == &SPSTestSwitch) {
		SPSPropellant.SPSTestSwitchToggled();
	
	} else if (s == &SPSLineHTRSSwitch) {  
		int *pump1 = (int*) Panelsdk.GetPointerByString("ELECTRIC:SPSPROPELLANTLINEHEATERA:PUMP");
		int *pump2 = (int*) Panelsdk.GetPointerByString("ELECTRIC:SPSPROPELLANTLINEHEATERB:PUMP");

		if (s->IsUp()) {
			*pump1 = SP_PUMP_ON;
			*pump2 = SP_PUMP_ON;

		} else if (s->IsDown()) {
			*pump1 = SP_PUMP_ON;
			*pump2 = SP_PUMP_OFF;

		} else {
			*pump1 = SP_PUMP_OFF;
			*pump2 = SP_PUMP_OFF;
		}
	} else if (s == &EMSModeSwitch) {
		ems.SwitchChanged();

	} else if (s == &OpticsMarkButton) {
		if (s->GetState() == 1) {
			agc.SetInputChannelBit(016, 6, 1);
		} else {
			agc.SetInputChannelBit(016, 6, 0);
		}

	} else if (s == &OpticsMarkRejectButton) {
		if (s->GetState() == 1) {
			agc.SetInputChannelBit(016, 7, 1);
		} else {
			agc.SetInputChannelBit(016, 7, 0);
		}
	}
}

void Saturn::PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) {

	if (s == &FuelCellPhIndicator) {
		FuelCellPhIndicator = true;	// Not simulated at the moment

	} else if (s == &FuelCellRadTempIndicator) {
		FuelCellStatus fc;
		GetFuelCellStatus(FuelCellIndicatorsSwitch.GetState(), fc);
		if (fc.CoolingTempF < -30.0)	// indication if temperature below -30°F
			FuelCellRadTempIndicator = false;
		else
			FuelCellRadTempIndicator = true;

	} else if (s == &FuelCellRadiators1Indicator) {
		FuelCellRadiators1Indicator = !FuelCellCoolingBypassed(1);

	} else if (s == &FuelCellRadiators2Indicator) {
		FuelCellRadiators2Indicator = !FuelCellCoolingBypassed(2);

	} else if (s == &FuelCellRadiators3Indicator) {
		FuelCellRadiators3Indicator = !FuelCellCoolingBypassed(3);

	} else if (s == &FuelCellReactants1Indicator) {
		if (FuelCells[0]->running) FuelCellReactants1Indicator = false;
		else FuelCellReactants1Indicator = true;

	} else if (s == &FuelCellReactants2Indicator) {
		if (FuelCells[1]->running) FuelCellReactants2Indicator = false;
		else FuelCellReactants2Indicator = true;

	} else if (s == &FuelCellReactants3Indicator) {
		if (FuelCells[2]->running) FuelCellReactants3Indicator = false;
        else FuelCellReactants3Indicator = true;

	} else if (s == &EcsRadiatorIndicator) {
		if (EcsRadiatorsFlowContPwrSwitch.IsUp()) {
			if (EcsRadiatorsFlowContAutoSwitch.IsDown())
				EcsRadiatorIndicator = false;
			else
				EcsRadiatorIndicator = true;
		}
	} else if (s == &DockingProbeAIndicator) {
		DockingProbeAIndicator = (dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_RETRACTED || dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_EXTENDED);

	} else if (s == &DockingProbeBIndicator) {
		DockingProbeBIndicator = (dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_RETRACTED || dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_EXTENDED);
	
	} else if (s == &SPSOxidFlowValveMaxIndicator) {
		SPSOxidFlowValveMaxIndicator = !SPSPropellant.IsOxidFlowValveMax();
		
	} else if (s == &SPSOxidFlowValveMinIndicator) {
		SPSOxidFlowValveMinIndicator = !SPSPropellant.IsOxidFlowValveMin();

	} else if (s == &SPSHeliumValveAIndicator) {
		SPSHeliumValveAIndicator = SPSPropellant.IsHeliumValveAOpen();

	} else if (s == &SPSHeliumValveBIndicator) {
		SPSHeliumValveBIndicator = SPSPropellant.IsHeliumValveBOpen();
	
	}  else if (s == &SPSInjectorValve1Indicator) {
		SPSInjectorValve1Indicator = SPSEngine.GetInjectorValves12Open();

	}  else if (s == &SPSInjectorValve2Indicator) {
		SPSInjectorValve2Indicator = SPSEngine.GetInjectorValves12Open();

	}  else if (s == &SPSInjectorValve3Indicator) {
		SPSInjectorValve3Indicator = SPSEngine.GetInjectorValves34Open();

	}  else if (s == &SPSInjectorValve4Indicator) {
		SPSInjectorValve4Indicator = SPSEngine.GetInjectorValves34Open();
	}
}

void Saturn::PanelRotationalSwitchChanged(RotationalSwitch *s) {

	if (s == &BatteryChargeRotary) {
		if (s->GetState() == 1 && MainBusTieBatAcSwitch.IsDown()) 
			BatteryCharger.Charge(1);
		else if (s->GetState() == 2 && MainBusTieBatBcSwitch.IsDown())  
			BatteryCharger.Charge(2);
		else if (s->GetState() == 3 && MainBusTieBatAcSwitch.IsDown() && MainBusTieBatBcSwitch.IsDown()) 
			BatteryCharger.Charge(3);
		else
			BatteryCharger.Charge(0);
	} else if (s == &EMSFunctionSwitch) {
		ems.SwitchChanged();
	}
}

void Saturn::PanelThumbwheelSwitchChanged(ThumbwheelSwitch *s) {

	if (s == &CabinTempAutoControlSwitch) {
		CabinTempAutoSwitchToggled();
	}
}

void Saturn::CryoTankHeaterSwitchToggled(ToggleSwitch *s, int *pump) {

	if (s->IsUp())
		*pump = SP_PUMP_AUTO;
	else if (s->IsCenter())
		*pump = SP_PUMP_OFF;
	else if (s->IsDown())
		*pump = SP_PUMP_ON;
}

void Saturn::FuelCellHeaterSwitchToggled(ToggleSwitch *s, int *pump) {

	if (s->IsUp())
		*pump = SP_PUMP_AUTO;
	else
		*pump = SP_PUMP_OFF;
}

void Saturn::FuelCellReactantsSwitchToggled(ToggleSwitch *s, int *start) {

	// Is the reactants valve latched?
	if (!FCReacsValvesSwitch) return;

	if (s->IsUp())
		*start = SP_FUELCELL_START;
	else if (s->IsCenter())
		*start = SP_FUELCELL_NONE;
	else if (s->IsDown())
		*start = SP_FUELCELL_STOP;
}

void Saturn::FuelCellPurgeSwitchToggled(ToggleSwitch *s, int *start) {

	if (s->IsUp()) {
		if (H2PurgeLineSwitch.GetState())
			*start = SP_FUELCELL_H2PURGE;
		else
			*start = SP_FUELCELL_NOPURGE;
	} else if (s->IsCenter())
		*start = SP_FUELCELL_NOPURGE;
	else if (s->IsDown())
		*start = SP_FUELCELL_O2PURGE;
}

void Saturn::CabinTempAutoSwitchToggled() {

	if (CabinTempAutoManSwitch.IsUp()) {
		double targetTemp = 294.0 + CabinTempAutoControlSwitch.GetState() * 6.0 / 9.0;

		PrimCabinHeatExchanger->tempMin = targetTemp;
		PrimCabinHeatExchanger->tempMax = targetTemp + 0.5;

		SecCabinHeatExchanger->tempMin = targetTemp;
		SecCabinHeatExchanger->tempMax = targetTemp + 0.5;

		CabinHeater->valueMin = targetTemp - 1.0;
		CabinHeater->valueMax = targetTemp;		
	}
}

void Saturn::MousePanel_MFDButton(int mfd, int event, int mx, int my) {

	if (oapiGetMFDMode(mfd) != MFD_NONE) {
		if (my > 277 && my < 293) {
			if (event & PANEL_MOUSE_LBDOWN) {
				if (mx > 57 && mx < 80) {
					ButtonClick();
					oapiToggleMFD_on(mfd);
				} else if (mx > 248 && mx < 271) {
					ButtonClick();
					oapiSendMFDKey(mfd, OAPI_KEY_F1);
				} else if (mx > 278 && mx < 301) {
					ButtonClick();
					oapiSendMFDKey(mfd, OAPI_KEY_GRAVE);
				}
			}
		} else if (mx > 8 && mx < 31 && my > 46 && my < 247) {
			if ((my - 46) % 37 < 16) {
				int bt = (my - 46) / 37 + 0;
				if (event & PANEL_MOUSE_LBDOWN)
					ButtonClick();
				oapiProcessMFDButton (mfd, bt, event);
			}
		} else if (mx > 326 && mx < 349 && my > 46 && my < 247) {
			if ((my - 46) % 37 < 16) {
				int bt = (my - 46) / 37 + 6;
				if (event & PANEL_MOUSE_LBDOWN)
					ButtonClick();
				oapiProcessMFDButton (mfd, bt, event);
			}
		}
	} else if (event & PANEL_MOUSE_LBDOWN) {
		ButtonClick();
		oapiToggleMFD_on(mfd);
	}
}

void Saturn::SwitchClick()

{
	Sclick.play(NOLOOP,255);
}

void Saturn::ButtonClick()

{
	Bclick.play(NOLOOP,255);
}

void Saturn::GuardClick()

{
	Gclick.play(NOLOOP,255);
}

void Saturn::ProbeSound()

{
	Psound.play(NOLOOP,255);
}

void Saturn::CabinFanSound()

{
	double volume = 0.0;

	//
	// We base the volume on the number of fans and the power supply to them.
	//

	if (CabinFan1Switch) {
		volume += ECSCabinFanAC1ACircuitBraker.Voltage();
		volume += ECSCabinFanAC1BCircuitBraker.Voltage();
		volume += ECSCabinFanAC1CCircuitBraker.Voltage();
	}

	if (CabinFan2Switch) {
		volume += ECSCabinFanAC2ACircuitBraker.Voltage();
		volume += ECSCabinFanAC2BCircuitBraker.Voltage();
		volume += ECSCabinFanAC2CCircuitBraker.Voltage();
	}

	//
	// Scale volume appropriately based on the expected max voltage (115V per phase)
	//

	CabinFans.play(LOOP, (int) ((64.0 * volume / 400.0) + 127.0));
}

void Saturn::StopCabinFanSound()

{
	CabinFans.stop();
}

//
// Signal a master alarm until reset.
//

void Saturn::MasterAlarm()

{
	//
	// Enable master alarm.
	//

	cws.SetMasterAlarm(true);
}

//
// And stop it.
//

void Saturn::StopMasterAlarm()

{
	SMasterAlarm.stop();
	cws.SetMasterAlarm(false);
}

void Saturn::RenderS1bEngineLight(bool EngineOn, SURFHANDLE dest, SURFHANDLE src, int xoffs, int yoffs)

{
	if (EngineOn)
	{
		oapiBlt(dest, src, xoffs, yoffs, xoffs, yoffs, 29, 29);
	}
	else
	{
		oapiBlt(dest, src, xoffs, yoffs, xoffs + 101, yoffs, 29, 29);
	}
}

bool Saturn::clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf)

{

	HDC hDC;
	HGDIOBJ brush = NULL;
	HGDIOBJ pen = NULL;


	// Enable this to trace the redraws, but then it's running horrible slow!
	// char tracebuffer[100];
	// sprintf(tracebuffer, "Saturn::clbkPanelRedrawEvent id %i", id);
	// TRACESETUP(tracebuffer);

	//
	// Note: if you crash in this function with a NULL surf handle, odds are you screwed up
	// the region definition so maxX < minX or maxY < minY.
	//

	//
	// Special handling illuminated "sequencer switches".
	// \todo This should really be moved into the switch code.
	//

	if (LAUNCHIND[0])
	{
		if (EDSSwitch.GetState())
			LiftoffNoAutoAbortSwitch.SetOffset(78, 81);
		else
			LiftoffNoAutoAbortSwitch.SetOffset(234, 81);
	}
	else 
	{
		LiftoffNoAutoAbortSwitch.SetOffset(0, 81);
	}

	//
	// Special handling for docking panel
	//

	if (id == AID_SM_RCS_MODE) {
		if (PanelId == SATPANEL_LEFT_RNDZ_WINDOW) {
			if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
				oapiBlt(surf, srf[SRF_SM_RCS_MODE], 0, 0, 0, 0, 75, 73, SURF_PREDEF_CK);
				OrbiterAttitudeToggle.SetVisible(true);
			} else {
				OrbiterAttitudeToggle.SetVisible(false);
			}
		} else {
			OrbiterAttitudeToggle.SetVisible(true);
		}

	}

	//
	// Process all the generic switches.
	//

	if (MainPanel.DrawRow(id, surf, PanelFlashOn))
		return true;

	//
	// Now special case the rest.
	//

	switch (id) {
	
	
	case AID_CSM_MNPNL_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_MNPNL_WDW_LES], 0, 0, 0, 0, 355, 155, SURF_PREDEF_CK);
		}
		return true;

	case AID_CSM_RNDZ_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_RNDZ_WDW_LES], 0, 0, 0, 0, 638, 732, SURF_PREDEF_CK);
		}
		return true;

	case AID_CSM_RIGHT_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_RIGHT_WDW_LES], 0, 0, 0, 0, 506, 483, SURF_PREDEF_CK);
		}
		return true;

	case AID_CSM_LEFT_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_LEFT_WDW_LES], 0, 0, 0, 0, 506, 483, SURF_PREDEF_CK);
		}
		return true;

	case AID_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_DSKY]);
		return true;

	case AID_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP]);
		return true;

	case AID_DSKY_KEY:
		dsky.RenderKeys(surf, srf[SRF_DSKYKEY]);
		return true;

	// ASCP
	case AID_ASCPDISPLAYROLL:
		ascp.PaintRollDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_ASCPDISPLAYPITCH:
		ascp.PaintPitchDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_ASCPDISPLAYYAW:
		ascp.PaintYawDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_ASCPROLL:
		ascp.PaintRoll(surf, srf[SRF_THUMBWHEEL_SMALL]);
		return true;

	case AID_ASCPPITCH:
		ascp.PaintPitch(surf, srf[SRF_THUMBWHEEL_SMALL]);
		return true;

	case AID_ASCPYAW:
		ascp.PaintYaw(surf, srf[SRF_THUMBWHEEL_SMALL]);
		return true;

	// FDAIs
	case AID_FDAI_LEFT:
		if (!fdaiDisabled){  // Is this FDAI enabled?
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;
			int no_att = 0;
			switch(FDAISelectSwitch.GetState()){
				case THREEPOSSWITCH_UP:     // 1+2 - FDAI1 shows IMU ATT / CMC ERR
					euler_rates = gdc.rates;					
					attitude = imu.GetTotalAttitude();
					errors = eda.ReturnCMCErrorNeedles();
					break;
				case THREEPOSSWITCH_DOWN:   // 1 -- ALTERNATE DIRECT MODE
					euler_rates = gdc.rates;					
					switch(FDAISourceSwitch.GetState()){
						case THREEPOSSWITCH_UP:   // IMU
							attitude = imu.GetTotalAttitude();
							errors = eda.ReturnCMCErrorNeedles();
							break;
						case THREEPOSSWITCH_CENTER: // ATT SET (ALTERNATE ATT-SET MODE)
							// Get attutude
							if(FDAIAttSetSwitch.GetState() == TOGGLESWITCH_UP){
								attitude = imu.GetTotalAttitude();
							}else{
								attitude = gdc.GetAttitude();
							}
							errors = eda.AdjustErrorsForRoll(attitude, eda.ReturnASCPError(attitude));
							break;
						case THREEPOSSWITCH_DOWN: // GDC
							attitude = gdc.GetAttitude();
							errors = eda.AdjustErrorsForRoll(attitude, eda.ReturnBMAG1Error());
							break;
					}
					break;				
				case THREEPOSSWITCH_CENTER: // 2
					attitude = _V(0,0,0);   // No
					errors = _V(0,0,0);
					euler_rates = gdc.rates;
					// euler_rates = _V(0,0,0); // Does not disconnect rate inputs?
					no_att = 1;
					break;
			}
			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			if(errors.x > 41){ errors.x = 41; }else{ if(errors.x < -41){ errors.x = -41; }}
			if(errors.y > 41){ errors.y = 41; }else{ if(errors.y < -41){ errors.y = -41; }}
			if(errors.z > 41){ errors.z = 41; }else{ if(errors.z < -41){ errors.z = -41; }}
			fdaiLeft.PaintMe(attitude, no_att, euler_rates, errors, FDAIScaleSwitch.GetState(), surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);			
		}
		return true;

	case AID_FDAI_RIGHT:
		if (!fdaiDisabled){  // Is this FDAI enabled?
			int no_att = 0;
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;
			switch(FDAISelectSwitch.GetState()){
				case THREEPOSSWITCH_UP:     // 1+2 - FDAI2 shows GDC ATT / BMAG1 ERR
					attitude = gdc.GetAttitude();
					euler_rates = gdc.rates;
					errors = eda.AdjustErrorsForRoll(attitude, eda.ReturnBMAG1Error());
					break;
				case THREEPOSSWITCH_CENTER: // 2
					euler_rates = gdc.rates;
					// Get attitude to display
					switch(FDAISourceSwitch.GetState()){
						case THREEPOSSWITCH_UP:   // IMU
							attitude = imu.GetTotalAttitude();
							errors = eda.ReturnCMCErrorNeedles();
							break;
						case THREEPOSSWITCH_CENTER: // ATT SET (ALTERNATE ATT-SET MODE)
							if(FDAIAttSetSwitch.GetState() == TOGGLESWITCH_UP){
								attitude = imu.GetTotalAttitude();
							}else{
								attitude = gdc.GetAttitude();
							}
							errors = eda.AdjustErrorsForRoll(attitude,eda.ReturnASCPError(attitude));
							break;
						case THREEPOSSWITCH_DOWN: // GDC
							attitude = gdc.GetAttitude();							
							errors = eda.AdjustErrorsForRoll(attitude, eda.ReturnBMAG1Error());
							break;
					}
					break;
				case THREEPOSSWITCH_DOWN:   // 1
					attitude = _V(0,0,0);   // No
					errors = _V(0,0,0);
					euler_rates = gdc.rates;
					// Does not null rates?
					no_att = 1;
					break;
			}
			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			if(errors.x > 41){ errors.x = 41; }else{ if(errors.x < -41){ errors.x = -41; }}
			if(errors.y > 41){ errors.y = 41; }else{ if(errors.y < -41){ errors.y = -41; }}
			if(errors.z > 41){ errors.z = 41; }else{ if(errors.z < -41){ errors.z = -41; }}
			fdaiRight.PaintMe(attitude, no_att, euler_rates, errors, FDAIScaleSwitch.GetState(), surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);
		}
		return true;

	case AID_DSKY2_LIGHTS:
		dsky2.RenderLights(surf, srf[SRF_DSKY]);
		return true;

	case AID_DSKY2_DISPLAY:
		dsky2.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP]);
		return true;

	case AID_DSKY2_KEY:
		dsky2.RenderKeys(surf, srf[SRF_DSKYKEY]);
		return true;

	case AID_ABORT_BUTTON:
		if (ABORT_IND) {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 62, 0, 62, 31);
		} else {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 0, 0, 62, 31);
		}
		return true;

	case AID_LV_ENGINE_LIGHTS:
		{
			if (SI_EngineNum > 5)
			{
				RenderS1bEngineLight(ENGIND[0], surf, srf[SRF_LV_ENG_S1B], 64, 42);
				RenderS1bEngineLight(ENGIND[1], surf, srf[SRF_LV_ENG_S1B], 64, 98);
				RenderS1bEngineLight(ENGIND[2], surf, srf[SRF_LV_ENG_S1B], 8, 98);
				RenderS1bEngineLight(ENGIND[3], surf, srf[SRF_LV_ENG_S1B], 7, 43);
				RenderS1bEngineLight(ENGIND[4], surf, srf[SRF_LV_ENG_S1B], 36, 41);
				RenderS1bEngineLight(ENGIND[5], surf, srf[SRF_LV_ENG_S1B], 51, 69);
				RenderS1bEngineLight(ENGIND[6], surf, srf[SRF_LV_ENG_S1B], 36, 98);
				RenderS1bEngineLight(ENGIND[7], surf, srf[SRF_LV_ENG_S1B], 22, 69);
			}
			else
			{
				if (ENGIND[0]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,44,55,44,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,44,157,44,27,27);
				}

				if (ENGIND[1]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,98,55,98,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,98,157,98,27,27);
				}
				if (ENGIND[2]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,98,20,98,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,98,122,98,27,27);
				}
				if (ENGIND[3]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,44,20,44,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,44,122,44,27,27);
				}
				if (ENGIND[4]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],37,71,37,71,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],37,71,140,71,27,27);
				}
			}
		}

		if (LVRateLight) 
		{
			oapiBlt(surf,srf[SRF_LV_ENG],6,4,6,4,27,27);
		}
		else 
		{
			oapiBlt(surf,srf[SRF_LV_ENG],6,4,108,4,27,27);
		}

		//
		// Saturn 1b doesn't have an SII sep light.
		//

		if (SaturnType == SAT_SATURNV)
		{
			if (SIISepState)
			{
				oapiBlt(surf,srf[SRF_LV_ENG],37,4,37,4,27,27);
			}
			else
			{
				oapiBlt(surf,srf[SRF_LV_ENG],37,4,139,4,27,27);
			}
		}

		if (LVGuidLight)
		{
			oapiBlt(surf,srf[SRF_LV_ENG],69,4,69,4,27,27);
		}
		else
		{
			oapiBlt(surf,srf[SRF_LV_ENG],69,4,171,4,27,27);
		}
		return true;

	// OPTICS
	case AID_OPTICSCLKAREASEXT:
		//write update stuff here

		oapiCameraSetCockpitDir (-optics.OpticsShaft, optics.SextTrunion - PI/2., true); //negative allows Optics shaft to rotate clockwise positive, the PI/2 allows rotation around the perpindicular axis
		//sprintf(oapiDebugString(), "Shaft %f, Trunion %f", optics.OpticsShaft/RAD, optics.SextTrunion/RAD);
		return true;

	case AID_OPTICSCLKAREATELE:
		//write update stuff here
		
		oapiCameraSetCockpitDir (-optics.OpticsShaft, optics.TeleTrunion - PI/2., true); //negative allows Optics shaft to rotate clockwise positive, the PI/2 allows rotation around the perpindicular axis
		//sprintf(oapiDebugString(), "Shaft %f, Trunion %f", optics.OpticsShaft/RAD, optics.TeleTrunion/RAD);
		return true;

	case AID_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_DIGITAL]);
		return true;

	case AID_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_EVENT_TIMER_DIGITS]);
		return true;

	case AID_ALTIMETER:
		RedrawPanel_Alt(surf);
		return true;

	case AID_ALTIMETER2:
		RedrawPanel_Alt2(surf);
		return true;

	case AID_MASTER_ALARM:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_LEFT);
		return true;

	case AID_MASTER_ALARM2:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_RIGHT);
		return true;

	case AID_MASTER_ALARM3:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], CWS_MASTERALARMPOSITION_NONE);
		return true;

	case AID_CWS_LIGHTS_LEFT:
		cws.RenderLights(surf, srf[SRF_CWSLIGHTS], true);
		return true;

	case AID_CWS_LIGHTS_RIGHT:
		cws.RenderLights(surf, srf[SRF_CWSLIGHTS], false);
		return true;

	case AID_MFDMAINLEFT:
		if (oapiGetMFDMode(MFD_LEFT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 358, 299);

			RedrawPanel_MFDButton(surf, MFD_LEFT, 0, 10, 47, 37);
			RedrawPanel_MFDButton(surf, MFD_LEFT, 1, 328, 47, 37);
		}
		return true;

	case AID_MFDMAINRIGHT:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 358, 299);

			RedrawPanel_MFDButton(surf, MFD_RIGHT, 0, 10, 47, 37);
			RedrawPanel_MFDButton(surf, MFD_RIGHT, 1, 328, 47, 37);
		}
		return true;

	case AID_MFDGNLEFT:
		if (oapiGetMFDMode(MFD_LEFT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 358, 299);

			RedrawPanel_MFDButton(surf, MFD_LEFT, 0, 10, 47, 37);
			RedrawPanel_MFDButton(surf, MFD_LEFT, 1, 328, 47, 37);
		}
		return true;

	case AID_MFDGNUSER1:
		if (oapiGetMFDMode(MFD_USER1) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 358, 299);

			RedrawPanel_MFDButton(surf, MFD_USER1, 0, 10, 47, 37);
			RedrawPanel_MFDButton(surf, MFD_USER1, 1, 328, 47, 37);
		}
		return true;

	case AID_MFDGNUSER2:
		if (oapiGetMFDMode(MFD_USER2) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 358, 299);

			RedrawPanel_MFDButton(surf, MFD_USER2, 0, 10, 47, 37);
			RedrawPanel_MFDButton(surf, MFD_USER2, 1, 328, 47, 37);
		}
		return true;

	case AID_MFDGNRIGHT:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 358, 299);

			RedrawPanel_MFDButton(surf, MFD_RIGHT, 0, 10, 47, 37);
			RedrawPanel_MFDButton(surf, MFD_RIGHT, 1, 328, 47, 37);
		}
		return true;

	case AID_MFDDOCK:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
			oapiBlt(surf, srf[SRF_MFDFRAME], 0, 0, 0, 0, 301, 251);

			RedrawPanel_MFDButton(surf, MFD_RIGHT, 0, 7, 38, 31);	// MFD_USER1
			RedrawPanel_MFDButton(surf, MFD_RIGHT, 1, 274, 38, 31);	// MFD_USER1
		}
		return true;

	case AID_MFDDOCK_POWER:
		if (oapiGetMFDMode(MFD_RIGHT) == MFD_NONE) {	// MFD_USER1
			oapiBlt(surf, srf[SRF_MFDPOWER], 0, 0, 0, 0, 20, 15);
		}
		return true;

	case AID_COAS:
		if (coasEnabled) {
			oapiBlt(surf, srf[SRF_COAS], 0, 0, 0, 0, 683, 620, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_COAS], 0, 0, 0, 620, 683, 620, SURF_PREDEF_CK);
		}
		return true;

	case AID_SPS_LIGHT:
		if (ems.SPSThrustLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 30, 16, 30, 16);
		} else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 16, 30, 16);
		}
		return true;

	case AID_PT05G_LIGHT:
		if (ems.pt05GLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 30, 0, 30, 16);
		} else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 0, 30, 16);
		}
		return true;

	case AID_EMS_SCROLL_LEO:

		if(!(GTASwitch.IsUp())) {
			hDC = oapiGetDC (srf[SRF_EMS_SCROLL_LEO]);
			SetBkMode (hDC, TRANSPARENT);
			pen = SelectObject(hDC,GetStockObject(BLACK_PEN));
			Polyline(hDC, ems.ScribePntArray, ems.ScribePntCnt);
			SelectObject(hDC,pen);
			oapiReleaseDC (srf[SRF_EMS_SCROLL_LEO], hDC);
		}


		oapiBlt(surf, srf[SRF_EMS_SCROLL_LEO], 5, 4, ems.GetScrollOffset(), 0, 132, 143);
		oapiBlt(surf, srf[SRF_EMS_SCROLL_BORDER], 0, 0, 0, 0, 142, 150, SURF_PREDEF_CK);

		return true;

	case AID_EMS_RSI_BKGRND:

		oapiBlt(surf, srf[SRF_EMS_RSI_BKGRND], 0,0,0,0,86,84);

		switch (ems.LiftVectLight()) {
			case -1:
				oapiBlt(surf, srf[SRF_EMS_LIGHTS], 33, 8, 60, 6, 20, 6);
				break;
			case 1:
				oapiBlt(surf, srf[SRF_EMS_LIGHTS], 32, 69, 60, 22, 22, 10);
				break;
			case 0:
				oapiBlt(surf, srf[SRF_EMS_LIGHTS], 33, 8, 60, 0, 20, 6);
				oapiBlt(surf, srf[SRF_EMS_LIGHTS], 32, 69, 60, 12, 22, 10);
				break;
		}
		
		hDC = oapiGetDC (srf[SRF_EMS_RSI_BKGRND]);
		SetBkMode (hDC, TRANSPARENT);
		pen = SelectObject(hDC,GetStockObject(WHITE_PEN));
		Ellipse(hDC, 14,14,71,68);
		brush = SelectObject(hDC,GetStockObject(BLACK_BRUSH));
		Polygon(hDC, ems.RSITriangle, 3);
		SelectObject(hDC,pen);
		SelectObject(hDC,brush);
		oapiReleaseDC (srf[SRF_EMS_RSI_BKGRND], hDC);

		return true;
	
	case AID_EMSDVSETSWITCH:
		
		switch ((int)EMSDvSetSwitch.GetPosition()) {
			case 1:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 47, 0, 47, 85);
				break;
			case 2:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 0, 0, 47, 85);
				break;
			case 3:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 188, 0, 47, 85);
				break;
			case 4:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 141, 0, 47, 85);
				break;
			default:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 94, 0, 47, 85);
				break;
		}

		return true;

	case AID_OPTICS_DSKY:
		if (opticsDskyEnabled) {
			oapiBlt(surf, srf[SRF_OPTICS_DSKY], 0, 0, 0, 0, 303, 349);
			dsky2.RenderLights(surf, srf[SRF_DSKY], 27, 28, false);
			dsky2.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP], 171, 23);
			dsky2.RenderKeys(surf, srf[SRF_DSKYKEY], 7, 218);
		}
		return true;
	}
	return false;
}

void Saturn::clbkMFDMode (int mfd, int mode) {

	switch (mfd) {
	case MFD_LEFT:
		oapiTriggerPanelRedrawArea(SATPANEL_MAIN, AID_MFDMAINLEFT);
		oapiTriggerPanelRedrawArea(SATPANEL_MAIN_MIDDLE, AID_MFDMAINLEFT);
		oapiTriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNLEFT);
		break;

	case MFD_RIGHT:
		if (!MainPanelSplit) oapiTriggerPanelRedrawArea(SATPANEL_MAIN, AID_MFDMAINRIGHT);
		oapiTriggerPanelRedrawArea(SATPANEL_MAIN_MIDDLE, AID_MFDMAINRIGHT);
		oapiTriggerPanelRedrawArea(SATPANEL_MAIN_RIGHT, AID_MFDMAINRIGHT);
		oapiTriggerPanelRedrawArea(SATPANEL_LEFT_RNDZ_WINDOW, AID_MFDDOCK);
		oapiTriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNRIGHT);
		break;

	case MFD_USER1:
		oapiTriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNUSER1);
		break;

	case MFD_USER2:
		oapiTriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNUSER2);
		break;

	}
}

//
// Set switches to default state.
//

void Saturn::InitSwitches() {

	coasEnabled = false;
	opticsDskyEnabled = false;

	AccelGMeter.Register(PSH, "AccelGMeter", -1, 15, 1);

	THCRotary.AddPosition(1, 0);
	THCRotary.AddPosition(2, 30);
	THCRotary.AddPosition(3, 330);
	THCRotary.Register(PSH, "THCRotary", 1);

	LiftoffNoAutoAbortSwitch.Register(PSH, "LiftoffNoAutoAbortSwitch", false, false);
	LesMotorFireSwitch.Register(PSH, "LesMotorFireSwitch", false, false);
	CanardDeploySwitch.Register(PSH, "CanardDeploySwitch", false, false);
	CsmLvSepSwitch = false;						// saved in SSwitchState.Sswitch5
	CsmLvSepSwitch.SetGuardState(false);		// saved in CSwitchState.Cswitch5
	ApexCoverJettSwitch.Register(PSH, "ApexCoverJettSwitch", false, false);
	DrogueDeploySwitch.Register(PSH, "DrogueDeploySwitch", false, false);
	MainDeploySwitch.Register(PSH, "MainDeploySwitch", false, false);
	CmRcsHeDumpSwitch = false;					// saved in SSwitchState.CMRHDswitch
	CmRcsHeDumpSwitch.SetGuardState(false);		// saved in CSwitchState.CMRHGswitch

	if (!SkylabCM) {
		SivbLmSepSwitch = false;					// saved in RPSwitchState.RPswitch16
		SivbLmSepSwitch.SetGuardState(false);		// saved in RPSwitchState.RPCswitch
		SivbLmSepSwitch.SetSpringLoaded(SPRINGLOADEDSWITCH_DOWN);
	}

	MissionTimerSwitch.Register(PSH, "MissionTimerSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);
	CautionWarningModeSwitch.Register(PSH, "CautionWarningModeSwitch", THREEPOSSWITCH_UP);
	CautionWarningCMCSMSwitch.Register(PSH, "CautionWarningCMCSMSwitch", 1);
	CautionWarningPowerSwitch.Register(PSH, "CautionWarningPowerSwitch", THREEPOSSWITCH_CENTER);
	CautionWarningLightTestSwitch.Register(PSH, "CautionWarningLightTestSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	CabinFan1Switch.Register(PSH, "CabinFan1Switch", false);
	CabinFan2Switch.Register(PSH, "CabinFan2Switch", false);
	H2Heater1Switch.Register(PSH, "H2Heater1Switch", THREEPOSSWITCH_UP);
	H2Heater2Switch.Register(PSH, "H2Heater2Switch", THREEPOSSWITCH_UP);
	O2Heater1Switch.Register(PSH, "O2Heater1Switch", THREEPOSSWITCH_UP);
	O2Heater2Switch.Register(PSH, "O2Heater2Switch", THREEPOSSWITCH_UP);
	O2PressIndSwitch.Register(PSH, "O2PressIndSwitch", true);
	H2Fan1Switch.Register(PSH, "H2Fan1Switch", THREEPOSSWITCH_UP);
	H2Fan2Switch.Register(PSH, "H2Fan2Switch", THREEPOSSWITCH_UP);
	O2Fan1Switch.Register(PSH, "O2Fan1Switch", THREEPOSSWITCH_UP);
	O2Fan2Switch.Register(PSH, "O2Fan2Switch", THREEPOSSWITCH_UP);

	SCContSwitch.Register(PSH, "SCContSwitch", false);
	CMCModeSwitch.Register(PSH, "CMCModeSwitch", THREEPOSSWITCH_DOWN);

	SCSTvcPitchSwitch.Register(PSH, "SCSTvcPitchSwitch", THREEPOSSWITCH_CENTER);
	SCSTvcYawSwitch.Register(PSH, "SCSTvcYawSwitch", THREEPOSSWITCH_CENTER);

	Pitch1Switch.Register(PSH, "Pitch1Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	Pitch2Switch.Register(PSH, "Pitch2Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	Yaw1Switch.Register(PSH, "Yaw1Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	Yaw2Switch.Register(PSH, "Yaw2Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);

	EMSRollSwitch.Register(PSH, "EMSRollSwitch", false);
	GSwitch.Register(PSH, "GSwitch", false);

	LVSPSPcIndicatorSwitch.Register(PSH, "LVSPSPcIndicatorSwitch", false);
	LVFuelTankPressIndicatorSwitch.Register(PSH, "LVFuelTankPressIndicatorSwitch", false);

	TVCGimbalDrivePitchSwitch.Register(PSH, "TVCGimbalDrivePitchSwitch", THREEPOSSWITCH_CENTER);
	TVCGimbalDriveYawSwitch.Register(PSH, "TVCGimbalDriveYawSwitch", THREEPOSSWITCH_CENTER);

	RunEVALightSwitch.Register(PSH, "RunEVALightSwitch", false);
	RndzLightSwitch.Register(PSH, "RndzLightSwitch", THREEPOSSWITCH_CENTER);
	TunnelLightSwitch.Register(PSH, "TunnelLightSwitch", false);

	LMPowerSwitch.Register(PSH, "LMPowerSwitch", THREEPOSSWITCH_CENTER);

	PostLDGVentValveLever.Register(PSH, "PostLDGVentValveLever", 1);

	GDCAlignButton.Register(PSH, "GDCAlignButton", false);

	GHATrackSwitch.Register(PSH, "GHATrackSwitch", THREEPOSSWITCH_CENTER);
	GHABeamSwitch.Register(PSH, "GHABeamSwitch", THREEPOSSWITCH_CENTER);

	GHAPowerSwitch.Register(PSH, "GHAPowerSwitch", THREEPOSSWITCH_CENTER);
	GHAServoElecSwitch.Register(PSH, "GHAServoElecSwitch", false);
	
	FuelCellPhIndicator.Register(PSH, "FuelCellPhIndicator", false);
	FuelCellRadTempIndicator.Register(PSH, "FuelCellRadTempIndicator", false);

	FuelCellRadiators1Indicator.Register(PSH, "FuelCellRadiators1Indicator", false);
	FuelCellRadiators2Indicator.Register(PSH, "FuelCellRadiators2Indicator", false);
	FuelCellRadiators3Indicator.Register(PSH, "FuelCellRadiators3Indicator", false);
	FuelCellRadiators1Switch.Register(PSH, "FuelCellRadiators1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellRadiators2Switch.Register(PSH, "FuelCellRadiators2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellRadiators3Switch.Register(PSH, "FuelCellRadiators3Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	FuelCellIndicatorsSwitch.AddPosition(1, 330);
	FuelCellIndicatorsSwitch.AddPosition(2,   0);
	FuelCellIndicatorsSwitch.AddPosition(3,  30);
	FuelCellIndicatorsSwitch.Register(PSH, "FuelCellIndicatorsSwitch", 1);

	FuelCellHeater1Switch.Register(PSH, "FuelCellHeater1Switch", true);
	FuelCellHeater2Switch.Register(PSH, "FuelCellHeater2Switch", true);
	FuelCellHeater3Switch.Register(PSH, "FuelCellHeater3Switch", true);

	FuelCellPurge1Switch.Register(PSH, "FuelCellPurge1Switch", THREEPOSSWITCH_CENTER);
	FuelCellPurge2Switch.Register(PSH, "FuelCellPurge2Switch", THREEPOSSWITCH_CENTER);
	FuelCellPurge3Switch.Register(PSH, "FuelCellPurge3Switch", THREEPOSSWITCH_CENTER);

	FuelCellReactants1Indicator.Register(PSH, "FuelCellReactants1Indicator", false);
	FuelCellReactants2Indicator.Register(PSH, "FuelCellReactants2Indicator", false);
	FuelCellReactants3Indicator.Register(PSH, "FuelCellReactants3Indicator", false);
	FuelCellReactants1Switch.Register(PSH, "FuelCellReactants1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellReactants2Switch.Register(PSH, "FuelCellReactants2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellReactants3Switch.Register(PSH, "FuelCellReactants3Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	FCReacsValvesSwitch.Register(PSH, "FCReacsValvesSwitch", true);
	H2PurgeLineSwitch.Register  (PSH, "H2PurgeLineSwitch",   false);

	FuelCellPumps1Switch.Register(PSH, "FuelCellPumps1Switch", THREEPOSSWITCH_UP);
	FuelCellPumps2Switch.Register(PSH, "FuelCellPumps2Switch", THREEPOSSWITCH_DOWN);
	FuelCellPumps3Switch.Register(PSH, "FuelCellPumps3Switch", THREEPOSSWITCH_DOWN);

	SuitCompressor1Switch.Register(PSH, "SuitCompressor1Switch", THREEPOSSWITCH_CENTER);
	SuitCompressor2Switch.Register(PSH, "SuitCompressor2Switch", THREEPOSSWITCH_CENTER);

	RightCOASPowerSwitch.Register(PSH, "RightCOASPowerSwitch", false);

	AudioControlSwitch.Register(PSH, "AudioControlSwitch", false);

	SuidPowerSwitch.Register(PSH, "SuidPowerSwitch", false);

	RightUtilityPowerSwitch.Register(PSH, "RightUtilityPowerSwitch", false);

	RightDockingTargetSwitch.Register(PSH, "RightDockingTargetSwitch", THREEPOSSWITCH_CENTER);

	RightModeIntercomSwitch.Register(PSH, "RightModeIntercomSwitch", THREEPOSSWITCH_CENTER);

	RightAudioPowerSwitch.Register(PSH, "RightAudioPowerSwitch", THREEPOSSWITCH_CENTER);

	RightPadCommSwitch.Register(PSH, "RightPadCommSwitch", THREEPOSSWITCH_CENTER);

	RightIntercomSwitch.Register(PSH, "RightIntercomSwitch", THREEPOSSWITCH_CENTER);

	RightSBandSwitch.Register(PSH, "RightSBandSwitch", THREEPOSSWITCH_CENTER);

	RightVHFAMSwitch.Register(PSH, "RightVHFAMSwitch", THREEPOSSWITCH_CENTER);

	GNPowerSwitch.Register(PSH, "GNPowerSwitch", THREEPOSSWITCH_CENTER);

	MainBusTieBatAcSwitch.Register(PSH, "MainBusTieBatAcSwitch", THREEPOSSWITCH_DOWN);
	MainBusTieBatBcSwitch.Register(PSH, "MainBusTieBatBcSwitch", THREEPOSSWITCH_DOWN);

	BatCHGRSwitch.Register(PSH, "BatCHGRSwitch", THREEPOSSWITCH_UP);

	NonessBusSwitch.Register(PSH, "NonessBusSwitch", THREEPOSSWITCH_CENTER);

	InteriorLightsFloodDimSwitch.Register(PSH, "InteriorLightsFloodDimSwitch", false);
	InteriorLightsFloodFixedSwitch.Register(PSH, "InteriorLightsFloodFixedSwitch", false);

	FloatBagSwitch1.Register(PSH, "FloatBagSwitch1", THREEPOSSWITCH_DOWN);
	FloatBagSwitch2.Register(PSH, "FloatBagSwitch2", THREEPOSSWITCH_DOWN);
	FloatBagSwitch3.Register(PSH, "FloatBagSwitch3", THREEPOSSWITCH_DOWN);

	Logic1Switch.Register(PSH, "Logic1Switch", false);
	Logic2Switch.Register(PSH, "Logic2Switch", false);
	PyroArmASwitch.Register(PSH, "PyroArmASwitch", false);
	PyroArmBSwitch.Register(PSH, "PyroArmBSwitch", false);

	EDSPowerSwitch.Register(PSH, "EDSPowerSwitch", false);

	TVCServoPower1Switch.Register(PSH, "TVCServoPower1Switch", THREEPOSSWITCH_CENTER);
	TVCServoPower2Switch.Register(PSH, "TVCServoPower2Switch", THREEPOSSWITCH_CENTER);

	LogicPowerSwitch.Register(PSH, "LogicPowerSwitch", true);

	SIGCondDriverBiasPower1Switch.Register(PSH, "SIGCondDriverBiasPower1Switch", THREEPOSSWITCH_CENTER);
	SIGCondDriverBiasPower2Switch.Register(PSH, "SIGCondDriverBiasPower2Switch", THREEPOSSWITCH_CENTER);

	SPSGaugingSwitch.Register(PSH, "SPSGaugingSwitch", THREEPOSSWITCH_UP);

	TelcomGroup1Switch.Register(PSH, "TelcomGroup1Switch", THREEPOSSWITCH_CENTER);
	TelcomGroup2Switch.Register(PSH, "TelcomGroup2Switch", THREEPOSSWITCH_CENTER);

	LeftCOASPowerSwitch.Register(PSH, "LeftCOASPowerSwitch", false);

	LeftUtilityPowerSwitch.Register(PSH, "LeftUtilityPowerSwitch", false);

	PostLandingBCNLTSwitch.Register(PSH, "PostLandingBCNLTSwitch", THREEPOSSWITCH_CENTER);

	PostLandingDYEMarkerSwitch.Register(PSH, "PostLandingDYEMarkerSwitch", false, false);

	GTASwitch.Register(PSH, "GTASwitch", false, false);
	
	PostLandingVentSwitch.Register(PSH, "PostLandingVentSwitch", THREEPOSSWITCH_DOWN);

	LeftModeIntercomVOXSensThumbwheelSwitch.Register(PSH, "LeftModeIntercomVOXSensThumbwheelSwitch", 2, 9);

	LeftPowerMasterVolumeThumbwheelSwitch.Register(PSH, "LeftPowerMasterVolumeThumbwheelSwitch", 9, 9);

	LeftPadCommVolumeThumbwheelSwitch.Register(PSH, "LeftPadCommVolumeThumbwheelSwitch", 2, 9);

	LeftIntercomVolumeThumbwheelSwitch.Register(PSH, "LeftIntercomVolumeThumbwheelSwitch", 2, 9);

	LeftSBandVolumeThumbwheelSwitch.Register(PSH, "LeftSBandVolumeThumbwheelSwitch", 2, 9);

	LeftVHFAMVolumeThumbwheelSwitch.Register(PSH, "LeftVHFAMVolumeThumbwheelSwitch", 2, 9);

	LeftModeIntercomSwitch.Register(PSH, "LeftModeIntercomSwitch", THREEPOSSWITCH_CENTER);

	LeftAudioPowerSwitch.Register(PSH, "LeftAudioPowerSwitch", THREEPOSSWITCH_CENTER);

	LeftPadCommSwitch.Register(PSH, "LeftPadCommSwitch", THREEPOSSWITCH_CENTER);

	LeftIntercomSwitch.Register(PSH, "LeftIntercomSwitch", THREEPOSSWITCH_CENTER);

	LeftSBandSwitch.Register(PSH, "LeftSBandSwitch", THREEPOSSWITCH_CENTER);

	LeftVHFAMSwitch.Register(PSH, "LeftVHFAMSwitch", THREEPOSSWITCH_CENTER);

	LeftAudioControlSwitch.Register(PSH, "LeftAudioControlSwitch", THREEPOSSWITCH_DOWN);

	LeftSuitPowerSwitch.Register(PSH, "LeftSuitPowerSwitch", false);

	VHFRNGSwitch.Register(PSH, "VHFRNGSwitch", false);

	AcRollA1Switch.Register(PSH, "AcRollA1Switch", THREEPOSSWITCH_CENTER);
	AcRollC1Switch.Register(PSH, "AcRollC1Switch", THREEPOSSWITCH_CENTER);
	AcRollA2Switch.Register(PSH, "AcRollA2Switch", THREEPOSSWITCH_CENTER);
	AcRollC2Switch.Register(PSH, "AcRollC2Switch", THREEPOSSWITCH_CENTER);
	BdRollB1Switch.Register(PSH, "BdRollB1Switch", THREEPOSSWITCH_CENTER);
	BdRollD1Switch.Register(PSH, "BdRollD1Switch", THREEPOSSWITCH_CENTER);
	BdRollB2Switch.Register(PSH, "BdRollB2Switch", THREEPOSSWITCH_CENTER);
	BdRollD2Switch.Register(PSH, "BdRollD2Switch", THREEPOSSWITCH_CENTER);
	PitchA3Switch.Register(PSH, "PitchA3Switch", THREEPOSSWITCH_CENTER);
	PitchC3Switch.Register(PSH, "PitchC3Switch", THREEPOSSWITCH_CENTER);
	PitchA4Switch.Register(PSH, "PitchA4Switch", THREEPOSSWITCH_CENTER);
	PitchC4Switch.Register(PSH, "PitchC4Switch", THREEPOSSWITCH_CENTER);
	YawB3Switch.Register(PSH, "YawB3Switch", THREEPOSSWITCH_CENTER);
	YawD3Switch.Register(PSH, "YawD3Switch", THREEPOSSWITCH_CENTER);
	YawB4Switch.Register(PSH, "YawB4Switch", THREEPOSSWITCH_CENTER);
	YawD4Switch.Register(PSH, "YawD4Switch", THREEPOSSWITCH_CENTER);

	FloodDimSwitch.Register(PSH, "FloodDimSwitch", false);

	FloodFixedSwitch.Register(PSH, "FloodFixedSwitch", THREEPOSSWITCH_CENTER);

	//RightWindowCoverSwitch.Register(PSH, "RightWindowCoverSwitch", false);

	SBandNormalXPDRSwitch.Register(PSH, "SBandNormalXPDRSwitch", THREEPOSSWITCH_CENTER);
	SBandNormalPwrAmpl1Switch.Register(PSH, "SBandNormalPwrAmpl1Switch", THREEPOSSWITCH_CENTER);
	SBandNormalPwrAmpl2Switch.Register(PSH, "SBandNormalPwrAmpl2Switch", THREEPOSSWITCH_CENTER);
	SBandNormalMode1Switch.Register(PSH, "SBandNormalMode1Switch", THREEPOSSWITCH_CENTER);
	SBandNormalMode2Switch.Register(PSH, "SBandNormalMode2Switch", THREEPOSSWITCH_CENTER);
	SBandNormalMode3Switch.Register(PSH, "SBandNormalMode3Switch", false);

	PwrAmplTB.Register(PSH, "PwrAmplTB", false);
	DseTapeTB.Register(PSH, "DseTapeTB", false);

	SBandAuxSwitch1.Register(PSH, "SBandAuxSwitch1", THREEPOSSWITCH_CENTER);
	SBandAuxSwitch2.Register(PSH, "SBandAuxSwitch2", THREEPOSSWITCH_CENTER);

	UPTLMSwitch1.Register(PSH, "UPTLMSwitch1", TOGGLESWITCH_DOWN);
	UPTLMSwitch2.Register(PSH, "UPTLMSwitch2", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);

	SBandAntennaSwitch1.Register(PSH, "SBandAntennaSwitch1", THREEPOSSWITCH_CENTER);
	SBandAntennaSwitch2.Register(PSH, "SBandAntennaSwitch2", THREEPOSSWITCH_CENTER);

	VHFAMASwitch.Register(PSH, "VHFAMASwitch", THREEPOSSWITCH_CENTER);
	VHFAMBSwitch.Register(PSH, "VHFAMBSwitch", THREEPOSSWITCH_CENTER);
	RCVOnlySwitch.Register(PSH, "RCVOnlySwitch", THREEPOSSWITCH_CENTER);
	VHFBeaconSwitch.Register(PSH, "VHFBeaconSwitch", false);
	VHFRangingSwitch.Register(PSH, "VHFRangingSwitch", false);

	TapeRecorder1Switch.Register(PSH, "TapeRecorder1Switch", false);
	TapeRecorder2Switch.Register(PSH, "TapeRecorder2Switch", THREEPOSSWITCH_CENTER);
	TapeRecorder3Switch.Register(PSH, "TapeRecorder3Switch", THREEPOSSWITCH_CENTER);

	SCESwitch.Register(PSH, "SCESwitch", THREEPOSSWITCH_CENTER);
	PMPSwitch.Register(PSH, "PMPSwitch", THREEPOSSWITCH_CENTER);

	PCMBitRateSwitch.Register(PSH, "PCMBitRateSwitch", false);
	DummySwitch.Register(PSH, "PMPSwitch", THREEPOSSWITCH_CENTER);

	MnA1Switch.Register(PSH, "MnA1Switch", true);
	MnB2Switch.Register(PSH, "MnB2Switch", true);
	MnA3Switch.Register(PSH, "MnA3Switch", THREEPOSSWITCH_CENTER);
	AcBus1Switch1.Register(PSH, "AcBus1Switch1", true);
	AcBus1Switch2.Register(PSH, "AcBus1Switch2", false);
	AcBus1Switch3.Register(PSH, "AcBus1Switch3", false);
	AcBus1ResetSwitch.Register(PSH, "AcBus1ResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	AcBus2Switch1.Register(PSH, "AcBus2Switch1", false);
	AcBus2Switch2.Register(PSH, "AcBus2Switch2", true);
	AcBus2Switch3.Register(PSH, "AcBus2Switch3", false);
	AcBus2ResetSwitch.Register(PSH, "AcBus2ResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	
	MainBusASwitch1.Register(PSH, "MainBusASwitch1", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusASwitch2.Register(PSH, "MainBusASwitch2", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusASwitch3.Register(PSH, "MainBusASwitch3", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusAResetSwitch.Register(PSH, "MainBusAResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBSwitch1.Register(PSH, "MainBusBSwitch1", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBSwitch2.Register(PSH, "MainBusBSwitch2", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBSwitch3.Register(PSH, "MainBusBSwitch3", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBResetSwitch.Register(PSH, "MainBusBResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBIndicator1.Register(PSH, "MainBusBIndicator1", true);
	MainBusBIndicator2.Register(PSH, "MainBusBIndicator2", true);
	MainBusBIndicator3.Register(PSH, "MainBusBIndicator3", true);
	MainBusAIndicator1.Register(PSH, "MainBusAIndicator1", true);
	MainBusAIndicator2.Register(PSH, "MainBusAIndicator2", true);
	MainBusAIndicator3.Register(PSH, "MainBusAIndicator3", true);
		
	MissionTimerHoursSwitch.Register(PSH, "MissionTimerHoursSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	MissionTimerMinutesSwitch.Register(PSH, "MissionTimerMinutesSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	MissionTimerSecondsSwitch.Register(PSH, "MissionTimerSecondsSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	SMRCSHelium1ASwitch.Register(PSH, "SMRCSHelium1ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium1BSwitch.Register(PSH, "SMRCSHelium1BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium1CSwitch.Register(PSH, "SMRCSHelium1CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium1DSwitch.Register(PSH, "SMRCSHelium1DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	SMRCSHelium1ATalkback.Register(PSH, "SMRCSHelium1ATalkback", true);
	SMRCSHelium1BTalkback.Register(PSH, "SMRCSHelium1BTalkback", true);
	SMRCSHelium1CTalkback.Register(PSH, "SMRCSHelium1CTalkback", true);
	SMRCSHelium1DTalkback.Register(PSH, "SMRCSHelium1DTalkback", true);

	SMRCSHelium2ASwitch.Register(PSH, "SMRCSHelium2ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium2BSwitch.Register(PSH, "SMRCSHelium2BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium2CSwitch.Register(PSH, "SMRCSHelium2CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium2DSwitch.Register(PSH, "SMRCSHelium2DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	SMRCSHelium2ATalkback.Register(PSH, "SMRCSHelium2ATalkback", true);
	SMRCSHelium2BTalkback.Register(PSH, "SMRCSHelium2BTalkback", true);
	SMRCSHelium2CTalkback.Register(PSH, "SMRCSHelium2CTalkback", true);
	SMRCSHelium2DTalkback.Register(PSH, "SMRCSHelium2DTalkback", true);

	SMRCSProp1ASwitch.Register(PSH, "SMRCSProp1ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp1BSwitch.Register(PSH, "SMRCSProp1BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp1CSwitch.Register(PSH, "SMRCSProp1CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp1DSwitch.Register(PSH, "SMRCSProp1DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	SMRCSProp1ATalkback.Register(PSH, "SMRCSProp1ATalkback", false);
	SMRCSProp1BTalkback.Register(PSH, "SMRCSProp1BTalkback", false);
	SMRCSProp1CTalkback.Register(PSH, "SMRCSProp1CTalkback", false);
	SMRCSProp1DTalkback.Register(PSH, "SMRCSProp1DTalkback", false);

	SMRCSProp2ASwitch.Register(PSH, "SMRCSProp2ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp2BSwitch.Register(PSH, "SMRCSProp2BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp2CSwitch.Register(PSH, "SMRCSProp2CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp2DSwitch.Register(PSH, "SMRCSProp2DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	SMRCSProp2ATalkback.Register(PSH, "SMRCSProp2ATalkback", false);
	SMRCSProp2BTalkback.Register(PSH, "SMRCSProp2BTalkback", false);
	SMRCSProp2CTalkback.Register(PSH, "SMRCSProp2CTalkback", false);
	SMRCSProp2DTalkback.Register(PSH, "SMRCSProp2DTalkback", false);

	SPSInjectorValve1Indicator.Register(PSH, "SPSInjectorValve1Indicator", false);
	SPSInjectorValve2Indicator.Register(PSH, "SPSInjectorValve2Indicator", false);
	SPSInjectorValve3Indicator.Register(PSH, "SPSInjectorValve3Indicator", false);
	SPSInjectorValve4Indicator.Register(PSH, "SPSInjectorValve4Indicator", false);

	SPSOxidFlowValveMaxIndicator.Register(PSH, "SPSOxidFlowValveMaxIndicator", true);
	SPSOxidFlowValveMinIndicator.Register(PSH, "SPSOxidFlowValveMinIndicator", true);

	SPSOxidFlowValveSwitch.Register(PSH, "SPSOxidFlowValveSwitch", THREEPOSSWITCH_CENTER);	
	SPSOxidFlowValveSelectorSwitch.Register(PSH, "SPSOxidFlowValveSelectorSwitch", TOGGLESWITCH_UP);
	SPSPugModeSwitch.Register(PSH, "SPSPugModeSwitch", THREEPOSSWITCH_CENTER);

	SPSHeliumValveAIndicator.Register(PSH, "SPSHeliumValveAIndicator", true);
	SPSHeliumValveBIndicator.Register(PSH, "SPSHeliumValveBIndicator", true);

	SPSHeliumValveASwitch.Register(PSH, "SPSHeliumValveASwitch", THREEPOSSWITCH_UP);
	SPSHeliumValveBSwitch.Register(PSH, "SPSHeliumValveBSwitch", THREEPOSSWITCH_UP);
	SPSLineHTRSSwitch.Register(PSH, "SPSLineHTRSSwitch", THREEPOSSWITCH_CENTER);
	SPSPressIndSwitch.Register(PSH, "SPSPressIndSwitch", THREEPOSSWITCH_UP);
	
	CMUplinkSwitch.Register(PSH, "CMUplinkSwitch", 1);

	if (!SkylabCM)
		IUUplinkSwitch.Register(PSH, "IUUplinkSwitch", 1);

	CMRCSPressSwitch.Register(PSH, "CMRCSPressSwitch", 0, 0);
	SMRCSIndSwitch.Register(PSH, "SMRCSIndSwitch", 0);

	RCSQuantityGauge.Register(PSH, "RCSQuantityGauge", 0.0, 1.0, 0.5);

	SMRCSHeaterASwitch.Register(PSH, "SMRCSHeaterASwitch", THREEPOSSWITCH_CENTER);
	SMRCSHeaterBSwitch.Register(PSH, "SMRCSHeaterBSwitch", THREEPOSSWITCH_CENTER);
	SMRCSHeaterCSwitch.Register(PSH, "SMRCSHeaterCSwitch", THREEPOSSWITCH_CENTER);
	SMRCSHeaterDSwitch.Register(PSH, "SMRCSHeaterDSwitch", THREEPOSSWITCH_CENTER);

	RCSCMDSwitch.Register(PSH, "RCSCMDSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSTrnfrSwitch.Register(PSH, "RCSTrnfrSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	CMRCSIsolate1.Register(PSH, "CMRCSIsolate1", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	CMRCSIsolate2.Register(PSH, "CMRCSIsolate2", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	CMRCSIsolate1Talkback.Register(PSH, "CMRCSIsolate1Talkback", false);
	CMRCSIsolate2Talkback.Register(PSH, "CMRCSIsolate2Talkback", false);

	ManualAttRollSwitch.Register(PSH, "ManualAttRollSwitch", THREEPOSSWITCH_CENTER);
	ManualAttPitchSwitch.Register(PSH, "ManualAttPitchSwitch", THREEPOSSWITCH_UP);
	ManualAttYawSwitch.Register(PSH, "ManualAttYawSwitch", THREEPOSSWITCH_CENTER);

	LimitCycleSwitch.Register(PSH, "LimitCycleSwitch", 0);
	AttDeadbandSwitch.Register(PSH, "AttDeadbandSwitch", 0);
	AttRateSwitch.Register(PSH, "AttRateSwitch", TOGGLESWITCH_UP);
	TransContrSwitch.Register(PSH, "TransContrSwitch", 0);

	BMAGRollSwitch.Register(PSH, "BMAGRollSwitch", THREEPOSSWITCH_UP);
	BMAGPitchSwitch.Register(PSH, "BMAGPitchSwitch", THREEPOSSWITCH_UP);
	BMAGYawSwitch.Register(PSH, "BMAGYawSwitch", THREEPOSSWITCH_UP);

	EMSModeSwitch.Register(PSH, "EMSModeSwitch", THREEPOSSWITCH_CENTER);
	CMCAttSwitch.Register(PSH, "CMCAttSwitch", 1);

	FDAIScaleSwitch.Register(PSH, "FDAIScaleSwitch", THREEPOSSWITCH_UP);
	FDAISelectSwitch.Register(PSH, "FDAISelectSwitch", THREEPOSSWITCH_UP);
	FDAISourceSwitch.Register(PSH, "FDAISourceSwitch", THREEPOSSWITCH_UP);
	FDAIAttSetSwitch.Register(PSH, "FDAIAttSetSwitch", false);

	IMUGuardedCageSwitch.Register(PSH, "IMUGuardedCageSwitch", 0, 0);

	RCSIndicatorsSwitch.AddPosition(1, 280);
	RCSIndicatorsSwitch.AddPosition(2, 320);
	RCSIndicatorsSwitch.AddPosition(3, 340);
	RCSIndicatorsSwitch.AddPosition(4, 20);
	RCSIndicatorsSwitch.AddPosition(5, 40);
	RCSIndicatorsSwitch.AddPosition(6, 70);
	RCSIndicatorsSwitch.Register(PSH, "RCSIndicatorsSwitch", 1);

	LVGuidanceSwitch.Register(PSH, "LVGuidanceSwitch", TOGGLESWITCH_UP, false);
	LVGuidanceSwitch.SetGuardResetsState(false);

	if (!SkylabCM) {
		SIISIVBSepSwitch.Register(PSH, "SIISIVBSepSwitch", false, false);
		TLIEnableSwitch.Register(PSH, "TLIEnableSwitch", true);
	}

	ECSIndicatorsSwitch.AddPosition(1, 340);
	ECSIndicatorsSwitch.AddPosition(2, 20);
	ECSIndicatorsSwitch.Register(PSH, "ECSIndicatorsSwitch", 1);

	CGSwitch.Register(PSH, "CGSwitch", 0);
	ELSLogicSwitch.Register(PSH, "ELSLogicSwitch", 0, 0);
	ELSAutoSwitch.Register(PSH, "ELSAutoSwitch", 1);
	CMRCSLogicSwitch.Register(PSH, "CMRCSLogicSwitch", 0);
	CMPropDumpSwitch.Register(PSH, "CMPropDumpSwitch", 0, 0);
	CPPropPurgeSwitch.Register(PSH, "CPPropPurgeSwitch", 0, 0);

	// FCSMSPSASwitch.Register(PSH, "FCSMSPSASwitch", 0);
	// FCSMSPSBSwitch.Register(PSH, "FCSMSPSBSwitch", 0);
	EventTimerUpDownSwitch.Register(PSH, "EventTimerUpDownSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerControlSwitch.Register(PSH, "EventTimerControlSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerMinutesSwitch.Register(PSH, "EventTimerMinutesSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	EventTimerSecondsSwitch.Register(PSH, "EventTimerSecondsSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	MainReleaseSwitch.Register(PSH, "MainReleaseSwitch", 0, 0, SPRINGLOADEDSWITCH_DOWN);

	PropDumpAutoSwitch.Register(PSH, "PropDumpAutoSwitch", 1);
	TwoEngineOutAutoSwitch.Register(PSH, "TwoEngineOutAutoSwitch", 1);
	LVRateAutoSwitch.Register(PSH, "LVRateAutoSwitch", 1);
	TowerJett1Switch.Register(PSH, "TowerJett1Switch", THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	TowerJett1Switch.SetGuardResetsState(false);
	TowerJett2Switch.Register(PSH, "TowerJett2Switch", THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	TowerJett2Switch.SetGuardResetsState(false);

	CmSmSep1Switch.Register(PSH, "CmSmSep1Switch", 0, 0, SPRINGLOADEDSWITCH_DOWN);
	CmSmSep2Switch.Register(PSH, "CmSmSep2Switch", 0, 0, SPRINGLOADEDSWITCH_DOWN);

	EDSSwitch.Register(PSH, "EDSSwitch", 0, SPRINGLOADEDSWITCH_NONE, "EDS Auto switch");
	CsmLmFinalSep1Switch.Register(PSH, "CsmLmFinalSep1Switch",  THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_DOWN);
	CsmLmFinalSep2Switch.Register(PSH, "CsmLmFinalSep2Switch",  THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_DOWN);

	RotPowerNormal1Switch.Register(PSH, "RotPowerNormal1Switch", THREEPOSSWITCH_CENTER);
	RotPowerNormal2Switch.Register(PSH, "RotPowerNormal2Switch", THREEPOSSWITCH_CENTER);
	RotPowerDirect1Switch.Register(PSH, "RotPowerDirect1Switch", THREEPOSSWITCH_CENTER);
	RotPowerDirect2Switch.Register(PSH, "RotPowerDirect2Switch", THREEPOSSWITCH_CENTER);

	dVThrust1Switch.Register(PSH, "dVThrust1Switch", 0, 0);
	dVThrust2Switch.Register(PSH, "dVThrust2Switch", 0, 0);

	SPSswitch.Register(PSH, "SPSswitch", false);

	SPSGimbalPitchThumbwheel.Register(PSH, "SPSGimbalPitchThumbwheel", 8, 16);
	SPSGimbalYawThumbwheel.Register(PSH, "SPSGimbalYawThumbwheel", 8, 16, true);

	SPSOxidPercentMeter.Register(PSH, "SPSOxidPercentMeter", 0, 0.999, 1, 0.999);
	SPSFuelPercentMeter.Register(PSH, "SPSFuelPercentMeter", 0, 0.999, 1, 0.999);
	SPSOxidUnbalMeter.Register(PSH, "SPSOxidUnbalMeter", -600, 600, 10);

	SPSTempMeter.Register(PSH, "SPSTempMeter", 0, 200, 2);
	SPSHeliumNitrogenPressMeter.Register(PSH, "SPSHeliumNitrogenPressMeter", 0, 5000, 2);
	SPSFuelPressMeter.Register(PSH, "SPSFuelPressMeter", 0, 250, 2);
	SPSOxidPressMeter.Register(PSH, "SPSOxidPressMeter", 0, 250, 2);

	LVSPSPcMeter.Register(PSH, "LVSPSPcMeter", 0, 150, 2);

	GPFPIPitch1Meter.Register(PSH, "GPFPIPitch1Meter", 0, 92, 2);
	GPFPIPitch2Meter.Register(PSH, "GPFPIPitch2Meter", 0, 92, 2);
	GPFPIYaw1Meter.Register(PSH, "GPFPIYaw1Meter", 0, 92, 2);
	GPFPIYaw2Meter.Register(PSH, "GPFPIYaw2Meter", 0, 92, 2);

	DirectUllageButton.Register(PSH, "DirectUllageButton", false);
	ThrustOnButton.Register(PSH, "ThrustOnButton", false);

	H2Pressure1Meter.Register(PSH, "H2Pressure1Meter", 0, 400, 10);
	H2Pressure2Meter.Register(PSH, "H2Pressure2Meter", 0, 400, 10);
	O2Pressure1Meter.Register(PSH, "O2Pressure1Meter", 100, 1050, 10);
	O2Pressure2Meter.Register(PSH, "O2Pressure2Meter", 100, 1050, 10);
	H2Quantity1Meter.Register(PSH, "H2Quantity1Meter", 0, 1, 10);
	H2Quantity2Meter.Register(PSH, "H2Quantity2Meter", 0, 1, 10);
	O2Quantity1Meter.Register(PSH, "O2Quantity1Meter", 0, 1, 10);
	O2Quantity2Meter.Register(PSH, "O2Quantity2Meter", 0, 1, 10);

	CSMACVoltMeter.Register(PSH, "ACVoltMeter", 85, 145, 3);
	CSMDCVoltMeter.Register(PSH, "DCVoltMeter", 17.5, 47.5, 3);
	SystemTestVoltMeter.Register(PSH, "SystemTestMeter", 0.0, 5.0, 3);
	DCAmpMeter.Register(PSH, "DCAmpMeter", 0, 100, 3);

	FuelCellH2FlowMeter.Register(PSH, "FuelCellH2FlowMeter", 0, 0.2, 2);
	FuelCellO2FlowMeter.Register(PSH, "FuelCellO2FlowMeter", 0, 1.6, 2);
	FuelCellTempMeter.Register(PSH, "FuelCellTempMeter", 100, 550, 2);
	FuelCellCondenserTempMeter.Register(PSH, "FuelCellCondenserTempMeter", 150, 250, 2);

	SuitTempMeter.Register(PSH, "SuitTempMeter", 20, 95, 2);
	CabinTempMeter.Register(PSH, "CabinTempMeter", 40, 120, 2);
	SuitPressMeter.Register(PSH, "SuitPressMeter", 0, 16, 2);
	CabinPressMeter.Register(PSH, "CabinPressMeter", 0, 16, 2);
	PartPressCO2Meter.Register(PSH, "PartPressCO2Meter", 0, 30, 2);

	SuitComprDeltaPMeter.Register(PSH, "SuitComprDeltaPMeter", -0.1, 1.1, 5);
	LeftO2FlowMeter.Register(PSH, "LeftO2FlowMeter", 0.1, 1.1, 5);

	SuitCabinDeltaPMeter.Register(PSH, "SuitCabinDeltaPMeter", -6, 6, 5);
	RightO2FlowMeter.Register(PSH, "RightO2FlowMeter", 0.1, 1.1, 5);

	EcsRadTempInletMeter.Register(PSH, "EcsRadTempInletMeter", 50, 130, 5);
	EcsRadTempPrimOutletMeter.Register(PSH, "EcsRadTempPrimOutletMeter", -60, 110, 5, 50);

	EcsRadTempSecOutletMeter.Register(PSH, "EcsRadTempSecOutletMeter", 26, 74, 5);
	GlyEvapTempOutletMeter.Register(PSH, "GlyEvapTempOutletMeter", 26, 74, 5);

	GlyEvapSteamPressMeter.Register(PSH, "GlyEvapSteamPressMeter", 0.03, 0.27, 5);
	GlycolDischPressMeter.Register(PSH, "GlycolDischPressMeter", -6, 66, 5);

	AccumQuantityMeter.Register(PSH, "AccumQuantityMeter", 0, 1, 5);
	H2oQuantityMeter.Register(PSH, "H2oQuantityMeter", 0, 1, 3);

	EcsRadiatorIndicator.Register(PSH, "EcsRadiatorIndicator", true);

	EcsRadiatorsFlowContAutoSwitch.Register(PSH, "ECSRadiatorsFlowContAutoSwitch", THREEPOSSWITCH_UP);
	EcsRadiatorsFlowContPwrSwitch.Register(PSH, "ECSRadiatorsFlowContPwrSwitch", THREEPOSSWITCH_CENTER);
	EcsRadiatorsManSelSwitch.Register(PSH, "ECSRadiatorsManSelSwitch", THREEPOSSWITCH_UP);
	EcsRadiatorsHeaterPrimSwitch.Register(PSH, "ECSRadiatorsHeaterPrimSwitch", THREEPOSSWITCH_CENTER);
	EcsRadiatorsHeaterSecSwitch.Register(PSH, "ECSRadiatorsHeaterSecSwitch", false);

	PotH2oHtrSwitch.Register(PSH, "PotH2oHtrSwitch", THREEPOSSWITCH_CENTER);
	SuitCircuitH2oAccumAutoSwitch.Register(PSH, "SuitCircuitH2oAccumAutoSwitch", THREEPOSSWITCH_CENTER);
	SuitCircuitH2oAccumOnSwitch.Register(PSH, "SuitCircuitH2oAccumOnSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SuitCircuitHeatExchSwitch.Register(PSH, "SuitCircuitHeatExchSwitch", THREEPOSSWITCH_CENTER);
	SecCoolantLoopEvapSwitch.Register(PSH, "SecCoolantLoopEvapSwitch", THREEPOSSWITCH_CENTER);
	SecCoolantLoopPumpSwitch.Register(PSH, "SecCoolantLoopPumpSwitch", THREEPOSSWITCH_CENTER);
	H2oQtyIndSwitch.Register(PSH, "H2oQtyIndSwitch", false);
	GlycolEvapTempInSwitch.Register(PSH, "GlycolEvapTempInSwitch", false);
	GlycolEvapSteamPressAutoManSwitch.Register(PSH, "GlycolEvapSteamPressAutoManSwitch", false);
	GlycolEvapSteamPressIncrDecrSwitch.Register(PSH, "GlycolEvapSteamPressIncrDecrSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	GlycolEvapH2oFlowSwitch.Register(PSH, "GlycolEvapH2oFlowSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);
	CabinTempAutoManSwitch.Register(PSH, "CabinTempAutoManSwitch", false);

	CabinTempAutoControlSwitch.Register(PSH, "CabinTempAutoControlSwitch", 5, 9);

	SquelchAThumbwheel.Register(PSH, "SquelchAThumbwheel", 2, 9);
	SquelchBThumbwheel.Register(PSH, "SquelchBThumbwheel", 2, 9);
	
	SPSTestSwitch.Register(PSH, "SPSTestSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	EcsGlycolPumpsSwitch.AddPosition(0, 240);
	EcsGlycolPumpsSwitch.AddPosition(1, 270);
	EcsGlycolPumpsSwitch.AddPosition(2, 300);
	EcsGlycolPumpsSwitch.AddPosition(3, 330);
	EcsGlycolPumpsSwitch.AddPosition(4,   0);
	EcsGlycolPumpsSwitch.Register(PSH, "EcsGlycolPumpsSwitch", 2);

	RightIntegralRotarySwitch.AddPosition(0,  210);
	RightIntegralRotarySwitch.AddPosition(1,  240);
	RightIntegralRotarySwitch.AddPosition(2,  270);
	RightIntegralRotarySwitch.AddPosition(3,  300);
	RightIntegralRotarySwitch.AddPosition(4,  330);
	RightIntegralRotarySwitch.AddPosition(5,    0);
	RightIntegralRotarySwitch.AddPosition(6,   30);
	RightIntegralRotarySwitch.AddPosition(7,   60);
	RightIntegralRotarySwitch.AddPosition(8,   90);
	RightIntegralRotarySwitch.AddPosition(9,  120);
	RightIntegralRotarySwitch.AddPosition(10, 150);
	RightIntegralRotarySwitch.Register(PSH, "RightIntegralRotarySwitch", 4);

	RightFloodRotarySwitch.AddPosition(0,  210);
	RightFloodRotarySwitch.AddPosition(1,  240);
	RightFloodRotarySwitch.AddPosition(2,  270);
	RightFloodRotarySwitch.AddPosition(3,  300);
	RightFloodRotarySwitch.AddPosition(4,  330);
	RightFloodRotarySwitch.AddPosition(5,    0);
	RightFloodRotarySwitch.AddPosition(6,   30);
	RightFloodRotarySwitch.AddPosition(7,   60);
	RightFloodRotarySwitch.AddPosition(8,   90);
	RightFloodRotarySwitch.AddPosition(9,  120);
	RightFloodRotarySwitch.AddPosition(10, 150);
	RightFloodRotarySwitch.Register(PSH, "RightFloodRotarySwitch", 4);

	HighGainAntennaPitchPositionSwitch.AddPosition(0,   0);
	HighGainAntennaPitchPositionSwitch.AddPosition(1,  30);
	HighGainAntennaPitchPositionSwitch.AddPosition(2,  60);
	HighGainAntennaPitchPositionSwitch.AddPosition(3,  90);
	HighGainAntennaPitchPositionSwitch.AddPosition(4, 120);
	HighGainAntennaPitchPositionSwitch.AddPosition(5, 150);
	HighGainAntennaPitchPositionSwitch.AddPosition(6, 180);
	HighGainAntennaPitchPositionSwitch.Register(PSH, "HighGainAntennaPitchPositionSwitch", 0);

	HighGainAntennaYawPositionSwitch.AddPosition(0,    0);
	HighGainAntennaYawPositionSwitch.AddPosition(1,   30);
	HighGainAntennaYawPositionSwitch.AddPosition(2,   60);
	HighGainAntennaYawPositionSwitch.AddPosition(3,   90);
	HighGainAntennaYawPositionSwitch.AddPosition(4,  120);
	HighGainAntennaYawPositionSwitch.AddPosition(5,  150);
	HighGainAntennaYawPositionSwitch.AddPosition(6,  180);
	HighGainAntennaYawPositionSwitch.AddPosition(7,  210);
	HighGainAntennaYawPositionSwitch.AddPosition(8,  240);
	HighGainAntennaYawPositionSwitch.AddPosition(9,  270);
	HighGainAntennaYawPositionSwitch.AddPosition(10, 300);
	HighGainAntennaYawPositionSwitch.AddPosition(11, 330);
	HighGainAntennaYawPositionSwitch.Register(PSH, "HighGainAntennaYawPositionSwitch", 0);

	EMSFunctionSwitch.AddPosition(0,  180);
	EMSFunctionSwitch.AddPosition(1,  210);
	EMSFunctionSwitch.AddPosition(2,  240);
	EMSFunctionSwitch.AddPosition(3,  270);
	EMSFunctionSwitch.AddPosition(4,  300);
	EMSFunctionSwitch.AddPosition(5,  330);
	EMSFunctionSwitch.AddPosition(6,    0);
	EMSFunctionSwitch.AddPosition(7,   30);
	EMSFunctionSwitch.AddPosition(8,   60);
	EMSFunctionSwitch.AddPosition(9,   90);
	EMSFunctionSwitch.AddPosition(10, 120);
	EMSFunctionSwitch.AddPosition(11, 150);
	EMSFunctionSwitch.Register(PSH, "EMSFunctionSwitch", 0);

	EMSDvDisplay.Register(PSH, "EMSDvDisplay", -1000, 14000, 1, 0);

	VHFAntennaRotarySwitch.AddPosition(0, 300);
	VHFAntennaRotarySwitch.AddPosition(1,   0);
	VHFAntennaRotarySwitch.AddPosition(2,  60);
	VHFAntennaRotarySwitch.Register(PSH, "VHFAntennaRotarySwitch", 0);

	DCIndicatorsRotary.AddPosition(0, 210);
	DCIndicatorsRotary.AddPosition(1, 240);
	DCIndicatorsRotary.AddPosition(2, 270);
	DCIndicatorsRotary.AddPosition(3, 300);
	DCIndicatorsRotary.AddPosition(4, 330);
	DCIndicatorsRotary.AddPosition(5,   0);
	DCIndicatorsRotary.AddPosition(6,  30);
	DCIndicatorsRotary.AddPosition(7,  60);
	DCIndicatorsRotary.AddPosition(8,  90);
	DCIndicatorsRotary.AddPosition(9, 120);
	DCIndicatorsRotary.AddPosition(10,150);
	DCIndicatorsRotary.Register(PSH, "DCIndicatorsRotary", 6);

	DCIndicatorsRotary.SetSource(0, FuelCells[0]);
	DCIndicatorsRotary.SetSource(1, FuelCells[1]);
	DCIndicatorsRotary.SetSource(2, FuelCells[2]);
	DCIndicatorsRotary.SetSource(3, MainBusA);
	DCIndicatorsRotary.SetSource(4, MainBusB);
	DCIndicatorsRotary.SetSource(5, &BatteryBusA);
	DCIndicatorsRotary.SetSource(6, &BatteryBusB);
	DCIndicatorsRotary.SetSource(7, &BatteryCharger);	
	DCIndicatorsRotary.SetSource(8, EntryBatteryC);
	DCIndicatorsRotary.SetSource(9, PyroBatteryA);
	DCIndicatorsRotary.SetSource(10, PyroBatteryB);

	ACIndicatorRotary.AddPosition(0, 290);
	ACIndicatorRotary.AddPosition(1, 315);
	ACIndicatorRotary.AddPosition(2, 340);
	ACIndicatorRotary.AddPosition(3, 20);
	ACIndicatorRotary.AddPosition(4, 45);
	ACIndicatorRotary.AddPosition(5, 70);
	ACIndicatorRotary.Register(PSH, "ACIndicatorRotary", 3);

	ACIndicatorRotary.SetSource(0, &ACBus1PhaseA);
	ACIndicatorRotary.SetSource(1, &ACBus1PhaseB);
	ACIndicatorRotary.SetSource(2, &ACBus1PhaseC);
	ACIndicatorRotary.SetSource(3, &ACBus2PhaseA);
	ACIndicatorRotary.SetSource(4, &ACBus2PhaseB);
	ACIndicatorRotary.SetSource(5, &ACBus2PhaseC);

	BatteryChargeRotary.AddPosition(0, 300);
	BatteryChargeRotary.AddPosition(1, 330);
	BatteryChargeRotary.AddPosition(2,   0);
	BatteryChargeRotary.AddPosition(3,  30);
	BatteryChargeRotary.Register(PSH, "BatteryChargeRotary", 0);
	
	DockingProbeExtdRelSwitch.Register(PSH, "DockingProbeExtdRelSwitch", THREEPOSSWITCH_CENTER, false);
	DockingProbeExtdRelSwitch.SetGuardResetsState(false);
	DockingProbeRetractPrimSwitch.Register(PSH, "DockingProbeRetractPrimSwitch", THREEPOSSWITCH_CENTER);
	DockingProbeRetractSecSwitch.Register(PSH, "DockingProbeRetractSecSwitch", THREEPOSSWITCH_CENTER);
	DockingProbeAIndicator.Register(PSH, "DockingProbeAIndicator", false);
	DockingProbeBIndicator.Register(PSH, "DockingProbeBIndicator", false);

	NumericRotarySwitch.AddPosition(0,  210);
	NumericRotarySwitch.AddPosition(1,  240);
	NumericRotarySwitch.AddPosition(2,  270);
	NumericRotarySwitch.AddPosition(3,  300);
	NumericRotarySwitch.AddPosition(4,  330);
	NumericRotarySwitch.AddPosition(5,    0);
	NumericRotarySwitch.AddPosition(6,   30);
	NumericRotarySwitch.AddPosition(7,   60);
	NumericRotarySwitch.AddPosition(8,   90);
	NumericRotarySwitch.AddPosition(9,  120);
	NumericRotarySwitch.AddPosition(10, 150);
	NumericRotarySwitch.Register(PSH, "NumericRotarySwitch", 0);

	FloodRotarySwitch.AddPosition(0,  210);
	FloodRotarySwitch.AddPosition(1,  240);
	FloodRotarySwitch.AddPosition(2,  270);
	FloodRotarySwitch.AddPosition(3,  300);
	FloodRotarySwitch.AddPosition(4,  330);
	FloodRotarySwitch.AddPosition(5,    0);
	FloodRotarySwitch.AddPosition(6,   30);
	FloodRotarySwitch.AddPosition(7,   60);
	FloodRotarySwitch.AddPosition(8,   90);
	FloodRotarySwitch.AddPosition(9,  120);
	FloodRotarySwitch.AddPosition(10, 150);
	FloodRotarySwitch.Register(PSH, "FloodRotarySwitch", 4);

	IntegralRotarySwitch.AddPosition(0,  210);
	IntegralRotarySwitch.AddPosition(1,  240);
	IntegralRotarySwitch.AddPosition(2,  270);
	IntegralRotarySwitch.AddPosition(3,  300);
	IntegralRotarySwitch.AddPosition(4,  330);
	IntegralRotarySwitch.AddPosition(5,    0);
	IntegralRotarySwitch.AddPosition(6,   30);
	IntegralRotarySwitch.AddPosition(7,   60);
	IntegralRotarySwitch.AddPosition(8,   90);
	IntegralRotarySwitch.AddPosition(9,  120);
	IntegralRotarySwitch.AddPosition(10, 150);
	IntegralRotarySwitch.Register(PSH, "IntegralRotarySwitch", 4);

	FDAIPowerRotarySwitch.AddPosition(0,  0);
	FDAIPowerRotarySwitch.AddPosition(1, 30);
	FDAIPowerRotarySwitch.AddPosition(2, 60);
	FDAIPowerRotarySwitch.AddPosition(3, 90);
	FDAIPowerRotarySwitch.Register(PSH, "FDAIPowerRotarySwitch", 0);

	SCSElectronicsPowerRotarySwitch.AddPosition(0, 30);
	SCSElectronicsPowerRotarySwitch.AddPosition(1, 60);
	SCSElectronicsPowerRotarySwitch.AddPosition(2, 90);
	SCSElectronicsPowerRotarySwitch.Register(PSH, "SCSElectronicsPowerRotarySwitch", 0);

	BMAGPowerRotary1Switch.AddPosition(0, 30);
	BMAGPowerRotary1Switch.AddPosition(1, 60);
	BMAGPowerRotary1Switch.AddPosition(2, 90);
	BMAGPowerRotary1Switch.Register(PSH, "BMAGPowerRotary1Switch", 1);

	BMAGPowerRotary2Switch.AddPosition(0, 30);
	BMAGPowerRotary2Switch.AddPosition(1, 60);
	BMAGPowerRotary2Switch.AddPosition(2, 90);
	BMAGPowerRotary2Switch.Register(PSH, "BMAGPowerRotary2Switch", 1);

	DirectO2RotarySwitch.AddPosition(0, 330);
	DirectO2RotarySwitch.AddPosition(1,   0);
	DirectO2RotarySwitch.AddPosition(2,  30);
	DirectO2RotarySwitch.AddPosition(3,  60);
	DirectO2RotarySwitch.AddPosition(4,  90);
	DirectO2RotarySwitch.AddPosition(5, 120);
	DirectO2RotarySwitch.AddPosition(6, 150);
	DirectO2RotarySwitch.Register(PSH, "DirectO2RotarySwitch", 6);

	GlycolReservoirInletRotary.AddPosition(0,  90);
	GlycolReservoirInletRotary.AddPosition(1, 180);
	GlycolReservoirInletRotary.Register(PSH, "GlycolReservoirInletRotary", 0);

	GlycolReservoirBypassRotary.AddPosition(0,  90);
	GlycolReservoirBypassRotary.AddPosition(1, 180);
	GlycolReservoirBypassRotary.Register(PSH, "GlycolReservoirBypassRotary", 1);
	
	GlycolReservoirOutletRotary.AddPosition(0,  90);
	GlycolReservoirOutletRotary.AddPosition(1, 180);
	GlycolReservoirOutletRotary.Register(PSH, "GlycolReservoirOutletRotary", 0);

	OxygenSurgeTankRotary.AddPosition(0,  0);
	OxygenSurgeTankRotary.AddPosition(1, 90);
	OxygenSurgeTankRotary.Register(PSH, "OxygenSurgeTankRotary", 1);

	OxygenSMSupplyRotary.AddPosition(0,  0);
	OxygenSMSupplyRotary.AddPosition(1, 90);
	OxygenSMSupplyRotary.Register(PSH, "OxygenSMSupplyRotary", 1);
	
	OxygenRepressPackageRotary.AddPosition(0, 330);
	OxygenRepressPackageRotary.AddPosition(1,  60);
	OxygenRepressPackageRotary.AddPosition(2, 120);
	OxygenRepressPackageRotary.Register(PSH, "OxygenRepressPackageRotary", 2);

	ORDEALAltSetRotary.AddPosition(0, 240);
	ORDEALAltSetRotary.AddPosition(1, 270);
	ORDEALAltSetRotary.AddPosition(2, 330);
	ORDEALAltSetRotary.AddPosition(3,   0);
	ORDEALAltSetRotary.AddPosition(4,  60);
	ORDEALAltSetRotary.AddPosition(5,  90);
	ORDEALAltSetRotary.AddPosition(6, 150);
	ORDEALAltSetRotary.Register(PSH, "ORDEALAltSetRotary", 3);

	LeftSystemTestRotarySwitch.AddPosition(0,  240);
	LeftSystemTestRotarySwitch.AddPosition(1,  270);
	LeftSystemTestRotarySwitch.AddPosition(2,  300);
	LeftSystemTestRotarySwitch.AddPosition(3,  330);
	LeftSystemTestRotarySwitch.AddPosition(4,    0);
	LeftSystemTestRotarySwitch.AddPosition(5,   30);
	LeftSystemTestRotarySwitch.AddPosition(6,   60);
	LeftSystemTestRotarySwitch.AddPosition(7,   90);
	LeftSystemTestRotarySwitch.AddPosition(8,  120);
	LeftSystemTestRotarySwitch.Register(PSH, "LeftSystemTestRotarySwitch", 0);
	
	RightSystemTestRotarySwitch.AddPosition(0,  315);
	RightSystemTestRotarySwitch.AddPosition(1,  340);
	RightSystemTestRotarySwitch.AddPosition(2,   20);
	RightSystemTestRotarySwitch.AddPosition(3,   45);
	RightSystemTestRotarySwitch.Register(PSH, "RightSystemTestRotarySwitch", 0);
	
	Panel100NumericRotarySwitch.AddPosition(0,  210);
	Panel100NumericRotarySwitch.AddPosition(1,  240);
	Panel100NumericRotarySwitch.AddPosition(2,  270);
	Panel100NumericRotarySwitch.AddPosition(3,  300);
	Panel100NumericRotarySwitch.AddPosition(4,  330);
	Panel100NumericRotarySwitch.AddPosition(5,    0);
	Panel100NumericRotarySwitch.AddPosition(6,   30);
	Panel100NumericRotarySwitch.AddPosition(7,   60);
	Panel100NumericRotarySwitch.AddPosition(8,   90);
	Panel100NumericRotarySwitch.AddPosition(9,  120);
	Panel100NumericRotarySwitch.AddPosition(10, 150);
	Panel100NumericRotarySwitch.Register(PSH, "Panel100NumericRotarySwitch", 0);

	Panel100FloodRotarySwitch.AddPosition(0,  210);
	Panel100FloodRotarySwitch.AddPosition(1,  240);
	Panel100FloodRotarySwitch.AddPosition(2,  270);
	Panel100FloodRotarySwitch.AddPosition(3,  300);
	Panel100FloodRotarySwitch.AddPosition(4,  330);
	Panel100FloodRotarySwitch.AddPosition(5,    0);
	Panel100FloodRotarySwitch.AddPosition(6,   30);
	Panel100FloodRotarySwitch.AddPosition(7,   60);
	Panel100FloodRotarySwitch.AddPosition(8,   90);
	Panel100FloodRotarySwitch.AddPosition(9,  120);
	Panel100FloodRotarySwitch.AddPosition(10, 150);
	Panel100FloodRotarySwitch.Register(PSH, "Panel100FloodRotarySwitch", 0);

	Panel100IntegralRotarySwitch.AddPosition(0,  210);
	Panel100IntegralRotarySwitch.AddPosition(1,  240);
	Panel100IntegralRotarySwitch.AddPosition(2,  270);
	Panel100IntegralRotarySwitch.AddPosition(3,  300);
	Panel100IntegralRotarySwitch.AddPosition(4,  330);
	Panel100IntegralRotarySwitch.AddPosition(5,    0);
	Panel100IntegralRotarySwitch.AddPosition(6,   30);
	Panel100IntegralRotarySwitch.AddPosition(7,   60);
	Panel100IntegralRotarySwitch.AddPosition(8,   90);
	Panel100IntegralRotarySwitch.AddPosition(9,  120);
	Panel100IntegralRotarySwitch.AddPosition(10, 150);
	Panel100IntegralRotarySwitch.Register(PSH, "Panel100IntegralRotarySwitch", 0);

	OxygenSurgeTankValveRotary.AddPosition(0,  330);
	OxygenSurgeTankValveRotary.AddPosition(1,    0);
	OxygenSurgeTankValveRotary.AddPosition(2,   30);
	OxygenSurgeTankValveRotary.AddPosition(3,   60);
	OxygenSurgeTankValveRotary.AddPosition(4,   90);
	OxygenSurgeTankValveRotary.AddPosition(5,  120);
	OxygenSurgeTankValveRotary.AddPosition(6,  150);
	OxygenSurgeTankValveRotary.AddPosition(7,  180);
	OxygenSurgeTankValveRotary.AddPosition(8,  210);
	OxygenSurgeTankValveRotary.Register(PSH, "OxygenSurgeTankValveRotary", 0);

	GlycolToRadiatorsRotary.AddPosition(0,  0);
	GlycolToRadiatorsRotary.AddPosition(1, 90);
	GlycolToRadiatorsRotary.Register(PSH, "GlycolToRadiatorsRotary", 1);

	GlycolRotary.AddPosition(0, 90);
	GlycolRotary.AddPosition(1,180);
	GlycolRotary.Register(PSH, "GlycolRotary", 0);
	
	AccumRotary.AddPosition(0, 90);
	AccumRotary.AddPosition(1,180);
	AccumRotary.Register(PSH, "AccumRotary", 0);

	PressureReliefRotary.AddPosition(0,   0);
	PressureReliefRotary.AddPosition(1,  90);
	PressureReliefRotary.AddPosition(2, 180);
	PressureReliefRotary.AddPosition(3, 270);
	PressureReliefRotary.Register(PSH, "PressureReliefRotary", 2);

	WasteTankInletRotary.AddPosition(0,  0);
	WasteTankInletRotary.AddPosition(1, 90);
	WasteTankInletRotary.Register(PSH, "WasteTankInletRotary", 0);

	PotableTankInletRotary.AddPosition(0,  0);
	PotableTankInletRotary.AddPosition(1, 90);
	PotableTankInletRotary.Register(PSH, "PotableTankInletRotary", 0);

	WasteTankServicingRotary.AddPosition(0,  0);
	WasteTankServicingRotary.AddPosition(1, 90);
	WasteTankServicingRotary.Register(PSH, "WasteTankServicingRotary", 0);

	CabinRepressValveRotary.AddPosition(0,  90);
	CabinRepressValveRotary.AddPosition(1, 120);
	CabinRepressValveRotary.AddPosition(2, 150);
	CabinRepressValveRotary.AddPosition(3, 180);
	CabinRepressValveRotary.AddPosition(4, 210);
	CabinRepressValveRotary.AddPosition(5, 240);
	CabinRepressValveRotary.AddPosition(6, 270);
	CabinRepressValveRotary.Register(PSH, "CabinRepressValveRotary", 0);

	SelectorInletValveRotary.AddPosition(0,   0);
	SelectorInletValveRotary.AddPosition(1,  90);
	SelectorInletValveRotary.AddPosition(2, 180);
	SelectorInletValveRotary.AddPosition(3, 270);
	SelectorInletValveRotary.Register(PSH, "SelectorInletValveRotary", 1);

	SelectorOutletValveRotary.AddPosition(0,   0);
	SelectorOutletValveRotary.AddPosition(1,  90);
	SelectorOutletValveRotary.AddPosition(2, 180);
	SelectorOutletValveRotary.AddPosition(3, 270);
	SelectorOutletValveRotary.Register(PSH, "SelectorOutletValveRotary", 1);

	EmergencyCabinPressureRotary.AddPosition(0,   0);
	EmergencyCabinPressureRotary.AddPosition(1,  90);
	EmergencyCabinPressureRotary.AddPosition(2, 180);
	EmergencyCabinPressureRotary.AddPosition(3, 270);
	EmergencyCabinPressureRotary.Register(PSH, "EmergencyCabinPressureRotary", 1);
		
	OrbiterAttitudeToggle.SetActive(false);		// saved in LPSwitchState.LPswitch5

	EpsSensorSignalDcMnaCircuitBraker.Register(PSH, "EpsSensorSignalDcMnaCircuitBraker", 1);
	EpsSensorSignalDcMnbCircuitBraker.Register(PSH, "EpsSensorSignalDcMnbCircuitBraker", 1);

	EpsSensorSignalAc1CircuitBraker.Register(PSH, "EpsSensorSignalAc1CircuitBraker", 1);
	EpsSensorSignalAc2CircuitBraker.Register(PSH, "EpsSensorSignalAc2CircuitBraker", 1);

	CWMnaCircuitBraker.Register(PSH, "CWMnaCircuitBraker", 1);
	CWMnbCircuitBraker.Register(PSH, "CWMnbCircuitBraker", 1);

	MnbLMPWR1CircuitBraker.Register(PSH, "MnbLMPWR1CircuitBraker", 1);
	MnbLMPWR2CircuitBraker.Register(PSH, "MnbLMPWR2CircuitBraker", 1);

	InverterControl1CircuitBraker.Register(PSH, "InverterControl1CircuitBraker", 1);
	InverterControl2CircuitBraker.Register(PSH, "InverterControl2CircuitBraker", 1);
	InverterControl3CircuitBraker.Register(PSH, "InverterControl3CircuitBraker", 1);

	EPSSensorUnitDcBusACircuitBraker.Register(PSH, "EPSSensorUnitDcBusACircuitBraker", 1);
	EPSSensorUnitDcBusBCircuitBraker.Register(PSH, "EPSSensorUnitDcBusBCircuitBraker", 1);
	EPSSensorUnitAcBus1CircuitBraker.Register(PSH, "EPSSensorUnitAcBus1CircuitBraker", 1);
	EPSSensorUnitAcBus2CircuitBraker.Register(PSH, "EPSSensorUnitAcBus2CircuitBraker", 1);

	BATRLYBusBatACircuitBraker.Register(PSH, "BATRLYBusBatACircuitBraker", 1);
	BATRLYBusBatBCircuitBraker.Register(PSH, "BATRLYBusBatBCircuitBraker", 1);

	ControllerAc1CircuitBraker.Register(PSH, "ControllerAc1CircuitBraker", 1);
	ControllerAc2CircuitBraker.Register(PSH, "ControllerAc2CircuitBraker", 1);
	CONTHTRSMnACircuitBraker.Register(PSH, "CONTHTRSMnACircuitBraker", 1);
	CONTHTRSMnBCircuitBraker.Register(PSH, "CONTHTRSMnBCircuitBraker", 1);
	HTRSOVLDBatACircuitBraker.Register(PSH, "HTRSOVLDBatACircuitBraker", 1);
	HTRSOVLDBatBCircuitBraker.Register(PSH, "HTRSOVLDBatBCircuitBraker", 1);

	BatteryChargerBatACircuitBraker.Register(PSH, "BatteryChargerBatACircuitBraker", 1);
	BatteryChargerBatBCircuitBraker.Register(PSH, "BatteryChargerBatBCircuitBraker", 1);
	BatteryChargerMnACircuitBraker.Register(PSH, "BatteryChargerMnACircuitBraker", 1);
	BatteryChargerMnBCircuitBraker.Register(PSH, "BatteryChargerMnBCircuitBraker", 1);
	BatteryChargerAcPwrCircuitBraker.Register(PSH, "BatteryChargerAcPWRCircuitBraker", 1);

	InstrumentLightingESSMnACircuitBraker.Register(PSH, "InstrumentLightingESSMnACircuitBraker", 1);
	InstrumentLightingESSMnBCircuitBraker.Register(PSH, "InstrumentLightingESSMnBCircuitBraker", 1);
	InstrumentLightingNonESSCircuitBraker.Register(PSH, "InstrumentLightingNonESSCircuitBraker", 1);
	InstrumentLightingSCIEquipSEP1CircuitBraker.Register(PSH, "InstrumentLightingSCIEquipSEP1CircuitBraker", 1);
	InstrumentLightingSCIEquipSEP2CircuitBraker.Register(PSH, "InstrumentLightingSCIEquipSEP2CircuitBraker", 1);
	InstrumentLightingSCIEquipHatchCircuitBraker.Register(PSH, "InstrumentLightingSCIEquipHatchCircuitBraker", 1);

	ECSPOTH2OHTRMnACircuitBraker.Register(PSH, "ECSPOTH2OHTRMnACircuitBraker", 1);
	ECSPOTH2OHTRMnBCircuitBraker.Register(PSH, "ECSPOTH2OHTRMnBCircuitBraker", 1);
	ECSH2OAccumMnACircuitBraker.Register(PSH, "ECSH2OAccumMnACircuitBraker", 1);
	ECSH2OAccumMnBCircuitBraker.Register(PSH, "ECSH2OAccumMnBCircuitBraker", 1);
	ECSTransducerWastePOTH2OMnACircuitBraker.Register(PSH, "ECSTransducerWastePOTH2OMnACircuitBraker", 1);
	ECSTransducerWastePOTH2OMnBCircuitBraker.Register(PSH, "ECSTransducerWastePOTH2OMnBCircuitBraker", 1);
	ECSTransducerPressGroup1MnACircuitBraker.Register(PSH, "ECSTransducerPressGroup1MnACircuitBraker", 1);
	ECSTransducerPressGroup1MnBCircuitBraker.Register(PSH, "ECSTransducerPressGroup1MnBCircuitBraker", 1);
	ECSTransducerPressGroup2MnACircuitBraker.Register(PSH, "ECSTransducerPressGroup2MnACircuitBraker", 1);
	ECSTransducerPressGroup2MnBCircuitBraker.Register(PSH, "ECSTransducerPressGroup2MnBCircuitBraker", 1);
	ECSTransducerTempMnACircuitBraker.Register(PSH, "ECSTransducerTempMnACircuitBraker", 1);
	ECSTransducerTempMnBCircuitBraker.Register(PSH, "ECSTransducerTempMnBCircuitBraker", 1);

	ECSSecCoolLoopAc1CircuitBraker.Register(PSH, "ECSSecCoolLoopAc1CircuitBraker", 1);
	ECSSecCoolLoopAc2CircuitBraker.Register(PSH, "ECSSecCoolLoopAc2CircuitBraker", 1);
	ECSSecCoolLoopRADHTRMnACircuitBraker.Register(PSH, "ECSSecCoolLoopRADHTRMnACircuitBraker", 1);
	ECSSecCoolLoopXducersMnACircuitBraker.Register(PSH, "ECSSecCoolLoopXducersMnACircuitBraker", 1);
	ECSSecCoolLoopXducersMnBCircuitBraker.Register(PSH, "ECSSecCoolLoopXducersMnBCircuitBraker", 1);
	ECSWasteH2OUrineDumpHTRMnACircuitBraker.Register(PSH, "ECSWasteH2OUrineDumpHTRMnACircuitBraker", 1);
	ECSWasteH2OUrineDumpHTRMnBCircuitBraker.Register(PSH, "ECSWasteH2OUrineDumpHTRMnBCircuitBraker", 1);
	ECSCabinFanAC1ACircuitBraker.Register(PSH, "ECSCabinFanAC1ACircuitBraker", 1);
	ECSCabinFanAC1BCircuitBraker.Register(PSH, "ECSCabinFanAC1BCircuitBraker", 1);
	ECSCabinFanAC1CCircuitBraker.Register(PSH, "ECSCabinFanAC1CCircuitBraker", 1);
	ECSCabinFanAC2ACircuitBraker.Register(PSH, "ECSCabinFanAC2ACircuitBraker", 1);
	ECSCabinFanAC2BCircuitBraker.Register(PSH, "ECSCabinFanAC2BCircuitBraker", 1);
	ECSCabinFanAC2CCircuitBraker.Register(PSH, "ECSCabinFanAC2CCircuitBraker", 1);

	GNPowerAc1CircuitBraker.Register(PSH, "GNPowerAc1CircuitBraker", 1);
	GNPowerAc2CircuitBraker.Register(PSH, "GNPowerAc2CircuitBraker", 1);
	GNIMUMnACircuitBraker.Register(PSH, "GNIMUMnACircuitBraker", 0);
	GNIMUMnBCircuitBraker.Register(PSH, "GNIMUMnBCircuitBraker", 0);
	GNIMUHTRMnACircuitBraker.Register(PSH, "GNIMUHTRMnACircuitBraker", 0);
	GNIMUHTRMnBCircuitBraker.Register(PSH, "GNIMUHTRMnBCircuitBraker", 0);
	GNComputerMnACircuitBraker.Register(PSH, "GNComputerMnACircuitBraker", 0);
	GNComputerMnBCircuitBraker.Register(PSH, "GNComputerMnBCircuitBraker", 0);
	GNOpticsMnACircuitBraker.Register(PSH, "GNOpticsMnACircuitBraker", 1);
	GNOpticsMnBCircuitBraker.Register(PSH, "GNOpticsMnBCircuitBraker", 1);

	SuitCompressorsAc1ACircuitBraker.Register(PSH, "SuitCompressorsAc1ACircuitBraker", 1);
	SuitCompressorsAc1BCircuitBraker.Register(PSH, "SuitCompressorsAc1BCircuitBraker", 1);
	SuitCompressorsAc1CCircuitBraker.Register(PSH, "SuitCompressorsAc1CCircuitBraker", 1);
	SuitCompressorsAc2ACircuitBraker.Register(PSH, "SuitCompressorsAc2ACircuitBraker", 1);
	SuitCompressorsAc2BCircuitBraker.Register(PSH, "SuitCompressorsAc2BCircuitBraker", 1);
	SuitCompressorsAc2CCircuitBraker.Register(PSH, "SuitCompressorsAc2CCircuitBraker", 1);

	ECSGlycolPumpsAc1ACircuitBraker.Register(PSH, "ECSGlycolPumpsAc1ACircuitBraker", 1);
	ECSGlycolPumpsAc1BCircuitBraker.Register(PSH, "ECSGlycolPumpsAc1BCircuitBraker", 1);
	ECSGlycolPumpsAc1CCircuitBraker.Register(PSH, "ECSGlycolPumpsAc1CCircuitBraker", 1);
	ECSGlycolPumpsAc2ACircuitBraker.Register(PSH, "ECSGlycolPumpsAc2ACircuitBraker", 1);
	ECSGlycolPumpsAc2BCircuitBraker.Register(PSH, "ECSGlycolPumpsAc2BCircuitBraker", 1);
	ECSGlycolPumpsAc2CCircuitBraker.Register(PSH, "ECSGlycolPumpsAc2CCircuitBraker", 1);

	StabContSystemTVCAc1CircuitBraker.Register(PSH, "StabContSystemTVCAc1CircuitBraker", 1);
	StabContSystemAc1CircuitBraker.Register(PSH, "StabContSystemAc1CircuitBraker", 1);
	StabContSystemAc2CircuitBraker.Register(PSH, "StabContSystemAc2CircuitBraker", 1);

	ECATVCAc2CircuitBraker.Register(PSH, "StabContSystemTVCAc1CircuitBraker", 1);
	DirectUllMnACircuitBraker.Register(PSH, "DirectUllMnACircuitBraker", 1);
	DirectUllMnBCircuitBraker.Register(PSH, "DirectUllMnBCircuitBraker", 1);
	ContrDirectMnA1CircuitBraker.Register(PSH, "ContrDirectMnA1CircuitBraker", 1);
	ContrDirectMnB1CircuitBraker.Register(PSH, "ContrDirectMnB1CircuitBraker", 1);
	ContrDirectMnA2CircuitBraker.Register(PSH, "ContrDirectMnA2CircuitBraker", 1);
	ContrDirectMnB2CircuitBraker.Register(PSH, "ContrDirectMnB2CircuitBraker", 1);
	ACRollMnACircuitBraker.Register(PSH, "ACRollMnACircuitBraker", 1);
	ACRollMnBCircuitBraker.Register(PSH, "ACRollMnBCircuitBraker", 1);
	BDRollMnACircuitBraker.Register(PSH, "BDRollMnACircuitBraker", 1);
	BDRollMnBCircuitBraker.Register(PSH, "BDRollMnBCircuitBraker", 1);
	PitchMnACircuitBraker.Register(PSH, "PitchMnACircuitBraker", 1);
	PitchMnBCircuitBraker.Register(PSH, "PitchMnBCircuitBraker", 1);
	YawMnACircuitBraker.Register(PSH, "YawMnACircuitBraker", 1);
	YawMnBCircuitBraker.Register(PSH, "YawMnBCircuitBraker", 1);

	OrdealAc2CircuitBraker.Register(PSH, "OrdealAc2CircuitBraker", 1);
	OrdealMnBCircuitBraker.Register(PSH, "OrdealMnBCircuitBraker", 1);
	ContrAutoMnACircuitBraker.Register(PSH, "ContrAutoMnACircuitBraker", 1);
	ContrAutoMnBCircuitBraker.Register(PSH, "ContrAutoMnBCircuitBraker", 1);
	LogicBus12MnACircuitBraker.Register(PSH, "LogicBus12MnACircuitBraker", 1);
	LogicBus34MnACircuitBraker.Register(PSH, "LogicBus34MnACircuitBraker", 1);
	LogicBus14MnBCircuitBraker.Register(PSH, "LogicBus14MnBCircuitBraker", 1);
	LogicBus23MnBCircuitBraker.Register(PSH, "LogicBus23MnBCircuitBraker", 1);
	SystemMnACircuitBraker.Register(PSH, "SystemMnACircuitBraker", 1);
	SystemMnBCircuitBraker.Register(PSH, "SystemMnBCircuitBraker", 1);

	CMHeater1MnACircuitBraker.Register(PSH, "CMHeater1MnACircuitBraker", 0);
	CMHeater2MnBCircuitBraker.Register(PSH, "CMHeater2MnBCircuitBraker", 0);
	SMHeatersAMnBCircuitBraker.Register(PSH, "SMHeatersAMnBCircuitBraker", 1);
	SMHeatersCMnBCircuitBraker.Register(PSH, "SMHeatersCMnBCircuitBraker", 1);
	SMHeatersBMnACircuitBraker.Register(PSH, "SMHeatersBMnACircuitBraker", 1);
	SMHeatersDMnACircuitBraker.Register(PSH, "SMHeatersDMnACircuitBraker", 1);
	PrplntIsolMnACircuitBraker.Register(PSH, "PrplntIsolMnACircuitBraker", 1);
	PrplntIsolMnBCircuitBraker.Register(PSH, "PrplntIsolMnBCircuitBraker", 1);
	RCSLogicMnACircuitBraker.Register(PSH, "RCSLogicMnACircuitBraker", 1);
	RCSLogicMnBCircuitBraker.Register(PSH, "RCSLogicMnBCircuitBraker", 1);
	EMSMnACircuitBraker.Register(PSH, "EMSMnACircuitBraker", 1);
	EMSMnBCircuitBraker.Register(PSH, "EMSMnBCircuitBraker", 1);
	DockProbeMnACircuitBraker.Register(PSH, "DockProbeMnACircuitBraker", 1);
	DockProbeMnBCircuitBraker.Register(PSH, "DockProbeMnBCircuitBraker", 1);

	GaugingMnACircuitBraker.Register(PSH, "GaugingMnACircuitBraker", 1);
	GaugingMnBCircuitBraker.Register(PSH, "GaugingMnBCircuitBraker", 1);
	GaugingAc1CircuitBraker.Register(PSH, "GaugingAc1CircuitBraker", 1);
	GaugingAc2CircuitBraker.Register(PSH, "GaugingAc2CircuitBraker", 1);
	HeValveMnACircuitBraker.Register(PSH, "HeValveMnACircuitBraker", 1);
	HeValveMnBCircuitBraker.Register(PSH, "HeValveMnBCircuitBraker", 1);
	PitchBatACircuitBraker.Register(PSH, "PitchBatACircuitBraker", 1);
	PitchBatBCircuitBraker.Register(PSH, "PitchBatBCircuitBraker", 1);
	YawBatACircuitBraker.Register(PSH, "YawBatACircuitBraker", 1);
	YawBatBCircuitBraker.Register(PSH, "YawBatBCircuitBraker", 1);
	PilotValveMnACircuitBraker.Register(PSH, "PilotValveMnACircuit", 1);
	PilotValveMnBCircuitBraker.Register(PSH, "PilotValveMnBCircuit", 1);

	FloatBag1BatACircuitBraker.Register(PSH, "FloatBag1BatACircuitBraker", 0);
	FloatBag2BatBCircuitBraker.Register(PSH, "FloatBag2BatBCircuitBraker", 0);
	FloatBag3FLTPLCircuitBraker.Register(PSH, "FloatBag3FLTPLCircuitBraker", 0);

	LogicBatACircuitBraker.Register(PSH, "LogicBatACircuitBraker", 0);
	LogicBatBCircuitBraker.Register(PSH, "LogicBatBCircuitBraker", 0);
	ArmBatACircuitBraker.Register(PSH, "ArmBatACircuitBraker", 0);
	ArmBatBCircuitBraker.Register(PSH, "ArmBatBCircuitBraker", 0);

	EDS1BatACircuitBraker.Register(PSH, "EDS1BatACircuitBraker", 1);
	EDS2BatBCircuitBraker.Register(PSH, "EDS2BatBCircuitBraker", 1);
	EDS3BatCCircuitBraker.Register(PSH, "EDS3BatCCircuitBraker", 1);

	ELSBatACircuitBraker.Register(PSH, "ELSBatACircuitBraker", 1);
	ELSBatBCircuitBraker.Register(PSH, "ELSBatBCircuitBraker", 1);

	FLTPLCircuitBraker.Register(PSH, "FLTPLCircuitBraker", 1);

	ModeIntercomVOXSensThumbwheelSwitch.Register(PSH, "ModeIntercomVOXSensThumbwheelSwitch", 2, 9);

	PowerMasterVolumeThumbwheelSwitch.Register(PSH, "PowerMasterVolumeThumbwheelSwitch", 9, 9);

	PadCommVolumeThumbwheelSwitch.Register(PSH, "PadCommVolumeThumbwheelSwitch", 2, 9);

	IntercomVolumeThumbwheelSwitch.Register(PSH, "IntercomVolumeThumbwheelSwitch", 2, 9);

	SBandVolumeThumbwheelSwitch.Register(PSH, "SBandVolumeThumbwheelSwitch", 2, 9);

	VHFAMVolumeThumbwheelSwitch.Register(PSH, "VHFAMVolumeThumbwheelSwitch", 2, 9);

	ModeSwitch.Register(PSH, "ModeSwitch", THREEPOSSWITCH_CENTER);
	ControllerSpeedSwitch.Register(PSH, "ControllerSpeedSwitch", THREEPOSSWITCH_UP);
	ControllerCouplingSwitch.Register(PSH, "ControllerCouplingSwitch", false);
	ControllerTrackerSwitch.Register(PSH, "ControllerTrackerSwitch", THREEPOSSWITCH_DOWN);
	ControllerTelescopeTrunnionSwitch.Register(PSH, "ControllerTelescopeTrunnionSwitch", THREEPOSSWITCH_UP);
	ConditionLampsSwitch.Register(PSH, "ConditionLampsSwitch", THREEPOSSWITCH_UP);
	UPTLMSwitch.Register(PSH, "UPTLMSwitch", false);
	OpticsHandcontrollerSwitch.Register(PSH, "OpticsHandcontrollerSwitch");
	OpticsMarkButton.Register(PSH, "OpticsMarkButton", false);
	OpticsMarkRejectButton.Register(PSH, "OpticsMarkRejectButton", false);
	MinImpulseHandcontrollerSwitch.Register(PSH, "OpticsMarkRejectButton", true);

	GlycolToRadiatorsLever.Register(PSH, "GlycolToRadiatorsLever", 1);
	CabinPressureReliefLever1.Register(PSH, "CabinPressureReliefLever1", 2, 2, true);
	CabinPressureReliefLever2.Register(PSH, "CabinPressureReliefLever2", 2, 3, true);

	SuitCircuitReturnValveLever.Register(PSH, "SuitCircuitReturnValveLever", 1);

	ORDEALFDAI1Switch.Register(PSH, "ORDEALFDAI1Switch", false);
	ORDEALFDAI2Switch.Register(PSH, "ORDEALFDAI2Switch", false);
	ORDEALEarthSwitch.Register(PSH, "ORDEALEarthSwitch", THREEPOSSWITCH_CENTER);
	ORDEALLightingSwitch.Register(PSH, "ORDEALLightingSwitch", THREEPOSSWITCH_CENTER);
	ORDEALModeSwitch.Register(PSH, "ORDEALModeSwitch", false);	
	ORDEALSlewSwitch.Register(PSH, "ORDEALSlewSwitch", THREEPOSSWITCH_CENTER);

	RNDZXPDRSwitch.Register(PSH, "RNDZXPDRSwitch", false);

	CMRCSHTRSSwitch.Register(PSH, "CMRCSHTRSSwitch", false);
	WasteH2ODumpSwitch.Register(PSH, "WasteH2ODumpSwitch", THREEPOSSWITCH_CENTER);
	UrineDumpSwitch.Register(PSH, "UrineDumpSwitch", THREEPOSSWITCH_CENTER);

	UtilityPowerSwitch.Register(PSH, "UtilityPowerSwitch", false);
	Panel100FloodDimSwitch.Register(PSH, "Panel100FloodDimSwitch", false);	
	Panel100FloodFixedSwitch.Register(PSH, "Panel100FloodFixedSwitch", false);
	GNPowerOpticsSwitch.Register(PSH, "GNPowerOpticsSwitch", false);
	GNPowerIMUSwitch.Register(PSH, "GNPowerIMUSwitch", TOGGLESWITCH_DOWN, false);
	GNPowerIMUSwitch.SetGuardResetsState(false);
	Panel100RNDZXPDRSwitch.Register(PSH, "RNDZXPDRSwitch", THREEPOSSWITCH_CENTER);

	SCIUtilPowerSwitch.Register(PSH, "SCIUtilPowerSwitch", false);

	InverterPower3MainBCircuitBraker.Register(PSH, "InverterPower3MainBCircuitBraker", 1);
	InverterPower3MainACircuitBraker.Register(PSH, "InverterPower3MainACircuitBraker", 1);
	InverterPower2MainBCircuitBraker.Register(PSH, "InverterPower2MainBCircuitBraker", 1);
	InverterPower1MainACircuitBraker.Register(PSH, "InverterPower1MainACircuitBraker", 1);
	FlightPostLandingMainBCircuitBraker.Register(PSH, "FlightPostLandingMainBCircuitBraker", 1);
	FlightPostLandingMainACircuitBraker.Register(PSH, "FlightPostLandingMainACircuitBraker", 1);
	FlightPostLandingBatCCircuitBraker.Register(PSH, "FlightPostLandingBatCCircuitBraker", 0);
	FlightPostLandingBatBusBCircuitBraker.Register(PSH, "FlightPostLandingBatBusBCircuitBraker", 0);
	FlightPostLandingBatBusACircuitBraker.Register(PSH, "FlightPostLandingBatBusACircuitBraker", 0);
	MainBBatBusBCircuitBraker.Register(PSH, "MainBBatBusBCircuitBraker", 1);
	MainBBatCCircuitBraker.Register(PSH, "MainBBatCCircuitBraker", 0);
	MainABatCCircuitBraker.Register(PSH, "MainABatCCircuitBraker", 0);
	MainABatBusACircuitBraker.Register(PSH, "MainABatBusACircuitBraker", 1);
	
	//
	// Old stuff. Delete when no longer required.
	//

	RPswitch17 = false;

	CMCswitch = true;
}

//
// Scenario state functions.
//

//
// The switch functions just pack the different switch areas from the control panel
// into 32-bit integers and unpack them from those integers. This provides a much more
// compact means of storing the switch states in the scenario file than saving them as
// individual values for each switch.
//

typedef union {
	struct {
		unsigned Cswitch1:1;
		unsigned Cswitch2:1;
		unsigned Cswitch3:1;
		unsigned Cswitch4:1;
		unsigned Cswitch5:1;
		unsigned Cswitch6:1;
		unsigned Cswitch7:1;
		unsigned Cswitch8:1;
		unsigned Cswitch9:1;
		unsigned DVCswitch:1;
		unsigned DVBCswitch:1;
		unsigned ELSCswitch:1;
		unsigned IMUCswitch:1;
		unsigned MRswitch:1;
		unsigned MRCswitch:1;
		unsigned TJ1switch:1;
		unsigned TJ1Cswitch:1;
		unsigned TJ2switch:1;
		unsigned TJ2Cswitch:1;
		unsigned IUswitch:1;
		unsigned IUCswitch:1;
		unsigned LVSswitch:1;
		unsigned LVSCswitch:1;
		unsigned CMRHGswitch:1;
	} u;
	unsigned long word;
} CSwitchState;

int Saturn::GetCSwitchState()

{
	CSwitchState state;

	state.word = 0;
	state.u.Cswitch5 = CsmLvSepSwitch.GetGuardState();
	state.u.CMRHGswitch = CmRcsHeDumpSwitch.GetGuardState();

	return state.word;
}

void Saturn::SetCSwitchState(int s)

{
	CSwitchState state;

	state.word = s;
	CsmLvSepSwitch.SetGuardState(state.u.Cswitch5);
	CmRcsHeDumpSwitch.SetGuardState(state.u.CMRHGswitch);
}

typedef union {
	struct {
		unsigned Sswitch1:1;
		unsigned Sswitch2:1;
		unsigned Sswitch3:1;
		unsigned Sswitch4:1;
		unsigned Sswitch5:1;
		unsigned Sswitch6:1;
		unsigned Sswitch7:1;
		unsigned Sswitch8:1;
		unsigned Sswitch9:1;
		unsigned DVAswitch:1;
		unsigned DVBswitch:1;
		unsigned ELSswitch:1;
		unsigned IMUswitch:1;
		unsigned CMRHDswitch:1;
	} u;
	unsigned long word;
} SSwitchState;

int Saturn::GetSSwitchState()

{
	SSwitchState state;

	state.word = 0;
	state.u.Sswitch5 = CsmLvSepSwitch;
	state.u.CMRHDswitch = CmRcsHeDumpSwitch;

	return state.word;
}

void Saturn::SetSSwitchState(int s)

{
	SSwitchState state;

	state.word = s;
	CsmLvSepSwitch = state.u.Sswitch5;
	CmRcsHeDumpSwitch = state.u.CMRHDswitch;
}

typedef union {
	struct {
		unsigned LPswitch4:1;
		unsigned LPswitch5:1;
		unsigned LPswitch6:1;
		unsigned LPswitch7:1;
		unsigned SPSswitch:1;
		unsigned EDSswitch:1;
		unsigned P11switch:1;
		unsigned P12switch:1;
		unsigned P13switch:1;
		unsigned P14switch:1;
		unsigned P15switch:1;
		unsigned P16switch:1;
		unsigned SCswitch:1;
		unsigned TLIswitch:1;
		unsigned P111switch:1;
		unsigned P112switch:1;
		unsigned P113switch:1;
		unsigned FCSMswitch:1;
	} u;
	unsigned long word;
} LPSwitchState;

int Saturn::GetLPSwitchState()

{
	LPSwitchState state;

	state.word = 0;
	state.u.LPswitch5 = OrbiterAttitudeToggle;
	state.u.SCswitch = SCswitch;

	return state.word;
}

void Saturn::SetLPSwitchState(int s)

{
	LPSwitchState state;

	state.word = s;
	OrbiterAttitudeToggle = state.u.LPswitch5;
	SCswitch = state.u.SCswitch;
}

typedef union {
	struct {
		unsigned RPswitch1:1;
		unsigned RPswitch2:1;
		unsigned RPswitch3:1;
		unsigned RPswitch4:1;
		unsigned RPswitch5:1;
		unsigned RPswitch6:1;
		unsigned RPswitch7:1;
		unsigned RPswitch8:1;
		unsigned RPswitch9:1;
		unsigned RPswitch10:1;
		unsigned RPswitch11:1;
		unsigned RPswitch12:1;
		unsigned RPswitch16:1;
		unsigned RPCswitch:1;
		unsigned CMRswitch:1;
		unsigned CMRCswitch:1;
		unsigned CMCswitch:1;
	} u;
	unsigned long word;
} RPSwitchState;

int Saturn::GetRPSwitchState()

{
	RPSwitchState state;

	state.word = 0;

	if (!SkylabCM) {
		state.u.RPswitch16 = SivbLmSepSwitch;
		state.u.RPCswitch = SivbLmSepSwitch.GetGuardState();
	}

	state.u.CMCswitch = CMCswitch;

	return state.word;
}

void Saturn::SetRPSwitchState(int s)

{
	RPSwitchState state;

	state.word = s;

	if (!SkylabCM) {
		SivbLmSepSwitch = state.u.RPswitch16;
		SivbLmSepSwitch.SetGuardState(state.u.RPCswitch);
	}

	CMCswitch = state.u.CMCswitch;
}

