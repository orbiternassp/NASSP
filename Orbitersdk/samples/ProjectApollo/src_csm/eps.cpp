/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2024

Command Module Electrical Power Subsystem

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

#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Hsystems.h"

#include "soundlib.h"
#include "nasspdefs.h"
#include "toggleswitch.h"

#include "saturn.h"

CryoPressureSwitch::CryoPressureSwitch()
{
	saturn = NULL;
	lowpress = 0;
	highpress = 0;
	PressureSwitch1 = false;
	PressureSwitch2 = false;
	dcacontrol = NULL;
	dcbcontrol = NULL;
}

CryoPressureSwitch::~CryoPressureSwitch() 
{

}

void CryoPressureSwitch::Init(Saturn *s, h_Tank *tnk1, h_Tank *tnk2, Boiler *htr1, Boiler *htr2, Boiler *fn1, Boiler *fn2, 
	ThreePosSwitch *htrsw1, ThreePosSwitch *htrsw2, ThreePosSwitch *fnsw1, ThreePosSwitch *fnsw2, DCBusController* dcacont, DCBusController* dcbcont,
	double lp, double hp)
{
	saturn = s;
	tank1 = tnk1;
	tank2 = tnk2;
	heater1 = htr1;
	heater2 = htr2;
	fan1 = fn1;
	fan2 = fn2;

	htrswitch1 = htrsw1;
	htrswitch2 = htrsw2;
	fanswitch1 = fnsw1;
	fanswitch2 = fnsw2;

	dcacontrol = dcacont;
	dcbcontrol = dcbcont;

	lowpress = lp;
	highpress = hp;
}

void CryoPressureSwitch::SystemTimestep(double simdt)
{
	if (!tank1 || !tank2) return;

	//Tank 1 Pressure Switch
	if (PressureSwitch1)
	{
		if (tank1->space.Press > (highpress / PSI))
		{
			PressureSwitch1 = false;
		}
	}
	else
	{
		if (tank1->space.Press < (lowpress / PSI))
		{
			PressureSwitch1 = true;
		}
	}

	//Tank 2 Pressure Switch
	if (PressureSwitch2)
	{
		if (tank2->space.Press > (highpress / PSI))
		{
			PressureSwitch2 = false;
		}
	}
	else
	{
		if (tank2->space.Press < (lowpress / PSI))
		{
			PressureSwitch2 = true;
		}
	}

	//Tank 1 Heater Control
	//AUTO
	if (PressureSwitch1 == true && PressureSwitch2 == true && htrswitch1->GetState() == THREEPOSSWITCH_UP)
	{
		heater1->SetPumpOn();
	}
	//ON
	else if (htrswitch1->GetState() == THREEPOSSWITCH_DOWN)
	{
		heater1->SetPumpOn();
	}
	//OFF
	else
	{
		heater1->SetPumpOff();
	}

	//Tank 2 Heater Control
	//AUTO
	if (PressureSwitch1 == true && PressureSwitch2 == true && htrswitch2->GetState() == THREEPOSSWITCH_UP)
	{
		heater2->SetPumpOn();
	}
	//ON
	else if (htrswitch2->GetState() == THREEPOSSWITCH_DOWN)
	{
		heater2->SetPumpOn();
	}
	//OFF
	else
	{
		heater2->SetPumpOff();
	}

	bool SMBusesPowered = (dcacontrol->IsSMBusPowered() || dcbcontrol->IsSMBusPowered());

	//Tank 1 Fan Control
	//AUTO
	if (SMBusesPowered && PressureSwitch1 == true && PressureSwitch2 == true && fanswitch1->GetState() == THREEPOSSWITCH_UP)
	{
		fan1->SetPumpOn();
	}
	//ON
	else if (fanswitch1->GetState() == THREEPOSSWITCH_DOWN)
	{
		fan1->SetPumpOn();
	}
	//OFF
	else
	{
		fan1->SetPumpOff();
	}

	//Tank 2 Fan Control
	//AUTO
	if (SMBusesPowered && PressureSwitch1 == true && PressureSwitch2 == true && fanswitch2->GetState() == THREEPOSSWITCH_UP)
	{
		fan2->SetPumpOn();
	}
	//ON
	else if (fanswitch2->GetState() == THREEPOSSWITCH_DOWN)
	{
		fan2->SetPumpOn();
	}
	//OFF
	else
	{
		fan2->SetPumpOff();
	}
}

void CryoPressureSwitch::LoadState(char *line, int strlen)
{
	int i, j;

	sscanf(line + strlen + 1, "%i %i", &i, &j);

	PressureSwitch1 = (i != 0);
	PressureSwitch2 = (j != 0);
}

