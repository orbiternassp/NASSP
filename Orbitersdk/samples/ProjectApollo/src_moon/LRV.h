/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Lunar Rover

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
  *	Revision 1.9  2007/05/12 10:26:15  redburne
  *	2007-05-12 OP:
  *	- fixed: wheel spin (tweaked to look more realistic; time acceleration compatible)
  *	- new: key 2 performs auto-center, just like 5
  *	- new: CTRL+1, CTRL+3, CTRL+PLUS, CTRL+MINUS act at one tenth the speed (good for driving at 10x)
  *	- known issues: steering and wheel spin are still not 100% correct. The error, however, is probably smaller than the real differences caused by wheel slip, sliding, etc ...
  *	
  *	Revision 1.8  2007/05/10 18:39:36  redburne
  *	- new: realistic max. wheel deflection of 50 deg for inner wheel (leads to wheel-body intersections in the mesh ...)
  *	- new: outer wheel deflection calculated accordingly (about 25 deg max)
  *	- new: realistic wheel deflection speed (lock-to-lock in about 5.5s)
  *	- new: gradual auto-centering
  *	- new: "real" turning based on speed and turn radius
  *	- new: different wheel spins for inner and outer wheels (seems a tiny, tiny bit off,  but better than before)
  *	- fixed: wheel rotation driving backwards would stop after one turn, max.
  *	
  *	Revision 1.7  2006/10/02 22:23:46  jasonims
  *	Animation updated for Orbiter 2006-P1, LRV @ 80% complete.  Wheels now rotate correctly and steer correctly to a point.  Discreet steering now supported.
  *	
  *	Revision 1.6  2006/08/28 14:30:35  jasonims
  *	temperarly removed animation calls due to Orbiter2006 bug causing CTD's, when patched, will reinstate animations.
  *	
  *	Revision 1.5  2006/08/23 06:31:04  jasonims
  *	Corrected potential SM Umbilical Animation problem.  Did all pre-set work for LRV Animations...wheels have ability to rotate, turn and bounce with shocks.  No actual implementation yet, but all that needs to be done is modify the proc_*ANIMATIONNAMEHERE* variables.  Those coming in next commit.
  *	
  *	Revision 1.4  2006/06/26 19:05:36  movieman523
  *	More doxygen, made Lunar EVA a VESSEL2, made SM breakup, made LRV use VESSEL2 save/load functions.
  *	
  *	Revision 1.3  2006/06/06 17:50:59  redburne
  *	- New gauges ("working" battery capacity indicator; other values are static)
  *	- boolean variable GoRover removed
  *	- more realistic accelleration (much slower, probably still a bit fast)
  *	- new speed handling: brake to standstill, release and press key again to further increase/decrease speed
  *	
  *	Revision 1.2  2006/05/06 06:00:35  jasonims
  *	No more venting our Astronauts into space...and no more LRV popping out of an Astronauts pocket....well sorta.
  *	
  *	Revision 1.1  2006/05/05 21:30:06  movieman523
  *	Added beginnings of LRV code.
  *	
  **************************************************************************/

///
/// \ingroup AstronautSettings
/// \brief LRV settings.
///
typedef struct 
{

	int MissionNo;			///< Apollo mission number.
	int Realism;			///< Realism level.

} LRVSettings;

///
/// \ingroup Astronauts
/// \brief Lunar Rover.
///
class LRV: public VESSEL2 {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	LRV(OBJHANDLE hObj, int fmodel);
	virtual ~LRV();

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

	///
	/// \brief Setup rover.
	///
	void SetRoverStage ();
	void init();

	///
	/// \brief Orbiter state saving function.
	/// \param scn Scenario file to save to.
	///
	void clbkSaveState (FILEHANDLE scn);

	///
	/// \brief Orbiter state loading function.
	/// \param scn Scenario file to load from.
	/// \param status Pointer to current vessel status.
	///
	void clbkLoadStateEx (FILEHANDLE scn, void *status);

	//
	// Virtual so it can be called from the LEM without needing to link in all the code
	// to process the function.
	//

	virtual void SetLRVStats(LRVSettings &lrvs);

	// LRV ANIMATIONS

	void DefineAnimations();
	void DoAnimations();
	void UpdateAnimations(double SimDT);

private:

