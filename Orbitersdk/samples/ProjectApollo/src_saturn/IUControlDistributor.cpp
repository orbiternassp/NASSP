/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Control Distributor

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

#include "nasspdefs.h"
#include "iu.h"
#include "IUControlDistributor.h"

IUControlDistributor::IUControlDistributor(IU *iu)
{
	this->iu = iu;

	SICOutboardEnginesCantInhibit = false;
}

void IUControlDistributor::Timestep(double simdt)
{
	if (iu->IsUmbilicalConnected())
		GSECommandVehicleLiftoffIndicationInhibit = true;
	else
		GSECommandVehicleLiftoffIndicationInhibit = false;

	if (iu->GetLVCommandConnector()->GetSIVBThrustOK() == false)
		SIVBThrustNotOK = true;
	else
		SIVBThrustNotOK = false;
}

bool IUControlDistributor::GetSIBurnMode()
{
	if (iu->GetLVCommandConnector()->GetStage() < LAUNCH_STAGE_SIVB && !IsSIIBurnMode && !GSECommandVehicleLiftoffIndicationInhibit)
		return true;

	return false;
}

bool IUControlDistributor::GetSIIBurnMode()
{
	if (iu->GetLVCommandConnector()->GetStage() < LAUNCH_STAGE_SIVB && IsSIIBurnMode && !GSECommandVehicleLiftoffIndicationInhibit)
		return true;

	return false;
}

bool IUControlDistributor::GetSIVBBurnMode()
{
	if (IsSIVBBurnModeA && !SIVBThrustNotOK) return true;
	if (IsSIVBBurnModeA && SIVBThrustNotOK && IsSIVBBurnModeB) return true;
	if (IsSIVBBurnModeB && !SIVBThrustNotOK) return true;

	return false;
}

bool IUControlDistributor::UseSICEngineCant()
{
	if (iu->GetLVCommandConnector()->GetStage() < LAUNCH_STAGE_TWO && SICEngineCantC && !SICOutboardEnginesCantInhibit)
	{
		if (SICEngineCantA) return true;
		else if (SICEngineCantB) return true;
	}
	else if (SICEngineCantA && SICEngineCantB && !SICOutboardEnginesCantInhibit)
	{
		return true;
	}

	return false;
}

void IUControlDistributor::SetSIIBurnModeEngineCantOff()
{
	IsSIIBurnMode = true;
	SICEngineCantA = false;
}

void IUControlDistributor::ResetBus1()
{
	IsSIIBurnMode = false;
	IsSIVBBurnModeB = false;
	if (!GSECommandVehicleLiftoffIndicationInhibit)
	{
		SICEngineCantB = false;
	}
}

void IUControlDistributor::ResetBus2()
{
	SICEngineCantC = false;
	IsSIVBBurnModeA = false;
	if (!GSECommandVehicleLiftoffIndicationInhibit)
	{
		SICEngineCantA = false;
	}
}