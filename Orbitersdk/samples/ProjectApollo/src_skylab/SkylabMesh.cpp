/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2023


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

  See https://github.com/orbiternassp/NASSP/blob/Orbiter2016/NASSP-LICENSE.txt
  for more details.

 **************************************************************************/
#include "skylab.h"

Skylab::SkylabAnimations::SkylabAnimations(Skylab* s)
{
	//Set Pointer to Skylab Vessel
	pSkylab = s;

	//Create Animation objects for later definition and use.
	anim_ATM = pSkylab->CreateAnimation(0.0);

	for (int ii = 0; ii < 11; ii++) {
		anim_ATMArray1[ii] = pSkylab->CreateAnimation(0.0);
		anim_ATMArray2[ii] = pSkylab->CreateAnimation(0.0);
		anim_ATMArray3[ii] = pSkylab->CreateAnimation(0.0);
		anim_ATMArray4[ii] = pSkylab->CreateAnimation(0.0);
	}

	for (int ii = 0; ii < 4; ii++) { anim_ClothesLineBoom[ii] = pSkylab->CreateAnimation(0.0); }
}

Skylab::SkylabAnimations::~SkylabAnimations()
{
	//Nothing to delete yet...
}

void Skylab::SkylabAnimations::DefineAnimations()
{
	//I don't like how much stuff is in this function either...

	//Define ATM Main Rotation -------------------------------------------------------------------------------------------------------------------------
	static UINT MGroupATM[18] = { 2,3,215,35,36,37,38,39,40,41,42,43,44,49,53,76,79,103 };
	static VECTOR3 ATMRotationPoint = _V(0, 0.3556, 7.5438);
	static VECTOR3 ATMRotationAxis = _V(1.0, 0.0, 0.0);
	static MGROUP_ROTATE rotateATM(pSkylab->skylabmeshID, MGroupATM, 18, ATMRotationPoint, ATMRotationAxis, (float)(RAD * -90));
	
	ANIMATIONCOMPONENT_HANDLE hComp_animATM = pSkylab->AddAnimationComponent(anim_ATM, 0.0f, 1.0f, &rotateATM);
	

	//Define ATM Solar Array 1 -------------------------------------------------------------------------------------------------------------------------
	static UINT MGroupATMArray1A[3] = { 84, 109, 129 };
	static UINT MGroupATMArray1B[3] = { 83, 108, 128 };
	static UINT MGroupATMArray1C[3] = { 86, 111, 131 };
	static UINT MGroupATMArray1D[3] = { 85, 110, 130 };
	static UINT MGroupATMArray1E[4] = { 51, 101, 126, 146 };
	static UINT MGroupATMArray1EArms[1] = { 10 };
	static UINT MGroupATMArray1DArms[1] = { 9 };
	static UINT MGroupATMArray1CArms[1] = { 8 };
	static UINT MGroupATMArray1BArms[1] = { 7 };
	static UINT MGroupATMArray1AArms[1] = { 6 };
	static UINT MGroupATMArray10Arms[1] = { 5 };

	static VECTOR3 ATMArray1RotationAxis = _V(1.0, -1.0, 0.0); //Same Rotation Axis for all pannels

	static VECTOR3 ATMArray1ARotationPoint = _V(-1.1175, -1.1175, 14.1977);
	static VECTOR3 ATMArray1BRotationPoint = _V(-1.6016, -1.6016, 11.6987);
	static VECTOR3 ATMArray1CRotationPoint = _V(-1.1697, -1.1697, 14.2168);
	static VECTOR3 ATMArray1DRotationPoint = _V(-1.6538, -1.6538, 11.7178);
	static VECTOR3 ATMArray1ERotationPoint = _V(-1.2219, -1.2219, 14.2358);
	static VECTOR3 ATMArray1ERotationPointArms = _V(-1.4639, -1.4639, 12.9863);
	static VECTOR3 ATMArray1DRotationPointArms = _V(-1.6799, -1.6799, 11.7273);
	static VECTOR3 ATMArray1CRotationPointArms = _V(-1.1958, -1.1958, 14.2263);
	static VECTOR3 ATMArray1BRotationPointArms = _V(-1.6277, -1.6277, 11.7082);
	static VECTOR3 ATMArray1ARotationPointArms = _V(-1.1436, -1.1436, 14.2072);
	static VECTOR3 ATMArray10RotationPointArms = _V(-1.5755, -1.5755, 11.6892);

	static MGROUP_ROTATE rotateATMArray1A(pSkylab->skylabmeshID, MGroupATMArray1A, 3, ATMArray1ARotationPoint, ATMArray1RotationAxis, (float)(RAD * -71));
	static MGROUP_ROTATE rotateATMArray1B(pSkylab->skylabmeshID, MGroupATMArray1B, 3, ATMArray1BRotationPoint, ATMArray1RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray1C(pSkylab->skylabmeshID, MGroupATMArray1C, 3, ATMArray1CRotationPoint, ATMArray1RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray1D(pSkylab->skylabmeshID, MGroupATMArray1D, 3, ATMArray1DRotationPoint, ATMArray1RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray1E(pSkylab->skylabmeshID, MGroupATMArray1E, 4, ATMArray1ERotationPoint, ATMArray1RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray1EArms(pSkylab->skylabmeshID, MGroupATMArray1EArms, 1, ATMArray1ERotationPointArms, ATMArray1RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray1DArms(pSkylab->skylabmeshID, MGroupATMArray1DArms, 1, ATMArray1DRotationPointArms, ATMArray1RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray1CArms(pSkylab->skylabmeshID, MGroupATMArray1CArms, 1, ATMArray1CRotationPointArms, ATMArray1RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray1BArms(pSkylab->skylabmeshID, MGroupATMArray1BArms, 1, ATMArray1BRotationPointArms, ATMArray1RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray1AArms(pSkylab->skylabmeshID, MGroupATMArray1AArms, 1, ATMArray1ARotationPointArms, ATMArray1RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray10Arms(pSkylab->skylabmeshID, MGroupATMArray10Arms, 1, ATMArray1ARotationPointArms, ATMArray1RotationAxis, (float)(RAD * -170));

	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1A = pSkylab->AddAnimationComponent(anim_ATMArray1[0], 0.0f, 1.0f, &rotateATMArray1A, hComp_animATM);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1B = pSkylab->AddAnimationComponent(anim_ATMArray1[1], 0.0f, 1.0f, &rotateATMArray1B, hComp_ATMArray1A);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1C = pSkylab->AddAnimationComponent(anim_ATMArray1[2], 0.0f, 1.0f, &rotateATMArray1C, hComp_ATMArray1B);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1D = pSkylab->AddAnimationComponent(anim_ATMArray1[3], 0.0f, 1.0f, &rotateATMArray1D, hComp_ATMArray1C);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1E = pSkylab->AddAnimationComponent(anim_ATMArray1[4], 0.0f, 1.0f, &rotateATMArray1E, hComp_ATMArray1D);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1EArms = pSkylab->AddAnimationComponent(anim_ATMArray1[5], 0.0f, 1.0f, &rotateATMArray1EArms, hComp_ATMArray1E);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1DArms = pSkylab->AddAnimationComponent(anim_ATMArray1[6], 0.0f, 1.0f, &rotateATMArray1DArms, hComp_ATMArray1EArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1CArms = pSkylab->AddAnimationComponent(anim_ATMArray1[7], 0.0f, 1.0f, &rotateATMArray1CArms, hComp_ATMArray1DArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1BArms = pSkylab->AddAnimationComponent(anim_ATMArray1[8], 0.0f, 1.0f, &rotateATMArray1BArms, hComp_ATMArray1CArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray1AArms = pSkylab->AddAnimationComponent(anim_ATMArray1[9], 0.0f, 1.0f, &rotateATMArray1AArms, hComp_ATMArray1BArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray10Arms = pSkylab->AddAnimationComponent(anim_ATMArray1[10], 0.0f, 1.0f, &rotateATMArray10Arms, hComp_ATMArray1AArms);

	//Define ATM Solar Array 2 -------------------------------------------------------------------------------------------------------------------------
	static UINT MGroupATMArray2A[3] = { 96, 121, 141 };
	static UINT MGroupATMArray2B[3] = { 95, 120, 140 };
	static UINT MGroupATMArray2C[3] = { 99, 124, 144 };
	static UINT MGroupATMArray2D[3] = { 98, 123, 143 };
	static UINT MGroupATMArray2E[3] = { 97, 122, 142 };
	static UINT MGroupATMArray2EArms[1] = { 28 };
	static UINT MGroupATMArray2DArms[1] = { 27 };
	static UINT MGroupATMArray2CArms[1] = { 26 };
	static UINT MGroupATMArray2BArms[1] = { 25 };
	static UINT MGroupATMArray2AArms[1] = { 24 };
	static UINT MGroupATMArray20Arms[1] = { 23 };

	static VECTOR3 ATMArray2RotationAxis = _V(1.0, 1.0, 0.0); //Same Rotation Axis for all pannels

	static VECTOR3 ATMArray2ARotationPoint = _V(1.1175, -1.1175, 14.1977);
	static VECTOR3 ATMArray2BRotationPoint = _V(1.6016, -1.6016, 11.6987);
	static VECTOR3 ATMArray2CRotationPoint = _V(1.1697, -1.1697, 14.2168);
	static VECTOR3 ATMArray2DRotationPoint = _V(1.6538, -1.6538, 11.7178);
	static VECTOR3 ATMArray2ERotationPoint = _V(1.2219, -1.2219, 14.2358);
	static VECTOR3 ATMArray2ERotationPointArms = _V(1.4639, -1.4639, 12.9863);
	static VECTOR3 ATMArray2DRotationPointArms = _V(1.6799, -1.6799, 11.7273);
	static VECTOR3 ATMArray2CRotationPointArms = _V(1.1958, -1.1958, 14.2263);
	static VECTOR3 ATMArray2BRotationPointArms = _V(1.6277, -1.6277, 11.7082);
	static VECTOR3 ATMArray2ARotationPointArms = _V(1.1436, -1.1436, 14.2072);
	static VECTOR3 ATMArray20RotationPointArms = _V(1.5755, -1.5755, 11.6892);

	static MGROUP_ROTATE rotateATMArray2A(pSkylab->skylabmeshID, MGroupATMArray2A, 3, ATMArray2ARotationPoint, ATMArray2RotationAxis, (float)(RAD * -71));
	static MGROUP_ROTATE rotateATMArray2B(pSkylab->skylabmeshID, MGroupATMArray2B, 3, ATMArray2BRotationPoint, ATMArray2RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray2C(pSkylab->skylabmeshID, MGroupATMArray2C, 3, ATMArray2CRotationPoint, ATMArray2RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray2D(pSkylab->skylabmeshID, MGroupATMArray2D, 3, ATMArray2DRotationPoint, ATMArray2RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray2E(pSkylab->skylabmeshID, MGroupATMArray2E, 3, ATMArray2ERotationPoint, ATMArray2RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray2EArms(pSkylab->skylabmeshID, MGroupATMArray2EArms, 1, ATMArray2ERotationPointArms, ATMArray2RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray2DArms(pSkylab->skylabmeshID, MGroupATMArray2DArms, 1, ATMArray2DRotationPointArms, ATMArray2RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray2CArms(pSkylab->skylabmeshID, MGroupATMArray2CArms, 1, ATMArray2CRotationPointArms, ATMArray2RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray2BArms(pSkylab->skylabmeshID, MGroupATMArray2BArms, 1, ATMArray2BRotationPointArms, ATMArray2RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray2AArms(pSkylab->skylabmeshID, MGroupATMArray2AArms, 1, ATMArray2ARotationPointArms, ATMArray2RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray20Arms(pSkylab->skylabmeshID, MGroupATMArray20Arms, 1, ATMArray2ARotationPointArms, ATMArray2RotationAxis, (float)(RAD * -170));

	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2A = pSkylab->AddAnimationComponent(anim_ATMArray2[0], 0.0f, 1.0f, &rotateATMArray2A, hComp_animATM);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2B = pSkylab->AddAnimationComponent(anim_ATMArray2[1], 0.0f, 1.0f, &rotateATMArray2B, hComp_ATMArray2A);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2C = pSkylab->AddAnimationComponent(anim_ATMArray2[2], 0.0f, 1.0f, &rotateATMArray2C, hComp_ATMArray2B);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2D = pSkylab->AddAnimationComponent(anim_ATMArray2[3], 0.0f, 1.0f, &rotateATMArray2D, hComp_ATMArray2C);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2E = pSkylab->AddAnimationComponent(anim_ATMArray2[4], 0.0f, 1.0f, &rotateATMArray2E, hComp_ATMArray2D);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2EArms = pSkylab->AddAnimationComponent(anim_ATMArray2[5], 0.0f, 1.0f, &rotateATMArray2EArms, hComp_ATMArray2E);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2DArms = pSkylab->AddAnimationComponent(anim_ATMArray2[6], 0.0f, 1.0f, &rotateATMArray2DArms, hComp_ATMArray2EArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2CArms = pSkylab->AddAnimationComponent(anim_ATMArray2[7], 0.0f, 1.0f, &rotateATMArray2CArms, hComp_ATMArray2DArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2BArms = pSkylab->AddAnimationComponent(anim_ATMArray2[8], 0.0f, 1.0f, &rotateATMArray2BArms, hComp_ATMArray2CArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray2AArms = pSkylab->AddAnimationComponent(anim_ATMArray2[9], 0.0f, 1.0f, &rotateATMArray2AArms, hComp_ATMArray2BArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray20Arms = pSkylab->AddAnimationComponent(anim_ATMArray2[10], 0.0f, 1.0f, &rotateATMArray20Arms, hComp_ATMArray2AArms);

	//Define ATM Solar Array 3 -------------------------------------------------------------------------------------------------------------------------
	static UINT MGroupATMArray3A[3] = { 92,117,137 };
	static UINT MGroupATMArray3B[3] = { 91,116,136 };
	static UINT MGroupATMArray3C[3] = { 94,119,139 };
	static UINT MGroupATMArray3D[3] = { 93,118,138 };
	static UINT MGroupATMArray3E[4] = { 50,100,125,145 };
	static UINT MGroupATMArray3EArms[1] = { 22 };
	static UINT MGroupATMArray3DArms[1] = { 21 };
	static UINT MGroupATMArray3CArms[1] = { 20 };
	static UINT MGroupATMArray3BArms[1] = { 19 };
	static UINT MGroupATMArray3AArms[1] = { 18 };
	static UINT MGroupATMArray30Arms[1] = { 17 };

	static VECTOR3 ATMArray3RotationAxis = _V(-1.0, 1.0, 0.0); //Same Rotation Axis for all pannels

	static VECTOR3 ATMArray3ARotationPoint = _V(1.1175, 1.1175, 14.1977);
	static VECTOR3 ATMArray3BRotationPoint = _V(1.6016, 1.6016, 11.6987);
	static VECTOR3 ATMArray3CRotationPoint = _V(1.1697, 1.1697, 14.2168);
	static VECTOR3 ATMArray3DRotationPoint = _V(1.6538, 1.6538, 11.7178);
	static VECTOR3 ATMArray3ERotationPoint = _V(1.2219, 1.2219, 14.2358);
	static VECTOR3 ATMArray3ERotationPointArms = _V(1.4639, 1.4639, 12.9863);
	static VECTOR3 ATMArray3DRotationPointArms = _V(1.6799, 1.6799, 11.7273);
	static VECTOR3 ATMArray3CRotationPointArms = _V(1.1958, 1.1958, 14.2263);
	static VECTOR3 ATMArray3BRotationPointArms = _V(1.6277, 1.6277, 11.7082);
	static VECTOR3 ATMArray3ARotationPointArms = _V(1.1436, 1.1436, 14.2072);
	static VECTOR3 ATMArray30RotationPointArms = _V(1.5755, 1.5755, 11.6892);

	static MGROUP_ROTATE rotateATMArray3A(pSkylab->skylabmeshID, MGroupATMArray3A, 3, ATMArray3ARotationPoint, ATMArray3RotationAxis, (float)(RAD * -71));
	static MGROUP_ROTATE rotateATMArray3B(pSkylab->skylabmeshID, MGroupATMArray3B, 3, ATMArray3BRotationPoint, ATMArray3RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray3C(pSkylab->skylabmeshID, MGroupATMArray3C, 3, ATMArray3CRotationPoint, ATMArray3RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray3D(pSkylab->skylabmeshID, MGroupATMArray3D, 3, ATMArray3DRotationPoint, ATMArray3RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray3E(pSkylab->skylabmeshID, MGroupATMArray3E, 4, ATMArray3ERotationPoint, ATMArray3RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray3EArms(pSkylab->skylabmeshID, MGroupATMArray3EArms, 1, ATMArray3ERotationPointArms, ATMArray3RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray3DArms(pSkylab->skylabmeshID, MGroupATMArray3DArms, 1, ATMArray3DRotationPointArms, ATMArray3RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray3CArms(pSkylab->skylabmeshID, MGroupATMArray3CArms, 1, ATMArray3CRotationPointArms, ATMArray3RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray3BArms(pSkylab->skylabmeshID, MGroupATMArray3BArms, 1, ATMArray3BRotationPointArms, ATMArray3RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray3AArms(pSkylab->skylabmeshID, MGroupATMArray3AArms, 1, ATMArray3ARotationPointArms, ATMArray3RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray30Arms(pSkylab->skylabmeshID, MGroupATMArray30Arms, 1, ATMArray3ARotationPointArms, ATMArray3RotationAxis, (float)(RAD * -170));

	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3A = pSkylab->AddAnimationComponent(anim_ATMArray3[0], 0.0f, 1.0f, &rotateATMArray3A, hComp_animATM);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3B = pSkylab->AddAnimationComponent(anim_ATMArray3[1], 0.0f, 1.0f, &rotateATMArray3B, hComp_ATMArray3A);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3C = pSkylab->AddAnimationComponent(anim_ATMArray3[2], 0.0f, 1.0f, &rotateATMArray3C, hComp_ATMArray3B);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3D = pSkylab->AddAnimationComponent(anim_ATMArray3[3], 0.0f, 1.0f, &rotateATMArray3D, hComp_ATMArray3C);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3E = pSkylab->AddAnimationComponent(anim_ATMArray3[4], 0.0f, 1.0f, &rotateATMArray3E, hComp_ATMArray3D);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3EArms = pSkylab->AddAnimationComponent(anim_ATMArray3[5], 0.0f, 1.0f, &rotateATMArray3EArms, hComp_ATMArray3E);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3DArms = pSkylab->AddAnimationComponent(anim_ATMArray3[6], 0.0f, 1.0f, &rotateATMArray3DArms, hComp_ATMArray3EArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3CArms = pSkylab->AddAnimationComponent(anim_ATMArray3[7], 0.0f, 1.0f, &rotateATMArray3CArms, hComp_ATMArray3DArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3BArms = pSkylab->AddAnimationComponent(anim_ATMArray3[8], 0.0f, 1.0f, &rotateATMArray3BArms, hComp_ATMArray3CArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray3AArms = pSkylab->AddAnimationComponent(anim_ATMArray3[9], 0.0f, 1.0f, &rotateATMArray3AArms, hComp_ATMArray3BArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray30Arms = pSkylab->AddAnimationComponent(anim_ATMArray3[10], 0.0f, 1.0f, &rotateATMArray30Arms, hComp_ATMArray3AArms);

	//Define ATM Solar Array 4 -------------------------------------------------------------------------------------------------------------------------
	static UINT MGroupATMArray4A[3] = { 88, 113, 133 };
	static UINT MGroupATMArray4B[3] = { 87, 112, 132 };
	static UINT MGroupATMArray4C[3] = { 90, 115, 135 };
	static UINT MGroupATMArray4D[3] = { 89, 114, 134 };
	static UINT MGroupATMArray4E[4] = { 52, 102, 127, 147 };
	static UINT MGroupATMArray4EArms[1] = { 16 };
	static UINT MGroupATMArray4DArms[1] = { 15 };
	static UINT MGroupATMArray4CArms[1] = { 14 };
	static UINT MGroupATMArray4BArms[1] = { 13 };
	static UINT MGroupATMArray4AArms[1] = { 12 };
	static UINT MGroupATMArray40Arms[1] = { 11 };

	static VECTOR3 ATMArray4RotationAxis = _V(-1.0, -1.0, 0.0); //Same Rotation Axis for all pannels

	static VECTOR3 ATMArray4ARotationPoint = _V(-1.1175, 1.1175, 14.1977);
	static VECTOR3 ATMArray4BRotationPoint = _V(-1.6016, 1.6016, 11.6987);
	static VECTOR3 ATMArray4CRotationPoint = _V(-1.1697, 1.1697, 14.2168);
	static VECTOR3 ATMArray4DRotationPoint = _V(-1.6538, 1.6538, 11.7178);
	static VECTOR3 ATMArray4ERotationPoint = _V(-1.2219, 1.2219, 14.2358);
	static VECTOR3 ATMArray4ERotationPointArms = _V(-1.4639, 1.4639, 12.9863);
	static VECTOR3 ATMArray4DRotationPointArms = _V(-1.6799, 1.6799, 11.7273);
	static VECTOR3 ATMArray4CRotationPointArms = _V(-1.1958, 1.1958, 14.2263);
	static VECTOR3 ATMArray4BRotationPointArms = _V(-1.6277, 1.6277, 11.7082);
	static VECTOR3 ATMArray4ARotationPointArms = _V(-1.1436, 1.1436, 14.2072);
	static VECTOR3 ATMArray40RotationPointArms = _V(-1.5755, 1.5755, 11.6892);

	static MGROUP_ROTATE rotateATMArray4A(pSkylab->skylabmeshID, MGroupATMArray4A, 3, ATMArray4ARotationPoint, ATMArray4RotationAxis, (float)(RAD * -71));
	static MGROUP_ROTATE rotateATMArray4B(pSkylab->skylabmeshID, MGroupATMArray4B, 3, ATMArray4BRotationPoint, ATMArray4RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray4C(pSkylab->skylabmeshID, MGroupATMArray4C, 3, ATMArray4CRotationPoint, ATMArray4RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray4D(pSkylab->skylabmeshID, MGroupATMArray4D, 3, ATMArray4DRotationPoint, ATMArray4RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray4E(pSkylab->skylabmeshID, MGroupATMArray4E, 4, ATMArray4ERotationPoint, ATMArray4RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray4EArms(pSkylab->skylabmeshID, MGroupATMArray4EArms, 1, ATMArray4ERotationPointArms, ATMArray4RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray4DArms(pSkylab->skylabmeshID, MGroupATMArray4DArms, 1, ATMArray4DRotationPointArms, ATMArray4RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray4CArms(pSkylab->skylabmeshID, MGroupATMArray4CArms, 1, ATMArray4CRotationPointArms, ATMArray4RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray4BArms(pSkylab->skylabmeshID, MGroupATMArray4BArms, 1, ATMArray4BRotationPointArms, ATMArray4RotationAxis, (float)(RAD * -170));
	static MGROUP_ROTATE rotateATMArray4AArms(pSkylab->skylabmeshID, MGroupATMArray4AArms, 1, ATMArray4ARotationPointArms, ATMArray4RotationAxis, (float)(RAD * 170));
	static MGROUP_ROTATE rotateATMArray40Arms(pSkylab->skylabmeshID, MGroupATMArray40Arms, 1, ATMArray4ARotationPointArms, ATMArray4RotationAxis, (float)(RAD * -170));

	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4A = pSkylab->AddAnimationComponent(anim_ATMArray4[0], 0.0f, 1.0f, &rotateATMArray4A, hComp_animATM);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4B = pSkylab->AddAnimationComponent(anim_ATMArray4[1], 0.0f, 1.0f, &rotateATMArray4B, hComp_ATMArray4A);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4C = pSkylab->AddAnimationComponent(anim_ATMArray4[2], 0.0f, 1.0f, &rotateATMArray4C, hComp_ATMArray4B);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4D = pSkylab->AddAnimationComponent(anim_ATMArray4[3], 0.0f, 1.0f, &rotateATMArray4D, hComp_ATMArray4C);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4E = pSkylab->AddAnimationComponent(anim_ATMArray4[4], 0.0f, 1.0f, &rotateATMArray4E, hComp_ATMArray4D);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4EArms = pSkylab->AddAnimationComponent(anim_ATMArray4[5], 0.0f, 1.0f, &rotateATMArray4EArms, hComp_ATMArray4E);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4DArms = pSkylab->AddAnimationComponent(anim_ATMArray4[6], 0.0f, 1.0f, &rotateATMArray4DArms, hComp_ATMArray4EArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4CArms = pSkylab->AddAnimationComponent(anim_ATMArray4[7], 0.0f, 1.0f, &rotateATMArray4CArms, hComp_ATMArray4DArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4BArms = pSkylab->AddAnimationComponent(anim_ATMArray4[8], 0.0f, 1.0f, &rotateATMArray4BArms, hComp_ATMArray4CArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray4AArms = pSkylab->AddAnimationComponent(anim_ATMArray4[9], 0.0f, 1.0f, &rotateATMArray4AArms, hComp_ATMArray4BArms);
	ANIMATIONCOMPONENT_HANDLE hComp_ATMArray40Arms = pSkylab->AddAnimationComponent(anim_ATMArray4[10], 0.0f, 1.0f, &rotateATMArray40Arms, hComp_ATMArray4AArms);

	//Define Clothes Line Boom ----------------------------------------------------------------------------------------------------------------------

	static UINT MGroupClothesLineBoomAUp[1] = { 45 };
	static UINT MGroupClothesLineBoomBUp[1] = { 46 };
	static UINT MGroupClothesLineBoomAOut[1] = { 45 };
	static UINT MGroupClothesLineBoomBOut[1] = { 46 };

	static VECTOR3 ClothesLineBoomRotationAxis = _V(0.0, 1.0, 0.0); //Same Rotation Axis for all parts

	static VECTOR3 ClothesLineBoomAUpRotationPoint = _V(-0.4191, 1.6383, 15.3162);
	static VECTOR3 ClothesLineBoomBUpRotationPoint = _V(0.4572, 1.6383, 15.3162);
	static VECTOR3 ClothesLineBoomAOutRotationPoint = _V(-0.4191, 8.1312, 6.2606);
	static VECTOR3 ClothesLineBoomBOutRotationPoint = _V(0.3398, 8.5694, 6.2617);

	static MGROUP_ROTATE rotateClothesLineBoomAUp(pSkylab->skylabmeshID, MGroupClothesLineBoomAUp, 1, ClothesLineBoomAUpRotationPoint, ClothesLineBoomRotationAxis, (float)(RAD * -30));
	static MGROUP_ROTATE rotateClothesLineBoomBUp(pSkylab->skylabmeshID, MGroupClothesLineBoomBUp, 1, ClothesLineBoomBUpRotationPoint, ClothesLineBoomRotationAxis, (float)(RAD * 30));
	static MGROUP_ROTATE rotateClothesLineBoomAOut(pSkylab->skylabmeshID, MGroupClothesLineBoomAOut, 1, ClothesLineBoomAOutRotationPoint, ClothesLineBoomRotationAxis, (float)(RAD * 75));
	static MGROUP_ROTATE rotateClothesLineBoomBOut(pSkylab->skylabmeshID, MGroupClothesLineBoomBOut, 1, ClothesLineBoomBOutRotationPoint, ClothesLineBoomRotationAxis, (float)(RAD * -180));

	ANIMATIONCOMPONENT_HANDLE hComp_ClothesLineBoomAUp = pSkylab->AddAnimationComponent(anim_ClothesLineBoom[0], 0.0f, 0.5f, &rotateClothesLineBoomAUp, hComp_animATM);
	ANIMATIONCOMPONENT_HANDLE hComp_ClothesLineBoomBUp = pSkylab->AddAnimationComponent(anim_ClothesLineBoom[1], 0.0f, 0.5f, &rotateClothesLineBoomBUp, hComp_ClothesLineBoomAUp);
	ANIMATIONCOMPONENT_HANDLE hComp_ClothesLineBoomAOut = pSkylab->AddAnimationComponent(anim_ClothesLineBoom[2], 0.5f, 1.0f, &rotateClothesLineBoomAOut);
	ANIMATIONCOMPONENT_HANDLE hComp_ClothesLineBoomBOut = pSkylab->AddAnimationComponent(anim_ClothesLineBoom[3], 0.5f, 1.0f, &rotateClothesLineBoomBOut, hComp_ClothesLineBoomAOut);

}

void Skylab::SkylabAnimations::SetATMAnimationState(double state) {
	pSkylab->SetAnimation(anim_ATM, state);
}

void Skylab::SkylabAnimations::SetATMArrayAnimationState(int ATMSAS, double state) {
	
	double ATM_DeployAngle = -pSkylab->GetAnimation(anim_ATM) * PI05; //left handed rotation about X.

	MATRIX3 ATM_RotationMatrix = _M(1.0, 0.0, 0.0,
		0.0, cos(ATM_DeployAngle), -sin(ATM_DeployAngle),
		0.0, sin(ATM_DeployAngle), cos(ATM_DeployAngle));

	switch (ATMSAS) {
	case 0:
		for (auto i : anim_ClothesLineBoom) {
			pSkylab->SetAnimation(i, state);
		}
		break;
	case 1:
		for (auto i : anim_ATMArray1) {
			pSkylab->SetAnimation(i, state);
		}
		break;
	case 2:
		for (auto i : anim_ATMArray2) {
			pSkylab->SetAnimation(i, state);
		}
		break;
	case 3:
		for (auto i : anim_ATMArray3) {
			pSkylab->SetAnimation(i, state);
		}
		break;
	case 4:
		for (auto i : anim_ATMArray4) {
			pSkylab->SetAnimation(i, state);
		}
		break;
	default:
		//nope
		return;
	}
}