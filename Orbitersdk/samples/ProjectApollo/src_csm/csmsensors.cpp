/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2018

  CSM Specific Sensors

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
#include "nasspdefs.h"
#include "soundlib.h"
#include "saturn.h"
#include "csmsensors.h"

CSMTankTransducer::CSMTankTransducer(char *i_name, double minIn, double maxIn) :
	Transducer(i_name, minIn, maxIn, 0.0, 5.0)
{

}

void CSMTankTransducer::Init(e_object *e, h_Tank *t)
{
	tank = t;
	WireTo(e);
}

CSMTankTempTransducer::CSMTankTempTransducer(char *i_name, double minIn, double maxIn) :
	CSMTankTransducer(i_name, minIn, maxIn)
{

}

double CSMTankTempTransducer::GetValue()
{
	if (tank) return KelvinToFahrenheit(tank->GetTemp());

	return 0.0;
}

CSMTankPressTransducer::CSMTankPressTransducer(char *i_name, double minIn, double maxIn) :
	CSMTankTransducer(i_name, minIn, maxIn)
{

}

double CSMTankPressTransducer::GetValue()
{
	if (tank) return tank->space.Press*PSI;

	return 0.0;
}

CSMCO2PressTransducer::CSMCO2PressTransducer(char *i_name, double minIn, double maxIn) :
	CSMTankTransducer(i_name, minIn, maxIn)
{
	scaleFactor = (maxIn - minIn) / 25.0;
}

double CSMCO2PressTransducer::Voltage()
{
	if (!IsPowered())
		return 0.0;

	double v = sqrt(max(0.0, (GetValue() - minInputValue) / scaleFactor));

	if (v < 0.0)
		return 0.0;

	double inVolts = SRC ? SRC->Voltage() : 0.0;

	if (v > inVolts)
		return inVolts;

	return v;
}

double CSMCO2PressTransducer::GetValue()
{
	if (tank) return tank->space.composition[4].p_press*MMHG;

	return 0.0;
}

CSMTankQuantityTransducer::CSMTankQuantityTransducer(char *i_name, double minIn, double maxIn, double tm) :
	CSMTankTransducer(i_name, minIn, maxIn)
{
	totalMass = tm;
}

double CSMTankQuantityTransducer::GetValue()
{
	if (tank) return tank->mass / totalMass;

	return 0.0;
}

CSMDeltaPressTransducer::CSMDeltaPressTransducer(char *i_name, double minIn, double maxIn) :
	Transducer(i_name, minIn, maxIn, 0.0, 5.0)
{

}

void CSMDeltaPressTransducer::Init(e_object *e, h_Tank *t1, h_Tank *t2)
{
	tank1 = t1;
	tank2 = t2;
	WireTo(e);
}

CSMDeltaPressINH2OTransducer::CSMDeltaPressINH2OTransducer(char *i_name, double minIn, double maxIn) :
	CSMDeltaPressTransducer(i_name, minIn, maxIn)
{

}

double CSMDeltaPressINH2OTransducer::GetValue()
{
	if (tank1 && tank2) return (tank1->space.Press - tank2->space.Press)*INH2O;

	return 0.0;
}

CSMDeltaPressPSITransducer::CSMDeltaPressPSITransducer(char *i_name, double minIn, double maxIn) :
	CSMDeltaPressTransducer(i_name, minIn, maxIn)
{

}

double CSMDeltaPressPSITransducer::GetValue()
{
	if (tank1 && tank2) return (tank1->space.Press - tank2->space.Press)*PSI;

	return 0.0;
}

CSMEvaporatorTransducer::CSMEvaporatorTransducer(char *i_name, double minIn, double maxIn) :
	Transducer(i_name, minIn, maxIn, 0.0, 5.0)
{

}

void CSMEvaporatorTransducer::Init(e_object *e, h_Evaporator *ev)
{
	evap = ev;
	WireTo(e);
}

CSMEvaporatorPressTransducer::CSMEvaporatorPressTransducer(char *i_name, double minIn, double maxIn) :
	CSMEvaporatorTransducer(i_name, minIn, maxIn)
{

}

double CSMEvaporatorPressTransducer::GetValue()
{
	if (evap) return evap->steamPressure*PSI;

	return 0.0;
}


CSMPipeFlowTransducer::CSMPipeFlowTransducer(char *i_name, double minIn, double maxIn) :
	Transducer(i_name, minIn, maxIn, 0.0, 5.0)
{

}

void CSMPipeFlowTransducer::Init(e_object *e, h_Pipe *p)
{
	pipe = p;
	WireTo(e);
}

double CSMPipeFlowTransducer::GetValue()
{
	if (pipe) return pipe->flow*LBH;

	return 0.0;
}