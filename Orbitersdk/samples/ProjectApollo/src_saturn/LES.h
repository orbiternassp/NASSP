/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LES class

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

#ifndef LES_H
#define LES_H

///
/// Flags structure indicating which of the LES settings are valid.
/// \brief LES settings flags.
/// \ingroup SepStageSettings
///
typedef union
{
	struct {
		unsigned LES_SETTINGS_MASS:1; 		///< Mass settings are valid.
		unsigned LES_SETTINGS_FUEL:1;		///< Fuel mass settings are valid.
		unsigned LES_SETTINGS_GENERAL:1;	///< General settings (e.g. Mission Time) are valid.
		unsigned LES_SETTINGS_ENGINES:1;	///< Engine settings (e.g. ISP) are valid.
		unsigned LES_SETTINGS_THRUST:1;		///< Thrust setting is valid.
		unsigned LES_SETTINGS_MAIN_FUEL:1;	///< Main fuel setting is valid, jettison fuel setting is not valid.
	};
	unsigned int word;						///< Set to zero to clear all flags.
} LESSettingFlags;

///
/// Data structure passed from main vessel to LES to configure it after staging.
/// \brief LES setup structure.
/// \ingroup SepStageSettings
///
typedef struct 
{
	LESSettingFlags SettingsType;		///< Which of the settings are valid?
	int VehicleNo;						///< Saturn vehicle number.

	double ISP_LET_VAC;					///< LET ISP in a vacuum
	double ISP_LET_SL;					///< LET ISP at sea level.

	double THRUST_VAC_LET;				///< LET thrust in vacuum.

	double MissionTime;					///< Current Mission Elapsed Time.
	double EmptyMass;					///< Empty mass of LET without fuel.

	double MainFuelKg;					///< Amount of fuel in kg available for the main abort engine.
	double JettisonFuelKg;				///< Amount of fuel in kg available for the jettison engine.

	bool ProbeAttached;					///< The docking probe is attached to the LES (i.e. this is an abort jettison).
	bool FireMain;						///< Fire the main abort motors rather than jettison motor?
	bool LowRes;						///< Use low-res mesh if available?
} LESSettings;

///
/// Specifies the main state of the LES.
/// \brief LES state.
/// \ingroup SepStageSettings
///
enum LESState
{
	LES_STATE_SETUP,			///< LES is waiting for setup call.
	LES_STATE_JETTISON,			///< LES is firing motors to jettison.
	LES_STATE_WAITING			///< LES is idle after motor burnout.
};

//
// LES stage class.
//

///
/// This code simulates the jettisoned Launch Escape Tower and Boost Protective Cover. Basically
/// it just fires the appropriate motor to push it away from the Saturn and then sits around waiting
/// to be deleted.
///
/// \brief Launch Escape System simulation.
/// \ingroup SepStages
///
class LES : public VESSEL2 {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	LES (OBJHANDLE hObj, int fmodel);
	virtual ~LES();

	///
	/// \brief Orbiter state saving function.
	/// \param scn Scenario file to save to.
	///
	void clbkSaveState (FILEHANDLE scn);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPreStep(double simt, double simdt, double mjd);

	///
	/// \brief Orbiter state loading function.
	/// \param scn Scenario file to load from.
	/// \param status Pointer to current vessel status.
	///
	void clbkLoadStateEx (FILEHANDLE scn, void *status);

	///
	/// \brief Orbiter class configuration function.
	/// \param cfg File to load configuration defaults from.
	///
	void clbkSetClassCaps (FILEHANDLE cfg);

	///
	/// \brief Orbiter dock state function. Does the LES need this?
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);

	///
	/// Pass settings from the main DLL to the jettisoned LES. This call must be virtual 
	/// so it can be called from other DLLs without building in the LES code.
	/// \brief Setup jettisoned LES.
	/// \param state LES state settings.
	///
	virtual void SetState(LESSettings &state);

protected:

	///
	/// Set LES meshes and propellant resources.
	/// \brief Setup LES vessel.
	///
	void SetLES();

	///
	/// Set everything to a default state.
	/// \brief Init the LES object.
	///
	void InitLES();

	///
	/// Add the Orbiter thrusters to the LES and wire them up to propellants.
	/// \brief Add the motors.
	///
	void AddEngines();

	///
	/// Get the main state array to save to a scenario file.
	/// \brief Get the main state values.
	/// \return An integer holding an array of state flags.
	///
	int GetMainState();

	///
	/// Set the main state based on an array of bits read from a scenario file.
	/// \brief Set the main state values.
	/// \param s An integer holding an array of state flags.
	///
	void SetMainState(int s);

	///
	/// \brief Apollo vehicle number.
	///
	int VehicleNo;

	///
	/// \brief Current state for automated operations.
	///
	LESState State;

	///
	/// \brief Flag use of low-res meshes if possible, to reduce graphics lag.
	///
	bool LowRes;

	///
	/// \brief Docking probe was detached with the LES in an abort jettison.
	///
	bool ProbeAttached;

	///
	/// Normally the LES is jettisoned using the special jettison motor whose thrust is high enough to
	/// pull the tower and BPC away from the CM, but not enough to abort safely with the CM attached. If
	/// the jettison motor fails, then the backup is to jettison the LES with the main abort motor, which
	/// gives it an acceleration of over 10g!
	/// \brief Fire main abort motor rather than jettison motor?
	///
	bool FireMain;

	///
	/// \brief Empty mass of the LES, in kilograms.
	///
	double EmptyMass;

	///
	/// \brief Total fuel mass of the main abort motor in kilograms.
	///
	double MainFuel;

	///
	/// \brief Total fuel mass of the jettison motor in kilograms.
	///
	double JettisonFuel;

	///
	/// The current Mission Elapsed Time. This is the main variable used for timing
	/// automated events during the mission, giving the time in seconds from launch
	/// (negative for the pre-launch countdown).
	/// \brief Mission Elapsed Time.
	///
	double MissionTime;

	///
	/// The time in seconds of the next automated event to occur in the mission. This 
	/// is a generic value used by the autopilot and staging code.
	/// \brief Time of next event.
	///
	double NextMissionEventTime;

	///
	/// The time in seconds of the previous automated event that occur in the mission. This 
	/// is a generic value used by the autopilot and staging code.
	/// \brief Time of last event.
	///
	double LastMissionEventTime;

	///
	/// \brief Thrust of the main abort motor in a vacuum, in Newtons.
	///
	double THRUST_VAC_MAIN;

	///
	/// \brief Thrust of the jettison motor in a vacuum, in Newtons.
	///
	double THRUST_VAC_JETTISON;

	///
	/// \brief ISP of the motor we're using, in a vacuum.
	///
	double ISP_VAC;

	///
	/// \brief ISP of the motor we're using, at sea level.
	///
	double ISP_SL;

	///
	/// \brief Jettison motor thrusters.
	///
	THRUSTER_HANDLE th_jettison[2];

	///
	/// \brief Main abort motor thrusters.
	///
	THRUSTER_HANDLE th_main[4];

	///
	/// \brief Thruster group for the motor we're using.
	///
	THGROUP_HANDLE thg_main;

	///
	/// \brief Jettison thruster propellant.
	///
	PROPELLANT_HANDLE ph_jettison;

	///
	/// \brief Main abort motor propellant.
	///
	PROPELLANT_HANDLE ph_main;
};

#endif // LES_H
