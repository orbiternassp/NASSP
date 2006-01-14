/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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
  *	Revision 1.51  2006/01/11 02:16:25  movieman523
  *	Added RCS propellant quantity gauge.
  *	
  *	Revision 1.50  2006/01/10 23:45:35  movieman523
  *	Revised RCS ISP and thrust to historical values.
  *	
  *	Revision 1.49  2006/01/08 16:57:11  flydba
  *	New bitmap added.
  *	
  *	Revision 1.48  2006/01/07 03:13:18  flydba
  *	New bitmap added.
  *	
  *	Revision 1.47  2006/01/06 20:37:18  movieman523
  *	Made the voltage and current meters work. Currently hard-coded to main bus A and AC bus 1.
  *	
  *	Revision 1.46  2006/01/06 02:04:53  flydba
  *	New bitmap added.
  *	
  *	Revision 1.45  2006/01/05 04:53:22  flydba
  *	Some new gfx added.!
  *	
  *	Revision 1.44  2006/01/03 17:51:02  flydba
  *	Some new bitmaps added.
  *	
  *	Revision 1.43  2006/01/03 13:00:18  flydba
  *	Some changes done on panel 15.
  *	
  *	Revision 1.42  2006/01/03 03:58:07  flydba
  *	Some changes done on panel 15.
  *	
  *	Revision 1.41  2005/11/19 20:54:47  movieman523
  *	Added SIVb DLL and wired it up to Saturn 1b.
  *	
  *	Revision 1.40  2005/11/18 04:44:01  flydba
  *	Window cover bitmap added.
  *	
  *	Revision 1.39  2005/11/18 03:12:51  flydba
  *	Some new switch bitmaps added.
  *	
  *	Revision 1.38  2005/11/17 07:08:20  flydba
  *	New srf added.
  *	
  *	Revision 1.37  2005/11/17 03:27:48  flydba
  *	Added new surfaces.
  *	
  *	Revision 1.36  2005/11/16 20:41:07  flydba
  *	New threeposswitch for panel 14 added.
  *	
  *	Revision 1.35  2005/10/31 10:31:34  tschachim
  *	New ONPAD_STAGE, new surfaces.
  *	
  *	Revision 1.34  2005/10/11 16:50:44  tschachim
  *	SRFs added.
  *	
  *	Revision 1.33  2005/09/30 11:27:21  tschachim
  *	New SRF.
  *	
  *	Revision 1.32  2005/08/29 19:25:25  tschachim
  *	Rendering of the DSKY keys.
  *	
  *	Revision 1.31  2005/08/23 22:18:47  movieman523
  *	SPS switch now works.
  *	
  *	Revision 1.30  2005/08/21 17:21:10  movieman523
  *	Added event timer display.
  *	
  *	Revision 1.29  2005/08/21 11:51:59  movieman523
  *	Initial version of CSM caution and warning lights: light test switch now works.
  *	
  *	Revision 1.28  2005/08/19 13:45:59  tschachim
  *	Added FDAI.
  *	
  *	Revision 1.27  2005/08/16 18:54:30  movieman523
  *	Added Altimeter and launch vehicle switches.
  *	
  *	Revision 1.26  2005/08/15 20:18:16  movieman523
  *	Made thrust meter work. Unfortunately on a real CSM it's not a thrust meter :).
  *	
  *	Revision 1.25  2005/08/13 22:24:20  movieman523
  *	Added the master alarm rendeing to CSM.
  *	
  *	Revision 1.24  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
  *	Revision 1.23  2005/08/11 16:29:33  spacex15
  *	Added PNGS and AGS mode control 3 pos switches
  *	
  *	Revision 1.22  2005/08/11 14:00:35  spacex15
  *	Added Descent Engine Command Override switch
  *	
  *	Revision 1.21  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.20  2005/08/10 20:00:55  spacex15
  *	Activated 3 position lem eng arm switch
  *	
  *	Revision 1.19  2005/08/09 09:18:24  tschachim
  *	New SRF constants
  *	
  *	Revision 1.18  2005/08/07 19:32:28  lazyd
  *	Changed ascent parameters to historical
  *	
  *	Revision 1.17  2005/07/19 16:23:11  tschachim
  *	New pen, surfaces and constants
  *	
  *	Revision 1.16  2005/06/06 12:10:21  tschachim
  *	New SRF_ defines
  *	
  *	Revision 1.15  2005/05/26 16:00:31  tschachim
  *	New SRF constants, new physics constant
  *	
  *	Revision 1.14  2005/05/18 23:34:23  movieman523
  *	Added roughly correct masses for the various Saturn payloads.
  *	
  *	Revision 1.13  2005/05/12 00:16:51  movieman523
  *	Updated Apollo 13 explosion time.
  *	
  *	Revision 1.12  2005/05/05 21:38:51  tschachim
  *	some new constants
  *	
  *	Revision 1.11  2005/04/22 14:02:25  tschachim
  *	Surface defines
  *	Systems constants
  *	
  *	Revision 1.10  2005/04/14 23:15:22  movieman523
  *	Increased version number to 6.5.
  *	
  *	Revision 1.9  2005/04/01 14:12:40  tschachim
  *	nsurf = 28
  *	
  *	Revision 1.8  2005/03/28 05:48:42  chode99
  *	Added new payload type (Apollo 7 COAS target).
  *	
  *	Revision 1.7  2005/03/27 03:37:56  chode99
  *	Support for new type of test article payload (Apollo 8).
  *	
  *	Revision 1.6  2005/03/09 22:01:31  tschachim
  *	nsurf = 27
  *	
  *	Revision 1.5  2005/03/03 17:54:58  tschachim
  *	new font
  *	
  *	Revision 1.4  2005/02/24 00:10:05  movieman523
  *	Updated to version 6.4 and added extra digit to support more sub-versions up to x.x.9
  *	
  *	Revision 1.3  2005/02/19 00:01:57  movieman523
  *	Added more comments and course correction time setting.
  *	
  *	Revision 1.2  2005/02/12 13:27:03  movieman523
  *	Updated version number
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

