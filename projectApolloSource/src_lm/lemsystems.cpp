/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

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

#include "LEM.h"

#include "CollisionSDK/CollisionSDK.h"

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

	// RCS stuff
	RCS_B_PQGS_DISP_CB.MaxAmps = 2.0;
	RCS_B_PQGS_DISP_CB.WireTo(&LMPs28VBus);

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

	// LGC and DSKY
	LGC_DSKY_CB.MaxAmps = 7.5;
	LGC_DSKY_CB.WireTo(&CDRs28VBus);
	agc.WirePower(&LGC_DSKY_CB,&LGC_DSKY_CB);
	dsky.Init(&LGC_DSKY_CB);

	// AGS stuff
	asa.Init(this);
	aea.Init(this);
	deda.Init(this);

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

	// The FDAI has two CBs, AC and DC, and both are 2 amp CBs
	// CDR FDAI
	CDR_FDAI_DC_CB.MaxAmps = 2.0;
	CDR_FDAI_DC_CB.WireTo(&CDRs28VBus);
	CDR_FDAI_AC_CB.MaxAmps = 2.0;
	CDR_FDAI_AC_CB.WireTo(&ACBusA);
	// And the CDR FDAI itself	
	fdaiLeft.WireTo(&CDR_FDAI_DC_CB,&CDR_FDAI_AC_CB);
	// LMP FDAI stuff
	LMP_EVT_TMP_FDAI_DC_CB.MaxAmps = 2.0;
	LMP_EVT_TMP_FDAI_DC_CB.WireTo(&LMPs28VBus);
	LMP_FDAI_AC_CB.MaxAmps = 2.0;
	LMP_FDAI_AC_CB.WireTo(&ACBusB);
	fdaiRight.WireTo(&LMP_EVT_TMP_FDAI_DC_CB,&LMP_FDAI_AC_CB);
	EventTimerDisplay.WireTo(&LMP_EVT_TMP_FDAI_DC_CB);

	// HEATERS
	HTR_RR_STBY_CB.MaxAmps = 7.5;
	HTR_RR_STBY_CB.WireTo(&CDRs28VBus);
	HTR_LR_CB.MaxAmps = 5.0;
	HTR_LR_CB.WireTo(&CDRs28VBus);
	HTR_SBD_ANT_CB.MaxAmps = 5.0;
	HTR_SBD_ANT_CB.WireTo(&LMPs28VBus);

	// Landing Radar
	LR.Init(this);
	// Rdz Radar
	RR.Init(this);

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

	// Lighting
	CDR_LTG_UTIL_CB.MaxAmps = 2.0;
	CDR_LTG_UTIL_CB.WireTo(&CDRs28VBus);
	CDR_LTG_ANUN_DOCK_COMPNT_CB.MaxAmps = 2.0;
	CDR_LTG_ANUN_DOCK_COMPNT_CB.WireTo(&CDRs28VBus);
	LTG_FLOOD_CB.MaxAmps = 5.0;
	LTG_FLOOD_CB.WireTo(&LMPs28VBus);
	NUM_LTG_AC_CB.MaxAmps = 2.0;
	NUM_LTG_AC_CB.WireTo(&ACBusB);

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

	// EDS initialization
	eds.Init(this);

	// DS20060413 Initialize joystick
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
	thc_tjt_id = -1; // Disabled
	thc_debug = -1;
	rhc_debug = -1;
	
	// Initialize other systems
	atca.Init(this);
}

void LEM::SystemsTimestep(double simt, double simdt) 

