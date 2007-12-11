/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn 1b mesh code

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
  *	Revision 1.64  2007/12/05 23:07:45  movieman523
  *	Revised to allow SLA panel rotaton to be specified up to 150 degrees. Also start of new connector-equipped vessel code which was mixed up with the rest!
  *	
  *	Revision 1.63  2007/12/04 20:26:33  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.62  2007/12/04 06:51:07  movieman523
  *	Shifted origin of Saturn 1b SIVB meshes to match Saturn V SIVB.
  *	
  *	Revision 1.61  2007/12/03 07:14:22  movieman523
  *	Moved Saturn 1b SIVB mesh back so that payload offsets match.
  *	
  *	Revision 1.60  2007/12/02 07:13:39  movieman523
  *	Updates for Apollo 5 and unmanned Saturn 1b missions.
  *	
  *	Revision 1.59  2007/10/18 00:23:20  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.58  2007/08/13 16:06:14  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.57  2007/06/06 15:02:16  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.56  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.55  2007/02/06 18:30:17  tschachim
  *	Bugfixes docking probe, CSM/LM separation. The ASTP stuff still needs fixing though.
  *	
  *	Revision 1.54  2007/01/23 14:41:57  tschachim
  *	Bugfix docking probe jettison.
  *	
  *	Revision 1.53  2007/01/21 18:34:12  jasonims
  *	shifted j2 exhast
  *	
  *	Revision 1.52  2006/12/07 18:52:43  tschachim
  *	New LC34, Bugfixes.
  *	
  *	Revision 1.51  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.50  2006/09/23 22:34:40  jasonims
  *	New J-2 Engine textures...
  *	
  *	Revision 1.49  2006/08/27 21:52:42  tschachim
  *	Added dummy docking port so the auto burn feature of IMFD 4.2 is working.
  *	
  *	Revision 1.48  2006/08/11 20:37:46  movieman523
  *	Added HasProbe flag for docking probe.
  *	
  *	Revision 1.47  2006/08/04 19:01:23  jasonims
  *	corrected many of the mesh problems experienced when on pad and during launch.
  *	
  *	Revision 1.45  2006/08/02 04:32:05  jasonims
  *	corrected probe problem
  *	
  *	Revision 1.44  2006/08/01 16:05:57  tschachim
  *	Improved staging exhausts.
  *	
  *	Revision 1.43  2006/07/31 12:24:06  tschachim
  *	Smoother staging.
  *	
  *	Revision 1.42  2006/06/26 19:05:36  movieman523
  *	More doxygen, made Lunar EVA a VESSEL2, made SM breakup, made LRV use VESSEL2 save/load functions.
  *	
  *	Revision 1.41  2006/06/11 21:30:57  movieman523
  *	Fixed Saturn 1b SIVb exhaust.
  *	
  *	Revision 1.40  2006/06/11 17:16:34  movieman523
  *	Saturn 1b can now reach orbit again :).
  *	
  *	Revision 1.39  2006/06/10 23:27:41  movieman523
  *	Updated abort code.
  *	
  *	Revision 1.38  2006/06/10 14:36:44  movieman523
  *	Numerous changes. Lots of bug-fixes, new LES jettison code, lighting for guarded push switches and a partial rewrite of the Saturn 1b mesh code.
  *	
  *	Revision 1.37  2006/05/27 11:50:04  movieman523
  *	Improved INT20 support, and made LET jettison work any time during launch on Saturn V.
  *	
  *	Revision 1.36  2006/05/01 03:33:21  jasonims
  *	New CM and all the fixin's....
  *	
  *	Revision 1.35  2006/03/30 00:21:37  movieman523
  *	Pass empty mass correctly and remember to check in SM files :).
  *	
  *	Revision 1.34  2006/03/30 00:14:46  movieman523
  *	First pass at SM DLL.
  *	
  *	Revision 1.33  2006/03/29 19:06:49  movieman523
  *	First support for new SM.
  *	
  *	Revision 1.32  2006/02/22 01:03:02  movieman523
  *	Initial Apollo 5 support.
  *	
  *	Revision 1.31  2006/01/27 22:11:38  movieman523
  *	Added support for low-res Saturn 1b.
  *	
  *	Revision 1.30  2006/01/26 19:26:31  movieman523
  *	Now we can set any scenario state from the config file for Saturn 1b or Saturn V. Also wired up a couple of LEM switches.
  *	
  *	Revision 1.29  2006/01/15 02:38:59  movieman523
  *	Moved CoG and removed phantom thrusters. Also delete launch site when we get a reasonable distance away.
  *	
  *	Revision 1.28  2006/01/08 04:00:24  movieman523
  *	Added first two engineering cameras.
  *	
  *	Revision 1.27  2006/01/06 22:55:53  movieman523
  *	Fixed SM seperation and cut off fuel cell power when it happens.
  *	
  *	Revision 1.26  2006/01/05 19:40:53  movieman523
  *	Added Saturn1b abort stages to build.
  *	
  *	Revision 1.25  2006/01/04 23:06:03  movieman523
  *	Moved meshes into ProjectApollo directory and renamed a few.
  *	
  *	Revision 1.24  2005/12/28 16:19:10  movieman523
  *	Should now be getting all config files from ProjectApollo directory.
  *	
  *	Revision 1.23  2005/11/24 20:31:23  movieman523
  *	Added support for engine thrust decay during launch.
  *	
  *	Revision 1.22  2005/11/23 02:21:30  movieman523
  *	Added S1b stage.
  *	
  *	Revision 1.21  2005/11/20 20:35:14  movieman523
  *	Moved mesh files into ProjectApollo directory, and fixed RCS on Saturn V SIVb after seperation.
  *	
  *	Revision 1.20  2005/11/20 01:06:27  movieman523
  *	Saturn V now uses SIVB DLL too.
  *	
  *	Revision 1.19  2005/11/19 22:58:32  movieman523
  *	Pass main fuel mass from Saturn 1b to SIVb and added main thrust from venting fuel.
  *	
  *	Revision 1.18  2005/11/19 22:19:07  movieman523
  *	Revised interface to update SIVB, and added payload mass and stage empty mass.
  *	
  *	Revision 1.17  2005/11/19 22:05:16  movieman523
  *	Added RCS to SIVb stage after seperation.
  *	
  *	Revision 1.16  2005/11/19 20:54:47  movieman523
  *	Added SIVb DLL and wired it up to Saturn 1b.
  *	
  *	Revision 1.15  2005/11/16 20:21:39  movieman523
  *	CSM/LEM renaming changes.
  *	
  *	Revision 1.14  2005/11/16 00:18:49  movieman523
  *	Added beginnings of really basic IU emulation. Added random failures of caution and warning lights on non-historical missions. Added initial support for Skylab CM and SM. Added LEM Name option in scenario file.
  *	
  *	Revision 1.13  2005/08/24 00:30:00  movieman523
  *	Revised CM RCS code, and removed a load of switches that aren't used anymore.
  *	
  *	Revision 1.12  2005/08/15 23:42:50  movieman523
  *	Improved ASTP a bit. Still buggy, but vaguely working.
  *	
  *	Revision 1.11  2005/08/10 21:54:04  movieman523
  *	Initial IMU implementation based on 'Virtual Apollo' code.
  *	
  *	Revision 1.10  2005/08/06 01:12:52  movieman523
  *	Added initial I/O channel support for CSM, and added Realism setting for LEM AGC.
  *	
  *	Revision 1.9  2005/07/31 01:43:12  movieman523
  *	Added CM and SM fuel and empty mass to scenario file and adjusted masses to more accurately match reality.
  *	
  *	Revision 1.8  2005/07/30 02:05:47  movieman523
  *	Revised Saturn 1b code. Performance and mass is now closer to reality, and I've added the mixture ratio shift late in the SIVB burn.
  *	
  *	Revision 1.7  2005/03/28 05:52:44  chode99
  *	Support for defining SIVB payloads in the scenario like the Saturn V.
  *	e.g. S4PL 5 is the Apollo 7 COAS target, 1 is the ASTP docking module.
  *	
  *	Revision 1.6  2005/03/26 01:46:30  chode99
  *	Added retros to first stage.
  *	
  *	Revision 1.5  2005/03/24 01:42:40  chode99
  *	Moved first stage thrusters,  added practice target for Apollo 7.
  *	
  *	Revision 1.4  2005/03/16 13:31:58  yogenfrutz
  *	added missing setview and crew,so that virtual cockpit does now display correctly
  *	
  *	Revision 1.3  2005/03/06 03:23:26  chode99
  *	Relocated and redirected the SIVB ullage thrusters to coincide with the mesh.
  *	
  *	Revision 1.2  2005/02/19 19:45:37  chode99
  *	Moved 1 line of code (VECTOR3 mesh_dir=...) to allow compilation in .NET 2003
  *	
  *	Revision 1.1  2005/02/11 12:54:07  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"

