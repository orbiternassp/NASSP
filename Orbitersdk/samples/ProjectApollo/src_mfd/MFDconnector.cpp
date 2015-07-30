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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.3  2009/12/17 17:47:18  tschachim
  *	New default checklist for ChecklistMFD together with a lot of related bugfixes and small enhancements.
  *	
  *	Revision 1.2  2009/09/17 17:48:41  tschachim
  *	DSKY support and enhancements of ChecklistMFD / ChecklistController
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.13  2008/05/24 17:30:40  tschachim
  *	Bugfixes, new flash toggle.
  *	
  *	Revision 1.12  2008/04/11 11:49:14  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.11  2008/01/25 20:06:05  lassombra
  *	Implemented delayable switch functions.
  *	
  *	Now, all register functions on all toggle switches should take, at the end, a boolean
  *	 for whether it is delayable, and an int for how many seconds to delay.
  *	
  *	Actual delay can be anywhere between the int and the int + 1.
  *	
  *	Function is implemented as a timestepped switch which is called intelligently from
  *	 the panel, which now gets a timestep call.
  *	
  *	Revision 1.10  2008/01/23 01:40:07  lassombra
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
  *	Revision 1.9  2008/01/16 19:03:02  lassombra
  *	All but time-step, automation, and complete save/load is now implemented on the Checklist Controller (and the files that depend on it).
  *	
  *	All bugs in current code should hopefully be gone, but not necessarily so.
  *	
  *	Revision 1.8  2008/01/15 17:43:59  lassombra
  *	Allows multiple senders to attach to a single receiver.
  *	
  *	Revision 1.7  2008/01/14 15:52:33  lassombra
  *	*Final* version of the interface for the checklist controller.  May need some more
  *	 data, but should be accessible at this point.  For some reason getting heap errors
  *	 in deconstructors, doing research into it.
  *	
  *	Revision 1.6  2008/01/09 09:39:06  lassombra
  *	Completed MFD<->ChecklistController interface.  Coding can now take place on two separate code paths.
  *	
  *	Anyone who wants to work on the MFD can at this point do so using the existing connector code.
  *	
  *	None of the functions will exactly DO anything at the moment, but that is being worked on.
  *	
  *	Revision 1.5  2008/01/09 01:46:45  movieman523
  *	Added initial support for talking to checklist controller from MFD.
  *	
  *	Revision 1.4  2007/12/21 18:10:27  movieman523
  *	Revised docking connector code; checking in a working version prior to a rewrite to automate the docking process.
  *	
  *	Revision 1.3  2007/12/21 02:47:08  movieman523
  *	Connector cleanup, and fix my build break!
  *	
  *	Revision 1.2  2007/12/21 02:31:17  movieman523
  *	Added SetState() call and some more documentation.
  *	
  *	Revision 1.1  2007/12/21 01:00:29  movieman523
  *	Really basic Checklist MFD based on Project Apollo MFD, along with the various support functions required to make it work.
  *	
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
