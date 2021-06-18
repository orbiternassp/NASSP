/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

LM connector classes

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
#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "iu.h"
#include "sivbsystems.h"
#include "sivb.h"
#include "lemconnector.h"

LEMConnector::LEMConnector(LEM *l)

{
	OurVessel = l;
}

LEMConnector::~LEMConnector()

{
}

LEMECSConnector::LEMECSConnector(LEM *l) : LEMConnector(l)
{
	type = NO_CONNECTION;
	connectedTo = 0;
}

bool LEMECSConnector::ReceiveMessage(Connector *from, ConnectorMessage &m) {

	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	if (m.messageType == 0)
	{
		m.val1.pValue = OurVessel->GetLMTunnelPipe();

		return true;
	}

	else if (m.messageType == 1)
	{
		OurVessel->ConnectTunnelToCabinVent();

		return true;
	}

	else if (m.messageType == 2)
	{
		m.val1.pValue = OurVessel->GetCSMO2HoseOutlet();

		return true;
	}

	return false;
}

// UMBILICAL
LEMPowerConnector::LEMPowerConnector(LEM *l) : LEMConnector(l)
{
	type = NO_CONNECTION;
	connectedTo = 0;
}

bool LEMPowerConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)
{
	return false;
}

bool LEMPowerConnector::GetBatteriesLVOn()
{
	ConnectorMessage cm;

	cm.destination = LEM_CSM_POWER;
	cm.messageType = 10;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool LEMPowerConnector::GetBatteriesLVHVOffA()
{
	ConnectorMessage cm;

	cm.destination = LEM_CSM_POWER;
	cm.messageType = 11;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool LEMPowerConnector::GetBatteriesLVHVOffB()
{
	ConnectorMessage cm;

	cm.destination = LEM_CSM_POWER;
	cm.messageType = 12;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

LMToSIVBConnector::LMToSIVBConnector(LEM *l) : LEMConnector(l)
{
}

LMToSIVBConnector::~LMToSIVBConnector()
{
}

void LMToSIVBConnector::StartSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_START_SEPARATION;

	SendMessage(cm);
}

void LMToSIVBConnector::StopSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_STOP_SEPARATION;

	SendMessage(cm);
}

LEMCommandConnector::LEMCommandConnector(LEM *l) : LEMConnector(l)
{
	type = CSM_PAYLOAD_COMMAND;
}

LEMCommandConnector::~LEMCommandConnector()
{

}

bool LEMCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	PayloadSIVBMessageType messageType;

	messageType = (PayloadSIVBMessageType)m.messageType;

	switch (messageType)
	{
	case SLA_START_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StartSeparationPyros();
			return true;
		}
		break;

	case SLA_STOP_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StopSeparationPyros();
			return true;
		}
		break;

	}

	return false;
}

LM_RRtoCSM_RRT_Connector::LM_RRtoCSM_RRT_Connector(LEM *l, LEM_RR *lm_rr) : LEMConnector(l)
{
	type = RADAR_RF_SIGNAL;
	lemrr = lm_rr;
}

LM_RRtoCSM_RRT_Connector::~LM_RRtoCSM_RRT_Connector()
{

}

void LM_RRtoCSM_RRT_Connector::SendRF(double freq, double XMITpow, double XMITgain, double Phase)
{
	ConnectorMessage cm;

	cm.destination = RADAR_RF_SIGNAL;
	cm.messageType = CW_RADAR_SIGNAL;

	cm.val1.dValue = freq;
	cm.val2.dValue = XMITpow;
	cm.val3.dValue = XMITgain;
	cm.val4.dValue = Phase;

	SendMessage(cm);

	//sprintf(oapiDebugString(), "Hey this Function got called at %lf", oapiGetSimTime());

}

bool LM_RRtoCSM_RRT_Connector::ReceiveMessage(Connector * from, ConnectorMessage & m)
{
	//sprintf(oapiDebugString(), "Hey this Function got called at %lf", oapiGetSimTime()); //debugging
	if (m.destination != type)
	{
		return false;
	}

	if (!lemrr)
	{
		return false;
	}

	RFconnectorMessageType messageType;
	messageType = (RFconnectorMessageType)m.messageType;

	switch (messageType)
	{
		case RR_XPDR_SIGNAL:
		{
			//sprintf(oapiDebugString(),"Frequency Received: %lf MHz", m.val1.dValue);
			lemrr->SetRCVDrfProp(m.val1.dValue, m.val2.dValue, m.val3.dValue, m.val4.dValue);

			return true;
		}
		case CW_RADAR_SIGNAL:
		{
			return false;
		}
	}

	return false;
}

//LEM VHF Connectors

LM_VHFtoCSM_VHF_Connector::LM_VHFtoCSM_VHF_Connector(LEM * l, LM_VHF * VHFsys): LEMConnector(l)
{
	type = VHF_RNG;
	pLM_VHFs = VHFsys;
}

LM_VHFtoCSM_VHF_Connector::~LM_VHFtoCSM_VHF_Connector()
{

}

void LM_VHFtoCSM_VHF_Connector::SendRF(double freq, double XMITpow, double XMITgain, double XMITphase, bool RangeTone)
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

bool LM_VHFtoCSM_VHF_Connector::ReceiveMessage(Connector * from, ConnectorMessage & m)
{
	if (!pLM_VHFs) //No more segfaults
	{
		return false;
	}

	//this checks that the incoming frequencies from the csm connector are within 1% of the tuned frequencies of the receivers
	//in actuality it should be something more like a resonance responce centered around the tuned receiver frequency, but this waaay more simple
	//and easy to compute every timestep

	if (m.val1.dValue > pLM_VHFs->freqXCVR_A*0.99f && m.val1.dValue < pLM_VHFs->freqXCVR_A*1.01f)
	{
		//sprintf(oapiDebugString(), "A");
		pLM_VHFs->RCVDfreqRCVR_A = m.val1.dValue;
		pLM_VHFs->RCVDpowRCVR_A = m.val2.dValue;
		pLM_VHFs->RCVDgainRCVR_A = m.val3.dValue;
		pLM_VHFs->RCVDPhaseRCVR_A = m.val4.dValue;
		return true;
	}
	else if (m.val1.dValue > pLM_VHFs->freqXCVR_B*0.99f && m.val1.dValue < pLM_VHFs->freqXCVR_B*1.01f)
	{
		//sprintf(oapiDebugString(), "B");
		pLM_VHFs->RCVDfreqRCVR_B = m.val1.dValue;
		pLM_VHFs->RCVDpowRCVR_B = m.val2.dValue;
		pLM_VHFs->RCVDgainRCVR_B = m.val3.dValue;
		pLM_VHFs->RCVDPhaseRCVR_B = m.val4.dValue;
		pLM_VHFs->RCVDRangeTone = m.val1.bValue;
		return true;
	}
	else
	{
		return false;
	}
}