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
/// \defgroup ScenarioState Scenario state flags.
/// \ingroup Config
/// \brief Flags for scenario state.
///
/// For simplicity we pack various scenario flags into unions which combine bitfields and 32-bit ints.
/// As a result we only need to save the int value to the scenario file, and when we load it we automatically
/// get all the individual flags without bothering to extract them manually from the int or save and load each
/// flag individually.
///
/// For backwards compatibility, if you remove any of these flags, change its value to 'unusedX' where X is
/// an appropriate number to be unique in the structure; that way old scenarios won't break when we load the
/// old value.
///

///
/// \defgroup FailFlags Failure flags.
/// \ingroup ScenarioState
/// \brief Collection of flags used to specify failures during the mission.
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
///
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

#define NASSP_VERSION 80000		///< Current NASSP version.

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
	HPEN pen[7];		///< GDI pens.
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
// Start putting in defines rather than hard-coded numbers.
//

// All inline functions and const variables should be static, see
// http://www.orbitersim.com/Forum/Default.aspx?g=posts&m=172439#172439

//
// Earth radius and gravity constants.
//

static const double ERADIUS = 6371.0;
static const double GK    = 6.67259e-20 ;			///<  Gravitational constant (km^3/(kg sec^2))
static const double GKSI  = 6.67259e-20 * 1e9;
static const double PSI   = 0.000145038;			///< Pa to PSI
static const double MMHG  = 0.00750064;				///< Pa to mmHg
static const double INH2O = 0.00401474;				///< Pa to in H2O
static const double LBH   = 7.93665;				///< g/s to lb/h
static const double FPS   = 3.2808399;				///< m/s to ft/s
static const double LBS = 0.0022046226218;			///< g to lbs

static const double TWO_PI = (PI * 2.);				///< Twice Pi.

///
/// This is a safe 'minus infinity' time to use as the default value for timers. So if you compare your
/// timer against MissionTime to decide whether to process events at a certain time interval, use this as
/// the default and any mission time should be guaranteed to be greater.
///
/// \brief Minus infinity time for initialising MET variables safely.
///
static const double MINUS_INFINITY = (-1000000000.0);

///
/// \brief Convert Kelvin temperature to Fahrenheit temperature for cabin displays.
///
static inline double KelvinToFahrenheit(double kelvin) {
	return kelvin * 1.8 - 459.67;
}

///
/// \brief Convert Fahrenheit temperature to Kelvin temperature.
///
static inline double FahrenheitToKelvin(double fahrenheit) {
	return (fahrenheit + 459.67) / 1.8;
}

//
// Engine information.
//

#define SPS_THRUST					92100.0		// Apollo 7 Mission Report
#define SPS_ISP						 3080.0
#define SPS_DEFAULT_PROPELLANT		18500.0		// Apollo 11 Mission Report
#define SPS_NORM_OXIDIZER_FLOW		(1.6 / (1.0 + 1.6))
#define SPS_PITCH_OFFSET            -2.15		// in deg
#define SPS_YAW_OFFSET               0.95		// in deg

#define APS_THRUST		15297.43	// Apollo 11 Mission Report
#define APS_ISP			(308.8 * G)	// Apollo 11 Mission Report
#define DPS_THRUST		44910
#define DPS_ISP			3107
#define DPS_DEFAULT_PROPELLANT		8375.0

#define RCS_FUEL_PER_QUAD		152.5	// Apollo 11 Mission Report, AOH
#define PRIM_RCS_FUEL_PER_QUAD	93.5	// AOH
#define SM_RCS_ISP				(290.0 * G)
#define SM_RCS_ISP_SL			50.0
#define SM_RCS_THRUST			441.5

#define CM_RCS_FUEL_PER_TANK	55.5		// The CM has 2 tanks (Apollo 11 Mission Report)
#define CM_RCS_ISP				(340.0 * G) // AOH, dumping takes 88s with 10 of 12 thrusters
#define CM_RCS_ISP_SL			50.0
#define CM_RCS_THRUST			413.7		// AOH

#define LM_RCS_FUEL_PER_TANK	133.084001

#define S4B_APS_FUEL_PER_TANK	143.0
#define S4B_APS_THRUST			670.0
#define S4B_APS_ULL_THRUST		310.0
#define S4B_APS_ISP				(290.0 * G)
#define S4B_APS_ISP_SL			50.0

//
// Mission times for specific events.
//

///
/// \brief Mission Elapsed Time when Apollo 13 Oxygen tank exploded.
///
#define APOLLO_13_EXPLOSION_TIME	((55.0 * 3600.0) + (54.0 * 60.0) + 53.0)

//
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
