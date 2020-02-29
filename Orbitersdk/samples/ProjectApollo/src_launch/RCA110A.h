/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  RCA-110A Saturn Ground Computer (Header)

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

#include <bitset>

#define RCA110A_INPUT_LINES 3024
#define RCA110A_OUTPUT_LINES 2016

class PadLCCInterface;
class LCCPadInterface;

class RCA110A
{
public:
	RCA110A();
	virtual ~RCA110A();
	virtual void Timestep(double simt, double simdt);
	void SwitchMode(int m);
	void Connect(RCA110A *o);
	void Disconnect();
	void SetInput(size_t n, bool val);
	void SetOutput(size_t n, bool val);
	bool GetOutputSignal(size_t n) const;

	RCA110A *other;
protected:
	bool GetInputSignal(size_t n) const;
	void TestProgram();
private:
	std::bitset<RCA110A_INPUT_LINES> inputdiscretes;
	std::bitset<RCA110A_OUTPUT_LINES> outputdiscretes;

	//0 = nothing, 1 = single scan mode, 2 = continuous scan mode, 3 = monitor mode, 4 = monitor mode with selectable priority interruot
	int mode;

	double simtime;
	double T0001;
};

//LCC Computer
class RCA110AL : public RCA110A
{
public:
	RCA110AL(PadLCCInterface *l);
	void Timestep(double simt, double simdt);
private:
	PadLCCInterface *lcc;
};

//Pad Computer
class RCA110AM : public RCA110A
{
public:
	RCA110AM(LCCPadInterface *m);
	void Timestep(double simt, double simdt);
private:
	LCCPadInterface *pad;
};