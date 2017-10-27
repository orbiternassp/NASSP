/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Launch Vehicle Data Adapter

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

#include "soundlib.h"

#include "ioChannels.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "LVDA.h"

LVDA::LVDA()
{
	iu = NULL;
}

void LVDA::Init(IU *i)
{
	iu = i;
}

void LVDA::SwitchSelector(int stage, int channel)
{
	if (stage < 0 || stage > 3) return;

	iu->ControlDistributor(stage, channel);
}

void LVDA::SetFCCAttitudeError(VECTOR3 atterr)
{
	iu->GetFCC()->SetAttitudeError(atterr);
}

VECTOR3 LVDA::GetLVIMUAttitude()
{
	return iu->lvimu.GetTotalAttitude();
}

void LVDA::ZeroLVIMUPIPACounters()
{
	iu->lvimu.ZeroPIPACounters();
}

void LVDA::ZeroLVIMUCDUs()
{
	iu->lvimu.ZeroIMUCDUFlag = true;
}

void LVDA::ReleaseLVIMUCDUs()
{
	iu->lvimu.ZeroIMUCDUFlag = false;
}

void LVDA::ReleaseLVIMU()
{
	iu->lvimu.SetCaged(false);
}

void LVDA::DriveLVIMUGimbals(double x, double y, double z)
{
	iu->lvimu.DriveGimbals(x, y, z);
}

VECTOR3 LVDA::GetLVIMUPIPARegisters()
{
	return _V(iu->lvimu.CDURegisters[LVRegPIPAX], iu->lvimu.CDURegisters[LVRegPIPAY], iu->lvimu.CDURegisters[LVRegPIPAZ]);
}

bool LVDA::GetSIEngineOut()
{
	return iu->GetEDS()->GetSIEngineOut();
}

bool LVDA::GetSIIEngineOut()
{
	return iu->GetEDS()->GetSIIEngineOut();
}

bool LVDA::GetCMCSIVBIgnitionSequenceStart()
{
	if (iu->GetCommandConnector()->LVGuidanceSwitchState() == THREEPOSSWITCH_DOWN && iu->GetCommandConnector()->GetAGCInputChannelBit(012, SIVBIgnitionSequenceStart))
		return true;

	return false;
}

bool LVDA::GetCMCSIVBShutdown()
{
	if (iu->GetCommandConnector()->LVGuidanceSwitchState() == THREEPOSSWITCH_DOWN && iu->GetCommandConnector()->GetAGCInputChannelBit(012, SIVBCutoff))
		return true;

	return false;
}

bool LVDA::GetCMCSIVBTakeover()
{
	if (iu->GetCommandConnector()->LVGuidanceSwitchState() == THREEPOSSWITCH_DOWN && iu->GetCommandConnector()->GetAGCInputChannelBit(012, EnableSIVBTakeover))
		return true;

	return false;
}
bool LVDA::GetLVIMUFailure()
{
	return iu->lvimu.IsFailed();
}

bool LVDA::SIVBInjectionDelay()
{
	return iu->GetCommandConnector()->TLIEnableSwitchState() == TOGGLESWITCH_DOWN;
}

bool LVDA::SCInitiationOfSIISIVBSeparation()
{
	return iu->GetCommandConnector()->SIISIVbSwitchState() == TOGGLESWITCH_UP;
}

bool LVDA::GetSIIPropellantDepletionEngineCutoff()
{
	return iu->GetSIIPropellantDepletionEngineCutoff();
}

bool LVDA::SpacecraftSeparationIndication()
{
	return iu->GetLVCommandConnector()->CSMSeparationSensed();
}

bool LVDA::GetSIVBEngineOut()
{
	return iu->GetSIVBEngineOut();
}

bool LVDA::GetSIPropellantDepletionEngineCutoff()
{
	return iu->GetSIPropellantDepletionEngineCutoff();
}

void LVDA::TLIBegun()
{
	iu->GetCommandConnector()->TLIBegun();
}

void LVDA::TLIEnded()
{
	iu->GetCommandConnector()->TLIEnded();
}