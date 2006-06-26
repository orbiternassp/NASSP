/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SIVb class

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
  *	Revision 1.9  2006/06/25 21:19:45  movieman523
  *	Lots of Doxygen updates.
  *	
  *	Revision 1.8  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.7  2006/01/26 03:07:50  movieman523
  *	Quick hack to support low-res mesh.
  *	
  *	Revision 1.6  2005/11/24 20:31:23  movieman523
  *	Added support for engine thrust decay during launch.
  *	
  *	Revision 1.5  2005/11/20 01:06:27  movieman523
  *	Saturn V now uses SIVB DLL too.
  *	
  *	Revision 1.4  2005/11/19 22:58:32  movieman523
  *	Pass main fuel mass from Saturn 1b to SIVb and added main thrust from venting fuel.
  *	
  *	Revision 1.3  2005/11/19 22:19:07  movieman523
  *	Revised interface to update SIVB, and added payload mass and stage empty mass.
  *	
  *	Revision 1.2  2005/11/19 22:05:16  movieman523
  *	Added RCS to SIVb stage after seperation.
  *	
  *	Revision 1.1  2005/11/19 21:27:31  movieman523
  *	Initial SIVb implementation.
  *	
  **************************************************************************/

//
// Data structure passed from main vessel to SIVB to configure stage.
//

///
/// Flags structure indicating which of the SIVB settings are valid.
///
/// \brief SIVB settings flags.
/// \ingroup SepStageSettings
///
union SIVbSettingFlags
{
	struct {
		unsigned SIVB_SETTINGS_MASS:1; 		///< Mass settings are valid.
		unsigned SIVB_SETTINGS_FUEL:1;		///< Fuel mass settings are valid.
		unsigned SIVB_SETTINGS_GENERAL:1;	///< General settings (e.g. Mission Time) are valid.
		unsigned SIVB_SETTINGS_PAYLOAD:1;	///< Payload settings are valid.
	};
	unsigned int word;						///< Set to zero to clear all flags.

	///
	/// \brief Constructor: clear all flags by default.
	///
	SIVbSettingFlags() { word = 0; };
};

///
/// Data structure passed from main vessel to SIVb to configure it after staging.
///
/// \brief SIVb setup structure.
/// \ingroup SepStageSettings
///
typedef struct 
{
	SIVbSettingFlags SettingsType;	///> Which settings are valid?

	int Payload;					///> Payload type.
	int VehicleNo;					///> Saturn vehicle number.
	int Realism;					///> Realism level.

	double MissionTime;				///> Current MET in seconds.
	double EmptyMass;				///> Empty mass in kg.
	double PayloadMass;				///> Payload mass in kg.
	double ApsFuelKg;				///> APS fuel in kg.
	double MainFuelKg;				///> Remaining fuel in kg.

	bool PanelsHinged;				///> Are SLA panels hinged?
	bool SaturnVStage;				///> Saturn V stage or Saturn 1b stage?
	bool LowRes;					///> Low-res meshes?
} SIVBSettings;


///
/// Specifies the main state of the SIVb
///
/// \brief SIVb state.
/// \ingroup SepStageSettings
///
typedef enum SIVbState
{
	SIVB_STATE_SETUP = -1,				///< SII is waiting for setup call.
	SIVB_STATE_WAITING					///< SII is idle after motor burnout.
};

///
/// This code simulates the seperated SIVb stage. Basically it simulates thrust decay if there is any fuel 
/// left, fires any retro rockets to push it away from the Saturn and then sits around waiting to be deleted.
///
/// \brief SIVb stage simulation.
/// \ingroup SepStages
///
class SIVB : public VESSEL2 {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	SIVB (OBJHANDLE hObj, int fmodel);
	virtual ~SIVB();

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
	/// \brief Orbiter dock state function.
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);

	///
	/// Pass settings from the main DLL to the jettisoned SIVb. This call must be virtual 
	/// so it can be called from other DLLs without building in the LES code.
	/// \brief Setup jettisoned SIVb.
	/// \param state SIVb state settings.
	///
	virtual void SetState(SIVBSettings &state);

protected:

	void SetS4b();

	///
	/// \brief Set SIVb state.
	///
	void InitS4b();

	int GetMainState();
	void SetMainState(int s);
	void GetApolloName(char *s);
	void AddRCS_S4B();				///< Add RCS for SIVb control.
	void Boiloff();					///< Boil off some LOX/LH2 in orbit.

	int Payload;					///< Payload type.
	int MissionNo;					///< Apollo mission number.
	int VehicleNo;					///< Saturn vehicle number.
	SIVbState State;				///< Main stage state.
	int Realism;					///< Realism level.

	double EmptyMass;				///< Empty mass in kg.
	double PayloadMass;				///< Payload mass in kg.
	double MainFuel;				///< Main fuel mass in kg.

	double MissionTime;				///< Current MET in seconds.
	double NextMissionEventTime;	///< Next event time for automated operation.
	double LastMissionEventTime;	///< Last event time.

	bool PanelsHinged;				///< SLA panels are hinged.
	bool PanelsOpened;				///< SLA Panels are open.
	bool SaturnVStage;				///< Stage from Saturn V.
	bool LowRes;					///< Using low-res meshes.

	double CurrentThrust;			///< Current thrust level (0.0 to 1.0).

	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;

	DOCKHANDLE hDock;

	THRUSTER_HANDLE th_att_rot[10], th_main[1], th_att_lin[2];                 // handles for APS engines
	THGROUP_HANDLE thg_aps, thg_main;
	PROPELLANT_HANDLE ph_aps, ph_main;
};
