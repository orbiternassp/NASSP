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

#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"


bool Saturn::clbkLoadVC (int id)
{
	switch (id) {
	case 0:
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		InVC = true;
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

	switch (viewpos)
	{
		case SATVIEW_CDR:
		v = _V(-0.6,0.7,offset);
		break;

		case SATVIEW_CMP:
		v = _V(0,0.7,offset);
		break;

		case SATVIEW_DMP:
		v = _V(0.6,0.7,offset);
		break;

		case SATVIEW_DOCK:
		if (dockstate==13) {
			v = _V(0,0,2.5 + offset);
		}else{
			v = _V(-0.65,1.05,0.25 + offset);
		}
		break;
	}

	if (InVC) {
		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;
	}

	SetCameraOffset(v);
}
