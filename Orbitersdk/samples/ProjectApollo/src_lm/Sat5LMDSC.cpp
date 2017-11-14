/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2004-2017

ORBITER vessel module: Lunar Module Descent Stage

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
#include "orbitersdk.h"
#include "stdio.h"
#include "Sat5LMDSC.h"

static int refcount = 0;
static MESHHANDLE LM_Descent;

Sat5LMDSC::Sat5LMDSC(OBJHANDLE hObj, int fmodel)
	: VESSEL3(hObj, fmodel)

{
	init();
}

Sat5LMDSC::~Sat5LMDSC()

{
	// Nothing for now.
}

void Sat5LMDSC::init()

{
}

void Sat5LMDSC::Setup()

{
	SetSize(5);
	SetEmptyMass(4570.0);
	SetPMI(_V(2.8, 2.29, 2.37));
	SetCrossSections(_V(21, 23, 17));
	SetCW(0.1, 0.3, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 0.7));
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0);
	ClearMeshes();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	double tdph = -2.7;
	double Mass = 4570.0;
	double ro = 4;
	TOUCHDOWNVTX td[7];
	double x_target = -0.25;
	double stiffness = (-1)*(Mass*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(Mass*stiffness));
	for (int i = 0; i<7; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = 0;
	td[0].pos.y = tdph;
	td[0].pos.z = 1 * ro;
	td[1].pos.x = -cos(30 * RAD)*ro;
	td[1].pos.y = tdph;
	td[1].pos.z = -sin(30 * RAD)*ro;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = tdph;
	td[2].pos.z = -sin(30 * RAD)*ro;
	td[3].pos.x = cos(30 * RAD)*ro;
	td[3].pos.y = tdph;
	td[3].pos.z = sin(30 * RAD)*ro;
	td[4].pos.x = -cos(30 * RAD)*ro;
	td[4].pos.y = tdph;
	td[4].pos.z = sin(30 * RAD)*ro;
	td[5].pos.x = 0;
	td[5].pos.y = tdph;
	td[5].pos.z = -1 * ro;
	td[6].pos.x = 0;
	td[6].pos.y = 1.5;
	td[6].pos.z = 0;

	SetTouchdownPoints(td, 7);

	VECTOR3 mesh_dir = _V(0, 0, 0);
	AddMesh(LM_Descent, &mesh_dir);
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		LM_Descent = oapiLoadMeshGlobal("ProjectApollo/LM_Descent");
	}
	return new Sat5LMDSC(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (Sat5LMDSC*)vessel;
}

void Sat5LMDSC::clbkSetClassCaps(FILEHANDLE cfg)
{
	init();
	Setup();
}

