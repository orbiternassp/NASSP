/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  VAB Transporter vessel animations

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
  *	Revision 1.9  2008/04/11 11:49:31  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.8  2007/06/06 15:02:10  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.7  2007/03/01 18:24:33  tschachim
  *	Fixed Saturn V assembly
  *	
  *	Revision 1.6  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.5  2006/04/25 13:56:41  tschachim
  *	New KSC.
  *	
  *	Revision 1.4  2005/11/21 13:31:34  tschachim
  *	New Saturn assembly meshes.
  *	
  *	Revision 1.3  2005/11/09 18:35:42  tschachim
  *	New Saturn assembly process.
  *	
  *	Revision 1.2  2005/10/31 19:18:39  tschachim
  *	Bugfixes.
  *	
  *	Revision 1.1  2005/10/31 11:59:22  tschachim
  *	New VAB.
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"

#include "VAB.h"
#include "ML.h"

#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"


void VAB::ToggleHighBay1Door() {

	if (highBay1Door_Status == DOOR_CLOSED || highBay1Door_Status == DOOR_CLOSING)
		highBay1Door_Status = DOOR_OPENING;
	else
		highBay1Door_Status = DOOR_CLOSING;
}

void VAB::ToggleHighBay3Door() {

	if (highBay3Door_Status == DOOR_CLOSED || highBay3Door_Status == DOOR_CLOSING)
		highBay3Door_Status = DOOR_OPENING;
	else
		highBay3Door_Status = DOOR_CLOSING;
}

void VAB::BuildSaturnStage() {

	if (crane_Status == CRANE_BUILDING || crane_Status == CRANE_UNBUILDING) return;

	// Check Saturn
	if (LVName[0] == '\0') return;
	OBJHANDLE hLV = oapiGetVesselByName(LVName);
	if (!hLV) return;
	Saturn *lav = (Saturn *) oapiGetVesselInterface(hLV);
	if (lav->GetStage() != ROLLOUT_STAGE) return;

	// Check ML
	OBJHANDLE hML = oapiGetVesselByName("ML");
	if (!hML) return;
	ML *ml = (ML *) oapiGetVesselInterface(hML);
	if (!ml->IsInVAB()) return;

	// Reopen platforms
	if (lav->GetBuildStatus() == animCraneCount && platform_Proc[0] > 0) { 
		currentAnimCrane = -1;
		crane_Status = CRANE_BUILDING;
		return;
	}
	if (lav->GetBuildStatus() >= animCraneCount) { 
		lav->LaunchVehicleBuild();
		return;
	}

	for (int i = 0; i < animCraneCount; i++) 
		SetAnimation(animCrane[i], 0);

	// Saturn Stage 1
	if (lav->GetBuildStatus() == 0) {		
		saturnStage1_Proc = 0.00001;
		SetAnimation(animSaturnStage1, saturnStage1_Proc);
	}

	currentAnimCrane = animCrane[lav->GetBuildStatus()];	
	crane_Proc = 0.00001;
	SetAnimation(currentAnimCrane, crane_Proc);

	crane_Status = CRANE_BUILDING;
}

void VAB::UnbuildSaturnStage() {

	int i = 0;

	if (crane_Status == CRANE_BUILDING || crane_Status == CRANE_UNBUILDING) return;

	// Check Saturn
	if (LVName[0] == '\0') return;
	OBJHANDLE hLV = oapiGetVesselByName(LVName);
	if (!hLV) return;
	Saturn *lav = (Saturn *) oapiGetVesselInterface(hLV);
	if (lav->GetStage() != ROLLOUT_STAGE) return;

	// Check ML
	OBJHANDLE hML = oapiGetVesselByName("ML");
	if (!hML) return;
	ML *ml = (ML *) oapiGetVesselInterface(hML);
	if (!ml->IsInVAB()) return;

	if (lav->GetBuildStatus() <= 0) return;
	if (lav->GetBuildStatus() > animCraneCount) {	
		lav->LaunchVehicleUnbuild();
		return;
	}

	// Close platforms
	if (lav->GetBuildStatus() == animCraneCount && platform_Proc[0] < 1.0) { 
		currentAnimCrane = -1;
		crane_Status = CRANE_UNBUILDING;
		return;
	}

	for (i = 0; i < animCraneCount; i++) 
		SetAnimation(animCrane[i], 0);

	// Saturn Stage 1
	if (lav->GetBuildStatus() == 1) {
		if (saturnStage1_Proc < 1.0) {
			adjustSaturnStage1 = true;
			if (saturnStage1_Proc < 0.3) {
				saturnStage1_Proc = 0.3;
				SetAnimation(animSaturnStage1, saturnStage1_Proc);
			}
		}
	}
	currentAnimCrane = animCrane[lav->GetBuildStatus() - 1];
	crane_Proc = 1.0;
	SetAnimation(currentAnimCrane, crane_Proc);

	crane_Status = CRANE_UNBUILDING;
}
