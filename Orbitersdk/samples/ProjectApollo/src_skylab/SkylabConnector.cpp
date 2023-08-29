/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

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

SkylabConnector::SkylabConnector(Skylab* s)

{
	OurVessel = s;
}

SkylabConnector::~SkylabConnector()

{
}


Skylab_VHFtoCSM_VHF_Connector::Skylab_VHFtoCSM_VHF_Connector(Skylab* s) : SkylabConnector(s)
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
	if (!pSkylab_VHFs) //No more segfaults
	{
		return false;
	}

	//this checks that the incoming frequencies from the csm connector are within 1% of the tuned frequencies of the receivers
	//in actuality it should be something more like a resonance responce centered around the tuned receiver frequency, but this waaay more simple
	//and easy to compute every timestep

	//if (m.val1.dValue > pSkylab_VHFs->freqXCVR_A * 0.99f && m.val1.dValue < pSkylab_VHFs->freqXCVR_A * 1.01f)
	//{
	//	//sprintf(oapiDebugString(), "A");
	//	pLM_VHFs->RCVDfreqRCVR_A = m.val1.dValue;
	//	pLM_VHFs->RCVDpowRCVR_A = m.val2.dValue;
	//	pLM_VHFs->RCVDgainRCVR_A = m.val3.dValue;
	//	pLM_VHFs->RCVDPhaseRCVR_A = m.val4.dValue;
	//	return true;
	//}
	//else if (m.val1.dValue > pLM_VHFs->freqXCVR_B * 0.99f && m.val1.dValue < pLM_VHFs->freqXCVR_B * 1.01f)
	//{
	//	//sprintf(oapiDebugString(), "B");
	//	pLM_VHFs->RCVDfreqRCVR_B = m.val1.dValue;
	//	pLM_VHFs->RCVDpowRCVR_B = m.val2.dValue;
	//	pLM_VHFs->RCVDgainRCVR_B = m.val3.dValue;
	//	pLM_VHFs->RCVDPhaseRCVR_B = m.val4.dValue;
	//	pLM_VHFs->RCVDRangeTone = m.val1.bValue;
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
}