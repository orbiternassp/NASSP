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
	AutoAbortInhibit = true;
	OverrateSimulate = false;
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

	for (int i = 0;i < 6;i++)
	{
		EDSAutoAbortSimulate[i] = false;
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

	//TBD: Don't do this until the ground computers are further in development
	/*if (SwitchFCCPowerOn && !SwitchFCCPowerOff)
		Umbilical->SwitchFCCPowerOn();
	if (SwitchFCCPowerOff || (!SwitchFCCPowerOn && !SwitchFCCPowerOff))
		Umbilical->SwitchFCCPowerOff();*/

	//Q-Ball Power
	if (Pad->SLCCGetOutputSignal(492))
	{
		Umbilical->SwitchQBallPowerOn();
	}
	if (Pad->SLCCGetOutputSignal(493))
	{
		Umbilical->SwitchQBallPowerOff();
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
		OverrateSimulate = true;
	}
	else if ((MissionTime >= -6660.0) && (LastMissionTime < -6660.0))
	{
		OverrateSimulate = false;
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
		EDSNotReady = EDSNotReady || Umbilical->GetEDSAutoAbortBus() || Umbilical->GetEDSExcessiveRateIndication();

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
		AutoAbortInhibit = true;
		OverrateSimulate = false;
		ThrustOKIndicateEnableInhibitA = true;
		ThrustOKIndicateEnableInhibitB = true;
		PadAbortRequest = false;
		Umbilical->EDSLiftoffEnableReset();
	}
	//Monitor
	else if (mode == LCC_EDS_MODE_MONITOR)
	{
		AutoAbortInhibit = true;
		EDSPowerInhibit = false;
		Umbilical->EDSLiftoffEnableReset();
	}
	//Test
	else if (mode == LCC_EDS_MODE_TEST)
	{
		AutoAbortInhibit = true;
		EDSPowerInhibit = false;
		Umbilical->EDSLiftoffEnableReset();
	}
	//Launch
	else if (mode == LCC_EDS_MODE_LAUNCH)
	{
		AutoAbortInhibit = true;
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

IUSV_ESE::IUSV_ESE(IUUmbilical *IuUmb, LCCPadInterface *p) : IU_ESE(IuUmb, p)
{
	SICOutboardEnginesCantInhibit = false;
	SICOutboardEnginesCantSimulate = false;
}