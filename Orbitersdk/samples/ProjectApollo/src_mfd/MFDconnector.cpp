/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER module: MFD Connector class

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
#include "orbiterSDK.h"

#include "nasspdefs.h"
#include "checklistController.h"

#include "connector.h"
#include "MFDconnector.h"

#include <stdio.h>
#include <string.h>

//
// MFD to Panel interface connector.
//

MFDConnector::MFDConnector()

{
	SetType(MFD_PANEL_INTERFACE);
}

MFDConnector::~MFDConnector()

{
}
bool MFDConnector::ConnectTo(Connector *other)
{
	if (other)
	{
		if (other->GetType() != type)
		{
			return false;
		}
		other->connectedTo = this;
	}
	connectedTo = other;
	return true;
}
bool MFDConnector::ConnectToVessel(VESSEL *v)

{
	//
	// Disconnect in case we're already connected.
	//
	Disconnect();

	//
	// See if we can find the appropriate connector on the vessel.
	//
	Connector *vessel = GetVesselConnector(v, VIRTUAL_CONNECTOR_PORT, type);

	//
	// Try to connect if we did.
	//
	if (vessel)
	{
		return ConnectTo(vessel);
	}

	return false;
}

void MFDConnector::DisconnectVessel()

{
	Disconnect();
}

bool MFDConnector::SetFlashing(char *n, bool flash)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_FLASH_ITEM;
	cm.val1.pValue = n;
	cm.val2.bValue = flash;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool MFDConnector::GetFlashing(char *n)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_GET_ITEM_FLASHING;
	cm.val1.pValue = n;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

int MFDConnector::GetState(char *n)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_GET_ITEM_STATE;
	cm.val1.pValue = n;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return (-1);
}

bool MFDConnector::SetState(char *n, int value, bool guard, bool hold)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_SET_ITEM_STATE;
	cm.val1.pValue = n;
	cm.val2.iValue = value;
	cm.val3.bValue = guard;
	cm.val4.bValue = hold;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool MFDConnector::GetFailed(char *n)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_GET_FAILED_STATE;
	cm.val1.pValue = n;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool MFDConnector::ChecklistAutocomplete(bool yesno)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_CHECKLIST_AUTOCOMPLETE;
	cm.val1.bValue = yesno;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}
bool MFDConnector::ChecklistAutocomplete()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_CHECKLIST_AUTOCOMPLETE_QUERY;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

void MFDConnector::SetChecklistFlashing(bool f)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_CHECKLIST_FLASHING;
	cm.val1.bValue = f;

	SendMessage(cm);
}

bool MFDConnector::GetChecklistFlashing()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_CHECKLIST_FLASHING_QUERY;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

ChecklistItem *MFDConnector::GetChecklistItem(int group, int index)
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_GET_CHECKLIST_ITEM;
	cm.val1.iValue = group;
	cm.val2.iValue = index;

	if (SendMessage(cm))
	{
		return (ChecklistItem *)cm.val1.pValue;
	}

	return NULL;
}

vector<ChecklistGroup> *MFDConnector::GetChecklistList()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_GET_CHECKLIST_LIST;

	if (SendMessage(cm))
	{
		return (vector<ChecklistGroup> *)cm.val1.pValue;
	}
	return NULL;
}
bool MFDConnector::failChecklistItem(ChecklistItem* in)
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_FAIL_ITEM;
	cm.val1.pValue = in;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}
bool MFDConnector::completeChecklistItem(ChecklistItem* in)
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_COMPLETE_ITEM;
	cm.val1.pValue = in;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}
char *MFDConnector::checklistName()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_CHECKLIST_NAME;
	
	if (SendMessage(cm))
		return static_cast<char *>(cm.val1.pValue);

	return 0;
}
bool MFDConnector::RetrieveChecklist(ChecklistContainer *input)
{
	ConnectorMessage cm;
	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_RETRIEVE_CHECKLIST;

	cm.val1.pValue = input;
	if (SendMessage(cm))
		return cm.val2.bValue;
	return false;
}
