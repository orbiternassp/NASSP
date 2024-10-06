/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Malfunction

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

#include "Malfunction.h"

Malfunction::Malfunction(std::string Desc, double FailedOutp) : Description(Desc), FailedOutput(FailedOutp)
{
	Activated = false;
	Failed = false;
	Condition = 0;
	ConditionValue = 0.0;
}

bool Malfunction::GetActivated()
{
	return Activated;
}

bool Malfunction::GetFailed()
{
	return Failed;
}

int Malfunction::GetCondition()
{
	return Condition;
}

double Malfunction::GetConditionValue()
{
	return ConditionValue;
}

double Malfunction::GetAnalogFailure()
{
	if (Failed) return FailedOutput;
	return 0.0;
}

std::string Malfunction::GetName()
{
	return Description;
}

void Malfunction::SetCondition(int Cond, double CondVal)
{
	Activated = true;
	Condition = Cond;
	ConditionValue = CondVal;
}

void Malfunction::SetFailed()
{
	Failed = true;
}

void Malfunction::Clear()
{
	Activated = false;
	Failed = false;
	Condition = 0;
	ConditionValue = 0.0;
}

SwitchMalfunction::SwitchMalfunction(std::string Desc, std::string switchname, int failst) : Malfunction(Desc),
SwitchName(switchname), fail_state(failst)
{

}