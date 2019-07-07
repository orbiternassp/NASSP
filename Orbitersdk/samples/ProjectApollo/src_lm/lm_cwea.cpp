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
#include "LEM.h"
#include "papi.h"
#include "lm_cwea.h"

// CWEA 

LEM_CWEA::LEM_CWEA(SoundLib &s, Sound &buttonsound) : soundlib(s), ButtonSound(buttonsound) {
	cwea_pwr = NULL;
	ma_pwr = NULL;
	lem = NULL;
	CWEAHeat = 0;
	SecCWEAHeat = 0;

	s.LoadSound(MasterAlarmSound, LM_MASTERALARM_SOUND);
	MasterAlarm = false;
	Operate = false;
	ECSFailureCount = 0;
}

void LEM_CWEA::Init(LEM *l, e_object *cwea, e_object *ma, h_HeatLoad *cweah, h_HeatLoad *seccweah) {
	int row = 0, col = 0;
	while (col < 8) {
		while (row < 5) {
			LightStatus[row][col] = 0;
			row++;
		}
		row = 0; col++;
	}
	soundlib.LoadSound(MasterAlarmSound, LM_MASTERALARM_SOUND, INTERNAL_ONLY);

	cwea_pwr = cwea;
	ma_pwr = ma;
	lem = l;
	CWEAHeat = cweah;
	SecCWEAHeat = seccweah;
}

bool LEM_CWEA::IsCWEAPowered() {
	if (cwea_pwr->Voltage() > SP_MIN_DCVOLTAGE)
		 return true;

	return false;
}