{
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
	//dsky.SystemTimestep(simdt);						    // DSKY power draw is broken.
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
	atca.Timestep(simdt);								    // Do Work
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
	SBandSteerable.TimeStep(simdt);							// Back to work...
	VHF.SystemTimestep(simdt);
	VHF.TimeStep(simt);
	SBand.SystemTimestep(simdt);
	SBand.TimeStep(simt);

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


void LEM::CheckRCS()
{
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

	/*
	sprintf(oapiDebugString(),"CheckRCS: %d %d %f %f",GetValveState(LEM_RCS_MAIN_SOV_A),GetValveState(LEM_RCS_MAIN_SOV_B),
		GetPropellantMass(ph_DscRCSA),GetPropellantMass(ph_DscRCSB)); */	

	if(GetValveState(LEM_RCS_MAIN_SOV_A)){
		SetThrusterResource(th_rcs[0],ph_RCSA);
		SetThrusterResource(th_rcs[1],ph_RCSA);
		SetThrusterResource(th_rcs[6],ph_RCSA);
		SetThrusterResource(th_rcs[7],ph_RCSA);
		SetThrusterResource(th_rcs[8],ph_RCSA);
		SetThrusterResource(th_rcs[9],ph_RCSA);
		SetThrusterResource(th_rcs[14],ph_RCSA);
		SetThrusterResource(th_rcs[15],ph_RCSA);
	}else{
		SetThrusterResource(th_rcs[0],NULL);
		SetThrusterResource(th_rcs[1],NULL);
		SetThrusterResource(th_rcs[6],NULL);
		SetThrusterResource(th_rcs[7],NULL);
		SetThrusterResource(th_rcs[8],NULL);
		SetThrusterResource(th_rcs[9],NULL);
		SetThrusterResource(th_rcs[14],NULL);
		SetThrusterResource(th_rcs[15],NULL);
	}
	if(GetValveState(LEM_RCS_MAIN_SOV_B)){
		SetThrusterResource(th_rcs[2],ph_RCSB);
		SetThrusterResource(th_rcs[3],ph_RCSB);
		SetThrusterResource(th_rcs[4],ph_RCSB);
		SetThrusterResource(th_rcs[5],ph_RCSB);
		SetThrusterResource(th_rcs[10],ph_RCSB);
		SetThrusterResource(th_rcs[11],ph_RCSB);
		SetThrusterResource(th_rcs[12],ph_RCSB);
		SetThrusterResource(th_rcs[13],ph_RCSB);
	}else{
		SetThrusterResource(th_rcs[2],NULL);
		SetThrusterResource(th_rcs[3],NULL);
		SetThrusterResource(th_rcs[4],NULL);
		SetThrusterResource(th_rcs[5],NULL);
		SetThrusterResource(th_rcs[10],NULL);
		SetThrusterResource(th_rcs[11],NULL);
		SetThrusterResource(th_rcs[12],NULL);
		SetThrusterResource(th_rcs[13],NULL);
	}
	return;
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

void LEM_RR::Init(LEM *s){
	lem = s;
	// Set up antenna.
	// LR antenna is designed to operate between ??F and 75F
	// The heater switches on if the temperature gets below ??F and turns it off again when the temperature reaches ??F
	// The CWEA complains if the temperature is outside of -54F to +148F
	// Values in the constructor are name, pos, vol, isol
	antenna.isolation = 1.0; 
	antenna.Area = 9187.8912; // Area of reflecting dish, probably good enough
	antenna.mass = 10000;
	antenna.SetTemp(255.1); 
	if(lem != NULL){
		antheater.WireTo(&lem->HTR_RR_STBY_CB);
		lem->Panelsdk.AddHydraulic(&antenna);
		lem->Panelsdk.AddElectrical(&antheater,false);
		antheater.Enable();
		antheater.SetPumpAuto();
	}
}

void LEM_RR::TimeStep(double simdt){
	if(lem == NULL){ return; }
	// sprintf(oapiDebugString(),"RR Antenna Temp: %f AH %f",antenna.Temp,antheater.pumping);
}

void LEM_RR::SaveState(FILEHANDLE scn,char *start_str,char *end_str){

}

void LEM_RR::LoadState(FILEHANDLE scn,char *end_str){

}

double LEM_RR::GetAntennaTempF(){

	return(0);
}

