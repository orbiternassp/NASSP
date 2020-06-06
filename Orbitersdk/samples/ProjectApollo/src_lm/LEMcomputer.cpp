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
#include "lemcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "LEM.h"

#include "lm_channels.h"

LEMcomputer::LEMcomputer(SoundLib &s, DSKY &display, IMU &im, LEM_RR &lm_rr, PanelSDK &p) :
	ApolloGuidance(false, s, display, im, p)

{

	isLGC = true;

	/* FIXME LOAD FILE SHOULD BE SET IN SCENARIO */
	//InitVirtualAGC("Config/ProjectApollo/Luminary099.bin");
	ffradgtset = 0;
	ffactv = 0;
	rdrcnt = 0;
	rdrval = 0;
	/* FIXME REMOVE THIS LATER, THIS IS TEMPORARY FOR TESTING ONLY AND SHOULD BE IN THE SCENARIO LATER */
	/* LM PAD LOAD FOR LUMINARY 099 AND APOLLO 11  - OFFICIAL VERSION */

	double speed_gain = 0.0004709/12800.0;
	double speed_limit = speed_gain*348.0;

	tcdu.SetAngleDevice(&lm_rr.trunnionAngle);
	tcdu.driver_speed_gain = speed_gain;
	tcdu.driver_speed_limit = speed_limit;

	scdu.SetAngleDevice(&lm_rr.shaftAngle);
	scdu.driver_speed_gain = speed_gain;
	scdu.driver_speed_limit = speed_limit;

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

		LEM *lem = (LEM *)OurVessel;
		lem->InvertStageBit = true;
	}
	else if (ApolloNo < 11)	// Luminary 069 Revision 2
	{
		binfile = "Config/ProjectApollo/LUM69R2.bin";
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
	else if (ApolloNo < 15)	// Luminary 178
	{
		binfile = "Config/ProjectApollo/Luminary178.bin";
	}
	else	//Luminary 210
	{
		binfile = "Config/ProjectApollo/Luminary210.bin";
	}

	agc_load_binfile(&vagc, binfile);
}

