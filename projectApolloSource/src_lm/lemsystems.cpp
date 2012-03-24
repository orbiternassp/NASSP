/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant
  Copyright 2002-2005 Chris Knestrick


  ORBITER vessel module: LEM systems code

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
  *	Revision 1.30  2011/07/16 18:46:48  dseagrav
  *	LM RR work, first part
  *	
  *	Revision 1.29  2011/07/15 00:50:21  vrouleau
  *	FDAI error needles displays the RR trunnion/shart angles
  *	
  *	Revision 1.28  2011/07/11 01:42:36  vrouleau
  *	- Removed AGC_SOCKET_ENABLED flag. Rework is needed to make this an optional feature instead of a conditional define. To many untested think exists in the socket version
  *	
  *	- Checkpoint commit on the LEM RR. If the RR as been slew to track the CSM , the auto mode will continue tracking it.
  *	
  *	Revision 1.27  2011/07/07 11:58:45  vrouleau
  *	Checkpoint commit for LEM rendezvous radar:
  *	 - Added range,rate and CSM direction calculation.
  *	 - Slewing of the shaft & trunnion
  *	 - IO from/to AGC.
  *	
  *	Revision 1.26  2010/08/28 16:16:33  dseagrav
  *	Fixed LM DSKY to use dimmer. (Dimmer source may be wrong)
  *	Corrected a typo and did some bracketization in DSKY source.
  *	Wasted a lot of time figuring out our copy of Luminary had been garbaged.
  *	Your Luminary 99 binary should be overwritten by the commit before this one.
  *	If your binary is 700 bytes in size you have the garbaged version.
  *	
  *	Revision 1.25  2010/07/16 17:14:42  tschachim
  *	Changes for Orbiter 2010 and bugfixes
  *	
  *	Revision 1.24  2010/05/24 03:50:34  dseagrav
  *	Updates to RCS, CWEA, ATCA
  *	
  *	Revision 1.23  2010/05/23 05:34:04  dseagrav
  *	CWEA test switch partially implemented, reorganized CBs and added the remaining CBs to the panels (but not systems yet)
  *	
  *	Revision 1.22  2010/05/16 04:54:12  dseagrav
  *	LM Checkpoint Commit. More CWEA stuff, ECS stuff, etc.
  *	
  *	Revision 1.21  2010/05/12 05:01:30  dseagrav
  *	CWEA stuff for LGC and ISS, beginnings of ECS
  *	
  *	Revision 1.20  2010/05/11 01:33:15  dseagrav
  *	More CWEA work. Light constantly on = Detection conditions for that light are not yet implemented.
  *	
  *	Revision 1.19  2010/05/10 06:45:25  dseagrav
  *	Started on LM CWEA
  *	
  *	Revision 1.18  2010/05/10 01:49:25  dseagrav
  *	Added more LM indicators.
  *	Hacked around a bug in toggleswitch where indicators with minimums below zero would float while unpowered.
  *	
  *	Revision 1.17  2010/05/02 16:04:05  dseagrav
  *	Added RCS and ECS indicators. Values are not yet provided.
  *	
  *	Revision 1.16  2010/05/01 12:55:15  dseagrav
  *	
  *	Cause LM mission timer to print value when adjusted. (Since you can't see it from the switches)
  *	Right-clicking causes the time to be printed but does not flip the switches.
  *	Left-clicking works as normal and prints the new value.
  *	The printed value is not updated and is removed after five seconds.
  *	
  *	Revision 1.15  2009/10/19 12:24:49  dseagrav
  *	LM checkpoint commit.
  *	Put back one FDAI for testing purposes (graphic is wrong)
  *	Messed around with mass properties
  *	LGC now runs Luminary 099 instead of 131
  *	Added LGC pad load, values need worked but addresses are checked.
  *	
  *	Revision 1.14  2009/09/17 17:48:41  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.13  2009/09/14 00:19:41  trebonian
  *	Stage commit for AGS - DEDA keyboard and display
  *	
  *	Revision 1.12  2009/09/13 20:31:33  dseagrav
  *	Joystick Z-axis detection fixes
  *	
  *	Revision 1.11  2009/09/13 15:20:15  dseagrav
  *	LM Checkpoint Commit. Adds LM telemetry, fixed missing switch row init, initial LM support for PAMFD.
  *	
  *	Revision 1.10  2009/09/10 02:12:37  dseagrav
  *	Added lm_ags and lm_telecom files, LM checkpoint commit.
  *	
  *	Revision 1.9  2009/09/03 19:22:48  vrouleau
  *	Remove usage on Joystick.ini and VirtualAGC.ini. Moved to respective .launchpag.cfg files
  *	
  *	Revision 1.8  2009/09/02 18:26:46  vrouleau
  *	MultiThread support for vAGC
  *	
  *	Revision 1.7  2009/09/01 06:18:32  dseagrav
  *	LM Checkpoint Commit. Added switches. Added history to LM SCS files. Added bitmap to LM. Added AIDs.
  *	
  *	Revision 1.6  2009/08/24 02:20:20  dseagrav
  *	LM Checkpoint Commit: Adds more systems, heater power drains, fix use of stage before init
  *	
  *	Revision 1.5  2009/08/16 03:12:38  dseagrav
  *	More LM EPS work. CSM to LM power transfer implemented. Optics bugs cleared up.
  *	
  *	Revision 1.4  2009/08/10 02:26:17  dseagrav
  *	I forgot to re-enable the optics timestep item.
  *	
  *	Revision 1.3  2009/08/10 02:23:06  dseagrav
  *	LEM EPS (Part 2)
  *	Split ECAs into channels, Made bus cross tie system, Added ascent systems and deadface/staging logic.
  *	
  *	Revision 1.2  2009/08/01 19:48:33  jasonims
  *	LM Optics Code Added, along with rudimentary Graphics for AOT.
  *	Reticle uses GDI objects to allow realtime rotation.
  *	
  *	Revision 1.1  2009/02/18 23:21:14  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.26  2008/04/11 11:49:44  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.25  2007/11/30 16:40:40  movieman523
  *	Revised LEM to use generic voltmeter and ammeter code. Note that the ED battery select switch needs to be implemented to fully support the voltmeter/ammeter now.
  *	
  *	Revision 1.24  2007/06/06 15:02:14  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.23  2006/08/21 03:04:38  dseagrav
  *	This patch adds DC volt/amp meters and associated switches, which was an unholy pain in the
  *	
  *	Revision 1.22  2006/08/20 08:28:06  dseagrav
  *	LM Stage Switch actually causes staging (VERY INCOMPLETE), Incorrect "Ascent RCS" removed, ECA outputs forced to 24V during initialization to prevent IMU/LGC failure on scenario load, Valves closed by default, EDS saves RCS valve states, would you like fries with that?
  *	
  *	Revision 1.21  2006/08/18 05:45:01  dseagrav
  *	LM EDS now exists. Talkbacks wired to a power source will revert to BP when they lose power.
  *	
  *	Revision 1.20  2006/08/13 23:33:16  dseagrav
  *	No comment.
  *	
  *	Revision 1.19  2006/08/13 23:12:41  dseagrav
  *	Joystick improvements
  *	
  *	Revision 1.18  2006/08/13 16:55:35  movieman523
  *	Removed a bunch of unused files.
  *	
  *	Revision 1.17  2006/08/13 16:01:52  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  *	Revision 1.16  2006/08/13 10:37:55  dseagrav
  *	TTCA enabled in JETS mode
  *	
  *	Revision 1.15  2006/08/13 06:30:49  dseagrav
  *	LM checkpoint commit.
  *	
  *	Revision 1.14  2006/07/24 06:41:29  dseagrav
  *	Many changes - Rearranged / corrected FDAI power usage, added LM AC equipment, many bugfixes
  *	
  *	Revision 1.13  2006/06/18 22:45:31  dseagrav
  *	LM ECA bug fix, LGC,IMU,DSKY and IMU OPR wired to CBs, IMU OPR,LGC,FDAI,and DSKY draw power
  *	
  *	Revision 1.12  2006/06/18 16:43:07  dseagrav
  *	LM EPS fixes, LMP/CDR DC busses now powered thru CBs, ECA power-off bug fixed and ECA speed improvement
  *	
  *	Revision 1.11  2006/06/11 09:20:30  dseagrav
  *	LM ECA #2 added, ECA low-voltage tap usage added, CDR & LMP DC busses wired to ECAs
  *	
  *	Revision 1.10  2006/05/01 08:52:50  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.9  2006/04/23 04:15:46  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.8  2006/04/12 06:27:19  dseagrav
  *	LM checkpoint commit. The LM is not airworthy at this point. Please be patient.
  *	
  *	Revision 1.7  2006/01/14 21:59:52  movieman523
  *	Added PanelSDK, init, timestep, save and load.
  *	
  *	Revision 1.6  2006/01/09 19:26:03  tschachim
  *	More attempts to make code build on MS C++ 2005
  *	
  *	Revision 1.5  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.4  2005/08/10 20:00:55  spacex15
  *	Activated 3 position lem eng arm switch
  *	
  *	Revision 1.3  2005/08/09 09:19:12  tschachim
  *	Introduced toggleswitch lib
  *	
  *	Revision 1.2  2005/08/07 19:28:08  lazyd
  *	Changed LM RCS and ascent parameters to historical
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
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
#include "lm_channels.h"
#include "dsky.h"
#include "IMU.h"
#include "PanelSDK/Internals/Esystems.h"
#include "ioChannels.h"

#include "LEM.h"

#include "CollisionSDK/CollisionSDK.h"


#define RR_SHAFT_STEP 0.000191747598876953125 
#define RR_TRUNNION_STEP 0.00004793689959716796875


void LEM::ResetThrusters()

{
	SetAttitudeRotLevel(0, 0);
	SetAttitudeRotLevel(1, 0);
	SetAttitudeRotLevel(2, 0);
	SetAttitudeLinLevel(0, 0);
	SetAttitudeLinLevel(1, 0);
	SetAttitudeLinLevel(2, 0);
	ActivateNavmode(NAVMODE_KILLROT);
}

void LEM::AddRCS_LMH(double TRANZ)
{
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = 1.35;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.15;
	const double ATTHEIGHT=3;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;

	double MaxThrust=445.0;
	double RCSISP=2840.0;

	// A1U
	th_rcs[0]=CreateThruster(_V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[0],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A1F
	th_rcs[1]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[1],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1L
	th_rcs[2]=CreateThruster(_V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[2],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1D
	th_rcs[3]=CreateThruster(_V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[3],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B2U
	th_rcs[4]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[4],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B2L
	th_rcs[5]=CreateThruster(_V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[5],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2A
	th_rcs[6]=CreateThruster(_V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[6],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2D
	th_rcs[7]=CreateThruster(_V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[7],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// A3U
	th_rcs[8]=CreateThruster(_V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[8],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A3R
	th_rcs[9]=CreateThruster(_V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(-1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[9],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3A
	th_rcs[10]=CreateThruster(_V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[10],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3D
	th_rcs[11]=CreateThruster(_V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[11],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B4U
	th_rcs[12]=CreateThruster(_V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[12],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B4F
	th_rcs[13]=CreateThruster(_V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[13],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4R
	th_rcs[14]=CreateThruster(_V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(-1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[14],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4D
	th_rcs[15]=CreateThruster(_V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[15],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// Setup Orbiter thruster groups
	th_rcs_orbiter_rot[0] = th_rcs[0];  // A1U
	th_rcs_orbiter_rot[1] = th_rcs[12]; // B4U
	th_rcs_orbiter_rot[2] = th_rcs[11]; // B3D
	th_rcs_orbiter_rot[3] = th_rcs[7];  // A2D
	CreateThrusterGroup (th_rcs_orbiter_rot+0, 4, THGROUP_ATT_PITCHDOWN);
	th_rcs_orbiter_rot[4] = th_rcs[15]; // A4D
	th_rcs_orbiter_rot[5] = th_rcs[3];  // B1D
	th_rcs_orbiter_rot[6] = th_rcs[8];  // A3U
	th_rcs_orbiter_rot[7] = th_rcs[4];  // B2U
	CreateThrusterGroup (th_rcs_orbiter_rot+4, 4, THGROUP_ATT_PITCHUP);
	th_rcs_orbiter_rot[8] = th_rcs[0];  // A1U
	th_rcs_orbiter_rot[9] = th_rcs[4];  // B2U
	th_rcs_orbiter_rot[10]= th_rcs[11]; // B3D
	th_rcs_orbiter_rot[11]= th_rcs[15]; // A4D
	CreateThrusterGroup (th_rcs_orbiter_rot+8, 4, THGROUP_ATT_BANKLEFT);
	th_rcs_orbiter_rot[12]= th_rcs[3];  // B1D
	th_rcs_orbiter_rot[13]= th_rcs[7];  // A2D
	th_rcs_orbiter_rot[14]= th_rcs[12]; // B4U
	th_rcs_orbiter_rot[15]= th_rcs[8];  // A3U
	CreateThrusterGroup (th_rcs_orbiter_rot+12, 4,THGROUP_ATT_BANKRIGHT);
	th_rcs_orbiter_rot[16]= th_rcs[1];  // A1F
	th_rcs_orbiter_rot[17]= th_rcs[14]; // A4R
	th_rcs_orbiter_rot[18]= th_rcs[10]; // B3A
	th_rcs_orbiter_rot[19]= th_rcs[5];  // B2L
	CreateThrusterGroup (th_rcs_orbiter_rot+16, 4,THGROUP_ATT_YAWLEFT);
	th_rcs_orbiter_rot[20]= th_rcs[2];  // B1L
	th_rcs_orbiter_rot[21]= th_rcs[9];  // A3R
	th_rcs_orbiter_rot[22]= th_rcs[13]; // B4F
	th_rcs_orbiter_rot[23]= th_rcs[6];  // A2A
	CreateThrusterGroup (th_rcs_orbiter_rot+20, 4,THGROUP_ATT_YAWRIGHT);

	th_rcs_orbiter_lin[0] = th_rcs[3];  // B1D
	th_rcs_orbiter_lin[1] = th_rcs[7];  // A2D
	th_rcs_orbiter_lin[2] = th_rcs[11]; // B3D
	th_rcs_orbiter_lin[3] = th_rcs[15]; // A4D
	CreateThrusterGroup (th_rcs_orbiter_lin + 0, 4, THGROUP_ATT_UP);
	th_rcs_orbiter_lin[4] = th_rcs[0];  // A1U
	th_rcs_orbiter_lin[5] = th_rcs[4];  // B2U
	th_rcs_orbiter_lin[6] = th_rcs[8];  // A3U
	th_rcs_orbiter_lin[7] = th_rcs[12]; // B4U
	CreateThrusterGroup (th_rcs_orbiter_lin + 4, 4, THGROUP_ATT_DOWN);
	th_rcs_orbiter_lin[8] = th_rcs[9];  // A3R
	th_rcs_orbiter_lin[9] = th_rcs[14]; // A4R
	CreateThrusterGroup (th_rcs_orbiter_lin + 8, 2, THGROUP_ATT_LEFT);
	th_rcs_orbiter_lin[10] = th_rcs[2];  // B1L
	th_rcs_orbiter_lin[11] = th_rcs[5];  // B2L
	CreateThrusterGroup (th_rcs_orbiter_lin + 10, 2, THGROUP_ATT_RIGHT);
	th_rcs_orbiter_lin[12] = th_rcs[6];  // A2A
	th_rcs_orbiter_lin[13] = th_rcs[10]; // B3A
	CreateThrusterGroup (th_rcs_orbiter_lin + 12, 2, THGROUP_ATT_FORWARD);
	th_rcs_orbiter_lin[14] = th_rcs[1];  // A1F
	th_rcs_orbiter_lin[15] = th_rcs[13]; // B4F
	CreateThrusterGroup (th_rcs_orbiter_lin + 14, 2, THGROUP_ATT_BACK);

	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/

//	CreateThrusterGroup (th_rcs,   1, THGROUP_ATT_YAWLEFT);
}

void LEM::AddRCS_LMH2(double TRANZ)
{
	const double ATTCOOR = 1.78;
	const double ATTCOOR2 = -0.50;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH=.15;
	const double ATTHEIGHT=3;
	const double TRANWIDTH=.2;
	const double TRANHEIGHT=1;
	const double RCSOFFSET=0.75;
	const double RCSOFFSETM=0.30;
	const double RCSOFFSETM2=0.47;

	double MaxThrust=445.0;
	double RCSISP=2840.0;

	// A1U
	th_rcs[0]=CreateThruster(_V(-ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[0],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A1F
	th_rcs[1]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[1],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1L
	th_rcs[2]=CreateThruster(_V(-ATTCOOR-0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[2],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B1D
	th_rcs[3]=CreateThruster(_V(-ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[3],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B2U
	th_rcs[4]=CreateThruster(_V(-ATTCOOR+.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.30), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[4],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B2L
	th_rcs[5]=CreateThruster(_V(-ATTCOOR-0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[5],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2A
	th_rcs[6]=CreateThruster(_V(-ATTCOOR+.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[6],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A2D
	th_rcs[7]=CreateThruster(_V(-ATTCOOR+.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[7],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// A3U
	th_rcs[8]=CreateThruster(_V(ATTCOOR-0.05,ATTCOOR2+0.0,TRANZ+RCSOFFSETM2-0.35), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[8],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A3R
	th_rcs[9]=CreateThruster(_V(ATTCOOR+0.2,ATTCOOR2-0.35,TRANZ+RCSOFFSETM2-.25), _V(-1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[9],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3A
	th_rcs[10]=CreateThruster(_V(ATTCOOR-.10,ATTCOOR2-0.35,TRANZ+RCSOFFSET-0.8), _V(0,0,1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[10],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B3D
	th_rcs[11]=CreateThruster(_V(ATTCOOR-0.1,ATTCOOR2-.65,TRANZ+RCSOFFSETM2-0.35), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[11],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// B4U
	th_rcs[12]=CreateThruster(_V(ATTCOOR,ATTCOOR2+0.3,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,-1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[12],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// B4F
	th_rcs[13]=CreateThruster(_V(ATTCOOR-.001,ATTCOOR2-0.18,TRANZ+RCSOFFSET+3.10), _V(0,0,-1), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[13],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4R
	th_rcs[14]=CreateThruster(_V(ATTCOOR+0.30,ATTCOOR2-0.18,TRANZ+RCSOFFSETM2+3.17), _V(-1,0,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[14],ATTHEIGHT,ATTWIDTH, exhaustTex);
	// A4D
	th_rcs[15]=CreateThruster(_V(ATTCOOR,ATTCOOR2-.55,TRANZ+RCSOFFSETM2-0.12+3.3), _V(0,1,0), MaxThrust, NULL, RCSISP, RCSISP);
	AddExhaust(th_rcs[15],ATTHEIGHT,ATTWIDTH, exhaustTex);

	// Setup Orbiter thruster groups
	th_rcs_orbiter_rot[0] = th_rcs[0];  // A1U
	th_rcs_orbiter_rot[1] = th_rcs[12]; // B4U
	th_rcs_orbiter_rot[2] = th_rcs[11]; // B3D
	th_rcs_orbiter_rot[3] = th_rcs[7];  // A2D
	CreateThrusterGroup (th_rcs_orbiter_rot+0, 4, THGROUP_ATT_PITCHDOWN);
	th_rcs_orbiter_rot[4] = th_rcs[15]; // A4D
	th_rcs_orbiter_rot[5] = th_rcs[3];  // B1D
	th_rcs_orbiter_rot[6] = th_rcs[8];  // A3U
	th_rcs_orbiter_rot[7] = th_rcs[4];  // B2U
	CreateThrusterGroup (th_rcs_orbiter_rot+4, 4, THGROUP_ATT_PITCHUP);
	th_rcs_orbiter_rot[8] = th_rcs[0];  // A1U
	th_rcs_orbiter_rot[9] = th_rcs[4];  // B2U
	th_rcs_orbiter_rot[10]= th_rcs[11]; // B3D
	th_rcs_orbiter_rot[11]= th_rcs[15]; // A4D
	CreateThrusterGroup (th_rcs_orbiter_rot+8, 4, THGROUP_ATT_BANKLEFT);
	th_rcs_orbiter_rot[12]= th_rcs[3];  // B1D
	th_rcs_orbiter_rot[13]= th_rcs[7];  // A2D
	th_rcs_orbiter_rot[14]= th_rcs[12]; // B4U
	th_rcs_orbiter_rot[15]= th_rcs[8];  // A3U
	CreateThrusterGroup (th_rcs_orbiter_rot+12, 4,THGROUP_ATT_BANKRIGHT);
	th_rcs_orbiter_rot[16]= th_rcs[1];  // A1F
	th_rcs_orbiter_rot[17]= th_rcs[14]; // A4R
	th_rcs_orbiter_rot[18]= th_rcs[10]; // B3A
	th_rcs_orbiter_rot[19]= th_rcs[5];  // B2L
	CreateThrusterGroup (th_rcs_orbiter_rot+16, 4,THGROUP_ATT_YAWLEFT);
	th_rcs_orbiter_rot[20]= th_rcs[2];  // B1L
	th_rcs_orbiter_rot[21]= th_rcs[9];  // A3R
	th_rcs_orbiter_rot[22]= th_rcs[13]; // B4F
	th_rcs_orbiter_rot[23]= th_rcs[6];  // A2A
	CreateThrusterGroup (th_rcs_orbiter_rot+20, 4,THGROUP_ATT_YAWRIGHT);

	th_rcs_orbiter_lin[0] = th_rcs[3];  // B1D
	th_rcs_orbiter_lin[1] = th_rcs[7];  // A2D
	th_rcs_orbiter_lin[2] = th_rcs[11]; // B3D
	th_rcs_orbiter_lin[3] = th_rcs[15]; // A4D
	CreateThrusterGroup (th_rcs_orbiter_lin + 0, 4, THGROUP_ATT_UP);
	th_rcs_orbiter_lin[4] = th_rcs[0];  // A1U
	th_rcs_orbiter_lin[5] = th_rcs[4];  // B2U
	th_rcs_orbiter_lin[6] = th_rcs[8];  // A3U
	th_rcs_orbiter_lin[7] = th_rcs[12]; // B4U
	CreateThrusterGroup (th_rcs_orbiter_lin + 4, 4, THGROUP_ATT_DOWN);
	th_rcs_orbiter_lin[8] = th_rcs[9];  // A3R
	th_rcs_orbiter_lin[9] = th_rcs[14]; // A4R
	CreateThrusterGroup (th_rcs_orbiter_lin + 8, 2, THGROUP_ATT_LEFT);
	th_rcs_orbiter_lin[10] = th_rcs[2];  // B1L
	th_rcs_orbiter_lin[11] = th_rcs[5];  // B2L
	CreateThrusterGroup (th_rcs_orbiter_lin + 10, 2, THGROUP_ATT_RIGHT);
	th_rcs_orbiter_lin[12] = th_rcs[6];  // A2A
	th_rcs_orbiter_lin[13] = th_rcs[10]; // B3A
	CreateThrusterGroup (th_rcs_orbiter_lin + 12, 2, THGROUP_ATT_FORWARD);
	th_rcs_orbiter_lin[14] = th_rcs[1];  // A1F
	th_rcs_orbiter_lin[15] = th_rcs[13]; // B4F
	CreateThrusterGroup (th_rcs_orbiter_lin + 14, 2, THGROUP_ATT_BACK);
}

bool LEM::CabinFansActive()

{
	return CABFswitch;
}

bool LEM::AscentEngineArmed()

{
	return (EngineArmSwitch.IsUp()); //&& !ASCHE1switch && !ASCHE2switch && ED1switch && ED6switch && ED7switch && ED8switch;
}


void LEM::SystemsInit()

{
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo/LEMSystems");

	// DS20060407 Start wiring things together

	// Batteries
	Battery1 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_A");
	Battery2 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_B");
	Battery3 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_C");
	Battery4 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:DSC_BATTERY_D");
	Battery5 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_A");
	Battery6 = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:ASC_BATTERY_B");
	LunarBattery = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:LUNAR_BATTERY");
	// Batteries 1-4 and the Lunar Stay Battery are jettisoned with the descent stage.

	// ECA #1 (DESCENT stage, LMP DC bus)
	ECA_1a.Init(this, Battery1, 2); // Battery 1 starts on LV
	ECA_1b.Init(this, Battery2, 0);
	ECA_1a.dc_source_tb = &DSCBattery1TB;
	ECA_1a.dc_source_tb->SetState(2); // Initialize to LV
	ECA_1b.dc_source_tb = &DSCBattery2TB;
	ECA_1b.dc_source_tb->SetState(0); // Initialize to off

	// ECA #2 (DESCENT stage, CDR DC bus)
	ECA_2a.Init(this, Battery3, 0);
	ECA_2b.Init(this, Battery4, 2); 
	ECA_2a.dc_source_tb = &DSCBattery3TB;
	ECA_2a.dc_source_tb->SetState(0); 
	ECA_2b.dc_source_tb = &DSCBattery4TB;
	ECA_2b.dc_source_tb->SetState(2);

	// ECA #1 and #2 are JETTISONED with the descent stage.
	// ECA #3 and #4 have no low voltage taps and can feed either bus.
	ECA_3a.Init(this, Battery5, 0);
	ECA_3b.Init(this, Battery5, 0);
	ECA_3a.dc_source_tb = &ASCBattery5ATB;
	ECA_3a.dc_source_tb->SetState(0); // Initialize to off
	ECA_3b.dc_source_tb = &ASCBattery5BTB;
	ECA_3b.dc_source_tb->SetState(0); // Initialize to off
	ECA_4a.Init(this, Battery6, 0);
	ECA_4b.Init(this, Battery6, 0);
	ECA_4a.dc_source_tb = &ASCBattery6ATB;
	ECA_4a.dc_source_tb->SetState(0); // Initialize to off
	ECA_4b.dc_source_tb = &ASCBattery6BTB;
	ECA_4b.dc_source_tb->SetState(0); // Initialize to off

	// Descent Stage Deadface Bus Stubs wire to the ECAs
	// stage is not defined here, so we can't do this.

	// Bus Tie Blocks (Not real objects)
	BTB_LMP_B.Init(this,&DES_LMPs28VBusA,&ECA_4b);
	BTB_LMP_C.Init(this,&DES_LMPs28VBusB,&ECA_3a);
	BTB_CDR_B.Init(this,&DES_CDRs28VBusA,&ECA_3b);
	BTB_CDR_C.Init(this,&DES_CDRs28VBusB,&ECA_4a);

	// Bus feed tie breakers are sourced from the descent busses AND from ECA 3/4
	// via ficticious Bus Tie Blocks
	LMPBatteryFeedTieCB1.MaxAmps = 100.0;
	LMPBatteryFeedTieCB1.WireTo(&BTB_LMP_B);
	LMPBatteryFeedTieCB2.MaxAmps = 100.0;
	LMPBatteryFeedTieCB2.WireTo(&BTB_LMP_C);
	CDRBatteryFeedTieCB1.MaxAmps = 100.0;
	CDRBatteryFeedTieCB1.WireTo(&BTB_CDR_B);
	CDRBatteryFeedTieCB2.MaxAmps = 100.0;
	CDRBatteryFeedTieCB2.WireTo(&BTB_CDR_C);

	// Set up XLunar system
	BTC_XLunar.Init(this);

	// Main busses can be fed from the ECAs via the BAT FEED TIE CBs,
	// the other bus via the CROSS TIE BUS / CROSS TIE BAL LOADS CBs,
	// or the CSM via the XLUNAR bus and associated etcetera.
	// At this point, we have the sum of all battery feeds on BTB D and the XLUNAR feeds on BTB A
	BTB_CDR_D.Init(this,&CDRBatteryFeedTieCB1,&CDRBatteryFeedTieCB2); // Tie dual CBs together for CDR bus
	BTB_LMP_D.Init(this,&LMPBatteryFeedTieCB1,&LMPBatteryFeedTieCB2); // Tie dual CBs together for LMP bus

	BTB_CDR_A.Init(this,&BTB_CDR_D,&BTC_XLunar.dc_output);		      // Tie batteries and XLUNAR feed for CDR bus
	BTB_LMP_A.Init(this,&BTB_LMP_D,NULL);							  // Not really necessary but keeps things even

	// Bus cross-tie breakers
	CDRCrossTieBalCB.MaxAmps = 30.0;
	CDRCrossTieBalCB.WireTo(&CDRs28VBus);
	CDRCrossTieBusCB.MaxAmps = 100.0;
	CDRCrossTieBusCB.WireTo(&CDRs28VBus);
	LMPCrossTieBalCB.MaxAmps = 30.0;
	LMPCrossTieBalCB.WireTo(&LMPs28VBus);
	LMPCrossTieBusCB.MaxAmps = 100.0;
	LMPCrossTieBusCB.WireTo(&LMPs28VBus);

	// Bus cross-tie setup: Wire the two busses to the multiplexer and tell it
	// where the CBs are. The multiplexer will do the rest.
	BTC_MPX.Init(this,&LMPs28VBus,&CDRs28VBus,&LMPCrossTieBalCB,&LMPCrossTieBusCB,&CDRCrossTieBalCB,&CDRCrossTieBusCB);
	
	// At this point, the sum of bus feeds are on BTB A, and the cross-tie sources are on the mpx.

	// Join cross-ties and main-ties for bus source
	BTB_LMP_E.Init(this,&BTB_LMP_A,&BTC_MPX.dc_output_lmp);
	BTB_CDR_E.Init(this,&BTB_CDR_A,&BTC_MPX.dc_output_cdr);

	// EPS Displays
	EPS_DISP_CB.MaxAmps = 2.0;
	EPS_DISP_CB.WireTo(&LMPs28VBus);
	EPSMonitorSelectRotary.WireTo(&EPS_DISP_CB);

	// Descent battery TBs
	DSCBattery1TB.WireTo(&EPS_DISP_CB);
	DSCBattery2TB.WireTo(&EPS_DISP_CB);
	DSCBattery3TB.WireTo(&EPS_DISP_CB);
	DSCBattery4TB.WireTo(&EPS_DISP_CB);
	DSCBattFeedTB.WireTo(&EPS_DISP_CB);
	// Ascent battery TBs
	ASCBattery5ATB.WireTo(&EPS_DISP_CB);
	ASCBattery5BTB.WireTo(&EPS_DISP_CB);
	ASCBattery6ATB.WireTo(&EPS_DISP_CB);
	ASCBattery6BTB.WireTo(&EPS_DISP_CB);

	// ECA stuff
	CDRAscECAContCB.MaxAmps = 3.0;
	CDRAscECAContCB.WireTo(&CDRs28VBus);
	CDRAscECAMainCB.MaxAmps = 2.0;
	CDRAscECAMainCB.WireTo(&CDRs28VBus);
	CDRDesECAContCB.MaxAmps = 5.0;
	CDRDesECAContCB.WireTo(&CDRs28VBus);
	CDRDesECAMainCB.MaxAmps = 3.0;
	CDRDesECAMainCB.WireTo(&CDRs28VBus);
	LMPAscECAContCB.MaxAmps = 3.0;
	LMPAscECAContCB.WireTo(&LMPs28VBus);
	LMPAscECAMainCB.MaxAmps = 2.0;
	LMPAscECAMainCB.WireTo(&LMPs28VBus);
	LMPDesECAContCB.MaxAmps = 5.0;
	LMPDesECAContCB.WireTo(&LMPs28VBus);
	LMPDesECAMainCB.MaxAmps = 3.0;
	LMPDesECAMainCB.WireTo(&LMPs28VBus);

	// REACTION CONTROL SYSTEM
	RCS_B_PQGS_DISP_CB.MaxAmps = 2.0;
	RCS_B_PQGS_DISP_CB.WireTo(&LMPs28VBus);
	LMRCSATempInd.WireTo(&RCS_B_PQGS_DISP_CB);
	LMRCSBTempInd.WireTo(&RCS_B_PQGS_DISP_CB);
	LMRCSAPressInd.WireTo(&RCS_B_PQGS_DISP_CB);
	LMRCSBPressInd.WireTo(&RCS_B_PQGS_DISP_CB);
	LMRCSAQtyInd.WireTo(&RCS_B_PQGS_DISP_CB);
	LMRCSBQtyInd.WireTo(&RCS_B_PQGS_DISP_CB);
	
	// CDR and LMP 28V DC busses.
	// Wire to ficticious bus tie block
	CDRs28VBus.WireTo(&BTB_CDR_E); 
	LMPs28VBus.WireTo(&BTB_LMP_E);

	// DC voltmeter CBs
	CDRDCBusVoltCB.MaxAmps = 2.0;
	CDRDCBusVoltCB.WireTo(&CDRs28VBus);
	LMPDCBusVoltCB.MaxAmps = 2.0;
	LMPDCBusVoltCB.WireTo(&LMPs28VBus);

	// AC Inverter CBs
	CDRInverter1CB.MaxAmps = 30.0;
	CDRInverter1CB.WireTo(&CDRs28VBus);
	LMPInverter2CB.MaxAmps = 30.0;
	LMPInverter2CB.WireTo(&LMPs28VBus);
	// AC Inverters
	INV_1.dc_input = &CDRInverter1CB;	
	INV_2.dc_input = &LMPInverter2CB; 	
	// AC bus voltmeter breaker
	AC_A_BUS_VOLT_CB.MaxAmps = 2.0;
	AC_A_BUS_VOLT_CB.WireTo(&ACBusA);
	// AC bus input breakers
	AC_A_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_1_FEED_CB.WireTo(&INV_1);
	AC_B_INV_1_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_1_FEED_CB.WireTo(&INV_1);
	AC_A_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_A_INV_2_FEED_CB.WireTo(&INV_2);
	AC_B_INV_2_FEED_CB.MaxAmps = 5.0;
	AC_B_INV_2_FEED_CB.WireTo(&INV_2);

	// AC busses
	ACBusA.Disconnect();
	ACBusB.Disconnect();
	// Situation load will wire these to their breakers later if needed

	// AC bus attenuator.
	ACVoltsAttenuator.WireTo(&AC_A_BUS_VOLT_CB);

	// RCS Main Shutoff valves
	RCSMainSovASwitch.WireTo(&CDRs28VBus);
	RCSMainSovATB.WireTo(&CDRs28VBus);
	RCSMainSovBTB.WireTo(&LMPs28VBus);
	RCSMainSovBSwitch.WireTo(&LMPs28VBus);

	// Lighting
	CDR_LTG_UTIL_CB.MaxAmps = 2.0;
	CDR_LTG_UTIL_CB.WireTo(&CDRs28VBus);
	CDR_LTG_ANUN_DOCK_COMPNT_CB.MaxAmps = 2.0;
	CDR_LTG_ANUN_DOCK_COMPNT_CB.WireTo(&CDRs28VBus);
	LTG_ANUN_DOCK_COMPNT_CB.MaxAmps = 2.0;
	LTG_ANUN_DOCK_COMPNT_CB.WireTo(&LMPs28VBus);
	LTG_FLOOD_CB.MaxAmps = 5.0;
	LTG_FLOOD_CB.WireTo(&LMPs28VBus);
	NUM_LTG_AC_CB.MaxAmps = 2.0;
	NUM_LTG_AC_CB.WireTo(&ACBusB);

	// LGC and DSKY
	LGC_DSKY_CB.MaxAmps = 7.5;
	LGC_DSKY_CB.WireTo(&CDRs28VBus);
	agc.WirePower(&LGC_DSKY_CB,&LGC_DSKY_CB);
	// The DSKY brightness IS controlled by the ANUN/NUM knob on panel 5, but by means of an isolated section of it.
	// The source of the isolated section may be from the LGC supply or AC bus. So this may not be correct. If the CB pops, investigate!
	dsky.Init(&NUM_LTG_AC_CB, &LtgAnunNumKnob);

	// AGS stuff
	asa.Init(this);
	aea.Init(this);
	deda.Init(&SCS_AEA_CB);

	// IMU OPERATE power (Logic DC power)
	IMU_OPR_CB.MaxAmps = 20.0;
	IMU_OPR_CB.WireTo(&CDRs28VBus);	
	imu.WireToBuses(&IMU_OPR_CB, NULL, NULL);
	// IMU STANDBY power (Heater DC power when not operating)
	IMU_SBY_CB.MaxAmps = 5.0;
	IMU_SBY_CB.WireTo(&CDRs28VBus);	
	// Set up IMU heater stuff
	imucase.isolation = 1.0; 
	imucase.Area = 3165.31625; // Surface area of 12.5 inch diameter sphere in cm
	imucase.mass = 19050;
	imucase.SetTemp(327); 
	imuheater.WireTo(&IMU_SBY_CB);
	Panelsdk.AddHydraulic(&imucase);
	Panelsdk.AddElectrical(&imuheater,false);
	imuheater.Enable();
	imuheater.SetPumpAuto();

	// Main Propulsion
	PROP_DISP_ENG_OVRD_LOGIC_CB.MaxAmps = 2.0;
	PROP_DISP_ENG_OVRD_LOGIC_CB.WireTo(&LMPs28VBus);
	THRUST_DISP_CB.MaxAmps = 2.0;
	THRUST_DISP_CB.WireTo(&CDRs28VBus);
	MainFuelTempInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainFuelPressInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainOxidizerTempInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	MainOxidizerPressInd.WireTo(&PROP_DISP_ENG_OVRD_LOGIC_CB);
	EngineThrustInd.WireTo(&THRUST_DISP_CB);
	CommandedThrustInd.WireTo(&THRUST_DISP_CB);

	// The FDAI has two CBs, AC and DC, and both are 2 amp CBs
	// CDR FDAI
	CDR_FDAI_DC_CB.MaxAmps = 2.0;
	CDR_FDAI_DC_CB.WireTo(&CDRs28VBus);
	CDR_FDAI_AC_CB.MaxAmps = 2.0;
	CDR_FDAI_AC_CB.WireTo(&ACBusA);
	// And the CDR FDAI itself	
	fdaiLeft.WireTo(&CDR_FDAI_DC_CB,&CDR_FDAI_AC_CB);
	// LMP FDAI stuff
	LMP_EVT_TMR_FDAI_DC_CB.MaxAmps = 2.0;
	LMP_EVT_TMR_FDAI_DC_CB.WireTo(&LMPs28VBus);
	LMP_FDAI_AC_CB.MaxAmps = 2.0;
	LMP_FDAI_AC_CB.WireTo(&ACBusB);
	fdaiRight.WireTo(&LMP_EVT_TMR_FDAI_DC_CB,&LMP_FDAI_AC_CB);
	EventTimerDisplay.WireTo(&LMP_EVT_TMR_FDAI_DC_CB);

	// HEATERS
	HTR_RR_STBY_CB.MaxAmps = 7.5;
	HTR_RR_STBY_CB.WireTo(&CDRs28VBus);
	HTR_LR_CB.MaxAmps = 5.0;
	HTR_LR_CB.WireTo(&CDRs28VBus);
	HTR_DISP_CB.MaxAmps = 2.0;
	HTR_DISP_CB.WireTo(&LMPs28VBus);
	HTR_SBD_ANT_CB.MaxAmps = 5.0;
	HTR_SBD_ANT_CB.WireTo(&LMPs28VBus);
	TempMonitorInd.WireTo(&HTR_DISP_CB);

	// Landing Radar
	LR.Init(this);
	// Rdz Radar
	RDZ_RDR_AC_CB.MaxAmps = 5.0;
	RDZ_RDR_AC_CB.WireTo(&CDRs28VBus);

	PGNS_RNDZ_RDR_CB.MaxAmps = 15.0; // Primary DC power
	PGNS_RNDZ_RDR_CB.WireTo(&CDRs28VBus);

	RDZ_RDR_AC_CB.MaxAmps = 2.0; // Primary AC power
	RDZ_RDR_AC_CB.WireTo(&ACBusA);
	RR.Init(this,&PGNS_RNDZ_RDR_CB,&RDZ_RDR_AC_CB); // This goes to the CB instead.

	RadarTape.Init(this);
	// CWEA
	CWEA.Init(this);

	// COMM
	// S-Band Steerable Ant
	SBandSteerable.Init(this);
	// SBand System
	SBand.Init(this);
	// VHF System
	VHF.Init(this);
	// CBs
	INST_SIG_CONDR_1_CB.MaxAmps = 2.0;
	INST_SIG_CONDR_1_CB.WireTo(&CDRs28VBus);
	INST_SIG_CONDR_2_CB.MaxAmps = 2.0;
	INST_SIG_CONDR_2_CB.WireTo(&LMPs28VBus);
	COMM_VHF_RCVR_A_CB.MaxAmps = 2.0;
	COMM_VHF_RCVR_A_CB.WireTo(&CDRs28VBus);
	COMM_VHF_XMTR_A_CB.MaxAmps = 5.0;
	COMM_VHF_XMTR_A_CB.WireTo(&LMPs28VBus);
	COMM_VHF_RCVR_B_CB.MaxAmps = 2.0;
	COMM_VHF_RCVR_B_CB.WireTo(&LMPs28VBus);
	COMM_VHF_XMTR_B_CB.MaxAmps = 5.0;
	COMM_VHF_XMTR_B_CB.WireTo(&CDRs28VBus);
	COMM_PRIM_SBAND_PA_CB.MaxAmps = 7.5;
	COMM_PRIM_SBAND_PA_CB.WireTo(&LMPs28VBus);
	COMM_PRIM_SBAND_XCVR_CB.MaxAmps = 5.0;
	COMM_PRIM_SBAND_XCVR_CB.WireTo(&LMPs28VBus);
	COMM_SEC_SBAND_PA_CB.MaxAmps = 7.5;
	COMM_SEC_SBAND_PA_CB.WireTo(&CDRs28VBus);
	COMM_SEC_SBAND_XCVR_CB.MaxAmps = 5.0;
	COMM_SEC_SBAND_XCVR_CB.WireTo(&CDRs28VBus);
	COMM_CDR_AUDIO_CB.MaxAmps = 2.0;
	COMM_CDR_AUDIO_CB.WireTo(&CDRs28VBus);
	COMM_SE_AUDIO_CB.MaxAmps = 2.0;
	COMM_SE_AUDIO_CB.WireTo(&LMPs28VBus);
	INST_CWEA_CB.MaxAmps = 2.0;
	INST_CWEA_CB.WireTo(&LMPs28VBus);
	INST_SIG_SENSOR_CB.MaxAmps = 2.0;
	INST_SIG_SENSOR_CB.WireTo(&LMPs28VBus);
	INST_PCMTEA_CB.MaxAmps = 2.0;
	INST_PCMTEA_CB.WireTo(&LMPs28VBus);
	COMM_PMP_CB.MaxAmps = 2.0;
	COMM_PMP_CB.WireTo(&LMPs28VBus);
	COMM_SBAND_ANT_CB.MaxAmps = 2.0;
	COMM_SBAND_ANT_CB.WireTo(&LMPs28VBus);
	COMM_DISP_CB.MaxAmps = 2.0;
	COMM_DISP_CB.WireTo(&LMPs28VBus);

	// EXPLOSIVE DEVICES SUPPLY CBs
	EDS_CB_LG_FLAG.MaxAmps = 2.0;
	EDS_CB_LG_FLAG.WireTo(&CDRs28VBus);
	EDS_CB_LOGIC_A.MaxAmps = 2.0;
	EDS_CB_LOGIC_A.WireTo(&CDRs28VBus);
	EDS_CB_LOGIC_B.MaxAmps = 2.0;
	EDS_CB_LOGIC_B.WireTo(&LMPs28VBus);

	// EXPLOSIVE DEVICES SYSTEMS
	EDLGTB.WireTo(&EDS_CB_LG_FLAG);

	// ABORT GUIDANCE SYSTEM
	SCS_ASA_CB.MaxAmps = 20.0;
	SCS_ASA_CB.WireTo(&LMPs28VBus);
	SCS_AEA_CB.MaxAmps = 10.0;
	SCS_AEA_CB.WireTo(&LMPs28VBus);
	CDR_SCS_AEA_CB.MaxAmps = 10.0;
	CDR_SCS_AEA_CB.WireTo(&CDRs28VBus);
	SCS_ATCA_AGS_CB.MaxAmps = 3.0;
	SCS_ATCA_AGS_CB.WireTo(&LMPs28VBus);

	// ENVIRONMENTAL CONTROL SYSTEM
	ECS_DISP_CB.MaxAmps = 2.0;
	ECS_DISP_CB.WireTo(&LMPs28VBus);
	LMSuitTempMeter.WireTo(&ECS_DISP_CB);
	LMCabinTempMeter.WireTo(&ECS_DISP_CB);
	LMSuitPressMeter.WireTo(&ECS_DISP_CB);
	LMCabinPressMeter.WireTo(&ECS_DISP_CB);
	LMCabinCO2Meter.WireTo(&ECS_DISP_CB);
	LMGlycolTempMeter.WireTo(&ECS_DISP_CB);
	LMGlycolPressMeter.WireTo(&ECS_DISP_CB);
	LMOxygenQtyMeter.WireTo(&ECS_DISP_CB);
	LMWaterQtyMeter.WireTo(&ECS_DISP_CB);
	ECS_CABIN_REPRESS_CB.MaxAmps = 2.0;
	ECS_CABIN_REPRESS_CB.WireTo(&LMPs28VBus);
	ECS_GLYCOL_PUMP_2_CB.MaxAmps = 5.0;
	ECS_GLYCOL_PUMP_2_CB.WireTo(&CDRs28VBus);

	// Mission timer.
	MISSION_TIMER_CB.MaxAmps = 2.0;
	MISSION_TIMER_CB.WireTo(&CDRs28VBus);
	MissionTimerDisplay.WireTo(&MISSION_TIMER_CB);

	// Arrange for updates of main busses, AC inverters, and the bus balancer
	Panelsdk.AddElectrical(&ACBusA, false);
	Panelsdk.AddElectrical(&ACBusB, false);
	Panelsdk.AddElectrical(&ACVoltsAttenuator, false);
	Panelsdk.AddElectrical(&INV_1, false);
	Panelsdk.AddElectrical(&INV_2, false);
	// The multiplexer will update the main 28V busses
	Panelsdk.AddElectrical(&BTC_MPX,false);
	
	// Arrange for updates of tie points and bus balancer
	Panelsdk.AddElectrical(&BTB_LMP_E, false); // Sum of BTB-A and bus cross-tie-balancer
	Panelsdk.AddElectrical(&BTB_LMP_A, false); // Sum of BTB-D and XLUNAR power
	Panelsdk.AddElectrical(&BTB_LMP_D, false); // Sum of battery feed ties
	Panelsdk.AddElectrical(&BTB_LMP_B, false); // Sum of ascent and descent feed lines
	Panelsdk.AddElectrical(&BTB_LMP_C, false); // Sum of ascent and descent feed lines
	Panelsdk.AddElectrical(&BTB_CDR_E, false);
	Panelsdk.AddElectrical(&BTB_CDR_A, false);
	Panelsdk.AddElectrical(&BTB_CDR_D, false);
	Panelsdk.AddElectrical(&BTB_CDR_B, false);
	Panelsdk.AddElectrical(&BTB_CDR_C, false);

	// XLUNAR source
	Panelsdk.AddElectrical(&BTC_XLunar, false);

	// Update ECA ties
	Panelsdk.AddElectrical(&DES_CDRs28VBusA, false);
	Panelsdk.AddElectrical(&DES_CDRs28VBusB, false);
	Panelsdk.AddElectrical(&DES_LMPs28VBusA, false); 
	Panelsdk.AddElectrical(&DES_LMPs28VBusB, false); 

	// Arrange for updates of ECAs
	Panelsdk.AddElectrical(&ECA_1a, false);
	Panelsdk.AddElectrical(&ECA_1b, false);
	Panelsdk.AddElectrical(&ECA_2a, false);
	Panelsdk.AddElectrical(&ECA_2b, false);
	Panelsdk.AddElectrical(&ECA_3a, false);
	Panelsdk.AddElectrical(&ECA_3b, false);
	Panelsdk.AddElectrical(&ECA_4a, false);
	Panelsdk.AddElectrical(&ECA_4b, false);

	// ECS
	ecs.Init(this);

	// EDS initialization
	eds.Init(this);

	// DPS and APS
	DPS.Init(this);
	APS.Init(this);

	// DS20060413 Initialize joystick
	js_enabled = 0;  // Disabled
	rhc_id = -1;     // Disabled
	rhc_rot_id = -1; // Disabled
	rhc_sld_id = -1; // Disabled
	rhc_rzx_id = -1; // Disabled
	thc_id = -1;     // Disabled
	thc_rot_id = -1; // Disabled
	thc_sld_id = -1; // Disabled
	thc_rzx_id = -1; // Disabled
	thc_tjt_id = -1; // Disabled
	thc_debug = -1;
	rhc_debug = -1;
	
	// Initialize other systems
	atca.Init(this);
}

void LEM::SystemsTimestep(double simt, double simdt) 

{
	// Clear debug line when timer runs out
	if(DebugLineClearTimer > 0){
		DebugLineClearTimer -= simdt;
		if(DebugLineClearTimer < 0){
			sprintf(oapiDebugString(),"");
			DebugLineClearTimer = 0;
		}
	}

	// Zero ACA and TTCA bits in channel 31
	LMChannelValue31 val31;
	val31.Value = agc.GetInputChannel(031);
	val31.Value &= 030000; // Leaves AttitudeHold and AutomaticStab alone

	// Joystick read
	if(js_enabled > 0 && oapiGetFocusInterface() == this){		
		if(thc_id != -1 && !(thc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as THC does not exist.");
		}
		if(rhc_id != -1 && !(rhc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as RHC does not exist.");
		}
		/* ACA OPERATION:

			The LM ACA is a lot different from the CM RHC.
			The ACA works on a D/A converter.
			The OUT OF DETENT switch closes at .5 degrees of travel, and enables the proportional
			voltage circuit ("A" CIRCUIT) to operate.
			The hand controller must be moved 2 degrees to generate a count.
			8 degrees of usable travel, at .190 degrees per count.			
			10 degrees total travel = 42 counts.

		*/
		// Axes have 32768 points of travel for the 13 degrees to hard stop
		// 2520 points per degree. It breaks out of detent at .5 degres, or 1260 pulses.
		// 480 points per count.
		rhc_pos[0] = 0; // Initialize
		rhc_pos[1] = 0;
		rhc_pos[2] = 0;

		// Read data
		HRESULT hr;
		// Handle RHC
		if(rhc_id != -1 && dx8_joystick[rhc_id] != NULL){
			// CHECK FOR POWER HERE
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
			dx8_joystick[rhc_id]->GetDeviceState(sizeof(dx8_jstate[rhc_id]),&dx8_jstate[rhc_id]);
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
			if(rhc_rzx_id != -1 && rhc_rot_id == -1){ // If we use the native Z-axis
				rhc_rot_pos = dx8_jstate[rhc_id].lZ;
			}	
			if(dx8_jstate[rhc_id].lX > 34028){ // Out of detent RIGHT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.PlusAzimuth = 1;
				rhc_pos[0] = dx8_jstate[rhc_id].lX-34028; // Results are 0 - 31507
			}
			if(dx8_jstate[rhc_id].lX < 31508){ // Out of detent LEFT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.MinusAzimuth = 1;
				rhc_pos[0] = dx8_jstate[rhc_id].lX-31508; // Results are 0 - -31508
			}
			if(dx8_jstate[rhc_id].lY > 34028){ // Out of detent UP
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.PlusElevation = 1;
				rhc_pos[1] = dx8_jstate[rhc_id].lY-34028; // Results are 0 - 31507
			}
			if(dx8_jstate[rhc_id].lY < 31508){ // Out of detent DOWN
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.MinusElevation = 1;
				rhc_pos[1] = dx8_jstate[rhc_id].lY-31508; // Results are 0 - -31508
			}
			// YAW IS REVERSED
			if(rhc_rot_pos > 34028){ // Out of detent RIGHT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.PlusYaw = 1;
				rhc_pos[2] = 34028-rhc_rot_pos; // Results are 0 - 31507
			}
			if(rhc_rot_pos < 31508){ // Out of detent LEFT
				val31.Bits.ACAOutOfDetent = 1;
				val31.Bits.MinusYaw = 1;
				rhc_pos[2] = 31508-rhc_rot_pos; // Results are 0 - -31508
			}
		}else{
			// No JS
		}
		// sprintf(oapiDebugString(),"RHC: X/Y/Z = %d / %d / %d | rzx_id %d rot_id %d", rhc_pos[0],rhc_pos[1],rhc_pos[2], rhc_rzx_id, rhc_rot_id); 
		// And now the THC...
		if(thc_id != -1 && thc_id < js_enabled){
			// CHECK FOR POWER HERE
			int thc_voltage = 5; // HAX
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
			// The LM TTCA is even wierder than the CM THC...			
			int thc_rot_pos = 32768,thc_tjt_pos=32768; // Initialize to centered			
			if(thc_voltage > 0){
				if(thc_tjt_id != -1){                    // If Throttle/Jets lever enabled
					thc_tjt_pos = dx8_jstate[thc_id].lZ; // Read
				}
				if(thc_tjt_pos < 10000){				 // Determine TTCA mode	
					ttca_mode = TTCA_MODE_THROTTLE;      // THROTTLE MODE					
					ttca_throttle_pos = dx8_jstate[thc_id].lY; // Relay throttle position
				}else{
					ttca_mode = TTCA_MODE_JETS;          // JETS MODE
					ttca_throttle_pos = 32768;           // Center of axis (just in case)
					if(dx8_jstate[thc_id].lY < 16384){
						val31.Bits.PlusX = 1;
					}
					if(dx8_jstate[thc_id].lY > 49152){						
						val31.Bits.MinusX = 1;
					}
				}
				if(dx8_jstate[thc_id].lX > 49152){
					val31.Bits.PlusY = 1;
				}
				if(dx8_jstate[thc_id].lX < 16384){												
					val31.Bits.MinusY = 1;
				}				
				// Z-axis read.
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
				if(thc_rzx_id != -1 && thc_rot_id == -1){ // If we use the native Z-axis
					thc_rot_pos = dx8_jstate[thc_id].lZ;
				}
				if(thc_rot_pos < 16384){
					val31.Bits.MinusZ = 1;
				}
				if(thc_rot_pos > 49152){
					val31.Bits.PlusZ = 1;
				}
				if(thc_debug != -1){ sprintf(oapiDebugString(),"THC: X/Y/Z = %d / %d / %d TJT = %d",dx8_jstate[thc_id].lX,dx8_jstate[thc_id].lY,
					thc_rot_pos,thc_tjt_pos); }
			}else{
				// No JS
			}
		}
	}
	// Write back channel data
	agc.SetInputChannel(031,val31.Value);

	// Each timestep is passed to the SPSDK
	// to perform internal computations on the 
	// systems.
	Panelsdk.Timestep(simt);

	// After that come all other systems simesteps	
	agc.Timestep(MissionTime, simdt);						// Do work
	agc.SystemTimestep(simdt);								// Draw power
	dsky.Timestep(MissionTime);								// Do work
	dsky.SystemTimestep(simdt);								// This can draw power now.
	asa.TimeStep(simdt);									// Do work
	aea.TimeStep(simdt);
	deda.TimeStep(simdt);
	imu.Timestep(MissionTime);								// Do work
	imu.SystemTimestep(simdt);								// Draw power
	// Manage IMU standby heater and temperature
	if(IMU_OPR_CB.Voltage() > 0){
		// IMU is operating.
		if(imuheater.h_pump != 0){ imuheater.SetPumpOff(); } // Disable standby heater if enabled
		// FIXME: IMU Enabled-Mode Heat Generation Goes Here
	}else{
		// IMU is not operating.
		if(imuheater.h_pump != 1){ imuheater.SetPumpAuto(); } // Enable standby heater if disabled.
	}
	// FIXME: Maintenance of IMU temperature channel bit should go here when ECS is complete

	// FIXME: Draw power for lighting system.
	// I can't find the actual power draw anywhere.

	// Allow ATCA to operate between the FDAI and AGC/AEA so that any changes the FDAI makes
	// can be shown on the FDAI, but any changes the AGC/AEA make are visible to the ATCA.
	atca.Timestep(simt);								    // Do Work
	fdaiLeft.Timestep(MissionTime, simdt);					// Do Work
	fdaiRight.Timestep(MissionTime, simdt);
	fdaiLeft.SystemTimestep(simdt);							// Draw Power
	fdaiRight.SystemTimestep(simdt);
	MissionTimerDisplay.Timestep(MissionTime, simdt);       // These just do work
	EventTimerDisplay.Timestep(MissionTime, simdt);
	eds.TimeStep();                                         // Do Work
	optics.TimeStep(simdt);									// Do Work
	LR.TimeStep(simdt);										// I don't wanna work
	RR.TimeStep(simdt);										// I just wanna bang on me drum all day
	RadarTape.TimeStep(MissionTime);										// I just wanna bang on me drum all day
	SBandSteerable.TimeStep(simdt);							// Back to work...
	VHF.SystemTimestep(simdt);
	VHF.TimeStep(simt);
	SBand.SystemTimestep(simdt);
	SBand.TimeStep(simt);
	ecs.TimeStep(simdt);
	DPS.TimeStep(simdt);
	APS.TimeStep(simdt);
	// Do this toward the end so we can see current system state
	CWEA.TimeStep(simdt);

	// Debug tests would go here
	
	/*
	double CDRAmps=0,LMPAmps=0;
	double CDRVolts = CDRs28VBus.Voltage(),LMPVolts = LMPs28VBus.Voltage();
	if(LMPVolts > 0){ LMPAmps = LMPs28VBus.PowerLoad()/LMPVolts; }
	if(CDRVolts > 0){ CDRAmps = CDRs28VBus.PowerLoad()/CDRVolts; }	
	sprintf(oapiDebugString(),"LM: LMP %f V/%f A CDR %f V/%f A | AC-A %f V AC-B %f V",LMPVolts,LMPAmps,
		CDRVolts,CDRAmps,ACBusA.Voltage(), ACBusB.Voltage());
	*/	
}

// PANEL SDK SUPPORT
void LEM::SetValveState(int valve, bool open)

{
	ValveState[valve] = open;

	int valve_state = open ? SP_VALVE_OPEN : SP_VALVE_CLOSE;

	if (pLEMValves[valve])
		*pLEMValves[valve] = valve_state;

	/*
	CheckRCSState();
	*/
}

bool LEM::GetValveState(int valve)

{
	//
	// First check whether the valve still exists!
	//

	/*
	if (valve < CM_VALVES_START) {
		if (stage > CSM_LEM_STAGE)
			return false;
	}
	*/

	if (pLEMValves[valve])
		return (*pLEMValves[valve] == SP_VALVE_OPEN);

	return ValveState[valve];
}

// SYSTEMS COMPONENTS
// UMBILICAL
LEMPowerConnector::LEMPowerConnector(){
	type = NO_CONNECTION;
	connectedTo = 0;
	csm_power_latch = 0;
}

bool LEMPowerConnector::ReceiveMessage(Connector *from, ConnectorMessage &m){
	// This should only get messages of type 42 from the CM telling it to switch relay states
	// on our side
	if(from != this && m.messageType == 42){
		// Relay Event
		// When connected, the CSM feeds the LM via two 7.5A umbilicals. Both feed the same stuff, they are redundant.
		// The CSM power comes in via the CDRs DC bus and returns to the CSM via the CDR and LMP XLUNAR busses.
		// That means in order to have CSM power, you need to have either:
		//   A: CDR XLUNAR CB closed
		//   B: LMP XLUNAR CB closed with the bus cross-tie CBs closed.
		// When CSM power is commanded on, it turns off the descent ECAs and prevents them from being turned back on.
		// Ascent power is not affected.
		// The ECA design makes it impossible to charge the LM batteries from the CSM power supply, since it prevents current from flowing backwards.
		switch(m.val1.iValue){
			case 0: // Disconnect				
				csm_power_latch = -1;
				// sprintf(oapiDebugString(),"LM/CSM Conn: Latch Reset");
				break;
			case 1: // Connect
				csm_power_latch = 1;
				// sprintf(oapiDebugString(),"LM/CSM Conn: Latch Set");
				break;
			default:
				sprintf(oapiDebugString(),"LM/CSM Conn: Relay Event: Bad parameter %d",m.val1.iValue);
				return false;
		}
		return true;
	}
	// Debug: Complain if we got garbage
	sprintf(oapiDebugString(),"LM/CSM Conn: Bad message: Type %d parameter %d",m.messageType,m.val1.iValue);
	return false;
}

// ELECTRICAL CONTROL ASSEMBLY SUBCHANNEL
LEM_ECAch::LEM_ECAch(){
	lem = NULL;
	dc_source_tb = NULL;
	input = -1; // Flag uninit
}

void LEM_ECAch::Init(LEM *s,e_object *src,int inp){
	lem = s;
	if(input == -1){ input = inp; }
	dc_source = src;
	Volts = 24;
}

void LEM_ECAch::SaveState(FILEHANDLE scn, char *start_str, char *end_str)

{
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "INPUT", input);
	oapiWriteLine(scn, end_str);
}

void LEM_ECAch::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "INPUT", 5)) {
			sscanf(line + 6, "%d", &dec);
			input = dec;
		}
	}
}

void LEM_ECAch::DrawPower(double watts){ 
	power_load += watts;
};

void LEM_ECAch::UpdateFlow(double dt){
	// ECA INPUTS CAN BE PARALLELED, BUT NOT IN THE SAME CHANNEL
	// That is, Battery 1 and 2 can be on at the same time.
	// Draw power from the source, and retake voltage, etc.

	// Take power
	switch(input){
		case 1: // HI tap
			if(dc_source != NULL){
				dc_source->DrawPower(power_load); // Draw 1:1
			}
			break;
		case 2: // LO tap
			if(dc_source != NULL){
				dc_source->DrawPower(power_load*1.06); // Draw 6% more
			}
			break;
	}
	
	// Resupply from source
	switch(input){
		case 0: // NULL
			Volts = 0;
			Amperes = 0;
			break;
		case 1: // HV
			if(dc_source != NULL){
				Volts =   dc_source->Voltage();
				Amperes = dc_source->Current();
			}
			break;
		case 2: // LV
			if(dc_source != NULL){
				Volts =   (dc_source->Voltage()*0.93);
				Amperes = dc_source->Current();
			}
			break;
	}

	// Reset for next pass.
	e_object::UpdateFlow(dt);	
}

// BUS TIE BLOCK

LEM_BusFeed::LEM_BusFeed(){
	lem = NULL;
	dc_source_a = NULL;
	dc_source_b = NULL;
}

void LEM_BusFeed::Init(LEM *s,e_object *sra,e_object *srb){
	lem = s;
	dc_source_a = sra;
	dc_source_b = srb;
	Volts = 0;
}

void LEM_BusFeed::DrawPower(double watts)
{ 
	power_load += watts;
};

void LEM_BusFeed::UpdateFlow(double dt){
	//sprintf(oapiDebugString(),"BTO Input = %d Voltage %f Load %f",input,Volts,power_load);
	// Draw power from the source, and retake voltage, etc.

	int csrc=0;                             // Current Sources Operational
	double PowerDrawPerSource;              // Current to draw, per source
	double power_load_src=power_load;		// Power load when we came in
	int cba_ok=0,cbb_ok=0;					// Circuit breaker OK flags
	
	// Find active sources
	if(dc_source_a != NULL && dc_source_a->Voltage() > 0){
		csrc++;
	}
	if(dc_source_b != NULL && dc_source_b->Voltage() > 0){
		csrc++;
	}
	// Compute draw
	if(csrc > 1){
		PowerDrawPerSource = power_load_src/2;
	}else{
		PowerDrawPerSource = power_load_src;
	}

	// Now take power
	if(dc_source_a != NULL){
		dc_source_a->DrawPower(PowerDrawPerSource); 
	}
	if(dc_source_b != NULL){
		dc_source_b->DrawPower(PowerDrawPerSource); 
	}
	
	double A_Volts = 0;
	double A_Amperes = 0;
	double B_Volts = 0;
	double B_Amperes = 0;

	// Resupply from source
	if(dc_source_a != NULL){
		A_Volts =   dc_source_a->Voltage();
		A_Amperes = dc_source_a->Current();
	}
	if(dc_source_b != NULL){
		B_Volts = dc_source_b->Voltage();
		B_Amperes = dc_source_b->Current();
	}
	// Final output
	switch(csrc){
		case 2: // DUAL
			Volts = (A_Volts + B_Volts) / 2;
			Amperes = A_Amperes+B_Amperes;
			break;
		case 1: // SINGLE
			if(A_Volts > 0){ // Only one (or no) input
				Volts = A_Volts;
				Amperes = A_Amperes;
			}else{
				Volts = B_Volts;
				Amperes = B_Amperes;
			}
			break;
		default: // OFF OR OTHER
			Volts = 0;
			Amperes = 0;
			break;
	}

	// if(this == &lem->BTB_CDR_D){ sprintf(oapiDebugString(),"LM_BTO: = Voltages %f %f | Load %f PS %f Output %f V",A_Volts,B_Volts,power_load,PowerDrawPerSource,Volts); }

	// Reset for next pass.
	power_load -= power_load_src;	
}

// XLUNAR BUS MANAGER OUTPUT SOURCE
LEM_XLBSource::LEM_XLBSource(){
	Volts = 0;
	enabled = true;
}

void LEM_XLBSource::SetVoltage(double v){
	Volts = v;
}

void LEM_XLBSource::DrawPower(double watts)
{ 
	power_load += watts;
};

// XLUNAR BUS MANAGER
LEM_XLBControl::LEM_XLBControl(){
	lem = NULL;
}

void LEM_XLBControl::Init(LEM *s){
	lem = s;
	dc_output.SetVoltage(0);
}

// Depreciated - Don't tie directly
void LEM_XLBControl::DrawPower(double watts)
{ 
	power_load += watts;
};

void LEM_XLBControl::UpdateFlow(double dt){
	// If we have no LEM, punt
	if(lem == NULL){ return; }
	// Do we have power from the other side?
	double sVoltage = lem->CSMToLEMPowerSource.Voltage();	
	// Is the CDR XLunar tie closed?
	if(lem->CDRXLunarBusTieCB.GetState() == 1){
		// Yes, we can put voltage on the CDR bus
		dc_output.SetVoltage(sVoltage);
	}else{
		// No -- Are we tied to the LMP bus, and is the XLunar tie closed on the LMP side?
		if(((lem->CDRCrossTieBalCB.GetState() == 1 || lem->CDRCrossTieBusCB.GetState() == 1) &&
			(lem->LMPCrossTieBalCB.GetState() == 1 || lem->LMPCrossTieBusCB.GetState() == 1)) &&
			lem->LMPXLunarBusTieCB.GetState() == 1){
			// Yes, we can put voltage on the CDR bus and it get there.
			dc_output.SetVoltage(sVoltage);
		}else{
			// No, we have no return path, so we have no voltage.
			dc_output.SetVoltage(0);
		}
	}
	// Handle switchery
	switch(lem->CSMToLEMPowerConnector.csm_power_latch){
		case 1:
			// If the CSM latch is set, keep the descent ECAs off
			lem->ECA_1a.input = 0; lem->ECA_1b.input = 0;
			lem->ECA_2a.input = 0; lem->ECA_2b.input = 0;
			break;
		case -1:
			// If the CSM latch is reset, turn on the LV taps on batteries 1 and 4.
			// And reset the latch to zero
			lem->ECA_1a.input = 2; lem->ECA_1b.input = 0;
			lem->ECA_2a.input = 0; lem->ECA_2b.input = 2;
			lem->CSMToLEMPowerConnector.csm_power_latch = 0;
			break;
	}	
	// So then, do we have xlunar voltage?
	if(dc_output.Voltage() > 0){
		// Process load at our feed point
		lem->CSMToLEMPowerSource.DrawPower(dc_output.PowerLoad());
		// sprintf(oapiDebugString(),"Drawing %f watts from CSM",dc_output.PowerLoad());
		dc_output.UpdateFlow(dt); // Shouldn't touch voltage since it has no SRC
	}
	
};

// CROSS-TIE BALANCER OUTPUT SOURCE
LEM_BCTSource::LEM_BCTSource(){
	Volts = 0;
}

void LEM_BCTSource::SetVoltage(double v){
	Volts = v;
}

// BUS CROSS-TIE BALANCER
LEM_BusCrossTie::LEM_BusCrossTie(){
	lem = NULL;
	dc_bus_lmp = NULL;
	dc_bus_cdr = NULL;
	lmp_bal_cb = NULL;	lmp_bus_cb = NULL;
	cdr_bal_cb = NULL;	cdr_bus_cb = NULL;
	last_cdr_ld = 0;
	last_lmp_ld = 0;
}

void LEM_BusCrossTie::Init(LEM *s,DCbus *sra,DCbus *srb,CircuitBrakerSwitch *cb1,CircuitBrakerSwitch *cb2,CircuitBrakerSwitch *cb3,CircuitBrakerSwitch *cb4){
	lem = s;
	dc_bus_lmp = sra;
	dc_bus_cdr = srb;
	lmp_bal_cb = cb1;	lmp_bus_cb = cb2;
	cdr_bal_cb = cb3;	cdr_bus_cb = cb4;
	dc_output_lmp.SetVoltage(0);
	dc_output_cdr.SetVoltage(0);	
	last_cdr_ld = 0;
	last_lmp_ld = 0;
}

// Depreciated - Don't tie directly
void LEM_BusCrossTie::DrawPower(double watts)
{ 
	power_load += watts;
};

void LEM_BusCrossTie::UpdateFlow(double dt){
	// Voltage, load, load-share-difference
	double cdr_v,cdr_l,cdr_ld;
	double lmp_v,lmp_l,lmp_ld;
	double loadshare;

	lmp_v = lem->BTB_LMP_A.Voltage(); // Measure bus voltages at their A tie point, so we don't get our own output 
	cdr_v = lem->BTB_CDR_A.Voltage(); 
	lmp_l = dc_bus_lmp->PowerLoad();
	cdr_l = dc_bus_cdr->PowerLoad();

	// If both busses are dead or both CBs on either side are out, the output is dead.
	if((cdr_v == 0 && lmp_v == 0) ||
		(lmp_bus_cb->GetState() == 0 && lmp_bal_cb->GetState() == 0) || 
		(cdr_bus_cb->GetState() == 0 && cdr_bal_cb->GetState() == 0)){ 
		dc_output_lmp.SetVoltage(0);
		dc_output_cdr.SetVoltage(0);
		lem->CDRs28VBus.UpdateFlow(dt);
		lem->LMPs28VBus.UpdateFlow(dt);
		return;
	}

	// Compute load-share and differences.
	if(lmp_v == 0 || cdr_v == 0){
		// We lost power on one or both busses. Reset the stored load split.
		last_cdr_ld = 0;
		last_lmp_ld = 0;
		// If one bus is powered, but the other is not,
		// we feed the dead bus from the live one.
		lem->CDRs28VBus.UpdateFlow(dt);
		lem->LMPs28VBus.UpdateFlow(dt);
		if(cdr_v == 0){
			// Draw CDR load from LMP side and equalize voltage
			dc_output_cdr.SetVoltage(lmp_v);
			dc_output_lmp.SetVoltage(0);
			dc_bus_lmp->DrawPower(cdr_l);
			double Draw = cdr_l / lmp_v;
			if(lmp_bus_cb->GetState() > 0){			
				if(Draw > 100){
					lmp_bus_cb->SetState(0);
				}
				if(lmp_bal_cb->GetState() > 0 && Draw > 60){
					lmp_bal_cb->SetState(0);
				}
			}else{
				if(lmp_bal_cb->GetState() > 0 && Draw > 30){
					lmp_bal_cb->SetState(0);
				}
			}
		}else{
			// Draw LMP load from CDR side and equalize voltage
			dc_output_lmp.SetVoltage(cdr_v);
			dc_output_cdr.SetVoltage(0);
			dc_bus_cdr->DrawPower(lmp_l);
			double Draw = lmp_l / cdr_v;
			if(cdr_bus_cb->GetState() > 0){			
				if(Draw > 100){
					cdr_bus_cb->SetState(0);
				}
				if(cdr_bal_cb->GetState() > 0 && Draw > 60){
					cdr_bal_cb->SetState(0);
				}
			}else{
				if(cdr_bal_cb->GetState() > 0 && Draw > 30){
					cdr_bal_cb->SetState(0);
				}
			}
		}
		return;
	}else{
		// If both sides are powered, then one side is going to have a higher load
		// than the other. We draw power from the low-load side to feed the high-load side.
		// The higher-load side will probably have the lower voltage.
		loadshare = (lmp_l+cdr_l)/2;
		cdr_ld = loadshare - cdr_l;
		lmp_ld = loadshare - lmp_l;			
	}

	// Are we within tolerance already?
	if((cdr_ld < 0.000001 && cdr_ld > -0.000001) && (lmp_ld < 0.000001 && lmp_ld > -0.000001)){
		// In this case, the busses are already balanced.
		// Use whatever numbers we used last time.
		cdr_ld = last_cdr_ld;
		lmp_ld = last_lmp_ld;
		// sprintf(oapiDebugString(),"BCT L: LMP/CDR V %f %f L %f %f | LS %f | DF %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);
	}else{
		// Include what we did before
		cdr_ld += last_cdr_ld;
		lmp_ld += last_lmp_ld;
		// Save this for later abuse
		last_cdr_ld = cdr_ld;
		last_lmp_ld = lmp_ld;
		// sprintf(oapiDebugString(),"BCT N: LMP/CDR V %f %f L %f %f | LS %f | DF %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);
	}

	// If this works the load on both sides should be equal, with each bus having half the total load.
	// sprintf(oapiDebugString(),"BCT: LMP/CDR V %f %f L %f %f | LS %f | D %f %f",lmp_v,cdr_v,lmp_l,cdr_l,loadshare,lmp_ld,cdr_ld);

	lem->CDRs28VBus.UpdateFlow(dt);
	lem->LMPs28VBus.UpdateFlow(dt);

	// Transfer power from the higher-voltage side

	// Balance voltage
	// dc_output_cdr.SetVoltage((cdr_v+lmp_v)/2);	
	// dc_output_lmp.SetVoltage((cdr_v+lmp_v)/2);		

	// Transfer load (works both ways)
	dc_bus_cdr->DrawPower(cdr_ld); 
	dc_bus_lmp->DrawPower(lmp_ld);
	// Last thing we do is blow CBs on overcurrent.
	// BUS TIE blows at 100 amps, BUS BAL blows at 30 amps, or 60 amps if the TIE breaker is also closed.
	if(cdr_ld > 0){
		double Draw = cdr_ld / cdr_v;
		if(cdr_bus_cb->GetState() > 0){			
			if(Draw > 100){
				cdr_bus_cb->SetState(0);
			}
			if(cdr_bal_cb->GetState() > 0 && Draw > 60){
				cdr_bal_cb->SetState(0);
			}
		}else{
			if(cdr_bal_cb->GetState() > 0 && Draw > 30){
				cdr_bal_cb->SetState(0);
			}		
		}
	}
	if(lmp_ld > 0){
		double Draw = lmp_ld / lmp_v;
		if(lmp_bus_cb->GetState() > 0){			
			if(Draw > 100){
				lmp_bus_cb->SetState(0);
			}
			if(lmp_bal_cb->GetState() > 0 && Draw > 60){
				lmp_bal_cb->SetState(0);
			}
		}else{
			if(lmp_bal_cb->GetState() > 0 && Draw > 30){
				lmp_bal_cb->SetState(0);
			}
		}
	}
}


// AC INVERTER

LEM_INV::LEM_INV(){
	lem = NULL;
	active = 0;
	dc_input = NULL;
}

void LEM_INV::Init(LEM *s){
	lem = s;
}

void LEM_INV::DrawPower(double watts)

{ 
	power_load += watts;
};

void LEM_INV::UpdateFlow(double dt){

	// Reset these before pass
	Volts = 0;
	Amperes = 0;

	// If not active, die.
	if(!active){ return; }

	if(dc_input != NULL){
		// First take power from source
		dc_input->DrawPower(power_load*2.5);  // Add inefficiency
		// Then supply the bus
		if(dc_input->Voltage() > 24){		  // Above 24V input
			Volts = 115.0;                    // Regulator supplies 115V
		}else{                                // Otherwise
			Volts = dc_input->Voltage()*4.8;  // Falls out of regulation
		}                                     // until the load trips the CB
		Amperes = power_load/Volts;           // AC load amps
	}

	// Debug
	/*
	if(dc_input->Voltage() > 0){
		sprintf(oapiDebugString(),"INV: DC V = %f A = %f | AC LOAD = %f V = %f A = %f",
			dc_input->Voltage(),(power_load/dc_input->Voltage()*2.5),power_load,Volts,Amperes);
	}else{
		sprintf(oapiDebugString(),"INV: INPUT V = %f LOAD = %f",dc_input->Voltage(),power_load);
	}
	*/
	// Reset for next pass
	e_object::UpdateFlow(dt);
}

// EXPLOSIVE DEVICES SYSTEM
LEM_EDS::LEM_EDS(){
	lem = NULL;
	LG_Deployed = FALSE;
}

void LEM_EDS::Init(LEM *s){
	lem = s;
}

void LEM_EDS::TimeStep(){
	// Set TBs
	// BP when descent stage detached
	if(LG_Deployed == TRUE && lem->status < 2){ lem->EDLGTB.SetState(1); }else{	lem->EDLGTB.SetState(0); }
	// Do we have power?
	if(lem->EDS_CB_LOGIC_A.Voltage() < 20 && lem->EDS_CB_LOGIC_B.Voltage() < 20){ return; }
	// Are we enabled?
	if(lem->EDMasterArm.GetState() != TOGGLESWITCH_UP){ return; }
	// PROCESS THESE IN THIS ORDER:
	// Landing Gear Deployment
	if(LG_Deployed == FALSE && lem->status == 0){
		// Check?
		if(lem->EDLGDeploy.GetState() == TOGGLESWITCH_UP){
			// Deploy landing gear
			lem->SetLmVesselHoverStage();
			LG_Deployed = TRUE;
		}
	}
	// RCS propellant pressurization
	// Descent Propellant Tank Prepressurization (ambient helium)
	// Descent Propellant Tank Prepressurization (supercritical helium)
	// Descent Propellant Tank Venting
	// Ascent Propellant Tank Pressurization
	// Interstage nut-and-bolt separation and ascent stage deadfacing
	if(lem->status < 2){
		if(lem->EDStage.GetState() == TOGGLESWITCH_UP){
			// Disconnect EPS stuff
			lem->DES_LMPs28VBusA.Disconnect();
			lem->DES_LMPs28VBusB.Disconnect();
			lem->DES_CDRs28VBusA.Disconnect();
			lem->DES_CDRs28VBusB.Disconnect();
			// Disconnect monitor select rotaries
			lem->EPSMonitorSelectRotary.SetSource(1, NULL);
			lem->EPSMonitorSelectRotary.SetSource(2, NULL);
			lem->EPSMonitorSelectRotary.SetSource(3, NULL);
			lem->EPSMonitorSelectRotary.SetSource(4, NULL);
			// Change descent TB
			lem->DSCBattFeedTB.SetState(0);
			// Stage
			lem->SeparateStage(1);
		}
	}
	// Interstage umbilical severance
}

void LEM_EDS::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "LG_DEP", LG_Deployed);
	oapiWriteScenario_int(scn, "SOV_A", lem->GetValveState(LEM_RCS_MAIN_SOV_A));
	oapiWriteScenario_int(scn, "SOV_B", lem->GetValveState(LEM_RCS_MAIN_SOV_B));
	oapiWriteLine(scn, end_str);
}

void LEM_EDS::LoadState(FILEHANDLE scn,char *end_str){
	char *line;
	int dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "LG_DEP", 6)) {
			sscanf(line + 6, "%d", &dec);
			LG_Deployed = (bool)(dec != 0);
		}
		if (!strnicmp (line, "SOV_A", 5)) {
			sscanf(line + 6, "%d", &dec);
			lem->SetValveState(LEM_RCS_MAIN_SOV_A,(bool)(dec != 0));			
		}
		if (!strnicmp (line, "SOV_B", 5)) {
			sscanf(line + 6, "%d", &dec);
			lem->SetValveState(LEM_RCS_MAIN_SOV_B,(bool)(dec != 0));			
		}
	}
}

// Landing Radar
LEM_LR::LEM_LR() : antenna("LEM-LR-Antenna",_vector3(0.013, -3.0, -0.03),0.03,0.04),
	antheater("LEM-LR-Antenna-Heater",1,NULL,35,55,0,285.9,294.2,&antenna)
{
	lem = NULL;
	lastTemp = 0;
}

void LEM_LR::Init(LEM *s){
	lem = s;
	// Set up antenna.
	// LR antenna is designed to operate between 0F and 185F
	// The heater switches on if the temperature gets below +55F and turns it off again when the temperature reaches +70F
	// Values in the constructor are name, pos, vol, isol
	antenna.isolation = 1.0; 
	antenna.Area = 1250; // 1250 cm
	antenna.mass = 10000;
	antenna.SetTemp(295.0); // 70-ish
	lastTemp = antenna.Temp;
	if(lem != NULL){
		antheater.WireTo(&lem->HTR_LR_CB);
		lem->Panelsdk.AddHydraulic(&antenna);
		// lem->Panelsdk.AddThermal(&antenna);  // This gives nonsensical results
		lem->Panelsdk.AddElectrical(&antheater,false);
		antheater.Enable();
		antheater.SetPumpAuto();
	}
}

void LEM_LR::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// sprintf(oapiDebugString(),"LR Antenna Temp: %f DT %f Change: %f, AH %f",antenna.Temp,simdt,(lastTemp-antenna.Temp)/simdt,antheater.pumping);
	lastTemp = antenna.Temp;
}

void LEM_LR::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_LR::LoadState(FILEHANDLE scn,char *end_str){

}

double LEM_LR::GetAntennaTempF(){

	return(0);
}

// Rendezvous Radar
// Position and draw numbers are just guesses!
LEM_RR::LEM_RR() : antenna("LEM-RR-Antenna",_vector3(0.013, 3.0, 0.03),0.03,0.04),
	antheater("LEM-RR-Antenna-Heater",1,NULL,15,20,0,255,288,&antenna)
{
	lem = NULL;	
}

void LEM_RR::Init(LEM *s,e_object *dc_src,e_object *ac_src){
	lem = s;
	// Set up antenna.
	// LR antenna is designed to operate between ??F and 75F
	// The heater switches on if the temperature gets below ??F and turns it off again when the temperature reaches ??F
	// The CWEA complains if the temperature is outside of -54F to +148F
	// Values in the constructor are name, pos, vol, isol
	// The DC side of the RR is most of it, the AC provides the transmit source.
	antenna.isolation = 1.0; 
	antenna.Area = 9187.8912; // Area of reflecting dish, probably good enough
	antenna.mass = 10000;
	antenna.SetTemp(255.1); 
	trunnionAngle = 0 * RAD; 
	trunnionMoved = 0 * RAD;
	shaftAngle = -180 * RAD;  // Stow
	shaftMoved = -180 * RAD;
	if(lem != NULL){
		antheater.WireTo(&lem->HTR_RR_STBY_CB);
		lem->Panelsdk.AddHydraulic(&antenna);
		lem->Panelsdk.AddElectrical(&antheater,false);
		antheater.Enable();
		antheater.SetPumpAuto();
	}
	dc_source = dc_src;
	ac_source = ac_src;

}


void LEM_RR::RRTrunionDrive(int val,int ch12) {

	int pulses;
	LMChannelValue12 val12;
	val12.Value = ch12;

	if (IsPowered() == 0) { return; }

	if (val&040000){ // Negative
		pulses = -((~val)&07777); 
	} else {
		pulses = val&07777; 
	}
	if (val12.Bits.EnableRRCDUErrorCounter){
		lem->agc.vagc.Erasable[0][RegOPTY] += pulses;
		lem->agc.vagc.Erasable[0][RegOPTY] &= 077777;
	}
	trunnionVel = (RR_TRUNNION_STEP*pulses);
	trunnionAngle += (RR_TRUNNION_STEP*pulses); 
	// sprintf(oapiDebugString(),"TRUNNION: %o PULSES, POS %o", pulses&077777 ,sat->agc.vagc.Erasable[0][035]);		
}


bool LEM_RR::IsPowered()

{
	if (IsDCPowered() && ac_source->Voltage() > 100) { 
		return true;
	}
	return false;
}

bool LEM_RR::IsDCPowered()

{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE) { 
		return false;
	}
	return true;
}

void LEM_RR::RRShaftDrive(int val,int ch12) {

	int pulses;
	LMChannelValue12 val12;
	val12.Value = ch12;
	
	if (!IsPowered()) { return; }

	if (val&040000){ // Negative
		pulses = -((~val)&07777); 
	} else {
		pulses = val&07777; 
	}
	shaftVel = (RR_SHAFT_STEP*pulses);
	shaftAngle += (RR_SHAFT_STEP*pulses);
	if (val12.Bits.EnableRRCDUErrorCounter){
		lem->agc.vagc.Erasable[0][RegOPTX] += pulses;
		lem->agc.vagc.Erasable[0][RegOPTX] &= 077777;
	}
	// sprintf(oapiDebugString(),"SHAFT: %o PULSES, POS %o", pulses&077777, sat->agc.vagc.Erasable[0][036]);
}


VECTOR3 LEM_RR::GetPYR(VECTOR3 Pitch, VECTOR3 YawRoll)
{	
	VECTOR3 Res = { 0, 0, 0 };

	// Normalize the vectors
	Pitch = Normalize(Pitch);
	YawRoll = Normalize(YawRoll);
	VECTOR3 H = Normalize(CrossProduct(Pitch, YawRoll));

	Res.data[YAW] = asin(Pitch.x);
	Res.data[ROLL] = -atan2(H.x, YawRoll.x);
	Res.data[PITCH] = -atan2(Pitch.y, Pitch.z);
	return Res;

}


VECTOR3 LEM_RR::GetPYR2(VECTOR3 Pitch, VECTOR3 YawRoll)
{	
	VECTOR3 Res = { 0, 0, 0 };

	// Normalize the vectors
	Pitch = Normalize(Pitch);
	YawRoll = Normalize(YawRoll);
	VECTOR3 H = Normalize(CrossProduct(Pitch, YawRoll));

	Res.data[YAW] = -asin(Pitch.x);
	Res.data[ROLL] = atan2(H.x, YawRoll.x);
	Res.data[PITCH] = atan2(Pitch.y, Pitch.z);
	return Res;

}


void LEM_RR::CalculateRadarData(double &pitch, double &yaw)
{
	VECTOR3 csmpos,  lmpos;
	VECTOR3 RelPos, RelVel;
    VECTOR3 RefAttitude,PitchYawRoll; //Reference attitude
	VESSELSTATUS Status;

	VESSEL *csm=lem->agc.GetCSM();
	VECTOR3 SpacecraftPos, TargetPos,  GVel;
	VECTOR3 GRelPos, H;

	lem->GetGlobalPos(lmpos);
	csm->GetGlobalPos(csmpos);
	lem->Global2Local(lmpos, SpacecraftPos); // Convert to positions to local coordinates
	lem->Global2Local(csmpos, TargetPos);
	RelPos = TargetPos - SpacecraftPos; // Calculate relative position of target in local coordinates
	lem->GetRelativePos(csm->GetHandle(), GRelPos); // Get position of spacecraft relative to target
	GRelPos = -GRelPos; // Reverse vector so that the it points from spacecraft to target
	lem->GetStatus(Status);
	if ( (Mag(CrossProduct(Normalize(-Status.rvel), Normalize(GRelPos)))) < 0.1 ) { // Check to see if target is too close to velocity vector. Corresponds to approx 5deg.
		H = CrossProduct(Status.rpos, GRelPos); // Use local vectical as roll reference
	}
	else {
		H = CrossProduct(Status.rvel, GRelPos); // Use velocity vector as roll reference
	}

	RefAttitude = GetPYR2(GRelPos, H);
	
    VECTOR3 GlobalPts_Pitch,GlobalPts_Yaw, LocalPts_Pitch, LocalPts_Yaw;
	VECTOR3 PitchUnit = {0, 0, 1.0}, YawRollUnit = {1.0, 0, 0};

	RotateVector(PitchUnit, RefAttitude, GlobalPts_Pitch);
	RotateVector(YawRollUnit, RefAttitude, GlobalPts_Yaw);

	GlobalPts_Pitch = lmpos + GlobalPts_Pitch;
	GlobalPts_Yaw = lmpos + GlobalPts_Yaw;	

	lem->Global2Local(GlobalPts_Pitch, LocalPts_Pitch);
	lem->Global2Local(GlobalPts_Yaw, LocalPts_Yaw);

	PitchYawRoll = GetPYR(LocalPts_Pitch, LocalPts_Yaw);

	// Calculate relative velocity
	lem->GetRelativeVel(csm->GetHandle(), GVel);
	lem->Global2Local((GVel + lmpos), RelVel);
	range =  Mag(RelPos);
	// Compute the radial component
	rate = (RelPos * RelVel) / Mag(RelPos);

	pitch = PitchYawRoll.x ; 
	yaw = PitchYawRoll.y ;
}


void LEM_RR::TimeStep(double simdt){

	LMChannelValue33 val33;
	LMChannelValue12 val12;
	LMChannelValue13 val13;
	val33.Value = lem->agc.GetCh33Switches();
	val12.Value = lem->agc.GetInputChannel(012);
	val13.Value = lem->agc.GetInputChannel(013);

	double ShaftRate = 0;
	double TrunRate = 0;

	radarDataGood = 0;
	if (!IsPowered() ) { 
		val33.Bits.RRPowerOnAuto = 0;
		val33.Bits.RRDataGood = 0;
		lem->agc.SetInputChannel(033,val33.Value);
		return;
	}
	// Max power used based on LM GNCStudyGuide. Is this good
	dc_source->DrawPower(130);
	// FIXME: Do you have a number for the AC side?
	
	switch(lem->SlewRateSwitch.GetState()) {
		case TOGGLESWITCH_UP:       // HI
			ShaftRate = 1775. * simdt;
			TrunRate  = 3640. * simdt;
			break;
		case TOGGLESWITCH_DOWN:     // LOW
			ShaftRate = 18. * simdt;
			TrunRate  = 36. * simdt;
			break;
	}

	double pitch,yaw;
	CalculateRadarData(pitch,yaw);

	if((fabs(shaftAngle-pitch) < 2*RAD ) &&  (fabs(trunnionAngle-yaw) < 2*RAD) && ( range < 740800.0) ) {
		radarDataGood = 1;
		val33.Bits.RRDataGood = radarDataGood;
		if (val13.Bits.RadarActivity && val13.Bits.RadarA) { // Request Range R-567-sec4-rev7-R10-R56.pdf R22.
			if ( range > 93681.639 ) { // Ref R-568-sec6.prf p 6-59
				val33.Bits.RRRangeLowScale = 1; // Inverted bits
				lem->agc.vagc.Erasable[0][RegRNRAD]=(int16_t) range * 0.043721214 ;
			}
			else {
				val33.Bits.RRRangeLowScale = 0; // Inverted bits
				lem->agc.vagc.Erasable[0][RegRNRAD]=(int16_t) range * 0.34976971 ;
			}	
			lem->agc.GenerateRadarupt();
		} else if (val13.Bits.RadarActivity && val13.Bits.RadarB) {
				lem->agc.vagc.Erasable[0][RegRNRAD]=(int16_t) rate;
				lem->agc.GenerateRadarupt();
	}
//		  	    sprintf(oapiDebugString(),"range = %f, rate=%f, CSM pitch=%f,CSM yaw=%f,Shaft=%f,Trun=%f",range,rate,pitch * DEG, yaw * DEG,shaftAngle*DEG,trunnionAngle*DEG);
	}// else
//		sprintf(oapiDebugString(),"NO TRACK, Shaft=%f,Trun=%f",shaftAngle*DEG,trunnionAngle*DEG);




	if( lem->RendezvousRadarRotary.GetState()==1 ) { // Slew
			if((lem->RadarSlewSwitch.GetState()==4) && trunnionAngle < (RAD*90)){
				trunnionAngle += RR_TRUNNION_STEP * TrunRate;				
				trunnionVel = RR_TRUNNION_STEP * TrunRate;
				while(fabs(fabs(trunnionAngle)-fabs(trunnionMoved)) >= RR_TRUNNION_STEP){					
					lem->agc.vagc.Erasable[0][RegOPTY]++;
					lem->agc.vagc.Erasable[0][RegOPTY] &= 077777;
					trunnionMoved += RR_TRUNNION_STEP;
				}
			}
			if((lem->RadarSlewSwitch.GetState()==3) && trunnionAngle > RAD*-90){
				trunnionAngle -= RR_TRUNNION_STEP * TrunRate;				
				trunnionVel = -RR_TRUNNION_STEP * TrunRate;
				while(fabs(fabs(trunnionAngle)-fabs(trunnionMoved)) >= RR_TRUNNION_STEP){					
					lem->agc.vagc.Erasable[0][RegOPTY]--;
					lem->agc.vagc.Erasable[0][RegOPTY] &= 077777;
					trunnionMoved -= RR_TRUNNION_STEP;
				}
			}
			if((lem->RadarSlewSwitch.GetState()==2) && shaftAngle > -(RAD*180)){
				shaftAngle -= RR_SHAFT_STEP * ShaftRate;					
				shaftVel = -RR_SHAFT_STEP * ShaftRate;					
				while(fabs(fabs(shaftAngle)-fabs(shaftMoved)) >= RR_SHAFT_STEP){
					lem->agc.vagc.Erasable[0][RegOPTX]--;
					lem->agc.vagc.Erasable[0][RegOPTX] &= 077777;
					shaftMoved -= RR_SHAFT_STEP;
				}
			}
			if((lem->RadarSlewSwitch.GetState()==0) && shaftAngle < (RAD*90)){
				shaftAngle += RR_SHAFT_STEP * ShaftRate;					
				shaftVel =RR_SHAFT_STEP * ShaftRate;					
				while(fabs(fabs(shaftAngle)-fabs(shaftMoved)) >= RR_SHAFT_STEP){
					lem->agc.vagc.Erasable[0][RegOPTX]++;
					lem->agc.vagc.Erasable[0][RegOPTX] &= 077777;
					shaftMoved += RR_SHAFT_STEP;
				}
			}
	} else if (lem->RendezvousRadarRotary.GetState() == 2 ) { // LGC
		val33.Bits.RRPowerOnAuto = 0; // Inverted ON
	} else
		val33.Bits.RRPowerOnAuto = 1; // Inverted OFF
	lem->agc.SetCh33Switches(val33.Value);

	// AutoTrack  the CSM using the RR
    if( ((val12.Bits.RRAutoTrackOrEnable == 1) || (lem->RendezvousRadarRotary.GetState()== 0  ) ) && radarDataGood == 1 ) {
		// Auto track within reach of trunnion/shaft
		if( ( pitch > -(RAD*180) ) && (pitch < (RAD * 90 ) && ( yaw > (RAD * -90) ) && (yaw < (RAD * 90))) ) {
			trunnionVel = (yaw-trunnionAngle) / simdt;					
			trunnionAngle = yaw;
			while(fabs(fabs(trunnionAngle)-fabs(trunnionMoved)) >= RR_TRUNNION_STEP){					
				if ( trunnionAngle < trunnionMoved ) {
					lem->agc.vagc.Erasable[0][RegOPTY]--;
					trunnionMoved -= RR_TRUNNION_STEP;
				} else {
					lem->agc.vagc.Erasable[0][RegOPTY]++;
					trunnionMoved += RR_TRUNNION_STEP;
				}
				lem->agc.vagc.Erasable[0][RegOPTY] &= 077777;
			}
			shaftVel = (pitch-shaftAngle) / simdt;					
			shaftAngle = pitch;
			while(fabs(fabs(shaftAngle)-fabs(shaftMoved)) >= RR_SHAFT_STEP){
				if( shaftAngle < shaftMoved ) {
					lem->agc.vagc.Erasable[0][RegOPTX]--;
					shaftMoved -= RR_SHAFT_STEP;
				} else {
					lem->agc.vagc.Erasable[0][RegOPTX]++;
					shaftMoved += RR_SHAFT_STEP;
				}
				lem->agc.vagc.Erasable[0][RegOPTX] &= 077777;
			}
		}
	}

	// sprintf(oapiDebugString(),"RR Antenna Temp: %f AH %f",antenna.Temp,antheater.pumping);
}

void LEM_RR::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_float(scn, "RR_TRUN", trunnionAngle);
	trunnionMoved = trunnionAngle;
	oapiWriteScenario_float(scn, "RR_SHAFT", shaftAngle);
	shaftMoved = shaftAngle;
	oapiWriteScenario_float(scn, "RR_ANTTEMP", GetAntennaTempF());
	oapiWriteLine(scn, end_str);
}

void LEM_RR::LoadState(FILEHANDLE scn,char *end_str){
		char *line;
	double dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "RR_TRUN", 7)) {
			sscanf(line + 7, "%f", &dec);
			trunnionAngle = dec;
		}
		if (!strnicmp (line, "RR_SHAFT", 7)) {
			sscanf(line + 7, "%f", &dec);
			shaftAngle = dec;
		}
	}
}

LEM_RadarTape::LEM_RadarTape()
{

}

void LEM_RadarTape::Init(LEM *s){
	lem = s;
}

void LEM_RadarTape::TimeStep(double simdt) {
	if( lem->AltRngMonSwitch.GetState()==TOGGLESWITCH_UP ) {
		if( lem->RR.IsRadarDataGood() ){
			setRange(lem->RR.GetRadarRange());
			setRate(lem->RR.GetRadarRate());
		} else {
			setRange(0);
			setRate(0);
		}
	} else {
		// LR
		setRange(0);
		setRate(0);
	}
	//
	//  Missing code to smooth out tape scrolling
	if( reqRange < (120000 *3.2808399) ) {
		dispRange = 6443 - 82 - (reqRange * 3.2808399) * 40 / 1000 ;
	} else {
		dispRange = 1642 - 82 - (reqRange * 0.000539956803)  * 40 / 1000 ;
	}
	dispRate  = 2881 - 82 -  reqRate * 3.3 * 40 / 1000 ; 
}


void LEM_RadarTape::SaveState(FILEHANDLE scn,char *start_str,char *end_str){
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "RDRTAPE_RANGE", dispRange);
	oapiWriteScenario_float(scn, "RDRTAPE_RATE", dispRate);
	oapiWriteLine(scn, end_str);
}

void LEM_RadarTape::LoadState(FILEHANDLE scn,char *end_str){
	char *line;
	int value = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp (line, "RDRTAPE_RANGE", 13)) {
			sscanf(line + 13, "%d", &value);
			dispRange = value;
		}
		if (!strnicmp (line, "RDRTAPE_RATE", 12)) {
			sscanf(line + 12, "%d", &value);
			dispRate = value;
		}
	}
}

void LEM_RadarTape::RenderRange(SURFHANDLE surf, SURFHANDLE tape)
{
    oapiBlt(surf,tape,0,0,0, dispRange ,43,163, SURF_PREDEF_CK); 
}

void LEM_RadarTape::RenderRate(SURFHANDLE surf, SURFHANDLE tape)
{
    oapiBlt(surf,tape,0,0,42, dispRate ,35,163, SURF_PREDEF_CK); 
}

double LEM_RR::GetAntennaTempF(){

	return(0);
}

// CWEA 

LEM_CWEA::LEM_CWEA(){
	lem = NULL;	
	CabinLowPressLt = 0;
	WaterWarningDisabled = 0;
}

void LEM_CWEA::Init(LEM *s){
	int row=0,col=0;
	while(col < 8){
		while(row < 5){
			LightStatus[row][col] = 0;
			row++;
		}
		row = 0; col++;
	}
	lem = s;
}

void LEM_CWEA::TimeStep(double simdt){
	LMChannelValue11 val11;
	LMChannelValue13 val13;
	LMChannelValue30 val30;
	LMChannelValue33 val33;		

	if(lem == NULL){ return; }
	val11.Value = lem->agc.GetOutputChannel(011);
	val13.Value = lem->agc.GetOutputChannel(013);
	val30.Value = lem->agc.GetInputChannel(030);
	val33.Value = lem->agc.GetInputChannel(033);

	// 6DS2 ASC PROP LOW
	// Pressure of either ascent helium tanks below 2773 psia prior to staging, - This reason goes out when stage deadface opens.
	// Blanket pressure in fuel or oxi lines at the bi-propellant valves of the ascent stage below 120 psia
	LightStatus[1][0] = 1;

	// 6DS3 HI/LO HELIUM REG OUTLET PRESS
	// Enabled by DES ENG "ON" command. Disabled by stage deadface open.
	// Pressure in descent helium lines downstream of the regulators is above 260 psia or below 220 psia.
	LightStatus[2][0] = 0; // Default
	if(lem->DPS.EngineOn){ // This should be forced off at staging
		if(lem->DPS.HePress[1] > 260 || lem->DPS.HePress[1] < 220){
			LightStatus[2][0] = 1;
		}
	}

	// 6DS4 DESCENT PROPELLANT LOW
	// On if fuel/oxi in descent stage below 2 minutes endurance @ 25% power prior to staging.
	// (This turns out to be 5.6%)
	// Master Alarm and Tone are disabled if this is active.
	if(lem->stage < 2 && lem->GetPropellantMass(lem->ph_Dsc) < 190.305586){
		LightStatus[3][0] = 1;
	}else{
		LightStatus[3][0] = 0;
	}

	// 6DS6 CES AC VOLTAGE FAILURE
	// Either CES AC voltage (26V or 28V) out of tolerance.
	// This power is provided by the ATCA main power supply and spins the RGAs and operate the AEA reference.
	// Disabled by Gyro Test Control in POS RT or NEG RT position.
	if(lem->SCS_ATCA_CB.Voltage() > 24 || lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER){
		LightStatus[0][1] = 0;
	}else{ 
		LightStatus[0][1] = 1;
	}

	// 6DS7 CES DC VOLTAGE FAILURE
	// Any CES DC voltage out of tolerance.
	// All of these are provided by the ATCA main power supply.
	// Disabled by Gyro Test Control in POS RT or NEG RT position.
	if(lem->SCS_ATCA_CB.Voltage() > 24 || lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER){
		LightStatus[1][1] = 0;
	}else{
		LightStatus[1][1] = 1;
	}

	// 6DS8 AGS FAILURE
	// On when any AGS power supply signals a failure, when AGS raises failure signal, or ASA heater fails.
	// Disabled when AGS status switch is OFF.
	// FIXME: Finish this!
	if(lem->AGSOperateSwitch.GetState() == THREEPOSSWITCH_DOWN){
		LightStatus[2][1] = 0;
	}else{
		LightStatus[2][1] = 1;
	}

	// 6DS9 LGC FAILURE
	// On when any LGC power supply signals a failure, scaler fails, LGC restarts, counter fails, or LGC raises failure signal.
	// Disabled by Guidance Control switch in AGS position.
	if((val13.Bits.TestAlarms || val33.Bits.LGC || val33.Bits.OscillatorAlarm) && lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP){
		LightStatus[3][1] = 1;
	}else{
		LightStatus[3][1] = 0;
	}

	// 6DS10 ISS FAILURE
	// On when ISS power supply fails, PIPA fails while main engine thrusting, gimbal servo fails, CDU fails.
	// Disabled by Guidance Control switch in AGS position.
	if ((val11.Bits.ISSWarning || val33.Bits.PIPAFailed || val30.Bits.IMUCDUFailure || val30.Bits.IMUFailure) && lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP){
		LightStatus[4][1] = 1;
	}else{
		LightStatus[4][1] = 0;
	}

	// 6DS11 RCS TCA WARNING
	// RCS fire command exists with no resulting chamber pressure,
	// chamber pressure present when no fire command exists,
	// opposing colinear jets on simultaneously
	// Disabled when failing TCA isol valve closes.
	// FIXME: Implement this test.
	LightStatus[0][2] = 0;

	// 6DS12 RCS A REGULATOR FAILURE
	// 6DS13 RCS B REGULATOR FAILURE
	// RCS helium line pressure above 205 pisa or below 165 psia. Disabled when main shutoff solenoid valves close.
	LightStatus[1][2] = 1;
	LightStatus[2][2] = 1;

	// 6DS14 DC BUS VOLTAGE FAILURE
	// On when CDR or SE DC bus below 26.5 V.
	if(lem->CDRs28VBus.Voltage() < 26.5 || lem->LMPs28VBus.Voltage() < 26.5){
		LightStatus[3][2] = 1;
	}else{
		LightStatus[3][2] = 0;
	}

	// 6DS16 CABIN LOW PRESSURE WARNING
	// On when cabin pressure below 4.15 psia (+/- 0.3 psia)
	// Off when cabin pressure above 4.65 psia (+/- 0.25 psia)
	// Disabled when both Atmosphere Revitalization Section Pressure Regulator Valves in EGRESS or CLOSE position.
	if(lem->ecs.Cabin_Press < 4.15){
		CabinLowPressLt = 1;
	}
	if(lem->ecs.Cabin_Press > 4.65 && CabinLowPressLt){
		CabinLowPressLt = 0;
	}
	// FIXME: Need to check valve when enabled
	if(CabinLowPressLt){
		LightStatus[0][3] = 1;
	}else{
		LightStatus[0][3] = 0;
	}

	// 6DS17 SUIT/FAN LOW PRESSURE WARNING
	// On when suit pressure below 3.12 psia or #2 suit circulation fan fails.
	// Suit fan failure alarm disabled when Suit Fan DP Control CB is open.
	// FIXME: IMPLEMENT #2 SUIT CIRC FAN TEST
	if(lem->ECS_SUIT_FAN_DP_CB.GetState() == 0 && lem->ecs.Suit_Press < 3.12){
		LightStatus[1][3] = 1;
	}

	// 6DS21 HIGH HELIUM REGULATOR OUTLET PRESSURE CAUTION
	// On when helium pressure downstream of regulators in ascent helium lines above 220 psia.
	if(lem->APS.HePress[1] > 220){
		LightStatus[0][4] = 1;
	}else{
		LightStatus[0][4] = 0;
	}

	// 6DS22 ASCENT PROPELLANT LOW QUANTITY CAUTION
	// On when less than 10 seconds of ascent propellant/oxidizer remains.
	// Disabled when ascent engine is not firing.
	// FIXME: This test probably used a fixed setpoint instead of division. Investigate.
	if(lem->APS.EngineOn && lem->GetPropellantFlowrate(lem->ph_Asc) > 0 && (lem->GetPropellantMass(lem->ph_Asc)/lem->GetPropellantFlowrate(lem->ph_Asc) < 10)){
		LightStatus[1][4] = 1;
	}else{
		LightStatus[1][4] = 0;
	}

	// 6DS23 AUTOMATIC GIMBAL FAILURE CAUTION
	// On when difference in commanded and actual descent engine trim position is detected.
	// Enabled when descent engine armed and engine gimbal switch is enabled.
	// Disabled by stage deadface open.
	// FIXME: We'll ignore this for now.
	LightStatus[2][4] = 0;

	// 6DS26 INVERTER FAILURE CAUTION
	// On when AC bus voltage below 112V or frequency below 398hz or above 402hz.
	// Disabled when AC Power switch is off.
	if(lem->EPSInverterSwitch.GetState() != THREEPOSSWITCH_DOWN){
		if(lem->ACBusA.Voltage() < 112 || lem->ACBusB.Voltage() < 112){
			LightStatus[0][5] = 1;
		}else{
			LightStatus[0][5] = 0;
		}
	}else{
		LightStatus[0][5] = 0;
	}

	// 6DS27 BATTERY FAILURE CAUTION
	// On when over-current, reverse-current, or over-temperature condition occurs in any ascent or descent battery.
	// Disabled if affected battery is turned off.
	// FIXME: We'll ignore this for now.
	LightStatus[1][5] = 0;

	// 6DS28 RENDEZVOUS RADAR DATA FAILURE CAUTION
	// On when RR indicates Data-Not-Good.
	// Disabled when RR mode switch is not set to AUTO TRACK.
	if(lem->RendezvousRadarRotary.GetState() != 0){
		LightStatus[2][5] = 0;
	}else{
		LightStatus[2][5] = 1;
	}

	// 6DS29 LANDING RADAR was not present on LM-7 thru LM-9!
	LightStatus[3][5] = 2;

	// 6DS30 PRE-AMPLIFIER POWER FAILURE CAUTION
	// On when either ATCA solenoid driver power supply fails.
	// Disabled by stage deadface open or Abort PB press.
	LightStatus[4][5] = 0;
	if(lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP && lem->CDR_SCS_ATCA_CB.Voltage() < 24){ LightStatus[4][5] = 1; }
	if(lem->GuidContSwitch.GetState() == TOGGLESWITCH_DOWN && lem->SCS_ATCA_AGS_CB.Voltage() < 24){ LightStatus[4][5] = 1; }
	// FIXME: Handle stage DF and abort PB disables

	// 6DS31 EDS RELAY FAILURE
	// On when any EDS relay fails.
	// Failures of stage relays disabled when stage relay switch in RESET position.
	// Disabled when MASTER ARM is ON or if ABORT STAGE commanded.
	// FIXME: We'll ignore this for now.
	LightStatus[0][6] = 0;

	// 6DS32 RCS FAILURE CAUTION
	// On when helium pressure in either RCS system below 1700 psia.
	// Disabled when RCS TEMP/PRESS MONITOR switch in HELIUM position.
	LightStatus[1][6] = 0;
	if(lem->TempPressMonRotary.GetState() != 0){
		LightStatus[1][6] = 1;
	}

	// 6DS33 HEATER FAILURE CAUTION
	// On when:
	// S-Band Antenna Electronic Drive Assembly < -64.08F or > 153.63F
	// RR Assembly < -57.07F or > 147.69F
	// LR Assembly < -19.26F or > 147.69F
	// Disabled when Temperature Monitor switch selects affected assembly.
	LightStatus[2][6] = 0;
	if(lem->TempMonitorRotary.GetState() != 6 && (lem->SBandSteerable.GetAntennaTempF() < -64.08 || lem->SBandSteerable.GetAntennaTempF() > 153.63)){
		LightStatus[2][6] = 1;
	}
	if(lem->TempMonitorRotary.GetState() != 0 && (lem->RR.GetAntennaTempF() < -57.07 || lem->RR.GetAntennaTempF() > 147.60)){
		LightStatus[2][6] = 1;
	}
	if(lem->TempMonitorRotary.GetState() != 1 && (lem->LR.GetAntennaTempF() < -19.27 || lem->LR.GetAntennaTempF() > 147.60)){
		LightStatus[2][6] = 1;
	}

	// 6DS34 CWEA POWER FAILURE CAUTION
	// On when any CWEA power supply indicates failure.
	// Not dimmable. Master Alarm associated with this failure cannot be silenced.
	// FIXME: We'll ignore this for now.
	LightStatus[3][6] = 0;

	// 6DS36 ECS FAILURE CAUTION
	// On when:
	// Glycol Pump Failure
	// CO2 Partial Pressure > 7.6mm
	// Water Separator Failure
	// Suit Fan #1 Failure
	// Off when (in order of failure):
	// Glycol pump pressure restored by selection of pump 2, or selecting INST(SEC) if #2 has failed
	// Restoration of normal CO2 pressure
	// Restoration of normal water separator speed
	// Selection of #2 suit fan
	LightStatus[0][7] = 1;

	// 6DS37 OXYGEN QUANTITY CAUTION
	// On when:
	// < 135 psia in descent oxygen tank, or Less than full (<682.4 / 681.6 psia) ascent oxygen tanks, WHEN NOT STAGED
	// Less than 99.6 psia in ascent oxygen tank #1
	// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
	LightStatus[1][7] = 0;
	if(lem->stage < 2 && (lem->ecs.Asc_Oxygen[0] < 2.43 || lem->ecs.Asc_Oxygen[1] < 2.43)){ LightStatus[1][7] = 1; }
	if(lem->stage < 2 && (lem->ecs.DescentOxyTankPressure(0) < 135 || lem->ecs.DescentOxyTankPressure(1) < 135)){ LightStatus[1][7] = 1; }
	if(lem->ecs.AscentOxyTankPressure(0) < 99.6){ LightStatus[1][7] = 1; }

	// 6DS38 GLYCOL FAILURE CAUTION
	// On when glycol qty low in primary coolant loop or primary loop glycol temp @ water evap outlet > 49.98F
	// Disabled by Glycol Pump to INST(SEC) position
	LightStatus[2][7] = 1;

	// 6DS39 WATER QUANTITY CAUTION
	// On when:
	// NOT STAGED: Descent water tank < 10% or less than full in either ascent tank
	// Unequal levels in either ascent tank
	// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
	LightStatus[3][7] = 0;
	if(WaterWarningDisabled == 0){
		if(lem->stage < 2 && (lem->ecs.Des_Water[0] < 33 || lem->ecs.Des_Water[1] < 33)){ LightStatus[3][7] = 1; }
		if(lem->stage < 2 && (lem->ecs.Asc_Water[0] < 42.5 || lem->ecs.Asc_Water[1] < 42.5)){ LightStatus[3][7] = 1; }
		if((int)lem->ecs.Asc_Water[0] != (int)lem->ecs.Asc_Water[1]){ LightStatus[3][7] = 1; }
	}
	if(lem->QtyMonRotary.GetState() == 0 && LightStatus[3][7] != 0){
		WaterWarningDisabled = 1;
	}

	// 6DS40 S-BAND RECEIVER FAILURE CAUTION
	// On when AGC signal lost.
	// Off when Range/TV function switch to OFF/RESET
	// Disabled when Range/TV switch is not in TV/CWEA ENABLE position
	LightStatus[4][7] = 0;
	if(lem->SBandRangeSwitch.GetState() == THREEPOSSWITCH_DOWN){
		LightStatus[4][7] = 1;
	}

	// RendezVous Radar Caution

	LightStatus[2][5]=0;
	if(lem->RendezvousRadarRotary.GetState()==0 && lem->RR.IsRadarDataGood() == 0 ) {
		LightStatus[2][5]=1;
	}
	// CWEA TEST SWITCH FUNCTIONALITY
	if(lem->LTG_MASTER_ALARM_CB.Voltage() > 0){
		switch(lem->LampToneTestRotary.GetState()){
			case 0: // OFF
			case 7: // OFF
			default:
				break;
			case 1: // ALARM/TONE
				// Light MASTER ALARM and sound tone
				// FIXME: IMPLEMENT THIS
				break;
			case 2: // C/W 1
				// Light Panel 1 first bank warning lamps
				LightStatus[0][0] = 1; LightStatus[1][0] = 1; LightStatus[2][0] = 1; LightStatus[3][0] = 1; LightStatus[4][0] = 1;
				LightStatus[0][1] = 1; LightStatus[1][1] = 1; LightStatus[2][1] = 1; LightStatus[3][1] = 1; LightStatus[4][1] = 1;
				break;
			case 3: // ENG PB & C/W 2
				// Light engine START/STOP lights and Panel 1 second bank warning lamps
				LightStatus[0][2] = 1; LightStatus[1][2] = 1; LightStatus[2][2] = 1; LightStatus[3][2] = 1; LightStatus[4][2] = 1;
				LightStatus[0][3] = 1; LightStatus[1][3] = 1; LightStatus[2][3] = 1; LightStatus[3][3] = 1; LightStatus[4][3] = 1;
				break;
			case 4: // C/W 3
				// Light Panel 2 first bank warning lamps;
				LightStatus[0][4] = 1; LightStatus[1][4] = 1; LightStatus[2][4] = 1; LightStatus[3][4] = 1; LightStatus[4][4] = 1;
				LightStatus[0][5] = 1; LightStatus[1][5] = 1; LightStatus[2][5] = 1; /* LightStatus[3][5] = 1; */ LightStatus[4][5] = 1; // LDG RDR lamp only for LM10+
				break;
			case 5: // C/W 4
				// Light Panel 2 second bank warning lamps;
				LightStatus[0][6] = 1; LightStatus[1][6] = 1; LightStatus[2][6] = 1; LightStatus[3][6] = 1; LightStatus[4][6] = 1;
				LightStatus[0][7] = 1; LightStatus[1][7] = 1; LightStatus[2][7] = 1; LightStatus[3][7] = 1; LightStatus[4][7] = 1;
				break;
			case 6: // COMPNT
				// Light component caution and Lunar Contact lights
				// FIXME: IMPLEMENT THIS
				break;
		}
	}
}	

void LEM_CWEA::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_CWEA::LoadState(FILEHANDLE scn,char *end_str){

}

void LEM_CWEA::RedrawLeft(SURFHANDLE sf, SURFHANDLE ssf){
	int row=0,col=0,dx=0,dy=0;
	while(col < 4){
		switch(col){
			case 0:
				dx = 0; break;
			case 1:
				dx = 71; break;
			case 2:
				dx = 167; break;
			case 3:
				dx = 238; break;
		}
		while(row < 5){
			if(LightStatus[row][col] == 1 && lem->INST_CWEA_CB.Voltage() > 24){
				dy=134;
			}else{
				dy=7;
			}
			if(LightStatus[row][col] == 2){
				// Special Hack: This Lamp Doesn't Exist
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 8, 7, 67, 19);
			}else{
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 8+dx, dy+(row*23), 67, 19);
			}
			row++;
		}
		row = 0; col++;
	}
}

void LEM_CWEA::RedrawRight(SURFHANDLE sf, SURFHANDLE ssf){
	int row=0,col=0,dx=0,dy=0;
	while(col < 4){
		switch(col){
			case 0:
				dx = 0; break;
			case 1:
				dx = 71; break;
			case 2:
				dx = 146; break;
			case 3:
				dx = 217; break;
		}
		while(row < 5){
			if(LightStatus[row][col+4] == 1 && lem->INST_CWEA_CB.Voltage() > 24){
				dy = 134;
			}else{
				dy = 7;
			}
			if(LightStatus[row][col+4] == 2){
				// Special Hack: This Lamp Doesn't Exist
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 8, 7, 67, 19);
			}else{
				oapiBlt(sf, ssf, 8+dx, 7+(row*23), 330+dx, dy+(row*23), 67, 19);
			}
			row++;
		}
		row = 0; col++;
	}
}

// Environmental Control System
LEM_ECS::LEM_ECS()
{
	lem = NULL;	
	// Initialize
	Asc_Oxygen[0] = 2.43; Asc_Oxygen[1] = 2.43;
	Des_Oxygen[0] = 48.01; Des_Oxygen[1] = 48.01;
	Asc_Water[0] = 42.5; Asc_Water[1] = 42.5;
	Des_Water[0] = 333; Des_Water[1] = 333;
	Primary_CL_Glycol_Press[0] = 0; Primary_CL_Glycol_Press[1] = 0; // Zero this, system will fill from accu
	Secondary_CL_Glycol_Press[0] = 0; Secondary_CL_Glycol_Press[1] = 0; // Zero this, system will fill from accu
	Primary_CL_Glycol_Temp[0] = 40; Primary_CL_Glycol_Temp[1] = 0; // 40 in the accu, 0 other side of the pump
	Secondary_CL_Glycol_Temp[0] = 40; Secondary_CL_Glycol_Temp[1] = 0; // 40 in the accu, 0 other side of the pump
	Primary_Glycol_Accu = 46; // Cubic inches of coolant
	Secondary_Glycol_Accu = 46; // Cubic inches of coolant
	Primary_Glycol = 0;
	Secondary_Glycol = 0;
	// Open valves as would be for IVT
	Des_O2 = 1; 
	Des_H2O_To_PLSS = 1;
	Cabin_Repress = 2; // Auto
	// For simplicity's sake, we'll use a docked LM as it would be at IVT, at first docking the LM is empty!
	Cabin_Press = 4.8; Cabin_Temp = 55; Cabin_CO2 = 1;
	Suit_Press = 4.8; Suit_Temp = 55; Suit_CO2 = 1;
}

void LEM_ECS::Init(LEM *s){
	lem = s;	
}

void LEM_ECS::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// **** Atmosphere Revitalization Section ****
	// First, get air from the suits and/or the cabin into the system.
	// Second, remove oxygen for and add CO2 from the crew.
	// Third, remove CO2 from the air and simulate the reaction in the LiOH can
	// Fourth, use the fans to move the resulting air through the suits and/or the cabin.
	// Fifth, use the heat exchanger to move heat from the air to the HTS if enabled (emergency ops)
	// Sixth, use the water separators to remove water from the air and add it to the WMS and surge tank.
	// Seventh, use the OSCPCS to add pressure if required
	// Eighth, use the regenerative heat exchanger to add heat to the air if required
	// Ninth and optionally, simulate the system behavior if a PGA failure is detected.
	// Tenth, simulate the LCG water movement operation.

	// **** Oxygen Supply and Cabin Pressure Control Section ****
	// Simple, move air from tanks to the cabin as required, and move air from the cabin to space as required.

	// **** Water Management Section ****
	// Also relatively simple, move water from tanks to the HTS / crew / etc as required.

	// **** Heat Transport Section ****
	// First, operate pumps to move glycol/water through the loops.
	// Second, move heat from the equipment to the glycol.
	// Third, move heat from the glycol to the sublimators.
	// Fourth, vent steam from the sublimators overboard.
}

void LEM_ECS::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_ECS::LoadState(FILEHANDLE scn,char *end_str){

}

double LEM_ECS::AscentOxyTankPressure(int tank){
	// 2.43 is a full tank, at 840 psia.
	// 0.14 is an empty tank, at 50 psia.
	// So 790 psia and 2.29 pounds of oxygen.
	// That's 344.9781659388646 psia per pound
	return(344.9781659388646*Asc_Oxygen[tank]);
}

double LEM_ECS::DescentOxyTankPressure(int tank){
	// 48.01 is a full tank, at 2690 psia.
	// 0.84 is an empty tank, at 50 psia.
	// So 2640 psia and 47.17 pounds of oxygen.
	// That's 55.96777612889548 psia per pound
	return(55.96777612889548*Des_Oxygen[tank]);
}

// Descent Propulsion System
LEM_DPS::LEM_DPS(){
	lem = NULL;	
	EngineOn = 0;
	HePress[0] = 0; HePress[1] = 0;
}

void LEM_DPS::Init(LEM *s){
	lem = s;
}

void LEM_DPS::TimeStep(double simdt){
	if(lem == NULL){ return; }
}

void LEM_DPS::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_DPS::LoadState(FILEHANDLE scn,char *end_str){

}

// Ascent Propulsion System
LEM_APS::LEM_APS(){
	lem = NULL;	
	EngineOn = 0;
	HePress[0] = 0; HePress[1] = 0;
}

void LEM_APS::Init(LEM *s){
	lem = s;
}

void LEM_APS::TimeStep(double simdt){
	if(lem == NULL){ return; }
}

void LEM_APS::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_APS::LoadState(FILEHANDLE scn,char *end_str){

}

