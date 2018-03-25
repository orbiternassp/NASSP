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
	ltg_pwr = NULL;
	lem = NULL;

	s.LoadSound(MasterAlarmSound, LM_MASTERALARM_SOUND);
	MasterAlarm = false;
	Operate = false;

	//Initialize all FF's as "reset"
	DesRegWarnFF = 0;
	AGSWarnFF = 0;
	CESDCWarnFF = 0;
	CESACWarnFF = 0;
	RCSCautFF1 = 0; RCSCautFF2 = 0;
	RRHeaterCautFF = 0; SBDHeaterCautFF = 0;
	OxygenCautFF1 = 0; OxygenCautFF2 = 0; OxygenCautFF3 = 0;
	WaterCautFF1 = 0; WaterCautFF2 = 0; WaterCautFF3 = 0;
	RRCautFF = 0;
	SBDCautFF = 0;
}

void LEM_CWEA::Init(LEM *l, e_object *cwea, e_object *ma, e_object *ltg ) {
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
	ltg_pwr = ltg;
	lem = l;
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
	if (ltg_pwr->Voltage() > SP_MIN_DCVOLTAGE || lem->CDR_LTG_ANUN_DOCK_COMPNT_CB.Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

void LEM_CWEA::SetMasterAlarm(bool alarm) {
	if (IsMAPowered())
	MasterAlarm = alarm;
}

void LEM_CWEA::TimeStep(double simdt) {
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
		// Pressure of either ascent helium tanks below 2773 psia prior to staging
		// Disabled when stage deadface opens.
		if (lem->stage < 2) {
			if (lem->GetAPSPropellant()->GetAscentHelium1PressPSI() < 2772.8 || lem->GetAPSPropellant()->GetAscentHelium2PressPSI() < 2773.0) {
				SetLight(1, 0, 1);
			}
		}
		else
			SetLight(1, 0, 0);

		// 6DS3 DES HI/LO HELIUM REG OUTLET PRESS
		// Enabled by DES ENG "ON" command. Disabled by stage deadface open.
		// Pressure in descent helium lines downstream of the regulators is above 259.1 psia or below 219.2 psia.
		lightlogic = false;
		if (lem->scera1.GetVoltage(3, 9) > 2.5 || lem->eds.GetHeliumPressDelayContactClosed()) { DesRegWarnFF = 1; }
		if (lem->stage < 2) {
			if (lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() > 259.1 || lem->DPSPropellant.GetHeliumRegulatorManifoldPressurePSI() < 219.2) { lightlogic = true; } //Pressure is default at 245 so this will not be true until He MP is simulated
		}
		if (lightlogic && DesRegWarnFF == 1) {
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
		if (lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER) { CESACWarnFF = 0; }
		if (lem->SCS_ATCA_CB.Voltage() < 24.0) { CESACWarnFF = 1; }

		if (CESACWarnFF == 1)
			SetLight(0, 1, 1);
		else
			SetLight(0, 1, 0);

		// 6DS7 CES DC VOLTAGE FAILURE
		// Any CES DC voltage out of tolerance.
		// All of these are provided by the ATCA main power supply.
		// Disabled by Gyro Test Control in POS RT or NEG RT position.
		// Needs RGA data
		if (lem->GyroTestRightSwitch.GetState() != THREEPOSSWITCH_CENTER) { CESDCWarnFF = 0; }
		if (lem->SCS_ATCA_CB.Voltage() < 24.0) { CESDCWarnFF = 1; }

		if (CESDCWarnFF == 1)
			SetLight(1, 1, 1);
		else
			SetLight(1, 1, 0);

		// 6DS8 AGS FAILURE
		// On when any ASA power supply signals a failure, when AGS raises failure signal, or ASA overtemp.
		// Disabled when AGS status switch is OFF.
		lightlogic = false;
		if (lem->QtyMonRotary.GetState() == 0) { AGSWarnFF = 0; }
		if (lem->scera1.GetVoltage(4, 1) > 2.5 && lem->AGSOperateSwitch.GetState() != THREEPOSSWITCH_DOWN) { AGSWarnFF = 1; } // AEA Test Mode Fail

		if (lem->AGSOperateSwitch.GetState() != THREEPOSSWITCH_DOWN) {
			if (lem->scera1.GetVoltage(15, 4) > (13.2 / 2.8) || lem->scera1.GetVoltage(15, 4) < (10.8 / 2.8)) { lightlogic = true; } // ASA +12VDC **Open circuit by overtemp condition**
			if (lem->scera2.GetVoltage(15, 3) > (30.8 / 8.0) || lem->scera2.GetVoltage(15, 3) < (25.2 / 8.0)) { lightlogic = true; } // ASA +28VDC  
			if (lem->scera1.GetVoltage(16, 2) > ((415.0 - 380.0) / 8.0) || lem->scera1.GetVoltage(16, 2) < ((385.0 - 380.0) / 8.0)) { lightlogic = true; } // ASA Freq
		}

		if (lightlogic || AGSWarnFF == 1)
			SetLight(2, 1, 1);
		else
			SetLight(2, 1, 0);

		// 6DS9 LGC FAILURE
		// On when any LGC power supply signals a failure, scaler fails, LGC restarts, counter fails, or LGC raises failure signal.
		// Disabled by Guidance Control switch in AGS position.
		if (lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP && lem->scera2.GetVoltage(3, 11) > 2.5)
			SetLight(3, 1, 1);
		else
			SetLight(3, 1, 0);

		// 6DS10 ISS FAILURE
		// On when ISS power supply fails, PIPA fails while main engine thrusting, gimbal servo fails, CDU fails.
		// Disabled by Guidance Control switch in AGS position.
		if (lem->GuidContSwitch.GetState() == TOGGLESWITCH_UP && lem->scera2.GetVoltage(3, 12) > 2.5)
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
		if (lem->APS.thrustOn) {
			if (lem->scera2.GetVoltage(2, 6) > 2.5 || lem->scera2.GetVoltage(2, 7) > 2.5) {
				SetLight(1, 4, 1);
			}
		}
		else
			SetLight(1, 4, 0);

		// 6DS23 AUTOMATIC GIMBAL FAILURE CAUTION
		// On when difference in commanded and actual descent engine trim position is detected.
		// Enabled when descent engine armed and engine gimbal switch is enabled.
		// Disabled by stage deadface open.
		if (lem->stage < 2) {
			if (lem->scera2.GetVoltage(3, 9) > 2.5 || lem->scera2.GetVoltage(3, 10) > 2.5) {
				SetLight(2, 4, 1);
			}
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
		// FIX ME!
		bool AutoTrackChanged;
		if (RRCautFF == 0) { AutoTrackChanged = 0; }
		if (lem->RendezvousRadarRotary.GetState() != 0 && AutoTrackChanged == 0) { AutoTrackChanged = 1; }

		if (lem->RendezvousRadarRotary.GetState() == 0 && lem->scera2.GetVoltage(2, 1) > 2.5 && AutoTrackChanged == 1) { RRCautFF = 0; }
		if (RRCautFF == 0 && lem->scera2.GetVoltage(2, 1) < 2.5 && lem->RendezvousRadarRotary.GetState() == 0) { RRCautFF = 1; }

		if (RRCautFF == 1 && lem->scera2.GetVoltage(2, 1) > 2.5 && lem->RendezvousRadarRotary.GetState() == 0) {
			SetLight(2, 5, 1);
		}
		else
			SetLight(2, 5, 0);

		sprintf(oapiDebugString(), "RRC %i ATC %i SCV %lf", RRCautFF, AutoTrackChanged, lem->scera2.GetVoltage(2, 1));

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
		if (!(lem->EDMasterArm.IsUp() || lem->AbortStageSwitch.GetState() == 0)) {
			if (lem->scera1.GetVoltage(14, 11) > 2.5 || lem->scera1.GetVoltage(14, 12) > 2.5)
				SetLight(0, 6, 1);
		}
		else
			SetLight(0, 6, 0);

		// 6DS32 RCS FAILURE CAUTION
		// On when helium pressure in either RCS system below 1700 psia.
		// Disabled when RCS TEMP/PRESS MONITOR switch in HELIUM position.
		if (lem->TempPressMonRotary.GetState() == 0) {
			RCSCautFF1 = 0;
			RCSCautFF2 = 0;
		}
		if (lem->scera1.GetVoltage(6, 1) < (1696.1 / 700.0)) { RCSCautFF1 = 1; }
		if (lem->scera1.GetVoltage(6, 2) < (1696.1 / 700.0)) { RCSCautFF2 = 1; }

		if (RCSCautFF1 == 1 || RCSCautFF2 == 1)
			SetLight(1, 6, 1);
		else
			SetLight(1, 6, 0);

		// 6DS33 HEATER FAILURE CAUTION
		// On when:
		// S-Band Antenna Electronic Drive Assembly < -64.08F or > 152.63F
		// RR Assembly < -54.07F or > 147.69F
		// Quad temps and LR temp do not turn the light on
		// Disabled when Temperature Monitor switch selects affected assembly.
		if (lem->TempMonitorRotary.GetState() == 0) { RRHeaterCautFF = 0; }
		if (lem->TempMonitorRotary.GetState() == 6) { SBDHeaterCautFF = 0; }

		if (lem->scera1.GetVoltage(21, 4) < ((-54.07 + 200.0) / 80.0) || lem->scera1.GetVoltage(21, 4) > ((147.69 + 200.0) / 80.0)) { RRHeaterCautFF = 1; }
		if (lem->scera2.GetVoltage(21, 1) < ((-64.08 + 200.0) / 80.0) || lem->scera2.GetVoltage(21, 1) > ((153.63 + 200.0) / 80.0)) { SBDHeaterCautFF = 1; }

		if (RRHeaterCautFF == 1 || SBDHeaterCautFF == 1)
			SetLight(2, 6, 1);
		else
			SetLight(2, 6, 0);

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
		if (lem->ECS_CO2_SENSOR_CB.IsPowered() && lem->scera1.GetVoltage(5, 2) >= (7.6 / 6.0)) { lightlogic = true; }	// CO2 Partial Pressure > 7.6mm
		if (lem->scera2.GetVoltage(13, 3) > 2.5) { lightlogic = true; } // Glycol pump failure
		if (lem->SuitFanRotary.GetState() == 1 && lem->scera2.GetVoltage(3, 5) > 2.5) { lightlogic = true; } // Suit fan 1 failure
		if (lem->scera1.GetVoltage(5, 3) < (792.5 / 720.0)) { lightlogic = true; } // Water separator failure

		if (lightlogic)
			SetLight(0, 7, 1);
		else
			SetLight(0, 7, 0);

		// 6DS37 OXYGEN QUANTITY CAUTION
		// On when:
		// < 135 psia in descent oxygen tank, or Less than full (<682.4 / 681.6 psia) ascent oxygen tanks, WHEN NOT STAGED
		// Less than 99.6 psia in ascent oxygen tank #1
		// Off by positioning O2/H20 QTY MON switch to CWEA RESET position.
		if (lem->QtyMonRotary.GetState() == 0) {
			OxygenCautFF1 = 0;
			OxygenCautFF2 = 0;
			OxygenCautFF3 = 0;
		}
		if (lem->stage < 2) {
			if (lem->scera1.GetVoltage(7, 1) < (681.6 / 200.0) || lem->scera1.GetVoltage(7, 2) < (682.4 / 200.0)) { OxygenCautFF1 = 1; } // Unstaged less than full ASC tanks
			if (lem->scera2.GetVoltage(8, 2) < (135.0 / 600.0)) { OxygenCautFF2 = 1; } // Unstaged low DES tank
		}
		if (lem->scera1.GetVoltage(7, 1) < (99.6 / 200.0)) { OxygenCautFF3 = 1; } // Low ASC tank 1

		if (OxygenCautFF1 == 1 || OxygenCautFF2 == 1 || OxygenCautFF3 == 1)
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
		if (lem->QtyMonRotary.GetState() == 0) {
			WaterCautFF1 = 0;
			WaterCautFF2 = 0;
			WaterCautFF3 = 0;
		}
		if (lem->stage < 2) {
			if (lem->scera1.GetVoltage(7, 3) < (0.1594 / 0.2)) { WaterCautFF1 = 1; } // Unstaged, DES tank < 15.94%
			if (lem->scera1.GetVoltage(8, 1) < (0.9478 / 0.2) || lem->scera1.GetVoltage(8, 2) < (0.9478 / 0.2)) { WaterCautFF2 = 1; } // Unstaged, ASC tank < 94.78%
		}
		if ((abs(lem->scera1.GetVoltage(8, 1) - lem->scera1.GetVoltage(8, 2)) / ((lem->scera1.GetVoltage(8, 1) + lem->scera1.GetVoltage(8, 2)) / 2.0)) >= 0.15) { WaterCautFF3 = 1; } // Staged ASC tank unbalance

		if (WaterCautFF1 == 1 || WaterCautFF2 == 1 || WaterCautFF3 == 1)
			SetLight(3, 7, 1);
		else
			SetLight(3, 7, 0);

		// 6DS40 S-BAND RECEIVER FAILURE CAUTION
		// On when reciever signal lost.
		// Off when Range/TV function switch to OFF/RESET
		// Disabled when Range/TV switch is not in TV/CWEA ENABLE position
		if (lem->SBandRangeSwitch.GetState() == THREEPOSSWITCH_CENTER) { SBDCautFF = 0; }
		if (lem->scera1.GetVoltage(5, 4) < 1.071) { SBDCautFF = 1; }

		if (lem->SBandRangeSwitch.GetState() == THREEPOSSWITCH_DOWN && SBDCautFF == 1)
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
		SetColumnLightStates(0, 1);
		SetColumnLightStates(1, 1);
		break;
	case 3: // ENG PB & C/W 2
			// Light engine START/STOP lights and Panel 1 second bank warning lamps
			// FIX ME: Lit engine start/stop light bmp and logic needed
		SetColumnLightStates(2, 1);
		SetColumnLightStates(3, 1);
		break;
	case 4: // C/W 3
			// Light Panel 2 first bank warning lamps;
		
		SetColumnLightStates(4, 1);
		SetColumnLightStates(5, 1);
		
		//hack
		LightStatus[3][5] = 2;
		break;
	case 5: // C/W 4
			// Light Panel 2 second bank warning lamps;
		SetColumnLightStates(6, 1);
		SetColumnLightStates(7, 1);
		break;
	case 6: // COMPNT
			// Light component caution and Lunar Contact lights
			// Lunar Contact and Component lights are lit in clbkPanelRedrawEvent code
		break;
	}

	//sprintf(oapiDebugString(), "MA %i AGS %i DC %i AC %i RCS1 %i RCS2 %i RRH %i SBH %i RRC %i O21 %i O22 %i O23 %i W1 %i W2 %i W3 %i SBD %i", MasterAlarm, AGSWarnFF, CESDCWarnFF, CESACWarnFF, RCSCautFF1, RCSCautFF2, RRHeaterCautFF, SBDHeaterCautFF, RRCautFF, OxygenCautFF1, OxygenCautFF2, OxygenCautFF3, WaterCautFF1, WaterCautFF2, WaterCautFF3, SBDCautFF);
}

void LEM_CWEA::SystemTimestep(double simdt) {

	if (IsCWEAPowered()) {
		cwea_pwr->DrawPower(11.48);
	}
	if (MasterAlarm == true)
		ma_pwr->DrawPower(7.2);

}

void LEM_CWEA::TurnOn()
{
	//Reset all the CWEA flip-flops when the CWEA is turned on

	if (!Operate)
	{
		DesRegWarnFF = 0;
		AGSWarnFF = 0;
		CESDCWarnFF = 0;
		CESACWarnFF = 0;
		RCSCautFF1 = 0; RCSCautFF2 = 0;
		RRHeaterCautFF = 0; SBDHeaterCautFF = 0;
		OxygenCautFF1 = 0; OxygenCautFF2 = 0; OxygenCautFF3 = 0;
		WaterCautFF1 = 0; WaterCautFF2 = 0; WaterCautFF3 = 0;
		RRCautFF = 0;
		SBDCautFF = 0;

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
	papiWriteScenario_bool(scn, "DESREGWARNFF", DesRegWarnFF);
	papiWriteScenario_bool(scn, "AGSWARNFF", AGSWarnFF);
	papiWriteScenario_bool(scn, "CESDCWARNFF", CESDCWarnFF);
	papiWriteScenario_bool(scn, "CESACWARNFF", CESACWarnFF);
	papiWriteScenario_bool(scn, "RCSCAUTFF1", RCSCautFF1);
	papiWriteScenario_bool(scn, "RCSCAUTFF2", RCSCautFF2);
	papiWriteScenario_bool(scn, "RRHEATERCAUTFF", RRHeaterCautFF);
	papiWriteScenario_bool(scn, "SBDHEATERCAUTFF", SBDHeaterCautFF);
	papiWriteScenario_bool(scn, "OXYGENCAUTFF1", OxygenCautFF1);
	papiWriteScenario_bool(scn, "OXYGENCAUTFF2", OxygenCautFF2);
	papiWriteScenario_bool(scn, "OXYGENCAUTFF3", OxygenCautFF3);
	papiWriteScenario_bool(scn, "WATERCAUTFF1", WaterCautFF1);
	papiWriteScenario_bool(scn, "WATERCAUTFF2", WaterCautFF2);
	papiWriteScenario_bool(scn, "WATERCAUTFF3", WaterCautFF3);
	papiWriteScenario_bool(scn, "RRCAUTFF", RRCautFF);
	papiWriteScenario_bool(scn, "SBDCAUTFF", SBDCautFF);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS0", &LightStatus[0][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS1", &LightStatus[1][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS2", &LightStatus[2][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS3", &LightStatus[3][0], 8);
	papiWriteScenario_intarr(scn, "LIGHTSTATUS4", &LightStatus[4][0], 8);

	oapiWriteLine(scn, end_str);
}

void LEM_CWEA::LoadState(FILEHANDLE scn, char *end_str)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, sizeof(end_str))) {
			return;
		}

		papiReadScenario_bool(line, "OPERATE", Operate);
		papiReadScenario_bool(line, "MASTERALARM", MasterAlarm);
		papiReadScenario_bool(line, "DESREGWARNFF", DesRegWarnFF);
		papiReadScenario_bool(line, "AGSWARNFF", AGSWarnFF);
		papiReadScenario_bool(line, "CESDCWARNFF", CESDCWarnFF);
		papiReadScenario_bool(line, "CESACWARNFF", CESACWarnFF);
		papiReadScenario_bool(line, "RCSCAUTFF1", RCSCautFF1);
		papiReadScenario_bool(line, "RCSCAUTFF2", RCSCautFF2);
		papiReadScenario_bool(line, "RRHEATERCAUTFF", RRHeaterCautFF);
		papiReadScenario_bool(line, "SBDHEATERCAUTFF", SBDHeaterCautFF);
		papiReadScenario_bool(line, "OXYGENCAUTFF1", OxygenCautFF1);
		papiReadScenario_bool(line, "OXYGENCAUTFF2", OxygenCautFF2);
		papiReadScenario_bool(line, "OXYGENCAUTFF3", OxygenCautFF3);
		papiReadScenario_bool(line, "WATERCAUTFF1", WaterCautFF1);
		papiReadScenario_bool(line, "WATERCAUTFF2", WaterCautFF2);
		papiReadScenario_bool(line, "WATERCAUTFF3", WaterCautFF3);
		papiReadScenario_bool(line, "RRCAUTFF", RRCautFF);
		papiReadScenario_bool(line, "SBDCAUTFF", SBDCautFF);
		papiReadScenario_intarr(line, "LIGHTSTATUS0", &LightStatus[0][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS1", &LightStatus[1][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS2", &LightStatus[2][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS3", &LightStatus[3][0], 8);
		papiReadScenario_intarr(line, "LIGHTSTATUS4", &LightStatus[4][0], 8);
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
			if (LightStatus[row][col + 4] == 1 && IsLTGPowered()) {
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

void LEM_CWEA::SetColumnLightStates(int col, int state)
{
	for (int i = 0;i < 5;i++)
	{
		LightStatus[i][col] = state;
	}
}