/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Signal Conditioning Equipment (Header)

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

class Saturn;

class SCE
{
public:
	SCE();
	void Init(Saturn *v);
	void Timestep();
	void SystemTimestep();
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	double GetVoltage(int n, int m);

	double GetSensorExcitation5V();
	double GetSensorExcitation10V();
	double GetSCEPosSupplyVolts();
	double GetSCENegSupplyVolts();
protected:
	bool IsPowered();
	void Reset();
	double scale_data(double data, double low, double high);

	//DC Active Attenuator
	double AA[15];

	//DC Differential Amplifier
	double DA[12];

	//DC Differential Bridge Amplifier
	double DBA[11];

	//AC to DC Converter
	double AC[2];

	Saturn *sat;
	bool Operate;
	bool FF;
};

#define SCE_START_STRING   "SCE_BEGIN"
#define SCE_END_STRING     "SCE_END"