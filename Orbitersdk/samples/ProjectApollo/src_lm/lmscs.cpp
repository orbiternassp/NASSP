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
			lem->agc.SetInputChannelBit(030, GNControlOfSC,1); // Tell the LGC it has control.
			if(haspower == 1 && lem->CDR_SCS_ATCA_CB.Voltage() < 24){ haspower = 0; } // PNGS path requires this.
			if(lem->ModeControlPGNSSwitch.GetState() != THREEPOSSWITCH_DOWN){ hasdriver = 1; } // Drivers disabled when mode control off
			break;

		case TOGGLESWITCH_DOWN:  // ABORT MODE
			// In this case, we have to generate thruster demand ourselves, taking "suggestions" from the AGS.
			// FIXME: Implement this.
			lem->agc.SetInputChannelBit(030, GNControlOfSC,0); // Tell the LGC it doesn't have control
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

void ATCA::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, ATCA_START_STRING);

	papiWriteScenario_bool(scn, "DIRECTPITCHACTIVE", DirectPitchActive);
	papiWriteScenario_bool(scn, "DIRECTYAWACTIVE", DirectYawActive);
	papiWriteScenario_bool(scn, "DIRECTROLLACTIVE", DirectRollActive);

	oapiWriteLine(scn, ATCA_END_STRING);
}

void ATCA::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, ATCA_END_STRING, sizeof(ATCA_END_STRING))) {
			return;
		}
		papiReadScenario_bool(line, "DIRECTPITCHACTIVE", DirectPitchActive);
		papiReadScenario_bool(line, "DIRECTYAWACTIVE", DirectYawActive);
		papiReadScenario_bool(line, "DIRECTROLLACTIVE", DirectRollActive);
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
	dpsthrustcommand = 0;
	lgcAutoThrust = 0;
	LMR = 0.859;
}

void DECA::ResetRelays()
{
	K1 = false;
	K2 = false;
	K3 = false;
	K4 = false;
	K5 = false;
	K6 = false;
	K7 = false;
	K8 = false;
	K9 = false;
	K10 = false;
	K13 = false;
	K14 = false;
	K15 = false;
	K16 = false;
	K21 = false;
	K22 = false;
	K23 = false;
	K24 = false;
	K25 = false;
	K26 = false;
	K27 = false;
	K28 = false;
}

void DECA::Init(LEM *v, e_object *dcbus) {
	// Initialize
	lem = v;
	dc_source = dcbus;
}

