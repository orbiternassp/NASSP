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
  *	Revision 1.4  2010/07/16 17:14:42  tschachim
  *	Changes for Orbiter 2010 and bugfixes
  *	
  *	Revision 1.3  2010/02/22 14:23:31  tschachim
  *	Apollo 7 S-IVB on orbit attitude control, venting and Saturn takeover mode for the VAGC.
  *	
  *	Revision 1.2  2010/01/04 12:31:15  tschachim
  *	Improved Saturn IB launch autopilot, bugfixes
  *	
  *	Revision 1.1  2009/02/18 23:21:34  tschachim
  *	Moved files as proposed by Artlav.
  *	
  *	Revision 1.26  2008/05/24 17:29:19  tschachim
  *	Improved autopilot/pitch table
  *	
  *	Revision 1.25  2008/04/11 12:19:16  tschachim
  *	New SM and CM RCS.
  *	Improved abort handling.
  *	Fixed BasicExcel for VC6, reduced VS2005 warnings, bugfixes.
  *	
  *	Revision 1.24  2008/01/16 05:52:07  movieman523
  *	Removed all dockstate code.
  *	
  *	Revision 1.23  2008/01/14 04:31:10  movieman523
  *	Initial tidyup: ASTP should now work too.
  *	
  *	Revision 1.22  2007/12/04 20:26:35  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.21  2007/06/06 15:02:17  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.20  2006/08/27 21:57:10  tschachim
  *	Bugfix RCS handing in CSM timestep.
  *	
  *	Revision 1.19  2006/07/31 12:25:38  tschachim
  *	Smoother staging.
  *	
  *	Revision 1.18  2006/06/24 15:40:06  movieman523
  *	Working on MET-driven audio playback. Also added initial Doxygen comments.
  *	
  *	Revision 1.17  2006/06/11 21:30:57  movieman523
  *	Fixed Saturn 1b SIVb exhaust.
  *	
  *	Revision 1.16  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.15  2006/05/29 21:03:45  movieman523
  *	Forgot ConfigureStageEngines().
  *	
  *	Revision 1.14  2006/05/27 11:50:04  movieman523
  *	Improved INT20 support, and made LET jettison work any time during launch on Saturn V.
  *	
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
#pragma once
#include "LVDC.h"
///
/// \brief Saturn V launch vehicle class.
/// \ingroup Saturns
///

class Saturn1b: public Saturn {
friend class LVDC1B;
public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	Saturn1b (OBJHANDLE hObj, int fmodel);
	virtual ~Saturn1b();

	void initSaturn1b();

	///
	/// \brief Orbiter state loading function.
	/// \param scn Scenario file to load from.
	/// \param status Pointer to current vessel status.
	///
	void clbkLoadStateEx (FILEHANDLE scn, void *status);

	///
	/// \brief Orbiter class configuration function.
	/// \param cfg File to load configuration defaults from.
	///
	void clbkSetClassCaps (FILEHANDLE cfg);
	int  clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPostStep (double simt, double simdt, double mjd);

	///
	/// \brief Set up J2 engines as fuel venting thruster.
	///
	virtual void SetVentingJ2Thruster();

protected:

	VECTOR3 RelPos;

	OBJHANDLE hSoyuz;
	OBJHANDLE hAstpDM;
	LVDC1B lvdc;
	double LiftCoeff (double aoa);

	void SetupMeshes();
	void AttitudeLaunch1();
	void AttitudeLaunch4();
	void AutoPilot(double autoT);
	void SetFirstStage ();
	void SetFirstStageMeshes(double offset);
	void SetFirstStageEngines ();
	void SetSecondStage ();
	void SetSecondStageMeshes(double offset);
	void SetSecondStageEngines ();

	void ConfigureStageMeshes(int stage_state);
	void ConfigureStageEngines(int stage_state);
	void CreateStageOne();

	void SaveVehicleStats(FILEHANDLE scn);
	void SeparateStage (int stage);
	void DoFirstTimestep(double simt);
	void Timestep (double simt, double simdt, double mjd);
	void StageOne(double simt, double simdt);
	void StageLaunchSIVB(double simt);
	void SetVehicleStats();
	void CalculateStageMass ();
	void SetSIVBMixtureRatio(double ratio);
	void ActivateStagingVent();
	void DeactivateStagingVent();
	void ActivatePrelaunchVenting();
	void DeactivatePrelaunchVenting();
};


const VECTOR3 OFS_STAGE1 = { 0, 0, -14};
const VECTOR3 OFS_STAGE12 = { 0, 0, -9.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, -24 + 1.45};
const VECTOR3 OFS_STAGE21 = { 1.85,1.85,32};
const VECTOR3 OFS_STAGE22 = { -1.85,1.85,32};
const VECTOR3 OFS_STAGE23 = { 1.85,-1.85,32};
const VECTOR3 OFS_STAGE24 = { -1.85,-1.85,32};
const VECTOR3 OFS_SM = { 0, 0, -2.05};
const VECTOR3 OFS_CM_CONE = { 0, 0, 36.05-12.25-21.5};

const double STG0O = 0;
const double STG1O = 10.25;

extern void Saturn1bLoadMeshes();