/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Stabilization & Control System (and associated parts)

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
#include <stdio.h>
#include <math.h>
#include "soundlib.h"
#include "lmresource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "LEMcomputer.h"
#include "IMU.h"
// #include "saturn.h"
#include "lm_channels.h"
#include "tracer.h"
#include "papi.h"
#include "LEM.h"

#define DECA_AUTOTHRUST_STEP 0.00026828571

// ATTITUDE & TRANSLATION CONTROL ASSEMBLY
ATCA::ATCA(){
	lem = NULL;
	DirectPitchActive = false;
	DirectYawActive = false;
	DirectRollActive = false;
}

void ATCA::Init(LEM *vessel){
	lem = vessel;
	int x=0; while(x < 16){ jet_request[x] = 0; jet_last_request[x] = 0; jet_start[x] = 0; jet_stop[x] = 0; x++; }
}
// GuidContSwitch is the Guidance Control switch

void ATCA::Timestep(double simt){
	double now = oapiGetSimTime(); // Get time
	int haspower = 0,hasdriver = 0,balcpl = 0;
	if(lem == NULL){ return; }
	// Fetch mode switch setting.
	int GC_Mode = lem->GuidContSwitch.GetState();
	// Determine ATCA power situation.
	if(lem->SCS_ATCA_CB.Voltage() > 24){
		// ATCA primary power is on.
		haspower = 1;
	}

	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/

	// *** Determine jet request source path ***
	switch(GC_Mode){
		case TOGGLESWITCH_UP:    // PGNS MODE
			// In this case, thruster demand is direct from the LGC. We have nothing to do.
			if(lem->agc.Yaagc){	lem->agc.SetInputChannelBit(030, GNControlOfSC,1); } // Tell the LGC it has control.
			if(haspower == 1 && lem->CDR_SCS_ATCA_CB.Voltage() < 24){ haspower = 0; } // PNGS path requires this.
			if(lem->ModeControlPNGSSwitch.GetState() != THREEPOSSWITCH_DOWN){ hasdriver = 1; } // Drivers disabled when mode control off
			break;

		case TOGGLESWITCH_DOWN:  // ABORT MODE
			// In this case, we have to generate thruster demand ourselves, taking "suggestions" from the AGS.
			// FIXME: Implement this.
			if(lem->agc.Yaagc){	lem->agc.SetInputChannelBit(030, GNControlOfSC,0); } // Tell the LGC it doesn't have control
			if(haspower == 1 && lem->SCS_ATCA_AGS_CB.Voltage() < 24){ haspower = 0; } // AGS path requires this.
			if(lem->ModeControlAGSSwitch.GetState() != THREEPOSSWITCH_DOWN){ hasdriver = 1; } // Drivers disabled when mode control off
			break;
	}
	// *** Test "Balanced Couples" switch ***
	if(lem->BALCPLSwitch.GetState() == TOGGLESWITCH_UP){ balcpl = 1; }

	// *** THRUSTER MAINTENANCE ***
	// LM RCS thrusters take 12.5ms to ramp up to full thrust and 17.5ms to ramp back down. There is a dead time of 10ms before thrust starts.
	// The upshot of this is that full thrust always starts 20ms after commanded and stops 8ms after commanded. 
	// Orbiter has no hope of providing enough resolution to properly map this. It depends on framerate.
	// We can do our best though...
	int x=0;
	while(x < 16){
		double power=0;
		// If the ATCA is not powered or driver voltage is absent, it won't work.
		if(haspower != 1 || hasdriver != 1){ jet_request[x] = 0; }
		// If the "Balanced Couples" switch is off, the abort preamps for the four upward-firing thrusters are disabled.
		if(GC_Mode == TOGGLESWITCH_DOWN && balcpl != 1 && (x == 0 || x == 4 || x == 8 || x == 12)){ jet_request[x] = 0;	}
		// Process jet request list to generate start and stop times.
		if(jet_request[x] == 1 && jet_last_request[x] == 0){
			// New fire request
			jet_start[x] = now;
		}else if(jet_request[x] == 0 && jet_last_request[x] == 1){
			// New stop request
			jet_stop[x] = now;
		}
		jet_last_request[x] = jet_request[x]; // Keep track of changes

		if(jet_start[x] == 0 && jet_stop[x] == 0){ x++; continue; } // Done
		// sprintf(oapiDebugString(),"Jet %d fire %f stop %f",x,jet_start[x],jet_stop[x]); 
		if(simt > jet_start[x]+0.01 && simt < jet_start[x]+0.0125){
			// Ramp up
			power = ((simt-jet_start[x])/0.0125);			
		}
		if(jet_stop[x] > 0 && (simt > jet_stop[x]+0.0075 && simt < jet_stop[x]+0.0175)){
		    // Ramp down
			power = 1.0 - ((simt - jet_stop[x] - 0.0075) / 0.01);
		}
		if(jet_stop[x] > 0 && simt > jet_stop[x]+0.0175){
			// Thruster off
			power=0; jet_start[x] = 0; jet_stop[x] = 0;
		}else{
			if(simt > jet_start[x]+0.0125){
				// Full Power
				power=1;
			}
		}
		// FIXME: This is just for testing.
		// if(power > 0.25){ power = 0.25; }
		lem->SetRCSJetLevelPrimary(x,power);
		x++;
	}
}


