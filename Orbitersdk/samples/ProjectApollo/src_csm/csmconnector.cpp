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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.3  2010/02/22 14:23:30  tschachim
  *	Apollo 7 S-IVB on orbit attitude control, venting and Saturn takeover mode for the VAGC.
  *	
  *	Revision 1.2  2009/07/15 22:51:45  bluedragon8144
  *	added a few more default orbiter functions for connector usage.
  *	
  *	Revision 1.1  2009/02/18 23:20:56  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.11  2008/04/11 11:49:33  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.10  2008/01/23 01:40:08  lassombra
  *	Implemented timestep functions and event management
  *	
  *	Events for Saturns are now fully implemented
  *	
  *	Removed all hardcoded checklists from Saturns.
  *	
  *	Automatic Checklists are coded into an excel file.
  *	
  *	Added function to get the name of the active checklist.
  *	
  *	ChecklistController is now 100% ready for Saturn.
  *	
  *	Revision 1.9  2008/01/14 01:17:04  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.8  2007/12/04 20:26:31  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.7  2007/06/06 15:02:11  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.6  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.5  2006/07/27 21:30:47  movieman523
  *	Added display of SIVb battery voltage and current.
  *	
  *	Revision 1.4  2006/07/21 23:04:34  movieman523
  *	Added Saturn 1b engine lights on panel and beginnings of electrical connector work (couldn't disentangle the changes). Be sure to get the config file for the SIVb as well.
  *	
  *	Revision 1.3  2006/07/09 16:09:38  movieman523
  *	Added Prog 59 for SIVb venting.
  *	
  *	Revision 1.2  2006/07/09 00:07:07  movieman523
  *	Initial tidy-up of connector code.
  *	
  *	Revision 1.1  2006/07/07 19:35:04  movieman523
  *	First version.
  *	
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
#include "dsky.h"

#include "connector.h"

#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"

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

	case IULV_GET_PROPELLANT_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSIVbPropellantMass();
			return true;
		}
		break;

	case IULV_GET_STATUS:
		if (OurVessel)
		{
			VESSELSTATUS *status = static_cast<VESSELSTATUS *> (m.val1.pValue);
			VESSELSTATUS stat;

			OurVessel->GetStatus(stat);

			*status = stat;
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

	case IULV_GET_AP_DIST:
		if (OurVessel)
		{
			OurVessel->GetApDist(m.val1.dValue);
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

	case IULV_GET_ELEMENTS:
		if (OurVessel)
		{
			ELEMENTS el;
			ELEMENTS *e = static_cast<ELEMENTS *> (m.val1.pValue);

			m.val3.hValue = OurVessel->GetElements(el, m.val2.dValue);

			*e = el;

			return true;
		}
		break;

	case IULV_GET_PMI:
		if (OurVessel)
		{
			OurVessel->GetPMI(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_SIZE:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetSize();
			return true;
		}
		break;

	case IULV_GET_MAXTHRUST:
		if (OurVessel)
		{
			m.val2.dValue = OurVessel->GetSaturnMaxThrust((ENGINETYPE) m.val1.iValue);
			return true;
		}
		break;

	case IULV_LOCAL2GLOBAL:
		if (OurVessel)
		{
			OurVessel->Local2Global(*(VECTOR3 *) m.val1.pValue, *(VECTOR3 *) m.val2.pValue);
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

	case IULV_GET_FORCEVECTOR:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->GetForceVector(*(VECTOR3 *) m.val1.pValue);
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

	case IULV_GET_PITCH:
		if (OurVessel)
		{	
			m.val1.dValue = OurVessel->GetPitch();
			return true;
		}
		break;
			
	case IULV_GET_BANK:
		if (OurVessel)
		{	
			m.val1.dValue = OurVessel->GetBank();
			return true;
		}
		break;
			
	case IULV_GET_SLIP_ANGLE:
		if (OurVessel)
		{	
			m.val1.dValue = OurVessel->GetSlipAngle();
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

	case IULV_J2_DONE:
		if (OurVessel) 
		{
			OurVessel->SetVentingJ2Thruster();
			return true;
		}
		break;

	case IULV_SET_J2_THRUST_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetJ2ThrustLevel(m.val1.dValue);
			return true;
		}
		break;

	case IULV_SET_APS_THRUST_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetAPSThrustLevel(m.val1.dValue);
			return true;
		}
		break;

	case IULV_SET_ATTITUDE_LIN_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetAttitudeLinLevel(m.val1.iValue, m.val2.iValue);
			return true;
		}
		break;

	case IULV_SET_ATTITUDE_ROT_LEVEL:
		if (OurVessel) 
		{
			OurVessel->SetSaturnAttitudeRotLevel(m.val1.vValue);
			return true;
		}
		break;

	case IULV_ACTIVATE_S4RCS:
		if (OurVessel)
		{
			OurVessel->ActivateS4RCS();
			return true;
		}
		break;

	case IULV_DEACTIVATE_S4RCS:
		if (OurVessel)
		{
			OurVessel->DeactivateS4RCS();
			return true;
		}
		break;

	case IULV_ENABLE_J2:
		if (OurVessel)
		{
			OurVessel->EnableDisableJ2(m.val1.bValue);
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
	case IUCSM_IS_VIRTUAL_AGC:
		m.val1.bValue = agc.IsVirtualAGC();
		return true;

	case IUCSM_SET_OUTPUT_CHANNEL:
		agc.SetOutputChannel(m.val1.iValue, m.val2.iValue);
		return true;

	case IUCSM_GET_SIISIVBSEP_SWITCH_STATE:
		if (OurVessel)
		{
			m.val1.iValue = OurVessel->GetSIISIVbSepSwitchState();
			return true;
		}
		break;

	case IUCSM_GET_TLI_ENABLE_SWITCH_STATE:
		if (OurVessel)
		{
			m.val1.iValue = OurVessel->GetTLIEnableSwitchState();
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

	case IUCSM_SLOW_IF_DESIRED:
		if (OurVessel)
		{
			OurVessel->SlowIfDesired();
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
	}

	return false;
}

bool CSMToIUConnector::IsTLICapable()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_IS_TLI_CAPABLE;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

void CSMToIUConnector::GetVesselStats(double &isp, double &thrust)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_GET_VESSEL_STATS;

	if (SendMessage(cm))
	{
		isp = cm.val1.dValue;
		thrust = cm.val2.dValue;
	}
}

double CSMToIUConnector::GetMass()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_GET_VESSEL_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 1.0;
}

double CSMToIUConnector::GetFuelMass()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_GET_VESSEL_FUEL;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 1.0;
}

