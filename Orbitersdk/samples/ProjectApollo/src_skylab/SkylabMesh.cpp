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
	pSkylab = s;
}

Skylab::SkylabAnimations::~SkylabAnimations()
{

}

void Skylab::SkylabAnimations::DefineAnimations()
{
	//I don't like how much stuff is in this function either...

	//Define ATM Main Rotation -------------------------------------------------------------------------------------------------------------------------
	static UINT MGroupATM[18] = { 2,3,215,35,36,37,38,39,40,41,42,43,44,49,53,76,79,103 };
	static VECTOR3 ATMRotationPoint = _V(0, 0.3556, 7.5438);
	static VECTOR3 ATMRotationAxis = _V(1.0, 0.0, 0.0);
	static MGROUP_ROTATE rotateATM(pSkylab->skylabmeshID, MGroupATM, 18, ATMRotationPoint, ATMRotationAxis, (float)(RAD * -90));
	anim_ATM = pSkylab->CreateAnimation(0.0);
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

	static VECTOR3 ATMArray1RotationAxis = _V(1.0, -1.0, 0.0); //Same Rotation Axis for all pannels

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

	anim_ATMArray1[0] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[1] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[2] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[3] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[4] = pSkylab->CreateAnimation(0.0);

	anim_ATMArray1[5] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[6] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[7] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[8] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[9] = pSkylab->CreateAnimation(0.0);
	anim_ATMArray1[10] = pSkylab->CreateAnimation(0.0);

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

}

void Skylab::SkylabAnimations::DeployATM(double state) {
	//demo code, delete before merge.
	double deploy = state / DEG;
	pSkylab->SetAnimation(anim_ATM, deploy);

	if (deploy < PI05) { return; }

	for (auto i : anim_ATMArray1) {
		pSkylab->SetAnimation(i, deploy - PI05);
	}
}