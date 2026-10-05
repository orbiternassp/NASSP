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

// To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"
#include "nassputils.h"
#include "resource.h"

#include "apolloguidance.h"
#include "dsky.h"
#include "CSMcomputer.h"
#include "toggleswitch.h"
#include "saturn.h"
#include "ioChannels.h"
#include "papi.h"
#include "Mission.h"
#include <thread>
#include <mutex>

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

	Start();
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
				std::lock_guard<std::mutex> guard(agcCycleMutex);
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
			// Light OSCILLATOR FAILURE and VOLTAGE FAIL to signify power transient, and be forceful about it.
			vagc.InputChannel[033] &= 037777;
			OutputChannel[033] &= 037777;
			vagc.InputChannel[077] |= 040;
			OutputChannel[077] |= 040;
			// Also turn off STBY and RESTART light while power is off.
			// The RESTART light will come on as soon as the AGC receives power again.
			// This happens externally to the AGC program. See CSM 104 SYS HBK pg 399
			vagc.RestartLight = 1;
			dsky.ClearRestart();
			dsky2.ClearRestart();
			dsky.ClearStby();
			dsky2.ClearStby();
			// Reset last cycling time
			LastCycled = 0;

			// We should issue telemetry though. Careful with the first timestep
			if (sat->pcm.last_update == 0)
			{
				sat->pcm.last_update = simt - simdt;
			}
			sat->pcm.TimeStep(simt);
			return;
		}

		//
		// Initial startup hack for Yaagc.
		//
		if(!PadLoaded) {

			//Get time reference for TEPHEM
			double TEPHEM0 = sat->pMission->GetTEPHEM0();

			// Synchronize clock with launch time (TEPHEM)
			double tephem;
			if (ProgramName == "Skylark048") //Only Skylark has a different address
			{
				tephem = vagc.Erasable[AGC_BANK(01702)][AGC_ADDR(01702)] +
					vagc.Erasable[AGC_BANK(01701)][AGC_ADDR(01701)] * pow((double) 2., (double) 14.) +
					vagc.Erasable[AGC_BANK(01700)][AGC_ADDR(01700)] * pow((double) 2., (double) 28.);
			}
			else
			{
				tephem = vagc.Erasable[AGC_BANK(01710)][AGC_ADDR(01710)] +
					vagc.Erasable[AGC_BANK(01707)][AGC_ADDR(01707)] * pow((double) 2., (double) 14.) +
					vagc.Erasable[AGC_BANK(01706)][AGC_ADDR(01706)] * pow((double) 2., (double) 28.);
			}

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
			std::lock_guard<std::mutex> guard(agcCycleMutex);
			thread_simt = simt;
			thread_simdt = simdt;
			timeStepEvent.Raise();
		} else {
			std::lock_guard<std::mutex> guard(agcCycleMutex);
			agcTimestep(simt, simdt);
		}

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

void CSMcomputer::GetRadarData(int radarBits)
{
	if (radarBits == 4)
	{
		sat->vhfranging.GetRangeCMC();
	}
}


//
// CM Optics class code
//

