/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module Caution and Warning Electronics Assembly

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
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "lm_channels.h"
#include "LEM.h"
#include "lm_cwea.h"

// CWEA 

LEM_CWEA::LEM_CWEA(SoundLib &s) : soundlib(s) {
	lem = NULL;
	s.LoadSound(MasterAlarmSound, LM_MASTERALARM_SOUND);
	MasterAlarm = false;

	WaterWarningDisabled = 0;
}

void LEM_CWEA::Init(LEM *s, e_object *cwea_pwr, e_object *ma_pwr) {
	int row = 0, col = 0;
	while (col < 8) {
		while (row < 5) {
			LightStatus[row][col] = 0;
			row++;
		}
		row = 0; col++;
	}
	soundlib.LoadSound(MasterAlarmSound, LM_MASTERALARM_SOUND);

	lem = s;
}

bool LEM_CWEA::IsPowered() {
	if (cwea_pwr->Voltage() > 24.0)
		 return true;

	return false;
}

bool LEM_CWEA::IsMAPowered() {
	if (ma_pwr->Voltage() > 24.0)
		return true;

	return false;
}

void LEM_CWEA::SetMasterAlarm(bool alarm) {
	MasterAlarm = alarm;
}

void LEM_CWEA::TimeStep(double simdt) {
	ChannelValue val11;
	ChannelValue val13;
	ChannelValue val30;
	ChannelValue val33;
	ChannelValue val163;

	if (MasterAlarm && IsMAPowered()) {
		if (!MasterAlarmSound.isPlaying()) {
			MasterAlarmSound.play(LOOP, 255);
		}
	}
	else {
		MasterAlarmSound.stop();
	}

	if (lem == NULL) { return; }
	val11 = lem->agc.GetOutputChannel(011);
	val13 = lem->agc.GetOutputChannel(013);
	val30 = lem->agc.GetInputChannel(030);
	val33 = lem->agc.GetInputChannel(033);
	val163 = lem->agc.GetOutputChannel(0163);

	// 6DS2 ASC PROP LOW
	// Pressure of either ascent helium tanks below 2773 psia prior to staging, - This reason goes out when stage deadface opens.
	// Blanket pressure in fuel or oxi lines at the bi-propellant valves of the ascent stage below 120 psia
	// FIXME: Finish this!
	LightStatus[1][0] = 1;

	// 6DS3 HI/LO HELIUM REG OUTLET PRESS
	// Enabled by DES ENG "ON" command. Disabled by stage deadface open.
	// Pressure in descent helium lines downstream of the regulators is above 260 psia or below 220 psia.
	LightStatus[2][0] = 0; // Default
	if (lem->stage < 2 && lem->deca.GetK10() && lem->deca.GetK23()) {
		if (lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() > 260 || lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() < 220) {
			LightStatus[2][0] = 1;
		}
	}

	// 6DS4 DESCENT PROPELLANT LOW
	// On if fuel/oxi in descent stage below 2 minutes endurance @ 25% power prior to staging.
	// (This turns out to be 5.6%)
	// Master Alarm and Tone are disabled if this is active.
	LightStatus[3][0] = 0;
	if (lem->stage < 2 && lem->DPS.thrustOn && lem->scera2.GetSwitch(12, 3)->IsClosed()) {
		LightStatus[3][0] = 1;
	}

	// 6DS6 CES AC VOLTAGE FAILURE
	// Either CES AC voltage (26V or 28V) out of tolerance.
	// This power is provided by the ATCA main power supply and spins the RGAs and operate the AEA reference.
	// Disabled by Gyro Test Control in POS RT or NEG RT position.
	if (lem->SCS_ATCA_CB.Voltage() > 24 || lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER) {
		LightStatus[0][1] = 0;
	}
	else {
		LightStatus[0][1] = 1;
	}

	// 6DS7 CES DC VOLTAGE FAILURE
	// Any CES DC voltage out of tolerance.
	// All of these are provided by the ATCA main power supply.
	// Disabled by Gyro Test Control in POS RT or NEG RT position.
	if (lem->SCS_ATCA_CB.Voltage() > 24 || lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER) {
		LightStatus[1][1] = 0;
	}
	else {
		LightStatus[1][1] = 1;
	}

	// 6DS8 AGS FAILURE
	// On when any AGS power supply signals a failure, when AGS raises failure signal, or ASA heater fails.
	// Disabled when AGS status switch is OFF.
	// FIXME: Finish this!
	if (lem->AGSOperateSwitch.GetState() == THREEPOSSWITCH_DOWN) {
		LightStatus[2][1] = 0;
	}
	else {
		LightStatus[2][1] = 1;
	}

	// 6DS9 LGC FAILURE
	// On when any LGC power supply signals a failure, scaler fails, LGC restarts, counter fails, or LGC raises failure signal.
	// Disabled by Guidance Control switch in AGS position.
	if ((val163[Ch163DSKYWarn]) && lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP) {
		LightStatus[3][1] = 1;
	}
	else {
		LightStatus[3][1] = 0;
	}

	// 6DS10 ISS FAILURE
	// On when ISS power supply fails, PIPA fails while main engine thrusting, gimbal servo fails, CDU fails.
	// Disabled by Guidance Control switch in AGS position.
	if ((val11[ISSWarning] || val33[PIPAFailed] || val30[IMUCDUFailure] || val30[IMUFailure]) && lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP) {
		LightStatus[4][1] = 1;
	}
	else {
		LightStatus[4][1] = 0;
	}

	// 6DS11 RCS TCA WARNING
	// RCS fire command exists with no resulting chamber pressure,
	// chamber pressure present when no fire command exists,
	// opposing colinear jets on simultaneously
	// Disabled when failing TCA isol valve closes.
	// FIXME: Implement this test.
	LightStatus[0][2] = 0;

	// 6DS12 RCS A REGULATOR FAILURE
	// 6DS13 RCS B REGULATOR FAILURE
	// RCS helium line pressure above 205 pisa or below 165 psia. Disabled when main shutoff solenoid valves close.
	LightStatus[1][2] = 1;
	LightStatus[2][2] = 1;

	// 6DS14 DC BUS VOLTAGE FAILURE
	// On when CDR or SE DC bus below 26.5 V.
	LightStatus[3][2] = 0;
	if (lem->scera1.GetVoltage(15, 3) < (26.5*0.125) || lem->scera2.GetVoltage(8, 3) < (26.5*0.125)) {
		LightStatus[3][2] = 1;
	}

	// 6DS16 CABIN LOW PRESSURE WARNING
	// On when cabin pressure below 4.15 psia (+/- 0.3 psia)
	// Off when cabin pressure above 4.65 psia (+/- 0.25 psia)
	// Controlled by GF3572X
	// Disabled when both Atmosphere Revitalization Section Pressure Regulator Valves in EGRESS or CLOSE position.
	// Disabled when CABIN REPRESS is in MANUAL position
	LightStatus[0][3] = 0;
	if (lem->scera2.GetVoltage(3, 8) > 2.5 && lem->CabinRepressValveSwitch.GetState() != 0) {
		LightStatus[0][3] = 1;
	}

	// 6DS17 SUIT/FAN LOW PRESSURE WARNING
	// On when suit pressure below 3.11 psia or #2 suit fan fails.
	// Suit fan failure alarm disabled when Suit Fan DP Control CB is open by de energizing K12.
	LightStatus[1][3] = 0;
	if (lem->scera1.GetVoltage(5, 1) < (3.11 / 2)) { LightStatus[1][3] = 1; }
	if (lem->scera2.GetVoltage(3, 6) > 2.5) { LightStatus[1][3] = 1; }

	// 6DS21 HIGH HELIUM REGULATOR OUTLET PRESSURE CAUTION
	// On when helium pressure downstream of regulators in ascent helium lines above 220 psia.
	if (lem->APSPropellant.GetHeliumRegulator1OutletPressurePSI() > 220.0) {
		LightStatus[0][4] = 1;
	}
	else {
		LightStatus[0][4] = 0;
	}

	// 6DS22 ASCENT PROPELLANT LOW QUANTITY CAUTION
	// On when less than 10 seconds of ascent propellant/oxidizer remains.
	// Disabled when ascent engine is not firing.
	// FIXME: This test probably used a fixed setpoint instead of division. Investigate.
	if (lem->ph_Asc && lem->APS.thrustOn && lem->GetPropellantFlowrate(lem->ph_Asc) > 0.0 && (lem->GetPropellantMass(lem->ph_Asc) / lem->GetPropellantFlowrate(lem->ph_Asc) < 10.0)) {
		LightStatus[1][4] = 1;
	}
	else {
		LightStatus[1][4] = 0;
	}

	// 6DS23 AUTOMATIC GIMBAL FAILURE CAUTION
	// On when difference in commanded and actual descent engine trim position is detected.
	// Enabled when descent engine armed and engine gimbal switch is enabled.
	// Disabled by stage deadface open.
	if (lem->stage < 2 && (abs(lem->DPS.pitchGimbalActuator.GetPosition()) >= 6.0 || abs(lem->DPS.rollGimbalActuator.GetPosition()) >= 6.0))
	{
		LightStatus[2][4] = 1;
	}
	else
	{
		LightStatus[2][4] = 0;
	}

	// 6DS26 INVERTER FAILURE CAUTION
	// On when AC bus voltage below 112V or frequency below 398hz or above 402hz.
	// Disabled when AC Power switch is off.
	LightStatus[0][5] = 0;
	if (lem->EPSInverterSwitch.GetState() != THREEPOSSWITCH_DOWN) {
		if (lem->scera1.GetVoltage(16, 1) >= ((402.0 - 380.0)/8.0) || lem->scera1.GetVoltage(16, 1) <= ((398.0 - 380.0) / 8.0) ) { LightStatus[0][5] = 1; }
		if (lem->scera1.GetVoltage(17, 1) <= (112.0 / 25.0)) { LightStatus[0][5] = 1; }
	}

	// 6DS27 BATTERY FAILURE CAUTION
	// On when over-current, reverse-current, or over-temperature condition occurs in any ascent or descent battery.
	// Disabled if affected battery is turned off.
	// FIXME: We'll ignore this for now.
	LightStatus[1][5] = 0;

	// 6DS28 RENDEZVOUS RADAR DATA FAILURE CAUTION
	// On when RR indicates Data-Not-Good.
	// Disabled when RR mode switch is not set to AUTO TRACK.
	if (lem->RendezvousRadarRotary.GetState() != 0) {
		LightStatus[2][5] = 0;
	}
	else {
		LightStatus[2][5] = 1;
	}

	// 6DS29 LANDING RADAR was not present on LM-7 thru LM-9!
	LightStatus[3][5] = 2;

	// 6DS30 PRE-AMPLIFIER POWER FAILURE CAUTION
	// On when either ATCA solenoid driver power supply fails.
	// Disabled by stage deadface open or Abort PB press.
	LightStatus[4][5] = 0;
	if (lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP && lem->CDR_SCS_ATCA_CB.Voltage() < 24.0) { LightStatus[4][5] = 1; }
	if (lem->GuidContSwitch.GetState() == TOGGLESWITCH_DOWN && lem->SCS_ATCA_AGS_CB.Voltage() < 24.0) { LightStatus[4][5] = 1; }
	// FIXME: Handle stage DF and abort PB disables

	// 6DS31 EDS RELAY FAILURE
	// On when any EDS relay fails.
	// Failures of stage relays disabled when stage relay switch in RESET position.
	// Disabled when MASTER ARM is ON or if ABORT STAGE commanded.
	LightStatus[0][6] = 0;
	if ((lem->eds.RelayBoxA.GetStageRelayMonitor() || lem->eds.RelayBoxA.GetStageRelayMonitor()) && !(lem->EDMasterArm.IsUp() || lem->AbortStageSwitch.GetState() == 0)){LightStatus[0][6] = 1;}

	// 6DS32 RCS FAILURE CAUTION
	// On when helium pressure in either RCS system below 1700 psia.
	// Disabled when RCS TEMP/PRESS MONITOR switch in HELIUM position.
	LightStatus[1][6] = 0;
	if (lem->TempPressMonRotary.GetState() != 0) {
		LightStatus[1][6] = 1;
	}

	// 6DS33 HEATER FAILURE CAUTION
	// On when:
	// S-Band Antenna Electronic Drive Assembly < -64.08F or > 152.63F
	// RR Assembly < -54.07F or > 147.69F
	// Quad temps and LR temp do not turn the light on
	// Disabled when Temperature Monitor switch selects affected assembly.
	LightStatus[2][6] = 0;
	if (lem->TempMonitorRotary.GetState() != 0 && (lem->scera1.GetVoltage(21, 4) < ((-54.07 + 200.0) / 80.0) || lem->scera1.GetVoltage(21, 4) > ((147.69 + 200.0) / 80.0))) {LightStatus[2][6] = 1;}
	if (lem->TempMonitorRotary.GetState() != 6 && (lem->scera2.GetVoltage(21, 1) < ((-64.08 + 200.0) / 80.0) || lem->scera2.GetVoltage(21, 1) > ((153.63 + 200.0) / 80.0))) {LightStatus[2][6] = 1;}

	// 6DS34 CWEA POWER FAILURE CAUTION
	// On when any CWEA power supply indicates failure.
	// Not dimmable. Master Alarm associated with this failure cannot be silenced.
	// FIXME: We'll ignore this for now.
	LightStatus[3][6] = 0;

	// 6DS36 ECS FAILURE CAUTION
	// On when:
	// Glycol Pump Failure
	// CO2 Partial Pressure > 7.6mm
	// Water Separator Failure
	// Suit Fan #1 Failure
	// Off when (in order of failure):
	// Glycol pump pressure restored by selection of pump 2, or selecting INST(SEC) if #2 has failed
	// Restoration of normal CO2 pressure
	// Restoration of normal water separator speed
	// Selection of #2 suit fan
	LightStatus[0][7] = 0;
	if (lem->ECS_CO2_SENSOR_CB.IsPowered() && lem->scera1.GetVoltage(5, 2) >= (7.6 / 6.0)) { LightStatus[0][7] = 1; }	// CO2 Partial Pressure > 7.6mm
	if (lem->scera2.GetSwitch(12, 2)->IsClosed()) { LightStatus[0][7] = 1; } //Coolant pump 1 failure
	if (lem->GlycolRotary.GetState() == 2 && lem->scera2.GetVoltage(13, 3) > 2.5) { LightStatus[0][7] = 1; } //Coolant pump 2 failure
	if (lem->SuitFanRotary.GetState() == 1 && lem->scera2.GetVoltage(3, 5) > 2.5) { LightStatus[0][7] = 1; } //Suit fan 1 failure
	if (lem->scera1.GetVoltage(5, 3) < 1.1) { LightStatus[0][7] = 1; } //Water separator failure

	// 6DS37 OXYGEN QUANTITY CAUTION
	// On when:
	// < 135 psia in descent oxygen tank, or Less than full (<682.4 / 681.6 psia) ascent oxygen tanks, WHEN NOT STAGED
	// Less than 99.6 psia in ascent oxygen tank #1
	// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
	LightStatus[1][7] = 0;
	if (WaterWarningDisabled == 0) {
		if (lem->stage < 2 && (lem->scera1.GetVoltage(7, 1) < (681.6/200.0) || lem->scera1.GetVoltage(7, 2) < (682.4/200.0))) { LightStatus[1][7] = 1; }
		if (lem->stage < 2 && (lem->scera2.GetVoltage(8, 2) < (135.0/600.0))) { LightStatus[1][7] = 1; }
		if (lem->scera1.GetVoltage(7, 1) < (99.6/200.0)) { LightStatus[1][7] = 1; }
	}
	if (lem->QtyMonRotary.GetState() == 0 && LightStatus[1][7] != 0) {
		WaterWarningDisabled = 1;
	}

	// 6DS38 GLYCOL FAILURE CAUTION
	// On when glycol qty low in primary coolant loop or primary loop glycol temp @ accumulator > 50F
	// Disabled by Glycol Pump to INST(SEC) position
	LightStatus[2][7] = 0;
		if (lem->GlycolRotary.GetState() != 0 && lem->scera2.GetVoltage(3, 3) > 2.5) { LightStatus[2][7] = 1; }	//Glycol LLS
		if (lem->GlycolRotary.GetState() != 0 && lem->scera1.GetVoltage(10, 1) > ((50.0 - 20.0) / 20.0)) { LightStatus[2][7] = 1; } //Glycol temp > 50F (1.5V scaled)

	// 6DS39 WATER QUANTITY CAUTION
	// On when:
	// NOT STAGED: Descent water tank < 15.94% or < 94.78% in either ascent tank
	// Unequal levels in either ascent tank
	// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
	LightStatus[3][7] = 0;
	if (WaterWarningDisabled == 0) {
		if (lem->stage < 2 && (lem->scera1.GetVoltage(7, 3) < (0.1594 / 0.2))) { LightStatus[3][7] = 1; }
		if (lem->stage < 2 && (lem->scera1.GetVoltage(8, 1)  < (0.9478 / 0.2) || lem->scera1.GetVoltage(8, 2)  < (0.9478 / 0.2))) { LightStatus[3][7] = 1; }
		if ((abs(lem->scera1.GetVoltage(8, 1) - lem->scera1.GetVoltage(8, 2))/((lem->scera1.GetVoltage(8, 1) + lem->scera1.GetVoltage(8, 2))/2.0))>=0.15) { LightStatus[3][7] = 1; }
	}
	if (lem->QtyMonRotary.GetState() == 0 && LightStatus[3][7] != 0) {
		WaterWarningDisabled = 1;
	}

	// 6DS40 S-BAND RECEIVER FAILURE CAUTION
	// On when AGC signal lost.
	// Off when Range/TV function switch to OFF/RESET
	// Disabled when Range/TV switch is not in TV/CWEA ENABLE position
	LightStatus[4][7] = 0;
	if (lem->SBandRangeSwitch.GetState() == THREEPOSSWITCH_DOWN) {
		LightStatus[4][7] = 1;
	}

	// Rendezvous Radar Caution
	LightStatus[2][5] = 0;
	if (lem->RendezvousRadarRotary.GetState() == 0 && lem->RR.IsRadarDataGood() == 0) {
		LightStatus[2][5] = 1;
	}
	// CWEA TEST SWITCH FUNCTIONALITY
	if (lem->LTG_MASTER_ALARM_CB.Voltage() > 0) {
		switch (lem->LampToneTestRotary.GetState()) {
		case 0: // OFF
			PushMasterAlarm();
			break;
		case 7: // OFF
			PushMasterAlarm();
			break;
		case 1: // ALARM/TONE
				// Light MASTER ALARM and sound tone
				// FIXME: IMPLEMENT THIS
			SetMasterAlarm(true);
			break;
		case 2: // C/W 1
				// Light Panel 1 first bank warning lamps
			LightStatus[0][0] = 1; LightStatus[1][0] = 1; LightStatus[2][0] = 1; LightStatus[3][0] = 1; LightStatus[4][0] = 1;
			LightStatus[0][1] = 1; LightStatus[1][1] = 1; LightStatus[2][1] = 1; LightStatus[3][1] = 1; LightStatus[4][1] = 1;
			break;
		case 3: // ENG PB & C/W 2
				// Light engine START/STOP lights and Panel 1 second bank warning lamps
				// FIX ME: Lit engine start/stop light bmp and logic needed
			LightStatus[0][2] = 1; LightStatus[1][2] = 1; LightStatus[2][2] = 1; LightStatus[3][2] = 1; LightStatus[4][2] = 1;
			LightStatus[0][3] = 1; LightStatus[1][3] = 1; LightStatus[2][3] = 1; LightStatus[3][3] = 1; LightStatus[4][3] = 1;
			break;
		case 4: // C/W 3
				// Light Panel 2 first bank warning lamps;
			LightStatus[0][4] = 1; LightStatus[1][4] = 1; LightStatus[2][4] = 1; LightStatus[3][4] = 1; LightStatus[4][4] = 1;
			LightStatus[0][5] = 1; LightStatus[1][5] = 1; LightStatus[2][5] = 1; /* LightStatus[3][5] = 1; */ LightStatus[4][5] = 1; // LDG RDR lamp only for LM10+
			break;
		case 5: // C/W 4
				// Light Panel 2 second bank warning lamps;
			LightStatus[0][6] = 1; LightStatus[1][6] = 1; LightStatus[2][6] = 1; LightStatus[3][6] = 1; LightStatus[4][6] = 1;
			LightStatus[0][7] = 1; LightStatus[1][7] = 1; LightStatus[2][7] = 1; LightStatus[3][7] = 1; LightStatus[4][7] = 1;
			break;
		case 6: // COMPNT
				// Light component caution and Lunar Contact lights
				// Lunar Contact and Component lights are lit in clbkPanelRedrawEvent code
			break;
		}
	}
}

