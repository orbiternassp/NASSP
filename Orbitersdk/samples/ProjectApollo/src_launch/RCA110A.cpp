/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  RCA-110A Saturn Ground Computer

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

#include <vector>
#include "Orbitersdk.h"
#include "PadLCCInterface.h"
#include "LCCPadInterface.h"
#include "RCA110A.h"

ATOLLSequence::ATOLLSequence()
{
	Clear();
}

void ATOLLSequence::Clear()
{
	Step = 0;
	SubStep = 0;
	Condition = 0;
	Value = 0.0;
	LowerLimit = 0.0;
	UpperLimit = 0.0;
	Time = 0.0;
}

bool ATOLLSequence::ReadIn(const char *str)
{
	int i = 0;

	if (str[i] == '\0')
	{
		return false;
	}

	std::string word;
	std::vector<std::string> seq;

	do
	{
		if (str[i] == ';')
		{
			seq.push_back(word);
			word.clear();
		}
		else
		{
			word.push_back(str[i]);
		}
		if (seq.size() == 10)
		{
			break;
		}
		i++;
		if (str[i] == '\0')
		{
			seq.push_back(word);
		}
	} while (str[i] != '\0' && seq.size() < 10);

	if (seq.size() != 10)
	{
		return false;
	}
	Step = atoi(seq[0].c_str());
	SubStep = atoi(seq[1].c_str());
	Operator = seq[2];
	Condition = atoi(seq[3].c_str());
	Value = atof(seq[4].c_str());
	LowerLimit = atof(seq[5].c_str());
	UpperLimit = atof(seq[6].c_str());
	Units = seq[7];
	Time = atof(seq[8].c_str());
	Variable = seq[9];

	return true;
}

int ATOLLSequence::GetOperator()
{
	if (Operator == "DISO")
	{
		return ATOLL_DISO;
	}
	else if (Operator == "DELY")
	{
		return ATOLL_DELY;
	}
	else if (Operator == "SCAN")
	{
		return ATOLL_SCAN;
	}
	else if (Operator == "SSEL")
	{
		return ATOLL_SSEL;
	}

	return -1;
}

ATOLLProcessor::ATOLLProcessor(RCA110AL *r)
{
	rca110a = r;
	opcode = 0;
	nextitemtime = 0.0;
	skipgetline = false;
	delaystatus = false;
}

void ATOLLProcessor::Timestep(double simt)
{
	if (ifs.is_open() == false) return;

	simtime = simt;
	if (simtime < nextitemtime) return;
	if (skipgetline == false)
	{
		if (!ifs.getline(line, 100))
		{
			ifs.close();
		}
	}
	seq.Clear();
	if (seq.ReadIn(line))
	{
		opcode = seq.GetOperator();
		switch (opcode)
		{
		case ATOLL_DELY:
			DELY();
			break;
		case ATOLL_DISO:
			DISO();
			break;
		case ATOLL_SCAN:
			SCAN();
			break;
		case ATOLL_SSEL:
			SSEL();
			break;
		}
	}
}

void ATOLLProcessor::DELY()
{
	if (seq.Time <= 0) return;
	nextitemtime = simtime + seq.Time / 1000.0;
}

void ATOLLProcessor::DISO()
{
	if (seq.Variable.size() == 0) return;
	if (seq.Variable[0] == 'D')
	{
		//Discrete output
		int chan;
		if (sscanf_s(seq.Variable.c_str(), "D%d", &chan) == 1)
		{
			bool on;

			if (seq.Condition == 1)
				on = true;
			else
				on = false;
			rca110a->SetConnectedOutput(chan, on);
		}
	}
}

void ATOLLProcessor::SCAN()
{
	if (seq.Time > 0)
	{
		if (skipgetline == false)
		{
			nextitemtime = simtime + seq.Time / 1000.0;
			skipgetline = true;
			delaystatus = true;
		}
		else
		{
			delaystatus = false;
		}
	}

	if (delaystatus) return;

	//Do Scanning
	delaystatus = false;
	skipgetline = false;
}

void ATOLLProcessor::SSEL()
{
	int stage, chan;

	if (sscanf_s(seq.Variable.c_str(), "%d,%d", &stage, &chan) == 2)
	{
		rca110a->IssueSwitchSelectorCmd(stage, chan);
	}
}

void ATOLLProcessor::ReadFile(const char *str)
{
	ifs.open(str);
}

RCA110A::RCA110A()
{
	mode = 0;
	other = NULL;
}

RCA110A::~RCA110A()
{
	Disconnect();
}

void RCA110A::Connect(RCA110A *o)
{
	if (o)
	{
		other = o;
		other->other = this;
	}
}

void RCA110A::Disconnect()
{
	if (other)
	{
		other->other = NULL;
		other = NULL;
	}
}

void RCA110A::SetInput(size_t n, bool val)
{
	inputdiscretes.set(n, val);
}

void RCA110A::SetOutput(size_t n, bool val)
{
	outputdiscretes.set(n, val);
}

void RCA110A::SetConnectedOutput(size_t n, bool val)
{
	if (other)
	{
		other->SetOutput(n, val);
	}
}

bool RCA110A::GetInputSignal(size_t n) const
{
	return inputdiscretes.test(n);
}

bool RCA110A::GetOutputSignal(size_t n) const
{
	return outputdiscretes.test(n);
}

void RCA110A::SwitchMode(int m)
{
	if (mode >= 0 && mode <= 4)
	{
		mode = m;
	}
}

RCA110AL::RCA110AL(PadLCCInterface *l) : atoll(this)
{
	lcc = l;
}

void RCA110AL::Timestep(double simt, double simdt)
{
	atoll.Timestep(simt);
	lcc->SLCCCheckDiscreteInput(this);

	if (other == NULL) return;

	size_t n;
	for (size_t i = 0;i < RCA110A_INPUT_LINES;i++)
	{
		//First 696 signals are set directly
		if (i <= 695)
		{
			other->SetOutput(i, GetInputSignal(i));
		}

		//TBD: 696 to 849, switch selector commands

		//These switch signals on or off
		if (i >= 1104 && GetInputSignal(i))
		{
			n = i / 2 + 504;
			other->SetOutput(n, ~(i % 2));
		}
	}
}

void RCA110AL::ReadFile(const char *str)
{
	atoll.ReadFile(str);
}

void RCA110AL::IssueSwitchSelectorCmd(int stage, int chan)
{
	if (other)
	{
		other->IssueSwitchSelectorCmd(stage, chan);
	}
}

RCA110AM::RCA110AM(LCCPadInterface *m)
{
	pad = m;

	//Default outputs

	//FCC Power On
	outputdiscretes.set(1823, true);
	//Thrust OK Indicate Enable Inhibit A
	outputdiscretes.set(1913, true);
	//Thrust OK Indicate Enable Inhibit B
	outputdiscretes.set(1914, true);
}

void RCA110AM::Timestep(double simt, double simdt)
{
	pad->SLCCCheckDiscreteInput(this);

	if (other == NULL) return;

	for (size_t i = 0;i < RCA110A_OUTPUT_LINES;i++)
	{
		//For now all inputs are directly sent to the LCC computer
		other->SetOutput(i, GetInputSignal(i));
	}

	//TestProgram();
}

void RCA110AM::IssueSwitchSelectorCmd(int stage, int chan)
{
	pad->IssueSwitchSelectorCmd(stage, chan);
}