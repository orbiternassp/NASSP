/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Cue Card class (Header)

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

class CueCardLocation;

class CueCard
{
public:
	CueCard(CueCardLocation *l, std::string meshname, VECTOR3 offs);

	//Called when meshes were deleted externally
	void ResetMeshIndex();
	//Update visible state of cue card
	void ManageCueCard(bool on);
	//Get mesh index
	int GetMeshIndex();
protected:

	//Vessel pointer
	VESSEL *V();
	//Get offset of the VC mesh
	VECTOR3 GetVCOffset();

	std::string name;
	int meshidx;
	MESHHANDLE mesh;
	//Optional mesh offset
	VECTOR3 meshoff;

	CueCardLocation *loc;
};