#include "saturn1b.h"

#include "sivb.h"
#include "s1b.h"
#include "sm.h"

//
// Meshes are loaded globally, once, so we use these global
// variables for them. Nothing vessel-specific should use a
// global variable.
//

static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1stg1low;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1intstglow;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg2low;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hNosecap;
static MESHHANDLE hastp;
static MESHHANDLE hastp2;
static MESHHANDLE hCOAStarget;

static SURFHANDLE exhaust_tex;

//
// Same for particle streams.
//

PARTICLESTREAMSPEC srb_contrail = {
	0, 12.0, 1, 50.0, 0.3, 4.0, 4, 3.0, PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};


/*
PARTICLESTREAMSPEC srb_exhaust = {
	0, 8.0, 2, 50.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,//	0, 4.0, 20, 150.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};
*/

PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	2.85,	// size
	1500,	// rate
	60.0,	// velocity
	0.1,	// velocity distribution
	1.0,	// lifetime
	2.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-1140, 1.0
};

PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0, 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};


void Saturn1b::SetFirstStage ()
{
	SetSize (45);
	SetEmptyMass (Stage1Mass);
	SetPMI (_V(140,145,28));
	SetCrossSections (_V(395,380,115));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();

	SetFirstStageMeshes(-14.0);
	SetSecondStageMeshes(13.95);

	SetView(34.95, false);

	Offset1st = -28.5;
	SetCameraOffset (_V(-1,1.0,35.15));

	EnableTransponder (true);

	// **************************** NAV radios *************************************

	InitNavRadios (4);
}

