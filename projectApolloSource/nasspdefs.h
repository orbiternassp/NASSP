/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

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
  *	Revision 1.89  2007/11/24 21:28:46  jasonims
  *	EMS Implementation Step 1 - jasonims :   EMSdVSet Switch now works, preliminary EMS Scroll work being done.
  *	
  *	Revision 1.88  2007/10/18 00:23:20  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.87  2007/09/07 17:43:52  tschachim
  *	(Re)moved obsolete bitmaps.
  *	
  *	Revision 1.86  2007/08/20 20:57:18  tschachim
  *	(Re)moved obsolete bitmap.
  *	
  *	Revision 1.85  2007/08/13 16:06:12  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.84  2007/07/17 14:33:07  tschachim
  *	Added entry and post landing stuff.
  *	
  *	Revision 1.83  2007/06/06 15:02:15  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.82  2007/04/25 18:48:08  tschachim
  *	EMS dV functions.
  *	
  *	Revision 1.81  2007/01/22 14:45:32  tschachim
  *	New resources, fixed some CSM constants.
  *	
  *	Revision 1.80  2006/12/07 18:52:43  tschachim
  *	New LC34, Bugfixes.
  *	
  *	Revision 1.79  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.78  2006/09/23 01:55:58  flydba
  *	Final graphics update completed.
  *	
  *	Revision 1.77  2006/08/09 00:48:14  flydba
  *	COAS bitmap added.
  *	
  *	Revision 1.76  2006/07/21 23:04:34  movieman523
  *	Added Saturn 1b engine lights on panel and beginnings of electrical connector work (couldn't disentangle the changes). Be sure to get the config file for the SIVb as well.
  *	
  *	Revision 1.75  2006/07/16 17:41:49  flydba
  *	New ORDEAL rotary bitmap added.
  *	
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


//
// Define Doxygen groups. These just need to be documented in one place.
//
// Be sure to define them in the order you want them listed in the doxygen output.
//

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
/// \defgroup Astronauts Astronaut code.
/// \brief This code supports the astronauts, lunar rover, etc.
///

///
/// \defgroup AstronautSettings Astronauts settings.
/// \ingroup Astronauts
/// \brief This code supports setting the state of astronaut DLLs.
///

///
/// \defgroup Sound Audio support.
/// \brief Code to support audio playback, either directly through DirectSound indirectly
/// through OrbiterSound.
///

///
/// \defgroup Config Configuration code.
/// \brief Scenario configuration code.
///

///
/// \defgroup Connectors Connector code.
/// \brief This code is used to connect different objects together.
///

///
/// \defgroup PanelItems Control panel code.
/// \brief Objects which are used to support the control panel.
///

///
/// \defgroup DLLsupport DLL support.
/// \brief Objects and structures related to basic DLL support.
///

///
/// \defgroup Ground Ground support vehicle code.
/// \brief This code is for the ground support (ML, VAB, etc).
///

///
/// \defgroup InternalSystems Internal systems simulation.
/// \brief Objects which are used to simulate internal systems.
///

///
/// \defgroup DSKY Display/Keyboard code.
/// \ingroup InternalSystems
/// \brief This code supports the Apollo DSKY emulation.
///

///
/// \defgroup InternalInterface Interface to the internal systems code.
/// \ingroup InternalSystems
/// \brief General interface support for the internal systems.
/// This group contains functions, classes and structures used by general code to query the state of
/// internal systems without having to call the Panel SDK code directly. This is the preferred method
/// of querying state so that the low-level Panel SDK code can be revised without any changes to the
/// high-level code which makes use of the systems state information.
///
/// In general we've tried to put all related state into a single structure so that you can retrieve
/// it all in one call rather than having to make multiple calls for each system. This means, for
/// example, that the Caution and Warning system can retrieve the entire state of a spacecraft system
/// in one call and then check whether it should warn about that state (high/low voltage/pressure, etc).
///

///
/// \defgroup InternalSystemsState Internal systems state code.
/// \ingroup InternalSystems
/// \brief State variables for the internal systems.
///

///
/// \defgroup LVSystems Launch vehicle systems code.
/// \ingroup InternalSystems
/// \brief Simulation of systems specific to the launch vehicle (e.g. the Saturn Instrument Unit).
///

///
/// \defgroup PanelSDK Panel SDK code.
/// \ingroup InternalSystems
/// \brief The Panel SDK performs low-level simulation of the physical hardware.
///

///
/// \defgroup Propulsion Spacecraft propulsion systems code.
/// \ingroup InternalSystems
/// \brief Simulation of systems for and supporting spacecraft propulsion.
///

///
/// \defgroup LEM LEM vehicle code.
/// \brief This code supports the LEM.
///

///
/// \defgroup MFD MFD code.
/// \brief Project Apollo MFD code.
///

///
/// \defgroup Saturns Saturn launch vehicle code.
/// \brief This code supports the Saturn launch vehicles and CSM.
///

///
/// For simplicity we pack all the failure flags into unions which combine bitfields and 32-bit ints.
/// As a result we only need to save the int value to the scenario file, and when we load it we automatically
/// get all the individual flags without bothering to extract them manually from the int or save and load each
/// flag individually.
///
/// \defgroup FailFlags Failure flags.
/// \ingroup Saturns
/// \brief Collection of flags used to specify failures during the mission.
///

///
/// \defgroup SepStages Separated stages.
/// \ingroup Saturns
/// \brief This code supports separated stages of the Saturn launch vehicles and CSM.
///

///
/// \defgroup SepStageSettings Separated stage settings.
/// \ingroup SepStages
/// \brief This code supports setting the state of separated stages of the Saturn launch vehicles and CSM.
///

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
/// brushes and pens for rendering, etc. Typically the DLL has one global instance of this structure
/// which stores the relevant information.
///
/// \ingroup DLLsupport
/// \brief Windows GDI parameter structure.
///
typedef struct {
	HINSTANCE hDLL;		///< DLL handle.
	HFONT font[3];		///< GDI fonts.
	DWORD col[6];		///< GDI colors.
	HBRUSH brush[4];	///< GDI brushes.
	HPEN pen[5];		///< GDI pens.
} GDIParams;

//
// Control stages.
//

#define NULL_STAGE					0		///< Non-existent; should never happen.

#define ROLLOUT_STAGE				1		///< Rolling out from the VAB to the pad.
#define ONPAD_STAGE					3		///< On the pad.
#define PRELAUNCH_STAGE				5		///< Getting ready to launch.

#define LAUNCH_STAGE_ONE			11		///< Stage one of the launch.
#define LAUNCH_STAGE_TWO			12		///< Stage two of the launch, prior to interstage jettison (if appropriate).
#define LAUNCH_STAGE_TWO_ISTG_JET	13		///< Stage two of the launch, after interstage jettison.
#define LAUNCH_STAGE_TWO_TWR_JET	14		///< Stage two of the launch, after the automated tower jettison.

#define LAUNCH_STAGE_SIVB			20		///< SIVB burn during launch.
#define STAGE_ORBIT_SIVB			21		///< SIVB in orbit.

#define CSM_LEM_STAGE				30		///< CSM in space, optionally with LEM.

#define CM_STAGE					40		///< CM separated from SM.
#define CM_ENTRY_STAGE				41		///< CM in entry.
#define CM_ENTRY_STAGE_TWO			42
#define CM_ENTRY_STAGE_THREE		43
#define CM_ENTRY_STAGE_FOUR			44
#define CM_ENTRY_STAGE_FIVE			45
#define CM_ENTRY_STAGE_SIX			46
#define CM_ENTRY_STAGE_SEVEN		47
#define CM_RECOVERY_STAGE			48		///< CM in water waiting for recovery.

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
const int nsurf = 122; // number of bitmap handles

#define SRF_INDICATOR								 1
#define SRF_NEEDLE									 2
#define SRF_DIGITAL									 3
#define SRF_SWITCHUP								 4
#define SRF_SWITCHLEVER								 5
#define SRF_SWITCHGUARDS							 6
#define SRF_ABORT									 7
#define SRF_LV_ENG									 8
#define SRF_ALTIMETER								 9
#define SRF_THRUSTMETER								10
#define SRF_SEQUENCERSWITCHES						11
#define SRF_LMTWOPOSLEVER           				12
#define SRF_MASTERALARM_BRIGHT						13
#define SRF_DSKY									14
#define SRF_THREEPOSSWITCH							16
#define SRF_MFDFRAME								17
#define SRF_MFDPOWER								18
#define SRF_ROTATIONALSWITCH						19
#define SRF_SUITCABINDELTAPMETER					20
#define SRF_THREEPOSSWITCH305   					21
#define SRF_LMABORTBUTTON							22
#define SRF_LMMFDFRAME								23
#define SRF_LMTHREEPOSLEVER         				24
#define SRF_LMTHREEPOSSWITCH        				25
#define SRF_DSKYDISP								26
#define SRF_FDAI				    				27
#define SRF_FDAIROLL			    				28
#define SRF_CWSLIGHTS								29
#define SRF_EVENT_TIMER_DIGITS						30
#define SRF_DSKYKEY									31
#define SRF_ECSINDICATOR							32
#define SRF_SWITCHUPSMALL							33
#define SRF_CMMFDFRAME								34
#define SRF_COAS									35
#define SRF_THUMBWHEEL_SMALLFONTS					36
#define SRF_CIRCUITBRAKER							37
#define SRF_THREEPOSSWITCH20						38
#define SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL			39
#define SRF_THREEPOSSWITCH30						40
#define SRF_SWITCH20								41
#define SRF_SWITCH30								42
#define SRF_CSMRIGHTWINDOWCOVER						43
#define SRF_SWITCH20LEFT							44
#define SRF_THREEPOSSWITCH20LEFT					45
#define SRF_GUARDEDSWITCH20							46
#define SRF_SWITCHGUARDPANEL15						47
#define SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT		48
#define SRF_THREEPOSSWITCH30LEFT					49
#define SRF_SWITCH30LEFT							50
#define SRF_THREEPOSSWITCH305LEFT					51
#define SRF_SWITCH305LEFT							52
#define SRF_FDAIPOWERROTARY							54
#define SRF_DIRECTO2ROTARY							56
#define SRF_ECSGLYCOLPUMPROTARY						57
#define SRF_GTACOVER								58
#define SRF_DCVOLTS									59
#define SRF_ACVOLTS									60
#define SRF_DCAMPS									61
#define SRF_POSTLDGVENTVLVLEVER						62
#define SRF_SPSMAXINDICATOR							64
#define SRF_ECSROTARY 								65
#define SRF_CSMMAINPANELWINDOWCOVER					66
#define SRF_CSMRIGHTRNDZWINDOWLESCOVER 				67
#define SRF_CSMLEFTWINDOWCOVER						68  
#define SRF_GLYCOLLEVER								69
#define SRF_LEMROTARY								70
#define SRF_FDAIOFFFLAG								71
#define SRF_FDAINEEDLES								72
#define SRF_THUMBWHEEL_LARGEFONTS					73
#define SRF_SPS_FONT_WHITE							74
#define SRF_SPS_FONT_BLACK							75
#define SRF_BORDER_34x29							76
#define SRF_BORDER_34x61							77
#define SRF_BORDER_55x111							78
#define SRF_BORDER_46x75							79
#define SRF_BORDER_39x38							80
#define SRF_BORDER_92x40							81
#define SRF_BORDER_34x33							82
#define SRF_BORDER_29x29							83
#define SRF_BORDER_34x31							84
#define SRF_BORDER_50x158							85
#define SRF_BORDER_38x52							86
#define SRF_BORDER_34x34							87
#define SRF_BORDER_90x90							88
#define SRF_BORDER_84x84							89
#define SRF_BORDER_70x70							90
#define SRF_BORDER_23x20							91
#define SRF_BORDER_78x78							92
#define SRF_BORDER_32x160							93
#define SRF_BORDER_72x72							94
#define SRF_BORDER_75x64							95
#define SRF_BORDER_34x39							96
#define SRF_THUMBWHEEL_SMALL						97
#define SRF_THUMBWHEEL_LARGEFONTSINV				98
#define SRF_SWLEVERTHREEPOS							99
#define SRF_LEM_COAS1								100
#define SRF_ORDEAL_ROTARY							101
#define SRF_LV_ENG_S1B								102
#define SRF_LEM_COAS2								103
#define SRF_SPSMININDICATOR							104
#define SRF_SPS_INJ_VLV								105
#define SRF_SM_RCS_MODE								106
#define SRF_THUMBWHEEL_GPI_PITCH					107
#define SRF_THUMBWHEEL_GPI_YAW						108
#define SRF_THC										109
#define SRF_EMS_LIGHTS								110
#define SRF_SUITRETURN_LEVER						111
#define SRF_CABINRELIEFUPPERLEVER					112
#define SRF_CABINRELIEFLOWERLEVER					113
#define SRF_CABINRELIEFGUARDLEVER					114
#define SRF_OPTICS_HANDCONTROLLER					115
#define SRF_MARK_BUTTONS							116
#define SRF_THREEPOSSWITCHSMALL						117
#define SRF_OPTICS_DSKY								118
#define SRF_MINIMPULSE_HANDCONTROLLER				119
#define SRF_EMS_SCROLL_LEO							120
#define SRF_EMSDVSETSWITCH							121

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
const double FPS   = 3.2808399;				///< m/s to ft/s

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

#define SPS_THRUST					92100.0		// Apollo 7 Mission Report
#define SPS_ISP						 3080.0
#define SPS_DEFAULT_PROPELLANT		18500.0		// Apollo 11 Mission Report
#define SPS_NORM_OXIDIZER_FLOW		(1.6 / (1.0 + 1.6))

#define APS_THRUST		15600
#define APS_ISP			2840
#define DPS_THRUST		44910
#define DPS_ISP			3107

#define RCS_FUEL_PER_QUAD	152.5	// Apollo 11 Mission Report, AOH
#define SM_RCS_ISP			(290.0 * G)
#define SM_RCS_ISP_SL		50.0
#define SM_RCS_THRUST		441.5

#define CM_RCS_FUEL_PER_TANK	55.5	// The CM has 2 tanks (Apollo 11 Mission Report)
#define CM_RCS_ISP				(290.0 * G)
#define CM_RCS_ISP_SL			50.0
#define CM_RCS_THRUST			412.0

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

#define PAYLOAD_LEM					0	///< Payload is a LEM.
#define PAYLOAD_ASTP				1	///< Payload is an ASTP docking adapter.
#define PAYLOAD_LTA					2	///< Payload is an LTA.
#define PAYLOAD_LM1					3	///< Payload is LM1.
#define PAYLOAD_LTA8				4	///< Payload is LTA8.
#define PAYLOAD_TARGET				5	///< Payload is a docking target (e.g. Apollo 7).
#define PAYLOAD_LTA6				6	///< Payload is LTA6.
#define PAYLOAD_EMPTY				7	///< Payload is empty (i.e. no payload).
#define PAYLOAD_DOCKING_ADAPTER		8	///< Payload is SIVB docking adapter (i.e. Apollo to Venus).

//
// Saturn types.
//

#define SAT_UNKNOWN		0	///< Bad value, should never happen.
#define SAT_SATURNV		1	///< Vehicle is Saturn V.
#define SAT_SATURN1B	2	///< Vehicle is Saturn 1b.
#define SAT_SATURN1		3	///< Vehicle is Saturn 1.
#define SAT_INT20		4	///< Vehicle is Saturn INT20.

//
// Internal systems.
//

#define CSM_H2TANK_CAPACITY  12700.0		///< in g, 28 lb
#define CSM_O2TANK_CAPACITY 145149.0		///< in g, 320 lb

#endif
