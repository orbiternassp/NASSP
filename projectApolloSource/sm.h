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
  *	Revision 1.6  2006/05/30 22:34:33  movieman523
  *	Various changes. Panel switches now need power, APO and PER correctly placed in scenario fle, disabled some warnings, moved 'window' sound message to the correct place, added heat measurement to SM DLL for re-entry.
  *	
  *	Revision 1.5  2006/05/04 20:46:50  movieman523
  *	Added re-entry texture and started heat tracking.
  *	
  *	Revision 1.4  2006/04/05 19:33:49  movieman523
  *	Support low-res RCS mesh, saved umbilical animation state so it only happens once, revised Apollo 13 support.
  *	
  *	Revision 1.3  2006/04/04 22:00:54  jasonims
  *	Apollo Spacecraft Mesh offset corrections and SM Umbilical Animation.
  *	
  *	Revision 1.2  2006/03/30 01:59:37  movieman523
  *	Added RCS to SM DLL.
  *	
  *	Revision 1.1  2006/03/30 00:21:37  movieman523
  *	Pass empty mass correctly and remember to check in SM files :).
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

#define SM_UMBILICALDETACH_PAUSE	0
#define SM_STATE_RCS_START			1
#define SM_STATE_RCS_ROLL_START		2
#define SM_STATE_RCS_ROLL_STOP		3
#define SM_STATE_WAITING			4

///
/// \brief Speed at which the SM to CM umbilical moves away from the CM.
///
const double UMBILICAL_SPEED = 0.5;

///
/// This code simulates the seperated Service Module. If the SM RCS has power and fuel it fires the RCS to push
/// the SM away from the CM and set it rotating. Then it disintegrates during re-entry.
///
/// \brief SM stage simulation.
/// \ingroup SepStages
///
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

	double umbilical_proc;
	UINT anim_umbilical;

protected:

	void SetSM();
	void InitSM();
	void AddEngines();
	void DefineAnimations();

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
	bool showCRYO;

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

	double Temperature;

	SURFHANDLE CMTex;

	PROPELLANT_HANDLE ph_rcsa, ph_rcsb, ph_rcsc, ph_rcsd;
	THRUSTER_HANDLE th_att_lin[24], th_att_rot[24], th_rcs_a[4], th_rcs_b[4], th_rcs_c[4], th_rcs_d[4];
};
