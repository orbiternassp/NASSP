/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2002-2005 Chris Knestrick

  Header file for spacecraft control

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
  *	Revision 1.2  2005/12/30 03:27:54  movieman523
  *	Hopefully fixed some compilation errors with VC++ 2005
  *	
  *	Revision 1.1  2005/02/11 12:17:54  tschachim
  *	Initial version
  *	
  **************************************************************************/

#ifndef CONTROL_H
#define CONTROL_H
#include <math.h>
#include "Orbitermath.h"

// The deadband for attitude adjustments.  These are just the "standard" deadband limits - a user
// can pass any deadband value to SetAttitude().  For example, a 5 degree deadband:
// SetAttitude(TargetYaw, CurrentYaw, YAW, DEADBAND(Radians(5)))
typedef double DEADBAND;

// CCK
//const DEADBAND DB_FINE = Radians(0.2),
//			   DB_NORMAL = Radians(0.5),
//			   DB_COARSE = Radians(1);

const DEADBAND DB_FINE = Radians(0.1),
			   DB_NORMAL = Radians(0.5),
			   DB_COARSE = Radians(1);
//CCK End

// Provide ease of access to vectors (e.g. Status.vrot.data[PITCH])
enum AXIS {PITCH, YAW, ROLL};

// Prototypes
bool SetAttitude(double TargetAttitude, double CurrentAttitude, double RotRate,
					AXIS Axis, DEADBAND DeadbandLow);
bool SetAttitude(double TargetAttitude, double CurrentAttitude, AXIS Axis, DEADBAND DeadbandLow);
bool NullRate(AXIS Axis);

static inline void NormalizeThrustLevel(double& Level)
{
	if (Level > 1.0) {
		Level = 1.0;
	} else if (Level < -1.0) {
		Level = -1.0;
	}
}

#endif