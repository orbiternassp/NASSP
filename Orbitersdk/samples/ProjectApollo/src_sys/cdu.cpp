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
	ErrorCounter = 0.0;
	OldReadCounter = 0.0;
	ZeroCDU = false;
	ErrorCounterEnabled = false;
}

void CDU::Timestep(double simdt)
{
	ChannelValue val12;

	val12 = agc.GetOutputChannel(012);

	if (val12[CDUZeroBit]) {
		DoZeroCDU();
	}
	else
	{
		ZeroCDU = false;
	}

	if (val12[CDUZeroBit]) {
		DoZeroCDU();
	}
	else
	{
		ZeroCDU = false;
	}

	int  pulses; //i, delta;
	double delta;

	delta = ReadCounter - OldReadCounter;
	if (delta > PI)
		delta -= PI2;
	if (delta < -PI)
		delta += PI2;

	// Gyro pulses to CDU pulses
	pulses = (int)(((double)radToGyroPulses(delta)) / 64.0);

	if (pulses && !ZeroCDU)
		agc.vagc.Erasable[0][loc] += pulses;

	OldReadCounter = ReadCounter;

	//sprintf(oapiDebugString(), "%f %f %o %d %d", ReadCounter, delta, pulses, ZeroCDU, CDUZeroBit);
	sprintf(oapiDebugString(), "%o", ErrorCounter);
}

void CDU::ChannelOutput(int address, ChannelValue val)
{
	ChannelValue val12;

	val12 = agc.GetOutputChannel(012);

	if (val12[CDUZeroBit]) {
		DoZeroCDU();
	}

	if (val12[ErrorCounterBit]) {
		if (ErrorCounterEnabled == false)
		{
			ErrorCounter = 0.0;
			ErrorCounterEnabled = true;
		}
	}
	else
	{
		if (ErrorCounterEnabled == true) {
			ErrorCounter = 0.0;
		}
		ErrorCounterEnabled = false;
	}

	if (address == err_channel)
	{
		if (val12[ErrorCounterBit]) {
			int delta = val.to_ulong() & 0777;
			// NEGATIVE = RIGHT
			if (val.to_ulong() & 040000) {
				ErrorCounter -= delta;
			}
			else {
				ErrorCounter += delta;
			}
		}
	}
}

double CDU::gyroPulsesToRad(int pulses) {
	return (((double)pulses) * PI2) / 2097152.0;
}

int CDU::radToGyroPulses(double angle) {
	return (int)((angle * 2097152.0) / PI2);
}

void CDU::DoZeroCDU()
{
	ReadCounter = 0.0;
	OldReadCounter = 0.0;
	ZeroCDU = true;
}

void CDU::SetReadCounter(double angle)
{
	if (!ZeroCDU)
	{
		ReadCounter = angle;
	}
}