void Saturn1b::SetFirstStageMeshes(double offset)

{
	double TCP=-54.485-TCPO;//STG0O;
	SetTouchdownPoints (_V(0,-1.0,TCP), _V(-.7,.7,TCP), _V(.7,.7,TCP));

	VECTOR3 mesh_dir=_V(0,0,offset);

	AddMesh (hStage1Mesh, &mesh_dir);
	mesh_dir=_V(0,0,16.2 + offset);
	AddMesh (hInterstageMesh, &mesh_dir);
}

void Saturn1b::SetFirstStageEngines()

{
	ClearThrusters();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	// ************************* propellant specs **********************************
	if (!ph_1st)
		ph_1st  = CreatePropellantResource(SI_FuelMass); //1st stage Propellant
	SetDefaultPropellantResource (ph_1st); // display 1st stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	int i;

	Offset1st = -80.1;//+STG0O;
	VECTOR3 m_exhaust_ref = {0,0,-1};

	VECTOR3 m_exhaust_pos5= {0,1.414,Offset1st+55};
    VECTOR3 m_exhaust_pos6= {1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos7= {0,-1.414,Offset1st+55};
	VECTOR3 m_exhaust_pos8= {-1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos1= {2.12,2.12,Offset1st+55};
    VECTOR3 m_exhaust_pos2= {2.12,-2.12,Offset1st+55};
	VECTOR3 m_exhaust_pos3= {-2.12,-2.12,Offset1st+55};
	VECTOR3 m_exhaust_pos4= {-2.12,2.12,Offset1st+55};

	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[5] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[6] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[7] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("ProjectApollo/Exhaust2");
	thg_main = CreateThrusterGroup (th_main, 8, THGROUP_MAIN);
	for (i = 0; i < 8; i++)
		AddExhaust(th_main[i], 30.0, 0.80, tex);

	srb_exhaust.tex = oapiRegisterParticleTexture ("Contrail2");

	double exhpos = -5;
	//AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,exhpos), &srb_exhaust);
	//AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,exhpos), &srb_exhaust);
	//AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,exhpos), &srb_exhaust);
	//AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,exhpos), &srb_exhaust);
	//AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,exhpos), &srb_exhaust);
	//AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,exhpos), &srb_exhaust);
	//AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,exhpos), &srb_exhaust);
	//AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,-15), &srb_contrail);
	AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,exhpos), &srb_exhaust);

	// Contrail
	for (i = 0; i < 8; i++) {
		if (contrail[i]) {
			DelExhaustStream(contrail[i]);
			contrail[i] = NULL;
		}
	}

	double conpos = -10;
	contrail[0] = AddParticleStream(&srb_contrail, m_exhaust_pos1+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[1] = AddParticleStream(&srb_contrail, m_exhaust_pos2+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[2] = AddParticleStream(&srb_contrail, m_exhaust_pos3+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[3] = AddParticleStream(&srb_contrail, m_exhaust_pos4+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[4] = AddParticleStream(&srb_contrail, m_exhaust_pos5+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[5] = AddParticleStream(&srb_contrail, m_exhaust_pos6+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[6] = AddParticleStream(&srb_contrail, m_exhaust_pos7+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[7] = AddParticleStream(&srb_contrail, m_exhaust_pos8+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);

}

void Saturn1b::SetSecondStage ()
{
	SetSize (22);
	SetCOG_elev (15.225);

	double EmptyMass = Stage2Mass + LESAttached ? Abort_Mass : 0.0;

	SetEmptyMass (EmptyMass);
	SetPMI (_V(94,94,20));
	SetCrossSections (_V(267,267,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);

    ClearMeshes();
	SetSecondStageMeshes(13.95-12.25);
}

void Saturn1b::SetSecondStageMeshes(double offset)
{
	UINT meshidx;

	VECTOR3 mesh_dir=_V(0,0,offset);
    AddMesh (hStage2Mesh, &mesh_dir);

	mesh_dir=_V(1.85,1.85,10.55 + offset);
    AddMesh (hStageSLA1Mesh, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,10.55 + offset);
    AddMesh (hStageSLA2Mesh, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,10.55 + offset);
    AddMesh (hStageSLA3Mesh, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,10.55 + offset);
    AddMesh (hStageSLA4Mesh, &mesh_dir);

	probeidx = -1;
	probeextidx = -1;
	crewidx = -1;
	cmpidx = -1;

	if (SaturnHasCSM()) {

		//
		// Add CSM.
		//

		AddSM(17.05 + offset, false);

		WORD CMMode = MESHVIS_VCEXTERNAL;

		if (LESAttached)
		{
			TowerOffset = 26.15 + offset;
			mesh_dir=_V(0, 0, TowerOffset);
			meshidx = AddMesh (hsat5tower, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			//
			// If the LES is attached, no point drawing things in the external view which can't
			// actually be seen.
			//
			CMMode = MESHVIS_VC;
		}
		else if (HasProbe)
		{
			mesh_dir=_V(0,0,21.3 + offset);
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		}

		mesh_dir=_V(0,0,21.3 + offset);
		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, CMMode);

		//
		// And the Crew.
		//
		if (Crewed) {
			cmpidx = AddMesh (hCMP, &mesh_dir);
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		}

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, CMMode);

		//
		// Don't Forget the Hatch
		//
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, CMMode);

	}
	else if (NosecapAttached)
	{
		//
		// Add nosecap.
		//
		mesh_dir=_V(0,0,29.77-12.25);
		AddMesh (hNosecap, &mesh_dir);
	}

	// Dummy docking port so the auto burn feature of IMFD 4.2 is working
	// Remove it when a newer release of IMFD don't need that anymore
	VECTOR3 dockpos = {0,0,24.8 + offset};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams(dockpos, dockdir, dockrot);

	SetCameraOffset (_V(-1,1.0,31.15-STG1O));
    SetView(22.7, false);
}

void Saturn1b::SetSecondStageEngines ()

{
	ClearThrusters();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	//
	// ************************* propellant specs **********************************
	//

	if(ph_1st)
	{
		//
		// Delete remaining S1c stage propellant.
		//
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}

	if (!ph_3rd)
	{
		//
		// Create SIVB stage Propellant
		//

		ph_3rd  = CreatePropellantResource(SII_FuelMass);
	}

	if (!ph_ullage3)
	{
		//
		// Create SIVB stage ullage rocket propellant
		//

		ph_ullage3  = CreatePropellantResource(80.0);
	}

	//
	// display SIVB stage propellant level in generic HUD
	//

	SetDefaultPropellantResource (ph_3rd);

	//
	// *********************** thruster definitions ********************************
	//

	VECTOR3 m_exhaust_pos1= {0,0,-9.-STG1O+10};

	//
	// orbiter main thrusters
	//

	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC, ph_3rd, ISP_SECOND_VAC, ISP_SECOND_SL);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);
	AddExhaust (th_main[0], 30.0, 2.9 ,J2Tex);

	//
	//  Ullage rockets (3)
	//

	VECTOR3	m_exhaust_pos6= _V(3.27,0.46,-2-STG1O+9);
	VECTOR3 m_exhaust_pos7= _V(-1.65,2.86,-2-STG1O+9);
	VECTOR3	m_exhaust_pos8= _V(-1.65,-2.86,-2-STG1O+9);

	int i;

	//
	// Ullage rocket thrust and ISP is a guess for now.
	//

	th_ver[0] = CreateThruster (m_exhaust_pos6, _V( -0.45,0.0,1), 10000, ph_ullage3, 3000);
	th_ver[1] = CreateThruster (m_exhaust_pos7, _V( 0.23,-0.39,1), 10000, ph_ullage3, 3000);
	th_ver[2] = CreateThruster (m_exhaust_pos8, _V( 0.23,0.39,1), 10000, ph_ullage3, 3000);

	for (i = 0; i < 3; i++) {
		AddExhaust(th_ver[i], 11.0, 0.25, exhaust_tex);
		AddExhaustStream(th_ver[i], &solid_exhaust);
	}
	thg_ver = CreateThrusterGroup (th_ver, 3,THGROUP_USER);

	//
	// Give the AGC our new stats.
	//

	agc.SetVesselStats(ISP_SECOND_VAC, THRUST_SECOND_VAC, false);
	iu.SetVesselStats(ISP_SECOND_VAC, THRUST_SECOND_VAC);
}

//
// Is this used anymore?
//

void Saturn1b::SetASTPStage ()
{
	ClearThrusterDefinitions();
	UINT meshidx;
	SetSize (4.0);
	SetCOG_elev (3.5);
	SetEmptyMass (19318);
	SetEngineLevel(ENGINE_MAIN, 0.0);
	SetPMI (_V(15,15,9.2));
	SetCrossSections (_V(40,40,14));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.3));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
	ClearExhaustRefs();
    ClearAttExhaustRefs();

	AddSM(30.25-12.25-21.5, true);

	VECTOR3 mesh_dir;

	//
	// Skylab SM and Apollo 7 have no HGA.
	//
	if (!NoHGA) {
		mesh_dir=_V(-2.2,-1.7,28.82-12.25-21.5);
		AddMesh (hSMhga, &mesh_dir);
	}

	mesh_dir=_V(0,0,34.4-12.25-21.5);
	meshidx = AddMesh (hCM, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		cmpidx = AddMesh (hCMP, &mesh_dir);
		crewidx = AddMesh (hCREW, &mesh_dir);
		SetCrewMesh();
	} else {
		cmpidx = -1;
		crewidx = -1;
	}

	meshidx = AddMesh (hCMInt, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	//Don't Forget the Hatch
	meshidx = AddMesh (hFHC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	if (HasProbe) {
		probeidx = AddMesh(hprobe, &mesh_dir);
		probeextidx = AddMesh(hprobeext, &mesh_dir);
		SetDockingProbeMesh();
	} else {
		probeidx = -1;
		probeextidx = -1;
	}

	mesh_dir=_V(0.0,-0.2,37.40-12.25-21.5);
	meshidx = AddMesh (hastp, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	VECTOR3 dockpos = {0,0,35.90-12.25-21.5+2.8};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams (dockpos, dockdir, dockrot);

	dockstate=3;

	SetView();
	AddRCSJets(-2.30,995);

    // ************************* propellant specs **********************************
	if (!ph_sps)  ph_sps  = CreatePropellantResource(20500,10500); //SPS stage Propellant
	SetDefaultPropellantResource (ph_sps); // display SPS stage propellant level in generic HUD
	if (!ph_rcs0)  ph_rcs0  = CreatePropellantResource(500); //RCS stage Propellant

	// *********************** thruster definitions ********************************

	VECTOR3 m_exhaust_pos1= {0,0,-8.-STG1O};
	// orbiter main thrusters
	th_main[0] = CreateThruster (_V(0,0,-5), _V(0,0,1), SPS_THRUST, ph_sps, SPS_ISP);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);

	AddExhaust (th_main[0], 20.0, 2.25, SMExhaustTex);

	ActivateASTP = true;
}

void Saturn1b::DockStage (UINT dockstatus)
{
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;
	VESSELSTATUS vs3;
	VESSELSTATUS vs4;
	VESSELSTATUS vs5;
	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);
	VECTOR3 ofs3 = _V(0,0,0);
	VECTOR3 ofs4 = _V(0,0,0);
	VECTOR3 ofs5 = _V(0,0,0);
	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);
	VECTOR3 vel3 = _V(0,0,0);
	VECTOR3 vel4 = _V(0,0,0);
	VECTOR3 vel5 = _V(0,0,0);
	SetEngineLevel(ENGINE_MAIN, 0);
	GetStatus (vs1);
	GetStatus (vs2);
	GetStatus (vs3);
	GetStatus (vs4);
	GetStatus (vs5);
	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;
	vs3.eng_main = vs3.eng_hovr = 0.0;
	vs4.eng_main = vs4.eng_hovr = 0.0;
	vs5.eng_main = vs5.eng_hovr = 0.0;

	if (dockstatus == 0)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0,0,-4.0);
		ofs2 = OFS_TOWER;
		vel2 = _V(2.0,2.0,12.0);
	}
	else if (dockstatus == 1)
	{
	 	ofs1 = OFS_STAGE2;
		vel1 = _V(0,0,-0.235);
		ofs2 = OFS_STAGE21;
		vel2 = _V(0.5,0.5,-0.55);
		ofs3 = OFS_STAGE22;
		vel3 = _V(-0.5,0.5,-0.55);
		ofs4 = OFS_STAGE23;
		vel4 = _V(0.5,-0.5,-0.55);
		ofs5 = OFS_STAGE24;
		vel5 = _V(-0.5,-0.5,-0.55);
	}
	else if (dockstatus == 2||dockstatus == 6)
	{
	 	ofs1 = RelPos;
		vel1 = _V(0,0,-0.5);
	}

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};
	VECTOR3 rofs3, rvel3 = {vs3.rvel.x, vs3.rvel.y, vs3.rvel.z};
	VECTOR3 rofs4, rvel4 = {vs4.rvel.x, vs4.rvel.y, vs4.rvel.z};
	VECTOR3 rofs5, rvel5 = {vs5.rvel.x, vs5.rvel.y, vs5.rvel.z};
	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);
	Local2Rel (ofs3, vs3.rpos);
	Local2Rel (ofs4, vs4.rpos);
	Local2Rel (ofs5, vs5.rpos);
	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);
	GlobalRot (vel3, rofs3);
	GlobalRot (vel4, rofs4);
	GlobalRot (vel5, rofs5);
	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;
	vs3.rvel.x = rvel3.x+rofs3.x;
	vs3.rvel.y = rvel3.y+rofs3.y;
	vs3.rvel.z = rvel3.z+rofs3.z;
	vs4.rvel.x = rvel4.x+rofs4.x;
	vs4.rvel.y = rvel4.y+rofs4.y;
	vs4.rvel.z = rvel4.z+rofs4.z;
	vs5.rvel.x = rvel5.x+rofs5.x;
	vs5.rvel.y = rvel5.y+rofs5.y;
	vs5.rvel.z = rvel5.z+rofs5.z;
	VESSELSTATUS vs4b;
	VECTOR3 ofs = _V(0,0,0);
	VECTOR3 vel = _V(0,0,0.6);

   switch (dockstatus)	
   {
   case 2:
	   {
			//
			//Interface initialization for mesh modification to SIVB
			//
			Undock(0);

			//
			// Tell the S4b that we've removed the payload.
			//

			SIVBSettings S4Config;
			SIVB *SIVBVessel;

			S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD = 1;

			S4Config.Payload = PAYLOAD_EMPTY;

			SIVBVessel = (SIVB *) oapiGetVesselInterface(hs4bM);
			SIVBVessel->SetState(S4Config);

			//
			//Time to hear the Stage separation
			//

			SMJetS.play();

			//
			//Now Lets reconfigure Apollo for the DM.
			//

			if (ASTPMission)
				SetASTPStage ();
			dockstate = 3;
			bManualUnDock = false;
			SetAttitudeLinLevel(2,-1);
		}
	   	break;

   case 3:
		if(bManualUnDock) {

			//DM Jetison preparation
			char VName2[256];
			ofs = OFS_DOCKING2;
			GetStatus (vs4b);
			vs4b.eng_main = vs4b.eng_hovr = 0.0;
			SetEngineLevel(ENGINE_MAIN, 0);
			StageTransform(this,&vs4b,ofs,vel);
			vs4b.status=0;
			vs4b.vrot.x = 0.0;
			vs4b.vrot.y = 0.0;
			vs4b.vrot.z = 0.0;
			if(ASTPMission){
				GetApolloName(VName2); strcat (VName2, "-ASTPDM");
				VESSEL::Create (VName2, "ProjectApollo/nSat1astp2", vs4b);
				hAstpDM=oapiGetVesselByName(VName2);
			}

			/// \todo Fix that...
			if (ProbeJetison){
				/// \todo SetCSM2Stage is buggy
				SetCSMStage();
				// SetCSM2Stage ();
				StageS.play();
				bManualUnDock= false;
				dockstate=4;
				ProbeJetison=false;
				break;
			}
			else{
				ShiftCentreOfMass (_V(0,0,-21.5));
				SetCSMStage ();
				SMJetS.play();
				dockstate=4;
				bManualUnDock= false;
				break;
			}
		}
		break;
   case 4:
	   //vessel->Undock(0);
	   if (hAstpDM){
		//
		   	SetASTPStage ();
			DestroyAstp=true;
	   }
	   break;
	case 5:
	   break;
	}
}

