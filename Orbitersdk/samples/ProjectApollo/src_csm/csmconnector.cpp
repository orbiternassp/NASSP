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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"

#include "ioChannels.h"
#include "toggleswitch.h"
#include "apolloguidance.h"

#include "connector.h"

#include "csmcomputer.h"

#include "iu.h"

#include "saturn.h"
#include "sivb.h"

#include <stdio.h>
#include <string.h>

SaturnConnector::SaturnConnector(Saturn *s)

{
	OurVessel = s;
}

SaturnConnector::~SaturnConnector()

{
}

SaturnToIUCommandConnector::SaturnToIUCommandConnector(Saturn *s) : SaturnConnector(s)

{
	type = LV_IU_COMMAND;
}

SaturnToIUCommandConnector::~SaturnToIUCommandConnector()

{
}

bool SaturnToIUCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IULVMessageType messageType;

	messageType = (IULVMessageType) m.messageType;

	switch (messageType)
	{
	case IULV_GET_J2_THRUST_LEVEL:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetJ2ThrustLevel();
			return true;
		}
		break;

	case IULV_GET_STAGE:
		if (OurVessel)
		{
			m.val1.iValue = OurVessel->GetStage();
			return true;
		}
		break;

	case IULV_GET_ALTITUDE:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetAltitude();
			return true;
		}
		break;

	case IULV_GET_GLOBAL_ORIENTATION:
		if (OurVessel)
		{
			VECTOR3 *arot = static_cast<VECTOR3 *> (m.val1.pValue);
			VECTOR3 ar;

			OurVessel->GetGlobalOrientation(ar);

			*arot = ar;
			return true;
		}
		break;

	case IULV_GET_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetMass();
			return true;
		}
		break;

	case IULV_GET_GRAVITY_REF:
		if (OurVessel)
		{
			m.val1.hValue = OurVessel->GetGravityRef();
			return true;
		}
		break;

	case IULV_GET_FUEL_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetFuelMass();
			return true;
		}
		break;

	case IULV_GET_MAX_FUEL_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetMaxFuelMass();
			return true;
		}
		break;

	case IULV_GET_RELATIVE_POS:
		if (OurVessel)
		{
			VECTOR3 pos;
			VECTOR3 *v = static_cast<VECTOR3 *> (m.val2.pValue);

			OurVessel->GetRelativePos(m.val1.hValue, pos);

			v->data[0] = pos.data[0];
			v->data[1] = pos.data[1];
			v->data[2] = pos.data[2];

			return true;
		}
		break;

	case IULV_GET_RELATIVE_VEL:
		if (OurVessel)
		{
			VECTOR3 vel;
			VECTOR3 *v = static_cast<VECTOR3 *> (m.val2.pValue);

			OurVessel->GetRelativeVel(m.val1.hValue, vel);

			v->data[0] = vel.data[0];
			v->data[1] = vel.data[1];
			v->data[2] = vel.data[2];

			return true;
		}
		break;

	case IULV_GET_GLOBAL_VEL:
		if (OurVessel)
		{
			OurVessel->GetGlobalVel(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_WEIGHTVECTOR:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->GetWeightVector(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_ROTATIONMATRIX:
		if (OurVessel)
		{
			OurVessel->GetRotationMatrix(*(MATRIX3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_ANGULARVEL:
		if (OurVessel)
		{
			OurVessel->GetAngularVel(*(VECTOR3 *)m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_MISSIONTIME:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetMissionTime();
			return true;
		}
		break;

	case IULV_GET_APOLLONO:
		if (OurVessel)
		{
			m.val1.iValue = OurVessel->GetApolloNo();
			return true;
		}
		break;

	case IULV_GET_SI_THRUST_OK:
		if (OurVessel)
		{
			OurVessel->GetSIThrustOK((bool *)m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_SII_THRUST_OK:
		if (OurVessel)
		{
			OurVessel->GetSIIThrustOK((bool *)m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_SIVB_THRUST_OK:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIVBThrustOK();
			return true;
		}
		break;

	case IULV_GET_SI_PROPELLANT_DEPLETION_ENGINE_CUTOFF:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIPropellantDepletionEngineCutoff();
			return true;
		}
		break;

	case IULV_GET_SI_INBOARD_ENGINE_OUT:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIInboardEngineOut();
			return true;
		}
		break;

	case IULV_GET_SI_OUTBOARD_ENGINE_OUT:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIOutboardEngineOut();
			return true;
		}
		break;

	case IULV_GET_SIB_LOW_LEVEL_SENSORS_DRY:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIBLowLevelSensorsDry();
			return true;
		}
		break;

	case IULV_GET_SII_ENGINE_OUT:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIIEngineOut();
			return true;
		}
		break;

	case IULV_GET_SII_PROPELLANT_DEPLETION_ENGINE_CUTOFF:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIIPropellantDepletionEngineCutoff();
			return true;
		}
		break;

	case IULV_GET_FIRST_STAGE_THRUST:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetFirstStageThrust();
			return true;
		}
		break;

	case IULV_GET_SII_FUEL_TANK_PRESSURE:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSIIFuelTankPressurePSI();
			return true;
		}
		break;

	case IULV_GET_SIVB_FUEL_TANK_PRESSURE:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSIVBFuelTankPressurePSI();
			return true;
		}
		break;

	case IULV_GET_SIVB_LOX_TANK_PRESSURE:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSIVBLOXTankPressurePSI();
			return true;
		}
		break;

	case IULV_ACTIVATE_NAVMODE:
		if (OurVessel)
		{
			OurVessel->ActivateNavmode(m.val1.iValue);
			return true;
		}
		break;

	case IULV_DEACTIVATE_NAVMODE:
		if (OurVessel)
		{
			OurVessel->DeactivateNavmode(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SWITCH_SELECTOR:
		if (OurVessel)
		{
			OurVessel->SwitchSelector(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SI_SWITCH_SELECTOR:
		if (OurVessel)
		{
			OurVessel->SISwitchSelector(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SII_SWITCH_SELECTOR:
		if (OurVessel)
		{
			OurVessel->SIISwitchSelector(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SIVB_SWITCH_SELECTOR:
		if (OurVessel)
		{
			OurVessel->SIVBSwitchSelector(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SEPARATE_STAGE:
		if (OurVessel)
		{
			OurVessel->SeparateStage(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SET_STAGE:
		if (OurVessel)
		{
			OurVessel->SetStage(m.val1.iValue);
			return true;
		}
		break;

	case IULV_SET_APS_ATTITUDE_ENGINE:
		if (OurVessel)
		{
			OurVessel->SetAPSAttitudeEngine(m.val1.iValue, m.val2.bValue);
			return true;
		}
		break;

	case IULV_SI_EDS_CUTOFF:
		if (OurVessel)
		{
			OurVessel->SIEDSCutoff(m.val1.bValue);
			return true;
		}
		break;

	case IULV_SII_EDS_CUTOFF:
		if (OurVessel)
		{
			OurVessel->SIIEDSCutoff(m.val1.bValue);
			return true;
		}
		break;

	case IULV_SIVB_EDS_CUTOFF:
		if (OurVessel)
		{
			OurVessel->SIVBEDSCutoff(m.val1.bValue);
			return true;
		}
		break;

	case IULV_SET_SI_THRUSTER_DIR:
		if (OurVessel)
		{
			OurVessel->SetSIThrusterDir(m.val1.iValue, m.val2.dValue, m.val3.dValue);
			return true;
		}
		break;

	case IULV_SET_SII_THRUSTER_DIR:
		if (OurVessel)
		{
			OurVessel->SetSIIThrusterDir(m.val1.iValue, m.val2.dValue, m.val3.dValue);
			return true;
		}
		break;

	case IULV_SET_SIVB_THRUSTER_DIR:
		if (OurVessel)
		{
			OurVessel->SetSIVBThrusterDir(m.val1.dValue, m.val2.dValue);
			return true;
		}
		break;

	case IULV_SET_QBALL_POWER_OFF:
		if (OurVessel)
		{
			OurVessel->SetQBallPowerOff();
			return true;
		}
		break;

	case IULV_ADD_FORCE:
		if (OurVessel)
		{
			OurVessel->AddForce(m.val1.vValue, m.val2.vValue);
			return true;
		}
		break;

	case IULV_ADD_S4RCS:
		if (OurVessel)
		{
			OurVessel->AddRCS_S4B();
			return true;
		}
		break;

	case IULV_ACTIVATE_PRELAUNCH_VENTING:
		if (OurVessel)
		{
			OurVessel->ActivatePrelaunchVenting();
			return true;
		}
		break;

	case IULV_DEACTIVATE_PRELAUNCH_VENTING:
		if (OurVessel)
		{
			OurVessel->DeactivatePrelaunchVenting();
			return true;
		}
		break;

	case IULV_CSM_SEPARATION_SENSED:
		if (OurVessel)
		{
			m.val1.bValue = false;
			return true;
		}
		break;
	}

	return false;
}

CSMToIUConnector::CSMToIUConnector(CSMcomputer &c, Saturn *s) : agc(c), SaturnConnector(s)

{
	type = CSM_IU_COMMAND;
}

CSMToIUConnector::~CSMToIUConnector()

{
}

bool CSMToIUConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IUCSMMessageType messageType;

	messageType = (IUCSMMessageType) m.messageType;

	switch (messageType)
	{
	case IUCSM_SET_INPUT_CHANNEL_BIT:
		if (OurVessel)
		{
			agc.SetInputChannelBit(m.val1.iValue, m.val2.iValue, m.val3.bValue);
			return true;
		}
		break;

	case IUCSM_GET_CMC_SIVB_TAKEOVER:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetCMCSIVBTakeover();
			return true;
		}
		break;

	case IUCSM_GET_CMC_SIVB_IGNITION:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetCMCSIVBIgnitionSequenceStart();
			return true;
		}
		break;

	case IUCSM_GET_CMC_SIVB_CUTOFF:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetCMCSIVBCutoff();
			return true;
		}
		break;

	case IUCSM_GET_SIISIVB_DIRECT_STAGING:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIISIVbDirectStagingSignal();
			return true;
		}
		break;

	case IUCSM_GET_LV_RATE_AUTO_SWITCH_STATE:
		if (OurVessel)
		{
			m.val1.iValue = OurVessel->GetLVRateAutoSwitchState();
			return true;
		}
		break;

	case IUCSM_GET_TWO_ENGINE_OUT_AUTO_SWITCH_STATE:
		if (OurVessel)
		{
			m.val1.iValue = OurVessel->GetTwoEngineOutAutoSwitchState();
			return true;
		}
		break;

	case IUCSM_GET_BECO_COMMAND:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->GetBECOSignal(m.val1.bValue);
			return true;
		}
		break;

	case IUCSM_IS_EDS_BUS_POWERED:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->IsEDSBusPowered(m.val1.iValue);
			return true;
		}
		break;

	case IUCSM_GET_AGC_ATTITUDE_ERROR:
		if (OurVessel)
		{
			m.val2.iValue = OurVessel->GetAGCAttitudeError(m.val1.iValue);
			return true;
		}
		break;

	case IUCSM_GET_ENGINE_INDICATOR:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->GetEngineIndicator(m.val1.iValue);
			return true;
		}
		break;

	case IUCSM_GET_TLI_INHIBIT:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetTLIInhibitSignal();
			return true;
		}
		break;

	case IUCSM_GET_IU_UPTLM_ACCEPT:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetIUUPTLMAccept();
			return true;
		}
		break;

	case IUCSM_SET_SII_SEP_LIGHT:
		if (OurVessel)
		{
			if (m.val1.bValue)
			{
				OurVessel->SetSIISep();
			}
			else
			{
				OurVessel->ClearSIISep();
			}
			return true;
		}
		break;

	case IUCSM_SET_LV_RATE_LIGHT:
		if (OurVessel)
		{
			if (m.val1.bValue)
			{
				OurVessel->SetLVRateLight();
			}
			else
			{
				OurVessel->ClearLVRateLight();
			}
			return true;
		}
		break;

	case IUCSM_SET_LV_GUID_LIGHT:
		if (OurVessel)
		{
			if (m.val1.bValue)
			{
				OurVessel->SetLVGuidLight();
			}
			else
			{
				OurVessel->ClearLVGuidLight();
			}
			return true;
		}
		break;

	case IUCSM_SET_ENGINE_INDICATORS:
		if (OurVessel)
		{
			if (m.val1.bValue)
			{
				OurVessel->SetEngineIndicators();
			}
			else
			{
				OurVessel->ClearEngineIndicators();
			}
			return true;
		}
		break;

	case IUCSM_SET_ENGINE_INDICATOR:
		if (OurVessel)
		{
			if (m.val2.bValue)
			{
				OurVessel->SetEngineIndicator(m.val1.iValue);
			}
			else
			{
				OurVessel->ClearEngineIndicator(m.val1.iValue);
			}
			return true;
		}
		break;

	case IUCSM_LOAD_TLI_SOUNDS:
		if (OurVessel)
		{
			OurVessel->LoadTLISounds();
			return true;
		}
		break;

	case IUCSM_PLAY_COUNT_SOUND:
		if (OurVessel)
		{
			OurVessel->PlayCountSound(m.val1.bValue);
			return true;
		}
		break;

	case IUCSM_PLAY_SECO_SOUND:
		if (OurVessel)
		{
			OurVessel->PlaySecoSound(m.val1.bValue);
			return true;
		}
		break;

	case IUCSM_PLAY_SEPS_SOUND:
		if (OurVessel)
		{
			OurVessel->PlaySepsSound(m.val1.bValue);
			return true;
		}
		break;

	case IUCSM_PLAY_TLI_SOUND:
		if (OurVessel)
		{
			OurVessel->PlayTLISound(m.val1.bValue);
			return true;
		}
		break;

	case IUCSM_PLAY_TLISTART_SOUND:
		if (OurVessel)
		{
			OurVessel->PlayTLIStartSound(m.val1.bValue);
			return true;
		}
		break;

	case IUCSM_CLEAR_TLI_SOUNDS:
		if (OurVessel)
		{
			OurVessel->ClearTLISounds();
			return true;
		}
		break;
	case IUCSM_TLI_BEGUN:
		if (OurVessel)
		{
			OurVessel->TLI_Begun();
			return true;
		}
		break;
	case IUCSM_TLI_ENDED:
		if (OurVessel)
		{
			OurVessel->TLI_Ended();
			return true;
		}
		break;
	}

	return false;
}

