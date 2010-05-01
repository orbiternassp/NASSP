/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: LEM-specific switches

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

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.4  2009/09/10 02:12:37  dseagrav
  *	Added lm_ags and lm_telecom files, LM checkpoint commit.
  *	
  *	Revision 1.3  2009/08/16 03:12:38  dseagrav
  *	More LM EPS work. CSM to LM power transfer implemented. Optics bugs cleared up.
  *	
  *	Revision 1.2  2009/08/10 02:23:06  dseagrav
  *	LEM EPS (Part 2)
  *	Split ECAs into channels, Made bus cross tie system, Added ascent systems and deadface/staging logic.
  *	
  *	Revision 1.1  2009/02/18 23:21:14  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.12  2008/04/11 11:49:43  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.11  2007/11/30 16:40:40  movieman523
  *	Revised LEM to use generic voltmeter and ammeter code. Note that the ED battery select switch needs to be implemented to fully support the voltmeter/ammeter now.
  *	
  *	Revision 1.10  2007/06/06 15:02:14  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.9  2006/08/21 03:04:38  dseagrav
  *	This patch adds DC volt/amp meters and associated switches, which was an unholy pain in the
  *	
  *	Revision 1.8  2006/08/13 16:01:52  movieman523
  *	Renamed LEM. Think it all builds properly, I'm checking it in before the lightning knocks out the power here :).
  *	
  *	Revision 1.7  2006/07/24 06:41:29  dseagrav
  *	Many changes - Rearranged / corrected FDAI power usage, added LM AC equipment, many bugfixes
  *	
  *	Revision 1.6  2006/06/18 22:45:31  dseagrav
  *	LM ECA bug fix, LGC,IMU,DSKY and IMU OPR wired to CBs, IMU OPR,LGC,FDAI,and DSKY draw power
  *	
  *	Revision 1.5  2006/06/18 16:43:07  dseagrav
  *	LM EPS fixes, LMP/CDR DC busses now powered thru CBs, ECA power-off bug fixed and ECA speed improvement
  *	
  *	Revision 1.4  2006/05/01 08:52:49  dseagrav
  *	LM checkpoint commit. Extended capabilities of IndicatorSwitch class to save memory, more LM ECA stuff, I forget what else changed. More work is needed yet.
  *	
  *	Revision 1.3  2006/04/23 04:15:46  dseagrav
  *	LEM checkpoint commit. The LEM is not yet airworthy. Please be patient.
  *	
  *	Revision 1.2  2006/04/17 18:14:27  movieman523
  *	Added flashing borders to all switches (I think).
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "OrbiterSoundSDK35.h"
#include "soundlib.h"

#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "LEMcomputer.h"
#include "IMU.h"

#include "LEM.h"

void LEMThreePosSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	lem = s;
}

// This is like a normal switch, except it shows the mission timer values (since we can't see it, it's on the other panel)
void LEMMissionTimerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s, int id)
{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	lem = s;
	sw = id;
}

bool LEMMissionTimerSwitch::CheckMouseClick(int event, int mx, int my)
{
	bool rv;
	// Is the event timer powered?
	if(lem->MissionTimerDisplay.IsPowered()){
		// Yes, print the time
		sprintf(oapiDebugString(),"LM MT: %.2d:%.2d:%.2d",
			lem->MissionTimerDisplay.GetHours(),
			lem->MissionTimerDisplay.GetMinutes(),
			lem->MissionTimerDisplay.GetSeconds());		
	}
	if(event & PANEL_MOUSE_RBDOWN || event & PANEL_MOUSE_RBUP){
		return false; // Disregard this
	}
	// Animate switch
	rv = LEMThreePosSwitch::CheckMouseClick(event, mx, my);
	// Perform function
	switch(sw){
		case 0: // Run-Stop-Reset
			switch(GetState()){
				case THREEPOSSWITCH_UP: // RUN
					lem->MissionTimerDisplay.SetRunning(true); break;
				case THREEPOSSWITCH_CENTER: // STOP
					lem->MissionTimerDisplay.SetRunning(false); break;
				case THREEPOSSWITCH_DOWN: // RESET
					lem->MissionTimerDisplay.Reset(); break;
			}
			break;
		case 1: // Hours Inc
			switch(GetState()){
				case THREEPOSSWITCH_UP: // RUN
					lem->MissionTimerDisplay.UpdateHours(10); break;
				case THREEPOSSWITCH_DOWN: // RESET
					lem->MissionTimerDisplay.UpdateHours(1); break;
			}
			break;
		case 2: // Minutes Inc
			switch(GetState()){
				case THREEPOSSWITCH_UP: // RUN
					lem->MissionTimerDisplay.UpdateMinutes(10); break;
				case THREEPOSSWITCH_DOWN: // RESET
					lem->MissionTimerDisplay.UpdateMinutes(1); break;
			}
			break;
		case 3: // Seconds Inc
			switch(GetState()){
				case THREEPOSSWITCH_UP: // RUN
					lem->MissionTimerDisplay.UpdateSeconds(10); break;
				case THREEPOSSWITCH_DOWN: // RESET
					lem->MissionTimerDisplay.UpdateSeconds(1); break;
			}
			break;
	}
	return rv;
}

