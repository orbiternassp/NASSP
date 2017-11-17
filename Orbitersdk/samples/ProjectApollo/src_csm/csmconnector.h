/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: CSM connector classes

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

#if !defined(_PA_CSMCONNECTOR_H)
#define _PA_CSMCONNECTOR_H

class Saturn;
class CSMcomputer;

///
/// \ingroup Connectors
/// \brief Saturn-class connector base class.
///
class SaturnConnector : public Connector
{
public:
	SaturnConnector(Saturn *s);
	~SaturnConnector();

	void SetSaturn(Saturn *sat) { OurVessel = sat; };

protected:
	Saturn *OurVessel;
};

///
/// \ingroup Connectors
/// \brief CSM to IU connector type.
///
class CSMToIUConnector : public SaturnConnector
{
public:
	CSMToIUConnector(CSMcomputer &c, Saturn *s);
	~CSMToIUConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	bool IsTLICapable();
	void GetVesselStats(double &isp, double &thrust);
	void ChannelOutput(int channel, int value);

	double GetMass();
	double GetFuelMass();

	bool GetLiftOffCircuit(bool sysA);
	bool GetEDSAbort(int n);

protected:
	CSMcomputer &agc;
};

class DockingProbe;

///
/// \ingroup Connectors
/// \brief CSM to SIVb connector type.
///
class CSMToSIVBControlConnector : public SaturnConnector
{
public:
	CSMToSIVBControlConnector(CSMcomputer &c, DockingProbe &probe, Saturn *s);
	~CSMToSIVBControlConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	bool IsVentable();

	double GetFuelMass();
	void GetMainBatteryPower(double &capacity, double &drain);
	void GetMainBatteryElectrics(double &volts, double &current);

	void StartSeparationPyros();
	void StopSeparationPyros();

protected:
	CSMcomputer &agc;
	DockingProbe &dockingprobe;
};

///
/// \ingroup Connectors
/// \brief Saturn to IU command connector type.
///
class SaturnToIUCommandConnector : public SaturnConnector
{
public:
	SaturnToIUCommandConnector(Saturn *s);
	~SaturnToIUCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

#endif // _PA_CSMCONNECTOR_H
