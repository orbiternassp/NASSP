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

protected:
	double velocity;
	double targetHeading;
	int reverseDirection;
	int standalone;
	bool firstTimestepDone;

	bool lastLatLongSet;
	double lastLat;
	double lastLong;

	bool keyAccelerate;
	bool keyBrake;
	bool keyLeft;
	bool keyRight;

	OBJHANDLE hML;
	OBJHANDLE hLV;
	OBJHANDLE hMSS;
	
	SoundLib soundlib;
	Sound soundEngine;

	char LVName[256];

	void DoFirstTimestep();
	bool IsAttached();
	void Attach();
	void Detach();
	void ToggleDirection();
	void SetView();
	void SlowIfDesired(double timeAcceleration);
};
