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
	type = MFD_PANEL_INTERFACE;
	vessel = 0;
}

MFDConnector::~MFDConnector()

{
}

bool MFDConnector::ConnectToVessel(VESSEL *v)

{
	if (vessel)
	{
		vessel->Disconnect();
		vessel = 0;
	}

	vessel = GetConnector(v, VIRTUAL_CONNECTOR_PORT, type);
	
	if (vessel)
	{
		return ConnectTo(vessel);
	}

	return false;
}

void MFDConnector::DisconnectVessel()

{
	if (vessel)
	{
		vessel->Disconnect();
		vessel = 0;
	}
}

//
// For now we don't process any messages from the panel.
//
bool MFDConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	return false;
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