void CSMToIUConnector::ChannelOutput(int channel, int value)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = CSMIU_CHANNEL_OUTPUT;
	cm.val1.iValue = channel;
	cm.val2.iValue = value;

	SendMessage(cm);
}

CSMToSIVBControlConnector::CSMToSIVBControlConnector(CSMcomputer &c,  DockingProbe &probe, Saturn *s) : agc(c), dockingprobe(probe), SaturnConnector(s)

{
	type = CSM_SIVB_COMMAND;
}

CSMToSIVBControlConnector::~CSMToSIVBControlConnector()

{
}

//
// For now we have to process the ignore docking event message from the SIVB.
//
bool CSMToSIVBControlConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	CSMSIVBMessageType messageType;

	messageType = static_cast<CSMSIVBMessageType> (m.messageType);

	switch (messageType)
	{
	case SIVBCSM_IGNORE_DOCK_EVENT:
		dockingprobe.SetIgnoreNextDockEvent();
		return true;

	case SIVBCSM_IGNORE_DOCK_EVENTS:
		dockingprobe.SetIgnoreNextDockEvents(m.val1.iValue);
		return true;

	case SIVBCSM_GET_PAYLOAD_SETTINGS:
		{
			PayloadSettings *p = static_cast<PayloadSettings *> (m.val1.pValue);
			OurVessel->GetPayloadSettings(*p);
			m.val1.bValue = true;
		}
		return true;

	default:
		return false;
	}
}

bool CSMToSIVBControlConnector::IsVentable()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_IS_VENTABLE;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool CSMToSIVBControlConnector::IsVenting()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_IS_VENTING;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

double CSMToSIVBControlConnector::GetFuelMass()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_GET_VESSEL_FUEL;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	//
	// Non-zero return just in case the calling code tries to divide
	// by it.
	//
	return 0.01;
}

void CSMToSIVBControlConnector::GetMainBatteryPower(double &capacity, double &drain)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_GET_MAIN_BATTERY_POWER;

	if (SendMessage(cm))
	{
		capacity = cm.val1.dValue;
		drain = cm.val2.dValue;
		return;
	}

	capacity = drain = 0.0;
}

void CSMToSIVBControlConnector::GetMainBatteryElectrics(double &volts, double &current)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_GET_MAIN_BATTERY_ELECTRICS;

	if (SendMessage(cm))
	{
		volts = cm.val1.dValue;
		current = cm.val2.dValue;
		return;
	}

	volts = current = 0.0;
}

void CSMToSIVBControlConnector::StartSeparationPyros()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_START_SEPARATION;

	SendMessage(cm);
}

void CSMToSIVBControlConnector::StopSeparationPyros()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_STOP_SEPARATION;

	SendMessage(cm);
}

void CSMToSIVBControlConnector::StartVenting()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_START_VENTING;

	SendMessage(cm);
}

void CSMToSIVBControlConnector::StopVenting()

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = CSMSIVB_STOP_VENTING;

	SendMessage(cm);
}
