/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: S1B class

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
  **************************************************************************/

//
// Data structure passed from main vessel to SIVB to configure stage.
//

typedef struct {

	int SettingsType;
	int VehicleNo;
	int Realism;
	int RetroNum;

	double THRUST_FIRST_VAC;
	double ISP_FIRST_VAC;
	double ISP_FIRST_SL;
	double MissionTime;
	double EmptyMass;
	double MainFuelKg;
	double CurrentThrust;

} S1BSettings;

//
// Which parts of the structure are active.
//

#define S1B_SETTINGS_MASS		0x1
#define S1B_SETTINGS_FUEL		0x2
#define S1B_SETTINGS_GENERAL	0x4
#define S1B_SETTINGS_ENGINES	0x8

//
// Stage states.
//

#define S1B_STATE_SHUTTING_DOWN		0
#define S1B_STATE_WAITING			1

//
// Stage class.
//

class S1B : public VESSEL2 {

public:
	S1B (OBJHANDLE hObj, int fmodel);
	virtual ~S1B();

	void clbkSaveState (FILEHANDLE scn);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkLoadStateEx (FILEHANDLE scn, void *status);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkDockEvent(int dock, OBJHANDLE connected);

	//
	// Must be virtual so it can be called from other DLLs.
	//
	virtual void SetState(S1BSettings &state);

protected:

	void SetS1b();
	void InitS1b();
	void AddEngines();

	int GetMainState();
	void SetMainState(int s);

	int MissionNo;
	int VehicleNo;
	int State;
	int Realism;
	int RetroNum;

	bool RetrosFired;

	double EmptyMass;
	double PayloadMass;
	double MainFuel;

	double MissionTime;
	double NextMissionEventTime;
	double LastMissionEventTime;

	double THRUST_FIRST_VAC;
	double ISP_FIRST_VAC;
	double ISP_FIRST_SL;
	double CurrentThrust;

	THRUSTER_HANDLE th_retro[4], th_main[8];
	THGROUP_HANDLE thg_retro, thg_main;
	PROPELLANT_HANDLE ph_retro, ph_main;
};
