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
  *	Revision 1.2  2009/12/17 17:47:18  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.13  2008/04/11 11:49:53  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.12  2006/07/27 20:40:06  movieman523
  *	We can now draw power from the SIVb in the Apollo to Venus scenario.
  *	
  *	Revision 1.11  2006/07/21 23:04:35  movieman523
  *	Added Saturn 1b engine lights on panel and beginnings of electrical connector work (couldn't disentangle the changes). Be sure to get the config file for the SIVb as well.
  *	
  *	Revision 1.10  2006/05/30 14:40:21  tschachim
  *	Fixed fuel cell - dc bus connectivity, added battery charger
  *	
  *	Revision 1.9  2006/01/14 20:58:16  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.8  2005/12/02 19:47:19  movieman523
  *	Replaced most PowerSource code with e_object.
  *	
  *	Revision 1.7  2005/12/02 19:29:24  movieman523
  *	Started integrating PowerSource code into PanelSDK.
  *	
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

	double Voltage();
	double Current();

protected:
};

class PowerMerge : public PowerSource {
public:
	PowerMerge(char *i_name, PanelSDK &p);
	double Voltage();
	void DrawPower(double watts);
	void WireToBuses(e_object *a, e_object *b) { BusA = a; BusB = b; };
	double Current();

protected:
	PanelSDK &sdk;

	e_object *BusA;
	e_object *BusB;
};

class ThreeWayPowerMerge : public PowerSource {
public:
	ThreeWayPowerMerge(char *i_name, PanelSDK &p);
	double Voltage();
	void DrawPower(double watts);
	void WireToBuses(e_object *a, e_object *b, e_object *c) { Phase1 = a; Phase2 = b; Phase3 = c; };
	void WireToBus(int bus, e_object* e);
	bool IsBusConnected(int bus);
	double Current();

protected:
	PanelSDK &sdk;

	e_object *Phase1;
	e_object *Phase2;
	e_object *Phase3;
};

class NWayPowerMerge : public PowerSource {
public:
	NWayPowerMerge(char *i_name, PanelSDK &p, int nSources);
	~NWayPowerMerge();

	double Voltage();
	void DrawPower(double watts);
	void WireToBus(int bus, e_object* e);
	bool IsBusConnected(int bus);
	double Current();

protected:
	PanelSDK &sdk;

	int nSources;
	e_object **sources;
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


class DCBusController : public e_object {

public:
	DCBusController(char *i_name, PanelSDK &p);
	void Init(e_object *fc1, e_object *fc2, e_object *fc3, e_object *bat1, e_object *bat2, e_object *gse, e_object *vp, e_object *bc1, e_object *bc2, e_object *bc3);
	void refresh(double dt);
	void ConnectFuelCell(int fc, bool connect);
	bool IsFuelCellConnected(int fc);
	bool IsBusContPowered(int fc);
	bool IsFuelCellDisconnectAlarm();
	e_object *GetBusSource() { return &busPower; };
	void SetTieState(int s) { tieState = s; };
	void SetTieAuto(bool s) { tieAuto = s; };
	void SetGSEState(int s);
	void Load(char *line);
	void Save(FILEHANDLE scn);

protected:
	PanelSDK &sdk;

	e_object *fuelcell1, *fuelcell2, *fuelcell3;
	e_object *battery1, *battery2;
	e_object *gseBattery;
	e_object *busCont1, *busCont2, *busCont3;

	NWayPowerMerge busPower;
	ThreeWayPowerMerge fcPower;
	PowerMerge batPower;
	bool fcDisconnectAlarm[4];
	int tieState;
	bool tieAuto;
	int gseState;
};


class BatteryCharger : public e_object {

public:
	BatteryCharger(char *i_name, PanelSDK &p);
	void Init(e_object *bat1, e_object *bat2, e_object *bat3, 
		      e_object *batSup1, e_object *batSup2, e_object *batSup3,
			  e_object *dc1, e_object *dc2, e_object* ac);
	void UpdateFlow(double dt);
	void DrawPower(double watts);
	void Charge(int bat);
	void Load(char *line);
	void Save(FILEHANDLE scn);

protected:
	PanelSDK &sdk;

	e_object *battery1, *battery2, *battery3;
	e_object *batSupply1, *batSupply2, *batSupply3;

	PowerMerge dcPower;
	e_object *acPower;

	e_object *currentBattery;
};

class Connector;

///
/// \brief Connector object: sends messages to a connector, rather than to the next e_object
/// in the chain. Mostly used for connecting to another vessel.
///
class PowerSourceConnectorObject : public e_object
{
public:
	PowerSourceConnectorObject(char *i_name, PanelSDK &p);
	void SetConnector(Connector *c) { connect = c; };

	double Voltage();
	double Current();
	void UpdateFlow(double dt);
	void DrawPower(double watts);

protected:
	Connector *connect;
};

///
/// \brief Connector object: receives messages from a connector, rather than the next e_object
/// in the chain. Mostly used for connecting to another vessel.
///
class PowerDrainConnectorObject : public e_object
{
public:
	PowerDrainConnectorObject(char *i_name, PanelSDK &sdk);

	void SetConnector(Connector *c) { connect = c; };

	void ProcessUpdateFlow(double dt);
	void ProcessDrawPower(double watts);
	void refresh(double dt);
	void Disconnected();

protected:
	Connector *connect;

	double PowerDraw;
	double PowerDrawn;
};

///
/// \ingroup Connectors
/// \brief Message type to process electric power through a connector.
///
enum PowerSourceMessageType
{
	POWERCON_GET_VOLTAGE,					///< Get voltage.
	POWERCON_GET_CURRENT,					///< Get current.
	POWERCON_DRAW_POWER,					///< Draw power from connector.
	POWERCON_UPDATE_FLOW,					///< Update power flow.
};

#endif
