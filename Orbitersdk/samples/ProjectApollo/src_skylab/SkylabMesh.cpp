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
	static VECTOR3 RotationPoint = _V(0, 0.3556, 7.5438);
	static VECTOR3 RotationAxis = _V(1.0, 0.0, 0.0);
	static UINT mgroupA[18] = { 2,3,215,35,36,37,38,39,40,41,42,43,44,49,53,76,79,103 };
	static MGROUP_ROTATE rotateA(pSkylab->skylabmeshID, mgroupA, 18, RotationPoint, RotationAxis, (float)(RAD * -90));
	anim_ATM = pSkylab->CreateAnimation(0.0);
	ANIMATIONCOMPONENT_HANDLE animATM = pSkylab->AddAnimationComponent(anim_ATM, 0.0f, 1.0f, &rotateA);
}

void Skylab::SkylabAnimations::DeployATM(double state) {
	//demo code, delete before merge.
	double deploy = state / DEG;
	pSkylab->SetAnimation(anim_ATM, deploy);
}