// Process thruster commands from LGC
void ATCA::ProcessLGC(int ch, int val){		
	if(lem->GuidContSwitch.GetState() != TOGGLESWITCH_UP){ val = 0; } // If not in primary mode, force jets off (so jets will switch off at programmed times)
	// When in primary, thruster commands are passed from LGC to jets.
	switch(ch){
		case 05:
			LMChannelValue5 ch5;
			ch5.Value = val;			
			if(ch5.Bits.B4U != 0){ jet_request[12] = 1; }else{ jet_request[12] = 0; }
			if(ch5.Bits.A4D != 0){ jet_request[15] = 1; }else{ jet_request[15] = 0; }
			if(ch5.Bits.A3U != 0){ jet_request[8]  = 1; }else{ jet_request[8]  = 0; }
			if(ch5.Bits.B3D != 0){ jet_request[11] = 1; }else{ jet_request[11] = 0; }
			if(ch5.Bits.B2U != 0){ jet_request[4]  = 1; }else{ jet_request[4]  = 0; }
			if(ch5.Bits.A2D != 0){ jet_request[7]  = 1; }else{ jet_request[7]  = 0; }
			if(ch5.Bits.A1U != 0){ jet_request[0]  = 1; }else{ jet_request[0]  = 0; }
			if(ch5.Bits.B1D != 0){ jet_request[3]  = 1; }else{ jet_request[3]  = 0; }			
			break;
		case 06:
			LMChannelValue6 ch6;
			ch6.Value = val;
			if(ch6.Bits.B3A != 0){ jet_request[10] = 1; }else{ jet_request[10] = 0; }
			if(ch6.Bits.B4F != 0){ jet_request[13] = 1; }else{ jet_request[13] = 0; }
			if(ch6.Bits.A1F != 0){ jet_request[1]  = 1; }else{ jet_request[1]  = 0; }
			if(ch6.Bits.A2A != 0){ jet_request[6]  = 1; }else{ jet_request[6]  = 0; }
			if(ch6.Bits.B2L != 0){ jet_request[5]  = 1; }else{ jet_request[5]  = 0; }
			if(ch6.Bits.A3R != 0){ jet_request[9]  = 1; }else{ jet_request[9]  = 0; }
			if(ch6.Bits.A4R != 0){ jet_request[14] = 1; }else{ jet_request[14] = 0; }
			if(ch6.Bits.B1L != 0){ jet_request[2]  = 1; }else{ jet_request[2]  = 0; }			
			break;
		default:
			sprintf(oapiDebugString(),"ATCA::ProcessLGC: Bad channel %o",ch);
	}
}

// DESCENT ENGINE CONTROL ASSEMBLY
DECA::DECA() {
	lem = NULL;
	powered = FALSE;
	dc_source = NULL;
	pitchactuatorcommand = 0;
	rollactuatorcommand = 0;
	engOn = false;
	engOff = false;
	dpsthrustcommand = 0;
	lgcAutoThrust = 0;
}

void DECA::Init(LEM *v, e_object *dcbus) {
	// Initialize
	lem = v;
	dc_source = dcbus;
}

