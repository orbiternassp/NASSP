/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

S-IC Tail Service Mast Umbilical

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
#include "s1csystems.h"
#include "TSMUmbilicalInterface.h"
#include "TSMUmbilical.h"

TSMUmbilical::TSMUmbilical(TSMUmbilicalInterface *ml)
{
	TSMUmb = ml;
	sic = NULL;
	UmbilicalConnected = false;
}

TSMUmbilical::~TSMUmbilical()
{
}

void TSMUmbilical::Connect(SICSystems *sic)
{
	if (sic)
	{
		this->sic = sic;
		sic->ConnectUmbilical(this);
		UmbilicalConnected = true;
	}
}

void TSMUmbilical::Disconnect()
{
	if (!UmbilicalConnected) return;

	sic->DisconnectUmbilical();
	UmbilicalConnected = false;
}

void TSMUmbilical::AbortDisconnect()
{
	UmbilicalConnected = false;
}

bool TSMUmbilical::ESEGetSICThrustOKSimulate(int eng)
{
	return TSMUmb->ESEGetSICThrustOKSimulate(eng);
}

bool TSMUmbilical::SIStageLogicCutoff()
{
	if (!UmbilicalConnected) return false;

	return sic->GetEngineStop();
}

void TSMUmbilical::SetEngineStart(int eng)
{
	if (!UmbilicalConnected) return;

	return sic->SetEngineStart(eng);
}