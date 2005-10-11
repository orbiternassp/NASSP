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
  *	Revision 1.5  2005/08/21 16:23:32  movieman523
  *	Added more alarms.
  *	
  *	Revision 1.4  2005/08/21 13:13:43  movieman523
  *	Wired in a few caution and warning lights.
  *	
  *	Revision 1.3  2005/08/21 11:51:59  movieman523
  *	Initial version of CSM caution and warning lights: light test switch now works.
  *	
  *	Revision 1.2  2005/08/13 16:41:15  movieman523
  *	Fully wired up the CSM caution and warning switches.
  *	
  *	Revision 1.1  2005/08/13 14:22:37  movieman523
  *	Initial implementation of caution and warning system.
  *	
  **************************************************************************/

#if !defined(_PA_CAUTIONWARNING_H)
#define _PA_CAUTIONWARNING_H

#define CWS_TEST_LIGHTS_NONE	0
#define CWS_TEST_LIGHTS_LEFT	1
#define CWS_TEST_LIGHTS_RIGHT	2

#define CWS_MODE_NORMAL		0
#define CWS_MODE_BOOST		1
#define CWS_MODE_ACK		2

#define CWS_SOURCE_LEM		0
#define CWS_SOURCE_CSM		0
#define CWS_SOURCE_CM		1

#define CWS_POWER_BUS_A		0
#define CWS_POWER_BUS_B		1
#define CWS_POWER_NONE		2

#define CWS_MASTERALARMPOSITION_NONE	0
#define CWS_MASTERALARMPOSITION_LEFT    1
#define CWS_MASTERALARMPOSITION_RIGHT	2


class CautionWarningSystem {

public:
	CautionWarningSystem(Sound &mastersound, Sound &buttonsound);
	virtual ~CautionWarningSystem();

	virtual void LightTest(int state);
	virtual void TimeStep(double simt);
	void SetMode(int mode);
	void SetSource(int source) { Source = source; };
	void SetPowerBus(int bus);
	void SetMasterAlarm(bool alarm);
	void MonitorVessel(VESSEL *v) { OurVessel = v; };

	//
	// Turn a light on or off. Lights 0-29 are on the left panel, lights
	// 30-59 are on the right panel.
	//
	// If the light wasn't on and is now being turned on, the Master Alarm
	// will sound.
	//

	void SetLight(int lightnum, bool state);

	void RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit, int position);
	virtual void RenderLights(SURFHANDLE surf, SURFHANDLE lightsurf, bool leftpanel);
	bool CheckMasterAlarmMouseClick(int event);

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	bool IsPowered();

protected:

	int TestState;
	int Mode;
	int Source;
	int PowerBus;

	bool MasterAlarmLightEnabled;
	bool MasterAlarm;
	bool MasterAlarmPressed;

	//
	// Light states.
	//
	// You get 30 lights per panel, as that fits nicely into a 32-bit integer. In
	// reality, neither CSM or LEM needs that many.
	//

	bool LeftLights[30];
	bool RightLights[30];

	//
	// These don't have to be saved.
	//

	bool MasterAlarmLit;
	double MasterAlarmCycleTime;
	Sound &MasterAlarmSound;
	Sound &ButtonSound;
	VESSEL *OurVessel;

	//
	// Helper functions.
	//

	int GetLightStates(bool *LightState);
	void SetLightStates(bool *LightState, int state);
};

//
// Strings for state saving.
//

#define CWS_START_STRING	"CWS_BEGIN"
#define CWS_END_STRING		"CWS_END"

#endif
