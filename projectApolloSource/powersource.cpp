/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Power distribution code

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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.7  2005/12/02 19:29:24  movieman523
  *	Started integrating PowerSource code into PanelSDK.
  *	
  *	Revision 1.6  2005/11/18 20:38:59  movieman523
  *	Revised condensor output from fuel cell to eliminate master alarms.
  *	
  *	Revision 1.5  2005/11/18 02:40:55  movieman523
  *	Major revamp of PanelSDK electrical code, and various modifications to run off fuel cells.
  *	
  *	Revision 1.4  2005/11/17 21:04:52  movieman523
  *	IMU and AGC now start powered-down. Revised battery code, and wired up all batteries in CSM.
  *	
  *	Revision 1.3  2005/11/17 19:19:12  movieman523
  *	Added three-phase AC bus and battery buses.
  *	
  *	Revision 1.2  2005/11/17 01:52:29  movieman523
  *	Simplified setup for circuit breakers, and added battery buses.
  *	
  *	Revision 1.1  2005/11/16 23:14:02  movieman523
  *	Initial support for wiring in the circuit breakers.
  *	
  **************************************************************************/


#include <stdio.h>
#include "orbitersdk.h"

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "powersource.h"

PowerSource::PowerSource()

{
	SRC = 0;
}

PowerSource::~PowerSource()

{
	// Nothing for now.
}

double PowerSource::Current()

{
	if (SRC)
		return SRC->Current();

	return 0.0;
}

//
// Default to passing calls on.
//

double PowerSource::Voltage()

{
	if (SRC)
		return SRC->Voltage();

	return 0.0;
}

double PowerBreaker::Voltage()

{
	if (!IsOpen() && SRC)
		return SRC->Voltage();

	return 0.0;
}

double PowerSDKObject::Voltage()

{
	if (SDKObj)
		return SDKObj->Voltage();

	return 0.0;
}

double PowerSDKObject::Current()

{
	if (SDKObj)
		return SDKObj->Current();

	return 0.0;
}

double PowerSDKObject::PowerLoad()

{
	if (SDKObj)
		return SDKObj->PowerLoad();

	return 0.0;
}

void PowerSDKObject::DrawPower(double watts)

{
	if (SDKObj)
		SDKObj->DrawPower(watts);
}

//
// Tie power together from two sources. For now we just take the
// largest voltage from both sources.
//

double PowerMerge::Voltage()

{
	double VoltsA = 0.0, VoltsB = 0.0;

	if (BusA)
		VoltsA = BusA->Voltage();
	if (BusB)
		VoltsB = BusB->Voltage();

	if (VoltsA > VoltsB)
		return VoltsA;

	return VoltsB;
}

double PowerMerge::Current()

{
	double current = 0.0;

	if (BusA)
		current += BusA->Current();
	if (BusB)
		current += BusB->Current();

	return current;
}

void PowerMerge::DrawPower(double watts)

{
	double Volts = 0.0;
	double VoltsA = 0.0;
	double VoltsB = 0.0;

	if (BusA)
		VoltsA = BusA->Voltage();
	if (BusB)
		VoltsB = BusB->Voltage();

	Volts = VoltsA + VoltsB;

	if (Volts > 0.0) {
		if (BusA)
			BusA->DrawPower(watts * VoltsA / Volts);
		if (BusB)
			BusB->DrawPower(watts * VoltsB / Volts);
	}
}

//
// Tie power together from three sources. For now we just take the
// largest voltage from all sources.
//

double ThreeWayPowerMerge::Voltage()

{
	double Volts1 = 0.0, Volts2 = 0.0, Volts3 = 0.0;
	double MaxVolts;

	if (Phase1)
		Volts1 = Phase1->Voltage();
	if (Phase2)
		Volts2 = Phase2->Voltage();
	if (Phase3)
		Volts3 = Phase3->Voltage();

	MaxVolts = Volts1;

	if (Volts2 > Volts1)
		MaxVolts = Volts2;
	if (Volts3 > MaxVolts)
		MaxVolts = Volts3;

	return MaxVolts;
}

double ThreeWayPowerMerge::Current()

{
	double Volts = 0.0;
	double VoltsA = 0.0;
	double VoltsB = 0.0;
	double VoltsC = 0.0;

	if (Phase1)
		VoltsA = Phase1->Voltage();
	if (Phase2)
		VoltsB = Phase2->Voltage();
	if (Phase3)
		VoltsC = Phase3->Voltage();

	Volts = VoltsA + VoltsB + VoltsC;

	if (Volts > 0.0) {
		return power_load / Volts;
	}
	return 0.0;
}

void ThreeWayPowerMerge::DrawPower(double watts)

{
	double Volts = 0.0;
	double VoltsA = 0.0;
	double VoltsB = 0.0;
	double VoltsC = 0.0;

	power_load += watts;

	if (Phase1)
		VoltsA = Phase1->Voltage();
	if (Phase2)
		VoltsB = Phase2->Voltage();
	if (Phase3)
		VoltsC = Phase3->Voltage();

	Volts = VoltsA + VoltsB + VoltsC;

	if (Volts > 0.0) {
		if (Phase1)
			Phase1->DrawPower(watts * VoltsA / Volts);
		if (Phase2)
			Phase2->DrawPower(watts * VoltsB / Volts);
		if (Phase3)
			Phase3->DrawPower(watts * VoltsC / Volts);
	}
}
