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

#include "orbiterSDK.h"

#include "nasspdefs.h"

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

bool MFDConnector::SetState(char *n, int value)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_SET_ITEM_STATE;
	cm.val1.pValue = n;
	cm.val2.iValue = value;

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

bool MFDConnector::ChecklistInit(char *checkFile)

{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = PanelConnector::MFD_PANEL_INIT_CHECKLIST;
	cm.val1.pValue = checkFile;

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