void LEM_CWEA::SystemTimestep(double simdt) {

}

void LEM_CWEA::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {

}

void LEM_CWEA::LoadState(FILEHANDLE scn, char *end_str) {

}

void LEM_CWEA::RedrawLeft(SURFHANDLE sf, SURFHANDLE ssf) {
	int row = 0, col = 0, dx = 0, dy = 0;
	while (col < 4) {
		switch (col) {
		case 0:
			dx = 0; break;
		case 1:
			dx = 71; break;
		case 2:
			dx = 167; break;
		case 3:
			dx = 238; break;
		}
		while (row < 5) {
			if (LightStatus[row][col] == 1 && lem->INST_CWEA_CB.Voltage() > 24) {
				dy = 134;
			}
			else {
				dy = 7;
			}
			if (LightStatus[row][col] == 2) {
				// Special Hack: This Lamp Doesn't Exist
				oapiBlt(sf, ssf, 8 + dx, 7 + (row * 23), 8, 7, 67, 19);
			}
			else {
				oapiBlt(sf, ssf, 8 + dx, 7 + (row * 23), 8 + dx, dy + (row * 23), 67, 19);
			}
			row++;
		}
		row = 0; col++;
	}
}

void LEM_CWEA::RedrawRight(SURFHANDLE sf, SURFHANDLE ssf) {
	int row = 0, col = 0, dx = 0, dy = 0;
	while (col < 4) {
		switch (col) {
		case 0:
			dx = 0; break;
		case 1:
			dx = 71; break;
		case 2:
			dx = 146; break;
		case 3:
			dx = 217; break;
		}
		while (row < 5) {
			if (LightStatus[row][col + 4] == 1 && lem->INST_CWEA_CB.Voltage() > 24) {
				dy = 134;
			}
			else {
				dy = 7;
			}
			if (LightStatus[row][col + 4] == 2) {
				// Special Hack: This Lamp Doesn't Exist
				oapiBlt(sf, ssf, 8 + dx, 7 + (row * 23), 8, 7, 67, 19);
			}
			else {
				oapiBlt(sf, ssf, 8 + dx, 7 + (row * 23), 330 + dx, dy + (row * 23), 67, 19);
			}
			row++;
		}
		row = 0; col++;
	}
}

void LEM_CWEA::RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit, SURFHANDLE border) {

			//
			// Draw the master alarm lit bitmap.
			//
		if (MasterAlarm)
		oapiBlt(surf, alarmLit, 0, 0, 0, 0, 47, 43);

			/*if (border)
					oapiBlt(surf, border, 0, 0, 0, 0, 45, 36, SURF_PREDEF_CK);
			*/
		
}

void LEM_CWEA::PushMasterAlarm()

{
	if (IsMAPowered()) {
		MasterAlarmSound.stop();
		SetMasterAlarm(false);
	}
}

bool LEM_CWEA::CheckMasterAlarmMouseClick(int event) {
	if (event & PANEL_MOUSE_LBDOWN) {
		PushMasterAlarm();
	}
	return true;
}