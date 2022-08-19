/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2022

InertialData class is the common source of inertial acceleration data
for variety of instruments (e.g. IMU, EMS, etc.).

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

#define INERTIAL_DATA_START_STRING	 "INERTIAL_DATA_BEGIN"
#define INERTIAL_DATA_END_STRING    "INERTIAL_DATA_END"

class InertialData {
	VECTOR3 accel;
	MATRIX3 rotmat;
	VESSEL *vessel;
	bool dVInitialized;
	VECTOR3 lastWeight;
	VECTOR3 lastGlobalVel;
	double lastSimDT;

public:
	InertialData(VESSEL *vessel);
	VECTOR3 GetGravityVector();
	void Timestep(double simdt);
	void getAcceleration(VECTOR3 &acc);
	void SaveState(FILEHANDLE scn);                                // SaveState callback
	void LoadState(FILEHANDLE scn);                                // LoadState callback
};