void Saturn1b::SeparateStage (int new_stage)

{
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;

	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);

	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);

	GetStatus (vs1);
	GetStatus (vs2);

	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;

	if (stage == LAUNCH_STAGE_ONE && !bAbort)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_ONE && bAbort)
	{
		ofs1= OFS_ABORT;
		vel1 = _V(0,0,-4.0);
	}

	if ((stage == LAUNCH_STAGE_TWO || stage == CSM_ABORT_STAGE) && !bAbort)
	{
		ofs1 = OFS_TOWER;
		vel1 = _V(15.0,15.0,106.0);
	}

	if (stage == LAUNCH_STAGE_TWO && bAbort)
	{
		ofs1= OFS_ABORT2;
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)
	{
	 	ofs1 = _V(0, 0, 1.7);
		vel1 = _V(0, 0, 0);
	}

	if (stage == CSM_LEM_STAGE)
	{
	 	ofs1 = OFS_SM;
		vel1 = _V(0, 0, -0.1);
		ofs2 = OFS_DOCKING;
		vel2 = _V(0.0, 0.0, 0.3);

	}

	if (stage == CM_STAGE)
	{
		ofs1 = OFS_CM_CONE;
		vel1 = _V(1.0,1.0,1.0);
	}

	if (stage == CSM_ABORT_STAGE)
	{
		ofs1 = OFS_ABORT_TOWER;
		vel1 = _V(15.0,15.0,50.0);
	}

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};

	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);

	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);

	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;

	if (stage == CM_STAGE)
	{
		if (GetAtmPressure()>35000)
		{
		}
		SetChuteStage1 ();
	}

    if (stage == LAUNCH_STAGE_ONE && !bAbort )
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;
		StageS.play();

		//
		// Create S1b stage and set it up.
		//

		if (hstg1) {
			S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
			stage1->DefSetState(&vs1);

			S1BSettings S1Config;

			S1Config.SettingsType.S1B_SETTINGS_ENGINES = 1;
			S1Config.SettingsType.S1B_SETTINGS_FUEL = 1;
			S1Config.SettingsType.S1B_SETTINGS_GENERAL = 1;
			S1Config.SettingsType.S1B_SETTINGS_MASS = 1;

			S1Config.EngineNum = 8;
			S1Config.RetroNum = 4;
			S1Config.EmptyMass = SI_EmptyMass;
			S1Config.MainFuelKg = GetPropellantMass(ph_1st);
			S1Config.MissionTime = MissionTime;
			S1Config.Realism = Realism;
			S1Config.VehicleNo = VehicleNo;
			S1Config.ISP_FIRST_SL = ISP_FIRST_SL;
			S1Config.ISP_FIRST_VAC = ISP_FIRST_VAC;
			S1Config.THRUST_FIRST_VAC = THRUST_FIRST_VAC;
			S1Config.CurrentThrust = GetThrusterLevel(th_main[0]);
			S1Config.LowRes = LowRes;

			stage1->SetState(S1Config);
		}

		SetSecondStage ();
		SetSecondStageEngines ();

		ShiftCentreOfMass (_V(0,0,12.25));

		SetThrusterGroupLevel(thg_ver,1.0);
	}

	if (stage == LAUNCH_STAGE_TWO && !bAbort )
	{
		if (SaturnHasCSM())
		{
			JettisonLET();
		}

		TowerJS.done();
		SetSecondStage ();
		AddRCS_S4B();
	}

	if (stage == LAUNCH_STAGE_SIVB)
	{
		AddRCS_S4B();
	}

	if (stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)
	{
		char VName[256]="";
		SIVB *SIVBVessel;

		GetApolloName(VName); strcat (VName, "-S4BSTG");
		hs4bM = oapiCreateVessel(VName, "ProjectApollo/nsat1stg2", vs1);

		SIVBSettings S4Config;

		//
		// For now we'll only seperate the panels on ASTP.
		//

		S4Config.SettingsType.SIVB_SETTINGS_FUEL = 1;
		S4Config.SettingsType.SIVB_SETTINGS_GENERAL = 1;
		S4Config.SettingsType.SIVB_SETTINGS_MASS = 1;
		S4Config.SettingsType.SIVB_SETTINGS_PAYLOAD = 1;
		S4Config.Payload = SIVBPayload;
		S4Config.PanelsHinged = !SLAWillSeparate;
		S4Config.VehicleNo = VehicleNo;
		S4Config.EmptyMass = SII_EmptyMass;
		S4Config.MainFuelKg = GetPropellantMass(ph_3rd);
		S4Config.PayloadMass = S4PL_Mass;
		S4Config.SaturnVStage = false;
		S4Config.MissionTime = MissionTime;
		S4Config.Realism = Realism;
		S4Config.LowRes = LowRes;
		S4Config.SLARotationLimit = (double) SLARotationLimit;

		SIVBVessel = (SIVB *) oapiGetVesselInterface(hs4bM);
		SIVBVessel->SetState(S4Config);

		SeparationS.play();

		SetCSMStage();
		ShiftCentreOfMass(_V(0, 0, 20.8));
		SeparationSpeed = 0.15;

		if(ASTPMission)
			dockstate = 1;
		else
			dockstate = 4;
	}

	if (stage == CSM_LEM_STAGE)
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		SMJetS.play();
		SMJetS.done();
		if (HasProbe && dockstate != 5) {	/// \todo No clue what dockstate means...
			VECTOR3 ofs = OFS_DOCKING2;
			VECTOR3 vel = {0.0,0.0,0.1};
			VESSELSTATUS vs4b;
			GetStatus (vs4b);
			StageTransform(this, &vs4b,ofs,vel);
			vs4b.vrot.x = 0.0;
			vs4b.vrot.y = 0.0;
			vs4b.vrot.z = 0.0;
			GetApolloName(VName); strcat (VName, "-DCKPRB");
			hPROBE = oapiCreateVessel(VName, "ProjectApollo/CMprobe", vs4b);
			HasProbe = false;

		}
		GetApolloName(VName); strcat (VName, "-SM");
		hSMJet = oapiCreateVessel(VName, "ProjectApollo/SM", vs1);

		SMSettings SMConfig;

		SMConfig.SettingsType.SM_SETTINGS_ENGINES = 1;
		SMConfig.SettingsType.SM_SETTINGS_FUEL = 1;
		SMConfig.SettingsType.SM_SETTINGS_GENERAL = 1;
		SMConfig.SettingsType.SM_SETTINGS_MASS = 1;

		SMConfig.EmptyMass = SM_EmptyMass;
		SMConfig.MainFuelKg = GetPropellantMass(ph_sps);
		SMConfig.RCSAFuelKg = GetPropellantMass(ph_rcs0);
		SMConfig.RCSBFuelKg = GetPropellantMass(ph_rcs1);
		SMConfig.RCSCFuelKg = GetPropellantMass(ph_rcs2);
		SMConfig.RCSDFuelKg = GetPropellantMass(ph_rcs3);
		SMConfig.MissionTime = MissionTime;
		SMConfig.Realism = Realism;
		SMConfig.VehicleNo = VehicleNo;
		SMConfig.LowRes = LowRes;
		SMConfig.showHGA = !NoHGA;
		SMConfig.A13Exploded = ApolloExploded;

		SM *SMVessel = (SM *) oapiGetVesselInterface(hSMJet);
		SMVessel->SetState(SMConfig);

		//
		// Tell AGC the CM has seperated from the SM.
		//

		agc.SetInputChannelBit(030, 2, true);

		SetReentryStage ();
	}

	if (stage == CM_STAGE)
	{
		SetChuteStage2 ();
	}

	if (stage == CM_ENTRY_STAGE_TWO)
	{
		SetChuteStage3 ();
	}

	if (stage == CM_ENTRY_STAGE_FOUR)
	{
		SetChuteStage4 ();
	}

	if (stage == CM_ENTRY_STAGE_FIVE)
	{
		SetSplashStage ();
	}

	if (stage == LAUNCH_STAGE_ONE && bAbort )
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel ("Saturn_Abort", "ProjectApollo/Saturn1bAbort1", vs1);
		SetAbortStage ();
	}

	if (stage == LAUNCH_STAGE_TWO && bAbort )
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel ("Saturn_Abort", "ProjectApollo/Saturn1bAbort2", vs1);
		SetAbortStage ();
	}

	if (stage == CSM_ABORT_STAGE)
	{
		JettisonLET();

		SetStage(CM_ENTRY_STAGE);
		SetReentryStage ();
		ActivateNavmode(NAVMODE_KILLROT);
	}
}

