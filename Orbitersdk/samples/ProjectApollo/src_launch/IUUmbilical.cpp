/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Umbilical

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
#include "iu.h"
#include "IUUmbilicalInterface.h"
#include "IUUmbilical.h"
#include "IU_ESE.h"

IUESEToIUCommandConnector::IUESEToIUCommandConnector()
{
	type = IUESE_IU_COMMAND;
	ourIU_ESE = NULL;
}

IUESEToIUCommandConnector::~IUESEToIUCommandConnector()
{

}

bool IUESEToIUCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)
{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IUESEMessageType messageType;

	messageType = (IUESEMessageType)m.messageType;

	switch (messageType)
	{
	case IU_IUESE_GET_COMMAND_VEHICLE_LIFTOFF_INDICATION_INHIBIT:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetCommandVehicleLiftoffIndicationInhibit();
			return true;
		}
		break;
	case IU_IUESE_GET_EXCESSIVE_ROLL_RATE_AUTO_ABORT_INHIBIT:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetExcessiveRollRateAutoAbortInhibit(m.val1.iValue);
			return true;
		}
		break;
	case IU_IUESE_GET_EXCESSIVE_PITCH_YAW_RATE_AUTO_ABORT_INHIBIT:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetExcessivePitchYawRateAutoAbortInhibit(m.val1.iValue);
			return true;
		}
		break;
	case IU_IUESE_GET_TWO_ENGINE_OUT_AUTO_ABORT_INHIBIT:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetTwoEngineOutAutoAbortInhibit(m.val1.iValue);
			return true;
		}
		break;
	case IU_IUESE_GET_GSE_OVERRATE_SIMULATE:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetOverrateSimulate(m.val1.iValue);
			return true;
		}
		break;
	case IU_IUESE_GET_EDS_POWER_INHIBIT:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetEDSPowerInhibit();
			return true;
		}
		break;
	case IU_IUESE_PAD_ABORT_REQUEST:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetEDSPadAbortRequest();
			return true;
		}
		break;
	case IU_IUESE_GET_ENGINE_THRUST_INDICATION_ENABLE_INHIBIT_A:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetThrustOKIndicateEnableInhibitA();
			return true;
		}
		break;
	case IU_IUESE_GET_ENGINE_THRUST_INDICATION_ENABLE_INHIBIT_B:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetThrustOKIndicateEnableInhibitB();
			return true;
		}
		break;
	case IU_IUESE_GET_EDS_LIFTOFF_INHIBIT_A:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetEDSLiftoffInhibitA();
			return true;
		}
		break;
	case IU_IUESE_GET_EDS_LIFTOFF_INHIBIT_B:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetEDSLiftoffInhibitB();
			return true;
		}
		break;
	case IU_IUESE_GET_SI_BURN_MODE_SUBSTITUTE:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetSIBurnModeSubstitute();
			return true;
		}
		break;
	case IU_IUESE_GET_GUIDANCE_REFERENCE_RELEASE:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetGuidanceReferenceRelease();
			return true;
		}
		break;
	case IU_IUESE_GET_Q_BALL_SIMULATE_CMD:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetQBallSimulateCmd();
			return true;
		}
		break;
	case IU_IUESE_GET_EDS_AUTO_ABORT_SIMULATE:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetEDSAutoAbortSimulate(m.val1.iValue);
			return true;
		}
		break;
	case IU_IUESE_GET_EDS_LV_CUTOFF_SIMULATE:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetEDSLVCutoffSimulate(m.val1.iValue);
			return true;
		}
		break;
	case IU_IUESE_GET_SIC_OUTBOARD_ENGINES_CANT_INHIBIT:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetSICOutboardEnginesCantInhibit();
			return true;
		}
		break;
	case IU_IUESE_GET_SIC_OUTBOARD_ENGINES_CANT_SIMULATE:
		if (ourIU_ESE)
		{
			m.val1.bValue = ourIU_ESE->GetSICOutboardEnginesCantSimulate();
			return true;
		}
		break;
	}
	return false;
}

void IUESEToIUCommandConnector::SetEDSLiftoffEnableA()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SET_EDS_LIFTOFF_ENABLE_A;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::SetEDSLiftoffEnableB()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SET_EDS_LIFTOFF_ENABLE_B;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::EDSLiftoffEnableReset()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_LIFTOFF_ENABLE_RESET;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::SwitchFCCPowerOn()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SET_FCC_POWER;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::SwitchFCCPowerOff()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SET_FCC_POWER;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::SwitchQBallPowerOn()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SET_Q_BALL_POWER;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::SwitchQBallPowerOff()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SET_Q_BALL_POWER;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::SetControlSignalProcessorPower(bool set)
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SET_CONTROL_SIGNAL_PROCESSOR_POWER;
	cm.val1.bValue = set;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::EDSGroupNo1Reset()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_GROUP_NO_1_RESET;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::EDSGroupNo2Reset()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_GROUP_NO_2_RESET;

	SendMessage(cm);
}

bool IUESEToIUCommandConnector::AllSIEnginesRunning()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_ALL_SI_ENGINES_RUNNING;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::IsEDSUnsafeA()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_IS_EDS_UNSAFE_A;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::IsEDSUnsafeB()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_IS_EDS_UNSAFE_B;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetEDSSCCutoff1()
{
	return GetEDSSCCutoff(1);
}

bool IUESEToIUCommandConnector::GetEDSSCCutoff2()
{
	return GetEDSSCCutoff(2);
}

bool IUESEToIUCommandConnector::GetEDSSCCutoff3()
{
	return GetEDSSCCutoff(3);
}

bool IUESEToIUCommandConnector::GetEDSSCCutoff(int n)
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_GET_EDS_SC_CUTOFF;
	cm.val1.iValue = n;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetEDSAutoAbortBus()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_GET_EDS_AUTO_ABORT_BUS;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetEDSExcessiveRollRateIndication()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_GET_EDS_EXCESSIVE_ROLL_RATE_INDICATION;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetEDSExcessivePitchYawRateIndication()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_GET_EDS_EXCESSIVE_PITCH_YAW_RATE_INDICATION;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetLVDCOutputRegisterDiscrete(int bit)
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_GET_LVDC_OUTPUT_REGISTER_DISCRETE;
	cm.val1.iValue = bit;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::FCCPowerIsOn()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_FCC_POWER_IS_ON;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

void IUESEToIUCommandConnector::SwitchSelector(int stage, int channel)
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_SWITCH_SELECTOR;
	cm.val1.iValue = stage;
	cm.val2.iValue = channel;

	SendMessage(cm);
}

void IUESEToIUCommandConnector::LVDCPrepareToLaunch()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_LVDC_PREPARE_TO_LAUNCH;

	SendMessage(cm);
}

bool IUESEToIUCommandConnector::GetSCCutoffEnabledA()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_SC_CUTOFF_ENABLE_A;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetSCCutoffEnabledB()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_SC_CUTOFF_ENABLE_B;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetLiftoffEnableA()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_GET_LIFTOFF_ENABLE_A;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

bool IUESEToIUCommandConnector::GetLiftoffEnableB()
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_GET_LIFTOFF_ENABLE_B;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}
	return false;
}

void IUESEToIUCommandConnector::GetEDSAbortCommandToSC(bool *abort)
{
	ConnectorMessage cm;

	cm.destination = IUESE_IU_COMMAND;
	cm.messageType = IUESE_IU_EDS_GET_ABORT_TO_SC;
	cm.val1.pValue = abort;

	if (SendMessage(cm))
	{
		return;
	}
	
	for (int i = 0; i < 6; i++)
	{
		abort[i] = false;
	}
}