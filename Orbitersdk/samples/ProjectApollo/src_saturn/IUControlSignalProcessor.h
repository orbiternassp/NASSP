/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Control Signal Processor 601A24 (Header)

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

class IU;

class IUControlSignalProcessor
{
public:
	IUControlSignalProcessor(IU *iu);
	void Timestep();

	bool GetPitchNo1Overrate() { return PitchNo1Overrate; }
	bool GetPitchNo2Overrate() { return PitchNo2Overrate; }
	bool GetPitchNo3Overrate() { return PitchNo3Overrate; }
	bool GetYawNo1Overrate() { return YawNo1Overrate; }
	bool GetYawNo2Overrate() { return YawNo2Overrate; }
	bool GetYawNo3Overrate() { return YawNo3Overrate; }
	bool GetRollNo1Overrate() { return RollNo1Overrate; }
	bool GetRollNo2Overrate() { return RollNo2Overrate; }
	bool GetRollNo3Overrate() { return RollNo3Overrate; }
private:
	//K122
	bool PitchNo1Overrate;
	//K123
	bool PitchNo2Overrate;
	//K124
	bool PitchNo3Overrate;
	//K125
	bool YawNo1Overrate;
	//K126
	bool YawNo2Overrate;
	//K127
	bool YawNo3Overrate;
	//K128
	bool RollNo1Overrate;
	//K129
	bool RollNo2Overrate;
	//K130
	bool RollNo3Overrate;

	double PYLimit;
	VECTOR3 AttRate;

	IU *iu;
};