void DECA::Timestep(double simdt) {
	powered = false;
	if (lem == NULL) return;

	if (lem->stage > 1)
	{
		//Set everything to false and then return
		lem->DPS.pitchGimbalActuator.ChangeLGCPosition(0);
		lem->DPS.rollGimbalActuator.ChangeLGCPosition(0);
		dpsthrustcommand = 0;

		ResetRelays();

		return;
	}

	//Needs voltage and a descent stage. The DECA is mounted on it.
	if (dc_source->Voltage() > SP_MIN_DCVOLTAGE && lem->stage < 2) {
		powered = true;
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

	//Descent Engine Control Section
	if ((lem->SCS_ENG_ARM_CB.IsPowered() || (lem->AbortSwitch.GetState() == 1 && lem->SCS_ENG_CONT_CB.IsPowered())) && lem->EngineArmSwitch.IsDown())
	{
		DEArm = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->AbortSwitch.GetState() == 0)
	{
		DEArm = true;
	}
	else
	{
		DEArm = false;
	}

	if (DEArm)
	{
		K1 = true;
		K10 = true;
		K23 = true;
		K25 = true;
	}
	else
	{
		K1 = false;
		K10 = false;
		K23 = false;
		K25 = false;
	}

	if (DEArm && !K26)
	{
		K2 = true;
		K24 = true;
	}
	else
	{
		K2 = false;
		K24 = false;
	}

	//Descent Engine Stop
	if (lem->SCS_ENG_CONT_CB.IsPowered() && (lem->EngineArmSwitch.IsUp() || lem->scca1.GetK20()))
	{
		K3 = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->ManualEngineStop.GetState() == 1)
	{
		K3 = true;
	}
	else
	{
		K3 = false;
	}

	//Descent Engine Start
	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->EngineArmSwitch.IsDown() && lem->scca2.GetK19() && !K3)
	{
		K7 = true;
	}
	else
	{
		K7 = false;
	}

	//Auto Engine On-Off
	bool X = lem->scca1.GetK18();
	bool Y = lem->scca1.GetK17();
	bool Q = K28;

	if ((X && !Y) || (Q && ((!X && !Y) || (X && Y))))
	{
		if (lem->SCS_DECA_PWR_CB.IsPowered() && (K1 || K23) && !K3)
		{
			K6 = true;
		}
		else
		{
			K6 = false;
		}
	}
	else
	{
		K6 = false;
	}

	if (lem->SCS_DECA_PWR_CB.IsPowered() && (lem->scca3.GetK6()) && (K7 || K6))
	{
		engOn = true;
		K16 = true;
	}
	else
	{
		engOn = false;
		K16 = false;
	}

	if (lem->SCS_DECA_PWR_CB.IsPowered() && lem->scca3.GetK6() && !K6)
	{
		K28 = true;
	}
	else
	{
		K28 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->THRContSwitch.IsDown())
	{
		K15 = true;
	}
	else
	{
		K15 = false;
	}

	if (!powered) //If off, send out all zeros
	{
		lem->DPS.pitchGimbalActuator.ChangeLGCPosition(0);
		lem->DPS.rollGimbalActuator.ChangeLGCPosition(0);
		return;
	}

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

	double newdpsthrustcommand = 0;

	//Process Throttle Commands
	if (!K15)
	{
		//Auto Thrust commands are generated in ProcessLGCThrustCommands()

		newdpsthrustcommand = lgcAutoThrust + lem->ttca_thrustcmd;
		if (newdpsthrustcommand > 0.925)
		{
			newdpsthrustcommand = 0.925;
		}
	}
	else
	{
		newdpsthrustcommand = lem->ttca_thrustcmd;
		lgcAutoThrust = 0.0;	//Reset auto throttle counter in manual mode
	}

	//DECA creates a voltage for the throttle command, this voltage can only change the thrust at a rate of 40,102 Newtons/second according to the GSOP.
	//Rounded this is 85.9% of the total throttle range, which should be a decent estimate for all missions.
	dposcmd = newdpsthrustcommand - dpsthrustcommand;
	poscmdsign = abs(newdpsthrustcommand - dpsthrustcommand) / (newdpsthrustcommand - dpsthrustcommand);
	if (abs(dposcmd)>LMR*simdt)
	{
		dpos = poscmdsign*LMR*simdt;
	}
	else
	{
		dpos = dposcmd;
	}

	dpsthrustcommand += dpos;

	lem->DPS.ThrottleActuator(dpsthrustcommand);

	//sprintf(oapiDebugString(), "engOn: %d engOff: %d Thrust: %f", engOn, engOff, dpsthrustcommand);
	//sprintf(oapiDebugString(), "Manual: K1 %d K3 %d K7 %d K10 %d K16 %d K23 %d K28 %d", K1, K3, K7, K10, K16, K23, K28);
	//sprintf(oapiDebugString(), "Auto: X %d Y %d Q %d K6 %d K10 %d K15 %d K16 %d K23 %d K28 %d", X, Y, Q, K6, K10, K15, K16, K23, K28);
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
	papiWriteScenario_bool(scn, "K1", K1);
	papiWriteScenario_bool(scn, "K2", K2);
	papiWriteScenario_bool(scn, "K3", K3);
	papiWriteScenario_bool(scn, "K4", K4);
	papiWriteScenario_bool(scn, "K5", K5);
	papiWriteScenario_bool(scn, "K6", K6);
	papiWriteScenario_bool(scn, "K7", K7);
	papiWriteScenario_bool(scn, "K8", K8);
	papiWriteScenario_bool(scn, "K9", K9);
	papiWriteScenario_bool(scn, "K10", K10);
	papiWriteScenario_bool(scn, "K13", K13);
	papiWriteScenario_bool(scn, "K14", K14);
	papiWriteScenario_bool(scn, "K15", K15);
	papiWriteScenario_bool(scn, "K16", K16);
	papiWriteScenario_bool(scn, "K21", K21);
	papiWriteScenario_bool(scn, "K22", K22);
	papiWriteScenario_bool(scn, "K23", K23);
	papiWriteScenario_bool(scn, "K24", K24);
	papiWriteScenario_bool(scn, "K25", K25);
	papiWriteScenario_bool(scn, "K26", K26);
	papiWriteScenario_bool(scn, "K27", K27);
	papiWriteScenario_bool(scn, "K28", K28);

	oapiWriteLine(scn, "DECA_END");
}

