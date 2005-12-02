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
  *	Revision 1.6  2005/11/18 20:38:59  movieman523
  *	Revised condensor output from fuel cell to eliminate master alarms.
  *	
  *	Revision 1.5  2005/11/18 02:40:55  movieman523
  *	Major revamp of PanelSDK electrical code, and various modifications to run off fuel cells.
  *	
  *	Revision 1.4  2005/11/17 19:19:12  movieman523
  *	Added three-phase AC bus and battery buses.
  *	
  *	Revision 1.3  2005/11/17 01:52:29  movieman523
  *	Simplified setup for circuit breakers, and added battery buses.
  *	
  *	Revision 1.2  2005/11/17 01:23:11  movieman523
  *	Revised circuit breaker code. Now all switchers are PowerSources, so no need for the seperate PowerBreaker class.
  *	
  *	Revision 1.1  2005/11/16 23:14:02  movieman523
  *	Initial support for wiring in the circuit breakers.
  *	
  **************************************************************************/

#if !defined(_PA_POWERSOURCE_H)
#define _PA_POWERSOURCE_H

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/ESystems.h"

class PowerSource : public e_object {

public:
	PowerSource();
	~PowerSource();

	void WireTo(e_object *p) { SRC = p; };
	bool IsWired() { return (SRC != 0); };
	double Voltage();
	double Current();

protected:
};

class PowerMerge : public PowerSource {
public:
	PowerMerge() { BusA = 0; BusB = 0; };
	double Voltage();
	void DrawPower(double watts);
	void WireToBuses(PowerSource *a, PowerSource *b) { BusA = a; BusB = b; };
	double Current();

protected:
	PowerSource *BusA;
	PowerSource *BusB;
};

class ThreeWayPowerMerge : public PowerSource {
public:
	ThreeWayPowerMerge() { Phase1 = 0; Phase2 = 0; Phase3 = 0; };
	double Voltage();
	void DrawPower(double watts);
	void WireToBuses(PowerSource *a, PowerSource *b, PowerSource *c) { Phase1 = a; Phase2 = b; Phase3 = c; };
	double Current();

protected:
	PowerSource *Phase1;
	PowerSource *Phase2;
	PowerSource *Phase3;
};

class PowerBreaker : public PowerSource {

public:
	PowerBreaker() { breaker_open = false; };
	double Voltage();
	bool IsOpen() { return breaker_open; };
	virtual void SetOpen(bool state) { breaker_open = state; };

protected:
	bool breaker_open;
};

class PowerSDKObject : public PowerSource {

public:
	PowerSDKObject() { SDKObj = 0; };

	double Voltage();
	double Current();
	double PowerLoad();

	void DrawPower(double watts);
	void WireToSDK(e_object *s) { SDKObj = s; };

protected:
	e_object *SDKObj;
};

#endif