LEMValveTalkback::LEMValveTalkback()

{
	Valve = 0;
	our_vessel = 0;
}

void LEMValveTalkback::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, int vlv, LEM *s)

{
	IndicatorSwitch::Init(xp, yp, w, h, surf, row);

	Valve = vlv;
	our_vessel = s;
}

int LEMValveTalkback::GetState()

{
	//sprintf(oapiDebugString(),"SRCV %f STATE %d",SRC->Voltage(),our_vessel->GetValveState(Valve));
	if (our_vessel && (SRC->Voltage() > 20))
		return our_vessel->GetValveState(Valve) ? 1 : 0;

	return 0;
}

void LEMValveSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s, int valve, IndicatorSwitch *ind)

{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	Valve = valve;
	Indicator = ind;
}

bool LEMValveSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		CheckValve(GetState());
		return true;
	}

	return false;
}

bool LEMValveSwitch::SwitchTo(int newState)

{
	if (LEMThreePosSwitch::SwitchTo(newState)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		CheckValve(newState);
		return true;
	}

	return false;
}

void LEMValveSwitch::CheckValve(int s) 

{
	//sprintf(oapiDebugString(),"Switching %d",SRC->Voltage());	
	if (lem && (SRC->Voltage() > 20)) {
		if (s == THREEPOSSWITCH_UP) {
			lem->SetValveState(Valve, true);
			lem->CheckRCS();
			if (Indicator)
				Indicator->SetState(1);
		}
		else if (s == THREEPOSSWITCH_DOWN) {
			lem->SetValveState(Valve, false);
			lem->CheckRCS();
			if (Indicator)
				Indicator->SetState(0);
		}
	}
}

void LEMBatterySwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s,
							LEM_ECAch *lem_eca, int src_no, int asc)
{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	eca = lem_eca;
	lem = s;
	afl = asc;
	srcno = src_no;
}

bool LEMBatterySwitch::CheckMouseClick(int event, int mx, int my)

{
	// If our associated CB has no power, do nothing.
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		// Check for control power
		if (afl == 1){
			if(lem->CDRAscECAContCB.Voltage() < 24 && lem->LMPAscECAContCB.Voltage() < 24){ return true; }
		}else{
			if(lem->CDRDesECAContCB.Voltage() < 24 && lem->LMPDesECAContCB.Voltage() < 24){ return true; }
		}
		switch(state){
			case THREEPOSSWITCH_UP:
				switch(srcno){
					case 1: // HV
						eca->input = 1;
						if(eca->dc_source_tb != NULL){
							eca->dc_source_tb->SetState(1);
						}
						break;
					case 2: // LV
						eca->input = 2;
						if(eca->dc_source_tb != NULL){
							eca->dc_source_tb->SetState(2);
						}
						break;
				}
				break;
			case THREEPOSSWITCH_DOWN:
				if(eca->dc_source_tb != NULL){
					eca->dc_source_tb->SetState(0);
				}
				eca->input = 0;
				break;
		}

		return true;
	}
	return false;
}

bool LEMBatterySwitch::SwitchTo(int newState)

{
	sprintf(oapiDebugString(),"NewState %d",newState);
	if (LEMThreePosSwitch::SwitchTo(newState)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		// CheckValve(newState);
		return true;
	}

	return false;
}

// LEM Descent Dead Face Switch
void LEMDeadFaceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s)
{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);
}

