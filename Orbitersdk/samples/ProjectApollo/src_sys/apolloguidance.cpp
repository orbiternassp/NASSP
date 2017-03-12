/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Basic Apollo Guidance computer setup

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


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "inttypes.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "yaAGC/agc_engine.h"
#include "ioChannels.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "IMU.h"
#include "powersource.h"
#include "papi.h"

#include "tracer.h"

char TwoSpaceTwoFormat[7] = "XXX XX";
char RegFormat[7] = "XXXXXX";

// Moved DELTAT definition to avoid INTERNAL COMPILER ERROR
#define DELTAT 2.0

ApolloGuidance::ApolloGuidance(SoundLib &s, DSKY &display, IMU &im, PanelSDK &p) : soundlib(s), dsky(display), imu(im), DCPower(0, p)

{
	Reset = false;
	CurrentTimestep = 0;
	LastTimestep = 0;
	LastCycled = 0;

	DesiredApogee = 0.0;
	DesiredPerigee = 0.0;
	DesiredAzimuth = 0.0;
	DesiredInclination = 0.0;

	LandingLongitude = 0.0;
	LandingLatitude = 0.0;
	LandingAltitude = 0.0;

	//
	// Expected dV from thrust decay of engine.
	//

	ThrustDecayDV = 0.0;

	//
	// Flight number.
	//

	ApolloNo = 0;

	OtherVesselName[0] = 0;

	//
	// Clear channels.
	//

	int i;

	for (i = 0; i <= MAX_OUTPUT_CHANNELS; i++)
		OutputChannel[i] = 0;
	for (i = 0; i <= MAX_INPUT_CHANNELS; i++)
		InputChannel[i] = 0;

	//
	// Dsky interface.
	//

	dsky2 = NULL;

	isFirstTimestep = true;
	PadLoaded = false;

	ProgAlarm = false;
	GimbalLockAlarm = false;

	//
	// Virtual AGC.
	//
	memset(&vagc, 0, sizeof(vagc));
	vagc.agc_clientdata = this;
	agc_engine_init(&vagc, NULL, NULL, 0);

#ifdef _DEBUG
	out_file = fopen("ProjectApollo AGC.log", "wt");
	vagc.out_file = out_file;
#endif

	PowerConnected = false;
}

ApolloGuidance::~ApolloGuidance()

{
#ifdef _DEBUG
	fclose(out_file);
#endif
}

void ApolloGuidance::InitVirtualAGC(char *binfile)

{

	(void) agc_load_binfile(&vagc, binfile);

	// Set channels only once, otherwise this code overwrites the channel values in the scenario
	if (!PadLoaded) { 
		ChannelValue val30;
		ChannelValue val31;
		ChannelValue val32;
		ChannelValue val33;

		//
		// Set default state. Note that these are oddities, as zero means
		// true and one means false for the 'Virtual AGC'!
		//

		val30 = 077777;
		// Enable to turn on
		// val30.Bits.IMUOperate = 0;
		val30.reset(TempInLimits);

		//
		// We default to the IMU turned off. If you change this, change the IMU code to
		// match.
		//
		val30.set(IMUOperate);

		vagc.InputChannel[030] = (int16_t)val30.to_ulong();
		InputChannel[030] = (val30.to_ulong() ^ 077777);

		val31 = 077777;
		// Default position of the CMC MODE switch is FREE
		val31[FreeFunction] = 0;

		vagc.InputChannel[031] = (int16_t)val31.to_ulong();
		InputChannel[031] = (val31.to_ulong() ^ 077777);


		val32 = 077777;
		vagc.InputChannel[032] = (int16_t)val32.to_ulong();
		InputChannel[032] = (val32.to_ulong() ^ 077777);

		val33 = 077777;
	//	val33.Bits.RangeUnitDataGood = 0;
	//	val33.Bits.BlockUplinkInput = 0;

		//
		// Setting AGCWarning is needed to avoid the 1107 alarm on startup. It basically forces the AGC to do a
		// hard reset in the RESTART code.
		//
		//	CA	BIT14			# IF AGC WARNING ON (BIT = 0), DO A FRESH
		//	EXTEND				# START ON THE ASSUMPTION THAT WE'RE IN A
		//	RAND	CHAN33		# RESTART LOOP.
		//

		val33[AGCWarning] = 0;
		
		vagc.InputChannel[033] = (int16_t)val33.to_ulong();
		InputChannel[033] = (val33.to_ulong() ^ 077777);
	}
}

