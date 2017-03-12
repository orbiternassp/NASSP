/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: S1C class

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
// Data structure passed from main vessel to S1c to configure stage.
//

///
/// Flags structure indicating which of the S1c settings are valid.
///
/// \brief S1c settings flags.
/// \ingroup SepStageSettings
///
union S1cSettingFlags
{
	struct {
		unsigned S1C_SETTINGS_MASS:1; 		///< Mass settings are valid.
		unsigned S1C_SETTINGS_FUEL:1;		///< Fuel mass settings are valid.
		unsigned S1C_SETTINGS_GENERAL:1;	///< General settings (e.g. Mission Time) are valid.
		unsigned S1C_SETTINGS_ENGINES:1;	///< Engine settings (e.g. ISP) are valid.
	};
	unsigned int word;						///< Set to zero to clear all flags.

	///
	/// \brief Constructor: clear all flags by default.
	///
	S1cSettingFlags() { word = 0; };
};

///
/// Data structure passed from main vessel to S1c to configure it after staging.
///
/// \brief S1c setup structure.
/// \ingroup SepStageSettings
///
typedef struct {

	S1cSettingFlags SettingsType;			///< Which settings are valid?

	int VehicleNo;							///< Saturn vehicle number.
	int RetroNum;							///< Number of retros.
	int EngineNum;							///< Number of engines.

	double THRUST_FIRST_VAC;				///< Single-engine thrust in vacuum.
	double ISP_FIRST_VAC;					///< ISP in vacuum.
	double ISP_FIRST_SL;					///< ISP at sea-level.
	double MissionTime;						///< Current MET.
	double EmptyMass;						///< Empty mass in kg.
	double MainFuelKg;						///< Current fuel mass in kg.
	double CurrentThrust;					///< Current thrust level (0.0 to 1.0)

	bool LowRes;							///< Low-res meshes?
	bool S4Interstage;						///< Is the SIVb interstage attached?
	bool Stretched;							///< Is the stage stretched?

} S1CSettings;

//
// Stage states.
//

///
/// Specifies the main state of the S1c
///
/// \brief S1c state.
/// \ingroup SepStageSettings
///
enum S1cState
{
	SIC_STATE_HIDDEN = -1,				///< S1c is waiting for setup call.
	S1C_STATE_SHUTTING_DOWN,			///< S1c is firing motors to jettison.
	S1C_STATE_WAITING					///< S1c is idle after motor burnout.
};

///
/// This code simulates the S1c stage. Basically it simulates thrust decay if there is any fuel left,
/// fires any retro rockets to push it away from the Saturn and then sits around waiting to be deleted.
///
/// \brief S1c stage simulation.
/// \ingroup SepStages
///
class S1C : public VESSEL2 {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	S1C (OBJHANDLE hObj, int fmodel);
	virtual ~S1C();

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
	/// \brief Orbiter dock state function. Does the S1c need this?
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);
	bool clbkLoadGenericCockpit ();
	bool clbkLoadVC (int id);

	///
	/// Pass settings from the main DLL to the jettisoned S1c. This call must be virtual 
	/// so it can be called from other DLLs without building in the S1c code.
	/// \brief Setup jettisoned S1c.
	/// \param state S1c state settings.
	///
	virtual void SetState(S1CSettings &state);
	virtual void LoadMeshes(bool lowres);

protected:

	///
	/// \brief Orbiter sound library.
	///
#if 0
	/// \todo The sound initialization seems to take a lot of time (up to 5s on my system) 
	/// causing problems during creation of the S1C. Since the sound inside the S1C isn't very important
	/// it's disabled for the moment. 
	SoundLib soundlib;
#endif

	void SetS1c();
	void InitS1c();
	void AddEngines();
	void ShowS1c();

	int GetMainState();
	void SetMainState(int s);

	int MissionNo;
	int VehicleNo;
	S1cState State;
	int RetroNum;
	int EngineNum;

	bool RetrosFired;
	bool LowRes;
	bool S4Interstage;
	bool Stretched;

	double EmptyMass;
	double PayloadMass;
	double MainFuel;

	double MissionTime;
	double NextMissionEventTime;
	double LastMissionEventTime;

	double THRUST_FIRST_VAC;
	double ISP_FIRST_VAC;
	double ISP_FIRST_SL;
	double CurrentThrust;

	MESHHANDLE hsat5stg1, hsat5stg1low, hS4Interstage;
	THRUSTER_HANDLE th_retro[4], th_main[5];
	THGROUP_HANDLE thg_retro, thg_main;
	PROPELLANT_HANDLE ph_retro, ph_main;
};