CMOptics::CMOptics() {

	sat = NULL;
	SextShaft = 0.0;
	TeleShaft = 0.0;
	SextTrunion = 0.0;
	TeleTrunion = 0.0;
	TeleShaftRate = 0.0;
	TeleTrunionRate = 0.0;
	dTrunion = 0.0;
	dShaft = 0.0;
	OpticsManualMovement = 0;
	Powered = 0;
	SextDualView = false;
	SextDVLOSTog = false;
	SextDVTimer = 0.0;
	OpticsCovered = true;
	OpticsVCDualViewFlashing = false;

	cmvcOptics.resize(NUM_MSHGRPS + NUM_RTCL); // 8 meshgroups from mesh + 2 extra for the reticles
	initVCOptics = true;
	CustomCam = true;
	VCOpticsRetAlpha = 0x80FFFFFF;
	ViewOpticsPanels = false;
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

bool CMOptics::PaintShaftDisplay(SURFHANDLE surf, SURFHANDLE digits, int TexMul){
	int value = (int)(TeleShaft*100.0*DEG);
	if (value < 0) { value += 36000; }
	return PaintDisplay(surf, digits, value, TexMul);
}

bool CMOptics::PaintTrunnionDisplay(SURFHANDLE surf, SURFHANDLE digits, int TexMul){
	int value = (int)(TeleTrunion*100.0*DEG);
	if (value < 0) { value += 36000; }
	return PaintDisplay(surf, digits, value, TexMul);
}

bool CMOptics::PaintDisplay(SURFHANDLE surf, SURFHANDLE digits, int value, int TexMul){
	int srx, sry, digit[5];
	int x=value;
	digit[0] = (x%10);
	digit[1] = (x%100)/10;
	digit[2] = (x%1000)/100;
	digit[3] = (x%10000)/1000;
	digit[4] = x/10000;

	srx = 8 + (digit[4] * 25);
	if (digit[4])
		sry = 33;
	else
		sry = 22;
	oapiBlt(surf, digits, 0, 0, srx*TexMul, sry*TexMul, 9*TexMul, 12*TexMul, SURF_PREDEF_CK);

	srx = 8 + (digit[3] * 25);
	if (digit[4] || digit[3])
		sry = 33;
	else
		sry = 22;
	oapiBlt(surf, digits, 10*TexMul, 0, srx*TexMul, sry*TexMul, 9*TexMul, 12*TexMul, SURF_PREDEF_CK);

	srx = 8 + (digit[2] * 25);
	oapiBlt(surf, digits, 20*TexMul, 0, srx*TexMul, 33*TexMul, 9*TexMul, 12*TexMul, SURF_PREDEF_CK);
	srx = 8 + (digit[1] * 25);
	oapiBlt(surf, digits, 30*TexMul, 0, srx*TexMul, 33*TexMul, 9*TexMul, 12*TexMul, SURF_PREDEF_CK);
	srx = 8 + (digit[0] * 25);
	sry = (int)(digit[0] * 1.2);
	oapiBlt(surf, digits, 40*TexMul, 0, srx*TexMul, 33*TexMul, 9*TexMul, 12*TexMul, SURF_PREDEF_CK);

	oapiColourFill(surf, oapiGetColour(255, 255, 255), 29*TexMul, 5*TexMul, 1*TexMul, 2*TexMul);
	return true;
}

void CMOptics::UpdateCMVCOptics()
{
	auto setVCCameraLOS = [](double shaft, double trunnion) noexcept {
		const double cosShaft = cos(shaft), sinShaft = sin(shaft);
		const double cosTrun = cos(trunnion), sinTrun = sin(trunnion);
		const double uzx = cosShaft * sinTrun, uzy = sinShaft * sinTrun, uzz = cosTrun;
		const double azimuth = asin(uzx), polar = -atan2(uzy, uzz);
		oapiCameraSetCockpitDir(polar, azimuth, false);
		};

	// If we are not in Optics view, Sextant or Teleskop, hide the VC Optics mesh
	if (sat->viewpos != SATVIEW_OPTICS_SCT && sat->viewpos != SATVIEW_OPTICS_SXT) {
		sat->SetMeshVisibilityMode(sat->hCMVCOpticsidx, MESHVIS_NEVER);
		return;
	}

	// If we are not in VC return
	if (!sat->vcmesh) return;
	if (oapiGetFocusInterface() != sat) return;

	VECTOR3 camPosGlobal, camPos, camDir, opticsPos, final_vertex;
	double aperture = 0.0;

	sat->SetCameraDefaultDirection(_V(0.0, -OPTICS_BASE_COS, OPTICS_BASE_SIN));
	oapiCameraSetCockpitDir(0, 0);
	sat->SetCameraCatchAngle(0.0);
	sat->SetCameraRotationRange(PI / 2., PI / 2., PI / 2., PI / 2.);
	bool isSextant = (sat->viewpos == SATVIEW_OPTICS_SXT);

	if (isSextant) { // Sextant
		bool isFlashing = OpticsVCDualViewFlashing;
		bool dualView = SextDualView;
		bool dvLOSTog = SextDVLOSTog;

		if (isFlashing && dualView && dvLOSTog) {
			setVCCameraLOS(SextShaft, 0.0);
			sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_SXT_CUSTOM_CAM, true);
		}
		else {
			setVCCameraLOS(SextShaft, SextTrunion);
			sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_SXT_CUSTOM_CAM, isFlashing || !dualView);
		}
		//		aperture = oapiCameraAperture() * multiplicator; // 1.2282;
		aperture = oapiCameraAperture() * 1.230;
	}
	else { // Telescope
		setVCCameraLOS(TeleShaft, TeleTrunion);
		//		aperture = oapiCameraAperture() * multiplicator2; //1.4637;
		aperture = oapiCameraAperture() * 1.467;
		// aperture = 1;	
	}

	// Get global camera position and direction. Is set in SATVIEW_OPTICS_SCT and SATVIEW_OPTICS_SXT
	oapiCameraGlobalPos(&camPosGlobal);
	oapiCameraGlobalDir(&camDir);

	MATRIX3 mRot;
	oapiCameraRotationMatrix(&mRot);
	// The up vector is the second column of the camera matrix.
	VECTOR3 gCamUp = _V(mRot.m12, mRot.m22, mRot.m32);

	// Transformation into the local ship system
	sat->Global2Local(camPosGlobal, camPos);

	// Local viewing direction
	VECTOR3 gTarget = camPosGlobal + camDir;
	VECTOR3 lTarget;
	sat->Global2Local(gTarget, lTarget);
	VECTOR3 lCamDir = lTarget - camPos;
	normalise(lCamDir);

	// Local Up Vector
	VECTOR3 gUpPos = camPosGlobal + gCamUp;
	VECTOR3 lUpPos;
	sat->Global2Local(gUpPos, lUpPos);
	VECTOR3 lCamUp = lUpPos - camPos;
	normalise(lCamUp);

	// Local Right Vector
	VECTOR3 lCamRight = crossp(lCamUp, lCamDir);
	normalise(lCamRight);

	VECTOR3 ofs;
	sat->GetMeshOffset(sat->vcidx, ofs);
	DEVMESHHANDLE hOpticsMesh = sat->GetDevMesh(sat->vis, sat->hCMVCOpticsidx);

	if (SextDualView) {
		// local custom camera direction
		VECTOR3 localDir = _V(0.0, -OPTICS_BASE_COS, OPTICS_BASE_SIN);

		// Calculate Local Up Vector to prevent image distortion
		// Since localDir only has Y and Z components,
		// we swap these and reverse one sign
		VECTOR3 localUp = _V(0.0, OPTICS_BASE_SIN, OPTICS_BASE_COS);

		// normalise the vectors
		normalise(localDir);
		normalise(localUp);

		UpdateOpticsCustomCam(camPos, localDir, localUp);

		// Superimposing using Sketchpad3 in Orbiter2016Beta or Sketchpad(DrawAPi) in OpenOrbiter
#ifdef _OPENORBITER
		oapi::Sketchpad* skp = oapiGetSketchpad(sat->srfOpticsCustomCam);
#else
		oapi::Sketchpad3* skp = (oapi::Sketchpad3*)oapiGetSketchpad(sat->srfOpticsCustomCam);
#endif // _OPENORBITER
		if (skp) {
			oapi::Brush* pBrush = oapiCreateBrush(VCOpticsRetAlpha);
			skp->SetBrush(pBrush);
			skp->SetBlendState(SKP_COPY_ALPHA);
			skp->Rectangle(0, 0, 2048, 2048);
			oapiReleaseBrush(pBrush);
			oapiReleaseSketchpad(skp);
		}
		oapiBlt(sat->srf[Saturn::SurfaceID_VC::SRF_VC_OPTICS_CUSTOMCAM], sat->srfOpticsCustomCam, 0, 0, 0, 0, 2048, 2048);
	}

	// Make copies of the mesh Vertices 
	if (initVCOptics) {
		MESHHANDLE hCVOptics = sat->GetMeshTemplate(sat->hCMVCOpticsidx); // handle for VC Optics Mesh

		// Order of mesh groups. This must be the same in the mesh
		// 0=Telescope eyepiece, 1=Sextant eyepiece, 2=dsky, 3=CMVCOptics_Panel_122, 4=Optics Clickpoints
		// 5=Custom Camera, 6=Optics Cover, 7=Telescope reticle, 8=Sextant reticle
		for (int i = FIRSTMSHGRP; i < NUM_MSHGRPS; i++) {
			cmvcOptics[i].mshgrp = oapiMeshGroup(hCVOptics, i);
			cmvcOptics[i].vtxcnt = cmvcOptics[i].mshgrp->nVtx;
			cmvcOptics[i].data.resize(cmvcOptics[i].vtxcnt);
			cmvcOptics[i].datanew.resize(cmvcOptics[i].vtxcnt);
			if (i > LASTMSHGRP) cmvcOptics[i + NUM_RTCL].data.resize(cmvcOptics[i].vtxcnt);

			for (int j = 0; j < cmvcOptics[i].vtxcnt; j++) {
				VECTOR3 vtx = _V(cmvcOptics[i].mshgrp->Vtx[j].x, cmvcOptics[i].mshgrp->Vtx[j].y, cmvcOptics[i].mshgrp->Vtx[j].z);
				cmvcOptics[i].data[j] = vtx;

				// We copy all the original mesh reticle vertices from the positions 6/7 of the mesh array to positions 8/9
				// This is needed for the rotation of the reticles. We need only the vertices. 
				if (i > LASTMSHGRP) cmvcOptics[i + NUM_RTCL].data[j] = vtx;
			}
			cmvcOptics[i].vertexdata.resize(cmvcOptics[i].vtxcnt);
			cmvcOptics[i].grp.Vtx = cmvcOptics[i].vertexdata.data();
			cmvcOptics[i].grp.nVtx = cmvcOptics[i].vtxcnt;
		}

		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_SCT_EYEPIECE, isSextant);
		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_SXT_EYEPIECE, !isSextant);
		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_OPTICS_DSKY, !ViewOpticsPanels);
		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_OPTICS_P122, !ViewOpticsPanels);
		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_OPTICS_CLKPNTS, true);
		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_SXT_CUSTOM_CAM, true);
		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_SCT_RETICLE, isSextant);
		sat->HideMeshGroup(sat->hCMVCOpticsidx, CMVC_SXT_RETICLE, !isSextant);
		sat->FovSaveVCOptics = 30 * RAD;

		initVCOptics = false;
		sat->CMVCOpticsInitP122Switches(); // Sync Panel 122 switches with the VC.
	}

	// Rotate Reticle
	if (!oapiGetPause()) { // *** oapiGetPause() maybe unnecessary ***
		double cos_a = std::cos(-TeleShaft);
		double sin_a = std::sin(-TeleShaft);
		for (int i = FIRSTRTCL; i < NUM_MSHGRPS; i++) { // If we wand also to rotate the reticle for the Custom camera we need to change i<7 to i<8
			for (int j = 0; j < cmvcOptics[i].vtxcnt; j++) {
				double rx = cmvcOptics[i + NUM_RTCL].data[j].x;
				double ry = cmvcOptics[i + NUM_RTCL].data[j].y;
				cmvcOptics[i].data[j].x = rx * cos_a - ry * sin_a;
				cmvcOptics[i].data[j].y = rx * sin_a + ry * cos_a;
				cmvcOptics[i].data[j].z = cmvcOptics[i + NUM_RTCL].data[j].z;
			}
		}
	}

	// Position the Opticsmesh 15cm in front of the camera
	opticsPos = camPos - ofs + (lCamDir * 0.15);

	GROUPEDITSPEC ges;
	ges.flags = GRPEDIT_VTXCRD;
	ges.vIdx = 0;

	// OPTIMIZATION: Multiply direction vectors once per frame by aperture to accelerate the loop
	VECTOR3 rScaled = lCamRight * aperture;
	VECTOR3 uScaled = lCamUp * aperture;
	VECTOR3 dScaled = lCamDir * aperture;

	// Transform Vertices
	for (int i = FIRSTMSHGRP; i < NUM_MSHGRPS; i++) {
		for (int j = 0; j < cmvcOptics[i].vtxcnt; j++) {
			VECTOR3 vtx = cmvcOptics[i].data[j];

			// Linear combination using pre-scaled vectors saves explicit vector multiplications
			final_vertex = rScaled * vtx.x + uScaled * vtx.y + dScaled * vtx.z;
			final_vertex += opticsPos;
			cmvcOptics[i].datanew[j] = final_vertex;

			cmvcOptics[i].grp.Vtx[j].x = (float)final_vertex.x;
			cmvcOptics[i].grp.Vtx[j].y = (float)final_vertex.y;
			cmvcOptics[i].grp.Vtx[j].z = (float)final_vertex.z;
		}

		// Send Mesh-Update to Orbiter
		ges.nVtx = cmvcOptics[i].vtxcnt;
		ges.Vtx = cmvcOptics[i].grp.Vtx;
		oapiEditMeshGroup(hOpticsMesh, i, &ges);
	}

	// UPDATE CLICKPOINTS
	double ClkArea = 0.0015 * aperture;     // Smaller CLickarea for the Switches
	double ClkAreaDSKY = 0.005 * aperture;  // Bigger for the DSKY

	for (int i = AID_VC_OPTICS_DSKY_VERB; i <= AID_VC_OPTICS_DSKY_RESET; i++) {
		oapiVCSetAreaClickmode_Spherical(i, cmvcOptics[CMVC_OPTICS_CLKPNTS].datanew[i - AID_VC_OPTICS_DSKY_VERB] + ofs, ClkAreaDSKY);
	}

	for (int i = AID_VC_OPTICS_ZERO_UP; i <= AID_VC_OPTICS_REJECT_BUTTON; i++) {
		oapiVCSetAreaClickmode_Spherical(i, cmvcOptics[CMVC_OPTICS_CLKPNTS].datanew[i - AID_VC_OPTICS_DSKY_VERB] + ofs, ClkArea);
	}

	oapiVCSetAreaClickmode_Spherical(AID_VC_OPTICS_HIDEPANELS, cmvcOptics[CMVC_OPTICS_CLKPNTS].datanew[AID_VC_OPTICS_HIDEPANELS - AID_VC_OPTICS_DSKY_VERB] + ofs, 0.015 * aperture);
	oapiVCSetAreaClickmode_Spherical(AID_VC_OPTICS_DUALVIEW_FLASHING, cmvcOptics[CMVC_OPTICS_CLKPNTS].datanew[AID_VC_OPTICS_DUALVIEW_FLASHING - AID_VC_OPTICS_DSKY_VERB] + ofs, 0.015 * aperture);
	oapiVCSetAreaClickmode_Spherical(AID_VC_OPTICS_RETICLE_PLUS, cmvcOptics[CMVC_OPTICS_CLKPNTS].datanew[AID_VC_OPTICS_RETICLE_PLUS - AID_VC_OPTICS_DSKY_VERB] + ofs, ClkArea);
	oapiVCSetAreaClickmode_Spherical(AID_VC_OPTICS_RETICLE_MINUS, cmvcOptics[CMVC_OPTICS_CLKPNTS].datanew[AID_VC_OPTICS_RETICLE_MINUS - AID_VC_OPTICS_DSKY_VERB] + ofs, ClkArea);

	// Update the DSKY. Instead of blitting every light and digits we simply blit the whole DSKY
	// from the other texture which all the lights and digits are already blittet.
	// This is done for every frame. I think it would be enough to do it at every timestep instead.
	oapiBlt(sat->srf[Saturn::SurfaceID_VC::SRF_VC_OPTICS_DSKY], sat->srf[Saturn::SurfaceID_VC::SRF_VC_4DSKY_LEB], 0, 0, 1754, 2107, 606, 400);

	sat->SetMeshVisibilityMode(sat->hCMVCOpticsidx, MESHVIS_VC);
}

