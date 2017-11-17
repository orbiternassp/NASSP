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
					SetCameraOffset (_V(-0.598, -1.00, 1.106));
					break;
				case LMPANEL_LEFTPANEL:
					SetCameraOffset (_V(-0.576, 1.90, 1.26));
					break;
				case LMPANEL_AOTVIEW:
					SetCameraOffset (_V(0, 2.88, 1.26));
					break;
				case LMPANEL_AOTZOOM:
					SetCameraOffset (_V(0, 2.88, 1.26));
					break;
				case LMPANEL_DOCKVIEW:
					SetCameraOffset (_V(-0.598, -1.00, 1.106));
					break;
			}
		}
		else
			SetCameraOffset (_V(0, 0, 0));
	}

	//
	// Change FOV for the LPD window, AOT zoom and docking view
	//
	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
	   // if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 70 degrees
		oapiCameraSetAperture(RAD * 35.0);
	}
	else if (PanelId == LMPANEL_AOTZOOM) {
		// if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees
		oapiCameraSetAperture(RAD * 30.0);
	}
	else if (PanelId == LMPANEL_DOCKVIEW) {
			// if this is the first time we've been here, save the current FOV
			if (InFOV) {
				SaveFOV = oapiCameraAperture();
				InFOV = false;
			}
			//set FOV to 20 degrees
			oapiCameraSetAperture(RAD * 10.0);
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