void DECA::Timestep(double simt) {
	powered = false;
	if (lem == NULL) return;

	//Needs voltage and a descent stage. The DECA is mounted on it.
	if (dc_source->Voltage() > SP_MIN_DCVOLTAGE && lem->stage < 2) {
		powered = true;
	}

	if (!powered) //If off, send out all zeros
	{
		lem->DPS.pitchGimbalActuator.ChangeLGCPosition(0);
		lem->DPS.rollGimbalActuator.ChangeLGCPosition(0);
		lem->DPS.engArm = false;
		lem->DPS.thrustOn = false;
		lem->DPS.thrustOff = false;
		dpsthrustcommand = 0;

		return;
	}

	//Process input and output
	//Input:
	//-Descent Engine Arm (Switch)
	//-Manual Descent Engine Start-Stop Commands (Buttons)
	//-Automatic/Manual Throttle Select (Switch)
	//-Automatic Descent Engine on-off commands (PGNS)
	//-Automatic PGNS Descent Engine Trim Commands (PGNS)
	//-Automatic Throttle Commands (PGNS)
	//-Gimbal Feedback Commands (Actuators)
	//-Manual Descent Engine Throttle Commands (TTCA)
	//-Automatic AGS Descent Engine Trim Commands (ATCA)
	//
	//Output:
	//-Descent Engine Trim Indication (PGNS)
	//-Throttle Commands (DPS)
	//-Engine On-Off Commands (DPS)
	//-Engine Arm Command (DPS)
	//-Gimbal Trim Commands(Actuators)

	ChannelValue val11, val12;
	val11 = lem->agc.GetOutputChannel(011);
	val12 = lem->agc.GetOutputChannel(012);

	if (lem->EngineArmSwitch.IsDown())
	{
		if (lem->GuidContSwitch.IsUp())
		{
			//Process Pitch Gimbal Actuator command
			int valx = val12[PlusPitchVehicleMotion];
			int valy = val12[MinusPitchVehicleMotion];
			pitchactuatorcommand = valx - valy;

			//Process Roll Gimbal Actuator command
			valx = val12[PlusRollVehicleMotion];
			valy = val12[MinusRollVehicleMotion];
			rollactuatorcommand = valx - valy;
		}
		else
		{
			//TBD: AGS Trim Commands
			pitchactuatorcommand = 0;
			rollactuatorcommand = 0;
		}
	}

	lem->DPS.pitchGimbalActuator.ChangeLGCPosition(pitchactuatorcommand);
	lem->DPS.rollGimbalActuator.ChangeLGCPosition(rollactuatorcommand);

	//Gimbal Failure Indication
	if (lem->DPS.pitchGimbalActuator.GimbalFail() || lem->DPS.rollGimbalActuator.GimbalFail())
	{
		lem->agc.SetInputChannelBit(032, ApparentDecscentEngineGimbalsFailed, 1);
	}

	//Engine arm
	lem->DPS.engArm = lem->EngineArmSwitch.IsDown();

	//Engine On-Off
	if (lem->GuidContSwitch.IsUp())	//PGNS signal
	{
		if (val11[EngineOn])
		{
			engOn = true;
		}
		else
		{
			engOn = false;
		}
		if (val11[EngineOff])
		{
			engOff = true;
		}
		else
		{
			engOff = false;
		}
	}
	else
	{
		//TBD: AGS signal
		engOn = false;
		engOff = false;
	}

	//Manual engine start signal, overrides LGC and AGS
	if (lem->ManualEngineStart.GetState() == 1)
	{
		engOn = true;
		engOff = false;
	}
	
	//Manual engine stop signal, overrides LGC and AGS
	if (lem->ManualEngineStop.GetState() == 1)
	{
		engOn = false;
		engOff = true;
	}

	//Send thrust signals to DPS
	lem->DPS.thrustOn = engOn;
	lem->DPS.thrustOff = engOff;

	//Process Throttle Commands
	if (lem->THRContSwitch.IsUp())
	{
		//Auto Thrust commands are generated in ProcessLGCThrustCommands()

		dpsthrustcommand = lgcAutoThrust + lem->ttca_thrustcmd;
		if (dpsthrustcommand > 0.925)
		{
			dpsthrustcommand = 0.925;
		}
	}
	else
	{
		dpsthrustcommand = lem->ttca_thrustcmd;
		lgcAutoThrust = 0.0;	//Reset auto throttle counter in manual mode
	}

	lem->DPS.thrustcommand = dpsthrustcommand;

	//sprintf(oapiDebugString(), "engOn: %d engOff: %d Thrust: %f", engOn, engOff, dpsthrustcommand);
}

void DECA::ProcessLGCThrustCommands(int val) {

	int pulses;
	double thrust_cmd;

	if (powered == 0) { return; }

	if (val & 040000) { // Negative
		pulses = -((~val) & 077777);
	}
	else {
		pulses = val & 077777;
	}

	thrust_cmd = (DECA_AUTOTHRUST_STEP*pulses);

	lgcAutoThrust += thrust_cmd;

	if (lgcAutoThrust > 0.825)
	{
		lgcAutoThrust = 0.825;
	}
	else if (lgcAutoThrust < 0)
	{
		lgcAutoThrust = 0.0;
	}

	//sprintf(oapiDebugString(), "Thrust val: %o, Thrust pulses: %d, thrustchange: %f, lgcAutoThrust: %f", val, pulses, thrust_cmd, lgcAutoThrust);
}

