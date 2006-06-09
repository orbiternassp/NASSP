/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LES class

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
// Data structure passed from main vessel to SII to configure stage.
//

typedef struct {

	int SettingsType;
	int VehicleNo;
	int Realism;

	double ISP_LET_VAC;
	double ISP_LET_SL;

	double THRUST_VAC_LET;

	double MissionTime;
	double EmptyMass;

	double MainFuelKg;
	double JettisonFuelKg;

	bool FireMain;
	bool LowRes;

} LESSettings;

//
// Which parts of the structure are active.
//

#define LES_SETTINGS_MASS		0x01
#define LES_SETTINGS_FUEL		0x02
#define LES_SETTINGS_GENERAL	0x04
#define LES_SETTINGS_ENGINES	0x08
#define LES_SETTINGS_THRUST		0x10
#define LES_SETTINGS_MAIN_FUEL	0x20

//
// Stage states.
//

#define LES_STATE_SETUP				0
#define LES_STATE_JETTISON			1
#define LES_STATE_WAITING			2

//
// Stage class.
//

class LES : public VESSEL2 {

public:
	LES (OBJHANDLE hObj, int fmodel);
	virtual ~LES();

	void clbkSaveState (FILEHANDLE scn);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkLoadStateEx (FILEHANDLE scn, void *status);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkDockEvent(int dock, OBJHANDLE connected);

	//
	// Must be virtual so it can be called from other DLLs.
	//
	virtual void SetState(LESSettings &state);

protected:

	void SetLES();
	void InitLES();
	void AddEngines();

	int GetMainState();
	void SetMainState(int s);

	int MissionNo;
	int VehicleNo;
	int State;
	int Realism;

	bool LowRes;
	bool FireMain;

	double EmptyMass;
	double PayloadMass;

	double MainFuel;
	double JettisonFuel;

	double MissionTime;
	double NextMissionEventTime;
	double LastMissionEventTime;

	double THRUST_VAC_MAIN;
	double THRUST_VAC_JETTISON;
	double ISP_VAC;
	double ISP_SL;

	THRUSTER_HANDLE th_jettison[2], th_main[4];
	THGROUP_HANDLE thg_main;
	PROPELLANT_HANDLE ph_jettison, ph_main;
};
