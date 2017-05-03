/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Connector class

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

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "powersource.h"
#include "connector.h"

#include <stdio.h>
#include <string.h>

Connector::Connector()

{
	type = NO_CONNECTION;
	connectedTo = 0;
}

Connector::~Connector()

{
	//
	// We have to be sure to disconnect before deletion so the other end doesn't try to send
	// us any more data.
	//
	Disconnect();
}

void Connector::Disconnect()

{
	if (connectedTo)
	{
		connectedTo->connectedTo = 0;
		connectedTo = 0;
	}
}

void Connector::Disconnected()

{
	//
	// Do nothing by default.
	//
}

bool Connector::ConnectTo(Connector *other)

{
	//
	// Other may be NULL. If so just clear this connection, otherwise
	// check the other end is the correct type and disconnected.
	//
	if (other)
	{
		if (other->GetType() != type || other->connectedTo)
		{
			return false;
		}

		other->connectedTo = this;
	}

	connectedTo = other;

	return true;
}

bool Connector::SendMessage(ConnectorMessage &m)

{
	if (connectedTo)
	{
		return connectedTo->ReceiveMessage(this, m);
	}

	return false;
}

bool Connector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// The default connector is one-way. Return an error if anything tries to send data to us.
	//
	return false;
}

ConnectorType Connector::GetType()

{
	return type;
}

MultiConnector::MultiConnector()

{
	int i;

	for (i = 0; i < N_MULTICONNECT_INPUTS; i++)
	{
		Inputs[i] = 0;
	}
}

MultiConnector::~MultiConnector()

{
	int i;

	for (i = 0; i < N_MULTICONNECT_INPUTS; i++)
	{
		if (Inputs[i])
		{
			Inputs[i]->connectedTo = 0;
			Inputs[i] = 0;
		}
	}
}

bool MultiConnector::AddTo(Connector *other)

{
	int i;

	if (!other)
		return false;

	//
	// First check this isn't already connected.
	//
	for (i = 0; i < N_MULTICONNECT_INPUTS; i++)
	{
		if (Inputs[i] == other)
		{
			other->connectedTo = this;
			return true;
		}
	}

	//
	// Only one connector of each type.
	//
	ConnectorType otherType = other->GetType();

	for (i = 0; i < N_MULTICONNECT_INPUTS; i++)
	{
		if (Inputs[i] && Inputs[i]->GetType() == otherType)
		{
			return false;
		}
	}

	//
	// Then add it if there's a free slot.
	//
	for (i = 0; i < N_MULTICONNECT_INPUTS; i++)
	{
		if (!Inputs[i])
		{
			Inputs[i] = other;
			other->connectedTo = this;
			return true;
		}
	}

	return false;
}

bool MultiConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	if (from == connectedTo)
	{
		int i;

		//
		// The message came from the other end of the connection, so forward to the
		// appropriate connection on this side.
		//
		for (i = 0; i < N_MULTICONNECT_INPUTS; i++)
		{
			if (Inputs[i] && Inputs[i]->GetType() == m.destination) 
			{
				return Inputs[i]->ReceiveMessage(from, m);
			}
		}
	}
	else if (connectedTo)
	{
		//
		// The message came from an input, so send to the other end.
		//
		return connectedTo->ReceiveMessage(this, m);
	}

	//
	// Should never happen unless we're disconnected from the other end.
	//
	return false;
}

void MultiConnector::Disconnect()

{
	int i;

	//
	// Tell all our connectors that we've disconnected.
	//
	for (i = 0; i < N_MULTICONNECT_INPUTS; i++)
	{
		if (Inputs[i])
		{
			Inputs[i]->Disconnected();
		}
	}

	Connector::Disconnect();
}

ProjectApolloConnectorVessel::ProjectApolloConnectorVessel(OBJHANDLE hObj, int fmodel) : VESSEL3(hObj, fmodel)

{
	int i;
	for (i = 0; i < PACV_N_CONNECTORS; i++)
	{
		ConnectorList[i].port = 0;
		ConnectorList[i].c = NULL;
	}

	ValidationValue = PACV_N_VALIDATION;
}

bool ProjectApolloConnectorVessel::ValidateVessel()

{
	return (ValidationValue == PACV_N_VALIDATION);
}

Connector *ProjectApolloConnectorVessel::GetConnector(int port, ConnectorType t)

{
	int i;
	for (i = 0; i < PACV_N_CONNECTORS; i++)
	{
		if (ConnectorList[i].c && (ConnectorList[i].port == port) && (ConnectorList[i].c->GetType() == t))
			return ConnectorList[i].c;
	}

	return NULL;
}

bool ProjectApolloConnectorVessel::RegisterConnector(int port, Connector *c)

{
	int i;
	for (i = 0; i < PACV_N_CONNECTORS; i++)
	{
		if (!ConnectorList[i].c)
		{
			ConnectorList[i].c = c;
			ConnectorList[i].port = port;
			return true;
		}
	}

	return false;
}

void ProjectApolloConnectorVessel::UndockConnectors(int port)

{
	int i;
	for (i = 0; i < PACV_N_CONNECTORS; i++)
	{
		if (ConnectorList[i].c && (ConnectorList[i].port == port))
		{
			ConnectorList[i].c->Disconnect();
		}
	}
}

void ProjectApolloConnectorVessel::DockConnectors(int port)

{
	DOCKHANDLE d = GetDockHandle(port);

	if (!d)
		return;

	OBJHANDLE connected = GetDockStatus(d);

	if (!connected)
		return;

	VESSEL *dockedWith = oapiGetVesselInterface(connected);

	if (!dockedWith)
		return;

	int i;
	for (i = 0; i < PACV_N_CONNECTORS; i++)
	{
		if (ConnectorList[i].c && (ConnectorList[i].port == port))
		{
			///
			/// \todo Find correct docking port on other vessel, don't assume we're
			/// always docked to port zero.
			///
			Connector *ours = ConnectorList[i].c;
			Connector *theirs = GetVesselConnector(dockedWith, 0, ours->GetType());

			if (theirs)
				ours->ConnectTo(theirs);
		}
	}
}


Connector *GetVesselConnector(VESSEL *v, int port, ConnectorType t)

{
	//
	// Check for null pointer, just in case!
	//
	if (!v)
		return NULL;

	char *classname = v->GetClassName();

	//
	// If this isn't a project Apollo vessel, assume it's not the
	// correct type.
	//
	if (strnicmp(classname, "ProjectApollo", 13))
		return NULL;

	//
	// Cast it to our vessel on the assumption that it is.
	//
	ProjectApolloConnectorVessel *pacv = static_cast<ProjectApolloConnectorVessel *> (v);

	//
	// Validate it to check that this is probably the right kind of vessel.
	//
	if (!pacv->ValidateVessel())
		return NULL;

	//
	// Finally, try to get the connector.
	//
	return pacv->GetConnector(port, t);
}
