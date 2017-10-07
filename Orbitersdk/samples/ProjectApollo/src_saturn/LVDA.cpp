/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Launch Vehicle Data Adapter

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

#include "Orbitersdk.h"

#include "soundlib.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "LVDA.h"

LVDA::LVDA()
{
	iu = NULL;
}

void LVDA::Init(IU *i)
{
	iu = i;
}

void LVDA::SwitchSelector(int stage, int channel)
{
	if (stage == SWITCH_SELECTOR_IU)
	{
		iu->SwitchSelector(channel);
	}
	/*else if (stage == SWITCH_SELECTOR_IU)
	{
		iu-> SISwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_IU)
	{
		iu->SIISwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_IU)
	{
		iu->SIVBSwitchSelector(channel);
	}*/
}

void LVDA::SetFCCAttitudeError(VECTOR3 atterr)
{
	iu->GetFCC()->SetAttitudeError(atterr);
}

VECTOR3 LVDA::GetLVIMUAttitude()
{
	return iu->lvimu.GetTotalAttitude();
}

void LVDA::ZeroLVIMUPIPACounters()
{
	iu->lvimu.ZeroPIPACounters();
}

void LVDA::ZeroLVIMUCDUs()
{
	iu->lvimu.ZeroIMUCDUFlag = true;
}

void LVDA::ReleaseLVIMUCDUs()
{
	iu->lvimu.ZeroIMUCDUFlag = false;
}

void LVDA::ReleaseLVIMU()
{
	iu->lvimu.SetCaged(false);
}

void LVDA::DriveLVIMUGimbals(double x, double y, double z)
{
	iu->lvimu.DriveGimbals(x, y, z);
}

VECTOR3 LVDA::GetLVIMUPIPARegisters()
{
	return _V(iu->lvimu.CDURegisters[LVRegPIPAX], iu->lvimu.CDURegisters[LVRegPIPAY], iu->lvimu.CDURegisters[LVRegPIPAZ]);
}