// CustomCamera for Optics
void CMOptics::UpdateOpticsCustomCam(VECTOR3 camPos, VECTOR3 camDir, VECTOR3 camUp) {
	gcCore* pCore = gcGetCoreInterface();
	if (pCore) {
		// Scaling factor for the Superimposing Custom Camera best match so far is 0.905
		// This should be normaly (1.5 * RAD) but it's not working. Earlier tests was 1.073, but this was before i matched the 3D FOV to the 2D.
		sat->hOpticsCustomCam = pCore->SetupCustomCamera(sat->hOpticsCustomCam, oapiCameraTarget(), camPos, camDir, camUp, 0.905 * RAD, sat->srfOpticsCustomCam, CUSTOMCAM_DEFAULTS);
		if (CustomCam) {
			pCore->CustomCameraOnOff(sat->hOpticsCustomCam, true);
			CustomCam = false;
		}
	}
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

void CMOptics::VC_Optics_Reticle_Plus()
{
#ifdef _OPENORBITER
	VCOpticsRetAlpha = (std::clamp((int)(VCOpticsRetAlpha >> 24) + 1, 1, 255) << 24) | 0xFFFFFF;
#else
	VCOpticsRetAlpha = ((std::max)(1, (std::min)((int)(VCOpticsRetAlpha >> 24) + 1, 255)) << 24) | 0xFFFFFF;
#endif
}

void CMOptics::VC_Optics_Reticle_Minus()
{
#ifdef _OPENORBITER
	VCOpticsRetAlpha = (std::clamp((int)(VCOpticsRetAlpha >> 24) - 1, 1, 255) << 24) | 0xFFFFFF;
#else
	VCOpticsRetAlpha = ((std::max)(1, (std::min)((int)(VCOpticsRetAlpha >> 24) - 1, 255)) << 24) | 0xFFFFFF;
#endif
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
		if (TeleShaft > 150. * RAD) {
			OpticsCovered = false;			
			sat->SetOpticsCoverMesh();
			sat->JettisonOpticsCover();
			sat->HideVCOpticsCoverMesh();
		}
	}

	if (Powered == 0) { return; }

	//Rates
	if (sat->OpticsZeroSwitch.IsUp())
	{
		//Optics zero speed is twice the angle, limit to max drive rate
		ShaftRate = min(abs(2.0*SextShaft), 19.5*RAD);
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
		if (SextShaft > 0)
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
				dShaft += (A_s_dot*cos(SextShaft) - A_t_dot * sin(SextShaft)) / max(sin(10.0*RAD), sin(SextTrunion));
				dTrunion += A_s_dot * sin(SextShaft) + A_t_dot * cos(SextShaft);
			}
		}

		if (sat->agc.GetOutputChannelBit(012, DisengageOpticsDAC) == false && (sat->pMission->HasRateAidedOptics()) || sat->OpticsModeSwitch.IsUp())
		{
			//26mV per bit, 30.8 revolutions per second per volt, 1/3080 gear ratio (Shaft), 2/11780 gear ratio (Trunnion)
			dShaft += 0.026*30.8*PI2*1.0 / 3080.0*simdt*(double)sat->scdu.GetErrorCounter();
			dTrunion += 0.026*30.8*PI2*2.0 / 11780.0*simdt*(double)sat->tcdu.GetErrorCounter();
		}

		//sprintf(oapiDebugString(), "Trun Err: %lf Shaft Err: %lf", (double)sat->tcdu.GetErrorCounter()*180.0*pow(2, -14), (double)sat->scdu.GetErrorCounter()*180.0*pow(2, -12));
		//sprintf(oapiDebugString(), "Trun: %lf %d Shaft: %lf %d", dTrunion / simdt * DEG, sat->tcdu.GetErrorCounter(), dShaft / simdt * DEG, sat->scdu.GetErrorCounter());
	}

	SextShaft += dShaft;
	SextTrunion += dTrunion;

	//Limits
	if (SextShaft > 270.0*RAD)
	{
		SextShaft = 270.0*RAD;
	}
	if (SextShaft < -270.0*RAD)
	{
		SextShaft = -270.0*RAD;
	}
	if (SextTrunion < -10.0*RAD)
	{
		SextTrunion = -10.0*RAD;
	}
	if (SextTrunion > 59.0*RAD)
	{
		SextTrunion = 59.0*RAD;
	}

	sat->tcdu.SetReadCounter(SextTrunion * 4.0);
	sat->scdu.SetReadCounter(SextShaft);

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
		case THREEPOSSWITCH_DOWN:		// Fixed 25 degrees
			TeleTrunionTarget = 25.0*RAD;
			break;
	}

	//Telescope Servo Drive
	TelescopeServoDrive(simdt, TeleTrunionTarget, TeleTrunion, TeleTrunionRate);
	TelescopeServoDrive(simdt, SextShaft, TeleShaft, TeleShaftRate);
	//sprintf(oapiDebugString(), "TA %lf %lf %lf SH %lf %lf %lf", TeleTrunionTarget*DEG, TeleTrunion*DEG, TeleTrunionRate*DEG, SextShaft*DEG, TeleShaft*DEG, TeleShaftRate*DEG);

	//Limits
	if (TeleShaft > 270.0*RAD)
	{
		TeleShaft = 270.0*RAD;
		TeleShaftRate = 0.0;
	}
	if (TeleShaft < -270.0*RAD)
	{
		TeleShaft = -270.0*RAD;
		TeleShaftRate = 0.0;
	}
	if (TeleTrunion < -10.0*RAD)
	{
		TeleTrunion = -10.0*RAD;
		TeleTrunionRate = 0.0;
	}
	if (TeleTrunion > 59.0*RAD)
	{
		TeleTrunion = 59.0*RAD;
		TeleTrunionRate = 0.0;
	}

	//sprintf(oapiDebugString(), "Optics Shaft %.2f, Sext Trunion %.2f, Tele Trunion %.2f", OpticsShaft/RAD, SextTrunion/RAD, TeleTrunion/RAD);
	//sprintf(oapiDebugString(), "Sext Trunion EMEM %o", sat->agc.vagc.Erasable[0][RegOPTY]);
}

