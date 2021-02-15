/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Signal Conditioning Equipment

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
#include "papi.h"
#include "soundlib.h"
#include "sce.h"
#include "saturn.h"

SCE::SCE()
{
	Reset();
	sat = NULL;
	Operate = false;
	FF = false;
}

void SCE::Init(Saturn *v)
{
	sat = v;
}

void SCE::Reset()
{
	int i;

	for (i = 0;i < 15;i++)
	{
		AA[i] = 0.0;
	}
	for (i = 0;i < 12;i++)
	{
		DA[i] = 0.0;
	}
	for (i = 0;i < 11;i++)
	{
		DBA[i] = 0.0;
	}
	for (i = 0;i < 2;i++)
	{
		AC[i] = 0.0;
	}
}

double SCE::scale_data(double data, double low, double high)
{
	double step = 0;

	// First eliminate cases outside of the scales
	if (data >= high) { return 5.0; }
	if (data <= low) { return 0.0; }

	// Now figure step value
	step = ((high - low) / 5.0);
	// and return result
	return (data - low) / step;
}

bool SCE::IsPowered()
{
	bool HasBusVoltage = sat->SIGCondrFLTBusCB.Voltage() > 5.0;
	double SCEVoltage = !sat->SCESwitch.IsCenter() ? sat->SIGCondrFLTBusCB.Voltage() : 0.0;
	bool AuxModeSelected = (sat->SCESwitch.IsDown() && HasBusVoltage) && (SCEVoltage > 5.0);
	bool PowerSupplyFail = false;
	bool OverUnderVolt = SCEVoltage > 5.0 && (sat->SIGCondrFLTBusCB.Voltage() < 22.9 || sat->SIGCondrFLTBusCB.Voltage() > 40.0);
	bool AutoTurnOff = !AuxModeSelected && OverUnderVolt;
	bool ErrorDetection = PowerSupplyFail && !OverUnderVolt;
	bool FFTrigger = AuxModeSelected || (!AuxModeSelected && ErrorDetection);

	if (HasBusVoltage)
	{
		if (FFTrigger)
		{
			FF = true;
		}
	}
	else
	{
		FF = false;
	}
	bool PSNo1OffSignal = HasBusVoltage && (!FF || AutoTurnOff);
	bool PSNo2OffSignal = HasBusVoltage && (FF || AutoTurnOff);
	bool PS1IsOn = SCEVoltage > 20.0 && !PSNo1OffSignal;
	bool PS2IsOn = SCEVoltage > 20.0 && !PSNo2OffSignal;

	//sprintf(oapiDebugString(), "%d %d %d %f", PS1IsOn, PS2IsOn, FF, sat->SIGCondrFLTBusCB.Voltage());

	if (PS1IsOn || PS2IsOn) return true;

	return false;
}

void SCE::SystemTimestep()
{
	if (Operate)
		sat->SIGCondrFLTBusCB.DrawPower(18.2);
}