void DECA::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "DECA_END", sizeof("DECA_END"))) {
			return;
		}

		papiReadScenario_int(line, "PITCHACTUATORCOMMAND", pitchactuatorcommand);
		papiReadScenario_int(line, "ROLLACTUATORCOMMAND", rollactuatorcommand);
		papiReadScenario_double(line, "DPSTHRUSTCOMMAND", dpsthrustcommand);
		papiReadScenario_double(line, "LGCAUTOTHRUST", lgcAutoThrust);
		papiReadScenario_bool(line, "K1", K1);
		papiReadScenario_bool(line, "K2", K2);
		papiReadScenario_bool(line, "K3", K3);
		papiReadScenario_bool(line, "K4", K4);
		papiReadScenario_bool(line, "K5", K5);
		papiReadScenario_bool(line, "K6", K6);
		papiReadScenario_bool(line, "K7", K7);
		papiReadScenario_bool(line, "K8", K8);
		papiReadScenario_bool(line, "K9", K9);
		papiReadScenario_bool(line, "K10", K10);
		papiReadScenario_bool(line, "K13", K13);
		papiReadScenario_bool(line, "K14", K14);
		papiReadScenario_bool(line, "K15", K15);
		papiReadScenario_bool(line, "K16", K16);
		papiReadScenario_bool(line, "K21", K21);
		papiReadScenario_bool(line, "K22", K22);
		papiReadScenario_bool(line, "K23", K23);
		papiReadScenario_bool(line, "K24", K24);
		papiReadScenario_bool(line, "K25", K25);
		papiReadScenario_bool(line, "K26", K26);
		papiReadScenario_bool(line, "K27", K27);
		papiReadScenario_bool(line, "K28", K28);
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

SCCA1::SCCA1() :
	AbortStageDelay(0.4)
{
	K1 = false;
	K3 = false;
	K5 = false;
	K7 = false;
	K8 = false;
	K9 = false;
	K10 = false;
	K11 = false;
	K12 = false;
	K13 = false;
	K14 = false;
	K15 = false;
	K16 = false;
	K17 = false;
	K18 = false;
	K19 = false;
	K20 = false;
	K21 = false;
	K22 = false;
	K23 = false;
	K24 = false;
	K25 = false;
	K201 = false;
	K203 = false;
	K204 = false;
	K205 = false;
	K206 = false;
	K207 = false;
	AutoOn = false;
}

void SCCA1::Init(LEM *s)
{
	lem = s;
}

