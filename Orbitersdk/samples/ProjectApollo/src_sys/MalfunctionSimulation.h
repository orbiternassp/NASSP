/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Malfunction Simulation (Header)

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

#include <vector>
#include "Orbitersdk.h"

class Malfunction;
class SwitchMalfunction;

class MalfunctionSimulation
{
public:
	MalfunctionSimulation();
	virtual ~MalfunctionSimulation();

	void Timestep(); //Update state of malfunctions

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	bool GetFailure(unsigned i); // Function to call by any system that checks on a failure condition
	double GetAnalogFailure(unsigned i); //Analog failure, e.g. a sudden shift in an accelerometer bias
	virtual unsigned int GetNumberOfMalfunctions(); //All, not only the active ones
	virtual std::string GetName(unsigned i);

	//Functions to set up the failures, e.g. by the PAMFD
	virtual void ArmFailure(unsigned i, int cond, double condval);
	virtual void ArmFailure(unsigned i);
	virtual void ResetFailureArm(unsigned i);
	virtual bool IsFailureArmed(unsigned i);
	virtual int GetCondition(unsigned i);
	virtual double GetConditionValue(unsigned i);
	virtual void ClearAllFailures();
protected:

	void Add(Malfunction *mal); //Add malfunctions, should be in the vessel specific constructor of derived classes

	virtual double GetTimeReference(int i) = 0; // Return the current time in vessel specific reference systems, e.g. mission time
	virtual void SetFailure(unsigned i) = 0; // For failures that need signals to some external system, send the signal in this function
	virtual void ResetFailure(unsigned i) = 0; // Same as SetFailure, but to reset signals
	virtual void SetSwitchFailure(unsigned i, bool set) = 0;

	void RandomizedFailure(unsigned i, double FailureChance); //Helper function to randomize failures

	void DeleteSwitchMalfunctions();

	virtual int GetDamageModel() = 0; //Check if failures are to be simulated

	std::vector<Malfunction*> malfunctions; //Permanently set up malfunctions for the vessel
	std::vector<SwitchMalfunction*> switchmalfunctions; //Dynamically loaded when failures are desired
};

#define FAILURES_START_STRING "FAILURES_BEGIN"
#define FAILURES_END_STRING "FAILURES_END"