void LEMcomputer::agcTimestep(double simt, double simdt)
{
	// Do single timesteps to maintain sync with telemetry engine
	SingleTimestepPrep(simt, simdt);        // Setup
	if (LastCycled == 0) {					// Use simdt as difference if new run
		LastCycled = (simt - simdt);
		lem->VHF.last_update = LastCycled;
	}
	double ThisTime = LastCycled;			// Save here

	long cycles = (long)((simt - LastCycled) / 0.00001171875);	// Get number of CPU cycles to do
	LastCycled += (0.00001171875 * cycles);						// Preserve the remainder
	long x = 0;
	while (x < cycles) {
		if (vagc.ScalerChanged) {
//			char buf[256];
			if ((vagc.Scaler & 0177) == 0177) {//F10A
				ChannelValue val13 = GetInputChannel(013);
				if (val13[RadarActivity] == 1) {
					ffactv = 1;  //ACTV flipflop
//					sprintf(buf, "scaler=%11o ffactv=1", vagc.Scaler);
//					oapiWriteLog(buf);
				}
				else {
//					sprintf(buf, "scaler=%11o ffactv=0", vagc.Scaler);
//					if(ffactv) oapiWriteLog(buf);
					ffactv = 0;

				}
				if (ffactv) {
					rdrcnt++;
//					sprintf(buf, "scaler=%11o increase rdrcnt=%d", vagc.Scaler, rdrcnt);
//					oapiWriteLog(buf);
				}

				// In the real hw before passing data
				// to LGC, the radar data is moved to
				// an internal register of the radar
				// itself. It happens gradually
				// through "strobe signals" during
				// rdrcnt goes form 1 and 8. However
				// we only take a single value to avoid
				// overcomplicating things. After this,
				// radar data is ready for asynchronous
				// transfer to LGC.
				if (rdrcnt == 8) {
					if (val13[RadarA] == 1)
						rdrval = lem->LR.GetRadarData(val13[RadarB], val13[RadarC]);
					else
						rdrval = lem->RR.GetRadarData(val13[RadarB], val13[RadarC]);
//					sprintf(buf, "scaler=%11o read radar rdrval=%5o", vagc.Scaler, rdrval);
//					oapiWriteLog(buf);

				}
			}

			int gtset = ((vagc.Scaler & 077) == 075); //GTSET=FS09 & FS08 & FS07 & FS06 & F05B
			if (rdrcnt == 9 && gtset) { 
				ffradgtset = 1;  //Radar's GTSET flipflop
//				sprintf(buf, "scaler=%11o GTSET RNRAD=%5ho", vagc.Scaler, vagc.Erasable[0][RegRNRAD]);
//				oapiWriteLog(buf);
			}

			if (ffradgtset) {
				if ((vagc.Scaler & 3) == 3) { //F05A 
					if (rdrval & 040000)
						setCntrReqP(&vagc, 022);
					else 
						setCntrReqM(&vagc, 022);
//					sprintf(buf, "scaler=%11o step rdrval=%5o RNRAD=%5ho", vagc.Scaler, rdrval, vagc.Erasable[0][RegRNRAD]);
//					oapiWriteLog(buf);
					rdrval = rdrval << 1;
				}

				if ((vagc.Scaler & 077) == 071) { //GTRST=FS09 & FS08 & FS07 & !FS06 & F05B
//					sprintf(buf, "scaler=%11o increase GTRST RNRAD=%5ho", vagc.Scaler, vagc.Erasable[0][RegRNRAD]);
//					oapiWriteLog(buf);
					//RADRPT happens
					ffradgtset = 0;
					rdrcnt = 0;
					SetInputChannelBit(013, RadarActivity, 0);
					GenerateRadarupt();
				}
			}

			if ((vagc.Scaler & 0177) == 0001) { //GTONE=!FS09 & !FS08 & !FS07A & !FS06 & F05B
				ChannelValue val14 = GetInputChannel(014);
				if (val14[AltitudeIndicator]) {
					ffaltgtset = 1;
					altval = 0;
				}
			}

			if (ffaltgtset) {
				ChannelValue val14 = GetInputChannel(014);
				if ((vagc.Scaler & 3) == 3) { //F05A 
					setCntrReqP(&vagc, 034);
				}
				else if (gtset) {
					ffaltgtset = 0;
					if (val14[AltitudeRate])
						lem->RadarTape.SetLGCAltitudeRate(altval);
					else
						lem->RadarTape.SetLGCAltitude(altval);
					SetInputChannelBit(014, AltitudeIndicator, 0);
				}
			}

		}
		SingleTimestep();
		ThisTime += 0.00001171875;								// Add time
		if ((ThisTime - lem->VHF.last_update) > 0.00015625) {	// If a step is needed
			lem->VHF.Timestep(ThisTime);						// do it
		}
		x++;
	}
	

	if (sumdt > 2.0) {

		for (int i = 0; i < NUM_CNT_REGS; i++) {
			char buf[256];
			sprintf(buf, "Pulses Reg0%2o %lf pulse/sec", i + 024, vagc.cntreqsum[i] / sumdt);
			oapiWriteLog(buf);
			vagc.cntreqsum[i] = 0;
		}
		char *name[NUM_INTERRUPT_TYPES+1] = {
			"RSTRT", "T6RUPT", "T5RUPT", "T3RUPT" , "T4RUPT", "KEYRUPT1",
			"KEYRUPT2", "UPRUPT", "DOWNRUPT", "RADRPUPT", "HANDRUPT"
		};
		for (int i = 0; i <= NUM_INTERRUPT_TYPES; i++) {
			char buf[256];	
			sprintf(buf, "Ruptreq %8s idx %2d %lf pulse/sec", name[i], i, vagc.rptreqsum[i] / sumdt);
			oapiWriteLog(buf);
			vagc.rptreqsum[i] = 0;
		}
		sumdt = 0;
	}
	else
		sumdt += simdt;
	//sprintf(oapiDebugString(), "Reg046: %hu",  vagc.Erasable[0][RegRNRAD]);
	//int tmp = vagc.ExtraDelay;
	//sprintf(oapiDebugString(), "scaler:%10i scalercounter:%4i ChanSCALER1=%4hx exdly=%d", vagc.Scaler, vagc.ScalerCounter, vagc.InputChannel[ChanSCALER1], tmp);
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
	lem = (LEM *) OurVessel;
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
		vagc.ExtraDelay = 2; // GOJAM and TC 4000 both take 1 MCT to execute
		// No idea about the interrupts/pending/etc so we reset those
		vagc.AllowInterrupt = 1;
		vagc.PendFlag = 0;
		vagc.PendDelay = 0;
		// Don't disturb erasable core
		// IO channels are flip-flop based and should reset, but that's difficult, so we'll ignore it.
		// Reset standby flip-flop
		vagc.Standby = 0;
		// Turn on EL display and LGC Light (DSKYWarn).
		vagc.DskyChannel163 = 1;
		SetOutputChannel(0163, 1);
		// Light OSCILLATOR FAILURE and LGC WARNING bits to signify power transient, and be forceful about it.	
		// Those two bits are what causes the CWEA to notice.
		vagc.InputChannel[033] &= 037777;
		OutputChannel[033] &= 037777;
		// Also, simulate the operation of the VOLTAGE ALARM, turn off STBY and RESTART light while power is off.
		// The RESTART light will come on as soon as the AGC receives power again.
		// This happens externally to the AGC program. See CSM 104 SYS HBK pg 399
		vagc.VoltageAlarm = 1;
		vagc.RestartLight = 1;
		dsky.ClearRestart();
		dsky.ClearStby();
		// Reset last cycling time
		LastCycled = 0;
		// We should issue telemetry though.
		lem->VHF.Timestep(simt);

		// and do nothing more.
		return;
	}
	
	//
	// If MultiThread is enabled and the simulation is accellerated, the run vAGC in the AGC Thread,
	// otherwise run in main thread. at x1 acceleration, it is better to run vAGC totally synchronized
	//
	if (lem->isMultiThread && oapiGetTimeAcceleration() > 1.0) {
		Lock lock(agcCycleMutex);
		thread_simt = simt;
		thread_simdt = simdt;
		timeStepEvent.Raise();
	} else {
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

void LEMcomputer::ProcessChannel10(ChannelValue val) {
	dsky.ProcessChannel10(val);
	if (lem->HasProgramer) lem->lmp.ProcessChannel10(val);
}

// DS20060413

void LEMcomputer::ProcessChannel13(ChannelValue val){
	LEM *lem = (LEM *) OurVessel;	
	ChannelValue ch13;
	ch13 = val;
	if(ch13[EnableRHCCounter] && ch13[RHCRead]){
		int rhc_count[3];
		rhc_count[0] = (int)(lem->CDR_ACA.GetACAProp(0)*42.0);
		rhc_count[1] = (int)(lem->CDR_ACA.GetACAProp(1)*42.0);
		rhc_count[2] = (int)(lem->CDR_ACA.GetACAProp(2)*42.0);
		
		if (!lem->scca2.GetK3())
			WriteMemory(042,rhc_count[1]); // PITCH
		if (!lem->scca2.GetK2())
			WriteMemory(043,rhc_count[2]); // YAW
		if (!lem->scca2.GetK4())
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
	
	/*ChannelValue val12;
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
	}*/
}

void LEMcomputer::ProcessChannel141(ChannelValue val) {

	/*ChannelValue val12;
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
	}*/
}

void LEMcomputer::ProcessLGCThrustCommands(ChannelValue val) {

	//LEM *lem = (LEM *)OurVessel;
	lem->deca.ProcessLGCThrustCommands(val.to_ulong());
}

void LEMcomputer::ProcessAltMeterBits(ChannelValue val) {

	// ChannelValue val14;
	// val14 = GetOutputChannel(014);
	// LEM *lem = (LEM *)OurVessel;

	// if (val14[AltitudeRate])
	// {
	// 	lem->RadarTape.SetLGCAltitudeRate(val.to_ulong());
	// }
	// else
	// 	{
	// 	lem->RadarTape.SetLGCAltitude(val.to_ulong());
	// }
	switch (val.to_ullong()) {
	case 015:
		altval = (altval << 1) | 1;
		break;
	case 016:
		altval = (altval << 1);
		break;
	}
}

void LEMcomputer::ProcessChannel34(ChannelValue val)
{
	lem->aea.SetDownlinkTelemetryRegister(((OutputChannel[013] & 0100) << 9) | val.to_ulong());
}

void LEMcomputer::ProcessIMUCDUReadCount(int channel, int val) {
	SetErasable(0, channel, val);
	lem->aea.SetPGNSIntegratorRegister(channel, val);
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
			if (lem->atca.lgc_err_ena == 1) {
				// sprintf(oapiDebugString(),"LEM: LGC-ERR: RESET");
				lem->atca.lgc_err_x = 0;
				lem->atca.lgc_err_y = 0;
				lem->atca.lgc_err_z = 0;
			}
			lem->atca.lgc_err_ena = 0;
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

void LEMcomputer::SaveVesselSpecific(FILEHANDLE scn) {
	oapiWriteScenario_int(scn, "FFACTV", (int) ffactv);
	oapiWriteScenario_int(scn, "FFRADGTSET", (int) ffradgtset);
	oapiWriteScenario_int(scn, "FFALTGTSET", (int) ffaltgtset);
	oapiWriteScenario_int(scn, "RDRCNT", (int) rdrcnt);
	oapiWriteScenario_int(scn, "RDRVAL", rdrval);
	oapiWriteScenario_int(scn, "ALTVAL", altval);
}

void LEMcomputer::LoadVesselSpecific(char *line) {
	int tmp;
	if (papiReadScenario_int(line, "FFACTV", tmp))
		ffactv = tmp;
	else if (papiReadScenario_int(line, "FFRADGTSET", tmp))
		ffradgtset = tmp;
	else if (papiReadScenario_int(line, "FFALTGTSET", tmp))
		ffaltgtset = tmp;
	else if (papiReadScenario_int(line, "RDRCNT", tmp))
		rdrcnt = tmp;
	else if (papiReadScenario_int(line, "RDRVAL", tmp))
		rdrval = tmp;
	else if (papiReadScenario_int(line, "ALTVAL", tmp))
		altval = tmp;
}

VESSEL * LEMcomputer::GetCSM()
{
	OBJHANDLE hcsm = oapiGetVesselByName(OtherVesselName);
	if (hcsm)
	{
		VESSEL *CSMVessel = oapiGetVesselInterface(hcsm);
		return CSMVessel;
	}

	return NULL;
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
	if (lem->AOTLampFeeder.Voltage() > SP_MIN_ACVOLTAGE)
	{
		lem->AOTLampFeeder.DrawPower(9.3);
		lem->CabinHeat->GenerateHeat(9.3);
	}

	if (lem->HTR_AOT_CB.Voltage() > SP_MIN_DCVOLTAGE)
	{
		lem->HTR_AOT_CB.DrawPower(5.6);
		lem->CabinHeat->GenerateHeat(5.6);	//Not sure if all AOT heat radiates into the cabin, but since the heaters/mirrors are in the cabin portion of the AOT, we will do this.
	}
}

bool LMOptics::PaintReticleAngle(SURFHANDLE surf, SURFHANDLE digits) {
	int beta, srx, sry, digit[4];
	int x = (int)((-OpticsReticle)*100.0*DEG);
	if (x < 0) { x += 36000; }
	int z = 3; // Multiply by factor for the larger AOT reticle display bitmap (aot_font.bmp).
	beta = (x % 10);
	digit[0] = (x % 100) / 10;
	digit[1] = (x % 1000) / 100;
	digit[2] = (x % 10000) / 1000;
	digit[3] = x / 10000;
	sry = (int)((beta * 1.2) *z);
	srx = (8 *z) + ((digit[3] * 25) *z);
	oapiBlt(surf, digits, 0, 0, srx, 33 *z, 9 *z, 12 *z, SURF_PREDEF_CK);
	srx = (8 *z) + ((digit[2] * 25) *z);
	oapiBlt(surf, digits, 10 *z, 0, srx, 33 *z, 9 *z, 12 *z, SURF_PREDEF_CK);
	srx = (8 *z) + ((digit[1] * 25) *z);
	oapiBlt(surf, digits, 20 *z, 0, srx, 33 *z, 9 *z, 12 *z, SURF_PREDEF_CK);
	srx = (8 *z) + ((digit[0] * 25) *z);
	if (beta == 0) {
		oapiBlt(surf, digits, 30 *z, 0, srx, 33 *z, 9 *z, 12 *z, SURF_PREDEF_CK);
	}
	else {
		oapiBlt(surf, digits, 30 *z, sry, srx, 33 *z, 9 *z, (12 *z) - sry, SURF_PREDEF_CK);
		if (digit[0] == 9) digit[0] = 0; else digit[0]++;
		srx = (8 *z) + ((digit[0] * 25) *z);
		oapiBlt(surf, digits, 30 *z, 0, srx, (45 *z) - sry, 9 *z, sry, SURF_PREDEF_CK);
	}
	return true;
}

void LMOptics::Timestep(double simdt) {
	OpticsReticle = OpticsReticle + simdt * ReticleMoved;

	/*if (ReticleMoved)
	{
		sprintf(oapiDebugString(), "Optics Shaft %d, Optics Reticle %.2f, Moved? %.4f, KnobTurning %d", OpticsShaft, 360.0 - OpticsReticle / RAD, ReticleMoved, KnobTurning);
	}*/

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
