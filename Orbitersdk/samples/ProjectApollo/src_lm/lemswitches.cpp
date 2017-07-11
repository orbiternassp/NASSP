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
#include "lm_channels.h"

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

/*bool LEMMissionTimerSwitch::CheckMouseClick(int event, int mx, int my)
{
	bool rv;
	// Is the event timer powered?
	if(lem->MissionTimerDisplay.IsPowered()){
		// Yes, print the time
		sprintf(oapiDebugString(),"LM MT: %.2d:%.2d:%.2d",
			lem->MissionTimerDisplay.GetHours(),
			lem->MissionTimerDisplay.GetMinutes(),
			lem->MissionTimerDisplay.GetSeconds());	
		lem->DebugLineClearTimer = 5;
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
}*/

bool LEMMissionTimerSwitch::SwitchTo(int newState, bool dontspring)
{
	bool rv;
	// Is the event timer powered?
	if (lem->MissionTimerDisplay.IsPowered()) {
		// Yes, print the time
		sprintf(oapiDebugString(), "LM MT: %.2d:%.2d:%.2d",
			lem->MissionTimerDisplay.GetHours(),
			lem->MissionTimerDisplay.GetMinutes(),
			lem->MissionTimerDisplay.GetSeconds());
		lem->DebugLineClearTimer = 5;
	}
	//if (event & PANEL_MOUSE_RBDOWN || event & PANEL_MOUSE_RBUP) {
	//	return false; // Disregard this
	//}
	// Animate switch
	rv = LEMThreePosSwitch::SwitchTo(newState, dontspring);
	// Perform function
	switch (sw) {
	case 0: // Run-Stop-Reset
		switch (newState) {
		case THREEPOSSWITCH_UP: // RUN
			lem->MissionTimerDisplay.SetRunning(true); break;
		case THREEPOSSWITCH_CENTER: // STOP
			lem->MissionTimerDisplay.SetRunning(false); break;
		case THREEPOSSWITCH_DOWN: // RESET
			lem->MissionTimerDisplay.Reset(); break;
		}
		break;
	case 1: // Hours Inc
		switch (newState) {
		case THREEPOSSWITCH_UP: // RUN
			lem->MissionTimerDisplay.UpdateHours(10); break;
		case THREEPOSSWITCH_DOWN: // RESET
			lem->MissionTimerDisplay.UpdateHours(1); break;
		}
		break;
	case 2: // Minutes Inc
		switch (newState) {
		case THREEPOSSWITCH_UP: // RUN
			lem->MissionTimerDisplay.UpdateMinutes(10); break;
		case THREEPOSSWITCH_DOWN: // RESET
			lem->MissionTimerDisplay.UpdateMinutes(1); break;
		}
		break;
	case 3: // Seconds Inc
		switch (newState) {
		case THREEPOSSWITCH_UP: // RUN
			lem->MissionTimerDisplay.UpdateSeconds(10); break;
		case THREEPOSSWITCH_DOWN: // RESET
			lem->MissionTimerDisplay.UpdateSeconds(1); break;
		}
		break;
	}
	return rv;
}

// ECS indicator, suit temp
LMSuitTempMeter::LMSuitTempMeter()

{
	NeedleSurface = 0;
}

void LMSuitTempMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMSuitTempMeter::QueryValue()

{
	if(!lem){ return 0; }
	return lem->ecs.Suit_Temp;
}

void LMSuitTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  3, 115-((int)((v-40)*1.7)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, cabin temp
LMCabinTempMeter::LMCabinTempMeter()

{
	NeedleSurface = 0;
}

void LMCabinTempMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMCabinTempMeter::QueryValue()

{
	if(!lem){ return 0; }
	return lem->ecs.Cabin_Temp;
}

void LMCabinTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  58, 115-((int)((v-40)*1.7)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, suit pressure
LMSuitPressMeter::LMSuitPressMeter()

{
	NeedleSurface = 0;
}

void LMSuitPressMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMSuitPressMeter::QueryValue()

{
	if(!lem){ return 0; }
	return lem->ecs.Suit_Press;
}

void LMSuitPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  94, 115-((int)(v*10.2)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, cabin pressure
LMCabinPressMeter::LMCabinPressMeter()

{
	NeedleSurface = 0;
}

void LMCabinPressMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMCabinPressMeter::QueryValue()

{
	if(!lem){ return 0; }
	return lem->ecs.Cabin_Press;
}

void LMCabinPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  149, 115-((int)(v*10.2)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, cabin CO2 level
LMCabinCO2Meter::LMCabinCO2Meter()

{
	NeedleSurface = 0;
}

void LMCabinCO2Meter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMCabinCO2Meter::QueryValue()

{
	if(!lem){ return 0; }
	// FIXME: NEED TO HANDLE SUIT GAS DIVERTER HERE
	return lem->ecs.Cabin_CO2;
}

void LMCabinCO2Meter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	double cf,sf; // Correction Factor, Scale factor
	int btm;      // Bottom of this segment
	// Determine needle range and scale factor
	if(v <= 5){
		btm = 114;
		sf = 8.0;
		cf = 0;
	}else{
		if(v <= 10){
			btm = 74;
			cf = 5;
			sf = 4.0;
		}else{
			if(v <= 15){
				btm = 54;
				cf = 10;
				sf = 3.0;
			}else{
				if(v <= 20){
					btm = 39;
					cf = 15;
					sf = 2;
				}else{
					btm = 29;
					cf = 20;
					sf = 1.5;
				}
			}
		}
	}
	oapiBlt(drawSurface, NeedleSurface,  267, btm-((int)((v-cf)*sf)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, Glycol Temp Meter
LMGlycolTempMeter::LMGlycolTempMeter()

{
	NeedleSurface = 0;
}

void LMGlycolTempMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMGlycolTempMeter::QueryValue()

{
	if(!lem){ return 0; }
	if(lem->GlycolRotary.GetState() == 0){
		// Secondary
		return(lem->ecs.Secondary_CL_Glycol_Temp[0]);
	}else{
		// Primary
		return(lem->ecs.Primary_CL_Glycol_Temp[0]);
	}
}

void LMGlycolTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  3, 111-((int)(v*1.2)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, Glycol Pressure Meter
LMGlycolPressMeter::LMGlycolPressMeter()

{
	NeedleSurface = 0;
}

void LMGlycolPressMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMGlycolPressMeter::QueryValue()

{
	if(!lem){ return 0; }
	if(lem->GlycolRotary.GetState() == 0){
		// Secondary
		return(lem->ecs.Secondary_CL_Glycol_Press[1]);
	}else{
		// Primary
		return(lem->ecs.Primary_CL_Glycol_Press[1]);
	}
}

void LMGlycolPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  58, 111-((int)(v*1.2)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, Oxygen Quantity Meter
LMOxygenQtyMeter::LMOxygenQtyMeter()

{
	NeedleSurface = 0;
}

void LMOxygenQtyMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMOxygenQtyMeter::QueryValue()

{
	if(!lem){ return 0; }
	switch(lem->QtyMonRotary){
		case 0: // RESET
		default:
			return 0;
		case 1: // DES
			return(((lem->ecs.Des_Oxygen[0] + lem->ecs.Des_Oxygen[1])/(48.01*2))*100);
		case 2: // ASC 1
			return((lem->ecs.Asc_Oxygen[0]/2.43)*100);
		case 3: // ASC 2
			return((lem->ecs.Asc_Oxygen[1]/2.43)*100);
	}
}

void LMOxygenQtyMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  94, 113-((int)(v*1.01)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// ECS indicator, Water Quantity Meter
LMWaterQtyMeter::LMWaterQtyMeter()

{
	NeedleSurface = 0;
}

void LMWaterQtyMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMWaterQtyMeter::QueryValue()

{
	if(!lem){ return 0; }
	switch(lem->QtyMonRotary){
		case 0: // RESET
		default:
			return 0;
		case 1: // DES
			return(((lem->ecs.Des_Water[0] + lem->ecs.Des_Water[1])/(333*2))*100);
		case 2: // ASC 1
			return((lem->ecs.Asc_Water[0]/42.5)*100);
		case 3: // ASC 2
			return((lem->ecs.Asc_Water[1]/42.5)*100);
	}
}

void LMWaterQtyMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  149, 113-((int)(v*1.01)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// RCS indicator, RCS A Temp
LMRCSATempInd::LMRCSATempInd()

{
	NeedleSurface = 0;
}

void LMRCSATempInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMRCSATempInd::QueryValue()

{
	return 70.0;
}

void LMRCSATempInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  3, 114-((int)((v-20)*1.01)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// RCS indicator, RCS B Temp
LMRCSBTempInd::LMRCSBTempInd()

{
	NeedleSurface = 0;
}

void LMRCSBTempInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMRCSBTempInd::QueryValue()

{
	return 70.0;
}

void LMRCSBTempInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  58, 114-((int)((v-20)*1.01)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// RCS indicator, RCS A Press
LMRCSAPressInd::LMRCSAPressInd()

{
	NeedleSurface = 0;
}

void LMRCSAPressInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMRCSAPressInd::QueryValue()

{
	return 200.0;
}

void LMRCSAPressInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  94, 101-((int)(v*0.22)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// RCS indicator, RCS B Press
LMRCSBPressInd::LMRCSBPressInd()

{
	NeedleSurface = 0;
}

void LMRCSBPressInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMRCSBPressInd::QueryValue()

{
	return 200.0;
}

void LMRCSBPressInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  149, 101-((int)(v*0.22)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// RCS indicator, RCS A Qty
LMRCSAQtyInd::LMRCSAQtyInd()

{
	NeedleSurface = 0;
}

void LMRCSAQtyInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMRCSAQtyInd::QueryValue()

{
	return 50.0;
}

void LMRCSAQtyInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  185, 97-((int)(v*0.8)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// RCS indicator, RCS B Qty
LMRCSBQtyInd::LMRCSBQtyInd()

{
	NeedleSurface = 0;
}

void LMRCSBQtyInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double LMRCSBQtyInd::QueryValue()

{
	return 50.0;
}

void LMRCSBQtyInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface,  240, 97-((int)(v*0.8)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// Temperature Monitor Indicator
TempMonitorInd::TempMonitorInd()

{
	NeedleSurface = 0;
}

void TempMonitorInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double TempMonitorInd::QueryValue()

{
	return 50.0;
}

void TempMonitorInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{	
	oapiBlt(drawSurface, NeedleSurface,  35, 112-((int)((v+100)*0.34)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// Engine Thrust Indicator
EngineThrustInd::EngineThrustInd()

{
	NeedleSurface = 0;
}

void EngineThrustInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double EngineThrustInd::QueryValue()

{
	if (lem->stage < 2)
	{
		return lem->GetThrusterLevel(lem->th_hover[0])*100.0;
	}
	else
	{
		return 0;
	}
}

void EngineThrustInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{	
	oapiBlt(drawSurface, NeedleSurface,  3, 114-((int)v), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// Commanded Thrust Indicator
CommandedThrustInd::CommandedThrustInd()

{
	NeedleSurface = 0;
}

void CommandedThrustInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double CommandedThrustInd::QueryValue()

{
	return lem->deca.GetCommandedThrust()*100.0;
}

void CommandedThrustInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{	
	oapiBlt(drawSurface, NeedleSurface,  58, 114-((int)v), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// Thrust/Weight Indicator
ThrustWeightInd::ThrustWeightInd()

{
	NeedleSurface = 0;
}

void ThrustWeightInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double ThrustWeightInd::QueryValue()

{
	return lem->mechanicalAccelerometer.GetYAccel() / 1.594104;
}

void ThrustWeightInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{
	oapiBlt(drawSurface, NeedleSurface, 20, (int)(161.5 - 25.0*v), 0, 0, 8, 7, SURF_PREDEF_CK);
}

double ThrustWeightInd::AdjustForPower(double val)
{
	return val;
}

// Main Fuel Temperature Indicator
MainFuelTempInd::MainFuelTempInd()

{
	NeedleSurface = 0;
}

void MainFuelTempInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double MainFuelTempInd::QueryValue()

{
	return 70.0;
}

void MainFuelTempInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{	
	oapiBlt(drawSurface, NeedleSurface,  94, 115-((int)((v-40)*1.7)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// Main Fuel Pressure Indicator
MainFuelPressInd::MainFuelPressInd()

{
	NeedleSurface = 0;
}

void MainFuelPressInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s, ThreePosSwitch *temppressmonswitch)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
	monswitch = temppressmonswitch;
}

double MainFuelPressInd::QueryValue()

{
	if (monswitch->IsUp())
	{
		return 150.0;
	}
	else if (monswitch->IsCenter() || monswitch->IsDown())
	{
		return lem->GetDPSPropellant()->GetFuelTankUllagePressurePSI();
	}

	return 0.0;
}

void MainFuelPressInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{	
	oapiBlt(drawSurface, NeedleSurface,  185, 115-((int)(v*0.34)), 0, 0, 7, 7, SURF_PREDEF_CK);
}

// Main Oxidizer Temperature Indicator
MainOxidizerTempInd::MainOxidizerTempInd()

{
	NeedleSurface = 0;
}

void MainOxidizerTempInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
}

double MainOxidizerTempInd::QueryValue()

{
	return 70.0;
}

void MainOxidizerTempInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{	
	oapiBlt(drawSurface, NeedleSurface,  149, 115-((int)((v-40)*1.7)), 7, 0, 7, 7, SURF_PREDEF_CK);
}

// Main Oxidizer Pressure Indicator
MainOxidizerPressInd::MainOxidizerPressInd()

{
	NeedleSurface = 0;
}

void MainOxidizerPressInd::Init(SURFHANDLE surf, SwitchRow &row, LEM *s, ThreePosSwitch *temppressmonswitch)

{
	MeterSwitch::Init(row);
	lem = s;
	NeedleSurface = surf;
	monswitch = temppressmonswitch;
}

double MainOxidizerPressInd::QueryValue()

{
	if (monswitch->IsUp())
	{
		return 150.0;
	}
	else if (monswitch->IsCenter() || monswitch->IsDown())
	{
		return lem->GetDPSPropellant()->GetOxidizerTankUllagePressurePSI();
	}

	return 0.0;
}

void MainOxidizerPressInd::DoDrawSwitch(double v, SURFHANDLE drawSurface)

{	
	oapiBlt(drawSurface, NeedleSurface,  240, 115-((int)(v*0.34)), 7, 0, 7, 7, SURF_PREDEF_CK);
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

/*bool LEMValveSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		CheckValve(GetState());
		return true;
	}

	return false;
}*/

bool LEMValveSwitch::SwitchTo(int newState, bool dontspring)

{
	if (LEMThreePosSwitch::SwitchTo(newState, dontspring)) {
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

/*bool LEMBatterySwitch::CheckMouseClick(int event, int mx, int my)

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
}*/

bool LEMBatterySwitch::SwitchTo(int newState, bool dontspring)

{
	//sprintf(oapiDebugString(),"NewState %d",newState);
	if (LEMThreePosSwitch::SwitchTo(newState, dontspring)) {
		// Check for control power
		if (afl == 1) {
			if (lem->CDRAscECAContCB.Voltage() < 24 && lem->LMPAscECAContCB.Voltage() < 24) { return true; }
		}
		else {
			if (lem->CDRDesECAContCB.Voltage() < 24 && lem->LMPDesECAContCB.Voltage() < 24) { return true; }
		}
		switch (state) {
		case THREEPOSSWITCH_UP:
			switch (srcno) {
			case 1: // HV
				eca->input = 1;
				if (eca->dc_source_tb != NULL) {
					eca->dc_source_tb->SetState(1);
				}
				break;
			case 2: // LV
				eca->input = 2;
				if (eca->dc_source_tb != NULL) {
					eca->dc_source_tb->SetState(2);
				}
				break;
			}
			break;
		case THREEPOSSWITCH_DOWN:
			if (eca->dc_source_tb != NULL) {
				eca->dc_source_tb->SetState(0);
			}
			eca->input = 0;
			break;
		}

		return true;
	}

	return false;
}

// LEM Descent Dead Face Switch
void LEMDeadFaceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s)
{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);
}

/*bool LEMDeadFaceSwitch::CheckMouseClick(int event, int mx, int my)

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
}*/

bool LEMDeadFaceSwitch::SwitchTo(int newState, bool dontspring)

{
	if (LEMThreePosSwitch::SwitchTo(newState, dontspring)) {
		switch (newState) {
		case THREEPOSSWITCH_UP:
			// Connect descent stage
			if (lem->CDRAscECAContCB.Voltage() < 24 && lem->LMPAscECAContCB.Voltage() < 24) { return true; }
			if (lem->stage < 2) {
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
			if (lem->CDRAscECAContCB.Voltage() < 24 && lem->LMPAscECAContCB.Voltage() < 24) { return true; }
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


// INVERTER SWITCH

void LEMInverterSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, LEM *s,
							LEM_INV *lem_inv_1, LEM_INV *lem_inv_2)
{
	LEMThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row, s);

	inv1 = lem_inv_1;
	inv2 = lem_inv_2;
}

/*bool LEMInverterSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (LEMThreePosSwitch::CheckMouseClick(event, mx, my)) {
		return ChangeState(state);		
	}	
	return false;
}*/

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

bool LEMInverterSwitch::SwitchTo(int newState, bool dontspring)
{
	if (LEMThreePosSwitch::SwitchTo(newState, dontspring)) {
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		return ChangeState(newState);
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

void EngineStartButton::Init(ToggleSwitch* stopbutton) {
	this->stopbutton = stopbutton;
}

bool EngineStartButton::Push()

{
	//Can only be switched when off and engine stop button is also off
	if (stopbutton->GetState() == 0 && state == 0)
	{
		if (ToggleSwitch::SwitchTo(1)) {

			sprintf(oapiDebugString(), "Engine Start: %d, Engine Stop: %d", GetState(), stopbutton->GetState());
			return true;
		}
	}

	return false;
}

void EngineStopButton::Init(ToggleSwitch* startbutton) {
	this->startbutton = startbutton;
}

bool EngineStopButton::Push()

{
	int newstate = !state;
	if (ToggleSwitch::SwitchTo(newstate)) {
		
		if (newstate = 1)
		{
			if (startbutton)
			{
				startbutton->SwitchTo(0);
				sprintf(oapiDebugString(), "Engine Start: %d, Engine Stop: %d", startbutton->GetState(), GetState());
			}
		}
		return true;
	}

	return false;
}

bool LMAbortButton::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	if (!visible) return false;
	if (mx < x || my < y) return false;
	if (mx >(x + width) || my >(y + height)) return false;

	if (event == PANEL_MOUSE_LBDOWN)
	{
		if (state == 0) {
			SwitchTo(1, true);
			Sclick.play();
			lem->agc.SetInputChannelBit(030, AbortWithDescentStage, false);
		}
		else if (state == 1) {
			SwitchTo(0, true);
			Sclick.play();
			lem->agc.SetInputChannelBit(030, AbortWithDescentStage, true);
		}
	}
	return true;
}

void LMAbortButton::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, LEM *l)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset);
	lem = l;
}

LMAbortStageButton::LMAbortStageButton() 
{ 
	lem = 0; 
};

void LMAbortStageButton::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, LEM *l)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset);
	lem = l;
}

void LMAbortStageButton::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if (guardState) {
		DoDrawSwitch(DrawSurface);
	}
	else {
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
	}
}

bool LMAbortStageButton::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth &&
			my >= guardY && my <= guardY + guardHeight) {
			if (guardState) {
				Guard();
			}
			else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	}
	else if (event & (PANEL_MOUSE_LBDOWN)) {
		if (guardState) {


			if (!visible) return false;
			if (mx < x || my < y) return false;
			if (mx >(x + width) || my >(y + height)) return false;

			if (state == 0) {
				SwitchTo(1);
				Sclick.play();
			}
			else if (state == 1) {
				SwitchTo(0);
				Sclick.play();
			}
			return true;
		}
	}
	return false;
}

void LEMPanelOrdeal::Init(SwitchRow &row, LEM *l) {
	MeterSwitch::Init(row);
	lem = l;
}

int LEMPanelOrdeal::GetState() {
	return lem->ordealEnabled;
}

void LEMPanelOrdeal::SetState(int value) {

	if (value == 0) value = -1;

	lem->ordealEnabled = value;
}

RadarSignalStrengthAttenuator::RadarSignalStrengthAttenuator(char *i_name, double minIn, double maxIn, double minOut, double maxOut) :
	VoltageAttenuator(i_name, minIn, maxIn, minOut, maxOut)
{
}

void RadarSignalStrengthAttenuator::Init(LEM *l, RotationalSwitch *testmonitorselectorswitch, e_object *Instrum)
{
	lem = l;
	TestMonitorRotarySwitch = testmonitorselectorswitch;

	WireTo(Instrum);
}

double RadarSignalStrengthAttenuator::GetValue()
{
	double val = 0.0;

	switch (TestMonitorRotarySwitch->GetState())
	{
	case 0:	//ALT XMTR
		val = 0.0;
		break;
	case 1:	//VEL XMTR
		val = 0.0;
		break;
	case 2:	//AGC
		val = lem->RR.GetSignalStrength();
		break;
	case 3:	//XMTR PWR
		val = 0.0;
		break;
	case 4:	//SHAFT ERR
		val = lem->RR.GetShaftErrorSignal();
		break;
	case 5:	//TRUN ERR
		val = lem->RR.GetTrunnionErrorSignal();
		break;
	}

	return val;
}

void LEMSteerableAntennaPitchMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface)
{
	LEMRoundMeter::Init(p0, p1, row, s);
	FrameSurface = frameSurface;
}

double LEMSteerableAntennaPitchMeter::QueryValue() {
	return lem->SBandSteerable.GetPitch();
}

void LEMSteerableAntennaPitchMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface) {
	v = (210.0 - v) * 0.75;
	DrawNeedle(drawSurface, 91 / 2, 90 / 2, 25.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 0, 0, 0, 91, 90, SURF_PREDEF_CK);
}

void LEMSteerableAntennaYawMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface)
{
	LEMRoundMeter::Init(p0, p1, row, s);
	FrameSurface = frameSurface;
}

double LEMSteerableAntennaYawMeter::QueryValue() {
	return lem->SBandSteerable.GetYaw();
}

void LEMSteerableAntennaYawMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface) {
	v = (120.0 - v) * 0.75;
	DrawNeedle(drawSurface, 91 / 2, 90 / 2, 25.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 0, 0, 0, 91, 90, SURF_PREDEF_CK);
}

void LEMSBandAntennaStrengthMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, LEM *s, SURFHANDLE frameSurface)
{
	LEMRoundMeter::Init(p0, p1, row, s);
	FrameSurface = frameSurface;
}

double LEMSBandAntennaStrengthMeter::QueryValue() {
	return lem->SBand.rcvr_agc_voltage;
}

void LEMSBandAntennaStrengthMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface) {
	v = 220.0 - 2.7*v;
	DrawNeedle(drawSurface, 91 / 2, 90 / 2, 25.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 0, 0, 0, 91, 90, SURF_PREDEF_CK);
}

LEMDPSValveTalkback::LEMDPSValveTalkback()
{
	valve = 0;
}


void LEMDPSValveTalkback::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, DPSValve *v, bool failopen)

{
	IndicatorSwitch::Init(xp, yp, w, h, surf, row, failopen);
	valve = v;
}

int LEMDPSValveTalkback::GetState()

{
	if (valve && SRC && (SRC->Voltage() > SP_MIN_DCVOLTAGE))
		state = valve->IsOpen() ? 1 : 0;
	else
		// Should this fail open?
		state = (failOpen ? 1 : 0);

	return state;
}

void LEMDPSDigitalMeter::Init(SURFHANDLE surf, SwitchRow &row, LEM *l)
{
	MeterSwitch::Init(row);
	Digits = surf;
	lem = l;
}

void LEMDPSDigitalMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (lem->stage > 1) return;
	if (Voltage() < SP_MIN_DCVOLTAGE || lem->PROP_PQGS_CB.Voltage() < SP_MIN_DCVOLTAGE || lem->QTYMonSwitch.IsDown()) return;

	double percent = v * 100.0;

	int Curdigit2 = (int)percent;
	int Curdigit = (int)percent / 10;

	oapiBlt(drawSurface, Digits, 0, 0, 19 * Curdigit, 0, 19, 21);
	oapiBlt(drawSurface, Digits, 20, 0, 19 * (Curdigit2 - (Curdigit * 10)), 0, 19, 21);
}

double LEMDPSOxidPercentMeter::QueryValue()
{
	return lem->GetDPSPropellant()->GetOxidPercent();
}


double LEMDPSFuelPercentMeter::QueryValue()
{
	return lem->GetDPSPropellant()->GetFuelPercent();
}

LEMDigitalHeliumPressureMeter::LEMDigitalHeliumPressureMeter()

{
	source = 0;
	Digits = 0;
}

void LEMDigitalHeliumPressureMeter::Init(SURFHANDLE surf, SwitchRow &row, RotationalSwitch *s, LEM *l)

{
	MeterSwitch::Init(row);
	source = s;
	Digits = surf;
	lem = l;
}

double LEMDigitalHeliumPressureMeter::QueryValue()

{
	if (!source) return 0;

	if (source->GetState() == 0)
	{
		return 0.0;
	}
	else if (source->GetState() == 1)
	{
		return lem->GetDPSPropellant()->GetAmbientHeliumPressPSI();
	}
	else if (source->GetState() == 2)
	{
		return lem->GetDPSPropellant()->GetSupercriticalHeliumPressPSI();
	}

	return 0;
}

void LEMDigitalHeliumPressureMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (Voltage() < SP_MIN_DCVOLTAGE || source->GetState() == 0) return;

	int Curdigit4 = (int)v;
	int Curdigit3 = (int)v / 10;
	int Curdigit2 = (int)v / 100;
	int Curdigit = (int)v / 1000;

	oapiBlt(drawSurface, Digits, 0, 0, 19 * Curdigit, 0, 19, 21);
	oapiBlt(drawSurface, Digits, 20, 0, 19 * (Curdigit2 - (Curdigit * 10)), 0, 19, 21);
	oapiBlt(drawSurface, Digits, 40, 0, 19 * (Curdigit3 - (Curdigit2 * 10)), 0, 19, 21);
	oapiBlt(drawSurface, Digits, 60, 0, 19 * (Curdigit4 - (Curdigit3 * 10)), 0, 19, 21);
}