//
// Force a hardware restart.
//

void ApolloGuidance::ForceRestart()

{
	Reset = false;
}

bool ApolloGuidance::OutOfReset()

{
	return true;
}


// Do a single timestep - Used by CM to maintain sync between telemetry and vAGC.
bool ApolloGuidance::SingleTimestepPrep(double simt, double simdt){
	LastTimestep = CurrentTimestep;
	CurrentTimestep = simt;
	return TRUE;
}

bool ApolloGuidance::SingleTimestep() {

	agc_engine(&vagc);
	return TRUE;
}

void ApolloGuidance::VirtualAGCCoreDump(char *fileName) {

	MakeCoreDump(&vagc, fileName); 
}

bool ApolloGuidance::GenericTimestep(double simt, double simdt)
{
//	TRACESETUP("COMPUTER TIMESTEP");
	int i;

	LastTimestep = CurrentTimestep;
	CurrentTimestep = simt;

	// Physical AGC timing was generated from a master 1024 KHz clock, divided by 12.
	// This resulted in a machine cycle of just over 11.7 microseconds.
	int cycles = (long) ((simdt) * 1024000 / 12);

	for (i = 0; i < cycles; i++) {
		agc_engine(&vagc);
	}

	return true;
}

void ApolloGuidance::SystemTimestep(double simdt) 

{
	if (!IsPowered()) return;

	if (OnStandby()) {
		DCPower.DrawPower(22.9);
	}
	else {
		DCPower.DrawPower(106.0);
	}
}

void ApolloGuidance::SetMissionInfo(int MissionNo, char *OtherName) 

{
	//
	// Older scenarios saved the mission number in the AGC. For backwards
	// compatibility we'll only let the new number overwrite the saved value
	// if it's zero.
	//

	if (!ApolloNo)
		ApolloNo = MissionNo; 

	if (OtherName != 0)
		strncpy(OtherVesselName, OtherName, 64);
}

//
// Most of this burn calculation code is lifted from the Soyuz guidance MFD.
//

// Returns the absolute value of a vector
double AbsOfVector(const VECTOR3 &Vec)
{
	double Result;
	Result = sqrt(Vec.x*Vec.x + Vec.y*Vec.y + Vec.z*Vec.z);
	return Result;
}

void ApolloGuidance::EquToRel(double vlat, double vlon, double vrad, VECTOR3 &pos)
{
		VECTOR3 a;
		double obliq, theta, rot;
		OBJHANDLE hbody=OurVessel->GetGravityRef();
		a.x=cos(vlat)*cos(vlon)*vrad;
		a.z=cos(vlat)*sin(vlon)*vrad;
		a.y=sin(vlat)*vrad;
		obliq=oapiGetPlanetObliquity(hbody);
		theta=oapiGetPlanetTheta(hbody);
		rot=oapiGetPlanetCurrentRotation(hbody);
		pos.x=a.x*(cos(theta)*cos(rot)-sin(theta)*cos(obliq)*sin(rot))-
			a.y*sin(theta)*sin(obliq)-
			a.z*(cos(theta)*sin(rot)+sin(theta)*cos(obliq)*cos(rot));
		pos.y=a.x*(-sin(obliq)*sin(rot))+
			a.y*cos(obliq)-
			a.z*sin(obliq)*cos(rot);
		pos.z=a.x*(sin(theta)*cos(rot)+cos(theta)*cos(obliq)*sin(rot))+
			a.y*cos(theta)*sin(obliq)+
			a.z*(-sin(theta)*sin(rot)+cos(theta)*cos(obliq)*cos(rot));

}

//
// Virtual AGC Erasable memory functions.
//
// Currenty do nothing.
//


