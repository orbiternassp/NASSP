/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

Electrical Support Equipment for the Instrument Unit

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
#include "IUUmbilical.h"
#include "LCCPadInterface.h"
#include "LVDA.h"
#include "IU_ESE.h"

IU_ESE::IU_ESE(IUUmbilical *IuUmb, LCCPadInterface *p)
{
	Umbilical = IuUmb;
	Pad = p;

	CommandVehicleLiftoffIndicationInhibit = true;
	ThrustOKIndicateEnableInhibitA = true;
	ThrustOKIndicateEnableInhibitB = true;
	EDSLiftoffInhibitA = true;
	EDSLiftoffInhibitB = true;
	PadAbortRequest = false;
	EDSPowerInhibit = false;
	SIBurnModeSubstitute = false;
	GuidanceReferenceRelease = false;
	EDSNotReady = false;
	InstrumentUnitReady = false;

	FCCPowerIsOn = false;
	QBallSimulateCmd = false;
	SwitchFCCPowerOn = false;
	SwitchFCCPowerOff = false;

	for (int i = 0;i < 9;i++)
	{
		OverrateSimulate[i] = false;
	}
	for (int i = 0;i < 6;i++)
	{
		EDSAutoAbortSimulate[i] = false;
	}
	for (int i = 0;i < 3;i++)
	{
		EDSCutoffFromSC[i] = false;
		ExcessivePitchYawRateAutoAbortInhibit[i] = false;
		ExcessiveRollRateAutoAbortInhibit[i] = false;
		TwoEngineOutAutoAbortInhibit[i] = false;
	}

	LastMissionTime = 10000000.0;
}

void IU_ESE::SaveState(FILEHANDLE scn)
{

}

void IU_ESE::LoadState(FILEHANDLE scn)
{

}

