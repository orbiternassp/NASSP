/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Cue Card Manager class (Header)

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
#include "CueCardLocation.h"

class CueCardManager
{
public:
	CueCardManager(int &vcidx, VESSEL *v, unsigned num);

	//Creates cue card, should be called in vessel constructor or clbkPostStep
	void CreateCueCard(unsigned location, std::string name, VECTOR3 offs = _V(0, 0, 0));
	//Called from clbkVCMouseEvent, cycles through cue cards for a specific location
	void CycleCueCard(unsigned location);
	//If meshes were deleted and recreated, reload current cue cards
	void ResetCueCards();
	//Reset mesh indizes if meshes were deleted externally
	void ResetMeshIndizes();
	//Get vector of all mesh indices
	void GetMeshIndexList(std::vector<UINT> &vec);

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	VESSEL *V() { return vessel; }
	VECTOR3 GetVCOffset();
protected:
	int &vc;
	VESSEL *vessel;
	std::vector<CueCardLocation> CueCardLocations;
};

#define CUECARDS_START_STRING	"CUECARDS_BEGIN"
#define CUECARDS_END_STRING		"CUECARDS_END"