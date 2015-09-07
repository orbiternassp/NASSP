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
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "rtcc.h"

RTCC::RTCC()
{
	mcc = NULL;
}

void RTCC::Init(MCC *ptr)
{
	mcc = ptr;
}

void RTCC::Calculation(LPVOID &pad)
{
	if (mcc->MissionType == MTP_C)
	{
		if (mcc->MissionState == MST_C_SEPARATION)
		{
			LambertMan lambert;
			AP7ManPADOpt opt;
			double P30TIG;
			VECTOR3 dV_LVLH;

			AP7MNV * form = (AP7MNV *)pad;

			lambert.T1 = 3 * 3600 + 20 * 60;
			lambert.T2 = 26 * 3600 + 25 * 60;
			lambert.N = 15;
			lambert.axis = RTCC_LAMBERT_XAXIS;
			lambert.Offset = _V(76.5 * 1852, 0, 0);
			lambert.PhaseAngle = 0;
			lambert.target = oapiGetVesselInterface(oapiGetVesselByName("AS-205-S4BSTG")); // Target should come from MCC later so we can parameterize it
			lambert.gravref = mcc->cm->GetGravityRef();
			lambert.prograde = RTCC_LAMBERT_PROGRADE;
			lambert.impulsive = RTCC_IMPULSIVE;
			lambert.Perturbation = RTCC_LAMBERT_SPHERICAL;

			LambertTargeting(&lambert, dV_LVLH, P30TIG);

			opt.TIG = P30TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.engopt = 2; //X- RCS Thrusters
			opt.HeadsUp = false;
			opt.sxtstardtime = 0;
			opt.REFSMMAT = GetREFSMMATfromAGC();
			opt.navcheckGET = 0;

			AP7ManeuverPAD(&opt, *form);

		}
		else if (mcc->MissionState == MST_C_COAST1)
		{
			AP7MNV * form = (AP7MNV *)pad;

			double P30TIG;
			VECTOR3 dV_LVLH;
			EntryOpt entopt;
			AP7ManPADOpt opt;

			entopt.impulsive = RTCC_NONIMPULSIVE;
			entopt.lng = -165.0*RAD;
			entopt.nominal = RTCC_ENTRY_NOMINAL;
			entopt.Range = 0;
			entopt.ReA = 0;
			entopt.TIGguess = 8 * 60 * 60 + 55 * 60;
			entopt.type = RTCC_ENTRY_DEORBIT;
			entopt.vessel = mcc->cm;

			EntryTargeting(&entopt, dV_LVLH, P30TIG);

			opt.TIG = P30TIG;
			opt.dV_LVLH = dV_LVLH;
			opt.engopt = 0;
			opt.HeadsUp = true;
			opt.sxtstardtime = -25 * 60;
			opt.REFSMMAT = GetREFSMMATfromAGC();
			opt.navcheckGET = 8 * 60 * 60 + 17 * 60;

			AP7ManeuverPAD(&opt, *form);
		}
	}
}

void RTCC::EntryTargeting(EntryOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	Entry* entry;
	double SVMJD, GET, GETbase;
	VECTOR3 RA0_orb, VA0_orb;
	bool stop;

	stop = false;
	
	mcc->cm->GetRelativePos(AGCGravityRef(opt->vessel), RA0_orb);
	mcc->cm->GetRelativeVel(AGCGravityRef(opt->vessel), VA0_orb);
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();
	GETbase = SVMJD - GET / 24.0 / 3600.0;

	entry = new Entry(opt->vessel, AGCGravityRef(opt->vessel), GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->type, opt->Range, opt->nominal);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	dV_LVLH = entry->Entry_DV;
	P30TIG = entry->EntryTIGcor;

	delete entry;

	if (opt->impulsive == RTCC_NONIMPULSIVE)
	{
		VECTOR3 Llambda, RA1_cor, VA1_cor, R0,V0,RA1,VA1,UX,UY,UZ,DV,i,j,k;
		double t_slip;
		MATRIX3 Rot, Q_Xx;
		
		Rot = OrbMech::J2000EclToBRCS(40222.525);

		R0 = mul(Rot, _V(RA0_orb.x, RA0_orb.z, RA0_orb.y));
		V0 = mul(Rot, _V(VA0_orb.x, VA0_orb.z, VA0_orb.y));

		OrbMech::oneclickcoast(R0, V0, SVMJD, P30TIG - GET, RA1, VA1, AGCGravityRef(opt->vessel), oapiGetObjectByName("Earth"));

		UY = unit(crossp(VA1, RA1));
		UZ = unit(-RA1);
		UX = crossp(UY, UZ);

		DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;

		OrbMech::impulsive(mcc->cm, RA1, VA1, AGCGravityRef(opt->vessel), mcc->cm->GetGroupThruster(THGROUP_MAIN, 0), DV, Llambda, t_slip);
		OrbMech::oneclickcoast(RA1, VA1, SVMJD + (P30TIG - GET) / 24.0 / 3600.0, t_slip, RA1_cor, VA1_cor, AGCGravityRef(opt->vessel), AGCGravityRef(opt->vessel));

		j = unit(crossp(VA1_cor, RA1_cor));
		k = unit(-RA1_cor);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG += t_slip;
	}
}

