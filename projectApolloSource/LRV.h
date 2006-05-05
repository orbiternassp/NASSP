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
  **************************************************************************/

typedef struct {

	int MissionNo;
	int Realism;

} EVASettings;

class LRV: public VESSEL2 {

public:
	LRV(OBJHANDLE hObj, int fmodel);
	virtual ~LRV();

	void clbkPreStep (double SimT, double SimDT, double mjd);
	int clbkConsumeDirectKey(char *kstate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkVisualCreated (VISHANDLE vis, int refcount);
	void clbkVisualDestroyed (VISHANDLE vis, int refcount);

	void SetAstroStage ();
	void SetRoverStage ();
	void init();
	void LoadState(FILEHANDLE scn, VESSELSTATUS *vs);
	void SaveState(FILEHANDLE scn);

	//
	// Virtual so it can be called from the LEM without needing to link in all the code
	// to process the function.
	//

	virtual void SetEVAStats(EVASettings &evas);

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
	bool GoRover;
	bool Astro;						
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
	bool GoFlag;		 
	bool FlagPlanted;

	bool FirstTimestep;
	bool SLEVAPlayed;
	bool StateSet;

	bool lastLatLongSet;
	double lastLat;
	double lastLong;

	double speed;  // current speed in m/s for both astronaut and rover

	SoundLib soundlib;
	Sound FlagSound;
	Sound SLEVA;

	// touchdown point test
	// double touchdownPointHeight;

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
	bool vccInitialized;	// true, as soon as vccInitLat and vccInitLong contain usable values
	double vccInitLat;	// latitude of last console navigation initialization (bearing and range reference point)
	double vccInitLong;	// longitude of last console navigation initialization (bearing and range reference point)
	double vccDistance;	// distance travelled since last console navigation initialization
	MESHGROUP_TRANSFORM mgtRotCompass;
	MESHGROUP_TRANSFORM mgtRotSpeed;
	MESHGROUP_TRANSFORM mgtRotDrums;  // Bearing, distance or range "drum"
};
