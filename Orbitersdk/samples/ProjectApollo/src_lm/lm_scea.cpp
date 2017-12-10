/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Signal-Conditioning Electronics Assembly

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
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "lm_scea.h"

SCEA_SA_5011::SCEA_SA_5011()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5022::SCEA_SA_5022()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5032::SCEA_SA_5032()
{
	for (int i = 0;i < 3;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5041::SCEA_SA_5041()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5042::SCEA_SA_5042()
{
	for (int i = 0;i < 10;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5043::SCEA_SA_5043()
{
	for (int i = 0;i < 12;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5044::SCEA_SA_5044()
{
	for (int i = 0;i < 12;i++)
	{
		Output[i] = 0.0;
		SolidStateSwitch[i] = false;
	}
}

void SCEA_SA_5044::SetOutput(int n, bool val)
{
	Output[n - 1] = val ? 5.0 : 0.0;
	SolidStateSwitch[n - 1] = val;
}

SCEA_SA_5045::SCEA_SA_5045()
{
	for (int i = 0;i < 12;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5051::SCEA_SA_5051()
{
	for (int i = 0;i < 3;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5062::SCEA_SA_5062()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

SCEA_SA_5071::SCEA_SA_5071()
{
	for (int i = 0;i < 4;i++)
	{
		Output[i] = 0.0;
	}
}

void SCERA::Init(LEM *l)
{
	lem = l;
}

double SCERA::scale_data(double data, double low, double high)
{
	double step = 0;

	// First eliminate cases outside of the scales
	if (data >= high) { return 5.0; }
	if (data <= low) { return 0.0; }

	// Now figure step value
	step = ((high - low) / 5.0);
	// and return result
	return (data - low) / step;
}

SCERA1::SCERA1()
{

}

void SCERA1::Timestep()
{
	//APS regulator outlet manifold pressure (GP0025)
	SA8.SetOutput(3, scale_data(lem->APSPropellant.GetHeliumRegulator1OutletPressurePSI(), 0.0, 300.0));
	//APS helium tank no. 1 pressure (GP0001)
	SA8.SetOutput(4, scale_data(lem->APSPropellant.GetAscentHelium1PressPSI(), 0.0, 4000.0));

	//APS helium primary line solenoid valve closed (GP0318)
	SA12.SetOutput(6, !lem->APSPropellant.GetHeliumValve1()->IsOpen());
	//APS helium secondary line solenoid valve closed (GP0320)
	SA12.SetOutput(7, !lem->APSPropellant.GetHeliumValve2()->IsOpen());

	//Automatic thrust command voltage (GH1331)
	SA15.SetOutput(1, scale_data(lem->deca.GetAutoThrustVoltage(), 0.0, 12.0));
	//Manual thrust command voltage (GH1311)
	SA15.SetOutput(2, scale_data(lem->deca.GetManualThrustVoltage(), 0.0, 14.6));
	//Commander's bus voltage (GC0301)
	SA15.SetOutput(3, scale_data(lem->CDRs28VBus.Voltage(), 0.0, 40.0));
	//Abort sensor assembly voltage (GH3215)
	SA15.SetOutput(4, scale_data(lem->SCS_ASA_CB.Voltage(), 0.0, 40.0));

	//Inverter bus voltage (GC0071)
	SA17.SetOutput(1, scale_data(lem->AC_A_BUS_VOLT_CB.Voltage(), 0.0, 125.0));

	//Commander's bus voltage (GC0301)
	SA18.SetOutput(3, scale_data(lem->CDRs28VBus.Voltage(), 0.0, 40.0));

	//APS helium tank no. 2 pressure (GP0002)
	SA19.SetOutput(1, scale_data(lem->APSPropellant.GetAscentHelium2PressPSI(), 0.0, 4000.0));
	//DPS helium regulator output manifold pressure (GQ3018)
	SA19.SetOutput(2, scale_data(lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI(), 0.0, 300.0));
	//APS fuel bipropellant valve inlet pressure (GP1501)
	SA19.SetOutput(3, scale_data(lem->APSPropellant.GetFuelTrimOrificeOutletPressurePSI(), 0.0, 250.0));
	//APS oxidizer bipropellant valve inlet pressure (GP1503)
	SA19.SetOutput(4, scale_data(lem->APSPropellant.GetOxidTrimOrificeOutletPressurePSI(), 0.0, 250.0));
}

double SCERA1::GetVoltage(int sa, int chan)
{
	if (sa == 2)
	{
		return SA2.GetVoltage(chan);
	}
	else if (sa == 3)
	{
		return SA3.GetVoltage(chan);
	}
	else if (sa == 4)
	{
		return SA4.GetVoltage(chan);
	}
	else if (sa == 5)
	{
		return SA5.GetVoltage(chan);
	}
	else if (sa == 6)
	{
		return SA6.GetVoltage(chan);
	}
	else if (sa == 7)
	{
		return SA7.GetVoltage(chan);
	}
	else if (sa == 8)
	{
		return SA8.GetVoltage(chan);
	}
	else if (sa == 9)
	{
		return SA9.GetVoltage(chan);
	}
	else if (sa == 10)
	{
		return SA10.GetVoltage(chan);
	}
	else if (sa == 11)
	{
		return SA11.GetVoltage(chan);
	}
	else if (sa == 12)
	{
		return SA12.GetVoltage(chan);
	}
	else if (sa == 13)
	{
		return SA13.GetVoltage(chan);
	}
	else if (sa == 14)
	{
		return SA14.GetVoltage(chan);
	}
	else if (sa == 15)
	{
		return SA15.GetVoltage(chan);
	}
	else if (sa == 16)
	{
		return SA16.GetVoltage(chan);
	}
	else if (sa == 17)
	{
		return SA17.GetVoltage(chan);
	}
	else if (sa == 18)
	{
		return SA18.GetVoltage(chan);
	}
	else if (sa == 19)
	{
		return SA19.GetVoltage(chan);
	}
	else if (sa == 20)
	{
		return SA20.GetVoltage(chan);
	}
	else if (sa == 21)
	{
		return SA21.GetVoltage(chan);
	}

	return 0.0;
}

SCERA2::SCERA2()
{

}

void SCERA2::Timestep()
{

}

double SCERA2::GetVoltage(int sa, int chan)
{
	if (sa == 2)
	{
		return SA2.GetVoltage(chan);
	}
	else if (sa == 3)
	{
		return SA3.GetVoltage(chan);
	}
	else if (sa == 4)
	{
		return SA4.GetVoltage(chan);
	}
	else if (sa == 5)
	{
		return SA5.GetVoltage(chan);
	}
	else if (sa == 6)
	{
		return SA6.GetVoltage(chan);
	}
	else if (sa == 7)
	{
		return SA7.GetVoltage(chan);
	}
	else if (sa == 8)
	{
		return SA8.GetVoltage(chan);
	}
	else if (sa == 9)
	{
		return SA9.GetVoltage(chan);
	}
	else if (sa == 10)
	{
		return SA10.GetVoltage(chan);
	}
	else if (sa == 12)
	{
		return SA12.GetVoltage(chan);
	}
	else if (sa == 13)
	{
		return SA13.GetVoltage(chan);
	}
	else if (sa == 14)
	{
		return SA14.GetVoltage(chan);
	}
	else if (sa == 15)
	{
		return SA15.GetVoltage(chan);
	}
	else if (sa == 16)
	{
		return SA16.GetVoltage(chan);
	}
	else if (sa == 17)
	{
		return SA17.GetVoltage(chan);
	}
	else if (sa == 18)
	{
		return SA18.GetVoltage(chan);
	}
	else if (sa == 19)
	{
		return SA19.GetVoltage(chan);
	}
	else if (sa == 20)
	{
		return SA20.GetVoltage(chan);
	}
	else if (sa == 21)
	{
		return SA21.GetVoltage(chan);
	}

	return 0.0;
}