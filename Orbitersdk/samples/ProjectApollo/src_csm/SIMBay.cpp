/****************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2025

  CSM SIMBay instrumentation

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

  **************************************************************************/

  // To force Orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include <winsock.h> // TODO: Replace with winsock2 after yaAGC updates
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "nassputils.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "CSMcomputer.h"
#include "saturn.h"
#include "LEM.h"
#include "ioChannels.h"
#include "tracer.h"
#include "Mission.h"
#include "papi.h"

using namespace nassp;

SIMBay::SIMBay() {
	sat = NULL;

	//CSM 112&114 Common animations
	MappingCameraCoverAnim = -1;
	MappingCameraCoverAnimState = 0;
	PanoramicCameraAnim = -1;
	PanoramicCameraAnimState = 0;
	MappingCameraAnim = -1;
	MappingCameraAnimState = 0;

	//CSM 112 Specific Animations
	GammaBayAnim = -1;
	GammaBayAnimState = 0;
	GammaBayJettAnim = -1;
	GammaBayJettAnimState = 0;
	MassSpectrometerAnim = -1;
	MassSpectrometerAnimState = 0;
	MassSpectrometerJettAnim = -1;
	MassSpectrometerJettAnimState = 0;
	SubSatCoverAnim = -1;
	SubSatCoverAnimState = 0;
	XRayCoverAnim = -1;
	XRayCoverAnimState = 0;

	//CSM 114 Specific Animations
	IRCoverAnim = -1;
	IRCoverAnimState = 0;
	UVCoverAnim = -1;
	UVCoverAnimState = 0;
}

void SIMBay::Init(Saturn* vessel) {
	sat = vessel;
}