bool LEM_CWEA::IsMAPowered() {
	if (ma_pwr->Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

bool LEM_CWEA::IsLTGPowered() {
	if (lem->lca.GetAnnunVoltage() > 2.25)
		return true;

	return false;
}

bool LEM_CWEA::IsCWPWRLTGPowered() {
	if (lem->lca.GetCompDockVoltage() > 2.25)
		return true;

	return false;
}

void LEM_CWEA::SetMasterAlarm(bool alarm) {
	if (IsMAPowered())
	MasterAlarm = alarm;
}

void LEM_CWEA::Timestep(double simdt) {
	bool lightlogic;

	if (MasterAlarm && IsMAPowered()) {
		if (!MasterAlarmSound.isPlaying()) {
			MasterAlarmSound.play(LOOP, 255);
		}
	}
	else if (!IsMAPowered() ){
		MasterAlarmSound.stop();
		MasterAlarm = false;
	}

	if (lem == NULL) { return; }

	if (!Operate) {
		if (IsCWEAPowered())
			TurnOn();
	}
	else if (!IsCWEAPowered()) {
		TurnOff();
	}

	if (IsCWEAPowered())
	{
		// 6DS34 CWEA POWER FAILURE CAUTION
		// On when any CWEA power supply indicates failure.
		// Not dimmable. Master Alarm associated with this failure cannot be silenced.
		SetLight(3, 6, 0);

		// 6DS2 ASC PRESS LOW
		// Pressure of either ascent helium tanks < 2773 psia prior to staging
		// Disabled when stage deadface opens.
		lightlogic = false;
		if (lem->stage < 2 && (lem->GetAPSPropellant()->GetAscentHelium1PressPSI() < 2772.8 || lem->GetAPSPropellant()->GetAscentHelium2PressPSI() < 2773.0)) {
			lightlogic = true;
		}
		// Fuel and oxidizer pressure < 119.8 psia prior to staging, cut and capped from the CWEA on LM-8 and subsequent
		if (((lem->ApolloNo < 14 || lem->ApolloNo == 1301) && lem->stage < 2) && (lem->APSPropellant.GetFuelTrimOrificeOutletPressurePSI() < 119.8 || lem->APSPropellant.GetOxidTrimOrificeOutletPressurePSI() < 119.8)) {
			lightlogic = true;
		}

		if (lightlogic) {
			SetLight(1, 0, 1);
		}
		else
			SetLight(1, 0, 0);

		// 6DS3 DES HI/LO HELIUM REG OUTLET PRESS
		// Enabled by DES ENG "ON" command. Disabled by stage deadface open.
		// Pressure in descent helium lines downstream of the regulators is above 259.1 psia or below 219.2 psia.
		lightlogic = false;

		DesRegWarnFF.Set(lem->scera1.GetVoltage(3, 9) > 2.5 || lem->eds.GetHeliumPressDelayContactClosed());
		DesRegWarnFF.Reset(false);

		if (lem->stage < 2 && (lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() > 259.1 || lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() < 219.2)) { 
				lightlogic = true; //Pressure is default at 245 so this will not be true until He MP is simulated
		}

		if (lightlogic && DesRegWarnFF.IsSet()) {
			SetLight(2, 0, 1);
		}
		else
			SetLight(2, 0, 0);

		//sprintf(oapiDebugString(), "LL %i DPSHe %lf DCFF %i", lightlogic, lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI(), DesRegWarnFF);

		// 6DS4 DESCENT PROPELLANT LOW
		// On if fuel/oxi in descent stage below 2 minutes endurance @ 25% power prior to staging.
		// (This turns out to be 5.6%)
		// Master Alarm and Tone are disabled if this is active.
		//if (lem->stage < 2 && lem->DPS.thrustOn && lem->scera2.GetSwitch(12, 3)->IsClosed())
		if (lem->scera2.GetSwitch(12, 3)->IsClosed()) // Only the quantity sets off the light, DPS "on" logic cut and capped from MA logic
			SetLight(3, 0, 1, false);
		else
			SetLight(3, 0, 0, false);

		// 6DS6 CES AC VOLTAGE FAILURE
		// Either CES AC voltage (26V or 28V) out of tolerance.
		// This power is provided by the ATCA main power supply and spins the RGAs and operate the AEA reference.
		// Disabled by Gyro Test Control in POS RT or NEG RT position.
		// Needs RGA data
		CESACWarnFF.Set(lem->SCS_ATCA_CB.Voltage() < 24.0);
		CESACWarnFF.Reset(lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER);

		if (CESACWarnFF.IsSet())
			SetLight(0, 1, 1);
		else
			SetLight(0, 1, 0);

		// 6DS7 CES DC VOLTAGE FAILURE
		// Any CES DC voltage out of tolerance.
		// All of these are provided by the ATCA main power supply.
		// Disabled by Gyro Test Control in POS RT or NEG RT position.
		// Needs RGA data
		CESDCWarnFF.Set(lem->SCS_ATCA_CB.Voltage() < 24.0);
		CESDCWarnFF.Reset(lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER);

		if (CESDCWarnFF.IsSet())
			SetLight(1, 1, 1);
		else
			SetLight(1, 1, 0);

		// 6DS8 AGS FAILURE
		// On when any ASA power supply signals a failure, when AGS raises failure signal, or ASA overtemp.
		// Disabled when AGS status switch is OFF.
		lightlogic = false;
		AGSWarnFF.Set(lem->scera1.GetVoltage(4, 1) > 2.5 && lem->AGSOperateSwitch.GetState() != THREEPOSSWITCH_DOWN);
		AGSWarnFF.Reset(lem->QtyMonRotary.GetState() == 0);

		if (lem->AGSOperateSwitch.GetState() != THREEPOSSWITCH_DOWN) {
			if (lem->scera1.GetVoltage(15, 4) > (13.2 / 2.8) || lem->scera1.GetVoltage(15, 4) < (10.8 / 2.8)) { lightlogic = true; } // ASA +12VDC **Open circuit by overtemp condition**
			if (lem->scera2.GetVoltage(15, 3) > (30.8 / 8.0) || lem->scera2.GetVoltage(15, 3) < (25.2 / 8.0)) { lightlogic = true; } // ASA +28VDC  
			if (lem->scera1.GetVoltage(16, 2) > ((415.0 - 380.0) / 8.0) || lem->scera1.GetVoltage(16, 2) < ((385.0 - 380.0) / 8.0)) { lightlogic = true; } // ASA Freq
		}

		if (lightlogic || AGSWarnFF.IsSet())
			SetLight(2, 1, 1);
		else
			SetLight(2, 1, 0);

		// 6DS9 LGC FAILURE
		// On when any LGC power supply signals a failure, scaler fails, LGC restarts, counter fails, or LGC raises failure signal.
		// Disabled by Guidance Control switch in AGS position.
		if (lem->scera2.GetVoltage(3, 5) < 2.5 && lem->scera2.GetVoltage(3, 11) > 2.5)
			SetLight(3, 1, 1);
		else
			SetLight(3, 1, 0);

		// 6DS10 ISS FAILURE
		// On when ISS power supply fails, PIPA fails while main engine thrusting, gimbal servo fails, CDU fails.
		// Disabled by Guidance Control switch in AGS position.
		if (lem->scera2.GetVoltage(3, 5) < 2.5 && lem->scera2.GetVoltage(3, 12) > 2.5)
			SetLight(4, 1, 1);
		else
			SetLight(4, 1, 0);

		// 6DS11 RCS TCA WARNING
		// RCS fire command exists with no resulting chamber pressure,
		// chamber pressure present when no fire command exists,
		// opposing colinear jets on simultaneously.
		// Disabled when failing TCA isol valve closes.
		if (lem->tca1A.GetTCAFailure() || lem->tca1B.GetTCAFailure() ||
			lem->tca2A.GetTCAFailure() || lem->tca2B.GetTCAFailure() ||
			lem->tca3A.GetTCAFailure() || lem->tca3B.GetTCAFailure() ||
			lem->tca4A.GetTCAFailure() || lem->tca4B.GetTCAFailure())
			SetLight(0, 2, 1);
		else
			SetLight(0, 2, 0);

		// 6DS12 RCS A REGULATOR FAILURE
		// RCS helium line pressure above 218.8 pisa or below 164.4 psia. Disabled when main shutoff solenoid valves close.
		if (lem->scera1.GetVoltage(12, 1) < 2.5 && (lem->scera1.GetVoltage(6, 3) > (218.8 / 70.0) || lem->scera1.GetVoltage(6, 3) < (164.4 / 70.0)))
			SetLight(1, 2, 1);
		else
			SetLight(1, 2, 0);

		// 6DS13 RCS B REGULATOR FAILURE
		// RCS helium line pressure above 218.8 pisa or below 164.4 psia. Disabled when main shutoff solenoid valves close.
		if (lem->scera1.GetVoltage(12, 2) < 2.5 && (lem->scera1.GetVoltage(6, 4) > (218.8 / 70.0) || lem->scera1.GetVoltage(6, 4) < (164.4 / 70.0)))
			SetLight(2, 2, 1);
		else
			SetLight(2, 2, 0);

		// 6DS14 DC BUS VOLTAGE FAILURE
		// On when CDR or SE DC bus below 26.5 V.
		if (lem->scera1.GetVoltage(15, 3) < (26.5*0.125) || lem->scera2.GetVoltage(15, 4) < (26.5*0.125))
			SetLight(3, 2, 1);
		else
			SetLight(3, 2, 0);

		// 6DS16 CABIN LOW PRESSURE WARNING
		// On when cabin pressure below 4.15 psia (+/- 0.3 psia)
		// Off when cabin pressure above 4.65 psia (+/- 0.25 psia)
		// Controlled by GF3572X
		// Disabled when both Atmosphere Revitalization Section Pressure Regulator Valves in EGRESS or CLOSE position.
		// Disabled when CABIN REPRESS is in MANUAL position
		if (lem->scera2.GetVoltage(3, 8) > 2.5 && lem->CabinRepressValveSwitch.GetState() != 0)
			SetLight(0, 3, 1);
		else
			SetLight(0, 3, 0);

		// 6DS17 SUIT/FAN LOW PRESSURE WARNING
		// On when suit pressure below 3.11 psia or #2 suit fan fails.
		// Suit fan failure alarm disabled when Suit Fan DP Control CB is open by de energizing K12.
		if (lem->scera1.GetVoltage(5, 1) < (3.11 / 2.0) || (lem->scera2.GetVoltage(3, 6) > 2.5))
			SetLight(1, 3, 1);
		else
			SetLight(1, 3, 0);

		// 6DS21 HIGH HELIUM REGULATOR OUTLET PRESSURE CAUTION
		// On when helium pressure downstream of regulators in ascent helium lines above 220 psia.
		if (lem->scera1.GetVoltage(8, 3) > (219.5 / 60.0))
			SetLight(0, 4, 1);
		else
			SetLight(0, 4, 0);

		// 6DS22 ASCENT PROPELLANT LOW QUANTITY CAUTION
		// On when less than 10 seconds of ascent propellant/oxidizer remains, <= 2.2%
		// Disabled when ascent engine is not firing.
		if (lem->scera2.GetVoltage(2, 10) > 2.5 && (lem->scera2.GetVoltage(2, 6) > 2.5 || lem->scera2.GetVoltage(2, 7) > 2.5)) {
			SetLight(1, 4, 1);
		}
		else
			SetLight(1, 4, 0);

		// 6DS23 AUTOMATIC GIMBAL FAILURE CAUTION
		// On when difference in commanded and actual descent engine trim position is detected.
		// Enabled when descent engine armed and engine gimbal switch is enabled.
		// Disabled by stage deadface open.
		if (lem->stage < 2 && (lem->scera2.GetVoltage(3, 9) > 2.5 || lem->scera2.GetVoltage(3, 10) > 2.5)) {
				SetLight(2, 4, 1);
		}
		else
			SetLight(2, 4, 0);

		// 6DS26 INVERTER FAILURE CAUTION
		// On when AC bus voltage below 112V or frequency below 398hz or above 402hz.
		// Disabled when AC Power switch is off.
		lightlogic = false;
		if (lem->EPSInverterSwitch.GetState() != THREEPOSSWITCH_DOWN) {
			if (lem->scera1.GetVoltage(16, 1) >= ((402.0 - 380.0) / 8.0) || lem->scera1.GetVoltage(16, 1) <= ((398.0 - 380.0) / 8.0) || lem->scera1.GetVoltage(17, 1) <= (112.0 / 25.0)) {
				lightlogic = true;
			}
		}

		if (lightlogic)
			SetLight(0, 5, 1);
		else
			SetLight(0, 5, 0);

		// 6DS27 BATTERY FAILURE CAUTION
		// On when over-current, reverse-current, or over-temperature condition occurs in any ascent or descent battery.
		// Disabled if affected battery is turned off.
		// FIXME: We'll ignore this for now until these data points are implemented in the ECA
		SetLight(1, 5, 0);

		// 6DS28 RENDEZVOUS RADAR DATA FAILURE CAUTION
		// On when RR indicates Data-Not-Good.
		// Disabled when RR mode switch is not set to AUTO TRACK.
		RRCautFF.Set(lem->scera2.GetVoltage(2, 5) < 2.5 && lem->RendezvousRadarRotary.GetState() == 0);
		RRCautFF.Reset(lem->RendezvousRadarRotary.GetState() == 0);

		if (RRCautFF.IsSet() && lem->scera2.GetVoltage(2, 5) >= 2.5 && lem->RendezvousRadarRotary.GetState() == 0) {
			SetLight(2, 5, 1);
		}
		else
			SetLight(2, 5, 0);

		//sprintf(oapiDebugString(), "RRC %i FFS %i FFR %i SCV %lf", RRCautFF.IsSet(), RRCautFF.GetSInput(), RRCautFF.GetRInput(), lem->scera2.GetVoltage(2, 5));

		// 6DS29 LANDING RADAR 
		// Was not present on LM-7 thru LM-9!  **What about LM 3-5?  Unlikely but need to research**
		SetLight(3, 5, 2);

		// 6DS30 PRE-AMPLIFIER POWER FAILURE CAUTION
		// On when either ATCA solenoid driver power supply fails.
		// Disabled by stage deadface open or Abort PB press.
		lightlogic = false;
		if (lem->scera1.GetVoltage(2, 9) < 2.5 && lem->stage < 2) {
			if (lem->scera2.GetVoltage(15, 1) < ((-5.2 + 9.4169) / 1.2048) || lem->scera2.GetVoltage(15, 1) > ((-4.2 + 9.4169) / 1.2048)) { lightlogic = true; }
			if (lem->scera2.GetVoltage(15, 2) < ((-5.2 + 9.4169) / 1.2048) || lem->scera2.GetVoltage(15, 2) > ((-4.2 + 9.4169) / 1.2048)) { lightlogic = true; }
		}

		if (lightlogic)
			SetLight(4, 5, 1);
		else
			SetLight(4, 5, 0);

		// 6DS31 EDS RELAY FAILURE
		// On when any EDS relay fails.
		// Failures of stage relays disabled when stage relay switch in RESET position.
		// Disabled when MASTER ARM is ON or if ABORT STAGE commanded.
		if (!(lem->EDMasterArm.IsUp() || lem->scca1.GetK19()) && (lem->scera1.GetVoltage(14, 11) > 2.5 || lem->scera1.GetVoltage(14, 12) > 2.5))
			SetLight(0, 6, 1);
		else
			SetLight(0, 6, 0);

		// 6DS32 RCS FAILURE CAUTION
		// On when helium pressure in either RCS system below 1700 psia.
		// Disabled when RCS TEMP/PRESS MONITOR switch in HELIUM position.
		RCSCautFF1.Set(lem->scera1.GetVoltage(6, 1) < 2.423);
		RCSCautFF1.Reset(lem->TempPressMonRotary.GetState() == 0);
		RCSCautFF2.Set(lem->scera1.GetVoltage(6, 2) < 2.423);
		RCSCautFF2.Reset(lem->TempPressMonRotary.GetState() == 0);

		if (RCSCautFF1.IsSet() || RCSCautFF2.IsSet())
			SetLight(1, 6, 1);
		else
			SetLight(1, 6, 0);

		// 6DS33 HEATER FAILURE CAUTION
		// On when:
		// LR temp cut and capped from CW logic

		// RR Assembly < -54.07F or > 147.69F
		RRHeaterCautFF.Set(lem->scera1.GetVoltage(21, 4) < 1.869 || lem->scera1.GetVoltage(21, 4) > 4.361);
		RRHeaterCautFF.Reset(lem->TempMonitorRotary.GetState() == 0);

		// RCS Quads < 118.8F  or > 190.5F, cut and capped on LM-8 and subsequent
		//Quad 1
		QD1HeaterCautFF.Set((lem->ApolloNo < 14 || lem->ApolloNo == 1301) && (lem->scera1.GetVoltage(20, 4) < 2.79 || lem->scera1.GetVoltage(20, 4) > 4.725));
		QD1HeaterCautFF.Reset(lem->TempMonitorRotary.GetState() == 2);

		//Quad 2
		QD2HeaterCautFF.Set((lem->ApolloNo < 14 || lem->ApolloNo == 1301) && (lem->scera1.GetVoltage(20, 3) < 2.79 || lem->scera1.GetVoltage(20, 3) > 4.725));
		QD2HeaterCautFF.Reset(lem->TempMonitorRotary.GetState() == 3);

		//Quad 3
		QD3HeaterCautFF.Set((lem->ApolloNo < 14 || lem->ApolloNo == 1301) && (lem->scera1.GetVoltage(20, 2) < 2.79 || lem->scera1.GetVoltage(20, 2) > 4.725));
		QD3HeaterCautFF.Reset(lem->TempMonitorRotary.GetState() == 4);

		//Quad 4
		QD4HeaterCautFF.Set((lem->ApolloNo < 14 || lem->ApolloNo == 1301) && (lem->scera1.GetVoltage(20, 1) < 2.79 || lem->scera1.GetVoltage(20, 1) > 4.725));
		QD4HeaterCautFF.Reset(lem->TempMonitorRotary.GetState() == 5);

		// S-Band Antenna Electronic Drive Assembly < -64.08F or > 152.63F
		SBDHeaterCautFF.Set(lem->scera2.GetVoltage(21, 2) < 1.743 || lem->scera2.GetVoltage(21, 2) > 4.421);
		SBDHeaterCautFF.Reset(lem->TempMonitorRotary.GetState() == 6);

		//Set CW Light
		if (RRHeaterCautFF.IsSet() || SBDHeaterCautFF.IsSet() || QD1HeaterCautFF.IsSet() || QD2HeaterCautFF.IsSet() || QD3HeaterCautFF.IsSet() || QD4HeaterCautFF.IsSet())
			SetLight(2, 6, 1);
		else
			SetLight(2, 6, 0);

		//sprintf(oapiDebugString(), "QD1HFF %i QD1HLogic %i QD1Prev %i QD2HFF %i QD3HFF %i QD4HFF %i", QD1HeaterCautFF, QD1HeaterLogic, QD1HeaterPrev, QD2HeaterCautFF, QD3HeaterCautFF, QD4HeaterCautFF);

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
		lightlogic = false;
		if (lem->ecs.GetSensorCO2MMHg() >= 7.6) { lightlogic = true;}	// CO2 Partial Pressure > 7.6mm
		if (lem->scera2.GetVoltage(13, 3) > 2.5) { lightlogic = true; } // Glycol pump failure
		if (lem->scera2.GetVoltage(3, 2) > 2.5) { lightlogic = true; } // Suit fan 1 failure
		if (lem->ecs.GetWaterSeparatorRPM() < 792.5) { lightlogic = true; } // Water separator failure

		// To avoid spurious alarms because of fluctuation at high time accelerations
		// the "bad" condition has to last for a few check counts.
		if (lightlogic)
		{
			if (ECSFailureCount < 20) ECSFailureCount++;
		}
		else
		{
			ECSFailureCount = 0;
		}

		if (lightlogic && ECSFailureCount >= 20)
			SetLight(0, 7, 1);
		else
			SetLight(0, 7, 0);

		// 6DS37 OXYGEN QUANTITY CAUTION
		// On when:
		// < 135 psia in descent oxygen tank, or Less than full (<682.4 / 681.6 psia) ascent oxygen tanks, WHEN NOT STAGED
		// Less than 99.6 psia in ascent oxygen tank #1
		// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.

		// Unstaged less than full ASC tanks
		OxygenCautFF1.Set(lem->scera1.GetVoltage(7, 1) < 3.408 || lem->scera1.GetVoltage(7, 2) < 3.408);
		OxygenCautFF1.Reset(lem->QtyMonRotary.GetState() == 0);

		// Unstaged low DES tank
		OxygenCautFF2.Set(lem->scera2.GetVoltage(8, 1) < 2.22);
		OxygenCautFF2.Reset(lem->QtyMonRotary.GetState() == 0);

		// Low ASC tank 1
		OxygenCautFF3.Set(lem->scera1.GetVoltage(7, 1) < 0.498);
		OxygenCautFF3.Reset(lem->QtyMonRotary.GetState() == 0);

		if ((lem->stage < 2 && (OxygenCautFF2.IsSet() || OxygenCautFF1.IsSet())) || OxygenCautFF3.IsSet())
			SetLight(1, 7, 1);
		else
			SetLight(1, 7, 0);

		// 6DS38 GLYCOL FAILURE CAUTION
		// On when glycol qty low in primary coolant loop or primary loop glycol temp @ accumulator > 50F
		// Disabled by Glycol Pump to INST(SEC) position
		lightlogic = false;
		if (lem->GlycolRotary.GetState() != 0 && lem->scera2.GetVoltage(3, 3) > 2.5) { lightlogic = true; }	// Glycol LLS
		if (lem->GlycolRotary.GetState() != 0 && lem->scera1.GetVoltage(10, 1) > ((50.0 - 20.0) / 20.0)) { lightlogic = true; } // Glycol temp > 50F

		if (lightlogic)
			SetLight(2, 7, 1);
		else
			SetLight(2, 7, 0);

		// 6DS39 WATER QUANTITY CAUTION
		// On when:
		// NOT STAGED: Descent water tank < 15.94% or < 94.78% in either ascent tank
		// Unequal levels in either ascent tank
		// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
		WaterCautFF1.Set(lem->scera1.GetVoltage(7, 3) < 0.799);
		WaterCautFF1.Reset(lem->QtyMonRotary.GetState() == 0);

		WaterCautFF2.Set(lem->scera1.GetVoltage(8, 1) < 4.739 || lem->scera1.GetVoltage(8, 2) < 4.739);
		WaterCautFF2.Reset(lem->QtyMonRotary.GetState() == 0);

		WaterCautFF3.Set((abs(lem->scera1.GetVoltage(8, 1) - lem->scera1.GetVoltage(8, 2)) / ((lem->scera1.GetVoltage(8, 1) + lem->scera1.GetVoltage(8, 2)) / 2.0)) >= 0.15);
		WaterCautFF3.Reset(lem->QtyMonRotary.GetState() == 0);

		if ((lem->stage < 2 && (WaterCautFF1.IsSet() || WaterCautFF2.IsSet())) || WaterCautFF3.IsSet())
			SetLight(3, 7, 1);
		else
			SetLight(3, 7, 0);

		// 6DS40 S-BAND RECEIVER FAILURE CAUTION
		// On when reciever signal lost.
		// Off when Range/TV function switch to OFF/RESET
		// Disabled when Range/TV switch is not in TV/CWEA ENABLE position
		SBDCautFF.Set(lem->scera1.GetVoltage(5, 4) < 1.071);
		SBDCautFF.Reset(lem->SBandRangeSwitch.GetState() == THREEPOSSWITCH_CENTER);

		if (lem->SBandRangeSwitch.GetState() == THREEPOSSWITCH_DOWN && SBDCautFF.IsSet())
			SetLight(4, 7, 1);
		else
			SetLight(4, 7, 0);
	}
	else
	{

		SetLightStates(0);

		//Only for LM10+
		SetLight(3, 5, 2);

		//CWEA PWR
		SetLight(3, 6, 1);
	}

	// CWEA TEST SWITCH FUNCTIONALITY
	switch (lem->LampToneTestRotary.GetState()) {
	case 0: // OFF
	case 7: // OFF
		break;
	case 1: // ALARM/TONE
		SetMasterAlarm(true);
		break;
	case 2: // C/W 1
			// Light Panel 1 first bank warning lamps
		LightStatus[1][0] = 1;
		LightStatus[2][0] = 1;
		LightStatus[3][0] = 1;
		LightStatus[0][1] = 1;
		LightStatus[1][1] = 1;
		LightStatus[2][1] = 1;
		LightStatus[3][1] = 1;
		LightStatus[4][1] = 1;
		break;
	case 3: // ENG PB & C/W 2
			// Light engine START/STOP lights and Panel 1 second bank warning lamps
			// ENG PB lights are lit in the EngineStopButton & EngineStartButton code
		LightStatus[0][2] = 1;
		LightStatus[1][2] = 1;
		LightStatus[2][2] = 1;
		LightStatus[3][2] = 1;
		LightStatus[0][3] = 1;
		LightStatus[1][3] = 1;
		break;
	case 4: // C/W 3
			// Light Panel 2 first bank warning lamps;
		LightStatus[0][4] = 1;
		LightStatus[1][4] = 1;
		LightStatus[2][4] = 1;
		LightStatus[0][5] = 1;
		LightStatus[1][5] = 1;
		LightStatus[2][5] = 1;
		LightStatus[4][5] = 1;
		break;
	case 5: // C/W 4
			// Light Panel 2 second bank warning lamps;
		LightStatus[0][6] = 1;
		LightStatus[1][6] = 1;
		LightStatus[2][6] = 1;
		LightStatus[3][6] = 1;
		LightStatus[0][7] = 1;
		LightStatus[1][7] = 1;
		LightStatus[2][7] = 1;
		LightStatus[3][7] = 1;
		LightStatus[4][7] = 1;
		break;
	case 6: // COMPNT
			// Light component caution and Lunar Contact lights
			// Lunar Contact and Component lights are lit in clbkPanelRedrawEvent code
		break;
	}
}

void LEM_CWEA::SystemTimestep(double simdt) {

	if (IsCWEAPowered()) {
		cwea_pwr->DrawPower(11.48);
		CWEAHeat->GenerateHeat(5.74);
		SecCWEAHeat->GenerateHeat(5.74);
	}
	if (IsLTGPowered())
		lem->lca.DrawDCPower(GetDimmableLoad() + GetNonDimmableLoad());
	if (MasterAlarm == true)
		ma_pwr->DrawPower(7.2);

}

void LEM_CWEA::TurnOn()
{
	//Reset all the CWEA flip-flops when the CWEA is turned on

	if (!Operate)
	{
		DesRegWarnFF.HardReset();
		AGSWarnFF.HardReset();
		CESDCWarnFF.HardReset();
		CESACWarnFF.HardReset();
		RCSCautFF1.HardReset();
		RCSCautFF2.HardReset();
		RRHeaterCautFF.HardReset();
		SBDHeaterCautFF.HardReset();
		QD1HeaterCautFF.HardReset();
		QD2HeaterCautFF.HardReset();
		QD3HeaterCautFF.HardReset();
		QD4HeaterCautFF.HardReset();
		OxygenCautFF1.HardReset();
		OxygenCautFF2.HardReset();
		OxygenCautFF3.HardReset();
		WaterCautFF1.HardReset();
		WaterCautFF2.HardReset();
		WaterCautFF3.HardReset();
		RRCautFF.HardReset();
		SBDCautFF.HardReset();

		//Reset TCA FF's
		lem->tca1A.GetTCAFailureFlipFlop()->Reset();
		lem->tca1B.GetTCAFailureFlipFlop()->Reset();
		lem->tca2A.GetTCAFailureFlipFlop()->Reset();
		lem->tca2B.GetTCAFailureFlipFlop()->Reset();
		lem->tca3A.GetTCAFailureFlipFlop()->Reset();
		lem->tca3B.GetTCAFailureFlipFlop()->Reset();
		lem->tca4A.GetTCAFailureFlipFlop()->Reset();
		lem->tca4B.GetTCAFailureFlipFlop()->Reset();

		Operate = true;
	}
}

void LEM_CWEA::TurnOff()
{
	if (Operate)
	{
		//Nothing for now
		Operate = false;
	}
}

void LEM_CWEA::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "OPERATE", Operate);
	papiWriteScenario_bool(scn, "MASTERALARM", MasterAlarm);
	CESDCWarnFF.SaveState(scn, "CESDCWARNFF");
	CESACWarnFF.SaveState(scn, "CESACWARNFF");
	RRCautFF.SaveState(scn, "RRCAUTFF");
	DesRegWarnFF.SaveState(scn, "DESREGWARNFF");
	AGSWarnFF.SaveState(scn, "AGSWARNFF");
	RCSCautFF1.SaveState(scn, "RCSCAUTFF1");
	RCSCautFF2.SaveState(scn, "RCSCAUTFF2");
	RRHeaterCautFF.SaveState(scn, "RRHEATERCAUTFF");
	SBDHeaterCautFF.SaveState(scn, "SBDHEATERCAUTFF");
	QD1HeaterCautFF.SaveState(scn, "QD1HEATERCAUTFF");
	QD2HeaterCautFF.SaveState(scn, "QD2HEATERCAUTFF");
	QD3HeaterCautFF.SaveState(scn, "QD3HEATERCAUTFF");
	QD4HeaterCautFF.SaveState(scn, "QD4HEATERCAUTFF");
	SBDCautFF.SaveState(scn, "SBDCAUTFF");
	WaterCautFF1.SaveState(scn, "WATERCAUTFF1");
	WaterCautFF2.SaveState(scn, "WATERCAUTFF2");
	WaterCautFF3.SaveState(scn, "WATERCAUTFF3");
	OxygenCautFF1.SaveState(scn, "OXYGENCAUTFF1");
	OxygenCautFF2.SaveState(scn, "OXYGENCAUTFF2");
	OxygenCautFF3.SaveState(scn, "OXYGENCAUTFF3");
	papiWriteScenario_intarr(scn, "LIGHTSTATUS0", &LightStatus[0][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS1", &LightStatus[1][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS2", &LightStatus[2][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS3", &LightStatus[3][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS4", &LightStatus[4][0], 8);
	oapiWriteScenario_int(scn, "ECSFAILURECOUNT", ECSFailureCount);

	oapiWriteLine(scn, end_str);
}

void LEM_CWEA::LoadState(FILEHANDLE scn, char *end_str)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str))) {
			return;
		}

		if (!strnicmp(line, "CESDCWARNFF", 11)) {
			CESDCWarnFF.LoadState(line, 11);
		}
		else if (!strnicmp(line, "CESACWARNFF", 11)) {
			CESACWarnFF.LoadState(line, 11);
		}
		else if (!strnicmp(line, "RRCAUTFF", 8)) {
			RRCautFF.LoadState(line, 8);
		}
		else if (!strnicmp(line, "DESREGWARNFF", 12)) {
			DesRegWarnFF.LoadState(line, 12);
		}
		else if (!strnicmp(line, "AGSWARNFF", 9)) {
			AGSWarnFF.LoadState(line, 9);
		}
		else if (!strnicmp(line, "RCSCAUTFF1", 10)) {
			RCSCautFF1.LoadState(line, 10);
		}
		else if (!strnicmp(line, "RCSCAUTFF2", 10)) {
			RCSCautFF2.LoadState(line, 10);
		}
		else if (!strnicmp(line, "RRHEATERCAUTFF", 14)) {
			RRHeaterCautFF.LoadState(line, 14);
		}
		else if (!strnicmp(line, "SBDHEATERCAUTFF", 15)) {
			SBDHeaterCautFF.LoadState(line, 15);
		}
		else if (!strnicmp(line, "QD1HEATERCAUTFF", 15)) {
			QD1HeaterCautFF.LoadState(line, 15);
		}
		else if (!strnicmp(line, "QD2HEATERCAUTFF", 15)) {
			QD2HeaterCautFF.LoadState(line, 15);
		}
		else if (!strnicmp(line, "QD3HEATERCAUTFF", 15)) {
			QD3HeaterCautFF.LoadState(line, 15);
		}
		else if (!strnicmp(line, "QD4HEATERCAUTFF", 15)) {
			QD4HeaterCautFF.LoadState(line, 15);
		}
		else if (!strnicmp(line, "SBDCAUTFF", 9)) {
			SBDCautFF.LoadState(line, 9);
		}
		else if (!strnicmp(line, "WATERCAUTFF1", 12)) {
			WaterCautFF1.LoadState(line, 12);
		}
		else if (!strnicmp(line, "WATERCAUTFF2", 12)) {
			WaterCautFF2.LoadState(line, 12);
		}
		else if (!strnicmp(line, "WATERCAUTFF3", 12)) {
			WaterCautFF3.LoadState(line, 12);
		}
		else if (!strnicmp(line, "OXYGENCAUTFF1", 13)) {
			OxygenCautFF1.LoadState(line, 13);
		}
		else if (!strnicmp(line, "OXYGENCAUTFF2", 13)) {
			OxygenCautFF2.LoadState(line, 13);
		}
		else if (!strnicmp(line, "OXYGENCAUTFF3", 13)) {
			OxygenCautFF3.LoadState(line, 13);
		}

		papiReadScenario_bool(line, "OPERATE", Operate);
		papiReadScenario_bool(line, "MASTERALARM", MasterAlarm);
		papiReadScenario_intarr(line, "LIGHTSTATUS0", &LightStatus[0][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS1", &LightStatus[1][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS2", &LightStatus[2][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS3", &LightStatus[3][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS4", &LightStatus[4][0], 8);
		papiReadScenario_int(line, "ECSFAILURECOUNT", ECSFailureCount);
	}
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
			if (LightStatus[row][col] == 1 && IsLTGPowered()) {
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
			if (row == 3 && col == 2)
			{
				//Condition for C/W PWR light
				if (LightStatus[row][col + 4] == 1 && IsCWPWRLTGPowered()) {
					dy = 134;
				}
				else {
					dy = 7;
				}
			}
			else
			{
				//All others
				if (LightStatus[row][col + 4] == 1 && IsLTGPowered()) {
					dy = 134;
				}
				else {
					dy = 7;
				}
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
}

void LEM_CWEA::PushMasterAlarm()

{
	if (IsMAPowered()) {
		MasterAlarmSound.stop();
		SetMasterAlarm(false);
	}
	ButtonSound.play(NOLOOP, 255);
}

bool LEM_CWEA::CheckMasterAlarmMouseClick(int event) {
	if (event & PANEL_MOUSE_LBDOWN) {
		PushMasterAlarm();
	}
	return true;
}

void LEM_CWEA::SetLight(int row, int column, int state, bool TriggerMA)
{
	//
	// Turn on Master Alarm if a new light is lit.
	//

	//For debugging
	//if (state == 0) return;

	if (state && !LightStatus[row][column]) {
		if (TriggerMA) {
			SetMasterAlarm(true);
		}
	}
	LightStatus[row][column] = state;
}

void LEM_CWEA::SetLightStates(int state)
{
	for (int i = 0;i < 5;i++)
	{
		for (int j = 0;j < 8;j++)
		{
			LightStatus[i][j] = state;
		}
	}
}

double LEM_CWEA::GetCWBank1Lights()	
{
	int counter = 0;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (LightStatus[i][j] == 1)
			{
				counter++;
			}
		}
	}
	return counter;
}

