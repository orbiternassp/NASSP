/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SII class

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
// Which parts of the structure are active.
//

///
/// Flags structure indicating which of the SII settings are valid.
///
/// \brief SII settings flags.
/// \ingroup SepStageSettings
///
union SIISettingFlags
{
	struct {
		unsigned SII_SETTINGS_MASS:1; 		///< Mass settings are valid.
		unsigned SII_SETTINGS_FUEL:1;		///< Fuel mass settings are valid.
		unsigned SII_SETTINGS_GENERAL:1;	///< General settings (e.g. Mission Time) are valid.
		unsigned SII_SETTINGS_ENGINES:1;	///< Engine settings (e.g. ISP) are valid.
	};
	unsigned int word;						///< Set to zero to clear all flags.

	///
	/// \brief Constructor: clear all flags by default.
	///
	SIISettingFlags() { word = 0; };
};

///
/// Data structure passed from main vessel to SII to configure it after staging.
///
/// \brief SII setup structure.
/// \ingroup SepStageSettings
///
typedef struct {

	SIISettingFlags SettingsType;

	int VehicleNo;					///< Saturn vehicle number.
	int Realism;					///< Realism level.
	int RetroNum;					///< Number of retros.

	double THRUST_SECOND_VAC;		///< Thrust of each engine in vacuum (Newtons)
	double ISP_SECOND_VAC;			///< ISP in vacuum.
	double ISP_SECOND_SL;			///< ISP at sea level.
	double MissionTime;				///< Current MET.
	double EmptyMass;				///< Empty mass in kg.
	double MainFuelKg;				///< Remaining fuel in kg.
	double CurrentThrust;			///< Current engine thrust (0.0 to 1.0)

	bool LowRes;					///< Use low-res meshes?

} SIISettings;

///
/// Specifies the main state of the SII
///
/// \brief SII state.
/// \ingroup SepStageSettings
///
typedef enum SIIState
{
	SII_STATE_SETUP = -1,				///< SII is waiting for setup call.
	SII_STATE_SHUTTING_DOWN,			///< SII is firing motors to jettison.
	SII_STATE_WAITING					///< SII is idle after motor burnout.
};

///
/// This code simulates the seperated SII stage. Basically it simulates thrust decay if there is any fuel 
/// left, fires any retro rockets to push it away from the Saturn and then sits around waiting to be deleted.
///
/// \brief SII stage simulation.
/// \ingroup SepStages
///
class SII : public VESSEL2 {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	SII (OBJHANDLE hObj, int fmodel);
	virtual ~SII();

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
	/// \brief Orbiter dock state function. Does the SII need this?
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);

	///
	/// Pass settings from the main DLL to the jettisoned SII. This call must be virtual 
	/// so it can be called from other DLLs without building in the LES code.
	/// \brief Setup jettisoned SII.
	/// \param state SII state settings.
	///
	virtual void SetState(SIISettings &state);

protected:

	void SetSII();
	void InitSII();
	void AddEngines();

	int GetMainState();
	void SetMainState(int s);

	int MissionNo;
	int VehicleNo;
	SIIState State;
	int Realism;
	int RetroNum;

	bool RetrosFired;
	bool LowRes;

	double EmptyMass;
	double PayloadMass;
	double MainFuel;

	double MissionTime;
	double NextMissionEventTime;
	double LastMissionEventTime;

	double THRUST_SECOND_VAC;
	double ISP_SECOND_VAC;
	double ISP_SECOND_SL;
	double CurrentThrust;

	SURFHANDLE SMMETex;
	SURFHANDLE exhaust_tex;

	THRUSTER_HANDLE th_retro[4], th_main[5];
	THGROUP_HANDLE thg_retro, thg_main;
	PROPELLANT_HANDLE ph_retro, ph_main;
};
