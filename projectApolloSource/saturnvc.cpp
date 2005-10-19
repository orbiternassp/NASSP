/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5
  Virtual cockpit code

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
  *	Revision 1.8  2005/10/11 16:46:34  tschachim
  *	Fixed camera offsets.
  *	
  *	Revision 1.7  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.6  2005/04/22 13:56:14  tschachim
  *	new panel ids and defines
  *	
  *	Revision 1.5  2005/04/11 23:49:32  yogenfrutz
  *	once more:corrected docking view position
  *	
  *	Revision 1.4  2005/03/17 06:43:26  yogenfrutz
  *	adjusted cameraoffset for docking alignment,this was necessary due to the correction of the docking positions of the LEM
  *	
  *	Revision 1.3  2005/03/09 22:04:16  tschachim
  *	Better docking camera offset from Yogenfrutz
  *	
  *	Revision 1.2  2005/03/03 17:52:47  tschachim
  *	camera offset for docking panel
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include <stdio.h>
#include <math.h>
#include "Orbitersdk.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"

#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"


bool Saturn::clbkLoadVC (int id)
{
	TRACESETUP("Saturn::clbkLoadVC");

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

void Saturn::JostleViewpoint(double amount)

{
	double j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double) ((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetz += j;

	if (ViewOffsetx > 0.10)
		ViewOffsetx = 0.10;
	if (ViewOffsetx < -0.10)
		ViewOffsetx = -0.10;

	if (ViewOffsety > 0.10)
		ViewOffsety = 0.10;
	if (ViewOffsety < -0.10)
		ViewOffsety = -0.10;

	if (ViewOffsetz > 0.05)
		ViewOffsetz = 0.05;
	if (ViewOffsetz < -0.05)
		ViewOffsetz = -0.05;

	SetView();
}

void Saturn::SetView()

{
	SetView(CurrentViewOffset);
}

void Saturn::SetView(double offset)

{
	VECTOR3 v;

	CurrentViewOffset = offset;

	if (InPanel) {
		if (PanelId == SATPANEL_LEFT_RNDZ_WINDOW) {
			v = _V(-1.022, 1.046, offset - 3.0);

		} else if (PanelId == SATPANEL_RIGHT_RNDZ_WINDOW) {
			v = _V(1.022, 1.046, offset - 3.0);

		} else {
			v = _V(0, 0, offset - 3.0);
		}
	} else if (!InVC) {		// generic panel
		v = _V(0, 0, offset - 3.0);

	} else {
		switch (viewpos) {
			case SATVIEW_CDR:
			v = _V(-0.6, 0.7, offset);
			break;

			case SATVIEW_CMP:
			v = _V(0, 0.7, offset);
			break;

			case SATVIEW_DMP:
			v = _V(0.6, 0.7, offset);
			break;

			case SATVIEW_DOCK:
			if (dockstate == 13) {
				v = _V(0, 0, 2.5 + offset);
			} else {
				v = _V(-0.65, 1.05, 0.25 + offset);
			}
			break;
		}
		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;
	}

	SetCameraOffset(v);
}
