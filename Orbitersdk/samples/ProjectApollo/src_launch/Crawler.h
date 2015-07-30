/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Crawler Transporter vessel

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
/// \ingroup Ground
///
class Crawler: public VESSEL2 {

public:
	Crawler(OBJHANDLE hObj, int fmodel);
	virtual ~Crawler();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	int clbkConsumeDirectKey(char *kstate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkPreStep (double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	void clbkVisualCreated(VISHANDLE vis, int refcount);
	void clbkVisualDestroyed(VISHANDLE vis, int refcount);
	bool clbkLoadGenericCockpit();
	// This will extract the mission time from the saturn in order to pass to ProjectApollo MFD
	double GetMissionTime() {return MissionTime;};

protected:
	double velocity;
	bool velocityStop;
	double targetHeading;
	int viewPos;
	double wheeldeflect;
	int standalone;
	bool firstTimestepDone;
	// Mission Time for passing to PAMFD:
	double MissionTime;

	bool lastLatLongSet;
	double lastLat;
	double lastLong;

	bool keyAccelerate;
	bool keyBrake;
	bool keyLeft;
	bool keyRight;
	bool keyCenter;

	OBJHANDLE hML;
	OBJHANDLE hLV;
	OBJHANDLE hMSS;
	VISHANDLE vccVis;	
	MESHGROUP_TRANSFORM vccSpeedGroup, vccSpeedGroupReverse;
	MESHGROUP_TRANSFORM vccSteering1Group, vccSteering2Group;
	MESHGROUP_TRANSFORM vccSteering1GroupReverse, vccSteering2GroupReverse;
	double vccSpeed, vccSteering;
    int meshidxCrawler;
    int meshidxTruck1;
	int meshidxTruck2;
	int meshidxTruck3;
	int meshidxTruck4;
    int meshidxPanel;
	int meshidxPanelReverse;

	SoundLib soundlib;
	Sound soundEngine;

	char LVName[256];

	void DoFirstTimestep();
	bool IsAttached();
	void Attach();
	void Detach();
	void SetView();
	void SetView(int viewpos);
	void SlowIfDesired(double timeAcceleration);
	void SetMeshesVisibility(WORD mode);

	VECTOR3 panelMeshoffset;
    int panelMeshidx;
};
