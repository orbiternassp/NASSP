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
  *	Revision 1.74  2006/07/16 02:54:10  flydba
  *	New COAS bitmap added.
  *	
  *	Revision 1.73  2006/07/07 19:44:58  movieman523
  *	First version of connector support.
  *	
  *	Revision 1.72  2006/07/06 02:13:07  movieman523
  *	First pass at Apollo to Venus orbital test flight.
  *	
  *	Revision 1.71  2006/07/01 23:49:13  movieman523
  *	Updated more documentation.
  *	
  *	Revision 1.70  2006/06/26 19:05:36  movieman523
  *	More doxygen, made Lunar EVA a VESSEL2, made SM breakup, made LRV use VESSEL2 save/load functions.
  *	
  *	Revision 1.69  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.68  2006/06/12 20:47:36  movieman523
  *	Made switch lighting optional based on REALISM, and fixed SII SEP light.
  *	
  *	Revision 1.67  2006/06/10 23:27:41  movieman523
  *	Updated abort code.
  *	
  *	Revision 1.66  2006/06/08 15:30:18  tschachim
  *	Fixed ASCP and some default switch positions.
  *	
  *	Revision 1.65  2006/06/07 09:53:19  tschachim
  *	Improved ASCP and GDC align button, added cabin closeout sound, bugfixes.
  *	
  *	Revision 1.64  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.63  2006/05/27 00:54:28  movieman523
  *	Simplified Saturn V mesh code a lot, and added beginnings ot INT-20.
  *	
  *	Revision 1.62  2006/05/15 22:56:53  movieman523
  *	Added VC2005 solution and project files.
  *	
  *	Revision 1.61  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  *	Revision 1.60  2006/04/17 15:16:16  movieman523
  *	Beginnings of checklist code, added support for flashing borders around control panel switches and updated a portion of the Saturn panel switches appropriately.
  *	
  *	Revision 1.59  2006/03/12 01:13:28  dseagrav
  *	Added lots of SCS items and FDAI stuff.
  *	
  *	Revision 1.58  2006/03/09 00:27:25  movieman523
  *	Added SPS fuel quantity gauges
  *	
  *	Revision 1.57  2006/03/07 02:19:41  flydba
  *	Circuit breakers not visible, tried to solve the problem.
  *	
  *	Revision 1.56  2006/03/04 22:50:52  dseagrav
  *	Added FDAI RATE logic, SPS TVC travel limited to 5.5 degrees plus or minus, added check for nonexistent joystick selection in DirectInput code. I forgot to date most of these.
  *	
  *	Revision 1.55  2006/03/04 20:55:59  flydba
  *	Circuit breaker resource for the LEM panels added to the panel code.
  *	
  *	Revision 1.54  2006/02/01 18:12:41  tschachim
  *	New surface SRF_FDAIOFFFLAG.
  *	
  *	Revision 1.53  2006/01/22 16:37:20  flydba
  *	New bitmap added.
  *	
  *	Revision 1.52  2006/01/14 12:33:04  flydba
  *	New panel added (325/326) for cabin press control.
  *	
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

//
// Compatibility warning crap.
//

#if _MSC_VER > 1300
#define strnicmp _strnicmp
#define stricmp _stricmp
#endif 

#define NASSP_VERSION 70000		///< Current NASSP version.

//
// Realism values.
//

#define REALISM_MIN		0		///< Lower REALISM limit.
#define REALISM_MAX		10		///< Highest REALISM limit.
#define REALISM_DEFAULT 5		///< Default REALISM value.
#define REALISM_PUSH_LIGHTS	4	///< REALISM level up to which we display lit push-buttons on the control panel.

///
/// We use this structure to store generic Windows information in one place, such as logical colors,
/// bruhes and pens for rendering, etc.
///
/// \brief Windows GDI paramater structure.
///
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

#define NULL_STAGE					0

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

//
// CSM_ABORT_STAGE is only for backward compatibility.
// It can be removed soon.
//

///
/// \deprecated
/// \brief CSM is aborting.
///
#define CSM_ABORT_STAGE				50

//
// Start putting in defines rather than hard-coded numbers.
//

//
// Remember that we start the surface count at 1, so nsurf must be one
// more than the highest value here.
//
const int nsurf = 111; // number of bitmap handles

