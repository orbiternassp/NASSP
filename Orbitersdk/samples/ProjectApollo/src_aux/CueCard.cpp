/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Cue Card class

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

#include "CueCard.h"
#include "CueCardLocation.h"

CueCard::CueCard(CueCardLocation *l, std::string meshname, VECTOR3 offs) : name(meshname), loc(l), meshoff(offs)
{
	meshidx = -1;
	mesh = oapiLoadMeshGlobal(name.c_str());
}

void CueCard::ResetMeshIndex()
{
	meshidx = -1;
}

int CueCard::GetMeshIndex()
{
	return meshidx;
}

void CueCard::ManageCueCard(bool on)
{
	//Should mesh be loaded?
	if (on)
	{
		//Yes, load mesh if it isn't already
		if (meshidx == -1)
		{
			VECTOR3 ofs = GetVCOffset() + meshoff;

			meshidx = V()->AddMesh(mesh, &ofs);
			V()->SetMeshVisibilityMode(meshidx, MESHVIS_VC);
		}
	}
	else
	{
		//No, delete mesh if it's loaded
		if (meshidx != -1)
		{
			V()->DelMesh(meshidx);
			meshidx = -1;
		}
	}
}

VESSEL *CueCard::V()
{
	return loc->V();
}

VECTOR3 CueCard::GetVCOffset()
{
	return loc->GetVCOffset();
}