void RTCC::LambertTargeting(LambertMan *lambert, VECTOR3 &dV_LVLH, double &P30TIG)
{
	VECTOR3 RA0, VA0, RP0, VP0, RA0_orb, VA0_orb, RP0_orb, VP0_orb;
	VECTOR3 RA1, VA1, RP2, VP2;
	MATRIX3 Rot;
	double SVMJD,GET,GETbase,dt1,dt2,mu;

	mcc->cm->GetRelativePos(lambert->gravref, RA0_orb);
	mcc->cm->GetRelativeVel(lambert->gravref, VA0_orb);
	lambert->target->GetRelativePos(lambert->gravref, RP0_orb);
	lambert->target->GetRelativeVel(lambert->gravref, VP0_orb);
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();

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

	if (lambert->impulsive == RTCC_IMPULSIVE)
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
		

		OrbMech::impulsive(mcc->cm, RA1, VA1, lambert->gravref, mcc->cm->GetGroupThruster(THGROUP_MAIN, 0), VA1_apo - VA1, Llambda, t_slip);
		OrbMech::rv_from_r0v0(RA1, VA1, t_slip, RA1_cor, VA1_cor, mu);

		j = unit(crossp(VA1_cor, RA1_cor));
		k = unit(-RA1_cor);
		i = crossp(j, k);
		Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG = lambert->T1 + t_slip;
	}

}

