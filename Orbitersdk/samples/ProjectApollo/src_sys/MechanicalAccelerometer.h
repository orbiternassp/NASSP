/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Mechanical Accelerometer Simulation Header

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

#define MECHACCEL_START_STRING	"MECHACCEL_BEGIN"
#define MECHACCEL_END_STRING	"MECHACCEL_END"

class MechanicalAccelerometer
{
public:
	MechanicalAccelerometer();
	void Init(VESSEL* v);
	void TimeStep(double simdt);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	double GetXAccel();
	double GetYAccel();
private:

	MATRIX3 GetRotationMatrixX(double angle);
	MATRIX3 GetRotationMatrixY(double angle);
	MATRIX3 GetRotationMatrixZ(double angle);

	VESSEL *vessel;
	bool dVInitialized;
	VECTOR3 lastWeight;
	VECTOR3 lastGlobalVel;
	double lastSimDT;
	double xacc, yacc;
};
