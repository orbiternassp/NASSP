/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant, Rodrigo R. M. B. Maia

  ORBITER vessel module: Saturn LEM computer

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

#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "IMU.h"
#include "lvimu.h"
#include "csmcomputer.h"
#include "lemcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "LEM.h"

#include "lm_channels.h"

LEMcomputer::LEMcomputer(SoundLib &s, DSKY &display, IMU &im, PanelSDK &p) : ApolloGuidance(s, display, im, p)

{

	isLGC = true;

	int i;
	for (i = 0; i < 16; i++) {
		RCSCommand[i] = 0;
	}
	CommandedAttitudeRotLevel = _V(0, 0, 0);
	CommandedAttitudeLinLevel = _V(0, 0, 0);

	//
	// Default ascent parameters.
	//

	DesiredApogee = 82.250;
	DesiredPerigee = 74.360;
	DesiredAzimuth = 270.0;

    mode = -1;
	simcomputert = -1.0;
    timeremaining = 99999.0;
	timeafterpdi = -1.0;

	/* FIXME LOAD FILE SHOULD BE SET IN SCENARIO */
	//InitVirtualAGC("Config/ProjectApollo/Luminary099.bin");

	/* FIXME REMOVE THIS LATER, THIS IS TEMPORARY FOR TESTING ONLY AND SHOULD BE IN THE SCENARIO LATER */
	/* LM PAD LOAD FOR LUMINARY 099 AND APOLLO 11  - OFFICIAL VERSION */

	thread.Resume();
}

LEMcomputer::~LEMcomputer()

{
	//
	// Nothing for now.
	//
}

void LEMcomputer::SetMissionInfo(int MissionNo, char *OtherVessel)

{
	ApolloGuidance::SetMissionInfo(MissionNo, OtherVessel);
	//
	// Pick the appropriate AGC binary file based on the mission number.
	//

	char *binfile;

	if (ApolloNo < 9)	// Sunburst 120
	{
		binfile = "Config/ProjectApollo/Sunburst120.bin";
	}
	else if (ApolloNo < 11)	// Luminary 069
	{
		binfile = "Config/ProjectApollo/Luminary069.bin";
	}
	else if (ApolloNo < 12)	// Luminary 099
	{
		binfile = "Config/ProjectApollo/Luminary099.bin";
	}
	else if (ApolloNo < 13)	// Luminary 116
	{
		binfile = "Config/ProjectApollo/Luminary116.bin";
	}
	else if (ApolloNo < 14 || ApolloNo == 1301)	// Luminary 131
	{
		binfile = "Config/ProjectApollo/Luminary131.bin";
	}
	else if (ApolloNo < 15)	// Luminary 210, modified for Apollo 14
	{
		binfile = "Config/ProjectApollo/Luminary210NBY71.bin";
	}
	else	//Luminary 210
	{
		binfile = "Config/ProjectApollo/Luminary210.bin";
	}

	agc_load_binfile(&vagc, binfile);
}

void LEMcomputer::agcTimestep(double simt, double simdt)
{
	GenericTimestep(simt, simdt);
}

void LEMcomputer::Run ()
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


void LEMcomputer::Timestep(double simt, double simdt)