void SCCA1::Timestep(double simdt)
{
	if (lem == NULL) { return; }

	AbortStageDelay.Timestep(simdt);

	//Abort Stage Handling

	if (lem->SCS_ABORT_STAGE_CB.IsPowered() && lem->AbortStageSwitch.GetState() == 0)
	{
		K9 = true;
		K10 = true;
		AbortStageDelay.SetRunning(true);
		if (AbortStageDelay.ContactClosed())
		{
			K23 = true;
		}
		else
		{
			K23 = false;
		}
	}
	else
	{
		K9 = false;
		K10 = false;
		K23 = false;
	}

	if (lem->CDR_SCS_ABORT_STAGE_CB.IsPowered() && lem->AbortStageSwitch.GetState() == 0)
	{
		K19 = true;
		K20 = true;
		K21 = true;
		K201 = true;
	}
	else
	{
		K19 = false;
		K20 = false;
		K21 = false;
		K201 = false;
	}

	//Automatic

	if (lem->SCS_ENG_ARM_CB.IsPowered())
	{
		K18 = lem->scca2.GetAutoEngOn();
		K24 = K18;
		K17 = lem->scca2.GetAutoEngOff();
		K25 = K17;
	}
	else
	{
		K17 = false;
		K18 = false;
		K24 = false;
		K25 = false;
	}

	//Ascent Engine Logic Circuit
	if ((K24 && !K25) || ((K22 || K23) && (K24 && K25 || !K24 && !K25)))
	{
		AutoOn = true;
	}
	else
	{
		AutoOn = false;
	}

	//Manual

	if (lem->EngineArmSwitch.IsUp() && (lem->SCS_ENG_START_OVRD_CB.IsPowered() || lem->PROP_DISP_ENG_OVRD_LOGIC_CB.IsPowered()))
	{
		K22 = true;
		K206 = true;
	}
	else
	{
		K22 = false;
		K206 = false;
	}

	if (lem->SCS_ENG_START_OVRD_CB.IsPowered() && lem->EngineArmSwitch.IsUp() && lem->scca2.GetK19())
	{
		K11 = true;
		K12 = true;
	}
	else
	{
		K11 = false;
		K12 = false;
	}

	if (K206 && K22 && lem->EngineArmSwitch.IsUp() && lem->SCS_ENG_ARM_CB.IsPowered())
	{
		K13 = true;
	}
	else if (!K206 && !K22 && K23 && lem->AbortStageSwitch.GetState() == 0 && lem->SCS_ABORT_STAGE_CB.IsPowered())
	{
		K13 = true;
	}
	else
	{
		K13 = false;
	}

	if (AutoOn && lem->ManualEngineStop.GetState() == 0 && lem->EngineArmSwitch.IsUp())
	{
		K14 = true;
	}
	else if (AutoOn && lem->ManualEngineStop.GetState() == 0 && K21)
	{
		K14 = true;
	}
	else
	{
		K14 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->deca.GetK16())
	{
		K15 = true;
	}
	else
	{
		K15 = false;
	}

	if (K14 && (lem->SCS_AELD_CB.IsPowered() || lem->SCS_ENG_ARM_CB.IsPowered()))
	{
		K207 = true;
	}
	else
	{
		K207 = false;
	}

	if (lem->SCS_AELD_CB.IsPowered() && ((K11 && K12) || (K13 && K207)))
	{
		lem->eds.RelayBoxB.SetStagingRelay();
		thrustOn = true;
		K204 = true;
		K205 = true;
	}
	else if (lem->CDR_SCS_AELD_CB.IsPowered() && ((K11 && K207) || (K13 && K12)))
	{
		lem->eds.RelayBoxB.SetStagingRelay();
		thrustOn = true;
		K204 = true;
		K205 = true;
	}
	else
	{
		thrustOn = false;
		K204 = false;
		K205 = false;
	}

	if (lem->SCS_AELD_CB.IsPowered() && ((K11 && K207) || (K13 && K12)))
	{
		lem->eds.RelayBoxA.SetStagingRelay();
		armedOn = true;
		K16 = true;
		K203 = true;
	}
	else if (lem->CDR_SCS_AELD_CB.IsPowered() && ((K11 && K12) || (K13 && K207)))
	{
		lem->eds.RelayBoxA.SetStagingRelay();
		armedOn = true;
		K16 = true;
		K203 = true;
	}
	else
	{
		armedOn = false;
		K16 = false;
		K203 = false;
	}

	//Start LGC Abort Stage
	if (K10 || K21)
	{
		lem->agc.SetInputChannelBit(030, AbortWithAscentStage, true);
	}
	else
	{
		lem->agc.SetInputChannelBit(030, AbortWithAscentStage, false);
	}

	//Send engine fire commands to APS

	lem->APS.armedOn = armedOn;
	lem->APS.thrustOn = thrustOn;

	//sprintf(oapiDebugString(), "Manual: K19 %d K22 %d K11 %d K12 %d K13 %d K14 %d", K19, K22, K11, K12, K13, K14);
	//sprintf(oapiDebugString(), "Auto: K13 %d K207 %d K206 %d K14 %d K24 %d K25 %d AutoOn %d", K13, K207, K206, K14, K24, K25, AutoOn);
	//sprintf(oapiDebugString(), "Abort: K21 %d K23 %d K13 %d K14 %d AutoOn %d armedOn %d thrustOn %d", K21, K23, K13, K14, AutoOn, armedOn, thrustOn);
}

