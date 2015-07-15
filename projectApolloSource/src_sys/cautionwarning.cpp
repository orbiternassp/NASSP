/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007 Mark Grant

  ORBITER vessel module: Generic caution and warning system code.

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
  *	Revision 1.3  2010/02/02 14:48:19  tschachim
  *	New correct master alarm sound by Bunyap.
  *	
  *	Revision 1.2  2009/12/22 18:14:47  tschachim
  *	More bugfixes related to the prelaunch/launch checklists.
  *	
  *	Revision 1.1  2009/02/18 23:21:48  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.25  2008/05/24 17:30:41  tschachim
  *	Bugfixes, new flash toggle.
  *	
  *	Revision 1.24  2008/04/11 11:49:32  tschachim
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.23  2007/10/18 00:23:18  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.22  2007/06/06 15:02:11  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.21  2007/01/22 14:39:58  tschachim
  *	Bugfix UplinkTestState.
  *	
  *	Revision 1.20  2007/01/06 04:44:48  dseagrav
  *	Corrected CREW ALARM command behavior, PCM downtelemetry generator now draws power
  *	
  *	Revision 1.19  2007/01/02 01:38:24  dseagrav
  *	Digital uplink and associated stuff.
  *	
  *	Revision 1.18  2006/07/01 23:49:13  movieman523
  *	Updated more documentation.
  *	
  *	Revision 1.17  2006/06/11 14:45:36  movieman523
  *	Quick fix for Apollo 4. Will need more work in the future.
  *	
  *	Revision 1.16  2006/05/19 13:48:28  tschachim
  *	Fixed a lot of devices and power consumptions.
  *	DirectO2 valve added.
  *	
  *	Revision 1.15  2006/01/14 20:58:15  movieman523
  *	Revised PowerSource code to ensure that classes which must be called each timestep are registered with the Panel SDK code.
  *	
  *	Revision 1.14  2006/01/05 11:24:10  tschachim
  *	Introduced InhibitNextMasterAlarm.
  *	
  *	Revision 1.13  2005/12/19 16:49:52  tschachim
  *	InhibitNextMasterAlarm for realism 0.
  *	
  *	Revision 1.12  2005/11/18 02:40:55  movieman523
  *	Major revamp of PanelSDK electrical code, and various modifications to run off fuel cells.
  *	
  *	Revision 1.11  2005/11/16 23:50:31  movieman523
  *	More updates to CWS operation. Still not completely correct, but closer.
  *	
  *	Revision 1.10  2005/11/16 23:14:01  movieman523
  *	Initial support for wiring in the circuit breakers.
  *	
  *	Revision 1.9  2005/11/16 00:18:49  movieman523
  *	Added beginnings of really basic IU emulation. Added random failures of caution and warning lights on non-historical missions. Added initial support for Skylab CM and SM. Added LEM Name option in scenario file.
  *	
  *	Revision 1.8  2005/10/11 16:28:11  tschachim
  *	Improved realism of the switch functionality.
  *	
  *	Revision 1.7  2005/08/23 03:19:59  flydba
  *	modified master alarm bitmap and correction of some switch positions
  *	
  *	Revision 1.6  2005/08/21 16:23:31  movieman523
  *	Added more alarms.
  *	
  *	Revision 1.5  2005/08/21 13:13:43  movieman523
  *	Wired in a few caution and warning lights.
  *	
  *	Revision 1.4  2005/08/21 11:51:59  movieman523
  *	Initial version of CSM caution and warning lights: light test switch now works.
  *	
  *	Revision 1.3  2005/08/13 22:24:20  movieman523
  *	Added the master alarm rendeing to CSM.
  *	
  *	Revision 1.2  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
  *	Revision 1.1  2005/08/13 14:22:37  movieman523
  *	Initial implementation of caution and warning system.
  *	
  **************************************************************************/


// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "soundlib.h"
#include "nasspsound.h"

#include "ioChannels.h"

#include "powersource.h"
#include "cautionwarning.h"
#include "nasspdefs.h"

