/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  The actual SaturnV class.

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
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

class SaturnV: public Saturn {

public:
	SaturnV (OBJHANDLE hObj, int fmodel);
	virtual ~SaturnV();

	//
	// General functions that handle calls from Orbiter.
	//

	void Timestep(double simt);
	void clbkLoadStateEx (FILEHANDLE scn, void *status);
	void clbkSetClassCaps (FILEHANDLE cfg);

	void initSaturnV();

	//
	// Functions that external code shouldn't need to access.
	//

private:
	void BuildFirstStage (int bstate);
	void SetFirstStage ();
	void SetSecondStage ();
	void SetSecondStage1 ();
	void SetSecondStage2 ();
	void SetThirdStage ();
	void Ullage(OBJHANDLE hvessel,double gaz);
	void Ullage2(OBJHANDLE hvessel,double gaz);
	void setupS4B(OBJHANDLE hvessel);
	void AttitudeLaunch1();
	void AttitudeLaunch2();
	void AttitudeLaunch4();
	void AutoPilot(double autoT);
	void SetSIICMixtureRatio (double ratio);
	void AccelS4B(OBJHANDLE hvessel, double time);
	void MoveEVA();

	void SeparateStage (int stage);
	void DockStage (UINT dockstatus);

	void SetVehicleStats();
	void CalculateStageMass ();

	bool SIVBStart();
	void SIVBStop();

	void SetPayloadMesh(VESSEL *s4b);

protected:

	//
	// Mission stage functions.
	//

	void StageOne(double simt);
	void StageTwo(double simt);
	void StageThree(double simt);
	void StageFour(double simt);
	void StageSix(double simt);
	void StageLaunchSIVB(double simt);

	//
	// Mission-support functions.
	//

	void SaveVehicleStats(FILEHANDLE scn);

	//
	// Odds and ends.
	//

	void DoFirstTimestep(double simt);

	//
	// Class variables.
	//

	int CurrentTimestep;
	int LongestTimestep;
	double LongestTimestepLength;

	bool stgSM;
	bool GoHover;

	bool Resetjet;

	bool S4Sep;
	bool velDISP;

	bool S4Bset;

	OBJHANDLE hstage1;
	OBJHANDLE hstage2;
	OBJHANDLE hLMV;

	//
	// End state.
	//

	double gaz;

	int UllageID6;
	int UllageID7;
	int UllageID8;
	int UllageID9;

	//
	// ISP and thrust values, which vary depending on vehicle number.
	//

	double ISP_FIRST_SL;
	double ISP_FIRST_VAC;
	double ISP_SECOND_SL;//300*G;
	double ISP_SECOND_VAC;//421*G;
	double ISP_THIRD_VAC;//421*G;

	double THRUST_FIRST_VAC;
	double THRUST_SECOND_VAC;//115200*G;
	double THRUST_THIRD_VAC;

	int SI_RetroNum;
	int SII_UllageNum;

	double Stage1Mass;
	double Stage2Mass;
	double Stage3Mass;

	//
	// Sounds.
	//

	Sound DockS;

	Sound SpeedS;

	Sound SCorrection;
	Sound S5P100;

	Sound SRover;
	Sound SPUShiftS;

	//
	// Obsolete variables... currently in for backward compatibility, and
	// removed as code is deleted.
	//

	double ignition_S4time;
	bool S4Shoot;

};

extern void LoadSat5Meshes();


const double STG0O= 20.4;
const double STG1O= -5.25;
const VECTOR3 OFS_STAGE1 =  { 0, 0, -54.935};
const VECTOR3 OFS_STAGE12 =  { 0, 0, -30.5};
const VECTOR3 OFS_STAGE2 =  { 0, 0, -20.25};
const VECTOR3 OFS_STAGE3 =  { 0, 0, 7.5+STG2O+21.5};
const VECTOR3 OFS_STAGE31 =  { -1.48,-1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE32 =  { 1.48,-1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE33 =  { 1.48,1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE34 =  { -1.48,1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_SM =  { 0, 0, 31.05-12.25-21.5-1.0};
const VECTOR3 OFS_ABORT =  { 0,0,-34.40};
const VECTOR3 OFS_ABORT2 =  { 0,0,-22.15};
const VECTOR3 OFS_ABORT_TOWER =  { 0,0,5};
const VECTOR3 OFS_CM_CONE =  { 0, 0, 36.05-12.25-21.5};
//const VECTOR3 OFS_MAINCHUTE =  { 0, 0, 30.15-12.25-21.5-4};

const VECTOR3 OFS_TOWER =  { 0, 0, 40.05};
const VECTOR3 OFS_DOCKING =  { 0.0,0.0,14.5};
const VECTOR3 OFS_DOCKING2 =  { 0.0,-2.4,22.4};