void SCCA1::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOON", AutoOn);
	papiWriteScenario_bool(scn, "ARMEDON", armedOn);
	papiWriteScenario_bool(scn, "THRUSTON", thrustOn);
	papiWriteScenario_bool(scn, "K1", K1);
	papiWriteScenario_bool(scn, "K3", K3);
	papiWriteScenario_bool(scn, "K5", K5);
	papiWriteScenario_bool(scn, "K7", K7);
	papiWriteScenario_bool(scn, "K8", K8);
	papiWriteScenario_bool(scn, "K9", K9);
	papiWriteScenario_bool(scn, "K10", K10);
	papiWriteScenario_bool(scn, "K11", K11);
	papiWriteScenario_bool(scn, "K12", K12);
	papiWriteScenario_bool(scn, "K13", K13);
	papiWriteScenario_bool(scn, "K14", K14);
	papiWriteScenario_bool(scn, "K15", K15);
	papiWriteScenario_bool(scn, "K16", K16);
	papiWriteScenario_bool(scn, "K17", K17);
	papiWriteScenario_bool(scn, "K18", K18);
	papiWriteScenario_bool(scn, "K19", K19);
	papiWriteScenario_bool(scn, "K20", K20);
	papiWriteScenario_bool(scn, "K21", K21);
	papiWriteScenario_bool(scn, "K22", K22);
	papiWriteScenario_bool(scn, "K23", K23);
	papiWriteScenario_bool(scn, "K24", K24);
	papiWriteScenario_bool(scn, "K25", K25);
	papiWriteScenario_bool(scn, "K201", K201);
	papiWriteScenario_bool(scn, "K203", K203);
	papiWriteScenario_bool(scn, "K204", K204);
	papiWriteScenario_bool(scn, "K205", K205);
	papiWriteScenario_bool(scn, "K206", K206);
	papiWriteScenario_bool(scn, "K207", K207);

	oapiWriteLine(scn, end_str);
}

void SCCA1::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "AUTOON", AutoOn);
		papiReadScenario_bool(line, "ARMEDON", armedOn);
		papiReadScenario_bool(line, "THRUSTON", thrustOn);
		papiReadScenario_bool(line, "K1", K1);
		papiReadScenario_bool(line, "K3", K3);
		papiReadScenario_bool(line, "K5", K5);
		papiReadScenario_bool(line, "K7", K7);
		papiReadScenario_bool(line, "K8", K8);
		papiReadScenario_bool(line, "K9", K9);
		papiReadScenario_bool(line, "K10", K10);
		papiReadScenario_bool(line, "K11", K11);
		papiReadScenario_bool(line, "K12", K12);
		papiReadScenario_bool(line, "K13", K13);
		papiReadScenario_bool(line, "K14", K14);
		papiReadScenario_bool(line, "K15", K15);
		papiReadScenario_bool(line, "K16", K16);
		papiReadScenario_bool(line, "K17", K17);
		papiReadScenario_bool(line, "K18", K18);
		papiReadScenario_bool(line, "K19", K19);
		papiReadScenario_bool(line, "K20", K20);
		papiReadScenario_bool(line, "K21", K21);
		papiReadScenario_bool(line, "K22", K22);
		papiReadScenario_bool(line, "K23", K23);
		papiReadScenario_bool(line, "K24", K24);
		papiReadScenario_bool(line, "K25", K25);
		papiReadScenario_bool(line, "K201", K201);
		papiReadScenario_bool(line, "K203", K203);
		papiReadScenario_bool(line, "K204", K204);
		papiReadScenario_bool(line, "K205", K205);
		papiReadScenario_bool(line, "K206", K206);
		papiReadScenario_bool(line, "K207", K207);

	}
}

SCCA2::SCCA2()
{
	K1 = false;
	K2 = false;
	K3 = false;
	K4 = false;
	K5 = false;
	K6 = false;
	K7 = false;
	K8 = false;
	K9 = false;
	K11 = false;
	K12 = false;
	K13 = false;
	K14 = false;
	K15 = false;
	K16 = false;
	K17 = false;
	K19 = false;
	K22 = false;
	K23 = false;
	K24 = false;
	AutoEngOn = false;
	AutoEngOff = false;
}

void SCCA2::Init(LEM *s)
{
	lem = s;
}