CautionWarningSystem::CautionWarningSystem(Sound &mastersound, Sound &buttonsound, PanelSDK &p) : 
	MasterAlarmSound(mastersound), ButtonSound(buttonsound), DCPower(0, p)

{
	TestState = CWS_TEST_LIGHTS_NONE;
	Mode = CWS_MODE_NORMAL;
	Source = CWS_SOURCE_CSM;
	PowerBus = CWS_POWER_NONE;	

	OurVessel = 0;

	LightsFailedLeft = 0;
	LightsFailedRight = 0;

	MasterAlarmLightEnabled = true;
	MasterAlarmCycleTime = MINUS_INFINITY;
	MasterAlarm = false;
	MasterAlarmLit = false;
	MasterAlarmPressed = false;
	InhibitNextMasterAlarm = false;
	PlaySounds = true;
	UplinkTestState = 0;

	for (int i = 0; i < 30; i++) {
		LeftLights[i] = false;
		RightLights[i] = false;
	}
}

CautionWarningSystem::~CautionWarningSystem()

{
	// Nothing for now.
}

void CautionWarningSystem::LightTest(LightTestState state)

{
	if (state != TestState) {
		TestState = state;

		switch (TestState) {
		case CWS_TEST_LIGHTS_NONE:
			// Turn all lights off. Next timestep will restore any that should be lit.
			break;

		case CWS_TEST_LIGHTS_LEFT:
			// Light all lights on the left panel.
			break;

		case CWS_TEST_LIGHTS_RIGHT:
			// Light all lights on the right panel.
			break;
		}
	}
}

//
// Check we have power.
//

bool CautionWarningSystem::IsPowered()

{
	switch (PowerBus) {

	//
	// Check that we have enough voltage. We should eventually simulate both of the internal
	// power supplies, so one can fail if desired.
	//

	case CWS_POWER_SUPPLY_1:
	case CWS_POWER_SUPPLY_2:
		if (DCPower.Voltage() > SP_MIN_DCVOLTAGE)
			return true;
		return false;

	default:
		return false;
	}
}

//
// Check whether the lights are powered. This merely requires enough voltage from either bus.
//

bool CautionWarningSystem::LightsPowered()

{
	if (DCPower.Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}


void CautionWarningSystem::SetPowerBus(CSWPowerSource bus) 

{ 
	PowerBus = bus; 

	// Enable MasterAlarm when power is switched on (Apollo Operations Handbook 2.10.4.1) 
	if (PowerBus != CWS_POWER_NONE) {
		SetMasterAlarm(true);
	}
}


//
// The timestep code will validate all the spacecraft systems and light the lights as required. Obviously
// much of this code will be CSM-specific or LEM-specific, so we'll probably want to derive CSM and LEM
// caution and warning systems which do their processing and then call this code for generic processing.
//

void CautionWarningSystem::TimeStep(double simt)

{
	//
	// Cycle master alarm light if required, and play sound if appropriate.
	//

	if (simt > MasterAlarmCycleTime){
		if (MasterAlarm) {
			MasterAlarmLit = !MasterAlarmLit;
		}
		if (MasterAlarm) { // || (UplinkTestState&010) != 0){ -- Incorrect?
			MasterAlarmCycleTime = simt + 0.25;
		}
		if (MasterAlarm && IsPowered() && PlaySounds) {
			if (!MasterAlarmSound.isPlaying()) {
				MasterAlarmSound.play(LOOP, 255);
			}
		} else {
			MasterAlarmSound.stop();
		}
	}
}

void CautionWarningSystem::SystemTimestep(double simdt) 

{
	double consumption = 0.0;

	if (IsPowered()) {
		consumption += 11.4;
		if (MasterAlarmLit)
			consumption += 2.0;			// This number is just made up for now.
	}
	if (MasterAlarmLit && LightsPowered())
		consumption += 1.0;				// This number is just made up for now.

	DCPower.DrawPower(consumption);
}

void CautionWarningSystem::SetMasterAlarm(bool alarm)

{
	if (MasterAlarm != alarm)
	{
		MasterAlarm = alarm;

		//
		// Always set light state to false. If the alarm is enabled, the next timestep will
		// take care of lighting it.
		//

		MasterAlarmLit = false;
		MasterAlarmCycleTime = MINUS_INFINITY;
	}
}

//
// Render the lit master alarm light if required.
//

void CautionWarningSystem::RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit, SURFHANDLE border, CWSMasterAlarmPosition position)