void SIMBay::DefineAnimations(UINT idx)
{
	if (sat->pMission->GetPanel230Version() == 1 || sat->pMission->GetPanel230Version() == 2)
	{
		///CSM112-114 Common Animations:
		///Mapping Camera Cover
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp16DeployedXfr1;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp16DeployedYfr1;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp16DeployedZfr1;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp16DeployedXfr2;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp16DeployedYfr2;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp16DeployedZfr2;
		static UINT SIMBAYGrp16[1] = { 14 };
		const VECTOR3 COVER_PIVOT = { 1.08675, 1.51867, 1.29618 };    //Cover Pivot Point
		MGROUP_ROTATE* mgr_SIMBAYGrp16Xfr1 = new MGROUP_ROTATE(idx, SIMBAYGrp16, 1, COVER_PIVOT, _V(1, 0, 0), (float)(RAD * -43.1));
		MGROUP_ROTATE* mgr_SIMBAYGrp16Yfr1 = new MGROUP_ROTATE(idx, SIMBAYGrp16, 1, COVER_PIVOT, _V(0, 1, 0), (float)(RAD * 26));
		MGROUP_ROTATE* mgr_SIMBAYGrp16Zfr1 = new MGROUP_ROTATE(idx, SIMBAYGrp16, 1, COVER_PIVOT, _V(0, 0, 1), (float)(RAD * 7.92));
		MGROUP_ROTATE* mgr_SIMBAYGrp16Xfr2 = new MGROUP_ROTATE(idx, SIMBAYGrp16, 1, COVER_PIVOT, _V(1, 0, 0), (float)(RAD * -31.5));
		MGROUP_ROTATE* mgr_SIMBAYGrp16Yfr2 = new MGROUP_ROTATE(idx, SIMBAYGrp16, 1, COVER_PIVOT, _V(0, 1, 0), (float)(RAD * 23.6));
		MGROUP_ROTATE* mgr_SIMBAYGrp16Zfr2 = new MGROUP_ROTATE(idx, SIMBAYGrp16, 1, COVER_PIVOT, _V(0, 0, 1), (float)(RAD * -5));
		MappingCameraCoverAnim = sat->CreateAnimation(0.0);
		ach_SIMBAYGrp16DeployedXfr1 = sat->AddAnimationComponent(MappingCameraCoverAnim, 0.0, 0.5, mgr_SIMBAYGrp16Yfr1);
		ach_SIMBAYGrp16DeployedZfr1 = sat->AddAnimationComponent(MappingCameraCoverAnim, 0.0, 0.5, mgr_SIMBAYGrp16Zfr1);
		ach_SIMBAYGrp16DeployedYfr1 = sat->AddAnimationComponent(MappingCameraCoverAnim, 0.0, 0.5, mgr_SIMBAYGrp16Xfr1);
		ach_SIMBAYGrp16DeployedXfr2 = sat->AddAnimationComponent(MappingCameraCoverAnim, 0.5, 1.0, mgr_SIMBAYGrp16Xfr2);
		ach_SIMBAYGrp16DeployedZfr2 = sat->AddAnimationComponent(MappingCameraCoverAnim, 0.5, 1.0, mgr_SIMBAYGrp16Yfr2);
		ach_SIMBAYGrp16DeployedYfr2 = sat->AddAnimationComponent(MappingCameraCoverAnim, 0.5, 1.0, mgr_SIMBAYGrp16Zfr2);

		//Mapping Camera extend
		ANIMATIONCOMPONENT_HANDLE ach_MappingGrp;
		ANIMATIONCOMPONENT_HANDLE ach_ShadeGrp;
		static UINT MappingGrp[9] = { 1,2,3,4,14,18,19,20,21 };
		static UINT ShadeGrp[1] = { 21 };
		MGROUP_TRANSLATE* mgt_MappingGrp = new MGROUP_TRANSLATE(idx, MappingGrp, 9, _V(0.161347, 0.234723, 0));
		MGROUP_TRANSLATE* mgt_ShadeGrp = new MGROUP_TRANSLATE(idx, ShadeGrp, 1, _V(-0.119963, 0.084116, 0));
		MappingCameraAnim = sat->CreateAnimation(0.0);
		ach_MappingGrp = sat->AddAnimationComponent(MappingCameraAnim, 0.0, 0.5, mgt_MappingGrp);
		ach_ShadeGrp = sat->AddAnimationComponent(MappingCameraAnim, 0.5, 1.0, mgt_ShadeGrp);

		//Panoramic Camera
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp9and10Z;
		static UINT SIMBAYGrp9and10[2] = { 9,10 };
		const VECTOR3 PAN_PIVOT = { 1.17471, 1.08905, 0.934385 };    //Camera Pivot Point
		MGROUP_ROTATE* mgr_SIMBAYGrp9and10Z = new MGROUP_ROTATE(idx, SIMBAYGrp9and10, 2, PAN_PIVOT, _V(0, 0, 1), (float)(RAD * 141));
		PanoramicCameraAnim = sat->CreateAnimation(0.0);
		ach_SIMBAYGrp9and10Z = sat->AddAnimationComponent(PanoramicCameraAnim, 0.0, 1.0, mgr_SIMBAYGrp9and10Z);

	}

	///CSM 112 Specific Animations
	if (sat->pMission->GetPanel230Version() == 1)
	{
		//Gamma Bay deploy/retract
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayCoverGrp23;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp32;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp33;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp34;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp35;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp36;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp37;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp38;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBayTubeGrp39;
		ANIMATIONCOMPONENT_HANDLE ach_GammaBaySpectGrp22;
		static UINT GammaBayCoverGrp23[1] = { 23 };
		static UINT GammaBayTubeGrp32[1] = { 32 };
		static UINT GammaBayTubeGrp33[1] = { 33 };
		static UINT GammaBayTubeGrp34[1] = { 34 };
		static UINT GammaBayTubeGrp35[1] = { 35 };
		static UINT GammaBayTubeGrp36[1] = { 36 };
		static UINT GammaBayTubeGrp37[1] = { 37 };
		static UINT GammaBayTubeGrp38[1] = { 38 };
		static UINT GammaBayTubeGrp39[1] = { 39 };
		static UINT GammaBaySpectGrp22[1] = { 22 };
		const VECTOR3 GAMMA_COVER_PIVOT = { 0.99671, 1.41535, 0.788007 };    //Cover Pivot Point
		MGROUP_ROTATE* mgr_GammaBayCoverGrp23 = new MGROUP_ROTATE(idx, GammaBayCoverGrp23, 1, GAMMA_COVER_PIVOT, _V(0, 0, 1), (float)(RAD * -146));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp32 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp32, 1, _V(0.40379, 0.522067, 0));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp33 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp33, 1, _V(0.80758, 1.04413, 0));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp34 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp34, 1, _V(1.21137, 1.5662, 0));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp35 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp35, 1, _V(1.61516, 2.08827, 0));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp36 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp36, 1, _V(2.01895, 2.61033, 0));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp37 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp37, 1, _V(2.42274, 3.1324, 0));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp38 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp38, 1, _V(2.82653, 3.65447, 0));
		MGROUP_TRANSLATE* mgt_GammaBayTubeGrp39 = new MGROUP_TRANSLATE(idx, GammaBayTubeGrp39, 1, _V(3.23032, 4.17653, 0));
		MGROUP_TRANSLATE* mgt_GammaBaySpectGrp22 = new MGROUP_TRANSLATE(idx, GammaBaySpectGrp22, 1, _V(3.23032, 4.17653, 0));
		GammaBayAnim = sat->CreateAnimation(0.0);
		ach_GammaBayCoverGrp23 = sat->AddAnimationComponent(GammaBayAnim, 0.0, 0.01875, mgr_GammaBayCoverGrp23);
		ach_GammaBayTubeGrp32 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp32);
		ach_GammaBayTubeGrp33 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp33);
		ach_GammaBayTubeGrp34 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp34);
		ach_GammaBayTubeGrp35 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp35);
		ach_GammaBayTubeGrp36 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp36);
		ach_GammaBayTubeGrp37 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp37);
		ach_GammaBayTubeGrp38 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp38);
		ach_GammaBayTubeGrp39 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBayTubeGrp39);
		ach_GammaBaySpectGrp22 = sat->AddAnimationComponent(GammaBayAnim, 0.01875, 1.0, mgt_GammaBaySpectGrp22);

		//Gamma Bay spectrometer Jett
		ANIMATIONCOMPONENT_HANDLE ach_GammaBaySpectAllGrp;
		static UINT GammaBaySpectAllGrp[10] = { 32, 33, 34, 35, 36, 37, 38, 39, 40, 22 };
		MGROUP_SCALE* mgs_GammaBaySpectAllGrp = new MGROUP_SCALE(idx, GammaBaySpectAllGrp, 10, _V(0.47427, 0.982481, 0.783395), _V(0.00001, 0.00001, 0.00001));
		GammaBayJettAnim = sat->CreateAnimation(0.0);
		ach_GammaBaySpectAllGrp = sat->AddAnimationComponent(GammaBayJettAnim, 0.0, 1.0, mgs_GammaBaySpectAllGrp);

		//Mass Spectrometer deploy/retract
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecCoverGrp25Xfr1;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecCoverGrp25Yfr1;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecCoverGrp25Zfr1;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecCoverGrp25Xfr2;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecCoverGrp25Yfr2;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecCoverGrp25Zfr2;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp41;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp42;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp43;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp44;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp45;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp46;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp47;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecTubeGrp48;
		ANIMATIONCOMPONENT_HANDLE ach_MassSpecGrp26;
		static UINT MassSpecCoverGrp25[1] = { 25 };
		static UINT MassSpecTubeGrp41[1] = { 41 };
		static UINT MassSpecTubeGrp42[1] = { 42 };
		static UINT MassSpecTubeGrp43[1] = { 43 };
		static UINT MassSpecTubeGrp44[1] = { 44 };
		static UINT MassSpecTubeGrp45[1] = { 45 };
		static UINT MassSpecTubeGrp46[1] = { 46 };
		static UINT MassSpecTubeGrp47[1] = { 47 };
		static UINT MassSpecTubeGrp48[1] = { 48 };
		static UINT MassSpecGrp26[1] = { 26 };
		const VECTOR3 MASS_COVER_PIVOT = { 1.54236, 1.05401, -0.653989 };    //Cover Pivot Point
		MGROUP_ROTATE* mgr_MassSpecCoverGrp25Xfr1 = new MGROUP_ROTATE(idx, MassSpecCoverGrp25, 1, MASS_COVER_PIVOT, _V(1, 0, 0), (float)(RAD * 14.7469));
		MGROUP_ROTATE* mgr_MassSpecCoverGrp25Zfr1 = new MGROUP_ROTATE(idx, MassSpecCoverGrp25, 1, MASS_COVER_PIVOT, _V(0, 0, 1), (float)(RAD * 11.8591));
		MGROUP_ROTATE* mgr_MassSpecCoverGrp25Yfr1 = new MGROUP_ROTATE(idx, MassSpecCoverGrp25, 1, MASS_COVER_PIVOT, _V(0, 1, 0), (float)(RAD * -38.5816));

		MGROUP_ROTATE* mgr_MassSpecCoverGrp25Xfr2 = new MGROUP_ROTATE(idx, MassSpecCoverGrp25, 1, MASS_COVER_PIVOT, _V(1, 0, 0), (float)(RAD * 28.7469));
		MGROUP_ROTATE* mgr_MassSpecCoverGrp25Zfr2 = new MGROUP_ROTATE(idx, MassSpecCoverGrp25, 1, MASS_COVER_PIVOT, _V(0, 0, 1), (float)(RAD * 11.8591));
		MGROUP_ROTATE* mgr_MassSpecCoverGrp25Yfr2 = new MGROUP_ROTATE(idx, MassSpecCoverGrp25, 1, MASS_COVER_PIVOT, _V(0, 1, 0), (float)(RAD * -39.5816));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp41 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp41, 1, _V(0.588691, 0.298402, 0));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp42 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp42, 1, _V(1.17738, 0.596803, 0));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp43 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp43, 1, _V(1.76607, 0.895205, 0));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp44 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp44, 1, _V(2.35476, 1.19361, 0));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp45 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp45, 1, _V(2.94345, 1.49201, 0));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp46 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp46, 1, _V(3.53214, 1.79041, 0));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp47 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp47, 1, _V(4.12083, 2.08881, 0));
		MGROUP_TRANSLATE* mgt_MassSpecTubeGrp48 = new MGROUP_TRANSLATE(idx, MassSpecTubeGrp48, 1, _V(4.70952, 2.38721, 0));
		MGROUP_TRANSLATE* mgt_MassSpecGrp26 = new MGROUP_TRANSLATE(idx, MassSpecGrp26, 1, _V(4.70952, 2.38721, 0));
		MassSpectrometerAnim = sat->CreateAnimation(0.0);
		ach_MassSpecCoverGrp25Xfr1 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.0, 0.01875, mgr_MassSpecCoverGrp25Yfr1);
		ach_MassSpecCoverGrp25Yfr1 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.0, 0.01875, mgr_MassSpecCoverGrp25Zfr1);
		ach_MassSpecCoverGrp25Zfr1 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.0, 0.01875, mgr_MassSpecCoverGrp25Xfr1);

		ach_MassSpecCoverGrp25Xfr2 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.0, 0.01875, mgr_MassSpecCoverGrp25Yfr2);
		ach_MassSpecCoverGrp25Yfr2 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.0, 0.01875, mgr_MassSpecCoverGrp25Zfr2);
		ach_MassSpecCoverGrp25Zfr2 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.0, 0.01875, mgr_MassSpecCoverGrp25Xfr2);
		ach_MassSpecTubeGrp41 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp41);
		ach_MassSpecTubeGrp42 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp42);
		ach_MassSpecTubeGrp43 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp43);
		ach_MassSpecTubeGrp44 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp44);
		ach_MassSpecTubeGrp45 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp45);
		ach_MassSpecTubeGrp46 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp46);
		ach_MassSpecTubeGrp47 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp47);
		ach_MassSpecTubeGrp48 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecTubeGrp48);
		ach_MassSpecGrp26 = sat->AddAnimationComponent(MassSpectrometerAnim, 0.01875, 1.0, mgt_MassSpecGrp26);

		//Mass spectrometer Jett
		ANIMATIONCOMPONENT_HANDLE ach_MassSpectAllGrp;
		static UINT MassSpectAllGrp[10] = { 11, 26, 41, 42, 43, 44, 45, 46, 47, 48 };
		MGROUP_SCALE* mgs_MassSpectAllGrp = new MGROUP_SCALE(idx, MassSpectAllGrp, 10, _V(0.827439, 0.71798, 0.790792), _V(0.00001, 0.00001, 0.00001));
		MassSpectrometerJettAnim = sat->CreateAnimation(0.0);
		ach_MassSpectAllGrp = sat->AddAnimationComponent(MassSpectrometerJettAnim, 0.0, 1.0, mgs_MassSpectAllGrp);

		//XRay Cover
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp30;
		static UINT SIMBAYGrp30[1] = { 30 };
		const VECTOR3 XRAY_COVER_PIVOT = { 0.700664, 1.654, -1.654 };    //Cover Pivot Point
		MGROUP_ROTATE* mgr_SIMBAYGrp30 = new MGROUP_ROTATE(idx, SIMBAYGrp30, 1, XRAY_COVER_PIVOT, _V(0, 0, 1), (float)(RAD * 128));
		XRayCoverAnim = sat->CreateAnimation(0.0);
		ach_SIMBAYGrp30 = sat->AddAnimationComponent(XRayCoverAnim, 0.0, 1.0, mgr_SIMBAYGrp30);

		//SubSatellite Launch
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp7and31X;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp7and31Z;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp7and31Y;
		static UINT SIMBAYGrp7and31[2] = { 7,31 };
		const VECTOR3 SUBSAT_COVER_PIVOT = { 0.735741, 1.74082, -0.619339 };    //Cover Pivot Point
		MGROUP_ROTATE* mgr_SIMBAYGrp7and31X = new MGROUP_ROTATE(idx, SIMBAYGrp7and31, 2, SUBSAT_COVER_PIVOT, _V(1, 0, 0), (float)(RAD * -90));
		MGROUP_ROTATE* mgr_SIMBAYGrp7and31Z = new MGROUP_ROTATE(idx, SIMBAYGrp7and31, 2, SUBSAT_COVER_PIVOT, _V(0, 0, 1), (float)(RAD * -17.4));
		MGROUP_ROTATE* mgr_SIMBAYGrp7and31Y = new MGROUP_ROTATE(idx, SIMBAYGrp7and31, 2, SUBSAT_COVER_PIVOT, _V(0, 1, 0), (float)(RAD * 18.5));
		SubSatCoverAnim = sat->CreateAnimation(0.0);
		ach_SIMBAYGrp7and31X = sat->AddAnimationComponent(SubSatCoverAnim, 0.0, 1.0, mgr_SIMBAYGrp7and31X);
		ach_SIMBAYGrp7and31Y = sat->AddAnimationComponent(SubSatCoverAnim, 0.0, 1.0, mgr_SIMBAYGrp7and31Y);
		ach_SIMBAYGrp7and31Z = sat->AddAnimationComponent(SubSatCoverAnim, 0.5, 1.0, mgr_SIMBAYGrp7and31Z);
	}

	///CSM 114 Specific Animations:
	if (sat->pMission->GetPanel230Version() == 2)
	{
		///IR Spect Cover
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp6DeployedXfr1;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp6DeployedYfr1;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp6DeployedZfr1;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp6DeployedXfr2;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp6DeployedYfr2;
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp6DeployedZfr2;
		static UINT SIMBAYGrp6[1] = { 6 };
		const VECTOR3 IR_COVER_PIVOT = { 1.06147, 1.3724, -0.769505 };    //Cover Pivot Point
		MGROUP_ROTATE* mgr_SIMBAYGrp6Xfr1 = new MGROUP_ROTATE(idx, SIMBAYGrp6, 1, IR_COVER_PIVOT, _V(1, 0, 0), (float)(RAD * 62.7));
		MGROUP_ROTATE* mgr_SIMBAYGrp6Yfr1 = new MGROUP_ROTATE(idx, SIMBAYGrp6, 1, IR_COVER_PIVOT, _V(0, 1, 0), (float)(RAD * -43.2));
		MGROUP_ROTATE* mgr_SIMBAYGrp6Zfr1 = new MGROUP_ROTATE(idx, SIMBAYGrp6, 1, IR_COVER_PIVOT, _V(0, 0, 1), (float)(RAD * 26.9));
		IRCoverAnim = sat->CreateAnimation(0.0);
		ach_SIMBAYGrp6DeployedXfr1 = sat->AddAnimationComponent(IRCoverAnim, 0.0, 1.0, mgr_SIMBAYGrp6Yfr1);
		ach_SIMBAYGrp6DeployedZfr1 = sat->AddAnimationComponent(IRCoverAnim, 0.0, 1.0, mgr_SIMBAYGrp6Zfr1);
		ach_SIMBAYGrp6DeployedYfr1 = sat->AddAnimationComponent(IRCoverAnim, 0.0, 1.0, mgr_SIMBAYGrp6Xfr1);

		///UV Spect Cover
		ANIMATIONCOMPONENT_HANDLE ach_SIMBAYGrp17DeployedZ;
		static UINT SIMBAYGrp17[1] = { 17 };
		const VECTOR3 UV_COVER_PIVOT = { 0.527138, 1.81048, -0.834598 };    //Cover Pivot Point
		MGROUP_ROTATE* mgr_SIMBAYGrp17Z = new MGROUP_ROTATE(idx, SIMBAYGrp17, 1, UV_COVER_PIVOT, _V(0, 0, 1), (float)(RAD * 128));
		UVCoverAnim = sat->CreateAnimation(0.0);
		ach_SIMBAYGrp17DeployedZ = sat->AddAnimationComponent(UVCoverAnim, 0.0, 1.0, mgr_SIMBAYGrp17Z);
	}
}


