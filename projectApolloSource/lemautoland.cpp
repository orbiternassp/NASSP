/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Saturn LEM computer
  Autoland code based on LMMFD

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

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"
#include "soundlib.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "lemcomputer.h"

#define MAXOFFPLANE	44000
#define BRAKDIST	425000
#define PDIDIST		470000
#define	MINDIST		661000
#define ORIENTSEC	120
#define ULLAGESEC	7
#define OFFSET		60

//
// Code goes here
//


void LEMcomputer::Prog63(double simt)

{
	switch (ProgState) {
	case 0:
		SetVerbNounAndFlash(6, 89);
		ProgState++;
		break;

	case 2:
		ProgState++;
		break;
	}
}

void LEMcomputer::Prog63Pressed(int R1, int R2, int R3)

{
	switch (ProgState)
	{
	//
	// 1: Getting landing site location.
	//
	case 1:
		if (R1 > 9000 || R1 < -9000)
			break;

		if (R2 < -18000 || R2 > 18000)
			break;

		LandingLatitude = (double) R1 / 100.0;
		LandingLongitude = (double) R2 / 100.0;
		LandingAltitude = (double) R3;
		ProgState++;
		return;
	}

	dsky.LightOprErr();
}