	void ScanMotherShip();
	void MoveLRV(double SimDT, VESSELSTATUS *eva, double heading);
	void SetMissionPath();
	void DoFirstTimestep();
	void SetMainState(int s);
	int GetMainState();
	void SetNeedleAngle(int idx, double val, double min_val, double max_val);

protected:

	int ApolloNo;
	int StepCount;
	int Realism;
	int LRVMeshIndex;

	//Animation Related

	//handles for turning tires, so they can bounce on terrain
	ANIMATIONCOMPONENT_HANDLE fr_fender;
	ANIMATIONCOMPONENT_HANDLE fl_fender;
	ANIMATIONCOMPONENT_HANDLE rr_fender;
	ANIMATIONCOMPONENT_HANDLE rl_fender;

	//handles for rotating tires so they can be turned
	ANIMATIONCOMPONENT_HANDLE fr_wheel;
	ANIMATIONCOMPONENT_HANDLE fl_wheel;
	ANIMATIONCOMPONENT_HANDLE rr_wheel;
	ANIMATIONCOMPONENT_HANDLE rl_wheel;

	UINT anim_fntrgtfender, anim_fntlftfender, anim_rearrgtfender, anim_rearlftfender;
	UINT anim_fntrgtwheel, anim_fntlftwheel, anim_rearrgtwheel, anim_rearlftwheel;
	UINT anim_fntrgttire, anim_fntlfttire, anim_rearrgttire, anim_rearlfttire;

	MGROUP_TRANSFORM *frwheel, *flwheel, *rrwheel, *rlwheel;
	MGROUP_TRANSFORM *frtire, *fltire, *rrtire, *rltire;

	double proc_fntrgtfender, proc_fntlftfender, proc_rearrgtfender, proc_rearlftfender;
	double proc_frontwheel_inner, proc_frontwheel_outer, proc_rearwheel_inner, proc_rearwheel_outer;
	double proc_tires_left, proc_tires_right;

    
	OBJHANDLE hMaster;
	//bool GoDock1
	bool starthover;
	bool MotherShip;
	char EVAName[256];
	char CSMName[256];
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
	bool KEYCTRL;

	bool FirstTimestep;
	bool SLEVAPlayed;
	bool StateSet;

	bool lastLatLongSet;
	double lastLat;
	double lastLong;

	double speed;  // current speed in m/s
	bool speedlock;  // true: lock speed change until both throttle keys are released
	double steering; //current steering setting [-1.0 ... 1.0]
	bool autocenter;  // true: wheel auto-centering (KP_5) in prog
	SoundLib soundlib;
	Sound SLEVA;

	// touchdown point test
	// double touchdownPointHeight;

	// power and temperature (currently simplistic and/or faked)
	double Bat1Cap;  // remaining capacity of battery 1 [Ah]
	double Bat2Cap;  // remaining capacity of battery 2 [Ah]
	double Bat1Volt; // voltage of battery 1 [V]
	double Bat2Volt; // voltage of battery 2 [V]
	double Bat1Temp; // temperature of battery 1 [°F]
	double Bat2Temp; // temperature of battery 2 [°F]

	// VC console
	int vccMeshIdx;
	VISHANDLE vccVis;
	double vccCompAngle;
	double vccBear001Angle;
	double vccBear010Angle;
	double vccBear100Angle;
	double vccDist001Angle;
	double vccDist010Angle;
	double vccDist100Angle;
	double vccRange001Angle;
	double vccRange010Angle;
	double vccRange100Angle;
	double vccSpeedAngle;
	double vccNeedleAngle[8];
	bool vccInitialized;	// true, as soon as vccInitLat and vccInitLong contain usable values
	double vccInitLat;	// latitude of last console navigation initialization (bearing and range reference point)
	double vccInitLong;	// longitude of last console navigation initialization (bearing and range reference point)
	double vccDistance;	// distance travelled since last console navigation initialization
	MESHGROUP_TRANSFORM mgtRotCompass;
	MESHGROUP_TRANSFORM mgtRotSpeed;
	MESHGROUP_TRANSFORM mgtRotDrums;  // Bearing, distance or range "drum"
	MESHGROUP_TRANSFORM mgtRotGauges;  // rotation of the 8 needles of the four power/temp gauges
};