int ApolloGuidance::GetErasable(int bank, int address)

{
	if (bank < 0 || bank > 8)
		return 0;
	if (address < 0 || address > 0400)
		return 0;

	return vagc.Erasable[bank][address];
}

void ApolloGuidance::SetErasable(int bank, int address, int value)

{
	if (bank < 0 || bank > 8)
		return;
	if (address < 0 || address > 0400)
		return;

	vagc.Erasable[bank][address] = value;
}

void ApolloGuidance::PulsePIPA(int RegPIPA, int pulses) 

{
	int i;
	//
	// No pulsing, Don't lock the thread mutex. Locking the mutex here slows time acceleration, like single thread.
	// There should not be any pulsing during time acceleration anyways. i.e. no thruster firing. Still locking
	// the mutex in case the agc does small maneuvering during time acceleration.
	//
	if (pulses == 0 ) 
		return;

	Lock lock(agcCycleMutex);


	if (pulses >= 0) {
    	for (i = 0; i < pulses; i++) {
			UnprogrammedIncrement(&vagc, RegPIPA, 0);	// PINC

    	}
	} else {
    	for (i = 0; i < -pulses; i++) {
			UnprogrammedIncrement(&vagc, RegPIPA, 2);	// MINC
    	}
	}

}

//
// State save/load routines.
//

//
// Note that this state structure is now full! If you want to add a bit you'll have to add it to
// a new structure.
//

typedef union

{
	struct {
		unsigned Reset:1;
		unsigned isFirstTimestep:1;
		unsigned ExtraCode:1;
		unsigned AllowInterrupt:1;
		unsigned InIsr:1;
		unsigned SubstituteInstruction:1;
		unsigned PendFlag:1;
		unsigned PendDelay:3;
		unsigned ExtraDelay:3;
		unsigned DownruptTimeValid:1;
		unsigned PadLoaded:1;
		unsigned NightWatchman:1;
		unsigned RuptLock:1;
		unsigned NoRupt:1;
		unsigned TCTrap:1;
		unsigned NoTC:1;
		unsigned Standby:1;
		unsigned SbyPressed:1;
		unsigned SbyStillPressed:1;
		unsigned ParityFail:1;
		unsigned CheckParity:1;
	} u;
	unsigned long word;
} AGCState;

//
// Global variables in agc_engine.c which probably have to be saved, too
//

extern "C" {
	extern int NextZ;
	extern int ScalerCounter;
	extern int ChannelRoutineCount;
}

void ApolloGuidance::SaveState(FILEHANDLE scn)

