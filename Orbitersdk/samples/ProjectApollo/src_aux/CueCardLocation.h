/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Cue Card Location class (Header)

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

#include "CueCard.h"

class CueCardManager;

class CueCardLocation
{
public:
	CueCardLocation(CueCardManager *m);

	//Create a new cue card at this location
	void CreateCueCard(std::string name, VECTOR3 offs);
	//Cycle through the possible states of the cue card location
	void CycleCueCard();
	//Update visible state of cue card
	void LoadCueCard();
	//Called when meshes were deleted externally
	void ResetMeshIndizes();
	//Get vector of all mesh indices
	void GetMeshIndexList(std::vector<UINT> &vec);

	unsigned GetState() { return state; }
	void SetState(unsigned s) { state = s; }

	//Vessel pointer
	VESSEL *V();
	//Get offset of the VC mesh
	VECTOR3 GetVCOffset();
protected:
	CueCardManager *man;
	unsigned state; //State of cue card location, 0 = no cue card shown
	std::vector<CueCard> CueCards;
};