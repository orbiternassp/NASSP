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
  *	Revision 1.1  2006/08/12 15:05:35  movieman523
  *	Renamed EVA and LEVA files.
  *	
  *	Revision 1.11  2006/08/12 14:47:57  movieman523
  *	Basic dox for LEVA.
  *	
  *	Revision 1.10  2006/08/12 14:14:18  movieman523
  *	Renamed EVA and LEVA classes, and added ApexCoverAttached flag to Saturn.
  *	
  *	Revision 1.9  2006/05/08 04:24:16  jasonims
  *	More LRV updates...
  *	
  *	Revision 1.8  2006/05/06 06:00:35  jasonims
  *	No more venting our Astronauts into space...and no more LRV popping out of an Astronauts pocket....well sorta.
  *	
  *	Revision 1.7  2006/04/22 03:53:48  jasonims
  *	Began initial support for multiple EVA's (two astronauts), as well as improving upon the LRV controls.  No longer turns while standing still.  Throttle controlled via (NUM+ and NUM-).
  *	
  *	Revision 1.6  2006/04/20 22:04:32  redburne
  *	New movement handling for LRV;
  *	LRV console now in separate mesh;
  *	Speed dial added.
  *	
  *	Revision 1.5  2006/04/17 14:23:23  redburne
  *	First working version of new 3d LRV console
  *	
  *	Revision 1.4  2006/01/22 18:08:11  redburne
  *	- timestep processing moved to VESSEL2 method clbkPreStep()
  *	- semi-realistic movement and turn speed values (as #defines)
  *	- turn speed now affected by time acceleration (but only tripled when acc is multiplied by 10)
  *	- LRV mesh visible from cockpit view (camera at driver head)
  *	
  *	Revision 1.3  2006/01/19 14:56:05  tschachim
  *	Initial Meshland support.
  *	
  *	Revision 1.2  2005/02/24 00:28:24  movieman523
  *	Revised for Orbitersound 3 and new Orbiter.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

///
/// \ingroup AstronautSettings
/// \brief LEVA settings.
///
typedef struct {

	int MissionNo;			///< Apollo mission number.
	int Realism;			///< Realism level.

} LEVASettings;

///
/// \ingroup Astronauts
/// \brief Lunar EVA astronaut.
///
class LEVA: public VESSEL2 {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	LEVA(OBJHANDLE hObj, int fmodel);
	virtual ~LEVA();

	///
	/// \brief Orbiter timestep function.
	/// \param SimT Current simulation time, in seconds since Orbiter was started.
	/// \param SimDT Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPreStep (double SimT, double SimDT, double mjd);

	///
	/// \brief Orbiter keyboard input function.
	/// \param kstate Key state.
	///
	int clbkConsumeDirectKey(char *kstate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);

	///
	/// \brief Orbiter class configuration function.
	/// \param cfg File to load configuration defaults from.
	///
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkVisualCreated (VISHANDLE vis, int refcount);
	void clbkVisualDestroyed (VISHANDLE vis, int refcount);

	void SetAstroStage ();

	///
	/// \brief Initialise state.
	///
	void init();

	///
	/// \brief State loading function.
	/// \param scn Scenario file to load from.
	/// \param vs Pointer to current vessel status.
	///
	void LoadState(FILEHANDLE scn, VESSELSTATUS *vs);

	///
	/// \brief State saving function.
	/// \param scn Scenario file to save to.
	///
	void SaveState(FILEHANDLE scn);

	///
	/// Set up the astronaut state. Virtual so it can be called from the LEM without needing to link in all the code
	/// to process the function.
	/// \brief EVA state initialisation.
	/// \param evas EVA state structure.
	//
	virtual void SetEVAStats(LEVASettings &evas);

private:

	void ScanMotherShip();
	void MoveEVA(double SimDT, VESSELSTATUS *eva, double heading);
	void SetFlag();
	void SetMissionPath();
	void DoFirstTimestep();
	void SetMainState(int s);
	int GetMainState();

protected:

	int ApolloNo;
	int StepCount;
	int Realism;

	OBJHANDLE hMaster;
	bool GoDock1;
	bool starthover;
	bool LRVDeployed;
	void ToggleLRV();
	bool Astro;						
	bool MotherShip;
	char EVAName[256];
	char LEMName[256];
	char MSName[256];
	bool KEY1;
	bool KEY2;
	bool KEY3;
	bool KEY4;
	bool KEY5;
	bool KEY6;
	bool KEY7;
	bool KEY8;
	bool KEY9;
	bool KEYADD;
	bool KEYSUBTRACT;
	bool GoFlag;		 
	bool FlagPlanted;

	bool FirstTimestep;
	bool SLEVAPlayed;
	bool StateSet;

	bool lastLatLongSet;
	double lastLat;
	double lastLong;

	double speed;		///< current speed in m/s

	SoundLib soundlib;
	Sound FlagSound;
	Sound SLEVA;

	// touchdown point test
	// double touchdownPointHeight;

};
