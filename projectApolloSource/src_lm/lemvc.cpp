/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LEM virtual cockpit code

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
  *	Revision 1.11  2008/04/11 11:49:45  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.10  2007/06/06 15:02:14  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.9  2006/08/13 16:55:35  movieman523
  *	Removed a bunch of unused files.
  *	
  *	Revision 1.8  2006/08/13 16:01:52  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  *	Revision 1.7  2005/11/17 03:41:33  darth_vader
  *	Reset the VC position to the previous value. Lowered the position of the docking window viewpoint, so that the crew will not be visible anymore. However part of the CM is still "cut" ... nothing much can be done for that I'm afraid ...
  *	
  *	Revision 1.6  2005/11/12 18:13:03  darth_vader
  *	Adjusted the camera position for the various 2D panels to something more realistic (I hope).
  *	
  *	Revision 1.5  2005/10/31 10:16:54  tschachim
  *	LEM rendezvous window camera offset.
  *	
  *	Revision 1.4  2005/08/14 16:08:20  tschachim
  *	LM is now a VESSEL2
  *	Changed panel restore mechanism because the CSM mechanism
  *	caused CTDs, reason is still unknown.
  *	
  *	Revision 1.3  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.2  2005/08/09 09:14:31  tschachim
  *	Introduced toggleswitch lib
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "IMU.h"

#include "LEM.h"

void LEM::SetView() {

	//
	// Set camera offset
	//
	if (InVC) {
		switch (viewpos) {
		case LMVIEW_CDR:
			SetCameraOffset (_V(-0.68, 1.65, 1.35));
			//SetCameraOffset (_V(-0.576, 1.90, 1.26));
			break;

		case LMVIEW_LMP:
			SetCameraOffset (_V(0.92, 1.65, 1.23));
			//SetCameraOffset (_V(0.576, 1.90, 1.26));
			break;
		}

	} else {

		if(InPanel)
		{
			switch(PanelId)
			{
				case LMPANEL_MAIN:
					SetCameraOffset (_V(0, 1.90, 1.26));
					break;
				case LMPANEL_RIGHTWINDOW:
					SetCameraOffset (_V(0.576, 1.90, 1.26));
					break;
				case LMPANEL_LEFTWINDOW:
					SetCameraOffset (_V(-0.576, 1.90, 1.26));
					break;
				case LMPANEL_LPDWINDOW:
					SetCameraOffset (_V(-0.576, 1.90, 1.26));
					break;
				case LMPANEL_RNDZWINDOW:
					SetCameraOffset (_V(-0.28, -1.00, 1.65));
					//SetCameraOffset (_V(-0.576, -0.80, 1.26));
					break;
				case LMPANEL_LEFTPANEL:
					SetCameraOffset (_V(-0.576, 1.90, 1.26));
					break;
				case LMPANEL_AOTVIEW:
					SetCameraOffset (_V(0, 2.88, 1.26));
					break;
			}
		}
		else
			SetCameraOffset (_V(0, 0, 0));
	}

	//
	// Change FOV for the LPD window 
	//
	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
	   // if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 70 degrees
		oapiCameraSetAperture(RAD * 35.0);

	} else {
		if(InFOV == false) {
			oapiCameraSetAperture(SaveFOV);
			InFOV = true;
		}
	}
}

bool LEM::clbkLoadVC (int id)

{
	switch (id) {
	case 0:
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		InVC = true;
		InPanel = false;
		SetView();
		return true;

	default:
		return false;
	}
}

