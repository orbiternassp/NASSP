/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

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


#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK3.h"

#include "soundlib.h"
#include "nasspsound.h"

#include "ioChannels.h"

#include "powersource.h"
#include "cautionwarning.h"
#include "nasspdefs.h"

CautionWarningSystem::CautionWarningSystem(Sound &mastersound, Sound &buttonsound) : MasterAlarmSound(mastersound), ButtonSound(buttonsound)

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

	for (int i = 0; i < 30; i++) {
		LeftLights[i] = false;
		RightLights[i] = false;
	}

	BusA = BusB = 0;
}

CautionWarningSystem::~CautionWarningSystem()

{
	// Nothing for now.
}

void CautionWarningSystem::LightTest(int state)

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
	// Check that the current bus is providing enough voltage.
	//

	case CWS_POWER_BUS_A:
		if (BusA && (BusA->Voltage() > 25.0))
			return true;
		return false;

	case CWS_POWER_BUS_B:
		if (BusB && (BusB->Voltage() > 25.0))
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
	if (BusA && (BusA->Voltage() > 25.0))
		return true;

	if (BusB && (BusB->Voltage() > 25.0))
		return true;

	return false;
}


void CautionWarningSystem::SetPowerBus(int bus) 

{ 
	PowerBus = bus; 

	// Enable MasterAlarm when power is switched on (Apollo Operations Handbook 2.10.4.1) 
	if (PowerBus != CWS_POWER_NONE) {
		SetMasterAlarm(true);
	}
}


//
// The timestep code will validate all the spacecraft systems and ligt the lights as required. Obviously
// much of this code will be CSM-specific or LEM-specific, so we'll probably want to derive CSM and LEM
// caution and warning systems which do their processing and then call this code for generic processing.
//

void CautionWarningSystem::TimeStep(double simt)

{
	//
	// Cycle master alarm light if required, and play sound if appropriate.
	//

	if ((simt > MasterAlarmCycleTime) && MasterAlarm) {
		MasterAlarmLit = !MasterAlarmLit;
		MasterAlarmCycleTime = simt + 0.25;
		if (MasterAlarmLit && IsPowered()) {
			MasterAlarmSound.play(NOLOOP,255);
		}
	}
}

void CautionWarningSystem::SetMasterAlarm(bool alarm)

{
	if (MasterAlarm != alarm) {
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

void CautionWarningSystem::RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit, int position)

{
	// In Boost-Mode only the left master alarm button is not illuminated (Apollo Operations Handbook 2.10.3)
	// The left/right lamp test illuminates the left/right master alarm button on the main panel (Apollo Operations Handbook 2.10.3)

	if (LightsPowered() && (
	       (MasterAlarmLit && (MasterAlarmLightEnabled || position != CWS_MASTERALARMPOSITION_LEFT)) || 
	       (TestState == CWS_TEST_LIGHTS_LEFT && position == CWS_MASTERALARMPOSITION_LEFT) ||
	       (TestState == CWS_TEST_LIGHTS_RIGHT && position == CWS_MASTERALARMPOSITION_RIGHT)
	   )) {
		//
		// Draw the master alarm lit bitmap.
		//
		oapiBlt(surf, alarmLit, 0, 0, 0, 0, 45, 36);
	}
}

bool CautionWarningSystem::CheckMasterAlarmMouseClick(int event)

{
	if (event & PANEL_MOUSE_LBDOWN) {
		MasterAlarmSound.stop();
		SetMasterAlarm(false); 
		MasterAlarmPressed = true;
		ButtonSound.play(NOLOOP, 255);
	} else if (event & PANEL_MOUSE_LBUP) {
		MasterAlarmPressed = false;
	}
	return true;
}

//
// Set operation mode. In BOOST mode the left master alarm light on the main panel is disabled.
//

void CautionWarningSystem::SetMode(int mode)

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

	if (lightnum > 30) {
		LightStates = RightLights;
		lightnum -= 30;
	}

	//
	// Turn on Master Alarm if a new light is lit.
	//

	if (state && !LightStates[lightnum]) {
		SetMasterAlarm(true);
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

	for (int i = 0; i < 30; i++) {
		if (LightState[i])
			lights |= mask;

		mask <<= 1;
	}

	return lights;
}

void CautionWarningSystem::SetLightStates(bool *LightState, int state)

{
	int mask = 1;

	for (int i = 0; i < 30; i++) {
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
	if (lightnum < 0 || lightnum > 59)
		return;

	if (failed) {
		if (lightnum < 30) {
			LightsFailedLeft |= (1 << lightnum);
		}
		else {
			LightsFailedRight |= (1 << (lightnum - 30));
		}
	}
	else {
		if (lightnum < 30) {
			LightsFailedLeft &= ~(1 << lightnum);
		}
		else {
			LightsFailedRight &= ~(1 << (lightnum - 30));
		}
	}
}

bool CautionWarningSystem::IsFailed(int lightnum)

{
	if (lightnum < 0 || lightnum > 59)
		return false;

	if (lightnum < 30) {
		return (LightsFailedLeft & (1 << lightnum)) != 0;
	}
	else {
		return (LightsFailedRight & (1 << (lightnum - 30))) != 0;
	}
}

void CautionWarningSystem::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, CWS_START_STRING);
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

	oapiWriteScenario_int (scn, "STATE", state.word);

	oapiWriteScenario_int (scn, "LLIGHTS", GetLightStates(LeftLights));
	oapiWriteScenario_int (scn, "RLIGHTS", GetLightStates(RightLights));

	if (LightsFailedLeft)
		oapiWriteScenario_int (scn, "LFAIL", LightsFailedLeft);

	if (LightsFailedRight)
		oapiWriteScenario_int (scn, "RFAIL", LightsFailedRight);

	oapiWriteLine(scn, CWS_END_STRING);
}


void CautionWarningSystem::LoadState(FILEHANDLE scn)

{
	char *line;
	int lights;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, CWS_END_STRING, sizeof(CWS_END_STRING)))
			return;
		if (!strnicmp (line, "MODE", 4)) {
			sscanf (line+4, "%d", &Mode);
		}
		else if (!strnicmp (line, "TEST", 4)) {
			sscanf (line+4, "%d", &TestState);
		}
		else if (!strnicmp (line, "LLIGHTS", 7)) {
			sscanf (line+7, "%d", &lights);
			SetLightStates(LeftLights, lights);
		}
		else if (!strnicmp (line, "RLIGHTS", 7)) {
			sscanf (line+7, "%d", &lights);
			SetLightStates(RightLights, lights);
		}
		else if (!strnicmp (line, "LFAIL", 5)) {
			sscanf (line+5, "%d", &LightsFailedLeft);
		}
		else if (!strnicmp (line, "RFAIL", 5)) {
			sscanf (line+5, "%d", &LightsFailedRight);
		}
		else if (!strnicmp (line, "STATE", 5)) {
			CWSState state;
			sscanf (line+5, "%d", &state.word);

			MasterAlarm = (state.u.MasterAlarm != 0);
			MasterAlarmLightEnabled = (state.u.MasterAlarmLightEnabled != 0);
			Source = state.u.Source;
			PowerBus = state.u.PowerBus;
		}
	}
}

