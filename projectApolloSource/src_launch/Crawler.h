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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.16  2007/12/15 19:48:26  lassombra
  *	Added functionality to allow ProjectApollo MFD to get mission time from the Crawler as well as the Saturn.  The Crawler actually extracts the mission time from the Saturn, no updates to scenario files needed.
  *	
  *	Revision 1.15  2007/08/16 16:49:53  tschachim
  *	New meshes.
  *	
  *	Revision 1.14  2007/03/03 00:14:41  tschachim
  *	Bugfix generic cockpit.
  *	
  *	Revision 1.13  2007/03/02 18:34:37  tschachim
  *	Improved crawler VC.
  *	
  *	Revision 1.12  2007/03/01 17:58:26  tschachim
  *	New VC panel
  *	
  *	Revision 1.11  2006/06/26 19:05:36  movieman523
  *	More doxygen, made Lunar EVA a VESSEL2, made SM breakup, made LRV use VESSEL2 save/load functions.
  *	
  *	Revision 1.10  2006/04/25 13:32:50  tschachim
  *	New KSC.
  *	
  *	Revision 1.9  2005/11/23 21:36:55  movieman523
  *	Allow specification of LV name in scenario file.
  *	
  *	Revision 1.8  2005/10/31 19:18:39  tschachim
  *	Bugfixes.
  *	
  *	Revision 1.7  2005/10/31 10:30:35  tschachim
  *	New VAB.
  *	
  *	Revision 1.6  2005/08/05 13:01:49  tschachim
  *	Saturn detachment handling
  *	
  *	Revision 1.5  2005/07/05 17:23:11  tschachim
  *	Scenario saving/loading
  *	
  *	Revision 1.4  2005/07/01 12:23:48  tschachim
  *	Introduced standalone flag
  *	
  *	Revision 1.3  2005/06/29 11:01:18  tschachim
  *	new dynamics, added attachment management
  *	
  *	Revision 1.2  2005/06/14 16:14:41  tschachim
  *	File header inserted.
  *	
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