void IU_ESE::Timestep(double MissionTime, double simdt)
{
	//FCC Power (MDI 0861, MDO 1823)
	FCCPowerIsOn = Umbilical->FCCPowerIsOn();

	if (Pad->SLCCGetOutputSignal(1823))
		SwitchFCCPowerOn = true;
	else
		SwitchFCCPowerOn = false;

	//TBD: Get analog signal from LCC
	SwitchFCCPowerOff = false;

	if (SwitchFCCPowerOn && !SwitchFCCPowerOff)
		Umbilical->SwitchFCCPowerOn();
	if (SwitchFCCPowerOff || (!SwitchFCCPowerOn && !SwitchFCCPowerOff))
		Umbilical->SwitchFCCPowerOff();

	//Q-Ball Power
	if (Pad->SLCCGetOutputSignal(492))
	{
		Umbilical->SwitchQBallPowerOn();
	}
	if (Pad->SLCCGetOutputSignal(493))
	{
		Umbilical->SwitchQBallPowerOff();
	}

	//EDS Group Reset
	if (Pad->SLCCGetOutputSignal(734))
	{
		Umbilical->EDSGroupNo1Reset();
	}
	if (Pad->SLCCGetOutputSignal(825))
	{
		Umbilical->EDSGroupNo2Reset();
	}

	//Q-Ball Simulate Command
	if (Pad->SLCCGetOutputSignal(413))
		QBallSimulateCmd = true;
	else
		QBallSimulateCmd = false;

	//EDS Auto Abort Simulate
	if (Pad->SLCCGetOutputSignal(741))
		EDSAutoAbortSimulate[0] = true;
	else
		EDSAutoAbortSimulate[0] = false;

	if (Pad->SLCCGetOutputSignal(742))
		EDSAutoAbortSimulate[1] = true;
	else
		EDSAutoAbortSimulate[1] = false;

	if (Pad->SLCCGetOutputSignal(743))
		EDSAutoAbortSimulate[2] = true;
	else
		EDSAutoAbortSimulate[2] = false;

	if (Pad->SLCCGetOutputSignal(753))
		EDSAutoAbortSimulate[3] = true;
	else
		EDSAutoAbortSimulate[3] = false;

	if (Pad->SLCCGetOutputSignal(765))
		EDSAutoAbortSimulate[4] = true;
	else
		EDSAutoAbortSimulate[4] = false;

	if (Pad->SLCCGetOutputSignal(766))
		EDSAutoAbortSimulate[5] = true;
	else
		EDSAutoAbortSimulate[5] = false;

	//EDS Cutoff from SC Simulate
	if (Pad->SLCCGetOutputSignal(802))
		EDSCutoffFromSC[0] = true;
	else
		EDSCutoffFromSC[0] = false;

	if (Pad->SLCCGetOutputSignal(803))
		EDSCutoffFromSC[1] = true;
	else
		EDSCutoffFromSC[1] = false;

	if (Pad->SLCCGetOutputSignal(804))
		EDSCutoffFromSC[2] = true;
	else
		EDSCutoffFromSC[2] = false;

	//Overrate Auto Abort Inhibit
	if (Pad->SLCCGetOutputSignal(805))
		ExcessivePitchYawRateAutoAbortInhibit[0] = false;
	else
		ExcessivePitchYawRateAutoAbortInhibit[0] = true;

	if (Pad->SLCCGetOutputSignal(806))
		ExcessivePitchYawRateAutoAbortInhibit[1] = false;
	else
		ExcessivePitchYawRateAutoAbortInhibit[1] = true;

	if (Pad->SLCCGetOutputSignal(807))
		ExcessivePitchYawRateAutoAbortInhibit[2] = false;
	else
		ExcessivePitchYawRateAutoAbortInhibit[2] = true;

	if (Pad->SLCCGetOutputSignal(808))
		ExcessiveRollRateAutoAbortInhibit[0] = false;
	else
		ExcessiveRollRateAutoAbortInhibit[0] = true;

	if (Pad->SLCCGetOutputSignal(809))
		ExcessiveRollRateAutoAbortInhibit[1] = false;
	else
		ExcessiveRollRateAutoAbortInhibit[1] = true;

	if (Pad->SLCCGetOutputSignal(810))
		ExcessiveRollRateAutoAbortInhibit[2] = false;
	else
		ExcessiveRollRateAutoAbortInhibit[2] = true;

	//Two Engine Out Auto Abort Inhibit
	if (!Pad->SLCCGetOutputSignal(811))
		TwoEngineOutAutoAbortInhibit[0] = true;
	else
		TwoEngineOutAutoAbortInhibit[0] = false;

	if (!Pad->SLCCGetOutputSignal(812))
		TwoEngineOutAutoAbortInhibit[1] = true;
	else
		TwoEngineOutAutoAbortInhibit[1] = false;

	if (!Pad->SLCCGetOutputSignal(813))
		TwoEngineOutAutoAbortInhibit[2] = true;
	else
		TwoEngineOutAutoAbortInhibit[2] = false;

	//Overrate Simulate
	if (Pad->SLCCGetOutputSignal(815))
		OverrateSimulate[3] = true;
	else
		OverrateSimulate[3] = false;

	if (Pad->SLCCGetOutputSignal(816))
		OverrateSimulate[4] = true;
	else
		OverrateSimulate[4] = false;

	if (Pad->SLCCGetOutputSignal(817))
		OverrateSimulate[5] = true;
	else
		OverrateSimulate[5] = false;

	if (Pad->SLCCGetOutputSignal(818))
		OverrateSimulate[6] = true;
	else
		OverrateSimulate[6] = false;

	if (Pad->SLCCGetOutputSignal(819))
		OverrateSimulate[7] = true;
	else
		OverrateSimulate[7] = false;

	if (Pad->SLCCGetOutputSignal(820))
		OverrateSimulate[8] = true;
	else
		OverrateSimulate[8] = false;

	if (Pad->SLCCGetOutputSignal(821))
		OverrateSimulate[0] = true;
	else
		OverrateSimulate[0] = false;

	if (Pad->SLCCGetOutputSignal(822))
		OverrateSimulate[1] = true;
	else
		OverrateSimulate[1] = false;

	if (Pad->SLCCGetOutputSignal(823))
		OverrateSimulate[2] = true;
	else
		OverrateSimulate[2] = false;

	//Thrust OK Indicate Enable Inhibit
	if (Pad->SLCCGetOutputSignal(1913))
		ThrustOKIndicateEnableInhibitA = true;
	else
		ThrustOKIndicateEnableInhibitA = false;

	if (Pad->SLCCGetOutputSignal(1914))
		ThrustOKIndicateEnableInhibitB = true;
	else
		ThrustOKIndicateEnableInhibitB = false;

	//EDS Test
	if ((MissionTime >= -6900.0) && (LastMissionTime < -6900.0))
	{
		//Abort Light On
		SetEDSMode(LCC_EDS_MODE_TEST);
		PadAbortRequest = true;
	}
	else if ((MissionTime >= -6840.0) && (LastMissionTime < -6840.0))
	{
		//Abort Light Off
		SetEDSMode(LCC_EDS_MODE_TEST);
		PadAbortRequest = false;
	}
	else if ((MissionTime >= -6780.0) && (LastMissionTime < -6780.0))
	{
		//LV engine indicators on
		SetEDSMode(LCC_EDS_MODE_TEST);
		ThrustOKIndicateEnableInhibitA = false;
		ThrustOKIndicateEnableInhibitB = false;
	}
	else if ((MissionTime >= -6720.0) && (LastMissionTime < -6720.0))
	{
		//LV rate light on
		OverrateSimulate[0] = true;
		OverrateSimulate[1] = true;
		OverrateSimulate[2] = true;
	}
	else if ((MissionTime >= -6660.0) && (LastMissionTime < -6660.0))
	{
		OverrateSimulate[0] = false;
		OverrateSimulate[1] = false;
		OverrateSimulate[2] = false;
		SetEDSMode(LCC_EDS_MODE_OFF);
	}
	else if ((MissionTime >= -6660.0) && (MissionTime < -390.0))
	{
		SetEDSMode(LCC_EDS_MODE_OFF);

	}
	else if ((MissionTime >= -390.0) && (MissionTime < -250.0))
	{
		SetEDSMode(LCC_EDS_MODE_LAUNCH);
	}
	else if (MissionTime >= -250.0)
	{
		SetEDSMode(LCC_EDS_MODE_LAUNCH);
		ThrustOKIndicateEnableInhibitA = false;
		ThrustOKIndicateEnableInhibitB = false;
		SIBurnModeSubstitute = true;

		EDSNotReady = Umbilical->IsEDSUnsafeA() || Umbilical->IsEDSUnsafeB() || !Umbilical->GetEDSSCCutoff1() || !Umbilical->GetEDSSCCutoff2() || !Umbilical->GetEDSSCCutoff3();
		EDSNotReady = EDSNotReady || Umbilical->GetEDSAutoAbortBus() || Umbilical->GetEDSExcessiveRollRateIndication() || Umbilical->GetEDSExcessivePitchYawRateIndication();

		InstrumentUnitReady = !Umbilical->GetLVDCOutputRegisterDiscrete(FiringCommitInhibit) && !Umbilical->GetLVDCOutputRegisterDiscrete(GuidanceReferenceFailureA)
			&& !Umbilical->GetLVDCOutputRegisterDiscrete(GuidanceReferenceFailureB) && !EDSNotReady;
	}

	LastMissionTime = MissionTime;
}