bool CSMToIUConnector::GetLiftOffCircuit(bool sysA)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_GET_LIFTOFF_CIRCUIT;
	cm.val1.bValue = sysA;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}

bool CSMToIUConnector::GetEDSAbort(int n)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_GET_EDS_ABORT;
	cm.val1.iValue = n;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}

double CSMToIUConnector::GetLVTankPressure(int n)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_GET_LV_TANK_PRESSURE;
	cm.val1.iValue = n;

	if (SendMessage(cm))
	{
		return cm.val2.dValue;
	}

	return 0.0;
}

CSMToLEMECSConnector::CSMToLEMECSConnector(Saturn *s) : SaturnConnector(s)
{

}

CSMToLEMECSConnector::~CSMToLEMECSConnector()
{

}

bool CSMToLEMECSConnector::ConnectTo(Connector *other)
{
	// Disconnect in case we're already connected.
	Disconnect();

	if (SaturnConnector::ConnectTo(other))
	{
		h_Pipe *cmpipe = OurVessel->GetCMTunnelPipe();
		h_Pipe *lmpipe = GetDockingTunnelPipe();

		cmpipe->out = lmpipe->in;
		lmpipe->in = NULL;

		return true;
	}

	return false;
}

void CSMToLEMECSConnector::Disconnect()
{
	OurVessel->ConnectTunnelToCabinVent();
	ConnectLMTunnelToCabinVent();

	SaturnConnector::Disconnect();
}

h_Pipe* CSMToLEMECSConnector::GetDockingTunnelPipe()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = 0;

	if (SendMessage(cm))
	{
		return static_cast<h_Pipe *> (cm.val1.pValue);
	}

	return NULL;
}

void CSMToLEMECSConnector::ConnectLMTunnelToCabinVent()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = 1;

	SendMessage(cm);
}

CSMToPayloadConnector::CSMToPayloadConnector(Saturn *s) : SaturnConnector(s)
{
	type = CSM_PAYLOAD_COMMAND;
}

CSMToPayloadConnector::~CSMToPayloadConnector()
{

}

void CSMToPayloadConnector::StartSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_START_SEPARATION;

	SendMessage(cm);
}

void CSMToPayloadConnector::StopSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_STOP_SEPARATION;

	SendMessage(cm);
}