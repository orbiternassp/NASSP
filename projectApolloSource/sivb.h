/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SIVb class

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
  *	Revision 1.5  2005/11/20 01:06:27  movieman523
  *	Saturn V now uses SIVB DLL too.
  *	
  *	Revision 1.4  2005/11/19 22:58:32  movieman523
  *	Pass main fuel mass from Saturn 1b to SIVb and added main thrust from venting fuel.
  *	
  *	Revision 1.3  2005/11/19 22:19:07  movieman523
  *	Revised interface to update SIVB, and added payload mass and stage empty mass.
  *	
  *	Revision 1.2  2005/11/19 22:05:16  movieman523
  *	Added RCS to SIVb stage after seperation.
  *	
  *	Revision 1.1  2005/11/19 21:27:31  movieman523
  *	Initial SIVb implementation.
  *	
  **************************************************************************/

//
// Data structure passed from main vessel to SIVB to configure stage.
//

typedef struct {
	int SettingsType;
	int Payload;
	int VehicleNo;
	int Realism;
	double MissionTime;
	double EmptyMass;
	double PayloadMass;
	double ApsFuelKg;
	double MainFuelKg;
	bool PanelsHinged;
	bool SaturnVStage;
} SIVBSettings;

//
// Which parts of the structure are active.
//

#define SIVB_SETTINGS_MASS		0x1
#define SIVB_SETTINGS_PAYLOAD	0x2
#define SIVB_SETTINGS_FUEL		0x4
#define SIVB_SETTINGS_GENERAL	0x8

//
// Stage states.
//

#define SIVB_STATE_WAITING		0

//
// Stage class.
//

class SIVB : public VESSEL2 {

public:
	SIVB (OBJHANDLE hObj, int fmodel);
	virtual ~SIVB();

	void clbkSaveState (FILEHANDLE scn);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkLoadStateEx (FILEHANDLE scn, void *status);
	void clbkSetClassCaps (FILEHANDLE cfg);
	void clbkDockEvent(int dock, OBJHANDLE connected);

	//
	// Must be virtual so it can be called from other DLLs.
	//
	virtual void SetState(SIVBSettings &state);

protected:

	void SetS4b();
	void InitS4b();

	int GetMainState();
	void SetMainState(int s);
	void GetApolloName(char *s);
	void AddRCS_S4B();
	void Boiloff();

	int Payload;
	int MissionNo;
	int VehicleNo;
	int State;
	int Realism;

	double EmptyMass;
	double PayloadMass;
	double MainFuel;

	double MissionTime;
	double NextMissionEventTime;
	double LastMissionEventTime;

	bool PanelsHinged;
	bool PanelsOpened;
	bool SaturnVStage;

	double CurrentThrust;

	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;

	DOCKHANDLE hDock;

	THRUSTER_HANDLE th_att_rot[10], th_main[1], th_att_lin[2];                 // handles for APS engines
	THGROUP_HANDLE thg_aps, thg_main;
	PROPELLANT_HANDLE ph_aps, ph_main;
};
