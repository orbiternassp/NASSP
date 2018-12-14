/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

Lunar Module Rendezvous Radar

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
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "lm_channels.h"
#include "papi.h"
#include "LEM.h"
#include "lm_rr.h"

#define RR_SHAFT_STEP 0.000191747598876953125 
#define RR_TRUNNION_STEP 0.00004793689959716796875

// Rendezvous Radar
// Position and draw numbers are just guesses!
LEM_RR::LEM_RR()
{
	lem = NULL;
	RREHeat = 0;
	RRESECHeat = 0;
	NoTrackSignal = false;
	radarDataGood = false;
}

void LEM_RR::Init(LEM *s, e_object *dc_src, e_object *ac_src, h_Radiator *ant, Boiler *anheat, Boiler *stbyanheat, h_HeatLoad *rreh, h_HeatLoad *secrreh, h_HeatLoad *rrh) {
	lem = s;
	// Set up antenna.
	// RR antenna is designed to operate between 10F and 75F
	// The standby heater switches on below -40F and turns it off again at 0F
	// The oprational heater switches on below 0F and turns it off again at 20F
	//The RR assembly has multiple heater systems within, we will only concern ourselves with the antenna itself
	// The CWEA complains if the temperature is outside of -54F to +148F
	// Values in the constructor are name, pos, vol, isol
	// The DC side of the RR is most of it, the AC provides the transmit source.
	antenna = ant;
	stbyantheater = stbyanheat;
	antheater = anheat;
	rrheat = rrh;
	RREHeat = rreh;
	RRESECHeat = secrreh;
	antenna->isolation = 0.000001;
	antenna->Area = 9187.8912; // Area of reflecting dish, probably good enough
	trunnionAngle = -180.0 * RAD;
	shaftAngle = 0.0 * RAD; // Stow
	dc_source = dc_src;
	ac_source = ac_src;
	mode = 2;
	if (lem != NULL) {
		stbyantheater->WireTo(&lem->HTR_RR_STBY_CB);
		antheater->WireTo(&lem->HTR_RR_OPR_CB);
	}

	hpbw_factor = acos(sqrt(sqrt(0.5))) / (3.5*RAD / 4.0);	//3.5° beamwidth
	SignalStrength = 0.0;
	AutoTrackEnabled = false;
	ShaftErrorSignal = 0.0;
	TrunnionErrorSignal = 0.0;
	GyroRates = _V(0.0, 0.0, 0.0);
	FrequencyLock = false;
	TrackingModeSwitch = false;
	RangeLock = true;
	RangeLockTimer = 0.0;
	tstime = 0.0;

	for (int i = 0;i < 4;i++)
	{
		SignalStrengthQuadrant[i] = 0.0;
		U_RRL[i] = _V(0.0, 0.0, 0.0);
	}
}

bool LEM_RR::IsDCPowered()

{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE) {
		return false;
	}
	return true;
}

bool LEM_RR::IsACPowered()

{
	if (ac_source->Voltage() > 100) {
		return true;
	}
	return false;
}

bool LEM_RR::IsPowered()

{
	if (IsDCPowered() && IsACPowered()) {
		return true;
	}
	return false;
}

double LEM_RR::GetShaftErrorSignal()
{
	if (!IsPowered() || !AutoTrackEnabled)
	{
		return 0;
	}

	return 2.5 + ShaftErrorSignal * 40.0;
}

double LEM_RR::GetTrunnionErrorSignal()
{
	if (!IsPowered() || !AutoTrackEnabled)
	{
		return 0;
	}

	return 2.5 + TrunnionErrorSignal * 40.0;
}

double LEM_RR::GetTransmitterPower()
{
	if (!IsPowered())
	{
		return 0;
	}

	return 3.7;
}

