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
#include "soundlib.h"
#include "ioChannels.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "sivb.h"
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "../src_rtccmfd/LMGuidanceSim.h"
#include "mcc.h"
#include "rtcc.h"

// SCENARIO FILE MACROLOGY
#define SAVE_BOOL(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_INT(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_DOUBLE(KEY,VALUE) papiWriteScenario_double(scn, KEY, VALUE)
#define SAVE_V3(KEY,VALUE) papiWriteScenario_vec(scn, KEY, VALUE)
#define SAVE_M3(KEY,VALUE) papiWriteScenario_mx(scn, KEY, VALUE)
#define SAVE_STRING(KEY,VALUE) oapiWriteScenario_string(scn, KEY, VALUE)
#define LOAD_BOOL(KEY,VALUE) if(strnicmp(line, KEY, strlen(KEY)) == 0){ sscanf(line + strlen(KEY), "%i", &tmp); if (tmp == 1) { VALUE = true; } else { VALUE = false; } }
#define LOAD_INT(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%i",&VALUE); }
#define LOAD_DOUBLE(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf",&VALUE); }
#define LOAD_V3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf",&VALUE.x,&VALUE.y,&VALUE.z); }
#define LOAD_M3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&VALUE.m11,&VALUE.m12,&VALUE.m13,&VALUE.m21,&VALUE.m22,&VALUE.m23,&VALUE.m31,&VALUE.m32,&VALUE.m33); }
#define LOAD_STRING(KEY,VALUE,LEN) if(strnicmp(line,KEY,strlen(KEY))==0){ strncpy(VALUE, line + (strlen(KEY)+1), LEN); }

SpaceDigitals::SpaceDigitals()
{
	WEIGHT = 0.0;
	GMTV = 0.0;
	GETV = 0.0;
	GETAxis = 0.0;
	GETR = 0.0;
	GET = 0.0;
	sprintf(REF, "");
	V = 0.0;
	PHI = 0.0;
	H = 0.0;
	ADA = 0.0;
	GAM = 0.0;
	LAM = 0.0;
	PSI = 0.0;
	GETVector1 = 0.0;
	sprintf(REF1, "");
	GETA = 0.0;
	HA = 0.0;
	HP = 0.0;
	H1 = 0.0;
	V1 = 0.0;
	GAM1 = 0.0;
	PSI1 = 0.0;
	PHI1 = 0.0;
	LAM1 = 0.0;
	HS = 0.0;
	HO = 0.0;
	PHIO = 0.0;
	IEMP = 0.0;
	W1 = 0.0;
	OMG = 0.0;
	PRA = 0.0;
	A1 = 0.0;
	L1 = 0.0;
	E1 = 0.0;
	I1 = 0.0;
	GETVector2 = 0.0;
	GETSI = 0.0;
	GETCA = 0.0;
	VCA = 0.0;
	HCA = 0.0;
	PCA = 0.0;
	LCA = 0.0;
	PSICA = 0.0;
	GETMN = 0.0;
	HMN = 0.0;
	PMN = 0.0;
	LMN = 0.0;
	DMN = 0.0;
	GETVector3 = 0.0;
	GETSE = 0.0;
	GETEI = 0.0;
	VEI = 0.0;
	GEI = 0.0;
	PEI = 0.0;
	LEI = 0.0;
	PSIEI = 0.0;
	GETVP = 0.0;
	VVP = 0.0;
	HVP = 0.0;
	PVP = 0.0;
	LVP = 0.0;
	PSIVP = 0.0;
	IE = 0.0;
	LN = 0.0;
}

RTCC::RTCC()
{
	mcc = NULL;
	TimeofIgnition = 0.0;
	SplashLatitude = 0.0;
	SplashLongitude = 0.0;
	DeltaV_LVLH = _V(0.0, 0.0, 0.0);
	calcParams.EI = 0.0;
	calcParams.TEI = 0.0;
	calcParams.TLI = 0.0;
	calcParams.R_TLI = _V(0,0,0);
	calcParams.V_TLI = _V(0,0,0);
	calcParams.alt_node = 0.0;
	calcParams.GET_node = 0.0;
	calcParams.lat_node = 0.0;
	calcParams.lng_node = 0.0;
	calcParams.LOI = 0.0;
	calcParams.SEP = 0.0;
	calcParams.DOI = 0.0;
	calcParams.PDI = 0.0;
	calcParams.TLAND = 0.0;
	calcParams.LSAlt = 0.0;
	calcParams.LSAzi = 0.0;
	calcParams.LSLat = 0.0;
	calcParams.LSLng = 0.0;
	calcParams.LunarLiftoff = 0.0;
	calcParams.Insertion = 0.0;
	calcParams.Phasing = 0.0;
	calcParams.CSI = 0.0;
	calcParams.CDH = 0.0;
	calcParams.TPI = 0.0;
	calcParams.src = NULL;
	calcParams.tgt = NULL;
	REFSMMATType = 0;
	calcParams.StoredREFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	calcParams.TEPHEM = 0.0;
	calcParams.PericynthionLatitude = 0.0;
	calcParams.TIGSTORE1 = 0.0;
	calcParams.DVSTORE1 = _V(0, 0, 0);
	calcParams.SVSTORE1.gravref = NULL;
	calcParams.SVSTORE1.mass = 0.0;
	calcParams.SVSTORE1.MJD = 0.0;
	calcParams.SVSTORE1.R = _V(0, 0, 0);
	calcParams.SVSTORE1.V = _V(0, 0, 0);
}

void RTCC::Init(MCC *ptr)
{
	mcc = ptr;
}

void RTCC::AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad)
{
	EarthEntryOpt entopt;
	EntryResults res;
	double v_e, m1, Vc;

	char weather[] = "GOOD";

	v_e = calcParams.src->GetThrusterIsp0(calcParams.src->GetGroupThruster(THGROUP_MAIN, 0));

	entopt.vessel = calcParams.src;
	entopt.GETbase = getGETBase();
	entopt.impulsive = RTCC_NONIMPULSIVE;
	entopt.nominal = RTCC_ENTRY_NOMINAL;
	entopt.ReA = 0;
	entopt.entrylongmanual = true;

	for (int i = 0;i < 8;i++)
	{
		pad.Area[i][0] = 0;
		pad.Wx[i][0] = 0;
	}

	for (int i = 0;i < opt->n;i++)
	{
		entopt.lng = opt->lng[i];
		entopt.TIGguess = opt->GETI[i];

		BlockDataProcessor(&entopt, &res);

		m1 = calcParams.src->GetMass()*exp(-length(res.dV_LVLH) / v_e);
		Vc = length(res.dV_LVLH)*cos(-2.15*RAD)*cos(0.95*RAD);// -60832.18 / m1;

		sprintf(pad.Area[i], opt->area[i].c_str());
		sprintf(pad.Wx[i], weather);

		pad.dVC[i] = Vc/0.3048;
		pad.GETI[i] = res.P30TIG;
		pad.Lat[i] = res.latitude*DEG;
		pad.Lng[i] = res.longitude*DEG;
	}
}

void RTCC::AP11BlockData(AP11BLKOpt *opt, P37PAD &pad)
{
	EntryOpt entopt;
	EntryResults res;

	entopt.entrylongmanual = true;
	entopt.GETbase = getGETBase();
	entopt.impulsive = RTCC_NONIMPULSIVE;
	entopt.ReA = 0;
	entopt.type = RTCC_ENTRY_ABORT;
	entopt.vessel = calcParams.src;

	if (opt->useSV)
	{
		entopt.useSV = true;
		entopt.RV_MCC = opt->RV_MCC;
	}

	for (int i = 0;i < opt->n;i++)
	{
		entopt.TIGguess = opt->GETI[i];
		entopt.lng = opt->lng[i];

		EntryTargeting(&entopt, &res);

		pad.dVT[i] = length(res.dV_LVLH) / 0.3048;
		pad.GET400K[i] = res.GET05G;
		pad.GETI[i] = opt->GETI[i];
		pad.lng[i] = opt->lng[i] * DEG;
	}
}

void RTCC::BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res)
{
	EarthEntry* entry;
	double GET;
	bool stop;
	SV sv;

	stop = false;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}
	GET = (sv.MJD - opt->GETbase)*24.0*3600.0;

	entry = new EarthEntry(sv.R, sv.V, sv.MJD, sv.gravref, opt->GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->nominal, opt->entrylongmanual);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = entry->EntryTIGcor;
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = entry->t2;
	res->GET05G = entry->EntryRET;
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	if (opt->impulsive > 0 )
	{
		bool main;
		double t_slip;
		VECTOR3 UX, UY, UZ, DV, Llambda, RA1_cor, VA1_cor;
		MATRIX3 Q_Xx;
		SV sv1;

		if (opt->impulsive == RTCC_NONIMPULSIVE)
		{
			main = true;
		}
		else
		{
			main = false;
		}

		sv1 = coast(sv, res->P30TIG - GET);

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		DV = UX*res->dV_LVLH.x + UY*res->dV_LVLH.y + UZ*res->dV_LVLH.z;

		FiniteBurntimeCompensation(0, sv1, 0.0, DV, main, Llambda, t_slip);

		OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD + (res->P30TIG - GET) / 24.0 / 3600.0, t_slip, RA1_cor, VA1_cor, sv1.gravref, sv1.gravref);

		Q_Xx = OrbMech::LVLH_Matrix(RA1_cor, VA1_cor);

		res->dV_LVLH = mul(Q_Xx, Llambda);
		res->P30TIG += t_slip;
	}
}

void RTCC::EntryTargeting(EntryOpt *opt, EntryResults *res)
{
	Entry* entry;
	double GET;
	bool stop;
	SV sv;

	stop = false;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}
	GET = (sv.MJD - opt->GETbase)*24.0*3600.0;

	entry = new Entry(sv.R, sv.V, sv.MJD, sv.gravref, opt->GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->type, opt->entrylongmanual);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = entry->EntryTIGcor;
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = entry->t2;
	res->GET05G = entry->EntryRET;
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	if (opt->impulsive == RTCC_NONIMPULSIVE)
	{
		double t_slip, LMmass;
		VECTOR3 UX, UY, UZ, DV, Llambda, RA1_cor, VA1_cor;
		MATRIX3 Q_Xx;
		SV sv1;

		sv1 = coast(sv, res->P30TIG - GET);

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		DV = UX*res->dV_LVLH.x + UY*res->dV_LVLH.y + UZ*res->dV_LVLH.z;

		if (opt->csmlmdocked)
		{
			LMmass = GetDockedVesselMass(opt->vessel);
		}
		else
		{
			LMmass = 0.0;
		}

		FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV, true, Llambda, t_slip);

		OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD + (res->P30TIG - GET) / 24.0 / 3600.0, t_slip, RA1_cor, VA1_cor, sv1.gravref, sv1.gravref);

		Q_Xx = OrbMech::LVLH_Matrix(RA1_cor, VA1_cor);

		res->dV_LVLH = mul(Q_Xx, Llambda);
		res->P30TIG += t_slip;
	}
}

void RTCC::LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res)
{
	SV sv_A1, sv_A1_apo, sv_A2, sv_P2;
	double dt1, dt1_apo, dt2, mu;
	OBJHANDLE gravref;
	bool prograde;

	gravref = lambert->sv_A.gravref;

	if (gravref == oapiGetObjectByName("Earth"))	//Hardcoded: Always prograde for Earth, always retrograde for Moon
	{
		prograde = true;
	}
	else
	{
		prograde = false;
	}

	mu = GGRAV*oapiGetMass(gravref);

	dt1 = lambert->T1 - (lambert->sv_A.MJD - lambert->GETbase) * 24.0 * 60.0 * 60.0;
	dt1_apo = lambert->T1 - (lambert->sv_P.MJD - lambert->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = lambert->T2 - lambert->T1;

	if (lambert->Perturbation == 1)
	{
		sv_A1 = coast(lambert->sv_A, dt1);
		sv_P2 = coast(lambert->sv_P, dt1_apo + dt2);
	}
	else
	{
		OrbMech::rv_from_r0v0(lambert->sv_A.R, lambert->sv_A.V, dt1, sv_A1.R, sv_A1.V, mu);
		OrbMech::rv_from_r0v0(lambert->sv_P.R, lambert->sv_P.V, dt1_apo + dt2, sv_P2.R, sv_P2.V, mu);
	}

	VECTOR3 RP2off, VP2off;
	double angle;

	if (lambert->use_XYZ_Offset)
	{
		angle = lambert->Offset.x / length(sv_P2.R);
	}
	else
	{
		angle = lambert->PhaseAngle;
	}

	OrbMech::rv_from_r0v0_ta(sv_P2.R, sv_P2.V, angle, RP2off, VP2off, mu);

	VECTOR3 i, j, k, VA1_apo;
	MATRIX3 Q_Xx2, Q_Xx;

	k = -unit(RP2off);
	j = unit(crossp(VP2off, RP2off));
	i = crossp(j, k);
	Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

	if (lambert->use_XYZ_Offset)
	{
		RP2off = RP2off + tmul(Q_Xx2, _V(0.0, lambert->Offset.y, lambert->Offset.z));
	}
	else
	{
		RP2off = RP2off + tmul(Q_Xx2, _V(0.0, 0.0, lambert->DH));
	}

	if (lambert->Perturbation == RTCC_LAMBERT_PERTURBED)
	{
		VA1_apo = OrbMech::Vinti(sv_A1.R, sv_A1.V, RP2off, sv_A1.MJD, dt2, lambert->N, prograde, gravref, gravref, gravref, _V(0.0, 0.0, 0.0)); //Vinti Targeting: For non-spherical gravity
	
		sv_A1_apo = sv_A1;
		sv_A1_apo.V = VA1_apo;
		sv_A2 = coast(sv_A1_apo, dt2);
	}
	else
	{
		if (lambert->axis == RTCC_LAMBERT_MULTIAXIS)
		{
			VA1_apo = OrbMech::elegant_lambert(sv_A1.R, sv_A1.V, RP2off, dt2, lambert->N, prograde, mu);	//Lambert Targeting
		}
		else
		{
			OrbMech::xaxislambert(sv_A1.R, sv_A1.V, RP2off, dt2, lambert->N, prograde, mu, VA1_apo, lambert->Offset.z);	//Lambert Targeting
		}

		sv_A1_apo = sv_A1;
		sv_A1_apo.V = VA1_apo;
		sv_A2 = sv_A1_apo;
		OrbMech::rv_from_r0v0(sv_A1_apo.R, sv_A1_apo.V, dt2, sv_A2.R, sv_A2.V, mu);
	}

	Q_Xx = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);
	res.dV = VA1_apo - sv_A1.V;
	res.dV_LVLH = mul(Q_Xx, res.dV);

	if (lambert->axis == RTCC_LAMBERT_XAXIS)
	{
		res.dV_LVLH.y = 0.0;
	}

	if (lambert->NCC_NSR_Flag)
	{
		SV sv_A2_apo;
		VECTOR3 u, R_A2, V_A2, R_PC, V_PC, DV;
		double dt_TPI;

		sv_A2_apo = sv_A2;

		u = unit(crossp(sv_P2.R, sv_P2.V));

		R_A2 = unit(sv_A2.R - u * dotp(sv_A2.R, u))*length(sv_A2.R);
		V_A2 = unit(sv_A2.V - u * dotp(sv_A2.V, u))*length(sv_A2.V);

		OrbMech::RADUP(sv_P2.R, sv_P2.V, R_A2, mu, R_PC, V_PC);
		DV = OrbMech::CoellipticDV(R_A2, R_PC, V_PC, mu) - V_A2;
		sv_A2_apo.V += DV;

		dt_TPI = OrbMech::findelev(sv_A2_apo.R, sv_A2_apo.V, sv_P2.R, sv_P2.V, sv_P2.MJD, lambert->Elevation, gravref);
		res.t_TPI = OrbMech::GETfromMJD(sv_P2.MJD, lambert->GETbase) + dt_TPI;
	}
}

void RTCC::LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step)
{
	double bt, dv_10p, mass40p, bt_40p, dv_40p, mass100p, bt_100p, time_10p, v_G, F_L, K4, mintime, t_throttleup;
	bool S_TH;

	F_L = 200.0*4.448222;
	K4 = 1050.0*4.448222;

	mintime = 4.0;
	t_throttleup = 26.0;

	v_G = dV_LVLH - 4.0*F_L / mass;
	if (v_G*mass / K4 < 95.0)
	{
		//LGC won't throttle up to 100% at 26 seconds
		S_TH = true;
	}
	else
	{
		//LGC will throttle up to 100% at 26 seconds
		S_TH = false;
	}

	if (mass > 30000.0)
	{
		//heavy docked burn, throttle up to 40% at 5 seconds
		time_10p = 5.0;
	}
	else
	{
		//undocked or light docked burn, throttle up to 40% at 15 seconds
		time_10p = 15.0;
	}

	//Throttle Profile:
	//Basic principle: spend at least 4 seconds at one throttle setting before cutoff
	//Burntime at 10% smaller than 21 seconds: full burn at 10%
	//

	bt = v_e / (0.1*F) *(mass)*(1.0 - exp(-dV_LVLH / v_e)); //burn time at 10% thrust

	if (bt > time_10p + mintime * 0.4 / 0.1)	//Burn longer than 4 seconds at 40%
	{
		dv_10p = v_e * log((mass) / (mass - 0.1*F / v_e * time_10p)); //Five seconds at 10%
		mass40p = mass - 0.1*F / v_e * time_10p;
		bt_40p = v_e / (0.4*F) *(mass40p)*(1.0 - exp(-(dV_LVLH - dv_10p) / v_e)); // rest burn time at 40% thrust

		if (S_TH == false)	//LGC will throttle up to 100% at 26 seconds
		{
			dv_40p = v_e * log((mass40p) / (mass40p - 0.4*F / v_e * (t_throttleup - time_10p))); //21 seconds at 40%
			mass100p = mass40p - 0.4*F / v_e * (t_throttleup - time_10p);
			bt_100p = v_e / F * (mass100p)*(1.0 - exp(-(dV_LVLH - dv_10p - dv_40p) / v_e)); // rest burn time at 100% thrust

			F_average = (0.1 * time_10p + 0.4*(t_throttleup - time_10p) + bt_100p) / (t_throttleup + bt_100p)*F;
			ManPADBurnTime = bt_100p + 26.0;
			bt_var = bt_100p;

			step = 2;
		}
		else
		{
			F_average = (0.1 * time_10p + 0.4*bt_40p) / (bt_40p + time_10p)*F;
			ManPADBurnTime = bt_40p + time_10p;
			bt_var = bt_40p;

			step = 1;
		}
	}
	else
	{
		F_average = 0.1*F;	//Full burn at 10%
		ManPADBurnTime = bt;
		bt_var = bt;

		step = 0;
	}
}

void RTCC::AP10CSIPAD(AP10CSIPADOpt *opt, AP10CSI &pad)
{
	SV sv1, sv2;
	MATRIX3 Q_Xx, M, M_R;
	VECTOR3 V_G, UX, UY, UZ, IMUangles, FDAIangles, dV_AGS;
	double dt, TIG_AGS;

	dt = opt->t_CSI - (opt->sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(opt->sv0, dt);

	PoweredFlightProcessor(sv1, opt->GETbase, opt->t_CSI, RTCC_VESSELTYPE_LM, opt->enginetype, 0.0, opt->dV_LVLH, true, TIG_AGS, dV_AGS, false);
	Q_Xx = OrbMech::LVLH_Matrix(sv1.R, sv1.V);
	V_G = tmul(Q_Xx, opt->dV_LVLH);

	UX = unit(V_G);

	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.DEDA373 = (opt->t_CSI - opt->KFactor) / 60.0;
	pad.DEDA275 = (opt->t_TPI - opt->KFactor) / 60.0;
	pad.PLM_FDAI = FDAIangles.y*DEG;
	pad.t_CSI = opt->t_CSI;
	pad.t_TPI = opt->t_TPI;
	pad.dV_LVLH = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = dV_AGS / 0.3048;
}

void RTCC::AP11LMManeuverPAD(AP11LMManPADOpt *opt, AP11LMMNV &pad)
{
	MATRIX3 M_R, M, Q_Xx;
	VECTOR3 V_G, X_B, UX, UY, UZ, IMUangles, FDAIangles;
	double dt, mu, CSMmass, F, v_e, F_average, ManPADBurnTime, bt, apo, peri, ManPADApo, ManPADPeri, ManPADDVR, ManBSSpitch, ManBSSXPos, R_E, headsswitch;
	int step, ManCOASstaroct;
	SV sv, sv1, sv2;

	//State vector from PAD options or get a new one
	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	//Coast until TIG
	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv, dt);

	//Docked mass
	if (opt->csmlmdocked == true)
	{
		CSMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		CSMmass = 0.0;
	}

	//Engine parameters
	if (opt->enginetype == RTCC_ENGINETYPE_RCS)
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}
	else if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		v_e = DPS_ISP;
		F = DPS_THRUST;
	}
	else
	{
		v_e = APS_ISP;
		F = APS_THRUST;
	}

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		//Estimates for average thrust (relevant for finite burntime compensation), complete and variable burntime
		LMThrottleProgram(F, v_e, CSMmass + sv1.mass, length(opt->dV_LVLH), F_average, ManPADBurnTime, bt, step);
	}
	else
	{
		F_average = F;
		ManPADBurnTime = v_e / F * (sv1.mass + CSMmass)*(1.0 - exp(-length(opt->dV_LVLH) / v_e));
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, CSMmass, Q_Xx, V_G, F_average, v_e);

	mu = GGRAV*oapiGetMass(sv1.gravref);
	//Only use landing site radius for the Moon
	if (sv1.gravref == oapiGetObjectByName("Moon"))
	{
		R_E = oapiGetSize(sv1.gravref) + opt->alt;
	}
	else
	{
		R_E = oapiGetSize(sv1.gravref);
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = unit(V_G);
	if (opt->enginetype == RTCC_ENGINETYPE_RCS && opt->directiontype == RTCC_DIRECTIONTYPE_MINUSX)
	{
		UX = -X_B;
	}
	else
	{
		UX = X_B;
	}
	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));


	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);


	ManPADDVR = length(opt->dV_LVLH);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));
	pad.IMUAtt = IMUangles;

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), ManCOASstaroct, ManBSSpitch, ManBSSXPos);
	
	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.burntime = ManPADBurnTime;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = mul(Q_Xx, V_G) / 0.3048;
	pad.GETI = opt->TIG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	pad.SPA = ManBSSpitch*DEG;
	pad.SXP = ManBSSXPos*DEG;
	pad.dVR = ManPADDVR / 0.3048;
	pad.CSMWeight = CSMmass / 0.45359237;
	pad.LMWeight = sv1.mass / 0.45359237;
}

void RTCC::AP11ManeuverPAD(AP11ManPADOpt *opt, AP11MNV &pad)
{
	MATRIX3 Q_Xx, M_R, M, M_RTM;
	VECTOR3 V_G, X_B, UX, UY, UZ, IMUangles, GDCangles;
	double dt, LMmass, F, v_e, headsswitch, mu, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim, p_T, y_T, ManPADDVC, ManPADBurnTime;
	double Mantrunnion, Manshaft, ManBSSpitch, ManBSSXPos, R_E;
	int GDCset, Manstaroct, ManCOASstaroct;
	SV sv, sv1, sv2;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	sv1 = coast(sv, dt);

	if (opt->vesseltype == 1)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		v_e = SPS_ISP;
		F = SPS_THRUST;
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, LMmass, Q_Xx, V_G, F, v_e);

	mu = GGRAV*oapiGetMass(sv1.gravref);

	//Only use landing site radius for the Moon
	if (sv1.gravref == oapiGetObjectByName("Moon"))
	{
		R_E = oapiGetSize(sv1.gravref) + opt->alt;
	}
	else
	{
		R_E = oapiGetSize(sv1.gravref);
	}

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		CalcSPSGimbalTrimAngles(sv1.mass, LMmass, p_T, y_T);

		ManPADPTrim = p_T + 2.15*RAD;
		ManPADYTrim = y_T - 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, sv1.R*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::tmat(M_R), M);

		ManPADDVC = length(opt->dV_LVLH)*cos(p_T)*cos(y_T);// -60832.18 / m1;
	}
	else
	{
		ManPADPTrim = 0.0;
		ManPADYTrim = 0.0;

		X_B = unit(V_G);
		if (opt->directiontype == RTCC_DIRECTIONTYPE_PLUSX)
		{
			UX = X_B;
			ManPADDVC = length(opt->dV_LVLH);
		}
		else
		{
			UX = -X_B;
			ManPADDVC = -length(opt->dV_LVLH);
		}
		UY = unit(crossp(UX, sv1.R*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	}

	ManPADBurnTime = v_e / F *(sv1.mass + LMmass)*(1.0 - exp(-length(opt->dV_LVLH) / v_e));

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	GDCangles = OrbMech::backupgdcalignment(opt->REFSMMAT, sv1.R, oapiGetSize(sv1.gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), Manstaroct, Mantrunnion, Manshaft);

	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), ManCOASstaroct, ManBSSpitch, ManBSSXPos);

	pad.Att = _V(OrbMech::imulimit(IMUangles.x*DEG), OrbMech::imulimit(IMUangles.y*DEG), OrbMech::imulimit(IMUangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.burntime = ManPADBurnTime;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.GDCangles = _V(OrbMech::imulimit(GDCangles.x*DEG), OrbMech::imulimit(GDCangles.y*DEG), OrbMech::imulimit(GDCangles.z*DEG));
	pad.GETI = opt->TIG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		sprintf(pad.PropGuid, "SPS/G&N");
	}
	else
	{
		sprintf(pad.PropGuid, "RCS/G&N");
	}

	pad.pTrim = ManPADPTrim*DEG;
	pad.yTrim = ManPADYTrim*DEG;

	if (length(GDCangles) == 0.0)
	{
		sprintf(pad.SetStars, "N/A");
	}
	else
	{
		if (GDCset == 0)
		{
			sprintf(pad.SetStars, "Vega, Deneb");
		}
		else if (GDCset == 1)
		{
			sprintf(pad.SetStars, "Navi, Polaris");
		}
		else
		{
			sprintf(pad.SetStars, "Acrux, Atria");
		}
	}
	pad.Shaft = Manshaft*DEG;
	pad.SPA = ManBSSpitch*DEG;
	pad.Star = Manstaroct;
	pad.SXP = ManBSSXPos*DEG;
	pad.Trun = Mantrunnion*DEG;
	pad.Vc = ManPADDVC / 0.3048;
	pad.Vt = length(opt->dV_LVLH) / 0.3048;
	pad.Weight = sv1.mass / 0.45359237;
	pad.LMWeight = LMmass / 0.45359237;

	pad.GET05G = 0;
	pad.lat = 0;
	pad.lng = 0;
	pad.RTGO = 0;
}

void RTCC::AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad)
{
	MATRIX3 Q_Xx, M, M_R, M_RTM;
	VECTOR3 V_G, X_B, UX, UY, UZ, Att;
	double dt, LMmass, v_e, F, mu, headsswitch, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim, y_T, p_T;
	SV sv, sv1, sv2;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv, dt);

	if (opt->vesseltype == 1)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		v_e = SPS_ISP;
		F = SPS_THRUST;
	}
	else
	{
		v_e = 2706.64;
		F = 400 * 4.448222;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, LMmass, Q_Xx, V_G, F, v_e);

	mu = GGRAV*oapiGetMass(sv1.gravref);

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - oapiGetSize(sv2.gravref);
	ManPADPeri = peri - oapiGetSize(sv2.gravref);

	pad.Weight = sv1.mass / 0.45359237;

	pad.burntime = v_e / F *(sv1.mass + LMmass)*(1.0 - exp(-length(opt->dV_LVLH) / v_e));

	if (opt->enginetype == RTCC_ENGINETYPE_SPSDPS)
	{
		CalcSPSGimbalTrimAngles(sv1.mass, LMmass, p_T, y_T);

		ManPADPTrim = p_T + 2.15*RAD;
		ManPADYTrim = y_T - 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, sv1.R*headsswitch));
		UZ = unit(crossp(X_B, UY));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::tmat(M_R), M);

		pad.Vc = length(opt->dV_LVLH)*cos(p_T)*cos(y_T);// -60832.18 / m1;
	}
	else
	{
		ManPADPTrim = 0.0;
		ManPADYTrim = 0.0;

		X_B = unit(V_G);
		if (opt->directiontype == RTCC_DIRECTIONTYPE_PLUSX)
		{
			UX = X_B;
			pad.Vc = length(opt->dV_LVLH);
		}
		else
		{
			UX = -X_B;
			pad.Vc = -length(opt->dV_LVLH);
		}
		UY = unit(crossp(UX, sv1.R*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	}
	Att = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	//GDCangles = OrbMech::backupgdcalignment(REFSMMAT, R1B, oapiGetSize(gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(round(Att.x*DEG)*RAD, round(Att.y*DEG)*RAD, round(Att.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), pad.Star, pad.Trun, pad.Shaft);

	if (opt->navcheckGET != 0.0)
	{
		VECTOR3 Rnav, Vnav;
		double alt, lat, lng;
		OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->navcheckGET - opt->TIG, Rnav, Vnav, sv1.gravref, sv1.gravref);
		navcheck(Rnav, Vnav, opt->GETbase + opt->navcheckGET / 24.0 / 3600.0, sv1.gravref, lat, lng, alt);

		pad.NavChk = opt->navcheckGET;
		pad.lat = lat*DEG;
		pad.lng = lng*DEG;
		pad.alt = alt / 1852;
	}

	pad.Att = _V(OrbMech::imulimit(Att.x*DEG), OrbMech::imulimit(Att.y*DEG), OrbMech::imulimit(Att.z*DEG));

	pad.GETI = opt->TIG;
	pad.pTrim = ManPADPTrim*DEG;
	pad.yTrim = ManPADYTrim*DEG;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.Vc /= 0.3048;
	pad.Shaft *= DEG;
	pad.Trun *= DEG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;
}

void RTCC::AP7TPIPAD(AP7TPIPADOpt *opt, AP7TPI &pad)
{
	double mu, dt, SVMJD;
	VECTOR3 R_A, V_A, R0B, V0B, RA3, VA3, R_P, V_P, RP0B, VP0B, RP3, VP3, u, U_L, UX, UY, UZ, U_R, U_R2, RA2, VA2, RP2, VP2, U_P, TPIPAD_BT, TPIPAD_dV_LOS;
	MATRIX3 Rot1, Rot2;
	double TPIPAD_AZ, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5,TPIPAD_dH, TPIPAD_ddH;
	VECTOR3 U_F, LOS, U_LOS, NN;

	OBJHANDLE gravref;

	gravref = AGCGravityRef(opt->vessel);

	mu = GGRAV*oapiGetMass(gravref);

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	opt->target->GetRelativePos(gravref, R_P);
	opt->target->GetRelativeVel(gravref, V_P);
	SVMJD = oapiGetSimMJD();

	dt = opt->TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	R0B = _V(R_A.x, R_A.z, R_A.y);
	V0B = _V(V_A.x, V_A.z, V_A.y);
	RP0B = _V(R_P.x, R_P.z, R_P.y);
	VP0B = _V(V_P.x, V_P.z, V_P.y);

	OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, RA3, VA3, gravref, gravref);
	OrbMech::oneclickcoast(RP0B, VP0B, SVMJD, dt, RP3, VP3, gravref, gravref);

	UY = unit(crossp(VA3, RA3));
	UZ = unit(-RA3);
	UX = crossp(UY, UZ);

	Rot1 = _M(UX.x, UY.x, UZ.x, UX.y, UY.y, UZ.y, UX.z, UY.z, UZ.z);

	u = unit(crossp(RA3, VA3));
	U_L = unit(RP3 - RA3);
	UX = U_L;
	UY = unit(crossp(crossp(u, UX), UX));
	UZ = crossp(UX, UY);

	Rot2 = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);

	TPIPAD_dV_LOS = mul(Rot2, mul(Rot1, opt->dV_LVLH));
	//TPIPAD_dH = abs(length(RP3) - length(RA3));
	double mass, F;

	mass = opt->vessel->GetMass();
	F = 200.0 * 4.448222;
	TPIPAD_BT = _V(abs(0.5*TPIPAD_dV_LOS.x), abs(TPIPAD_dV_LOS.y), abs(TPIPAD_dV_LOS.z))*mass / F;

	VECTOR3 i, j, k, dr, dv, dr0, dv0, Omega;
	MATRIX3 Q_Xx;
	double t1, t2, dxmin, n, dxmax;

	j = unit(VP3);
	k = unit(crossp(RP3, j));
	i = crossp(j, k);
	Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	dr = RA3 - RP3;
	n = length(VP3) / length(RP3);
	Omega = k*n;
	dv = VA3 - VP3 - crossp(Omega, dr);
	dr0 = mul(Q_Xx, dr);
	dv0 = mul(Q_Xx, dv);
	t1 = 1.0 / n*atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y));
	t2 = 1.0 / n*(atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y)) + PI);
	dxmax = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t1) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t1);
	dxmin = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t2) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t2);

	OrbMech::oneclickcoast(RA3, VA3, SVMJD + dt / 24.0 / 3600.0, -5.0*60.0, RA2, VA2, gravref, gravref);
	OrbMech::oneclickcoast(RP3, VP3, SVMJD + dt / 24.0 / 3600.0, -5.0*60.0, RP2, VP2, gravref, gravref);

	U_R2 = unit(RP2 - RA2);

	TPIPAD_dH = -dr0.x;
	TPIPAD_ddH = abs(dxmax - dxmin);
	TPIPAD_R = abs(length(RP2 - RA2));
	TPIPAD_Rdot = dotp(VP2 - VA2, U_R2);

	U_L = unit(RP2 - RA2);
	U_P = unit(U_L - RA2*dotp(U_L, RA2) / length(RA2) / length(RA2));

	TPIPAD_ELmin5 = acos(dotp(U_L, U_P*OrbMech::sign(dotp(U_P, crossp(u, RA2)))));

	U_F = unit(crossp(crossp(RA2, VA2), RA2));
	U_R = unit(RA2);
	LOS = RP2 - RA2;
	U_LOS = unit(LOS - U_R*dotp(LOS, U_R));
	TPIPAD_AZ = acos(dotp(U_LOS, U_F));//atan2(-TPIPAD_dV_LOS.z, TPIPAD_dV_LOS.x);
	NN = crossp(U_LOS, U_F);
	if (dotp(NN, RA2) < 0)
	{
		TPIPAD_AZ = PI2 - TPIPAD_AZ;
	}

	pad.AZ = TPIPAD_AZ*DEG;
	pad.Backup_bT = TPIPAD_BT;
	pad.Backup_dV = TPIPAD_dV_LOS / 0.3048;
	pad.R = TPIPAD_R / 1852.0;
	pad.Rdot = TPIPAD_Rdot / 0.3048;
	pad.EL = TPIPAD_ELmin5*DEG;
	pad.GETI = opt->TIG;
	pad.Vg = opt->dV_LVLH / 0.3048;
	pad.dH_TPI = TPIPAD_dH / 1852.0;
	pad.dH_Max = TPIPAD_ddH / 1852.0;
}

void RTCC::AP9LMTPIPAD(AP9LMTPIPADOpt *opt, AP9LMTPI &pad)
{
	SV sv_A1, sv_P1;
	MATRIX3 Rot1, Rot2, M;
	VECTOR3 u, U_L, dV_LOS, IMUangles, FDAIangles, R1, R2, R3;
	double R, Rdot;

	sv_A1 = coast(opt->sv_A, opt->TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, opt->GETbase));
	sv_P1 = coast(opt->sv_P, opt->TIG - OrbMech::GETfromMJD(opt->sv_P.MJD, opt->GETbase));
	Rot1 = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);

	u = unit(crossp(sv_A1.R, sv_A1.V));
	U_L = unit(sv_P1.R - sv_A1.R);
	R1 = U_L;
	R2 = unit(crossp(crossp(u, R1), R1));
	R3 = crossp(R1, R2);
	Rot2 = _M(R1.x, R1.y, R1.z, R2.x, R2.y, R2.z, R3.x, R3.y, R3.z);

	dV_LOS = mul(Rot2, tmul(Rot1, opt->dV_LVLH));

	R = abs(length(sv_P1.R - sv_A1.R));
	Rdot = dotp(sv_P1.V - sv_A1.V, U_L);

	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), Rot1));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.Backup_dV = dV_LOS / 0.3048;
	pad.dVR = length(opt->dV_LVLH) / 0.3048;
	pad.GETI = opt->TIG;
	pad.R = R / 1852.0;
	pad.Rdot = Rdot / 0.3048;
	pad.Vg = opt->dV_LVLH / 0.3048;
}

void RTCC::AP9LMCDHPAD(AP9LMCDHPADOpt *opt, AP9LMCDH &pad)
{
	SV sv_A1;
	MATRIX3 Rot1, M;
	VECTOR3 IMUangles, FDAIangles, V_G;

	sv_A1 = coast(opt->sv_A, opt->TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, opt->GETbase));
	Rot1 = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);
	V_G = tmul(Rot1, opt->dV_LVLH);

	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), Rot1));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Pitch = OrbMech::imulimit(FDAIangles.y*DEG);
	pad.Vg_AGS = mul(Rot1, V_G) / 0.3048;
	pad.GETI = opt->TIG;
	pad.Vg = opt->dV_LVLH / 0.3048;
}

void RTCC::CSMDAPUpdate(VESSEL *v, AP10DAPDATA &pad)
{
	double CSMmass, LMmass, p_T, y_T;

	CSMmass = v->GetMass();
	LMmass = GetDockedVesselMass(v);

	CalcSPSGimbalTrimAngles(CSMmass, LMmass, p_T, y_T);

	pad.ThisVehicleWeight = CSMmass / 0.45359237;
	pad.OtherVehicleWeight = LMmass / 0.45359237;
	pad.PitchTrim = p_T * DEG + 2.15;
	pad.YawTrim = y_T * DEG - 0.95;
}

void RTCC::LMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool asc)
{
	double CSMmass, LMmass;

	if (asc)
	{
		if (!stricmp(v->GetClassName(), "ProjectApollo\\LEM") ||
			!stricmp(v->GetClassName(), "ProjectApollo/LEM") ||
			!stricmp(v->GetClassName(), "ProjectApollo\\LEMSaturn") ||
			!stricmp(v->GetClassName(), "ProjectApollo/LEMSaturn")) {
			LEM *lem = (LEM *)v;
			LMmass = lem->GetAscentStageMass();
		}
		else
		{
			LMmass = v->GetMass();
		}
	}
	else
	{
		LMmass = v->GetMass();
	}

	CSMmass = GetDockedVesselMass(v);

	pad.ThisVehicleWeight = LMmass / 0.45359237;
	pad.OtherVehicleWeight = CSMmass / 0.45359237;
	pad.PitchTrim = 6.0;
	pad.YawTrim = 6.0;
}

void RTCC::EarthOrbitEntry(EarthEntryPADOpt *opt, AP7ENT &pad)
{
	double mu, SVMJD, GET, EMSAlt, theta_T, m1,v_e, EIAlt, lat, lng, KTETA;
	double dt;//from SV time to deorbit maneuver
	double t_go; //from deorbit TIG to shutdown
	double dt2; //from shutdown to EI
	double dt3; //from EI to 300k
	double dt4; //from 300k to 0.05g
	VECTOR3 R_A, V_A, R0B, V0B, R1B, V1B, UX, UY, UZ, DV_P, DV_C, V_G, R2B, V2B, R05G, V05G, EIangles, REI, VEI, R300K, V300K;
	VECTOR3 UXD, UYD, UZD;
	MATRIX3 M_R;
	OBJHANDLE gravref;

	double ALFATRIM = -20.0*RAD;

	gravref = AGCGravityRef(opt->vessel);
	mu = GGRAV*oapiGetMass(gravref);

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	GET = (SVMJD - opt->GETbase)*24.0*3600.0;

	R0B = _V(R_A.x, R_A.z, R_A.y);
	V0B = _V(V_A.x, V_A.z, V_A.y);

	EMSAlt = 284643.0*0.3048;
	EIAlt = 400000.0*0.3048;

	KTETA = 1000.0;

	if (opt->preburn)
		{
		double F, m_cut, EntryRTGO, EntryVIO, EntryRET;

		F = SPS_THRUST;
		v_e = SPS_ISP;

		dt = opt->P30TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
		OrbMech::oneclickcoast(R0B, V0B, SVMJD, dt, R1B, V1B, gravref, gravref);

		UY = unit(crossp(V1B, R1B));
		UZ = unit(-R1B);
		UX = crossp(UY, UZ);

		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		theta_T = length(crossp(R1B, V1B))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(R1B), 2.0) / 92100.0;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*opt->dV_LVLH.y;
		OrbMech::poweredflight(R1B, V1B, SVMJD + dt / 24.0 / 3600.0, gravref, F, v_e, opt->vessel->GetMass(), V_G, R2B, V2B, m_cut, t_go);

		dt2 = OrbMech::time_radius_integ(R2B, V2B, SVMJD + (dt + t_go) / 3600.0 / 24.0, oapiGetSize(gravref) + EIAlt, -1, gravref, gravref, REI, VEI);
		dt3 = OrbMech::time_radius_integ(REI, VEI, SVMJD + (dt + t_go + dt2) / 3600.0 / 24.0, oapiGetSize(gravref) + 300000.0*0.3048, -1, gravref, gravref, R300K, V300K);
		dt4 = OrbMech::time_radius_integ(R300K, V300K, SVMJD + (dt + t_go + dt2 + dt3) / 3600.0 / 24.0, oapiGetSize(gravref) + EMSAlt, -1, gravref, gravref, R05G, V05G);

		EntryCalculations::Reentry(REI, VEI, SVMJD + (dt + t_go + dt2) / 3600.0 / 24.0, false, lat, lng, EntryRTGO, EntryVIO, EntryRET);

		UX = unit(-V05G);
		UY = unit(crossp(UX, -R05G));
		UZ = unit(crossp(UX, crossp(UX, -R05G)));

		UYD = UY;
		UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
		UZD = crossp(UXD, UYD);

		M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
		EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

		m1 = opt->vessel->GetMass()*exp(-length(opt->dV_LVLH) / v_e);

		double WIE, WT, theta_rad, LSMJD;
		VECTOR3 RTE, UTR, urh, URT0, URT, R_LS, R_P;
		MATRIX3 Rot2;
		LSMJD = (dt + t_go + dt2 + dt3 + dt4) / 24.0 / 3600.0 + SVMJD;
		R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
		Rot2 = OrbMech::GetRotationMatrix(gravref, LSMJD);
		R_LS = mul(Rot2, R_P);
		//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
		URT0 = R_LS;
		WIE = 72.9211505e-6;
		UZ = _V(0, 0, 1);
		RTE = crossp(UZ, URT0);
		UTR = crossp(RTE, UZ);
		urh = unit(R05G);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
		theta_rad = acos(dotp(URT0, urh));
		for (int i = 0;i < 10;i++)
		{
			WT = WIE*(KTETA*theta_rad);
			URT = URT0 + UTR*(cos(WT) - 1.0) + RTE*sin(WT);
			theta_rad = acos(dotp(URT, urh));
		}

		pad.Att400K[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
		pad.dVTO[0] = 0.0;//-60832.18 / m1 / 0.3048;
		if (opt->lat == 0)
		{
			pad.Lat[0] = lat*DEG;
			pad.Lng[0] = lng*DEG;
		}
		else
		{
			pad.Lat[0] = opt->lat*DEG;
			pad.Lng[0] = opt->lng*DEG;
		}
		pad.Ret05[0] = t_go + dt2 + dt3 + dt4;
		pad.RTGO[0] = theta_rad*3437.7468;//entry->EntryRTGO;
		pad.VIO[0] = EntryVIO / 0.3048;
	}
	else
	{
		double EMSTime, LSMJD, dt5, theta_rad, theta_nm;
		VECTOR3 R_P, R_LS;
		MATRIX3 Rot2;

		dt = opt->P30TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
		dt2 = OrbMech::time_radius_integ(R0B, V0B, SVMJD, oapiGetSize(gravref) + EIAlt, -1, gravref, gravref, REI, VEI);
		dt3 = OrbMech::time_radius_integ(REI, VEI, SVMJD + dt2 / 24.0 / 3600.0, oapiGetSize(gravref) + 300000.0*0.3048, -1, gravref, gravref, R300K, V300K);
		dt4 = OrbMech::time_radius_integ(R300K, V300K, SVMJD + (dt2 + dt3) / 24.0 / 3600.0, oapiGetSize(gravref) + EMSAlt, -1, gravref, gravref, R05G, V05G);

		UX = unit(-V05G);
		UY = unit(crossp(UX, -R05G));
		UZ = unit(crossp(UX, crossp(UX, -R05G)));

		UYD = UY;
		UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
		UZD = crossp(UXD, UYD);

		M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
		EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

		dt5 = 500.0;
		EMSTime = dt2 + dt3 + dt4 + (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

		/*for (int i = 0;i < 10;i++)
		{
			LSMJD = (EMSTime + dt5) / 24.0 / 3600.0 + opt->GETbase;
			R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
			Rot2 = OrbMech::GetRotationMatrix2(gravref, LSMJD);
			R_LS = mul(Rot2, R_P);
			R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
			theta_rad = acos(dotp(R_LS, unit(R300K)));
			theta_nm = 3437.7468*theta_rad;
			if (length(V300K) >= 26000.0*0.3048)
			{
				dt5 = theta_nm / 3.0;
			}
			else
			{
				dt5 = 8660.0*theta_nm / (length(V300K) / 0.3048);
			}
		}*/
		double WIE, WT;
		VECTOR3 RTE, UTR, urh, URT0, URT;
		LSMJD = EMSTime / 24.0 / 3600.0 + opt->GETbase;
		R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
		Rot2 = OrbMech::GetRotationMatrix(gravref, LSMJD);
		R_LS = mul(Rot2, R_P);
		//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
		URT0 = R_LS;
		WIE = 72.9211505e-6;
		UZ = _V(0, 0, 1);
		RTE = crossp(UZ, URT0);
		UTR = crossp(RTE, UZ);
		urh = unit(R05G);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
		theta_rad = acos(dotp(URT0, urh));
		for (int i = 0;i < 10;i++)
		{
			WT = WIE*(KTETA*theta_rad);
			URT = URT0 + UTR*(cos(WT) - 1.0) + RTE*sin(WT);
			theta_rad = acos(dotp(URT, urh));
		}
		theta_nm = theta_rad*3437.7468;

		pad.PB_RTGO[0] = theta_nm;//-3437.7468*acos(dotp(unit(R300K), unit(R05G)));
		pad.PB_R400K[0] = EIangles.x*DEG;
		pad.PB_Ret05[0] = dt2 + dt3 + dt4 - dt;
		pad.PB_VIO[0] = length(V05G) / 0.3048;
	}
}

void RTCC::LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad)
{
	VECTOR3 R_A, V_A, R0B, V0B, R_P, R_LS, URT0, UUZ, RTE, UTR, urh, URT, UX, UY, UZ, EIangles, UREI;
	MATRIX3 M_R, Rot2;
	double SVMJD, dt, dt2, dt3, EIAlt, Alt300K, EMSAlt, S_FPA, g_T, V_T, v_BAR, RET05, liftline, EntryPADV400k, EntryPADVIO, mu_M;
	double WIE, WT, LSMJD, theta_rad, theta_nm, EntryPADDO, EntryPADGMax, EntryPADgamma400k, EntryPADHorChkGET, EIGET, EntryPADHorChkPit, KTETA;
	OBJHANDLE gravref, hEarth, hMoon;
	SV sv0;		// "Now"
	SV sv1;		// "Now" or just after the maneuver
	SV svEI;	// EI/400K
	SV sv300K;  // 300K
	SV sv05G;   // EMS Altitude / 0.05G

	gravref = AGCGravityRef(opt->vessel);
	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_M = GGRAV*oapiGetMass(hMoon);

	EIAlt = 400000.0*0.3048;
	Alt300K = 300000.0*0.3048;
	EMSAlt = 297431.0*0.3048;

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	R0B = _V(R_A.x, R_A.z, R_A.y);
	V0B = _V(V_A.x, V_A.z, V_A.y);

	sv0.gravref = gravref;
	sv0.MJD = SVMJD;
	sv0.R = R0B;
	sv0.V = V0B;
	sv0.mass = opt->vessel->GetMass();

	if (opt->direct || length(opt->dV_LVLH) == 0.0)	//Check against a DV of 0
	{
		sv1 = sv0;
	}
	else
	{
		sv1 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, 0);
	}

	if (sv1.gravref == hMoon)
	{
		double dt_r = OrbMech::time_radius(sv1.R, sv1.V, 64373760.0, 1.0, mu_M);
		sv1 = coast(sv1, dt_r);
	}

	dt = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, oapiGetSize(hEarth) + EIAlt, -1, sv1.gravref, hEarth, svEI.R, svEI.V);
	svEI.gravref = hEarth;
	svEI.mass = sv1.mass;
	svEI.MJD = sv1.MJD + dt / 24.0 / 3600.0;

	double EntryRET, lat, lng, rtgo, vio;

	EntryCalculations::Reentry(svEI.R, svEI.V, svEI.MJD, true, lat, lng, rtgo, vio, EntryRET);

	LSMJD = svEI.MJD + EntryRET / 24.0 / 3600.0;

	dt2 = OrbMech::time_radius_integ(svEI.R, svEI.V, svEI.MJD, oapiGetSize(hEarth) + Alt300K, -1, hEarth, hEarth, sv300K.R, sv300K.V);
	sv300K.gravref = hEarth;
	sv300K.mass = svEI.mass;
	sv300K.MJD = svEI.MJD + dt2 / 24.0 / 3600.0;

	dt3 = OrbMech::time_radius_integ(sv300K.R, sv300K.V, sv300K.MJD, oapiGetSize(hEarth) + EMSAlt, -1, hEarth, hEarth, sv05G.R, sv05G.V);
	sv05G.gravref = hEarth;
	sv05G.mass = sv300K.mass;
	sv05G.MJD = sv300K.MJD + dt3 / 24.0 / 3600.0;

	EntryPADVIO = length(sv05G.V);

	R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
	Rot2 = OrbMech::GetRotationMatrix(hEarth, LSMJD);
	R_LS = mul(Rot2, R_P);
	//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
	R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
	URT0 = R_LS;
	WIE = 72.9211505e-6;
	KTETA = 1000.0;
	UUZ = _V(0, 0, 1);
	RTE = crossp(UUZ, URT0);
	UTR = crossp(RTE, UUZ);
	urh = unit(sv05G.R);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
	theta_rad = acos(dotp(URT0, urh));
	for (int i = 0;i < 10;i++)
	{
		WT = WIE*(KTETA*theta_rad);
		URT = URT0 + UTR*(cos(WT) - 1.0) + RTE*sin(WT);
		theta_rad = acos(dotp(URT, urh));
	}
	theta_nm = theta_rad*3437.7468;

	UX = unit(-sv05G.V);
	UY = unit(crossp(UX, -sv05G.R));
	UZ = unit(crossp(UX, crossp(UX, -sv05G.R)));

	double ALFATRIM = -20.0*RAD;
	VECTOR3 UXD, UYD, UZD;

	UYD = UY;
	UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
	UZD = crossp(UXD, UYD);

	M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
	EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

	S_FPA = dotp(unit(sv300K.R), sv300K.V) / length(sv300K.V);
	g_T = asin(S_FPA);
	V_T = length(sv300K.V);
	v_BAR = (V_T / 0.3048 - 36000.0) / 20000.0;
	EntryPADGMax = 4.0 / (1.0 + 4.8*v_BAR*v_BAR)*(abs(g_T)*DEG - 6.05 - 2.4*v_BAR*v_BAR) + 10.0;

	UREI = unit(svEI.R);
	EntryPADV400k = length(svEI.V);
	S_FPA = dotp(UREI, svEI.V) / EntryPADV400k;
	EntryPADgamma400k = asin(S_FPA);
	EIGET = (svEI.MJD - opt->GETbase)*24.0*3600.0;
	RET05 = (sv05G.MJD - svEI.MJD)*24.0*3600.0;

	double vei;
	vei = length(svEI.V) / 0.3048;

	EntryPADDO = 4.2317708e-9*vei*vei + 1.4322917e-6*vei - 1.600664062;

	liftline = 4.055351e-10*vei*vei - 3.149125e-5*vei + 0.503280635;
	if (S_FPA > atan(liftline))
	{
		sprintf(pad.LiftVector[0], "DN");
	}
	else
	{
		sprintf(pad.LiftVector[0], "UP");
	}

	SV svHorCheck, svSxtCheck;
	svHorCheck = coast(svEI, -17.0*60.0);
	svSxtCheck = coast(svEI, -60.0*60.0);

	EntryPADHorChkGET = EIGET - 17.0*60.0;

	double Entrytrunnion, Entryshaft, EntryBSSpitch, EntryBSSXPos;
	int Entrystaroct, EntryCOASstaroct;
	OrbMech::checkstar(opt->REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), svSxtCheck.R, oapiGetSize(hEarth), Entrystaroct, Entrytrunnion, Entryshaft);
	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), svSxtCheck.R, oapiGetSize(hEarth), EntryCOASstaroct, EntryBSSpitch, EntryBSSXPos);

	double horang, coastang, IGA, cosIGA, sinIGA;
	VECTOR3 X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, A_MG;

	horang = asin(oapiGetSize(gravref) / length(svHorCheck.R));
	coastang = dotp(unit(svEI.R), unit(svHorCheck.R));

	Z_NB = unit(-svEI.R);
	Y_NB = unit(crossp(svEI.V, svEI.R));
	X_NB = crossp(Y_NB, Z_NB);

	X_SM = _V(opt->REFSMMAT.m11, opt->REFSMMAT.m12, opt->REFSMMAT.m13);
	Y_SM = _V(opt->REFSMMAT.m21, opt->REFSMMAT.m22, opt->REFSMMAT.m23);
	Z_SM = _V(opt->REFSMMAT.m31, opt->REFSMMAT.m32, opt->REFSMMAT.m33);
	A_MG = unit(crossp(X_NB, Y_SM));
	cosIGA = dotp(A_MG, Z_SM);
	sinIGA = dotp(A_MG, X_SM);
	IGA = atan2(sinIGA, cosIGA);

	EntryPADHorChkPit = PI2 - (horang + coastang + 31.7*RAD) + IGA;

	pad.Att05[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
	pad.BSS[0] = EntryCOASstaroct;
	pad.DLMax[0] = 0.0;
	pad.DLMin[0] = 0.0;
	pad.DO[0] = EntryPADDO;
	pad.Gamma400K[0] = EntryPADgamma400k*DEG;
	pad.GETHorCheck[0] = EntryPADHorChkGET;
	pad.Lat[0] = opt->lat*DEG;
	pad.Lng[0] = opt->lng*DEG;
	pad.MaxG[0] = EntryPADGMax;
	pad.PitchHorCheck[0] = OrbMech::imulimit(EntryPADHorChkPit*DEG);
	pad.RET05[0] = RET05;
	pad.RETBBO[0] = 0.0;
	pad.RETDRO[0] = 0.0;
	pad.RETEBO[0] = 0.0;
	pad.RETVCirc[0] = 0.0;
	pad.RRT[0] = EIGET;
	pad.RTGO[0] = theta_nm;
	pad.SXTS[0] = Entrystaroct;
	pad.SFT[0] = Entryshaft*DEG;
	pad.TRN[0] = Entrytrunnion*DEG;
	pad.SPA[0] = EntryBSSpitch*DEG;
	pad.SXP[0] = EntryBSSXPos*DEG;
	pad.V400K[0] = EntryPADV400k / 0.3048;
	pad.VIO[0] = EntryPADVIO / 0.3048;
	pad.VLMax[0] = 0.0;
	pad.VLMin[0] = 0.0;
}

MATRIX3 RTCC::GetREFSMMATfromAGC(agc_t *agc, double AGCEpoch, int addroff)
{
	MATRIX3 REFSMMAT;
	char Buffer[100];
	int REFSMMAToct[20];

	int REFSaddr = 01735 + addroff;

	unsigned short REFSoct[20];
	REFSoct[2] = agc->Erasable[0][REFSaddr];
	REFSoct[3] = agc->Erasable[0][REFSaddr + 1];
	REFSoct[4] = agc->Erasable[0][REFSaddr + 2];
	REFSoct[5] = agc->Erasable[0][REFSaddr + 3];
	REFSoct[6] = agc->Erasable[0][REFSaddr + 4];
	REFSoct[7] = agc->Erasable[0][REFSaddr + 5];
	REFSoct[8] = agc->Erasable[0][REFSaddr + 6];
	REFSoct[9] = agc->Erasable[0][REFSaddr + 7];
	REFSoct[10] = agc->Erasable[0][REFSaddr + 8];
	REFSoct[11] = agc->Erasable[0][REFSaddr + 9];
	REFSoct[12] = agc->Erasable[0][REFSaddr + 10];
	REFSoct[13] = agc->Erasable[0][REFSaddr + 11];
	REFSoct[14] = agc->Erasable[0][REFSaddr + 12];
	REFSoct[15] = agc->Erasable[0][REFSaddr + 13];
	REFSoct[16] = agc->Erasable[0][REFSaddr + 14];
	REFSoct[17] = agc->Erasable[0][REFSaddr + 15];
	REFSoct[18] = agc->Erasable[0][REFSaddr + 16];
	REFSoct[19] = agc->Erasable[0][REFSaddr + 17];
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
	
	return mul(REFSMMAT, OrbMech::J2000EclToBRCS(AGCEpoch));
}

double RTCC::GetClockTimeFromAGC(agc_t *agc)
{
	return agc->Erasable[AGC_BANK(025)][AGC_ADDR(025)] + agc->Erasable[AGC_BANK(024)][AGC_ADDR(024)] * pow((double) 2., (double) 14.);
}

double RTCC::GetTEPHEMFromAGC(agc_t *agc)
{
	return agc->Erasable[AGC_BANK(01710)][AGC_ADDR(01710)] +
		agc->Erasable[AGC_BANK(01707)][AGC_ADDR(01707)] * pow((double) 2., (double) 14.) +
		agc->Erasable[AGC_BANK(01706)][AGC_ADDR(01706)] * pow((double) 2., (double) 28.);
}

void RTCC::navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt)
{
	//R and V in the J2000

	MATRIX3 Rot2;
	VECTOR3 Requ, Recl, u;
	double sinl, a, b, gamma,r_0;

	a = 6378166;
	b = 6356784;

	Rot2 = OrbMech::GetRotationMatrix(gravref, MJD);

	Recl = _V(R.x, R.z, R.y);

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

SV RTCC::StateVectorCalc(VESSEL *vessel, double SVMJD)
{
	VECTOR3 R, V;
	double dt;
	OBJHANDLE gravref;
	SV sv, sv1;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	sv.MJD = oapiGetSimMJD();

	sv.R = _V(R.x, R.z, R.y);
	sv.V = _V(V.x, V.z, V.y);

	sv.gravref = gravref;
	sv.mass = vessel->GetMass();

	if (SVMJD != 0.0)
	{
		dt = (SVMJD - sv.MJD)*24.0*3600.0;
		sv1 = coast(sv, dt);
	}
	else
	{
		sv1 = sv;
	}

	return sv1;
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

double RTCC::getGETBase()
{
	double GET, SVMJD;
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();
	return SVMJD - GET / 24.0 / 3600.0;
}

MATRIX3 RTCC::REFSMMATCalc(REFSMMATOpt *opt)
{
	VECTOR3 UX, UY, UZ;
	OBJHANDLE hMoon, hEarth;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	//Here the options that don't require a state vector or thrust parameters
	if (opt->REFSMMATopt == 9)
	{
		MATRIX3 M;
		VECTOR3 X_NB, Y_NB, Z_NB, X_NB_apo, Y_NB_apo, Z_NB_apo;

		M = OrbMech::CALCSMSC(opt->IMUAngles);
		X_NB_apo = _V(M.m11, M.m12, M.m13);
		Y_NB_apo = _V(M.m21, M.m22, M.m23);
		Z_NB_apo = _V(M.m31, M.m32, M.m33);
		X_NB = tmul(opt->PresentREFSMMAT, X_NB_apo);
		Y_NB = tmul(opt->PresentREFSMMAT, Y_NB_apo);
		Z_NB = tmul(opt->PresentREFSMMAT, Z_NB_apo);

		return _M(X_NB.x, X_NB.y, X_NB.z, Y_NB.x, Y_NB.y, Y_NB.z, Z_NB.x, Z_NB.y, Z_NB.z);
	}
	else if (opt->REFSMMATopt == 4)
	{
		//For now a default LC-39A, 72° launch
		return OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, opt->GETbase, 72 * RAD);
	}
	else if (opt->REFSMMATopt == 6)
	{
		double MoonPos[12];
		double PTCMJD;
		VECTOR3 R_ME;

		PTCMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);

		cMoon->clbkEphemeris(PTCMJD, EPHEM_TRUEPOS, MoonPos);

		R_ME = -_V(MoonPos[0], MoonPos[1], MoonPos[2]);

		UX = unit(crossp(_V(0.0, 1.0, 0.0), unit(R_ME)));
		//UX = mul(Rot, _V(UX.x, UX.z, UX.y));
		UX = _V(UX.x, UX.z, UX.y);

		UZ = _V(0.0, 0.0, -1.0);
		UY = crossp(UZ, UX);
		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else if (opt->REFSMMATopt == 8)
	{
		MATRIX3 Rot2;
		VECTOR3 R_P, R_LS, R_LPO, V_LPO, H_LPO, axis, H_C;
		double LSMJD;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat)));
		LSMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;
		Rot2 = OrbMech::GetRotationMatrix(oapiGetObjectByName("Moon"), LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);

		OrbMech::adbar_from_rv(1.0, 1.0, opt->LSLng, opt->LSLat, PI05, opt->LSAzi, R_LPO, V_LPO);
		H_LPO = crossp(R_LPO, V_LPO);
		axis = unit(H_LPO);
		H_C = unit(rhmul(Rot2, axis));

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}

	double SVMJD, dt, LMmass;
	VECTOR3 R_A, V_A, X_B;
	VECTOR3 DV_P, DV_C, V_G, X_SM, Y_SM, Z_SM;
	double theta_T;
	OBJHANDLE gravref;
	SV sv0, sv2;
	THGROUP_TYPE th_main;

	if (opt->vessel->GetGroupThruster(THGROUP_MAIN, 0) == NULL)
	{
		th_main = THGROUP_HOVER;
	}
	else
	{
		th_main = THGROUP_MAIN;
	}

	gravref = AGCGravityRef(opt->vessel);

	opt->vessel->GetRelativePos(gravref, R_A);
	opt->vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();

	R_A = _V(R_A.x, R_A.z, R_A.y);
	V_A = _V(V_A.x, V_A.z, V_A.y);

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	sv0.gravref = gravref;
	sv0.mass = opt->vessel->GetMass();
	sv0.MJD = SVMJD;
	sv0.R = R_A;
	sv0.V = V_A;

	if (opt->REFSMMATopt == 7)
	{
		SV sv1;

		sv1 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, LMmass);
		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG2, opt->dV_LVLH2, sv1, LMmass);
	}
	else if (opt->REFSMMATdirect == false && length(opt->dV_LVLH) != 0.0 )	//Check against DV = 0, some calculations could break down
	{
		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, LMmass);
	}
	else
	{
		sv2 = sv0;
	}

	if (opt->REFSMMATopt == 5)
	{
		double LSMJD;
		VECTOR3 R_P, R_LS, H_C;
		MATRIX3 Rot2;
		SV sv3;

		LSMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat)));

		Rot2 = OrbMech::GetRotationMatrix(oapiGetObjectByName("Moon"), LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);

		OrbMech::oneclickcoast(sv2.R, sv2.V, sv2.MJD, (LSMJD - sv2.MJD)*24.0*3600.0, sv3.R, sv3.V, sv2.gravref, hMoon);
		sv3.gravref = hMoon;
		sv3.mass = sv2.mass;
		H_C = crossp(sv3.R, sv3.V);

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else
	{
		SV sv4;

		if (opt->REFSMMATopt == 2)
		{
			dt = opt->REFSMMATTime - (sv2.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
			sv4 = coast(sv2, dt);
		}
		else if (opt->REFSMMATopt == 7)
		{
			double t_p, mu;
			SV sv3;

			mu = GGRAV*oapiGetMass(hMoon);
			t_p = OrbMech::period(sv2.R, sv2.V, mu);
			OrbMech::oneclickcoast(sv2.R, sv2.V, sv2.MJD, 1.5*t_p, sv3.R, sv3.V, sv2.gravref, hMoon);
			sv3.gravref = hMoon;
			sv3.mass = sv2.mass;
			sv3.MJD = sv2.MJD + 1.5*t_p / 24.0 / 3600.0;

			OrbMech::time_radius_integ(sv3.R, sv3.V, sv3.MJD, oapiGetSize(hMoon) + 60.0*1852.0, -1, hMoon, hMoon, sv4.R, sv4.V);
		}
		else if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			dt = opt->P30TIG - (sv2.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
			sv4 = coast(sv2, dt);
		}
		else
		{
			dt = OrbMech::time_radius_integ(sv2.R, sv2.V, sv2.MJD, oapiGetSize(hEarth) + 400000.0*0.3048, -1, sv2.gravref, hEarth, sv4.R, sv4.V);
		}

		if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			UY = unit(crossp(sv4.V, sv4.R));
			UZ = unit(-sv4.R);
			UX = crossp(UY, UZ);

			double headsswitch;

			if (opt->HeadsUp)
			{
				headsswitch = 1.0;
			}
			else
			{
				headsswitch = -1.0;
			}

			DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
			if (length(DV_P) != 0.0)
			{
				theta_T = length(crossp(sv4.R, sv4.V))*length(opt->dV_LVLH)*(sv4.mass + LMmass) / OrbMech::power(length(sv4.R), 2.0) / 92100.0;
				DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
				V_G = DV_C + UY*opt->dV_LVLH.y;
			}
			else
			{
				V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
			}

			if (opt->vesseltype < 2)
			{
				MATRIX3 M, M_R, M_RTM;
				double p_T, y_T;

				CalcSPSGimbalTrimAngles(sv4.mass, LMmass, p_T, y_T);

				if (opt->REFSMMATopt == 0)
				{
					X_B = unit(V_G);
				}
				else
				{
					headsswitch = 1.0;
					p_T = 2.15*RAD;
					X_B = -unit(V_G);
				}
				UX = X_B;
				UY = unit(crossp(UX, sv4.R*headsswitch));
				UZ = unit(crossp(UX, UY));

				M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
				M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
				M_RTM = mul(OrbMech::tmat(M_R), M);
				X_SM = mul(M_RTM, _V(1.0, 0.0, 0.0));
				Y_SM = mul(M_RTM, _V(0.0, 1.0, 0.0));
				Z_SM = mul(M_RTM, _V(0.0, 0.0, 1.0));
			}
			else
			{
				VECTOR3 U_FDI;

				U_FDI = unit(V_G);

				if (opt->REFSMMATopt == 0)
				{
					X_SM = U_FDI;
				}
				else
				{
					X_SM = -U_FDI;
				}
				if (abs(dotp(X_SM, unit(sv4.R))) < cos(0.01*RAD))
				{
					Y_SM = unit(crossp(X_SM, -sv4.R*headsswitch));
				}
				else
				{
					Y_SM = unit(crossp(X_SM, sv4.V));
				}
				Z_SM = unit(crossp(X_SM, Y_SM));
			}

			return _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

			//IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(transpose_matrix(M), M_R));
			//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);
		}
		else
		{
			if (opt->vesseltype < 2)
			{
				UY = unit(crossp(sv4.V, sv4.R));
				UZ = unit(-sv4.R);
				UX = crossp(UY, UZ);
			}
			else
			{
				UX = unit(sv4.R);
				UY = unit(crossp(sv4.V, sv4.R));
				UZ = unit(crossp(UX, UY));
			}

			return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		}
		//
	}
}

double RTCC::CDHcalc(CDHOpt *opt, VECTOR3 &dV_LVLH, double &P30TIG)			//Calculates the required DV vector of a coelliptic burn
{
	double mu;
	double SVMJD, SVtime, dt, dt2, c1, c2, theta, SW, dh_CDH, VPV, dt2_apo, CDHtime_cor, mass;
	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb;
	VECTOR3 RA0, VA0, RP0, VP0, RA2_alt, VA2_alt, V_A2_apo, CDHdeltaV;
	VECTOR3 u, RPC, VPC;
	double epsilon, a_P, a_A, V_AH, V_AV;
	MATRIX3 Q_Xx;
	//MATRIX3 obli;
	VECTOR3 RA2, VA2, RP2, VP2;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(opt->vessel);

	mu = GGRAV*oapiGetMass(gravref);

	//DH_met = DH*1852.0;							//Calculates the desired delta height of the coellitpic orbit in metric units

	opt->vessel->GetRelativePos(gravref, RA0_orb);	//vessel position vector
	opt->vessel->GetRelativeVel(gravref, VA0_orb);	//vessel velocity vector
	opt->target->GetRelativePos(gravref, RP0_orb);	//target position vector
	opt->target->GetRelativeVel(gravref, VP0_orb);	//target velocity vector
	SVtime = oapiGetSimTime();					//The time mark of the state vectors

	//oapiGetPlanetObliquityMatrix(gravref, &obli);

	//RA0_orb = mul(OrbMech::inverse(obli), RA0_orb);
	//VA0_orb = mul(OrbMech::inverse(obli), VA0_orb);
	//RP0_orb = mul(OrbMech::inverse(obli), RP0_orb);
	//VP0_orb = mul(OrbMech::inverse(obli), VP0_orb);

	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);	//Coordinate system nightmare. My calculation methods use another system
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
	RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
	VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

	SVMJD = oapiTime2MJD(SVtime);				//Calculates the MJD of the state vector

	mass = opt->vessel->GetMass();

	if (opt->CDHtimemode == 0)
	{
		CDHtime_cor = opt->TIG;
		dt2 = opt->TIG - (SVMJD - opt->GETbase) * 24 * 60 * 60;
	}
	else
	{
		int n = 0;

		dt = opt->TIG - (SVMJD - opt->GETbase) * 24 * 60 * 60;

		dt2 = dt + 10.0;							//A secant search method is used to find the time, when the desired delta height is reached. Other values might work better.

		while (abs(dt2 - dt) > 0.1 && n <= 20)					//0.1 seconds accuracy should be enough
		{
			c1 = OrbMech::NSRsecant(RA0, VA0, RP0, VP0, SVMJD, dt, opt->DH, gravref);		//c is the difference between desired and actual DH
			c2 = OrbMech::NSRsecant(RA0, VA0, RP0, VP0, SVMJD, dt2, opt->DH, gravref);

			dt2_apo = dt2 - (dt2 - dt) / (c2 - c1)*c2;						//secant method
			dt = dt2;
			dt2 = dt2_apo;
			n++;
		}

		CDHtime_cor = dt2 + (SVMJD - opt->GETbase) * 24 * 60 * 60;		//the new, calculated CDH time

	}

	//OrbMech::rv_from_r0v0(RA0, VA0, dt2, RA2, VA2, mu);
	//OrbMech::rv_from_r0v0(RP0, VP0, dt2, RP2, VP2, mu);
	OrbMech::oneclickcoast(RA0, VA0, SVMJD, dt2, RA2, VA2, gravref, gravref);//Coasting Integration, active vessel state to CDH time
	OrbMech::oneclickcoast(RP0, VP0, SVMJD, dt2, RP2, VP2, gravref, gravref);//Coasting Integration, passive vessel state to CDH time

	//This block of equations is taken from the GSOP for Luminary document: 
	//http://www.ibiblio.org/apollo/NARA-SW/R-567-sec5-rev8-5.4-5.5.pdf
	//Pre-CDH Program, page 5.4-18
	u = unit(crossp(RP2, VP2));
	RA2_alt = RA2;
	VA2_alt = VA2;
	RA2 = unit(RA2 - u*dotp(RA2, u))*length(RA2);
	VA2 = unit(VA2 - u*dotp(VA2, u))*length(VA2);

	theta = acos(dotp(RA2, RP2) / length(RA2) / length(RP2));
	SW = OrbMech::sign(dotp(u, crossp(RP2, RA2)));
	theta = SW*theta;
	OrbMech::rv_from_r0v0_ta(RP2, VP2, theta, RPC, VPC, mu);
	dh_CDH = length(RPC) - length(RA2);

	if (opt->CDHtimemode == 0)
	{
		//DH = dh_CDH / 1852.0;
	}

	VPV = dotp(VPC, RA2 / length(RA2));

	epsilon = (length(VPC)*length(VPC)) / 2 - mu / length(RPC);
	a_P = -mu / (2.0 * epsilon);
	a_A = a_P - dh_CDH;
	V_AV = VPV*OrbMech::power((a_P / a_A), 1.5);
	V_AH = sqrt(mu*(2.0 / length(RA2) - 1.0 / a_A) - (V_AV*V_AV));
	V_A2_apo = unit(crossp(u, RA2))*V_AH + unit(RA2)*V_AV;	//The desired velocity vector at CDH time													
	
	if (opt->impulsive == RTCC_IMPULSIVE)
	{
		Q_Xx = OrbMech::LVLH_Matrix(RA2, VA2);

		CDHdeltaV = mul(Q_Xx, V_A2_apo - VA2_alt); //Calculates the desired P30 LVLH DV Vector

		P30TIG = CDHtime_cor;
		dV_LVLH = CDHdeltaV;
	}
	else
	{
		VECTOR3 Llambda, RA2_cor, VA2_cor;double t_slip;

		SV sv_tig;

		sv_tig.gravref = gravref;
		sv_tig.mass = mass;
		sv_tig.MJD = opt->GETbase + CDHtime_cor / 24.0 / 3600.0;
		sv_tig.R = RA2;
		sv_tig.V = VA2_alt;

		FiniteBurntimeCompensation(opt->vesseltype, sv_tig, 0.0, V_A2_apo - VA2_alt, true, Llambda, t_slip);

		OrbMech::rv_from_r0v0(RA2, VA2_alt, t_slip, RA2_cor, VA2_cor, mu);

		Q_Xx = OrbMech::LVLH_Matrix(RA2_cor, VA2_cor);

		CDHdeltaV = mul(Q_Xx, Llambda); //Calculates the desired P30 LVLH DV Vector
		P30TIG = CDHtime_cor + t_slip;
		dV_LVLH = CDHdeltaV;

		/*dV_LVLH = CDHdeltaV;
		P30TIG = CDHtime_cor;*/
	}

	return dh_CDH;
}

double RTCC::FindDH(SV sv_A, SV sv_P, double GETbase, double TIGguess, double DH)
{
	SV sv_A1, sv_P1;
	double dt, dt2, t_A, t_P, SVMJD, c1, c2, dt2_apo, CDHtime_cor;
	int n = 0;

	t_A = OrbMech::GETfromMJD(sv_A.MJD, GETbase);
	t_P = OrbMech::GETfromMJD(sv_P.MJD, GETbase);
	sv_A1 = coast(sv_A, TIGguess - t_A);
	sv_P1 = coast(sv_P, TIGguess - t_P);
	SVMJD = GETbase + TIGguess / 24.0 / 3600.0;

	dt = 0.0;
	dt2 = dt + 10.0;
	
	//A secant search method is used to find the time, when the desired delta height is reached. Other values might work better.
	while (abs(dt2 - dt) > 0.1 && n <= 20)					//0.1 seconds accuracy should be enough
	{
		c1 = OrbMech::NSRsecant(sv_A1.R, sv_A1.V, sv_P1.R, sv_P1.V, SVMJD, dt, DH, sv_A1.gravref);		//c is the difference between desired and actual DH
		c2 = OrbMech::NSRsecant(sv_A1.R, sv_A1.V, sv_P1.R, sv_P1.V, SVMJD, dt2, DH, sv_A1.gravref);

		dt2_apo = dt2 - (dt2 - dt) / (c2 - c1)*c2;						//secant method
		dt = dt2;
		dt2 = dt2_apo;
		n++;
	}

	CDHtime_cor = dt2 + (SVMJD - GETbase) * 24 * 60 * 60;		//the new, calculated CDH time
	return CDHtime_cor;
}

LambertMan RTCC::set_lambertoptions(SV sv_A, SV sv_P, double GETbase, double T1, double T2, int N, int axis, int Perturbation, VECTOR3 Offset, double PhaseAngle)
{
	LambertMan opt;

	opt.axis = axis;
	opt.GETbase = GETbase;
	opt.N = N;
	opt.Offset = Offset;
	opt.Perturbation = Perturbation;
	opt.PhaseAngle = PhaseAngle;
	opt.sv_A = sv_A;
	opt.sv_P = sv_P;
	opt.T1 = T1;
	opt.T2 = T2;
	
	return opt;
}

double RTCC::lambertelev(VESSEL* vessel, VESSEL* target, double GETbase, double elev)
{
	double SVMJD, dt1;
	VECTOR3 RA0_orb, VA0_orb, RP0_orb, VP0_orb, RA0, VA0, RP0, VP0;
	OBJHANDLE gravref;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, RA0_orb);
	vessel->GetRelativeVel(gravref, VA0_orb);
	target->GetRelativePos(gravref, RP0_orb);
	target->GetRelativeVel(gravref, VP0_orb);
	SVMJD = oapiGetSimMJD();

	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);
	RP0 = _V(RP0_orb.x, RP0_orb.z, RP0_orb.y);
	VP0 = _V(VP0_orb.x, VP0_orb.z, VP0_orb.y);

	dt1 = OrbMech::findelev(RA0, VA0, RP0, VP0, SVMJD, elev, gravref);

	return dt1 + (SVMJD - GETbase) * 24.0 * 60.0 * 60.0;
}

void RTCC::DOITargeting(DOIMan *opt, VECTOR3 &dv, double &P30TIG)
{
	double CR, t_L, t_PDI;

	DOITargeting(opt, dv, P30TIG, t_PDI, t_L, CR);
}

void RTCC::DOITargeting(DOIMan *opt, VECTOR3 &DV, double &P30TIG, double &t_PDI, double &t_L, double &CR)
{
	double GET, h_DP, theta_F, t_F, mu, t_DOI;
	VECTOR3 R_LSA;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");

	GET = (opt->sv0.MJD - opt->GETbase)*24.0*3600.0;

	R_LSA = _V(cos(opt->lng)*cos(opt->lat), sin(opt->lng)*cos(opt->lat), sin(opt->lat))*(oapiGetSize(hMoon) + opt->alt);
	h_DP = opt->PeriAlt;
	theta_F = opt->PeriAng;
	t_F = 718.0;
	mu = GGRAV*oapiGetMass(hMoon);

	if (opt->opt == 0)
	{
		OrbMech::LunarLandingPrediction(opt->sv0.R, opt->sv0.V, GET, opt->EarliestGET, R_LSA, h_DP, theta_F, t_F, hMoon, opt->GETbase, mu, opt->N, t_DOI, t_PDI, t_L, DV, CR);
	}
	else
	{
		OrbMech::LunarLandingPrediction2(opt->sv0.R, opt->sv0.V, GET, opt->EarliestGET, R_LSA, h_DP, 1.0, theta_F, t_F, hMoon, opt->GETbase, mu, opt->N, t_DOI, t_PDI, t_L, DV, CR);
	}

	P30TIG = t_DOI;
}

void RTCC::PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	double SVMJD, cosA, sinA, A, dt, MJD_A, lng, lat, rad, GET, mu, LMmass, mass, t_slip;
	MATRIX3 Rot, Q_Xx;
	VECTOR3 R_A, V_A, RLS, n1, u, n2, V_PC2, DV, loc, Llambda;
	SV sv0, sv1, sv_PC, sv_cor;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0.gravref = AGCGravityRef(opt->vessel);
		opt->vessel->GetRelativePos(sv0.gravref, R_A);
		opt->vessel->GetRelativeVel(sv0.gravref, V_A);
		SVMJD = oapiGetSimMJD();
		sv0.R = _V(R_A.x, R_A.z, R_A.y);
		sv0.V = _V(V_A.x, V_A.z, V_A.y);
		sv0.mass = opt->vessel->GetMass();
	}

	GET = (SVMJD - opt->GETbase)*24.0*3600.0;
	MJD_A = opt->GETbase + opt->t_A / 24.0 / 3600.0;
	Rot = OrbMech::GetRotationMatrix(sv0.gravref, MJD_A);
	mu = GGRAV*oapiGetMass(sv0.gravref);

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	if (opt->landed)
	{
		opt->target->GetEquPos(lng, lat, rad);
		loc = unit(_V(cos(lng)*cos(lat), sin(lat), sin(lng)*cos(lat)))*rad;
	}
	else
	{
		loc = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)))*oapiGetSize(sv0.gravref);
	}

	RLS = mul(Rot, loc);
	RLS = _V(RLS.x, RLS.z, RLS.y);
	sv1 = coast(sv0, opt->EarliestGET - GET);

	n1 = unit(crossp(sv1.R, sv1.V));
	u = unit(crossp(-RLS, n1));
	cosA = dotp(u, unit(sv1.R));
	sinA = sqrt(1.0 - cosA*cosA);
	A = atan2(sinA, cosA);
	dt = OrbMech::time_theta(sv1.R, sv1.V, A, mu);
	sv_PC = coast(sv1, dt);
	n2 = unit(crossp(sv_PC.R, -RLS));
	V_PC2 = unit(crossp(n2, sv_PC.R))*length(sv_PC.V);

	DV = V_PC2 - sv_PC.V;

	FiniteBurntimeCompensation(opt->vesseltype, sv_PC, LMmass, DV, true, Llambda, t_slip);

	sv_cor = coast(sv_PC, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_cor.R, sv_cor.V);

	dV_LVLH = mul(Q_Xx, Llambda);

	P30TIG = opt->EarliestGET + dt + t_slip;
}

void RTCC::LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node, SV &sv_post)
{
	SV sv0, sv1;
	VECTOR3 axis, u_LPO, H, u_LAH, u_node, p, q, H_LPO;
	MATRIX3 Rot;
	double MJD_LAND, mu, dt, dt_node, R_M, apo, peri, a, e, h, mass, LMmass, GET, r, GET_node;
	OBJHANDLE hMoon;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV*oapiGetMass(hMoon);
	sv_node.gravref = hMoon;

	MJD_LAND = opt->GETbase + opt->t_land / 24.0 / 3600.0;
	Rot = OrbMech::GetRotationMatrix(hMoon, MJD_LAND);

	//Lunar radius above LLS
	R_M = oapiGetSize(hMoon) + opt->alt;

	//Initial guess for the node, at pericynthion
	dt = OrbMech::timetoperi_integ(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hMoon, sv1.R, sv1.V);
	sv1 = coast(sv0, dt);
	H = crossp(sv1.R, sv1.V);

	//Lunar Approach Hyperbola orientation
	u_LAH = unit(H);

	VECTOR3 R_LPO, V_LPO;
	double r_LPO, v_LPO;

	r_LPO = R_M + 60.0*1852.0;
	v_LPO = sqrt(mu / r_LPO);

	OrbMech::adbar_from_rv(r_LPO, v_LPO, opt->lng, opt->lat, PI05, opt->azi, R_LPO, V_LPO);

	if (opt->type == 0)
	{
		H_LPO = crossp(R_LPO, V_LPO);

		axis = unit(H_LPO);

		//Lunar Parking Orbit orientation
		u_LPO = unit(rhmul(Rot, axis));

		//Node unit vector
		u_node = unit(crossp(u_LPO, u_LAH));

		//Periapsis vector and node should be within 90°, if not the actual intersection is in the other direction
		if (dotp(u_node, unit(sv1.R)) < 0.0)
		{
			u_node = -u_node;
		}

		//state vector at the node
		dt_node = OrbMech::timetonode_integ(sv1.R, sv1.V, sv1.MJD, hMoon, u_node, sv_node.R, sv_node.V);
	}
	else
	{
		axis = unit(rhmul(Rot, R_LPO));

		//Lunar Parking Orbit orientation
		u_LPO = unit(crossp(axis, unit(sv1.R)));

		if (dotp(u_LPO, u_LAH) < 0.0)
		{
			u_LPO = -u_LPO;
		}

		u_node = unit(sv1.R);
		sv_node.R = sv1.R;
		sv_node.V = sv1.V;
		dt_node = 0.0;
	}

	//Time at node
	GET_node = GET + dt + dt_node;
	sv_node.MJD = opt->GETbase + GET_node / 24.0 / 3600.0;

	apo = R_M + opt->h_apo;
	peri = R_M + opt->h_peri;
	r = length(sv_node.R);

	if (r > apo)													//If the maneuver radius is higher than the desired apoapsis, then we would get no solution
	{
		apo = r;													//sets the desired apoapsis to the current radius, so that we can calculate a maneuver
	}
	else if (r < peri)												//If the maneuver radius is lower than the desired periapsis, then we would also get no solution
	{
		peri = r;													//sets the desired periapsis to the current radius, so that we can calculate a maneuver
	}

	a = (apo + peri) / 2.0;
	e = (apo - peri) / (apo + peri);
	h = sqrt(mu*a*(1.0 - e*e));

	double ta[2];
	VECTOR3 DV[2], R_ref[2], V_ref[2];
	MATRIX3 Q_Xx;
	int sol;

	ta[0] = acos(min(1.0, max(-1.0, (a / r*(1.0 - e*e) - 1.0) / e)));	//The true anomaly of the desired orbit, min and max just to make sure this value isn't out of bounds for acos
	ta[1] = PI2 - ta[0];												//Calculates the second possible true anomaly of the desired orbit

	for (int ii = 0;ii < 2;ii++)
	{

		p = OrbMech::RotateVector(u_LPO, -ta[ii], u_node);
		q = unit(crossp(u_LPO, p));

		R_ref[ii] = (p*cos(ta[ii]) + q*sin(ta[ii]))*h*h / mu / (1.0 + e*cos(ta[ii]));
		V_ref[ii] = (-p*sin(ta[ii]) + q*(e + cos(ta[ii])))*mu / h;

		DV[ii] = V_ref[ii] - sv_node.V;
	}

	if (opt->EllipseRotation == 0)
	{
		if (length(DV[1]) < length(DV[0]))
		{
			sol = 1;
		}
		else
		{
			sol = 0;
		}
	}
	else
	{
		if (opt->EllipseRotation == 1)
		{
			sol = 0;
		}
		else
		{
			sol = 1;
		}
	}

	OELEMENTS coe;

	coe = OrbMech::coe_from_sv(sv_node.R, sv_node.V, mu);

	if (opt->impulsive == RTCC_NONIMPULSIVE)
	{
		VECTOR3 Llambda, R_cut, V_cut, R_cor, V_cor;
		double f_T, isp, t_slip, MJD_cut, m_cut;;

		if (opt->vesseltype == 0)
		{
			f_T = SPS_THRUST;
			isp = SPS_ISP;
		}
		else
		{
			f_T = DPS_THRUST;
			isp = DPS_ISP;
		}

		OrbMech::impulsive(sv_node.R, sv_node.V, sv_node.MJD, hMoon, f_T, f_T, isp, mass, R_ref[sol], V_ref[sol], Llambda, t_slip, R_cut, V_cut, MJD_cut, m_cut);

		OrbMech::oneclickcoast(sv_node.R, sv_node.V, sv_node.MJD, t_slip, R_cor, V_cor, hMoon, hMoon); //Calculate the state vector at the corrected ignition time

		Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

		dV_LVLH = mul(Q_Xx, Llambda);
		P30TIG = GET_node + t_slip;

		sv_post = sv_node;
		sv_post.R = R_cut;
		sv_post.V = V_cut;
		sv_post.MJD = MJD_cut;
		sv_post.mass = m_cut;
	}
	else
	{
		Q_Xx = OrbMech::LVLH_Matrix(sv_node.R, sv_node.V);

		dV_LVLH = mul(Q_Xx, DV[sol]);
		P30TIG = GET_node;

		sv_post = sv_node;
		sv_post.R = R_ref[sol];
		sv_post.V = V_ref[sol];
	}

}

void RTCC::LOI2Targeting(LOI2Man *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	SV sv0, sv1;
	double GET, LMmass, mass, mu;
	OBJHANDLE hMoon;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV*oapiGetMass(hMoon);

	double a, dt2, LOIGET, v_circ;
	VECTOR3 U_H, U_hor, VA2_apo, DVX;
	MATRIX3 Q_Xx;

	mu = GGRAV*oapiGetMass(hMoon);
	a = oapiGetSize(hMoon) + opt->h_circ + opt->alt;

	dt2 = OrbMech::time_radius_integ(sv0.R, sv0.V, sv0.MJD, a, -1, hMoon, hMoon, sv1.R, sv1.V);
	sv1 = coast(sv0, dt2);
	LOIGET = dt2 + (sv0.MJD - opt->GETbase) * 24 * 60 * 60;

	U_H = unit(crossp(sv1.R, sv1.V));
	U_hor = unit(crossp(U_H, unit(sv1.R)));
	v_circ = sqrt(mu*(2.0 / length(sv1.R) - 1.0 / a));
	VA2_apo = U_hor*v_circ;

	DVX = VA2_apo - sv1.V;

	VECTOR3 Llambda;
	double t_slip;
	SV sv_tig;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DVX, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	dV_LVLH = mul(Q_Xx, Llambda);

	P30TIG = LOIGET + t_slip;
}

void RTCC::TranslunarInjectionProcessorNodal(TLIManNode *opt, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut)
{
	double GET;
	OBJHANDLE hMoon;
	SV sv0, sv1;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;

	double TIGMJD, PeriMJD, dt1, dt2, TIGguess, dTIG;
	VECTOR3 R_peri, VA1_apo, V_peri, RA2, DVX;
	MATRIX3 Q_Xx;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	int ii;

	PeriMJD = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	TIGguess = opt->TLI_TIG + 6.0*60.0;
	dTIG = -60.0;
	ii = 0;

	double TIGvar[3], dv[3];
	VECTOR3 Llambda, R2_cor, V2_cor, dVLVLH; double t_slip, f_T, isp, boil, m1, m0, TIG, mcut;

	f_T = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	isp = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	boil = (1.0 - 0.99998193) / 10.0;
	m0 = opt->vessel->GetEmptyMass();

	while (abs(dTIG) > 1.0)
	{
		TIGMJD = TIGguess / 24.0 / 3600.0 + opt->GETbase;
		dt1 = TIGguess - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
		dt2 = (PeriMJD - TIGMJD)*24.0*3600.0;

		sv1 = coast(sv0, dt1);

		double mu_E, mu_M;
		double MoonPos[12];
		CELBODY *cMoon;
		VECTOR3 R_I_star, delta_I_star, delta_I_star_dot, R_P, R_m, V_m;
		MATRIX3 Rot2;
		R_I_star = delta_I_star = delta_I_star_dot = _V(0.0, 0.0, 0.0);

		cMoon = oapiGetCelbodyInterface(hMoon);
		mu_E = GGRAV*oapiGetMass(hEarth);
		mu_M = GGRAV*oapiGetMass(hMoon);

		R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
		Rot2 = OrbMech::GetRotationMatrix(hMoon, PeriMJD);

		R_peri = mul(Rot2, R_P);
		//R_peri = unit(mul(Rot, _V(R_peri.x, R_peri.z, R_peri.y)))*(oapiGetSize(hMoon) + opt->h_peri);
		R_peri = unit(_V(R_peri.x, R_peri.z, R_peri.y))*(oapiGetSize(hMoon) + opt->h_peri);

		cMoon->clbkEphemeris(PeriMJD, EPHEM_TRUEPOS | EPHEM_TRUEVEL, MoonPos);
		//R_m = mul(Rot, _V(MoonPos[0], MoonPos[2], MoonPos[1]));
		//V_m = mul(Rot, _V(MoonPos[3], MoonPos[5], MoonPos[4]));
		R_m = _V(MoonPos[0], MoonPos[2], MoonPos[1]);
		V_m = _V(MoonPos[3], MoonPos[5], MoonPos[4]);

		//Initial trajectory, only accurate to 1000 meters
		V_peri = OrbMech::ThreeBodyLambert(PeriMJD, TIGMJD, R_peri, V_m, sv1.R, R_m, V_m, 24.0*oapiGetSize(hEarth), mu_E, mu_M, R_I_star, delta_I_star, delta_I_star_dot);
		//OrbMech::oneclickcoast(R_peri, V_peri, PeriMJD, -dt2, RA2, VA1_apo, hMoon, hEarth);

		//Precise backwards targeting
		V_peri = OrbMech::Vinti(R_peri, V_peri, sv1.R, PeriMJD, -dt2, 0, false, hMoon, hMoon, hEarth, V_peri);

		//Finding the new ignition state
		OrbMech::oneclickcoast(R_peri, V_peri, PeriMJD, (sv1.MJD - PeriMJD)*24.0*3600.0, RA2, VA1_apo, hMoon, hEarth);

		//Final pass through the targeting, forwards and precise
		//VA1_apo = OrbMech::Vinti(RA1, VA1, R_peri, TIGMJD, dt2, 0, true, hEarth, hEarth, hMoon, VA1_apo);

		DVX = VA1_apo - sv1.V;

		if (ii < 2)
		{
			TIGvar[ii + 1] = (TIGMJD - sv0.MJD)*24.0*3600.0;
			dv[ii + 1] = length(DVX);
		}
		else
		{
			dv[0] = dv[1];
			dv[1] = dv[2];
			dv[2] = length(DVX);
			TIGvar[0] = TIGvar[1];
			TIGvar[1] = TIGvar[2];
			TIGvar[2] = (TIGMJD - sv0.MJD)*24.0*3600.0;

			dTIG = OrbMech::quadratic(TIGvar, dv) + (sv0.MJD - TIGMJD)*24.0*3600.0;

			if (abs(dTIG) > 10.0*60.0)
			{
				dTIG = OrbMech::sign(dTIG)*10.0*60.0;
			}
		}

		TIGguess += dTIG;
		ii++;
	}

	m1 = (sv0.mass - m0)*exp(-boil*dt1);

	OrbMech::impulsive(sv1.R, sv1.V, TIGMJD, hEarth, f_T, f_T, isp, m0 + m1, DVX, Llambda, t_slip, Rcut, Vcut, MJDcut, mcut); //Calculate the impulsive equivalent of the maneuver

	OrbMech::oneclickcoast(sv1.R, sv1.V, TIGMJD, t_slip, R2_cor, V2_cor, hEarth, hEarth);//Calculate the state vector at the corrected ignition time

	Q_Xx = OrbMech::LVLH_Matrix(R2_cor, V2_cor);

	dVLVLH = mul(Q_Xx, Llambda);
	TIG = TIGguess + t_slip;

	P30TIG = TIG;
	dV_LVLH = dVLVLH;
}

void RTCC::TranslunarInjectionProcessorFreeReturn(TLIManFR *opt, TLMCCResults *res, VECTOR3 &Rcut, VECTOR3 &Vcut, double &MJDcut)
{
	//state vector "now"
	SV sv0;
	//state vector at initial guess for ignition
	SV sv1;
	//state vector at current ignition time
	SV sv2;
	//state vector at periapsis
	SV sv_peri;
	//state vector at reentry
	SV sv_reentry;
	MATRIX3 Q_Xx;
	VECTOR3 DV, DV_guess, Llambda, R2_cor, V2_cor, dVLVLH, var_conv;
	double PeriMJDguess, dTIG, TIGguess, dt0, dt1, TIGMJD, f_T, isp, boil, m0, m1, t_slip, mcut, TIG, mu_E;
	double TIGvar[3], dv[3];
	int ii;
	OBJHANDLE hEarth;

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	dTIG = -60.0;
	ii = 0;
	hEarth = oapiGetObjectByName("Earth");
	mu_E = GGRAV*oapiGetMass(hEarth);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;
	TIGguess = opt->TLI_TIG + 6.0*60.0;
	dt0 = TIGguess - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv0, dt0);

	TLIFirstGuessConic(sv1, opt->lat, opt->h_peri, PeriMJDguess, DV_guess, var_conv);
	IntegratedTLMC(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, var_conv, DV_guess, var_conv, sv_peri);

	while (abs(dTIG) > 1.0)
	{
		TIGMJD = TIGguess / 24.0 / 3600.0 + opt->GETbase;
		dt1 = (TIGMJD - sv1.MJD) * 24.0 * 60.0 * 60.0;
		sv2 = coast(sv1, dt1);

		TLIFlyby(sv2, opt->lat, opt->h_peri, sv_peri, DV, sv_peri, sv_reentry);

		if (ii < 2)
		{
			TIGvar[ii + 1] = (TIGMJD - sv0.MJD)*24.0*3600.0;
			dv[ii + 1] = length(DV);
		}
		else
		{
			dv[0] = dv[1];
			dv[1] = dv[2];
			dv[2] = length(DV);
			TIGvar[0] = TIGvar[1];
			TIGvar[1] = TIGvar[2];
			TIGvar[2] = (TIGMJD - sv0.MJD)*24.0*3600.0;

			dTIG = OrbMech::quadratic(TIGvar, dv) + (sv0.MJD - TIGMJD)*24.0*3600.0;

			if (abs(dTIG) > 10.0*60.0)
			{
				dTIG = OrbMech::sign(dTIG)*10.0*60.0;
			}
		}

		TIGguess += dTIG;
		ii++;
	}

	f_T = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	isp = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
	boil = (1.0 - 0.99998193) / 10.0;
	m0 = opt->vessel->GetEmptyMass();

	m1 = (sv0.mass - m0)*exp(-boil*dt1);

	OrbMech::impulsive(sv2.R, sv2.V, TIGMJD, hEarth, f_T, f_T, isp, m0 + m1, DV, Llambda, t_slip, Rcut, Vcut, MJDcut, mcut); //Calculate the impulsive equivalent of the maneuver

	OrbMech::oneclickcoast(sv2.R, sv2.V, TIGMJD, t_slip, R2_cor, V2_cor, hEarth, hEarth);//Calculate the state vector at the corrected ignition time

	Q_Xx = OrbMech::LVLH_Matrix(R2_cor, V2_cor);

	dVLVLH = mul(Q_Xx, Llambda);
	TIG = TIGguess + t_slip;

	res->P30TIG = TIG;
	res->dV_LVLH = dVLVLH;
	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(hEarth, sv_reentry.MJD);
	R_geo = rhtmul(Rot, sv_reentry.R);
	V_geo = rhtmul(Rot, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);
}

void RTCC::TranslunarMidcourseCorrectionTargetingNodal(MCCNodeMan *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	MATRIX3 Q_Xx, Rot, M_EMP;
	VECTOR3 R_selen, R, R_EMP, DV1, DV2, var_conv1, var_conv2, DV3, Llambda;
	double LMmass, mass, dt1, dt2, NodeMJD, t_slip;
	double lat_EMP, lng_EMP;
	OBJHANDLE hMoon;
	SV sv0, sv1, sv_tig, sv_peri2;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->NodeGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	NodeMJD = opt->NodeGET / 24.0 / 3600.0 + opt->GETbase;

	//Convert selenographic coordinates to EMP latitude and longitude

	R_selen = OrbMech::r_from_latlong(opt->lat, opt->lng);
	Rot = OrbMech::GetRotationMatrix(hMoon, NodeMJD);
	R = rhmul(Rot, R_selen);
	M_EMP = OrbMech::EMPMatrix(NodeMJD);
	R_EMP = mul(M_EMP, R);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	if (lng_EMP < 0.0)
	{
		lng_EMP += PI2;
	}

	//Step 1: Converge TLMC to the desired end conditions in order to provide good first guesses.
	TLMCFirstGuessConic(sv1, lat_EMP, opt->h_node, 0.0, NodeMJD, DV1, var_conv1);

	//Step 2: Converge integrated TLMC
	IntegratedTLMC(sv1, lat_EMP, opt->h_node, 0.0, NodeMJD, var_conv1, DV2, var_conv2, sv_peri2);

	//Step 3: Converge integrated XYZ and T trajectory
	TLMCIntegratedXYZT(sv1, lat_EMP, lng_EMP, opt->h_node, NodeMJD, DV2, DV3);

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV3, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	dV_LVLH = mul(Q_Xx, Llambda);
	P30TIG = opt->MCCGET + t_slip;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingFreeReturn(MCCFRMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_reentry2, sv_peri3, sv_reentry3, sv_peri, sv_reentry, sv_peri5, sv_node3, sv_node;
	double mass, LMmass, dt1, dt2, PeriMJDguess, mu_E, lat_EMP3;
	double r_peri3, v_peri3, lng_peri3, lat_peri3, fpav_peri3, azi_peri3;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 Llambda, DV, DV1, var_conv1, DV2, DV3, R_EMP3, V_EMP3, DV5, var_converged5;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);
	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, DV1, var_conv1);

	//Step 2: Converge conic, free-return trajectory
	TLMCFlybyConic(sv1, opt->lat, opt->h_peri, DV1, DV2, sv_peri2, sv_reentry2);

	//Step 3: Converge conic full mission
	if (!TLMCConic_BAP_FR_LPO(opt, sv1, opt->lat, opt->h_peri, DV2, DV3, sv_peri3, sv_node3, sv_reentry3, lat_EMP3))
	{
		return false;
	}

	OrbMech::EclToEMP(sv_peri3.R, sv_peri3.V, sv_peri3.MJD, R_EMP3, V_EMP3);
	OrbMech::rv_from_adbar(R_EMP3, V_EMP3, r_peri3, v_peri3, lng_peri3, lat_peri3, fpav_peri3, azi_peri3);

	//Step 5: Converge integrated TLMC to new targets
	IntegratedTLMC(sv1, lat_EMP3, opt->h_peri, 0.0, sv_peri3.MJD, _V(v_peri3, azi_peri3, lng_peri3), DV5, var_converged5, sv_peri5);

	//Step 6: Converge integrated full mission
	if (!TLMC_BAP_FR_LPO(opt, sv1, lat_EMP3, opt->h_peri, DV5, DV, sv_peri, sv_node, sv_reentry, res->EMPLatitude))
	{
		return false;
	}

	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;

	//Calculate nodal target
	OrbMech::latlong_from_J2000(sv_node.R, sv_node.MJD, sv_node.gravref, res->NodeLat, res->NodeLng);
	res->NodeAlt = length(sv_node.R) - oapiGetSize(hMoon);
	res->NodeGET = (sv_node.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot_reentry;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot_reentry = OrbMech::GetRotationMatrix(hEarth, sv_reentry.MJD);
	R_geo = rhtmul(Rot_reentry, sv_reentry.R);
	V_geo = rhtmul(Rot_reentry, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingNonFreeReturn(MCCNFRMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_node4, sv_node2;
	double mass, LMmass, dt1, dt2, PeriMJD, lat_EMP2, r_nd2, v_nd2, fpa_nd2, azi_nd2;
	double h_nd2, lat_nd2, lng_nd2, lat_nd4, lng_nd4, h_nd4;
	OBJHANDLE hMoon;
	VECTOR3 Llambda, DV1, var_conv1, DV2, DV4, R_EMP2, V_EMP2, var_conv4, DV5, R_EMP4, V_EMP4;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	hMoon = oapiGetObjectByName("Moon");

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJD = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJD, DV1, var_conv1);

	//Step 2: Converge a conic non-free select full mission
	if (!TLMCConic_BAP_NFR_LPO(opt, sv1, opt->lat, opt->h_peri, PeriMJD, DV1, DV2, sv_peri2, sv_node2, lat_EMP2))
	{
		return false;
	}

	OrbMech::EclToEMP(sv_node2.R, sv_node2.V, sv_node2.MJD, R_EMP2, V_EMP2);

	OrbMech::rv_from_adbar(R_EMP2, V_EMP2, r_nd2, v_nd2, lng_nd2, lat_nd2, fpa_nd2, azi_nd2);
	h_nd2 = r_nd2 - oapiGetSize(hMoon);

	//Step 4: Converge integrated TLMC
	IntegratedTLMC(sv1, lat_nd2, h_nd2, fpa_nd2 - PI05, sv_node2.MJD, _V(v_nd2, azi_nd2, lng_nd2), DV4, var_conv4, sv_node4);

	OrbMech::EclToEMP(sv_node4.R, sv_node4.V, sv_node4.MJD, R_EMP4, V_EMP4);
	OrbMech::latlong_from_r(R_EMP4, lat_nd4, lng_nd4);

	if (lng_nd4 < 0.0)
	{
		lng_nd4 += PI2;
	}

	h_nd4 = length(R_EMP4) - oapiGetSize(hMoon);

	//Step 5: Converge precision trajectory to optimized LOI conditions
	TLMCIntegratedXYZT(sv1, lat_nd4, lng_nd4, h_nd4, sv_node4.MJD, DV4, DV5);

	//Calculate nodal target
	OrbMech::latlong_from_J2000(sv_node4.R, sv_node4.MJD, sv_node4.gravref, res->NodeLat, res->NodeLng);
	res->NodeAlt = length(sv_node4.R) - oapiGetSize(hMoon);
	res->NodeGET = (sv_node4.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate LOI DV
	LOIMan loiopt;
	double TIG_LOI, MJD_meridian;
	SV sv_node6, sv_postLOI, sv_postLOI2;

	sv_node4.mass = sv1.mass;

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.impulsive = RTCC_NONIMPULSIVE;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.RV_MCC = sv_node4;
	loiopt.type = opt->type;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.vesseltype = opt->vesseltype;
	loiopt.EllipseRotation = opt->LOIEllipseRotation;

	LOITargeting(&loiopt, res->dV_LVLH_LOI, TIG_LOI, sv_node6, sv_postLOI);
	sv_postLOI2 = coast(sv_postLOI, 3600.0);
	MJD_meridian = OrbMech::P29TimeOfLongitude(sv_postLOI2.R, sv_postLOI2.V, sv_postLOI2.MJD, sv_postLOI2.gravref, 180.0*RAD);
	res->t_Rev2Meridian = OrbMech::GETfromMJD(MJD_meridian, opt->GETbase);

	if (opt->DOIType == 1)
	{
		//Calculate DOI DV
		DOIMan doiopt;
		SV sv_DOI, sv_postDOI;
		MATRIX3 Q_DOI;
		VECTOR3 dV_DOI;
		double TIG_DOI, mu_M, r_DOI_apo, r_DOI_peri;

		mu_M = GGRAV * oapiGetMass(hMoon);

		doiopt.alt = opt->alt;
		doiopt.EarliestGET = TIG_LOI + 3.5*3600.0;
		doiopt.GETbase = opt->GETbase;
		doiopt.lat = opt->LSlat;
		doiopt.lng = opt->LSlng;
		doiopt.N = opt->N;
		doiopt.opt = opt->DOIType;
		doiopt.sv0 = sv_postLOI2;
		doiopt.PeriAng = opt->DOIPeriAng;
		doiopt.PeriAlt = opt->DOIPeriAlt;

		DOITargeting(&doiopt, dV_DOI, TIG_DOI);
		sv_DOI = coast(sv_postLOI2, TIG_DOI - OrbMech::GETfromMJD(sv_postLOI2.MJD, opt->GETbase));
		Q_DOI = OrbMech::LVLH_Matrix(sv_DOI.R, sv_DOI.V);
		res->dV_LVLH_DOI = mul(Q_DOI, dV_DOI);

		sv_postDOI = sv_DOI;
		sv_postDOI.V = sv_DOI.V + dV_DOI;
		OrbMech::periapo(sv_postDOI.R, sv_postDOI.V, mu_M, r_DOI_apo, r_DOI_peri);

		res->h_apo_postDOI = r_DOI_apo - (oapiGetSize(hMoon) + opt->alt);
		res->h_peri_postDOI = r_DOI_peri - (oapiGetSize(hMoon) + opt->alt);
	}

	//Finite burntime compensation
	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV5, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;

	res->EMPLatitude = lat_nd4;

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingFlyby(MCCFlybyMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_peri3, sv_reentry3;
	double mass, LMmass, dt1, dt2, PeriMJDguess, mu_E;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 Llambda, DV1, var_conv1, DV2, var_conv2, DV3;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, DV1, var_conv1);

	//Step 2: Converge integrated TLMC
	IntegratedTLMC(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, var_conv1, DV2, var_conv2, sv_peri2);

	//Step 3: Converge integrated free-return trajectory
	if (!TLMCFlyby(sv1, opt->lat, opt->h_peri, DV2, DV3, sv_peri3, sv_reentry3))
	{
		return false;
	}

	res->PericynthionGET = (sv_peri3.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry3.MJD - opt->GETbase)*24.0*3600.0;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV3, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(hEarth, sv_reentry3.MJD);
	R_geo = rhtmul(Rot, sv_reentry3.R);
	V_geo = rhtmul(Rot, sv_reentry3.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry3.R, -sv_reentry3.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry3.R, sv_reentry3.V, sv_reentry3.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry3.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(MCCSPSLunarFlybyMan *opt, TLMCCResults *res, int &step)
{
	SV sv0, sv1, sv_peri, sv_reentry;
	double mass, LMmass, dt1, dt2, PeriMJDguess, mu_E;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 Llambda, DV, var_converged;
	MATRIX3 Q_Xx;
	double t_slip;
	SV sv_tig;

	step = 0;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	SV sv_p1, sv_p2, sv_r2, sv_p3, sv_r3, sv_p3B, S1, S2C, sv_p4, sv_r4, sv_p5, sv_r5, S_apo, sv_p6, sv_r6;
	VECTOR3 DV1, DV2, DV3, DV3B, DV4, DV5, DV6_guess, DV6, DV7_guess;
	double h_peri_fg, LunarRightAscension, LunarDeclination, LunarDistance, InclFRGuess, lat_EMP_SPLIT, lat_EMP_Target3, lat_EMP_Target6, lattemp;
	double lat_pc_max, lat_pc_min;

	h_peri_fg = 80.0*1852.0;

	//TBD: the 80NM first guess for the perilune height could be a manual input. 2000+ NM flybys don't work very well with this initial guess.
	//So for now limit the difference between the heights to 1000NM.
	if (opt->h_peri - h_peri_fg > 1000.0*1852.0)
	{
		h_peri_fg += opt->h_peri - h_peri_fg - 1000.0*1852.0;
	}

	step++;

	//Step 1: Conic TLMC First Guess
	TLMCFirstGuessConic(sv1, opt->lat, h_peri_fg, 0.0, PeriMJDguess, DV1, var_converged);
	step++;

	//Step 2: Conic Flyby to minimum inclination flyby

	OrbMech::GetLunarEquatorialCoordinates(PeriMJDguess, LunarRightAscension, LunarDeclination, LunarDistance);

	//User input constraint. Inclination has to be lunar declination at pericynthion passage plus 5 degrees
	if (opt->FRInclination < abs(LunarDeclination) + 5.0*RAD)
	{
		return false;
	}

	InclFRGuess = abs(LunarDeclination) + 2.0*RAD;

	if (!TLMCConicFlybyToInclinationSubprocessor(sv1, h_peri_fg, InclFRGuess, DV1, DV2, sv_p2, sv_r2, lat_EMP_SPLIT))
	{
		return false;
	}

	step++;

	//Step 3: Execute conic flyby to the biased latitude of perilune
	if (opt->AscendingNode)
	{
		lat_EMP_Target3 = lat_EMP_SPLIT + 2.0*RAD;
	}
	else
	{
		lat_EMP_Target3 = lat_EMP_SPLIT - 2.0*RAD;
	}

	if (!TLMCFlybyConic(sv1, lat_EMP_Target3, h_peri_fg, DV2, DV3, sv_p3, sv_r3))
	{
		return false;
	}

	S1 = sv1;
	S2C = sv1;
	S2C.V += DV3;

	//Step 3B: Integrated first guess with new latitude
	TLMCFirstGuess(sv1, lat_EMP_Target3, h_peri_fg, sv_p3.MJD, DV3B, sv_p3B);
	step++;

	//Step 4: Execute integrated flyby to the same latitude of perilune as step 3
	if (!TLMCFlyby(sv1, lat_EMP_Target3, h_peri_fg, DV3B, DV4, sv_p4, sv_r4))
	{
		return false;
	}

	step++;

	//Step 5: Execute integrated flyby to same inclination of free return and height as step 2
	if (!TLMCIntegratedFlybyToInclinationSubprocessor(sv1, h_peri_fg, InclFRGuess, DV4, DV5, sv_p5, sv_r5, lattemp))
	{
		return false;
	}

	S_apo = S2C;
	S_apo.V -= DV5;
	DV6_guess = S2C.V - S_apo.V;

	if (opt->AscendingNode)
	{
		lat_pc_max = 20.0*RAD;
		lat_pc_min = lat_EMP_SPLIT - 0.5*RAD;
	}
	else
	{
		lat_pc_max = lat_EMP_SPLIT + 0.5*RAD;
		lat_pc_min = -20.0*RAD;
	}

	step++;

	//Step 6: Converge a conic free return (select mode using S_apo as the state vector)
	if (!TLMCConicFlybyToInclinationSubprocessor(S_apo, opt->h_peri, opt->FRInclination, DV6_guess, DV6, sv_p6, sv_r6, lat_EMP_Target6))
	{
		return false;
	}

	step++;

	S2C = S_apo;
	S2C.V += DV6;
	DV7_guess = S2C.V - S_apo.V;

	//Step 7: Converge integrated flyby to specified inclination of free return and height using state S1
	if (!TLMCIntegratedFlybyToInclinationSubprocessor(sv1, opt->h_peri, opt->FRInclination, DV7_guess, DV, sv_peri, sv_reentry, res->EMPLatitude))
	{
		return false;
	}

	step++;

	FiniteBurntimeCompensation(opt->vesseltype, sv1, LMmass, DV, true, Llambda, t_slip); //Calculate the impulsive equivalent of the maneuver
	sv_tig = coast(sv1, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	res->dV_LVLH = mul(Q_Xx, Llambda);
	res->P30TIG = opt->MCCGET + t_slip;
	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(hEarth, sv_reentry.MJD);
	R_geo = rhtmul(Rot, sv_reentry.R);
	V_geo = rhtmul(Rot, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = oapiGetSize(hEarth) + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, mu_E);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG)
{
	GPMPRESULTS res;
	return GeneralManeuverProcessor(opt, dV_i, P30TIG, res);
}

bool RTCC::GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG, GPMPRESULTS &res)
{
	//SV at threshold time
	SV sv1;
	//SV at TIG
	SV sv2;

	VECTOR3 DV;
	double dt1, R_E, mu, dt2;
	int code;

	code = opt->ManeuverCode;
	dt1 = opt->TIG_GET + (opt->GETbase - opt->RV_MCC.MJD)*24.0*3600.0;
	sv1 = coast(opt->RV_MCC, dt1);

	//Body radius
	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		if (opt->AltRef == 0)
		{
			R_E = 6371.0e3;
		}
		else
		{
			R_E = oapiGetSize(sv1.gravref);
		}
	}
	else
	{
		if (opt->AltRef == 0)
		{
			R_E = oapiGetSize(sv1.gravref);
		}
		else
		{
			R_E = oapiGetSize(sv1.gravref) + opt->LSAlt;
		}
	}

	mu = GGRAV*oapiGetMass(sv1.gravref);

	//Advance state to maneuver time
	//Apogee
	if (code == RTCC_GMP_HAO || code == RTCC_GMP_FCA || code == RTCC_GMP_PHA || code == RTCC_GMP_HNA ||
		code == RTCC_GMP_CRA || code == RTCC_GMP_CPA || code == RTCC_GMP_CNA || code == RTCC_GMP_SAA)
	{
		dt2 = OrbMech::timetoapo_integ(sv1.R, sv1.V, sv1.MJD, sv1.gravref);
		sv2 = coast(sv1, dt2);
	}
	//Equatorial crossing
	else if (code == RTCC_GMP_PCE || code == RTCC_GMP_FCE)
	{
		dt2 = OrbMech::FindNextEquatorialCrossing(sv1.R, sv1.V, sv1.MJD, sv1.gravref);
		sv2 = coast(sv1, dt2);
	}
	//Perigee
	else if (code == RTCC_GMP_HPO || code == RTCC_GMP_FCP || code == RTCC_GMP_PHP || code == RTCC_GMP_HNP ||
		code == RTCC_GMP_CRP || code == RTCC_GMP_CPP || code == RTCC_GMP_CNP)
	{
		dt2 = OrbMech::timetoperi_integ(sv1.R, sv1.V, sv1.MJD, sv1.gravref, sv1.gravref);
		sv2 = coast(sv1, dt2);
	}
	//Longitude
	else if (code == RTCC_GMP_PCL || code == RTCC_GMP_CRL || code == RTCC_GMP_HOL || code == RTCC_GMP_FCL||
		code == RTCC_GMP_SAL || code == RTCC_GMP_PHL || code == RTCC_GMP_CPL || code == RTCC_GMP_HBL || 
		code == RTCC_GMP_CNL || code == RTCC_GMP_HNL || code == RTCC_GMP_NSL || code == RTCC_GMP_NHL)
	{
		double MJD_TIG;
		MJD_TIG = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, opt->long_D);
		sv2 = coast(sv1, (MJD_TIG - sv1.MJD)*24.0*3600.0);
	}
	//Height
	else if (code == RTCC_GMP_CRH || code == RTCC_GMP_HBH || code == RTCC_GMP_FCH || code == RTCC_GMP_CPH ||
		code == RTCC_GMP_PCH || code == RTCC_GMP_NSH || code == RTCC_GMP_HOH || code == RTCC_GMP_CNH)
	{
		double dt21, dt22, r_H;

		r_H = R_E + opt->H_D;
		dt21 = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, r_H, 1.0, sv1.gravref, sv1.gravref);
		dt22 = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, r_H, -1.0, sv1.gravref, sv1.gravref);

		if (abs(dt21) > abs(dt22))
		{
			dt2 = dt22;
		}
		else
		{
			dt2 = dt21;
		}

		sv2 = coast(sv1, dt2);
	}
	//Time
	else if (code == RTCC_GMP_PCT || code == RTCC_GMP_HOT || code == RTCC_GMP_NST || code == RTCC_GMP_HBT || code == RTCC_GMP_FCT ||
		code == RTCC_GMP_NHT || code == RTCC_GMP_PHT || code == RTCC_GMP_SAT || code == RTCC_GMP_HNT || code == RTCC_GMP_CRT ||
		code == RTCC_GMP_CPT || code == RTCC_GMP_CNT)
	{
		sv2 = sv1;
	}
	//Optimum Node Shift
	else if (code==RTCC_GMP_NSO)
	{
		OELEMENTS coe;
		MATRIX3 Rot;
		VECTOR3 R1_equ, V1_equ;
		double u_b, u_D;

		Rot = OrbMech::GetObliquityMatrix(sv1.gravref, sv1.MJD);
		R1_equ = rhtmul(Rot, sv1.R);
		V1_equ = rhtmul(Rot, sv1.V);
		coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
		u_b = fmod(coe.TA + coe.w, PI2);

		u_D = atan2(1.0 + cos(opt->dLAN), -sin(opt->dLAN)*cos(coe.RA));
		if (u_D < 0)
		{
			u_D += PI2;
		}
		if (u_b >= u_D)
		{
			if (u_b < fmod(u_D + PI, PI2))
			{
				u_D += PI;
			}
		}

		sv2 = GeneralTrajectoryPropagation(sv1, 2, u_D);
	}
	//Optimum apogee and perigee change
	else if (code == RTCC_GMP_HBO)
	{
		SV sv_a, sv_p;
		OELEMENTS coe_b, coe_p, coe_a;
		double p_d, p_b, r_AD, r_PD, a_d, e_d, a_b, e_b, r_a_b, r_p_b, R_MD, dt21, dt22, T_p, a_p, a_a, dr_a_max, dr_p_max, R_MD_apo, cos_theta, theta, r_p_apo, r_a_apo;
		int K1, n, nmax;

		//If desired apogee is equal to perigee, bias perigee to prevent zero eccentricity
		if (opt->H_P == opt->H_A)
		{
			opt->H_P -= 0.1*1852.0;
		}

		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		coe_b = OrbMech::coe_from_sv(sv1.R, sv1.V, mu);

		ApsidesDeterminationSubroutine(sv1, sv_a, sv_p);
		r_a_b = length(sv_a.R);
		r_p_b = length(sv_p.R);
		coe_p = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
		a_p = coe_p.h*coe_p.h / (mu*(1.0 - coe_p.e * coe_p.e));
		coe_a = OrbMech::coe_from_sv(sv_a.R, sv_a.V, mu);
		a_a = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e * coe_a.e));

		T_p = OrbMech::period(sv1.R, sv1.V, mu);

		if (r_AD<r_p_b || r_PD>r_a_b)
		{
			return false;
		}

		if (r_PD > r_a_b && r_PD > r_p_b)
		{
			K1 = -1;
		}
		else if (r_PD < r_a_b && r_PD < r_p_b)
		{
			K1 = -1;
		}
		else
		{
			K1 = 1;
		}

		dr_a_max = (a_p*(1.0 + coe_p.e) - r_a_b)*(r_AD / r_a_b);
		dr_p_max = (a_a*(1.0 - coe_a.e) - r_p_b)*(r_PD / r_p_b);
		a_d = (r_AD + r_PD) / 2.0;
		e_d = abs((a_d - r_PD) / a_d);

		e_b = coe_b.e;
		a_b = coe_b.h*coe_b.h / (mu*(1.0 - e_b * e_b));

		n = 0;
		nmax = 5;
		R_MD_apo = 1.0;
		sv2 = sv1;

		p_d = a_d * (1.0 - e_d * e_d);
		p_b = a_b * (1.0 - e_b * e_b);

		if (K1 < 0)
		{
			R_MD = 2.0*(p_d - p_b)*a_d*a_b / (p_d*a_d - p_b * a_b);
		}
		else
		{
			R_MD = (e_b*p_d - e_d * p_b) / (e_b - e_d);
		}

		do
		{
			R_MD_apo = R_MD;

			dt21 = OrbMech::time_radius_integ(sv2.R, sv2.V, sv2.MJD, R_MD, 1.0, sv2.gravref, sv2.gravref);
			if (dt21 < 0 && n == 0)
			{
				dt21 += T_p;
			}
			dt22 = OrbMech::time_radius_integ(sv2.R, sv2.V, sv2.MJD, R_MD, -1.0, sv2.gravref, sv2.gravref);
			if (dt22 < 0 && n == 0)
			{
				dt22 += T_p;
			}

			if (dt21 > dt22)
			{
				dt2 = dt22;
			}
			else
			{
				dt2 = dt21;
			}
			sv2 = coast(sv2, dt2);
			coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
			e_b = coe_b.e;
			a_b = coe_b.h*coe_b.h / (mu*(1.0 - e_b * e_b));

			cos_theta = (a_d*(1.0 - e_d) - R_MD_apo) / (e_d*R_MD_apo);
			if (abs(cos_theta) > 1.0)
			{
				if (cos_theta <= 0)
				{
					cos_theta = -1.0;
				}
				else
				{
					cos_theta = 1.0;
				}
			}
			theta = acos(cos_theta);
			r_a_apo = r_AD + (PI - theta) / PI * dr_a_max;
			r_p_apo = r_PD + theta / PI * dr_p_max;
			a_d = (r_a_apo + r_p_apo) / 2.0;
			e_d = abs((a_d - r_p_apo) / a_d);

			p_d = a_d * (1.0 - e_d * e_d);
			p_b = a_b * (1.0 - e_b * e_b);

			if (K1 < 0)
			{
				R_MD = 2.0*(p_d - p_b)*a_d*a_b / (p_d*a_d - p_b * a_b);
			}
			else
			{
				R_MD = (e_b*p_d - e_d * p_b) / (e_b - e_d);
			}

			n++;
		} while (abs(R_MD - R_MD_apo) > 1.0 && n < nmax);
	}
	//Optimum apse line rotation
	else if (code == RTCC_GMP_SAO)
	{
		OELEMENTS coe_b;
		double phi, dphi, a, b, theta_11, theta_12, dtheta1, dtheta2, dtheta;

		coe_b = OrbMech::coe_from_sv(sv1.R, sv1.V, mu);

		a = coe_b.e*coe_b.h*coe_b.h*(1.0 - cos(opt->dLOA));
		b = -coe_b.e*coe_b.h*coe_b.h*sin(opt->dLOA);
		phi = atan2(b, a);
		dphi = PI05;
		theta_11 = fmod(phi + dphi, PI2);
		theta_12 = fmod(phi - dphi, PI2);
		dtheta1 = fmod(theta_11 - coe_b.TA, PI2);
		dtheta2 = fmod(theta_12 - coe_b.TA, PI2);

		if (dtheta1 < dtheta2)
		{
			dtheta = dtheta1;
		}
		else
		{
			dtheta = dtheta2;
		}

		dt2 = OrbMech::time_theta(sv1.R, sv1.V, dtheta, mu);
		sv2 = coast(sv1, dt2);
	}
	//Apogee and perigee change + apse line rotation to specified longitude
	else if (code == RTCC_GMP_HAS)
	{
		SV sv_a, sv_p, sv2_apo;
		double r_AD, r_PD, lat_p, lng_p, dlng, dt, ddt, dlng_apo, eps, w_E, T_P;
		int n, nmax;

		ddt = 5.0*60.0;
		dt = 0.0;
		n = 0;
		nmax = 100;
		eps = 0.01*RAD;
		dlng = 1.0;
		w_E = PI2 / oapiGetPlanetPeriod(sv1.gravref);

		//If desired apogee is equal to perigee, bias perigee to prevent zero eccentricity
		if (opt->H_P == opt->H_A)
		{
			opt->H_P -= 0.1*1852.0;
		}

		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		while (abs(dlng) > eps && n < nmax)
		{
			sv2 = coast(sv1, dt);

			DV = ApoapsisPeriapsisChangeInteg(sv2, r_AD, r_PD);
			sv2_apo = sv2;
			sv2_apo.V += DV;
			T_P = OrbMech::period(sv2_apo.R, sv2_apo.V, mu);

			ApsidesDeterminationSubroutine(sv2_apo, sv_a, sv_p);
			OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_p.gravref, lat_p, lng_p);
			lng_p += -T_P * w_E*(double)opt->N;
			lng_p = fmod(lng_p, PI2);
			dlng = OrbMech::calculateDifferenceBetweenAngles(lng_p, opt->long_D);

			if (n > 0 && dlng*dlng_apo < 0 && abs(dlng_apo) < PI05)
			{
				ddt = -ddt / 2.0;
			}

			dt += ddt;
			dlng_apo = dlng;
			n++;
		}
	}

	//Maneuver calculation

	//Plane change
	if (code == RTCC_GMP_PCE || code == RTCC_GMP_PCL || code == RTCC_GMP_PCT || code == RTCC_GMP_PCH)
	{
		OELEMENTS coe_b, coe_a;
		MATRIX3 Rot;
		VECTOR3 R_a, V_a, R2_equ, V2_equ;

		Rot = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a = OrbMech::PlaneChange(coe_b, opt->dW);

		OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
		DV = rhmul(Rot, V_a - V2_equ);
	}
	//Circularization
	else if (code == RTCC_GMP_CRL || code == RTCC_GMP_CRH || code == RTCC_GMP_CRT || code == RTCC_GMP_CRA || code == RTCC_GMP_CRP)
	{
		DV = CircularizationManeuverInteg(sv2);
	}
	//Height Maneuver
	else if (code == RTCC_GMP_HOL || code == RTCC_GMP_HOT || code == RTCC_GMP_HAO || code == RTCC_GMP_HPO || code == RTCC_GMP_HOH)
	{
		DV = HeightManeuverInteg(sv2, opt->dH_D);
	}
	//Node Shift
	else if (code == RTCC_GMP_NSL || code == RTCC_GMP_NST || code == RTCC_GMP_NSH || code == RTCC_GMP_NSO)
	{
		OELEMENTS coe_b, coe_a;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a, V_a;

		Rot = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a = OrbMech::NodeShift(coe_b, opt->dLAN);

		OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
		DV = rhmul(Rot, V_a - V2_equ);
	}
	//Change apogee and perigee
	else if (code == RTCC_GMP_HBT || code == RTCC_GMP_HBH || code == RTCC_GMP_HBO || code == RTCC_GMP_HBL)
	{
		OELEMENTS coe_b, coe_a;
		double r_AD, r_PD;

		coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		DV = ApoapsisPeriapsisChangeInteg(sv2, r_AD, r_PD);
	}
	//Flight Controller Input
	else if (code == RTCC_GMP_FCT || code == RTCC_GMP_FCL || code == RTCC_GMP_FCH || code == RTCC_GMP_FCA || code == RTCC_GMP_FCP || code == RTCC_GMP_FCE)
	{
		MATRIX3 Rot;
		VECTOR3 dV_FC;

		dV_FC = _V(opt->dV*cos(opt->Pitch)*cos(opt->Yaw), opt->dV*sin(opt->Yaw), -opt->dV*sin(opt->Pitch)*cos(opt->Yaw));
		Rot = OrbMech::LVLH_Matrix(sv2.R, sv2.V);
		DV = tmul(Rot, dV_FC);
	}
	//Node Shift + Apogee/Perigee Change
	else if (code == RTCC_GMP_NHT || code == RTCC_GMP_NHL)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a, V_a, DV1, DV2;
		double r_AD, r_PD;

		sv2_apo1 = sv2;
		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		Rot = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::NodeShift(coe_b, opt->dLAN);

		OrbMech::sv_from_coe(coe_a1, mu, R_a, V_a);
		sv2_apo1.V = rhmul(Rot, V_a);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = ApoapsisPeriapsisChangeInteg(sv2_apo1, r_AD, r_PD);
		DV = DV1 + DV2;
	}
	//Height change + plane change
	else if (code == RTCC_GMP_PHL || code == RTCC_GMP_PHT || code == RTCC_GMP_PHA || code == RTCC_GMP_PHP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::PlaneChange(coe_b, opt->dW);

		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = HeightManeuverInteg(sv2_apo1, opt->dH_D);
		DV = DV1 + DV2;
	}
	//Circularization + plane change
	else if (code == RTCC_GMP_CPL || code == RTCC_GMP_CPH || code == RTCC_GMP_CPT || code == RTCC_GMP_CPA || code == RTCC_GMP_CPP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::PlaneChange(coe_b, opt->dW);

		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = CircularizationManeuverInteg(sv2_apo1);
		DV = DV1 + DV2;
	}
	//Shift line-of-apsides
	else if (code == RTCC_GMP_SAT || code == RTCC_GMP_SAO || code == RTCC_GMP_SAL)
	{
		OELEMENTS coe_b, coe_a;
		VECTOR3 R_a, V_a;
		double u;

		coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
		u = fmod(coe_b.TA + coe_b.w, PI2);

		coe_a.e = coe_b.e*cos(coe_b.TA) / cos(coe_b.TA - opt->dLOA);
		coe_a.h = coe_b.h;
		coe_a.i = coe_b.i;
		coe_a.RA = coe_b.RA;
		coe_a.TA = fmod(coe_b.TA - opt->dLOA, PI2);
		coe_a.w = fmod(u - coe_a.TA, PI2);

		OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
		DV = V_a - sv2.V;
	}
	//Shift line-of-apsides to specified longitude
	else if (code == RTCC_GMP_SAA)
	{
		SV sv_a, sv_p, sv2_apo;
		OELEMENTS coe_b, coe_a;
		VECTOR3 R_a, V_a;
		double u, lat_p, lng_p, dLOA, eps, ddLOA;
		int n, nmax;

		coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
		u = fmod(coe_b.TA + coe_b.w, PI2);

		sv2_apo = sv2;

		n = 0;
		nmax = 20;
		eps = 0.01*RAD;
		dLOA = 0.0;
		ddLOA = 1.0;

		while (abs(ddLOA) > eps && n < nmax)
		{
			ApsidesDeterminationSubroutine(sv2_apo, sv_a, sv_p);
			OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_p.gravref, lat_p, lng_p);

			ddLOA = OrbMech::calculateDifferenceBetweenAngles(lng_p, opt->long_D);
			dLOA += ddLOA;

			coe_a.e = coe_b.e*cos(coe_b.TA) / cos(coe_b.TA - dLOA);
			coe_a.h = coe_b.h;
			coe_a.i = coe_b.i;
			coe_a.RA = coe_b.RA;
			coe_a.TA = fmod(coe_b.TA - dLOA, PI2);
			coe_a.w = fmod(u - coe_a.TA, PI2);

			OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
			DV = V_a - sv2.V;
			sv2_apo.V = sv2.V + DV;
			n++;
		}
	}
	//Circularization + node shift
	else if (code == RTCC_GMP_CNL || code == RTCC_GMP_CNH || code == RTCC_GMP_CNT || code == RTCC_GMP_CNA || code == RTCC_GMP_CNP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::NodeShift(coe_b, opt->dLAN);
		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = CircularizationManeuverInteg(sv2_apo1);
		DV = DV1 + DV2;
	}
	//Height change + node shift
	else if (code == RTCC_GMP_HNL || code == RTCC_GMP_HNT || code == RTCC_GMP_HNA || code == RTCC_GMP_HNP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::NodeShift(coe_b, opt->dLAN);
		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = HeightManeuverInteg(sv2_apo1, opt->dH_D);
		DV = DV1 + DV2;
	}

	dV_i = DV;
	P30TIG = (sv2.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate orbital elements
	SV sv_a, sv_p, sv2_apo;
	OELEMENTS coe_a, coe_b;
	MATRIX3 obl, Q_Xx;
	VECTOR3 dV_LVLH, R2_equ, V2_equ, V2_equ_apo;

	sv2_apo = sv2;
	sv2_apo.V += DV;
	obl = OrbMech::GetObliquityMatrix(sv2.gravref, sv2.MJD);
	R2_equ = rhtmul(obl, sv2.R);
	V2_equ = rhtmul(obl, sv2.V);
	V2_equ_apo = rhtmul(obl, sv2_apo.V);
	coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);
	coe_a = OrbMech::coe_from_sv(R2_equ, V2_equ_apo, mu);
	ApsidesDeterminationSubroutine(sv2_apo, sv_a, sv_p);
	Q_Xx = OrbMech::LVLH_Matrix(sv2.R, sv2.V);
	dV_LVLH = mul(Q_Xx, DV);

	res.GET_A = (sv_a.MJD - opt->GETbase)*24.0*3600.0;
	res.HA = length(sv_a.R) - R_E;
	OrbMech::latlong_from_J2000(sv_a.R, sv_a.MJD, sv_a.gravref, res.lat_A, res.long_A);
	res.GET_P = (sv_p.MJD - opt->GETbase)*24.0*3600.0;
	res.HP = length(sv_p.R) - R_E;
	OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_p.gravref, res.lat_P, res.long_P);
	res.A = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e*coe_a.e));
	res.E = coe_a.e;
	res.I = coe_a.i;
	res.Node_Ang = coe_a.RA;
	res.Del_G = coe_a.w - coe_b.w;
	if (res.Del_G < 0)
	{
		res.Del_G += PI2;
	}
	res.Pitch_Man = atan2(-dV_LVLH.z, dV_LVLH.x);
	res.Yaw_Man = atan2(dV_LVLH.y, sqrt(dV_LVLH.x*dV_LVLH.x + dV_LVLH.z*dV_LVLH.z));
	res.H_Man = length(sv2.R) - R_E;
	OrbMech::latlong_from_J2000(sv2.R, sv2.MJD, sv2.gravref, res.lat_Man, res.long_Man);

	/*coe_before.elem.a = coe_b.h*coe_b.h / (mu*(1.0 - coe_b.e*coe_b.e));
	coe_before.elem.e = coe_b.e;
	coe_before.elem.i = coe_b.i;
	coe_before.elem.theta = coe_b.RA;
	coe_before.param.ApD = length(sv_a.R) - R_E;
	coe_before.param.ApT = (sv_a.MJD - opt->GETbase)*24.0*3600.0;
	coe_before.param.T = PI2 * sqrt(pow(coe_before.elem.a, 3) / mu);
	coe_before.param.PeD = length(sv_p.R) - R_E;
	coe_before.param.PeT = (sv_p.MJD - opt->GETbase)*24.0*3600.0;
	coe_before.param.TrA = coe_b.TA;

	ApsidesDeterminationSubroutine(sv2_apo, sv_a, sv_p);
	coe_after.elem.a = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e*coe_a.e));
	coe_after.elem.e = coe_a.e;
	coe_after.elem.i = coe_a.i;
	coe_after.elem.theta = coe_a.RA;
	coe_after.param.ApD = length(sv_a.R) - R_E;
	coe_after.param.ApT = (sv_a.MJD - opt->GETbase)*24.0*3600.0;
	coe_after.param.T = PI2 * sqrt(pow(coe_after.elem.a, 3) / mu);
	coe_after.param.PeD = length(sv_p.R) - R_E;
	coe_after.param.PeT = (sv_p.MJD - opt->GETbase)*24.0*3600.0;
	coe_after.param.TrA = coe_a.TA;*/

	return true;
}

void RTCC::TLI_PAD(TLIPADOpt* opt, TLIPAD &pad)
{
	MATRIX3 M_R, M, M_RTM;
	VECTOR3 R_A, V_A, R0, V0, UX, UY, UZ, DV_P, DV_C, V_G, U_TD, X_B, IgnAtt, SepATT, ExtATT;
	double boil, SVMJD, m0, mass, dt, t_go, F, v_e, theta_T, dVC;
	SV sv0, sv1, sv2, sv3;

	boil = (1.0 - 0.99998193) / 10.0;

	sv0.gravref = AGCGravityRef(opt->vessel);

	opt->vessel->GetRelativePos(sv0.gravref, R_A);
	opt->vessel->GetRelativeVel(sv0.gravref, V_A);
	SVMJD = oapiGetSimMJD();

	dt = opt->TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	R0 = _V(R_A.x, R_A.z, R_A.y);
	V0 = _V(V_A.x, V_A.z, V_A.y);

	m0 = opt->vessel->GetEmptyMass();
	mass = opt->vessel->GetMass();

	sv0.MJD = SVMJD;
	sv0.R = R0;
	sv0.V = V0;
	sv0.mass = mass;

	dt = opt->TIG - (SVMJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	sv1 = coast(sv0, dt);
	sv1.mass = m0 + (mass - m0)*exp(-boil*dt);

	if (opt->uselvdc)
	{
		t_go = opt->TLI - opt->TIG;

		UY = unit(crossp(sv1.V, -sv1.R));
		UZ = unit(sv1.R);
		UX = crossp(UY, UZ);

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		IgnAtt = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

		sv2 = sv1;
		sv2.R = opt->R_TLI;
		sv2.V = opt->V_TLI;
		sv2.MJD += t_go / 24.0 / 3600.0;

		sv3 = coast(sv2, 900.0);
	}
	else
	{

		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->TIG, opt->dV_LVLH, sv1, 0);

		sv3 = coast(sv2, 900.0);

		t_go = (sv2.MJD - sv1.MJD)*24.0*3600.0;

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		v_e = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
		F = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));

		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		if (length(DV_P) != 0.0)
		{
			theta_T = length(crossp(sv1.R, sv1.V))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(sv1.R), 2.0) / F;
			DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*opt->dV_LVLH.y;
		}
		else
		{
			V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
		}

		U_TD = unit(V_G);
		X_B = unit(V_G);

		UX = X_B;
		UY = unit(crossp(X_B, -sv1.R));
		UZ = unit(crossp(X_B, crossp(X_B, -sv1.R)));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);

		IgnAtt = OrbMech::CALCGAR(opt->REFSMMAT, M_R);
	}

	UY = unit(crossp(sv3.V, sv3.R));
	UZ = unit(-sv3.R);
	UX = crossp(UY, UZ);
	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = OrbMech::CALCSMSC(_V(PI - opt->SeparationAttitude.x, opt->SeparationAttitude.y, opt->SeparationAttitude.z));
	M_RTM = mul(M, M_R);

	SepATT = OrbMech::CALCGAR(opt->REFSMMAT, M_RTM);
	ExtATT = _V(300.0*RAD - SepATT.x, SepATT.y + PI, PI2 - SepATT.z);

	dVC = length(opt->dV_LVLH);

	pad.BurnTime = t_go;
	pad.dVC = dVC / 0.3048;
	pad.IgnATT = _V(OrbMech::imulimit(IgnAtt.x*DEG), OrbMech::imulimit(IgnAtt.y*DEG), OrbMech::imulimit(IgnAtt.z*DEG));
	pad.SepATT = _V(OrbMech::imulimit(SepATT.x*DEG), OrbMech::imulimit(SepATT.y*DEG), OrbMech::imulimit(SepATT.z*DEG));
	pad.ExtATT = _V(OrbMech::imulimit(ExtATT.x*DEG), OrbMech::imulimit(ExtATT.y*DEG), OrbMech::imulimit(ExtATT.z*DEG));
	pad.TB6P = opt->TIG - 577.6;
	pad.VI = length(sv2.V) / 0.3048;
}

bool RTCC::PDI_PAD(PDIPADOpt* opt, AP11PDIPAD &pad)
{
	SV sv0, sv1, sv2, sv_I;
	VECTOR3 U_FDP;
	double C_R, TTT, t_IG;

	sv0 = StateVectorCalc(opt->vessel);

	if (opt->direct)
	{
		sv2 = sv0;
	}
	else
	{
		sv2 = ExecuteManeuver(opt->vessel, opt->GETbase, opt->P30TIG, opt->dV_LVLH, sv0, 0.0);
	}

	if (!PDIIgnitionAlgorithm(sv2, opt->GETbase, opt->R_LS, opt->t_land, opt->REFSMMAT, sv_I, TTT, C_R, U_FDP))
	{
		return false;
	}

	t_IG = OrbMech::GETfromMJD(sv_I.MJD, opt->GETbase);

	VECTOR3 X_B, UX, UY, UZ, IMUangles, FDAIangles;
	MATRIX3 M, M_R;
	double headsswitch;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = tmul(opt->REFSMMAT, unit(U_FDP));
	UX = X_B;
	if (abs(dotp(UX, unit(sv_I.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv_I.R*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, sv_I.V));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.CR = C_R / 1852.0;
	pad.DEDA231 = length(opt->R_LS) / 0.3048 / 100.0;
	pad.GETI = t_IG;
	pad.t_go = -TTT;

	return true;
}

void RTCC::LunarAscentPAD(ASCPADOpt opt, AP11LMASCPAD &pad)
{
	SV sv_Ins, sv_CSM_TIG;
	MATRIX3 Rot_LG, Rot_VG;
	VECTOR3 R_LSP, Q, U_R, Z_B, X_AGS, Y_AGS, Z_AGS;
	double CR, MJD_TIG, SMa, R_D, delta_L, sin_DL, cos_DL;

	MJD_TIG = OrbMech::MJDfromGET(opt.TIG, opt.GETbase);
	sv_CSM_TIG = GeneralTrajectoryPropagation(opt.sv_CSM, 0, MJD_TIG);

	Rot_LG = OrbMech::GetRotationMatrix(sv_CSM_TIG.gravref, MJD_TIG);
	R_LSP = rhmul(Rot_LG, opt.R_LS);

	Q = unit(crossp(sv_CSM_TIG.V, sv_CSM_TIG.R));
	U_R = unit(R_LSP);
	R_D = length(R_LSP) + 60000.0*0.3048;
	CR = -R_D * asin(dotp(U_R, Q));

	sv_Ins.R = _V(R_D, 0, 0);
	sv_Ins.V = _V(opt.v_LV, opt.v_LH, 0);
	sv_Ins.gravref = sv_CSM_TIG.gravref;
	SMa = GetSemiMajorAxis(sv_Ins);

	Rot_VG = OrbMech::GetVesselToGlobalRotMatrix(opt.Rot_VL, Rot_LG);
	Z_B = mul(Rot_VG, _V(0, 0, 1));
	Z_B = unit(_V(Z_B.x, Z_B.z, Z_B.y));
	X_AGS = U_R;
	Z_AGS = unit(crossp(-Q, X_AGS));
	Y_AGS = unit(crossp(Z_AGS, X_AGS));

	delta_L = atan2(dotp(Z_B, Z_AGS), dotp(Z_B, Y_AGS));
	sin_DL = sin(delta_L);
	cos_DL = cos(delta_L);

	pad.CR = CR / 1852.0;
	pad.TIG = opt.TIG;
	pad.V_hor = opt.v_LH / 0.3048;
	pad.V_vert = opt.v_LV / 0.3048;
	pad.DEDA225_226 = SMa / 0.3048 / 100.0;
	pad.DEDA231 = length(opt.R_LS) / 0.3048 / 100.0;
	sprintf(pad.remarks, "");
	pad.DEDA047 = OrbMech::DoubleToDEDA(sin_DL, 14);
	pad.DEDA053 = OrbMech::DoubleToDEDA(cos_DL, 14);
}

void RTCC::AGCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH, int DVAddr)
{
	double getign = P30TIG;
	int emem[24];

	emem[0] = 12;
	emem[1] = DVAddr;
	emem[2] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	emem[3] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	emem[4] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	emem[5] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	emem[8] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	emem[9] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	V71Update(str, emem, 10);
}

void RTCC::AGCStateVectorUpdate(char *str, SV sv, bool csm, double AGCEpoch, double GETbase, bool v66)
{
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	MATRIX3 Rot = OrbMech::J2000EclToBRCS(AGCEpoch);

	VECTOR3 vel, pos;
	double get;
	int emem[24];

	pos = mul(Rot, sv.R);
	vel = mul(Rot, sv.V)*0.01;
	get = (sv.MJD - GETbase)*24.0*3600.0;

	if (sv.gravref == hMoon) {

		emem[0] = 21;
		emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (csm)
		{
			emem[2] = 2;
		}
		else
		{
			emem[2] = 77775;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	else if (sv.gravref == hEarth) {

		emem[0] = 21;
		emem[1] = 1501;

		if (csm)
		{
			emem[2] = 1;
		}
		else
		{
			emem[2] = 77776;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	V71Update(str, emem, 17);
	if (v66)
	{
		sprintf(str, "%sV66EV37E00E", str);
	}
}

void RTCC::LandingSiteUplink(char *str, double lat, double lng, double alt, int RLSAddr)
{
	VECTOR3 R_P, R;
	double r_0;
	int emem[8];

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)));
	r_0 = oapiGetSize(oapiGetObjectByName("Moon"));

	R = R_P * (r_0 + alt);

	emem[0] = 10;
	emem[1] = RLSAddr;
	emem[2] = OrbMech::DoubleToBuffer(R.x, 27, 1);
	emem[3] = OrbMech::DoubleToBuffer(R.x, 27, 0);
	emem[4] = OrbMech::DoubleToBuffer(R.y, 27, 1);
	emem[5] = OrbMech::DoubleToBuffer(R.y, 27, 0);
	emem[6] = OrbMech::DoubleToBuffer(R.z, 27, 1);
	emem[7] = OrbMech::DoubleToBuffer(R.z, 27, 0);

	V71Update(str, emem, 8);
}

void RTCC::IncrementAGCTime(char *list, double dt)
{
	int emem[2];

	sprintf(list, "V73E");

	emem[0] = OrbMech::DoubleToBuffer(dt, 28, 1);
	emem[1] = OrbMech::DoubleToBuffer(dt, 28, 0);

	sprintf(list, "%s%dE", list, emem[0]);
	sprintf(list, "%s%dE", list, emem[1]);
	sprintf(list, "%sV33E", list);
}

void RTCC::V71Update(char *list, int *emem, int n)
{
	sprintf(list, "V71E%dE", emem[0]);
	for (int i = 1;i < n;i++)
	{
		sprintf(list, "%s%dE", list, emem[i]);
	}
	sprintf(list, "%sV33E", list);
}

void RTCC::TLANDUpdate(char *list, double t_land, int tlandaddr)
{
	int emem[5];
	emem[0] = 5;

	emem[1] = tlandaddr;
	emem[3] = tlandaddr + 1;

	emem[2] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 1);
	emem[4] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 0);

	V72Update(list, emem, 5);
}

void RTCC::V72Update(char *list, int *emem, int n)
{
	sprintf(list, "V72E%dE", emem[0]);
	for (int i = 1;i < n;i++)
	{
		sprintf(list, "%s%dE", list, emem[i]);
	}
	sprintf(list, "%sV33E", list);
}

void RTCC::SunburstAttitudeManeuver(char *list, VECTOR3 imuangles)
{
	int emem[3];

	if (imuangles.x > PI)
	{
		imuangles.x -= PI2;
	}
	if (imuangles.y > PI)
	{
		imuangles.y -= PI2;
	}
	if (imuangles.z > PI)
	{
		imuangles.z -= PI2;
	}

	emem[0] = OrbMech::DoubleToBuffer(imuangles.x / PI, 0, 1);
	emem[1] = OrbMech::DoubleToBuffer(imuangles.y / PI, 0, 1);
	emem[2] = OrbMech::DoubleToBuffer(imuangles.z / PI, 0, 1);

	sprintf(list, "V21N1E372E0EV25N1E1631E%dE%dE%dEV25N26E20001E2067E70063EV30ER", emem[0], emem[1], emem[2]);
}

void RTCC::SunburstLMPCommand(char *list, int code)
{
	sprintf(list, "V67E%oEV33ER", code);
}

void RTCC::SunburstMassUpdate(char *list, double masskg)
{
	int emem[2];

	emem[0] = OrbMech::DoubleToBuffer(masskg, 15, 1);
	emem[1] = OrbMech::DoubleToBuffer(masskg, 15, 0);

	sprintf(list, "V24N1E1320E%dE%dER", emem[0], emem[1]);
}

void RTCC::AGCDesiredREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, bool cmc, bool AGCCoordSystem)
{
	MATRIX3 a;
	int emem[24];

	if (AGCCoordSystem)
	{
		a = REFSMMAT;
	}
	else
	{
		a = mul(REFSMMAT, OrbMech::tmat(OrbMech::J2000EclToBRCS(AGCEpoch)));
	}

	emem[0] = 24;
	if (cmc)
	{
		emem[1] = 306;
	}
	else
	{
		emem[1] = 3606;
	}
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	V71Update(list, emem, 20);
}

void RTCC::AGCREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, int offset, bool AGCCoordSystem)
{
	MATRIX3 a;
	int emem[24];

	if (AGCCoordSystem)
	{
		a = REFSMMAT;
	}
	else
	{
		a = mul(REFSMMAT, OrbMech::tmat(OrbMech::J2000EclToBRCS(AGCEpoch)));
	}

	emem[0] = 24;
	emem[1] = 1735 + offset;
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	V71Update(list, emem, 20);
}

void RTCC::CMCRetrofireExternalDeltaVUpdate(char *list, double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH)
{
	int emem[24];
	double getign = P30TIG;

	emem[0] = 16;
	emem[1] = 3400;
	emem[2] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 1);
	emem[3] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 0);
	emem[4] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 1);
	emem[5] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 0);
	emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	emem[8] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	emem[9] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	emem[10] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	emem[11] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	emem[12] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	emem[13] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	V71Update(list, emem, 14);
}

void RTCC::CMCEntryUpdate(char *list, double LatSPL, double LngSPL)
{
	int emem[24];

	emem[0] = 06;
	emem[1] = 3400;
	emem[2] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 1);
	emem[3] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 0);
	emem[4] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 1);
	emem[5] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 0);

	V71Update(list, emem, 6);
}

void RTCC::AGSStateVectorPAD(AGSSVOpt *opt, AP11AGSSVPAD &pad)
{
	SV sv1;
	VECTOR3 R, V;
	double AGSEpochTime, AGSEpochTime2, AGSEpochTime3, dt, scalR, scalV;

	AGSEpochTime = (opt->sv.MJD - opt->GETbase)*24.0*3600.0;
	AGSEpochTime2 = ceil(AGSEpochTime / 6.0)*6.0;

	dt = AGSEpochTime2 - AGSEpochTime;

	sv1 = coast(opt->sv, dt);

	R = mul(opt->REFSMMAT, sv1.R);
	V = mul(opt->REFSMMAT, sv1.V);

	AGSEpochTime3 = AGSEpochTime2 - opt->AGSbase;

	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		scalR = 1000.0;
		scalV = 1.0;
	}
	else
	{
		scalR = 100.0;
		scalV = 0.1;
	}

	if (opt->csm)
	{
		pad.DEDA244 = R.x / 0.3048 / scalR;
		pad.DEDA245 = R.y / 0.3048 / scalR;
		pad.DEDA246 = R.z / 0.3048 / scalR;
		pad.DEDA264 = V.x / 0.3048 / scalV;
		pad.DEDA265 = V.y / 0.3048 / scalV;
		pad.DEDA266 = V.z / 0.3048 / scalV;
		pad.DEDA272 = AGSEpochTime3 / 60.0;
	}
	else
	{
		pad.DEDA240 = R.x / 0.3048 / scalR;
		pad.DEDA241 = R.y / 0.3048 / scalR;
		pad.DEDA242 = R.z / 0.3048 / scalR;
		pad.DEDA260 = V.x / 0.3048 / scalV;
		pad.DEDA261 = V.y / 0.3048 / scalV;
		pad.DEDA262 = V.z / 0.3048 / scalV;
		pad.DEDA254 = AGSEpochTime3 / 60.0;
	}
}

void RTCC::NavCheckPAD(SV sv, AP7NAV &pad, double GETbase, double GET)
{
	double lat, lng, alt, navcheckdt;
	VECTOR3 R1, V1;
	OBJHANDLE outgrav = NULL;
	
	if (GET == 0.0)
	{
		navcheckdt = 30 * 60;
		pad.NavChk[0] = (sv.MJD - GETbase)*24.0*3600.0 + navcheckdt;
	}
	else
	{
		navcheckdt = GET - (sv.MJD - GETbase)*24.0*3600.0;
		pad.NavChk[0] = GET;
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, navcheckdt, R1, V1, sv.gravref, outgrav);
	navcheck(R1, V1, sv.MJD + navcheckdt/24.0/3600.0, outgrav, lat, lng, alt);

	pad.alt[0] = alt / 1852.0;
	pad.lat[0] = lat*DEG;
	pad.lng[0] = lng*DEG;
}

void RTCC::P27PADCalc(P27Opt *opt, double AGCEpoch, P27PAD &pad)
{
	double lat, lng, alt, get, SVMJD;
	VECTOR3 pos, vel;
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");
	bool csm = true;
	SV sv, sv1;

	SVMJD = opt->GETbase + opt->SVGET / 24.0 / 3600.0;

	sv = StateVectorCalc(opt->vessel, SVMJD);
	sv1 = coast(sv, opt->navcheckGET - opt->SVGET);

	navcheck(sv1.R, sv1.V, sv1.MJD, sv1.gravref, lat, lng, alt);
	
	sprintf(pad.Purpose[0], "SV");
	pad.GET[0] = opt->SVGET;
	pad.alt = alt / 1852.0;
	pad.lat = lat*DEG;
	pad.lng = lng*DEG;
	pad.NavChk = opt->navcheckGET;
	pad.Index[0] = 21;
	pad.Verb[0] = 71;

	pos = mul(OrbMech::J2000EclToBRCS(AGCEpoch), sv.R);
	vel = mul(OrbMech::J2000EclToBRCS(AGCEpoch), sv.V)*0.01;
	get = opt->SVGET;

	if (sv.gravref == hMoon) {

		pad.Data[0][0] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (csm)
		{
			pad.Data[0][1] = 2;
		}
		else
		{
			pad.Data[0][1] = 77775;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

	if (sv.gravref == hEarth) {

		pad.Data[0][0] = 1501;

		if (csm)
		{
			pad.Data[0][1] = 1;
		}
		else
		{
			pad.Data[0][1] = 77776;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

}

void RTCC::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, RTCC_START_STRING);
	// Booleans
	// Integers
	SAVE_INT("RTCC_REFSMMATType", REFSMMATType);
	// Floats
	SAVE_DOUBLE("RTCC_P30TIG", TimeofIgnition);
	SAVE_DOUBLE("RTCC_SplLat", SplashLatitude);
	SAVE_DOUBLE("RTCC_SplLng", SplashLongitude);
	SAVE_DOUBLE("RTCC_TEI", calcParams.TEI);
	SAVE_DOUBLE("RTCC_EI", calcParams.EI);
	SAVE_DOUBLE("RTCC_TLI", calcParams.TLI);
	SAVE_DOUBLE("RTCC_LOI", calcParams.LOI);
	SAVE_DOUBLE("RTCC_SEP", calcParams.SEP);
	SAVE_DOUBLE("RTCC_DOI", calcParams.DOI);
	SAVE_DOUBLE("RTCC_PDI", calcParams.PDI);
	SAVE_DOUBLE("RTCC_TLAND", calcParams.TLAND);
	SAVE_DOUBLE("RTCC_LSAlt", calcParams.LSAlt);
	SAVE_DOUBLE("RTCC_LSAzi", calcParams.LSAzi);
	SAVE_DOUBLE("RTCC_LSLat", calcParams.LSLat);
	SAVE_DOUBLE("RTCC_LSLng", calcParams.LSLng);
	SAVE_DOUBLE("RTCC_LunarLiftoff", calcParams.LunarLiftoff);
	SAVE_DOUBLE("RTCC_Insertion", calcParams.Insertion);
	SAVE_DOUBLE("RTCC_Phasing", calcParams.Phasing);
	SAVE_DOUBLE("RTCC_CSI", calcParams.CSI);
	SAVE_DOUBLE("RTCC_CDH", calcParams.CDH);
	SAVE_DOUBLE("RTCC_TPI", calcParams.TPI);
	SAVE_DOUBLE("RTCC_alt_node", calcParams.alt_node);
	SAVE_DOUBLE("RTCC_GET_node", calcParams.GET_node);
	SAVE_DOUBLE("RTCC_lat_node", calcParams.lat_node);
	SAVE_DOUBLE("RTCC_lng_node", calcParams.lng_node);
	SAVE_DOUBLE("RTCC_TEPHEM", calcParams.TEPHEM);
	SAVE_DOUBLE("RTCC_PericynthionLatitude", calcParams.PericynthionLatitude);
	SAVE_DOUBLE("RTCC_TIGSTORE1", calcParams.TIGSTORE1);
	// Strings
	// Vectors
	SAVE_V3("RTCC_DVLVLH", DeltaV_LVLH);
	SAVE_V3("RTCC_R_TLI", calcParams.R_TLI);
	SAVE_V3("RTCC_V_TLI", calcParams.V_TLI);
	SAVE_V3("RTCC_DVSTORE1", calcParams.DVSTORE1);
	// Matrizes
	SAVE_M3("RTCC_StoredREFSMMAT", calcParams.StoredREFSMMAT);
	// State vectors
	papiWriteScenario_SV(scn, "RTCC_SVSTORE1", calcParams.SVSTORE1);
	oapiWriteLine(scn, RTCC_END_STRING);
}

// Load State
void RTCC::LoadState(FILEHANDLE scn) {
	char *line;
	int tmp = 0; // Used in boolean type loader

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, RTCC_END_STRING, sizeof(RTCC_END_STRING))) {
			break;
		}
		LOAD_INT("RTCC_REFSMMATType", REFSMMATType);
		LOAD_DOUBLE("RTCC_P30TIG", TimeofIgnition);
		LOAD_DOUBLE("RTCC_SplLat", SplashLatitude);
		LOAD_DOUBLE("RTCC_SplLng", SplashLongitude);
		LOAD_DOUBLE("RTCC_TEI", calcParams.TEI);
		LOAD_DOUBLE("RTCC_EI", calcParams.EI);
		LOAD_DOUBLE("RTCC_TLI", calcParams.TLI);
		LOAD_DOUBLE("RTCC_LOI", calcParams.LOI);
		LOAD_DOUBLE("RTCC_SEP", calcParams.SEP);
		LOAD_DOUBLE("RTCC_DOI", calcParams.DOI);
		LOAD_DOUBLE("RTCC_PDI", calcParams.PDI);
		LOAD_DOUBLE("RTCC_TLAND", calcParams.TLAND);
		LOAD_DOUBLE("RTCC_LSAlt", calcParams.LSAlt);
		LOAD_DOUBLE("RTCC_LSAzi", calcParams.LSAzi);
		LOAD_DOUBLE("RTCC_LSLat", calcParams.LSLat);
		LOAD_DOUBLE("RTCC_LSLng", calcParams.LSLng);
		LOAD_DOUBLE("RTCC_LunarLiftoff", calcParams.LunarLiftoff);
		LOAD_DOUBLE("RTCC_Insertion", calcParams.Insertion);
		LOAD_DOUBLE("RTCC_Phasing", calcParams.Phasing);
		LOAD_DOUBLE("RTCC_CSI", calcParams.CSI);
		LOAD_DOUBLE("RTCC_CDH", calcParams.CDH);
		LOAD_DOUBLE("RTCC_TPI", calcParams.TPI);
		LOAD_DOUBLE("RTCC_alt_node", calcParams.alt_node);
		LOAD_DOUBLE("RTCC_GET_node", calcParams.GET_node);
		LOAD_DOUBLE("RTCC_lat_node", calcParams.lat_node);
		LOAD_DOUBLE("RTCC_lng_node", calcParams.lng_node);
		LOAD_DOUBLE("RTCC_TEPHEM", calcParams.TEPHEM);
		LOAD_DOUBLE("RTCC_PericynthionLatitude", calcParams.PericynthionLatitude);
		LOAD_DOUBLE("RTCC_TIGSTORE1", calcParams.TIGSTORE1);
		LOAD_V3("RTCC_DVLVLH", DeltaV_LVLH);
		LOAD_V3("RTCC_R_TLI", calcParams.R_TLI);
		LOAD_V3("RTCC_V_TLI", calcParams.V_TLI);
		LOAD_V3("RTCC_DVSTORE1", calcParams.DVSTORE1);
		LOAD_M3("RTCC_StoredREFSMMAT", calcParams.StoredREFSMMAT);
		papiReadScenario_SV(line, "RTCC_SVSTORE1", calcParams.SVSTORE1);
	}
	return;
}

void RTCC::SetManeuverData(double TIG, VECTOR3 DV)
{
	TimeofIgnition = (TIG - getGETBase())*24.0*3600.0;
	DeltaV_LVLH = DV;
}

int RTCC::SPSRCSDecision(double a, VECTOR3 dV_LVLH)
{
	double t;

	t = length(dV_LVLH) / a;

	if (t > 0.5)
	{
		return RTCC_ENGINETYPE_SPSDPS;
	}
	else
	{
		return RTCC_ENGINETYPE_RCS;
	}
}

SV RTCC::ExecuteManeuver(VESSEL* vessel, double GETbase, double P30TIG, VECTOR3 dV_LVLH, SV sv, double attachedMass, double F, double isp)
{
	//INPUT:
	//vessel: vessel interface
	//GETbase: usually launch MJD
	//P30TIG: Time of ignition in seconds relative to GETbase
	//dV_LVLH: DV Vector in LVLH coordinates
	//sv: state vector on trajectory before the maneuver, doesn't have to be at TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//optional:
	//F: thrust in Newton
	//isp: specific impulse in m/s
	//OUTPUT:
	//sv3: state vector at cutoff

	if (length(dV_LVLH) == 0.0)
	{
		return sv;
	}

	double t_go, theta_T;
	VECTOR3 UX, UY, UZ, DV, DV_P, V_G, DV_C;
	SV sv2, sv3;

	if (vessel->GetGroupThruster(THGROUP_MAIN, 0))
	{
		if (F == 0.0)
		{
			F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
		}

		if (isp == 0.0)
		{
			isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
		}
	}
	else
	{
		if (F == 0.0)
		{
			F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
		}

		if (isp == 0.0)
		{
			isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
		}
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, P30TIG - (sv.MJD - GETbase)*24.0*3600.0, sv2.R, sv2.V, sv.gravref, sv2.gravref);
	sv2.mass = sv.mass;
	sv2.MJD = GETbase + P30TIG / 24.0 / 3600.0;

	UY = unit(crossp(sv2.V, sv2.R));
	UZ = unit(-sv2.R);
	UX = crossp(UY, UZ);
	DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;

	if (length(DV_P) != 0.0)
	{
		theta_T = length(crossp(sv2.R, sv2.V))*length(dV_LVLH)*(sv2.mass + attachedMass) / OrbMech::power(length(sv2.R), 2.0) / F;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;
	}
	else
	{
		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	}

	OrbMech::poweredflight(sv2.R, sv2.V, sv2.MJD, sv2.gravref, F, isp, sv2.mass + attachedMass, V_G, sv3.R, sv3.V, sv3.mass, t_go);
	sv3.gravref = sv2.gravref;
	sv3.MJD = sv2.MJD + t_go / 24.0 / 3600.0;
	sv3.mass -= attachedMass;

	return sv3;
}

SV RTCC::ExecuteManeuver(VESSEL* vessel, double GETbase, double P30TIG, VECTOR3 dV_LVLH, SV sv, double attachedMass, MATRIX3 &Q_Xx, VECTOR3 &V_G, double F, double isp)
{
	//INPUT:
	//vessel: vessel interface
	//GETbase: usually launch MJD
	//P30TIG: Time of ignition in seconds relative to GETbase
	//dV_LVLH: DV Vector in LVLH coordinates
	//sv: state vector on trajectory before the maneuver, doesn't have to be at TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//optional:
	//F: thrust in Newton
	//isp: specific impulse in m/s
	//OUTPUT:
	//sv3: state vector at cutoff

	double t_go, theta_T;
	VECTOR3 UX, UY, UZ, DV, DV_P, DV_C;
	SV sv2, sv3;

	if (vessel)
	{
		if (vessel->GetGroupThruster(THGROUP_MAIN, 0))
		{
			if (F == 0.0)
			{
				F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
			}

			if (isp == 0.0)
			{
				isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_MAIN, 0));
			}
		}
		else
		{
			if (F == 0.0)
			{
				F = vessel->GetThrusterMax0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
			}

			if (isp == 0.0)
			{
				isp = vessel->GetThrusterIsp0(vessel->GetGroupThruster(THGROUP_HOVER, 0));
			}
		}
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, P30TIG - (sv.MJD - GETbase)*24.0*3600.0, sv2.R, sv2.V, sv.gravref, sv2.gravref);
	sv2.mass = sv.mass;
	sv2.MJD = GETbase + P30TIG / 24.0 / 3600.0;

	UY = unit(crossp(sv2.V, sv2.R));
	UZ = unit(-sv2.R);
	UX = crossp(UY, UZ);
	Q_Xx = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;

	if (length(DV_P) != 0.0)
	{
		theta_T = length(crossp(sv2.R, sv2.V))*length(dV_LVLH)*(sv2.mass + attachedMass) / OrbMech::power(length(sv2.R), 2.0) / F;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;
	}
	else
	{
		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	}

	OrbMech::poweredflight(sv2.R, sv2.V, sv2.MJD, sv2.gravref, F, isp, sv2.mass + attachedMass, V_G, sv3.R, sv3.V, sv3.mass, t_go);
	sv3.gravref = sv2.gravref;
	sv3.MJD = sv2.MJD + t_go / 24.0 / 3600.0;
	sv3.mass -= attachedMass;

	return sv3;
}

void RTCC::RTEFlybyTargeting(RTEFlybyOpt *opt, EntryResults *res)
{
	Flyby* flybycalc;
	bool endi = false;
	double EMSAlt, dt22, mu_E, MJDguess, CSMmass, LMmass;
	VECTOR3 R05G, V05G, R_A, V_A, R0M, V0M;
	VECTOR3 Llambda, R_cor, V_cor;
	double t_slip, SVMJD;
	MATRIX3 Q_Xx;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	OBJHANDLE gravref = AGCGravityRef(opt->vessel);

	EMSAlt = 297431.0*0.3048;
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		R0M = opt->RV_MCC.R;
		V0M = opt->RV_MCC.V;
		SVMJD = opt->RV_MCC.MJD;
		gravref = opt->RV_MCC.gravref;
		CSMmass = opt->RV_MCC.mass;
	}
	else
	{
		gravref = AGCGravityRef(opt->vessel);
		opt->vessel->GetRelativePos(gravref, R_A);
		opt->vessel->GetRelativeVel(gravref, V_A);
		SVMJD = oapiGetSimMJD();
		R0M = _V(R_A.x, R_A.z, R_A.y);
		V0M = _V(V_A.x, V_A.z, V_A.y);

		CSMmass = opt->vessel->GetMass();
	}


	if (opt->TIGguess == 0.0)
	{
		//TEI targeting uses "now" as the initial guess
		MJDguess = SVMJD;
	}
	else
	{
		MJDguess = opt->GETbase + opt->TIGguess / 24.0 / 3600.0;
	}

	flybycalc = new Flyby(R0M, V0M, SVMJD, gravref, MJDguess, opt->EntryLng, opt->entrylongmanual, opt->returnspeed, opt->FlybyType, opt->Inclination, opt->Ascending);

	while (!endi)
	{
		endi = flybycalc->Flybyiter();
	}

	dt22 = OrbMech::time_radius(flybycalc->R_EI, flybycalc->V_EI, oapiGetSize(hEarth) + EMSAlt, -1, mu_E);
	OrbMech::rv_from_r0v0(flybycalc->R_EI, flybycalc->V_EI, dt22, R05G, V05G, mu_E); //Entry Interface to 0.05g

	res->latitude = flybycalc->EntryLatcor;
	res->longitude = flybycalc->EntryLngcor;
	res->P30TIG = (flybycalc->TIG - opt->GETbase)*24.0*3600.0;
	res->dV_LVLH = flybycalc->Entry_DV;
	res->ReA = flybycalc->EntryAng;
	res->GET400K = (flybycalc->EIMJD - opt->GETbase)*24.0*3600.0;
	res->GET05G = res->GET400K + dt22;
	res->RTGO = 1285.0 - 3437.7468*acos(dotp(unit(flybycalc->R_EI), unit(R05G)));
	res->VIO = length(V05G);
	res->precision = flybycalc->precision;
	res->Incl = flybycalc->ReturnInclination;
	res->FlybyAlt = flybycalc->FlybyPeriAlt;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	SV sv_tig;

	sv_tig.gravref = hMoon;
	sv_tig.mass = CSMmass;
	sv_tig.MJD = flybycalc->TIG;
	sv_tig.R = flybycalc->Rig;
	sv_tig.V = flybycalc->Vig;

	res->sv_postburn = sv_tig;
	res->sv_postburn.V = flybycalc->Vig_apo;

	FiniteBurntimeCompensation(opt->vesseltype, sv_tig, LMmass, flybycalc->Vig_apo - flybycalc->Vig, true, Llambda, t_slip);

	OrbMech::oneclickcoast(flybycalc->Rig, flybycalc->Vig, flybycalc->TIG, t_slip, R_cor, V_cor, hMoon, hMoon);

	Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

	//EntryTIGcor = teicalc->TIG_imp + t_slip;
	res->P30TIG = (flybycalc->TIG - opt->GETbase)*24.0*3600.0 + t_slip;
	res->dV_LVLH = mul(Q_Xx, Llambda);

	delete flybycalc;
}

void RTCC::TEITargeting(TEIOpt *opt, EntryResults *res)
{
	TEI* teicalc;
	SV sv0;
	bool endi = false;
	double EMSAlt, dt22, mu_E, MJDguess, LMmass;
	VECTOR3 R05G, V05G;
	VECTOR3 Llambda, R_cor, V_cor;
	double t_slip;
	MATRIX3 Q_Xx;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");

	EMSAlt = 297431.0*0.3048;
	mu_E = GGRAV*oapiGetMass(hEarth);

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}


	if (opt->TIGguess == 0.0)
	{
		//TEI targeting uses "now" as the initial guess
		MJDguess = sv0.MJD;
	}
	else
	{
		MJDguess = opt->GETbase + opt->TIGguess / 24.0 / 3600.0;
	}
	
	teicalc = new TEI(sv0.R, sv0.V, sv0.MJD, sv0.gravref, MJDguess, opt->EntryLng, opt->entrylongmanual, opt->returnspeed, opt->RevsTillTEI, opt->Inclination, opt->Ascending);

	while (!endi)
	{
		endi = teicalc->TEIiter();
	}

	dt22 = OrbMech::time_radius(teicalc->R_EI, teicalc->V_EI, oapiGetSize(hEarth) + EMSAlt, -1, mu_E);
	OrbMech::rv_from_r0v0(teicalc->R_EI, teicalc->V_EI, dt22, R05G, V05G, mu_E); //Entry Interface to 0.05g

	res->latitude = teicalc->EntryLatcor;
	res->longitude = teicalc->EntryLngcor;
	res->P30TIG = (teicalc->TIG - opt->GETbase)*24.0*3600.0;
	res->dV_LVLH = teicalc->Entry_DV;
	res->ReA = teicalc->EntryAng;
	res->GET400K = (teicalc->EIMJD - opt->GETbase)*24.0*3600.0;
	res->GET05G = res->GET400K + dt22;
	res->RTGO = 1285.0 - 3437.7468*acos(dotp(unit(teicalc->R_EI), unit(R05G)));
	res->VIO = length(V05G);
	res->precision = teicalc->precision;
	res->Incl = teicalc->ReturnInclination;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	SV sv_tig;

	sv_tig.gravref = hMoon;
	sv_tig.mass = sv0.mass;
	sv_tig.MJD = teicalc->TIG;
	sv_tig.R = teicalc->Rig;
	sv_tig.V = teicalc->Vig;

	FiniteBurntimeCompensation(opt->vesseltype, sv_tig, LMmass, teicalc->Vig_apo - teicalc->Vig, 1, Llambda, t_slip);

	OrbMech::oneclickcoast(teicalc->Rig, teicalc->Vig, teicalc->TIG, t_slip, R_cor, V_cor, hMoon, hMoon);

	Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

	//EntryTIGcor = teicalc->TIG_imp + t_slip;
	res->P30TIG = (teicalc->TIG - opt->GETbase)*24.0*3600.0 + t_slip;
	res->dV_LVLH = mul(Q_Xx, Llambda);

	delete teicalc;
}

void RTCC::LunarOrbitMapUpdate(SV sv0, double GETbase, AP10MAPUPDATE &pad, double pm)
{
	double ttoLOS, ttoAOS, ttoSS, ttoSR, ttoPM;
	OBJHANDLE hEarth, hSun;

	double t_lng;

	hEarth = oapiGetObjectByName("Earth");
	hSun = oapiGetObjectByName("Sun");

	ttoLOS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hEarth, 0, 0, true);
	ttoAOS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hEarth, 1, 0, true);

	pad.LOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoLOS;
	pad.AOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoAOS;

	ttoSS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hSun, 0, 0, true);
	ttoSR = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hSun, 1, 0, true);

	pad.SSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoSS;
	pad.SRGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoSR;

	t_lng = OrbMech::P29TimeOfLongitude(sv0.R, sv0.V, sv0.MJD, sv0.gravref, pm);
	ttoPM = (t_lng - sv0.MJD)*24.0 * 3600.0;
	pad.PMGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoPM;
}

void RTCC::LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad)
{
	VECTOR3 RA0_orb, VA0_orb, RA0, VA0, R_P, RA1, VA1, u;
	double SVMJD, dt1, dt2, get, MJDguess, sinl, gamma, r_0, LmkRange;
	OBJHANDLE hEarth, hMoon, gravref;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	gravref = AGCGravityRef(opt->vessel);

	opt->vessel->GetRelativePos(gravref, RA0_orb);
	opt->vessel->GetRelativeVel(gravref, VA0_orb);
	SVMJD = oapiGetSimMJD();
	get = (SVMJD - opt->GETbase)*24.0*3600.0;
	RA0 = _V(RA0_orb.x, RA0_orb.z, RA0_orb.y);
	VA0 = _V(VA0_orb.x, VA0_orb.z, VA0_orb.y);

	for (int i = 0;i < opt->entries;i++)
	{
		MJDguess = opt->GETbase + opt->LmkTime[i] / 24.0 / 3600.0;

		R_P = unit(_V(cos(opt->lng[i])*cos(opt->lat[i]), sin(opt->lng[i])*cos(opt->lat[i]), sin(opt->lat[i])))*(oapiGetSize(gravref) + opt->alt[i]);

		OrbMech::oneclickcoast(RA0, VA0, SVMJD, opt->LmkTime[i] - get, RA1, VA1, gravref, gravref);

		dt1 = OrbMech::findelev_gs(RA1, VA1, R_P, MJDguess, 180.0*RAD, gravref, LmkRange);
		dt2 = OrbMech::findelev_gs(RA1, VA1, R_P, MJDguess, 145.0*RAD, gravref, LmkRange);

		pad.T1[i] = dt1 + (MJDguess - opt->GETbase) * 24.0 * 60.0 * 60.0;
		pad.T2[i] = dt2 + (MJDguess - opt->GETbase) * 24.0 * 60.0 * 60.0;

		u = unit(R_P);
		sinl = u.z;

		if (gravref == hEarth)
		{
			double a, b, r_F;
			a = 6378166;
			b = 6356784;
			gamma = b * b / a / a;
			r_F = sqrt(b*b / (1.0 - (1.0 - b * b / a / a)*(1.0 - sinl * sinl)));
			r_0 = r_F;
		}
		else
		{
			gamma = 1.0;
			r_0 = oapiGetSize(gravref);
		}

		pad.Alt[i] = (length(R_P) - r_0) / 1852.0;
		pad.CRDist[i] = LmkRange / 1852.0;

		pad.Lat[i] = atan2(u.z, gamma*sqrt(u.x*u.x + u.y*u.y))*DEG;
		pad.Lng05[i] = opt->lng[i] / 2.0*DEG;
	}

	pad.entries = opt->entries;
}

SV RTCC::coast(SV sv0, double dt)
{
	SV sv1;
	OBJHANDLE gravout = NULL;

	OrbMech::oneclickcoast(sv0.R, sv0.V, sv0.MJD, dt, sv1.R, sv1.V, sv0.gravref, gravout);
	sv1.gravref = gravout;
	sv1.mass = sv0.mass;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}

void RTCC::GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD)
{
	VECTOR3 RIgn, VIgn;
	double GETbase;
	SV sv, sv2;

	GETbase = getGETBase();

	IgnMJD = GETbase + TimeofIgnition / 24.0 / 3600.0;

	sv = StateVectorCalc(calcParams.src);

	sv2 = coast(sv, (IgnMJD - sv.MJD)*24.0*3600.0);

	//RIgn = tmul(Rot, sv2.R);
	//VIgn = tmul(Rot, sv2.V);
	RIgn = sv2.R;
	VIgn = sv2.V;

	RIgn_global = _V(RIgn.x, RIgn.z, RIgn.y);
	VIgn_global = _V(VIgn.x, VIgn.z, VIgn.y);
	dV_LVLH = DeltaV_LVLH;
}

bool RTCC::REFSMMATDecision(VECTOR3 Att)
{
	if (cos(Att.z) > 0.5) //Yaw between 300° and 60°
	{
		return true;
	}

	return false;
}

SevenParameterUpdate RTCC::TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double GETbase, double P30TIG, double TB5, double mu, double T_RG)
{
	//Inputs:
	//
	//R_TLI: TLI cutoff position vector, right-handed, ecliptic coordinate system
	//V_TLI: TLI cutoff velocity vector, right-handed, ecliptic coordinate system

	MATRIX3 Rot;
	VECTOR3 R_TLI2, V_TLI2;
	double T_RP, tb5start;
	SevenParameterUpdate param;
	OELEMENTS coe;

	tb5start = TB5 - 17.0;
	T_RP = P30TIG - tb5start - T_RG;

	Rot = OrbMech::GetRotationMatrix(oapiGetObjectByName("Earth"), GETbase - 17.0 / 24.0 / 3600.0);

	R_TLI2 = tmul(Rot, _V(R_TLI.x, R_TLI.z, R_TLI.y));
	V_TLI2 = tmul(Rot, _V(V_TLI.x, V_TLI.z, V_TLI.y));

	R_TLI2 = _V(R_TLI2.x, R_TLI2.z, R_TLI2.y);
	V_TLI2 = _V(V_TLI2.x, V_TLI2.z, V_TLI2.y);

	coe = OrbMech::coe_from_PACSS4(R_TLI2, V_TLI2, mu);

	param.alpha_D = coe.w;
	param.C3 = coe.h;
	param.e = coe.e;
	param.f = coe.TA;
	param.Inclination = coe.i;
	param.theta_N = coe.RA;
	param.T_RP = T_RP;

	return param;
}

void RTCC::LVDCTLIPredict(LVDCTLIparam lvdc, VESSEL* vessel, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, VECTOR3 &R_TLI, VECTOR3 &V_TLI, double &T_TLI)
{
	double SVMJD, day, theta_E, MJD_GRR, mu_E, MJD_TST, dt, cos_psiT, sin_psiT, Inclination, X_1, X_2, theta_N, p_N, T_M, R, e, p, alpha_D;
	double MJD_TIG, R_T, V_T, K_5, G_T, gamma_T, boil, tau3, dL_3, F2, F3, V_ex2, V_ex3, dt_tailoff;
	VECTOR3 R_A, V_A, PosS, DotS, T_P, N, PosP, Sbar, DotP, Sbardot, R0, V0, R1, V1, Sbar_1, Cbar_1, R2, V2;
	MATRIX3 mat, MX_EPH, MX_B, MX_G;
	OBJHANDLE gravref;
	OELEMENTS coe;

	//Constants
	gravref = AGCGravityRef(vessel);
	mu_E = GGRAV*oapiGetMass(gravref);
	boil = (1.0 - 0.99998193) / 10.0;
	F2 = 802543.5;
	V_ex2 = 4226.708571;
	V_ex3 = 4188.581306;
	F3 = 906114.066;
	dt_tailoff = 0.25538125;

	//State Vector
	modf(oapiGetSimMJD(), &day);
	MJD_GRR = day + lvdc.T_L / 24.0 / 3600.0;
	mat = OrbMech::Orbiter2PACSS13(MJD_GRR, lvdc.phi_L, -80.6041140*RAD, lvdc.Azimuth);
	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	R0 = _V(R_A.x, R_A.z, R_A.y);
	V0 = _V(V_A.x, V_A.z, V_A.y);

	//Find TB6 start
	//Determination of S-bar and S-bar-dot
	theta_E = lvdc.theta_EO + lvdc.omega_E*lvdc.t_D;
	MX_EPH = mul(OrbMech::tmat(lvdc.MX_A), _M(cos(theta_E), sin(theta_E), 0, 0, 0, -1, -sin(theta_E), cos(theta_E), 0));
	T_P = mul(MX_EPH, unit(lvdc.TargetVector));

	MJD_TST = MJD_GRR + (lvdc.TB5 + lvdc.T_ST) / 24.0 / 3600.0;
	OrbMech::oneclickcoast(R0, V0, SVMJD, (MJD_TST - SVMJD) * 24.0 * 3600.0, R1, V1, gravref, gravref);

	dt = 0;

	do
	{
		OrbMech::oneclickcoast(R1, V1, MJD_TST, dt, R2, V2, gravref, gravref);
		//R2 = tmul(Rot, R2);
		//V2 = tmul(Rot, V2);
		R2 = _V(R2.x, R2.z, R2.y);
		V2 = _V(V2.x, V2.z, V2.y);
		PosS = mul(mat, R2);
		DotS = mul(mat, V2);
		//OrbMech::rv_from_r0v0(R1, V1, dt, PosS, DotS, mu_E);
		N = unit(crossp(PosS, DotS));
		PosP = crossp(N, unit(PosS));
		Sbar = unit(PosS)*cos(lvdc.beta) + PosP*sin(lvdc.beta);
		DotP = crossp(N, DotS / length(PosS));

		Sbardot = DotS / length(PosS)*cos(lvdc.beta) + DotP*sin(lvdc.beta);
		dt += 1.0;

	} while (!((dotp(Sbardot, T_P) < 0 && dotp(Sbar, T_P) <= cos(lvdc.alpha_TS))));

	//Advance to Ignition State
	OrbMech::rv_from_r0v0(PosS, DotS, lvdc.T_RG, PosS, DotS, mu_E);

	cos_psiT = dotp(Sbar,T_P);
	sin_psiT = sqrt(1.0 - pow(cos_psiT, 2));
	Sbar_1 = (Sbar*cos_psiT - T_P)*(1.0 / sin_psiT);
	Cbar_1 = crossp(Sbar_1, Sbar);
	Inclination = acos(dotp(_V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23),Cbar_1));
	X_1 = dotp(_V(lvdc.MX_A.m31, lvdc.MX_A.m32, lvdc.MX_A.m33), crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23)));
	X_2 = dotp(_V(lvdc.MX_A.m11, lvdc.MX_A.m12, lvdc.MX_A.m13), crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23)));
	theta_N = atan2(X_1, X_2);
	p_N = lvdc.mu / lvdc.C_3*(pow(lvdc.e_N, 2) - 1.0);
	T_M = p_N / (1.0 - lvdc.e_N*lvdc.cos_sigma);
	R = length(PosS);
	e = R / lvdc.R_N*(lvdc.e_N - 1.0) + 1.0;
	p = lvdc.mu / lvdc.C_3*(pow(e, 2) - 1.0);

	alpha_D = acos(dotp(Sbar, T_P)) - acos((1.0 - p / T_M) / e) + atan2(dotp(Sbar_1, crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23))), dotp(Sbar, crossp(Cbar_1, _V(lvdc.MX_A.m21, lvdc.MX_A.m22, lvdc.MX_A.m23))));

	MJD_TIG = MJD_TST + (dt + lvdc.T_RG) / 24.0 / 3600.0;
	P30TIG = (MJD_TIG - GETbase) * 24.0 * 3600.0;

	MX_B = _M(cos(theta_N), 0, sin(theta_N), sin(theta_N)*sin(Inclination), cos(Inclination), -cos(theta_N)*sin(Inclination),
		-sin(theta_N)*cos(Inclination), sin(Inclination), cos(theta_N)*cos(Inclination));
	MX_G = mul(MX_B, lvdc.MX_A);
	R_T = p / (1.0 + e*cos(lvdc.f));
	K_5 = sqrt(lvdc.mu / p);
	V_T = K_5*sqrt(1.0 + 2.0 * e*cos(lvdc.f) + pow(e, 2));
	gamma_T = atan((e*sin(lvdc.f)) / (1.0 + cos(lvdc.f)));
	G_T = -lvdc.mu / pow(R_T, 2);

	int UP;
	double mass, a_T, tau2, V, sin_gam, cos_gam, dot_phi_1, dot_phi_T, phi_T, xi_T, dot_zeta_T, dot_xi_T, ddot_zeta_GT, ddot_xi_GT, m0, m1, dt1;
	double dot_dxit, dot_detat, dot_dzetat, dV, f, T_1c, Tt_3, T_2, L_2, L_12, Lt_3, Lt_Y, Tt_T, dT_3, T_3, T_T, dT, Tt_T_old, m2, m3, a_T3, m_cut, a_T_cut;
	VECTOR3 Pos4, PosXEZ, DotXEZ, ddotG_act, DDotXEZ_G;
	MATRIX3 MX_phi_T, MX_K;

	dT = 1.0;
	Tt_T_old = 0.0;
	UP = 0;
	T_2 = lvdc.T_2R;
	T_1c = T_2;
	Tt_3 = lvdc.Tt_3R;
	Tt_T = T_2 + Tt_3;

	mass = vessel->GetMass();
	m0 = vessel->GetEmptyMass();
	dt1 = dt + (MJD_TST - SVMJD) * 24.0 * 3600.0;
	m1 = (mass - m0)*exp(-boil*dt1);
	m2 = m0 + m1;

	a_T = F2 / m2;
	tau2 = V_ex2 / a_T;
	m3 = m2 - F2 / V_ex2*T_2;
	a_T3 = F3 / m3;
	tau3 = V_ex3 / a_T3;

	while (abs(dT) > 0.01)
	{
		//IGM
		ddotG_act = -PosS*mu_E / OrbMech::power(length(PosS), 3.0);
		Pos4 = mul(MX_G, PosS);

		L_2 = V_ex2 * log(tau2 / (tau2 - T_2));
		L_12 = L_2;
		Lt_3 = V_ex3 * log(tau3 / (tau3 - Tt_3));

		Lt_Y = (L_12 + Lt_3);
gtupdate:
		V = length(DotS);
		R = length(PosS);
		sin_gam = ((PosS.x*DotS.x) + (PosS.y*DotS.y) + (PosS.z*DotS.z)) / (R*V);
		cos_gam = pow(1.0 - pow(sin_gam, 2), 0.5);
		dot_phi_1 = (V*cos_gam) / R;
		dot_phi_T = (V_T*cos(gamma_T)) / R_T;
		phi_T = atan2(Pos4.z, Pos4.x) + (((dot_phi_1 + dot_phi_T) / 2.0)*Tt_T);

		f = phi_T + alpha_D;
		R_T = p / (1 + ((e*(cos(f)))));
		V_T = K_5 * pow(1 + ((2 * e)*(cos(f))) + pow(e, 2), 0.5);
		gamma_T = atan2((e*(sin(f))), (1 + (e*(cos(f)))));
		G_T = -mu_E / pow(R_T, 2);

		xi_T = R_T*cos(gamma_T);
		dot_zeta_T = V_T;
		dot_xi_T = 0.0;
		ddot_zeta_GT = G_T*sin(gamma_T);
		ddot_xi_GT = G_T*cos(gamma_T);
		phi_T = phi_T - gamma_T;

		MX_phi_T.m11 = (cos(phi_T));    MX_phi_T.m12 = 0; MX_phi_T.m13 = ((sin(phi_T)));
		MX_phi_T.m21 = 0;               MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
		MX_phi_T.m31 = (-sin(phi_T)); MX_phi_T.m32 = 0; MX_phi_T.m33 = (cos(phi_T));
		MX_K = mul(MX_phi_T, MX_G);
		PosXEZ = mul(MX_K, PosS);
		DotXEZ = mul(MX_K, DotS);
		VECTOR3 RTT_T1, RTT_T2;
		RTT_T1.x = ddot_xi_GT; RTT_T1.y = 0;        RTT_T1.z = ddot_zeta_GT;
		RTT_T2 = ddotG_act;
		RTT_T2 = mul(MX_K, RTT_T2);
		RTT_T1 = RTT_T1 + RTT_T2;
		DDotXEZ_G = _V(0.5*RTT_T1.x, 0.5*RTT_T1.y, 0.5*RTT_T1.z);

		dot_dxit = dot_xi_T - DotXEZ.x - (DDotXEZ_G.x*Tt_T);
		dot_detat = -DotXEZ.y - (DDotXEZ_G.y * Tt_T);
		dot_dzetat = dot_zeta_T - DotXEZ.z - (DDotXEZ_G.z * Tt_T);

		dV = pow((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)), 0.5);
		dL_3 = (((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)) / Lt_Y) - Lt_Y) / 2;

		dT_3 = (dL_3*(tau3 - Tt_3)) / V_ex3;
		T_3 = Tt_3 + dT_3;
		T_T = Tt_T + dT_3;

		// TARGET PARAMETER UPDATE
		if (!(UP > 0)) {
			UP = 1;
			Tt_3 = T_3;
			Tt_T = T_T;
			Lt_3 = Lt_3 + dL_3;
			Lt_Y = Lt_Y + dL_3;
			goto gtupdate; // Recycle. 
		}

		UP = -1;

		Tt_3 = T_3;
		T_1c = T_2;
		Tt_T = T_1c + Tt_3;

		dT = Tt_T - Tt_T_old;
		Tt_T_old = Tt_T;
	}

	coe.e = e;
	coe.h = lvdc.C_3;
	coe.i = Inclination;
	coe.RA = theta_N;
	coe.TA = f;
	coe.w = alpha_D;

	OrbMech::PACSS13_from_coe(coe, lvdc.phi_L, lvdc.Azimuth, mu_E, R_TLI, V_TLI);

	R_TLI = tmul(mat, R_TLI);
	V_TLI = tmul(mat, V_TLI);
	R_TLI = _V(R_TLI.x, R_TLI.z, R_TLI.y);
	V_TLI = _V(V_TLI.x, V_TLI.z, V_TLI.y);

	T_TLI = P30TIG + Tt_T;

	m_cut = m3 - F3 / V_ex3 * T_3;
	a_T_cut = F3 / m_cut;

	dV_LVLH = _V(1.0, 0.0, 0.0)*(dV - a_T_cut * dt_tailoff);
}

void RTCC::FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_out, bool agc)
{
	//For CSM, LM or CSM/LM docked maneuvers
	//INPUT:
	//vesseltype: thrusting vessel, 0 = CSM, 1 = LM, 2 = S-IVB
	//sv: State vector at impulsive TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//DV: Delta velocity of impulsive thrusting maneuver
	//engine: 0 = use RCS (APS for S-IVB), 1 = use SPS/DPS engine (LOX dump for S-IVB), 2 = use LM APS engine
	//
	//OUTPUT:
	//DV_imp: non-impulsive delta velocity
	//t_slip: time in seconds the maneuver has slipped to compensate for finite burntime; should always be negative
	//sv_out: complete state vector at actual cutoff

	double f_T, isp, F_average;

	sv_out.gravref = sv.gravref;

	//CSM
	if (vesseltype == 0)
	{
		//RCS
		if (engine == 0)
		{
			isp = 2706.64;
			f_T = 400.0*4.448222;
			F_average = f_T;
		}
		//SPS
		else
		{
			f_T = SPS_THRUST;
			isp = SPS_ISP;
			F_average = f_T;
		}
	}
	//LM
	else if (vesseltype == 1)
	{
		//RCS
		if (engine == 0)
		{
			isp = 2706.64;
			f_T = 400.0*4.448222;
			F_average = f_T;
		}
		//DPS
		else if (engine == 1)
		{
			double bt, bt_var;
			int step;

			f_T = DPS_THRUST;
			isp = DPS_ISP;

			LMThrottleProgram(f_T, isp, sv.mass + attachedMass, length(DV), F_average, bt, bt_var, step);
		}
		//APS
		else
		{
			f_T = APS_THRUST;
			isp = APS_ISP;
			F_average = f_T;
		}
	}
	//S-IVB
	else
	{
		//APS burn
		if (engine == 0)
		{
			f_T = 2.0*310.0;
			isp = 290.0 * G;
			F_average = f_T;
		}
		//LOX Dump
		else if (engine == 1)
		{
			f_T = 3300.0;
			isp = 157.0;
			F_average = f_T;
		}
	}

	OrbMech::impulsive(sv.R, sv.V, sv.MJD, sv.gravref, f_T, F_average, isp, sv.mass + attachedMass, DV, DV_imp, t_slip, sv_out.R, sv_out.V, sv_out.MJD, sv_out.mass, agc);

	sv_out.mass -= attachedMass;
}

void RTCC::FiniteBurntimeCompensation(int vesseltype, SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, bool agc)
{
	SV sv_out;

	FiniteBurntimeCompensation(vesseltype, sv, attachedMass, DV, engine, DV_imp, t_slip, sv_out, agc);
}

void RTCC::PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int vesseltype, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc)
{
	SV sv_pre, sv_tig, sv_post;
	MATRIX3 Q_Xx;
	VECTOR3 Llambda, DeltaV;
	double t_slip;

	sv_pre = coast(sv0, GET_TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));

	if (DVIsLVLH)
	{
		Q_Xx = OrbMech::LVLH_Matrix(sv_pre.R, sv_pre.V);
		DeltaV = tmul(Q_Xx, DV);
	}
	else
	{
		DeltaV = DV;
	}

	FiniteBurntimeCompensation(vesseltype, sv_pre, attachedMass, DeltaV, enginetype, Llambda, t_slip, sv_post, agc);

	sv_tig = coast(sv_pre, t_slip);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);
	GET_TIG = GET_TIG_imp + t_slip;
	dV_LVLH = mul(Q_Xx, Llambda);
}

VECTOR3 RTCC::ConvertDVtoLVLH(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_imp)
{
	MATRIX3 Q_Xx;
	SV sv_tig;

	sv_tig = coast(sv0, TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));
	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);
	return mul(Q_Xx, DV_imp);
}

VECTOR3 RTCC::ConvertDVtoInertial(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_LVLH_imp)
{
	MATRIX3 Q_Xx;
	SV sv_tig;

	sv_tig = coast(sv0, TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));
	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);
	return tmul(Q_Xx, DV_LVLH_imp);
}

double RTCC::GetDockedVesselMass(VESSEL *vessel)
{
	//INPUT:
	//vessel: primary vessel
	//OUTPUT:
	//mass of the docked vessel, 0 if no vessel docked

	DOCKHANDLE dock;
	OBJHANDLE hLM;
	VESSEL *lm;
	double LMmass;

	if (vessel->DockingStatus(0) == 1)
	{
		dock = vessel->GetDockHandle(0);
		hLM = vessel->GetDockStatus(dock);
		lm = oapiGetVesselInterface(hLM);

		//Special case: S-IVB, but we want the LM mass
		if (!stricmp(lm->GetClassName(), "ProjectApollo\\sat5stg3") ||
			!stricmp(lm->GetClassName(), "ProjectApollo/sat5stg3"))
		{
			SIVB *sivb = (SIVB *)lm;
			LMmass = sivb->GetPayloadMass();
		}
		else
		{
			LMmass = lm->GetMass();
		}
	}
	else
	{
		LMmass = 0.0;
	}

	return LMmass;
}

double RTCC::PericynthionTime(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 R_A, V_A, R0, V0;
	double SVMJD, mu, dt, GETbase;

	GETbase = getGETBase();
	gravref = AGCGravityRef(vessel);

	mu = GGRAV*oapiGetMass(gravref);
	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	R0 = _V(R_A.x, R_A.z, R_A.y);
	V0 = _V(V_A.x, V_A.z, V_A.y);

	dt = OrbMech::timetoperi(R0, V0, mu);
	return (SVMJD-GETbase)*24.0*3600.0 + dt;
}

SV RTCC::FindPericynthion(SV sv0)
{
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	double dt = OrbMech::timetoperi_integ(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hMoon);
	return coast(sv0, dt);
}

double RTCC::FindOrbitalMidnight(SV sv, double GETbase, double t_TPI_guess)
{
	SV sv1;
	double GET_SV, dt, ttoMidnight;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, GETbase);
	dt = t_TPI_guess - GET_SV;

	sv1 = coast(sv, dt);

	ttoMidnight = OrbMech::sunrise(sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, 1, 1, false);
	return t_TPI_guess + ttoMidnight;
}

void RTCC::FindRadarAOSLOS(SV sv, double GETbase, double lat, double lng, double &GET_AOS, double &GET_LOS)
{
	VECTOR3 R_P;
	double LmkRange, dt1, dt2;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt1 = OrbMech::findelev_gs(sv.R, sv.V, R_P, sv.MJD, 175.0*RAD, sv.gravref, LmkRange);
	dt2 = OrbMech::findelev_gs(sv.R, sv.V, R_P, sv.MJD, 5.0*RAD, sv.gravref, LmkRange);

	GET_AOS = OrbMech::GETfromMJD(sv.MJD, GETbase) + dt1;
	GET_LOS = OrbMech::GETfromMJD(sv.MJD, GETbase) + dt2;
}

void RTCC::FindRadarMidPass(SV sv, double GETbase, double lat, double lng, double &GET_Mid)
{
	VECTOR3 R_P;
	double LmkRange, dt;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt = OrbMech::findelev_gs(sv.R, sv.V, R_P, sv.MJD, 90.0*RAD, sv.gravref, LmkRange);

	GET_Mid = OrbMech::GETfromMJD(sv.MJD, GETbase) + dt;
}

double RTCC::FindOrbitalSunrise(SV sv, double GETbase, double t_sunrise_guess)
{
	SV sv1;
	double GET_SV, dt, ttoSunrise;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, GETbase);
	dt = t_sunrise_guess - GET_SV;

	sv1 = coast(sv, dt);

	ttoSunrise = OrbMech::sunrise(sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, true, false, false);
	return t_sunrise_guess + ttoSunrise;
}

VECTOR3 RTCC::PointAOTWithCSM(MATRIX3 REFSMMAT, SV sv, int AOTdetent, int star, double dockingangle)
{
	MATRIX3 Rot, C_LSM, R_ML, C_MSM;
	VECTOR3 s_REF, U_D, U_OAN, U_A, U_DL, U_AL, U_R, GA;
	double EL, AZ, A;

	s_REF = navstars[star - 1];
	U_D = mul(REFSMMAT, s_REF);

	EL = 45.0*RAD;
	AZ = 60.0*RAD*((double)AOTdetent) - 120.0*RAD;

	U_OAN = _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
	Rot = OrbMech::CSMBodyToLMBody(dockingangle);
	U_A = tmul(Rot, U_OAN);

	C_LSM = OrbMech::tmat(OrbMech::CALCSMSC(_V(0, 0, 0)));
	U_DL = tmul(C_LSM, U_D);
	U_AL = U_A;
	U_R = -unit(crossp(U_DL, U_AL));
	A = acos(dotp(U_AL, U_DL));
	R_ML = OrbMech::ROTCOMP(U_R, A);
	C_MSM = mul(C_LSM, R_ML);
	GA = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), OrbMech::tmat(C_MSM));

	return GA;
}

bool RTCC::NC1NC2Program(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh_F, double n_H1, int s, double dh, double n_C, VECTOR3 &dV_NC1_LVLH, double &dh_NC2, double &dv_NC2, double &t_NC2, VECTOR3 &dV_NC2_LVLH, double &dv_NCC, double &t_NCC, double &t_NSR, VECTOR3 &dV_NSR, bool NPC)
{
	int s_F;
	double t_N, dv_C, dv_H1, dv_H2, c, theta, mu, eps1, pi, t, t_H1, t_H2, e_H1, eps2, c_I, t_S, e_H2, e_P, c_F, dvo_C, eo_P;
	double d, MJD, p_H1, p_H2, p_C, e_H1o, e_H2o, dv_H1o, dv_H2o, tt, t_WC;
	VECTOR3 R_WS, V_WS, R_CC, V_CC, R_WC, V_WC, am, ur_WD, R_WF, V_WF, R_WJ, V_WJ, R_CFD, R_CH1, V_CH1, R_CH2, V_CH2, V_CCF, V_CH1F;
	VECTOR3 R_WH2, V_WH2, V_CH2F, R_CS, V_CS, R_CF, V_CF, R_CSF, V_CSF,R_WAF, V_WAF, X, Y, Z, dV_LVLH, dV_NSR_LVLH;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);
	eps1 = 0.000001;
	eps2 = 0.01;
	p_C = 1;
	MJD = GETbase + t_C / 24.0 / 3600.0;

	s_F = 0;
	p_H1 = p_H2 = c_F = c_I = 0.0;
	t_N = t_C;
	R_WS = sv_W.R;
	V_WS = sv_W.V;
	pi = PI;
	dv_C = 225.0*0.3048;
	dv_H1 = 165.0*0.3048;
	dv_H2 = 25.0*0.3048;

	if (s == 2)
	{
		n_C = n_H1;
		dv_C = dv_H1;
	}

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CC, V_CC, gravref, gravref);
	OrbMech::rv_from_r0v0(sv_W.R, sv_W.V, (GETbase - sv_W.MJD)*24.0*3600.0 + t_C, R_WC, V_WC, mu);
	t_WC = t_C;
	do
	{
		am = unit(crossp(R_WC, V_WC));
		ur_WD = unit(crossp(crossp(crossp(R_CC, V_CC), R_CC), am));
		d = OrbMech::sign(n_C - 18.0);
		c = OrbMech::sign(dotp(crossp(ur_WD, R_WC), am));
		theta = pi - c*(pi - acos(dotp(ur_WD, unit(R_WC)))) + (d + 1.0)*(c + 1.0)*pi*0.5;

		if (abs(theta) >= eps1)
		{
			if (c_I == 10)
			{
				return false;
			}
			t = OrbMech::time_theta(R_WC, V_WC, theta, mu);
			if (pi != 0.0)
			{
				pi = 0.0;
				R_WC = R_WS;
				V_WC = V_WS;
				t_WC = (sv_W.MJD - GETbase)*24.0*3600.0;
			}
			c_I++;
			t_N -= t;
			OrbMech::oneclickcoast(R_WC, V_WC, MJD, t_N - t_WC, R_WC, V_WC, gravref, gravref);
			t_WC = t_N;
		}
	} while (abs(theta) >= eps1);

	R_CC = unit(R_CC - am*dotp(R_CC, am))*length(R_CC);
	V_CC = unit(V_CC - am*dotp(V_CC, am))*length(V_CC);

	OrbMech::rv_from_r0v0(R_WC, V_WC, t_F - t_N, R_WF, V_WF, mu);

	if (!OrbMech::QDRTPI(R_WF, V_WF, MJD, gravref, mu, dh_F, E_L, 0, R_WJ, V_WJ))
	{
		return false;
	}

	R_CFD = R_WJ - unit(R_WJ)*dh_F;

	do
	{
		V_CCF = V_CC + unit(crossp(am, R_CC))*dv_C;
		OrbMech::REVUP(R_CC, V_CCF, n_C, mu, R_CH1, V_CH1, tt);
		t_H1 = t_C + tt;

		if (s == 1)
		{
			if (c_F == 0)
			{
				c_I = 0;
			}
			else
			{
				c_I = 0.5;
			}
			do
			{
				V_CH1F = V_CH1 + unit(crossp(am, R_CH1))*dv_H1;
				OrbMech::REVUP(R_CH1, V_CH1F, n_H1, mu, R_CH2, V_CH2, tt);
				t_H2 = t_H1 + tt;
				OrbMech::RADUP(R_WC, V_WC, R_CH2, mu, R_WH2, V_WH2);
				e_H1 = length(R_WH2) - length(R_CH2) - dh;
				if (p_H1 == 0 || abs(e_H1) >= eps2)
				{
					OrbMech::ITER(c_I, s_F, e_H1, p_H1, dv_H1, e_H1o, dv_H1o);
					if (s_F == 1)
					{
						return false;
					}
				}
			} while (abs(e_H1) >= eps2);
		}
		else
		{
			t_H2 = t_H1;
			R_CH2 = R_CH1;
			V_CH2 = V_CH1;
		}
		if (c_F == 0)
		{
			c_I = 0;
		}
		else
		{
			c_I = 0.5;
		}
		t_S = t_H2 + dt;
		do
		{
			V_CH2F = V_CH2 + unit(crossp(am, R_CH2))*dv_H2;
			OrbMech::rv_from_r0v0(R_CH2, V_CH2F, dt, R_CS, V_CS, mu);
			OrbMech::RADUP(R_WC, V_WC, R_CS, mu, R_WS, V_WS);
			OrbMech::COE(R_WS, V_WS, length(R_WS) - length(R_CS), mu, R_CSF, V_CSF);
			OrbMech::rv_from_r0v0(R_CS, V_CSF, t_F - t_S, R_CF, V_CF, mu);
			OrbMech::RADUP(R_WC, V_WC, R_CF, mu, R_WAF, V_WAF);
			e_H2 = length(R_WAF) - length(R_CF) - dh_F;

			if (p_H2 == 0 || abs(e_H2) >= eps2)
			{
				OrbMech::ITER(c_I, s_F, e_H2, p_H2, dv_H2, e_H2o, dv_H2o);
				if (s_F == 1)
				{
					return false;
				}
			}
		} while (abs(e_H2) >= eps2);
		e_P = OrbMech::sign(dotp(crossp(R_CF, R_CFD), am))*acos(dotp(R_CFD / length(R_CFD), R_CF / length(R_CF)));
		if (abs(e_P) >= 4.0*eps1)
		{
			OrbMech::ITER(c_F, s_F, e_P, p_C, dv_C, eo_P, dvo_C);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(e_P) >= 4.0*eps1);

	if (NPC)
	{
		double dt_PC, dv_PC, Y_C;
		VECTOR3 u2, R_WCC, V_WCC, R_CCC, V_CCC, dV_NC, R_WPC, V_WPC, R_CPC, V_CPC, XX, YY, ZZ, V_CCC_apo;
		double c_PC, p_PC, eo_PC, dvo_PC;
		int s_PC;

		c_PC = 0.0;
		p_PC = 0.0;

		OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CCC, V_CCC, gravref, gravref);
		OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_C, R_WCC, V_WCC, gravref, gravref);

		u2 = unit(crossp(V_WCC, R_WCC));
		dv_PC = -dotp(V_CCC, u2);

		ZZ = -unit(R_CCC);
		YY = unit(crossp(V_CCC, R_CCC));
		XX = crossp(YY, ZZ);

		do
		{
			dV_LVLH = _V(dv_C, dv_PC, 0);
			dV_NC = tmul(_M(XX.x, XX.y, XX.z, YY.x, YY.y, YY.z, ZZ.x, ZZ.y, ZZ.z), dV_LVLH);
			V_CCC_apo = V_CCC + dV_NC;

			dt_PC = OrbMech::time_theta(R_CCC, V_CCC_apo, PI05, mu);

			OrbMech::oneclickcoast(R_CCC, V_CCC_apo, MJD, dt_PC, R_CPC, V_CPC, gravref, gravref);
			OrbMech::oneclickcoast(R_WCC, V_WCC, MJD, dt_PC, R_WPC, V_WPC, gravref, gravref);
			u2 = unit(crossp(V_WPC, R_WPC));

			Y_C = dotp(R_CPC, u2);
			if (p_PC == 0 || abs(Y_C) >= eps2)
			{
				OrbMech::ITER(c_PC, s_PC, Y_C, p_PC, dv_PC, eo_PC, dvo_PC);
				if (s_PC == 1)
				{
					return false;
				}
			}
		} while (abs(Y_C) >= eps2);
	}
	else
	{
		dV_LVLH = _V(dv_C, 0, 0);
	}
	if (s == 1)
	{
		OrbMech::RADUP(R_WC, V_WC, R_CH1, mu, R_WF, V_WF);
		dh_NC2 = length(R_WF) - length(R_CH1);
		dv_NC2 = dv_H1;
		dV_NC1_LVLH = dV_LVLH;
		t_NC2 = t_H1;
	}
	else
	{
		dV_NC2_LVLH = dV_LVLH;
	}
	dv_NCC = dv_H2;
	dV_NSR = V_CSF - V_CS;
	t_NCC = t_H2;
	t_NSR = t_S;
	Z = -unit(R_CS);
	Y = -am;
	X = crossp(Y, Z);
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NSR);

	return true;
}

void RTCC::NCCProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh, VECTOR3 &dV_NCC_LVLH, double &t_NSR, VECTOR3 &dV_NSR_LVLH)
{
	VECTOR3 R_CC, V_CC, R_WF, V_WF, R_WJ, V_WJ, R_CF, V_CF, R_CT, V_CT, V_CCF, R_CTF, V_CTF, X, Y, Z, dV_NCC, dV_NSR;
	double MJD, mu;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);
	MJD = GETbase + t_C / 24.0 / 3600.0;

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CC, V_CC, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_F, R_WF, V_WF, gravref, gravref);
	OrbMech::QDRTPI(R_WF, V_WF, MJD, gravref, mu, dh, E_L, 1, R_WJ, V_WJ);
	OrbMech::COE(R_WJ, V_WJ, dh, mu, R_CF, V_CF);
	OrbMech::oneclickcoast(R_CF, V_CF, MJD, t_C + dt - t_F, R_CT, V_CT, gravref, gravref);
	V_CCF = OrbMech::Vinti(R_CC, V_CC, R_CT, MJD, dt, 0, true, gravref, gravref, gravref, _V(0, 0, 0));
	OrbMech::oneclickcoast(R_CC, V_CCF, sv_C.MJD, dt, R_CTF, V_CTF, gravref, gravref);

	dV_NCC = V_CCF - V_CC;
	dV_NSR = V_CT - V_CTF;

	Z = -unit(R_CTF);
	Y = unit(crossp(V_CTF, R_CTF));
	X = crossp(Y, Z);
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NSR);

	Z = -unit(R_CC);
	Y = unit(crossp(V_CC, R_CC));
	X = crossp(Y, Z);
	dV_NCC_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NCC);
}

void RTCC::NSRProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t2, double t3, VECTOR3 &dV_NSR_LVLH)
{
	VECTOR3 R_C2, V_C2, R_W2, V_W2, u1, u2, u, R_WC, V_WC, dV_2, X, Y, Z, V_C2_apo;
	double Y_C, Y_C_dot, Y_W_dot, cos_phi, sin_phi, SW, phi, dh_NSR, v_WV, a_W, a_C, v_CV, v_CH, mu, dt;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t2, R_C2, V_C2, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t2, R_W2, V_W2, gravref, gravref);

	u1 = unit(crossp(V_C2, R_C2));
	u2 = unit(crossp(V_W2, R_W2));
	Y_C = dotp(R_C2, u2);
	Y_C_dot = dotp(V_C2, u2);
	Y_W_dot = dotp(V_W2, u1);
	
	u = unit(crossp(R_W2, V_W2));
	R_C2 = unit(R_C2 - u*dotp(R_C2, u))*length(R_C2);
	V_C2 = unit(V_C2 - u*dotp(V_C2, u))*length(V_C2);

	cos_phi = dotp(unit(R_C2), unit(R_W2));
	sin_phi = sqrt(1.0 - cos_phi*cos_phi);
	SW = dotp(u, crossp(R_W2, R_C2));

	phi = atan2(OrbMech::sign(SW)*sin_phi, cos_phi);
	dt = OrbMech::time_theta(R_W2, V_W2, phi, mu);
	OrbMech::rv_from_r0v0(R_W2, V_W2, dt, R_WC, V_WC, mu);

	dh_NSR = length(R_WC) - length(R_C2);
	v_WV = dotp(V_WC, unit(R_C2));
	a_W =  1.0 / (2.0 / length(R_W2) - dotp(V_W2, V_W2) / mu);
	a_C = a_W - dh_NSR;
	v_CV = v_WV*OrbMech::power(a_W / a_C, 1.5);
	v_CH = sqrt(mu*(2.0 / length(R_C2) - 1.0 / a_C) - v_CV*v_CV);
	V_C2_apo = unit(crossp(u, R_C2))*v_CH + unit(R_C2)*v_CV;
	dV_2 = V_C2_apo - V_C2;
	Z = unit(-R_C2);
	Y = -u;
	X = unit(crossp(Y, Z));
	
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_2) + _V(0.0, -Y_C_dot, 0.0);
}

void RTCC::NPCProgram(SV sv_C, SV sv_W, double GETbase, double t, double &t_NPC, VECTOR3 &dV_NPC_LVLH)
{
	VECTOR3 R_C2, V_C2, R_C3, V_C3, R_W3, V_W3, u1, u2;
	double mu, dt, Y_C, Y_C_dot, Y_W_dot, T_P;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t, R_C2, V_C2, gravref, gravref);
	dt = OrbMech::time_theta(R_C2, V_C2, PI05*3.0, mu);
	T_P = OrbMech::period(R_C2, V_C2, mu);

	if (dt < 0)
	{
		dt = T_P + dt;
	}

	t_NPC = t + dt;

	OrbMech::oneclickcoast(R_C2, V_C2, sv_C.MJD, dt, R_C3, V_C3, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_NPC, R_W3, V_W3, gravref, gravref);

	u1 = unit(crossp(V_C3, R_C3));
	u2 = unit(crossp(V_W3, R_W3));
	Y_C = dotp(R_C3, u2);
	Y_C_dot = dotp(V_C3, u2);
	Y_W_dot = dotp(V_W3, u1);

	dV_NPC_LVLH = _V(0, -Y_C_dot, abs(Y_C_dot) / tan(70.0*RAD));
}

bool RTCC::SkylabRendezvous(SkyRendOpt *opt, SkylabRendezvousResults *res)
{
	SV sv_C, sv_W, sv_tig;
	VECTOR3 dV_NC1, dV_LVLH, dV_NC2, dV_NCC, dV_NSR, dV_NPC;
	double LMmass, P30TIG, t_NPC;
	OBJHANDLE gravref;
	bool SolutionGood = true;	//True = Calculation successful, False = Calculation failed

	if (opt->useSV)
	{
		sv_C = opt->RV_MCC;
	}
	else
	{
		sv_C = StateVectorCalc(opt->vessel);
	}

	sv_W = StateVectorCalc(opt->target);
	gravref = sv_C.gravref;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	if (opt->man == 0)
	{
		SV sv_W1;
		double ttoMidnight;
		OBJHANDLE hSun;

		hSun = oapiGetObjectByName("Sun");

		sv_W1 = coast(sv_W, (opt->GETbase - sv_W.MJD)*24.0*3600.0 + opt->TPIGuess);

		ttoMidnight = OrbMech::sunrise(sv_W1.R, sv_W1.V, sv_W1.MJD, gravref, hSun, 1, 1, false);
		res->t_TPI = opt->TPIGuess + ttoMidnight;

		return true;
	}
	else if (opt->man == 1)	//NC1
	{
		SolutionGood = NC1NC2Program(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, 0.5, 1, opt->DH1, opt->n_C, dV_NC1, res->dH_NC2, res->dv_NC2, res->t_NC2, dV_NC2, res->dv_NCC, res->t_NCC, res->t_NSR, res->dV_NSR, opt->NPCOption);
		
		if (SolutionGood)
		{
			dV_LVLH = dV_NC1;
			P30TIG = opt->t_C;
		}
	}
	else if (opt->man == 2)	//NC2
	{
		SolutionGood = NC1NC2Program(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, 0.5, 2, opt->DH1, opt->n_C, dV_NC1, res->dH_NC2, res->dv_NC2, res->t_NC2, dV_NC2, res->dv_NCC, res->t_NCC, res->t_NSR, res->dV_NSR, opt->NPCOption);
		if (SolutionGood)
		{
			dV_LVLH = dV_NC2;
			P30TIG = opt->t_C;
		}
	}
	else if (opt->man == 3)	//NCC
	{
		NCCProgram(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, dV_NCC, res->t_NSR, res->dV_NSR);
		
		dV_LVLH = dV_NCC;
		P30TIG = opt->t_C;
	}
	else if (opt->man == 4)	//NSR
	{
		NSRProgram(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, opt->t_TPI, dV_NSR);

		dV_LVLH = dV_NSR;
		P30TIG = opt->t_C;
	}
	else if (opt->man == 5)	//TPI
	{
		LambertMan lambert;
		TwoImpulseResuls lamres;
		double dt;

		dt = OrbMech::time_theta(sv_W.R, sv_W.V, 140.0*RAD, GGRAV*oapiGetMass(gravref));
		lambert = set_lambertoptions(sv_C, sv_W, opt->GETbase, opt->t_C, opt->t_C + dt, 0, RTCC_LAMBERT_MULTIAXIS, RTCC_LAMBERT_PERTURBED, _V(0, 0, 0), 0);
		LambertTargeting(&lambert, lamres);

		res->dV_LVLH = dV_LVLH = lamres.dV_LVLH;
		res->P30TIG = opt->t_C;

		return true;
	}
	else if (opt->man == 6)	//TPM
	{
		LambertMan lambert;
		TwoImpulseResuls lamres;
		double dt;

		dt = OrbMech::time_theta(sv_W.R, sv_W.V, 140.0*RAD, GGRAV*oapiGetMass(gravref));
		lambert = set_lambertoptions(sv_C, sv_W, opt->GETbase, opt->t_C, opt->t_TPI + dt, 0, RTCC_LAMBERT_MULTIAXIS, RTCC_LAMBERT_PERTURBED, _V(0, 0, 0), 0);
		LambertTargeting(&lambert, lamres);

		res->dV_LVLH = dV_LVLH = lamres.dV_LVLH;
		res->P30TIG = opt->t_C;

		return true;
	}
	else if (opt->man == 7)	//NPC
	{
		NPCProgram(sv_C, sv_W, opt->GETbase, opt->t_NC, t_NPC, dV_NPC);

		dV_LVLH = dV_NPC;
		P30TIG = t_NPC;
	}

	if (SolutionGood == false)
	{
		return false;
	}

	VECTOR3 R_cor, V_cor, Llambda, DVX;
	MATRIX3 Q_Xx;
	double t_slip;

	sv_tig = coast(sv_C, (opt->GETbase - sv_C.MJD)*24.0*3600.0 + P30TIG);

	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);

	DVX = tmul(Q_Xx, dV_LVLH);

	FiniteBurntimeCompensation(RTCC_VESSELTYPE_CSM, sv_tig, LMmass, DVX, 1, Llambda, t_slip);

	OrbMech::oneclickcoast(sv_tig.R, sv_tig.V, sv_tig.MJD, t_slip, R_cor, V_cor, gravref, gravref);

	Q_Xx = OrbMech::LVLH_Matrix(R_cor, V_cor);

	res->P30TIG = P30TIG + t_slip;
	res->dV_LVLH = mul(Q_Xx, Llambda);

	return true;
}

void RTCC::TLIFirstGuessConic(SV sv_mcc, double lat_EMP, double h_peri, double MJD_P, VECTOR3 &DV, VECTOR3 &var_converged)
{
	SV sv_p;
	MATRIX3 M_EMP;
	VECTOR3 R_EMP, V_EMP, var_fg, V_MCC_apo;
	double r_peri, mu, ddt, R_E, lng_peri, azi_peri, v_peri;
	OBJHANDLE hMoon, hEarth;
	OELEMENTS coe;
	double rtest, lngtest, lattest, fpatest;

	R_E = 6378.137e3;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu = GGRAV*oapiGetMass(hMoon);

	r_peri = oapiGetSize(hMoon) + h_peri;
	sv_p.MJD = MJD_P;
	sv_p.gravref = hMoon;

	//Initial guess
	var_fg = TLMCEmpiricalFirstGuess(r_peri, 0.0);
	lng_peri = var_fg.z;
	azi_peri = var_fg.y;
	v_peri = var_fg.x;

	//Conic first guess seems to break down above 1500NM pericynthion altitude
	if (r_peri > 4.5e6)
	{
		var_converged = var_fg;
		return;
	}

	do
	{
		//Position vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri, lng_peri, lat_EMP, PI05, azi_peri, R_EMP, V_EMP);

		//EMP Matrix
		M_EMP = OrbMech::EMPMatrix(sv_p.MJD);

		//Convert EMP position to ecliptic
		sv_p.R = tmul(M_EMP, R_EMP);
		sv_p.V = tmul(M_EMP, V_EMP);

		//Calculate pericynthion velocity
		OrbMech::ThirdBodyConic(sv_p.R, hMoon, sv_mcc.R, hEarth, sv_p.MJD, (sv_mcc.MJD - sv_p.MJD)*24.0*3600.0, sv_p.V, sv_p.V, V_MCC_apo, 100.0);

		//save azi and vmag as new initial guess
		V_EMP = mul(M_EMP, sv_p.V);
		OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lngtest, lattest, fpatest, azi_peri);

		coe = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
		ddt = OrbMech::timetoperi(sv_p.R, sv_p.V, mu);

		if (coe.TA > PI)
		{
			lng_peri += coe.TA - PI2;
		}
		else
		{
			lng_peri += coe.TA;
		}
	} while (abs(ddt) > 0.01);

	DV = V_MCC_apo - sv_mcc.V;

	var_converged = _V(v_peri, azi_peri, lng_peri);
}

VECTOR3 RTCC::TLMCEmpiricalFirstGuess(double r, double dt)
{
	double lambda, psi, V, R_E;

	R_E = 6378.137e3;

	lambda = PI - 0.025*dt*3600.0;
	psi = 270.0*RAD;
	V = sqrt(0.184 + 0.553 / (r / R_E))*R_E / 3600.0 - 0.0022*dt*3600.0;

	return _V(V, psi, lambda);
}

bool RTCC::TLIFlyby(SV sv_TLI, double lat_EMP, double h_peri, SV sv_peri_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_TLI_apo, sv_p, sv_r;
	MATRIX3 M_EMP;
	VECTOR3 R_EMP, V_EMP;
	double dt, mu, ddt, VacPeri, R_E, e_H, e_Ho, c_I, p_H, eps2, dto, r_peri, lngtest, lattest, fpatest, rtest, v_peri, lng_EMP, azi_peri;
	int s_F;
	OBJHANDLE hMoon, hEarth;
	OELEMENTS coe;

	c_I = p_H = dto = 0.0;
	eps2 = 0.1;
	s_F = 0;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV*oapiGetMass(hMoon);
	R_E = oapiGetSize(hEarth);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	r_peri = oapiGetSize(hMoon) + h_peri;
	dt = (sv_peri_guess.MJD - sv_TLI.MJD)*24.0*3600.0;

	M_EMP = OrbMech::EMPMatrix(sv_peri_guess.MJD);
	R_EMP = mul(M_EMP, sv_peri_guess.R);
	V_EMP = mul(M_EMP, sv_peri_guess.V);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lng_EMP, lattest, fpatest, azi_peri);

	do
	{
		sv_p.MJD = sv_TLI.MJD + dt / 24.0 / 3600.0;

		do
		{
			//Position vector in EMP Coordinates
			OrbMech::adbar_from_rv(r_peri, v_peri, lng_EMP, lat_EMP, PI05, azi_peri, R_EMP, V_EMP);

			//EMP Matrix
			M_EMP = OrbMech::EMPMatrix(sv_p.MJD);

			//Convert EMP position to ecliptic
			sv_p.R = tmul(M_EMP, R_EMP);
			sv_p.V = tmul(M_EMP, V_EMP);

			//Calculate pericynthion velocity
			sv_p.V = OrbMech::Vinti(sv_p.R, _V(0.0, 0.0, 0.0), sv_TLI.R, sv_p.MJD, -dt, 0, false, hMoon, hMoon, sv_TLI.gravref, sv_p.V);

			//save azi and vmag as new initial guess
			V_EMP = mul(M_EMP, sv_p.V);
			OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lngtest, lattest, fpatest, azi_peri);

			coe = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
			ddt = OrbMech::timetoperi(sv_p.R, sv_p.V, mu);

			if (coe.TA > PI)
			{
				lng_EMP += coe.TA - PI2;
			}
			else
			{
				lng_EMP += coe.TA;
			}
		} while (abs(ddt) > 0.01);

		OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);
		VacPeri = length(sv_r.R);

		//20NM vacuum perigee
		e_H = VacPeri - R_E - 20.0*1852.0;

		if (p_H == 0 || abs(dt - dto) >= eps2)
		{
			OrbMech::ITER(c_I, s_F, e_H, p_H, dt, e_Ho, dto);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(dt - dto) >= eps2);

	sv_TLI_apo = coast(sv_p, -dt);

	DV = sv_TLI_apo.V - sv_TLI.V;
	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCFlyby(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_p, sv_r;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 step, target;
	double dt1;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	target = _V(h_peri, lat_EMP, 20.0*1852.0);
	step = _V(10e-3, 10e-3, 10e-3);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	DV = OrbMech::TLMCIntegratedFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;
	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCConic_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor)
{
	SV sv_p, sv_n, sv_r, sv_postLOI;
	LOIMan loiopt;
	VECTOR3 target, DV_LOI, DV_MCC, step;
	double TIG_LOI, dV_T, lat_iter, mu_E, mu_M;
	double latarr[3], dvarr[3];
	int s_F, c_I;
	OBJHANDLE hEarth, hMoon;
	double dt1, dt2;
	VECTOR3 R_patch, V_patch;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;

	sv_p.gravref = hMoon;
	sv_p.mass = sv_mcc.mass;

	DV_MCC = DV_guess;
	lat_iter = lat_EMP;

	step = _V(10e-3, 10e-3, 10e-3);
	dvarr[0] = dvarr[1] = dvarr[2] = 0.0;
	c_I = s_F = 0;

	do
	{
		target = _V(h_peri, lat_iter, 20.0*1852.0);
		DV_MCC = OrbMech::TLMCConicFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_MCC, target, step);

		if (sv_mcc.gravref == hEarth)
		{
			dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_MCC, sv_mcc.MJD, mu_E, mu_M, R_patch, V_patch);
		}
		else
		{
			R_patch = sv_mcc.R;
			V_patch = sv_mcc.V + DV_MCC;
			dt1 = 0.0;
		}

		dt2 = OrbMech::timetoperi(R_patch, V_patch, mu_M);
		OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, mu_M);
		sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

		loiopt.RV_MCC = sv_p;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n, sv_postLOI);

		dV_T = length(DV_LOI) + length(DV_MCC);
		OrbMech::QuadraticIterator(c_I, s_F, lat_iter, latarr, dvarr, dV_T, 0.02*RAD, 0.2*RAD);
		if (s_F == 1)
		{
			return false;
		}

	} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMC_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor)
{
	SV sv_p, sv_r, sv_n, sv_postLOI;
	LOIMan loiopt;
	VECTOR3 target, DV_LOI, DV_MCC, step;
	double TIG_LOI, dV_T, lat_iter, mu_E, mu_M;
	double latarr[3], dvarr[3];
	int s_F, c_I;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;

	loiopt.RV_MCC.gravref = sv_mcc.gravref;
	loiopt.RV_MCC.R = sv_mcc.R;
	loiopt.RV_MCC.mass = sv_mcc.mass;
	loiopt.RV_MCC.MJD = sv_mcc.MJD;

	DV_MCC = DV_guess;
	lat_iter = lat_EMP;

	step = _V(10e-3, 10e-3, 10e-3);
	dvarr[0] = dvarr[1] = dvarr[2] = 0.0;
	c_I = s_F = 0;

	do
	{
		target = _V(h_peri, lat_iter, 20.0*1852.0);
		DV_MCC = OrbMech::TLMCIntegratedFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_MCC, target, step);

		loiopt.RV_MCC.V = sv_mcc.V + DV_MCC;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n, sv_postLOI);

		dV_T = length(DV_LOI) + length(DV_MCC);
		OrbMech::QuadraticIterator(c_I, s_F, lat_iter, latarr, dvarr, dV_T, 0.02*RAD, 0.2*RAD);
		if (s_F == 1)
		{
			return false;
		}

	} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);

	double dt1;

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV_MCC, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;
	sv_p.gravref = hMoon;

	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);
	sv_p.gravref = hEarth;

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_reentry = sv_r;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMCConic_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor)
{
	SV sv_p, sv_mcc_apo, sv_n, sv_postLOI;
	VECTOR3 R_EMP, V_EMP, DV_LOI, var_converged, RP_M, VP_M, var_guess, DV_MCC, step, DV_DOI;
	double lng_peri_guess, dt, dt1, mu_E, mu_M, TIG_LOI, dV_T, r_peri, v_peri_guess, azi_peri_guess, v_peri_conv, azi_peri_conv, lng_peri_conv, TIG_DOI;
	double rtemp, lattemp, fpatemp, lat_iter;
	double dLat, dV_To;
	int c_I;
	OBJHANDLE hMoon, hEarth;
	LOIMan loiopt;
	DOIMan doiopt;

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;
	loiopt.EllipseRotation = opt->LOIEllipseRotation;

	doiopt.alt = opt->alt;
	doiopt.GETbase = opt->GETbase;
	doiopt.lat = opt->LSlat;
	doiopt.lng = opt->LSlng;
	doiopt.N = opt->N;
	doiopt.opt = opt->DOIType;
	doiopt.PeriAng = opt->DOIPeriAng;
	doiopt.PeriAlt = opt->DOIPeriAlt;

	c_I = 0;
	dLat = 0.0;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu_M = GGRAV*oapiGetMass(hMoon);
	mu_E = GGRAV*oapiGetMass(hEarth);

	sv_p.mass = sv_mcc.mass;
	sv_p.gravref = hMoon;

	sv_mcc_apo = sv_mcc;
	sv_mcc_apo.V = sv_mcc.V + DV_guess;

	r_peri = oapiGetSize(hMoon) + h_peri;
	lat_iter = lat_EMP;
	step = _V(10e-3, 10e-3, 10e-3);

	dt = (MJD_peri - sv_mcc.MJD)*24.0*3600.0;

	dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_guess, sv_mcc.MJD, mu_E, mu_M, RP_M, VP_M);
	OrbMech::rv_from_r0v0(RP_M, VP_M, dt - dt1, sv_p.R, sv_p.V, mu_M);
	sv_p.MJD = sv_mcc.MJD + dt / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtemp, v_peri_guess, lng_peri_guess, lattemp, fpatemp, azi_peri_guess);

	var_guess = _V(v_peri_guess, azi_peri_guess, lng_peri_guess);

	do
	{
		var_converged = OrbMech::TLMCConicFirstGuessIterator(r_peri, lat_iter, 0.0, var_guess, sv_mcc.R, step, MJD_peri, -dt, hMoon, sv_mcc.gravref);

		v_peri_conv = var_converged.x;
		azi_peri_conv = var_converged.y;
		lng_peri_conv = var_converged.z;

		var_guess = var_converged;

		//Position and velocity vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat_iter, PI05, azi_peri_conv, R_EMP, V_EMP);

		//Convert EMP position to ecliptic
		OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);

		OrbMech::rv_from_r0v0_tb(sv_p.R, sv_p.V, sv_p.MJD, hMoon, sv_mcc.gravref, -dt, sv_mcc_apo.R, sv_mcc_apo.V);
		DV_MCC = sv_mcc_apo.V - sv_mcc.V;

		loiopt.RV_MCC = sv_p;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n, sv_postLOI);

		if (doiopt.opt == 1)
		{
			doiopt.EarliestGET = TIG_LOI + 3.5*3600.0;
			doiopt.sv0 = sv_postLOI;
			DOITargeting(&doiopt, DV_DOI, TIG_DOI);
		}
		else
		{
			DV_DOI = _V(0, 0, 0);
		}

		dV_T = length(DV_DOI) + length(DV_LOI) + length(DV_MCC);

		if (c_I == 0)
		{
			dLat = -1.0*RAD;
		}
		else
		{
			if (dV_T > dV_To)
			{
				dLat = -dLat / 2.0;
			}
		}

		lat_iter += dLat;
		dV_To = dV_T;
		c_I++;

		//dvar = OrbMech::QuadraticIterator(c_I, s_F, lat_EMPcor, latarr, dvarr, dV_T, 0.1*RAD, 1.0*RAD);
		//if (s_F == 1)
		//{
		//	return false;
		//}
		//} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);
	} while (abs(dLat) > 0.01*RAD);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMC_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor)
{
	SV sv_p, sv_mcc_apo, sv_n, sv_postLOI;
	VECTOR3 R_EMP, V_EMP, DV_LOI, var_converged, var_guess, DV_MCC, step;
	double lng_peri_guess, dt, mu_E, mu_M, TIG_LOI, dV_T, r_peri, v_peri_guess, azi_peri_guess, v_peri_conv, azi_peri_conv, lng_peri_conv;
	double rtemp, lattemp, fpatemp, lat_iter;
	double dLat, dV_To;
	int c_I;
	OBJHANDLE hMoon, hEarth;
	LOIMan loiopt;

	loiopt.alt = opt->alt;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = opt->csmlmdocked;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.useSV = true;
	loiopt.vessel = opt->vessel;
	loiopt.impulsive = RTCC_IMPULSIVE;
	loiopt.type = opt->type;

	c_I = 0;
	dLat = 0.0;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	mu_M = GGRAV*oapiGetMass(hMoon);
	mu_E = GGRAV*oapiGetMass(hEarth);

	loiopt.RV_MCC.mass = sv_mcc.mass;
	loiopt.RV_MCC.gravref = hMoon;

	sv_mcc_apo = sv_mcc;
	sv_mcc_apo.V = sv_mcc.V + DV_guess;

	r_peri = oapiGetSize(hMoon) + h_peri;
	lat_iter = lat_EMP;
	step = _V(10e-3, 10e-3, 10e-3);

	dt = (MJD_peri - sv_mcc.MJD)*24.0*3600.0;

	sv_p = coast(sv_mcc_apo, dt);

	//dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_guess, sv_mcc.MJD, mu_E, mu_M, RP_M, VP_M);
	//OrbMech::rv_from_r0v0(RP_M, VP_M, dt - dt1, sv_p.R, sv_p.V, mu_M);
	//sv_p.MJD = sv_mcc.MJD + dt / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtemp, v_peri_guess, lng_peri_guess, lattemp, fpatemp, azi_peri_guess);

	var_guess = _V(v_peri_guess, azi_peri_guess, lng_peri_guess);

	do
	{
		var_converged = OrbMech::IntegratedTLMCIterator(r_peri, lat_iter, PI05, var_guess, sv_mcc.R, step, MJD_peri, -dt, hMoon, sv_mcc.gravref);

		v_peri_conv = var_converged.x;
		azi_peri_conv = var_converged.y;
		lng_peri_conv = var_converged.z;

		var_guess = var_converged;

		//Position and velocity vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat_iter, PI05, azi_peri_conv, R_EMP, V_EMP);

		//Convert EMP position to ecliptic
		OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);

		sv_mcc_apo = coast(sv_p, -dt);
		DV_MCC = sv_mcc_apo.V - sv_mcc.V;

		loiopt.RV_MCC = sv_p;

		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n, sv_postLOI);

		dV_T = length(DV_LOI) + length(DV_MCC);

		if (c_I == 0)
		{
			dLat = -1.0*RAD;
		}
		else
		{
			if (dV_T > dV_To)
			{
				dLat = -dLat / 2.0;
			}
		}

		lat_EMPcor += dLat;
		dV_To = dV_T;
		c_I++;

		//dvar = OrbMech::QuadraticIterator(c_I, s_F, lat_EMPcor, latarr, dvarr, dV_T, 0.1*RAD, 1.0*RAD);
		//if (s_F == 1)
		//{
		//	return false;
		//}
		//} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);
	} while (abs(dLat) > 0.01*RAD);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

void RTCC::TLMCFirstGuess(SV sv_mcc, double lat_EMP, double h_peri, double MJD_P, VECTOR3 &DV, SV &sv_peri)
{
	VECTOR3 var_guess, var_converged;
	double r_peri;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");
	r_peri = oapiGetSize(hMoon) + h_peri;
	var_guess = TLMCEmpiricalFirstGuess(r_peri, 0.0);

	IntegratedTLMC(sv_mcc, lat_EMP, h_peri, 0.0, MJD_P, var_guess, DV, var_converged, sv_peri);
}

void RTCC::IntegratedTLMC(SV sv_mcc, double lat, double h, double gamma, double MJD, VECTOR3 var_guess, VECTOR3 &DV, VECTOR3 &var_converged, SV &sv_node)
{
	SV sv_n, sv_mcc_apo;
	OBJHANDLE hMoon;
	VECTOR3 R_EMP, V_EMP, step;
	double r_peri, dt, v_peri_conv, azi_peri_conv, lng_peri_conv;

	hMoon = oapiGetObjectByName("Moon");
	r_peri = oapiGetSize(hMoon) + h;
	sv_n.MJD = MJD;
	sv_n.gravref = hMoon;
	dt = (sv_n.MJD - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);//_V(pow(2.0, -19.0)*1769.7, pow(2.0, -19.0)*RAD, pow(2.0, -19.0));

	var_converged = OrbMech::IntegratedTLMCIterator(r_peri, lat, gamma, var_guess, sv_mcc.R, step, MJD, -dt, hMoon, sv_mcc.gravref);

	v_peri_conv = var_converged.x;
	azi_peri_conv = var_converged.y;
	lng_peri_conv = var_converged.z;

	OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat, gamma + PI05, azi_peri_conv, R_EMP, V_EMP);
	OrbMech::EMPToEcl(R_EMP, V_EMP, sv_n.MJD, sv_n.R, sv_n.V);
	sv_mcc_apo = coast(sv_n, -dt);

	DV = sv_mcc_apo.V - sv_mcc.V;

	sv_node = sv_n;
}

void RTCC::TLMCIntegratedXYZT(SV sv_mcc, double lat_node, double lng_node, double h_node, double MJD_node, VECTOR3 DV_guess, VECTOR3 &DV)
{
	SV sv_p;
	VECTOR3 step;
	double dt;

	dt = (MJD_node - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCIntegratedXYZTIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, _V(h_node, lat_node, lng_node), step, dt);
}

void RTCC::TLMCFirstGuessConic(SV sv_mcc, double lat, double h, double gamma, double MJD_P, VECTOR3 &DV, VECTOR3 &var_converged)
{
	SV sv_p;
	VECTOR3 R_EMP, V_EMP, R_MCC, V_MCC_apo, var_guess, step;
	double r_peri, lng_peri_conv, azi_peri_conv, v_peri_conv, dt;
	OBJHANDLE hMoon, hEarth;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	//mu = GGRAV*oapiGetMass(hMoon);

	r_peri = oapiGetSize(hMoon) + h;
	sv_p.MJD = MJD_P;
	dt = (MJD_P - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);

	//Initial guess
	var_guess = TLMCEmpiricalFirstGuess(r_peri, 0.0);

	var_converged = OrbMech::TLMCConicFirstGuessIterator(r_peri, lat, gamma, var_guess, sv_mcc.R, step, MJD_P, -dt, hMoon, sv_mcc.gravref);

	v_peri_conv = var_converged.x;
	azi_peri_conv = var_converged.y;
	lng_peri_conv = var_converged.z;

	OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat, gamma + PI05, azi_peri_conv, R_EMP, V_EMP);
	OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);
	OrbMech::rv_from_r0v0_tb(sv_p.R, sv_p.V, sv_p.MJD, hMoon, sv_mcc.gravref, -dt, R_MCC, V_MCC_apo);

	DV = V_MCC_apo - sv_mcc.V;
}

bool RTCC::TLMCFlybyConic(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_patch, V_patch, step;
	double mu_M, mu_E, dt1, dt2;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	target = _V(h_peri, lat_EMP, 20.0*1852.0);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCConicFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	if (sv_mcc.gravref == hEarth)
	{
		dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, mu_E, mu_M, R_patch, V_patch);
	}
	else
	{
		R_patch = sv_mcc.R;
		V_patch = sv_mcc.V + DV;
		dt1 = 0.0;
	}

	dt2 = OrbMech::timetoperi(R_patch, V_patch, mu_M);
	OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, mu_M);
	sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

	OrbMech::ReturnPerigeeConic(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCConicFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_patch, V_patch, R_EMP, V_EMP, step;
	double mu_M, mu_E, dt1, dt2, lng_EMP;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	target = _V(h_peri, 20.0*1852.0, inc_fr_des);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCConicSPSLunarFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	if (sv_mcc.gravref == hEarth)
	{
		dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, mu_E, mu_M, R_patch, V_patch);
	}
	else
	{
		R_patch = sv_mcc.R;
		V_patch = sv_mcc.V + DV;
		dt1 = 0.0;
	}

	dt2 = OrbMech::timetoperi(R_patch, V_patch, mu_M);
	OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, mu_M);

	sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	OrbMech::ReturnPerigeeConic(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCIntegratedFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_EMP, V_EMP, step;
	double mu_M, mu_E, dt1, lng_EMP;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu_E = GGRAV*oapiGetMass(hEarth);
	mu_M = GGRAV*oapiGetMass(hMoon);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	target = _V(h_peri, 20.0*1852.0, inc_fr_des);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCIntegratedSPSLunarFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

void RTCC::LaunchTimePredictionProcessor(LunarLiftoffTimeOpt *opt, LunarLiftoffResults *res)
{
	VECTOR3 R_LS;
	double h_1, theta_Ins, DH, E, theta_F, R_M, MJD_guess, t_L_guess;
	SV sv_P, sv_Ins;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");
	R_M = oapiGetSize(hMoon);

	theta_F = 130.0*RAD;
	h_1 = 60000.0*0.3048;
	theta_Ins = 18.2*RAD;
	DH = 15.0*1852.0;
	E = 26.6*RAD;

	R_LS = OrbMech::r_from_latlong(opt->lat, opt->lng, R_M + opt->alt);

	//Initial guess for launch is CSM flying over landing site longitude
	sv_P = GeneralTrajectoryPropagation(opt->sv_CSM, 0, OrbMech::MJDfromGET(opt->t_hole,opt->GETbase));
	MJD_guess = OrbMech::P29TimeOfLongitude(sv_P.R, sv_P.V, sv_P.MJD, sv_P.gravref, opt->lng);
	t_L_guess = OrbMech::GETfromMJD(MJD_guess, opt->GETbase);

	if (opt->opt == 0)
	{
		SV sv_TPI_guess;
		double t_TPI_guess, ttoMidnight;
		OBJHANDLE hSun;

		hSun = oapiGetObjectByName("Sun");

		//About 2.5 hours between liftoff and TPI
		t_TPI_guess = t_L_guess + 2.5*3600.0;
		sv_TPI_guess = GeneralTrajectoryPropagation(sv_P, 0, OrbMech::MJDfromGET(t_TPI_guess, opt->GETbase));

		ttoMidnight = OrbMech::sunrise(sv_TPI_guess.R, sv_TPI_guess.V, sv_TPI_guess.MJD, hMoon, hSun, 1, 1, false);
		res->t_TPI = t_TPI_guess + ttoMidnight;

		LunarLiftoffTimePredictionCFP(R_LS, sv_P, opt->GETbase, hMoon, opt->dt_1, h_1, opt->theta_1, theta_Ins, DH, E, t_L_guess, res->t_TPI, theta_F, *res);
	}
	else if (opt->opt == 1)
	{
		LunarLiftoffTimePredictionDT(R_LS, sv_P, opt->GETbase, hMoon, opt->dt_1, h_1, opt->theta_1, opt->dt_2, DH, E, t_L_guess, theta_F, *res);
	}
	else if (opt->opt == 2)
	{
		LunarLiftoffTimePredictionTCDT(R_LS, sv_P, opt->GETbase, hMoon, opt->dt_1, h_1, opt->theta_1, t_L_guess, *res);
	}
}

void RTCC::EntryUpdateCalc(SV sv0, double GETbase, double entryrange, bool highspeed, EntryResults *res)
{
	VECTOR3 REI, VEI, R3, V3, UR3, U_H3, U_LS, LSEF;
	double RCON, mu, dt2, t32, v3, S_FPA, gammaE, phie, te, t_LS, Sphie, Cphie, tLSMJD, l, m, n, phi, lambda2, EntryInterface;
	MATRIX3 R;
	OBJHANDLE hEarth;
	VECTOR3 R05G, V05G, UREI;
	double dt22, t2, EMSAlt, vEI;
	int rangeiter;

	if (entryrange == 0)
	{
		rangeiter = 1;
	}
	else
	{
		rangeiter = 2;
	}

	if (highspeed)
	{
		EMSAlt = 297431.0*0.3048;
	}
	else
	{
		EMSAlt = 284643.0*0.3048;
	}

	hEarth = oapiGetObjectByName("Earth");

	EntryInterface = 400000.0 * 0.3048;
	RCON = oapiGetSize(hEarth) + EntryInterface;
	mu = GGRAV*oapiGetMass(hEarth);

	dt2 = OrbMech::time_radius_integ(sv0.R, sv0.V, sv0.MJD, RCON, -1, sv0.gravref, hEarth, REI, VEI);

	UREI = unit(REI);
	vEI = length(VEI);
	res->ReA = asin(dotp(UREI, VEI) / vEI);

	t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, mu);
	OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, mu);
	UR3 = unit(R3);
	v3 = length(V3);
	S_FPA = dotp(UR3, V3) / v3;
	gammaE = asin(S_FPA);
	EntryCalculations::augekugel(v3, gammaE, phie, te);

	for (int iii = 0;iii < rangeiter;iii++)
	{
		t2 = (sv0.MJD - GETbase) * 24.0 * 3600.0 + dt2;	//EI time in seconds from launch
		t_LS = t2 + t32 + te;
		Sphie = sin(0.00029088821*phie);
		Cphie = cos(0.00029088821*phie);
		U_H3 = unit(crossp(crossp(R3, V3), R3));
		U_LS = UR3*Cphie + U_H3*Sphie;

		tLSMJD = GETbase + t_LS / 24.0 / 3600.0;
		//U_LS = tmul(Rot, U_LS);
		U_LS = _V(U_LS.x, U_LS.z, U_LS.y);
		R = OrbMech::GetRotationMatrix(hEarth, tLSMJD);
		LSEF = tmul(R, U_LS);
		l = LSEF.x;
		m = LSEF.z;
		n = LSEF.y;
		phi = asin(n);
		if (m > 0)
		{
			lambda2 = acos(l / cos(phi));
		}
		else
		{
			lambda2 = PI2 - acos(l / cos(phi));
		}
		if (lambda2 > PI) { lambda2 -= PI2; }

		res->latitude = phi;
		res->longitude = lambda2;


		dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, mu);
		OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, mu);

		res->RTGO = phie - 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		if (entryrange != 0)
		{
			te *= entryrange / res->RTGO;
			phie = entryrange + 3437.7468*acos(dotp(unit(R3), unit(R05G)));
		}
	}
	res->VIO = length(V05G);
	res->GET400K = t2;
	res->GET05G = t2 + t32 + dt22;	
}

void RTCC::CalcSPSGimbalTrimAngles(double CSMmass, double LMmass, double &p_T, double &y_T)
{
	double x1;

	x1 = LMmass / (CSMmass + LMmass)*6.2;
	p_T = atan2(-2.15 * RAD * 5.0, 5.0 + x1);
	y_T = atan2(0.95 * RAD * 5.0, 5.0 + x1);
}

bool RTCC::DockingInitiationProcessor(DKIOpt opt, DKIResults &res)
{
	// NOMENCLATURE:
	// R = position vector, V = velocity vector
	// A = active vehicle, P = passive vehicle
	// P = phasing maneuver, B = boost, HAM = height adjustment, H = height maneuver, C = coelliptic maneuver, F = final maneuver (TPI)

	//Check on N
	if (opt.plan == 0 || opt.plan == 1)
	{
		if (opt.N_HC % 2 == 0) return false;
		if (opt.N_PB % 2 == 0) return false;
	}
	else if (opt.plan == 2)
	{
		if (opt.N_HC % 2 != 0) return false;
		if (opt.N_PB % 2 == 0) return false;
	}
	else if (opt.plan == 4)
	{
		if (opt.N_HC % 2 == 0) return false;
		if (opt.N_PB % 2 != 0) return false;
	}

	SV sv_AP, sv_TPI_guess, sv_TPI, sv_PC;
	VECTOR3 u, R_AP, V_AP, V_APF, R_AH, V_AH, V_AHF, R_AC, V_AC, R_PC, V_PC, V_ACF, R_PJ, V_PJ, R_AFD, R_AF, V_AF;
	double mu, dv_P, p_P, dt_PH, c_P, eps_P, dv_H, dt_HC, t_H, t_C, dv_Po, e_Po, e_P, dv_rad_const;
	int s_P;

	eps_P = 0.000004;	//radians
	s_P = 0;
	p_P = c_P = 0.0;
	if (opt.plan == 4)
	{
		dv_P = 300.0*0.3048;
	}
	else
	{
		dv_P = 100.0*0.3048;
	}
	dv_H = 0.0;
	mu = GGRAV * oapiGetMass(opt.sv_A.gravref);

	if (opt.radial_dv == false)
	{
		dv_rad_const = 0.0;
	}
	else
	{
		dv_rad_const = -50.0*0.3048;
	}

	sv_TPI_guess = coast(opt.sv_P, opt.t_TPI_guess - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));

	if (opt.tpimode == 0)
	{
		sv_TPI = sv_TPI_guess;
		res.t_TPI = opt.t_TPI_guess;
	}
	else
	{
		OBJHANDLE hSun = oapiGetObjectByName("Sun");

		if (opt.tpimode == 1)
		{

			double ttoMidnight;
			ttoMidnight = OrbMech::sunrise(sv_TPI_guess.R, sv_TPI_guess.V, sv_TPI_guess.MJD, sv_TPI_guess.gravref, hSun, 1, 1, false);
			res.t_TPI = opt.t_TPI_guess + ttoMidnight;
			sv_TPI = coast(sv_TPI_guess, res.t_TPI - opt.t_TPI_guess);
		}
		else
		{
			SV sv_sunrise_guess;
			double ttoSunrise;

			sv_sunrise_guess = coast(sv_TPI_guess, opt.dt_TPI_sunrise);
			ttoSunrise = OrbMech::sunrise(sv_sunrise_guess.R, sv_sunrise_guess.V, sv_sunrise_guess.MJD, sv_sunrise_guess.gravref, hSun, 1, 0, false);
			res.t_TPI = opt.t_TPI_guess + ttoSunrise;
		}

		sv_TPI = coast(sv_TPI_guess, res.t_TPI - opt.t_TPI_guess);
	}

	//If we just needed the TPI time, return here
	if (opt.plan == 3) return true;

	sv_AP = coast(opt.sv_A, opt.t_TIG - OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase));

	OrbMech::QDRTPI(sv_TPI.R, sv_TPI.V, sv_TPI.MJD, sv_TPI.gravref, mu, opt.DH, opt.E, 0, R_PJ, V_PJ);
	R_AFD = R_PJ - unit(R_PJ)*opt.DH;

	u = unit(crossp(sv_TPI.R, sv_TPI.V));

	R_AP = unit(sv_AP.R - u * dotp(sv_AP.R, u))*length(sv_AP.R);
	V_AP = unit(sv_AP.V - u * dotp(sv_AP.V, u))*length(sv_AP.V);

	if (opt.plan == 2)
	{
		OrbMech::CSIToDH(R_AP, V_AP, sv_TPI.R, sv_TPI.V, opt.DH, mu, dv_P);
		V_APF = V_AP + tmul(OrbMech::LVLH_Matrix(R_AP, V_AP), _V(dv_P, 0.0, dv_rad_const));

		OrbMech::REVUP(R_AP, V_APF, 0.5, mu, R_AH, V_AH, dt_PH);
		t_H = opt.t_TIG + dt_PH;

		do
		{
			V_AHF = V_AH + unit(crossp(u, R_AH))*dv_H;
			OrbMech::REVUP(R_AH, V_AHF, 0.5*(double)opt.N_HC, mu, R_AC, V_AC, dt_HC);
			t_C = t_H + dt_HC;

			OrbMech::RADUP(sv_TPI.R, sv_TPI.V, R_AC, mu, R_PC, V_PC);
			V_ACF = OrbMech::CoellipticDV(R_AC, R_PC, V_PC, mu);

			OrbMech::rv_from_r0v0(R_AC, V_ACF, res.t_TPI - t_C, R_AF, V_AF, mu);
			e_P = OrbMech::sign(dotp(crossp(R_AF, R_AFD), u))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));

			if (p_P == 0 || abs(e_P) >= eps_P)
			{
				OrbMech::ITER(c_P, s_P, e_P, p_P, dv_H, e_Po, dv_Po);
			}
		} while (abs(e_P) >= eps_P);
	}
	else
	{
		do
		{
			V_APF = V_AP + tmul(OrbMech::LVLH_Matrix(R_AP, V_AP), _V(dv_P, 0.0, dv_rad_const));

			if (opt.plan == 1)
			{
				VECTOR3 R_AB, V_AB, V_ABF, R_AHAM, V_AHAM;
				double dt_PB, t_B, dv_B, t_HAM, dt_BHAM, dt_HAMH;

				dv_B = 10.0*0.3048;

				if (opt.maneuverline)
				{
					dt_PB = OrbMech::timetoapo(R_AP, V_APF, mu, 1);
				}
				else
				{
					dt_PB = opt.DeltaT_PBH;
				}

				t_B = opt.t_TIG + dt_PB;
				OrbMech::rv_from_r0v0(R_AP, V_APF, dt_PB, R_AB, V_AB, mu);

				V_ABF = V_AB + unit(crossp(u, R_AB))*dv_B;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AB, V_ABF, 0.5*(double)opt.N_PB, mu, R_AHAM, V_AHAM, dt_BHAM);
				}
				else
				{
					dt_BHAM = opt.DeltaT_BHAM;
					OrbMech::rv_from_r0v0(R_AB, V_ABF, dt_BHAM, R_AHAM, V_AHAM, mu);
				}

				t_HAM = t_B + dt_BHAM;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AHAM, V_AHAM, 0.5, mu, R_AH, V_AH, dt_HAMH);
				}
				else
				{
					dt_HAMH = opt.Delta_HAMH;
					OrbMech::rv_from_r0v0(R_AHAM, V_AHAM, dt_HAMH, R_AH, V_AH, mu);
				}

				t_H = t_HAM + dt_HAMH;

				res.t_Boost = t_B;
				res.dv_Boost = dv_B;
				res.t_HAM = t_HAM;
			}
			else if (opt.plan == 4)
			{
				VECTOR3 R_AB, V_AB, V_ABF;
				double dt_PB, dv_B, dt_BH, t_B;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AP, V_APF, 0.5*(double)opt.N_PB, mu, R_AB, V_AB, dt_PB);
				}
				else
				{
					dt_PB = opt.DeltaT_PBH;
					OrbMech::rv_from_r0v0(R_AP, V_APF, dt_PB, R_AB, V_AB, mu);
				}

				t_B = opt.t_TIG + dt_PB;

				OrbMech::CSIToDH(R_AB, V_AB, sv_TPI.R, sv_TPI.V, opt.DH, mu, dv_B);
				V_ABF = V_AB + unit(crossp(u, R_AB))*dv_B;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AB, V_ABF, 0.5, mu, R_AH, V_AH, dt_BH);
				}
				else
				{
					dt_BH = opt.DeltaT_BHAM;
					OrbMech::rv_from_r0v0(R_AB, V_ABF, dt_BH, R_AH, V_AH, mu);
				}

				t_H = t_B + dt_BH;

				res.t_Boost = t_B;
				res.dv_Boost = dv_B;
			}
			else
			{
				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AP, V_APF, 0.5*(double)opt.N_PB, mu, R_AH, V_AH, dt_PH);
				}
				else
				{
					dt_PH = opt.DeltaT_PBH;
					OrbMech::rv_from_r0v0(R_AP, V_APF, dt_PH, R_AH, V_AH, mu);
				}

				t_H = opt.t_TIG + dt_PH;
			}

			OrbMech::CSIToDH(R_AH, V_AH, sv_TPI.R, sv_TPI.V, opt.DH, mu, dv_H);
			V_AHF = V_AH + unit(crossp(u, R_AH))*dv_H;
			OrbMech::REVUP(R_AH, V_AHF, 0.5*(double)opt.N_HC, mu, R_AC, V_AC, dt_HC);
			t_C = t_H + dt_HC;
			OrbMech::RADUP(sv_TPI.R, sv_TPI.V, R_AC, mu, R_PC, V_PC);

			V_ACF = OrbMech::CoellipticDV(R_AC, R_PC, V_PC, mu);
			OrbMech::rv_from_r0v0(R_AC, V_ACF, res.t_TPI - t_C, R_AF, V_AF, mu);
			e_P = OrbMech::sign(dotp(crossp(R_AF, R_AFD), u))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));

			if (p_P == 0 || abs(e_P) >= eps_P)
			{
				OrbMech::ITER(c_P, s_P, e_P, p_P, dv_P, e_Po, dv_Po);
			}
		} while (abs(e_P) >= eps_P);
	}

	res.DV_Phasing = tmul(OrbMech::LVLH_Matrix(R_AP, V_AP), _V(dv_P, 0.0, dv_rad_const));
	res.t_CSI = t_H;
	res.dv_CSI = dv_H;
	res.t_CDH = t_C;
	res.DV_CDH = V_ACF - V_AC;

	return true;
}

void RTCC::ConcentricRendezvousProcessor(SPQOpt *opt, SPQResults &res)
{
	SV sv_A1, sv_P1;
	MATRIX3 Q_Xx;
	VECTOR3 u, R_A1, V_A1, V_A1F, R_A2, V_A2, R_P2, V_P2, R_PC, V_PC, V_A2F;
	double dv_CSI, mu, dt_1, t_CDH, dt_TPI;

	dv_CSI = 0.0;

	mu = GGRAV * oapiGetMass(opt->sv_A.gravref);
	sv_A1 = coast(opt->sv_A, opt->t_TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, opt->GETbase));
	sv_P1 = coast(opt->sv_P, opt->t_TIG - OrbMech::GETfromMJD(opt->sv_P.MJD, opt->GETbase));

	u = unit(crossp(sv_P1.R, sv_P1.V));

	R_A1 = unit(sv_A1.R - u * dotp(sv_A1.R, u))*length(sv_A1.R);
	V_A1 = unit(sv_A1.V - u * dotp(sv_A1.V, u))*length(sv_A1.V);

	//CDH calculation
	if (opt->maneuver == 1)
	{
		OrbMech::RADUP(sv_P1.R, sv_P1.V, R_A1, mu, R_PC, V_PC);
		V_A1F = OrbMech::CoellipticDV(R_A1, R_PC, V_PC, mu);
		Q_Xx = OrbMech::LVLH_Matrix(R_A1, V_A1);
		dt_TPI = OrbMech::findelev_conic(R_A1, V_A1F, sv_P1.R, sv_P1.V, opt->E, mu);

		res.DH = length(R_PC) - length(R_A1);
		res.dV_CDH = mul(Q_Xx, V_A1F - V_A1);
		res.t_CDH = opt->t_TIG;
		res.t_TPI = opt->t_TIG + dt_TPI;
		return;
	}

	if (opt->type == 0)
	{
		VECTOR3 R_P3, V_P3, R_PJ, V_PJ, R_AF, V_AF, R_AFD;
		double p_C, c_C, eps_C, e_C, e_Co, dv_CSIo, DH;
		int s_C;

		eps_C = 0.000004;	//radians
		p_C = c_C = 0.0;
		s_C = 0;
		dv_CSI = 10.0*0.3048;

		OrbMech::rv_from_r0v0(sv_P1.R, sv_P1.V, opt->t_TPI - opt->t_TIG, R_P3, V_P3, mu);

		do
		{
			V_A1F = V_A1 + unit(crossp(u, R_A1))*dv_CSI;
			OrbMech::REVUP(R_A1, V_A1F, 0.5, mu, R_A2, V_A2, dt_1);
			t_CDH = opt->t_TIG + dt_1;
			OrbMech::RADUP(R_P3, V_P3, R_A2, mu, R_PC, V_PC);
			DH = length(R_PC) - length(R_A2);
			V_A2F = OrbMech::CoellipticDV(R_A2, R_PC, V_PC, mu);
			OrbMech::rv_from_r0v0(R_A2, V_A2F, opt->t_TPI - t_CDH, R_AF, V_AF, mu);

			OrbMech::QDRTPI(R_P3, V_P3, opt->GETbase + opt->t_TPI / 24.0 / 3600.0, sv_P1.gravref, mu, DH, opt->E, 0, R_PJ, V_PJ);
			R_AFD = R_PJ - unit(R_PJ)*DH;

			e_C = OrbMech::sign(dotp(crossp(R_AF, R_AFD), u))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));

			if (p_C == 0 || abs(e_C) >= eps_C)
			{
				OrbMech::ITER(c_C, s_C, e_C, p_C, dv_CSI, e_Co, dv_CSIo);
			}
		} while (abs(e_C) >= eps_C);

		res.dV_CSI = _V(dv_CSI, 0, 0);
		res.t_CDH = t_CDH;
		Q_Xx = OrbMech::LVLH_Matrix(R_A2, V_A2);
		res.dV_CDH = mul(Q_Xx, V_A2F - V_A2);
		res.t_TPI = opt->t_TPI;
		res.DH = DH;
	}
	else
	{
		OrbMech::CSIToDH(R_A1, V_A1, sv_P1.R, sv_P1.V, opt->DH, mu, dv_CSI);
		V_A1F = V_A1 + unit(crossp(u, R_A1))*dv_CSI;
		OrbMech::REVUP(R_A1, V_A1F, 0.5, mu, R_A2, V_A2, dt_1);
		t_CDH = opt->t_TIG + dt_1;
		OrbMech::RADUP(sv_P1.R, sv_P1.V, R_A2, mu, R_PC, V_PC);
		V_A2F = OrbMech::CoellipticDV(R_A2, R_PC, V_PC, mu);
		OrbMech::rv_from_r0v0(sv_P1.R, sv_P1.V, t_CDH - opt->t_TIG, R_P2, V_P2, mu);
		dt_TPI = OrbMech::findelev_conic(R_A2, V_A2F, R_P2, V_P2, opt->E, mu);

		res.dV_CSI = _V(dv_CSI, 0, 0);
		res.t_CDH = t_CDH;
		Q_Xx = OrbMech::LVLH_Matrix(R_A2, V_A2);
		res.dV_CDH = mul(Q_Xx, V_A2F - V_A2);
		res.DH = opt->DH;
		res.t_TPI = t_CDH + dt_TPI;
	}
}

VECTOR3 RTCC::HatchOpenThermalControl(VESSEL *v, MATRIX3 REFSMMAT)
{
	MATRIX3 SMNB, MY, MX;
	VECTOR3 R_SC, UX, UY, UZ;
	OBJHANDLE hSun;

	hSun = oapiGetObjectByName("Sun");
	v->GetRelativePos(hSun, R_SC);
	R_SC = _V(R_SC.x, R_SC.z, R_SC.y);
	UZ = unit(R_SC);
	UY = unit(crossp(UZ, _V(0.0, 0.0, -1.0)));
	UX = crossp(UY, UZ);

	SMNB = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	MY = OrbMech::_MRy(-15.0*RAD);
	MX = OrbMech::_MRx(-80.0*RAD);
	SMNB = mul(MX, mul(MY, SMNB));
	return OrbMech::CALCGAR(REFSMMAT, SMNB);
}

void RTCC::DockingAlignmentProcessor(DockAlignOpt &opt)
{
	//Option 1: LM REFSMMAT from CSM REFSMMAT, CSM attitude, docking angle and LM gimbal angles
	//Option 2: LM gimbal angles from CSM REFSMMAT, LM REFSMMAT, CSM gimbal angles and docking angle
	//Option 3: CSM gimbal angles from CSM REFSMMAT, LM REFSMMAT, LM gimbal angles and docking angle
	//Coordinate Systems:
	//Navigation Base (NB)
	//Stable Member (SM)
	//Basic Reference Coordinate System (BRCS)
	//REFSMMAT is BRCS to SM

	MATRIX3 M_NBCSM_NBLM;

	M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(opt.DockingAngle);

	if (opt.type == 1)
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_NBLM, M_BRCS_NBCSM, M_BRCS_NBLM, M_BRCS_SMLM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(opt.CSMAngles);
		M_SMLM_NBLM = OrbMech::CALCSMSC(opt.LMAngles);
		M_BRCS_NBCSM = mul(M_SMCSM_NBCSM, opt.CSM_REFSMMAT);
		M_BRCS_NBLM = mul(M_NBCSM_NBLM, M_BRCS_NBCSM);
		M_BRCS_SMLM = mul(OrbMech::tmat(M_SMLM_NBLM), M_BRCS_NBLM);

		opt.LM_REFSMMAT = M_BRCS_SMLM;
	}
	else if (opt.type == 2)
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_SMCSM, M_SMCSM_NBLM, M_SMLM_NBLM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(opt.CSMAngles);
		M_SMLM_SMCSM = mul(opt.CSM_REFSMMAT, OrbMech::tmat(opt.LM_REFSMMAT));
		M_SMCSM_NBLM = mul(M_NBCSM_NBLM, M_SMCSM_NBCSM);
		M_SMLM_NBLM = mul(M_SMCSM_NBLM, M_SMLM_SMCSM);

		opt.LMAngles = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), M_SMLM_NBLM);
	}
	else if (opt.type == 3)
	{
		MATRIX3 M_SMLM_NBLM, M_SMCSM_NBCSM, M_SMCSM_SMLM, M_SMCSM_NBLM;

		M_SMLM_NBLM = OrbMech::CALCSMSC(opt.LMAngles);
		M_SMCSM_SMLM = mul(opt.LM_REFSMMAT, OrbMech::tmat(opt.CSM_REFSMMAT));
		M_SMCSM_NBLM = mul(M_SMLM_NBLM, M_SMCSM_SMLM);
		M_SMCSM_NBCSM = mul(OrbMech::tmat(M_NBCSM_NBLM), M_SMCSM_NBLM);

		opt.CSMAngles = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), M_SMCSM_NBCSM);
	}
}

SV RTCC::GeneralTrajectoryPropagation(SV sv0, int opt, double param)
{
	//Update to the given time
	if (opt == 0)
	{
		double MJD1, dt;

		MJD1 = param;
		dt = (MJD1 - sv0.MJD)*24.0*3600.0;
		return coast(sv0, dt);
	}
	//Update to the given mean anomaly
	else if (opt == 1)
	{
		SV sv1;
		OELEMENTS coe;
		double l_D, mu, E1, f1, dt, err, df;
		int n, nmax;

		l_D = param;
		mu = GGRAV * oapiGetMass(sv0.gravref);
		coe = OrbMech::coe_from_sv(sv0.R, sv0.V, mu);
		sv1 = sv0;
		n = 0;
		nmax = 10;
		err = 1e-6;

		do
		{
			coe = OrbMech::coe_from_sv(sv1.R, sv1.V, mu);
			E1 = OrbMech::kepler_E(coe.e, l_D);
			f1 = 2.0*atan2(sqrt(1.0 + coe.e)*sin(E1 / 2.0), sqrt(1.0 - coe.e)*cos(E1 / 2.0));
			df = fmod(f1 - coe.TA, PI2);
			dt = OrbMech::time_theta(sv1.R, sv1.V, df, mu);
			if (n == 0)
			{
				double T_p = OrbMech::period(sv1.R, sv1.V, mu);
				if (dt < 0.0)
				{
					dt += T_p;
				}
			}
			sv1 = coast(sv1, dt);
			n++;
		} while (n < nmax && abs(df) > err);

		return sv1;
	}
	//Update to given argument of latitude
	else if (opt == 2)
	{
		SV sv1;
		OELEMENTS coe;
		MATRIX3 obl;
		VECTOR3 R1_equ, V1_equ;
		double u_D, mu, dt, err, nn, T_p, u_0, du;
		int n, nmax;
		bool lowecclogic;

		u_D = param;
		mu = GGRAV * oapiGetMass(sv0.gravref);
		coe = OrbMech::coe_from_sv(sv0.R, sv0.V, mu);
		sv1 = sv0;
		n = 0;
		nmax = 10;
		err = 1e-6;
		T_p = OrbMech::period(sv0.R, sv0.V, mu);
		nn = PI2 / T_p;

		if (coe.e > 0.005)
		{
			lowecclogic = false;
		}
		else
		{
			lowecclogic = true;
		}

		do
		{
			obl = OrbMech::GetObliquityMatrix(sv1.gravref, sv1.MJD);
			R1_equ = rhtmul(obl, sv1.R);
			V1_equ = rhtmul(obl, sv1.V);
			coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);

			u_0 = coe.TA + coe.w;
			du = fmod(u_D - u_0, PI2);
			if (abs(du) > PI)
			{
				du -= PI2 * OrbMech::sign(du);
			}

			if (lowecclogic)
			{
				dt = du / nn;
			}
			else
			{
				dt = OrbMech::time_theta(sv1.R, sv1.V, du, mu);
			}

			if (n == 0)
			{
				double T_p = OrbMech::period(sv1.R, sv1.V, mu);
				if (dt < 0.0)
				{
					dt += T_p;
				}
			}
			sv1 = coast(sv1, dt);
			n++;
		} while (n < nmax && abs(du) > err);

		return sv1;
	}

	return sv0;
}

void RTCC::ApsidesDeterminationSubroutine(SV sv0, SV &sv_a, SV &sv_p)
{
	OELEMENTS coe;
	double mu;
	bool lowecclogic;
	
	mu = GGRAV * oapiGetMass(sv0.gravref);
	coe = OrbMech::coe_from_sv(sv0.R, sv0.V, mu);

	if (coe.e > 0.005 || sv0.gravref == oapiGetObjectByName("Moon"))
	{
		lowecclogic = false;
	}
	else
	{
		lowecclogic = true;
	}

	if (lowecclogic == false)
	{
		sv_p = GeneralTrajectoryPropagation(sv0, 1, 0.0);
		sv_a = GeneralTrajectoryPropagation(sv0, 1, PI);
	}
	else
	{
		SV sv1, sv2;
		double u_x, u_y;

		//First guess
		ApsidesArgumentofLatitudeDetermination(sv0, u_x, u_y);

		sv1 = GeneralTrajectoryPropagation(sv0, 2, u_x);
		sv2 = GeneralTrajectoryPropagation(sv0, 2, u_y);

		if (length(sv1.R) > length(sv2.R))
		{
			sv_a = sv1;
			sv_p = sv2;
		}
		else
		{
			sv_p = sv1;
			sv_a = sv2;
		}
	}
}

VECTOR3 RTCC::HeightManeuverInteg(SV sv0, double dh)
{
	OELEMENTS coe;
	SV sv0_apo, sv1;
	MATRIX3 Rot;
	VECTOR3 R1_equ, V1_equ, am;
	double r_D, dv_H, e_H, eps, p_H, c_I, e_Ho, dv_Ho, u_b, mu, u_d;
	int s_F;

	sv0_apo = sv0;
	mu = GGRAV * oapiGetMass(sv0.gravref);

	Rot = OrbMech::GetObliquityMatrix(sv0.gravref, sv0.MJD);
	R1_equ = rhtmul(Rot, sv0.R);
	V1_equ = rhtmul(Rot, sv0.V);
	coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
	u_b = fmod(coe.TA + coe.w, PI2);
	u_d = fmod(u_b + PI, PI2);
	sv1 = GeneralTrajectoryPropagation(sv0, 2, u_d);

	r_D = length(sv1.R) + dh;
	am = unit(crossp(sv0.R, sv0.V));
	dv_H = 0.0;
	eps = 1.0;
	p_H = c_I = 0.0;
	s_F = 0;

	do
	{
		sv0_apo.V = sv0.V + unit(crossp(am, sv0.R))*dv_H;

		V1_equ = rhtmul(Rot, sv0_apo.V);
		coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
		u_b = fmod(coe.TA + coe.w, PI2);
		u_d = fmod(u_b + PI, PI2);
		sv1 = GeneralTrajectoryPropagation(sv0_apo, 2, u_d);
		e_H = length(sv1.R) - r_D;

		if (p_H == 0 || abs(e_H) >= eps)
		{
			OrbMech::ITER(c_I, s_F, e_H, p_H, dv_H, e_Ho, dv_Ho);
			if (s_F == 1)
			{
				return _V(0, 0, 0);
			}
		}
	} while (abs(e_H) >= eps);

	return sv0_apo.V - sv0.V;
}

VECTOR3 RTCC::ApoapsisPeriapsisChangeInteg(SV sv0, double r_AD, double r_PD)
{
	OELEMENTS coe_bef, coe_aft, coe_p, coe_a;
	SV sv0_apo, sv_a, sv_p;
	double r_a_b, r_p_b, a_ap, a_p, dr_a_max, dr_p_max, a_D, e_D, mu, r_b, cos_theta_A, theta_A, r_a, r_p, a_a, cos_E_a, E_a, u_b, r_a_a, r_p_a;
	double dr_ap0, dr_p0, dr_ap_c, dr_p_c, dr_ap1, dr_p1, ddr_ap, ddr_p, eps;
	int n, nmax;

	mu = GGRAV * oapiGetMass(sv0.gravref);
	sv0_apo = sv0;
	n = 0;
	nmax = 100;
	eps = 1.0;

	coe_bef = OrbMech::coe_from_sv(sv0.R, sv0.V, mu);
	u_b = fmod(coe_bef.TA + coe_bef.w, PI2);
	ApsidesDeterminationSubroutine(sv0, sv_a, sv_p);

	if (r_AD == r_PD)
	{
		r_PD = r_PD - 0.1*1852.0;
	}

	r_b = length(sv0.R);

	if (r_AD < r_b)
	{
		r_AD = r_b;
	}
	if (r_PD > r_b)
	{
		r_PD = r_b;
	}

	r_a_b = length(sv_a.R);
	r_p_b = length(sv_p.R);
	coe_p = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
	a_p = coe_p.h*coe_p.h / (mu*(1.0 - coe_p.e * coe_p.e));
	coe_a = OrbMech::coe_from_sv(sv_a.R, sv_a.V, mu);
	a_ap = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e * coe_a.e));

	dr_a_max = (a_p*(1.0 + coe_p.e) - r_a_b)*(r_AD / r_a_b);
	dr_p_max = (a_ap*(1.0 - coe_a.e) - r_p_b)*(r_PD / r_p_b);
	a_D = (r_AD + r_PD) / 2.0;
	e_D = abs((a_D - r_PD) / a_D);
	cos_theta_A = (a_D*(1.0 - e_D * e_D) - r_b) / (e_D*r_b);
	if (abs(cos_theta_A) > 1.0)
	{
		if (cos_theta_A < 0)
		{
			cos_theta_A = -1.0;
		}
		else
		{
			cos_theta_A = 1.0;
		}
	}
	theta_A = acos(cos_theta_A);
	r_a = r_AD + (PI - theta_A) / PI * dr_a_max;
	r_p = r_PD + theta_A / PI * dr_p_max;

	do
	{
		a_a = (r_a + r_p) / 2.0;
		coe_aft.e = abs((a_a - r_p) / a_a);
		cos_E_a = (a_a - r_b) / (a_a*coe_aft.e);

		if (abs(cos_E_a) > 1.0)
		{
			if (cos_E_a < 0)
			{
				cos_E_a = -1.0;
			}
			else
			{
				cos_E_a = 1.0;
			}
			a_a = r_b / (1.0 - coe_aft.e * cos_E_a);
		}

		E_a = acos(cos_E_a);
		coe_aft.TA = acos2((a_a*(1.0 - coe_aft.e * coe_aft.e) - r_b) / (coe_aft.e*r_b));
		if (coe_bef.TA > PI)
		{
			E_a = PI2 - E_a;
			coe_aft.TA = PI2 - coe_aft.TA;
		}
		//l_a = E_a - coe_aft.e * sin(E_a);
		coe_aft.w = u_b - coe_aft.TA;
		coe_aft.i = coe_bef.i;
		coe_aft.RA = coe_bef.RA;
		coe_aft.h = sqrt(a_a*mu*(1.0 - coe_aft.e*coe_aft.e));
		if (coe_aft.w < 0)
		{
			coe_aft.w += PI2;
		}
		OrbMech::sv_from_coe(coe_aft, mu, sv0_apo.R, sv0_apo.V);
		ApsidesDeterminationSubroutine(sv0_apo, sv_a, sv_p);
		r_a_a = length(sv_a.R);
		r_p_a = length(sv_p.R);

		if (n == 0)
		{
			dr_ap0 = r_AD - r_a_a;
			dr_p0 = r_PD - r_p_a;
			dr_ap_c = dr_ap0;
			dr_p_c = dr_p0;
		}
		else
		{
			dr_ap1 = r_AD - r_a_a;
			dr_p1 = r_PD - r_p_a;
			ddr_ap = dr_ap0 - dr_ap1;
			ddr_p = dr_p0 - dr_p1;
			dr_ap_c = dr_ap1 * dr_ap_c / ddr_ap;
			dr_p_c = dr_p1 * dr_p_c / ddr_p;
			dr_ap0 = dr_ap1;
			dr_p0 = dr_p1;
		}

		r_a = r_a + dr_ap_c;
		r_p = r_p + dr_p_c;

		n++;
		
	} while (n < nmax && (abs(r_AD - r_a_a) > eps || abs(r_PD - r_p_a) > eps));

	return sv0_apo.V - sv0.V;
}

void RTCC::ApsidesArgumentofLatitudeDetermination(SV sv0, double &u_x, double &u_y)
{
	OELEMENTS coe;
	SV sv[3];
	MATRIX3 obl;
	VECTOR3 R1_equ, V1_equ;
	double u[3], r[3], gamma, u_0, mu;

	mu = GGRAV * oapiGetMass(sv0.gravref);

	sv[0] = sv0;
	sv[1] = coast(sv[0], 15.0*60.0);
	sv[2] = coast(sv[1], 15.0*60.0);

	for (int i = 0;i < 3;i++)
	{
		obl = OrbMech::GetObliquityMatrix(sv[i].gravref, sv[i].MJD);
		R1_equ = rhtmul(obl, sv[i].R);
		V1_equ = rhtmul(obl, sv[i].V);
		coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
		u[i] = fmod(coe.TA + coe.w, PI2);
		r[i] = length(R1_equ);
	}

	gamma = (r[0] - r[1]) / (r[0] - r[2]);
	u_0 = atan2(sin(u[0]) - sin(u[1]) - gamma * (sin(u[0]) - sin(u[2])), gamma*(cos(u[2]) - cos(u[0])) - cos(u[1]) + cos(u[0]));
	if (u_0 < 0)
	{
		u_0 += PI2;
	}

	u_x = u_0 + PI05;
	u_y = u_0 - PI05;

	if (u_x < 0)
	{
		u_x += PI2;
	}
	if (u_y < 0)
	{
		u_y += PI2;
	}

}

VECTOR3 RTCC::CircularizationManeuverInteg(SV sv0)
{
	SV sv0_apo, sv1;
	OELEMENTS coe;
	MATRIX3 Rot, Q_Xx;
	VECTOR3 R0_equ, V0_equ, V_LVLH, DV1, DV2;
	double u_b, u_d, dh, mu;

	mu = GGRAV * oapiGetMass(sv0.gravref);
	sv0_apo = sv0;

	Q_Xx = OrbMech::LVLH_Matrix(sv0_apo.R, sv0_apo.V);
	V_LVLH = mul(Q_Xx, sv0_apo.V);
	V_LVLH.z = 0.0;
	sv0_apo.V = tmul(Q_Xx, V_LVLH);
	DV1 = sv0_apo.V - sv0.V;

	Rot = OrbMech::GetObliquityMatrix(sv0_apo.gravref, sv0_apo.MJD);
	R0_equ = rhtmul(Rot, sv0_apo.R);
	V0_equ = rhtmul(Rot, sv0_apo.V);
	coe = OrbMech::coe_from_sv(R0_equ, V0_equ, mu);
	u_b = fmod(coe.TA + coe.w, PI2);
	u_d = fmod(u_b + PI, PI2);
	sv1 = GeneralTrajectoryPropagation(sv0_apo, 2, u_d);
	dh = length(sv0_apo.R) - length(sv1.R);
	DV2 = HeightManeuverInteg(sv0_apo, dh);

	return DV1 + DV2;
}

bool RTCC::GETEval(double get)
{
	if (OrbMech::GETfromMJD(oapiGetSimMJD(), calcParams.TEPHEM) > get)
	{
		return true;
	}

	return false;
}

void RTCC::AGOPCislunarNavigation(SV sv, MATRIX3 REFSMMAT, int star, double yaw, VECTOR3 &IMUAngles, double &TA, double &SA)
{
	MATRIX3 M, SBNB, CBNB;
	VECTOR3 U_S, R_ZC, U_Z, u0, u1, u2, R_H, U_SH, t0, t1, R_L, R_CH;
	double r_Z, x_H, y_H, A, alpha, beta, A0, A1, a;
	bool H, k;

	U_S = navstars[star - 1];
	R_ZC = sv.R;
	//Near Horizon
	H = false;
	U_Z = _V(0, 0, 1);
	r_Z = oapiGetSize(sv.gravref);
	a = -0.5676353234;
	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));

	u2 = unit(crossp(U_S, R_ZC));
	u0 = unit(crossp(U_Z, u2));
	u1 = crossp(u2, u0);
	M = _M(u0.x, u0.y, u0.z, u1.x, u1.y, u1.z, u2.x, u2.y, u2.z);

	R_H = mul(M, R_ZC);
	U_SH = mul(M, U_S);

	x_H = R_H.x;
	y_H = R_H.z;

	A = (x_H*x_H + y_H * y_H) / (r_Z*r_Z);
	alpha = y_H * sqrt(A - 1.0);
	beta = x_H * sqrt(A - 1.0);

	t0 = _V(x_H + alpha, y_H - beta, 0)*1.0 / A;
	t1 = _V(x_H - alpha, y_H + beta, 0)*1.0 / A;
	A0 = dotp(U_SH, unit(t0 - R_H));
	A1 = dotp(U_SH, unit(t1 - R_H));

	if (A1 > A0)
		k = true;
	else
		k = false;

	if (H) k = !k;

	if (k == false)
		R_L = tmul(M, t0);
	else
		R_L = tmul(M, t1);

	//Vector pointing from CSM to horizon
	R_CH = R_L - R_ZC;

	MATRIX3 CBSB;
	VECTOR3 X_SB, Y_SB, Z_SB;

	Z_SB = unit(R_CH);
	Y_SB = unit(crossp(Z_SB, U_S));
	X_SB = crossp(Y_SB, Z_SB);

	CBSB = _M(X_SB.x, X_SB.y, X_SB.z, Y_SB.x, Y_SB.y, Y_SB.z, Z_SB.x, Z_SB.y, Z_SB.z);

	CBNB = mul(SBNB, CBSB);
	IMUAngles = OrbMech::CALCGAR(REFSMMAT, CBNB);

	//Calculate sextant angles
	OrbMech::CALCSXA(CBNB, U_S, TA, SA);
}

VECTOR3 RTCC::LOICrewChartUpdateProcessor(SV sv0, double GETbase, MATRIX3 REFSMMAT, double p_EMP, double LOI_TIG, VECTOR3 dV_LVLH_LOI, double p_T, double y_T)
{
	SV sv_tig;
	MATRIX3 M_EMP, M_R, M, M_RTM;
	VECTOR3 X_B, UX, UY, UZ, IMUangles;
	double dt, headsswitch;

	headsswitch = -1.0;

	dt = LOI_TIG - OrbMech::GETfromMJD(sv0.MJD, GETbase);
	sv_tig = coast(sv0, dt);
	M_EMP = OrbMech::EMPMatrix(sv_tig.MJD);
	X_B = tmul(M_EMP, _V(-sin(p_EMP), cos(p_EMP), 0));

	UX = X_B;
	UY = unit(crossp(X_B, sv_tig.R*headsswitch));
	UZ = unit(crossp(X_B, crossp(X_B, sv_tig.R*headsswitch)));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
	M_RTM = mul(OrbMech::tmat(M_R), M);
	IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(OrbMech::tmat(M), M_R));

	return IMUangles;
}

void RTCC::LunarAscentProcessor(VECTOR3 R_LS, double m0, SV sv_CSM, double GETbase, double t_liftoff, double v_LH, double v_LV, double &theta, double &dt_asc, SV &sv_Ins)
{
	//Test
	AscentGuidance asc;
	AscDescIntegrator integ;
	SV sv_CSM_TIG, sv_ins;
	MATRIX3 Rot;
	VECTOR3 R0, V0, R, V, U_FDP, U_M, u, R0_proj;
	double t_go, Thrust, w_M, dt, t_total, isp, m1;
	bool stop = false;

	t_total = 0.0;
	dt = t_liftoff - OrbMech::GETfromMJD(sv_CSM.MJD, GETbase);
	sv_CSM_TIG = coast(sv_CSM, dt);

	asc.Init(sv_CSM_TIG.R, sv_CSM_TIG.V, m0, length(R_LS), v_LH, v_LV);
	Rot = OrbMech::GetRotationMatrix(sv_CSM_TIG.gravref, sv_CSM_TIG.MJD);
	R0 = rhmul(Rot, R_LS);
	w_M = PI2 / oapiGetPlanetPeriod(sv_CSM_TIG.gravref);
	U_M = rhmul(Rot, _V(0, 0, 1));
	V0 = crossp(U_M, R0)*w_M;

	R = R0;
	V = V0;
	m1 = m0;
	integ.Init(unit(R));

	while (stop == false)
	{
		asc.Guidance(R, V, m1, t_total, U_FDP, t_go, Thrust, isp);
		stop = integ.Integration(R, V, m1, t_total, U_FDP, t_go, Thrust, isp);
	}

	u = unit(crossp(R, V));
	R0_proj = unit(R0 - u * dotp(R0, u))*length(R0);
	theta = acos(dotp(unit(R0_proj), unit(R)));
	dt_asc = t_total;

	sv_ins.R = R;
	sv_ins.V = V;
	sv_ins.mass = m1;
	sv_ins.gravref = sv_CSM_TIG.gravref;
	sv_ins.MJD = sv_CSM_TIG.MJD + dt_asc / 24.0 / 3600.0;
	sv_Ins = sv_ins;
}

bool RTCC::PDIIgnitionAlgorithm(SV sv, double GETbase, VECTOR3 R_LS, double TLAND, MATRIX3 REFSMMAT, SV &sv_IG, double &t_go, double &CR, VECTOR3 &U_IG)
{
	SV sv_I;
	MATRIX3 C_GP, Rot;
	VECTOR3 U_FDP, dV_TrP, R_TG, V_TG, A_TG, R_LSI, R_LSP, W_I, W_P, R_P, G_P, V_P, R_G, V_SURFP, V_G, A_G, A_FDP;
	VECTOR3 C_XGP, C_YGP, C_ZGP;
	double GUIDDURN, AF_TRIM, DELTTRIM, TTT, t_pip, dt_I, FRAC;
	double v_IGG, r_IGXG, r_IGZG, K_X, K_Y, K_V;
	double J_TZG, A_TZG, V_TZG, R_TZG;
	double LEADTIME, w_M, t_2, t_I, PIPTIME, mu, t_pipold, eps, dTTT, TTT_P, TEM, q;
	int n1, n2, COUNT_TTT;

	GUIDDURN = 664.4;
	AF_TRIM = 0.350133;
	DELTTRIM = 26.0;
	n1 = 40;
	n2 = 2;
	COUNT_TTT = 0;
	U_FDP = dV_TrP = _V(0, 0, 0);
	C_GP = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	TTT = 0.0;
	t_pip = TLAND;
	dt_I = 1.0;
	FRAC = 43455.0;
	mu = GGRAV * oapiGetMass(sv.gravref);

	v_IGG = 1690.256208;
	r_IGXG = -39782.453328;
	r_IGZG = -436655.657;
	K_X = 0.617631;
	K_Y = 2.4770341207e-6;
	K_V = 410.0;
	J_TZG = -0.01882677*0.3048;
	A_TZG = -54.6264*0.3048 / 6.0;
	V_TZG = -18.72*0.3048 / 18.0;
	R_TG = _V(52.375308, 0.0, -3254.836061);
	R_TZG = R_TG.z;
	V_TG = _V(-105.876, 0.0, -1.04)*0.3048;
	A_TG = _V(0.6241, 0.0, -9.1044)*0.3048;
	LEADTIME = 2.2;
	w_M = 2.66169948e-6;

	t_2 = OrbMech::GETfromMJD(sv.MJD, GETbase);
	Rot = OrbMech::GetRotationMatrix(sv.gravref, GETbase + TLAND / 24.0 / 3600.0);
	R_LSI = rhmul(Rot, R_LS);
	R_LSP = mul(REFSMMAT, R_LSI);
	t_I = TLAND - GUIDDURN;
	W_I = rhmul(Rot, _V(0.0, 0.0, 1.0));
	W_P = mul(REFSMMAT, W_I)*w_M;
	sv_I = coast(sv, t_I - t_2);

	while (abs(dt_I) > 0.08 && n1>0)
	{
		PIPTIME = t_I;
		R_P = mul(REFSMMAT, sv_I.R);
		G_P = -R_P / pow(length(R_P), 3.0)*mu;

		n2 = 2;

		t_pipold = t_pip;
		t_pip = PIPTIME;
		t_2 = PIPTIME;
		R_LSP = unit(R_LSP + crossp(W_P, R_LSP)*(t_pip - t_pipold))*length(R_LS);
		TTT = TTT + t_pip - t_pipold;

		while (n2 > 0)
		{
			V_P = mul(REFSMMAT, sv_I.V) + dV_TrP;

			R_G = mul(C_GP, R_P - R_LSP);
			V_SURFP = V_P - crossp(W_P, R_P);
			V_G = mul(C_GP, V_SURFP);

			COUNT_TTT = 0;
			eps = abs(TTT / 128.0);
			do
			{
				dTTT = -(J_TZG*pow(TTT, 3.0) + 6.0*A_TZG*TTT*TTT + (18.0*V_TZG + 6.0*V_G.z)*TTT + 24.0*(R_TZG - R_G.z)) / (3.0*J_TZG*TTT*TTT + 12.0*A_TZG*TTT + 18.0*V_TZG + 6.0*V_G.z);
				TTT += dTTT;
				COUNT_TTT++;
			} while (abs(dTTT) > eps && COUNT_TTT < 8);

			if (COUNT_TTT == 8)
			{
				return false;
			}

			TTT_P = TTT + LEADTIME;
			A_G = (R_TG - R_G)*(-24.0*TTT_P / pow(TTT, 3.0) + 36.0*TTT_P*TTT_P / pow(TTT, 4.0));
			A_G += V_TG * (-18.0*TTT_P / TTT / TTT + 24.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += V_G * (-6.0*TTT_P / TTT / TTT + 12.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += A_TG * (6.0*TTT_P*TTT_P / TTT / TTT - 6.0*TTT_P / TTT + 1.0);

			A_FDP = tmul(C_GP, A_G) - G_P;
			//s = length(A_FDP);
			TEM = FRAC * FRAC / sv.mass / sv.mass - A_FDP.x*A_FDP.x - A_FDP.y*A_FDP.y;
			if (TEM < 0.0)
			{
				TEM = 0.0;
			}
			if (sqrt(TEM) + A_FDP.z < 0.0)
			{
				A_FDP.z = -sqrt(TEM);
			}
			U_FDP = A_FDP;

			C_XGP = unit(R_LSP);
			C_YGP = unit(crossp(unit(V_SURFP*TTT / 4.0 + R_LSP - R_P), R_LSP));
			C_ZGP = crossp(C_XGP, C_YGP);
			C_GP = _M(C_XGP.x, C_YGP.x, C_ZGP.x, C_XGP.y, C_YGP.y, C_ZGP.y, C_XGP.z, C_YGP.z, C_ZGP.z);

			dV_TrP = unit(U_FDP)*DELTTRIM*AF_TRIM;

			n2--;
		}

		n1--;
		q = K_X * (R_G.x - r_IGXG) + K_Y * R_G.y*R_G.y + R_G.z - r_IGZG + K_V * (length(V_G) - v_IGG);
		dt_I = -q / (V_G.z + K_X * V_G.x);
		t_I += dt_I;

		if (abs(dt_I) > 0.08)
		{
			sv_I = coast(sv_I, dt_I);
		}
	}

	if (n1 == 0)
	{
		return false;
	}

	sv_IG = coast(sv_I, -DELTTRIM);
	t_go = TTT;
	U_IG = U_FDP;
	CR = dotp(unit(crossp(V_P, R_P)), R_LSP);

	return true;
}

//Based on NTRS 19740072723
bool RTCC::PoweredDescentAbortProgram(PDAPOpt opt, PDAPResults &res)
{
	SPQOpt conopt;
	SPQResults conres;
	DescentGuidance descguid;
	AscentGuidance ascguid;
	AscDescIntegrator integ;
	SV sv_I_guess, sv_IG, sv_D, sv_CSM_Ins, sv_LM_Ins, sv_Abort, sv_CAN, sv_CAN_apo, sv_CSM_Abort;
	MATRIX3 Rot, Q_Xx;
	VECTOR3 U_FDP, WM, WI, W, R_LSP, U_FDP_abort;
	double t_go, CR, t_PDI, t_D, t_UL, t_stage, W_TD, T_DPS, dt_abort, Z_D_dot, R_D_dot, W_TA, t, T, isp, t_Ins, TS, theta, mu, r_Ins, A_Ins, H_a, t_CSI, DH_D;
	double SLOPE, dV_Inc, dh_apo, w_M, V_H_min, t_CAN, dt_CSI, R_a, R_a_apo, dt_CAN, theta_D, theta_apo;
	int K_loop;
	bool K_stage;
	//false = CSI/CDH, true = Boost + CSI/CDH
	bool K3;
	bool LandFlag = false;
	bool InsertionFlag = false;
	bool stop = false;
	std::vector<double> t_Abort_Table;
	std::vector<double> dV_Abort_Table;
	std::vector<double> Phase_Table;
	std::vector<double> A_ins_Table;
	int i = 0;

	K3 = false;
	w_M = 2.66169948e-6;
	t_UL = 7.9;
	Z_D_dot = 5650.0*0.3048;
	R_D_dot = 19.5*0.3048;
	mu = GGRAV * oapiGetMass(opt.sv_A.gravref);
	DH_D = 15.0*1852.0;
	V_H_min = 5515.0*0.3048;
	dt_CSI = opt.dt_CSI;
	dt_CAN = opt.dt_CAN;
	conopt.E = 26.6*RAD;
	conopt.GETbase = opt.GETbase;
	conopt.maneuver = 0;
	conopt.type = 0;
	conopt.t_TPI = opt.t_TPI;

	res.R_amin = length(opt.R_LS) + opt.h_amin;

	dt_abort = opt.dt_step;
	sv_I_guess = coast(opt.sv_A, opt.TLAND - OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase));
	if (!PDIIgnitionAlgorithm(sv_I_guess, opt.GETbase, opt.R_LS, opt.TLAND, opt.REFSMMAT, sv_IG, t_go, CR, U_FDP))
	{
		return false;
	}
	t_PDI = OrbMech::GETfromMJD(sv_IG.MJD, opt.GETbase);
	t_D = t_PDI - t_UL;
	sv_Abort = coast(sv_IG, -t_UL);
	t_stage = t_PDI + opt.dt_stage;

	Rot = OrbMech::GetRotationMatrix(sv_IG.gravref, sv_IG.MJD);
	WI = rhmul(Rot, _V(0, 0, 1));
	W = mul(opt.REFSMMAT, WI)*w_M;
	R_LSP = mul(opt.REFSMMAT, rhmul(Rot, opt.R_LS));
	descguid.Init(sv_IG.R, sv_IG.V, opt.sv_A.mass, t_PDI, opt.REFSMMAT, R_LSP, t_PDI, W);
	W_TD = opt.sv_A.mass;
	U_FDP_abort = tmul(opt.REFSMMAT, unit(U_FDP));

	res.DEDA225 = (length(R_LSP) + 60000.0*0.3048 + length(R_LSP) + opt.h_amin) / 2.0;
	res.DEDA226 = 7031200.0*0.3048;

	do
	{
		stop = false;

		integ.Init(U_FDP_abort);
		sv_D = sv_Abort;
		K_loop = 0;
		SLOPE = 2.0 / 1852.0*0.3048;

		do
		{
			if (LandFlag)
			{
				t_D = t_PDI + dt_abort;
				t_stage = 0.;
				break;
			}
			else
			{
				descguid.Guidance(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
				LandFlag = integ.Integration(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
				if (LandFlag)
				{
					t_D = t_PDI + dt_abort;
					t_stage = 0.;
					break;
				}
			}
		} while (t_D <= t_PDI + dt_abort);

		T = 43192.23;
		sv_Abort = sv_D;
		U_FDP_abort = U_FDP;

		sv_CSM_Abort = coast(opt.sv_P, t_D - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));
		WM = unit(crossp(sv_CSM_Abort.V, sv_CSM_Abort.R));
		TS = dotp(crossp(unit(sv_CSM_Abort.R), unit(sv_Abort.R)), WM);
		theta_D = OrbMech::sign(TS)*acos(dotp(unit(sv_CSM_Abort.R), unit(sv_Abort.R)));

		do
		{
			sv_D = sv_Abort;
			ascguid.Init(opt.sv_P.R, opt.sv_P.V, W_TD, length(opt.R_LS), Z_D_dot, R_D_dot, false);
			t = t_D;
			t_go = dt_abort;
			W_TA = W_TD;
			if (t >= t_stage || W_TA <= opt.W_TDRY)
			{
				t_go *= 2.0;
			}
			ascguid.SetTGO(t_go);
			K_stage = false;

			do
			{
				if (K_stage == false)
				{
					if (t >= t_stage || W_TA <= opt.W_TDRY)
					{
						K_stage = true;
						W_TA = opt.W_TAPS;
						ascguid.SetThrustParams(true);
					}
				}

				ascguid.Guidance(sv_D.R, sv_D.V, W_TA, t, U_FDP, t_go, T, isp);
				if (dotp(U_FDP, integ.GetCurrentTD()) < 0)
				{
					if (acos(dotp(integ.GetCurrentTD(), unit(sv_D.R))) > 30.0*RAD)
					{
						U_FDP = unit(sv_D.R);
					}
				}
				InsertionFlag = integ.Integration(sv_D.R, sv_D.V, W_TA, t, U_FDP, t_go, T, isp);
			} while (InsertionFlag == false);
			t_Ins = t;
			sv_LM_Ins = sv_D;
			sv_LM_Ins.mass = W_TA;
			sv_LM_Ins.MJD = OrbMech::MJDfromGET(t_Ins, opt.GETbase);

			//Overwrite actual insertion velocity with desired one; gives more consistent results
			Q_Xx = OrbMech::LVLH_Matrix(sv_LM_Ins.R, sv_LM_Ins.V);
			sv_LM_Ins.V = tmul(Q_Xx, _V(Z_D_dot, 0, -R_D_dot));

			sv_CSM_Ins = coast(opt.sv_P, t_Ins - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));
			WM = unit(crossp(sv_CSM_Ins.V, sv_CSM_Ins.R));
			TS = dotp(crossp(unit(sv_CSM_Ins.R), unit(sv_LM_Ins.R)), WM);
			theta = OrbMech::sign(TS)*acos(dotp(unit(sv_CSM_Ins.R), unit(sv_LM_Ins.R)));
			
			r_Ins = length(sv_D.R);
			A_Ins = mu * r_Ins / (2.0*mu - r_Ins * Z_D_dot*Z_D_dot);
			H_a = 2.0*A_Ins - r_Ins - length(opt.R_LS);
			R_a = 2.0*A_Ins - r_Ins;
			t_CSI = t_Ins + dt_CSI;

			if (K3 == false || dt_CAN <= 0.0 || opt.dv_CAN <= 0.0)
			{
				conopt.sv_A = sv_LM_Ins;
			}
			else
			{
				t_CAN = t_Ins + dt_CAN;
				sv_CAN = coast(sv_LM_Ins, t_CAN - t_Ins);
				sv_CAN_apo = sv_CAN;
				sv_CAN_apo.V += tmul(OrbMech::LVLH_Matrix(sv_CAN.R, sv_CAN.V), _V(opt.dv_CAN, 0.0, 0.0));
				conopt.sv_A = sv_CAN_apo;
			}

			conopt.sv_P = sv_CSM_Ins;
			conopt.t_TIG = t_CSI;

			ConcentricRendezvousProcessor(&conopt, conres);
			K_loop++;
			if (K_loop > 1)
			{
				SLOPE = dV_Inc / (conres.DH - dh_apo);
			}

			dV_Inc = SLOPE * (DH_D - conres.DH);
			Z_D_dot += dV_Inc;
			dh_apo = conres.DH;

		} while (abs(dV_Inc) > 0.1*0.3048 && K_loop < 10);

		if (opt.IsTwoSegment && R_a < res.R_amin)
		{
			if (K3 == false)
			{
				K_loop = 0;
				dt_CSI += opt.dt_2CSI;
				conopt.t_TPI += opt.dt_2TPI;
				res.Theta_LIM = theta_apo + (theta_D - theta_apo) / (R_a - R_a_apo)*(res.R_amin - R_a_apo);
				if (dt_CAN >= dt_CSI)
				{
					dt_CAN = 0.0;
				}

				OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.K1, res.J1);
				res.DEDA227 = res.K1;
				res.DEDA224 = res.J1;
				Phase_Table.clear();
				A_ins_Table.clear();
				K3 = true;
			}
			else
			{
				OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.K2, res.J2);
				stop = true;
			}
		}
		else
		{
			t_Abort_Table.push_back(dt_abort);
			dV_Abort_Table.push_back(Z_D_dot);
			Phase_Table.push_back(theta);
			A_ins_Table.push_back(A_Ins);
			theta_apo = theta_D;
			dt_abort += opt.dt_step;
			R_a_apo = R_a;
		}

		if (!opt.IsTwoSegment && Z_D_dot <= V_H_min)
		{
			stop = true;
		}

	} while (stop == false);

	//If we use the two segment logic, then everything has already been calculated
	if (opt.IsTwoSegment) return true;

	//Apollo 11: First 4 solutions from T vs. V_H table for cubic function
	if (dV_Abort_Table.size() < 4)
	{
		return false;
	}

	double t_Abort[4] = { t_Abort_Table[0], t_Abort_Table[1] , t_Abort_Table[2] , t_Abort_Table[3] };
	double dV_Abort[4] = { dV_Abort_Table[0], dV_Abort_Table[1] , dV_Abort_Table[2] , dV_Abort_Table[3] };
	double coeff[4];
	OrbMech::CubicInterpolation(t_Abort, dV_Abort, coeff);

	res.ABTCOF1 = coeff[0];
	res.ABTCOF2 = coeff[1];
	res.ABTCOF3 = coeff[2];
	res.ABTCOF4 = coeff[3];

	OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.DEDA227, res.DEDA224);

	return true;
}

bool RTCC::LunarLiftoffTimePredictionCFP(VECTOR3 R_LS, SV sv_P, double GETbase, OBJHANDLE hMoon, double dt_1, double h_1, double theta_1, double theta_Ins, double DH, double E, double t_L_guess, double t_TPI, double theta_F, LunarLiftoffResults &res)
{
	// NOMENCLATURE:
	// R = position vector, V = velocity vector
	// A = active vehicle, P = passive vehicle
	// 1 = Insertion, 2 = CSI, 3 = CDH, 4 = TPI (actual), 5 = TPI (LM, desired), 6 = TPI (CSM, desired), 7 = TPF (CSM and LM)
	SV sv_6;
	MATRIX3 Rot;
	VECTOR3 u, R_1, V_1, R_2, V_2, R_5, V_2F, R_3, V_3, R_L, U_L, R_3F, V_3F, R_PJ, V_PJ, R_P3S, V_P3S, R_4, V_4, V_4F, R_7, V_7, R_7F, V_7F;
	int n;
	double r_M, mu, r_A, dt_3, MJD_TPI, dt, MJD_L, t_3, r_Ins, dV_CSI;
	double dt_2, e_Ins, h_Ins, t_L, t_1, t_2, dt_4, e_P, eps, to_L, eo_P, c_F, p_C;
	int s_F;

	r_M = length(R_LS);
	mu = GGRAV * oapiGetMass(hMoon);
	MJD_TPI = GETbase + t_TPI / 24.0 / 3600.0;

	s_F = 0;
	c_F = p_C = 0.0;
	eps = 0.002*RAD;
	n = 0;
	dt = 100.0;
	t_L = t_L_guess;
	r_Ins = r_M + h_1;
	dV_CSI = 10.0*0.3048;

	sv_6 = coast(sv_P, (MJD_TPI - sv_P.MJD)*24.0*3600.0);
	u = unit(crossp(sv_6.R, sv_6.V));

	OrbMech::QDRTPI(sv_6.R, sv_6.V, MJD_TPI, hMoon, mu, DH, E, 0, R_PJ, V_PJ);
	R_5 = R_PJ - unit(R_PJ)*DH;
	r_A = length(R_5);

	e_Ins = (r_A - r_Ins) / (r_A + cos(theta_Ins)*r_Ins);
	h_Ins = sqrt(r_A*mu*(1.0 - e_Ins));
	res.v_LV = mu / h_Ins * e_Ins*sin(theta_Ins);
	res.v_LH = mu / h_Ins * (1.0 + e_Ins * cos(theta_Ins));

	do
	{
		//Launch to Insertion
		MJD_L = GETbase + t_L / 24.0 / 3600.0;
		Rot = OrbMech::GetRotationMatrix(hMoon, MJD_L);
		R_L = rhmul(Rot, R_LS);
		U_L = unit(R_L - u * dotp(u, R_L));
		R_1 = (U_L*cos(theta_1) + crossp(u, U_L)*sin(theta_1))*r_Ins;
		V_1 = unit(crossp(u, unit(R_1)))*res.v_LH + unit(R_1)*res.v_LV;
		t_1 = t_L + dt_1;
		//Insertion to CSI
		dt_2 = OrbMech::timetoapo(R_1, V_1, mu);
		OrbMech::rv_from_r0v0(R_1, V_1, dt_2, R_2, V_2, mu);
		t_2 = t_1 + dt_2;
		OrbMech::CSIToDH(R_2, V_2, sv_6.R, sv_6.V, DH, mu, dV_CSI);
		V_2F = V_2 + unit(crossp(u, R_2))*dV_CSI;
		//CSI to CDH
		OrbMech::REVUP(R_2, V_2F, 0.5, mu, R_3, V_3, dt_3);
		t_3 = t_2 + dt_3;
		OrbMech::RADUP(sv_6.R, sv_6.V, R_3, mu, R_P3S, V_P3S);
		OrbMech::COE(R_P3S, V_P3S, length(R_P3S) - length(R_3), mu, R_3F, V_3F);

		//CDH to TPI
		dt_4 = t_TPI - t_3;
		OrbMech::rv_from_r0v0(R_3, V_3F, dt_4, R_4, V_4, mu);

		e_P = OrbMech::sign(dotp(crossp(R_4, R_5), u))*acos(dotp(unit(R_5), unit(R_4)));
		if (abs(e_P) >= eps)
		{
			OrbMech::ITER(c_F, s_F, e_P, p_C, t_L, eo_P, to_L);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(e_P) >= eps);

	double dt_F;

	res.t_L = t_L;
	res.t_Ins = t_1;
	res.t_CSI = t_2;
	res.t_CDH = t_3;

	dt_F = OrbMech::time_theta(sv_6.R, sv_6.V, theta_F, mu);
	OrbMech::rv_from_r0v0(sv_6.R, sv_6.V, dt_F, R_7, V_7, mu);
	V_4F = OrbMech::elegant_lambert(R_4, V_4, R_7, dt_F, 0, false, mu);
	OrbMech::rv_from_r0v0(R_4, V_4F, dt_F, R_7F, V_7F, mu);

	res.t_TPF = t_TPI + dt_F;
	res.DV_CSI = length(V_2F - V_2);
	res.DV_CDH = length(V_3F - V_3);
	res.DV_TPI = length(V_4F - V_4);
	res.DV_TPF = length(V_7 - V_7F);
	res.DV_T = res.DV_CSI + res.DV_CDH + res.DV_TPI + res.DV_TPF;

	return true;
}

bool RTCC::LunarLiftoffTimePredictionTCDT(VECTOR3 R_LS, SV sv_P, double GETbase, OBJHANDLE hMoon, double dt_1, double h_1, double theta_1, double t_L_guess, LunarLiftoffResults &res)
{
	SV sv_PLI;
	MATRIX3 Rot;
	VECTOR3 R_1, V_1, R_PF, V_PF, U_L, R_L, R_AF, u, V_AF;
	double t_L, r_M, mu, r_Ins, r_A, dt, e_Ins, h_Ins, MJD_L, dt_2, t_1, c_F, p_C, eps, eo_P, to_L, e_P, t_2;
	int n, s_F;

	r_M = length(R_LS);
	mu = GGRAV * oapiGetMass(hMoon);
	t_L = t_L_guess;

	sv_PLI = coast(sv_P, t_L - OrbMech::GETfromMJD(sv_P.MJD, GETbase));

	r_Ins = r_M + h_1;
	r_A = r_M + 60.0*1852.0;
	e_Ins = (r_A - r_Ins) / (r_A + r_Ins);
	h_Ins = sqrt(r_A*mu*(1.0 - e_Ins));
	res.v_LH = mu / h_Ins * (1.0 + e_Ins);
	res.v_LV = 0.0;
	u = unit(crossp(sv_PLI.R, sv_PLI.V));

	s_F = 0;
	c_F = p_C = 0.0;
	eps = 0.002*RAD;
	dt = 100.0;
	n = 0;

	do
	{
		//Launch to Insertion
		MJD_L = GETbase + t_L / 24.0 / 3600.0;
		Rot = OrbMech::GetRotationMatrix(hMoon, MJD_L);
		R_L = rhmul(Rot, R_LS);
		U_L = unit(R_L - u * dotp(u, R_L));
		R_1 = (U_L*cos(theta_1) + crossp(u, U_L)*sin(theta_1))*r_Ins;
		V_1 = unit(crossp(u, unit(R_1)))*res.v_LH;
		t_1 = t_L + dt_1;
		//Insertion to TPF
		OrbMech::REVUP(R_1, V_1, 0.5, mu, R_AF, V_AF, dt_2);
		t_2 = t_1 + dt_2;
		OrbMech::rv_from_r0v0(sv_PLI.R, sv_PLI.V, (t_L - t_L_guess) + dt_1 + dt_2, R_PF, V_PF, mu);

		e_P = OrbMech::sign(dotp(crossp(R_AF, R_PF), u))*acos(dotp(unit(R_PF), unit(R_AF)));
		if (abs(e_P) >= eps)
		{
			OrbMech::ITER(c_F, s_F, e_P, p_C, t_L, eo_P, to_L);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(e_P) >= eps);

	res.t_L = t_L;
	res.t_Ins = t_L + dt_1;
	res.t_TPF = t_L + dt_1 + dt_2;
	res.DV_CSI = 0.0;
	res.DV_CDH = 0.0;
	res.DV_TPI = 0.0;
	res.DV_TPF = length(V_PF - V_AF);
	res.DV_T = res.DV_TPF;

	return true;
}

bool RTCC::LunarLiftoffTimePredictionDT(VECTOR3 R_LS, SV sv_P, double GETbase, OBJHANDLE hMoon, double dt_1, double h_1, double theta_1, double dt_2, double DH, double E, double t_L_guess, double theta_F, LunarLiftoffResults &res)
{
	MATRIX3 Rot;
	VECTOR3 R_1, V_1, R_2, V_2, R_6, V_6, R_5, R_L, U_L, R_PJ, V_PJ, u, R_7, V_7, V_2F, R_7F, V_7F;
	int n;
	double r_M, mu, r_A, MJD_TPI, dt, MJD_L, t_1, t_2, sw, theta_u, r_Ins, dt_F;
	double t_L, v_LHo, eps1, e_T, p_I, c_I, e_To;
	int s_F;

	r_M = length(R_LS);
	mu = GGRAV * oapiGetMass(hMoon);

	n = 0;
	dt = 100.0;
	r_Ins = r_M + h_1;
	t_L = t_L_guess;

	res.v_LV = 32.0*0.3048;
	res.v_LH = 5525.0*0.3048;

	eps1 = 0.01;

	while (n < 10 && abs(dt)>0.5)
	{
		res.t_TPI = t_L + dt_1 + dt_2;
		MJD_TPI = GETbase + res.t_TPI / 24.0 / 3600.0;
		OrbMech::oneclickcoast(sv_P.R, sv_P.V, sv_P.MJD, (MJD_TPI - sv_P.MJD)*24.0*3600.0, R_6, V_6, hMoon, hMoon);
		OrbMech::QDRTPI(R_6, V_6, MJD_TPI, hMoon, mu, DH, E, 1, R_PJ, V_PJ);
		R_5 = R_PJ - unit(R_PJ)*DH;
		r_A = length(R_5);
		u = unit(crossp(R_6, V_6));

		//Launch to Insertion
		MJD_L = GETbase + t_L / 24.0 / 3600.0;
		Rot = OrbMech::GetRotationMatrix(hMoon, MJD_L);
		R_L = rhmul(Rot, R_LS);
		U_L = unit(R_L - u * dotp(u, R_L));
		R_1 = (U_L*cos(theta_1) + crossp(u, U_L)*sin(theta_1))*r_Ins;

		p_I = c_I = 0.0;
		s_F = 0;

		do
		{
			V_1 = unit(crossp(u, unit(R_1)))*res.v_LH + unit(R_1)*res.v_LV;
			OrbMech::rv_from_r0v0(R_1, V_1, dt_2, R_2, V_2, mu);

			e_T = r_A - length(R_2);

			if (abs(e_T) >= eps1)
			{
				OrbMech::ITER(c_I, s_F, e_T, p_I, res.v_LH, e_To, v_LHo);
				if (s_F == 1)
				{
					return false;
				}
			}

		} while (abs(e_T) >= eps1);

		
		t_1 = t_L + dt_1;
		//Insertion to TPI
		OrbMech::rv_from_r0v0(R_1, V_1, dt_2, R_2, V_2, mu);
		t_2 = t_1 + dt_2;

		sw = OrbMech::sign(dotp(u, crossp(R_2, R_5)));
		theta_u = sw * acos(dotp(unit(R_2), unit(R_5))) + PI * (1.0 - sw);
		dt = OrbMech::time_theta(R_6, V_6, theta_u, mu);
		t_L -= dt;
		n++;
	}

	res.t_L = t_L;
	res.t_Ins = t_L + dt_1;
	res.t_TPI = t_L + dt_1 + dt_2;

	dt_F = OrbMech::time_theta(R_6, V_6, theta_F, mu);
	OrbMech::rv_from_r0v0(R_6, V_6, dt_F, R_7, V_7, mu);
	V_2F = OrbMech::elegant_lambert(R_2, V_2, R_7, dt_F, 0, false, mu);
	OrbMech::rv_from_r0v0(R_2, V_2F, dt_F, R_7F, V_7F, mu);

	res.t_TPF = res.t_TPI + dt_F;
	res.DV_CDH = 0.0;
	res.DV_CSI = 0.0;
	res.DV_TPI = length(V_2F - V_2);
	res.DV_TPF = length(V_7F - V_7);
	res.DV_T = res.DV_TPI + res.DV_TPF;

	return true;
}

double RTCC::GetSemiMajorAxis(SV sv)
{
	double mu = GGRAV * oapiGetMass(sv.gravref);
	double eps = length(sv.V)*length(sv.V) / 2.0 - mu / (length(sv.R));
	return -mu / (2.0*eps);
}

void RTCC::papiWriteScenario_SV(FILEHANDLE scn, char *item, SV sv)
{

	char buffer[256], name[16];

	if (sv.gravref)
	{
		oapiGetObjectName(sv.gravref, name, 16);
	}
	else
	{
		sprintf(name, "None");
	}

	sprintf(buffer, "  %s %s %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf", item, name, sv.mass, sv.MJD, sv.R.x, sv.R.y, sv.R.z, sv.V.x, sv.V.y, sv.V.z);
	oapiWriteLine(scn, buffer);
}

bool RTCC::papiReadScenario_SV(char *line, char *item, SV &sv)
{

	char buffer[256], name[16];
	SV v;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			if (sscanf(line, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf", buffer, name, &v.mass, &v.MJD, &v.R.x, &v.R.y, &v.R.z, &v.V.x, &v.V.y, &v.V.z) == 10) {
				v.gravref = oapiGetObjectByName(name);
				sv = v;
				return true;
			}
		}
	}
	return false;
}

VECTOR3 RTCC::RLS_from_latlng(double lat, double lng, double alt)
{
	double R_M = oapiGetSize(oapiGetObjectByName("Moon"));
	return OrbMech::r_from_latlong(lat, lng, R_M + alt);
}

void RTCC::FIDOOrbitDigitalsUpdate(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res)
{
	SV sv_p, sv_cc;
	OELEMENTS coe;
	MATRIX3 Rot;
	VECTOR3 R_equ, V_equ;
	double lat, lng, mu, a, R_B, lng_cc, MJD_cc;

	//sprintf(res.VEHID, opt.v->GetName());
	res.K = 0.0;
	res.GETID = OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase);
	oapiGetObjectName(opt.sv_A.gravref, res.REF, 64);

	R_B = oapiGetSize(opt.sv_A.gravref);
	mu = GGRAV * oapiGetMass(opt.sv_A.gravref);
	Rot = OrbMech::GetRotationMatrix(opt.sv_A.gravref, opt.sv_A.MJD);
	R_equ = rhtmul(Rot, opt.sv_A.R);
	V_equ = rhtmul(Rot, opt.sv_A.V);

	coe = OrbMech::coe_from_sv(R_equ, V_equ, mu);
	OrbMech::latlong_from_r(R_equ, lat, lng);
	res.PPP = lat * DEG;
	res.LPP = lng * DEG;
	res.TAPP = coe.TA*DEG;
	res.H = (length(opt.sv_A.R) - R_B) / 1852.0;
	res.V = length(opt.sv_A.V) / 0.3048;
	res.GAM = atan2(coe.e*sin(coe.TA), 1.0 + coe.e*cos(coe.TA))*DEG;
	a = coe.h*coe.h / mu / (1.0 - coe.e*coe.e);
	res.A = a / 1852.0;
	res.E = coe.e;
	res.I = coe.i*DEG;

	if (coe.e > 1.0)
	{
		double dt;

		dt = OrbMech::timetoperi_integ(opt.sv_A.R, opt.sv_A.V, opt.sv_A.MJD, opt.sv_A.gravref, opt.sv_A.gravref, sv_p.R, sv_p.V);
		sv_p.gravref = opt.sv_A.gravref;
		sv_p.mass = opt.sv_A.mass;
		sv_p.MJD = opt.sv_A.MJD + dt / 24.0 / 3600.0;
	}
	else
	{
		SV sv_a;
		VECTOR3 Rlat, Vlat;
		double dt;

		ApsidesDeterminationSubroutine(opt.sv_A, sv_a, sv_p);
		OrbMech::latlong_from_J2000(sv_a.R, sv_a.MJD, opt.sv_A.gravref, lat, lng);
		res.HA = (length(sv_a.R) - R_B) / 1852.0;
		res.PA = lat * DEG;
		res.LA = lng * DEG;
		res.GETA = OrbMech::GETfromMJD(sv_a.MJD, opt.GETbase);

		res.TO = OrbMech::period(opt.sv_A.R, opt.sv_A.V, mu);
		dt = OrbMech::findlatitude(opt.sv_A.R, opt.sv_A.V, opt.sv_A.MJD, opt.sv_A.gravref, 0.0, true, Rlat, Vlat);
		OrbMech::latlong_from_J2000(Rlat, opt.sv_A.MJD + dt / 24.0 / 3600.0, opt.sv_A.gravref, lat, lng);
		res.LNPP = lng * DEG;
	}

	OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, opt.sv_A.gravref, lat, lng);
	res.HP = (length(sv_p.R) - R_B) / 1852.0;
	res.PP = lat * DEG;
	res.LP = lng * DEG;
	res.GETP = OrbMech::GETfromMJD(sv_p.MJD, opt.GETbase);

	res.ORBWT = opt.sv_A.mass / 0.45359237;

	if (opt.sv_A.gravref == oapiGetObjectByName("Earth"))
	{
		lng_cc = -80.0*RAD;
	}
	else
	{
		lng_cc = -180.0*RAD;
	}

	MJD_cc = OrbMech::P29TimeOfLongitude(opt.sv_A.R, opt.sv_A.V, opt.sv_A.MJD, opt.sv_A.gravref, lng_cc);
	sv_cc = GeneralTrajectoryPropagation(opt.sv_A, 0, MJD_cc);
	res.GETCC = OrbMech::GETfromMJD(sv_cc.MJD, opt.GETbase);
}

void RTCC::FIDOOrbitDigitalsCycle(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res)
{
	SV sv_cur, sv_p, sv_cc;
	OELEMENTS coe;
	MATRIX3 Rot;
	VECTOR3 R_equ, V_equ;
	double lat, lng, mu, a, R_B, lng_cc, MJD_cc;

	sv_cur = GeneralTrajectoryPropagation(opt.sv_A, 0, opt.MJD);

	res.K = 0.0;
	res.GET = OrbMech::GETfromMJD(sv_cur.MJD, opt.GETbase);
	oapiGetObjectName(sv_cur.gravref, res.REF, 64);

	R_B = oapiGetSize(sv_cur.gravref);
	mu = GGRAV * oapiGetMass(sv_cur.gravref);
	Rot = OrbMech::GetRotationMatrix(sv_cur.gravref, sv_cur.MJD);
	R_equ = rhtmul(Rot, sv_cur.R);
	V_equ = rhtmul(Rot, sv_cur.V);

	coe = OrbMech::coe_from_sv(R_equ, V_equ, mu);
	OrbMech::latlong_from_r(R_equ, lat, lng);

	if (lat > 0 && res.PPP < 0)
	{
		VECTOR3 Rlat, Vlat;
		double dt;

		dt = OrbMech::findlatitude(sv_cur.R, sv_cur.V, sv_cur.MJD, sv_cur.gravref, 0.0, true, Rlat, Vlat);
		OrbMech::latlong_from_J2000(Rlat, sv_cur.MJD + dt / 24.0 / 3600.0, sv_cur.gravref, lat, lng);
		res.LNPP = lng * DEG;
	}

	res.PPP = lat * DEG;
	res.LPP = lng * DEG;
	res.TAPP = coe.TA*DEG;
	res.H = (length(sv_cur.R) - R_B) / 1852.0;
	res.V = length(sv_cur.V) / 0.3048;
	res.GAM = atan2(coe.e*sin(coe.TA), 1.0 + coe.e*cos(coe.TA))*DEG;
	a = coe.h*coe.h / mu / (1.0 - coe.e*coe.e);
	res.A = a / 1852.0;
	res.E = coe.e;
	res.I = coe.i*DEG;

	if (sv_cur.gravref == oapiGetObjectByName("Earth"))
	{
		lng_cc = -80.0*RAD;
	}
	else
	{
		lng_cc = -180.0*RAD;
	}

	MJD_cc = OrbMech::P29TimeOfLongitude(sv_cur.R, sv_cur.V, sv_cur.MJD, sv_cur.gravref, lng_cc);
	sv_cc = GeneralTrajectoryPropagation(sv_cur, 0, MJD_cc);
	res.GETCC = OrbMech::GETfromMJD(sv_cc.MJD, opt.GETbase);

	if (coe.e < 1.0)
	{
		res.TO = OrbMech::period(sv_cur.R, sv_cur.V, mu);
	}
}

void RTCC::FIDOOrbitDigitalsApsidesCycle(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res)
{
	OELEMENTS coe;
	SV sv_cur, sv_p;
	MATRIX3 Rot;
	VECTOR3 R_equ, V_equ;
	double R_B, mu, lat, lng;

	sv_cur = GeneralTrajectoryPropagation(opt.sv_A, 0, opt.MJD);
	R_B = oapiGetSize(sv_cur.gravref);
	mu = GGRAV * oapiGetMass(sv_cur.gravref);
	Rot = OrbMech::GetRotationMatrix(sv_cur.gravref, sv_cur.MJD);
	R_equ = rhtmul(Rot, sv_cur.R);
	V_equ = rhtmul(Rot, sv_cur.V);

	coe = OrbMech::coe_from_sv(R_equ, V_equ, mu);

	if (coe.e > 1.0)
	{
		double dt;

		dt = OrbMech::timetoperi_integ(sv_cur.R, sv_cur.V, sv_cur.MJD, sv_cur.gravref, sv_cur.gravref, sv_p.R, sv_p.V);
		sv_p.gravref = sv_cur.gravref;
		sv_p.mass = sv_cur.mass;
		sv_p.MJD = sv_cur.MJD + dt / 24.0 / 3600.0;
	}
	else
	{
		SV sv_a;

		ApsidesDeterminationSubroutine(sv_cur, sv_a, sv_p);
		OrbMech::latlong_from_J2000(sv_a.R, sv_a.MJD, sv_cur.gravref, lat, lng);
		res.HA = (length(sv_a.R) - R_B) / 1852.0;
		res.PA = lat * DEG;
		res.LA = lng * DEG;
		res.GETA = OrbMech::GETfromMJD(sv_a.MJD, opt.GETbase);
	}

	OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_cur.gravref, lat, lng);
	res.HP = (length(sv_p.R) - R_B) / 1852.0;
	res.PP = lat * DEG;
	res.LP = lng * DEG;
	res.GETP = OrbMech::GETfromMJD(sv_p.MJD, opt.GETbase);
}

void RTCC::FIDOOrbitDigitalsCalculateLongitude(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res)
{
	SV sv_L;
	double lat, lng;

	sv_L = coast(opt.sv_A, res.GETL - OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase));
	OrbMech::latlong_from_J2000(sv_L.R, sv_L.MJD, sv_L.gravref, lat, lng);
	res.L = lng * DEG;
}

void RTCC::FIDOOrbitDigitalsCalculateGETL(const FIDOOrbitDigitalsOpt &opt, FIDOOrbitDigitals &res)
{
	SV sv_cur;
	double MJD_L;

	sv_cur = GeneralTrajectoryPropagation(opt.sv_A, 0, opt.MJD);
	MJD_L = OrbMech::P29TimeOfLongitude(sv_cur.R, sv_cur.V, sv_cur.MJD, sv_cur.gravref, res.L*RAD);
	res.GETL = OrbMech::GETfromMJD(MJD_L, opt.GETbase);
}

void RTCC::FIDOSpaceDigitalsUpdate(const SpaceDigitalsOpt &opt, SpaceDigitals &res)
{
	//Determine phase: 0 = Earth or lunar low orbit, 1 = TLC in Earth SOI, 2 = TLC in Moon SOI, 3 = TEC in Moon SOI, 4 = TEC in Earth SOI

	OBJHANDLE hEarth, hMoon;
	OELEMENTS coe;
	MATRIX3 Rot;
	VECTOR3 R_SI, V_SI, R_equ, V_equ;
	double mu, v_r, r_SPH, dt_SI, MJD_SI, intpart, lat, lng, r_B, rmag, vmag, fpav, az, mu_E, mu_M;
	int phase;

	r_SPH = 64373760.0;
	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV * oapiGetMass(opt.sv_A.gravref);
	mu_E = GGRAV * oapiGetMass(hEarth);
	mu_M = GGRAV * oapiGetMass(hMoon);

	if (opt.sv_A.gravref == hEarth)
	{
		r_B = oapiGetSize(hEarth);
	}
	else
	{
		r_B = oapiGetSize(hMoon) + opt.LSAlt;
	}

	Rot = OrbMech::GetRotationMatrix(opt.sv_A.gravref, opt.sv_A.MJD);
	R_equ = rhtmul(Rot, opt.sv_A.R);
	V_equ = rhtmul(Rot, opt.sv_A.V);
	coe = OrbMech::coe_from_sv(R_equ, V_equ, mu);
	OrbMech::rv_from_adbar(R_equ, V_equ, rmag, vmag, lng, lat, fpav, az);

	res.WEIGHT = opt.sv_A.mass / 0.45359237;
	res.GMTV = modf(opt.sv_A.MJD, &intpart)*24.0*3600.0;
	res.GETV = OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase);

	oapiGetObjectName(opt.sv_A.gravref, res.REF, 64);
	res.GET = res.GETV;
	res.V = vmag / 0.3048;
	res.PHI = lat * DEG;
	res.LAM = lng * DEG;
	res.H = (rmag - r_B) / 1852.0;
	res.ADA = coe.TA*DEG;
	res.GAM = 90.0 - fpav * DEG;
	res.PSI = az * DEG;

	if (opt.sv_A.gravref == hEarth && coe.e < 0.9)
	{
		//phase = 0
		return;
	}
	else if (opt.sv_A.gravref == hMoon && coe.e < 1.0)
	{
		//phase = 0
		return;
	}

	VECTOR3 R_CA, V_CA, R_SE, V_SE, R_VP, V_VP, R_EI, V_EI;
	double r_M, r_E, dt_CA, MJD_CA, dt_SE, MJD_SE, dt_VP, MJD_VP, r_EI, dt_EI, MJD_EI;
	
	r_M = oapiGetSize(hMoon) + opt.LSAlt;
	r_E = oapiGetSize(hEarth);
	r_EI = r_E + 400000.0*0.3048;

	v_r = dotp(opt.sv_A.R, opt.sv_A.V) / length(opt.sv_A.R);

	if (opt.sv_A.gravref == hEarth)
	{
		if (v_r > 0)
		{
			VECTOR3 Rtemp, Vtemp;
			OBJHANDLE gravout = NULL;
			//Check for SOI change within the next 100 hours
			if (OrbMech::oneclickcoast(opt.sv_A.R, opt.sv_A.V, opt.sv_A.MJD, 100.0*3600.0, Rtemp, Vtemp, opt.sv_A.gravref, gravout))
			{
				phase = 1;
			}
			else
			{
				phase = 4;
			}
		}
		else
		{
			phase = 4;
		}
	}
	else
	{
		if (v_r < 0)
		{
			phase = 2;
		}
		else
		{
			phase = 3;
		}
	}

	if (phase <= 1)
	{
		dt_SI = OrbMech::time_radius_integ(opt.sv_A.R, opt.sv_A.V, opt.sv_A.MJD, r_SPH, -1.0, opt.sv_A.gravref, hMoon, R_SI, V_SI);
		MJD_SI = opt.sv_A.MJD + dt_SI / 24.0 / 3600.0;
		res.GETSI = OrbMech::GETfromMJD(MJD_SI, opt.GETbase);
	}
	else
	{
		R_SI = opt.sv_A.R;
		V_SI = opt.sv_A.V;
		MJD_SI = opt.sv_A.MJD;
	}

	if (phase <= 2)
	{
		dt_CA = OrbMech::timetoperi_integ(R_SI, V_SI, MJD_SI, hMoon, hMoon, R_CA, V_CA);
		MJD_CA = MJD_SI + dt_CA / 24.0 / 3600.0;
		res.GETCA = OrbMech::GETfromMJD(MJD_CA, opt.GETbase);

		Rot = OrbMech::GetRotationMatrix(hMoon, MJD_CA);
		R_equ = rhtmul(Rot, R_CA);
		V_equ = rhtmul(Rot, V_CA);
		coe = OrbMech::coe_from_sv(R_equ, V_equ, mu_M);
		OrbMech::rv_from_adbar(R_equ, V_equ, rmag, vmag, lng, lat, fpav, az);

		res.VCA = vmag / 0.3048;
		res.HCA = (rmag - r_M) / 1852.0;
		res.PCA = lat * DEG;
		res.LCA = lng * DEG;
		res.PSICA = az * DEG;

		VECTOR3 u_LAH, R_LPO, V_LPO, H_LPO, axis, u_LPO, u_node, R_MN, V_MN;
		double r_LPO, v_LPO, MJD_LAND, dt_MN, MJD_MN, DMN;

		MJD_LAND = opt.GETbase + opt.t_land / 24.0 / 3600.0;
		Rot = OrbMech::GetRotationMatrix(hMoon, MJD_LAND);

		u_LAH = unit(crossp(R_CA, V_CA));
		r_LPO = r_M + 60.0*1852.0;
		v_LPO = sqrt(mu / r_LPO);

		OrbMech::adbar_from_rv(r_LPO, v_LPO, opt.LSLng, opt.LSLat, PI05, opt.LSAzi, R_LPO, V_LPO);
		H_LPO = crossp(R_LPO, V_LPO);
		axis = unit(H_LPO);
		u_LPO = unit(rhmul(Rot, axis));

		//Node unit vector
		u_node = unit(crossp(u_LPO, u_LAH));

		//Periapsis vector and node should be within 90°, if not the actual intersection is in the other direction
		if (dotp(u_node, unit(R_CA)) < 0.0)
		{
			u_node = -u_node;
		}

		//state vector at the node
		dt_MN = OrbMech::timetonode_integ(R_CA, V_CA, MJD_CA, hMoon, u_node, R_MN, V_MN);
		MJD_MN = MJD_CA + dt_MN / 24.0 / 3600.0;
		res.GETMN = OrbMech::GETfromMJD(MJD_MN, opt.GETbase);

		Rot = OrbMech::GetRotationMatrix(hMoon, MJD_MN);
		R_equ = rhtmul(Rot, R_MN);
		V_equ = rhtmul(Rot, V_MN);
		coe = OrbMech::coe_from_sv(R_equ, V_equ, mu_M);
		OrbMech::rv_from_adbar(R_equ, V_equ, rmag, vmag, lng, lat, fpav, az);

		res.HMN = (rmag - r_M) / 1852.0;
		res.PMN = lat * DEG;
		res.LMN = lng * DEG;

		DMN = acos(dotp(u_LPO, u_LAH));
		if (u_LPO.z > u_LAH.z)
		{
			DMN = -DMN;
		}
		res.DMN = DMN * DEG;
	}
	else
	{
		R_CA = R_SI;
		V_CA = V_SI;
		MJD_CA = MJD_SI;
	}

	if (phase <= 3)
	{
		dt_SE = OrbMech::time_radius_integ(R_CA, V_CA, MJD_CA, r_SPH, 1.0, hMoon, hMoon, R_SE, V_SE);
		MJD_SE = MJD_CA + dt_SE / 24.0 / 3600.0;
		res.GETSE = OrbMech::GETfromMJD(MJD_SE, opt.GETbase);

		dt_VP = OrbMech::timetoperi_integ(R_SE, V_SE, MJD_SE, hMoon, hEarth, R_VP, V_VP);
	}
	else
	{
		R_SE = R_CA;
		V_SE = V_CA;
		MJD_SE = MJD_CA;

		dt_VP = OrbMech::timetoperi_integ(R_SE, V_SE, MJD_SE, hEarth, hEarth, R_VP, V_VP);
	}

	MJD_VP = MJD_SE + dt_VP / 24.0 / 3600.0;
	res.GETVP = OrbMech::GETfromMJD(MJD_VP, opt.GETbase);
	Rot = OrbMech::GetRotationMatrix(hEarth, MJD_VP);
	R_equ = rhtmul(Rot, R_VP);
	V_equ = rhtmul(Rot, V_VP);
	coe = OrbMech::coe_from_sv(R_equ, V_equ, mu_E);
	OrbMech::rv_from_adbar(R_equ, V_equ, rmag, vmag, lng, lat, fpav, az);

	res.VVP = vmag / 0.3048;
	res.HVP = (rmag - r_E) / 1852.0;
	res.PVP = lat * DEG;
	res.LVP = lng * DEG;
	res.PSIVP = az * DEG;

	res.IE = coe.i*DEG;
	res.LN = coe.RA*DEG;

	if (rmag < r_EI)
	{
		VECTOR3 R_INT, V_INT;
		double dt_INT, MJD_INT;

		dt_INT = -3600.0;
		OrbMech::oneclickcoast(R_VP, V_VP, MJD_VP, dt_INT, R_INT, V_INT, hEarth, hEarth);
		MJD_INT = MJD_VP + dt_INT / 24.0 / 3600.0;

		dt_EI = OrbMech::time_radius_integ(R_INT, V_INT, MJD_INT, r_EI, -1.0, hEarth, hEarth, R_EI, V_EI);
		MJD_EI = MJD_INT + dt_EI / 24.0 / 3600.0;
		res.GETEI = OrbMech::GETfromMJD(MJD_EI, opt.GETbase);
		Rot = OrbMech::GetRotationMatrix(hEarth, MJD_EI);
		R_equ = rhtmul(Rot, R_EI);
		V_equ = rhtmul(Rot, V_EI);
		coe = OrbMech::coe_from_sv(R_equ, V_equ, mu_E);
		OrbMech::rv_from_adbar(R_equ, V_equ, rmag, vmag, lng, lat, fpav, az);

		res.VEI = vmag / 0.3048;
		res.GEI = 90.0 - fpav * DEG;
		res.PEI = lat * DEG;
		res.LEI = lng * DEG;
		res.PSIEI = az * DEG;
	}
}

void RTCC::FIDOSpaceDigitalsCycle(const SpaceDigitalsOpt &opt, SpaceDigitals &res)
{
	SV sv_cur;
	OELEMENTS coe;
	OBJHANDLE hEarth, hMoon;
	MATRIX3 Rot;
	VECTOR3 R_equ, V_equ;
	double mu, r_B, rmag, vmag, lng, lat, fpav, az;

	sv_cur = GeneralTrajectoryPropagation(opt.sv_A, 0, opt.MJD);

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV * oapiGetMass(sv_cur.gravref);

	if (sv_cur.gravref == hEarth)
	{
		r_B = oapiGetSize(hEarth);
	}
	else
	{
		r_B = oapiGetSize(hMoon) + opt.LSAlt;
	}

	Rot = OrbMech::GetRotationMatrix(sv_cur.gravref, sv_cur.MJD);
	R_equ = rhtmul(Rot, sv_cur.R);
	V_equ = rhtmul(Rot, sv_cur.V);
	coe = OrbMech::coe_from_sv(R_equ, V_equ, mu);
	OrbMech::rv_from_adbar(R_equ, V_equ, rmag, vmag, lng, lat, fpav, az);

	oapiGetObjectName(sv_cur.gravref, res.REF, 64);
	res.GET = OrbMech::GETfromMJD(opt.MJD, opt.GETbase);
	res.V = vmag / 0.3048;
	res.PHI = lat * DEG;
	res.LAM = lng * DEG;
	res.H = (rmag - r_B) / 1852.0;
	res.ADA = coe.TA*DEG;
	res.GAM = 90.0 - fpav * DEG;
	res.PSI = az * DEG;
}

void RTCC::FIDOSpaceDigitalsGET(const SpaceDigitalsOpt &opt, SpaceDigitals &res)
{
	OBJHANDLE hEarth, hMoon;
	OELEMENTS coe;
	SV sv_cur;
	MATRIX3 Rot, EMPMat;
	VECTOR3 R_equ, V_equ, R_EMP, V_EMP, H_EMP;
	double r_B, mu, apo, peri, rmag, vmag, lng, lat, fpav, az, r_S, a;

	sv_cur = GeneralTrajectoryPropagation(opt.sv_A, 0, opt.MJD);

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	mu = GGRAV * oapiGetMass(sv_cur.gravref);

	oapiGetObjectName(sv_cur.gravref, res.REF1, 64);

	if (sv_cur.gravref == hEarth)
	{
		r_B = r_S = oapiGetSize(hEarth);
	}
	else
	{
		r_S = oapiGetSize(hMoon);
		r_B = r_S + opt.LSAlt;
	}

	OrbMech::periapo(sv_cur.R, sv_cur.V, mu, apo, peri);

	res.HA = (apo - r_B) / 1852.0;
	if (res.HA > 99999.9)
	{
		res.HA = 99999.9;
	}
	res.HP = (peri - r_B) / 1852.0;

	Rot = OrbMech::GetRotationMatrix(sv_cur.gravref, sv_cur.MJD);
	R_equ = rhtmul(Rot, sv_cur.R);
	V_equ = rhtmul(Rot, sv_cur.V);
	coe = OrbMech::coe_from_sv(R_equ, V_equ, mu);
	OrbMech::rv_from_adbar(R_equ, V_equ, rmag, vmag, lng, lat, fpav, az);

	res.H1 = (rmag - r_B) / 1852.0;
	res.V1 = vmag / 0.3048;
	res.GAM1 = 90.0 - fpav * DEG;
	res.PSI1 = az * DEG;
	res.PHIO = res.PHI1 = lat * DEG;
	res.LAM1 = lng * DEG;
	res.HS = res.HO = (rmag - r_S) / 1852.0;

	res.W1 = coe.w*DEG;
	res.OMG = coe.RA*DEG;
	res.PRA = fmod(coe.RA + coe.w, PI2)*DEG;
	a = coe.h*coe.h / mu / (1.0 - coe.e*coe.e);
	res.A1 = a / 1852.0;
	res.L1 = fmod(coe.TA + coe.w, PI2)*DEG;
	res.E1 = coe.e;
	res.I1 = coe.i*DEG;

	EMPMat = OrbMech::EMPMatrix(sv_cur.MJD);
	R_EMP = mul(EMPMat, sv_cur.R);
	V_EMP = mul(EMPMat, sv_cur.V);
	H_EMP = crossp(R_EMP, V_EMP);
	res.IEMP = acos(H_EMP.z / length(H_EMP))*DEG;
}