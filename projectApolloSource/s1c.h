/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: S1C class

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
  *	Revision 1.4  2006/05/19 13:46:56  tschachim
  *	Smoother S-IC staging.
  *	
  *	Revision 1.3  2006/01/26 03:07:49  movieman523
  *	Quick hack to support low-res mesh.
  *	
  *	Revision 1.2  2006/01/08 04:00:24  movieman523
  *	Added first two engineering cameras.
  *	
  *	Revision 1.1  2005/11/23 00:34:16  movieman523
  *	S1C dll code.
  *	
  *	
  **************************************************************************/

//
// Data structure passed from main vessel to SIVB to configure stage.
//

typedef struct {

	int SettingsType;
	int VehicleNo;
	int Realism;
	int RetroNum;
	int EngineNum;

	double THRUST_FIRST_VAC;
	double ISP_FIRST_VAC;
	double ISP_FIRST_SL;
	double MissionTime;
	double EmptyMass;
	double MainFuelKg;
	double CurrentThrust;

	bool LowRes;
	bool S4Interstage;
	bool Stretched;

} S1CSettings;

//
// Which parts of the structure are active.
//

#define S1C_SETTINGS_MASS		0x1
#define S1C_SETTINGS_FUEL		0x2
#define S1C_SETTINGS_GENERAL	0x4
#define S1C_SETTINGS_ENGINES	0x8

//
// Stage states.
//

#define SIC_STATE_HIDDEN           -1
#define S1C_STATE_SHUTTING_DOWN		0
#define S1C_STATE_WAITING			1

//
// Stage class.
//

class S1C : public VESSEL2 {

public:
	S1C (OBJHANDLE hObj, int fmodel);
	virtual ~S1C();

	void clbkSaveState (FILEHANDLE scn);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkLoadStateEx (FILEHANDLE scn, void *status);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkDockEvent(int dock, OBJHANDLE connected);
	bool clbkLoadGenericCockpit ();
	bool clbkLoadVC (int id);

	//
	// Must be virtual so it can be called from other DLLs.
	//
	virtual void SetState(S1CSettings &state);
	virtual void LoadMeshes(bool lowres);

protected:

	SoundLib soundlib;

	void SetS1c();
	void InitS1c();
	void AddEngines();
	void ShowS1c();

	int GetMainState();
	void SetMainState(int s);

	int MissionNo;
	int VehicleNo;
	int State;
	int Realism;
	int RetroNum;
	int EngineNum;

	bool RetrosFired;
	bool LowRes;
	bool S4Interstage;
	bool Stretched;

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

	MESHHANDLE hsat5stg1, hsat5stg1low, hS4Interstage;
	THRUSTER_HANDLE th_retro[4], th_main[5];
	THGROUP_HANDLE thg_retro, thg_main;
	PROPELLANT_HANDLE ph_retro, ph_main;
};