void SCCA2::Timestep(double simdt)
{
	if (lem == NULL) { return; }

	if (lem->CDR_SCS_ATCA_CB.IsPowered() && lem->GuidContSwitch.IsUp())
	{
		K1 = false;
		K2 = false;
		K3 = false;
		K4 = false;
		K5 = false;
		K6 = false;
		K7 = false;
		K8 = false;
		K9 = false;
		K11 = false;
		K12 = false;
		K13 = false;
	}
	if (lem->SCS_ATCA_CB.IsPowered() && lem->GuidContSwitch.IsDown())
	{
		K1 = true;
		K2 = true;
		K3 = true;
		K4 = true;
		K5 = true;
		K6 = true;
		K7 = true;
		K8 = true;
		K9 = true;
		K11 = true;
		K12 = true;
		K13 = true;
	}

	if (lem->SCS_ENG_START_OVRD_CB.IsPowered() && lem->ManualEngineStart.GetState() == 1)
	{
		K15 = true;
		K19 = true;
		K22 = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->ManualEngineStop.GetState() == 1)
	{
		K15 = false;
		K19 = false;
		K22 = false;
	}

	if (lem->SCS_ENG_ARM_CB.IsPowered() && !lem->EngineArmSwitch.IsCenter())
	{
		K14 = true;
	}
	else
	{
		K14 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->deca.GetK16())
	{
		K16 = true;
	}
	else
	{
		K16 = false;
	}

	if (K16 && lem->SCS_ENG_CONT_CB.IsPowered() && lem->deca.GetK16() && lem->GroundContact())
	{
		K17 = true;
	}
	else
	{
		K17 = false;
	}

	if (K7 && lem->THRContSwitch.IsDown())
	{
		lem->agc.SetInputChannelBit(030, AutoThrottle, false);
	}
	else
	{
		lem->agc.SetInputChannelBit(030, AutoThrottle, true);
	}

	if (K14)
	{
		lem->agc.SetInputChannelBit(030, EngineArmed, true);
	}
	else
	{
		lem->agc.SetInputChannelBit(030, EngineArmed, false);
	}

	ChannelValue val11;
	val11 = lem->agc.GetOutputChannel(011);

	if (K8)
	{
		//TBD: AGS Auto On Signal
		AutoEngOn = false;
	}
	else
	{
		AutoEngOn = val11[EngineOn];
	}

	if (K9)
	{
		//TBD: AGS Auto Off Signal
		AutoEngOff = false;
	}
	else
	{
		if (lem->ModeControlPGNSSwitch.IsUp())
		{
			AutoEngOff = val11[EngineOff];
		}
		else
		{
			AutoEngOff = false;
		}
	}
}

void SCCA2::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOENGON", AutoEngOn);
	papiWriteScenario_bool(scn, "AUTOENGOFF", AutoEngOff);
	papiWriteScenario_bool(scn, "K1", K1);
	papiWriteScenario_bool(scn, "K2", K2);
	papiWriteScenario_bool(scn, "K3", K3);
	papiWriteScenario_bool(scn, "K4", K4);
	papiWriteScenario_bool(scn, "K5", K5);
	papiWriteScenario_bool(scn, "K6", K6);
	papiWriteScenario_bool(scn, "K7", K7);
	papiWriteScenario_bool(scn, "K8", K8);
	papiWriteScenario_bool(scn, "K9", K9);
	papiWriteScenario_bool(scn, "K11", K11);
	papiWriteScenario_bool(scn, "K12", K12);
	papiWriteScenario_bool(scn, "K13", K13);
	papiWriteScenario_bool(scn, "K14", K14);
	papiWriteScenario_bool(scn, "K15", K15);
	papiWriteScenario_bool(scn, "K16", K16);
	papiWriteScenario_bool(scn, "K17", K17);
	papiWriteScenario_bool(scn, "K19", K19);
	papiWriteScenario_bool(scn, "K22", K22);
	papiWriteScenario_bool(scn, "K23", K23);
	papiWriteScenario_bool(scn, "K24", K24);

	oapiWriteLine(scn, end_str);
}

void SCCA2::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "AUTOENGON", AutoEngOn);
		papiReadScenario_bool(line, "AUTOENGOFF", AutoEngOff);
		papiReadScenario_bool(line, "K1", K1);
		papiReadScenario_bool(line, "K2", K2);
		papiReadScenario_bool(line, "K3", K3);
		papiReadScenario_bool(line, "K4", K4);
		papiReadScenario_bool(line, "K5", K5);
		papiReadScenario_bool(line, "K6", K6);
		papiReadScenario_bool(line, "K7", K7);
		papiReadScenario_bool(line, "K8", K8);
		papiReadScenario_bool(line, "K9", K9);
		papiReadScenario_bool(line, "K11", K11);
		papiReadScenario_bool(line, "K12", K12);
		papiReadScenario_bool(line, "K13", K13);
		papiReadScenario_bool(line, "K14", K14);
		papiReadScenario_bool(line, "K15", K15);
		papiReadScenario_bool(line, "K16", K16);
		papiReadScenario_bool(line, "K17", K17);
		papiReadScenario_bool(line, "K19", K19);
		papiReadScenario_bool(line, "K22", K22);
		papiReadScenario_bool(line, "K23", K23);
		papiReadScenario_bool(line, "K24", K24);

	}
}