void IU_ESE::SetEDSMode(int mode)
{
	//Off
	if (mode == LCC_EDS_MODE_OFF)
	{
		EDSPowerInhibit = true;
		EDSLiftoffInhibitA = true;
		EDSLiftoffInhibitB = true;
		for (int i = 0;i < 9;i++)
		{
			OverrateSimulate[i] = false;
		}
		ThrustOKIndicateEnableInhibitA = true;
		ThrustOKIndicateEnableInhibitB = true;
		PadAbortRequest = false;
		Umbilical->EDSLiftoffEnableReset();
	}
	//Monitor
	else if (mode == LCC_EDS_MODE_MONITOR)
	{
		ExcessiveRollRateAutoAbortInhibit[0] = true;
		ExcessiveRollRateAutoAbortInhibit[1] = true;
		ExcessiveRollRateAutoAbortInhibit[2] = true;
		ExcessivePitchYawRateAutoAbortInhibit[0] = true;
		ExcessivePitchYawRateAutoAbortInhibit[1] = true;
		ExcessivePitchYawRateAutoAbortInhibit[2] = true;
		EDSPowerInhibit = false;
		Umbilical->EDSLiftoffEnableReset();
	}
	//Test
	else if (mode == LCC_EDS_MODE_TEST)
	{
		ExcessiveRollRateAutoAbortInhibit[0] = true;
		ExcessiveRollRateAutoAbortInhibit[1] = true;
		ExcessiveRollRateAutoAbortInhibit[2] = true;
		ExcessivePitchYawRateAutoAbortInhibit[0] = true;
		ExcessivePitchYawRateAutoAbortInhibit[1] = true;
		ExcessivePitchYawRateAutoAbortInhibit[2] = true;
		EDSPowerInhibit = false;
		Umbilical->EDSLiftoffEnableReset();
	}
	//Launch
	else if (mode == LCC_EDS_MODE_LAUNCH)
	{
		ExcessiveRollRateAutoAbortInhibit[0] = true;
		ExcessiveRollRateAutoAbortInhibit[1] = true;
		ExcessiveRollRateAutoAbortInhibit[2] = true;
		ExcessivePitchYawRateAutoAbortInhibit[0] = true;
		ExcessivePitchYawRateAutoAbortInhibit[1] = true;
		ExcessivePitchYawRateAutoAbortInhibit[2] = true;
		EDSPowerInhibit = false;
		Umbilical->SetEDSLiftoffEnableA();
		Umbilical->SetEDSLiftoffEnableB();
	}
}