void DECA::SystemTimestep(double simdt) {

	if (powered && dc_source)
		dc_source->DrawPower(113.0);  // take DC power
}

void DECA::SaveState(FILEHANDLE scn) {

	// START_STRING is written in LEM
	oapiWriteScenario_int(scn, "PITCHACTUATORCOMMAND", pitchactuatorcommand);
	oapiWriteScenario_int(scn, "ROLLACTUATORCOMMAND", rollactuatorcommand);
	papiWriteScenario_double(scn, "DPSTHRUSTCOMMAND", dpsthrustcommand);
	papiWriteScenario_double(scn, "LGCAUTOTHRUST", lgcAutoThrust);

	oapiWriteLine(scn, "DECA_END");
}

void DECA::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "DECA_END", sizeof("DECA_END"))) {
			return;
		}

		if (!strnicmp(line, "PITCHACTUATORCOMMAND", 20)) {
			sscanf(line + 20, "%d", &rollactuatorcommand);
		}
		else if (!strnicmp(line, "ROLLACTUATORCOMMAND", 19)) {
			sscanf(line + 19, "%d", &pitchactuatorcommand);
		}
		else if (!strnicmp(line, "DPSTHRUSTCOMMAND", 16)) {
			sscanf(line + 16, "%lf", &dpsthrustcommand);
		}
		else if (!strnicmp(line, "LGCAUTOTHRUST", 13)) {
			sscanf(line + 13, "%lf", &lgcAutoThrust);
		}
	}
}

GASTA::GASTA()
{
	imu_att = _V(0, 0, 0);
	gasta_att = _V(0, 0, 0);
}

void GASTA::Init(LEM *v, e_object *dcsource, e_object *acsource, IMU* imu) {
	// Initialize
	lem = v;
	dc_source = dcsource;
	ac_source = acsource;
	this->imu = imu;
}

bool GASTA::IsPowered()
{
	if (ac_source && dc_source) {
		if (ac_source->Voltage() > SP_MIN_ACVOLTAGE && dc_source->Voltage() > SP_MIN_DCVOLTAGE)
		{
			return true;
		}
	}
	return false;
}

void GASTA::Timestep(double simt)
{
	if (lem == NULL) return;

	if (!IsPowered())
	{
		gasta_att = _V(0, 0, 0);	//I guess the FDAI would show all zeros when no attitude signal is supplied? It's not like turning off BMAG power in the CSM. There the attitude freezes.
		return;
	}

	//This is all I do. P.S. Now I do more!
	imu_att = imu->GetTotalAttitude();

	gasta_att.z = asin(-cos(imu_att.z)*sin(imu_att.x));
	if (abs(sin(gasta_att.z)) != 1.0)
	{
		gasta_att.y = atan2(((sin(imu_att.y)*cos(imu_att.x) + cos(imu_att.y)*sin(imu_att.z)*sin(imu_att.x)) / cos(gasta_att.z)), (cos(imu_att.y)*cos(imu_att.x) - sin(imu_att.y)*sin(imu_att.z)*sin(imu_att.x)) / cos(gasta_att.z));
	}

	if (abs(sin(gasta_att.z)) != 1.0)
	{
		gasta_att.x = atan2(sin(imu_att.z), cos(imu_att.z)*cos(imu_att.x));
	}

	//Map angles between 0° and 360°, just to be sure
	if (gasta_att.x < 0)
	{
		gasta_att.x += PI2;
	}
	if (gasta_att.y < 0)
	{
		gasta_att.y += PI2;
	}
	if (gasta_att.z < 0)
	{
		gasta_att.z += PI2;
	}

	//sprintf(oapiDebugString(), "OGA: %f, IGA: %f, MGA: %f, Roll: %f, Pitch: %f, Yaw: %f", imu_att.x*DEG, imu_att.y*DEG, imu_att.z*DEG, gasta_att.x*DEG, gasta_att.y*DEG, gasta_att.z*DEG);
}

void GASTA::SystemTimestep(double simdt)
{
	if (IsPowered())
	{
		if (ac_source)
		{
			ac_source->DrawPower(10); //10 Watts from AC BUS A
		}
		if (dc_source)
		{
			dc_source->DrawPower(7.8); //7.8 Watts from CDR DC BUS
		}
	}
}