double LEM_CWEA::GetCWBank2Lights()
{
	int counter = 0;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 2; j < 4; j++)
		{
			if (LightStatus[i][j] == 1)
			{
				counter++;
			}
		}
	}
	return counter;
}

double LEM_CWEA::GetCWBank3Lights()
{
	int counter = 0;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 4; j < 6; j++)
		{
			if (LightStatus[i][j] == 1)
			{
				counter++;
			}
		}
	}
	return counter;
}

double LEM_CWEA::GetCWBank4Lights()
{
	int counter = 0;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 6; j < 8; j++)
		{
			if (LightStatus[i][j] == 1)
			{
				counter++;
			}
		}
	}
	if (LightStatus[3][6] == 1)
	{
		counter = counter - 1;
	}
	return counter;
}

double LEM_CWEA::GetNumberLightsOn()	//Counts number of CW lights lit minus the cw power light
{
	return GetCWBank1Lights() + GetCWBank2Lights() + GetCWBank3Lights() + GetCWBank4Lights();
}

double LEM_CWEA::GetNonDimmableLoad()	//Returns bulb draw if the CW power light is lit or a lamp test is active
{
	if (LightStatus[3][6] == 1 && lem->LampToneTestRotary != 5) {
		return 1.18;
	}
	
	if (lem->LampToneTestRotary == 2) {
		return GetCWBank1Lights() * 1.18;
	}
	else if (lem->LampToneTestRotary == 3) {
		return GetCWBank2Lights() * 1.18;
	}
	else if (lem->LampToneTestRotary == 4) {
		return GetCWBank3Lights() * 1.18;
	}
	else if (lem->LampToneTestRotary == 5) {
		return GetCWBank4Lights() * 1.18;
	}
	return 0.0;
}