{
	// In Boost-Mode only the left master alarm button is not illuminated (Apollo Operations Handbook 2.10.3)
	// The left/right lamp test illuminates the left/right master alarm button on the main panel (Apollo Operations Handbook 2.10.3)

	if (LightsPowered() && (
	       (MasterAlarmLit && (MasterAlarmLightEnabled || position != CWS_MASTERALARMPOSITION_LEFT)) || 
	       (TestState == CWS_TEST_LIGHTS_LEFT && position == CWS_MASTERALARMPOSITION_LEFT && MasterAlarmLightEnabled) ||
	       (TestState == CWS_TEST_LIGHTS_RIGHT && position == CWS_MASTERALARMPOSITION_RIGHT) ||
		   ((UplinkTestState&(int)position) != 0)
	   )) {
		//
		// Draw the master alarm lit bitmap.
		//
		oapiBlt(surf, alarmLit, 0, 0, 0, 0, 45, 36);
	}
	if (border)
		oapiBlt(surf, border, 0, 0, 0, 0, 45, 36, SURF_PREDEF_CK);
}

bool CautionWarningSystem::CheckMasterAlarmMouseClick(int event)

{
	if (event & PANEL_MOUSE_LBDOWN)
	{
		PushMasterAlarm();
		MasterAlarmPressed = true;
	}
	else if (event & PANEL_MOUSE_LBUP)
	{
		MasterAlarmPressed = false;
	}
	return true;
}

void CautionWarningSystem::PushMasterAlarm()

{
	MasterAlarmSound.stop();
	SetMasterAlarm(false); 
	ButtonSound.play(NOLOOP, 255);
}


//
// Set operation mode. In BOOST mode the left master alarm light on the main panel is disabled.
//

void CautionWarningSystem::SetMode(CWSOperationMode mode)

{
	Mode = mode;

	switch (Mode) {
	case CWS_MODE_NORMAL:
		MasterAlarmLightEnabled = true;
		break;

	case CWS_MODE_BOOST:
		MasterAlarmLightEnabled = false;
		break;

	case CWS_MODE_ACK:
		MasterAlarmLightEnabled = true;
		break;
	}
}

void CautionWarningSystem::SetLight(int lightnum, bool state)

{
	bool *LightStates = LeftLights;

	if (lightnum >= CWS_LIGHTS_PER_PANEL) {
		LightStates = RightLights;
		lightnum -= CWS_LIGHTS_PER_PANEL;
	}

	//
	// Turn on Master Alarm if a new light is lit.
	//

	if (state && !LightStates[lightnum]) {
		if (InhibitNextMasterAlarm) {
			InhibitNextMasterAlarm = false;
		} else {
			SetMasterAlarm(true);
		}
	}
	LightStates[lightnum] = state;
}

void CautionWarningSystem::RenderLights(SURFHANDLE surf, SURFHANDLE lightsurf, bool leftpanel)

{
	//
	// Do nothing, by default.
	//
}

typedef union

{
	struct {
		unsigned MasterAlarmLightEnabled:1;
		unsigned MasterAlarm:1;
		unsigned Source:2;
		unsigned PowerBus:2;
		unsigned InhibitNextMasterAlarm:1;
	} u;
	unsigned long word;
} CWSState;

//
// Functions to pack and unpack light states.
//

int CautionWarningSystem::GetLightStates(bool *LightState)

{
	int	lights = 0;
	int mask = 1;

	for (int i = 0; i < CWS_LIGHTS_PER_PANEL; i++) {
		if (LightState[i])
			lights |= mask;

		mask <<= 1;
	}

	return lights;
}

void CautionWarningSystem::SetLightStates(bool *LightState, int state)

