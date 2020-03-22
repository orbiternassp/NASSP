/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Coupling Data Unit (Header)

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

#include "apolloguidance.h"
#include "powersource.h"

#define CDU_STEP 9.587379924285257e-05

// The auto optics don't work right with the current logic for some reason. What the CMC wants is a total desired angle change,
// but what it sends is increments that always make the auto optics overshoot the target. This flag can be used to let the CMC
// send the total new value of the error counter to the OCDUs instead of an increment. The downside is that the optics drive never
// reaches the maximum speed, which probably isn't correct either. This logic is only used in the OCDUs and only when the
// coarse align mode is active.
#define OCDU_CA_ERROR_INCREMENT true

class CDU
{
public:
	CDU(ApolloGuidance &comp, int l, int err, int cdutype);
	void Timestep(double simdt);
	void ChannelOutput(int address, ChannelValue val);
	void SetReadCounter(double angle);
	int GetErrorCounter();
	int GetAltOutput();

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
protected:
	void DoZeroCDU();

	uint16_t ReadCounter;
	int ErrorCounter;
	double NewReadCounter;

	int AltOutput;

	bool ZeroCDU;
	bool ErrorCounterEnabled;

	//0 = ICDU, 1 = RR CDU, 2 = OCDU
	int CDUType;
	//Address of AGC CDU counter
	int loc;
	//Channel for error counter commands
	int err_channel;

	//Output channel bit for CDU Zeroing
	int CDUZeroBit;
	//Output channel bit for error counter enabled
	int ErrorCounterBit;

	int AltOutBit;

	//Mode switches
	bool CA;

	ApolloGuidance &agc;
};