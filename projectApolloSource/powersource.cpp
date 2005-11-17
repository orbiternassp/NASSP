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
	next_source = 0;
}

PowerSource::~PowerSource()

{
	// Nothing for now.
}

//
// Default to passing calls on.
//

double PowerSource::Voltage()

{
	if (next_source)
		return next_source->Voltage();

	return 0.0;
}

void PowerSource::DrawPower(double watts)

{
	if (next_source)
		next_source->DrawPower(watts);
}

double PowerBreaker::Voltage()

{
	if (!IsOpen() && next_source)
		return next_source->Voltage();

	return 0.0;
}

double PowerSDKObject::Voltage()

{
	if (SDKObj)
		return SDKObj->Volts;

	return 0.0;
}

void PowerSDKObject::DrawPower(double watts)

{
	//
	// Nothing for now.
	//
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

void PowerMerge::DrawPower(double watts)

{
	//
	// I guess we should draw power from each source proportionally to the
	// voltage?
	//
}