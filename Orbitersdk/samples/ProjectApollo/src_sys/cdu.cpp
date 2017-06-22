/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Coupling Data Unit

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
#include "cdu.h"
#include "papi.h"

CDU::CDU(ApolloGuidance &comp, int l, int err, bool isicdu) : agc(comp)
{
	loc = l;
	err_channel = err;
	IsICDU = isicdu;

	if (IsICDU)
	{
		CDUZeroBit = 4;
		ErrorCounterBit = 5;
	}
	else
	{
		CDUZeroBit = 0;
		ErrorCounterBit = 1;
	}

	ReadCounter = 0.0;
	ErrorCounter = 0;
	NewReadCounter = 0.0;
	ZeroCDU = false;
	ErrorCounterEnabled = false;
	AltOutBit = 5;
	AltOutput = 0;
}

void CDU::Timestep(double simdt)
{
	ChannelValue val12;

	val12 = agc.GetOutputChannel(012);

	if (ZeroCDU == false)
	{
		double delta;

		delta = NewReadCounter - ReadCounter;

		if (delta < 0) {
			while (fabs(fabs(NewReadCounter) - fabs(ReadCounter)) >= CDU_STEP) {
				agc.vagc.Erasable[0][loc]--;
				agc.vagc.Erasable[0][loc] &= 077777;
				ReadCounter -= CDU_STEP;
			}
		}
		if (delta > 0) {
			while (fabs(fabs(NewReadCounter) - fabs(ReadCounter)) >= CDU_STEP) {
				agc.vagc.Erasable[0][loc]++;
				agc.vagc.Erasable[0][loc] &= 077777;
				ReadCounter += CDU_STEP;
			}
		}
	}

	if (!IsICDU && val12[AltOutBit] == 1)
	{
		AltOutput = ErrorCounter;
	}
	else
	{
		AltOutput = 0;
	}

	//sprintf(oapiDebugString(), "ReadCounter %f NewReadCounter %f ZeroCDU %d CDUZeroBit %d", ReadCounter*DEG, NewReadCounter*DEG, ZeroCDU, CDUZeroBit);
	//sprintf(oapiDebugString(), "ReadCounter %f ErrorCounter %d ErrorCounterEnabled %d", ReadCounter*DEG, ErrorCounter, ErrorCounterEnabled);
}

void CDU::ChannelOutput(int address, ChannelValue val)
{
	if (address == 012)
	{
		if (val[CDUZeroBit] == 1) {
			DoZeroCDU();
		}
		else
		{
			ZeroCDU = false;
		}

		if (val[ErrorCounterBit] == 1) {
			if (ErrorCounterEnabled == false)
			{
				ErrorCounter = 0;
				ErrorCounterEnabled = true;
			}
		}
		else
		{
			if (ErrorCounterEnabled == true) {
				ErrorCounter = 0;
			}
			ErrorCounterEnabled = false;
		}
	}

	if (address == err_channel)
	{
		if (ErrorCounterEnabled) {
			int delta = val.to_ulong();

			if (delta & 040000) { // Negative
				ErrorCounter += -((~delta) & 077777);
			}
			else {
				ErrorCounter += delta & 077777;
			}
		}
	}

	//sprintf(oapiDebugString(), "ReadCounter %f ErrorCounter %o Bit %d", ReadCounter*DEG, ErrorCounter, val12[ErrorCounterBit] == 1);
}

void CDU::DoZeroCDU()
{
	ReadCounter = 0;
	NewReadCounter = 0;
	ZeroCDU = true;
}

void CDU::SetReadCounter(double angle)
{
	if (!ZeroCDU)
	{
		NewReadCounter = angle;

		if (NewReadCounter >= PI2) {
			NewReadCounter -= PI2;
		}
		if (NewReadCounter < 0) {
			NewReadCounter += PI2;
		}
	}
}

int CDU::GetErrorCounter()
{ 
	if (ErrorCounterEnabled)
	{
		return ErrorCounter;
	}
	
	return 0;
}

int CDU::GetAltOutput()
{
	return AltOutput;
}

void CDU::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_double(scn, "READCOUNTER", ReadCounter);
	papiWriteScenario_double(scn, "NEWREADCOUNTER", NewReadCounter);
	oapiWriteScenario_int(scn, "ERRORCOUNTER", ErrorCounter);
	papiWriteScenario_bool(scn, "ZEROCDU", ZeroCDU);
	papiWriteScenario_bool(scn, "ERRORCOUNTERENABLED", ErrorCounterEnabled);

	oapiWriteLine(scn, end_str);
}

void CDU::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_double(line, "READCOUNTER", ReadCounter);
		papiReadScenario_double(line, "NEWREADCOUNTER", NewReadCounter);
		papiReadScenario_int(line, "ERRORCOUNTER", ErrorCounter);
		papiReadScenario_bool(line, "ZEROCDU", ZeroCDU);
		papiReadScenario_bool(line, "ERRORCOUNTERENABLED", ErrorCounterEnabled);
	}
}