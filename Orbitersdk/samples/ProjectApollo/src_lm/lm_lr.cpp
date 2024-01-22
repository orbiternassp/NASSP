/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2023

Lunar Module Landing Radar

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
#include "lm_lr.h"

// Landing Radar
LEM_LR::LEM_LR()
{
	lem = NULL;
	lrheat = 0;
	antennaAngle = 24; // Position 1
}

void LEM_LR::Init(LEM *s, e_object *dc_src, h_Radiator *ant, Boiler *anheat, h_HeatLoad *hl) {
	lem = s;
	// Set up antenna.
	// LR antenna is designed to operate between 0F and 185F
	// The heater switches on if the temperature gets below +55F and turns it off again when the temperature reaches +70F
	// Values in the constructor are name, pos, vol, isol
	antenna = ant;
	antheater = anheat;
	lrheat = hl;
	antenna->isolation = 0.00001;
	antenna->Area = 1250; // 1250 cm
	if (lem != NULL) {
		antheater->WireTo(&lem->HTR_LR_CB);
	}
	// Attach power source
	dc_source = dc_src;
	// Clear flags
	range = 0;
	rate[0] = rate[1] = rate[2] = 0;
	rangeGood = 0;
	velocityGood = 0;
}

// Are we on?
bool LEM_LR::IsPowered()

{
	if (dc_source->Voltage() < SP_MIN_DCVOLTAGE || lem->stage > 1) {
		return false;
	}
	return true;
}

void LEM_LR::GetRangeLGC()
{
	if (!IsPowered()) return;

	// High range is 5.395 feet per count
	// Low range is 1.079 feet per count
	if (range >= 2500.0) {
		// Hi Range
		lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 5.395);
	}
	else {
		// Lo Range
		lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(range / 1.079);
	}
}

void LEM_LR::GetVelocityXLGC()
{
	if (!IsPowered()) return;

	// 12288 COUNTS = -000000 F/S
	// SIGN REVERSED
	// 0.643966 F/S PER COUNT
	lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(12288.0 - (rate[0] / 0.643966));
}

void LEM_LR::GetVelocityYLGC()
{
	if (!IsPowered()) return;

	// 12288 COUNTS = +000000 F/S
	// 1.211975 F/S PER COUNT
	lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(12288.0 + (rate[1] / 1.211975));
}

void LEM_LR::GetVelocityZLGC()
{
	if (!IsPowered()) return;

	// 12288 COUNTS = +00000 F/S
	// 0.866807 F/S PER COUNT
	lem->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)(12288.0 + (rate[2] / 0.866807));
}

double LEM_LR::GetAltTransmitterPower()
{
	if (!IsPowered())
	{
		return 0;
	}

	return 3.0;
}

double LEM_LR::GetVelTransmitterPower()
{
	if (!IsPowered())
	{
		return 0;
	}

	return 3.0;
}