{
	LEM *lem = (LEM *) OurVessel;
	// If the power is out, the computer should restart.
	// HARDWARE MUST RESTART
	if (!IsPowered()) {
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
			vagc.AllowInterrupt = 0;
			vagc.PendFlag = 0;
			vagc.PendDelay = 0;
			// Don't disturb erasable core
			// IO channels are flip-flop based and should reset, but that's difficult, so we'll ignore it.
			// Reset standby flip-flop
			vagc.Standby = 0;
			// Turn on EL display and LGC Light (DSKYWarn).
			SetOutputChannel(0163, 1);
			// Light OSCILLATOR FAILURE and LGC WARNING bits to signify power transient, and be forceful about it.	
			// Those two bits are what causes the CWEA to notice.
			InputChannel[033] &= 037777;
			vagc.InputChannel[033] &= 037777;
			OutputChannel[033] &= 037777;
			vagc.Ch33Switches &= 037777;
			// Also, simulate the operation of the VOLTAGE ALARM, turn off STBY and RESTART light while power is off.
			// The RESTART light will come on as soon as the AGC receives power again.
			// This happens externally to the AGC program. See CSM 104 SYS HBK pg 399
			vagc.VoltageAlarm = 1;
			vagc.RestartLight = 1;
			dsky.ClearRestart();
			dsky.ClearStby();

		// and do nothing more.
		return;
	}
	
	//
	// If MultiThread is enabled and the simulation is accellerated, the run vAGC in the AGC Thread,
	// otherwise run in main thread. at x1 acceleration, it is better to run vAGC totally synchronized
	//
	if(lem->isMultiThread && oapiGetTimeAcceleration() > 1.0){
		Lock lock(agcCycleMutex);
		thread_simt = simt;
		thread_simdt = simdt;
		timeStepEvent.Raise();
	}else{
		agcTimestep(simt,simdt);
	}
	return;
}

//
// Access simulated erasable memory.
//

bool LEMcomputer::ReadMemory(unsigned int loc, int &val)

{
	return GenericReadMemory(loc, val);
}

void LEMcomputer::WriteMemory(unsigned int loc, int val)

{
	GenericWriteMemory(loc, val);
}

//
// Special case handling of I/O channel changes.
//

void LEMcomputer::SetInputChannelBit(int channel, int bit, bool val)

{
	ApolloGuidance::SetInputChannelBit(channel, bit, val);
}

int LEMcomputer::GetStatus(double *simcomputert,
                    int    *mode,
				    double *timeremaining,
					double *timeafterpdi,
					double *timetoapproach)
				
{
	*simcomputert = this->simcomputert;
	*mode = this->mode;
	*timeremaining = this->timeremaining;
	*timeafterpdi = this->timeafterpdi;
	*timetoapproach = this->timetoapproach;
	return true;
}

int LEMcomputer::SetStatus(double simcomputert,
                       int    mode,
				       double timeremaining,
					   double timeafterpdi,
					   double timetoapproach)
					   
{
	this->simcomputert = simcomputert;
	this->mode = mode;
	this->timeremaining = timeremaining;
	this->timeafterpdi = timeafterpdi;
	this->timetoapproach = timetoapproach;
	return true;
}

// DS20060413

void LEMcomputer::ProcessChannel13(ChannelValue val){
	LEM *lem = (LEM *) OurVessel;	
	ChannelValue ch13;
	ch13 = val;
	if(ch13[EnableRHCCounter] && ch13[RHCRead]){
		int rhc_count[3];
		rhc_count[0] = lem->rhc_pos[0]/550;
		rhc_count[1] = lem->rhc_pos[1]/550;
		rhc_count[2] = lem->rhc_pos[2]/550;
		
		WriteMemory(042,rhc_count[1]); // PITCH 
		WriteMemory(043,rhc_count[2]); // YAW   
		WriteMemory(044,rhc_count[0]); // ROLL
		/*
		sprintf(oapiDebugString(),"LM CH13: %o RHC: SENT %d %d %d",val,
			rhc_count[0],rhc_count[1],rhc_count[2]);
		return;
		*/
	}
//	sprintf(oapiDebugString(),"LM CH13: %o",val);
}

void LEMcomputer::ProcessChannel5(ChannelValue val){
	// This is now handled inside the ATCA
	LEM *lem = (LEM *) OurVessel;	
	lem->atca.ProcessLGC(5,val.to_ulong());
}

void LEMcomputer::ProcessChannel6(ChannelValue val){
	// This is now handled inside the ATCA
	LEM *lem = (LEM *) OurVessel;	
	lem->atca.ProcessLGC(6,val.to_ulong());
}