void SCE::Timestep()
{
	if (!Operate) {
		if (IsPowered())
			Operate = true;
		else
			return;
	}
	else if (!IsPowered()) {
		Reset();
		return;
	}

	FuelCellStatus fcStatus1, fcStatus2, fcStatus3;

	sat->GetFuelCellStatus(1, fcStatus1);
	sat->GetFuelCellStatus(2, fcStatus2);
	sat->GetFuelCellStatus(3, fcStatus3);

	//MAIN BUS A VOLTS (CC0206V)
	AA[0] = scale_data(sat->MainBusA->Voltage(), 0.0, 45.0);
	//MAIN BUS B VOLTS (CC0207V)
	AA[1] = scale_data(sat->MainBusB->Voltage(), 0.0, 45.0);
	//BAT BUS A VOLTS (CC0210V)
	AA[2] = scale_data(sat->BatteryBusA.Voltage(), 0.0, 45.0);
	//BAT BUS B VOLTS (CC0211V)
	AA[3] = scale_data(sat->BatteryBusB.Voltage(), 0.0, 45.0);
	//BAT RELAY BUS VOLTS (CC0232V)
	AA[4] = scale_data(sat->BatteryRelayBus.Voltage(), 0.0, 45.0);
	//RAD FLOW CONT SYS 1 OR 2 (SF0266X)
	AA[5] = sat->EcsRadiatorIndicator.GetState() == 1 ? 5.0 : 0.0;
	//BMAG MODE SW - ROLL ATT 1 RT 2 (CH3635X)
	AA[6] = (sat->BMAGRollSwitch.IsCenter() && sat->SCSLogicBus2.Voltage() > 10.0) ? 5.0 : 0.0;
	//BMAG MODE SW - ROLL RATE 2 (CH3636X)
	AA[7] = (sat->BMAGRollSwitch.IsUp() && sat->SCSLogicBus2.Voltage() > 10.0) ? 5.0 : 0.0;
	//BMAG MODE SW - PITCH ATT 1 RT 2 (CH3638X)
	AA[8] = (sat->BMAGPitchSwitch.IsCenter() && sat->SCSLogicBus3.Voltage() > 10.0) ? 5.0 : 0.0;
	//BMAG MODE SW - PITCH RATE 2 (CH3639X)
	AA[9] = (sat->BMAGPitchSwitch.IsUp() && sat->SCSLogicBus3.Voltage() > 10.0) ? 5.0 : 0.0;
	//BMAG MODE SW - YAW ATT 1 RT 2 (CH3641X)
	AA[10] = (sat->BMAGYawSwitch.IsCenter() && sat->SCSLogicBus3.Voltage() > 10.0) ? 5.0 : 0.0;
	//BMAG MODE SW - YAW RATE 2 (CH3642X)
	AA[11] = (sat->BMAGYawSwitch.IsUp() && sat->SCSLogicBus3.Voltage() > 10.0) ? 5.0 : 0.0;
	//SM EDS ABORT REQUEST A (BS0080X)
	AA[12] = (sat->secs.AbortLightPowerA() && ((sat->cws.UplinkTestState & 001) != 0)) ? 5.0 : 0.0;
	//SM EDS ABORT REQUEST B (BS0081X)
	AA[13] = (sat->secs.AbortLightPowerB() && ((sat->cws.UplinkTestState & 002) != 0)) ? 5.0 : 0.0;
	//MASTER CAUTION WARNING ON (CS0150X)
	AA[14] = sat->cws.GetMasterAlarm() ? 5.0 : 0.0;

	//BAT CHGR (CC0215C)
	DA[0] = scale_data(sat->BatteryCharger.Current(), 0.0, 5.0);
	//BAT A CUR (CC0222C)
	DA[1] = scale_data(sat->EntryBatteryA->Current(), 0.0, 100.0);
	//BAT B CUR (CC0223C)
	DA[2] = scale_data(sat->EntryBatteryB->Current(), 0.0, 100.0);
	//BAT C CUR (CC0224C)
	DA[3] = scale_data(sat->EntryBatteryC->Current(), 0.0, 100.0);
	//FC 1 CUR (SC2113C)
	DA[4] = scale_data(fcStatus1.Current, 0.0, 100.0);
	//FC 2 CUR (SC2114C)
	DA[5] = scale_data(fcStatus2.Current, 0.0, 100.0);
	//FC 3 CUR (SC2115C)
	DA[6] = scale_data(fcStatus3.Current, 0.0, 100.0);
	//LM HEATER CURRENT (CC2962C)
	DA[7] = scale_data(sat->LMUmbilicalFeeder.Current(), 0.0, 10.0);
	//S-IVB OX TK PRESS A (BS0050P)
	//S-IVB OX TK PRESS B (BS0051P)
	//S_IVB FUEL PRESS A (BS0052P)
	//S-IVB FUEL PRESS B (BS0053P)

	//INVERTER 1 TEMP (CC0175T)
	DBA[0] = scale_data(0.0, 32.0, 248.0);
	//INVERTER 2 TEMP (CC0176T)
	DBA[1] = scale_data(0.0, 32.0, 248.0);
	//INVERTER 3 TEMP (CC0177T)
	DBA[2] = scale_data(0.0, 32.0, 248.0);
	//FC 1 COND EXH TEMP (SC2081T)
	DBA[3] = scale_data(fcStatus1.CondenserTempF, 145, 250);
	//FC 2 COND EXH TEMP (SC2082T)
	DBA[4] = scale_data(fcStatus2.CondenserTempF, 145, 250);
	//FC 3 COND EXH TEMP (SC2083T)
	DBA[5] = scale_data(fcStatus3.CondenserTempF, 145, 250);
	//FC 1 SKIN TEMP (SC2084T)
	DBA[6] = scale_data(fcStatus1.TempF, 80, 550);
	//FC 2 SKIN TEMP (SC2085T)
	DBA[7] = scale_data(fcStatus2.TempF, 80, 550);
	//FC 3 SKIN TEMP (SC2086T)
	DBA[8] = scale_data(fcStatus3.TempF, 80, 550);
	//ENG INJECTOR FLANGE TEMP 1 (SP0061T)
	DBA[9] = scale_data(0, 0.0, 600.0);
	//ENG INJECTOR FLANGE TEMP 2 (SP0062T)
	DBA[10] = scale_data(0.0, 0.0, 600.0);

	//AC BUS 1 VOLTS (CC0200V)
	AC[0] = scale_data(sat->ACBus1.Voltage(), 0.0, 150.0);
	//AC BUS 2 VOLTS (CC0203V)
	AC[1] = scale_data(sat->ACBus2.Voltage(), 0.0, 150.0);
}

void SCE::SaveState(FILEHANDLE scn)
{
	oapiWriteLine(scn, SCE_START_STRING);

	papiWriteScenario_bool(scn, "FF", FF);

	oapiWriteLine(scn, SCE_END_STRING);
}

void SCE::LoadState(FILEHANDLE scn)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SCE_END_STRING, sizeof(SCE_END_STRING))) {
			return;
		}

		papiReadScenario_bool(line, "FF", FF);
	}
}

double SCE::GetVoltage(int n, int m)
{
	if (n == 0)
	{
		if (m < 15) return AA[m];
	}
	else if (n == 1)
	{
		if (m < 12) return DA[m];
	}
	else if (n == 2)
	{
		if (m < 11) return DBA[m];
	}
	else if (n == 3)
	{
		if (m < 2) return AC[m];
	}

	return 0.0;
}

double SCE::GetSensorExcitation5V()
{
	if (Operate) return 5.0;
	return 0.0;
}

double SCE::GetSensorExcitation10V()
{
	if (Operate) return 10.0;
	return 0.0;
}

double SCE::GetSCEPosSupplyVolts()
{
	if (Operate) return 20.0;
	return 0.0;
}

double SCE::GetSCENegSupplyVolts()
{
	if (Operate) return -20.0;
	return 0.0;
}