void LEM_LR::Timestep(double simdt) {
	if (lem == NULL) { return; }
	// char debugmsg[256];
	ChannelValue val12;
	ChannelValue val33;
	val12 = lem->agc.GetInputChannel(012);
	val33 = lem->agc.GetInputChannel(033);

	if (IsPowered())
	{
		antheater->SetPumpOff();
	}

	else
	{
		antheater->SetPumpAuto();
	}

	if (!IsPowered()) {
		// Clobber data.
		bool clobber = FALSE;
		if (val33[LRDataGood]) { clobber = TRUE; val33[LRDataGood] = 0; }
		if (val33[LRVelocityDataGood]) { clobber = TRUE; val33[LRVelocityDataGood] = 0; }
		if (val33[LRPos1]) { clobber = TRUE; val33[LRPos1] = 0; }
		if (val33[LRPos2]) { clobber = TRUE; val33[LRPos2] = 0; }
		if (val33[LRRangeLowScale] == 0) { clobber = TRUE; val33[LRRangeLowScale] = 1; }
		if (clobber == TRUE) { lem->agc.SetInputChannel(033, val33); }
		rangeGood = 0;
		velocityGood = 0;
		return;
	}

	// The altimeter works up to 40,000 feet.
	// The velocity data should be working by 24,000 feet.
	// Velocity Z is forward, Velocity X is lateral, meaning Velocity Y must be vertical.
	// Below 500 feet, the radar may lose lock due to zero doppler.
	// Below 50 feet, the LGC starts ignoring the radar.

	// Follow drive commands and use power
	// The antenna takes 10 seconds to move, and draws 15 watts while doing so.
	// The computer can command position 2, but it can't command position 1 from position 2.
	if (val12[LRPositionCommand] == 1 || lem->LandingAntSwitch.GetState() == THREEPOSSWITCH_DOWN) {
		if (antennaAngle != 0) {
			// Drive to Position 2
			antennaAngle -= (2.4*simdt);
			if (antennaAngle < 0) { antennaAngle = 0; }
			dc_source->DrawPower(140);
			// sprintf(oapiDebugString(),"LR CPos %d Pos %0.1f",val12.Bits.LRPositionCommand,antennaAngle);
		}
		else {
			// At position 2
			dc_source->DrawPower(125);
		}
	}
	else {
		if (lem->LandingAntSwitch.GetState() == THREEPOSSWITCH_CENTER && antennaAngle != 24) {
			// Drive to Position 1
			antennaAngle += (2.4*simdt);
			if (antennaAngle > 24) { antennaAngle = 24; }
			dc_source->DrawPower(140);
			// sprintf(oapiDebugString(),"LR CPos %d Pos %0.1f",val12.Bits.LRPositionCommand,antennaAngle);
		}
		else {
			// At position 1
			dc_source->DrawPower(125);
		}
	}
	// Maintain antenna angle discretes
	// If at Pos 1
	if (antennaAngle == 24) {
		// Light Pos 1
		if (val33[LRPos1] == 0) {
			val33[LRPos1] = 1;
			lem->agc.SetInputChannel(033, val33);
		}
	}
	else {
		// Otherwise
		// Clobber Pos 1 flag
		if (val33[LRPos1] == 1) {
			val33[LRPos1] = 0;
			lem->agc.SetInputChannel(033, val33);
		}
		// If at Pos 2
		if (antennaAngle == 0) {
			// Light Pos 2
			if (val33[LRPos2] == 0) {
				val33[LRPos2] = 1;
				lem->agc.SetInputChannel(033, val33);
			}
		}
		else {
			// Otherwise clobber Pos 2 flag
			if (val33[LRPos2] == 1) {
				val33[LRPos2] = 0;
				lem->agc.SetInputChannel(033, val33);
			}
		}
	}

	// Data Determination
	if (lem->RadarTestSwitch.GetState() == THREEPOSSWITCH_DOWN) {
		if (antennaAngle == 0) {
			// Test Mode POS 2
			// Drive to:
			//
			//
			//
			//
			range = 8000;
			rate[0] = -494;
			rate[1] = 1861;
			rate[2] = 1331;
			rangeGood = 1;
			velocityGood = 1;
		}
		else {
			// Test Mode
			// Drive to:
			// Alt 8287 ft
			// Vel -494,1861,1331 ft/sec
			// on the LGC
			// For some reason this should show up as 8000 ft and -480 fps on the alt/alt-rate monitor?
			range = 8287;
			rate[0] = -494;
			rate[1] = 1861;
			rate[2] = 1331;
			rangeGood = 1;
			velocityGood = 1;
		}
	}
	else {
		// Operate Mode
		rangeGood = 0;
		velocityGood = 0;

		MATRIX3 Rot;
		VECTOR3 pos, lrvec_glob, U_XAB, U_YAB, U_ZAB, U_RBA, U_RBB, U_RBB_lh;
		OBJHANDLE gravref;
		double alt, cos_ang, alpha, beta, dh;

		//landing radar under CoG of LM
		dh = 3.0;

		//Gravity reference
		gravref = lem->GetGravityRef();

		//Altitude
		alt = lem->GetAltitude(ALTMODE_GROUND) - dh;

		//Rotation matrix
		lem->GetRotationMatrix(Rot);

		//state vector
		lem->GetRelativePos(gravref, pos);

		pos = pos * (length(pos) - dh) / length(pos);

		//Radar Beams Orientation Subroutine
		alpha = -6.0*RAD;
		beta = -antennaAngle * RAD;

		U_XAB = _V(cos(beta), sin(alpha)*sin(beta), -sin(beta)*cos(alpha));
		U_YAB = _V(0, cos(alpha), sin(alpha));
		U_ZAB = _V(sin(beta), -sin(alpha)*cos(beta), cos(beta)*cos(alpha));

		U_RBA = _V(-cos(20.38*RAD), 0, -sin(20.38*RAD));

		U_RBB = mul(_M(U_XAB.x, U_YAB.x, U_ZAB.x, U_XAB.y, U_YAB.y, U_ZAB.y, U_XAB.z, U_YAB.z, U_ZAB.z), U_RBA);

		//Now Left handed. But also needs to change coordinate system differences
		U_RBB_lh = _V(U_RBB.y, U_RBB.x, U_RBB.z);

		//convert local LR vector to global frame.
		lrvec_glob = mul(Rot, U_RBB_lh);

		//Angle between local vertical and LR vector
		cos_ang = dotp(unit(-pos), unit(lrvec_glob));

		//Assumption: Moon is flat
		range = alt / cos_ang / 0.3048;

		//Doesn't point at the moon
		if (range < 0)
		{
			range = 1000000.0;
		}

		if (range > 10.0 && range < 50000.0) //Arbitrary, goal is lock on at 40,000 feet altitude
		{
			rangeGood = 1;
		}

		//Now velocity data
		VECTOR3 vel, vel_lh, vel_LR;

		lem->GetGroundspeedVector(FRAME_LOCAL, vel_lh);

		//In LM navigation base coordinates
		vel = _V(vel_lh.y, vel_lh.x, vel_lh.z);

		//Rotate to LR position
		vel_LR = tmul(_M(U_XAB.x, U_YAB.x, U_ZAB.x, U_XAB.y, U_YAB.y, U_ZAB.y, U_XAB.z, U_YAB.z, U_ZAB.z), vel);

		rate[0] = vel_LR.x / 0.3048;
		rate[1] = vel_LR.y / 0.3048;
		rate[2] = vel_LR.z / 0.3048;

		if (range > 10.0 && range < 50000.0)
		{
			velocityGood = 1;
		}

		//sprintf(oapiDebugString(), "Alt: %f, Range: %f, Velocity: %f %f %f", alt/0.3048, range, rate[0], rate[1], rate[2]);
	}

	// Computer interface
	/*
	sprintf(debugmsg,"LR STATUS: ");
	if(val12.Bits.LRPositionCommand != 0){ sprintf(debugmsg,"%s LRPos2",debugmsg); }
	if(val13.Bits.RadarA != 0){ sprintf(debugmsg,"%s RadarA",debugmsg); }
	if(val13.Bits.RadarB != 0){ sprintf(debugmsg,"%s RadarB",debugmsg); }
	if(val13.Bits.RadarC != 0){ sprintf(debugmsg,"%s RadarC",debugmsg); }
	if(val13.Bits.RadarActivity != 0){ sprintf(debugmsg,"%s RdrActy",debugmsg); }
	sprintf(oapiDebugString(),debugmsg);
	*/

	// Maintain discretes
	// Range data good
	if (rangeGood == 1 && val33[LRDataGood] == 0) { val33[LRDataGood] = 1; lem->agc.SetInputChannel(033, val33); }
	if (rangeGood == 0 && val33[LRDataGood] == 1) { val33[LRDataGood] = 0; lem->agc.SetInputChannel(033, val33); }
	// RANGE SCALE:
	// C++ VALUE OF 1 = HIGH RANGE
	// C++ VALUE OF 0 = LOW RANGE
	// We switch from high range to low range at 2500 feet
	// Range scale affects only the altimeter, velocity is not affected.
	if ((rangeGood == 1 && range < 2500) && val33[LRRangeLowScale] == 0) { val33[LRRangeLowScale] = 1; lem->agc.SetInputChannel(033, val33); }
	if ((rangeGood == 0 || range > 2500) && val33[LRRangeLowScale] == 1) { val33[LRRangeLowScale] = 0; lem->agc.SetInputChannel(033, val33); }
	// Velocity data good
	if (velocityGood == 1 && val33[LRVelocityDataGood] == 0) { val33[LRVelocityDataGood] = 1; lem->agc.SetInputChannel(033, val33); }
	if (velocityGood == 0 && val33[LRVelocityDataGood] == 1) { val33[LRVelocityDataGood] = 0; lem->agc.SetInputChannel(033, val33); }

	//sprintf(oapiDebugString(), "rangeGood: %d velocityGood: %d ruptSent: %d  RadarActivity: %d Position %f° Range: %f", rangeGood, velocityGood, ruptSent, val13[RadarActivity] == 1, antennaAngle, range);
}