void LEM_RR::Timestep(double simdt) {

	lem->rr_proc[0] = shaftAngle / PI2;
	if (lem->rr_proc[0] < 0) lem->rr_proc[0] += 1.0;
	lem->rr_proc[1] = trunnionAngle / PI2;
	if (lem->rr_proc[0] < 0) lem->rr_proc[0] += 1.0;
	lem->SetAnimation(lem->anim_RRPitch, lem->rr_proc[0]);
	lem->SetAnimation(lem->anim_RRYaw, lem->rr_proc[1]);

	ChannelValue val12;
	ChannelValue val13;
	ChannelValue val14;
	ChannelValue val30;
	ChannelValue val33;
	val12 = lem->agc.GetInputChannel(012);
	val13 = lem->agc.GetInputChannel(013);
	val14 = lem->agc.GetInputChannel(014);
	val30 = lem->agc.GetInputChannel(030);
	val33 = lem->agc.GetInputChannel(033);

	double ShaftRate = 0;
	double TrunRate = 0;
	trunnionVel = 0;
	shaftVel = 0;

	/*
	This is backwards?
	if(val30.Bits.RRCDUFailure != 1){
	val30.Bits.RRCDUFailure = 1; // No failure
	lem->agc.SetInputChannel(030,val30.Value);
	sprintf(oapiDebugString(),"RR CDU Failure Flag Cleared");
	}
	*/

	//NO TRACK RELAY
	if (IsDCPowered() && !radarDataGood)
	{
		NoTrackSignal = true;
	}
	else
	{
		NoTrackSignal = false;
	}

	if (!IsPowered()) {
		bool clobber = FALSE;
		if (val33[RRPowerOnAuto]) { clobber = TRUE; val33[RRPowerOnAuto] = 0; }
		if (val33[RRDataGood]) { clobber = TRUE; val33[RRDataGood] = 0; }
		if (clobber == TRUE) { lem->agc.SetInputChannel(033, val33); }
		SignalStrength = 0.0;
		radarDataGood = false;
		FrequencyLock = false;
		RangeLock = false;
		range = 0.0;
		rate = 0.0;
		if (val13[RadarActivity] == 1) {
			int radarBits = 0;
			if (val13[RadarA] == 1) { radarBits |= 1; }
			if (val13[RadarB] == 1) { radarBits |= 2; }
			if (val13[RadarC] == 1) { radarBits |= 4; }
			switch (radarBits) {
			case 2:
			case 4:
				lem->agc.SetInputChannelBit(013, RadarActivity, 0);
				lem->agc.GenerateRadarupt();
				break;
			}
		}
		return;
	}

	// Determine slew rate
	switch (lem->SlewRateSwitch.GetState()) {
	case TOGGLESWITCH_UP:       // HI
		ShaftRate = 7.0*RAD;
		TrunRate = 7.0*RAD;
		break;
	case TOGGLESWITCH_DOWN:     // LOW
		ShaftRate = 1.33*RAD;
		TrunRate = 1.33*RAD;
		break;
	}

	//Gyro rates
	lem->GetAngularVel(GyroRates);

	// If we are in test mode...
	if (lem->RadarTestSwitch.GetState() == THREEPOSSWITCH_UP) {
		//double trunnionTarget = 0, shaftTarget = 0;
		// TEST MODE:
		// NO TRACK light on
		// Range Rate to -500 FPS,
		// Shaft/Trunnion varies between +/- 5 degrees (at 0.015 d/s)
		// After 12 seconds, Range to 195.5nm and NO TRACK light out
		internalrangerate = -152.4;
		internalrange = 362066; // 195.5 nautical miles in meters

		//Square wave
		tstime += simdt;
		if (tstime > 2.0)
		{
			tstime -= 2.0;
		}
		else if (tstime > 1.0)
		{
			SignalStrengthQuadrant[0] = 0.41;
			SignalStrengthQuadrant[1] = 0.45;
			SignalStrengthQuadrant[2] = 0.41;
			SignalStrengthQuadrant[3] = 0.45;
		}
		else
		{
			SignalStrengthQuadrant[0] = 0.45;
			SignalStrengthQuadrant[1] = 0.41;
			SignalStrengthQuadrant[2] = 0.45;
			SignalStrengthQuadrant[3] = 0.41;
		}

		SignalStrength = (SignalStrengthQuadrant[0] + SignalStrengthQuadrant[1] + SignalStrengthQuadrant[2] + SignalStrengthQuadrant[3]) / 4.0;

		//sprintf(oapiDebugString(),"RR TEST MODE TIMER %0.2f STATE T/S %d %d POS %0.2f %0.2f TPOS %0.2f %0.2f",tstime,tstate[0],tstate[1],shaftAngle*DEG,trunnionAngle*DEG,shaftTarget*DEG,trunnionTarget*DEG);
	}
	else {
		// Clobber test data if not already zero
		if (tstime > 0) { tstime = 0; }
		// We must be in normal operation.
		internalrange = 0;
		internalrangerate = 0;
		SignalStrengthQuadrant[0] = 0.0;
		SignalStrengthQuadrant[1] = 0.0;
		SignalStrengthQuadrant[2] = 0.0;
		SignalStrengthQuadrant[3] = 0.0;
		SignalStrength = 0.0;

		VECTOR3 CSMPos, CSMVel, LMPos, LMVel, U_R, U_RR, R;
		MATRIX3 Rot;
		double relang;

		double anginc = 0.1*RAD;

		VESSEL *csm = lem->agc.GetCSM();

		if (csm)
		{

			//Global position of Earth, Moon and spacecraft, spacecraft rotation matrix from local to global
			lem->GetGlobalPos(LMPos);
			csm->GetGlobalPos(CSMPos);
			//oapiGetGlobalPos(hEarth, &R_E);
			//oapiGetGlobalPos(hMoon, &R_M);
			lem->GetRotationMatrix(Rot);

			//Vector pointing from LM to CSM
			R = CSMPos - LMPos;

			//Unit vector of it
			U_R = unit(R);

			//Unit vector of antenna in navigation base vessel's local frame, right handed
			U_RRL[0] = unit(_V(sin(shaftAngle + anginc)*cos(trunnionAngle), -sin(trunnionAngle), cos(shaftAngle + anginc)*cos(trunnionAngle)));
			U_RRL[1] = unit(_V(sin(shaftAngle - anginc)*cos(trunnionAngle), -sin(trunnionAngle), cos(shaftAngle - anginc)*cos(trunnionAngle)));
			U_RRL[2] = unit(_V(sin(shaftAngle)*cos(trunnionAngle + anginc), -sin(trunnionAngle + anginc), cos(shaftAngle)*cos(trunnionAngle + anginc)));
			U_RRL[3] = unit(_V(sin(shaftAngle)*cos(trunnionAngle - anginc), -sin(trunnionAngle - anginc), cos(shaftAngle)*cos(trunnionAngle - anginc)));

			//In LM navigation base coordinates, left handed
			for (int i = 0;i < 4;i++)
			{
				U_RRL[i] = _V(U_RRL[i].y, U_RRL[i].x, U_RRL[i].z);

				//Calculate antenna pointing vector in global frame
				U_RR = mul(Rot, U_RRL[i]);

				//relative angle between antenna pointing vector and direction of CSM
				relang = acos(dotp(U_RR, U_R));

				SignalStrengthQuadrant[i] = (pow(cos(hpbw_factor*relang), 2.0) + 1.0) / 2.0*exp(-25.0*relang*relang);
			}

			SignalStrength = (SignalStrengthQuadrant[0] + SignalStrengthQuadrant[1] + SignalStrengthQuadrant[2] + SignalStrengthQuadrant[3]) / 4.0;

			if (SignalStrength > 0.375 && length(R) > 80.0*0.3048 && length(R) < 400.0*1852.0)
			{
				internalrange = length(R);

				lem->GetGlobalVel(LMVel);
				csm->GetGlobalVel(CSMVel);

				internalrangerate = dotp(CSMVel - LMVel, U_R);
			}

			//sprintf(oapiDebugString(), "Shaft: %f, Trunnion: %f, Relative Angle: %f°, SignalStrength %f %f %f %f", shaftAngle*DEG, trunnionAngle*DEG, relang*DEG, SignalStrengthQuadrant[0], SignalStrengthQuadrant[1], SignalStrengthQuadrant[2], SignalStrengthQuadrant[3]);

		}
	}

	// Let's test.
	// First, manage the status bit.
	if (lem->RendezvousRadarRotary.GetState() == 2) {
		if (val33[RRPowerOnAuto] != 1) {
			val33[RRPowerOnAuto] = 1;
			lem->agc.SetInputChannel(033, val33);
			//sprintf(oapiDebugString(),"RR Power On Discrete Enabled");
		}
	}
	else {
		if (val33[RRPowerOnAuto] != 0) {
			val33[RRPowerOnAuto] = 0;
			lem->agc.SetInputChannel(033, val33);
			//sprintf(oapiDebugString(),"RR Power On Discrete Disabled");
		}
	}

	//Auto Tracking Logic
	if (lem->RendezvousRadarRotary.GetState() == 0)
	{
		AutoTrackEnabled = true;
	}
	else if (lem->RendezvousRadarRotary.GetState() == 2 && val12[RRAutoTrackOrEnable] == 1)
	{
		AutoTrackEnabled = true;
	}
	else
	{
		AutoTrackEnabled = false;
	}

	//Frequency Lock
	if (AutoTrackEnabled && SignalStrength > 0.375 && internalrange > 80.0*0.3048 && internalrange < 400.0*1852.0)
	{
		FrequencyLock = true;
	}
	else
	{
		FrequencyLock = false;
	}

	if (AutoTrackEnabled && FrequencyLock)
	{
		TrackingModeSwitch = true;
	}
	else
	{
		TrackingModeSwitch = false;
	}

	//AUTO TRACKING
	if (TrackingModeSwitch)
	{
		ShaftErrorSignal = (SignalStrengthQuadrant[0] - SignalStrengthQuadrant[1])*0.25;
		TrunnionErrorSignal = (SignalStrengthQuadrant[2] - SignalStrengthQuadrant[3])*0.25;

		shaftAngle += (ShaftErrorSignal - GyroRates.x)*simdt;
		shaftVel = ShaftErrorSignal;

		trunnionAngle += (TrunnionErrorSignal - GyroRates.y)*simdt;
		trunnionVel = TrunnionErrorSignal;

		//sprintf(oapiDebugString(), "Shaft: %f, Trunnion: %f, ShaftErrorSignal %f TrunnionErrorSignal %f", shaftAngle*DEG, trunnionAngle*DEG, ShaftErrorSignal, TrunnionErrorSignal);
	}
	else
	{
		ShaftErrorSignal = 0.0;
		TrunnionErrorSignal = 0.0;

		// Handle mode switch
		switch (lem->RendezvousRadarRotary.GetState()) {
		case 0:	// AUTO TRACK
			//Only here when there is no frequency lock
			trunnionVel = 0.0;
			shaftVel = 0.0;
			break;

		case 1: // SLEW
				// Watch the SLEW switch. 
			if (lem->RadarSlewSwitch.GetState() == 4) {	// Can we move up?
				trunnionAngle -= TrunRate * simdt;						// Move the trunnion
				trunnionVel = -TrunRate;
			}
			if (lem->RadarSlewSwitch.GetState() == 3) {	// Can we move down?
				trunnionAngle += TrunRate * simdt;						// Move the trunnion
				trunnionVel = TrunRate;
			}
			if (lem->RadarSlewSwitch.GetState() == 2) {
				shaftAngle += ShaftRate * simdt;
				shaftVel = ShaftRate;
			}
			if (lem->RadarSlewSwitch.GetState() == 0) {
				shaftAngle -= ShaftRate * simdt;
				shaftVel = -ShaftRate;
			}

			//sprintf(oapiDebugString(), "Ang %f Vel %f", shaftAngle*DEG, shaftVel);

			//if(lem->RadarTestSwitch.GetState() != THREEPOSSWITCH_UP){ sprintf(oapiDebugString(),"RR SLEW: SHAFT %f TRUNNION %f",shaftAngle*DEG,trunnionAngle*DEG); }
			break;
		case 2: // AGC
			{
				int pulses;

				pulses = lem->scdu.GetErrorCounter();

				shaftVel = (RR_SHAFT_STEP*pulses);
				shaftAngle += (RR_SHAFT_STEP*pulses)*simdt;

				pulses = lem->tcdu.GetErrorCounter();

				trunnionVel = (RR_SHAFT_STEP*pulses);
				trunnionAngle += (RR_SHAFT_STEP*pulses)*simdt;
			}
			break;
		}
	}

	if (AutoTrackEnabled && FrequencyLock)
	{
		if (RangeLockTimer < 12.0)
		{
			RangeLockTimer += simdt;
		}
	}
	else
	{
		RangeLockTimer = 0.0;
	}

	if (RangeLockTimer >= 12.0)
	{
		RangeLock = true;
	}
	else
	{
		RangeLock = false;
	}

	if (FrequencyLock && RangeLock)
	{
		radarDataGood = true;
	}
	else
	{
		radarDataGood = false;
	}

	if (FrequencyLock)
	{
		rate = internalrangerate;
	}
	else
	{
		rate = 0.0;
	}

	if (RangeLock)
	{
		range = internalrange;
	}
	else
	{
		range = 0.0;
	}

	//sprintf(oapiDebugString(), "Auto %d FreqLock %d Timer %f RLock %d DataGood %d", AutoTrackEnabled, FrequencyLock, RangeLockTimer, RangeLock, radarDataGood);

	//Limits

	if (trunnionAngle > 70.0*RAD)
	{
		trunnionVel = 0.0;
		trunnionAngle = 70.0*RAD;
	}
	else if (trunnionAngle < -250.0*RAD)
	{
		trunnionVel = 0.0;
		trunnionAngle = -250.0*RAD;
	}

	if (shaftAngle > 68.0*RAD)
	{
		shaftAngle = 68.0*RAD;
		shaftVel = 0.0;
	}
	else if (shaftAngle < -148.0*RAD)
	{
		shaftAngle = -148.0*RAD;
		shaftVel = 0.0;
	}

	//Mode I or II determination
	if (cos(trunnionAngle) > 0.0 && mode == 2)
	{
		mode = 1;
	}
	else if (cos(trunnionAngle) < 0.0 && mode == 1)
	{
		mode = 2;
	}

	lem->tcdu.SetReadCounter(trunnionAngle);
	lem->scdu.SetReadCounter(shaftAngle);

	if (lem->RendezvousRadarRotary.GetState() == 2)
	{

		//sprintf(oapiDebugString(),"RR MOVEMENT: SHAFT %f TRUNNION %f RANGE %f RANGE-RATE %f",shaftAngle*DEG,trunnionAngle*DEG,range,rate);

		// Maintain RADAR GOOD state
		if (radarDataGood == 1 && val33[RRDataGood] == 0) { val33[RRDataGood] = 1; lem->agc.SetInputChannel(033, val33); }
		if (radarDataGood == 0 && val33[RRDataGood] == 1) { val33[RRDataGood] = 0; lem->agc.SetInputChannel(033, val33); }
		// Maintain radar scale indicator
		// We use high scale above 50.6nm, and low scale below that.
		if (range > 93700 && val33[RRRangeLowScale] == 1) {
			// HI SCALE
			val33[RRRangeLowScale] = 0; lem->agc.SetInputChannel(033, val33);
		}
		if (range < 93701 && val33[RRRangeLowScale] == 0) {
			// LO SCALE
			val33[RRRangeLowScale] = 1; lem->agc.SetInputChannel(033, val33);
		}

		// Print status
		/*
		char debugmsg[256];
		sprintf(debugmsg,"RADAR STATUS: ");
		if(val12.Bits.ZeroRRCDU != 0){ sprintf(debugmsg,"%s ZeroRRCDU",debugmsg); }
		if(val12.Bits.EnableRRCDUErrorCounter != 0){ sprintf(debugmsg,"%s EnableEC",debugmsg); }
		if(val12.Bits.LRPositionCommand != 0){ sprintf(debugmsg,"%s LRPos2",debugmsg); }
		if(val12.Bits.RRAutoTrackOrEnable != 0){ sprintf(debugmsg,"%s RRAutoTrk",debugmsg); }
		if(val13.Bits.RadarA != 0){ sprintf(debugmsg,"%s RadarA",debugmsg); }
		if(val13.Bits.RadarB != 0){ sprintf(debugmsg,"%s RadarB",debugmsg); }
		if(val13.Bits.RadarC != 0){ sprintf(debugmsg,"%s RadarC",debugmsg); }
		if(val13.Bits.RadarActivity != 0){ sprintf(debugmsg,"%s RdrActy",debugmsg); }

		if(val14.Bits.ShaftAngleCDUDrive != 0){ sprintf(debugmsg,"%s DriveS(%f)",debugmsg,shaftAngle*DEG); }
		if(val14.Bits.TrunnionAngleCDUDrive != 0){ sprintf(debugmsg,"%s DriveT(%f)",debugmsg,trunnionAngle*DEG); }
		sprintf(oapiDebugString(),debugmsg);
		*/

		// The computer wants something from the radar.
		if (val13[RadarActivity] == 1) {
			int radarBits = 0;
			if (val13[RadarA] == 1) { radarBits |= 1; }
			if (val13[RadarB] == 1) { radarBits |= 2; }
			if (val13[RadarC] == 1) { radarBits |= 4; }
			switch (radarBits) {
			case 1:
				// LR (LR VEL X)
				// Not our problem
				break;
			case 2:
				// RR RANGE RATE
				// Our center point is at 17000 counts.
				// Counts are 0.627826 F/COUNT, negative = positive rate, positive = negative rate
				lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(17000.0 - (rate / 0.191361));
				lem->agc.SetInputChannelBit(013, RadarActivity, 0);
				lem->agc.GenerateRadarupt();
				ruptSent = 2;

				break;
			case 3:
				// LR (LR VEL Z)
				// Not our problem
				break;
			case 4:
				// RR RANGE
				// We use high scale above 50.6nm, and low scale below that.
				if (range > 93700) {
					// HI SCALE
					// Docs says this should be 75.04 feet/bit, or 22.8722 meters/bit
					lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 22.8722);
				}
				else {
					// LO SCALE
					// Should be 9.38 feet/bit
					lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 2.85902);
				}
				lem->agc.SetInputChannelBit(013, RadarActivity, 0);
				lem->agc.GenerateRadarupt();
				ruptSent = 4;

				break;
			case 5:
				// LR (LR VEL Y)
				// Not our problem
				break;
			case 7:
				// LR (LR RANGE)
				// Not our problem
				break;
				/*
				default:
				sprintf(oapiDebugString(),"%s BADBITS",debugmsg);
				*/
			}

		}
		else {
			ruptSent = 0;
		}
	}

	//sprintf(oapiDebugString(), "Shaft %f, Trunnion %f Mode %d", shaftAngle*DEG, trunnionAngle*DEG, mode);
	//sprintf(oapiDebugString(), "RRDataGood: %d ruptSent: %d  RadarActivity: %d Range: %f", val33[RRDataGood] == 0, ruptSent, val13[RadarActivity] == 1, range);
}

