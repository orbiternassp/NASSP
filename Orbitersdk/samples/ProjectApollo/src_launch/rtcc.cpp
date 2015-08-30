/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC)

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

// To force orbitersdk.h to use <fstream> in any compiler version
//#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "IMU.h"
#include "lvimu.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"
#include "OrbMech.h"
#include "rtcc.h"

RTCC::RTCC()
{
	cm = NULL;
}

void RTCC::Init(Saturn *vs)
{
	cm = vs;
}

void RTCC::Calculation(int missiontype, int missionstate, LPVOID &pad)
{
	if (missiontype == MTP_C)
	{
		if (missionstate == MST_C_SEPARATION)
		{
			LambertMan lambert;

			lambert.T1 = 3 * 3600 + 20 * 60;
			lambert.T2 = 26 * 3600 + 25 * 60;
			lambert.N = 15;
			lambert.axis = RTCC_LAMBERT_XAXIS;
			lambert.Offset = _V(76.5 * 1852, 0, 0);
			lambert.PhaseAngle = 0;
			lambert.target = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG"));
			lambert.gravref = cm->GetGravityRef();
			lambert.prograde = RTCC_LAMBERT_PROGRADE;
			lambert.impulsive = RTCC_LAMBERT_IMPULSIVE;
			lambert.Perturbation = RTCC_LAMBERT_SPHERICAL;

			double P30TIG;
			VECTOR3 dV_LVLH;
			LambertTargeting(&lambert, dV_LVLH, P30TIG);

			AP7MNV * form = (AP7MNV *)pad;

			form->dV = dV_LVLH;
			form->GETI = P30TIG;

		}
	}
}

void RTCC::LambertTargeting(LambertMan *lambert, VECTOR3 &dV_LVLH, double &P30TIG)
{
	VECTOR3 RA0, VA0, RP0, VP0, RA0_orb, VA0_orb, RP0_orb, VP0_orb;
	VECTOR3 RA1, VA1, RP2, VP2;
	MATRIX3 Rot;
	double SVMJD,GET,GETbase,dt1,dt2,mu;

	cm->GetRelativePos(lambert->gravref, RA0_orb);
	cm->GetRelativeVel(lambert->gravref, VA0_orb);
	lambert->target->GetRelativePos(lambert->gravref, RP0_orb);
	lambert->target->GetRelativeVel(lambert->gravref, VP0_orb);
	SVMJD = oapiGetSimMJD();
	GET = cm->GetMissionTime();

	GETbase = SVMJD - GET / 24.0 / 3600.0;
	Rot = OrbMech::J2000EclToBRCS(40222.525);
	mu = GGRAV*oapiGetMass(lambert->gravref);

	RA0 = mul(Rot, _V(RA0_orb.x, RA0_orb.z, RA0_orb.y));
	VA0 = mul(Rot, _V(VA0_orb.x, VA0_orb.z, VA0_orb.y));
	RP0 = mul(Rot, _V(RP0_orb.x, RP0_orb.z, RP0_orb.y));
	VP0 = mul(Rot, _V(VP0_orb.x, VP0_orb.z, VP0_orb.y));

	dt1 = lambert->T1 - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = lambert->T2 - lambert->T1;

	if (lambert->Perturbation == 1)
	{
		OrbMech::oneclickcoast(RA0, VA0, SVMJD, dt1, RA1, VA1, lambert->gravref, lambert->gravref);
		OrbMech::oneclickcoast(RP0, VP0, SVMJD, dt1 + dt2, RP2, VP2, lambert->gravref, lambert->gravref);
	}
	else
	{
		OrbMech::rv_from_r0v0(RA0, VA0, dt1, RA1, VA1, mu);
		OrbMech::rv_from_r0v0(RP0, VP0, dt1 + dt2, RP2, VP2, mu);
	}

	VECTOR3 i, j, k, yvec,RP2off,VA1_apo;
	double angle;
	MATRIX3 Q_Xx2, Q_Xx;

	k = -RP2 / length(RP2);
	j = crossp(VP2, RP2) / length(RP2) / length(VP2);
	i = crossp(j, k);
	Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	
	RP2off = RP2 + tmul(Q_Xx2, _V(0.0, lambert->Offset.y, lambert->Offset.z));
	angle = lambert->Offset.x / length(RP2);
	yvec = _V(Q_Xx2.m21, Q_Xx2.m22, Q_Xx2.m23);
	RP2off = OrbMech::RotateVector(yvec, -angle, RP2off);

	if (lambert->Perturbation == RTCC_LAMBERT_PERTURBED)
	{
		VA1_apo = OrbMech::Vinti(RA1, VA1, RP2off, SVMJD + dt1 / 24.0 / 3600.0, dt2, lambert->N, lambert->prograde, lambert->gravref); //Vinti Targeting: For non-spherical gravity
	}
	else
	{
		if (lambert->axis == RTCC_LAMBERT_MULTIAXIS)
		{
			VA1_apo = OrbMech::elegant_lambert(RA1, VA1, RP2off, dt2, lambert->N, lambert->prograde, mu);	//Lambert Targeting
		}
		else
		{
			OrbMech::xaxislambert(RA1, VA1, RP2off, dt2, lambert->N, lambert->prograde, mu, VA1_apo, lambert->Offset.z);	//Lambert Targeting
		}
	}

	if (lambert->impulsive == RTCC_LAMBERT_IMPULSIVE)
	{
		j = unit(crossp(VA1, RA1));
		k = unit(-RA1);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z); 
		dV_LVLH = mul(Q_Xx, VA1_apo - VA1);
		P30TIG = lambert->T1;
	}
	else
	{
		VECTOR3 Llambda,RA1_cor,VA1_cor;
		double t_slip;
		

		OrbMech::impulsive(cm, RA1, VA1, lambert->gravref, cm->GetGroupThruster(THGROUP_MAIN, 0), VA1_apo - VA1, Llambda, t_slip);
		OrbMech::rv_from_r0v0(RA1, VA1, t_slip, RA1_cor, VA1_cor, mu);

		j = unit(crossp(VA1_cor, RA1_cor));
		k = unit(-RA1_cor);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG = lambert->T1 + t_slip;
	}

}