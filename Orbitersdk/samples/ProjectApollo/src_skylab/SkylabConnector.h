/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

LM connector classes (Header)

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

#if !defined(_SKYLAB_CONNECTOR_H)
#define _SKYLAB_CONNECTOR_H

#include "Orbitersdk.h"
#include "connector.h"


class Skylab;

class SkylabConnector : public Connector
{
public:
	SkylabConnector(Skylab* s);
	~SkylabConnector();

	void SetSkylab(Skylab* skylab) { OurVessel = skylab; };

protected:
	Skylab* OurVessel;
};

class Skylab_VHFtoCSM_VHF_Connector : public SkylabConnector
{
public:
	Skylab_VHFtoCSM_VHF_Connector(Skylab* s);
	~Skylab_VHFtoCSM_VHF_Connector();

	void SendRF(double freq, double XMITpow, double XMITgain, double XMITphase, bool RangeTone, VECTOR3 Position);
	bool ReceiveMessage(Connector* from, ConnectorMessage& m);
protected:
	Skylab* pSkylab_VHFs;
};

#endif