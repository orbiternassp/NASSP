/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Saturn CSM computer

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
#include "stdio.h"
#include "math.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "toggleswitch.h"
#include "saturn.h"
#include "ioChannels.h"
#include "papi.h"
#include "thread.h"

CSMcomputer::CSMcomputer(SoundLib &s, DSKY &display, DSKY &display2, IMU &im, CDU &sc, CDU &tc, PanelSDK &p) :
	ApolloGuidance(s, display, im, sc, tc, p), dsky2(display2)

{
	isLGC = false;

	//
	// Last RCS settings.
	//

	LastOut5 = 0;
	LastOut6 = 0;
	LastOut11 = 0;

	thread.Resume ();
}

CSMcomputer::~CSMcomputer()

{
	//
	// Nothing for now.
	//
}

void CSMcomputer::SetMissionInfo(std::string AGCVersion, char *OtherVessel)

{
	ApolloGuidance::SetMissionInfo(AGCVersion, OtherVessel);

	//
	// Pick the appropriate AGC binary file based on name.
	//
	//

	char Buffer[100];
	sprintf(Buffer, "Config/ProjectApollo/%s.bin", AGCVersion.c_str());

	InitVirtualAGC(Buffer);
}

void CSMcomputer::agcTimestep(double simt, double simdt)
{
	// Do single timesteps to maintain sync with telemetry engine
	SingleTimestepPrep(simt, simdt);        // Setup
	if (LastCycled == 0) {					// Use simdt as difference if new run
		LastCycled = (simt - simdt); 
		sat->pcm.last_update = LastCycled;
	}	  
	double ThisTime = LastCycled;			// Save here
	
	long cycles = (long)((simt - LastCycled) / 0.00001171875);	// Get number of CPU cycles to do
	LastCycled += (0.00001171875 * cycles);						// Preserve the remainder
	long x = 0; 
	while(x < cycles) {
		SingleTimestep();
		ThisTime += 0.00001171875;								// Add time
		if((ThisTime - sat->pcm.last_update) > 0.00015625) {	// If a step is needed
			sat->pcm.TimeStep(ThisTime);						// do it
		}
		x++;
	}
}

void CSMcomputer::Run ()
	{
		while(true)
		{
			timeStepEvent.Wait();
			{
				Lock lock(agcCycleMutex);
				agcTimestep(thread_simt,thread_simdt);
			}
		}
	};


void CSMcomputer::Timestep(double simt, double simdt)