void LEMcomputer::ProcessChannel140(ChannelValue val) {
	
	ChannelValue val12;
	val12 = GetOutputChannel(012);
	LEM *lem = (LEM *) OurVessel;

	if (val12[DispayInertialData])
	{
		lem->crossPointerLeft.SetForwardVelocity(val.to_ulong(), val12);
		lem->crossPointerRight.SetForwardVelocity(val.to_ulong(), val12);
	}
	else
	{
		lem->RR.RRShaftDrive(val.to_ulong(), val12);
	}
}

void LEMcomputer::ProcessChannel141(ChannelValue val) {

	ChannelValue val12;
	val12 = GetOutputChannel(012);
	LEM *lem = (LEM *) OurVessel;

	if (val12[DispayInertialData])
	{
		lem->crossPointerLeft.SetLateralVelocity(val.to_ulong(), val12);
		lem->crossPointerRight.SetLateralVelocity(val.to_ulong(), val12);
	}
	else
	{
		lem->RR.RRTrunionDrive(val.to_ulong(), val12);
	}
}

void LEMcomputer::ProcessChannel142(ChannelValue val) {

	LEM *lem = (LEM *)OurVessel;
	lem->deca.ProcessLGCThrustCommands(val.to_ulong());
}

void LEMcomputer::ProcessChannel143(ChannelValue val) {

	ChannelValue val14;
	val14 = GetOutputChannel(014);
	LEM *lem = (LEM *)OurVessel;

	if (val14[AltitudeRate])
	{
		lem->RadarTape.SetLGCAltitudeRate(val.to_ulong());
	}
	else
	{
		lem->RadarTape.SetLGCAltitude(val.to_ulong());
	}
}

// Process IMU CDU error counters.
void LEMcomputer::ProcessIMUCDUErrorCount(int channel, ChannelValue val){
	// FULL NEEDLE DEFLECTION is 16.88 DEGREES
	// 030 PULSES = MAX IN ONE RELAY EVENT
	// 22 PULSES IS ONE DEGREE, 384 PULSES = FULL SCALE
	// 0.10677083 PIXELS PER PULSE
	LEM *lem = (LEM *) OurVessel;	
	ChannelValue val12;
	if(channel != 012){ val12 = GetOutputChannel(012); }else{ val12 = val; }
	// 174 = X, 175 = Y, 176 = Z
	if(val12[CoarseAlignEnable]){ return; } // Does not apply to us here.
	switch(channel){
	case 012:
		if(val12[EnableIMUCDUErrorCounters]){
			if(lem->atca.lgc_err_ena == 0){
				// sprintf(oapiDebugString(),"LEM: LGC-ERR: RESET");
				lem->atca.lgc_err_x = 0;
				lem->atca.lgc_err_y = 0;
				lem->atca.lgc_err_z = 0;
				lem->atca.lgc_err_ena = 1;
			}
		}else{
			if (sat->gdc.fdai_err_ena == 1) {
				// sprintf(oapiDebugString(),"FDAI: RESET");
				sat->gdc.fdai_err_x = 0;
				sat->gdc.fdai_err_y = 0;
				sat->gdc.fdai_err_z = 0;
			}
			lem->atca.lgc_err_ena = 0;
		}

		// Reset cross pointer needles
		if (val12[DisplayInertialData]) {
			if (val12[EnableRRCDUErrorCounter]) {
				if (!lem->crossPointerLeft.lgcErrorCountersEnabled) {	//Dirty hack: voltage for cross pointers originates in RRCDU, the displays just get the voltages
					lem->crossPointerLeft.ZeroLGCVelocity();
					lem->crossPointerRight.ZeroLGCVelocity();
					lem->crossPointerLeft.lgcErrorCountersEnabled = true;
					lem->crossPointerRight.lgcErrorCountersEnabled = true;
				}
			}
			else {
				lem->crossPointerLeft.lgcErrorCountersEnabled = false;
				lem->crossPointerRight.lgcErrorCountersEnabled = false;
			}
		}

		break;

	case 0174: // YAW ERROR
		if(val12[EnableIMUCDUErrorCounters]){
			int delta = val.to_ulong()&0777;
			// NEGATIVE = RIGHT
			if(val.to_ulong()&040000){
				lem->atca.lgc_err_z -= delta;
			}else{
				lem->atca.lgc_err_z += delta;
			}
		}
		// sprintf(oapiDebugString(),"LEM: LGC-ERR: %d %d %d",lem->atca.lgc_err_x,lem->atca.lgc_err_y,lem->atca.lgc_err_z);
		break;
	
	case 0175: // PITCH ERROR
		if(val12[EnableIMUCDUErrorCounters]){
			int delta = val.to_ulong()&0777;
			// NEGATIVE = DOWN
			if(val.to_ulong()&040000){
				lem->atca.lgc_err_y -= delta;
			}else{
				lem->atca.lgc_err_y += delta;
			}
		}
		// sprintf(oapiDebugString(),"LEM: LGC-ERR: %d %d %d",lem->atca.lgc_err_x,lem->atca.lgc_err_y,lem->atca.lgc_err_z);
		break;

	case 0176: // ROLL ERROR
		if(val12[EnableIMUCDUErrorCounters]){
			int delta = val.to_ulong()&0777;
			// NEGATIVE = RIGHT
			if(val.to_ulong()&040000){
				lem->atca.lgc_err_x += delta;
			}else{
				lem->atca.lgc_err_x -= delta;
			}
		}
		// sprintf(oapiDebugString(),"LEM: LGC-ERR: %d %d %d",lem->atca.lgc_err_x,lem->atca.lgc_err_y,lem->atca.lgc_err_z);
		break;
	}

}