{
	char fname[32], str[32], buffer[256];
	int i;
	int val;

	oapiWriteLine(scn, AGC_START_STRING);

	oapiWriteScenario_float(scn, "TGTA", DesiredApogee);
	oapiWriteScenario_float(scn, "TGTP", DesiredPerigee);
	oapiWriteScenario_float(scn, "TGTZ", DesiredAzimuth);

	if (OtherVesselName[0])
		oapiWriteScenario_string(scn, "ONAME", OtherVesselName);

	//
	// Copy internal state to the structure.
	//

	AGCState state;

	state.word = 0;
	state.u.Reset = Reset;
	state.u.isFirstTimestep = isFirstTimestep;
	state.u.ExtraCode = vagc.ExtraCode;
	state.u.AllowInterrupt = vagc.AllowInterrupt;
	state.u.InIsr = vagc.InIsr;
	state.u.SubstituteInstruction = vagc.SubstituteInstruction;
	state.u.PendFlag = vagc.PendFlag;
	state.u.PendDelay = vagc.PendDelay;
	state.u.ExtraDelay = vagc.ExtraDelay;
	state.u.DownruptTimeValid = vagc.DownruptTimeValid;
	state.u.PadLoaded = PadLoaded;
	state.u.NightWatchman = vagc.NightWatchman;
	state.u.RuptLock = vagc.RuptLock;
	state.u.NoRupt = vagc.NoRupt;
	state.u.TCTrap = vagc.TCTrap;
	state.u.NoTC = vagc.NoTC;
	state.u.Standby = vagc.Standby;
	state.u.SbyPressed = vagc.SbyPressed;
	state.u.SbyStillPressed = vagc.SbyStillPressed;
	state.u.ParityFail = vagc.ParityFail;
	state.u.CheckParity = vagc.CheckParity;

	oapiWriteScenario_int(scn, "STATE", state.word);

	//
	// Write out any non-zero EMEM state.
	//

	for (i = 0; i < EMEM_ENTRIES; i++) {
		// Always save RegZ because it's set in agc_engine_init, so we have to store 0, too
		if (ReadMemory(i, val) && (val != 0 || i == RegZ)) {
			sprintf(fname, "EMEM%04o", i);
			sprintf(str, "%o", val);
			oapiWriteScenario_string (scn, fname, str);
		}
	}

	//
	// And non-zero I/O state.
	//

	for (i = 0; i < MAX_INPUT_CHANNELS; i++) {
		val = GetInputChannel(i);
		if (val != 0) {
			sprintf(fname, "ICHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	for (i = 0; i < MAX_OUTPUT_CHANNELS; i++) {
		val = GetOutputChannel(i);
		if (val != 0) {
			sprintf(fname, "OCHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}


	for (i = 0; i < NUM_CHANNELS; i++) {
		val = vagc.InputChannel[i];
		// Always save channel 030 - 033 because they're set in agc_engine_init, so we have to store 0, too
		if (val != 0 || (i >= 030 && i <= 033)) {
			sprintf(fname, "VICHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	oapiWriteScenario_int (scn, "VOC7", vagc.OutputChannel7);
	oapiWriteScenario_int (scn, "IDXV", vagc.IndexValue);
	oapiWriteScenario_int (scn, "NEXTZ", NextZ);
	oapiWriteScenario_int (scn, "SCALERCOUNTER", ScalerCounter);
	oapiWriteScenario_int (scn, "CRCOUNT", ChannelRoutineCount);
	oapiWriteScenario_int (scn, "CH33SWITCHES", vagc.Ch33Switches);

	sprintf(buffer, "  CYCLECOUNTER %I64d", vagc.CycleCounter);
	oapiWriteLine(scn, buffer);
		
	for (i = 0; i < 16; i++) {
		val = vagc.OutputChannel10[i];
		sprintf(fname, "V10CHAN%03d", i);
		oapiWriteScenario_int (scn, fname, val);
	}

	for (i = 0; i < (1 + NUM_INTERRUPT_TYPES); i++) {
		val = vagc.InterruptRequests[i];
		sprintf(fname, "VINT%03d", i);
		oapiWriteScenario_int (scn, fname, val);
	}

	papiWriteScenario_bool(scn, "PROGALARM", ProgAlarm);
	papiWriteScenario_bool(scn, "GIMBALLOCKALARM", GimbalLockAlarm);

	oapiWriteLine(scn, AGC_END_STRING);
}

void ApolloGuidance::LoadState(FILEHANDLE scn)

{
	char	*line;
	float	flt;

	//
	// Now load the data.
	//

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, AGC_END_STRING, sizeof(AGC_END_STRING)))
			break;
			
		if (!strnicmp (line, "TGTA", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredApogee = flt;
		}
		else if (!strnicmp (line, "TGTP", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredPerigee = flt;
		}
		else if (!strnicmp (line, "TGTZ", 4)) {
			sscanf (line+4, "%f", &flt);
			DesiredAzimuth = flt;
		}
		else if (!strnicmp (line, "EMEM", 4)) {
			int num, val;
			sscanf(line+4, "%o", &num);
			sscanf(line+9, "%o", &val);
			WriteMemory(num, val);
		}
		else if (!strnicmp (line, "ICHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line+5, "%d", &num);
			sscanf(line+9, "%d", &val);
			InputChannel[num] = val;
		}
		else if (!strnicmp (line, "VICHAN", 6)) {
			int num;
			unsigned int val;
			sscanf(line+6, "%d", &num);
			sscanf(line+10, "%d", &val);
			vagc.InputChannel[num] = val;
		}
		else if (!strnicmp (line, "V10CHAN", 7)) {
			int num;
			unsigned int val;
			sscanf(line+7, "%d", &num);
			sscanf(line+11, "%d", &val);
			vagc.OutputChannel10[num] = val;
		}
		else if (!strnicmp (line, "OCHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line+5, "%d", &num);
			sscanf(line+9, "%d", &val);
			OutputChannel[num] = val;
		}
		else if (!strnicmp (line, "VOC7", 4)) {
			sscanf (line+4, "%" SCNd16, &vagc.OutputChannel7);
		}
		else if (!strnicmp (line, "IDXV", 4)) {
			sscanf (line+4, "%" SCNd16, &vagc.IndexValue);
		}
		else if (!strnicmp (line, "NEXTZ", 5)) {
			sscanf (line+5, "%d", &NextZ);
		}
		else if (!strnicmp (line, "SCALERCOUNTER", 13)) {
			sscanf (line+13, "%d", &ScalerCounter);
		}
		else if (!strnicmp (line, "CRCOUNT", 7)) {
			sscanf (line+7, "%d", &ChannelRoutineCount);
		}
		else if (!strnicmp (line, "CH33SWITCHES", 12)) {
			sscanf (line+12, "%" SCNd16, &vagc.Ch33Switches);
		}
		/*
		TODO Do NOT load CycleCounter until CduFifos are saved/loaded, too
		else if (!strnicmp (line, "CYCLECOUNTER", 12)) {
			sscanf (line+12, "%I64d", &vagc.CycleCounter);
		}
		*/
		else if (!strnicmp (line, "VINT", 4)) {
			int num;
			unsigned int val;
			sscanf(line+4, "%d", &num);
			sscanf(line+8, "%d", &val);
			vagc.InterruptRequests[num] = val;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			AGCState state;
			sscanf (line+5, "%d", &state.word);

			Reset = state.u.Reset;
			isFirstTimestep = (state.u.isFirstTimestep != 0);
			vagc.ExtraCode = state.u.ExtraCode;
			vagc.AllowInterrupt = state.u.AllowInterrupt;
			vagc.InIsr = state.u.InIsr;
			vagc.SubstituteInstruction = state.u.SubstituteInstruction;
			vagc.PendFlag = state.u.PendFlag;
			vagc.PendDelay = state.u.PendDelay;
			vagc.ExtraDelay = state.u.ExtraDelay;
			vagc.DownruptTimeValid = state.u.DownruptTimeValid;
			PadLoaded = state.u.PadLoaded;
			vagc.NightWatchman = state.u.NightWatchman;
			vagc.RuptLock = state.u.RuptLock;
			vagc.NoRupt = state.u.NoRupt;
			vagc.TCTrap = state.u.TCTrap;
			vagc.NoTC = state.u.NoTC;
			vagc.Standby = state.u.Standby;
			vagc.SbyPressed = state.u.SbyPressed;
			vagc.SbyStillPressed = state.u.SbyStillPressed;
			vagc.ParityFail = state.u.ParityFail;
			vagc.CheckParity = state.u.CheckParity;
		}
		else if (!strnicmp (line, "ONAME", 5)) {
			strncpy (OtherVesselName, line + 6, 64);
		}

		papiReadScenario_bool(line, "PROGALARM", ProgAlarm);
		papiReadScenario_bool(line, "GIMBALLOCKALARM", GimbalLockAlarm);
	}

	//
	// Quick hack to make the code work with old scenario files. Can be removed after NASSP 7
	// release.
	//

	if (!OtherVesselName[0] && OurVessel) {
		strncpy (OtherVesselName, OurVessel->GetName(), 63);
		OtherVesselName[6] = 0;
	}
}

//
// Power.
//

bool ApolloGuidance::IsPowered()

{
	if (DCPower.Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	//
	// Quick hack for now: if no power connected, pretend we
	// have power.
	//

	if (!PowerConnected)
		return true;

	return false;
}

//
// I/O channel support code.
//
// Note that the AGC 'bit 1' is actually 'bit 0' in today's terminology, so we have
// to adjust the bit number here to match the real AGC.
//

bool ApolloGuidance::GetOutputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return false;

	int val = OutputChannel[channel];
	return (OutputChannel[channel] & (1 << (bit))) != 0;
}

unsigned int ApolloGuidance::GetOutputChannel(int channel)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return 0;

	return OutputChannel[channel];
}

void ApolloGuidance::SetInputChannel(int channel, std::bitset<16> val) 
{
	if (channel >= 0 && channel <= MAX_INPUT_CHANNELS)
		InputChannel[channel] = val.to_ulong();

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

#ifdef _DEBUG
	//
	// Don't print debug for IMU channels or we get a multi-gigabyte log file!
	//
	if (!(channel & 0x80))
		fprintf(out_file, "Wrote %05o to input channel %04o\n", channel, val);
#endif

	if (channel & 0x80) {
		// In this case we're dealing with a counter increment.
		// So increment the counter.
		UnprogrammedIncrement (&vagc, channel, val.to_ulong());
	}
	else {
		// If this is a keystroke from the DSKY, generate an interrupt req.
		if (channel == 015){
			vagc.InterruptRequests[5] = 1;
		}else{ if (channel == 016){ // Secondary DSKY
			vagc.InterruptRequests[6] = 1;
		}}

		//
		// Channels 030-034 are inverted!
		//

		if (channel >= 030 && channel <= 034){
			val ^= 077777;
		}

		WriteIO(&vagc, channel, val.to_ulong());
	}
}

void ApolloGuidance::SetInputChannelBit(int channel, int bit, bool val)

{
	unsigned int mask = (1 << (bit));
	int	data = InputChannel[channel];

	data = vagc.InputChannel[channel];
	//
	// Channels 030-034 are inverted!
	//

	if ((channel >= 030) && (channel <= 034))
		data ^= 077777;

#ifdef _DEBUG
		fprintf(out_file, "Set bit %d of input channel %04o to %d\n", bit, channel, val ? 1 : 0); 
#endif

	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return;

	if (val) {
		data |= mask;
	}
	else {
		data &= ~mask;
	}

	InputChannel[channel] = data;

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	//
	// Channels 030-034 are inverted!
	//

	if ((channel >= 030) && (channel <= 034))
		data ^= 077777;

	// Channel 33 special hack
	if(channel == 033){
		if(bit == 10){
			// Update channel 33 switch bits
			int ch33bits = GetCh33Switches();
			if(val != 0){ ch33bits |= 001000; }else{ ch33bits &= 076777; }
				SetCh33Switches(ch33bits);
			// We're done here. SetCh33Switches rewrites the IO channel.
			return;
		}
	}

	// If this is a keystroke from the DSKY (Or MARK/MARKREJ), generate an interrupt req.
	if (channel == 015 && val != 0){
		vagc.InterruptRequests[5] = 1;
	}else{ if (channel == 016 && val != 0){ // Secondary DSKY
		vagc.InterruptRequests[6] = 1;
	}}

	WriteIO(&vagc, channel, data);
}

void ApolloGuidance::SetOutputChannel(int channel, ChannelValue val)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return;

	OutputChannel[channel] = val.to_ulong();

#ifdef _DEBUG
	if (Yaagc) {
		switch (channel) {
		case 010:
		case 034:
		case 035:
		case 01:
		case 02:
			break;

		default:
			fprintf(out_file, "AGC write %05o to %04o\n", val, channel);
			break;
		}
	}
#endif

	//
	// Special-case processing.
	//

	switch (channel)
	{
	case 05:
		ProcessChannel5(val);
		break;

	case 06:
		ProcessChannel6(val);
		break;

	case 010:
		ProcessChannel10(val);
		break;

	case 011:
		ProcessChannel11(val);
		break;

	case 013:
		ProcessChannel13(val);
		break;

	case 014:
		{
			// DS20060225 Enable SPS gimbal control
			// TVC Enable does not disconnect the IMU from this channel			
			// (Even though it probably doesn't matter)
			imu.ChannelOutput(channel, val);
			// DS20060829 Allow other stuff too
			ProcessChannel14(val);
		}
		break;

	// Various control bits
	case 012:		
	// 174-177 are ficticious channels with the IMU CDU angles.
	case 0174:  // FDAI ROLL CHANNEL
	case 0175:  // FDAI PITCH CHANNEL
	case 0176:  // FDAI YAW CHANNEL
	case 0177:		
		ProcessIMUCDUErrorCount(channel, val);
		imu.ChannelOutput(channel, val);
		break;

	// DS20060225 Enable SPS gimbal control
	// Ficticious channels 140 & 141 have the optics shaft & trunion angles.
	case 0140:
		ProcessChannel140(val);
		break;

	case 0141:
		ProcessChannel141(val);
		break;
	case 0142:
		ProcessChannel142(val);
		break;
	case 0143:
		ProcessChannel143(val);
		break;
	case 0163:	//Virtual AGC DSKY stuff
		ProcessChannel163(val);
		break;

	case 033: 
		/* No longer needed DS20061226
		{			
			ChannelValue33 val33;
			val33.Value = val;
		} */
		break;
	}
}

//
// By default, do nothing for the RCS channels.
//

void ApolloGuidance::ProcessChannel5(ChannelValue val){
}

void ApolloGuidance::ProcessChannel6(ChannelValue val){
}

// DS20060226 Stubs for optics controls and TVC
void ApolloGuidance::ProcessChannel14(ChannelValue val){
}

void ApolloGuidance::ProcessChannel140(ChannelValue val){
}

void ApolloGuidance::ProcessChannel141(ChannelValue val){
}

// Stub for LGC thrust drive
void ApolloGuidance::ProcessChannel142(ChannelValue val) {
}

// Stub for LGC altitude meter drive
void ApolloGuidance::ProcessChannel143(ChannelValue val) {
}

// DS20060308 Stub for FDAI
void ApolloGuidance::ProcessIMUCDUErrorCount(int channel, ChannelValue val){
}

void ApolloGuidance::GenerateHandrupt() {
	GenerateHANDRUPT(&vagc);
}

// DS20060402 DOWNRUPT
void ApolloGuidance::GenerateDownrupt(){
	GenerateDOWNRUPT(&vagc);
}

void ApolloGuidance::GenerateUprupt(){
	GenerateUPRUPT(&vagc);
}

void ApolloGuidance::GenerateRadarupt(){
	GenerateRADARUPT(&vagc);
}

bool ApolloGuidance::IsUpruptActive() {
	return (IsUPRUPTActive(&vagc) == 1);
}

// DS200608xx CH33 SWITCHES
void ApolloGuidance::SetCh33Switches(unsigned int val){
	if( isLGC)
		SetLMCh33Bits(&vagc,val);
	else 
		SetCh33Bits(&vagc,val);
}

unsigned int ApolloGuidance::GetCh33Switches(){
	return vagc.Ch33Switches; 
}


// DS20060903 PINC, DINC, ETC
int ApolloGuidance::DoPINC(int16_t *Counter){
	return(CounterPINC(Counter));
}

int ApolloGuidance::DoPCDU(int16_t *Counter){
	return(CounterPCDU(Counter));
}

int ApolloGuidance::DoMCDU(int16_t *Counter){
	return(CounterMCDU(Counter));
}

int ApolloGuidance::DoDINC(int CounterNum, int16_t *Counter){
	return(CounterDINC(&vagc,CounterNum,Counter));
}


void ApolloGuidance::SetOutputChannelBit(int channel, int bit, bool val)

{
	unsigned int mask = (1 << (bit));

	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return;

	if (val) {
		OutputChannel[channel] |= mask;
	}
	else {
		OutputChannel[channel] &= ~mask;
	}

	//
	// Special-case processing.
	//

	switch (channel)
	{
	case 05:
		ProcessChannel5(OutputChannel[05]);
		break;

	case 06:
		ProcessChannel6(OutputChannel[06]);
		break;

	case 010:
		ProcessChannel10(OutputChannel[010]);
		break;

	case 011:
		ProcessChannel11Bit(bit, val);
		break;

	case 012:
	case 014:
		imu.ChannelOutput(channel, OutputChannel[channel]);
		break;
	}
}

bool ApolloGuidance::GetInputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return false;

	return (GetInputChannel(channel) & (1 << (bit))) != 0;
}

unsigned int ApolloGuidance::GetInputChannel(int channel)

{
	if (channel < 0 || channel >= NUM_CHANNELS)
		return 0;

	//
	// Virtual AGC code stores values in native form. C++ AGC expects to read them out in
	// 0 = false, 1 = true form.
	//

	unsigned int val = vagc.InputChannel[channel];
	
	if ((channel >= 030) && (channel <= 034))
		val ^= 077777;

	return val;
}

void ApolloGuidance::KillAllThrusters()
{
	OurVessel->SetAttitudeLinLevel(0, 0);
	OurVessel->SetAttitudeLinLevel(1, 0);
	OurVessel->SetAttitudeLinLevel(2, 0);
	OurVessel->SetAttitudeRotLevel(0, 0);
	OurVessel->SetAttitudeRotLevel(1, 0);
	OurVessel->SetAttitudeRotLevel(2, 0);
}

void ApolloGuidance::SetDesiredLanding(double latitude, double longitude, double altitude)

{
	LandingAltitude = altitude;
	LandingLongitude = longitude;
	LandingLatitude = latitude;
}

bool ApolloGuidance::GenericReadMemory(unsigned int loc, int &val)

{
	int bank, addr;

	bank = (loc / 0400);
	addr = loc - (bank * 0400);

	if (bank >= 0 && bank < 8) {
		val = vagc.Erasable[bank][addr];
		return true;
	}

	val = 0;
	return true;

}

//
// Load a PAD value into the AGC. Used for initialising the LEM when created.
//

void ApolloGuidance::PadLoad(unsigned int address, unsigned int value)

{
	WriteMemory(address, value);
}

void ApolloGuidance::GenericWriteMemory(unsigned int loc, int val)

{
	int bank, addr;

	bank = (loc / 0400);
	addr = loc - (bank * 0400);

	if (bank >= 0 && bank < 8)
		vagc.Erasable[bank][addr] = val;
	return;
}

int16_t ApolloGuidance::ConvertDecimalToAGCOctal(double x, bool highByte) 

{
	int sign = 0, value = 0, i = 0;

	if (x < 0) {
		sign = 1;
		x = -x;
    }
	
	if (x >= 1.0) {
		// Illegal value: Must be <1.0 according to AGC rules
		return -1;
    }

	for (value = 0, i = 0; i < 28; i++) {
		value = value << 1;
		if (x >= 0.5) {
			value++;
			x -= 0.5;
		}
		x *= 2;
    }
  
	if (x >= 0.5)
		value++;

	i = value & 0x00003fff;
	value = (value >> 14) & 0x00003fff;
	if (sign) {
		value = ~value;
		i = ~i;
		i &= 0x00007fff;
		value &= 0x00007fff;
	}

	if (highByte)
		return value;
	else
		return i;
}


//
// Virtual AGC functions.
//


//-----------------------------------------------------------------------------
// Function for broadcasting "output channel" data to all connected clients.

void ChannelOutput (agc_t * State, int Channel, int Value) 

{

  // Some output channels have purposes within the CPU, so we have to
  // account for those separately.
  if (Channel == 7)
    {
      State->InputChannel[7] = State->OutputChannel7 = (Value & 0160);
      return;
    }
  // Most output channels are simply transmitted to clients representing
  // hardware simulations.

  ApolloGuidance *agc;

  agc = (ApolloGuidance *) State->agc_clientdata;
  agc->SetOutputChannel(Channel, Value);
}

void ShiftToDeda (agc_t *State, int Data)

{
	// Nothing for now.
}

//
// Do nothing here. We'll process input seperately.
//

int ChannelInput (agc_t *State)

{
	return 0;
}

void ChannelRoutine (agc_t *State)

{
}