{
	// DS20060302 For joystick stuff below
	sat = (Saturn *) OurVessel;

		//
		// Reduce time acceleration as per configured, not to jump to x100 or x1000 and freeze the simulation
		//
		
		if( sat->maxTimeAcceleration>0 )
		{
			if( oapiGetTimeAcceleration() > (double)sat->maxTimeAcceleration )
				oapiSetTimeAcceleration(sat->maxTimeAcceleration);
		}
		
		//
		// Do nothing if we have no power. (vAGC)
		//
		if (!IsPowered()) {
			// HARDWARE MUST RESTART

			// Clear flip-flop based registers
			vagc.Erasable[0][00] = 0;     // A
			vagc.Erasable[0][01] = 0;     // L
			vagc.Erasable[0][02] = 0;     // Q
			vagc.Erasable[0][03] = 0;     // EB
			vagc.Erasable[0][04] = 0;     // FB
			vagc.Erasable[0][05] = 04000; // Z
			vagc.Erasable[0][06] = 0;     // BB
			// Clear ISR flag
			vagc.InIsr = 0;
			// Clear interrupt requests
			vagc.InterruptRequests[0] = 0;
			vagc.InterruptRequests[1] = 0;
			vagc.InterruptRequests[2] = 0;
			vagc.InterruptRequests[3] = 0;
			vagc.InterruptRequests[4] = 0;
			vagc.InterruptRequests[5] = 0;
			vagc.InterruptRequests[6] = 0;
			vagc.InterruptRequests[7] = 0;
			vagc.InterruptRequests[8] = 0;
			vagc.InterruptRequests[9] = 0;
			vagc.InterruptRequests[10] = 0;
			// Reset cycle counter and Extracode flags
			vagc.CycleCounter = 0;
			vagc.ExtraCode = 0;
			vagc.ExtraDelay = 2; // GOJAM and TC 4000 both take 1 MCT to execute
			// No idea about the interrupts/pending/etc so we reset those
			vagc.AllowInterrupt = 1;
			vagc.PendFlag = 0;
			vagc.PendDelay = 0;
			// Don't disturb erasable core
			// IO channels are flip-flop based and should reset, but that's difficult, so we'll ignore it.
			// Reset standby flip-flop
			vagc.Standby = 0;
			// Turn on EL display and CMC Light (DSKYWarn).
			vagc.DskyChannel163 = 1;
			SetOutputChannel(0163, 1);
			// Light OSCILLATOR FAILURE to signify power transient, and be forceful about it.
			vagc.InputChannel[033] &= 037777;
			OutputChannel[033] &= 037777;
			// Also, simulate the operation of the VOLTAGE ALARM, turn off STBY and RESTART light while power is off.
			// The RESTART light will come on as soon as the AGC receives power again.
			// This happens externally to the AGC program. See CSM 104 SYS HBK pg 399
			vagc.RestartLight = 1;
			dsky.ClearRestart();
			dsky2.ClearRestart();
			dsky.ClearStby();
			dsky2.ClearStby();
			// Reset last cycling time
			LastCycled = 0;

			// We should issue telemetry though.
			sat->pcm.TimeStep(simt);
			return;
		}

		//
		// Initial startup hack for Yaagc.
		//
		if(!PadLoaded) {

			double latitude, longitude, radius, heading, TEPHEM0;

			// init pad load
			OurVessel->GetEquPos(longitude, latitude, radius);
			oapiGetHeading(OurVessel->GetHandle(), &heading);

			// set launch pad latitude
			vagc.Erasable[5][2] = ConvertDecimalToAGCOctal(latitude / TWO_PI, true);
			vagc.Erasable[5][3] = ConvertDecimalToAGCOctal(latitude / TWO_PI, false);

			if (ProgramName == "Colossus237" || ProgramName == "Colossus249" || ProgramName == "Comanche055NBY69")
			{
				// set launch pad longitude
				if (longitude < 0) { longitude += TWO_PI; }
				vagc.Erasable[2][0263] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0264] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0272] = 01;	// 17.7 nmi
				vagc.Erasable[2][0272] = 0;
				vagc.Erasable[2][0273] = (int16_t)(0.5 * OurVessel->GetAltitude());

				TEPHEM0 = 40038.;
			}
			else if (ProgramName == "Comanche055")	// Comanche 055
			{
				// set launch pad longitude
				if (longitude < 0) { longitude += TWO_PI; }
				vagc.Erasable[2][0263] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0264] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0272] = 01;	// 17.7 nmi
				vagc.Erasable[2][0272] = 0;
				vagc.Erasable[2][0273] = (int16_t)(0.5 * OurVessel->GetAltitude());

				TEPHEM0 = 40403.;
			}
			else if (ProgramName == "Artemis072NBY71")	//Artemis 072 for Apollo 14
			{
				// set launch pad longitude
				if (longitude < 0) longitude += TWO_PI;
				vagc.Erasable[2][0135] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0136] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0133] = 01;	// 17.7 nmi
				vagc.Erasable[2][0133] = 0;
				vagc.Erasable[2][0134] = (int16_t)(0.5 * OurVessel->GetAltitude());

				TEPHEM0 = 40768.;
			}
			else	//Artemis 072
			{
				// set launch pad longitude
				if (longitude < 0) longitude += TWO_PI;
				vagc.Erasable[2][0135] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0136] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0133] = 01;	// 17.7 nmi
				vagc.Erasable[2][0133] = 0;
				vagc.Erasable[2][0134] = (int16_t)(0.5 * OurVessel->GetAltitude());

				TEPHEM0 = 41133.;
			}

			// Synchronize clock with launch time (TEPHEM)
			double tephem = vagc.Erasable[AGC_BANK(01710)][AGC_ADDR(01710)] +
				vagc.Erasable[AGC_BANK(01707)][AGC_ADDR(01707)] * pow((double) 2., (double) 14.) +
				vagc.Erasable[AGC_BANK(01706)][AGC_ADDR(01706)] * pow((double) 2., (double) 28.);
			tephem = (tephem / 8640000.) + TEPHEM0;
			double clock = (oapiGetSimMJD() - tephem) * 8640000. * pow((double) 2., (double)-28.);
			vagc.Erasable[AGC_BANK(024)][AGC_ADDR(024)] = ConvertDecimalToAGCOctal(clock, true);
			vagc.Erasable[AGC_BANK(025)][AGC_ADDR(025)] = ConvertDecimalToAGCOctal(clock, false);

			PadLoaded = true;
		}

		//
		// If MultiThread is enabled and the simulation is accellerated, the run vAGC in the AGC Thread,
		// otherwise run in main thread. at x1 acceleration, it is better to run vAGC totally synchronized
		//
		if(sat->IsMultiThread && oapiGetTimeAcceleration() > 1.0)
		{
			
			Lock lock(agcCycleMutex);
			thread_simt = simt;
			thread_simdt = simdt;
			timeStepEvent.Raise();
		}
		else
			agcTimestep(simt,simdt);

		//
		// Check nonspherical gravity sources
		//
		if (!OurVessel->NonsphericalGravityEnabled()) {
			sprintf(oapiDebugString(), "*** PLEASE ENABLE NONSPHERICAL GRAVITY SOURCES ***");
		}
		// Done!
		//sprintf(oapiDebugString(), "Standby: %d %d %I64d", sat->agc.vagc.Standby, sat->agc.vagc.SbyPressed, sat->agc.vagc.CycleCounter);

		return;
}

