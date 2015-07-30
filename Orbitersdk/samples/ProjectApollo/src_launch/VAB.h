/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  VAB vessel

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
  *	Revision 1.1  2009/02/18 23:21:14  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.6  2007/03/01 18:24:33  tschachim
  *	Fixed Saturn V assembly
  *	
  *	Revision 1.5  2006/06/26 19:05:36  movieman523
  *	More doxygen, made Lunar EVA a VESSEL2, made SM breakup, made LRV use VESSEL2 save/load functions.
  *	
  *	Revision 1.4  2006/04/25 13:44:22  tschachim
  *	New KSC.
  *	
  *	Revision 1.3  2005/11/21 13:31:34  tschachim
  *	New Saturn assembly meshes.
  *	
  *	Revision 1.2  2005/11/09 18:41:03  tschachim
  *	New Saturn assembly process.
  *	
  *	Revision 1.1  2005/10/31 11:59:22  tschachim
  *	New VAB.
  *	
  **************************************************************************/

///
/// \ingroup Ground
///
class VAB: public VESSEL2 {

public:
	VAB(OBJHANDLE hObj, int fmodel);
	virtual ~VAB();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	int clbkConsumeDirectKey(char *kstate);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);
	void clbkPostCreation();

	// Called by Crawler
	virtual void ToggleHighBay1Door();
	virtual void ToggleHighBay3Door();
	virtual void BuildSaturnStage();
	virtual void UnbuildSaturnStage();

protected:
	bool firstTimestepDone;

	int meshindexVAB;
	int meshindexSaturn[24];
	int meshcountSaturn;

	enum CraneStatus { 
		CRANE_BEGIN, 
		CRANE_END, 
		CRANE_BUILDING, 
		CRANE_UNBUILDING 
	} crane_Status;

	UINT animCrane[5];
	int currentAnimCrane;
	int animCraneCount;
	double crane_Proc;
	MGROUP_TRANSFORM *mgroupCrane[5][16];
	MGROUP_TRANSFORM *mgroupSaturn[24][8];
	MGROUP_TRANSFORM *mgroupCrane2[5][8];
	UINT animPlatform[5];
	double platform_Proc[5];
	UINT animSaturnStage1;
	double saturnStage1_Proc;
	bool adjustSaturnStage1;

	enum DoorStatus { 
		DOOR_CLOSED, 
		DOOR_OPEN, 
		DOOR_CLOSING, 
		DOOR_OPENING 
	} highBay1Door_Status, highBay3Door_Status;

	UINT anim_HighBay1Door;         
	UINT anim_HighBay3Door;         
	double highBay1Door_Proc;
	double highBay3Door_Proc;

	char LVName[256];
	int LVVisible;

	SoundLib soundlib;
	Sound soundEngine;

	void DoFirstTimestep();
	void DefineAnimations();
	void DefineCraneAnimation(MGROUP_TRANSFORM *mgroup[], double pickupHeight, double releaseHeight);
	void DefineSaturnAnimation(MGROUP_TRANSFORM *mgroup[], int meshindex, double pickupHeight, double releaseHeight);
	void DefineCrane2Animation(MGROUP_TRANSFORM *mgroup[], double pickupHeight, double length);
	void SetSaturnMeshVisibilityMode(int buildStatus, WORD mode);
};