void CMOptics::TelescopeServoDrive(double dt, double sxt_angle, double &sct_angle, double &sct_rate)
{
	//Direct solution of the transfer function in the Apollo 15 Delco manual
	double C1, C2, TEMP1, TEMP2, TEMP3;
	static const double SCT_SERVO_CONST1 = 1.98673;
	static const double SCT_SERVO_CONST2 = 1.77;

	C2 = sct_angle - sxt_angle;
	C1 = (sct_rate + SCT_SERVO_CONST2 * C2) / SCT_SERVO_CONST1;
	TEMP3 = exp(-SCT_SERVO_CONST2 * dt);
	TEMP1 = TEMP3 * sin(SCT_SERVO_CONST1*dt);
	TEMP2 = TEMP3 * cos(SCT_SERVO_CONST1*dt);

	sct_rate = -SCT_SERVO_CONST2 * C1*TEMP1 + SCT_SERVO_CONST1 * C1*TEMP2 - SCT_SERVO_CONST1 * C2*TEMP1 - SCT_SERVO_CONST2 * C2*TEMP2;
	sct_angle = sxt_angle + C1 * TEMP1 + C2 * TEMP2;
}

void CMOptics::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, CMOPTICS_START_STRING);
	oapiWriteScenario_int(scn, "POWERED", Powered);
	oapiWriteScenario_int(scn, "OPTICSMANUALMOVEMENT", OpticsManualMovement);
	papiWriteScenario_double(scn, "OPTICSSHAFT", SextShaft); //Keep it named OPTICSSHAFT for backwards compatibility
	papiWriteScenario_double(scn, "SEXTTRUNION", SextTrunion);
	papiWriteScenario_double(scn, "TELESHAFT", TeleShaft);
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
			sscanf (line+11, "%lf", &SextShaft);
		}
		else if (!strnicmp (line, "SEXTTRUNION", 11)) {
			sscanf (line+11, "%lf", &SextTrunion);
		}
		else if (!strnicmp(line, "TELESHAFT", 9)) {
			sscanf(line + 9, "%lf", &TeleShaft);
		}
		else if (!strnicmp (line, "TELETRUNION", 11)) {
			sscanf (line+11, "%lf", &TeleTrunion);
		}
		papiReadScenario_bool(line, "OPTICSCOVERED", OpticsCovered); 
	}
}