void SIMBay::SystemTimestep(double simdt)
{
	double speed = 0.33; // anim duration: 3 sec
	double mappingspeed = 1.0 / 80; // mapping camera extend anim duration: 1m 20sec
	double spectspeed = 1.0 / 160; // spectrometers deploy anim duration: 2m 40sec

	if (sat->pMission->GetPanel230Version() == 1 || sat->pMission->GetPanel230Version() == 2)
	{
		//CSM 112&114 Common Animations
		//MappingCameraCover Deploying
		if (sat->MappingCameraCoverDeployed)
		{
			MappingCameraCoverAnimState += simdt * speed;
			if (MappingCameraCoverAnimState > 1.0)
				MappingCameraCoverAnimState = 1.0;
		}
		else
		{
			MappingCameraCoverAnimState -= simdt * speed;
			if (MappingCameraCoverAnimState < 0.0)
				MappingCameraCoverAnimState = 0.0;
		}
		sat->SetAnimation(MappingCameraCoverAnim, MappingCameraCoverAnimState);

		//Panoramic Camera
		if (sat->PanoramicCameraON)
		{
			PanoramicCameraAnimState += simdt * speed;
			if (PanoramicCameraAnimState > 1.0)
				PanoramicCameraAnimState = 1.0;
		}
		else
		{
			PanoramicCameraAnimState -= simdt * speed;
			if (PanoramicCameraAnimState < 0.0)
				PanoramicCameraAnimState = 0.0;
		}
		sat->SetAnimation(PanoramicCameraAnim, PanoramicCameraAnimState);

		//Mapping Camera Extend
		if (sat->MappingCameraExtended)
		{
			MappingCameraAnimState += simdt * mappingspeed;
			if (MappingCameraAnimState > 1.0)
				MappingCameraAnimState = 1.0;
		}
		else
		{
			MappingCameraAnimState -= simdt * mappingspeed;
			if (MappingCameraAnimState < 0.0)
				MappingCameraAnimState = 0.0;
		}
		sat->SetAnimation(MappingCameraAnim, MappingCameraAnimState);

		if (MappingCameraAnimState < 1)
		{
			sat->MappingCamera2Indicator.SetState(0);

			if (MappingCameraAnimState == 0)
			{
				sat->MappingCamera2Indicator.SetState(1);
			}
		}
	}

	if (sat->pMission->GetPanel230Version() == 1)
	{
		//CSM 112 Specific Animations
		//Gamma Bay animaton
		if (sat->GammaBayDeployed)
		{
			GammaBayAnimState += simdt * spectspeed;
			if (GammaBayAnimState > 1.0)
				GammaBayAnimState = 1.0;
		}
		else
		{
			GammaBayAnimState -= simdt * spectspeed;
			if (GammaBayAnimState < 0.0)
				GammaBayAnimState = 0.0;
		}
		sat->SetAnimation(GammaBayAnim, GammaBayAnimState);

		if (GammaBayAnimState < 1)
		{
			sat->GammaBay1Indicator.SetState(0);

			if (GammaBayAnimState == 0)
			{
				sat->GammaBay1Indicator.SetState(1);
			}
		}

		//Gamma Bay Spectrometer Jett Animation
		if (sat->GammaBayJett)
		{
			GammaBayJettAnimState = 1;
		}
		else
		{
			GammaBayJettAnimState = 0;
		}
		sat->SetAnimation(GammaBayJettAnim, GammaBayJettAnimState);

		//Mass Spectrometer animaton
		if (sat->MassSpectrometerDeployed)
		{
			MassSpectrometerAnimState += simdt * spectspeed;
			if (MassSpectrometerAnimState > 1.0)
				MassSpectrometerAnimState = 1.0;
		}
		else
		{
			MassSpectrometerAnimState -= simdt * spectspeed;
			if (MassSpectrometerAnimState < 0.0)
				MassSpectrometerAnimState = 0.0;
		}
		sat->SetAnimation(MassSpectrometerAnim, MassSpectrometerAnimState);

		if (MassSpectrometerAnimState < 1)
		{
			sat->MassSpectrometer1Indicator.SetState(0);

			if (MassSpectrometerAnimState == 0)
			{
				sat->MassSpectrometer1Indicator.SetState(1);
			}
		}

		//Gamma Bay Spectrometer Jett Animation
		if (sat->MassSpectrometerJett)
		{
			MassSpectrometerJettAnimState = 1;
		}
		else
		{
			MassSpectrometerJettAnimState = 0;
		}
		sat->SetAnimation(MassSpectrometerJettAnim, MassSpectrometerJettAnimState);

		//XRay Cover animation
		if (sat->XRayCoverDeployed)
		{
			XRayCoverAnimState += simdt * speed;
			if (XRayCoverAnimState > 1.0)
				XRayCoverAnimState = 1.0;
		}
		else
		{
			XRayCoverAnimState -= simdt * speed;
			if (XRayCoverAnimState < 0.0)
				XRayCoverAnimState = 0.0;
		}
		sat->SetAnimation(XRayCoverAnim, XRayCoverAnimState);

		//SubSat Launch Animation
		if (sat->SubSatLaunched && !sat->SubSatRetracted)
		{
			SubSatCoverAnimState += simdt * speed;
			if (SubSatCoverAnimState > 1.0)
				SubSatCoverAnimState = 1.0;
		}
		else if (sat->SubSatLaunched && sat->SubSatRetracted)
		{
			SubSatCoverAnimState -= simdt * speed;
			if (SubSatCoverAnimState < 0.0)
				SubSatCoverAnimState = 0.0;
		}
		sat->SetAnimation(SubSatCoverAnim, SubSatCoverAnimState);
	}

	if (sat->pMission->GetPanel230Version() == 2)
	{
		//CSM 114 Specific Animations
		//IRCover Deploying
		if (sat->IRCoverDeployed)
		{
			IRCoverAnimState += simdt * speed;
			if (IRCoverAnimState > 1.0)
				IRCoverAnimState = 1.0;
		}
		else
		{
			IRCoverAnimState -= simdt * speed;
			if (IRCoverAnimState < 0.0)
				IRCoverAnimState = 0.0;
		}
		sat->SetAnimation(IRCoverAnim, IRCoverAnimState);

		//UVCover Deploying
		if (sat->UVCoverDeployed)
		{
			UVCoverAnimState += simdt * speed;
			if (UVCoverAnimState > 1.0)
				UVCoverAnimState = 1.0;
		}
		else
		{
			UVCoverAnimState -= simdt * speed;
			if (UVCoverAnimState < 0.0)
				UVCoverAnimState = 0.0;
		}
		sat->SetAnimation(UVCoverAnim, UVCoverAnimState);
	}
}

// Load
void SIMBay::MappingCameraLoadState(char* line) {
	sscanf(line + 13, "%lf", &MappingCameraAnimState);
}

// Save
void SIMBay::MappingCameraSaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%lf", MappingCameraAnimState);

	oapiWriteScenario_string(scn, "MAPPINGCAMERA", buffer);
}

//CSM 112 Only
// Load
void SIMBay::GammaBayLoadState(char* line) {
	sscanf(line + 8, "%lf", &GammaBayAnimState);
}

// Save
void SIMBay::GammaBaySaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%lf", GammaBayAnimState);

	oapiWriteScenario_string(scn, "GAMMABAY", buffer);
}

// Load
void SIMBay::MassSpectrometerLoadState(char* line) {
	sscanf(line + 8, "%lf", &MassSpectrometerAnimState);
}

// Save
void SIMBay::MassSpectrometerSaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%lf", MassSpectrometerAnimState);

	oapiWriteScenario_string(scn, "MASSSPEC", buffer);
}