//
// Access simulated erasable memory.
//

bool CSMcomputer::ReadMemory(unsigned int loc, int &val)

{
	return GenericReadMemory(loc, val);
}

void CSMcomputer::WriteMemory(unsigned int loc, int val)

{
	GenericWriteMemory(loc, val);
}

void CSMcomputer::SetInputChannelBit(int channel, int bit, bool val){
	ApolloGuidance::SetInputChannelBit(channel, bit, val);
}

void CSMcomputer::SetOutputChannel(int channel, ChannelValue val){
	ApolloGuidance::SetOutputChannel(channel, val);
}

//
// We need to pass these I/O channels to both DSKYs.
//

void CSMcomputer::ProcessChannel10(ChannelValue val){
	dsky.ProcessChannel10(val);
	dsky2.ProcessChannel10(val);

	// Gimbal Lock & Prog alarm
	ChannelValue10 val10;
	val10.Value = val.to_ulong();
	if (val10.Bits.a == 12) {
		// Gimbal Lock
		GimbalLockAlarm = ((val10.Value & (1 << 5)) != 0);
		// Tracker alarm
		TrackerAlarm = ((val10.Value & (1 << 7)) != 0);
		// Prog alarm
		ProgAlarm = ((val10.Value & (1 << 8)) != 0);
	}
}

void CSMcomputer::ProcessChannel11Bit(int bit, bool val){
	dsky.ProcessChannel11Bit(bit, val);
	dsky2.ProcessChannel11Bit(bit, val);

	LastOut11 = GetOutputChannel(011);
}

void CSMcomputer::ProcessChannel11(ChannelValue val){
	dsky.ProcessChannel11(val);
	dsky2.ProcessChannel11(val);

	LastOut11 = val.to_ulong();
}

//
// Process RCS channels
//

