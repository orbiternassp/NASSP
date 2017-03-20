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

  **************************************************************************/

///
/// \ingroup AstronautSettings
/// \brief LEVA settings.
///
typedef struct {

	int MissionNo;			///< Apollo mission number.

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
