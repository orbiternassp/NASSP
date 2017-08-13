/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Thrust/Translation Controller Assembly (Header)

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

class LEM;

class LEM_TTCA
{
public:
	LEM_TTCA();
	void Init(LEM *l);
	void Timestep(int* ttca_pos, int leverposition, bool realistic_throttle, double throttle_pos_dig);

	double GetThrottlePosition() { return throttle; }
	bool GetMinusXTrans() { return S1; }
	bool GetPlusXTrans() { return S2; }
	bool GetPlusYTrans() { return S3; }
	bool GetMinusYTrans() { return S4; }
	bool GetMinusZTrans() { return S5; }
	bool GetPlusZTrans() { return S6; }
protected:
	void ResetSwitches();

	double deflection[3];
	double throttle;

	//Switches

	//-X Translation
	bool S1;
	//+X Translation
	bool S2;
	//+Y Translation
	bool S3;
	//-Y Translation
	bool S4;
	//-Z Translation
	bool S5;
	//+Z Translation
	bool S6;
	//Throttle-Jets Lever
	bool S7;

	LEM *lem;
};