void CSMcomputer::ProcessChannel5(ChannelValue val){
	ChannelValue val30;
	val30 = GetInputChannel(030);

	Saturn *sat = (Saturn *) OurVessel;
	if ((sat->SCContSwitch.IsDown() && sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE) || (sat->THCRotary.IsClockwise() && sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE)) {
		return;
	}

	CSMOut5 Current;
	CSMOut5 Changed;

	//
	// Get the current state and a mask of any changed state.
	//
	
	Current.word = val.to_ulong();
	Changed.word = (val.to_ulong() ^ LastOut5);	

	//
	// Update any thrusters that have changed.
	//

	if (Changed.u.SMA3) {
		sat->rjec.SetThruster(3,Current.u.SMA3 != 0);
	}
	if (Changed.u.SMA4) {
		sat->rjec.SetThruster(2,Current.u.SMA4 != 0);
	}

	if (Changed.u.SMB3) {
		sat->rjec.SetThruster(7,Current.u.SMB3 != 0);
	}
	if (Changed.u.SMB4) {
		sat->rjec.SetThruster(6,Current.u.SMB4 != 0);
	}

	if (Changed.u.SMC3) {
		sat->rjec.SetThruster(1,Current.u.SMC3 != 0);
	}
	if (Changed.u.SMC4) {
		sat->rjec.SetThruster(4,Current.u.SMC4 != 0);
	}

	if (Changed.u.SMD3) {
		sat->rjec.SetThruster(5,Current.u.SMD3 != 0);
	}
	if (Changed.u.SMD4) {
		sat->rjec.SetThruster(8,Current.u.SMD4 != 0);
	}

	LastOut5 = val.to_ulong();
}

void CSMcomputer::ProcessChannel6(ChannelValue val){
	ChannelValue val30;
	val30 = GetInputChannel(030);

	Saturn *sat = (Saturn *) OurVessel;	
	if ((sat->SCContSwitch.IsDown() && sat->SCSLogicBus3.Voltage() > SP_MIN_DCVOLTAGE) || (sat->THCRotary.IsClockwise() && sat->SCSLogicBus2.Voltage() > SP_MIN_DCVOLTAGE)) {
		return;
	}

	CSMOut6 Current;
	CSMOut6 Changed;

	//
	// Get the current state and a mask of any changed state.
	//

	Current.word = val.to_ulong();
	Changed.word = (val.to_ulong() ^ LastOut6);	

	//
	// Update any thrusters that have changed.
	//

	if (Changed.u.SMA1) {
		sat->rjec.SetThruster(13,Current.u.SMA1 != 0);
	}
	if (Changed.u.SMA2) {
		sat->rjec.SetThruster(14,Current.u.SMA2 != 0);
	}

	if (Changed.u.SMB1) {
		sat->rjec.SetThruster(9,Current.u.SMB1 != 0);
	}
	if (Changed.u.SMB2) {
		sat->rjec.SetThruster(12,Current.u.SMB2 != 0);
	}

	if (Changed.u.SMC1) {
		sat->rjec.SetThruster(15,Current.u.SMC1 != 0);
	}
	if (Changed.u.SMC2) {
		sat->rjec.SetThruster(16,Current.u.SMC2 != 0);
	}

	if (Changed.u.SMD1) {
		sat->rjec.SetThruster(11,Current.u.SMD1 != 0);
	}
	if (Changed.u.SMD2) {
		sat->rjec.SetThruster(10,Current.u.SMD2 != 0);
	}

	LastOut6 = val.to_ulong();
}

void CSMcomputer::ProcessIMUCDUReadCount(int channel, int val) {
	SetErasable(0, channel, val);
}