#define SRF_INDICATOR								 1
#define SRF_NEEDLE									 2
#define SRF_DIGITAL									 4
#define SRF_SWITCHUP								 6
#define SRF_SWITCHLEVER								 7
#define SRF_SWITCHGUARDS							 8
#define SRF_ABORT									 9
#define SRF_ALTIMETER								14
#define SRF_THRUSTMETER								16
#define SRF_SEQUENCERSWITCHES						17
#define SRF_LMTWOPOSLEVER           				19
#define SRF_MASTERALARM_BRIGHT						20
#define SRF_DSKY									21
#define SRF_ALLROUND								22
#define SRF_THREEPOSSWITCH							23
#define SRF_MFDFRAME								24
#define SRF_MFDPOWER								25
#define SRF_ROTATIONALSWITCH						27
#define SRF_SUITCABINDELTAPMETER					28
#define SRF_THREEPOSSWITCH305   					29
#define SRF_LMABORTBUTTON							30
#define SRF_LMMFDFRAME								31
#define SRF_LMTHREEPOSLEVER         				32
#define SRF_LMTHREEPOSSWITCH        				33
#define SRF_DSKYDISP								34
#define SRF_FDAI				    				35
#define SRF_FDAIROLL			    				36
#define SRF_CWSLIGHTS								37
#define SRF_EVENT_TIMER_DIGITS						38
#define SRF_DSKYKEY									39
#define SRF_ECSINDICATOR							40
#define SRF_SWITCHUPSMALL							41
#define SRF_CMMFDFRAME								42
#define SRF_COAS									43
#define SRF_THUMBWHEEL_SMALLFONTS					44
#define SRF_CIRCUITBRAKER							45
#define SRF_THREEPOSSWITCH20						46
#define SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL			47
#define SRF_THREEPOSSWITCH30						48
#define SRF_SWITCH20								49
#define SRF_SWITCH30								50
#define SRF_CSMRIGHTWINDOWCOVER						51
#define SRF_SWITCH20LEFT							52
#define SRF_THREEPOSSWITCH20LEFT					53
#define SRF_GUARDEDSWITCH20							54
#define SRF_SWITCHGUARDPANEL15						55
#define SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT		56
#define SRF_THREEPOSSWITCH30LEFT					57
#define SRF_SWITCH30LEFT							58
#define SRF_THREEPOSSWITCH305LEFT					59
#define SRF_SWITCH305LEFT							60
#define SRF_LIGHTROTARY								61
#define SRF_FDAIPOWERROTARY							62
#define SRF_SCSBMAGROTARY							63
#define SRF_DIRECTO2ROTARY							64
#define SRF_ECSGLYCOLPUMPROTARY						65
#define SRF_GTACOVER								66
#define SRF_DCVOLTS									67
#define SRF_ACVOLTS									68
#define SRF_DCAMPS									69
#define SRF_POSTLDGVENTVLVLEVER						70
#define SRF_VHFANTENNAROTARY						71
#define SRF_SPSMAXINDICATOR							72
#define SRF_ECSROTARY 								73
#define SRF_CSMMAINPANELWINDOWCOVER					74
#define SRF_CSMRIGHTRNDZWINDOWLESCOVER 				75
#define SRF_CSMLEFTWINDOWCOVER						76  
#define SRF_GLYCOLLEVER								77
#define SRF_LEMROTARY								78
#define SRF_FDAIOFFFLAG								79
#define SRF_FDAINEEDLES								80
#define SRF_CIRCUITBRAKERLEM						81
#define SRF_THUMBWHEEL_LARGEFONTS					82
#define SRF_SPS_FONT_WHITE							83
#define SRF_SPS_FONT_BLACK							84
#define SRF_BORDER_34x29							85
#define SRF_BORDER_34x61							86
#define SRF_BORDER_55x111							87
#define SRF_BORDER_44x67							88
#define SRF_BORDER_39x38							89
#define SRF_BORDER_92x40							90
#define SRF_BORDER_34x33							91
#define SRF_BORDER_29x29							92
#define SRF_BORDER_34x31							93
#define SRF_BORDER_50x158							94
#define SRF_BORDER_38x49							95
#define SRF_BORDER_34x34							96
#define SRF_BORDER_90x90							97
#define SRF_BORDER_84x84							98
#define SRF_BORDER_70x70							99
#define SRF_BORDER_23x20							100
#define SRF_BORDER_78x78							101
#define SRF_BORDER_32x160							102
#define SRF_BORDER_72x72							103
#define SRF_BORDER_75x64							104
#define SRF_BORDER_34x39							105
#define SRF_THUMBWHEEL_SMALL						106
#define SRF_THUMBWHEEL_LARGEFONTSINV				107
#define SRF_SWLEVERTHREEPOS							108
#define SRF_LEM_COAS1								109
#define SRF_ORDEAL_ROTARY							110

//
// Earth radius and gravity constants.
//

static const double ERADIUS = 6371.0;
const double GK    = 6.67259e-20 ;			///<  Gravitational constant (km^3/(kg sec^2))
const double GKSI  = 6.67259e-20 * 1e9;
const double PSI   = 0.000145038;			///< Pa to PSI
const double MMHG  = 0.00750064;			///< Pa to mmHg
const double INH2O = 0.00401474;			///< Pa to in H2O
const double LBH   = 7.93665;				///< g/s to lb/h

const double TWO_PI= (PI * 2);				///< Twice Pi.

