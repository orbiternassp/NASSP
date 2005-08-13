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

class CautionWarningSystem {

public:
	CautionWarningSystem(Sound &mastersound);
	virtual ~CautionWarningSystem();

	virtual void LightTest(int state);
	virtual void TimeStep(double simt);
	void SetMode(int mode);
	void SetSource(int source) { Source = source; };
	void SetPowerBus(int bus) { PowerBus = bus; };
	void SetMasterAlarm(bool alarm);

	void RenderMasterAlarm(SURFHANDLE surf, SURFHANDLE alarmLit);

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

	//
	// These don't have to be saved.
	//

	bool MasterAlarmLit;
	double MasterAlarmCycleTime;
	Sound &MasterAlarmSound;
};

//
// Strings for state saving.
//

#define CWS_START_STRING	"CWS_BEGIN"
#define CWS_END_STRING		"CWS_END"

#endif
