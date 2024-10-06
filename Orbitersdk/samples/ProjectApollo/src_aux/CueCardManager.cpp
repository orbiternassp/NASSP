/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Cue Card Manager class

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

#include "CueCardManager.h"

CueCardManager::CueCardManager(int &vcidx, VESSEL *v, unsigned num) : vc(vcidx)
{
	vessel = v;
	CueCardLocations.resize(num, CueCardLocation(this));
}

void CueCardManager::CreateCueCard(unsigned location, std::string name, VECTOR3 offs)
{
	if (CueCardLocations.size() <= location) return;
	CueCardLocations[location].CreateCueCard(name, offs);
}

void CueCardManager::CycleCueCard(unsigned location)
{
	if (CueCardLocations.size() <= location) return;
	CueCardLocations[location].CycleCueCard();
}

void CueCardManager::ResetCueCards()
{
	for (unsigned i = 0; i < CueCardLocations.size(); i++)
	{
		CueCardLocations[i].LoadCueCard();
	}
}

void CueCardManager::ResetMeshIndizes()
{
	for (unsigned i = 0; i < CueCardLocations.size(); i++)
	{
		CueCardLocations[i].ResetMeshIndizes();
	}
}

void CueCardManager::GetMeshIndexList(std::vector<UINT> &vec)
{
	vec.clear();
	for (unsigned i = 0; i < CueCardLocations.size(); i++)
	{
		CueCardLocations[i].GetMeshIndexList(vec);
	}
}

VECTOR3 CueCardManager::GetVCOffset()
{
	VECTOR3 ofs;
	if (vc != -1)
	{
		vessel->GetMeshOffset(vc, ofs);
	}
	else
	{
		ofs = _V(0, 0, 0); //Just in case
	}
	return ofs;
}

void CueCardManager::SaveState(FILEHANDLE scn)
{
	bool save = false;
	char Buffer[128];
	for (unsigned i = 0; i < CueCardLocations.size(); i++)
	{
		if (CueCardLocations[i].GetState() != 0)
		{
			if (save == false)
			{
				oapiWriteLine(scn, CUECARDS_START_STRING);
				save = true;
			}

			sprintf(Buffer, "%d %d", i, CueCardLocations[i].GetState());
			oapiWriteScenario_string(scn, "CUECARDS", Buffer);
		}
	}

	if (save) oapiWriteLine(scn, CUECARDS_END_STRING);
}

void CueCardManager::LoadState(FILEHANDLE scn)
{
	unsigned loctemp, statetemp;

	char *line;
	double flt = 0;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!_strnicmp(line, CUECARDS_END_STRING, sizeof(CUECARDS_END_STRING)))
			break;

		if (!_strnicmp(line, "CUECARDS", 8))
		{
			sscanf(line + 8, "%d %d", &loctemp, &statetemp);
			if (CueCardLocations.size() > loctemp)
			{
				CueCardLocations[loctemp].SetState(statetemp);
			}
		}
	}
}