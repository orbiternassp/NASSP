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

#include "Orbitersdk.h"
#include "nasspdefs.h"
#include "papi.h"
#include "iu.h"
#include "IUControlDistributor.h"

IUControlDistributor::IUControlDistributor(IU *iu)
{
	this->iu = iu;

	FCCPowerOn = true;
	ControlSignalProcessorPowerOn = true;
	IsSIVBBurnModeA = false;
	IsSIVBBurnModeB = false;
	for (int i = 0;i < 5;i++)
	{
		SwitchPoint1to5[i] = false;
	}
	SIVBThrustNotOK = false;
	GSECommandVehicleLiftoffIndicationInhibit = false;
	ExcessiveRatePYRAutoAbortInhibitEnable = false;
	TwoEngOutAutoAbortInhibit = false;
	ExcessiveRatePYRAutoAbortInhibit = false;
	ExcessiveRateRollAutoAbortInhibit = false;
	ExcessiveRateRollAutoAbortInhibitEnable = false;
	TwoEngOutAutoAbortInhibitEnable = false;
}

void IUControlDistributor::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	papiWriteScenario_bool(scn, "TWOENGOUTAUTOABORTINHIBITENABLE", TwoEngOutAutoAbortInhibitEnable);
	papiWriteScenario_bool(scn, "TWOENGOUTAUTOABORTINHIBIT", TwoEngOutAutoAbortInhibit);
	papiWriteScenario_bool(scn, "EXCESSIVERATEPYRAUTOABORTINHIBITENABLE", ExcessiveRatePYRAutoAbortInhibitEnable);
	papiWriteScenario_bool(scn, "EXCESSIVERATEPYRAUTOABORTINHIBIT", ExcessiveRatePYRAutoAbortInhibit);
	papiWriteScenario_bool(scn, "EXCESSIVERATEROLLAUTOABORTINHIBITENABLE", ExcessiveRateRollAutoAbortInhibitEnable);
	papiWriteScenario_bool(scn, "EXCESSIVERATEROLLAUTOABORTINHIBIT", ExcessiveRateRollAutoAbortInhibit);
	papiWriteScenario_bool(scn, "ISSIVBBURNMODEA", IsSIVBBurnModeA);
	papiWriteScenario_bool(scn, "ISSIVBBURNMODEB", IsSIVBBurnModeB);
	papiWriteScenario_boolarr(scn, "SWITCHPOINT1TO5", SwitchPoint1to5, 5);
}

void IUControlDistributor::LoadState(char *line)
{
	papiReadScenario_bool(line, "TWOENGOUTAUTOABORTINHIBITENABLE", TwoEngOutAutoAbortInhibitEnable);
	papiReadScenario_bool(line, "TWOENGOUTAUTOABORTINHIBIT", TwoEngOutAutoAbortInhibit);
	papiReadScenario_bool(line, "EXCESSIVERATESAUTOABORTINHIBIT", ExcessiveRatePYRAutoAbortInhibit);
	papiReadScenario_bool(line, "EXCESSIVERATEPYRAUTOABORTINHIBITENABLE", ExcessiveRatePYRAutoAbortInhibitEnable);
	papiReadScenario_bool(line, "EXCESSIVERATEPYRAUTOABORTINHIBIT", ExcessiveRatePYRAutoAbortInhibit);
	papiReadScenario_bool(line, "EXCESSIVERATEROLLAUTOABORTINHIBITENABLE", ExcessiveRateRollAutoAbortInhibitEnable);
	papiReadScenario_bool(line, "EXCESSIVERATEROLLAUTOABORTINHIBIT", ExcessiveRateRollAutoAbortInhibit);
	papiReadScenario_bool(line, "ISSIVBBURNMODEA", IsSIVBBurnModeA);
	papiReadScenario_bool(line, "ISSIVBBURNMODEB", IsSIVBBurnModeB);
}

bool IUControlDistributor::GetSIVBBurnMode()
{
	if (IsSIVBBurnModeA && !SIVBThrustNotOK) return true;
	if (IsSIVBBurnModeA && SIVBThrustNotOK && IsSIVBBurnModeB) return true;
	if (IsSIVBBurnModeB && !SIVBThrustNotOK) return true;

	return false;
}

bool IUControlDistributor::GetTwoEnginesOutAutoAbortInhibit()
{
	return (TwoEngOutAutoAbortInhibitEnable && TwoEngOutAutoAbortInhibit);
}

bool IUControlDistributor::GetExcessiveRatePYRAutoAbortInhibit()
{
	return (ExcessiveRatePYRAutoAbortInhibitEnable && ExcessiveRatePYRAutoAbortInhibit);
}

bool IUControlDistributor::GetExcessiveRateRollAutoAbortInhibit()
{
	return (ExcessiveRateRollAutoAbortInhibitEnable && ExcessiveRateRollAutoAbortInhibit);
}

void IUControlDistributor::SwitchSelector(int stage, int channel)
{
	if (stage == SWITCH_SELECTOR_IU)
	{
		iu->SwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_SI)
	{
		iu->GetLVCommandConnector()->SISwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_SII)
	{
		iu->GetLVCommandConnector()->SIISwitchSelector(channel);
	}
	else if (stage == SWITCH_SELECTOR_SIVB)
	{
		iu->GetLVCommandConnector()->SIVBSwitchSelector(channel);
	}
}

