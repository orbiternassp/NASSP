/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

S-IB Short Cable Mast Umbilical

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

#include "Orbitersdk.h"
#include "s1bsystems.h"
#include "SCMUmbilicalInterface.h"
#include "SCMUmbilical.h"

SCMUmbilical::SCMUmbilical(SCMUmbilicalInterface *ml)
{
	SCMUmb = ml;
	sib = NULL;
	UmbilicalConnected = false;
}

SCMUmbilical::~SCMUmbilical()
{
}

void SCMUmbilical::Connect(SIBSystems *sib)
{
	if (sib)
	{
		this->sib = sib;
		sib->ConnectUmbilical(this);
		UmbilicalConnected = true;
	}
}

void SCMUmbilical::Disconnect()
{
	if (!UmbilicalConnected) return;

	sib->DisconnectUmbilical();
	UmbilicalConnected = false;
}

void SCMUmbilical::AbortDisconnect()
{
	UmbilicalConnected = false;
}

bool SCMUmbilical::ESEGetSIBThrustOKSimulate(int eng)
{
	return SCMUmb->ESEGetSIBThrustOKSimulate(eng);
}

bool SCMUmbilical::SIStageLogicCutoff()
{
	if (!UmbilicalConnected) return false;

	return sib->GetEngineStop();
}

void SCMUmbilical::SetEngineStart(int eng)
{
	if (!UmbilicalConnected) return;

	sib->SetEngineStart(eng);
}

void SCMUmbilical::SIGSECutoff(bool cut)
{
	if (!UmbilicalConnected) return;

	sib->GSEEnginesCutoff(cut);
}