bool LEMDeadFaceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		switch(state){
			case THREEPOSSWITCH_UP:
				// Connect descent stage
				if (lem->CDRAscECAContCB.Voltage() < 24 && lem->LMPAscECAContCB.Voltage() < 24){ return true; }
				if(lem->stage < 2){
					// Reconnect ECA outputs
					lem->DES_LMPs28VBusA.WireTo(&lem->ECA_1a);
					lem->DES_LMPs28VBusB.WireTo(&lem->ECA_1b);
					lem->DES_CDRs28VBusA.WireTo(&lem->ECA_2a); 
					lem->DES_CDRs28VBusB.WireTo(&lem->ECA_2b); 
					// Reconnect EPS monitor stuff
					lem->EPSMonitorSelectRotary.SetSource(1, lem->Battery1);
					lem->EPSMonitorSelectRotary.SetSource(2, lem->Battery2);
					lem->EPSMonitorSelectRotary.SetSource(3, lem->Battery3);
					lem->EPSMonitorSelectRotary.SetSource(4, lem->Battery4);
					lem->DSCBattFeedTB.SetState(1);
				}
				break;
			case THREEPOSSWITCH_DOWN:
				// Disconnect descent stage
				if (lem->CDRAscECAContCB.Voltage() < 24 && lem->LMPAscECAContCB.Voltage() < 24){ return true; }
				lem->DES_LMPs28VBusA.Disconnect();
				lem->DES_LMPs28VBusB.Disconnect();
				lem->DES_CDRs28VBusA.Disconnect();
				lem->DES_CDRs28VBusB.Disconnect();
				lem->EPSMonitorSelectRotary.SetSource(1, NULL);
				lem->EPSMonitorSelectRotary.SetSource(2, NULL);
				lem->EPSMonitorSelectRotary.SetSource(3, NULL);
				lem->EPSMonitorSelectRotary.SetSource(4, NULL);
				lem->DSCBattFeedTB.SetState(0);
				break;
		}		
		return true;
	}	
	return false;
}

bool LEMDeadFaceSwitch::SwitchTo(int newState)

{
	sprintf(oapiDebugString(),"NewState %d",newState);
	if (LEMThreePosSwitch::SwitchTo(newState)) {
		// Stuff goes here later
		return true;
	}

	return false;
}


// INVERTER SWITCH

void LEMInverterSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s,
							LEM_INV *lem_inv_1, LEM_INV *lem_inv_2)
{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	inv1 = lem_inv_1;
	inv2 = lem_inv_2;
}

bool LEMInverterSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		return ChangeState(state);		
	}	
	return false;
}

bool LEMInverterSwitch::ChangeState(int newState){
	switch(newState){
		case THREEPOSSWITCH_UP:      // INV 2			
			if(inv1 != NULL){ inv1->active = 0; }
			if(inv2 != NULL){ inv2->active = 1; }
			lem->ACBusA.WireTo(&lem->AC_A_INV_2_FEED_CB);
			lem->ACBusB.WireTo(&lem->AC_B_INV_2_FEED_CB);
			break;
		case THREEPOSSWITCH_CENTER:  // INV 1
			if(inv1 != NULL){ inv1->active = 1; }
			if(inv2 != NULL){ inv2->active = 0; }			
			lem->ACBusA.WireTo(&lem->AC_A_INV_1_FEED_CB);
			lem->ACBusB.WireTo(&lem->AC_B_INV_1_FEED_CB);
			break;
		case THREEPOSSWITCH_DOWN:    // OFF				
			if(inv1 != NULL){ inv1->active = 0; }
			if(inv2 != NULL){ inv2->active = 0; }
			lem->ACBusA.Disconnect();
			lem->ACBusB.Disconnect();
			break;
	}
	return true;	
}

bool LEMInverterSwitch::SwitchTo(int newState)
{
	sprintf(oapiDebugString(),"NewState %d",newState);
	if (LEMThreePosSwitch::SwitchTo(newState)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		// CheckValve(newState);
		return true;
	}
	return false;
}

// Meters
void LEMRoundMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, LEM *s)

{
	RoundMeter::Init(p0, p1, row);
	lem = s;
}

// DC Voltmeter
double LEMDCVoltMeter::QueryValue()

