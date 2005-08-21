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

#include "cautionwarning.h"
#include "nasspdefs.h"

CautionWarningSystem::CautionWarningSystem(Sound &mastersound) : MasterAlarmSound(mastersound)

{
	TestState = CWS_TEST_LIGHTS_NONE;
	Mode = CWS_MODE_NORMAL;
	Source = CWS_SOURCE_CSM;
	PowerBus = CWS_POWER_BUS_A;

	OurVessel = 0;

	MasterAlarmLightEnabled = true;
	MasterAlarmCycleTime = MINUS_INFINITY;
	MasterAlarm = false;
	MasterAlarmLit = false;

	for (int i = 0; i < 30; i++) {
		LeftLights[i] = false;
		RightLights[i] = false;
	}
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
	// Later we can check for bus voltage, etc. Now just check the source.
	//

	case CWS_POWER_BUS_A:
		return true;

	case CWS_POWER_BUS_B:
		return true;

	default:
		return false;
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

void CautionWarningSystem::RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit)

{
	if (MasterAlarmLit && MasterAlarmLightEnabled && IsPowered()) {
		//
		// Draw the master alarm lit bitmap.
		//
		oapiBlt(surf, alarmLit, 0, 0, 0, 0, 49, 40);
	}
}

//
// Set operation mode. In BOOST mode the master alarm light is disabled.
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