///
/// This is a safe 'minus infinity' time to use as the default value for timers. So if you compare your
/// timer against MissionTime to decide whether to process events at a certain time interval, use this as
/// the default and any mission time should be guaranteed to be greater.
///
/// \brief Minus infinity time for initialising MET variables safely.
///
const double MINUS_INFINITY = (-1000000000.0);

///
/// \brief Convert Kelvin temperature to Fahrenheit temperature for cabin displays.
///
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

///
/// \brief Mission Elapsed Time when Apollo 13 Oxygen tank exploded.
///
#define APOLLO_13_EXPLOSION_TIME	((55.0 * 3600.0) + (55.0 * 60.0) + 10.0)

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

#define PAYLOAD_LEM					0
#define PAYLOAD_ASTP				1
#define PAYLOAD_LTA					2
#define PAYLOAD_LM1					3
#define PAYLOAD_LTA8				4
#define PAYLOAD_TARGET				5
#define PAYLOAD_LTA6				6
#define PAYLOAD_EMPTY				7
#define PAYLOAD_DOCKING_ADAPTER		8

//
// Saturn types.
//

#define SAT_UNKNOWN		0
#define SAT_SATURNV		1
#define SAT_SATURN1B	2
#define SAT_SATURN1		3
#define SAT_INT20		4

//
// Internal systems.
//

#define CSM_H2TANK_CAPACITY  12700.0		// in g, 28 lb
#define CSM_O2TANK_CAPACITY 145149.0		// in g, 320 lb

//
// Define Doxygen groups. These just need to be documented in one place.
//

///
/// \defgroup PanelItems Control panel code.
/// \brief Objects which are used to support the control panel.
///

///
/// \defgroup InternalSystems Internal systems simulation.
/// \brief Objects which are used to simulate internal systems.
///

///
/// \defgroup PanelSDK Panel SDK code.
/// \ingroup InternalSystems
/// \brief The Panel SDK performs low-level simulation of the physical hardware.
///

///
/// \defgroup AGC Apollo Guidance Computer code.
/// \ingroup InternalSystems
/// \brief This code supports the Apollo Guidance Computers on the CSM and LEM.
///

///
/// Flagword definitions as used by the real AGC software. These are really intended 
/// for future expansion as the AGC simulation gets closer to the real thing.
/// For the moment only a few of these flags are actually used.
///
/// Note that some flags marked as unused weren't used on the real AGC but are used to
/// store new flags for the C++ AGC.
///
/// See the Apollo 15 Delco manual for full explanations of what these flags are used
/// for in the real AGC.
///
/// \defgroup AGCFlags Apollo Guidance Computer flag words.
/// \ingroup AGC
/// \brief Flagword definitions for the Apollo Guidance Computers on the CSM and LEM.
///

///
/// Bit definitions for the AGC I/O channels to simplify conversion between channel values
/// and function.
///
/// \defgroup AGCIO Apollo Guidance Computer I/O channels.
/// \ingroup AGC
/// \brief I/O definitions for the Apollo Guidance Computers on the CSM and LEM.
///

///
/// \defgroup DSKY Display/Keyboard code.
/// \ingroup InternalSystems
/// \brief This code supports the Apollo DSKY emulation.
///

///
/// \defgroup InternalSystemsState Internal systems state code.
/// \ingroup InternalSystems
/// \brief State variables for the internal systems.
///

///
/// \defgroup Saturns Saturn launch vehicle code.
/// \brief This code supports the Saturn launch vehicles and CSM.
///

///
/// \defgroup SepStages Seperated stages.
/// \ingroup Saturns
/// \brief This code supports seperated stages of the Saturn launch vehicles and CSM.
///

///
/// For simplicity we pack all the failure flags into uniont which combine bitfields and 32-bit ints.
/// As a result we only need to save the int value to the scenario file, and when we load it we automatically
/// get all the individual flags without bothering to extract them manually from the int or save and load each
/// flag individually.
///
/// \defgroup FailFlags Failure flags.
/// \ingroup Saturns
/// \brief Collection of flags used to specify failures during the mission.
///

///
/// \defgroup SepStageSettings Seperated stage settings.
/// \ingroup SepStages
/// \brief This code supports setting the state of seperated stages of the Saturn launch vehicles and CSM.
///

///
/// \defgroup Astronauts Astronaut code.
/// \brief This code supports the astronauts, lunar rover, etc.
///

///
/// \defgroup AstronautSettings Astronauts settings.
/// \ingroup Astronauts
/// \brief This code supports setting the state of astronaut DLLs.
///

///
/// \defgroup LEM LEM vehicle code.
/// \brief This code supports the LEM.
///

///
/// \defgroup Ground Ground support vehicle code.
/// \brief This code is for the ground support (ML, VAB, etc).
///

///
/// \defgroup Connectors Connector code.
/// \brief This code is used to connect different objects together.
///
#endif
