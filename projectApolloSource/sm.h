/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Service Module class

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
  *	
  **************************************************************************/

//
// Data structure passed from main vessel to SIVB to configure stage.
//

typedef struct {

	int SettingsType;
	int VehicleNo;
	int Realism;

	double MissionTime;
	double EmptyMass;
	double MainFuelKg;
	double RCSAFuelKg;
	double RCSBFuelKg;
	double RCSCFuelKg;
	double RCSDFuelKg;

	bool LowRes;
	bool showHGA;
	bool A13Exploded;

} SMSettings;

//
// Which parts of the structure are active.
//

#define SM_SETTINGS_MASS		0x1
#define SM_SETTINGS_FUEL		0x2
#define SM_SETTINGS_GENERAL	0x4
#define SM_SETTINGS_ENGINES	0x8

//
// Stage states.
//

#define SM_STATE_SHUTTING_DOWN		0
#define SM_STATE_WAITING			1

//
// Stage class.
//

class SM : public VESSEL2 {

public:
	SM (OBJHANDLE hObj, int fmodel);
	virtual ~SM();

	void clbkSaveState (FILEHANDLE scn);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkLoadStateEx (FILEHANDLE scn, void *status);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkDockEvent(int dock, OBJHANDLE connected);

	//
	// Must be virtual so it can be called from other DLLs.
	//
	virtual void SetState(SMSettings &state);

protected:

	void SetSM();
	void InitSM();
	void AddEngines();

	int GetMainState();
	void SetMainState(int s);

	int MissionNo;
	int VehicleNo;
	int State;
	int Realism;

	bool RetrosFired;
	bool LowRes;

	//
	// Which parts to display?
	//

	bool showSPS;
	bool showRCS;
	bool showPanel1;
	bool showPanel2;
	bool showPanel3;
	bool showPanel4;
	bool showPanel5;
	bool showPanel6;
	bool showHGA;

	bool A13Exploded;

	double EmptyMass;
	double PayloadMass;
	double MainFuel;
	double RCSAFuel;
	double RCSBFuel;
	double RCSCFuel;
	double RCSDFuel;

	double MissionTime;
	double NextMissionEventTime;
	double LastMissionEventTime;

	PROPELLANT_HANDLE ph_rcsa, ph_rcsb, ph_rcsc, ph_rcsd;
};
