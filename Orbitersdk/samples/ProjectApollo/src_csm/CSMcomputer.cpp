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
#include "IMU.h"
#include "toggleswitch.h"
#include "saturn.h"
#include "ioChannels.h"
#include "papi.h"
#include "thread.h"

CSMcomputer::CSMcomputer(SoundLib &s, DSKY &display, DSKY &display2, IMU &im, PanelSDK &p, CSMToIUConnector &i, CSMToSIVBControlConnector &sivb) : 
	ApolloGuidance(s, display, im, p), dsky2(display2), iu(i), lv(sivb)

{
	isLGC = false;

	VesselStatusDisplay = 0;

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

void CSMcomputer::SetMissionInfo(int MissionNo, char *OtherVessel)

{
	ApolloGuidance::SetMissionInfo(MissionNo, OtherVessel);

	//
	// Pick the appropriate AGC binary file based on the mission number.
	//
	// same criterium in CSMcomputer::Timestep because of pad load

	char *binfile;

	if (ApolloNo < 9)	// Colossus 237
	{
		binfile = "Config/ProjectApollo/Colossus237.bin";
	}
	else if (ApolloNo < 10)	// Colossus 249
	{
		binfile = "Config/ProjectApollo/Colossus249.bin";
	}
	else if (ApolloNo < 11)	// Comanche 055, modified for Apollo 10
	{
		binfile = "Config/ProjectApollo/Comanche055NBY69.bin";
	}
	else if (ApolloNo < 14 || ApolloNo == 1301)	// Comanche 055
	{
		binfile = "Config/ProjectApollo/Comanche055.bin";
	}
	else if (ApolloNo < 15)	// Artemis 72, modified for Apollo 14
	{
		binfile = "Config/ProjectApollo/Artemis072NBY71.bin";
	}
	else	//Artemis 072
	{
		binfile = "Config/ProjectApollo/Artemis072.bin";
	}

	InitVirtualAGC(binfile);
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
		if (!IsPowered()){
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
				vagc.ExtraDelay = 0;
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
				vagc.VoltageAlarm = 1;
				vagc.RestartLight = 1;
				sat->dsky.ClearRestart();
				sat->dsky2.ClearRestart();
				sat->dsky.ClearStby();
				sat->dsky2.ClearStby();
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

			if (ApolloNo < 10)	//Colossus 249 and criterium in SetMissionInfo
			{
				// set launch pad longitude
				if (longitude < 0) { longitude += TWO_PI; }
				vagc.Erasable[2][0263] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0264] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0272] = 01;	// 17.7 nmi
				vagc.Erasable[2][0272] = 0;
				vagc.Erasable[2][0273] = (int16_t)(0.5 * OurVessel->GetAltitude());

				// z-component of the normalized earth's rotational vector in basic reference coord.
				// x and y are 0313 and 0315 and are defined in the scenario
				vagc.Erasable[3][0317] = 037777;
				vagc.Erasable[3][0320] = 037777;

				TEPHEM0 = 40038.;
			}
			else if (ApolloNo < 14 || ApolloNo == 1301)	// Comanche 055
			{
				// set launch pad longitude
				if (longitude < 0) { longitude += TWO_PI; }
				vagc.Erasable[2][0263] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0264] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0272] = 01;	// 17.7 nmi
				vagc.Erasable[2][0272] = 0;
				vagc.Erasable[2][0273] = (int16_t)(0.5 * OurVessel->GetAltitude());

				// z-component of the normalized earth's rotational vector in basic reference coord.
				// x and y are 0313 and 0315 and are defined in the scenario
				vagc.Erasable[3][0317] = 037777;
				vagc.Erasable[3][0320] = 037777;

				TEPHEM0 = 40403.;
			}
			else if (ApolloNo < 15)	//Artemis 072 for Apollo 14
			{
				// set launch pad longitude
				if (longitude < 0) longitude += TWO_PI;
				vagc.Erasable[2][0135] = ConvertDecimalToAGCOctal(longitude / TWO_PI, true);
				vagc.Erasable[2][0136] = ConvertDecimalToAGCOctal(longitude / TWO_PI, false);

				// set launch pad altitude
				//vagc.Erasable[2][0133] = 01;	// 17.7 nmi
				vagc.Erasable[2][0133] = 0;
				vagc.Erasable[2][0134] = (int16_t)(0.5 * OurVessel->GetAltitude());

				// z-component of the normalized earth's rotational vector in basic reference coord.
				// x and y are 0313 and 0315 and are zero
				vagc.Erasable[3][0315] = 037777;
				vagc.Erasable[3][0316] = 037777;

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

				// z-component of the normalized earth's rotational vector in basic reference coord.
				// x and y are 0313 and 0315 and are zero
				vagc.Erasable[3][0315] = 037777;
				vagc.Erasable[3][0316] = 037777;

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
	if (sat->SCContSwitch.IsDown() || sat->THCRotary.IsClockwise()) {
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
	if (sat->SCContSwitch.IsDown() || sat->THCRotary.IsClockwise()) {
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

		// Reset TVC
		if (val12[TVCEnable]) {
			if (val12[EnableOpticsCDUErrorCounters]) {
				if (!sat->SPSEngine.cmcErrorCountersEnabled) {
					sat->SPSEngine.pitchGimbalActuator.ZeroCMCPosition();
					sat->SPSEngine.yawGimbalActuator.ZeroCMCPosition();
					sat->SPSEngine.cmcErrorCountersEnabled = true;
				}
			} else {
				sat->SPSEngine.cmcErrorCountersEnabled = false;
			}
		}

		// If OPTICS TRACKER switch is not up
		if(sat->ControllerTrackerSwitch.GetState() != THREEPOSSWITCH_UP){
			// leave
			break;
		}else{
			// If the switch is up, print optics register status
			bool IssueDebug = FALSE;
			char DebugMsg[256];

			sprintf(DebugMsg,"OPTICS: ");
			if(val12[DisengageOpticsDAC]){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s DISENGAGE-DAC",DebugMsg);
			}
			if(val12[EnableOpticsCDUErrorCounters]){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s ENABLE-ERR-CTR",DebugMsg);
			}else{
				// This caused problems.
				// sat->agc.vagc.block_ocdu_err_ctr = 0;
			}
			if(val12[ZeroOptics]){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s ZERO-OPTICS",DebugMsg);
			}
			if(val12[ZeroOpticsCDUs]){
				IssueDebug = TRUE;
				sprintf(DebugMsg,"%s ZERO-CDU",DebugMsg);
			}
			if(IssueDebug != FALSE){
				sprintf(oapiDebugString(),"%s",DebugMsg);
			}else{
				sprintf(oapiDebugString(),"OPTICS: NIL");
			}		
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

// TVC / Optics control

void CSMcomputer::ProcessChannel140(ChannelValue val) {
	
	ChannelValue val12;
	val12 = GetOutputChannel(012);
	Saturn *sat = (Saturn *) OurVessel;
	double error = 0;
	int valx = val.to_ulong();

	// TVC enable controls SPS gimballing.			
	if (val12[TVCEnable]) {
		// TVC PITCH
		int tvc_pitch_pulses = 0;
		double tvc_pitch_cmd = 0;
		// One pulse means .023725 degree of rotation.
		if(valx&077000){ // Negative
			tvc_pitch_pulses = (~valx)&0777;
			if(tvc_pitch_pulses == 0){ return; } // HACK
			tvc_pitch_cmd = (double)0.023725 * tvc_pitch_pulses;
			tvc_pitch_cmd = 0 - tvc_pitch_cmd; // Invert
		}else{
			tvc_pitch_pulses = valx&0777;
			tvc_pitch_cmd = (double)0.023725 * tvc_pitch_pulses;
		}		
		sat->SPSEngine.pitchGimbalActuator.ChangeCMCPosition(tvc_pitch_cmd);

	} else {
		sat->optics.CMCShaftDrive(valx,val12.to_ulong());
	}	
}

void CSMcomputer::ProcessChannel141(ChannelValue val) {

	ChannelValue val12;
	val12 = GetOutputChannel(012);
	Saturn *sat = (Saturn *) OurVessel;
	double error = 0;
	int valx = val.to_ulong();

	if (val12[TVCEnable]) {
		// TVC YAW
		int tvc_yaw_pulses = 0;
		double tvc_yaw_cmd = 0;		
		if(valx&077000){ 
			tvc_yaw_pulses = (~valx)&0777;
			if(tvc_yaw_pulses == 0){ return; } 
			tvc_yaw_cmd = (double)0.023725 * tvc_yaw_pulses;
			tvc_yaw_cmd = 0 - tvc_yaw_cmd; 
		}else{
			tvc_yaw_pulses = valx&0777;
			tvc_yaw_cmd = (double)0.023725 * tvc_yaw_pulses;
		}				
		sat->SPSEngine.yawGimbalActuator.ChangeCMCPosition(tvc_yaw_cmd);

	} else {
		sat->optics.CMCTrunionDrive(valx,val12.to_ulong());
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

void CSMcomputer::LVGuidanceSwitchToggled(PanelSwitchItem *s) {
	ChannelValue val30;

	val30 = GetInputChannel(030); // Get current data

	if (s->GetState() == TOGGLESWITCH_UP) {
		val30[SCControlOfSaturn] = 0;
	} else {
	    val30[SCControlOfSaturn] = 1;
	}
	SetInputChannel(030, val30);
}



//
// CM Optics class code
//

CMOptics::CMOptics() {

	sat = NULL;
	OpticsShaft = 0.0;
	SextTrunion = 0.0;
	TeleTrunion = 0.0;
	ShaftMoved = 0.0;
	TrunionMoved = 0.0;
	dTrunion = 0.0;
	dShaft = 0.0;
	OpticsManualMovement = 0;
	Powered = 0;
	SextDualView = false;
	SextDVLOSTog = false;
	SextDVTimer = 0.0;
	OpticsCovered = true;

	TargetShaft = 0;
	TargetTrunion = 0;
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

void CMOptics::CMCTrunionDrive(int val,int ch12) {

	int pulses;
	ChannelValue val12;
	val12 = ch12;

	if (Powered == 0) { return; }

	if (val&040000){ // Negative
		pulses = -((~val)&07777); 
	} else {
		pulses = val&07777; 
	}
	if (val12[EnableOpticsCDUErrorCounters]){
		sat->agc.vagc.Erasable[0][RegOPTY] += pulses;
		sat->agc.vagc.Erasable[0][RegOPTY] &= 077777;
	}
	SextTrunion += (OCDU_TRUNNION_STEP*pulses); 
	TrunionMoved = SextTrunion;
	// sprintf(oapiDebugString(),"TRUNNION: %o PULSES, POS %o", pulses&077777 ,sat->agc.vagc.Erasable[0][035]);		
}

void CMOptics::CMCShaftDrive(int val,int ch12) {

	int pulses;
	ChannelValue val12;
	val12 = ch12;

	if (Powered == 0) { return; }

	if (val&040000){ // Negative
		pulses = -((~val)&07777); 
	} else {
		pulses = val&07777; 
	}
	OpticsShaft += (OCDU_SHAFT_STEP*pulses);
	ShaftMoved = OpticsShaft;
	if (val12[EnableOpticsCDUErrorCounters]){
		sat->agc.vagc.Erasable[0][RegOPTX] += pulses;
		sat->agc.vagc.Erasable[0][RegOPTX] &= 077777;
	}
	// sprintf(oapiDebugString(),"SHAFT: %o PULSES, POS %o", pulses&077777, sat->agc.vagc.Erasable[0][036]);
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

	// Generate rates for telescope and manual mode
	switch(sat->ControllerSpeedSwitch.GetState()) {
		case THREEPOSSWITCH_UP:       // HI
			ShaftRate = 1775. * simdt;
			TrunRate  = 3640. * simdt;
			break;
		case THREEPOSSWITCH_CENTER:   // MED
			ShaftRate = 182. * simdt;
			TrunRate  = 364. * simdt;
			break;
		case THREEPOSSWITCH_DOWN:     // LOW
			ShaftRate = 18. * simdt;
			TrunRate  = 36. * simdt;
			break;
	}

	switch(sat->ModeSwitch.GetState()) {
		case THREEPOSSWITCH_DOWN: // ZERO OPTICS
			// Force MANUAL HI rate for zero optics mode.
			ShaftRate = 1775. * simdt;
			TrunRate = 3640. * simdt;

			if(OpticsShaft > 0){
				if(OpticsShaft > OCDU_SHAFT_STEP*ShaftRate){
					OpticsShaft -= OCDU_SHAFT_STEP*ShaftRate;
					ShaftMoved = OpticsShaft;
				}else{
					OpticsShaft = 0;
					ShaftMoved = 0;
				}
			}
			if(OpticsShaft < 0){
				if(OpticsShaft < (-OCDU_SHAFT_STEP*ShaftRate)){
					OpticsShaft += OCDU_SHAFT_STEP*ShaftRate;
					ShaftMoved = OpticsShaft;
				}else{
					OpticsShaft = 0;
					ShaftMoved = 0;
				}
			}
			if(SextTrunion > 0){
				if(SextTrunion > OCDU_TRUNNION_STEP*TrunRate){
					SextTrunion -= OCDU_TRUNNION_STEP*TrunRate;
					TrunionMoved = SextTrunion;
				}else{
					SextTrunion = 0;
					TrunionMoved = 0;
				}				
			}
			if(SextTrunion < 0){
				if(SextTrunion < (-OCDU_TRUNNION_STEP*TrunRate)){
					SextTrunion += OCDU_TRUNNION_STEP*TrunRate;
					TrunionMoved = SextTrunion;
				}else{
					SextTrunion = 0;
					TrunionMoved = 0;
				}				
			}
			break;
		case THREEPOSSWITCH_CENTER: // MANUAL

			/* About "SextTrunion < (RAD*59.0)":

			# Page 711
			# PROGRAM NAME -- PICAPAR   DATE: DEC 20 66
			# MOD 1            LOG SECTION: P51-P53
			#            ASSEMBLY:  SUNDISK REV40
			# BY KEN VINCENT
			#
			# FUNCTION
			#   THIS PROGRAM READS THE IMU-CDUS AND COMPUTES THE VEHICLE ORIENTATION
			#   WITH RESPECT TO INERTIAL SPACE.  IT THEN COMPUTES THE SHAFT AXIS (SAX)
			#   WITH RESPECT TO REFERENCE INTERTIAL.  EACH STAR IN THE CATALOG IS TESTED
			#    TO DETERMINE IF IT IS OCCULTED BY EITHER EARTH, SUN OR MOON.  IF A
			#    STAR IS NOT OCCULTED THEN IT IS PARIED WITH ALL STARS OF LOWER INDEX.
			#    THE PAIRED STAR IS TESTED FOR OCCULTATION.  PAIRS OF STARS THAT PASS
			#   THE OCCULTATION TESTS ARE TESTED FOR GOOD SEPARATION.  A PAIR OF STARS
			#   HAVE GOOD SEPARATION IF THE ANGLE BETWEEN THEM IS LESS THAN 66 DEGREES
			#   AND MORE THAN 40 DEGREES.  THOSE PAIRS WITH GOOD SEPARATION
			#   ARE THEN TESTED TO SEE IF THEY LIE IN CURRENT FIELD OF VIEW.  (WITHIN
			#   33 DEGREES OF SAX).  THE PAIR WITH MAX SEPARATION IS CHOSEN FROM
			#   THOSE WITH GOOD SEPARATION, AND IN FIELD OF VIEW.

			As stated above, the angular difference between the 2 stars should be 40°-66°, a rather tight margin. 
			Greater than 66° isn't working at all, but smaller than 40° is at least possible (i.e. no errors) however less precise.

			But the stated max. field of view = max. trunnion angle of 33° seems to be wrong. 
			With trunnion angles greater than 60°, P51 isn't working anymore (as I figured out by testing). 
			But PICAPAR does choose stars with a trunnion angle greater than 33° (current "record" 45°), so restricting the max. trunnion
			angle to 33° is NO option. AOH 2.2.3.3.1 seems to state a max. trunnion angle of 50°.

			http://www.ibiblio.org/mscorbit/mscforum/index.php?topic=2514.msg20287#msg20287
			*/

			dTrunion = 0.0;
			dShaft = 0.0;

			switch (sat->ControllerCouplingSwitch.GetState()) {
			case TOGGLESWITCH_UP: // DIRECT

				if ((OpticsManualMovement & 0x01) != 0 && SextTrunion < (RAD*59.0)) {
					dTrunion = OCDU_TRUNNION_STEP * TrunRate;
				}
				if ((OpticsManualMovement & 0x02) != 0 && SextTrunion > 0) {
					dTrunion = -OCDU_TRUNNION_STEP * TrunRate;
				}
				if ((OpticsManualMovement & 0x04) != 0 && OpticsShaft > -(RAD*270.0)) {
					dShaft = -OCDU_SHAFT_STEP * ShaftRate;
				}
				if ((OpticsManualMovement & 0x08) != 0 && OpticsShaft < (RAD*270.0)) {
					dShaft = OCDU_SHAFT_STEP * ShaftRate;
				}
				break;

			case TOGGLESWITCH_DOWN: // RESOLVED
				double A_t_dot, A_s_dot;
				A_t_dot = 0.0;
				A_s_dot = 0.0;

				if ((OpticsManualMovement & 0x01) != 0) {// && SextTrunion < (RAD*59.0)) {
					A_t_dot = OCDU_TRUNNION_STEP * TrunRate;
				}
				if ((OpticsManualMovement & 0x02) != 0) {//&& SextTrunion > 0) {
					A_t_dot = -OCDU_TRUNNION_STEP * TrunRate;
				}
				if ((OpticsManualMovement & 0x04) != 0) {//&& OpticsShaft > -(RAD*270.0)) {
					A_s_dot = -OCDU_SHAFT_STEP * ShaftRate;
				}
				if ((OpticsManualMovement & 0x08) != 0) {//&& OpticsShaft < (RAD*270.0)) {
					A_s_dot = OCDU_SHAFT_STEP * ShaftRate;
				}

				dShaft = (A_s_dot*cos(OpticsShaft) - A_t_dot*sin(OpticsShaft)) / max(sin(10.0*RAD), sin(SextTrunion));
				dTrunion = A_s_dot*sin(OpticsShaft) + A_t_dot*cos(OpticsShaft);

				TrunRate = abs(dTrunion) / OCDU_TRUNNION_STEP;	//Just so that the telescope trunnion moves correctly

				break;
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

			if (dTrunion > 0) {
				while (fabs(fabs(SextTrunion) - fabs(TrunionMoved)) >= OCDU_TRUNNION_STEP) {
					sat->agc.vagc.Erasable[0][RegOPTY]++;
					sat->agc.vagc.Erasable[0][RegOPTY] &= 077777;
					TrunionMoved += OCDU_TRUNNION_STEP;
				}
			}
			if (dTrunion < 0) {
				while (fabs(fabs(SextTrunion) - fabs(TrunionMoved)) >= OCDU_TRUNNION_STEP) {
					sat->agc.vagc.Erasable[0][RegOPTY]--;
					sat->agc.vagc.Erasable[0][RegOPTY] &= 077777;
					TrunionMoved -= OCDU_TRUNNION_STEP;
				}
			}
			if (dShaft < 0) {
				while (fabs(fabs(OpticsShaft) - fabs(ShaftMoved)) >= OCDU_SHAFT_STEP) {
					sat->agc.vagc.Erasable[0][RegOPTX]--;
					sat->agc.vagc.Erasable[0][RegOPTX] &= 077777;
					ShaftMoved -= OCDU_SHAFT_STEP;
				}
			}
			if (dShaft > 0) {
				while (fabs(fabs(OpticsShaft) - fabs(ShaftMoved)) >= OCDU_SHAFT_STEP) {
					sat->agc.vagc.Erasable[0][RegOPTX]++;
					sat->agc.vagc.Erasable[0][RegOPTX] &= 077777;
					ShaftMoved += OCDU_SHAFT_STEP;
				}
			}

			break;
	}

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
	if(TeleTrunion > TeleTrunionTarget){
		if(TeleTrunion > TeleTrunionTarget-(OCDU_TRUNNION_STEP*TrunRate)){
			TeleTrunion -= OCDU_TRUNNION_STEP*TrunRate;				
		}else{
			TeleTrunion = TeleTrunionTarget;
		}				
	}
	if(TeleTrunion < TeleTrunionTarget){
		if(TeleTrunion < TeleTrunionTarget+(-OCDU_TRUNNION_STEP*TrunRate)){
			TeleTrunion += OCDU_TRUNNION_STEP*TrunRate;
		}else{
			TeleTrunion = TeleTrunionTarget;
		}
	}

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
	papiWriteScenario_double(scn, "TARGETSHAFT", TargetShaft);
	papiWriteScenario_double(scn, "TARGETTRUNION", TargetTrunion);
	papiWriteScenario_double(scn, "SHAFTMOVED", ShaftMoved);
	papiWriteScenario_double(scn, "TRUNIONMOVED", TrunionMoved);
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
		else if (!strnicmp (line, "TARGETSHAFT", 11)) {
			sscanf (line+11, "%lf", &TargetShaft);
		}
		else if (!strnicmp (line, "TARGETTRUNION", 13)) {
			sscanf (line+13, "%lf", &TargetTrunion);
		}
		else if (!strnicmp (line, "SHAFTMOVED", 10)) {
			sscanf (line+10, "%lf", &ShaftMoved);
		}
		else if (!strnicmp (line, "TRUNIONMOVED", 12)) {
			sscanf (line+12, "%lf", &TrunionMoved);
		} 
		papiReadScenario_bool(line, "OPTICSCOVERED", OpticsCovered); 
	}
}
