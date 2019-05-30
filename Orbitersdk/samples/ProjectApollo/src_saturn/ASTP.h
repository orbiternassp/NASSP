/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra

  ORBITER vessel module: ASTP

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

#pragma once

#include "connector.h"
#include "payload.h"

class ASTP;

const VECTOR3 DM_CSM_DOCKING_PORT_DIR = _V(0, 0, 1);
const VECTOR3 DM_SLA_DOCKING_PORT_DIR = _V(0, 0, -1);

  ///
  /// \ingroup Connectors
  /// \brief DM to S-IVB connector type.
  ///
class DMToSIVBConnector : public Connector
{
public:
	DMToSIVBConnector();
	~DMToSIVBConnector();

	void StartSeparationPyros();
	void StopSeparationPyros();
};

class DMConnector : public Connector
{
public:
	DMConnector(ASTP *a);
	~DMConnector();

	void SetDM(ASTP *dm) { OurVessel = dm; };

protected:
	ASTP *OurVessel;
};

class DMCommandConnector : public DMConnector
{
public:
	DMCommandConnector(ASTP *a);
	~DMCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

class ASTP: public Payload {

public:

	ASTP (OBJHANDLE hObj, int fmodel);
	virtual ~ASTP();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkDockEvent(int dock, OBJHANDLE connected);

	void init();
	void Setup();

	void StartSeparationPyros();
	void StopSeparationPyros();

protected:

	//
	// No variables needed for now.
	//

	ATTACHMENTHANDLE hattDROGUE;

	MultiConnector DMToCSMConnector;
	DMCommandConnector CSMToDMCommandConnector;
	DMToSIVBConnector SLAConnector;

};
