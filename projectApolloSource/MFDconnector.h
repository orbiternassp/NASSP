/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: MFD Connector header file

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

#if !defined(_PA_MFDCONNECTOR_H)
#define _PA_MFDCONNECTOR_H

///
/// \ingroup Connectors
/// \brief Connector type.
///

class MFDConnector : public Connector
{
public:
	MFDConnector();
	~MFDConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
	bool ConnectToVessel(VESSEL *v);
	void DisconnectVessel();
	bool SetFlashing(char *n, bool flash);
	int GetValue(char *n);

protected:
	Connector *vessel;
};

#endif // _PA_MFDCONNECTOR_H
