/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2023

  Apollo Telescope Mount Digital Computer (Header)

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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/NASSP-LICENSE.txt
  for more details.

**************************************************************************/

#pragma once

#include "Orbitersdk.h"

class ATMDC
{
public:
	ATMDC(VESSEL *v);

	void Timestep();
	bool GetThrusterDemand(int i);

	void SetAttitudeControlMode(int mode);
	int GetAttitudeControlMode() { return AttitudeControlMode; }

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	virtual void CommandSystem(int Function, int Command);
private:

	MATRIX3 GetAttitudeMatrix(); //Ecliptic to body (right handed)

	//Internal variables
	int AttitudeControlMode; //0 = free, 1 = attitude hold, 2 = solar inertial, 3 = ZLV (+X to VV), 4 = ZLV (-X to VV), 5 = manual

	MATRIX3 M_AttHold; //Attitude matrix reference for attitude hold (global to body)

	//Constants
	double a_0p, a_0y, a_0r, a_1p, a_1y, a_1r;	//Attitude control gains
	double MaxErr;								//Maximum attitude error to limit the attitude rate

	bool ThrusterDemand[6];

	//Orbiter stuff
	VESSEL *vessel;
	OBJHANDLE hEarth, hSun;
};

#define ATMDC_START_STRING	"ATMDC_BEGIN"
#define ATMDC_END_STRING	"ATMDC_END"