#if !defined(_PA_NASSPDEFS_H)
#define _PA_NASSPDEFS_H

#define NASSP_VERSION 60050
#define REALISM_DEFAULT 5

typedef struct {
	HINSTANCE hDLL;
	HFONT font[3];
	DWORD col[6];
	HBRUSH brush[4];
	HPEN pen[5];
} GDIParams;

//
// Control stages.
//

#define ROLLOUT_STAGE				1
#define ONPAD_STAGE					3
#define PRELAUNCH_STAGE				5

#define LAUNCH_STAGE_ONE			11
#define LAUNCH_STAGE_TWO			12
#define LAUNCH_STAGE_TWO_ISTG_JET	13
#define LAUNCH_STAGE_TWO_TWR_JET	14

#define LAUNCH_STAGE_SIVB			20
#define STAGE_ORBIT_SIVB			21

#define CSM_LEM_STAGE				30

#define CM_STAGE					40
#define CM_ENTRY_STAGE				41
#define CM_ENTRY_STAGE_TWO			42
#define CM_ENTRY_STAGE_THREE		43
#define CM_ENTRY_STAGE_FOUR			44
#define CM_ENTRY_STAGE_FIVE			45
#define CM_ENTRY_STAGE_SIX			46
#define CM_ENTRY_STAGE_SEVEN		47
#define CM_RECOVERY_STAGE			48

#define CSM_ABORT_STAGE				50

//
// Start putting in defines rather than hard-coded numbers.
//

const int nsurf = 78; // number of bitmap handles

