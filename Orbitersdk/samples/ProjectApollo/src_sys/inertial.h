/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2004-2022

Generic Apollo Guidance computer class which is hooked
up to the DSKY to allow easy support for the CSM and LEM
computers. This defines the interfaces that the DSKY will
use to either computer, so only parts specific to the CSM
or LEM will need to be written specially.

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

#pragma once
#include "Orbitersdk.h"

class IntertialData {
	VECTOR3 accel;
	MATRIX3 rotmat;
	VESSEL *vessel;
	bool dVInitialized;
	VECTOR3 lastWeight;
	VECTOR3 lastGlobalVel;
	double lastSimDT;

public:
	IntertialData(VESSEL *vessel);
	VECTOR3 IntertialData::GetGravityVector();
	void timestep(double simdt);
	void getIntertialAccel(VECTOR3 &acc);
};