void CryoPressureSwitch::SaveState(FILEHANDLE scn, char *name_str)
{
	char buffer[100];

	sprintf(buffer, "%d %d", PressureSwitch1, PressureSwitch2);
	oapiWriteScenario_string(scn, name_str, buffer);
}

//Flood Lights
FloodLights::FloodLights()
{
	saturn = NULL;
	Flood1cb = NULL;
	Flood2cb = NULL;
	FloodPLcb = NULL;
	PNL8_FIXEDsw = NULL;
	FIXEDsw = NULL;
	DIMsw = NULL;
	Rotary = NULL;
}

FloodLights::~FloodLights()
{

}

void FloodLights::Init(Saturn *s, e_object *flood_src1, e_object *flood_src2, e_object *flood_pl,
	ThreePosSwitch *pnl8_fixed, ToggleSwitch *fixed, ToggleSwitch *dim, ContinuousRotationalSwitch *rty)
{
	saturn = s;
	Flood1cb = flood_src1;
	Flood2cb = flood_src2;
	FloodPLcb = flood_pl;
	PNL8_FIXEDsw = pnl8_fixed;
	FIXEDsw = fixed;
	DIMsw = dim;
	Rotary = rty;
}

double FloodLights::GetPrimVoltage() //Primary flood bulb voltage and power
{
	if (FIXEDsw == NULL) //checks if LH panel is being used to use correct pointers
	{
		if (Flood1cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_UP)  //Dim 1
		{
			return Flood1cb->Voltage() * Rotary->GetOutput(); //returns bus voltage scaled by rotary position (0-1)
			//watts1 = (volts / 28.0) * 14.0; //returns power draw
		}
		else if (Flood2cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_DOWN && PNL8_FIXEDsw->GetState() == THREEPOSSWITCH_UP) //Dim 2 Fixed
		{
			return Flood2cb->Voltage(); //returns bus voltage
			//watts2 = (volts / 28.0) * 14.0; //returns power draw
		}
		else if (FloodPLcb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_DOWN && PNL8_FIXEDsw->GetState() == THREEPOSSWITCH_DOWN) //Dim 2 PL
		{
			return  FloodPLcb->Voltage(); //returns bus voltage
			//wattsPL = (volts / 28.0) * 14.0; //returns power draw
		}
	}
	else if (PNL8_FIXEDsw == NULL)
	{
		if (Flood1cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_UP)  //Dim 1
		{
			return  Flood1cb->Voltage() * Rotary->GetOutput(); //returns bus voltage scaled by rotary position (0-1)
			//watts1 = (volts / 28.0) * 14.0; //returns power draw
		}
		else if (Flood2cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_DOWN && FIXEDsw->GetState() == TOGGLESWITCH_UP) //Dim 2 Fixed
		{
			return Flood2cb->Voltage(); //returns bus voltage
			//watts2 = (volts / 28.0) * 14.0; //returns power draw
		}
	}
	return 0.0;
}

double FloodLights::GetSecVoltage() //Secondary flood bulb voltage and power
{
	if (FIXEDsw = NULL) //checks if LH panel is being used to use correct pointers
	{
		if (Flood1cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_DOWN)  //Dim 2
		{
			return Flood1cb->Voltage() * Rotary->GetOutput(); //returns bus voltage scaled by rotary position (0-1)
			//watts1 = (volts / 28.0) * 14.0; //returns power draw
		}
		else if (Flood2cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_UP && PNL8_FIXEDsw->GetState() == THREEPOSSWITCH_UP) //Dim 1 Fixed
		{
			return Flood2cb->Voltage(); //returns bus voltage
			//watts2 = (volts / 28.0) * 14.0; //returns power draw
		}
		else if (FloodPLcb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_UP && PNL8_FIXEDsw->GetState() == THREEPOSSWITCH_DOWN) //Dim 1 PL
		{
			return FloodPLcb->Voltage(); //returns bus voltage
			//wattsPL = (volts / 28.0) * 14.0; //returns power draw
		}
	}
	else if (PNL8_FIXEDsw == NULL)
	{
		if (Flood1cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_DOWN)  //Dim 2
		{
			return Flood1cb->Voltage() * Rotary->GetOutput(); //returns bus voltage scaled by rotary position (0-1)
			//watts1 = (volts / 28.0) * 14.0; //returns power draw
		}
		else if (Flood2cb->Voltage() > SP_MIN_DCVOLTAGE && DIMsw->GetState() == TOGGLESWITCH_UP && FIXEDsw->GetState() == TOGGLESWITCH_UP) //Dim 1 Fixed
		{
			return Flood2cb->Voltage(); //returns bus voltage
			//watts2 = (volts / 28.0) * 14.0; //returns power draw
		}
	}
	return 0.0;
}

void FloodLights::Timestep(double simdt)
{
	//Can be used to light floods
}

void FloodLights::SystemTimestep(double simdt)
{
	//Power draw 
}