//
// Load the meshes once per DLL.
//

void Saturn1bLoadMeshes()

{
	hSat1stg1 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg1");
	hSat1stg1low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg1");
	hSat1intstg = oapiLoadMeshGlobal ("ProjectApollo/nsat1intstg");
	hSat1intstglow = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1intstg");
	hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
	hSat1stg2low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg2");

	hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
	hastp = oapiLoadMeshGlobal ("ProjectApollo/nASTP3");
	hastp2 = oapiLoadMeshGlobal ("ProjectApollo/nASTP2");
	hCOAStarget = oapiLoadMeshGlobal ("ProjectApollo/sat_target");
	hNosecap = oapiLoadMeshGlobal ("ProjectApollo/nsat1aerocap");

	exhaust_tex = oapiRegisterExhaustTexture ("ProjectApollo/Exhaust2");
}

//
// Update per-vessel handles to the appropriate low-res or high-res meshes.
//

void Saturn1b::SetupMeshes()

{
	if (LowRes) {
		hStage1Mesh = hSat1stg1low;
		hStage2Mesh = hSat1stg2low;
		hInterstageMesh = hSat1intstglow;
		hStageSLA1Mesh = hSat1stg21;
		hStageSLA2Mesh = hSat1stg22;
		hStageSLA3Mesh = hSat1stg23;
		hStageSLA4Mesh = hSat1stg24;
	}
	else {
		hStage1Mesh = hSat1stg1;
		hStage2Mesh = hSat1stg2;
		hInterstageMesh = hSat1intstg;
		hStageSLA1Mesh = hSat1stg21;
		hStageSLA2Mesh = hSat1stg22;
		hStageSLA3Mesh = hSat1stg23;
		hStageSLA4Mesh = hSat1stg24;
	}
}

void Saturn1b::CreateStageOne() {

	// Create hidden SIB vessel
	char VName[256];
	VESSELSTATUS vs;

	GetStatus(vs);
	strcpy (VName, GetName()); 
	strcat (VName, "-STG1");
	hstg1 = oapiCreateVessel(VName,"ProjectApollo/nsat1stg1", vs);

	// Load only the necessary meshes
	S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
	stage1->LoadMeshes(LowRes);
}
