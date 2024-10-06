/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Malfunction (Header)

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

#include <string>

class Malfunction
{
public:
	Malfunction(std::string Desc, double FailedOutp = 0.0);

	//Has malfunction been scheduled?
	bool GetActivated();
	//Has malfunction already occured?
	bool GetFailed();
	//Get failure trigger for malfunction e.g. 0 = mission time
	int GetCondition();
	//Get value of failure trigger, e.g. the mission time at which the malfunction will occur
	double GetConditionValue();
	//Returns either 0.0 (malfunction has not occured) or FailedOutput (if malfunction has occured)
	double GetAnalogFailure();
	//Name of malfunction for display
	std::string GetName();

	//Functions for saving/loading
	void SetCondition(int Cond, double CondVal);
	void SetFailed();
	void Clear();
protected:
	bool Activated; //Failure has been armed
	bool Failed; //Failure has occured
	int Condition; //Type of failure trigger (0 = mission time, 1 = simulation time, 2+ vessel specific)
	double ConditionValue; //Value for Condition

	const double FailedOutput; //For analog failures
	std::string Description;
};

class SwitchMalfunction : public Malfunction
{
public:
	SwitchMalfunction(std::string Desc, std::string switchname, int failst);

	int GetFailState() { return fail_state; }
	std::string GetSwitchName() { return SwitchName; }
protected:
	int fail_state;
	std::string SwitchName;
};