// DS20060308 FDAI
void CSMcomputer::ProcessIMUCDUErrorCount(int channel, ChannelValue val){
	// These pulses work like the TVC pulses.
	// FULL NEEDLE DEFLECTION is 16.88 DEGREES
	// 030 PULSES = MAX IN ONE RELAY EVENT
	// 22 PULSES IS ONE DEGREE, 384 PULSES = FULL SCALE
	// 0.10677083 PIXELS PER PULSE

	Saturn *sat = (Saturn *) OurVessel;
	ChannelValue val12;
	if(channel != 012){ val12 = GetOutputChannel(012); }else{ val12 = val; }
	// 174 = X, 175 = Y, 176 = Z
	if(val12[CoarseAlignEnable]){ return; } // Does not apply to us here.
	switch(channel){
	case 012:
		// Reset FDAI
		if (val12[EnableIMUCDUErrorCounters]) {
			if (sat->gdc.fdai_err_ena == 0) {
				// sprintf(oapiDebugString(),"FDAI: RESET");						
				sat->gdc.fdai_err_x = 0;
				sat->gdc.fdai_err_y = 0;
				sat->gdc.fdai_err_z = 0;
				sat->gdc.fdai_err_ena = 1;
			}
		} else {
			if (sat->gdc.fdai_err_ena == 1) {
				// sprintf(oapiDebugString(),"FDAI: RESET");
				sat->gdc.fdai_err_x = 0;
				sat->gdc.fdai_err_y = 0;
				sat->gdc.fdai_err_z = 0;
			}
			sat->gdc.fdai_err_ena = 0;
		}

		break;
		
	case 0174: // FDAI ROLL ERROR
		if(val12[EnableIMUCDUErrorCounters]){
			int delta = (val.to_ulong()&0777);
			// Direction for these is inverted.
			if(val.to_ulong()&040000){
				sat->gdc.fdai_err_x += delta;
			}else{
				sat->gdc.fdai_err_x -= delta;
			}
		}
//		sprintf(oapiDebugString(),"FDAI: NEEDLES: %d %d %d",sat->gdc.fdai_err_x,sat->gdc.fdai_err_y,sat->gdc.fdai_err_z);
		break;
	
	case 0175: // FDAI PITCH ERROR
		if(val12[EnableIMUCDUErrorCounters]){
			int delta = val.to_ulong()&0777;
			if(val.to_ulong()&040000){
				sat->gdc.fdai_err_y -= delta;
			}else{
				sat->gdc.fdai_err_y += delta;
			}
		}
//		sprintf(oapiDebugString(),"FDAI: NEEDLES: %d %d %d",sat->gdc.fdai_err_x,sat->gdc.fdai_err_y,sat->gdc.fdai_err_z);
		break;

	case 0176: // FDAI YAW ERROR
		if(val12[EnableIMUCDUErrorCounters]){
			int delta = val.to_ulong()&0777;
			if(val.to_ulong()&040000){
				sat->gdc.fdai_err_z += delta;
			}else{
				sat->gdc.fdai_err_z -= delta;
			}
		}
//		sprintf(oapiDebugString(),"FDAI: NEEDLES: %d %d %d",sat->gdc.fdai_err_x,sat->gdc.fdai_err_y,sat->gdc.fdai_err_z);
		break;
	}
}

void CSMcomputer::ProcessChannel14(ChannelValue val){
	// This entire deal is no longer necessary, but we'll leave the stub here in case it's needed later.
	/*
	ChannelValue12 val12;
	ChannelValue14 val14;
	val12.Value = GetOutputChannel(012);
	val14.Value = val;
	Saturn *sat = (Saturn *) OurVessel;	

	if(val12.Bits.TVCEnable){
		return; // Ignore
	} */
}

VESSEL *CSMcomputer::GetLM()
{
	OBJHANDLE hcsm = oapiGetVesselByName(OtherVesselName);
	if (hcsm)
	{
		VESSEL *LMVessel = oapiGetVesselInterface(hcsm);
		return LMVessel;
	}

	return NULL;
}


//
// CM Optics class code
//