void LEM_RR::SystemTimestep(double simdt) {
	if (IsDCPowered())
	{
		dc_source->DrawPower(117);
		RREHeat->GenerateHeat(58.5);
		RRESECHeat->GenerateHeat(58.5);
	}

	if (IsACPowered())
	{
		ac_source->DrawPower(13.8);
		RREHeat->GenerateHeat(6.9);
		RRESECHeat->GenerateHeat(6.9);
	}

	if (abs(shaftVel) > 0.01*RAD)
	{
		dc_source->DrawPower(16.5);
		rrheat->GenerateHeat(10.0); //Guessed as a lower number to control RR heat since all the power will not be converted to heat 
	}

	if (abs(trunnionVel) > 0.01*RAD)
	{
		dc_source->DrawPower(16.5);
		rrheat->GenerateHeat(10.0); //Guessed as a lower number to control RR heat since all the power will not be converted to heat 
	}

}

void LEM_RR::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_double(scn, "RR_TRUN", trunnionAngle);
	papiWriteScenario_double(scn, "RR_SHAFT", shaftAngle);
	papiWriteScenario_double(scn, "RR_ANTTEMP", GetAntennaTempF());
	oapiWriteScenario_int(scn, "RR_MODE", mode);
	papiWriteScenario_double(scn, "RR_RANGELOCKTIMER", RangeLockTimer);
	papiWriteScenario_bool(scn, "RR_RADARDATAGOOD", radarDataGood);
	papiWriteScenario_double(scn, "RR_RANGE", range);
	papiWriteScenario_double(scn, "RR_RATE", rate);
	oapiWriteLine(scn, end_str);
}

void LEM_RR::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	double dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;

		papiReadScenario_double(line, "RR_TRUN", trunnionAngle);
		papiReadScenario_double(line, "RR_SHAFT", shaftAngle);
		papiReadScenario_int(line, "RR_MODE", mode);
		papiReadScenario_double(line, "RR_RANGELOCKTIMER", RangeLockTimer);
		papiReadScenario_bool(line, "RR_RADARDATAGOOD", radarDataGood);
		papiReadScenario_double(line, "RR_RANGE", range);
		papiReadScenario_double(line, "RR_RATE", rate);
	}
}