void LEM_LR::SystemTimestep(double simdt)
{
	if (IsPowered())
	{
		lrheat->GenerateHeat(118);
	}
}

void LEM_LR::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_double(scn, "RANGE", range);
	papiWriteScenario_double(scn, "ANTENNAANGLE", antennaAngle);
	oapiWriteScenario_int(scn, "RANGEGOOD", rangeGood);
	oapiWriteScenario_int(scn, "VELOCITYGOOD", velocityGood);
	papiWriteScenario_vec(scn, "RATE", _V(rate[0], rate[1], rate[2]));
	oapiWriteLine(scn, end_str);
}

void LEM_LR::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	double dec = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;
		if (!strnicmp(line, "RANGE", 5)) {
			sscanf(line + 5, "%lf", &dec);
			range = dec;
		}
		if (!strnicmp(line, "ANTENNAANGLE", 12)) {
			sscanf(line + 12, "%lf", &dec);
			antennaAngle = dec;
		}
		if (!strnicmp(line, "RANGEGOOD", 9)) {
			sscanf(line + 9, "%d", &rangeGood);
		}
		if (!strnicmp(line, "VELOCITYGOOD", 12)) {
			sscanf(line + 12, "%d", &velocityGood);
		}
		if (!strnicmp(line, "RATE", 4)) {
			sscanf(line + 4, "%lf %lf %lf", &rate[0], &rate[1], &rate[2]);
		}
	}
}

double LEM_LR::GetAntennaTempF() {
	if (lem->stage < 2) {
		return KelvinToFahrenheit(antenna->GetTemp());
	}
	else {
		return KelvinToFahrenheit(0.0);
	}
}