CMOptics::CMOptics() {

	sat = NULL;
	OpticsShaft = 0.0;
	SextTrunion = 0.0;
	TeleTrunion = 0.0;
	dTrunion = 0.0;
	dShaft = 0.0;
	OpticsManualMovement = 0;
	Powered = 0;
	SextDualView = false;
	SextDVLOSTog = false;
	SextDVTimer = 0.0;
	OpticsCovered = true;
}

void CMOptics::Init(Saturn *vessel) {

	sat = vessel;
}

void CMOptics::SystemTimestep(double simdt) {

	// Optics system apparently uses 124.4 watts of power to operate.
	// This should probably vary up and down when the motors run, but I couldn't find data for it.
	Powered = 0; // Reset
	if (sat->GNOpticsMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE){
		Powered |= 1;
	}
	if (sat->GNOpticsMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE){
		Powered |= 2;
	}
	if (sat->GNPowerOpticsSwitch.IsDown()) {
		Powered = 0;
	}
	switch(Powered){
		case 0: // OFF
			break;
		case 1: // MNA
			sat->GNOpticsMnACircuitBraker.DrawPower(124.4);
			break;
		case 2: // MNB
			sat->GNOpticsMnBCircuitBraker.DrawPower(124.4);
			break;
		case 3: // BOTH
			sat->GNOpticsMnACircuitBraker.DrawPower(62.2);
			sat->GNOpticsMnBCircuitBraker.DrawPower(62.2);
			break;
	}

}

// Paint counters. The documentation is not clear if the displayed number is supposed to be decimal degrees or CDU counts.
// The counters are mechanically connected to the telescope, so it is assumed to be decimal degrees.

bool CMOptics::PaintShaftDisplay(SURFHANDLE surf, SURFHANDLE digits){
	int value = (int)(OpticsShaft*100.0*DEG);
	if (value < 0) { value += 36000; }
	return PaintDisplay(surf, digits, value);
}

bool CMOptics::PaintTrunnionDisplay(SURFHANDLE surf, SURFHANDLE digits){
	int value = (int)(TeleTrunion*1000.0*DEG);
	if (value < 0) { value += 36000; }
	return PaintDisplay(surf, digits, value);
}

