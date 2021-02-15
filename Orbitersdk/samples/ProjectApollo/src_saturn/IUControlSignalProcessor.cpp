/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Control Signal Processor 601A24

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
#include "iu.h"
#include "IUControlSignalProcessor.h"

IUControlSignalProcessor::IUControlSignalProcessor(IU *iu)
{
	this->iu = iu;
	PitchNo1Overrate = false;
	PitchNo2Overrate = false;
	PitchNo3Overrate = false;
	YawNo1Overrate = false;
	YawNo2Overrate = false;
	YawNo3Overrate = false;
	RollNo1Overrate = false;
	RollNo2Overrate = false;
	RollNo3Overrate = false;
	PYLimit = 0.0;
	AttRate = _V(0, 0, 0);
}

void IUControlSignalProcessor::Timestep()
{
	//Signal "A"
	if (iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit())
	{
		PYLimit = 9.2*RAD;
	}
	else
	{
		PYLimit = 4.0*RAD;
	}

	AttRate = iu->GetLVRG()->GetRates();

	if (abs(AttRate.x) > 20.0*RAD)
	{
		RollNo1Overrate = true;
		RollNo2Overrate = true;
		RollNo3Overrate = true;
	}
	else
	{
		RollNo1Overrate = false;
		RollNo2Overrate = false;
		RollNo3Overrate = false;
	}

	if (abs(AttRate.y) > PYLimit)
	{
		PitchNo1Overrate = true;
		PitchNo2Overrate = true;
		PitchNo3Overrate = true;
	}
	else
	{
		PitchNo1Overrate = false;
		PitchNo2Overrate = false;
		PitchNo3Overrate = false;
	}

	if (abs(AttRate.z) > PYLimit)
	{
		YawNo1Overrate = true;
		YawNo2Overrate = true;
		YawNo3Overrate = true;
	}
	else
	{
		YawNo1Overrate = false;
		YawNo2Overrate = false;
		YawNo3Overrate = false;
	}
}