#define SRF_INDICATOR				 1
#define SRF_NEEDLE					 2
#define SRF_DIGITAL					 4
#define SRF_SWITCHUP				 6
#define SRF_SWITCHLEVER				 7
#define SRF_SWITCHGUARDS			 8
#define SRF_ABORT					 9
#define SRF_ALTIMETER				14
#define SRF_THRUSTMETER				16
#define SRF_SEQUENCERSWITCHES		17
#define SRF_LMTWOPOSLEVER           19
#define SRF_MASTERALARM_BRIGHT		20
#define SRF_DSKY					21
#define SRF_ALLROUND				22
#define SRF_THREEPOSSWITCH			23
#define SRF_MFDFRAME				24
#define SRF_MFDPOWER				25
#define SRF_ROTATIONALSWITCH		27
#define SRF_SUITCABINDELTAPMETER	28
#define SRF_THREEPOSSWITCH305   	29
#define SRF_LMABORTBUTTON			30
#define SRF_LMMFDFRAME				31
#define SRF_LMTHREEPOSLEVER         32
#define SRF_LMTHREEPOSSWITCH        33
#define SRF_DSKYDISP				34
#define SRF_FDAI				    35
#define SRF_FDAIROLL			    36
#define SRF_CWSLIGHTS				37
#define SRF_EVENT_TIMER_DIGITS		38
#define SRF_DSKYKEY					39
#define SRF_ECSINDICATOR			40
#define SRF_SWITCHUPSMALL			41
#define SRF_CMMFDFRAME				42
#define SRF_COAS					43
#define SRF_THUMBWHEEL_SMALLFONTS	44
#define SRF_CIRCUITBRAKER			45
#define SRF_THREEPOSSWITCH20		46
#define SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL	47
#define SRF_THREEPOSSWITCH30		48
#define SRF_SWITCH20				49
#define SRF_SWITCH30				50
#define SRF_CSMRIGHTWINDOWCOVER		51
#define SRF_SWITCH20LEFT			52
#define SRF_THREEPOSSWITCH20LEFT	53
#define SRF_GUARDEDSWITCH20			54
#define SRF_SWITCHGUARDPANEL15		55
#define SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT	56
#define SRF_THREEPOSSWITCH30LEFT	57
#define SRF_SWITCH30LEFT			58
#define SRF_THREEPOSSWITCH305LEFT	59
#define SRF_SWITCH305LEFT			60
#define SRF_LIGHTROTARY				61
#define SRF_FDAIPOWERROTARY			62
#define SRF_SCSBMAGROTARY			63
#define SRF_DIRECTO2ROTARY			64
#define SRF_ECSGLYCOLPUMPROTARY		65
#define SRF_GTACOVER				66
#define SRF_DCVOLTS					67
#define SRF_ACVOLTS					68
#define SRF_DCAMPS					69
#define SRF_POSTLDGVENTVLVLEVER		70
#define SRF_VHFANTENNAROTARY		71
#define SRF_SPSMAXINDICATOR			72
#define SRF_ECSROTARY 				73
#define SRF_CSMMAINPANELWINDOWCOVER	74
#define SRF_CSMRIGHTRNDZWINDOWLESCOVER 75
#define SRF_CSMLEFTWINDOWCOVER		76  
#define SRF_GLYCOLLEVER				77

//
// Earth radius and gravity constants.
//

static const double ERADIUS = 6371.0;
const double GK    = 6.67259e-20 ;			//  Gravitational constant (km^3/(kg sec^2))
const double GKSI  = 6.67259e-20 * 1e9;
const double PSI   = 0.000145038;			// Pa to PSI
const double MMHG  = 0.00750064;			// Pa to mmHg
const double INH2O = 0.00401474;			// Pa to in H2O
const double LBH   = 7.93665;				// g/s to lb/h

const double TWO_PI= (PI * 2);

//
// This is a safe 'minus infinity' time to use as the default value for timers. So if you compare your
// timer against MissionTime to decide whether to process events at a certain time interval, use this as
// the default and any mission time should be guaranteed to be greater.
//

const double MINUS_INFINITY = (-1000000000.0);

inline double KelvinToFahrenheit(double kelvin) {
	return kelvin * 1.8 - 459.67;
}

//
// Engine information.
//

#define SPS_THRUST		100552.5
#define SPS_ISP			3778.5
//#define APS_THRUST		15880
//#define APS_ISP			2921
#define APS_THRUST		15600
#define APS_ISP			2840
#define DPS_THRUST		44910
#define DPS_ISP			3107

#define RCS_FUEL_PER_QUAD	310
#define SM_RCS_ISP			(290.0 * G)
#define SM_RCS_ISP_SL		50.0
#define SM_RCS_THRUST		441.5

#define RCS_FUEL_CM			90.0
#define CM_RCS_ISP			(290.0 * G)
#define CM_RCS_ISP_SL		50.0
#define CM_RCS_THRUST		412.0

//
// Mission times for specific events.
//

#define APOLLO_13_EXPLOSION_TIME	(55.0 * 3600 + 55.0 + 10.0)

//
// We include a sizeable overlap here for the course correction earliest and latest times,
// to allow for people using high time accelerations.
//

#define COURSE_CORRECTION_START_TIME	219400
#define COURSE_CORRECTION_END_TIME	(COURSE_CORRECTION_START_TIME + 10000)

//
// Flags to show the mesh is visible externally and in the virtual cockpit.
//

#define MESHVIS_VCEXTERNAL	(MESHVIS_EXTERNAL|MESHVIS_VC)

//
// SIVB payloads.
//

#define PAYLOAD_LEM		0
#define PAYLOAD_ASTP	1
#define PAYLOAD_LTA		2
#define PAYLOAD_LM1		3
#define PAYLOAD_LTA8	4
#define PAYLOAD_TARGET	5
#define PAYLOAD_LTA6	6
#define PAYLOAD_EMPTY	7

//
// Internal systems.
//

#define CSM_H2TANK_CAPACITY  12700.0		// in g, 28 lb
#define CSM_O2TANK_CAPACITY 145149.0		// in g, 320 lb

#endif
