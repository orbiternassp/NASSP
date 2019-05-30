/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

Service Module Jettison Controller (Header)

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

#include "DelayTimer.h"

struct SMJCState
{
	bool Z1;
	bool Z2;
	bool Z3;
	bool FireMinusXTrans;
	bool FirePlusRoll;
	DelayTimerState TD2State;
	DelayTimerState TD3State;
};

class SMJC
{
public:
	SMJC();
	void Timestep(double simdt, bool smjettbuspowered);
	void SaveState(FILEHANDLE scn, char *start_str);
	void LoadState(FILEHANDLE scn);

	void SMJettControllerStart();
	void GSEReset();

	bool GetFireMinusXTranslation() { return FireMinusXTrans; }
	bool GetFirePositiveRoll() { return FirePlusRoll; }
	bool IsSMJettControllerStarted() { return Z1; }

	//For the SM vessel
	void SetState(const SMJCState &state);
	void GetState(SMJCState &state);

protected:

	bool FireMinusXTrans;
	bool FirePlusRoll;

	//RELAYS

	//SMJC Activate/ -X Translation
	bool Z1;
	//Positive Roll Initiate
	bool Z2;
	//Positive Roll Deactive
	bool Z3;

	//TIME DELAYS

	//Roll Initiate
	DelayTimer TD2;
	//Roll Deactivate
	DelayTimer TD3;
};

#define SMJCA_START_STRING		"SMJCA_BEGIN"
#define SMJCB_START_STRING		"SMJCB_BEGIN"
#define SMJC_END_STRING			"SMJC_END"