{
	int mask = 1;

	for (int i = 0; i < CWS_LIGHTS_PER_PANEL; i++) {
		if (state & mask) {
			LightState[i] = true;
		}
		else {
			LightState[i] = false;
		}

		mask <<= 1;
	}
}

//
// Fail or fix a light.
//

void CautionWarningSystem::FailLight(int lightnum, bool failed)

{
	if (lightnum < 0 || lightnum >= (CWS_LIGHTS_PER_PANEL * 2))
		return;

	if (failed) {
		if (lightnum < CWS_LIGHTS_PER_PANEL) {
			LightsFailedLeft |= (1 << lightnum);
		}
		else {
			LightsFailedRight |= (1 << (lightnum - CWS_LIGHTS_PER_PANEL));
		}
	}
	else {
		if (lightnum < CWS_LIGHTS_PER_PANEL) {
			LightsFailedLeft &= ~(1 << lightnum);
		}
		else {
			LightsFailedRight &= ~(1 << (lightnum - CWS_LIGHTS_PER_PANEL));
		}
	}
}

bool CautionWarningSystem::IsFailed(int lightnum)

{
	if (lightnum < 0 || lightnum >= (CWS_LIGHTS_PER_PANEL*2))
		return false;

	if (lightnum < CWS_LIGHTS_PER_PANEL) {
		return (LightsFailedLeft & (1 << lightnum)) != 0;
	}
	else {
		return (LightsFailedRight & (1 << (lightnum - CWS_LIGHTS_PER_PANEL))) != 0;
	}
}

void CautionWarningSystem::SaveState(FILEHANDLE scn)

{
	oapiWriteScenario_int (scn, "MODE", Mode);
	oapiWriteScenario_int (scn, "TEST", TestState);

	//
	// Copy internal state to the structure.
	//

	CWSState state;

	state.word = 0;
	state.u.MasterAlarmLightEnabled = MasterAlarmLightEnabled;
	state.u.MasterAlarm = MasterAlarm;
	state.u.Source = Source;
	state.u.PowerBus = PowerBus;
	state.u.InhibitNextMasterAlarm = InhibitNextMasterAlarm;

	oapiWriteScenario_int (scn, "STATE", state.word);

	oapiWriteScenario_int (scn, "LLIGHTS", GetLightStates(LeftLights));
	oapiWriteScenario_int (scn, "RLIGHTS", GetLightStates(RightLights));

	if (LightsFailedLeft)
		oapiWriteScenario_int (scn, "LFAIL", LightsFailedLeft);

	if (LightsFailedRight)
		oapiWriteScenario_int (scn, "RFAIL", LightsFailedRight);
}

void CautionWarningSystem::LoadState(char *line)

{
	int lights;

	if (!strnicmp (line, "MODE", 4))
	{
		sscanf (line+4, "%d", &lights);
		Mode = (CWSOperationMode) lights;
	}
	else if (!strnicmp (line, "TEST", 4))
	{
		sscanf (line+4, "%d", &lights);
		TestState = (LightTestState) lights;
	}
	else if (!strnicmp (line, "LLIGHTS", 7))
	{
		sscanf (line+7, "%d", &lights);
		SetLightStates(LeftLights, lights);
	}
	else if (!strnicmp (line, "RLIGHTS", 7))
	{
		sscanf (line+7, "%d", &lights);
		SetLightStates(RightLights, lights);
	}
	else if (!strnicmp (line, "LFAIL", 5))
	{
		sscanf (line+5, "%d", &LightsFailedLeft);
	}
	else if (!strnicmp (line, "RFAIL", 5))
	{
		sscanf (line+5, "%d", &LightsFailedRight);
	}
	else if (!strnicmp (line, "STATE", 5))
	{
		CWSState state;
		sscanf (line+5, "%d", &state.word);

		MasterAlarm = (state.u.MasterAlarm != 0);
		MasterAlarmLightEnabled = (state.u.MasterAlarmLightEnabled != 0);
		Source = (CSWSource) state.u.Source;
		PowerBus = (CSWPowerSource) state.u.PowerBus;
		InhibitNextMasterAlarm = (state.u.InhibitNextMasterAlarm != 0);
	}
}
