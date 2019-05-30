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

// To force orbitersdk.h to use <fstream> in any compiler version

#pragma include_alias( <fstream.h>, <fstream> )

#define ORBITER_MODULE

#include "orbitersdk.h"
#include "stdio.h"

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "connector.h"
#include "sivbsystems.h"
#include "sivb.h"
#include "astp.h"

HINSTANCE g_hDLL;

static int refcount = 0;
static MESHHANDLE hastp;

DLLCLBK void InitModule(HINSTANCE hModule)
{
	g_hDLL = hModule;

	hastp = oapiLoadMeshGlobal("ProjectApollo/nASTP2");
}

DLLCLBK void ExitModule(HINSTANCE hModule)
{

}

DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel)
{
	return new ASTP(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel)
{
	if (vessel) delete (ASTP*)vessel;
}

ASTP::ASTP (OBJHANDLE hObj, int fmodel)
: Payload (hObj, fmodel),
CSMToDMCommandConnector(this)

{
	init();
}

ASTP::~ASTP ()

{
	// Nothing for now.
}

void ASTP::clbkSetClassCaps(FILEHANDLE cfg)
{
	VESSEL2::clbkSetClassCaps(cfg);

	Setup();
}

void ASTP::init()
{
	DMToCSMConnector.SetType(CSM_LEM_DOCKING);
	SLAConnector.SetType(PAYLOAD_SLA_CONNECT);

	DMToCSMConnector.AddTo(&CSMToDMCommandConnector);

	RegisterConnector(0, &DMToCSMConnector);
	RegisterConnector(1, &SLAConnector);
}

void ASTP::Setup()

{
	//
	// These numbers are all wrong since they're for the SIVB + ASTP combo. We need
	// to find the correct numbers to use.
	//
	// Currently it also only has one docking port.
	//
	SetSize (1.6);
	SetCOG_elev (15.225);
	SetEmptyMass (1741.0);
	SetPMI (_V(1.297,1.297,0.615));
	SetCrossSections (_V(5,5,5));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

    VECTOR3 mesh_dir=_V(0,0,0);
	AddMesh (hastp, &mesh_dir);

	// Switch to compatible dock mode 
	SetDockMode(0);

	CreateDock(_V(0.0, -0.16, 1.8), DM_CSM_DOCKING_PORT_DIR, _V(0.0, -1.0, 0));
	hattDROGUE = CreateAttachment(true, _V(0.0, -0.16, 1.8), DM_CSM_DOCKING_PORT_DIR, _V(1.0, 0.0, 0), "PADROGUE");

	// Docking port used for DM/SLA connection
	DOCKHANDLE docksla;
	docksla = CreateDock(_V(0.0, -0.16, -1.421), DM_SLA_DOCKING_PORT_DIR, _V(-1.0, 0.0, 0.0));
}

void ASTP::clbkPreStep(double simt, double simdt, double mjd)
{

}

void ASTP::clbkDockEvent(int dock, OBJHANDLE connected)
{
	//For now restrict this to docking port 1 (aka DM/SLA connection)
	if (dock == 1)
	{
		if (connected)
		{
			DockConnectors(dock);
		}
		else
		{
			UndockConnectors(dock);
		}
	}
}

void ASTP::StartSeparationPyros()
{
	SLAConnector.StartSeparationPyros();
}

void ASTP::StopSeparationPyros()
{
	SLAConnector.StopSeparationPyros();
}

DMToSIVBConnector::DMToSIVBConnector()
{
}

DMToSIVBConnector::~DMToSIVBConnector()
{
}

void DMToSIVBConnector::StartSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_START_SEPARATION;

	SendMessage(cm);
}

void DMToSIVBConnector::StopSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_STOP_SEPARATION;

	SendMessage(cm);
}

DMConnector::DMConnector(ASTP *a)

{
	OurVessel = a;
}

DMConnector::~DMConnector()

{
}

DMCommandConnector::DMCommandConnector(ASTP *a) : DMConnector(a)
{
	type = CSM_PAYLOAD_COMMAND;
}

DMCommandConnector::~DMCommandConnector()
{

}

bool DMCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	PayloadSIVBMessageType messageType;

	messageType = (PayloadSIVBMessageType)m.messageType;

	switch (messageType)
	{
	case SLA_START_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StartSeparationPyros();
			return true;
		}
		break;

	case SLA_STOP_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StopSeparationPyros();
			return true;
		}
		break;

	}

	return false;
}