bool IU_ESE::GetEDSAutoAbortSimulate(int n)
{
	if (n >= 1 && n <= 6)
	{
		return EDSAutoAbortSimulate[n - 1];
	}
	return false;
}

bool IU_ESE::GetEDSLVCutoffSimulate(int n)
{
	if (n >= 1 && n <= 3)
	{
		return EDSCutoffFromSC[n - 1];
	}
	return false;
}

bool IU_ESE::GetOverrateSimulate(int n)
{
	if (n >= 1 && n <= 9)
	{
		return OverrateSimulate[n - 1];
	}
	return false;
}

bool IU_ESE::GetExcessiveRollRateAutoAbortInhibit(int n)
{
	if (n >= 1 && n <= 3)
	{
		return ExcessiveRollRateAutoAbortInhibit[n - 1];
	}
	return false;
}

bool IU_ESE::GetExcessivePitchYawRateAutoAbortInhibit(int n)
{
	if (n >= 1 && n <= 3)
	{
		return ExcessivePitchYawRateAutoAbortInhibit[n - 1];
	}
	return false;
}

bool IU_ESE::GetTwoEngineOutAutoAbortInhibit(int n)
{
	if (n >= 1 && n <= 3)
	{
		return TwoEngineOutAutoAbortInhibit[n - 1];
	}
	return false;
}

IUSV_ESE::IUSV_ESE(IUUmbilical *IuUmb, LCCPadInterface *p) : IU_ESE(IuUmb, p)
{
	SICOutboardEnginesCantInhibit = false;
	SICOutboardEnginesCantSimulate = false;
}