/// \todo Dirty Hack for the AGC++ RCS burn control, 
/// remove this and use I/O channels and pulsed thrusters 
/// identical to the VAGC instead
///

void LEMcomputer::ResetAttitudeLevel() {

	int i;
	for (i = 0; i < 16; i++) {
		RCSCommand[i] = 0;
	}
}

void LEMcomputer::AddAttitudeRotLevel(VECTOR3 level) {

	int i;

	// Pitch
	if (level.x < 0) {
		RCSCommand[0] -= level.x;
		RCSCommand[7] -= level.x;
		RCSCommand[11] -= level.x;
		RCSCommand[12] -= level.x;
	}
	else {
		RCSCommand[3] += level.x;
		RCSCommand[4] += level.x;
		RCSCommand[8] += level.x;
		RCSCommand[15] += level.x;
	}

	// Roll
	if (level.z < 0) {
		RCSCommand[0] -= level.z;
		RCSCommand[4] -= level.z;
		RCSCommand[11] -= level.z;
		RCSCommand[15] -= level.z;
	}
	else {
		RCSCommand[3] += level.z;
		RCSCommand[7] += level.z;
		RCSCommand[8] += level.z;
		RCSCommand[12] += level.z;
	}

	// Yaw
	if (level.y > 0) {
		RCSCommand[1] += level.y;
		RCSCommand[5] += level.y;
		RCSCommand[10] += level.y;
		RCSCommand[14] += level.y;
	}
	else {
		RCSCommand[2] -= level.y;
		RCSCommand[6] -= level.y;
		RCSCommand[9] -= level.y;
		RCSCommand[13] -= level.y;
	}

	// Renormalize
	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] > 1) {
			RCSCommand[i] = 1;
		}
	}

	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] < -1) {
			RCSCommand[i] = -1;
		}
	}

	// Set thrust
	LEM *lem = (LEM *) OurVessel;
	for (i = 0; i < 16; i++) {
		lem->SetRCSJetLevelPrimary(i, RCSCommand[i]);
	}
}

void LEMcomputer::AddAttitudeLinLevel(int axis, double level) {

	VECTOR3 l = _V(0, 0, 0);
	if (axis == 0) l.x = level;
	if (axis == 1) l.y = level;
	if (axis == 2) l.z = level;
	AddAttitudeLinLevel(l);
}