IUControlDistributor1B::IUControlDistributor1B(IU *iu) : IUControlDistributor(iu)
{

}

void IUControlDistributor1B::Timestep(double simdt)
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

void IUControlDistributor1B::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	IUControlDistributor::SaveState(scn, start_str, end_str);

	oapiWriteLine(scn, end_str);
}

void IUControlDistributor1B::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		IUControlDistributor::LoadState(line);
	}
}

bool IUControlDistributor1B::GetSIBurnMode()
{
	//Normal S-I Burn Mode Logic
	if (iu->GetLVCommandConnector()->GetStage() < LAUNCH_STAGE_SIVB && !GSECommandVehicleLiftoffIndicationInhibit) return true;
	//GSE S-I Burn Mode Substitute
	if (iu->GetLVCommandConnector()->GetStage() == PRELAUNCH_STAGE) return true;

	return false;
}

IUControlDistributorSV::IUControlDistributorSV(IU *iu) : IUControlDistributor(iu)
{
	IsSIIBurnMode = false;
	SICOutboardEnginesCantInhibit = false;
	SICEngineCantA = false;
	SICEngineCantB = false;
	SICEngineCantC = false;
	for (int i = 0;i < 4;i++)
	{
		SwitchPoint6to9[i] = false;
	}
}

void IUControlDistributorSV::Timestep(double simdt)
{
	if (iu->IsUmbilicalConnected())
		GSECommandVehicleLiftoffIndicationInhibit = true;
	else
		GSECommandVehicleLiftoffIndicationInhibit = false;

	if (iu->GetLVCommandConnector()->GetSIVBThrustOK() == false)
		SIVBThrustNotOK = true;
	else
		SIVBThrustNotOK = false;

	//sprintf(oapiDebugString(), "%d %d %d %d %d %d", SwitchPoint1, SwitchPoint2, SwitchPoint3, SwitchPoint4, SwitchPoint5, SwitchPoint6);
}

void IUControlDistributorSV::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	IUControlDistributor::SaveState(scn, start_str, end_str);

	papiWriteScenario_boolarr(scn, "SWITCHPOINT6TO9", SwitchPoint6to9, 4);
	papiWriteScenario_bool(scn, "ISSIIBURNMODE", IsSIIBurnMode);
	papiWriteScenario_bool(scn, "SICENGINECANTA", SICEngineCantA);
	papiWriteScenario_bool(scn, "SICENGINECANTB", SICEngineCantB);
	papiWriteScenario_bool(scn, "SICENGINECANTC", SICEngineCantC);

	oapiWriteLine(scn, end_str);
}

void IUControlDistributorSV::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		IUControlDistributor::LoadState(line);
		papiReadScenario_boolarr(line, "SWITCHPOINT6TO9", SwitchPoint6to9, 4);
		papiReadScenario_bool(line, "ISSIIBURNMODE", IsSIIBurnMode);
		papiReadScenario_bool(line, "SICENGINECANTA", SICEngineCantA);
		papiReadScenario_bool(line, "SICENGINECANTB", SICEngineCantB);
		papiReadScenario_bool(line, "SICENGINECANTC", SICEngineCantC);
	}
}

bool IUControlDistributorSV::GetSIBurnMode()
{
	//GSE S-I Burn Mode Substitute
	if (iu->GetLVCommandConnector()->GetStage() == PRELAUNCH_STAGE) return true;
	//Normal S-IC Burn Mode Logic
	if (iu->GetLVCommandConnector()->GetStage() < LAUNCH_STAGE_SIVB && !IsSIIBurnMode && !GSECommandVehicleLiftoffIndicationInhibit) return true;

	return false;
}

bool IUControlDistributorSV::GetSIIBurnMode()
{
	if (iu->GetLVCommandConnector()->GetStage() < LAUNCH_STAGE_SIVB && IsSIIBurnMode && !GSECommandVehicleLiftoffIndicationInhibit)
		return true;

	return false;
}

bool IUControlDistributorSV::UseSICEngineCant()
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

void IUControlDistributorSV::SetSIIBurnModeEngineCantOff()
{
	IsSIIBurnMode = true;
	SICEngineCantA = false;
}

void IUControlDistributorSV::ResetBus1()
{
	IsSIIBurnMode = false;
	IsSIVBBurnModeB = false;
	if (!GSECommandVehicleLiftoffIndicationInhibit)
	{
		SICEngineCantB = false;
	}
	ExcessiveRatePYRAutoAbortInhibitEnable = false;
	ExcessiveRateRollAutoAbortInhibitEnable = false;
	TwoEngOutAutoAbortInhibit = false;
}

void IUControlDistributorSV::ResetBus2()
{
	SICEngineCantC = false;
	IsSIVBBurnModeA = false;
	if (!GSECommandVehicleLiftoffIndicationInhibit)
	{
		SICEngineCantA = false;
	}
	ExcessiveRatePYRAutoAbortInhibit = false;
	ExcessiveRateRollAutoAbortInhibit = false;
	TwoEngOutAutoAbortInhibitEnable = false;
}