void RTCC::AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad)
{
	VECTOR3 DV_P, DV_C, V_G;
	double SVMJD, dt, mu, theta_T, t_go;
	VECTOR3 R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, X_B, DV, U_TD, R2B, V2B, Att;
	MATRIX3 Rot, M, M_R, M_RTM;
	double p_T, y_T, peri, apo, m1;
	double headsswitch, GET, GETbase;
	OBJHANDLE gravref;

	gravref = mcc->cm->GetGravityRef();

	mu = GGRAV*oapiGetMass(mcc->cm->GetGravityRef());

	mcc->cm->GetRelativePos(gravref, R_A);
	mcc->cm->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();
	GETbase = SVMJD - GET / 24.0 / 3600.0;

	dt = opt->TIG - (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R_A.x, R_A.z, R_A.y));
	V0B = mul(Rot, _V(V_A.x, V_A.z, V_A.y));

	OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);

	UY = unit(crossp(V1B, R1B));
	UZ = unit(-R1B);
	UX = crossp(UY, UZ);

	DV = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;

	pad.Weight = mcc->cm->GetMass() / 0.45359237;

	double v_e, F;
	if (opt->engopt == 0)
	{
		v_e = mcc->cm->GetThrusterIsp0(mcc->cm->GetGroupThruster(THGROUP_MAIN, 0));
		F = mcc->cm->GetThrusterMax0(mcc->cm->GetGroupThruster(THGROUP_MAIN, 0));
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	pad.burntime = v_e / F *mcc->cm->GetMass()*(1.0 - exp(-length(opt->dV_LVLH) / v_e));

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	if (opt->engopt == 0)
	{
		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		if (length(DV_P) != 0.0)
		{
			theta_T = length(crossp(R1B, V1B))*length(opt->dV_LVLH)*mcc->cm->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
			DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*opt->dV_LVLH.y;
		}
		else
		{
			V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
		}

		U_TD = unit(V_G);

		OrbMech::poweredflight(mcc->cm, R1B, V1B, gravref, mcc->cm->GetGroupThruster(THGROUP_MAIN, 0), V_G, R2B, V2B, t_go);

		OrbMech::periapo(R2B, V2B, mu, apo, peri);
		pad.HA = apo - oapiGetSize(gravref);
		pad.HP = peri - oapiGetSize(gravref);

		p_T = -2.15*RAD;
		y_T = 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, R1B*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::transpose_matrix(M_R), M);

		m1 = mcc->cm->GetMass()*exp(-length(opt->dV_LVLH) / v_e);
		pad.Vc = length(opt->dV_LVLH)*cos(-2.15*RAD)*cos(0.95*RAD) - 60832.18 / m1;
	}
	else
	{
		OrbMech::periapo(R1B, V1B + DV, mu, apo, peri);
		pad.HA = apo - oapiGetSize(gravref);
		pad.HP = peri - oapiGetSize(gravref);

		V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
		X_B = unit(V_G);
		if (opt->engopt == 1)
		{
			UX = X_B;
		}
		else
		{
			UX = -X_B;
		}
		UY = unit(crossp(UX, R1B*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, R1B*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

		pad.Vc = length(opt->dV_LVLH);
	}
	Att = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::transpose_matrix(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	//GDCangles = OrbMech::backupgdcalignment(REFSMMAT, R1B, oapiGetSize(gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, opt->sxtstardtime, Rsxt, Vsxt, gravref, gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(OrbMech::round(pad.Att.x*DEG)*RAD, OrbMech::round(pad.Att.y*DEG)*RAD, OrbMech::round(pad.Att.z*DEG)*RAD), Rsxt, oapiGetSize(gravref), pad.Star, pad.Trun, pad.Shaft);

	if (opt->navcheckGET != 0.0)
	{
		VECTOR3 Rnav, Vnav;
		double alt, lat, lng;
		OrbMech::oneclickcoast(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, opt->navcheckGET - opt->TIG, Rnav, Vnav, gravref, gravref);
		navcheck(Rnav, Vnav, GETbase + opt->navcheckGET / 24.0 / 3600.0, gravref, lat, lng, alt);

		pad.NavChk = opt->navcheckGET;
		pad.lat = lat*DEG;
		pad.lng = lng*DEG;
		pad.alt = alt / 1852;
	}

	pad.Att = _V(OrbMech::imulimit(Att.x*DEG), OrbMech::imulimit(Att.y*DEG), OrbMech::imulimit(Att.z*DEG));

	pad.GETI = opt->TIG;
	pad.pTrim = 0;
	pad.yTrim = 0;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.Vc /= 0.3048;
	pad.Shaft *= DEG;
	pad.Trun *= DEG;
	pad.HA /= 1852.0;
	pad.HP /= 1852.0;
}

MATRIX3 RTCC::GetREFSMMATfromAGC()
{
	MATRIX3 REFSMMAT;
	char Buffer[100];
	int REFSMMAToct[20];

	if (mcc->cm->IsVirtualAGC() == FALSE)
	{
		
	}
	else
	{
		unsigned short REFSoct[20];
		REFSoct[2] = mcc->cm->agc.vagc.Erasable[0][01735];
		REFSoct[3] = mcc->cm->agc.vagc.Erasable[0][01736];
		REFSoct[4] = mcc->cm->agc.vagc.Erasable[0][01737];
		REFSoct[5] = mcc->cm->agc.vagc.Erasable[0][01740];
		REFSoct[6] = mcc->cm->agc.vagc.Erasable[0][01741];
		REFSoct[7] = mcc->cm->agc.vagc.Erasable[0][01742];
		REFSoct[8] = mcc->cm->agc.vagc.Erasable[0][01743];
		REFSoct[9] = mcc->cm->agc.vagc.Erasable[0][01744];
		REFSoct[10] = mcc->cm->agc.vagc.Erasable[0][01745];
		REFSoct[11] = mcc->cm->agc.vagc.Erasable[0][01746];
		REFSoct[12] = mcc->cm->agc.vagc.Erasable[0][01747];
		REFSoct[13] = mcc->cm->agc.vagc.Erasable[0][01750];
		REFSoct[14] = mcc->cm->agc.vagc.Erasable[0][01751];
		REFSoct[15] = mcc->cm->agc.vagc.Erasable[0][01752];
		REFSoct[16] = mcc->cm->agc.vagc.Erasable[0][01753];
		REFSoct[17] = mcc->cm->agc.vagc.Erasable[0][01754];
		REFSoct[18] = mcc->cm->agc.vagc.Erasable[0][01755];
		REFSoct[19] = mcc->cm->agc.vagc.Erasable[0][01756];
		for (int i = 2; i < 20; i++)
		{
			sprintf(Buffer, "%05o", REFSoct[i]);
			REFSMMAToct[i] = atoi(Buffer);
		}

		REFSMMAT.m11 = OrbMech::DecToDouble(REFSoct[2], REFSoct[3])*2.0;
		REFSMMAT.m12 = OrbMech::DecToDouble(REFSoct[4], REFSoct[5])*2.0;
		REFSMMAT.m13 = OrbMech::DecToDouble(REFSoct[6], REFSoct[7])*2.0;
		REFSMMAT.m21 = OrbMech::DecToDouble(REFSoct[8], REFSoct[9])*2.0;
		REFSMMAT.m22 = OrbMech::DecToDouble(REFSoct[10], REFSoct[11])*2.0;
		REFSMMAT.m23 = OrbMech::DecToDouble(REFSoct[12], REFSoct[13])*2.0;
		REFSMMAT.m31 = OrbMech::DecToDouble(REFSoct[14], REFSoct[15])*2.0;
		REFSMMAT.m32 = OrbMech::DecToDouble(REFSoct[16], REFSoct[17])*2.0;
		REFSMMAT.m33 = OrbMech::DecToDouble(REFSoct[18], REFSoct[19])*2.0;
	}
	return REFSMMAT;
}

void RTCC::navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt)
{
	//R and V in the BRCS

	MATRIX3 Rot, Rot2;
	VECTOR3 Requ, Recl, u;
	double sinl, a, b, gamma,r_0;

	a = 6378166;
	b = 6356784;

	Rot = OrbMech::J2000EclToBRCS(40222.525);
	Rot2 = OrbMech::GetRotationMatrix2(gravref, MJD);

	Recl = tmul(Rot, R);
	Recl = _V(Recl.x, Recl.z, Recl.y);

	Requ = tmul(Rot2, Recl);
	Requ = _V(Requ.x, Requ.z, Requ.y);

	u = unit(Requ);
	sinl = u.z;

	if (gravref == oapiGetObjectByName("Earth"))
	{
		gamma = b*b / a / a;
	}
	else
	{
		gamma = 1;
	}
	r_0 = oapiGetSize(gravref);

	lat = atan(u.z/(gamma*sqrt(u.x*u.x + u.y*u.y)));
	lng = atan2(u.y, u.x);
	alt = length(Requ) - r_0;
}

void RTCC::StateVectorCalc(VESSEL *vessel, double &SVGET, VECTOR3 &BRCSPos, VECTOR3 &BRCSVel)
{
	VECTOR3 R, V, R0B, V0B, R1B, V1B;
	MATRIX3 Rot;
	double SVMJD, dt, GET, GETbase;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();
	GETbase = SVMJD - GET / 24.0 / 3600.0;

	Rot = OrbMech::J2000EclToBRCS(40222.525);

	R0B = mul(Rot, _V(R.x, R.z, R.y));
	V0B = mul(Rot, _V(V.x, V.z, V.y));

	if (SVGET != 0.0)
	{
		dt = SVGET - (SVMJD - GETbase)*24.0*3600.0;
		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);
	}
	else
	{
		SVGET = (SVMJD - GETbase)*24.0*3600.0;
		R1B = R0B;
		V1B = V0B;
	}

	//oapiGetPlanetObliquityMatrix(gravref, &obl);
	//convmat = mul(_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0),mul(transpose_matrix(obl),_M(1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0)));

	BRCSPos = R1B;
	BRCSVel = V1B;
}

OBJHANDLE RTCC::AGCGravityRef(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 rsph;

	gravref = oapiGetObjectByName("Moon");
	vessel->GetRelativePos(gravref, rsph);
	if (length(rsph) > 64373760.0)
	{
		gravref = oapiGetObjectByName("Earth");
	}
	return gravref;
}