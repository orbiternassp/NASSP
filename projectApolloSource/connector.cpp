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

  **************************** Revision History ****************************
  *	$Log$
  **************************************************************************/

#include "orbiterSDK.h"

#include "nasspdefs.h"
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

