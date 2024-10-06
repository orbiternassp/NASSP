/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Cue Card Location class

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

#include "CueCardLocation.h"
#include "CueCardManager.h"

CueCardLocation::CueCardLocation(CueCardManager *m) : man(m)
{
	state = 0;
}

void CueCardLocation::CycleCueCard()
{
	//Assume only one or no meshes are currently loaded
	if (state != 0)
	{
		CueCards[state - 1].ManageCueCard(false);
	}
	state++;
	if (state > CueCards.size()) state = 0;

	LoadCueCard();
}

void CueCardLocation::LoadCueCard()
{
	if (state != 0)
	{
		CueCards[state - 1].ManageCueCard(true);
	}
}

void CueCardLocation::ResetMeshIndizes()
{
	for (unsigned i = 0; i < CueCards.size(); i++)
	{
		CueCards[i].ResetMeshIndex();
	}
}

void CueCardLocation::GetMeshIndexList(std::vector<UINT> &vec)
{
	UINT idx;
	for (unsigned i = 0; i < CueCards.size(); i++)
	{
		idx = CueCards[i].GetMeshIndex();
		if (idx != -1) vec.push_back(idx);
	}
}

void CueCardLocation::CreateCueCard(std::string name, VECTOR3 offs)
{
	CueCard c(this, name, offs);
	CueCards.push_back(c);
}

VESSEL *CueCardLocation::V()
{
	return man->V();
}

VECTOR3 CueCardLocation::GetVCOffset()
{
	return man->GetVCOffset();
}