{
	switch(lem->EPSMonitorSelectRotary.GetState()){
		case 0: // ED/OFF
			switch(lem->EPSEDVoltSelect.GetState()){
				case THREEPOSSWITCH_UP:		// ED Battery A
				case THREEPOSSWITCH_DOWN:	// ED Battery B
					return 37.1; // Fake unloaded battery
					break;
				case THREEPOSSWITCH_CENTER: // OFF
					return 0;
					break;
				default:
					return 0;
					break;
			}
			break;
		case 1: // Battery 1
			if(lem->Battery1){ return(lem->Battery1->Voltage()); }else{ return 0; }
			break;
		case 2: // Battery 2
			if(lem->Battery2){ return(lem->Battery2->Voltage()); }else{ return 0; }
			break;
		case 3: // Battery 3
			if(lem->Battery3){ return(lem->Battery3->Voltage()); }else{ return 0; }
			break;
		case 4: // Battery 4
			if(lem->Battery4){ return(lem->Battery4->Voltage()); }else{ return 0; }
			break;
		case 5: // Battery 5
			if(lem->Battery5){ return(lem->Battery5->Voltage()); }else{ return 0; }
			break;
		case 6: // Battery 6
			if(lem->Battery6){ return(lem->Battery6->Voltage()); }else{ return 0; }
			break;
		case 7: // CDR DC BUS
			return(lem->CDRs28VBus.Voltage());
			break;
		case 8: // LMP DC BUS
			return(lem->LMPs28VBus.Voltage());
			break;
		case 9: // AC BUS (?)
			if(lem->ACBusA.Voltage() > 85){
				return(lem->ACBusA.Voltage()-85);
			}else{
				return(0);
			}
			break;		
		default:
			return(0);
	}
}

void LEMDCVoltMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface){
	// 40V = -35 deg and 20V = 215 deg
	// 250 degrees of sweep across 20 volts, for 12.5 degrees per volt

	// 20V = 180+35	
	v = 240-((v-18)*12.5);
	DrawNeedle(drawSurface, 49, 49, 25.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 0, 0, 0, 99, 98, SURF_PREDEF_CK);
}

// DC Ammeter
double LEMDCAmMeter::QueryValue(){	
	switch(lem->EPSMonitorSelectRotary.GetState()){
		case 0: // ED/OFF
			return 0; // Means either off or unloaded ED battery
			break;
		case 1: // Battery 1
			if(lem->Battery1 && lem->Battery1->Voltage() > 0){ 
				return(lem->Battery1->PowerLoad()/lem->Battery1->Voltage()); }else{ return 0; }
			break;
		case 2: // Battery 2
			if(lem->Battery2 && lem->Battery2->Voltage() > 0){
				return(lem->Battery2->PowerLoad()/lem->Battery2->Voltage()); }else{ return 0; }
			break;
		case 3: // Battery 3
			if(lem->Battery3 && lem->Battery3->Voltage() > 0){
				return(lem->Battery3->PowerLoad()/lem->Battery3->Voltage()); }else{ return 0; }
			break;
		case 4: // Battery 4
			if(lem->Battery4 && lem->Battery4->Voltage() > 0){
				return(lem->Battery4->PowerLoad()/lem->Battery4->Voltage()); }else{ return 0; }
			break;
		case 5: // Battery 5
			if(lem->Battery5 && lem->Battery5->Voltage() > 0){
				return(lem->Battery5->PowerLoad()/lem->Battery5->Voltage()); }else{ return 0; }
			break;
		case 6: // Battery 6
			if(lem->Battery6 && lem->Battery6->Voltage() > 0){
				return(lem->Battery6->PowerLoad()/lem->Battery6->Voltage()); }else{ return 0; }
			break;
		case 7: // CDR DC BUS
			if(lem->CDRs28VBus.Voltage() > 0){
				return(lem->CDRs28VBus.PowerLoad()/lem->CDRs28VBus.Voltage()); }else{ return 0; }
			break;
		case 8: // LMP DC BUS
			if(lem->LMPs28VBus.Voltage() > 0){
				return(lem->LMPs28VBus.PowerLoad()/lem->LMPs28VBus.Voltage()); }else{ return 0; }
			break;
		case 9: // AC BUS (?)
			if(lem->ACBusA.Voltage() > 0){
				return(lem->ACBusA.PowerLoad()/lem->ACBusA.Voltage());
			}else{
				return(0);
			}
			break;		
		default:
			return(0);
	}	
}

void LEMDCAmMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface){
	// 100A = 90 deg and 20A = 270 deg
	// 180 degress of sweep across 80 amps, for 2.25 degrees per amp
	
	v = 220-(v*2.25);
	DrawNeedle(drawSurface, 49, 49, 25.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 0, 0, 0, 99, 98, SURF_PREDEF_CK);
}

// LEM Voltmeter-feeding CB hack
double LEMVoltCB::Current()
{	
	if ((state != 0) && SRC && SRC->IsEnabled()) {
		Volts = SRC->Voltage();
		if (Volts > 0.0)
			Amperes = SRC->Current();
		else 
			Amperes = 0.0; 
	}
	return Amperes;
}
