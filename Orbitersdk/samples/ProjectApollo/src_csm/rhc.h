/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

CSM Rotational Hand Controller (Header)

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

class RHC
{
public:
	RHC();
	void Timestep(int* rhc_pos, bool normdc, bool normac, bool dirdcab, bool dirdcredun);
	void Reset();
	//Signal 1/2 22
	double GetPitchPropRate() { return PropRate.y; }

	//Signal 1/2 23
	double GetYawPropRate() { return PropRate.z; }

	//Signal 1/2 24
	double GetRollPropRate() { return PropRate.x; }

	//Signal AB 25
	bool GetPlusPitchBreakoutSwitch() { return PlusBO[1]; }
	bool GetMinusPitchBreakoutSwitch() { return MinusBO[1]; }

	//Signal AB 26
	bool GetPlusYawBreakoutSwitch() { return PlusBO[2]; }
	bool GetMinusYawBreakoutSwitch() { return MinusBO[2]; }

	//Signal AB 27
	bool GetPlusRollBreakoutSwitch() { return PlusBO[0]; }
	bool GetMinusRollBreakoutSwitch() { return MinusBO[0]; }

	//Signal AB 28
	bool GetPlusPitchHardStopSwitch() { return PlusHO[1]; }
	bool GetMinusPitchHardStopSwitch() { return MinusHO[1]; }

	//Signal AB 29
	bool GetPlusYawHardStopSwitch() { return PlusHO[2]; }
	bool GetMinusYawHardStopSwitch() { return MinusHO[2]; }

	//Signal AB 30
	bool GetPlusRollHardStopSwitch() { return PlusHO[0]; }
	bool GetMinusRollHardStopSwitch() { return MinusHO[0]; }

	//Signal AB 72
	bool GetRedunPlusPitchHardStopSwitch() { return PlusHO2[1]; }
	bool GetRedunMinusPitchHardStopSwitch() { return MinusHO2[1]; }

	//Signal AB 73
	bool GetRedunPlusYawHardStopSwitch() { return PlusHO2[2]; }
	bool GetRedunMinusYawHardStopSwitch() { return MinusHO2[2]; }

	//Signal AB 74
	bool GetRedunPlusRollHardStopSwitch() { return PlusHO2[0]; }
	bool GetRedunMinusRollHardStopSwitch() { return MinusHO2[0]; }
protected:

	VECTOR3 deflection;
	VECTOR3 PropRate;
	bool PlusBO[3];
	bool MinusBO[3];
	bool PlusHO[3];
	bool MinusHO[3];
	bool PlusHO2[3];
	bool MinusHO2[3];
};