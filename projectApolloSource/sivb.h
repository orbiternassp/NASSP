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
  *	Revision 1.2  2005/11/19 22:05:16  movieman523
  *	Added RCS to SIVb stage after seperation.
  *	
  *	Revision 1.1  2005/11/19 21:27:31  movieman523
  *	Initial SIVb implementation.
  *	
  **************************************************************************/

typedef struct {
	int SettingsType;
	int Payload;
	int VehicleNo;
	double EmptyMass;
	double PayloadMass;
	double ApsFuelKg;
	double MainFuelKg;
	bool PanelsHinged;
} SIVBSettings;

#define SIVB_SETTINGS_MASS		0x1
#define SIVB_SETTINGS_PAYLOAD	0x2
#define SIVB_SETTINGS_FUEL		0x4
#define SIVB_SETTINGS_GENERAL	0x8

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

	int Payload;
	int MissionNo;
	int VehicleNo;

	double EmptyMass;
	double PayloadMass;
	double MainFuel;

	bool PanelsHinged;
	bool PanelsOpened;

	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;

	DOCKHANDLE hDock;

	THRUSTER_HANDLE th_att_rot[10], th_att_lin[2];                 // handles for APS engines
	THGROUP_HANDLE thg_aps;
	PROPELLANT_HANDLE ph_aps, ph_main;
};