double LEM_CWEA::GetDimmableLoad()
{
	if (lem->LampToneTestRotary == 0 || lem->LampToneTestRotary == 1 || lem->LampToneTestRotary == 6 || lem->LampToneTestRotary == 7) {
		return (GetNumberLightsOn() * 1.18) * lem->lca.GetAnnunDimPct();	//Approx 1.18W per bulb, scaled with LCA dimming
	}
	else if (lem->LampToneTestRotary == 2) {
		return ((GetCWBank2Lights() + GetCWBank3Lights() + GetCWBank4Lights()) * 1.18) * lem->lca.GetAnnunDimPct();
	}
	else if (lem->LampToneTestRotary == 3) {
		return ((GetCWBank1Lights() + GetCWBank3Lights() + GetCWBank4Lights()) * 1.18) * lem->lca.GetAnnunDimPct();
	}
	else if (lem->LampToneTestRotary == 4) {
		return ((GetCWBank1Lights() + GetCWBank2Lights() + GetCWBank4Lights()) * 1.18) * lem->lca.GetAnnunDimPct();
	}
	else if (lem->LampToneTestRotary == 5) {
		return ((GetCWBank1Lights() + GetCWBank2Lights() + GetCWBank3Lights()) * 1.18) * lem->lca.GetAnnunDimPct();
	}
	else
		return 0.0;
}