bool CMOptics::PaintDisplay(SURFHANDLE surf, SURFHANDLE digits, int value){
	int srx, sry, digit[5];
	int x=value;	
	digit[0] = (x%10);
	digit[1] = (x%100)/10;
	digit[2] = (x%1000)/100;
	digit[3] = (x%10000)/1000;
	digit[4] = x/10000;
	sry = (int)(digit[0] * 1.2);
	srx = 8 + (digit[4] * 25);
	oapiBlt(surf, digits, 0, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8 + (digit[3] * 25);
	oapiBlt(surf, digits, 10, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8 + (digit[2] * 25);
	oapiBlt(surf, digits, 20, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8 + (digit[1] * 25);
	oapiBlt(surf, digits, 30, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	srx = 8 + (digit[0] * 25);
	oapiBlt(surf, digits, 40, 0, srx, 33, 9, 12, SURF_PREDEF_CK);
	return true;
}

void CMOptics::OpticsSwitchToggled()
{
	if (sat->OpticsZeroSwitch.IsUp())
	{
		sat->agc.SetInputChannelBit(033, ZeroOptics_33, true);
	}
	else
	{
		sat->agc.SetInputChannelBit(033, ZeroOptics_33, false);
	}
	if (sat->OpticsModeSwitch.IsUp() && sat->OpticsZeroSwitch.IsDown())
	{
		sat->agc.SetInputChannelBit(033, CMCControl, true);
	}
	else
	{
		sat->agc.SetInputChannelBit(033, CMCControl, false);
	}
}

void CMOptics::TimeStep(double simdt) {

	double ShaftRate = 0;
	double TrunRate = 0;

	SextDVTimer = SextDVTimer+simdt;
	if (SextDVTimer >= 0.06666){
		SextDVTimer = 0.0;
		SextDVLOSTog=!SextDVLOSTog;
	}

	// Optics cover handling
	if (OpticsCovered && sat->GetStage() >= STAGE_ORBIT_SIVB) {
		if (OpticsShaft > 150. * RAD) {
			OpticsCovered = false;			
			sat->SetOpticsCoverMesh();
			sat->JettisonOpticsCover();
		}
	}

	if (Powered == 0) { return; }

	//Rates
	if (sat->OpticsZeroSwitch.IsUp())
	{
		//Optics zero speed is twice the angle, limit to max drive rate
		ShaftRate = min(abs(2.0*OpticsShaft), 19.5*RAD);
		TrunRate = min(abs(2.0*SextTrunion), 10.0*RAD);
	}
	else
	{
		// Generate rates for telescope and manual mode
		switch (sat->ControllerSpeedSwitch.GetState()) {
		case THREEPOSSWITCH_UP:       // HI
			ShaftRate = 19.5*RAD;
			TrunRate = 10.0*RAD;
			break;
		case THREEPOSSWITCH_CENTER:   // MED
			ShaftRate = 2.0*RAD;
			TrunRate = 1.0*RAD;
			break;
		case THREEPOSSWITCH_DOWN:     // LOW
			ShaftRate = 0.2*RAD;
			TrunRate = 0.1*RAD;
			break;
		}
	}

	dTrunion = 0.0;
	dShaft = 0.0;

	//ZERO OPTICS
	if (sat->OpticsZeroSwitch.IsUp())
	{
		if (OpticsShaft > 0)
		{
			dShaft = -ShaftRate * simdt;
		}
		else
		{
			dShaft = ShaftRate * simdt;
		}
		if (SextTrunion > 0)
		{
			dTrunion = -TrunRate * simdt;
		}
		else
		{
			dTrunion = TrunRate * simdt;
		}
	}
	else
	{
		// MANUAL
		if (sat->OpticsModeSwitch.IsDown())
		{
			double A_t_dot, A_s_dot;
			A_t_dot = 0.0;
			A_s_dot = 0.0;

			if ((OpticsManualMovement & 0x01) != 0) {
				A_t_dot = TrunRate * simdt;
			}
			if ((OpticsManualMovement & 0x02) != 0) {
				A_t_dot = -TrunRate * simdt;
			}
			if ((OpticsManualMovement & 0x04) != 0) {
				A_s_dot = -ShaftRate * simdt;
			}
			if ((OpticsManualMovement & 0x08) != 0) {
				A_s_dot = ShaftRate * simdt;
			}

			// DIRECT
			if (sat->ControllerCouplingSwitch.IsUp())
			{
				dShaft += A_s_dot;
				dTrunion += A_t_dot;
			}
			// RESOLVED
			else
			{
				dShaft += (A_s_dot*cos(OpticsShaft) - A_t_dot * sin(OpticsShaft)) / max(sin(10.0*RAD), sin(SextTrunion));
				dTrunion += A_s_dot * sin(OpticsShaft) + A_t_dot * cos(OpticsShaft);
			}
		}

		if (sat->agc.GetOutputChannelBit(012, DisengageOpticsDAC) == false)
		{
			//26mV per bit, 30.8 revolutions per second per volt, 1/3080 gear ratio (Shaft), 2/11780 gear ratio (Trunnion)
			dShaft += 0.026*30.8*PI2*1.0 / 3080.0*simdt*(double)sat->scdu.GetErrorCounter();
			dTrunion += 0.026*30.8*PI2*2.0 / 11780.0*simdt*(double)sat->tcdu.GetErrorCounter();
		}

		//sprintf(oapiDebugString(), "Trun Err: %lf Shaft Err: %lf", (double)sat->tcdu.GetErrorCounter()*180.0*pow(2, -14), (double)sat->scdu.GetErrorCounter()*180.0*pow(2, -12));
		//sprintf(oapiDebugString(), "Trun: %lf %d Shaft: %lf %d", dTrunion / simdt * DEG, sat->tcdu.GetErrorCounter(), dShaft / simdt * DEG, sat->scdu.GetErrorCounter());
	}

	OpticsShaft += dShaft;
	SextTrunion += dTrunion;

	//Limits
	if (OpticsShaft > 270.0*RAD)
	{
		OpticsShaft = 270.0*RAD;
	}
	if (OpticsShaft < -270.0*RAD)
	{
		OpticsShaft = -270.0*RAD;
	}
	if (SextTrunion < 0.0)
	{
		SextTrunion = 0.0;
	}
	if (SextTrunion > 59.0*RAD)
	{
		SextTrunion = 59.0*RAD;
	}

	sat->tcdu.SetReadCounter(SextTrunion * 4.0);
	sat->scdu.SetReadCounter(OpticsShaft);

	//sprintf(oapiDebugString(), "%d %d", sat->tcdu.GetErrorCounter(), sat->scdu.GetErrorCounter());

	// TELESCOPE TRUNNION MAINTENANCE (happens in all modes)
	// If the CMC issued pulses, they will have happened before we got here, so the sextant angle will be right.
	// If the order of timestep() calls is changed, this will "lag".

	double TeleTrunionTarget = 0;
	switch(sat->ControllerTelescopeTrunnionSwitch.GetState()){
		case THREEPOSSWITCH_UP:			// SLAVE TO SEXTANT			
			TeleTrunionTarget = SextTrunion;
			break;
		case THREEPOSSWITCH_CENTER:		// 0 DEG
			TeleTrunionTarget = 0;
			break;
		case THREEPOSSWITCH_DOWN:		// OFFSET 25 DEG
			TeleTrunionTarget = SextTrunion + 0.218166156; // Add 12.5 degrees to sextant angle
			break;
	}

	//Roughly from the transfer function (Apollo 15 Delco manual)
	TeleTrunion += (TeleTrunionTarget - TeleTrunion)*2.0*simdt;

	//sprintf(oapiDebugString(), "Optics Shaft %.2f, Sext Trunion %.2f, Tele Trunion %.2f", OpticsShaft/RAD, SextTrunion/RAD, TeleTrunion/RAD);
	//sprintf(oapiDebugString(), "Sext Trunion EMEM %o", sat->agc.vagc.Erasable[0][RegOPTY]);
}

void CMOptics::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, CMOPTICS_START_STRING);
	oapiWriteScenario_int(scn, "POWERED", Powered);
	oapiWriteScenario_int(scn, "OPTICSMANUALMOVEMENT", OpticsManualMovement);
	papiWriteScenario_double(scn, "OPTICSSHAFT", OpticsShaft);
	papiWriteScenario_double(scn, "SEXTTRUNION", SextTrunion);
	papiWriteScenario_double(scn, "TELETRUNION", TeleTrunion);
	papiWriteScenario_bool(scn, "OPTICSCOVERED", OpticsCovered); 
	oapiWriteLine(scn, CMOPTICS_END_STRING);
}

void CMOptics::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, CMOPTICS_END_STRING, sizeof(CMOPTICS_END_STRING)))
			return;
		else if (!strnicmp (line, "POWERED", 7)) {
			sscanf (line+7, "%d", &Powered);
		}
		else if (!strnicmp (line, "OPTICSMANUALMOVEMENT", 20)) {
			sscanf (line+20, "%d", &OpticsManualMovement);
		}
		else if (!strnicmp (line, "OPTICSSHAFT", 11)) {
			sscanf (line+11, "%lf", &OpticsShaft);
		}
		else if (!strnicmp (line, "SEXTTRUNION", 11)) {
			sscanf (line+11, "%lf", &SextTrunion);
		}
		else if (!strnicmp (line, "TELETRUNION", 11)) {
			sscanf (line+11, "%lf", &TeleTrunion);
		}
		papiReadScenario_bool(line, "OPTICSCOVERED", OpticsCovered); 
	}
}
