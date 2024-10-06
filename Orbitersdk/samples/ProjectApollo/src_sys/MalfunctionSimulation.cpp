/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Malfunction Simulation

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
#include "MalfunctionSimulation.h"

MalfunctionSimulation::MalfunctionSimulation()
{

}

MalfunctionSimulation::~MalfunctionSimulation()
{
	for (unsigned i = 0; i < malfunctions.size(); i++)
	{
		delete malfunctions[i];
	}
	malfunctions.clear();

	DeleteSwitchMalfunctions();
}

void MalfunctionSimulation::Timestep()
{
	//Don't simulate failures if it has been disabled in the Orbiter launchpad
	if (GetDamageModel() == 0) return;

	unsigned i;
	bool activate;

	//Digital and analog malfunctions
	for (i = 0; i < malfunctions.size(); i++)
	{
		if (malfunctions[i]->GetActivated() && !malfunctions[i]->GetFailed())
		{
			activate = false;

			//Check on time condition
			if (GetTimeReference(malfunctions[i]->GetCondition()) > malfunctions[i]->GetConditionValue()) activate = true;

			if (activate)
			{
				malfunctions[i]->SetFailed(); //For passive failures
				SetFailure(i); //For active failures
			}
		}
	}

	//Switch malfunctions
	for (i = 0; i < switchmalfunctions.size(); i++)
	{
		if (switchmalfunctions[i]->GetActivated() && !switchmalfunctions[i]->GetFailed())
		{
			activate = false;

			//Check on time condition
			if (GetTimeReference(switchmalfunctions[i]->GetCondition()) > switchmalfunctions[i]->GetConditionValue()) activate = true;

			if (activate)
			{
				SetSwitchFailure(i, true);
			}
		}
	}
}

void MalfunctionSimulation::SaveState(FILEHANDLE scn)
{
	char buffer[128];
	unsigned i;

	oapiWriteLine(scn, FAILURES_START_STRING);

	for (i = 0; i < malfunctions.size(); i++)
	{
		if (malfunctions[i]->GetActivated())
		{
			sprintf(buffer, "%d %d %d %lf", i, malfunctions[i]->GetFailed(), malfunctions[i]->GetCondition(), malfunctions[i]->GetConditionValue());
			oapiWriteScenario_string(scn, "MALFUNCTION", buffer);
		}
	}
	for (i = 0; i < switchmalfunctions.size(); i++)
	{
		if (switchmalfunctions[i]->GetActivated())
		{
			sprintf(buffer, "%s %d %d %lf", switchmalfunctions[i]->GetSwitchName().c_str(), switchmalfunctions[i]->GetFailState(), switchmalfunctions[i]->GetCondition(), switchmalfunctions[i]->GetConditionValue());
			oapiWriteScenario_string(scn, "SWITCHMALFUNCTION", buffer);
		}
	}

	oapiWriteLine(scn, FAILURES_END_STRING);
}

void MalfunctionSimulation::LoadState(FILEHANDLE scn)
{
	char *line;
	unsigned num;
	int itemp[2];
	double val;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!_strnicmp(line, FAILURES_END_STRING, sizeof(FAILURES_END_STRING))) {
			break;
		}

		if (!_strnicmp(line, "MALFUNCTION", 11))
		{
			sscanf(line + 11, "%d %d %d %lf", &num, &itemp[0], &itemp[1], &val);

			if (num < malfunctions.size())
			{
				if (itemp[0] != 0) malfunctions[num]->SetFailed();
				malfunctions[num]->SetCondition(itemp[1], val);
			}
		}
		else if (!_strnicmp(line, "SWITCHMALFUNCTION", 17))
		{
			char name[128];

			sscanf(line + 17, "%s %d %d %lf", name, &itemp[0], &itemp[1], &val);

			SwitchMalfunction *mal = new SwitchMalfunction("", name, itemp[0]);
			mal->SetCondition(itemp[1], val);

			switchmalfunctions.push_back(mal);

			//Set switch failed
			if (mal->GetFailed())
			{
				SetSwitchFailure(switchmalfunctions.size() - 1U, true);
			}
		}
	}
}

void MalfunctionSimulation::ArmFailure(unsigned i)
{
	if (i >= malfunctions.size()) return;

	malfunctions[i]->SetCondition(1, 0.0);
}

void MalfunctionSimulation::ArmFailure(unsigned i, int cond, double condval)
{
	if (i >= malfunctions.size()) return;

	malfunctions[i]->SetCondition(cond, condval);
}

void MalfunctionSimulation::ResetFailureArm(unsigned i)
{
	if (i >= malfunctions.size()) return;

	malfunctions[i]->Clear(); //Internal
	ResetFailure(i); //External
}

bool MalfunctionSimulation::GetFailure(unsigned i)
{
	if (i >= malfunctions.size()) return false;

	return malfunctions[i]->GetFailed();
}

double MalfunctionSimulation::GetAnalogFailure(unsigned i)
{
	if (i >= malfunctions.size()) return 0.0;

	return malfunctions[i]->GetAnalogFailure();
}

unsigned int MalfunctionSimulation::GetNumberOfMalfunctions()
{
	return malfunctions.size();
}

std::string MalfunctionSimulation::GetName(unsigned i)
{
	if (i >= malfunctions.size()) return "None";

	return malfunctions[i]->GetName();
}

bool MalfunctionSimulation::IsFailureArmed(unsigned i)
{
	if (i >= malfunctions.size()) return false;

	return malfunctions[i]->GetActivated();
}

int MalfunctionSimulation::GetCondition(unsigned i)
{
	if (i >= malfunctions.size()) return 0;

	return malfunctions[i]->GetCondition();
}

double MalfunctionSimulation:: GetConditionValue(unsigned i)
{
	if (i >= malfunctions.size()) return 0.0;

	return malfunctions[i]->GetConditionValue();
}

void MalfunctionSimulation::Add(Malfunction *mal)
{
	malfunctions.push_back(mal);
}

void MalfunctionSimulation::RandomizedFailure(unsigned i, double FailureChance)
{
	if (!(rand() & (int)(FailureChance)))
	{
		ArmFailure(i);
	}
	else
	{
		ResetFailureArm(i);
	}
}

void MalfunctionSimulation::ClearAllFailures()
{
	unsigned i;

	for (i = 0; i < malfunctions.size(); i++)
	{
		malfunctions[i]->Clear();
	}
	for (i = 0; i < switchmalfunctions.size(); i++)
	{
		SetSwitchFailure(i, false);
	}
	DeleteSwitchMalfunctions();
}

void MalfunctionSimulation::DeleteSwitchMalfunctions()
{
	for (unsigned i = 0; i < switchmalfunctions.size(); i++)
	{
		delete switchmalfunctions[i];
	}
	switchmalfunctions.clear();
}