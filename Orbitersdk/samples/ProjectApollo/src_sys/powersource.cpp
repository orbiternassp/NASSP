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

  **************************************************************************/


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include <stdio.h>

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "powersource.h"
#include "connector.h"

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
// Tie power together from two sources. 
//

PowerMerge::PowerMerge(char *i_name, PanelSDK &p) : sdk(p)

{
	if (i_name)
		strcpy(name, i_name);

	BusA = 0; 
	BusB = 0;

	sdk.AddElectrical(this, false);
}

double PowerMerge::Voltage()

{
	double VoltsA = 0;
	double VoltsB = 0;

	if (BusA) VoltsA = BusA->Voltage();
	if (BusB) VoltsB = BusB->Voltage();

	if (VoltsA != 0 && VoltsB != 0) return (VoltsA + VoltsB) / 2.0;
	if (VoltsA != 0) return VoltsA;
	if (VoltsB != 0) return VoltsB;

	return 0;
}

double PowerMerge::Current()

{
	double Volts = Voltage();

	if (Volts > 0.0) {
		return power_load / Volts;
	}
	return 0.0;
}

void PowerMerge::DrawPower(double watts)

{
	double Volts = 0.0;
	double VoltsA = 0.0;
	double VoltsB = 0.0;

	power_load += watts;

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
// Tie power together from three sources. 
//

double ThreeWayPowerMerge::Voltage()

{
	double Volts1 = 0;
	double Volts2 = 0;
	double Volts3 = 0;

	if (Phase1)	Volts1 = Phase1->Voltage();
	if (Phase2)	Volts2 = Phase2->Voltage();
	if (Phase3)	Volts3 = Phase3->Voltage();

	if (Volts1 != 0 && Volts2 != 0 && Volts3 != 0) return (Volts1 + Volts2 + Volts3) / 3.0;

	if (Volts1 != 0 && Volts2 != 0) return (Volts1 + Volts2) / 2.0;
	if (Volts1 != 0 && Volts3 != 0) return (Volts1 + Volts3) / 2.0;
	if (Volts2 != 0 && Volts3 != 0) return (Volts2 + Volts3) / 2.0;

	if (Volts1 != 0) return Volts1;
	if (Volts2 != 0) return Volts2;
	if (Volts3 != 0) return Volts3;

	return 0;
}

double ThreeWayPowerMerge::Current()

{
	double Volts = Voltage();

	if (Volts > 0.0) {
		return power_load / Volts;
	}
	return 0.0;
}

ThreeWayPowerMerge::ThreeWayPowerMerge(char *i_name, PanelSDK &p) : sdk(p)

{
	if (i_name)
		strcpy (name, i_name);

	Phase1 = 0;
	Phase2 = 0;
	Phase3 = 0;

	//
	// Register with the Panel SDK so it will call our update function.
	//

	sdk.AddElectrical(this, false);
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

void ThreeWayPowerMerge::WireToBus(int bus, e_object* e)

{
	if (bus == 1) Phase1 = e;
	if (bus == 2) Phase2 = e;
	if (bus == 3) Phase3 = e;
}

bool ThreeWayPowerMerge::IsBusConnected(int bus)

{
	if (bus == 1) return (Phase1 != NULL);
	if (bus == 2) return (Phase2 != NULL);
	if (bus == 3) return (Phase3 != NULL);
	return false;
}

NWayPowerMerge::NWayPowerMerge(char *i_name, PanelSDK &p, int n) : sdk(p)

{
	if (i_name)
		strcpy (name, i_name);

	nSources = n;
	sources = new e_object *[nSources];

	int i;

	for (i = 0; i < nSources; i++)
	{
		sources[i] = 0;
	}

	//
	// Register with the Panel SDK so it will call our update function.
	//

	sdk.AddElectrical(this, false);
}

NWayPowerMerge::~NWayPowerMerge()

{
	if (sources)
	{
		delete[] sources;
		sources = 0;
	}
}

double NWayPowerMerge::Voltage()

{
	double V = 0;

	int i;
	int activeSources = 0;

	for (i = 0; i < nSources; i++)
	{
		if (sources[i])
		{
			double VS = fabs(sources[i]->Voltage());
			if (VS != 0)
			{
				V += VS;
				activeSources++;
			}
		}
	}

	if (!activeSources)
		return 0.0;

	return V / (double) activeSources;
}

double NWayPowerMerge::Current()

{
	double Volts = Voltage();

	if (Volts > 0.0)
	{
		return power_load / Volts;
	}
	return 0.0;
}

void NWayPowerMerge::DrawPower(double watts)

{
	double Volts = 0.0;
	int i;

	//
	// Sum the voltage from all sources.
	//
	for (i = 0; i < nSources; i++)
	{
		if (sources[i])
		{
			double VS = fabs(sources[i]->Voltage());
			if (VS != 0)
			{
				Volts += VS;
			}
		}
	}

	power_load += watts;

	//
	// Divide the power drain up across the sources by voltage.
	//
	if (Volts > 0.0)
	{
		int i;

		for (i = 0; i < nSources; i++)
		{
			if (sources[i])
			{
				sources[i]->DrawPower(watts * fabs(sources[i]->Voltage()) / Volts);
			}
		}
	}
}

bool NWayPowerMerge::IsBusConnected(int bus)

{
	if (bus < 1 || bus > nSources)
		return false;

	return (sources[bus - 1] != 0);
}

void NWayPowerMerge::WireToBus(int bus, e_object* e)

{
	if (bus > 0 && bus <= nSources)
		sources[bus - 1] = e;
}

DCBusController::DCBusController(char *i_name, PanelSDK &p) : 
	sdk(p), fcPower(0, p), batPower(0, p), busPower(0, p, 3)

{
	if (i_name)
		strcpy(name, i_name);

	fuelcell1 = 0;
	fuelcell2 = 0;
	fuelcell3 = 0;
	battery1 = 0;
	battery2 = 0;
	gseBattery = 0;
	fcDisconnectAlarm[1] = false;
	fcDisconnectAlarm[2] = false;
	fcDisconnectAlarm[3] = false;
	tieState = 0;
	tieAuto = false;
	gseState = 0;

	sdk.AddElectrical(this, false);
}

void DCBusController::Init(e_object *fc1, e_object *fc2, e_object *fc3, e_object *bat1, e_object *bat2, e_object *gse, e_object *bc1, e_object *bc2, e_object *bc3)

{
	fuelcell1 = fc1;
	fuelcell2 = fc2;
	fuelcell3 = fc3;

	busCont1 = bc1;
	busCont2 = bc2;
	busCont3 = bc3;

	battery1 = bat1;
	battery2 = bat2;
	gseBattery = gse;

	busPower.WireToBus(1, &fcPower);
	busPower.WireToBus(2, &batPower);
	busPower.WireToBus(3, NULL);		// Source 3 is for ground power.
}

void DCBusController::ConnectFuelCell(int fc, bool connect)

{
	if (connect) {
		if (fc == 1) {
			fcPower.WireToBus(1, fuelcell1);
		} else if (fc == 2) {
			fcPower.WireToBus(2, fuelcell2);
		} else if (fc == 3) {
			fcPower.WireToBus(3, fuelcell3);
		}
	} else {
		if (fc == 1) {
			fcPower.WireToBus(1, NULL);
		} else if (fc == 2) {
			fcPower.WireToBus(2, NULL);
		} else if (fc == 3) {
			fcPower.WireToBus(3, NULL);
		}
	}
	fcDisconnectAlarm[fc] = false;
}

void DCBusController::refresh(double dt)

{
	// Disconnect because of to high current
	if (fuelcell1->Current() > 75.0 && fcPower.IsBusConnected(1) && busCont1->Voltage() > SP_MIN_DCVOLTAGE) {
		fcPower.WireToBus(1, NULL);
		fcDisconnectAlarm[1] = true;
	}
	if (fuelcell2->Current() > 75.0 && fcPower.IsBusConnected(2) && busCont2->Voltage() > SP_MIN_DCVOLTAGE) {
		fcPower.WireToBus(2, NULL);
		fcDisconnectAlarm[2] = true;
	}
	if (fuelcell3->Current() > 75.0 && fcPower.IsBusConnected(3) && busCont3->Voltage() > SP_MIN_DCVOLTAGE) {
		fcPower.WireToBus(3, NULL);
		fcDisconnectAlarm[3] = true;
	}

	// Disconnect because of reverse current
	if (busPower.Voltage() > 0) {
		if (fuelcell1->Voltage() <= 0 && fcPower.IsBusConnected(1) && busCont1->Voltage() > SP_MIN_DCVOLTAGE) {
			fcPower.WireToBus(1, NULL);
			fcDisconnectAlarm[1] = true;
		}
		if (fuelcell2->Voltage() <= 0 && fcPower.IsBusConnected(2) && busCont2->Voltage() > SP_MIN_DCVOLTAGE) {
			fcPower.WireToBus(2, NULL);
			fcDisconnectAlarm[2] = true;
		}
		if (fuelcell3->Voltage() <= 0 && fcPower.IsBusConnected(3) && busCont3->Voltage() > SP_MIN_DCVOLTAGE) {
			fcPower.WireToBus(3, NULL);
			fcDisconnectAlarm[3] = true;
		}
	}

	// Main bus tie
	if (tieState == 2) {
		batPower.WireToBuses(battery1, battery2);

	} else if (tieState == 1) {
		// Auto
		if (!tieAuto) {
			batPower.WireToBuses(NULL, NULL);
		} else {
			batPower.WireToBuses(battery1, battery2);
		}

	} else {
		batPower.WireToBuses(NULL, NULL);
	}

	// The batteries are connected via diodes, so the battery voltage needs to be higher than the 
	// fuel cell voltage to draw power
	if (batPower.Voltage() > fcPower.Voltage() + .7)	// diode bias
		busPower.WireToBus(2, &batPower);
	else if (batPower.Voltage() < fcPower.Voltage())
		busPower.WireToBus(2, NULL);

	//if (!strcmp(name, "MainBusAController"))
	//	sprintf(oapiDebugString(), "FC %.2f, BAT %.2f", fcPower.Voltage(), batPower.Voltage());
}

bool DCBusController::IsFuelCellConnected(int fc)

{
	return fcPower.IsBusConnected(fc);
}

bool DCBusController::IsBusContPowered(int fc)

{
	if (fc == 1)
		return (busCont1->Voltage() > SP_MIN_DCVOLTAGE);
	else if (fc == 2)
		return (busCont2->Voltage() > SP_MIN_DCVOLTAGE);
	else
		return (busCont3->Voltage() > SP_MIN_DCVOLTAGE);
}

//Temporary until the SM buses are properly simulated
bool DCBusController::IsSMBusPowered()
{
	return (fcPower.Voltage() > SP_MIN_DCVOLTAGE);
}

bool DCBusController::IsFuelCellDisconnectAlarm()

{
	return fcDisconnectAlarm[1] || fcDisconnectAlarm[2] || fcDisconnectAlarm[3];
}

void DCBusController::SetGSEState(int s) 

{ 
	gseState = s; 
	if (gseState)
		busPower.WireToBus(3, gseBattery);
	else
		busPower.WireToBus(3, NULL);
}

void DCBusController::Load(char *line)

{
	int fc1, fc2, fc3, gse, ta, fca1, fca2, fca3;
	
	sscanf (line,"    <DCBUSCONTROLLER> %s %i %i %i %i %i %i %i %i %i", name, &fc1, &fc2, &fc3, &tieState, &gse, &ta, &fca1, &fca2, &fca3);
	if (fc1) fcPower.WireToBus(1, fuelcell1);
	if (fc2) fcPower.WireToBus(2, fuelcell2);
	if (fc3) fcPower.WireToBus(3, fuelcell3);
	if (ta) tieAuto = true;
	// tieState is evaluated in refresh()	
	SetGSEState(gse);

	fcDisconnectAlarm[1] = (fca1 ? true : false);
	fcDisconnectAlarm[2] = (fca2 ? true : false);
	fcDisconnectAlarm[3] = (fca3 ? true : false);
}

void DCBusController::Save(FILEHANDLE scn)

{
	char cbuf[1000];
	sprintf (cbuf, "%s %i %i %i %i %i %i %i %i %i", name, IsFuelCellConnected(1) ? 1 : 0, 
								  		                  IsFuelCellConnected(2) ? 1 : 0, 
										                  IsFuelCellConnected(3) ? 1 : 0,												 
														  tieState, gseState,
												          tieAuto ? 1 : 0,
												          fcDisconnectAlarm[1] ? 1 : 0,
												          fcDisconnectAlarm[2] ? 1 : 0,
												          fcDisconnectAlarm[3] ? 1 : 0);

	oapiWriteScenario_string (scn, "    <DCBUSCONTROLLER> ", cbuf);
}


BatteryCharger::BatteryCharger(char *i_name, PanelSDK &p) : 
	sdk(p), dcPower(0, p)

{
	if (i_name)
		strcpy(name, i_name);

	SRC = NULL;

	battery1 = NULL;
	battery2 = NULL;
	battery3 = NULL;
	batSupply1 = NULL;
	batSupply2 = NULL;
	batSupply3 = NULL;
	currentBattery = NULL;
	acPower = NULL;
	bat3Power = NULL;

	sdk.AddElectrical(this, false);
}

void BatteryCharger::Init(e_object *bat1, e_object *bat2, e_object *bat3, 
					      e_object *batSup1, e_object *batSup2, e_object *batSup3,
			              e_object *dc1, e_object *dc2, e_object *ac,
						  e_object *bat3pwr)
{
	battery1 = bat1;
	battery2 = bat2;
	battery3 = bat3;

	batSupply1 = batSup1;
	batSupply2 = batSup2;
	batSupply3 = batSup3;

	bat3Power = bat3pwr;

	dcPower.WireToBuses(dc1, dc2);
	acPower = ac;
}

void BatteryCharger::Charge(int bat)
{
	if (bat == 0) {
		batSupply1->WireTo(NULL);
		batSupply2->WireTo(NULL);

		//Battery 3 online
		battery3->WireTo(NULL);
		bat3Power->WireTo(battery3);
		batSupply3->WireTo(bat3Power);

		currentBattery = NULL;

	} else if (bat == 1) {
		batSupply1->WireTo(this);
		batSupply2->WireTo(NULL);

		//Battery 3 online
		battery3->WireTo(NULL);
		bat3Power->WireTo(battery3);
		batSupply3->WireTo(bat3Power);

		currentBattery = battery1;

	} else if (bat == 2) {
		batSupply1->WireTo(NULL);
		batSupply2->WireTo(this);

		//Battery 3 online
		battery3->WireTo(NULL);
		bat3Power->WireTo(battery3);
		batSupply3->WireTo(bat3Power);

		currentBattery = battery2;

	} else if (bat == 3) {
		batSupply1->WireTo(NULL);
		batSupply2->WireTo(NULL);

		//Battery 3 offline
		batSupply3->WireTo(this);
		bat3Power->WireTo(batSupply3);
		battery3->WireTo(bat3Power);

		currentBattery = battery3;
	}
}

void BatteryCharger::UpdateFlow(double dt)

{
	e_object::UpdateFlow(dt);

	if (currentBattery && dcPower.Voltage() > SP_MIN_DCVOLTAGE && acPower->Voltage() > SP_MIN_ACVOLTAGE) {
		Volts = max(10.0, currentBattery->Voltage());
	} else {
		Volts = 0;
		Amperes = 0;
	}
}

void BatteryCharger::DrawPower(double watts)
{ 
	power_load += watts;
	dcPower.DrawPower(1.4 * watts);
	acPower->DrawPower(0.92 * watts);
}

void BatteryCharger::Load(char *line)

{
	int bat;
	
	sscanf (line,"    <BATTERYCHARGER> %s %i", name, &bat);
	Charge(bat);
}

void BatteryCharger::Save(FILEHANDLE scn)

{
	char cbuf[1000];

	int bat;

	bat = 0;
	if (currentBattery == battery1) bat = 1;
	if (currentBattery == battery2) bat = 2;
	if (currentBattery == battery3) bat = 3;

	sprintf (cbuf, "%s %i", name, bat);
	oapiWriteScenario_string (scn, "    <BATTERYCHARGER> ", cbuf);
}

PowerSourceConnectorObject::PowerSourceConnectorObject(char *i_name, PanelSDK &sdk)

{
	if (i_name)
		strcpy (name, i_name);

	connect = 0;
	sdk.AddElectrical(this, false);
}

double PowerSourceConnectorObject::Voltage()

{
	if (connect)
	{
		ConnectorMessage cm;

		cm.destination = connect->GetType();
		cm.messageType = POWERCON_GET_VOLTAGE;

		if (connect->SendMessage(cm))
		{
			return cm.val1.dValue;
		}
	}	

	return 0.0;
}

double PowerSourceConnectorObject::Current()

{
	if (connect)
	{
		ConnectorMessage cm;

		cm.destination = connect->GetType();
		cm.messageType = POWERCON_GET_CURRENT;

		if (connect->SendMessage(cm))
		{
			return cm.val1.dValue;
		}
	}	

	return 0.0;
}

void PowerSourceConnectorObject::DrawPower(double watts)

{
	if (connect)
	{
		ConnectorMessage cm;

		cm.destination = connect->GetType();
		cm.messageType = POWERCON_DRAW_POWER;

		cm.val1.dValue = watts;

		connect->SendMessage(cm);
	}
}

void PowerSourceConnectorObject::UpdateFlow(double dt)

{
	if (connect)
	{
		ConnectorMessage cm;

		cm.destination = connect->GetType();
		cm.messageType = POWERCON_UPDATE_FLOW;

		cm.val1.dValue = dt;

		connect->SendMessage(cm);
	}
}

PowerDrainConnectorObject::PowerDrainConnectorObject(char *i_name, PanelSDK &sdk)

{
	if (i_name)
		strcpy (name, i_name);

	PowerDraw = 0.0;
	PowerDrawn = 0.0;

	connect = 0;

	sdk.AddElectrical(this, false);
}

void PowerDrainConnectorObject::ProcessUpdateFlow(double dt)

{
	PowerDraw = PowerDrawn;
	PowerDrawn = 0.0;
}

void PowerDrainConnectorObject::ProcessDrawPower(double watts)

{
	PowerDrawn += watts;
}

void PowerDrainConnectorObject::refresh(double dt)

{
	DrawPower(PowerDraw);
}

void PowerDrainConnectorObject::Disconnected()

{
	PowerDrawn = PowerDraw = 0.0;
}

PowerDrainConnector::PowerDrainConnector()

{
	power_drain = 0;
}

PowerDrainConnector::~PowerDrainConnector()

{
}

void PowerDrainConnector::SetPowerDrain(PowerDrainConnectorObject *p)

{
	power_drain = p;
}

void PowerDrainConnector::Disconnected()

{
	//
	// If we've disconnected then stop drawing power.
	//
	if (power_drain)
	{
		power_drain->Disconnected();
	}
}

bool PowerDrainConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	PowerSourceMessageType messageType;

	messageType = (PowerSourceMessageType) m.messageType;

	switch (messageType)
	{
	case POWERCON_GET_VOLTAGE:
		if (power_drain)
		{
			m.val1.dValue = power_drain->Voltage();
			return true;
		}
		break;

	case POWERCON_GET_CURRENT:
		if (power_drain)
		{
			m.val1.dValue = power_drain->Current();
			return true;
		}
		break;

	case POWERCON_DRAW_POWER:
		if (power_drain)
		{
			power_drain->ProcessDrawPower(m.val1.dValue);
			return true;
		}
		break;

	case POWERCON_UPDATE_FLOW:
		if (power_drain)
		{
			power_drain->ProcessUpdateFlow(m.val1.dValue);
			return true;
		}
		break;
	}

	return false;
}
