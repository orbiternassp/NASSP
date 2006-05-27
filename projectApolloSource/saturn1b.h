/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  The actual Saturn1b class.

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
  *	Revision 1.13  2006/01/27 22:11:38  movieman523
  *	Added support for low-res Saturn 1b.
  *	
  *	Revision 1.12  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.11  2006/01/15 02:38:59  movieman523
  *	Moved CoG and removed phantom thrusters. Also delete launch site when we get a reasonable distance away.
  *	
  *	Revision 1.10  2006/01/12 14:48:44  tschachim
  *	Added prelaunch tank venting.
  *	
  *	Revision 1.9  2005/12/19 17:02:25  tschachim
  *	Introduced clbkConsumeBufferedKey
  *	
  *	Revision 1.8  2005/12/13 15:21:46  tschachim
  *	Fixed SIVB separation offset (hopefully).
  *	
  *	Revision 1.7  2005/11/24 20:31:23  movieman523
  *	Added support for engine thrust decay during launch.
  *	
  *	Revision 1.6  2005/11/23 02:21:30  movieman523
  *	Added S1b stage.
  *	
  *	Revision 1.5  2005/11/19 20:54:47  movieman523
  *	Added SIVb DLL and wired it up to Saturn 1b.
  *	
  *	Revision 1.4  2005/08/17 00:01:59  movieman523
  *	Added ECS indicator switch, revised state saving, revised Timestep code to pass in the delta-time so we don't need to keep calculating it.
  *	
  *	Revision 1.3  2005/07/30 02:05:55  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.2  2005/07/29 22:44:05  movieman523
  *	Pitch program, SI center shutdown time, SII center shutdown time and SII PU shift time can now all be specified in the scenario files.
  *	
  *	Revision 1.1  2005/02/11 12:17:55  tschachim
  *	Initial version
  *	
  **************************************************************************/

class Saturn1b: public Saturn {

public:
	Saturn1b (OBJHANDLE hObj, int fmodel);
	virtual ~Saturn1b();

	void initSaturn1b();

protected:

	bool MasterVessel;
	bool TargetDocked;
	bool DestroyAstp;
	bool ReadyAstp;
	bool ReadyAstp1;
	bool S4BASTP;
	bool FIRSTCSM;

	VECTOR3 RelPos;

	OBJHANDLE hSoyuz;
	OBJHANDLE hAstpDM;

	double LiftCoeff (double aoa);

	void SetupMeshes();
	void AttitudeLaunch1();
	void AttitudeLaunch4();
	void AutoPilot(double autoT);
	void SetFirstStage ();
	void SetSecondStage ();
	void SetSecondStage1 ();
	void SetSecondStage2 ();

	void ConfigureStageMeshes(int stage_state);

	void SetASTPStage ();
	void clbkLoadStateEx (FILEHANDLE scn, void *vs);
	void clbkSetClassCaps (FILEHANDLE cfg);
	int  clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void SaveVehicleStats(FILEHANDLE scn);
	void SeparateStage (int stage);
	void DoFirstTimestep(double simt);
	void Timestep (double simt, double simdt);
	void DockStage (UINT dockstatus);
	void StageOne(double simt, double simdt);
	void StageStartSIVB(double simt);
	void StageLaunchSIVB(double simt);
	void SetVehicleStats();
	void CalculateStageMass ();
	void SetSIVBMixtureRatio(double ratio);
	// Prelaunch tank venting not implemented for the Saturn 1b at the moment
	void ActivatePrelaunchVenting() {};
	void DeactivatePrelaunchVenting() {};
};

const VECTOR3 OFS_STAGE1 =  { 0, 0, -26};
const VECTOR3 OFS_STAGE12 =  { 0, 0, -9.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, -24 + 1.45};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,32};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,32};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,32};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,32};
const VECTOR3 OFS_SM =  { 0, 0, 30.15-12.25-21.5};
const VECTOR3 OFS_ABORT =  { 0,0,-34.40};
const VECTOR3 OFS_ABORT2 =  { 0,0,-22.15};
const VECTOR3 OFS_ABORT_TOWER =  { 0,0,5};
const VECTOR3 OFS_CM_CONE =  { 0, 0, 36.05-12.25-21.5};

const double STG0O= 0;
const double STG1O= 10.25;

const VECTOR3 OFS_TOWER =  { 0, 0, 40.05};
const VECTOR3 OFS_DOCKING =  { 0.0,0.0,14.5};
const VECTOR3 OFS_DOCKING2 =  { 0.0,0.0,4.0};

extern void Saturn1bLoadMeshes();