void LEMcomputer::AddAttitudeLinLevel(VECTOR3 level) {

	int i;

	// Left/right
	if (level.x < 0) {
		RCSCommand[9] -= level.x;
		RCSCommand[14] -= level.x;
	}
	else {
		RCSCommand[2] += level.x;
		RCSCommand[5] += level.x;
	}

	// Down/up
	if (level.y < 0) {
		RCSCommand[0] -= level.y;
		RCSCommand[4] -= level.y;
		RCSCommand[8] -= level.y;
		RCSCommand[12] -= level.y;
	}
	else {
		RCSCommand[3] += level.y;
		RCSCommand[7] += level.y;
		RCSCommand[11] += level.y;
		RCSCommand[15] += level.y;
	}

	// Back/forward
	if (level.z < 0) {
		RCSCommand[1] -= level.z;
		RCSCommand[13] -= level.z;
	}
	else {
		RCSCommand[6] += level.z;
		RCSCommand[10] += level.z;
	}

	// Renormalize
	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] > 1) {
			RCSCommand[i] = 1;
		}
	}

	for (i = 0; i < 16; i++) {
		if (RCSCommand[i] < -1) {
			RCSCommand[i] = -1;
		}
	}

	// Set thrust
	LEM *lem = (LEM *) OurVessel;
	for (i = 0; i < 16; i++) {
		lem->SetRCSJetLevelPrimary(i, RCSCommand[i]);
	}
}

void LEMcomputer::SetAttitudeRotLevel(VECTOR3 level) {

	ResetAttitudeLevel();
	AddAttitudeRotLevel(level);

}

VESSEL * LEMcomputer::GetCSM()
{
	OBJHANDLE hcsm = oapiGetVesselByName(OtherVesselName);
	VESSEL *CSMVessel = oapiGetVesselInterface(hcsm);
	return CSMVessel;
}

//
// LM Optics class code
//

LMOptics::LMOptics() {

	lem = NULL;
	OpticsShaft = 3;
	OpticsReticle = 0.0;
	ReticleMoved = 0;
	RetDimmer = 255;
	KnobTurning = 0;
}

void LMOptics::Init(LEM *vessel) {

	lem = vessel;
}

void LMOptics::SystemTimestep(double simdt) {

	// LEM Optics is a manual system... no power required.
	// There were however heaters that would keep the optics from freezing or fogging.
	// Might want to implment those...


}

void LMOptics::TimeStep(double simdt) {
	OpticsReticle = OpticsReticle + simdt * ReticleMoved;

	if (ReticleMoved)
	{
		sprintf(oapiDebugString(), "Optics Shaft %d, Optics Reticle %.2f, Moved? %.4f, KnobTurning %d", OpticsShaft, 360.0 - OpticsReticle / RAD, ReticleMoved, KnobTurning);
	}

	if (OpticsReticle > 2*PI) OpticsReticle -= 2*PI;
	if (OpticsReticle < 0) OpticsReticle += 2*PI;
}

void LMOptics::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, LMOPTICS_START_STRING);
	papiWriteScenario_double(scn, "OPTICSSHAFT", OpticsShaft);
	papiWriteScenario_double(scn, "OPTICSRETICLE", OpticsReticle);
	papiWriteScenario_double(scn, "RETDIMMER", RetDimmer);
	oapiWriteLine(scn, LMOPTICS_END_STRING);
}

void LMOptics::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, LMOPTICS_END_STRING, sizeof(LMOPTICS_END_STRING)))
			return;
		else if (!strnicmp (line, "OPTICSSHAFT", 11)) {
			sscanf (line+11, "%d", &OpticsShaft);
		}
		else if (!strnicmp (line, "OPTICSRETICLE", 11)) {
			sscanf (line+11, "%lf", &OpticsReticle);
		}
		else if (!strnicmp (line, "RETDIMMER", 11)) {
			sscanf (line+11, "%d", &RetDimmer);
		}
	}
}
