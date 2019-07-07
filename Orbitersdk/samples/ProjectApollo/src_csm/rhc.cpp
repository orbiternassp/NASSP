/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

CSM Rotational Hand Controller

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
#include "nasspsound.h"
#include "toggleswitch.h"
#include "rhc.h"

RHC::RHC()
{
	deflection = _V(0, 0, 0);
	Reset();
}

void RHC::Reset()
{
	PropRate = _V(0, 0, 0);

	for (int i = 0;i < 3;i++)
	{
		PlusBO[i] = false;
		MinusBO[i] = false;
		PlusHO[i] = false;
		MinusHO[i] = false;
		PlusHO2[i] = false;
		MinusHO2[i] = false;
	}
}

void RHC::Timestep(int* rhc_pos, bool normdc, bool normac, bool dirdcab, bool dirdcredun)
{
	//Map to -11.5° to 11.5°
	for (int i = 0;i < 3;i++)
	{
		deflection.data[i] = ((double)(rhc_pos[i] - 32768))*23.0 / 65536.0;
	}


	//Proportional Rate
	if (normac)
	{
		PropRate = deflection / 11.5;
	}
	else
	{
		PropRate = _V(0, 0, 0);
	}

	//Breakout Switches
	if (normdc)
	{
		if (deflection.x > 1.5)
			PlusBO[0] = true;
		else
			PlusBO[0] = false;

		if (deflection.x < -1.5)
			MinusBO[0] = true;
		else
			MinusBO[0] = false;

		if (deflection.y > 1.5)
			PlusBO[1] = true;
		else
			PlusBO[1] = false;

		if (deflection.y < -1.5)
			MinusBO[1] = true;
		else
			MinusBO[1] = false;

		if (deflection.z > 1.5)
			PlusBO[2] = true;
		else
			PlusBO[2] = false;

		if (deflection.z < -1.5)
			MinusBO[2] = true;
		else
			MinusBO[2] = false;
	}
	else
	{
		PlusBO[0] = false;
		PlusBO[1] = false;
		PlusBO[2] = false;
		MinusBO[0] = false;
		MinusBO[1] = false;
		MinusBO[2] = false;
	}

	//Hard Stop Switches
	if (dirdcab)
	{
		if (deflection.x > 11.0)
			PlusHO[0] = true;
		else
			PlusHO[0] = false;

		if (deflection.x < -11.0)
			MinusHO[0] = true;
		else
			MinusHO[0] = false;

		if (deflection.y > 11.0)
			PlusHO[1] = true;
		else
			PlusHO[1] = false;

		if (deflection.y < -11.0)
			MinusHO[1] = true;
		else
			MinusHO[1] = false;

		if (deflection.z > 11.0)
			PlusHO[2] = true;
		else
			PlusHO[2] = false;

		if (deflection.z < -11.0)
			MinusHO[2] = true;
		else
			MinusHO[2] = false;
	}
	else
	{
		PlusHO[0] = false;
		PlusHO[1] = false;
		PlusHO[2] = false;
		MinusHO[0] = false;
		MinusHO[1] = false;
		MinusHO[2] = false;
	}

	//Redundant Hard Stop Switches
	if (dirdcredun)
	{
		if (deflection.x > 11.0)
			PlusHO2[0] = true;
		else
			PlusHO2[0] = false;

		if (deflection.x < -11.0)
			MinusHO2[0] = true;
		else
			MinusHO2[0] = false;

		if (deflection.y > 11.0)
			PlusHO2[1] = true;
		else
			PlusHO2[1] = false;

		if (deflection.y < -11.0)
			MinusHO2[1] = true;
		else
			MinusHO2[1] = false;

		if (deflection.z > 11.0)
			PlusHO2[2] = true;
		else
			PlusHO2[2] = false;

		if (deflection.z < -11.0)
			MinusHO2[2] = true;
		else
			MinusHO2[2] = false;
	}
	else
	{
		PlusHO2[0] = false;
		PlusHO2[1] = false;
		PlusHO2[2] = false;
		MinusHO2[0] = false;
		MinusHO2[1] = false;
		MinusHO2[2] = false;
	}
}