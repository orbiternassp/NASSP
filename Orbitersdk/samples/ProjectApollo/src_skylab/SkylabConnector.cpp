/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Skylab Connector Classes

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
#include "Skylab.h"
#include "SkylabConnector.h"


Skylab_VHFtoCSM_VHF_Connector::Skylab_VHFtoCSM_VHF_Connector()
{
	type = VHF_RNG;
}

Skylab_VHFtoCSM_VHF_Connector::~Skylab_VHFtoCSM_VHF_Connector()
{

}

void Skylab_VHFtoCSM_VHF_Connector::SendRF(double freq, double XMITpow, double XMITgain, double XMITphase, bool RangeTone)
{
	ConnectorMessage cm;

	cm.destination = VHF_RNG;
	cm.messageType = VHF_RNG_SIGNAL_LM;

	cm.val1.dValue = freq; //MHz
	cm.val2.dValue = XMITpow; //W
	cm.val3.dValue = XMITgain; //dBi
	cm.val4.dValue = XMITphase;
	cm.val1.bValue = RangeTone;

	SendMessage(cm);
}

bool Skylab_VHFtoCSM_VHF_Connector::ReceiveMessage(Connector* from, ConnectorMessage& m)
{
	return true;
}