SCCA3::SCCA3()
{
	EngStopPower = false;
	K1_1 = false;
	K2_1 = false;
	K3_1 = false;
	K4_1 = false;
	K5_1 = false;
	K6_1 = false;
	K1_2 = false;
	K2_2 = false;
	K3_2 = false;
	K4_2 = false;
	K5_2 = false;
	K6_2 = false;
	K7_3 = false;
}

void SCCA3::Init(LEM *s)
{
	lem = s;
}

void SCCA3::Timestep(double simdt)
{
	if (lem == NULL) { return; }

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->AbortSwitch.GetState() == 1 && lem->EngineArmSwitch.IsDown())
	{
		EngStopPower = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->AbortSwitch.GetState() == 0)
	{
		EngStopPower = true;
	}
	else if (lem->SCS_ENG_ARM_CB.IsPowered() && lem->EngineArmSwitch.IsDown())
	{
		EngStopPower = true;
	}
	else
	{
		EngStopPower = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->ManualEngineStop.GetState() == 1)
	{
		K4_1 = true;
	}
	else
	{
		K4_1 = false;
	}

	if (EngStopPower && lem->ManualEngineStop.GetState() == 1)
	{
		K4_2 = true;
	}
	else
	{
		K4_2 = false;
	}

	if (EngStopPower && lem->ManualEngineStop.GetState() == 0 && !lem->scca1.GetK9())
	{
		K6_1 = true;
		K6_2 = true;
	}
	else
	{
		K6_1 = false;
		K6_2 = false;
	}

	if (lem->SCS_ABORT_STAGE_CB.IsPowered() && lem->AbortStageSwitch.GetState() == 1 && lem->EngineDescentCommandOverrideSwitch.IsUp() && !K4_1 && !K4_2)
	{
		K5_1 = true;
	}
	else
	{
		K5_1 = false;
	}

	if (EngStopPower && lem->ManualEngineStop.GetState() == 0 && !lem->scca1.GetK9() && lem->EngineDescentCommandOverrideSwitch.IsUp() && !K4_1 && !K4_2)
	{
		K5_2 = true;
	}
	else
	{
		K5_2 = false;
	}

	//sprintf(oapiDebugString(), "DE Command Override: K4 %d %d K5 %d %d K6 %d %d", K4_1, K4_2, K5_1, K5_2, K6_1, K6_2);
}

void SCCA3::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "K1_1", K1_1);
	papiWriteScenario_bool(scn, "K2_1", K2_1);
	papiWriteScenario_bool(scn, "K3_1", K3_1);
	papiWriteScenario_bool(scn, "K4_1", K4_1);
	papiWriteScenario_bool(scn, "K5_1", K5_1);
	papiWriteScenario_bool(scn, "K6_1", K6_1);
	papiWriteScenario_bool(scn, "K1_2", K1_2);
	papiWriteScenario_bool(scn, "K2_2", K2_2);
	papiWriteScenario_bool(scn, "K3_2", K3_2);
	papiWriteScenario_bool(scn, "K4_2", K4_2);
	papiWriteScenario_bool(scn, "K5_2", K5_2);
	papiWriteScenario_bool(scn, "K6_2", K6_2);
	papiWriteScenario_bool(scn, "K7_3", K7_3);

	oapiWriteLine(scn, end_str);
}

void SCCA3::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "K1_1", K1_1);
		papiReadScenario_bool(line, "K2_1", K2_1);
		papiReadScenario_bool(line, "K3_1", K3_1);
		papiReadScenario_bool(line, "K4_1", K4_1);
		papiReadScenario_bool(line, "K5_1", K5_1);
		papiReadScenario_bool(line, "K6_1", K6_1);
		papiReadScenario_bool(line, "K1_2", K1_2);
		papiReadScenario_bool(line, "K2_2", K2_2);
		papiReadScenario_bool(line, "K3_2", K3_2);
		papiReadScenario_bool(line, "K4_2", K4_2);
		papiReadScenario_bool(line, "K5_2", K5_2);
		papiReadScenario_bool(line, "K6_2", K6_2);
		papiReadScenario_bool(line, "K7_3", K7_3);
	}
}